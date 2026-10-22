#اگر_غیر_تعریف_شدہ سرخی_CURL_FORMDATA_H
#تعریف_کرو سرخی_CURL_FORMDATA_H
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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FORM_API

#شامل "bufref.h"

/* used by FormAdd for temporary storage */
ساخت FormInfo {
  ساخت bufref name;
  ساخت bufref قدر;
  ساخت bufref contenttype;
  ساخت bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  حرف *بفر;      /* pointer to existing buffer used for file upload */
  حرف *userp;        /* pointer for the read callback */
  ساخت FormInfo *مزید;
  ساخت curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  غیرعلامتی حرف flags;
};

CURLcode Curl_getformdata(CURL *ڈیٹا,
                          curl_mimepart *finalform,
                          ساخت curl_httppost *post,
                          curl_read_callback fread_func);
#اگر_ختم /* CURL_DISABLE_FORM_API */

#اگر_ختم /* HEADER_CURL_FORMDATA_H */
