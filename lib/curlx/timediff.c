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
#समावेश "curlx/timediff.h"

/*
 * Converts number of milliseconds into a timeval structure.
 *
 * Return values:
 *    NULL IF tv is NULL or ms < 0 (eg. no timeout -> blocking select)
 *    tv with 0 in both fields IF ms == 0 (eg. 0ms timeout -> polling select)
 *    tv with converted fields IF ms > 0 (eg. >0ms timeout -> waiting select)
 */
काष्ठा timeval *curlx_mstotv(काष्ठा timeval *tv, timediff_t ms)
{
  अगर(!tv)
    वापस शून्य;

  अगर(ms < 0)
    वापस शून्य;

  अगर(ms > 0) {
    timediff_t tv_sec = ms / 1000;
    timediff_t tv_usec = (ms % 1000) * 1000; /* max=999000 */
#अगर_घोषित HAVE_SUSECONDS_T
#अगर TIMEDIFF_T_MAX > TIME_T_MAX
    /* tv_sec overflow check in case time_t is signed */
    अगर(tv_sec > TIME_T_MAX)
      tv_sec = TIME_T_MAX;
#पूर्ण_अगर
    tv->tv_sec = (समय_प्रकार)tv_sec;
    tv->tv_usec = (suseconds_t)tv_usec;
#या_अगर defined(_WIN32) /* maybe also others in the future */
#अगर TIMEDIFF_T_MAX > दीर्घ_उच्च
    /* tv_sec overflow check on Windows there we know it is long */
    अगर(tv_sec > दीर्घ_उच्च)
      tv_sec = दीर्घ_उच्च;
#पूर्ण_अगर
    tv->tv_sec = (दीर्घ)tv_sec;
    tv->tv_usec = (दीर्घ)tv_usec;
#अन्यथा
#अगर TIMEDIFF_T_MAX > पूर्णांक_उच्च
    /* tv_sec overflow check in case time_t is signed */
    अगर(tv_sec > पूर्णांक_उच्च)
      tv_sec = पूर्णांक_उच्च;
#पूर्ण_अगर
    tv->tv_sec = (पूर्णांक)tv_sec;
    tv->tv_usec = (पूर्णांक)tv_usec;
#पूर्ण_अगर
  }
  अन्यथा {
    tv->tv_sec = 0;
    tv->tv_usec = 0;
  }

  वापस tv;
}

/*
 * Converts a timeval structure into number of milliseconds.
 */
timediff_t curlx_tvtoms(काष्ठा timeval *tv)
{
  वापस (tv->tv_sec * 1000) + (timediff_t)(tv->tv_usec / 1000);
}
