#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

int choose(uint32_t t) {
	return rand() % t;
}

// this should be enough
static char buf[65536];
int buf_p;
int token_num;
static inline void gen_rand_expr() {
	if (65536 - buf_p < 200 || token_num >= 1023) return; // incase buffer overflow
	uint32_t ud;
	switch(choose(4)) {
		case 0: 
		ud = (uint32_t) rand();
			sprintf(buf+buf_p, "%uu", ud);
			buf_p = strlen(buf);
			token_num++;
			break;
		case 1:
			sprintf(buf+buf_p++, "(");
			token_num++;
			gen_rand_expr();
			sprintf(buf+buf_p++, ")");
			token_num++;
			break;
		case 2:
			gen_rand_expr();
			sprintf(buf+buf_p++, " ");
			token_num++;
			break;
		case 3:
			gen_rand_expr();
			ud = choose(4);
			token_num++;
			switch(ud) {
				case 0: sprintf(buf+buf_p++, "+"); break;
				case 1: sprintf(buf+buf_p++, "-"); break;
				case 2: sprintf(buf+buf_p++, "*"); break;
				case 3: sprintf(buf+buf_p++, "/"); break;
			}
			gen_rand_expr();
			break;
	}
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
		buf_p = 0;
		token_num = 0;
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Wall -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
