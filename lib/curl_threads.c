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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "curl_threads.h"
#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ଶୂନ୍ୟ *), ଶୂନ୍ୟ *arg)
{
  curl_thread_t t = CreateThread(ଖାଲି, 0, func, arg, 0, ଖାଲି);
  ଯଦି(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    ଫେରନ୍ତୁ curl_thread_t_null;
  }
  ଫେରନ୍ତୁ t;
}

ଶୂନ୍ୟ Curl_thread_destroy(curl_thread_t *hnd)
{
  ଯଦି(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_thread_join(curl_thread_t *hnd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  ଫେରନ୍ତୁ ret;
}

#ନଚେତଯଦି defined(HAVE_THREADS_POSIX)

ସଂରଚନା Curl_actual_call {
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*func)(ଶୂନ୍ୟ *);
  ଶୂନ୍ୟ *arg;
};

ସ୍ଥିର ଶୂନ୍ୟ *curl_thread_create_thunk(ଶୂନ୍ୟ *arg)
{
  ସଂରଚନା Curl_actual_call *ac = arg;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*func)(ଶୂନ୍ୟ *) = ac->func;
  ଶୂନ୍ୟ *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  ଫେରନ୍ତୁ 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ଶୂନ୍ୟ *), ଶୂନ୍ୟ *arg)
{
  curl_thread_t t = curlx_malloc(ଆକାର(pthread_t));
  ସଂରଚନା Curl_actual_call *ac = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(t)
    ac = curlx_malloc(ଆକାର(ସଂରଚନା Curl_actual_call));
  ଯଦି(!(ac && t))
    ଯାଅ err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, ଖାଲି, curl_thread_create_thunk, ac);
  ଯଦି(rc) {
    errno = rc;
    ଯାଅ err;
  }

  ଫେରନ୍ତୁ t;

err:
  curlx_free(t);
  curlx_free(ac);
  ଫେରନ୍ତୁ curl_thread_t_null;
}

ଶୂନ୍ୟ Curl_thread_destroy(curl_thread_t *hnd)
{
  ଯଦି(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_thread_join(curl_thread_t *hnd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = (pthread_join(**hnd, ଖାଲି) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  ଫେରନ୍ତୁ ret;
}

#ଅନ୍ୟଥା
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

ଶୂନ୍ୟ Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

ଶୂନ୍ୟ Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  ଯଦି(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    ଫେରନ୍ତୁ (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ଫେରନ୍ତୁ tid == GetCurrentThreadId();
}

#ନଚେତଯଦି defined(HAVE_THREADS_POSIX)

ଶୂନ୍ୟ Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (ଶୂନ୍ୟ)pthread_cond_signal(c);
}

ଶୂନ୍ୟ Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (ଶୂନ୍ୟ)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  ସଂରଚନା curltime now;
  ସଂରଚନା timespec ts;
  timediff_t usec;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  ସଂରଚନା timeval tv;
  (ଶୂନ୍ୟ)gettimeofday(&tv, ଖାଲି);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv.tv_usec;
#ଅନ୍ୟଥା
  now.tv_sec = time(ଖାଲି);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  ଯଦି(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  ଯଦି(rc == SOCKETIMEDOUT)
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
  ଫେରନ୍ତୁ rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ଫେରନ୍ତୁ !!pthread_equal(tid, pthread_self());
}

#ଅନ୍ୟଥା
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
