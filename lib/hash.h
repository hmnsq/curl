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
#অন্তর্ভাব "curl_setup.h"

প্রকারনির্দেশ শূন্য (*Curl_hash_dtor)(শূন্য *);

প্রকারনির্দেশ শূন্য (*Curl_hash_elem_dtor)(ধ্রুব শূন্য *চাবি, size_t চাবি_len, শূন্য *p);

/* How keys in a hash are interpreted. */
প্রকারনির্দেশ গণনা {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

সংরচনা Curl_hash_element {
  সংরচনা Curl_hash_element *next;
  শূন্য *ptr;
  Curl_hash_elem_dtor dtor;
  size_t চাবি_len;
  বর্ণ চাবি[1]; /* allocated memory following the struct */
};

সংরচনা Curl_hash {
  সংরচনা Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t size;
  uint8_t type; /* Curl_hash_type */
#ifdef DEBUGBUILD
  পূর্ণাঙ্ক init;
#endif
};

সংরচনা Curl_hash_iterator {
  সংরচনা Curl_hash *hash;
  size_t slot_index;
  সংরচনা Curl_hash_element *current;
#ifdef DEBUGBUILD
  পূর্ণাঙ্ক init;
#endif
};

CURLcode Curl_hash_global_init(শূন্য);

শূন্য Curl_hash_init(সংরচনা Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

শূন্য *Curl_hash_add(সংরচনা Curl_hash *h,
                    ধ্রুব শূন্য *চাবি, size_t চাবি_len, শূন্য *p);
শূন্য *Curl_hash_add2(সংরচনা Curl_hash *h,
                     ধ্রুব শূন্য *চাবি, size_t চাবি_len, শূন্য *p,
                     Curl_hash_elem_dtor dtor);
পূর্ণাঙ্ক Curl_hash_delete(সংরচনা Curl_hash *h, ধ্রুব শূন্য *চাবি, size_t চাবি_len);
শূন্য *Curl_hash_pick(সংরচনা Curl_hash *h, ধ্রুব শূন্য *চাবি, size_t চাবি_len);

শূন্য Curl_hash_destroy(সংরচনা Curl_hash *h);
size_t Curl_hash_গুনতি(সংরচনা Curl_hash *h);
শূন্য Curl_hash_clean(সংরচনা Curl_hash *h);
শূন্য Curl_hash_clean_with_criterium(সংরচনা Curl_hash *h, শূন্য *user,
                                    পূর্ণাঙ্ক (*comp)(শূন্য *, শূন্য *));
শূন্য Curl_hash_start_iterate(সংরচনা Curl_hash *hash,
                             সংরচনা Curl_hash_iterator *iter);
সংরচনা Curl_hash_element *Curl_hash_next_element(
  সংরচনা Curl_hash_iterator *iter);

শূন্য Curl_hash_print(সংরচনা Curl_hash *h, শূন্য (*func)(শূন্য *));

#endif /* HEADER_CURL_HASH_H */
