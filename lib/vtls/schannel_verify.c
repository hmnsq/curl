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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_SCHANNEL

#اگر_غیر_تعریف_شدہ USE_WINDOWS_SSPI
#غلطی "cannot compile Schannel support without SSPI."
#اگر_ختم

#شامل "vtls/schannel.h"
#شامل "vtls/schannel_int.h"

#شامل "vtls/hostcheck.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_int.h"
#شامل "curl_trc.h"
#شامل "strerror.h"
#شامل "curlx/win32-fopen.h"
#شامل "curlx/inet_pton.h"
#شامل "curlx/multibyte.h"
#شامل "curlx/version_win32.h"
#شامل "curlx/winapi.h"

#تعریف_کرو BACKEND ((ساخت schannel_ssl_backend_ڈیٹا *)connssl->backend)

#تعریف_کرو MAX_CAFILE_سائز (1024 * 1024) /* 1 MiB */
#تعریف_کرو BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#تعریف_کرو انتہا_CERT        "\n-----END CERTIFICATE-----"

ساخت cert_chain_engine_config_win8 {
  DWORD cb_سائز;
  HCERTSTORE h_Restricted_جڑ;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD زیادہ_سے_زیادہ_Cached_Certificates;
  DWORD چکر_Detection_Modulus;
  HCERTSTORE h_Exclusive_جڑ;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#اگر_غیر_تعریف_شدہ CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#تعریف_کرو CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#اگر_ختم

/* Legacy structure to supply size to Win7 clients */
ساخت cert_chain_engine_config_win7 {
  DWORD cb_سائز;
  HCERTSTORE h_Restricted_جڑ;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD زیادہ_سے_زیادہ_Cached_Certificates;
  DWORD چکر_Detection_Modulus;
  HCERTSTORE h_Exclusive_جڑ;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
ساکن ثابت حرف *c_memmem(ثابت خالی *haystack, size_t haystacklen,
                            ثابت خالی *needle, size_t needlelen)
{
  ثابت حرف *p;
  حرف پہلا;
  ثابت حرف *str_حد_بندی = (ثابت حرف *)haystack + haystacklen;
  اگر(!needlelen || needlelen > haystacklen)
    واپس تہی;
  پہلا = *(ثابت حرف *)needle;
  برائے(p = (ثابت حرف *)haystack; p <= (str_حد_بندی - needlelen); p++)
    اگر((*p == پہلا) && !memcmp(p, needle, needlelen))
      واپس p;

  واپس تہی;
}

ساکن CURLcode جوڑو_certs_ڈیٹا_مقصد_store(HCERTSTORE trust_store,
                                        ثابت حرف *ca_بفر,
                                        size_t ca_بفر_سائز,
                                        ثابت حرف *ca_file_متن,
                                        ساخت Curl_easy *ڈیٹا)
{
  ثابت size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  ثابت size_t انتہا_cert_len = CURL_CSTRLEN(انتہا_CERT);
  CURLcode نتیجہ = CURLE_OK;
  صحیح عدد_certs = 0;
  bool مزید_certs = 1;
  ثابت حرف *current_ca_file_ptr = ca_بفر;
  ثابت حرف *ca_بفر_حد_بندی = ca_بفر + ca_بفر_سائز;

  جب(مزید_certs && (current_ca_file_ptr < ca_بفر_حد_بندی)) {
    ثابت حرف *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_بفر_حد_بندی -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    اگر(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      مزید_certs = 0;
    }
    ورنہ {
      ثابت حرف *انتہا_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_بفر_حد_بندی-begin_cert_ptr,
                                          انتہا_CERT,
                                          انتہا_cert_len);
      اگر(!انتہا_cert_ptr) {
        failf(ڈیٹا,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_متن);
        نتیجہ = CURLE_SSL_CACERT_BADFILE;
        مزید_certs = 0;
      }
      ورنہ {
        CERT_BLOB cert_blob;
        ثابت CERT_CONTEXT *cert_context = تہی;
        BOOL جوڑو_cert_نتیجہ = FALSE;
        DWORD cert_سائز =
          (DWORD)((انتہا_cert_ptr + انتہا_cert_len) - begin_cert_ptr);

        cert_blob.pb_ڈیٹا = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cb_ڈیٹا = cert_سائز;
        /* Caution: CryptQueryObject() is deprecated */
        اگر(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             تہی,
                             تہی,
                             تہی,
                             تہی,
                             تہی,
                             (ثابت خالی **)&cert_context)) {
          حرف بفر[WINAPI_ERROR_LEN];
          failf(ڈیٹا,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_متن,
                curlx_winapi_strerror(Get_آخری_Error(), بفر, حجم(بفر)));
          نتیجہ = CURLE_SSL_CACERT_BADFILE;
          مزید_certs = 0;
        }
        ورنہ {
          current_ca_file_ptr = begin_cert_ptr + cert_سائز;

          جوڑو_cert_نتیجہ =
            Cert_جوڑو_Certificate_Context_مقصد_Store(trust_store,
                                             cert_context,
                                             CERT_STORE_جوڑو_ALWAYS,
                                             تہی);
          اگر(!جوڑو_cert_نتیجہ) {
            حرف بفر[WINAPI_ERROR_LEN];
            failf(ڈیٹا,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_متن,
                  curlx_winapi_strerror(Get_آخری_Error(), بفر,
                                        حجم(بفر)));
            نتیجہ = CURLE_SSL_CACERT_BADFILE;
            مزید_certs = 0;
          }
          ورنہ {
            عدد_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  اگر(نتیجہ == CURLE_OK) {
    اگر(!عدد_certs) {
      infof(ڈیٹا, "schannel: did not add any certificates from CA file '%s'",
            ca_file_متن);
    }
    ورنہ {
      infof(ڈیٹا, "schannel: added %d certificate(s) from CA file '%s'",
            عدد_certs, ca_file_متن);
    }
  }
  واپس نتیجہ;
}

ساکن CURLcode جوڑو_certs_file_مقصد_store(HCERTSTORE trust_store,
                                        ثابت حرف *ca_file,
                                        ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;
  FILE *ca_file_handle;
  حرف *ca_file_بفر = تہی;
  طویل ca_file_bufsize = 0;
  طویل کل_بائٹیں_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  اگر(!ca_file_handle) {
    failf(ڈیٹا, "schannel: failed to open CA file '%s'", ca_file);
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    جاؤ cleanup;
  }

  اگر(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(ڈیٹا, "schannel: failed seeking to end of CA file '%s'", ca_file);
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    جاؤ cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  اگر(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(ڈیٹا, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    جاؤ cleanup;
  }

  اگر(ca_file_bufsize < 0) {
    failf(ڈیٹا, "schannel: failed to get length of CA file '%s'", ca_file);
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    جاؤ cleanup;
  }

  اگر(ca_file_bufsize > MAX_CAFILE_سائز) {
    failf(ڈیٹا, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_سائز);
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    جاؤ cleanup;
  }

  ca_file_بفر = curlx_malloc(ca_file_bufsize + 1);
  اگر(!ca_file_بفر) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ cleanup;
  }

  جب(کل_بائٹیں_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_بفر + کل_بائٹیں_read, 1,
                         ca_file_bufsize - کل_بائٹیں_read, ca_file_handle);

    اگر(ferror(ca_file_handle)) {
      failf(ڈیٹا, "schannel: failed to read from CA file '%s'", ca_file);
      نتیجہ = CURLE_SSL_CACERT_BADFILE;
      جاؤ cleanup;
    }

    اگر(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = کل_بائٹیں_read;
    }
    ورنہ {
      کل_بائٹیں_read += (طویل)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_بفر[ca_file_bufsize] = '\0';

  نتیجہ = جوڑو_certs_ڈیٹا_مقصد_store(trust_store,
                                   ca_file_بفر, ca_file_bufsize,
                                   ca_file,
                                   ڈیٹا);

cleanup:
  اگر(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_بفر);

  واپس نتیجہ;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
ساکن DWORD cert_get_name_string(ساخت Curl_easy *ڈیٹا,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_نام,
                                  DWORD لمبائی,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_لمبائی = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = تہی;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  اگر(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#اگر_غیر_تعریف_شدہ CERT_NAME_تلاش_ALL_نام_FLAG
#تعریف_کرو CERT_NAME_تلاش_ALL_نام_FLAG 0x2
#اگر_ختم
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_تلاش_ALL_نام_FLAG;
    actual_لمبائی = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      تہی,
                                      host_نام,
                                      لمبائی);
    واپس actual_لمبائی;
  }

  اگر(!alt_name_info)
    واپس 0;

  compute_content = host_نام && لمبائی != 0;

  /* Initialize default return values. */
  actual_لمبائی = 1;
  اگر(compute_content) {
    *host_نام = '\0';
  }

  current_pos = host_نام;

  /* Iterate over the alternate names and populate host_names. */
  برائے(i = 0; i < alt_name_info->c_Alt_اندراج; i++) {
    ثابت CERT_ALT_NAME_اندراج *اندراج_2 = &alt_name_info->rg_Alt_اندراج[i];
    wchar_t *dns_w = تہی;
    size_t current_لمبائی = 0;

    اگر(اندراج_2->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      جاری;
    }
    اگر(!اندراج_2->pwszDNSName) {
      infof(ڈیٹا, "schannel: Empty DNS name.");
      جاری;
    }
    current_لمبائی = wcslen(اندراج_2->pwszDNSName) + 1;
    اگر(!compute_content) {
      actual_لمبائی += (DWORD)current_لمبائی;
      جاری;
    }
    /* Sanity check to prevent buffer overrun. */
    اگر((actual_لمبائی + current_لمبائی) > لمبائی) {
      failf(ڈیٹا, "schannel: Not enough memory to list all hostnames.");
      توڑو;
    }
    dns_w = اندراج_2->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    جب(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_لمبائی += (DWORD)current_لمبائی;
  }
  اگر(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  واپس actual_لمبائی;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

ساکن bool get_عدد_host_info(ساخت عدد_ip_ڈیٹا *ip_blob, LPCSTR hostname)
{
  ساخت in_addr ia;
  صحیح res = curlx_inet_pton(AF_INET, hostname, &ia);
  اگر(res) {
    ip_blob->سائز = حجم(ساخت in_addr);
    حافظہ_نقل(&ip_blob->b_ڈیٹا.ia, &ia, حجم(ساخت in_addr));
    واپس TRUE;
  }
  ورنہ {
    ساخت in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    اگر(res) {
      ip_blob->سائز = حجم(ساخت in6_addr);
      حافظہ_نقل(&ip_blob->b_ڈیٹا.ia6, &ia6, حجم(ساخت in6_addr));
      واپس TRUE;
    }
  }
  واپس FALSE;
}

ساکن bool get_alt_name_info(ساخت Curl_easy *ڈیٹا,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_سائز)
{
  PCERT_INFO cert_info = تہی;
  PCERT_EXTENSION extension = تہی;
  CRYPT_DECODE_PARA decode_para = { حجم(CRYPT_DECODE_PARA), تہی, تہی };

  اگر(!ctx) {
    failf(ڈیٹا, "schannel: Null certificate context.");
    واپس FALSE;
  }

  cert_info = ctx->pCertInfo;
  اگر(!cert_info) {
    failf(ڈیٹا, "schannel: Null certificate info.");
    واپس FALSE;
  }

  extension = Cert_ڈھونڈو_Extension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  اگر(!extension) {
    failf(ڈیٹا, "schannel: CertFindExtension() returned no extension.");
    واپس FALSE;
  }

  اگر(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->قدر_2.pb_ڈیٹا,
                          extension->قدر_2.cb_ڈیٹا,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_سائز)) {
    failf(ڈیٹا, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    واپس FALSE;
  }
  واپس TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = تہی;
  SECURITY_حالت sspi_حالت;
  TCHAR *cert_hostname_buff = تہی;
  size_t cert_hostname_buff_اشاریہ = 0;
  ثابت حرف *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = لڑی_لمبائی(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = تہی;
  DWORD alt_name_info_سائز = 0;
  ساخت عدد_ip_ڈیٹا ip_blob = { 0 };
  bool Win8_compat;
  ساخت عدد_ip_ڈیٹا *p = &ip_blob;
  DWORD i;

  sspi_حالت =
    Curl_pSecFn->Query_Context_اوصاف(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  اگر((sspi_حالت != SEC_E_OK) || !pCertContextServer) {
    حرف بفر[WINAPI_ERROR_LEN];
    failf(ڈیٹا, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    جاؤ cleanup;
  }

  Win8_compat = curlx_verify_windows_نسخہ(6, 2, 0, PLATFORM_WINNT,
                                             نسخہ_GREATER_THAN_EQUAL);
  اگر(get_عدد_host_info(p, conn_hostname) || !Win8_compat) {
    اگر(!get_alt_name_info(ڈیٹا, pCertContextServer,
                          &alt_name_info, &alt_name_info_سائز)) {
      جاؤ cleanup;
    }
  }

  اگر(p->سائز && alt_name_info) {
    برائے(i = 0; i < alt_name_info->c_Alt_اندراج; ++i) {
      PCERT_ALT_NAME_اندراج اندراج_2 = &alt_name_info->rg_Alt_اندراج[i];
      اگر(اندراج_2->dwAltNameChoice == CERT_ALT_NAME_IP_پتہ) {
        اگر(اندراج_2->IPAddress.cb_ڈیٹا == p->سائز) {
          اگر(!memcmp(اندراج_2->IPAddress.pb_ڈیٹا, &p->b_ڈیٹا,
                     اندراج_2->IPAddress.cb_ڈیٹا)) {
            نتیجہ = CURLE_OK;
            infof(ڈیٹا,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            توڑو;
          }
        }
      }
    }
  }
  ورنہ {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(ڈیٹا, pCertContextServer,
                               تہی, 0, alt_name_info, Win8_compat);
    اگر(len == 0) {
      failf(ڈیٹا,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      جاؤ cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * حجم(TCHAR));
    اگر(!cert_hostname_buff) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ cleanup;
    }
    actual_len = cert_get_name_string(ڈیٹا, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    اگر(actual_len != len) {
      failf(ڈیٹا,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      جاؤ cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    جب(cert_hostname_buff_اشاریہ < len &&
          cert_hostname_buff[cert_hostname_buff_اشاریہ] != متن_2('\0') &&
          نتیجہ == CURLE_PEER_FAILED_VERIFICATION) {
      حرف *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_مقصد_UTF8(
        &cert_hostname_buff[cert_hostname_buff_اشاریہ]);
      اگر(!cert_hostname) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      }
      ورنہ {
        اگر(Curl_cert_hostcheck(cert_hostname, لڑی_لمبائی(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(ڈیٹا, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          نتیجہ = CURLE_OK;
        }
        ورنہ {
          size_t cert_hostname_len;

          infof(ڈیٹا,
                "schannel: connection hostname (%s) did not match "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);

          cert_hostname_len =
            _tcslen(&cert_hostname_buff[cert_hostname_buff_اشاریہ]);

          /* Move on to next cert name */
          cert_hostname_buff_اشاریہ += cert_hostname_len + 1;

          نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
        }
        curlx_free(cert_hostname);
      }
    }

    اگر(نتیجہ == CURLE_PEER_FAILED_VERIFICATION) {
      failf(ڈیٹا,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    ورنہ اگر(نتیجہ != CURLE_OK)
      failf(ڈیٹا, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  اگر(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  واپس نتیجہ;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_حالت sspi_حالت;
  CURLcode نتیجہ = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = تہی;
  ثابت CERT_CHAIN_CONTEXT *pChainContext = تہی;
  HCERTCHAINENGINE cert_chain_engine = تہی;
  HCERTSTORE trust_store = تہی;
  HCERTSTORE own_trust_store = تہی;

  DEBUGASSERT(BACKEND);

  sspi_حالت =
    Curl_pSecFn->Query_Context_اوصاف(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  اگر((sspi_حالت != SEC_E_OK) || !pCertContextServer) {
    حرف بفر[WINAPI_ERROR_LEN];
    failf(ڈیٹا, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
  }

  اگر(نتیجہ == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    اگر(curlx_verify_windows_نسخہ(6, 1, 0, PLATFORM_WINNT,
                                    نسخہ_LESS_THAN)) {
      failf(ڈیٹا, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      نتیجہ = CURLE_SSL_CACERT_BADFILE;
    }
    ورنہ {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, ڈیٹا);

      اگر(trust_store) {
        infof(ڈیٹا, "schannel: reusing certificate store from cache");
      }
      ورنہ {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_حافظہ,
                                    0,
                                    (HCRYPTPROV)تہی,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    تہی);
        اگر(!trust_store) {
          حرف بفر[WINAPI_ERROR_LEN];
          failf(ڈیٹا, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(Get_آخری_Error(), بفر, حجم(بفر)));
          نتیجہ = CURLE_SSL_CACERT_BADFILE;
        }
        ورنہ {
          ثابت ساخت curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          اگر(ca_info_blob) {
            نتیجہ = جوڑو_certs_ڈیٹا_مقصد_store(trust_store,
                                             (ثابت حرف *)ca_info_blob->ڈیٹا,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             ڈیٹا);
          }
          ورنہ {
            نتیجہ = جوڑو_certs_file_مقصد_store(trust_store,
                                             conn_config->CAfile,
                                             ڈیٹا);
          }
          اگر(نتیجہ == CURLE_OK) {
            اگر(Curl_schannel_سیٹ_cached_cert_store(cf, ڈیٹا, trust_store)) {
              own_trust_store = تہی;
            }
          }
        }
      }
    }

    اگر(نتیجہ == CURLE_OK) {
      ساخت cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_نتیجہ;

      memset(&engine_config, 0, حجم(engine_config));
      engine_config.h_Exclusive_جڑ = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      اگر(curlx_verify_windows_نسخہ(6, 2, 0, PLATFORM_WINNT,
                                      نسخہ_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cb_سائز = حجم(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      ورنہ
        engine_config.cb_سائز = حجم(ساخت cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_نتیجہ =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      اگر(!create_engine_نتیجہ) {
        حرف بفر[WINAPI_ERROR_LEN];
        failf(ڈیٹا,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(Get_آخری_Error(), بفر, حجم(بفر)));
        نتیجہ = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  اگر(نتیجہ == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, حجم(ChainPara));
    ChainPara.cb_سائز = حجم(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_مطابقت_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    اگر(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                تہی,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                تہی,
                                &pChainContext)) {
      حرف بفر[WINAPI_ERROR_LEN];
      failf(ڈیٹا, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(Get_آخری_Error(), بفر, حجم(بفر)));
      pChainContext = تہی;
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    }

    اگر(نتیجہ == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dw_Trust_Error_نقاب = ~(DWORD)(CERT_TRUST_IS_NOT_وقت_NESTED);
      dw_Trust_Error_نقاب &= pSimpleChain->Trust_حالت.dw_Error_حالت;

      اگر(conn_config->revoke_بہترین_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dw_Trust_Error_نقاب &= ~(DWORD)(CERT_TRUST_REVOCATION_حالت_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      اگر(dw_Trust_Error_نقاب) {
        اگر(dw_Trust_Error_نقاب & CERT_TRUST_IS_REVOKED)
          failf(ڈیٹا, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        ورنہ اگر(dw_Trust_Error_نقاب & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(ڈیٹا, "schannel: the certificate chain is incomplete");
        ورنہ اگر(dw_Trust_Error_نقاب & CERT_TRUST_IS_UNTRUSTED_جڑ)
          failf(ڈیٹا, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        ورنہ اگر(dw_Trust_Error_نقاب & CERT_TRUST_IS_NOT_وقت_VALID)
          failf(ڈیٹا, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        ورنہ اگر(dw_Trust_Error_نقاب & CERT_TRUST_REVOCATION_حالت_UNKNOWN)
          failf(ڈیٹا, "schannel: the revocation status is unknown");
        ورنہ
          failf(ڈیٹا, "schannel: error 0x%08lx", dw_Trust_Error_نقاب);
        نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  اگر(نتیجہ == CURLE_OK) {
    اگر(conn_config->verifyhost) {
      نتیجہ = Curl_verify_host(cf, ڈیٹا);
    }
  }

  اگر(cert_chain_engine) {
    CertFreeCertificateChainEngine(cert_chain_engine);
  }

  اگر(own_trust_store) {
    CertCloseStore(own_trust_store, 0);
  }

  اگر(pChainContext)
    CertFreeCertificateChain(pChainContext);

  اگر(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  واپس نتیجہ;
}

#اگر_ختم /* USE_SCHANNEL */
