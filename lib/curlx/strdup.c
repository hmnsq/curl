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

#ifdef _WIN32
#ಅನ್ತರ್ಭಾವ <wchar.h>
#endif

#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"

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
wchar_t *curlx_wcsdup(ಧ್ರುವ wchar_t *src)
{
  size_t length = wcslen(src);

  ಯದಿ(length > (SIZE_MAX / ಪರಿಮಾಣ(wchar_t)) - 1)
    ಪ್ರತ್ಯಾಗಮ (wchar_t *)ರಿಕ್ತ; /* integer overflow */

  ಪ್ರತ್ಯಾಗಮ (wchar_t *)curlx_memdup(src, (length + 1) * ಪರಿಮಾಣ(wchar_t));
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
ಶೂನ್ಯ *curlx_memdup(ಧ್ರುವ ಶೂನ್ಯ *src, size_t length)
{
  ಶೂನ್ಯ *buffer = curlx_malloc(length);
  ಯದಿ(!buffer)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ; /* fail */

  memcpy(buffer, src, length);

  ಪ್ರತ್ಯಾಗಮ buffer;
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
ಶೂನ್ಯ *curlx_memdup0(ಧ್ರುವ ವರ್ಣ *src, size_t length)
{
  ವರ್ಣ *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : ರಿಕ್ತ;
  ಯದಿ(!buf)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಯದಿ(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  ಪ್ರತ್ಯಾಗಮ buf;
}

#ifdef USE_CURLX_MEMZERO
ಸ್ಥಿರ ಶೂನ್ಯ *(* ಧ್ರುವ ಚಞ್ಚಲ p_curlx_memset)(ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಙ್ಕ val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
ಶೂನ್ಯ curlx_memzero_ಕೆಳಗಿನ(ಶೂನ್ಯ *buf, size_t size)
{
  ಯದಿ(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
ಶೂನ್ಯ curlx_memzero(ಶೂನ್ಯ *buf, size_t size)
{
  ಯದಿ(buf)
    curlx_memzero_ಕೆಳಗಿನ(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
ಶೂನ್ಯ curlx_strzero(ಶೂನ್ಯ *buf)
{
  ಯದಿ(buf)
    curlx_memzero_ಕೆಳಗಿನ(buf, strlen(buf));
}
