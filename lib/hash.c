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
typedef var_na hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

sa_nracanaa hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
static size_t hash_seed;

CURLcode Curl_hash_global_init(void)
{
  static bool seeded;
  unsigned var_na buf[parimaa_na(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  ប្រសិនបើ(seeded)
    ត្រឡប់ CURLE_OK;

  result = Curl_rand_bytes(NULL,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, parimaa_na(buf));
  ប្រសិនបើ(result)
    ត្រឡប់ result;

  សម្រាប់(i = 0; i < parimaa_na(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  ត្រឡប់ CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(const void *key, size_t key_length,
                                size_t slots_num, size_t seed);
UNITTEST size_t hash_str_seeded(const void *key, size_t key_length,
                                size_t slots_num, size_t seed)
{
  const var_na *key_str = (const var_na *)key;
  const var_na *end = key_str + key_length;
  size_t h = 5381 ^ seed;

  ខណៈពេល(key_str < end) {
    size_t j = (size_t)*key_str++;
    h += h << 5;
    h ^= j;
  }

  ត្រឡប់ (h % slots_num);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(const void *key, size_t key_length, size_t slots_num);
UNITTEST size_t hash_str(const void *key, size_t key_length, size_t slots_num)
{
  ត្រឡប់ hash_str_seeded(key, key_length, slots_num, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
static var_na *hash_elem_key(sa_nracanaa Curl_hash_element *he)
{
  ត្រឡប់ (var_na *)he + offsetof(sa_nracanaa Curl_hash_element, key);
}

static size_t hash_socket(const void *key, size_t key_length,
                          size_t slots_num)
{
  curl_socket_t socket;

  DEBUGASSERT(key_length == parimaa_na(socket));
  ប្រសិនបើ(key_length != parimaa_na(socket))
    ត្រឡប់ 0;
  memcpy(&socket, key, parimaa_na(socket));
  ត្រឡប់ (size_t)(socket % (curl_socket_t)slots_num);
}

static size_t compare_socket(const void *key1, size_t key1_len,
                             const void *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  ប្រសិនបើ((key1_len != parimaa_na(socket1)) || (key2_len != parimaa_na(socket2)))
    ត្រឡប់ 0;
  memcpy(&socket1, key1, parimaa_na(socket1));
  memcpy(&socket2, key2, parimaa_na(socket2));
  ត្រឡប់ socket1 == socket2;
}

static size_t compare_bytes(const void *key1, size_t key1_len,
                            const void *key2, size_t key2_len)
{
  ត្រឡប់ (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

static const sa_nracanaa hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

static const sa_nracanaa hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  ប្រសិនបើ((unsigned ចំនួនគត់)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  ត្រឡប់ &hash_functions[type];
}

static bool hash_key_is_valid(uint8_t type, size_t key_len)
{
  ត្រឡប់ (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (key_len == parimaa_na(curl_socket_t)));
}

#ប្រសិនបើ 0 /* useful function for debugging hashes and their contents */
void Curl_hash_print(sa_nracanaa Curl_hash *h, void (*func)(void *))
{
  sa_nracanaa Curl_hash_iterator iter;
  sa_nracanaa Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  ប្រសិនបើ(!h)
    ត្រឡប់;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  ខណៈពេល(he) {
    ប្រសិនបើ(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (ចំនួនគត់)iter.slot_index);
      ប្រសិនបើ(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    ប្រសិនបើ(func)
      func(he->ptr);
    បើមិនដូច្នេះ
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (ចំនួនគត់)he->key_len, hash_elem_key(he),
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
void Curl_hash_init(sa_nracanaa Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned ចំនួនគត់)type < CURL_HASH_TYPE_LAST);
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

static sa_nracanaa Curl_hash_element *hash_elem_create(const void *key,
                                                  size_t key_len,
                                                  void *p,
                                                  Curl_hash_elem_dtor dtor)
{
  sa_nracanaa Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(parimaa_na(sa_nracanaa Curl_hash_element) + key_len);
  ប្រសិនបើ(he) {
    he->next = NULL;
    /* copy the key */
    memcpy(hash_elem_key(he), key, key_len);
    he->key_len = key_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  ត្រឡប់ he;
}

static void hash_elem_clear_ptr(sa_nracanaa Curl_hash *h,
                                sa_nracanaa Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  ប្រសិនបើ(he->ptr) {
    ប្រសិនបើ(he->dtor)
      he->dtor(hash_elem_key(he), he->key_len, he->ptr);
    បើមិនដូច្នេះ
      h->dtor(he->ptr);
    he->ptr = NULL;
  }
}

static void hash_elem_destroy(sa_nracanaa Curl_hash *h,
                              sa_nracanaa Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

static void hash_elem_unlink(sa_nracanaa Curl_hash *h,
                             sa_nracanaa Curl_hash_element **he_anchor,
                             sa_nracanaa Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

static void hash_elem_link(sa_nracanaa Curl_hash *h,
                           sa_nracanaa Curl_hash_element **he_anchor,
                           sa_nracanaa Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

void *Curl_hash_add2(sa_nracanaa Curl_hash *h,
                     const void *key, size_t key_len, void *p,
                     Curl_hash_elem_dtor dtor)
{
  const sa_nracanaa hash_functions *functions;
  sa_nracanaa Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ប្រសិនបើ(!hash_key_is_valid(h->type, key_len))
    ត្រឡប់ NULL;
  ប្រសិនបើ(!h->table) {
    h->table = curlx_calloc(h->slots, parimaa_na(sa_nracanaa Curl_hash_element *));
    ប្រសិនបើ(!h->table)
      ត្រឡប់ NULL; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(key, key_len, h->slots)];
  សម្រាប់(he = *slot; he; he = he->next) {
    ប្រសិនបើ(functions->compare(hash_elem_key(he), he->key_len, key, key_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      ត្រឡប់ p;
    }
  }

  he = hash_elem_create(key, key_len, p, dtor);
  ប្រសិនបើ(!he)
    ត្រឡប់ NULL; /* OOM */

  hash_elem_link(h, slot, he);
  ត្រឡប់ p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
void *Curl_hash_add(sa_nracanaa Curl_hash *h,
                    const void *key, size_t key_len, void *p)
{
  ត្រឡប់ Curl_hash_add2(h, key, key_len, p, NULL);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
ចំនួនគត់ Curl_hash_delete(sa_nracanaa Curl_hash *h, const void *key, size_t key_len)
{
  const sa_nracanaa hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ប្រសិនបើ(!hash_key_is_valid(h->type, key_len))
    ត្រឡប់ 1;
  ប្រសិនបើ(h->table) {
    sa_nracanaa Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(key, key_len, h->slots)];
    ខណៈពេល(*he_anchor) {
      he = *he_anchor;
      ប្រសិនបើ(functions->compare(hash_elem_key(he), he->key_len, key, key_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        ត្រឡប់ 0;
      }
      he_anchor = &he->next;
    }
  }
  ត្រឡប់ 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
void *Curl_hash_pick(sa_nracanaa Curl_hash *h, const void *key, size_t key_len)
{
  const sa_nracanaa hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  ប្រសិនបើ(!hash_key_is_valid(h->type, key_len))
    ត្រឡប់ NULL;
  ប្រសិនបើ(h->table) {
    sa_nracanaa Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(key, key_len, h->slots)];
    ខណៈពេល(he) {
      ប្រសិនបើ(functions->compare(hash_elem_key(he), he->key_len, key, key_len)) {
        ត្រឡប់ he->ptr;
      }
      he = he->next;
    }
  }
  ត្រឡប់ NULL;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
void Curl_hash_destroy(sa_nracanaa Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ប្រសិនបើ(h->table) {
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
void Curl_hash_clean(sa_nracanaa Curl_hash *h)
{
  ប្រសិនបើ(h && h->table) {
    sa_nracanaa Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    សម្រាប់(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      ខណៈពេល(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_count(sa_nracanaa Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ត្រឡប់ h->size;
}

/* Cleans all entries that pass the comp function criteria. */
void Curl_hash_clean_with_criterium(sa_nracanaa Curl_hash *h, void *user,
                                    ចំនួនគត់ (*comp)(void *, void *))
{
  size_t i;

  ប្រសិនបើ(!h || !h->table)
    ត្រឡប់;

  DEBUGASSERT(h->init == HASHINIT);
  សម្រាប់(i = 0; i < h->slots; ++i) {
    sa_nracanaa Curl_hash_element *he, **he_anchor = &h->table[i];
    ខណៈពេល(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      ប្រសិនបើ(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      បើមិនដូច្នេះ
        he_anchor = &(*he_anchor)->next;
    }
  }
}

void Curl_hash_start_iterate(sa_nracanaa Curl_hash *hash,
                             sa_nracanaa Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = NULL;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

sa_nracanaa Curl_hash_element *Curl_hash_next_element(
  sa_nracanaa Curl_hash_iterator *iter)
{
  sa_nracanaa Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  ប្រសិនបើ(!h->table)
    ត្រឡប់ NULL; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  ប្រសិនបើ(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  ប្រសិនបើ(!iter->current) {
    size_t i;
    សម្រាប់(i = iter->slot_index; i < h->slots; i++) {
      ប្រសិនបើ(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        bha_nga;
      }
    }
  }

  ត្រឡប់ iter->current;
}
