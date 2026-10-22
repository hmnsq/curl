#اگر_غیر_تعریف_شدہ سرخی_CURL_BUFREF_H
#تعریف_کرو سرخی_CURL_BUFREF_H
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
ساخت bufref {
  خالی (*dtor)(خالی *);         /* Associated destructor. */
  ثابت غیرعلامتی حرف *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح signature;                /* Detect API use mistakes. */
#اگر_ختم
};

خالی Curl_bufref_init(ساخت bufref *br);
خالی Curl_bufref_سیٹ(ساخت bufref *br, ثابت خالی *ptr, size_t len,
                     خالی (*dtor)(خالی *));
ثابت حرف *Curl_bufref_ptr(ثابت ساخت bufref *br);
ثابت غیرعلامتی حرف *Curl_bufref_uptr(ثابت ساخت bufref *br);
size_t Curl_bufref_len(ثابت ساخت bufref *br);
CURLcode Curl_bufref_memdup0(ساخت bufref *br, ثابت خالی *ptr, size_t len);
خالی Curl_bufref_free(ساخت bufref *br);

/* return a strdup() version of the buffer */
#تعریف_کرو Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#اگر_ختم
