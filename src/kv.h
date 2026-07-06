#ifndef KV_H
#define KV_H

// Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// A couple of assumptions:
// - not using a macro at the library level here, instead a const to later be used internally
//.  in the .c file to allocate a static block of memory, same same but diff, user can then
//.  crrate the config 1 level up from wehre this gets called
// - for the error types, im going to use the standard errrno.h ferrors, although i could also
//   make an enum with the error types, and instead pass storage pointers to the get function,
//.  either way, i think either implementation is fine, this is more zephyr like
// - KV_CFG_MAX_KEY_LEN includes the NUL terminator (max 31 visible chars)

#define KV_CFG_MAX_KEY_LEN 32
#define KV_CFG_MAX_ITEMS 16

typedef struct
{
    bool set;
    char name[KV_CFG_MAX_KEY_LEN];
    int32_t value;
} kv_t;

typedef struct
{
    bool initialized;
    kv_t entries[KV_CFG_MAX_ITEMS];
} kv_store_t;

int kv_init(kv_store_t *store);
int kv_set(kv_store_t *store, const char *key, int32_t value);
int kv_get(const kv_store_t *store, const char *key, int32_t *value);

// Stage 2
int kv_delete(kv_store_t *store, const char *key);
size_t kv_count(const kv_store_t *store);

#endif
