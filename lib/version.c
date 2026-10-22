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

#यदि_परिभाषितम् USE_NGHTTP2
#अन्तर्भाव <nghttp2/nghttp2.h>
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "http2.h"
#अन्तर्भाव "vssh/ssh.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "easy_lock.h"

#यदि_परिभाषितम् USE_ARES
#  include <ares.h>
#यदि_अन्तः

#यदि_परिभाषितम् USE_LIBIDN2
#अन्तर्भाव <idn2.h>
#यदि_अन्तः

#यदि_परिभाषितम् USE_LIBPSL
#अन्तर्भाव <libpsl.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_LIBZ
#अन्तर्भाव <zlib.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_BROTLI
#यदि_परिभाषितम् CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Wvla"
#यदि_अन्तः
#अन्तर्भाव <brotli/decode.h>
#यदि_परिभाषितम् CURL_HAVE_DIAG
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_ZSTD
#अन्तर्भाव <zstd.h>
#यदि_अन्तः

#यदि_परिभाषितम् USE_GSASL
#अन्तर्भाव <gsasl.h>
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_LDAP
#अन्तर्भाव "curl_ldap.h"
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_BROTLI
स्थिर शून्य brotli_संस्करणम्(वर्ण *buf, size_t bufsz)
{
  uint32_t brotli_संस्करणम् = Brotli_Decoder_संस्करणम्();
  अचिह्नित पूर्णाङ्क major = brotli_संस्करणम् >> 24;
  अचिह्नित पूर्णाङ्क minor = (brotli_संस्करणम् & 0x00FFFFFF) >> 12;
  अचिह्नित पूर्णाङ्क patch = brotli_संस्करणम् & 0x00000FFF;
  (शून्य)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_ZSTD
स्थिर शून्य zstd_संस्करणम्(वर्ण *buf, size_t bufsz)
{
  अचिह्नित पूर्णाङ्क संस्करणम्_2 = ZSTD_संस्करणम्_सङ्ख्या();
  अचिह्नित पूर्णाङ्क major = संस्करणम्_2 / (100 * 100);
  अचिह्नित पूर्णाङ्क minor = (संस्करणम्_2 - (major * 100 * 100)) / 100;
  अचिह्नित पूर्णाङ्क patch = संस्करणम्_2 - (major * 100 * 100) - (minor * 100);
  (शून्य)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#यदि_अन्तः

#यदि_परिभाषितम् USE_LIBPSL
स्थिर शून्य psl_संस्करणम्(वर्ण *buf, size_t bufsz)
{
#यदि defined(PSL_संस्करणम्_MAJOR) && (PSL_संस्करणम्_MAJOR > 0 ||     \
                                   PSL_संस्करणम्_MINOR >= 11)
  पूर्णाङ्क अङ्कः = psl_check_संस्करणम्_सङ्ख्या(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 अङ्कः >> 16, (अङ्कः >> 8) & 0xff, अङ्कः & 0xff);
#अन्यथा
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_संस्करणम्());
#यदि_अन्तः
}
#यदि_अन्तः

#यदि defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#परिभाषय USE_IDN
#यदि_अन्तः

#यदि_परिभाषितम् USE_IDN
स्थिर शून्य idn_संस्करणम्(वर्ण *buf, size_t bufsz)
{
#यदि_परिभाषितम् USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_संस्करणम्(रिक्त));
#नोचेद्यदि defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#नोचेद्यदि defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#यदि_अन्तः
}
#यदि_अन्तः

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#परिभाषय संस्करणम्_PARTS 16 /* number of substrings we can concatenate */

