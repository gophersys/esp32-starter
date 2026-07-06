/* =====================  Z-STAGE (on-target)  =====================
 * Make this module THREAD-SAFE. Two Zephyr threads will hammer it
 * concurrently (see check_kvz in checks.c). Your job:
 *   1. K_MUTEX_DEFINE(...) a lock for the store
 *   2. take it in kv_set / kv_get / kv_delete / kv_count
 *      (k_mutex_lock(&m, K_FOREVER) / k_mutex_unlock(&m))
 *   3. THE TRAP: these functions have many early returns — every
 *      single exit path must unlock, or the second caller hangs
 *      forever. Restructure if you need to (single-exit, or a
 *      ret variable + goto out). This trap is the exercise.
 * ================================================================= */
#include "kv.h"

// Standard includes
#include <errno.h>
#include <string.h>

#define KV_ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

int kv_init(kv_store_t *store)
{
    if (store == NULL)
    {
        return -EINVAL;
    }

    // NOTE: no "already initialized?" guard — a fresh stack struct holds
    // garbage bytes, so reading store->initialized BEFORE init is UB.
    // Re-init is simply defined as: wipe everything.
    memset(store, 0, sizeof(*store));
    store->initialized = true;

    return 0;
}

int kv_set(kv_store_t *store, const char *key, int32_t value)
{
    if (store == NULL || !store->initialized)
    {
        return -EINVAL;
    }

    // NULL check MUST come before strlen (strlen(NULL) = crash)
    if (key == NULL || key[0] == '\0')
    {
        return -EINVAL;
    }

    if (strlen(key) >= KV_CFG_MAX_KEY_LEN)
    {
        return -EINVAL;
    }

    // First, lets see if we have already seen this guy -> overwrite
    for (size_t e = 0; e < KV_ARRAY_LEN(store->entries); e++)
    {
        if (store->entries[e].set && strcmp(store->entries[e].name, key) == 0)
        {
            store->entries[e].value = value;
            return 0;
        }
    }

    // If we don't have this guy, find the first avail spot, and set
    for (size_t e = 0; e < KV_ARRAY_LEN(store->entries); e++)
    {
        if (!store->entries[e].set)
        {
            // safe: length validated above; strcpy copies the NUL with it
            strcpy(store->entries[e].name, key);
            store->entries[e].value = value;
            store->entries[e].set = true;
            return 0;
        }
    }

    // If we got this far we out of storage (ENOSPC = container full;
    // ENOMEM would imply a failed allocation, and we never allocate)
    return -ENOSPC;
}

int kv_get(const kv_store_t *store, const char *key, int32_t *value)
{
    if (store == NULL || !store->initialized || key == NULL || value == NULL)
    {
        return -EINVAL;
    }

    for (size_t e = 0; e < KV_ARRAY_LEN(store->entries); e++)
    {
        if (store->entries[e].set && strcmp(store->entries[e].name, key) == 0)
        {
            *value = store->entries[e].value;
            return 0;
        }
    }

    return -ENOENT;
}

// ---- stage 2: YOURS to implement ----

int kv_delete(kv_store_t *store, const char *key)
{
    // TODO: validate -> find the live entry -> free its slot. -ENOENT if missing.
    (void)store;
    (void)key;
    return -ENOSYS;
}

size_t kv_count(const kv_store_t *store)
{
    // TODO: how many live entries? (what should a NULL store return, and why?)
    (void)store;
    return 0;
}
