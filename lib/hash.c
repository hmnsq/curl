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
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو HASHINIT 0x7017e781
#وصف_ڪريو ITERINIT 0x5FEDCBA9
#جيڪڏهن_پورو


تعريفنوع size_t (*hash_function)(ثابت فراغ *ڪنجي,
                                size_t ڪنجي_ڊيگهائي,
                                size_t slots_انگ);

تعريفنوع size_t (*comp_function)(ثابت فراغ *key1,
                                size_t key1_len,
                                ثابت فراغ *key2,
                                size_t key2_len);

/* Curl_hash stores its type in a uint8_t. */
تعريفنوع حرف hash_types_fit_uint8[
  ((CURL_HASH_TYPE_آخري - 1) <= UINT8_MAX) ? 1 : -1];

بنية hash_functions {
  hash_function hash;
  comp_function compare;
};

/* set at global init, read-only afterwards */
ساكن size_t hash_ٻج;

CURLcode Curl_hash_global_init(فراغ)
{
  ساكن bool seeded;
  بنانشان حرف buf[حجم(size_t)];
  size_t ٻج = 0;
  size_t i;
  CURLcode نتيجو;

  إذا(seeded)
    أرجع CURLE_OK;

  نتيجو = Curl_rand_بائيٽون(عدم,
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
                           FALSE,
#جيڪڏهن_پورو
                           buf, حجم(buf));
  إذا(نتيجو)
    أرجع نتيجو;

  لكل(i = 0; i < حجم(buf); i++)
    ٻج = (ٻج << 8) | buf[i];

  hash_ٻج = ٻج ? ٻج : 1;
  seeded = TRUE;
  أرجع CURLE_OK;
}

/* @unittest 1603
 */
UNITTEST size_t hash_str_seeded(ثابت فراغ *ڪنجي, size_t ڪنجي_ڊيگهائي,
                                size_t slots_انگ, size_t ٻج);
UNITTEST size_t hash_str_seeded(ثابت فراغ *ڪنجي, size_t ڪنجي_ڊيگهائي,
                                size_t slots_انگ, size_t ٻج)
{
  ثابت حرف *ڪنجي_str = (ثابت حرف *)ڪنجي;
  ثابت حرف *پڇاڙي = ڪنجي_str + ڪنجي_ڊيگهائي;
  size_t h = 5381 ^ ٻج;

  طالما(ڪنجي_str < پڇاڙي) {
    size_t j = (size_t)*ڪنجي_str++;
    h += h << 5;
    h ^= j;
  }

  أرجع (h % slots_انگ);
}

/* @unittest 1603
 */
UNITTEST size_t hash_str(ثابت فراغ *ڪنجي, size_t ڪنجي_ڊيگهائي, size_t slots_انگ);
UNITTEST size_t hash_str(ثابت فراغ *ڪنجي, size_t ڪنجي_ڊيگهائي, size_t slots_انگ)
{
  أرجع hash_str_seeded(ڪنجي, ڪنجي_ڊيگهائي, slots_انگ, hash_ٻج);
}

/* Avoid treating the variable-length key[1] as a one-byte object. */
ساكن حرف *hash_elem_ڪنجي(بنية Curl_hash_element *he)
{
  أرجع (حرف *)he + offsetof(بنية Curl_hash_element, ڪنجي);
}

ساكن size_t hash_socket(ثابت فراغ *ڪنجي, size_t ڪنجي_ڊيگهائي,
                          size_t slots_انگ)
{
  curl_socket_t socket;

  DEBUGASSERT(ڪنجي_ڊيگهائي == حجم(socket));
  إذا(ڪنجي_ڊيگهائي != حجم(socket))
    أرجع 0;
  يادگيري_نقل(&socket, ڪنجي, حجم(socket));
  أرجع (size_t)(socket % (curl_socket_t)slots_انگ);
}

ساكن size_t compare_socket(ثابت فراغ *key1, size_t key1_len,
                             ثابت فراغ *key2, size_t key2_len)
{
  curl_socket_t socket1;
  curl_socket_t socket2;

  إذا((key1_len != حجم(socket1)) || (key2_len != حجم(socket2)))
    أرجع 0;
  يادگيري_نقل(&socket1, key1, حجم(socket1));
  يادگيري_نقل(&socket2, key2, حجم(socket2));
  أرجع socket1 == socket2;
}

ساكن size_t compare_بائيٽون(ثابت فراغ *key1, size_t key1_len,
                            ثابت فراغ *key2, size_t key2_len)
{
  أرجع (key1_len == key2_len) && !memcmp(key1, key2, key1_len);
}

