#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_NTLM_CORE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_NTLM_CORE_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURL_NTLM_CORE

ರಚನೆ ntlmdata;

/* Helpers to generate function byte arguments in little endian order */
#ವ್ಯಾಖ್ಯಾನಿಸು SHORTPAIR(x)   ((ಪೂರ್ಣಾಂಕ)((x) & 0xff)), ((ಪೂರ್ಣಾಂಕ)(((x) >> 8) & 0xff))
#ವ್ಯಾಖ್ಯಾನಿಸು LONGQUARTET(x) ((ಪೂರ್ಣಾಂಕ)((x) & 0xff)), ((ಪೂರ್ಣಾಂಕ)(((x) >> 8) & 0xff)), \
  ((ಪೂರ್ಣಾಂಕ)(((x) >> 16) & 0xff)), ((ಪೂರ್ಣಾಂಕ)(((x) >> 24) & 0xff))

ಶೂನ್ಯ Curl_ntlm_core_lm_resp(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿಗಳು,
                            ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *plaintext,
                            ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *results);

CURLcode Curl_ntlm_core_mk_lm_hash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *password,
                                   ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *lmbuffer /* 21 bytes */);

CURLcode Curl_ntlm_core_mk_nt_hash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *password,
                                   ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntbuffer /* 21 bytes */);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINDOWS_SSPI

CURLcode Curl_hmac_md5(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ keylen,
                       ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ datalen,
                       ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output);

CURLcode Curl_ntlm_core_mk_ntlmv2_hash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user, size_t userlen,
                                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain, size_t domlen,
                                       ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntlmhash,
                                       ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntlmv2hash);

CURLcode Curl_ntlm_core_mk_ntlmv2_resp(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntlmv2hash,
                                       ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *challenge_client,
                                       ಸ್ಥಿರಾಂಕ ರಚನೆ ntlmdata *ntlm,
                                       ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **ntresp,
                                       ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *ntresp_len);

CURLcode Curl_ntlm_core_mk_lmv2_resp(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ntlmv2hash,
                                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *challenge_client,
                                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *challenge_server,
                                     ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *lmresp);

#ಆದರೆ_ಅಂತ್ಯ /* !USE_WINDOWS_SSPI */

#ಆದರೆ_ಅಂತ್ಯ /* USE_CURL_NTLM_CORE */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_NTLM_CORE_H */
