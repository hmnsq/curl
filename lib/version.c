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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2
#ಅನ್ತರ್ಭಾವ <nghttp2/nghttp2.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "http2.h"
#ಅನ್ತರ್ಭಾವ "vssh/ssh.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "easy_lock.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
#  include <ares.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBIDN2
#ಅನ್ತರ್ಭಾವ <idn2.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
#ಅನ್ತರ್ಭಾವ <libpsl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
#ಅನ್ತರ್ಭಾವ <zlib.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wvla"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <brotli/decode.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_HAVE_DIAG
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
#ಅನ್ತರ್ಭಾವ <zstd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
#ಅನ್ತರ್ಭಾವ <gsasl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_LDAP
#ಅನ್ತರ್ಭಾವ "curl_ldap.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
ಸ್ಥಿರ ಶೂನ್ಯ brotli_ಆವೃತ್ತಿ(ಅಕ್ಷರ *buf, size_t bufsz)
{
  uint32_t brotli_ಆವೃತ್ತಿ = Brotli_Decoder_ಆವೃತ್ತಿ();
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ major = brotli_ಆವೃತ್ತಿ >> 24;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ minor = (brotli_ಆವೃತ್ತಿ & 0x00FFFFFF) >> 12;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ patch = brotli_ಆವೃತ್ತಿ & 0x00000FFF;
  (ಶೂನ್ಯ)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
ಸ್ಥಿರ ಶೂನ್ಯ zstd_ಆವೃತ್ತಿ(ಅಕ್ಷರ *buf, size_t bufsz)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ_2 = ZSTD_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ();
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ major = ಆವೃತ್ತಿ_2 / (100 * 100);
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ minor = (ಆವೃತ್ತಿ_2 - (major * 100 * 100)) / 100;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ patch = ಆವೃತ್ತಿ_2 - (major * 100 * 100) - (minor * 100);
  (ಶೂನ್ಯ)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
ಸ್ಥಿರ ಶೂನ್ಯ psl_ಆವೃತ್ತಿ(ಅಕ್ಷರ *buf, size_t bufsz)
{
#ಆದರೆ defined(PSL_ಆವೃತ್ತಿ_MAJOR) && (PSL_ಆವೃತ್ತಿ_MAJOR > 0 ||     \
                                   PSL_ಆವೃತ್ತಿ_MINOR >= 11)
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ = psl_check_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 ಅಂಕೆ >> 16, (ಅಂಕೆ >> 8) & 0xff, ಅಂಕೆ & 0xff);
#ಇಲ್ಲದಿದ್ದರೆ
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_ಆವೃತ್ತಿ());
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#ವ್ಯಾಖ್ಯಾನಿಸು USE_IDN
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IDN
ಸ್ಥಿರ ಶೂನ್ಯ idn_ಆವೃತ್ತಿ(ಅಕ್ಷರ *buf, size_t bufsz)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_ಆವೃತ್ತಿ(ಖಾಲಿ));
#ಅಥವಾಆದರೆ defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#ಅಥವಾಆದರೆ defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು ಆವೃತ್ತಿ_PARTS 16 /* number of substrings we can concatenate */

