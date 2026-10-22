#ifndef HEADER_CURL_BUFREF_H
#define HEADER_CURL_BUFREF_H
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

/*
 * Generic buffer reference.
 */
sa_nracanaa bufref {
  void (*dtor)(void *);         /* Associated destructor. */
  const unsigned var_na *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ifdef DEBUGBUILD
  puur_naa_nka signature;                /* Detect API use mistakes. */
#endif
};

void Curl_bufref_init(sa_nracanaa bufref *br);
void Curl_bufref_set(sa_nracanaa bufref *br, const void *ptr, size_t len,
                     void (*dtor)(void *));
const var_na *Curl_bufref_ptr(const sa_nracanaa bufref *br);
const unsigned var_na *Curl_bufref_uptr(const sa_nracanaa bufref *br);
size_t Curl_bufref_len(const sa_nracanaa bufref *br);
CURLcode Curl_bufref_memdup0(sa_nracanaa bufref *br, const void *ptr, size_t len);
void Curl_bufref_free(sa_nracanaa bufref *br);

/* return a strdup() version of the buffer */
#define Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#endif
