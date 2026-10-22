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
ସଂରଚନା bufref {
  ଶୂନ୍ୟ (*dtor)(ଶୂନ୍ୟ *);         /* Associated destructor. */
  ଅଚଳ unsigned ଅକ୍ଷର *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ifdef DEBUGBUILD
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା signature;                /* Detect API use mistakes. */
#endif
};

ଶୂନ୍ୟ Curl_bufref_init(ସଂରଚନା bufref *br);
ଶୂନ୍ୟ Curl_bufref_set(ସଂରଚନା bufref *br, ଅଚଳ ଶୂନ୍ୟ *ptr, size_t len,
                     ଶୂନ୍ୟ (*dtor)(ଶୂନ୍ୟ *));
ଅଚଳ ଅକ୍ଷର *Curl_bufref_ptr(ଅଚଳ ସଂରଚନା bufref *br);
ଅଚଳ unsigned ଅକ୍ଷର *Curl_bufref_uptr(ଅଚଳ ସଂରଚନା bufref *br);
size_t Curl_bufref_len(ଅଚଳ ସଂରଚନା bufref *br);
CURLcode Curl_bufref_memdup0(ସଂରଚନା bufref *br, ଅଚଳ ଶୂନ୍ୟ *ptr, size_t len);
ଶୂନ୍ୟ Curl_bufref_free(ସଂରଚନା bufref *br);

/* return a strdup() version of the buffer */
#define Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#endif
