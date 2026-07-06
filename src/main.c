#include <zephyr/kernel.h>

// Zephyr includes
#include <zephyr/logging/log.h>

// Program includes
#include "msg_helper.h"

LOG_MODULE_DECLARE(app, CONFIG_LOG_DEFAULT_LEVEL);

/* ids are no longer assigned — they are computed from the payload.
 * msgs[0] and msgs[1] carry IDENTICAL payloads -> the second must dedup
 * as SEEN even though the sender never numbered them. */
static struct msg msgs[] = {
	{
		.payload = {12, 2, 3, 3, 4, 5, 5, 6},
		.payload_len = 8,
	},
	{
		.payload = {12, 2, 3, 3, 4, 5, 5, 6},
		.payload_len = 8,
	},
	{
		.payload = {9, 9, 9},
		.payload_len = 3,
	},
};

int main(void)
{
	static struct msg_helper h;

	if (msg_helper_init(&h) != 0) {
		LOG_ERR("No init");
		return -1;
	}

	for (size_t i = 0; i < ARRAY_SIZE(msgs); i++) {
		msg_state_t ret = msg_helper_add(&h, &msgs[i]);

		if (ret == MSG_STATE_SEEN) {
			LOG_WRN("already have message id=0x%08x", msgs[i].id);
		} else if (ret == MSG_STATE_ACK) {
			LOG_INF("message ack id=0x%08x (len=%zu)",
				msgs[i].id, msgs[i].payload_len);
		} else if (ret == MSG_STATE_NOMEM) {
			LOG_ERR("No space left in helper, have %d messages",
				(int)ARRAY_SIZE(h.messages));
		} else {
			LOG_ERR("Invalid message or arguments (ret %d)", ret);
		}
	}

	return 0;
}
