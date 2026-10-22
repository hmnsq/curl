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
#شامل "curl_setup.h"
#شامل "curl_threads.h"
#شامل "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(تهی *), تهی *arg)
{
  curl_thread_t t = CreateThread(پوچ, 0, func, arg, 0, پوچ);
  اگر(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    بازگشت curl_thread_t_null;
  }
  بازگشت t;
}

تهی Curl_thread_destroy(curl_thread_t *hnd)
{
  اگر(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

صحیح Curl_thread_join(curl_thread_t *hnd)
{
  صحیح ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  بازگشت ret;
}

#وگرنهاگر defined(HAVE_THREADS_POSIX)

ساختار Curl_actual_call {
  unsigned صحیح (*func)(تهی *);
  تهی *arg;
};

ایستا تهی *curl_thread_create_thunk(تهی *arg)
{
  ساختار Curl_actual_call *ac = arg;
  unsigned صحیح (*func)(تهی *) = ac->func;
  تهی *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  بازگشت 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(تهی *), تهی *arg)
{
  curl_thread_t t = curlx_malloc(اندازه(pthread_t));
  ساختار Curl_actual_call *ac = پوچ;
  صحیح rc;

  اگر(t)
    ac = curlx_malloc(اندازه(ساختار Curl_actual_call));
  اگر(!(ac && t))
    برو err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, پوچ, curl_thread_create_thunk, ac);
  اگر(rc) {
    errno = rc;
    برو err;
  }

  بازگشت t;

err:
  curlx_free(t);
  curlx_free(ac);
  بازگشت curl_thread_t_null;
}

تهی Curl_thread_destroy(curl_thread_t *hnd)
{
  اگر(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

صحیح Curl_thread_join(curl_thread_t *hnd)
{
  صحیح ret = (pthread_join(**hnd, پوچ) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  بازگشت ret;
}

#وگرنه
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

تهی Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

تهی Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  اگر(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    بازگشت (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  بازگشت CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(تهی)
{
  بازگشت GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  بازگشت tid == GetCurrentThreadId();
}

#وگرنهاگر defined(HAVE_THREADS_POSIX)

تهی Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (تهی)pthread_cond_signal(c);
}

تهی Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (تهی)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  ساختار curltime now;
  ساختار timespec ts;
  timediff_t usec;
  صحیح rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  ساختار timeval tv;
  (تهی)gettimeofday(&tv, پوچ);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (صحیح)tv.tv_usec;
#وگرنه
  now.tv_sec = time(پوچ);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  اگر(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  اگر(rc == SOCKETIMEDOUT)
    بازگشت CURLE_OPERATION_TIMEDOUT;
  بازگشت rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(تهی)
{
  بازگشت pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  بازگشت !!pthread_equal(tid, pthread_self());
}

#وگرنه
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
