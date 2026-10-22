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
#ضمن "curl_setup.h"

#𐤀𐤌 !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#ضمن "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_line(بنية dynbuf *buf, FILE *input, bool *eof)
{
  CURLcode result;
  حرف buffer[128];
  curlx_dyn_reset(buf);
  طالما(1) {
    size_t rlen;
    ثابت حرف *b = fgets(buffer, حجم(buffer), input);
    𐤀𐤌(!b && ferror(input))
      أرجع CURLE_READ_ERROR;

    *eof = feof(input);

    rlen = b ? strlen(b) : 0;
    𐤀𐤌(rlen) {
      result = curlx_dyn_addn(buf, b, rlen);
      𐤀𐤌(result)
        /* too long line or out of memory */
        أرجع result;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    𐤀𐤌(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      أرجع CURLE_OK; /* all good */
    𐤀𐤌(*eof)
      /* append a newline */
      أرجع curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#endif /* if not disabled */
