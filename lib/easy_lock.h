#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_EASY_ਤਾਲਾ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_EASY_ਤਾਲਾ_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GLOBAL_INIT_IS_THREADSAFE

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_simple_ਤਾਲਾ      SRWLOCK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SIMPLE_ਤਾਲਾ_INIT SRWLOCK_INIT

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_simple_ਤਾਲਾ_ਤਾਲਾ(m)   AcquireSRWLockExclusive(m)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_simple_ਤਾਲਾ_unlock(m) ReleaseSRWLockExclusive(m)

#ਜਾਂਜੇ defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#ਅਨ੍ਤਰ੍ਭਾਵ <stdatomic.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SCHED_YIELD
#ਅਨ੍ਤਰ੍ਭਾਵ <sched.h>
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_simple_ਤਾਲਾ      atomic_int
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SIMPLE_ਤਾਲਾ_INIT 0

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#ਜੇ (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_BUILTIN_IA32_PAUSE
#ਜਾਂਜੇ defined(__has_builtin)  /* Keep this PP check separate from others */
#ਜੇ __has_builtin(__builtin_ia32_pause)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_BUILTIN_IA32_PAUSE
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* !__INTEL_COMPILER */

ਸਥਿਰ CURL_INLINE ਖਾਲੀ curl_simple_ਤਾਲਾ_ਤਾਲਾ(curl_simple_ਤਾਲਾ *ਤਾਲਾ)
{
  ਲਈ(;;) {
    ਜੇ(!atomic_exchange_explicit(ਤਾਲਾ, true, ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_acquire))
      ਰੋਕੋ;
    /* Reduce cache coherency traffic */
    ਜਦੋਂ(atomic_load_explicit(ਤਾਲਾ, ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed)) {
      /* Reduce load (not mandatory) */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#ਜਾਂਜੇ defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ ਅਸਥਿਰ("yield" ::: "memory");
#ਜਾਂਜੇ defined(HAVE_SCHED_YIELD)
      sched_yield();
#ਜੇ_ਅੰਤ
    }
  }
}

ਸਥਿਰ CURL_INLINE ਖਾਲੀ curl_simple_ਤਾਲਾ_unlock(curl_simple_ਤਾਲਾ *ਤਾਲਾ)
{
  atomic_store_explicit(ਤਾਲਾ, false, ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_release);
}

#ਜਾਂਜੇ defined(HAVE_ਧਾਗੇ_POSIX)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_simple_ਤਾਲਾ           pthread_mutex_t
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SIMPLE_ਤਾਲਾ_INIT      PTHREAD_MUTEX_INITIALIZER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_simple_ਤਾਲਾ_ਤਾਲਾ(m)   pthread_mutex_ਤਾਲਾ(m)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ curl_simple_ਤਾਲਾ_unlock(m) pthread_mutex_unlock(m)

#ਨਹੀਂਤਾਂ

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ GLOBAL_INIT_IS_THREADSAFE

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_EASY_LOCK_H */
