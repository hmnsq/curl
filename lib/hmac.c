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
#شامل "curl_setup.h"

#اگر (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#شامل "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

ایستا ثابت unsigned نویسه hmac_ipad = 0x36;
ایستا ثابت unsigned نویسه hmac_opad = 0x5C;

ساختار HMAC_context *Curl_HMAC_init(ثابت ساختار HMAC_params *hashparams,
                                    ثابت unsigned نویسه *کلید,
                                    unsigned صحیح keylen)
{
  size_t i;
  ساختار HMAC_context *ctxt;
  unsigned نویسه *hkey;
  unsigned نویسه b;

  /* Create HMAC context. */
  i = اندازه(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  اگر(!ctxt)
    بازگشت ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (تهی *)(ctxt + 1);
  ctxt->hashctxt2 = (تهی *)((نویسه *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  اگر(keylen > hashparams->maxkeylen) {
    اگر(hashparams->hinit(ctxt->hashctxt1))
      برو fail;
    hashparams->hupdate(ctxt->hashctxt1, کلید, keylen);
    hkey = (unsigned نویسه *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    کلید = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  اگر(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    برو fail;

  برای(i = 0; i < keylen; i++) {
    b = (unsigned نویسه)(*کلید ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned نویسه)(*کلید++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  برای(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  بازگشت ctxt;

fail:
  curlx_free(ctxt);
  بازگشت پوچ;
}

تهی Curl_HMAC_update(ساختار HMAC_context *ctxt,
                      ثابت unsigned نویسه *data,
                      unsigned صحیح len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

صحیح Curl_HMAC_final(ساختار HMAC_context *ctxt, unsigned نویسه *output)
{
  ثابت ساختار HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  اگر(!output)
    output = (unsigned نویسه *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  بازگشت 0;
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
CURLcode Curl_hmacit(ثابت ساختار HMAC_params *hashparams,
                     ثابت unsigned نویسه *کلید, ثابت size_t keylen,
                     ثابت unsigned نویسه *data, size_t datalen,
                     unsigned نویسه *output)
{
  ساختار HMAC_context *ctxt;
  اگر(keylen > UINT_MAX) /* unlikely to ever happen */
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, کلید, curlx_uztoui(keylen));

  اگر(!ctxt)
    بازگشت CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  انجامبده {
    unsigned صحیح ilen = (unsigned صحیح)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } وقتی(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  بازگشت CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
