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
#include "curl_setup.h"

#ပေႃး (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#include "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

static const unsigned var_na hmac_ipad = 0x36;
static const unsigned var_na hmac_opad = 0x5C;

sa_nracanaa HMAC_context *Curl_HMAC_init(const sa_nracanaa HMAC_params *hashparams,
                                    const unsigned var_na *key,
                                    unsigned puur_naa_nka keylen)
{
  size_t i;
  sa_nracanaa HMAC_context *ctxt;
  unsigned var_na *hkey;
  unsigned var_na b;

  /* Create HMAC context. */
  i = parimaa_na(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  ပေႃး(!ctxt)
    ပွၵ်ႈ ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (void *)(ctxt + 1);
  ctxt->hashctxt2 = (void *)((var_na *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  ပေႃး(keylen > hashparams->maxkeylen) {
    ပေႃး(hashparams->hinit(ctxt->hashctxt1))
      goto fail;
    hashparams->hupdate(ctxt->hashctxt1, key, keylen);
    hkey = (unsigned var_na *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    key = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  ပေႃး(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    goto fail;

  for(i = 0; i < keylen; i++) {
    b = (unsigned var_na)(*key ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned var_na)(*key++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  for(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  ပွၵ်ႈ ctxt;

fail:
  curlx_free(ctxt);
  ပွၵ်ႈ NULL;
}

void Curl_HMAC_update(sa_nracanaa HMAC_context *ctxt,
                      const unsigned var_na *data,
                      unsigned puur_naa_nka len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

puur_naa_nka Curl_HMAC_final(sa_nracanaa HMAC_context *ctxt, unsigned var_na *output)
{
  const sa_nracanaa HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  ပေႃး(!output)
    output = (unsigned var_na *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  ပွၵ်ႈ 0;
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
CURLcode Curl_hmacit(const sa_nracanaa HMAC_params *hashparams,
                     const unsigned var_na *key, const size_t keylen,
                     const unsigned var_na *data, size_t datalen,
                     unsigned var_na *output)
{
  sa_nracanaa HMAC_context *ctxt;
  ပေႃး(keylen > UINT_MAX) /* unlikely to ever happen */
    ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, key, curlx_uztoui(keylen));

  ပေႃး(!ctxt)
    ပွၵ်ႈ CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  do {
    unsigned puur_naa_nka ilen = (unsigned puur_naa_nka)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } ၶၢဝ်း(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  ပွၵ်ႈ CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
