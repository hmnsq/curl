#ifndef HEADER_CURL_PTRARRAY_H
#define HEADER_CURL_PTRARRAY_H
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

/* An array of pointers with up to UINT32_MAX entries */
ساختار ptrarray {
  تهی **data; /* #capacity array of pointers */
  uint32_t n;
  uint32_t capacity;
};

تهی Curl_ptrarray_init(ساختار ptrarray *pa);

تهی Curl_ptrarray_clear(ساختار ptrarray *pa);

uint32_t Curl_ptrarray_شمار(ساختار ptrarray *pa);

CURLcode Curl_ptrarray_add(ساختار ptrarray *pa, تهی *ptr);

تهی *Curl_ptrarray_remove(ساختار ptrarray *pa, uint32_t idx);

تهی *Curl_ptrarray_get(ساختار ptrarray *pa, uint32_t idx);

#define CURL_PTRARRAY_شمار(a)    ((a)->n)
#define CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->data[(i)] : پوچ)

#endif /* HEADER_CURL_PTRARRAY_H */
