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
#அந்தர்பாவ "curl_setup.h"

/* An array of pointers with up to UINT32_MAX entries */
கட்டமைப்பு ptrarray {
  வெறுமை **data; /* #capacity array of pointers */
  uint32_t n;
  uint32_t capacity;
};

வெறுமை Curl_ptrarray_init(கட்டமைப்பு ptrarray *pa);

வெறுமை Curl_ptrarray_clear(கட்டமைப்பு ptrarray *pa);

uint32_t Curl_ptrarray_மொத்தம்(கட்டமைப்பு ptrarray *pa);

CURLcode Curl_ptrarray_add(கட்டமைப்பு ptrarray *pa, வெறுமை *ptr);

வெறுமை *Curl_ptrarray_remove(கட்டமைப்பு ptrarray *pa, uint32_t idx);

வெறுமை *Curl_ptrarray_get(கட்டமைப்பு ptrarray *pa, uint32_t idx);

#define CURL_PTRARRAY_மொத்தம்(a)    ((a)->n)
#define CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->data[(i)] : வெற்று)

#endif /* HEADER_CURL_PTRARRAY_H */
