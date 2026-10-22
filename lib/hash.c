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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ <stddef.h> /* for offsetof() */

#അന്തര്ഭാവ "hash.h"
#അന്തര്ഭാവ "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


തരനിർവചനം size_t (*hash_function)(സ്ഥിരം ശൂന്യം *താക്കോൽ,
                                size_t താക്കോൽ_length,
                                size_t slots_അക്കം);

തരനിർവചനം size_t (*comp_function)(സ്ഥിരം ശൂന്യം *key1,
                                size_t key1_len,
                                സ്ഥിരം ശൂന്യം *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
തരനിർവചനം അക്ഷരം hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

ഘടന hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
സ്ഥിതം size_t hash_seed;

CURLcode Curl_hash_global_init(ശൂന്യം)
{
  സ്ഥിതം bool seeded;
  unsigned അക്ഷരം buf[വലുപ്പം(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  എങ്കിൽ(seeded)
    തിരികെ CURLE_OK;

  result = Curl_rand_bytes(ശൂന്യ,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, വലുപ്പം(buf));
  എങ്കിൽ(result)
    തിരികെ result;

  വേണ്ടി(i = 0; i < വലുപ്പം(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  തിരികെ CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_length,
                                size_t slots_അക്കം, size_t seed);
UNITTEST size_t hash_str_seeded(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_length,
                                size_t slots_അക്കം, size_t seed)
{
  സ്ഥിരം അക്ഷരം *താക്കോൽ_str = (സ്ഥിരം അക്ഷരം *)താക്കോൽ;
  സ്ഥിരം അക്ഷരം *end = താക്കോൽ_str + താക്കോൽ_length;
  size_t h = 5381 ^ seed;

  ആകുമ്പോൾ(താക്കോൽ_str < end) {
    size_t j = (size_t)*താക്കോൽ_str++;
    h += h << 5;
    h ^= j;
  }

  തിരികെ (h % slots_അക്കം);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_length, size_t slots_അക്കം);
UNITTEST size_t hash_str(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_length, size_t slots_അക്കം)
{
  തിരികെ hash_str_seeded(താക്കോൽ, താക്കോൽ_length, slots_അക്കം, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
സ്ഥിതം അക്ഷരം *hash_elem_താക്കോൽ(ഘടന Curl_hash_element *he)
{
  തിരികെ (അക്ഷരം *)he + offsetof(ഘടന Curl_hash_element, താക്കോൽ);
}

സ്ഥിതം size_t hash_socket(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_length,
                          size_t slots_അക്കം)
{
  curl_socket_t socket;

  DEBUGASSERT(താക്കോൽ_length == വലുപ്പം(socket));
  എങ്കിൽ(താക്കോൽ_length != വലുപ്പം(socket))
    തിരികെ 0;
  memcpy(&socket, താക്കോൽ, വലുപ്പം(socket));
  തിരികെ (size_t)(socket % (curl_socket_t)slots_അക്കം);
}

സ്ഥിതം size_t compare_socket(സ്ഥിരം ശൂന്യം *key1, size_t key1_len,
                             സ്ഥിരം ശൂന്യം *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  എങ്കിൽ((key1_len != വലുപ്പം(socket1)) || (key2_len != വലുപ്പം(socket2)))
    തിരികെ 0;
  memcpy(&socket1, key1, വലുപ്പം(socket1));
  memcpy(&socket2, key2, വലുപ്പം(socket2));
  തിരികെ socket1 == socket2;
}

സ്ഥിതം size_t compare_bytes(സ്ഥിരം ശൂന്യം *key1, size_t key1_len,
                            സ്ഥിരം ശൂന്യം *key2, size_t key2_len)
{
  തിരികെ (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

സ്ഥിതം സ്ഥിരം ഘടന hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

സ്ഥിതം സ്ഥിരം ഘടന hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  എങ്കിൽ((unsigned പൂർണ്ണസംഖ്യ)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  തിരികെ &hash_functions[type];
}

സ്ഥിതം bool hash_താക്കോൽ_is_valid(uint8_t type, size_t താക്കോൽ_len)
{
  തിരികെ (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (താക്കോൽ_len == വലുപ്പം(curl_socket_t)));
}

#എങ്കിൽ 0 /* useful function for debugging hashes and their contents */
ശൂന്യം Curl_hash_print(ഘടന Curl_hash *h, ശൂന്യം (*func)(ശൂന്യം *))
{
  ഘടന Curl_hash_iterator iter;
  ഘടന Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  എങ്കിൽ(!h)
    തിരികെ;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  ആകുമ്പോൾ(he) {
    എങ്കിൽ(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (പൂർണ്ണസംഖ്യ)iter.slot_index);
      എങ്കിൽ(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    എങ്കിൽ(func)
      func(he->ptr);
    അല്ലെങ്കിൽ
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (പൂർണ്ണസംഖ്യ)he->താക്കോൽ_len, hash_elem_താക്കോൽ(he),
                    (ശൂന്യം *)he, (ശൂന്യം *)he->ptr);

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
ശൂന്യം Curl_hash_init(ഘടന Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned പൂർണ്ണസംഖ്യ)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = ശൂന്യ;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

സ്ഥിതം ഘടന Curl_hash_element *hash_elem_create(സ്ഥിരം ശൂന്യം *താക്കോൽ,
                                                  size_t താക്കോൽ_len,
                                                  ശൂന്യം *p,
                                                  Curl_hash_elem_dtor dtor)
{
  ഘടന Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(വലുപ്പം(ഘടന Curl_hash_element) + താക്കോൽ_len);
  എങ്കിൽ(he) {
    he->next = ശൂന്യ;
    /* copy the key */
    memcpy(hash_elem_താക്കോൽ(he), താക്കോൽ, താക്കോൽ_len);
    he->താക്കോൽ_len = താക്കോൽ_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  തിരികെ he;
}

സ്ഥിതം ശൂന്യം hash_elem_clear_ptr(ഘടന Curl_hash *h,
                                ഘടന Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  എങ്കിൽ(he->ptr) {
    എങ്കിൽ(he->dtor)
      he->dtor(hash_elem_താക്കോൽ(he), he->താക്കോൽ_len, he->ptr);
    അല്ലെങ്കിൽ
      h->dtor(he->ptr);
    he->ptr = ശൂന്യ;
  }
}

സ്ഥിതം ശൂന്യം hash_elem_destroy(ഘടന Curl_hash *h,
                              ഘടന Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

സ്ഥിതം ശൂന്യം hash_elem_unlink(ഘടന Curl_hash *h,
                             ഘടന Curl_hash_element **he_anchor,
                             ഘടന Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

സ്ഥിതം ശൂന്യം hash_elem_link(ഘടന Curl_hash *h,
                           ഘടന Curl_hash_element **he_anchor,
                           ഘടന Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

ശൂന്യം *Curl_hash_add2(ഘടന Curl_hash *h,
                     സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len, ശൂന്യം *p,
                     Curl_hash_elem_dtor dtor)
{
  സ്ഥിരം ഘടന hash_functions *functions;
  ഘടന Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  എങ്കിൽ(!hash_താക്കോൽ_is_valid(h->type, താക്കോൽ_len))
    തിരികെ ശൂന്യ;
  എങ്കിൽ(!h->table) {
    h->table = curlx_calloc(h->slots, വലുപ്പം(ഘടന Curl_hash_element *));
    എങ്കിൽ(!h->table)
      തിരികെ ശൂന്യ; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(താക്കോൽ, താക്കോൽ_len, h->slots)];
  വേണ്ടി(he = *slot; he; he = he->next) {
    എങ്കിൽ(functions->compare(hash_elem_താക്കോൽ(he), he->താക്കോൽ_len, താക്കോൽ, താക്കോൽ_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      തിരികെ p;
    }
  }

  he = hash_elem_create(താക്കോൽ, താക്കോൽ_len, p, dtor);
  എങ്കിൽ(!he)
    തിരികെ ശൂന്യ; /* OOM */

  hash_elem_link(h, slot, he);
  തിരികെ p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ശൂന്യം *Curl_hash_add(ഘടന Curl_hash *h,
                    സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len, ശൂന്യം *p)
{
  തിരികെ Curl_hash_add2(h, താക്കോൽ, താക്കോൽ_len, p, ശൂന്യ);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
പൂർണ്ണസംഖ്യ Curl_hash_delete(ഘടന Curl_hash *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len)
{
  സ്ഥിരം ഘടന hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  എങ്കിൽ(!hash_താക്കോൽ_is_valid(h->type, താക്കോൽ_len))
    തിരികെ 1;
  എങ്കിൽ(h->table) {
    ഘടന Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(താക്കോൽ, താക്കോൽ_len, h->slots)];
    ആകുമ്പോൾ(*he_anchor) {
      he = *he_anchor;
      എങ്കിൽ(functions->compare(hash_elem_താക്കോൽ(he), he->താക്കോൽ_len, താക്കോൽ, താക്കോൽ_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        തിരികെ 0;
      }
      he_anchor = &he->next;
    }
  }
  തിരികെ 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
ശൂന്യം *Curl_hash_pick(ഘടന Curl_hash *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len)
{
  സ്ഥിരം ഘടന hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  എങ്കിൽ(!hash_താക്കോൽ_is_valid(h->type, താക്കോൽ_len))
    തിരികെ ശൂന്യ;
  എങ്കിൽ(h->table) {
    ഘടന Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(താക്കോൽ, താക്കോൽ_len, h->slots)];
    ആകുമ്പോൾ(he) {
      എങ്കിൽ(functions->compare(hash_elem_താക്കോൽ(he), he->താക്കോൽ_len, താക്കോൽ, താക്കോൽ_len)) {
        തിരികെ he->ptr;
      }
      he = he->next;
    }
  }
  തിരികെ ശൂന്യ;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ശൂന്യം Curl_hash_destroy(ഘടന Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  എങ്കിൽ(h->table) {
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
ശൂന്യം Curl_hash_clean(ഘടന Curl_hash *h)
{
  എങ്കിൽ(h && h->table) {
    ഘടന Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    വേണ്ടി(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      ആകുമ്പോൾ(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_എണ്ണം(ഘടന Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  തിരികെ h->size;
}

/* Cleans all entries that pass the comp function criteria. */
ശൂന്യം Curl_hash_clean_with_criterium(ഘടന Curl_hash *h, ശൂന്യം *user,
                                    പൂർണ്ണസംഖ്യ (*comp)(ശൂന്യം *, ശൂന്യം *))
{
  size_t i;

  എങ്കിൽ(!h || !h->table)
    തിരികെ;

  DEBUGASSERT(h->init == HASHINIT);
  വേണ്ടി(i = 0; i < h->slots; ++i) {
    ഘടന Curl_hash_element *he, **he_anchor = &h->table[i];
    ആകുമ്പോൾ(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      എങ്കിൽ(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      അല്ലെങ്കിൽ
        he_anchor = &(*he_anchor)->next;
    }
  }
}

ശൂന്യം Curl_hash_start_iterate(ഘടന Curl_hash *hash,
                             ഘടന Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = ശൂന്യ;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

ഘടന Curl_hash_element *Curl_hash_next_element(
  ഘടന Curl_hash_iterator *iter)
{
  ഘടന Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  എങ്കിൽ(!h->table)
    തിരികെ ശൂന്യ; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  എങ്കിൽ(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  എങ്കിൽ(!iter->current) {
    size_t i;
    വേണ്ടി(i = iter->slot_index; i < h->slots; i++) {
      എങ്കിൽ(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        നിർത്തുക;
      }
    }
  }

  തിരികെ iter->current;
}
