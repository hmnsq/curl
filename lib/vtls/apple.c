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

#समावेश "curl_setup.h"

#अगर_घोषित USE_APPLE_SECTRUST

#समावेश "urldata.h"
#समावेश "cfilters.h"
#समावेश "curl_trc.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/apple.h"

#समावेश <Security/Security.h>

#अगर (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 101400) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 120000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 50000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#घोषणा SUPPORTS_SecTrustEvaluateWithError 1
#पूर्ण_अगर

#अगर defined(SUPPORTS_SecTrustEvaluateWithError) && \
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
#घोषणा REQUIRES_SecTrustEvaluateWithError 1
#पूर्ण_अगर

#अगर defined(SUPPORTS_SecTrustEvaluateWithError) && \
  !defined(HAVE_BUILTIN_AVAILABLE) &&              \
  !defined(REQUIRES_SecTrustEvaluateWithError)
#अघोषित SUPPORTS_SecTrustEvaluateWithError
#पूर्ण_अगर

#अगर (defined(__MAC_OS_X_VERSION_MAX_ALLOWED) && \
             __MAC_OS_X_VERSION_MAX_ALLOWED >= 100900) || \
    (defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && \
             __IPHONE_OS_VERSION_MAX_ALLOWED >= 70000) || \
    (defined(__TV_OS_VERSION_MAX_ALLOWED) && \
             __TV_OS_VERSION_MAX_ALLOWED >= 90000) || \
    (defined(__WATCH_OS_VERSION_MAX_ALLOWED) && \
             __WATCH_OS_VERSION_MAX_ALLOWED >= 20000) || \
    (defined(__VISION_OS_VERSION_MAX_ALLOWED) && \
             __VISION_OS_VERSION_MAX_ALLOWED >= 10000)
#घोषणा SUPPORTS_SecOCSP 1
#पूर्ण_अगर

CURLcode Curl_vtls_apple_verify(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                काष्ठा ssl_peer *peer,
                                माप_प्रकार अंक_certs,
                                Curl_vtls_get_cert_der *der_cb,
                                व्योम *cb_user_तथ्य,
                                स्थिर अचिन्हित अक्षर *ocsp_buf,
                                माप_प्रकार ocsp_len,
                                bool require_ocsp)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  SecTrustRef trust = शून्य;
  SecPolicyRef policy = शून्य;
  CFMutableArrayRef policies = शून्य;
  CFMutableArrayRef cert_array = शून्य;
  CFStringRef host_str = शून्य;
  CFErrorRef error = शून्य;
  OSStatus status = noErr;
  CFStringRef error_ref = शून्य;
  अक्षर *err_desc = शून्य;
  माप_प्रकार i;

  अगर(conn_config->verifyhost) {
    host_str = CFStringCreateWithCString(शून्य,
      peer->sni ? peer->sni : peer->origin->hostname, kCFStringEncodingUTF8);
    अगर(!host_str) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }

  policies = CFArrayCreateMutable(शून्य, 2, &kCFTypeArrayCallBacks);
  अगर(!policies) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  policy = SecPolicyCreateSSL(true, host_str);
  अगर(!policy) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  CFArray_Append_मूल्य(policies, policy);
  CFRelease(policy);
  policy = शून्य;

#अगर defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  {
    अगर(!conn_config->no_revoke) {
      अगर(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
        /* Even without this set, validation seemingly-unavoidably fails
         * for certificates that trustd already knows to be revoked.
         * This policy further allows trustd to consult CRLs and OCSP data
         * to determine revocation status (which it may then cache). */
        CFOptionFlags revocation_flags = kSecRevocationUseAnyAvailableMethod;
        अगर(require_ocsp)
          revocation_flags |= kSecRevocationRequirePositiveResponse;
        policy = SecPolicyCreateRevocation(revocation_flags);
        अगर(!policy) {
          परिणाम = CURLE_OUT_OF_MEMORY;
          जाओ out;
        }

        CFArray_Append_मूल्य(policies, policy);
      }
    }
  }
#अन्यथा
  /* OCSP check not available in this Apple OS version. */
  अगर(require_ocsp) {
    failf(तथ्य, "Apple SecTrust version does not support OCSP checks");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ out;
  }

