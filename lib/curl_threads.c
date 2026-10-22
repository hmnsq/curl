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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "curl_threads.h"
#అన్తర్భావ "curlx/timeval.h"

#నిర్వచితమైతే USE_దారాలు

#నిర్వచితమైతే _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(శూన్య *), శూన్య *arg)
{
  curl_thread_t t = CreateThread(ఖాళీ, 0, func, arg, 0, ఖాళీ);
  ఒకవేళ(!t) {
    DWORD gle = Get_చివరి_Error();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_జ్ఞాపకం) ?
             EACCES : EINVAL;
    తిరిగి curl_thread_t_null;
  }
  తిరిగి t;
}

శూన్య Curl_thread_destroy(curl_thread_t *hnd)
{
  ఒకవేళ(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

పూర్ణాంకం Curl_thread_join(curl_thread_t *hnd)
{
  పూర్ణాంకం ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  తిరిగి ret;
}

#లేదావేళ defined(HAVE_దారాలు_POSIX)

నిర్మాణం Curl_actual_call {
  గుర్తులేని పూర్ణాంకం (*func)(శూన్య *);
  శూన్య *arg;
};

స్థిర శూన్య *curl_thread_create_thunk(శూన్య *arg)
{
  నిర్మాణం Curl_actual_call *ac = arg;
  గుర్తులేని పూర్ణాంకం (*func)(శూన్య *) = ac->func;
  శూన్య *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  తిరిగి 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(శూన్య *), శూన్య *arg)
{
  curl_thread_t t = curlx_malloc(పరిమాణం(pthread_t));
  నిర్మాణం Curl_actual_call *ac = ఖాళీ;
  పూర్ణాంకం rc;

  ఒకవేళ(t)
    ac = curlx_malloc(పరిమాణం(నిర్మాణం Curl_actual_call));
  ఒకవేళ(!(ac && t))
    వెళ్ళు err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, ఖాళీ, curl_thread_create_thunk, ac);
  ఒకవేళ(rc) {
    errno = rc;
    వెళ్ళు err;
  }

  తిరిగి t;

err:
  curlx_free(t);
  curlx_free(ac);
  తిరిగి curl_thread_t_null;
}

శూన్య Curl_thread_destroy(curl_thread_t *hnd)
{
  ఒకవేళ(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

పూర్ణాంకం Curl_thread_join(curl_thread_t *hnd)
{
  పూర్ణాంకం ret = (pthread_join(**hnd, ఖాళీ) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  తిరిగి ret;
}

#లేకపోతే
#దోషం neither HAVE_దారాలు_POSIX nor _WIN32 defined
#అయితే_ముగింపు
#అయితే_ముగింపు /* USE_THREADS */

#నిర్వచితమైతే USE_MUTEX

#నిర్వచితమైతే _WIN32

శూన్య Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

శూన్య Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  ఒకవేళ(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = Get_చివరి_Error();
    తిరిగి (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  తిరిగి CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(శూన్య)
{
  తిరిగి GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  తిరిగి tid == GetCurrentThreadId();
}

#లేదావేళ defined(HAVE_దారాలు_POSIX)

శూన్య Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (శూన్య)pthread_cond_signal(c);
}

శూన్య Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (శూన్య)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  నిర్మాణం curltime now;
  నిర్మాణం timespec ts;
  timediff_t usec;
  పూర్ణాంకం rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#నిర్వచితమైతే HAVE_GETTIMEOFDAY
  నిర్మాణం timeval tv;
  (శూన్య)gettimeofday(&tv, ఖాళీ);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (పూర్ణాంకం)tv.tv_usec;
#లేకపోతే
  now.tv_sec = సమయం(ఖాళీ);
  now.tv_usec = 0;
#అయితే_ముగింపు

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  ఒకవేళ(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (దీర్ఘ)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  ఒకవేళ(rc == SOCKETIMEDOUT)
    తిరిగి CURLE_OPERATION_TIMEDOUT;
  తిరిగి rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(శూన్య)
{
  తిరిగి pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  తిరిగి !!pthread_equal(tid, pthread_self());
}

#లేకపోతే
#దోషం neither HAVE_దారాలు_POSIX nor _WIN32 defined
#అయితే_ముగింపు
#అయితే_ముగింపు /* USE_MUTEX */
