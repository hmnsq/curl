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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_NGHTTP2
#অন্তর্ভাৱ <nghttp2/nghttp2.h>
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "http2.h"
#অন্তর্ভাৱ "vssh/ssh.h"
#অন্তর্ভাৱ "vquic/vquic.h"
#অন্তর্ভাৱ "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#অন্তর্ভাৱ <idn2.h>
#endif

#ifdef USE_LIBPSL
#অন্তর্ভাৱ <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#অন্তর্ভাৱ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wvla"
#endif
#অন্তর্ভাৱ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বেৰ_কৰো
#endif
#endif

#ifdef HAVE_ZSTD
#অন্তর্ভাৱ <zstd.h>
#endif

#ifdef USE_GSASL
#অন্তর্ভাৱ <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#অন্তর্ভাৱ "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
স্থির শূন্য brotli_version(ৱর্ণ *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned পূর্ণাঙ্ক major = brotli_version >> 24;
  unsigned পূর্ণাঙ্ক minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned পূর্ণাঙ্ক patch = brotli_version & 0x00000FFF;
  (শূন্য)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
স্থির শূন্য zstd_version(ৱর্ণ *buf, size_t bufsz)
{
  unsigned পূর্ণাঙ্ক version = ZSTD_version_সংখ্যা();
  unsigned পূর্ণাঙ্ক major = version / (100 * 100);
  unsigned পূর্ণাঙ্ক minor = (version - (major * 100 * 100)) / 100;
  unsigned পূর্ণাঙ্ক patch = version - (major * 100 * 100) - (minor * 100);
  (শূন্য)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
স্থির শূন্য psl_version(ৱর্ণ *buf, size_t bufsz)
{
#যদি defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  পূর্ণাঙ্ক অঙ্ক = psl_check_version_সংখ্যা(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 অঙ্ক >> 16, (অঙ্ক >> 8) & 0xff, অঙ্ক & 0xff);
#অন্যথা
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#যদি defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
স্থির শূন্য idn_version(ৱর্ণ *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(রিক্ত));
#নোচেদ্যদি defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#নোচেদ্যদি defined(USE_APPLE_IDN)
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

ৱর্ণ *curl_version(শূন্য)
{
  স্থির ৱর্ণ out[300];
  ৱর্ণ *outp;
  size_t outlen;
  ধ্রুৱ ৱর্ণ *src[VERSION_PARTS];
#ifdef USE_SSL
  ৱর্ণ ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  ৱর্ণ z_version[30];
#endif
#ifdef HAVE_BROTLI
  ৱর্ণ br_version[30];
#endif
#ifdef HAVE_ZSTD
  ৱর্ণ zstd_ver[30];
#endif
#ifdef USE_ARES
  ৱর্ণ cares_version[30];
#endif
#ifdef USE_IDN
  ৱর্ণ idn_ver[30];
#endif
#ifdef USE_LIBPSL
  ৱর্ণ psl_ver[30];
#endif
#ifdef USE_SSH
  ৱর্ণ ssh_version[30];
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  ৱর্ণ h2_version[30];
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  ৱর্ণ h3_version[30];
#endif
#ifdef USE_GSASL
  ৱর্ণ gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  ৱর্ণ gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  ৱর্ণ ldap_buf[30];
#endif
  পূর্ণাঙ্ক i = 0;
  পূর্ণাঙ্ক j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  ধ্রুৱ ৱর্ণ *debugversion = getenv("CURL_DEBUG_VERSION");
  যদি(debugversion) {
    curl_msnprintf(out, পরিমাণ(out), "%s", debugversion);
    ঘূৰাই out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, পরিমাণ(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, পরিমাণ(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, পরিমাণ(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, পরিমাণ(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, পরিমাণ(cares_version),
                 "c-ares/%s", ares_version(রিক্ত));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, পরিমাণ(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, পরিমাণ(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, পরিমাণ(ssh_version));
  src[i++] = ssh_version;
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, পরিমাণ(h2_version));
  src[i++] = h2_version;
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, পরিমাণ(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, পরিমাণ(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(রিক্ত));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, পরিমাণ(gss_buf), "AppleGSS");
#নোচেদ্যদি defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, পরিমাণ(gss_buf), "libgss/%s", GSS_VERSION);
#নোচেদ্যদি defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, পরিমাণ(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#অন্যথা
  curl_msnprintf(gss_buf, পরিমাণ(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, পরিমাণ(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = পরিমাণ(out);
  বাবে(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    যদি(outlen <= (n + 2))
      ভঙ্গ;
    যদি(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  ঘূৰাই out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#যদি !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  রিক্ত
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#যদি defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
স্থির পূর্ণাঙ্ক idn_present(curl_version_info_data *info)
{
#যদি defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (শূন্য)info;
  ঘূৰাই TRUE;
#অন্যথা
  ঘূৰাই !!info->libidn;
#endif
}
#endif

#যদি defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
স্থির পূর্ণাঙ্ক https_proxy_present(curl_version_info_data *info)
{
  (শূন্য)info;
  ঘূৰাই Curl_ssl_supports(রিক্ত, SSLSUPP_HTTPS_PROXY);
}
#endif

#যদি defined(USE_SSL) && defined(USE_ECH)
স্থির পূর্ণাঙ্ক ech_present(curl_version_info_data *info)
{
  (শূন্য)info;
  ঘূৰাই Curl_ssl_supports(রিক্ত, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

সংরচনা feat {
  ধ্রুৱ ৱর্ণ *name;
  পূর্ণাঙ্ক        (*present)(curl_version_info_data *info);
  পূর্ণাঙ্ক        bitmask;
};

স্থির ধ্রুৱ সংরচনা feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     রিক্ত,                CURL_VERSION_ALTSVC),
#endif
#যদি defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     রিক্ত,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   রিক্ত,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      রিক্ত,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       রিক্ত,                CURL_VERSION_DEBUG),
#endif
#যদি defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       রিক্ত,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     রিক্ত,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        রিক্ত,                CURL_VERSION_HSTS),
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       রিক্ত,                CURL_VERSION_HTTP2),
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       রিক্ত,                CURL_VERSION_HTTP3),
#endif
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     রিক্ত,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     রিক্ত,                0),
#endif
#যদি defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        রিক্ত,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    রিক্ত,                CURL_VERSION_KERBEROS5),
#endif
#যদি (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   রিক্ত,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        রিক্ত,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    রিক্ত,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        রিক্ত,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", রিক্ত,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         রিক্ত,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", রিক্ত,              0),
#নোচেদ্যদি defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    রিক্ত,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      রিক্ত,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         রিক্ত,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", রিক্ত,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        রিক্ত,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  রিক্ত,                CURL_VERSION_THREADSAFE),
#endif
#যদি defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     রিক্ত,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", রিক্ত,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        রিক্ত,                CURL_VERSION_ZSTD),
#endif
  {রিক্ত,                 রিক্ত,                0}
};

স্থির ধ্রুৱ ৱর্ণ *feature_names[CURL_ARRAYSIZE(features_table)] = { রিক্ত };

স্থির curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_অঙ্ক,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  রিক্ত, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  রিক্ত, /* zlib_version */
  supported_protocols,
  রিক্ত, /* c-ares version */
  0,    /* c-ares version numerical */
  রিক্ত, /* libidn version */
  0,    /* iconv version */
  রিক্ত, /* ssh lib version */
  0,    /* brotli_ver_num */
  রিক্ত, /* brotli version */
  0,    /* nghttp2 version number */
  রিক্ত, /* nghttp2 version string */
  রিক্ত, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#অন্যথা
  রিক্ত,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#অন্যথা
  রিক্ত,
#endif
  0,    /* zstd_ver_num */
  রিক্ত, /* zstd version */
  রিক্ত, /* Hyper version */
  রিক্ত, /* gsasl version */
  feature_names,
  রিক্ত  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  ধ্রুৱ সংরচনা feat *p;
  পূর্ণাঙ্ক features = 0;

#ifdef USE_SSH
  স্থির ৱর্ণ ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  স্থির ৱর্ণ ssl_buffer[200];
#অন্যথা
  স্থির ৱর্ণ ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  স্থির ৱর্ণ brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  স্থির ৱর্ণ zstd_buffer[80];
#endif

  (শূন্য)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, পরিমাণ(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    পূর্ণাঙ্ক aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_অঙ্ক = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, পরিমাণ(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_অঙ্ক = BrotliDecoderVersion();
  brotli_version(brotli_buffer, পরিমাণ(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_অঙ্ক = ZSTD_version_সংখ্যা();
  zstd_version(zstd_buffer, পরিমাণ(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_অঙ্ক = (unsigned পূর্ণাঙ্ক)h2->version_অঙ্ক;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    স্থির ৱর্ণ quicbuffer[80];
    Curl_quic_ver(quicbuffer, পরিমাণ(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(রিক্ত);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  বাবে(p = features_table; p->name; p++)
    যদি(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = রিক্ত;  /* Terminate array. */
  version_info.features = features;

  ঘূৰাই &version_info;
}
