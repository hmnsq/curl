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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_WINDOWS_SSPI

#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"

/* Pointer to SSPI dispatch table */
PSecurityFunctionTable Curl_pSecFn = ರಿಕ್ತ;

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
CURLcode Curl_sspi_global_init(ಶೂನ್ಯ)
{
  /* If security interface is not yet initialized try to do this */
  ಯದಿ(!Curl_pSecFn) {
    /* Get pointer to Security Service Provider Interface dispatch table */
    Curl_pSecFn = InitSecurityInterface();
    ಯದಿ(!Curl_pSecFn)
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ಶೂನ್ಯ Curl_sspi_global_cleanup(ಶೂನ್ಯ)
{
  ಯದಿ(Curl_pSecFn) {
    Curl_pSecFn = ರಿಕ್ತ;
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
CURLcode Curl_create_sspi_identity(ಧ್ರುವ ವರ್ಣ *userp, ಧ್ರುವ ವರ್ಣ *passwdp,
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
  memset(identity, 0, ಪರಿಮಾಣ(*identity));
  identity->Version = SEC_WINNT_AUTH_IDENTITY_VERSION;
  identity->Length = ಪರಿಮಾಣ(*identity);

  useranddomain.tchar_ptr = curlx_convert_UTF8_to_tchar(userp);
  ಯದಿ(!useranddomain.tchar_ptr)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, _TEXT('\\'));
  ಯದಿ(!user.const_tchar_ptr)
    user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, _TEXT('/'));

  ಯದಿ(user.tchar_ptr) {
    domain.tchar_ptr = useranddomain.tchar_ptr;
    domlen = user.tchar_ptr - useranddomain.tchar_ptr;
    user.tchar_ptr++;
  }
  ಅನ್ಯಥಾ {
    user.tchar_ptr = useranddomain.tchar_ptr;
    domain.const_tchar_ptr = _TEXT("");
    domlen = 0;
  }

  /* Setup the identity's user and length */
  dup_user.tchar_ptr = curlx_tcsdup(user.tchar_ptr);
  ಯದಿ(!dup_user.tchar_ptr) {
    curlx_free(useranddomain.tchar_ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  /* Setup the identity's domain and length */
  dup_domain.tchar_ptr = curlx_malloc(ಪರಿಮಾಣ(TCHAR) * (domlen + 1));
  ಯದಿ(!dup_domain.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  ಯದಿ(_tcsncpy_s(dup_domain.tchar_ptr, domlen + 1, domain.tchar_ptr, domlen)) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  curlx_free(useranddomain.tchar_ptr);

  /* Setup the identity's password and length */
  passwd.tchar_ptr = curlx_convert_UTF8_to_tchar(passwdp);
  ಯದಿ(!passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  pwlen = _tcslen(passwd.tchar_ptr);
  dup_passwd.tchar_ptr = curlx_tcsdup(passwd.tchar_ptr);
  ಯದಿ(!dup_passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_memzero(passwd.tchar_ptr, pwlen * ಪರಿಮಾಣ(*passwd.tchar_ptr));
    curlx_free(passwd.tchar_ptr);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  identity->Password = dup_passwd.tbyte_ptr;
  identity->PasswordLength = curlx_uztoul(pwlen);
  dup_passwd.tchar_ptr = ರಿಕ್ತ;

  curlx_memzero(passwd.tchar_ptr, pwlen * ಪರಿಮಾಣ(*passwd.tchar_ptr));
  curlx_free(passwd.tchar_ptr);

  identity->User = dup_user.tbyte_ptr;
  identity->UserLength = curlx_uztoul(_tcslen(dup_user.tchar_ptr));
  dup_user.tchar_ptr = ರಿಕ್ತ;
  identity->Domain = dup_domain.tbyte_ptr;
  identity->DomainLength = curlx_uztoul(domlen);
  dup_domain.tchar_ptr = ರಿಕ್ತ;

  identity->Flags = CURL_SEC_WINNT_AUTH_IDENTITY;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ಶೂನ್ಯ Curl_sspi_free_identity(SEC_WINNT_AUTH_IDENTITY_EX *identity)
{
  ಯದಿ(identity) {
    curlx_safefree(identity->User);
    curlx_memzero(identity->Password,
                  identity->PasswordLength * ಪರಿಮಾಣ(*identity->Password));
    curlx_safefree(identity->Password);
    curlx_safefree(identity->Domain);
  }
}

#endif /* USE_WINDOWS_SSPI */
