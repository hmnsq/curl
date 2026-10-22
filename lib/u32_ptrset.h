#నిర్వచితం_కాకపోతే శీర్షిక_CURL_U32_PTRSET_H
#నిర్వచించు శీర్షిక_CURL_U32_PTRSET_H
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

రకనిర్వచనం శూన్య Curl_u32_ptrset_dtor(uint32_t id, శూన్య *ptr);

/* A set that can hold up to INT32_MAX+1 pointers identified by an `id'.
 * Setting a pointer for an existing id replaces the previous one.
 * Getting the pointer for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
నిర్మాణం u32_ptrset {
  శూన్య **దత్తాంశం; /* #slots array of pointers */
  uint32_t *ids; /* #slots array of `id` values */
  uint32_t *psl; /* #slots array of "probe sequence length" values */
  uint32_t slotmask;
  uint32_t లెక్క;
  Curl_u32_ptrset_dtor *dtor;
  uint8_t slotbits;
#నిర్వచితమైతే DEBUGBUILD
  int32_t init;
#అయితే_ముగింపు
};

శూన్య Curl_u32_ptrset_init(నిర్మాణం u32_ptrset *సమితి,
                          Curl_u32_ptrset_dtor *dtor);
శూన్య Curl_u32_ptrset_clear(నిర్మాణం u32_ptrset *సమితి);

uint32_t Curl_u32_ptrset_లెక్క(నిర్మాణం u32_ptrset *సమితి);

శూన్య *Curl_u32_ptrset_get(నిర్మాణం u32_ptrset *సమితి, uint32_t id);

/* Set pointer for id. */
CURLcode Curl_u32_ptrset_సమితి(నిర్మాణం u32_ptrset *సమితి,
                             uint32_t id, శూన్య *ptr);

శూన్య Curl_u32_ptrset_unset(నిర్మాణం u32_ptrset *సమితి, uint32_t id);

రకనిర్వచనం bool Curl_u32_ptrset_సందర్శించు_cb(uint32_t id, శూన్య *ptr,
                                       శూన్య *user_దత్తాంశం);

/* Visit all entries in the set until the callback returns FALSE */
శూన్య Curl_u32_ptrset_సందర్శించు(నిర్మాణం u32_ptrset *సమితి,
                            Curl_u32_ptrset_సందర్శించు_cb *cb,
                            శూన్య *user_దత్తాంశం);

#అయితే_ముగింపు /* HEADER_CURL_U32_PTRSET_H */
