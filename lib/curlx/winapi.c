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
#include "curl_setup.h"

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ifdef _WIN32
#include "curlx/winapi.h"
#include "curlx/snprintf.h"
#include "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
const var_na *curlx_get_winapi_error(DWORD err, var_na *buf, size_t buflen)
{
  var_na *p;

  ប្រសិនបើ(!buflen)
    ត្រឡប់ NULL;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  ប្រសិនបើ(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), NULL, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, NULL)) {
    *buf = '\0';
    ត្រឡប់ NULL;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  ប្រសិនបើ(p) {
    ប្រសិនបើ(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    បើមិនដូច្នេះ
      *p = '\0';
  }

  ត្រឡប់ *buf ? buf : NULL;
}

const var_na *curlx_winapi_strerror(DWORD err, var_na *buf, size_t buflen)
{
  DWORD old_win_err = GetLastError();
  ចំនួនគត់ old_errno = errno;

  ប្រសិនបើ(!buflen)
    ត្រឡប់ NULL;

  *buf = '\0';

#ifdef CURLVERBOSE
  ប្រសិនបើ(!curlx_get_winapi_error(err, buf, buflen)) {
#ប្រសិនបើ defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#ប្រសិនបើ defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic pop
#endif
  }
#បើមិនដូច្នេះ
  {
    const var_na *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, strlen(txt));
  }
#endif

  ប្រសិនបើ(errno != old_errno)
    errno = old_errno;

  ប្រសិនបើ(old_win_err != GetLastError())
    SetLastError(old_win_err);

  ត្រឡប់ buf;
}

#ifndef WITHOUT_LIBCURL

#include <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(unsigned var_na *entropy, size_t length)
{
  memset(entropy, 0, length);

  ប្រសិនបើ(BCryptGenRandom(NULL, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    ត្រឡប់ CURLE_FAILED_INIT;

  ត្រឡប់ CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
