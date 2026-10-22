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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "vtls/keylog.h"

#ଯଦି defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
ସ୍ଥିର FILE *keylog_file_fp;
/* Used for verbose logging */
ସ୍ଥିର ଅକ୍ଷର *keylog_file_name;

ଶୂନ୍ୟ Curl_tls_keylog_open(ଶୂନ୍ୟ)
{
  ଯଦି(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    ଯଦି(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      ଯଦି(keylog_file_fp) {
#ifdef _WIN32
        ଯଦି(setvbuf(keylog_file_fp, ଖାଲି, _IONBF, 0))
#ଅନ୍ୟଥା
        ଯଦି(setvbuf(keylog_file_fp, ଖାଲି, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = ଖାଲି;
        }
      }
    }
  }
}

ଶୂନ୍ୟ Curl_tls_keylog_close(ଶୂନ୍ୟ)
{
  ଯଦି(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = ଖାଲି;
  }
}

bool Curl_tls_keylog_enabled(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ !!keylog_file_fp;
}

ଅଚଳ ଅକ୍ଷର *Curl_tls_keylog_file_name(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ keylog_file_name;
}

bool Curl_tls_keylog_write_ଧାଡ଼ି(ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  ଅକ୍ଷର buf[256];

  ଯଦି(!keylog_file_fp || !ଧାଡ଼ି) {
    ଫେରନ୍ତୁ FALSE;
  }

  linelen = strlen(ଧାଡ଼ି);
  ଯଦି(linelen == 0 || linelen > ଆକାର(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    ଫେରନ୍ତୁ FALSE;
  }

  memcpy(buf, ଧାଡ଼ି, linelen);
  ଯଦି(ଧାଡ଼ି[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  ଫେରନ୍ତୁ TRUE;
}

bool Curl_tls_keylog_write(ଅଚଳ ଅକ୍ଷର *label,
                           ଅଚଳ unsigned ଅକ୍ଷର *client_random,
                           size_t random_size,
                           ଅଚଳ unsigned ଅକ୍ଷର *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned ଅକ୍ଷର ଧାଡ଼ି[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  ଯଦି(random_size < CLIENT_RANDOM_SIZE)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(!keylog_file_fp)
    ଫେରନ୍ତୁ FALSE;

  pos = strlen(label);
  ଯଦି(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    ଫେରନ୍ତୁ FALSE;
  }

  memcpy(ଧାଡ଼ି, label, pos);
  ଧାଡ଼ି[pos++] = ' ';

  /* Client Random */
  ପାଇଁ(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&ଧାଡ଼ି[pos], client_random[i]);
    pos += 2;
  }
  ଧାଡ଼ି[pos++] = ' ';

  /* Secret */
  ପାଇଁ(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&ଧାଡ଼ି[pos], secret[i]);
    pos += 2;
  }
  ଧାଡ଼ି[pos++] = '\n';
  ଧାଡ଼ି[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((ଅକ୍ଷର *)ଧାଡ଼ି, keylog_file_fp);
  ଫେରନ୍ତୁ TRUE;
}

#ଅନ୍ୟଥା /* TLS backend */

bool Curl_tls_keylog_enabled(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ FALSE;
}

ଅଚଳ ଅକ୍ଷର *Curl_tls_keylog_file_name(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ ଖାଲି;
}

#endif  /* TLS backend */
