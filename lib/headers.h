#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ಶೀರ್ಷಿಕೆ_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ಶೀರ್ಷಿಕೆ_H
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

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ಶೀರ್ಷಿಕೆಗಳು_API)

ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store {
  ರಚನೆ Curl_llist_ಗಂಟು ಗಂಟು;
  ಅಕ್ಷರ *name; /* points into 'buffer' */
  ಅಕ್ಷರ *ಮೌಲ್ಯ; /* points into 'buffer' */
  size_t nh_amount; /* cached amount for curl_easy_nextheader() */
  size_t nh_ಸೂಚ್ಯಂಕ; /* cached index for curl_easy_nextheader() */
  ಪೂರ್ಣಾಂಕ request; /* 0 is the first request, then 1.. 2.. */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ type; /* CURLH_* defines */
  ಅಕ್ಷರ ಬಫರ್[1]; /* this is the raw header blob */
};

/*
 * Initialize header collecting for a transfer.
 * Add a client writer that catches CLIENTWRITE_HEADER writes.
 */
CURLcode Curl_ಶೀರ್ಷಿಕೆಗಳು_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/*
 * Curl_headers_push() gets passed a full header to store.
 */
CURLcode Curl_ಶೀರ್ಷಿಕೆಗಳು_ತಳ್ಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ,
                           size_t hlen, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ type);

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_ಶೀರ್ಷಿಕೆಗಳು_cleanup(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ಶೀರ್ಷಿಕೆಗಳು_init(x)          CURLE_OK
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ಶೀರ್ಷಿಕೆಗಳು_ತಳ್ಳು(x, y, z, a) \
  ((ಶೂನ್ಯ)(x), (ಶೂನ್ಯ)(y), (ಶೂನ್ಯ)(z), (ಶೂನ್ಯ)(a), CURLE_OK)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ಶೀರ್ಷಿಕೆಗಳು_cleanup(x)       Curl_nop_stmt
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_HEADER_H */
