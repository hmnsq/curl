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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_NGHTTP2
#అన్తర్భావ <nghttp2/nghttp2.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "http2.h"
#అన్తర్భావ "vssh/ssh.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "easy_lock.h"

#నిర్వచితమైతే USE_ARES
#  include <ares.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_LIBIDN2
#అన్తర్భావ <idn2.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_LIBPSL
#అన్తర్భావ <libpsl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_LIBZ
#అన్తర్భావ <zlib.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_BROTLI
#నిర్వచితమైతే CURL_HAVE_DIAG
/* Ignore -Wvla warnings in brotli headers */
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wvla"
#అయితే_ముగింపు
#అన్తర్భావ <brotli/decode.h>
#నిర్వచితమైతే CURL_HAVE_DIAG
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ZSTD
#అన్తర్భావ <zstd.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_GSASL
#అన్తర్భావ <gsasl.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_LDAP
#అన్తర్భావ "curl_ldap.h"
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_BROTLI
స్థిర శూన్య brotli_సంస్కరణ(అక్షరం *buf, size_t bufsz)
{
  uint32_t brotli_సంస్కరణ = Brotli_Decoder_సంస్కరణ();
  గుర్తులేని పూర్ణాంకం major = brotli_సంస్కరణ >> 24;
  గుర్తులేని పూర్ణాంకం minor = (brotli_సంస్కరణ & 0x00FFFFFF) >> 12;
  గుర్తులేని పూర్ణాంకం patch = brotli_సంస్కరణ & 0x00000FFF;
  (శూన్య)curl_msnprintf(buf, bufsz, "brotli/%u.%u.%u", major, minor, patch);
}
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ZSTD
స్థిర శూన్య zstd_సంస్కరణ(అక్షరం *buf, size_t bufsz)
{
  గుర్తులేని పూర్ణాంకం సంస్కరణ_2 = ZSTD_సంస్కరణ_సంఖ్య();
  గుర్తులేని పూర్ణాంకం major = సంస్కరణ_2 / (100 * 100);
  గుర్తులేని పూర్ణాంకం minor = (సంస్కరణ_2 - (major * 100 * 100)) / 100;
  గుర్తులేని పూర్ణాంకం patch = సంస్కరణ_2 - (major * 100 * 100) - (minor * 100);
  (శూన్య)curl_msnprintf(buf, bufsz, "zstd/%u.%u.%u", major, minor, patch);
}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_LIBPSL
స్థిర శూన్య psl_సంస్కరణ(అక్షరం *buf, size_t bufsz)
{
#ఒకవేళ defined(PSL_సంస్కరణ_MAJOR) && (PSL_సంస్కరణ_MAJOR > 0 ||     \
                                   PSL_సంస్కరణ_MINOR >= 11)
  పూర్ణాంకం అంకె = psl_check_సంస్కరణ_సంఖ్య(0);
  curl_msnprintf(buf, bufsz, "libpsl/%d.%d.%d",
                 అంకె >> 16, (అంకె >> 8) & 0xff, అంకె & 0xff);
#లేకపోతే
  curl_msnprintf(buf, bufsz, "libpsl/%s", psl_get_సంస్కరణ());
#అయితే_ముగింపు
}
#అయితే_ముగింపు

#ఒకవేళ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
#నిర్వచించు USE_IDN
#అయితే_ముగింపు

#నిర్వచితమైతే USE_IDN
స్థిర శూన్య idn_సంస్కరణ(అక్షరం *buf, size_t bufsz)
{
#నిర్వచితమైతే USE_LIBIDN2
  curl_msnprintf(buf, bufsz, "libidn2/%s", idn2_check_సంస్కరణ(ఖాళీ));
#లేదావేళ defined(USE_WIN32_IDN)
  curl_msnprintf(buf, bufsz, "WinIDN");
#లేదావేళ defined(USE_APPLE_IDN)
  curl_msnprintf(buf, bufsz, "AppleIDN");
#అయితే_ముగింపు
}
#అయితే_ముగింపు

/*
 * curl_version() returns a pointer to a static buffer.
 *
 * It is implemented to work multi-threaded by making sure repeated invokes
 * generate the exact same string and never write any temporary data like
 * zeros in the data.
 */

