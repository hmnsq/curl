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
#समावेश "curl_setup.h"

#अगर_घोषित USE_NGHTTP2
#समावेश <nghttp2/nghttp2.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "vtls/vtls.h"
#समावेश "http2.h"
#समावेश "vssh/ssh.h"
#समावेश "vquic/vquic.h"
#समावेश "easy_lock.h"

#अगर_घोषित USE_ARES
#  include <ares.h>
#पूर्ण_अगर

#अगर_घोषित USE_LIBIDN2
#समावेश <idn2.h>
#पूर्ण_अगर

#अगर_घोषित USE_LIBPSL
#समावेश <libpsl.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_LIBZ
#समावेश <zlib.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_BROTLI
#अगर_घोषित CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Wvla"
#पूर्ण_अगर
#समावेश <brotli/decode.h>
#अगर_घोषित CURL_HAVE_DIAG
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_घोषित HAVE_ZSTD
#समावेश <zstd.h>
#पूर्ण_अगर

#अगर_घोषित USE_GSASL
#समावेश <gsasl.h>
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_LDAP
#समावेश "curl_ldap.h"
#पूर्ण_अगर

#अगर_घोषित HAVE_BROTLI
अटल व्योम brotli_version(अक्षर *buf, माप_प्रकार bufsz)
{
  uint32_t brotli_version = BrotliDecoderVersion();
  अचिन्हित पूर्णांक major = brotli_version >> 24;
  अचिन्हित पूर्णांक minor = (brotli_version & 0x00FFFFFF) >> 12;
  अचिन्हित पूर्णांक patch = brotli_version & 0x00000FFF;
  (व्योम)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#पूर्ण_अगर

#अगर_घोषित HAVE_ZSTD
अटल व्योम zstd_version(अक्षर *buf, माप_प्रकार bufsz)
{
  अचिन्हित पूर्णांक version = ZSTD_version_संख्या();
  अचिन्हित पूर्णांक major = version / (100 * 100);
  अचिन्हित पूर्णांक minor = (version - (major * 100 * 100)) / 100;
  अचिन्हित पूर्णांक patch = version - (major * 100 * 100) - (minor * 100);
  (व्योम)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#पूर्ण_अगर

#अगर_घोषित USE_LIBPSL
अटल व्योम psl_version(अक्षर *buf, माप_प्रकार bufsz)
{
#अगर defined(PSL_VERSION_MAJOR) && (PSL_VERSION_MAJOR > 0 ||     \
                                   PSL_VERSION_MINOR >= 11)
  पूर्णांक अंक = psl_check_version_संख्या(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 अंक >> 16, (अंक >> 8) & 0xff, अंक & 0xff);
#अन्यथा
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_version());
#पूर्ण_अगर
}
#पूर्ण_अगर

#अगर defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#घोषणा USE_IDN
#पूर्ण_अगर

#अगर_घोषित USE_IDN
अटल व्योम idn_version(अक्षर *buf, माप_प्रकार bufsz)
{
#अगर_घोषित USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_version(शून्य));
#या_अगर defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#या_अगर defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#पूर्ण_अगर
}
#पूर्ण_अगर

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#घोषणा VERSION_PARTS 16 /* number of substrings we can concatenate */

