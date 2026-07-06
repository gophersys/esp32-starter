#include <zephyr/kernel.h>

int main(void)
{
	int count = 0;

	printk("esp32-starter booted on %s\n", CONFIG_BOARD_TARGET);
	while (1) {
		printk("tick %d\n", count++);
		k_sleep(K_SECONDS(1));
	}
	return 0;
}
