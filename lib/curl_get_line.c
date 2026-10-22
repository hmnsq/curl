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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#ಅನ್ತರ್ಭಾವ "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_ಸಾಲು(ಸಂರಚನಾ dynbuf *buf, FILE *ಒಳಹರಿವು, bool *eof)
{
  CURLcode result;
  ವರ್ಣ buffer[128];
  curlx_dyn_reset(buf);
  ಯಾವತ್(1) {
    size_t rlen;
    ಧ್ರುವ ವರ್ಣ *b = fgets(buffer, ಪರಿಮಾಣ(buffer), ಒಳಹರಿವು);
    ಯದಿ(!b && ferror(ಒಳಹರಿವು))
      ಪ್ರತ್ಯಾಗಮ CURLE_READ_ERROR;

    *eof = feof(ಒಳಹರಿವು);

    rlen = b ? strlen(b) : 0;
    ಯದಿ(rlen) {
      result = curlx_dyn_addn(buf, b, rlen);
      ಯದಿ(result)
        /* too long line or out of memory */
        ಪ್ರತ್ಯಾಗಮ result;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    ಯದಿ(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      ಪ್ರತ್ಯಾಗಮ CURLE_OK; /* all good */
    ಯದಿ(*eof)
      /* append a newline */
      ಪ್ರತ್ಯಾಗಮ curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#endif /* if not disabled */
