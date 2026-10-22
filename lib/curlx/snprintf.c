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
#समावेश "curlx/snprintf.h"

#अगर_घोषित _WIN32
#समावेश <stdarg.h>

/* Simplified wrapper for the Windows platform to use the correct symbol and
   ensuring null-termination. Omit returning a length to keep it simple. */
व्योम curlx_win32_snprintf(अक्षर *buf, माप_प्रकार maxlen, स्थिर अक्षर *fmt, ...)
{
  बहु_सूची ap;
  अगर(!maxlen)
    वापस;
  बहु_शुरू(ap, fmt);
#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wformat-nonliteral"
#पूर्ण_अगर
  /* !checksrc! disable BANNEDFUNC 1 */
  (व्योम)vsnprintf(buf, maxlen, fmt, ap);
#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
  buf[maxlen - 1] = 0;
  बहु_पूर्ण(ap);
}
#पूर्ण_अगर /* _WIN32 */
