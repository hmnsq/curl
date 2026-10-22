#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_IDN_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_IDN_H
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

ರಚನೆ Curl_str;
ರಚನೆ hostname;

bool Curl_is_ASCII_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname);
bool Curl_is_ASCII_str(ರಚನೆ Curl_str *s);

CURLcode Curl_idnconvert_hostname(ರಚನೆ hostname *host);

#ಆದರೆ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#ವ್ಯಾಖ್ಯಾನಿಸು USE_IDN
ಶೂನ್ಯ Curl_free_idnconverted_hostname(ರಚನೆ hostname *host);
CURLcode Curl_idn_decode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, ಅಕ್ಷರ **output);
CURLcode Curl_idn_encode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *puny, ಅಕ್ಷರ **output);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_free_idnconverted_hostname(x)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_idn_decode(x) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_IDN_H */