वर्ण *curl_संस्करणम्(शून्य)
{
  स्थिर वर्ण बहिर्गमनम्[300];
  वर्ण *outp;
  size_t outlen;
  ध्रुव वर्ण *src[संस्करणम्_PARTS];
#यदि_परिभाषितम् USE_SSL
  वर्ण ssl_संस्करणम्[200];
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_LIBZ
  वर्ण z_संस्करणम्[30];
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_BROTLI
  वर्ण br_संस्करणम्[30];
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ZSTD
  वर्ण zstd_ver[30];
#यदि_अन्तः
#यदि_परिभाषितम् USE_ARES
  वर्ण cares_संस्करणम्[30];
#यदि_अन्तः
#यदि_परिभाषितम् USE_IDN
  वर्ण idn_ver[30];
#यदि_अन्तः
#यदि_परिभाषितम् USE_LIBPSL
  वर्ण psl_ver[30];
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
  वर्ण ssh_संस्करणम्[30];
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  वर्ण h2_संस्करणम्[30];
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  वर्ण h3_संस्करणम्[30];
#यदि_अन्तः
#यदि_परिभाषितम् USE_GSASL
  वर्ण gsasl_buf[30];
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_GSSAPI
  वर्ण gss_buf[40];
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_LDAP
  वर्ण ldap_buf[30];
#यदि_अन्तः
  पूर्णाङ्क i = 0;
  पूर्णाङ्क j;

#यदि_परिभाषितम् DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
समुच्चयः*/
  ध्रुव वर्ण *debugversion = getenv("CURL_DEBUG_VERSION");
  यदि(debugversion) {
    curl_msnprintf(बहिर्गमनम्, परिमाण(बहिर्गमनम्), "%s", debugversion);
    प्रत्यागम बहिर्गमनम्;
  }
#यदि_अन्तः

  src[i++] = LIBCURL_NAME "/" LIBCURL_संस्करणम्;
#यदि_परिभाषितम् USE_SSL
  Curl_ssl_संस्करणम्(ssl_संस्करणम्, परिमाण(ssl_संस्करणम्));
  src[i++] = ssl_संस्करणम्;
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_LIBZ
  curl_msnprintf(z_संस्करणम्, परिमाण(z_संस्करणम्), "zlib/%s", zlib_संस्करणम्());
  src[i++] = z_संस्करणम्;
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_BROTLI
  brotli_संस्करणम्(br_संस्करणम्, परिमाण(br_संस्करणम्));
  src[i++] = br_संस्करणम्;
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ZSTD
  zstd_संस्करणम्(zstd_ver, परिमाण(zstd_ver));
  src[i++] = zstd_ver;
#यदि_अन्तः
#यदि_परिभाषितम् USE_ARES
  curl_msnprintf(cares_संस्करणम्, परिमाण(cares_संस्करणम्),
                 "c-ares/%s", ares_संस्करणम्(रिक्त));
  src[i++] = cares_संस्करणम्;
#यदि_अन्तः
#यदि_परिभाषितम् USE_IDN
  idn_संस्करणम्(idn_ver, परिमाण(idn_ver));
  src[i++] = idn_ver;
#यदि_अन्तः
#यदि_परिभाषितम् USE_LIBPSL
  psl_संस्करणम्(psl_ver, परिमाण(psl_ver));
  src[i++] = psl_ver;
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
  Curl_ssh_संस्करणम्(ssh_संस्करणम्, परिमाण(ssh_संस्करणम्));
  src[i++] = ssh_संस्करणम्;
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_संस्करणम्, परिमाण(h2_संस्करणम्));
  src[i++] = h2_संस्करणम्;
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_संस्करणम्, परिमाण(h3_संस्करणम्));
  src[i++] = h3_संस्करणम्;
#यदि_अन्तः
#यदि_परिभाषितम् USE_GSASL
  curl_msnprintf(gsasl_buf, परिमाण(gsasl_buf), "libgsasl/%s",
                 gsasl_check_संस्करणम्(रिक्त));
  src[i++] = gsasl_buf;
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_GSSAPI
#यदि_परिभाषितम् HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, परिमाण(gss_buf), "AppleGSS");
#नोचेद्यदि defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, परिमाण(gss_buf), "libgss/%s", GSS_संस्करणम्);
#नोचेद्यदि defined(CURL_KRB5_संस्करणम्)
  curl_msnprintf(gss_buf, परिमाण(gss_buf), "mit-krb5/%s", CURL_KRB5_संस्करणम्);
#अन्यथा
  curl_msnprintf(gss_buf, परिमाण(gss_buf), "mit-krb5");
#यदि_अन्तः
  src[i++] = gss_buf;
