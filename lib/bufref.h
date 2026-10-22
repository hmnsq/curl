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
கட்டமைப்பு bufref {
  வெறுமை (*dtor)(வெறுமை *);         /* Associated destructor. */
  மாறிலி unsigned எழுத்து *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ifdef DEBUGBUILD
  முழு signature;                /* Detect API use mistakes. */
#endif
};

வெறுமை Curl_bufref_init(கட்டமைப்பு bufref *br);
வெறுமை Curl_bufref_set(கட்டமைப்பு bufref *br, மாறிலி வெறுமை *ptr, size_t len,
                     வெறுமை (*dtor)(வெறுமை *));
மாறிலி எழுத்து *Curl_bufref_ptr(மாறிலி கட்டமைப்பு bufref *br);
மாறிலி unsigned எழுத்து *Curl_bufref_uptr(மாறிலி கட்டமைப்பு bufref *br);
size_t Curl_bufref_len(மாறிலி கட்டமைப்பு bufref *br);
CURLcode Curl_bufref_memdup0(கட்டமைப்பு bufref *br, மாறிலி வெறுமை *ptr, size_t len);
வெறுமை Curl_bufref_free(கட்டமைப்பு bufref *br);

/* return a strdup() version of the buffer */
#define Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#endif
