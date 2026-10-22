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
CURLcode Curl_get_पङ्क्ति(संरचना dynbuf *buf, खाता *आगत, bool *eof)
{
  CURLcode result;
  वर्ण buffer[128];
  curlx_dyn_reset(buf);
  जबसम्म(1) {
    माप_प्रकार rlen;
    अचर वर्ण *b = ख_माला_लो(buffer, परिमाण(buffer), आगत);
    यदि(!b && ख_त्रुटि(आगत))
      फर्काऊ CURLE_READ_ERROR;

    *eof = ख_पूर्ण(आगत);

    rlen = b ? म_माप(b) : 0;
    यदि(rlen) {
      result = curlx_dyn_addn(buf, b, rlen);
      यदि(result)
        /* too long line or out of memory */
        फर्काऊ result;
    }
    /* now check the full line */
    rlen = curlx_dyn_len(buf);
    b = curlx_dyn_ptr(buf);
    यदि(rlen && (b[rlen - 1] == '\n'))
      /* LF at end of the line */
      फर्काऊ CURLE_OK; /* all good */
    यदि(*eof)
      /* append a newline */
      फर्काऊ curlx_dyn_addn(buf, "\n", 1);
    /* otherwise get next line to append */
  }
  /* UNREACHABLE */
}

#endif /* if not disabled */
