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

#ifdef _WIN32
#अन्तर्भाव <wchar.h>
#endif

#अन्तर्भाव "curlx/strdup.h"

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
wchar_t *curlx_wcsdup(ध्रुव wchar_t *src)
{
  size_t length = wcslen(src);

  जइ(length > (SIZE_MAX / परिमाण(wchar_t)) - 1)
    प्रत्यागम (wchar_t *)रिक्त; /* integer overflow */

  प्रत्यागम (wchar_t *)curlx_memdup(src, (length + 1) * परिमाण(wchar_t));
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
शून्य *curlx_memdup(ध्रुव शून्य *src, size_t length)
{
  शून्य *buffer = curlx_malloc(length);
  जइ(!buffer)
    प्रत्यागम रिक्त; /* fail */

  memcpy(buffer, src, length);

  प्रत्यागम buffer;
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
शून्य *curlx_memdup0(ध्रुव वर्ण *src, size_t length)
{
  वर्ण *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : रिक्त;
  जइ(!buf)
    प्रत्यागम रिक्त;
  जइ(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  प्रत्यागम buf;
}

#ifdef USE_CURLX_MEMZERO
स्थिर शून्य *(* ध्रुव चञ्चल p_curlx_memset)(शून्य *buf, पूर्णाङ्क val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
शून्य curlx_memzero_नीचः(शून्य *buf, size_t size)
{
  जइ(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
शून्य curlx_memzero(शून्य *buf, size_t size)
{
  जइ(buf)
    curlx_memzero_नीचः(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
शून्य curlx_strzero(शून्य *buf)
{
  जइ(buf)
    curlx_memzero_नीचः(buf, strlen(buf));
}
