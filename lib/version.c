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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_NGHTTP2
#অন্তর্ভাব <nghttp2/nghttp2.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "http2.h"
#অন্তর্ভাব "vssh/ssh.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#অন্তর্ভাব <idn2.h>
#endif

#ifdef USE_LIBPSL
#অন্তর্ভাব <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#অন্তর্ভাব <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wvla"
#endif
#অন্তর্ভাব <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic বের_করো
#endif
#endif

#ifdef HAVE_ZSTD
#অন্তর্ভাব <zstd.h>
#endif

#ifdef USE_GSASL
#অন্তর্ভাব <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#অন্তর্ভাব "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
স্থির শূন্য brotli_version(অক্ষর *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned পূর্ণসংখ্যা major = brotli_version >> 24;
  unsigned পূর্ণসংখ্যা minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned পূর্ণসংখ্যা patch = brotli_version & 0x00000FFF;
  (শূন্য)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
স্থির শূন্য zstd_version(অক্ষর *buf, size_t bufsz)
{
  unsigned পূর্ণসংখ্যা version = ZSTD_version_সংখ্যা();
  unsigned পূর্ণসংখ্যা major = version / (100 * 100);
  unsigned পূর্ণসংখ্যা minor = (version - (major * 100 * 100)) / 100;
  unsigned পূর্ণসংখ্যা patch = version - (major * 100 * 100) - (minor * 100);
  (শূন্য)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
স্থির শূন্য psl_version(অক্ষর *buf, size_t bufsz)
{
#যদি defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  পূর্ণসংখ্যা অঙ্ক = psl_check_version_সংখ্যা(0);
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
স্থির শূন্য idn_version(অক্ষর *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(ফাঁকা));
#নতুবা_যদি defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#নতুবা_যদি defined(USE_APPLE_IDN)
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

অক্ষর *curl_version(শূন্য)
{
  স্থির অক্ষর out[300];
  অক্ষর *outp;
  size_t outlen;
  ধ্রুবক অক্ষর *src[VERSION_PARTS];
#ifdef USE_SSL
  অক্ষর ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  অক্ষর z_version[30];
#endif
#ifdef HAVE_BROTLI
  অক্ষর br_version[30];
#endif
#ifdef HAVE_ZSTD
  অক্ষর zstd_ver[30];
#endif
#ifdef USE_ARES
  অক্ষর cares_version[30];
#endif
#ifdef USE_IDN
  অক্ষর idn_ver[30];
#endif
#ifdef USE_LIBPSL
  অক্ষর psl_ver[30];
#endif
#ifdef USE_SSH
  অক্ষর ssh_version[30];
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  অক্ষর h2_version[30];
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  অক্ষর h3_version[30];
#endif
#ifdef USE_GSASL
  অক্ষর gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  অক্ষর gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  অক্ষর ldap_buf[30];
#endif
  পূর্ণসংখ্যা i = 0;
  পূর্ণসংখ্যা j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  ধ্রুবক অক্ষর *debugversion = getenv("CURL_DEBUG_VERSION");
  যদি(debugversion) {
    curl_msnprintf(out, আকার(out), "%s", debugversion);
    ফেরত out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, আকার(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, আকার(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, আকার(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, আকার(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, আকার(cares_version),
                 "c-ares/%s", ares_version(ফাঁকা));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, আকার(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, আকার(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, আকার(ssh_version));
  src[i++] = ssh_version;
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, আকার(h2_version));
  src[i++] = h2_version;
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, আকার(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, আকার(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(ফাঁকা));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, আকার(gss_buf), "AppleGSS");
#নতুবা_যদি defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, আকার(gss_buf), "libgss/%s", GSS_VERSION);
#নতুবা_যদি defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, আকার(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#অন্যথা
  curl_msnprintf(gss_buf, আকার(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, আকার(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = আকার(out);
  জন্য(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    যদি(outlen <= (n + 2))
      বিরতি;
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

  ফেরত out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

স্থির ধ্রুবক অক্ষর * ধ্রুবক supported_protocols[] = {
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

  ফাঁকা
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#যদি defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
স্থির পূর্ণসংখ্যা idn_present(curl_version_info_তথ্য *info)
{
#যদি defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (শূন্য)info;
  ফেরত TRUE;
#অন্যথা
  ফেরত !!info->libidn;
#endif
}
#endif

#যদি defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
স্থির পূর্ণসংখ্যা https_proxy_present(curl_version_info_তথ্য *info)
{
  (শূন্য)info;
  ফেরত Curl_ssl_supports(ফাঁকা, SSLSUPP_HTTPS_PROXY);
}
#endif

#যদি defined(USE_SSL) && defined(USE_ECH)
স্থির পূর্ণসংখ্যা ech_present(curl_version_info_তথ্য *info)
{
  (শূন্য)info;
  ফেরত Curl_ssl_supports(ফাঁকা, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

কাঠামো feat {
  ধ্রুবক অক্ষর *name;
  পূর্ণসংখ্যা        (*present)(curl_version_info_তথ্য *info);
  পূর্ণসংখ্যা        bitmask;
};

স্থির ধ্রুবক কাঠামো feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     ফাঁকা,                CURL_VERSION_ALTSVC),
#endif
#যদি defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     ফাঁকা,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   ফাঁকা,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      ফাঁকা,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       ফাঁকা,                CURL_VERSION_DEBUG),
#endif
#যদি defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       ফাঁকা,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     ফাঁকা,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        ফাঁকা,                CURL_VERSION_HSTS),
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       ফাঁকা,                CURL_VERSION_HTTP2),
#endif
#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       ফাঁকা,                CURL_VERSION_HTTP3),
#endif
#যদি defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     ফাঁকা,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     ফাঁকা,                0),
#endif
#যদি defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        ফাঁকা,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    ফাঁকা,                CURL_VERSION_KERBEROS5),
#endif
#যদি (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   ফাঁকা,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        ফাঁকা,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    ফাঁকা,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        ফাঁকা,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", ফাঁকা,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         ফাঁকা,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", ফাঁকা,              0),
#নতুবা_যদি defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    ফাঁকা,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      ফাঁকা,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         ফাঁকা,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", ফাঁকা,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        ফাঁকা,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  ফাঁকা,                CURL_VERSION_THREADSAFE),
#endif
#যদি defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     ফাঁকা,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", ফাঁকা,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        ফাঁকা,                CURL_VERSION_ZSTD),
#endif
  {ফাঁকা,                 ফাঁকা,                0}
};

স্থির ধ্রুবক অক্ষর *feature_names[CURL_ARRAYSIZE(features_table)] = { ফাঁকা };

স্থির curl_version_info_তথ্য version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_অঙ্ক,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  ফাঁকা, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  ফাঁকা, /* zlib_version */
  supported_protocols,
  ফাঁকা, /* c-ares version */
  0,    /* c-ares version numerical */
  ফাঁকা, /* libidn version */
  0,    /* iconv version */
  ফাঁকা, /* ssh lib version */
  0,    /* brotli_ver_num */
  ফাঁকা, /* brotli version */
  0,    /* nghttp2 version number */
  ফাঁকা, /* nghttp2 version string */
  ফাঁকা, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#অন্যথা
  ফাঁকা,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#অন্যথা
  ফাঁকা,
#endif
  0,    /* zstd_ver_num */
  ফাঁকা, /* zstd version */
  ফাঁকা, /* Hyper version */
  ফাঁকা, /* gsasl version */
  feature_names,
  ফাঁকা  /* rtmp version */
};

curl_version_info_তথ্য *curl_version_info(CURLversion stamp)
{
  size_t n;
  ধ্রুবক কাঠামো feat *p;
  পূর্ণসংখ্যা features = 0;

#ifdef USE_SSH
  স্থির অক্ষর ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  স্থির অক্ষর ssl_বাফার[200];
#অন্যথা
  স্থির অক্ষর ssl_বাফার[80];
#endif
#endif
#ifdef HAVE_BROTLI
  স্থির অক্ষর brotli_বাফার[80];
#endif
#ifdef HAVE_ZSTD
  স্থির অক্ষর zstd_বাফার[80];
#endif

  (শূন্য)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_বাফার, আকার(ssl_বাফার));
  version_info.ssl_version = ssl_বাফার;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    পূর্ণসংখ্যা aresnum;
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
  Curl_ssh_version(ssh_buf, আকার(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_অঙ্ক = BrotliDecoderVersion();
  brotli_version(brotli_বাফার, আকার(brotli_বাফার));
  version_info.brotli_version = brotli_বাফার;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_অঙ্ক = ZSTD_version_সংখ্যা();
  zstd_version(zstd_বাফার, আকার(zstd_বাফার));
  version_info.zstd_version = zstd_বাফার;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_অঙ্ক = (unsigned পূর্ণসংখ্যা)h2->version_অঙ্ক;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#যদি !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    স্থির অক্ষর quicbuffer[80];
    Curl_quic_ver(quicbuffer, আকার(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(ফাঁকা);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  জন্য(p = features_table; p->name; p++)
    যদি(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = ফাঁকা;  /* Terminate array. */
  version_info.features = features;

  ফেরত &version_info;
}
