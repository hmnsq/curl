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

#اگر_تعریف_شدہ DEBUGBUILD
#تعریف_کرو SIGNATURE 0x5c48e9b2    /* Random pattern. */
#اگر_ختم

/*
 * Init a bufref struct.
 */
خالی Curl_bufref_init(ساخت bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = تہی;
  br->ptr = تہی;
  br->len = 0;

#اگر_تعریف_شدہ DEBUGBUILD
  br->signature = SIGNATURE;
#اگر_ختم
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

خالی Curl_bufref_free(ساخت bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  اگر(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = تہی;
  br->ptr = تہی;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
خالی Curl_bufref_سیٹ(ساخت bufref *br, ثابت خالی *ptr, size_t len,
                     خالی (*dtor)(خالی *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_داخلہ_لمبائی);

  Curl_bufref_free(br);
  br->ptr = (ثابت غیرعلامتی حرف *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
ثابت غیرعلامتی حرف *Curl_bufref_uptr(ثابت ساخت bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  واپس br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
ثابت حرف *Curl_bufref_ptr(ثابت ساخت bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  واپس (ثابت حرف *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(ثابت ساخت bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  واپس br->len;
}

CURLcode Curl_bufref_memdup0(ساخت bufref *br, ثابت خالی *ptr, size_t len)
{
  غیرعلامتی حرف *cpy = تہی;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_داخلہ_لمبائی);

  اگر(ptr) {
    cpy = curlx_memdup0(ptr, len);
    اگر(!cpy)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  Curl_bufref_سیٹ(br, cpy, len, curl_free);
  واپس CURLE_OK;
}
