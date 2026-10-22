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
#അന്തര്ഭാവ "curlx/snprintf.h"

#ifdef _WIN32
#അന്തര്ഭാവ <stdarg.h>

/* Simplified wrapper for the Windows platform to use the correct symbol and
   ensuring null-termination. Omit returning a length to keep it simple. */
ശൂന്യം curlx_win32_snprintf(അക്ഷരം *buf, size_t maxlen, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  va_list ap;
  എങ്കിൽ(!maxlen)
    തിരികെ;
  va_start(ap, fmt);
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
  /* !checksrc! disable BANNEDFUNC 1 */
  (ശൂന്യം)vsnprintf(buf, maxlen, fmt, ap);
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic എടുക്കുക
#endif
  buf[maxlen - 1] = 0;
  va_end(ap);
}
#endif /* _WIN32 */
