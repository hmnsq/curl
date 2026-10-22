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

#جے_تعریف_شدہ USE_NGHTTP2
#ضمن <nghttp2/nghttp2.h>
#جے_انت

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "http2.h"
#ضمن "vssh/ssh.h"
#ضمن "vquic/vquic.h"
#ضمن "easy_lock.h"

#جے_تعریف_شدہ USE_ARES
#  include <ares.h>
#جے_انت

#جے_تعریف_شدہ USE_LIBIDN2
#ضمن <idn2.h>
#جے_انت

#جے_تعریف_شدہ USE_LIBPSL
#ضمن <libpsl.h>
#جے_انت

#جے_تعریف_شدہ HAVE_LIBZ
#ضمن <zlib.h>
#جے_انت

#جے_تعریف_شدہ HAVE_BROTLI
#جے_تعریف_شدہ CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wvla"
#جے_انت
#ضمن <brotli/decode.h>
#جے_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic نکالو
#جے_انت
#جے_انت

#جے_تعریف_شدہ HAVE_ZSTD
#ضمن <zstd.h>
#جے_انت

#جے_تعریف_شدہ USE_GSASL
#ضمن <gsasl.h>
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_LDAP
#ضمن "curl_ldap.h"
#جے_انت

#جے_تعریف_شدہ HAVE_BROTLI
ساكن فراغ brotli_نسخہ(حرف *buf, size_t bufsz)
{
  uint32_t brotli_نسخہ = Brotli_Decoder_نسخہ();
  بنانشانی صحيح major = brotli_نسخہ >> 24;
  بنانشانی صحيح minor = (brotli_نسخہ & 0x00FFFFFF) >> 12;
  بنانشانی صحيح patch = brotli_نسخہ & 0x00000FFF;
  (فراغ)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#جے_انت

#جے_تعریف_شدہ HAVE_ZSTD
ساكن فراغ zstd_نسخہ(حرف *buf, size_t bufsz)
{
  بنانشانی صحيح نسخہ_2 = ZSTD_نسخہ_نمبر();
  بنانشانی صحيح major = نسخہ_2 / (100 * 100);
  بنانشانی صحيح minor = (نسخہ_2 - (major * 100 * 100)) / 100;
  بنانشانی صحيح patch = نسخہ_2 - (major * 100 * 100) - (minor * 100);
  (فراغ)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#جے_انت

#جے_تعریف_شدہ USE_LIBPSL
ساكن فراغ psl_نسخہ(حرف *buf, size_t bufsz)
{
#إذا defined(PSL_نسخہ_MAJOR) && (PSL_نسخہ_MAJOR > 0 ||     \
                                   PSL_نسخہ_MINOR >= 11)
  صحيح عدد = psl_check_نسخہ_نمبر(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 عدد >> 16, (عدد >> 8) & 0xff, عدد & 0xff);
#وإلا
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_نسخہ());
#جے_انت
}
#جے_انت

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#تعریف_کرو USE_IDN
#جے_انت

#جے_تعریف_شدہ USE_IDN
ساكن فراغ idn_نسخہ(حرف *buf, size_t bufsz)
{
#جے_تعریف_شدہ USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_نسخہ(عدم));
#وإلاإذا defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#وإلاإذا defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#جے_انت
}
#جے_انت

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#تعریف_کرو نسخہ_PARTS 16 /* number of substrings we can concatenate */

