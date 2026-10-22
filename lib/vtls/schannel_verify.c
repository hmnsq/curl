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

#ifdef USE_SCHANNEL

#ifndef USE_WINDOWS_SSPI
#error "cannot compile Schannel support without SSPI."
#endif

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

#define BACKEND ((ساختار schannel_ssl_backend_data *)connssl->backend)

#define MAX_CAFILE_SIZE (1024 * 1024) /* 1 MiB */
#define BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#define END_CERT        "\n-----END CERTIFICATE-----"

ساختار cert_chain_engine_config_win8 {
  DWORD cbSize;
  HCERTSTORE h_Restricted_ریشه;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_ریشه;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#define CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#endif

/* Legacy structure to supply size to Win7 clients */
ساختار cert_chain_engine_config_win7 {
  DWORD cbSize;
  HCERTSTORE h_Restricted_ریشه;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_ریشه;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
ایستا ثابت نویسه *c_memmem(ثابت تهی *haystack, size_t haystacklen,
                            ثابت تهی *needle, size_t needlelen)
{
  ثابت نویسه *p;
  نویسه first;
  ثابت نویسه *str_limit = (ثابت نویسه *)haystack + haystacklen;
  اگر(!needlelen || needlelen > haystacklen)
    بازگشت پوچ;
  first = *(ثابت نویسه *)needle;
  برای(p = (ثابت نویسه *)haystack; p <= (str_limit - needlelen); p++)
    اگر((*p == first) && !memcmp(p, needle, needlelen))
      بازگشت p;

  بازگشت پوچ;
}

ایستا CURLcode add_certs_data_to_store(HCERTSTORE trust_store,
                                        ثابت نویسه *ca_buffer,
                                        size_t ca_buffer_size,
                                        ثابت نویسه *ca_file_text,
                                        ساختار Curl_easy *data)
{
  ثابت size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  ثابت size_t end_cert_len = CURL_CSTRLEN(END_CERT);
  CURLcode result = CURLE_OK;
  صحیح رقم_certs = 0;
  bool more_certs = 1;
  ثابت نویسه *current_ca_file_ptr = ca_buffer;
  ثابت نویسه *ca_buffer_limit = ca_buffer + ca_buffer_size;

  وقتی(more_certs && (current_ca_file_ptr < ca_buffer_limit)) {
    ثابت نویسه *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_buffer_limit -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    اگر(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      more_certs = 0;
    }
    وگرنه {
      ثابت نویسه *end_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_buffer_limit-begin_cert_ptr,
                                          END_CERT,
                                          end_cert_len);
      اگر(!end_cert_ptr) {
        failf(data,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_text);
        result = CURLE_SSL_CACERT_BADFILE;
        more_certs = 0;
      }
      وگرنه {
        CERT_BLOB cert_blob;
        ثابت CERT_CONTEXT *cert_context = پوچ;
        BOOL add_cert_result = FALSE;
        DWORD cert_size =
          (DWORD)((end_cert_ptr + end_cert_len) - begin_cert_ptr);

        cert_blob.pbData = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cbData = cert_size;
        /* Caution: CryptQueryObject() is deprecated */
        اگر(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             پوچ,
                             پوچ,
                             پوچ,
                             پوچ,
                             پوچ,
                             (ثابت تهی **)&cert_context)) {
          نویسه buffer[WINAPI_ERROR_LEN];
          failf(data,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_text,
                curlx_winapi_strerror(GetLastError(), buffer, اندازه(buffer)));
          result = CURLE_SSL_CACERT_BADFILE;
          more_certs = 0;
        }
        وگرنه {
          current_ca_file_ptr = begin_cert_ptr + cert_size;

          add_cert_result =
            CertAddCertificateContextToStore(trust_store,
                                             cert_context,
                                             CERT_STORE_ADD_ALWAYS,
                                             پوچ);
          اگر(!add_cert_result) {
            نویسه buffer[WINAPI_ERROR_LEN];
            failf(data,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_text,
                  curlx_winapi_strerror(GetLastError(), buffer,
                                        اندازه(buffer)));
            result = CURLE_SSL_CACERT_BADFILE;
            more_certs = 0;
          }
          وگرنه {
            رقم_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  اگر(result == CURLE_OK) {
    اگر(!رقم_certs) {
      infof(data, "schannel: did not add any certificates from CA file '%s'",
            ca_file_text);
    }
    وگرنه {
      infof(data, "schannel: added %d certificate(s) from CA file '%s'",
            رقم_certs, ca_file_text);
    }
  }
  بازگشت result;
}

ایستا CURLcode add_certs_file_to_store(HCERTSTORE trust_store,
                                        ثابت نویسه *ca_file,
                                        ساختار Curl_easy *data)
{
  CURLcode result;
  FILE *ca_file_handle;
  نویسه *ca_file_buffer = پوچ;
  long ca_file_bufsize = 0;
  long total_bytes_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  اگر(!ca_file_handle) {
    failf(data, "schannel: failed to open CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    برو cleanup;
  }

  اگر(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(data, "schannel: failed seeking to end of CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    برو cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  اگر(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(data, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    برو cleanup;
  }

  اگر(ca_file_bufsize < 0) {
    failf(data, "schannel: failed to get length of CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    برو cleanup;
  }

  اگر(ca_file_bufsize > MAX_CAFILE_SIZE) {
    failf(data, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_SIZE);
    result = CURLE_SSL_CACERT_BADFILE;
    برو cleanup;
  }

  ca_file_buffer = curlx_malloc(ca_file_bufsize + 1);
  اگر(!ca_file_buffer) {
    result = CURLE_OUT_OF_MEMORY;
    برو cleanup;
  }

  وقتی(total_bytes_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_buffer + total_bytes_read, 1,
                         ca_file_bufsize - total_bytes_read, ca_file_handle);

    اگر(ferror(ca_file_handle)) {
      failf(data, "schannel: failed to read from CA file '%s'", ca_file);
      result = CURLE_SSL_CACERT_BADFILE;
      برو cleanup;
    }

    اگر(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = total_bytes_read;
    }
    وگرنه {
      total_bytes_read += (long)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_buffer[ca_file_bufsize] = '\0';

  result = add_certs_data_to_store(trust_store,
                                   ca_file_buffer, ca_file_bufsize,
                                   ca_file,
                                   data);

cleanup:
  اگر(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_buffer);

  بازگشت result;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
ایستا DWORD cert_get_name_string(ساختار Curl_easy *data,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_names,
                                  DWORD length,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_length = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = پوچ;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  اگر(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_NAME_جستجو_ALL_NAMES_FLAG
#define CERT_NAME_جستجو_ALL_NAMES_FLAG 0x2
#endif
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_جستجو_ALL_NAMES_FLAG;
    actual_length = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      پوچ,
                                      host_names,
                                      length);
    بازگشت actual_length;
  }

  اگر(!alt_name_info)
    بازگشت 0;

  compute_content = host_names && length != 0;

  /* Initialize default return values. */
  actual_length = 1;
  اگر(compute_content) {
    *host_names = '\0';
  }

  current_pos = host_names;

  /* Iterate over the alternate names and populate host_names. */
  برای(i = 0; i < alt_name_info->cAltEntry; i++) {
    ثابت CERT_ALT_NAME_ENTRY *entry = &alt_name_info->rgAltEntry[i];
    wchar_t *dns_w = پوچ;
    size_t current_length = 0;

    اگر(entry->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      ادامه;
    }
    اگر(!entry->pwszDNSName) {
      infof(data, "schannel: Empty DNS name.");
      ادامه;
    }
    current_length = wcslen(entry->pwszDNSName) + 1;
    اگر(!compute_content) {
      actual_length += (DWORD)current_length;
      ادامه;
    }
    /* Sanity check to prevent buffer overrun. */
    اگر((actual_length + current_length) > length) {
      failf(data, "schannel: Not enough memory to list all hostnames.");
      بشکن;
    }
    dns_w = entry->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    وقتی(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_length += (DWORD)current_length;
  }
  اگر(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  بازگشت actual_length;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

ایستا bool get_رقم_host_info(ساختار رقم_ip_data *ip_blob, LPCSTR hostname)
{
  ساختار in_addr ia;
  صحیح res = curlx_inet_pton(AF_INET, hostname, &ia);
  اگر(res) {
    ip_blob->size = اندازه(ساختار in_addr);
    memcpy(&ip_blob->bData.ia, &ia, اندازه(ساختار in_addr));
    بازگشت TRUE;
  }
  وگرنه {
    ساختار in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    اگر(res) {
      ip_blob->size = اندازه(ساختار in6_addr);
      memcpy(&ip_blob->bData.ia6, &ia6, اندازه(ساختار in6_addr));
      بازگشت TRUE;
    }
  }
  بازگشت FALSE;
}

ایستا bool get_alt_name_info(ساختار Curl_easy *data,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_size)
{
  PCERT_INFO cert_info = پوچ;
  PCERT_EXTENSION extension = پوچ;
  CRYPT_DECODE_PARA decode_para = { اندازه(CRYPT_DECODE_PARA), پوچ, پوچ };

  اگر(!ctx) {
    failf(data, "schannel: Null certificate context.");
    بازگشت FALSE;
  }

  cert_info = ctx->pCertInfo;
  اگر(!cert_info) {
    failf(data, "schannel: Null certificate info.");
    بازگشت FALSE;
  }

  extension = CertFindExtension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  اگر(!extension) {
    failf(data, "schannel: CertFindExtension() returned no extension.");
    بازگشت FALSE;
  }

  اگر(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->مقدار_2.pbData,
                          extension->مقدار_2.cbData,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_size)) {
    failf(data, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    بازگشت FALSE;
  }
  بازگشت TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
  ساختار ssl_connect_data *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = پوچ;
  SECURITY_STATUS sspi_status;
  TCHAR *cert_hostname_buff = پوچ;
  size_t cert_hostname_buff_index = 0;
  ثابت نویسه *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = strlen(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = پوچ;
  DWORD alt_name_info_size = 0;
  ساختار رقم_ip_data ip_blob = { 0 };
  bool Win8_compat;
  ساختار رقم_ip_data *p = &ip_blob;
  DWORD i;

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  اگر((sspi_status != SEC_E_OK) || !pCertContextServer) {
    نویسه buffer[WINAPI_ERROR_LEN];
    failf(data, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, buffer, اندازه(buffer)));
    برو cleanup;
  }

  Win8_compat = curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                             VERSION_GREATER_THAN_EQUAL);
  اگر(get_رقم_host_info(p, conn_hostname) || !Win8_compat) {
    اگر(!get_alt_name_info(data, pCertContextServer,
                          &alt_name_info, &alt_name_info_size)) {
      برو cleanup;
    }
  }

  اگر(p->size && alt_name_info) {
    برای(i = 0; i < alt_name_info->cAltEntry; ++i) {
      PCERT_ALT_NAME_ENTRY entry = &alt_name_info->rgAltEntry[i];
      اگر(entry->dwAltNameChoice == CERT_ALT_NAME_IP_ADDRESS) {
        اگر(entry->IPAddress.cbData == p->size) {
          اگر(!memcmp(entry->IPAddress.pbData, &p->bData,
                     entry->IPAddress.cbData)) {
            result = CURLE_OK;
            infof(data,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            بشکن;
          }
        }
      }
    }
  }
  وگرنه {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(data, pCertContextServer,
                               پوچ, 0, alt_name_info, Win8_compat);
    اگر(len == 0) {
      failf(data,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      برو cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * اندازه(TCHAR));
    اگر(!cert_hostname_buff) {
      result = CURLE_OUT_OF_MEMORY;
      برو cleanup;
    }
    actual_len = cert_get_name_string(data, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    اگر(actual_len != len) {
      failf(data,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      برو cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    وقتی(cert_hostname_buff_index < len &&
          cert_hostname_buff[cert_hostname_buff_index] != TEXT('\0') &&
          result == CURLE_PEER_FAILED_VERIFICATION) {
      نویسه *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_to_UTF8(
        &cert_hostname_buff[cert_hostname_buff_index]);
      اگر(!cert_hostname) {
        result = CURLE_OUT_OF_MEMORY;
      }
      وگرنه {
        اگر(Curl_cert_hostcheck(cert_hostname, strlen(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(data, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          result = CURLE_OK;
        }
        وگرنه {
          size_t cert_hostname_len;

          infof(data,
                "schannel: connection hostname (%s) did not match "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);

          cert_hostname_len =
            _tcslen(&cert_hostname_buff[cert_hostname_buff_index]);

          /* Move on to next cert name */
          cert_hostname_buff_index += cert_hostname_len + 1;

          result = CURLE_PEER_FAILED_VERIFICATION;
        }
        curlx_free(cert_hostname);
      }
    }

    اگر(result == CURLE_PEER_FAILED_VERIFICATION) {
      failf(data,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    وگرنه اگر(result != CURLE_OK)
      failf(data, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  اگر(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  بازگشت result;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status;
  CURLcode result = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = پوچ;
  ثابت CERT_CHAIN_CONTEXT *pChainContext = پوچ;
  HCERTCHAINENGINE cert_chain_engine = پوچ;
  HCERTSTORE trust_store = پوچ;
  HCERTSTORE own_trust_store = پوچ;

  DEBUGASSERT(BACKEND);

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  اگر((sspi_status != SEC_E_OK) || !pCertContextServer) {
    نویسه buffer[WINAPI_ERROR_LEN];
    failf(data, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, buffer, اندازه(buffer)));
    result = CURLE_PEER_FAILED_VERIFICATION;
  }

  اگر(result == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    اگر(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_LESS_THAN)) {
      failf(data, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      result = CURLE_SSL_CACERT_BADFILE;
    }
    وگرنه {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, data);

      اگر(trust_store) {
        infof(data, "schannel: reusing certificate store from cache");
      }
      وگرنه {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_MEMORY,
                                    0,
                                    (HCRYPTPROV)پوچ,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    پوچ);
        اگر(!trust_store) {
          نویسه buffer[WINAPI_ERROR_LEN];
          failf(data, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(GetLastError(), buffer, اندازه(buffer)));
          result = CURLE_SSL_CACERT_BADFILE;
        }
        وگرنه {
          ثابت ساختار curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          اگر(ca_info_blob) {
            result = add_certs_data_to_store(trust_store,
                                             (ثابت نویسه *)ca_info_blob->data,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             data);
          }
          وگرنه {
            result = add_certs_file_to_store(trust_store,
                                             conn_config->CAfile,
                                             data);
          }
          اگر(result == CURLE_OK) {
            اگر(Curl_schannel_set_cached_cert_store(cf, data, trust_store)) {
              own_trust_store = پوچ;
            }
          }
        }
      }
    }

    اگر(result == CURLE_OK) {
      ساختار cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_result;

      memset(&engine_config, 0, اندازه(engine_config));
      engine_config.h_Exclusive_ریشه = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      اگر(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cbSize = اندازه(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      وگرنه
        engine_config.cbSize = اندازه(ساختار cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_result =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      اگر(!create_engine_result) {
        نویسه buffer[WINAPI_ERROR_LEN];
        failf(data,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(GetLastError(), buffer, اندازه(buffer)));
        result = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  اگر(result == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, اندازه(ChainPara));
    ChainPara.cbSize = اندازه(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    اگر(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                پوچ,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                پوچ,
                                &pChainContext)) {
      نویسه buffer[WINAPI_ERROR_LEN];
      failf(data, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(GetLastError(), buffer, اندازه(buffer)));
      pChainContext = پوچ;
      result = CURLE_PEER_FAILED_VERIFICATION;
    }

    اگر(result == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dwTrustErrorMask = ~(DWORD)(CERT_TRUST_IS_NOT_TIME_NESTED);
      dwTrustErrorMask &= pSimpleChain->TrustStatus.dwErrorStatus;

      اگر(conn_config->revoke_best_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dwTrustErrorMask &= ~(DWORD)(CERT_TRUST_REVOCATION_STATUS_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      اگر(dwTrustErrorMask) {
        اگر(dwTrustErrorMask & CERT_TRUST_IS_REVOKED)
          failf(data, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        وگرنه اگر(dwTrustErrorMask & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(data, "schannel: the certificate chain is incomplete");
        وگرنه اگر(dwTrustErrorMask & CERT_TRUST_IS_UNTRUSTED_ریشه)
          failf(data, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        وگرنه اگر(dwTrustErrorMask & CERT_TRUST_IS_NOT_TIME_VALID)
          failf(data, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        وگرنه اگر(dwTrustErrorMask & CERT_TRUST_REVOCATION_STATUS_UNKNOWN)
          failf(data, "schannel: the revocation status is unknown");
        وگرنه
          failf(data, "schannel: error 0x%08lx", dwTrustErrorMask);
        result = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  اگر(result == CURLE_OK) {
    اگر(conn_config->verifyhost) {
      result = Curl_verify_host(cf, data);
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

  بازگشت result;
}

#endif /* USE_SCHANNEL */
