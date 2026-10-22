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
সংরচনা bufref {
  শূন্য (*dtor)(শূন্য *);         /* Associated destructor. */
  ধ্রুৱ unsigned ৱর্ণ *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ifdef DEBUGBUILD
  পূর্ণাঙ্ক signature;                /* Detect API use mistakes. */
#endif
};

শূন্য Curl_bufref_init(সংরচনা bufref *br);
শূন্য Curl_bufref_set(সংরচনা bufref *br, ধ্রুৱ শূন্য *ptr, size_t len,
                     শূন্য (*dtor)(শূন্য *));
ধ্রুৱ ৱর্ণ *Curl_bufref_ptr(ধ্রুৱ সংরচনা bufref *br);
ধ্রুৱ unsigned ৱর্ণ *Curl_bufref_uptr(ধ্রুৱ সংরচনা bufref *br);
size_t Curl_bufref_len(ধ্রুৱ সংরচনা bufref *br);
CURLcode Curl_bufref_memdup0(সংরচনা bufref *br, ধ্রুৱ শূন্য *ptr, size_t len);
শূন্য Curl_bufref_free(সংরচনা bufref *br);

/* return a strdup() version of the buffer */
#define Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#endif
