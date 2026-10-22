#अगर_अघोषित HEADER_CURL_FILEINFO_H
#घोषणा HEADER_CURL_FILEINFO_H
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

#समावेश "llist.h"
#समावेश "curlx/dynbuf.h"

काष्ठा fileinfo {
  काष्ठा curl_fileinfo info;
  काष्ठा Curl_llist_गाँठ list;
  काष्ठा dynbuf buf;
};

काष्ठा fileinfo *Curl_fileinfo_alloc(व्योम);
व्योम Curl_fileinfo_cleanup(काष्ठा fileinfo *finfo);

#पूर्ण_अगर /* HEADER_CURL_FILEINFO_H */
