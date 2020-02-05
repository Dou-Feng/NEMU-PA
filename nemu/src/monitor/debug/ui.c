#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
void exec_once(void);
void isa_reg_display(void);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

// Pause the program after stepping N instructions
// @param: args == NULL or args is numeric
// @output: Execute successfully return 0; If args is invalid, assert!!!
static int cmd_si(char *args) {
	int N = 1;
	if (args != NULL) {
		/* check validation of args */
		char *str_n = strtok(args, " ");
		if (strtok(NULL, " ") != NULL) {
			printf( "Too many arguments\n");
			return 0;
		}
		sscanf(str_n, "%d", &N);
	}
	
	// step N instructions
	while (N--) {
		exec_once();	
	}
	
	return 0;		
	
}

// Print values of registers or watch points
// @param: args is 'r' or 'w'
// @output: Print values of registers or watch points, which depend on the args.
// @return: 0  
static int cmd_info(char *args){
	// check the args
	if (args == NULL) {
		printf("Expect argument\n");
		return 0;
	}
	char op;
	if (sscanf(args, "%c", &op) == 0) {
		printf("Expect argument\n");
		return 0;
	}
	
	// Print values of registers
	if (op == 'r') {
		isa_reg_display();
	} else if (op == 'w') {
	// Print values of watch points
		watchpoint_display();
	} else {
		printf("Unexpected argument\n");
		return 0;
	}
	return 0;
}

// Evaluate the expression and Print the result 
// @param: args including a valid expression
// @output: Print the reuslt of expression
// @return: always return 0
static int cmd_p(char *args) {
	bool success;
	uint32_t ans = expr(args, &success);
	if (success) {
		printf("%s = %d, 0x%08x\n", args, ans, ans);
	} else {
		printf("Unexpected argument\n");
	}

	return 0;
}

// Print consecutive N bytes starting from the result of expr
// @param: args including N and expr
// @output: Print memory information of consecutive N bytes
// @return: Execute successfully return 0, or Assert
static int cmd_x(char *args) {
	// ensure the args is not NULL
	if (!args) {
		printf("Expect argument\n");
		return 0;
	}

	char *str_end = args + strlen(args);
	/* parse the args */
	// get number N
	unsigned N = 1;	
	char *str_n = strtok(args, " ");
	if (sscanf(str_n, "%d", &N) != 1) {
		printf("Expect a positive integer\n");
		return 0;
	}
	// get expr
	char *str_expr = args + strlen(str_n) + 1;
	if (str_expr == str_end) {
		printf("Expect more argument\n");
		return 0;
	}
	bool success;
	paddr_t begin = expr(str_expr, &success);
	if (!success) {
		printf("Unexpected argument\n");
		return 0;
	}
	
	// print consecutive N bytes memory
	for (paddr_t i = 0; i < N; i++) {
		printf("0x%08x: 0x%08x\n", begin+(i<<2), paddr_read(begin+(i<<2), 4));
	}
	return 0;
}


static int cmd_w(char *args) {
	WP *wp;
	if ((wp = new_wp(args))) {
		printf("watchpiont at %s: 0x%08x, %u\n", wp->str, wp->value, wp->value);
		return 0;
	} else {
		printf("Unexpected argument\n");
		return 0;
	}
}


static int cmd_d(char *args) {
	if (!args) {
		printf("Expect argument\n");
		return 0;
	}
	WP wp;
	if (sscanf(args, "%u", &wp.NO) != 1) {
		printf("Expect a positive integer\n");
		return 0;
	}
	if (free_wp(&wp)) {
		printf("Delete watchpoint %u\n", wp.NO);
	} else {
		printf("Delete watchpoint fail\n");
	}
	return 0;
}


static int cmd_help(char *args);


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  /* TODO: Add more commands */
	{ "si", "Pause the program after stepping N instructions. The default N is 1.", cmd_si },
	{ "info", "print info of register or watch point", cmd_info },
	{ "p", "Evaluate the value of EXPR", cmd_p },
	{ "x", "Find the value of expression EXPR, take the result as the starting memory address, and output n consecutive 4 bytes in hexadecimal form", cmd_x },
	{ "w", "Pause program execution when the value of expression EXPR changes", cmd_w },
	{ "d", "Delete the watch point with N", cmd_d },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void test_cmd_p() {
	// test function cmd_p
	FILE *fp = fopen("/tmp/input", "r");
	assert(fp != NULL);
	uint32_t result = -1;
	bool success;	
	char line[65536 + 100];

	while (fgets(line, 65536 + 100, fp) != NULL) {
		char *number = strtok(line, " ");
		char *args = line + strlen(number) + 1; 
		// delete last '\n'
		if (args[strlen(args)-1] == '\n') {
			args[strlen(args)-1] = '\0';
		}

		sscanf(number, "%u", &result);
		uint32_t ans = expr(args, &success);
		if (!success || ans != result) {
			if (ans == UINT32_MAX) continue;
			printf("Test Failure! %s\n %u != %u\n", args, ans, result);
			return;
		}
	}
	printf("cmd_p test success!\n");
	fclose(fp);
}
