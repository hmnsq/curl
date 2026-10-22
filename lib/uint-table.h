#यदि_अपरिभाषितम् शीर्षकम्_CURL_UINT_सारणी_H
#परिभाषय शीर्षकम्_CURL_UINT_सारणी_H
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

संरचना uint32_tbl {
  शून्य **पङ्क्तिकाः;  /* array of void* holding entries */
  uint32_t nrows;  /* length of `rows` array */
  uint32_t nentries; /* entries in table */
  uint32_t चरमः_कुञ्चिका_added; /* UINT_MAX or last key added */
#यदि_परिभाषितम् DEBUGBUILD
  पूर्णाङ्क init;
#यदि_अन्तः
};

/* Initialize the table with 0 capacity. */
शून्य Curl_uint32_tbl_init(संरचना uint32_tbl *tbl);

/* Resize the table to change capacity `nmax`. When `nmax` is reduced,
 * all present entries with key equal or larger to `nmax` are removed. */
CURLcode Curl_uint32_tbl_resize(संरचना uint32_tbl *tbl, uint32_t nrows);

/* Destroy the table, freeing its storage. */
शून्य Curl_uint32_tbl_destroy(संरचना uint32_tbl *tbl);

/* Get the table capacity. */
uint32_t Curl_uint32_tbl_क्षमता(संरचना uint32_tbl *tbl);

/* Get the number of entries in the table. */
uint32_t Curl_uint32_tbl_गणनम्(संरचना uint32_tbl *tbl);

/* Get the entry for key or NULL if not present */
शून्य *Curl_uint32_tbl_get(संरचना uint32_tbl *tbl, uint32_t कुञ्चिका);

/* Add a new entry to the table and assign it a free key.
 * Returns FALSE if the table is full.
 *
 * Keys are assigned in a round-robin manner.
 * No matter the capacity, UINT_MAX is never assigned. */
bool Curl_uint32_tbl_सङ्कलय(संरचना uint32_tbl *tbl, शून्य *प्रविष्टिः, uint32_t *pkey);

/* Remove the entry with `key`. */
शून्य Curl_uint32_tbl_remove(संरचना uint32_tbl *tbl, uint32_t कुञ्चिका);

/* Return TRUE if the table contains an tryn with that keys. */
bool Curl_uint32_tbl_अन्तर्भवति(संरचना uint32_tbl *tbl, uint32_t कुञ्चिका);

/* Get the first entry in the table (with the smallest `key`).
 * Returns FALSE if the table is empty. */
bool Curl_uint32_tbl_first(संरचना uint32_tbl *tbl,
                           uint32_t *pkey, शून्य **pentry);

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
bool Curl_uint32_tbl_अग्र(संरचना uint32_tbl *tbl, uint32_t चरमः_कुञ्चिका,
                          uint32_t *pkey, शून्य **pentry);

#यदि_अन्तः /* HEADER_CURL_UINT_TABLE_H */
