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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব <stddef.h> /* for offsetof() */

#অন্তর্ভাব "hash.h"
#অন্তর্ভাব "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


প্রকারনির্দেশ size_t (*hash_function)(ধ্রুব শূন্য *চাবি,
                                size_t চাবি_length,
                                size_t slots_অঙ্ক);

প্রকারনির্দেশ size_t (*comp_function)(ধ্রুব শূন্য *key1,
                                size_t key1_len,
                                ধ্রুব শূন্য *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
প্রকারনির্দেশ বর্ণ hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

সংরচনা hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
স্থির size_t hash_seed;

CURLcode Curl_hash_global_init(শূন্য)
{
  স্থির bool seeded;
  unsigned বর্ণ buf[পরিমাণ(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  যদি(seeded)
    প্রত্যাগম CURLE_OK;

  result = Curl_rand_bytes(রিক্ত,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, পরিমাণ(buf));
  যদি(result)
    প্রত্যাগম result;

  প্রতি(i = 0; i < পরিমাণ(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  প্রত্যাগম CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ধ্রুব শূন্য *চাবি, size_t চাবি_length,
                                size_t slots_অঙ্ক, size_t seed);
UNITTEST size_t hash_str_seeded(ধ্রুব শূন্য *চাবি, size_t চাবি_length,
                                size_t slots_অঙ্ক, size_t seed)
{
  ধ্রুব বর্ণ *চাবি_str = (ধ্রুব বর্ণ *)চাবি;
  ধ্রুব বর্ণ *end = চাবি_str + চাবি_length;
  size_t h = 5381 ^ seed;

  যাবত্(চাবি_str < end) {
    size_t j = (size_t)*চাবি_str++;
    h += h << 5;
    h ^= j;
  }

  প্রত্যাগম (h % slots_অঙ্ক);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ধ্রুব শূন্য *চাবি, size_t চাবি_length, size_t slots_অঙ্ক);
UNITTEST size_t hash_str(ধ্রুব শূন্য *চাবি, size_t চাবি_length, size_t slots_অঙ্ক)
{
  প্রত্যাগম hash_str_seeded(চাবি, চাবি_length, slots_অঙ্ক, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
স্থির বর্ণ *hash_elem_চাবি(সংরচনা Curl_hash_element *he)
{
  প্রত্যাগম (বর্ণ *)he + offsetof(সংরচনা Curl_hash_element, চাবি);
}

স্থির size_t hash_socket(ধ্রুব শূন্য *চাবি, size_t চাবি_length,
                          size_t slots_অঙ্ক)
{
  curl_socket_t socket;

  DEBUGASSERT(চাবি_length == পরিমাণ(socket));
  যদি(চাবি_length != পরিমাণ(socket))
    প্রত্যাগম 0;
  memcpy(&socket, চাবি, পরিমাণ(socket));
  প্রত্যাগম (size_t)(socket % (curl_socket_t)slots_অঙ্ক);
}

স্থির size_t compare_socket(ধ্রুব শূন্য *key1, size_t key1_len,
                             ধ্রুব শূন্য *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  যদি((key1_len != পরিমাণ(socket1)) || (key2_len != পরিমাণ(socket2)))
    প্রত্যাগম 0;
  memcpy(&socket1, key1, পরিমাণ(socket1));
  memcpy(&socket2, key2, পরিমাণ(socket2));
  প্রত্যাগম socket1 == socket2;
}

স্থির size_t compare_bytes(ধ্রুব শূন্য *key1, size_t key1_len,
                            ধ্রুব শূন্য *key2, size_t key2_len)
{
  প্রত্যাগম (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

স্থির ধ্রুব সংরচনা hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

স্থির ধ্রুব সংরচনা hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  যদি((unsigned পূর্ণাঙ্ক)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  প্রত্যাগম &hash_functions[type];
}

স্থির bool hash_চাবি_is_valid(uint8_t type, size_t চাবি_len)
{
  প্রত্যাগম (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (চাবি_len == পরিমাণ(curl_socket_t)));
}

#যদি 0 /* useful function for debugging hashes and their contents */
শূন্য Curl_hash_print(সংরচনা Curl_hash *h, শূন্য (*func)(শূন্য *))
{
  সংরচনা Curl_hash_iterator iter;
  সংরচনা Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  যদি(!h)
    প্রত্যাগম;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  যাবত্(he) {
    যদি(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (পূর্ণাঙ্ক)iter.slot_index);
      যদি(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    যদি(func)
      func(he->ptr);
    অন্যথা
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (পূর্ণাঙ্ক)he->চাবি_len, hash_elem_চাবি(he),
                    (শূন্য *)he, (শূন্য *)he->ptr);

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
শূন্য Curl_hash_init(সংরচনা Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned পূর্ণাঙ্ক)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = রিক্ত;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

স্থির সংরচনা Curl_hash_element *hash_elem_create(ধ্রুব শূন্য *চাবি,
                                                  size_t চাবি_len,
                                                  শূন্য *p,
                                                  Curl_hash_elem_dtor dtor)
{
  সংরচনা Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(পরিমাণ(সংরচনা Curl_hash_element) + চাবি_len);
  যদি(he) {
    he->next = রিক্ত;
    /* copy the key */
    memcpy(hash_elem_চাবি(he), চাবি, চাবি_len);
    he->চাবি_len = চাবি_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  প্রত্যাগম he;
}

স্থির শূন্য hash_elem_clear_ptr(সংরচনা Curl_hash *h,
                                সংরচনা Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  যদি(he->ptr) {
    যদি(he->dtor)
      he->dtor(hash_elem_চাবি(he), he->চাবি_len, he->ptr);
    অন্যথা
      h->dtor(he->ptr);
    he->ptr = রিক্ত;
  }
}

স্থির শূন্য hash_elem_destroy(সংরচনা Curl_hash *h,
                              সংরচনা Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

স্থির শূন্য hash_elem_unlink(সংরচনা Curl_hash *h,
                             সংরচনা Curl_hash_element **he_anchor,
                             সংরচনা Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

স্থির শূন্য hash_elem_link(সংরচনা Curl_hash *h,
                           সংরচনা Curl_hash_element **he_anchor,
                           সংরচনা Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

শূন্য *Curl_hash_add2(সংরচনা Curl_hash *h,
                     ধ্রুব শূন্য *চাবি, size_t চাবি_len, শূন্য *p,
                     Curl_hash_elem_dtor dtor)
{
  ধ্রুব সংরচনা hash_functions *functions;
  সংরচনা Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  যদি(!hash_চাবি_is_valid(h->type, চাবি_len))
    প্রত্যাগম রিক্ত;
  যদি(!h->table) {
    h->table = curlx_calloc(h->slots, পরিমাণ(সংরচনা Curl_hash_element *));
    যদি(!h->table)
      প্রত্যাগম রিক্ত; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(চাবি, চাবি_len, h->slots)];
  প্রতি(he = *slot; he; he = he->next) {
    যদি(functions->compare(hash_elem_চাবি(he), he->চাবি_len, চাবি, চাবি_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      প্রত্যাগম p;
    }
  }

  he = hash_elem_create(চাবি, চাবি_len, p, dtor);
  যদি(!he)
    প্রত্যাগম রিক্ত; /* OOM */

  hash_elem_link(h, slot, he);
  প্রত্যাগম p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
শূন্য *Curl_hash_add(সংরচনা Curl_hash *h,
                    ধ্রুব শূন্য *চাবি, size_t চাবি_len, শূন্য *p)
{
  প্রত্যাগম Curl_hash_add2(h, চাবি, চাবি_len, p, রিক্ত);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
পূর্ণাঙ্ক Curl_hash_delete(সংরচনা Curl_hash *h, ধ্রুব শূন্য *চাবি, size_t চাবি_len)
{
  ধ্রুব সংরচনা hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  যদি(!hash_চাবি_is_valid(h->type, চাবি_len))
    প্রত্যাগম 1;
  যদি(h->table) {
    সংরচনা Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(চাবি, চাবি_len, h->slots)];
    যাবত্(*he_anchor) {
      he = *he_anchor;
      যদি(functions->compare(hash_elem_চাবি(he), he->চাবি_len, চাবি, চাবি_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        প্রত্যাগম 0;
      }
      he_anchor = &he->next;
    }
  }
  প্রত্যাগম 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
শূন্য *Curl_hash_pick(সংরচনা Curl_hash *h, ধ্রুব শূন্য *চাবি, size_t চাবি_len)
{
  ধ্রুব সংরচনা hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  যদি(!hash_চাবি_is_valid(h->type, চাবি_len))
    প্রত্যাগম রিক্ত;
  যদি(h->table) {
    সংরচনা Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(চাবি, চাবি_len, h->slots)];
    যাবত্(he) {
      যদি(functions->compare(hash_elem_চাবি(he), he->চাবি_len, চাবি, চাবি_len)) {
        প্রত্যাগম he->ptr;
      }
      he = he->next;
    }
  }
  প্রত্যাগম রিক্ত;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
শূন্য Curl_hash_destroy(সংরচনা Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  যদি(h->table) {
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
শূন্য Curl_hash_clean(সংরচনা Curl_hash *h)
{
  যদি(h && h->table) {
    সংরচনা Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    প্রতি(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      যাবত্(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_গুনতি(সংরচনা Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  প্রত্যাগম h->size;
}

/* Cleans all entries that pass the comp function criteria. */
শূন্য Curl_hash_clean_with_criterium(সংরচনা Curl_hash *h, শূন্য *user,
                                    পূর্ণাঙ্ক (*comp)(শূন্য *, শূন্য *))
{
  size_t i;

  যদি(!h || !h->table)
    প্রত্যাগম;

  DEBUGASSERT(h->init == HASHINIT);
  প্রতি(i = 0; i < h->slots; ++i) {
    সংরচনা Curl_hash_element *he, **he_anchor = &h->table[i];
    যাবত্(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      যদি(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      অন্যথা
        he_anchor = &(*he_anchor)->next;
    }
  }
}

শূন্য Curl_hash_start_iterate(সংরচনা Curl_hash *hash,
                             সংরচনা Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = রিক্ত;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

সংরচনা Curl_hash_element *Curl_hash_next_element(
  সংরচনা Curl_hash_iterator *iter)
{
  সংরচনা Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  যদি(!h->table)
    প্রত্যাগম রিক্ত; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  যদি(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  যদি(!iter->current) {
    size_t i;
    প্রতি(i = iter->slot_index; i < h->slots; i++) {
      যদি(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        ভঙ্গ;
      }
    }
  }

  প্রত্যাগম iter->current;
}
