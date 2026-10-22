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
#അന്തര്ഭാവ "curl_setup.h"

#ifndef CURL_DISABLE_FORM_API

#അന്തര്ഭാവ "bufref.h"

/* used by FormAdd for temporary storage */
ഘടന FormInfo {
  ഘടന bufref name;
  ഘടന bufref മൂല്യം;
  ഘടന bufref contenttype;
  ഘടന bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  അക്ഷരം *buffer;      /* pointer to existing buffer used for file upload */
  അക്ഷരം *userp;        /* pointer for the read callback */
  ഘടന FormInfo *more;
  ഘടന curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  unsigned അക്ഷരം flags;
};

CURLcode Curl_getformdata(CURL *data,
                          curl_mimepart *finalform,
                          ഘടന curl_httppost *post,
                          curl_read_callback fread_func);
#endif /* CURL_DISABLE_FORM_API */

#endif /* HEADER_CURL_FORMDATA_H */
