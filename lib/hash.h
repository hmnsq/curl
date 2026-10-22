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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*Curl_hash_dtor)(ಶೂನ್ಯ *);

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*Curl_hash_elem_dtor)(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p);

/* How keys in a hash are interpreted. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

ಸಂರಚನಾ Curl_hash_element {
  ಸಂರಚನಾ Curl_hash_element *next;
  ಶೂನ್ಯ *ptr;
  Curl_hash_elem_dtor dtor;
  size_t ಕೀಲಿ_len;
  ವರ್ಣ ಕೀಲಿ[1]; /* allocated memory following the struct */
};

ಸಂರಚನಾ Curl_hash {
  ಸಂರಚನಾ Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t size;
  uint8_t type; /* Curl_hash_type */
#ifdef DEBUGBUILD
  ಪೂರ್ಣಾಙ್ಕ init;
#endif
};

ಸಂರಚನಾ Curl_hash_iterator {
  ಸಂರಚನಾ Curl_hash *hash;
  size_t slot_index;
  ಸಂರಚನಾ Curl_hash_element *current;
#ifdef DEBUGBUILD
  ಪೂರ್ಣಾಙ್ಕ init;
#endif
};

CURLcode Curl_hash_global_init(ಶೂನ್ಯ);

ಶೂನ್ಯ Curl_hash_init(ಸಂರಚನಾ Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

ಶೂನ್ಯ *Curl_hash_add(ಸಂರಚನಾ Curl_hash *h,
                    ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p);
ಶೂನ್ಯ *Curl_hash_add2(ಸಂರಚನಾ Curl_hash *h,
                     ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p,
                     Curl_hash_elem_dtor dtor);
ಪೂರ್ಣಾಙ್ಕ Curl_hash_delete(ಸಂರಚನಾ Curl_hash *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len);
ಶೂನ್ಯ *Curl_hash_pick(ಸಂರಚನಾ Curl_hash *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len);

ಶೂನ್ಯ Curl_hash_destroy(ಸಂರಚನಾ Curl_hash *h);
size_t Curl_hash_ಎಣಿಕೆ(ಸಂರಚನಾ Curl_hash *h);
ಶೂನ್ಯ Curl_hash_clean(ಸಂರಚನಾ Curl_hash *h);
ಶೂನ್ಯ Curl_hash_clean_with_criterium(ಸಂರಚನಾ Curl_hash *h, ಶೂನ್ಯ *user,
                                    ಪೂರ್ಣಾಙ್ಕ (*comp)(ಶೂನ್ಯ *, ಶೂನ್ಯ *));
ಶೂನ್ಯ Curl_hash_start_iterate(ಸಂರಚನಾ Curl_hash *hash,
                             ಸಂರಚನಾ Curl_hash_iterator *iter);
ಸಂರಚನಾ Curl_hash_element *Curl_hash_next_element(
  ಸಂರಚನಾ Curl_hash_iterator *iter);

ಶೂನ್ಯ Curl_hash_print(ಸಂರಚನಾ Curl_hash *h, ಶೂನ್ಯ (*func)(ಶೂನ್ಯ *));

#endif /* HEADER_CURL_HASH_H */
