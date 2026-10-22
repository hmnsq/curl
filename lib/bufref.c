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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "curlx/strdup.h"

#ifdef DEBUGBUILD
#define SIGNATURE 0x5c48e9b2    /* Random pattern. */
#endif

/*
 * Init a bufref struct.
 */
শূন্য Curl_bufref_init(সংরচনা bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = রিক্ত;
  br->ptr = রিক্ত;
  br->len = 0;

#ifdef DEBUGBUILD
  br->signature = SIGNATURE;
#endif
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

শূন্য Curl_bufref_free(সংরচনা bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  যদি(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = রিক্ত;
  br->ptr = রিক্ত;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
শূন্য Curl_bufref_set(সংরচনা bufref *br, ধ্রুব শূন্য *ptr, size_t len,
                     শূন্য (*dtor)(শূন্য *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_প্রবেশ_LENGTH);

  Curl_bufref_free(br);
  br->ptr = (ধ্রুব unsigned বর্ণ *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
ধ্রুব unsigned বর্ণ *Curl_bufref_uptr(ধ্রুব সংরচনা bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  প্রত্যাগম br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
ধ্রুব বর্ণ *Curl_bufref_ptr(ধ্রুব সংরচনা bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  প্রত্যাগম (ধ্রুব বর্ণ *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(ধ্রুব সংরচনা bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  প্রত্যাগম br->len;
}

CURLcode Curl_bufref_memdup0(সংরচনা bufref *br, ধ্রুব শূন্য *ptr, size_t len)
{
  unsigned বর্ণ *cpy = রিক্ত;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_প্রবেশ_LENGTH);

  যদি(ptr) {
    cpy = curlx_memdup0(ptr, len);
    যদি(!cpy)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  Curl_bufref_set(br, cpy, len, curl_free);
  প্রত্যাগম CURLE_OK;
}
