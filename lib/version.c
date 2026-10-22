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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_NGHTTP2
#അന്തര്ഭാവ <nghttp2/nghttp2.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "http2.h"
#അന്തര്ഭാവ "vssh/ssh.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#അന്തര്ഭാവ <idn2.h>
#endif

#ifdef USE_LIBPSL
#അന്തര്ഭാവ <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#അന്തര്ഭാവ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Wvla"
#endif
#അന്തര്ഭാവ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic എടുക്കുക
#endif
#endif

#ifdef HAVE_ZSTD
#അന്തര്ഭാവ <zstd.h>
#endif

#ifdef USE_GSASL
#അന്തര്ഭാവ <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#അന്തര്ഭാവ "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
സ്ഥിതം ശൂന്യം brotli_version(അക്ഷരം *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned പൂർണ്ണസംഖ്യ major = brotli_version >> 24;
  unsigned പൂർണ്ണസംഖ്യ minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned പൂർണ്ണസംഖ്യ patch = brotli_version & 0x00000FFF;
  (ശൂന്യം)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
സ്ഥിതം ശൂന്യം zstd_version(അക്ഷരം *buf, size_t bufsz)
{
  unsigned പൂർണ്ണസംഖ്യ version = ZSTD_version_സംഖ്യ();
  unsigned പൂർണ്ണസംഖ്യ major = version / (100 * 100);
  unsigned പൂർണ്ണസംഖ്യ minor = (version - (major * 100 * 100)) / 100;
  unsigned പൂർണ്ണസംഖ്യ patch = version - (major * 100 * 100) - (minor * 100);
  (ശൂന്യം)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
സ്ഥിതം ശൂന്യം psl_version(അക്ഷരം *buf, size_t bufsz)
{
#എങ്കിൽ defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  പൂർണ്ണസംഖ്യ അക്കം = psl_check_version_സംഖ്യ(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 അക്കം >> 16, (അക്കം >> 8) & 0xff, അക്കം & 0xff);
#അല്ലെങ്കിൽ
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#എങ്കിൽ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
സ്ഥിതം ശൂന്യം idn_version(അക്ഷരം *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(ശൂന്യ));
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#അല്ലെങ്കിലെങ്കിൽ defined(USE_APPLE_IDN)
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

അക്ഷരം *curl_version(ശൂന്യം)
{
  സ്ഥിതം അക്ഷരം out[300];
  അക്ഷരം *outp;
  size_t outlen;
  സ്ഥിരം അക്ഷരം *src[VERSION_PARTS];
#ifdef USE_SSL
  അക്ഷരം ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  അക്ഷരം z_version[30];
#endif
#ifdef HAVE_BROTLI
  അക്ഷരം br_version[30];
#endif
#ifdef HAVE_ZSTD
  അക്ഷരം zstd_ver[30];
#endif
#ifdef USE_ARES
  അക്ഷരം cares_version[30];
#endif
#ifdef USE_IDN
  അക്ഷരം idn_ver[30];
#endif
#ifdef USE_LIBPSL
  അക്ഷരം psl_ver[30];
#endif
#ifdef USE_SSH
  അക്ഷരം ssh_version[30];
#endif
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  അക്ഷരം h2_version[30];
#endif
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  അക്ഷരം h3_version[30];
#endif
#ifdef USE_GSASL
  അക്ഷരം gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  അക്ഷരം gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  അക്ഷരം ldap_buf[30];
#endif
  പൂർണ്ണസംഖ്യ i = 0;
  പൂർണ്ണസംഖ്യ j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  സ്ഥിരം അക്ഷരം *debugversion = getenv("CURL_DEBUG_VERSION");
  എങ്കിൽ(debugversion) {
    curl_msnprintf(out, വലുപ്പം(out), "%s", debugversion);
    തിരികെ out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, വലുപ്പം(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, വലുപ്പം(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, വലുപ്പം(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, വലുപ്പം(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, വലുപ്പം(cares_version),
                 "c-ares/%s", ares_version(ശൂന്യ));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, വലുപ്പം(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, വലുപ്പം(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, വലുപ്പം(ssh_version));
  src[i++] = ssh_version;
#endif
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, വലുപ്പം(h2_version));
  src[i++] = h2_version;
#endif
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, വലുപ്പം(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, വലുപ്പം(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(ശൂന്യ));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, വലുപ്പം(gss_buf), "AppleGSS");
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, വലുപ്പം(gss_buf), "libgss/%s", GSS_VERSION);
#അല്ലെങ്കിലെങ്കിൽ defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, വലുപ്പം(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#അല്ലെങ്കിൽ
  curl_msnprintf(gss_buf, വലുപ്പം(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, വലുപ്പം(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = വലുപ്പം(out);
  വേണ്ടി(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    എങ്കിൽ(outlen <= (n + 2))
      നിർത്തുക;
    എങ്കിൽ(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  തിരികെ out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#എങ്കിൽ !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  ശൂന്യ
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#എങ്കിൽ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
സ്ഥിതം പൂർണ്ണസംഖ്യ idn_present(curl_version_info_data *info)
{
#എങ്കിൽ defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (ശൂന്യം)info;
  തിരികെ TRUE;
#അല്ലെങ്കിൽ
  തിരികെ !!info->libidn;
#endif
}
#endif

#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
സ്ഥിതം പൂർണ്ണസംഖ്യ https_proxy_present(curl_version_info_data *info)
{
  (ശൂന്യം)info;
  തിരികെ Curl_ssl_supports(ശൂന്യ, SSLSUPP_HTTPS_PROXY);
}
#endif

#എങ്കിൽ defined(USE_SSL) && defined(USE_ECH)
സ്ഥിതം പൂർണ്ണസംഖ്യ ech_present(curl_version_info_data *info)
{
  (ശൂന്യം)info;
  തിരികെ Curl_ssl_supports(ശൂന്യ, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

ഘടന feat {
  സ്ഥിരം അക്ഷരം *name;
  പൂർണ്ണസംഖ്യ        (*present)(curl_version_info_data *info);
  പൂർണ്ണസംഖ്യ        bitmask;
};

സ്ഥിതം സ്ഥിരം ഘടന feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     ശൂന്യ,                CURL_VERSION_ALTSVC),
#endif
#എങ്കിൽ defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     ശൂന്യ,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   ശൂന്യ,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      ശൂന്യ,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       ശൂന്യ,                CURL_VERSION_DEBUG),
#endif
#എങ്കിൽ defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       ശൂന്യ,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     ശൂന്യ,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        ശൂന്യ,                CURL_VERSION_HSTS),
#endif
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       ശൂന്യ,                CURL_VERSION_HTTP2),
#endif
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       ശൂന്യ,                CURL_VERSION_HTTP3),
#endif
#എങ്കിൽ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     ശൂന്യ,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     ശൂന്യ,                0),
#endif
#എങ്കിൽ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        ശൂന്യ,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    ശൂന്യ,                CURL_VERSION_KERBEROS5),
#endif
#എങ്കിൽ (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   ശൂന്യ,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        ശൂന്യ,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    ശൂന്യ,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        ശൂന്യ,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", ശൂന്യ,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         ശൂന്യ,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", ശൂന്യ,              0),
#അല്ലെങ്കിലെങ്കിൽ defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    ശൂന്യ,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      ശൂന്യ,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         ശൂന്യ,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", ശൂന്യ,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        ശൂന്യ,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  ശൂന്യ,                CURL_VERSION_THREADSAFE),
#endif
#എങ്കിൽ defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     ശൂന്യ,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", ശൂന്യ,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        ശൂന്യ,                CURL_VERSION_ZSTD),
#endif
  {ശൂന്യ,                 ശൂന്യ,                0}
};

സ്ഥിതം സ്ഥിരം അക്ഷരം *feature_names[CURL_ARRAYSIZE(features_table)] = { ശൂന്യ };

സ്ഥിതം curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_അക്കം,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  ശൂന്യ, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  ശൂന്യ, /* zlib_version */
  supported_protocols,
  ശൂന്യ, /* c-ares version */
  0,    /* c-ares version numerical */
  ശൂന്യ, /* libidn version */
  0,    /* iconv version */
  ശൂന്യ, /* ssh lib version */
  0,    /* brotli_ver_num */
  ശൂന്യ, /* brotli version */
  0,    /* nghttp2 version number */
  ശൂന്യ, /* nghttp2 version string */
  ശൂന്യ, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#അല്ലെങ്കിൽ
  ശൂന്യ,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#അല്ലെങ്കിൽ
  ശൂന്യ,
#endif
  0,    /* zstd_ver_num */
  ശൂന്യ, /* zstd version */
  ശൂന്യ, /* Hyper version */
  ശൂന്യ, /* gsasl version */
  feature_names,
  ശൂന്യ  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  സ്ഥിരം ഘടന feat *p;
  പൂർണ്ണസംഖ്യ features = 0;

#ifdef USE_SSH
  സ്ഥിതം അക്ഷരം ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  സ്ഥിതം അക്ഷരം ssl_buffer[200];
#അല്ലെങ്കിൽ
  സ്ഥിതം അക്ഷരം ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  സ്ഥിതം അക്ഷരം brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  സ്ഥിതം അക്ഷരം zstd_buffer[80];
#endif

  (ശൂന്യം)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, വലുപ്പം(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    പൂർണ്ണസംഖ്യ aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_അക്കം = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, വലുപ്പം(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_അക്കം = BrotliDecoderVersion();
  brotli_version(brotli_buffer, വലുപ്പം(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_അക്കം = ZSTD_version_സംഖ്യ();
  zstd_version(zstd_buffer, വലുപ്പം(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_അക്കം = (unsigned പൂർണ്ണസംഖ്യ)h2->version_അക്കം;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    സ്ഥിതം അക്ഷരം quicbuffer[80];
    Curl_quic_ver(quicbuffer, വലുപ്പം(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(ശൂന്യ);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  വേണ്ടി(p = features_table; p->name; p++)
    എങ്കിൽ(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = ശൂന്യ;  /* Terminate array. */
  version_info.features = features;

  തിരികെ &version_info;
}
