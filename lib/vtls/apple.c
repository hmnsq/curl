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

/* This file is for implementing all "generic" SSL functions that all libcurl
   internals should use. It is then responsible for calling the proper
   "backend" function.

   SSL-functions in libcurl should call functions in this source file, and not
   to any specific SSL-layer.

   Curl_ssl_ - prefix for generic ones

   Note that this source code uses the functions of the configured SSL
   backend via the global Curl_ssl instance.

   "SSL/TLS Strong Encryption: An Introduction"
   https://httpd.apache.org/docs/2.0/ssl/ssl_intro.html
 */

#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/apple.h"

#ಅನ್ತರ್ಭಾವ <Security/Security.h>

#ಆದರೆ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 101400) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 50000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#ವ್ಯಾಖ್ಯಾನಿಸು SUPPORTS_SecTrustEvaluateWithError 1
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(SUPPORTS_SecTrustEvaluateWithError) && \
  ((defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && \
            __MAC_OS_X_VERSION_MIN_REQUIRED >= 101400) || \
   (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && \
            __IPHONE_OS_VERSION_MIN_REQUIRED >= 120000) || \
   (defined(__TV_OS_VERSION_MIN_REQUIRED) && \
            __TV_OS_VERSION_MIN_REQUIRED >= 120000) || \
   (defined(__WATCH_OS_VERSION_MIN_REQUIRED) && \
            __WATCH_OS_VERSION_MIN_REQUIRED >= 50000) || \
   (defined(__VISION_OS_VERSION_MIN_REQUIRED) && \
            __VISION_OS_VERSION_MIN_REQUIRED >= 10000))
#ವ್ಯಾಖ್ಯಾನಿಸು REQUIRES_SecTrustEvaluateWithError 1
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_ಲಭ್ಯ) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು SUPPORTS_SecTrustEvaluateWithError
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 100900) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 70000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 90000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 20000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#ವ್ಯಾಖ್ಯಾನಿಸು SUPPORTS_SecOCSP 1
#ಆದರೆ_ಅಂತ್ಯ

