#اگر_تعریف_نشده سرآیند_CURL_نخها_H
#تعریف_کن سرآیند_CURL_نخها_H
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
#ضمن "curl_setup.h"

#اگر_تعریف_شده USE_MUTEX
#اگر_تعریف_شده _WIN32
#  define CURL_THREAD_RETURN_T   DWORD
#  define CURL_STDCALL           WINAPI
#  define curl_mutex_t           CRITICAL_SECTION
#  define curl_thread_t          HANDLE
#  define curl_thread_id_t       DWORD
#  define curl_thread_t_null     (HANDLE)0
#  define Curl_mutex_init(m)     InitializeCriticalSectionEx(m, 0, 1)
#  define Curl_mutex_acquire(m)  EnterCriticalSection(m)
#  define Curl_mutex_release(m)  LeaveCriticalSection(m)
#  define Curl_mutex_destroy(m)  DeleteCriticalSection(m)
#  define curl_cond_t            CONDITION_VARIABLE
#  define Curl_cond_init(c)      InitializeConditionVariable(c)
#  define Curl_cond_destroy(c)   (فراغ)(c)
#وإلاإذا defined(HAVE_نخها_POSIX)
#  define CURL_THREAD_RETURN_T   بیعلامت صحيح
#  define CURL_STDCALL
#  define curl_mutex_t           pthread_mutex_t
#  define curl_thread_t          pthread_t *
#  define curl_thread_id_t       pthread_t
#  define curl_thread_t_null     (pthread_t *)0
#  define Curl_mutex_init(m)     pthread_mutex_init(m, عدم)
#  define Curl_mutex_acquire(m)  pthread_mutex_قفل(m)
#  define Curl_mutex_release(m)  pthread_mutex_unlock(m)
#  define Curl_mutex_destroy(m)  pthread_mutex_destroy(m)
#  define curl_cond_t            pthread_cond_t
#  define Curl_cond_init(c)      pthread_cond_init(c, عدم)
#  define Curl_cond_destroy(c)   pthread_cond_destroy(c)
#وإلا
#خطا neither HAVE_نخها_POSIX nor _WIN32 defined
#پایان_اگر

فراغ Curl_cond_signal(curl_cond_t *c);
فراغ Curl_cond_wait(curl_cond_t *c, curl_mutex_t *m);
/* Returns CURLE_OPERATION_TIMEDOUT on timeout */
CURLcode Curl_cond_timedwait(curl_cond_t *c, curl_mutex_t *m,
                             uint32_t timeout_ms);

curl_thread_id_t Curl_thread_get_current_id(فراغ);
bool Curl_thread_is_current(curl_thread_id_t tid);

#پایان_اگر /* USE_MUTEX */

#اگر_تعریف_شده USE_نخها

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(فراغ *), فراغ *arg);

فراغ Curl_thread_destroy(curl_thread_t *hnd);

صحيح Curl_thread_join(curl_thread_t *hnd);

#پایان_اگر /* USE_THREADS */

#پایان_اگر /* HEADER_CURL_THREADS_H */
