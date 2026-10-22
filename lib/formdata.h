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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifndef CURL_DISABLE_FORM_API

#ଅନ୍ତର୍ଭାଵ "bufref.h"

/* used by FormAdd for temporary storage */
ସଂରଚନା FormInfo {
  ସଂରଚନା bufref name;
  ସଂରଚନା bufref ମୂଲ୍ୟ;
  ସଂରଚନା bufref contenttype;
  ସଂରଚନା bufref showfilename; /* The filename to show. If not set, the actual
                                 filename will be used */
  ଅକ୍ଷର *buffer;      /* pointer to existing buffer used for file upload */
  ଅକ୍ଷର *userp;        /* pointer for the read callback */
  ସଂରଚନା FormInfo *more;
  ସଂରଚନା curl_slist *contentheader;
  curl_off_t contentslength;
  size_t namelength;
  size_t bufferlength;
  unsigned ଅକ୍ଷର flags;
};

CURLcode Curl_getformdata(CURL *data,
                          curl_mimepart *finalform,
                          ସଂରଚନା curl_httppost *post,
                          curl_read_callback fread_func);
#endif /* CURL_DISABLE_FORM_API */

#endif /* HEADER_CURL_FORMDATA_H */
