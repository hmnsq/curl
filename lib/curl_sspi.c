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
 ***************************************************************************/
#समावेश "curl_setup.h"

#अगर_घोषित USE_WINDOWS_SSPI

#समावेश "curl_sspi.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/multibyte.h"

/* Pointer to SSPI dispatch table */
PSecurityFunctionTable Curl_pSecFn = शून्य;

/*
 * Curl_sspi_global_init()
 *
 * This is used to load the Security Service Provider Interface (SSPI)
 * dynamic link library portably across all Windows versions, without
 * the need to directly link libcurl, nor the application using it, at
 * build time.
 *
 * Once this function has been executed, Windows SSPI functions can be
 * called through the Security Service Provider Interface dispatch table.
 *
 * Parameters:
 *
 * None.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_sspi_global_init(व्योम)
{
  /* If security interface is not yet initialized try to do this */
  अगर(!Curl_pSecFn) {
    /* Get pointer to Security Service Provider Interface dispatch table */
    Curl_pSecFn = InitSecurityInterface();
    अगर(!Curl_pSecFn)
      वापस CURLE_FAILED_INIT;
  }

  वापस CURLE_OK;
}

/*
 * Curl_sspi_global_cleanup()
 *
 * This deinitializes the Security Service Provider Interface from libcurl.
 *
 * Parameters:
 *
 * None.
 */
व्योम Curl_sspi_global_cleanup(व्योम)
{
  अगर(Curl_pSecFn) {
    Curl_pSecFn = शून्य;
  }
}

/*
 * Curl_create_sspi_identity()
 *
 * This is used to populate an SSPI identity structure based on the supplied
 * username and password.
 *
 * Parameters:
 *
 * userp    [in]     - The username in the format User or Domain\User.
 * passwdp  [in]     - The user's password.
 * identity [in/out] - The identity structure.
 *
 * Returns CURLE_OK on success.
 */
CURLcode Curl_create_sspi_identity(स्थिर अक्षर *userp, स्थिर अक्षर *passwdp,
                                   SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  xcharp_u useranddomain;
  xcharp_u user, dup_user;
  xcharp_u domain, dup_domain;
  xcharp_u passwd, dup_passwd;
  माप_प्रकार domlen = 0;
  माप_प्रकार pwlen;

  domain.const_tchar_ptr = _TEXT("");

  /* Initialize the identity */
  स_रखो(identity, 0, माप(*identity));
  identity->Version = SEC_WINNT_AUTH_IDENTITY_VERSION;
  identity->लंबाई_2 = माप(*identity);

  useranddomain.tchar_ptr = curlx_convert_UTF8_to_tchar(userp);
  अगर(!useranddomain.tchar_ptr)
    वापस CURLE_OUT_OF_MEMORY;

  user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, _TEXT('\\'));
  अगर(!user.const_tchar_ptr)
    user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, _TEXT('/'));

  अगर(user.tchar_ptr) {
    domain.tchar_ptr = useranddomain.tchar_ptr;
    domlen = user.tchar_ptr - useranddomain.tchar_ptr;
    user.tchar_ptr++;
  }
  अन्यथा {
    user.tchar_ptr = useranddomain.tchar_ptr;
    domain.const_tchar_ptr = _TEXT("");
    domlen = 0;
  }

  /* Setup the identity's user and length */
  dup_user.tchar_ptr = curlx_tcsdup(user.tchar_ptr);
  अगर(!dup_user.tchar_ptr) {
    curlx_free(useranddomain.tchar_ptr);
    वापस CURLE_OUT_OF_MEMORY;
  }

  /* Setup the identity's domain and length */
  dup_domain.tchar_ptr = curlx_malloc(माप(TCHAR) * (domlen + 1));
  अगर(!dup_domain.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    वापस CURLE_OUT_OF_MEMORY;
  }
  अगर(_tcsncpy_s(dup_domain.tchar_ptr, domlen + 1, domain.tchar_ptr, domlen)) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    वापस CURLE_OUT_OF_MEMORY;
  }

  curlx_free(useranddomain.tchar_ptr);

  /* Setup the identity's password and length */
  passwd.tchar_ptr = curlx_convert_UTF8_to_tchar(passwdp);
  अगर(!passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    वापस CURLE_OUT_OF_MEMORY;
  }
  pwlen = _tcslen(passwd.tchar_ptr);
  dup_passwd.tchar_ptr = curlx_tcsdup(passwd.tchar_ptr);
  अगर(!dup_passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_memzero(passwd.tchar_ptr, pwlen * माप(*passwd.tchar_ptr));
    curlx_free(passwd.tchar_ptr);
    वापस CURLE_OUT_OF_MEMORY;
  }
  identity->Password = dup_passwd.tbyte_ptr;
  identity->Password_लंबाई = curlx_uztoul(pwlen);
  dup_passwd.tchar_ptr = शून्य;

  curlx_memzero(passwd.tchar_ptr, pwlen * माप(*passwd.tchar_ptr));
  curlx_free(passwd.tchar_ptr);

  identity->User = dup_user.tbyte_ptr;
  identity->User_लंबाई = curlx_uztoul(_tcslen(dup_user.tchar_ptr));
  dup_user.tchar_ptr = शून्य;
  identity->Domain = dup_domain.tbyte_ptr;
  identity->Domain_लंबाई = curlx_uztoul(domlen);
  dup_domain.tchar_ptr = शून्य;

  identity->Flags = CURL_SEC_WINNT_AUTH_IDENTITY;

  वापस CURLE_OK;
}

/*
 * Curl_sspi_free_identity()
 *
 * This is used to free the contents of an SSPI identifier structure.
 *
 * Parameters:
 *
 * identity [in/out] - The identity structure.
 */
व्योम Curl_sspi_free_identity(SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  अगर(identity) {
    curlx_safefree(identity->User);
    curlx_memzero(identity->Password,
                  identity->Password_लंबाई * माप(*identity->Password));
    curlx_safefree(identity->Password);
    curlx_safefree(identity->Domain);
  }
}

#पूर्ण_अगर /* USE_WINDOWS_SSPI */
