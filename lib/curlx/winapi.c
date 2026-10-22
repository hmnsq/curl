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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"
#ಅನ್ತರ್ಭಾವ "curlx/snprintf.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curlx_get_winapi_error(DWORD err, ಅಕ್ಷರ *buf, size_t buflen)
{
  ಅಕ್ಷರ *p;

  ಆದರೆ(!buflen)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  ಆದರೆ(!Format_ಸಂದೇಶ_A((FORMAT_ಸಂದೇಶ_FROM_SYSTEM |
                      FORMAT_ಸಂದೇಶ_IGNORE_INSERTS), ಖಾಲಿ, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, ಖಾಲಿ)) {
    *buf = '\0';
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  ಆದರೆ(p) {
    ಆದರೆ(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    ಇಲ್ಲದಿದ್ದರೆ
      *p = '\0';
  }

  ಹಿಂತಿರುಗಿ *buf ? buf : ಖಾಲಿ;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curlx_winapi_strerror(DWORD err, ಅಕ್ಷರ *buf, size_t buflen)
{
  DWORD old_win_err = Get_ಕೊನೆಯ_Error();
  ಪೂರ್ಣಾಂಕ old_errno = errno;

  ಆದರೆ(!buflen)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  *buf = '\0';

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(!curlx_get_winapi_error(err, buf, buflen)) {
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 7
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic warning "-Wformat-truncation=1"
#ಆದರೆ_ಅಂತ್ಯ
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#ಆದರೆ defined(__GNUC__) && __GNUC__ >= 7
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  }
#ಇಲ್ಲದಿದ್ದರೆ
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(txt));
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(errno != old_errno)
    errno = old_errno;

  ಆದರೆ(old_win_err != Get_ಕೊನೆಯ_Error())
    ಗಣ_ಕೊನೆಯ_Error(old_win_err);

  ಹಿಂತಿರುಗಿ buf;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WITHOUT_LIBCURL

#ಅನ್ತರ್ಭಾವ <bcrypt.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಸ್ಥಿತಿ_SUCCESS
#ವ್ಯಾಖ್ಯಾನಿಸು ಸ್ಥಿತಿ_SUCCESS ((NTSTATUS)0x00000000L)
#ಆದರೆ_ಅಂತ್ಯ

CURLcode curlx_win32_random(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *entropy, size_t ಉದ್ದಳತೆ)
{
  memset(entropy, 0, ಉದ್ದಳತೆ);

  ಆದರೆ(BCryptGenRandom(ಖಾಲಿ, entropy, (ULONG)ಉದ್ದಳತೆ,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != ಸ್ಥಿತಿ_SUCCESS)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* WITHOUT_LIBCURL */

#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */
