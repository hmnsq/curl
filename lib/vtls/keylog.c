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
#అన్తర్భావ "vtls/keylog.h"

#ఒకవేళ defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#అన్తర్భావ "escape.h"
#అన్తర్భావ "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
స్థిర FILE *keylog_file_fp;
/* Used for verbose logging */
స్థిర అక్షరం *keylog_file_name;

శూన్య Curl_tls_keylog_open(శూన్య)
{
  ఒకవేళ(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    ఒకవేళ(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      ఒకవేళ(keylog_file_fp) {
#నిర్వచితమైతే _WIN32
        ఒకవేళ(setvbuf(keylog_file_fp, ఖాళీ, _IONBF, 0))
#లేకపోతే
        ఒకవేళ(setvbuf(keylog_file_fp, ఖాళీ, _IOLBF, 4096))
#అయితే_ముగింపు
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = ఖాళీ;
        }
      }
    }
  }
}

శూన్య Curl_tls_keylog_close(శూన్య)
{
  ఒకవేళ(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = ఖాళీ;
  }
}

bool Curl_tls_keylog_enabled(శూన్య)
{
  తిరిగి !!keylog_file_fp;
}

స్థిరరాశి అక్షరం *Curl_tls_keylog_file_name(శూన్య)
{
  తిరిగి keylog_file_name;
}

bool Curl_tls_keylog_write_పంక్తి(స్థిరరాశి అక్షరం *పంక్తి)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  అక్షరం buf[256];

  ఒకవేళ(!keylog_file_fp || !పంక్తి) {
    తిరిగి FALSE;
  }

  linelen = అక్షరశ్రేణి_పొడవు(పంక్తి);
  ఒకవేళ(linelen == 0 || linelen > పరిమాణం(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    తిరిగి FALSE;
  }

  జ్ఞాపకం_నకలు(buf, పంక్తి, linelen);
  ఒకవేళ(పంక్తి[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  తిరిగి TRUE;
}

bool Curl_tls_keylog_write(స్థిరరాశి అక్షరం *label,
                           స్థిరరాశి గుర్తులేని అక్షరం *client_random,
                           size_t random_కొలత,
                           స్థిరరాశి గుర్తులేని అక్షరం *secret, size_t secretlen)
{
  size_t pos, i;
  గుర్తులేని అక్షరం పంక్తి[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_కొలత) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_కొలత >= CLIENT_RANDOM_కొలత);
  ఒకవేళ(random_కొలత < CLIENT_RANDOM_కొలత)
    తిరిగి FALSE;
  ఒకవేళ(!keylog_file_fp)
    తిరిగి FALSE;

  pos = అక్షరశ్రేణి_పొడవు(label);
  ఒకవేళ(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    తిరిగి FALSE;
  }

  జ్ఞాపకం_నకలు(పంక్తి, label, pos);
  పంక్తి[pos++] = ' ';

  /* Client Random */
  కొరకు(i = 0; i < CLIENT_RANDOM_కొలత; i++) {
    Curl_hexbyte(&పంక్తి[pos], client_random[i]);
    pos += 2;
  }
  పంక్తి[pos++] = ' ';

  /* Secret */
  కొరకు(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&పంక్తి[pos], secret[i]);
    pos += 2;
  }
  పంక్తి[pos++] = '\n';
  పంక్తి[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((అక్షరం *)పంక్తి, keylog_file_fp);
  తిరిగి TRUE;
}

#లేకపోతే /* TLS backend */

bool Curl_tls_keylog_enabled(శూన్య)
{
  తిరిగి FALSE;
}

స్థిరరాశి అక్షరం *Curl_tls_keylog_file_name(శూన్య)
{
  తిరిగి ఖాళీ;
}

#అయితే_ముగింపు  /* TLS backend */
