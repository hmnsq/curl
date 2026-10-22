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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI

#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"

/* Pointer to SSPI dispatch table */
PSecurity_Function_ಕೋಷ್ಟಕ Curl_pSecFn = ಖಾಲಿ;

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
  ಆದರೆ(!Curl_pSecFn) {
    /* Get pointer to Security Service Provider Interface dispatch table */
    Curl_pSecFn = InitSecurityInterface();
    ಆದರೆ(!Curl_pSecFn)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
  ಆದರೆ(Curl_pSecFn) {
    Curl_pSecFn = ಖಾಲಿ;
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
CURLcode Curl_create_sspi_ತಟಸ್ಥ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *userp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwdp,
                                   SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *ತಟಸ್ಥ)
{
  xcharp_u useranddomain;
  xcharp_u user, dup_user;
  xcharp_u domain, dup_domain;
  xcharp_u passwd, dup_passwd;
  size_t domlen = 0;
  size_t pwlen;

  domain.const_tchar_ptr = ಪಠ್ಯ("");

  /* Initialize the identity */
  memset(ತಟಸ್ಥ, 0, ಗಾತ್ರ(*ತಟಸ್ಥ));
  ತಟಸ್ಥ->ಆವೃತ್ತಿ = SEC_WINNT_AUTH_ತಟಸ್ಥ_ಆವೃತ್ತಿ;
  ತಟಸ್ಥ->ಉದ್ದಳತೆ_2 = ಗಾತ್ರ(*ತಟಸ್ಥ);

  useranddomain.tchar_ptr = curlx_convert_UTF8_ಗಮ್ಯ_tchar(userp);
  ಆದರೆ(!useranddomain.tchar_ptr)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, ಪಠ್ಯ('\\'));
  ಆದರೆ(!user.const_tchar_ptr)
    user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, ಪಠ್ಯ('/'));

  ಆದರೆ(user.tchar_ptr) {
    domain.tchar_ptr = useranddomain.tchar_ptr;
    domlen = user.tchar_ptr - useranddomain.tchar_ptr;
    user.tchar_ptr++;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    user.tchar_ptr = useranddomain.tchar_ptr;
    domain.const_tchar_ptr = ಪಠ್ಯ("");
    domlen = 0;
  }

  /* Setup the identity's user and length */
  dup_user.tchar_ptr = curlx_tcsdup(user.tchar_ptr);
  ಆದರೆ(!dup_user.tchar_ptr) {
    curlx_free(useranddomain.tchar_ptr);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  /* Setup the identity's domain and length */
  dup_domain.tchar_ptr = curlx_malloc(ಗಾತ್ರ(TCHAR) * (domlen + 1));
  ಆದರೆ(!dup_domain.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಆದರೆ(_tcsncpy_s(dup_domain.tchar_ptr, domlen + 1, domain.tchar_ptr, domlen)) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  curlx_free(useranddomain.tchar_ptr);

  /* Setup the identity's password and length */
  passwd.tchar_ptr = curlx_convert_UTF8_ಗಮ್ಯ_tchar(passwdp);
  ಆದರೆ(!passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  pwlen = _tcslen(passwd.tchar_ptr);
  dup_passwd.tchar_ptr = curlx_tcsdup(passwd.tchar_ptr);
  ಆದರೆ(!dup_passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_memzero(passwd.tchar_ptr, pwlen * ಗಾತ್ರ(*passwd.tchar_ptr));
    curlx_free(passwd.tchar_ptr);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ತಟಸ್ಥ->Password = dup_passwd.tbyte_ptr;
  ತಟಸ್ಥ->Password_ಉದ್ದಳತೆ = curlx_uztoul(pwlen);
  dup_passwd.tchar_ptr = ಖಾಲಿ;

  curlx_memzero(passwd.tchar_ptr, pwlen * ಗಾತ್ರ(*passwd.tchar_ptr));
  curlx_free(passwd.tchar_ptr);

  ತಟಸ್ಥ->User = dup_user.tbyte_ptr;
  ತಟಸ್ಥ->User_ಉದ್ದಳತೆ = curlx_uztoul(_tcslen(dup_user.tchar_ptr));
  dup_user.tchar_ptr = ಖಾಲಿ;
  ತಟಸ್ಥ->Domain = dup_domain.tbyte_ptr;
  ತಟಸ್ಥ->Domain_ಉದ್ದಳತೆ = curlx_uztoul(domlen);
  dup_domain.tchar_ptr = ಖಾಲಿ;

  ತಟಸ್ಥ->Flags = CURL_SEC_WINNT_AUTH_ತಟಸ್ಥ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಶೂನ್ಯ Curl_sspi_free_ತಟಸ್ಥ(SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *ತಟಸ್ಥ)
{
  ಆದರೆ(ತಟಸ್ಥ) {
    curlx_safefree(ತಟಸ್ಥ->User);
    curlx_memzero(ತಟಸ್ಥ->Password,
                  ತಟಸ್ಥ->Password_ಉದ್ದಳತೆ * ಗಾತ್ರ(*ತಟಸ್ಥ->Password));
    curlx_safefree(ತಟಸ್ಥ->Password);
    curlx_safefree(ತಟಸ್ಥ->Domain);
  }
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_WINDOWS_SSPI */
