/* The checking harness. You shouldn't need to touch this file —
 * read it freely though, the expected values ARE the spec. */

#include "exercises.h"
#include <errno.h>
#include <string.h>

static int pass_n, fail_n;

#define CHECK(desc, cond)                                                  \
	do {                                                               \
		if (cond) {                                                \
			pass_n++;                                          \
			printk("  PASS  %s\n", desc);                      \
		} else {                                                   \
			fail_n++;                                          \
			printk("  FAIL  %s\n", desc);                      \
		}                                                          \
	} while (0)

/* ------------------------------ C track ------------------------------ */

static void check_c1_reverse(void)
{
	char a[] = "embedded";
	char b[] = "x";
	char c[] = "ab";

	ex_reverse_string(a);
	ex_reverse_string(b);
	ex_reverse_string(c);
	CHECK("C1 reverse: \"embedded\" -> \"deddebme\"",
	      strcmp(a, "deddebme") == 0);
	CHECK("C1 reverse: single char unchanged", strcmp(b, "x") == 0);
	CHECK("C1 reverse: even length \"ab\" -> \"ba\"",
	      strcmp(c, "ba") == 0);
}

static void check_c2_popcount(void)
{
	CHECK("C2 popcount(0) == 0", ex_popcount(0) == 0);
	CHECK("C2 popcount(0xFF) == 8", ex_popcount(0xFF) == 8);
	CHECK("C2 popcount(0x80000001) == 2", ex_popcount(0x80000001u) == 2);
	CHECK("C2 popcount(0xFFFFFFFF) == 32",
	      ex_popcount(0xFFFFFFFFu) == 32);
}

static void check_c3_parse_int(void)
{
	int32_t v = 999;

	CHECK("C3 parse \"42\"", ex_parse_int("42", &v) == 0 && v == 42);
	CHECK("C3 parse \"-17\"", ex_parse_int("-17", &v) == 0 && v == -17);
	CHECK("C3 parse \"+5\"", ex_parse_int("+5", &v) == 0 && v == 5);
	CHECK("C3 parse \"0\"", ex_parse_int("0", &v) == 0 && v == 0);
	CHECK("C3 reject \"\"", ex_parse_int("", &v) == -EINVAL);
	CHECK("C3 reject \"12x\"", ex_parse_int("12x", &v) == -EINVAL);
	CHECK("C3 reject \"+\"", ex_parse_int("+", &v) == -EINVAL);
}

static void check_c4_ring(void)
{
	struct ex_ring r;
	uint8_t out = 0;
	int ok = 1;

	ex_ring_init(&r);
	CHECK("C4 pop on empty -> -ENODATA",
	      ex_ring_pop(&r, &out) == -ENODATA);

	for (int i = 0; i < EX_RING_CAP; i++) {
		if (ex_ring_push(&r, (uint8_t)i) != 0) {
			ok = 0;
		}
	}
	CHECK("C4 16 pushes fit", ok);
	CHECK("C4 17th push -> -ENOSPC", ex_ring_push(&r, 99) == -ENOSPC);

	ok = 1;
	for (int i = 0; i < EX_RING_CAP; i++) {
		if (ex_ring_pop(&r, &out) != 0 || out != (uint8_t)i) {
			ok = 0;
		}
	}
	CHECK("C4 FIFO order preserved", ok);
	CHECK("C4 empty again after drain",
	      ex_ring_pop(&r, &out) == -ENODATA);
}

static void check_c5_list(void)
{
	struct ex_node a = { .value = 5 };
	struct ex_node b = { .value = 1 };
	struct ex_node c = { .value = 3 };
	struct ex_node *head = NULL;

	head = ex_list_insert_sorted(head, &a);   /* 5            */
	head = ex_list_insert_sorted(head, &b);   /* 1 -> 5       */
	head = ex_list_insert_sorted(head, &c);   /* 1 -> 3 -> 5  */

	CHECK("C5 insert into empty list", head != NULL);
	CHECK("C5 sorted order 1->3->5",
	      head == &b && b.next == &c && c.next == &a && a.next == NULL);
}

