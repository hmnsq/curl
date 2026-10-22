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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ <stddef.h> /* for offsetof() */

#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


ಪ್ರಕಾರನಿರ್ದೇಶ size_t (*hash_function)(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ,
                                size_t ಕೀಲಿ_length,
                                size_t slots_ಅಂಕೆ);

ಪ್ರಕಾರನಿರ್ದೇಶ size_t (*comp_function)(ಧ್ರುವ ಶೂನ್ಯ *key1,
                                size_t key1_len,
                                ಧ್ರುವ ಶೂನ್ಯ *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
ಪ್ರಕಾರನಿರ್ದೇಶ ವರ್ಣ hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

ಸಂರಚನಾ hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
ಸ್ಥಿರ size_t hash_seed;

CURLcode Curl_hash_global_init(ಶೂನ್ಯ)
{
  ಸ್ಥಿರ bool seeded;
  unsigned ವರ್ಣ buf[ಪರಿಮಾಣ(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  ಯದಿ(seeded)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  result = Curl_rand_bytes(ರಿಕ್ತ,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, ಪರಿಮಾಣ(buf));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಪ್ರತಿ(i = 0; i < ಪರಿಮಾಣ(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_length,
                                size_t slots_ಅಂಕೆ, size_t seed);
UNITTEST size_t hash_str_seeded(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_length,
                                size_t slots_ಅಂಕೆ, size_t seed)
{
  ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_str = (ಧ್ರುವ ವರ್ಣ *)ಕೀಲಿ;
  ಧ್ರುವ ವರ್ಣ *end = ಕೀಲಿ_str + ಕೀಲಿ_length;
  size_t h = 5381 ^ seed;

  ಯಾವತ್(ಕೀಲಿ_str < end) {
    size_t j = (size_t)*ಕೀಲಿ_str++;
    h += h << 5;
    h ^= j;
  }

  ಪ್ರತ್ಯಾಗಮ (h % slots_ಅಂಕೆ);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_length, size_t slots_ಅಂಕೆ);
UNITTEST size_t hash_str(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_length, size_t slots_ಅಂಕೆ)
{
  ಪ್ರತ್ಯಾಗಮ hash_str_seeded(ಕೀಲಿ, ಕೀಲಿ_length, slots_ಅಂಕೆ, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ಸ್ಥಿರ ವರ್ಣ *hash_elem_ಕೀಲಿ(ಸಂರಚನಾ Curl_hash_element *he)
{
  ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)he + offsetof(ಸಂರಚನಾ Curl_hash_element, ಕೀಲಿ);
}

ಸ್ಥಿರ size_t hash_socket(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_length,
                          size_t slots_ಅಂಕೆ)
{
  curl_socket_t socket;

  DEBUGASSERT(ಕೀಲಿ_length == ಪರಿಮಾಣ(socket));
  ಯದಿ(ಕೀಲಿ_length != ಪರಿಮಾಣ(socket))
    ಪ್ರತ್ಯಾಗಮ 0;
  memcpy(&socket, ಕೀಲಿ, ಪರಿಮಾಣ(socket));
  ಪ್ರತ್ಯಾಗಮ (size_t)(socket % (curl_socket_t)slots_ಅಂಕೆ);
}

ಸ್ಥಿರ size_t compare_socket(ಧ್ರುವ ಶೂನ್ಯ *key1, size_t key1_len,
                             ಧ್ರುವ ಶೂನ್ಯ *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  ಯದಿ((key1_len != ಪರಿಮಾಣ(socket1)) || (key2_len != ಪರಿಮಾಣ(socket2)))
    ಪ್ರತ್ಯಾಗಮ 0;
  memcpy(&socket1, key1, ಪರಿಮಾಣ(socket1));
  memcpy(&socket2, key2, ಪರಿಮಾಣ(socket2));
  ಪ್ರತ್ಯಾಗಮ socket1 == socket2;
}

ಸ್ಥಿರ size_t compare_bytes(ಧ್ರುವ ಶೂನ್ಯ *key1, size_t key1_len,
                            ಧ್ರುವ ಶೂನ್ಯ *key2, size_t key2_len)
{
  ಪ್ರತ್ಯಾಗಮ (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  ಯದಿ((unsigned ಪೂರ್ಣಾಙ್ಕ)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  ಪ್ರತ್ಯಾಗಮ &hash_functions[type];
}

ಸ್ಥಿರ bool hash_ಕೀಲಿ_is_valid(uint8_t type, size_t ಕೀಲಿ_len)
{
  ಪ್ರತ್ಯಾಗಮ (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (ಕೀಲಿ_len == ಪರಿಮಾಣ(curl_socket_t)));
}

#ಯದಿ 0 /* useful function for debugging hashes and their contents */
ಶೂನ್ಯ Curl_hash_print(ಸಂರಚನಾ Curl_hash *h, ಶೂನ್ಯ (*func)(ಶೂನ್ಯ *))
{
  ಸಂರಚನಾ Curl_hash_iterator iter;
  ಸಂರಚನಾ Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  ಯದಿ(!h)
    ಪ್ರತ್ಯಾಗಮ;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  ಯಾವತ್(he) {
    ಯದಿ(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (ಪೂರ್ಣಾಙ್ಕ)iter.slot_index);
      ಯದಿ(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    ಯದಿ(func)
      func(he->ptr);
    ಅನ್ಯಥಾ
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (ಪೂರ್ಣಾಙ್ಕ)he->ಕೀಲಿ_len, hash_elem_ಕೀಲಿ(he),
                    (ಶೂನ್ಯ *)he, (ಶೂನ್ಯ *)he->ptr);

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
ಶೂನ್ಯ Curl_hash_init(ಸಂರಚನಾ Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned ಪೂರ್ಣಾಙ್ಕ)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = ರಿಕ್ತ;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_hash_element *hash_elem_create(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ,
                                                  size_t ಕೀಲಿ_len,
                                                  ಶೂನ್ಯ *p,
                                                  Curl_hash_elem_dtor dtor)
{
  ಸಂರಚನಾ Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_hash_element) + ಕೀಲಿ_len);
  ಯದಿ(he) {
    he->next = ರಿಕ್ತ;
    /* copy the key */
    memcpy(hash_elem_ಕೀಲಿ(he), ಕೀಲಿ, ಕೀಲಿ_len);
    he->ಕೀಲಿ_len = ಕೀಲಿ_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  ಪ್ರತ್ಯಾಗಮ he;
}

ಸ್ಥಿರ ಶೂನ್ಯ hash_elem_clear_ptr(ಸಂರಚನಾ Curl_hash *h,
                                ಸಂರಚನಾ Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  ಯದಿ(he->ptr) {
    ಯದಿ(he->dtor)
      he->dtor(hash_elem_ಕೀಲಿ(he), he->ಕೀಲಿ_len, he->ptr);
    ಅನ್ಯಥಾ
      h->dtor(he->ptr);
    he->ptr = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ hash_elem_destroy(ಸಂರಚನಾ Curl_hash *h,
                              ಸಂರಚನಾ Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

ಸ್ಥಿರ ಶೂನ್ಯ hash_elem_unlink(ಸಂರಚನಾ Curl_hash *h,
                             ಸಂರಚನಾ Curl_hash_element **he_anchor,
                             ಸಂರಚನಾ Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

ಸ್ಥಿರ ಶೂನ್ಯ hash_elem_link(ಸಂರಚನಾ Curl_hash *h,
                           ಸಂರಚನಾ Curl_hash_element **he_anchor,
                           ಸಂರಚನಾ Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

ಶೂನ್ಯ *Curl_hash_add2(ಸಂರಚನಾ Curl_hash *h,
                     ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p,
                     Curl_hash_elem_dtor dtor)
{
  ಧ್ರುವ ಸಂರಚನಾ hash_functions *functions;
  ಸಂರಚನಾ Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ಯದಿ(!hash_ಕೀಲಿ_is_valid(h->type, ಕೀಲಿ_len))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಯದಿ(!h->table) {
    h->table = curlx_calloc(h->slots, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_hash_element *));
    ಯದಿ(!h->table)
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(ಕೀಲಿ, ಕೀಲಿ_len, h->slots)];
  ಪ್ರತಿ(he = *slot; he; he = he->next) {
    ಯದಿ(functions->compare(hash_elem_ಕೀಲಿ(he), he->ಕೀಲಿ_len, ಕೀಲಿ, ಕೀಲಿ_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      ಪ್ರತ್ಯಾಗಮ p;
    }
  }

  he = hash_elem_create(ಕೀಲಿ, ಕೀಲಿ_len, p, dtor);
  ಯದಿ(!he)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* OOM */

  hash_elem_link(h, slot, he);
  ಪ್ರತ್ಯಾಗಮ p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ಶೂನ್ಯ *Curl_hash_add(ಸಂರಚನಾ Curl_hash *h,
                    ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p)
{
  ಪ್ರತ್ಯಾಗಮ Curl_hash_add2(h, ಕೀಲಿ, ಕೀಲಿ_len, p, ರಿಕ್ತ);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
ಪೂರ್ಣಾಙ್ಕ Curl_hash_delete(ಸಂರಚನಾ Curl_hash *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len)
{
  ಧ್ರುವ ಸಂರಚನಾ hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ಯದಿ(!hash_ಕೀಲಿ_is_valid(h->type, ಕೀಲಿ_len))
    ಪ್ರತ್ಯಾಗಮ 1;
  ಯದಿ(h->table) {
    ಸಂರಚನಾ Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(ಕೀಲಿ, ಕೀಲಿ_len, h->slots)];
    ಯಾವತ್(*he_anchor) {
      he = *he_anchor;
      ಯದಿ(functions->compare(hash_elem_ಕೀಲಿ(he), he->ಕೀಲಿ_len, ಕೀಲಿ, ಕೀಲಿ_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        ಪ್ರತ್ಯಾಗಮ 0;
      }
      he_anchor = &he->next;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
ಶೂನ್ಯ *Curl_hash_pick(ಸಂರಚನಾ Curl_hash *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len)
{
  ಧ್ರುವ ಸಂರಚನಾ hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  ಯದಿ(!hash_ಕೀಲಿ_is_valid(h->type, ಕೀಲಿ_len))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಯದಿ(h->table) {
    ಸಂರಚನಾ Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(ಕೀಲಿ, ಕೀಲಿ_len, h->slots)];
    ಯಾವತ್(he) {
      ಯದಿ(functions->compare(hash_elem_ಕೀಲಿ(he), he->ಕೀಲಿ_len, ಕೀಲಿ, ಕೀಲಿ_len)) {
        ಪ್ರತ್ಯಾಗಮ he->ptr;
      }
      he = he->next;
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ಶೂನ್ಯ Curl_hash_destroy(ಸಂರಚನಾ Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ಯದಿ(h->table) {
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
ಶೂನ್ಯ Curl_hash_clean(ಸಂರಚನಾ Curl_hash *h)
{
  ಯದಿ(h && h->table) {
    ಸಂರಚನಾ Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    ಪ್ರತಿ(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      ಯಾವತ್(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_ಎಣಿಕೆ(ಸಂರಚನಾ Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ಪ್ರತ್ಯಾಗಮ h->size;
}

/* Cleans all entries that pass the comp function criteria. */
ಶೂನ್ಯ Curl_hash_clean_with_criterium(ಸಂರಚನಾ Curl_hash *h, ಶೂನ್ಯ *user,
                                    ಪೂರ್ಣಾಙ್ಕ (*comp)(ಶೂನ್ಯ *, ಶೂನ್ಯ *))
{
  size_t i;

  ಯದಿ(!h || !h->table)
    ಪ್ರತ್ಯಾಗಮ;

  DEBUGASSERT(h->init == HASHINIT);
  ಪ್ರತಿ(i = 0; i < h->slots; ++i) {
    ಸಂರಚನಾ Curl_hash_element *he, **he_anchor = &h->table[i];
    ಯಾವತ್(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      ಯದಿ(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      ಅನ್ಯಥಾ
        he_anchor = &(*he_anchor)->next;
    }
  }
}

ಶೂನ್ಯ Curl_hash_start_iterate(ಸಂರಚನಾ Curl_hash *hash,
                             ಸಂರಚನಾ Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = ರಿಕ್ತ;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

ಸಂರಚನಾ Curl_hash_element *Curl_hash_next_element(
  ಸಂರಚನಾ Curl_hash_iterator *iter)
{
  ಸಂರಚನಾ Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  ಯದಿ(!h->table)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  ಯದಿ(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  ಯದಿ(!iter->current) {
    size_t i;
    ಪ್ರತಿ(i = iter->slot_index; i < h->slots; i++) {
      ಯದಿ(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        ಭಙ್ಗ;
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ iter->current;
}