अक्षर *curl_version(व्योम)
{
  अटल अक्षर out[300];
  अक्षर *outp;
  माप_प्रकार outlen;
  स्थिर अक्षर *src[VERSION_PARTS];
#अगर_घोषित USE_SSL
  अक्षर ssl_version[200];
#पूर्ण_अगर
#अगर_घोषित HAVE_LIBZ
  अक्षर z_version[30];
#पूर्ण_अगर
#अगर_घोषित HAVE_BROTLI
  अक्षर br_version[30];
#पूर्ण_अगर
#अगर_घोषित HAVE_ZSTD
  अक्षर zstd_ver[30];
#पूर्ण_अगर
#अगर_घोषित USE_ARES
  अक्षर cares_version[30];
#पूर्ण_अगर
#अगर_घोषित USE_IDN
  अक्षर idn_ver[30];
#पूर्ण_अगर
#अगर_घोषित USE_LIBPSL
  अक्षर psl_ver[30];
#पूर्ण_अगर
#अगर_घोषित USE_SSH
  अक्षर ssh_version[30];
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  अक्षर h2_version[30];
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  अक्षर h3_version[30];
#पूर्ण_अगर
#अगर_घोषित USE_GSASL
  अक्षर gsasl_buf[30];
#पूर्ण_अगर
#अगर_घोषित HAVE_GSSAPI
  अक्षर gss_buf[40];
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_LDAP
  अक्षर ldap_buf[30];
#पूर्ण_अगर
  पूर्णांक i = 0;
  पूर्णांक j;

#अगर_घोषित DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
     set */
  स्थिर अक्षर *debugversion = दो_पर्या("CURL_DEBUG_VERSION");
  अगर(debugversion) {
    curl_msnprintf(out, माप(out), "%s", debugversion);
    वापस out;
  }
#पूर्ण_अगर

  src[i++] = LIBCURL_NAME "/" LIBCURL_VERSION;
#अगर_घोषित USE_SSL
  Curl_ssl_version(ssl_version, माप(ssl_version));
  src[i++] = ssl_version;
#पूर्ण_अगर
#अगर_घोषित HAVE_LIBZ
  curl_msnprintf(z_version, माप(z_version), "zlib/%s", zlibVersion());
  src[i++] = z_version;
#पूर्ण_अगर
#अगर_घोषित HAVE_BROTLI
  brotli_version(br_version, माप(br_version));
  src[i++] = br_version;
#पूर्ण_अगर
#अगर_घोषित HAVE_ZSTD
  zstd_version(zstd_ver, माप(zstd_ver));
  src[i++] = zstd_ver;
#पूर्ण_अगर
#अगर_घोषित USE_ARES
  curl_msnprintf(cares_version, माप(cares_version),
                 "c-ares/%s", ares_version(शून्य));
  src[i++] = cares_version;
#पूर्ण_अगर
#अगर_घोषित USE_IDN
  idn_version(idn_ver, माप(idn_ver));
  src[i++] = idn_ver;
#पूर्ण_अगर
#अगर_घोषित USE_LIBPSL
  psl_version(psl_ver, माप(psl_ver));
  src[i++] = psl_ver;
#पूर्ण_अगर
#अगर_घोषित USE_SSH
  Curl_ssh_version(ssh_version, माप(ssh_version));
  src[i++] = ssh_version;
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_version, माप(h2_version));
  src[i++] = h2_version;
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_version, माप(h3_version));
  src[i++] = h3_version;
#पूर्ण_अगर
#अगर_घोषित USE_GSASL
  curl_msnprintf(gsasl_buf, माप(gsasl_buf), "libgsasl/%s",
                 gsasl_check_version(शून्य));
  src[i++] = gsasl_buf;
#पूर्ण_अगर
#अगर_घोषित HAVE_GSSAPI
#अगर_घोषित HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, माप(gss_buf), "AppleGSS");
#या_अगर defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, माप(gss_buf), "libgss/%s", GSS_VERSION);
#या_अगर defined(CURL_KRB5_VERSION)
  curl_msnprintf(gss_buf, माप(gss_buf), "mit-krb5/%s", CURL_KRB5_VERSION);
#अन्यथा
  curl_msnprintf(gss_buf, माप(gss_buf), "mit-krb5");
#पूर्ण_अगर
  src[i++] = gss_buf;
#पूर्ण_अगर /* HAVE_GSSAPI */
#अगर_अघोषित CURL_DISABLE_LDAP
  Curl_ldap_version(ldap_buf, माप(ldap_buf));
  src[i++] = ldap_buf;
