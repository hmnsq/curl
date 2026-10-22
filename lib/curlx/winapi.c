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
#ضمن "curl_setup.h"

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#اگر_تعریف_شده _WIN32
#ضمن "curlx/winapi.h"
#ضمن "curlx/snprintf.h"
#ضمن "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ثابت حرف *curlx_get_winapi_error(DWORD err, حرف *buf, size_t buflen)
{
  حرف *p;

  إذا(!buflen)
    أرجع عدم;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  إذا(!Format_پیام_A((FORMAT_پیام_FROM_SYSTEM |
                      FORMAT_پیام_IGNORE_INSERTS), عدم, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, عدم)) {
    *buf = '\0';
    أرجع عدم;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  إذا(p) {
    إذا(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    وإلا
      *p = '\0';
  }

  أرجع *buf ? buf : عدم;
}

ثابت حرف *curlx_winapi_strerror(DWORD err, حرف *buf, size_t buflen)
{
  DWORD old_win_err = Get_آخرین_Error();
  صحيح old_errno = errno;

  إذا(!buflen)
    أرجع عدم;

  *buf = '\0';

#اگر_تعریف_شده CURLVERBOSE
  إذا(!curlx_get_winapi_error(err, buf, buflen)) {
#إذا defined(__GNUC__) && __GNUC__ >= 7
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic warning "-Wformat-truncation=1"
#پایان_اگر
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#إذا defined(__GNUC__) && __GNUC__ >= 7
#رهنمود GCC diagnostic برداشت
#پایان_اگر
  }
#وإلا
  {
    ثابت حرف *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, طول_رشته(txt));
  }
#پایان_اگر

  إذا(errno != old_errno)
    errno = old_errno;

  إذا(old_win_err != Get_آخرین_Error())
    مجموعه_آخرین_Error(old_win_err);

  أرجع buf;
}

#اگر_تعریف_نشده WITHOUT_LIBCURL

#ضمن <bcrypt.h>
#اگر_تعریف_نشده وضعیت_SUCCESS
#تعریف_کن وضعیت_SUCCESS ((NTSTATUS)0x00000000L)
#پایان_اگر

CURLcode curlx_win32_random(بیعلامت حرف *entropy, size_t درازی)
{
  memset(entropy, 0, درازی);

  إذا(BCryptGenRandom(عدم, entropy, (ULONG)درازی,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != وضعیت_SUCCESS)
    أرجع CURLE_FAILED_INIT;

  أرجع CURLE_OK;
}
#پایان_اگر /* WITHOUT_LIBCURL */

#پایان_اگر /* _WIN32 */