#पूर्ण_अगर

  cert_array = CFArrayCreateMutable(शून्य, अंक_certs, &kCFTypeArrayCallBacks);
  अगर(!cert_array) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  क्रम(i = 0; i < अंक_certs; i++) {
    SecCertificateRef cert;
    CFDataRef certdata;
    अचिन्हित अक्षर *der;
    माप_प्रकार der_len;

    परिणाम = der_cb(cf, तथ्य, cb_user_तथ्य, i, &der, &der_len);
    अगर(परिणाम)
      जाओ out;

    certdata = CFDataCreate(शून्य, der, (CFIndex)der_len);
    अगर(!certdata) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }

    cert = Sec_Certificate_Create_With_तथ्य(शून्य, certdata);
    CFRelease(certdata);
    अगर(!cert) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }

    CFArray_Append_मूल्य(cert_array, cert);
    CFRelease(cert);
  }

  status = SecTrustCreateWithCertificates(cert_array, policies, &trust);
  अगर(status != noErr || !trust) {
    failf(तथ्य, "Apple SecTrust: failed to create validation trust");
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    जाओ out;
  }

  अगर(ocsp_len > 0) {
    bool checked = FALSE;
#अगर defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
    अगर(__builtin_available(macOS 10.9, iOS 7, tvOS 9, watchOS 2, *)) {
      CFDataRef ocspdata = CFDataCreate(शून्य, ocsp_buf, (CFIndex)ocsp_len);
      अगर(!ocspdata) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
      status = SecTrustSetOCSPResponse(trust, ocspdata);
      CFRelease(ocspdata);
      अगर(status != noErr) {
        failf(तथ्य, "Apple SecTrust: failed to set OCSP response: %d",
              (पूर्णांक)status);
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;
        जाओ out;
      }
      checked = TRUE;
    }
#पूर्ण_अगर
    अगर(!checked) {
      (व्योम)ocsp_buf;
      failf(तथ्य, "Apple SecTrust: OCSP verification not supported");
      परिणाम = CURLE_NOT_BUILT_IN;
      जाओ out;
    }
  }

#अगर_घोषित SUPPORTS_SecTrustEvaluateWithError
#अगर_घोषित HAVE_BUILTIN_AVAILABLE
  अगर(__builtin_available(macOS 10.14, iOS 12, tvOS 12, watchOS 5, *)) {
#अन्यथा
  अगर(1) {
#पूर्ण_अगर
    परिणाम = SecTrustEvaluateWithError(trust, &error) ?
             CURLE_OK : CURLE_PEER_FAILED_VERIFICATION;
    अगर(error) {
      CFIndex code = CFErrorGetCode(error);
      error_ref = CFErrorCopyDescription(error);

      अगर(error_ref) {
        CFIndex आकार = CFString_Get_Maximum_आकार_For_Encoding(
          CFString_Get_लंबाई(error_ref), kCFStringEncodingUTF8);
        err_desc = curlx_malloc(आकार + 1);
        अगर(err_desc) {
          अगर(!CFStringGetCString(error_ref, err_desc, आकार,
                                 kCFStringEncodingUTF8))
            curlx_safefree(err_desc);
        }
      }
      failf(तथ्य, "Apple SecTrust failure %ld%s%s", code,
            err_desc ? ": " : "", err_desc ? err_desc : "");
      अगर(code == errSecIncompleteCertRevocationCheck)
        परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    }
  }
  अन्यथा
#पूर्ण_अगर /* SUPPORTS_SecTrustEvaluateWithError */
  {
#अगर_अघोषित REQUIRES_SecTrustEvaluateWithError
    Sec_Trust_परिणाम_Type sec_परिणाम;
    status = SecTrustEvaluate(trust, &sec_परिणाम);

    अगर(status != noErr) {
      failf(तथ्य, "Apple SecTrust verification failed: error %d", (पूर्णांक)status);
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
    अन्यथा अगर((sec_परिणाम == k_Sec_Trust_परिणाम_Unspecified) ||
            (sec_परिणाम == k_Sec_Trust_परिणाम_Proceed)) {
      /* "unspecified" means system-trusted with no explicit user setting */
      परिणाम = CURLE_OK;
    }
    अन्यथा {
      /* Any other trust result is a verification failure in this context */
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
#पूर्ण_अगर /* REQUIRES_SecTrustEvaluateWithError */
  }

out:
  curlx_free(err_desc);
  अगर(error_ref)
    CFRelease(error_ref);
  अगर(error)
    CFRelease(error);
  अगर(host_str)
    CFRelease(host_str);
  अगर(policies)
    CFRelease(policies);
  अगर(policy)
    CFRelease(policy);
  अगर(cert_array)
    CFRelease(cert_array);
  अगर(trust)
    CFRelease(trust);
  वापस परिणाम;
}

bool Curl_vtls_apple_verifies_ocsp(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  (व्योम)cf;
  (व्योम)तथ्य;
#अगर defined(HAVE_BUILTIN_AVAILABLE) && defined(SUPPORTS_SecOCSP)
  वापस TRUE;
#अन्यथा
  वापस FALSE;
#पूर्ण_अगर
}

#पूर्ण_अगर /* USE_APPLE_SECTRUST */
