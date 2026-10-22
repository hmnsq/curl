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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "curl_range.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#ଯଦି !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(data->state.use_range && data->state.range) {
    curl_off_t from, to;
    bool first_ଅଙ୍କ = TRUE;
    ଅଚଳ ଅକ୍ଷର *p = data->state.range;
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &from, CURL_OFF_T_MAX))
      first_ଅଙ୍କ = FALSE;

    ଯଦି(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      ଫେରନ୍ତୁ CURLE_RANGE_ERROR;

    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &to, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      data->state.resume_from = from;
      DEBUGF(infof(data, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    ଅନ୍ୟଥା ଯଦି(!first_ଅଙ୍କ) {
      /* -Y */
      ଯଦି(!to)
        /* "-0" is wrong */
        ଫେରନ୍ତୁ CURLE_RANGE_ERROR;

      data->req.maxdownload = to;
      data->state.resume_from = -to;
      DEBUGF(infof(data, "RANGE the last %" FMT_OFF_T " bytes", to));
    }
    ଅନ୍ୟଥା {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      ଯଦି(from > to)
        ଫେରନ୍ତୁ CURLE_RANGE_ERROR;

      totalsize = to - from;
      ଯଦି(totalsize == CURL_OFF_T_MAX)
        ଫେରନ୍ତୁ CURLE_RANGE_ERROR;

      data->req.maxdownload = totalsize + 1; /* include last byte */
      data->state.resume_from = from;
      DEBUGF(infof(data, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, data->req.maxdownload));
    }
    DEBUGF(infof(data, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, to, data->req.maxdownload));
  }
  ଅନ୍ୟଥା
    data->req.maxdownload = -1;
  ଫେରନ୍ତୁ CURLE_OK;
}

#endif
