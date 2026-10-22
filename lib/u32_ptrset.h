#اگر_غیر_تعریف_شدہ سرخی_CURL_U32_PTRSET_H
#تعریف_کرو سرخی_CURL_U32_PTRSET_H
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
#شامل "curl_setup.h"

قسمتعریف خالی Curl_u32_ptrset_dtor(uint32_t id, خالی *ptr);

/* A set that can hold up to INT32_MAX+1 pointers identified by an `id'.
 * Setting a pointer for an existing id replaces the previous one.
 * Getting the pointer for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
ساخت u32_ptrset {
  خالی **ڈیٹا; /* #slots array of pointers */
  uint32_t *ids; /* #slots array of `id` values */
  uint32_t *psl; /* #slots array of "probe sequence length" values */
  uint32_t slotmask;
  uint32_t گنتی;
  Curl_u32_ptrset_dtor *dtor;
  uint8_t slotbits;
#اگر_تعریف_شدہ DEBUGBUILD
  int32_t init;
#اگر_ختم
};

خالی Curl_u32_ptrset_init(ساخت u32_ptrset *سیٹ,
                          Curl_u32_ptrset_dtor *dtor);
خالی Curl_u32_ptrset_clear(ساخت u32_ptrset *سیٹ);

uint32_t Curl_u32_ptrset_گنتی(ساخت u32_ptrset *سیٹ);

خالی *Curl_u32_ptrset_get(ساخت u32_ptrset *سیٹ, uint32_t id);

/* Set pointer for id. */
CURLcode Curl_u32_ptrset_سیٹ(ساخت u32_ptrset *سیٹ,
                             uint32_t id, خالی *ptr);

خالی Curl_u32_ptrset_unset(ساخت u32_ptrset *سیٹ, uint32_t id);

قسمتعریف bool Curl_u32_ptrset_دورہ_cb(uint32_t id, خالی *ptr,
                                       خالی *user_ڈیٹا);

/* Visit all entries in the set until the callback returns FALSE */
خالی Curl_u32_ptrset_دورہ(ساخت u32_ptrset *سیٹ,
                            Curl_u32_ptrset_دورہ_cb *cb,
                            خالی *user_ڈیٹا);

#اگر_ختم /* HEADER_CURL_U32_PTRSET_H */
