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


تعريفنوع size_t (*hash_function)(ثابت فراغ *مفتاح,
                                size_t مفتاح_length,
                                size_t slots_رقم);

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

  إذا(seeded)
    إرجاع CURLE_OK;

  result = Curl_rand_bytes(عدم,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, حجم(buf));
  إذا(result)
    إرجاع result;

  لكل(i = 0; i < حجم(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  إرجاع CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ثابت فراغ *مفتاح, size_t مفتاح_length,
                                size_t slots_رقم, size_t seed);
UNITTEST size_t hash_str_seeded(ثابت فراغ *مفتاح, size_t مفتاح_length,
                                size_t slots_رقم, size_t seed)
{
  ثابت حرف *مفتاح_str = (ثابت حرف *)مفتاح;
  ثابت حرف *end = مفتاح_str + مفتاح_length;
  size_t h = 5381 ^ seed;

  بينما(مفتاح_str < end) {
    size_t j = (size_t)*مفتاح_str++;
    h += h << 5;
    h ^= j;
  }

  إرجاع (h % slots_رقم);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ثابت فراغ *مفتاح, size_t مفتاح_length, size_t slots_رقم);
UNITTEST size_t hash_str(ثابت فراغ *مفتاح, size_t مفتاح_length, size_t slots_رقم)
{
  إرجاع hash_str_seeded(مفتاح, مفتاح_length, slots_رقم, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ساكن حرف *hash_elem_مفتاح(بنية Curl_hash_element *he)
{
  إرجاع (حرف *)he + offsetof(بنية Curl_hash_element, مفتاح);
}

ساكن size_t hash_socket(ثابت فراغ *مفتاح, size_t مفتاح_length,
                          size_t slots_رقم)
{
  curl_socket_t socket;

  DEBUGASSERT(مفتاح_length == حجم(socket));
  إذا(مفتاح_length != حجم(socket))
    إرجاع 0;
  memcpy(&socket, مفتاح, حجم(socket));
  إرجاع (size_t)(socket % (curl_socket_t)slots_رقم);
}

ساكن size_t compare_socket(ثابت فراغ *key1, size_t key1_len,
                             ثابت فراغ *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  إذا((key1_len != حجم(socket1)) || (key2_len != حجم(socket2)))
    إرجاع 0;
  memcpy(&socket1, key1, حجم(socket1));
  memcpy(&socket2, key2, حجم(socket2));
  إرجاع socket1 == socket2;
}

ساكن size_t compare_bytes(ثابت فراغ *key1, size_t key1_len,
                            ثابت فراغ *key2, size_t key2_len)
{
  إرجاع (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ساكن ثابت بنية hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

ساكن ثابت بنية hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  إذا((unsigned صحيح)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  إرجاع &hash_functions[type];
}

ساكن bool hash_مفتاح_is_valid(uint8_t type, size_t مفتاح_len)
{
  إرجاع (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (مفتاح_len == حجم(curl_socket_t)));
}

#إذا 0 /* useful function for debugging hashes and their contents */
فراغ Curl_hash_print(بنية Curl_hash *h, فراغ (*func)(فراغ *))
{
  بنية Curl_hash_iterator iter;
  بنية Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  إذا(!h)
    إرجاع;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  بينما(he) {
    إذا(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (صحيح)iter.slot_index);
      إذا(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    إذا(func)
      func(he->ptr);
    وإلا
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (صحيح)he->مفتاح_len, hash_elem_مفتاح(he),
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

ساكن بنية Curl_hash_element *hash_elem_create(ثابت فراغ *مفتاح,
                                                  size_t مفتاح_len,
                                                  فراغ *p,
                                                  Curl_hash_elem_dtor dtor)
{
  بنية Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(حجم(بنية Curl_hash_element) + مفتاح_len);
  إذا(he) {
    he->next = عدم;
    /* copy the key */
    memcpy(hash_elem_مفتاح(he), مفتاح, مفتاح_len);
    he->مفتاح_len = مفتاح_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  إرجاع he;
}

ساكن فراغ hash_elem_clear_ptr(بنية Curl_hash *h,
                                بنية Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  إذا(he->ptr) {
    إذا(he->dtor)
      he->dtor(hash_elem_مفتاح(he), he->مفتاح_len, he->ptr);
    وإلا
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
                     ثابت فراغ *مفتاح, size_t مفتاح_len, فراغ *p,
                     Curl_hash_elem_dtor dtor)
{
  ثابت بنية hash_functions *functions;
  بنية Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  إذا(!hash_مفتاح_is_valid(h->type, مفتاح_len))
    إرجاع عدم;
  إذا(!h->table) {
    h->table = curlx_calloc(h->slots, حجم(بنية Curl_hash_element *));
    إذا(!h->table)
      إرجاع عدم; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(مفتاح, مفتاح_len, h->slots)];
  لكل(he = *slot; he; he = he->next) {
    إذا(functions->compare(hash_elem_مفتاح(he), he->مفتاح_len, مفتاح, مفتاح_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      إرجاع p;
    }
  }

  he = hash_elem_create(مفتاح, مفتاح_len, p, dtor);
  إذا(!he)
    إرجاع عدم; /* OOM */

  hash_elem_link(h, slot, he);
  إرجاع p; /* return the new entry */
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
                    ثابت فراغ *مفتاح, size_t مفتاح_len, فراغ *p)
{
  إرجاع Curl_hash_add2(h, مفتاح, مفتاح_len, p, عدم);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
صحيح Curl_hash_delete(بنية Curl_hash *h, ثابت فراغ *مفتاح, size_t مفتاح_len)
{
  ثابت بنية hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  إذا(!hash_مفتاح_is_valid(h->type, مفتاح_len))
    إرجاع 1;
  إذا(h->table) {
    بنية Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(مفتاح, مفتاح_len, h->slots)];
    بينما(*he_anchor) {
      he = *he_anchor;
      إذا(functions->compare(hash_elem_مفتاح(he), he->مفتاح_len, مفتاح, مفتاح_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        إرجاع 0;
      }
      he_anchor = &he->next;
    }
  }
  إرجاع 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
فراغ *Curl_hash_pick(بنية Curl_hash *h, ثابت فراغ *مفتاح, size_t مفتاح_len)
{
  ثابت بنية hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  إذا(!hash_مفتاح_is_valid(h->type, مفتاح_len))
    إرجاع عدم;
  إذا(h->table) {
    بنية Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(مفتاح, مفتاح_len, h->slots)];
    بينما(he) {
      إذا(functions->compare(hash_elem_مفتاح(he), he->مفتاح_len, مفتاح, مفتاح_len)) {
        إرجاع he->ptr;
      }
      he = he->next;
    }
  }
  إرجاع عدم;
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
  إذا(h->table) {
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
  إذا(h && h->table) {
    بنية Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    لكل(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      بينما(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_تعداد(بنية Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  إرجاع h->size;
}

/* Cleans all entries that pass the comp function criteria. */
فراغ Curl_hash_clean_with_criterium(بنية Curl_hash *h, فراغ *user,
                                    صحيح (*comp)(فراغ *, فراغ *))
{
  size_t i;

  إذا(!h || !h->table)
    إرجاع;

  DEBUGASSERT(h->init == HASHINIT);
  لكل(i = 0; i < h->slots; ++i) {
    بنية Curl_hash_element *he, **he_anchor = &h->table[i];
    بينما(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      إذا(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      وإلا
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
  إذا(!h->table)
    إرجاع عدم; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  إذا(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  إذا(!iter->current) {
    size_t i;
    لكل(i = iter->slot_index; i < h->slots; i++) {
      إذا(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        اكسر;
      }
    }
  }

  إرجاع iter->current;
}
