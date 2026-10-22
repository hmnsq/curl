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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_NGHTTP2
#ଅନ୍ତର୍ଭାଵ <nghttp2/nghttp2.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "http2.h"
#ଅନ୍ତର୍ଭାଵ "vssh/ssh.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#ଅନ୍ତର୍ଭାଵ <idn2.h>
#endif

#ifdef USE_LIBPSL
#ଅନ୍ତର୍ଭାଵ <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#ଅନ୍ତର୍ଭାଵ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Wvla"
#endif
#ଅନ୍ତର୍ଭାଵ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic କାଢ଼
#endif
#endif

#ifdef HAVE_ZSTD
#ଅନ୍ତର୍ଭାଵ <zstd.h>
#endif

#ifdef USE_GSASL
#ଅନ୍ତର୍ଭାଵ <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#ଅନ୍ତର୍ଭାଵ "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
ସ୍ଥିର ଶୂନ୍ୟ brotli_version(ଅକ୍ଷର *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା major = brotli_version >> 24;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା patch = brotli_version & 0x00000FFF;
  (ଶୂନ୍ୟ)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
ସ୍ଥିର ଶୂନ୍ୟ zstd_version(ଅକ୍ଷର *buf, size_t bufsz)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା version = ZSTD_version_ସଂଖ୍ୟା();
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା major = version / (100 * 100);
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା minor = (version - (major * 100 * 100)) / 100;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା patch = version - (major * 100 * 100) - (minor * 100);
  (ଶୂନ୍ୟ)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
ସ୍ଥିର ଶୂନ୍ୟ psl_version(ଅକ୍ଷର *buf, size_t bufsz)
{
#ଯଦି defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ = psl_check_version_ସଂଖ୍ୟା(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 ଅଙ୍କ >> 16, (ଅଙ୍କ >> 8) & 0xff, ଅଙ୍କ & 0xff);
#ଅନ୍ୟଥା
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#ଯଦି defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
ସ୍ଥିର ଶୂନ୍ୟ idn_version(ଅକ୍ଷର *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(ଖାଲି));
#ନଚେତଯଦି defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#ନଚେତଯଦି defined(USE_APPLE_IDN)
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

ଅକ୍ଷର *curl_version(ଶୂନ୍ୟ)
{
  ସ୍ଥିର ଅକ୍ଷର out[300];
  ଅକ୍ଷର *outp;
  size_t outlen;
  ଅଚଳ ଅକ୍ଷର *src[VERSION_PARTS];
#ifdef USE_SSL
  ଅକ୍ଷର ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  ଅକ୍ଷର z_version[30];
#endif
#ifdef HAVE_BROTLI
  ଅକ୍ଷର br_version[30];
#endif
#ifdef HAVE_ZSTD
  ଅକ୍ଷର zstd_ver[30];
#endif
#ifdef USE_ARES
  ଅକ୍ଷର cares_version[30];
#endif
#ifdef USE_IDN
  ଅକ୍ଷର idn_ver[30];
#endif
#ifdef USE_LIBPSL
  ଅକ୍ଷର psl_ver[30];
#endif
#ifdef USE_SSH
  ଅକ୍ଷର ssh_version[30];
#endif
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  ଅକ୍ଷର h2_version[30];
#endif
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  ଅକ୍ଷର h3_version[30];
#endif
#ifdef USE_GSASL
  ଅକ୍ଷର gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  ଅକ୍ଷର gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  ଅକ୍ଷର ldap_buf[30];
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  ଅଚଳ ଅକ୍ଷର *debugversion = getenv("CURL_DEBUG_VERSION");
  ଯଦି(debugversion) {
    curl_msnprintf(out, ଆକାର(out), "%s", debugversion);
    ଫେରନ୍ତୁ out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, ଆକାର(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, ଆକାର(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, ଆକାର(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, ଆକାର(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, ଆକାର(cares_version),
                 "c-ares/%s", ares_version(ଖାଲି));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, ଆକାର(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, ଆକାର(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, ଆକାର(ssh_version));
  src[i++] = ssh_version;
#endif
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, ଆକାର(h2_version));
  src[i++] = h2_version;
#endif
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, ଆକାର(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, ଆକାର(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(ଖାଲି));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, ଆକାର(gss_buf), "AppleGSS");
#ନଚେତଯଦି defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, ଆକାର(gss_buf), "libgss/%s", GSS_VERSION);
#ନଚେତଯଦି defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, ଆକାର(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#ଅନ୍ୟଥା
  curl_msnprintf(gss_buf, ଆକାର(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, ଆକାର(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = ଆକାର(out);
  ପାଇଁ(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    ଯଦି(outlen <= (n + 2))
      ଭାଙ୍ଗ;
    ଯଦି(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  ଫେରନ୍ତୁ out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#ଯଦି !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  ଖାଲି
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#ଯଦି defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା idn_present(curl_version_info_data *info)
{
#ଯଦି defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (ଶୂନ୍ୟ)info;
  ଫେରନ୍ତୁ TRUE;
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ !!info->libidn;
#endif
}
#endif

#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା https_proxy_present(curl_version_info_data *info)
{
  (ଶୂନ୍ୟ)info;
  ଫେରନ୍ତୁ Curl_ssl_supports(ଖାଲି, SSLSUPP_HTTPS_PROXY);
}
#endif

#ଯଦି defined(USE_SSL) && defined(USE_ECH)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ech_present(curl_version_info_data *info)
{
  (ଶୂନ୍ୟ)info;
  ଫେରନ୍ତୁ Curl_ssl_supports(ଖାଲି, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

ସଂରଚନା feat {
  ଅଚଳ ଅକ୍ଷର *name;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା        (*present)(curl_version_info_data *info);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା        bitmask;
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     ଖାଲି,                CURL_VERSION_ALTSVC),
#endif
#ଯଦି defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     ଖାଲି,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   ଖାଲି,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      ଖାଲି,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       ଖାଲି,                CURL_VERSION_DEBUG),
#endif
#ଯଦି defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       ଖାଲି,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     ଖାଲି,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        ଖାଲି,                CURL_VERSION_HSTS),
#endif
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       ଖାଲି,                CURL_VERSION_HTTP2),
#endif
#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       ଖାଲି,                CURL_VERSION_HTTP3),
#endif
#ଯଦି defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     ଖାଲି,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     ଖାଲି,                0),
#endif
#ଯଦି defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        ଖାଲି,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    ଖାଲି,                CURL_VERSION_KERBEROS5),
#endif
#ଯଦି (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   ଖାଲି,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        ଖାଲି,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    ଖାଲି,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        ଖାଲି,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", ଖାଲି,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         ଖାଲି,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", ଖାଲି,              0),
#ନଚେତଯଦି defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    ଖାଲି,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      ଖାଲି,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         ଖାଲି,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", ଖାଲି,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        ଖାଲି,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  ଖାଲି,                CURL_VERSION_THREADSAFE),
#endif
#ଯଦି defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     ଖାଲି,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", ଖାଲି,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        ଖାଲି,                CURL_VERSION_ZSTD),
#endif
  {ଖାଲି,                 ଖାଲି,                0}
};

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *feature_names[CURL_ARRAYSIZE(features_table)] = { ଖାଲି };

ସ୍ଥିର curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_ଅଙ୍କ,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  ଖାଲି, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  ଖାଲି, /* zlib_version */
  supported_protocols,
  ଖାଲି, /* c-ares version */
  0,    /* c-ares version numerical */
  ଖାଲି, /* libidn version */
  0,    /* iconv version */
  ଖାଲି, /* ssh lib version */
  0,    /* brotli_ver_num */
  ଖାଲି, /* brotli version */
  0,    /* nghttp2 version number */
  ଖାଲି, /* nghttp2 version string */
  ଖାଲି, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#ଅନ୍ୟଥା
  ଖାଲି,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#ଅନ୍ୟଥା
  ଖାଲି,
#endif
  0,    /* zstd_ver_num */
  ଖାଲି, /* zstd version */
  ଖାଲି, /* Hyper version */
  ଖାଲି, /* gsasl version */
  feature_names,
  ଖାଲି  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  ଅଚଳ ସଂରଚନା feat *p;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା features = 0;

#ifdef USE_SSH
  ସ୍ଥିର ଅକ୍ଷର ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  ସ୍ଥିର ଅକ୍ଷର ssl_buffer[200];
#ଅନ୍ୟଥା
  ସ୍ଥିର ଅକ୍ଷର ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  ସ୍ଥିର ଅକ୍ଷର brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  ସ୍ଥିର ଅକ୍ଷର zstd_buffer[80];
#endif

  (ଶୂନ୍ୟ)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, ଆକାର(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_ଅଙ୍କ = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, ଆକାର(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_ଅଙ୍କ = BrotliDecoderVersion();
  brotli_version(brotli_buffer, ଆକାର(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_ଅଙ୍କ = ZSTD_version_ସଂଖ୍ୟା();
  zstd_version(zstd_buffer, ଆକାର(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_ଅଙ୍କ = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)h2->version_ଅଙ୍କ;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#ଯଦି !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ସ୍ଥିର ଅକ୍ଷର quicbuffer[80];
    Curl_quic_ver(quicbuffer, ଆକାର(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(ଖାଲି);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  ପାଇଁ(p = features_table; p->name; p++)
    ଯଦି(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = ଖାଲି;  /* Terminate array. */
  version_info.features = features;

  ଫେରନ୍ତୁ &version_info;
}
