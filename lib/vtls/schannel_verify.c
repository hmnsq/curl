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

#ifdef USE_SCHANNEL

#ifndef USE_WINDOWS_SSPI
#error "cannot compile Schannel support without SSPI."
#endif

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

#define BACKEND ((بنية schannel_ssl_backend_data *)connssl->backend)

#define MAX_CAFILE_SIZE (1024 * 1024) /* 1 MiB */
#define BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#define END_CERT        "\n-----END CERTIFICATE-----"

بنية cert_chain_engine_config_win8 {
  DWORD cbSize;
  HCERTSTORE h_Restricted_جڑ;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_جڑ;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#define CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#endif

/* Legacy structure to supply size to Win7 clients */
بنية cert_chain_engine_config_win7 {
  DWORD cbSize;
  HCERTSTORE h_Restricted_جڑ;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_جڑ;
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
  حرف first;
  ثابت حرف *str_limit = (ثابت حرف *)haystack + haystacklen;
  إذا(!needlelen || needlelen > haystacklen)
    أرجع عدم;
  first = *(ثابت حرف *)needle;
  لكل(p = (ثابت حرف *)haystack; p <= (str_limit - needlelen); p++)
    إذا((*p == first) && !memcmp(p, needle, needlelen))
      أرجع p;

  أرجع عدم;
}

ساكن CURLcode add_certs_data_to_store(HCERTSTORE trust_store,
                                        ثابت حرف *ca_buffer,
                                        size_t ca_buffer_size,
                                        ثابت حرف *ca_file_text,
                                        بنية Curl_easy *data)
{
  ثابت size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  ثابت size_t end_cert_len = CURL_CSTRLEN(END_CERT);
  CURLcode result = CURLE_OK;
  صحيح عدد_certs = 0;
  bool more_certs = 1;
  ثابت حرف *current_ca_file_ptr = ca_buffer;
  ثابت حرف *ca_buffer_limit = ca_buffer + ca_buffer_size;

  طالما(more_certs && (current_ca_file_ptr < ca_buffer_limit)) {
    ثابت حرف *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_buffer_limit -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    إذا(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      more_certs = 0;
    }
    وإلا {
      ثابت حرف *end_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_buffer_limit-begin_cert_ptr,
                                          END_CERT,
                                          end_cert_len);
      إذا(!end_cert_ptr) {
        failf(data,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_text);
        result = CURLE_SSL_CACERT_BADFILE;
        more_certs = 0;
      }
      وإلا {
        CERT_BLOB cert_blob;
        ثابت CERT_CONTEXT *cert_context = عدم;
        BOOL add_cert_result = FALSE;
        DWORD cert_size =
          (DWORD)((end_cert_ptr + end_cert_len) - begin_cert_ptr);

        cert_blob.pbData = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cbData = cert_size;
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
          حرف buffer[WINAPI_ERROR_LEN];
          failf(data,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_text,
                curlx_winapi_strerror(GetLastError(), buffer, حجم(buffer)));
          result = CURLE_SSL_CACERT_BADFILE;
          more_certs = 0;
        }
        وإلا {
          current_ca_file_ptr = begin_cert_ptr + cert_size;

          add_cert_result =
            CertAddCertificateContextToStore(trust_store,
                                             cert_context,
                                             CERT_STORE_ADD_ALWAYS,
                                             عدم);
          إذا(!add_cert_result) {
            حرف buffer[WINAPI_ERROR_LEN];
            failf(data,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_text,
                  curlx_winapi_strerror(GetLastError(), buffer,
                                        حجم(buffer)));
            result = CURLE_SSL_CACERT_BADFILE;
            more_certs = 0;
          }
          وإلا {
            عدد_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  إذا(result == CURLE_OK) {
    إذا(!عدد_certs) {
      infof(data, "schannel: did not add any certificates from CA file '%s'",
            ca_file_text);
    }
    وإلا {
      infof(data, "schannel: added %d certificate(s) from CA file '%s'",
            عدد_certs, ca_file_text);
    }
  }
  أرجع result;
}

ساكن CURLcode add_certs_file_to_store(HCERTSTORE trust_store,
                                        ثابت حرف *ca_file,
                                        بنية Curl_easy *data)
{
  CURLcode result;
  FILE *ca_file_handle;
  حرف *ca_file_buffer = عدم;
  long ca_file_bufsize = 0;
  long total_bytes_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  إذا(!ca_file_handle) {
    failf(data, "schannel: failed to open CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  إذا(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(data, "schannel: failed seeking to end of CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  إذا(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(data, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  إذا(ca_file_bufsize < 0) {
    failf(data, "schannel: failed to get length of CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  إذا(ca_file_bufsize > MAX_CAFILE_SIZE) {
    failf(data, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_SIZE);
    result = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  ca_file_buffer = curlx_malloc(ca_file_bufsize + 1);
  إذا(!ca_file_buffer) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب cleanup;
  }

  طالما(total_bytes_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_buffer + total_bytes_read, 1,
                         ca_file_bufsize - total_bytes_read, ca_file_handle);

    إذا(ferror(ca_file_handle)) {
      failf(data, "schannel: failed to read from CA file '%s'", ca_file);
      result = CURLE_SSL_CACERT_BADFILE;
      اذهب cleanup;
    }

    إذا(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = total_bytes_read;
    }
    وإلا {
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
  إذا(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_buffer);

  أرجع result;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
ساكن DWORD cert_get_name_string(بنية Curl_easy *data,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_names,
                                  DWORD length,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_length = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = عدم;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  إذا(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_NAME_تلاش_ALL_NAMES_FLAG
#define CERT_NAME_تلاش_ALL_NAMES_FLAG 0x2
#endif
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_تلاش_ALL_NAMES_FLAG;
    actual_length = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      عدم,
                                      host_names,
                                      length);
    أرجع actual_length;
  }

  إذا(!alt_name_info)
    أرجع 0;

  compute_content = host_names && length != 0;

  /* Initialize default return values. */
  actual_length = 1;
  إذا(compute_content) {
    *host_names = '\0';
  }

  current_pos = host_names;

  /* Iterate over the alternate names and populate host_names. */
  لكل(i = 0; i < alt_name_info->cAltEntry; i++) {
    ثابت CERT_ALT_NAME_ENTRY *entry = &alt_name_info->rgAltEntry[i];
    wchar_t *dns_w = عدم;
    size_t current_length = 0;

    إذا(entry->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      استمر;
    }
    إذا(!entry->pwszDNSName) {
      infof(data, "schannel: Empty DNS name.");
      استمر;
    }
    current_length = wcslen(entry->pwszDNSName) + 1;
    إذا(!compute_content) {
      actual_length += (DWORD)current_length;
      استمر;
    }
    /* Sanity check to prevent buffer overrun. */
    إذا((actual_length + current_length) > length) {
      failf(data, "schannel: Not enough memory to list all hostnames.");
      اكسر;
    }
    dns_w = entry->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    طالما(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_length += (DWORD)current_length;
  }
  إذا(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  أرجع actual_length;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

ساكن bool get_عدد_host_info(بنية عدد_ip_data *ip_blob, LPCSTR hostname)
{
  بنية in_addr ia;
  صحيح res = curlx_inet_pton(AF_INET, hostname, &ia);
  إذا(res) {
    ip_blob->size = حجم(بنية in_addr);
    memcpy(&ip_blob->bData.ia, &ia, حجم(بنية in_addr));
    أرجع TRUE;
  }
  وإلا {
    بنية in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    إذا(res) {
      ip_blob->size = حجم(بنية in6_addr);
      memcpy(&ip_blob->bData.ia6, &ia6, حجم(بنية in6_addr));
      أرجع TRUE;
    }
  }
  أرجع FALSE;
}

ساكن bool get_alt_name_info(بنية Curl_easy *data,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_size)
{
  PCERT_INFO cert_info = عدم;
  PCERT_EXTENSION extension = عدم;
  CRYPT_DECODE_PARA decode_para = { حجم(CRYPT_DECODE_PARA), عدم, عدم };

  إذا(!ctx) {
    failf(data, "schannel: Null certificate context.");
    أرجع FALSE;
  }

  cert_info = ctx->pCertInfo;
  إذا(!cert_info) {
    failf(data, "schannel: Null certificate info.");
    أرجع FALSE;
  }

  extension = CertFindExtension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  إذا(!extension) {
    failf(data, "schannel: CertFindExtension() returned no extension.");
    أرجع FALSE;
  }

  إذا(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->قدر_2.pbData,
                          extension->قدر_2.cbData,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_size)) {
    failf(data, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    أرجع FALSE;
  }
  أرجع TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
  بنية ssl_connect_data *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = عدم;
  SECURITY_STATUS sspi_status;
  TCHAR *cert_hostname_buff = عدم;
  size_t cert_hostname_buff_index = 0;
  ثابت حرف *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = strlen(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = عدم;
  DWORD alt_name_info_size = 0;
  بنية عدد_ip_data ip_blob = { 0 };
  bool Win8_compat;
  بنية عدد_ip_data *p = &ip_blob;
  DWORD i;

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  إذا((sspi_status != SEC_E_OK) || !pCertContextServer) {
    حرف buffer[WINAPI_ERROR_LEN];
    failf(data, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, buffer, حجم(buffer)));
    اذهب cleanup;
  }

  Win8_compat = curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                             VERSION_GREATER_THAN_EQUAL);
  إذا(get_عدد_host_info(p, conn_hostname) || !Win8_compat) {
    إذا(!get_alt_name_info(data, pCertContextServer,
                          &alt_name_info, &alt_name_info_size)) {
      اذهب cleanup;
    }
  }

  إذا(p->size && alt_name_info) {
    لكل(i = 0; i < alt_name_info->cAltEntry; ++i) {
      PCERT_ALT_NAME_ENTRY entry = &alt_name_info->rgAltEntry[i];
      إذا(entry->dwAltNameChoice == CERT_ALT_NAME_IP_ADDRESS) {
        إذا(entry->IPAddress.cbData == p->size) {
          إذا(!memcmp(entry->IPAddress.pbData, &p->bData,
                     entry->IPAddress.cbData)) {
            result = CURLE_OK;
            infof(data,
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
    len = cert_get_name_string(data, pCertContextServer,
                               عدم, 0, alt_name_info, Win8_compat);
    إذا(len == 0) {
      failf(data,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      اذهب cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * حجم(TCHAR));
    إذا(!cert_hostname_buff) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب cleanup;
    }
    actual_len = cert_get_name_string(data, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    إذا(actual_len != len) {
      failf(data,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      اذهب cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    طالما(cert_hostname_buff_index < len &&
          cert_hostname_buff[cert_hostname_buff_index] != TEXT('\0') &&
          result == CURLE_PEER_FAILED_VERIFICATION) {
      حرف *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_to_UTF8(
        &cert_hostname_buff[cert_hostname_buff_index]);
      إذا(!cert_hostname) {
        result = CURLE_OUT_OF_MEMORY;
      }
      وإلا {
        إذا(Curl_cert_hostcheck(cert_hostname, strlen(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(data, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          result = CURLE_OK;
        }
        وإلا {
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

    إذا(result == CURLE_PEER_FAILED_VERIFICATION) {
      failf(data,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    وإلا إذا(result != CURLE_OK)
      failf(data, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  إذا(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  أرجع result;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status;
  CURLcode result = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = عدم;
  ثابت CERT_CHAIN_CONTEXT *pChainContext = عدم;
  HCERTCHAINENGINE cert_chain_engine = عدم;
  HCERTSTORE trust_store = عدم;
  HCERTSTORE own_trust_store = عدم;

  DEBUGASSERT(BACKEND);

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  إذا((sspi_status != SEC_E_OK) || !pCertContextServer) {
    حرف buffer[WINAPI_ERROR_LEN];
    failf(data, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, buffer, حجم(buffer)));
    result = CURLE_PEER_FAILED_VERIFICATION;
  }

  إذا(result == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    إذا(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_LESS_THAN)) {
      failf(data, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      result = CURLE_SSL_CACERT_BADFILE;
    }
    وإلا {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, data);

      إذا(trust_store) {
        infof(data, "schannel: reusing certificate store from cache");
      }
      وإلا {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_MEMORY,
                                    0,
                                    (HCRYPTPROV)عدم,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    عدم);
        إذا(!trust_store) {
          حرف buffer[WINAPI_ERROR_LEN];
          failf(data, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(GetLastError(), buffer, حجم(buffer)));
          result = CURLE_SSL_CACERT_BADFILE;
        }
        وإلا {
          ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          إذا(ca_info_blob) {
            result = add_certs_data_to_store(trust_store,
                                             (ثابت حرف *)ca_info_blob->data,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             data);
          }
          وإلا {
            result = add_certs_file_to_store(trust_store,
                                             conn_config->CAfile,
                                             data);
          }
          إذا(result == CURLE_OK) {
            إذا(Curl_schannel_set_cached_cert_store(cf, data, trust_store)) {
              own_trust_store = عدم;
            }
          }
        }
      }
    }

    إذا(result == CURLE_OK) {
      بنية cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_result;

      memset(&engine_config, 0, حجم(engine_config));
      engine_config.h_Exclusive_جڑ = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      إذا(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cbSize = حجم(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      وإلا
        engine_config.cbSize = حجم(بنية cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_result =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      إذا(!create_engine_result) {
        حرف buffer[WINAPI_ERROR_LEN];
        failf(data,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(GetLastError(), buffer, حجم(buffer)));
        result = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  إذا(result == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, حجم(ChainPara));
    ChainPara.cbSize = حجم(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_AND;
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
      حرف buffer[WINAPI_ERROR_LEN];
      failf(data, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(GetLastError(), buffer, حجم(buffer)));
      pChainContext = عدم;
      result = CURLE_PEER_FAILED_VERIFICATION;
    }

    إذا(result == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dwTrustErrorMask = ~(DWORD)(CERT_TRUST_IS_NOT_TIME_NESTED);
      dwTrustErrorMask &= pSimpleChain->TrustStatus.dwErrorStatus;

      إذا(conn_config->revoke_best_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dwTrustErrorMask &= ~(DWORD)(CERT_TRUST_REVOCATION_STATUS_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      إذا(dwTrustErrorMask) {
        إذا(dwTrustErrorMask & CERT_TRUST_IS_REVOKED)
          failf(data, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        وإلا إذا(dwTrustErrorMask & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(data, "schannel: the certificate chain is incomplete");
        وإلا إذا(dwTrustErrorMask & CERT_TRUST_IS_UNTRUSTED_جڑ)
          failf(data, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        وإلا إذا(dwTrustErrorMask & CERT_TRUST_IS_NOT_TIME_VALID)
          failf(data, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        وإلا إذا(dwTrustErrorMask & CERT_TRUST_REVOCATION_STATUS_UNKNOWN)
          failf(data, "schannel: the revocation status is unknown");
        وإلا
          failf(data, "schannel: error 0x%08lx", dwTrustErrorMask);
        result = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  إذا(result == CURLE_OK) {
    إذا(conn_config->verifyhost) {
      result = Curl_verify_host(cf, data);
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

  أرجع result;
}

#endif /* USE_SCHANNEL */
