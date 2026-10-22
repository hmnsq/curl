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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SCHANNEL

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINDOWS_SSPI
#ದೋಷ "cannot compile Schannel support without SSPI."
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "vtls/schannel.h"
#ಅನ್ತರ್ಭಾವ "vtls/schannel_int.h"

#ಅನ್ತರ್ಭಾವ "vtls/hostcheck.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strerror.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"
#ಅನ್ತರ್ಭಾವ "curlx/version_win32.h"
#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"

#ವ್ಯಾಖ್ಯಾನಿಸು BACKEND ((ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend)

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_CAFILE_ಗಾತ್ರ (1024 * 1024) /* 1 MiB */
#ವ್ಯಾಖ್ಯಾನಿಸು BEGIN_CERT      "-----BEGIN CERTIFICATE-----"
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆ_CERT        "\n-----END CERTIFICATE-----"

ರಚನೆ cert_chain_engine_config_win8 {
  DWORD cb_ಗಾತ್ರ;
  HCERTSTORE h_Restricted_ಬೇರು;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD ಗರಿಷ್ಠ_Cached_Certificates;
  DWORD ಚಕ್ರ_Detection_Modulus;
  HCERTSTORE h_Exclusive_ಬೇರು;
  HCERTSTORE hExclusiveTrustedPeople;
  DWORD dwExclusiveFlags;
};

/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG
#ವ್ಯಾಖ್ಯಾನಿಸು CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG 0x00000001
#ಆದರೆ_ಅಂತ್ಯ

/* Legacy structure to supply size to Win7 clients */
ರಚನೆ cert_chain_engine_config_win7 {
  DWORD cb_ಗಾತ್ರ;
  HCERTSTORE h_Restricted_ಬೇರು;
  HCERTSTORE hRestrictedTrust;
  HCERTSTORE hRestrictedOther;
  DWORD cAdditionalStore;
  HCERTSTORE *rghAdditionalStore;
  DWORD dwFlags;
  DWORD dwUrlRetrievalTimeout;
  DWORD ಗರಿಷ್ಠ_Cached_Certificates;
  DWORD ಚಕ್ರ_Detection_Modulus;
  HCERTSTORE h_Exclusive_ಬೇರು;
  HCERTSTORE hExclusiveTrustedPeople;
};

