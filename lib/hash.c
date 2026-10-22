/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#include "curl_setup.h"

#include <stddef.h> /* for offsetof() */

#include "hash.h"
#include "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


typedef size_t (*hash_function)(const void *key,
                                size_t key_length,
                                size_t slots_num);

typedef size_t (*comp_function)(const void *key1,
                                size_t key1_len,
                                const void *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
typedef char hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

struct hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
static size_t hash_seed;

CURLcode Curl_hash_global_init(void)
{
  static bool seeded;
  unsigned char buf[sizeof(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  만약(seeded)
    반환 CURLE_OK;

  result = Curl_rand_bytes(NULL,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, sizeof(buf));
  만약(result)
    반환 result;

  위해(i = 0; i < sizeof(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  반환 CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(const void *key, size_t key_length,
                                size_t slots_num, size_t seed);
UNITTEST size_t hash_str_seeded(const void *key, size_t key_length,
                                size_t slots_num, size_t seed)
{
  const char *key_str = (const char *)key;
  const char *end = key_str + key_length;
  size_t h = 5381 ^ seed;

  동안(key_str < end) {
    size_t j = (size_t)*key_str++;
    h += h << 5;
    h ^= j;
  }

  반환 (h % slots_num);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(const void *key, size_t key_length, size_t slots_num);
UNITTEST size_t hash_str(const void *key, size_t key_length, size_t slots_num)
{
  반환 hash_str_seeded(key, key_length, slots_num, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
static char *hash_elem_key(struct Curl_hash_element *he)
{
  반환 (char *)he + offsetof(struct Curl_hash_element, key);
}

static size_t hash_socket(const void *key, size_t key_length,
                          size_t slots_num)
{
  curl_socket_t socket;

  DEBUGASSERT(key_length == sizeof(socket));
  만약(key_length != sizeof(socket))
    반환 0;
  memcpy(&socket, key, sizeof(socket));
  반환 (size_t)(socket % (curl_socket_t)slots_num);
}

static size_t compare_socket(const void *key1, size_t key1_len,
                             const void *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  만약((key1_len != sizeof(socket1)) || (key2_len != sizeof(socket2)))
    반환 0;
  memcpy(&socket1, key1, sizeof(socket1));
  memcpy(&socket2, key2, sizeof(socket2));
  반환 socket1 == socket2;
}

static size_t compare_bytes(const void *key1, size_t key1_len,
                            const void *key2, size_t key2_len)
{
  반환 (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

static const struct hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

static const struct hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  만약((unsigned 정수)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  반환 &hash_functions[type];
}

static bool hash_key_is_valid(uint8_t type, size_t key_len)
{
  반환 (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (key_len == sizeof(curl_socket_t)));
}

#만약 0 /* useful function for debugging hashes and their contents */
void Curl_hash_print(struct Curl_hash *h, void (*func)(void *))
{
  struct Curl_hash_iterator iter;
  struct Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  만약(!h)
    반환;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  동안(he) {
    만약(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (정수)iter.slot_index);
      만약(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    만약(func)
      func(he->ptr);
    아니면
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (정수)he->key_len, hash_elem_key(he),
                    (void *)he, (void *)he->ptr);

    he = Curl_hash_next_element(&iter);
  }
  curl_mfprintf(stderr, "\n");
}
#endif

/* Initializes a hash structure.
 *
 * @unittest: 1602
 * @unittest: 1603
 */
void Curl_hash_init(struct Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned 정수)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = NULL;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

static struct Curl_hash_element *hash_elem_create(const void *key,
                                                  size_t key_len,
                                                  void *p,
                                                  Curl_hash_elem_dtor dtor)
{
  struct Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(sizeof(struct Curl_hash_element) + key_len);
  만약(he) {
    he->next = NULL;
    /* copy the key */
    memcpy(hash_elem_key(he), key, key_len);
    he->key_len = key_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  반환 he;
}

static void hash_elem_clear_ptr(struct Curl_hash *h,
                                struct Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  만약(he->ptr) {
    만약(he->dtor)
      he->dtor(hash_elem_key(he), he->key_len, he->ptr);
    아니면
      h->dtor(he->ptr);
    he->ptr = NULL;
  }
}

static void hash_elem_destroy(struct Curl_hash *h,
                              struct Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

static void hash_elem_unlink(struct Curl_hash *h,
                             struct Curl_hash_element **he_anchor,
                             struct Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

static void hash_elem_link(struct Curl_hash *h,
                           struct Curl_hash_element **he_anchor,
                           struct Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

void *Curl_hash_add2(struct Curl_hash *h,
                     const void *key, size_t key_len, void *p,
                     Curl_hash_elem_dtor dtor)
{
  const struct hash_functions *functions;
  struct Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  만약(!hash_key_is_valid(h->type, key_len))
    반환 NULL;
  만약(!h->table) {
    h->table = curlx_calloc(h->slots, sizeof(struct Curl_hash_element *));
    만약(!h->table)
      반환 NULL; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(key, key_len, h->slots)];
  위해(he = *slot; he; he = he->next) {
    만약(functions->compare(hash_elem_key(he), he->key_len, key, key_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      반환 p;
    }
  }

  he = hash_elem_create(key, key_len, p, dtor);
  만약(!he)
    반환 NULL; /* OOM */

  hash_elem_link(h, slot, he);
  반환 p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
void *Curl_hash_add(struct Curl_hash *h,
                    const void *key, size_t key_len, void *p)
{
  반환 Curl_hash_add2(h, key, key_len, p, NULL);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
정수 Curl_hash_delete(struct Curl_hash *h, const void *key, size_t key_len)
{
  const struct hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  만약(!hash_key_is_valid(h->type, key_len))
    반환 1;
  만약(h->table) {
    struct Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(key, key_len, h->slots)];
    동안(*he_anchor) {
      he = *he_anchor;
      만약(functions->compare(hash_elem_key(he), he->key_len, key, key_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        반환 0;
      }
      he_anchor = &he->next;
    }
  }
  반환 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
void *Curl_hash_pick(struct Curl_hash *h, const void *key, size_t key_len)
{
  const struct hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  만약(!hash_key_is_valid(h->type, key_len))
    반환 NULL;
  만약(h->table) {
    struct Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(key, key_len, h->slots)];
    동안(he) {
      만약(functions->compare(hash_elem_key(he), he->key_len, key, key_len)) {
        반환 he->ptr;
      }
      he = he->next;
    }
  }
  반환 NULL;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
void Curl_hash_destroy(struct Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  만약(h->table) {
    Curl_hash_clean(h);
    curlx_safefree(h->table);
  }
  DEBUGASSERT(h->size == 0);
  h->slots = 0;
}

/* Removes all the entries in the given hash.
 *
 * @unittest: 1602
 */
void Curl_hash_clean(struct Curl_hash *h)
{
  만약(h && h->table) {
    struct Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    위해(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      동안(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_count(struct Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  반환 h->size;
}

/* Cleans all entries that pass the comp function criteria. */
void Curl_hash_clean_with_criterium(struct Curl_hash *h, void *user,
                                    정수 (*comp)(void *, void *))
{
  size_t i;

  만약(!h || !h->table)
    반환;

  DEBUGASSERT(h->init == HASHINIT);
  위해(i = 0; i < h->slots; ++i) {
    struct Curl_hash_element *he, **he_anchor = &h->table[i];
    동안(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      만약(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      아니면
        he_anchor = &(*he_anchor)->next;
    }
  }
}

void Curl_hash_start_iterate(struct Curl_hash *hash,
                             struct Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = NULL;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

struct Curl_hash_element *Curl_hash_next_element(
  struct Curl_hash_iterator *iter)
{
  struct Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  만약(!h->table)
    반환 NULL; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  만약(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  만약(!iter->current) {
    size_t i;
    위해(i = iter->slot_index; i < h->slots; i++) {
      만약(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        break;
      }
    }
  }

  반환 iter->current;
}
