#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_BUFREF_H
#وصف_ڪريو سرو_CURL_BUFREF_H
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
بنية bufref {
  فراغ (*dtor)(فراغ *);         /* Associated destructor. */
  ثابت بنانشان حرف *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  صحيح signature;                /* Detect API use mistakes. */
#جيڪڏهن_پورو
};

فراغ Curl_bufref_init(بنية bufref *br);
فراغ Curl_bufref_سيٽ(بنية bufref *br, ثابت فراغ *ptr, size_t len,
                     فراغ (*dtor)(فراغ *));
ثابت حرف *Curl_bufref_ptr(ثابت بنية bufref *br);
ثابت بنانشان حرف *Curl_bufref_uptr(ثابت بنية bufref *br);
size_t Curl_bufref_len(ثابت بنية bufref *br);
CURLcode Curl_bufref_memdup0(بنية bufref *br, ثابت فراغ *ptr, size_t len);
فراغ Curl_bufref_free(بنية bufref *br);

/* return a strdup() version of the buffer */
#وصف_ڪريو Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#جيڪڏهن_پورو
