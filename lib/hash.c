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


প্রকারলেখ size_t (*hash_function)(ধ্রুবক শূন্য *চাবি,
                                size_t চাবি_দৈর্ঘ্য,
                                size_t slots_অঙ্ক);

প্রকারলেখ size_t (*comp_function)(ধ্রুবক শূন্য *key1,
                                size_t key1_len,
                                ধ্রুবক শূন্য *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
প্রকারলেখ অক্ষর hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

কাঠামো hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
স্থির size_t hash_seed;

CURLcode Curl_hash_global_init(শূন্য)
{
  স্থির bool seeded;
  unsigned অক্ষর buf[আকার(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode ফলাফল;

  যদি(seeded)
    ফেরত CURLE_OK;

  ফলাফল = Curl_rand_bytes(ফাঁকা,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, আকার(buf));
  যদি(ফলাফল)
    ফেরত ফলাফল;

  জন্য(i = 0; i < আকার(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  ফেরত CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ধ্রুবক শূন্য *চাবি, size_t চাবি_দৈর্ঘ্য,
                                size_t slots_অঙ্ক, size_t seed);
UNITTEST size_t hash_str_seeded(ধ্রুবক শূন্য *চাবি, size_t চাবি_দৈর্ঘ্য,
                                size_t slots_অঙ্ক, size_t seed)
{
  ধ্রুবক অক্ষর *চাবি_str = (ধ্রুবক অক্ষর *)চাবি;
  ধ্রুবক অক্ষর *end = চাবি_str + চাবি_দৈর্ঘ্য;
  size_t h = 5381 ^ seed;

  যখন(চাবি_str < end) {
    size_t j = (size_t)*চাবি_str++;
    h += h << 5;
    h ^= j;
  }

  ফেরত (h % slots_অঙ্ক);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ধ্রুবক শূন্য *চাবি, size_t চাবি_দৈর্ঘ্য, size_t slots_অঙ্ক);
UNITTEST size_t hash_str(ধ্রুবক শূন্য *চাবি, size_t চাবি_দৈর্ঘ্য, size_t slots_অঙ্ক)
{
  ফেরত hash_str_seeded(চাবি, চাবি_দৈর্ঘ্য, slots_অঙ্ক, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
স্থির অক্ষর *hash_elem_চাবি(কাঠামো Curl_hash_element *he)
{
  ফেরত (অক্ষর *)he + offsetof(কাঠামো Curl_hash_element, চাবি);
}

স্থির size_t hash_socket(ধ্রুবক শূন্য *চাবি, size_t চাবি_দৈর্ঘ্য,
                          size_t slots_অঙ্ক)
{
  curl_socket_t socket;

  DEBUGASSERT(চাবি_দৈর্ঘ্য == আকার(socket));
  যদি(চাবি_দৈর্ঘ্য != আকার(socket))
    ফেরত 0;
  memcpy(&socket, চাবি, আকার(socket));
  ফেরত (size_t)(socket % (curl_socket_t)slots_অঙ্ক);
}

স্থির size_t compare_socket(ধ্রুবক শূন্য *key1, size_t key1_len,
                             ধ্রুবক শূন্য *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  যদি((key1_len != আকার(socket1)) || (key2_len != আকার(socket2)))
    ফেরত 0;
  memcpy(&socket1, key1, আকার(socket1));
  memcpy(&socket2, key2, আকার(socket2));
  ফেরত socket1 == socket2;
}

স্থির size_t compare_bytes(ধ্রুবক শূন্য *key1, size_t key1_len,
                            ধ্রুবক শূন্য *key2, size_t key2_len)
{
  ফেরত (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

স্থির ধ্রুবক কাঠামো hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

স্থির ধ্রুবক কাঠামো hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  যদি((unsigned পূর্ণসংখ্যা)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  ফেরত &hash_functions[type];
}

স্থির bool hash_চাবি_is_valid(uint8_t type, size_t চাবি_len)
{
  ফেরত (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (চাবি_len == আকার(curl_socket_t)));
}

#যদি 0 /* useful function for debugging hashes and their contents */
শূন্য Curl_hash_print(কাঠামো Curl_hash *h, শূন্য (*func)(শূন্য *))
{
  কাঠামো Curl_hash_iterator iter;
  কাঠামো Curl_hash_element *he;
  size_t last_সূচক = UINT_MAX;

  যদি(!h)
    ফেরত;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_পরবর্তী_element(&iter);
  যখন(he) {
    যদি(iter.slot_সূচক != last_সূচক) {
      curl_mfprintf(stderr, "index %d:", (পূর্ণসংখ্যা)iter.slot_সূচক);
      যদি(last_সূচক != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_সূচক = iter.slot_সূচক;
    }

    যদি(func)
      func(he->ptr);
    অন্যথা
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (পূর্ণসংখ্যা)he->চাবি_len, hash_elem_চাবি(he),
                    (শূন্য *)he, (শূন্য *)he->ptr);

    he = Curl_hash_পরবর্তী_element(&iter);
  }
  curl_mfprintf(stderr, "\n");
}
#endif

/* Initializes a hash structure.
 *
 * @unittest: 1602
 * @unittest: 1603
 */
শূন্য Curl_hash_init(কাঠামো Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned পূর্ণসংখ্যা)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = ফাঁকা;
  h->dtor = dtor;
  h->আকার_2 = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

স্থির কাঠামো Curl_hash_element *hash_elem_create(ধ্রুবক শূন্য *চাবি,
                                                  size_t চাবি_len,
                                                  শূন্য *p,
                                                  Curl_hash_elem_dtor dtor)
{
  কাঠামো Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(আকার(কাঠামো Curl_hash_element) + চাবি_len);
  যদি(he) {
    he->পরবর্তী = ফাঁকা;
    /* copy the key */
    memcpy(hash_elem_চাবি(he), চাবি, চাবি_len);
    he->চাবি_len = চাবি_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  ফেরত he;
}

স্থির শূন্য hash_elem_clear_ptr(কাঠামো Curl_hash *h,
                                কাঠামো Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  যদি(he->ptr) {
    যদি(he->dtor)
      he->dtor(hash_elem_চাবি(he), he->চাবি_len, he->ptr);
    অন্যথা
      h->dtor(he->ptr);
    he->ptr = ফাঁকা;
  }
}

স্থির শূন্য hash_elem_destroy(কাঠামো Curl_hash *h,
                              কাঠামো Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

স্থির শূন্য hash_elem_unlink(কাঠামো Curl_hash *h,
                             কাঠামো Curl_hash_element **he_anchor,
                             কাঠামো Curl_hash_element *he)
{
  *he_anchor = he->পরবর্তী;
  --h->আকার_2;
}

স্থির শূন্য hash_elem_link(কাঠামো Curl_hash *h,
                           কাঠামো Curl_hash_element **he_anchor,
                           কাঠামো Curl_hash_element *he)
{
  he->পরবর্তী = *he_anchor;
  *he_anchor = he;
  ++h->আকার_2;
}

শূন্য *Curl_hash_add2(কাঠামো Curl_hash *h,
                     ধ্রুবক শূন্য *চাবি, size_t চাবি_len, শূন্য *p,
                     Curl_hash_elem_dtor dtor)
{
  ধ্রুবক কাঠামো hash_functions *functions;
  কাঠামো Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  যদি(!hash_চাবি_is_valid(h->type, চাবি_len))
    ফেরত ফাঁকা;
  যদি(!h->table) {
    h->table = curlx_calloc(h->slots, আকার(কাঠামো Curl_hash_element *));
    যদি(!h->table)
      ফেরত ফাঁকা; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(চাবি, চাবি_len, h->slots)];
  জন্য(he = *slot; he; he = he->পরবর্তী) {
    যদি(functions->compare(hash_elem_চাবি(he), he->চাবি_len, চাবি, চাবি_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      ফেরত p;
    }
  }

  he = hash_elem_create(চাবি, চাবি_len, p, dtor);
  যদি(!he)
    ফেরত ফাঁকা; /* OOM */

  hash_elem_link(h, slot, he);
  ফেরত p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
শূন্য *Curl_hash_add(কাঠামো Curl_hash *h,
                    ধ্রুবক শূন্য *চাবি, size_t চাবি_len, শূন্য *p)
{
  ফেরত Curl_hash_add2(h, চাবি, চাবি_len, p, ফাঁকা);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
পূর্ণসংখ্যা Curl_hash_delete(কাঠামো Curl_hash *h, ধ্রুবক শূন্য *চাবি, size_t চাবি_len)
{
  ধ্রুবক কাঠামো hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  যদি(!hash_চাবি_is_valid(h->type, চাবি_len))
    ফেরত 1;
  যদি(h->table) {
    কাঠামো Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(চাবি, চাবি_len, h->slots)];
    যখন(*he_anchor) {
      he = *he_anchor;
      যদি(functions->compare(hash_elem_চাবি(he), he->চাবি_len, চাবি, চাবি_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        ফেরত 0;
      }
      he_anchor = &he->পরবর্তী;
    }
  }
  ফেরত 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
শূন্য *Curl_hash_pick(কাঠামো Curl_hash *h, ধ্রুবক শূন্য *চাবি, size_t চাবি_len)
{
  ধ্রুবক কাঠামো hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  যদি(!hash_চাবি_is_valid(h->type, চাবি_len))
    ফেরত ফাঁকা;
  যদি(h->table) {
    কাঠামো Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(চাবি, চাবি_len, h->slots)];
    যখন(he) {
      যদি(functions->compare(hash_elem_চাবি(he), he->চাবি_len, চাবি, চাবি_len)) {
        ফেরত he->ptr;
      }
      he = he->পরবর্তী;
    }
  }
  ফেরত ফাঁকা;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
শূন্য Curl_hash_destroy(কাঠামো Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  যদি(h->table) {
    Curl_hash_clean(h);
    curlx_safefree(h->table);
  }
  DEBUGASSERT(h->আকার_2 == 0);
  h->slots = 0;
}

/* Removes all the entries in the given hash.
 *
 * @unittest: 1602
 */
শূন্য Curl_hash_clean(কাঠামো Curl_hash *h)
{
  যদি(h && h->table) {
    কাঠামো Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    জন্য(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      যখন(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_গুনতি(কাঠামো Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ফেরত h->আকার_2;
}

/* Cleans all entries that pass the comp function criteria. */
শূন্য Curl_hash_clean_with_criterium(কাঠামো Curl_hash *h, শূন্য *user,
                                    পূর্ণসংখ্যা (*comp)(শূন্য *, শূন্য *))
{
  size_t i;

  যদি(!h || !h->table)
    ফেরত;

  DEBUGASSERT(h->init == HASHINIT);
  জন্য(i = 0; i < h->slots; ++i) {
    কাঠামো Curl_hash_element *he, **he_anchor = &h->table[i];
    যখন(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      যদি(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      অন্যথা
        he_anchor = &(*he_anchor)->পরবর্তী;
    }
  }
}

শূন্য Curl_hash_start_iterate(কাঠামো Curl_hash *hash,
                             কাঠামো Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_সূচক = 0;
  iter->current = ফাঁকা;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

কাঠামো Curl_hash_element *Curl_hash_পরবর্তী_element(
  কাঠামো Curl_hash_iterator *iter)
{
  কাঠামো Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  যদি(!h->table)
    ফেরত ফাঁকা; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  যদি(iter->current)
    iter->current = iter->current->পরবর্তী;

  /* If we have reached the end of the list, find the next one */
  যদি(!iter->current) {
    size_t i;
    জন্য(i = iter->slot_সূচক; i < h->slots; i++) {
      যদি(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_সূচক = i + 1;
        বিরতি;
      }
    }
  }

  ফেরত iter->current;
}
