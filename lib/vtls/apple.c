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

#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_APPLE_SECTRUST

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/apple.h"

#ଅନ୍ତର୍ଭାଵ <Security/Security.h>

#ଯଦି (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

#ଯଦି defined(SUPPORTS_SecTrustEvaluateWithError) && \
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

#ଯଦି defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#undef SUPPORTS_SecTrustEvaluateWithError
#endif

#ଯଦି (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

CURLcode Curl_vtls_apple_verify(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ssl_peer *peer,
                                size_t ଅଙ୍କ_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                ଶୂନ୍ୟ *cb_user_data,
                                ଅଚଳ unsigned ଅକ୍ଷର *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  SecTrustRef trust = ଖାଲି;
  SecPolicyRef policy = ଖାଲି;
  CFMutableArrayRef policies = ଖାଲି;
  CFMutableArrayRef cert_array = ଖାଲି;
  CFStringRef host_str = ଖାଲି;
  CFErrorRef error = ଖାଲି;
  OSStatus status = noErr;
  CFStringRef error_ref = ଖାଲି;
  ଅକ୍ଷର *err_desc = ଖାଲି;
  size_t i;

  ଯଦି(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(ଖାଲି,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    ଯଦି(!host_str) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }

  policies = CFArrayCreateMutable(ଖାଲି, 2, &kCFTypeArrayCallBacks);
  ଯଦି(!policies) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  ଯଦି(!policy) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  CFArray_Append_ମୂଲ୍ୟ(policies, policy);
  CFRelease(policy);
  policy = ଖାଲି;

#ଯଦି defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    ଯଦି(!conn_config->no_revoke) {
      ଯଦି(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        ଯଦି(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        ଯଦି(!policy) {
          result = CURLE_OUT_OF_MEMORY;
          ଯାଅ out;
        }

        CFArray_Append_ମୂଲ୍ୟ(policies, policy);
      }
    }
  }
#ଅନ୍ୟଥା
  /* OCSP check not available in this Apple OS version. */
  ଯଦି(require_ocsp) {
    failf(data, "Apple SecTrust version does not support OCSP checks");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ out;
  }

#endif

  cert_array = CFArrayCreateMutable(ଖାଲି, ଅଙ୍କ_certs, &kCFTypeArrayCallBacks);
  ଯଦି(!cert_array) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  ପାଇଁ(i = 0; i < ଅଙ୍କ_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    unsigned ଅକ୍ଷର *der;
    size_t der_len;

    result = der_cb(cf, data, cb_user_data, i, &der, &der_len);
    ଯଦି(result)
      ଯାଅ out;

    certdata = CFDataCreate(ଖାଲି, der, (CFIndex)der_len);
    ଯଦି(!certdata) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }

    cert = SecCertificateCreateWithData(ଖାଲି, certdata);
    CFRelease(certdata);
    ଯଦି(!cert) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }

    CFArray_Append_ମୂଲ୍ୟ(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  ଯଦି(status != noErr || !trust) {
    failf(data, "Apple SecTrust: failed to create validation trust");
    result = CURLE_PEER_FAILED_VERIFICATION;
    ଯାଅ out;
  }

  ଯଦି(ocsp_len > 0) {
    bool checked = FALSE;
#ଯଦି defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    ଯଦି(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(ଖାଲି, ocsp_buf, (CFIndex)ocsp_len);
      ଯଦି(!ocspdata) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      ଯଦି(status != noErr) {
        failf(data, "Apple SecTrust: failed to set OCSP response: %d",
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)status);
        result = CURLE_PEER_FAILED_VERIFICATION;
        ଯାଅ out;
      }
      checked = TRUE;
    }
#endif
    ଯଦି(!checked) {
      (ଶୂନ୍ୟ)ocsp_buf;
      failf(data, "Apple SecTrust: OCSP verification not supported");
      result = CURLE_NOT_BUILT_IN;
      ଯାଅ out;
    }
  }

#ifdef SUPPORTS_SecTrustEvaluateWithError
#ifdef HAVE_BUILTIN_AVAILABLE
  ଯଦି(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#ଅନ୍ୟଥା
  ଯଦି(1) {
#endif
    result = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    ଯଦି(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      ଯଦି(error_ref) {
        CFIndex size = CFStringGetMaximumSizeForEncoding(
          CFStringGetLength(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(size + 1);
        ଯଦି(err_desc) {
          ଯଦି(!CFStringGetCString(error_ref, err_desc, size,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(data, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      ଯଦି(code == errSecIncompleteCertRevocationCheck)
        result = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  ଅନ୍ୟଥା
#endif /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ifndef REQUIRES_SecTrustEvaluateWithError
    SecTrustResultType sec_result;
    status = SecTrustEvaluate(trust, &sec_result);

    ଯଦି(status != noErr) {
      failf(data, "Apple SecTrust verification failed: error %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)status);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    ଅନ୍ୟଥା ଯଦି((sec_result == kSecTrustResultUnspecified) ||
            (sec_result == kSecTrustResultProceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      result = CURLE_OK;
    }
    ଅନ୍ୟଥା {
      /* Any other trust result is a verification failure in this context */
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
#endif /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  ଯଦି(error_ref)
    CFRelease(error_ref);
  ଯଦି(error)
    CFRelease(error);
  ଯଦି(host_str)
    CFRelease(host_str);
  ଯଦି(policies)
    CFRelease(policies);
  ଯଦି(policy)
    CFRelease(policy);
  ଯଦି(cert_array)
    CFRelease(cert_array);
  ଯଦି(trust)
    CFRelease(trust);
  ଫେରନ୍ତୁ result;
}

bool Curl_vtls_apple_verifies_ocsp(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
#ଯଦି defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  ଫେରନ୍ତୁ TRUE;
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ FALSE;
#endif
}

#endif /* USE_APPLE_SECTRUST */
