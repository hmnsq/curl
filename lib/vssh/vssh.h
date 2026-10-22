#اگر_تعریف_نشده سرآیند_CURL_VSSH_VSSH_H
#تعریف_کن سرآیند_CURL_VSSH_VSSH_H
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

#اگر_تعریف_شده USE_SSH

#ضمن "urldata.h"

بنية ssh_conn;

CURLcode Curl_getworkingpath(بنية Curl_easy *داده,
                             ثابت حرف *homedir,
                             حرف **مسیر);

CURLcode Curl_get_pathname(ثابت حرف **cpp, حرف **مسیر, ثابت حرف *homedir);

CURLcode Curl_ssh_range(بنية Curl_easy *داده,
                        ثابت حرف *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep);

CURLcode Curl_ssh_pollset(بنية Curl_easy *داده, بنية easy_pollset *ps);
CURLcode Curl_ssh_راهاندازی_pkey(بنية Curl_easy *داده, بنية ssh_conn *sshc);

#پایان_اگر /* USE_SSH */
#پایان_اگر /* HEADER_CURL_VSSH_VSSH_H */
