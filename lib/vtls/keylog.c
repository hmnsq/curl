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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "vtls/keylog.h"

#જો defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
સ્થિર FILE *keylog_file_fp;
/* Used for verbose logging */
સ્થિર અક્ષર *keylog_file_name;

શૂન્ય Curl_tls_keylog_open(શૂન્ય)
{
  જો(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    જો(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      જો(keylog_file_fp) {
#ifdef _WIN32
        જો(setvbuf(keylog_file_fp, ખાલી, _IONBF, 0))
#નહીંતર
        જો(setvbuf(keylog_file_fp, ખાલી, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = ખાલી;
        }
      }
    }
  }
}

શૂન્ય Curl_tls_keylog_close(શૂન્ય)
{
  જો(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = ખાલી;
  }
}

bool Curl_tls_keylog_enabled(શૂન્ય)
{
  પાછા !!keylog_file_fp;
}

અચલ અક્ષર *Curl_tls_keylog_file_name(શૂન્ય)
{
  પાછા keylog_file_name;
}

bool Curl_tls_keylog_write_લીટી(અચલ અક્ષર *લીટી)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  અક્ષર buf[256];

  જો(!keylog_file_fp || !લીટી) {
    પાછા FALSE;
  }

  linelen = strlen(લીટી);
  જો(linelen == 0 || linelen > કદ(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    પાછા FALSE;
  }

  memcpy(buf, લીટી, linelen);
  જો(લીટી[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  પાછા TRUE;
}

bool Curl_tls_keylog_write(અચલ અક્ષર *label,
                           અચલ unsigned અક્ષર *client_random,
                           size_t random_size,
                           અચલ unsigned અક્ષર *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned અક્ષર લીટી[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  જો(random_size < CLIENT_RANDOM_SIZE)
    પાછા FALSE;
  જો(!keylog_file_fp)
    પાછા FALSE;

  pos = strlen(label);
  જો(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    પાછા FALSE;
  }

  memcpy(લીટી, label, pos);
  લીટી[pos++] = ' ';

  /* Client Random */
  માટે(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&લીટી[pos], client_random[i]);
    pos += 2;
  }
  લીટી[pos++] = ' ';

  /* Secret */
  માટે(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&લીટી[pos], secret[i]);
    pos += 2;
  }
  લીટી[pos++] = '\n';
  લીટી[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((અક્ષર *)લીટી, keylog_file_fp);
  પાછા TRUE;
}

#નહીંતર /* TLS backend */

bool Curl_tls_keylog_enabled(શૂન્ય)
{
  પાછા FALSE;
}

અચલ અક્ષર *Curl_tls_keylog_file_name(શૂન્ય)
{
  પાછા ખાલી;
}

#endif  /* TLS backend */
