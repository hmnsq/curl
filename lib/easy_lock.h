#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_EASY_ಬೀಗ_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_EASY_ಬೀಗ_H
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

#ವ್ಯಾಖ್ಯಾನಿಸು GLOBAL_INIT_IS_THREADSAFE

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

#ವ್ಯಾಖ್ಯಾನಿಸು curl_simple_ಬೀಗ      SRWLOCK
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SIMPLE_ಬೀಗ_INIT SRWLOCK_INIT

#ವ್ಯಾಖ್ಯಾನಿಸು curl_simple_ಬೀಗ_ಬೀಗ(m)   AcquireSRWLockExclusive(m)
#ವ್ಯಾಖ್ಯಾನಿಸು curl_simple_ಬೀಗ_unlock(m) ReleaseSRWLockExclusive(m)

#ಅಥವಾಆದರೆ defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#ಅನ್ತರ್ಭಾವ <stdatomic.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SCHED_YIELD
#ಅನ್ತರ್ಭಾವ <sched.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು curl_simple_ಬೀಗ      atomic_int
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SIMPLE_ಬೀಗ_INIT 0

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#ಆದರೆ (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_BUILTIN_IA32_PAUSE
#ಅಥವಾಆದರೆ defined(__has_builtin)  /* Keep this PP check separate from others */
#ಆದರೆ __has_builtin(__builtin_ia32_pause)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_BUILTIN_IA32_PAUSE
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* !__INTEL_COMPILER */

ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ curl_simple_ಬೀಗ_ಬೀಗ(curl_simple_ಬೀಗ *ಬೀಗ)
{
  ಗಾಗಿ(;;) {
    ಆದರೆ(!atomic_exchange_explicit(ಬೀಗ, true, ಸ್ಮರಣೆ_ಕ್ರಮ_acquire))
      ನಿಲ್ಲಿಸು;
    /* Reduce cache coherency traffic */
    ಆಗ(atomic_load_explicit(ಬೀಗ, ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed)) {
      /* Reduce load (not mandatory) */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#ಅಥವಾಆದರೆ defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ ಅಸ್ಥಿರ("yield" ::: "memory");
#ಅಥವಾಆದರೆ defined(HAVE_SCHED_YIELD)
      sched_yield();
#ಆದರೆ_ಅಂತ್ಯ
    }
  }
}

ಸ್ಥಿರ CURL_INLINE ಶೂನ್ಯ curl_simple_ಬೀಗ_unlock(curl_simple_ಬೀಗ *ಬೀಗ)
{
  atomic_store_explicit(ಬೀಗ, false, ಸ್ಮರಣೆ_ಕ್ರಮ_release);
}

#ಅಥವಾಆದರೆ defined(HAVE_ಎಳೆಗಳು_POSIX)

#ವ್ಯಾಖ್ಯಾನಿಸು curl_simple_ಬೀಗ           pthread_mutex_t
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SIMPLE_ಬೀಗ_INIT      PTHREAD_MUTEX_INITIALIZER
#ವ್ಯಾಖ್ಯಾನಿಸು curl_simple_ಬೀಗ_ಬೀಗ(m)   pthread_mutex_ಬೀಗ(m)
#ವ್ಯಾಖ್ಯಾನಿಸು curl_simple_ಬೀಗ_unlock(m) pthread_mutex_unlock(m)

#ಇಲ್ಲದಿದ್ದರೆ

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು GLOBAL_INIT_IS_THREADSAFE

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_EASY_LOCK_H */
