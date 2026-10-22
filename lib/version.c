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

#اگر_تعریف_شده USE_NGHTTP2
#ضمن <nghttp2/nghttp2.h>
#پایان_اگر

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "http2.h"
#ضمن "vssh/ssh.h"
#ضمن "vquic/vquic.h"
#ضمن "easy_lock.h"

#اگر_تعریف_شده USE_ARES
#  include <ares.h>
#پایان_اگر

#اگر_تعریف_شده USE_LIBIDN2
#ضمن <idn2.h>
#پایان_اگر

#اگر_تعریف_شده USE_LIBPSL
#ضمن <libpsl.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_LIBZ
#ضمن <zlib.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_BROTLI
#اگر_تعریف_شده CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wvla"
#پایان_اگر
#ضمن <brotli/decode.h>
#اگر_تعریف_شده CURL_HAVE_DIAG
#رهنمود GCC diagnostic برداشت
#پایان_اگر
#پایان_اگر

#اگر_تعریف_شده HAVE_ZSTD
#ضمن <zstd.h>
#پایان_اگر

#اگر_تعریف_شده USE_GSASL
#ضمن <gsasl.h>
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_LDAP
#ضمن "curl_ldap.h"
#پایان_اگر

#اگر_تعریف_شده HAVE_BROTLI
ساكن فراغ brotli_نسخه(حرف *buf, size_t bufsz)
{
  uint32_t brotli_نسخه = Brotli_Decoder_نسخه();
  بیعلامت صحيح major = brotli_نسخه >> 24;
  بیعلامت صحيح minor = (brotli_نسخه & 0x00FFFFFF) >> 12;
  بیعلامت صحيح patch = brotli_نسخه & 0x00000FFF;
  (فراغ)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#پایان_اگر

#اگر_تعریف_شده HAVE_ZSTD
ساكن فراغ zstd_نسخه(حرف *buf, size_t bufsz)
{
  بیعلامت صحيح نسخه_2 = ZSTD_نسخه_عدد();
  بیعلامت صحيح major = نسخه_2 / (100 * 100);
  بیعلامت صحيح minor = (نسخه_2 - (major * 100 * 100)) / 100;
  بیعلامت صحيح patch = نسخه_2 - (major * 100 * 100) - (minor * 100);
  (فراغ)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#پایان_اگر

#اگر_تعریف_شده USE_LIBPSL
ساكن فراغ psl_نسخه(حرف *buf, size_t bufsz)
{
#إذا defined(PSL_نسخه_MAJOR) && (PSL_نسخه_MAJOR > 0 ||     \
                                   PSL_نسخه_MINOR >= 11)
  صحيح رقم = psl_check_نسخه_عدد(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 رقم >> 16, (رقم >> 8) & 0xff, رقم & 0xff);
#وإلا
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_نسخه());
#پایان_اگر
}
#پایان_اگر

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#تعریف_کن USE_IDN
#پایان_اگر

#اگر_تعریف_شده USE_IDN
ساكن فراغ idn_نسخه(حرف *buf, size_t bufsz)
{
#اگر_تعریف_شده USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_نسخه(عدم));
#وإلاإذا defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#وإلاإذا defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#پایان_اگر
}
#پایان_اگر

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#تعریف_کن نسخه_PARTS 16 /* number of substrings we can concatenate */

