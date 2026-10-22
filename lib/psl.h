#اگر_غیر_تعریف_شدہ سرخی_PSL_H
#تعریف_کرو سرخی_PSL_H
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
#اگر_تعریف_شدہ USE_LIBPSL

#شامل <libpsl.h>

ساخت Curl_easy;

#تعریف_کرو PSL_TTL (72 * 3600)     /* PSL time to live before a refresh. */

ساخت PslCache {
  ثابت psl_ctx_t *psl; /* The PSL. */
  time_t expires; /* Time this PSL life expires. */
  ثنائی_ہندسہ(dynamic); /* PSL should be released when no longer needed. */
};

ثابت psl_ctx_t *Curl_psl_use(ساخت Curl_easy *easy);
خالی Curl_psl_release(ساخت Curl_easy *easy);
خالی Curl_psl_destroy(ساخت PslCache *pslcache);

#ورنہ

#تعریف_کرو Curl_psl_use(easy) تہی
#تعریف_کرو Curl_psl_release(easy)
#تعریف_کرو Curl_psl_destroy(pslcache)

#اگر_ختم /* USE_LIBPSL */
#اگر_ختم /* HEADER_PSL_H */
