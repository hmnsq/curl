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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#define GLOBAL_INIT_IS_THREADSAFE

#ifdef _WIN32

#define curl_simple_lock      SRWLOCK
#define CURL_SIMPLE_LOCK_INIT SRWLOCK_INIT

#define curl_simple_lock_lock(m)   AcquireSRWLockExclusive(m)
#define curl_simple_lock_unlock(m) ReleaseSRWLockExclusive(m)

#ନଚେତଯଦି defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#ଅନ୍ତର୍ଭାଵ <stdatomic.h>
#ifdef HAVE_SCHED_YIELD
#ଅନ୍ତର୍ଭାଵ <sched.h>
#endif

#define curl_simple_lock      atomic_int
#define CURL_SIMPLE_LOCK_INIT 0

#ifndef __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#ଯଦି (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#define HAVE_BUILTIN_IA32_PAUSE
#ନଚେତଯଦି defined(__has_builtin)  /* Keep this PP check separate from others */
#ଯଦି __has_builtin(__builtin_ia32_pause)
#define HAVE_BUILTIN_IA32_PAUSE
#endif
#endif

#endif /* !__INTEL_COMPILER */

ସ୍ଥିର CURL_INLINE ଶୂନ୍ୟ curl_simple_lock_lock(curl_simple_lock *lock)
{
  ପାଇଁ(;;) {
    ଯଦି(!atomic_exchange_explicit(lock, true, memory_order_acquire))
      ଭାଙ୍ଗ;
    /* Reduce cache coherency traffic */
    ଯେତେବେଳେ(atomic_load_explicit(lock, memory_order_relaxed)) {
      /* Reduce load (not mandatory) */
#ifdef HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#ନଚେତଯଦି defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ ଅସ୍ଥିର("yield" ::: "memory");
#ନଚେତଯଦି defined(HAVE_SCHED_YIELD)
      sched_yield();
#endif
    }
  }
}

ସ୍ଥିର CURL_INLINE ଶୂନ୍ୟ curl_simple_lock_unlock(curl_simple_lock *lock)
{
  atomic_store_explicit(lock, false, memory_order_release);
}

#ନଚେତଯଦି defined(HAVE_THREADS_POSIX)

#define curl_simple_lock           pthread_mutex_t
#define CURL_SIMPLE_LOCK_INIT      PTHREAD_MUTEX_INITIALIZER
#define curl_simple_lock_lock(m)   pthread_mutex_lock(m)
#define curl_simple_lock_unlock(m) pthread_mutex_unlock(m)

#ଅନ୍ୟଥା

#undef GLOBAL_INIT_IS_THREADSAFE

#endif

#endif /* HEADER_CURL_EASY_LOCK_H */
