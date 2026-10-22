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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2
#ਅਨ੍ਤਰ੍ਭਾਵ <nghttp2/nghttp2.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/ssh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "easy_lock.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
#  include <ares.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBIDN2
#ਅਨ੍ਤਰ੍ਭਾਵ <idn2.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
#ਅਨ੍ਤਰ੍ਭਾਵ <libpsl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
#ਅਨ੍ਤਰ੍ਭਾਵ <zlib.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wvla"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <brotli/decode.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_HAVE_DIAG
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
#ਅਨ੍ਤਰ੍ਭਾਵ <zstd.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
#ਅਨ੍ਤਰ੍ਭਾਵ <gsasl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_LDAP
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ldap.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
ਸਥਿਰ ਖਾਲੀ brotli_ਸੰਸਕਰਣ(ਅੱਖਰ *buf, size_t bufsz)
{
  uint32_t brotli_ਸੰਸਕਰਣ = Brotli_Decoder_ਸੰਸਕਰਣ();
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ major = brotli_ਸੰਸਕਰਣ >> 24;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ minor = (brotli_ਸੰਸਕਰਣ & 0x00FFFFFF) >> 12;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ patch = brotli_ਸੰਸਕਰਣ & 0x00000FFF;
  (ਖਾਲੀ)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
ਸਥਿਰ ਖਾਲੀ zstd_ਸੰਸਕਰਣ(ਅੱਖਰ *buf, size_t bufsz)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੰਸਕਰਣ_2 = ZSTD_ਸੰਸਕਰਣ_ਸੰਖਿਆ();
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ major = ਸੰਸਕਰਣ_2 / (100 * 100);
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ minor = (ਸੰਸਕਰਣ_2 - (major * 100 * 100)) / 100;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ patch = ਸੰਸਕਰਣ_2 - (major * 100 * 100) - (minor * 100);
  (ਖਾਲੀ)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
ਸਥਿਰ ਖਾਲੀ psl_ਸੰਸਕਰਣ(ਅੱਖਰ *buf, size_t bufsz)
{
#ਜੇ defined(PSL_ਸੰਸਕਰਣ_MAJOR) && (PSL_ਸੰਸਕਰਣ_MAJOR > 0 ||     \
                                   PSL_ਸੰਸਕਰਣ_MINOR >= 11)
  ਪੂਰਨ ਅੰਕ = psl_check_ਸੰਸਕਰਣ_ਸੰਖਿਆ(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 ਅੰਕ >> 16, (ਅੰਕ >> 8) & 0xff, ਅੰਕ & 0xff);
#ਨਹੀਂਤਾਂ
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_ਸੰਸਕਰਣ());
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_IDN
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IDN
ਸਥਿਰ ਖਾਲੀ idn_ਸੰਸਕਰਣ(ਅੱਖਰ *buf, size_t bufsz)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_ਸੰਸਕਰਣ(ਨਿਰਾ));
#ਜਾਂਜੇ defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#ਜਾਂਜੇ defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੰਸਕਰਣ_PARTS 16 /* number of substrings we can concatenate */

