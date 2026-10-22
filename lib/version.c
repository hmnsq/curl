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
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic ignored "-Wvla"
#endif
#अन्तर्भाव <brotli/decode.h>
#ifdef CURL_HAVE_DIAG
#pragma GCC diagnostic काढा
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
स्थिर शून्य brotli_version(अक्षर *buf, माप_प्रकार bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  अचिन्हित पूर्णांक major = brotli_version >> 24;
  अचिन्हित पूर्णांक minor = (brotli_version & 0x00FFFFFF) >> 12;
  अचिन्हित पूर्णांक patch = brotli_version & 0x00000FFF;
  (शून्य)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef HAVE_ZSTD
स्थिर शून्य zstd_version(अक्षर *buf, माप_प्रकार bufsz)
{
  अचिन्हित पूर्णांक version = ZSTD_version_संख्या();
  अचिन्हित पूर्णांक major = version / (100 * 100);
  अचिन्हित पूर्णांक minor = (version - (major * 100 * 100)) / 100;
  अचिन्हित पूर्णांक patch = version - (major * 100 * 100) - (minor * 100);
  (शून्य)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#endif

#ifdef USE_LIBPSL
स्थिर शून्य psl_version(अक्षर *buf, माप_प्रकार bufsz)
{
#जर defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  पूर्णांक अंक = psl_check_version_संख्या(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 अंक >> 16, (अंक >> 8) & 0xff, अंक & 0xff);
#अन्यथा
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#endif
}
#endif

#जर defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#define USE_IDN
#endif

#ifdef USE_IDN
स्थिर शून्य idn_version(अक्षर *buf, माप_प्रकार bufsz)
{
#ifdef USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(रिक्त));
#किंवाजर defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#किंवाजर defined(USE_APPLE_IDN)
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

अक्षर *curl_version(शून्य)
{
  स्थिर अक्षर out[300];
  अक्षर *outp;
  माप_प्रकार outlen;
  स्थिरांक अक्षर *src[VERSION_PARTS];
#ifdef USE_SSL
  अक्षर ssl_version[200];
#endif
#ifdef HAVE_LIBZ
  अक्षर z_version[30];
#endif
#ifdef HAVE_BROTLI
  अक्षर br_version[30];
#endif
#ifdef HAVE_ZSTD
  अक्षर zstd_ver[30];
#endif
#ifdef USE_ARES
  अक्षर cares_version[30];
#endif
#ifdef USE_IDN
  अक्षर idn_ver[30];
#endif
#ifdef USE_LIBPSL
  अक्षर psl_ver[30];
#endif
#ifdef USE_SSH
  अक्षर ssh_version[30];
#endif
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  अक्षर h2_version[30];
#endif
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  अक्षर h3_version[30];
#endif
#ifdef USE_GSASL
  अक्षर gsasl_buf[30];
#endif
#ifdef HAVE_GSSAPI
  अक्षर gss_buf[40];
#endif
#ifndef CURL_DISABLE_LDAP
  अक्षर ldap_buf[30];
#endif
  पूर्णांक i = 0;
  पूर्णांक j;

#ifdef DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  स्थिरांक अक्षर *debugversion = दो_पर्या("CURL_DEBUG_VERSION");
  जर(debugversion) {
    curl_msnprintf(out, आकार(out), "%s", debugversion);
    परत out;
  }
#endif

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#ifdef USE_SSL
  Curl_ssl_version(ssl_version, आकार(ssl_version));
  src[i++] = ssl_version;
#endif
#ifdef HAVE_LIBZ
  curl_msnprintf(z_version, आकार(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#endif
#ifdef HAVE_BROTLI
  brotli_version(br_version, आकार(br_version));
  src[i++] = br_version;
#endif
#ifdef HAVE_ZSTD
  zstd_version(zstd_ver, आकार(zstd_ver));
  src[i++] = zstd_ver;
#endif
#ifdef USE_ARES
  curl_msnprintf(cares_version, आकार(cares_version),
                 "c-ares/%s", ares_version(रिक्त));
  src[i++] = cares_version;
#endif
#ifdef USE_IDN
  idn_version(idn_ver, आकार(idn_ver));
  src[i++] = idn_ver;
#endif
#ifdef USE_LIBPSL
  psl_version(psl_ver, आकार(psl_ver));
  src[i++] = psl_ver;
#endif
#ifdef USE_SSH
  Curl_ssh_version(ssh_version, आकार(ssh_version));
  src[i++] = ssh_version;
#endif
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, आकार(h2_version));
  src[i++] = h2_version;
#endif
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, आकार(h3_version));
  src[i++] = h3_version;
#endif
#ifdef USE_GSASL
  curl_msnprintf(gsasl_buf, आकार(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(रिक्त));
  src[i++] = gsasl_buf;
#endif
#ifdef HAVE_GSSAPI
#ifdef HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, आकार(gss_buf), "AppleGSS");
#किंवाजर defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, आकार(gss_buf), "libgss/%s", GSS_VERSION);
#किंवाजर defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, आकार(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#अन्यथा
  curl_msnprintf(gss_buf, आकार(gss_buf), "mit-krb5");
#endif
  src[i++] = gss_buf;
#endif /* HAVE_GSSAPI */
#ifndef CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, आकार(ldap_buf));
  src[i++] = ldap_buf;
#endif

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = आकार(out);
  साठी(j = 0; j < i; j++) {
    माप_प्रकार n = म_माप(src[j]);
    /* we need room for a space, the string and the final zero */
    जर(outlen <= (n + 2))
      थांबा;
    जर(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    स_नकल(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  परत out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

स्थिर स्थिरांक अक्षर * स्थिरांक supported_protocols[] = {
#ifndef CURL_DISABLE_DICT
  "dict",
#endif
#ifndef CURL_DISABLE_FILE
  "file",
#endif
#ifndef CURL_DISABLE_FTP
  "ftp",
#endif
#जर defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#endif
#ifndef CURL_DISABLE_GOPHER
  "gopher",
#endif
#जर defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#endif
#ifndef CURL_DISABLE_HTTP
  "http",
#endif
#जर defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#endif
#ifndef CURL_DISABLE_IMAP
  "imap",
#endif
#जर defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#endif
#ifndef CURL_DISABLE_LDAP
  "ldap",
#जर !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#endif
#endif
#ifndef CURL_DISABLE_MQTT
  "mqtt",
#endif
#जर defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#endif
#ifndef CURL_DISABLE_POP3
  "pop3",
#endif
#जर defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
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
#जर defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
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
#जर defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
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

#जर defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
स्थिर पूर्णांक idn_present(curl_version_info_data *info)
{
#जर defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (शून्य)info;
  परत TRUE;
#अन्यथा
  परत !!info->libidn;
#endif
}
#endif

#जर defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
स्थिर पूर्णांक https_proxy_present(curl_version_info_data *info)
{
  (शून्य)info;
  परत Curl_ssl_supports(रिक्त, SSLSUPP_HTTPS_PROXY);
}
#endif

#जर defined(USE_SSL) && defined(USE_ECH)
स्थिर पूर्णांक ech_present(curl_version_info_data *info)
{
  (शून्य)info;
  परत Curl_ssl_supports(रिक्त, SSLSUPP_ECH);
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
  स्थिरांक अक्षर *name;
  पूर्णांक        (*present)(curl_version_info_data *info);
  पूर्णांक        bitmask;
};

स्थिर स्थिरांक संरचना feat features_table[] = {
#ifndef CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     रिक्त,                CURL_VERSION_ALTSVC),
#endif
#जर defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
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
#जर defined(USE_SSL) && defined(USE_ECH)
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
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       रिक्त,                CURL_VERSION_HTTP2),
#endif
#जर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       रिक्त,                CURL_VERSION_HTTP3),
#endif
#जर defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#endif
#ifndef CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     रिक्त,                0),
#endif
#ifdef USE_HTTPSRR
  FEATURE("HTTPSRR",     रिक्त,                0),
#endif
#जर defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#endif
#ifdef USE_IPV6
  FEATURE("IPv6",        रिक्त,                CURL_VERSION_IPV6),
#endif
#ifdef USE_KERBEROS5
  FEATURE("Kerberos",    रिक्त,                CURL_VERSION_KERBEROS5),
#endif
#जर (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
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
#किंवाजर defined(CURL_CA_NATIVE)
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
#जर defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
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

स्थिर स्थिरांक अक्षर *feature_names[CURL_ARRAYSIZE(features_table)] = { रिक्त };

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
  माप_प्रकार n;
  स्थिरांक संरचना feat *p;
  पूर्णांक features = 0;

#ifdef USE_SSH
  स्थिर अक्षर ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
  स्थिर अक्षर ssl_buffer[200];
#अन्यथा
  स्थिर अक्षर ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
  स्थिर अक्षर brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
  स्थिर अक्षर zstd_buffer[80];
#endif

  (शून्य)stamp;

#ifdef USE_SSL
  Curl_ssl_version(ssl_buffer, आकार(ssl_buffer));
  version_info.ssl_version = ssl_buffer;
#endif

#ifdef HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#endif
#ifdef USE_ARES
  {
    पूर्णांक aresnum;
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
  Curl_ssh_version(ssh_buf, आकार(ssh_buf));
  version_info.libssh_version = ssh_buf;
#endif

#ifdef HAVE_BROTLI
  version_info.brotli_ver_अंक = BrotliDecoderVersion();
  brotli_version(brotli_buffer, आकार(brotli_buffer));
  version_info.brotli_version = brotli_buffer;
#endif

#ifdef HAVE_ZSTD
  version_info.zstd_ver_अंक = ZSTD_version_संख्या();
  zstd_version(zstd_buffer, आकार(zstd_buffer));
  version_info.zstd_version = zstd_buffer;
#endif

#ifdef USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_अंक = (अचिन्हित पूर्णांक)h2->version_अंक;
    version_info.nghttp2_version = h2->version_str;
  }
#endif

#जर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    स्थिर अक्षर quicbuffer[80];
    Curl_quic_ver(quicbuffer, आकार(quicbuffer));
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
  साठी(p = features_table; p->name; p++)
    जर(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#ifdef DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#endif

  feature_names[n] = रिक्त;  /* Terminate array. */
  version_info.features = features;

  परत &version_info;
}
