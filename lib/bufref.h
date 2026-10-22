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
ساختار bufref {
  تهی (*dtor)(تهی *);         /* Associated destructor. */
  ثابت unsigned نویسه *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ifdef DEBUGBUILD
  صحیح signature;                /* Detect API use mistakes. */
#endif
};

تهی Curl_bufref_init(ساختار bufref *br);
تهی Curl_bufref_set(ساختار bufref *br, ثابت تهی *ptr, size_t len,
                     تهی (*dtor)(تهی *));
ثابت نویسه *Curl_bufref_ptr(ثابت ساختار bufref *br);
ثابت unsigned نویسه *Curl_bufref_uptr(ثابت ساختار bufref *br);
size_t Curl_bufref_len(ثابت ساختار bufref *br);
CURLcode Curl_bufref_memdup0(ساختار bufref *br, ثابت تهی *ptr, size_t len);
تهی Curl_bufref_free(ساختار bufref *br);

/* return a strdup() version of the buffer */
#define Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#endif
