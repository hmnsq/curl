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

#অন্তর্ভাব "curl_setup.h"

#ifdef USE_APPLE_SECTRUST

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/apple.h"

#অন্তর্ভাব <Security/Security.h>

#যদি (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

#যদি defined(SUPPORTS_SecTrustEvaluateWithError) && \
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

#যদি defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#undef SUPPORTS_SecTrustEvaluateWithError
#endif

#যদি (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

CURLcode Curl_vtls_apple_verify(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                কাঠামো ssl_peer *peer,
                                size_t অঙ্ক_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                শূন্য *cb_user_তথ্য,
                                ধ্রুবক unsigned অক্ষর *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ফলাফল = CURLE_OK;
  SecTrustRef trust = ফাঁকা;
  SecPolicyRef policy = ফাঁকা;
  CFMutableArrayRef policies = ফাঁকা;
  CFMutableArrayRef cert_array = ফাঁকা;
  CFStringRef host_str = ফাঁকা;
  CFErrorRef error = ফাঁকা;
  OSStatus status = noErr;
  CFStringRef error_ref = ফাঁকা;
  অক্ষর *err_desc = ফাঁকা;
  size_t i;

  যদি(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(ফাঁকা,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    যদি(!host_str) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }

  policies = CFArrayCreateMutable(ফাঁকা, 2, &kCFTypeArrayCallBacks);
  যদি(!policies) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  যদি(!policy) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  CFArray_Append_মান(policies, policy);
  CFRelease(policy);
  policy = ফাঁকা;

#যদি defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    যদি(!conn_config->no_revoke) {
      যদি(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        যদি(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        যদি(!policy) {
          ফলাফল = CURLE_OUT_OF_MEMORY;
          যাও out;
        }

        CFArray_Append_মান(policies, policy);
      }
    }
  }
#অন্যথা
  /* OCSP check not available in this Apple OS version. */
  যদি(require_ocsp) {
    failf(তথ্য, "Apple SecTrust version does not support OCSP checks");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও out;
  }

#endif

  cert_array = CFArrayCreateMutable(ফাঁকা, অঙ্ক_certs, &kCFTypeArrayCallBacks);
  যদি(!cert_array) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  জন্য(i = 0; i < অঙ্ক_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    unsigned অক্ষর *der;
    size_t der_len;

    ফলাফল = der_cb(cf, তথ্য, cb_user_তথ্য, i, &der, &der_len);
    যদি(ফলাফল)
      যাও out;

    certdata = CFDataCreate(ফাঁকা, der, (CFIndex)der_len);
    যদি(!certdata) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }

    cert = Sec_Certificate_Create_With_তথ্য(ফাঁকা, certdata);
    CFRelease(certdata);
    যদি(!cert) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }

    CFArray_Append_মান(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  যদি(status != noErr || !trust) {
    failf(তথ্য, "Apple SecTrust: failed to create validation trust");
    ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    যাও out;
  }

  যদি(ocsp_len > 0) {
    bool checked = FALSE;
#যদি defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    যদি(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(ফাঁকা, ocsp_buf, (CFIndex)ocsp_len);
      যদি(!ocspdata) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      যদি(status != noErr) {
        failf(তথ্য, "Apple SecTrust: failed to set OCSP response: %d",
              (পূর্ণসংখ্যা)status);
        ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
        যাও out;
      }
      checked = TRUE;
    }
#endif
    যদি(!checked) {
      (শূন্য)ocsp_buf;
      failf(তথ্য, "Apple SecTrust: OCSP verification not supported");
      ফলাফল = CURLE_NOT_BUILT_IN;
      যাও out;
    }
  }

#ifdef SUPPORTS_SecTrustEvaluateWithError
#ifdef HAVE_BUILTIN_AVAILABLE
  যদি(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#অন্যথা
  যদি(1) {
#endif
    ফলাফল = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    যদি(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      যদি(error_ref) {
        CFIndex আকার_2 = CFString_Get_Maximum_আকার_For_Encoding(
          CFString_Get_দৈর্ঘ্য(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(আকার_2 + 1);
        যদি(err_desc) {
          যদি(!CFStringGetCString(error_ref, err_desc, আকার_2,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(তথ্য, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      যদি(code == errSecIncompleteCertRevocationCheck)
        ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  অন্যথা
#endif /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ifndef REQUIRES_SecTrustEvaluateWithError
    Sec_Trust_ফলাফল_Type sec_ফলাফল;
    status = SecTrustEvaluate(trust, &sec_ফলাফল);

    যদি(status != noErr) {
      failf(তথ্য, "Apple SecTrust verification failed: error %d", (পূর্ণসংখ্যা)status);
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    }
    অন্যথা যদি((sec_ফলাফল == k_Sec_Trust_ফলাফল_Unspecified) ||
            (sec_ফলাফল == k_Sec_Trust_ফলাফল_Proceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      ফলাফল = CURLE_OK;
    }
    অন্যথা {
      /* Any other trust result is a verification failure in this context */
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    }
#endif /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  যদি(error_ref)
    CFRelease(error_ref);
  যদি(error)
    CFRelease(error);
  যদি(host_str)
    CFRelease(host_str);
  যদি(policies)
    CFRelease(policies);
  যদি(policy)
    CFRelease(policy);
  যদি(cert_array)
    CFRelease(cert_array);
  যদি(trust)
    CFRelease(trust);
  ফেরত ফলাফল;
}

bool Curl_vtls_apple_verifies_ocsp(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  (শূন্য)cf;
  (শূন্য)তথ্য;
#যদি defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  ফেরত TRUE;
#অন্যথা
  ফেরত FALSE;
#endif
}

#endif /* USE_APPLE_SECTRUST */
