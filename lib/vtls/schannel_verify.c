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

#यदि_परिभाषितम् USE_SCHANNEL

#यदि_अपरिभाषितम् USE_WINDOWS_SSPI
#दोषः "cannot compile Schannel support without SSPI."
#यदि_अन्तः

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

#परिभाषय BACKEND ((संरचना schannel_ssl_backend_दत्त *)connssl->backend)

#परिभाषय MAX_CAFILE_आकार (1024 * 1024) /* 1 MiB */
#परिभाषय BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#परिभाषय अन्तः_CERT        "\n-----END CERTIFICATE-----"

संरचना cert_chain_engine_config_win8 {
  DWORD cb_आकार;
  HCERTSTORE h_Restricted_मूलम्;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD अधिकतमम्_Cached_Certificates;
  DWORD आवृत्तिः_Detection_Modulus;
  HCERTSTORE h_Exclusive_मूलम्;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#यदि_अपरिभाषितम् CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#परिभाषय CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#यदि_अन्तः

/* Legacy structure to supply size to Win7 clients */
संरचना cert_chain_engine_config_win7 {
  DWORD cb_आकार;
  HCERTSTORE h_Restricted_मूलम्;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD अधिकतमम्_Cached_Certificates;
  DWORD आवृत्तिः_Detection_Modulus;
  HCERTSTORE h_Exclusive_मूलम्;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
स्थिर ध्रुव वर्ण *c_memmem(ध्रुव शून्य *haystack, size_t haystacklen,
                            ध्रुव शून्य *needle, size_t needlelen)
{
  ध्रुव वर्ण *p;
  वर्ण first;
  ध्रुव वर्ण *str_सीमा = (ध्रुव वर्ण *)haystack + haystacklen;
  यदि(!needlelen || needlelen > haystacklen)
    प्रत्यागम रिक्त;
  first = *(ध्रुव वर्ण *)needle;
  प्रति(p = (ध्रुव वर्ण *)haystack; p <= (str_सीमा - needlelen); p++)
    यदि((*p == first) && !memcmp(p, needle, needlelen))
      प्रत्यागम p;

  प्रत्यागम रिक्त;
}

स्थिर CURLcode सङ्कलय_certs_दत्त_गन्तव्यम्_store(HCERTSTORE trust_store,
                                        ध्रुव वर्ण *ca_बफर,
                                        size_t ca_बफर_आकार,
                                        ध्रुव वर्ण *ca_file_text,
                                        संरचना Curl_easy *दत्त)
{
  ध्रुव size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  ध्रुव size_t अन्तः_cert_len = CURL_CSTRLEN(अन्तः_CERT);
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क अङ्कः_certs = 0;
  bool अधिकम्_certs = 1;
  ध्रुव वर्ण *current_ca_file_ptr = ca_बफर;
  ध्रुव वर्ण *ca_बफर_सीमा = ca_बफर + ca_बफर_आकार;

  यावत्(अधिकम्_certs && (current_ca_file_ptr < ca_बफर_सीमा)) {
    ध्रुव वर्ण *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_बफर_सीमा -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    यदि(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      अधिकम्_certs = 0;
    }
    अन्यथा {
      ध्रुव वर्ण *अन्तः_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_बफर_सीमा-begin_cert_ptr,
                                          अन्तः_CERT,
                                          अन्तः_cert_len);
      यदि(!अन्तः_cert_ptr) {
        failf(दत्त,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_text);
        परिणाम = CURLE_SSL_CACERT_BADFILE;
        अधिकम्_certs = 0;
      }
      अन्यथा {
        CERT_BLOB cert_blob;
        ध्रुव CERT_CONTEXT *cert_context = रिक्त;
        BOOL सङ्कलय_cert_परिणाम = FALSE;
        DWORD cert_आकार =
          (DWORD)((अन्तः_cert_ptr + अन्तः_cert_len) - begin_cert_ptr);

        cert_blob.pb_दत्त = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cb_दत्त = cert_आकार;
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
                             (ध्रुव शून्य **)&cert_context)) {
          वर्ण बफर[WINAPI_ERROR_LEN];
          failf(दत्त,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_text,
                curlx_winapi_strerror(Get_चरमः_Error(), बफर, परिमाण(बफर)));
          परिणाम = CURLE_SSL_CACERT_BADFILE;
          अधिकम्_certs = 0;
        }
        अन्यथा {
          current_ca_file_ptr = begin_cert_ptr + cert_आकार;

          सङ्कलय_cert_परिणाम =
            Cert_सङ्कलय_Certificate_Context_गन्तव्यम्_Store(trust_store,
                                             cert_context,
                                             CERT_STORE_सङ्कलय_ALWAYS,
                                             रिक्त);
          यदि(!सङ्कलय_cert_परिणाम) {
            वर्ण बफर[WINAPI_ERROR_LEN];
            failf(दत्त,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_text,
                  curlx_winapi_strerror(Get_चरमः_Error(), बफर,
                                        परिमाण(बफर)));
            परिणाम = CURLE_SSL_CACERT_BADFILE;
            अधिकम्_certs = 0;
          }
          अन्यथा {
            अङ्कः_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  यदि(परिणाम == CURLE_OK) {
    यदि(!अङ्कः_certs) {
      infof(दत्त, "schannel: did not add any certificates from CA file '%s'",
            ca_file_text);
    }
    अन्यथा {
      infof(दत्त, "schannel: added %d certificate(s) from CA file '%s'",
            अङ्कः_certs, ca_file_text);
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode सङ्कलय_certs_file_गन्तव्यम्_store(HCERTSTORE trust_store,
                                        ध्रुव वर्ण *ca_file,
                                        संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;
  FILE *ca_file_handle;
  वर्ण *ca_file_बफर = रिक्त;
  दीर्घ ca_file_bufsize = 0;
  दीर्घ योग_अष्टकानि_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  यदि(!ca_file_handle) {
    failf(दत्त, "schannel: failed to open CA file '%s'", ca_file);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  यदि(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(दत्त, "schannel: failed seeking to end of CA file '%s'", ca_file);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  यदि(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(दत्त, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  यदि(ca_file_bufsize < 0) {
    failf(दत्त, "schannel: failed to get length of CA file '%s'", ca_file);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  यदि(ca_file_bufsize > MAX_CAFILE_आकार) {
    failf(दत्त, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_आकार);
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  ca_file_बफर = curlx_malloc(ca_file_bufsize + 1);
  यदि(!ca_file_बफर) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ cleanup;
  }

  यावत्(योग_अष्टकानि_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_बफर + योग_अष्टकानि_read, 1,
                         ca_file_bufsize - योग_अष्टकानि_read, ca_file_handle);

    यदि(ferror(ca_file_handle)) {
      failf(दत्त, "schannel: failed to read from CA file '%s'", ca_file);
      परिणाम = CURLE_SSL_CACERT_BADFILE;
      गच्छ cleanup;
    }

    यदि(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = योग_अष्टकानि_read;
    }
    अन्यथा {
      योग_अष्टकानि_read += (दीर्घ)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_बफर[ca_file_bufsize] = '\0';

  परिणाम = सङ्कलय_certs_दत्त_गन्तव्यम्_store(trust_store,
                                   ca_file_बफर, ca_file_bufsize,
                                   ca_file,
                                   दत्त);

cleanup:
  यदि(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_बफर);

  प्रत्यागम परिणाम;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
स्थिर DWORD cert_get_name_string(संरचना Curl_easy *दत्त,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_नामानि,
                                  DWORD दीर्घता,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_दीर्घता = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = रिक्त;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  यदि(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#यदि_अपरिभाषितम् CERT_NAME_अन्वेषणम्_ALL_नामानि_FLAG
#परिभाषय CERT_NAME_अन्वेषणम्_ALL_नामानि_FLAG 0x2
#यदि_अन्तः
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_अन्वेषणम्_ALL_नामानि_FLAG;
    actual_दीर्घता = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      रिक्त,
                                      host_नामानि,
                                      दीर्घता);
    प्रत्यागम actual_दीर्घता;
  }

  यदि(!alt_name_info)
    प्रत्यागम 0;

  compute_content = host_नामानि && दीर्घता != 0;

  /* Initialize default return values. */
  actual_दीर्घता = 1;
  यदि(compute_content) {
    *host_नामानि = '\0';
  }

  current_pos = host_नामानि;

  /* Iterate over the alternate names and populate host_names. */
  प्रति(i = 0; i < alt_name_info->c_Alt_प्रविष्टिः; i++) {
    ध्रुव CERT_ALT_NAME_प्रविष्टिः *प्रविष्टिः = &alt_name_info->rg_Alt_प्रविष्टिः[i];
    wchar_t *dns_w = रिक्त;
    size_t current_दीर्घता = 0;

    यदि(प्रविष्टिः->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      अनुवर्त;
    }
    यदि(!प्रविष्टिः->pwszDNSName) {
      infof(दत्त, "schannel: Empty DNS name.");
      अनुवर्त;
    }
    current_दीर्घता = wcslen(प्रविष्टिः->pwszDNSName) + 1;
    यदि(!compute_content) {
      actual_दीर्घता += (DWORD)current_दीर्घता;
      अनुवर्त;
    }
    /* Sanity check to prevent buffer overrun. */
    यदि((actual_दीर्घता + current_दीर्घता) > दीर्घता) {
      failf(दत्त, "schannel: Not enough memory to list all hostnames.");
      भङ्ग;
    }
    dns_w = प्रविष्टिः->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    यावत्(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_दीर्घता += (DWORD)current_दीर्घता;
  }
  यदि(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  प्रत्यागम actual_दीर्घता;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

स्थिर bool get_अङ्कः_host_info(संरचना अङ्कः_ip_दत्त *ip_blob, LPCSTR hostname)
{
  संरचना in_addr ia;
  पूर्णाङ्क res = curlx_inet_pton(AF_INET, hostname, &ia);
  यदि(res) {
    ip_blob->आकार = परिमाण(संरचना in_addr);
    स्मृतिं_प्रतिलिख(&ip_blob->b_दत्त.ia, &ia, परिमाण(संरचना in_addr));
    प्रत्यागम TRUE;
  }
  अन्यथा {
    संरचना in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    यदि(res) {
      ip_blob->आकार = परिमाण(संरचना in6_addr);
      स्मृतिं_प्रतिलिख(&ip_blob->b_दत्त.ia6, &ia6, परिमाण(संरचना in6_addr));
      प्रत्यागम TRUE;
    }
  }
  प्रत्यागम FALSE;
}

स्थिर bool get_alt_name_info(संरचना Curl_easy *दत्त,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_आकार)
{
  PCERT_INFO cert_info = रिक्त;
  PCERT_EXTENSION extension = रिक्त;
  CRYPT_DECODE_PARA decode_para = { परिमाण(CRYPT_DECODE_PARA), रिक्त, रिक्त };

  यदि(!ctx) {
    failf(दत्त, "schannel: Null certificate context.");
    प्रत्यागम FALSE;
  }

  cert_info = ctx->pCertInfo;
  यदि(!cert_info) {
    failf(दत्त, "schannel: Null certificate info.");
    प्रत्यागम FALSE;
  }

  extension = CertFindExtension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  यदि(!extension) {
    failf(दत्त, "schannel: CertFindExtension() returned no extension.");
    प्रत्यागम FALSE;
  }

  यदि(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->मानम्_2.pb_दत्त,
                          extension->मानम्_2.cb_दत्त,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_आकार)) {
    failf(दत्त, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    प्रत्यागम FALSE;
  }
  प्रत्यागम TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_PEER_FAILED_VERIFICATION;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = रिक्त;
  SECURITY_स्थितिः sspi_स्थितिः;
  TCHAR *cert_hostname_buff = रिक्त;
  size_t cert_hostname_buff_सूचकांक = 0;
  ध्रुव वर्ण *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = वाक्यदैर्घ्यम्(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = रिक्त;
  DWORD alt_name_info_आकार = 0;
  संरचना अङ्कः_ip_दत्त ip_blob = { 0 };
  bool Win8_compat;
  संरचना अङ्कः_ip_दत्त *p = &ip_blob;
  DWORD i;

  sspi_स्थितिः =
    Curl_pSecFn->Query_Context_गुणाः(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  यदि((sspi_स्थितिः != SEC_E_OK) || !pCertContextServer) {
    वर्ण बफर[WINAPI_ERROR_LEN];
    failf(दत्त, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
    गच्छ cleanup;
  }

  Win8_compat = curlx_verify_windows_संस्करणम्(6, 2, 0, PLATFORM_WINNT,
                                             संस्करणम्_GREATER_THAN_EQUAL);
  यदि(get_अङ्कः_host_info(p, conn_hostname) || !Win8_compat) {
    यदि(!get_alt_name_info(दत्त, pCertContextServer,
                          &alt_name_info, &alt_name_info_आकार)) {
      गच्छ cleanup;
    }
  }

  यदि(p->आकार && alt_name_info) {
    प्रति(i = 0; i < alt_name_info->c_Alt_प्रविष्टिः; ++i) {
      PCERT_ALT_NAME_प्रविष्टिः प्रविष्टिः = &alt_name_info->rg_Alt_प्रविष्टिः[i];
      यदि(प्रविष्टिः->dwAltNameChoice == CERT_ALT_NAME_IP_स्थानाङ्कः) {
        यदि(प्रविष्टिः->IPAddress.cb_दत्त == p->आकार) {
          यदि(!memcmp(प्रविष्टिः->IPAddress.pb_दत्त, &p->b_दत्त,
                     प्रविष्टिः->IPAddress.cb_दत्त)) {
            परिणाम = CURLE_OK;
            infof(दत्त,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            भङ्ग;
          }
        }
      }
    }
  }
  अन्यथा {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(दत्त, pCertContextServer,
                               रिक्त, 0, alt_name_info, Win8_compat);
    यदि(len == 0) {
      failf(दत्त,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      गच्छ cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * परिमाण(TCHAR));
    यदि(!cert_hostname_buff) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ cleanup;
    }
    actual_len = cert_get_name_string(दत्त, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    यदि(actual_len != len) {
      failf(दत्त,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      गच्छ cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    यावत्(cert_hostname_buff_सूचकांक < len &&
          cert_hostname_buff[cert_hostname_buff_सूचकांक] != TEXT('\0') &&
          परिणाम == CURLE_PEER_FAILED_VERIFICATION) {
      वर्ण *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_गन्तव्यम्_UTF8(
        &cert_hostname_buff[cert_hostname_buff_सूचकांक]);
      यदि(!cert_hostname) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
      अन्यथा {
        यदि(Curl_cert_hostcheck(cert_hostname, वाक्यदैर्घ्यम्(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(दत्त, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          परिणाम = CURLE_OK;
        }
        अन्यथा {
          size_t cert_hostname_len;

          infof(दत्त,
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

    यदि(परिणाम == CURLE_PEER_FAILED_VERIFICATION) {
      failf(दत्त,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    अन्यथा यदि(परिणाम != CURLE_OK)
      failf(दत्त, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  यदि(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  प्रत्यागम परिणाम;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_स्थितिः sspi_स्थितिः;
  CURLcode परिणाम = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = रिक्त;
  ध्रुव CERT_CHAIN_CONTEXT *pChainContext = रिक्त;
  HCERTCHAINENGINE cert_chain_engine = रिक्त;
  HCERTSTORE trust_store = रिक्त;
  HCERTSTORE own_trust_store = रिक्त;

  DEBUGASSERT(BACKEND);

  sspi_स्थितिः =
    Curl_pSecFn->Query_Context_गुणाः(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  यदि((sspi_स्थितिः != SEC_E_OK) || !pCertContextServer) {
    वर्ण बफर[WINAPI_ERROR_LEN];
    failf(दत्त, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_स्थितिः, बफर, परिमाण(बफर)));
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
  }

  यदि(परिणाम == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    यदि(curlx_verify_windows_संस्करणम्(6, 1, 0, PLATFORM_WINNT,
                                    संस्करणम्_LESS_THAN)) {
      failf(दत्त, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      परिणाम = CURLE_SSL_CACERT_BADFILE;
    }
    अन्यथा {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, दत्त);

      यदि(trust_store) {
        infof(दत्त, "schannel: reusing certificate store from cache");
      }
      अन्यथा {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_स्मृतिः,
                                    0,
                                    (HCRYPTPROV)रिक्त,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    रिक्त);
        यदि(!trust_store) {
          वर्ण बफर[WINAPI_ERROR_LEN];
          failf(दत्त, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(Get_चरमः_Error(), बफर, परिमाण(बफर)));
          परिणाम = CURLE_SSL_CACERT_BADFILE;
        }
        अन्यथा {
          ध्रुव संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          यदि(ca_info_blob) {
            परिणाम = सङ्कलय_certs_दत्त_गन्तव्यम्_store(trust_store,
                                             (ध्रुव वर्ण *)ca_info_blob->दत्त,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             दत्त);
          }
          अन्यथा {
            परिणाम = सङ्कलय_certs_file_गन्तव्यम्_store(trust_store,
                                             conn_config->CAfile,
                                             दत्त);
          }
          यदि(परिणाम == CURLE_OK) {
            यदि(Curl_schannel_समुच्चयः_cached_cert_store(cf, दत्त, trust_store)) {
              own_trust_store = रिक्त;
            }
          }
        }
      }
    }

    यदि(परिणाम == CURLE_OK) {
      संरचना cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_परिणाम;

      स्मृतिं_पूरय(&engine_config, 0, परिमाण(engine_config));
      engine_config.h_Exclusive_मूलम् = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      यदि(curlx_verify_windows_संस्करणम्(6, 2, 0, PLATFORM_WINNT,
                                      संस्करणम्_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cb_आकार = परिमाण(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      अन्यथा
        engine_config.cb_आकार = परिमाण(संरचना cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_परिणाम =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      यदि(!create_engine_परिणाम) {
        वर्ण बफर[WINAPI_ERROR_LEN];
        failf(दत्त,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(Get_चरमः_Error(), बफर, परिमाण(बफर)));
        परिणाम = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  यदि(परिणाम == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    स्मृतिं_पूरय(&ChainPara, 0, परिमाण(ChainPara));
    ChainPara.cb_आकार = परिमाण(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_मेलनम्_TYPE_AND;
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
      वर्ण बफर[WINAPI_ERROR_LEN];
      failf(दत्त, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(Get_चरमः_Error(), बफर, परिमाण(बफर)));
      pChainContext = रिक्त;
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }

    यदि(परिणाम == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dw_Trust_Error_आवरणम् = ~(DWORD)(CERT_TRUST_IS_NOT_TIME_NESTED);
      dw_Trust_Error_आवरणम् &= pSimpleChain->Trust_स्थितिः.dw_Error_स्थितिः;

      यदि(conn_config->revoke_best_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dw_Trust_Error_आवरणम् &= ~(DWORD)(CERT_TRUST_REVOCATION_स्थितिः_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      यदि(dw_Trust_Error_आवरणम्) {
        यदि(dw_Trust_Error_आवरणम् & CERT_TRUST_IS_REVOKED)
          failf(दत्त, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        अन्यथा यदि(dw_Trust_Error_आवरणम् & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(दत्त, "schannel: the certificate chain is incomplete");
        अन्यथा यदि(dw_Trust_Error_आवरणम् & CERT_TRUST_IS_UNTRUSTED_मूलम्)
          failf(दत्त, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        अन्यथा यदि(dw_Trust_Error_आवरणम् & CERT_TRUST_IS_NOT_TIME_VALID)
          failf(दत्त, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        अन्यथा यदि(dw_Trust_Error_आवरणम् & CERT_TRUST_REVOCATION_स्थितिः_UNKNOWN)
          failf(दत्त, "schannel: the revocation status is unknown");
        अन्यथा
          failf(दत्त, "schannel: error 0x%08lx", dw_Trust_Error_आवरणम्);
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  यदि(परिणाम == CURLE_OK) {
    यदि(conn_config->verifyhost) {
      परिणाम = Curl_verify_host(cf, दत्त);
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

  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* USE_SCHANNEL */
