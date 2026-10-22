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
#اگر_تعریف_شدہ _WIN32
#شامل "curlx/winapi.h"
#شامل "curlx/snprintf.h"
#شامل "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ثابت حرف *curlx_get_winapi_error(DWORD err, حرف *buf, size_t buflen)
{
  حرف *p;

  اگر(!buflen)
    واپس تہی;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  اگر(!Format_پیغام_A((FORMAT_پیغام_FROM_SYSTEM |
                      FORMAT_پیغام_IGNORE_INSERTS), تہی, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, تہی)) {
    *buf = '\0';
    واپس تہی;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  اگر(p) {
    اگر(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    ورنہ
      *p = '\0';
  }

  واپس *buf ? buf : تہی;
}

ثابت حرف *curlx_winapi_strerror(DWORD err, حرف *buf, size_t buflen)
{
  DWORD old_win_err = Get_آخری_Error();
  صحیح old_errno = errno;

  اگر(!buflen)
    واپس تہی;

  *buf = '\0';

#اگر_تعریف_شدہ CURLVERBOSE
  اگر(!curlx_get_winapi_error(err, buf, buflen)) {
#اگر defined(__GNUC__) && __GNUC__ >= 7
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic warning "-Wformat-truncation=1"
#اگر_ختم
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#اگر defined(__GNUC__) && __GNUC__ >= 7
#ہدایت GCC diagnostic نکالو
#اگر_ختم
  }
#ورنہ
  {
    ثابت حرف *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, لڑی_لمبائی(txt));
  }
#اگر_ختم

  اگر(errno != old_errno)
    errno = old_errno;

  اگر(old_win_err != Get_آخری_Error())
    سیٹ_آخری_Error(old_win_err);

  واپس buf;
}

#اگر_غیر_تعریف_شدہ WITHOUT_LIBCURL

#شامل <bcrypt.h>
#اگر_غیر_تعریف_شدہ حالت_SUCCESS
#تعریف_کرو حالت_SUCCESS ((NTSTATUS)0x00000000L)
#اگر_ختم

CURLcode curlx_win32_random(غیرعلامتی حرف *entropy, size_t لمبائی)
{
  memset(entropy, 0, لمبائی);

  اگر(BCryptGenRandom(تہی, entropy, (ULONG)لمبائی,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != حالت_SUCCESS)
    واپس CURLE_FAILED_INIT;

  واپس CURLE_OK;
}
#اگر_ختم /* WITHOUT_LIBCURL */

#اگر_ختم /* _WIN32 */
