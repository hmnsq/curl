#ifndef HEADER_CURL_FORMDATA_H
#define HEADER_CURL_FORMDATA_H
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
#অন্তর্ভাব "curl_setup.h"

#ifndef CURL_DISABLE_FORM_API

#অন্তর্ভাব "bufref.h"

/* used by FormAdd for temporary storage */
সংরচনা FormInfo {
  সংরচনা bufref name;
  সংরচনা bufref মান;
  সংরচনা bufref contenttype;
  সংরচনা bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  বর্ণ *buffer;      /* pointer to existing buffer used for file upload */
  বর্ণ *userp;        /* pointer for the read callback */
  সংরচনা FormInfo *more;
  সংরচনা curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  unsigned বর্ণ flags;
};

CURLcode Curl_getformdata(CURL *data,
                          curl_mimepart *finalform,
                          সংরচনা curl_httppost *post,
                          curl_read_callback fread_func);
#endif /* CURL_DISABLE_FORM_API */

#endif /* HEADER_CURL_FORMDATA_H */
