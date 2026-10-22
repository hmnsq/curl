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
#ضمن "curl_setup.h"

#اگر_تعریف_شده USE_WINDOWS_SSPI

#ضمن "curl_sspi.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/multibyte.h"

/* Pointer to SSPI dispatch table */
PSecurity_Function_جدول Curl_pSecFn = عدم;

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
CURLcode Curl_sspi_global_init(فراغ)
{
  /* If security interface is not yet initialized try to do this */
  إذا(!Curl_pSecFn) {
    /* Get pointer to Security Service Provider Interface dispatch table */
    Curl_pSecFn = InitSecurityInterface();
    إذا(!Curl_pSecFn)
      أرجع CURLE_FAILED_INIT;
  }

  أرجع CURLE_OK;
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
فراغ Curl_sspi_global_cleanup(فراغ)
{
  إذا(Curl_pSecFn) {
    Curl_pSecFn = عدم;
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
CURLcode Curl_create_sspi_عنصر_خنثی(ثابت حرف *userp, ثابت حرف *passwdp,
                                   SEC_WINNT_AUTH_عنصر_خنثی_EX *عنصر_خنثی)
{
  xcharp_u useranddomain;
  xcharp_u user, dup_user;
  xcharp_u domain, dup_domain;
  xcharp_u passwd, dup_passwd;
  size_t domlen = 0;
  size_t pwlen;

  domain.const_tchar_ptr = متن("");

  /* Initialize the identity */
  memset(عنصر_خنثی, 0, حجم(*عنصر_خنثی));
  عنصر_خنثی->نسخه = SEC_WINNT_AUTH_عنصر_خنثی_نسخه;
  عنصر_خنثی->درازی_2 = حجم(*عنصر_خنثی);

  useranddomain.tchar_ptr = curlx_convert_UTF8_مقصد_tchar(userp);
  إذا(!useranddomain.tchar_ptr)
    أرجع CURLE_بیرون_از_حافظه;

  user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, متن('\\'));
  إذا(!user.const_tchar_ptr)
    user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, متن('/'));

  إذا(user.tchar_ptr) {
    domain.tchar_ptr = useranddomain.tchar_ptr;
    domlen = user.tchar_ptr - useranddomain.tchar_ptr;
    user.tchar_ptr++;
  }
  وإلا {
    user.tchar_ptr = useranddomain.tchar_ptr;
    domain.const_tchar_ptr = متن("");
    domlen = 0;
  }

  /* Setup the identity's user and length */
  dup_user.tchar_ptr = curlx_tcsdup(user.tchar_ptr);
  إذا(!dup_user.tchar_ptr) {
    curlx_free(useranddomain.tchar_ptr);
    أرجع CURLE_بیرون_از_حافظه;
  }

  /* Setup the identity's domain and length */
  dup_domain.tchar_ptr = curlx_malloc(حجم(TCHAR) * (domlen + 1));
  إذا(!dup_domain.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    أرجع CURLE_بیرون_از_حافظه;
  }
  إذا(_tcsncpy_s(dup_domain.tchar_ptr, domlen + 1, domain.tchar_ptr, domlen)) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    أرجع CURLE_بیرون_از_حافظه;
  }

  curlx_free(useranddomain.tchar_ptr);

  /* Setup the identity's password and length */
  passwd.tchar_ptr = curlx_convert_UTF8_مقصد_tchar(passwdp);
  إذا(!passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    أرجع CURLE_بیرون_از_حافظه;
  }
  pwlen = _tcslen(passwd.tchar_ptr);
  dup_passwd.tchar_ptr = curlx_tcsdup(passwd.tchar_ptr);
  إذا(!dup_passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_memzero(passwd.tchar_ptr, pwlen * حجم(*passwd.tchar_ptr));
    curlx_free(passwd.tchar_ptr);
    أرجع CURLE_بیرون_از_حافظه;
  }
  عنصر_خنثی->Password = dup_passwd.tbyte_ptr;
  عنصر_خنثی->Password_درازی = curlx_uztoul(pwlen);
  dup_passwd.tchar_ptr = عدم;

  curlx_memzero(passwd.tchar_ptr, pwlen * حجم(*passwd.tchar_ptr));
  curlx_free(passwd.tchar_ptr);

  عنصر_خنثی->User = dup_user.tbyte_ptr;
  عنصر_خنثی->User_درازی = curlx_uztoul(_tcslen(dup_user.tchar_ptr));
  dup_user.tchar_ptr = عدم;
  عنصر_خنثی->Domain = dup_domain.tbyte_ptr;
  عنصر_خنثی->Domain_درازی = curlx_uztoul(domlen);
  dup_domain.tchar_ptr = عدم;

  عنصر_خنثی->Flags = CURL_SEC_WINNT_AUTH_عنصر_خنثی;

  أرجع CURLE_OK;
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
فراغ Curl_sspi_free_عنصر_خنثی(SEC_WINNT_AUTH_عنصر_خنثی_EX *عنصر_خنثی)
{
  إذا(عنصر_خنثی) {
    curlx_safefree(عنصر_خنثی->User);
    curlx_memzero(عنصر_خنثی->Password,
                  عنصر_خنثی->Password_درازی * حجم(*عنصر_خنثی->Password));
    curlx_safefree(عنصر_خنثی->Password);
    curlx_safefree(عنصر_خنثی->Domain);
  }
}

#پایان_اگر /* USE_WINDOWS_SSPI */
