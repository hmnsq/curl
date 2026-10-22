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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#അന്തര്ഭാവ "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം hmac_ipad = 0x36;
സ്ഥിതം സ്ഥിരം unsigned അക്ഷരം hmac_opad = 0x5C;

ഘടന HMAC_context *Curl_HMAC_init(സ്ഥിരം ഘടന HMAC_params *hashparams,
                                    സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ,
                                    unsigned പൂർണ്ണസംഖ്യ keylen)
{
  size_t i;
  ഘടന HMAC_context *ctxt;
  unsigned അക്ഷരം *hkey;
  unsigned അക്ഷരം b;

  /* Create HMAC context. */
  i = വലുപ്പം(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  എങ്കിൽ(!ctxt)
    തിരികെ ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (ശൂന്യം *)(ctxt + 1);
  ctxt->hashctxt2 = (ശൂന്യം *)((അക്ഷരം *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  എങ്കിൽ(keylen > hashparams->maxkeylen) {
    എങ്കിൽ(hashparams->hinit(ctxt->hashctxt1))
      പോകുക fail;
    hashparams->hupdate(ctxt->hashctxt1, താക്കോൽ, keylen);
    hkey = (unsigned അക്ഷരം *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    താക്കോൽ = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  എങ്കിൽ(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    പോകുക fail;

  വേണ്ടി(i = 0; i < keylen; i++) {
    b = (unsigned അക്ഷരം)(*താക്കോൽ ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (unsigned അക്ഷരം)(*താക്കോൽ++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  വേണ്ടി(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  തിരികെ ctxt;

fail:
  curlx_free(ctxt);
  തിരികെ ശൂന്യ;
}

ശൂന്യം Curl_HMAC_update(ഘടന HMAC_context *ctxt,
                      സ്ഥിരം unsigned അക്ഷരം *data,
                      unsigned പൂർണ്ണസംഖ്യ len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

പൂർണ്ണസംഖ്യ Curl_HMAC_final(ഘടന HMAC_context *ctxt, unsigned അക്ഷരം *output)
{
  സ്ഥിരം ഘടന HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  എങ്കിൽ(!output)
    output = (unsigned അക്ഷരം *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  തിരികെ 0;
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
CURLcode Curl_hmacit(സ്ഥിരം ഘടന HMAC_params *hashparams,
                     സ്ഥിരം unsigned അക്ഷരം *താക്കോൽ, സ്ഥിരം size_t keylen,
                     സ്ഥിരം unsigned അക്ഷരം *data, size_t datalen,
                     unsigned അക്ഷരം *output)
{
  ഘടന HMAC_context *ctxt;
  എങ്കിൽ(keylen > UINT_MAX) /* unlikely to ever happen */
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, താക്കോൽ, curlx_uztoui(keylen));

  എങ്കിൽ(!ctxt)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  ചെയ്യുക {
    unsigned പൂർണ്ണസംഖ്യ ilen = (unsigned പൂർണ്ണസംഖ്യ)CURLMIN(datalen, UINT_MAX);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } ആകുമ്പോൾ(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  തിരികെ CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
