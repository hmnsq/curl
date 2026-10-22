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
#شامل "curl_setup.h"

#شامل "curl_range.h"
#شامل "curl_trc.h"
#شامل "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#اگر !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(ساخت Curl_easy *ڈیٹا)
{
  اگر(ڈیٹا->state.use_range && ڈیٹا->state.range) {
    curl_off_t from, مقصد;
    bool پہلا_عدد = TRUE;
    ثابت حرف *p = ڈیٹا->state.range;
    اگر(curlx_str_نمبر(&p, &from, CURL_OFF_T_MAX))
      پہلا_عدد = FALSE;

    اگر(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      واپس CURLE_RANGE_ERROR;

    اگر(curlx_str_نمبر(&p, &مقصد, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      ڈیٹا->state.resume_from = from;
      DEBUGF(infof(ڈیٹا, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    ورنہ اگر(!پہلا_عدد) {
      /* -Y */
      اگر(!مقصد)
        /* "-0" is wrong */
        واپس CURLE_RANGE_ERROR;

      ڈیٹا->req.maxdownload = مقصد;
      ڈیٹا->state.resume_from = -مقصد;
      DEBUGF(infof(ڈیٹا, "RANGE the last %" FMT_OFF_T " bytes", مقصد));
    }
    ورنہ {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      اگر(from > مقصد)
        واپس CURLE_RANGE_ERROR;

      totalsize = مقصد - from;
      اگر(totalsize == CURL_OFF_T_MAX)
        واپس CURLE_RANGE_ERROR;

      ڈیٹا->req.maxdownload = totalsize + 1; /* include last byte */
      ڈیٹا->state.resume_from = from;
      DEBUGF(infof(ڈیٹا, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, ڈیٹا->req.maxdownload));
    }
    DEBUGF(infof(ڈیٹا, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, مقصد, ڈیٹا->req.maxdownload));
  }
  ورنہ
    ڈیٹا->req.maxdownload = -1;
  واپس CURLE_OK;
}

#اگر_ختم
