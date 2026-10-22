/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Marc Hoersken, <info@marc-hoersken.de>
 * Copyright (C) Mark Salisbury, <mark.salisbury@hp.com>
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
/*
 * Source file for Schannel-specific certificate verification. This code should
 * only be invoked by code in schannel.c.
 */
#ضمن "curl_setup.h"

#اگر_تعریف_شده USE_SCHANNEL

#اگر_تعریف_نشده USE_WINDOWS_SSPI
#خطا "cannot compile Schannel support without SSPI."
#پایان_اگر

#ضمن "vtls/schannel.h"
#ضمن "vtls/schannel_int.h"

#ضمن "vtls/hostcheck.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_int.h"
#ضمن "curl_trc.h"
#ضمن "strerror.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "curlx/inet_pton.h"
#ضمن "curlx/multibyte.h"
#ضمن "curlx/version_win32.h"
#ضمن "curlx/winapi.h"

#تعریف_کن BACKEND ((بنية schannel_ssl_backend_داده *)connssl->backend)

#تعریف_کن MAX_CAFILE_اندازه (1024 * 1024) /* 1 MiB */
#تعریف_کن BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#تعریف_کن انتها_CERT        "\n-----END CERTIFICATE-----"

بنية cert_chain_engine_config_win8 {
  DWORD cb_اندازه;
  HCERTSTORE h_Restricted_ریشه;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD بیشینه_Cached_Certificates;
  DWORD چرخه_Detection_Modulus;
  HCERTSTORE h_Exclusive_ریشه;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#اگر_تعریف_نشده CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#تعریف_کن CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#پایان_اگر

/* Legacy structure to supply size to Win7 clients */
بنية cert_chain_engine_config_win7 {
  DWORD cb_اندازه;
  HCERTSTORE h_Restricted_ریشه;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD بیشینه_Cached_Certificates;
  DWORD چرخه_Detection_Modulus;
  HCERTSTORE h_Exclusive_ریشه;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
ساكن ثابت حرف *c_memmem(ثابت فراغ *haystack, size_t haystacklen,
                            ثابت فراغ *needle, size_t needlelen)
{
  ثابت حرف *p;
  حرف اولی;
  ثابت حرف *str_حد = (ثابت حرف *)haystack + haystacklen;
  إذا(!needlelen || needlelen > haystacklen)
    أرجع عدم;
  اولی = *(ثابت حرف *)needle;
  لكل(p = (ثابت حرف *)haystack; p <= (str_حد - needlelen); p++)
    إذا((*p == اولی) && !memcmp(p, needle, needlelen))
      أرجع p;

  أرجع عدم;
}

ساكن CURLcode جمع_کن_certs_داده_مقصد_store(HCERTSTORE trust_store,
                                        ثابت حرف *ca_میانگیر,
                                        size_t ca_میانگیر_اندازه,
                                        ثابت حرف *ca_file_متن,
                                        بنية Curl_easy *داده)
{
  ثابت size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  ثابت size_t انتها_cert_len = CURL_CSTRLEN(انتها_CERT);
  CURLcode نتیجه = CURLE_OK;
  صحيح رقم_certs = 0;
  bool بیشتر_certs = 1;
  ثابت حرف *current_ca_file_ptr = ca_میانگیر;
  ثابت حرف *ca_میانگیر_حد = ca_میانگیر + ca_میانگیر_اندازه;

  طالما(بیشتر_certs && (current_ca_file_ptr < ca_میانگیر_حد)) {
    ثابت حرف *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_میانگیر_حد -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    إذا(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      بیشتر_certs = 0;
    }
    وإلا {
      ثابت حرف *انتها_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_میانگیر_حد-begin_cert_ptr,
                                          انتها_CERT,
                                          انتها_cert_len);
      إذا(!انتها_cert_ptr) {
        failf(داده,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_متن);
        نتیجه = CURLE_SSL_CACERT_BADFILE;
        بیشتر_certs = 0;
      }
      وإلا {
        CERT_BLOB cert_blob;
        ثابت CERT_CONTEXT *cert_context = عدم;
        BOOL جمع_کن_cert_نتیجه = FALSE;
        DWORD cert_اندازه =
          (DWORD)((انتها_cert_ptr + انتها_cert_len) - begin_cert_ptr);

        cert_blob.pb_داده = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cb_داده = cert_اندازه;
        /* Caution: CryptQueryObject() is deprecated */
        إذا(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             عدم,
                             عدم,
                             عدم,
                             عدم,
                             عدم,
                             (ثابت فراغ **)&cert_context)) {
          حرف میانگیر[WINAPI_ERROR_LEN];
          failf(داده,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_متن,
                curlx_winapi_strerror(Get_آخرین_Error(), میانگیر, حجم(میانگیر)));
          نتیجه = CURLE_SSL_CACERT_BADFILE;
          بیشتر_certs = 0;
        }
        وإلا {
          current_ca_file_ptr = begin_cert_ptr + cert_اندازه;

          جمع_کن_cert_نتیجه =
            Cert_جمع_کن_Certificate_Context_مقصد_Store(trust_store,
                                             cert_context,
                                             CERT_STORE_جمع_کن_ALWAYS,
                                             عدم);
          إذا(!جمع_کن_cert_نتیجه) {
            حرف میانگیر[WINAPI_ERROR_LEN];
            failf(داده,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_متن,
                  curlx_winapi_strerror(Get_آخرین_Error(), میانگیر,
                                        حجم(میانگیر)));
            نتیجه = CURLE_SSL_CACERT_BADFILE;
            بیشتر_certs = 0;
          }
          وإلا {
            رقم_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  إذا(نتیجه == CURLE_OK) {
    إذا(!رقم_certs) {
      infof(داده, "schannel: did not add any certificates from CA file '%s'",
            ca_file_متن);
    }
    وإلا {
      infof(داده, "schannel: added %d certificate(s) from CA file '%s'",
            رقم_certs, ca_file_متن);
    }
  }
  أرجع نتیجه;
}

ساكن CURLcode جمع_کن_certs_file_مقصد_store(HCERTSTORE trust_store,
                                        ثابت حرف *ca_file,
                                        بنية Curl_easy *داده)
{
  CURLcode نتیجه;
  FILE *ca_file_handle;
  حرف *ca_file_میانگیر = عدم;
  دراز ca_file_bufsize = 0;
  دراز مجموع_بایتها_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  إذا(!ca_file_handle) {
    failf(داده, "schannel: failed to open CA file '%s'", ca_file);
    نتیجه = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  إذا(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(داده, "schannel: failed seeking to end of CA file '%s'", ca_file);
    نتیجه = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  إذا(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(داده, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    نتیجه = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  إذا(ca_file_bufsize < 0) {
    failf(داده, "schannel: failed to get length of CA file '%s'", ca_file);
    نتیجه = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  إذا(ca_file_bufsize > MAX_CAFILE_اندازه) {
    failf(داده, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_اندازه);
    نتیجه = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  ca_file_میانگیر = curlx_malloc(ca_file_bufsize + 1);
  إذا(!ca_file_میانگیر) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب cleanup;
  }

  طالما(مجموع_بایتها_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_میانگیر + مجموع_بایتها_read, 1,
                         ca_file_bufsize - مجموع_بایتها_read, ca_file_handle);

    إذا(ferror(ca_file_handle)) {
      failf(داده, "schannel: failed to read from CA file '%s'", ca_file);
      نتیجه = CURLE_SSL_CACERT_BADFILE;
      اذهب cleanup;
    }

    إذا(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = مجموع_بایتها_read;
    }
    وإلا {
      مجموع_بایتها_read += (دراز)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_میانگیر[ca_file_bufsize] = '\0';

  نتیجه = جمع_کن_certs_داده_مقصد_store(trust_store,
                                   ca_file_میانگیر, ca_file_bufsize,
                                   ca_file,
                                   داده);

cleanup:
  إذا(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_میانگیر);

  أرجع نتیجه;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
ساكن DWORD cert_get_name_string(بنية Curl_easy *داده,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_نامها,
                                  DWORD درازی,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_درازی = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = عدم;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  إذا(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#اگر_تعریف_نشده CERT_NAME_جستجو_ALL_نامها_FLAG
#تعریف_کن CERT_NAME_جستجو_ALL_نامها_FLAG 0x2
#پایان_اگر
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_جستجو_ALL_نامها_FLAG;
    actual_درازی = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      عدم,
                                      host_نامها,
                                      درازی);
    أرجع actual_درازی;
  }

  إذا(!alt_name_info)
    أرجع 0;

  compute_content = host_نامها && درازی != 0;

  /* Initialize default return values. */
  actual_درازی = 1;
  إذا(compute_content) {
    *host_نامها = '\0';
  }

  current_pos = host_نامها;

  /* Iterate over the alternate names and populate host_names. */
  لكل(i = 0; i < alt_name_info->c_Alt_مدخل; i++) {
    ثابت CERT_ALT_NAME_مدخل *مدخل = &alt_name_info->rg_Alt_مدخل[i];
    wchar_t *dns_w = عدم;
    size_t current_درازی = 0;

    إذا(مدخل->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      استمر;
    }
    إذا(!مدخل->pwszDNSName) {
      infof(داده, "schannel: Empty DNS name.");
      استمر;
    }
    current_درازی = wcslen(مدخل->pwszDNSName) + 1;
    إذا(!compute_content) {
      actual_درازی += (DWORD)current_درازی;
      استمر;
    }
    /* Sanity check to prevent buffer overrun. */
    إذا((actual_درازی + current_درازی) > درازی) {
      failf(داده, "schannel: Not enough memory to list all hostnames.");
      اكسر;
    }
    dns_w = مدخل->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    طالما(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_درازی += (DWORD)current_درازی;
  }
  إذا(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  أرجع actual_درازی;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

ساكن bool get_رقم_host_info(بنية رقم_ip_داده *ip_blob, LPCSTR hostname)
{
  بنية in_addr ia;
  صحيح res = curlx_inet_pton(AF_INET, hostname, &ia);
  إذا(res) {
    ip_blob->اندازه = حجم(بنية in_addr);
    کپی_حافظه(&ip_blob->b_داده.ia, &ia, حجم(بنية in_addr));
    أرجع TRUE;
  }
  وإلا {
    بنية in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    إذا(res) {
      ip_blob->اندازه = حجم(بنية in6_addr);
      کپی_حافظه(&ip_blob->b_داده.ia6, &ia6, حجم(بنية in6_addr));
      أرجع TRUE;
    }
  }
  أرجع FALSE;
}

ساكن bool get_alt_name_info(بنية Curl_easy *داده,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_اندازه)
{
  PCERT_INFO cert_info = عدم;
  PCERT_EXTENSION extension = عدم;
  CRYPT_DECODE_PARA decode_para = { حجم(CRYPT_DECODE_PARA), عدم, عدم };

  إذا(!ctx) {
    failf(داده, "schannel: Null certificate context.");
    أرجع FALSE;
  }

  cert_info = ctx->pCertInfo;
  إذا(!cert_info) {
    failf(داده, "schannel: Null certificate info.");
    أرجع FALSE;
  }

  extension = Cert_پیدا_کن_Extension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  إذا(!extension) {
    failf(داده, "schannel: CertFindExtension() returned no extension.");
    أرجع FALSE;
  }

  إذا(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->مقدار_2.pb_داده,
                          extension->مقدار_2.cb_داده,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_اندازه)) {
    failf(داده, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    أرجع FALSE;
  }
  أرجع TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_PEER_FAILED_VERIFICATION;
  بنية ssl_connect_داده *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = عدم;
  SECURITY_وضعیت sspi_وضعیت;
  TCHAR *cert_hostname_buff = عدم;
  size_t cert_hostname_buff_شاخص = 0;
  ثابت حرف *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = طول_رشته(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = عدم;
  DWORD alt_name_info_اندازه = 0;
  بنية رقم_ip_داده ip_blob = { 0 };
  bool Win8_compat;
  بنية رقم_ip_داده *p = &ip_blob;
  DWORD i;

  sspi_وضعیت =
    Curl_pSecFn->Query_Context_صفات(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  إذا((sspi_وضعیت != SEC_E_OK) || !pCertContextServer) {
    حرف میانگیر[WINAPI_ERROR_LEN];
    failf(داده, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
    اذهب cleanup;
  }

  Win8_compat = curlx_verify_windows_نسخه(6, 2, 0, PLATFORM_WINNT,
                                             نسخه_GREATER_THAN_EQUAL);
  إذا(get_رقم_host_info(p, conn_hostname) || !Win8_compat) {
    إذا(!get_alt_name_info(داده, pCertContextServer,
                          &alt_name_info, &alt_name_info_اندازه)) {
      اذهب cleanup;
    }
  }

  إذا(p->اندازه && alt_name_info) {
    لكل(i = 0; i < alt_name_info->c_Alt_مدخل; ++i) {
      PCERT_ALT_NAME_مدخل مدخل = &alt_name_info->rg_Alt_مدخل[i];
      إذا(مدخل->dwAltNameChoice == CERT_ALT_NAME_IP_آدرس) {
        إذا(مدخل->IPAddress.cb_داده == p->اندازه) {
          إذا(!memcmp(مدخل->IPAddress.pb_داده, &p->b_داده,
                     مدخل->IPAddress.cb_داده)) {
            نتیجه = CURLE_OK;
            infof(داده,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            اكسر;
          }
        }
      }
    }
  }
  وإلا {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(داده, pCertContextServer,
                               عدم, 0, alt_name_info, Win8_compat);
    إذا(len == 0) {
      failf(داده,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      اذهب cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * حجم(TCHAR));
    إذا(!cert_hostname_buff) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب cleanup;
    }
    actual_len = cert_get_name_string(داده, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    إذا(actual_len != len) {
      failf(داده,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      اذهب cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    طالما(cert_hostname_buff_شاخص < len &&
          cert_hostname_buff[cert_hostname_buff_شاخص] != متن_2('\0') &&
          نتیجه == CURLE_PEER_FAILED_VERIFICATION) {
      حرف *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_مقصد_UTF8(
        &cert_hostname_buff[cert_hostname_buff_شاخص]);
      إذا(!cert_hostname) {
        نتیجه = CURLE_بیرون_از_حافظه;
      }
      وإلا {
        إذا(Curl_cert_hostcheck(cert_hostname, طول_رشته(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(داده, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          نتیجه = CURLE_OK;
        }
        وإلا {
          size_t cert_hostname_len;

          infof(داده,
                "schannel: connection hostname (%s) did not match "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);

          cert_hostname_len =
            _tcslen(&cert_hostname_buff[cert_hostname_buff_شاخص]);

          /* Move on to next cert name */
          cert_hostname_buff_شاخص += cert_hostname_len + 1;

          نتیجه = CURLE_PEER_FAILED_VERIFICATION;
        }
        curlx_free(cert_hostname);
      }
    }

    إذا(نتیجه == CURLE_PEER_FAILED_VERIFICATION) {
      failf(داده,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    وإلا إذا(نتیجه != CURLE_OK)
      failf(داده, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  إذا(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  أرجع نتیجه;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_وضعیت sspi_وضعیت;
  CURLcode نتیجه = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = عدم;
  ثابت CERT_CHAIN_CONTEXT *pChainContext = عدم;
  HCERTCHAINENGINE cert_chain_engine = عدم;
  HCERTSTORE trust_store = عدم;
  HCERTSTORE own_trust_store = عدم;

  DEBUGASSERT(BACKEND);

  sspi_وضعیت =
    Curl_pSecFn->Query_Context_صفات(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  إذا((sspi_وضعیت != SEC_E_OK) || !pCertContextServer) {
    حرف میانگیر[WINAPI_ERROR_LEN];
    failf(داده, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_وضعیت, میانگیر, حجم(میانگیر)));
    نتیجه = CURLE_PEER_FAILED_VERIFICATION;
  }

  إذا(نتیجه == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    إذا(curlx_verify_windows_نسخه(6, 1, 0, PLATFORM_WINNT,
                                    نسخه_LESS_THAN)) {
      failf(داده, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      نتیجه = CURLE_SSL_CACERT_BADFILE;
    }
    وإلا {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, داده);

      إذا(trust_store) {
        infof(داده, "schannel: reusing certificate store from cache");
      }
      وإلا {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_حافظه,
                                    0,
                                    (HCRYPTPROV)عدم,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    عدم);
        إذا(!trust_store) {
          حرف میانگیر[WINAPI_ERROR_LEN];
          failf(داده, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(Get_آخرین_Error(), میانگیر, حجم(میانگیر)));
          نتیجه = CURLE_SSL_CACERT_BADFILE;
        }
        وإلا {
          ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          إذا(ca_info_blob) {
            نتیجه = جمع_کن_certs_داده_مقصد_store(trust_store,
                                             (ثابت حرف *)ca_info_blob->داده,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             داده);
          }
          وإلا {
            نتیجه = جمع_کن_certs_file_مقصد_store(trust_store,
                                             conn_config->CAfile,
                                             داده);
          }
          إذا(نتیجه == CURLE_OK) {
            إذا(Curl_schannel_مجموعه_cached_cert_store(cf, داده, trust_store)) {
              own_trust_store = عدم;
            }
          }
        }
      }
    }

    إذا(نتیجه == CURLE_OK) {
      بنية cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_نتیجه;

      memset(&engine_config, 0, حجم(engine_config));
      engine_config.h_Exclusive_ریشه = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      إذا(curlx_verify_windows_نسخه(6, 2, 0, PLATFORM_WINNT,
                                      نسخه_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cb_اندازه = حجم(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      وإلا
        engine_config.cb_اندازه = حجم(بنية cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_نتیجه =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      إذا(!create_engine_نتیجه) {
        حرف میانگیر[WINAPI_ERROR_LEN];
        failf(داده,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(Get_آخرین_Error(), میانگیر, حجم(میانگیر)));
        نتیجه = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  إذا(نتیجه == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, حجم(ChainPara));
    ChainPara.cb_اندازه = حجم(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_تطابق_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    إذا(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                عدم,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                عدم,
                                &pChainContext)) {
      حرف میانگیر[WINAPI_ERROR_LEN];
      failf(داده, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(Get_آخرین_Error(), میانگیر, حجم(میانگیر)));
      pChainContext = عدم;
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    }

    إذا(نتیجه == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dw_Trust_Error_ماسک = ~(DWORD)(CERT_TRUST_IS_NOT_زمان_NESTED);
      dw_Trust_Error_ماسک &= pSimpleChain->Trust_وضعیت.dw_Error_وضعیت;

      إذا(conn_config->revoke_بهترین_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dw_Trust_Error_ماسک &= ~(DWORD)(CERT_TRUST_REVOCATION_وضعیت_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      إذا(dw_Trust_Error_ماسک) {
        إذا(dw_Trust_Error_ماسک & CERT_TRUST_IS_REVOKED)
          failf(داده, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        وإلا إذا(dw_Trust_Error_ماسک & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(داده, "schannel: the certificate chain is incomplete");
        وإلا إذا(dw_Trust_Error_ماسک & CERT_TRUST_IS_UNTRUSTED_ریشه)
          failf(داده, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        وإلا إذا(dw_Trust_Error_ماسک & CERT_TRUST_IS_NOT_زمان_VALID)
          failf(داده, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        وإلا إذا(dw_Trust_Error_ماسک & CERT_TRUST_REVOCATION_وضعیت_UNKNOWN)
          failf(داده, "schannel: the revocation status is unknown");
        وإلا
          failf(داده, "schannel: error 0x%08lx", dw_Trust_Error_ماسک);
        نتیجه = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  إذا(نتیجه == CURLE_OK) {
    إذا(conn_config->verifyhost) {
      نتیجه = Curl_verify_host(cf, داده);
    }
  }

  إذا(cert_chain_engine) {
    CertFreeCertificateChainEngine(cert_chain_engine);
  }

  إذا(own_trust_store) {
    CertCloseStore(own_trust_store, 0);
  }

  إذا(pChainContext)
    CertFreeCertificateChain(pChainContext);

  إذا(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  أرجع نتیجه;
}

#پایان_اگر /* USE_SCHANNEL */
