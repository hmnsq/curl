#अगर_अघोषित HEADER_CURL_UINT_TABLE_H
#घोषणा HEADER_CURL_UINT_TABLE_H
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

काष्ठा uint32_tbl {
  व्योम **rows;  /* array of void* holding entries */
  uint32_t nrows;  /* length of `rows` array */
  uint32_t nentries; /* entries in table */
  uint32_t last_कुंजी_added; /* UINT_MAX or last key added */
#अगर_घोषित DEBUGBUILD
  पूर्णांक init;
#पूर्ण_अगर
};

/* Initialize the table with 0 capacity. */
व्योम Curl_uint32_tbl_init(काष्ठा uint32_tbl *tbl);

/* Resize the table to change capacity `nmax`. When `nmax` is reduced,
 * all present entries with key equal or larger to `nmax` are removed. */
CURLcode Curl_uint32_tbl_resize(काष्ठा uint32_tbl *tbl, uint32_t nrows);

/* Destroy the table, freeing its storage. */
व्योम Curl_uint32_tbl_destroy(काष्ठा uint32_tbl *tbl);

/* Get the table capacity. */
uint32_t Curl_uint32_tbl_capacity(काष्ठा uint32_tbl *tbl);

/* Get the number of entries in the table. */
uint32_t Curl_uint32_tbl_गिनती(काष्ठा uint32_tbl *tbl);

/* Get the entry for key or NULL if not present */
व्योम *Curl_uint32_tbl_get(काष्ठा uint32_tbl *tbl, uint32_t कुंजी);

/* Add a new entry to the table and assign it a free key.
 * Returns FALSE if the table is full.
 *
 * Keys are assigned in a round-robin manner.
 * No matter the capacity, UINT_MAX is never assigned. */
bool Curl_uint32_tbl_add(काष्ठा uint32_tbl *tbl, व्योम *entry, uint32_t *pkey);

/* Remove the entry with `key`. */
व्योम Curl_uint32_tbl_remove(काष्ठा uint32_tbl *tbl, uint32_t कुंजी);

/* Return TRUE if the table contains an tryn with that keys. */
bool Curl_uint32_tbl_contains(काष्ठा uint32_tbl *tbl, uint32_t कुंजी);

/* Get the first entry in the table (with the smallest `key`).
 * Returns FALSE if the table is empty. */
bool Curl_uint32_tbl_first(काष्ठा uint32_tbl *tbl,
                           uint32_t *pkey, व्योम **pentry);

/* Get the next key in the table, following `last_key` in natural order.
 * Put another way, this is the smallest key greater than `last_key` in
 * the table. `last_key` does not have to be present in the table.
 *
 * Returns FALSE when no such entry is in the table.
 *
 * This allows to iterate the table while being modified:
 * - added keys higher than 'last_key' will be picked up by the iteration.
 * - added keys lower than 'last_key' will not show up.
 * - removed keys lower or equal to 'last_key' will not show up.
 * - removed keys higher than 'last_key' will not be visited. */
bool Curl_uint32_tbl_अग्र(काष्ठा uint32_tbl *tbl, uint32_t last_कुंजी,
                          uint32_t *pkey, व्योम **pentry);

#पूर्ण_अगर /* HEADER_CURL_UINT_TABLE_H */
