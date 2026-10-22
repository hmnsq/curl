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
#અન્તર્ભાવ "curl_setup.h"

પ્રકારવ્યાખ્યા શૂન્ય (*Curl_hash_dtor)(શૂન્ય *);

પ્રકારવ્યાખ્યા શૂન્ય (*Curl_hash_elem_dtor)(અચલ શૂન્ય *ચાવી, size_t ચાવી_len, શૂન્ય *p);

/* How keys in a hash are interpreted. */
પ્રકારવ્યાખ્યા ગણના {
  CURL_HASH_TYPE_BYTES,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_LAST
} Curl_hash_type;

સંરચના Curl_hash_element {
  સંરચના Curl_hash_element *next;
  શૂન્ય *ptr;
  Curl_hash_elem_dtor dtor;
  size_t ચાવી_len;
  અક્ષર ચાવી[1]; /* allocated memory following the struct */
};

સંરચના Curl_hash {
  સંરચના Curl_hash_element **table;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t size;
  uint8_t type; /* Curl_hash_type */
#ifdef DEBUGBUILD
  પૂર્ણાંક init;
#endif
};

સંરચના Curl_hash_iterator {
  સંરચના Curl_hash *hash;
  size_t slot_index;
  સંરચના Curl_hash_element *current;
#ifdef DEBUGBUILD
  પૂર્ણાંક init;
#endif
};

CURLcode Curl_hash_global_init(શૂન્ય);

શૂન્ય Curl_hash_init(સંરચના Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

શૂન્ય *Curl_hash_add(સંરચના Curl_hash *h,
                    અચલ શૂન્ય *ચાવી, size_t ચાવી_len, શૂન્ય *p);
શૂન્ય *Curl_hash_add2(સંરચના Curl_hash *h,
                     અચલ શૂન્ય *ચાવી, size_t ચાવી_len, શૂન્ય *p,
                     Curl_hash_elem_dtor dtor);
પૂર્ણાંક Curl_hash_delete(સંરચના Curl_hash *h, અચલ શૂન્ય *ચાવી, size_t ચાવી_len);
શૂન્ય *Curl_hash_pick(સંરચના Curl_hash *h, અચલ શૂન્ય *ચાવી, size_t ચાવી_len);

શૂન્ય Curl_hash_destroy(સંરચના Curl_hash *h);
size_t Curl_hash_ગણતરી(સંરચના Curl_hash *h);
શૂન્ય Curl_hash_clean(સંરચના Curl_hash *h);
શૂન્ય Curl_hash_clean_with_criterium(સંરચના Curl_hash *h, શૂન્ય *user,
                                    પૂર્ણાંક (*comp)(શૂન્ય *, શૂન્ય *));
શૂન્ય Curl_hash_start_iterate(સંરચના Curl_hash *hash,
                             સંરચના Curl_hash_iterator *iter);
સંરચના Curl_hash_element *Curl_hash_next_element(
  સંરચના Curl_hash_iterator *iter);

શૂન્ય Curl_hash_print(સંરચના Curl_hash *h, શૂન્ય (*func)(શૂન્ય *));

#endif /* HEADER_CURL_HASH_H */
