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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_SSH

#अन्तर्भाव "urldata.h"

संरचना ssh_conn;

CURLcode Curl_getworkingpath(संरचना Curl_easy *data,
                             ध्रुव वर्ण *homedir,
                             वर्ण **path);

CURLcode Curl_get_pathname(ध्रुव वर्ण **cpp, वर्ण **path, ध्रुव वर्ण *homedir);

CURLcode Curl_ssh_range(संरचना Curl_easy *data,
                        ध्रुव वर्ण *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep);

CURLcode Curl_ssh_pollset(संरचना Curl_easy *data, संरचना easy_pollset *ps);
CURLcode Curl_ssh_setup_pkey(संरचना Curl_easy *data, संरचना ssh_conn *sshc);

#endif /* USE_SSH */
#endif /* HEADER_CURL_VSSH_VSSH_H */
