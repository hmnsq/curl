#यदि_अपरिभाषितम् शीर्षकम्_CURL_SSPI_H
#परिभाषय शीर्षकम्_CURL_SSPI_H
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

#अन्तर्भाव <sspi.h>

/* Local helper macro */
#यदि_परिभाषितम् UNICODE
#परिभाषय CURL_SEC_WINNT_AUTH_तटस्थः SEC_WINNT_AUTH_तटस्थः_UNICODE
#अन्यथा
#परिभाषय CURL_SEC_WINNT_AUTH_तटस्थः SEC_WINNT_AUTH_तटस्थः_ANSI
#यदि_अन्तः

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#यदि_अपरिभाषितम् SECPKG_ATTR_ENDPOINT_BINDINGS
#परिभाषय SECPKG_ATTR_ENDPOINT_BINDINGS 26
/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकारनिर्देश संरचना {
  अचिह्नित दीर्घ Bindings_दीर्घता;
  SEC_CHANNEL_BINDINGS *Bindings;
} SecPkgContext_Bindings;
#यदि_अन्तः

CURLcode Curl_sspi_global_init(शून्य);
शून्य Curl_sspi_global_cleanup(शून्य);

/* This is used to populate the domain in an SSPI identity structure */
CURLcode Curl_override_sspi_http_realm(ध्रुव वर्ण *chlg,
                                       SEC_WINNT_AUTH_तटस्थः_EX *तटस्थः);

/* This is used to generate an SSPI identity structure */
CURLcode Curl_create_sspi_तटस्थः(ध्रुव वर्ण *userp, ध्रुव वर्ण *passwdp,
                                   SEC_WINNT_AUTH_तटस्थः_EX *तटस्थः);

/* This is used to free an SSPI identity structure */
शून्य Curl_sspi_free_तटस्थः(SEC_WINNT_AUTH_तटस्थः_EX *तटस्थः);

/* Forward-declaration of global variables defined in curl_sspi.c */
बाह्य PSecurity_Function_सारणी Curl_pSecFn;

/* Provide Service Principal names as macros */
#परिभाषय SP_NAME_DIGEST              "WDigest"
#परिभाषय SP_NAME_NTLM                "NTLM"
#परिभाषय SP_NAME_NEGOTIATE           "Negotiate"
#परिभाषय SP_NAME_KERBEROS            "Kerberos"

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#यदि_अपरिभाषितम् ISC_REQ_USE_HTTP_STYLE
#परिभाषय ISC_REQ_USE_HTTP_STYLE                0x01000000
#यदि_अन्तः

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#यदि_अपरिभाषितम् SEC_E_INVALID_PARAMETER
#परिभाषय SEC_E_INVALID_PARAMETER               ((HRESULT)0x8009035DL)
#यदि_अन्तः
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#यदि_अपरिभाषितम् SEC_E_DELEGATION_POLICY
#परिभाषय SEC_E_DELEGATION_POLICY               ((HRESULT)0x8009035EL)
#यदि_अन्तः
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#यदि_अपरिभाषितम् SEC_E_POLICY_NLTM_ONLY
#परिभाषय SEC_E_POLICY_NLTM_ONLY                ((HRESULT)0x8009035FL)
#यदि_अन्तः

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#यदि_अपरिभाषितम् SEC_I_SIGNATURE_NEEDED
#परिभाषय SEC_I_SIGNATURE_NEEDED                ((HRESULT)0x0009035CL)
#यदि_अन्तः

/*
 * Definitions required from ntsecapi.h are directly provided below this point
 * to avoid including ntsecapi.h due to a conflict with OpenSSL's safestack.h
 */
#परिभाषय KERB_WRAP_NO_ENCRYPT 0x80000001

#यदि_अन्तः /* USE_WINDOWS_SSPI */

#यदि_अन्तः /* HEADER_CURL_SSPI_H */
