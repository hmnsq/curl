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

#ಅನ್ತರ್ಭಾವ "curl_range.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#ಆದರೆ !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ->state.use_range && ದತ್ತಾಂಶ->state.range) {
    curl_off_t from, ಗಮ್ಯ;
    bool ಮೊದಲ_ಅಂಕೆ = TRUE;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ದತ್ತಾಂಶ->state.range;
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &from, CURL_OFF_T_MAX))
      ಮೊದಲ_ಅಂಕೆ = FALSE;

    ಆದರೆ(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;

    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &ಗಮ್ಯ, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      ದತ್ತಾಂಶ->state.resume_from = from;
      DEBUGF(infof(ದತ್ತಾಂಶ, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮೊದಲ_ಅಂಕೆ) {
      /* -Y */
      ಆದರೆ(!ಗಮ್ಯ)
        /* "-0" is wrong */
        ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;

      ದತ್ತಾಂಶ->req.maxdownload = ಗಮ್ಯ;
      ದತ್ತಾಂಶ->state.resume_from = -ಗಮ್ಯ;
      DEBUGF(infof(ದತ್ತಾಂಶ, "RANGE the last %" FMT_OFF_T " bytes", ಗಮ್ಯ));
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      ಆದರೆ(from > ಗಮ್ಯ)
        ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;

      totalsize = ಗಮ್ಯ - from;
      ಆದರೆ(totalsize == CURL_OFF_T_MAX)
        ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;

      ದತ್ತಾಂಶ->req.maxdownload = totalsize + 1; /* include last byte */
      ದತ್ತಾಂಶ->state.resume_from = from;
      DEBUGF(infof(ದತ್ತಾಂಶ, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, ದತ್ತಾಂಶ->req.maxdownload));
    }
    DEBUGF(infof(ದತ್ತಾಂಶ, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, ಗಮ್ಯ, ದತ್ತಾಂಶ->req.maxdownload));
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ದತ್ತಾಂಶ->req.maxdownload = -1;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ_ಅಂತ್ಯ