#यदि_अन्तः /* HAVE_GSSAPI */
#यदि_अपरिभाषितम् CURL_DISABLE_LDAP
  Curl_ldap_संस्करणम्(ldap_buf, परिमाण(ldap_buf));
  src[i++] = ldap_buf;
#यदि_अन्तः

  DEBUGASSERT(i <= संस्करणम्_PARTS);

  outp = &बहिर्गमनम्[0];
  outlen = परिमाण(बहिर्गमनम्);
  प्रति(j = 0; j < i; j++) {
    size_t n = वाक्यदैर्घ्यम्(src[j]);
    /* we need room for a space, the string and the final zero */
    यदि(outlen <= (n + 2))
      भङ्ग;
    यदि(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    स्मृतिं_प्रतिलिख(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  प्रत्यागम बहिर्गमनम्;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

स्थिर ध्रुव वर्ण * ध्रुव supported_protocols[] = {
#यदि_अपरिभाषितम् CURL_DISABLE_DICT
  "dict",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FILE
  "file",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_FTP
  "ftp",
#यदि_अन्तः
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_GOPHER
  "gopher",
#यदि_अन्तः
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  "http",
#यदि_अन्तः
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_IMAP
  "imap",
#यदि_अन्तः
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_LDAP
  "ldap",
#यदि !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#यदि_अन्तः
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_MQTT
  "mqtt",
#यदि_अन्तः
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_POP3
  "pop3",
#यदि_अन्तः
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_RTSP
  "rtsp",
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSH
  "scp",
  "sftp",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_SMTP
  "smtp",
#यदि_अन्तः
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_TELNET
  "telnet",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_TFTP
  "tftp",
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
  "ws",
#यदि_अन्तः
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#यदि_अन्तः
#यदि_अन्तः

  रिक्त
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#यदि defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
स्थिर पूर्णाङ्क idn_present(curl_संस्करणम्_info_दत्त *info)
{
#यदि defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (शून्य)info;
  प्रत्यागम TRUE;
#अन्यथा
  प्रत्यागम !!info->libidn;
#यदि_अन्तः
}
#यदि_अन्तः

#यदि defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
स्थिर पूर्णाङ्क https_proxy_present(curl_संस्करणम्_info_दत्त *info)
{
  (शून्य)info;
  प्रत्यागम Curl_ssl_supports(रिक्त, SSLSUPP_HTTPS_PROXY);
}
#यदि_अन्तः

#यदि defined(USE_SSL) && defined(USE_ECH)
स्थिर पूर्णाङ्क ech_present(curl_संस्करणम्_info_दत्त *info)
{
  (शून्य)info;
  प्रत्यागम Curl_ssl_supports(रिक्त, SSLSUPP_ECH);
}
#यदि_अन्तः

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#परिभाषय FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

संरचना feat {
  ध्रुव वर्ण *name;
  पूर्णाङ्क        (*present)(curl_संस्करणम्_info_दत्त *info);
  पूर्णाङ्क        bitmask;
};

स्थिर ध्रुव संरचना feat features_सारणी[] = {
#यदि_अपरिभाषितम् CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     रिक्त,                CURL_संस्करणम्_ALTSVC),
#यदि_अन्तः
#यदि defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     रिक्त,                0),
#यदि_अन्तः
#यदि_परिभाषितम् CURLRES_ASYNCH
  FEATURE("AsynchDNS",   रिक्त,                CURL_संस्करणम्_ASYNCHDNS),
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_BROTLI
  FEATURE("brotli",      रिक्त,                CURL_संस्करणम्_BROTLI),
#यदि_अन्तः
#यदि_परिभाषितम् DEBUGBUILD
  FEATURE("Debug",       रिक्त,                CURL_संस्करणम्_DEBUG),
#यदि_अन्तः
#यदि defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#यदि_अपरिभाषितम् USE_HTTPSRR
#दोषः "ECH enabled but not HTTPSRR, must be a config error"
#यदि_अन्तः
#यदि_अन्तः
#यदि_परिभाषितम् USE_GSASL
  FEATURE("gsasl",       रिक्त,                CURL_संस्करणम्_GSASL),
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_GSSAPI
  FEATURE("GSS-API",     रिक्त,                CURL_संस्करणम्_GSSAPI),
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
  FEATURE("HSTS",        रिक्त,                CURL_संस्करणम्_HSTS),
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       रिक्त,                CURL_संस्करणम्_HTTP2),
#यदि_अन्तः
#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       रिक्त,                CURL_संस्करणम्_HTTP3),
#यदि_अन्तः
#यदि defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_संस्करणम्_HTTPS_PROXY),
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     रिक्त,                0),
#यदि_अन्तः
#यदि_परिभाषितम् USE_HTTPSRR
  FEATURE("HTTPSRR",     रिक्त,                0),
#यदि_अन्तः
#यदि defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_संस्करणम्_IDN),
#यदि_अन्तः
#यदि_परिभाषितम् USE_IPV6
  FEATURE("IPv6",        रिक्त,                CURL_संस्करणम्_IPV6),
#यदि_अन्तः
#यदि_परिभाषितम् USE_KERBEROS5
  FEATURE("Kerberos",    रिक्त,                CURL_संस्करणम्_KERBEROS5),
#यदि_अन्तः
#यदि (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   रिक्त,                CURL_संस्करणम्_LARGEFILE),
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_LIBZ
  FEATURE("libz",        रिक्त,                CURL_संस्करणम्_LIBZ),
#यदि_अन्तः
#यदि_परिभाषितम् CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    रिक्त,                CURL_संस्करणम्_MULTI_SSL),
#यदि_अन्तः
#यदि_परिभाषितम् USE_NTLM
  FEATURE("NTLM",        रिक्त,                CURL_संस्करणम्_NTLM),
#यदि_अन्तः
#यदि_परिभाषितम् USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", रिक्त,                0),
#यदि_अन्तः
#यदि_परिभाषितम् USE_LIBPSL
  FEATURE("PSL",         रिक्त,                CURL_संस्करणम्_PSL),
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSL
#यदि_परिभाषितम् USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", रिक्त,              0),
#नोचेद्यदि defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    रिक्त,              0),
#यदि_अन्तः
#यदि_अन्तः /* USE_SSL */
#यदि_परिभाषितम् USE_SPNEGO
  FEATURE("SPNEGO",      रिक्त,                CURL_संस्करणम्_SPNEGO),
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSL
  FEATURE("SSL",         रिक्त,                CURL_संस्करणम्_SSL),
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", रिक्त,                0),
#यदि_अन्तः
#यदि_परिभाषितम् USE_WINDOWS_SSPI
  FEATURE("SSPI",        रिक्त,                CURL_संस्करणम्_SSPI),
#यदि_अन्तः
#यदि_परिभाषितम् GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  रिक्त,                CURL_संस्करणम्_THREADSAFE),
#यदि_अन्तः
#यदि defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     रिक्त,                CURL_संस्करणम्_UNICODE),
#यदि_अन्तः
#यदि_परिभाषितम् USE_UNIX_SOCKETS
  FEATURE("UnixSockets", रिक्त,                CURL_संस्करणम्_UNIX_SOCKETS),
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ZSTD
  FEATURE("zstd",        रिक्त,                CURL_संस्करणम्_ZSTD),
#यदि_अन्तः
  {रिक्त,                 रिक्त,                0}
};

स्थिर ध्रुव वर्ण *feature_नामानि[CURL_ARRAYSIZE(features_सारणी)] = { रिक्त };

स्थिर curl_संस्करणम्_info_दत्त संस्करणम्_info = {
  CURLVERSION_NOW,
  LIBCURL_संस्करणम्,
  LIBCURL_संस्करणम्_अङ्कः,
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
#यदि_परिभाषितम् CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#अन्यथा
  रिक्त,
#यदि_अन्तः
#यदि_परिभाषितम् CURL_CA_मार्गः
  CURL_CA_मार्गः,  /* capath */
#अन्यथा
  रिक्त,
#यदि_अन्तः
  0,    /* zstd_ver_num */
  रिक्त, /* zstd version */
  रिक्त, /* Hyper version */
  रिक्त, /* gsasl version */
  feature_नामानि,
  रिक्त  /* rtmp version */
};

curl_संस्करणम्_info_दत्त *curl_संस्करणम्_info(CURLversion stamp)
{
  size_t n;
  ध्रुव संरचना feat *p;
  पूर्णाङ्क features = 0;

#यदि_परिभाषितम् USE_SSH
  स्थिर वर्ण ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#यदि_अन्तः
#यदि_परिभाषितम् USE_SSL
#यदि_परिभाषितम् CURL_WITH_MULTI_SSL
  स्थिर वर्ण ssl_बफर[200];
#अन्यथा
  स्थिर वर्ण ssl_बफर[80];
#यदि_अन्तः
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_BROTLI
  स्थिर वर्ण brotli_बफर[80];
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ZSTD
  स्थिर वर्ण zstd_बफर[80];
#यदि_अन्तः

  (शून्य)stamp;

#यदि_परिभाषितम् USE_SSL
  Curl_ssl_संस्करणम्(ssl_बफर, परिमाण(ssl_बफर));
  संस्करणम्_info.ssl_संस्करणम् = ssl_बफर;
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_LIBZ
  संस्करणम्_info.libz_संस्करणम् = zlib_संस्करणम्();
  /* libz left NULL if non-existing */
#यदि_अन्तः
#यदि_परिभाषितम् USE_ARES
  {
    पूर्णाङ्क aresnum;
    संस्करणम्_info.ares = ares_संस्करणम्(&aresnum);
    संस्करणम्_info.ares_अङ्कः = aresnum;
  }
#यदि_अन्तः
#यदि_परिभाषितम् USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  संस्करणम्_info.libidn = idn2_check_संस्करणम्(IDN2_संस्करणम्);
#यदि_अन्तः

#यदि_परिभाषितम् USE_SSH
  Curl_ssh_संस्करणम्(ssh_buf, परिमाण(ssh_buf));
  संस्करणम्_info.libssh_संस्करणम् = ssh_buf;
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_BROTLI
  संस्करणम्_info.brotli_ver_अङ्कः = Brotli_Decoder_संस्करणम्();
  brotli_संस्करणम्(brotli_बफर, परिमाण(brotli_बफर));
  संस्करणम्_info.brotli_संस्करणम् = brotli_बफर;
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_ZSTD
  संस्करणम्_info.zstd_ver_अङ्कः = ZSTD_संस्करणम्_सङ्ख्या();
  zstd_संस्करणम्(zstd_बफर, परिमाण(zstd_बफर));
  संस्करणम्_info.zstd_संस्करणम् = zstd_बफर;
#यदि_अन्तः

#यदि_परिभाषितम् USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_संस्करणम्(0);
    संस्करणम्_info.nghttp2_ver_अङ्कः = (अचिह्नित पूर्णाङ्क)h2->संस्करणम्_अङ्कः;
    संस्करणम्_info.nghttp2_संस्करणम् = h2->संस्करणम्_str;
  }
#यदि_अन्तः

#यदि !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    स्थिर वर्ण quicbuffer[80];
    Curl_quic_ver(quicbuffer, परिमाण(quicbuffer));
    संस्करणम्_info.quic_संस्करणम् = quicbuffer;
  }
#यदि_अन्तः

#यदि_परिभाषितम् USE_GSASL
  {
    संस्करणम्_info.gsasl_संस्करणम् = gsasl_check_संस्करणम्(रिक्त);
  }
#यदि_अन्तः

  /* Get available features, build bitmask and names array. */
  n = 0;
  प्रति(p = features_सारणी; p->name; p++)
    यदि(!p->present || p->present(&संस्करणम्_info)) {
      features |= p->bitmask;
      feature_नामानि[n++] = p->name;
    }

#यदि_परिभाषितम् DEBUGBUILD
  features |= CURL_संस्करणम्_CURLDEBUG; /* for compatibility */
#यदि_अन्तः

  feature_नामानि[n] = रिक्त;  /* Terminate array. */
  संस्करणम्_info.features = features;

  प्रत्यागम &संस्करणम्_info;
}
