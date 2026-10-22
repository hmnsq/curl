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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "curlx/timeval.h"

#ifdef USE_THREADS

#ifdef _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(शून्य *), शून्य *arg)
{
  curl_thread_t t = CreateThread(रिक्त, 0, func, arg, 0, रिक्त);
  यदि(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    त्रुटि_सं = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    फर्काऊ curl_thread_t_null;
  }
  फर्काऊ t;
}

शून्य Curl_thread_destroy(curl_thread_t *hnd)
{
  यदि(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

पूर्णाङ्क Curl_thread_join(curl_thread_t *hnd)
{
  पूर्णाङ्क ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  फर्काऊ ret;
}

#अथवायदि defined(HAVE_THREADS_POSIX)

संरचना Curl_actual_call {
  अचिन्हित पूर्णाङ्क (*func)(शून्य *);
  शून्य *arg;
};

स्थिर शून्य *curl_thread_create_thunk(शून्य *arg)
{
  संरचना Curl_actual_call *ac = arg;
  अचिन्हित पूर्णाङ्क (*func)(शून्य *) = ac->func;
  शून्य *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  फर्काऊ 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(शून्य *), शून्य *arg)
{
  curl_thread_t t = curlx_malloc(परिमाण(pthread_t));
  संरचना Curl_actual_call *ac = रिक्त;
  पूर्णाङ्क rc;

  यदि(t)
    ac = curlx_malloc(परिमाण(संरचना Curl_actual_call));
  यदि(!(ac && t))
    गच्छ err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, रिक्त, curl_thread_create_thunk, ac);
  यदि(rc) {
    त्रुटि_सं = rc;
    गच्छ err;
  }

  फर्काऊ t;

err:
  curlx_free(t);
  curlx_free(ac);
  फर्काऊ curl_thread_t_null;
}

शून्य Curl_thread_destroy(curl_thread_t *hnd)
{
  यदि(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

पूर्णाङ्क Curl_thread_join(curl_thread_t *hnd)
{
  पूर्णाङ्क ret = (pthread_join(**hnd, रिक्त) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  फर्काऊ ret;
}

#अन्यथा
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_THREADS */

#ifdef USE_MUTEX

#ifdef _WIN32

शून्य Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

शून्य Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  यदि(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    फर्काऊ (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  फर्काऊ CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(शून्य)
{
  फर्काऊ GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  फर्काऊ tid == GetCurrentThreadId();
}

#अथवायदि defined(HAVE_THREADS_POSIX)

शून्य Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (शून्य)pthread_cond_signal(c);
}

शून्य Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (शून्य)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  संरचना curltime now;
  संरचना timespec ts;
  timediff_t usec;
  पूर्णाङ्क rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#ifdef HAVE_GETTIMEOFDAY
  संरचना timeval tv;
  (शून्य)gettimeofday(&tv, रिक्त);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (पूर्णाङ्क)tv.tv_usec;
#अन्यथा
  now.tv_sec = समय(रिक्त);
  now.tv_usec = 0;
#endif

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  यदि(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (दीर्घ)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  यदि(rc == SOCKETIMEDOUT)
    फर्काऊ CURLE_OPERATION_TIMEDOUT;
  फर्काऊ rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(शून्य)
{
  फर्काऊ pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  फर्काऊ !!pthread_equal(tid, pthread_self());
}

#अन्यथा
#error neither HAVE_THREADS_POSIX nor _WIN32 defined
#endif
#endif /* USE_MUTEX */
