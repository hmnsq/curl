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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ <stddef.h> /* for offsetof() */

#அந்தர்பாவ "hash.h"
#அந்தர்பாவ "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


வகைவரை size_t (*hash_function)(மாறிலி வெறுமை *சாவி,
                                size_t சாவி_length,
                                size_t slots_இலக்கம்);

வகைவரை size_t (*comp_function)(மாறிலி வெறுமை *key1,
                                size_t key1_len,
                                மாறிலி வெறுமை *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
வகைவரை எழுத்து hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

கட்டமைப்பு hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
நிலையான size_t hash_seed;

CURLcode Curl_hash_global_init(வெறுமை)
{
  நிலையான bool seeded;
  unsigned எழுத்து buf[அளவு(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  எனில்(seeded)
    திரும்பு CURLE_OK;

  result = Curl_rand_bytes(வெற்று,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, அளவு(buf));
  எனில்(result)
    திரும்பு result;

  க்கு(i = 0; i < அளவு(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  திரும்பு CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(மாறிலி வெறுமை *சாவி, size_t சாவி_length,
                                size_t slots_இலக்கம், size_t seed);
UNITTEST size_t hash_str_seeded(மாறிலி வெறுமை *சாவி, size_t சாவி_length,
                                size_t slots_இலக்கம், size_t seed)
{
  மாறிலி எழுத்து *சாவி_str = (மாறிலி எழுத்து *)சாவி;
  மாறிலி எழுத்து *end = சாவி_str + சாவி_length;
  size_t h = 5381 ^ seed;

  போது(சாவி_str < end) {
    size_t j = (size_t)*சாவி_str++;
    h += h << 5;
    h ^= j;
  }

  திரும்பு (h % slots_இலக்கம்);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(மாறிலி வெறுமை *சாவி, size_t சாவி_length, size_t slots_இலக்கம்);
UNITTEST size_t hash_str(மாறிலி வெறுமை *சாவி, size_t சாவி_length, size_t slots_இலக்கம்)
{
  திரும்பு hash_str_seeded(சாவி, சாவி_length, slots_இலக்கம், hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
நிலையான எழுத்து *hash_elem_சாவி(கட்டமைப்பு Curl_hash_element *he)
{
  திரும்பு (எழுத்து *)he + offsetof(கட்டமைப்பு Curl_hash_element, சாவி);
}

நிலையான size_t hash_socket(மாறிலி வெறுமை *சாவி, size_t சாவி_length,
                          size_t slots_இலக்கம்)
{
  curl_socket_t socket;

  DEBUGASSERT(சாவி_length == அளவு(socket));
  எனில்(சாவி_length != அளவு(socket))
    திரும்பு 0;
  memcpy(&socket, சாவி, அளவு(socket));
  திரும்பு (size_t)(socket % (curl_socket_t)slots_இலக்கம்);
}

நிலையான size_t compare_socket(மாறிலி வெறுமை *key1, size_t key1_len,
                             மாறிலி வெறுமை *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  எனில்((key1_len != அளவு(socket1)) || (key2_len != அளவு(socket2)))
    திரும்பு 0;
  memcpy(&socket1, key1, அளவு(socket1));
  memcpy(&socket2, key2, அளவு(socket2));
  திரும்பு socket1 == socket2;
}

நிலையான size_t compare_bytes(மாறிலி வெறுமை *key1, size_t key1_len,
                            மாறிலி வெறுமை *key2, size_t key2_len)
{
  திரும்பு (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

நிலையான மாறிலி கட்டமைப்பு hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

நிலையான மாறிலி கட்டமைப்பு hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  எனில்((unsigned முழு)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  திரும்பு &hash_functions[type];
}

நிலையான bool hash_சாவி_is_valid(uint8_t type, size_t சாவி_len)
{
  திரும்பு (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (சாவி_len == அளவு(curl_socket_t)));
}

#எனில் 0 /* useful function for debugging hashes and their contents */
வெறுமை Curl_hash_print(கட்டமைப்பு Curl_hash *h, வெறுமை (*func)(வெறுமை *))
{
  கட்டமைப்பு Curl_hash_iterator iter;
  கட்டமைப்பு Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  எனில்(!h)
    திரும்பு;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  போது(he) {
    எனில்(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (முழு)iter.slot_index);
      எனில்(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    எனில்(func)
      func(he->ptr);
    இல்லையெனில்
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (முழு)he->சாவி_len, hash_elem_சாவி(he),
                    (வெறுமை *)he, (வெறுமை *)he->ptr);

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
வெறுமை Curl_hash_init(கட்டமைப்பு Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned முழு)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = வெற்று;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

நிலையான கட்டமைப்பு Curl_hash_element *hash_elem_create(மாறிலி வெறுமை *சாவி,
                                                  size_t சாவி_len,
                                                  வெறுமை *p,
                                                  Curl_hash_elem_dtor dtor)
{
  கட்டமைப்பு Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(அளவு(கட்டமைப்பு Curl_hash_element) + சாவி_len);
  எனில்(he) {
    he->next = வெற்று;
    /* copy the key */
    memcpy(hash_elem_சாவி(he), சாவி, சாவி_len);
    he->சாவி_len = சாவி_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  திரும்பு he;
}

நிலையான வெறுமை hash_elem_clear_ptr(கட்டமைப்பு Curl_hash *h,
                                கட்டமைப்பு Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  எனில்(he->ptr) {
    எனில்(he->dtor)
      he->dtor(hash_elem_சாவி(he), he->சாவி_len, he->ptr);
    இல்லையெனில்
      h->dtor(he->ptr);
    he->ptr = வெற்று;
  }
}

நிலையான வெறுமை hash_elem_destroy(கட்டமைப்பு Curl_hash *h,
                              கட்டமைப்பு Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

நிலையான வெறுமை hash_elem_unlink(கட்டமைப்பு Curl_hash *h,
                             கட்டமைப்பு Curl_hash_element **he_anchor,
                             கட்டமைப்பு Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

நிலையான வெறுமை hash_elem_link(கட்டமைப்பு Curl_hash *h,
                           கட்டமைப்பு Curl_hash_element **he_anchor,
                           கட்டமைப்பு Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

வெறுமை *Curl_hash_add2(கட்டமைப்பு Curl_hash *h,
                     மாறிலி வெறுமை *சாவி, size_t சாவி_len, வெறுமை *p,
                     Curl_hash_elem_dtor dtor)
{
  மாறிலி கட்டமைப்பு hash_functions *functions;
  கட்டமைப்பு Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  எனில்(!hash_சாவி_is_valid(h->type, சாவி_len))
    திரும்பு வெற்று;
  எனில்(!h->table) {
    h->table = curlx_calloc(h->slots, அளவு(கட்டமைப்பு Curl_hash_element *));
    எனில்(!h->table)
      திரும்பு வெற்று; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(சாவி, சாவி_len, h->slots)];
  க்கு(he = *slot; he; he = he->next) {
    எனில்(functions->compare(hash_elem_சாவி(he), he->சாவி_len, சாவி, சாவி_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      திரும்பு p;
    }
  }

  he = hash_elem_create(சாவி, சாவி_len, p, dtor);
  எனில்(!he)
    திரும்பு வெற்று; /* OOM */

  hash_elem_link(h, slot, he);
  திரும்பு p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
வெறுமை *Curl_hash_add(கட்டமைப்பு Curl_hash *h,
                    மாறிலி வெறுமை *சாவி, size_t சாவி_len, வெறுமை *p)
{
  திரும்பு Curl_hash_add2(h, சாவி, சாவி_len, p, வெற்று);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
முழு Curl_hash_delete(கட்டமைப்பு Curl_hash *h, மாறிலி வெறுமை *சாவி, size_t சாவி_len)
{
  மாறிலி கட்டமைப்பு hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  எனில்(!hash_சாவி_is_valid(h->type, சாவி_len))
    திரும்பு 1;
  எனில்(h->table) {
    கட்டமைப்பு Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(சாவி, சாவி_len, h->slots)];
    போது(*he_anchor) {
      he = *he_anchor;
      எனில்(functions->compare(hash_elem_சாவி(he), he->சாவி_len, சாவி, சாவி_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        திரும்பு 0;
      }
      he_anchor = &he->next;
    }
  }
  திரும்பு 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
வெறுமை *Curl_hash_pick(கட்டமைப்பு Curl_hash *h, மாறிலி வெறுமை *சாவி, size_t சாவி_len)
{
  மாறிலி கட்டமைப்பு hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  எனில்(!hash_சாவி_is_valid(h->type, சாவி_len))
    திரும்பு வெற்று;
  எனில்(h->table) {
    கட்டமைப்பு Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(சாவி, சாவி_len, h->slots)];
    போது(he) {
      எனில்(functions->compare(hash_elem_சாவி(he), he->சாவி_len, சாவி, சாவி_len)) {
        திரும்பு he->ptr;
      }
      he = he->next;
    }
  }
  திரும்பு வெற்று;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
வெறுமை Curl_hash_destroy(கட்டமைப்பு Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  எனில்(h->table) {
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
வெறுமை Curl_hash_clean(கட்டமைப்பு Curl_hash *h)
{
  எனில்(h && h->table) {
    கட்டமைப்பு Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    க்கு(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      போது(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_மொத்தம்(கட்டமைப்பு Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  திரும்பு h->size;
}

/* Cleans all entries that pass the comp function criteria. */
வெறுமை Curl_hash_clean_with_criterium(கட்டமைப்பு Curl_hash *h, வெறுமை *user,
                                    முழு (*comp)(வெறுமை *, வெறுமை *))
{
  size_t i;

  எனில்(!h || !h->table)
    திரும்பு;

  DEBUGASSERT(h->init == HASHINIT);
  க்கு(i = 0; i < h->slots; ++i) {
    கட்டமைப்பு Curl_hash_element *he, **he_anchor = &h->table[i];
    போது(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      எனில்(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      இல்லையெனில்
        he_anchor = &(*he_anchor)->next;
    }
  }
}

வெறுமை Curl_hash_start_iterate(கட்டமைப்பு Curl_hash *hash,
                             கட்டமைப்பு Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = வெற்று;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

கட்டமைப்பு Curl_hash_element *Curl_hash_next_element(
  கட்டமைப்பு Curl_hash_iterator *iter)
{
  கட்டமைப்பு Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  எனில்(!h->table)
    திரும்பு வெற்று; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  எனில்(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  எனில்(!iter->current) {
    size_t i;
    க்கு(i = iter->slot_index; i < h->slots; i++) {
      எனில்(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        நிறுத்து;
      }
    }
  }

  திரும்பு iter->current;
}
