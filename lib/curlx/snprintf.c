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
#અન્તર્ભાવ "curlx/snprintf.h"

#ifdef _WIN32
#અન્તર્ભાવ <stdarg.h>

/* Simplified wrapper for the Windows platform to use the correct symbol and
   ensuring null-termination. Omit returning a length to keep it simple. */
શૂન્ય curlx_win32_snprintf(અક્ષર *buf, size_t maxlen, અચલ અક્ષર *fmt, ...)
{
  va_list ap;
  જો(!maxlen)
    પાછા;
  va_start(ap, fmt);
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
  /* !checksrc! disable BANNEDFUNC 1 */
  (શૂન્ય)vsnprintf(buf, maxlen, fmt, ap);
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic કાઢો
#endif
  buf[maxlen - 1] = 0;
  va_end(ap);
}
#endif /* _WIN32 */
