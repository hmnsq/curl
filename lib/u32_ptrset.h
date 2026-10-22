#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_U32_PTRSET_H
#وصف_ڪريو سرو_CURL_U32_PTRSET_H
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
#ضمن "curl_setup.h"

تعريفنوع فراغ Curl_u32_ptrset_dtor(uint32_t id, فراغ *ptr);

/* A set that can hold up to INT32_MAX+1 pointers identified by an `id'.
 * Setting a pointer for an existing id replaces the previous one.
 * Getting the pointer for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
بنية u32_ptrset {
  فراغ **معلومات; /* #slots array of pointers */
  uint32_t *ids; /* #slots array of `id` values */
  uint32_t *psl; /* #slots array of "probe sequence length" values */
  uint32_t slotmask;
  uint32_t ڳڻپ;
  Curl_u32_ptrset_dtor *dtor;
  uint8_t slotbits;
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  int32_t init;
#جيڪڏهن_پورو
};

فراغ Curl_u32_ptrset_init(بنية u32_ptrset *سيٽ,
                          Curl_u32_ptrset_dtor *dtor);
فراغ Curl_u32_ptrset_clear(بنية u32_ptrset *سيٽ);

uint32_t Curl_u32_ptrset_ڳڻپ(بنية u32_ptrset *سيٽ);

فراغ *Curl_u32_ptrset_get(بنية u32_ptrset *سيٽ, uint32_t id);

/* Set pointer for id. */
CURLcode Curl_u32_ptrset_سيٽ(بنية u32_ptrset *سيٽ,
                             uint32_t id, فراغ *ptr);

فراغ Curl_u32_ptrset_unset(بنية u32_ptrset *سيٽ, uint32_t id);

تعريفنوع bool Curl_u32_ptrset_گهمو_cb(uint32_t id, فراغ *ptr,
                                       فراغ *user_معلومات);

/* Visit all entries in the set until the callback returns FALSE */
فراغ Curl_u32_ptrset_گهمو(بنية u32_ptrset *سيٽ,
                            Curl_u32_ptrset_گهمو_cb *cb,
                            فراغ *user_معلومات);

#جيڪڏهن_پورو /* HEADER_CURL_U32_PTRSET_H */
