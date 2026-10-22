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
CURLcode Curl_range(بنية Curl_easy *انکڑے)
{
  إذا(انکڑے->state.use_range && انکڑے->state.range) {
    curl_off_t from, منزل;
    bool پہلا_عدد = TRUE;
    ثابت حرف *p = انکڑے->state.range;
    إذا(curlx_str_نمبر(&p, &from, CURL_OFF_T_MAX))
      پہلا_عدد = FALSE;

    إذا(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      أرجع CURLE_RANGE_ERROR;

    إذا(curlx_str_نمبر(&p, &منزل, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      انکڑے->state.resume_from = from;
      DEBUGF(infof(انکڑے, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    وإلا إذا(!پہلا_عدد) {
      /* -Y */
      إذا(!منزل)
        /* "-0" is wrong */
        أرجع CURLE_RANGE_ERROR;

      انکڑے->req.maxdownload = منزل;
      انکڑے->state.resume_from = -منزل;
      DEBUGF(infof(انکڑے, "RANGE the last %" FMT_OFF_T " bytes", منزل));
    }
    وإلا {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      إذا(from > منزل)
        أرجع CURLE_RANGE_ERROR;

      totalsize = منزل - from;
      إذا(totalsize == CURL_OFF_T_MAX)
        أرجع CURLE_RANGE_ERROR;

      انکڑے->req.maxdownload = totalsize + 1; /* include last byte */
      انکڑے->state.resume_from = from;
      DEBUGF(infof(انکڑے, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, انکڑے->req.maxdownload));
    }
    DEBUGF(infof(انکڑے, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, منزل, انکڑے->req.maxdownload));
  }
  وإلا
    انکڑے->req.maxdownload = -1;
  أرجع CURLE_OK;
}

#جے_انت
