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

#که_تعریف_شوی USE_NGHTTP2
#ضمن <nghttp2/nghttp2.h>
#د_که_پای

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "http2.h"
#ضمن "vssh/ssh.h"
#ضمن "vquic/vquic.h"
#ضمن "easy_lock.h"

#که_تعریف_شوی USE_ARES
#  include <ares.h>
#د_که_پای

#که_تعریف_شوی USE_LIBIDN2
#ضمن <idn2.h>
#د_که_پای

#که_تعریف_شوی USE_LIBPSL
#ضمن <libpsl.h>
#د_که_پای

#که_تعریف_شوی HAVE_LIBZ
#ضمن <zlib.h>
#د_که_پای

#که_تعریف_شوی HAVE_BROTLI
#که_تعریف_شوی CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Wvla"
#د_که_پای
#ضمن <brotli/decode.h>
#که_تعریف_شوی CURL_HAVE_DIAG
#لارښوونه GCC diagnostic راوباسه
#د_که_پای
#د_که_پای

#که_تعریف_شوی HAVE_ZSTD
#ضمن <zstd.h>
#د_که_پای

#که_تعریف_شوی USE_GSASL
#ضمن <gsasl.h>
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_LDAP
#ضمن "curl_ldap.h"
#د_که_پای

#که_تعریف_شوی HAVE_BROTLI
ساكن فراغ brotli_نسخه(حرف *buf, size_t bufsz)
{
  uint32_t brotli_نسخه = Brotli_Decoder_نسخه();
  بینښه صحيح major = brotli_نسخه >> 24;
  بینښه صحيح minor = (brotli_نسخه & 0x00FFFFFF) >> 12;
  بینښه صحيح patch = brotli_نسخه & 0x00000FFF;
  (فراغ)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#د_که_پای

#که_تعریف_شوی HAVE_ZSTD
ساكن فراغ zstd_نسخه(حرف *buf, size_t bufsz)
{
  بینښه صحيح نسخه_2 = ZSTD_نسخه_شمېره();
  بینښه صحيح major = نسخه_2 / (100 * 100);
  بینښه صحيح minor = (نسخه_2 - (major * 100 * 100)) / 100;
  بینښه صحيح patch = نسخه_2 - (major * 100 * 100) - (minor * 100);
  (فراغ)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#د_که_پای

#که_تعریف_شوی USE_LIBPSL
ساكن فراغ psl_نسخه(حرف *buf, size_t bufsz)
{
#إذا defined(PSL_نسخه_MAJOR) && (PSL_نسخه_MAJOR > 0 ||     \
                                   PSL_نسخه_MINOR >= 11)
  صحيح عدد = psl_check_نسخه_شمېره(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 عدد >> 16, (عدد >> 8) & 0xff, عدد & 0xff);
#وإلا
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_نسخه());
#د_که_پای
}
#د_که_پای

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#تعریف_کړه USE_IDN
#د_که_پای

#که_تعریف_شوی USE_IDN
ساكن فراغ idn_نسخه(حرف *buf, size_t bufsz)
{
#که_تعریف_شوی USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_نسخه(عدم));
#وإلاإذا defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#وإلاإذا defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#د_که_پای
}
#د_که_پای

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#تعریف_کړه نسخه_PARTS 16 /* number of substrings we can concatenate */

