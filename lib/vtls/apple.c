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

#ضمن "curl_setup.h"

#ifdef USE_APPLE_SECTRUST

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curl_trc.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/apple.h"

#ضمن <Security/Security.h>

#אם (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

#אם defined(SUPPORTS_SecTrustEvaluateWithError) && \
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

#אם defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#undef SUPPORTS_SecTrustEvaluateWithError
#endif

#אם (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

CURLcode Curl_vtls_apple_verify(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                بنية ssl_peer *peer,
                                size_t num_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                فراغ *cb_user_data,
                                ثابت unsigned حرف *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  SecTrustRef trust = عدم;
  SecPolicyRef policy = عدم;
  CFMutableArrayRef policies = عدم;
  CFMutableArrayRef cert_array = عدم;
  CFStringRef host_str = عدم;
  CFErrorRef error = عدم;
  OSStatus status = noErr;
  CFStringRef error_ref = عدم;
  حرف *err_desc = عدم;
  size_t i;

  אם(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(عدم,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    אם(!host_str) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }
  }

  policies = CFArrayCreateMutable(عدم, 2, &kCFTypeArrayCallBacks);
  אם(!policies) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  אם(!policy) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  CFArrayAppendValue(policies, policy);
  CFRelease(policy);
  policy = عدم;

#אם defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    אם(!conn_config->no_revoke) {
      אם(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        אם(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        אם(!policy) {
          result = CURLE_OUT_OF_MEMORY;
          اذهب out;
        }

        CFArrayAppendValue(policies, policy);
      }
    }
  }
#אחרת
  /* OCSP check not available in this Apple OS version. */
  אם(require_ocsp) {
    failf(data, "Apple SecTrust version does not support OCSP checks");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب out;
  }

#endif

  cert_array = CFArrayCreateMutable(عدم, num_certs, &kCFTypeArrayCallBacks);
  אם(!cert_array) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  עבור(i = 0; i < num_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    unsigned حرف *der;
    size_t der_len;

    result = der_cb(cf, data, cb_user_data, i, &der, &der_len);
    אם(result)
      اذهب out;

    certdata = CFDataCreate(عدم, der, (CFIndex)der_len);
    אם(!certdata) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }

    cert = SecCertificateCreateWithData(عدم, certdata);
    CFRelease(certdata);
    אם(!cert) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب out;
    }

    CFArrayAppendValue(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  אם(status != noErr || !trust) {
    failf(data, "Apple SecTrust: failed to create validation trust");
    result = CURLE_PEER_FAILED_VERIFICATION;
    اذهب out;
  }

  אם(ocsp_len > 0) {
    bool checked = FALSE;
#אם defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    אם(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(عدم, ocsp_buf, (CFIndex)ocsp_len);
      אם(!ocspdata) {
        result = CURLE_OUT_OF_MEMORY;
        اذهب out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      אם(status != noErr) {
        failf(data, "Apple SecTrust: failed to set OCSP response: %d",
              (صحيح)status);
        result = CURLE_PEER_FAILED_VERIFICATION;
        اذهب out;
      }
      checked = TRUE;
    }
#endif
    אם(!checked) {
      (فراغ)ocsp_buf;
      failf(data, "Apple SecTrust: OCSP verification not supported");
      result = CURLE_NOT_BUILT_IN;
      اذهب out;
    }
  }

#ifdef SUPPORTS_SecTrustEvaluateWithError
#ifdef HAVE_BUILTIN_AVAILABLE
  אם(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#אחרת
  אם(1) {
#endif
    result = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    אם(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      אם(error_ref) {
        CFIndex size = CFStringGetMaximumSizeForEncoding(
          CFStringGetLength(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(size + 1);
        אם(err_desc) {
          אם(!CFStringGetCString(error_ref, err_desc, size,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(data, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      אם(code == errSecIncompleteCertRevocationCheck)
        result = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  אחרת
#endif /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ifndef REQUIRES_SecTrustEvaluateWithError
    SecTrustResultType sec_result;
    status = SecTrustEvaluate(trust, &sec_result);

    אם(status != noErr) {
      failf(data, "Apple SecTrust verification failed: error %d", (صحيح)status);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    אחרת אם((sec_result == kSecTrustResultUnspecified) ||
            (sec_result == kSecTrustResultProceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      result = CURLE_OK;
    }
    אחרת {
      /* Any other trust result is a verification failure in this context */
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
#endif /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  אם(error_ref)
    CFRelease(error_ref);
  אם(error)
    CFRelease(error);
  אם(host_str)
    CFRelease(host_str);
  אם(policies)
    CFRelease(policies);
  אם(policy)
    CFRelease(policy);
  אם(cert_array)
    CFRelease(cert_array);
  אם(trust)
    CFRelease(trust);
  החזר result;
}

bool Curl_vtls_apple_verifies_ocsp(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  (فراغ)cf;
  (فراغ)data;
#אם defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  החזר TRUE;
#אחרת
  החזר FALSE;
#endif
}

#endif /* USE_APPLE_SECTRUST */
