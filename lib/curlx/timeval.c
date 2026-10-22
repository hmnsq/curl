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
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

#ifdef _WIN32

ಸ್ಥಿರ LARGE_INTEGER s_time_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
ಶೂನ್ಯ curlx_now_init(ಶೂನ್ಯ)
{
  QueryPerformanceFrequency(&s_time_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
ಶೂನ್ಯ curlx_pnow(ಸಂರಚನಾ curltime *pnow)
{
  LARGE_INTEGER ಎಣಿಕೆ;
  DEBUGASSERT(s_time_freq.QuadPart);
  QueryPerformanceCounter(&ಎಣಿಕೆ);
  pnow->tv_sec = (time_t)(ಎಣಿಕೆ.QuadPart / s_time_freq.QuadPart);
  pnow->tv_usec = (ಪೂರ್ಣಾಙ್ಕ)((ಎಣಿಕೆ.QuadPart % s_time_freq.QuadPart) * 1000000 /
                        s_time_freq.QuadPart);
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

ಶೂನ್ಯ curlx_pnow(ಸಂರಚನಾ curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  ಸಂರಚನಾ timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#ಯದಿ defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
  bool have_clock_gettime = FALSE;
  ಯದಿ(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#endif

#ifdef HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  ಯದಿ(
#ಯದಿ defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (ಪೂರ್ಣಾಙ್ಕ)(tsnow.tv_nsec / 1000);
  }
  ಅನ್ಯಥಾ
#endif

  ಯದಿ(
#ಯದಿ defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (ಪೂರ್ಣಾಙ್ಕ)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#ifdef HAVE_GETTIMEOFDAY
  ಅನ್ಯಥಾ {
    ಸಂರಚನಾ timeval now;
    (ಶೂನ್ಯ)gettimeofday(&now, ರಿಕ್ತ);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (ಪೂರ್ಣಾಙ್ಕ)now.tv_usec;
  }
#ಅನ್ಯಥಾ
  ಅನ್ಯಥಾ {
    pnow->tv_sec = time(ರಿಕ್ತ);
    pnow->tv_usec = 0;
  }
#endif
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_MACH_ABSOLUTE_TIME)

#ಅನ್ತರ್ಭಾವ <mach/mach_time.h>

ಶೂನ್ಯ curlx_pnow(ಸಂರಚನಾ curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  ಸ್ಥಿರ mach_timebase_info_data_t timebase;
  uint64_t usecs;

  ಯದಿ(timebase.denom == 0)
    (ಶೂನ್ಯ)mach_timebase_info(&timebase);

  usecs = mach_absolute_time();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (ಪೂರ್ಣಾಙ್ಕ)(usecs % 1000000);
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_GETTIMEOFDAY)

ಶೂನ್ಯ curlx_pnow(ಸಂರಚನಾ curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  ಸಂರಚನಾ timeval now;
  (ಶೂನ್ಯ)gettimeofday(&now, ರಿಕ್ತ);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (ಪೂರ್ಣಾಙ್ಕ)now.tv_usec;
}

#ಅನ್ಯಥಾ

ಶೂನ್ಯ curlx_pnow(ಸಂರಚನಾ curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = time(ರಿಕ್ತ);
  pnow->tv_usec = 0;
  ಯದಿ(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#endif

ಸಂರಚನಾ curltime curlx_now(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ curltime now;
  curlx_pnow(&now);
  ಪ್ರತ್ಯಾಗಮ now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(ಧ್ರುವ ಸಂರಚನಾ curltime *newer,
                              ಧ್ರುವ ಸಂರಚನಾ curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ಯದಿ(diff >= (TIMEDIFF_T_MAX / 1000))
    ಪ್ರತ್ಯಾಗಮ TIMEDIFF_T_MAX;
  ಅನ್ಯಥಾ ಯದಿ(diff <= (TIMEDIFF_T_MIN / 1000))
    ಪ್ರತ್ಯಾಗಮ TIMEDIFF_T_MIN;
  ಪ್ರತ್ಯಾಗಮ (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(ಸಂರಚನಾ curltime newer, ಸಂರಚನಾ curltime older)
{
  ಪ್ರತ್ಯಾಗಮ curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(ಸಂರಚನಾ curltime newer,
                                  ಸಂರಚನಾ curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  ಯದಿ(diff >= (TIMEDIFF_T_MAX / 1000))
    ಪ್ರತ್ಯಾಗಮ TIMEDIFF_T_MAX;
  ಅನ್ಯಥಾ ಯದಿ(diff <= (TIMEDIFF_T_MIN / 1000))
    ಪ್ರತ್ಯಾಗಮ TIMEDIFF_T_MIN;
  ಪ್ರತ್ಯಾಗಮ (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_to_ceil_ms(timediff_t us)
{
  ಪ್ರತ್ಯಾಗಮ (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(ಧ್ರುವ ಸಂರಚನಾ curltime *newer,
                              ಧ್ರುವ ಸಂರಚನಾ curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ಯದಿ(diff >= (TIMEDIFF_T_MAX / 1000000))
    ಪ್ರತ್ಯಾಗಮ TIMEDIFF_T_MAX;
  ಅನ್ಯಥಾ ಯದಿ(diff <= (TIMEDIFF_T_MIN / 1000000))
    ಪ್ರತ್ಯಾಗಮ TIMEDIFF_T_MIN;
  ಪ್ರತ್ಯಾಗಮ (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(ಸಂರಚನಾ curltime newer, ಸಂರಚನಾ curltime older)
{
  ಪ್ರತ್ಯಾಗಮ curlx_ptimediff_us(&newer, &older);
}

#ಯದಿ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#ಅನ್ತರ್ಭಾವ <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#ifdef _USE_32BIT_TIME_T
#define gmtime_s _gmtime32_s
#ಅನ್ಯಥಾ
#define gmtime_s _gmtime64_s
#endif
#endif

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(time_t intime, ಸಂರಚನಾ tm *store)
{
#ifdef _WIN32
  ಯದಿ(gmtime_s(store, &intime)) /* thread-safe */
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
#ನೋಚೇದ್ಯದಿ defined(HAVE_GMTIME_R)
  ಧ್ರುವ ಸಂರಚನಾ tm *tm;
  tm = gmtime_r(&intime, store); /* thread-safe */
  ಯದಿ(!tm)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
#ಅನ್ಯಥಾ
  ಧ್ರುವ ಸಂರಚನಾ tm *tm;
  /* !checksrc! disable BANNEDFUNC 1 */
  tm = gmtime(&intime); /* not thread-safe */
  ಯದಿ(tm)
    *store = *tm; /* copy the pointed struct to the local copy */
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
#endif

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
