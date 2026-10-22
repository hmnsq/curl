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

#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_APPLE_SECTRUST

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/apple.h"

#అన్తర్భావ <Security/Security.h>

#ఒకవేళ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 101400) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 50000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#నిర్వచించు SUPPORTS_SecTrustEvaluateWithError 1
#అయితే_ముగింపు

#ఒకవేళ defined(SUPPORTS_SecTrustEvaluateWithError) && \
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
#నిర్వచించు REQUIRES_SecTrustEvaluateWithError 1
#అయితే_ముగింపు

#ఒకవేళ defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_అందుబాటు) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#నిర్వచనం_రద్దు SUPPORTS_SecTrustEvaluateWithError
#అయితే_ముగింపు

#ఒకవేళ (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 100900) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 70000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 90000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 20000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#నిర్వచించు SUPPORTS_SecOCSP 1
#అయితే_ముగింపు

CURLcode Curl_vtls_apple_verify(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ssl_peer *peer,
                                size_t అంకె_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                శూన్య *cb_user_దత్తాంశం,
                                స్థిరరాశి గుర్తులేని అక్షరం *ocsp_buf,
                                size_t ocsp_len,
                                bool require_ocsp)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ఫలితం = CURLE_OK;
  SecTrustRef trust = ఖాళీ;
  SecPolicyRef policy = ఖాళీ;
  CFMutableArrayRef policies = ఖాళీ;
  CFMutableArrayRef cert_array = ఖాళీ;
  CFStringRef host_str = ఖాళీ;
  CFErrorRef error = ఖాళీ;
  OSStatus స్థితి_2 = noErr;
  CFStringRef error_ref = ఖాళీ;
  అక్షరం *err_desc = ఖాళీ;
  size_t i;

  ఒకవేళ(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(ఖాళీ,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    ఒకవేళ(!host_str) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }

  policies = CFArrayCreateMutable(ఖాళీ, 2, &kCFTypeArrayCallBacks);
  ఒకవేళ(!policies) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  ఒకవేళ(!policy) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  CFArray_Append_విలువ(policies, policy);
  CFRelease(policy);
  policy = ఖాళీ;

#ఒకవేళ defined(HAVE_BUILTIN_అందుబాటు) && defined(SUPPORTS_SecOCSP)
  {
    ఒకవేళ(!conn_config->no_revoke) {
      ఒకవేళ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = k_Sec_Revocation_Use_Any_అందుబాటు_పద్ధతి;
        ఒకవేళ(require_ocsp)
          revocation_flags |= k_Sec_Revocation_Require_ధనాత్మక_Response;
        policy = SecPolicyCreateRevocation(revocation_flags);
        ఒకవేళ(!policy) {
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
          వెళ్ళు బహిర్గతం;
        }

        CFArray_Append_విలువ(policies, policy);
      }
    }
  }
#లేకపోతే
  /* OCSP check not available in this Apple OS version. */
  ఒకవేళ(require_ocsp) {
    failf(దత్తాంశం, "Apple SecTrust version does not support OCSP checks");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు బహిర్గతం;
  }

