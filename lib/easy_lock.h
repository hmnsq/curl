#ifndef HEADER_CURL_EASY_LOCK_H
#define HEADER_CURL_EASY_LOCK_H
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
#অন্তর্ভাব "curl_setup.h"

#define GLOBAL_INIT_IS_THREADSAFE

#ifdef _WIN32

#define curl_simple_lock      SRWLOCK
#define CURL_SIMPLE_LOCK_INIT SRWLOCK_INIT

#define curl_simple_lock_lock(m)   AcquireSRWLockExclusive(m)
#define curl_simple_lock_unlock(m) ReleaseSRWLockExclusive(m)

#নতুবা_যদি defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#অন্তর্ভাব <stdatomic.h>
#ifdef HAVE_SCHED_YIELD
#অন্তর্ভাব <sched.h>
#endif

#define curl_simple_lock      atomic_int
#define CURL_SIMPLE_LOCK_INIT 0

#ifndef __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#যদি (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#define HAVE_BUILTIN_IA32_PAUSE
#নতুবা_যদি defined(__has_builtin)  /* Keep this PP check separate from others */
#যদি __has_builtin(__builtin_ia32_pause)
#define HAVE_BUILTIN_IA32_PAUSE
#endif
#endif

#endif /* !__INTEL_COMPILER */

স্থির CURL_INLINE শূন্য curl_simple_lock_lock(curl_simple_lock *lock)
{
  জন্য(;;) {
    যদি(!atomic_exchange_explicit(lock, true, memory_order_acquire))
      বিরতি;
    /* Reduce cache coherency traffic */
    যখন(atomic_load_explicit(lock, memory_order_relaxed)) {
      /* Reduce load (not mandatory) */
#ifdef HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#নতুবা_যদি defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ অস্থির("yield" ::: "memory");
#নতুবা_যদি defined(HAVE_SCHED_YIELD)
      sched_yield();
#endif
    }
  }
}

স্থির CURL_INLINE শূন্য curl_simple_lock_unlock(curl_simple_lock *lock)
{
  atomic_store_explicit(lock, false, memory_order_release);
}

#নতুবা_যদি defined(HAVE_THREADS_POSIX)

#define curl_simple_lock           pthread_mutex_t
#define CURL_SIMPLE_LOCK_INIT      PTHREAD_MUTEX_INITIALIZER
#define curl_simple_lock_lock(m)   pthread_mutex_lock(m)
#define curl_simple_lock_unlock(m) pthread_mutex_unlock(m)

#অন্যথা

#undef GLOBAL_INIT_IS_THREADSAFE

#endif

#endif /* HEADER_CURL_EASY_LOCK_H */
