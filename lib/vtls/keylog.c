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
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/keylog.h"

#ਜੇ defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
ਸਥਿਰ FILE *keylog_file_fp;
/* Used for verbose logging */
ਸਥਿਰ ਅੱਖਰ *keylog_file_name;

ਖਾਲੀ Curl_tls_keylog_open(ਖਾਲੀ)
{
  ਜੇ(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    ਜੇ(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      ਜੇ(keylog_file_fp) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
        ਜੇ(setvbuf(keylog_file_fp, ਨਿਰਾ, _IONBF, 0))
#ਨਹੀਂਤਾਂ
        ਜੇ(setvbuf(keylog_file_fp, ਨਿਰਾ, _IOLBF, 4096))
#ਜੇ_ਅੰਤ
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = ਨਿਰਾ;
        }
      }
    }
  }
}

ਖਾਲੀ Curl_tls_keylog_close(ਖਾਲੀ)
{
  ਜੇ(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = ਨਿਰਾ;
  }
}

bool Curl_tls_keylog_enabled(ਖਾਲੀ)
{
  ਵਾਪਸ !!keylog_file_fp;
}

ਅਚਲ ਅੱਖਰ *Curl_tls_keylog_file_name(ਖਾਲੀ)
{
  ਵਾਪਸ keylog_file_name;
}

bool Curl_tls_keylog_write_ਸਤਰ(ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  ਅੱਖਰ buf[256];

  ਜੇ(!keylog_file_fp || !ਸਤਰ) {
    ਵਾਪਸ FALSE;
  }

  linelen = ਲੜੀ_ਲੰਬਾਈ(ਸਤਰ);
  ਜੇ(linelen == 0 || linelen > ਆਕਾਰ(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    ਵਾਪਸ FALSE;
  }

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, ਸਤਰ, linelen);
  ਜੇ(ਸਤਰ[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  ਵਾਪਸ TRUE;
}

bool Curl_tls_keylog_write(ਅਚਲ ਅੱਖਰ *label,
                           ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *client_random,
                           size_t random_ਮਾਪ,
                           ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *secret, size_t secretlen)
{
  size_t pos, i;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਸਤਰ[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_ਮਾਪ) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_ਮਾਪ >= CLIENT_RANDOM_ਮਾਪ);
  ਜੇ(random_ਮਾਪ < CLIENT_RANDOM_ਮਾਪ)
    ਵਾਪਸ FALSE;
  ਜੇ(!keylog_file_fp)
    ਵਾਪਸ FALSE;

  pos = ਲੜੀ_ਲੰਬਾਈ(label);
  ਜੇ(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    ਵਾਪਸ FALSE;
  }

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਸਤਰ, label, pos);
  ਸਤਰ[pos++] = ' ';

  /* Client Random */
  ਲਈ(i = 0; i < CLIENT_RANDOM_ਮਾਪ; i++) {
    Curl_hexbyte(&ਸਤਰ[pos], client_random[i]);
    pos += 2;
  }
  ਸਤਰ[pos++] = ' ';

  /* Secret */
  ਲਈ(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&ਸਤਰ[pos], secret[i]);
    pos += 2;
  }
  ਸਤਰ[pos++] = '\n';
  ਸਤਰ[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((ਅੱਖਰ *)ਸਤਰ, keylog_file_fp);
  ਵਾਪਸ TRUE;
}

#ਨਹੀਂਤਾਂ /* TLS backend */

bool Curl_tls_keylog_enabled(ਖਾਲੀ)
{
  ਵਾਪਸ FALSE;
}

ਅਚਲ ਅੱਖਰ *Curl_tls_keylog_file_name(ਖਾਲੀ)
{
  ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ  /* TLS backend */
