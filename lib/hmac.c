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
#અન્તર્ભાવ "curl_setup.h"

#જો (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#અન્તર્ભાવ "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

સ્થિર અચલ unsigned અક્ષર hmac_ipad = 0x36;
સ્થિર અચલ unsigned અક્ષર hmac_opad = 0x5C;

સંરચના HMAC_context *Curl_HMAC_init(અચલ સંરચના HMAC_params *hashparams,
                                    અચલ unsigned અક્ષર *ચાવી,
                                    unsigned પૂર્ણાંક keylen)
{
  size_t i;
  સંરચના HMAC_context *ctxt;
  unsigned અક્ષર *hkey;
  unsigned અક્ષર b;

  /* Create HMAC context. */
  i = કદ(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  જો(!ctxt)
    પાછા ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (શૂન્ય *)(ctxt + 1);
  ctxt->hashctxt2 = (શૂન્ય *)((અક્ષર *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  જો(keylen > hashparams->maxkeylen) {
    જો(hashparams->hinit(ctxt->hashctxt1))
      જાઓ fail;
    hashparams->hupdate(ctxt->hashctxt1, ચાવી, keylen);
    hkey = (unsigned અક્ષર *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    ચાવી = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  જો(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    જાઓ fail;

  માટે(i = 0; i < keylen; i++) {
    b = (unsigned અક્ષર)(*ચાવી ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned અક્ષર)(*ચાવી++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  માટે(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  પાછા ctxt;

fail:
  curlx_free(ctxt);
  પાછા ખાલી;
}

શૂન્ય Curl_HMAC_update(સંરચના HMAC_context *ctxt,
                      અચલ unsigned અક્ષર *data,
                      unsigned પૂર્ણાંક len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

પૂર્ણાંક Curl_HMAC_final(સંરચના HMAC_context *ctxt, unsigned અક્ષર *output)
{
  અચલ સંરચના HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  જો(!output)
    output = (unsigned અક્ષર *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  પાછા 0;
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
CURLcode Curl_hmacit(અચલ સંરચના HMAC_params *hashparams,
                     અચલ unsigned અક્ષર *ચાવી, અચલ size_t keylen,
                     અચલ unsigned અક્ષર *data, size_t datalen,
                     unsigned અક્ષર *output)
{
  સંરચના HMAC_context *ctxt;
  જો(keylen > UINT_MAX) /* unlikely to ever happen */
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, ચાવી, curlx_uztoui(keylen));

  જો(!ctxt)
    પાછા CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  કરો {
    unsigned પૂર્ણાંક ilen = (unsigned પૂર્ણાંક)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } જ્યારે(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  પાછા CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
