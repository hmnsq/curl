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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ಅನ್ತರ್ಭಾವ <wchar.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
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
wchar_t *curlx_wcsdup(ಸ್ಥಿರಾಂಕ wchar_t *src)
{
  size_t ಉದ್ದಳತೆ = wcslen(src);

  ಆದರೆ(ಉದ್ದಳತೆ > (SIZE_MAX / ಗಾತ್ರ(wchar_t)) - 1)
    ಹಿಂತಿರುಗಿ (wchar_t *)ಖಾಲಿ; /* integer overflow */

  ಹಿಂತಿರುಗಿ (wchar_t *)curlx_memdup(src, (ಉದ್ದಳತೆ + 1) * ಗಾತ್ರ(wchar_t));
}
#ಆದರೆ_ಅಂತ್ಯ

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
ಶೂನ್ಯ *curlx_memdup(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, size_t ಉದ್ದಳತೆ)
{
  ಶೂನ್ಯ *ಬಫರ್ = curlx_malloc(ಉದ್ದಳತೆ);
  ಆದರೆ(!ಬಫರ್)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* fail */

  ಸ್ಮರಣೆ_ನಕಲು(ಬಫರ್, src, ಉದ್ದಳತೆ);

  ಹಿಂತಿರುಗಿ ಬಫರ್;
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
ಶೂನ್ಯ *curlx_memdup0(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, size_t ಉದ್ದಳತೆ)
{
  ಅಕ್ಷರ *buf = (ಉದ್ದಳತೆ < SIZE_MAX) ? curlx_malloc(ಉದ್ದಳತೆ + 1) : ಖಾಲಿ;
  ಆದರೆ(!buf)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಆದರೆ(ಉದ್ದಳತೆ) {
    DEBUGASSERT(src); /* must never be NULL */
    ಸ್ಮರಣೆ_ನಕಲು(buf, src, ಉದ್ದಳತೆ);
  }
  buf[ಉದ್ದಳತೆ] = 0;
  ಹಿಂತಿರುಗಿ buf;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CURLX_MEMZERO
ಸ್ಥಿರ ಶೂನ್ಯ *(* ಸ್ಥಿರಾಂಕ ಅಸ್ಥಿರ p_curlx_memset)(ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಂಕ val,
                                               size_t ಗಾತ್ರ_2) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
ಶೂನ್ಯ curlx_memzero_ಕೆಳಗಿನ(ಶೂನ್ಯ *buf, size_t ಗಾತ್ರ_2)
{
  ಆದರೆ(buf)
    p_curlx_memset(buf, 0, ಗಾತ್ರ_2);
}
#ಆದರೆ_ಅಂತ್ಯ

/* Fill 'buf' with zeroes. */
ಶೂನ್ಯ curlx_memzero(ಶೂನ್ಯ *buf, size_t ಗಾತ್ರ_2)
{
  ಆದರೆ(buf)
    curlx_memzero_ಕೆಳಗಿನ(buf, ಗಾತ್ರ_2);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
ಶೂನ್ಯ curlx_strzero(ಶೂನ್ಯ *buf)
{
  ಆದರೆ(buf)
    curlx_memzero_ಕೆಳಗಿನ(buf, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf));
}