/* Search the substring needle,needlelen into string haystack,haystacklen
 * Strings do not need to be terminated by a '\0'.
 * Similar of macOS/Linux memmem (not available on Visual Studio).
 * Return position of beginning of first occurrence or NULL if not found
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c_memmem(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *haystack, size_t haystacklen,
                            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *needle, size_t needlelen)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
  ಅಕ್ಷರ ಮೊದಲ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str_ಎಲ್ಲೆ = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)haystack + haystacklen;
  ಆದರೆ(!needlelen || needlelen > haystacklen)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಮೊದಲ = *(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)needle;
  ಗಾಗಿ(p = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)haystack; p <= (str_ಎಲ್ಲೆ - needlelen); p++)
    ಆದರೆ((*p == ಮೊದಲ) && !memcmp(p, needle, needlelen))
      ಹಿಂತಿರುಗಿ p;

  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ CURLcode ಕೂಡಿಸು_certs_ದತ್ತಾಂಶ_ಗಮ್ಯ_store(HCERTSTORE trust_store,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ca_ಬಫರ್,
                                        size_t ca_ಬಫರ್_ಗಾತ್ರ,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ca_file_ಪಠ್ಯ,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ size_t begin_cert_len = CURL_CSTRLEN(BEGIN_CERT);
  ಸ್ಥಿರಾಂಕ size_t ಕೊನೆ_cert_len = CURL_CSTRLEN(ಕೊನೆ_CERT);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ_certs = 0;
  bool ಇನ್ನಷ್ಟು_certs = 1;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *current_ca_file_ptr = ca_ಬಫರ್;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ca_ಬಫರ್_ಎಲ್ಲೆ = ca_ಬಫರ್ + ca_ಬಫರ್_ಗಾತ್ರ;

  ಆಗ(ಇನ್ನಷ್ಟು_certs && (current_ca_file_ptr < ca_ಬಫರ್_ಎಲ್ಲೆ)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *begin_cert_ptr = c_memmem(current_ca_file_ptr,
                                          ca_ಬಫರ್_ಎಲ್ಲೆ -
                                          current_ca_file_ptr - 1,
                                          BEGIN_CERT,
                                          begin_cert_len);
    ಆದರೆ(!begin_cert_ptr || !ISNEWLINE(begin_cert_ptr[begin_cert_len])) {
      ಇನ್ನಷ್ಟು_certs = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ_cert_ptr = c_memmem(begin_cert_ptr,
                                          ca_ಬಫರ್_ಎಲ್ಲೆ-begin_cert_ptr,
                                          ಕೊನೆ_CERT,
                                          ಕೊನೆ_cert_len);
      ಆದರೆ(!ಕೊನೆ_cert_ptr) {
        failf(ದತ್ತಾಂಶ,
              "schannel: CA file '%s' is not correctly formatted",
              ca_file_ಪಠ್ಯ);
        ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
        ಇನ್ನಷ್ಟು_certs = 0;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        CERT_BLOB cert_blob;
        ಸ್ಥಿರಾಂಕ CERT_CONTEXT *cert_context = ಖಾಲಿ;
        BOOL ಕೂಡಿಸು_cert_ಫಲಿತಾಂಶ = FALSE;
        DWORD cert_ಗಾತ್ರ =
          (DWORD)((ಕೊನೆ_cert_ptr + ಕೊನೆ_cert_len) - begin_cert_ptr);

        cert_blob.pb_ದತ್ತಾಂಶ = (BYTE *)CURL_UNCONST(begin_cert_ptr);
        cert_blob.cb_ದತ್ತಾಂಶ = cert_ಗಾತ್ರ;
        /* Caution: CryptQueryObject() is deprecated */
        ಆದರೆ(!CryptQueryObject(CERT_QUERY_OBJECT_BLOB,
                             &cert_blob,
                             CERT_QUERY_CONTENT_FLAG_CERT,
                             CERT_QUERY_FORMAT_FLAG_BASE64_ENCODED,
                             0,
                             ಖಾಲಿ,
                             ಖಾಲಿ,
                             ಖಾಲಿ,
                             ಖಾಲಿ,
                             ಖಾಲಿ,
                             (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **)&cert_context)) {
          ಅಕ್ಷರ ಬಫರ್[WINAPI_ERROR_LEN];
          failf(ದತ್ತಾಂಶ,
                "schannel: failed to extract certificate from CA file "
                "'%s': %s",
                ca_file_ಪಠ್ಯ,
                curlx_winapi_strerror(Get_ಕೊನೆಯ_Error(), ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
          ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
          ಇನ್ನಷ್ಟು_certs = 0;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          current_ca_file_ptr = begin_cert_ptr + cert_ಗಾತ್ರ;

          ಕೂಡಿಸು_cert_ಫಲಿತಾಂಶ =
            Cert_ಕೂಡಿಸು_Certificate_Context_ಗಮ್ಯ_Store(trust_store,
                                             cert_context,
                                             CERT_STORE_ಕೂಡಿಸು_ALWAYS,
                                             ಖಾಲಿ);
          ಆದರೆ(!ಕೂಡಿಸು_cert_ಫಲಿತಾಂಶ) {
            ಅಕ್ಷರ ಬಫರ್[WINAPI_ERROR_LEN];
            failf(ದತ್ತಾಂಶ,
                  "schannel: failed to add certificate from CA file '%s' "
                  "to certificate store: %s",
                  ca_file_ಪಠ್ಯ,
                  curlx_winapi_strerror(Get_ಕೊನೆಯ_Error(), ಬಫರ್,
                                        ಗಾತ್ರ(ಬಫರ್)));
            ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
            ಇನ್ನಷ್ಟು_certs = 0;
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            ಅಂಕೆ_certs++;
          }

          CertFreeCertificateContext(cert_context);
        }
      }
    }
  }

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK) {
    ಆದರೆ(!ಅಂಕೆ_certs) {
      infof(ದತ್ತಾಂಶ, "schannel: did not add any certificates from CA file '%s'",
            ca_file_ಪಠ್ಯ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, "schannel: added %d certificate(s) from CA file '%s'",
            ಅಂಕೆ_certs, ca_file_ಪಠ್ಯ);
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ಕೂಡಿಸು_certs_file_ಗಮ್ಯ_store(HCERTSTORE trust_store,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ca_file,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;
  FILE *ca_file_handle;
  ಅಕ್ಷರ *ca_file_ಬಫರ್ = ಖಾಲಿ;
  ದೀರ್ಘ ca_file_bufsize = 0;
  ದೀರ್ಘ ಒಟ್ಟು_ಬೈಟುಗಳು_read = 0;

  /*
   * Read the CA file completely into memory before parsing it. This
   * optimizes for the common case where the CA file is relatively
   * small ( < 1 MiB ).
   */
  ca_file_handle = curlx_fopen(ca_file, "rb");
  ಆದರೆ(!ca_file_handle) {
    failf(ದತ್ತಾಂಶ, "schannel: failed to open CA file '%s'", ca_file);
    ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
    ಹೋಗು cleanup;
  }

  ಆದರೆ(curlx_fseek(ca_file_handle, 0, SEEK_END)) {
    failf(ದತ್ತಾಂಶ, "schannel: failed seeking to end of CA file '%s'", ca_file);
    ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
    ಹೋಗು cleanup;
  }

  ca_file_bufsize = ftell(ca_file_handle);

  ಆದರೆ(curlx_fseek(ca_file_handle, 0, SEEK_SET)) {
    failf(ದತ್ತಾಂಶ, "schannel: failed seeking to beginning of CA file '%s'",
          ca_file);
    ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
    ಹೋಗು cleanup;
  }

  ಆದರೆ(ca_file_bufsize < 0) {
    failf(ದತ್ತಾಂಶ, "schannel: failed to get length of CA file '%s'", ca_file);
    ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
    ಹೋಗು cleanup;
  }

  ಆದರೆ(ca_file_bufsize > MAX_CAFILE_ಗಾತ್ರ) {
    failf(ದತ್ತಾಂಶ, "schannel: CA file exceeds max size of %d bytes",
          MAX_CAFILE_ಗಾತ್ರ);
    ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
    ಹೋಗು cleanup;
  }

  ca_file_ಬಫರ್ = curlx_malloc(ca_file_bufsize + 1);
  ಆದರೆ(!ca_file_ಬಫರ್) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು cleanup;
  }

  ಆಗ(ಒಟ್ಟು_ಬೈಟುಗಳು_read < ca_file_bufsize) {
    size_t nread = fread(ca_file_ಬಫರ್ + ಒಟ್ಟು_ಬೈಟುಗಳು_read, 1,
                         ca_file_bufsize - ಒಟ್ಟು_ಬೈಟುಗಳು_read, ca_file_handle);

    ಆದರೆ(ferror(ca_file_handle)) {
      failf(ದತ್ತಾಂಶ, "schannel: failed to read from CA file '%s'", ca_file);
      ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
      ಹೋಗು cleanup;
    }

    ಆದರೆ(nread == 0) {
      /* Premature EOF -- adjust the bufsize to the new value */
      ca_file_bufsize = ಒಟ್ಟು_ಬೈಟುಗಳು_read;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಒಟ್ಟು_ಬೈಟುಗಳು_read += (ದೀರ್ಘ)nread;
    }
  }

  /* null-terminate the buffer */
  ca_file_ಬಫರ್[ca_file_bufsize] = '\0';

  ಫಲಿತಾಂಶ = ಕೂಡಿಸು_certs_ದತ್ತಾಂಶ_ಗಮ್ಯ_store(trust_store,
                                   ca_file_ಬಫರ್, ca_file_bufsize,
                                   ca_file,
                                   ದತ್ತಾಂಶ);

cleanup:
  ಆದರೆ(ca_file_handle) {
    curlx_fclose(ca_file_handle);
  }
  curlx_safefree(ca_file_ಬಫರ್);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Returns the number of characters necessary to populate all the host_names.
 * If host_names is not NULL, populate it with all the hostnames. Each string
 * in the host_names is null-terminated and the last string is double
 * null-terminated. If no DNS names are found, a single null-terminated empty
 * string is returned.
 */
ಸ್ಥಿರ DWORD cert_get_name_string(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  CERT_CONTEXT *cert_context,
                                  LPTSTR host_ಹೆಸರುಗಳು,
                                  DWORD ಉದ್ದಳತೆ,
                                  PCERT_ALT_NAME_INFO alt_name_info,
                                  BOOL Win8_compat)
{
  DWORD actual_ಉದ್ದಳತೆ = 0;
  BOOL compute_content = FALSE;
  LPTSTR current_pos = ಖಾಲಿ;
  DWORD i;

  /* CERT_NAME_SEARCH_ALL_NAMES_FLAG is available from Windows 8 onwards. */
  ಆದರೆ(Win8_compat) {
/* Offered by mingw-w64 v4+, MS SDK 8.0/~VS2012+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CERT_NAME_ಹುಡುಕು_ALL_ಹೆಸರುಗಳು_FLAG
#ವ್ಯಾಖ್ಯಾನಿಸು CERT_NAME_ಹುಡುಕು_ALL_ಹೆಸರುಗಳು_FLAG 0x2
#ಆದರೆ_ಅಂತ್ಯ
    /* CertGetNameString provides the 8-bit character string without
     * any decoding */
    DWORD name_flags =
      CERT_NAME_DISABLE_IE4_UTF8_FLAG | CERT_NAME_ಹುಡುಕು_ALL_ಹೆಸರುಗಳು_FLAG;
    actual_ಉದ್ದಳತೆ = CertGetNameString(cert_context,
                                      CERT_NAME_DNS_TYPE,
                                      name_flags,
                                      ಖಾಲಿ,
                                      host_ಹೆಸರುಗಳು,
                                      ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ actual_ಉದ್ದಳತೆ;
  }

  ಆದರೆ(!alt_name_info)
    ಹಿಂತಿರುಗಿ 0;

  compute_content = host_ಹೆಸರುಗಳು && ಉದ್ದಳತೆ != 0;

  /* Initialize default return values. */
  actual_ಉದ್ದಳತೆ = 1;
  ಆದರೆ(compute_content) {
    *host_ಹೆಸರುಗಳು = '\0';
  }

  current_pos = host_ಹೆಸರುಗಳು;

  /* Iterate over the alternate names and populate host_names. */
  ಗಾಗಿ(i = 0; i < alt_name_info->c_Alt_ನಮೂದು; i++) {
    ಸ್ಥಿರಾಂಕ CERT_ALT_NAME_ನಮೂದು *ನಮೂದು = &alt_name_info->rg_Alt_ನಮೂದು[i];
    wchar_t *dns_w = ಖಾಲಿ;
    size_t current_ಉದ್ದಳತೆ = 0;

    ಆದರೆ(ನಮೂದು->dwAltNameChoice != CERT_ALT_NAME_DNS_NAME) {
      ಮುಂದುವರಿ;
    }
    ಆದರೆ(!ನಮೂದು->pwszDNSName) {
      infof(ದತ್ತಾಂಶ, "schannel: Empty DNS name.");
      ಮುಂದುವರಿ;
    }
    current_ಉದ್ದಳತೆ = wcslen(ನಮೂದು->pwszDNSName) + 1;
    ಆದರೆ(!compute_content) {
      actual_ಉದ್ದಳತೆ += (DWORD)current_ಉದ್ದಳತೆ;
      ಮುಂದುವರಿ;
    }
    /* Sanity check to prevent buffer overrun. */
    ಆದರೆ((actual_ಉದ್ದಳತೆ + current_ಉದ್ದಳತೆ) > ಉದ್ದಳತೆ) {
      failf(ದತ್ತಾಂಶ, "schannel: Not enough memory to list all hostnames.");
      ನಿಲ್ಲಿಸು;
    }
    dns_w = ನಮೂದು->pwszDNSName;
    /* pwszDNSName is in ia5 string format and hence does not contain any
     * non-ASCII characters. */
    ಆಗ(*dns_w != '\0') {
      *current_pos++ = (TCHAR)(*dns_w++);
    }
    *current_pos++ = '\0';
    actual_ಉದ್ದಳತೆ += (DWORD)current_ಉದ್ದಳತೆ;
  }
  ಆದರೆ(compute_content) {
    /* Last string has double null-terminator. */
    *current_pos = '\0';
  }
  ಹಿಂತಿರುಗಿ actual_ಉದ್ದಳತೆ;
}

/*
 * Returns TRUE if the hostname is a numeric IPv4/IPv6 Address,
 * and populates the buffer with IPv4/IPv6 info.
 */

ಸ್ಥಿರ bool get_ಅಂಕೆ_host_info(ರಚನೆ ಅಂಕೆ_ip_ದತ್ತಾಂಶ *ip_blob, LPCSTR hostname)
{
  ರಚನೆ in_addr ia;
  ಪೂರ್ಣಾಂಕ res = curlx_inet_pton(AF_INET, hostname, &ia);
  ಆದರೆ(res) {
    ip_blob->ಗಾತ್ರ_2 = ಗಾತ್ರ(ರಚನೆ in_addr);
    ಸ್ಮರಣೆ_ನಕಲು(&ip_blob->b_ದತ್ತಾಂಶ.ia, &ia, ಗಾತ್ರ(ರಚನೆ in_addr));
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ರಚನೆ in6_addr ia6;
    res = curlx_inet_pton(AF_INET6, hostname, &ia6);
    ಆದರೆ(res) {
      ip_blob->ಗಾತ್ರ_2 = ಗಾತ್ರ(ರಚನೆ in6_addr);
      ಸ್ಮರಣೆ_ನಕಲು(&ip_blob->b_ದತ್ತಾಂಶ.ia6, &ia6, ಗಾತ್ರ(ರಚನೆ in6_addr));
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ bool get_alt_name_info(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              PCCERT_CONTEXT ctx,
                              PCERT_ALT_NAME_INFO *alt_name_info,
                              LPDWORD alt_name_info_ಗಾತ್ರ)
{
  PCERT_INFO cert_info = ಖಾಲಿ;
  PCERT_EXTENSION extension = ಖಾಲಿ;
  CRYPT_DECODE_PARA decode_para = { ಗಾತ್ರ(CRYPT_DECODE_PARA), ಖಾಲಿ, ಖಾಲಿ };

  ಆದರೆ(!ctx) {
    failf(ದತ್ತಾಂಶ, "schannel: Null certificate context.");
    ಹಿಂತಿರುಗಿ FALSE;
  }

  cert_info = ctx->pCertInfo;
  ಆದರೆ(!cert_info) {
    failf(ದತ್ತಾಂಶ, "schannel: Null certificate info.");
    ಹಿಂತಿರುಗಿ FALSE;
  }

  extension = Cert_ಕಂಡುಹಿಡಿ_Extension(szOID_SUBJECT_ALT_NAME2,
                                cert_info->cExtension,
                                cert_info->rgExtension);
  ಆದರೆ(!extension) {
    failf(ದತ್ತಾಂಶ, "schannel: CertFindExtension() returned no extension.");
    ಹಿಂತಿರುಗಿ FALSE;
  }

  ಆದರೆ(!CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                          szOID_SUBJECT_ALT_NAME2,
                          extension->ಮೌಲ್ಯ_2.pb_ದತ್ತಾಂಶ,
                          extension->ಮೌಲ್ಯ_2.cb_ದತ್ತಾಂಶ,
                          CRYPT_DECODE_ALLOC_FLAG | CRYPT_DECODE_NOCOPY_FLAG,
                          &decode_para,
                          alt_name_info,
                          alt_name_info_ಗಾತ್ರ)) {
    failf(ದತ್ತಾಂಶ, "schannel: CryptDecodeObjectEx() returned no alternate name "
          "information.");
    ಹಿಂತಿರುಗಿ FALSE;
  }
  ಹಿಂತಿರುಗಿ TRUE;
}

/* Verify the server's hostname */
CURLcode Curl_verify_host(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  CERT_CONTEXT *pCertContextServer = ಖಾಲಿ;
  SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ;
  TCHAR *cert_hostname_buff = ಖಾಲಿ;
  size_t cert_hostname_buff_ಸೂಚ್ಯಂಕ = 0;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *conn_hostname = connssl->peer.origin->hostname;
  size_t hostlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(conn_hostname);
  DWORD len = 0;
  DWORD actual_len = 0;
  PCERT_ALT_NAME_INFO alt_name_info = ಖಾಲಿ;
  DWORD alt_name_info_ಗಾತ್ರ = 0;
  ರಚನೆ ಅಂಕೆ_ip_ದತ್ತಾಂಶ ip_blob = { 0 };
  bool Win8_compat;
  ರಚನೆ ಅಂಕೆ_ip_ದತ್ತಾಂಶ *p = &ip_blob;
  DWORD i;

  sspi_ಸ್ಥಿತಿ =
    Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  ಆದರೆ((sspi_ಸ್ಥಿತಿ != SEC_E_OK) || !pCertContextServer) {
    ಅಕ್ಷರ ಬಫರ್[WINAPI_ERROR_LEN];
    failf(ದತ್ತಾಂಶ, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಹೋಗು cleanup;
  }

  Win8_compat = curlx_verify_windows_ಆವೃತ್ತಿ(6, 2, 0, PLATFORM_WINNT,
                                             ಆವೃತ್ತಿ_GREATER_THAN_EQUAL);
  ಆದರೆ(get_ಅಂಕೆ_host_info(p, conn_hostname) || !Win8_compat) {
    ಆದರೆ(!get_alt_name_info(ದತ್ತಾಂಶ, pCertContextServer,
                          &alt_name_info, &alt_name_info_ಗಾತ್ರ)) {
      ಹೋಗು cleanup;
    }
  }

  ಆದರೆ(p->ಗಾತ್ರ_2 && alt_name_info) {
    ಗಾಗಿ(i = 0; i < alt_name_info->c_Alt_ನಮೂದು; ++i) {
      PCERT_ALT_NAME_ನಮೂದು ನಮೂದು = &alt_name_info->rg_Alt_ನಮೂದು[i];
      ಆದರೆ(ನಮೂದು->dwAltNameChoice == CERT_ALT_NAME_IP_ವಿಳಾಸ) {
        ಆದರೆ(ನಮೂದು->IPAddress.cb_ದತ್ತಾಂಶ == p->ಗಾತ್ರ_2) {
          ಆದರೆ(!memcmp(ನಮೂದು->IPAddress.pb_ದತ್ತಾಂಶ, &p->b_ದತ್ತಾಂಶ,
                     ನಮೂದು->IPAddress.cb_ದತ್ತಾಂಶ)) {
            ಫಲಿತಾಂಶ = CURLE_OK;
            infof(ದತ್ತಾಂಶ,
             "schannel: connection hostname (%s) matched cert's IP address!",
             conn_hostname);
            ನಿಲ್ಲಿಸು;
          }
        }
      }
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Determine the size of the string needed for the cert hostname */
    len = cert_get_name_string(ದತ್ತಾಂಶ, pCertContextServer,
                               ಖಾಲಿ, 0, alt_name_info, Win8_compat);
    ಆದರೆ(len == 0) {
      failf(ದತ್ತಾಂಶ,
            "schannel: CertGetNameString() returned no "
            "certificate name information");
      ಹೋಗು cleanup;
    }

    /* CertGetNameString guarantees that the returned name does not contain
     * embedded NUL bytes. This appears to be undocumented behavior.
     */
    cert_hostname_buff = curlx_malloc(len * ಗಾತ್ರ(TCHAR));
    ಆದರೆ(!cert_hostname_buff) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು cleanup;
    }
    actual_len = cert_get_name_string(ದತ್ತಾಂಶ, pCertContextServer,
                                      (LPTSTR)cert_hostname_buff, len,
                                      alt_name_info, Win8_compat);

    /* Sanity check */
    ಆದರೆ(actual_len != len) {
      failf(ದತ್ತಾಂಶ,
            "schannel: CertGetNameString() returned certificate "
            "name information of unexpected size");
      ಹೋಗು cleanup;
    }

    /* cert_hostname_buff contains all DNS names, where each name is
     * null-terminated and the last DNS name is double null-terminated. Due to
     * this encoding, use the length of the buffer to iterate over all names.
     */
    ಆಗ(cert_hostname_buff_ಸೂಚ್ಯಂಕ < len &&
          cert_hostname_buff[cert_hostname_buff_ಸೂಚ್ಯಂಕ] != ಪಠ್ಯ_2('\0') &&
          ಫಲಿತಾಂಶ == CURLE_PEER_FAILED_VERIFICATION) {
      ಅಕ್ಷರ *cert_hostname;

      /* Comparing the cert name and the connection hostname encoded as UTF-8
       * is acceptable since both values are assumed to use ASCII
       * (or some equivalent) encoding
       */
      cert_hostname = curlx_convert_tchar_ಗಮ್ಯ_UTF8(
        &cert_hostname_buff[cert_hostname_buff_ಸೂಚ್ಯಂಕ]);
      ಆದರೆ(!cert_hostname) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(Curl_cert_hostcheck(cert_hostname, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(cert_hostname),
                               conn_hostname, hostlen)) {
          infof(ದತ್ತಾಂಶ, "schannel: connection hostname (%s) validated "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);
          ಫಲಿತಾಂಶ = CURLE_OK;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          size_t cert_hostname_len;

          infof(ದತ್ತಾಂಶ,
                "schannel: connection hostname (%s) did not match "
                "against certificate name (%s)",
                conn_hostname, cert_hostname);

          cert_hostname_len =
            _tcslen(&cert_hostname_buff[cert_hostname_buff_ಸೂಚ್ಯಂಕ]);

          /* Move on to next cert name */
          cert_hostname_buff_ಸೂಚ್ಯಂಕ += cert_hostname_len + 1;

          ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
        }
        curlx_free(cert_hostname);
      }
    }

    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_PEER_FAILED_VERIFICATION) {
      failf(ದತ್ತಾಂಶ,
            "schannel: CertGetNameString() failed to match "
            "connection hostname (%s) against server certificate names",
            conn_hostname);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_OK)
      failf(ದತ್ತಾಂಶ, "schannel: server certificate name verification failed");
  }

cleanup:
  LocalFree(alt_name_info);
  curlx_safefree(cert_hostname_buff);

  ಆದರೆ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Verify the server's certificate and hostname */
CURLcode Curl_verify_certificate(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  SECURITY_ಸ್ಥಿತಿ sspi_ಸ್ಥಿತಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  CERT_CONTEXT *pCertContextServer = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ CERT_CHAIN_CONTEXT *pChainContext = ಖಾಲಿ;
  HCERTCHAINENGINE cert_chain_engine = ಖಾಲಿ;
  HCERTSTORE trust_store = ಖಾಲಿ;
  HCERTSTORE own_trust_store = ಖಾಲಿ;

  DEBUGASSERT(BACKEND);

  sspi_ಸ್ಥಿತಿ =
    Curl_pSecFn->Query_Context_ಗುಣಲಕ್ಷಣಗಳು(&BACKEND->ctxt->ctxt_handle,
                                        SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                        &pCertContextServer);

  ಆದರೆ((sspi_ಸ್ಥಿತಿ != SEC_E_OK) || !pCertContextServer) {
    ಅಕ್ಷರ ಬಫರ್[WINAPI_ERROR_LEN];
    failf(ದತ್ತಾಂಶ, "schannel: Failed to read remote certificate context: %s",
          Curl_sspi_strerror(sspi_ಸ್ಥಿತಿ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
  }

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK &&
     (conn_config->CAfile || conn_config->ca_info_blob) &&
     BACKEND->use_manual_cred_validation) {
    /*
     * Create a chain engine that uses the certificates in the CA file as
     * trusted certificates. This is only supported on Windows 7+.
     */

    ಆದರೆ(curlx_verify_windows_ಆವೃತ್ತಿ(6, 1, 0, PLATFORM_WINNT,
                                    ಆವೃತ್ತಿ_LESS_THAN)) {
      failf(ದತ್ತಾಂಶ, "schannel: this version of Windows is too old to support "
            "certificate verification via CA bundle file.");
      ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* try cache */
      trust_store = Curl_schannel_get_cached_cert_store(cf, ದತ್ತಾಂಶ);

      ಆದರೆ(trust_store) {
        infof(ದತ್ತಾಂಶ, "schannel: reusing certificate store from cache");
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* Open the certificate store */
        trust_store = CertOpenStore(CERT_STORE_PROV_ಸ್ಮರಣೆ,
                                    0,
                                    (HCRYPTPROV)ಖಾಲಿ,
                                    CERT_STORE_CREATE_NEW_FLAG,
                                    ಖಾಲಿ);
        ಆದರೆ(!trust_store) {
          ಅಕ್ಷರ ಬಫರ್[WINAPI_ERROR_LEN];
          failf(ದತ್ತಾಂಶ, "schannel: failed to create certificate store: %s",
                curlx_winapi_strerror(Get_ಕೊನೆಯ_Error(), ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
          ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ca_info_blob = conn_config->ca_info_blob;
          own_trust_store = trust_store;

          ಆದರೆ(ca_info_blob) {
            ಫಲಿತಾಂಶ = ಕೂಡಿಸು_certs_ದತ್ತಾಂಶ_ಗಮ್ಯ_store(trust_store,
                                             (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ca_info_blob->ದತ್ತಾಂಶ,
                                             ca_info_blob->len,
                                             "(memory blob)",
                                             ದತ್ತಾಂಶ);
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            ಫಲಿತಾಂಶ = ಕೂಡಿಸು_certs_file_ಗಮ್ಯ_store(trust_store,
                                             conn_config->CAfile,
                                             ದತ್ತಾಂಶ);
          }
          ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK) {
            ಆದರೆ(Curl_schannel_ಗಣ_cached_cert_store(cf, ದತ್ತಾಂಶ, trust_store)) {
              own_trust_store = ಖಾಲಿ;
            }
          }
        }
      }
    }

    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK) {
      ರಚನೆ cert_chain_engine_config_win8 engine_config;
      BOOL create_engine_ಫಲಿತಾಂಶ;

      memset(&engine_config, 0, ಗಾತ್ರ(engine_config));
      engine_config.h_Exclusive_ಬೇರು = trust_store;

      /* Win8/Server2012 allows us to match partial chains */
      ಆದರೆ(curlx_verify_windows_ಆವೃತ್ತಿ(6, 2, 0, PLATFORM_WINNT,
                                      ಆವೃತ್ತಿ_GREATER_THAN_EQUAL) &&
         !conn_config->no_partialchain) {
        engine_config.cb_ಗಾತ್ರ = ಗಾತ್ರ(engine_config);
        engine_config.dwExclusiveFlags = CERT_CHAIN_EXCLUSIVE_ENABLE_CA_FLAG;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        engine_config.cb_ಗಾತ್ರ = ಗಾತ್ರ(ರಚನೆ cert_chain_engine_config_win7);

      /* CertCreateCertificateChainEngine checks the expected size of the
       * CERT_CHAIN_ENGINE_CONFIG structure and fail if the specified size
       * does not match the expected size. When this occurs, it indicates that
       * CAINFO is not supported on the version of Windows in use.
       */
      create_engine_ಫಲಿತಾಂಶ =
        CertCreateCertificateChainEngine(
          (CERT_CHAIN_ENGINE_CONFIG *)&engine_config, &cert_chain_engine);
      ಆದರೆ(!create_engine_ಫಲಿತಾಂಶ) {
        ಅಕ್ಷರ ಬಫರ್[WINAPI_ERROR_LEN];
        failf(ದತ್ತಾಂಶ,
              "schannel: failed to create certificate chain engine: %s",
              curlx_winapi_strerror(Get_ಕೊನೆಯ_Error(), ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
        ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
      }
    }
  }

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK) {
    CERT_CHAIN_PARA ChainPara;
    LPSTR serverAuthOID = CURL_UNCONST(szOID_PKIX_KP_SERVER_AUTH);

    memset(&ChainPara, 0, ಗಾತ್ರ(ChainPara));
    ChainPara.cb_ಗಾತ್ರ = ಗಾತ್ರ(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_ಹೊಂದಿಕೆ_TYPE_AND;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier = 1;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = &serverAuthOID;

    ಆದರೆ(!CertGetCertificateChain(cert_chain_engine,
                                pCertContextServer,
                                ಖಾಲಿ,
                                pCertContextServer->hCertStore,
                                &ChainPara,
                                (conn_config->no_revoke ? 0 :
                                 CERT_CHAIN_REVOCATION_CHECK_CHAIN),
                                ಖಾಲಿ,
                                &pChainContext)) {
      ಅಕ್ಷರ ಬಫರ್[WINAPI_ERROR_LEN];
      failf(ದತ್ತಾಂಶ, "schannel: failed to get the certificate chain: %s",
            curlx_winapi_strerror(Get_ಕೊನೆಯ_Error(), ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      pChainContext = ಖಾಲಿ;
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    }

    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK) {
      CERT_SIMPLE_CHAIN *pSimpleChain = pChainContext->rgpChain[0];
      DWORD dw_Trust_Error_ಮುಖವಾಡ = ~(DWORD)(CERT_TRUST_IS_NOT_ಸಮಯ_NESTED);
      dw_Trust_Error_ಮುಖವಾಡ &= pSimpleChain->Trust_ಸ್ಥಿತಿ.dw_Error_ಸ್ಥಿತಿ;

      ಆದರೆ(conn_config->revoke_ಉತ್ತಮ_effort) {
        /* Ignore errors when root certificates are missing the revocation
         * list URL, or when the list could not be downloaded because the
         * server is currently unreachable. */
        dw_Trust_Error_ಮುಖವಾಡ &= ~(DWORD)(CERT_TRUST_REVOCATION_ಸ್ಥಿತಿ_UNKNOWN |
                                     CERT_TRUST_IS_OFFLINE_REVOCATION);
      }

      ಆದರೆ(dw_Trust_Error_ಮುಖವಾಡ) {
        ಆದರೆ(dw_Trust_Error_ಮುಖವಾಡ & CERT_TRUST_IS_REVOKED)
          failf(ದತ್ತಾಂಶ, "schannel: trust for this certificate or one of "
                "the certificates in the certificate chain has been revoked");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dw_Trust_Error_ಮುಖವಾಡ & CERT_TRUST_IS_PARTIAL_CHAIN)
          failf(ದತ್ತಾಂಶ, "schannel: the certificate chain is incomplete");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dw_Trust_Error_ಮುಖವಾಡ & CERT_TRUST_IS_UNTRUSTED_ಬೇರು)
          failf(ದತ್ತಾಂಶ, "schannel: the certificate or certificate chain is "
                "based on an untrusted root");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dw_Trust_Error_ಮುಖವಾಡ & CERT_TRUST_IS_NOT_ಸಮಯ_VALID)
          failf(ದತ್ತಾಂಶ, "schannel: this certificate or one of the certificates "
                "in the certificate chain is not time valid");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dw_Trust_Error_ಮುಖವಾಡ & CERT_TRUST_REVOCATION_ಸ್ಥಿತಿ_UNKNOWN)
          failf(ದತ್ತಾಂಶ, "schannel: the revocation status is unknown");
        ಇಲ್ಲದಿದ್ದರೆ
          failf(ದತ್ತಾಂಶ, "schannel: error 0x%08lx", dw_Trust_Error_ಮುಖವಾಡ);
        ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
      }
    }
  }

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK) {
    ಆದರೆ(conn_config->verifyhost) {
      ಫಲಿತಾಂಶ = Curl_verify_host(cf, ದತ್ತಾಂಶ);
    }
  }

  ಆದರೆ(cert_chain_engine) {
    CertFreeCertificateChainEngine(cert_chain_engine);
  }

  ಆದರೆ(own_trust_store) {
    CertCloseStore(own_trust_store, 0);
  }

  ಆದರೆ(pChainContext)
    CertFreeCertificateChain(pChainContext);

  ಆದರೆ(pCertContextServer)
    CertFreeCertificateContext(pCertContextServer);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_SCHANNEL */
