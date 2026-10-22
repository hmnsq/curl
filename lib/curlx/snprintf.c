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
#अन्तर्भाव "curlx/snprintf.h"

#यदि_परिभाषितम् _WIN32
#अन्तर्भाव <stdarg.h>

/* Simplified wrapper for the Windows platform to use the correct symbol and
   ensuring null-termination. Omit returning a length to keep it simple. */
शून्य curlx_win32_snprintf(वर्ण *buf, size_t maxlen, ध्रुव वर्ण *fmt, ...)
{
  va_list ap;
  यदि(!maxlen)
    प्रत्यागम;
  va_start(ap, fmt);
#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wformat-nonliteral"
#यदि_अन्तः
  /* !checksrc! disable BANNEDFUNC 1 */
  (शून्य)vsnprintf(buf, maxlen, fmt, ap);
#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः
  buf[maxlen - 1] = 0;
  va_end(ap);
}
#यदि_अन्तः /* _WIN32 */
