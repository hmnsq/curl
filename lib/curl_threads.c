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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "curl_threads.h"
#અન્તર્ભાવ "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(શૂન્ય *), શૂન્ય *arg)
{
  curl_thread_t t = CreateThread(ખાલી, 0, func, arg, 0, ખાલી);
  જો(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    પાછા curl_thread_t_null;
  }
  પાછા t;
}

શૂન્ય Curl_thread_destroy(curl_thread_t *hnd)
{
  જો(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

પૂર્ણાંક Curl_thread_join(curl_thread_t *hnd)
{
  પૂર્ણાંક ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  પાછા ret;
}

#અથવાજો defined(HAVE_THREADS_POSIX)

સંરચના Curl_actual_call {
  unsigned પૂર્ણાંક (*func)(શૂન્ય *);
  શૂન્ય *arg;
};

સ્થિર શૂન્ય *curl_thread_create_thunk(શૂન્ય *arg)
{
  સંરચના Curl_actual_call *ac = arg;
  unsigned પૂર્ણાંક (*func)(શૂન્ય *) = ac->func;
  શૂન્ય *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  પાછા 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(શૂન્ય *), શૂન્ય *arg)
{
  curl_thread_t t = curlx_malloc(કદ(pthread_t));
  સંરચના Curl_actual_call *ac = ખાલી;
  પૂર્ણાંક rc;

  જો(t)
    ac = curlx_malloc(કદ(સંરચના Curl_actual_call));
  જો(!(ac && t))
    જાઓ err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, ખાલી, curl_thread_create_thunk, ac);
  જો(rc) {
    errno = rc;
    જાઓ err;
  }

  પાછા t;

err:
  curlx_free(t);
  curlx_free(ac);
  પાછા curl_thread_t_null;
}

શૂન્ય Curl_thread_destroy(curl_thread_t *hnd)
{
  જો(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

પૂર્ણાંક Curl_thread_join(curl_thread_t *hnd)
{
  પૂર્ણાંક ret = (pthread_join(**hnd, ખાલી) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  પાછા ret;
}

#નહીંતર
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

શૂન્ય Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

શૂન્ય Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  જો(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    પાછા (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  પાછા CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(શૂન્ય)
{
  પાછા GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  પાછા tid == GetCurrentThreadId();
}

#અથવાજો defined(HAVE_THREADS_POSIX)

શૂન્ય Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (શૂન્ય)pthread_cond_signal(c);
}

શૂન્ય Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (શૂન્ય)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  સંરચના curltime now;
  સંરચના timespec ts;
  timediff_t usec;
  પૂર્ણાંક rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  સંરચના timeval tv;
  (શૂન્ય)gettimeofday(&tv, ખાલી);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (પૂર્ણાંક)tv.tv_usec;
#નહીંતર
  now.tv_sec = time(ખાલી);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  જો(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  જો(rc == SOCKETIMEDOUT)
    પાછા CURLE_OPERATION_TIMEDOUT;
  પાછા rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(શૂન્ય)
{
  પાછા pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  પાછા !!pthread_equal(tid, pthread_self());
}

#નહીંતર
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
