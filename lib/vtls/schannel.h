#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SCHANNEL_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SCHANNEL_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Marc Hoersken, <info@marc-hoersken.de>, et al.
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SCHANNEL

#ಅನ್ತರ್ಭಾವ <schannel.h>

#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_schannel;

CURLcode Curl_verify_host(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

CURLcode Curl_verify_certificate(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ಆದರೆ_ಅಂತ್ಯ /* USE_SCHANNEL */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SCHANNEL_H */
