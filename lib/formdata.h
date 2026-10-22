#که_نه_تعریف_شوی سرلیک_CURL_FORMDATA_H
#تعریف_کړه سرلیک_CURL_FORMDATA_H
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
#ضمن "curl_setup.h"

#که_نه_تعریف_شوی CURL_DISABLE_FORM_API

#ضمن "bufref.h"

/* used by FormAdd for temporary storage */
بنية FormInfo {
  بنية bufref name;
  بنية bufref ارزښت;
  بنية bufref contenttype;
  بنية bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  حرف *بفر;      /* pointer to existing buffer used for file upload */
  حرف *userp;        /* pointer for the read callback */
  بنية FormInfo *نور;
  بنية curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  بینښه حرف flags;
};

CURLcode Curl_getformdata(CURL *معلومات,
                          curl_mimepart *finalform,
                          بنية curl_httppost *post,
                          curl_read_callback fread_func);
#د_که_پای /* CURL_DISABLE_FORM_API */

#د_که_پای /* HEADER_CURL_FORMDATA_H */
