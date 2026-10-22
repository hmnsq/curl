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

#اگر_تعریف_شده USE_APPLE_SECTRUST

#ضمن "urldata.h"
#ضمن "cfilters.h"
#ضمن "curl_trc.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/apple.h"

#ضمن <Security/Security.h>

#إذا (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 101400) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 50000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#تعریف_کن SUPPORTS_SecTrustEvaluateWithError 1
#پایان_اگر

#إذا defined(SUPPORTS_SecTrustEvaluateWithError) && \
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
#تعریف_کن REQUIRES_SecTrustEvaluateWithError 1
#پایان_اگر

#إذا defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_موجود) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#لغو_تعریف SUPPORTS_SecTrustEvaluateWithError
#پایان_اگر

#إذا (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 100900) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 70000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 90000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 20000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#تعریف_کن SUPPORTS_SecOCSP 1
#پایان_اگر

CURLcode Curl_vtls_apple_verify(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                بنية ssl_peer *peer,
                                size_t رقم_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                فراغ *cb_user_داده,
                                ثابت بیعلامت حرف *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجه = CURLE_OK;
  SecTrustRef trust = عدم;
  SecPolicyRef policy = عدم;
  CFMutableArrayRef policies = عدم;
  CFMutableArrayRef cert_array = عدم;
  CFStringRef host_str = عدم;
  CFErrorRef error = عدم;
  OSStatus وضعیت = noErr;
  CFStringRef error_ref = عدم;
  حرف *err_desc = عدم;
  size_t i;

  إذا(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(عدم,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    إذا(!host_str) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }

  policies = CFArrayCreateMutable(عدم, 2, &kCFTypeArrayCallBacks);
  إذا(!policies) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  إذا(!policy) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  CFArray_Append_مقدار(policies, policy);
  CFRelease(policy);
  policy = عدم;

#إذا defined(HAVE_BUILTIN_موجود) && defined(SUPPORTS_SecOCSP)
  {
    إذا(!conn_config->no_revoke) {
      إذا(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = k_Sec_Revocation_Use_Any_موجود_روش;
        إذا(require_ocsp)
          revocation_flags |= k_Sec_Revocation_Require_مثبت_Response;
        policy = SecPolicyCreateRevocation(revocation_flags);
        إذا(!policy) {
          نتیجه = CURLE_بیرون_از_حافظه;
          اذهب بیرون;
        }

        CFArray_Append_مقدار(policies, policy);
      }
    }
  }
#وإلا
  /* OCSP check not available in this Apple OS version. */
  إذا(require_ocsp) {
    failf(داده, "Apple SecTrust version does not support OCSP checks");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب بیرون;
  }

#پایان_اگر

  cert_array = CFArrayCreateMutable(عدم, رقم_certs, &kCFTypeArrayCallBacks);
  إذا(!cert_array) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  لكل(i = 0; i < رقم_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    بیعلامت حرف *der;
    size_t der_len;

    نتیجه = der_cb(cf, داده, cb_user_داده, i, &der, &der_len);
    إذا(نتیجه)
      اذهب بیرون;

    certdata = CFDataCreate(عدم, der, (CFIndex)der_len);
    إذا(!certdata) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }

    cert = Sec_Certificate_Create_With_داده(عدم, certdata);
    CFRelease(certdata);
    إذا(!cert) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }

    CFArray_Append_مقدار(cert_array, cert);
    CFRelease(cert);
  }

  وضعیت = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  إذا(وضعیت != noErr || !trust) {
    failf(داده, "Apple SecTrust: failed to create validation trust");
    نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    اذهب بیرون;
  }

  إذا(ocsp_len > 0) {
    bool checked = FALSE;
#إذا defined(HAVE_BUILTIN_موجود) && defined(SUPPORTS_SecOCSP)
    إذا(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(عدم, ocsp_buf, (CFIndex)ocsp_len);
      إذا(!ocspdata) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
      وضعیت = Sec_Trust_مجموعه_OCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      إذا(وضعیت != noErr) {
        failf(داده, "Apple SecTrust: failed to set OCSP response: %d",
              (صحيح)وضعیت);
        نتیجه = CURLE_PEER_FAILED_VERIFICATION;
        اذهب بیرون;
      }
      checked = TRUE;
    }
#پایان_اگر
    إذا(!checked) {
      (فراغ)ocsp_buf;
      failf(داده, "Apple SecTrust: OCSP verification not supported");
      نتیجه = CURLE_NOT_BUILT_درون;
      اذهب بیرون;
    }
  }

#اگر_تعریف_شده SUPPORTS_SecTrustEvaluateWithError
#اگر_تعریف_شده HAVE_BUILTIN_موجود
  إذا(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#وإلا
  إذا(1) {
#پایان_اگر
    نتیجه = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    إذا(error) {
      CFIndex کد = CFError_Get_کد(error);
      error_ref = CFErrorCopyDescription(error);

      إذا(error_ref) {
        CFIndex اندازه = CFString_Get_بیشینه_اندازه_For_Encoding(
          CFString_Get_درازی(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(اندازه + 1);
        إذا(err_desc) {
          إذا(!CFStringGetCString(error_ref, err_desc, اندازه,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(داده, "Apple SecTrust failure %ld%s%s", کد,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      إذا(کد == errSecIncompleteCertRevocationCheck)
        نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  وإلا
#پایان_اگر /* SUPPORTS_SecTrustEvaluateWithError */
  {
#اگر_تعریف_نشده REQUIRES_SecTrustEvaluateWithError
    Sec_Trust_نتیجه_Type sec_نتیجه;
    وضعیت = SecTrustEvaluate(trust, &sec_نتیجه);

    إذا(وضعیت != noErr) {
      failf(داده, "Apple SecTrust verification failed: error %d", (صحيح)وضعیت);
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    }
    وإلا إذا((sec_نتیجه == k_Sec_Trust_نتیجه_Unspecified) ||
            (sec_نتیجه == k_Sec_Trust_نتیجه_Proceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      نتیجه = CURLE_OK;
    }
    وإلا {
      /* Any other trust result is a verification failure in this context */
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    }
#پایان_اگر /* REQUIRES_SecTrustEvaluateWithError */
  }

بیرون:
  curlx_free(err_desc);
  إذا(error_ref)
    CFRelease(error_ref);
  إذا(error)
    CFRelease(error);
  إذا(host_str)
    CFRelease(host_str);
  إذا(policies)
    CFRelease(policies);
  إذا(policy)
    CFRelease(policy);
  إذا(cert_array)
    CFRelease(cert_array);
  إذا(trust)
    CFRelease(trust);
  أرجع نتیجه;
}

bool Curl_vtls_apple_verifies_ocsp(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  (فراغ)cf;
  (فراغ)داده;
#إذا defined(HAVE_BUILTIN_موجود) && defined(SUPPORTS_SecOCSP)
  أرجع TRUE;
#وإلا
  أرجع FALSE;
#پایان_اگر
}

#پایان_اگر /* USE_APPLE_SECTRUST */
