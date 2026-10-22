#اگر_تعریف_نشده سرآیند_CURL_EASY_قفل_H
#تعریف_کن سرآیند_CURL_EASY_قفل_H
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

#تعریف_کن GLOBAL_INIT_IS_THREADSAFE

#اگر_تعریف_شده _WIN32

#تعریف_کن curl_simple_قفل      SRWLOCK
#تعریف_کن CURL_SIMPLE_قفل_INIT SRWLOCK_INIT

#تعریف_کن curl_simple_قفل_قفل(m)   AcquireSRWLockExclusive(m)
#تعریف_کن curl_simple_قفل_unlock(m) ReleaseSRWLockExclusive(m)

#وإلاإذا defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#ضمن <stdatomic.h>
#اگر_تعریف_شده HAVE_SCHED_YIELD
#ضمن <sched.h>
#پایان_اگر

#تعریف_کن curl_simple_قفل      atomic_int
#تعریف_کن CURL_SIMPLE_قفل_INIT 0

#اگر_تعریف_نشده __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#إذا (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#تعریف_کن HAVE_BUILTIN_IA32_PAUSE
#وإلاإذا defined(__has_builtin)  /* Keep this PP check separate from others */
#إذا __has_builtin(__builtin_ia32_pause)
#تعریف_کن HAVE_BUILTIN_IA32_PAUSE
#پایان_اگر
#پایان_اگر

#پایان_اگر /* !__INTEL_COMPILER */

ساكن CURL_INLINE فراغ curl_simple_قفل_قفل(curl_simple_قفل *قفل)
{
  لكل(;;) {
    إذا(!atomic_exchange_explicit(قفل, true, حافظه_ترتیب_acquire))
      اكسر;
    /* Reduce cache coherency traffic */
    طالما(atomic_load_explicit(قفل, حافظه_ترتیب_relaxed)) {
      /* Reduce load (not mandatory) */
#اگر_تعریف_شده HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#وإلاإذا defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ متطاير("yield" ::: "memory");
#وإلاإذا defined(HAVE_SCHED_YIELD)
      sched_yield();
#پایان_اگر
    }
  }
}

ساكن CURL_INLINE فراغ curl_simple_قفل_unlock(curl_simple_قفل *قفل)
{
  atomic_store_explicit(قفل, false, حافظه_ترتیب_release);
}

#وإلاإذا defined(HAVE_نخها_POSIX)

#تعریف_کن curl_simple_قفل           pthread_mutex_t
#تعریف_کن CURL_SIMPLE_قفل_INIT      PTHREAD_MUTEX_INITIALIZER
#تعریف_کن curl_simple_قفل_قفل(m)   pthread_mutex_قفل(m)
#تعریف_کن curl_simple_قفل_unlock(m) pthread_mutex_unlock(m)

#وإلا

#لغو_تعریف GLOBAL_INIT_IS_THREADSAFE

#پایان_اگر

#پایان_اگر /* HEADER_CURL_EASY_LOCK_H */
