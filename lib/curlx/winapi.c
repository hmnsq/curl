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
#यदि_परिभाषितम् _WIN32
#अन्तर्भाव "curlx/winapi.h"
#अन्तर्भाव "curlx/snprintf.h"
#अन्तर्भाव "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ध्रुव वर्ण *curlx_get_winapi_error(DWORD err, वर्ण *buf, size_t buflen)
{
  वर्ण *p;

  यदि(!buflen)
    प्रत्यागम रिक्त;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  यदि(!Format_सन्देशः_A((FORMAT_सन्देशः_FROM_SYSTEM |
                      FORMAT_सन्देशः_IGNORE_INSERTS), रिक्त, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, रिक्त)) {
    *buf = '\0';
    प्रत्यागम रिक्त;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  यदि(p) {
    यदि(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    अन्यथा
      *p = '\0';
  }

  प्रत्यागम *buf ? buf : रिक्त;
}

ध्रुव वर्ण *curlx_winapi_strerror(DWORD err, वर्ण *buf, size_t buflen)
{
  DWORD old_win_err = Get_चरमः_Error();
  पूर्णाङ्क old_errno = errno;

  यदि(!buflen)
    प्रत्यागम रिक्त;

  *buf = '\0';

#यदि_परिभाषितम् CURLVERBOSE
  यदि(!curlx_get_winapi_error(err, buf, buflen)) {
#यदि defined(__GNUC__) && __GNUC__ >= 7
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic warning "-Wformat-truncation=1"
#यदि_अन्तः
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#यदि defined(__GNUC__) && __GNUC__ >= 7
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः
  }
#अन्यथा
  {
    ध्रुव वर्ण *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, वाक्यदैर्घ्यम्(txt));
  }
#यदि_अन्तः

  यदि(errno != old_errno)
    errno = old_errno;

  यदि(old_win_err != Get_चरमः_Error())
    समुच्चयः_चरमः_Error(old_win_err);

  प्रत्यागम buf;
}

#यदि_अपरिभाषितम् WITHOUT_LIBCURL

#अन्तर्भाव <bcrypt.h>
#यदि_अपरिभाषितम् स्थितिः_SUCCESS
#परिभाषय स्थितिः_SUCCESS ((NTSTATUS)0x00000000L)
#यदि_अन्तः

CURLcode curlx_win32_random(अचिह्नित वर्ण *entropy, size_t दीर्घता)
{
  स्मृतिं_पूरय(entropy, 0, दीर्घता);

  यदि(BCryptGenRandom(रिक्त, entropy, (ULONG)दीर्घता,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != स्थितिः_SUCCESS)
    प्रत्यागम CURLE_FAILED_INIT;

  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः /* WITHOUT_LIBCURL */

#यदि_अन्तः /* _WIN32 */
