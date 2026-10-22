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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_WINDOWS_SSPI

#شامل "curl_sspi.h"
#شامل "curlx/strdup.h"
#شامل "curlx/multibyte.h"

/* Pointer to SSPI dispatch table */
PSecurity_Function_جدول Curl_pSecFn = تہی;

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
CURLcode Curl_sspi_global_init(خالی)
{
  /* If security interface is not yet initialized try to do this */
  اگر(!Curl_pSecFn) {
    /* Get pointer to Security Service Provider Interface dispatch table */
    Curl_pSecFn = InitSecurityInterface();
    اگر(!Curl_pSecFn)
      واپس CURLE_FAILED_INIT;
  }

  واپس CURLE_OK;
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
خالی Curl_sspi_global_cleanup(خالی)
{
  اگر(Curl_pSecFn) {
    Curl_pSecFn = تہی;
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
CURLcode Curl_create_sspi_شناختی_عنصر(ثابت حرف *userp, ثابت حرف *passwdp,
                                   SEC_WINNT_AUTH_شناختی_عنصر_EX *شناختی_عنصر)
{
  xcharp_u useranddomain;
  xcharp_u user, dup_user;
  xcharp_u domain, dup_domain;
  xcharp_u passwd, dup_passwd;
  size_t domlen = 0;
  size_t pwlen;

  domain.const_tchar_ptr = متن("");

  /* Initialize the identity */
  memset(شناختی_عنصر, 0, حجم(*شناختی_عنصر));
  شناختی_عنصر->نسخہ = SEC_WINNT_AUTH_شناختی_عنصر_نسخہ;
  شناختی_عنصر->لمبائی_2 = حجم(*شناختی_عنصر);

  useranddomain.tchar_ptr = curlx_convert_UTF8_مقصد_tchar(userp);
  اگر(!useranddomain.tchar_ptr)
    واپس CURLE_برآمد_کا_حافظہ;

  user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, متن('\\'));
  اگر(!user.const_tchar_ptr)
    user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, متن('/'));

  اگر(user.tchar_ptr) {
    domain.tchar_ptr = useranddomain.tchar_ptr;
    domlen = user.tchar_ptr - useranddomain.tchar_ptr;
    user.tchar_ptr++;
  }
  ورنہ {
    user.tchar_ptr = useranddomain.tchar_ptr;
    domain.const_tchar_ptr = متن("");
    domlen = 0;
  }

  /* Setup the identity's user and length */
  dup_user.tchar_ptr = curlx_tcsdup(user.tchar_ptr);
  اگر(!dup_user.tchar_ptr) {
    curlx_free(useranddomain.tchar_ptr);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  /* Setup the identity's domain and length */
  dup_domain.tchar_ptr = curlx_malloc(حجم(TCHAR) * (domlen + 1));
  اگر(!dup_domain.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    واپس CURLE_برآمد_کا_حافظہ;
  }
  اگر(_tcsncpy_s(dup_domain.tchar_ptr, domlen + 1, domain.tchar_ptr, domlen)) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  curlx_free(useranddomain.tchar_ptr);

  /* Setup the identity's password and length */
  passwd.tchar_ptr = curlx_convert_UTF8_مقصد_tchar(passwdp);
  اگر(!passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    واپس CURLE_برآمد_کا_حافظہ;
  }
  pwlen = _tcslen(passwd.tchar_ptr);
  dup_passwd.tchar_ptr = curlx_tcsdup(passwd.tchar_ptr);
  اگر(!dup_passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_memzero(passwd.tchar_ptr, pwlen * حجم(*passwd.tchar_ptr));
    curlx_free(passwd.tchar_ptr);
    واپس CURLE_برآمد_کا_حافظہ;
  }
  شناختی_عنصر->Password = dup_passwd.tbyte_ptr;
  شناختی_عنصر->Password_لمبائی = curlx_uztoul(pwlen);
  dup_passwd.tchar_ptr = تہی;

  curlx_memzero(passwd.tchar_ptr, pwlen * حجم(*passwd.tchar_ptr));
  curlx_free(passwd.tchar_ptr);

  شناختی_عنصر->User = dup_user.tbyte_ptr;
  شناختی_عنصر->User_لمبائی = curlx_uztoul(_tcslen(dup_user.tchar_ptr));
  dup_user.tchar_ptr = تہی;
  شناختی_عنصر->Domain = dup_domain.tbyte_ptr;
  شناختی_عنصر->Domain_لمبائی = curlx_uztoul(domlen);
  dup_domain.tchar_ptr = تہی;

  شناختی_عنصر->Flags = CURL_SEC_WINNT_AUTH_شناختی_عنصر;

  واپس CURLE_OK;
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
خالی Curl_sspi_free_شناختی_عنصر(SEC_WINNT_AUTH_شناختی_عنصر_EX *شناختی_عنصر)
{
  اگر(شناختی_عنصر) {
    curlx_safefree(شناختی_عنصر->User);
    curlx_memzero(شناختی_عنصر->Password,
                  شناختی_عنصر->Password_لمبائی * حجم(*شناختی_عنصر->Password));
    curlx_safefree(شناختی_عنصر->Password);
    curlx_safefree(شناختی_عنصر->Domain);
  }
}

#اگر_ختم /* USE_WINDOWS_SSPI */
