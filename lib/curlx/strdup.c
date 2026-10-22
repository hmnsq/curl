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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే _WIN32
#అన్తర్భావ <wchar.h>
#అయితే_ముగింపు

#అన్తర్భావ "curlx/strdup.h"

#నిర్వచితమైతే _WIN32
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
wchar_t *curlx_wcsdup(స్థిరరాశి wchar_t *src)
{
  size_t నిడివి = wcslen(src);

  ఒకవేళ(నిడివి > (SIZE_MAX / పరిమాణం(wchar_t)) - 1)
    తిరిగి (wchar_t *)ఖాళీ; /* integer overflow */

  తిరిగి (wchar_t *)curlx_memdup(src, (నిడివి + 1) * పరిమాణం(wchar_t));
}
#అయితే_ముగింపు

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
శూన్య *curlx_memdup(స్థిరరాశి శూన్య *src, size_t నిడివి)
{
  శూన్య *బఫరు = curlx_malloc(నిడివి);
  ఒకవేళ(!బఫరు)
    తిరిగి ఖాళీ; /* fail */

  జ్ఞాపకం_నకలు(బఫరు, src, నిడివి);

  తిరిగి బఫరు;
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
శూన్య *curlx_memdup0(స్థిరరాశి అక్షరం *src, size_t నిడివి)
{
  అక్షరం *buf = (నిడివి < SIZE_MAX) ? curlx_malloc(నిడివి + 1) : ఖాళీ;
  ఒకవేళ(!buf)
    తిరిగి ఖాళీ;
  ఒకవేళ(నిడివి) {
    DEBUGASSERT(src); /* must never be NULL */
    జ్ఞాపకం_నకలు(buf, src, నిడివి);
  }
  buf[నిడివి] = 0;
  తిరిగి buf;
}

#నిర్వచితమైతే USE_CURLX_MEMZERO
స్థిర శూన్య *(* స్థిరరాశి అస్థిర p_curlx_memset)(శూన్య *buf, పూర్ణాంకం val,
                                               size_t కొలత) = memset;

/* Local fallback in case there is no system function to securely zero a memory
   buffer. */
శూన్య curlx_memzero_దిగువ(శూన్య *buf, size_t కొలత)
{
  ఒకవేళ(buf)
    p_curlx_memset(buf, 0, కొలత);
}
#అయితే_ముగింపు

/* Fill 'buf' with zeroes. */
శూన్య curlx_memzero(శూన్య *buf, size_t కొలత)
{
  ఒకవేళ(buf)
    curlx_memzero_దిగువ(buf, కొలత);
}

/* Fill 'buf' with zeroes, where 'buf' is null-terminated. */
శూన్య curlx_strzero(శూన్య *buf)
{
  ఒకవేళ(buf)
    curlx_memzero_దిగువ(buf, అక్షరశ్రేణి_పొడవు(buf));
}
