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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_NGHTTP2
#شامل <nghttp2/nghttp2.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "vtls/vtls.h"
#شامل "http2.h"
#شامل "vssh/ssh.h"
#شامل "vquic/vquic.h"
#شامل "easy_lock.h"

#اگر_تعریف_شدہ USE_ARES
#  include <ares.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_LIBIDN2
#شامل <idn2.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_LIBPSL
#شامل <libpsl.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_LIBZ
#شامل <zlib.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_BROTLI
#اگر_تعریف_شدہ CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wvla"
#اگر_ختم
#شامل <brotli/decode.h>
#اگر_تعریف_شدہ CURL_HAVE_DIAG
#ہدایت GCC diagnostic نکالو
#اگر_ختم
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ZSTD
#شامل <zstd.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_GSASL
#شامل <gsasl.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_LDAP
#شامل "curl_ldap.h"
#اگر_ختم

#اگر_تعریف_شدہ HAVE_BROTLI
ساکن خالی brotli_نسخہ(حرف *buf, size_t bufsz)
{
  uint32_t brotli_نسخہ = Brotli_Decoder_نسخہ();
  غیرعلامتی صحیح major = brotli_نسخہ >> 24;
  غیرعلامتی صحیح minor = (brotli_نسخہ & 0x00FFFFFF) >> 12;
  غیرعلامتی صحیح patch = brotli_نسخہ & 0x00000FFF;
  (خالی)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ZSTD
ساکن خالی zstd_نسخہ(حرف *buf, size_t bufsz)
{
  غیرعلامتی صحیح نسخہ_2 = ZSTD_نسخہ_نمبر();
  غیرعلامتی صحیح major = نسخہ_2 / (100 * 100);
  غیرعلامتی صحیح minor = (نسخہ_2 - (major * 100 * 100)) / 100;
  غیرعلامتی صحیح patch = نسخہ_2 - (major * 100 * 100) - (minor * 100);
  (خالی)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#اگر_ختم

#اگر_تعریف_شدہ USE_LIBPSL
ساکن خالی psl_نسخہ(حرف *buf, size_t bufsz)
{
#اگر defined(PSL_نسخہ_MAJOR) && (PSL_نسخہ_MAJOR > 0 ||     \
                                   PSL_نسخہ_MINOR >= 11)
  صحیح عدد = psl_check_نسخہ_نمبر(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 عدد >> 16, (عدد >> 8) & 0xff, عدد & 0xff);
#ورنہ
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_نسخہ());
#اگر_ختم
}
#اگر_ختم

#اگر defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#تعریف_کرو USE_IDN
#اگر_ختم

#اگر_تعریف_شدہ USE_IDN
ساکن خالی idn_نسخہ(حرف *buf, size_t bufsz)
{
#اگر_تعریف_شدہ USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_نسخہ(تہی));
#وگرنہ_اگر defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#وگرنہ_اگر defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#اگر_ختم
}
#اگر_ختم

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#تعریف_کرو نسخہ_PARTS 16 /* number of substrings we can concatenate */

حرف *curl_نسخہ(خالی)
{
  ساکن حرف برآمد[300];
  حرف *outp;
  size_t outlen;
  ثابت حرف *src[نسخہ_PARTS];
#اگر_تعریف_شدہ USE_SSL
  حرف ssl_نسخہ[200];
#اگر_ختم
#اگر_تعریف_شدہ HAVE_LIBZ
  حرف z_نسخہ[30];
#اگر_ختم
#اگر_تعریف_شدہ HAVE_BROTLI
  حرف br_نسخہ[30];
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ZSTD
  حرف zstd_ver[30];
#اگر_ختم
#اگر_تعریف_شدہ USE_ARES
  حرف cares_نسخہ[30];
#اگر_ختم
#اگر_تعریف_شدہ USE_IDN
  حرف idn_ver[30];
#اگر_ختم
#اگر_تعریف_شدہ USE_LIBPSL
  حرف psl_ver[30];
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
  حرف ssh_نسخہ[30];
#اگر_ختم
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  حرف h2_نسخہ[30];
#اگر_ختم
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  حرف h3_نسخہ[30];
#اگر_ختم
#اگر_تعریف_شدہ USE_GSASL
  حرف gsasl_buf[30];
#اگر_ختم
#اگر_تعریف_شدہ HAVE_GSSAPI
  حرف gss_buf[40];
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_LDAP
  حرف ldap_buf[30];
#اگر_ختم
  صحیح i = 0;
  صحیح j;

#اگر_تعریف_شدہ DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
سیٹ*/
  ثابت حرف *debugversion = getenv("CURL_DEBUG_VERSION");
  اگر(debugversion) {
    curl_msnprintf(برآمد, حجم(برآمد), "%s", debugversion);
    واپس برآمد;
  }
#اگر_ختم

  src[i++] = LIBCURL_NAME "/" LIBCURL_نسخہ;
#اگر_تعریف_شدہ USE_SSL
  Curl_ssl_نسخہ(ssl_نسخہ, حجم(ssl_نسخہ));
  src[i++] = ssl_نسخہ;
#اگر_ختم
#اگر_تعریف_شدہ HAVE_LIBZ
  curl_msnprintf(z_نسخہ, حجم(z_نسخہ), "zlib/%s", zlib_نسخہ());
  src[i++] = z_نسخہ;
#اگر_ختم
#اگر_تعریف_شدہ HAVE_BROTLI
  brotli_نسخہ(br_نسخہ, حجم(br_نسخہ));
  src[i++] = br_نسخہ;
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ZSTD
  zstd_نسخہ(zstd_ver, حجم(zstd_ver));
  src[i++] = zstd_ver;
#اگر_ختم
#اگر_تعریف_شدہ USE_ARES
  curl_msnprintf(cares_نسخہ, حجم(cares_نسخہ),
                 "c-ares/%s", ares_نسخہ(تہی));
  src[i++] = cares_نسخہ;
#اگر_ختم
#اگر_تعریف_شدہ USE_IDN
  idn_نسخہ(idn_ver, حجم(idn_ver));
  src[i++] = idn_ver;
#اگر_ختم
#اگر_تعریف_شدہ USE_LIBPSL
  psl_نسخہ(psl_ver, حجم(psl_ver));
  src[i++] = psl_ver;
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
  Curl_ssh_نسخہ(ssh_نسخہ, حجم(ssh_نسخہ));
  src[i++] = ssh_نسخہ;
#اگر_ختم
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_نسخہ, حجم(h2_نسخہ));
  src[i++] = h2_نسخہ;
#اگر_ختم
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_نسخہ, حجم(h3_نسخہ));
  src[i++] = h3_نسخہ;
#اگر_ختم
#اگر_تعریف_شدہ USE_GSASL
  curl_msnprintf(gsasl_buf, حجم(gsasl_buf), "libgsasl/%s",
                 gsasl_check_نسخہ(تہی));
  src[i++] = gsasl_buf;
#اگر_ختم
#اگر_تعریف_شدہ HAVE_GSSAPI
#اگر_تعریف_شدہ HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, حجم(gss_buf), "AppleGSS");
#وگرنہ_اگر defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, حجم(gss_buf), "libgss/%s", GSS_نسخہ);
#وگرنہ_اگر defined(CURL_KRB5_نسخہ)
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5/%s", CURL_KRB5_نسخہ);
#ورنہ
  curl_msnprintf(gss_buf, حجم(gss_buf), "mit-krb5");
#اگر_ختم
  src[i++] = gss_buf;
#اگر_ختم /* HAVE_GSSAPI */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_LDAP
  Curl_ldap_نسخہ(ldap_buf, حجم(ldap_buf));
  src[i++] = ldap_buf;
