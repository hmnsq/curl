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
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/snprintf.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>

/* Simplified wrapper for the Windows platform to use the correct symbol and
   ensuring null-termination. Omit returning a length to keep it simple. */
ਖਾਲੀ curlx_win32_snprintf(ਅੱਖਰ *buf, size_t maxlen, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  va_list ap;
  ਜੇ(!maxlen)
    ਵਾਪਸ;
  va_start(ap, fmt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wformat-nonliteral"
#ਜੇ_ਅੰਤ
  /* !checksrc! disable BANNEDFUNC 1 */
  (ਖਾਲੀ)vsnprintf(buf, maxlen, fmt, ap);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  buf[maxlen - 1] = 0;
  va_end(ap);
}
#ਜੇ_ਅੰਤ /* _WIN32 */