#पूर्ण_अगर

  DEBUGASSERT(i <= VERSION_PARTS);

  outp = &out[0];
  outlen = माप(out);
  क्रम(j = 0; j < i; j++) {
    माप_प्रकार n = म_माप(src[j]);
    /* we need room for a space, the string and the final zero */
    अगर(outlen <= (n + 2))
      अवरोध;
    अगर(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    स_नकल(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  वापस out;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

अटल स्थिर अक्षर * स्थिर supported_protocols[] = {
#अगर_अघोषित CURL_DISABLE_DICT
  "dict",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FILE
  "file",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_FTP
  "ftp",
#पूर्ण_अगर
#अगर defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_GOPHER
  "gopher",
#पूर्ण_अगर
#अगर defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTP
  "http",
#पूर्ण_अगर
#अगर defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_IMAP
  "imap",
#पूर्ण_अगर
#अगर defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_LDAP
  "ldap",
#अगर !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#पूर्ण_अगर
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_MQTT
  "mqtt",
#पूर्ण_अगर
#अगर defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_POP3
  "pop3",
#पूर्ण_अगर
#अगर defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_RTSP
  "rtsp",
#पूर्ण_अगर
#अगर_घोषित USE_SSH
  "scp",
  "sftp",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_SMTP
  "smtp",
#पूर्ण_अगर
#अगर defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_TELNET
  "telnet",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_TFTP
  "tftp",
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
  "ws",
#पूर्ण_अगर
#अगर defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#पूर्ण_अगर
#पूर्ण_अगर

  शून्य
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#अगर defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
अटल पूर्णांक idn_present(curl_version_info_तथ्य *info)
{
#अगर defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (व्योम)info;
  वापस TRUE;
#अन्यथा
  वापस !!info->libidn;
#पूर्ण_अगर
}
#पूर्ण_अगर

#अगर defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
अटल पूर्णांक https_proxy_present(curl_version_info_तथ्य *info)
{
  (व्योम)info;
  वापस Curl_ssl_supports(शून्य, SSLSUPP_HTTPS_PROXY);
}
#पूर्ण_अगर

#अगर defined(USE_SSL) && defined(USE_ECH)
अटल पूर्णांक ech_present(curl_version_info_तथ्य *info)
{
  (व्योम)info;
  वापस Curl_ssl_supports(शून्य, SSLSUPP_ECH);
}
#पूर्ण_अगर

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#घोषणा FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

काष्ठा feat {
  स्थिर अक्षर *name;
  पूर्णांक        (*present)(curl_version_info_तथ्य *info);
  पूर्णांक        bitmask;
};

अटल स्थिर काष्ठा feat features_table[] = {
#अगर_अघोषित CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     शून्य,                CURL_VERSION_ALTSVC),
#पूर्ण_अगर
#अगर defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     शून्य,                0),
#पूर्ण_अगर
#अगर_घोषित CURLRES_ASYNCH
  FEATURE("AsynchDNS",   शून्य,                CURL_VERSION_ASYNCHDNS),
#पूर्ण_अगर
#अगर_घोषित HAVE_BROTLI
  FEATURE("brotli",      शून्य,                CURL_VERSION_BROTLI),
#पूर्ण_अगर
#अगर_घोषित DEBUGBUILD
  FEATURE("Debug",       शून्य,                CURL_VERSION_DEBUG),
#पूर्ण_अगर
#अगर defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#अगर_अघोषित USE_HTTPSRR
#त्रुटि "ECH enabled but not HTTPSRR, must be a config error"
#पूर्ण_अगर
#पूर्ण_अगर
#अगर_घोषित USE_GSASL
  FEATURE("gsasl",       शून्य,                CURL_VERSION_GSASL),
#पूर्ण_अगर
#अगर_घोषित HAVE_GSSAPI
  FEATURE("GSS-API",     शून्य,                CURL_VERSION_GSSAPI),
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HSTS
  FEATURE("HSTS",        शून्य,                CURL_VERSION_HSTS),
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       शून्य,                CURL_VERSION_HTTP2),
#पूर्ण_अगर
#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       शून्य,                CURL_VERSION_HTTP3),
#पूर्ण_अगर
#अगर defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_VERSION_HTTPS_PROXY),
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     शून्य,                0),
#पूर्ण_अगर
#अगर_घोषित USE_HTTPSRR
  FEATURE("HTTPSRR",     शून्य,                0),
#पूर्ण_अगर
#अगर defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_VERSION_IDN),
#पूर्ण_अगर
#अगर_घोषित USE_IPV6
  FEATURE("IPv6",        शून्य,                CURL_VERSION_IPV6),
#पूर्ण_अगर
#अगर_घोषित USE_KERBEROS5
  FEATURE("Kerberos",    शून्य,                CURL_VERSION_KERBEROS5),
#पूर्ण_अगर
#अगर (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   शून्य,                CURL_VERSION_LARGEFILE),
#पूर्ण_अगर
#अगर_घोषित HAVE_LIBZ
  FEATURE("libz",        शून्य,                CURL_VERSION_LIBZ),
#पूर्ण_अगर
#अगर_घोषित CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    शून्य,                CURL_VERSION_MULTI_SSL),
#पूर्ण_अगर
#अगर_घोषित USE_NTLM
  FEATURE("NTLM",        शून्य,                CURL_VERSION_NTLM),
#पूर्ण_अगर
#अगर_घोषित USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", शून्य,                0),
#पूर्ण_अगर
#अगर_घोषित USE_LIBPSL
  FEATURE("PSL",         शून्य,                CURL_VERSION_PSL),
#पूर्ण_अगर
#अगर_घोषित USE_SSL
#अगर_घोषित USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", शून्य,              0),
#या_अगर defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    शून्य,              0),
#पूर्ण_अगर
#पूर्ण_अगर /* USE_SSL */
#अगर_घोषित USE_SPNEGO
  FEATURE("SPNEGO",      शून्य,                CURL_VERSION_SPNEGO),
#पूर्ण_अगर
#अगर_घोषित USE_SSL
  FEATURE("SSL",         शून्य,                CURL_VERSION_SSL),
#पूर्ण_अगर
#अगर_घोषित USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", शून्य,                0),
#पूर्ण_अगर
#अगर_घोषित USE_WINDOWS_SSPI
  FEATURE("SSPI",        शून्य,                CURL_VERSION_SSPI),
#पूर्ण_अगर
#अगर_घोषित GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  शून्य,                CURL_VERSION_THREADSAFE),
#पूर्ण_अगर
#अगर defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     शून्य,                CURL_VERSION_UNICODE),
#पूर्ण_अगर
#अगर_घोषित USE_UNIX_SOCKETS
  FEATURE("UnixSockets", शून्य,                CURL_VERSION_UNIX_SOCKETS),
#पूर्ण_अगर
#अगर_घोषित HAVE_ZSTD
  FEATURE("zstd",        शून्य,                CURL_VERSION_ZSTD),
#पूर्ण_अगर
  {शून्य,                 शून्य,                0}
};

अटल स्थिर अक्षर *feature_names[CURL_ARRAYSIZE(features_table)] = { शून्य };

अटल curl_version_info_तथ्य version_info = {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_अंक,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  शून्य, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  शून्य, /* zlib_version */
  supported_protocols,
  शून्य, /* c-ares version */
  0,    /* c-ares version numerical */
  शून्य, /* libidn version */
  0,    /* iconv version */
  शून्य, /* ssh lib version */
  0,    /* brotli_ver_num */
  शून्य, /* brotli version */
  0,    /* nghttp2 version number */
  शून्य, /* nghttp2 version string */
  शून्य, /* quic library string */
#अगर_घोषित CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#अन्यथा
  शून्य,
#पूर्ण_अगर
#अगर_घोषित CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#अन्यथा
  शून्य,
#पूर्ण_अगर
  0,    /* zstd_ver_num */
  शून्य, /* zstd version */
  शून्य, /* Hyper version */
  शून्य, /* gsasl version */
  feature_names,
  शून्य  /* rtmp version */
};

curl_version_info_तथ्य *curl_version_info(CURLversion stamp)
{
  माप_प्रकार n;
  स्थिर काष्ठा feat *p;
  पूर्णांक features = 0;

#अगर_घोषित USE_SSH
  अटल अक्षर ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#पूर्ण_अगर
#अगर_घोषित USE_SSL
#अगर_घोषित CURL_WITH_MULTI_SSL
  अटल अक्षर ssl_बफर[200];
#अन्यथा
  अटल अक्षर ssl_बफर[80];
#पूर्ण_अगर
#पूर्ण_अगर
#अगर_घोषित HAVE_BROTLI
  अटल अक्षर brotli_बफर[80];
#पूर्ण_अगर
#अगर_घोषित HAVE_ZSTD
  अटल अक्षर zstd_बफर[80];
#पूर्ण_अगर

  (व्योम)stamp;

#अगर_घोषित USE_SSL
  Curl_ssl_version(ssl_बफर, माप(ssl_बफर));
  version_info.ssl_version = ssl_बफर;
#पूर्ण_अगर

#अगर_घोषित HAVE_LIBZ
  version_info.libz_version = zlibVersion();
  /* libz left NULL if non-existing */
#पूर्ण_अगर
#अगर_घोषित USE_ARES
  {
    पूर्णांक aresnum;
    version_info.ares = ares_version(&aresnum);
    version_info.ares_अंक = aresnum;
  }
#पूर्ण_अगर
#अगर_घोषित USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  version_info.libidn = idn2_check_version(IDN2_VERSION);
#पूर्ण_अगर

#अगर_घोषित USE_SSH
  Curl_ssh_version(ssh_buf, माप(ssh_buf));
  version_info.libssh_version = ssh_buf;
#पूर्ण_अगर

#अगर_घोषित HAVE_BROTLI
  version_info.brotli_ver_अंक = BrotliDecoderVersion();
  brotli_version(brotli_बफर, माप(brotli_बफर));
  version_info.brotli_version = brotli_बफर;
#पूर्ण_अगर

#अगर_घोषित HAVE_ZSTD
  version_info.zstd_ver_अंक = ZSTD_version_संख्या();
  zstd_version(zstd_बफर, माप(zstd_बफर));
  version_info.zstd_version = zstd_बफर;
#पूर्ण_अगर

#अगर_घोषित USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_version(0);
    version_info.nghttp2_ver_अंक = (अचिन्हित पूर्णांक)h2->version_अंक;
    version_info.nghttp2_version = h2->version_str;
  }
#पूर्ण_अगर

#अगर !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    अटल अक्षर quicbuffer[80];
    Curl_quic_ver(quicbuffer, माप(quicbuffer));
    version_info.quic_version = quicbuffer;
  }
#पूर्ण_अगर

#अगर_घोषित USE_GSASL
  {
    version_info.gsasl_version = gsasl_check_version(शून्य);
  }
#पूर्ण_अगर

  /* Get available features, build bitmask and names array. */
  n = 0;
  क्रम(p = features_table; p->name; p++)
    अगर(!p->present || p->present(&version_info)) {
      features |= p->bitmask;
      feature_names[n++] = p->name;
    }

#अगर_घोषित DEBUGBUILD
  features |= CURL_VERSION_CURLDEBUG; /* for compatibility */
#पूर्ण_अगर

  feature_names[n] = शून्य;  /* Terminate array. */
  version_info.features = features;

  वापस &version_info;
}
