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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_ਸਤਰ(ਬਣਤਰ dynbuf *buf, FILE *ਨਿਵੇਸ਼, bool *eof)
{
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ ਬਫਰ[128];
  curlx_dyn_reset(buf);
  ਜਦੋਂ(1) {
    size_t rlen;
    ਅਚਲ ਅੱਖਰ *b = fgets(ਬਫਰ, ਆਕਾਰ(ਬਫਰ), ਨਿਵੇਸ਼);
    ਜੇ(!b && ferror(ਨਿਵੇਸ਼))
      ਵਾਪਸ CURLE_READ_ERROR;

    *eof = feof(ਨਿਵੇਸ਼);

    rlen = b ? ਲੜੀ_ਲੰਬਾਈ(b) : 0;
    ਜੇ(rlen) {
      ਨਤੀਜਾ = curlx_dyn_addn(buf, b, rlen);
      ਜੇ(ਨਤੀਜਾ)
        /* too long line or out of memory */
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    ਜੇ(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      ਵਾਪਸ CURLE_OK; /* all good */
    ਜੇ(*eof)
      /* append a newline */
      ਵਾਪਸ curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#ਜੇ_ਅੰਤ /* if not disabled */
