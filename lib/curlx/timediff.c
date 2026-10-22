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
#ಅನ್ತರ್ಭಾವ "curlx/timediff.h"

/*
 * Converts number of milliseconds into a timeval structure.
 *
 * Return values:
 *    NULL IF tv is NULL or ms < 0 (eg. no timeout -> blocking select)
 *    tv with 0 in both fields IF ms == 0 (eg. 0ms timeout -> polling select)
 *    tv with converted fields IF ms > 0 (eg. >0ms timeout -> waiting select)
 */
ರಚನೆ timeval *curlx_mstotv(ರಚನೆ timeval *tv, timediff_t ms)
{
  ಆದರೆ(!tv)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಆದರೆ(ms < 0)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಆದರೆ(ms > 0) {
    timediff_t tv_sec = ms / 1000;
    timediff_t tv_usec = (ms % 1000) * 1000; /* max=999000 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SUSECONDS_T
#ಆದರೆ TIMEDIFF_T_MAX > ಸಮಯ_T_MAX
    /* tv_sec overflow check in case time_t is signed */
    ಆದರೆ(tv_sec > ಸಮಯ_T_MAX)
      tv_sec = ಸಮಯ_T_MAX;
#ಆದರೆ_ಅಂತ್ಯ
    tv->tv_sec = (time_t)tv_sec;
    tv->tv_usec = (suseconds_t)tv_usec;
#ಅಥವಾಆದರೆ defined(_WIN32) /* maybe also others in the future */
#ಆದರೆ TIMEDIFF_T_MAX > LONG_MAX
    /* tv_sec overflow check on Windows there we know it is long */
    ಆದರೆ(tv_sec > LONG_MAX)
      tv_sec = LONG_MAX;
#ಆದರೆ_ಅಂತ್ಯ
    tv->tv_sec = (ದೀರ್ಘ)tv_sec;
    tv->tv_usec = (ದೀರ್ಘ)tv_usec;
#ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ TIMEDIFF_T_MAX > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ
    /* tv_sec overflow check in case time_t is signed */
    ಆದರೆ(tv_sec > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
      tv_sec = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
#ಆದರೆ_ಅಂತ್ಯ
    tv->tv_sec = (ಪೂರ್ಣಾಂಕ)tv_sec;
    tv->tv_usec = (ಪೂರ್ಣಾಂಕ)tv_usec;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    tv->tv_sec = 0;
    tv->tv_usec = 0;
  }

  ಹಿಂತಿರುಗಿ tv;
}

/*
 * Converts a timeval structure into number of milliseconds.
 */
timediff_t curlx_tvtoms(ರಚನೆ timeval *tv)
{
  ಹಿಂತಿರುಗಿ (tv->tv_sec * 1000) + (timediff_t)(tv->tv_usec / 1000);
}
