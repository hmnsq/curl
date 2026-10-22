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
#समावेश "curlx/timeval.h"

#अगर_घोषित _WIN32

अटल LARGE_INTEGER s_time_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
व्योम curlx_now_init(व्योम)
{
  QueryPerformanceFrequency(&s_time_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
व्योम curlx_pnow(काष्ठा curltime *pnow)
{
  LARGE_INTEGER गिनती;
  DEBUGASSERT(s_time_freq.QuadPart);
  Query_Performance_गणक(&गिनती);
  pnow->tv_sec = (समय_प्रकार)(गिनती.QuadPart / s_time_freq.QuadPart);
  pnow->tv_usec = (पूर्णांक)((गिनती.QuadPart % s_time_freq.QuadPart) * 1000000 /
                        s_time_freq.QuadPart);
}

#या_अगर defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

व्योम curlx_pnow(काष्ठा curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  काष्ठा timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#अगर defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
  bool have_clock_gettime = FALSE;
  अगर(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#पूर्ण_अगर

#अगर_घोषित HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  अगर(
#अगर defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#पूर्ण_अगर
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (पूर्णांक)(tsnow.tv_nsec / 1000);
  }
  अन्यथा
#पूर्ण_अगर

  अगर(
#अगर defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#पूर्ण_अगर
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (पूर्णांक)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#अगर_घोषित HAVE_GETTIMEOFDAY
  अन्यथा {
    काष्ठा timeval now;
    (व्योम)gettimeofday(&now, शून्य);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (पूर्णांक)now.tv_usec;
  }
#अन्यथा
  अन्यथा {
    pnow->tv_sec = समय(शून्य);
    pnow->tv_usec = 0;
  }
#पूर्ण_अगर
}

#या_अगर defined(HAVE_MACH_ABSOLUTE_TIME)

#समावेश <mach/mach_time.h>

व्योम curlx_pnow(काष्ठा curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  अटल mach_timebase_info_तथ्य_t timebase;
  uint64_t usecs;

  अगर(timebase.denom == 0)
    (व्योम)mach_timebase_info(&timebase);

  usecs = mach_absolute_time();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (पूर्णांक)(usecs % 1000000);
}

#या_अगर defined(HAVE_GETTIMEOFDAY)

व्योम curlx_pnow(काष्ठा curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  काष्ठा timeval now;
  (व्योम)gettimeofday(&now, शून्य);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (पूर्णांक)now.tv_usec;
}

#अन्यथा

व्योम curlx_pnow(काष्ठा curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = समय(शून्य);
  pnow->tv_usec = 0;
  अगर(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#पूर्ण_अगर

काष्ठा curltime curlx_now(व्योम)
{
  काष्ठा curltime now;
  curlx_pnow(&now);
  वापस now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(स्थिर काष्ठा curltime *newer,
                              स्थिर काष्ठा curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  अगर(diff >= (TIMEDIFF_T_MAX / 1000))
    वापस TIMEDIFF_T_MAX;
  अन्यथा अगर(diff <= (TIMEDIFF_T_MIN / 1000))
    वापस TIMEDIFF_T_MIN;
  वापस (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(काष्ठा curltime newer, काष्ठा curltime older)
{
  वापस curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(काष्ठा curltime newer,
                                  काष्ठा curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  अगर(diff >= (TIMEDIFF_T_MAX / 1000))
    वापस TIMEDIFF_T_MAX;
  अन्यथा अगर(diff <= (TIMEDIFF_T_MIN / 1000))
    वापस TIMEDIFF_T_MIN;
  वापस (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_to_ceil_ms(timediff_t us)
{
  वापस (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(स्थिर काष्ठा curltime *newer,
                              स्थिर काष्ठा curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  अगर(diff >= (TIMEDIFF_T_MAX / 1000000))
    वापस TIMEDIFF_T_MAX;
  अन्यथा अगर(diff <= (TIMEDIFF_T_MIN / 1000000))
    वापस TIMEDIFF_T_MIN;
  वापस (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(काष्ठा curltime newer, काष्ठा curltime older)
{
  वापस curlx_ptimediff_us(&newer, &older);
}

#अगर defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#समावेश <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#अगर_घोषित _USE_32BIT_TIME_T
#घोषणा gmtime_s _gmtime32_s
#अन्यथा
#घोषणा gmtime_s _gmtime64_s
#पूर्ण_अगर
#पूर्ण_अगर

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(समय_प्रकार intime, काष्ठा पंचांग *store)
{
#अगर_घोषित _WIN32
  अगर(gmtime_s(store, &intime)) /* thread-safe */
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
#या_अगर defined(HAVE_GMTIME_R)
  स्थिर काष्ठा पंचांग *पंचांग;
  पंचांग = gmtime_r(&intime, store); /* thread-safe */
  अगर(!पंचांग)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
#अन्यथा
  स्थिर काष्ठा पंचांग *पंचांग;
  /* !checksrc! disable BANNEDFUNC 1 */
  पंचांग = स_जमट(&intime); /* not thread-safe */
  अगर(पंचांग)
    *store = *पंचांग; /* copy the pointed struct to the local copy */
  अन्यथा
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
#पूर्ण_अगर

  वापस CURLE_OK;
}
