#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void printchain(void);

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(char *args) {
	WP* p = free_, *q;
	// limit the amount of watchpoint
	Assert(p != NULL, "Watchpoint pool is empty");

	// check the expr is valid
	bool success;
	uint32_t v = expr(args, &success);
	if (!success) {
		return NULL;
	}

	// get a WP node
	if (p->next)
		q = p->next;
	else
		q = p;
	while (q->next) {
		p = q;
		q = q->next;
	}
	// delete the node q from free_ chain
	if (q == free_) {
		free_ = NULL;
	} else { 
		p->next = NULL;
	}

	// add the node q to the head chain
	WP* h = head;
	if (h) {
		while (h->next) {
			h = h->next;
		}
		h->next = q;
	} else {
		head = q;
	}
	
	// copy the expression the str of the node q
	Assert(strlen(args) < 1024, "new_wp, the expr is too long");
	strcpy(q->str, args);
	
	// assign the value to node.value
	q->value = v;

	printchain();
	return q;
}

bool free_wp(WP *wp) {
	if (!wp || !head) {
		return false;
	}

	// delete wp from head chain
	WP *p = head, *q;
	q = p;

	while (q) {
		if (q->NO == wp->NO) {
			break;
		}
		p = q;
		q = q->next;
	}

	// find the last node of free_ chain
	WP* f = free_;
	while (f && f->next) {
		f = f->next;
	}
	
	// not find
	if (!q) return false;

	// q is the head
	if (head->NO == wp->NO) {
		if (!f) {
			free_ = head;
			f = free_;
		} else {
			f->next = head;
		}
		WP *p_head = head;
		head = head->next;
		p_head->next = NULL;
		// for debug
		printchain();
		return true;
	}
	
	// if q is not the head
	if (!f) {
		free_ = q;
		f = free_;
	} else {
		f->next = q;
	}
	p->next = q->next;
	q->next = NULL;
	printchain();
	return true;
}


void watchpoint_display() {
	printf("watchpoint:\n");
	WP *p = head;
	while (p) {
		printf("%u\t %s\t 0x%08x\n", p->NO, p->str, p->value);
		p = p->next;
	}
}

void printchain() {
	printf("Head chain: ");
	WP *p = head;
	while (p) {
		printf("%d-> ", p->NO);
		p = p->next;
	}
	printf("NULL\n");
	printf("Free_ chain: ");
	p = free_;
	while (p) {
		printf("%d-> ", p->NO);
		p = p->next;
	}
	printf("NULL\n");
}

// update the watchpoints' value
// @return: if there exists new value, return the watchpoint; Or return NULL
WP* update_value() {
	WP *p = head;
	bool success;
	while (p) {
		uint32_t new_v = expr(p->str, &success);
		Assert(success, "UpdateValue, watchpoint %u's expr error", p->NO);
		if (new_v != p->value) {
			p->value = new_v;
			return p;
		}
		p = p->next;
	}
	return NULL;
}


