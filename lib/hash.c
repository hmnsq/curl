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


प्रकारव्याख्या माप_प्रकार (*hash_function)(स्थिरांक शून्य *किल्ली,
                                माप_प्रकार किल्ली_length,
                                माप_प्रकार slots_अंक);

प्रकारव्याख्या माप_प्रकार (*comp_function)(स्थिरांक शून्य *key1,
                                माप_प्रकार key1_len,
                                स्थिरांक शून्य *key2,
                                माप_प्रकार key2_len);

/* Curl_hash stores its type in a uint8_t. */
प्रकारव्याख्या अक्षर hash_types_fit_uint8[
  ((CURL_HASH_TYPE_LAST - 1) <= UINT8_MAX) ? 1 : -1];

संरचना hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
स्थिर माप_प्रकार hash_seed;

CURLcode Curl_hash_global_init(शून्य)
{
  स्थिर bool seeded;
  अचिन्हित अक्षर buf[आकार(माप_प्रकार)];
  माप_प्रकार seed = 0;
  माप_प्रकार i;
  CURLcode result;

  जर(seeded)
    परत CURLE_OK;

  result = Curl_rand_bytes(रिक्त,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, आकार(buf));
  जर(result)
    परत result;

  साठी(i = 0; i < आकार(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  परत CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST माप_प्रकार hash_str_seeded(स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_length,
                                माप_प्रकार slots_अंक, माप_प्रकार seed);
UNITTEST माप_प्रकार hash_str_seeded(स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_length,
                                माप_प्रकार slots_अंक, माप_प्रकार seed)
{
  स्थिरांक अक्षर *किल्ली_str = (स्थिरांक अक्षर *)किल्ली;
  स्थिरांक अक्षर *end = किल्ली_str + किल्ली_length;
  माप_प्रकार h = 5381 ^ seed;

  पर्यंत(किल्ली_str < end) {
    माप_प्रकार j = (माप_प्रकार)*किल्ली_str++;
    h += h << 5;
    h ^= j;
  }

  परत (h % slots_अंक);
}

/* @unittest 1603
 */
UNITTEST माप_प्रकार hash_str(स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_length, माप_प्रकार slots_अंक);
UNITTEST माप_प्रकार hash_str(स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_length, माप_प्रकार slots_अंक)
{
  परत hash_str_seeded(किल्ली, किल्ली_length, slots_अंक, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
स्थिर अक्षर *hash_elem_किल्ली(संरचना Curl_hash_element *he)
{
  परत (अक्षर *)he + दुरत्व(संरचना Curl_hash_element, किल्ली);
}

स्थिर माप_प्रकार hash_socket(स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_length,
                          माप_प्रकार slots_अंक)
{
  curl_socket_t socket;

  DEBUGASSERT(किल्ली_length == आकार(socket));
  जर(किल्ली_length != आकार(socket))
    परत 0;
  स_नकल(&socket, किल्ली, आकार(socket));
  परत (माप_प्रकार)(socket % (curl_socket_t)slots_अंक);
}

स्थिर माप_प्रकार compare_socket(स्थिरांक शून्य *key1, माप_प्रकार key1_len,
                             स्थिरांक शून्य *key2, माप_प्रकार key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  जर((key1_len != आकार(socket1)) || (key2_len != आकार(socket2)))
    परत 0;
  स_नकल(&socket1, key1, आकार(socket1));
  स_नकल(&socket2, key2, आकार(socket2));
  परत socket1 == socket2;
}

स्थिर माप_प्रकार compare_bytes(स्थिरांक शून्य *key1, माप_प्रकार key1_len,
                            स्थिरांक शून्य *key2, माप_प्रकार key2_len)
{
  परत (key1_len == key2_len) && !स_भेद(key1, key2, key1_len);
}

स्थिर स्थिरांक संरचना hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

स्थिर स्थिरांक संरचना hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  जर((अचिन्हित पूर्णांक)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  परत &hash_functions[type];
}

स्थिर bool hash_किल्ली_is_valid(uint8_t type, माप_प्रकार किल्ली_len)
{
  परत (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (किल्ली_len == आकार(curl_socket_t)));
}

#जर 0 /* useful function for debugging hashes and their contents */
शून्य Curl_hash_print(संरचना Curl_hash *h, शून्य (*func)(शून्य *))
{
  संरचना Curl_hash_iterator iter;
  संरचना Curl_hash_element *he;
  माप_प्रकार last_index = अच_पूर्णांक_उच्च;

  जर(!h)
    परत;

  curl_mfprintf(मानक_त्रुटि, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  पर्यंत(he) {
    जर(iter.slot_index != last_index) {
      curl_mfprintf(मानक_त्रुटि, "index %d:", (पूर्णांक)iter.slot_index);
      जर(last_index != अच_पूर्णांक_उच्च) {
        curl_mfprintf(मानक_त्रुटि, "\n");
      }
      last_index = iter.slot_index;
    }

    जर(func)
      func(he->ptr);
    अन्यथा
      curl_mfprintf(मानक_त्रुटि, " [key=%.*s, he=%p, ptr=%p]",
                    (पूर्णांक)he->किल्ली_len, hash_elem_किल्ली(he),
                    (शून्य *)he, (शून्य *)he->ptr);

    he = Curl_hash_next_element(&iter);
  }
  curl_mfprintf(मानक_त्रुटि, "\n");
}
#endif

/* Initializes a hash structure.
 *
 * @unittest: 1602
 * @unittest: 1603
 */
शून्य Curl_hash_init(संरचना Curl_hash *h,
                    माप_प्रकार slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((अचिन्हित पूर्णांक)type < CURL_HASH_TYPE_LAST);
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

स्थिर संरचना Curl_hash_element *hash_elem_create(स्थिरांक शून्य *किल्ली,
                                                  माप_प्रकार किल्ली_len,
                                                  शून्य *p,
                                                  Curl_hash_elem_dtor dtor)
{
  संरचना Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(आकार(संरचना Curl_hash_element) + किल्ली_len);
  जर(he) {
    he->next = रिक्त;
    /* copy the key */
    स_नकल(hash_elem_किल्ली(he), किल्ली, किल्ली_len);
    he->किल्ली_len = किल्ली_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  परत he;
}

स्थिर शून्य hash_elem_clear_ptr(संरचना Curl_hash *h,
                                संरचना Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  जर(he->ptr) {
    जर(he->dtor)
      he->dtor(hash_elem_किल्ली(he), he->किल्ली_len, he->ptr);
    अन्यथा
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
                     स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len, शून्य *p,
                     Curl_hash_elem_dtor dtor)
{
  स्थिरांक संरचना hash_functions *functions;
  संरचना Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  जर(!hash_किल्ली_is_valid(h->type, किल्ली_len))
    परत रिक्त;
  जर(!h->table) {
    h->table = curlx_calloc(h->slots, आकार(संरचना Curl_hash_element *));
    जर(!h->table)
      परत रिक्त; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(किल्ली, किल्ली_len, h->slots)];
  साठी(he = *slot; he; he = he->next) {
    जर(functions->compare(hash_elem_किल्ली(he), he->किल्ली_len, किल्ली, किल्ली_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      परत p;
    }
  }

  he = hash_elem_create(किल्ली, किल्ली_len, p, dtor);
  जर(!he)
    परत रिक्त; /* OOM */

  hash_elem_link(h, slot, he);
  परत p; /* return the new entry */
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
                    स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len, शून्य *p)
{
  परत Curl_hash_add2(h, किल्ली, किल्ली_len, p, रिक्त);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
पूर्णांक Curl_hash_delete(संरचना Curl_hash *h, स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len)
{
  स्थिरांक संरचना hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  जर(!hash_किल्ली_is_valid(h->type, किल्ली_len))
    परत 1;
  जर(h->table) {
    संरचना Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(किल्ली, किल्ली_len, h->slots)];
    पर्यंत(*he_anchor) {
      he = *he_anchor;
      जर(functions->compare(hash_elem_किल्ली(he), he->किल्ली_len, किल्ली, किल्ली_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        परत 0;
      }
      he_anchor = &he->next;
    }
  }
  परत 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
शून्य *Curl_hash_pick(संरचना Curl_hash *h, स्थिरांक शून्य *किल्ली, माप_प्रकार किल्ली_len)
{
  स्थिरांक संरचना hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  जर(!hash_किल्ली_is_valid(h->type, किल्ली_len))
    परत रिक्त;
  जर(h->table) {
    संरचना Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(किल्ली, किल्ली_len, h->slots)];
    पर्यंत(he) {
      जर(functions->compare(hash_elem_किल्ली(he), he->किल्ली_len, किल्ली, किल्ली_len)) {
        परत he->ptr;
      }
      he = he->next;
    }
  }
  परत रिक्त;
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
  जर(h->table) {
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
  जर(h && h->table) {
    संरचना Curl_hash_element *he, **he_anchor;
    माप_प्रकार i;
    DEBUGASSERT(h->init == HASHINIT);
    साठी(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      पर्यंत(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

माप_प्रकार Curl_hash_मोजणी(संरचना Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  परत h->size;
}

/* Cleans all entries that pass the comp function criteria. */
शून्य Curl_hash_clean_with_criterium(संरचना Curl_hash *h, शून्य *user,
                                    पूर्णांक (*comp)(शून्य *, शून्य *))
{
  माप_प्रकार i;

  जर(!h || !h->table)
    परत;

  DEBUGASSERT(h->init == HASHINIT);
  साठी(i = 0; i < h->slots; ++i) {
    संरचना Curl_hash_element *he, **he_anchor = &h->table[i];
    पर्यंत(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      जर(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      अन्यथा
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
  जर(!h->table)
    परत रिक्त; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  जर(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  जर(!iter->current) {
    माप_प्रकार i;
    साठी(i = iter->slot_index; i < h->slots; i++) {
      जर(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        थांबा;
      }
    }
  }

  परत iter->current;
}
