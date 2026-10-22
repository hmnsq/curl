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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curlx/strdup.h"

#ifdef DEBUGBUILD
#define SIGNATURE 0x5c48e9b2    /* Random pattern. */
#endif

/*
 * Init a bufref struct.
 */
வெறுமை Curl_bufref_init(கட்டமைப்பு bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = வெற்று;
  br->ptr = வெற்று;
  br->len = 0;

#ifdef DEBUGBUILD
  br->signature = SIGNATURE;
#endif
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

வெறுமை Curl_bufref_free(கட்டமைப்பு bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  எனில்(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = வெற்று;
  br->ptr = வெற்று;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
வெறுமை Curl_bufref_set(கட்டமைப்பு bufref *br, மாறிலி வெறுமை *ptr, size_t len,
                     வெறுமை (*dtor)(வெறுமை *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_உள்ளீடு_LENGTH);

  Curl_bufref_free(br);
  br->ptr = (மாறிலி unsigned எழுத்து *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
மாறிலி unsigned எழுத்து *Curl_bufref_uptr(மாறிலி கட்டமைப்பு bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  திரும்பு br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
மாறிலி எழுத்து *Curl_bufref_ptr(மாறிலி கட்டமைப்பு bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  திரும்பு (மாறிலி எழுத்து *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(மாறிலி கட்டமைப்பு bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  திரும்பு br->len;
}

CURLcode Curl_bufref_memdup0(கட்டமைப்பு bufref *br, மாறிலி வெறுமை *ptr, size_t len)
{
  unsigned எழுத்து *cpy = வெற்று;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_உள்ளீடு_LENGTH);

  எனில்(ptr) {
    cpy = curlx_memdup0(ptr, len);
    எனில்(!cpy)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  Curl_bufref_set(br, cpy, len, curl_free);
  திரும்பு CURLE_OK;
}
