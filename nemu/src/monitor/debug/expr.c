#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ

  /* TODO: Add more token types */
	, TK_DECIMAL
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
	{"\\(", '('},					// left bracket
	{"\\)", ')'},					// right bracket
	{"[0-9]+u?", TK_DECIMAL}, // decimal
	{"\\*", '*'},					// multiply
	{"/", '/'},						// divide
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
	{"\\-", '-'},					// minus
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

#define MAX_TOKEN_NUM 1024
static Token tokens[MAX_TOKEN_NUM] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

uint32_t eval(int , int, bool *);

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
	
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
				// dimiss the space
				if (rules[i].token_type == TK_NOTYPE) {
					break;
				}
				Assert(nr_token < MAX_TOKEN_NUM, "Too many tokens");
				tokens[nr_token].type = rules[i].token_type;
        switch (rules[i].token_type) {
					case TK_DECIMAL:
						Assert(substr_len <= 31, "Length of decimal is too long");
						strncpy(tokens[nr_token].str, substr_start, substr_len);

						// add '\0' to terminate the string
						tokens[nr_token].str[substr_len] = '\0';
						break;
          default: break; 
        }
				nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
	
  /* TODO: Insert codes to evaluate the expression. */
	uint32_t ans = eval(0, nr_token-1, success);
	if (*success) {
		return ans;
	} else {
		return 0;
	}
}

bool check_parenthenses(int p, int q) {
	// Assert(q <= nr_token, "check_parenthenses: q > nr_token");
	// Assert(p >= 0, "check_parenthenses: p < 0");
	if (tokens[p].type == '(' && tokens[q].type == ')') {
		int brkt = 0;
		for (int i = p+1; i < q; i++) {
			if (tokens[i].type == '(') {
				brkt++;
			} else if (tokens[i].type == ')') {
				if (brkt > 0) brkt--;
				else return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

// check the tokens[p] is a unary operator
// @input: the index of tokens
// @return: if it's unary operator return true, or return false
bool check_unaryop(int p) {
	Assert(p >= 0 && p < nr_token, "Check_unaryop, not in range");
	return (tokens[p].type == '+' || tokens[p].type == '-' || tokens[p].type == '*') && (
					p == 0 || (tokens[p-1].type != ')' && tokens[p-1].type != TK_DECIMAL));
}

// evaluate a sub expression between p and q, which are the index of tokens.
// @input: start pointer p and end pointer q, success is a pointer that indicate whether the eval is sucess
// @output: success indicate the result of eval
// @return: the result evaluated from the expression
uint32_t eval(int p, int q, bool *success) {
	if (p > q) {
		Log("Bad expression: p > q");
		*success = false;
		return 0;
	} else if (p == q) { // single token
		/* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
		if (tokens[p].type != TK_DECIMAL) {
			Log("Bad expression: decimal type error");
			*success = false;
			return 0;
		}
		uint64_t d;
		if (sscanf(tokens[p].str, "%ld", &d) != 1) {
			Log("Bad expression: decimal number error");
			*success = false;
			return 0;
		}
		uint32_t d32 = (uint32_t) d;
		*success = true;
		return d32;
	} else if (check_parenthenses(p, q) == true) {
		/* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
		return eval(p+1, q-1, success);
	} else if (p + 1 == q) {
		// if there are only two tokens and the first token is one of the {'+', '-'},
		// and the second token is a decimal
		int type_f = tokens[p].type;
		int type_s = tokens[q].type;
		if ((type_f == '+' || type_f == '-') && type_s == TK_DECIMAL) {
			uint32_t ans = eval(q, q, success);
			if (!*success) {
				return 0;
			}
			return (type_f == '+')?ans:-ans;
		} else {
			Log("Bad expression, invalid expression in (%d, %s)", type_f, tokens[q].str);
			*success = false;
			return 0;
		}
	
	} else {
		// check the validation
		if (tokens[p].type == ')' || tokens[q].type == '(') {
			Log("Bad expression: bracket mismatch");
			*success = false;
			return 0;
		}
		
		// find the pviot oprand
		int i;
		int inbrkt = 0;
		int pivot = -1;
		int grade = 0;
		for (i = p; i <= q; i++) {
			if (tokens[i].type == '(') {
				inbrkt++;
			} else if (tokens[i].type == ')') {
				inbrkt--;
				// find the bracket mismatch
				if (inbrkt < 0) {
					Log("Bad expression, bracket mismatch");
					*success = false;
					return 0;
				}
			}
			// if the oprand is in the bracket, skip
			if (inbrkt > 0) continue;
			
			// if it's a unary operator, skip
			if (check_unaryop(i)) continue;
				
			// if the oprand is '+' or '-', which is inferior.
			if ((tokens[i].type == '+' || tokens[i].type == '-') && inbrkt == 0) {
				pivot = i;
				grade = 1;
			} else if ((tokens[i].type == '*' || tokens[i].type == '/') && inbrkt == 0 && grade == 0) {
				pivot = i;
			}
		}

		// check the bracket appear in pair
		if (inbrkt != 0) {
			Log("Bad expression, bracket mismatch");
			*success = false;
			return 0;
		}
		// ensure the expression is valid
		if (pivot == -1) {
			Log("Bad expression: can't find pivot");
			*success = false;
			return 0;
		}
		// Divide and conquer
		uint32_t a = eval(p, pivot-1, success);
		if (!*success) return 0;
		uint32_t b = eval(pivot+1, q, success);
		if (!*success) return 0;

		switch(tokens[pivot].type) {
			case '+':	return a + b; 
			case '-': return a - b;
			case '*': return a * b;
			case '/': 
				if (b == 0) {
					*success = false;
					return 0;
				}
				return a / b;
		}
	}
	*success = false;
	return 0;
}
