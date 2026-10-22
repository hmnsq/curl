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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"

#ifdef DEBUGBUILD
#define SIGNATURE 0x5c48e9b2    /* Random pattern. */
#endif

/*
 * Init a bufref struct.
 */
ಶೂನ್ಯ Curl_bufref_init(ಸಂರಚನಾ bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = ರಿಕ್ತ;
  br->ptr = ರಿಕ್ತ;
  br->len = 0;

#ifdef DEBUGBUILD
  br->signature = SIGNATURE;
#endif
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

ಶೂನ್ಯ Curl_bufref_free(ಸಂರಚನಾ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ಯದಿ(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = ರಿಕ್ತ;
  br->ptr = ರಿಕ್ತ;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
ಶೂನ್ಯ Curl_bufref_set(ಸಂರಚನಾ bufref *br, ಧ್ರುವ ಶೂನ್ಯ *ptr, size_t len,
                     ಶೂನ್ಯ (*dtor)(ಶೂನ್ಯ *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ಒಳಹರಿವು_LENGTH);

  Curl_bufref_free(br);
  br->ptr = (ಧ್ರುವ unsigned ವರ್ಣ *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
ಧ್ರುವ unsigned ವರ್ಣ *Curl_bufref_uptr(ಧ್ರುವ ಸಂರಚನಾ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ಪ್ರತ್ಯಾಗಮ br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
ಧ್ರುವ ವರ್ಣ *Curl_bufref_ptr(ಧ್ರುವ ಸಂರಚನಾ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ ವರ್ಣ *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(ಧ್ರುವ ಸಂರಚನಾ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ಪ್ರತ್ಯಾಗಮ br->len;
}

CURLcode Curl_bufref_memdup0(ಸಂರಚನಾ bufref *br, ಧ್ರುವ ಶೂನ್ಯ *ptr, size_t len)
{
  unsigned ವರ್ಣ *cpy = ರಿಕ್ತ;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ಒಳಹರಿವು_LENGTH);

  ಯದಿ(ptr) {
    cpy = curlx_memdup0(ptr, len);
    ಯದಿ(!cpy)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  Curl_bufref_set(br, cpy, len, curl_free);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
