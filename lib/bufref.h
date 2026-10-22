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
संरचना bufref {
  शून्य (*dtor)(शून्य *);         /* Associated destructor. */
  अचर अचिन्हित वर्ण *ptr;     /* Referenced data buffer. */
  माप_प्रकार len;                   /* The data size in bytes. */
#ifdef DEBUGBUILD
  पूर्णाङ्क signature;                /* Detect API use mistakes. */
#endif
};

शून्य Curl_bufref_init(संरचना bufref *br);
शून्य Curl_bufref_set(संरचना bufref *br, अचर शून्य *ptr, माप_प्रकार len,
                     शून्य (*dtor)(शून्य *));
अचर वर्ण *Curl_bufref_ptr(अचर संरचना bufref *br);
अचर अचिन्हित वर्ण *Curl_bufref_uptr(अचर संरचना bufref *br);
माप_प्रकार Curl_bufref_len(अचर संरचना bufref *br);
CURLcode Curl_bufref_memdup0(संरचना bufref *br, अचर शून्य *ptr, माप_प्रकार len);
शून्य Curl_bufref_free(संरचना bufref *br);

/* return a strdup() version of the buffer */
#define Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#endif
