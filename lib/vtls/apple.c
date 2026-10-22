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

#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_APPLE_SECTRUST

#شامل "urldata.h"
#شامل "cfilters.h"
#شامل "curl_trc.h"
#شامل "vtls/vtls.h"
#شامل "vtls/apple.h"

#شامل <Security/Security.h>

#اگر (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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
#اگر_ختم

#اگر defined(SUPPORTS_SecTrustEvaluateWithError) && \
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
#اگر_ختم

#اگر defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_دستیاب) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#تعریف_منسوخ SUPPORTS_SecTrustEvaluateWithError
#اگر_ختم

#اگر (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
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
#اگر_ختم

CURLcode Curl_vtls_apple_verify(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                ساخت ssl_peer *peer,
                                size_t عدد_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                خالی *cb_user_ڈیٹا,
                                ثابت غیرعلامتی حرف *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_OK;
  SecTrustRef trust = تہی;
  SecPolicyRef policy = تہی;
  CFMutableArrayRef policies = تہی;
  CFMutableArrayRef cert_array = تہی;
  CFStringRef host_str = تہی;
  CFErrorRef error = تہی;
  OSStatus حالت_2 = noErr;
  CFStringRef error_ref = تہی;
  حرف *err_desc = تہی;
  size_t i;

  اگر(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(تہی,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    اگر(!host_str) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }

  policies = CFArrayCreateMutable(تہی, 2, &kCFTypeArrayCallBacks);
  اگر(!policies) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  اگر(!policy) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  CFArray_Append_قدر(policies, policy);
  CFRelease(policy);
  policy = تہی;

#اگر defined(HAVE_BUILTIN_دستیاب) && defined(SUPPORTS_SecOCSP)
  {
    اگر(!conn_config->no_revoke) {
      اگر(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = k_Sec_Revocation_Use_Any_دستیاب_طریقہ;
        اگر(require_ocsp)
          revocation_flags |= k_Sec_Revocation_Require_مثبت_Response;
        policy = SecPolicyCreateRevocation(revocation_flags);
        اگر(!policy) {
          نتیجہ = CURLE_برآمد_کا_حافظہ;
          جاؤ برآمد;
        }

        CFArray_Append_قدر(policies, policy);
      }
    }
  }
#ورنہ
  /* OCSP check not available in this Apple OS version. */
  اگر(require_ocsp) {
    failf(ڈیٹا, "Apple SecTrust version does not support OCSP checks");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ برآمد;
  }

#اگر_ختم

  cert_array = CFArrayCreateMutable(تہی, عدد_certs, &kCFTypeArrayCallBacks);
  اگر(!cert_array) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  برائے(i = 0; i < عدد_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    غیرعلامتی حرف *der;
    size_t der_len;

    نتیجہ = der_cb(cf, ڈیٹا, cb_user_ڈیٹا, i, &der, &der_len);
    اگر(نتیجہ)
      جاؤ برآمد;

    certdata = CFDataCreate(تہی, der, (CFIndex)der_len);
    اگر(!certdata) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }

    cert = Sec_Certificate_Create_With_ڈیٹا(تہی, certdata);
    CFRelease(certdata);
    اگر(!cert) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }

    CFArray_Append_قدر(cert_array, cert);
    CFRelease(cert);
  }

  حالت_2 = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  اگر(حالت_2 != noErr || !trust) {
    failf(ڈیٹا, "Apple SecTrust: failed to create validation trust");
    نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    جاؤ برآمد;
  }

  اگر(ocsp_len > 0) {
    bool checked = FALSE;
#اگر defined(HAVE_BUILTIN_دستیاب) && defined(SUPPORTS_SecOCSP)
    اگر(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(تہی, ocsp_buf, (CFIndex)ocsp_len);
      اگر(!ocspdata) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
      حالت_2 = Sec_Trust_سیٹ_OCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      اگر(حالت_2 != noErr) {
        failf(ڈیٹا, "Apple SecTrust: failed to set OCSP response: %d",
              (صحیح)حالت_2);
        نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
        جاؤ برآمد;
      }
      checked = TRUE;
    }
#اگر_ختم
    اگر(!checked) {
      (خالی)ocsp_buf;
      failf(ڈیٹا, "Apple SecTrust: OCSP verification not supported");
      نتیجہ = CURLE_NOT_BUILT_اندر;
      جاؤ برآمد;
    }
  }

#اگر_تعریف_شدہ SUPPORTS_SecTrustEvaluateWithError
#اگر_تعریف_شدہ HAVE_BUILTIN_دستیاب
  اگر(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#ورنہ
  اگر(1) {
#اگر_ختم
    نتیجہ = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    اگر(error) {
      CFIndex رمز = CFError_Get_رمز(error);
      error_ref = CFErrorCopyDescription(error);

      اگر(error_ref) {
        CFIndex سائز = CFString_Get_زیادہ_سے_زیادہ_سائز_For_Encoding(
          CFString_Get_لمبائی(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(سائز + 1);
        اگر(err_desc) {
          اگر(!CFStringGetCString(error_ref, err_desc, سائز,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(ڈیٹا, "Apple SecTrust failure %ld%s%s", رمز,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      اگر(رمز == errSecIncompleteCertRevocationCheck)
        نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  ورنہ
#اگر_ختم /* SUPPORTS_SecTrustEvaluateWithError */
  {
#اگر_غیر_تعریف_شدہ REQUIRES_SecTrustEvaluateWithError
    Sec_Trust_نتیجہ_Type sec_نتیجہ;
    حالت_2 = SecTrustEvaluate(trust, &sec_نتیجہ);

    اگر(حالت_2 != noErr) {
      failf(ڈیٹا, "Apple SecTrust verification failed: error %d", (صحیح)حالت_2);
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ورنہ اگر((sec_نتیجہ == k_Sec_Trust_نتیجہ_Unspecified) ||
            (sec_نتیجہ == k_Sec_Trust_نتیجہ_Proceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      نتیجہ = CURLE_OK;
    }
    ورنہ {
      /* Any other trust result is a verification failure in this context */
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    }
#اگر_ختم /* REQUIRES_SecTrustEvaluateWithError */
  }

برآمد:
  curlx_free(err_desc);
  اگر(error_ref)
    CFRelease(error_ref);
  اگر(error)
    CFRelease(error);
  اگر(host_str)
    CFRelease(host_str);
  اگر(policies)
    CFRelease(policies);
  اگر(policy)
    CFRelease(policy);
  اگر(cert_array)
    CFRelease(cert_array);
  اگر(trust)
    CFRelease(trust);
  واپس نتیجہ;
}

bool Curl_vtls_apple_verifies_ocsp(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  (خالی)cf;
  (خالی)ڈیٹا;
#اگر defined(HAVE_BUILTIN_دستیاب) && defined(SUPPORTS_SecOCSP)
  واپس TRUE;
#ورنہ
  واپس FALSE;
#اگر_ختم
}

#اگر_ختم /* USE_APPLE_SECTRUST */
