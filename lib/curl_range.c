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
#समावेश "curl_setup.h"

#समावेश "curl_range.h"
#समावेश "curl_trc.h"
#समावेश "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#अगर !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(काष्ठा Curl_easy *तथ्य)
{
  अगर(तथ्य->state.use_range && तथ्य->state.range) {
    curl_off_t from, to;
    bool first_अंक = TRUE;
    स्थिर अक्षर *p = तथ्य->state.range;
    अगर(curlx_str_संख्या(&p, &from, CURL_OFF_T_MAX))
      first_अंक = FALSE;

    अगर(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      वापस CURLE_RANGE_ERROR;

    अगर(curlx_str_संख्या(&p, &to, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      तथ्य->state.resume_from = from;
      DEBUGF(infof(तथ्य, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    अन्यथा अगर(!first_अंक) {
      /* -Y */
      अगर(!to)
        /* "-0" is wrong */
        वापस CURLE_RANGE_ERROR;

      तथ्य->req.maxdownload = to;
      तथ्य->state.resume_from = -to;
      DEBUGF(infof(तथ्य, "RANGE the last %" FMT_OFF_T " bytes", to));
    }
    अन्यथा {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      अगर(from > to)
        वापस CURLE_RANGE_ERROR;

      totalsize = to - from;
      अगर(totalsize == CURL_OFF_T_MAX)
        वापस CURLE_RANGE_ERROR;

      तथ्य->req.maxdownload = totalsize + 1; /* include last byte */
      तथ्य->state.resume_from = from;
      DEBUGF(infof(तथ्य, "RANGE from %" FMT_OFF_T
                   " getting %" FMT_OFF_T " bytes",
                   from, तथ्य->req.maxdownload));
    }
    DEBUGF(infof(तथ्य, "range-download from %" FMT_OFF_T
                 " to %" FMT_OFF_T ", totally %" FMT_OFF_T " bytes",
                 from, to, तथ्य->req.maxdownload));
  }
  अन्यथा
    तथ्य->req.maxdownload = -1;
  वापस CURLE_OK;
}

#पूर्ण_अगर
