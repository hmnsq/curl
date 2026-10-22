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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

/* An array of pointers with up to UINT32_MAX entries */
ସଂରଚନା ptrarray {
  ଶୂନ୍ୟ **data; /* #capacity array of pointers */
  uint32_t n;
  uint32_t capacity;
};

ଶୂନ୍ୟ Curl_ptrarray_init(ସଂରଚନା ptrarray *pa);

ଶୂନ୍ୟ Curl_ptrarray_clear(ସଂରଚନା ptrarray *pa);

uint32_t Curl_ptrarray_ଗଣନା(ସଂରଚନା ptrarray *pa);

CURLcode Curl_ptrarray_add(ସଂରଚନା ptrarray *pa, ଶୂନ୍ୟ *ptr);

ଶୂନ୍ୟ *Curl_ptrarray_remove(ସଂରଚନା ptrarray *pa, uint32_t idx);

ଶୂନ୍ୟ *Curl_ptrarray_get(ସଂରଚନା ptrarray *pa, uint32_t idx);

#define CURL_PTRARRAY_ଗଣନା(a)    ((a)->n)
#define CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->data[(i)] : ଖାଲି)

#endif /* HEADER_CURL_PTRARRAY_H */
