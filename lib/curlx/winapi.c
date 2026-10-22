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
#அந்தர்பாவ "curl_setup.h"

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ifdef _WIN32
#அந்தர்பாவ "curlx/winapi.h"
#அந்தர்பாவ "curlx/snprintf.h"
#அந்தர்பாவ "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
மாறிலி எழுத்து *curlx_get_winapi_error(DWORD err, எழுத்து *buf, size_t buflen)
{
  எழுத்து *p;

  எனில்(!buflen)
    திரும்பு வெற்று;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  எனில்(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), வெற்று, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, வெற்று)) {
    *buf = '\0';
    திரும்பு வெற்று;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  எனில்(p) {
    எனில்(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    இல்லையெனில்
      *p = '\0';
  }

  திரும்பு *buf ? buf : வெற்று;
}

மாறிலி எழுத்து *curlx_winapi_strerror(DWORD err, எழுத்து *buf, size_t buflen)
{
  DWORD old_win_err = GetLastError();
  முழு old_errno = errno;

  எனில்(!buflen)
    திரும்பு வெற்று;

  *buf = '\0';

#ifdef CURLVERBOSE
  எனில்(!curlx_get_winapi_error(err, buf, buflen)) {
#எனில் defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#எனில் defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic எடு
#endif
  }
#இல்லையெனில்
  {
    மாறிலி எழுத்து *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, strlen(txt));
  }
#endif

  எனில்(errno != old_errno)
    errno = old_errno;

  எனில்(old_win_err != GetLastError())
    SetLastError(old_win_err);

  திரும்பு buf;
}

#ifndef WITHOUT_LIBCURL

#அந்தர்பாவ <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(unsigned எழுத்து *entropy, size_t length)
{
  memset(entropy, 0, length);

  எனில்(BCryptGenRandom(வெற்று, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    திரும்பு CURLE_FAILED_INIT;

  திரும்பு CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
