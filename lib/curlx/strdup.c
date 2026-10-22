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

#यदि_परिभाषितम् _WIN32
#अन्तर्भाव <wchar.h>
#यदि_अन्तः

#अन्तर्भाव "curlx/strdup.h"

#यदि_परिभाषितम् _WIN32
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
  size_t दीर्घता = wcslen(src);

  यदि(दीर्घता > (SIZE_MAX / परिमाण(wchar_t)) - 1)
    प्रत्यागम (wchar_t *)रिक्त; /* integer overflow */

  प्रत्यागम (wchar_t *)curlx_memdup(src, (दीर्घता + 1) * परिमाण(wchar_t));
}
#यदि_अन्तः

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
शून्य *curlx_memdup(ध्रुव शून्य *src, size_t दीर्घता)
{
  शून्य *बफर = curlx_malloc(दीर्घता);
  यदि(!बफर)
    प्रत्यागम रिक्त; /* fail */

  स्मृतिं_प्रतिलिख(बफर, src, दीर्घता);

  प्रत्यागम बफर;
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
शून्य *curlx_memdup0(ध्रुव वर्ण *src, size_t दीर्घता)
{
  वर्ण *buf = (दीर्घता < SIZE_MAX) ? curlx_malloc(दीर्घता + 1) : रिक्त;
  यदि(!buf)
    प्रत्यागम रिक्त;
  यदि(दीर्घता) {
    DEBUGASSERT(src); /* must never be NULL */
    स्मृतिं_प्रतिलिख(buf, src, दीर्घता);
  }
  buf[दीर्घता] = 0;
  प्रत्यागम buf;
}

#यदि_परिभाषितम् USE_CURLX_MEMZERO
स्थिर शून्य *(* ध्रुव चञ्चल p_curlx_memset)(शून्य *buf, पूर्णाङ्क val,
                                               size_t आकार) = स्मृतिं_पूरय;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
शून्य curlx_memzero_नीचः(शून्य *buf, size_t आकार)
{
  यदि(buf)
    p_curlx_memset(buf, 0, आकार);
}
#यदि_अन्तः

/* Fill 'buf' with zeroes. */
शून्य curlx_memzero(शून्य *buf, size_t आकार)
{
  यदि(buf)
    curlx_memzero_नीचः(buf, आकार);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
शून्य curlx_strzero(शून्य *buf)
{
  यदि(buf)
    curlx_memzero_नीचः(buf, वाक्यदैर्घ्यम्(buf));
}