حرف *curl_نسخہ(فراغ)
{
  ساكن حرف باہرلا[300];
  حرف *outp;
  size_t outlen;
  ثابت حرف *src[نسخہ_PARTS];
#جے_تعریف_شدہ USE_SSL
  حرف ssl_نسخہ[200];
#جے_انت
#جے_تعریف_شدہ HAVE_LIBZ
  حرف z_نسخہ[30];
#جے_انت
#جے_تعریف_شدہ HAVE_BROTLI
  حرف br_نسخہ[30];
#جے_انت
#جے_تعریف_شدہ HAVE_ZSTD
  حرف zstd_ver[30];
#جے_انت
#جے_تعریف_شدہ USE_ARES
  حرف cares_نسخہ[30];
#جے_انت
#جے_تعریف_شدہ USE_IDN
  حرف idn_ver[30];
#جے_انت
#جے_تعریف_شدہ USE_LIBPSL
  حرف psl_ver[30];
#جے_انت
#جے_تعریف_شدہ USE_SSH
  حرف ssh_نسخہ[30];
#جے_انت
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  حرف h2_نسخہ[30];
#جے_انت
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  حرف h3_نسخہ[30];
#جے_انت
#جے_تعریف_شدہ USE_GSASL
  حرف gsasl_buf[30];
#جے_انت
#جے_تعریف_شدہ HAVE_GSSAPI
  حرف gss_buf[40];
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_LDAP
  حرف ldap_buf[30];
#جے_انت
  صحيح i = 0;
  صحيح j;

#جے_تعریف_شدہ DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
سیٹ*/
  ثابت حرف *debugversion = getenv("CURL_DEBUG_VERSION");
  إذا(debugversion) {
    curl_msnprintf(باہرلا, حجم(باہرلا), "%s", debugversion);
    أرجع باہرلا;
  }
#جے_انت

  src[i++] = LIBCURL_NAME "/" LIBCURL_نسخہ;
#جے_تعریف_شدہ USE_SSL
  Curl_ssl_نسخہ(ssl_نسخہ, حجم(ssl_نسخہ));
  src[i++] = ssl_نسخہ;
#جے_انت
#جے_تعریف_شدہ HAVE_LIBZ
  curl_msnprintf(z_نسخہ, حجم(z_نسخہ), "zlib/%s", zlib_نسخہ());
  src[i++] = z_نسخہ;
#جے_انت
#جے_تعریف_شدہ HAVE_BROTLI
  brotli_نسخہ(br_نسخہ, حجم(br_نسخہ));
  src[i++] = br_نسخہ;
#جے_انت
#جے_تعریف_شدہ HAVE_ZSTD
  zstd_نسخہ(zstd_ver, حجم(zstd_ver));
  src[i++] = zstd_ver;
#جے_انت
#جے_تعریف_شدہ USE_ARES
  curl_msnprintf(cares_نسخہ, حجم(cares_نسخہ),
                 "c-ares/%s", ares_نسخہ(عدم));
  src[i++] = cares_نسخہ;
#جے_انت
#جے_تعریف_شدہ USE_IDN
  idn_نسخہ(idn_ver, حجم(idn_ver));
  src[i++] = idn_ver;
#جے_انت
#جے_تعریف_شدہ USE_LIBPSL
  psl_نسخہ(psl_ver, حجم(psl_ver));
  src[i++] = psl_ver;
#جے_انت
#جے_تعریف_شدہ USE_SSH
  Curl_ssh_نسخہ(ssh_نسخہ, حجم(ssh_نسخہ));
  src[i++] = ssh_نسخہ;
#جے_انت
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_نسخہ, حجم(h2_نسخہ));
  src[i++] = h2_نسخہ;
#جے_انت
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_نسخہ, حجم(h3_نسخہ));
  src[i++] = h3_نسخہ;
#جے_انت
#جے_تعریف_شدہ USE_GSASL
  curl_msnprintf(gsasl_buf, حجم(gsasl_buf), "libgsasl/%s",
                 gsasl_check_نسخہ(عدم));
  src[i++] = gsasl_buf;
#جے_انت
#جے_تعریف_شدہ HAVE_GSSAPI
#جے_تعریف_شدہ HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, حجم(gss_buf), "AppleGSS");
#وإلاإذا defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, حجم(gss_buf), "libgss/%s", GSS_نسخہ);
#وإلاإذا defined(CURL_KRB5_نسخہ)
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5/%s", CURL_KRB5_نسخہ);
#وإلا
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5");
#جے_انت
  src[i++] = gss_buf;
#جے_انت /* HAVE_GSSAPI */
#جے_بنا_تعریف CURL_DISABLE_LDAP
  Curl_ldap_نسخہ(ldap_buf, حجم(ldap_buf));
  src[i++] = ldap_buf;
