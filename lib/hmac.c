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
#अन्तर्भाव "curl_setup.h"

#जर (defined(USE_CURL_NTLM_CORE) && !defined(USE_WINDOWS_SSPI)) ||    \
  !defined(CURL_DISABLE_AWS) || !defined(CURL_DISABLE_DIGEST_AUTH) || \
  defined(USE_SSL)

#अन्तर्भाव "curl_hmac.h"

/*
 * Generic HMAC algorithm.
 *
 *   This module computes HMAC digests based on any hash function. Parameters
 * and computing procedures are setup dynamically at HMAC computation context
 * initialization.
 */

स्थिर स्थिरांक अचिन्हित अक्षर hmac_ipad = 0x36;
स्थिर स्थिरांक अचिन्हित अक्षर hmac_opad = 0x5C;

संरचना HMAC_context *Curl_HMAC_init(स्थिरांक संरचना HMAC_params *hashparams,
                                    स्थिरांक अचिन्हित अक्षर *किल्ली,
                                    अचिन्हित पूर्णांक keylen)
{
  माप_प्रकार i;
  संरचना HMAC_context *ctxt;
  अचिन्हित अक्षर *hkey;
  अचिन्हित अक्षर b;

  /* Create HMAC context. */
  i = आकार(*ctxt) + (2 * hashparams->ctxtsize) + hashparams->resultlen;
  ctxt = curlx_malloc(i);

  जर(!ctxt)
    परत ctxt;

  ctxt->hash = hashparams;
  ctxt->hashctxt1 = (शून्य *)(ctxt + 1);
  ctxt->hashctxt2 = (शून्य *)((अक्षर *)ctxt->hashctxt1 + hashparams->ctxtsize);

  /* If the key is too long, replace it by its hash digest. */
  जर(keylen > hashparams->maxkeylen) {
    जर(hashparams->hinit(ctxt->hashctxt1))
      जा fail;
    hashparams->hupdate(ctxt->hashctxt1, किल्ली, keylen);
    hkey = (अचिन्हित अक्षर *)ctxt->hashctxt2 + hashparams->ctxtsize;
    hashparams->hfinal(hkey, ctxt->hashctxt1);
    किल्ली = hkey;
    keylen = hashparams->resultlen;
  }

  /* Prime the two hash contexts with the modified key. */
  जर(hashparams->hinit(ctxt->hashctxt1) ||
     hashparams->hinit(ctxt->hashctxt2))
    जा fail;

  साठी(i = 0; i < keylen; i++) {
    b = (अचिन्हित अक्षर)(*किल्ली ^ hmac_ipad);
    hashparams->hupdate(ctxt->hashctxt1, &b, 1);
    b = (अचिन्हित अक्षर)(*किल्ली++ ^ hmac_opad);
    hashparams->hupdate(ctxt->hashctxt2, &b, 1);
  }

  साठी(; i < hashparams->maxkeylen; i++) {
    hashparams->hupdate(ctxt->hashctxt1, &hmac_ipad, 1);
    hashparams->hupdate(ctxt->hashctxt2, &hmac_opad, 1);
  }

  /* Done, return pointer to HMAC context. */
  परत ctxt;

fail:
  curlx_free(ctxt);
  परत रिक्त;
}

शून्य Curl_HMAC_update(संरचना HMAC_context *ctxt,
                      स्थिरांक अचिन्हित अक्षर *data,
                      अचिन्हित पूर्णांक len)
{
  /* Update first hash calculation. */
  ctxt->hash->hupdate(ctxt->hashctxt1, data, len);
}

पूर्णांक Curl_HMAC_final(संरचना HMAC_context *ctxt, अचिन्हित अक्षर *output)
{
  स्थिरांक संरचना HMAC_params *hashparams = ctxt->hash;

  /* Do not get output if called with a null parameter: only release
     storage. */

  जर(!output)
    output = (अचिन्हित अक्षर *)ctxt->hashctxt2 + ctxt->hash->ctxtsize;

  hashparams->hfinal(output, ctxt->hashctxt1);
  hashparams->hupdate(ctxt->hashctxt2, output, hashparams->resultlen);
  hashparams->hfinal(output, ctxt->hashctxt2);
  curlx_free(ctxt);
  परत 0;
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
CURLcode Curl_hmacit(स्थिरांक संरचना HMAC_params *hashparams,
                     स्थिरांक अचिन्हित अक्षर *किल्ली, स्थिरांक माप_प्रकार keylen,
                     स्थिरांक अचिन्हित अक्षर *data, माप_प्रकार datalen,
                     अचिन्हित अक्षर *output)
{
  संरचना HMAC_context *ctxt;
  जर(keylen > अच_पूर्णांक_उच्च) /* unlikely to ever happen */
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  ctxt = Curl_HMAC_init(hashparams, किल्ली, curlx_uztoui(keylen));

  जर(!ctxt)
    परत CURLE_OUT_OF_MEMORY;

  /* Update the digest with the given challenge */
  करा {
    अचिन्हित पूर्णांक ilen = (अचिन्हित पूर्णांक)CURLMIN(datalen, अच_पूर्णांक_उच्च);
    Curl_HMAC_update(ctxt, data, ilen);
    datalen -= ilen;
    data += ilen;
  } पर्यंत(datalen);

  /* Finalise the digest */
  Curl_HMAC_final(ctxt, output);

  परत CURLE_OK;
}

#endif /* Using NTLM (without SSPI) or AWS */
