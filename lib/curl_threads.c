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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "curl_threads.h"
#அந்தர்பாவ "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(வெறுமை *), வெறுமை *arg)
{
  curl_thread_t t = CreateThread(வெற்று, 0, func, arg, 0, வெற்று);
  எனில்(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    திரும்பு curl_thread_t_null;
  }
  திரும்பு t;
}

வெறுமை Curl_thread_destroy(curl_thread_t *hnd)
{
  எனில்(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

முழு Curl_thread_join(curl_thread_t *hnd)
{
  முழு ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  திரும்பு ret;
}

#அல்லதெனில் defined(HAVE_THREADS_POSIX)

கட்டமைப்பு Curl_actual_call {
  unsigned முழு (*func)(வெறுமை *);
  வெறுமை *arg;
};

நிலையான வெறுமை *curl_thread_create_thunk(வெறுமை *arg)
{
  கட்டமைப்பு Curl_actual_call *ac = arg;
  unsigned முழு (*func)(வெறுமை *) = ac->func;
  வெறுமை *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  திரும்பு 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(வெறுமை *), வெறுமை *arg)
{
  curl_thread_t t = curlx_malloc(அளவு(pthread_t));
  கட்டமைப்பு Curl_actual_call *ac = வெற்று;
  முழு rc;

  எனில்(t)
    ac = curlx_malloc(அளவு(கட்டமைப்பு Curl_actual_call));
  எனில்(!(ac && t))
    செல் err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, வெற்று, curl_thread_create_thunk, ac);
  எனில்(rc) {
    errno = rc;
    செல் err;
  }

  திரும்பு t;

err:
  curlx_free(t);
  curlx_free(ac);
  திரும்பு curl_thread_t_null;
}

வெறுமை Curl_thread_destroy(curl_thread_t *hnd)
{
  எனில்(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

முழு Curl_thread_join(curl_thread_t *hnd)
{
  முழு ret = (pthread_join(**hnd, வெற்று) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  திரும்பு ret;
}

#இல்லையெனில்
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

வெறுமை Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

வெறுமை Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  எனில்(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    திரும்பு (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  திரும்பு CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(வெறுமை)
{
  திரும்பு GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  திரும்பு tid == GetCurrentThreadId();
}

#அல்லதெனில் defined(HAVE_THREADS_POSIX)

வெறுமை Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (வெறுமை)pthread_cond_signal(c);
}

வெறுமை Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (வெறுமை)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  கட்டமைப்பு curltime now;
  கட்டமைப்பு timespec ts;
  timediff_t usec;
  முழு rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  கட்டமைப்பு timeval tv;
  (வெறுமை)gettimeofday(&tv, வெற்று);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (முழு)tv.tv_usec;
#இல்லையெனில்
  now.tv_sec = time(வெற்று);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  எனில்(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  எனில்(rc == SOCKETIMEDOUT)
    திரும்பு CURLE_OPERATION_TIMEDOUT;
  திரும்பு rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(வெறுமை)
{
  திரும்பு pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  திரும்பு !!pthread_equal(tid, pthread_self());
}

#இல்லையெனில்
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
