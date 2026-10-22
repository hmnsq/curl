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
#شامل "curlx/snprintf.h"

#اگر_تعریف_شدہ _WIN32
#شامل <stdarg.h>

/* Simplified wrapper for the Windows platform to use the correct symbol and
   ensuring null-termination. Omit returning a length to keep it simple. */
خالی curlx_win32_snprintf(حرف *buf, size_t maxlen, ثابت حرف *fmt, ...)
{
  va_list ap;
  اگر(!maxlen)
    واپس;
  va_start(ap, fmt);
#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wformat-nonliteral"
#اگر_ختم
  /* !checksrc! disable BANNEDFUNC 1 */
  (خالی)vsnprintf(buf, maxlen, fmt, ap);
#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic نکالو
#اگر_ختم
  buf[maxlen - 1] = 0;
  va_end(ap);
}
#اگر_ختم /* _WIN32 */