#నిర్వచించు సంస్కరణ_PARTS 16 /* number of substrings we can concatenate */

అక్షరం *curl_సంస్కరణ(శూన్య)
{
  స్థిర అక్షరం బహిర్గతం[300];
  అక్షరం *outp;
  size_t outlen;
  స్థిరరాశి అక్షరం *src[సంస్కరణ_PARTS];
#నిర్వచితమైతే USE_SSL
  అక్షరం ssl_సంస్కరణ[200];
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_LIBZ
  అక్షరం z_సంస్కరణ[30];
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_BROTLI
  అక్షరం br_సంస్కరణ[30];
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ZSTD
  అక్షరం zstd_ver[30];
#అయితే_ముగింపు
#నిర్వచితమైతే USE_ARES
  అక్షరం cares_సంస్కరణ[30];
#అయితే_ముగింపు
#నిర్వచితమైతే USE_IDN
  అక్షరం idn_ver[30];
#అయితే_ముగింపు
#నిర్వచితమైతే USE_LIBPSL
  అక్షరం psl_ver[30];
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
  అక్షరం ssh_సంస్కరణ[30];
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  అక్షరం h2_సంస్కరణ[30];
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  అక్షరం h3_సంస్కరణ[30];
#అయితే_ముగింపు
#నిర్వచితమైతే USE_GSASL
  అక్షరం gsasl_buf[30];
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_GSSAPI
  అక్షరం gss_buf[40];
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_LDAP
  అక్షరం ldap_buf[30];
#అయితే_ముగింపు
  పూర్ణాంకం i = 0;
  పూర్ణాంకం j;

#నిర్వచితమైతే DEBUGBUILD
  /* Override version string when environment variable CURL_DEBUG_VERSION is
సమితి*/
  స్థిరరాశి అక్షరం *debugversion = getenv("CURL_DEBUG_VERSION");
  ఒకవేళ(debugversion) {
    curl_msnprintf(బహిర్గతం, పరిమాణం(బహిర్గతం), "%s", debugversion);
    తిరిగి బహిర్గతం;
  }
#అయితే_ముగింపు

  src[i++] = LIBCURL_NAME "/" LIBCURL_సంస్కరణ;
#నిర్వచితమైతే USE_SSL
  Curl_ssl_సంస్కరణ(ssl_సంస్కరణ, పరిమాణం(ssl_సంస్కరణ));
  src[i++] = ssl_సంస్కరణ;
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_LIBZ
  curl_msnprintf(z_సంస్కరణ, పరిమాణం(z_సంస్కరణ), "zlib/%s", zlib_సంస్కరణ());
  src[i++] = z_సంస్కరణ;
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_BROTLI
  brotli_సంస్కరణ(br_సంస్కరణ, పరిమాణం(br_సంస్కరణ));
  src[i++] = br_సంస్కరణ;
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ZSTD
  zstd_సంస్కరణ(zstd_ver, పరిమాణం(zstd_ver));
  src[i++] = zstd_ver;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_ARES
  curl_msnprintf(cares_సంస్కరణ, పరిమాణం(cares_సంస్కరణ),
                 "c-ares/%s", ares_సంస్కరణ(ఖాళీ));
  src[i++] = cares_సంస్కరణ;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_IDN
  idn_సంస్కరణ(idn_ver, పరిమాణం(idn_ver));
  src[i++] = idn_ver;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_LIBPSL
  psl_సంస్కరణ(psl_ver, పరిమాణం(psl_ver));
  src[i++] = psl_ver;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
  Curl_ssh_సంస్కరణ(ssh_సంస్కరణ, పరిమాణం(ssh_సంస్కరణ));
  src[i++] = ssh_సంస్కరణ;
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  Curl_http2_ver(h2_సంస్కరణ, పరిమాణం(h2_సంస్కరణ));
  src[i++] = h2_సంస్కరణ;
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  Curl_quic_ver(h3_సంస్కరణ, పరిమాణం(h3_సంస్కరణ));
  src[i++] = h3_సంస్కరణ;
#అయితే_ముగింపు
#నిర్వచితమైతే USE_GSASL
  curl_msnprintf(gsasl_buf, పరిమాణం(gsasl_buf), "libgsasl/%s",
                 gsasl_check_సంస్కరణ(ఖాళీ));
  src[i++] = gsasl_buf;
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_GSSAPI
#నిర్వచితమైతే HAVE_GSSAPPLE
  curl_msnprintf(gss_buf, పరిమాణం(gss_buf), "AppleGSS");
#లేదావేళ defined(HAVE_GSSGNU)
  curl_msnprintf(gss_buf, పరిమాణం(gss_buf), "libgss/%s", GSS_సంస్కరణ);
#లేదావేళ defined(CURL_KRB5_సంస్కరణ)
  curl_msnprintf(gss_buf, పరిమాణం(gss_buf), "mit-krb5/%s", CURL_KRB5_సంస్కరణ);
#లేకపోతే
  curl_msnprintf(gss_buf, పరిమాణం(gss_buf), "mit-krb5");
#అయితే_ముగింపు
  src[i++] = gss_buf;
#అయితే_ముగింపు /* HAVE_GSSAPI */
#నిర్వచితం_కాకపోతే CURL_DISABLE_LDAP
  Curl_ldap_సంస్కరణ(ldap_buf, పరిమాణం(ldap_buf));
  src[i++] = ldap_buf;
#అయితే_ముగింపు

  DEBUGASSERT(i <= సంస్కరణ_PARTS);

  outp = &బహిర్గతం[0];
  outlen = పరిమాణం(బహిర్గతం);
  కొరకు(j = 0; j < i; j++) {
    size_t n = అక్షరశ్రేణి_పొడవు(src[j]);
    /* we need room for a space, the string and the final zero */
    ఒకవేళ(outlen <= (n + 2))
      ఆపు;
    ఒకవేళ(j) {
      /* prepend a space if not the first */
      *outp++ = ' ';
      outlen--;
    }
    జ్ఞాపకం_నకలు(outp, src[j], n);
    outp += n;
    outlen -= n;
  }
  *outp = 0;

  తిరిగి బహిర్గతం;
}

