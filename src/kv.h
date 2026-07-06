#ifndef KV_H
#define KV_H

// Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <zephyr/kernel.h>

// Assumptions:
// - fixed compile-time capacity, no dynamic allocation; caller owns the store
// - errors: standard negative errno values (0 = success) — zephyr-like
// - KV_CFG_MAX_KEY_LEN includes the NUL terminator (max 31 visible chars)
// - the store carries its own mutex (initialized in kv_init):
//   all operations are thread-safe

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
    struct k_mutex lock;   // per-store lock — instances lock independently
    kv_t entries[KV_CFG_MAX_ITEMS];
} kv_store_t;

int kv_init(kv_store_t *store);
int kv_set(kv_store_t *store, const char *key, int32_t value);
int kv_get(kv_store_t *store, const char *key, int32_t *value);

// Stage 2
int kv_delete(kv_store_t *store, const char *key);
size_t kv_count(kv_store_t *store);

#endif
