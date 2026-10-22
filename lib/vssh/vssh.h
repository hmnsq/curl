#ifndef HEADER_CURL_VSSH_VSSH_H
#define HEADER_CURL_VSSH_VSSH_H
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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_SSH

#অন্তর্ভাব "urldata.h"

কাঠামো ssh_conn;

CURLcode Curl_getworkingpath(কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক অক্ষর *homedir,
                             অক্ষর **path);

CURLcode Curl_get_pathname(ধ্রুবক অক্ষর **cpp, অক্ষর **path, ধ্রুবক অক্ষর *homedir);

CURLcode Curl_ssh_range(কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক অক্ষর *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep);

CURLcode Curl_ssh_pollset(কাঠামো Curl_easy *তথ্য, কাঠামো easy_pollset *ps);
CURLcode Curl_ssh_setup_pkey(কাঠামো Curl_easy *তথ্য, কাঠামো ssh_conn *sshc);

#endif /* USE_SSH */
#endif /* HEADER_CURL_VSSH_VSSH_H */
