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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SCHANNEL

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
#ਗਲਤੀ "cannot compile Schannel support without SSPI."
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/schannel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/schannel_int.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/hostcheck.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strerror.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/multibyte.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/version_win32.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/winapi.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKEND ((ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ *)connssl->backend)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_CAFILE_ਮਾਪ (1024 * 1024) /* 1 MiB */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਾ_CERT        "\n-----END CERTIFICATE-----"

ਬਣਤਰ cert_chain_engine_config_win8 {
  DWORD cb_ਮਾਪ;
  HCERTSTORE h_Restricted_ਜੜ੍ਹ;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD ਵੱਧ_ਤੋਂ_ਵੱਧ_Cached_Certificates;
  DWORD ਚੱਕਰ_Detection_Modulus;
  HCERTSTORE h_Exclusive_ਜੜ੍ਹ;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#ਜੇ_ਅੰਤ

/* Legacy structure to supply size to Win7 clients */
ਬਣਤਰ cert_chain_engine_config_win7 {
  DWORD cb_ਮਾਪ;
  HCERTSTORE h_Restricted_ਜੜ੍ਹ;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD ਵੱਧ_ਤੋਂ_ਵੱਧ_Cached_Certificates;
  DWORD ਚੱਕਰ_Detection_Modulus;
  HCERTSTORE h_Exclusive_ਜੜ੍ਹ;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *c_memmem(ਅਚਲ ਖਾਲੀ *haystack, size_t haystacklen,
                            ਅਚਲ ਖਾਲੀ *needle, size_t needlelen)
{
  ਅਚਲ ਅੱਖਰ *p;
  ਅੱਖਰ ਪਹਿਲਾ;
  ਅਚਲ ਅੱਖਰ *str_ਹੱਦਬੰਦੀ = (ਅਚਲ ਅੱਖਰ *)haystack + haystacklen;
  ਜੇ(!needlelen || needlelen > haystacklen)
    ਵਾਪਸ ਨਿਰਾ;
  ਪਹਿਲਾ = *(ਅਚਲ ਅੱਖਰ *)needle;
  ਲਈ(p = (ਅਚਲ ਅੱਖਰ *)haystack; p <= (str_ਹੱਦਬੰਦੀ - needlelen); p++)
    ਜੇ((*p == ਪਹਿਲਾ) && !memcmp(p, needle, needlelen))
      ਵਾਪਸ p;

  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ CURLcode ਜੋੜੋ_certs_ਅੰਕੜੇ_ਮੰਜ਼ਿਲ_store(HCERTSTORE trust_store,
                                        ਅਚਲ ਅੱਖਰ *ca_ਬਫਰ,
                                        size_t ca_ਬਫਰ_ਮਾਪ,
                                        ਅਚਲ ਅੱਖਰ *ca_file_ਪਾਠ,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  ਅਚਲ size_t ਸਿਰਾ_cert_len = CURL_CSTRLEN(ਸਿਰਾ_CERT);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ ਅੰਕ_certs = 0;
  bool ਹੋਰ_certs = 1;
  ਅਚਲ ਅੱਖਰ *current_ca_file_ptr = ca_ਬਫਰ;
  ਅਚਲ ਅੱਖਰ *ca_ਬਫਰ_ਹੱਦਬੰਦੀ = ca_ਬਫਰ + ca_ਬਫਰ_ਮਾਪ;

  ਜਦੋਂ(ਹੋਰ_certs && (current_ca_file_ptr < ca_ਬਫਰ_ਹੱਦਬੰਦੀ)) {
    ਅਚਲ ਅੱਖਰ *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_ਬਫਰ_ਹੱਦਬੰਦੀ -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    ਜੇ(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      ਹੋਰ_certs = 0;
    }
    ਨਹੀਂਤਾਂ {
      ਅਚਲ ਅੱਖਰ *ਸਿਰਾ_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_ਬਫਰ_ਹੱਦਬੰਦੀ-begin_cert_ptr,
                                          ਸਿਰਾ_CERT,
                                          ਸਿਰਾ_cert_len);
      ਜੇ(!ਸਿਰਾ_cert_ptr) {
        failf(ਅੰਕੜੇ,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_ਪਾਠ);
        ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
        ਹੋਰ_certs = 0;
      }
      ਨਹੀਂਤਾਂ {
        CERT_BLOB cert_blob;
        ਅਚਲ CERT_CONTEXT *cert_context = ਨਿਰਾ;
        BOOL ਜੋੜੋ_cert_ਨਤੀਜਾ = FALSE;
        DWORD cert_ਮਾਪ =
          (DWORD)((ਸਿਰਾ_cert_ptr + ਸਿਰਾ_cert_len) - begin_cert_ptr);

        cert_blob.pb_ਅੰਕੜੇ = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cb_ਅੰਕੜੇ = cert_ਮਾਪ;
        /* Caution: CryptQueryObject() is deprecated */
        ਜੇ(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             ਨਿਰਾ,
                             ਨਿਰਾ,
                             ਨਿਰਾ,
                             ਨਿਰਾ,
                             ਨਿਰਾ,
                             (ਅਚਲ ਖਾਲੀ **)&cert_context)) {
          ਅੱਖਰ ਬਫਰ[WINAPI_ERROR_LEN];
          failf(ਅੰਕੜੇ,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_ਪਾਠ,
                curlx_winapi_strerror(Get_ਆਖਰੀ_Error(), ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
          ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
          ਹੋਰ_certs = 0;
        }
        ਨਹੀਂਤਾਂ {
          current_ca_file_ptr = begin_cert_ptr + cert_ਮਾਪ;

          ਜੋੜੋ_cert_ਨਤੀਜਾ =
            Cert_ਜੋੜੋ_Certificate_Context_ਮੰਜ਼ਿਲ_Store(trust_store,
                                             cert_context,
                                             CERT_STORE_ਜੋੜੋ_ALWAYS,
                                             ਨਿਰਾ);
          ਜੇ(!ਜੋੜੋ_cert_ਨਤੀਜਾ) {
            ਅੱਖਰ ਬਫਰ[WINAPI_ERROR_LEN];
            failf(ਅੰਕੜੇ,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_ਪਾਠ,
                  curlx_winapi_strerror(Get_ਆਖਰੀ_Error(), ਬਫਰ,
                                        ਆਕਾਰ(ਬਫਰ)));
            ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
            ਹੋਰ_certs = 0;
          }
          ਨਹੀਂਤਾਂ {
            ਅੰਕ_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  ਜੇ(ਨਤੀਜਾ == CURLE_OK) {
    ਜੇ(!ਅੰਕ_certs) {
      infof(ਅੰਕੜੇ, "schannel: did not add any certificates from CA file '%s'",
            ca_file_ਪਾਠ);
    }
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, "schannel: added %d certificate(s) from CA file '%s'",
            ਅੰਕ_certs, ca_file_ਪਾਠ);
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ਜੋੜੋ_certs_file_ਮੰਜ਼ਿਲ_store(HCERTSTORE trust_store,
                                        ਅਚਲ ਅੱਖਰ *ca_file,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;
  FILE *ca_file_handle;
  ਅੱਖਰ *ca_file_ਬਫਰ = ਨਿਰਾ;
  ਲੰਮਾ ca_file_bufsize = 0;
  ਲੰਮਾ ਕੁੱਲ_ਬਾਈਟਾਂ_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  ਜੇ(!ca_file_handle) {
    failf(ਅੰਕੜੇ, "schannel: failed to open CA file '%s'", ca_file);
    ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
    ਜਾਓ cleanup;
  }

  ਜੇ(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(ਅੰਕੜੇ, "schannel: failed seeking to end of CA file '%s'", ca_file);
    ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
    ਜਾਓ cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  ਜੇ(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(ਅੰਕੜੇ, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
    ਜਾਓ cleanup;
  }

  ਜੇ(ca_file_bufsize < 0) {
    failf(ਅੰਕੜੇ, "schannel: failed to get length of CA file '%s'", ca_file);
    ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
    ਜਾਓ cleanup;
  }

  ਜੇ(ca_file_bufsize > MAX_CAFILE_ਮਾਪ) {
    failf(ਅੰਕੜੇ, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_ਮਾਪ);
    ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
    ਜਾਓ cleanup;
  }

  ca_file_ਬਫਰ = curlx_malloc(ca_file_bufsize + 1);
  ਜੇ(!ca_file_ਬਫਰ) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ cleanup;
  }

  ਜਦੋਂ(ਕੁੱਲ_ਬਾਈਟਾਂ_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_ਬਫਰ + ਕੁੱਲ_ਬਾਈਟਾਂ_read, 1,
                         ca_file_bufsize - ਕੁੱਲ_ਬਾਈਟਾਂ_read, ca_file_handle);

    ਜੇ(ferror(ca_file_handle)) {
      failf(ਅੰਕੜੇ, "schannel: failed to read from CA file '%s'", ca_file);
      ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
      ਜਾਓ cleanup;
    }

    ਜੇ(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = ਕੁੱਲ_ਬਾਈਟਾਂ_read;
    }
    ਨਹੀਂਤਾਂ {
      ਕੁੱਲ_ਬਾਈਟਾਂ_read += (ਲੰਮਾ)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_ਬਫਰ[ca_file_bufsize] = '\0';

  ਨਤੀਜਾ = ਜੋੜੋ_certs_ਅੰਕੜੇ_ਮੰਜ਼ਿਲ_store(trust_store,
                                   ca_file_ਬਫਰ, ca_file_bufsize,
                                   ca_file,
                                   ਅੰਕੜੇ);

cleanup:
  ਜੇ(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_ਬਫਰ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
ਸਥਿਰ DWORD cert_get_name_string(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_ਨਾਂ,
                                  DWORD ਲੰਮਾਈ,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_ਲੰਮਾਈ = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = ਨਿਰਾ;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  ਜੇ(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CERT_NAME_ਖੋਜ_ALL_ਨਾਂ_FLAG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CERT_NAME_ਖੋਜ_ALL_ਨਾਂ_FLAG 0x2
#ਜੇ_ਅੰਤ
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_ਖੋਜ_ALL_ਨਾਂ_FLAG;
    actual_ਲੰਮਾਈ = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      ਨਿਰਾ,
                                      host_ਨਾਂ,
                                      ਲੰਮਾਈ);
    ਵਾਪਸ actual_ਲੰਮਾਈ;
  }

  ਜੇ(!alt_name_info)
    ਵਾਪਸ 0;

  compute_content = host_ਨਾਂ && ਲੰਮਾਈ != 0;

  /* Initialize default return values. */
  actual_ਲੰਮਾਈ = 1;
  ਜੇ(compute_content) {
    *host_ਨਾਂ = '\0';
  }

  current_pos = host_ਨਾਂ;

  /* Iterate over the alternate names and populate host_names. */
  ਲਈ(i = 0; i < alt_name_info->c_Alt_ਇੰਦਰਾਜ; i++) {
    ਅਚਲ CERT_ALT_NAME_ਇੰਦਰਾਜ *ਇੰਦਰਾਜ = &alt_name_info->rg_Alt_ਇੰਦਰਾਜ[i];
    wchar_t *dns_w = ਨਿਰਾ;
    size_t current_ਲੰਮਾਈ = 0;

    ਜੇ(ਇੰਦਰਾਜ->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      ਜਾਰੀ;
    }
    ਜੇ(!ਇੰਦਰਾਜ->pwszDNSName) {
      infof(ਅੰਕੜੇ, "schannel: Empty DNS name.");
      ਜਾਰੀ;
    }
    current_ਲੰਮਾਈ = wcslen(ਇੰਦਰਾਜ->pwszDNSName) + 1;
    ਜੇ(!compute_content) {
      actual_ਲੰਮਾਈ += (DWORD)current_ਲੰਮਾਈ;
      ਜਾਰੀ;
    }
    /* Sanity check to prevent buffer overrun. */
    ਜੇ((actual_ਲੰਮਾਈ + current_ਲੰਮਾਈ) > ਲੰਮਾਈ) {
      failf(ਅੰਕੜੇ, "schannel: Not enough memory to list all hostnames.");
      ਰੋਕੋ;
    }
    dns_w = ਇੰਦਰਾਜ->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    ਜਦੋਂ(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_ਲੰਮਾਈ += (DWORD)current_ਲੰਮਾਈ;
  }
  ਜੇ(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  ਵਾਪਸ actual_ਲੰਮਾਈ;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

ਸਥਿਰ bool get_ਅੰਕ_host_info(ਬਣਤਰ ਅੰਕ_ip_ਅੰਕੜੇ *ip_blob, LPCSTR hostname)
{
  ਬਣਤਰ in_addr ia;
  ਪੂਰਨ res = curlx_inet_pton(AF_INET, hostname, &ia);
  ਜੇ(res) {
    ip_blob->ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ in_addr);
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ip_blob->b_ਅੰਕੜੇ.ia, &ia, ਆਕਾਰ(ਬਣਤਰ in_addr));
    ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ {
    ਬਣਤਰ in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    ਜੇ(res) {
      ip_blob->ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ in6_addr);
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(&ip_blob->b_ਅੰਕੜੇ.ia6, &ia6, ਆਕਾਰ(ਬਣਤਰ in6_addr));
      ਵਾਪਸ TRUE;
    }
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ bool get_alt_name_info(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_ਮਾਪ)
{
  PCERT_INFO cert_info = ਨਿਰਾ;
  PCERT_EXTENSION extension = ਨਿਰਾ;
  CRYPT_DECODE_PARA decode_para = { ਆਕਾਰ(CRYPT_DECODE_PARA), ਨਿਰਾ, ਨਿਰਾ };

  ਜੇ(!ctx) {
    failf(ਅੰਕੜੇ, "schannel: Null certificate context.");
    ਵਾਪਸ FALSE;
  }

  cert_info = ctx->pCertInfo;
  ਜੇ(!cert_info) {
    failf(ਅੰਕੜੇ, "schannel: Null certificate info.");
    ਵਾਪਸ FALSE;
  }

  extension = Cert_ਲੱਭੋ_Extension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  ਜੇ(!extension) {
    failf(ਅੰਕੜੇ, "schannel: CertFindExtension() returned no extension.");
    ਵਾਪਸ FALSE;
  }

  ਜੇ(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->ਮੁੱਲ_2.pb_ਅੰਕੜੇ,
                          extension->ਮੁੱਲ_2.cb_ਅੰਕੜੇ,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_ਮਾਪ)) {
    failf(ਅੰਕੜੇ, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    ਵਾਪਸ FALSE;
  }
  ਵਾਪਸ TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = ਨਿਰਾ;
  SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ;
  TCHAR *cert_hostname_buff = ਨਿਰਾ;
  size_t cert_hostname_buff_ਸੂਚਕਾਂਕ = 0;
  ਅਚਲ ਅੱਖਰ *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = ਲੜੀ_ਲੰਬਾਈ(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = ਨਿਰਾ;
  DWORD alt_name_info_ਮਾਪ = 0;
  ਬਣਤਰ ਅੰਕ_ip_ਅੰਕੜੇ ip_blob = { 0 };
  bool Win8_compat;
  ਬਣਤਰ ਅੰਕ_ip_ਅੰਕੜੇ *p = &ip_blob;
  DWORD i;

  sspi_ਸਥਿਤੀ =
    Curl_pSecFn->Query_Context_ਗੁਣ(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  ਜੇ((sspi_ਸਥਿਤੀ != SEC_E_OK) || !pCertContextServer) {
    ਅੱਖਰ ਬਫਰ[WINAPI_ERROR_LEN];
    failf(ਅੰਕੜੇ, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਜਾਓ cleanup;
  }

  Win8_compat = curlx_verify_windows_ਸੰਸਕਰਣ(6, 2, 0, PLATFORM_WINNT,
                                             ਸੰਸਕਰਣ_GREATER_THAN_EQUAL);
  ਜੇ(get_ਅੰਕ_host_info(p, conn_hostname) || !Win8_compat) {
    ਜੇ(!get_alt_name_info(ਅੰਕੜੇ, pCertContextServer,
                          &alt_name_info, &alt_name_info_ਮਾਪ)) {
      ਜਾਓ cleanup;
    }
  }

  ਜੇ(p->ਮਾਪ && alt_name_info) {
    ਲਈ(i = 0; i < alt_name_info->c_Alt_ਇੰਦਰਾਜ; ++i) {
      PCERT_ALT_NAME_ਇੰਦਰਾਜ ਇੰਦਰਾਜ = &alt_name_info->rg_Alt_ਇੰਦਰਾਜ[i];
      ਜੇ(ਇੰਦਰਾਜ->dwAltNameChoice == CERT_ALT_NAME_IP_ਪਤਾ) {
        ਜੇ(ਇੰਦਰਾਜ->IPAddress.cb_ਅੰਕੜੇ == p->ਮਾਪ) {
          ਜੇ(!memcmp(ਇੰਦਰਾਜ->IPAddress.pb_ਅੰਕੜੇ, &p->b_ਅੰਕੜੇ,
                     ਇੰਦਰਾਜ->IPAddress.cb_ਅੰਕੜੇ)) {
            ਨਤੀਜਾ = CURLE_OK;
            infof(ਅੰਕੜੇ,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            ਰੋਕੋ;
          }
        }
      }
    }
  }
  ਨਹੀਂਤਾਂ {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(ਅੰਕੜੇ, pCertContextServer,
                               ਨਿਰਾ, 0, alt_name_info, Win8_compat);
    ਜੇ(len == 0) {
      failf(ਅੰਕੜੇ,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      ਜਾਓ cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * ਆਕਾਰ(TCHAR));
    ਜੇ(!cert_hostname_buff) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ cleanup;
    }
    actual_len = cert_get_name_string(ਅੰਕੜੇ, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    ਜੇ(actual_len != len) {
      failf(ਅੰਕੜੇ,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      ਜਾਓ cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    ਜਦੋਂ(cert_hostname_buff_ਸੂਚਕਾਂਕ < len &&
          cert_hostname_buff[cert_hostname_buff_ਸੂਚਕਾਂਕ] != ਪਾਠ_2('\0') &&
          ਨਤੀਜਾ == CURLE_PEER_FAILED_VERIFICATION) {
      ਅੱਖਰ *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_ਮੰਜ਼ਿਲ_UTF8(
        &cert_hostname_buff[cert_hostname_buff_ਸੂਚਕਾਂਕ]);
      ਜੇ(!cert_hostname) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(Curl_cert_hostcheck(cert_hostname, ਲੜੀ_ਲੰਬਾਈ(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(ਅੰਕੜੇ, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          ਨਤੀਜਾ = CURLE_OK;
        }
        ਨਹੀਂਤਾਂ {
          size_t cert_hostname_len;

          infof(ਅੰਕੜੇ,
                "schannel: connection hostname (%s) did not match "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);

          cert_hostname_len =
            _tcslen(&cert_hostname_buff[cert_hostname_buff_ਸੂਚਕਾਂਕ]);

          /* Move on to next cert name */
          cert_hostname_buff_ਸੂਚਕਾਂਕ += cert_hostname_len + 1;

          ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
        }
        curlx_free(cert_hostname);
      }
    }

    ਜੇ(ਨਤੀਜਾ == CURLE_PEER_FAILED_VERIFICATION) {
      failf(ਅੰਕੜੇ,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ != CURLE_OK)
      failf(ਅੰਕੜੇ, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  ਜੇ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_ਸਥਿਤੀ sspi_ਸਥਿਤੀ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = ਨਿਰਾ;
  ਅਚਲ CERT_CHAIN_CONTEXT *pChainContext = ਨਿਰਾ;
  HCERTCHAINENGINE cert_chain_engine = ਨਿਰਾ;
  HCERTSTORE trust_store = ਨਿਰਾ;
  HCERTSTORE own_trust_store = ਨਿਰਾ;

  DEBUGASSERT(BACKEND);

  sspi_ਸਥਿਤੀ =
    Curl_pSecFn->Query_Context_ਗੁਣ(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  ਜੇ((sspi_ਸਥਿਤੀ != SEC_E_OK) || !pCertContextServer) {
    ਅੱਖਰ ਬਫਰ[WINAPI_ERROR_LEN];
    failf(ਅੰਕੜੇ, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_ਸਥਿਤੀ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
  }

  ਜੇ(ਨਤੀਜਾ == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    ਜੇ(curlx_verify_windows_ਸੰਸਕਰਣ(6, 1, 0, PLATFORM_WINNT,
                                    ਸੰਸਕਰਣ_LESS_THAN)) {
      failf(ਅੰਕੜੇ, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
    }
    ਨਹੀਂਤਾਂ {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, ਅੰਕੜੇ);

      ਜੇ(trust_store) {
        infof(ਅੰਕੜੇ, "schannel: reusing certificate store from cache");
      }
      ਨਹੀਂਤਾਂ {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_ਸਮ੍ਰਿਤੀ,
                                    0,
                                    (HCRYPTPROV)ਨਿਰਾ,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    ਨਿਰਾ);
        ਜੇ(!trust_store) {
          ਅੱਖਰ ਬਫਰ[WINAPI_ERROR_LEN];
          failf(ਅੰਕੜੇ, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(Get_ਆਖਰੀ_Error(), ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
          ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
        }
        ਨਹੀਂਤਾਂ {
          ਅਚਲ ਬਣਤਰ curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          ਜੇ(ca_info_blob) {
            ਨਤੀਜਾ = ਜੋੜੋ_certs_ਅੰਕੜੇ_ਮੰਜ਼ਿਲ_store(trust_store,
                                             (ਅਚਲ ਅੱਖਰ *)ca_info_blob->ਅੰਕੜੇ,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             ਅੰਕੜੇ);
          }
          ਨਹੀਂਤਾਂ {
            ਨਤੀਜਾ = ਜੋੜੋ_certs_file_ਮੰਜ਼ਿਲ_store(trust_store,
                                             conn_config->CAfile,
                                             ਅੰਕੜੇ);
          }
          ਜੇ(ਨਤੀਜਾ == CURLE_OK) {
            ਜੇ(Curl_schannel_ਸੈੱਟ_cached_cert_store(cf, ਅੰਕੜੇ, trust_store)) {
              own_trust_store = ਨਿਰਾ;
            }
          }
        }
      }
    }

    ਜੇ(ਨਤੀਜਾ == CURLE_OK) {
      ਬਣਤਰ cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_ਨਤੀਜਾ;

      memset(&engine_config, 0, ਆਕਾਰ(engine_config));
      engine_config.h_Exclusive_ਜੜ੍ਹ = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      ਜੇ(curlx_verify_windows_ਸੰਸਕਰਣ(6, 2, 0, PLATFORM_WINNT,
                                      ਸੰਸਕਰਣ_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cb_ਮਾਪ = ਆਕਾਰ(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      ਨਹੀਂਤਾਂ
        engine_config.cb_ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_ਨਤੀਜਾ =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      ਜੇ(!create_engine_ਨਤੀਜਾ) {
        ਅੱਖਰ ਬਫਰ[WINAPI_ERROR_LEN];
        failf(ਅੰਕੜੇ,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(Get_ਆਖਰੀ_Error(), ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
        ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  ਜੇ(ਨਤੀਜਾ == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, ਆਕਾਰ(ChainPara));
    ChainPara.cb_ਮਾਪ = ਆਕਾਰ(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_ਮੇਲ_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    ਜੇ(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                ਨਿਰਾ,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                ਨਿਰਾ,
                                &pChainContext)) {
      ਅੱਖਰ ਬਫਰ[WINAPI_ERROR_LEN];
      failf(ਅੰਕੜੇ, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(Get_ਆਖਰੀ_Error(), ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      pChainContext = ਨਿਰਾ;
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    }

    ਜੇ(ਨਤੀਜਾ == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dw_Trust_Error_ਨਕਾਬ = ~(DWORD)(CERT_TRUST_IS_NOT_ਸਮਾਂ_NESTED);
      dw_Trust_Error_ਨਕਾਬ &= pSimpleChain->Trust_ਸਥਿਤੀ.dw_Error_ਸਥਿਤੀ;

      ਜੇ(conn_config->revoke_ਉੱਤਮ_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dw_Trust_Error_ਨਕਾਬ &= ~(DWORD)(CERT_TRUST_REVOCATION_ਸਥਿਤੀ_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      ਜੇ(dw_Trust_Error_ਨਕਾਬ) {
        ਜੇ(dw_Trust_Error_ਨਕਾਬ & CERT_TRUST_IS_REVOKED)
          failf(ਅੰਕੜੇ, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        ਨਹੀਂਤਾਂ ਜੇ(dw_Trust_Error_ਨਕਾਬ & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(ਅੰਕੜੇ, "schannel: the certificate chain is incomplete");
        ਨਹੀਂਤਾਂ ਜੇ(dw_Trust_Error_ਨਕਾਬ & CERT_TRUST_IS_UNTRUSTED_ਜੜ੍ਹ)
          failf(ਅੰਕੜੇ, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        ਨਹੀਂਤਾਂ ਜੇ(dw_Trust_Error_ਨਕਾਬ & CERT_TRUST_IS_NOT_ਸਮਾਂ_VALID)
          failf(ਅੰਕੜੇ, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        ਨਹੀਂਤਾਂ ਜੇ(dw_Trust_Error_ਨਕਾਬ & CERT_TRUST_REVOCATION_ਸਥਿਤੀ_UNKNOWN)
          failf(ਅੰਕੜੇ, "schannel: the revocation status is unknown");
        ਨਹੀਂਤਾਂ
          failf(ਅੰਕੜੇ, "schannel: error 0x%08lx", dw_Trust_Error_ਨਕਾਬ);
        ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  ਜੇ(ਨਤੀਜਾ == CURLE_OK) {
    ਜੇ(conn_config->verifyhost) {
      ਨਤੀਜਾ = Curl_verify_host(cf, ਅੰਕੜੇ);
    }
  }

  ਜੇ(cert_chain_engine) {
    CertFreeCertificateChainEngine(cert_chain_engine);
  }

  ਜੇ(own_trust_store) {
    CertCloseStore(own_trust_store, 0);
  }

  ਜੇ(pChainContext)
    CertFreeCertificateChain(pChainContext);

  ਜੇ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* USE_SCHANNEL */
