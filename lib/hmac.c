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
#অন্তর্ভাব "curl_setup.h"

#যদি (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#অন্তর্ভাব "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

স্থির ধ্রুবক unsigned অক্ষর hmac_ipad = 0x36;
স্থির ধ্রুবক unsigned অক্ষর hmac_opad = 0x5C;

কাঠামো HMAC_context *Curl_HMAC_init(ধ্রুবক কাঠামো HMAC_params *hashparams,
                                    ধ্রুবক unsigned অক্ষর *চাবি,
                                    unsigned পূর্ণসংখ্যা keylen)
{
  size_t i;
  কাঠামো HMAC_context *ctxt;
  unsigned অক্ষর *hkey;
  unsigned অক্ষর b;

  /* Create HMAC context. */
  i = আকার(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  যদি(!ctxt)
    ফেরত ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (শূন্য *)(ctxt + 1);
  ctxt->hashctxt2 = (শূন্য *)((অক্ষর *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  যদি(keylen > hashparams->maxkeylen) {
    যদি(hashparams->hinit(ctxt->hashctxt1))
      যাও fail;
    hashparams->hupdate(ctxt->hashctxt1, চাবি, keylen);
    hkey = (unsigned অক্ষর *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    চাবি = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  যদি(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    যাও fail;

  জন্য(i = 0; i < keylen; i++) {
    b = (unsigned অক্ষর)(*চাবি ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned অক্ষর)(*চাবি++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  জন্য(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  ফেরত ctxt;

fail:
  curlx_free(ctxt);
  ফেরত ফাঁকা;
}

শূন্য Curl_HMAC_update(কাঠামো HMAC_context *ctxt,
                      ধ্রুবক unsigned অক্ষর *তথ্য,
                      unsigned পূর্ণসংখ্যা len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, তথ্য, len);
}

পূর্ণসংখ্যা Curl_HMAC_final(কাঠামো HMAC_context *ctxt, unsigned অক্ষর *output)
{
  ধ্রুবক কাঠামো HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  যদি(!output)
    output = (unsigned অক্ষর *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  ফেরত 0;
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
CURLcode Curl_hmacit(ধ্রুবক কাঠামো HMAC_params *hashparams,
                     ধ্রুবক unsigned অক্ষর *চাবি, ধ্রুবক size_t keylen,
                     ধ্রুবক unsigned অক্ষর *তথ্য, size_t datalen,
                     unsigned অক্ষর *output)
{
  কাঠামো HMAC_context *ctxt;
  যদি(keylen > UINT_MAX) /* unlikely to ever happen */
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, চাবি, curlx_uztoui(keylen));

  যদি(!ctxt)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  করো {
    unsigned পূর্ণসংখ্যা ilen = (unsigned পূর্ণসংখ্যা)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, তথ্য, ilen);
    datalen -= ilen;
    তথ্য += ilen;
  } যখন(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  ফেরত CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
