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
#अन्तर्भाव "curl_setup.h"

प्रकारनिर्देश शून्य Curl_u32_ptrset_dtor(uint32_t id, शून्य *ptr);

/* A set that can hold up to INT32_MAX+1 pointers identified by an `id'.
 * Setting a pointer for an existing id replaces the previous one.
 * Getting the pointer for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
संरचना u32_ptrset {
  शून्य **data; /* #slots array of pointers */
  uint32_t *ids; /* #slots array of `id` values */
  uint32_t *psl; /* #slots array of "probe sequence length" values */
  uint32_t slotmask;
  uint32_t गिनती;
  Curl_u32_ptrset_dtor *dtor;
  uint8_t slotbits;
#ifdef DEBUGBUILD
  int32_t init;
#endif
};

शून्य Curl_u32_ptrset_init(संरचना u32_ptrset *set,
                          Curl_u32_ptrset_dtor *dtor);
शून्य Curl_u32_ptrset_clear(संरचना u32_ptrset *set);

uint32_t Curl_u32_ptrset_गिनती(संरचना u32_ptrset *set);

शून्य *Curl_u32_ptrset_get(संरचना u32_ptrset *set, uint32_t id);

/* Set pointer for id. */
CURLcode Curl_u32_ptrset_set(संरचना u32_ptrset *set,
                             uint32_t id, शून्य *ptr);

शून्य Curl_u32_ptrset_unset(संरचना u32_ptrset *set, uint32_t id);

प्रकारनिर्देश bool Curl_u32_ptrset_visit_cb(uint32_t id, शून्य *ptr,
                                       शून्य *user_data);

/* Visit all entries in the set until the callback returns FALSE */
शून्य Curl_u32_ptrset_visit(संरचना u32_ptrset *set,
                            Curl_u32_ptrset_visit_cb *cb,
                            शून्य *user_data);

#endif /* HEADER_CURL_U32_PTRSET_H */
