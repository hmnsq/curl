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

#جيڪڏهن_وصف_ٿيل USE_SCHANNEL

#جيڪڏهن_وصف_نه_ٿيل USE_WINDOWS_SSPI
#غلطي "cannot compile Schannel support without SSPI."
#جيڪڏهن_پورو

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

#وصف_ڪريو BACKEND ((بنية schannel_ssl_backend_معلومات *)connssl->backend)

#وصف_ڪريو MAX_CAFILE_سائيز (1024 * 1024) /* 1 MiB */
#وصف_ڪريو BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#وصف_ڪريو پڇاڙي_CERT        "\n-----END CERTIFICATE-----"

بنية cert_chain_engine_config_win8 {
  DWORD cb_سائيز;
  HCERTSTORE h_Restricted_پاڙ;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD وڌ_کان_وڌ_Cached_Certificates;
  DWORD چڪر_Detection_Modulus;
  HCERTSTORE h_Exclusive_پاڙ;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#جيڪڏهن_وصف_نه_ٿيل CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#وصف_ڪريو CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#جيڪڏهن_پورو

/* Legacy structure to supply size to Win7 clients */
بنية cert_chain_engine_config_win7 {
  DWORD cb_سائيز;
  HCERTSTORE h_Restricted_پاڙ;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD وڌ_کان_وڌ_Cached_Certificates;
  DWORD چڪر_Detection_Modulus;
  HCERTSTORE h_Exclusive_پاڙ;
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
  حرف پهريون;
  ثابت حرف *str_حد_بندي = (ثابت حرف *)haystack + haystacklen;
  إذا(!needlelen || needlelen > haystacklen)
    أرجع عدم;
  پهريون = *(ثابت حرف *)needle;
  لكل(p = (ثابت حرف *)haystack; p <= (str_حد_بندي - needlelen); p++)
    إذا((*p == پهريون) && !memcmp(p, needle, needlelen))
      أرجع p;

  أرجع عدم;
}

ساكن CURLcode جوڙيو_certs_معلومات_منزل_store(HCERTSTORE trust_store,
                                        ثابت حرف *ca_بفر,
                                        size_t ca_بفر_سائيز,
                                        ثابت حرف *ca_file_متن,
                                        بنية Curl_easy *معلومات)
{
  ثابت size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  ثابت size_t پڇاڙي_cert_len = CURL_CSTRLEN(پڇاڙي_CERT);
  CURLcode نتيجو = CURLE_OK;
  صحيح انگ_certs = 0;
  bool وڌيڪ_certs = 1;
  ثابت حرف *current_ca_file_ptr = ca_بفر;
  ثابت حرف *ca_بفر_حد_بندي = ca_بفر + ca_بفر_سائيز;

  طالما(وڌيڪ_certs && (current_ca_file_ptr < ca_بفر_حد_بندي)) {
    ثابت حرف *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_بفر_حد_بندي -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    إذا(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      وڌيڪ_certs = 0;
    }
    وإلا {
      ثابت حرف *پڇاڙي_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_بفر_حد_بندي-begin_cert_ptr,
                                          پڇاڙي_CERT,
                                          پڇاڙي_cert_len);
      إذا(!پڇاڙي_cert_ptr) {
        failf(معلومات,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_متن);
        نتيجو = CURLE_SSL_CACERT_BADFILE;
        وڌيڪ_certs = 0;
      }
      وإلا {
        CERT_BLOB cert_blob;
        ثابت CERT_CONTEXT *cert_context = عدم;
        BOOL جوڙيو_cert_نتيجو = FALSE;
        DWORD cert_سائيز =
          (DWORD)((پڇاڙي_cert_ptr + پڇاڙي_cert_len) - begin_cert_ptr);

        cert_blob.pb_معلومات = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cb_معلومات = cert_سائيز;
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
          حرف بفر[WINAPI_ERROR_LEN];
          failf(معلومات,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_متن,
                curlx_winapi_strerror(Get_آخري_Error(), بفر, حجم(بفر)));
          نتيجو = CURLE_SSL_CACERT_BADFILE;
          وڌيڪ_certs = 0;
        }
        وإلا {
          current_ca_file_ptr = begin_cert_ptr + cert_سائيز;

          جوڙيو_cert_نتيجو =
            Cert_جوڙيو_Certificate_Context_منزل_Store(trust_store,
                                             cert_context,
                                             CERT_STORE_جوڙيو_ALWAYS,
                                             عدم);
          إذا(!جوڙيو_cert_نتيجو) {
            حرف بفر[WINAPI_ERROR_LEN];
            failf(معلومات,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_متن,
                  curlx_winapi_strerror(Get_آخري_Error(), بفر,
                                        حجم(بفر)));
            نتيجو = CURLE_SSL_CACERT_BADFILE;
            وڌيڪ_certs = 0;
          }
          وإلا {
            انگ_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  إذا(نتيجو == CURLE_OK) {
    إذا(!انگ_certs) {
      infof(معلومات, "schannel: did not add any certificates from CA file '%s'",
            ca_file_متن);
    }
    وإلا {
      infof(معلومات, "schannel: added %d certificate(s) from CA file '%s'",
            انگ_certs, ca_file_متن);
    }
  }
  أرجع نتيجو;
}

ساكن CURLcode جوڙيو_certs_file_منزل_store(HCERTSTORE trust_store,
                                        ثابت حرف *ca_file,
                                        بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;
  FILE *ca_file_handle;
  حرف *ca_file_بفر = عدم;
  ڊگهو ca_file_bufsize = 0;
  ڊگهو ڪل_بائيٽون_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  إذا(!ca_file_handle) {
    failf(معلومات, "schannel: failed to open CA file '%s'", ca_file);
    نتيجو = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  إذا(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(معلومات, "schannel: failed seeking to end of CA file '%s'", ca_file);
    نتيجو = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  إذا(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(معلومات, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    نتيجو = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  إذا(ca_file_bufsize < 0) {
    failf(معلومات, "schannel: failed to get length of CA file '%s'", ca_file);
    نتيجو = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  إذا(ca_file_bufsize > MAX_CAFILE_سائيز) {
    failf(معلومات, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_سائيز);
    نتيجو = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  ca_file_بفر = curlx_malloc(ca_file_bufsize + 1);
  إذا(!ca_file_بفر) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب cleanup;
  }

  طالما(ڪل_بائيٽون_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_بفر + ڪل_بائيٽون_read, 1,
                         ca_file_bufsize - ڪل_بائيٽون_read, ca_file_handle);

    إذا(ferror(ca_file_handle)) {
      failf(معلومات, "schannel: failed to read from CA file '%s'", ca_file);
      نتيجو = CURLE_SSL_CACERT_BADFILE;
      اذهب cleanup;
    }

    إذا(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = ڪل_بائيٽون_read;
    }
    وإلا {
      ڪل_بائيٽون_read += (ڊگهو)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_بفر[ca_file_bufsize] = '\0';

  نتيجو = جوڙيو_certs_معلومات_منزل_store(trust_store,
                                   ca_file_بفر, ca_file_bufsize,
                                   ca_file,
                                   معلومات);

cleanup:
  إذا(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_بفر);

  أرجع نتيجو;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
ساكن DWORD cert_get_name_string(بنية Curl_easy *معلومات,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_نالا,
                                  DWORD ڊيگهائي,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_ڊيگهائي = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = عدم;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  إذا(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#جيڪڏهن_وصف_نه_ٿيل CERT_NAME_ڳولا_ALL_نالا_FLAG
#وصف_ڪريو CERT_NAME_ڳولا_ALL_نالا_FLAG 0x2
#جيڪڏهن_پورو
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_ڳولا_ALL_نالا_FLAG;
    actual_ڊيگهائي = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      عدم,
                                      host_نالا,
                                      ڊيگهائي);
    أرجع actual_ڊيگهائي;
  }

  إذا(!alt_name_info)
    أرجع 0;

  compute_content = host_نالا && ڊيگهائي != 0;

  /* Initialize default return values. */
  actual_ڊيگهائي = 1;
  إذا(compute_content) {
    *host_نالا = '\0';
  }

  current_pos = host_نالا;

  /* Iterate over the alternate names and populate host_names. */
  لكل(i = 0; i < alt_name_info->c_Alt_داخلا; i++) {
    ثابت CERT_ALT_NAME_داخلا *داخلا_2 = &alt_name_info->rg_Alt_داخلا[i];
    wchar_t *dns_w = عدم;
    size_t current_ڊيگهائي = 0;

    إذا(داخلا_2->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      استمر;
    }
    إذا(!داخلا_2->pwszDNSName) {
      infof(معلومات, "schannel: Empty DNS name.");
      استمر;
    }
    current_ڊيگهائي = wcslen(داخلا_2->pwszDNSName) + 1;
    إذا(!compute_content) {
      actual_ڊيگهائي += (DWORD)current_ڊيگهائي;
      استمر;
    }
    /* Sanity check to prevent buffer overrun. */
    إذا((actual_ڊيگهائي + current_ڊيگهائي) > ڊيگهائي) {
      failf(معلومات, "schannel: Not enough memory to list all hostnames.");
      اكسر;
    }
    dns_w = داخلا_2->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    طالما(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_ڊيگهائي += (DWORD)current_ڊيگهائي;
  }
  إذا(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  أرجع actual_ڊيگهائي;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

ساكن bool get_انگ_host_info(بنية انگ_ip_معلومات *ip_blob, LPCSTR hostname)
{
  بنية in_addr ia;
  صحيح res = curlx_inet_pton(AF_INET, hostname, &ia);
  إذا(res) {
    ip_blob->سائيز = حجم(بنية in_addr);
    يادگيري_نقل(&ip_blob->b_معلومات.ia, &ia, حجم(بنية in_addr));
    أرجع TRUE;
  }
  وإلا {
    بنية in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    إذا(res) {
      ip_blob->سائيز = حجم(بنية in6_addr);
      يادگيري_نقل(&ip_blob->b_معلومات.ia6, &ia6, حجم(بنية in6_addr));
      أرجع TRUE;
    }
  }
  أرجع FALSE;
}

ساكن bool get_alt_name_info(بنية Curl_easy *معلومات,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_سائيز)
{
  PCERT_INFO cert_info = عدم;
  PCERT_EXTENSION extension = عدم;
  CRYPT_DECODE_PARA decode_para = { حجم(CRYPT_DECODE_PARA), عدم, عدم };

  إذا(!ctx) {
    failf(معلومات, "schannel: Null certificate context.");
    أرجع FALSE;
  }

  cert_info = ctx->pCertInfo;
  إذا(!cert_info) {
    failf(معلومات, "schannel: Null certificate info.");
    أرجع FALSE;
  }

  extension = Cert_ڳوليو_Extension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  إذا(!extension) {
    failf(معلومات, "schannel: CertFindExtension() returned no extension.");
    أرجع FALSE;
  }

  إذا(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->قدر_2.pb_معلومات,
                          extension->قدر_2.cb_معلومات,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_سائيز)) {
    failf(معلومات, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    أرجع FALSE;
  }
  أرجع TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  CURLcode نتيجو = CURLE_PEER_FAILED_VERIFICATION;
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = عدم;
  SECURITY_حالت sspi_حالت;
  TCHAR *cert_hostname_buff = عدم;
  size_t cert_hostname_buff_اشاريو = 0;
  ثابت حرف *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = لڙي_ڊيگهه(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = عدم;
  DWORD alt_name_info_سائيز = 0;
  بنية انگ_ip_معلومات ip_blob = { 0 };
  bool Win8_compat;
  بنية انگ_ip_معلومات *p = &ip_blob;
  DWORD i;

  sspi_حالت =
    Curl_pSecFn->Query_Context_خاصيتون(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  إذا((sspi_حالت != SEC_E_OK) || !pCertContextServer) {
    حرف بفر[WINAPI_ERROR_LEN];
    failf(معلومات, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    اذهب cleanup;
  }

  Win8_compat = curlx_verify_windows_نسخو(6, 2, 0, PLATFORM_WINNT,
                                             نسخو_GREATER_THAN_EQUAL);
  إذا(get_انگ_host_info(p, conn_hostname) || !Win8_compat) {
    إذا(!get_alt_name_info(معلومات, pCertContextServer,
                          &alt_name_info, &alt_name_info_سائيز)) {
      اذهب cleanup;
    }
  }

  إذا(p->سائيز && alt_name_info) {
    لكل(i = 0; i < alt_name_info->c_Alt_داخلا; ++i) {
      PCERT_ALT_NAME_داخلا داخلا_2 = &alt_name_info->rg_Alt_داخلا[i];
      إذا(داخلا_2->dwAltNameChoice == CERT_ALT_NAME_IP_پتو) {
        إذا(داخلا_2->IPAddress.cb_معلومات == p->سائيز) {
          إذا(!memcmp(داخلا_2->IPAddress.pb_معلومات, &p->b_معلومات,
                     داخلا_2->IPAddress.cb_معلومات)) {
            نتيجو = CURLE_OK;
            infof(معلومات,
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
    len = cert_get_name_string(معلومات, pCertContextServer,
                               عدم, 0, alt_name_info, Win8_compat);
    إذا(len == 0) {
      failf(معلومات,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      اذهب cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * حجم(TCHAR));
    إذا(!cert_hostname_buff) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب cleanup;
    }
    actual_len = cert_get_name_string(معلومات, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    إذا(actual_len != len) {
      failf(معلومات,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      اذهب cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    طالما(cert_hostname_buff_اشاريو < len &&
          cert_hostname_buff[cert_hostname_buff_اشاريو] != متن_2('\0') &&
          نتيجو == CURLE_PEER_FAILED_VERIFICATION) {
      حرف *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_منزل_UTF8(
        &cert_hostname_buff[cert_hostname_buff_اشاريو]);
      إذا(!cert_hostname) {
        نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      }
      وإلا {
        إذا(Curl_cert_hostcheck(cert_hostname, لڙي_ڊيگهه(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(معلومات, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          نتيجو = CURLE_OK;
        }
        وإلا {
          size_t cert_hostname_len;

          infof(معلومات,
                "schannel: connection hostname (%s) did not match "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);

          cert_hostname_len =
            _tcslen(&cert_hostname_buff[cert_hostname_buff_اشاريو]);

          /* Move on to next cert name */
          cert_hostname_buff_اشاريو += cert_hostname_len + 1;

          نتيجو = CURLE_PEER_FAILED_VERIFICATION;
        }
        curlx_free(cert_hostname);
      }
    }

    إذا(نتيجو == CURLE_PEER_FAILED_VERIFICATION) {
      failf(معلومات,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    وإلا إذا(نتيجو != CURLE_OK)
      failf(معلومات, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  إذا(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  أرجع نتيجو;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_حالت sspi_حالت;
  CURLcode نتيجو = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = عدم;
  ثابت CERT_CHAIN_CONTEXT *pChainContext = عدم;
  HCERTCHAINENGINE cert_chain_engine = عدم;
  HCERTSTORE trust_store = عدم;
  HCERTSTORE own_trust_store = عدم;

  DEBUGASSERT(BACKEND);

  sspi_حالت =
    Curl_pSecFn->Query_Context_خاصيتون(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  إذا((sspi_حالت != SEC_E_OK) || !pCertContextServer) {
    حرف بفر[WINAPI_ERROR_LEN];
    failf(معلومات, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_حالت, بفر, حجم(بفر)));
    نتيجو = CURLE_PEER_FAILED_VERIFICATION;
  }

  إذا(نتيجو == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    إذا(curlx_verify_windows_نسخو(6, 1, 0, PLATFORM_WINNT,
                                    نسخو_LESS_THAN)) {
      failf(معلومات, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      نتيجو = CURLE_SSL_CACERT_BADFILE;
    }
    وإلا {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, معلومات);

      إذا(trust_store) {
        infof(معلومات, "schannel: reusing certificate store from cache");
      }
      وإلا {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_يادگيري,
                                    0,
                                    (HCRYPTPROV)عدم,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    عدم);
        إذا(!trust_store) {
          حرف بفر[WINAPI_ERROR_LEN];
          failf(معلومات, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(Get_آخري_Error(), بفر, حجم(بفر)));
          نتيجو = CURLE_SSL_CACERT_BADFILE;
        }
        وإلا {
          ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          إذا(ca_info_blob) {
            نتيجو = جوڙيو_certs_معلومات_منزل_store(trust_store,
                                             (ثابت حرف *)ca_info_blob->معلومات,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             معلومات);
          }
          وإلا {
            نتيجو = جوڙيو_certs_file_منزل_store(trust_store,
                                             conn_config->CAfile,
                                             معلومات);
          }
          إذا(نتيجو == CURLE_OK) {
            إذا(Curl_schannel_سيٽ_cached_cert_store(cf, معلومات, trust_store)) {
              own_trust_store = عدم;
            }
          }
        }
      }
    }

    إذا(نتيجو == CURLE_OK) {
      بنية cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_نتيجو;

      memset(&engine_config, 0, حجم(engine_config));
      engine_config.h_Exclusive_پاڙ = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      إذا(curlx_verify_windows_نسخو(6, 2, 0, PLATFORM_WINNT,
                                      نسخو_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cb_سائيز = حجم(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      وإلا
        engine_config.cb_سائيز = حجم(بنية cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_نتيجو =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      إذا(!create_engine_نتيجو) {
        حرف بفر[WINAPI_ERROR_LEN];
        failf(معلومات,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(Get_آخري_Error(), بفر, حجم(بفر)));
        نتيجو = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  إذا(نتيجو == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, حجم(ChainPara));
    ChainPara.cb_سائيز = حجم(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_ميلاپ_TYPE_AND;
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
      حرف بفر[WINAPI_ERROR_LEN];
      failf(معلومات, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(Get_آخري_Error(), بفر, حجم(بفر)));
      pChainContext = عدم;
      نتيجو = CURLE_PEER_FAILED_VERIFICATION;
    }

    إذا(نتيجو == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dw_Trust_Error_نقاب = ~(DWORD)(CERT_TRUST_IS_NOT_وقت_NESTED);
      dw_Trust_Error_نقاب &= pSimpleChain->Trust_حالت.dw_Error_حالت;

      إذا(conn_config->revoke_بهترين_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dw_Trust_Error_نقاب &= ~(DWORD)(CERT_TRUST_REVOCATION_حالت_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      إذا(dw_Trust_Error_نقاب) {
        إذا(dw_Trust_Error_نقاب & CERT_TRUST_IS_REVOKED)
          failf(معلومات, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        وإلا إذا(dw_Trust_Error_نقاب & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(معلومات, "schannel: the certificate chain is incomplete");
        وإلا إذا(dw_Trust_Error_نقاب & CERT_TRUST_IS_UNTRUSTED_پاڙ)
          failf(معلومات, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        وإلا إذا(dw_Trust_Error_نقاب & CERT_TRUST_IS_NOT_وقت_VALID)
          failf(معلومات, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        وإلا إذا(dw_Trust_Error_نقاب & CERT_TRUST_REVOCATION_حالت_UNKNOWN)
          failf(معلومات, "schannel: the revocation status is unknown");
        وإلا
          failf(معلومات, "schannel: error 0x%08lx", dw_Trust_Error_نقاب);
        نتيجو = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  إذا(نتيجو == CURLE_OK) {
    إذا(conn_config->verifyhost) {
      نتيجو = Curl_verify_host(cf, معلومات);
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

  أرجع نتيجو;
}

#جيڪڏهن_پورو /* USE_SCHANNEL */
