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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_NGHTTP2
#அந்தர்பாவ <nghttp2/nghttp2.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "http2.h"
#அந்தர்பாவ "vssh/ssh.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#அந்தர்பாவ <idn2.h>
#endif

#ifdef USE_LIBPSL
#அந்தர்பாவ <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#அந்தர்பாவ <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Wvla"
#endif
#அந்தர்பாவ <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic எடு
#endif
#endif

#ifdef HAVE_ZSTD
#அந்தர்பாவ <zstd.h>
#endif

#ifdef USE_GSASL
#அந்தர்பாவ <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#அந்தர்பாவ "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
நிலையான வெறுமை brotli_version(எழுத்து *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned முழு major = brotli_version >> 24;
  unsigned முழு minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned முழு patch = brotli_version & 0x00000FFF;
  (வெறுமை)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
நிலையான வெறுமை zstd_version(எழுத்து *buf, size_t bufsz)
{
  unsigned முழு version = ZSTD_version_எண்();
  unsigned முழு major = version / (100 * 100);
  unsigned முழு minor = (version - (major * 100 * 100)) / 100;
  unsigned முழு patch = version - (major * 100 * 100) - (minor * 100);
  (வெறுமை)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
நிலையான வெறுமை psl_version(எழுத்து *buf, size_t bufsz)
{
#எனில் defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  முழு இலக்கம் = psl_check_version_எண்(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 இலக்கம் >> 16, (இலக்கம் >> 8) & 0xff, இலக்கம் & 0xff);
#இல்லையெனில்
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#எனில் defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
நிலையான வெறுமை idn_version(எழுத்து *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(வெற்று));
#அல்லதெனில் defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#அல்லதெனில் defined(USE_APPLE_IDN)
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

எழுத்து *curl_version(வெறுமை)
{
  நிலையான எழுத்து out[300];
  எழுத்து *outp;
  size_t outlen;
  மாறிலி எழுத்து *src[VERSION_PARTS];
#ifdef USE_SSL
  எழுத்து ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  எழுத்து z_version[30];
#endif
#ifdef HAVE_BROTLI
  எழுத்து br_version[30];
#endif
#ifdef HAVE_ZSTD
  எழுத்து zstd_ver[30];
#endif
#ifdef USE_ARES
  எழுத்து cares_version[30];
#endif
#ifdef USE_IDN
  எழுத்து idn_ver[30];
#endif
#ifdef USE_LIBPSL
  எழுத்து psl_ver[30];
#endif
#ifdef USE_SSH
  எழுத்து ssh_version[30];
#endif
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  எழுத்து h2_version[30];
#endif
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  எழுத்து h3_version[30];
#endif
#ifdef USE_GSASL
  எழுத்து gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  எழுத்து gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  எழுத்து ldap_buf[30];
#endif
  முழு i = 0;
  முழு j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  மாறிலி எழுத்து *debugversion = getenv("CURL_DEBUG_VERSION");
  எனில்(debugversion) {
    curl_msnprintf(out, அளவு(out), "%s", debugversion);
    திரும்பு out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, அளவு(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, அளவு(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, அளவு(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, அளவு(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, அளவு(cares_version),
                 "c-ares/%s", ares_version(வெற்று));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, அளவு(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, அளவு(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, அளவு(ssh_version));
  src[i++] = ssh_version;
#endif
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, அளவு(h2_version));
  src[i++] = h2_version;
#endif
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, அளவு(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, அளவு(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(வெற்று));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, அளவு(gss_buf), "AppleGSS");
#அல்லதெனில் defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, அளவு(gss_buf), "libgss/%s", GSS_VERSION);
#அல்லதெனில் defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, அளவு(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#இல்லையெனில்
  curl_msnprintf(gss_buf, அளவு(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, அளவு(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = அளவு(out);
  க்கு(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    எனில்(outlen <= (n + 2))
      நிறுத்து;
    எனில்(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  திரும்பு out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

நிலையான மாறிலி எழுத்து * மாறிலி supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#எனில் !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  வெற்று
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#எனில் defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
நிலையான முழு idn_present(curl_version_info_data *info)
{
#எனில் defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (வெறுமை)info;
  திரும்பு TRUE;
#இல்லையெனில்
  திரும்பு !!info->libidn;
#endif
}
#endif

#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
நிலையான முழு https_proxy_present(curl_version_info_data *info)
{
  (வெறுமை)info;
  திரும்பு Curl_ssl_supports(வெற்று, SSLSUPP_HTTPS_PROXY);
}
#endif

#எனில் defined(USE_SSL) && defined(USE_ECH)
நிலையான முழு ech_present(curl_version_info_data *info)
{
  (வெறுமை)info;
  திரும்பு Curl_ssl_supports(வெற்று, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

கட்டமைப்பு feat {
  மாறிலி எழுத்து *name;
  முழு        (*present)(curl_version_info_data *info);
  முழு        bitmask;
};

நிலையான மாறிலி கட்டமைப்பு feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     வெற்று,                CURL_VERSION_ALTSVC),
#endif
#எனில் defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     வெற்று,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   வெற்று,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      வெற்று,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       வெற்று,                CURL_VERSION_DEBUG),
#endif
#எனில் defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       வெற்று,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     வெற்று,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        வெற்று,                CURL_VERSION_HSTS),
#endif
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       வெற்று,                CURL_VERSION_HTTP2),
#endif
#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       வெற்று,                CURL_VERSION_HTTP3),
#endif
#எனில் defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     வெற்று,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     வெற்று,                0),
#endif
#எனில் defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        வெற்று,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    வெற்று,                CURL_VERSION_KERBEROS5),
#endif
#எனில் (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   வெற்று,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        வெற்று,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    வெற்று,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        வெற்று,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", வெற்று,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         வெற்று,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", வெற்று,              0),
#அல்லதெனில் defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    வெற்று,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      வெற்று,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         வெற்று,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", வெற்று,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        வெற்று,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  வெற்று,                CURL_VERSION_THREADSAFE),
#endif
#எனில் defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     வெற்று,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", வெற்று,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        வெற்று,                CURL_VERSION_ZSTD),
#endif
  {வெற்று,                 வெற்று,                0}
};

நிலையான மாறிலி எழுத்து *feature_names[CURL_ARRAYSIZE(features_table)] = { வெற்று };

நிலையான curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_இலக்கம்,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  வெற்று, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  வெற்று, /* zlib_version */
  supported_protocols,
  வெற்று, /* c-ares version */
  0,    /* c-ares version numerical */
  வெற்று, /* libidn version */
  0,    /* iconv version */
  வெற்று, /* ssh lib version */
  0,    /* brotli_ver_num */
  வெற்று, /* brotli version */
  0,    /* nghttp2 version number */
  வெற்று, /* nghttp2 version string */
  வெற்று, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#இல்லையெனில்
  வெற்று,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#இல்லையெனில்
  வெற்று,
#endif
  0,    /* zstd_ver_num */
  வெற்று, /* zstd version */
  வெற்று, /* Hyper version */
  வெற்று, /* gsasl version */
  feature_names,
  வெற்று  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  மாறிலி கட்டமைப்பு feat *p;
  முழு features = 0;

#ifdef USE_SSH
  நிலையான எழுத்து ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  நிலையான எழுத்து ssl_buffer[200];
#இல்லையெனில்
  நிலையான எழுத்து ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  நிலையான எழுத்து brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  நிலையான எழுத்து zstd_buffer[80];
#endif

  (வெறுமை)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, அளவு(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    முழு aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_இலக்கம் = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, அளவு(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_இலக்கம் = BrotliDecoderVersion();
  brotli_version(brotli_buffer, அளவு(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_இலக்கம் = ZSTD_version_எண்();
  zstd_version(zstd_buffer, அளவு(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_இலக்கம் = (unsigned முழு)h2->version_இலக்கம்;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#எனில் !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    நிலையான எழுத்து quicbuffer[80];
    Curl_quic_ver(quicbuffer, அளவு(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(வெற்று);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  க்கு(p = features_table; p->name; p++)
    எனில்(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = வெற்று;  /* Terminate array. */
  version_info.features = features;

  திரும்பு &version_info;
}
