#अगर_अघोषित HEADER_CURL_AMIGAOS_H
#घोषणा HEADER_CURL_AMIGAOS_H
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

#अगर defined(__AMIGA__) && defined(HAVE_PROTO_BSDSOCKET_H) && \
  (!defined(USE_AMISSL) || defined(__amigaos4__))

CURLcode Curl_amiga_init(व्योम);
व्योम Curl_amiga_cleanup(व्योम);

#अन्यथा

#घोषणा Curl_amiga_init()    CURLE_OK
#घोषणा Curl_amiga_cleanup() Curl_nop_stmt

#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_AMIGAOS_H */
