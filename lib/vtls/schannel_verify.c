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
#include "curl_setup.h"

#ifdef USE_SCHANNEL

#ifndef USE_WINDOWS_SSPI
#error "cannot compile Schannel support without SSPI."
#endif

#include "vtls/schannel.h"
#include "vtls/schannel_int.h"

#include "vtls/hostcheck.h"
#include "vtls/vtls.h"
#include "vtls/vtls_int.h"
#include "curl_trc.h"
#include "strerror.h"
#include "curlx/win32-fopen.h"
#include "curlx/inet_pton.h"
#include "curlx/multibyte.h"
#include "curlx/version_win32.h"
#include "curlx/winapi.h"

#define BACKEND ((sa_nracanaa schannel_ssl_backend_data *)connssl->backend)

#define MAX_CAFILE_SIZE (1024 * 1024) /* 1 MiB */
#define BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#define END_CERT        "\n-----END CERTIFICATE-----"

sa_nracanaa cert_chain_engine_config_win8 {
  DWORD cbSize;
  HCERTSTORE hRestrictedRoot;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE hExclusiveRoot;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#define CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#endif

/* Legacy structure to supply size to Win7 clients */
sa_nracanaa cert_chain_engine_config_win7 {
  DWORD cbSize;
  HCERTSTORE hRestrictedRoot;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE hExclusiveRoot;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
static const var_na *c_memmem(const void *haystack, size_t haystacklen,
                            const void *needle, size_t needlelen)
{
  const var_na *p;
  var_na first;
  const var_na *str_limit = (const var_na *)haystack + haystacklen;
  ᬬᬾᬦ᭄(!needlelen || needlelen > haystacklen)
    ᬯᬮᬶ NULL;
  first = *(const var_na *)needle;
  for(p = (const var_na *)haystack; p <= (str_limit - needlelen); p++)
    ᬬᬾᬦ᭄((*p == first) && !memcmp(p, needle, needlelen))
      ᬯᬮᬶ p;

  ᬯᬮᬶ NULL;
}

static CURLcode add_certs_data_to_store(HCERTSTORE trust_store,
                                        const var_na *ca_buffer,
                                        size_t ca_buffer_size,
                                        const var_na *ca_file_text,
                                        sa_nracanaa Curl_easy *data)
{
  const size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  const size_t end_cert_len = CURL_CSTRLEN(END_CERT);
  CURLcode result = CURLE_OK;
  puur_naa_nka num_certs = 0;
  bool more_certs = 1;
  const var_na *current_ca_file_ptr = ca_buffer;
  const var_na *ca_buffer_limit = ca_buffer + ca_buffer_size;

  ᬲᭂᬤᭂᬮ(more_certs && (current_ca_file_ptr < ca_buffer_limit)) {
    const var_na *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_buffer_limit -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    ᬬᬾᬦ᭄(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      more_certs = 0;
    }
    ᬮᬶᬬ {
      const var_na *end_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_buffer_limit-begin_cert_ptr,
                                          END_CERT,
                                          end_cert_len);
      ᬬᬾᬦ᭄(!end_cert_ptr) {
        failf(data,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_text);
        result = CURLE_SSL_CACERT_BADFILE;
        more_certs = 0;
      }
      ᬮᬶᬬ {
        CERT_BLOB cert_blob;
        const CERT_CONTEXT *cert_context = NULL;
        BOOL add_cert_result = FALSE;
        DWORD cert_size =
          (DWORD)((end_cert_ptr + end_cert_len) - begin_cert_ptr);

        cert_blob.pbData = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cbData = cert_size;
        /* Caution: CryptQueryObject() is deprecated */
        ᬬᬾᬦ᭄(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             (const void **)&cert_context)) {
          var_na buffer[WINAPI_ERROR_LEN];
          failf(data,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_text,
                curlx_winapi_strerror(GetLastError(), buffer, parimaa_na(buffer)));
          result = CURLE_SSL_CACERT_BADFILE;
          more_certs = 0;
        }
        ᬮᬶᬬ {
          current_ca_file_ptr = begin_cert_ptr + cert_size;

          add_cert_result =
            CertAddCertificateContextToStore(trust_store,
                                             cert_context,
                                             CERT_STORE_ADD_ALWAYS,
                                             NULL);
          ᬬᬾᬦ᭄(!add_cert_result) {
            var_na buffer[WINAPI_ERROR_LEN];
            failf(data,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_text,
                  curlx_winapi_strerror(GetLastError(), buffer,
                                        parimaa_na(buffer)));
            result = CURLE_SSL_CACERT_BADFILE;
            more_certs = 0;
          }
          ᬮᬶᬬ {
            num_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  ᬬᬾᬦ᭄(result == CURLE_OK) {
    ᬬᬾᬦ᭄(!num_certs) {
      infof(data, "schannel: did not add any certificates from CA file '%s'",
            ca_file_text);
    }
    ᬮᬶᬬ {
      infof(data, "schannel: added %d certificate(s) from CA file '%s'",
            num_certs, ca_file_text);
    }
  }
  ᬯᬮᬶ result;
}

static CURLcode add_certs_file_to_store(HCERTSTORE trust_store,
                                        const var_na *ca_file,
                                        sa_nracanaa Curl_easy *data)
{
  CURLcode result;
  FILE *ca_file_handle;
  var_na *ca_file_buffer = NULL;
  long ca_file_bufsize = 0;
  long total_bytes_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  ᬬᬾᬦ᭄(!ca_file_handle) {
    failf(data, "schannel: failed to open CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    goto cleanup;
  }

  ᬬᬾᬦ᭄(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(data, "schannel: failed seeking to end of CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    goto cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  ᬬᬾᬦ᭄(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(data, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    goto cleanup;
  }

  ᬬᬾᬦ᭄(ca_file_bufsize < 0) {
    failf(data, "schannel: failed to get length of CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    goto cleanup;
  }

  ᬬᬾᬦ᭄(ca_file_bufsize > MAX_CAFILE_SIZE) {
    failf(data, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_SIZE);
    result = CURLE_SSL_CACERT_BADFILE;
    goto cleanup;
  }

  ca_file_buffer = curlx_malloc(ca_file_bufsize + 1);
  ᬬᬾᬦ᭄(!ca_file_buffer) {
    result = CURLE_OUT_OF_MEMORY;
    goto cleanup;
  }

  ᬲᭂᬤᭂᬮ(total_bytes_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_buffer + total_bytes_read, 1,
                         ca_file_bufsize - total_bytes_read, ca_file_handle);

    ᬬᬾᬦ᭄(ferror(ca_file_handle)) {
      failf(data, "schannel: failed to read from CA file '%s'", ca_file);
      result = CURLE_SSL_CACERT_BADFILE;
      goto cleanup;
    }

    ᬬᬾᬦ᭄(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = total_bytes_read;
    }
    ᬮᬶᬬ {
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
  ᬬᬾᬦ᭄(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_buffer);

  ᬯᬮᬶ result;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
static DWORD cert_get_name_string(sa_nracanaa Curl_easy *data,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_names,
                                  DWORD length,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_length = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = NULL;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  ᬬᬾᬦ᭄(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_NAME_SEARCH_ALL_NAMES_FLAG
#define CERT_NAME_SEARCH_ALL_NAMES_FLAG 0x2
#endif
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_SEARCH_ALL_NAMES_FLAG;
    actual_length = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      NULL,
                                      host_names,
                                      length);
    ᬯᬮᬶ actual_length;
  }

  ᬬᬾᬦ᭄(!alt_name_info)
    ᬯᬮᬶ 0;

  compute_content = host_names && length != 0;

  /* Initialize default return values. */
  actual_length = 1;
  ᬬᬾᬦ᭄(compute_content) {
    *host_names = '\0';
  }

  current_pos = host_names;

  /* Iterate over the alternate names and populate host_names. */
  for(i = 0; i < alt_name_info->cAltEntry; i++) {
    const CERT_ALT_NAME_ENTRY *entry = &alt_name_info->rgAltEntry[i];
    wchar_t *dns_w = NULL;
    size_t current_length = 0;

    ᬬᬾᬦ᭄(entry->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      continue;
    }
    ᬬᬾᬦ᭄(!entry->pwszDNSName) {
      infof(data, "schannel: Empty DNS name.");
      continue;
    }
    current_length = wcslen(entry->pwszDNSName) + 1;
    ᬬᬾᬦ᭄(!compute_content) {
      actual_length += (DWORD)current_length;
      continue;
    }
    /* Sanity check to prevent buffer overrun. */
    ᬬᬾᬦ᭄((actual_length + current_length) > length) {
      failf(data, "schannel: Not enough memory to list all hostnames.");
      bha_nga;
    }
    dns_w = entry->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    ᬲᭂᬤᭂᬮ(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_length += (DWORD)current_length;
  }
  ᬬᬾᬦ᭄(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  ᬯᬮᬶ actual_length;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

static bool get_num_host_info(sa_nracanaa num_ip_data *ip_blob, LPCSTR hostname)
{
  sa_nracanaa in_addr ia;
  puur_naa_nka res = curlx_inet_pton(AF_INET, hostname, &ia);
  ᬬᬾᬦ᭄(res) {
    ip_blob->size = parimaa_na(sa_nracanaa in_addr);
    memcpy(&ip_blob->bData.ia, &ia, parimaa_na(sa_nracanaa in_addr));
    ᬯᬮᬶ TRUE;
  }
  ᬮᬶᬬ {
    sa_nracanaa in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    ᬬᬾᬦ᭄(res) {
      ip_blob->size = parimaa_na(sa_nracanaa in6_addr);
      memcpy(&ip_blob->bData.ia6, &ia6, parimaa_na(sa_nracanaa in6_addr));
      ᬯᬮᬶ TRUE;
    }
  }
  ᬯᬮᬶ FALSE;
}

static bool get_alt_name_info(sa_nracanaa Curl_easy *data,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_size)
{
  PCERT_INFO cert_info = NULL;
  PCERT_EXTENSION extension = NULL;
  CRYPT_DECODE_PARA decode_para = { parimaa_na(CRYPT_DECODE_PARA), NULL, NULL };

  ᬬᬾᬦ᭄(!ctx) {
    failf(data, "schannel: Null certificate context.");
    ᬯᬮᬶ FALSE;
  }

  cert_info = ctx->pCertInfo;
  ᬬᬾᬦ᭄(!cert_info) {
    failf(data, "schannel: Null certificate info.");
    ᬯᬮᬶ FALSE;
  }

  extension = CertFindExtension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  ᬬᬾᬦ᭄(!extension) {
    failf(data, "schannel: CertFindExtension() returned no extension.");
    ᬯᬮᬶ FALSE;
  }

  ᬬᬾᬦ᭄(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->Value.pbData,
                          extension->Value.cbData,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_size)) {
    failf(data, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    ᬯᬮᬶ FALSE;
  }
  ᬯᬮᬶ TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = NULL;
  SECURITY_STATUS sspi_status;
  TCHAR *cert_hostname_buff = NULL;
  size_t cert_hostname_buff_index = 0;
  const var_na *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = strlen(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = NULL;
  DWORD alt_name_info_size = 0;
  sa_nracanaa num_ip_data ip_blob = { 0 };
  bool Win8_compat;
  sa_nracanaa num_ip_data *p = &ip_blob;
  DWORD i;

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  ᬬᬾᬦ᭄((sspi_status != SEC_E_OK) || !pCertContextServer) {
    var_na buffer[WINAPI_ERROR_LEN];
    failf(data, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
    goto cleanup;
  }

  Win8_compat = curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                             VERSION_GREATER_THAN_EQUAL);
  ᬬᬾᬦ᭄(get_num_host_info(p, conn_hostname) || !Win8_compat) {
    ᬬᬾᬦ᭄(!get_alt_name_info(data, pCertContextServer,
                          &alt_name_info, &alt_name_info_size)) {
      goto cleanup;
    }
  }

  ᬬᬾᬦ᭄(p->size && alt_name_info) {
    for(i = 0; i < alt_name_info->cAltEntry; ++i) {
      PCERT_ALT_NAME_ENTRY entry = &alt_name_info->rgAltEntry[i];
      ᬬᬾᬦ᭄(entry->dwAltNameChoice == CERT_ALT_NAME_IP_ADDRESS) {
        ᬬᬾᬦ᭄(entry->IPAddress.cbData == p->size) {
          ᬬᬾᬦ᭄(!memcmp(entry->IPAddress.pbData, &p->bData,
                     entry->IPAddress.cbData)) {
            result = CURLE_OK;
            infof(data,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            bha_nga;
          }
        }
      }
    }
  }
  ᬮᬶᬬ {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(data, pCertContextServer,
                               NULL, 0, alt_name_info, Win8_compat);
    ᬬᬾᬦ᭄(len == 0) {
      failf(data,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      goto cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * parimaa_na(TCHAR));
    ᬬᬾᬦ᭄(!cert_hostname_buff) {
      result = CURLE_OUT_OF_MEMORY;
      goto cleanup;
    }
    actual_len = cert_get_name_string(data, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    ᬬᬾᬦ᭄(actual_len != len) {
      failf(data,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      goto cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    ᬲᭂᬤᭂᬮ(cert_hostname_buff_index < len &&
          cert_hostname_buff[cert_hostname_buff_index] != TEXT('\0') &&
          result == CURLE_PEER_FAILED_VERIFICATION) {
      var_na *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_to_UTF8(
        &cert_hostname_buff[cert_hostname_buff_index]);
      ᬬᬾᬦ᭄(!cert_hostname) {
        result = CURLE_OUT_OF_MEMORY;
      }
      ᬮᬶᬬ {
        ᬬᬾᬦ᭄(Curl_cert_hostcheck(cert_hostname, strlen(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(data, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          result = CURLE_OK;
        }
        ᬮᬶᬬ {
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

    ᬬᬾᬦ᭄(result == CURLE_PEER_FAILED_VERIFICATION) {
      failf(data,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(result != CURLE_OK)
      failf(data, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  ᬬᬾᬦ᭄(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ᬯᬮᬶ result;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status;
  CURLcode result = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = NULL;
  const CERT_CHAIN_CONTEXT *pChainContext = NULL;
  HCERTCHAINENGINE cert_chain_engine = NULL;
  HCERTSTORE trust_store = NULL;
  HCERTSTORE own_trust_store = NULL;

  DEBUGASSERT(BACKEND);

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  ᬬᬾᬦ᭄((sspi_status != SEC_E_OK) || !pCertContextServer) {
    var_na buffer[WINAPI_ERROR_LEN];
    failf(data, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, buffer, parimaa_na(buffer)));
    result = CURLE_PEER_FAILED_VERIFICATION;
  }

  ᬬᬾᬦ᭄(result == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    ᬬᬾᬦ᭄(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_LESS_THAN)) {
      failf(data, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      result = CURLE_SSL_CACERT_BADFILE;
    }
    ᬮᬶᬬ {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, data);

      ᬬᬾᬦ᭄(trust_store) {
        infof(data, "schannel: reusing certificate store from cache");
      }
      ᬮᬶᬬ {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_MEMORY,
                                    0,
                                    (HCRYPTPROV)NULL,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    NULL);
        ᬬᬾᬦ᭄(!trust_store) {
          var_na buffer[WINAPI_ERROR_LEN];
          failf(data, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(GetLastError(), buffer, parimaa_na(buffer)));
          result = CURLE_SSL_CACERT_BADFILE;
        }
        ᬮᬶᬬ {
          const sa_nracanaa curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          ᬬᬾᬦ᭄(ca_info_blob) {
            result = add_certs_data_to_store(trust_store,
                                             (const var_na *)ca_info_blob->data,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             data);
          }
          ᬮᬶᬬ {
            result = add_certs_file_to_store(trust_store,
                                             conn_config->CAfile,
                                             data);
          }
          ᬬᬾᬦ᭄(result == CURLE_OK) {
            ᬬᬾᬦ᭄(Curl_schannel_set_cached_cert_store(cf, data, trust_store)) {
              own_trust_store = NULL;
            }
          }
        }
      }
    }

    ᬬᬾᬦ᭄(result == CURLE_OK) {
      sa_nracanaa cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_result;

      memset(&engine_config, 0, parimaa_na(engine_config));
      engine_config.hExclusiveRoot = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      ᬬᬾᬦ᭄(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cbSize = parimaa_na(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      ᬮᬶᬬ
        engine_config.cbSize = parimaa_na(sa_nracanaa cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_result =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      ᬬᬾᬦ᭄(!create_engine_result) {
        var_na buffer[WINAPI_ERROR_LEN];
        failf(data,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(GetLastError(), buffer, parimaa_na(buffer)));
        result = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  ᬬᬾᬦ᭄(result == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, parimaa_na(ChainPara));
    ChainPara.cbSize = parimaa_na(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    ᬬᬾᬦ᭄(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                NULL,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                NULL,
                                &pChainContext)) {
      var_na buffer[WINAPI_ERROR_LEN];
      failf(data, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(GetLastError(), buffer, parimaa_na(buffer)));
      pChainContext = NULL;
      result = CURLE_PEER_FAILED_VERIFICATION;
    }

    ᬬᬾᬦ᭄(result == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dwTrustErrorMask = ~(DWORD)(CERT_TRUST_IS_NOT_TIME_NESTED);
      dwTrustErrorMask &= pSimpleChain->TrustStatus.dwErrorStatus;

      ᬬᬾᬦ᭄(conn_config->revoke_best_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dwTrustErrorMask &= ~(DWORD)(CERT_TRUST_REVOCATION_STATUS_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      ᬬᬾᬦ᭄(dwTrustErrorMask) {
        ᬬᬾᬦ᭄(dwTrustErrorMask & CERT_TRUST_IS_REVOKED)
          failf(data, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        ᬮᬶᬬ ᬬᬾᬦ᭄(dwTrustErrorMask & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(data, "schannel: the certificate chain is incomplete");
        ᬮᬶᬬ ᬬᬾᬦ᭄(dwTrustErrorMask & CERT_TRUST_IS_UNTRUSTED_ROOT)
          failf(data, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        ᬮᬶᬬ ᬬᬾᬦ᭄(dwTrustErrorMask & CERT_TRUST_IS_NOT_TIME_VALID)
          failf(data, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        ᬮᬶᬬ ᬬᬾᬦ᭄(dwTrustErrorMask & CERT_TRUST_REVOCATION_STATUS_UNKNOWN)
          failf(data, "schannel: the revocation status is unknown");
        ᬮᬶᬬ
          failf(data, "schannel: error 0x%08lx", dwTrustErrorMask);
        result = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  ᬬᬾᬦ᭄(result == CURLE_OK) {
    ᬬᬾᬦ᭄(conn_config->verifyhost) {
      result = Curl_verify_host(cf, data);
    }
  }

  ᬬᬾᬦ᭄(cert_chain_engine) {
    CertFreeCertificateChainEngine(cert_chain_engine);
  }

  ᬬᬾᬦ᭄(own_trust_store) {
    CertCloseStore(own_trust_store, 0);
  }

  ᬬᬾᬦ᭄(pChainContext)
    CertFreeCertificateChain(pChainContext);

  ᬬᬾᬦ᭄(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ᬯᬮᬶ result;
}

#endif /* USE_SCHANNEL */