ساكن ثابت بنية hash_functions hash_functions[] = {
  { hash_str, compare_بائيٽون },
  { hash_socket, compare_socket }
};

ساكن ثابت بنية hash_functions *hash_get_functions(uint8_t type)
{
  DEBUGASSERT(CURL_ARRAYSIZE(hash_functions) == CURL_HASH_TYPE_آخري);
  إذا((بنانشان صحيح)type >= CURL_HASH_TYPE_آخري)
    type = CURL_HASH_TYPE_بائيٽون;
  أرجع &hash_functions[type];
}

ساكن bool hash_ڪنجي_is_valid(uint8_t type, size_t ڪنجي_len)
{
  أرجع (type == CURL_HASH_TYPE_بائيٽون) ||
    ((type == CURL_HASH_TYPE_SOCKET) &&
     (ڪنجي_len == حجم(curl_socket_t)));
}

#إذا 0 /* useful function for debugging hashes and their contents */
فراغ Curl_hash_print(بنية Curl_hash *h, فراغ (*func)(فراغ *))
{
  بنية Curl_hash_iterator iter;
  بنية Curl_hash_element *he;
  size_t آخري_اشاريو = UINT_MAX;

  إذا(!h)
    أرجع;

  curl_mfprintf(stderr, "=Hash dump=\n");

  Curl_hash_شروعات_iterate(h, &iter);

  he = Curl_hash_ايندڙ_element(&iter);
  طالما(he) {
    إذا(iter.جڳهه_اشاريو != آخري_اشاريو) {
      curl_mfprintf(stderr, "index %d:", (صحيح)iter.جڳهه_اشاريو);
      إذا(آخري_اشاريو != UINT_MAX) {
        curl_mfprintf(stderr, "\n");
      }
      آخري_اشاريو = iter.جڳهه_اشاريو;
    }

    إذا(func)
      func(he->ptr);
    وإلا
      curl_mfprintf(stderr, " [key=%.*s, he=%p, ptr=%p]",
                    (صحيح)he->ڪنجي_len, hash_elem_ڪنجي(he),
                    (فراغ *)he, (فراغ *)he->ptr);

    he = Curl_hash_ايندڙ_element(&iter);
  }
  curl_mfprintf(stderr, "\n");
}
#جيڪڏهن_پورو

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
  DEBUGASSERT((بنانشان صحيح)type < CURL_HASH_TYPE_آخري);
  DEBUGASSERT(dtor);

  h->جدول = عدم;
  h->dtor = dtor;
  h->سائيز = 0;
  h->slots = slots;
  h->type = (uint8_t)type;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  h->init = HASHINIT;
#جيڪڏهن_پورو
}