حرف *curl_نسخه(فراغ)
{
  ساكن حرف بهر[300];
  حرف *outp;
  size_t outlen;
  ثابت حرف *src[نسخه_PARTS];
#که_تعریف_شوی USE_SSL
  حرف ssl_نسخه[200];
#د_که_پای
#که_تعریف_شوی HAVE_LIBZ
  حرف z_نسخه[30];
#د_که_پای
#که_تعریف_شوی HAVE_BROTLI
  حرف br_نسخه[30];
#د_که_پای
#که_تعریف_شوی HAVE_ZSTD
  حرف zstd_ver[30];
#د_که_پای
#که_تعریف_شوی USE_ARES
  حرف cares_نسخه[30];
#د_که_پای
#که_تعریف_شوی USE_IDN
  حرف idn_ver[30];
#د_که_پای
#که_تعریف_شوی USE_LIBPSL
  حرف psl_ver[30];
#د_که_پای
#که_تعریف_شوی USE_SSH
  حرف ssh_نسخه[30];
#د_که_پای
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  حرف h2_نسخه[30];
#د_که_پای
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  حرف h3_نسخه[30];
#د_که_پای
#که_تعریف_شوی USE_GSASL
  حرف gsasl_buf[30];
#د_که_پای
#که_تعریف_شوی HAVE_GSSAPI
  حرف gss_buf[40];
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_LDAP
  حرف ldap_buf[30];
#د_که_پای
  صحيح i = 0;
  صحيح j;

#که_تعریف_شوی DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
سېټ*/
  ثابت حرف *debugversion = getenv("CURL_DEBUG_VERSION");
  إذا(debugversion) {
    curl_msnprintf(بهر, حجم(بهر), "%s", debugversion);
    أرجع بهر;
  }
#د_که_پای

  src[i++] = LIBCURL_NAME "/" LIBCURL_نسخه;
#که_تعریف_شوی USE_SSL
  Curl_ssl_نسخه(ssl_نسخه, حجم(ssl_نسخه));
  src[i++] = ssl_نسخه;
#د_که_پای
#که_تعریف_شوی HAVE_LIBZ
  curl_msnprintf(z_نسخه, حجم(z_نسخه), "zlib/%s", zlib_نسخه());
  src[i++] = z_نسخه;
#د_که_پای
#که_تعریف_شوی HAVE_BROTLI
  brotli_نسخه(br_نسخه, حجم(br_نسخه));
  src[i++] = br_نسخه;
#د_که_پای
#که_تعریف_شوی HAVE_ZSTD
  zstd_نسخه(zstd_ver, حجم(zstd_ver));
  src[i++] = zstd_ver;
#د_که_پای
#که_تعریف_شوی USE_ARES
  curl_msnprintf(cares_نسخه, حجم(cares_نسخه),
                 "c-ares/%s", ares_نسخه(عدم));
  src[i++] = cares_نسخه;
#د_که_پای
#که_تعریف_شوی USE_IDN
  idn_نسخه(idn_ver, حجم(idn_ver));
  src[i++] = idn_ver;
#د_که_پای
#که_تعریف_شوی USE_LIBPSL
  psl_نسخه(psl_ver, حجم(psl_ver));
  src[i++] = psl_ver;
#د_که_پای
#که_تعریف_شوی USE_SSH
  Curl_ssh_نسخه(ssh_نسخه, حجم(ssh_نسخه));
  src[i++] = ssh_نسخه;
#د_که_پای
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_نسخه, حجم(h2_نسخه));
  src[i++] = h2_نسخه;
#د_که_پای
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_نسخه, حجم(h3_نسخه));
  src[i++] = h3_نسخه;
#د_که_پای
#که_تعریف_شوی USE_GSASL
  curl_msnprintf(gsasl_buf, حجم(gsasl_buf), "libgsasl/%s",
                 gsasl_check_نسخه(عدم));
  src[i++] = gsasl_buf;
#د_که_پای
#که_تعریف_شوی HAVE_GSSAPI
#که_تعریف_شوی HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, حجم(gss_buf), "AppleGSS");
#وإلاإذا defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, حجم(gss_buf), "libgss/%s", GSS_نسخه);
#وإلاإذا defined(CURL_KRB5_نسخه)
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5/%s", CURL_KRB5_نسخه);
#وإلا
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5");
#د_که_پای
  src[i++] = gss_buf;
#د_که_پای /* HAVE_GSSAPI */
#که_نه_تعریف_شوی CURL_DISABLE_LDAP
  Curl_ldap_نسخه(ldap_buf, حجم(ldap_buf));
  src[i++] = ldap_buf;
