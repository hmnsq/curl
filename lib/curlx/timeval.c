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
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

ਸਥਿਰ LARGE_INTEGER s_ਸਮਾਂ_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
ਖਾਲੀ curlx_now_init(ਖਾਲੀ)
{
  QueryPerformanceFrequency(&s_ਸਮਾਂ_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
ਖਾਲੀ curlx_pnow(ਬਣਤਰ curltime *pnow)
{
  LARGE_INTEGER ਗਿਣਤੀ_2;
  DEBUGASSERT(s_ਸਮਾਂ_freq.QuadPart);
  QueryPerformanceCounter(&ਗਿਣਤੀ_2);
  pnow->tv_sec = (time_t)(ਗਿਣਤੀ_2.QuadPart / s_ਸਮਾਂ_freq.QuadPart);
  pnow->tv_usec = (ਪੂਰਨ)((ਗਿਣਤੀ_2.QuadPart % s_ਸਮਾਂ_freq.QuadPart) * 1000000 /
                        s_ਸਮਾਂ_freq.QuadPart);
}

#ਜਾਂਜੇ defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

ਖਾਲੀ curlx_pnow(ਬਣਤਰ curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  ਬਣਤਰ timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#ਜੇ defined(__APPLE__) && defined(HAVE_BUILTIN_ਉਪਲਬਧ) && \
  (HAVE_BUILTIN_ਉਪਲਬਧ == 1)
  bool have_clock_gettime = FALSE;
  ਜੇ(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  ਜੇ(
#ਜੇ defined(__APPLE__) && defined(HAVE_BUILTIN_ਉਪਲਬਧ) && \
  (HAVE_BUILTIN_ਉਪਲਬਧ == 1)
    have_clock_gettime &&
#ਜੇ_ਅੰਤ
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (ਪੂਰਨ)(tsnow.tv_nsec / 1000);
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ

  ਜੇ(
#ਜੇ defined(__APPLE__) && defined(HAVE_BUILTIN_ਉਪਲਬਧ) && \
  (HAVE_BUILTIN_ਉਪਲਬਧ == 1)
    have_clock_gettime &&
#ਜੇ_ਅੰਤ
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (ਪੂਰਨ)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETTIMEOFDAY
  ਨਹੀਂਤਾਂ {
    ਬਣਤਰ timeval now;
    (ਖਾਲੀ)gettimeofday(&now, ਨਿਰਾ);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (ਪੂਰਨ)now.tv_usec;
  }
#ਨਹੀਂਤਾਂ
  ਨਹੀਂਤਾਂ {
    pnow->tv_sec = ਸਮਾਂ(ਨਿਰਾ);
    pnow->tv_usec = 0;
  }
#ਜੇ_ਅੰਤ
}

#ਜਾਂਜੇ defined(HAVE_MACH_ABSOLUTE_ਸਮਾਂ)

#ਅਨ੍ਤਰ੍ਭਾਵ <mach/mach_time.h>

ਖਾਲੀ curlx_pnow(ਬਣਤਰ curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  ਸਥਿਰ mach_timebase_info_ਅੰਕੜੇ_t timebase;
  uint64_t usecs;

  ਜੇ(timebase.denom == 0)
    (ਖਾਲੀ)mach_timebase_info(&timebase);

  usecs = mach_absolute_ਸਮਾਂ();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (ਪੂਰਨ)(usecs % 1000000);
}

#ਜਾਂਜੇ defined(HAVE_GETTIMEOFDAY)

ਖਾਲੀ curlx_pnow(ਬਣਤਰ curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  ਬਣਤਰ timeval now;
  (ਖਾਲੀ)gettimeofday(&now, ਨਿਰਾ);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (ਪੂਰਨ)now.tv_usec;
}

#ਨਹੀਂਤਾਂ

ਖਾਲੀ curlx_pnow(ਬਣਤਰ curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = ਸਮਾਂ(ਨਿਰਾ);
  pnow->tv_usec = 0;
  ਜੇ(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#ਜੇ_ਅੰਤ

ਬਣਤਰ curltime curlx_now(ਖਾਲੀ)
{
  ਬਣਤਰ curltime now;
  curlx_pnow(&now);
  ਵਾਪਸ now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(ਅਚਲ ਬਣਤਰ curltime *newer,
                              ਅਚਲ ਬਣਤਰ curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ਜੇ(diff >= (TIMEDIFF_T_MAX / 1000))
    ਵਾਪਸ TIMEDIFF_T_MAX;
  ਨਹੀਂਤਾਂ ਜੇ(diff <= (TIMEDIFF_T_MIN / 1000))
    ਵਾਪਸ TIMEDIFF_T_MIN;
  ਵਾਪਸ (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(ਬਣਤਰ curltime newer, ਬਣਤਰ curltime older)
{
  ਵਾਪਸ curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(ਬਣਤਰ curltime newer,
                                  ਬਣਤਰ curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  ਜੇ(diff >= (TIMEDIFF_T_MAX / 1000))
    ਵਾਪਸ TIMEDIFF_T_MAX;
  ਨਹੀਂਤਾਂ ਜੇ(diff <= (TIMEDIFF_T_MIN / 1000))
    ਵਾਪਸ TIMEDIFF_T_MIN;
  ਵਾਪਸ (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_ਮੰਜ਼ਿਲ_ceil_ms(timediff_t us)
{
  ਵਾਪਸ (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(ਅਚਲ ਬਣਤਰ curltime *newer,
                              ਅਚਲ ਬਣਤਰ curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ਜੇ(diff >= (TIMEDIFF_T_MAX / 1000000))
    ਵਾਪਸ TIMEDIFF_T_MAX;
  ਨਹੀਂਤਾਂ ਜੇ(diff <= (TIMEDIFF_T_MIN / 1000000))
    ਵਾਪਸ TIMEDIFF_T_MIN;
  ਵਾਪਸ (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(ਬਣਤਰ curltime newer, ਬਣਤਰ curltime older)
{
  ਵਾਪਸ curlx_ptimediff_us(&newer, &older);
}

#ਜੇ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#ਅਨ੍ਤਰ੍ਭਾਵ <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_32_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸਮਾਂ_T
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ gmtime_s _gmtime32_s
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ gmtime_s _gmtime64_s
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(time_t intime, ਬਣਤਰ tm *store)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  ਜੇ(gmtime_s(store, &intime)) /* thread-safe */
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
#ਜਾਂਜੇ defined(HAVE_GMTIME_R)
  ਅਚਲ ਬਣਤਰ tm *tm;
  tm = gmtime_r(&intime, store); /* thread-safe */
  ਜੇ(!tm)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
#ਨਹੀਂਤਾਂ
  ਅਚਲ ਬਣਤਰ tm *tm;
  /* !checksrc! disable BANNEDFUNC 1 */
  tm = gmtime(&intime); /* not thread-safe */
  ਜੇ(tm)
    *store = *tm; /* copy the pointed struct to the local copy */
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
#ਜੇ_ਅੰਤ

  ਵਾਪਸ CURLE_OK;
}
