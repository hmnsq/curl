#अगर_अघोषित HEADER_CURL_FORMDATA_H
#घोषणा HEADER_CURL_FORMDATA_H
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

#अगर_अघोषित CURL_DISABLE_FORM_API

#समावेश "bufref.h"

/* used by FormAdd for temporary storage */
काष्ठा FormInfo {
  काष्ठा bufref name;
  काष्ठा bufref मूल्य;
  काष्ठा bufref contenttype;
  काष्ठा bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  अक्षर *बफर;      /* pointer to existing buffer used for file upload */
  अक्षर *userp;        /* pointer for the read callback */
  काष्ठा FormInfo *more;
  काष्ठा curl_slist *contentheader;
  curl_off_t contentslength;
  माप_प्रकार namelength;
  माप_प्रकार bufferlength;
  अचिन्हित अक्षर flags;
};

CURLcode Curl_getformdata(CURL *तथ्य,
                          curl_mimepart *finalform,
                          काष्ठा curl_httppost *post,
                          curl_read_callback fread_func);
#पूर्ण_अगर /* CURL_DISABLE_FORM_API */

#पूर्ण_अगर /* HEADER_CURL_FORMDATA_H */