#د_که_پای

  DEBUGASSERT(i <= نسخه_PARTS);

  outp = &بهر[0];
  outlen = حجم(بهر);
  لكل(j = 0; j < i; j++) {
    size_t n = مزي_اوږدوالی(src[j]);
    /* we need room for a space, the string and the final zero */
    إذا(outlen <= (n + 2))
      اكسر;
    إذا(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    حافظه_نقل(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  أرجع بهر;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ساكن ثابت حرف * ثابت supported_protocols[] = {
#که_نه_تعریف_شوی CURL_DISABLE_DICT
  "dict",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FILE
  "file",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_FTP
  "ftp",
#د_که_پای
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_GOPHER
  "gopher",
#د_که_پای
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  "http",
#د_که_پای
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_IMAP
  "imap",
#د_که_پای
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_LDAP
  "ldap",
#إذا !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#د_که_پای
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_MQTT
  "mqtt",
#د_که_پای
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_POP3
  "pop3",
#د_که_پای
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  "rtsp",
#د_که_پای
#که_تعریف_شوی USE_SSH
  "scp",
  "sftp",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_SMTP
  "smtp",
#د_که_پای
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_TELNET
  "telnet",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_TFTP
  "tftp",
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#که_نه_تعریف_شوی CURL_DISABLE_WEBSOCKETS
  "ws",
#د_که_پای
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#د_که_پای
#د_که_پای

  عدم
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ساكن صحيح idn_present(curl_نسخه_info_معلومات *info)
{
#إذا defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (فراغ)info;
  أرجع TRUE;
#وإلا
  أرجع !!info->libidn;
#د_که_پای
}
#د_که_پای

#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ساكن صحيح https_proxy_present(curl_نسخه_info_معلومات *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_HTTPS_PROXY);
}
#د_که_پای

#إذا defined(USE_SSL) && defined(USE_ECH)
ساكن صحيح ech_present(curl_نسخه_info_معلومات *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_ECH);
}
#د_که_پای

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#تعریف_کړه FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

بنية feat {
  ثابت حرف *name;
  صحيح        (*present)(curl_نسخه_info_معلومات *info);
  صحيح        bitmask;
};

ساكن ثابت بنية feat features_جدول[] = {
#که_نه_تعریف_شوی CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     عدم,                CURL_نسخه_ALTSVC),
#د_که_پای
#إذا defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     عدم,                0),
#د_که_پای
#که_تعریف_شوی CURLRES_ASYNCH
  FEATURE("AsynchDNS",   عدم,                CURL_نسخه_ASYNCHDNS),
#د_که_پای
#که_تعریف_شوی HAVE_BROTLI
  FEATURE("brotli",      عدم,                CURL_نسخه_BROTLI),
#د_که_پای
#که_تعریف_شوی DEBUGBUILD
  FEATURE("Debug",       عدم,                CURL_نسخه_DEBUG),
#د_که_پای
#إذا defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#که_نه_تعریف_شوی USE_HTTPSRR
#تېروتنه "ECH enabled but not HTTPSRR, must be a config error"
#د_که_پای
#د_که_پای
#که_تعریف_شوی USE_GSASL
  FEATURE("gsasl",       عدم,                CURL_نسخه_GSASL),
#د_که_پای
#که_تعریف_شوی HAVE_GSSAPI
  FEATURE("GSS-API",     عدم,                CURL_نسخه_GSSAPI),
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HSTS
  FEATURE("HSTS",        عدم,                CURL_نسخه_HSTS),
#د_که_پای
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       عدم,                CURL_نسخه_HTTP2),
#د_که_پای
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       عدم,                CURL_نسخه_HTTP3),
#د_که_پای
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_نسخه_HTTPS_PROXY),
#د_که_پای
#که_نه_تعریف_شوی CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     عدم,                0),
#د_که_پای
#که_تعریف_شوی USE_HTTPSRR
  FEATURE("HTTPSRR",     عدم,                0),
#د_که_پای
#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_نسخه_IDN),
#د_که_پای
#که_تعریف_شوی USE_IPV6
  FEATURE("IPv6",        عدم,                CURL_نسخه_IPV6),
#د_که_پای
#که_تعریف_شوی USE_KERBEROS5
  FEATURE("Kerberos",    عدم,                CURL_نسخه_KERBEROS5),
#د_که_پای
#إذا (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   عدم,                CURL_نسخه_LARGEFILE),
#د_که_پای
#که_تعریف_شوی HAVE_LIBZ
  FEATURE("libz",        عدم,                CURL_نسخه_LIBZ),
#د_که_پای
#که_تعریف_شوی CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    عدم,                CURL_نسخه_MULTI_SSL),
#د_که_پای
#که_تعریف_شوی USE_NTLM
  FEATURE("NTLM",        عدم,                CURL_نسخه_NTLM),
#د_که_پای
#که_تعریف_شوی USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", عدم,                0),
#د_که_پای
#که_تعریف_شوی USE_LIBPSL
  FEATURE("PSL",         عدم,                CURL_نسخه_PSL),
#د_که_پای
#که_تعریف_شوی USE_SSL
#که_تعریف_شوی USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", عدم,              0),
#وإلاإذا defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    عدم,              0),
#د_که_پای
#د_که_پای /* USE_SSL */
#که_تعریف_شوی USE_SPNEGO
  FEATURE("SPNEGO",      عدم,                CURL_نسخه_SPNEGO),
#د_که_پای
#که_تعریف_شوی USE_SSL
  FEATURE("SSL",         عدم,                CURL_نسخه_SSL),
#د_که_پای
#که_تعریف_شوی USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", عدم,                0),
#د_که_پای
#که_تعریف_شوی USE_WINDOWS_SSPI
  FEATURE("SSPI",        عدم,                CURL_نسخه_SSPI),
#د_که_پای
#که_تعریف_شوی GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  عدم,                CURL_نسخه_THREADSAFE),
#د_که_پای
#إذا defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     عدم,                CURL_نسخه_UNICODE),
#د_که_پای
#که_تعریف_شوی USE_UNIX_SOCKETS
  FEATURE("UnixSockets", عدم,                CURL_نسخه_UNIX_SOCKETS),
