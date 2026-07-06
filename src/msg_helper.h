#ifndef MSG_HELPER_H
#define MSG_HELPER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_PAYLOAD_SIZE     512
#define MAX_MSGS_PER_SESSION 64

typedef enum {
	MSG_STATE_SEEN,
	MSG_STATE_ACK,
	MSG_STATE_NOMEM,
	MSG_STATE_INVALID,   /* bad arguments (NULL, empty/oversized payload) */
} msg_state_t;

struct msg {
	bool used;
	/* Content-derived id: crc32_ieee over payload[0..payload_len).
	 * Computed by msg_helper_add() — callers no longer assign ids.
	 * CRC-32/IEEE is config- and platform-stable, so the peer can
	 * compute the identical id over the same bytes. */
	uint32_t id;
	uint8_t payload[MAX_PAYLOAD_SIZE];
	size_t payload_len;
};

struct msg_helper {
	struct msg messages[MAX_MSGS_PER_SESSION];
};

int msg_helper_init(struct msg_helper *helper);

/* Hashes m->payload into m->id, then dedups by that id.
 * Returns MSG_STATE_ACK (stored), MSG_STATE_SEEN (duplicate content),
 * MSG_STATE_NOMEM (full), MSG_STATE_INVALID (bad args). */
msg_state_t msg_helper_add(struct msg_helper *helper, struct msg *m);

#endif // MSG_HELPER_H
