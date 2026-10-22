#ifndef HEADER_PSL_H
#define HEADER_PSL_H
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
#ifdef USE_LIBPSL

#অন্তর্ভাব <libpsl.h>

কাঠামো Curl_easy;

#define PSL_TTL (72 * 3600)     /* PSL time to live before a refresh. */

কাঠামো PslCache {
  ধ্রুবক psl_ctx_t *psl; /* The PSL. */
  time_t expires; /* Time this PSL life expires. */
  BIT(dynamic); /* PSL should be released when no longer needed. */
};

ধ্রুবক psl_ctx_t *Curl_psl_use(কাঠামো Curl_easy *easy);
শূন্য Curl_psl_release(কাঠামো Curl_easy *easy);
শূন্য Curl_psl_destroy(কাঠামো PslCache *pslcache);

#অন্যথা

#define Curl_psl_use(easy) ফাঁকা
#define Curl_psl_release(easy)
#define Curl_psl_destroy(pslcache)

#endif /* USE_LIBPSL */
#endif /* HEADER_PSL_H */
