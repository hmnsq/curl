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
#అన్తర్భావ "curl_setup.h"

/*
 * curlx_winapi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows API.
 */
#నిర్వచితమైతే _WIN32
#అన్తర్భావ "curlx/winapi.h"
#అన్తర్భావ "curlx/snprintf.h"
#అన్తర్భావ "curlx/strcopy.h"

/* This is a helper function for curlx_strerror that converts Windows API error
 * codes (GetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
స్థిరరాశి అక్షరం *curlx_get_winapi_error(DWORD err, అక్షరం *buf, size_t buflen)
{
  అక్షరం *p;

  ఒకవేళ(!buflen)
    తిరిగి ఖాళీ;

  /* We return the local codepage version of the error string because if it is
     output to the user's terminal, it is likely done with functions which
     expect the local codepage (eg fprintf, failf, infof). */
  ఒకవేళ(!Format_సందేశం_A((FORMAT_సందేశం_FROM_SYSTEM |
                      FORMAT_సందేశం_IGNORE_INSERTS), ఖాళీ, err,
                     LANG_NEUTRAL, buf, (DWORD)buflen, ఖాళీ)) {
    *buf = '\0';
    తిరిగి ఖాళీ;
  }

  /* Truncate multiple lines */
  p = strchr(buf, '\n');
  ఒకవేళ(p) {
    ఒకవేళ(p > buf && *(p - 1) == '\r')
      *(p - 1) = '\0';
    లేకపోతే
      *p = '\0';
  }

  తిరిగి *buf ? buf : ఖాళీ;
}

స్థిరరాశి అక్షరం *curlx_winapi_strerror(DWORD err, అక్షరం *buf, size_t buflen)
{
  DWORD old_win_err = Get_చివరి_Error();
  పూర్ణాంకం old_errno = errno;

  ఒకవేళ(!buflen)
    తిరిగి ఖాళీ;

  *buf = '\0';

#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(!curlx_get_winapi_error(err, buf, buflen)) {
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 7
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic warning "-Wformat-truncation=1"
#అయితే_ముగింపు
    /* some GCC compilers cause false positive warnings if we allow this
       warning */
    SNPRINTF(buf, buflen, "Unknown error %lu (0x%08lx)", err, err);
#ఒకవేళ defined(__GNUC__) && __GNUC__ >= 7
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు
  }
#లేకపోతే
  {
    స్థిరరాశి అక్షరం *txt = (err == ERROR_SUCCESS) ? "No error" : "Error";
    curlx_strcopy(buf, buflen, txt, అక్షరశ్రేణి_పొడవు(txt));
  }
#అయితే_ముగింపు

  ఒకవేళ(errno != old_errno)
    errno = old_errno;

  ఒకవేళ(old_win_err != Get_చివరి_Error())
    సమితి_చివరి_Error(old_win_err);

  తిరిగి buf;
}

#నిర్వచితం_కాకపోతే WITHOUT_LIBCURL

#అన్తర్భావ <bcrypt.h>
#నిర్వచితం_కాకపోతే స్థితి_SUCCESS
#నిర్వచించు స్థితి_SUCCESS ((NTSTATUS)0x00000000L)
#అయితే_ముగింపు

CURLcode curlx_win32_random(గుర్తులేని అక్షరం *entropy, size_t నిడివి)
{
  memset(entropy, 0, నిడివి);

  ఒకవేళ(BCryptGenRandom(ఖాళీ, entropy, (ULONG)నిడివి,
                     BCRYPT_USE_SYSTEM_PREFERRED_RNG) != స్థితి_SUCCESS)
    తిరిగి CURLE_FAILED_INIT;

  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* WITHOUT_LIBCURL */

#అయితే_ముగింపు /* _WIN32 */
