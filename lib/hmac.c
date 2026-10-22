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
#समावेश "curl_setup.h"

#अगर (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#समावेश "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

अटल स्थिर अचिन्हित अक्षर hmac_ipad = 0x36;
अटल स्थिर अचिन्हित अक्षर hmac_opad = 0x5C;

काष्ठा HMAC_context *Curl_HMAC_init(स्थिर काष्ठा HMAC_params *hashparams,
                                    स्थिर अचिन्हित अक्षर *कुंजी,
                                    अचिन्हित पूर्णांक keylen)
{
  माप_प्रकार i;
  काष्ठा HMAC_context *ctxt;
  अचिन्हित अक्षर *hkey;
  अचिन्हित अक्षर b;

  /* Create HMAC context. */
  i = माप(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  अगर(!ctxt)
    वापस ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (व्योम *)(ctxt + 1);
  ctxt->hashctxt2 = (व्योम *)((अक्षर *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  अगर(keylen > hashparams->maxkeylen) {
    अगर(hashparams->hinit(ctxt->hashctxt1))
      जाओ fail;
    hashparams->hupdate(ctxt->hashctxt1, कुंजी, keylen);
    hkey = (अचिन्हित अक्षर *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    कुंजी = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  अगर(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    जाओ fail;

  क्रम(i = 0; i < keylen; i++) {
    b = (अचिन्हित अक्षर)(*कुंजी ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (अचिन्हित अक्षर)(*कुंजी++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  क्रम(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  वापस ctxt;

fail:
  curlx_free(ctxt);
  वापस शून्य;
}

व्योम Curl_HMAC_update(काष्ठा HMAC_context *ctxt,
                      स्थिर अचिन्हित अक्षर *तथ्य,
                      अचिन्हित पूर्णांक len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, तथ्य, len);
}

पूर्णांक Curl_HMAC_final(काष्ठा HMAC_context *ctxt, अचिन्हित अक्षर *output)
{
  स्थिर काष्ठा HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  अगर(!output)
    output = (अचिन्हित अक्षर *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  वापस 0;
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
CURLcode Curl_hmacit(स्थिर काष्ठा HMAC_params *hashparams,
                     स्थिर अचिन्हित अक्षर *कुंजी, स्थिर माप_प्रकार keylen,
                     स्थिर अचिन्हित अक्षर *तथ्य, माप_प्रकार datalen,
                     अचिन्हित अक्षर *output)
{
  काष्ठा HMAC_context *ctxt;
  अगर(keylen > अच_पूर्णांक_उच्च) /* unlikely to ever happen */
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, कुंजी, curlx_uztoui(keylen));

  अगर(!ctxt)
    वापस CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  करो {
    अचिन्हित पूर्णांक ilen = (अचिन्हित पूर्णांक)CURLMIN(datalen, अच_पूर्णांक_उच्च);
    Curl_HMAC_update(ctxt, तथ्य, ilen);
    datalen -= ilen;
    तथ्य += ilen;
  } जबतक(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  वापस CURLE_OK;
}

#पूर्ण_अगर /* Using NTLM (without SSPI) or AWS */