ਅੱਖਰ *curl_ਸੰਸਕਰਣ(ਖਾਲੀ)
{
  ਸਥਿਰ ਅੱਖਰ ਬਾਹਰਲਾ[300];
  ਅੱਖਰ *outp;
  size_t outlen;
  ਅਚਲ ਅੱਖਰ *src[ਸੰਸਕਰਣ_PARTS];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  ਅੱਖਰ ssl_ਸੰਸਕਰਣ[200];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
  ਅੱਖਰ z_ਸੰਸਕਰਣ[30];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
  ਅੱਖਰ br_ਸੰਸਕਰਣ[30];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
  ਅੱਖਰ zstd_ver[30];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
  ਅੱਖਰ cares_ਸੰਸਕਰਣ[30];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IDN
  ਅੱਖਰ idn_ver[30];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  ਅੱਖਰ psl_ver[30];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  ਅੱਖਰ ssh_ਸੰਸਕਰਣ[30];
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  ਅੱਖਰ h2_ਸੰਸਕਰਣ[30];
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  ਅੱਖਰ h3_ਸੰਸਕਰਣ[30];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
  ਅੱਖਰ gsasl_buf[30];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSSAPI
  ਅੱਖਰ gss_buf[40];
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_LDAP
  ਅੱਖਰ ldap_buf[30];
#ਜੇ_ਅੰਤ
  ਪੂਰਨ i = 0;
  ਪੂਰਨ j;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
ਸੈੱਟ*/
  ਅਚਲ ਅੱਖਰ *debugversion = getenv("CURL_DEBUG_VERSION");
  ਜੇ(debugversion) {
    curl_msnprintf(ਬਾਹਰਲਾ, ਆਕਾਰ(ਬਾਹਰਲਾ), "%s", debugversion);
    ਵਾਪਸ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ

  src[i++] = LIBCURL_NAME "/" LIBCURL_ਸੰਸਕਰਣ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  Curl_ssl_ਸੰਸਕਰਣ(ssl_ਸੰਸਕਰਣ, ਆਕਾਰ(ssl_ਸੰਸਕਰਣ));
  src[i++] = ssl_ਸੰਸਕਰਣ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
  curl_msnprintf(z_ਸੰਸਕਰਣ, ਆਕਾਰ(z_ਸੰਸਕਰਣ), "zlib/%s", zlib_ਸੰਸਕਰਣ());
  src[i++] = z_ਸੰਸਕਰਣ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
  brotli_ਸੰਸਕਰਣ(br_ਸੰਸਕਰਣ, ਆਕਾਰ(br_ਸੰਸਕਰਣ));
  src[i++] = br_ਸੰਸਕਰਣ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
  zstd_ਸੰਸਕਰਣ(zstd_ver, ਆਕਾਰ(zstd_ver));
  src[i++] = zstd_ver;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
  curl_msnprintf(cares_ਸੰਸਕਰਣ, ਆਕਾਰ(cares_ਸੰਸਕਰਣ),
                 "c-ares/%s", ares_ਸੰਸਕਰਣ(ਨਿਰਾ));
  src[i++] = cares_ਸੰਸਕਰਣ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IDN
  idn_ਸੰਸਕਰਣ(idn_ver, ਆਕਾਰ(idn_ver));
  src[i++] = idn_ver;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  psl_ਸੰਸਕਰਣ(psl_ver, ਆਕਾਰ(psl_ver));
  src[i++] = psl_ver;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  Curl_ssh_ਸੰਸਕਰਣ(ssh_ਸੰਸਕਰਣ, ਆਕਾਰ(ssh_ਸੰਸਕਰਣ));
  src[i++] = ssh_ਸੰਸਕਰਣ;
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_ਸੰਸਕਰਣ, ਆਕਾਰ(h2_ਸੰਸਕਰਣ));
  src[i++] = h2_ਸੰਸਕਰਣ;
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_ਸੰਸਕਰਣ, ਆਕਾਰ(h3_ਸੰਸਕਰਣ));
  src[i++] = h3_ਸੰਸਕਰਣ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
  curl_msnprintf(gsasl_buf, ਆਕਾਰ(gsasl_buf), "libgsasl/%s",
                 gsasl_check_ਸੰਸਕਰਣ(ਨਿਰਾ));
  src[i++] = gsasl_buf;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSSAPI
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, ਆਕਾਰ(gss_buf), "AppleGSS");
#ਜਾਂਜੇ defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, ਆਕਾਰ(gss_buf), "libgss/%s", GSS_ਸੰਸਕਰਣ);
#ਜਾਂਜੇ defined(CURL_KRB5_ਸੰਸਕਰਣ)
  curl_msnprintf(gss_buf, ਆਕਾਰ(gss_buf), "mit-krb5/%s", CURL_KRB5_ਸੰਸਕਰਣ);
