#اگر_تعریف_نشده سرآیند_CURL_PTRARRAY_H
#تعریف_کن سرآیند_CURL_PTRARRAY_H
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
#ضمن "curl_setup.h"

/* An array of pointers with up to UINT32_MAX entries */
بنية ptrarray {
  فراغ **داده; /* #capacity array of pointers */
  uint32_t n;
  uint32_t ظرفیت;
};

فراغ Curl_ptrarray_init(بنية ptrarray *pa);

فراغ Curl_ptrarray_clear(بنية ptrarray *pa);

uint32_t Curl_ptrarray_شمار(بنية ptrarray *pa);

CURLcode Curl_ptrarray_جمع_کن(بنية ptrarray *pa, فراغ *ptr);

فراغ *Curl_ptrarray_remove(بنية ptrarray *pa, uint32_t idx);

فراغ *Curl_ptrarray_get(بنية ptrarray *pa, uint32_t idx);

#تعریف_کن CURL_PTRARRAY_شمار(a)    ((a)->n)
#تعریف_کن CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->داده[(i)] : عدم)

#پایان_اگر /* HEADER_CURL_PTRARRAY_H */
