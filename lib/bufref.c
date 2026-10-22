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

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curlx/strdup.h"

#నిర్వచితమైతే DEBUGBUILD
#నిర్వచించు SIGNATURE 0x5c48e9b2    /* Random pattern. */
#అయితే_ముగింపు

/*
 * Init a bufref struct.
 */
శూన్య Curl_bufref_init(నిర్మాణం bufref *br)
{
  DEBUGASSERT(br);
  br->dtor = ఖాళీ;
  br->ptr = ఖాళీ;
  br->len = 0;

#నిర్వచితమైతే DEBUGBUILD
  br->signature = SIGNATURE;
#అయితే_ముగింపు
}

/*
 * Free the buffer and re-init the necessary fields. It does not touch the
 * 'signature' field and thus this buffer reference can be reused.
 */

శూన్య Curl_bufref_free(నిర్మాణం bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  ఒకవేళ(br->ptr && br->dtor)
    br->dtor(CURL_UNCONST(br->ptr));

  br->dtor = ఖాళీ;
  br->ptr = ఖాళీ;
  br->len = 0;
}

/*
 * Set the buffer reference to new values. The previously referenced buffer
 * is released before assignment.
 */
శూన్య Curl_bufref_సమితి(నిర్మాణం bufref *br, స్థిరరాశి శూన్య *ptr, size_t len,
                     శూన్య (*dtor)(శూన్య *))
{
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ఆదానం_నిడివి);

  Curl_bufref_free(br);
  br->ptr = (స్థిరరాశి గుర్తులేని అక్షరం *)ptr;
  br->len = len;
  br->dtor = dtor;
}

/*
 * Get a pointer to the referenced buffer.
 */
స్థిరరాశి గుర్తులేని అక్షరం *Curl_bufref_uptr(స్థిరరాశి నిర్మాణం bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  తిరిగి br->ptr;
}

/*
 * Get a pointer to the referenced string.
 */
స్థిరరాశి అక్షరం *Curl_bufref_ptr(స్థిరరాశి నిర్మాణం bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  తిరిగి (స్థిరరాశి అక్షరం *)br->ptr;
}

/*
 * Get the length of the referenced buffer data.
 */
size_t Curl_bufref_len(స్థిరరాశి నిర్మాణం bufref *br)
{
  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);

  తిరిగి br->len;
}

CURLcode Curl_bufref_memdup0(నిర్మాణం bufref *br, స్థిరరాశి శూన్య *ptr, size_t len)
{
  గుర్తులేని అక్షరం *cpy = ఖాళీ;

  DEBUGASSERT(br);
  DEBUGASSERT(br->signature == SIGNATURE);
  DEBUGASSERT(br->ptr || !br->len);
  DEBUGASSERT(ptr || !len);
  DEBUGASSERT(len <= CURL_MAX_ఆదానం_నిడివి);

  ఒకవేళ(ptr) {
    cpy = curlx_memdup0(ptr, len);
    ఒకవేళ(!cpy)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  Curl_bufref_సమితి(br, cpy, len, curl_free);
  తిరిగి CURLE_OK;
}
