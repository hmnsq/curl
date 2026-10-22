#यदि_अपरिभाषितम् शीर्षकम्_CURL_MD4_H
#परिभाषय शीर्षकम्_CURL_MD4_H
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

#यदि_परिभाषितम् USE_CURL_NTLM_CORE

#परिभाषय MD4_DIGEST_दीर्घता 16

CURLcode Curl_md4it(अचिह्नित वर्ण *output, ध्रुव अचिह्नित वर्ण *निवेशः,
                    ध्रुव size_t len);

#यदि_अन्तः /* USE_CURL_NTLM_CORE */

#यदि_अन्तः /* HEADER_CURL_MD4_H */
