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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_WINDOWS_SSPI

#அந்தர்பாவ "curl_sspi.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/multibyte.h"

/* Pointer to SSPI dispatch table */
PSecurityFunctionTable Curl_pSecFn = வெற்று;

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
CURLcode Curl_sspi_global_init(வெறுமை)
{
  /* If security interface is not yet initialized try to do this */
  எனில்(!Curl_pSecFn) {
    /* Get pointer to Security Service Provider Interface dispatch table */
    Curl_pSecFn = InitSecurityInterface();
    எனில்(!Curl_pSecFn)
      திரும்பு CURLE_FAILED_INIT;
  }

  திரும்பு CURLE_OK;
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
வெறுமை Curl_sspi_global_cleanup(வெறுமை)
{
  எனில்(Curl_pSecFn) {
    Curl_pSecFn = வெற்று;
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
CURLcode Curl_create_sspi_identity(மாறிலி எழுத்து *userp, மாறிலி எழுத்து *passwdp,
                                   SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  xcharp_u useranddomain;
  xcharp_u user, dup_user;
  xcharp_u domain, dup_domain;
  xcharp_u passwd, dup_passwd;
  size_t domlen = 0;
  size_t pwlen;

  domain.const_tchar_ptr = _TEXT("");

  /* Initialize the identity */
  memset(identity, 0, அளவு(*identity));
  identity->Version = SEC_WINNT_AUTH_IDENTITY_VERSION;
  identity->Length = அளவு(*identity);

  useranddomain.tchar_ptr = curlx_convert_UTF8_to_tchar(userp);
  எனில்(!useranddomain.tchar_ptr)
    திரும்பு CURLE_OUT_OF_MEMORY;

  user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, _TEXT('\\'));
  எனில்(!user.const_tchar_ptr)
    user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, _TEXT('/'));

  எனில்(user.tchar_ptr) {
    domain.tchar_ptr = useranddomain.tchar_ptr;
    domlen = user.tchar_ptr - useranddomain.tchar_ptr;
    user.tchar_ptr++;
  }
  இல்லையெனில் {
    user.tchar_ptr = useranddomain.tchar_ptr;
    domain.const_tchar_ptr = _TEXT("");
    domlen = 0;
  }

  /* Setup the identity's user and length */
  dup_user.tchar_ptr = curlx_tcsdup(user.tchar_ptr);
  எனில்(!dup_user.tchar_ptr) {
    curlx_free(useranddomain.tchar_ptr);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  /* Setup the identity's domain and length */
  dup_domain.tchar_ptr = curlx_malloc(அளவு(TCHAR) * (domlen + 1));
  எனில்(!dup_domain.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
  எனில்(_tcsncpy_s(dup_domain.tchar_ptr, domlen + 1, domain.tchar_ptr, domlen)) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  curlx_free(useranddomain.tchar_ptr);

  /* Setup the identity's password and length */
  passwd.tchar_ptr = curlx_convert_UTF8_to_tchar(passwdp);
  எனில்(!passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
  pwlen = _tcslen(passwd.tchar_ptr);
  dup_passwd.tchar_ptr = curlx_tcsdup(passwd.tchar_ptr);
  எனில்(!dup_passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_memzero(passwd.tchar_ptr, pwlen * அளவு(*passwd.tchar_ptr));
    curlx_free(passwd.tchar_ptr);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
  identity->Password = dup_passwd.tbyte_ptr;
  identity->PasswordLength = curlx_uztoul(pwlen);
  dup_passwd.tchar_ptr = வெற்று;

  curlx_memzero(passwd.tchar_ptr, pwlen * அளவு(*passwd.tchar_ptr));
  curlx_free(passwd.tchar_ptr);

  identity->User = dup_user.tbyte_ptr;
  identity->UserLength = curlx_uztoul(_tcslen(dup_user.tchar_ptr));
  dup_user.tchar_ptr = வெற்று;
  identity->Domain = dup_domain.tbyte_ptr;
  identity->DomainLength = curlx_uztoul(domlen);
  dup_domain.tchar_ptr = வெற்று;

  identity->Flags = CURL_SEC_WINNT_AUTH_IDENTITY;

  திரும்பு CURLE_OK;
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
வெறுமை Curl_sspi_free_identity(SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  எனில்(identity) {
    curlx_safefree(identity->User);
    curlx_memzero(identity->Password,
                  identity->PasswordLength * அளவு(*identity->Password));
    curlx_safefree(identity->Password);
    curlx_safefree(identity->Domain);
  }
}

#endif /* USE_WINDOWS_SSPI */