/* data for curl_version_info

   Keep the list sorted alphabetically. It is also written so that each
   protocol line has its own #if line to make things easier on the eye.
 */

స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి supported_protocols[] = {
#నిర్వచితం_కాకపోతే CURL_DISABLE_DICT
  "dict",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FILE
  "file",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP
  "ftp",
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_FTP)
  "ftps",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_GOPHER
  "gopher",
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_GOPHER)
  "gophers",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  "http",
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_HTTP)
  "https",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_IMAP
  "imap",
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_IMAP)
  "imaps",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_LDAP
  "ldap",
#ఒకవేళ !defined(CURL_DISABLE_LDAPS) && \
  ((defined(USE_OPENLDAP) && defined(USE_SSL)) || \
   (!defined(USE_OPENLDAP) && defined(HAVE_LDAP_SSL)))
  "ldaps",
#అయితే_ముగింపు
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_MQTT
  "mqtt",
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_MQTT)
  "mqtts",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_POP3
  "pop3",
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_POP3)
  "pop3s",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_RTSP
  "rtsp",
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSH
  "scp",
  "sftp",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_SMTP
  "smtp",
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_SMTP)
  "smtps",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_TELNET
  "telnet",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_TFTP
  "tftp",
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
  /* WebSocket support relies on HTTP */
#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
  "ws",
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_WEBSOCKETS)
  "wss",
#అయితే_ముగింపు
#అయితే_ముగింపు

  ఖాళీ
};

/*
 * Feature presence runtime check functions.
 *
 * Warning: the value returned by these should not change between
 * curl_global_init() and curl_global_cleanup() calls.
 */

#ఒకవేళ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
స్థిర పూర్ణాంకం idn_present(curl_సంస్కరణ_info_దత్తాంశం *info)
{
#ఒకవేళ defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  (శూన్య)info;
  తిరిగి TRUE;
#లేకపోతే
  తిరిగి !!info->libidn;
#అయితే_ముగింపు
}
#అయితే_ముగింపు

#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
స్థిర పూర్ణాంకం https_proxy_present(curl_సంస్కరణ_info_దత్తాంశం *info)
{
  (శూన్య)info;
  తిరిగి Curl_ssl_supports(ఖాళీ, SSLSUPP_HTTPS_PROXY);
}
#అయితే_ముగింపు

