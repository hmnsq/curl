#अगर_अघोषित HEADER_CURL_PTRARRAY_H
#घोषणा HEADER_CURL_PTRARRAY_H
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
#समावेश "curl_setup.h"

/* An array of pointers with up to UINT32_MAX entries */
काष्ठा ptrarray {
  व्योम **तथ्य; /* #capacity array of pointers */
  uint32_t n;
  uint32_t capacity;
};

व्योम Curl_ptrarray_init(काष्ठा ptrarray *pa);

व्योम Curl_ptrarray_clear(काष्ठा ptrarray *pa);

uint32_t Curl_ptrarray_गिनती(काष्ठा ptrarray *pa);

CURLcode Curl_ptrarray_add(काष्ठा ptrarray *pa, व्योम *ptr);

व्योम *Curl_ptrarray_remove(काष्ठा ptrarray *pa, uint32_t idx);

व्योम *Curl_ptrarray_get(काष्ठा ptrarray *pa, uint32_t idx);

#घोषणा CURL_PTRARRAY_गिनती(a)    ((a)->n)
#घोषणा CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->तथ्य[(i)] : शून्य)

#पूर्ण_अगर /* HEADER_CURL_PTRARRAY_H */
