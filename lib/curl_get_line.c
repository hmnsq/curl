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

#यदि !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#अन्तर्भाव "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_पङ्क्तिः(संरचना dynbuf *buf, FILE *निवेशः, bool *eof)
{
  CURLcode परिणाम;
  वर्ण बफर[128];
  curlx_dyn_reset(buf);
  यावत्(1) {
    size_t rlen;
    ध्रुव वर्ण *b = fgets(बफर, परिमाण(बफर), निवेशः);
    यदि(!b && ferror(निवेशः))
      प्रत्यागम CURLE_READ_ERROR;

    *eof = feof(निवेशः);

    rlen = b ? वाक्यदैर्घ्यम्(b) : 0;
    यदि(rlen) {
      परिणाम = curlx_dyn_addn(buf, b, rlen);
      यदि(परिणाम)
        /* too long line or out of memory */
        प्रत्यागम परिणाम;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    यदि(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      प्रत्यागम CURLE_OK; /* all good */
    यदि(*eof)
      /* append a newline */
      प्रत्यागम curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#यदि_अन्तः /* if not disabled */
