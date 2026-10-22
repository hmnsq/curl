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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"

#ifdef DEBUGBUILD
#define SIGNATURE 0x5c48e9b2    /* Random pattern. */
#endif

/*
 * Init a bufref struct.
 */
ଶୂନ୍ୟ Curl_bufref_init(ସଂରଚନା bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = ଖାଲି;
  br->ptr = ଖାଲି;
  br->len = 0;

#ifdef DEBUGBUILD
  br->signature = SIGNATURE;
#endif
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

ଶୂନ୍ୟ Curl_bufref_free(ସଂରଚନା bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ଯଦି(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = ଖାଲି;
  br->ptr = ଖାଲି;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
ଶୂନ୍ୟ Curl_bufref_set(ସଂରଚନା bufref *br, ଅଚଳ ଶୂନ୍ୟ *ptr, size_t len,
                     ଶୂନ୍ୟ (*dtor)(ଶୂନ୍ୟ *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ନିବେଶ_LENGTH);

  Curl_bufref_free(br);
  br->ptr = (ଅଚଳ unsigned ଅକ୍ଷର *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
ଅଚଳ unsigned ଅକ୍ଷର *Curl_bufref_uptr(ଅଚଳ ସଂରଚନା bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ଫେରନ୍ତୁ br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
ଅଚଳ ଅକ୍ଷର *Curl_bufref_ptr(ଅଚଳ ସଂରଚନା bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ଫେରନ୍ତୁ (ଅଚଳ ଅକ୍ଷର *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(ଅଚଳ ସଂରଚନା bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ଫେରନ୍ତୁ br->len;
}

CURLcode Curl_bufref_memdup0(ସଂରଚନା bufref *br, ଅଚଳ ଶୂନ୍ୟ *ptr, size_t len)
{
  unsigned ଅକ୍ଷର *cpy = ଖାଲି;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ନିବେଶ_LENGTH);

  ଯଦି(ptr) {
    cpy = curlx_memdup0(ptr, len);
    ଯଦି(!cpy)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  Curl_bufref_set(br, cpy, len, curl_free);
  ଫେରନ୍ତୁ CURLE_OK;
}
