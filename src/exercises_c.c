/* Track 1: C fundamentals. Implement each TODO, then ./ctl.sh run. */

#include "exercises.h"
#include <errno.h>
#include <string.h>

/* C1 — hint: two indices walking toward each other. */
void ex_reverse_string(char *s)
{
	/* TODO: implement */
	(void)s;
}

/* C2 — hint: v &= v - 1 clears the lowest set bit. Or shift and mask. */
int ex_popcount(uint32_t v)
{
	/* TODO: implement */
	(void)v;
	return -1;
}

/* C3 — hint: optional sign, then digits, reject anything else.
 * Watch out: empty-after-sign ("+") is invalid too. */
int ex_parse_int(const char *s, int32_t *out)
{
	/* TODO: implement */
	(void)s;
	(void)out;
	return -ENOSYS;
}

/* C4 — hint: head/tail wrap with % EX_RING_CAP; count tracks fullness. */
void ex_ring_init(struct ex_ring *r)
{
	/* TODO: implement */
	(void)r;
}

int ex_ring_push(struct ex_ring *r, uint8_t b)
{
	/* TODO: implement */
	(void)r;
	(void)b;
	return -ENOSYS;
}

int ex_ring_pop(struct ex_ring *r, uint8_t *out)
{
	/* TODO: implement */
	(void)r;
	(void)out;
	return -ENOSYS;
}

/* C5 — hint: walk with a struct ex_node **cursor and you won't need a
 * special case for inserting at the head. */
struct ex_node *ex_list_insert_sorted(struct ex_node *head,
				      struct ex_node *node)
{
	/* TODO: implement */
	(void)node;
	return head;
}

/* C6 — hint: build the mask as ((1u << width) - 1) << shift.
 * Clear those bits in reg, OR in the (truncated, shifted) value. */
uint32_t ex_reg_set_field(uint32_t reg, unsigned int shift,
			  unsigned int width, uint32_t value)
{
	/* TODO: implement */
	(void)shift;
	(void)width;
	(void)value;
	return reg;
}
