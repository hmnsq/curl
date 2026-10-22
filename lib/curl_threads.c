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
  ᬬᬾᬦ᭄(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    ᬯᬮᬶ curl_thread_t_null;
  }
  ᬯᬮᬶ t;
}

void Curl_thread_destroy(curl_thread_t *hnd)
{
  ᬬᬾᬦ᭄(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

puur_naa_nka Curl_thread_join(curl_thread_t *hnd)
{
  puur_naa_nka ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  ᬯᬮᬶ ret;
}

#elif defined(HAVE_THREADS_POSIX)

sa_nracanaa Curl_actual_call {
  unsigned puur_naa_nka (*func)(void *);
  void *arg;
};

static void *curl_thread_create_thunk(void *arg)
{
  sa_nracanaa Curl_actual_call *ac = arg;
  unsigned puur_naa_nka (*func)(void *) = ac->func;
  void *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  ᬯᬮᬶ 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(void *), void *arg)
{
  curl_thread_t t = curlx_malloc(parimaa_na(pthread_t));
  sa_nracanaa Curl_actual_call *ac = NULL;
  puur_naa_nka rc;

  ᬬᬾᬦ᭄(t)
    ac = curlx_malloc(parimaa_na(sa_nracanaa Curl_actual_call));
  ᬬᬾᬦ᭄(!(ac && t))
    goto err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, NULL, curl_thread_create_thunk, ac);
  ᬬᬾᬦ᭄(rc) {
    errno = rc;
    goto err;
  }

  ᬯᬮᬶ t;

err:
  curlx_free(t);
  curlx_free(ac);
  ᬯᬮᬶ curl_thread_t_null;
}

void Curl_thread_destroy(curl_thread_t *hnd)
{
  ᬬᬾᬦ᭄(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

puur_naa_nka Curl_thread_join(curl_thread_t *hnd)
{
  puur_naa_nka ret = (pthread_join(**hnd, NULL) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  ᬯᬮᬶ ret;
}

#ᬮᬶᬬ
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
  ᬬᬾᬦ᭄(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    ᬯᬮᬶ (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  ᬯᬮᬶ CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(void)
{
  ᬯᬮᬶ GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ᬯᬮᬶ tid == GetCurrentThreadId();
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
  puur_naa_nka rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  sa_nracanaa timeval tv;
  (void)gettimeofday(&tv, NULL);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (puur_naa_nka)tv.tv_usec;
#ᬮᬶᬬ
  now.tv_sec = time(NULL);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  ᬬᬾᬦ᭄(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  ᬬᬾᬦ᭄(rc == SOCKETIMEDOUT)
    ᬯᬮᬶ CURLE_OPERATION_TIMEDOUT;
  ᬯᬮᬶ rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(void)
{
  ᬯᬮᬶ pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ᬯᬮᬶ !!pthread_equal(tid, pthread_self());
}

#ᬮᬶᬬ
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
