#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SSPI_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SSPI_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI

#ಅನ್ತರ್ಭಾವ <sspi.h>

/* Local helper macro */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNICODE
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SEC_WINNT_AUTH_ತಟಸ್ಥ SEC_WINNT_AUTH_ತಟಸ್ಥ_UNICODE
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SEC_WINNT_AUTH_ತಟಸ್ಥ SEC_WINNT_AUTH_ತಟಸ್ಥ_ANSI
#ಆದರೆ_ಅಂತ್ಯ

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SECPKG_ATTR_ENDPOINT_BINDINGS
#ವ್ಯಾಖ್ಯಾನಿಸು SECPKG_ATTR_ENDPOINT_BINDINGS 26
/* !checksrc! disable TYPEDEFSTRUCT 1 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ Bindings_ಉದ್ದಳತೆ;
  SEC_CHANNEL_BINDINGS *Bindings;
} SecPkgContext_Bindings;
#ಆದರೆ_ಅಂತ್ಯ

CURLcode Curl_sspi_global_init(ಶೂನ್ಯ);
ಶೂನ್ಯ Curl_sspi_global_cleanup(ಶೂನ್ಯ);

/* This is used to populate the domain in an SSPI identity structure */
CURLcode Curl_override_sspi_http_realm(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chlg,
                                       SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *ತಟಸ್ಥ);

/* This is used to generate an SSPI identity structure */
CURLcode Curl_create_sspi_ತಟಸ್ಥ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *userp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwdp,
                                   SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *ತಟಸ್ಥ);

/* This is used to free an SSPI identity structure */
ಶೂನ್ಯ Curl_sspi_free_ತಟಸ್ಥ(SEC_WINNT_AUTH_ತಟಸ್ಥ_EX *ತಟಸ್ಥ);

/* Forward-declaration of global variables defined in curl_sspi.c */
ಬಾಹ್ಯ PSecurity_Function_ಕೋಷ್ಟಕ Curl_pSecFn;

/* Provide Service Principal names as macros */
#ವ್ಯಾಖ್ಯಾನಿಸು SP_NAME_DIGEST              "WDigest"
#ವ್ಯಾಖ್ಯಾನಿಸು SP_NAME_NTLM                "NTLM"
#ವ್ಯಾಖ್ಯಾನಿಸು SP_NAME_NEGOTIATE           "Negotiate"
#ವ್ಯಾಖ್ಯಾನಿಸು SP_NAME_KERBEROS            "Kerberos"

/* Offered by mingw-w64 v9+, MS SDK 7.0A/VS2010+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ISC_REQ_USE_HTTP_STYLE
#ವ್ಯಾಖ್ಯಾನಿಸು ISC_REQ_USE_HTTP_STYLE                0x01000000
#ಆದರೆ_ಅಂತ್ಯ

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SEC_E_INVALID_PARAMETER
#ವ್ಯಾಖ್ಯಾನಿಸು SEC_E_INVALID_PARAMETER               ((HRESULT)0x8009035DL)
#ಆದರೆ_ಅಂತ್ಯ
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SEC_E_DELEGATION_POLICY
#ವ್ಯಾಖ್ಯಾನಿಸು SEC_E_DELEGATION_POLICY               ((HRESULT)0x8009035EL)
#ಆದರೆ_ಅಂತ್ಯ
/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SEC_E_POLICY_NLTM_ONLY
#ವ್ಯಾಖ್ಯಾನಿಸು SEC_E_POLICY_NLTM_ONLY                ((HRESULT)0x8009035FL)
#ಆದರೆ_ಅಂತ್ಯ

/* Offered by mingw-w64 v8+, MS SDK 6.0A/VS2008+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SEC_I_SIGNATURE_NEEDED
#ವ್ಯಾಖ್ಯಾನಿಸು SEC_I_SIGNATURE_NEEDED                ((HRESULT)0x0009035CL)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Definitions required from ntsecapi.h are directly provided below this point
 * to avoid including ntsecapi.h due to a conflict with OpenSSL's safestack.h
 */
#ವ್ಯಾಖ್ಯಾನಿಸು KERB_WRAP_NO_ENCRYPT 0x80000001

#ಆದರೆ_ಅಂತ್ಯ /* USE_WINDOWS_SSPI */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SSPI_H */
