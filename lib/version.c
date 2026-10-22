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

#جيڪڏهن_وصف_ٿيل USE_NGHTTP2
#ضمن <nghttp2/nghttp2.h>
#جيڪڏهن_پورو

#ضمن "urldata.h"
#ضمن "vtls/vtls.h"
#ضمن "http2.h"
#ضمن "vssh/ssh.h"
#ضمن "vquic/vquic.h"
#ضمن "easy_lock.h"

#جيڪڏهن_وصف_ٿيل USE_ARES
#  include <ares.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_LIBIDN2
#ضمن <idn2.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_LIBPSL
#ضمن <libpsl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
#ضمن <zlib.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
#جيڪڏهن_وصف_ٿيل CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Wvla"
#جيڪڏهن_پورو
#ضمن <brotli/decode.h>
#جيڪڏهن_وصف_ٿيل CURL_HAVE_DIAG
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
#ضمن <zstd.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_GSASL
#ضمن <gsasl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_LDAP
#ضمن "curl_ldap.h"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
ساكن فراغ brotli_نسخو(حرف *buf, size_t bufsz)
{
  uint32_t brotli_نسخو = Brotli_Decoder_نسخو();
  بنانشان صحيح major = brotli_نسخو >> 24;
  بنانشان صحيح minor = (brotli_نسخو & 0x00FFFFFF) >> 12;
  بنانشان صحيح patch = brotli_نسخو & 0x00000FFF;
  (فراغ)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
ساكن فراغ zstd_نسخو(حرف *buf, size_t bufsz)
{
  بنانشان صحيح نسخو_2 = ZSTD_نسخو_عدد();
  بنانشان صحيح major = نسخو_2 / (100 * 100);
  بنانشان صحيح minor = (نسخو_2 - (major * 100 * 100)) / 100;
  بنانشان صحيح patch = نسخو_2 - (major * 100 * 100) - (minor * 100);
  (فراغ)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_LIBPSL
ساكن فراغ psl_نسخو(حرف *buf, size_t bufsz)
{
#إذا defined(PSL_نسخو_MAJOR) && (PSL_نسخو_MAJOR > 0 ||     \
                                   PSL_نسخو_MINOR >= 11)
  صحيح انگ = psl_check_نسخو_عدد(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 انگ >> 16, (انگ >> 8) & 0xff, انگ & 0xff);
#وإلا
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_نسخو());
#جيڪڏهن_پورو
}
#جيڪڏهن_پورو

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#وصف_ڪريو USE_IDN
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_IDN
ساكن فراغ idn_نسخو(حرف *buf, size_t bufsz)
{
#جيڪڏهن_وصف_ٿيل USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_نسخو(عدم));
#وإلاإذا defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#وإلاإذا defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#جيڪڏهن_پورو
}
#جيڪڏهن_پورو

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#وصف_ڪريو نسخو_PARTS 16 /* number of substrings we can concatenate */

حرف *curl_نسخو(فراغ)
{
  ساكن حرف ٻاهريون[300];
  حرف *outp;
  size_t outlen;
  ثابت حرف *src[نسخو_PARTS];
#جيڪڏهن_وصف_ٿيل USE_SSL
  حرف ssl_نسخو[200];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
  حرف z_نسخو[30];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
  حرف br_نسخو[30];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
  حرف zstd_ver[30];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_ARES
  حرف cares_نسخو[30];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_IDN
  حرف idn_ver[30];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_LIBPSL
  حرف psl_ver[30];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSH
  حرف ssh_نسخو[30];
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  حرف h2_نسخو[30];
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  حرف h3_نسخو[30];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_GSASL
  حرف gsasl_buf[30];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_GSSAPI
  حرف gss_buf[40];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_LDAP
  حرف ldap_buf[30];
#جيڪڏهن_پورو
  صحيح i = 0;
  صحيح j;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
سيٽ*/
  ثابت حرف *debugversion = getenv("CURL_DEBUG_VERSION");
  إذا(debugversion) {
    curl_msnprintf(ٻاهريون, حجم(ٻاهريون), "%s", debugversion);
    أرجع ٻاهريون;
  }
#جيڪڏهن_پورو

  src[i++] = LIBCURL_NAME "/" LIBCURL_نسخو;
#جيڪڏهن_وصف_ٿيل USE_SSL
  Curl_ssl_نسخو(ssl_نسخو, حجم(ssl_نسخو));
  src[i++] = ssl_نسخو;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
  curl_msnprintf(z_نسخو, حجم(z_نسخو), "zlib/%s", zlib_نسخو());
  src[i++] = z_نسخو;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
  brotli_نسخو(br_نسخو, حجم(br_نسخو));
  src[i++] = br_نسخو;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
  zstd_نسخو(zstd_ver, حجم(zstd_ver));
  src[i++] = zstd_ver;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_ARES
  curl_msnprintf(cares_نسخو, حجم(cares_نسخو),
                 "c-ares/%s", ares_نسخو(عدم));
  src[i++] = cares_نسخو;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_IDN
  idn_نسخو(idn_ver, حجم(idn_ver));
  src[i++] = idn_ver;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_LIBPSL
  psl_نسخو(psl_ver, حجم(psl_ver));
  src[i++] = psl_ver;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSH
  Curl_ssh_نسخو(ssh_نسخو, حجم(ssh_نسخو));
  src[i++] = ssh_نسخو;
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_نسخو, حجم(h2_نسخو));
  src[i++] = h2_نسخو;
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_نسخو, حجم(h3_نسخو));
  src[i++] = h3_نسخو;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_GSASL
  curl_msnprintf(gsasl_buf, حجم(gsasl_buf), "libgsasl/%s",
                 gsasl_check_نسخو(عدم));
  src[i++] = gsasl_buf;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_GSSAPI
#جيڪڏهن_وصف_ٿيل HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, حجم(gss_buf), "AppleGSS");
#وإلاإذا defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, حجم(gss_buf), "libgss/%s", GSS_نسخو);
#وإلاإذا defined(CURL_KRB5_نسخو)
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5/%s", CURL_KRB5_نسخو);
#وإلا
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5");
#جيڪڏهن_پورو
  src[i++] = gss_buf;
#جيڪڏهن_پورو /* HAVE_GSSAPI */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_LDAP
  Curl_ldap_نسخو(ldap_buf, حجم(ldap_buf));
  src[i++] = ldap_buf;
#جيڪڏهن_پورو

  DEBUGASSERT(i <= نسخو_PARTS);

  outp = &ٻاهريون[0];
  outlen = حجم(ٻاهريون);
  لكل(j = 0; j < i; j++) {
    size_t n = لڙي_ڊيگهه(src[j]);
    /* we need room for a space, the string and the final zero */
    إذا(outlen <= (n + 2))
      اكسر;
    إذا(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    يادگيري_نقل(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  أرجع ٻاهريون;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ساكن ثابت حرف * ثابت supported_protocols[] = {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DICT
  "dict",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FILE
  "file",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_FTP
  "ftp",
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_GOPHER
  "gopher",
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  "http",
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_IMAP
  "imap",
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_LDAP
  "ldap",
#إذا !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#جيڪڏهن_پورو
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_MQTT
  "mqtt",
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_POP3
  "pop3",
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
  "rtsp",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSH
  "scp",
  "sftp",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_SMTP
  "smtp",
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_TELNET
  "telnet",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_TFTP
  "tftp",
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS
  "ws",
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#جيڪڏهن_پورو
#جيڪڏهن_پورو

  عدم
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ساكن صحيح idn_present(curl_نسخو_info_معلومات *info)
{
#إذا defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (فراغ)info;
  أرجع TRUE;
#وإلا
  أرجع !!info->libidn;
#جيڪڏهن_پورو
}
#جيڪڏهن_پورو

#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ساكن صحيح https_proxy_present(curl_نسخو_info_معلومات *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_HTTPS_PROXY);
}
#جيڪڏهن_پورو

#إذا defined(USE_SSL) && defined(USE_ECH)
ساكن صحيح ech_present(curl_نسخو_info_معلومات *info)
{
  (فراغ)info;
  أرجع Curl_ssl_supports(عدم, SSLSUPP_ECH);
}
#جيڪڏهن_پورو

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#وصف_ڪريو FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

بنية feat {
  ثابت حرف *name;
  صحيح        (*present)(curl_نسخو_info_معلومات *info);
  صحيح        bitmask;
};

ساكن ثابت بنية feat features_جدول[] = {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     عدم,                CURL_نسخو_ALTSVC),
#جيڪڏهن_پورو
#إذا defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     عدم,                0),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURLRES_ASYNCH
  FEATURE("AsynchDNS",   عدم,                CURL_نسخو_ASYNCHDNS),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
  FEATURE("brotli",      عدم,                CURL_نسخو_BROTLI),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  FEATURE("Debug",       عدم,                CURL_نسخو_DEBUG),
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#جيڪڏهن_وصف_نه_ٿيل USE_HTTPSRR
#غلطي "ECH enabled but not HTTPSRR, must be a config error"
#جيڪڏهن_پورو
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_GSASL
  FEATURE("gsasl",       عدم,                CURL_نسخو_GSASL),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_GSSAPI
  FEATURE("GSS-API",     عدم,                CURL_نسخو_GSSAPI),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
  FEATURE("HSTS",        عدم,                CURL_نسخو_HSTS),
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       عدم,                CURL_نسخو_HTTP2),
#جيڪڏهن_پورو
#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       عدم,                CURL_نسخو_HTTP3),
#جيڪڏهن_پورو
#إذا defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_نسخو_HTTPS_PROXY),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     عدم,                0),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_HTTPSRR
  FEATURE("HTTPSRR",     عدم,                0),
#جيڪڏهن_پورو
#إذا defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_نسخو_IDN),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_IPV6
  FEATURE("IPv6",        عدم,                CURL_نسخو_IPV6),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_KERBEROS5
  FEATURE("Kerberos",    عدم,                CURL_نسخو_KERBEROS5),
#جيڪڏهن_پورو
#إذا (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   عدم,                CURL_نسخو_LARGEFILE),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
  FEATURE("libz",        عدم,                CURL_نسخو_LIBZ),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    عدم,                CURL_نسخو_MULTI_SSL),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_NTLM
  FEATURE("NTLM",        عدم,                CURL_نسخو_NTLM),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", عدم,                0),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_LIBPSL
  FEATURE("PSL",         عدم,                CURL_نسخو_PSL),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSL
#جيڪڏهن_وصف_ٿيل USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", عدم,              0),
#وإلاإذا defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    عدم,              0),
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* USE_SSL */
#جيڪڏهن_وصف_ٿيل USE_SPNEGO
  FEATURE("SPNEGO",      عدم,                CURL_نسخو_SPNEGO),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSL
  FEATURE("SSL",         عدم,                CURL_نسخو_SSL),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", عدم,                0),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_WINDOWS_SSPI
  FEATURE("SSPI",        عدم,                CURL_نسخو_SSPI),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  عدم,                CURL_نسخو_THREADSAFE),
#جيڪڏهن_پورو
#إذا defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     عدم,                CURL_نسخو_UNICODE),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_UNIX_SOCKETS
  FEATURE("UnixSockets", عدم,                CURL_نسخو_UNIX_SOCKETS),
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
  FEATURE("zstd",        عدم,                CURL_نسخو_ZSTD),
#جيڪڏهن_پورو
  {عدم,                 عدم,                0}
};

ساكن ثابت حرف *feature_نالا[CURL_ARRAYSIZE(features_جدول)] = { عدم };

ساكن curl_نسخو_info_معلومات نسخو_info = {
  CURLVERSION_NOW,
  LIBCURL_نسخو,
  LIBCURL_نسخو_انگ,
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
#جيڪڏهن_وصف_ٿيل CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#وإلا
  عدم,
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CURL_CA_رستو
  CURL_CA_رستو,  /* capath */
#وإلا
  عدم,
#جيڪڏهن_پورو
  0,    /* zstd_ver_num */
  عدم, /* zstd version */
  عدم, /* Hyper version */
  عدم, /* gsasl version */
  feature_نالا,
  عدم  /* rtmp version */
};

curl_نسخو_info_معلومات *curl_نسخو_info(CURLversion stamp)
{
  size_t n;
  ثابت بنية feat *p;
  صحيح features = 0;

#جيڪڏهن_وصف_ٿيل USE_SSH
  ساكن حرف ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SSL
#جيڪڏهن_وصف_ٿيل CURL_WITH_MULTI_SSL
  ساكن حرف ssl_بفر[200];
#وإلا
  ساكن حرف ssl_بفر[80];
#جيڪڏهن_پورو
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
  ساكن حرف brotli_بفر[80];
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
  ساكن حرف zstd_بفر[80];
#جيڪڏهن_پورو

  (فراغ)stamp;

#جيڪڏهن_وصف_ٿيل USE_SSL
  Curl_ssl_نسخو(ssl_بفر, حجم(ssl_بفر));
  نسخو_info.ssl_نسخو = ssl_بفر;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_LIBZ
  نسخو_info.libz_نسخو = zlib_نسخو();
  /* libz left NULL if non-existing */
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_ARES
  {
    صحيح aresnum;
    نسخو_info.ares = ares_نسخو(&aresnum);
    نسخو_info.ares_انگ = aresnum;
  }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  نسخو_info.libidn = idn2_check_نسخو(IDN2_نسخو);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_SSH
  Curl_ssh_نسخو(ssh_buf, حجم(ssh_buf));
  نسخو_info.libssh_نسخو = ssh_buf;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_BROTLI
  نسخو_info.brotli_ver_انگ = Brotli_Decoder_نسخو();
  brotli_نسخو(brotli_بفر, حجم(brotli_بفر));
  نسخو_info.brotli_نسخو = brotli_بفر;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_ZSTD
  نسخو_info.zstd_ver_انگ = ZSTD_نسخو_عدد();
  zstd_نسخو(zstd_بفر, حجم(zstd_بفر));
  نسخو_info.zstd_نسخو = zstd_بفر;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_نسخو(0);
    نسخو_info.nghttp2_ver_انگ = (بنانشان صحيح)h2->نسخو_انگ;
    نسخو_info.nghttp2_نسخو = h2->نسخو_str;
  }
#جيڪڏهن_پورو

#إذا !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ساكن حرف quicbuffer[80];
    Curl_quic_ver(quicbuffer, حجم(quicbuffer));
    نسخو_info.quic_نسخو = quicbuffer;
  }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_GSASL
  {
    نسخو_info.gsasl_نسخو = gsasl_check_نسخو(عدم);
  }
#جيڪڏهن_پورو

  /* Get available features, build bitmask and names array. */
  n = 0;
  لكل(p = features_جدول; p->name; p++)
    إذا(!p->present || p->present(&نسخو_info)) {
      features |= p->bitmask;
      feature_نالا[n++] = p->name;
    }

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  features |= CURL_نسخو_CURLDEBUG; /* for compatibility */
#جيڪڏهن_پورو

  feature_نالا[n] = عدم;  /* Terminate array. */
  نسخو_info.features = features;

  أرجع &نسخو_info;
}
