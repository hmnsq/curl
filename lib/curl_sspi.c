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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sspi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"

/* Pointer to SSPI dispatch table */
PSecurity_Function_ਸਾਰਣੀ Curl_pSecFn = ਨਿਰਾ;

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
CURLcode Curl_sspi_global_init(ਖਾਲੀ)
{
  /* If security interface is not yet initialized try to do this */
  ਜੇ(!Curl_pSecFn) {
    /* Get pointer to Security Service Provider Interface dispatch table */
    Curl_pSecFn = InitSecurityInterface();
    ਜੇ(!Curl_pSecFn)
      ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਵਾਪਸ CURLE_OK;
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
ਖਾਲੀ Curl_sspi_global_cleanup(ਖਾਲੀ)
{
  ਜੇ(Curl_pSecFn) {
    Curl_pSecFn = ਨਿਰਾ;
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
CURLcode Curl_create_sspi_ਪਛਾਣ_ਤੱਤ(ਅਚਲ ਅੱਖਰ *userp, ਅਚਲ ਅੱਖਰ *passwdp,
                                   SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *ਪਛਾਣ_ਤੱਤ)
{
  xcharp_u useranddomain;
  xcharp_u user, dup_user;
  xcharp_u domain, dup_domain;
  xcharp_u passwd, dup_passwd;
  size_t domlen = 0;
  size_t pwlen;

  domain.const_tchar_ptr = ਪਾਠ("");

  /* Initialize the identity */
  memset(ਪਛਾਣ_ਤੱਤ, 0, ਆਕਾਰ(*ਪਛਾਣ_ਤੱਤ));
  ਪਛਾਣ_ਤੱਤ->ਸੰਸਕਰਣ = SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_ਸੰਸਕਰਣ;
  ਪਛਾਣ_ਤੱਤ->ਲੰਮਾਈ_2 = ਆਕਾਰ(*ਪਛਾਣ_ਤੱਤ);

  useranddomain.tchar_ptr = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(userp);
  ਜੇ(!useranddomain.tchar_ptr)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, ਪਾਠ('\\'));
  ਜੇ(!user.const_tchar_ptr)
    user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, ਪਾਠ('/'));

  ਜੇ(user.tchar_ptr) {
    domain.tchar_ptr = useranddomain.tchar_ptr;
    domlen = user.tchar_ptr - useranddomain.tchar_ptr;
    user.tchar_ptr++;
  }
  ਨਹੀਂਤਾਂ {
    user.tchar_ptr = useranddomain.tchar_ptr;
    domain.const_tchar_ptr = ਪਾਠ("");
    domlen = 0;
  }

  /* Setup the identity's user and length */
  dup_user.tchar_ptr = curlx_tcsdup(user.tchar_ptr);
  ਜੇ(!dup_user.tchar_ptr) {
    curlx_free(useranddomain.tchar_ptr);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  /* Setup the identity's domain and length */
  dup_domain.tchar_ptr = curlx_malloc(ਆਕਾਰ(TCHAR) * (domlen + 1));
  ਜੇ(!dup_domain.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਜੇ(_tcsncpy_s(dup_domain.tchar_ptr, domlen + 1, domain.tchar_ptr, domlen)) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  curlx_free(useranddomain.tchar_ptr);

  /* Setup the identity's password and length */
  passwd.tchar_ptr = curlx_convert_UTF8_ਮੰਜ਼ਿਲ_tchar(passwdp);
  ਜੇ(!passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  pwlen = _tcslen(passwd.tchar_ptr);
  dup_passwd.tchar_ptr = curlx_tcsdup(passwd.tchar_ptr);
  ਜੇ(!dup_passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_memzero(passwd.tchar_ptr, pwlen * ਆਕਾਰ(*passwd.tchar_ptr));
    curlx_free(passwd.tchar_ptr);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਪਛਾਣ_ਤੱਤ->Password = dup_passwd.tbyte_ptr;
  ਪਛਾਣ_ਤੱਤ->Password_ਲੰਮਾਈ = curlx_uztoul(pwlen);
  dup_passwd.tchar_ptr = ਨਿਰਾ;

  curlx_memzero(passwd.tchar_ptr, pwlen * ਆਕਾਰ(*passwd.tchar_ptr));
  curlx_free(passwd.tchar_ptr);

  ਪਛਾਣ_ਤੱਤ->User = dup_user.tbyte_ptr;
  ਪਛਾਣ_ਤੱਤ->User_ਲੰਮਾਈ = curlx_uztoul(_tcslen(dup_user.tchar_ptr));
  dup_user.tchar_ptr = ਨਿਰਾ;
  ਪਛਾਣ_ਤੱਤ->Domain = dup_domain.tbyte_ptr;
  ਪਛਾਣ_ਤੱਤ->Domain_ਲੰਮਾਈ = curlx_uztoul(domlen);
  dup_domain.tchar_ptr = ਨਿਰਾ;

  ਪਛਾਣ_ਤੱਤ->Flags = CURL_SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ;

  ਵਾਪਸ CURLE_OK;
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
ਖਾਲੀ Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *ਪਛਾਣ_ਤੱਤ)
{
  ਜੇ(ਪਛਾਣ_ਤੱਤ) {
    curlx_safefree(ਪਛਾਣ_ਤੱਤ->User);
    curlx_memzero(ਪਛਾਣ_ਤੱਤ->Password,
                  ਪਛਾਣ_ਤੱਤ->Password_ਲੰਮਾਈ * ਆਕਾਰ(*ਪਛਾਣ_ਤੱਤ->Password));
    curlx_safefree(ਪਛਾਣ_ਤੱਤ->Password);
    curlx_safefree(ਪਛਾਣ_ਤੱਤ->Domain);
  }
}

#ਜੇ_ਅੰਤ /* USE_WINDOWS_SSPI */