CURLcode Curl_vtls_apple_verify(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ssl_peer *peer,
                                size_t ಅಂಕೆ_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ,
                                ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  SecTrustRef trust = ಖಾಲಿ;
  SecPolicyRef policy = ಖಾಲಿ;
  CFMutableArrayRef policies = ಖಾಲಿ;
  CFMutableArrayRef cert_array = ಖಾಲಿ;
  CFStringRef host_str = ಖಾಲಿ;
  CFErrorRef error = ಖಾಲಿ;
  OSStatus ಸ್ಥಿತಿ_2 = noErr;
  CFStringRef error_ref = ಖಾಲಿ;
  ಅಕ್ಷರ *err_desc = ಖಾಲಿ;
  size_t i;

  ಆದರೆ(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(ಖಾಲಿ,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    ಆದರೆ(!host_str) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  policies = CFArrayCreateMutable(ಖಾಲಿ, 2, &kCFTypeArrayCallBacks);
  ಆದರೆ(!policies) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  ಆದರೆ(!policy) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  CFArray_Append_ಮೌಲ್ಯ(policies, policy);
  CFRelease(policy);
  policy = ಖಾಲಿ;

#ಆದರೆ defined(HAVE_BUILTIN_ಲಭ್ಯ) && defined(SUPPORTS_SecOCSP)
  {
    ಆದರೆ(!conn_config->no_revoke) {
      ಆದರೆ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = k_Sec_Revocation_Use_Any_ಲಭ್ಯ_ವಿಧಾನ;
        ಆದರೆ(require_ocsp)
          revocation_flags |= k_Sec_Revocation_Require_ಧನಾತ್ಮಕ_Response;
        policy = SecPolicyCreateRevocation(revocation_flags);
        ಆದರೆ(!policy) {
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
          ಹೋಗು ಹೊರಹೋಗು;
        }

        CFArray_Append_ಮೌಲ್ಯ(policies, policy);
      }
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  /* OCSP check not available in this Apple OS version. */
  ಆದರೆ(require_ocsp) {
    failf(ದತ್ತಾಂಶ, "Apple SecTrust version does not support OCSP checks");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ಆದರೆ_ಅಂತ್ಯ

  cert_array = CFArrayCreateMutable(ಖಾಲಿ, ಅಂಕೆ_certs, &kCFTypeArrayCallBacks);
  ಆದರೆ(!cert_array) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಗಾಗಿ(i = 0; i < ಅಂಕೆ_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *der;
    size_t der_len;

    ಫಲಿತಾಂಶ = der_cb(cf, ದತ್ತಾಂಶ, cb_user_ದತ್ತಾಂಶ, i, &der, &der_len);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    certdata = CFDataCreate(ಖಾಲಿ, der, (CFIndex)der_len);
    ಆದರೆ(!certdata) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    cert = Sec_Certificate_Create_With_ದತ್ತಾಂಶ(ಖಾಲಿ, certdata);
    CFRelease(certdata);
    ಆದರೆ(!cert) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    CFArray_Append_ಮೌಲ್ಯ(cert_array, cert);
    CFRelease(cert);
  }

  ಸ್ಥಿತಿ_2 = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != noErr || !trust) {
    failf(ದತ್ತಾಂಶ, "Apple SecTrust: failed to create validation trust");
    ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(ocsp_len > 0) {
    bool checked = FALSE;
#ಆದರೆ defined(HAVE_BUILTIN_ಲಭ್ಯ) && defined(SUPPORTS_SecOCSP)
    ಆದರೆ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(ಖಾಲಿ, ocsp_buf, (CFIndex)ocsp_len);
      ಆದರೆ(!ocspdata) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಸ್ಥಿತಿ_2 = Sec_Trust_ಗಣ_OCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      ಆದರೆ(ಸ್ಥಿತಿ_2 != noErr) {
        failf(ದತ್ತಾಂಶ, "Apple SecTrust: failed to set OCSP response: %d",
              (ಪೂರ್ಣಾಂಕ)ಸ್ಥಿತಿ_2);
        ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      checked = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(!checked) {
      (ಶೂನ್ಯ)ocsp_buf;
      failf(ದತ್ತಾಂಶ, "Apple SecTrust: OCSP verification not supported");
      ಫಲಿತಾಂಶ = CURLE_NOT_BUILT_ಒಳ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SUPPORTS_SecTrustEvaluateWithError
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BUILTIN_ಲಭ್ಯ
  ಆದರೆ(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(1) {
#ಆದರೆ_ಅಂತ್ಯ
    ಫಲಿತಾಂಶ = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    ಆದರೆ(error) {
      CFIndex ಸಂಕೇತ_2 = CFError_Get_ಸಂಕೇತ(error);
      error_ref = CFErrorCopyDescription(error);

      ಆದರೆ(error_ref) {
        CFIndex ಗಾತ್ರ_2 = CFString_Get_ಗರಿಷ್ಠ_ಗಾತ್ರ_For_Encoding(
          CFString_Get_ಉದ್ದಳತೆ(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(ಗಾತ್ರ_2 + 1);
        ಆದರೆ(err_desc) {
          ಆದರೆ(!CFStringGetCString(error_ref, err_desc, ಗಾತ್ರ_2,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(ದತ್ತಾಂಶ, "Apple SecTrust failure %ld%s%s", ಸಂಕೇತ_2,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      ಆದರೆ(ಸಂಕೇತ_2 == errSecIncompleteCertRevocationCheck)
        ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ REQUIRES_SecTrustEvaluateWithError
    Sec_Trust_ಫಲಿತಾಂಶ_Type sec_ಫಲಿತಾಂಶ;
    ಸ್ಥಿತಿ_2 = SecTrustEvaluate(trust, &sec_ಫಲಿತಾಂಶ);

    ಆದರೆ(ಸ್ಥಿತಿ_2 != noErr) {
      failf(ದತ್ತಾಂಶ, "Apple SecTrust verification failed: error %d", (ಪೂರ್ಣಾಂಕ)ಸ್ಥಿತಿ_2);
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((sec_ಫಲಿತಾಂಶ == k_Sec_Trust_ಫಲಿತಾಂಶ_Unspecified) ||
            (sec_ಫಲಿತಾಂಶ == k_Sec_Trust_ಫಲಿತಾಂಶ_Proceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      ಫಲಿತಾಂಶ = CURLE_OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* Any other trust result is a verification failure in this context */
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    }
#ಆದರೆ_ಅಂತ್ಯ /* REQUIRES_SecTrustEvaluateWithError */
  }

ಹೊರಹೋಗು:
  curlx_free(err_desc);
  ಆದರೆ(error_ref)
    CFRelease(error_ref);
  ಆದರೆ(error)
    CFRelease(error);
  ಆದರೆ(host_str)
    CFRelease(host_str);
  ಆದರೆ(policies)
    CFRelease(policies);
  ಆದರೆ(policy)
    CFRelease(policy);
  ಆದರೆ(cert_array)
    CFRelease(cert_array);
  ಆದರೆ(trust)
    CFRelease(trust);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_vtls_apple_verifies_ocsp(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ defined(HAVE_BUILTIN_ಲಭ್ಯ) && defined(SUPPORTS_SecOCSP)
  ಹಿಂತಿರುಗಿ TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_APPLE_SECTRUST */
