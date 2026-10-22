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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_SCHANNEL

#ifndef USE_WINDOWS_SSPI
#error "cannot compile Schannel support without SSPI."
#endif

#अन्तर्भाव "vtls/schannel.h"
#अन्तर्भाव "vtls/schannel_int.h"

#अन्तर्भाव "vtls/hostcheck.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_int.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strerror.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "curlx/multibyte.h"
#अन्तर्भाव "curlx/version_win32.h"
#अन्तर्भाव "curlx/winapi.h"

#define BACKEND ((संरचना schannel_ssl_backend_data *)connssl->backend)

#define MAX_CAFILE_SIZE (1024 * 1024) /* 1 MiB */
#define BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#define END_CERT        "\n-----END CERTIFICATE-----"

संरचना cert_chain_engine_config_win8 {
  DWORD cbSize;
  HCERTSTORE h_Restricted_जरा;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_जरा;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#define CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#endif

/* Legacy structure to supply size to Win7 clients */
संरचना cert_chain_engine_config_win7 {
  DWORD cbSize;
  HCERTSTORE h_Restricted_जरा;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_जरा;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
स्थिर अचर वर्ण *c_memmem(अचर शून्य *haystack, माप_प्रकार haystacklen,
                            अचर शून्य *needle, माप_प्रकार needlelen)
{
  अचर वर्ण *p;
  वर्ण first;
  अचर वर्ण *str_limit = (अचर वर्ण *)haystack + haystacklen;
  यदि(!needlelen || needlelen > haystacklen)
    फर्काऊ रिक्त;
  first = *(अचर वर्ण *)needle;
  लागि(p = (अचर वर्ण *)haystack; p <= (str_limit - needlelen); p++)
    यदि((*p == first) && !स_भेद(p, needle, needlelen))
      फर्काऊ p;

  फर्काऊ रिक्त;
}

स्थिर CURLcode add_certs_data_to_store(HCERTSTORE trust_store,
                                        अचर वर्ण *ca_buffer,
                                        माप_प्रकार ca_buffer_size,
                                        अचर वर्ण *ca_file_text,
                                        संरचना Curl_easy *data)
{
  अचर माप_प्रकार begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  अचर माप_प्रकार end_cert_len = CURL_CSTRLEN(END_CERT);
  CURLcode result = CURLE_OK;
  पूर्णाङ्क अङ्क_certs = 0;
  bool more_certs = 1;
  अचर वर्ण *current_ca_file_ptr = ca_buffer;
  अचर वर्ण *ca_buffer_limit = ca_buffer + ca_buffer_size;

  जबसम्म(more_certs && (current_ca_file_ptr < ca_buffer_limit)) {
    अचर वर्ण *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_buffer_limit -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    यदि(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      more_certs = 0;
    }
    अन्यथा {
      अचर वर्ण *end_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_buffer_limit-begin_cert_ptr,
                                          END_CERT,
                                          end_cert_len);
      यदि(!end_cert_ptr) {
        failf(data,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_text);
        result = CURLE_SSL_CACERT_BADFILE;
        more_certs = 0;
      }
      अन्यथा {
        CERT_BLOB cert_blob;
        अचर CERT_CONTEXT *cert_context = रिक्त;
        BOOL add_cert_result = FALSE;
        DWORD cert_size =
          (DWORD)((end_cert_ptr + end_cert_len) - begin_cert_ptr);

        cert_blob.pbData = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cbData = cert_size;
        /* Caution: CryptQueryObject() is deprecated */
        यदि(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             रिक्त,
                             रिक्त,
                             रिक्त,
                             रिक्त,
                             रिक्त,
                             (अचर शून्य **)&cert_context)) {
          वर्ण buffer[WINAPI_ERROR_LEN];
          failf(data,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_text,
                curlx_winapi_strerror(GetLastError(), buffer, परिमाण(buffer)));
          result = CURLE_SSL_CACERT_BADFILE;
          more_certs = 0;
        }
        अन्यथा {
          current_ca_file_ptr = begin_cert_ptr + cert_size;

          add_cert_result =
            CertAddCertificateContextToStore(trust_store,
                                             cert_context,
                                             CERT_STORE_ADD_ALWAYS,
                                             रिक्त);
          यदि(!add_cert_result) {
            वर्ण buffer[WINAPI_ERROR_LEN];
            failf(data,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_text,
                  curlx_winapi_strerror(GetLastError(), buffer,
                                        परिमाण(buffer)));
            result = CURLE_SSL_CACERT_BADFILE;
            more_certs = 0;
          }
          अन्यथा {
            अङ्क_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  यदि(result == CURLE_OK) {
    यदि(!अङ्क_certs) {
      infof(data, "schannel: did not add any certificates from CA file '%s'",
            ca_file_text);
    }
    अन्यथा {
      infof(data, "schannel: added %d certificate(s) from CA file '%s'",
            अङ्क_certs, ca_file_text);
    }
  }
  फर्काऊ result;
}

स्थिर CURLcode add_certs_file_to_store(HCERTSTORE trust_store,
                                        अचर वर्ण *ca_file,
                                        संरचना Curl_easy *data)
{
  CURLcode result;
  खाता *ca_file_handle;
  वर्ण *ca_file_buffer = रिक्त;
  दीर्घ ca_file_bufsize = 0;
  दीर्घ total_bytes_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  यदि(!ca_file_handle) {
    failf(data, "schannel: failed to open CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  यदि(curlx_fseek(ca_file_handle, 0, अंत_से)) {
    failf(data, "schannel: failed seeking to end of CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  ca_file_bufsize = ख_बताओ(ca_file_handle);

  यदि(curlx_fseek(ca_file_handle, 0, शुरू_से)) {
    failf(data, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  यदि(ca_file_bufsize < 0) {
    failf(data, "schannel: failed to get length of CA file '%s'", ca_file);
    result = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  यदि(ca_file_bufsize > MAX_CAFILE_SIZE) {
    failf(data, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_SIZE);
    result = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  ca_file_buffer = curlx_malloc(ca_file_bufsize + 1);
  यदि(!ca_file_buffer) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ cleanup;
  }

  जबसम्म(total_bytes_read < ca_file_bufsize) {
    माप_प्रकार nread = ख_पढ़ो(ca_file_buffer + total_bytes_read, 1,
                         ca_file_bufsize - total_bytes_read, ca_file_handle);

    यदि(ख_त्रुटि(ca_file_handle)) {
      failf(data, "schannel: failed to read from CA file '%s'", ca_file);
      result = CURLE_SSL_CACERT_BADFILE;
      गच्छ cleanup;
    }

    यदि(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = total_bytes_read;
    }
    अन्यथा {
      total_bytes_read += (दीर्घ)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_buffer[ca_file_bufsize] = '\0';

  result = add_certs_data_to_store(trust_store,
                                   ca_file_buffer, ca_file_bufsize,
                                   ca_file,
                                   data);

cleanup:
  यदि(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_buffer);

  फर्काऊ result;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
स्थिर DWORD cert_get_name_string(संरचना Curl_easy *data,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_names,
                                  DWORD length,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_length = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = रिक्त;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  यदि(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ifndef CERT_NAME_खोज_ALL_NAMES_FLAG
#define CERT_NAME_खोज_ALL_NAMES_FLAG 0x2
#endif
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_खोज_ALL_NAMES_FLAG;
    actual_length = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      रिक्त,
                                      host_names,
                                      length);
    फर्काऊ actual_length;
  }

  यदि(!alt_name_info)
    फर्काऊ 0;

  compute_content = host_names && length != 0;

  /* Initialize default return values. */
  actual_length = 1;
  यदि(compute_content) {
    *host_names = '\0';
  }

  current_pos = host_names;

  /* Iterate over the alternate names and populate host_names. */
  लागि(i = 0; i < alt_name_info->cAltEntry; i++) {
    अचर CERT_ALT_NAME_ENTRY *entry = &alt_name_info->rgAltEntry[i];
    ब_अक्षर_प्रकार *dns_w = रिक्त;
    माप_प्रकार current_length = 0;

    यदि(entry->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      जारी;
    }
    यदि(!entry->pwszDNSName) {
      infof(data, "schannel: Empty DNS name.");
      जारी;
    }
    current_length = wcslen(entry->pwszDNSName) + 1;
    यदि(!compute_content) {
      actual_length += (DWORD)current_length;
      जारी;
    }
    /* Sanity check to prevent buffer overrun. */
    यदि((actual_length + current_length) > length) {
      failf(data, "schannel: Not enough memory to list all hostnames.");
      रोक;
    }
    dns_w = entry->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    जबसम्म(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_length += (DWORD)current_length;
  }
  यदि(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  फर्काऊ actual_length;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

स्थिर bool get_अङ्क_host_info(संरचना अङ्क_ip_data *ip_blob, LPCSTR hostname)
{
  संरचना in_addr ia;
  पूर्णाङ्क res = curlx_inet_pton(AF_INET, hostname, &ia);
  यदि(res) {
    ip_blob->size = परिमाण(संरचना in_addr);
    स_नकल(&ip_blob->bData.ia, &ia, परिमाण(संरचना in_addr));
    फर्काऊ TRUE;
  }
  अन्यथा {
    संरचना in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    यदि(res) {
      ip_blob->size = परिमाण(संरचना in6_addr);
      स_नकल(&ip_blob->bData.ia6, &ia6, परिमाण(संरचना in6_addr));
      फर्काऊ TRUE;
    }
  }
  फर्काऊ FALSE;
}

स्थिर bool get_alt_name_info(संरचना Curl_easy *data,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_size)
{
  PCERT_INFO cert_info = रिक्त;
  PCERT_EXTENSION extension = रिक्त;
  CRYPT_DECODE_PARA decode_para = { परिमाण(CRYPT_DECODE_PARA), रिक्त, रिक्त };

  यदि(!ctx) {
    failf(data, "schannel: Null certificate context.");
    फर्काऊ FALSE;
  }

  cert_info = ctx->pCertInfo;
  यदि(!cert_info) {
    failf(data, "schannel: Null certificate info.");
    फर्काऊ FALSE;
  }

  extension = CertFindExtension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  यदि(!extension) {
    failf(data, "schannel: CertFindExtension() returned no extension.");
    फर्काऊ FALSE;
  }

  यदि(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->मान_2.pbData,
                          extension->मान_2.cbData,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_size)) {
    failf(data, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    फर्काऊ FALSE;
  }
  फर्काऊ TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
  संरचना ssl_connect_data *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = रिक्त;
  SECURITY_STATUS sspi_status;
  TCHAR *cert_hostname_buff = रिक्त;
  माप_प्रकार cert_hostname_buff_index = 0;
  अचर वर्ण *conn_hostname = connssl->peer.origin->hostname;
  माप_प्रकार hostlen = म_माप(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = रिक्त;
  DWORD alt_name_info_size = 0;
  संरचना अङ्क_ip_data ip_blob = { 0 };
  bool Win8_compat;
  संरचना अङ्क_ip_data *p = &ip_blob;
  DWORD i;

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  यदि((sspi_status != SEC_E_OK) || !pCertContextServer) {
    वर्ण buffer[WINAPI_ERROR_LEN];
    failf(data, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, buffer, परिमाण(buffer)));
    गच्छ cleanup;
  }

  Win8_compat = curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                             VERSION_GREATER_THAN_EQUAL);
  यदि(get_अङ्क_host_info(p, conn_hostname) || !Win8_compat) {
    यदि(!get_alt_name_info(data, pCertContextServer,
                          &alt_name_info, &alt_name_info_size)) {
      गच्छ cleanup;
    }
  }

  यदि(p->size && alt_name_info) {
    लागि(i = 0; i < alt_name_info->cAltEntry; ++i) {
      PCERT_ALT_NAME_ENTRY entry = &alt_name_info->rgAltEntry[i];
      यदि(entry->dwAltNameChoice == CERT_ALT_NAME_IP_ADDRESS) {
        यदि(entry->IPAddress.cbData == p->size) {
          यदि(!स_भेद(entry->IPAddress.pbData, &p->bData,
                     entry->IPAddress.cbData)) {
            result = CURLE_OK;
            infof(data,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            रोक;
          }
        }
      }
    }
  }
  अन्यथा {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(data, pCertContextServer,
                               रिक्त, 0, alt_name_info, Win8_compat);
    यदि(len == 0) {
      failf(data,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      गच्छ cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * परिमाण(TCHAR));
    यदि(!cert_hostname_buff) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ cleanup;
    }
    actual_len = cert_get_name_string(data, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    यदि(actual_len != len) {
      failf(data,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      गच्छ cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    जबसम्म(cert_hostname_buff_index < len &&
          cert_hostname_buff[cert_hostname_buff_index] != TEXT('\0') &&
          result == CURLE_PEER_FAILED_VERIFICATION) {
      वर्ण *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_to_UTF8(
        &cert_hostname_buff[cert_hostname_buff_index]);
      यदि(!cert_hostname) {
        result = CURLE_OUT_OF_MEMORY;
      }
      अन्यथा {
        यदि(Curl_cert_hostcheck(cert_hostname, म_माप(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(data, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          result = CURLE_OK;
        }
        अन्यथा {
          माप_प्रकार cert_hostname_len;

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

    यदि(result == CURLE_PEER_FAILED_VERIFICATION) {
      failf(data,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    अन्यथा यदि(result != CURLE_OK)
      failf(data, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  यदि(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  फर्काऊ result;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status;
  CURLcode result = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = रिक्त;
  अचर CERT_CHAIN_CONTEXT *pChainContext = रिक्त;
  HCERTCHAINENGINE cert_chain_engine = रिक्त;
  HCERTSTORE trust_store = रिक्त;
  HCERTSTORE own_trust_store = रिक्त;

  DEBUGASSERT(BACKEND);

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  यदि((sspi_status != SEC_E_OK) || !pCertContextServer) {
    वर्ण buffer[WINAPI_ERROR_LEN];
    failf(data, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, buffer, परिमाण(buffer)));
    result = CURLE_PEER_FAILED_VERIFICATION;
  }

  यदि(result == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    यदि(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_LESS_THAN)) {
      failf(data, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      result = CURLE_SSL_CACERT_BADFILE;
    }
    अन्यथा {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, data);

      यदि(trust_store) {
        infof(data, "schannel: reusing certificate store from cache");
      }
      अन्यथा {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_MEMORY,
                                    0,
                                    (HCRYPTPROV)रिक्त,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    रिक्त);
        यदि(!trust_store) {
          वर्ण buffer[WINAPI_ERROR_LEN];
          failf(data, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(GetLastError(), buffer, परिमाण(buffer)));
          result = CURLE_SSL_CACERT_BADFILE;
        }
        अन्यथा {
          अचर संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          यदि(ca_info_blob) {
            result = add_certs_data_to_store(trust_store,
                                             (अचर वर्ण *)ca_info_blob->data,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             data);
          }
          अन्यथा {
            result = add_certs_file_to_store(trust_store,
                                             conn_config->CAfile,
                                             data);
          }
          यदि(result == CURLE_OK) {
            यदि(Curl_schannel_set_cached_cert_store(cf, data, trust_store)) {
              own_trust_store = रिक्त;
            }
          }
        }
      }
    }

    यदि(result == CURLE_OK) {
      संरचना cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_result;

      स_रखो(&engine_config, 0, परिमाण(engine_config));
      engine_config.h_Exclusive_जरा = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      यदि(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cbSize = परिमाण(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      अन्यथा
        engine_config.cbSize = परिमाण(संरचना cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_result =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      यदि(!create_engine_result) {
        वर्ण buffer[WINAPI_ERROR_LEN];
        failf(data,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(GetLastError(), buffer, परिमाण(buffer)));
        result = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  यदि(result == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    स_रखो(&ChainPara, 0, परिमाण(ChainPara));
    ChainPara.cbSize = परिमाण(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    यदि(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                रिक्त,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                रिक्त,
                                &pChainContext)) {
      वर्ण buffer[WINAPI_ERROR_LEN];
      failf(data, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(GetLastError(), buffer, परिमाण(buffer)));
      pChainContext = रिक्त;
      result = CURLE_PEER_FAILED_VERIFICATION;
    }

    यदि(result == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dwTrustErrorMask = ~(DWORD)(CERT_TRUST_IS_NOT_TIME_NESTED);
      dwTrustErrorMask &= pSimpleChain->TrustStatus.dwErrorStatus;

      यदि(conn_config->revoke_best_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dwTrustErrorMask &= ~(DWORD)(CERT_TRUST_REVOCATION_STATUS_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      यदि(dwTrustErrorMask) {
        यदि(dwTrustErrorMask & CERT_TRUST_IS_REVOKED)
          failf(data, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        अन्यथा यदि(dwTrustErrorMask & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(data, "schannel: the certificate chain is incomplete");
        अन्यथा यदि(dwTrustErrorMask & CERT_TRUST_IS_UNTRUSTED_जरा)
          failf(data, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        अन्यथा यदि(dwTrustErrorMask & CERT_TRUST_IS_NOT_TIME_VALID)
          failf(data, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        अन्यथा यदि(dwTrustErrorMask & CERT_TRUST_REVOCATION_STATUS_UNKNOWN)
          failf(data, "schannel: the revocation status is unknown");
        अन्यथा
          failf(data, "schannel: error 0x%08lx", dwTrustErrorMask);
        result = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  यदि(result == CURLE_OK) {
    यदि(conn_config->verifyhost) {
      result = Curl_verify_host(cf, data);
    }
  }

  यदि(cert_chain_engine) {
    CertFreeCertificateChainEngine(cert_chain_engine);
  }

  यदि(own_trust_store) {
    CertCloseStore(own_trust_store, 0);
  }

  यदि(pChainContext)
    CertFreeCertificateChain(pChainContext);

  यदि(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  फर्काऊ result;
}

#endif /* USE_SCHANNEL */
