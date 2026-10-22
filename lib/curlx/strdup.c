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
ब_अक्षर_प्रकार *curlx_wcsdup(अचर ब_अक्षर_प्रकार *src)
{
  माप_प्रकार length = wcslen(src);

  यदि(length > (SIZE_MAX / परिमाण(ब_अक्षर_प्रकार)) - 1)
    फर्काऊ (ब_अक्षर_प्रकार *)रिक्त; /* integer overflow */

  फर्काऊ (ब_अक्षर_प्रकार *)curlx_memdup(src, (length + 1) * परिमाण(ब_अक्षर_प्रकार));
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
शून्य *curlx_memdup(अचर शून्य *src, माप_प्रकार length)
{
  शून्य *buffer = curlx_malloc(length);
  यदि(!buffer)
    फर्काऊ रिक्त; /* fail */

  स_नकल(buffer, src, length);

  फर्काऊ buffer;
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
शून्य *curlx_memdup0(अचर वर्ण *src, माप_प्रकार length)
{
  वर्ण *buf = (length < SIZE_MAX) ? curlx_malloc(length + 1) : रिक्त;
  यदि(!buf)
    फर्काऊ रिक्त;
  यदि(length) {
    DEBUGASSERT(src); /* must never be NULL */
    स_नकल(buf, src, length);
  }
  buf[length] = 0;
  फर्काऊ buf;
}

#ifdef USE_CURLX_MEMZERO
स्थिर शून्य *(* अचर चञ्चल p_curlx_memset)(शून्य *buf, पूर्णाङ्क val,
                                               माप_प्रकार size) = स_रखो;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
शून्य curlx_memzero_तल्लो(शून्य *buf, माप_प्रकार size)
{
  यदि(buf)
    p_curlx_memset(buf, 0, size);
}
#endif

/* Fill 'buf' with zeroes. */
शून्य curlx_memzero(शून्य *buf, माप_प्रकार size)
{
  यदि(buf)
    curlx_memzero_तल्लो(buf, size);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
शून्य curlx_strzero(शून्य *buf)
{
  यदि(buf)
    curlx_memzero_तल्लो(buf, म_माप(buf));
}
