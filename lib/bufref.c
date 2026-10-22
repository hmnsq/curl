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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "curlx/strdup.h"

#ifdef DEBUGBUILD
#define SIGNATURE 0x5c48e9b2    /* Random pattern. */
#endif

/*
 * Init a bufref struct.
 */
शून्य Curl_bufref_init(संरचना bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = रिक्त;
  br->ptr = रिक्त;
  br->len = 0;

#ifdef DEBUGBUILD
  br->signature = SIGNATURE;
#endif
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

शून्य Curl_bufref_free(संरचना bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  यदि(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = रिक्त;
  br->ptr = रिक्त;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
शून्य Curl_bufref_set(संरचना bufref *br, ध्रुव शून्य *ptr, माप_प्रकार len,
                     शून्य (*dtor)(शून्य *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_निवेश_LENGTH);

  Curl_bufref_free(br);
  br->ptr = (ध्रुव अचिन्हित वर्ण *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
ध्रुव अचिन्हित वर्ण *Curl_bufref_uptr(ध्रुव संरचना bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  प्रत्यागम br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
ध्रुव वर्ण *Curl_bufref_ptr(ध्रुव संरचना bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  प्रत्यागम (ध्रुव वर्ण *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
माप_प्रकार Curl_bufref_len(ध्रुव संरचना bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  प्रत्यागम br->len;
}

CURLcode Curl_bufref_memdup0(संरचना bufref *br, ध्रुव शून्य *ptr, माप_प्रकार len)
{
  अचिन्हित वर्ण *cpy = रिक्त;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_निवेश_LENGTH);

  यदि(ptr) {
    cpy = curlx_memdup0(ptr, len);
    यदि(!cpy)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  Curl_bufref_set(br, cpy, len, curl_free);
  प्रत्यागम CURLE_OK;
}
