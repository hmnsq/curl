#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SSPI_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SSPI_H
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

#ਅਨ੍ਤਰ੍ਭਾਵ <sspi.h>

/* Local helper macro */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNICODE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_UNICODE
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_ANSI
#ਜੇ_ਅੰਤ

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SECPKG_ATTR_ENDPOINT_BINDINGS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECPKG_ATTR_ENDPOINT_BINDINGS 26
/* !checksrc! disable TYPEDEFSTRUCT 1 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ Bindings_ਲੰਮਾਈ;
  SEC_CHANNEL_BINDINGS *Bindings;
} SecPkgContext_Bindings;
#ਜੇ_ਅੰਤ

CURLcode Curl_sspi_global_init(ਖਾਲੀ);
ਖਾਲੀ Curl_sspi_global_cleanup(ਖਾਲੀ);

/* This is used to populate the domain in an SSPI identity structure */
CURLcode Curl_override_sspi_http_realm(ਅਚਲ ਅੱਖਰ *chlg,
                                       SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *ਪਛਾਣ_ਤੱਤ);

/* This is used to generate an SSPI identity structure */
CURLcode Curl_create_sspi_ਪਛਾਣ_ਤੱਤ(ਅਚਲ ਅੱਖਰ *userp, ਅਚਲ ਅੱਖਰ *passwdp,
                                   SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *ਪਛਾਣ_ਤੱਤ);

/* This is used to free an SSPI identity structure */
ਖਾਲੀ Curl_sspi_free_ਪਛਾਣ_ਤੱਤ(SEC_WINNT_AUTH_ਪਛਾਣ_ਤੱਤ_EX *ਪਛਾਣ_ਤੱਤ);

/* Forward-declaration of global variables defined in curl_sspi.c */
ਬਾਹਰੀ PSecurity_Function_ਸਾਰਣੀ Curl_pSecFn;

/* Provide Service Principal names as macros */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_NAME_DIGEST              "WDigest"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_NAME_NTLM                "NTLM"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_NAME_NEGOTIATE           "Negotiate"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SP_NAME_KERBEROS            "Kerberos"

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ISC_REQ_USE_HTTP_STYLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISC_REQ_USE_HTTP_STYLE                0x01000000
#ਜੇ_ਅੰਤ

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEC_E_INVALID_PARAMETER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEC_E_INVALID_PARAMETER               ((HRESULT)0x8009035DL)
#ਜੇ_ਅੰਤ
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEC_E_DELEGATION_POLICY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEC_E_DELEGATION_POLICY               ((HRESULT)0x8009035EL)
#ਜੇ_ਅੰਤ
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEC_E_POLICY_NLTM_ONLY
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEC_E_POLICY_NLTM_ONLY                ((HRESULT)0x8009035FL)
#ਜੇ_ਅੰਤ

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEC_I_SIGNATURE_NEEDED
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEC_I_SIGNATURE_NEEDED                ((HRESULT)0x0009035CL)
#ਜੇ_ਅੰਤ

/*
 * Definitions required from ntsecapi.h are directly provided below this point
 * to avoid including ntsecapi.h due to a conflict with OpenSSL's safestack.h
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KERB_WRAP_NO_ENCRYPT 0x80000001

#ਜੇ_ਅੰਤ /* USE_WINDOWS_SSPI */

#ਜੇ_ਅੰਤ /* HEADER_CURL_SSPI_H */