#ਨਹੀਂਤਾਂ
  curl_msnprintf(gss_buf, ਆਕਾਰ(gss_buf), "mit-krb5");
#ਜੇ_ਅੰਤ
  src[i++] = gss_buf;
#ਜੇ_ਅੰਤ /* HAVE_GSSAPI */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_LDAP
  Curl_ldap_ਸੰਸਕਰਣ(ldap_buf, ਆਕਾਰ(ldap_buf));
  src[i++] = ldap_buf;
#ਜੇ_ਅੰਤ

  DEBUGASSERT(i <= ਸੰਸਕਰਣ_PARTS);

  outp = &ਬਾਹਰਲਾ[0];
  outlen = ਆਕਾਰ(ਬਾਹਰਲਾ);
  ਲਈ(j = 0; j < i; j++) {
    size_t n = ਲੜੀ_ਲੰਬਾਈ(src[j]);
    /* we need room for a space, the string and the final zero */
    ਜੇ(outlen <= (n + 2))
      ਰੋਕੋ;
    ਜੇ(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  ਵਾਪਸ ਬਾਹਰਲਾ;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ supported_protocols[] = {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DICT
  "dict",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FILE
  "file",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  "ftp",
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_GOPHER
  "gopher",
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  "http",
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_IMAP
  "imap",
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_LDAP
  "ldap",
#ਜੇ !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MQTT
  "mqtt",
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_POP3
  "pop3",
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  "rtsp",
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  "scp",
  "sftp",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
  "smtp",
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TELNET
  "telnet",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TFTP
  "tftp",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
  "ws",
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

  ਨਿਰਾ
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#ਜੇ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ਸਥਿਰ ਪੂਰਨ idn_present(curl_ਸੰਸਕਰਣ_info_ਅੰਕੜੇ *info)
{
#ਜੇ defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (ਖਾਲੀ)info;
  ਵਾਪਸ TRUE;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ !!info->libidn;
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ਸਥਿਰ ਪੂਰਨ https_proxy_present(curl_ਸੰਸਕਰਣ_info_ਅੰਕੜੇ *info)
{
  (ਖਾਲੀ)info;
  ਵਾਪਸ Curl_ssl_supports(ਨਿਰਾ, SSLSUPP_HTTPS_PROXY);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_SSL) && defined(USE_ECH)
ਸਥਿਰ ਪੂਰਨ ech_present(curl_ਸੰਸਕਰਣ_info_ਅੰਕੜੇ *info)
{
  (ਖਾਲੀ)info;
  ਵਾਪਸ Curl_ssl_supports(ਨਿਰਾ, SSLSUPP_ECH);
}
#ਜੇ_ਅੰਤ

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

ਬਣਤਰ feat {
  ਅਚਲ ਅੱਖਰ *name;
  ਪੂਰਨ        (*present)(curl_ਸੰਸਕਰਣ_info_ਅੰਕੜੇ *info);
  ਪੂਰਨ        bitmask;
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ feat features_ਸਾਰਣੀ[] = {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_ALTSVC),
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     ਨਿਰਾ,                0),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLRES_ASYNCH
  FEATURE("AsynchDNS",   ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_ASYNCHDNS),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
  FEATURE("brotli",      ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_BROTLI),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  FEATURE("Debug",       ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_DEBUG),
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
#ਗਲਤੀ "ECH enabled but not HTTPSRR, must be a config error"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
  FEATURE("gsasl",       ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_GSASL),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GSSAPI
  FEATURE("GSS-API",     ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_GSSAPI),
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
  FEATURE("HSTS",        ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_HSTS),
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_HTTP2),
#ਜੇ_ਅੰਤ
#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_HTTP3),
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_ਸੰਸਕਰਣ_HTTPS_PROXY),
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     ਨਿਰਾ,                0),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTPSRR
  FEATURE("HTTPSRR",     ਨਿਰਾ,                0),
#ਜੇ_ਅੰਤ
#ਜੇ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_ਸੰਸਕਰਣ_IDN),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  FEATURE("IPv6",        ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_IPV6),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_KERBEROS5
  FEATURE("Kerberos",    ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_KERBEROS5),
#ਜੇ_ਅੰਤ
#ਜੇ (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_LARGEFILE),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
  FEATURE("libz",        ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_LIBZ),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_MULTI_SSL),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NTLM
  FEATURE("NTLM",        ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_NTLM),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", ਨਿਰਾ,                0),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBPSL
  FEATURE("PSL",         ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_PSL),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", ਨਿਰਾ,              0),
