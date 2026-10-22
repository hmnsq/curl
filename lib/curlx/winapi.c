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

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ifdef _WIN32
#અન્તર્ભાવ "curlx/winapi.h"
#અન્તર્ભાવ "curlx/snprintf.h"
#અન્તર્ભાવ "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
અચલ અક્ષર *curlx_get_winapi_error(DWORD err, અક્ષર *buf, size_t buflen)
{
  અક્ષર *p;

  જો(!buflen)
    પાછા ખાલી;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  જો(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), ખાલી, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, ખાલી)) {
    *buf = '\0';
    પાછા ખાલી;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  જો(p) {
    જો(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    નહીંતર
      *p = '\0';
  }

  પાછા *buf ? buf : ખાલી;
}

અચલ અક્ષર *curlx_winapi_strerror(DWORD err, અક્ષર *buf, size_t buflen)
{
  DWORD old_win_err = GetLastError();
  પૂર્ણાંક old_errno = errno;

  જો(!buflen)
    પાછા ખાલી;

  *buf = '\0';

#ifdef CURLVERBOSE
  જો(!curlx_get_winapi_error(err, buf, buflen)) {
#જો defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#જો defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic કાઢો
#endif
  }
#નહીંતર
  {
    અચલ અક્ષર *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, strlen(txt));
  }
#endif

  જો(errno != old_errno)
    errno = old_errno;

  જો(old_win_err != GetLastError())
    SetLastError(old_win_err);

  પાછા buf;
}

#ifndef WITHOUT_LIBCURL

#અન્તર્ભાવ <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(unsigned અક્ષર *entropy, size_t length)
{
  memset(entropy, 0, length);

  જો(BCryptGenRandom(ખાલી, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    પાછા CURLE_FAILED_INIT;

  પાછા CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
