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
#समावेश "curl_setup.h"

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#अगर_घोषित _WIN32
#समावेश "curlx/winapi.h"
#समावेश "curlx/snprintf.h"
#समावेश "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
स्थिर अक्षर *curlx_get_winapi_error(DWORD err, अक्षर *buf, माप_प्रकार buflen)
{
  अक्षर *p;

  अगर(!buflen)
    वापस शून्य;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  अगर(!FormatMessageA((FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS), शून्य, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, शून्य)) {
    *buf = '\0';
    वापस शून्य;
  }

  /* Truncate multiple lines */
  p = म_अक्षर(buf, '\n');
  अगर(p) {
    अगर(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    अन्यथा
      *p = '\0';
  }

  वापस *buf ? buf : शून्य;
}

स्थिर अक्षर *curlx_winapi_strerror(DWORD err, अक्षर *buf, माप_प्रकार buflen)
{
  DWORD old_win_err = GetLastError();
  पूर्णांक old_errno = त्रुटि_सं;

  अगर(!buflen)
    वापस शून्य;

  *buf = '\0';

#अगर_घोषित CURLVERBOSE
  अगर(!curlx_get_winapi_error(err, buf, buflen)) {
#अगर defined(__GNUC__) && __GNUC__ >= 7
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic warning "-Wformat-truncation=1"
#पूर्ण_अगर
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#अगर defined(__GNUC__) && __GNUC__ >= 7
#आशय GCC diagnostic निकालो
#पूर्ण_अगर
  }
#अन्यथा
  {
    स्थिर अक्षर *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, म_माप(txt));
  }
#पूर्ण_अगर

  अगर(त्रुटि_सं != old_errno)
    त्रुटि_सं = old_errno;

  अगर(old_win_err != GetLastError())
    SetLastError(old_win_err);

  वापस buf;
}

#अगर_अघोषित WITHOUT_LIBCURL

#समावेश <bcrypt.h>
#अगर_अघोषित STATUS_SUCCESS
#घोषणा STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#पूर्ण_अगर

CURLcode curlx_win32_random(अचिन्हित अक्षर *entropy, माप_प्रकार लंबाई)
{
  स_रखो(entropy, 0, लंबाई);

  अगर(BCryptGenRandom(शून्य, entropy, (ULONG)लंबाई,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != STATUS_SUCCESS)
    वापस CURLE_FAILED_INIT;

  वापस CURLE_OK;
}
#पूर्ण_अगर /* WITHOUT_LIBCURL */

#पूर्ण_अगर /* _WIN32 */
