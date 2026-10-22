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
ساخت timeval *curlx_mstotv(ساخت timeval *tv, timediff_t ms)
{
  اگر(!tv)
    واپس تہی;

  اگر(ms < 0)
    واپس تہی;

  اگر(ms > 0) {
    timediff_t tv_sec = ms / 1000;
    timediff_t tv_usec = (ms % 1000) * 1000; /* max=999000 */
#اگر_تعریف_شدہ HAVE_SUSECONDS_T
#اگر TIMEDIFF_T_MAX > وقت_T_MAX
    /* tv_sec overflow check in case time_t is signed */
    اگر(tv_sec > وقت_T_MAX)
      tv_sec = وقت_T_MAX;
#اگر_ختم
    tv->tv_sec = (time_t)tv_sec;
    tv->tv_usec = (suseconds_t)tv_usec;
#وگرنہ_اگر defined(_WIN32) /* maybe also others in the future */
#اگر TIMEDIFF_T_MAX > LONG_MAX
    /* tv_sec overflow check on Windows there we know it is long */
    اگر(tv_sec > LONG_MAX)
      tv_sec = LONG_MAX;
#اگر_ختم
    tv->tv_sec = (طویل)tv_sec;
    tv->tv_usec = (طویل)tv_usec;
#ورنہ
#اگر TIMEDIFF_T_MAX > زیادہ_سے_زیادہ_عدد
    /* tv_sec overflow check in case time_t is signed */
    اگر(tv_sec > زیادہ_سے_زیادہ_عدد)
      tv_sec = زیادہ_سے_زیادہ_عدد;
#اگر_ختم
    tv->tv_sec = (صحیح)tv_sec;
    tv->tv_usec = (صحیح)tv_usec;
#اگر_ختم
  }
  ورنہ {
    tv->tv_sec = 0;
    tv->tv_usec = 0;
  }

  واپس tv;
}

/*
 * Converts a timeval structure into number of milliseconds.
 */
timediff_t curlx_tvtoms(ساخت timeval *tv)
{
  واپس (tv->tv_sec * 1000) + (timediff_t)(tv->tv_usec / 1000);
}
