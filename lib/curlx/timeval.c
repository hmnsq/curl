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
#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"

#ifdef _WIN32

ସ୍ଥିର LARGE_INTEGER s_time_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
ଶୂନ୍ୟ curlx_now_init(ଶୂନ୍ୟ)
{
  QueryPerformanceFrequency(&s_time_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
ଶୂନ୍ୟ curlx_pnow(ସଂରଚନା curltime *pnow)
{
  LARGE_INTEGER ଗଣନା_2;
  DEBUGASSERT(s_time_freq.QuadPart);
  QueryPerformanceCounter(&ଗଣନା_2);
  pnow->tv_sec = (time_t)(ଗଣନା_2.QuadPart / s_time_freq.QuadPart);
  pnow->tv_usec = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((ଗଣନା_2.QuadPart % s_time_freq.QuadPart) * 1000000 /
                        s_time_freq.QuadPart);
}

#ନଚେତଯଦି defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

ଶୂନ୍ୟ curlx_pnow(ସଂରଚନା curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  ସଂରଚନା timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#ଯଦି defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
  bool have_clock_gettime = FALSE;
  ଯଦି(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#endif

#ifdef HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  ଯଦି(
#ଯଦି defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tsnow.tv_nsec / 1000);
  }
  ଅନ୍ୟଥା
#endif

  ଯଦି(
#ଯଦି defined(__APPLE__) && defined(HAVE_BUILTIN_AVAILABLE) && \
  (HAVE_BUILTIN_AVAILABLE == 1)
    have_clock_gettime &&
#endif
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#ifdef HAVE_GETTIMEOFDAY
  ଅନ୍ୟଥା {
    ସଂରଚନା timeval now;
    (ଶୂନ୍ୟ)gettimeofday(&now, ଖାଲି);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)now.tv_usec;
  }
#ଅନ୍ୟଥା
  ଅନ୍ୟଥା {
    pnow->tv_sec = time(ଖାଲି);
    pnow->tv_usec = 0;
  }
#endif
}

#ନଚେତଯଦି defined(HAVE_MACH_ABSOLUTE_TIME)

#ଅନ୍ତର୍ଭାଵ <mach/mach_time.h>

ଶୂନ୍ୟ curlx_pnow(ସଂରଚନା curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  ସ୍ଥିର mach_timebase_info_data_t timebase;
  uint64_t usecs;

  ଯଦି(timebase.denom == 0)
    (ଶୂନ୍ୟ)mach_timebase_info(&timebase);

  usecs = mach_absolute_time();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(usecs % 1000000);
}

#ନଚେତଯଦି defined(HAVE_GETTIMEOFDAY)

ଶୂନ୍ୟ curlx_pnow(ସଂରଚନା curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  ସଂରଚନା timeval now;
  (ଶୂନ୍ୟ)gettimeofday(&now, ଖାଲି);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)now.tv_usec;
}

#ଅନ୍ୟଥା

ଶୂନ୍ୟ curlx_pnow(ସଂରଚନା curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = time(ଖାଲି);
  pnow->tv_usec = 0;
  ଯଦି(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#endif

ସଂରଚନା curltime curlx_now(ଶୂନ୍ୟ)
{
  ସଂରଚନା curltime now;
  curlx_pnow(&now);
  ଫେରନ୍ତୁ now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(ଅଚଳ ସଂରଚନା curltime *newer,
                              ଅଚଳ ସଂରଚନା curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ଯଦି(diff >= (TIMEDIFF_T_MAX / 1000))
    ଫେରନ୍ତୁ TIMEDIFF_T_MAX;
  ଅନ୍ୟଥା ଯଦି(diff <= (TIMEDIFF_T_MIN / 1000))
    ଫେରନ୍ତୁ TIMEDIFF_T_MIN;
  ଫେରନ୍ତୁ (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(ସଂରଚନା curltime newer, ସଂରଚନା curltime older)
{
  ଫେରନ୍ତୁ curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(ସଂରଚନା curltime newer,
                                  ସଂରଚନା curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  ଯଦି(diff >= (TIMEDIFF_T_MAX / 1000))
    ଫେରନ୍ତୁ TIMEDIFF_T_MAX;
  ଅନ୍ୟଥା ଯଦି(diff <= (TIMEDIFF_T_MIN / 1000))
    ଫେରନ୍ତୁ TIMEDIFF_T_MIN;
  ଫେରନ୍ତୁ (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_to_ceil_ms(timediff_t us)
{
  ଫେରନ୍ତୁ (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(ଅଚଳ ସଂରଚନା curltime *newer,
                              ଅଚଳ ସଂରଚନା curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  ଯଦି(diff >= (TIMEDIFF_T_MAX / 1000000))
    ଫେରନ୍ତୁ TIMEDIFF_T_MAX;
  ଅନ୍ୟଥା ଯଦି(diff <= (TIMEDIFF_T_MIN / 1000000))
    ଫେରନ୍ତୁ TIMEDIFF_T_MIN;
  ଫେରନ୍ତୁ (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(ସଂରଚନା curltime newer, ସଂରଚନା curltime older)
{
  ଫେରନ୍ତୁ curlx_ptimediff_us(&newer, &older);
}

#ଯଦି defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#ଅନ୍ତର୍ଭାଵ <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#ifdef _USE_32BIT_TIME_T
#define gmtime_s _gmtime32_s
#ଅନ୍ୟଥା
#define gmtime_s _gmtime64_s
#endif
#endif

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(time_t intime, ସଂରଚନା tm *store)
{
#ifdef _WIN32
  ଯଦି(gmtime_s(store, &intime)) /* thread-safe */
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
#ନଚେତଯଦି defined(HAVE_GMTIME_R)
  ଅଚଳ ସଂରଚନା tm *tm;
  tm = gmtime_r(&intime, store); /* thread-safe */
  ଯଦି(!tm)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
#ଅନ୍ୟଥା
  ଅଚଳ ସଂରଚନା tm *tm;
  /* !checksrc! disable BANNEDFUNC 1 */
  tm = gmtime(&intime); /* not thread-safe */
  ଯଦି(tm)
    *store = *tm; /* copy the pointed struct to the local copy */
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
#endif

  ଫେରନ୍ତୁ CURLE_OK;
}
