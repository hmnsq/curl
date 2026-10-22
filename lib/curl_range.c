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

#అన్తర్భావ "curl_range.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#ఒకవేళ !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(దత్తాంశం->state.use_range && దత్తాంశం->state.range) {
    curl_off_t from, గమ్యం;
    bool మొదటి_అంకె = TRUE;
    స్థిరరాశి అక్షరం *p = దత్తాంశం->state.range;
    ఒకవేళ(curlx_str_సంఖ్య(&p, &from, CURL_OFF_T_MAX))
      మొదటి_అంకె = FALSE;

    ఒకవేళ(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      తిరిగి CURLE_RANGE_ERROR;

    ఒకవేళ(curlx_str_సంఖ్య(&p, &గమ్యం, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      దత్తాంశం->state.resume_from = from;
      DEBUGF(infof(దత్తాంశం, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    లేకపోతే ఒకవేళ(!మొదటి_అంకె) {
      /* -Y */
      ఒకవేళ(!గమ్యం)
        /* "-0" is wrong */
        తిరిగి CURLE_RANGE_ERROR;

      దత్తాంశం->req.maxdownload = గమ్యం;
      దత్తాంశం->state.resume_from = -గమ్యం;
      DEBUGF(infof(దత్తాంశం, "RANGE the last %" FMT_OFF_T " bytes", గమ్యం));
    }
    లేకపోతే {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      ఒకవేళ(from > గమ్యం)
        తిరిగి CURLE_RANGE_ERROR;

      totalsize = గమ్యం - from;
      ఒకవేళ(totalsize == CURL_OFF_T_MAX)
        తిరిగి CURLE_RANGE_ERROR;

      దత్తాంశం->req.maxdownload = totalsize + 1; /* include last byte */
      దత్తాంశం->state.resume_from = from;
      DEBUGF(infof(దత్తాంశం, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, దత్తాంశం->req.maxdownload));
    }
    DEBUGF(infof(దత్తాంశం, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, గమ్యం, దత్తాంశం->req.maxdownload));
  }
  లేకపోతే
    దత్తాంశం->req.maxdownload = -1;
  తిరిగి CURLE_OK;
}

#అయితే_ముగింపు