حرف *curl_نسخه(فراغ)
{
  ساكن حرف بیرون[300];
  حرف *outp;
  size_t outlen;
  ثابت حرف *src[نسخه_PARTS];
#اگر_تعریف_شده USE_SSL
  حرف ssl_نسخه[200];
#پایان_اگر
#اگر_تعریف_شده HAVE_LIBZ
  حرف z_نسخه[30];
#پایان_اگر
#اگر_تعریف_شده HAVE_BROTLI
  حرف br_نسخه[30];
#پایان_اگر
#اگر_تعریف_شده HAVE_ZSTD
  حرف zstd_ver[30];
#پایان_اگر
#اگر_تعریف_شده USE_ARES
  حرف cares_نسخه[30];
#پایان_اگر
#اگر_تعریف_شده USE_IDN
  حرف idn_ver[30];
#پایان_اگر
#اگر_تعریف_شده USE_LIBPSL
  حرف psl_ver[30];
#پایان_اگر
#اگر_تعریف_شده USE_SSH
  حرف ssh_نسخه[30];
#پایان_اگر
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  حرف h2_نسخه[30];
#پایان_اگر
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  حرف h3_نسخه[30];
#پایان_اگر
#اگر_تعریف_شده USE_GSASL
  حرف gsasl_buf[30];
#پایان_اگر
#اگر_تعریف_شده HAVE_GSSAPI
  حرف gss_buf[40];
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_LDAP
  حرف ldap_buf[30];
#پایان_اگر
  صحيح i = 0;
  صحيح j;

#اگر_تعریف_شده DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
مجموعه*/
  ثابت حرف *debugversion = getenv("CURL_DEBUG_VERSION");
  إذا(debugversion) {
    curl_msnprintf(بیرون, حجم(بیرون), "%s", debugversion);
    أرجع بیرون;
  }
#پایان_اگر

  src[i++] = LIBCURL_NAME "/" LIBCURL_نسخه;
#اگر_تعریف_شده USE_SSL
  Curl_ssl_نسخه(ssl_نسخه, حجم(ssl_نسخه));
  src[i++] = ssl_نسخه;
#پایان_اگر
#اگر_تعریف_شده HAVE_LIBZ
  curl_msnprintf(z_نسخه, حجم(z_نسخه), "zlib/%s", zlib_نسخه());
  src[i++] = z_نسخه;
#پایان_اگر
#اگر_تعریف_شده HAVE_BROTLI
  brotli_نسخه(br_نسخه, حجم(br_نسخه));
  src[i++] = br_نسخه;
#پایان_اگر
#اگر_تعریف_شده HAVE_ZSTD
  zstd_نسخه(zstd_ver, حجم(zstd_ver));
  src[i++] = zstd_ver;
#پایان_اگر
#اگر_تعریف_شده USE_ARES
  curl_msnprintf(cares_نسخه, حجم(cares_نسخه),
                 "c-ares/%s", ares_نسخه(عدم));
  src[i++] = cares_نسخه;
#پایان_اگر
#اگر_تعریف_شده USE_IDN
  idn_نسخه(idn_ver, حجم(idn_ver));
  src[i++] = idn_ver;
#پایان_اگر
#اگر_تعریف_شده USE_LIBPSL
  psl_نسخه(psl_ver, حجم(psl_ver));
  src[i++] = psl_ver;
#پایان_اگر
#اگر_تعریف_شده USE_SSH
  Curl_ssh_نسخه(ssh_نسخه, حجم(ssh_نسخه));
  src[i++] = ssh_نسخه;
#پایان_اگر
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_نسخه, حجم(h2_نسخه));
  src[i++] = h2_نسخه;
#پایان_اگر
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_نسخه, حجم(h3_نسخه));
  src[i++] = h3_نسخه;
#پایان_اگر
#اگر_تعریف_شده USE_GSASL
  curl_msnprintf(gsasl_buf, حجم(gsasl_buf), "libgsasl/%s",
                 gsasl_check_نسخه(عدم));
  src[i++] = gsasl_buf;
#پایان_اگر
#اگر_تعریف_شده HAVE_GSSAPI
#اگر_تعریف_شده HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, حجم(gss_buf), "AppleGSS");
#وإلاإذا defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, حجم(gss_buf), "libgss/%s", GSS_نسخه);
#وإلاإذا defined(CURL_KRB5_نسخه)
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5/%s", CURL_KRB5_نسخه);
#وإلا
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5");
#پایان_اگر
  src[i++] = gss_buf;
#پایان_اگر /* HAVE_GSSAPI */
#اگر_تعریف_نشده CURL_DISABLE_LDAP
  Curl_ldap_نسخه(ldap_buf, حجم(ldap_buf));
  src[i++] = ldap_buf;
