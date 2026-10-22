#నిర్వచితం_కాకపోతే శీర్షిక_CURL_UINT_పట్టిక_H
#నిర్వచించు శీర్షిక_CURL_UINT_పట్టిక_H
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

నిర్మాణం uint32_tbl {
  శూన్య **అడ్డువరుసలు;  /* array of void* holding entries */
  uint32_t nrows;  /* length of `rows` array */
  uint32_t nentries; /* entries in table */
  uint32_t చివరి_కీలకం_added; /* UINT_MAX or last key added */
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం init;
#అయితే_ముగింపు
};

/* Initialize the table with 0 capacity. */
శూన్య Curl_uint32_tbl_init(నిర్మాణం uint32_tbl *tbl);

/* Resize the table to change capacity `nmax`. When `nmax` is reduced,
 * all present entries with key equal or larger to `nmax` are removed. */
CURLcode Curl_uint32_tbl_resize(నిర్మాణం uint32_tbl *tbl, uint32_t nrows);

/* Destroy the table, freeing its storage. */
శూన్య Curl_uint32_tbl_destroy(నిర్మాణం uint32_tbl *tbl);

/* Get the table capacity. */
uint32_t Curl_uint32_tbl_సామర్థ్యం(నిర్మాణం uint32_tbl *tbl);

/* Get the number of entries in the table. */
uint32_t Curl_uint32_tbl_లెక్క(నిర్మాణం uint32_tbl *tbl);

/* Get the entry for key or NULL if not present */
శూన్య *Curl_uint32_tbl_get(నిర్మాణం uint32_tbl *tbl, uint32_t కీలకం);

/* Add a new entry to the table and assign it a free key.
 * Returns FALSE if the table is full.
 *
 * Keys are assigned in a round-robin manner.
 * No matter the capacity, UINT_MAX is never assigned. */
bool Curl_uint32_tbl_కూడు(నిర్మాణం uint32_tbl *tbl, శూన్య *నమోదు_2, uint32_t *pkey);

/* Remove the entry with `key`. */
శూన్య Curl_uint32_tbl_remove(నిర్మాణం uint32_tbl *tbl, uint32_t కీలకం);

/* Return TRUE if the table contains an tryn with that keys. */
bool Curl_uint32_tbl_కలిగియుంది(నిర్మాణం uint32_tbl *tbl, uint32_t కీలకం);

/* Get the first entry in the table (with the smallest `key`).
 * Returns FALSE if the table is empty. */
bool Curl_uint32_tbl_మొదటి(నిర్మాణం uint32_tbl *tbl,
                           uint32_t *pkey, శూన్య **pentry);

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
bool Curl_uint32_tbl_తదుపరి(నిర్మాణం uint32_tbl *tbl, uint32_t చివరి_కీలకం,
                          uint32_t *pkey, శూన్య **pentry);

#అయితే_ముగింపు /* HEADER_CURL_UINT_TABLE_H */
