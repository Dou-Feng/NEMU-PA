#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
	
  /* TODO: Add more members if necessary */
	char str[1024]; // to record the expression
	int32_t value; // to record the orginal value
} WP;

WP* new_wp(char *expr);

bool free_wp(WP *wp);

void watchpoint_display(void);

WP* update_value(void);
#endif
