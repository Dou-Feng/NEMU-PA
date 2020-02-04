int init_monitor(int, char *[]);
void ui_mainloop(int);
void test_cmd_p(void);

#define TEST_CMD_P

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);
#ifdef TEST_CMD_P
 test_cmd_p();	

#endif

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
