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
#अन्तर्भाव "curl_setup.h"

#जइ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#अन्तर्भाव "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_पङ्क्तिः(संरचना dynbuf *buf, FILE *निवेशः, bool *eof)
{
  CURLcode result;
  वर्ण buffer[128];
  curlx_dyn_reset(buf);
  जाव(1) {
    size_t rlen;
    ध्रुव वर्ण *b = fgets(buffer, परिमाण(buffer), निवेशः);
    जइ(!b && ferror(निवेशः))
      प्रत्यागम CURLE_READ_ERROR;

    *eof = feof(निवेशः);

    rlen = b ? strlen(b) : 0;
    जइ(rlen) {
      result = curlx_dyn_addn(buf, b, rlen);
      जइ(result)
        /* too long line or out of memory */
        प्रत्यागम result;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    जइ(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      प्रत्यागम CURLE_OK; /* all good */
    जइ(*eof)
      /* append a newline */
      प्रत्यागम curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#endif /* if not disabled */
