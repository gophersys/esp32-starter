/* Track 2: Zephyr kernel primitives. Docs: docs.zephyrproject.org
 * (Kernel Services). Implement each TODO, then ./ctl.sh run. */

#include "exercises.h"
#include <errno.h>

/* ---- shared state the checker observes (leave these alone) ---- */
volatile int ex_timer_count;
volatile int ex_shared_counter;
volatile int ex_work_flag;
K_MUTEX_DEFINE(ex_lock);

/* static kernel objects for you to use */
static struct k_timer ex_timer;
static struct k_work_delayable ex_dwork;
K_THREAD_STACK_ARRAY_DEFINE(ex_stacks, 2, 1024);
static struct k_thread ex_threads[2];

/* Z1 — hint: k_timer_init(&ex_timer, callback, NULL) once, then
 * k_timer_start(&ex_timer, K_MSEC(p), K_MSEC(p)). The callback signature
 * is void fn(struct k_timer *). It runs in ISR context — keep it tiny. */
void ex_timer_start(uint32_t period_ms)
{
	/* TODO: implement (callback increments ex_timer_count) */
	(void)period_ms;
}

void ex_timer_stop(void)
{
	/* TODO: implement */
}

/* Z2 — hint: k_sem_take returns 0 or -EAGAIN when given a finite
 * K_MSEC() timeout. This one is almost free — it's an API warm-up. */
int ex_sem_take_with_timeout(struct k_sem *sem, uint32_t timeout_ms)
{
	/* TODO: implement */
	(void)sem;
	(void)timeout_ms;
	return -ENOSYS;
}

/* Z3 — hint: k_msgq_put / k_msgq_get. K_NO_WAIT vs K_MSEC(100).
 * Drain means: pop until the queue says no. */
int ex_msgq_send_burst(struct k_msgq *q, const int32_t *vals, size_t n)
{
	/* TODO: implement */
	(void)q;
	(void)vals;
	(void)n;
	return -ENOSYS;
}

int32_t ex_msgq_drain_sum(struct k_msgq *q)
{
	/* TODO: implement */
	(void)q;
	return 0;
}

/* Z4 — hint: k_thread_create(&ex_threads[idx], ex_stacks[idx],
 * K_THREAD_STACK_SIZEOF(ex_stacks[idx]), entry, NULL, NULL, NULL,
 * 5, 0, K_NO_WAIT). In the entry: 1000 × { lock ex_lock, increment,
 * unlock }. Sprinkle a k_yield() every ~100 iterations so the two
 * threads actually interleave and an unlocked version really fails. */
k_tid_t ex_spawn_counter_thread(int idx)
{
	/* TODO: implement */
	(void)idx;
	return NULL;
}

/* Z5 — hint: k_work_init_delayable(&ex_dwork, handler) once, then
 * k_work_schedule(&ex_dwork, K_MSEC(delay_ms)). Handler signature is
 * void fn(struct k_work *). */
void ex_schedule_flag_set(uint32_t delay_ms)
{
	/* TODO: implement (handler sets ex_work_flag = 1) */
	(void)delay_ms;
}
