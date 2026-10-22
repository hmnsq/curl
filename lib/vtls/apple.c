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

#यदि_परिभाषितम् USE_APPLE_SECTRUST

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
#परिभाषय SUPPORTS_SecTrustEvaluateWithError 1
#यदि_अन्तः

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
#परिभाषय REQUIRES_SecTrustEvaluateWithError 1
#यदि_अन्तः

#यदि defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_उपलब्धम्) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#अपरिभाषय SUPPORTS_SecTrustEvaluateWithError
#यदि_अन्तः

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
#परिभाषय SUPPORTS_SecOCSP 1
#यदि_अन्तः

CURLcode Curl_vtls_apple_verify(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                संरचना ssl_peer *peer,
                                size_t अङ्कः_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                शून्य *cb_user_दत्त,
                                ध्रुव अचिह्नित वर्ण *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  SecTrustRef trust = रिक्त;
  SecPolicyRef policy = रिक्त;
  CFMutableArrayRef policies = रिक्त;
  CFMutableArrayRef cert_array = रिक्त;
  CFStringRef host_str = रिक्त;
  CFErrorRef error = रिक्त;
  OSStatus स्थितिः = noErr;
  CFStringRef error_ref = रिक्त;
  वर्ण *err_desc = रिक्त;
  size_t i;

  यदि(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(रिक्त,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    यदि(!host_str) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }

  policies = CFArrayCreateMutable(रिक्त, 2, &kCFTypeArrayCallBacks);
  यदि(!policies) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  यदि(!policy) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  CFArray_Append_मानम्(policies, policy);
  CFRelease(policy);
  policy = रिक्त;

#यदि defined(HAVE_BUILTIN_उपलब्धम्) && defined(SUPPORTS_SecOCSP)
  {
    यदि(!conn_config->no_revoke) {
      यदि(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = k_Sec_Revocation_Use_Any_उपलब्धम्_विधानम्;
        यदि(require_ocsp)
          revocation_flags |= k_Sec_Revocation_Require_धनात्मकम्_Response;
        policy = SecPolicyCreateRevocation(revocation_flags);
        यदि(!policy) {
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
          गच्छ बहिर्गमनम्;
        }

        CFArray_Append_मानम्(policies, policy);
      }
    }
  }
#अन्यथा
  /* OCSP check not available in this Apple OS version. */
  यदि(require_ocsp) {
    failf(दत्त, "Apple SecTrust version does not support OCSP checks");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ बहिर्गमनम्;
  }

#यदि_अन्तः

  cert_array = CFArrayCreateMutable(रिक्त, अङ्कः_certs, &kCFTypeArrayCallBacks);
  यदि(!cert_array) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  प्रति(i = 0; i < अङ्कः_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    अचिह्नित वर्ण *der;
    size_t der_len;

    परिणाम = der_cb(cf, दत्त, cb_user_दत्त, i, &der, &der_len);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    certdata = CFDataCreate(रिक्त, der, (CFIndex)der_len);
    यदि(!certdata) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }

    cert = Sec_Certificate_Create_With_दत्त(रिक्त, certdata);
    CFRelease(certdata);
    यदि(!cert) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }

    CFArray_Append_मानम्(cert_array, cert);
    CFRelease(cert);
  }

  स्थितिः = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  यदि(स्थितिः != noErr || !trust) {
    failf(दत्त, "Apple SecTrust: failed to create validation trust");
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    गच्छ बहिर्गमनम्;
  }

  यदि(ocsp_len > 0) {
    bool checked = FALSE;
#यदि defined(HAVE_BUILTIN_उपलब्धम्) && defined(SUPPORTS_SecOCSP)
    यदि(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(रिक्त, ocsp_buf, (CFIndex)ocsp_len);
      यदि(!ocspdata) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
      स्थितिः = Sec_Trust_समुच्चयः_OCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      यदि(स्थितिः != noErr) {
        failf(दत्त, "Apple SecTrust: failed to set OCSP response: %d",
              (पूर्णाङ्क)स्थितिः);
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;
        गच्छ बहिर्गमनम्;
      }
      checked = TRUE;
    }
#यदि_अन्तः
    यदि(!checked) {
      (शून्य)ocsp_buf;
      failf(दत्त, "Apple SecTrust: OCSP verification not supported");
      परिणाम = CURLE_NOT_BUILT_अन्तर्;
      गच्छ बहिर्गमनम्;
    }
  }

#यदि_परिभाषितम् SUPPORTS_SecTrustEvaluateWithError
#यदि_परिभाषितम् HAVE_BUILTIN_उपलब्धम्
  यदि(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#अन्यथा
  यदि(1) {
#यदि_अन्तः
    परिणाम = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    यदि(error) {
      CFIndex संहिता = CFError_Get_संहिता(error);
      error_ref = CFErrorCopyDescription(error);

      यदि(error_ref) {
        CFIndex आकार = CFString_Get_अधिकतमम्_आकार_For_Encoding(
          CFString_Get_दीर्घता(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(आकार + 1);
        यदि(err_desc) {
          यदि(!CFStringGetCString(error_ref, err_desc, आकार,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(दत्त, "Apple SecTrust failure %ld%s%s", संहिता,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      यदि(संहिता == errSecIncompleteCertRevocationCheck)
        परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  अन्यथा
#यदि_अन्तः /* SUPPORTS_SecTrustEvaluateWithError */
  {
#यदि_अपरिभाषितम् REQUIRES_SecTrustEvaluateWithError
    Sec_Trust_परिणाम_Type sec_परिणाम;
    स्थितिः = SecTrustEvaluate(trust, &sec_परिणाम);

    यदि(स्थितिः != noErr) {
      failf(दत्त, "Apple SecTrust verification failed: error %d", (पूर्णाङ्क)स्थितिः);
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
    अन्यथा यदि((sec_परिणाम == k_Sec_Trust_परिणाम_Unspecified) ||
            (sec_परिणाम == k_Sec_Trust_परिणाम_Proceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      परिणाम = CURLE_OK;
    }
    अन्यथा {
      /* Any other trust result is a verification failure in this context */
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
#यदि_अन्तः /* REQUIRES_SecTrustEvaluateWithError */
  }

बहिर्गमनम्:
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
  प्रत्यागम परिणाम;
}

bool Curl_vtls_apple_verifies_ocsp(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  (शून्य)cf;
  (शून्य)दत्त;
#यदि defined(HAVE_BUILTIN_उपलब्धम्) && defined(SUPPORTS_SecOCSP)
  प्रत्यागम TRUE;
#अन्यथा
  प्रत्यागम FALSE;
#यदि_अन्तः
}

#यदि_अन्तः /* USE_APPLE_SECTRUST */
