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
#অন্তর্ভাৱ "curl_setup.h"

/* An array of pointers with up to UINT32_MAX entries */
সংরচনা ptrarray {
  শূন্য **data; /* #capacity array of pointers */
  uint32_t n;
  uint32_t capacity;
};

শূন্য Curl_ptrarray_init(সংরচনা ptrarray *pa);

শূন্য Curl_ptrarray_clear(সংরচনা ptrarray *pa);

uint32_t Curl_ptrarray_গুনতি(সংরচনা ptrarray *pa);

CURLcode Curl_ptrarray_add(সংরচনা ptrarray *pa, শূন্য *ptr);

শূন্য *Curl_ptrarray_remove(সংরচনা ptrarray *pa, uint32_t idx);

শূন্য *Curl_ptrarray_get(সংরচনা ptrarray *pa, uint32_t idx);

#define CURL_PTRARRAY_গুনতি(a)    ((a)->n)
#define CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->data[(i)] : রিক্ত)

#endif /* HEADER_CURL_PTRARRAY_H */
