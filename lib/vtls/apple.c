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

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/apple.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <Security/Security.h>

#ਜੇ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 101400) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 50000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUPPORTS_SecTrustEvaluateWithError 1
#ਜੇ_ਅੰਤ

#ਜੇ defined(SUPPORTS_SecTrustEvaluateWithError) && \
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ REQUIRES_SecTrustEvaluateWithError 1
#ਜੇ_ਅੰਤ

#ਜੇ defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_ਉਪਲਬਧ) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ SUPPORTS_SecTrustEvaluateWithError
#ਜੇ_ਅੰਤ

#ਜੇ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 100900) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 70000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 90000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 20000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUPPORTS_SecOCSP 1
#ਜੇ_ਅੰਤ

CURLcode Curl_vtls_apple_verify(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ssl_peer *peer,
                                size_t ਅੰਕ_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                ਖਾਲੀ *cb_user_ਅੰਕੜੇ,
                                ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  SecTrustRef trust = ਨਿਰਾ;
  SecPolicyRef policy = ਨਿਰਾ;
  CFMutableArrayRef policies = ਨਿਰਾ;
  CFMutableArrayRef cert_array = ਨਿਰਾ;
  CFStringRef host_str = ਨਿਰਾ;
  CFErrorRef error = ਨਿਰਾ;
  OSStatus ਸਥਿਤੀ = noErr;
  CFStringRef error_ref = ਨਿਰਾ;
  ਅੱਖਰ *err_desc = ਨਿਰਾ;
  size_t i;

  ਜੇ(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(ਨਿਰਾ,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    ਜੇ(!host_str) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  policies = CFArrayCreateMutable(ਨਿਰਾ, 2, &kCFTypeArrayCallBacks);
  ਜੇ(!policies) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  ਜੇ(!policy) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  CFArray_Append_ਮੁੱਲ(policies, policy);
  CFRelease(policy);
  policy = ਨਿਰਾ;

#ਜੇ defined(HAVE_BUILTIN_ਉਪਲਬਧ) && defined(SUPPORTS_SecOCSP)
  {
    ਜੇ(!conn_config->no_revoke) {
      ਜੇ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = k_Sec_Revocation_Use_Any_ਉਪਲਬਧ_ਵਿਧੀ;
        ਜੇ(require_ocsp)
          revocation_flags |= k_Sec_Revocation_Require_ਸਕਾਰਾਤਮਕ_Response;
        policy = SecPolicyCreateRevocation(revocation_flags);
        ਜੇ(!policy) {
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
          ਜਾਓ ਬਾਹਰਲਾ;
        }

        CFArray_Append_ਮੁੱਲ(policies, policy);
      }
    }
  }
#ਨਹੀਂਤਾਂ
  /* OCSP check not available in this Apple OS version. */
  ਜੇ(require_ocsp) {
    failf(ਅੰਕੜੇ, "Apple SecTrust version does not support OCSP checks");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ_ਅੰਤ

  cert_array = CFArrayCreateMutable(ਨਿਰਾ, ਅੰਕ_certs, &kCFTypeArrayCallBacks);
  ਜੇ(!cert_array) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਲਈ(i = 0; i < ਅੰਕ_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *der;
    size_t der_len;

    ਨਤੀਜਾ = der_cb(cf, ਅੰਕੜੇ, cb_user_ਅੰਕੜੇ, i, &der, &der_len);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    certdata = CFDataCreate(ਨਿਰਾ, der, (CFIndex)der_len);
    ਜੇ(!certdata) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    cert = Sec_Certificate_Create_With_ਅੰਕੜੇ(ਨਿਰਾ, certdata);
    CFRelease(certdata);
    ਜੇ(!cert) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    CFArray_Append_ਮੁੱਲ(cert_array, cert);
    CFRelease(cert);
  }

  ਸਥਿਤੀ = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  ਜੇ(ਸਥਿਤੀ != noErr || !trust) {
    failf(ਅੰਕੜੇ, "Apple SecTrust: failed to create validation trust");
    ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(ocsp_len > 0) {
    bool checked = FALSE;
#ਜੇ defined(HAVE_BUILTIN_ਉਪਲਬਧ) && defined(SUPPORTS_SecOCSP)
    ਜੇ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(ਨਿਰਾ, ocsp_buf, (CFIndex)ocsp_len);
      ਜੇ(!ocspdata) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਸਥਿਤੀ = Sec_Trust_ਸੈੱਟ_OCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      ਜੇ(ਸਥਿਤੀ != noErr) {
        failf(ਅੰਕੜੇ, "Apple SecTrust: failed to set OCSP response: %d",
              (ਪੂਰਨ)ਸਥਿਤੀ);
        ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      checked = TRUE;
    }
#ਜੇ_ਅੰਤ
    ਜੇ(!checked) {
      (ਖਾਲੀ)ocsp_buf;
      failf(ਅੰਕੜੇ, "Apple SecTrust: OCSP verification not supported");
      ਨਤੀਜਾ = CURLE_NOT_BUILT_ਵਿੱਚ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SUPPORTS_SecTrustEvaluateWithError
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BUILTIN_ਉਪਲਬਧ
  ਜੇ(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#ਨਹੀਂਤਾਂ
  ਜੇ(1) {
#ਜੇ_ਅੰਤ
    ਨਤੀਜਾ = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    ਜੇ(error) {
      CFIndex ਕੋਡ = CFError_Get_ਕੋਡ(error);
      error_ref = CFErrorCopyDescription(error);

      ਜੇ(error_ref) {
        CFIndex ਮਾਪ = CFString_Get_ਵੱਧ_ਤੋਂ_ਵੱਧ_ਮਾਪ_For_Encoding(
          CFString_Get_ਲੰਮਾਈ(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(ਮਾਪ + 1);
        ਜੇ(err_desc) {
          ਜੇ(!CFStringGetCString(error_ref, err_desc, ਮਾਪ,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(ਅੰਕੜੇ, "Apple SecTrust failure %ld%s%s", ਕੋਡ,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      ਜੇ(ਕੋਡ == errSecIncompleteCertRevocationCheck)
        ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ REQUIRES_SecTrustEvaluateWithError
    Sec_Trust_ਨਤੀਜਾ_Type sec_ਨਤੀਜਾ;
    ਸਥਿਤੀ = SecTrustEvaluate(trust, &sec_ਨਤੀਜਾ);

    ਜੇ(ਸਥਿਤੀ != noErr) {
      failf(ਅੰਕੜੇ, "Apple SecTrust verification failed: error %d", (ਪੂਰਨ)ਸਥਿਤੀ);
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ਨਹੀਂਤਾਂ ਜੇ((sec_ਨਤੀਜਾ == k_Sec_Trust_ਨਤੀਜਾ_Unspecified) ||
            (sec_ਨਤੀਜਾ == k_Sec_Trust_ਨਤੀਜਾ_Proceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      ਨਤੀਜਾ = CURLE_OK;
    }
    ਨਹੀਂਤਾਂ {
      /* Any other trust result is a verification failure in this context */
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    }
#ਜੇ_ਅੰਤ /* REQUIRES_SecTrustEvaluateWithError */
  }

ਬਾਹਰਲਾ:
  curlx_free(err_desc);
  ਜੇ(error_ref)
    CFRelease(error_ref);
  ਜੇ(error)
    CFRelease(error);
  ਜੇ(host_str)
    CFRelease(host_str);
  ਜੇ(policies)
    CFRelease(policies);
  ਜੇ(policy)
    CFRelease(policy);
  ਜੇ(cert_array)
    CFRelease(cert_array);
  ਜੇ(trust)
    CFRelease(trust);
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_vtls_apple_verifies_ocsp(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ defined(HAVE_BUILTIN_ਉਪਲਬਧ) && defined(SUPPORTS_SecOCSP)
  ਵਾਪਸ TRUE;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅੰਤ /* USE_APPLE_SECTRUST */
