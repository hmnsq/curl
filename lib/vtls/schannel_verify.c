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
#समावेश "curl_setup.h"

#अगर_घोषित USE_SCHANNEL

#अगर_अघोषित USE_WINDOWS_SSPI
#त्रुटि "cannot compile Schannel support without SSPI."
#पूर्ण_अगर

#समावेश "vtls/schannel.h"
#समावेश "vtls/schannel_int.h"

#समावेश "vtls/hostcheck.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_int.h"
#समावेश "curl_trc.h"
#समावेश "strerror.h"
#समावेश "curlx/win32-fopen.h"
#समावेश "curlx/inet_pton.h"
#समावेश "curlx/multibyte.h"
#समावेश "curlx/version_win32.h"
#समावेश "curlx/winapi.h"

#घोषणा BACKEND ((काष्ठा schannel_ssl_backend_तथ्य *)connssl->backend)

#घोषणा MAX_CAFILE_आकार (1024 * 1024) /* 1 MiB */
#घोषणा BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#घोषणा END_CERT        "\n-----END CERTIFICATE-----"

काष्ठा cert_chain_engine_config_win8 {
  DWORD cb_आकार;
  HCERTSTORE h_Restricted_मूल;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_मूल;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#अगर_अघोषित CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#घोषणा CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#पूर्ण_अगर

/* Legacy structure to supply size to Win7 clients */
काष्ठा cert_chain_engine_config_win7 {
  DWORD cb_आकार;
  HCERTSTORE h_Restricted_मूल;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD MaximumCachedCertificates;
  DWORD CycleDetectionModulus;
  HCERTSTORE h_Exclusive_मूल;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
अटल स्थिर अक्षर *c_memmem(स्थिर व्योम *haystack, माप_प्रकार haystacklen,
                            स्थिर व्योम *needle, माप_प्रकार needlelen)
{
  स्थिर अक्षर *p;
  अक्षर first;
  स्थिर अक्षर *str_limit = (स्थिर अक्षर *)haystack + haystacklen;
  अगर(!needlelen || needlelen > haystacklen)
    वापस शून्य;
  first = *(स्थिर अक्षर *)needle;
  क्रम(p = (स्थिर अक्षर *)haystack; p <= (str_limit - needlelen); p++)
    अगर((*p == first) && !स_भेद(p, needle, needlelen))
      वापस p;

  वापस शून्य;
}

अटल CURLcode add_certs_तथ्य_to_store(HCERTSTORE trust_store,
                                        स्थिर अक्षर *ca_बफर,
                                        माप_प्रकार ca_बफर_आकार,
                                        स्थिर अक्षर *ca_file_text,
                                        काष्ठा Curl_easy *तथ्य)
{
  स्थिर माप_प्रकार begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  स्थिर माप_प्रकार end_cert_len = CURL_CSTRLEN(END_CERT);
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक अंक_certs = 0;
  bool more_certs = 1;
  स्थिर अक्षर *current_ca_file_ptr = ca_बफर;
  स्थिर अक्षर *ca_बफर_limit = ca_बफर + ca_बफर_आकार;

  जबतक(more_certs && (current_ca_file_ptr < ca_बफर_limit)) {
    स्थिर अक्षर *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_बफर_limit -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    अगर(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      more_certs = 0;
    }
    अन्यथा {
      स्थिर अक्षर *end_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_बफर_limit-begin_cert_ptr,
                                          END_CERT,
                                          end_cert_len);
      अगर(!end_cert_ptr) {
        failf(तथ्य,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_text);
        परिणाम = CURLE_SSL_CACERT_BADFILE;
        more_certs = 0;
      }
      अन्यथा {
        CERT_BLOB cert_blob;
        स्थिर CERT_CONTEXT *cert_context = शून्य;
        BOOL add_cert_परिणाम = FALSE;
        DWORD cert_आकार =
          (DWORD)((end_cert_ptr + end_cert_len) - begin_cert_ptr);

        cert_blob.pb_तथ्य = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cb_तथ्य = cert_आकार;
        /* Caution: CryptQueryObject() is deprecated */
        अगर(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             शून्य,
                             शून्य,
                             शून्य,
                             शून्य,
                             शून्य,
                             (स्थिर व्योम **)&cert_context)) {
          अक्षर बफर[WINAPI_ERROR_LEN];
          failf(तथ्य,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_text,
                curlx_winapi_strerror(GetLastError(), बफर, माप(बफर)));
          परिणाम = CURLE_SSL_CACERT_BADFILE;
          more_certs = 0;
        }
        अन्यथा {
          current_ca_file_ptr = begin_cert_ptr + cert_आकार;

          add_cert_परिणाम =
            CertAddCertificateContextToStore(trust_store,
                                             cert_context,
                                             CERT_STORE_ADD_ALWAYS,
                                             शून्य);
          अगर(!add_cert_परिणाम) {
            अक्षर बफर[WINAPI_ERROR_LEN];
            failf(तथ्य,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_text,
                  curlx_winapi_strerror(GetLastError(), बफर,
                                        माप(बफर)));
            परिणाम = CURLE_SSL_CACERT_BADFILE;
            more_certs = 0;
          }
          अन्यथा {
            अंक_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  अगर(परिणाम == CURLE_OK) {
    अगर(!अंक_certs) {
      infof(तथ्य, "schannel: did not add any certificates from CA file '%s'",
            ca_file_text);
    }
    अन्यथा {
      infof(तथ्य, "schannel: added %d certificate(s) from CA file '%s'",
            अंक_certs, ca_file_text);
    }
  }
  वापस परिणाम;
}

अटल CURLcode add_certs_file_to_store(HCERTSTORE trust_store,
                                        स्थिर अक्षर *ca_file,
                                        काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;
  खाता *ca_file_handle;
  अक्षर *ca_file_बफर = शून्य;
  दीर्घ ca_file_bufsize = 0;
  दीर्घ योग_bytes_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  अगर(!ca_file_handle) {
    failf(तथ्य, "schannel: failed to open CA file '%s'", ca_file);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    जाओ cleanup;
  }

  अगर(curlx_fseek(ca_file_handle, 0, अंत_से)) {
    failf(तथ्य, "schannel: failed seeking to end of CA file '%s'", ca_file);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    जाओ cleanup;
  }

  ca_file_bufsize = ख_बताओ(ca_file_handle);

  अगर(curlx_fseek(ca_file_handle, 0, शुरू_से)) {
    failf(तथ्य, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    जाओ cleanup;
  }

  अगर(ca_file_bufsize < 0) {
    failf(तथ्य, "schannel: failed to get length of CA file '%s'", ca_file);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    जाओ cleanup;
  }

  अगर(ca_file_bufsize > MAX_CAFILE_आकार) {
    failf(तथ्य, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_आकार);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    जाओ cleanup;
  }

  ca_file_बफर = curlx_malloc(ca_file_bufsize + 1);
  अगर(!ca_file_बफर) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ cleanup;
  }

  जबतक(योग_bytes_read < ca_file_bufsize) {
    माप_प्रकार nread = ख_पढ़ो(ca_file_बफर + योग_bytes_read, 1,
                         ca_file_bufsize - योग_bytes_read, ca_file_handle);

    अगर(ख_त्रुटि(ca_file_handle)) {
      failf(तथ्य, "schannel: failed to read from CA file '%s'", ca_file);
      परिणाम = CURLE_SSL_CACERT_BADFILE;
      जाओ cleanup;
    }

    अगर(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = योग_bytes_read;
    }
    अन्यथा {
      योग_bytes_read += (दीर्घ)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_बफर[ca_file_bufsize] = '\0';

  परिणाम = add_certs_तथ्य_to_store(trust_store,
                                   ca_file_बफर, ca_file_bufsize,
                                   ca_file,
                                   तथ्य);

cleanup:
  अगर(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_बफर);

  वापस परिणाम;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
अटल DWORD cert_get_name_string(काष्ठा Curl_easy *तथ्य,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_names,
                                  DWORD लंबाई,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_लंबाई = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = शून्य;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  अगर(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#अगर_अघोषित CERT_NAME_खोज_ALL_NAMES_FLAG
#घोषणा CERT_NAME_खोज_ALL_NAMES_FLAG 0x2
#पूर्ण_अगर
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_खोज_ALL_NAMES_FLAG;
    actual_लंबाई = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      शून्य,
                                      host_names,
                                      लंबाई);
    वापस actual_लंबाई;
  }

  अगर(!alt_name_info)
    वापस 0;

  compute_content = host_names && लंबाई != 0;

  /* Initialize default return values. */
  actual_लंबाई = 1;
  अगर(compute_content) {
    *host_names = '\0';
  }

  current_pos = host_names;

  /* Iterate over the alternate names and populate host_names. */
  क्रम(i = 0; i < alt_name_info->cAltEntry; i++) {
    स्थिर CERT_ALT_NAME_ENTRY *entry = &alt_name_info->rgAltEntry[i];
    ब_अक्षर_प्रकार *dns_w = शून्य;
    माप_प्रकार current_लंबाई = 0;

    अगर(entry->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      जारी;
    }
    अगर(!entry->pwszDNSName) {
      infof(तथ्य, "schannel: Empty DNS name.");
      जारी;
    }
    current_लंबाई = wcslen(entry->pwszDNSName) + 1;
    अगर(!compute_content) {
      actual_लंबाई += (DWORD)current_लंबाई;
      जारी;
    }
    /* Sanity check to prevent buffer overrun. */
    अगर((actual_लंबाई + current_लंबाई) > लंबाई) {
      failf(तथ्य, "schannel: Not enough memory to list all hostnames.");
      अवरोध;
    }
    dns_w = entry->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    जबतक(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_लंबाई += (DWORD)current_लंबाई;
  }
  अगर(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  वापस actual_लंबाई;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

अटल bool get_अंक_host_info(काष्ठा अंक_ip_तथ्य *ip_blob, LPCSTR hostname)
{
  काष्ठा in_addr ia;
  पूर्णांक res = curlx_inet_pton(AF_INET, hostname, &ia);
  अगर(res) {
    ip_blob->आकार = माप(काष्ठा in_addr);
    स_नकल(&ip_blob->b_तथ्य.ia, &ia, माप(काष्ठा in_addr));
    वापस TRUE;
  }
  अन्यथा {
    काष्ठा in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    अगर(res) {
      ip_blob->आकार = माप(काष्ठा in6_addr);
      स_नकल(&ip_blob->b_तथ्य.ia6, &ia6, माप(काष्ठा in6_addr));
      वापस TRUE;
    }
  }
  वापस FALSE;
}

अटल bool get_alt_name_info(काष्ठा Curl_easy *तथ्य,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_आकार)
{
  PCERT_INFO cert_info = शून्य;
  PCERT_EXTENSION extension = शून्य;
  CRYPT_DECODE_PARA decode_para = { माप(CRYPT_DECODE_PARA), शून्य, शून्य };

  अगर(!ctx) {
    failf(तथ्य, "schannel: Null certificate context.");
    वापस FALSE;
  }

  cert_info = ctx->pCertInfo;
  अगर(!cert_info) {
    failf(तथ्य, "schannel: Null certificate info.");
    वापस FALSE;
  }

  extension = CertFindExtension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  अगर(!extension) {
    failf(तथ्य, "schannel: CertFindExtension() returned no extension.");
    वापस FALSE;
  }

  अगर(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->मूल्य_2.pb_तथ्य,
                          extension->मूल्य_2.cb_तथ्य,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_आकार)) {
    failf(तथ्य, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    वापस FALSE;
  }
  वापस TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_PEER_FAILED_VERIFICATION;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = शून्य;
  SECURITY_STATUS sspi_status;
  TCHAR *cert_hostname_buff = शून्य;
  माप_प्रकार cert_hostname_buff_सूचकांक = 0;
  स्थिर अक्षर *conn_hostname = connssl->peer.origin->hostname;
  माप_प्रकार hostlen = म_माप(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = शून्य;
  DWORD alt_name_info_आकार = 0;
  काष्ठा अंक_ip_तथ्य ip_blob = { 0 };
  bool Win8_compat;
  काष्ठा अंक_ip_तथ्य *p = &ip_blob;
  DWORD i;

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  अगर((sspi_status != SEC_E_OK) || !pCertContextServer) {
    अक्षर बफर[WINAPI_ERROR_LEN];
    failf(तथ्य, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
    जाओ cleanup;
  }

  Win8_compat = curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                             VERSION_GREATER_THAN_EQUAL);
  अगर(get_अंक_host_info(p, conn_hostname) || !Win8_compat) {
    अगर(!get_alt_name_info(तथ्य, pCertContextServer,
                          &alt_name_info, &alt_name_info_आकार)) {
      जाओ cleanup;
    }
  }

  अगर(p->आकार && alt_name_info) {
    क्रम(i = 0; i < alt_name_info->cAltEntry; ++i) {
      PCERT_ALT_NAME_ENTRY entry = &alt_name_info->rgAltEntry[i];
      अगर(entry->dwAltNameChoice == CERT_ALT_NAME_IP_ADDRESS) {
        अगर(entry->IPAddress.cb_तथ्य == p->आकार) {
          अगर(!स_भेद(entry->IPAddress.pb_तथ्य, &p->b_तथ्य,
                     entry->IPAddress.cb_तथ्य)) {
            परिणाम = CURLE_OK;
            infof(तथ्य,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            अवरोध;
          }
        }
      }
    }
  }
  अन्यथा {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(तथ्य, pCertContextServer,
                               शून्य, 0, alt_name_info, Win8_compat);
    अगर(len == 0) {
      failf(तथ्य,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      जाओ cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * माप(TCHAR));
    अगर(!cert_hostname_buff) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ cleanup;
    }
    actual_len = cert_get_name_string(तथ्य, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    अगर(actual_len != len) {
      failf(तथ्य,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      जाओ cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    जबतक(cert_hostname_buff_सूचकांक < len &&
          cert_hostname_buff[cert_hostname_buff_सूचकांक] != TEXT('\0') &&
          परिणाम == CURLE_PEER_FAILED_VERIFICATION) {
      अक्षर *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_to_UTF8(
        &cert_hostname_buff[cert_hostname_buff_सूचकांक]);
      अगर(!cert_hostname) {
        परिणाम = CURLE_OUT_OF_MEMORY;
      }
      अन्यथा {
        अगर(Curl_cert_hostcheck(cert_hostname, म_माप(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(तथ्य, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          परिणाम = CURLE_OK;
        }
        अन्यथा {
          माप_प्रकार cert_hostname_len;

          infof(तथ्य,
                "schannel: connection hostname (%s) did not match "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);

          cert_hostname_len =
            _tcslen(&cert_hostname_buff[cert_hostname_buff_सूचकांक]);

          /* Move on to next cert name */
          cert_hostname_buff_सूचकांक += cert_hostname_len + 1;

          परिणाम = CURLE_PEER_FAILED_VERIFICATION;
        }
        curlx_free(cert_hostname);
      }
    }

    अगर(परिणाम == CURLE_PEER_FAILED_VERIFICATION) {
      failf(तथ्य,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    अन्यथा अगर(परिणाम != CURLE_OK)
      failf(तथ्य, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  अगर(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  वापस परिणाम;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_STATUS sspi_status;
  CURLcode परिणाम = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = शून्य;
  स्थिर CERT_CHAIN_CONTEXT *pChainContext = शून्य;
  HCERTCHAINENGINE cert_chain_engine = शून्य;
  HCERTSTORE trust_store = शून्य;
  HCERTSTORE own_trust_store = शून्य;

  DEBUGASSERT(BACKEND);

  sspi_status =
    Curl_pSecFn->QueryContextAttributes(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  अगर((sspi_status != SEC_E_OK) || !pCertContextServer) {
    अक्षर बफर[WINAPI_ERROR_LEN];
    failf(तथ्य, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_status, बफर, माप(बफर)));
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
  }

  अगर(परिणाम == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    अगर(curlx_verify_windows_version(6, 1, 0, PLATFORM_WINNT,
                                    VERSION_LESS_THAN)) {
      failf(तथ्य, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      परिणाम = CURLE_SSL_CACERT_BADFILE;
    }
    अन्यथा {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, तथ्य);

      अगर(trust_store) {
        infof(तथ्य, "schannel: reusing certificate store from cache");
      }
      अन्यथा {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_MEMORY,
                                    0,
                                    (HCRYPTPROV)शून्य,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    शून्य);
        अगर(!trust_store) {
          अक्षर बफर[WINAPI_ERROR_LEN];
          failf(तथ्य, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(GetLastError(), बफर, माप(बफर)));
          परिणाम = CURLE_SSL_CACERT_BADFILE;
        }
        अन्यथा {
          स्थिर काष्ठा curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          अगर(ca_info_blob) {
            परिणाम = add_certs_तथ्य_to_store(trust_store,
                                             (स्थिर अक्षर *)ca_info_blob->तथ्य,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             तथ्य);
          }
          अन्यथा {
            परिणाम = add_certs_file_to_store(trust_store,
                                             conn_config->CAfile,
                                             तथ्य);
          }
          अगर(परिणाम == CURLE_OK) {
            अगर(Curl_schannel_set_cached_cert_store(cf, तथ्य, trust_store)) {
              own_trust_store = शून्य;
            }
          }
        }
      }
    }

    अगर(परिणाम == CURLE_OK) {
      काष्ठा cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_परिणाम;

      स_रखो(&engine_config, 0, माप(engine_config));
      engine_config.h_Exclusive_मूल = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      अगर(curlx_verify_windows_version(6, 2, 0, PLATFORM_WINNT,
                                      VERSION_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cb_आकार = माप(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      अन्यथा
        engine_config.cb_आकार = माप(काष्ठा cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_परिणाम =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      अगर(!create_engine_परिणाम) {
        अक्षर बफर[WINAPI_ERROR_LEN];
        failf(तथ्य,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(GetLastError(), बफर, माप(बफर)));
        परिणाम = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  अगर(परिणाम == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    स_रखो(&ChainPara, 0, माप(ChainPara));
    ChainPara.cb_आकार = माप(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    अगर(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                शून्य,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                शून्य,
                                &pChainContext)) {
      अक्षर बफर[WINAPI_ERROR_LEN];
      failf(तथ्य, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(GetLastError(), बफर, माप(बफर)));
      pChainContext = शून्य;
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }

    अगर(परिणाम == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dwTrustErrorMask = ~(DWORD)(CERT_TRUST_IS_NOT_TIME_NESTED);
      dwTrustErrorMask &= pSimpleChain->TrustStatus.dwErrorStatus;

      अगर(conn_config->revoke_best_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dwTrustErrorMask &= ~(DWORD)(CERT_TRUST_REVOCATION_STATUS_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      अगर(dwTrustErrorMask) {
        अगर(dwTrustErrorMask & CERT_TRUST_IS_REVOKED)
          failf(तथ्य, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        अन्यथा अगर(dwTrustErrorMask & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(तथ्य, "schannel: the certificate chain is incomplete");
        अन्यथा अगर(dwTrustErrorMask & CERT_TRUST_IS_UNTRUSTED_मूल)
          failf(तथ्य, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        अन्यथा अगर(dwTrustErrorMask & CERT_TRUST_IS_NOT_TIME_VALID)
          failf(तथ्य, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        अन्यथा अगर(dwTrustErrorMask & CERT_TRUST_REVOCATION_STATUS_UNKNOWN)
          failf(तथ्य, "schannel: the revocation status is unknown");
        अन्यथा
          failf(तथ्य, "schannel: error 0x%08lx", dwTrustErrorMask);
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  अगर(परिणाम == CURLE_OK) {
    अगर(conn_config->verifyhost) {
      परिणाम = Curl_verify_host(cf, तथ्य);
    }
  }

  अगर(cert_chain_engine) {
    CertFreeCertificateChainEngine(cert_chain_engine);
  }

  अगर(own_trust_store) {
    CertCloseStore(own_trust_store, 0);
  }

  अगर(pChainContext)
    CertFreeCertificateChain(pChainContext);

  अगर(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  वापस परिणाम;
}

#पूर्ण_अगर /* USE_SCHANNEL */
