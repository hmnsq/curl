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

#ضمن "curl_range.h"
#ضمن "curl_trc.h"
#ضمن "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#إذا !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(بنية Curl_easy *داده)
{
  إذا(داده->state.use_range && داده->state.range) {
    curl_off_t from, مقصد;
    bool اولی_رقم = TRUE;
    ثابت حرف *p = داده->state.range;
    إذا(curlx_str_عدد(&p, &from, CURL_OFF_T_MAX))
      اولی_رقم = FALSE;

    إذا(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      أرجع CURLE_RANGE_ERROR;

    إذا(curlx_str_عدد(&p, &مقصد, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      داده->state.resume_from = from;
      DEBUGF(infof(داده, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    وإلا إذا(!اولی_رقم) {
      /* -Y */
      إذا(!مقصد)
        /* "-0" is wrong */
        أرجع CURLE_RANGE_ERROR;

      داده->req.maxdownload = مقصد;
      داده->state.resume_from = -مقصد;
      DEBUGF(infof(داده, "RANGE the last %" FMT_OFF_T " bytes", مقصد));
    }
    وإلا {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      إذا(from > مقصد)
        أرجع CURLE_RANGE_ERROR;

      totalsize = مقصد - from;
      إذا(totalsize == CURL_OFF_T_MAX)
        أرجع CURLE_RANGE_ERROR;

      داده->req.maxdownload = totalsize + 1; /* include last byte */
      داده->state.resume_from = from;
      DEBUGF(infof(داده, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, داده->req.maxdownload));
    }
    DEBUGF(infof(داده, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, مقصد, داده->req.maxdownload));
  }
  وإلا
    داده->req.maxdownload = -1;
  أرجع CURLE_OK;
}

#پایان_اگر
