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

#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_APPLE_SECTRUST

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/apple.h"

#അന്തര്ഭാവ <Security/Security.h>

#എങ്കിൽ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

#എങ്കിൽ defined(SUPPORTS_SecTrustEvaluateWithError) && \
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

#എങ്കിൽ defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#undef SUPPORTS_SecTrustEvaluateWithError
#endif

#എങ്കിൽ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

CURLcode Curl_vtls_apple_verify(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                ഘടന ssl_peer *peer,
                                size_t അക്കം_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                ശൂന്യം *cb_user_data,
                                സ്ഥിരം unsigned അക്ഷരം *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  SecTrustRef trust = ശൂന്യ;
  SecPolicyRef policy = ശൂന്യ;
  CFMutableArrayRef policies = ശൂന്യ;
  CFMutableArrayRef cert_array = ശൂന്യ;
  CFStringRef host_str = ശൂന്യ;
  CFErrorRef error = ശൂന്യ;
  OSStatus status = noErr;
  CFStringRef error_ref = ശൂന്യ;
  അക്ഷരം *err_desc = ശൂന്യ;
  size_t i;

  എങ്കിൽ(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(ശൂന്യ,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    എങ്കിൽ(!host_str) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
  }

  policies = CFArrayCreateMutable(ശൂന്യ, 2, &kCFTypeArrayCallBacks);
  എങ്കിൽ(!policies) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  എങ്കിൽ(!policy) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  CFArray_Append_മൂല്യം(policies, policy);
  CFRelease(policy);
  policy = ശൂന്യ;

#എങ്കിൽ defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    എങ്കിൽ(!conn_config->no_revoke) {
      എങ്കിൽ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        എങ്കിൽ(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        എങ്കിൽ(!policy) {
          result = CURLE_OUT_OF_MEMORY;
          പോകുക out;
        }

        CFArray_Append_മൂല്യം(policies, policy);
      }
    }
  }
#അല്ലെങ്കിൽ
  /* OCSP check not available in this Apple OS version. */
  എങ്കിൽ(require_ocsp) {
    failf(data, "Apple SecTrust version does not support OCSP checks");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക out;
  }

#endif

  cert_array = CFArrayCreateMutable(ശൂന്യ, അക്കം_certs, &kCFTypeArrayCallBacks);
  എങ്കിൽ(!cert_array) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  വേണ്ടി(i = 0; i < അക്കം_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    unsigned അക്ഷരം *der;
    size_t der_len;

    result = der_cb(cf, data, cb_user_data, i, &der, &der_len);
    എങ്കിൽ(result)
      പോകുക out;

    certdata = CFDataCreate(ശൂന്യ, der, (CFIndex)der_len);
    എങ്കിൽ(!certdata) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }

    cert = SecCertificateCreateWithData(ശൂന്യ, certdata);
    CFRelease(certdata);
    എങ്കിൽ(!cert) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }

    CFArray_Append_മൂല്യം(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  എങ്കിൽ(status != noErr || !trust) {
    failf(data, "Apple SecTrust: failed to create validation trust");
    result = CURLE_PEER_FAILED_VERIFICATION;
    പോകുക out;
  }

  എങ്കിൽ(ocsp_len > 0) {
    bool checked = FALSE;
#എങ്കിൽ defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    എങ്കിൽ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(ശൂന്യ, ocsp_buf, (CFIndex)ocsp_len);
      എങ്കിൽ(!ocspdata) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      എങ്കിൽ(status != noErr) {
        failf(data, "Apple SecTrust: failed to set OCSP response: %d",
              (പൂർണ്ണസംഖ്യ)status);
        result = CURLE_PEER_FAILED_VERIFICATION;
        പോകുക out;
      }
      checked = TRUE;
    }
#endif
    എങ്കിൽ(!checked) {
      (ശൂന്യം)ocsp_buf;
      failf(data, "Apple SecTrust: OCSP verification not supported");
      result = CURLE_NOT_BUILT_IN;
      പോകുക out;
    }
  }

#ifdef SUPPORTS_SecTrustEvaluateWithError
#ifdef HAVE_BUILTIN_AVAILABLE
  എങ്കിൽ(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#അല്ലെങ്കിൽ
  എങ്കിൽ(1) {
#endif
    result = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    എങ്കിൽ(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      എങ്കിൽ(error_ref) {
        CFIndex size = CFStringGetMaximumSizeForEncoding(
          CFStringGetLength(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(size + 1);
        എങ്കിൽ(err_desc) {
          എങ്കിൽ(!CFStringGetCString(error_ref, err_desc, size,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(data, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      എങ്കിൽ(code == errSecIncompleteCertRevocationCheck)
        result = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  അല്ലെങ്കിൽ
#endif /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ifndef REQUIRES_SecTrustEvaluateWithError
    SecTrustResultType sec_result;
    status = SecTrustEvaluate(trust, &sec_result);

    എങ്കിൽ(status != noErr) {
      failf(data, "Apple SecTrust verification failed: error %d", (പൂർണ്ണസംഖ്യ)status);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((sec_result == kSecTrustResultUnspecified) ||
            (sec_result == kSecTrustResultProceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      result = CURLE_OK;
    }
    അല്ലെങ്കിൽ {
      /* Any other trust result is a verification failure in this context */
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
#endif /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  എങ്കിൽ(error_ref)
    CFRelease(error_ref);
  എങ്കിൽ(error)
    CFRelease(error);
  എങ്കിൽ(host_str)
    CFRelease(host_str);
  എങ്കിൽ(policies)
    CFRelease(policies);
  എങ്കിൽ(policy)
    CFRelease(policy);
  എങ്കിൽ(cert_array)
    CFRelease(cert_array);
  എങ്കിൽ(trust)
    CFRelease(trust);
  തിരികെ result;
}

bool Curl_vtls_apple_verifies_ocsp(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  (ശൂന്യം)cf;
  (ശൂന്യം)data;
#എങ്കിൽ defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  തിരികെ TRUE;
#അല്ലെങ്കിൽ
  തിരികെ FALSE;
#endif
}

#endif /* USE_APPLE_SECTRUST */
