#جے_بنا_تعریف سرخی_CURL_SSPI_H
#تعریف_کرو سرخی_CURL_SSPI_H
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

#جے_تعریف_شدہ USE_WINDOWS_SSPI

#ضمن <sspi.h>

/* Local helper macro */
#جے_تعریف_شدہ UNICODE
#تعریف_کرو CURL_SEC_WINNT_AUTH_پچھان_تت SEC_WINNT_AUTH_پچھان_تت_UNICODE
#وإلا
#تعریف_کرو CURL_SEC_WINNT_AUTH_پچھان_تت SEC_WINNT_AUTH_پچھان_تت_ANSI
#جے_انت

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#جے_بنا_تعریف SECPKG_ATTR_ENDPOINT_BINDINGS
#تعریف_کرو SECPKG_ATTR_ENDPOINT_BINDINGS 26
/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية {
  بنانشانی لما Bindings_لمائی;
  SEC_CHANNEL_BINDINGS *Bindings;
} SecPkgContext_Bindings;
#جے_انت

CURLcode Curl_sspi_global_init(فراغ);
فراغ Curl_sspi_global_cleanup(فراغ);

/* This is used to populate the domain in an SSPI identity structure */
CURLcode Curl_override_sspi_http_realm(ثابت حرف *chlg,
                                       SEC_WINNT_AUTH_پچھان_تت_EX *پچھان_تت);

/* This is used to generate an SSPI identity structure */
CURLcode Curl_create_sspi_پچھان_تت(ثابت حرف *userp, ثابت حرف *passwdp,
                                   SEC_WINNT_AUTH_پچھان_تت_EX *پچھان_تت);

/* This is used to free an SSPI identity structure */
فراغ Curl_sspi_free_پچھان_تت(SEC_WINNT_AUTH_پچھان_تت_EX *پچھان_تت);

/* Forward-declaration of global variables defined in curl_sspi.c */
خارجي PSecurity_Function_جدول Curl_pSecFn;

/* Provide Service Principal names as macros */
#تعریف_کرو SP_NAME_DIGEST              "WDigest"
#تعریف_کرو SP_NAME_NTLM                "NTLM"
#تعریف_کرو SP_NAME_NEGOTIATE           "Negotiate"
#تعریف_کرو SP_NAME_KERBEROS            "Kerberos"

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#جے_بنا_تعریف ISC_REQ_USE_HTTP_STYLE
#تعریف_کرو ISC_REQ_USE_HTTP_STYLE                0x01000000
#جے_انت

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#جے_بنا_تعریف SEC_E_INVALID_PARAMETER
#تعریف_کرو SEC_E_INVALID_PARAMETER               ((HRESULT)0x8009035DL)
#جے_انت
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#جے_بنا_تعریف SEC_E_DELEGATION_POLICY
#تعریف_کرو SEC_E_DELEGATION_POLICY               ((HRESULT)0x8009035EL)
#جے_انت
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#جے_بنا_تعریف SEC_E_POLICY_NLTM_ONLY
#تعریف_کرو SEC_E_POLICY_NLTM_ONLY                ((HRESULT)0x8009035FL)
#جے_انت

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#جے_بنا_تعریف SEC_I_SIGNATURE_NEEDED
#تعریف_کرو SEC_I_SIGNATURE_NEEDED                ((HRESULT)0x0009035CL)
#جے_انت

/*
 * Definitions required from ntsecapi.h are directly provided below this point
 * to avoid including ntsecapi.h due to a conflict with OpenSSL's safestack.h
 */
#تعریف_کرو KERB_WRAP_NO_ENCRYPT 0x80000001

#جے_انت /* USE_WINDOWS_SSPI */

#جے_انت /* HEADER_CURL_SSPI_H */
