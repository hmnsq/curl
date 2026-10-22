#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VSSH_VSSH_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VSSH_VSSH_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

ਬਣਤਰ ssh_conn;

CURLcode Curl_getworkingpath(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਅੱਖਰ *homedir,
                             ਅੱਖਰ **ਰਸਤਾ);

CURLcode Curl_get_pathname(ਅਚਲ ਅੱਖਰ **cpp, ਅੱਖਰ **ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *homedir);

CURLcode Curl_ssh_range(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep);

CURLcode Curl_ssh_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ easy_pollset *ps);
CURLcode Curl_ssh_ਤਿਆਰੀ_pkey(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ssh_conn *sshc);

#ਜੇ_ਅੰਤ /* USE_SSH */
#ਜੇ_ਅੰਤ /* HEADER_CURL_VSSH_VSSH_H */
