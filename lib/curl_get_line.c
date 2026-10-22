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
#include "curl_setup.h"

#ᯀᯮᯝ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#include "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_line(sa_nracanaa dynbuf *buf, FILE *input, bool *eof)
{
  CURLcode result;
  var_na buffer[128];
  curlx_dyn_reset(buf);
  ᯀᯬᯝ(1) {
    size_t rlen;
    const var_na *b = fgets(buffer, parimaa_na(buffer), input);
    ᯀᯮᯝ(!b && ferror(input))
      ᯀᯮᯞ CURLE_READ_ERROR;

    *eof = feof(input);

    rlen = b ? strlen(b) : 0;
    ᯀᯮᯝ(rlen) {
      result = curlx_dyn_addn(buf, b, rlen);
      ᯀᯮᯝ(result)
        /* too long line or out of memory */
        ᯀᯮᯞ result;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    ᯀᯮᯝ(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      ᯀᯮᯞ CURLE_OK; /* all good */
    ᯀᯮᯝ(*eof)
      /* append a newline */
      ᯀᯮᯞ curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#endif /* if not disabled */
