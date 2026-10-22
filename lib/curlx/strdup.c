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
#समावेश "curl_setup.h"

#अगर_घोषित _WIN32
#समावेश <wchar.h>
#पूर्ण_अगर

#समावेश "curlx/strdup.h"

#अगर_घोषित _WIN32
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
ब_अक्षर_प्रकार *curlx_wcsdup(स्थिर ब_अक्षर_प्रकार *src)
{
  माप_प्रकार लंबाई = wcslen(src);

  अगर(लंबाई > (SIZE_MAX / माप(ब_अक्षर_प्रकार)) - 1)
    वापस (ब_अक्षर_प्रकार *)शून्य; /* integer overflow */

  वापस (ब_अक्षर_प्रकार *)curlx_memdup(src, (लंबाई + 1) * माप(ब_अक्षर_प्रकार));
}
#पूर्ण_अगर

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
व्योम *curlx_memdup(स्थिर व्योम *src, माप_प्रकार लंबाई)
{
  व्योम *बफर = curlx_malloc(लंबाई);
  अगर(!बफर)
    वापस शून्य; /* fail */

  स_नकल(बफर, src, लंबाई);

  वापस बफर;
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
व्योम *curlx_memdup0(स्थिर अक्षर *src, माप_प्रकार लंबाई)
{
  अक्षर *buf = (लंबाई < SIZE_MAX) ? curlx_malloc(लंबाई + 1) : शून्य;
  अगर(!buf)
    वापस शून्य;
  अगर(लंबाई) {
    DEBUGASSERT(src); /* must never be NULL */
    स_नकल(buf, src, लंबाई);
  }
  buf[लंबाई] = 0;
  वापस buf;
}

#अगर_घोषित USE_CURLX_MEMZERO
अटल व्योम *(* स्थिर अस्थिर p_curlx_memset)(व्योम *buf, पूर्णांक val,
                                               माप_प्रकार आकार) = स_रखो;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
व्योम curlx_memzero_निचला(व्योम *buf, माप_प्रकार आकार)
{
  अगर(buf)
    p_curlx_memset(buf, 0, आकार);
}
#पूर्ण_अगर

/* Fill 'buf' with zeroes. */
व्योम curlx_memzero(व्योम *buf, माप_प्रकार आकार)
{
  अगर(buf)
    curlx_memzero_निचला(buf, आकार);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
व्योम curlx_strzero(व्योम *buf)
{
  अगर(buf)
    curlx_memzero_निचला(buf, म_माप(buf));
}
