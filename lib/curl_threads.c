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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_threads.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ਧਾਗੇ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ਖਾਲੀ *), ਖਾਲੀ *arg)
{
  curl_thread_t t = CreateThread(ਨਿਰਾ, 0, func, arg, 0, ਨਿਰਾ);
  ਜੇ(!t) {
    DWORD gle = Get_ਆਖਰੀ_Error();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_ਸਮ੍ਰਿਤੀ) ?
             EACCES : EINVAL;
    ਵਾਪਸ curl_thread_t_null;
  }
  ਵਾਪਸ t;
}

ਖਾਲੀ Curl_thread_destroy(curl_thread_t *hnd)
{
  ਜੇ(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ਪੂਰਨ Curl_thread_join(curl_thread_t *hnd)
{
  ਪੂਰਨ ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  ਵਾਪਸ ret;
}

#ਜਾਂਜੇ defined(HAVE_ਧਾਗੇ_POSIX)

ਬਣਤਰ Curl_actual_call {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ (*func)(ਖਾਲੀ *);
  ਖਾਲੀ *arg;
};

ਸਥਿਰ ਖਾਲੀ *curl_thread_create_thunk(ਖਾਲੀ *arg)
{
  ਬਣਤਰ Curl_actual_call *ac = arg;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ (*func)(ਖਾਲੀ *) = ac->func;
  ਖਾਲੀ *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  ਵਾਪਸ 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ਖਾਲੀ *), ਖਾਲੀ *arg)
{
  curl_thread_t t = curlx_malloc(ਆਕਾਰ(pthread_t));
  ਬਣਤਰ Curl_actual_call *ac = ਨਿਰਾ;
  ਪੂਰਨ rc;

  ਜੇ(t)
    ac = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ Curl_actual_call));
  ਜੇ(!(ac && t))
    ਜਾਓ err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, ਨਿਰਾ, curl_thread_create_thunk, ac);
  ਜੇ(rc) {
    errno = rc;
    ਜਾਓ err;
  }

  ਵਾਪਸ t;

err:
  curlx_free(t);
  curlx_free(ac);
  ਵਾਪਸ curl_thread_t_null;
}

ਖਾਲੀ Curl_thread_destroy(curl_thread_t *hnd)
{
  ਜੇ(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ਪੂਰਨ Curl_thread_join(curl_thread_t *hnd)
{
  ਪੂਰਨ ret = (pthread_join(**hnd, ਨਿਰਾ) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  ਵਾਪਸ ret;
}

#ਨਹੀਂਤਾਂ
#ਗਲਤੀ neither HAVE_ਧਾਗੇ_POSIX nor _WIN32 defined
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* USE_THREADS */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

ਖਾਲੀ Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

ਖਾਲੀ Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  ਜੇ(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = Get_ਆਖਰੀ_Error();
    ਵਾਪਸ (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  ਵਾਪਸ CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ਖਾਲੀ)
{
  ਵਾਪਸ GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ਵਾਪਸ tid == GetCurrentThreadId();
}

#ਜਾਂਜੇ defined(HAVE_ਧਾਗੇ_POSIX)

ਖਾਲੀ Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (ਖਾਲੀ)pthread_cond_signal(c);
}

ਖਾਲੀ Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (ਖਾਲੀ)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  ਬਣਤਰ curltime now;
  ਬਣਤਰ timespec ts;
  timediff_t usec;
  ਪੂਰਨ rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_GETTIMEOFDAY
  ਬਣਤਰ timeval tv;
  (ਖਾਲੀ)gettimeofday(&tv, ਨਿਰਾ);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (ਪੂਰਨ)tv.tv_usec;
#ਨਹੀਂਤਾਂ
  now.tv_sec = ਸਮਾਂ(ਨਿਰਾ);
  now.tv_usec = 0;
#ਜੇ_ਅੰਤ

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  ਜੇ(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (ਲੰਮਾ)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  ਜੇ(rc == SOCKETIMEDOUT)
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
  ਵਾਪਸ rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ਖਾਲੀ)
{
  ਵਾਪਸ pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ਵਾਪਸ !!pthread_equal(tid, pthread_self());
}

#ਨਹੀਂਤਾਂ
#ਗਲਤੀ neither HAVE_ਧਾਗੇ_POSIX nor _WIN32 defined
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* USE_MUTEX */
