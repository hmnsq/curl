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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
#ವ್ಯಾಖ್ಯಾನಿಸು SIGNATURE 0x5c48e9b2    /* Random pattern. */
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Init a bufref struct.
 */
ಶೂನ್ಯ Curl_bufref_init(ರಚನೆ bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = ಖಾಲಿ;
  br->ptr = ಖಾಲಿ;
  br->len = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  br->signature = SIGNATURE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

ಶೂನ್ಯ Curl_bufref_free(ರಚನೆ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ಆದರೆ(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = ಖಾಲಿ;
  br->ptr = ಖಾಲಿ;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
ಶೂನ್ಯ Curl_bufref_ಗಣ(ರಚನೆ bufref *br, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ptr, size_t len,
                     ಶೂನ್ಯ (*dtor)(ಶೂನ್ಯ *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

  Curl_bufref_free(br);
  br->ptr = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *Curl_bufref_uptr(ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ಹಿಂತಿರುಗಿ br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_bufref_ptr(ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ಹಿಂತಿರುಗಿ br->len;
}

CURLcode Curl_bufref_memdup0(ರಚನೆ bufref *br, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ptr, size_t len)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *cpy = ಖಾಲಿ;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

  ಆದರೆ(ptr) {
    cpy = curlx_memdup0(ptr, len);
    ಆದರೆ(!cpy)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  Curl_bufref_ಗಣ(br, cpy, len, curl_free);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
