#include "msg_helper.h"

// Zephyr includes
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/crc.h>

// Standard includes
#include <string.h>

LOG_MODULE_REGISTER(app);

int msg_helper_init(struct msg_helper *helper)
{
	if (helper == NULL) {
		LOG_ERR("Null msg helper object");
		return -EINVAL;
	}

	memset(helper, 0, sizeof(*helper));

	return 0;
}

msg_state_t msg_helper_add(struct msg_helper *helper, struct msg *m)
{
	if (helper == NULL || m == NULL) {
		LOG_ERR("Null argument");
		return MSG_STATE_INVALID;
	}

	// Validate BEFORE hashing/copying: an oversized payload_len would
	// otherwise overflow the destination buffer in the memcpy below
	if (m->payload_len == 0 || m->payload_len > MAX_PAYLOAD_SIZE) {
		LOG_ERR("Bad payload length %zu", m->payload_len);
		return MSG_STATE_INVALID;
	}

	// Protocol change: the id IS the payload hash (content-addressed).
	// crc32_ieee: built-in, standardized, identical on every platform
	// and config — unlike sys_hash32(), whose algorithm is Kconfig-
	// dependent and therefore unsuitable as a wire-protocol field.
	m->id = crc32_ieee(m->payload, m->payload_len);

	// Dedup by content id. Note: only LIVE slots may be compared —
	// a fresh store is all-zeros, and a payload whose crc32 happens
	// to be 0 would otherwise false-match every empty slot.
	for (size_t i = 0; i < ARRAY_SIZE(helper->messages); i++) {
		struct msg *current = &helper->messages[i];

		if (current->used && current->id == m->id) {
			return MSG_STATE_SEEN;
		}
	}

	// Not seen: store in the first free slot
	for (size_t i = 0; i < ARRAY_SIZE(helper->messages); i++) {
		struct msg *current = &helper->messages[i];

		if (!current->used) {
			current->id = m->id;
			memcpy(current->payload, m->payload, m->payload_len);
			current->payload_len = m->payload_len;
			current->used = true;
			return MSG_STATE_ACK;
		}
	}

	LOG_WRN("Message helper out of storage");
	return MSG_STATE_NOMEM;
}