#جے_انت

  DEBUGASSERT(i <= نسخہ_PARTS);

  outp = &باہرلا[0];
  outlen = حجم(باہرلا);
  لكل(j = 0; j < i; j++) {
    size_t n = لڑی_لمبائی(src[j]);
    /* we need room for a space, the string and the final zero */
    إذا(outlen <= (n + 2))
      اكسر;
    إذا(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    حافظہ_نقل(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  أرجع باہرلا;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ساكن ثابت حرف * ثابت supported_protocols[] = {
#جے_بنا_تعریف CURL_DISABLE_DICT
  "dict",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_FILE
  "file",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_FTP
  "ftp",
#جے_انت
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_GOPHER
  "gopher",
#جے_انت
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HTTP
  "http",
#جے_انت
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_IMAP
  "imap",
#جے_انت
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_LDAP
  "ldap",
#إذا !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#جے_انت
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_MQTT
  "mqtt",
#جے_انت
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_POP3
  "pop3",
#جے_انت
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_RTSP
  "rtsp",
#جے_انت
#جے_تعریف_شدہ USE_SSH
  "scp",
  "sftp",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_SMTP
  "smtp",
#جے_انت
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_TELNET
  "telnet",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_TFTP
  "tftp",
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#جے_بنا_تعریف CURL_DISABLE_WEBSOCKETS
  "ws",
#جے_انت
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#جے_انت
#جے_انت

  عدم
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ساكن صحيح idn_present(curl_نسخہ_info_انکڑے *info)
{
#إذا defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (فراغ)info;
  أرجع TRUE;
#وإلا
  أرجع !!info->libidn;
#جے_انت
}
#جے_انت

#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ساكن صحيح https_proxy_present(curl_نسخہ_info_انکڑے *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_HTTPS_PROXY);
}
#جے_انت

#إذا defined(USE_SSL) && defined(USE_ECH)
ساكن صحيح ech_present(curl_نسخہ_info_انکڑے *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_ECH);
}
#جے_انت

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#تعریف_کرو FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

بنية feat {
  ثابت حرف *name;
  صحيح        (*present)(curl_نسخہ_info_انکڑے *info);
  صحيح        bitmask;
};

ساكن ثابت بنية feat features_جدول[] = {
#جے_بنا_تعریف CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     عدم,                CURL_نسخہ_ALTSVC),
#جے_انت
#إذا defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     عدم,                0),
#جے_انت
#جے_تعریف_شدہ CURLRES_ASYNCH
  FEATURE("AsynchDNS",   عدم,                CURL_نسخہ_ASYNCHDNS),
#جے_انت
#جے_تعریف_شدہ HAVE_BROTLI
  FEATURE("brotli",      عدم,                CURL_نسخہ_BROTLI),
#جے_انت
#جے_تعریف_شدہ DEBUGBUILD
  FEATURE("Debug",       عدم,                CURL_نسخہ_DEBUG),
#جے_انت
#إذا defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#جے_بنا_تعریف USE_HTTPSRR
#غلطی "ECH enabled but not HTTPSRR, must be a config error"
#جے_انت
#جے_انت
#جے_تعریف_شدہ USE_GSASL
  FEATURE("gsasl",       عدم,                CURL_نسخہ_GSASL),
#جے_انت
#جے_تعریف_شدہ HAVE_GSSAPI
  FEATURE("GSS-API",     عدم,                CURL_نسخہ_GSSAPI),
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HSTS
  FEATURE("HSTS",        عدم,                CURL_نسخہ_HSTS),
#جے_انت
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       عدم,                CURL_نسخہ_HTTP2),
#جے_انت
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       عدم,                CURL_نسخہ_HTTP3),
#جے_انت
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_نسخہ_HTTPS_PROXY),
#جے_انت
#جے_بنا_تعریف CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     عدم,                0),
#جے_انت
#جے_تعریف_شدہ USE_HTTPSRR
  FEATURE("HTTPSRR",     عدم,                0),
#جے_انت
#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_نسخہ_IDN),
#جے_انت
#جے_تعریف_شدہ USE_IPV6
  FEATURE("IPv6",        عدم,                CURL_نسخہ_IPV6),
#جے_انت
#جے_تعریف_شدہ USE_KERBEROS5
  FEATURE("Kerberos",    عدم,                CURL_نسخہ_KERBEROS5),
#جے_انت
#إذا (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   عدم,                CURL_نسخہ_LARGEFILE),
#جے_انت
#جے_تعریف_شدہ HAVE_LIBZ
  FEATURE("libz",        عدم,                CURL_نسخہ_LIBZ),
#جے_انت
#جے_تعریف_شدہ CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    عدم,                CURL_نسخہ_MULTI_SSL),
#جے_انت
#جے_تعریف_شدہ USE_NTLM
  FEATURE("NTLM",        عدم,                CURL_نسخہ_NTLM),
#جے_انت
#جے_تعریف_شدہ USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", عدم,                0),
#جے_انت
#جے_تعریف_شدہ USE_LIBPSL
  FEATURE("PSL",         عدم,                CURL_نسخہ_PSL),
#جے_انت
#جے_تعریف_شدہ USE_SSL
#جے_تعریف_شدہ USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", عدم,              0),
#وإلاإذا defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    عدم,              0),
#جے_انت
#جے_انت /* USE_SSL */
#جے_تعریف_شدہ USE_SPNEGO
  FEATURE("SPNEGO",      عدم,                CURL_نسخہ_SPNEGO),
#جے_انت
#جے_تعریف_شدہ USE_SSL
  FEATURE("SSL",         عدم,                CURL_نسخہ_SSL),
#جے_انت
#جے_تعریف_شدہ USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", عدم,                0),
#جے_انت
#جے_تعریف_شدہ USE_WINDOWS_SSPI
  FEATURE("SSPI",        عدم,                CURL_نسخہ_SSPI),
#جے_انت
#جے_تعریف_شدہ GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  عدم,                CURL_نسخہ_THREADSAFE),
#جے_انت
#إذا defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     عدم,                CURL_نسخہ_UNICODE),
#جے_انت
#جے_تعریف_شدہ USE_UNIX_SOCKETS
  FEATURE("UnixSockets", عدم,                CURL_نسخہ_UNIX_SOCKETS),
