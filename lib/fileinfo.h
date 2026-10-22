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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "llist.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"

ഘടന fileinfo {
  ഘടന curl_fileinfo info;
  ഘടന Curl_llist_കണ്ണി list;
  ഘടന dynbuf buf;
};

ഘടന fileinfo *Curl_fileinfo_alloc(ശൂന്യം);
ശൂന്യം Curl_fileinfo_cleanup(ഘടന fileinfo *finfo);

#endif /* HEADER_CURL_FILEINFO_H */
