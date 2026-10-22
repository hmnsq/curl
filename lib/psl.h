#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_PSL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_PSL_H
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL

#ਅਨ੍ਤਰ੍ਭਾਵ <libpsl.h>

ਬਣਤਰ Curl_easy;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PSL_TTL (72 * 3600)     /* PSL time to live before a refresh. */

ਬਣਤਰ PslCache {
  ਅਚਲ psl_ctx_t *psl; /* The PSL. */
  time_t expires; /* Time this PSL life expires. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(dynamic); /* PSL should be released when no longer needed. */
};

ਅਚਲ psl_ctx_t *Curl_psl_use(ਬਣਤਰ Curl_easy *easy);
ਖਾਲੀ Curl_psl_release(ਬਣਤਰ Curl_easy *easy);
ਖਾਲੀ Curl_psl_destroy(ਬਣਤਰ PslCache *pslcache);

#ਨਹੀਂਤਾਂ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_psl_use(easy) ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_psl_release(easy)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_psl_destroy(pslcache)

#ਜੇ_ਅੰਤ /* USE_LIBPSL */
#ਜੇ_ਅੰਤ /* HEADER_PSL_H */
