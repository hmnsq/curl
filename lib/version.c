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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_NGHTTP2
#અન્તર્ભાવ <nghttp2/nghttp2.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "http2.h"
#અન્તર્ભાવ "vssh/ssh.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#અન્તર્ભાવ <idn2.h>
#endif

#ifdef USE_LIBPSL
#અન્તર્ભાવ <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#અન્તર્ભાવ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Wvla"
#endif
#અન્તર્ભાવ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic કાઢો
#endif
#endif

#ifdef HAVE_ZSTD
#અન્તર્ભાવ <zstd.h>
#endif

#ifdef USE_GSASL
#અન્તર્ભાવ <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#અન્તર્ભાવ "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
સ્થિર શૂન્ય brotli_version(અક્ષર *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned પૂર્ણાંક major = brotli_version >> 24;
  unsigned પૂર્ણાંક minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned પૂર્ણાંક patch = brotli_version & 0x00000FFF;
  (શૂન્ય)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
સ્થિર શૂન્ય zstd_version(અક્ષર *buf, size_t bufsz)
{
  unsigned પૂર્ણાંક version = ZSTD_version_સંખ્યા();
  unsigned પૂર્ણાંક major = version / (100 * 100);
  unsigned પૂર્ણાંક minor = (version - (major * 100 * 100)) / 100;
  unsigned પૂર્ણાંક patch = version - (major * 100 * 100) - (minor * 100);
  (શૂન્ય)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
સ્થિર શૂન્ય psl_version(અક્ષર *buf, size_t bufsz)
{
#જો defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  પૂર્ણાંક અંક = psl_check_version_સંખ્યા(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 અંક >> 16, (અંક >> 8) & 0xff, અંક & 0xff);
#નહીંતર
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#જો defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
સ્થિર શૂન્ય idn_version(અક્ષર *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(ખાલી));
#અથવાજો defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#અથવાજો defined(USE_APPLE_IDN)
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

અક્ષર *curl_version(શૂન્ય)
{
  સ્થિર અક્ષર out[300];
  અક્ષર *outp;
  size_t outlen;
  અચલ અક્ષર *src[VERSION_PARTS];
#ifdef USE_SSL
  અક્ષર ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  અક્ષર z_version[30];
#endif
#ifdef HAVE_BROTLI
  અક્ષર br_version[30];
#endif
#ifdef HAVE_ZSTD
  અક્ષર zstd_ver[30];
#endif
#ifdef USE_ARES
  અક્ષર cares_version[30];
#endif
#ifdef USE_IDN
  અક્ષર idn_ver[30];
#endif
#ifdef USE_LIBPSL
  અક્ષર psl_ver[30];
#endif
#ifdef USE_SSH
  અક્ષર ssh_version[30];
#endif
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  અક્ષર h2_version[30];
#endif
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  અક્ષર h3_version[30];
#endif
#ifdef USE_GSASL
  અક્ષર gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  અક્ષર gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  અક્ષર ldap_buf[30];
#endif
  પૂર્ણાંક i = 0;
  પૂર્ણાંક j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  અચલ અક્ષર *debugversion = getenv("CURL_DEBUG_VERSION");
  જો(debugversion) {
    curl_msnprintf(out, કદ(out), "%s", debugversion);
    પાછા out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, કદ(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, કદ(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, કદ(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, કદ(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, કદ(cares_version),
                 "c-ares/%s", ares_version(ખાલી));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, કદ(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, કદ(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, કદ(ssh_version));
  src[i++] = ssh_version;
#endif
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, કદ(h2_version));
  src[i++] = h2_version;
#endif
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, કદ(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, કદ(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(ખાલી));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, કદ(gss_buf), "AppleGSS");
#અથવાજો defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, કદ(gss_buf), "libgss/%s", GSS_VERSION);
#અથવાજો defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, કદ(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#નહીંતર
  curl_msnprintf(gss_buf, કદ(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, કદ(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = કદ(out);
  માટે(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    જો(outlen <= (n + 2))
      તોડો;
    જો(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  પાછા out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

સ્થિર અચલ અક્ષર * અચલ supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#જો defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#જો defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#જો defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#જો defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#જો !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#જો defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#જો defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#જો defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#જો defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  ખાલી
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#જો defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
સ્થિર પૂર્ણાંક idn_present(curl_version_info_data *info)
{
#જો defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (શૂન્ય)info;
  પાછા TRUE;
#નહીંતર
  પાછા !!info->libidn;
#endif
}
#endif

#જો defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
સ્થિર પૂર્ણાંક https_proxy_present(curl_version_info_data *info)
{
  (શૂન્ય)info;
  પાછા Curl_ssl_supports(ખાલી, SSLSUPP_HTTPS_PROXY);
}
#endif

#જો defined(USE_SSL) && defined(USE_ECH)
સ્થિર પૂર્ણાંક ech_present(curl_version_info_data *info)
{
  (શૂન્ય)info;
  પાછા Curl_ssl_supports(ખાલી, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

સંરચના feat {
  અચલ અક્ષર *name;
  પૂર્ણાંક        (*present)(curl_version_info_data *info);
  પૂર્ણાંક        bitmask;
};

સ્થિર અચલ સંરચના feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     ખાલી,                CURL_VERSION_ALTSVC),
#endif
#જો defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     ખાલી,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   ખાલી,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      ખાલી,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       ખાલી,                CURL_VERSION_DEBUG),
#endif
#જો defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       ખાલી,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     ખાલી,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        ખાલી,                CURL_VERSION_HSTS),
#endif
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       ખાલી,                CURL_VERSION_HTTP2),
#endif
#જો !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       ખાલી,                CURL_VERSION_HTTP3),
#endif
#જો defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     ખાલી,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     ખાલી,                0),
#endif
#જો defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        ખાલી,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    ખાલી,                CURL_VERSION_KERBEROS5),
#endif
#જો (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   ખાલી,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        ખાલી,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    ખાલી,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        ખાલી,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", ખાલી,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         ખાલી,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", ખાલી,              0),
#અથવાજો defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    ખાલી,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      ખાલી,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         ખાલી,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", ખાલી,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        ખાલી,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  ખાલી,                CURL_VERSION_THREADSAFE),
#endif
#જો defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     ખાલી,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", ખાલી,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        ખાલી,                CURL_VERSION_ZSTD),
#endif
  {ખાલી,                 ખાલી,                0}
};

સ્થિર અચલ અક્ષર *feature_names[CURL_ARRAYSIZE(features_table)] = { ખાલી };

સ્થિર curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_અંક,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  ખાલી, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  ખાલી, /* zlib_version */
  supported_protocols,
  ખાલી, /* c-ares version */
  0,    /* c-ares version numerical */
  ખાલી, /* libidn version */
  0,    /* iconv version */
  ખાલી, /* ssh lib version */
  0,    /* brotli_ver_num */
  ખાલી, /* brotli version */
  0,    /* nghttp2 version number */
  ખાલી, /* nghttp2 version string */
  ખાલી, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#નહીંતર
  ખાલી,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#નહીંતર
  ખાલી,
#endif
  0,    /* zstd_ver_num */
  ખાલી, /* zstd version */
  ખાલી, /* Hyper version */
  ખાલી, /* gsasl version */
  feature_names,
  ખાલી  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  અચલ સંરચના feat *p;
  પૂર્ણાંક features = 0;

#ifdef USE_SSH
  સ્થિર અક્ષર ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  સ્થિર અક્ષર ssl_buffer[200];
#નહીંતર
  સ્થિર અક્ષર ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  સ્થિર અક્ષર brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  સ્થિર અક્ષર zstd_buffer[80];
#endif

  (શૂન્ય)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, કદ(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    પૂર્ણાંક aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_અંક = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, કદ(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_અંક = BrotliDecoderVersion();
  brotli_version(brotli_buffer, કદ(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_અંક = ZSTD_version_સંખ્યા();
  zstd_version(zstd_buffer, કદ(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_અંક = (unsigned પૂર્ણાંક)h2->version_અંક;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#જો !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    સ્થિર અક્ષર quicbuffer[80];
    Curl_quic_ver(quicbuffer, કદ(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(ખાલી);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  માટે(p = features_table; p->name; p++)
    જો(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = ખાલી;  /* Terminate array. */
  version_info.features = features;

  પાછા &version_info;
}
