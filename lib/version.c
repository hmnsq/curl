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

#ifdef USE_NGHTTP2
#شامل <nghttp2/nghttp2.h>
#endif

#شامل "urldata.h"
#شامل "vtls/vtls.h"
#شامل "http2.h"
#شامل "vssh/ssh.h"
#شامل "vquic/vquic.h"
#شامل "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#شامل <idn2.h>
#endif

#ifdef USE_LIBPSL
#شامل <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#شامل <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wvla"
#endif
#شامل <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic برداشت
#endif
#endif

#ifdef HAVE_ZSTD
#شامل <zstd.h>
#endif

#ifdef USE_GSASL
#شامل <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#شامل "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
ایستا تهی brotli_version(نویسه *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned صحیح major = brotli_version >> 24;
  unsigned صحیح minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned صحیح patch = brotli_version & 0x00000FFF;
  (تهی)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
ایستا تهی zstd_version(نویسه *buf, size_t bufsz)
{
  unsigned صحیح version = ZSTD_version_عدد();
  unsigned صحیح major = version / (100 * 100);
  unsigned صحیح minor = (version - (major * 100 * 100)) / 100;
  unsigned صحیح patch = version - (major * 100 * 100) - (minor * 100);
  (تهی)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
ایستا تهی psl_version(نویسه *buf, size_t bufsz)
{
#اگر defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  صحیح رقم = psl_check_version_عدد(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 رقم >> 16, (رقم >> 8) & 0xff, رقم & 0xff);
#وگرنه
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#اگر defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
ایستا تهی idn_version(نویسه *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(پوچ));
#وگرنهاگر defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#وگرنهاگر defined(USE_APPLE_IDN)
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

نویسه *curl_version(تهی)
{
  ایستا نویسه out[300];
  نویسه *outp;
  size_t outlen;
  ثابت نویسه *src[VERSION_PARTS];
#ifdef USE_SSL
  نویسه ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  نویسه z_version[30];
#endif
#ifdef HAVE_BROTLI
  نویسه br_version[30];
#endif
#ifdef HAVE_ZSTD
  نویسه zstd_ver[30];
#endif
#ifdef USE_ARES
  نویسه cares_version[30];
#endif
#ifdef USE_IDN
  نویسه idn_ver[30];
#endif
#ifdef USE_LIBPSL
  نویسه psl_ver[30];
#endif
#ifdef USE_SSH
  نویسه ssh_version[30];
#endif
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  نویسه h2_version[30];
#endif
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  نویسه h3_version[30];
#endif
#ifdef USE_GSASL
  نویسه gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  نویسه gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  نویسه ldap_buf[30];
#endif
  صحیح i = 0;
  صحیح j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  ثابت نویسه *debugversion = getenv("CURL_DEBUG_VERSION");
  اگر(debugversion) {
    curl_msnprintf(out, اندازه(out), "%s", debugversion);
    بازگشت out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, اندازه(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, اندازه(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, اندازه(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, اندازه(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, اندازه(cares_version),
                 "c-ares/%s", ares_version(پوچ));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, اندازه(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, اندازه(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, اندازه(ssh_version));
  src[i++] = ssh_version;
#endif
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, اندازه(h2_version));
  src[i++] = h2_version;
#endif
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, اندازه(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, اندازه(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(پوچ));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, اندازه(gss_buf), "AppleGSS");
#وگرنهاگر defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, اندازه(gss_buf), "libgss/%s", GSS_VERSION);
#وگرنهاگر defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, اندازه(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#وگرنه
  curl_msnprintf(gss_buf, اندازه(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, اندازه(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = اندازه(out);
  برای(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    اگر(outlen <= (n + 2))
      بشکن;
    اگر(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  بازگشت out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ایستا ثابت نویسه * ثابت supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#اگر !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  پوچ
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#اگر defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ایستا صحیح idn_present(curl_version_info_data *info)
{
#اگر defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (تهی)info;
  بازگشت TRUE;
#وگرنه
  بازگشت !!info->libidn;
#endif
}
#endif

#اگر defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ایستا صحیح https_proxy_present(curl_version_info_data *info)
{
  (تهی)info;
  بازگشت Curl_ssl_supports(پوچ, SSLSUPP_HTTPS_PROXY);
}
#endif

#اگر defined(USE_SSL) && defined(USE_ECH)
ایستا صحیح ech_present(curl_version_info_data *info)
{
  (تهی)info;
  بازگشت Curl_ssl_supports(پوچ, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

ساختار feat {
  ثابت نویسه *name;
  صحیح        (*present)(curl_version_info_data *info);
  صحیح        bitmask;
};

ایستا ثابت ساختار feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     پوچ,                CURL_VERSION_ALTSVC),
#endif
#اگر defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     پوچ,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   پوچ,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      پوچ,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       پوچ,                CURL_VERSION_DEBUG),
#endif
#اگر defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       پوچ,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     پوچ,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        پوچ,                CURL_VERSION_HSTS),
#endif
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       پوچ,                CURL_VERSION_HTTP2),
#endif
#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       پوچ,                CURL_VERSION_HTTP3),
#endif
#اگر defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     پوچ,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     پوچ,                0),
#endif
#اگر defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        پوچ,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    پوچ,                CURL_VERSION_KERBEROS5),
#endif
#اگر (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   پوچ,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        پوچ,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    پوچ,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        پوچ,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", پوچ,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         پوچ,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", پوچ,              0),
#وگرنهاگر defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    پوچ,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      پوچ,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         پوچ,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", پوچ,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        پوچ,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  پوچ,                CURL_VERSION_THREADSAFE),
#endif
#اگر defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     پوچ,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", پوچ,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        پوچ,                CURL_VERSION_ZSTD),
#endif
  {پوچ,                 پوچ,                0}
};

ایستا ثابت نویسه *feature_names[CURL_ARRAYSIZE(features_table)] = { پوچ };

ایستا curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_رقم,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  پوچ, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  پوچ, /* zlib_version */
  supported_protocols,
  پوچ, /* c-ares version */
  0,    /* c-ares version numerical */
  پوچ, /* libidn version */
  0,    /* iconv version */
  پوچ, /* ssh lib version */
  0,    /* brotli_ver_num */
  پوچ, /* brotli version */
  0,    /* nghttp2 version number */
  پوچ, /* nghttp2 version string */
  پوچ, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#وگرنه
  پوچ,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#وگرنه
  پوچ,
#endif
  0,    /* zstd_ver_num */
  پوچ, /* zstd version */
  پوچ, /* Hyper version */
  پوچ, /* gsasl version */
  feature_names,
  پوچ  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  ثابت ساختار feat *p;
  صحیح features = 0;

#ifdef USE_SSH
  ایستا نویسه ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  ایستا نویسه ssl_buffer[200];
#وگرنه
  ایستا نویسه ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  ایستا نویسه brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  ایستا نویسه zstd_buffer[80];
#endif

  (تهی)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, اندازه(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    صحیح aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_رقم = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, اندازه(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_رقم = BrotliDecoderVersion();
  brotli_version(brotli_buffer, اندازه(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_رقم = ZSTD_version_عدد();
  zstd_version(zstd_buffer, اندازه(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_رقم = (unsigned صحیح)h2->version_رقم;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#اگر !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ایستا نویسه quicbuffer[80];
    Curl_quic_ver(quicbuffer, اندازه(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(پوچ);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  برای(p = features_table; p->name; p++)
    اگر(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = پوچ;  /* Terminate array. */
  version_info.features = features;

  بازگشت &version_info;
}
