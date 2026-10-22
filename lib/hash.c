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
#समावेश "curl_setup.h"

#समावेश <stddef.h> /* for offsetof() */

#समावेश "hash.h"
#समावेश "rand.h"

/* random patterns for API verification */
#अगर_घोषित DEBUGBUILD
#घोषणा HASHINIT 0x7017e781
#घोषणा ITERINIT 0x5FEDCBA9
#पूर्ण_अगर


प्रकार माप_प्रकार (*hash_function)(स्थिर व्योम *कुंजी,
                                माप_प्रकार कुंजी_लंबाई,
                                माप_प्रकार slots_अंक);

प्रकार माप_प्रकार (*comp_function)(स्थिर व्योम *key1,
                                माप_प्रकार key1_len,
                                स्थिर व्योम *key2,
                                माप_प्रकार key2_len);

/* Curl_hash stores its type in a uint8_t. */
प्रकार अक्षर hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

काष्ठा hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
अटल माप_प्रकार hash_seed;

CURLcode Curl_hash_global_init(व्योम)
{
  अटल bool seeded;
  अचिन्हित अक्षर buf[माप(माप_प्रकार)];
  माप_प्रकार seed = 0;
  माप_प्रकार i;
  CURLcode परिणाम;

  अगर(seeded)
    वापस CURLE_OK;

  परिणाम = Curl_rand_bytes(शून्य,
#अगर_घोषित DEBUGBUILD
                           FALSE,
#पूर्ण_अगर
                           buf, माप(buf));
  अगर(परिणाम)
    वापस परिणाम;

  क्रम(i = 0; i < माप(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  वापस CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST माप_प्रकार hash_str_seeded(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_लंबाई,
                                माप_प्रकार slots_अंक, माप_प्रकार seed);
UNITTEST माप_प्रकार hash_str_seeded(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_लंबाई,
                                माप_प्रकार slots_अंक, माप_प्रकार seed)
{
  स्थिर अक्षर *कुंजी_str = (स्थिर अक्षर *)कुंजी;
  स्थिर अक्षर *end = कुंजी_str + कुंजी_लंबाई;
  माप_प्रकार h = 5381 ^ seed;

  जबतक(कुंजी_str < end) {
    माप_प्रकार j = (माप_प्रकार)*कुंजी_str++;
    h += h << 5;
    h ^= j;
  }

  वापस (h % slots_अंक);
}

/* @unittest 1603
 */
UNITTEST माप_प्रकार hash_str(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_लंबाई, माप_प्रकार slots_अंक);
UNITTEST माप_प्रकार hash_str(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_लंबाई, माप_प्रकार slots_अंक)
{
  वापस hash_str_seeded(कुंजी, कुंजी_लंबाई, slots_अंक, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
अटल अक्षर *hash_elem_कुंजी(काष्ठा Curl_hash_element *he)
{
  वापस (अक्षर *)he + दुरत्व(काष्ठा Curl_hash_element, कुंजी);
}

अटल माप_प्रकार hash_socket(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_लंबाई,
                          माप_प्रकार slots_अंक)
{
  curl_socket_t socket;

  DEBUGASSERT(कुंजी_लंबाई == माप(socket));
  अगर(कुंजी_लंबाई != माप(socket))
    वापस 0;
  स_नकल(&socket, कुंजी, माप(socket));
  वापस (माप_प्रकार)(socket % (curl_socket_t)slots_अंक);
}

अटल माप_प्रकार compare_socket(स्थिर व्योम *key1, माप_प्रकार key1_len,
                             स्थिर व्योम *key2, माप_प्रकार key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  अगर((key1_len != माप(socket1)) || (key2_len != माप(socket2)))
    वापस 0;
  स_नकल(&socket1, key1, माप(socket1));
  स_नकल(&socket2, key2, माप(socket2));
  वापस socket1 == socket2;
}

अटल माप_प्रकार compare_bytes(स्थिर व्योम *key1, माप_प्रकार key1_len,
                            स्थिर व्योम *key2, माप_प्रकार key2_len)
{
  वापस (key1_len == key2_len) && !स_भेद(key1, key2, key1_len);
}

अटल स्थिर काष्ठा hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

अटल स्थिर काष्ठा hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  अगर((अचिन्हित पूर्णांक)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  वापस &hash_functions[type];
}

अटल bool hash_कुंजी_is_valid(uint8_t type, माप_प्रकार कुंजी_len)
{
  वापस (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (कुंजी_len == माप(curl_socket_t)));
}

#अगर 0 /* useful function for debugging hashes and their contents */
व्योम Curl_hash_print(काष्ठा Curl_hash *h, व्योम (*func)(व्योम *))
{
  काष्ठा Curl_hash_iterator iter;
  काष्ठा Curl_hash_element *he;
  माप_प्रकार last_सूचकांक = अच_पूर्णांक_उच्च;

  अगर(!h)
    वापस;

  curl_mfprintf(मानक_त्रुटि, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_अग्र_element(&iter);
  जबतक(he) {
    अगर(iter.slot_सूचकांक != last_सूचकांक) {
      curl_mfprintf(मानक_त्रुटि, "index %d:", (पूर्णांक)iter.slot_सूचकांक);
      अगर(last_सूचकांक != अच_पूर्णांक_उच्च) {
        curl_mfprintf(मानक_त्रुटि, "\n");
      }
      last_सूचकांक = iter.slot_सूचकांक;
    }

    अगर(func)
      func(he->ptr);
    अन्यथा
      curl_mfprintf(मानक_त्रुटि, " [key=%.*s, he=%p, ptr=%p]",
                    (पूर्णांक)he->कुंजी_len, hash_elem_कुंजी(he),
                    (व्योम *)he, (व्योम *)he->ptr);

    he = Curl_hash_अग्र_element(&iter);
  }
  curl_mfprintf(मानक_त्रुटि, "\n");
}
#पूर्ण_अगर

/* Initializes a hash structure.
 *
 * @unittest: 1602
 * @unittest: 1603
 */
व्योम Curl_hash_init(काष्ठा Curl_hash *h,
                    माप_प्रकार slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((अचिन्हित पूर्णांक)type < CURL_HASH_TYPE_LAST);
  DEBUGASSERT(dtor);

  h->table = शून्य;
  h->dtor = dtor;
  h->आकार = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#अगर_घोषित DEBUGBUILD
  h->init = HASHINIT;
#पूर्ण_अगर
}

अटल काष्ठा Curl_hash_element *hash_elem_create(स्थिर व्योम *कुंजी,
                                                  माप_प्रकार कुंजी_len,
                                                  व्योम *p,
                                                  Curl_hash_elem_dtor dtor)
{
  काष्ठा Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(माप(काष्ठा Curl_hash_element) + कुंजी_len);
  अगर(he) {
    he->अग्र = शून्य;
    /* copy the key */
    स_नकल(hash_elem_कुंजी(he), कुंजी, कुंजी_len);
    he->कुंजी_len = कुंजी_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  वापस he;
}

अटल व्योम hash_elem_clear_ptr(काष्ठा Curl_hash *h,
                                काष्ठा Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  अगर(he->ptr) {
    अगर(he->dtor)
      he->dtor(hash_elem_कुंजी(he), he->कुंजी_len, he->ptr);
    अन्यथा
      h->dtor(he->ptr);
    he->ptr = शून्य;
  }
}

अटल व्योम hash_elem_destroy(काष्ठा Curl_hash *h,
                              काष्ठा Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

अटल व्योम hash_elem_unlink(काष्ठा Curl_hash *h,
                             काष्ठा Curl_hash_element **he_anchor,
                             काष्ठा Curl_hash_element *he)
{
  *he_anchor = he->अग्र;
  --h->आकार;
}

अटल व्योम hash_elem_link(काष्ठा Curl_hash *h,
                           काष्ठा Curl_hash_element **he_anchor,
                           काष्ठा Curl_hash_element *he)
{
  he->अग्र = *he_anchor;
  *he_anchor = he;
  ++h->आकार;
}

व्योम *Curl_hash_add2(काष्ठा Curl_hash *h,
                     स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len, व्योम *p,
                     Curl_hash_elem_dtor dtor)
{
  स्थिर काष्ठा hash_functions *functions;
  काष्ठा Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  अगर(!hash_कुंजी_is_valid(h->type, कुंजी_len))
    वापस शून्य;
  अगर(!h->table) {
    h->table = curlx_calloc(h->slots, माप(काष्ठा Curl_hash_element *));
    अगर(!h->table)
      वापस शून्य; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(कुंजी, कुंजी_len, h->slots)];
  क्रम(he = *slot; he; he = he->अग्र) {
    अगर(functions->compare(hash_elem_कुंजी(he), he->कुंजी_len, कुंजी, कुंजी_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      वापस p;
    }
  }

  he = hash_elem_create(कुंजी, कुंजी_len, p, dtor);
  अगर(!he)
    वापस शून्य; /* OOM */

  hash_elem_link(h, slot, he);
  वापस p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
व्योम *Curl_hash_add(काष्ठा Curl_hash *h,
                    स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len, व्योम *p)
{
  वापस Curl_hash_add2(h, कुंजी, कुंजी_len, p, शून्य);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
पूर्णांक Curl_hash_delete(काष्ठा Curl_hash *h, स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len)
{
  स्थिर काष्ठा hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  अगर(!hash_कुंजी_is_valid(h->type, कुंजी_len))
    वापस 1;
  अगर(h->table) {
    काष्ठा Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(कुंजी, कुंजी_len, h->slots)];
    जबतक(*he_anchor) {
      he = *he_anchor;
      अगर(functions->compare(hash_elem_कुंजी(he), he->कुंजी_len, कुंजी, कुंजी_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        वापस 0;
      }
      he_anchor = &he->अग्र;
    }
  }
  वापस 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
व्योम *Curl_hash_pick(काष्ठा Curl_hash *h, स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len)
{
  स्थिर काष्ठा hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  अगर(!hash_कुंजी_is_valid(h->type, कुंजी_len))
    वापस शून्य;
  अगर(h->table) {
    काष्ठा Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(कुंजी, कुंजी_len, h->slots)];
    जबतक(he) {
      अगर(functions->compare(hash_elem_कुंजी(he), he->कुंजी_len, कुंजी, कुंजी_len)) {
        वापस he->ptr;
      }
      he = he->अग्र;
    }
  }
  वापस शून्य;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
व्योम Curl_hash_destroy(काष्ठा Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  अगर(h->table) {
    Curl_hash_clean(h);
    curlx_safefree(h->table);
  }
  DEBUGASSERT(h->आकार == 0);
  h->slots = 0;
}

/* Removes all the entries in the given hash.
 *
 * @unittest: 1602
 */
व्योम Curl_hash_clean(काष्ठा Curl_hash *h)
{
  अगर(h && h->table) {
    काष्ठा Curl_hash_element *he, **he_anchor;
    माप_प्रकार i;
    DEBUGASSERT(h->init == HASHINIT);
    क्रम(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      जबतक(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

माप_प्रकार Curl_hash_गिनती(काष्ठा Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  वापस h->आकार;
}

/* Cleans all entries that pass the comp function criteria. */
व्योम Curl_hash_clean_with_criterium(काष्ठा Curl_hash *h, व्योम *user,
                                    पूर्णांक (*comp)(व्योम *, व्योम *))
{
  माप_प्रकार i;

  अगर(!h || !h->table)
    वापस;

  DEBUGASSERT(h->init == HASHINIT);
  क्रम(i = 0; i < h->slots; ++i) {
    काष्ठा Curl_hash_element *he, **he_anchor = &h->table[i];
    जबतक(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      अगर(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      अन्यथा
        he_anchor = &(*he_anchor)->अग्र;
    }
  }
}

व्योम Curl_hash_start_iterate(काष्ठा Curl_hash *hash,
                             काष्ठा Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->slot_सूचकांक = 0;
  iter->current = शून्य;
#अगर_घोषित DEBUGBUILD
  iter->init = ITERINIT;
#पूर्ण_अगर
}

काष्ठा Curl_hash_element *Curl_hash_अग्र_element(
  काष्ठा Curl_hash_iterator *iter)
{
  काष्ठा Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  अगर(!h->table)
    वापस शून्य; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  अगर(iter->current)
    iter->current = iter->current->अग्र;

  /* If we have reached the end of the list, find the next one */
  अगर(!iter->current) {
    माप_प्रकार i;
    क्रम(i = iter->slot_सूचकांक; i < h->slots; i++) {
      अगर(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_सूचकांक = i + 1;
        अवरोध;
      }
    }
  }

  वापस iter->current;
}
