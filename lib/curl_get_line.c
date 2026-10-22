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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#ଅନ୍ତର୍ଭାଵ "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_ଧାଡ଼ି(ସଂରଚନା dynbuf *buf, FILE *ନିବେଶ, bool *eof)
{
  CURLcode result;
  ଅକ୍ଷର buffer[128];
  curlx_dyn_reset(buf);
  ଯେତେବେଳେ(1) {
    size_t rlen;
    ଅଚଳ ଅକ୍ଷର *b = fgets(buffer, ଆକାର(buffer), ନିବେଶ);
    ଯଦି(!b && ferror(ନିବେଶ))
      ଫେରନ୍ତୁ CURLE_READ_ERROR;

    *eof = feof(ନିବେଶ);

    rlen = b ? strlen(b) : 0;
    ଯଦି(rlen) {
      result = curlx_dyn_addn(buf, b, rlen);
      ଯଦି(result)
        /* too long line or out of memory */
        ଫେରନ୍ତୁ result;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    ଯଦି(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      ଫେରନ୍ତୁ CURLE_OK; /* all good */
    ଯଦି(*eof)
      /* append a newline */
      ଫେରନ୍ତୁ curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#endif /* if not disabled */