#ਜਾਂਜੇ defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    ਨਿਰਾ,              0),
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* USE_SSL */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SPNEGO
  FEATURE("SPNEGO",      ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_SPNEGO),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  FEATURE("SSL",         ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_SSL),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", ਨਿਰਾ,                0),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  FEATURE("SSPI",        ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_SSPI),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_THREADSAFE),
#ਜੇ_ਅੰਤ
#ਜੇ defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_UNICODE),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_UNIX_SOCKETS
  FEATURE("UnixSockets", ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_UNIX_SOCKETS),
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
  FEATURE("zstd",        ਨਿਰਾ,                CURL_ਸੰਸਕਰਣ_ZSTD),
#ਜੇ_ਅੰਤ
  {ਨਿਰਾ,                 ਨਿਰਾ,                0}
};

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *feature_ਨਾਂ[CURL_ARRAYSIZE(features_ਸਾਰਣੀ)] = { ਨਿਰਾ };

ਸਥਿਰ curl_ਸੰਸਕਰਣ_info_ਅੰਕੜੇ ਸੰਸਕਰਣ_info = {
  CURLVERSION_NOW,
  LIBCURL_ਸੰਸਕਰਣ,
  LIBCURL_ਸੰਸਕਰਣ_ਅੰਕ,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  ਨਿਰਾ, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  ਨਿਰਾ, /* zlib_version */
  supported_protocols,
  ਨਿਰਾ, /* c-ares version */
  0,    /* c-ares version numerical */
  ਨਿਰਾ, /* libidn version */
  0,    /* iconv version */
  ਨਿਰਾ, /* ssh lib version */
  0,    /* brotli_ver_num */
  ਨਿਰਾ, /* brotli version */
  0,    /* nghttp2 version number */
  ਨਿਰਾ, /* nghttp2 version string */
  ਨਿਰਾ, /* quic library string */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#ਨਹੀਂਤਾਂ
  ਨਿਰਾ,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CA_ਰਸਤਾ
  CURL_CA_ਰਸਤਾ,  /* capath */
#ਨਹੀਂਤਾਂ
  ਨਿਰਾ,
#ਜੇ_ਅੰਤ
  0,    /* zstd_ver_num */
  ਨਿਰਾ, /* zstd version */
  ਨਿਰਾ, /* Hyper version */
  ਨਿਰਾ, /* gsasl version */
  feature_ਨਾਂ,
  ਨਿਰਾ  /* rtmp version */
};

