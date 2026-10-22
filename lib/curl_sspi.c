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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_WINDOWS_SSPI

#అన్తర్భావ "curl_sspi.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/multibyte.h"

/* Pointer to SSPI dispatch table */
PSecurity_Function_పట్టిక Curl_pSecFn = ఖాళీ;

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
CURLcode Curl_sspi_global_init(శూన్య)
{
  /* If security interface is not yet initialized try to do this */
  ఒకవేళ(!Curl_pSecFn) {
    /* Get pointer to Security Service Provider Interface dispatch table */
    Curl_pSecFn = InitSecurityInterface();
    ఒకవేళ(!Curl_pSecFn)
      తిరిగి CURLE_FAILED_INIT;
  }

  తిరిగి CURLE_OK;
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
శూన్య Curl_sspi_global_cleanup(శూన్య)
{
  ఒకవేళ(Curl_pSecFn) {
    Curl_pSecFn = ఖాళీ;
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
CURLcode Curl_create_sspi_తటస్థం(స్థిరరాశి అక్షరం *userp, స్థిరరాశి అక్షరం *passwdp,
                                   SEC_WINNT_AUTH_తటస్థం_EX *తటస్థం)
{
  xcharp_u useranddomain;
  xcharp_u user, dup_user;
  xcharp_u domain, dup_domain;
  xcharp_u passwd, dup_passwd;
  size_t domlen = 0;
  size_t pwlen;

  domain.const_tchar_ptr = పాఠ్యం("");

  /* Initialize the identity */
  memset(తటస్థం, 0, పరిమాణం(*తటస్థం));
  తటస్థం->సంస్కరణ = SEC_WINNT_AUTH_తటస్థం_సంస్కరణ;
  తటస్థం->నిడివి_2 = పరిమాణం(*తటస్థం);

  useranddomain.tchar_ptr = curlx_convert_UTF8_గమ్యం_tchar(userp);
  ఒకవేళ(!useranddomain.tchar_ptr)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, పాఠ్యం('\\'));
  ఒకవేళ(!user.const_tchar_ptr)
    user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, పాఠ్యం('/'));

  ఒకవేళ(user.tchar_ptr) {
    domain.tchar_ptr = useranddomain.tchar_ptr;
    domlen = user.tchar_ptr - useranddomain.tchar_ptr;
    user.tchar_ptr++;
  }
  లేకపోతే {
    user.tchar_ptr = useranddomain.tchar_ptr;
    domain.const_tchar_ptr = పాఠ్యం("");
    domlen = 0;
  }

  /* Setup the identity's user and length */
  dup_user.tchar_ptr = curlx_tcsdup(user.tchar_ptr);
  ఒకవేళ(!dup_user.tchar_ptr) {
    curlx_free(useranddomain.tchar_ptr);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  /* Setup the identity's domain and length */
  dup_domain.tchar_ptr = curlx_malloc(పరిమాణం(TCHAR) * (domlen + 1));
  ఒకవేళ(!dup_domain.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  ఒకవేళ(_tcsncpy_s(dup_domain.tchar_ptr, domlen + 1, domain.tchar_ptr, domlen)) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  curlx_free(useranddomain.tchar_ptr);

  /* Setup the identity's password and length */
  passwd.tchar_ptr = curlx_convert_UTF8_గమ్యం_tchar(passwdp);
  ఒకవేళ(!passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  pwlen = _tcslen(passwd.tchar_ptr);
  dup_passwd.tchar_ptr = curlx_tcsdup(passwd.tchar_ptr);
  ఒకవేళ(!dup_passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_memzero(passwd.tchar_ptr, pwlen * పరిమాణం(*passwd.tchar_ptr));
    curlx_free(passwd.tchar_ptr);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  తటస్థం->Password = dup_passwd.tbyte_ptr;
  తటస్థం->Password_నిడివి = curlx_uztoul(pwlen);
  dup_passwd.tchar_ptr = ఖాళీ;

  curlx_memzero(passwd.tchar_ptr, pwlen * పరిమాణం(*passwd.tchar_ptr));
  curlx_free(passwd.tchar_ptr);

  తటస్థం->User = dup_user.tbyte_ptr;
  తటస్థం->User_నిడివి = curlx_uztoul(_tcslen(dup_user.tchar_ptr));
  dup_user.tchar_ptr = ఖాళీ;
  తటస్థం->Domain = dup_domain.tbyte_ptr;
  తటస్థం->Domain_నిడివి = curlx_uztoul(domlen);
  dup_domain.tchar_ptr = ఖాళీ;

  తటస్థం->Flags = CURL_SEC_WINNT_AUTH_తటస్థం;

  తిరిగి CURLE_OK;
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
శూన్య Curl_sspi_free_తటస్థం(SEC_WINNT_AUTH_తటస్థం_EX *తటస్థం)
{
  ఒకవేళ(తటస్థం) {
    curlx_safefree(తటస్థం->User);
    curlx_memzero(తటస్థం->Password,
                  తటస్థం->Password_నిడివి * పరిమాణం(*తటస్థం->Password));
    curlx_safefree(తటస్థం->Password);
    curlx_safefree(తటస్థం->Domain);
  }
}

#అయితే_ముగింపు /* USE_WINDOWS_SSPI */
