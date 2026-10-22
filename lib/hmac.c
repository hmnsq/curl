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

#もし (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
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

static const unsigned char hmac_ipad = 0x36;
static const unsigned char hmac_opad = 0x5C;

struct HMAC_context *Curl_HMAC_init(const struct HMAC_params *hashparams,
                                    const unsigned char *key,
                                    unsigned 整数 keylen)
{
  size_t i;
  struct HMAC_context *ctxt;
  unsigned char *hkey;
  unsigned char b;

  /* Create HMAC context. */
  i = sizeof(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  もし(!ctxt)
    戻る ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (void *)(ctxt + 1);
  ctxt->hashctxt2 = (void *)((char *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  もし(keylen > hashparams->maxkeylen) {
    もし(hashparams->hinit(ctxt->hashctxt1))
      goto fail;
    hashparams->hupdate(ctxt->hashctxt1, key, keylen);
    hkey = (unsigned char *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    key = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  もし(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    goto fail;

  ために(i = 0; i < keylen; i++) {
    b = (unsigned char)(*key ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned char)(*key++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  ために(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  戻る ctxt;

fail:
  curlx_free(ctxt);
  戻る NULL;
}

void Curl_HMAC_update(struct HMAC_context *ctxt,
                      const unsigned char *data,
                      unsigned 整数 len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

整数 Curl_HMAC_final(struct HMAC_context *ctxt, unsigned char *output)
{
  const struct HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  もし(!output)
    output = (unsigned char *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  戻る 0;
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
CURLcode Curl_hmacit(const struct HMAC_params *hashparams,
                     const unsigned char *key, const size_t keylen,
                     const unsigned char *data, size_t datalen,
                     unsigned char *output)
{
  struct HMAC_context *ctxt;
  もし(keylen > UINT_MAX) /* unlikely to ever happen */
    戻る CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, key, curlx_uztoui(keylen));

  もし(!ctxt)
    戻る CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  do {
    unsigned 整数 ilen = (unsigned 整数)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } ながら(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  戻る CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
