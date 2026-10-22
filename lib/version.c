/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_NGHTTP2
#ಅನ್ತರ್ಭಾವ <nghttp2/nghttp2.h>
#endif

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "http2.h"
#ಅನ್ತರ್ಭಾವ "vssh/ssh.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#ಅನ್ತರ್ಭಾವ <idn2.h>
#endif

#ifdef USE_LIBPSL
#ಅನ್ತರ್ಭಾವ <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#ಅನ್ತರ್ಭಾವ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Wvla"
#endif
#ಅನ್ತರ್ಭಾವ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic ತೆಗೆ
#endif
#endif

#ifdef HAVE_ZSTD
#ಅನ್ತರ್ಭಾವ <zstd.h>
#endif

#ifdef USE_GSASL
#ಅನ್ತರ್ಭಾವ <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#ಅನ್ತರ್ಭಾವ "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
ಸ್ಥಿರ ಶೂನ್ಯ brotli_version(ವರ್ಣ *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned ಪೂರ್ಣಾಙ್ಕ major = brotli_version >> 24;
  unsigned ಪೂರ್ಣಾಙ್ಕ minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned ಪೂರ್ಣಾಙ್ಕ patch = brotli_version & 0x00000FFF;
  (ಶೂನ್ಯ)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
ಸ್ಥಿರ ಶೂನ್ಯ zstd_version(ವರ್ಣ *buf, size_t bufsz)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ version = ZSTD_version_ಸಂಖ್ಯೆ();
  unsigned ಪೂರ್ಣಾಙ್ಕ major = version / (100 * 100);
  unsigned ಪೂರ್ಣಾಙ್ಕ minor = (version - (major * 100 * 100)) / 100;
  unsigned ಪೂರ್ಣಾಙ್ಕ patch = version - (major * 100 * 100) - (minor * 100);
  (ಶೂನ್ಯ)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
ಸ್ಥಿರ ಶೂನ್ಯ psl_version(ವರ್ಣ *buf, size_t bufsz)
{
#ಯದಿ defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ = psl_check_version_ಸಂಖ್ಯೆ(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 ಅಂಕೆ >> 16, (ಅಂಕೆ >> 8) & 0xff, ಅಂಕೆ & 0xff);
#ಅನ್ಯಥಾ
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#ಯದಿ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
ಸ್ಥಿರ ಶೂನ್ಯ idn_version(ವರ್ಣ *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(ರಿಕ್ತ));
#ನೋಚೇದ್ಯದಿ defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#ನೋಚೇದ್ಯದಿ defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#endif
}
#endif

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#define VERSION_PARTS 16 /* number of substrings we can concatenate */

ವರ್ಣ *curl_version(ಶೂನ್ಯ)
{
  ಸ್ಥಿರ ವರ್ಣ out[300];
  ವರ್ಣ *outp;
  size_t outlen;
  ಧ್ರುವ ವರ್ಣ *src[VERSION_PARTS];
#ifdef USE_SSL
  ವರ್ಣ ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  ವರ್ಣ z_version[30];
#endif
#ifdef HAVE_BROTLI
  ವರ್ಣ br_version[30];
#endif
#ifdef HAVE_ZSTD
  ವರ್ಣ zstd_ver[30];
#endif
#ifdef USE_ARES
  ವರ್ಣ cares_version[30];
#endif
#ifdef USE_IDN
  ವರ್ಣ idn_ver[30];
#endif
#ifdef USE_LIBPSL
  ವರ್ಣ psl_ver[30];
#endif
#ifdef USE_SSH
  ವರ್ಣ ssh_version[30];
#endif
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  ವರ್ಣ h2_version[30];
#endif
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  ವರ್ಣ h3_version[30];
#endif
#ifdef USE_GSASL
  ವರ್ಣ gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  ವರ್ಣ gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  ವರ್ಣ ldap_buf[30];
#endif
  ಪೂರ್ಣಾಙ್ಕ i = 0;
  ಪೂರ್ಣಾಙ್ಕ j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  ಧ್ರುವ ವರ್ಣ *debugversion = getenv("CURL_DEBUG_VERSION");
  ಯದಿ(debugversion) {
    curl_msnprintf(out, ಪರಿಮಾಣ(out), "%s", debugversion);
    ಪ್ರತ್ಯಾಗಮ out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, ಪರಿಮಾಣ(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, ಪರಿಮಾಣ(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, ಪರಿಮಾಣ(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, ಪರಿಮಾಣ(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, ಪರಿಮಾಣ(cares_version),
                 "c-ares/%s", ares_version(ರಿಕ್ತ));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, ಪರಿಮಾಣ(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, ಪರಿಮಾಣ(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, ಪರಿಮಾಣ(ssh_version));
  src[i++] = ssh_version;
#endif
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, ಪರಿಮಾಣ(h2_version));
  src[i++] = h2_version;
#endif
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, ಪರಿಮಾಣ(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, ಪರಿಮಾಣ(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(ರಿಕ್ತ));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, ಪರಿಮಾಣ(gss_buf), "AppleGSS");
#ನೋಚೇದ್ಯದಿ defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, ಪರಿಮಾಣ(gss_buf), "libgss/%s", GSS_VERSION);
#ನೋಚೇದ್ಯದಿ defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, ಪರಿಮಾಣ(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#ಅನ್ಯಥಾ
  curl_msnprintf(gss_buf, ಪರಿಮಾಣ(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, ಪರಿಮಾಣ(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = ಪರಿಮಾಣ(out);
  ಪ್ರತಿ(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    ಯದಿ(outlen <= (n + 2))
      ಭಙ್ಗ;
    ಯದಿ(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  ಪ್ರತ್ಯಾಗಮ out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#ಯದಿ !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#endif
#ifndef CURL_DISABLE_RTSP
  "rtsp",
#endif
#ifdef USE_SSH
  "scp",
  "sftp",
#endif
#ifndef CURL_DISABLE_SMTP
  "smtp",
#endif
#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#endif
#ifndef CURL_DISABLE_TELNET
  "telnet",
#endif
#ifndef CURL_DISABLE_TFTP
  "tftp",
#endif
#ifndef CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#ifndef CURL_DISABLE_WEBSOCKETS
  "ws",
#endif
#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  ರಿಕ್ತ
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#ಯದಿ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ idn_present(curl_version_info_data *info)
{
#ಯದಿ defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (ಶೂನ್ಯ)info;
  ಪ್ರತ್ಯಾಗಮ TRUE;
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ !!info->libidn;
#endif
}
#endif

#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ https_proxy_present(curl_version_info_data *info)
{
  (ಶೂನ್ಯ)info;
  ಪ್ರತ್ಯಾಗಮ Curl_ssl_supports(ರಿಕ್ತ, SSLSUPP_HTTPS_PROXY);
}
#endif

#ಯದಿ defined(USE_SSL) && defined(USE_ECH)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ech_present(curl_version_info_data *info)
{
  (ಶೂನ್ಯ)info;
  ಪ್ರತ್ಯಾಗಮ Curl_ssl_supports(ರಿಕ್ತ, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

ಸಂರಚನಾ feat {
  ಧ್ರುವ ವರ್ಣ *name;
  ಪೂರ್ಣಾಙ್ಕ        (*present)(curl_version_info_data *info);
  ಪೂರ್ಣಾಙ್ಕ        bitmask;
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     ರಿಕ್ತ,                CURL_VERSION_ALTSVC),
#endif
#ಯದಿ defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     ರಿಕ್ತ,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   ರಿಕ್ತ,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      ರಿಕ್ತ,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       ರಿಕ್ತ,                CURL_VERSION_DEBUG),
#endif
#ಯದಿ defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       ರಿಕ್ತ,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     ರಿಕ್ತ,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        ರಿಕ್ತ,                CURL_VERSION_HSTS),
#endif
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       ರಿಕ್ತ,                CURL_VERSION_HTTP2),
#endif
#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       ರಿಕ್ತ,                CURL_VERSION_HTTP3),
#endif
#ಯದಿ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     ರಿಕ್ತ,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     ರಿಕ್ತ,                0),
#endif
#ಯದಿ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        ರಿಕ್ತ,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    ರಿಕ್ತ,                CURL_VERSION_KERBEROS5),
#endif
#ಯದಿ (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   ರಿಕ್ತ,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        ರಿಕ್ತ,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    ರಿಕ್ತ,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        ರಿಕ್ತ,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", ರಿಕ್ತ,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         ರಿಕ್ತ,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", ರಿಕ್ತ,              0),
#ನೋಚೇದ್ಯದಿ defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    ರಿಕ್ತ,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      ರಿಕ್ತ,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         ರಿಕ್ತ,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", ರಿಕ್ತ,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        ರಿಕ್ತ,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  ರಿಕ್ತ,                CURL_VERSION_THREADSAFE),
#endif
#ಯದಿ defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     ರಿಕ್ತ,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", ರಿಕ್ತ,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        ರಿಕ್ತ,                CURL_VERSION_ZSTD),
#endif
  {ರಿಕ್ತ,                 ರಿಕ್ತ,                0}
};

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *feature_names[CURL_ARRAYSIZE(features_table)] = { ರಿಕ್ತ };

ಸ್ಥಿರ curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_ಅಂಕೆ,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  ರಿಕ್ತ, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  ರಿಕ್ತ, /* zlib_version */
  supported_protocols,
  ರಿಕ್ತ, /* c-ares version */
  0,    /* c-ares version numerical */
  ರಿಕ್ತ, /* libidn version */
  0,    /* iconv version */
  ರಿಕ್ತ, /* ssh lib version */
  0,    /* brotli_ver_num */
  ರಿಕ್ತ, /* brotli version */
  0,    /* nghttp2 version number */
  ರಿಕ್ತ, /* nghttp2 version string */
  ರಿಕ್ತ, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#ಅನ್ಯಥಾ
  ರಿಕ್ತ,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#ಅನ್ಯಥಾ
  ರಿಕ್ತ,
#endif
  0,    /* zstd_ver_num */
  ರಿಕ್ತ, /* zstd version */
  ರಿಕ್ತ, /* Hyper version */
  ರಿಕ್ತ, /* gsasl version */
  feature_names,
  ರಿಕ್ತ  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  ಧ್ರುವ ಸಂರಚನಾ feat *p;
  ಪೂರ್ಣಾಙ್ಕ features = 0;

#ifdef USE_SSH
  ಸ್ಥಿರ ವರ್ಣ ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  ಸ್ಥಿರ ವರ್ಣ ssl_buffer[200];
#ಅನ್ಯಥಾ
  ಸ್ಥಿರ ವರ್ಣ ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  ಸ್ಥಿರ ವರ್ಣ brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  ಸ್ಥಿರ ವರ್ಣ zstd_buffer[80];
#endif

  (ಶೂನ್ಯ)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, ಪರಿಮಾಣ(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    ಪೂರ್ಣಾಙ್ಕ aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_ಅಂಕೆ = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, ಪರಿಮಾಣ(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_ಅಂಕೆ = BrotliDecoderVersion();
  brotli_version(brotli_buffer, ಪರಿಮಾಣ(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_ಅಂಕೆ = ZSTD_version_ಸಂಖ್ಯೆ();
  zstd_version(zstd_buffer, ಪರಿಮಾಣ(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_ಅಂಕೆ = (unsigned ಪೂರ್ಣಾಙ್ಕ)h2->version_ಅಂಕೆ;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#ಯದಿ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ಸ್ಥಿರ ವರ್ಣ quicbuffer[80];
    Curl_quic_ver(quicbuffer, ಪರಿಮಾಣ(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(ರಿಕ್ತ);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  ಪ್ರತಿ(p = features_table; p->name; p++)
    ಯದಿ(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = ರಿಕ್ತ;  /* Terminate array. */
  version_info.features = features;

  ಪ್ರತ್ಯಾಗಮ &version_info;
}