#پایان_اگر

  DEBUGASSERT(i <= نسخه_PARTS);

  outp = &بیرون[0];
  outlen = حجم(بیرون);
  لكل(j = 0; j < i; j++) {
    size_t n = طول_رشته(src[j]);
    /* we need room for a space, the string and the final zero */
    إذا(outlen <= (n + 2))
      اكسر;
    إذا(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    کپی_حافظه(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  أرجع بیرون;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ساكن ثابت حرف * ثابت supported_protocols[] = {
#اگر_تعریف_نشده CURL_DISABLE_DICT
  "dict",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_FILE
  "file",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_FTP
  "ftp",
#پایان_اگر
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_GOPHER
  "gopher",
#پایان_اگر
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HTTP
  "http",
#پایان_اگر
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_IMAP
  "imap",
#پایان_اگر
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_LDAP
  "ldap",
#إذا !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#پایان_اگر
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_MQTT
  "mqtt",
#پایان_اگر
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_POP3
  "pop3",
#پایان_اگر
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_RTSP
  "rtsp",
#پایان_اگر
#اگر_تعریف_شده USE_SSH
  "scp",
  "sftp",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_SMTP
  "smtp",
#پایان_اگر
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_TELNET
  "telnet",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_TFTP
  "tftp",
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS
  "ws",
#پایان_اگر
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#پایان_اگر
#پایان_اگر

  عدم
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ساكن صحيح idn_present(curl_نسخه_info_داده *info)
{
#إذا defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (فراغ)info;
  أرجع TRUE;
#وإلا
  أرجع !!info->libidn;
#پایان_اگر
}
#پایان_اگر

#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ساكن صحيح https_proxy_present(curl_نسخه_info_داده *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_HTTPS_PROXY);
}
#پایان_اگر

#إذا defined(USE_SSL) && defined(USE_ECH)
ساكن صحيح ech_present(curl_نسخه_info_داده *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_ECH);
}
#پایان_اگر

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#تعریف_کن FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

بنية feat {
  ثابت حرف *name;
  صحيح        (*present)(curl_نسخه_info_داده *info);
  صحيح        bitmask;
};

ساكن ثابت بنية feat features_جدول[] = {
#اگر_تعریف_نشده CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     عدم,                CURL_نسخه_ALTSVC),
#پایان_اگر
#إذا defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     عدم,                0),
#پایان_اگر
#اگر_تعریف_شده CURLRES_ASYNCH
  FEATURE("AsynchDNS",   عدم,                CURL_نسخه_ASYNCHDNS),
#پایان_اگر
#اگر_تعریف_شده HAVE_BROTLI
  FEATURE("brotli",      عدم,                CURL_نسخه_BROTLI),
#پایان_اگر
#اگر_تعریف_شده DEBUGBUILD
  FEATURE("Debug",       عدم,                CURL_نسخه_DEBUG),
#پایان_اگر
#إذا defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#اگر_تعریف_نشده USE_HTTPSRR
#خطا "ECH enabled but not HTTPSRR, must be a config error"
#پایان_اگر
#پایان_اگر
#اگر_تعریف_شده USE_GSASL
  FEATURE("gsasl",       عدم,                CURL_نسخه_GSASL),
#پایان_اگر
#اگر_تعریف_شده HAVE_GSSAPI
  FEATURE("GSS-API",     عدم,                CURL_نسخه_GSSAPI),
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HSTS
  FEATURE("HSTS",        عدم,                CURL_نسخه_HSTS),
#پایان_اگر
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       عدم,                CURL_نسخه_HTTP2),
#پایان_اگر
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       عدم,                CURL_نسخه_HTTP3),
#پایان_اگر
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_نسخه_HTTPS_PROXY),
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     عدم,                0),
#پایان_اگر
#اگر_تعریف_شده USE_HTTPSRR
  FEATURE("HTTPSRR",     عدم,                0),
#پایان_اگر
#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_نسخه_IDN),
#پایان_اگر
#اگر_تعریف_شده USE_IPV6
  FEATURE("IPv6",        عدم,                CURL_نسخه_IPV6),
#پایان_اگر
#اگر_تعریف_شده USE_KERBEROS5
  FEATURE("Kerberos",    عدم,                CURL_نسخه_KERBEROS5),
#پایان_اگر
#إذا (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   عدم,                CURL_نسخه_LARGEFILE),
#پایان_اگر
#اگر_تعریف_شده HAVE_LIBZ
  FEATURE("libz",        عدم,                CURL_نسخه_LIBZ),
#پایان_اگر
#اگر_تعریف_شده CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    عدم,                CURL_نسخه_MULTI_SSL),
#پایان_اگر
#اگر_تعریف_شده USE_NTLM
  FEATURE("NTLM",        عدم,                CURL_نسخه_NTLM),
#پایان_اگر
#اگر_تعریف_شده USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", عدم,                0),
#پایان_اگر
#اگر_تعریف_شده USE_LIBPSL
  FEATURE("PSL",         عدم,                CURL_نسخه_PSL),
#پایان_اگر
#اگر_تعریف_شده USE_SSL
#اگر_تعریف_شده USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", عدم,              0),
#وإلاإذا defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    عدم,              0),
#پایان_اگر
#پایان_اگر /* USE_SSL */
#اگر_تعریف_شده USE_SPNEGO
  FEATURE("SPNEGO",      عدم,                CURL_نسخه_SPNEGO),
#پایان_اگر
#اگر_تعریف_شده USE_SSL
  FEATURE("SSL",         عدم,                CURL_نسخه_SSL),
#پایان_اگر
#اگر_تعریف_شده USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", عدم,                0),
#پایان_اگر
#اگر_تعریف_شده USE_WINDOWS_SSPI
  FEATURE("SSPI",        عدم,                CURL_نسخه_SSPI),
#پایان_اگر
#اگر_تعریف_شده GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  عدم,                CURL_نسخه_THREADSAFE),
#پایان_اگر
#إذا defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     عدم,                CURL_نسخه_UNICODE),
#پایان_اگر
#اگر_تعریف_شده USE_UNIX_SOCKETS
  FEATURE("UnixSockets", عدم,                CURL_نسخه_UNIX_SOCKETS),
