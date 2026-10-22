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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "llist.h"
#அந்தர்பாவ "curlx/dynbuf.h"

கட்டமைப்பு fileinfo {
  கட்டமைப்பு curl_fileinfo info;
  கட்டமைப்பு Curl_llist_முனை list;
  கட்டமைப்பு dynbuf buf;
};

கட்டமைப்பு fileinfo *Curl_fileinfo_alloc(வெறுமை);
வெறுமை Curl_fileinfo_cleanup(கட்டமைப்பு fileinfo *finfo);

#endif /* HEADER_CURL_FILEINFO_H */
