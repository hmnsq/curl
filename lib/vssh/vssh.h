#اگر_غیر_تعریف_شدہ سرخی_CURL_VSSH_VSSH_H
#تعریف_کرو سرخی_CURL_VSSH_VSSH_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_SSH

#شامل "urldata.h"

ساخت ssh_conn;

CURLcode Curl_getworkingpath(ساخت Curl_easy *ڈیٹا,
                             ثابت حرف *homedir,
                             حرف **راستہ);

CURLcode Curl_get_pathname(ثابت حرف **cpp, حرف **راستہ, ثابت حرف *homedir);

CURLcode Curl_ssh_range(ساخت Curl_easy *ڈیٹا,
                        ثابت حرف *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep);

CURLcode Curl_ssh_pollset(ساخت Curl_easy *ڈیٹا, ساخت easy_pollset *ps);
CURLcode Curl_ssh_تیاری_pkey(ساخت Curl_easy *ڈیٹا, ساخت ssh_conn *sshc);

#اگر_ختم /* USE_SSH */
#اگر_ختم /* HEADER_CURL_VSSH_VSSH_H */
