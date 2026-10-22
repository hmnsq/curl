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

#ಆದರೆ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ hmac_ipad = 0x36;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ hmac_opad = 0x5C;

ರಚನೆ HMAC_context *Curl_HMAC_init(ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params *hashparams,
                                    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ,
                                    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ keylen)
{
  size_t i;
  ರಚನೆ HMAC_context *ctxt;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *hkey;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ b;

  /* Create HMAC context. */
  i = ಗಾತ್ರ(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  ಆದರೆ(!ctxt)
    ಹಿಂತಿರುಗಿ ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (ಶೂನ್ಯ *)(ctxt + 1);
  ctxt->hashctxt2 = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  ಆದರೆ(keylen > hashparams->maxkeylen) {
    ಆದರೆ(hashparams->hinit(ctxt->hashctxt1))
      ಹೋಗು fail;
    hashparams->hupdate(ctxt->hashctxt1, ಕೀಲಿ, keylen);
    hkey = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    ಕೀಲಿ = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  ಆದರೆ(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    ಹೋಗು fail;

  ಗಾಗಿ(i = 0; i < keylen; i++) {
    b = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(*ಕೀಲಿ ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(*ಕೀಲಿ++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  ಗಾಗಿ(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  ಹಿಂತಿರುಗಿ ctxt;

fail:
  curlx_free(ctxt);
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ Curl_HMAC_update(ರಚನೆ HMAC_context *ctxt,
                      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, ದತ್ತಾಂಶ, len);
}

ಪೂರ್ಣಾಂಕ Curl_HMAC_final(ರಚನೆ HMAC_context *ctxt, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  ಆದರೆ(!output)
    output = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  ಹಿಂತಿರುಗಿ 0;
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
CURLcode Curl_hmacit(ಸ್ಥಿರಾಂಕ ರಚನೆ HMAC_params *hashparams,
                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ size_t keylen,
                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ, size_t datalen,
                     ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *output)
{
  ರಚನೆ HMAC_context *ctxt;
  ಆದರೆ(keylen > UINT_MAX) /* unlikely to ever happen */
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ctxt = Curl_HMAC_init(hashparams, ಕೀಲಿ, curlx_uztoui(keylen));

  ಆದರೆ(!ctxt)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* Update the digest with the given challenge */
  ಮಾಡು {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ilen = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, ದತ್ತಾಂಶ, ilen);
    datalen -= ilen;
    ದತ್ತಾಂಶ += ilen;
  } ಆಗ(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ_ಅಂತ್ಯ /* Using NTLM (without SSPI) or AWS */
