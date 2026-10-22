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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ <stddef.h> /* for offsetof() */

#ଅନ୍ତର୍ଭାଵ "hash.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


ପ୍ରକାରବ୍ୟାଖ୍ୟା size_t (*hash_function)(ଅଚଳ ଶୂନ୍ୟ *ଚାବି,
                                size_t ଚାବି_length,
                                size_t slots_ଅଙ୍କ);

ପ୍ରକାରବ୍ୟାଖ୍ୟା size_t (*comp_function)(ଅଚଳ ଶୂନ୍ୟ *key1,
                                size_t key1_len,
                                ଅଚଳ ଶୂନ୍ୟ *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଅକ୍ଷର hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

ସଂରଚନା hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
ସ୍ଥିର size_t hash_seed;

CURLcode Curl_hash_global_init(ଶୂନ୍ୟ)
{
  ସ୍ଥିର bool seeded;
  unsigned ଅକ୍ଷର buf[ଆକାର(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  ଯଦି(seeded)
    ଫେରନ୍ତୁ CURLE_OK;

  result = Curl_rand_bytes(ଖାଲି,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, ଆକାର(buf));
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ପାଇଁ(i = 0; i < ଆକାର(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_length,
                                size_t slots_ଅଙ୍କ, size_t seed);
UNITTEST size_t hash_str_seeded(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_length,
                                size_t slots_ଅଙ୍କ, size_t seed)
{
  ଅଚଳ ଅକ୍ଷର *ଚାବି_str = (ଅଚଳ ଅକ୍ଷର *)ଚାବି;
  ଅଚଳ ଅକ୍ଷର *end = ଚାବି_str + ଚାବି_length;
  size_t h = 5381 ^ seed;

  ଯେତେବେଳେ(ଚାବି_str < end) {
    size_t j = (size_t)*ଚାବି_str++;
    h += h << 5;
    h ^= j;
  }

  ଫେରନ୍ତୁ (h % slots_ଅଙ୍କ);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_length, size_t slots_ଅଙ୍କ);
UNITTEST size_t hash_str(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_length, size_t slots_ଅଙ୍କ)
{
  ଫେରନ୍ତୁ hash_str_seeded(ଚାବି, ଚାବି_length, slots_ଅଙ୍କ, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ସ୍ଥିର ଅକ୍ଷର *hash_elem_ଚାବି(ସଂରଚନା Curl_hash_element *he)
{
  ଫେରନ୍ତୁ (ଅକ୍ଷର *)he + offsetof(ସଂରଚନା Curl_hash_element, ଚାବି);
}

ସ୍ଥିର size_t hash_socket(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_length,
                          size_t slots_ଅଙ୍କ)
{
  curl_socket_t socket;

  DEBUGASSERT(ଚାବି_length == ଆକାର(socket));
  ଯଦି(ଚାବି_length != ଆକାର(socket))
    ଫେରନ୍ତୁ 0;
  memcpy(&socket, ଚାବି, ଆକାର(socket));
  ଫେରନ୍ତୁ (size_t)(socket % (curl_socket_t)slots_ଅଙ୍କ);
}

ସ୍ଥିର size_t compare_socket(ଅଚଳ ଶୂନ୍ୟ *key1, size_t key1_len,
                             ଅଚଳ ଶୂନ୍ୟ *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  ଯଦି((key1_len != ଆକାର(socket1)) || (key2_len != ଆକାର(socket2)))
    ଫେରନ୍ତୁ 0;
  memcpy(&socket1, key1, ଆକାର(socket1));
  memcpy(&socket2, key2, ଆକାର(socket2));
  ଫେରନ୍ତୁ socket1 == socket2;
}

ସ୍ଥିର size_t compare_bytes(ଅଚଳ ଶୂନ୍ୟ *key1, size_t key1_len,
                            ଅଚଳ ଶୂନ୍ୟ *key2, size_t key2_len)
{
  ଫେରନ୍ତୁ (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  ଯଦି((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  ଫେରନ୍ତୁ &hash_functions[type];
}

ସ୍ଥିର bool hash_ଚାବି_is_valid(uint8_t type, size_t ଚାବି_len)
{
  ଫେରନ୍ତୁ (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (ଚାବି_len == ଆକାର(curl_socket_t)));
}

#ଯଦି 0 /* useful function for debugging hashes and their contents */
ଶୂନ୍ୟ Curl_hash_print(ସଂରଚନା Curl_hash *h, ଶୂନ୍ୟ (*func)(ଶୂନ୍ୟ *))
{
  ସଂରଚନା Curl_hash_iterator iter;
  ସଂରଚନା Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  ଯଦି(!h)
    ଫେରନ୍ତୁ;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  ଯେତେବେଳେ(he) {
    ଯଦି(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)iter.slot_index);
      ଯଦି(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    ଯଦି(func)
      func(he->ptr);
    ଅନ୍ୟଥା
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)he->ଚାବି_len, hash_elem_ଚାବି(he),
                    (ଶୂନ୍ୟ *)he, (ଶୂନ୍ୟ *)he->ptr);

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
ଶୂନ୍ୟ Curl_hash_init(ସଂରଚନା Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = ଖାଲି;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

ସ୍ଥିର ସଂରଚନା Curl_hash_element *hash_elem_create(ଅଚଳ ଶୂନ୍ୟ *ଚାବି,
                                                  size_t ଚାବି_len,
                                                  ଶୂନ୍ୟ *p,
                                                  Curl_hash_elem_dtor dtor)
{
  ସଂରଚନା Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(ଆକାର(ସଂରଚନା Curl_hash_element) + ଚାବି_len);
  ଯଦି(he) {
    he->next = ଖାଲି;
    /* copy the key */
    memcpy(hash_elem_ଚାବି(he), ଚାବି, ଚାବି_len);
    he->ଚାବି_len = ଚାବି_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  ଫେରନ୍ତୁ he;
}

ସ୍ଥିର ଶୂନ୍ୟ hash_elem_clear_ptr(ସଂରଚନା Curl_hash *h,
                                ସଂରଚନା Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  ଯଦି(he->ptr) {
    ଯଦି(he->dtor)
      he->dtor(hash_elem_ଚାବି(he), he->ଚାବି_len, he->ptr);
    ଅନ୍ୟଥା
      h->dtor(he->ptr);
    he->ptr = ଖାଲି;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ hash_elem_destroy(ସଂରଚନା Curl_hash *h,
                              ସଂରଚନା Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

ସ୍ଥିର ଶୂନ୍ୟ hash_elem_unlink(ସଂରଚନା Curl_hash *h,
                             ସଂରଚନା Curl_hash_element **he_anchor,
                             ସଂରଚନା Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

ସ୍ଥିର ଶୂନ୍ୟ hash_elem_link(ସଂରଚନା Curl_hash *h,
                           ସଂରଚନା Curl_hash_element **he_anchor,
                           ସଂରଚନା Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

ଶୂନ୍ୟ *Curl_hash_add2(ସଂରଚନା Curl_hash *h,
                     ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len, ଶୂନ୍ୟ *p,
                     Curl_hash_elem_dtor dtor)
{
  ଅଚଳ ସଂରଚନା hash_functions *functions;
  ସଂରଚନା Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ଯଦି(!hash_ଚାବି_is_valid(h->type, ଚାବି_len))
    ଫେରନ୍ତୁ ଖାଲି;
  ଯଦି(!h->table) {
    h->table = curlx_calloc(h->slots, ଆକାର(ସଂରଚନା Curl_hash_element *));
    ଯଦି(!h->table)
      ଫେରନ୍ତୁ ଖାଲି; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(ଚାବି, ଚାବି_len, h->slots)];
  ପାଇଁ(he = *slot; he; he = he->next) {
    ଯଦି(functions->compare(hash_elem_ଚାବି(he), he->ଚାବି_len, ଚାବି, ଚାବି_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      ଫେରନ୍ତୁ p;
    }
  }

  he = hash_elem_create(ଚାବି, ଚାବି_len, p, dtor);
  ଯଦି(!he)
    ଫେରନ୍ତୁ ଖାଲି; /* OOM */

  hash_elem_link(h, slot, he);
  ଫେରନ୍ତୁ p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ଶୂନ୍ୟ *Curl_hash_add(ସଂରଚନା Curl_hash *h,
                    ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len, ଶୂନ୍ୟ *p)
{
  ଫେରନ୍ତୁ Curl_hash_add2(h, ଚାବି, ଚାବି_len, p, ଖାଲି);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_hash_delete(ସଂରଚନା Curl_hash *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len)
{
  ଅଚଳ ସଂରଚନା hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  ଯଦି(!hash_ଚାବି_is_valid(h->type, ଚାବି_len))
    ଫେରନ୍ତୁ 1;
  ଯଦି(h->table) {
    ସଂରଚନା Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(ଚାବି, ଚାବି_len, h->slots)];
    ଯେତେବେଳେ(*he_anchor) {
      he = *he_anchor;
      ଯଦି(functions->compare(hash_elem_ଚାବି(he), he->ଚାବି_len, ଚାବି, ଚାବି_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        ଫେରନ୍ତୁ 0;
      }
      he_anchor = &he->next;
    }
  }
  ଫେରନ୍ତୁ 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
ଶୂନ୍ୟ *Curl_hash_pick(ସଂରଚନା Curl_hash *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len)
{
  ଅଚଳ ସଂରଚନା hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  ଯଦି(!hash_ଚାବି_is_valid(h->type, ଚାବି_len))
    ଫେରନ୍ତୁ ଖାଲି;
  ଯଦି(h->table) {
    ସଂରଚନା Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(ଚାବି, ଚାବି_len, h->slots)];
    ଯେତେବେଳେ(he) {
      ଯଦି(functions->compare(hash_elem_ଚାବି(he), he->ଚାବି_len, ଚାବି, ଚାବି_len)) {
        ଫେରନ୍ତୁ he->ptr;
      }
      he = he->next;
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
ଶୂନ୍ୟ Curl_hash_destroy(ସଂରଚନା Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ଯଦି(h->table) {
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
ଶୂନ୍ୟ Curl_hash_clean(ସଂରଚନା Curl_hash *h)
{
  ଯଦି(h && h->table) {
    ସଂରଚନା Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    ପାଇଁ(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      ଯେତେବେଳେ(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_ଗଣନା(ସଂରଚନା Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  ଫେରନ୍ତୁ h->size;
}

/* Cleans all entries that pass the comp function criteria. */
ଶୂନ୍ୟ Curl_hash_clean_with_criterium(ସଂରଚନା Curl_hash *h, ଶୂନ୍ୟ *user,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*comp)(ଶୂନ୍ୟ *, ଶୂନ୍ୟ *))
{
  size_t i;

  ଯଦି(!h || !h->table)
    ଫେରନ୍ତୁ;

  DEBUGASSERT(h->init == HASHINIT);
  ପାଇଁ(i = 0; i < h->slots; ++i) {
    ସଂରଚନା Curl_hash_element *he, **he_anchor = &h->table[i];
    ଯେତେବେଳେ(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      ଯଦି(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      ଅନ୍ୟଥା
        he_anchor = &(*he_anchor)->next;
    }
  }
}

ଶୂନ୍ୟ Curl_hash_start_iterate(ସଂରଚନା Curl_hash *hash,
                             ସଂରଚନା Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = ଖାଲି;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

ସଂରଚନା Curl_hash_element *Curl_hash_next_element(
  ସଂରଚନା Curl_hash_iterator *iter)
{
  ସଂରଚନା Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  ଯଦି(!h->table)
    ଫେରନ୍ତୁ ଖାଲି; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  ଯଦି(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  ଯଦି(!iter->current) {
    size_t i;
    ପାଇଁ(i = iter->slot_index; i < h->slots; i++) {
      ଯଦି(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        ଭାଙ୍ଗ;
      }
    }
  }

  ଫେରନ୍ତୁ iter->current;
}
