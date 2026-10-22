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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "curl_threads.h"
#অন্তর্ভাব "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(শূন্য *), শূন্য *arg)
{
  curl_thread_t t = CreateThread(রিক্ত, 0, func, arg, 0, রিক্ত);
  যদি(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    প্রত্যাগম curl_thread_t_null;
  }
  প্রত্যাগম t;
}

শূন্য Curl_thread_destroy(curl_thread_t *hnd)
{
  যদি(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

পূর্ণাঙ্ক Curl_thread_join(curl_thread_t *hnd)
{
  পূর্ণাঙ্ক ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  প্রত্যাগম ret;
}

#নোচেদ্যদি defined(HAVE_THREADS_POSIX)

সংরচনা Curl_actual_call {
  unsigned পূর্ণাঙ্ক (*func)(শূন্য *);
  শূন্য *arg;
};

স্থির শূন্য *curl_thread_create_thunk(শূন্য *arg)
{
  সংরচনা Curl_actual_call *ac = arg;
  unsigned পূর্ণাঙ্ক (*func)(শূন্য *) = ac->func;
  শূন্য *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  প্রত্যাগম 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(শূন্য *), শূন্য *arg)
{
  curl_thread_t t = curlx_malloc(পরিমাণ(pthread_t));
  সংরচনা Curl_actual_call *ac = রিক্ত;
  পূর্ণাঙ্ক rc;

  যদি(t)
    ac = curlx_malloc(পরিমাণ(সংরচনা Curl_actual_call));
  যদি(!(ac && t))
    গচ্ছ err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, রিক্ত, curl_thread_create_thunk, ac);
  যদি(rc) {
    errno = rc;
    গচ্ছ err;
  }

  প্রত্যাগম t;

err:
  curlx_free(t);
  curlx_free(ac);
  প্রত্যাগম curl_thread_t_null;
}

শূন্য Curl_thread_destroy(curl_thread_t *hnd)
{
  যদি(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

পূর্ণাঙ্ক Curl_thread_join(curl_thread_t *hnd)
{
  পূর্ণাঙ্ক ret = (pthread_join(**hnd, রিক্ত) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  প্রত্যাগম ret;
}

#অন্যথা
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

শূন্য Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

শূন্য Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  যদি(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    প্রত্যাগম (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  প্রত্যাগম CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(শূন্য)
{
  প্রত্যাগম GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  প্রত্যাগম tid == GetCurrentThreadId();
}

#নোচেদ্যদি defined(HAVE_THREADS_POSIX)

শূন্য Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (শূন্য)pthread_cond_signal(c);
}

শূন্য Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (শূন্য)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  সংরচনা curltime now;
  সংরচনা timespec ts;
  timediff_t usec;
  পূর্ণাঙ্ক rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  সংরচনা timeval tv;
  (শূন্য)gettimeofday(&tv, রিক্ত);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (পূর্ণাঙ্ক)tv.tv_usec;
#অন্যথা
  now.tv_sec = time(রিক্ত);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  যদি(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  যদি(rc == SOCKETIMEDOUT)
    প্রত্যাগম CURLE_OPERATION_TIMEDOUT;
  প্রত্যাগম rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(শূন্য)
{
  প্রত্যাগম pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  প্রত্যাগম !!pthread_equal(tid, pthread_self());
}

#অন্যথা
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
