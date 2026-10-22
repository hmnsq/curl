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
#شامل "curlx/timediff.h"

/*
 * Converts number of milliseconds into a timeval structure.
 *
 * Return values:
 *    NULL IF tv is NULL or ms < 0 (eg. no timeout -> blocking select)
 *    tv with 0 in both fields IF ms == 0 (eg. 0ms timeout -> polling select)
 *    tv with converted fields IF ms > 0 (eg. >0ms timeout -> waiting select)
 */
ساختار timeval *curlx_mstotv(ساختار timeval *tv, timediff_t ms)
{
  اگر(!tv)
    بازگشت پوچ;

  اگر(ms < 0)
    بازگشت پوچ;

  اگر(ms > 0) {
    timediff_t tv_sec = ms / 1000;
    timediff_t tv_usec = (ms % 1000) * 1000; /* max=999000 */
#ifdef HAVE_SUSECONDS_T
#اگر TIMEDIFF_T_MAX > TIME_T_MAX
    /* tv_sec overflow check in case time_t is signed */
    اگر(tv_sec > TIME_T_MAX)
      tv_sec = TIME_T_MAX;
#endif
    tv->tv_sec = (time_t)tv_sec;
    tv->tv_usec = (suseconds_t)tv_usec;
#وگرنهاگر defined(_WIN32) /* maybe also others in the future */
#اگر TIMEDIFF_T_MAX > LONG_MAX
    /* tv_sec overflow check on Windows there we know it is long */
    اگر(tv_sec > LONG_MAX)
      tv_sec = LONG_MAX;
#endif
    tv->tv_sec = (long)tv_sec;
    tv->tv_usec = (long)tv_usec;
#وگرنه
#اگر TIMEDIFF_T_MAX > INT_MAX
    /* tv_sec overflow check in case time_t is signed */
    اگر(tv_sec > INT_MAX)
      tv_sec = INT_MAX;
#endif
    tv->tv_sec = (صحیح)tv_sec;
    tv->tv_usec = (صحیح)tv_usec;
#endif
  }
  وگرنه {
    tv->tv_sec = 0;
    tv->tv_usec = 0;
  }

  بازگشت tv;
}

/*
 * Converts a timeval structure into number of milliseconds.
 */
timediff_t curlx_tvtoms(ساختار timeval *tv)
{
  بازگشت (tv->tv_sec * 1000) + (timediff_t)(tv->tv_usec / 1000);
}
