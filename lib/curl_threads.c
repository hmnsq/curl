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
#include "curl_setup.h"
#include "curl_threads.h"
#include "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(void *), void *arg)
{
  curl_thread_t t = CreateThread(NULL, 0, func, arg, 0, NULL);
  如果(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    返回 curl_thread_t_null;
  }
  返回 t;
}

void Curl_thread_destroy(curl_thread_t *hnd)
{
  如果(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

整数 Curl_thread_join(curl_thread_t *hnd)
{
  整数 ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  返回 ret;
}

#elif defined(HAVE_THREADS_POSIX)

struct Curl_actual_call {
  unsigned 整数 (*func)(void *);
  void *arg;
};

static void *curl_thread_create_thunk(void *arg)
{
  struct Curl_actual_call *ac = arg;
  unsigned 整数 (*func)(void *) = ac->func;
  void *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  返回 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(void *), void *arg)
{
  curl_thread_t t = curlx_malloc(sizeof(pthread_t));
  struct Curl_actual_call *ac = NULL;
  整数 rc;

  如果(t)
    ac = curlx_malloc(sizeof(struct Curl_actual_call));
  如果(!(ac && t))
    goto err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, NULL, curl_thread_create_thunk, ac);
  如果(rc) {
    errno = rc;
    goto err;
  }

  返回 t;

err:
  curlx_free(t);
  curlx_free(ac);
  返回 curl_thread_t_null;
}

void Curl_thread_destroy(curl_thread_t *hnd)
{
  如果(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

整数 Curl_thread_join(curl_thread_t *hnd)
{
  整数 ret = (pthread_join(**hnd, NULL) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  返回 ret;
}

#否则
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

void Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

void Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  如果(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    返回 (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  返回 CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(void)
{
  返回 GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  返回 tid == GetCurrentThreadId();
}

#elif defined(HAVE_THREADS_POSIX)

void Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (void)pthread_cond_signal(c);
}

void Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (void)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  struct curltime now;
  struct timespec ts;
  timediff_t usec;
  整数 rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  struct timeval tv;
  (void)gettimeofday(&tv, NULL);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (整数)tv.tv_usec;
#否则
  now.tv_sec = time(NULL);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  如果(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  如果(rc == SOCKETIMEDOUT)
    返回 CURLE_OPERATION_TIMEDOUT;
  返回 rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(void)
{
  返回 pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  返回 !!pthread_equal(tid, pthread_self());
}

#否则
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
