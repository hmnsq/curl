#جے_بنا_تعریف سرخی_CURL_EASY_جندرا_H
#تعریف_کرو سرخی_CURL_EASY_جندرا_H
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

#تعریف_کرو GLOBAL_INIT_IS_THREADSAFE

#جے_تعریف_شدہ _WIN32

#تعریف_کرو curl_simple_جندرا      SRWLOCK
#تعریف_کرو CURL_SIMPLE_جندرا_INIT SRWLOCK_INIT

#تعریف_کرو curl_simple_جندرا_جندرا(m)   AcquireSRWLockExclusive(m)
#تعریف_کرو curl_simple_جندرا_unlock(m) ReleaseSRWLockExclusive(m)

#وإلاإذا defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#ضمن <stdatomic.h>
#جے_تعریف_شدہ HAVE_SCHED_YIELD
#ضمن <sched.h>
#جے_انت

#تعریف_کرو curl_simple_جندرا      atomic_int
#تعریف_کرو CURL_SIMPLE_جندرا_INIT 0

#جے_بنا_تعریف __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#إذا (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#تعریف_کرو HAVE_BUILTIN_IA32_PAUSE
#وإلاإذا defined(__has_builtin)  /* Keep this PP check separate from others */
#إذا __has_builtin(__builtin_ia32_pause)
#تعریف_کرو HAVE_BUILTIN_IA32_PAUSE
#جے_انت
#جے_انت

#جے_انت /* !__INTEL_COMPILER */

ساكن CURL_INLINE فراغ curl_simple_جندرا_جندرا(curl_simple_جندرا *جندرا)
{
  لكل(;;) {
    إذا(!atomic_exchange_explicit(جندرا, true, حافظہ_ترتیب_acquire))
      اكسر;
    /* Reduce cache coherency traffic */
    طالما(atomic_load_explicit(جندرا, حافظہ_ترتیب_relaxed)) {
      /* Reduce load (not mandatory) */
#جے_تعریف_شدہ HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#وإلاإذا defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ متطاير("yield" ::: "memory");
#وإلاإذا defined(HAVE_SCHED_YIELD)
      sched_yield();
#جے_انت
    }
  }
}

ساكن CURL_INLINE فراغ curl_simple_جندرا_unlock(curl_simple_جندرا *جندرا)
{
  atomic_store_explicit(جندرا, false, حافظہ_ترتیب_release);
}

#وإلاإذا defined(HAVE_دھاگے_POSIX)

#تعریف_کرو curl_simple_جندرا           pthread_mutex_t
#تعریف_کرو CURL_SIMPLE_جندرا_INIT      PTHREAD_MUTEX_INITIALIZER
#تعریف_کرو curl_simple_جندرا_جندرا(m)   pthread_mutex_جندرا(m)
#تعریف_کرو curl_simple_جندرا_unlock(m) pthread_mutex_unlock(m)

#وإلا

#تعریف_رد GLOBAL_INIT_IS_THREADSAFE

#جے_انت

#جے_انت /* HEADER_CURL_EASY_LOCK_H */
