#ifndef HEADER_CURLX_SNPRINTF_H
#define HEADER_CURLX_SNPRINTF_H
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

/* Raw snprintf() for curlx */

#ifdef _WIN32
வெறுமை curlx_win32_snprintf(எழுத்து *buf, size_t maxlen, மாறிலி எழுத்து *fmt, ...)
  CURL_PRINTF(3, 4);
#endif

#ifdef WITHOUT_LIBCURL /* when built for the test servers */
#ifdef _WIN32
#define SNPRINTF curlx_win32_snprintf
#இல்லையெனில்
#define SNPRINTF snprintf
#endif
#இல்லையெனில் /* !WITHOUT_LIBCURL */
#அந்தர்பாவ <curl/mprintf.h>
#define SNPRINTF curl_msnprintf
#endif /* WITHOUT_LIBCURL */
#endif /* HEADER_CURLX_SNPRINTF_H */
