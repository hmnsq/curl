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
#ضمن "curl_setup.h"

#ضمن <stddef.h> /* for offsetof() */

#ضمن "hash.h"
#ضمن "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


تعريفنوع size_t (*hash_function)(ثابت فراغ *key,
                                size_t key_length,
                                size_t slots_num);

تعريفنوع size_t (*comp_function)(ثابت فراغ *key1,
                                size_t key1_len,
                                ثابت فراغ *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
تعريفنوع حرف hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

بنية hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
ساكن size_t hash_seed;

CURLcode Curl_hash_global_init(فراغ)
{
  ساكن bool seeded;
  unsigned حرف buf[حجم(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  ܐܢ(seeded)
    ܦܢܐ CURLE_OK;

  result = Curl_rand_bytes(عدم,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, حجم(buf));
  ܐܢ(result)
    ܦܢܐ result;

  لكل(i = 0; i < حجم(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  ܦܢܐ CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ثابت فراغ *key, size_t key_length,
                                size_t slots_num, size_t seed);
UNITTEST size_t hash_str_seeded(ثابت فراغ *key, size_t key_length,
                                size_t slots_num, size_t seed)
{
  ثابت حرف *key_str = (ثابت حرف *)key;
  ثابت حرف *end = key_str + key_length;
  size_t h = 5381 ^ seed;

  ܟܡܐ(key_str < end) {
    size_t j = (size_t)*key_str++;
    h += h << 5;
    h ^= j;
  }

  ܦܢܐ (h % slots_num);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ثابت فراغ *key, size_t key_length, size_t slots_num);
UNITTEST size_t hash_str(ثابت فراغ *key, size_t key_length, size_t slots_num)
{
  ܦܢܐ hash_str_seeded(key, key_length, slots_num, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ساكن حرف *hash_elem_key(بنية Curl_hash_element *he)
{
  ܦܢܐ (حرف *)he + offsetof(بنية Curl_hash_element, key);
}

ساكن size_t hash_socket(ثابت فراغ *key, size_t key_length,
                          size_t slots_num)
{
  curl_socket_t socket;

  DEBUGASSERT(key_length == حجم(socket));
  ܐܢ(key_length != حجم(socket))
    ܦܢܐ 0;
  memcpy(&socket, key, حجم(socket));
  ܦܢܐ (size_t)(socket % (curl_socket_t)slots_num);
}

ساكن size_t compare_socket(ثابت فراغ *key1, size_t key1_len,
                             ثابت فراغ *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  ܐܢ((key1_len != حجم(socket1)) || (key2_len != حجم(socket2)))
    ܦܢܐ 0;
  memcpy(&socket1, key1, حجم(socket1));
  memcpy(&socket2, key2, حجم(socket2));
  ܦܢܐ socket1 == socket2;
}

ساكن size_t compare_bytes(ثابت فراغ *key1, size_t key1_len,
                            ثابت فراغ *key2, size_t key2_len)
{
  ܦܢܐ (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ساكن ثابت بنية hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

ساكن ثابت بنية hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  ܐܢ((unsigned صحيح)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  ܦܢܐ &hash_functions[type];
}

ساكن bool hash_key_is_valid(uint8_t type, size_t key_len)
{
  ܦܢܐ (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (key_len == حجم(curl_socket_t)));
}

#ܐܢ 0 /* useful function for debugging hashes and their contents */
فراغ Curl_hash_print(بنية Curl_hash *h, فراغ (*func)(فراغ *))
{
  بنية Curl_hash_iterator iter;
  بنية Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  ܐܢ(!h)
    ܦܢܐ;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  ܟܡܐ(he) {
    ܐܢ(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (صحيح)iter.slot_index);
      ܐܢ(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    ܐܢ(func)
      func(he->ptr);
    ܐܚܪܢܐ
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (صحيح)he->key_len, hash_elem_key(he),
                    (فراغ *)he, (فراغ *)he->ptr);

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
فراغ Curl_hash_init(بنية Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned صحيح)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = عدم;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

ساكن بنية Curl_hash_element *hash_elem_create(ثابت فراغ *key,
                                                  size_t key_len,
                                                  فراغ *p,
                                                  Curl_hash_elem_dtor dtor)
{
  بنية Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(حجم(بنية Curl_hash_element) + key_len);
  ܐܢ(he) {
    he->next = عدم;
    /* copy the key */
    memcpy(hash_elem_key(he), key, key_len);
    he->key_len = key_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  ܦܢܐ he;
}

ساكن فراغ hash_elem_clear_ptr(بنية Curl_hash *h,
                                بنية Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  ܐܢ(he->ptr) {
    ܐܢ(he->dtor)
      he->dtor(hash_elem_key(he), he->key_len, he->ptr);
    ܐܚܪܢܐ
      h->dtor(he->ptr);
    he->ptr = عدم;
  }
}

ساكن فراغ hash_elem_destroy(بنية Curl_hash *h,
                              بنية Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

ساكن فراغ hash_elem_unlink(بنية Curl_hash *h,
                             بنية Curl_hash_element **he_anchor,
                             بنية Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

ساكن فراغ hash_elem_link(بنية Curl_hash *h,
                           بنية Curl_hash_element **he_anchor,
                           بنية Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

فراغ *Curl_hash_add2(بنية Curl_hash *h,
                     ثابت فراغ *key, size_t key_len, فراغ *p,
                     Curl_hash_elem_dtor dtor)
{
  ثابت بنية hash_functions *functions;
  بنية Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ܐܢ(!hash_key_is_valid(h->type, key_len))
    ܦܢܐ عدم;
  ܐܢ(!h->table) {
    h->table = curlx_calloc(h->slots, حجم(بنية Curl_hash_element *));
    ܐܢ(!h->table)
      ܦܢܐ عدم; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(key, key_len, h->slots)];
  لكل(he = *slot; he; he = he->next) {
    ܐܢ(functions->compare(hash_elem_key(he), he->key_len, key, key_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      ܦܢܐ p;
    }
  }

  he = hash_elem_create(key, key_len, p, dtor);
  ܐܢ(!he)
    ܦܢܐ عدم; /* OOM */

  hash_elem_link(h, slot, he);
  ܦܢܐ p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
فراغ *Curl_hash_add(بنية Curl_hash *h,
                    ثابت فراغ *key, size_t key_len, فراغ *p)
{
  ܦܢܐ Curl_hash_add2(h, key, key_len, p, عدم);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
صحيح Curl_hash_delete(بنية Curl_hash *h, ثابت فراغ *key, size_t key_len)
{
  ثابت بنية hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ܐܢ(!hash_key_is_valid(h->type, key_len))
    ܦܢܐ 1;
  ܐܢ(h->table) {
    بنية Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(key, key_len, h->slots)];
    ܟܡܐ(*he_anchor) {
      he = *he_anchor;
      ܐܢ(functions->compare(hash_elem_key(he), he->key_len, key, key_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        ܦܢܐ 0;
      }
      he_anchor = &he->next;
    }
  }
  ܦܢܐ 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
فراغ *Curl_hash_pick(بنية Curl_hash *h, ثابت فراغ *key, size_t key_len)
{
  ثابت بنية hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  ܐܢ(!hash_key_is_valid(h->type, key_len))
    ܦܢܐ عدم;
  ܐܢ(h->table) {
    بنية Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(key, key_len, h->slots)];
    ܟܡܐ(he) {
      ܐܢ(functions->compare(hash_elem_key(he), he->key_len, key, key_len)) {
        ܦܢܐ he->ptr;
      }
      he = he->next;
    }
  }
  ܦܢܐ عدم;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
فراغ Curl_hash_destroy(بنية Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ܐܢ(h->table) {
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
فراغ Curl_hash_clean(بنية Curl_hash *h)
{
  ܐܢ(h && h->table) {
    بنية Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    لكل(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      ܟܡܐ(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_count(بنية Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ܦܢܐ h->size;
}

/* Cleans all entries that pass the comp function criteria. */
فراغ Curl_hash_clean_with_criterium(بنية Curl_hash *h, فراغ *user,
                                    صحيح (*comp)(فراغ *, فراغ *))
{
  size_t i;

  ܐܢ(!h || !h->table)
    ܦܢܐ;

  DEBUGASSERT(h->init == HASHINIT);
  لكل(i = 0; i < h->slots; ++i) {
    بنية Curl_hash_element *he, **he_anchor = &h->table[i];
    ܟܡܐ(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      ܐܢ(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      ܐܚܪܢܐ
        he_anchor = &(*he_anchor)->next;
    }
  }
}

فراغ Curl_hash_start_iterate(بنية Curl_hash *hash,
                             بنية Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = عدم;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

بنية Curl_hash_element *Curl_hash_next_element(
  بنية Curl_hash_iterator *iter)
{
  بنية Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  ܐܢ(!h->table)
    ܦܢܐ عدم; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  ܐܢ(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  ܐܢ(!iter->current) {
    size_t i;
    لكل(i = iter->slot_index; i < h->slots; i++) {
      ܐܢ(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        اكسر;
      }
    }
  }

  ܦܢܐ iter->current;
}
