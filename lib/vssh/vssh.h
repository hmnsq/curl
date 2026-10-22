#अगर_अघोषित HEADER_CURL_VSSH_VSSH_H
#घोषणा HEADER_CURL_VSSH_VSSH_H
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

#अगर_घोषित USE_SSH

#समावेश "urldata.h"

काष्ठा ssh_conn;

CURLcode Curl_getworkingpath(काष्ठा Curl_easy *तथ्य,
                             स्थिर अक्षर *homedir,
                             अक्षर **path);

CURLcode Curl_get_pathname(स्थिर अक्षर **cpp, अक्षर **path, स्थिर अक्षर *homedir);

CURLcode Curl_ssh_range(काष्ठा Curl_easy *तथ्य,
                        स्थिर अक्षर *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep);

CURLcode Curl_ssh_pollset(काष्ठा Curl_easy *तथ्य, काष्ठा easy_pollset *ps);
CURLcode Curl_ssh_setup_pkey(काष्ठा Curl_easy *तथ्य, काष्ठा ssh_conn *sshc);

#पूर्ण_अगर /* USE_SSH */
#पूर्ण_अगर /* HEADER_CURL_VSSH_VSSH_H */
