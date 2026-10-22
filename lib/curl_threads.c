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
  အကယ်၍(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    ပြန်ပေး curl_thread_t_null;
  }
  ပြန်ပေး t;
}

void Curl_thread_destroy(curl_thread_t *hnd)
{
  အကယ်၍(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ကိန်းပြည့် Curl_thread_join(curl_thread_t *hnd)
{
  ကိန်းပြည့် ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  ပြန်ပေး ret;
}

#elif defined(HAVE_THREADS_POSIX)

sa_nracanaa Curl_actual_call {
  unsigned ကိန်းပြည့် (*func)(void *);
  void *arg;
};

static void *curl_thread_create_thunk(void *arg)
{
  sa_nracanaa Curl_actual_call *ac = arg;
  unsigned ကိန်းပြည့် (*func)(void *) = ac->func;
  void *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  ပြန်ပေး 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(void *), void *arg)
{
  curl_thread_t t = curlx_malloc(parimaa_na(pthread_t));
  sa_nracanaa Curl_actual_call *ac = NULL;
  ကိန်းပြည့် rc;

  အကယ်၍(t)
    ac = curlx_malloc(parimaa_na(sa_nracanaa Curl_actual_call));
  အကယ်၍(!(ac && t))
    goto err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, NULL, curl_thread_create_thunk, ac);
  အကယ်၍(rc) {
    errno = rc;
    goto err;
  }

  ပြန်ပေး t;

err:
  curlx_free(t);
  curlx_free(ac);
  ပြန်ပေး curl_thread_t_null;
}

void Curl_thread_destroy(curl_thread_t *hnd)
{
  အကယ်၍(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ကိန်းပြည့် Curl_thread_join(curl_thread_t *hnd)
{
  ကိန်းပြည့် ret = (pthread_join(**hnd, NULL) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  ပြန်ပေး ret;
}

#မဟုတ်ပါက
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
  အကယ်၍(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    ပြန်ပေး (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  ပြန်ပေး CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(void)
{
  ပြန်ပေး GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ပြန်ပေး tid == GetCurrentThreadId();
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
  ကိန်းပြည့် rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  sa_nracanaa timeval tv;
  (void)gettimeofday(&tv, NULL);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (ကိန်းပြည့်)tv.tv_usec;
#မဟုတ်ပါက
  now.tv_sec = time(NULL);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  အကယ်၍(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  အကယ်၍(rc == SOCKETIMEDOUT)
    ပြန်ပေး CURLE_OPERATION_TIMEDOUT;
  ပြန်ပေး rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(void)
{
  ပြန်ပေး pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ပြန်ပေး !!pthread_equal(tid, pthread_self());
}

#မဟုတ်ပါက
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