curl_ਸੰਸਕਰਣ_info_ਅੰਕੜੇ *curl_ਸੰਸਕਰਣ_info(CURLversion stamp)
{
  size_t n;
  ਅਚਲ ਬਣਤਰ feat *p;
  ਪੂਰਨ features = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  ਸਥਿਰ ਅੱਖਰ ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_WITH_MULTI_SSL
  ਸਥਿਰ ਅੱਖਰ ssl_ਬਫਰ[200];
#ਨਹੀਂਤਾਂ
  ਸਥਿਰ ਅੱਖਰ ssl_ਬਫਰ[80];
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
  ਸਥਿਰ ਅੱਖਰ brotli_ਬਫਰ[80];
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
  ਸਥਿਰ ਅੱਖਰ zstd_ਬਫਰ[80];
#ਜੇ_ਅੰਤ

  (ਖਾਲੀ)stamp;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  Curl_ssl_ਸੰਸਕਰਣ(ssl_ਬਫਰ, ਆਕਾਰ(ssl_ਬਫਰ));
  ਸੰਸਕਰਣ_info.ssl_ਸੰਸਕਰਣ = ssl_ਬਫਰ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LIBZ
  ਸੰਸਕਰਣ_info.libz_ਸੰਸਕਰਣ = zlib_ਸੰਸਕਰਣ();
  /* libz left NULL if non-existing */
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ARES
  {
    ਪੂਰਨ aresnum;
    ਸੰਸਕਰਣ_info.ares = ares_ਸੰਸਕਰਣ(&aresnum);
    ਸੰਸਕਰਣ_info.ares_ਅੰਕ = aresnum;
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  ਸੰਸਕਰਣ_info.libidn = idn2_check_ਸੰਸਕਰਣ(IDN2_ਸੰਸਕਰਣ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH
  Curl_ssh_ਸੰਸਕਰਣ(ssh_buf, ਆਕਾਰ(ssh_buf));
  ਸੰਸਕਰਣ_info.libssh_ਸੰਸਕਰਣ = ssh_buf;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BROTLI
  ਸੰਸਕਰਣ_info.brotli_ver_ਅੰਕ = Brotli_Decoder_ਸੰਸਕਰਣ();
  brotli_ਸੰਸਕਰਣ(brotli_ਬਫਰ, ਆਕਾਰ(brotli_ਬਫਰ));
  ਸੰਸਕਰਣ_info.brotli_ਸੰਸਕਰਣ = brotli_ਬਫਰ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ZSTD
  ਸੰਸਕਰਣ_info.zstd_ver_ਅੰਕ = ZSTD_ਸੰਸਕਰਣ_ਸੰਖਿਆ();
  zstd_ਸੰਸਕਰਣ(zstd_ਬਫਰ, ਆਕਾਰ(zstd_ਬਫਰ));
  ਸੰਸਕਰਣ_info.zstd_ਸੰਸਕਰਣ = zstd_ਬਫਰ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_ਸੰਸਕਰਣ(0);
    ਸੰਸਕਰਣ_info.nghttp2_ver_ਅੰਕ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)h2->ਸੰਸਕਰਣ_ਅੰਕ;
    ਸੰਸਕਰਣ_info.nghttp2_ਸੰਸਕਰਣ = h2->ਸੰਸਕਰਣ_str;
  }
#ਜੇ_ਅੰਤ

#ਜੇ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ਸਥਿਰ ਅੱਖਰ quicbuffer[80];
    Curl_quic_ver(quicbuffer, ਆਕਾਰ(quicbuffer));
    ਸੰਸਕਰਣ_info.quic_ਸੰਸਕਰਣ = quicbuffer;
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GSASL
  {
    ਸੰਸਕਰਣ_info.gsasl_ਸੰਸਕਰਣ = gsasl_check_ਸੰਸਕਰਣ(ਨਿਰਾ);
  }
#ਜੇ_ਅੰਤ

  /* Get available features, build bitmask and names array. */
  n = 0;
  ਲਈ(p = features_ਸਾਰਣੀ; p->name; p++)
    ਜੇ(!p->present || p->present(&ਸੰਸਕਰਣ_info)) {
      features |= p->bitmask;
      feature_ਨਾਂ[n++] = p->name;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  features |= CURL_ਸੰਸਕਰਣ_CURLDEBUG; /* for compatibility */
#ਜੇ_ਅੰਤ

  feature_ਨਾਂ[n] = ਨਿਰਾ;  /* Terminate array. */
  ਸੰਸਕਰਣ_info.features = features;

  ਵਾਪਸ &ਸੰਸਕਰਣ_info;
}
