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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_SCHANNEL

#ifndef USE_WINDOWS_SSPI
#error "cannot compile Schannel support without SSPI."
#endif

#অন্তর্ভাব "vtls/schannel.h"
#অন্তর্ভাব "vtls/schannel_int.h"

#অন্তর্ভাব "vtls/hostcheck.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strerror.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "curlx/multibyte.h"
#অন্তর্ভাব "curlx/version_win32.h"
#অন্তর্ভাব "curlx/winapi.h"

#define BACKEND ((কাঠামো schannel_ssl_backend_তথ্য *)connssl->backend)

#define MAX_CAFILE_আকার (1024 * 1024) /* 1 MiB */
#define BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#define END_CERT        "\n-----END CERTIFICATE-----"

কাঠামো cert_chain_engine_config_win8 {
  DWORD cb_আকার;
  HCERTSTORE h_Restricted_মূল;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_মূল;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#define CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#endif

/* Legacy structure to supply size to Win7 clients */
কাঠামো cert_chain_engine_config_win7 {
  DWORD cb_আকার;
  HCERTSTORE h_Restricted_মূল;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_মূল;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
স্থির ধ্রুবক অক্ষর *c_memmem(ধ্রুবক শূন্য *haystack, size_t haystacklen,
                            ধ্রুবক শূন্য *needle, size_t needlelen)
{
  ধ্রুবক অক্ষর *p;
  অক্ষর first;
  ধ্রুবক অক্ষর *str_limit = (ধ্রুবক অক্ষর *)haystack + haystacklen;
  যদি(!needlelen || needlelen > haystacklen)
    ফেরত ফাঁকা;
  first = *(ধ্রুবক অক্ষর *)needle;
  জন্য(p = (ধ্রুবক অক্ষর *)haystack; p <= (str_limit - needlelen); p++)
    যদি((*p == first) && !memcmp(p, needle, needlelen))
      ফেরত p;

  ফেরত ফাঁকা;
}

স্থির CURLcode add_certs_তথ্য_to_store(HCERTSTORE trust_store,
                                        ধ্রুবক অক্ষর *ca_বাফার,
                                        size_t ca_বাফার_আকার,
                                        ধ্রুবক অক্ষর *ca_file_text,
                                        কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  ধ্রুবক size_t end_cert_len = CURL_CSTRLEN(END_CERT);
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা অঙ্ক_certs = 0;
  bool more_certs = 1;
  ধ্রুবক অক্ষর *current_ca_file_ptr = ca_বাফার;
  ধ্রুবক অক্ষর *ca_বাফার_limit = ca_বাফার + ca_বাফার_আকার;

  যখন(more_certs && (current_ca_file_ptr < ca_বাফার_limit)) {
    ধ্রুবক অক্ষর *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_বাফার_limit -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    যদি(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      more_certs = 0;
    }
    অন্যথা {
      ধ্রুবক অক্ষর *end_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_বাফার_limit-begin_cert_ptr,
                                          END_CERT,
                                          end_cert_len);
      যদি(!end_cert_ptr) {
        failf(তথ্য,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_text);
        ফলাফল = CURLE_SSL_CACERT_BADFILE;
        more_certs = 0;
      }
      অন্যথা {
        CERT_BLOB cert_blob;
        ধ্রুবক CERT_CONTEXT *cert_context = ফাঁকা;
        BOOL add_cert_ফলাফল = FALSE;
        DWORD cert_আকার =
          (DWORD)((end_cert_ptr + end_cert_len) - begin_cert_ptr);

        cert_blob.pb_তথ্য = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cb_তথ্য = cert_আকার;
        /* Caution: CryptQueryObject() is deprecated */
        যদি(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             ফাঁকা,
                             ফাঁকা,
                             ফাঁকা,
                             ফাঁকা,
                             ফাঁকা,
                             (ধ্রুবক শূন্য **)&cert_context)) {
          অক্ষর বাফার[WINAPI_ERROR_LEN];
          failf(তথ্য,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_text,
                curlx_winapi_strerror(GetLastError(), বাফার, আকার(বাফার)));
          ফলাফল = CURLE_SSL_CACERT_BADFILE;
          more_certs = 0;
        }
        অন্যথা {
          current_ca_file_ptr = begin_cert_ptr + cert_আকার;

          add_cert_ফলাফল =
            CertAddCertificateContextToStore(trust_store,
                                             cert_context,
                                             CERT_STORE_ADD_ALWAYS,
                                             ফাঁকা);
          যদি(!add_cert_ফলাফল) {
            অক্ষর বাফার[WINAPI_ERROR_LEN];
            failf(তথ্য,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_text,
                  curlx_winapi_strerror(GetLastError(), বাফার,
                                        আকার(বাফার)));
            ফলাফল = CURLE_SSL_CACERT_BADFILE;
            more_certs = 0;
          }
          অন্যথা {
            অঙ্ক_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  যদি(ফলাফল == CURLE_OK) {
    যদি(!অঙ্ক_certs) {
      infof(তথ্য, "schannel: did not add any certificates from CA file '%s'",
            ca_file_text);
    }
    অন্যথা {
      infof(তথ্য, "schannel: added %d certificate(s) from CA file '%s'",
            অঙ্ক_certs, ca_file_text);
    }
  }
  ফেরত ফলাফল;
}

স্থির CURLcode add_certs_file_to_store(HCERTSTORE trust_store,
                                        ধ্রুবক অক্ষর *ca_file,
                                        কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;
  FILE *ca_file_handle;
  অক্ষর *ca_file_বাফার = ফাঁকা;
  long ca_file_bufsize = 0;
  long যোগ_bytes_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  যদি(!ca_file_handle) {
    failf(তথ্য, "schannel: failed to open CA file '%s'", ca_file);
    ফলাফল = CURLE_SSL_CACERT_BADFILE;
    যাও cleanup;
  }

  যদি(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(তথ্য, "schannel: failed seeking to end of CA file '%s'", ca_file);
    ফলাফল = CURLE_SSL_CACERT_BADFILE;
    যাও cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  যদি(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(তথ্য, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    ফলাফল = CURLE_SSL_CACERT_BADFILE;
    যাও cleanup;
  }

  যদি(ca_file_bufsize < 0) {
    failf(তথ্য, "schannel: failed to get length of CA file '%s'", ca_file);
    ফলাফল = CURLE_SSL_CACERT_BADFILE;
    যাও cleanup;
  }

  যদি(ca_file_bufsize > MAX_CAFILE_আকার) {
    failf(তথ্য, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_আকার);
    ফলাফল = CURLE_SSL_CACERT_BADFILE;
    যাও cleanup;
  }

  ca_file_বাফার = curlx_malloc(ca_file_bufsize + 1);
  যদি(!ca_file_বাফার) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও cleanup;
  }

  যখন(যোগ_bytes_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_বাফার + যোগ_bytes_read, 1,
                         ca_file_bufsize - যোগ_bytes_read, ca_file_handle);

    যদি(ferror(ca_file_handle)) {
      failf(তথ্য, "schannel: failed to read from CA file '%s'", ca_file);
      ফলাফল = CURLE_SSL_CACERT_BADFILE;
      যাও cleanup;
    }

    যদি(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = যোগ_bytes_read;
    }
    অন্যথা {
      যোগ_bytes_read += (long)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_বাফার[ca_file_bufsize] = '\0';

  ফলাফল = add_certs_তথ্য_to_store(trust_store,
                                   ca_file_বাফার, ca_file_bufsize,
                                   ca_file,
                                   তথ্য);

cleanup:
  যদি(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_বাফার);

  ফেরত ফলাফল;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
স্থির DWORD cert_get_name_string(কাঠামো Curl_easy *তথ্য,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_names,
                                  DWORD দৈর্ঘ্য,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_দৈর্ঘ্য = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = ফাঁকা;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  যদি(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_NAME_অনুসন্ধান_ALL_NAMES_FLAG
#define CERT_NAME_অনুসন্ধান_ALL_NAMES_FLAG 0x2
#endif
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_অনুসন্ধান_ALL_NAMES_FLAG;
    actual_দৈর্ঘ্য = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      ফাঁকা,
                                      host_names,
                                      দৈর্ঘ্য);
    ফেরত actual_দৈর্ঘ্য;
  }

  যদি(!alt_name_info)
    ফেরত 0;

  compute_content = host_names && দৈর্ঘ্য != 0;

  /* Initialize default return values. */
  actual_দৈর্ঘ্য = 1;
  যদি(compute_content) {
    *host_names = '\0';
  }

  current_pos = host_names;

  /* Iterate over the alternate names and populate host_names. */
  জন্য(i = 0; i < alt_name_info->cAltEntry; i++) {
    ধ্রুবক CERT_ALT_NAME_ENTRY *entry = &alt_name_info->rgAltEntry[i];
    wchar_t *dns_w = ফাঁকা;
    size_t current_দৈর্ঘ্য = 0;

    যদি(entry->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      চালু;
    }
    যদি(!entry->pwszDNSName) {
      infof(তথ্য, "schannel: Empty DNS name.");
      চালু;
    }
    current_দৈর্ঘ্য = wcslen(entry->pwszDNSName) + 1;
    যদি(!compute_content) {
      actual_দৈর্ঘ্য += (DWORD)current_দৈর্ঘ্য;
      চালু;
    }
    /* Sanity check to prevent buffer overrun. */
    যদি((actual_দৈর্ঘ্য + current_দৈর্ঘ্য) > দৈর্ঘ্য) {
      failf(তথ্য, "schannel: Not enough memory to list all hostnames.");
      বিরতি;
    }
    dns_w = entry->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    যখন(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_দৈর্ঘ্য += (DWORD)current_দৈর্ঘ্য;
  }
  যদি(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  ফেরত actual_দৈর্ঘ্য;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

স্থির bool get_অঙ্ক_host_info(কাঠামো অঙ্ক_ip_তথ্য *ip_blob, LPCSTR hostname)
{
  কাঠামো in_addr ia;
  পূর্ণসংখ্যা res = curlx_inet_pton(AF_INET, hostname, &ia);
  যদি(res) {
    ip_blob->আকার_2 = আকার(কাঠামো in_addr);
    memcpy(&ip_blob->b_তথ্য.ia, &ia, আকার(কাঠামো in_addr));
    ফেরত TRUE;
  }
  অন্যথা {
    কাঠামো in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    যদি(res) {
      ip_blob->আকার_2 = আকার(কাঠামো in6_addr);
      memcpy(&ip_blob->b_তথ্য.ia6, &ia6, আকার(কাঠামো in6_addr));
      ফেরত TRUE;
    }
  }
  ফেরত FALSE;
}

স্থির bool get_alt_name_info(কাঠামো Curl_easy *তথ্য,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_আকার)
{
  PCERT_INFO cert_info = ফাঁকা;
  PCERT_EXTENSION extension = ফাঁকা;
  CRYPT_DECODE_PARA decode_para = { আকার(CRYPT_DECODE_PARA), ফাঁকা, ফাঁকা };

  যদি(!ctx) {
    failf(তথ্য, "schannel: Null certificate context.");
    ফেরত FALSE;
  }

  cert_info = ctx->pCertInfo;
  যদি(!cert_info) {
    failf(তথ্য, "schannel: Null certificate info.");
    ফেরত FALSE;
  }

  extension = CertFindExtension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  যদি(!extension) {
    failf(তথ্য, "schannel: CertFindExtension() returned no extension.");
    ফেরত FALSE;
  }

  যদি(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->মান_2.pb_তথ্য,
                          extension->মান_2.cb_তথ্য,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_আকার)) {
    failf(তথ্য, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    ফেরত FALSE;
  }
  ফেরত TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = ফাঁকা;
  SECURITY_STATUS sspi_status;
  TCHAR *cert_hostname_buff = ফাঁকা;
  size_t cert_hostname_buff_সূচক = 0;
  ধ্রুবক অক্ষর *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = strlen(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = ফাঁকা;
  DWORD alt_name_info_আকার = 0;
  কাঠামো অঙ্ক_ip_তথ্য ip_blob = { 0 };
  bool Win8_compat;
  কাঠামো অঙ্ক_ip_তথ্য *p = &ip_blob;
  DWORD i;

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  যদি((sspi_status != SEC_E_OK) || !pCertContextServer) {
    অক্ষর বাফার[WINAPI_ERROR_LEN];
    failf(তথ্য, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
    যাও cleanup;
  }

  Win8_compat = curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                             VERSION_GREATER_THAN_EQUAL);
  যদি(get_অঙ্ক_host_info(p, conn_hostname) || !Win8_compat) {
    যদি(!get_alt_name_info(তথ্য, pCertContextServer,
                          &alt_name_info, &alt_name_info_আকার)) {
      যাও cleanup;
    }
  }

  যদি(p->আকার_2 && alt_name_info) {
    জন্য(i = 0; i < alt_name_info->cAltEntry; ++i) {
      PCERT_ALT_NAME_ENTRY entry = &alt_name_info->rgAltEntry[i];
      যদি(entry->dwAltNameChoice == CERT_ALT_NAME_IP_ADDRESS) {
        যদি(entry->IPAddress.cb_তথ্য == p->আকার_2) {
          যদি(!memcmp(entry->IPAddress.pb_তথ্য, &p->b_তথ্য,
                     entry->IPAddress.cb_তথ্য)) {
            ফলাফল = CURLE_OK;
            infof(তথ্য,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            বিরতি;
          }
        }
      }
    }
  }
  অন্যথা {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(তথ্য, pCertContextServer,
                               ফাঁকা, 0, alt_name_info, Win8_compat);
    যদি(len == 0) {
      failf(তথ্য,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      যাও cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * আকার(TCHAR));
    যদি(!cert_hostname_buff) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও cleanup;
    }
    actual_len = cert_get_name_string(তথ্য, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    যদি(actual_len != len) {
      failf(তথ্য,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      যাও cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    যখন(cert_hostname_buff_সূচক < len &&
          cert_hostname_buff[cert_hostname_buff_সূচক] != TEXT('\0') &&
          ফলাফল == CURLE_PEER_FAILED_VERIFICATION) {
      অক্ষর *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_to_UTF8(
        &cert_hostname_buff[cert_hostname_buff_সূচক]);
      যদি(!cert_hostname) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
      }
      অন্যথা {
        যদি(Curl_cert_hostcheck(cert_hostname, strlen(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(তথ্য, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          ফলাফল = CURLE_OK;
        }
        অন্যথা {
          size_t cert_hostname_len;

          infof(তথ্য,
                "schannel: connection hostname (%s) did not match "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);

          cert_hostname_len =
            _tcslen(&cert_hostname_buff[cert_hostname_buff_সূচক]);

          /* Move on to next cert name */
          cert_hostname_buff_সূচক += cert_hostname_len + 1;

          ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
        }
        curlx_free(cert_hostname);
      }
    }

    যদি(ফলাফল == CURLE_PEER_FAILED_VERIFICATION) {
      failf(তথ্য,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    অন্যথা যদি(ফলাফল != CURLE_OK)
      failf(তথ্য, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  যদি(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ফেরত ফলাফল;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status;
  CURLcode ফলাফল = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = ফাঁকা;
  ধ্রুবক CERT_CHAIN_CONTEXT *pChainContext = ফাঁকা;
  HCERTCHAINENGINE cert_chain_engine = ফাঁকা;
  HCERTSTORE trust_store = ফাঁকা;
  HCERTSTORE own_trust_store = ফাঁকা;

  DEBUGASSERT(BACKEND);

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  যদি((sspi_status != SEC_E_OK) || !pCertContextServer) {
    অক্ষর বাফার[WINAPI_ERROR_LEN];
    failf(তথ্য, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, বাফার, আকার(বাফার)));
    ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
  }

  যদি(ফলাফল == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    যদি(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_LESS_THAN)) {
      failf(তথ্য, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      ফলাফল = CURLE_SSL_CACERT_BADFILE;
    }
    অন্যথা {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, তথ্য);

      যদি(trust_store) {
        infof(তথ্য, "schannel: reusing certificate store from cache");
      }
      অন্যথা {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_MEMORY,
                                    0,
                                    (HCRYPTPROV)ফাঁকা,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    ফাঁকা);
        যদি(!trust_store) {
          অক্ষর বাফার[WINAPI_ERROR_LEN];
          failf(তথ্য, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(GetLastError(), বাফার, আকার(বাফার)));
          ফলাফল = CURLE_SSL_CACERT_BADFILE;
        }
        অন্যথা {
          ধ্রুবক কাঠামো curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          যদি(ca_info_blob) {
            ফলাফল = add_certs_তথ্য_to_store(trust_store,
                                             (ধ্রুবক অক্ষর *)ca_info_blob->তথ্য,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             তথ্য);
          }
          অন্যথা {
            ফলাফল = add_certs_file_to_store(trust_store,
                                             conn_config->CAfile,
                                             তথ্য);
          }
          যদি(ফলাফল == CURLE_OK) {
            যদি(Curl_schannel_set_cached_cert_store(cf, তথ্য, trust_store)) {
              own_trust_store = ফাঁকা;
            }
          }
        }
      }
    }

    যদি(ফলাফল == CURLE_OK) {
      কাঠামো cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_ফলাফল;

      memset(&engine_config, 0, আকার(engine_config));
      engine_config.h_Exclusive_মূল = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      যদি(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cb_আকার = আকার(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      অন্যথা
        engine_config.cb_আকার = আকার(কাঠামো cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_ফলাফল =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      যদি(!create_engine_ফলাফল) {
        অক্ষর বাফার[WINAPI_ERROR_LEN];
        failf(তথ্য,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(GetLastError(), বাফার, আকার(বাফার)));
        ফলাফল = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  যদি(ফলাফল == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, আকার(ChainPara));
    ChainPara.cb_আকার = আকার(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    যদি(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                ফাঁকা,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                ফাঁকা,
                                &pChainContext)) {
      অক্ষর বাফার[WINAPI_ERROR_LEN];
      failf(তথ্য, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(GetLastError(), বাফার, আকার(বাফার)));
      pChainContext = ফাঁকা;
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    }

    যদি(ফলাফল == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dwTrustErrorMask = ~(DWORD)(CERT_TRUST_IS_NOT_TIME_NESTED);
      dwTrustErrorMask &= pSimpleChain->TrustStatus.dwErrorStatus;

      যদি(conn_config->revoke_best_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dwTrustErrorMask &= ~(DWORD)(CERT_TRUST_REVOCATION_STATUS_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      যদি(dwTrustErrorMask) {
        যদি(dwTrustErrorMask & CERT_TRUST_IS_REVOKED)
          failf(তথ্য, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        অন্যথা যদি(dwTrustErrorMask & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(তথ্য, "schannel: the certificate chain is incomplete");
        অন্যথা যদি(dwTrustErrorMask & CERT_TRUST_IS_UNTRUSTED_মূল)
          failf(তথ্য, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        অন্যথা যদি(dwTrustErrorMask & CERT_TRUST_IS_NOT_TIME_VALID)
          failf(তথ্য, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        অন্যথা যদি(dwTrustErrorMask & CERT_TRUST_REVOCATION_STATUS_UNKNOWN)
          failf(তথ্য, "schannel: the revocation status is unknown");
        অন্যথা
          failf(তথ্য, "schannel: error 0x%08lx", dwTrustErrorMask);
        ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  যদি(ফলাফল == CURLE_OK) {
    যদি(conn_config->verifyhost) {
      ফলাফল = Curl_verify_host(cf, তথ্য);
    }
  }

  যদি(cert_chain_engine) {
    CertFreeCertificateChainEngine(cert_chain_engine);
  }

  যদি(own_trust_store) {
    CertCloseStore(own_trust_store, 0);
  }

  যদি(pChainContext)
    CertFreeCertificateChain(pChainContext);

  যদি(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ফেরত ফলাফল;
}

#endif /* USE_SCHANNEL */