#اگر_ختم

  DEBUGASSERT(i <= نسخہ_PARTS);

  outp = &برآمد[0];
  outlen = حجم(برآمد);
  برائے(j = 0; j < i; j++) {
    size_t n = لڑی_لمبائی(src[j]);
    /* we need room for a space, the string and the final zero */
    اگر(outlen <= (n + 2))
      توڑو;
    اگر(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    حافظہ_نقل(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  واپس برآمد;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ساکن ثابت حرف * ثابت supported_protocols[] = {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_DICT
  "dict",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FILE
  "file",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP
  "ftp",
#اگر_ختم
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_GOPHER
  "gopher",
#اگر_ختم
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  "http",
#اگر_ختم
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_IMAP
  "imap",
#اگر_ختم
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_LDAP
  "ldap",
#اگر !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#اگر_ختم
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_MQTT
  "mqtt",
#اگر_ختم
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_POP3
  "pop3",
#اگر_ختم
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
  "rtsp",
#اگر_ختم
#اگر_تعریف_شدہ USE_SSH
  "scp",
  "sftp",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_SMTP
  "smtp",
#اگر_ختم
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_TELNET
  "telnet",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_TFTP
  "tftp",
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
  "ws",
#اگر_ختم
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#اگر_ختم
#اگر_ختم

  تہی
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#اگر defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ساکن صحیح idn_present(curl_نسخہ_info_ڈیٹا *info)
{
#اگر defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (خالی)info;
  واپس TRUE;
#ورنہ
  واپس !!info->libidn;
#اگر_ختم
}
#اگر_ختم

#اگر defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ساکن صحیح https_proxy_present(curl_نسخہ_info_ڈیٹا *info)
{
  (خالی)info;
  واپس Curl_ssl_supports(تہی, SSLSUPP_HTTPS_PROXY);
}
#اگر_ختم

#اگر defined(USE_SSL) && defined(USE_ECH)
ساکن صحیح ech_present(curl_نسخہ_info_ڈیٹا *info)
{
  (خالی)info;
  واپس Curl_ssl_supports(تہی, SSLSUPP_ECH);
}
#اگر_ختم

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#تعریف_کرو FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

ساخت feat {
  ثابت حرف *name;
  صحیح        (*present)(curl_نسخہ_info_ڈیٹا *info);
  صحیح        bitmask;
};

ساکن ثابت ساخت feat features_جدول[] = {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     تہی,                CURL_نسخہ_ALTSVC),
#اگر_ختم
#اگر defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     تہی,                0),
#اگر_ختم
#اگر_تعریف_شدہ CURLRES_ASYNCH
  FEATURE("AsynchDNS",   تہی,                CURL_نسخہ_ASYNCHDNS),
#اگر_ختم
#اگر_تعریف_شدہ HAVE_BROTLI
  FEATURE("brotli",      تہی,                CURL_نسخہ_BROTLI),
#اگر_ختم
#اگر_تعریف_شدہ DEBUGBUILD
  FEATURE("Debug",       تہی,                CURL_نسخہ_DEBUG),
#اگر_ختم
#اگر defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#اگر_غیر_تعریف_شدہ USE_HTTPSRR
#غلطی "ECH enabled but not HTTPSRR, must be a config error"
#اگر_ختم
#اگر_ختم
#اگر_تعریف_شدہ USE_GSASL
  FEATURE("gsasl",       تہی,                CURL_نسخہ_GSASL),
#اگر_ختم
#اگر_تعریف_شدہ HAVE_GSSAPI
  FEATURE("GSS-API",     تہی,                CURL_نسخہ_GSSAPI),
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
  FEATURE("HSTS",        تہی,                CURL_نسخہ_HSTS),
#اگر_ختم
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       تہی,                CURL_نسخہ_HTTP2),
#اگر_ختم
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       تہی,                CURL_نسخہ_HTTP3),
#اگر_ختم
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_نسخہ_HTTPS_PROXY),
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     تہی,                0),
#اگر_ختم
#اگر_تعریف_شدہ USE_HTTPSRR
  FEATURE("HTTPSRR",     تہی,                0),
#اگر_ختم
#اگر defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_نسخہ_IDN),
#اگر_ختم
#اگر_تعریف_شدہ USE_IPV6
  FEATURE("IPv6",        تہی,                CURL_نسخہ_IPV6),
#اگر_ختم
#اگر_تعریف_شدہ USE_KERBEROS5
  FEATURE("Kerberos",    تہی,                CURL_نسخہ_KERBEROS5),
#اگر_ختم
#اگر (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   تہی,                CURL_نسخہ_LARGEFILE),
#اگر_ختم
#اگر_تعریف_شدہ HAVE_LIBZ
  FEATURE("libz",        تہی,                CURL_نسخہ_LIBZ),
#اگر_ختم
#اگر_تعریف_شدہ CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    تہی,                CURL_نسخہ_MULTI_SSL),
#اگر_ختم
#اگر_تعریف_شدہ USE_NTLM
  FEATURE("NTLM",        تہی,                CURL_نسخہ_NTLM),
#اگر_ختم
#اگر_تعریف_شدہ USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", تہی,                0),
#اگر_ختم
#اگر_تعریف_شدہ USE_LIBPSL
  FEATURE("PSL",         تہی,                CURL_نسخہ_PSL),
#اگر_ختم
#اگر_تعریف_شدہ USE_SSL
#اگر_تعریف_شدہ USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", تہی,              0),
#وگرنہ_اگر defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    تہی,              0),
#اگر_ختم
#اگر_ختم /* USE_SSL */
#اگر_تعریف_شدہ USE_SPNEGO
  FEATURE("SPNEGO",      تہی,                CURL_نسخہ_SPNEGO),
#اگر_ختم
#اگر_تعریف_شدہ USE_SSL
  FEATURE("SSL",         تہی,                CURL_نسخہ_SSL),
#اگر_ختم
#اگر_تعریف_شدہ USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", تہی,                0),
#اگر_ختم
#اگر_تعریف_شدہ USE_WINDOWS_SSPI
  FEATURE("SSPI",        تہی,                CURL_نسخہ_SSPI),
#اگر_ختم
#اگر_تعریف_شدہ GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  تہی,                CURL_نسخہ_THREADSAFE),
#اگر_ختم
#اگر defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     تہی,                CURL_نسخہ_UNICODE),
#اگر_ختم
#اگر_تعریف_شدہ USE_UNIX_SOCKETS
  FEATURE("UnixSockets", تہی,                CURL_نسخہ_UNIX_SOCKETS),
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ZSTD
  FEATURE("zstd",        تہی,                CURL_نسخہ_ZSTD),
#اگر_ختم
  {تہی,                 تہی,                0}
};

ساکن ثابت حرف *feature_نام[CURL_ARRAYSIZE(features_جدول)] = { تہی };

ساکن curl_نسخہ_info_ڈیٹا نسخہ_info = {
  CURLVERSION_NOW,
  LIBCURL_نسخہ,
  LIBCURL_نسخہ_عدد,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  تہی, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  تہی, /* zlib_version */
  supported_protocols,
  تہی, /* c-ares version */
  0,    /* c-ares version numerical */
  تہی, /* libidn version */
  0,    /* iconv version */
  تہی, /* ssh lib version */
  0,    /* brotli_ver_num */
  تہی, /* brotli version */
  0,    /* nghttp2 version number */
  تہی, /* nghttp2 version string */
  تہی, /* quic library string */
#اگر_تعریف_شدہ CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#ورنہ
  تہی,
#اگر_ختم
#اگر_تعریف_شدہ CURL_CA_راستہ
  CURL_CA_راستہ,  /* capath */
#ورنہ
  تہی,
#اگر_ختم
  0,    /* zstd_ver_num */
  تہی, /* zstd version */
  تہی, /* Hyper version */
  تہی, /* gsasl version */
  feature_نام,
  تہی  /* rtmp version */
};

curl_نسخہ_info_ڈیٹا *curl_نسخہ_info(CURLversion stamp)
{
  size_t n;
  ثابت ساخت feat *p;
  صحیح features = 0;

#اگر_تعریف_شدہ USE_SSH
  ساکن حرف ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#اگر_ختم
#اگر_تعریف_شدہ USE_SSL
#اگر_تعریف_شدہ CURL_WITH_MULTI_SSL
  ساکن حرف ssl_بفر[200];
#ورنہ
  ساکن حرف ssl_بفر[80];
#اگر_ختم
#اگر_ختم
#اگر_تعریف_شدہ HAVE_BROTLI
  ساکن حرف brotli_بفر[80];
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ZSTD
  ساکن حرف zstd_بفر[80];
#اگر_ختم

  (خالی)stamp;

#اگر_تعریف_شدہ USE_SSL
  Curl_ssl_نسخہ(ssl_بفر, حجم(ssl_بفر));
  نسخہ_info.ssl_نسخہ = ssl_بفر;
#اگر_ختم

#اگر_تعریف_شدہ HAVE_LIBZ
  نسخہ_info.libz_نسخہ = zlib_نسخہ();
  /* libz left NULL if non-existing */
#اگر_ختم
#اگر_تعریف_شدہ USE_ARES
  {
    صحیح aresnum;
    نسخہ_info.ares = ares_نسخہ(&aresnum);
    نسخہ_info.ares_عدد = aresnum;
  }
#اگر_ختم
#اگر_تعریف_شدہ USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  نسخہ_info.libidn = idn2_check_نسخہ(IDN2_نسخہ);
#اگر_ختم

#اگر_تعریف_شدہ USE_SSH
  Curl_ssh_نسخہ(ssh_buf, حجم(ssh_buf));
  نسخہ_info.libssh_نسخہ = ssh_buf;
#اگر_ختم

#اگر_تعریف_شدہ HAVE_BROTLI
  نسخہ_info.brotli_ver_عدد = Brotli_Decoder_نسخہ();
  brotli_نسخہ(brotli_بفر, حجم(brotli_بفر));
  نسخہ_info.brotli_نسخہ = brotli_بفر;
#اگر_ختم

#اگر_تعریف_شدہ HAVE_ZSTD
  نسخہ_info.zstd_ver_عدد = ZSTD_نسخہ_نمبر();
  zstd_نسخہ(zstd_بفر, حجم(zstd_بفر));
  نسخہ_info.zstd_نسخہ = zstd_بفر;
#اگر_ختم

#اگر_تعریف_شدہ USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_نسخہ(0);
    نسخہ_info.nghttp2_ver_عدد = (غیرعلامتی صحیح)h2->نسخہ_عدد;
    نسخہ_info.nghttp2_نسخہ = h2->نسخہ_str;
  }
#اگر_ختم

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ساکن حرف quicbuffer[80];
    Curl_quic_ver(quicbuffer, حجم(quicbuffer));
    نسخہ_info.quic_نسخہ = quicbuffer;
  }
#اگر_ختم

#اگر_تعریف_شدہ USE_GSASL
  {
    نسخہ_info.gsasl_نسخہ = gsasl_check_نسخہ(تہی);
  }
#اگر_ختم

  /* Get available features, build bitmask and names array. */
  n = 0;
  برائے(p = features_جدول; p->name; p++)
    اگر(!p->present || p->present(&نسخہ_info)) {
      features |= p->bitmask;
      feature_نام[n++] = p->name;
    }

#اگر_تعریف_شدہ DEBUGBUILD
  features |= CURL_نسخہ_CURLDEBUG; /* for compatibility */
#اگر_ختم

  feature_نام[n] = تہی;  /* Terminate array. */
  نسخہ_info.features = features;

  واپس &نسخہ_info;
}
