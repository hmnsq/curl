#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_HASH_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_HASH_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*Curl_hash_dtor)(ਖਾਲੀ *);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*Curl_hash_elem_dtor)(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len, ਖਾਲੀ *p);

/* How keys in a hash are interpreted. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_HASH_TYPE_ਬਾਈਟਾਂ,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_ਆਖਰੀ
} Curl_hash_type;

ਬਣਤਰ Curl_hash_element {
  ਬਣਤਰ Curl_hash_element *ਅਗਲਾ;
  ਖਾਲੀ *ptr;
  Curl_hash_elem_dtor dtor;
  size_t ਕੁੰਜੀ_len;
  ਅੱਖਰ ਕੁੰਜੀ[1]; /* allocated memory following the struct */
};

ਬਣਤਰ Curl_hash {
  ਬਣਤਰ Curl_hash_element **ਸਾਰਣੀ;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t ਮਾਪ;
  uint8_t type; /* Curl_hash_type */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ init;
#ਜੇ_ਅੰਤ
};

ਬਣਤਰ Curl_hash_iterator {
  ਬਣਤਰ Curl_hash *hash;
  size_t ਥਾਂ_ਸੂਚਕਾਂਕ;
  ਬਣਤਰ Curl_hash_element *current;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ init;
#ਜੇ_ਅੰਤ
};

CURLcode Curl_hash_global_init(ਖਾਲੀ);

ਖਾਲੀ Curl_hash_init(ਬਣਤਰ Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

ਖਾਲੀ *Curl_hash_ਜੋੜੋ(ਬਣਤਰ Curl_hash *h,
                    ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len, ਖਾਲੀ *p);
ਖਾਲੀ *Curl_hash_add2(ਬਣਤਰ Curl_hash *h,
                     ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len, ਖਾਲੀ *p,
                     Curl_hash_elem_dtor dtor);
ਪੂਰਨ Curl_hash_delete(ਬਣਤਰ Curl_hash *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len);
ਖਾਲੀ *Curl_hash_pick(ਬਣਤਰ Curl_hash *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len);

ਖਾਲੀ Curl_hash_destroy(ਬਣਤਰ Curl_hash *h);
size_t Curl_hash_ਗਿਣਤੀ(ਬਣਤਰ Curl_hash *h);
ਖਾਲੀ Curl_hash_clean(ਬਣਤਰ Curl_hash *h);
ਖਾਲੀ Curl_hash_clean_with_criterium(ਬਣਤਰ Curl_hash *h, ਖਾਲੀ *user,
                                    ਪੂਰਨ (*comp)(ਖਾਲੀ *, ਖਾਲੀ *));
ਖਾਲੀ Curl_hash_ਸ਼ੁਰੂ_iterate(ਬਣਤਰ Curl_hash *hash,
                             ਬਣਤਰ Curl_hash_iterator *iter);
ਬਣਤਰ Curl_hash_element *Curl_hash_ਅਗਲਾ_element(
  ਬਣਤਰ Curl_hash_iterator *iter);

ਖਾਲੀ Curl_hash_print(ਬਣਤਰ Curl_hash *h, ਖਾਲੀ (*func)(ਖਾਲੀ *));

#ਜੇ_ਅੰਤ /* HEADER_CURL_HASH_H */
