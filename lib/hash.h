#నిర్వచితం_కాకపోతే శీర్షిక_CURL_HASH_H
#నిర్వచించు శీర్షిక_CURL_HASH_H
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
#అన్తర్భావ "curl_setup.h"

రకనిర్వచనం శూన్య (*Curl_hash_dtor)(శూన్య *);

రకనిర్వచనం శూన్య (*Curl_hash_elem_dtor)(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len, శూన్య *p);

/* How keys in a hash are interpreted. */
రకనిర్వచనం గణన {
  CURL_HASH_TYPE_బైట్లు,
  CURL_HASH_TYPE_SOCKET,
  CURL_HASH_TYPE_చివరి
} Curl_hash_type;

నిర్మాణం Curl_hash_element {
  నిర్మాణం Curl_hash_element *తదుపరి;
  శూన్య *ptr;
  Curl_hash_elem_dtor dtor;
  size_t కీలకం_len;
  అక్షరం కీలకం[1]; /* allocated memory following the struct */
};

నిర్మాణం Curl_hash {
  నిర్మాణం Curl_hash_element **పట్టిక;
  /* General element destructor, unless element itself carries one */
  Curl_hash_dtor dtor;
  size_t slots;
  size_t కొలత;
  uint8_t type; /* Curl_hash_type */
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం init;
#అయితే_ముగింపు
};

నిర్మాణం Curl_hash_iterator {
  నిర్మాణం Curl_hash *hash;
  size_t స్థానం_సూచీ;
  నిర్మాణం Curl_hash_element *current;
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం init;
#అయితే_ముగింపు
};

CURLcode Curl_hash_global_init(శూన్య);

శూన్య Curl_hash_init(నిర్మాణం Curl_hash *h,
                    size_t slots,
                    Curl_hash_type type,
                    Curl_hash_dtor dtor);

శూన్య *Curl_hash_కూడు(నిర్మాణం Curl_hash *h,
                    స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len, శూన్య *p);
శూన్య *Curl_hash_add2(నిర్మాణం Curl_hash *h,
                     స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len, శూన్య *p,
                     Curl_hash_elem_dtor dtor);
పూర్ణాంకం Curl_hash_delete(నిర్మాణం Curl_hash *h, స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len);
శూన్య *Curl_hash_pick(నిర్మాణం Curl_hash *h, స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len);

శూన్య Curl_hash_destroy(నిర్మాణం Curl_hash *h);
size_t Curl_hash_లెక్క(నిర్మాణం Curl_hash *h);
శూన్య Curl_hash_clean(నిర్మాణం Curl_hash *h);
శూన్య Curl_hash_clean_with_criterium(నిర్మాణం Curl_hash *h, శూన్య *user,
                                    పూర్ణాంకం (*comp)(శూన్య *, శూన్య *));
శూన్య Curl_hash_ప్రారంభం_iterate(నిర్మాణం Curl_hash *hash,
                             నిర్మాణం Curl_hash_iterator *iter);
నిర్మాణం Curl_hash_element *Curl_hash_తదుపరి_element(
  నిర్మాణం Curl_hash_iterator *iter);

శూన్య Curl_hash_print(నిర్మాణం Curl_hash *h, శూన్య (*func)(శూన్య *));

#అయితే_ముగింపు /* HEADER_CURL_HASH_H */
