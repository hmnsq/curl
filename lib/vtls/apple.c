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

#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_APPLE_SECTRUST

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/apple.h"

#અન્તર્ભાવ <Security/Security.h>

#જો (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

#જો defined(SUPPORTS_SecTrustEvaluateWithError) && \
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

#જો defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#undef SUPPORTS_SecTrustEvaluateWithError
#endif

#જો (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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

CURLcode Curl_vtls_apple_verify(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                સંરચના ssl_peer *peer,
                                size_t અંક_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                શૂન્ય *cb_user_data,
                                અચલ unsigned અક્ષર *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  SecTrustRef trust = ખાલી;
  SecPolicyRef policy = ખાલી;
  CFMutableArrayRef policies = ખાલી;
  CFMutableArrayRef cert_array = ખાલી;
  CFStringRef host_str = ખાલી;
  CFErrorRef error = ખાલી;
  OSStatus status = noErr;
  CFStringRef error_ref = ખાલી;
  અક્ષર *err_desc = ખાલી;
  size_t i;

  જો(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(ખાલી,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    જો(!host_str) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }

  policies = CFArrayCreateMutable(ખાલી, 2, &kCFTypeArrayCallBacks);
  જો(!policies) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  જો(!policy) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  CFArray_Append_કિંમત(policies, policy);
  CFRelease(policy);
  policy = ખાલી;

#જો defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    જો(!conn_config->no_revoke) {
      જો(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        જો(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        જો(!policy) {
          result = CURLE_OUT_OF_MEMORY;
          જાઓ out;
        }

        CFArray_Append_કિંમત(policies, policy);
      }
    }
  }
#નહીંતર
  /* OCSP check not available in this Apple OS version. */
  જો(require_ocsp) {
    failf(data, "Apple SecTrust version does not support OCSP checks");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ out;
  }

#endif

  cert_array = CFArrayCreateMutable(ખાલી, અંક_certs, &kCFTypeArrayCallBacks);
  જો(!cert_array) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  માટે(i = 0; i < અંક_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    unsigned અક્ષર *der;
    size_t der_len;

    result = der_cb(cf, data, cb_user_data, i, &der, &der_len);
    જો(result)
      જાઓ out;

    certdata = CFDataCreate(ખાલી, der, (CFIndex)der_len);
    જો(!certdata) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }

    cert = SecCertificateCreateWithData(ખાલી, certdata);
    CFRelease(certdata);
    જો(!cert) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }

    CFArray_Append_કિંમત(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  જો(status != noErr || !trust) {
    failf(data, "Apple SecTrust: failed to create validation trust");
    result = CURLE_PEER_FAILED_VERIFICATION;
    જાઓ out;
  }

  જો(ocsp_len > 0) {
    bool checked = FALSE;
#જો defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    જો(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(ખાલી, ocsp_buf, (CFIndex)ocsp_len);
      જો(!ocspdata) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      જો(status != noErr) {
        failf(data, "Apple SecTrust: failed to set OCSP response: %d",
              (પૂર્ણાંક)status);
        result = CURLE_PEER_FAILED_VERIFICATION;
        જાઓ out;
      }
      checked = TRUE;
    }
#endif
    જો(!checked) {
      (શૂન્ય)ocsp_buf;
      failf(data, "Apple SecTrust: OCSP verification not supported");
      result = CURLE_NOT_BUILT_IN;
      જાઓ out;
    }
  }

#ifdef SUPPORTS_SecTrustEvaluateWithError
#ifdef HAVE_BUILTIN_AVAILABLE
  જો(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#નહીંતર
  જો(1) {
#endif
    result = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    જો(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      જો(error_ref) {
        CFIndex size = CFStringGetMaximumSizeForEncoding(
          CFStringGetLength(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(size + 1);
        જો(err_desc) {
          જો(!CFStringGetCString(error_ref, err_desc, size,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(data, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      જો(code == errSecIncompleteCertRevocationCheck)
        result = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  નહીંતર
#endif /* SUPPORTS_SecTrustEvaluateWithError */
  {
#ifndef REQUIRES_SecTrustEvaluateWithError
    SecTrustResultType sec_result;
    status = SecTrustEvaluate(trust, &sec_result);

    જો(status != noErr) {
      failf(data, "Apple SecTrust verification failed: error %d", (પૂર્ણાંક)status);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    નહીંતર જો((sec_result == kSecTrustResultUnspecified) ||
            (sec_result == kSecTrustResultProceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      result = CURLE_OK;
    }
    નહીંતર {
      /* Any other trust result is a verification failure in this context */
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
#endif /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  જો(error_ref)
    CFRelease(error_ref);
  જો(error)
    CFRelease(error);
  જો(host_str)
    CFRelease(host_str);
  જો(policies)
    CFRelease(policies);
  જો(policy)
    CFRelease(policy);
  જો(cert_array)
    CFRelease(cert_array);
  જો(trust)
    CFRelease(trust);
  પાછા result;
}

bool Curl_vtls_apple_verifies_ocsp(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  (શૂન્ય)cf;
  (શૂન્ય)data;
#જો defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  પાછા TRUE;
#નહીંતર
  પાછા FALSE;
#endif
}

#endif /* USE_APPLE_SECTRUST */
