#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_EASY_تالو_H
#وصف_ڪريو سرو_CURL_EASY_تالو_H
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

#وصف_ڪريو GLOBAL_INIT_IS_THREADSAFE

#جيڪڏهن_وصف_ٿيل _WIN32

#وصف_ڪريو curl_simple_تالو      SRWLOCK
#وصف_ڪريو CURL_SIMPLE_تالو_INIT SRWLOCK_INIT

#وصف_ڪريو curl_simple_تالو_تالو(m)   AcquireSRWLockExclusive(m)
#وصف_ڪريو curl_simple_تالو_unlock(m) ReleaseSRWLockExclusive(m)

#وإلاإذا defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#ضمن <stdatomic.h>
#جيڪڏهن_وصف_ٿيل HAVE_SCHED_YIELD
#ضمن <sched.h>
#جيڪڏهن_پورو

#وصف_ڪريو curl_simple_تالو      atomic_int
#وصف_ڪريو CURL_SIMPLE_تالو_INIT 0

#جيڪڏهن_وصف_نه_ٿيل __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#إذا (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#وصف_ڪريو HAVE_BUILTIN_IA32_PAUSE
#وإلاإذا defined(__has_builtin)  /* Keep this PP check separate from others */
#إذا __has_builtin(__builtin_ia32_pause)
#وصف_ڪريو HAVE_BUILTIN_IA32_PAUSE
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* !__INTEL_COMPILER */

ساكن CURL_INLINE فراغ curl_simple_تالو_تالو(curl_simple_تالو *تالو)
{
  لكل(;;) {
    إذا(!atomic_exchange_explicit(تالو, true, يادگيري_ترتيب_acquire))
      اكسر;
    /* Reduce cache coherency traffic */
    طالما(atomic_load_explicit(تالو, يادگيري_ترتيب_relaxed)) {
      /* Reduce load (not mandatory) */
#جيڪڏهن_وصف_ٿيل HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#وإلاإذا defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ متطاير("yield" ::: "memory");
#وإلاإذا defined(HAVE_SCHED_YIELD)
      sched_yield();
#جيڪڏهن_پورو
    }
  }
}

ساكن CURL_INLINE فراغ curl_simple_تالو_unlock(curl_simple_تالو *تالو)
{
  atomic_store_explicit(تالو, false, يادگيري_ترتيب_release);
}

#وإلاإذا defined(HAVE_ڌاڳا_POSIX)

#وصف_ڪريو curl_simple_تالو           pthread_mutex_t
#وصف_ڪريو CURL_SIMPLE_تالو_INIT      PTHREAD_MUTEX_INITIALIZER
#وصف_ڪريو curl_simple_تالو_تالو(m)   pthread_mutex_تالو(m)
#وصف_ڪريو curl_simple_تالو_unlock(m) pthread_mutex_unlock(m)

#وإلا

#وصف_رد GLOBAL_INIT_IS_THREADSAFE

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_EASY_LOCK_H */
