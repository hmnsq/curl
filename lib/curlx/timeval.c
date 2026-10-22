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
#అన్తర్భావ "curlx/timeval.h"

#నిర్వచితమైతే _WIN32

స్థిర LARGE_INTEGER s_సమయం_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
శూన్య curlx_now_init(శూన్య)
{
  QueryPerformanceFrequency(&s_సమయం_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
శూన్య curlx_pnow(నిర్మాణం curltime *pnow)
{
  LARGE_INTEGER లెక్క;
  DEBUGASSERT(s_సమయం_freq.QuadPart);
  QueryPerformanceCounter(&లెక్క);
  pnow->tv_sec = (time_t)(లెక్క.QuadPart / s_సమయం_freq.QuadPart);
  pnow->tv_usec = (పూర్ణాంకం)((లెక్క.QuadPart % s_సమయం_freq.QuadPart) * 1000000 /
                        s_సమయం_freq.QuadPart);
}

#లేదావేళ defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

శూన్య curlx_pnow(నిర్మాణం curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  నిర్మాణం timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#ఒకవేళ defined(__APPLE__) && defined(HAVE_BUILTIN_అందుబాటు) && \
  (HAVE_BUILTIN_అందుబాటు == 1)
  bool have_clock_gettime = FALSE;
  ఒకవేళ(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  ఒకవేళ(
#ఒకవేళ defined(__APPLE__) && defined(HAVE_BUILTIN_అందుబాటు) && \
  (HAVE_BUILTIN_అందుబాటు == 1)
    have_clock_gettime &&
#అయితే_ముగింపు
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (పూర్ణాంకం)(tsnow.tv_nsec / 1000);
  }
  లేకపోతే
#అయితే_ముగింపు

  ఒకవేళ(
#ఒకవేళ defined(__APPLE__) && defined(HAVE_BUILTIN_అందుబాటు) && \
  (HAVE_BUILTIN_అందుబాటు == 1)
    have_clock_gettime &&
#అయితే_ముగింపు
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (పూర్ణాంకం)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#నిర్వచితమైతే HAVE_GETTIMEOFDAY
  లేకపోతే {
    నిర్మాణం timeval now;
    (శూన్య)gettimeofday(&now, ఖాళీ);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (పూర్ణాంకం)now.tv_usec;
  }
#లేకపోతే
  లేకపోతే {
    pnow->tv_sec = సమయం(ఖాళీ);
    pnow->tv_usec = 0;
  }
#అయితే_ముగింపు
}

#లేదావేళ defined(HAVE_MACH_ABSOLUTE_సమయం)

#అన్తర్భావ <mach/mach_time.h>

శూన్య curlx_pnow(నిర్మాణం curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  స్థిర mach_timebase_info_దత్తాంశం_t timebase;
  uint64_t usecs;

  ఒకవేళ(timebase.denom == 0)
    (శూన్య)mach_timebase_info(&timebase);

  usecs = mach_absolute_సమయం();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (పూర్ణాంకం)(usecs % 1000000);
}

#లేదావేళ defined(HAVE_GETTIMEOFDAY)

శూన్య curlx_pnow(నిర్మాణం curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  నిర్మాణం timeval now;
  (శూన్య)gettimeofday(&now, ఖాళీ);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (పూర్ణాంకం)now.tv_usec;
}

#లేకపోతే

శూన్య curlx_pnow(నిర్మాణం curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = సమయం(ఖాళీ);
  pnow->tv_usec = 0;
  ఒకవేళ(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#అయితే_ముగింపు

నిర్మాణం curltime curlx_now(శూన్య)
{
  నిర్మాణం curltime now;
  curlx_pnow(&now);
  తిరిగి now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(స్థిరరాశి నిర్మాణం curltime *newer,
                              స్థిరరాశి నిర్మాణం curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ఒకవేళ(diff >= (TIMEDIFF_T_MAX / 1000))
    తిరిగి TIMEDIFF_T_MAX;
  లేకపోతే ఒకవేళ(diff <= (TIMEDIFF_T_MIN / 1000))
    తిరిగి TIMEDIFF_T_MIN;
  తిరిగి (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(నిర్మాణం curltime newer, నిర్మాణం curltime older)
{
  తిరిగి curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(నిర్మాణం curltime newer,
                                  నిర్మాణం curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  ఒకవేళ(diff >= (TIMEDIFF_T_MAX / 1000))
    తిరిగి TIMEDIFF_T_MAX;
  లేకపోతే ఒకవేళ(diff <= (TIMEDIFF_T_MIN / 1000))
    తిరిగి TIMEDIFF_T_MIN;
  తిరిగి (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_గమ్యం_ceil_ms(timediff_t us)
{
  తిరిగి (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(స్థిరరాశి నిర్మాణం curltime *newer,
                              స్థిరరాశి నిర్మాణం curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ఒకవేళ(diff >= (TIMEDIFF_T_MAX / 1000000))
    తిరిగి TIMEDIFF_T_MAX;
  లేకపోతే ఒకవేళ(diff <= (TIMEDIFF_T_MIN / 1000000))
    తిరిగి TIMEDIFF_T_MIN;
  తిరిగి (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(నిర్మాణం curltime newer, నిర్మాణం curltime older)
{
  తిరిగి curlx_ptimediff_us(&newer, &older);
}

#ఒకవేళ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#అన్తర్భావ <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#నిర్వచితమైతే USE_32_బిట్టు_సమయం_T
#నిర్వచించు gmtime_s _gmtime32_s
#లేకపోతే
#నిర్వచించు gmtime_s _gmtime64_s
#అయితే_ముగింపు
#అయితే_ముగింపు

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(time_t intime, నిర్మాణం tm *store)
{
#నిర్వచితమైతే _WIN32
  ఒకవేళ(gmtime_s(store, &intime)) /* thread-safe */
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
#లేదావేళ defined(HAVE_GMTIME_R)
  స్థిరరాశి నిర్మాణం tm *tm;
  tm = gmtime_r(&intime, store); /* thread-safe */
  ఒకవేళ(!tm)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
#లేకపోతే
  స్థిరరాశి నిర్మాణం tm *tm;
  /* !checksrc! disable BANNEDFUNC 1 */
  tm = gmtime(&intime); /* not thread-safe */
  ఒకవేళ(tm)
    *store = *tm; /* copy the pointed struct to the local copy */
  లేకపోతే
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
#అయితే_ముగింపు

  తిరిగి CURLE_OK;
}
