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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ <stddef.h> /* for offsetof() */

#અન્તર્ભાવ "hash.h"
#અન્તર્ભાવ "rand.h"

/* random patterns for API verification */
#ifdef DEBUGBUILD
#define HASHINIT 0x7017e781
#define ITERINIT 0x5FEDCBA9
#endif


પ્રકારવ્યાખ્યા size_t (*hash_function)(અચલ શૂન્ય *ચાવી,
                                size_t ચાવી_length,
                                size_t slots_અંક);

પ્રકારવ્યાખ્યા size_t (*comp_function)(અચલ શૂન્ય *key1,
                                size_t key1_len,
                                અચલ શૂન્ય *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
પ્રકારવ્યાખ્યા અક્ષર hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

સંરચના hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
સ્થિર size_t hash_seed;

CURLcode Curl_hash_global_init(શૂન્ય)
{
  સ્થિર bool seeded;
  unsigned અક્ષર buf[કદ(size_t)];
  size_t seed = 0;
  size_t i;
  CURLcode result;

  જો(seeded)
    પાછા CURLE_OK;

  result = Curl_rand_bytes(ખાલી,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, કદ(buf));
  જો(result)
    પાછા result;

  માટે(i = 0; i < કદ(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  પાછા CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(અચલ શૂન્ય *ચાવી, size_t ચાવી_length,
                                size_t slots_અંક, size_t seed);
UNITTEST size_t hash_str_seeded(અચલ શૂન્ય *ચાવી, size_t ચાવી_length,
                                size_t slots_અંક, size_t seed)
{
  અચલ અક્ષર *ચાવી_str = (અચલ અક્ષર *)ચાવી;
  અચલ અક્ષર *end = ચાવી_str + ચાવી_length;
  size_t h = 5381 ^ seed;

  જ્યારે(ચાવી_str < end) {
    size_t j = (size_t)*ચાવી_str++;
    h += h << 5;
    h ^= j;
  }

  પાછા (h % slots_અંક);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(અચલ શૂન્ય *ચાવી, size_t ચાવી_length, size_t slots_અંક);
UNITTEST size_t hash_str(અચલ શૂન્ય *ચાવી, size_t ચાવી_length, size_t slots_અંક)
{
  પાછા hash_str_seeded(ચાવી, ચાવી_length, slots_અંક, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
સ્થિર અક્ષર *hash_elem_ચાવી(સંરચના Curl_hash_element *he)
{
  પાછા (અક્ષર *)he + offsetof(સંરચના Curl_hash_element, ચાવી);
}

સ્થિર size_t hash_socket(અચલ શૂન્ય *ચાવી, size_t ચાવી_length,
                          size_t slots_અંક)
{
  curl_socket_t socket;

  DEBUGASSERT(ચાવી_length == કદ(socket));
  જો(ચાવી_length != કદ(socket))
    પાછા 0;
  memcpy(&socket, ચાવી, કદ(socket));
  પાછા (size_t)(socket % (curl_socket_t)slots_અંક);
}

સ્થિર size_t compare_socket(અચલ શૂન્ય *key1, size_t key1_len,
                             અચલ શૂન્ય *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  જો((key1_len != કદ(socket1)) || (key2_len != કદ(socket2)))
    પાછા 0;
  memcpy(&socket1, key1, કદ(socket1));
  memcpy(&socket2, key2, કદ(socket2));
  પાછા socket1 == socket2;
}

સ્થિર size_t compare_bytes(અચલ શૂન્ય *key1, size_t key1_len,
                            અચલ શૂન્ય *key2, size_t key2_len)
{
  પાછા (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

સ્થિર અચલ સંરચના hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

સ્થિર અચલ સંરચના hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  જો((unsigned પૂર્ણાંક)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  પાછા &hash_functions[type];
}

સ્થિર bool hash_ચાવી_is_valid(uint8_t type, size_t ચાવી_len)
{
  પાછા (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (ચાવી_len == કદ(curl_socket_t)));
}

#જો 0 /* useful function for debugging hashes and their contents */
શૂન્ય Curl_hash_print(સંરચના Curl_hash *h, શૂન્ય (*func)(શૂન્ય *))
{
  સંરચના Curl_hash_iterator iter;
  સંરચના Curl_hash_element *he;
  size_t last_index = UINT_MAX;

  જો(!h)
    પાછા;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  જ્યારે(he) {
    જો(iter.slot_index != last_index) {
      curl_mfprintf(stderr, "index %d:", (પૂર્ણાંક)iter.slot_index);
      જો(last_index != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      last_index = iter.slot_index;
    }

    જો(func)
      func(he->ptr);
    નહીંતર
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (પૂર્ણાંક)he->ચાવી_len, hash_elem_ચાવી(he),
                    (શૂન્ય *)he, (શૂન્ય *)he->ptr);

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
શૂન્ય Curl_hash_init(સંરચના Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((unsigned પૂર્ણાંક)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = ખાલી;
  h->dtor = dtor;
  h->size = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#ifdef DEBUGBUILD
  h->init = HASHINIT;
#endif
}

સ્થિર સંરચના Curl_hash_element *hash_elem_create(અચલ શૂન્ય *ચાવી,
                                                  size_t ચાવી_len,
                                                  શૂન્ય *p,
                                                  Curl_hash_elem_dtor dtor)
{
  સંરચના Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(કદ(સંરચના Curl_hash_element) + ચાવી_len);
  જો(he) {
    he->next = ખાલી;
    /* copy the key */
    memcpy(hash_elem_ચાવી(he), ચાવી, ચાવી_len);
    he->ચાવી_len = ચાવી_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  પાછા he;
}

સ્થિર શૂન્ય hash_elem_clear_ptr(સંરચના Curl_hash *h,
                                સંરચના Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  જો(he->ptr) {
    જો(he->dtor)
      he->dtor(hash_elem_ચાવી(he), he->ચાવી_len, he->ptr);
    નહીંતર
      h->dtor(he->ptr);
    he->ptr = ખાલી;
  }
}

સ્થિર શૂન્ય hash_elem_destroy(સંરચના Curl_hash *h,
                              સંરચના Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

સ્થિર શૂન્ય hash_elem_unlink(સંરચના Curl_hash *h,
                             સંરચના Curl_hash_element **he_anchor,
                             સંરચના Curl_hash_element *he)
{
  *he_anchor = he->next;
  --h->size;
}

સ્થિર શૂન્ય hash_elem_link(સંરચના Curl_hash *h,
                           સંરચના Curl_hash_element **he_anchor,
                           સંરચના Curl_hash_element *he)
{
  he->next = *he_anchor;
  *he_anchor = he;
  ++h->size;
}

શૂન્ય *Curl_hash_add2(સંરચના Curl_hash *h,
                     અચલ શૂન્ય *ચાવી, size_t ચાવી_len, શૂન્ય *p,
                     Curl_hash_elem_dtor dtor)
{
  અચલ સંરચના hash_functions *functions;
  સંરચના Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  જો(!hash_ચાવી_is_valid(h->type, ચાવી_len))
    પાછા ખાલી;
  જો(!h->table) {
    h->table = curlx_calloc(h->slots, કદ(સંરચના Curl_hash_element *));
    જો(!h->table)
      પાછા ખાલી; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(ચાવી, ચાવી_len, h->slots)];
  માટે(he = *slot; he; he = he->next) {
    જો(functions->compare(hash_elem_ચાવી(he), he->ચાવી_len, ચાવી, ચાવી_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      પાછા p;
    }
  }

  he = hash_elem_create(ચાવી, ચાવી_len, p, dtor);
  જો(!he)
    પાછા ખાલી; /* OOM */

  hash_elem_link(h, slot, he);
  પાછા p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
શૂન્ય *Curl_hash_add(સંરચના Curl_hash *h,
                    અચલ શૂન્ય *ચાવી, size_t ચાવી_len, શૂન્ય *p)
{
  પાછા Curl_hash_add2(h, ચાવી, ચાવી_len, p, ખાલી);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
પૂર્ણાંક Curl_hash_delete(સંરચના Curl_hash *h, અચલ શૂન્ય *ચાવી, size_t ચાવી_len)
{
  અચલ સંરચના hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  જો(!hash_ચાવી_is_valid(h->type, ચાવી_len))
    પાછા 1;
  જો(h->table) {
    સંરચના Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(ચાવી, ચાવી_len, h->slots)];
    જ્યારે(*he_anchor) {
      he = *he_anchor;
      જો(functions->compare(hash_elem_ચાવી(he), he->ચાવી_len, ચાવી, ચાવી_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        પાછા 0;
      }
      he_anchor = &he->next;
    }
  }
  પાછા 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
શૂન્ય *Curl_hash_pick(સંરચના Curl_hash *h, અચલ શૂન્ય *ચાવી, size_t ચાવી_len)
{
  અચલ સંરચના hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  જો(!hash_ચાવી_is_valid(h->type, ચાવી_len))
    પાછા ખાલી;
  જો(h->table) {
    સંરચના Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(ચાવી, ચાવી_len, h->slots)];
    જ્યારે(he) {
      જો(functions->compare(hash_elem_ચાવી(he), he->ચાવી_len, ચાવી, ચાવી_len)) {
        પાછા he->ptr;
      }
      he = he->next;
    }
  }
  પાછા ખાલી;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
શૂન્ય Curl_hash_destroy(સંરચના Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  જો(h->table) {
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
શૂન્ય Curl_hash_clean(સંરચના Curl_hash *h)
{
  જો(h && h->table) {
    સંરચના Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    માટે(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      જ્યારે(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_ગણતરી(સંરચના Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  પાછા h->size;
}

/* Cleans all entries that pass the comp function criteria. */
શૂન્ય Curl_hash_clean_with_criterium(સંરચના Curl_hash *h, શૂન્ય *user,
                                    પૂર્ણાંક (*comp)(શૂન્ય *, શૂન્ય *))
{
  size_t i;

  જો(!h || !h->table)
    પાછા;

  DEBUGASSERT(h->init == HASHINIT);
  માટે(i = 0; i < h->slots; ++i) {
    સંરચના Curl_hash_element *he, **he_anchor = &h->table[i];
    જ્યારે(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      જો(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      નહીંતર
        he_anchor = &(*he_anchor)->next;
    }
  }
}

શૂન્ય Curl_hash_start_iterate(સંરચના Curl_hash *hash,
                             સંરચના Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_index = 0;
  iter->current = ખાલી;
#ifdef DEBUGBUILD
  iter->init = ITERINIT;
#endif
}

સંરચના Curl_hash_element *Curl_hash_next_element(
  સંરચના Curl_hash_iterator *iter)
{
  સંરચના Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  જો(!h->table)
    પાછા ખાલી; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  જો(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  જો(!iter->current) {
    size_t i;
    માટે(i = iter->slot_index; i < h->slots; i++) {
      જો(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        તોડો;
      }
    }
  }

  પાછા iter->current;
}
