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
#அந்தர்பாவ "curl_setup.h"

வகைவரை வெறுமை (*Curl_hash_dtor)(வெறுமை *);

வகைவரை வெறுமை (*Curl_hash_elem_dtor)(மாறிலி வெறுமை *சாவி, size_t சாவி_len, வெறுமை *p);

/* How keys in a hash are interpreted. */
வகைவரை எண்ணிக்கை {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

கட்டமைப்பு Curl_hash_element {
  கட்டமைப்பு Curl_hash_element *next;
  வெறுமை *ptr;
  Curl_hash_elem_dtor dtor;
  size_t சாவி_len;
  எழுத்து சாவி[1]; /* allocated memory following the struct */
};

கட்டமைப்பு Curl_hash {
  கட்டமைப்பு Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t size;
  uint8_t type; /* Curl_hash_type */
#ifdef DEBUGBUILD
  முழு init;
#endif
};

கட்டமைப்பு Curl_hash_iterator {
  கட்டமைப்பு Curl_hash *hash;
  size_t slot_index;
  கட்டமைப்பு Curl_hash_element *current;
#ifdef DEBUGBUILD
  முழு init;
#endif
};

CURLcode Curl_hash_global_init(வெறுமை);

வெறுமை Curl_hash_init(கட்டமைப்பு Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

வெறுமை *Curl_hash_add(கட்டமைப்பு Curl_hash *h,
                    மாறிலி வெறுமை *சாவி, size_t சாவி_len, வெறுமை *p);
வெறுமை *Curl_hash_add2(கட்டமைப்பு Curl_hash *h,
                     மாறிலி வெறுமை *சாவி, size_t சாவி_len, வெறுமை *p,
                     Curl_hash_elem_dtor dtor);
முழு Curl_hash_delete(கட்டமைப்பு Curl_hash *h, மாறிலி வெறுமை *சாவி, size_t சாவி_len);
வெறுமை *Curl_hash_pick(கட்டமைப்பு Curl_hash *h, மாறிலி வெறுமை *சாவி, size_t சாவி_len);

வெறுமை Curl_hash_destroy(கட்டமைப்பு Curl_hash *h);
size_t Curl_hash_மொத்தம்(கட்டமைப்பு Curl_hash *h);
வெறுமை Curl_hash_clean(கட்டமைப்பு Curl_hash *h);
வெறுமை Curl_hash_clean_with_criterium(கட்டமைப்பு Curl_hash *h, வெறுமை *user,
                                    முழு (*comp)(வெறுமை *, வெறுமை *));
வெறுமை Curl_hash_start_iterate(கட்டமைப்பு Curl_hash *hash,
                             கட்டமைப்பு Curl_hash_iterator *iter);
கட்டமைப்பு Curl_hash_element *Curl_hash_next_element(
  கட்டமைப்பு Curl_hash_iterator *iter);

வெறுமை Curl_hash_print(கட்டமைப்பு Curl_hash *h, வெறுமை (*func)(வெறுமை *));

#endif /* HEADER_CURL_HASH_H */