ಅಕ್ಷರ *curl_ಆವೃತ್ತಿ(ಶೂನ್ಯ)
{
  ಸ್ಥಿರ ಅಕ್ಷರ ಹೊರಹೋಗು[300];
  ಅಕ್ಷರ *outp;
  size_t outlen;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src[ಆವೃತ್ತಿ_PARTS];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  ಅಕ್ಷರ ssl_ಆವೃತ್ತಿ[200];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
  ಅಕ್ಷರ z_ಆವೃತ್ತಿ[30];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
  ಅಕ್ಷರ br_ಆವೃತ್ತಿ[30];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
  ಅಕ್ಷರ zstd_ver[30];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
  ಅಕ್ಷರ cares_ಆವೃತ್ತಿ[30];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IDN
  ಅಕ್ಷರ idn_ver[30];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
  ಅಕ್ಷರ psl_ver[30];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  ಅಕ್ಷರ ssh_ಆವೃತ್ತಿ[30];
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  ಅಕ್ಷರ h2_ಆವೃತ್ತಿ[30];
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  ಅಕ್ಷರ h3_ಆವೃತ್ತಿ[30];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
  ಅಕ್ಷರ gsasl_buf[30];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSSAPI
  ಅಕ್ಷರ gss_buf[40];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_LDAP
  ಅಕ್ಷರ ldap_buf[30];
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ i = 0;
  ಪೂರ್ಣಾಂಕ j;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
ಗಣ*/
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *debugversion = getenv("CURL_DEBUG_VERSION");
  ಆದರೆ(debugversion) {
    curl_msnprintf(ಹೊರಹೋಗು, ಗಾತ್ರ(ಹೊರಹೋಗು), "%s", debugversion);
    ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ

  src[i++] = LIBCURL_NAME "/" LIBCURL_ಆವೃತ್ತಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  Curl_ssl_ಆವೃತ್ತಿ(ssl_ಆವೃತ್ತಿ, ಗಾತ್ರ(ssl_ಆವೃತ್ತಿ));
  src[i++] = ssl_ಆವೃತ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
  curl_msnprintf(z_ಆವೃತ್ತಿ, ಗಾತ್ರ(z_ಆವೃತ್ತಿ), "zlib/%s", zlib_ಆವೃತ್ತಿ());
  src[i++] = z_ಆವೃತ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
  brotli_ಆವೃತ್ತಿ(br_ಆವೃತ್ತಿ, ಗಾತ್ರ(br_ಆವೃತ್ತಿ));
  src[i++] = br_ಆವೃತ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
  zstd_ಆವೃತ್ತಿ(zstd_ver, ಗಾತ್ರ(zstd_ver));
  src[i++] = zstd_ver;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
  curl_msnprintf(cares_ಆವೃತ್ತಿ, ಗಾತ್ರ(cares_ಆವೃತ್ತಿ),
                 "c-ares/%s", ares_ಆವೃತ್ತಿ(ಖಾಲಿ));
  src[i++] = cares_ಆವೃತ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IDN
  idn_ಆವೃತ್ತಿ(idn_ver, ಗಾತ್ರ(idn_ver));
  src[i++] = idn_ver;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
  psl_ಆವೃತ್ತಿ(psl_ver, ಗಾತ್ರ(psl_ver));
  src[i++] = psl_ver;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  Curl_ssh_ಆವೃತ್ತಿ(ssh_ಆವೃತ್ತಿ, ಗಾತ್ರ(ssh_ಆವೃತ್ತಿ));
  src[i++] = ssh_ಆವೃತ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_ಆವೃತ್ತಿ, ಗಾತ್ರ(h2_ಆವೃತ್ತಿ));
  src[i++] = h2_ಆವೃತ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_ಆವೃತ್ತಿ, ಗಾತ್ರ(h3_ಆವೃತ್ತಿ));
  src[i++] = h3_ಆವೃತ್ತಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
  curl_msnprintf(gsasl_buf, ಗಾತ್ರ(gsasl_buf), "libgsasl/%s",
                 gsasl_check_ಆವೃತ್ತಿ(ಖಾಲಿ));
  src[i++] = gsasl_buf;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSSAPI
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, ಗಾತ್ರ(gss_buf), "AppleGSS");
#ಅಥವಾಆದರೆ defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, ಗಾತ್ರ(gss_buf), "libgss/%s", GSS_ಆವೃತ್ತಿ);
#ಅಥವಾಆದರೆ defined(CURL_KRB5_ಆವೃತ್ತಿ)
  curl_msnprintf(gss_buf, ಗಾತ್ರ(gss_buf), "mit-krb5/%s", CURL_KRB5_ಆವೃತ್ತಿ);
#ಇಲ್ಲದಿದ್ದರೆ
  curl_msnprintf(gss_buf, ಗಾತ್ರ(gss_buf), "mit-krb5");
#ಆದರೆ_ಅಂತ್ಯ
  src[i++] = gss_buf;
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_GSSAPI */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_LDAP
  Curl_ldap_ಆವೃತ್ತಿ(ldap_buf, ಗಾತ್ರ(ldap_buf));
  src[i++] = ldap_buf;
