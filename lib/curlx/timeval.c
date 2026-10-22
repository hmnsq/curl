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
#ضمن "curlx/timeval.h"

#ifdef _WIN32

ساكن LARGE_INTEGER s_time_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
فراغ curlx_now_init(فراغ)
{
  QueryPerformanceFrequency(&s_time_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
فراغ curlx_pnow(بنية curltime *pnow)
{
  LARGE_INTEGER كدي;
  DEBUGASSERT(s_time_freq.QuadPart);
  QueryPerformanceCounter(&كدي);
  pnow->tv_sec = (time_t)(كدي.QuadPart / s_time_freq.QuadPart);
  pnow->tv_usec = (صحيح)((كدي.QuadPart % s_time_freq.QuadPart) * 1000000 /
                        s_time_freq.QuadPart);
}

#وإلاإذا defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

فراغ curlx_pnow(بنية curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  بنية timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#إذا defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
  bool have_clock_gettime = FALSE;
  إذا(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#endif

#ifdef HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  إذا(
#إذا defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (صحيح)(tsnow.tv_nsec / 1000);
  }
  وإلا
#endif

  إذا(
#إذا defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (صحيح)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#ifdef HAVE_GETTIMEOFDAY
  وإلا {
    بنية timeval now;
    (فراغ)gettimeofday(&now, عدم);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (صحيح)now.tv_usec;
  }
#وإلا
  وإلا {
    pnow->tv_sec = time(عدم);
    pnow->tv_usec = 0;
  }
#endif
}

#وإلاإذا defined(HAVE_MACH_ABSOLUTE_TIME)

#ضمن <mach/mach_time.h>

فراغ curlx_pnow(بنية curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  ساكن mach_timebase_info_data_t timebase;
  uint64_t usecs;

  إذا(timebase.denom == 0)
    (فراغ)mach_timebase_info(&timebase);

  usecs = mach_absolute_time();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (صحيح)(usecs % 1000000);
}

#وإلاإذا defined(HAVE_GETTIMEOFDAY)

فراغ curlx_pnow(بنية curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  بنية timeval now;
  (فراغ)gettimeofday(&now, عدم);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (صحيح)now.tv_usec;
}

#وإلا

فراغ curlx_pnow(بنية curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = time(عدم);
  pnow->tv_usec = 0;
  إذا(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#endif

بنية curltime curlx_now(فراغ)
{
  بنية curltime now;
  curlx_pnow(&now);
  أرجع now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(ثابت بنية curltime *newer,
                              ثابت بنية curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  إذا(diff >= (TIMEDIFF_T_MAX / 1000))
    أرجع TIMEDIFF_T_MAX;
  وإلا إذا(diff <= (TIMEDIFF_T_MIN / 1000))
    أرجع TIMEDIFF_T_MIN;
  أرجع (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(بنية curltime newer, بنية curltime older)
{
  أرجع curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(بنية curltime newer,
                                  بنية curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  إذا(diff >= (TIMEDIFF_T_MAX / 1000))
    أرجع TIMEDIFF_T_MAX;
  وإلا إذا(diff <= (TIMEDIFF_T_MIN / 1000))
    أرجع TIMEDIFF_T_MIN;
  أرجع (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_to_ceil_ms(timediff_t us)
{
  أرجع (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(ثابت بنية curltime *newer,
                              ثابت بنية curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  إذا(diff >= (TIMEDIFF_T_MAX / 1000000))
    أرجع TIMEDIFF_T_MAX;
  وإلا إذا(diff <= (TIMEDIFF_T_MIN / 1000000))
    أرجع TIMEDIFF_T_MIN;
  أرجع (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(بنية curltime newer, بنية curltime older)
{
  أرجع curlx_ptimediff_us(&newer, &older);
}

#إذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#ضمن <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#ifdef _USE_32BIT_TIME_T
#define gmtime_s _gmtime32_s
#وإلا
#define gmtime_s _gmtime64_s
#endif
#endif

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(time_t intime, بنية tm *store)
{
#ifdef _WIN32
  إذا(gmtime_s(store, &intime)) /* thread-safe */
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
#وإلاإذا defined(HAVE_GMTIME_R)
  ثابت بنية tm *tm;
  tm = gmtime_r(&intime, store); /* thread-safe */
  إذا(!tm)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
#وإلا
  ثابت بنية tm *tm;
  /* !checksrc! disable BANNEDFUNC 1 */
  tm = gmtime(&intime); /* not thread-safe */
  إذا(tm)
    *store = *tm; /* copy the pointed struct to the local copy */
  وإلا
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
#endif

  أرجع CURLE_OK;
}
