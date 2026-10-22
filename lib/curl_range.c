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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_range.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#ਜੇ !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ਅੰਕੜੇ->state.use_range && ਅੰਕੜੇ->state.range) {
    curl_off_t from, ਮੰਜ਼ਿਲ;
    bool ਪਹਿਲਾ_ਅੰਕ = TRUE;
    ਅਚਲ ਅੱਖਰ *p = ਅੰਕੜੇ->state.range;
    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &from, CURL_OFF_T_MAX))
      ਪਹਿਲਾ_ਅੰਕ = FALSE;

    ਜੇ(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      ਵਾਪਸ CURLE_RANGE_ERROR;

    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &ਮੰਜ਼ਿਲ, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      ਅੰਕੜੇ->state.resume_from = from;
      DEBUGF(infof(ਅੰਕੜੇ, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    ਨਹੀਂਤਾਂ ਜੇ(!ਪਹਿਲਾ_ਅੰਕ) {
      /* -Y */
      ਜੇ(!ਮੰਜ਼ਿਲ)
        /* "-0" is wrong */
        ਵਾਪਸ CURLE_RANGE_ERROR;

      ਅੰਕੜੇ->req.maxdownload = ਮੰਜ਼ਿਲ;
      ਅੰਕੜੇ->state.resume_from = -ਮੰਜ਼ਿਲ;
      DEBUGF(infof(ਅੰਕੜੇ, "RANGE the last %" FMT_OFF_T " bytes", ਮੰਜ਼ਿਲ));
    }
    ਨਹੀਂਤਾਂ {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      ਜੇ(from > ਮੰਜ਼ਿਲ)
        ਵਾਪਸ CURLE_RANGE_ERROR;

      totalsize = ਮੰਜ਼ਿਲ - from;
      ਜੇ(totalsize == CURL_OFF_T_MAX)
        ਵਾਪਸ CURLE_RANGE_ERROR;

      ਅੰਕੜੇ->req.maxdownload = totalsize + 1; /* include last byte */
      ਅੰਕੜੇ->state.resume_from = from;
      DEBUGF(infof(ਅੰਕੜੇ, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, ਅੰਕੜੇ->req.maxdownload));
    }
    DEBUGF(infof(ਅੰਕੜੇ, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, ਮੰਜ਼ਿਲ, ਅੰਕੜੇ->req.maxdownload));
  }
  ਨਹੀਂਤਾਂ
    ਅੰਕੜੇ->req.maxdownload = -1;
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਅੰਤ
