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


प्रकारनिर्देश माप_प्रकार (*hash_function)(अचर शून्य *साँचो,
                                माप_प्रकार साँचो_length,
                                माप_प्रकार slots_अङ्क);

प्रकारनिर्देश माप_प्रकार (*comp_function)(अचर शून्य *key1,
                                माप_प्रकार key1_len,
                                अचर शून्य *key2,
                                माप_प्रकार key2_len);

/* Curl_hash stores its type in a uint8_t. */
प्रकारनिर्देश वर्ण hash_types_fit_uint8[
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
  अचिन्हित वर्ण buf[परिमाण(माप_प्रकार)];
  माप_प्रकार seed = 0;
  माप_प्रकार i;
  CURLcode result;

  यदि(seeded)
    फर्काऊ CURLE_OK;

  result = Curl_rand_bytes(रिक्त,
#ifdef DEBUGBUILD
                           FALSE,
#endif
                           buf, परिमाण(buf));
  यदि(result)
    फर्काऊ result;

  लागि(i = 0; i < परिमाण(buf); i++)
    seed = (seed << 8) | buf[i];

  hash_seed = seed ? seed : 1;
  seeded = TRUE;
  फर्काऊ CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST माप_प्रकार hash_str_seeded(अचर शून्य *साँचो, माप_प्रकार साँचो_length,
                                माप_प्रकार slots_अङ्क, माप_प्रकार seed);
UNITTEST माप_प्रकार hash_str_seeded(अचर शून्य *साँचो, माप_प्रकार साँचो_length,
                                माप_प्रकार slots_अङ्क, माप_प्रकार seed)
{
  अचर वर्ण *साँचो_str = (अचर वर्ण *)साँचो;
  अचर वर्ण *end = साँचो_str + साँचो_length;
  माप_प्रकार h = 5381 ^ seed;

  जबसम्म(साँचो_str < end) {
    माप_प्रकार j = (माप_प्रकार)*साँचो_str++;
    h += h << 5;
    h ^= j;
  }

  फर्काऊ (h % slots_अङ्क);
}

/* @unittest 1603
 */
UNITTEST माप_प्रकार hash_str(अचर शून्य *साँचो, माप_प्रकार साँचो_length, माप_प्रकार slots_अङ्क);
UNITTEST माप_प्रकार hash_str(अचर शून्य *साँचो, माप_प्रकार साँचो_length, माप_प्रकार slots_अङ्क)
{
  फर्काऊ hash_str_seeded(साँचो, साँचो_length, slots_अङ्क, hash_seed);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
स्थिर वर्ण *hash_elem_साँचो(संरचना Curl_hash_element *he)
{
  फर्काऊ (वर्ण *)he + दुरत्व(संरचना Curl_hash_element, साँचो);
}

स्थिर माप_प्रकार hash_socket(अचर शून्य *साँचो, माप_प्रकार साँचो_length,
                          माप_प्रकार slots_अङ्क)
{
  curl_socket_t socket;

  DEBUGASSERT(साँचो_length == परिमाण(socket));
  यदि(साँचो_length != परिमाण(socket))
    फर्काऊ 0;
  स_नकल(&socket, साँचो, परिमाण(socket));
  फर्काऊ (माप_प्रकार)(socket % (curl_socket_t)slots_अङ्क);
}

स्थिर माप_प्रकार compare_socket(अचर शून्य *key1, माप_प्रकार key1_len,
                             अचर शून्य *key2, माप_प्रकार key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  यदि((key1_len != परिमाण(socket1)) || (key2_len != परिमाण(socket2)))
    फर्काऊ 0;
  स_नकल(&socket1, key1, परिमाण(socket1));
  स_नकल(&socket2, key2, परिमाण(socket2));
  फर्काऊ socket1 == socket2;
}

स्थिर माप_प्रकार compare_bytes(अचर शून्य *key1, माप_प्रकार key1_len,
                            अचर शून्य *key2, माप_प्रकार key2_len)
{
  फर्काऊ (key1_len == key2_len) && !स_भेद(key1, key2, key1_len);
}

स्थिर अचर संरचना hash_functions hash_functions[] = {
  { hash_str, compare_bytes },
  { hash_socket, compare_socket }
};

स्थिर अचर संरचना hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_LAST);
  यदि((अचिन्हित पूर्णाङ्क)type >= CURL_HASH_TYPE_LAST)
    type = CURL_HASH_TYPE_BYTES;
  फर्काऊ &hash_functions[type];
}

स्थिर bool hash_साँचो_is_valid(uint8_t type, माप_प्रकार साँचो_len)
{
  फर्काऊ (type == CURL_HASH_TYPE_BYTES) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (साँचो_len == परिमाण(curl_socket_t)));
}

