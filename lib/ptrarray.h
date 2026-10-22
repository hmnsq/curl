#నిర్వచితం_కాకపోతే శీర్షిక_CURL_PTRARRAY_H
#నిర్వచించు శీర్షిక_CURL_PTRARRAY_H
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
#అన్తర్భావ "curl_setup.h"

/* An array of pointers with up to UINT32_MAX entries */
నిర్మాణం ptrarray {
  శూన్య **దత్తాంశం; /* #capacity array of pointers */
  uint32_t n;
  uint32_t సామర్థ్యం;
};

శూన్య Curl_ptrarray_init(నిర్మాణం ptrarray *pa);

శూన్య Curl_ptrarray_clear(నిర్మాణం ptrarray *pa);

uint32_t Curl_ptrarray_లెక్క(నిర్మాణం ptrarray *pa);

CURLcode Curl_ptrarray_కూడు(నిర్మాణం ptrarray *pa, శూన్య *ptr);

శూన్య *Curl_ptrarray_remove(నిర్మాణం ptrarray *pa, uint32_t idx);

శూన్య *Curl_ptrarray_get(నిర్మాణం ptrarray *pa, uint32_t idx);

#నిర్వచించు CURL_PTRARRAY_లెక్క(a)    ((a)->n)
#నిర్వచించు CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->దత్తాంశం[(i)] : ఖాళీ)

#అయితే_ముగింపు /* HEADER_CURL_PTRARRAY_H */
