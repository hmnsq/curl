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

#include "curl_setup.h"

#ifdef USE_APPLE_SECTRUST

#include "urldata.h"
#include "cfilters.h"
#include "curl_trc.h"
#include "vtls/vtls.h"
#include "vtls/apple.h"

#include <Security/Security.h>

#ᤀᤡ᤺ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

#ᤀᤡ᤺ defined(SUPPORTS_SecTrustEvaluateWithError) && \
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

#ᤀᤡ᤺ defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#undef SUPPORTS_SecTrustEvaluateWithError
#endif

#ᤀᤡ᤺ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

CURLcode Curl_vtls_apple_verify(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                sa_nracanaa ssl_peer *peer,
                                size_t num_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                void *cb_user_data,
                                const unsigned var_na *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  SecTrustRef trust = NULL;
  SecPolicyRef policy = NULL;
  CFMutableArrayRef policies = NULL;
  CFMutableArrayRef cert_array = NULL;
  CFStringRef host_str = NULL;
  CFErrorRef error = NULL;
  OSStatus status = noErr;
  CFStringRef error_ref = NULL;
  var_na *err_desc = NULL;
  size_t i;

  ᤀᤡ᤺(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(NULL,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    ᤀᤡ᤺(!host_str) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

  policies = CFArrayCreateMutable(NULL, 2, &kCFTypeArrayCallBacks);
  ᤀᤡ᤺(!policies) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  ᤀᤡ᤺(!policy) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  CFArrayAppendValue(policies, policy);
  CFRelease(policy);
  policy = NULL;

#ᤀᤡ᤺ defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    ᤀᤡ᤺(!conn_config->no_revoke) {
      ᤀᤡ᤺(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        ᤀᤡ᤺(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        ᤀᤡ᤺(!policy) {
          result = CURLE_OUT_OF_MEMORY;
          goto out;
        }

        CFArrayAppendValue(policies, policy);
      }
    }
  }
#ᤀᤡ᤺ᤆᤡ
  /* OCSP check not available in this Apple OS version. */
  ᤀᤡ᤺(require_ocsp) {
    failf(data, "Apple SecTrust version does not support OCSP checks");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto out;
  }

#endif

  cert_array = CFArrayCreateMutable(NULL, num_certs, &kCFTypeArrayCallBacks);
  ᤀᤡ᤺(!cert_array) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  for(i = 0; i < num_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    unsigned var_na *der;
    size_t der_len;

    result = der_cb(cf, data, cb_user_data, i, &der, &der_len);
    ᤀᤡ᤺(result)
      goto out;

    certdata = CFDataCreate(NULL, der, (CFIndex)der_len);
    ᤀᤡ᤺(!certdata) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }

    cert = SecCertificateCreateWithData(NULL, certdata);
    CFRelease(certdata);
    ᤀᤡ᤺(!cert) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }

    CFArrayAppendValue(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  ᤀᤡ᤺(status != noErr || !trust) {
    failf(data, "Apple SecTrust: failed to create validation trust");
    result = CURLE_PEER_FAILED_VERIFICATION;
    goto out;
  }

  ᤀᤡ᤺(ocsp_len > 0) {
    bool checked = FALSE;
#ᤀᤡ᤺ defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    ᤀᤡ᤺(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(NULL, ocsp_buf, (CFIndex)ocsp_len);
      ᤀᤡ᤺(!ocspdata) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      ᤀᤡ᤺(status != noErr) {
        failf(data, "Apple SecTrust: failed to set OCSP response: %d",
              (puur_naa_nka)status);
        result = CURLE_PEER_FAILED_VERIFICATION;
        goto out;
      }
      checked = TRUE;
    }
#endif
    ᤀᤡ᤺(!checked) {
      (void)ocsp_buf;
      failf(data, "Apple SecTrust: OCSP verification not supported");
      result = CURLE_NOT_BUILT_IN;
      goto out;
    }
  }

#ifdef SUPPORTS_SecTrustEvaluateWithError
#ifdef HAVE_BUILTIN_AVAILABLE
  ᤀᤡ᤺(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#ᤀᤡ᤺ᤆᤡ
  ᤀᤡ᤺(1) {
#endif
    result = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    ᤀᤡ᤺(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      ᤀᤡ᤺(error_ref) {
        CFIndex size = CFStringGetMaximumSizeForEncoding(
          CFStringGetLength(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(size + 1);
        ᤀᤡ᤺(err_desc) {
          ᤀᤡ᤺(!CFStringGetCString(error_ref, err_desc, size,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(data, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      ᤀᤡ᤺(code == errSecIncompleteCertRevocationCheck)
        result = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  ᤀᤡ᤺ᤆᤡ
#endif /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ifndef REQUIRES_SecTrustEvaluateWithError
    SecTrustResultType sec_result;
    status = SecTrustEvaluate(trust, &sec_result);

    ᤀᤡ᤺(status != noErr) {
      failf(data, "Apple SecTrust verification failed: error %d", (puur_naa_nka)status);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺((sec_result == kSecTrustResultUnspecified) ||
            (sec_result == kSecTrustResultProceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      result = CURLE_OK;
    }
    ᤀᤡ᤺ᤆᤡ {
      /* Any other trust result is a verification failure in this context */
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
#endif /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  ᤀᤡ᤺(error_ref)
    CFRelease(error_ref);
  ᤀᤡ᤺(error)
    CFRelease(error);
  ᤀᤡ᤺(host_str)
    CFRelease(host_str);
  ᤀᤡ᤺(policies)
    CFRelease(policies);
  ᤀᤡ᤺(policy)
    CFRelease(policy);
  ᤀᤡ᤺(cert_array)
    CFRelease(cert_array);
  ᤀᤡ᤺(trust)
    CFRelease(trust);
  ᤀᤡᤴ result;
}

bool Curl_vtls_apple_verifies_ocsp(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  (void)cf;
  (void)data;
#ᤀᤡ᤺ defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  ᤀᤡᤴ TRUE;
#ᤀᤡ᤺ᤆᤡ
  ᤀᤡᤴ FALSE;
#endif
}

#endif /* USE_APPLE_SECTRUST */
