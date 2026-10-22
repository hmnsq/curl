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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/winapi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/snprintf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ਅਚਲ ਅੱਖਰ *curlx_get_winapi_error(DWORD err, ਅੱਖਰ *buf, size_t buflen)
{
  ਅੱਖਰ *p;

  ਜੇ(!buflen)
    ਵਾਪਸ ਨਿਰਾ;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  ਜੇ(!Format_ਸੁਨੇਹਾ_A((FORMAT_ਸੁਨੇਹਾ_FROM_SYSTEM |
                      FORMAT_ਸੁਨੇਹਾ_IGNORE_INSERTS), ਨਿਰਾ, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, ਨਿਰਾ)) {
    *buf = '\0';
    ਵਾਪਸ ਨਿਰਾ;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  ਜੇ(p) {
    ਜੇ(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    ਨਹੀਂਤਾਂ
      *p = '\0';
  }

  ਵਾਪਸ *buf ? buf : ਨਿਰਾ;
}

ਅਚਲ ਅੱਖਰ *curlx_winapi_strerror(DWORD err, ਅੱਖਰ *buf, size_t buflen)
{
  DWORD old_win_err = Get_ਆਖਰੀ_Error();
  ਪੂਰਨ old_errno = errno;

  ਜੇ(!buflen)
    ਵਾਪਸ ਨਿਰਾ;

  *buf = '\0';

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(!curlx_get_winapi_error(err, buf, buflen)) {
#ਜੇ defined(__GNUC__) && __GNUC__ >= 7
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic warning "-Wformat-truncation=1"
#ਜੇ_ਅੰਤ
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#ਜੇ defined(__GNUC__) && __GNUC__ >= 7
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  }
#ਨਹੀਂਤਾਂ
  {
    ਅਚਲ ਅੱਖਰ *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, ਲੜੀ_ਲੰਬਾਈ(txt));
  }
#ਜੇ_ਅੰਤ

  ਜੇ(errno != old_errno)
    errno = old_errno;

  ਜੇ(old_win_err != Get_ਆਖਰੀ_Error())
    ਸੈੱਟ_ਆਖਰੀ_Error(old_win_err);

  ਵਾਪਸ buf;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WITHOUT_LIBCURL

#ਅਨ੍ਤਰ੍ਭਾਵ <bcrypt.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਥਿਤੀ_SUCCESS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਥਿਤੀ_SUCCESS ((NTSTATUS)0x00000000L)
#ਜੇ_ਅੰਤ

CURLcode curlx_win32_random(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *entropy, size_t ਲੰਮਾਈ)
{
  memset(entropy, 0, ਲੰਮਾਈ);

  ਜੇ(BCryptGenRandom(ਨਿਰਾ, entropy, (ULONG)ਲੰਮਾਈ,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != ਸਥਿਤੀ_SUCCESS)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* WITHOUT_LIBCURL */

#ਜੇ_ਅੰਤ /* _WIN32 */
