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
#अन्तर्भाव "curl_setup.h"

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#ifdef _WIN32
#अन्तर्भाव "curlx/winapi.h"
#अन्तर्भाव "curlx/snprintf.h"
#अन्तर्भाव "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
स्थिरांक अक्षर *curlx_get_winapi_error(DWORD err, अक्षर *buf, माप_प्रकार buflen)
{
  अक्षर *p;

  जर(!buflen)
    परत रिक्त;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  जर(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), रिक्त, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, रिक्त)) {
    *buf = '\0';
    परत रिक्त;
  }

  /* Truncate multiple lines */
  p = म_अक्षर(buf, '\n');
  जर(p) {
    जर(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    अन्यथा
      *p = '\0';
  }

  परत *buf ? buf : रिक्त;
}

स्थिरांक अक्षर *curlx_winapi_strerror(DWORD err, अक्षर *buf, माप_प्रकार buflen)
{
  DWORD old_win_err = GetLastError();
  पूर्णांक old_errno = त्रुटि_सं;

  जर(!buflen)
    परत रिक्त;

  *buf = '\0';

#ifdef CURLVERBOSE
  जर(!curlx_get_winapi_error(err, buf, buflen)) {
#जर defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic warning "-Wformat-truncation=1"
#endif
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#जर defined(__GNUC__) && __GNUC__ >= 7
#pragma GCC diagnostic काढा
#endif
  }
#अन्यथा
  {
    स्थिरांक अक्षर *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, म_माप(txt));
  }
#endif

  जर(त्रुटि_सं != old_errno)
    त्रुटि_सं = old_errno;

  जर(old_win_err != GetLastError())
    SetLastError(old_win_err);

  परत buf;
}

#ifndef WITHOUT_LIBCURL

#अन्तर्भाव <bcrypt.h>
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

CURLcode curlx_win32_random(अचिन्हित अक्षर *entropy, माप_प्रकार length)
{
  स_रखो(entropy, 0, length);

  जर(BCryptGenRandom(रिक्त, entropy, (ULONG)length,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    परत CURLE_FAILED_INIT;

  परत CURLE_OK;
}
#endif /* WITHOUT_LIBCURL */

#endif /* _WIN32 */
