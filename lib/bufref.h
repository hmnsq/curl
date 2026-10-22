#నిర్వచితం_కాకపోతే శీర్షిక_CURL_BUFREF_H
#నిర్వచించు శీర్షిక_CURL_BUFREF_H
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

/*
 * Generic buffer reference.
 */
నిర్మాణం bufref {
  శూన్య (*dtor)(శూన్య *);         /* Associated destructor. */
  స్థిరరాశి గుర్తులేని అక్షరం *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం signature;                /* Detect API use mistakes. */
#అయితే_ముగింపు
};

శూన్య Curl_bufref_init(నిర్మాణం bufref *br);
శూన్య Curl_bufref_సమితి(నిర్మాణం bufref *br, స్థిరరాశి శూన్య *ptr, size_t len,
                     శూన్య (*dtor)(శూన్య *));
స్థిరరాశి అక్షరం *Curl_bufref_ptr(స్థిరరాశి నిర్మాణం bufref *br);
స్థిరరాశి గుర్తులేని అక్షరం *Curl_bufref_uptr(స్థిరరాశి నిర్మాణం bufref *br);
size_t Curl_bufref_len(స్థిరరాశి నిర్మాణం bufref *br);
CURLcode Curl_bufref_memdup0(నిర్మాణం bufref *br, స్థిరరాశి శూన్య *ptr, size_t len);
శూన్య Curl_bufref_free(నిర్మాణం bufref *br);

/* return a strdup() version of the buffer */
#నిర్వచించు Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#అయితే_ముగింపు
