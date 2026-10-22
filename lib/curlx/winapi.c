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
#شامل "curl_setup.h"

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ifdef _WIN32
#شامل "curlx/winapi.h"
#شامل "curlx/snprintf.h"
#شامل "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ثابت نویسه *curlx_get_winapi_error(DWORD err, نویسه *buf, size_t buflen)
{
  نویسه *p;

  اگر(!buflen)
    بازگشت پوچ;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  اگر(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), پوچ, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, پوچ)) {
    *buf = '\0';
    بازگشت پوچ;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  اگر(p) {
    اگر(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    وگرنه
      *p = '\0';
  }

  بازگشت *buf ? buf : پوچ;
}

ثابت نویسه *curlx_winapi_strerror(DWORD err, نویسه *buf, size_t buflen)
{
  DWORD old_win_err = GetLastError();
  صحیح old_errno = errno;

  اگر(!buflen)
    بازگشت پوچ;

  *buf = '\0';

#ifdef CURLVERBOSE
  اگر(!curlx_get_winapi_error(err, buf, buflen)) {
#اگر defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#اگر defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic برداشت
#endif
  }
#وگرنه
  {
    ثابت نویسه *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, strlen(txt));
  }
#endif

  اگر(errno != old_errno)
    errno = old_errno;

  اگر(old_win_err != GetLastError())
    SetLastError(old_win_err);

  بازگشت buf;
}

#ifndef WITHOUT_LIBCURL

#شامل <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(unsigned نویسه *entropy, size_t length)
{
  memset(entropy, 0, length);

  اگر(BCryptGenRandom(پوچ, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    بازگشت CURLE_FAILED_INIT;

  بازگشت CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
