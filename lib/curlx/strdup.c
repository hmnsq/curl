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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef _WIN32
#ଅନ୍ତର୍ଭାଵ <wchar.h>
#endif

#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"

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
wchar_t *curlx_wcsdup(ଅଚଳ wchar_t *src)
{
  size_t length = wcslen(src);

  ଯଦି(length > (SIZE_MAX / ଆକାର(wchar_t)) - 1)
    ଫେରନ୍ତୁ (wchar_t *)ଖାଲି; /* integer overflow */

  ଫେରନ୍ତୁ (wchar_t *)curlx_memdup(src, (length + 1) * ଆକାର(wchar_t));
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
ଶୂନ୍ୟ *curlx_memdup(ଅଚଳ ଶୂନ୍ୟ *src, size_t length)
{
  ଶୂନ୍ୟ *buffer = curlx_malloc(length);
  ଯଦି(!buffer)
    ଫେରନ୍ତୁ ଖାଲି; /* fail */

  memcpy(buffer, src, length);

  ଫେରନ୍ତୁ buffer;
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
ଶୂନ୍ୟ *curlx_memdup0(ଅଚଳ ଅକ୍ଷର *src, size_t length)
{
  ଅକ୍ଷର *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : ଖାଲି;
  ଯଦି(!buf)
    ଫେରନ୍ତୁ ଖାଲି;
  ଯଦି(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  ଫେରନ୍ତୁ buf;
}

#ifdef USE_CURLX_MEMZERO
ସ୍ଥିର ଶୂନ୍ୟ *(* ଅଚଳ ଅସ୍ଥିର p_curlx_memset)(ଶୂନ୍ୟ *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
ଶୂନ୍ୟ curlx_memzero_ତଳ(ଶୂନ୍ୟ *buf, size_t size)
{
  ଯଦି(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
ଶୂନ୍ୟ curlx_memzero(ଶୂନ୍ୟ *buf, size_t size)
{
  ଯଦି(buf)
    curlx_memzero_ତଳ(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
ଶୂନ୍ୟ curlx_strzero(ଶୂନ୍ୟ *buf)
{
  ଯଦି(buf)
    curlx_memzero_ତଳ(buf, strlen(buf));
}
