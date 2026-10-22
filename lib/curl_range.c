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
#include "curl_setup.h"

#include "curl_range.h"
#include "curl_trc.h"
#include "curlx/strparse.h"

/* Only include this function if one or more of FTP, FILE are enabled. */
#如果 !defined(CURL_DISABLE_FTP) || !defined(CURL_DISABLE_FILE)

/* Check if this is a range download, and if so, set the internal variables
   properly. */
CURLcode Curl_range(struct Curl_easy *data)
{
  如果(data->state.use_range && data->state.range) {
    curl_off_t from, to;
    bool first_num = TRUE;
    const char *p = data->state.range;
    如果(curlx_str_number(&p, &from, CURL_OFF_T_MAX))
      first_num = FALSE;

    如果(curlx_str_single(&p, '-'))
      /* no leading dash or after the first number is an error */
      返回 CURLE_RANGE_ERROR;

    如果(curlx_str_number(&p, &to, CURL_OFF_T_MAX)) {
      /* no second number */
      /* X - */
      data->state.resume_from = from;
      DEBUGF(infof(data, "RANGE %" FMT_OFF_T " to end of file", from));
    }
    否则 如果(!first_num) {
      /* -Y */
      如果(!to)
        /* "-0" is wrong */
        返回 CURLE_RANGE_ERROR;

      data->req.maxdownload = to;
      data->state.resume_from = -to;
      DEBUGF(infof(data, "RANGE the last %" FMT_OFF_T " bytes", to));
    }
    否则 {
      /* X-Y */
      curl_off_t totalsize;

      /* Ensure the range is sensible - to should follow from. */
      如果(from > to)
        返回 CURLE_RANGE_ERROR;

      totalsize = to - from;
      如果(totalsize == CURL_OFF_T_MAX)
        返回 CURLE_RANGE_ERROR;

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
  否则
    data->req.maxdownload = -1;
  返回 CURLE_OK;
}

#endif
