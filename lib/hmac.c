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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#ಅನ್ತರ್ಭಾವ "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ hmac_ipad = 0x36;
ಸ್ಥಿರ ಧ್ರುವ unsigned ವರ್ಣ hmac_opad = 0x5C;

ಸಂರಚನಾ HMAC_context *Curl_HMAC_init(ಧ್ರುವ ಸಂರಚನಾ HMAC_params *hashparams,
                                    ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿ,
                                    unsigned ಪೂರ್ಣಾಙ್ಕ keylen)
{
  size_t i;
  ಸಂರಚನಾ HMAC_context *ctxt;
  unsigned ವರ್ಣ *hkey;
  unsigned ವರ್ಣ b;

  /* Create HMAC context. */
  i = ಪರಿಮಾಣ(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  ಯದಿ(!ctxt)
    ಪ್ರತ್ಯಾಗಮ ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (ಶೂನ್ಯ *)(ctxt + 1);
  ctxt->hashctxt2 = (ಶೂನ್ಯ *)((ವರ್ಣ *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  ಯದಿ(keylen > hashparams->maxkeylen) {
    ಯದಿ(hashparams->hinit(ctxt->hashctxt1))
      ಗಚ್ಛ fail;
    hashparams->hupdate(ctxt->hashctxt1, ಕೀಲಿ, keylen);
    hkey = (unsigned ವರ್ಣ *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    ಕೀಲಿ = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  ಯದಿ(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    ಗಚ್ಛ fail;

  ಪ್ರತಿ(i = 0; i < keylen; i++) {
    b = (unsigned ವರ್ಣ)(*ಕೀಲಿ ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned ವರ್ಣ)(*ಕೀಲಿ++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  ಪ್ರತಿ(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  ಪ್ರತ್ಯಾಗಮ ctxt;

fail:
  curlx_free(ctxt);
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ Curl_HMAC_update(ಸಂರಚನಾ HMAC_context *ctxt,
                      ಧ್ರುವ unsigned ವರ್ಣ *data,
                      unsigned ಪೂರ್ಣಾಙ್ಕ len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

ಪೂರ್ಣಾಙ್ಕ Curl_HMAC_final(ಸಂರಚನಾ HMAC_context *ctxt, unsigned ವರ್ಣ *output)
{
  ಧ್ರುವ ಸಂರಚನಾ HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  ಯದಿ(!output)
    output = (unsigned ವರ್ಣ *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  ಪ್ರತ್ಯಾಗಮ 0;
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
CURLcode Curl_hmacit(ಧ್ರುವ ಸಂರಚನಾ HMAC_params *hashparams,
                     ಧ್ರುವ unsigned ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ size_t keylen,
                     ಧ್ರುವ unsigned ವರ್ಣ *data, size_t datalen,
                     unsigned ವರ್ಣ *output)
{
  ಸಂರಚನಾ HMAC_context *ctxt;
  ಯದಿ(keylen > UINT_MAX) /* unlikely to ever happen */
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, ಕೀಲಿ, curlx_uztoui(keylen));

  ಯದಿ(!ctxt)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  ಕುರು {
    unsigned ಪೂರ್ಣಾಙ್ಕ ilen = (unsigned ಪೂರ್ಣಾಙ್ಕ)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } ಯಾವತ್(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
