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

#جے_تعریف_شدہ USE_APPLE_SECTRUST

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
#تعریف_کرو SUPPORTS_SecTrustEvaluateWithError 1
#جے_انت

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
#تعریف_کرو REQUIRES_SecTrustEvaluateWithError 1
#جے_انت

#إذا defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_دستیاب) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#تعریف_رد SUPPORTS_SecTrustEvaluateWithError
#جے_انت

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
#تعریف_کرو SUPPORTS_SecOCSP 1
#جے_انت

CURLcode Curl_vtls_apple_verify(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                بنية ssl_peer *peer,
                                size_t عدد_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                فراغ *cb_user_انکڑے,
                                ثابت بنانشانی حرف *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_OK;
  SecTrustRef trust = عدم;
  SecPolicyRef policy = عدم;
  CFMutableArrayRef policies = عدم;
  CFMutableArrayRef cert_array = عدم;
  CFStringRef host_str = عدم;
  CFErrorRef error = عدم;
  OSStatus حالت = noErr;
  CFStringRef error_ref = عدم;
  حرف *err_desc = عدم;
  size_t i;

  إذا(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(عدم,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    إذا(!host_str) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }
  }

  policies = CFArrayCreateMutable(عدم, 2, &kCFTypeArrayCallBacks);
  إذا(!policies) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  إذا(!policy) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  CFArray_Append_قدر(policies, policy);
  CFRelease(policy);
  policy = عدم;

#إذا defined(HAVE_BUILTIN_دستیاب) && defined(SUPPORTS_SecOCSP)
  {
    إذا(!conn_config->no_revoke) {
      إذا(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = k_Sec_Revocation_Use_Any_دستیاب_طریقہ;
        إذا(require_ocsp)
          revocation_flags |= k_Sec_Revocation_Require_مثبت_Response;
        policy = SecPolicyCreateRevocation(revocation_flags);
        إذا(!policy) {
          نتیجہ = CURLE_باہرلا_دا_حافظہ;
          اذهب باہرلا;
        }

        CFArray_Append_قدر(policies, policy);
      }
    }
  }
#وإلا
  /* OCSP check not available in this Apple OS version. */
  إذا(require_ocsp) {
    failf(انکڑے, "Apple SecTrust version does not support OCSP checks");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب باہرلا;
  }

#جے_انت

  cert_array = CFArrayCreateMutable(عدم, عدد_certs, &kCFTypeArrayCallBacks);
  إذا(!cert_array) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب باہرلا;
  }

  لكل(i = 0; i < عدد_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    بنانشانی حرف *der;
    size_t der_len;

    نتیجہ = der_cb(cf, انکڑے, cb_user_انکڑے, i, &der, &der_len);
    إذا(نتیجہ)
      اذهب باہرلا;

    certdata = CFDataCreate(عدم, der, (CFIndex)der_len);
    إذا(!certdata) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }

    cert = Sec_Certificate_Create_With_انکڑے(عدم, certdata);
    CFRelease(certdata);
    إذا(!cert) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب باہرلا;
    }

    CFArray_Append_قدر(cert_array, cert);
    CFRelease(cert);
  }

  حالت = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  إذا(حالت != noErr || !trust) {
    failf(انکڑے, "Apple SecTrust: failed to create validation trust");
    نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    اذهب باہرلا;
  }

  إذا(ocsp_len > 0) {
    bool checked = FALSE;
#إذا defined(HAVE_BUILTIN_دستیاب) && defined(SUPPORTS_SecOCSP)
    إذا(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(عدم, ocsp_buf, (CFIndex)ocsp_len);
      إذا(!ocspdata) {
        نتیجہ = CURLE_باہرلا_دا_حافظہ;
        اذهب باہرلا;
      }
      حالت = Sec_Trust_سیٹ_OCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      إذا(حالت != noErr) {
        failf(انکڑے, "Apple SecTrust: failed to set OCSP response: %d",
              (صحيح)حالت);
        نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
        اذهب باہرلا;
      }
      checked = TRUE;
    }
#جے_انت
    إذا(!checked) {
      (فراغ)ocsp_buf;
      failf(انکڑے, "Apple SecTrust: OCSP verification not supported");
      نتیجہ = CURLE_NOT_BUILT_وچ;
      اذهب باہرلا;
    }
  }

#جے_تعریف_شدہ SUPPORTS_SecTrustEvaluateWithError
#جے_تعریف_شدہ HAVE_BUILTIN_دستیاب
  إذا(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#وإلا
  إذا(1) {
#جے_انت
    نتیجہ = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    إذا(error) {
      CFIndex کوڈ = CFError_Get_کوڈ(error);
      error_ref = CFErrorCopyDescription(error);

      إذا(error_ref) {
        CFIndex ناپ = CFString_Get_ودھ_توں_ودھ_ناپ_For_Encoding(
          CFString_Get_لمائی(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(ناپ + 1);
        إذا(err_desc) {
          إذا(!CFStringGetCString(error_ref, err_desc, ناپ,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(انکڑے, "Apple SecTrust failure %ld%s%s", کوڈ,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      إذا(کوڈ == errSecIncompleteCertRevocationCheck)
        نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  وإلا
#جے_انت /* SUPPORTS_SecTrustEvaluateWithError */
  {
#جے_بنا_تعریف REQUIRES_SecTrustEvaluateWithError
    Sec_Trust_نتیجہ_Type sec_نتیجہ;
    حالت = SecTrustEvaluate(trust, &sec_نتیجہ);

    إذا(حالت != noErr) {
      failf(انکڑے, "Apple SecTrust verification failed: error %d", (صحيح)حالت);
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    }
    وإلا إذا((sec_نتیجہ == k_Sec_Trust_نتیجہ_Unspecified) ||
            (sec_نتیجہ == k_Sec_Trust_نتیجہ_Proceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      نتیجہ = CURLE_OK;
    }
    وإلا {
      /* Any other trust result is a verification failure in this context */
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    }
#جے_انت /* REQUIRES_SecTrustEvaluateWithError */
  }

باہرلا:
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
  أرجع نتیجہ;
}

bool Curl_vtls_apple_verifies_ocsp(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے)
{
  (فراغ)cf;
  (فراغ)انکڑے;
#إذا defined(HAVE_BUILTIN_دستیاب) && defined(SUPPORTS_SecOCSP)
  أرجع TRUE;
#وإلا
  أرجع FALSE;
#جے_انت
}

#جے_انت /* USE_APPLE_SECTRUST */
