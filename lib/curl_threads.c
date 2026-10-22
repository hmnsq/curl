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
#ضمن "curl_setup.h"
#ضمن "curl_threads.h"
#ضمن "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(فراغ *), فراغ *arg)
{
  curl_thread_t t = CreateThread(عدم, 0, func, arg, 0, عدم);
  ܐܢ(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    ܦܢܐ curl_thread_t_null;
  }
  ܦܢܐ t;
}

فراغ Curl_thread_destroy(curl_thread_t *hnd)
{
  ܐܢ(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

صحيح Curl_thread_join(curl_thread_t *hnd)
{
  صحيح ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  ܦܢܐ ret;
}

#وإلاإذا defined(HAVE_THREADS_POSIX)

بنية Curl_actual_call {
  unsigned صحيح (*func)(فراغ *);
  فراغ *arg;
};

ساكن فراغ *curl_thread_create_thunk(فراغ *arg)
{
  بنية Curl_actual_call *ac = arg;
  unsigned صحيح (*func)(فراغ *) = ac->func;
  فراغ *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  ܦܢܐ 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(فراغ *), فراغ *arg)
{
  curl_thread_t t = curlx_malloc(حجم(pthread_t));
  بنية Curl_actual_call *ac = عدم;
  صحيح rc;

  ܐܢ(t)
    ac = curlx_malloc(حجم(بنية Curl_actual_call));
  ܐܢ(!(ac && t))
    اذهب err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, عدم, curl_thread_create_thunk, ac);
  ܐܢ(rc) {
    errno = rc;
    اذهب err;
  }

  ܦܢܐ t;

err:
  curlx_free(t);
  curlx_free(ac);
  ܦܢܐ curl_thread_t_null;
}

فراغ Curl_thread_destroy(curl_thread_t *hnd)
{
  ܐܢ(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

صحيح Curl_thread_join(curl_thread_t *hnd)
{
  صحيح ret = (pthread_join(**hnd, عدم) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  ܦܢܐ ret;
}

#ܐܚܪܢܐ
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

فراغ Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

فراغ Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  ܐܢ(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    ܦܢܐ (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  ܦܢܐ CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(فراغ)
{
  ܦܢܐ GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ܦܢܐ tid == GetCurrentThreadId();
}

#وإلاإذا defined(HAVE_THREADS_POSIX)

فراغ Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (فراغ)pthread_cond_signal(c);
}

فراغ Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (فراغ)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  بنية curltime now;
  بنية timespec ts;
  timediff_t usec;
  صحيح rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  بنية timeval tv;
  (فراغ)gettimeofday(&tv, عدم);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (صحيح)tv.tv_usec;
#ܐܚܪܢܐ
  now.tv_sec = time(عدم);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  ܐܢ(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  ܐܢ(rc == SOCKETIMEDOUT)
    ܦܢܐ CURLE_OPERATION_TIMEDOUT;
  ܦܢܐ rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(فراغ)
{
  ܦܢܐ pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ܦܢܐ !!pthread_equal(tid, pthread_self());
}

#ܐܚܪܢܐ
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
