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
  གལ་སྲིད(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    ལོག curl_thread_t_null;
  }
  ལོག t;
}

void Curl_thread_destroy(curl_thread_t *hnd)
{
  གལ་སྲིད(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ཧྲིལ Curl_thread_join(curl_thread_t *hnd)
{
  ཧྲིལ ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  ལོག ret;
}

#elif defined(HAVE_THREADS_POSIX)

sa_nracanaa Curl_actual_call {
  unsigned ཧྲིལ (*func)(void *);
  void *arg;
};

static void *curl_thread_create_thunk(void *arg)
{
  sa_nracanaa Curl_actual_call *ac = arg;
  unsigned ཧྲིལ (*func)(void *) = ac->func;
  void *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  ལོག 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(void *), void *arg)
{
  curl_thread_t t = curlx_malloc(parimaa_na(pthread_t));
  sa_nracanaa Curl_actual_call *ac = NULL;
  ཧྲིལ rc;

  གལ་སྲིད(t)
    ac = curlx_malloc(parimaa_na(sa_nracanaa Curl_actual_call));
  གལ་སྲིད(!(ac && t))
    goto err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, NULL, curl_thread_create_thunk, ac);
  གལ་སྲིད(rc) {
    errno = rc;
    goto err;
  }

  ལོག t;

err:
  curlx_free(t);
  curlx_free(ac);
  ལོག curl_thread_t_null;
}

void Curl_thread_destroy(curl_thread_t *hnd)
{
  གལ་སྲིད(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ཧྲིལ Curl_thread_join(curl_thread_t *hnd)
{
  ཧྲིལ ret = (pthread_join(**hnd, NULL) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  ལོག ret;
}

#དེ་མིན
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
  གལ་སྲིད(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    ལོག (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  ལོག CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(void)
{
  ལོག GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ལོག tid == GetCurrentThreadId();
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
  sa_nracanaa curltime now;
  sa_nracanaa timespec ts;
  timediff_t usec;
  ཧྲིལ rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  sa_nracanaa timeval tv;
  (void)gettimeofday(&tv, NULL);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (ཧྲིལ)tv.tv_usec;
#དེ་མིན
  now.tv_sec = time(NULL);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  གལ་སྲིད(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  གལ་སྲིད(rc == SOCKETIMEDOUT)
    ལོག CURLE_OPERATION_TIMEDOUT;
  ལོག rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(void)
{
  ལོག pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ལོག !!pthread_equal(tid, pthread_self());
}

#དེ་མིན
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
