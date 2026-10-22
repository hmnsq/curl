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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_NGHTTP2
#अन्तर्भाव <nghttp2/nghttp2.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "vssh/ssh.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "easy_lock.h"

#ifdef USE_ARES
#  include <ares.h>
#endif

#ifdef USE_LIBIDN2
#अन्तर्भाव <idn2.h>
#endif

#ifdef USE_LIBPSL
#अन्तर्भाव <libpsl.h>
#endif

#ifdef HAVE_LIBZ
#अन्तर्भाव <zlib.h>
#endif

#ifdef HAVE_BROTLI
#ifdef CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#pragma GCC diagnostic धकेलो
#pragma GCC diagnostic ignored "-Wvla"
#endif
#अन्तर्भाव <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic निकालो
#endif
#endif

#ifdef HAVE_ZSTD
#अन्तर्भाव <zstd.h>
#endif

#ifdef USE_GSASL
#अन्तर्भाव <gsasl.h>
#endif

#ifndef CURL_DISABLE_LDAP
#अन्तर्भाव "curl_ldap.h"
#endif

#ifdef HAVE_BROTLI
स्थिर शून्य brotli_version(वर्ण *buf, size_t bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  unsigned पूर्णाङ्क major = brotli_version >> 24;
  unsigned पूर्णाङ्क minor = (brotli_version & 0x00FFFFFF) >> 12;
  unsigned पूर्णाङ्क patch = brotli_version & 0x00000FFF;
  (शून्य)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
स्थिर शून्य zstd_version(वर्ण *buf, size_t bufsz)
{
  unsigned पूर्णाङ्क version = ZSTD_version_संख्या();
  unsigned पूर्णाङ्क major = version / (100 * 100);
  unsigned पूर्णाङ्क minor = (version - (major * 100 * 100)) / 100;
  unsigned पूर्णाङ्क patch = version - (major * 100 * 100) - (minor * 100);
  (शून्य)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
स्थिर शून्य psl_version(वर्ण *buf, size_t bufsz)
{
#यदि defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  पूर्णाङ्क अंक = psl_check_version_संख्या(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 अंक >> 16, (अंक >> 8) & 0xff, अंक & 0xff);
#अन्यथा
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#यदि defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
स्थिर शून्य idn_version(वर्ण *buf, size_t bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(रिक्त));
#नोचेद्यदि defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#नोचेद्यदि defined(USE_APPLE_IDN)
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

वर्ण *curl_version(शून्य)
{
  स्थिर वर्ण out[300];
  वर्ण *outp;
  size_t outlen;
  ध्रुव वर्ण *src[VERSION_PARTS];
#ifdef USE_SSL
  वर्ण ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  वर्ण z_version[30];
#endif
#ifdef HAVE_BROTLI
  वर्ण br_version[30];
#endif
#ifdef HAVE_ZSTD
  वर्ण zstd_ver[30];
#endif
#ifdef USE_ARES
  वर्ण cares_version[30];
#endif
#ifdef USE_IDN
  वर्ण idn_ver[30];
#endif
#ifdef USE_LIBPSL
  वर्ण psl_ver[30];
#endif
#ifdef USE_SSH
  वर्ण ssh_version[30];
#endif
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  वर्ण h2_version[30];
#endif
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  वर्ण h3_version[30];
#endif
#ifdef USE_GSASL
  वर्ण gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  वर्ण gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  वर्ण ldap_buf[30];
#endif
  पूर्णाङ्क i = 0;
  पूर्णाङ्क j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  ध्रुव वर्ण *debugversion = getenv("CURL_DEBUG_VERSION");
  यदि(debugversion) {
    curl_msnprintf(out, परिमाण(out), "%s", debugversion);
    प्रत्यागम out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, परिमाण(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, परिमाण(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, परिमाण(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, परिमाण(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, परिमाण(cares_version),
                 "c-ares/%s", ares_version(रिक्त));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, परिमाण(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, परिमाण(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, परिमाण(ssh_version));
  src[i++] = ssh_version;
#endif
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, परिमाण(h2_version));
  src[i++] = h2_version;
#endif
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, परिमाण(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, परिमाण(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(रिक्त));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, परिमाण(gss_buf), "AppleGSS");
#नोचेद्यदि defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, परिमाण(gss_buf), "libgss/%s", GSS_VERSION);
#नोचेद्यदि defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, परिमाण(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#अन्यथा
  curl_msnprintf(gss_buf, परिमाण(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, परिमाण(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = परिमाण(out);
  प्रति(j = 0; j < i; j++) {
    size_t n = strlen(src[j]);
    /* we need room for a space, the string and the final zero */
    यदि(outlen <= (n + 2))
      भङ्ग;
    यदि(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    memcpy(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  प्रत्यागम out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

स्थिर ध्रुव वर्ण * ध्रुव supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#यदि !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#endif
#endif

  रिक्त
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#यदि defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
स्थिर पूर्णाङ्क idn_present(curl_version_info_data *info)
{
#यदि defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (शून्य)info;
  प्रत्यागम TRUE;
#अन्यथा
  प्रत्यागम !!info->libidn;
#endif
}
#endif

#यदि defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
स्थिर पूर्णाङ्क https_proxy_present(curl_version_info_data *info)
{
  (शून्य)info;
  प्रत्यागम Curl_ssl_supports(रिक्त, SSLSUPP_HTTPS_PROXY);
}
#endif

#यदि defined(USE_SSL) && defined(USE_ECH)
स्थिर पूर्णाङ्क ech_present(curl_version_info_data *info)
{
  (शून्य)info;
  प्रत्यागम Curl_ssl_supports(रिक्त, SSLSUPP_ECH);
}
#endif

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#define FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

संरचना feat {
  ध्रुव वर्ण *name;
  पूर्णाङ्क        (*present)(curl_version_info_data *info);
  पूर्णाङ्क        bitmask;
};

स्थिर ध्रुव संरचना feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     रिक्त,                CURL_VERSION_ALTSVC),
#endif
#यदि defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     रिक्त,                0),
#endif
#ifdef CURLRES_ASYNCH
  FEATURE("AsynchDNS",   रिक्त,                CURL_VERSION_ASYNCHDNS),
#endif
#ifdef HAVE_BROTLI
  FEATURE("brotli",      रिक्त,                CURL_VERSION_BROTLI),
#endif
#ifdef DEBUGBUILD
  FEATURE("Debug",       रिक्त,                CURL_VERSION_DEBUG),
#endif
#यदि defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ifndef USE_HTTPSRR
#error "ECH enabled but not HTTPSRR, must be a config error"
#endif
#endif
#ifdef USE_GSASL
  FEATURE("gsasl",       रिक्त,                CURL_VERSION_GSASL),
#endif
#ifdef HAVE_GSSAPI
  FEATURE("GSS-API",     रिक्त,                CURL_VERSION_GSSAPI),
#endif
#ifndef CURL_DISABLE_HSTS
  FEATURE("HSTS",        रिक्त,                CURL_VERSION_HSTS),
#endif
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       रिक्त,                CURL_VERSION_HTTP2),
#endif
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       रिक्त,                CURL_VERSION_HTTP3),
#endif
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     रिक्त,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     रिक्त,                0),
#endif
#यदि defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        रिक्त,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    रिक्त,                CURL_VERSION_KERBEROS5),
#endif
#यदि (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   रिक्त,                CURL_VERSION_LARGEFILE),
#endif
#ifdef HAVE_LIBZ
  FEATURE("libz",        रिक्त,                CURL_VERSION_LIBZ),
#endif
#ifdef CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    रिक्त,                CURL_VERSION_MULTI_SSL),
#endif
#ifdef USE_NTLM
  FEATURE("NTLM",        रिक्त,                CURL_VERSION_NTLM),
#endif
#ifdef USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", रिक्त,                0),
#endif
#ifdef USE_LIBPSL
  FEATURE("PSL",         रिक्त,                CURL_VERSION_PSL),
#endif
#ifdef USE_SSL
#ifdef USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", रिक्त,              0),
#नोचेद्यदि defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    रिक्त,              0),
#endif
#endif /* USE_SSL */
#ifdef USE_SPNEGO
  FEATURE("SPNEGO",      रिक्त,                CURL_VERSION_SPNEGO),
#endif
#ifdef USE_SSL
  FEATURE("SSL",         रिक्त,                CURL_VERSION_SSL),
#endif
#ifdef USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", रिक्त,                0),
#endif
#ifdef USE_WINDOWS_SSPI
  FEATURE("SSPI",        रिक्त,                CURL_VERSION_SSPI),
#endif
#ifdef GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  रिक्त,                CURL_VERSION_THREADSAFE),
#endif
#यदि defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     रिक्त,                CURL_VERSION_UNICODE),
#endif
#ifdef USE_UNIX_SOCKETS
  FEATURE("UnixSockets", रिक्त,                CURL_VERSION_UNIX_SOCKETS),
#endif
#ifdef HAVE_ZSTD
  FEATURE("zstd",        रिक्त,                CURL_VERSION_ZSTD),
#endif
  {रिक्त,                 रिक्त,                0}
};

स्थिर ध्रुव वर्ण *feature_names[CURL_ARRAYSIZE(features_table)] = { रिक्त };

स्थिर curl_version_info_data version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_अंक,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  रिक्त, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  रिक्त, /* zlib_version */
  supported_protocols,
  रिक्त, /* c-ares version */
  0,    /* c-ares version numerical */
  रिक्त, /* libidn version */
  0,    /* iconv version */
  रिक्त, /* ssh lib version */
  0,    /* brotli_ver_num */
  रिक्त, /* brotli version */
  0,    /* nghttp2 version number */
  रिक्त, /* nghttp2 version string */
  रिक्त, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#अन्यथा
  रिक्त,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#अन्यथा
  रिक्त,
#endif
  0,    /* zstd_ver_num */
  रिक्त, /* zstd version */
  रिक्त, /* Hyper version */
  रिक्त, /* gsasl version */
  feature_names,
  रिक्त  /* rtmp version */
};

curl_version_info_data *curl_version_info(CURLversion stamp)
{
  size_t n;
  ध्रुव संरचना feat *p;
  पूर्णाङ्क features = 0;

#ifdef USE_SSH
  स्थिर वर्ण ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  स्थिर वर्ण ssl_buffer[200];
#अन्यथा
  स्थिर वर्ण ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  स्थिर वर्ण brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  स्थिर वर्ण zstd_buffer[80];
#endif

  (शून्य)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, परिमाण(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    पूर्णाङ्क aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_अंक = aresnum;
  }
#endif
#ifdef USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#endif

#ifdef USE_SSH
  Curl_ssh_version(ssh_buf, परिमाण(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_अंक = BrotliDecoderVersion();
  brotli_version(brotli_buffer, परिमाण(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_अंक = ZSTD_version_संख्या();
  zstd_version(zstd_buffer, परिमाण(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_अंक = (unsigned पूर्णाङ्क)h2->version_अंक;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    स्थिर वर्ण quicbuffer[80];
    Curl_quic_ver(quicbuffer, परिमाण(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#endif

#ifdef USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(रिक्त);
  }
#endif

  /* Get available features, build bitmask and names array. */
  n = 0;
  प्रति(p = features_table; p->name; p++)
    यदि(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = रिक्त;  /* Terminate array. */
  version_info.features = features;

  प्रत्यागम &version_info;
}
