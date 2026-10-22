#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VSSH_VSSH_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VSSH_VSSH_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH

#ಅನ್ತರ್ಭಾವ "urldata.h"

ರಚನೆ ssh_conn;

CURLcode Curl_getworkingpath(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *homedir,
                             ಅಕ್ಷರ **ಮಾರ್ಗ);

CURLcode Curl_get_pathname(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **cpp, ಅಕ್ಷರ **ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *homedir);

CURLcode Curl_ssh_range(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep);

CURLcode Curl_ssh_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ easy_pollset *ps);
CURLcode Curl_ssh_ಸಿದ್ಧತೆ_pkey(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ssh_conn *sshc);

#ಆದರೆ_ಅಂತ್ಯ /* USE_SSH */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VSSH_VSSH_H */
