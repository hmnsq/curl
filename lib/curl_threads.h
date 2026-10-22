#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ಎಳೆಗಳು_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ಎಳೆಗಳು_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
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
#  define Curl_cond_destroy(c)   (ಶೂನ್ಯ)(c)
#ಅಥವಾಆದರೆ defined(HAVE_ಎಳೆಗಳು_POSIX)
#  define CURL_THREAD_RETURN_T   ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ
#  define CURL_STDCALL
#  define curl_mutex_t           pthread_mutex_t
#  define curl_thread_t          pthread_t *
#  define curl_thread_id_t       pthread_t
#  define curl_thread_t_null     (pthread_t *)0
#  define Curl_mutex_init(m)     pthread_mutex_init(m, ಖಾಲಿ)
#  define Curl_mutex_acquire(m)  pthread_mutex_ಬೀಗ(m)
#  define Curl_mutex_release(m)  pthread_mutex_unlock(m)
#  define Curl_mutex_destroy(m)  pthread_mutex_destroy(m)
#  define curl_cond_t            pthread_cond_t
#  define Curl_cond_init(c)      pthread_cond_init(c, ಖಾಲಿ)
#  define Curl_cond_destroy(c)   pthread_cond_destroy(c)
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ neither HAVE_ಎಳೆಗಳು_POSIX nor _WIN32 defined
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ Curl_cond_signal(curl_cond_t *c);
ಶೂನ್ಯ Curl_cond_wait(curl_cond_t *c, curl_mutex_t *m);
/* Returns CURLE_OPERATION_TIMEDOUT on timeout */
CURLcode Curl_cond_timedwait(curl_cond_t *c, curl_mutex_t *m,
                             uint32_t timeout_ms);

curl_thread_id_t Curl_thread_get_current_id(ಶೂನ್ಯ);
bool Curl_thread_is_current(curl_thread_id_t tid);

#ಆದರೆ_ಅಂತ್ಯ /* USE_MUTEX */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ಎಳೆಗಳು

curl_thread_t Curl_thread_create(
  CURL_THREAD_RETURN_T(CURL_STDCALL *func)(ಶೂನ್ಯ *), ಶೂನ್ಯ *arg);

ಶೂನ್ಯ Curl_thread_destroy(curl_thread_t *hnd);

ಪೂರ್ಣಾಂಕ Curl_thread_join(curl_thread_t *hnd);

#ಆದರೆ_ಅಂತ್ಯ /* USE_THREADS */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_THREADS_H */
