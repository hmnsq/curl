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
#ضمن "curl_setup.h"

#ضمن <stddef.h> /* for offsetof() */

#ضمن "hash.h"
#ضمن "rand.h"

/* random patterns for API verification */
#اگر_تعریف_شده DEBUGBUILD
#تعریف_کن HASHINIT 0x7017e781
#تعریف_کن ITERINIT 0x5FEDCBA9
#پایان_اگر


تعريفنوع size_t (*hash_function)(ثابت فراغ *کلید,
                                size_t کلید_درازی,
                                size_t slots_رقم);

تعريفنوع size_t (*comp_function)(ثابت فراغ *key1,
                                size_t key1_len,
                                ثابت فراغ *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
تعريفنوع حرف hash_types_fit_uint8[
  ((CURL_HASH_TYPE_آخرین - 1) <= UINT8_MAX) ? 1 : -1];

بنية hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
ساكن size_t hash_دانه;

CURLcode Curl_hash_global_init(فراغ)
{
  ساكن bool seeded;
  بیعلامت حرف buf[حجم(size_t)];
  size_t دانه = 0;
  size_t i;
  CURLcode نتیجه;

  إذا(seeded)
    أرجع CURLE_OK;

  نتیجه = Curl_rand_بایتها(عدم,
#اگر_تعریف_شده DEBUGBUILD
                           FALSE,
#پایان_اگر
                           buf, حجم(buf));
  إذا(نتیجه)
    أرجع نتیجه;

  لكل(i = 0; i < حجم(buf); i++)
    دانه = (دانه << 8) | buf[i];

  hash_دانه = دانه ? دانه : 1;
  seeded = TRUE;
  أرجع CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ثابت فراغ *کلید, size_t کلید_درازی,
                                size_t slots_رقم, size_t دانه);
UNITTEST size_t hash_str_seeded(ثابت فراغ *کلید, size_t کلید_درازی,
                                size_t slots_رقم, size_t دانه)
{
  ثابت حرف *کلید_str = (ثابت حرف *)کلید;
  ثابت حرف *انتها = کلید_str + کلید_درازی;
  size_t h = 5381 ^ دانه;

  طالما(کلید_str < انتها) {
    size_t j = (size_t)*کلید_str++;
    h += h << 5;
    h ^= j;
  }

  أرجع (h % slots_رقم);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ثابت فراغ *کلید, size_t کلید_درازی, size_t slots_رقم);
UNITTEST size_t hash_str(ثابت فراغ *کلید, size_t کلید_درازی, size_t slots_رقم)
{
  أرجع hash_str_seeded(کلید, کلید_درازی, slots_رقم, hash_دانه);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ساكن حرف *hash_elem_کلید(بنية Curl_hash_element *he)
{
  أرجع (حرف *)he + offsetof(بنية Curl_hash_element, کلید);
}

ساكن size_t hash_socket(ثابت فراغ *کلید, size_t کلید_درازی,
                          size_t slots_رقم)
{
  curl_socket_t socket;

  DEBUGASSERT(کلید_درازی == حجم(socket));
  إذا(کلید_درازی != حجم(socket))
    أرجع 0;
  کپی_حافظه(&socket, کلید, حجم(socket));
  أرجع (size_t)(socket % (curl_socket_t)slots_رقم);
}

ساكن size_t compare_socket(ثابت فراغ *key1, size_t key1_len,
                             ثابت فراغ *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  إذا((key1_len != حجم(socket1)) || (key2_len != حجم(socket2)))
    أرجع 0;
  کپی_حافظه(&socket1, key1, حجم(socket1));
  کپی_حافظه(&socket2, key2, حجم(socket2));
  أرجع socket1 == socket2;
}

ساكن size_t compare_بایتها(ثابت فراغ *key1, size_t key1_len,
                            ثابت فراغ *key2, size_t key2_len)
{
  أرجع (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ساكن ثابت بنية hash_functions hash_functions[] = {
  { hash_str, compare_بایتها },
  { hash_socket, compare_socket }
};

ساكن ثابت بنية hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_آخرین);
  إذا((بیعلامت صحيح)type >= CURL_HASH_TYPE_آخرین)
    type = CURL_HASH_TYPE_بایتها;
  أرجع &hash_functions[type];
}

ساكن bool hash_کلید_is_valid(uint8_t type, size_t کلید_len)
{
  أرجع (type == CURL_HASH_TYPE_بایتها) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (کلید_len == حجم(curl_socket_t)));
}

#إذا 0 /* useful function for debugging hashes and their contents */
فراغ Curl_hash_print(بنية Curl_hash *h, فراغ (*func)(فراغ *))
{
  بنية Curl_hash_iterator iter;
  بنية Curl_hash_element *he;
  size_t آخرین_شاخص = UINT_MAX;

  إذا(!h)
    أرجع;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_شروع_iterate(h, &iter);

  he = Curl_hash_بعدی_element(&iter);
  طالما(he) {
    إذا(iter.جایگاه_شاخص != آخرین_شاخص) {
      curl_mfprintf(stderr, "index %d:", (صحيح)iter.جایگاه_شاخص);
      إذا(آخرین_شاخص != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      آخرین_شاخص = iter.جایگاه_شاخص;
    }

    إذا(func)
      func(he->ptr);
    وإلا
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (صحيح)he->کلید_len, hash_elem_کلید(he),
                    (فراغ *)he, (فراغ *)he->ptr);

    he = Curl_hash_بعدی_element(&iter);
  }
  curl_mfprintf(stderr, "\n");
}
#پایان_اگر

/* Initializes a hash structure.
 *
 * @unittest: 1602
 * @unittest: 1603
 */
فراغ Curl_hash_init(بنية Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor)
{
  DEBUGASSERT(h);
  DEBUGASSERT(slots);
  DEBUGASSERT((بیعلامت صحيح)type < CURL_HASH_TYPE_آخرین);
  DEBUGASSERT(dtor);

  h->جدول = عدم;
  h->dtor = dtor;
  h->اندازه = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#اگر_تعریف_شده DEBUGBUILD
  h->init = HASHINIT;
#پایان_اگر
}

ساكن بنية Curl_hash_element *hash_elem_create(ثابت فراغ *کلید,
                                                  size_t کلید_len,
                                                  فراغ *p,
                                                  Curl_hash_elem_dtor dtor)
{
  بنية Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(حجم(بنية Curl_hash_element) + کلید_len);
  إذا(he) {
    he->بعدی = عدم;
    /* copy the key */
    کپی_حافظه(hash_elem_کلید(he), کلید, کلید_len);
    he->کلید_len = کلید_len;
    he->ptr = p;
    he->dtor = dtor;
  }
  أرجع he;
}

ساكن فراغ hash_elem_clear_ptr(بنية Curl_hash *h,
                                بنية Curl_hash_element *he)
{
  DEBUGASSERT(h);
  DEBUGASSERT(he);
  إذا(he->ptr) {
    إذا(he->dtor)
      he->dtor(hash_elem_کلید(he), he->کلید_len, he->ptr);
    وإلا
      h->dtor(he->ptr);
    he->ptr = عدم;
  }
}

ساكن فراغ hash_elem_destroy(بنية Curl_hash *h,
                              بنية Curl_hash_element *he)
{
  hash_elem_clear_ptr(h, he);
  curlx_free(he);
}

ساكن فراغ hash_elem_unlink(بنية Curl_hash *h,
                             بنية Curl_hash_element **he_anchor,
                             بنية Curl_hash_element *he)
{
  *he_anchor = he->بعدی;
  --h->اندازه;
}

ساكن فراغ hash_elem_link(بنية Curl_hash *h,
                           بنية Curl_hash_element **he_anchor,
                           بنية Curl_hash_element *he)
{
  he->بعدی = *he_anchor;
  *he_anchor = he;
  ++h->اندازه;
}

فراغ *Curl_hash_add2(بنية Curl_hash *h,
                     ثابت فراغ *کلید, size_t کلید_len, فراغ *p,
                     Curl_hash_elem_dtor dtor)
{
  ثابت بنية hash_functions *functions;
  بنية Curl_hash_element *he, **جایگاه;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  إذا(!hash_کلید_is_valid(h->type, کلید_len))
    أرجع عدم;
  إذا(!h->جدول) {
    h->جدول = curlx_calloc(h->slots, حجم(بنية Curl_hash_element *));
    إذا(!h->جدول)
      أرجع عدم; /* OOM */
  }

  functions = hash_get_functions(h->type);
  جایگاه = &h->جدول[functions->hash(کلید, کلید_len, h->slots)];
  لكل(he = *جایگاه; he; he = he->بعدی) {
    إذا(functions->compare(hash_elem_کلید(he), he->کلید_len, کلید, کلید_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      أرجع p;
    }
  }

  he = hash_elem_create(کلید, کلید_len, p, dtor);
  إذا(!he)
    أرجع عدم; /* OOM */

  hash_elem_link(h, جایگاه, he);
  أرجع p; /* return the new entry */
}

/* Insert the data in the hash. If there already was a match in the hash, that
 * data is replaced. This function also "lazily" allocates the table if
 * needed, as it is not done in the _init function (anymore).
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
فراغ *Curl_hash_جمع_کن(بنية Curl_hash *h,
                    ثابت فراغ *کلید, size_t کلید_len, فراغ *p)
{
  أرجع Curl_hash_add2(h, کلید, کلید_len, p, عدم);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
صحيح Curl_hash_delete(بنية Curl_hash *h, ثابت فراغ *کلید, size_t کلید_len)
{
  ثابت بنية hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  إذا(!hash_کلید_is_valid(h->type, کلید_len))
    أرجع 1;
  إذا(h->جدول) {
    بنية Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->جدول[functions->hash(کلید, کلید_len, h->slots)];
    طالما(*he_anchor) {
      he = *he_anchor;
      إذا(functions->compare(hash_elem_کلید(he), he->کلید_len, کلید, کلید_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        أرجع 0;
      }
      he_anchor = &he->بعدی;
    }
  }
  أرجع 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
فراغ *Curl_hash_pick(بنية Curl_hash *h, ثابت فراغ *کلید, size_t کلید_len)
{
  ثابت بنية hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  إذا(!hash_کلید_is_valid(h->type, کلید_len))
    أرجع عدم;
  إذا(h->جدول) {
    بنية Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->جدول[functions->hash(کلید, کلید_len, h->slots)];
    طالما(he) {
      إذا(functions->compare(hash_elem_کلید(he), he->کلید_len, کلید, کلید_len)) {
        أرجع he->ptr;
      }
      he = he->بعدی;
    }
  }
  أرجع عدم;
}

/* Destroys all the entries in the given hash and resets its attributes,
 * prepping the given hash for [static|dynamic] deallocation.
 *
 * @unittest: 1305
 * @unittest: 1602
 * @unittest: 1603
 */
فراغ Curl_hash_destroy(بنية Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  إذا(h->جدول) {
    Curl_hash_clean(h);
    curlx_safefree(h->جدول);
  }
  DEBUGASSERT(h->اندازه == 0);
  h->slots = 0;
}

/* Removes all the entries in the given hash.
 *
 * @unittest: 1602
 */
فراغ Curl_hash_clean(بنية Curl_hash *h)
{
  إذا(h && h->جدول) {
    بنية Curl_hash_element *he, **he_anchor;
    size_t i;
    DEBUGASSERT(h->init == HASHINIT);
    لكل(i = 0; i < h->slots; ++i) {
      he_anchor = &h->جدول[i];
      طالما(*he_anchor) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
    }
  }
}

size_t Curl_hash_شمار(بنية Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  أرجع h->اندازه;
}

/* Cleans all entries that pass the comp function criteria. */
فراغ Curl_hash_clean_with_criterium(بنية Curl_hash *h, فراغ *user,
                                    صحيح (*comp)(فراغ *, فراغ *))
{
  size_t i;

  إذا(!h || !h->جدول)
    أرجع;

  DEBUGASSERT(h->init == HASHINIT);
  لكل(i = 0; i < h->slots; ++i) {
    بنية Curl_hash_element *he, **he_anchor = &h->جدول[i];
    طالما(*he_anchor) {
      /* ask the callback function if we shall remove this entry or not */
      إذا(!comp || comp(user, (*he_anchor)->ptr)) {
        he = *he_anchor;
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
      }
      وإلا
        he_anchor = &(*he_anchor)->بعدی;
    }
  }
}

فراغ Curl_hash_شروع_iterate(بنية Curl_hash *hash,
                             بنية Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->جایگاه_شاخص = 0;
  iter->current = عدم;
#اگر_تعریف_شده DEBUGBUILD
  iter->init = ITERINIT;
#پایان_اگر
}

بنية Curl_hash_element *Curl_hash_بعدی_element(
  بنية Curl_hash_iterator *iter)
{
  بنية Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  إذا(!h->جدول)
    أرجع عدم; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  إذا(iter->current)
    iter->current = iter->current->بعدی;

  /* If we have reached the end of the list, find the next one */
  إذا(!iter->current) {
    size_t i;
    لكل(i = iter->جایگاه_شاخص; i < h->slots; i++) {
      إذا(h->جدول[i]) {
        iter->current = h->جدول[i];
        iter->جایگاه_شاخص = i + 1;
        اكسر;
      }
    }
  }

  أرجع iter->current;
}
