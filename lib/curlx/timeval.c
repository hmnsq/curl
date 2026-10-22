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

#اگر_تعریف_شدہ _WIN32

ساکن LARGE_INTEGER s_وقت_freq;

/* For tool or tests, we must initialize before calling curlx_now().
   Providing this function here is wrong. */
خالی curlx_now_init(خالی)
{
  QueryPerformanceFrequency(&s_وقت_freq);
}

/* In case of bug fix this function has a counterpart in tool_util.c */
خالی curlx_pnow(ساخت curltime *pnow)
{
  LARGE_INTEGER گنتی;
  DEBUGASSERT(s_وقت_freq.QuadPart);
  Query_Performance_شمار(&گنتی);
  pnow->tv_sec = (time_t)(گنتی.QuadPart / s_وقت_freq.QuadPart);
  pnow->tv_usec = (صحیح)((گنتی.QuadPart % s_وقت_freq.QuadPart) * 1000000 /
                        s_وقت_freq.QuadPart);
}

#وگرنہ_اگر defined(HAVE_CLOCK_GETTIME_MONOTONIC) || \
  defined(HAVE_CLOCK_GETTIME_MONOTONIC_RAW)

خالی curlx_pnow(ساخت curltime *pnow)
{
  /*
   * clock_gettime() is granted to be increased monotonically when the
   * monotonic clock is queried. Time starting point is unspecified, it
   * could be the system start-up time, the Epoch, or something else,
   * in any case the time starting point does not change once that the
   * system has started up.
   */
  ساخت timespec tsnow;

  /*
   * clock_gettime() may be defined by Apple's SDK as weak symbol thus
   * code compiles but fails during runtime if clock_gettime() is
   * called on unsupported OS version.
   */
#اگر defined(__APPLE__) && defined(HAVE_BUILTIN_دستیاب) && \
  (HAVE_BUILTIN_دستیاب == 1)
  bool have_clock_gettime = FALSE;
  اگر(__builtin_available(macOS 10.12, iOS 10, tvOS 10, watchOS 3, *))
    have_clock_gettime = TRUE;
#اگر_ختم

#اگر_تعریف_شدہ HAVE_CLOCK_GETTIME_MONOTONIC_RAW
  اگر(
#اگر defined(__APPLE__) && defined(HAVE_BUILTIN_دستیاب) && \
  (HAVE_BUILTIN_دستیاب == 1)
    have_clock_gettime &&
#اگر_ختم
    (clock_gettime(CLOCK_MONOTONIC_RAW, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (صحیح)(tsnow.tv_nsec / 1000);
  }
  ورنہ
#اگر_ختم

  اگر(
#اگر defined(__APPLE__) && defined(HAVE_BUILTIN_دستیاب) && \
  (HAVE_BUILTIN_دستیاب == 1)
    have_clock_gettime &&
#اگر_ختم
    (clock_gettime(CLOCK_MONOTONIC, &tsnow) == 0)) {
    pnow->tv_sec = tsnow.tv_sec;
    pnow->tv_usec = (صحیح)(tsnow.tv_nsec / 1000);
  }
  /*
   * Even when the configure process has truly detected monotonic clock
   * availability, it might happen that it is not actually available at
   * runtime. When this occurs, fallback to other time source.
   */
#اگر_تعریف_شدہ HAVE_GETTIMEOFDAY
  ورنہ {
    ساخت timeval now;
    (خالی)gettimeofday(&now, تہی);
    pnow->tv_sec = now.tv_sec;
    pnow->tv_usec = (صحیح)now.tv_usec;
  }
#ورنہ
  ورنہ {
    pnow->tv_sec = وقت(تہی);
    pnow->tv_usec = 0;
  }
#اگر_ختم
}

#وگرنہ_اگر defined(HAVE_MACH_ABSOLUTE_وقت)

#شامل <mach/mach_time.h>

خالی curlx_pnow(ساخت curltime *pnow)
{
  /*
   * Monotonic timer on macOS is provided by mach_absolute_time(), which
   * returns time in Mach "absolute time units," which are platform-dependent.
   * To convert to nanoseconds, one must use conversion factors specified by
   * mach_timebase_info().
   */
  ساکن mach_timebase_info_ڈیٹا_t timebase;
  uint64_t usecs;

  اگر(timebase.denom == 0)
    (خالی)mach_timebase_info(&timebase);

  usecs = mach_absolute_وقت();
  usecs *= timebase.numer; /* spellchecker:disable-line */
  usecs /= timebase.denom;
  usecs /= 1000;

  pnow->tv_sec = usecs / 1000000;
  pnow->tv_usec = (صحیح)(usecs % 1000000);
}

#وگرنہ_اگر defined(HAVE_GETTIMEOFDAY)

خالی curlx_pnow(ساخت curltime *pnow)
{
  /*
   * gettimeofday() is not granted to be increased monotonically, due to
   * clock drifting and external source time synchronization it can jump
   * forward or backward in time.
   */
  ساخت timeval now;
  (خالی)gettimeofday(&now, تہی);
  pnow->tv_sec = now.tv_sec;
  pnow->tv_usec = (صحیح)now.tv_usec;
}

#ورنہ

خالی curlx_pnow(ساخت curltime *pnow)
{
  /*
   * time() returns the value of time in seconds since the Epoch.
   */
  pnow->tv_sec = وقت(تہی);
  pnow->tv_usec = 0;
  اگر(!pnow->tv_sec) /* avoid a `now` fully zero */
    pnow->tv_usec = 1;
}

#اگر_ختم

ساخت curltime curlx_now(خالی)
{
  ساخت curltime now;
  curlx_pnow(&now);
  واپس now;
}

/*
 * Returns: time difference in number of milliseconds. For too large diffs it
 * returns max value.
 *
 * @unittest: 1323
 */
timediff_t curlx_ptimediff_ms(ثابت ساخت curltime *newer,
                              ثابت ساخت curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  اگر(diff >= (TIMEDIFF_T_MAX / 1000))
    واپس TIMEDIFF_T_MAX;
  ورنہ اگر(diff <= (TIMEDIFF_T_MIN / 1000))
    واپس TIMEDIFF_T_MIN;
  واپس (diff * 1000) + ((newer->tv_usec - older->tv_usec) / 1000);
}

timediff_t curlx_timediff_ms(ساخت curltime newer, ساخت curltime older)
{
  واپس curlx_ptimediff_ms(&newer, &older);
}

/*
 * Returns: time difference in number of milliseconds, rounded up.
 * For too large diffs it returns max value.
 */
timediff_t curlx_timediff_ceil_ms(ساخت curltime newer,
                                  ساخت curltime older)
{
  timediff_t diff = (timediff_t)newer.tv_sec - older.tv_sec;
  اگر(diff >= (TIMEDIFF_T_MAX / 1000))
    واپس TIMEDIFF_T_MAX;
  ورنہ اگر(diff <= (TIMEDIFF_T_MIN / 1000))
    واپس TIMEDIFF_T_MIN;
  واپس (diff * 1000) + ((newer.tv_usec - older.tv_usec + 999) / 1000);
}

timediff_t curlx_us_مقصد_ceil_ms(timediff_t us)
{
  واپس (us / 1000) + ((us > 0) && (us % 1000));
}

/*
 * Returns: time difference in number of microseconds. For too large diffs it
 * returns max value.
 */
timediff_t curlx_ptimediff_us(ثابت ساخت curltime *newer,
                              ثابت ساخت curltime *older)
{
  timediff_t diff = (timediff_t)newer->tv_sec - older->tv_sec;
  اگر(diff >= (TIMEDIFF_T_MAX / 1000000))
    واپس TIMEDIFF_T_MAX;
  ورنہ اگر(diff <= (TIMEDIFF_T_MIN / 1000000))
    واپس TIMEDIFF_T_MIN;
  واپس (diff * 1000000) + newer->tv_usec - older->tv_usec;
}

timediff_t curlx_timediff_us(ساخت curltime newer, ساخت curltime older)
{
  واپس curlx_ptimediff_us(&newer, &older);
}

#اگر defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 3)
#شامل <sec_api/time_s.h>  /* for _gmtime32_s(), _gmtime64_s() */
#اگر_تعریف_شدہ USE_32_ثنائی_ہندسہ_وقت_T
#تعریف_کرو gmtime_s _gmtime32_s
#ورنہ
#تعریف_کرو gmtime_s _gmtime64_s
#اگر_ختم
#اگر_ختم

/*
 * curlx_gmtime() is a gmtime() replacement for portability. Do not use
 * the gmtime_s(), gmtime_r() or gmtime() functions anywhere else but here.
 */
CURLcode curlx_gmtime(time_t intime, ساخت tm *store)
{
#اگر_تعریف_شدہ _WIN32
  اگر(gmtime_s(store, &intime)) /* thread-safe */
    واپس CURLE_BAD_FUNCTION_دلیل;
#وگرنہ_اگر defined(HAVE_GMTIME_R)
  ثابت ساخت tm *tm;
  tm = gmtime_r(&intime, store); /* thread-safe */
  اگر(!tm)
    واپس CURLE_BAD_FUNCTION_دلیل;
#ورنہ
  ثابت ساخت tm *tm;
  /* !checksrc! disable BANNEDFUNC 1 */
  tm = gmtime(&intime); /* not thread-safe */
  اگر(tm)
    *store = *tm; /* copy the pointed struct to the local copy */
  ورنہ
    واپس CURLE_BAD_FUNCTION_دلیل;
#اگر_ختم

  واپس CURLE_OK;
}
