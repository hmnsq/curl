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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#অন্তর্ভাব "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_সারি(কাঠামো dynbuf *buf, FILE *প্রবেশ, bool *eof)
{
  CURLcode ফলাফল;
  অক্ষর বাফার[128];
  curlx_dyn_reset(buf);
  যখন(1) {
    size_t rlen;
    ধ্রুবক অক্ষর *b = fgets(বাফার, আকার(বাফার), প্রবেশ);
    যদি(!b && ferror(প্রবেশ))
      ফেরত CURLE_READ_ERROR;

    *eof = feof(প্রবেশ);

    rlen = b ? strlen(b) : 0;
    যদি(rlen) {
      ফলাফল = curlx_dyn_addn(buf, b, rlen);
      যদি(ফলাফল)
        /* too long line or out of memory */
        ফেরত ফলাফল;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    যদি(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      ফেরত CURLE_OK; /* all good */
    যদি(*eof)
      /* append a newline */
      ফেরত curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#endif /* if not disabled */
