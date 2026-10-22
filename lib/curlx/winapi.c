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

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ifdef _WIN32
#অন্তর্ভাব "curlx/winapi.h"
#অন্তর্ভাব "curlx/snprintf.h"
#অন্তর্ভাব "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ধ্রুব বর্ণ *curlx_get_winapi_error(DWORD err, বর্ণ *buf, size_t buflen)
{
  বর্ণ *p;

  যদি(!buflen)
    প্রত্যাগম রিক্ত;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  যদি(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), রিক্ত, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, রিক্ত)) {
    *buf = '\0';
    প্রত্যাগম রিক্ত;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  যদি(p) {
    যদি(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    অন্যথা
      *p = '\0';
  }

  প্রত্যাগম *buf ? buf : রিক্ত;
}

ধ্রুব বর্ণ *curlx_winapi_strerror(DWORD err, বর্ণ *buf, size_t buflen)
{
  DWORD old_win_err = GetLastError();
  পূর্ণাঙ্ক old_errno = errno;

  যদি(!buflen)
    প্রত্যাগম রিক্ত;

  *buf = '\0';

#ifdef CURLVERBOSE
  যদি(!curlx_get_winapi_error(err, buf, buflen)) {
#যদি defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#যদি defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic বের_করো
#endif
  }
#অন্যথা
  {
    ধ্রুব বর্ণ *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, strlen(txt));
  }
#endif

  যদি(errno != old_errno)
    errno = old_errno;

  যদি(old_win_err != GetLastError())
    SetLastError(old_win_err);

  প্রত্যাগম buf;
}

#ifndef WITHOUT_LIBCURL

#অন্তর্ভাব <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(unsigned বর্ণ *entropy, size_t length)
{
  memset(entropy, 0, length);

  যদি(BCryptGenRandom(রিক্ত, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    প্রত্যাগম CURLE_FAILED_INIT;

  প্রত্যাগম CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
