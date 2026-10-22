#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_PTRARRAY_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_PTRARRAY_H
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

/* An array of pointers with up to UINT32_MAX entries */
ਬਣਤਰ ptrarray {
  ਖਾਲੀ **ਅੰਕੜੇ; /* #capacity array of pointers */
  uint32_t n;
  uint32_t ਸਮਰੱਥਾ;
};

ਖਾਲੀ Curl_ptrarray_init(ਬਣਤਰ ptrarray *pa);

ਖਾਲੀ Curl_ptrarray_clear(ਬਣਤਰ ptrarray *pa);

uint32_t Curl_ptrarray_ਗਿਣਤੀ(ਬਣਤਰ ptrarray *pa);

CURLcode Curl_ptrarray_ਜੋੜੋ(ਬਣਤਰ ptrarray *pa, ਖਾਲੀ *ptr);

ਖਾਲੀ *Curl_ptrarray_remove(ਬਣਤਰ ptrarray *pa, uint32_t idx);

ਖਾਲੀ *Curl_ptrarray_get(ਬਣਤਰ ptrarray *pa, uint32_t idx);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PTRARRAY_ਗਿਣਤੀ(a)    ((a)->n)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->ਅੰਕੜੇ[(i)] : ਨਿਰਾ)

#ਜੇ_ਅੰਤ /* HEADER_CURL_PTRARRAY_H */