#ಆದರೆ_ಅಂತ್ಯ

  DEBUGASSERT(i <= ಆವೃತ್ತಿ_PARTS);

  outp = &ಹೊರಹೋಗು[0];
  outlen = ಗಾತ್ರ(ಹೊರಹೋಗು);
  ಗಾಗಿ(j = 0; j < i; j++) {
    size_t n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(src[j]);
    /* we need room for a space, the string and the final zero */
    ಆದರೆ(outlen <= (n + 2))
      ನಿಲ್ಲಿಸು;
    ಆದರೆ(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    ಸ್ಮರಣೆ_ನಕಲು(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ supported_protocols[] = {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DICT
  "dict",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FILE
  "file",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP
  "ftp",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_GOPHER
  "gopher",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  "http",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_IMAP
  "imap",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_LDAP
  "ldap",
#ಆದರೆ !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_MQTT
  "mqtt",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_POP3
  "pop3",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_RTSP
  "rtsp",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  "scp",
  "sftp",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_SMTP
  "smtp",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TELNET
  "telnet",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TFTP
  "tftp",
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
  "ws",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

  ಖಾಲಿ
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#ಆದರೆ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ idn_present(curl_ಆವೃತ್ತಿ_info_ದತ್ತಾಂಶ *info)
{
#ಆದರೆ defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (ಶೂನ್ಯ)info;
  ಹಿಂತಿರುಗಿ TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ !!info->libidn;
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ https_proxy_present(curl_ಆವೃತ್ತಿ_info_ದತ್ತಾಂಶ *info)
{
  (ಶೂನ್ಯ)info;
  ಹಿಂತಿರುಗಿ Curl_ssl_supports(ಖಾಲಿ, SSLSUPP_HTTPS_PROXY);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_SSL) && defined(USE_ECH)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ech_present(curl_ಆವೃತ್ತಿ_info_ದತ್ತಾಂಶ *info)
{
  (ಶೂನ್ಯ)info;
  ಹಿಂತಿರುಗಿ Curl_ssl_supports(ಖಾಲಿ, SSLSUPP_ECH);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

ರಚನೆ feat {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
  ಪೂರ್ಣಾಂಕ        (*present)(curl_ಆವೃತ್ತಿ_info_ದತ್ತಾಂಶ *info);
  ಪೂರ್ಣಾಂಕ        bitmask;
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ feat features_ಕೋಷ್ಟಕ[] = {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_ALTSVC),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     ಖಾಲಿ,                0),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLRES_ASYNCH
  FEATURE("AsynchDNS",   ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_ASYNCHDNS),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
  FEATURE("brotli",      ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_BROTLI),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  FEATURE("Debug",       ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_DEBUG),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_HTTPSRR
#ದೋಷ "ECH enabled but not HTTPSRR, must be a config error"
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
  FEATURE("gsasl",       ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_GSASL),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GSSAPI
  FEATURE("GSS-API",     ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_GSSAPI),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
  FEATURE("HSTS",        ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_HSTS),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_HTTP2),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_HTTP3),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_ಆವೃತ್ತಿ_HTTPS_PROXY),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     ಖಾಲಿ,                0),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTPSRR
  FEATURE("HTTPSRR",     ಖಾಲಿ,                0),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_ಆವೃತ್ತಿ_IDN),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  FEATURE("IPv6",        ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_IPV6),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_KERBEROS5
  FEATURE("Kerberos",    ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_KERBEROS5),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_LARGEFILE),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
  FEATURE("libz",        ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_LIBZ),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_MULTI_SSL),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
  FEATURE("NTLM",        ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_NTLM),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", ಖಾಲಿ,                0),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBPSL
  FEATURE("PSL",         ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_PSL),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", ಖಾಲಿ,              0),
#ಅಥವಾಆದರೆ defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    ಖಾಲಿ,              0),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
  FEATURE("SPNEGO",      ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_SPNEGO),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  FEATURE("SSL",         ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_SSL),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", ಖಾಲಿ,                0),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  FEATURE("SSPI",        ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_SSPI),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_THREADSAFE),
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_UNICODE),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_UNIX_SOCKETS
  FEATURE("UnixSockets", ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_UNIX_SOCKETS),
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
  FEATURE("zstd",        ಖಾಲಿ,                CURL_ಆವೃತ್ತಿ_ZSTD),
#ಆದರೆ_ಅಂತ್ಯ
  {ಖಾಲಿ,                 ಖಾಲಿ,                0}
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *feature_ಹೆಸರುಗಳು[CURL_ARRAYSIZE(features_ಕೋಷ್ಟಕ)] = { ಖಾಲಿ };

ಸ್ಥಿರ curl_ಆವೃತ್ತಿ_info_ದತ್ತಾಂಶ ಆವೃತ್ತಿ_info = {
  CURLVERSION_NOW,
  LIBCURL_ಆವೃತ್ತಿ,
  LIBCURL_ಆವೃತ್ತಿ_ಅಂಕೆ,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  ಖಾಲಿ, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  ಖಾಲಿ, /* zlib_version */
  supported_protocols,
  ಖಾಲಿ, /* c-ares version */
  0,    /* c-ares version numerical */
  ಖಾಲಿ, /* libidn version */
  0,    /* iconv version */
  ಖಾಲಿ, /* ssh lib version */
  0,    /* brotli_ver_num */
  ಖಾಲಿ, /* brotli version */
  0,    /* nghttp2 version number */
  ಖಾಲಿ, /* nghttp2 version string */
  ಖಾಲಿ, /* quic library string */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#ಇಲ್ಲದಿದ್ದರೆ
  ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CA_ಮಾರ್ಗ
  CURL_CA_ಮಾರ್ಗ,  /* capath */
#ಇಲ್ಲದಿದ್ದರೆ
  ಖಾಲಿ,
#ಆದರೆ_ಅಂತ್ಯ
  0,    /* zstd_ver_num */
  ಖಾಲಿ, /* zstd version */
  ಖಾಲಿ, /* Hyper version */
  ಖಾಲಿ, /* gsasl version */
  feature_ಹೆಸರುಗಳು,
  ಖಾಲಿ  /* rtmp version */
};

