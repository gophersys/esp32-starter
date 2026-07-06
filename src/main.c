#include <zephyr/kernel.h>
#include "exercises.h"

int main(void)
{
	printk("\n========================================\n");
	printk(" C / Zephyr review — make the checks pass\n");
	printk(" edit src/exercises_c.c + src/exercises_zephyr.c\n");
	printk(" then: ./ctl.sh run\n");
	printk("========================================\n\n");

	int failing = checks_run_all();

	while (1) {
		k_sleep(K_SECONDS(60));
		printk("(%d failing — edit, save, ./ctl.sh run)\n", failing);
	}
	return 0;
}
