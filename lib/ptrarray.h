#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_PTRARRAY_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_PTRARRAY_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

/* An array of pointers with up to UINT32_MAX entries */
ರಚನೆ ptrarray {
  ಶೂನ್ಯ **ದತ್ತಾಂಶ; /* #capacity array of pointers */
  uint32_t n;
  uint32_t ಸಾಮರ್ಥ್ಯ;
};

ಶೂನ್ಯ Curl_ptrarray_init(ರಚನೆ ptrarray *pa);

ಶೂನ್ಯ Curl_ptrarray_clear(ರಚನೆ ptrarray *pa);

uint32_t Curl_ptrarray_ಎಣಿಕೆ(ರಚನೆ ptrarray *pa);

CURLcode Curl_ptrarray_ಕೂಡಿಸು(ರಚನೆ ptrarray *pa, ಶೂನ್ಯ *ptr);

ಶೂನ್ಯ *Curl_ptrarray_remove(ರಚನೆ ptrarray *pa, uint32_t idx);

ಶೂನ್ಯ *Curl_ptrarray_get(ರಚನೆ ptrarray *pa, uint32_t idx);

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_PTRARRAY_ಎಣಿಕೆ(a)    ((a)->n)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_PTRARRAY_GET(a, i)   (((i) < ((a)->n)) ? (a)->ದತ್ತಾಂಶ[(i)] : ಖಾಲಿ)

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_PTRARRAY_H */
