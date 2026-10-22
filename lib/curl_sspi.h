#अगर_अघोषित HEADER_CURL_SSPI_H
#घोषणा HEADER_CURL_SSPI_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_WINDOWS_SSPI

#समावेश <sspi.h>

/* Local helper macro */
#अगर_घोषित UNICODE
#घोषणा CURL_SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_UNICODE
#अन्यथा
#घोषणा CURL_SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_ANSI
#पूर्ण_अगर

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#अगर_अघोषित SECPKG_ATTR_ENDPOINT_BINDINGS
#घोषणा SECPKG_ATTR_ENDPOINT_BINDINGS 26
/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकार काष्ठा {
  अचिन्हित दीर्घ Bindings_लंबाई;
  SEC_CHANNEL_BINDINGS *Bindings;
} SecPkgContext_Bindings;
#पूर्ण_अगर

CURLcode Curl_sspi_global_init(व्योम);
व्योम Curl_sspi_global_cleanup(व्योम);

/* This is used to populate the domain in an SSPI identity structure */
CURLcode Curl_override_sspi_http_realm(स्थिर अक्षर *chlg,
                                       SEC_WINNT_AUTH_IDENTITY_EX *identity);

/* This is used to generate an SSPI identity structure */
CURLcode Curl_create_sspi_identity(स्थिर अक्षर *userp, स्थिर अक्षर *passwdp,
                                   SEC_WINNT_AUTH_IDENTITY_EX *identity);

/* This is used to free an SSPI identity structure */
व्योम Curl_sspi_free_identity(SEC_WINNT_AUTH_IDENTITY_EX *identity);

/* Forward-declaration of global variables defined in curl_sspi.c */
बाह्य PSecurityFunctionTable Curl_pSecFn;

/* Provide Service Principal names as macros */
#घोषणा SP_NAME_DIGEST              "WDigest"
#घोषणा SP_NAME_NTLM                "NTLM"
#घोषणा SP_NAME_NEGOTIATE           "Negotiate"
#घोषणा SP_NAME_KERBEROS            "Kerberos"

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#अगर_अघोषित ISC_REQ_USE_HTTP_STYLE
#घोषणा ISC_REQ_USE_HTTP_STYLE                0x01000000
#पूर्ण_अगर

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#अगर_अघोषित SEC_E_INVALID_PARAMETER
#घोषणा SEC_E_INVALID_PARAMETER               ((HRESULT)0x8009035DL)
#पूर्ण_अगर
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#अगर_अघोषित SEC_E_DELEGATION_POLICY
#घोषणा SEC_E_DELEGATION_POLICY               ((HRESULT)0x8009035EL)
#पूर्ण_अगर
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#अगर_अघोषित SEC_E_POLICY_NLTM_ONLY
#घोषणा SEC_E_POLICY_NLTM_ONLY                ((HRESULT)0x8009035FL)
#पूर्ण_अगर

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#अगर_अघोषित SEC_I_SIGNATURE_NEEDED
#घोषणा SEC_I_SIGNATURE_NEEDED                ((HRESULT)0x0009035CL)
#पूर्ण_अगर

/*
 * Definitions required from ntsecapi.h are directly provided below this point
 * to avoid including ntsecapi.h due to a conflict with OpenSSL's safestack.h
 */
#घोषणा KERB_WRAP_NO_ENCRYPT 0x80000001

#पूर्ण_अगर /* USE_WINDOWS_SSPI */

#पूर्ण_अगर /* HEADER_CURL_SSPI_H */
