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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ hmac_ipad = 0x36;
ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ hmac_opad = 0x5C;

ਬਣਤਰ HMAC_context *Curl_HMAC_init(ਅਚਲ ਬਣਤਰ HMAC_params *hashparams,
                                    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ,
                                    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ keylen)
{
  size_t i;
  ਬਣਤਰ HMAC_context *ctxt;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *hkey;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ b;

  /* Create HMAC context. */
  i = ਆਕਾਰ(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  ਜੇ(!ctxt)
    ਵਾਪਸ ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (ਖਾਲੀ *)(ctxt + 1);
  ctxt->hashctxt2 = (ਖਾਲੀ *)((ਅੱਖਰ *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  ਜੇ(keylen > hashparams->maxkeylen) {
    ਜੇ(hashparams->hinit(ctxt->hashctxt1))
      ਜਾਓ fail;
    hashparams->hupdate(ctxt->hashctxt1, ਕੁੰਜੀ, keylen);
    hkey = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    ਕੁੰਜੀ = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  ਜੇ(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    ਜਾਓ fail;

  ਲਈ(i = 0; i < keylen; i++) {
    b = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(*ਕੁੰਜੀ ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)(*ਕੁੰਜੀ++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  ਲਈ(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  ਵਾਪਸ ctxt;

fail:
  curlx_free(ctxt);
  ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ Curl_HMAC_update(ਬਣਤਰ HMAC_context *ctxt,
                      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ,
                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, ਅੰਕੜੇ, len);
}

ਪੂਰਨ Curl_HMAC_final(ਬਣਤਰ HMAC_context *ctxt, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output)
{
  ਅਚਲ ਬਣਤਰ HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  ਜੇ(!output)
    output = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  ਵਾਪਸ 0;
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
CURLcode Curl_hmacit(ਅਚਲ ਬਣਤਰ HMAC_params *hashparams,
                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ size_t keylen,
                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ, size_t datalen,
                     ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *output)
{
  ਬਣਤਰ HMAC_context *ctxt;
  ਜੇ(keylen > UINT_MAX) /* unlikely to ever happen */
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ctxt = Curl_HMAC_init(hashparams, ਕੁੰਜੀ, curlx_uztoui(keylen));

  ਜੇ(!ctxt)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* Update the digest with the given challenge */
  ਕਰੋ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ilen = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, ਅੰਕੜੇ, ilen);
    datalen -= ilen;
    ਅੰਕੜੇ += ilen;
  } ਜਦੋਂ(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਅੰਤ /* Using NTLM (without SSPI) or AWS */
