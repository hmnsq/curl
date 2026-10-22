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
#അന്തര്ഭാവ "curl_setup.h"

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ifdef _WIN32
#അന്തര്ഭാവ "curlx/winapi.h"
#അന്തര്ഭാവ "curlx/snprintf.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
സ്ഥിരം അക്ഷരം *curlx_get_winapi_error(DWORD err, അക്ഷരം *buf, size_t buflen)
{
  അക്ഷരം *p;

  എങ്കിൽ(!buflen)
    തിരികെ ശൂന്യ;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  എങ്കിൽ(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), ശൂന്യ, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, ശൂന്യ)) {
    *buf = '\0';
    തിരികെ ശൂന്യ;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  എങ്കിൽ(p) {
    എങ്കിൽ(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    അല്ലെങ്കിൽ
      *p = '\0';
  }

  തിരികെ *buf ? buf : ശൂന്യ;
}

സ്ഥിരം അക്ഷരം *curlx_winapi_strerror(DWORD err, അക്ഷരം *buf, size_t buflen)
{
  DWORD old_win_err = GetLastError();
  പൂർണ്ണസംഖ്യ old_errno = errno;

  എങ്കിൽ(!buflen)
    തിരികെ ശൂന്യ;

  *buf = '\0';

#ifdef CURLVERBOSE
  എങ്കിൽ(!curlx_get_winapi_error(err, buf, buflen)) {
#എങ്കിൽ defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#എങ്കിൽ defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic എടുക്കുക
#endif
  }
#അല്ലെങ്കിൽ
  {
    സ്ഥിരം അക്ഷരം *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, strlen(txt));
  }
#endif

  എങ്കിൽ(errno != old_errno)
    errno = old_errno;

  എങ്കിൽ(old_win_err != GetLastError())
    SetLastError(old_win_err);

  തിരികെ buf;
}

#ifndef WITHOUT_LIBCURL

#അന്തര്ഭാവ <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(unsigned അക്ഷരം *entropy, size_t length)
{
  memset(entropy, 0, length);

  എങ്കിൽ(BCryptGenRandom(ശൂന്യ, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    തിരികെ CURLE_FAILED_INIT;

  തിരികെ CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
