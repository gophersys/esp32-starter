#ifndef EXERCISES_H_
#define EXERCISES_H_

#include <zephyr/kernel.h>
#include <stdint.h>
#include <stddef.h>

/*
 * C / Zephyr review exercises.
 *
 * Implement the TODOs in exercises_c.c (track 1) and exercises_zephyr.c
 * (track 2). Then build+flash+monitor (./ctl.sh run) and make the checks
 * pass. Do them in order — they build up.
 */

/* ------------------------- Track 1: C ------------------------- */

/* C1: reverse a NUL-terminated string in place. */
void ex_reverse_string(char *s);

/* C2: count set bits. No __builtin_popcount — do it by hand. */
int ex_popcount(uint32_t v);

/* C3: parse a decimal integer ("42", "-17", "+5").
 * Return 0 and write *out on success; -EINVAL on empty string or any
 * trailing garbage ("12x" is invalid). */
int ex_parse_int(const char *s, int32_t *out);

/* C4: fixed-size byte FIFO (ring buffer) — the embedded classic. */
#define EX_RING_CAP 16
struct ex_ring {
	uint8_t buf[EX_RING_CAP];
	size_t head;   /* next write index */
	size_t tail;   /* next read index  */
	size_t count;  /* elements stored  */
};
void ex_ring_init(struct ex_ring *r);
int ex_ring_push(struct ex_ring *r, uint8_t b);       /* -ENOSPC when full  */
int ex_ring_pop(struct ex_ring *r, uint8_t *out);     /* -ENODATA when empty */

/* C5: sorted insert into a singly linked list (ascending by value).
 * No malloc — nodes are caller-owned. Return the (possibly new) head. */
struct ex_node {
	int32_t value;
	struct ex_node *next;
};
struct ex_node *ex_list_insert_sorted(struct ex_node *head,
				      struct ex_node *node);

/* C6: register field write — clear `width` bits at `shift`, then write
 * `value` there (value truncated to width). Classic MMIO pattern. */
uint32_t ex_reg_set_field(uint32_t reg, unsigned int shift,
			  unsigned int width, uint32_t value);

/* ---------------------- Track 2: Zephyr ----------------------- */

/* Z1: periodic k_timer. Start a timer whose callback increments
 * ex_timer_count every period_ms. ex_timer_stop() stops it. */
extern volatile int ex_timer_count;
void ex_timer_start(uint32_t period_ms);
void ex_timer_stop(void);

/* Z2: take a semaphore with a millisecond timeout.
 * Return 0 on success, -EAGAIN on timeout (hint: k_sem_take's return). */
int ex_sem_take_with_timeout(struct k_sem *sem, uint32_t timeout_ms);

/* Z3: message queue. Send n values (block up to 100ms each); then drain
 * whatever is queued (non-blocking) and return the sum. */
int ex_msgq_send_burst(struct k_msgq *q, const int32_t *vals, size_t n);
int32_t ex_msgq_drain_sum(struct k_msgq *q);

/* Z4: spawn a thread (idx 0 or 1, static stacks provided in the .c) that
 * increments ex_shared_counter 1000 times, holding ex_lock for each
 * increment (it races with its twin without it). Return the k_tid_t. */
extern volatile int ex_shared_counter;
extern struct k_mutex ex_lock;
k_tid_t ex_spawn_counter_thread(int idx);

/* Z5: delayable work. Schedule work that sets ex_work_flag=1 after
 * delay_ms (hint: k_work_init_delayable + k_work_schedule). */
extern volatile int ex_work_flag;
void ex_schedule_flag_set(uint32_t delay_ms);

/* --------------------------- harness --------------------------- */
/* Runs every check, prints PASS/FAIL lines, returns #failing. */
int checks_run_all(void);

#endif /* EXERCISES_H_ */