#ఒకవేళ defined(USE_SSL) && defined(USE_ECH)
స్థిర పూర్ణాంకం ech_present(curl_సంస్కరణ_info_దత్తాంశం *info)
{
  (శూన్య)info;
  తిరిగి Curl_ssl_supports(ఖాళీ, SSLSUPP_ECH);
}
#అయితే_ముగింపు

/*
 * Features table.
 *
 * Keep the features alphabetically sorted.
 * Use FEATURE() macro to define an entry: this allows documentation check.
 */

#నిర్వచించు FEATURE(name, present, bitmask) { (name), (present), (bitmask) }

నిర్మాణం feat {
  స్థిరరాశి అక్షరం *name;
  పూర్ణాంకం        (*present)(curl_సంస్కరణ_info_దత్తాంశం *info);
  పూర్ణాంకం        bitmask;
};

స్థిర స్థిరరాశి నిర్మాణం feat features_పట్టిక[] = {
#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
  FEATURE("alt-svc",     ఖాళీ,                CURL_సంస్కరణ_ALTSVC),
#అయితే_ముగింపు
#ఒకవేళ defined(USE_ARES) && defined(USE_RESOLV_THREADED) && defined(USE_HTTPSRR)
  FEATURE("asyn-rr",     ఖాళీ,                0),
#అయితే_ముగింపు
#నిర్వచితమైతే CURLRES_ASYNCH
  FEATURE("AsynchDNS",   ఖాళీ,                CURL_సంస్కరణ_ASYNCHDNS),
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_BROTLI
  FEATURE("brotli",      ఖాళీ,                CURL_సంస్కరణ_BROTLI),
#అయితే_ముగింపు
#నిర్వచితమైతే DEBUGBUILD
  FEATURE("Debug",       ఖాళీ,                CURL_సంస్కరణ_DEBUG),
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && defined(USE_ECH)
  FEATURE("ECH",         ech_present,         0),

#నిర్వచితం_కాకపోతే USE_HTTPSRR
#దోషం "ECH enabled but not HTTPSRR, must be a config error"
#అయితే_ముగింపు
#అయితే_ముగింపు
#నిర్వచితమైతే USE_GSASL
  FEATURE("gsasl",       ఖాళీ,                CURL_సంస్కరణ_GSASL),
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_GSSAPI
  FEATURE("GSS-API",     ఖాళీ,                CURL_సంస్కరణ_GSSAPI),
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
  FEATURE("HSTS",        ఖాళీ,                CURL_సంస్కరణ_HSTS),
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_NGHTTP2)
  FEATURE("HTTP2",       ఖాళీ,                CURL_సంస్కరణ_HTTP2),
#అయితే_ముగింపు
#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  FEATURE("HTTP3",       ఖాళీ,                CURL_సంస్కరణ_HTTP3),
#అయితే_ముగింపు
#ఒకవేళ defined(USE_SSL) && !defined(CURL_DISABLE_PROXY) && \
  !defined(CURL_DISABLE_HTTP)
  FEATURE("HTTPS-proxy", https_proxy_present, CURL_సంస్కరణ_HTTPS_PROXY),
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTPSIG
  FEATURE("HTTPSIG",     ఖాళీ,                0),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_HTTPSRR
  FEATURE("HTTPSRR",     ఖాళీ,                0),
#అయితే_ముగింపు
#ఒకవేళ defined(USE_LIBIDN2) || defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN)
  FEATURE("IDN",         idn_present,         CURL_సంస్కరణ_IDN),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_IPV6
  FEATURE("IPv6",        ఖాళీ,                CURL_సంస్కరణ_IPV6),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_KERBEROS5
  FEATURE("Kerberos",    ఖాళీ,                CURL_సంస్కరణ_KERBEROS5),
