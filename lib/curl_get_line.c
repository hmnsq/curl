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
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ !defined(CURL_DISABLE_COOKIES) || !defined(CURL_DISABLE_ALTSVC) || \
  !defined(CURL_DISABLE_HSTS) || !defined(CURL_DISABLE_NETRC)

#అన్తర్భావ "curl_get_line.h"

/*
 * Curl_get_line() returns only complete whole lines that end with newline.
 * When 'eof' is set TRUE, the last line has been read.
 */
CURLcode Curl_get_పంక్తి(నిర్మాణం dynbuf *buf, FILE *ఆదానం, bool *eof)
{
  CURLcode ఫలితం;
  అక్షరం బఫరు[128];
  curlx_dyn_reset(buf);
  అయితే(1) {
    size_t rlen;
    స్థిరరాశి అక్షరం *b = fgets(బఫరు, పరిమాణం(బఫరు), ఆదానం);
    ఒకవేళ(!b && ferror(ఆదానం))
      తిరిగి CURLE_READ_ERROR;

    *eof = feof(ఆదానం);

    rlen = b ? అక్షరశ్రేణి_పొడవు(b) : 0;
    ఒకవేళ(rlen) {
      ఫలితం = curlx_dyn_addn(buf, b, rlen);
      ఒకవేళ(ఫలితం)
        /* too long line or out of memory */
        తిరిగి ఫలితం;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    ఒకవేళ(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      తిరిగి CURLE_OK; /* all good */
    ఒకవేళ(*eof)
      /* append a newline */
      తిరిగి curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#అయితే_ముగింపు /* if not disabled */
