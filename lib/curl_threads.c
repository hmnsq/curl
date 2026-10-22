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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಎಳೆಗಳು

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ಶೂನ್ಯ *), ಶೂನ್ಯ *arg)
{
  curl_thread_t t = CreateThread(ಖಾಲಿ, 0, func, arg, 0, ಖಾಲಿ);
  ಆದರೆ(!t) {
    DWORD gle = Get_ಕೊನೆಯ_Error();
    /* !checksrc! disable ERRNOVAR 1 */
    errno = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_ಸ್ಮರಣೆ) ?
             EACCES : EINVAL;
    ಹಿಂತಿರುಗಿ curl_thread_t_null;
  }
  ಹಿಂತಿರುಗಿ t;
}

ಶೂನ್ಯ Curl_thread_destroy(curl_thread_t *hnd)
{
  ಆದರೆ(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ಪೂರ್ಣಾಂಕ Curl_thread_join(curl_thread_t *hnd)
{
  ಪೂರ್ಣಾಂಕ ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  ಹಿಂತಿರುಗಿ ret;
}

#ಅಥವಾಆದರೆ defined(HAVE_ಎಳೆಗಳು_POSIX)

ರಚನೆ Curl_actual_call {
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ (*func)(ಶೂನ್ಯ *);
  ಶೂನ್ಯ *arg;
};

ಸ್ಥಿರ ಶೂನ್ಯ *curl_thread_create_thunk(ಶೂನ್ಯ *arg)
{
  ರಚನೆ Curl_actual_call *ac = arg;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ (*func)(ಶೂನ್ಯ *) = ac->func;
  ಶೂನ್ಯ *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  ಹಿಂತಿರುಗಿ 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ಶೂನ್ಯ *), ಶೂನ್ಯ *arg)
{
  curl_thread_t t = curlx_malloc(ಗಾತ್ರ(pthread_t));
  ರಚನೆ Curl_actual_call *ac = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(t)
    ac = curlx_malloc(ಗಾತ್ರ(ರಚನೆ Curl_actual_call));
  ಆದರೆ(!(ac && t))
    ಹೋಗು err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, ಖಾಲಿ, curl_thread_create_thunk, ac);
  ಆದರೆ(rc) {
    errno = rc;
    ಹೋಗು err;
  }

  ಹಿಂತಿರುಗಿ t;

err:
  curlx_free(t);
  curlx_free(ac);
  ಹಿಂತಿರುಗಿ curl_thread_t_null;
}

ಶೂನ್ಯ Curl_thread_destroy(curl_thread_t *hnd)
{
  ಆದರೆ(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

ಪೂರ್ಣಾಂಕ Curl_thread_join(curl_thread_t *hnd)
{
  ಪೂರ್ಣಾಂಕ ret = (pthread_join(**hnd, ಖಾಲಿ) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  ಹಿಂತಿರುಗಿ ret;
}

#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ neither HAVE_ಎಳೆಗಳು_POSIX nor _WIN32 defined
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* USE_THREADS */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

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
  ಆದರೆ(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = Get_ಕೊನೆಯ_Error();
    ಹಿಂತಿರುಗಿ (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ಹಿಂತಿರುಗಿ tid == GetCurrentThreadId();
}

#ಅಥವಾಆದರೆ defined(HAVE_ಎಳೆಗಳು_POSIX)

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
  ರಚನೆ curltime now;
  ರಚನೆ timespec ts;
  timediff_t usec;
  ಪೂರ್ಣಾಂಕ rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_GETTIMEOFDAY
  ರಚನೆ timeval tv;
  (ಶೂನ್ಯ)gettimeofday(&tv, ಖಾಲಿ);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (ಪೂರ್ಣಾಂಕ)tv.tv_usec;
#ಇಲ್ಲದಿದ್ದರೆ
  now.tv_sec = ಸಮಯ(ಖಾಲಿ);
  now.tv_usec = 0;
#ಆದರೆ_ಅಂತ್ಯ

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  ಆದರೆ(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (ದೀರ್ಘ)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  ಆದರೆ(rc == SOCKETIMEDOUT)
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
  ಹಿಂತಿರುಗಿ rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  ಹಿಂತಿರುಗಿ !!pthread_equal(tid, pthread_self());
}

#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ neither HAVE_ಎಳೆಗಳು_POSIX nor _WIN32 defined
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* USE_MUTEX */
