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

#অন্তর্ভাব "curl_range.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#যদি !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(কাঠামো Curl_easy *তথ্য)
{
  যদি(তথ্য->state.use_range && তথ্য->state.range) {
    curl_off_t from, to;
    bool first_অঙ্ক = TRUE;
    ধ্রুবক অক্ষর *p = তথ্য->state.range;
    যদি(curlx_str_সংখ্যা(&p, &from, CURL_OFF_T_MAX))
      first_অঙ্ক = FALSE;

    যদি(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      ফেরত CURLE_RANGE_ERROR;

    যদি(curlx_str_সংখ্যা(&p, &to, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      তথ্য->state.resume_from = from;
      DEBUGF(infof(তথ্য, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    অন্যথা যদি(!first_অঙ্ক) {
      /* -Y */
      যদি(!to)
        /* "-0" is wrong */
        ফেরত CURLE_RANGE_ERROR;

      তথ্য->req.maxdownload = to;
      তথ্য->state.resume_from = -to;
      DEBUGF(infof(তথ্য, "RANGE the last %" FMT_OFF_T " bytes", to));
    }
    অন্যথা {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      যদি(from > to)
        ফেরত CURLE_RANGE_ERROR;

      totalsize = to - from;
      যদি(totalsize == CURL_OFF_T_MAX)
        ফেরত CURLE_RANGE_ERROR;

      তথ্য->req.maxdownload = totalsize + 1; /* include last byte */
      তথ্য->state.resume_from = from;
      DEBUGF(infof(তথ্য, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, তথ্য->req.maxdownload));
    }
    DEBUGF(infof(তথ্য, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, to, তথ্য->req.maxdownload));
  }
  অন্যথা
    তথ্য->req.maxdownload = -1;
  ফেরত CURLE_OK;
}

#endif
