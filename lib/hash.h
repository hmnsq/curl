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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*Curl_hash_dtor)(ଶୂନ୍ୟ *);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*Curl_hash_elem_dtor)(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len, ଶୂନ୍ୟ *p);

/* How keys in a hash are interpreted. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

ସଂରଚନା Curl_hash_element {
  ସଂରଚନା Curl_hash_element *next;
  ଶୂନ୍ୟ *ptr;
  Curl_hash_elem_dtor dtor;
  size_t ଚାବି_len;
  ଅକ୍ଷର ଚାବି[1]; /* allocated memory following the struct */
};

ସଂରଚନା Curl_hash {
  ସଂରଚନା Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t size;
  uint8_t type; /* Curl_hash_type */
#ifdef DEBUGBUILD
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା init;
#endif
};

ସଂରଚନା Curl_hash_iterator {
  ସଂରଚନା Curl_hash *hash;
  size_t slot_index;
  ସଂରଚନା Curl_hash_element *current;
#ifdef DEBUGBUILD
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା init;
#endif
};

CURLcode Curl_hash_global_init(ଶୂନ୍ୟ);

ଶୂନ୍ୟ Curl_hash_init(ସଂରଚନା Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

ଶୂନ୍ୟ *Curl_hash_add(ସଂରଚନା Curl_hash *h,
                    ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len, ଶୂନ୍ୟ *p);
ଶୂନ୍ୟ *Curl_hash_add2(ସଂରଚନା Curl_hash *h,
                     ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len, ଶୂନ୍ୟ *p,
                     Curl_hash_elem_dtor dtor);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_hash_delete(ସଂରଚନା Curl_hash *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len);
ଶୂନ୍ୟ *Curl_hash_pick(ସଂରଚନା Curl_hash *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len);

ଶୂନ୍ୟ Curl_hash_destroy(ସଂରଚନା Curl_hash *h);
size_t Curl_hash_ଗଣନା(ସଂରଚନା Curl_hash *h);
ଶୂନ୍ୟ Curl_hash_clean(ସଂରଚନା Curl_hash *h);
ଶୂନ୍ୟ Curl_hash_clean_with_criterium(ସଂରଚନା Curl_hash *h, ଶୂନ୍ୟ *user,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*comp)(ଶୂନ୍ୟ *, ଶୂନ୍ୟ *));
ଶୂନ୍ୟ Curl_hash_start_iterate(ସଂରଚନା Curl_hash *hash,
                             ସଂରଚନା Curl_hash_iterator *iter);
ସଂରଚନା Curl_hash_element *Curl_hash_next_element(
  ସଂରଚନା Curl_hash_iterator *iter);

ଶୂନ୍ୟ Curl_hash_print(ସଂରଚନା Curl_hash *h, ଶୂନ୍ୟ (*func)(ଶୂନ୍ୟ *));

#endif /* HEADER_CURL_HASH_H */