#అయితే_ముగింపు
#ఒకవేళ (SIZEOF_CURL_OFF_T > 4) && ((SIZEOF_OFF_T > 4) || defined(_WIN32))
  FEATURE("Largefile",   ఖాళీ,                CURL_సంస్కరణ_LARGEFILE),
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_LIBZ
  FEATURE("libz",        ఖాళీ,                CURL_సంస్కరణ_LIBZ),
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_WITH_MULTI_SSL
  FEATURE("MultiSSL",    ఖాళీ,                CURL_సంస్కరణ_MULTI_SSL),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_NTLM
  FEATURE("NTLM",        ఖాళీ,                CURL_సంస్కరణ_NTLM),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_PROXY_HTTP3
  FEATURE("proxy-HTTP3", ఖాళీ,                0),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_LIBPSL
  FEATURE("PSL",         ఖాళీ,                CURL_సంస్కరణ_PSL),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSL
#నిర్వచితమైతే USE_APPLE_SECTRUST
  FEATURE("AppleSecTrust", ఖాళీ,              0),
#లేదావేళ defined(CURL_CA_NATIVE)
  FEATURE("NativeCA",    ఖాళీ,              0),
#అయితే_ముగింపు
#అయితే_ముగింపు /* USE_SSL */
#నిర్వచితమైతే USE_SPNEGO
  FEATURE("SPNEGO",      ఖాళీ,                CURL_సంస్కరణ_SPNEGO),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSL
  FEATURE("SSL",         ఖాళీ,                CURL_సంస్కరణ_SSL),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSLS_EXPORT
  FEATURE("SSLS-EXPORT", ఖాళీ,                0),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_WINDOWS_SSPI
  FEATURE("SSPI",        ఖాళీ,                CURL_సంస్కరణ_SSPI),
#అయితే_ముగింపు
#నిర్వచితమైతే GLOBAL_INIT_IS_THREADSAFE
  FEATURE("threadsafe",  ఖాళీ,                CURL_సంస్కరణ_THREADSAFE),
#అయితే_ముగింపు
#ఒకవేళ defined(_WIN32) && defined(UNICODE) && defined(_UNICODE)
  FEATURE("Unicode",     ఖాళీ,                CURL_సంస్కరణ_UNICODE),
#అయితే_ముగింపు
#నిర్వచితమైతే USE_UNIX_SOCKETS
  FEATURE("UnixSockets", ఖాళీ,                CURL_సంస్కరణ_UNIX_SOCKETS),
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ZSTD
  FEATURE("zstd",        ఖాళీ,                CURL_సంస్కరణ_ZSTD),
#అయితే_ముగింపు
  {ఖాళీ,                 ఖాళీ,                0}
};

స్థిర స్థిరరాశి అక్షరం *feature_పేర్లు[CURL_ARRAYSIZE(features_పట్టిక)] = { ఖాళీ };

స్థిర curl_సంస్కరణ_info_దత్తాంశం సంస్కరణ_info = {
  CURLVERSION_NOW,
  LIBCURL_సంస్కరణ,
  LIBCURL_సంస్కరణ_అంకె,
  CURL_OS, /* as found by configure or set by hand at build-time */
  0,    /* features bitmask is built at runtime */
  ఖాళీ, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  ఖాళీ, /* zlib_version */
  supported_protocols,
  ఖాళీ, /* c-ares version */
  0,    /* c-ares version numerical */
  ఖాళీ, /* libidn version */
  0,    /* iconv version */
  ఖాళీ, /* ssh lib version */
  0,    /* brotli_ver_num */
  ఖాళీ, /* brotli version */
  0,    /* nghttp2 version number */
  ఖాళీ, /* nghttp2 version string */
  ఖాళీ, /* quic library string */
#నిర్వచితమైతే CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#లేకపోతే
  ఖాళీ,
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_CA_మార్గం
  CURL_CA_మార్గం,  /* capath */
#లేకపోతే
  ఖాళీ,
#అయితే_ముగింపు
  0,    /* zstd_ver_num */
  ఖాళీ, /* zstd version */
  ఖాళీ, /* Hyper version */
  ఖాళీ, /* gsasl version */
  feature_పేర్లు,
  ఖాళీ  /* rtmp version */
};

