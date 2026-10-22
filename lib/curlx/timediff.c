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
#include "curlx/timediff.h"

/*
 * Converts number of milliseconds into a timeval structure.
 *
 * Return values:
 *    NULL IF tv is NULL or ms < 0 (eg. no timeout -> blocking select)
 *    tv with 0 in both fields IF ms == 0 (eg. 0ms timeout -> polling select)
 *    tv with converted fields IF ms > 0 (eg. >0ms timeout -> waiting select)
 */
sa_nracanaa timeval *curlx_mstotv(sa_nracanaa timeval *tv, timediff_t ms)
{
  ຖ້າ(!tv)
    ສົ່ງຄືນ NULL;

  ຖ້າ(ms < 0)
    ສົ່ງຄືນ NULL;

  ຖ້າ(ms > 0) {
    timediff_t tv_sec = ms / 1000;
    timediff_t tv_usec = (ms % 1000) * 1000; /* max=999000 */
#ifdef HAVE_SUSECONDS_T
#ຖ້າ TIMEDIFF_T_MAX > TIME_T_MAX
    /* tv_sec overflow check in case time_t is signed */
    ຖ້າ(tv_sec > TIME_T_MAX)
      tv_sec = TIME_T_MAX;
#endif
    tv->tv_sec = (time_t)tv_sec;
    tv->tv_usec = (suseconds_t)tv_usec;
#elif defined(_WIN32) /* maybe also others in the future */
#ຖ້າ TIMEDIFF_T_MAX > LONG_MAX
    /* tv_sec overflow check on Windows there we know it is long */
    ຖ້າ(tv_sec > LONG_MAX)
      tv_sec = LONG_MAX;
#endif
    tv->tv_sec = (long)tv_sec;
    tv->tv_usec = (long)tv_usec;
#ບໍ່ດັ່ງນັ້ນ
#ຖ້າ TIMEDIFF_T_MAX > INT_MAX
    /* tv_sec overflow check in case time_t is signed */
    ຖ້າ(tv_sec > INT_MAX)
      tv_sec = INT_MAX;
#endif
    tv->tv_sec = (ຈຳນວນເຕັມ)tv_sec;
    tv->tv_usec = (ຈຳນວນເຕັມ)tv_usec;
#endif
  }
  ບໍ່ດັ່ງນັ້ນ {
    tv->tv_sec = 0;
    tv->tv_usec = 0;
  }

  ສົ່ງຄືນ tv;
}

/*
 * Converts a timeval structure into number of milliseconds.
 */
timediff_t curlx_tvtoms(sa_nracanaa timeval *tv)
{
  ສົ່ງຄືນ (tv->tv_sec * 1000) + (timediff_t)(tv->tv_usec / 1000);
}