#అయితే_ముగింపు

  cert_array = CFArrayCreateMutable(ఖాళీ, అంకె_certs, &kCFTypeArrayCallBacks);
  ఒకవేళ(!cert_array) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  కొరకు(i = 0; i < అంకె_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    గుర్తులేని అక్షరం *der;
    size_t der_len;

    ఫలితం = der_cb(cf, దత్తాంశం, cb_user_దత్తాంశం, i, &der, &der_len);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    certdata = CFDataCreate(ఖాళీ, der, (CFIndex)der_len);
    ఒకవేళ(!certdata) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }

    cert = Sec_Certificate_Create_With_దత్తాంశం(ఖాళీ, certdata);
    CFRelease(certdata);
    ఒకవేళ(!cert) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }

    CFArray_Append_విలువ(cert_array, cert);
    CFRelease(cert);
  }

  స్థితి_2 = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  ఒకవేళ(స్థితి_2 != noErr || !trust) {
    failf(దత్తాంశం, "Apple SecTrust: failed to create validation trust");
    ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(ocsp_len > 0) {
    bool checked = FALSE;
#ఒకవేళ defined(HAVE_BUILTIN_అందుబాటు) && defined(SUPPORTS_SecOCSP)
    ఒకవేళ(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(ఖాళీ, ocsp_buf, (CFIndex)ocsp_len);
      ఒకవేళ(!ocspdata) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
      స్థితి_2 = Sec_Trust_సమితి_OCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      ఒకవేళ(స్థితి_2 != noErr) {
        failf(దత్తాంశం, "Apple SecTrust: failed to set OCSP response: %d",
              (పూర్ణాంకం)స్థితి_2);
        ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
        వెళ్ళు బహిర్గతం;
      }
      checked = TRUE;
    }
#అయితే_ముగింపు
    ఒకవేళ(!checked) {
      (శూన్య)ocsp_buf;
      failf(దత్తాంశం, "Apple SecTrust: OCSP verification not supported");
      ఫలితం = CURLE_NOT_BUILT_లోపల;
      వెళ్ళు బహిర్గతం;
    }
  }

#నిర్వచితమైతే SUPPORTS_SecTrustEvaluateWithError
#నిర్వచితమైతే HAVE_BUILTIN_అందుబాటు
  ఒకవేళ(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#లేకపోతే
  ఒకవేళ(1) {
#అయితే_ముగింపు
    ఫలితం = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    ఒకవేళ(error) {
      CFIndex సంకేతం_2 = CFError_Get_సంకేతం(error);
      error_ref = CFErrorCopyDescription(error);

      ఒకవేళ(error_ref) {
        CFIndex కొలత = CFString_Get_గరిష్ఠ_కొలత_For_Encoding(
          CFString_Get_నిడివి(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(కొలత + 1);
        ఒకవేళ(err_desc) {
          ఒకవేళ(!CFStringGetCString(error_ref, err_desc, కొలత,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(దత్తాంశం, "Apple SecTrust failure %ld%s%s", సంకేతం_2,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      ఒకవేళ(సంకేతం_2 == errSecIncompleteCertRevocationCheck)
        ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  లేకపోతే
#అయితే_ముగింపు /* SUPPORTS_SecTrustEvaluateWithError */
  {
#నిర్వచితం_కాకపోతే REQUIRES_SecTrustEvaluateWithError
    Sec_Trust_ఫలితం_Type sec_ఫలితం;
    స్థితి_2 = SecTrustEvaluate(trust, &sec_ఫలితం);

    ఒకవేళ(స్థితి_2 != noErr) {
      failf(దత్తాంశం, "Apple SecTrust verification failed: error %d", (పూర్ణాంకం)స్థితి_2);
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    }
    లేకపోతే ఒకవేళ((sec_ఫలితం == k_Sec_Trust_ఫలితం_Unspecified) ||
            (sec_ఫలితం == k_Sec_Trust_ఫలితం_Proceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      ఫలితం = CURLE_OK;
    }
    లేకపోతే {
      /* Any other trust result is a verification failure in this context */
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    }
#అయితే_ముగింపు /* REQUIRES_SecTrustEvaluateWithError */
  }

బహిర్గతం:
  curlx_free(err_desc);
  ఒకవేళ(error_ref)
    CFRelease(error_ref);
  ఒకవేళ(error)
    CFRelease(error);
  ఒకవేళ(host_str)
    CFRelease(host_str);
  ఒకవేళ(policies)
    CFRelease(policies);
  ఒకవేళ(policy)
    CFRelease(policy);
  ఒకవేళ(cert_array)
    CFRelease(cert_array);
  ఒకవేళ(trust)
    CFRelease(trust);
  తిరిగి ఫలితం;
}

bool Curl_vtls_apple_verifies_ocsp(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
#ఒకవేళ defined(HAVE_BUILTIN_అందుబాటు) && defined(SUPPORTS_SecOCSP)
  తిరిగి TRUE;
#లేకపోతే
  తిరిగి FALSE;
#అయితే_ముగింపు
}

#అయితే_ముగింపు /* USE_APPLE_SECTRUST */
