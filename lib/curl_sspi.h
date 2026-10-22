#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_SSPI_H
#وصف_ڪريو سرو_CURL_SSPI_H
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

#جيڪڏهن_وصف_ٿيل USE_WINDOWS_SSPI

#ضمن <sspi.h>

/* Local helper macro */
#جيڪڏهن_وصف_ٿيل UNICODE
#وصف_ڪريو CURL_SEC_WINNT_AUTH_سڃاڻپ_عنصر SEC_WINNT_AUTH_سڃاڻپ_عنصر_UNICODE
#وإلا
#وصف_ڪريو CURL_SEC_WINNT_AUTH_سڃاڻپ_عنصر SEC_WINNT_AUTH_سڃاڻپ_عنصر_ANSI
#جيڪڏهن_پورو

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#جيڪڏهن_وصف_نه_ٿيل SECPKG_ATTR_ENDPOINT_BINDINGS
#وصف_ڪريو SECPKG_ATTR_ENDPOINT_BINDINGS 26
/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية {
  بنانشان ڊگهو Bindings_ڊيگهائي;
  SEC_CHANNEL_BINDINGS *Bindings;
} SecPkgContext_Bindings;
#جيڪڏهن_پورو

CURLcode Curl_sspi_global_init(فراغ);
فراغ Curl_sspi_global_cleanup(فراغ);

/* This is used to populate the domain in an SSPI identity structure */
CURLcode Curl_override_sspi_http_realm(ثابت حرف *chlg,
                                       SEC_WINNT_AUTH_سڃاڻپ_عنصر_EX *سڃاڻپ_عنصر);

/* This is used to generate an SSPI identity structure */
CURLcode Curl_create_sspi_سڃاڻپ_عنصر(ثابت حرف *userp, ثابت حرف *passwdp,
                                   SEC_WINNT_AUTH_سڃاڻپ_عنصر_EX *سڃاڻپ_عنصر);

/* This is used to free an SSPI identity structure */
فراغ Curl_sspi_free_سڃاڻپ_عنصر(SEC_WINNT_AUTH_سڃاڻپ_عنصر_EX *سڃاڻپ_عنصر);

/* Forward-declaration of global variables defined in curl_sspi.c */
خارجي PSecurity_Function_جدول Curl_pSecFn;

/* Provide Service Principal names as macros */
#وصف_ڪريو SP_NAME_DIGEST              "WDigest"
#وصف_ڪريو SP_NAME_NTLM                "NTLM"
#وصف_ڪريو SP_NAME_NEGOTIATE           "Negotiate"
#وصف_ڪريو SP_NAME_KERBEROS            "Kerberos"

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#جيڪڏهن_وصف_نه_ٿيل ISC_REQ_USE_HTTP_STYLE
#وصف_ڪريو ISC_REQ_USE_HTTP_STYLE                0x01000000
#جيڪڏهن_پورو

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#جيڪڏهن_وصف_نه_ٿيل SEC_E_INVALID_PARAMETER
#وصف_ڪريو SEC_E_INVALID_PARAMETER               ((HRESULT)0x8009035DL)
#جيڪڏهن_پورو
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#جيڪڏهن_وصف_نه_ٿيل SEC_E_DELEGATION_POLICY
#وصف_ڪريو SEC_E_DELEGATION_POLICY               ((HRESULT)0x8009035EL)
#جيڪڏهن_پورو
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#جيڪڏهن_وصف_نه_ٿيل SEC_E_POLICY_NLTM_ONLY
#وصف_ڪريو SEC_E_POLICY_NLTM_ONLY                ((HRESULT)0x8009035FL)
#جيڪڏهن_پورو

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#جيڪڏهن_وصف_نه_ٿيل SEC_I_SIGNATURE_NEEDED
#وصف_ڪريو SEC_I_SIGNATURE_NEEDED                ((HRESULT)0x0009035CL)
#جيڪڏهن_پورو

/*
 * Definitions required from ntsecapi.h are directly provided below this point
 * to avoid including ntsecapi.h due to a conflict with OpenSSL's safestack.h
 */
#وصف_ڪريو KERB_WRAP_NO_ENCRYPT 0x80000001

#جيڪڏهن_پورو /* USE_WINDOWS_SSPI */

#جيڪڏهن_پورو /* HEADER_CURL_SSPI_H */
