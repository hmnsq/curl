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
ಸಂರಚನಾ bufref {
  ಶೂನ್ಯ (*dtor)(ಶೂನ್ಯ *);         /* Associated destructor. */
  ಧ್ರುವ unsigned ವರ್ಣ *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ifdef DEBUGBUILD
  ಪೂರ್ಣಾಙ್ಕ signature;                /* Detect API use mistakes. */
#endif
};

ಶೂನ್ಯ Curl_bufref_init(ಸಂರಚನಾ bufref *br);
ಶೂನ್ಯ Curl_bufref_set(ಸಂರಚನಾ bufref *br, ಧ್ರುವ ಶೂನ್ಯ *ptr, size_t len,
                     ಶೂನ್ಯ (*dtor)(ಶೂನ್ಯ *));
ಧ್ರುವ ವರ್ಣ *Curl_bufref_ptr(ಧ್ರುವ ಸಂರಚನಾ bufref *br);
ಧ್ರುವ unsigned ವರ್ಣ *Curl_bufref_uptr(ಧ್ರುವ ಸಂರಚನಾ bufref *br);
size_t Curl_bufref_len(ಧ್ರುವ ಸಂರಚನಾ bufref *br);
CURLcode Curl_bufref_memdup0(ಸಂರಚನಾ bufref *br, ಧ್ರುವ ಶೂನ್ಯ *ptr, size_t len);
ಶೂನ್ಯ Curl_bufref_free(ಸಂರಚನಾ bufref *br);

/* return a strdup() version of the buffer */
#define Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#endif
