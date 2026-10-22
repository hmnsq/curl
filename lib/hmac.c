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
#অন্তর্ভাৱ "curl_setup.h"

#যদি (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#অন্তর্ভাৱ "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

স্থির ধ্রুৱ unsigned ৱর্ণ hmac_ipad = 0x36;
স্থির ধ্রুৱ unsigned ৱর্ণ hmac_opad = 0x5C;

সংরচনা HMAC_context *Curl_HMAC_init(ধ্রুৱ সংরচনা HMAC_params *hashparams,
                                    ধ্রুৱ unsigned ৱর্ণ *চাবি,
                                    unsigned পূর্ণাঙ্ক keylen)
{
  size_t i;
  সংরচনা HMAC_context *ctxt;
  unsigned ৱর্ণ *hkey;
  unsigned ৱর্ণ b;

  /* Create HMAC context. */
  i = পরিমাণ(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  যদি(!ctxt)
    ঘূৰাই ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (শূন্য *)(ctxt + 1);
  ctxt->hashctxt2 = (শূন্য *)((ৱর্ণ *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  যদি(keylen > hashparams->maxkeylen) {
    যদি(hashparams->hinit(ctxt->hashctxt1))
      গচ্ছ fail;
    hashparams->hupdate(ctxt->hashctxt1, চাবি, keylen);
    hkey = (unsigned ৱর্ণ *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    চাবি = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  যদি(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    গচ্ছ fail;

  বাবে(i = 0; i < keylen; i++) {
    b = (unsigned ৱর্ণ)(*চাবি ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned ৱর্ণ)(*চাবি++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  বাবে(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  ঘূৰাই ctxt;

fail:
  curlx_free(ctxt);
  ঘূৰাই রিক্ত;
}

শূন্য Curl_HMAC_update(সংরচনা HMAC_context *ctxt,
                      ধ্রুৱ unsigned ৱর্ণ *data,
                      unsigned পূর্ণাঙ্ক len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

পূর্ণাঙ্ক Curl_HMAC_final(সংরচনা HMAC_context *ctxt, unsigned ৱর্ণ *output)
{
  ধ্রুৱ সংরচনা HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  যদি(!output)
    output = (unsigned ৱর্ণ *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  ঘূৰাই 0;
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
CURLcode Curl_hmacit(ধ্রুৱ সংরচনা HMAC_params *hashparams,
                     ধ্রুৱ unsigned ৱর্ণ *চাবি, ধ্রুৱ size_t keylen,
                     ধ্রুৱ unsigned ৱর্ণ *data, size_t datalen,
                     unsigned ৱর্ণ *output)
{
  সংরচনা HMAC_context *ctxt;
  যদি(keylen > UINT_MAX) /* unlikely to ever happen */
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, চাবি, curlx_uztoui(keylen));

  যদি(!ctxt)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  কৰা {
    unsigned পূর্ণাঙ্ক ilen = (unsigned পূর্ণাঙ্ক)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } যেতিয়ালৈ(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  ঘূৰাই CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
