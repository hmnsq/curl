#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SSPI_H
#నిర్వచించు శీర్షిక_CURL_SSPI_H
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

#అన్తర్భావ <sspi.h>

/* Local helper macro */
#నిర్వచితమైతే UNICODE
#నిర్వచించు CURL_SEC_WINNT_AUTH_తటస్థం SEC_WINNT_AUTH_తటస్థం_UNICODE
#లేకపోతే
#నిర్వచించు CURL_SEC_WINNT_AUTH_తటస్థం SEC_WINNT_AUTH_తటస్థం_ANSI
#అయితే_ముగింపు

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#నిర్వచితం_కాకపోతే SECPKG_ATTR_ENDPOINT_BINDINGS
#నిర్వచించు SECPKG_ATTR_ENDPOINT_BINDINGS 26
/* !checksrc! disable TYPEDEFSTRUCT 1 */
రకనిర్వచనం నిర్మాణం {
  గుర్తులేని దీర్ఘ Bindings_నిడివి;
  SEC_CHANNEL_BINDINGS *Bindings;
} SecPkgContext_Bindings;
#అయితే_ముగింపు

CURLcode Curl_sspi_global_init(శూన్య);
శూన్య Curl_sspi_global_cleanup(శూన్య);

/* This is used to populate the domain in an SSPI identity structure */
CURLcode Curl_override_sspi_http_realm(స్థిరరాశి అక్షరం *chlg,
                                       SEC_WINNT_AUTH_తటస్థం_EX *తటస్థం);

/* This is used to generate an SSPI identity structure */
CURLcode Curl_create_sspi_తటస్థం(స్థిరరాశి అక్షరం *userp, స్థిరరాశి అక్షరం *passwdp,
                                   SEC_WINNT_AUTH_తటస్థం_EX *తటస్థం);

/* This is used to free an SSPI identity structure */
శూన్య Curl_sspi_free_తటస్థం(SEC_WINNT_AUTH_తటస్థం_EX *తటస్థం);

/* Forward-declaration of global variables defined in curl_sspi.c */
బాహ్య PSecurity_Function_పట్టిక Curl_pSecFn;

/* Provide Service Principal names as macros */
#నిర్వచించు SP_NAME_DIGEST              "WDigest"
#నిర్వచించు SP_NAME_NTLM                "NTLM"
#నిర్వచించు SP_NAME_NEGOTIATE           "Negotiate"
#నిర్వచించు SP_NAME_KERBEROS            "Kerberos"

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#నిర్వచితం_కాకపోతే ISC_REQ_USE_HTTP_STYLE
#నిర్వచించు ISC_REQ_USE_HTTP_STYLE                0x01000000
#అయితే_ముగింపు

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#నిర్వచితం_కాకపోతే SEC_E_INVALID_PARAMETER
#నిర్వచించు SEC_E_INVALID_PARAMETER               ((HRESULT)0x8009035DL)
#అయితే_ముగింపు
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#నిర్వచితం_కాకపోతే SEC_E_DELEGATION_POLICY
#నిర్వచించు SEC_E_DELEGATION_POLICY               ((HRESULT)0x8009035EL)
#అయితే_ముగింపు
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#నిర్వచితం_కాకపోతే SEC_E_POLICY_NLTM_ONLY
#నిర్వచించు SEC_E_POLICY_NLTM_ONLY                ((HRESULT)0x8009035FL)
#అయితే_ముగింపు

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#నిర్వచితం_కాకపోతే SEC_I_SIGNATURE_NEEDED
#నిర్వచించు SEC_I_SIGNATURE_NEEDED                ((HRESULT)0x0009035CL)
#అయితే_ముగింపు

/*
 * Definitions required from ntsecapi.h are directly provided below this point
 * to avoid including ntsecapi.h due to a conflict with OpenSSL's safestack.h
 */
#నిర్వచించు KERB_WRAP_NO_ENCRYPT 0x80000001

#అయితే_ముగింపు /* USE_WINDOWS_SSPI */

#అయితే_ముగింపు /* HEADER_CURL_SSPI_H */
