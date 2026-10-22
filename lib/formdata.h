#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_FORMDATA_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_FORMDATA_H
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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FORM_API

#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"

/* used by FormAdd for temporary storage */
ਬਣਤਰ FormInfo {
  ਬਣਤਰ bufref name;
  ਬਣਤਰ bufref ਮੁੱਲ;
  ਬਣਤਰ bufref contenttype;
  ਬਣਤਰ bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  ਅੱਖਰ *ਬਫਰ;      /* pointer to existing buffer used for file upload */
  ਅੱਖਰ *userp;        /* pointer for the read callback */
  ਬਣਤਰ FormInfo *ਹੋਰ;
  ਬਣਤਰ curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ flags;
};

CURLcode Curl_getformdata(CURL *ਅੰਕੜੇ,
                          curl_mimepart *finalform,
                          ਬਣਤਰ curl_httppost *post,
                          curl_read_callback fread_func);
#ਜੇ_ਅੰਤ /* CURL_DISABLE_FORM_API */

#ਜੇ_ਅੰਤ /* HEADER_CURL_FORMDATA_H */
