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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "bufref.h"
#شامل "curlx/strdup.h"

#ifdef DEBUGBUILD
#define SIGNATURE 0x5c48e9b2    /* Random pattern. */
#endif

/*
 * Init a bufref struct.
 */
تهی Curl_bufref_init(ساختار bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = پوچ;
  br->ptr = پوچ;
  br->len = 0;

#ifdef DEBUGBUILD
  br->signature = SIGNATURE;
#endif
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

تهی Curl_bufref_free(ساختار bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  اگر(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = پوچ;
  br->ptr = پوچ;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
تهی Curl_bufref_set(ساختار bufref *br, ثابت تهی *ptr, size_t len,
                     تهی (*dtor)(تهی *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_درون_داد_LENGTH);

  Curl_bufref_free(br);
  br->ptr = (ثابت unsigned نویسه *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
ثابت unsigned نویسه *Curl_bufref_uptr(ثابت ساختار bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  بازگشت br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
ثابت نویسه *Curl_bufref_ptr(ثابت ساختار bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  بازگشت (ثابت نویسه *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(ثابت ساختار bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  بازگشت br->len;
}

CURLcode Curl_bufref_memdup0(ساختار bufref *br, ثابت تهی *ptr, size_t len)
{
  unsigned نویسه *cpy = پوچ;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_درون_داد_LENGTH);

  اگر(ptr) {
    cpy = curlx_memdup0(ptr, len);
    اگر(!cpy)
      بازگشت CURLE_OUT_OF_MEMORY;
  }

  Curl_bufref_set(br, cpy, len, curl_free);
  بازگشت CURLE_OK;
}
