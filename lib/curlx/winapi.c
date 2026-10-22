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

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ifdef _WIN32
#ଅନ୍ତର୍ଭାଵ "curlx/winapi.h"
#ଅନ୍ତର୍ଭାଵ "curlx/snprintf.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ଅଚଳ ଅକ୍ଷର *curlx_get_winapi_error(DWORD err, ଅକ୍ଷର *buf, size_t buflen)
{
  ଅକ୍ଷର *p;

  ଯଦି(!buflen)
    ଫେରନ୍ତୁ ଖାଲି;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  ଯଦି(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), ଖାଲି, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, ଖାଲି)) {
    *buf = '\0';
    ଫେରନ୍ତୁ ଖାଲି;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  ଯଦି(p) {
    ଯଦି(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    ଅନ୍ୟଥା
      *p = '\0';
  }

  ଫେରନ୍ତୁ *buf ? buf : ଖାଲି;
}

ଅଚଳ ଅକ୍ଷର *curlx_winapi_strerror(DWORD err, ଅକ୍ଷର *buf, size_t buflen)
{
  DWORD old_win_err = GetLastError();
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_errno = errno;

  ଯଦି(!buflen)
    ଫେରନ୍ତୁ ଖାଲି;

  *buf = '\0';

#ifdef CURLVERBOSE
  ଯଦି(!curlx_get_winapi_error(err, buf, buflen)) {
#ଯଦି defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#ଯଦି defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic କାଢ଼
#endif
  }
#ଅନ୍ୟଥା
  {
    ଅଚଳ ଅକ୍ଷର *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, strlen(txt));
  }
#endif

  ଯଦି(errno != old_errno)
    errno = old_errno;

  ଯଦି(old_win_err != GetLastError())
    SetLastError(old_win_err);

  ଫେରନ୍ତୁ buf;
}

#ifndef WITHOUT_LIBCURL

#ଅନ୍ତର୍ଭାଵ <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(unsigned ଅକ୍ଷର *entropy, size_t length)
{
  memset(entropy, 0, length);

  ଯଦି(BCryptGenRandom(ଖାଲି, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
