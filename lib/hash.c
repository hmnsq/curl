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
#شامل "curl_setup.h"

#شامل <stddef.h> /* for offsetof() */

#شامل "hash.h"
#شامل "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


تعریفنوع size_t (*hash_function)(ثابت تهی *کلید,
                                size_t کلید_length,
                                size_t slots_رقم);

تعریفنوع size_t (*comp_function)(ثابت تهی *key1,
                                size_t key1_len,
                                ثابت تهی *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
تعریفنوع نویسه hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

ساختار hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
ایستا size_t hash_seed;

CURLcode Curl_hash_global_init(تهی)
{
  ایستا bool seeded;
  unsigned نویسه buf[اندازه(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  اگر(seeded)
    بازگشت CURLE_OK;

  result = Curl_rand_bytes(پوچ,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, اندازه(buf));
  اگر(result)
    بازگشت result;

  برای(i = 0; i < اندازه(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  بازگشت CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ثابت تهی *کلید, size_t کلید_length,
                                size_t slots_رقم, size_t seed);
UNITTEST size_t hash_str_seeded(ثابت تهی *کلید, size_t کلید_length,
                                size_t slots_رقم, size_t seed)
{
  ثابت نویسه *کلید_str = (ثابت نویسه *)کلید;
  ثابت نویسه *end = کلید_str + کلید_length;
  size_t h = 5381 ^ seed;

  وقتی(کلید_str < end) {
    size_t j = (size_t)*کلید_str++;
    h += h << 5;
    h ^= j;
  }

  بازگشت (h % slots_رقم);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ثابت تهی *کلید, size_t کلید_length, size_t slots_رقم);
UNITTEST size_t hash_str(ثابت تهی *کلید, size_t کلید_length, size_t slots_رقم)
{
  بازگشت hash_str_seeded(کلید, کلید_length, slots_رقم, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ایستا نویسه *hash_elem_کلید(ساختار Curl_hash_element *he)
{
  بازگشت (نویسه *)he + offsetof(ساختار Curl_hash_element, کلید);
}

ایستا size_t hash_socket(ثابت تهی *کلید, size_t کلید_length,
                          size_t slots_رقم)
{
  curl_socket_t socket;

  DEBUGASSERT(کلید_length == اندازه(socket));
  اگر(کلید_length != اندازه(socket))
    بازگشت 0;
  memcpy(&socket, کلید, اندازه(socket));
  بازگشت (size_t)(socket % (curl_socket_t)slots_رقم);
}

ایستا size_t compare_socket(ثابت تهی *key1, size_t key1_len,
                             ثابت تهی *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  اگر((key1_len != اندازه(socket1)) || (key2_len != اندازه(socket2)))
    بازگشت 0;
  memcpy(&socket1, key1, اندازه(socket1));
  memcpy(&socket2, key2, اندازه(socket2));
  بازگشت socket1 == socket2;
}

ایستا size_t compare_bytes(ثابت تهی *key1, size_t key1_len,
                            ثابت تهی *key2, size_t key2_len)
{
  بازگشت (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ایستا ثابت ساختار hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

ایستا ثابت ساختار hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  اگر((unsigned صحیح)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  بازگشت &hash_functions[type];
}

ایستا bool hash_کلید_is_valid(uint8_t type, size_t کلید_len)
{
  بازگشت (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (کلید_len == اندازه(curl_socket_t)));
}

#اگر 0 /* useful function for debugging hashes and their contents */
تهی Curl_hash_print(ساختار Curl_hash *h, تهی (*func)(تهی *))
{
  ساختار Curl_hash_iterator iter;
  ساختار Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  اگر(!h)
    بازگشت;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  وقتی(he) {
    اگر(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (صحیح)iter.slot_index);
      اگر(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    اگر(func)
      func(he->ptr);
    وگرنه
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (صحیح)he->کلید_len, hash_elem_کلید(he),
                    (تهی *)he, (تهی *)he->ptr);

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
تهی Curl_hash_init(ساختار Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned صحیح)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = پوچ;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

ایستا ساختار Curl_hash_element *hash_elem_create(ثابت تهی *کلید,
                                                  size_t کلید_len,
                                                  تهی *p,
                                                  Curl_hash_elem_dtor dtor)
{
  ساختار Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(اندازه(ساختار Curl_hash_element) + کلید_len);
  اگر(he) {
    he->next = پوچ;
    /* copy the key */
    memcpy(hash_elem_کلید(he), کلید, کلید_len);
    he->کلید_len = کلید_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  بازگشت he;
}

ایستا تهی hash_elem_clear_ptr(ساختار Curl_hash *h,
                                ساختار Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  اگر(he->ptr) {
    اگر(he->dtor)
      he->dtor(hash_elem_کلید(he), he->کلید_len, he->ptr);
    وگرنه
      h->dtor(he->ptr);
    he->ptr = پوچ;
  }
}

ایستا تهی hash_elem_destroy(ساختار Curl_hash *h,
                              ساختار Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

ایستا تهی hash_elem_unlink(ساختار Curl_hash *h,
                             ساختار Curl_hash_element **he_anchor,
                             ساختار Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

ایستا تهی hash_elem_link(ساختار Curl_hash *h,
                           ساختار Curl_hash_element **he_anchor,
                           ساختار Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

تهی *Curl_hash_add2(ساختار Curl_hash *h,
                     ثابت تهی *کلید, size_t کلید_len, تهی *p,
                     Curl_hash_elem_dtor dtor)
{
  ثابت ساختار hash_functions *functions;
  ساختار Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  اگر(!hash_کلید_is_valid(h->type, کلید_len))
    بازگشت پوچ;
  اگر(!h->table) {
    h->table = curlx_calloc(h->slots, اندازه(ساختار Curl_hash_element *));
    اگر(!h->table)
      بازگشت پوچ; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(کلید, کلید_len, h->slots)];
  برای(he = *slot; he; he = he->next) {
    اگر(functions->compare(hash_elem_کلید(he), he->کلید_len, کلید, کلید_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      بازگشت p;
    }
  }

  he = hash_elem_create(کلید, کلید_len, p, dtor);
  اگر(!he)
    بازگشت پوچ; /* OOM */

  hash_elem_link(h, slot, he);
  بازگشت p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
تهی *Curl_hash_add(ساختار Curl_hash *h,
                    ثابت تهی *کلید, size_t کلید_len, تهی *p)
{
  بازگشت Curl_hash_add2(h, کلید, کلید_len, p, پوچ);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
صحیح Curl_hash_delete(ساختار Curl_hash *h, ثابت تهی *کلید, size_t کلید_len)
{
  ثابت ساختار hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  اگر(!hash_کلید_is_valid(h->type, کلید_len))
    بازگشت 1;
  اگر(h->table) {
    ساختار Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(کلید, کلید_len, h->slots)];
    وقتی(*he_anchor) {
      he = *he_anchor;
      اگر(functions->compare(hash_elem_کلید(he), he->کلید_len, کلید, کلید_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        بازگشت 0;
      }
      he_anchor = &he->next;
    }
  }
  بازگشت 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
تهی *Curl_hash_pick(ساختار Curl_hash *h, ثابت تهی *کلید, size_t کلید_len)
{
  ثابت ساختار hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  اگر(!hash_کلید_is_valid(h->type, کلید_len))
    بازگشت پوچ;
  اگر(h->table) {
    ساختار Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(کلید, کلید_len, h->slots)];
    وقتی(he) {
      اگر(functions->compare(hash_elem_کلید(he), he->کلید_len, کلید, کلید_len)) {
        بازگشت he->ptr;
      }
      he = he->next;
    }
  }
  بازگشت پوچ;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
تهی Curl_hash_destroy(ساختار Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  اگر(h->table) {
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
تهی Curl_hash_clean(ساختار Curl_hash *h)
{
  اگر(h && h->table) {
    ساختار Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    برای(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      وقتی(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_شمار(ساختار Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  بازگشت h->size;
}

/* Cleans all entries that pass the comp function criteria. */
تهی Curl_hash_clean_with_criterium(ساختار Curl_hash *h, تهی *user,
                                    صحیح (*comp)(تهی *, تهی *))
{
  size_t i;

  اگر(!h || !h->table)
    بازگشت;

  DEBUGASSERT(h->init == HASHINIT);
  برای(i = 0; i < h->slots; ++i) {
    ساختار Curl_hash_element *he, **he_anchor = &h->table[i];
    وقتی(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      اگر(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      وگرنه
        he_anchor = &(*he_anchor)->next;
    }
  }
}

تهی Curl_hash_start_iterate(ساختار Curl_hash *hash,
                             ساختار Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = پوچ;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

ساختار Curl_hash_element *Curl_hash_next_element(
  ساختار Curl_hash_iterator *iter)
{
  ساختار Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  اگر(!h->table)
    بازگشت پوچ; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  اگر(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  اگر(!iter->current) {
    size_t i;
    برای(i = iter->slot_index; i < h->slots; i++) {
      اگر(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        بشکن;
      }
    }
  }

  بازگشت iter->current;
}