static void check_c6_regfield(void)
{
	CHECK("C6 write 0xA at [7:4] of 0",
	      ex_reg_set_field(0x0, 4, 4, 0xA) == 0xA0);
	CHECK("C6 write 0x12 at [15:8] of all-ones",
	      ex_reg_set_field(0xFFFFFFFFu, 8, 8, 0x12) == 0xFFFF12FFu);
	CHECK("C6 value truncated to width",
	      ex_reg_set_field(0x0, 0, 4, 0x1FF) == 0xF);
}

/* ---------------------------- Zephyr track ---------------------------- */

static void check_z1_timer(void)
{
	ex_timer_count = 0;
	ex_timer_start(50);
	k_sleep(K_MSEC(275));
	ex_timer_stop();
	int snap = ex_timer_count;

	k_sleep(K_MSEC(120));
	CHECK("Z1 timer ticked ~5x in 275ms @50ms", snap >= 4 && snap <= 6);
	CHECK("Z1 timer actually stopped", ex_timer_count == snap);
}

static void check_z2_sem(void)
{
	struct k_sem sem;

	k_sem_init(&sem, 0, 1);

	int64_t t0 = k_uptime_get();
	int ret = ex_sem_take_with_timeout(&sem, 100);
	int64_t dt = k_uptime_get() - t0;

	CHECK("Z2 timeout returns -EAGAIN", ret == -EAGAIN);
	CHECK("Z2 timeout actually waited ~100ms", dt >= 90 && dt <= 400);

	k_sem_give(&sem);
	CHECK("Z2 take succeeds when available",
	      ex_sem_take_with_timeout(&sem, 100) == 0);
}

static void check_z3_msgq(void)
{
	static char qbuf[8 * sizeof(int32_t)];
	struct k_msgq q;
	const int32_t vals[5] = { 1, 2, 3, 4, 5 };

	k_msgq_init(&q, qbuf, sizeof(int32_t), 8);

	CHECK("Z3 burst send returns 0",
	      ex_msgq_send_burst(&q, vals, 5) == 0);
	CHECK("Z3 drain sums to 15", ex_msgq_drain_sum(&q) == 15);
	CHECK("Z3 queue empty after drain", k_msgq_num_used_get(&q) == 0);
}

static void check_z4_threads(void)
{
	ex_shared_counter = 0;

	k_tid_t t0 = ex_spawn_counter_thread(0);
	k_tid_t t1 = ex_spawn_counter_thread(1);

	CHECK("Z4 threads spawned", t0 != NULL && t1 != NULL);
	if (t0 == NULL || t1 == NULL) {
		fail_n++; /* count the result check as failed too */
		printk("  FAIL  Z4 counter == 2000 (threads missing)\n");
		return;
	}
	k_thread_join(t0, K_SECONDS(5));
	k_thread_join(t1, K_SECONDS(5));
	CHECK("Z4 counter == 2000 (mutex kept it exact)",
	      ex_shared_counter == 2000);
}

static void check_z5_work(void)
{
	ex_work_flag = 0;
	ex_schedule_flag_set(150);
	k_sleep(K_MSEC(50));
	CHECK("Z5 flag still 0 before the delay", ex_work_flag == 0);
	k_sleep(K_MSEC(200));
	CHECK("Z5 flag set after the delay", ex_work_flag == 1);
}

/* ------------------------------ runner ------------------------------- */

int checks_run_all(void)
{
	pass_n = 0;
	fail_n = 0;

	printk("--- Track 1: C ---\n");
	check_c1_reverse();
	check_c2_popcount();
	check_c3_parse_int();
	check_c4_ring();
	check_c5_list();
	check_c6_regfield();

	printk("--- Track 2: Zephyr ---\n");
	check_z1_timer();
	check_z2_sem();
	check_z3_msgq();
	check_z4_threads();
	check_z5_work();

	printk("\n===== %d passing / %d failing =====\n", pass_n, fail_n);
	if (fail_n == 0) {
		printk("ALL GREEN. You are not rusty anymore. \\o/\n");
	}
	return fail_n;
}
