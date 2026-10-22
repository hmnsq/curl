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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIGNATURE 0x5c48e9b2    /* Random pattern. */
#ਜੇ_ਅੰਤ

/*
 * Init a bufref struct.
 */
ਖਾਲੀ Curl_bufref_init(ਬਣਤਰ bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = ਨਿਰਾ;
  br->ptr = ਨਿਰਾ;
  br->len = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  br->signature = SIGNATURE;
#ਜੇ_ਅੰਤ
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

ਖਾਲੀ Curl_bufref_free(ਬਣਤਰ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ਜੇ(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = ਨਿਰਾ;
  br->ptr = ਨਿਰਾ;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
ਖਾਲੀ Curl_bufref_ਸੈੱਟ(ਬਣਤਰ bufref *br, ਅਚਲ ਖਾਲੀ *ptr, size_t len,
                     ਖਾਲੀ (*dtor)(ਖਾਲੀ *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

  Curl_bufref_free(br);
  br->ptr = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *Curl_bufref_uptr(ਅਚਲ ਬਣਤਰ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ਵਾਪਸ br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
ਅਚਲ ਅੱਖਰ *Curl_bufref_ptr(ਅਚਲ ਬਣਤਰ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ਵਾਪਸ (ਅਚਲ ਅੱਖਰ *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(ਅਚਲ ਬਣਤਰ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ਵਾਪਸ br->len;
}

CURLcode Curl_bufref_memdup0(ਬਣਤਰ bufref *br, ਅਚਲ ਖਾਲੀ *ptr, size_t len)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *cpy = ਨਿਰਾ;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

  ਜੇ(ptr) {
    cpy = curlx_memdup0(ptr, len);
    ਜੇ(!cpy)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  Curl_bufref_ਸੈੱਟ(br, cpy, len, curl_free);
  ਵਾਪਸ CURLE_OK;
}
