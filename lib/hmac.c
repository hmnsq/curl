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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#ଅନ୍ତର୍ଭାଵ "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର hmac_ipad = 0x36;
ସ୍ଥିର ଅଚଳ unsigned ଅକ୍ଷର hmac_opad = 0x5C;

ସଂରଚନା HMAC_context *Curl_HMAC_init(ଅଚଳ ସଂରଚନା HMAC_params *hashparams,
                                    ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି,
                                    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା keylen)
{
  size_t i;
  ସଂରଚନା HMAC_context *ctxt;
  unsigned ଅକ୍ଷର *hkey;
  unsigned ଅକ୍ଷର b;

  /* Create HMAC context. */
  i = ଆକାର(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  ଯଦି(!ctxt)
    ଫେରନ୍ତୁ ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (ଶୂନ୍ୟ *)(ctxt + 1);
  ctxt->hashctxt2 = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  ଯଦି(keylen > hashparams->maxkeylen) {
    ଯଦି(hashparams->hinit(ctxt->hashctxt1))
      ଯାଅ fail;
    hashparams->hupdate(ctxt->hashctxt1, ଚାବି, keylen);
    hkey = (unsigned ଅକ୍ଷର *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    ଚାବି = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  ଯଦି(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    ଯାଅ fail;

  ପାଇଁ(i = 0; i < keylen; i++) {
    b = (unsigned ଅକ୍ଷର)(*ଚାବି ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned ଅକ୍ଷର)(*ଚାବି++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  ପାଇଁ(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  ଫେରନ୍ତୁ ctxt;

fail:
  curlx_free(ctxt);
  ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ Curl_HMAC_update(ସଂରଚନା HMAC_context *ctxt,
                      ଅଚଳ unsigned ଅକ୍ଷର *data,
                      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_HMAC_final(ସଂରଚନା HMAC_context *ctxt, unsigned ଅକ୍ଷର *output)
{
  ଅଚଳ ସଂରଚନା HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  ଯଦି(!output)
    output = (unsigned ଅକ୍ଷର *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  ଫେରନ୍ତୁ 0;
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
CURLcode Curl_hmacit(ଅଚଳ ସଂରଚନା HMAC_params *hashparams,
                     ଅଚଳ unsigned ଅକ୍ଷର *ଚାବି, ଅଚଳ size_t keylen,
                     ଅଚଳ unsigned ଅକ୍ଷର *data, size_t datalen,
                     unsigned ଅକ୍ଷର *output)
{
  ସଂରଚନା HMAC_context *ctxt;
  ଯଦି(keylen > UINT_MAX) /* unlikely to ever happen */
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, ଚାବି, curlx_uztoui(keylen));

  ଯଦି(!ctxt)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  କର {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ilen = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } ଯେତେବେଳେ(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  ଫେରନ୍ତୁ CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
