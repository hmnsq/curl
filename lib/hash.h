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

প্রকারলেখ শূন্য (*Curl_hash_dtor)(শূন্য *);

প্রকারলেখ শূন্য (*Curl_hash_elem_dtor)(ধ্রুবক শূন্য *চাবি, size_t চাবি_len, শূন্য *p);

/* How keys in a hash are interpreted. */
প্রকারলেখ গণনা {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

কাঠামো Curl_hash_element {
  কাঠামো Curl_hash_element *পরবর্তী;
  শূন্য *ptr;
  Curl_hash_elem_dtor dtor;
  size_t চাবি_len;
  অক্ষর চাবি[1]; /* allocated memory following the struct */
};

কাঠামো Curl_hash {
  কাঠামো Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t আকার_2;
  uint8_t type; /* Curl_hash_type */
#ifdef DEBUGBUILD
  পূর্ণসংখ্যা init;
#endif
};

কাঠামো Curl_hash_iterator {
  কাঠামো Curl_hash *hash;
  size_t slot_সূচক;
  কাঠামো Curl_hash_element *current;
#ifdef DEBUGBUILD
  পূর্ণসংখ্যা init;
#endif
};

CURLcode Curl_hash_global_init(শূন্য);

শূন্য Curl_hash_init(কাঠামো Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

শূন্য *Curl_hash_add(কাঠামো Curl_hash *h,
                    ধ্রুবক শূন্য *চাবি, size_t চাবি_len, শূন্য *p);
শূন্য *Curl_hash_add2(কাঠামো Curl_hash *h,
                     ধ্রুবক শূন্য *চাবি, size_t চাবি_len, শূন্য *p,
                     Curl_hash_elem_dtor dtor);
পূর্ণসংখ্যা Curl_hash_delete(কাঠামো Curl_hash *h, ধ্রুবক শূন্য *চাবি, size_t চাবি_len);
শূন্য *Curl_hash_pick(কাঠামো Curl_hash *h, ধ্রুবক শূন্য *চাবি, size_t চাবি_len);

শূন্য Curl_hash_destroy(কাঠামো Curl_hash *h);
size_t Curl_hash_গুনতি(কাঠামো Curl_hash *h);
শূন্য Curl_hash_clean(কাঠামো Curl_hash *h);
শূন্য Curl_hash_clean_with_criterium(কাঠামো Curl_hash *h, শূন্য *user,
                                    পূর্ণসংখ্যা (*comp)(শূন্য *, শূন্য *));
শূন্য Curl_hash_start_iterate(কাঠামো Curl_hash *hash,
                             কাঠামো Curl_hash_iterator *iter);
কাঠামো Curl_hash_element *Curl_hash_পরবর্তী_element(
  কাঠামো Curl_hash_iterator *iter);

শূন্য Curl_hash_print(কাঠামো Curl_hash *h, শূন্য (*func)(শূন্য *));

#endif /* HEADER_CURL_HASH_H */
