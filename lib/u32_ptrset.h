#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_U32_PTRSET_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_U32_PTRSET_H
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ Curl_u32_ptrset_dtor(uint32_t id, ਖਾਲੀ *ptr);

/* A set that can hold up to INT32_MAX+1 pointers identified by an `id'.
 * Setting a pointer for an existing id replaces the previous one.
 * Getting the pointer for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
ਬਣਤਰ u32_ptrset {
  ਖਾਲੀ **ਅੰਕੜੇ; /* #slots array of pointers */
  uint32_t *ids; /* #slots array of `id` values */
  uint32_t *psl; /* #slots array of "probe sequence length" values */
  uint32_t slotmask;
  uint32_t ਗਿਣਤੀ_2;
  Curl_u32_ptrset_dtor *dtor;
  uint8_t slotbits;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  int32_t init;
#ਜੇ_ਅੰਤ
};

ਖਾਲੀ Curl_u32_ptrset_init(ਬਣਤਰ u32_ptrset *ਸੈੱਟ,
                          Curl_u32_ptrset_dtor *dtor);
ਖਾਲੀ Curl_u32_ptrset_clear(ਬਣਤਰ u32_ptrset *ਸੈੱਟ);

uint32_t Curl_u32_ptrset_ਗਿਣਤੀ(ਬਣਤਰ u32_ptrset *ਸੈੱਟ);

ਖਾਲੀ *Curl_u32_ptrset_get(ਬਣਤਰ u32_ptrset *ਸੈੱਟ, uint32_t id);

/* Set pointer for id. */
CURLcode Curl_u32_ptrset_ਸੈੱਟ(ਬਣਤਰ u32_ptrset *ਸੈੱਟ,
                             uint32_t id, ਖਾਲੀ *ptr);

ਖਾਲੀ Curl_u32_ptrset_unset(ਬਣਤਰ u32_ptrset *ਸੈੱਟ, uint32_t id);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ bool Curl_u32_ptrset_ਦੇਖੋ_cb(uint32_t id, ਖਾਲੀ *ptr,
                                       ਖਾਲੀ *user_ਅੰਕੜੇ);

/* Visit all entries in the set until the callback returns FALSE */
ਖਾਲੀ Curl_u32_ptrset_ਦੇਖੋ(ਬਣਤਰ u32_ptrset *ਸੈੱਟ,
                            Curl_u32_ptrset_ਦੇਖੋ_cb *cb,
                            ਖਾਲੀ *user_ਅੰਕੜੇ);

#ਜੇ_ਅੰਤ /* HEADER_CURL_U32_PTRSET_H */
