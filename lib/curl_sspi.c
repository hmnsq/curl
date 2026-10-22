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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_WINDOWS_SSPI

#अन्तर्भाव "curl_sspi.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/multibyte.h"

/* Pointer to SSPI dispatch table */
PSecurity_Function_सारणी Curl_pSecFn = रिक्त;

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
CURLcode Curl_sspi_global_init(शून्य)
{
  /* If security interface is not yet initialized try to do this */
  यदि(!Curl_pSecFn) {
    /* Get pointer to Security Service Provider Interface dispatch table */
    Curl_pSecFn = InitSecurityInterface();
    यदि(!Curl_pSecFn)
      प्रत्यागम CURLE_FAILED_INIT;
  }

  प्रत्यागम CURLE_OK;
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
शून्य Curl_sspi_global_cleanup(शून्य)
{
  यदि(Curl_pSecFn) {
    Curl_pSecFn = रिक्त;
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
CURLcode Curl_create_sspi_तटस्थः(ध्रुव वर्ण *userp, ध्रुव वर्ण *passwdp,
                                   SEC_WINNT_AUTH_तटस्थः_EX *तटस्थः)
{
  xcharp_u useranddomain;
  xcharp_u user, dup_user;
  xcharp_u domain, dup_domain;
  xcharp_u passwd, dup_passwd;
  size_t domlen = 0;
  size_t pwlen;

  domain.const_tchar_ptr = _TEXT("");

  /* Initialize the identity */
  स्मृतिं_पूरय(तटस्थः, 0, परिमाण(*तटस्थः));
  तटस्थः->संस्करणम् = SEC_WINNT_AUTH_तटस्थः_संस्करणम्;
  तटस्थः->दीर्घता_2 = परिमाण(*तटस्थः);

  useranddomain.tchar_ptr = curlx_convert_UTF8_गन्तव्यम्_tchar(userp);
  यदि(!useranddomain.tchar_ptr)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, _TEXT('\\'));
  यदि(!user.const_tchar_ptr)
    user.const_tchar_ptr = _tcschr(useranddomain.const_tchar_ptr, _TEXT('/'));

  यदि(user.tchar_ptr) {
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
  यदि(!dup_user.tchar_ptr) {
    curlx_free(useranddomain.tchar_ptr);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  /* Setup the identity's domain and length */
  dup_domain.tchar_ptr = curlx_malloc(परिमाण(TCHAR) * (domlen + 1));
  यदि(!dup_domain.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  यदि(_tcsncpy_s(dup_domain.tchar_ptr, domlen + 1, domain.tchar_ptr, domlen)) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_free(useranddomain.tchar_ptr);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  curlx_free(useranddomain.tchar_ptr);

  /* Setup the identity's password and length */
  passwd.tchar_ptr = curlx_convert_UTF8_गन्तव्यम्_tchar(passwdp);
  यदि(!passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  pwlen = _tcslen(passwd.tchar_ptr);
  dup_passwd.tchar_ptr = curlx_tcsdup(passwd.tchar_ptr);
  यदि(!dup_passwd.tchar_ptr) {
    curlx_free(dup_user.tchar_ptr);
    curlx_free(dup_domain.tchar_ptr);
    curlx_memzero(passwd.tchar_ptr, pwlen * परिमाण(*passwd.tchar_ptr));
    curlx_free(passwd.tchar_ptr);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  तटस्थः->Password = dup_passwd.tbyte_ptr;
  तटस्थः->Password_दीर्घता = curlx_uztoul(pwlen);
  dup_passwd.tchar_ptr = रिक्त;

  curlx_memzero(passwd.tchar_ptr, pwlen * परिमाण(*passwd.tchar_ptr));
  curlx_free(passwd.tchar_ptr);

  तटस्थः->User = dup_user.tbyte_ptr;
  तटस्थः->User_दीर्घता = curlx_uztoul(_tcslen(dup_user.tchar_ptr));
  dup_user.tchar_ptr = रिक्त;
  तटस्थः->Domain = dup_domain.tbyte_ptr;
  तटस्थः->Domain_दीर्घता = curlx_uztoul(domlen);
  dup_domain.tchar_ptr = रिक्त;

  तटस्थः->Flags = CURL_SEC_WINNT_AUTH_तटस्थः;

  प्रत्यागम CURLE_OK;
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
शून्य Curl_sspi_free_तटस्थः(SEC_WINNT_AUTH_तटस्थः_EX *तटस्थः)
{
  यदि(तटस्थः) {
    curlx_safefree(तटस्थः->User);
    curlx_memzero(तटस्थः->Password,
                  तटस्थः->Password_दीर्घता * परिमाण(*तटस्थः->Password));
    curlx_safefree(तटस्थः->Password);
    curlx_safefree(तटस्थः->Domain);
  }
}

#यदि_अन्तः /* USE_WINDOWS_SSPI */
