#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SETOPT_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SETOPT_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
CURLcode Curl_setopt_SSLVERSION(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option,
                                ದೀರ್ಘ arg);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_setopt_SSLVERSION(a, b, c) CURLE_NOT_BUILT_ಒಳ
#ಆದರೆ_ಅಂತ್ಯ

CURLcode Curl_setstropt(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಎಣಿಕೆ dupstring id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;
CURLcode Curl_setblobopt(ರಚನೆ curl_blob **blobp,
                         ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *blob) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;
CURLcode Curl_vsetopt(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLoption option, va_list param)
  WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SETOPT_H */
