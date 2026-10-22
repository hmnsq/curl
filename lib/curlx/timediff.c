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
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timediff.h"

/*
 * Converts number of milliseconds into a timeval structure.
 *
 * Return values:
 *    NULL IF tv is NULL or ms < 0 (eg. no timeout -> blocking select)
 *    tv with 0 in both fields IF ms == 0 (eg. 0ms timeout -> polling select)
 *    tv with converted fields IF ms > 0 (eg. >0ms timeout -> waiting select)
 */
ਬਣਤਰ timeval *curlx_mstotv(ਬਣਤਰ timeval *tv, timediff_t ms)
{
  ਜੇ(!tv)
    ਵਾਪਸ ਨਿਰਾ;

  ਜੇ(ms < 0)
    ਵਾਪਸ ਨਿਰਾ;

  ਜੇ(ms > 0) {
    timediff_t tv_sec = ms / 1000;
    timediff_t tv_usec = (ms % 1000) * 1000; /* max=999000 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SUSECONDS_T
#ਜੇ TIMEDIFF_T_MAX > ਸਮਾਂ_T_MAX
    /* tv_sec overflow check in case time_t is signed */
    ਜੇ(tv_sec > ਸਮਾਂ_T_MAX)
      tv_sec = ਸਮਾਂ_T_MAX;
#ਜੇ_ਅੰਤ
    tv->tv_sec = (time_t)tv_sec;
    tv->tv_usec = (suseconds_t)tv_usec;
#ਜਾਂਜੇ defined(_WIN32) /* maybe also others in the future */
#ਜੇ TIMEDIFF_T_MAX > LONG_MAX
    /* tv_sec overflow check on Windows there we know it is long */
    ਜੇ(tv_sec > LONG_MAX)
      tv_sec = LONG_MAX;
#ਜੇ_ਅੰਤ
    tv->tv_sec = (ਲੰਮਾ)tv_sec;
    tv->tv_usec = (ਲੰਮਾ)tv_usec;
#ਨਹੀਂਤਾਂ
#ਜੇ TIMEDIFF_T_MAX > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ
    /* tv_sec overflow check in case time_t is signed */
    ਜੇ(tv_sec > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
      tv_sec = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
#ਜੇ_ਅੰਤ
    tv->tv_sec = (ਪੂਰਨ)tv_sec;
    tv->tv_usec = (ਪੂਰਨ)tv_usec;
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ {
    tv->tv_sec = 0;
    tv->tv_usec = 0;
  }

  ਵਾਪਸ tv;
}

/*
 * Converts a timeval structure into number of milliseconds.
 */
timediff_t curlx_tvtoms(ਬਣਤਰ timeval *tv)
{
  ਵਾਪਸ (tv->tv_sec * 1000) + (timediff_t)(tv->tv_usec / 1000);
}
