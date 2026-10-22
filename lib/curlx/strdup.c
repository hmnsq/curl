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
#અન્તર્ભાવ "curl_setup.h"

#ifdef _WIN32
#અન્તર્ભાવ <wchar.h>
#endif

#અન્તર્ભાવ "curlx/strdup.h"

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
wchar_t *curlx_wcsdup(અચલ wchar_t *src)
{
  size_t length = wcslen(src);

  જો(length > (SIZE_MAX / કદ(wchar_t)) - 1)
    પાછા (wchar_t *)ખાલી; /* integer overflow */

  પાછા (wchar_t *)curlx_memdup(src, (length + 1) * કદ(wchar_t));
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
શૂન્ય *curlx_memdup(અચલ શૂન્ય *src, size_t length)
{
  શૂન્ય *buffer = curlx_malloc(length);
  જો(!buffer)
    પાછા ખાલી; /* fail */

  memcpy(buffer, src, length);

  પાછા buffer;
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
શૂન્ય *curlx_memdup0(અચલ અક્ષર *src, size_t length)
{
  અક્ષર *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : ખાલી;
  જો(!buf)
    પાછા ખાલી;
  જો(length) {
    DEBUGASSERT(src); /* must never be NULL */
    memcpy(buf, src, length);
  }
  buf[length] = 0;
  પાછા buf;
}

#ifdef USE_CURLX_MEMZERO
સ્થિર શૂન્ય *(* અચલ અસ્થિર p_curlx_memset)(શૂન્ય *buf, પૂર્ણાંક val,
                                               size_t size) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
શૂન્ય curlx_memzero_નીચલું(શૂન્ય *buf, size_t size)
{
  જો(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
શૂન્ય curlx_memzero(શૂન્ય *buf, size_t size)
{
  જો(buf)
    curlx_memzero_નીચલું(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
શૂન્ય curlx_strzero(શૂન્ય *buf)
{
  જો(buf)
    curlx_memzero_નીચલું(buf, strlen(buf));
}
