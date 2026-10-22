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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ಶೂನ್ಯ *), ಶೂನ್ಯ *arg)
{
  curl_thread_t t = CreateThread(ರಿಕ್ತ, 0, func, arg, 0, ರಿಕ್ತ);
  ಯದಿ(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    ಪ್ರತ್ಯಾಗಮ curl_thread_t_null;
  }
  ಪ್ರತ್ಯಾಗಮ t;
}

ಶೂನ್ಯ Curl_thread_destroy(curl_thread_t *hnd)
{
  ಯದಿ(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ಪೂರ್ಣಾಙ್ಕ Curl_thread_join(curl_thread_t *hnd)
{
  ಪೂರ್ಣಾಙ್ಕ ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  ಪ್ರತ್ಯಾಗಮ ret;
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_THREADS_POSIX)

ಸಂರಚನಾ Curl_actual_call {
  unsigned ಪೂರ್ಣಾಙ್ಕ (*func)(ಶೂನ್ಯ *);
  ಶೂನ್ಯ *arg;
};

ಸ್ಥಿರ ಶೂನ್ಯ *curl_thread_create_thunk(ಶೂನ್ಯ *arg)
{
  ಸಂರಚನಾ Curl_actual_call *ac = arg;
  unsigned ಪೂರ್ಣಾಙ್ಕ (*func)(ಶೂನ್ಯ *) = ac->func;
  ಶೂನ್ಯ *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  ಪ್ರತ್ಯಾಗಮ 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ಶೂನ್ಯ *), ಶೂನ್ಯ *arg)
{
  curl_thread_t t = curlx_malloc(ಪರಿಮಾಣ(pthread_t));
  ಸಂರಚನಾ Curl_actual_call *ac = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(t)
    ac = curlx_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_actual_call));
  ಯದಿ(!(ac && t))
    ಗಚ್ಛ err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, ರಿಕ್ತ, curl_thread_create_thunk, ac);
  ಯದಿ(rc) {
    errno = rc;
    ಗಚ್ಛ err;
  }

  ಪ್ರತ್ಯಾಗಮ t;

err:
  curlx_free(t);
  curlx_free(ac);
  ಪ್ರತ್ಯಾಗಮ curl_thread_t_null;
}

ಶೂನ್ಯ Curl_thread_destroy(curl_thread_t *hnd)
{
  ಯದಿ(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ಪೂರ್ಣಾಙ್ಕ Curl_thread_join(curl_thread_t *hnd)
{
  ಪೂರ್ಣಾಙ್ಕ ret = (pthread_join(**hnd, ರಿಕ್ತ) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  ಪ್ರತ್ಯಾಗಮ ret;
}

#ಅನ್ಯಥಾ
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

ಶೂನ್ಯ Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

ಶೂನ್ಯ Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  ಯದಿ(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    ಪ್ರತ್ಯಾಗಮ (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ಪ್ರತ್ಯಾಗಮ tid == GetCurrentThreadId();
}

#ನೋಚೇದ್ಯದಿ defined(HAVE_THREADS_POSIX)

ಶೂನ್ಯ Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (ಶೂನ್ಯ)pthread_cond_signal(c);
}

ಶೂನ್ಯ Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (ಶೂನ್ಯ)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  ಸಂರಚನಾ curltime now;
  ಸಂರಚನಾ timespec ts;
  timediff_t usec;
  ಪೂರ್ಣಾಙ್ಕ rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  ಸಂರಚನಾ timeval tv;
  (ಶೂನ್ಯ)gettimeofday(&tv, ರಿಕ್ತ);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (ಪೂರ್ಣಾಙ್ಕ)tv.tv_usec;
#ಅನ್ಯಥಾ
  now.tv_sec = time(ರಿಕ್ತ);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  ಯದಿ(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (long)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  ಯದಿ(rc == SOCKETIMEDOUT)
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
  ಪ್ರತ್ಯಾಗಮ rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ಪ್ರತ್ಯಾಗಮ !!pthread_equal(tid, pthread_self());
}

#ಅನ್ಯಥಾ
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
