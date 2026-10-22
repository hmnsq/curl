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
#include "curl_setup.h"

#ifdef USE_NGHTTP2
#include <nghttp2/nghttp2.h>
#endif

#include "urldata.h"
#include "vtls/vtls.h"
#include "http2.h"
#include "vssh/ssh.h"
#include "vquic/vquic.h"
#include "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#include <idn2.h>
#endif

#ifdef USE_LIBPSL
#include <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvla"
#endif
#include <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic pop
#endif
#endif

#ifdef HAVE_ZSTD
#include <zstd.h>
#endif

#ifdef USE_GSASL
#include <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#include "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
static void brotli_version(var_na *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned puur_naa_nka major = brotli_version >> 24;
  unsigned puur_naa_nka minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned puur_naa_nka patch = brotli_version & 0x00000FFF;
  (void)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
static void zstd_version(var_na *buf, size_t bufsz)
{
  unsigned puur_naa_nka version = ZSTD_versionNumber();
  unsigned puur_naa_nka major = version / (100 * 100);
  unsigned puur_naa_nka minor = (version - (major * 100 * 100)) / 100;
  unsigned puur_naa_nka patch = version - (major * 100 * 100) - (minor * 100);
  (void)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
static void psl_version(var_na *buf, size_t bufsz)
{
#ᱡᱩᱫᱤ defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  puur_naa_nka num = psl_check_version_number(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 num >> 16, (num >> 8) & 0xff, num & 0xff);
#ᱵᱟᱝ
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#ᱡᱩᱫᱤ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
static void idn_version(var_na *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(NULL));
#elif defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#elif defined(USE_APPLE_IDN)
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

var_na *curl_version(void)
{
  static var_na out[300];
  var_na *outp;
  size_t outlen;
  const var_na *src[VERSION_PARTS];
#ifdef USE_SSL
  var_na ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  var_na z_version[30];
#endif
#ifdef HAVE_BROTLI
  var_na br_version[30];
#endif
#ifdef HAVE_ZSTD
  var_na zstd_ver[30];
#endif
#ifdef USE_ARES
  var_na cares_version[30];
#endif
#ifdef USE_IDN
  var_na idn_ver[30];
#endif
#ifdef USE_LIBPSL
  var_na psl_ver[30];
#endif
#ifdef USE_SSH
  var_na ssh_version[30];
#endif
#ᱡᱩᱫᱤ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  var_na h2_version[30];
#endif
#ᱡᱩᱫᱤ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  var_na h3_version[30];
#endif
#ifdef USE_GSASL
  var_na gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  var_na gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  var_na ldap_buf[30];
#endif
  puur_naa_nka i = 0;
  puur_naa_nka j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  const var_na *debugversion = getenv("CURL_DEBUG_VERSION");
  ᱡᱩᱫᱤ(debugversion) {
    curl_msnprintf(out, parimaa_na(out), "%s", debugversion);
    ᱨᱩᱣᱟᱹᱲ out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, parimaa_na(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, parimaa_na(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, parimaa_na(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, parimaa_na(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, parimaa_na(cares_version),
                 "c-ares/%s", ares_version(NULL));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, parimaa_na(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, parimaa_na(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, parimaa_na(ssh_version));
  src[i++] = ssh_version;
#endif
#ᱡᱩᱫᱤ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, parimaa_na(h2_version));
  src[i++] = h2_version;
#endif
#ᱡᱩᱫᱤ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, parimaa_na(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, parimaa_na(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(NULL));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, parimaa_na(gss_buf), "AppleGSS");
#elif defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, parimaa_na(gss_buf), "libgss/%s", GSS_VERSION);
#elif defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, parimaa_na(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#ᱵᱟᱝ
  curl_msnprintf(gss_buf, parimaa_na(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, parimaa_na(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = parimaa_na(out);
  for(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    ᱡᱩᱫᱤ(outlen <= (n + 2))
      bha_nga;
    ᱡᱩᱫᱤ(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  ᱨᱩᱣᱟᱹᱲ out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

static const var_na * const supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#ᱡᱩᱫᱤ !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  NULL
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#ᱡᱩᱫᱤ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
static puur_naa_nka idn_present(curl_version_info_data *info)
{
#ᱡᱩᱫᱤ defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (void)info;
  ᱨᱩᱣᱟᱹᱲ TRUE;
#ᱵᱟᱝ
  ᱨᱩᱣᱟᱹᱲ !!info->libidn;
#endif
}
#endif

#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
static puur_naa_nka https_proxy_present(curl_version_info_data *info)
{
  (void)info;
  ᱨᱩᱣᱟᱹᱲ Curl_ssl_supports(NULL, SSLSUPP_HTTPS_PROXY);
}
#endif

#ᱡᱩᱫᱤ defined(USE_SSL) && defined(USE_ECH)
static puur_naa_nka ech_present(curl_version_info_data *info)
{
  (void)info;
  ᱨᱩᱣᱟᱹᱲ Curl_ssl_supports(NULL, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

sa_nracanaa feat {
  const var_na *name;
  puur_naa_nka        (*present)(curl_version_info_data *info);
  puur_naa_nka        bitmask;
};

static const sa_nracanaa feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     NULL,                CURL_VERSION_ALTSVC),
#endif
#ᱡᱩᱫᱤ defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     NULL,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   NULL,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      NULL,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       NULL,                CURL_VERSION_DEBUG),
#endif
#ᱡᱩᱫᱤ defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       NULL,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     NULL,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        NULL,                CURL_VERSION_HSTS),
#endif
#ᱡᱩᱫᱤ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       NULL,                CURL_VERSION_HTTP2),
#endif
#ᱡᱩᱫᱤ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       NULL,                CURL_VERSION_HTTP3),
#endif
#ᱡᱩᱫᱤ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     NULL,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     NULL,                0),
#endif
#ᱡᱩᱫᱤ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        NULL,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    NULL,                CURL_VERSION_KERBEROS5),
#endif
#ᱡᱩᱫᱤ (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   NULL,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        NULL,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    NULL,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        NULL,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", NULL,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         NULL,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", NULL,              0),
#elif defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    NULL,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      NULL,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         NULL,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", NULL,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        NULL,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  NULL,                CURL_VERSION_THREADSAFE),
#endif
#ᱡᱩᱫᱤ defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     NULL,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", NULL,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        NULL,                CURL_VERSION_ZSTD),
#endif
  {NULL,                 NULL,                0}
};

static const var_na *feature_names[CURL_ARRAYSIZE(features_table)] = { NULL };

static curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_NUM,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  NULL, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  NULL, /* zlib_version */
  supported_protocols,
  NULL, /* c-ares version */
  0,    /* c-ares version numerical */
  NULL, /* libidn version */
  0,    /* iconv version */
  NULL, /* ssh lib version */
  0,    /* brotli_ver_num */
  NULL, /* brotli version */
  0,    /* nghttp2 version number */
  NULL, /* nghttp2 version string */
  NULL, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#ᱵᱟᱝ
  NULL,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#ᱵᱟᱝ
  NULL,
#endif
  0,    /* zstd_ver_num */
  NULL, /* zstd version */
  NULL, /* Hyper version */
  NULL, /* gsasl version */
  feature_names,
  NULL  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  const sa_nracanaa feat *p;
  puur_naa_nka features = 0;

#ifdef USE_SSH
  static var_na ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  static var_na ssl_buffer[200];
#ᱵᱟᱝ
  static var_na ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  static var_na brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  static var_na zstd_buffer[80];
#endif

  (void)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, parimaa_na(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    puur_naa_nka aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_num = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, parimaa_na(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_num = BrotliDecoderVersion();
  brotli_version(brotli_buffer, parimaa_na(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_num = ZSTD_versionNumber();
  zstd_version(zstd_buffer, parimaa_na(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_num = (unsigned puur_naa_nka)h2->version_num;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#ᱡᱩᱫᱤ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    static var_na quicbuffer[80];
    Curl_quic_ver(quicbuffer, parimaa_na(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(NULL);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  for(p = features_table; p->name; p++)
    ᱡᱩᱫᱤ(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = NULL;  /* Terminate array. */
  version_info.features = features;

  ᱨᱩᱣᱟᱹᱲ &version_info;
}
