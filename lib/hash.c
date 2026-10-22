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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव <stddef.h> /* for offsetof() */

#अन्तर्भाव "hash.h"
#अन्तर्भाव "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


प्रकारनिर्देश size_t (*hash_function)(ध्रुव शून्य *कुञ्चिका,
                                size_t कुञ्चिका_length,
                                size_t slots_अङ्कः);

प्रकारनिर्देश size_t (*comp_function)(ध्रुव शून्य *key1,
                                size_t key1_len,
                                ध्रुव शून्य *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
प्रकारनिर्देश वर्ण hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

संरचना hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
स्थिर size_t hash_seed;

CURLcode Curl_hash_global_init(शून्य)
{
  स्थिर bool seeded;
  unsigned वर्ण buf[परिमाण(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  जइ(seeded)
    प्रत्यागम CURLE_OK;

  result = Curl_rand_bytes(रिक्त,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, परिमाण(buf));
  जइ(result)
    प्रत्यागम result;

  प्रति(i = 0; i < परिमाण(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  प्रत्यागम CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_length,
                                size_t slots_अङ्कः, size_t seed);
UNITTEST size_t hash_str_seeded(ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_length,
                                size_t slots_अङ्कः, size_t seed)
{
  ध्रुव वर्ण *कुञ्चिका_str = (ध्रुव वर्ण *)कुञ्चिका;
  ध्रुव वर्ण *end = कुञ्चिका_str + कुञ्चिका_length;
  size_t h = 5381 ^ seed;

  जाव(कुञ्चिका_str < end) {
    size_t j = (size_t)*कुञ्चिका_str++;
    h += h << 5;
    h ^= j;
  }

  प्रत्यागम (h % slots_अङ्कः);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_length, size_t slots_अङ्कः);
UNITTEST size_t hash_str(ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_length, size_t slots_अङ्कः)
{
  प्रत्यागम hash_str_seeded(कुञ्चिका, कुञ्चिका_length, slots_अङ्कः, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
स्थिर वर्ण *hash_elem_कुञ्चिका(संरचना Curl_hash_element *he)
{
  प्रत्यागम (वर्ण *)he + offsetof(संरचना Curl_hash_element, कुञ्चिका);
}

स्थिर size_t hash_socket(ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_length,
                          size_t slots_अङ्कः)
{
  curl_socket_t socket;

  DEBUGASSERT(कुञ्चिका_length == परिमाण(socket));
  जइ(कुञ्चिका_length != परिमाण(socket))
    प्रत्यागम 0;
  memcpy(&socket, कुञ्चिका, परिमाण(socket));
  प्रत्यागम (size_t)(socket % (curl_socket_t)slots_अङ्कः);
}

स्थिर size_t compare_socket(ध्रुव शून्य *key1, size_t key1_len,
                             ध्रुव शून्य *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  जइ((key1_len != परिमाण(socket1)) || (key2_len != परिमाण(socket2)))
    प्रत्यागम 0;
  memcpy(&socket1, key1, परिमाण(socket1));
  memcpy(&socket2, key2, परिमाण(socket2));
  प्रत्यागम socket1 == socket2;
}

स्थिर size_t compare_bytes(ध्रुव शून्य *key1, size_t key1_len,
                            ध्रुव शून्य *key2, size_t key2_len)
{
  प्रत्यागम (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

स्थिर ध्रुव संरचना hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

स्थिर ध्रुव संरचना hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  जइ((unsigned पूर्णाङ्क)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  प्रत्यागम &hash_functions[type];
}

स्थिर bool hash_कुञ्चिका_is_valid(uint8_t type, size_t कुञ्चिका_len)
{
  प्रत्यागम (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (कुञ्चिका_len == परिमाण(curl_socket_t)));
}

#जइ 0 /* useful function for debugging hashes and their contents */
शून्य Curl_hash_print(संरचना Curl_hash *h, शून्य (*func)(शून्य *))
{
  संरचना Curl_hash_iterator iter;
  संरचना Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  जइ(!h)
    प्रत्यागम;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  जाव(he) {
    जइ(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (पूर्णाङ्क)iter.slot_index);
      जइ(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    जइ(func)
      func(he->ptr);
    अण्णहा
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (पूर्णाङ्क)he->कुञ्चिका_len, hash_elem_कुञ्चिका(he),
                    (शून्य *)he, (शून्य *)he->ptr);

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
शून्य Curl_hash_init(संरचना Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned पूर्णाङ्क)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = रिक्त;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

स्थिर संरचना Curl_hash_element *hash_elem_create(ध्रुव शून्य *कुञ्चिका,
                                                  size_t कुञ्चिका_len,
                                                  शून्य *p,
                                                  Curl_hash_elem_dtor dtor)
{
  संरचना Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(परिमाण(संरचना Curl_hash_element) + कुञ्चिका_len);
  जइ(he) {
    he->next = रिक्त;
    /* copy the key */
    memcpy(hash_elem_कुञ्चिका(he), कुञ्चिका, कुञ्चिका_len);
    he->कुञ्चिका_len = कुञ्चिका_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  प्रत्यागम he;
}

स्थिर शून्य hash_elem_clear_ptr(संरचना Curl_hash *h,
                                संरचना Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  जइ(he->ptr) {
    जइ(he->dtor)
      he->dtor(hash_elem_कुञ्चिका(he), he->कुञ्चिका_len, he->ptr);
    अण्णहा
      h->dtor(he->ptr);
    he->ptr = रिक्त;
  }
}

स्थिर शून्य hash_elem_destroy(संरचना Curl_hash *h,
                              संरचना Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

स्थिर शून्य hash_elem_unlink(संरचना Curl_hash *h,
                             संरचना Curl_hash_element **he_anchor,
                             संरचना Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

स्थिर शून्य hash_elem_link(संरचना Curl_hash *h,
                           संरचना Curl_hash_element **he_anchor,
                           संरचना Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

शून्य *Curl_hash_add2(संरचना Curl_hash *h,
                     ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_len, शून्य *p,
                     Curl_hash_elem_dtor dtor)
{
  ध्रुव संरचना hash_functions *functions;
  संरचना Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  जइ(!hash_कुञ्चिका_is_valid(h->type, कुञ्चिका_len))
    प्रत्यागम रिक्त;
  जइ(!h->table) {
    h->table = curlx_calloc(h->slots, परिमाण(संरचना Curl_hash_element *));
    जइ(!h->table)
      प्रत्यागम रिक्त; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(कुञ्चिका, कुञ्चिका_len, h->slots)];
  प्रति(he = *slot; he; he = he->next) {
    जइ(functions->compare(hash_elem_कुञ्चिका(he), he->कुञ्चिका_len, कुञ्चिका, कुञ्चिका_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      प्रत्यागम p;
    }
  }

  he = hash_elem_create(कुञ्चिका, कुञ्चिका_len, p, dtor);
  जइ(!he)
    प्रत्यागम रिक्त; /* OOM */

  hash_elem_link(h, slot, he);
  प्रत्यागम p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
शून्य *Curl_hash_add(संरचना Curl_hash *h,
                    ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_len, शून्य *p)
{
  प्रत्यागम Curl_hash_add2(h, कुञ्चिका, कुञ्चिका_len, p, रिक्त);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
पूर्णाङ्क Curl_hash_delete(संरचना Curl_hash *h, ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_len)
{
  ध्रुव संरचना hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  जइ(!hash_कुञ्चिका_is_valid(h->type, कुञ्चिका_len))
    प्रत्यागम 1;
  जइ(h->table) {
    संरचना Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(कुञ्चिका, कुञ्चिका_len, h->slots)];
    जाव(*he_anchor) {
      he = *he_anchor;
      जइ(functions->compare(hash_elem_कुञ्चिका(he), he->कुञ्चिका_len, कुञ्चिका, कुञ्चिका_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        प्रत्यागम 0;
      }
      he_anchor = &he->next;
    }
  }
  प्रत्यागम 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
शून्य *Curl_hash_pick(संरचना Curl_hash *h, ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_len)
{
  ध्रुव संरचना hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  जइ(!hash_कुञ्चिका_is_valid(h->type, कुञ्चिका_len))
    प्रत्यागम रिक्त;
  जइ(h->table) {
    संरचना Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(कुञ्चिका, कुञ्चिका_len, h->slots)];
    जाव(he) {
      जइ(functions->compare(hash_elem_कुञ्चिका(he), he->कुञ्चिका_len, कुञ्चिका, कुञ्चिका_len)) {
        प्रत्यागम he->ptr;
      }
      he = he->next;
    }
  }
  प्रत्यागम रिक्त;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
शून्य Curl_hash_destroy(संरचना Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  जइ(h->table) {
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
शून्य Curl_hash_clean(संरचना Curl_hash *h)
{
  जइ(h && h->table) {
    संरचना Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    प्रति(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      जाव(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_गणनम्(संरचना Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  प्रत्यागम h->size;
}

/* Cleans all entries that pass the comp function criteria. */
शून्य Curl_hash_clean_with_criterium(संरचना Curl_hash *h, शून्य *user,
                                    पूर्णाङ्क (*comp)(शून्य *, शून्य *))
{
  size_t i;

  जइ(!h || !h->table)
    प्रत्यागम;

  DEBUGASSERT(h->init == HASHINIT);
  प्रति(i = 0; i < h->slots; ++i) {
    संरचना Curl_hash_element *he, **he_anchor = &h->table[i];
    जाव(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      जइ(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      अण्णहा
        he_anchor = &(*he_anchor)->next;
    }
  }
}

शून्य Curl_hash_start_iterate(संरचना Curl_hash *hash,
                             संरचना Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = रिक्त;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

संरचना Curl_hash_element *Curl_hash_next_element(
  संरचना Curl_hash_iterator *iter)
{
  संरचना Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  जइ(!h->table)
    प्रत्यागम रिक्त; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  जइ(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  जइ(!iter->current) {
    size_t i;
    प्रति(i = iter->slot_index; i < h->slots; i++) {
      जइ(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        भङ्ग;
      }
    }
  }

  प्रत्यागम iter->current;
}