#جے_انت
#جے_تعریف_شدہ HAVE_ZSTD
  FEATURE("zstd",        عدم,                CURL_نسخہ_ZSTD),
#جے_انت
  {عدم,                 عدم,                0}
};

ساكن ثابت حرف *feature_ناں[CURL_ARRAYSIZE(features_جدول)] = { عدم };

ساكن curl_نسخہ_info_انکڑے نسخہ_info = {
  CURLVERSION_NOW,
  LIBCURL_نسخہ,
  LIBCURL_نسخہ_عدد,
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
#جے_تعریف_شدہ CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#وإلا
  عدم,
#جے_انت
#جے_تعریف_شدہ CURL_CA_رستہ
  CURL_CA_رستہ,  /* capath */
#وإلا
  عدم,
#جے_انت
  0,    /* zstd_ver_num */
  عدم, /* zstd version */
  عدم, /* Hyper version */
  عدم, /* gsasl version */
  feature_ناں,
  عدم  /* rtmp version */
};

curl_نسخہ_info_انکڑے *curl_نسخہ_info(CURLversion stamp)
{
  size_t n;
  ثابت بنية feat *p;
  صحيح features = 0;

#جے_تعریف_شدہ USE_SSH
  ساكن حرف ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#جے_انت
#جے_تعریف_شدہ USE_SSL
#جے_تعریف_شدہ CURL_WITH_MULTI_SSL
  ساكن حرف ssl_بفر[200];
#وإلا
  ساكن حرف ssl_بفر[80];
#جے_انت
#جے_انت
#جے_تعریف_شدہ HAVE_BROTLI
  ساكن حرف brotli_بفر[80];
#جے_انت
#جے_تعریف_شدہ HAVE_ZSTD
  ساكن حرف zstd_بفر[80];
#جے_انت

  (فراغ)stamp;

#جے_تعریف_شدہ USE_SSL
  Curl_ssl_نسخہ(ssl_بفر, حجم(ssl_بفر));
  نسخہ_info.ssl_نسخہ = ssl_بفر;
#جے_انت

#جے_تعریف_شدہ HAVE_LIBZ
  نسخہ_info.libz_نسخہ = zlib_نسخہ();
  /* libz left NULL if non-existing */
#جے_انت
#جے_تعریف_شدہ USE_ARES
  {
    صحيح aresnum;
    نسخہ_info.ares = ares_نسخہ(&aresnum);
    نسخہ_info.ares_عدد = aresnum;
  }
#جے_انت
#جے_تعریف_شدہ USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  نسخہ_info.libidn = idn2_check_نسخہ(IDN2_نسخہ);
#جے_انت

#جے_تعریف_شدہ USE_SSH
  Curl_ssh_نسخہ(ssh_buf, حجم(ssh_buf));
  نسخہ_info.libssh_نسخہ = ssh_buf;
#جے_انت

#جے_تعریف_شدہ HAVE_BROTLI
  نسخہ_info.brotli_ver_عدد = Brotli_Decoder_نسخہ();
  brotli_نسخہ(brotli_بفر, حجم(brotli_بفر));
  نسخہ_info.brotli_نسخہ = brotli_بفر;
#جے_انت

#جے_تعریف_شدہ HAVE_ZSTD
  نسخہ_info.zstd_ver_عدد = ZSTD_نسخہ_نمبر();
  zstd_نسخہ(zstd_بفر, حجم(zstd_بفر));
  نسخہ_info.zstd_نسخہ = zstd_بفر;
#جے_انت

#جے_تعریف_شدہ USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_نسخہ(0);
    نسخہ_info.nghttp2_ver_عدد = (بنانشانی صحيح)h2->نسخہ_عدد;
    نسخہ_info.nghttp2_نسخہ = h2->نسخہ_str;
  }
#جے_انت

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ساكن حرف quicbuffer[80];
    Curl_quic_ver(quicbuffer, حجم(quicbuffer));
    نسخہ_info.quic_نسخہ = quicbuffer;
  }
#جے_انت

#جے_تعریف_شدہ USE_GSASL
  {
    نسخہ_info.gsasl_نسخہ = gsasl_check_نسخہ(عدم);
  }
#جے_انت

  /* Get available features, build bitmask and names array. */
  n = 0;
  لكل(p = features_جدول; p->name; p++)
    إذا(!p->present || p->present(&نسخہ_info)) {
      features |= p->bitmask;
      feature_ناں[n++] = p->name;
    }

#جے_تعریف_شدہ DEBUGBUILD
  features |= CURL_نسخہ_CURLDEBUG; /* for compatibility */
#جے_انت

  feature_ناں[n] = عدم;  /* Terminate array. */
  نسخہ_info.features = features;

  أرجع &نسخہ_info;
}
