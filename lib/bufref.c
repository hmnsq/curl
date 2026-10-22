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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "bufref.h"
#ضمن "curlx/strdup.h"

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
#وصف_ڪريو SIGNATURE 0x5c48e9b2    /* Random pattern. */
#جيڪڏهن_پورو

/*
 * Init a bufref struct.
 */
فراغ Curl_bufref_init(بنية bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = عدم;
  br->ptr = عدم;
  br->len = 0;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  br->signature = SIGNATURE;
#جيڪڏهن_پورو
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

فراغ Curl_bufref_free(بنية bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  إذا(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = عدم;
  br->ptr = عدم;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
فراغ Curl_bufref_سيٽ(بنية bufref *br, ثابت فراغ *ptr, size_t len,
                     فراغ (*dtor)(فراغ *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_داخلا_ڊيگهائي);

  Curl_bufref_free(br);
  br->ptr = (ثابت بنانشان حرف *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
ثابت بنانشان حرف *Curl_bufref_uptr(ثابت بنية bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  أرجع br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
ثابت حرف *Curl_bufref_ptr(ثابت بنية bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  أرجع (ثابت حرف *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(ثابت بنية bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  أرجع br->len;
}

CURLcode Curl_bufref_memdup0(بنية bufref *br, ثابت فراغ *ptr, size_t len)
{
  بنانشان حرف *cpy = عدم;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_داخلا_ڊيگهائي);

  إذا(ptr) {
    cpy = curlx_memdup0(ptr, len);
    إذا(!cpy)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  Curl_bufref_سيٽ(br, cpy, len, curl_free);
  أرجع CURLE_OK;
}
