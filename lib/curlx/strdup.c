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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ <wchar.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
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
wchar_t *curlx_wcsdup(ਅਚਲ wchar_t *src)
{
  size_t ਲੰਮਾਈ = wcslen(src);

  ਜੇ(ਲੰਮਾਈ > (SIZE_MAX / ਆਕਾਰ(wchar_t)) - 1)
    ਵਾਪਸ (wchar_t *)ਨਿਰਾ; /* integer overflow */

  ਵਾਪਸ (wchar_t *)curlx_memdup(src, (ਲੰਮਾਈ + 1) * ਆਕਾਰ(wchar_t));
}
#ਜੇ_ਅੰਤ

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
ਖਾਲੀ *curlx_memdup(ਅਚਲ ਖਾਲੀ *src, size_t ਲੰਮਾਈ)
{
  ਖਾਲੀ *ਬਫਰ = curlx_malloc(ਲੰਮਾਈ);
  ਜੇ(!ਬਫਰ)
    ਵਾਪਸ ਨਿਰਾ; /* fail */

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਬਫਰ, src, ਲੰਮਾਈ);

  ਵਾਪਸ ਬਫਰ;
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
ਖਾਲੀ *curlx_memdup0(ਅਚਲ ਅੱਖਰ *src, size_t ਲੰਮਾਈ)
{
  ਅੱਖਰ *buf = (ਲੰਮਾਈ < SIZE_MAX) ? curlx_malloc(ਲੰਮਾਈ + 1) : ਨਿਰਾ;
  ਜੇ(!buf)
    ਵਾਪਸ ਨਿਰਾ;
  ਜੇ(ਲੰਮਾਈ) {
    DEBUGASSERT(src); /* must never be NULL */
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, src, ਲੰਮਾਈ);
  }
  buf[ਲੰਮਾਈ] = 0;
  ਵਾਪਸ buf;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CURLX_MEMZERO
ਸਥਿਰ ਖਾਲੀ *(* ਅਚਲ ਅਸਥਿਰ p_curlx_memset)(ਖਾਲੀ *buf, ਪੂਰਨ val,
                                               size_t ਮਾਪ) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
ਖਾਲੀ curlx_memzero_ਨੀਵਾਂ(ਖਾਲੀ *buf, size_t ਮਾਪ)
{
  ਜੇ(buf)
    p_curlx_memset(buf, 0, ਮਾਪ);
}
#ਜੇ_ਅੰਤ

/* Fill 'buf' with zeroes. */
ਖਾਲੀ curlx_memzero(ਖਾਲੀ *buf, size_t ਮਾਪ)
{
  ਜੇ(buf)
    curlx_memzero_ਨੀਵਾਂ(buf, ਮਾਪ);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
ਖਾਲੀ curlx_strzero(ਖਾਲੀ *buf)
{
  ਜੇ(buf)
    curlx_memzero_ਨੀਵਾਂ(buf, ਲੜੀ_ਲੰਬਾਈ(buf));
}
