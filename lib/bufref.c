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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "bufref.h"
#समावेश "curlx/strdup.h"

#अगर_घोषित DEBUGBUILD
#घोषणा SIGNATURE 0x5c48e9b2    /* Random pattern. */
#पूर्ण_अगर

/*
 * Init a bufref struct.
 */
व्योम Curl_bufref_init(काष्ठा bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = शून्य;
  br->ptr = शून्य;
  br->len = 0;

#अगर_घोषित DEBUGBUILD
  br->signature = SIGNATURE;
#पूर्ण_अगर
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

व्योम Curl_bufref_free(काष्ठा bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  अगर(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = शून्य;
  br->ptr = शून्य;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
व्योम Curl_bufref_set(काष्ठा bufref *br, स्थिर व्योम *ptr, माप_प्रकार len,
                     व्योम (*dtor)(व्योम *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_निवेश_लंबाई);

  Curl_bufref_free(br);
  br->ptr = (स्थिर अचिन्हित अक्षर *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
स्थिर अचिन्हित अक्षर *Curl_bufref_uptr(स्थिर काष्ठा bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  वापस br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
स्थिर अक्षर *Curl_bufref_ptr(स्थिर काष्ठा bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  वापस (स्थिर अक्षर *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
माप_प्रकार Curl_bufref_len(स्थिर काष्ठा bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  वापस br->len;
}

CURLcode Curl_bufref_memdup0(काष्ठा bufref *br, स्थिर व्योम *ptr, माप_प्रकार len)
{
  अचिन्हित अक्षर *cpy = शून्य;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_निवेश_लंबाई);

  अगर(ptr) {
    cpy = curlx_memdup0(ptr, len);
    अगर(!cpy)
      वापस CURLE_OUT_OF_MEMORY;
  }

  Curl_bufref_set(br, cpy, len, curl_free);
  वापस CURLE_OK;
}
