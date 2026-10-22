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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

ಸ್ಥಿರ LARGE_INTEGER s_ಸಮಯ_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
ಶೂನ್ಯ curlx_now_init(ಶೂನ್ಯ)
{
  QueryPerformanceFrequency(&s_ಸಮಯ_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
ಶೂನ್ಯ curlx_pnow(ರಚನೆ curltime *pnow)
{
  LARGE_INTEGER ಎಣಿಕೆ_2;
  DEBUGASSERT(s_ಸಮಯ_freq.QuadPart);
  QueryPerformanceCounter(&ಎಣಿಕೆ_2);
  pnow->tv_sec = (time_t)(ಎಣಿಕೆ_2.QuadPart / s_ಸಮಯ_freq.QuadPart);
  pnow->tv_usec = (ಪೂರ್ಣಾಂಕ)((ಎಣಿಕೆ_2.QuadPart % s_ಸಮಯ_freq.QuadPart) * 1000000 /
                        s_ಸಮಯ_freq.QuadPart);
}

#ಅಥವಾಆದರೆ defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

ಶೂನ್ಯ curlx_pnow(ರಚನೆ curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  ರಚನೆ timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#ಆದರೆ defined(__APPLE__) && defined(HAVE_BUILTIN_ಲಭ್ಯ) && \
  (HAVE_BUILTIN_ಲಭ್ಯ == 1)
  bool have_clock_gettime = FALSE;
  ಆದರೆ(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  ಆದರೆ(
#ಆದರೆ defined(__APPLE__) && defined(HAVE_BUILTIN_ಲಭ್ಯ) && \
  (HAVE_BUILTIN_ಲಭ್ಯ == 1)
    have_clock_gettime &&
#ಆದರೆ_ಅಂತ್ಯ
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (ಪೂರ್ಣಾಂಕ)(tsnow.tv_nsec / 1000);
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(
#ಆದರೆ defined(__APPLE__) && defined(HAVE_BUILTIN_ಲಭ್ಯ) && \
  (HAVE_BUILTIN_ಲಭ್ಯ == 1)
    have_clock_gettime &&
#ಆದರೆ_ಅಂತ್ಯ
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (ಪೂರ್ಣಾಂಕ)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETTIMEOFDAY
  ಇಲ್ಲದಿದ್ದರೆ {
    ರಚನೆ timeval now;
    (ಶೂನ್ಯ)gettimeofday(&now, ಖಾಲಿ);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (ಪೂರ್ಣಾಂಕ)now.tv_usec;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  ಇಲ್ಲದಿದ್ದರೆ {
    pnow->tv_sec = ಸಮಯ(ಖಾಲಿ);
    pnow->tv_usec = 0;
  }
#ಆದರೆ_ಅಂತ್ಯ
}

#ಅಥವಾಆದರೆ defined(HAVE_MACH_ABSOLUTE_ಸಮಯ)

#ಅನ್ತರ್ಭಾವ <mach/mach_time.h>

ಶೂನ್ಯ curlx_pnow(ರಚನೆ curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  ಸ್ಥಿರ mach_timebase_info_ದತ್ತಾಂಶ_t timebase;
  uint64_t usecs;

  ಆದರೆ(timebase.denom == 0)
    (ಶೂನ್ಯ)mach_timebase_info(&timebase);

  usecs = mach_absolute_ಸಮಯ();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (ಪೂರ್ಣಾಂಕ)(usecs % 1000000);
}

#ಅಥವಾಆದರೆ defined(HAVE_GETTIMEOFDAY)

ಶೂನ್ಯ curlx_pnow(ರಚನೆ curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  ರಚನೆ timeval now;
  (ಶೂನ್ಯ)gettimeofday(&now, ಖಾಲಿ);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (ಪೂರ್ಣಾಂಕ)now.tv_usec;
}

#ಇಲ್ಲದಿದ್ದರೆ

ಶೂನ್ಯ curlx_pnow(ರಚನೆ curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = ಸಮಯ(ಖಾಲಿ);
  pnow->tv_usec = 0;
  ಆದರೆ(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ curltime curlx_now(ಶೂನ್ಯ)
{
  ರಚನೆ curltime now;
  curlx_pnow(&now);
  ಹಿಂತಿರುಗಿ now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *newer,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ಆದರೆ(diff >= (TIMEDIFF_T_MAX / 1000))
    ಹಿಂತಿರುಗಿ TIMEDIFF_T_MAX;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(diff <= (TIMEDIFF_T_MIN / 1000))
    ಹಿಂತಿರುಗಿ TIMEDIFF_T_MIN;
  ಹಿಂತಿರುಗಿ (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(ರಚನೆ curltime newer, ರಚನೆ curltime older)
{
  ಹಿಂತಿರುಗಿ curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(ರಚನೆ curltime newer,
                                  ರಚನೆ curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  ಆದರೆ(diff >= (TIMEDIFF_T_MAX / 1000))
    ಹಿಂತಿರುಗಿ TIMEDIFF_T_MAX;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(diff <= (TIMEDIFF_T_MIN / 1000))
    ಹಿಂತಿರುಗಿ TIMEDIFF_T_MIN;
  ಹಿಂತಿರುಗಿ (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_ಗಮ್ಯ_ceil_ms(timediff_t us)
{
  ಹಿಂತಿರುಗಿ (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *newer,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ಆದರೆ(diff >= (TIMEDIFF_T_MAX / 1000000))
    ಹಿಂತಿರುಗಿ TIMEDIFF_T_MAX;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(diff <= (TIMEDIFF_T_MIN / 1000000))
    ಹಿಂತಿರುಗಿ TIMEDIFF_T_MIN;
  ಹಿಂತಿರುಗಿ (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(ರಚನೆ curltime newer, ರಚನೆ curltime older)
{
  ಹಿಂತಿರುಗಿ curlx_ptimediff_us(&newer, &older);
}

#ಆದರೆ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#ಅನ್ತರ್ಭಾವ <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_32_ಬಿಟ್_ಸಮಯ_T
#ವ್ಯಾಖ್ಯಾನಿಸು gmtime_s _gmtime32_s
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು gmtime_s _gmtime64_s
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(time_t intime, ರಚನೆ tm *store)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  ಆದರೆ(gmtime_s(store, &intime)) /* thread-safe */
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
#ಅಥವಾಆದರೆ defined(HAVE_GMTIME_R)
  ಸ್ಥಿರಾಂಕ ರಚನೆ tm *tm;
  tm = gmtime_r(&intime, store); /* thread-safe */
  ಆದರೆ(!tm)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ ರಚನೆ tm *tm;
  /* !checksrc! disable BANNEDFUNC 1 */
  tm = gmtime(&intime); /* not thread-safe */
  ಆದರೆ(tm)
    *store = *tm; /* copy the pointed struct to the local copy */
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ CURLE_OK;
}
