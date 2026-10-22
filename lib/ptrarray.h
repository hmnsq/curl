#اگر_غیر_تعریف_شدہ سرخی_CURL_PTRARRAY_H
#تعریف_کرو سرخی_CURL_PTRARRAY_H
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
ساخت ptrarray {
  خالی **ڈیٹا; /* #capacity array of pointers */
  uint32_t n;
  uint32_t استعداد;
};

خالی Curl_ptrarray_init(ساخت ptrarray *pa);

خالی Curl_ptrarray_clear(ساخت ptrarray *pa);

uint32_t Curl_ptrarray_گنتی(ساخت ptrarray *pa);

CURLcode Curl_ptrarray_جوڑو(ساخت ptrarray *pa, خالی *ptr);

خالی *Curl_ptrarray_remove(ساخت ptrarray *pa, uint32_t idx);

خالی *Curl_ptrarray_get(ساخت ptrarray *pa, uint32_t idx);

#تعریف_کرو CURL_PTRARRAY_گنتی(a)    ((a)->n)
#تعریف_کرو CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->ڈیٹا[(i)] : تہی)

#اگر_ختم /* HEADER_CURL_PTRARRAY_H */
