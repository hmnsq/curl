#अगर_अघोषित HEADER_CURL_U32_PTRSET_H
#घोषणा HEADER_CURL_U32_PTRSET_H
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
#समावेश "curl_setup.h"

प्रकार व्योम Curl_u32_ptrset_dtor(uint32_t id, व्योम *ptr);

/* A set that can hold up to INT32_MAX+1 pointers identified by an `id'.
 * Setting a pointer for an existing id replaces the previous one.
 * Getting the pointer for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
काष्ठा u32_ptrset {
  व्योम **तथ्य; /* #slots array of pointers */
  uint32_t *ids; /* #slots array of `id` values */
  uint32_t *psl; /* #slots array of "probe sequence length" values */
  uint32_t slotmask;
  uint32_t गिनती;
  Curl_u32_ptrset_dtor *dtor;
  uint8_t slotbits;
#अगर_घोषित DEBUGBUILD
  int32_t init;
#पूर्ण_अगर
};

व्योम Curl_u32_ptrset_init(काष्ठा u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor);
व्योम Curl_u32_ptrset_clear(काष्ठा u32_ptrset *set);

uint32_t Curl_u32_ptrset_गिनती(काष्ठा u32_ptrset *set);

व्योम *Curl_u32_ptrset_get(काष्ठा u32_ptrset *set, uint32_t id);

/* Set pointer for id. */
CURLcode Curl_u32_ptrset_set(काष्ठा u32_ptrset *set,
                             uint32_t id, व्योम *ptr);

व्योम Curl_u32_ptrset_unset(काष्ठा u32_ptrset *set, uint32_t id);

प्रकार bool Curl_u32_ptrset_visit_cb(uint32_t id, व्योम *ptr,
                                       व्योम *user_तथ्य);

/* Visit all entries in the set until the callback returns FALSE */
व्योम Curl_u32_ptrset_visit(काष्ठा u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            व्योम *user_तथ्य);

#पूर्ण_अगर /* HEADER_CURL_U32_PTRSET_H */
