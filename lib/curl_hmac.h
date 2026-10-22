#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HMAC_H
#నిర్వచించు శీర్షిక_CURL_HMAC_H
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

#ఒకవేళ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  !defined(CURL_DISABLE_HTTPSIG) ||                                   \
  defined(USE_LIBSSH2) || defined(USE_SSL)

#నిర్వచించు HMAC_MD5_నిడివి 16

రకనిర్వచనం CURLcode (*HMAC_hinit)(శూన్య *context);
రకనిర్వచనం శూన్య (*HMAC_hupdate)(శూన్య *context,
                             స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                             గుర్తులేని పూర్ణాంకం len);
రకనిర్వచనం శూన్య (*HMAC_hfinal)(గుర్తులేని అక్షరం *ఫలితం, శూన్య *context);

/* Per-hash function HMAC parameters. */
నిర్మాణం HMAC_params {
  HMAC_hinit  hinit;       /* Initialize context procedure. */
  HMAC_hupdate hupdate;    /* Update context with data. */
  HMAC_hfinal hfinal;      /* Get final result procedure. */
  గుర్తులేని పూర్ణాంకం ctxtsize;   /* Context structure size. */
  గుర్తులేని పూర్ణాంకం maxkeylen;  /* Maximum key length (bytes). */
  గుర్తులేని పూర్ణాంకం resultlen;  /* Result length (bytes). */
};

/* HMAC computation context. */
నిర్మాణం HMAC_context {
  స్థిరరాశి నిర్మాణం HMAC_params *hash; /* Hash function definition. */
  శూన్య *hashctxt1;         /* Hash function context 1. */
  శూన్య *hashctxt2;         /* Hash function context 2. */
};

/* Prototypes. */
నిర్మాణం HMAC_context *Curl_HMAC_init(స్థిరరాశి నిర్మాణం HMAC_params *hashparams,
                                    స్థిరరాశి గుర్తులేని అక్షరం *కీలకం,
                                    గుర్తులేని పూర్ణాంకం keylen);
శూన్య Curl_HMAC_update(నిర్మాణం HMAC_context *ctxt,
                      స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                      గుర్తులేని పూర్ణాంకం len);
పూర్ణాంకం Curl_HMAC_final(నిర్మాణం HMAC_context *ctxt, గుర్తులేని అక్షరం *output);

CURLcode Curl_hmacit(స్థిరరాశి నిర్మాణం HMAC_params *hashparams,
                     స్థిరరాశి గుర్తులేని అక్షరం *కీలకం, స్థిరరాశి size_t keylen,
                     స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం, size_t datalen,
                     గుర్తులేని అక్షరం *output);
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_HMAC_H */
