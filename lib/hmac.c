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

ساکن ثابت غیرعلامتی حرف hmac_ipad = 0x36;
ساکن ثابت غیرعلامتی حرف hmac_opad = 0x5C;

ساخت HMAC_context *Curl_HMAC_init(ثابت ساخت HMAC_params *hashparams,
                                    ثابت غیرعلامتی حرف *کلید,
                                    غیرعلامتی صحیح keylen)
{
  size_t i;
  ساخت HMAC_context *ctxt;
  غیرعلامتی حرف *hkey;
  غیرعلامتی حرف b;

  /* Create HMAC context. */
  i = حجم(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  اگر(!ctxt)
    واپس ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (خالی *)(ctxt + 1);
  ctxt->hashctxt2 = (خالی *)((حرف *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  اگر(keylen > hashparams->maxkeylen) {
    اگر(hashparams->hinit(ctxt->hashctxt1))
      جاؤ fail;
    hashparams->hupdate(ctxt->hashctxt1, کلید, keylen);
    hkey = (غیرعلامتی حرف *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    کلید = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  اگر(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    جاؤ fail;

  برائے(i = 0; i < keylen; i++) {
    b = (غیرعلامتی حرف)(*کلید ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (غیرعلامتی حرف)(*کلید++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  برائے(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  واپس ctxt;

fail:
  curlx_free(ctxt);
  واپس تہی;
}

خالی Curl_HMAC_update(ساخت HMAC_context *ctxt,
                      ثابت غیرعلامتی حرف *ڈیٹا,
                      غیرعلامتی صحیح len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, ڈیٹا, len);
}

صحیح Curl_HMAC_final(ساخت HMAC_context *ctxt, غیرعلامتی حرف *output)
{
  ثابت ساخت HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  اگر(!output)
    output = (غیرعلامتی حرف *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  واپس 0;
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
CURLcode Curl_hmacit(ثابت ساخت HMAC_params *hashparams,
                     ثابت غیرعلامتی حرف *کلید, ثابت size_t keylen,
                     ثابت غیرعلامتی حرف *ڈیٹا, size_t datalen,
                     غیرعلامتی حرف *output)
{
  ساخت HMAC_context *ctxt;
  اگر(keylen > UINT_MAX) /* unlikely to ever happen */
    واپس CURLE_BAD_FUNCTION_دلیل;
  ctxt = Curl_HMAC_init(hashparams, کلید, curlx_uztoui(keylen));

  اگر(!ctxt)
    واپس CURLE_برآمد_کا_حافظہ;

  /* Update the digest with the given challenge */
  کرو {
    غیرعلامتی صحیح ilen = (غیرعلامتی صحیح)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, ڈیٹا, ilen);
    datalen -= ilen;
    ڈیٹا += ilen;
  } جب(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  واپس CURLE_OK;
}

#اگر_ختم /* Using NTLM (without SSPI) or AWS */
