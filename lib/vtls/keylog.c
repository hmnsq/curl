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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "vtls/keylog.h"

#যদি defined(USE_OPENSSL) || defined(USE_GNUTLS) || defined(USE_WOLFSSL) || \
  defined(USE_RUSTLS)

#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "curlx/win32-fopen.h"

/* The fp for the open SSLKEYLOGFILE, or NULL if not open */
স্থির FILE *keylog_file_fp;
/* Used for verbose logging */
স্থির ৱর্ণ *keylog_file_name;

শূন্য Curl_tls_keylog_open(শূন্য)
{
  যদি(!keylog_file_fp) {
    keylog_file_name = getenv("SSLKEYLOGFILE");
    যদি(keylog_file_name) {
      keylog_file_fp = curlx_fopen(keylog_file_name, FOPEN_APPENDTEXT);
      যদি(keylog_file_fp) {
#ifdef _WIN32
        যদি(setvbuf(keylog_file_fp, রিক্ত, _IONBF, 0))
#অন্যথা
        যদি(setvbuf(keylog_file_fp, রিক্ত, _IOLBF, 4096))
#endif
        {
          curlx_fclose(keylog_file_fp);
          keylog_file_fp = রিক্ত;
        }
      }
    }
  }
}

শূন্য Curl_tls_keylog_close(শূন্য)
{
  যদি(keylog_file_fp) {
    curlx_fclose(keylog_file_fp);
    keylog_file_fp = রিক্ত;
  }
}

bool Curl_tls_keylog_enabled(শূন্য)
{
  ঘূৰাই !!keylog_file_fp;
}

ধ্রুৱ ৱর্ণ *Curl_tls_keylog_file_name(শূন্য)
{
  ঘূৰাই keylog_file_name;
}

bool Curl_tls_keylog_write_সাৰি(ধ্রুৱ ৱর্ণ *সাৰি)
{
  /* The current maximum valid keylog line length LF and NUL is 195. */
  size_t linelen;
  ৱর্ণ buf[256];

  যদি(!keylog_file_fp || !সাৰি) {
    ঘূৰাই FALSE;
  }

  linelen = strlen(সাৰি);
  যদি(linelen == 0 || linelen > পরিমাণ(buf) - 2) {
    /* Empty line or too big to fit in an LF and NUL. */
    ঘূৰাই FALSE;
  }

  memcpy(buf, সাৰি, linelen);
  যদি(সাৰি[linelen - 1] != '\n') {
    buf[linelen++] = '\n';
  }
  buf[linelen] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs(buf, keylog_file_fp);
  ঘূৰাই TRUE;
}

bool Curl_tls_keylog_write(ধ্রুৱ ৱর্ণ *label,
                           ধ্রুৱ unsigned ৱর্ণ *client_random,
                           size_t random_size,
                           ধ্রুৱ unsigned ৱর্ণ *secret, size_t secretlen)
{
  size_t pos, i;
  unsigned ৱর্ণ সাৰি[KEYLOG_LABEL_MAXLEN + 1 +
                     (2 * CLIENT_RANDOM_SIZE) + 1 +
                     (2 * SECRET_MAXLEN) + 1 + 1];
  DEBUGASSERT(random_size >= CLIENT_RANDOM_SIZE);
  যদি(random_size < CLIENT_RANDOM_SIZE)
    ঘূৰাই FALSE;
  যদি(!keylog_file_fp)
    ঘূৰাই FALSE;

  pos = strlen(label);
  যদি(pos > KEYLOG_LABEL_MAXLEN || !secretlen || secretlen > SECRET_MAXLEN) {
    /* Should never happen - sanity check anyway. */
    ঘূৰাই FALSE;
  }

  memcpy(সাৰি, label, pos);
  সাৰি[pos++] = ' ';

  /* Client Random */
  বাবে(i = 0; i < CLIENT_RANDOM_SIZE; i++) {
    Curl_hexbyte(&সাৰি[pos], client_random[i]);
    pos += 2;
  }
  সাৰি[pos++] = ' ';

  /* Secret */
  বাবে(i = 0; i < secretlen; i++) {
    Curl_hexbyte(&সাৰি[pos], secret[i]);
    pos += 2;
  }
  সাৰি[pos++] = '\n';
  সাৰি[pos] = '\0';

  /* Using fputs here instead of fprintf since libcurl's fprintf replacement
     may not be thread-safe. */
  fputs((ৱর্ণ *)সাৰি, keylog_file_fp);
  ঘূৰাই TRUE;
}

#অন্যথা /* TLS backend */

bool Curl_tls_keylog_enabled(শূন্য)
{
  ঘূৰাই FALSE;
}

ধ্রুৱ ৱর্ণ *Curl_tls_keylog_file_name(শূন্য)
{
  ঘূৰাই রিক্ত;
}

#endif  /* TLS backend */