#यदि 0 /* useful function for debugging hashes and their contents */
शून्य Curl_hash_print(संरचना Curl_hash *h, शून्य (*func)(शून्य *))
{
  संरचना Curl_hash_iterator iter;
  संरचना Curl_hash_element *he;
  माप_प्रकार last_index = अच_पूर्णांक_उच्च;

  यदि(!h)
    फर्काऊ;

  curl_mfprintf(मानक_त्रुटि, "=Hash dump=\n");

  Curl_hash_start_iterate(h, &iter);

  he = Curl_hash_next_element(&iter);
  जबसम्म(he) {
    यदि(iter.slot_index != last_index) {
      curl_mfprintf(मानक_त्रुटि, "index %d:", (पूर्णाङ्क)iter.slot_index);
      यदि(last_index != अच_पूर्णांक_उच्च) {
        curl_mfprintf(मानक_त्रुटि, "\n");
      }
      last_index = iter.slot_index;
    }

    यदि(func)
      func(he->ptr);
    अन्यथा
      curl_mfprintf(मानक_त्रुटि, " [key=%.*s, he=%p, ptr=%p]",
                    (पूर्णाङ्क)he->साँचो_len, hash_elem_साँचो(he),
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
  DEBUGASSERT((अचिन्हित पूर्णाङ्क)type < CURL_HASH_TYPE_LAST);
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

स्थिर संरचना Curl_hash_element *hash_elem_create(अचर शून्य *साँचो,
                                                  माप_प्रकार साँचो_len,
                                                  शून्य *p,
                                                  Curl_hash_elem_dtor dtor)
{
  संरचना Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(परिमाण(संरचना Curl_hash_element) + साँचो_len);
  यदि(he) {
    he->next = रिक्त;
    /* copy the key */
    स_नकल(hash_elem_साँचो(he), साँचो, साँचो_len);
    he->साँचो_len = साँचो_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  फर्काऊ he;
}

स्थिर शून्य hash_elem_clear_ptr(संरचना Curl_hash *h,
                                संरचना Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  यदि(he->ptr) {
    यदि(he->dtor)
      he->dtor(hash_elem_साँचो(he), he->साँचो_len, he->ptr);
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
                     अचर शून्य *साँचो, माप_प्रकार साँचो_len, शून्य *p,
                     Curl_hash_elem_dtor dtor)
{
  अचर संरचना hash_functions *functions;
  संरचना Curl_hash_element *he, **slot;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  यदि(!hash_साँचो_is_valid(h->type, साँचो_len))
    फर्काऊ रिक्त;
  यदि(!h->table) {
    h->table = curlx_calloc(h->slots, परिमाण(संरचना Curl_hash_element *));
    यदि(!h->table)
      फर्काऊ रिक्त; /* OOM */
  }

  functions = hash_get_functions(h->type);
  slot = &h->table[functions->hash(साँचो, साँचो_len, h->slots)];
  लागि(he = *slot; he; he = he->next) {
    यदि(functions->compare(hash_elem_साँचो(he), he->साँचो_len, साँचो, साँचो_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      फर्काऊ p;
    }
  }

  he = hash_elem_create(साँचो, साँचो_len, p, dtor);
  यदि(!he)
    फर्काऊ रिक्त; /* OOM */

  hash_elem_link(h, slot, he);
  फर्काऊ p; /* return the new entry */
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
                    अचर शून्य *साँचो, माप_प्रकार साँचो_len, शून्य *p)
{
  फर्काऊ Curl_hash_add2(h, साँचो, साँचो_len, p, रिक्त);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
पूर्णाङ्क Curl_hash_delete(संरचना Curl_hash *h, अचर शून्य *साँचो, माप_प्रकार साँचो_len)
{
  अचर संरचना hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  यदि(!hash_साँचो_is_valid(h->type, साँचो_len))
    फर्काऊ 1;
  यदि(h->table) {
    संरचना Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->table[functions->hash(साँचो, साँचो_len, h->slots)];
    जबसम्म(*he_anchor) {
      he = *he_anchor;
      यदि(functions->compare(hash_elem_साँचो(he), he->साँचो_len, साँचो, साँचो_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        फर्काऊ 0;
      }
      he_anchor = &he->next;
    }
  }
  फर्काऊ 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
शून्य *Curl_hash_pick(संरचना Curl_hash *h, अचर शून्य *साँचो, माप_प्रकार साँचो_len)
{
  अचर संरचना hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  यदि(!hash_साँचो_is_valid(h->type, साँचो_len))
    फर्काऊ रिक्त;
  यदि(h->table) {
    संरचना Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->table[functions->hash(साँचो, साँचो_len, h->slots)];
    जबसम्म(he) {
      यदि(functions->compare(hash_elem_साँचो(he), he->साँचो_len, साँचो, साँचो_len)) {
        फर्काऊ he->ptr;
      }
      he = he->next;
    }
  }
  फर्काऊ रिक्त;
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
  यदि(h->table) {
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
  यदि(h && h->table) {
    संरचना Curl_hash_element *he, **he_anchor;
    माप_प्रकार i;
    DEBUGASSERT(h->init == HASHINIT);
    लागि(i = 0; i < h->slots; ++i) {
      he_anchor = &h->table[i];
      जबसम्म(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

माप_प्रकार Curl_hash_गन्ती(संरचना Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  फर्काऊ h->size;
}

/* Cleans all entries that pass the comp function criteria. */
शून्य Curl_hash_clean_with_criterium(संरचना Curl_hash *h, शून्य *user,
                                    पूर्णाङ्क (*comp)(शून्य *, शून्य *))
{
  माप_प्रकार i;

  यदि(!h || !h->table)
    फर्काऊ;

  DEBUGASSERT(h->init == HASHINIT);
  लागि(i = 0; i < h->slots; ++i) {
    संरचना Curl_hash_element *he, **he_anchor = &h->table[i];
    जबसम्म(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      यदि(!comp || comp(user, (*he_anchor)->ptr)) {
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
  यदि(!h->table)
    फर्काऊ रिक्त; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  यदि(iter->current)
    iter->current = iter->current->next;

  /* If we have reached the end of the list, find the next one */
  यदि(!iter->current) {
    माप_प्रकार i;
    लागि(i = iter->slot_index; i < h->slots; i++) {
      यदि(h->table[i]) {
        iter->current = h->table[i];
        iter->slot_index = i + 1;
        रोक;
      }
    }
  }

  फर्काऊ iter->current;
}
