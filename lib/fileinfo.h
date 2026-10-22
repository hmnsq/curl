#ifndef HEADER_CURL_FILEINFO_H
#define HEADER_CURL_FILEINFO_H
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

#ଅନ୍ତର୍ଭାଵ "llist.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"

ସଂରଚନା fileinfo {
  ସଂରଚନା curl_fileinfo info;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି list;
  ସଂରଚନା dynbuf buf;
};

ସଂରଚନା fileinfo *Curl_fileinfo_alloc(ଶୂନ୍ୟ);
ଶୂନ୍ୟ Curl_fileinfo_cleanup(ସଂରଚନା fileinfo *finfo);

#endif /* HEADER_CURL_FILEINFO_H */