ساكن بنية Curl_hash_element *hash_elem_create(ثابت فراغ *ڪنجي,
                                                  size_t ڪنجي_len,
                                                  فراغ *p,
                                                  Curl_hash_elem_dtor dtor)
{
  بنية Curl_hash_element *he;

  /* allocate the struct plus memory after it to store the key */
  he = curlx_malloc(حجم(بنية Curl_hash_element) + ڪنجي_len);
  إذا(he) {
    he->ايندڙ = عدم;
    /* copy the key */
    يادگيري_نقل(hash_elem_ڪنجي(he), ڪنجي, ڪنجي_len);
    he->ڪنجي_len = ڪنجي_len;
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
      he->dtor(hash_elem_ڪنجي(he), he->ڪنجي_len, he->ptr);
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
  *he_anchor = he->ايندڙ;
  --h->سائيز;
}

ساكن فراغ hash_elem_link(بنية Curl_hash *h,
                           بنية Curl_hash_element **he_anchor,
                           بنية Curl_hash_element *he)
{
  he->ايندڙ = *he_anchor;
  *he_anchor = he;
  ++h->سائيز;
}

فراغ *Curl_hash_add2(بنية Curl_hash *h,
                     ثابت فراغ *ڪنجي, size_t ڪنجي_len, فراغ *p,
                     Curl_hash_elem_dtor dtor)
{
  ثابت بنية hash_functions *functions;
  بنية Curl_hash_element *he, **جڳهه;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  إذا(!hash_ڪنجي_is_valid(h->type, ڪنجي_len))
    أرجع عدم;
  إذا(!h->جدول) {
    h->جدول = curlx_calloc(h->slots, حجم(بنية Curl_hash_element *));
    إذا(!h->جدول)
      أرجع عدم; /* OOM */
  }

  functions = hash_get_functions(h->type);
  جڳهه = &h->جدول[functions->hash(ڪنجي, ڪنجي_len, h->slots)];
  لكل(he = *جڳهه; he; he = he->ايندڙ) {
    إذا(functions->compare(hash_elem_ڪنجي(he), he->ڪنجي_len, ڪنجي, ڪنجي_len)) {
      /* existing key entry, overwrite by clearing old pointer */
      hash_elem_clear_ptr(h, he);
      he->ptr = p;
      he->dtor = dtor;
      أرجع p;
    }
  }

  he = hash_elem_create(ڪنجي, ڪنجي_len, p, dtor);
  إذا(!he)
    أرجع عدم; /* OOM */

  hash_elem_link(h, جڳهه, he);
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
فراغ *Curl_hash_جوڙيو(بنية Curl_hash *h,
                    ثابت فراغ *ڪنجي, size_t ڪنجي_len, فراغ *p)
{
  أرجع Curl_hash_add2(h, ڪنجي, ڪنجي_len, p, عدم);
}

/* Remove the identified hash entry.
 * Returns non-zero on failure.
 *
 * @unittest: 1603
 */
صحيح Curl_hash_delete(بنية Curl_hash *h, ثابت فراغ *ڪنجي, size_t ڪنجي_len)
{
  ثابت بنية hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->slots);
  DEBUGASSERT(h->init == HASHINIT);
  إذا(!hash_ڪنجي_is_valid(h->type, ڪنجي_len))
    أرجع 1;
  إذا(h->جدول) {
    بنية Curl_hash_element *he, **he_anchor;

    functions = hash_get_functions(h->type);
    he_anchor = &h->جدول[functions->hash(ڪنجي, ڪنجي_len, h->slots)];
    طالما(*he_anchor) {
      he = *he_anchor;
      إذا(functions->compare(hash_elem_ڪنجي(he), he->ڪنجي_len, ڪنجي, ڪنجي_len)) {
        hash_elem_unlink(h, he_anchor, he);
        hash_elem_destroy(h, he);
        أرجع 0;
      }
      he_anchor = &he->ايندڙ;
    }
  }
  أرجع 1;
}

/* Retrieves a hash element.
 *
 * @unittest: 1603
 */
فراغ *Curl_hash_pick(بنية Curl_hash *h, ثابت فراغ *ڪنجي, size_t ڪنجي_len)
{
  ثابت بنية hash_functions *functions;

  DEBUGASSERT(h);
  DEBUGASSERT(h->init == HASHINIT);
  إذا(!hash_ڪنجي_is_valid(h->type, ڪنجي_len))
    أرجع عدم;
  إذا(h->جدول) {
    بنية Curl_hash_element *he;
    DEBUGASSERT(h->slots);
    functions = hash_get_functions(h->type);
    he = h->جدول[functions->hash(ڪنجي, ڪنجي_len, h->slots)];
    طالما(he) {
      إذا(functions->compare(hash_elem_ڪنجي(he), he->ڪنجي_len, ڪنجي, ڪنجي_len)) {
        أرجع he->ptr;
      }
      he = he->ايندڙ;
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
  DEBUGASSERT(h->سائيز == 0);
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

size_t Curl_hash_ڳڻپ(بنية Curl_hash *h)
{
  DEBUGASSERT(h->init == HASHINIT);
  أرجع h->سائيز;
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
        he_anchor = &(*he_anchor)->ايندڙ;
    }
  }
}

فراغ Curl_hash_شروعات_iterate(بنية Curl_hash *hash,
                             بنية Curl_hash_iterator *iter)
{
  DEBUGASSERT(hash->init == HASHINIT);
  iter->hash = hash;
  iter->جڳهه_اشاريو = 0;
  iter->current = عدم;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  iter->init = ITERINIT;
#جيڪڏهن_پورو
}

بنية Curl_hash_element *Curl_hash_ايندڙ_element(
  بنية Curl_hash_iterator *iter)
{
  بنية Curl_hash *h;
  DEBUGASSERT(iter->init == ITERINIT);
  h = iter->hash;
  إذا(!h->جدول)
    أرجع عدم; /* empty hash, nothing to return */

  /* Get the next element in the current list, if any */
  إذا(iter->current)
    iter->current = iter->current->ايندڙ;

  /* If we have reached the end of the list, find the next one */
  إذا(!iter->current) {
    size_t i;
    لكل(i = iter->جڳهه_اشاريو; i < h->slots; i++) {
      إذا(h->جدول[i]) {
        iter->current = h->جدول[i];
        iter->جڳهه_اشاريو = i + 1;
        اكسر;
      }
    }
  }

  أرجع iter->current;
}
