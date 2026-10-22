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
#அந்தர்பாவ "curl_setup.h"

#ifndef CURL_DISABLE_FORM_API

#அந்தர்பாவ "bufref.h"

/* used by FormAdd for temporary storage */
கட்டமைப்பு FormInfo {
  கட்டமைப்பு bufref name;
  கட்டமைப்பு bufref மதிப்பு;
  கட்டமைப்பு bufref contenttype;
  கட்டமைப்பு bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  எழுத்து *buffer;      /* pointer to existing buffer used for file upload */
  எழுத்து *userp;        /* pointer for the read callback */
  கட்டமைப்பு FormInfo *more;
  கட்டமைப்பு curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  unsigned எழுத்து flags;
};

CURLcode Curl_getformdata(CURL *data,
                          curl_mimepart *finalform,
                          கட்டமைப்பு curl_httppost *post,
                          curl_read_callback fread_func);
#endif /* CURL_DISABLE_FORM_API */

#endif /* HEADER_CURL_FORMDATA_H */
