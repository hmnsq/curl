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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "curl_threads.h"
#അന്തര്ഭാവ "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ശൂന്യം *), ശൂന്യം *arg)
{
  curl_thread_t t = CreateThread(ശൂന്യ, 0, func, arg, 0, ശൂന്യ);
  എങ്കിൽ(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    തിരികെ curl_thread_t_null;
  }
  തിരികെ t;
}

ശൂന്യം Curl_thread_destroy(curl_thread_t *hnd)
{
  എങ്കിൽ(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

പൂർണ്ണസംഖ്യ Curl_thread_join(curl_thread_t *hnd)
{
  പൂർണ്ണസംഖ്യ ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  തിരികെ ret;
}

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_THREADS_POSIX)

ഘടന Curl_actual_call {
  unsigned പൂർണ്ണസംഖ്യ (*func)(ശൂന്യം *);
  ശൂന്യം *arg;
};

സ്ഥിതം ശൂന്യം *curl_thread_create_thunk(ശൂന്യം *arg)
{
  ഘടന Curl_actual_call *ac = arg;
  unsigned പൂർണ്ണസംഖ്യ (*func)(ശൂന്യം *) = ac->func;
  ശൂന്യം *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  തിരികെ 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ശൂന്യം *), ശൂന്യം *arg)
{
  curl_thread_t t = curlx_malloc(വലുപ്പം(pthread_t));
  ഘടന Curl_actual_call *ac = ശൂന്യ;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(t)
    ac = curlx_malloc(വലുപ്പം(ഘടന Curl_actual_call));
  എങ്കിൽ(!(ac && t))
    പോകുക err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, ശൂന്യ, curl_thread_create_thunk, ac);
  എങ്കിൽ(rc) {
    errno = rc;
    പോകുക err;
  }

  തിരികെ t;

err:
  curlx_free(t);
  curlx_free(ac);
  തിരികെ curl_thread_t_null;
}

ശൂന്യം Curl_thread_destroy(curl_thread_t *hnd)
{
  എങ്കിൽ(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

പൂർണ്ണസംഖ്യ Curl_thread_join(curl_thread_t *hnd)
{
  പൂർണ്ണസംഖ്യ ret = (pthread_join(**hnd, ശൂന്യ) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  തിരികെ ret;
}

#അല്ലെങ്കിൽ
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

ശൂന്യം Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

ശൂന്യം Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  എങ്കിൽ(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    തിരികെ (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  തിരികെ CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ശൂന്യം)
{
  തിരികെ GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  തിരികെ tid == GetCurrentThreadId();
}

#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_THREADS_POSIX)

ശൂന്യം Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (ശൂന്യം)pthread_cond_signal(c);
}

ശൂന്യം Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (ശൂന്യം)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  ഘടന curltime now;
  ഘടന timespec ts;
  timediff_t usec;
  പൂർണ്ണസംഖ്യ rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  ഘടന timeval tv;
  (ശൂന്യം)gettimeofday(&tv, ശൂന്യ);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (പൂർണ്ണസംഖ്യ)tv.tv_usec;
#അല്ലെങ്കിൽ
  now.tv_sec = time(ശൂന്യ);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  എങ്കിൽ(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  എങ്കിൽ(rc == SOCKETIMEDOUT)
    തിരികെ CURLE_OPERATION_TIMEDOUT;
  തിരികെ rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ശൂന്യം)
{
  തിരികെ pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  തിരികെ !!pthread_equal(tid, pthread_self());
}

#അല്ലെങ്കിൽ
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
