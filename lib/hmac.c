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
#அந்தர்பாவ "curl_setup.h"

#எனில் (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#அந்தர்பாவ "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

நிலையான மாறிலி unsigned எழுத்து hmac_ipad = 0x36;
நிலையான மாறிலி unsigned எழுத்து hmac_opad = 0x5C;

கட்டமைப்பு HMAC_context *Curl_HMAC_init(மாறிலி கட்டமைப்பு HMAC_params *hashparams,
                                    மாறிலி unsigned எழுத்து *சாவி,
                                    unsigned முழு keylen)
{
  size_t i;
  கட்டமைப்பு HMAC_context *ctxt;
  unsigned எழுத்து *hkey;
  unsigned எழுத்து b;

  /* Create HMAC context. */
  i = அளவு(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  எனில்(!ctxt)
    திரும்பு ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (வெறுமை *)(ctxt + 1);
  ctxt->hashctxt2 = (வெறுமை *)((எழுத்து *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  எனில்(keylen > hashparams->maxkeylen) {
    எனில்(hashparams->hinit(ctxt->hashctxt1))
      செல் fail;
    hashparams->hupdate(ctxt->hashctxt1, சாவி, keylen);
    hkey = (unsigned எழுத்து *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    சாவி = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  எனில்(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    செல் fail;

  க்கு(i = 0; i < keylen; i++) {
    b = (unsigned எழுத்து)(*சாவி ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned எழுத்து)(*சாவி++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  க்கு(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  திரும்பு ctxt;

fail:
  curlx_free(ctxt);
  திரும்பு வெற்று;
}

வெறுமை Curl_HMAC_update(கட்டமைப்பு HMAC_context *ctxt,
                      மாறிலி unsigned எழுத்து *data,
                      unsigned முழு len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

முழு Curl_HMAC_final(கட்டமைப்பு HMAC_context *ctxt, unsigned எழுத்து *output)
{
  மாறிலி கட்டமைப்பு HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  எனில்(!output)
    output = (unsigned எழுத்து *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  திரும்பு 0;
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
CURLcode Curl_hmacit(மாறிலி கட்டமைப்பு HMAC_params *hashparams,
                     மாறிலி unsigned எழுத்து *சாவி, மாறிலி size_t keylen,
                     மாறிலி unsigned எழுத்து *data, size_t datalen,
                     unsigned எழுத்து *output)
{
  கட்டமைப்பு HMAC_context *ctxt;
  எனில்(keylen > UINT_MAX) /* unlikely to ever happen */
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, சாவி, curlx_uztoui(keylen));

  எனில்(!ctxt)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  செய் {
    unsigned முழு ilen = (unsigned முழு)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } போது(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  திரும்பு CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
