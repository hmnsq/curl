#यदि_अपरिभाषितम् शीर्षकम्_CURL_BUFREF_H
#परिभाषय शीर्षकम्_CURL_BUFREF_H
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
संरचना bufref {
  शून्य (*dtor)(शून्य *);         /* Associated destructor. */
  ध्रुव अचिह्नित वर्ण *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#यदि_परिभाषितम् DEBUGBUILD
  पूर्णाङ्क signature;                /* Detect API use mistakes. */
#यदि_अन्तः
};

शून्य Curl_bufref_init(संरचना bufref *br);
शून्य Curl_bufref_समुच्चयः(संरचना bufref *br, ध्रुव शून्य *ptr, size_t len,
                     शून्य (*dtor)(शून्य *));
ध्रुव वर्ण *Curl_bufref_ptr(ध्रुव संरचना bufref *br);
ध्रुव अचिह्नित वर्ण *Curl_bufref_uptr(ध्रुव संरचना bufref *br);
size_t Curl_bufref_len(ध्रुव संरचना bufref *br);
CURLcode Curl_bufref_memdup0(संरचना bufref *br, ध्रुव शून्य *ptr, size_t len);
शून्य Curl_bufref_free(संरचना bufref *br);

/* return a strdup() version of the buffer */
#परिभाषय Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#यदि_अन्तः
