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
#ضمن "curl_setup.h"

#ifdef USE_NGHTTP2
#ضمن <nghttp2/nghttp2.h>
#endif

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "http2.h"
#ضمن "vssh/ssh.h"
#ضمن "vquic/vquic.h"
#ضمن "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#ضمن <idn2.h>
#endif

#ifdef USE_LIBPSL
#ضمن <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#ضمن <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ڈالو
#pragma GCC diagnostic ignored "-Wvla"
#endif
#ضمن <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic نکالو
#endif
#endif

#ifdef HAVE_ZSTD
#ضمن <zstd.h>
#endif

#ifdef USE_GSASL
#ضمن <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#ضمن "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
ساكن فراغ brotli_version(حرف *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned صحيح major = brotli_version >> 24;
  unsigned صحيح minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned صحيح patch = brotli_version & 0x00000FFF;
  (فراغ)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
ساكن فراغ zstd_version(حرف *buf, size_t bufsz)
{
  unsigned صحيح version = ZSTD_version_نمبر();
  unsigned صحيح major = version / (100 * 100);
  unsigned صحيح minor = (version - (major * 100 * 100)) / 100;
  unsigned صحيح patch = version - (major * 100 * 100) - (minor * 100);
  (فراغ)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
ساكن فراغ psl_version(حرف *buf, size_t bufsz)
{
#إذا defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  صحيح عدد = psl_check_version_نمبر(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 عدد >> 16, (عدد >> 8) & 0xff, عدد & 0xff);
#وإلا
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
ساكن فراغ idn_version(حرف *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(عدم));
#وإلاإذا defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#وإلاإذا defined(USE_APPLE_IDN)
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

حرف *curl_version(فراغ)
{
  ساكن حرف out[300];
  حرف *outp;
  size_t outlen;
  ثابت حرف *src[VERSION_PARTS];
#ifdef USE_SSL
  حرف ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  حرف z_version[30];
#endif
#ifdef HAVE_BROTLI
  حرف br_version[30];
#endif
#ifdef HAVE_ZSTD
  حرف zstd_ver[30];
#endif
#ifdef USE_ARES
  حرف cares_version[30];
#endif
#ifdef USE_IDN
  حرف idn_ver[30];
#endif
#ifdef USE_LIBPSL
  حرف psl_ver[30];
#endif
#ifdef USE_SSH
  حرف ssh_version[30];
#endif
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  حرف h2_version[30];
#endif
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  حرف h3_version[30];
#endif
#ifdef USE_GSASL
  حرف gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  حرف gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  حرف ldap_buf[30];
#endif
  صحيح i = 0;
  صحيح j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  ثابت حرف *debugversion = getenv("CURL_DEBUG_VERSION");
  إذا(debugversion) {
    curl_msnprintf(out, حجم(out), "%s", debugversion);
    أرجع out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, حجم(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, حجم(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, حجم(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, حجم(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, حجم(cares_version),
                 "c-ares/%s", ares_version(عدم));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, حجم(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, حجم(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, حجم(ssh_version));
  src[i++] = ssh_version;
#endif
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, حجم(h2_version));
  src[i++] = h2_version;
#endif
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, حجم(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, حجم(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(عدم));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, حجم(gss_buf), "AppleGSS");
#وإلاإذا defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, حجم(gss_buf), "libgss/%s", GSS_VERSION);
#وإلاإذا defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#وإلا
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, حجم(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = حجم(out);
  لكل(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    إذا(outlen <= (n + 2))
      اكسر;
    إذا(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  أرجع out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ساكن ثابت حرف * ثابت supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#إذا !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  عدم
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ساكن صحيح idn_present(curl_version_info_data *info)
{
#إذا defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (فراغ)info;
  أرجع TRUE;
#وإلا
  أرجع !!info->libidn;
#endif
}
#endif

#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ساكن صحيح https_proxy_present(curl_version_info_data *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_HTTPS_PROXY);
}
#endif

#إذا defined(USE_SSL) && defined(USE_ECH)
ساكن صحيح ech_present(curl_version_info_data *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

بنية feat {
  ثابت حرف *name;
  صحيح        (*present)(curl_version_info_data *info);
  صحيح        bitmask;
};

ساكن ثابت بنية feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     عدم,                CURL_VERSION_ALTSVC),
#endif
#إذا defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     عدم,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   عدم,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      عدم,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       عدم,                CURL_VERSION_DEBUG),
#endif
#إذا defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       عدم,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     عدم,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        عدم,                CURL_VERSION_HSTS),
#endif
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       عدم,                CURL_VERSION_HTTP2),
#endif
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       عدم,                CURL_VERSION_HTTP3),
#endif
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     عدم,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     عدم,                0),
#endif
#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        عدم,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    عدم,                CURL_VERSION_KERBEROS5),
#endif
#إذا (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   عدم,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        عدم,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    عدم,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        عدم,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", عدم,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         عدم,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", عدم,              0),
#وإلاإذا defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    عدم,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      عدم,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         عدم,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", عدم,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        عدم,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  عدم,                CURL_VERSION_THREADSAFE),
#endif
#إذا defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     عدم,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", عدم,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        عدم,                CURL_VERSION_ZSTD),
#endif
  {عدم,                 عدم,                0}
};

ساكن ثابت حرف *feature_names[CURL_ARRAYSIZE(features_table)] = { عدم };

ساكن curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_عدد,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  عدم, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  عدم, /* zlib_version */
  supported_protocols,
  عدم, /* c-ares version */
  0,    /* c-ares version numerical */
  عدم, /* libidn version */
  0,    /* iconv version */
  عدم, /* ssh lib version */
  0,    /* brotli_ver_num */
  عدم, /* brotli version */
  0,    /* nghttp2 version number */
  عدم, /* nghttp2 version string */
  عدم, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#وإلا
  عدم,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#وإلا
  عدم,
#endif
  0,    /* zstd_ver_num */
  عدم, /* zstd version */
  عدم, /* Hyper version */
  عدم, /* gsasl version */
  feature_names,
  عدم  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  ثابت بنية feat *p;
  صحيح features = 0;

#ifdef USE_SSH
  ساكن حرف ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  ساكن حرف ssl_buffer[200];
#وإلا
  ساكن حرف ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  ساكن حرف brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  ساكن حرف zstd_buffer[80];
#endif

  (فراغ)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, حجم(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    صحيح aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_عدد = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, حجم(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_عدد = BrotliDecoderVersion();
  brotli_version(brotli_buffer, حجم(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_عدد = ZSTD_version_نمبر();
  zstd_version(zstd_buffer, حجم(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_عدد = (unsigned صحيح)h2->version_عدد;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ساكن حرف quicbuffer[80];
    Curl_quic_ver(quicbuffer, حجم(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(عدم);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  لكل(p = features_table; p->name; p++)
    إذا(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = عدم;  /* Terminate array. */
  version_info.features = features;

  أرجع &version_info;
}
