#अगर_अघोषित HEADER_CURL_BUFREF_H
#घोषणा HEADER_CURL_BUFREF_H
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
काष्ठा bufref {
  व्योम (*dtor)(व्योम *);         /* Associated destructor. */
  स्थिर अचिन्हित अक्षर *ptr;     /* Referenced data buffer. */
  माप_प्रकार len;                   /* The data size in bytes. */
#अगर_घोषित DEBUGBUILD
  पूर्णांक signature;                /* Detect API use mistakes. */
#पूर्ण_अगर
};

व्योम Curl_bufref_init(काष्ठा bufref *br);
व्योम Curl_bufref_set(काष्ठा bufref *br, स्थिर व्योम *ptr, माप_प्रकार len,
                     व्योम (*dtor)(व्योम *));
स्थिर अक्षर *Curl_bufref_ptr(स्थिर काष्ठा bufref *br);
स्थिर अचिन्हित अक्षर *Curl_bufref_uptr(स्थिर काष्ठा bufref *br);
माप_प्रकार Curl_bufref_len(स्थिर काष्ठा bufref *br);
CURLcode Curl_bufref_memdup0(काष्ठा bufref *br, स्थिर व्योम *ptr, माप_प्रकार len);
व्योम Curl_bufref_free(काष्ठा bufref *br);

/* return a strdup() version of the buffer */
#घोषणा Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#पूर्ण_अगर
