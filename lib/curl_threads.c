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
#समावेश "curl_setup.h"
#समावेश "curl_threads.h"
#समावेश "curlx/timeval.h"

#अगर_घोषित USE_THREADS

#अगर_घोषित _WIN32

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(व्योम *), व्योम *arg)
{
  curl_thread_t t = CreateThread(शून्य, 0, func, arg, 0, शून्य);
  अगर(!t) {
    DWORD gle = GetLastError();
    /* !checksrc! disable ERRNOVAR 1 */
    त्रुटि_सं = (gle == ERROR_ACCESS_DENIED ||
             gle == ERROR_NOT_ENOUGH_MEMORY) ?
             EACCES : EINVAL;
    वापस curl_thread_t_null;
  }
  वापस t;
}

व्योम Curl_thread_destroy(curl_thread_t *hnd)
{
  अगर(*hnd != curl_thread_t_null) {
    CloseHandle(*hnd);
    *hnd = curl_thread_t_null;
  }
}

पूर्णांक Curl_thread_join(curl_thread_t *hnd)
{
  पूर्णांक ret = (WaitForSingleObjectEx(*hnd, INFINITE, FALSE) == WAIT_OBJECT_0);

  Curl_thread_destroy(hnd);

  वापस ret;
}

#या_अगर defined(HAVE_THREADS_POSIX)

काष्ठा Curl_actual_call {
  अचिन्हित पूर्णांक (*func)(व्योम *);
  व्योम *arg;
};

अटल व्योम *curl_thread_create_thunk(व्योम *arg)
{
  काष्ठा Curl_actual_call *ac = arg;
  अचिन्हित पूर्णांक (*func)(व्योम *) = ac->func;
  व्योम *real_arg = ac->arg;

  curlx_free(ac);

  (*func)(real_arg);

  वापस 0;
}

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(व्योम *), व्योम *arg)
{
  curl_thread_t t = curlx_malloc(माप(pthread_t));
  काष्ठा Curl_actual_call *ac = शून्य;
  पूर्णांक rc;

  अगर(t)
    ac = curlx_malloc(माप(काष्ठा Curl_actual_call));
  अगर(!(ac && t))
    जाओ err;

  ac->func = func;
  ac->arg = arg;

  rc = pthread_create(t, शून्य, curl_thread_create_thunk, ac);
  अगर(rc) {
    त्रुटि_सं = rc;
    जाओ err;
  }

  वापस t;

err:
  curlx_free(t);
  curlx_free(ac);
  वापस curl_thread_t_null;
}

व्योम Curl_thread_destroy(curl_thread_t *hnd)
{
  अगर(*hnd != curl_thread_t_null) {
    pthread_detach(**hnd);
    curlx_free(*hnd);
    *hnd = curl_thread_t_null;
  }
}

पूर्णांक Curl_thread_join(curl_thread_t *hnd)
{
  पूर्णांक ret = (pthread_join(**hnd, शून्य) == 0);

  curlx_free(*hnd);
  *hnd = curl_thread_t_null;

  वापस ret;
}

#अन्यथा
#त्रुटि neither HAVE_THREADS_POSIX nor _WIN32 defined
#पूर्ण_अगर
#पूर्ण_अगर /* USE_THREADS */

#अगर_घोषित USE_MUTEX

#अगर_घोषित _WIN32

व्योम Curl_cond_signal(CONDITION_VARIABLE *c)
{
  WakeConditionVariable(c);
}

व्योम Curl_cond_wait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m)
{
  SleepConditionVariableCS(c, m, INFINITE);
}

CURLcode Curl_cond_timedwait(CONDITION_VARIABLE *c, CRITICAL_SECTION *m,
                             uint32_t timeout_ms)
{
  अगर(!SleepConditionVariableCS(c, m, (DWORD)timeout_ms)) {
    DWORD err = GetLastError();
    वापस (err == ERROR_TIMEOUT) ?
           CURLE_OPERATION_TIMEDOUT : CURLE_UNRECOVERABLE_POLL;
  }
  वापस CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(व्योम)
{
  वापस GetCurrentThreadId();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  वापस tid == GetCurrentThreadId();
}

#या_अगर defined(HAVE_THREADS_POSIX)

व्योम Curl_cond_signal(pthread_cond_t *c)
{
  /* return code defined as always 0 */
  (व्योम)pthread_cond_signal(c);
}

व्योम Curl_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
  /* return code defined as always 0 */
  (व्योम)pthread_cond_wait(c, m);
}

CURLcode Curl_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m,
                             uint32_t timeout_ms)
{
  काष्ठा curltime now;
  काष्ठा timespec ts;
  timediff_t usec;
  पूर्णांक rc;

  /* POSIX expects an "absolute" time until the condition wait ends.
   * We cannot use `curlx_now()` here that may run on some monotonic clock
   * that will be most likely in the past, as far as POSIX abstime is
   * concerned. */
#अगर_घोषित HAVE_GETTIMEOFDAY
  काष्ठा timeval tv;
  (व्योम)gettimeofday(&tv, शून्य);
  now.tv_sec = tv.tv_sec;
  now.tv_usec = (पूर्णांक)tv.tv_usec;
#अन्यथा
  now.tv_sec = समय(शून्य);
  now.tv_usec = 0;
#पूर्ण_अगर

  ts.tv_sec = now.tv_sec + (timeout_ms / 1000);
  usec = now.tv_usec + ((timeout_ms % 1000) * 1000);
  अगर(usec >= 1000000) {
    ++ts.tv_sec;
    usec %= 1000000;
  }
  ts.tv_nsec = (दीर्घ)usec * 1000;

  rc = pthread_cond_timedwait(c, m, &ts);
  अगर(rc == SOCKETIMEDOUT)
    वापस CURLE_OPERATION_TIMEDOUT;
  वापस rc ? CURLE_UNRECOVERABLE_POLL : CURLE_OK;
}

curl_thread_id_t Curl_thread_get_current_id(व्योम)
{
  वापस pthread_self();
}

bool Curl_thread_is_current(curl_thread_id_t tid)
{
  वापस !!pthread_equal(tid, pthread_self());
}

#अन्यथा
#त्रुटि neither HAVE_THREADS_POSIX nor _WIN32 defined
#पूर्ण_अगर
#पूर्ण_अगर /* USE_MUTEX */
