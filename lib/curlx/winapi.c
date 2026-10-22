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

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ifdef _WIN32
#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"
#ಅನ್ತರ್ಭಾವ "curlx/snprintf.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ಧ್ರುವ ವರ್ಣ *curlx_get_winapi_error(DWORD err, ವರ್ಣ *buf, size_t buflen)
{
  ವರ್ಣ *p;

  ಯದಿ(!buflen)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  ಯದಿ(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), ರಿಕ್ತ, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, ರಿಕ್ತ)) {
    *buf = '\0';
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  ಯದಿ(p) {
    ಯದಿ(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    ಅನ್ಯಥಾ
      *p = '\0';
  }

  ಪ್ರತ್ಯಾಗಮ *buf ? buf : ರಿಕ್ತ;
}

ಧ್ರುವ ವರ್ಣ *curlx_winapi_strerror(DWORD err, ವರ್ಣ *buf, size_t buflen)
{
  DWORD old_win_err = GetLastError();
  ಪೂರ್ಣಾಙ್ಕ old_errno = errno;

  ಯದಿ(!buflen)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  *buf = '\0';

#ifdef CURLVERBOSE
  ಯದಿ(!curlx_get_winapi_error(err, buf, buflen)) {
#ಯದಿ defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#ಯದಿ defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic ತೆಗೆ
#endif
  }
#ಅನ್ಯಥಾ
  {
    ಧ್ರುವ ವರ್ಣ *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, strlen(txt));
  }
#endif

  ಯದಿ(errno != old_errno)
    errno = old_errno;

  ಯದಿ(old_win_err != GetLastError())
    SetLastError(old_win_err);

  ಪ್ರತ್ಯಾಗಮ buf;
}

#ifndef WITHOUT_LIBCURL

#ಅನ್ತರ್ಭಾವ <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(unsigned ವರ್ಣ *entropy, size_t length)
{
  memset(entropy, 0, length);

  ಯದಿ(BCryptGenRandom(ರಿಕ್ತ, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
