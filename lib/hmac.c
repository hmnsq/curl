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
#ضمن "curl_setup.h"

#إذا (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#ضمن "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

ساكن ثابت بیعلامت حرف hmac_ipad = 0x36;
ساكن ثابت بیعلامت حرف hmac_opad = 0x5C;

بنية HMAC_context *Curl_HMAC_init(ثابت بنية HMAC_params *hashparams,
                                    ثابت بیعلامت حرف *کلید,
                                    بیعلامت صحيح keylen)
{
  size_t i;
  بنية HMAC_context *ctxt;
  بیعلامت حرف *hkey;
  بیعلامت حرف b;

  /* Create HMAC context. */
  i = حجم(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  إذا(!ctxt)
    أرجع ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (فراغ *)(ctxt + 1);
  ctxt->hashctxt2 = (فراغ *)((حرف *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  إذا(keylen > hashparams->maxkeylen) {
    إذا(hashparams->hinit(ctxt->hashctxt1))
      اذهب fail;
    hashparams->hupdate(ctxt->hashctxt1, کلید, keylen);
    hkey = (بیعلامت حرف *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    کلید = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  إذا(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    اذهب fail;

  لكل(i = 0; i < keylen; i++) {
    b = (بیعلامت حرف)(*کلید ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (بیعلامت حرف)(*کلید++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  لكل(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  أرجع ctxt;

fail:
  curlx_free(ctxt);
  أرجع عدم;
}

فراغ Curl_HMAC_update(بنية HMAC_context *ctxt,
                      ثابت بیعلامت حرف *داده,
                      بیعلامت صحيح len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, داده, len);
}

صحيح Curl_HMAC_final(بنية HMAC_context *ctxt, بیعلامت حرف *output)
{
  ثابت بنية HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  إذا(!output)
    output = (بیعلامت حرف *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  أرجع 0;
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
CURLcode Curl_hmacit(ثابت بنية HMAC_params *hashparams,
                     ثابت بیعلامت حرف *کلید, ثابت size_t keylen,
                     ثابت بیعلامت حرف *داده, size_t datalen,
                     بیعلامت حرف *output)
{
  بنية HMAC_context *ctxt;
  إذا(keylen > UINT_MAX) /* unlikely to ever happen */
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  ctxt = Curl_HMAC_init(hashparams, کلید, curlx_uztoui(keylen));

  إذا(!ctxt)
    أرجع CURLE_بیرون_از_حافظه;

  /* Update the digest with the given challenge */
  افعل {
    بیعلامت صحيح ilen = (بیعلامت صحيح)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, داده, ilen);
    datalen -= ilen;
    داده += ilen;
  } طالما(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  أرجع CURLE_OK;
}

#پایان_اگر /* Using NTLM (without SSPI) or AWS */
