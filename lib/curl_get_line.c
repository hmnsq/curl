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

#ಆದರೆ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#ಅನ್ತರ್ಭಾವ "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_ಸಾಲು(ರಚನೆ dynbuf *buf, FILE *ಒಳಹರಿವು, bool *eof)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ ಬಫರ್[128];
  curlx_dyn_reset(buf);
  ಆಗ(1) {
    size_t rlen;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b = fgets(ಬಫರ್, ಗಾತ್ರ(ಬಫರ್), ಒಳಹರಿವು);
    ಆದರೆ(!b && ferror(ಒಳಹರಿವು))
      ಹಿಂತಿರುಗಿ CURLE_READ_ERROR;

    *eof = feof(ಒಳಹರಿವು);

    rlen = b ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(b) : 0;
    ಆದರೆ(rlen) {
      ಫಲಿತಾಂಶ = curlx_dyn_addn(buf, b, rlen);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        /* too long line or out of memory */
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    ಆದರೆ(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      ಹಿಂತಿರುಗಿ CURLE_OK; /* all good */
    ಆದರೆ(*eof)
      /* append a newline */
      ಹಿಂತಿರುಗಿ curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#ಆದರೆ_ಅಂತ್ಯ /* if not disabled */
