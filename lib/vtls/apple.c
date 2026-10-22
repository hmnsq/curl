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

#ifdef USE_APPLE_SECTRUST

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/apple.h"

#ಅನ್ತರ್ಭಾವ <Security/Security.h>

#ಯದಿ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 101400) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 50000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#define SUPPORTS_SecTrustEvaluateWithError 1
#endif

#ಯದಿ defined(SUPPORTS_SecTrustEvaluateWithError) && \
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
#define REQUIRES_SecTrustEvaluateWithError 1
#endif

#ಯದಿ defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#undef SUPPORTS_SecTrustEvaluateWithError
#endif

#ಯದಿ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 100900) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 70000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 90000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 20000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#define SUPPORTS_SecOCSP 1
#endif

CURLcode Curl_vtls_apple_verify(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ ssl_peer *peer,
                                size_t ಅಂಕೆ_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                ಶೂನ್ಯ *cb_user_data,
                                ಧ್ರುವ unsigned ವರ್ಣ *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  SecTrustRef trust = ರಿಕ್ತ;
  SecPolicyRef policy = ರಿಕ್ತ;
  CFMutableArrayRef policies = ರಿಕ್ತ;
  CFMutableArrayRef cert_array = ರಿಕ್ತ;
  CFStringRef host_str = ರಿಕ್ತ;
  CFErrorRef error = ರಿಕ್ತ;
  OSStatus status = noErr;
  CFStringRef error_ref = ರಿಕ್ತ;
  ವರ್ಣ *err_desc = ರಿಕ್ತ;
  size_t i;

  ಯದಿ(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(ರಿಕ್ತ,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    ಯದಿ(!host_str) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }

  policies = CFArrayCreateMutable(ರಿಕ್ತ, 2, &kCFTypeArrayCallBacks);
  ಯದಿ(!policies) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  ಯದಿ(!policy) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  CFArray_Append_ಮೌಲ್ಯ(policies, policy);
  CFRelease(policy);
  policy = ರಿಕ್ತ;

#ಯದಿ defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    ಯದಿ(!conn_config->no_revoke) {
      ಯದಿ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        ಯದಿ(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        ಯದಿ(!policy) {
          result = CURLE_OUT_OF_MEMORY;
          ಗಚ್ಛ out;
        }

        CFArray_Append_ಮೌಲ್ಯ(policies, policy);
      }
    }
  }
#ಅನ್ಯಥಾ
  /* OCSP check not available in this Apple OS version. */
  ಯದಿ(require_ocsp) {
    failf(data, "Apple SecTrust version does not support OCSP checks");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ out;
  }

#endif

  cert_array = CFArrayCreateMutable(ರಿಕ್ತ, ಅಂಕೆ_certs, &kCFTypeArrayCallBacks);
  ಯದಿ(!cert_array) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ಪ್ರತಿ(i = 0; i < ಅಂಕೆ_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    unsigned ವರ್ಣ *der;
    size_t der_len;

    result = der_cb(cf, data, cb_user_data, i, &der, &der_len);
    ಯದಿ(result)
      ಗಚ್ಛ out;

    certdata = CFDataCreate(ರಿಕ್ತ, der, (CFIndex)der_len);
    ಯದಿ(!certdata) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }

    cert = SecCertificateCreateWithData(ರಿಕ್ತ, certdata);
    CFRelease(certdata);
    ಯದಿ(!cert) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }

    CFArray_Append_ಮೌಲ್ಯ(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  ಯದಿ(status != noErr || !trust) {
    failf(data, "Apple SecTrust: failed to create validation trust");
    result = CURLE_PEER_FAILED_VERIFICATION;
    ಗಚ್ಛ out;
  }

  ಯದಿ(ocsp_len > 0) {
    bool checked = FALSE;
#ಯದಿ defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    ಯದಿ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(ರಿಕ್ತ, ocsp_buf, (CFIndex)ocsp_len);
      ಯದಿ(!ocspdata) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      ಯದಿ(status != noErr) {
        failf(data, "Apple SecTrust: failed to set OCSP response: %d",
              (ಪೂರ್ಣಾಙ್ಕ)status);
        result = CURLE_PEER_FAILED_VERIFICATION;
        ಗಚ್ಛ out;
      }
      checked = TRUE;
    }
#endif
    ಯದಿ(!checked) {
      (ಶೂನ್ಯ)ocsp_buf;
      failf(data, "Apple SecTrust: OCSP verification not supported");
      result = CURLE_NOT_BUILT_IN;
      ಗಚ್ಛ out;
    }
  }

#ifdef SUPPORTS_SecTrustEvaluateWithError
#ifdef HAVE_BUILTIN_AVAILABLE
  ಯದಿ(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#ಅನ್ಯಥಾ
  ಯದಿ(1) {
#endif
    result = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    ಯದಿ(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      ಯದಿ(error_ref) {
        CFIndex size = CFStringGetMaximumSizeForEncoding(
          CFStringGetLength(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(size + 1);
        ಯದಿ(err_desc) {
          ಯದಿ(!CFStringGetCString(error_ref, err_desc, size,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(data, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      ಯದಿ(code == errSecIncompleteCertRevocationCheck)
        result = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  ಅನ್ಯಥಾ
#endif /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ifndef REQUIRES_SecTrustEvaluateWithError
    SecTrustResultType sec_result;
    status = SecTrustEvaluate(trust, &sec_result);

    ಯದಿ(status != noErr) {
      failf(data, "Apple SecTrust verification failed: error %d", (ಪೂರ್ಣಾಙ್ಕ)status);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    ಅನ್ಯಥಾ ಯದಿ((sec_result == kSecTrustResultUnspecified) ||
            (sec_result == kSecTrustResultProceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      result = CURLE_OK;
    }
    ಅನ್ಯಥಾ {
      /* Any other trust result is a verification failure in this context */
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
#endif /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  ಯದಿ(error_ref)
    CFRelease(error_ref);
  ಯದಿ(error)
    CFRelease(error);
  ಯದಿ(host_str)
    CFRelease(host_str);
  ಯದಿ(policies)
    CFRelease(policies);
  ಯದಿ(policy)
    CFRelease(policy);
  ಯದಿ(cert_array)
    CFRelease(cert_array);
  ಯದಿ(trust)
    CFRelease(trust);
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_vtls_apple_verifies_ocsp(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
#ಯದಿ defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  ಪ್ರತ್ಯಾಗಮ TRUE;
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

#endif /* USE_APPLE_SECTRUST */
