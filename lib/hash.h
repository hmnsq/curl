#اگر_غیر_تعریف_شدہ سرخی_CURL_HASH_H
#تعریف_کرو سرخی_CURL_HASH_H
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

قسمتعریف خالی (*Curl_hash_dtor)(خالی *);

قسمتعریف خالی (*Curl_hash_elem_dtor)(ثابت خالی *کلید, size_t کلید_len, خالی *p);

/* How keys in a hash are interpreted. */
قسمتعریف شمار {
  CURL_HASH_TYPE_بائٹیں,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_آخری
} Curl_hash_type;

ساخت Curl_hash_element {
  ساخت Curl_hash_element *اگلا;
  خالی *ptr;
  Curl_hash_elem_dtor dtor;
  size_t کلید_len;
  حرف کلید[1]; /* allocated memory following the struct */
};

ساخت Curl_hash {
  ساخت Curl_hash_element **جدول;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t سائز;
  uint8_t type; /* Curl_hash_type */
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح init;
#اگر_ختم
};

ساخت Curl_hash_iterator {
  ساخت Curl_hash *hash;
  size_t خانہ_اشاریہ;
  ساخت Curl_hash_element *current;
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح init;
#اگر_ختم
};

CURLcode Curl_hash_global_init(خالی);

خالی Curl_hash_init(ساخت Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

خالی *Curl_hash_جوڑو(ساخت Curl_hash *h,
                    ثابت خالی *کلید, size_t کلید_len, خالی *p);
خالی *Curl_hash_add2(ساخت Curl_hash *h,
                     ثابت خالی *کلید, size_t کلید_len, خالی *p,
                     Curl_hash_elem_dtor dtor);
صحیح Curl_hash_delete(ساخت Curl_hash *h, ثابت خالی *کلید, size_t کلید_len);
خالی *Curl_hash_pick(ساخت Curl_hash *h, ثابت خالی *کلید, size_t کلید_len);

خالی Curl_hash_destroy(ساخت Curl_hash *h);
size_t Curl_hash_گنتی(ساخت Curl_hash *h);
خالی Curl_hash_clean(ساخت Curl_hash *h);
خالی Curl_hash_clean_with_criterium(ساخت Curl_hash *h, خالی *user,
                                    صحیح (*comp)(خالی *, خالی *));
خالی Curl_hash_آغاز_iterate(ساخت Curl_hash *hash,
                             ساخت Curl_hash_iterator *iter);
ساخت Curl_hash_element *Curl_hash_اگلا_element(
  ساخت Curl_hash_iterator *iter);

خالی Curl_hash_print(ساخت Curl_hash *h, خالی (*func)(خالی *));

#اگر_ختم /* HEADER_CURL_HASH_H */
