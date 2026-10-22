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

#شامل "curl_setup.h"

#ifdef USE_APPLE_SECTRUST

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "curl_trc.h"
#شامل "vtls/vtls.h"
#شامل "vtls/apple.h"

#شامل <Security/Security.h>

#اگر (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

#اگر defined(SUPPORTS_SecTrustEvaluateWithError) && \
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

#اگر defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#undef SUPPORTS_SecTrustEvaluateWithError
#endif

#اگر (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

CURLcode Curl_vtls_apple_verify(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                ساختار ssl_peer *peer,
                                size_t رقم_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                تهی *cb_user_data,
                                ثابت unsigned نویسه *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  SecTrustRef trust = پوچ;
  SecPolicyRef policy = پوچ;
  CFMutableArrayRef policies = پوچ;
  CFMutableArrayRef cert_array = پوچ;
  CFStringRef host_str = پوچ;
  CFErrorRef error = پوچ;
  OSStatus status = noErr;
  CFStringRef error_ref = پوچ;
  نویسه *err_desc = پوچ;
  size_t i;

  اگر(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(پوچ,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    اگر(!host_str) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }

  policies = CFArrayCreateMutable(پوچ, 2, &kCFTypeArrayCallBacks);
  اگر(!policies) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  اگر(!policy) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  CFArray_Append_مقدار(policies, policy);
  CFRelease(policy);
  policy = پوچ;

#اگر defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    اگر(!conn_config->no_revoke) {
      اگر(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        اگر(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        اگر(!policy) {
          result = CURLE_OUT_OF_MEMORY;
          برو out;
        }

        CFArray_Append_مقدار(policies, policy);
      }
    }
  }
#وگرنه
  /* OCSP check not available in this Apple OS version. */
  اگر(require_ocsp) {
    failf(data, "Apple SecTrust version does not support OCSP checks");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو out;
  }

#endif

  cert_array = CFArrayCreateMutable(پوچ, رقم_certs, &kCFTypeArrayCallBacks);
  اگر(!cert_array) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  برای(i = 0; i < رقم_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    unsigned نویسه *der;
    size_t der_len;

    result = der_cb(cf, data, cb_user_data, i, &der, &der_len);
    اگر(result)
      برو out;

    certdata = CFDataCreate(پوچ, der, (CFIndex)der_len);
    اگر(!certdata) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }

    cert = SecCertificateCreateWithData(پوچ, certdata);
    CFRelease(certdata);
    اگر(!cert) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }

    CFArray_Append_مقدار(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  اگر(status != noErr || !trust) {
    failf(data, "Apple SecTrust: failed to create validation trust");
    result = CURLE_PEER_FAILED_VERIFICATION;
    برو out;
  }

  اگر(ocsp_len > 0) {
    bool checked = FALSE;
#اگر defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    اگر(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(پوچ, ocsp_buf, (CFIndex)ocsp_len);
      اگر(!ocspdata) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      اگر(status != noErr) {
        failf(data, "Apple SecTrust: failed to set OCSP response: %d",
              (صحیح)status);
        result = CURLE_PEER_FAILED_VERIFICATION;
        برو out;
      }
      checked = TRUE;
    }
#endif
    اگر(!checked) {
      (تهی)ocsp_buf;
      failf(data, "Apple SecTrust: OCSP verification not supported");
      result = CURLE_NOT_BUILT_IN;
      برو out;
    }
  }

#ifdef SUPPORTS_SecTrustEvaluateWithError
#ifdef HAVE_BUILTIN_AVAILABLE
  اگر(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#وگرنه
  اگر(1) {
#endif
    result = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    اگر(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      اگر(error_ref) {
        CFIndex size = CFStringGetMaximumSizeForEncoding(
          CFStringGetLength(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(size + 1);
        اگر(err_desc) {
          اگر(!CFStringGetCString(error_ref, err_desc, size,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(data, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      اگر(code == errSecIncompleteCertRevocationCheck)
        result = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  وگرنه
#endif /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ifndef REQUIRES_SecTrustEvaluateWithError
    SecTrustResultType sec_result;
    status = SecTrustEvaluate(trust, &sec_result);

    اگر(status != noErr) {
      failf(data, "Apple SecTrust verification failed: error %d", (صحیح)status);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    وگرنه اگر((sec_result == kSecTrustResultUnspecified) ||
            (sec_result == kSecTrustResultProceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      result = CURLE_OK;
    }
    وگرنه {
      /* Any other trust result is a verification failure in this context */
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
#endif /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  اگر(error_ref)
    CFRelease(error_ref);
  اگر(error)
    CFRelease(error);
  اگر(host_str)
    CFRelease(host_str);
  اگر(policies)
    CFRelease(policies);
  اگر(policy)
    CFRelease(policy);
  اگر(cert_array)
    CFRelease(cert_array);
  اگر(trust)
    CFRelease(trust);
  بازگشت result;
}

bool Curl_vtls_apple_verifies_ocsp(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  (تهی)cf;
  (تهی)data;
#اگر defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  بازگشت TRUE;
#وگرنه
  بازگشت FALSE;
#endif
}

#endif /* USE_APPLE_SECTRUST */
