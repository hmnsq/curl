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
#अन्तर्भाव "curl_setup.h"

#ifndef CURL_DISABLE_FORM_API

#अन्तर्भाव "bufref.h"

/* used by FormAdd for temporary storage */
संरचना FormInfo {
  संरचना bufref name;
  संरचना bufref मूल्य;
  संरचना bufref contenttype;
  संरचना bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  वर्ण *buffer;      /* pointer to existing buffer used for file upload */
  वर्ण *userp;        /* pointer for the read callback */
  संरचना FormInfo *more;
  संरचना curl_slist *contentheader;
  curl_off_t contentslength;
  माप_प्रकार namelength;
  माप_प्रकार bufferlength;
  अचिन्हित वर्ण flags;
};

CURLcode Curl_getformdata(CURL *data,
                          curl_mimepart *finalform,
                          संरचना curl_httppost *post,
                          curl_read_callback fread_func);
#endif /* CURL_DISABLE_FORM_API */

#endif /* HEADER_CURL_FORMDATA_H */
