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
સંરચના bufref {
  શૂન્ય (*dtor)(શૂન્ય *);         /* Associated destructor. */
  અચલ unsigned અક્ષર *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ifdef DEBUGBUILD
  પૂર્ણાંક signature;                /* Detect API use mistakes. */
#endif
};

શૂન્ય Curl_bufref_init(સંરચના bufref *br);
શૂન્ય Curl_bufref_set(સંરચના bufref *br, અચલ શૂન્ય *ptr, size_t len,
                     શૂન્ય (*dtor)(શૂન્ય *));
અચલ અક્ષર *Curl_bufref_ptr(અચલ સંરચના bufref *br);
અચલ unsigned અક્ષર *Curl_bufref_uptr(અચલ સંરચના bufref *br);
size_t Curl_bufref_len(અચલ સંરચના bufref *br);
CURLcode Curl_bufref_memdup0(સંરચના bufref *br, અચલ શૂન્ય *ptr, size_t len);
શૂન્ય Curl_bufref_free(સંરચના bufref *br);

/* return a strdup() version of the buffer */
#define Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#endif
