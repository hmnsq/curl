#اگر_تعریف_نشده سرآیند_CURL_FORMDATA_H
#تعریف_کن سرآیند_CURL_FORMDATA_H
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

#اگر_تعریف_نشده CURL_DISABLE_FORM_API

#ضمن "bufref.h"

/* used by FormAdd for temporary storage */
بنية FormInfo {
  بنية bufref name;
  بنية bufref مقدار;
  بنية bufref contenttype;
  بنية bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  حرف *میانگیر;      /* pointer to existing buffer used for file upload */
  حرف *userp;        /* pointer for the read callback */
  بنية FormInfo *بیشتر;
  بنية curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  بیعلامت حرف flags;
};

CURLcode Curl_getformdata(CURL *داده,
                          curl_mimepart *finalform,
                          بنية curl_httppost *post,
                          curl_read_callback fread_func);
#پایان_اگر /* CURL_DISABLE_FORM_API */

#پایان_اگر /* HEADER_CURL_FORMDATA_H */
