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
#ಅನ್ತರ್ಭಾವ "curlx/snprintf.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ಅನ್ತರ್ಭಾವ <stdarg.h>

/* Simplified wrapper for the Windows platform to use the correct symbol and
   ensuring null-termination. Omit returning a length to keep it simple. */
ಶೂನ್ಯ curlx_win32_snprintf(ಅಕ್ಷರ *buf, size_t maxlen, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  va_list ap;
  ಆದರೆ(!maxlen)
    ಹಿಂತಿರುಗಿ;
  va_start(ap, fmt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wformat-nonliteral"
#ಆದರೆ_ಅಂತ್ಯ
  /* !checksrc! disable BANNEDFUNC 1 */
  (ಶೂನ್ಯ)vsnprintf(buf, maxlen, fmt, ap);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  buf[maxlen - 1] = 0;
  va_end(ap);
}
#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */
