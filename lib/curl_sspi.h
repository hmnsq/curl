#ifndef HEADER_CURL_SSPI_H
#define HEADER_CURL_SSPI_H
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

#ifdef USE_WINDOWS_SSPI

#ضمن <sspi.h>

/* Local helper macro */
#ifdef UNICODE
#define CURL_SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_UNICODE
#ܐܚܪܢܐ
#define CURL_SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_ANSI
#endif

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#ifndef SECPKG_ATTR_ENDPOINT_BINDINGS
#define SECPKG_ATTR_ENDPOINT_BINDINGS 26
/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية {
  unsigned long BindingsLength;
  SEC_CHANNEL_BINDINGS *Bindings;
} SecPkgContext_Bindings;
#endif

CURLcode Curl_sspi_global_init(فراغ);
فراغ Curl_sspi_global_cleanup(فراغ);

/* This is used to populate the domain in an SSPI identity structure */
CURLcode Curl_override_sspi_http_realm(ثابت حرف *chlg,
                                       SEC_WINNT_AUTH_IDENTITY_EX *identity);

/* This is used to generate an SSPI identity structure */
CURLcode Curl_create_sspi_identity(ثابت حرف *userp, ثابت حرف *passwdp,
                                   SEC_WINNT_AUTH_IDENTITY_EX *identity);

/* This is used to free an SSPI identity structure */
فراغ Curl_sspi_free_identity(SEC_WINNT_AUTH_IDENTITY_EX *identity);

/* Forward-declaration of global variables defined in curl_sspi.c */
خارجي PSecurityFunctionTable Curl_pSecFn;

/* Provide Service Principal names as macros */
#define SP_NAME_DIGEST              "WDigest"
#define SP_NAME_NTLM                "NTLM"
#define SP_NAME_NEGOTIATE           "Negotiate"
#define SP_NAME_KERBEROS            "Kerberos"

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#ifndef ISC_REQ_USE_HTTP_STYLE
#define ISC_REQ_USE_HTTP_STYLE                0x01000000
#endif

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ifndef SEC_E_INVALID_PARAMETER
#define SEC_E_INVALID_PARAMETER               ((HRESULT)0x8009035DL)
#endif
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ifndef SEC_E_DELEGATION_POLICY
#define SEC_E_DELEGATION_POLICY               ((HRESULT)0x8009035EL)
#endif
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ifndef SEC_E_POLICY_NLTM_ONLY
#define SEC_E_POLICY_NLTM_ONLY                ((HRESULT)0x8009035FL)
#endif

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ifndef SEC_I_SIGNATURE_NEEDED
#define SEC_I_SIGNATURE_NEEDED                ((HRESULT)0x0009035CL)
#endif

/*
 * Definitions required from ntsecapi.h are directly provided below this point
 * to avoid including ntsecapi.h due to a conflict with OpenSSL's safestack.h
 */
#define KERB_WRAP_NO_ENCRYPT 0x80000001

#endif /* USE_WINDOWS_SSPI */

#endif /* HEADER_CURL_SSPI_H */
