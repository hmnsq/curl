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

#ifdef _WIN32
#அந்தர்பாவ <wchar.h>
#endif

#அந்தர்பாவ "curlx/strdup.h"

#ifdef _WIN32
/***************************************************************************
 *
 * curlx_wcsdup(source)
 *
 * Copies the 'source' wchar string to a newly allocated buffer (that is
 * returned). Used by macro curlx_tcsdup().
 *
 * Returns the new pointer or NULL on failure.
 *
 ***************************************************************************/
wchar_t *curlx_wcsdup(மாறிலி wchar_t *src)
{
  size_t length = wcslen(src);

  எனில்(length > (SIZE_MAX / அளவு(wchar_t)) - 1)
    திரும்பு (wchar_t *)வெற்று; /* integer overflow */

  திரும்பு (wchar_t *)curlx_memdup(src, (length + 1) * அளவு(wchar_t));
}
#endif

/***************************************************************************
 *
 * curlx_memdup(source, length)
 *
 * Copies the 'source' data to a newly allocated buffer (that is
 * returned). Copies 'length' bytes.
 *
 * Returns the new pointer or NULL on failure.
 *
 ***************************************************************************/
வெறுமை *curlx_memdup(மாறிலி வெறுமை *src, size_t length)
{
  வெறுமை *buffer = curlx_malloc(length);
  எனில்(!buffer)
    திரும்பு வெற்று; /* fail */

  memcpy(buffer, src, length);

  திரும்பு buffer;
}

/***************************************************************************
 *
 * curlx_memdup0(source, length)
 *
 * Copies the 'source' string to a newly allocated buffer (that is returned).
 * Copies 'length' bytes then adds a null-terminator.
 *
 * Returns the new pointer or NULL on failure.
 *
 ***************************************************************************/
வெறுமை *curlx_memdup0(மாறிலி எழுத்து *src, size_t length)
{
  எழுத்து *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : வெற்று;
  எனில்(!buf)
    திரும்பு வெற்று;
  எனில்(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  திரும்பு buf;
}

#ifdef USE_CURLX_MEMZERO
நிலையான வெறுமை *(* மாறிலி நிலையற்ற p_curlx_memset)(வெறுமை *buf, முழு val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
வெறுமை curlx_memzero_கீழ்(வெறுமை *buf, size_t size)
{
  எனில்(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
வெறுமை curlx_memzero(வெறுமை *buf, size_t size)
{
  எனில்(buf)
    curlx_memzero_கீழ்(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
வெறுமை curlx_strzero(வெறுமை *buf)
{
  எனில்(buf)
    curlx_memzero_கீழ்(buf, strlen(buf));
}
