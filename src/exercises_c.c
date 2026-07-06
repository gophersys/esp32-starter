/* Track 1: C fundamentals. Implement each TODO, then ./ctl.sh run. */

// Private includes
#include "exercises.h"

// Standard includes
#include <errno.h>
#include <string.h>

// Zephyr includes
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

/* Use the default Zephyr log level here; in a multi-file module, one file
 * should use LOG_MODULE_REGISTER() and the others LOG_MODULE_DECLARE().
 */
LOG_MODULE_DECLARE(exercise_c, CONFIG_LOG_DEFAULT_LEVEL);

/* C1 — hint: two indices walking toward each other. */
void ex_reverse_string(char *s)
{
	size_t len = strlen(s);

	// Easy why we can't swap a string of 2
	if (strlen(s) < 2) {
		LOG_WRN("Cant' switch a string of only 2 characters big dawg\n");
		return;
	}

	// Get both pointers
	char *start = s;
	char *end = s + len - 1;

	while (start < end) {
		// Swap the pointers, first save one value
		char tmp = *start;
		*start = *end;
		*end = tmp;

		// Advance the pointers
		start++;
		end--;
	}

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
struct ex_node *ex_list_insert_sorted(struct ex_node *head, struct ex_node *node)
{
	/* TODO: implement */
	(void)node;
	return head;
}

/* C6 — hint: build the mask as ((1u << width) - 1) << shift.
 * Clear those bits in reg, OR in the (truncated, shifted) value. */
uint32_t ex_reg_set_field(uint32_t reg, unsigned int shift, unsigned int width, uint32_t value)
{
	/* TODO: implement */
	(void)shift;
	(void)width;
	(void)value;
	return reg;
}
