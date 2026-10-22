#अगर_अघोषित HEADER_PSL_H
#घोषणा HEADER_PSL_H
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
#अगर_घोषित USE_LIBPSL

#समावेश <libpsl.h>

काष्ठा Curl_easy;

#घोषणा PSL_TTL (72 * 3600)     /* PSL time to live before a refresh. */

काष्ठा PslCache {
  स्थिर psl_ctx_t *psl; /* The PSL. */
  समय_प्रकार expires; /* Time this PSL life expires. */
  BIT(dynamic); /* PSL should be released when no longer needed. */
};

स्थिर psl_ctx_t *Curl_psl_use(काष्ठा Curl_easy *easy);
व्योम Curl_psl_release(काष्ठा Curl_easy *easy);
व्योम Curl_psl_destroy(काष्ठा PslCache *pslcache);

#अन्यथा

#घोषणा Curl_psl_use(easy) शून्य
#घोषणा Curl_psl_release(easy)
#घोषणा Curl_psl_destroy(pslcache)

#पूर्ण_अगर /* USE_LIBPSL */
#पूर्ण_अगर /* HEADER_PSL_H */
