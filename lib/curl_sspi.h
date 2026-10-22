#اگر_تعریف_نشده سرآیند_CURL_SSPI_H
#تعریف_کن سرآیند_CURL_SSPI_H
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

#ضمن <sspi.h>

/* Local helper macro */
#اگر_تعریف_شده UNICODE
#تعریف_کن CURL_SEC_WINNT_AUTH_عنصر_خنثی SEC_WINNT_AUTH_عنصر_خنثی_UNICODE
#وإلا
#تعریف_کن CURL_SEC_WINNT_AUTH_عنصر_خنثی SEC_WINNT_AUTH_عنصر_خنثی_ANSI
#پایان_اگر

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#اگر_تعریف_نشده SECPKG_ATTR_ENDPOINT_BINDINGS
#تعریف_کن SECPKG_ATTR_ENDPOINT_BINDINGS 26
/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية {
  بیعلامت دراز Bindings_درازی;
  SEC_CHANNEL_BINDINGS *Bindings;
} SecPkgContext_Bindings;
#پایان_اگر

CURLcode Curl_sspi_global_init(فراغ);
فراغ Curl_sspi_global_cleanup(فراغ);

/* This is used to populate the domain in an SSPI identity structure */
CURLcode Curl_override_sspi_http_realm(ثابت حرف *chlg,
                                       SEC_WINNT_AUTH_عنصر_خنثی_EX *عنصر_خنثی);

/* This is used to generate an SSPI identity structure */
CURLcode Curl_create_sspi_عنصر_خنثی(ثابت حرف *userp, ثابت حرف *passwdp,
                                   SEC_WINNT_AUTH_عنصر_خنثی_EX *عنصر_خنثی);

/* This is used to free an SSPI identity structure */
فراغ Curl_sspi_free_عنصر_خنثی(SEC_WINNT_AUTH_عنصر_خنثی_EX *عنصر_خنثی);

/* Forward-declaration of global variables defined in curl_sspi.c */
خارجي PSecurity_Function_جدول Curl_pSecFn;

/* Provide Service Principal names as macros */
#تعریف_کن SP_NAME_DIGEST              "WDigest"
#تعریف_کن SP_NAME_NTLM                "NTLM"
#تعریف_کن SP_NAME_NEGOTIATE           "Negotiate"
#تعریف_کن SP_NAME_KERBEROS            "Kerberos"

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#اگر_تعریف_نشده ISC_REQ_USE_HTTP_STYLE
#تعریف_کن ISC_REQ_USE_HTTP_STYLE                0x01000000
#پایان_اگر

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#اگر_تعریف_نشده SEC_E_INVALID_PARAMETER
#تعریف_کن SEC_E_INVALID_PARAMETER               ((HRESULT)0x8009035DL)
#پایان_اگر
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#اگر_تعریف_نشده SEC_E_DELEGATION_POLICY
#تعریف_کن SEC_E_DELEGATION_POLICY               ((HRESULT)0x8009035EL)
#پایان_اگر
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#اگر_تعریف_نشده SEC_E_POLICY_NLTM_ONLY
#تعریف_کن SEC_E_POLICY_NLTM_ONLY                ((HRESULT)0x8009035FL)
#پایان_اگر

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#اگر_تعریف_نشده SEC_I_SIGNATURE_NEEDED
#تعریف_کن SEC_I_SIGNATURE_NEEDED                ((HRESULT)0x0009035CL)
#پایان_اگر

/*
 * Definitions required from ntsecapi.h are directly provided below this point
 * to avoid including ntsecapi.h due to a conflict with OpenSSL's safestack.h
 */
#تعریف_کن KERB_WRAP_NO_ENCRYPT 0x80000001

#پایان_اگر /* USE_WINDOWS_SSPI */

#پایان_اگر /* HEADER_CURL_SSPI_H */
