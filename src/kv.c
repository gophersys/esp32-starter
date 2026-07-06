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

    k_mutex_init(&store->lock);   // programmatic init — no K_MUTEX_DEFINE
    store->initialized = true;

    return 0;
}

int kv_set(kv_store_t *store, const char *key, int32_t value)
{
    int ret = -ENOSPC;   // assume full; any hit below overrides

    if (store == NULL || !store->initialized)
    {
        return -EINVAL;
    }

    // NULL check MUST come before strlen (strlen(NULL) = crash)
    if (key == NULL || key[0] == '\0' || strlen(key) >= KV_CFG_MAX_KEY_LEN)
    {
        return -EINVAL;
    }

    /* single-exit from here down: everything below holds the lock, and the
     * ONLY way out is through the unlock at the bottom. This is the
     * discipline that lets early-exit logic coexist with a mutex. */
    k_mutex_lock(&store->lock, K_FOREVER);

    // First, lets see if we have already seen this guy -> overwrite
    for (size_t e = 0; e < KV_ARRAY_LEN(store->entries); e++)
    {
        if (store->entries[e].set && strcmp(store->entries[e].name, key) == 0)
        {
            store->entries[e].value = value;
            ret = 0;
            goto out;
        }
    }

    // If we do not have this guy, find the first avail spot, and set
    for (size_t e = 0; e < KV_ARRAY_LEN(store->entries); e++)
    {
        if (!store->entries[e].set)
        {
            // safe: length validated above; strcpy copies the NUL with it
            strcpy(store->entries[e].name, key);
            store->entries[e].value = value;
            store->entries[e].set = true;
            ret = 0;
            goto out;
        }
    }

out:
    k_mutex_unlock(&store->lock);
    return ret;
}

int kv_get(kv_store_t *store, const char *key, int32_t *value)
{
    int ret = -ENOENT;

    if (store == NULL || !store->initialized || key == NULL || value == NULL)
    {
        return -EINVAL;
    }

    k_mutex_lock(&store->lock, K_FOREVER);
    for (size_t e = 0; e < KV_ARRAY_LEN(store->entries); e++)
    {
        if (store->entries[e].set && strcmp(store->entries[e].name, key) == 0)
        {
            *value = store->entries[e].value;
            ret = 0;
            break;
        }
    }
    k_mutex_unlock(&store->lock);

    return ret;
}

int kv_delete(kv_store_t *store, const char *key)
{
    int ret = -ENOENT;

    if (store == NULL || !store->initialized || key == NULL || key[0] == '\0')
    {
        return -EINVAL;
    }

    k_mutex_lock(&store->lock, K_FOREVER);
    for (size_t e = 0; e < KV_ARRAY_LEN(store->entries); e++)
    {
        if (store->entries[e].set && strcmp(store->entries[e].name, key) == 0)
        {
            // freeing the slot = clearing the flag; name/value bytes may
            // stay — every reader is gated on .set, so stale bytes are
            // inert (and kv_set overwrites them on reuse)
            store->entries[e].set = false;
            ret = 0;
            break;
        }
    }
    k_mutex_unlock(&store->lock);

    return ret;
}

size_t kv_count(kv_store_t *store)
{
    size_t n = 0;

    // size_t cannot carry -EINVAL: a NULL/uninitialized store is defined
    // to simply have zero entries (documented API decision)
    if (store == NULL || !store->initialized)
    {
        return 0;
    }

    k_mutex_lock(&store->lock, K_FOREVER);
    for (size_t e = 0; e < KV_ARRAY_LEN(store->entries); e++)
    {
        if (store->entries[e].set)
        {
            n++;
        }
    }
    k_mutex_unlock(&store->lock);

    return n;
}
