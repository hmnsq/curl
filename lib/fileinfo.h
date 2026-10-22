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
#ضمن "curl_setup.h"

#ضمن "llist.h"
#ضمن "curlx/dynbuf.h"

بنية fileinfo {
  بنية curl_fileinfo info;
  بنية Curl_llist_گرہ list;
  بنية dynbuf buf;
};

بنية fileinfo *Curl_fileinfo_alloc(فراغ);
فراغ Curl_fileinfo_cleanup(بنية fileinfo *finfo);

#endif /* HEADER_CURL_FILEINFO_H */
