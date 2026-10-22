#ifndef HEADER_CURL_HASH_H
#define HEADER_CURL_HASH_H
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

تعریفنوع تهی (*Curl_hash_dtor)(تهی *);

تعریفنوع تهی (*Curl_hash_elem_dtor)(ثابت تهی *کلید, size_t کلید_len, تهی *p);

/* How keys in a hash are interpreted. */
تعریفنوع شمارش {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

ساختار Curl_hash_element {
  ساختار Curl_hash_element *next;
  تهی *ptr;
  Curl_hash_elem_dtor dtor;
  size_t کلید_len;
  نویسه کلید[1]; /* allocated memory following the struct */
};

ساختار Curl_hash {
  ساختار Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t size;
  uint8_t type; /* Curl_hash_type */
#ifdef DEBUGBUILD
  صحیح init;
#endif
};

ساختار Curl_hash_iterator {
  ساختار Curl_hash *hash;
  size_t slot_index;
  ساختار Curl_hash_element *current;
#ifdef DEBUGBUILD
  صحیح init;
#endif
};

CURLcode Curl_hash_global_init(تهی);

تهی Curl_hash_init(ساختار Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

تهی *Curl_hash_add(ساختار Curl_hash *h,
                    ثابت تهی *کلید, size_t کلید_len, تهی *p);
تهی *Curl_hash_add2(ساختار Curl_hash *h,
                     ثابت تهی *کلید, size_t کلید_len, تهی *p,
                     Curl_hash_elem_dtor dtor);
صحیح Curl_hash_delete(ساختار Curl_hash *h, ثابت تهی *کلید, size_t کلید_len);
تهی *Curl_hash_pick(ساختار Curl_hash *h, ثابت تهی *کلید, size_t کلید_len);

تهی Curl_hash_destroy(ساختار Curl_hash *h);
size_t Curl_hash_شمار(ساختار Curl_hash *h);
تهی Curl_hash_clean(ساختار Curl_hash *h);
تهی Curl_hash_clean_with_criterium(ساختار Curl_hash *h, تهی *user,
                                    صحیح (*comp)(تهی *, تهی *));
تهی Curl_hash_start_iterate(ساختار Curl_hash *hash,
                             ساختار Curl_hash_iterator *iter);
ساختار Curl_hash_element *Curl_hash_next_element(
  ساختار Curl_hash_iterator *iter);

تهی Curl_hash_print(ساختار Curl_hash *h, تهی (*func)(تهی *));

#endif /* HEADER_CURL_HASH_H */
