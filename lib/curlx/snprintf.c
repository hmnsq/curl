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
#అన్తర్భావ "curlx/snprintf.h"

#నిర్వచితమైతే _WIN32
#అన్తర్భావ <stdarg.h>

/* Simplified wrapper for the Windows platform to use the correct symbol and
   ensuring null-termination. Omit returning a length to keep it simple. */
శూన్య curlx_win32_snprintf(అక్షరం *buf, size_t maxlen, స్థిరరాశి అక్షరం *fmt, ...)
{
  va_list ap;
  ఒకవేళ(!maxlen)
    తిరిగి;
  va_start(ap, fmt);
#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wformat-nonliteral"
#అయితే_ముగింపు
  /* !checksrc! disable BANNEDFUNC 1 */
  (శూన్య)vsnprintf(buf, maxlen, fmt, ap);
#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
  buf[maxlen - 1] = 0;
  va_end(ap);
}
#అయితే_ముగింపు /* _WIN32 */
