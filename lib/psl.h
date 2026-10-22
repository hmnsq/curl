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

#अन्तर्भाव <libpsl.h>

संरचना Curl_easy;

#define PSL_TTL (72 * 3600)     /* PSL time to live before a refresh. */

संरचना PslCache {
  स्थिरांक psl_ctx_t *psl; /* The PSL. */
  समय_प्रकार expires; /* Time this PSL life expires. */
  BIT(dynamic); /* PSL should be released when no longer needed. */
};

स्थिरांक psl_ctx_t *Curl_psl_use(संरचना Curl_easy *easy);
शून्य Curl_psl_release(संरचना Curl_easy *easy);
शून्य Curl_psl_destroy(संरचना PslCache *pslcache);

#अन्यथा

#define Curl_psl_use(easy) रिक्त
#define Curl_psl_release(easy)
#define Curl_psl_destroy(pslcache)

#endif /* USE_LIBPSL */
#endif /* HEADER_PSL_H */
