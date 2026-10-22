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
#شامل "curlx/timeval.h"

#ifdef _WIN32

ایستا LARGE_INTEGER s_time_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
تهی curlx_now_init(تهی)
{
  QueryPerformanceFrequency(&s_time_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
تهی curlx_pnow(ساختار curltime *pnow)
{
  LARGE_INTEGER شمار;
  DEBUGASSERT(s_time_freq.QuadPart);
  QueryPerformanceCounter(&شمار);
  pnow->tv_sec = (time_t)(شمار.QuadPart / s_time_freq.QuadPart);
  pnow->tv_usec = (صحیح)((شمار.QuadPart % s_time_freq.QuadPart) * 1000000 /
                        s_time_freq.QuadPart);
}

#وگرنهاگر defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

تهی curlx_pnow(ساختار curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  ساختار timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#اگر defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
  bool have_clock_gettime = FALSE;
  اگر(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#endif

#ifdef HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  اگر(
#اگر defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (صحیح)(tsnow.tv_nsec / 1000);
  }
  وگرنه
#endif

  اگر(
#اگر defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (صحیح)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#ifdef HAVE_GETTIMEOFDAY
  وگرنه {
    ساختار timeval now;
    (تهی)gettimeofday(&now, پوچ);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (صحیح)now.tv_usec;
  }
#وگرنه
  وگرنه {
    pnow->tv_sec = time(پوچ);
    pnow->tv_usec = 0;
  }
#endif
}

#وگرنهاگر defined(HAVE_MACH_ABSOLUTE_TIME)

#شامل <mach/mach_time.h>

تهی curlx_pnow(ساختار curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  ایستا mach_timebase_info_data_t timebase;
  uint64_t usecs;

  اگر(timebase.denom == 0)
    (تهی)mach_timebase_info(&timebase);

  usecs = mach_absolute_time();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (صحیح)(usecs % 1000000);
}

#وگرنهاگر defined(HAVE_GETTIMEOFDAY)

تهی curlx_pnow(ساختار curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  ساختار timeval now;
  (تهی)gettimeofday(&now, پوچ);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (صحیح)now.tv_usec;
}

#وگرنه

تهی curlx_pnow(ساختار curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = time(پوچ);
  pnow->tv_usec = 0;
  اگر(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#endif

ساختار curltime curlx_now(تهی)
{
  ساختار curltime now;
  curlx_pnow(&now);
  بازگشت now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(ثابت ساختار curltime *newer,
                              ثابت ساختار curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  اگر(diff >= (TIMEDIFF_T_MAX / 1000))
    بازگشت TIMEDIFF_T_MAX;
  وگرنه اگر(diff <= (TIMEDIFF_T_MIN / 1000))
    بازگشت TIMEDIFF_T_MIN;
  بازگشت (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(ساختار curltime newer, ساختار curltime older)
{
  بازگشت curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(ساختار curltime newer,
                                  ساختار curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  اگر(diff >= (TIMEDIFF_T_MAX / 1000))
    بازگشت TIMEDIFF_T_MAX;
  وگرنه اگر(diff <= (TIMEDIFF_T_MIN / 1000))
    بازگشت TIMEDIFF_T_MIN;
  بازگشت (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_to_ceil_ms(timediff_t us)
{
  بازگشت (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(ثابت ساختار curltime *newer,
                              ثابت ساختار curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  اگر(diff >= (TIMEDIFF_T_MAX / 1000000))
    بازگشت TIMEDIFF_T_MAX;
  وگرنه اگر(diff <= (TIMEDIFF_T_MIN / 1000000))
    بازگشت TIMEDIFF_T_MIN;
  بازگشت (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(ساختار curltime newer, ساختار curltime older)
{
  بازگشت curlx_ptimediff_us(&newer, &older);
}

#اگر defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#شامل <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#ifdef _USE_32BIT_TIME_T
#define gmtime_s _gmtime32_s
#وگرنه
#define gmtime_s _gmtime64_s
#endif
#endif

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(time_t intime, ساختار tm *store)
{
#ifdef _WIN32
  اگر(gmtime_s(store, &intime)) /* thread-safe */
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
#وگرنهاگر defined(HAVE_GMTIME_R)
  ثابت ساختار tm *tm;
  tm = gmtime_r(&intime, store); /* thread-safe */
  اگر(!tm)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
#وگرنه
  ثابت ساختار tm *tm;
  /* !checksrc! disable BANNEDFUNC 1 */
  tm = gmtime(&intime); /* not thread-safe */
  اگر(tm)
    *store = *tm; /* copy the pointed struct to the local copy */
  وگرنه
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
#endif

  بازگشت CURLE_OK;
}