curl_సంస్కరణ_info_దత్తాంశం *curl_సంస్కరణ_info(CURLversion stamp)
{
  size_t n;
  స్థిరరాశి నిర్మాణం feat *p;
  పూర్ణాంకం features = 0;

#నిర్వచితమైతే USE_SSH
  స్థిర అక్షరం ssh_buf[80];  /* 'ssh_buffer' clashes with libssh/libssh.h */
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SSL
#నిర్వచితమైతే CURL_WITH_MULTI_SSL
  స్థిర అక్షరం ssl_బఫరు[200];
#లేకపోతే
  స్థిర అక్షరం ssl_బఫరు[80];
#అయితే_ముగింపు
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_BROTLI
  స్థిర అక్షరం brotli_బఫరు[80];
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ZSTD
  స్థిర అక్షరం zstd_బఫరు[80];
#అయితే_ముగింపు

  (శూన్య)stamp;

#నిర్వచితమైతే USE_SSL
  Curl_ssl_సంస్కరణ(ssl_బఫరు, పరిమాణం(ssl_బఫరు));
  సంస్కరణ_info.ssl_సంస్కరణ = ssl_బఫరు;
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_LIBZ
  సంస్కరణ_info.libz_సంస్కరణ = zlib_సంస్కరణ();
  /* libz left NULL if non-existing */
#అయితే_ముగింపు
#నిర్వచితమైతే USE_ARES
  {
    పూర్ణాంకం aresnum;
    సంస్కరణ_info.ares = ares_సంస్కరణ(&aresnum);
    సంస్కరణ_info.ares_అంకె = aresnum;
  }
#అయితే_ముగింపు
#నిర్వచితమైతే USE_LIBIDN2
  /* This returns a version string if we use the given version or later,
     otherwise it returns NULL */
  సంస్కరణ_info.libidn = idn2_check_సంస్కరణ(IDN2_సంస్కరణ);
#అయితే_ముగింపు

#నిర్వచితమైతే USE_SSH
  Curl_ssh_సంస్కరణ(ssh_buf, పరిమాణం(ssh_buf));
  సంస్కరణ_info.libssh_సంస్కరణ = ssh_buf;
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_BROTLI
  సంస్కరణ_info.brotli_ver_అంకె = Brotli_Decoder_సంస్కరణ();
  brotli_సంస్కరణ(brotli_బఫరు, పరిమాణం(brotli_బఫరు));
  సంస్కరణ_info.brotli_సంస్కరణ = brotli_బఫరు;
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_ZSTD
  సంస్కరణ_info.zstd_ver_అంకె = ZSTD_సంస్కరణ_సంఖ్య();
  zstd_సంస్కరణ(zstd_బఫరు, పరిమాణం(zstd_బఫరు));
  సంస్కరణ_info.zstd_సంస్కరణ = zstd_బఫరు;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_NGHTTP2
  {
    nghttp2_info *h2 = nghttp2_సంస్కరణ(0);
    సంస్కరణ_info.nghttp2_ver_అంకె = (గుర్తులేని పూర్ణాంకం)h2->సంస్కరణ_అంకె;
    సంస్కరణ_info.nghttp2_సంస్కరణ = h2->సంస్కరణ_str;
  }
#అయితే_ముగింపు

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && defined(USE_HTTP3)
  {
    స్థిర అక్షరం quicbuffer[80];
    Curl_quic_ver(quicbuffer, పరిమాణం(quicbuffer));
    సంస్కరణ_info.quic_సంస్కరణ = quicbuffer;
  }
#అయితే_ముగింపు

#నిర్వచితమైతే USE_GSASL
  {
    సంస్కరణ_info.gsasl_సంస్కరణ = gsasl_check_సంస్కరణ(ఖాళీ);
  }
#అయితే_ముగింపు

  /* Get available features, build bitmask and names array. */
  n = 0;
  కొరకు(p = features_పట్టిక; p->name; p++)
    ఒకవేళ(!p->present || p->present(&సంస్కరణ_info)) {
      features |= p->bitmask;
      feature_పేర్లు[n++] = p->name;
    }

#నిర్వచితమైతే DEBUGBUILD
  features |= CURL_సంస్కరణ_CURLDEBUG; /* for compatibility */
#అయితే_ముగింపు

  feature_పేర్లు[n] = ఖాళీ;  /* Terminate array. */
  సంస్కరణ_info.features = features;

  తిరిగి &సంస్కరణ_info;
}
