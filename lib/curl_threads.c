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
#شامل "curl_setup.h"
#شامل "curl_threads.h"
#شامل "curlx/timeval.h"

#اگر_تعریف_شدہ USE_دھاگے

#اگر_تعریف_شدہ _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(خالی *), خالی *arg)
{
  curl_thread_t t = CreateThread(تہی, 0, func, arg, 0, تہی);
  اگر(!t) {
    DWORD gle = Get_آخری_Error();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_حافظہ) ?
             EACCES : EINVAL;
    واپس curl_thread_t_null;
  }
  واپس t;
}

خالی Curl_thread_destroy(curl_thread_t *hnd)
{
  اگر(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

صحیح Curl_thread_join(curl_thread_t *hnd)
{
  صحیح ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  واپس ret;
}

#وگرنہ_اگر defined(HAVE_دھاگے_POSIX)

ساخت Curl_actual_call {
  غیرعلامتی صحیح (*func)(خالی *);
  خالی *arg;
};

ساکن خالی *curl_thread_create_thunk(خالی *arg)
{
  ساخت Curl_actual_call *ac = arg;
  غیرعلامتی صحیح (*func)(خالی *) = ac->func;
  خالی *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  واپس 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(خالی *), خالی *arg)
{
  curl_thread_t t = curlx_malloc(حجم(pthread_t));
  ساخت Curl_actual_call *ac = تہی;
  صحیح rc;

  اگر(t)
    ac = curlx_malloc(حجم(ساخت Curl_actual_call));
  اگر(!(ac && t))
    جاؤ err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, تہی, curl_thread_create_thunk, ac);
  اگر(rc) {
    errno = rc;
    جاؤ err;
  }

  واپس t;

err:
  curlx_free(t);
  curlx_free(ac);
  واپس curl_thread_t_null;
}

خالی Curl_thread_destroy(curl_thread_t *hnd)
{
  اگر(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

صحیح Curl_thread_join(curl_thread_t *hnd)
{
  صحیح ret = (pthread_join(**hnd, تہی) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  واپس ret;
}

#ورنہ
#غلطی neither HAVE_دھاگے_POSIX nor _WIN32 defined
#اگر_ختم
#اگر_ختم /* USE_THREADS */

#اگر_تعریف_شدہ USE_MUTEX

#اگر_تعریف_شدہ _WIN32

خالی Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

خالی Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  اگر(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = Get_آخری_Error();
    واپس (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  واپس CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(خالی)
{
  واپس GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  واپس tid == GetCurrentThreadId();
}

#وگرنہ_اگر defined(HAVE_دھاگے_POSIX)

خالی Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (خالی)pthread_cond_signal(c);
}

خالی Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (خالی)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  ساخت curltime now;
  ساخت timespec ts;
  timediff_t usec;
  صحیح rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#اگر_تعریف_شدہ HAVE_GETTIMEOFDAY
  ساخت timeval tv;
  (خالی)gettimeofday(&tv, تہی);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (صحیح)tv.tv_usec;
#ورنہ
  now.tv_sec = وقت(تہی);
  now.tv_usec = 0;
#اگر_ختم

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  اگر(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (طویل)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  اگر(rc == SOCKETIMEDOUT)
    واپس CURLE_OPERATION_TIMEDOUT;
  واپس rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(خالی)
{
  واپس pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  واپس !!pthread_equal(tid, pthread_self());
}

#ورنہ
#غلطی neither HAVE_دھاگے_POSIX nor _WIN32 defined
#اگر_ختم
#اگر_ختم /* USE_MUTEX */
