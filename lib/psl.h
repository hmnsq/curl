#నిర్వచితం_కాకపోతే శీర్షిక_PSL_H
#నిర్వచించు శీర్షిక_PSL_H
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
#నిర్వచితమైతే USE_LIBPSL

#అన్తర్భావ <libpsl.h>

నిర్మాణం Curl_easy;

#నిర్వచించు PSL_TTL (72 * 3600)     /* PSL time to live before a refresh. */

నిర్మాణం PslCache {
  స్థిరరాశి psl_ctx_t *psl; /* The PSL. */
  time_t expires; /* Time this PSL life expires. */
  బిట్టు(dynamic); /* PSL should be released when no longer needed. */
};

స్థిరరాశి psl_ctx_t *Curl_psl_use(నిర్మాణం Curl_easy *easy);
శూన్య Curl_psl_release(నిర్మాణం Curl_easy *easy);
శూన్య Curl_psl_destroy(నిర్మాణం PslCache *pslcache);

#లేకపోతే

#నిర్వచించు Curl_psl_use(easy) ఖాళీ
#నిర్వచించు Curl_psl_release(easy)
#నిర్వచించు Curl_psl_destroy(pslcache)

#అయితే_ముగింపు /* USE_LIBPSL */
#అయితే_ముగింపు /* HEADER_PSL_H */
