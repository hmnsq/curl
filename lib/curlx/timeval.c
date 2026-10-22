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
#অন্তর্ভাৱ "curlx/timeval.h"

#ifdef _WIN32

স্থির LARGE_INTEGER s_time_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
শূন্য curlx_now_init(শূন্য)
{
  QueryPerformanceFrequency(&s_time_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
শূন্য curlx_pnow(সংরচনা curltime *pnow)
{
  LARGE_INTEGER গুনতি;
  DEBUGASSERT(s_time_freq.QuadPart);
  QueryPerformanceCounter(&গুনতি);
  pnow->tv_sec = (time_t)(গুনতি.QuadPart / s_time_freq.QuadPart);
  pnow->tv_usec = (পূর্ণাঙ্ক)((গুনতি.QuadPart % s_time_freq.QuadPart) * 1000000 /
                        s_time_freq.QuadPart);
}

#নোচেদ্যদি defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

শূন্য curlx_pnow(সংরচনা curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  সংরচনা timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#যদি defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
  bool have_clock_gettime = FALSE;
  যদি(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#endif

#ifdef HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  যদি(
#যদি defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (পূর্ণাঙ্ক)(tsnow.tv_nsec / 1000);
  }
  অন্যথা
#endif

  যদি(
#যদি defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (পূর্ণাঙ্ক)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#ifdef HAVE_GETTIMEOFDAY
  অন্যথা {
    সংরচনা timeval now;
    (শূন্য)gettimeofday(&now, রিক্ত);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (পূর্ণাঙ্ক)now.tv_usec;
  }
#অন্যথা
  অন্যথা {
    pnow->tv_sec = time(রিক্ত);
    pnow->tv_usec = 0;
  }
#endif
}

#নোচেদ্যদি defined(HAVE_MACH_ABSOLUTE_TIME)

#অন্তর্ভাৱ <mach/mach_time.h>

শূন্য curlx_pnow(সংরচনা curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  স্থির mach_timebase_info_data_t timebase;
  uint64_t usecs;

  যদি(timebase.denom == 0)
    (শূন্য)mach_timebase_info(&timebase);

  usecs = mach_absolute_time();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (পূর্ণাঙ্ক)(usecs % 1000000);
}

#নোচেদ্যদি defined(HAVE_GETTIMEOFDAY)

শূন্য curlx_pnow(সংরচনা curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  সংরচনা timeval now;
  (শূন্য)gettimeofday(&now, রিক্ত);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (পূর্ণাঙ্ক)now.tv_usec;
}

#অন্যথা

শূন্য curlx_pnow(সংরচনা curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = time(রিক্ত);
  pnow->tv_usec = 0;
  যদি(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#endif

সংরচনা curltime curlx_now(শূন্য)
{
  সংরচনা curltime now;
  curlx_pnow(&now);
  ঘূৰাই now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(ধ্রুৱ সংরচনা curltime *newer,
                              ধ্রুৱ সংরচনা curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  যদি(diff >= (TIMEDIFF_T_MAX / 1000))
    ঘূৰাই TIMEDIFF_T_MAX;
  অন্যথা যদি(diff <= (TIMEDIFF_T_MIN / 1000))
    ঘূৰাই TIMEDIFF_T_MIN;
  ঘূৰাই (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(সংরচনা curltime newer, সংরচনা curltime older)
{
  ঘূৰাই curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(সংরচনা curltime newer,
                                  সংরচনা curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  যদি(diff >= (TIMEDIFF_T_MAX / 1000))
    ঘূৰাই TIMEDIFF_T_MAX;
  অন্যথা যদি(diff <= (TIMEDIFF_T_MIN / 1000))
    ঘূৰাই TIMEDIFF_T_MIN;
  ঘূৰাই (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_to_ceil_ms(timediff_t us)
{
  ঘূৰাই (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(ধ্রুৱ সংরচনা curltime *newer,
                              ধ্রুৱ সংরচনা curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  যদি(diff >= (TIMEDIFF_T_MAX / 1000000))
    ঘূৰাই TIMEDIFF_T_MAX;
  অন্যথা যদি(diff <= (TIMEDIFF_T_MIN / 1000000))
    ঘূৰাই TIMEDIFF_T_MIN;
  ঘূৰাই (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(সংরচনা curltime newer, সংরচনা curltime older)
{
  ঘূৰাই curlx_ptimediff_us(&newer, &older);
}

#যদি defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#অন্তর্ভাৱ <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#ifdef _USE_32BIT_TIME_T
#define gmtime_s _gmtime32_s
#অন্যথা
#define gmtime_s _gmtime64_s
#endif
#endif

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(time_t intime, সংরচনা tm *store)
{
#ifdef _WIN32
  যদি(gmtime_s(store, &intime)) /* thread-safe */
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
#নোচেদ্যদি defined(HAVE_GMTIME_R)
  ধ্রুৱ সংরচনা tm *tm;
  tm = gmtime_r(&intime, store); /* thread-safe */
  যদি(!tm)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
#অন্যথা
  ধ্রুৱ সংরচনা tm *tm;
  /* !checksrc! disable BANNEDFUNC 1 */
  tm = gmtime(&intime); /* not thread-safe */
  যদি(tm)
    *store = *tm; /* copy the pointed struct to the local copy */
  অন্যথা
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
#endif

  ঘূৰাই CURLE_OK;
}
