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
#شامل "curl_setup.h"

#شامل <stddef.h> /* for offsetof() */

#شامل "hash.h"
#شامل "rand.h"

/* random patterns for API verification */
#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو HASHINIT 0x7017e781
#تعریف_کرو ITERINIT 0x5FEDCBA9
#اگر_ختم


قسمتعریف size_t (*hash_function)(ثابت خالی *کلید,
                                size_t کلید_لمبائی,
                                size_t slots_عدد);

قسمتعریف size_t (*comp_function)(ثابت خالی *key1,
                                size_t key1_len,
                                ثابت خالی *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
قسمتعریف حرف hash_types_fit_uint8[
  ((CURL_HASH_TYPE_آخری - 1) <= UINT8_MAX) ? 1 : -1];

ساخت hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
ساکن size_t hash_بیج;

CURLcode Curl_hash_global_init(خالی)
{
  ساکن bool seeded;
  غیرعلامتی حرف buf[حجم(size_t)];
  size_t بیج = 0;
  size_t i;
  CURLcode نتیجہ;

  اگر(seeded)
    واپس CURLE_OK;

  نتیجہ = Curl_rand_بائٹیں(تہی,
#اگر_تعریف_شدہ DEBUGBUILD
                           FALSE,
#اگر_ختم
                           buf, حجم(buf));
  اگر(نتیجہ)
    واپس نتیجہ;

  برائے(i = 0; i < حجم(buf); i++)
    بیج = (بیج << 8) | buf[i];

  hash_بیج = بیج ? بیج : 1;
  seeded = TRUE;
  واپس CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ثابت خالی *کلید, size_t کلید_لمبائی,
                                size_t slots_عدد, size_t بیج);
UNITTEST size_t hash_str_seeded(ثابت خالی *کلید, size_t کلید_لمبائی,
                                size_t slots_عدد, size_t بیج)
{
  ثابت حرف *کلید_str = (ثابت حرف *)کلید;
  ثابت حرف *انتہا = کلید_str + کلید_لمبائی;
  size_t h = 5381 ^ بیج;

  جب(کلید_str < انتہا) {
    size_t j = (size_t)*کلید_str++;
    h += h << 5;
    h ^= j;
  }

  واپس (h % slots_عدد);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ثابت خالی *کلید, size_t کلید_لمبائی, size_t slots_عدد);
UNITTEST size_t hash_str(ثابت خالی *کلید, size_t کلید_لمبائی, size_t slots_عدد)
{
  واپس hash_str_seeded(کلید, کلید_لمبائی, slots_عدد, hash_بیج);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ساکن حرف *hash_elem_کلید(ساخت Curl_hash_element *he)
{
  واپس (حرف *)he + offsetof(ساخت Curl_hash_element, کلید);
}

ساکن size_t hash_socket(ثابت خالی *کلید, size_t کلید_لمبائی,
                          size_t slots_عدد)
{
  curl_socket_t socket;

  DEBUGASSERT(کلید_لمبائی == حجم(socket));
  اگر(کلید_لمبائی != حجم(socket))
    واپس 0;
  حافظہ_نقل(&socket, کلید, حجم(socket));
  واپس (size_t)(socket % (curl_socket_t)slots_عدد);
}

ساکن size_t compare_socket(ثابت خالی *key1, size_t key1_len,
                             ثابت خالی *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  اگر((key1_len != حجم(socket1)) || (key2_len != حجم(socket2)))
    واپس 0;
  حافظہ_نقل(&socket1, key1, حجم(socket1));
  حافظہ_نقل(&socket2, key2, حجم(socket2));
  واپس socket1 == socket2;
}

ساکن size_t compare_بائٹیں(ثابت خالی *key1, size_t key1_len,
                            ثابت خالی *key2, size_t key2_len)
{
  واپس (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ساکن ثابت ساخت hash_functions hash_functions[] = {
  { hash_str, compare_بائٹیں },
  { hash_socket, compare_socket }
};

ساکن ثابت ساخت hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_آخری);
  اگر((غیرعلامتی صحیح)type >= CURL_HASH_TYPE_آخری)
    type = CURL_HASH_TYPE_بائٹیں;
  واپس &hash_functions[type];
}

ساکن bool hash_کلید_is_valid(uint8_t type, size_t کلید_len)
{
  واپس (type == CURL_HASH_TYPE_بائٹیں) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (کلید_len == حجم(curl_socket_t)));
}

#اگر 0 /* useful function for debugging hashes and their contents */
خالی Curl_hash_print(ساخت Curl_hash *h, خالی (*func)(خالی *))
{
  ساخت Curl_hash_iterator iter;
  ساخت Curl_hash_element *he;
  size_t آخری_اشاریہ = UINT_MAX;

  اگر(!h)
    واپس;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_آغاز_iterate(h, &iter);

  he = Curl_hash_اگلا_element(&iter);
  جب(he) {
    اگر(iter.خانہ_اشاریہ != آخری_اشاریہ) {
      curl_mfprintf(stderr, "index %d:", (صحیح)iter.خانہ_اشاریہ);
      اگر(آخری_اشاریہ != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      آخری_اشاریہ = iter.خانہ_اشاریہ;
    }

    اگر(func)
      func(he->ptr);
    ورنہ
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (صحیح)he->کلید_len, hash_elem_کلید(he),
                    (خالی *)he, (خالی *)he->ptr);

    he = Curl_hash_اگلا_element(&iter);
  }
  curl_mfprintf(stderr, "\n");
}
#اگر_ختم

/* Initializes a hash structure.
 *
 * @unittest: 1602
 * @unittest: 1603
 */
خالی Curl_hash_init(ساخت Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((غیرعلامتی صحیح)type < CURL_HASH_TYPE_آخری);
  DEBUGASSERT(dtor);

  h->جدول = تہی;
  h->dtor = dtor;
  h->سائز = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#اگر_تعریف_شدہ DEBUGBUILD
  h->init = HASHINIT;
#اگر_ختم
}

ساکن ساخت Curl_hash_element *hash_elem_create(ثابت خالی *کلید,
                                                  size_t کلید_len,
                                                  خالی *p,
                                                  Curl_hash_elem_dtor dtor)
{
  ساخت Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(حجم(ساخت Curl_hash_element) + کلید_len);
  اگر(he) {
    he->اگلا = تہی;
    /* copy the key */
    حافظہ_نقل(hash_elem_کلید(he), کلید, کلید_len);
    he->کلید_len = کلید_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  واپس he;
}

ساکن خالی hash_elem_clear_ptr(ساخت Curl_hash *h,
                                ساخت Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  اگر(he->ptr) {
    اگر(he->dtor)
      he->dtor(hash_elem_کلید(he), he->کلید_len, he->ptr);
    ورنہ
      h->dtor(he->ptr);
    he->ptr = تہی;
  }
}

ساکن خالی hash_elem_destroy(ساخت Curl_hash *h,
                              ساخت Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

ساکن خالی hash_elem_unlink(ساخت Curl_hash *h,
                             ساخت Curl_hash_element **he_anchor,
                             ساخت Curl_hash_element *he)
{
  *he_anchor = he->اگلا;
  --h->سائز;
}

ساکن خالی hash_elem_link(ساخت Curl_hash *h,
                           ساخت Curl_hash_element **he_anchor,
                           ساخت Curl_hash_element *he)
{
  he->اگلا = *he_anchor;
  *he_anchor = he;
  ++h->سائز;
}

خالی *Curl_hash_add2(ساخت Curl_hash *h,
                     ثابت خالی *کلید, size_t کلید_len, خالی *p,
                     Curl_hash_elem_dtor dtor)
{
  ثابت ساخت hash_functions *functions;
  ساخت Curl_hash_element *he, **خانہ;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  اگر(!hash_کلید_is_valid(h->type, کلید_len))
    واپس تہی;
  اگر(!h->جدول) {
    h->جدول = curlx_calloc(h->slots, حجم(ساخت Curl_hash_element *));
    اگر(!h->جدول)
      واپس تہی; /* OOM */
  }

  functions = hash_get_functions(h->type);
  خانہ = &h->جدول[functions->hash(کلید, کلید_len, h->slots)];
  برائے(he = *خانہ; he; he = he->اگلا) {
    اگر(functions->compare(hash_elem_کلید(he), he->کلید_len, کلید, کلید_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      واپس p;
    }
  }

  he = hash_elem_create(کلید, کلید_len, p, dtor);
  اگر(!he)
    واپس تہی; /* OOM */

  hash_elem_link(h, خانہ, he);
  واپس p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
خالی *Curl_hash_جوڑو(ساخت Curl_hash *h,
                    ثابت خالی *کلید, size_t کلید_len, خالی *p)
{
  واپس Curl_hash_add2(h, کلید, کلید_len, p, تہی);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
صحیح Curl_hash_delete(ساخت Curl_hash *h, ثابت خالی *کلید, size_t کلید_len)
{
  ثابت ساخت hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  اگر(!hash_کلید_is_valid(h->type, کلید_len))
    واپس 1;
  اگر(h->جدول) {
    ساخت Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->جدول[functions->hash(کلید, کلید_len, h->slots)];
    جب(*he_anchor) {
      he = *he_anchor;
      اگر(functions->compare(hash_elem_کلید(he), he->کلید_len, کلید, کلید_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        واپس 0;
      }
      he_anchor = &he->اگلا;
    }
  }
  واپس 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
خالی *Curl_hash_pick(ساخت Curl_hash *h, ثابت خالی *کلید, size_t کلید_len)
{
  ثابت ساخت hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  اگر(!hash_کلید_is_valid(h->type, کلید_len))
    واپس تہی;
  اگر(h->جدول) {
    ساخت Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->جدول[functions->hash(کلید, کلید_len, h->slots)];
    جب(he) {
      اگر(functions->compare(hash_elem_کلید(he), he->کلید_len, کلید, کلید_len)) {
        واپس he->ptr;
      }
      he = he->اگلا;
    }
  }
  واپس تہی;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
خالی Curl_hash_destroy(ساخت Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  اگر(h->جدول) {
    Curl_hash_clean(h);
    curlx_safefree(h->جدول);
  }
  DEBUGASSERT(h->سائز == 0);
  h->slots = 0;
}

/* Removes all the entries in the given hash.
 *
 * @unittest: 1602
 */
خالی Curl_hash_clean(ساخت Curl_hash *h)
{
  اگر(h && h->جدول) {
    ساخت Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    برائے(i = 0; i < h->slots; ++i) {
      he_anchor = &h->جدول[i];
      جب(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_گنتی(ساخت Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  واپس h->سائز;
}

/* Cleans all entries that pass the comp function criteria. */
خالی Curl_hash_clean_with_criterium(ساخت Curl_hash *h, خالی *user,
                                    صحیح (*comp)(خالی *, خالی *))
{
  size_t i;

  اگر(!h || !h->جدول)
    واپس;

  DEBUGASSERT(h->init == HASHINIT);
  برائے(i = 0; i < h->slots; ++i) {
    ساخت Curl_hash_element *he, **he_anchor = &h->جدول[i];
    جب(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      اگر(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      ورنہ
        he_anchor = &(*he_anchor)->اگلا;
    }
  }
}

خالی Curl_hash_آغاز_iterate(ساخت Curl_hash *hash,
                             ساخت Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->خانہ_اشاریہ = 0;
  iter->current = تہی;
#اگر_تعریف_شدہ DEBUGBUILD
  iter->init = ITERINIT;
#اگر_ختم
}

ساخت Curl_hash_element *Curl_hash_اگلا_element(
  ساخت Curl_hash_iterator *iter)
{
  ساخت Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  اگر(!h->جدول)
    واپس تہی; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  اگر(iter->current)
    iter->current = iter->current->اگلا;

  /* If we have reached the end of the list, find the next one */
  اگر(!iter->current) {
    size_t i;
    برائے(i = iter->خانہ_اشاریہ; i < h->slots; i++) {
      اگر(h->جدول[i]) {
        iter->current = h->جدول[i];
        iter->خانہ_اشاریہ = i + 1;
        توڑو;
      }
    }
  }

  واپس iter->current;
}
