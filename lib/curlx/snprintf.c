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
#ضمن "curlx/snprintf.h"

#که_تعریف_شوی _WIN32
#ضمن <stdarg.h>

/* Simplified wrapper for the Windows platform to use the correct symbol and
   ensuring null-termination. Omit returning a length to keep it simple. */
فراغ curlx_win32_snprintf(حرف *buf, size_t maxlen, ثابت حرف *fmt, ...)
{
  va_list ap;
  إذا(!maxlen)
    أرجع;
  va_start(ap, fmt);
#که_تعریف_شوی CURL_HAVE_DIAG
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wformat-nonliteral"
#د_که_پای
  /* !checksrc! disable BANNEDFUNC 1 */
  (فراغ)vsnprintf(buf, maxlen, fmt, ap);
#که_تعریف_شوی CURL_HAVE_DIAG
#لارښوونه GCC diagnostic راوباسه
#د_که_پای
  buf[maxlen - 1] = 0;
  va_end(ap);
}
#د_که_پای /* _WIN32 */
