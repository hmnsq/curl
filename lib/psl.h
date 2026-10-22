#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_PSL_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_PSL_H
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL

#ಅನ್ತರ್ಭಾವ <libpsl.h>

ರಚನೆ Curl_easy;

#ವ್ಯಾಖ್ಯಾನಿಸು PSL_TTL (72 * 3600)     /* PSL time to live before a refresh. */

ರಚನೆ PslCache {
  ಸ್ಥಿರಾಂಕ psl_ctx_t *psl; /* The PSL. */
  time_t expires; /* Time this PSL life expires. */
  ಬಿಟ್(dynamic); /* PSL should be released when no longer needed. */
};

ಸ್ಥಿರಾಂಕ psl_ctx_t *Curl_psl_use(ರಚನೆ Curl_easy *easy);
ಶೂನ್ಯ Curl_psl_release(ರಚನೆ Curl_easy *easy);
ಶೂನ್ಯ Curl_psl_destroy(ರಚನೆ PslCache *pslcache);

#ಇಲ್ಲದಿದ್ದರೆ

#ವ್ಯಾಖ್ಯಾನಿಸು Curl_psl_use(easy) ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_psl_release(easy)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_psl_destroy(pslcache)

#ಆದರೆ_ಅಂತ್ಯ /* USE_LIBPSL */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_PSL_H */