curl_ಆವೃತ್ತಿ_info_ದತ್ತಾಂಶ *curl_ಆವೃತ್ತಿ_info(CURLversion stamp)
{
  size_t n;
  ಸ್ಥಿರಾಂಕ ರಚನೆ feat *p;
  ಪೂರ್ಣಾಂಕ features = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  ಸ್ಥಿರ ಅಕ್ಷರ ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_WITH_MULTI_SSL
  ಸ್ಥಿರ ಅಕ್ಷರ ssl_ಬಫರ್[200];
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರ ಅಕ್ಷರ ssl_ಬಫರ್[80];
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
  ಸ್ಥಿರ ಅಕ್ಷರ brotli_ಬಫರ್[80];
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
  ಸ್ಥಿರ ಅಕ್ಷರ zstd_ಬಫರ್[80];
#ಆದರೆ_ಅಂತ್ಯ

  (ಶೂನ್ಯ)stamp;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  Curl_ssl_ಆವೃತ್ತಿ(ssl_ಬಫರ್, ಗಾತ್ರ(ssl_ಬಫರ್));
  ಆವೃತ್ತಿ_info.ssl_ಆವೃತ್ತಿ = ssl_ಬಫರ್;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LIBZ
  ಆವೃತ್ತಿ_info.libz_ಆವೃತ್ತಿ = zlib_ಆವೃತ್ತಿ();
  /* libz left NULL if non-existing */
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ARES
  {
    ಪೂರ್ಣಾಂಕ aresnum;
    ಆವೃತ್ತಿ_info.ares = ares_ಆವೃತ್ತಿ(&aresnum);
    ಆವೃತ್ತಿ_info.ares_ಅಂಕೆ = aresnum;
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  ಆವೃತ್ತಿ_info.libidn = idn2_check_ಆವೃತ್ತಿ(IDN2_ಆವೃತ್ತಿ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH
  Curl_ssh_ಆವೃತ್ತಿ(ssh_buf, ಗಾತ್ರ(ssh_buf));
  ಆವೃತ್ತಿ_info.libssh_ಆವೃತ್ತಿ = ssh_buf;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BROTLI
  ಆವೃತ್ತಿ_info.brotli_ver_ಅಂಕೆ = Brotli_Decoder_ಆವೃತ್ತಿ();
  brotli_ಆವೃತ್ತಿ(brotli_ಬಫರ್, ಗಾತ್ರ(brotli_ಬಫರ್));
  ಆವೃತ್ತಿ_info.brotli_ಆವೃತ್ತಿ = brotli_ಬಫರ್;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ZSTD
  ಆವೃತ್ತಿ_info.zstd_ver_ಅಂಕೆ = ZSTD_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ();
  zstd_ಆವೃತ್ತಿ(zstd_ಬಫರ್, ಗಾತ್ರ(zstd_ಬಫರ್));
  ಆವೃತ್ತಿ_info.zstd_ಆವೃತ್ತಿ = zstd_ಬಫರ್;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_ಆವೃತ್ತಿ(0);
    ಆವೃತ್ತಿ_info.nghttp2_ver_ಅಂಕೆ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)h2->ಆವೃತ್ತಿ_ಅಂಕೆ;
    ಆವೃತ್ತಿ_info.nghttp2_ಆವೃತ್ತಿ = h2->ಆವೃತ್ತಿ_str;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    ಸ್ಥಿರ ಅಕ್ಷರ quicbuffer[80];
    Curl_quic_ver(quicbuffer, ಗಾತ್ರ(quicbuffer));
    ಆವೃತ್ತಿ_info.quic_ಆವೃತ್ತಿ = quicbuffer;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
  {
    ಆವೃತ್ತಿ_info.gsasl_ಆವೃತ್ತಿ = gsasl_check_ಆವೃತ್ತಿ(ಖಾಲಿ);
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* Get available features, build bitmask and names array. */
  n = 0;
  ಗಾಗಿ(p = features_ಕೋಷ್ಟಕ; p->name; p++)
    ಆದರೆ(!p->present || p->present(&ಆವೃತ್ತಿ_info)) {
      features |= p->bitmask;
      feature_ಹೆಸರುಗಳು[n++] = p->name;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  features |= CURL_ಆವೃತ್ತಿ_CURLDEBUG; /* for compatibility */
#ಆದರೆ_ಅಂತ್ಯ

  feature_ಹೆಸರುಗಳು[n] = ಖಾಲಿ;  /* Terminate array. */
  ಆವೃತ್ತಿ_info.features = features;

  ಹಿಂತಿರುಗಿ &ಆವೃತ್ತಿ_info;
}
