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
 * RFC2104 Keyed-Hashing for Message Authentication
 *
 ***************************************************************************/
#అన్తర్భావ "curl_setup.h"

#ఒకవేళ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#అన్తర్భావ "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

స్థిర స్థిరరాశి గుర్తులేని అక్షరం hmac_ipad = 0x36;
స్థిర స్థిరరాశి గుర్తులేని అక్షరం hmac_opad = 0x5C;

నిర్మాణం HMAC_context *Curl_HMAC_init(స్థిరరాశి నిర్మాణం HMAC_params *hashparams,
                                    స్థిరరాశి గుర్తులేని అక్షరం *కీలకం,
                                    గుర్తులేని పూర్ణాంకం keylen)
{
  size_t i;
  నిర్మాణం HMAC_context *ctxt;
  గుర్తులేని అక్షరం *hkey;
  గుర్తులేని అక్షరం b;

  /* Create HMAC context. */
  i = పరిమాణం(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  ఒకవేళ(!ctxt)
    తిరిగి ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (శూన్య *)(ctxt + 1);
  ctxt->hashctxt2 = (శూన్య *)((అక్షరం *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  ఒకవేళ(keylen > hashparams->maxkeylen) {
    ఒకవేళ(hashparams->hinit(ctxt->hashctxt1))
      వెళ్ళు fail;
    hashparams->hupdate(ctxt->hashctxt1, కీలకం, keylen);
    hkey = (గుర్తులేని అక్షరం *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    కీలకం = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  ఒకవేళ(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    వెళ్ళు fail;

  కొరకు(i = 0; i < keylen; i++) {
    b = (గుర్తులేని అక్షరం)(*కీలకం ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (గుర్తులేని అక్షరం)(*కీలకం++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  కొరకు(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  తిరిగి ctxt;

fail:
  curlx_free(ctxt);
  తిరిగి ఖాళీ;
}

శూన్య Curl_HMAC_update(నిర్మాణం HMAC_context *ctxt,
                      స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం,
                      గుర్తులేని పూర్ణాంకం len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, దత్తాంశం, len);
}

పూర్ణాంకం Curl_HMAC_final(నిర్మాణం HMAC_context *ctxt, గుర్తులేని అక్షరం *output)
{
  స్థిరరాశి నిర్మాణం HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  ఒకవేళ(!output)
    output = (గుర్తులేని అక్షరం *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  తిరిగి 0;
}

/*
 * Curl_hmacit()
 *
 * This is used to generate a HMAC hash, for the specified input data, given
 * the specified hash function and key.
 *
 * Parameters:
 *
 * hashparams [in]     - The hash function (Curl_HMAC_MD5).
 * key        [in]     - The key to use.
 * keylen     [in]     - The length of the key.
 * buf        [in]     - The data to encrypt.
 * buflen     [in]     - The length of the data.
 * output     [in/out] - The output buffer.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_hmacit(స్థిరరాశి నిర్మాణం HMAC_params *hashparams,
                     స్థిరరాశి గుర్తులేని అక్షరం *కీలకం, స్థిరరాశి size_t keylen,
                     స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం, size_t datalen,
                     గుర్తులేని అక్షరం *output)
{
  నిర్మాణం HMAC_context *ctxt;
  ఒకవేళ(keylen > UINT_MAX) /* unlikely to ever happen */
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  ctxt = Curl_HMAC_init(hashparams, కీలకం, curlx_uztoui(keylen));

  ఒకవేళ(!ctxt)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* Update the digest with the given challenge */
  చేయి {
    గుర్తులేని పూర్ణాంకం ilen = (గుర్తులేని పూర్ణాంకం)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, దత్తాంశం, ilen);
    datalen -= ilen;
    దత్తాంశం += ilen;
  } అయితే(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  తిరిగి CURLE_OK;
}

#అయితే_ముగింపు /* Using NTLM (without SSPI) or AWS */
