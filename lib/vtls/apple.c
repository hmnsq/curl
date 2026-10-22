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

#अन्तर्भाव "curl_setup.h"

#ifdef USE_APPLE_SECTRUST

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/apple.h"

#अन्तर्भाव <Security/Security.h>

#यदि (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

#यदि defined(SUPPORTS_SecTrustEvaluateWithError) && \
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

#यदि defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#undef SUPPORTS_SecTrustEvaluateWithError
#endif

#यदि (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

CURLcode Curl_vtls_apple_verify(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                संरचना ssl_peer *peer,
                                माप_प्रकार अङ्क_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                शून्य *cb_user_data,
                                अचर अचिन्हित वर्ण *ocsp_buf,
                                माप_प्रकार ocsp_len,
                                bool require_ocsp)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  SecTrustRef trust = रिक्त;
  SecPolicyRef policy = रिक्त;
  CFMutableArrayRef policies = रिक्त;
  CFMutableArrayRef cert_array = रिक्त;
  CFStringRef host_str = रिक्त;
  CFErrorRef error = रिक्त;
  OSStatus status = noErr;
  CFStringRef error_ref = रिक्त;
  वर्ण *err_desc = रिक्त;
  माप_प्रकार i;

  यदि(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(रिक्त,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    यदि(!host_str) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }
  }

  policies = CFArrayCreateMutable(रिक्त, 2, &kCFTypeArrayCallBacks);
  यदि(!policies) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  यदि(!policy) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  CFArray_Append_मान(policies, policy);
  CFRelease(policy);
  policy = रिक्त;

#यदि defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    यदि(!conn_config->no_revoke) {
      यदि(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        यदि(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        यदि(!policy) {
          result = CURLE_OUT_OF_MEMORY;
          गच्छ out;
        }

        CFArray_Append_मान(policies, policy);
      }
    }
  }
#अन्यथा
  /* OCSP check not available in this Apple OS version. */
  यदि(require_ocsp) {
    failf(data, "Apple SecTrust version does not support OCSP checks");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ out;
  }

#endif

  cert_array = CFArrayCreateMutable(रिक्त, अङ्क_certs, &kCFTypeArrayCallBacks);
  यदि(!cert_array) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  लागि(i = 0; i < अङ्क_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    अचिन्हित वर्ण *der;
    माप_प्रकार der_len;

    result = der_cb(cf, data, cb_user_data, i, &der, &der_len);
    यदि(result)
      गच्छ out;

    certdata = CFDataCreate(रिक्त, der, (CFIndex)der_len);
    यदि(!certdata) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }

    cert = SecCertificateCreateWithData(रिक्त, certdata);
    CFRelease(certdata);
    यदि(!cert) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ out;
    }

    CFArray_Append_मान(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  यदि(status != noErr || !trust) {
    failf(data, "Apple SecTrust: failed to create validation trust");
    result = CURLE_PEER_FAILED_VERIFICATION;
    गच्छ out;
  }

  यदि(ocsp_len > 0) {
    bool checked = FALSE;
#यदि defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    यदि(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(रिक्त, ocsp_buf, (CFIndex)ocsp_len);
      यदि(!ocspdata) {
        result = CURLE_OUT_OF_MEMORY;
        गच्छ out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      यदि(status != noErr) {
        failf(data, "Apple SecTrust: failed to set OCSP response: %d",
              (पूर्णाङ्क)status);
        result = CURLE_PEER_FAILED_VERIFICATION;
        गच्छ out;
      }
      checked = TRUE;
    }
#endif
    यदि(!checked) {
      (शून्य)ocsp_buf;
      failf(data, "Apple SecTrust: OCSP verification not supported");
      result = CURLE_NOT_BUILT_IN;
      गच्छ out;
    }
  }

#ifdef SUPPORTS_SecTrustEvaluateWithError
#ifdef HAVE_BUILTIN_AVAILABLE
  यदि(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#अन्यथा
  यदि(1) {
#endif
    result = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    यदि(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      यदि(error_ref) {
        CFIndex size = CFStringGetMaximumSizeForEncoding(
          CFStringGetLength(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(size + 1);
        यदि(err_desc) {
          यदि(!CFStringGetCString(error_ref, err_desc, size,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(data, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      यदि(code == errSecIncompleteCertRevocationCheck)
        result = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  अन्यथा
#endif /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ifndef REQUIRES_SecTrustEvaluateWithError
    SecTrustResultType sec_result;
    status = SecTrustEvaluate(trust, &sec_result);

    यदि(status != noErr) {
      failf(data, "Apple SecTrust verification failed: error %d", (पूर्णाङ्क)status);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    अन्यथा यदि((sec_result == kSecTrustResultUnspecified) ||
            (sec_result == kSecTrustResultProceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      result = CURLE_OK;
    }
    अन्यथा {
      /* Any other trust result is a verification failure in this context */
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
#endif /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  यदि(error_ref)
    CFRelease(error_ref);
  यदि(error)
    CFRelease(error);
  यदि(host_str)
    CFRelease(host_str);
  यदि(policies)
    CFRelease(policies);
  यदि(policy)
    CFRelease(policy);
  यदि(cert_array)
    CFRelease(cert_array);
  यदि(trust)
    CFRelease(trust);
  फर्काऊ result;
}

bool Curl_vtls_apple_verifies_ocsp(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  (शून्य)cf;
  (शून्य)data;
#यदि defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  फर्काऊ TRUE;
#अन्यथा
  फर्काऊ FALSE;
#endif
}

#endif /* USE_APPLE_SECTRUST */
