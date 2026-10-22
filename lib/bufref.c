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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curlx/strdup.h"

#ifdef DEBUGBUILD
#define SIGNATURE 0x5c48e9b2    /* Random pattern. */
#endif

/*
 * Init a bufref struct.
 */
ശൂന്യം Curl_bufref_init(ഘടന bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = ശൂന്യ;
  br->ptr = ശൂന്യ;
  br->len = 0;

#ifdef DEBUGBUILD
  br->signature = SIGNATURE;
#endif
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

ശൂന്യം Curl_bufref_free(ഘടന bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  എങ്കിൽ(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = ശൂന്യ;
  br->ptr = ശൂന്യ;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
ശൂന്യം Curl_bufref_set(ഘടന bufref *br, സ്ഥിരം ശൂന്യം *ptr, size_t len,
                     ശൂന്യം (*dtor)(ശൂന്യം *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_നിവേശം_LENGTH);

  Curl_bufref_free(br);
  br->ptr = (സ്ഥിരം unsigned അക്ഷരം *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
സ്ഥിരം unsigned അക്ഷരം *Curl_bufref_uptr(സ്ഥിരം ഘടന bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  തിരികെ br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
സ്ഥിരം അക്ഷരം *Curl_bufref_ptr(സ്ഥിരം ഘടന bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  തിരികെ (സ്ഥിരം അക്ഷരം *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(സ്ഥിരം ഘടന bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  തിരികെ br->len;
}

CURLcode Curl_bufref_memdup0(ഘടന bufref *br, സ്ഥിരം ശൂന്യം *ptr, size_t len)
{
  unsigned അക്ഷരം *cpy = ശൂന്യ;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_നിവേശം_LENGTH);

  എങ്കിൽ(ptr) {
    cpy = curlx_memdup0(ptr, len);
    എങ്കിൽ(!cpy)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  Curl_bufref_set(br, cpy, len, curl_free);
  തിരികെ CURLE_OK;
}