#پایان_اگر
#اگر_تعریف_شده HAVE_ZSTD
  FEATURE("zstd",        عدم,                CURL_نسخه_ZSTD),
#پایان_اگر
  {عدم,                 عدم,                0}
};

ساكن ثابت حرف *feature_نامها[CURL_ARRAYSIZE(features_جدول)] = { عدم };

ساكن curl_نسخه_info_داده نسخه_info = {
  CURLVERSION_NOW,
  LIBCURL_نسخه,
  LIBCURL_نسخه_رقم,
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
#اگر_تعریف_شده CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#وإلا
  عدم,
#پایان_اگر
#اگر_تعریف_شده CURL_CA_مسیر
  CURL_CA_مسیر,  /* capath */
#وإلا
  عدم,
#پایان_اگر
  0,    /* zstd_ver_num */
  عدم, /* zstd version */
  عدم, /* Hyper version */
  عدم, /* gsasl version */
  feature_نامها,
  عدم  /* rtmp version */
};

curl_نسخه_info_داده *curl_نسخه_info(CURLversion stamp)
{
  size_t n;
  ثابت بنية feat *p;
  صحيح features = 0;

#اگر_تعریف_شده USE_SSH
  ساكن حرف ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#پایان_اگر
#اگر_تعریف_شده USE_SSL
#اگر_تعریف_شده CURL_WITH_MULTI_SSL
  ساكن حرف ssl_میانگیر[200];
#وإلا
  ساكن حرف ssl_میانگیر[80];
#پایان_اگر
#پایان_اگر
#اگر_تعریف_شده HAVE_BROTLI
  ساكن حرف brotli_میانگیر[80];
#پایان_اگر
#اگر_تعریف_شده HAVE_ZSTD
  ساكن حرف zstd_میانگیر[80];
#پایان_اگر

  (فراغ)stamp;

#اگر_تعریف_شده USE_SSL
  Curl_ssl_نسخه(ssl_میانگیر, حجم(ssl_میانگیر));
  نسخه_info.ssl_نسخه = ssl_میانگیر;
#پایان_اگر

#اگر_تعریف_شده HAVE_LIBZ
  نسخه_info.libz_نسخه = zlib_نسخه();
  /* libz left NULL if non-existing */
#پایان_اگر
#اگر_تعریف_شده USE_ARES
  {
    صحيح aresnum;
    نسخه_info.ares = ares_نسخه(&aresnum);
    نسخه_info.ares_رقم = aresnum;
  }
#پایان_اگر
#اگر_تعریف_شده USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  نسخه_info.libidn = idn2_check_نسخه(IDN2_نسخه);
#پایان_اگر

#اگر_تعریف_شده USE_SSH
  Curl_ssh_نسخه(ssh_buf, حجم(ssh_buf));
  نسخه_info.libssh_نسخه = ssh_buf;
#پایان_اگر

#اگر_تعریف_شده HAVE_BROTLI
  نسخه_info.brotli_ver_رقم = Brotli_Decoder_نسخه();
  brotli_نسخه(brotli_میانگیر, حجم(brotli_میانگیر));
  نسخه_info.brotli_نسخه = brotli_میانگیر;
#پایان_اگر

#اگر_تعریف_شده HAVE_ZSTD
  نسخه_info.zstd_ver_رقم = ZSTD_نسخه_عدد();
  zstd_نسخه(zstd_میانگیر, حجم(zstd_میانگیر));
  نسخه_info.zstd_نسخه = zstd_میانگیر;
#پایان_اگر

#اگر_تعریف_شده USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_نسخه(0);
    نسخه_info.nghttp2_ver_رقم = (بیعلامت صحيح)h2->نسخه_رقم;
    نسخه_info.nghttp2_نسخه = h2->نسخه_str;
  }
#پایان_اگر

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ساكن حرف quicbuffer[80];
    Curl_quic_ver(quicbuffer, حجم(quicbuffer));
    نسخه_info.quic_نسخه = quicbuffer;
  }
#پایان_اگر

#اگر_تعریف_شده USE_GSASL
  {
    نسخه_info.gsasl_نسخه = gsasl_check_نسخه(عدم);
  }
#پایان_اگر

  /* Get available features, build bitmask and names array. */
  n = 0;
  لكل(p = features_جدول; p->name; p++)
    إذا(!p->present || p->present(&نسخه_info)) {
      features |= p->bitmask;
      feature_نامها[n++] = p->name;
    }

#اگر_تعریف_شده DEBUGBUILD
  features |= CURL_نسخه_CURLDEBUG; /* for compatibility */
#پایان_اگر

  feature_نامها[n] = عدم;  /* Terminate array. */
  نسخه_info.features = features;

  أرجع &نسخه_info;
}