#د_که_پای
#که_تعریف_شوی HAVE_ZSTD
  FEATURE("zstd",        عدم,                CURL_نسخه_ZSTD),
#د_که_پای
  {عدم,                 عدم,                0}
};

ساكن ثابت حرف *feature_نومونه[CURL_ARRAYSIZE(features_جدول)] = { عدم };

ساكن curl_نسخه_info_معلومات نسخه_info = {
  CURLVERSION_NOW,
  LIBCURL_نسخه,
  LIBCURL_نسخه_عدد,
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
#که_تعریف_شوی CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#وإلا
  عدم,
#د_که_پای
#که_تعریف_شوی CURL_CA_لار
  CURL_CA_لار,  /* capath */
#وإلا
  عدم,
#د_که_پای
  0,    /* zstd_ver_num */
  عدم, /* zstd version */
  عدم, /* Hyper version */
  عدم, /* gsasl version */
  feature_نومونه,
  عدم  /* rtmp version */
};

curl_نسخه_info_معلومات *curl_نسخه_info(CURLversion stamp)
{
  size_t n;
  ثابت بنية feat *p;
  صحيح features = 0;

#که_تعریف_شوی USE_SSH
  ساكن حرف ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#د_که_پای
#که_تعریف_شوی USE_SSL
#که_تعریف_شوی CURL_WITH_MULTI_SSL
  ساكن حرف ssl_بفر[200];
#وإلا
  ساكن حرف ssl_بفر[80];
#د_که_پای
#د_که_پای
#که_تعریف_شوی HAVE_BROTLI
  ساكن حرف brotli_بفر[80];
#د_که_پای
#که_تعریف_شوی HAVE_ZSTD
  ساكن حرف zstd_بفر[80];
#د_که_پای

  (فراغ)stamp;

#که_تعریف_شوی USE_SSL
  Curl_ssl_نسخه(ssl_بفر, حجم(ssl_بفر));
  نسخه_info.ssl_نسخه = ssl_بفر;
#د_که_پای

#که_تعریف_شوی HAVE_LIBZ
  نسخه_info.libz_نسخه = zlib_نسخه();
  /* libz left NULL if non-existing */
#د_که_پای
#که_تعریف_شوی USE_ARES
  {
    صحيح aresnum;
    نسخه_info.ares = ares_نسخه(&aresnum);
    نسخه_info.ares_عدد = aresnum;
  }
#د_که_پای
#که_تعریف_شوی USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  نسخه_info.libidn = idn2_check_نسخه(IDN2_نسخه);
#د_که_پای

#که_تعریف_شوی USE_SSH
  Curl_ssh_نسخه(ssh_buf, حجم(ssh_buf));
  نسخه_info.libssh_نسخه = ssh_buf;
#د_که_پای

#که_تعریف_شوی HAVE_BROTLI
  نسخه_info.brotli_ver_عدد = Brotli_Decoder_نسخه();
  brotli_نسخه(brotli_بفر, حجم(brotli_بفر));
  نسخه_info.brotli_نسخه = brotli_بفر;
#د_که_پای

#که_تعریف_شوی HAVE_ZSTD
  نسخه_info.zstd_ver_عدد = ZSTD_نسخه_شمېره();
  zstd_نسخه(zstd_بفر, حجم(zstd_بفر));
  نسخه_info.zstd_نسخه = zstd_بفر;
#د_که_پای

#که_تعریف_شوی USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_نسخه(0);
    نسخه_info.nghttp2_ver_عدد = (بینښه صحيح)h2->نسخه_عدد;
    نسخه_info.nghttp2_نسخه = h2->نسخه_str;
  }
#د_که_پای

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ساكن حرف quicbuffer[80];
    Curl_quic_ver(quicbuffer, حجم(quicbuffer));
    نسخه_info.quic_نسخه = quicbuffer;
  }
#د_که_پای

#که_تعریف_شوی USE_GSASL
  {
    نسخه_info.gsasl_نسخه = gsasl_check_نسخه(عدم);
  }
#د_که_پای

  /* Get available features, build bitmask and names array. */
  n = 0;
  لكل(p = features_جدول; p->name; p++)
    إذا(!p->present || p->present(&نسخه_info)) {
      features |= p->bitmask;
      feature_نومونه[n++] = p->name;
    }

#که_تعریف_شوی DEBUGBUILD
  features |= CURL_نسخه_CURLDEBUG; /* for compatibility */
#د_که_پای

  feature_نومونه[n] = عدم;  /* Terminate array. */
  نسخه_info.features = features;

  أرجع &نسخه_info;
}
