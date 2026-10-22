#ifndef HEADER_CURL_U32_PTRSET_H
#define HEADER_CURL_U32_PTRSET_H
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
#અન્તર્ભાવ "curl_setup.h"

પ્રકારવ્યાખ્યા શૂન્ય Curl_u32_ptrset_dtor(uint32_t id, શૂન્ય *ptr);

/* A set that can hold up to INT32_MAX+1 pointers identified by an `id'.
 * Setting a pointer for an existing id replaces the previous one.
 * Getting the pointer for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
સંરચના u32_ptrset {
  શૂન્ય **data; /* #slots array of pointers */
  uint32_t *ids; /* #slots array of `id` values */
  uint32_t *psl; /* #slots array of "probe sequence length" values */
  uint32_t slotmask;
  uint32_t ગણતરી;
  Curl_u32_ptrset_dtor *dtor;
  uint8_t slotbits;
#ifdef DEBUGBUILD
  int32_t init;
#endif
};

શૂન્ય Curl_u32_ptrset_init(સંરચના u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor);
શૂન્ય Curl_u32_ptrset_clear(સંરચના u32_ptrset *set);

uint32_t Curl_u32_ptrset_ગણતરી(સંરચના u32_ptrset *set);

શૂન્ય *Curl_u32_ptrset_get(સંરચના u32_ptrset *set, uint32_t id);

/* Set pointer for id. */
CURLcode Curl_u32_ptrset_set(સંરચના u32_ptrset *set,
                             uint32_t id, શૂન્ય *ptr);

શૂન્ય Curl_u32_ptrset_unset(સંરચના u32_ptrset *set, uint32_t id);

પ્રકારવ્યાખ્યા bool Curl_u32_ptrset_visit_cb(uint32_t id, શૂન્ય *ptr,
                                       શૂન્ય *user_data);

/* Visit all entries in the set until the callback returns FALSE */
શૂન્ય Curl_u32_ptrset_visit(સંરચના u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            શૂન્ય *user_data);

#endif /* HEADER_CURL_U32_PTRSET_H */
