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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_SCHANNEL

#నిర్వచితం_కాకపోతే USE_WINDOWS_SSPI
#దోషం "cannot compile Schannel support without SSPI."
#అయితే_ముగింపు

#అన్తర్భావ "vtls/schannel.h"
#అన్తర్భావ "vtls/schannel_int.h"

#అన్తర్భావ "vtls/hostcheck.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_int.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "strerror.h"
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "curlx/multibyte.h"
#అన్తర్భావ "curlx/version_win32.h"
#అన్తర్భావ "curlx/winapi.h"

#నిర్వచించు BACKEND ((నిర్మాణం schannel_ssl_backend_దత్తాంశం *)connssl->backend)

#నిర్వచించు MAX_CAFILE_కొలత (1024 * 1024) /* 1 MiB */
#నిర్వచించు BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#నిర్వచించు చివర_CERT        "\n-----END CERTIFICATE-----"

నిర్మాణం cert_chain_engine_config_win8 {
  DWORD cb_కొలత;
  HCERTSTORE h_Restricted_మూలం;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD గరిష్ఠ_Cached_Certificates;
  DWORD చక్రం_Detection_Modulus;
  HCERTSTORE h_Exclusive_మూలం;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#నిర్వచితం_కాకపోతే CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#నిర్వచించు CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#అయితే_ముగింపు

/* Legacy structure to supply size to Win7 clients */
నిర్మాణం cert_chain_engine_config_win7 {
  DWORD cb_కొలత;
  HCERTSTORE h_Restricted_మూలం;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD గరిష్ఠ_Cached_Certificates;
  DWORD చక్రం_Detection_Modulus;
  HCERTSTORE h_Exclusive_మూలం;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
స్థిర స్థిరరాశి అక్షరం *c_memmem(స్థిరరాశి శూన్య *haystack, size_t haystacklen,
                            స్థిరరాశి శూన్య *needle, size_t needlelen)
{
  స్థిరరాశి అక్షరం *p;
  అక్షరం మొదటి;
  స్థిరరాశి అక్షరం *str_హద్దు = (స్థిరరాశి అక్షరం *)haystack + haystacklen;
  ఒకవేళ(!needlelen || needlelen > haystacklen)
    తిరిగి ఖాళీ;
  మొదటి = *(స్థిరరాశి అక్షరం *)needle;
  కొరకు(p = (స్థిరరాశి అక్షరం *)haystack; p <= (str_హద్దు - needlelen); p++)
    ఒకవేళ((*p == మొదటి) && !memcmp(p, needle, needlelen))
      తిరిగి p;

  తిరిగి ఖాళీ;
}

స్థిర CURLcode కూడు_certs_దత్తాంశం_గమ్యం_store(HCERTSTORE trust_store,
                                        స్థిరరాశి అక్షరం *ca_బఫరు,
                                        size_t ca_బఫరు_కొలత,
                                        స్థిరరాశి అక్షరం *ca_file_పాఠ్యం,
                                        నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  స్థిరరాశి size_t చివర_cert_len = CURL_CSTRLEN(చివర_CERT);
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం అంకె_certs = 0;
  bool మరిన్ని_certs = 1;
  స్థిరరాశి అక్షరం *current_ca_file_ptr = ca_బఫరు;
  స్థిరరాశి అక్షరం *ca_బఫరు_హద్దు = ca_బఫరు + ca_బఫరు_కొలత;

  అయితే(మరిన్ని_certs && (current_ca_file_ptr < ca_బఫరు_హద్దు)) {
    స్థిరరాశి అక్షరం *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_బఫరు_హద్దు -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    ఒకవేళ(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      మరిన్ని_certs = 0;
    }
    లేకపోతే {
      స్థిరరాశి అక్షరం *చివర_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_బఫరు_హద్దు-begin_cert_ptr,
                                          చివర_CERT,
                                          చివర_cert_len);
      ఒకవేళ(!చివర_cert_ptr) {
        failf(దత్తాంశం,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_పాఠ్యం);
        ఫలితం = CURLE_SSL_CACERT_BADFILE;
        మరిన్ని_certs = 0;
      }
      లేకపోతే {
        CERT_BLOB cert_blob;
        స్థిరరాశి CERT_CONTEXT *cert_context = ఖాళీ;
        BOOL కూడు_cert_ఫలితం = FALSE;
        DWORD cert_కొలత =
          (DWORD)((చివర_cert_ptr + చివర_cert_len) - begin_cert_ptr);

        cert_blob.pb_దత్తాంశం = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cb_దత్తాంశం = cert_కొలత;
        /* Caution: CryptQueryObject() is deprecated */
        ఒకవేళ(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             ఖాళీ,
                             ఖాళీ,
                             ఖాళీ,
                             ఖాళీ,
                             ఖాళీ,
                             (స్థిరరాశి శూన్య **)&cert_context)) {
          అక్షరం బఫరు[WINAPI_ERROR_LEN];
          failf(దత్తాంశం,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_పాఠ్యం,
                curlx_winapi_strerror(Get_చివరి_Error(), బఫరు, పరిమాణం(బఫరు)));
          ఫలితం = CURLE_SSL_CACERT_BADFILE;
          మరిన్ని_certs = 0;
        }
        లేకపోతే {
          current_ca_file_ptr = begin_cert_ptr + cert_కొలత;

          కూడు_cert_ఫలితం =
            Cert_కూడు_Certificate_Context_గమ్యం_Store(trust_store,
                                             cert_context,
                                             CERT_STORE_కూడు_ALWAYS,
                                             ఖాళీ);
          ఒకవేళ(!కూడు_cert_ఫలితం) {
            అక్షరం బఫరు[WINAPI_ERROR_LEN];
            failf(దత్తాంశం,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_పాఠ్యం,
                  curlx_winapi_strerror(Get_చివరి_Error(), బఫరు,
                                        పరిమాణం(బఫరు)));
            ఫలితం = CURLE_SSL_CACERT_BADFILE;
            మరిన్ని_certs = 0;
          }
          లేకపోతే {
            అంకె_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  ఒకవేళ(ఫలితం == CURLE_OK) {
    ఒకవేళ(!అంకె_certs) {
      infof(దత్తాంశం, "schannel: did not add any certificates from CA file '%s'",
            ca_file_పాఠ్యం);
    }
    లేకపోతే {
      infof(దత్తాంశం, "schannel: added %d certificate(s) from CA file '%s'",
            అంకె_certs, ca_file_పాఠ్యం);
    }
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode కూడు_certs_file_గమ్యం_store(HCERTSTORE trust_store,
                                        స్థిరరాశి అక్షరం *ca_file,
                                        నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;
  FILE *ca_file_handle;
  అక్షరం *ca_file_బఫరు = ఖాళీ;
  దీర్ఘ ca_file_bufsize = 0;
  దీర్ఘ మొత్తం_బైట్లు_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  ఒకవేళ(!ca_file_handle) {
    failf(దత్తాంశం, "schannel: failed to open CA file '%s'", ca_file);
    ఫలితం = CURLE_SSL_CACERT_BADFILE;
    వెళ్ళు cleanup;
  }

  ఒకవేళ(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(దత్తాంశం, "schannel: failed seeking to end of CA file '%s'", ca_file);
    ఫలితం = CURLE_SSL_CACERT_BADFILE;
    వెళ్ళు cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  ఒకవేళ(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(దత్తాంశం, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    ఫలితం = CURLE_SSL_CACERT_BADFILE;
    వెళ్ళు cleanup;
  }

  ఒకవేళ(ca_file_bufsize < 0) {
    failf(దత్తాంశం, "schannel: failed to get length of CA file '%s'", ca_file);
    ఫలితం = CURLE_SSL_CACERT_BADFILE;
    వెళ్ళు cleanup;
  }

  ఒకవేళ(ca_file_bufsize > MAX_CAFILE_కొలత) {
    failf(దత్తాంశం, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_కొలత);
    ఫలితం = CURLE_SSL_CACERT_BADFILE;
    వెళ్ళు cleanup;
  }

  ca_file_బఫరు = curlx_malloc(ca_file_bufsize + 1);
  ఒకవేళ(!ca_file_బఫరు) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు cleanup;
  }

  అయితే(మొత్తం_బైట్లు_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_బఫరు + మొత్తం_బైట్లు_read, 1,
                         ca_file_bufsize - మొత్తం_బైట్లు_read, ca_file_handle);

    ఒకవేళ(ferror(ca_file_handle)) {
      failf(దత్తాంశం, "schannel: failed to read from CA file '%s'", ca_file);
      ఫలితం = CURLE_SSL_CACERT_BADFILE;
      వెళ్ళు cleanup;
    }

    ఒకవేళ(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = మొత్తం_బైట్లు_read;
    }
    లేకపోతే {
      మొత్తం_బైట్లు_read += (దీర్ఘ)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_బఫరు[ca_file_bufsize] = '\0';

  ఫలితం = కూడు_certs_దత్తాంశం_గమ్యం_store(trust_store,
                                   ca_file_బఫరు, ca_file_bufsize,
                                   ca_file,
                                   దత్తాంశం);

cleanup:
  ఒకవేళ(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_బఫరు);

  తిరిగి ఫలితం;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
స్థిర DWORD cert_get_name_string(నిర్మాణం Curl_easy *దత్తాంశం,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_పేర్లు,
                                  DWORD నిడివి,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_నిడివి = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = ఖాళీ;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  ఒకవేళ(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#నిర్వచితం_కాకపోతే CERT_NAME_శోధన_ALL_పేర్లు_FLAG
#నిర్వచించు CERT_NAME_శోధన_ALL_పేర్లు_FLAG 0x2
#అయితే_ముగింపు
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_శోధన_ALL_పేర్లు_FLAG;
    actual_నిడివి = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      ఖాళీ,
                                      host_పేర్లు,
                                      నిడివి);
    తిరిగి actual_నిడివి;
  }

  ఒకవేళ(!alt_name_info)
    తిరిగి 0;

  compute_content = host_పేర్లు && నిడివి != 0;

  /* Initialize default return values. */
  actual_నిడివి = 1;
  ఒకవేళ(compute_content) {
    *host_పేర్లు = '\0';
  }

  current_pos = host_పేర్లు;

  /* Iterate over the alternate names and populate host_names. */
  కొరకు(i = 0; i < alt_name_info->c_Alt_నమోదు; i++) {
    స్థిరరాశి CERT_ALT_NAME_నమోదు *నమోదు_2 = &alt_name_info->rg_Alt_నమోదు[i];
    wchar_t *dns_w = ఖాళీ;
    size_t current_నిడివి = 0;

    ఒకవేళ(నమోదు_2->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      కొనసాగు;
    }
    ఒకవేళ(!నమోదు_2->pwszDNSName) {
      infof(దత్తాంశం, "schannel: Empty DNS name.");
      కొనసాగు;
    }
    current_నిడివి = wcslen(నమోదు_2->pwszDNSName) + 1;
    ఒకవేళ(!compute_content) {
      actual_నిడివి += (DWORD)current_నిడివి;
      కొనసాగు;
    }
    /* Sanity check to prevent buffer overrun. */
    ఒకవేళ((actual_నిడివి + current_నిడివి) > నిడివి) {
      failf(దత్తాంశం, "schannel: Not enough memory to list all hostnames.");
      ఆపు;
    }
    dns_w = నమోదు_2->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    అయితే(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_నిడివి += (DWORD)current_నిడివి;
  }
  ఒకవేళ(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  తిరిగి actual_నిడివి;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

స్థిర bool get_అంకె_host_info(నిర్మాణం అంకె_ip_దత్తాంశం *ip_blob, LPCSTR hostname)
{
  నిర్మాణం in_addr ia;
  పూర్ణాంకం res = curlx_inet_pton(AF_INET, hostname, &ia);
  ఒకవేళ(res) {
    ip_blob->కొలత = పరిమాణం(నిర్మాణం in_addr);
    జ్ఞాపకం_నకలు(&ip_blob->b_దత్తాంశం.ia, &ia, పరిమాణం(నిర్మాణం in_addr));
    తిరిగి TRUE;
  }
  లేకపోతే {
    నిర్మాణం in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    ఒకవేళ(res) {
      ip_blob->కొలత = పరిమాణం(నిర్మాణం in6_addr);
      జ్ఞాపకం_నకలు(&ip_blob->b_దత్తాంశం.ia6, &ia6, పరిమాణం(నిర్మాణం in6_addr));
      తిరిగి TRUE;
    }
  }
  తిరిగి FALSE;
}

స్థిర bool get_alt_name_info(నిర్మాణం Curl_easy *దత్తాంశం,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_కొలత)
{
  PCERT_INFO cert_info = ఖాళీ;
  PCERT_EXTENSION extension = ఖాళీ;
  CRYPT_DECODE_PARA decode_para = { పరిమాణం(CRYPT_DECODE_PARA), ఖాళీ, ఖాళీ };

  ఒకవేళ(!ctx) {
    failf(దత్తాంశం, "schannel: Null certificate context.");
    తిరిగి FALSE;
  }

  cert_info = ctx->pCertInfo;
  ఒకవేళ(!cert_info) {
    failf(దత్తాంశం, "schannel: Null certificate info.");
    తిరిగి FALSE;
  }

  extension = Cert_కనుగొను_Extension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  ఒకవేళ(!extension) {
    failf(దత్తాంశం, "schannel: CertFindExtension() returned no extension.");
    తిరిగి FALSE;
  }

  ఒకవేళ(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->విలువ_2.pb_దత్తాంశం,
                          extension->విలువ_2.cb_దత్తాంశం,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_కొలత)) {
    failf(దత్తాంశం, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    తిరిగి FALSE;
  }
  తిరిగి TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = ఖాళీ;
  SECURITY_స్థితి sspi_స్థితి;
  TCHAR *cert_hostname_buff = ఖాళీ;
  size_t cert_hostname_buff_సూచీ = 0;
  స్థిరరాశి అక్షరం *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = అక్షరశ్రేణి_పొడవు(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = ఖాళీ;
  DWORD alt_name_info_కొలత = 0;
  నిర్మాణం అంకె_ip_దత్తాంశం ip_blob = { 0 };
  bool Win8_compat;
  నిర్మాణం అంకె_ip_దత్తాంశం *p = &ip_blob;
  DWORD i;

  sspi_స్థితి =
    Curl_pSecFn->Query_Context_లక్షణాలు(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  ఒకవేళ((sspi_స్థితి != SEC_E_OK) || !pCertContextServer) {
    అక్షరం బఫరు[WINAPI_ERROR_LEN];
    failf(దత్తాంశం, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
    వెళ్ళు cleanup;
  }

  Win8_compat = curlx_verify_windows_సంస్కరణ(6, 2, 0, PLATFORM_WINNT,
                                             సంస్కరణ_GREATER_THAN_EQUAL);
  ఒకవేళ(get_అంకె_host_info(p, conn_hostname) || !Win8_compat) {
    ఒకవేళ(!get_alt_name_info(దత్తాంశం, pCertContextServer,
                          &alt_name_info, &alt_name_info_కొలత)) {
      వెళ్ళు cleanup;
    }
  }

  ఒకవేళ(p->కొలత && alt_name_info) {
    కొరకు(i = 0; i < alt_name_info->c_Alt_నమోదు; ++i) {
      PCERT_ALT_NAME_నమోదు నమోదు_2 = &alt_name_info->rg_Alt_నమోదు[i];
      ఒకవేళ(నమోదు_2->dwAltNameChoice == CERT_ALT_NAME_IP_చిరునామా) {
        ఒకవేళ(నమోదు_2->IPAddress.cb_దత్తాంశం == p->కొలత) {
          ఒకవేళ(!memcmp(నమోదు_2->IPAddress.pb_దత్తాంశం, &p->b_దత్తాంశం,
                     నమోదు_2->IPAddress.cb_దత్తాంశం)) {
            ఫలితం = CURLE_OK;
            infof(దత్తాంశం,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            ఆపు;
          }
        }
      }
    }
  }
  లేకపోతే {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(దత్తాంశం, pCertContextServer,
                               ఖాళీ, 0, alt_name_info, Win8_compat);
    ఒకవేళ(len == 0) {
      failf(దత్తాంశం,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      వెళ్ళు cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * పరిమాణం(TCHAR));
    ఒకవేళ(!cert_hostname_buff) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు cleanup;
    }
    actual_len = cert_get_name_string(దత్తాంశం, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    ఒకవేళ(actual_len != len) {
      failf(దత్తాంశం,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      వెళ్ళు cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    అయితే(cert_hostname_buff_సూచీ < len &&
          cert_hostname_buff[cert_hostname_buff_సూచీ] != పాఠ్యం_2('\0') &&
          ఫలితం == CURLE_PEER_FAILED_VERIFICATION) {
      అక్షరం *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_గమ్యం_UTF8(
        &cert_hostname_buff[cert_hostname_buff_సూచీ]);
      ఒకవేళ(!cert_hostname) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      లేకపోతే {
        ఒకవేళ(Curl_cert_hostcheck(cert_hostname, అక్షరశ్రేణి_పొడవు(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(దత్తాంశం, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          ఫలితం = CURLE_OK;
        }
        లేకపోతే {
          size_t cert_hostname_len;

          infof(దత్తాంశం,
                "schannel: connection hostname (%s) did not match "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);

          cert_hostname_len =
            _tcslen(&cert_hostname_buff[cert_hostname_buff_సూచీ]);

          /* Move on to next cert name */
          cert_hostname_buff_సూచీ += cert_hostname_len + 1;

          ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
        }
        curlx_free(cert_hostname);
      }
    }

    ఒకవేళ(ఫలితం == CURLE_PEER_FAILED_VERIFICATION) {
      failf(దత్తాంశం,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    లేకపోతే ఒకవేళ(ఫలితం != CURLE_OK)
      failf(దత్తాంశం, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  ఒకవేళ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  తిరిగి ఫలితం;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_స్థితి sspi_స్థితి;
  CURLcode ఫలితం = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = ఖాళీ;
  స్థిరరాశి CERT_CHAIN_CONTEXT *pChainContext = ఖాళీ;
  HCERTCHAINENGINE cert_chain_engine = ఖాళీ;
  HCERTSTORE trust_store = ఖాళీ;
  HCERTSTORE own_trust_store = ఖాళీ;

  DEBUGASSERT(BACKEND);

  sspi_స్థితి =
    Curl_pSecFn->Query_Context_లక్షణాలు(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  ఒకవేళ((sspi_స్థితి != SEC_E_OK) || !pCertContextServer) {
    అక్షరం బఫరు[WINAPI_ERROR_LEN];
    failf(దత్తాంశం, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_స్థితి, బఫరు, పరిమాణం(బఫరు)));
    ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
  }

  ఒకవేళ(ఫలితం == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    ఒకవేళ(curlx_verify_windows_సంస్కరణ(6, 1, 0, PLATFORM_WINNT,
                                    సంస్కరణ_LESS_THAN)) {
      failf(దత్తాంశం, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      ఫలితం = CURLE_SSL_CACERT_BADFILE;
    }
    లేకపోతే {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, దత్తాంశం);

      ఒకవేళ(trust_store) {
        infof(దత్తాంశం, "schannel: reusing certificate store from cache");
      }
      లేకపోతే {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_జ్ఞాపకం,
                                    0,
                                    (HCRYPTPROV)ఖాళీ,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    ఖాళీ);
        ఒకవేళ(!trust_store) {
          అక్షరం బఫరు[WINAPI_ERROR_LEN];
          failf(దత్తాంశం, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(Get_చివరి_Error(), బఫరు, పరిమాణం(బఫరు)));
          ఫలితం = CURLE_SSL_CACERT_BADFILE;
        }
        లేకపోతే {
          స్థిరరాశి నిర్మాణం curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          ఒకవేళ(ca_info_blob) {
            ఫలితం = కూడు_certs_దత్తాంశం_గమ్యం_store(trust_store,
                                             (స్థిరరాశి అక్షరం *)ca_info_blob->దత్తాంశం,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             దత్తాంశం);
          }
          లేకపోతే {
            ఫలితం = కూడు_certs_file_గమ్యం_store(trust_store,
                                             conn_config->CAfile,
                                             దత్తాంశం);
          }
          ఒకవేళ(ఫలితం == CURLE_OK) {
            ఒకవేళ(Curl_schannel_సమితి_cached_cert_store(cf, దత్తాంశం, trust_store)) {
              own_trust_store = ఖాళీ;
            }
          }
        }
      }
    }

    ఒకవేళ(ఫలితం == CURLE_OK) {
      నిర్మాణం cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_ఫలితం;

      memset(&engine_config, 0, పరిమాణం(engine_config));
      engine_config.h_Exclusive_మూలం = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      ఒకవేళ(curlx_verify_windows_సంస్కరణ(6, 2, 0, PLATFORM_WINNT,
                                      సంస్కరణ_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cb_కొలత = పరిమాణం(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      లేకపోతే
        engine_config.cb_కొలత = పరిమాణం(నిర్మాణం cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_ఫలితం =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      ఒకవేళ(!create_engine_ఫలితం) {
        అక్షరం బఫరు[WINAPI_ERROR_LEN];
        failf(దత్తాంశం,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(Get_చివరి_Error(), బఫరు, పరిమాణం(బఫరు)));
        ఫలితం = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  ఒకవేళ(ఫలితం == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, పరిమాణం(ChainPara));
    ChainPara.cb_కొలత = పరిమాణం(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_సరిపోలిక_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    ఒకవేళ(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                ఖాళీ,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                ఖాళీ,
                                &pChainContext)) {
      అక్షరం బఫరు[WINAPI_ERROR_LEN];
      failf(దత్తాంశం, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(Get_చివరి_Error(), బఫరు, పరిమాణం(బఫరు)));
      pChainContext = ఖాళీ;
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    }

    ఒకవేళ(ఫలితం == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dw_Trust_Error_ముసుగు = ~(DWORD)(CERT_TRUST_IS_NOT_సమయం_NESTED);
      dw_Trust_Error_ముసుగు &= pSimpleChain->Trust_స్థితి.dw_Error_స్థితి;

      ఒకవేళ(conn_config->revoke_ఉత్తమ_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dw_Trust_Error_ముసుగు &= ~(DWORD)(CERT_TRUST_REVOCATION_స్థితి_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      ఒకవేళ(dw_Trust_Error_ముసుగు) {
        ఒకవేళ(dw_Trust_Error_ముసుగు & CERT_TRUST_IS_REVOKED)
          failf(దత్తాంశం, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        లేకపోతే ఒకవేళ(dw_Trust_Error_ముసుగు & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(దత్తాంశం, "schannel: the certificate chain is incomplete");
        లేకపోతే ఒకవేళ(dw_Trust_Error_ముసుగు & CERT_TRUST_IS_UNTRUSTED_మూలం)
          failf(దత్తాంశం, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        లేకపోతే ఒకవేళ(dw_Trust_Error_ముసుగు & CERT_TRUST_IS_NOT_సమయం_VALID)
          failf(దత్తాంశం, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        లేకపోతే ఒకవేళ(dw_Trust_Error_ముసుగు & CERT_TRUST_REVOCATION_స్థితి_UNKNOWN)
          failf(దత్తాంశం, "schannel: the revocation status is unknown");
        లేకపోతే
          failf(దత్తాంశం, "schannel: error 0x%08lx", dw_Trust_Error_ముసుగు);
        ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  ఒకవేళ(ఫలితం == CURLE_OK) {
    ఒకవేళ(conn_config->verifyhost) {
      ఫలితం = Curl_verify_host(cf, దత్తాంశం);
    }
  }

  ఒకవేళ(cert_chain_engine) {
    CertFreeCertificateChainEngine(cert_chain_engine);
  }

  ఒకవేళ(own_trust_store) {
    CertCloseStore(own_trust_store, 0);
  }

  ఒకవేళ(pChainContext)
    CertFreeCertificateChain(pChainContext);

  ఒకవేళ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* USE_SCHANNEL */
