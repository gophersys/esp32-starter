#ifndef KV_H
#define KV_H

// Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __ZEPHYR__
#include <zephyr/kernel.h>
#endif

// Assumptions:
// - fixed compile-time capacity, no dynamic allocation; caller owns the store
// - errors: standard negative errno values (0 = success) — zephyr-like
// - KV_CFG_MAX_KEY_LEN includes the NUL terminator (max 31 visible chars)
// - under Zephyr the store carries its own mutex (initialized in kv_init):
//   all operations are thread-safe. On host builds the lock compiles away.

#define KV_CFG_MAX_KEY_LEN 32
#define KV_CFG_MAX_ITEMS 16

typedef struct
{
    bool set;
    char name[KV_CFG_MAX_KEY_LEN];
    int32_t value;
} kv_t;

typedef struct kv_store
{
    bool initialized;
#ifdef __ZEPHYR__
    struct k_mutex lock;   // per-store lock — instances lock independently
#endif
    kv_t entries[KV_CFG_MAX_ITEMS];
} kv_store_t;

int kv_init(kv_store_t *store);
int kv_set(kv_store_t *store, const char *key, int32_t value);
int kv_get(const kv_store_t *store, const char *key, int32_t *value);

// Stage 2
int kv_delete(kv_store_t *store, const char *key);
size_t kv_count(const kv_store_t *store);

#endif
