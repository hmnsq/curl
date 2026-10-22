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

#अन्तर्भाव "curl_range.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#यदि !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(संरचना Curl_easy *दत्त)
{
  यदि(दत्त->state.use_range && दत्त->state.range) {
    curl_off_t from, गन्तव्यम्;
    bool first_अङ्कः = TRUE;
    ध्रुव वर्ण *p = दत्त->state.range;
    यदि(curlx_str_सङ्ख्या(&p, &from, CURL_OFF_T_MAX))
      first_अङ्कः = FALSE;

    यदि(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      प्रत्यागम CURLE_RANGE_ERROR;

    यदि(curlx_str_सङ्ख्या(&p, &गन्तव्यम्, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      दत्त->state.resume_from = from;
      DEBUGF(infof(दत्त, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    अन्यथा यदि(!first_अङ्कः) {
      /* -Y */
      यदि(!गन्तव्यम्)
        /* "-0" is wrong */
        प्रत्यागम CURLE_RANGE_ERROR;

      दत्त->req.maxdownload = गन्तव्यम्;
      दत्त->state.resume_from = -गन्तव्यम्;
      DEBUGF(infof(दत्त, "RANGE the last %" FMT_OFF_T " bytes", गन्तव्यम्));
    }
    अन्यथा {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      यदि(from > गन्तव्यम्)
        प्रत्यागम CURLE_RANGE_ERROR;

      totalsize = गन्तव्यम् - from;
      यदि(totalsize == CURL_OFF_T_MAX)
        प्रत्यागम CURLE_RANGE_ERROR;

      दत्त->req.maxdownload = totalsize + 1; /* include last byte */
      दत्त->state.resume_from = from;
      DEBUGF(infof(दत्त, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, दत्त->req.maxdownload));
    }
    DEBUGF(infof(दत्त, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, गन्तव्यम्, दत्त->req.maxdownload));
  }
  अन्यथा
    दत्त->req.maxdownload = -1;
  प्रत्यागम CURLE_OK;
}

#यदि_अन्तः
