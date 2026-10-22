#यदि_अपरिभाषितम् शीर्षकम्_CURL_PTRARRAY_H
#परिभाषय शीर्षकम्_CURL_PTRARRAY_H
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
#अन्तर्भाव "curl_setup.h"

/* An array of pointers with up to UINT32_MAX entries */
संरचना ptrarray {
  शून्य **दत्त; /* #capacity array of pointers */
  uint32_t n;
  uint32_t क्षमता;
};

शून्य Curl_ptrarray_init(संरचना ptrarray *pa);

शून्य Curl_ptrarray_clear(संरचना ptrarray *pa);

uint32_t Curl_ptrarray_गणनम्(संरचना ptrarray *pa);

CURLcode Curl_ptrarray_सङ्कलय(संरचना ptrarray *pa, शून्य *ptr);

शून्य *Curl_ptrarray_remove(संरचना ptrarray *pa, uint32_t idx);

शून्य *Curl_ptrarray_get(संरचना ptrarray *pa, uint32_t idx);

#परिभाषय CURL_PTRARRAY_गणनम्(a)    ((a)->n)
#परिभाषय CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->दत्त[(i)] : रिक्त)

#यदि_अन्तः /* HEADER_CURL_PTRARRAY_H */
