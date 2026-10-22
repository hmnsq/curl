#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_U32_PTRSET_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_U32_PTRSET_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ Curl_u32_ptrset_dtor(uint32_t id, ಶೂನ್ಯ *ptr);

/* A set that can hold up to INT32_MAX+1 pointers identified by an `id'.
 * Setting a pointer for an existing id replaces the previous one.
 * Getting the pointer for an id not in the set returns NULL.
 * Setting an id to NULL unsets the id.
 */
ರಚನೆ u32_ptrset {
  ಶೂನ್ಯ **ದತ್ತಾಂಶ; /* #slots array of pointers */
  uint32_t *ids; /* #slots array of `id` values */
  uint32_t *psl; /* #slots array of "probe sequence length" values */
  uint32_t slotmask;
  uint32_t ಎಣಿಕೆ_2;
  Curl_u32_ptrset_dtor *dtor;
  uint8_t slotbits;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  int32_t init;
#ಆದರೆ_ಅಂತ್ಯ
};

ಶೂನ್ಯ Curl_u32_ptrset_init(ರಚನೆ u32_ptrset *ಗಣ,
                          Curl_u32_ptrset_dtor *dtor);
ಶೂನ್ಯ Curl_u32_ptrset_clear(ರಚನೆ u32_ptrset *ಗಣ);

uint32_t Curl_u32_ptrset_ಎಣಿಕೆ(ರಚನೆ u32_ptrset *ಗಣ);

ಶೂನ್ಯ *Curl_u32_ptrset_get(ರಚನೆ u32_ptrset *ಗಣ, uint32_t id);

/* Set pointer for id. */
CURLcode Curl_u32_ptrset_ಗಣ(ರಚನೆ u32_ptrset *ಗಣ,
                             uint32_t id, ಶೂನ್ಯ *ptr);

ಶೂನ್ಯ Curl_u32_ptrset_unset(ರಚನೆ u32_ptrset *ಗಣ, uint32_t id);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ bool Curl_u32_ptrset_ಭೇಟಿ_cb(uint32_t id, ಶೂನ್ಯ *ptr,
                                       ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

/* Visit all entries in the set until the callback returns FALSE */
ಶೂನ್ಯ Curl_u32_ptrset_ಭೇಟಿ(ರಚನೆ u32_ptrset *ಗಣ,
                            Curl_u32_ptrset_ಭೇಟಿ_cb *cb,
                            ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_U32_PTRSET_H */
