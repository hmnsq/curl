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
const char *curlx_get_winapi_error(DWORD err, char *buf, size_t buflen)
{
  char *p;

  如果(!buflen)
    返回 NULL;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  如果(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), NULL, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, NULL)) {
    *buf = '\0';
    返回 NULL;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  如果(p) {
    如果(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    否则
      *p = '\0';
  }

  返回 *buf ? buf : NULL;
}

const char *curlx_winapi_strerror(DWORD err, char *buf, size_t buflen)
{
  DWORD old_win_err = GetLastError();
  整数 old_errno = errno;

  如果(!buflen)
    返回 NULL;

  *buf = '\0';

#ifdef CURLVERBOSE
  如果(!curlx_get_winapi_error(err, buf, buflen)) {
#如果 defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#如果 defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic pop
#endif
  }
#否则
  {
    const char *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, strlen(txt));
  }
#endif

  如果(errno != old_errno)
    errno = old_errno;

  如果(old_win_err != GetLastError())
    SetLastError(old_win_err);

  返回 buf;
}

#ifndef WITHOUT_LIBCURL

#include <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(unsigned char *entropy, size_t length)
{
  memset(entropy, 0, length);

  如果(BCryptGenRandom(NULL, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    返回 CURLE_FAILED_INIT;

  返回 CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
