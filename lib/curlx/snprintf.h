#यदि_अपरिभाषितम् शीर्षकम्_CURLX_SNPRINTF_H
#परिभाषय शीर्षकम्_CURLX_SNPRINTF_H
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

/* Raw snprintf() for curlx */

#यदि_परिभाषितम् _WIN32
शून्य curlx_win32_snprintf(वर्ण *buf, size_t maxlen, ध्रुव वर्ण *fmt, ...)
  CURL_PRINTF(3, 4);
#यदि_अन्तः

#यदि_परिभाषितम् WITHOUT_LIBCURL /* when built for the test servers */
#यदि_परिभाषितम् _WIN32
#परिभाषय SNPRINTF curlx_win32_snprintf
#अन्यथा
#परिभाषय SNPRINTF snprintf
#यदि_अन्तः
#अन्यथा /* !WITHOUT_LIBCURL */
#अन्तर्भाव <curl/mprintf.h>
#परिभाषय SNPRINTF curl_msnprintf
#यदि_अन्तः /* WITHOUT_LIBCURL */
#यदि_अन्तः /* HEADER_CURLX_SNPRINTF_H */
