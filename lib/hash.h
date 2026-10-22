#اگر_تعریف_نشده سرآیند_CURL_HASH_H
#تعریف_کن سرآیند_CURL_HASH_H
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

تعريفنوع فراغ (*Curl_hash_dtor)(فراغ *);

تعريفنوع فراغ (*Curl_hash_elem_dtor)(ثابت فراغ *کلید, size_t کلید_len, فراغ *p);

/* How keys in a hash are interpreted. */
تعريفنوع تعداد {
  CURL_HASH_TYPE_بایتها,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_آخرین
} Curl_hash_type;

بنية Curl_hash_element {
  بنية Curl_hash_element *بعدی;
  فراغ *ptr;
  Curl_hash_elem_dtor dtor;
  size_t کلید_len;
  حرف کلید[1]; /* allocated memory following the struct */
};

بنية Curl_hash {
  بنية Curl_hash_element **جدول;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t اندازه;
  uint8_t type; /* Curl_hash_type */
#اگر_تعریف_شده DEBUGBUILD
  صحيح init;
#پایان_اگر
};

بنية Curl_hash_iterator {
  بنية Curl_hash *hash;
  size_t جایگاه_شاخص;
  بنية Curl_hash_element *current;
#اگر_تعریف_شده DEBUGBUILD
  صحيح init;
#پایان_اگر
};

CURLcode Curl_hash_global_init(فراغ);

فراغ Curl_hash_init(بنية Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

فراغ *Curl_hash_جمع_کن(بنية Curl_hash *h,
                    ثابت فراغ *کلید, size_t کلید_len, فراغ *p);
فراغ *Curl_hash_add2(بنية Curl_hash *h,
                     ثابت فراغ *کلید, size_t کلید_len, فراغ *p,
                     Curl_hash_elem_dtor dtor);
صحيح Curl_hash_delete(بنية Curl_hash *h, ثابت فراغ *کلید, size_t کلید_len);
فراغ *Curl_hash_pick(بنية Curl_hash *h, ثابت فراغ *کلید, size_t کلید_len);

فراغ Curl_hash_destroy(بنية Curl_hash *h);
size_t Curl_hash_شمار(بنية Curl_hash *h);
فراغ Curl_hash_clean(بنية Curl_hash *h);
فراغ Curl_hash_clean_with_criterium(بنية Curl_hash *h, فراغ *user,
                                    صحيح (*comp)(فراغ *, فراغ *));
فراغ Curl_hash_شروع_iterate(بنية Curl_hash *hash,
                             بنية Curl_hash_iterator *iter);
بنية Curl_hash_element *Curl_hash_بعدی_element(
  بنية Curl_hash_iterator *iter);

فراغ Curl_hash_print(بنية Curl_hash *h, فراغ (*func)(فراغ *));

#پایان_اگر /* HEADER_CURL_HASH_H */
