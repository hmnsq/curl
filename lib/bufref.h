#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_BUFREF_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_BUFREF_H
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
ਬਣਤਰ bufref {
  ਖਾਲੀ (*dtor)(ਖਾਲੀ *);         /* Associated destructor. */
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ signature;                /* Detect API use mistakes. */
#ਜੇ_ਅੰਤ
};

ਖਾਲੀ Curl_bufref_init(ਬਣਤਰ bufref *br);
ਖਾਲੀ Curl_bufref_ਸੈੱਟ(ਬਣਤਰ bufref *br, ਅਚਲ ਖਾਲੀ *ptr, size_t len,
                     ਖਾਲੀ (*dtor)(ਖਾਲੀ *));
ਅਚਲ ਅੱਖਰ *Curl_bufref_ptr(ਅਚਲ ਬਣਤਰ bufref *br);
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *Curl_bufref_uptr(ਅਚਲ ਬਣਤਰ bufref *br);
size_t Curl_bufref_len(ਅਚਲ ਬਣਤਰ bufref *br);
CURLcode Curl_bufref_memdup0(ਬਣਤਰ bufref *br, ਅਚਲ ਖਾਲੀ *ptr, size_t len);
ਖਾਲੀ Curl_bufref_free(ਬਣਤਰ bufref *br);

/* return a strdup() version of the buffer */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#ਜੇ_ਅੰਤ
