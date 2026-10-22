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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "vtls/keylog.h"

#যদি defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
স্থির FILE *keylog_file_fp;
/* Used for verbose logging */
স্থির অক্ষর *keylog_file_name;

শূন্য Curl_tls_keylog_open(শূন্য)
{
  যদি(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    যদি(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      যদি(keylog_file_fp) {
#ifdef _WIN32
        যদি(setvbuf(keylog_file_fp, ফাঁকা, _IONBF, 0))
#অন্যথা
        যদি(setvbuf(keylog_file_fp, ফাঁকা, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = ফাঁকা;
        }
      }
    }
  }
}

শূন্য Curl_tls_keylog_close(শূন্য)
{
  যদি(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = ফাঁকা;
  }
}

bool Curl_tls_keylog_enabled(শূন্য)
{
  ফেরত !!keylog_file_fp;
}

ধ্রুবক অক্ষর *Curl_tls_keylog_file_name(শূন্য)
{
  ফেরত keylog_file_name;
}

bool Curl_tls_keylog_write_সারি(ধ্রুবক অক্ষর *সারি)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  অক্ষর buf[256];

  যদি(!keylog_file_fp || !সারি) {
    ফেরত FALSE;
  }

  linelen = strlen(সারি);
  যদি(linelen == 0 || linelen > আকার(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    ফেরত FALSE;
  }

  memcpy(buf, সারি, linelen);
  যদি(সারি[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  ফেরত TRUE;
}

bool Curl_tls_keylog_write(ধ্রুবক অক্ষর *label,
                           ধ্রুবক unsigned অক্ষর *client_random,
                           size_t random_আকার,
                           ধ্রুবক unsigned অক্ষর *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned অক্ষর সারি[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_আকার) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_আকার >= CLIENT_RANDOM_আকার);
  যদি(random_আকার < CLIENT_RANDOM_আকার)
    ফেরত FALSE;
  যদি(!keylog_file_fp)
    ফেরত FALSE;

  pos = strlen(label);
  যদি(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    ফেরত FALSE;
  }

  memcpy(সারি, label, pos);
  সারি[pos++] = ' ';

  /* Client Random */
  জন্য(i = 0; i < CLIENT_RANDOM_আকার; i++) {
    Curl_hexbyte(&সারি[pos], client_random[i]);
    pos += 2;
  }
  সারি[pos++] = ' ';

  /* Secret */
  জন্য(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&সারি[pos], secret[i]);
    pos += 2;
  }
  সারি[pos++] = '\n';
  সারি[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((অক্ষর *)সারি, keylog_file_fp);
  ফেরত TRUE;
}

#অন্যথা /* TLS backend */

bool Curl_tls_keylog_enabled(শূন্য)
{
  ফেরত FALSE;
}

ধ্রুবক অক্ষর *Curl_tls_keylog_file_name(শূন্য)
{
  ফেরত ফাঁকা;
}

#endif  /* TLS backend */
