#که_نه_تعریف_شوی سرلیک_CURL_EASY_قلف_H
#تعریف_کړه سرلیک_CURL_EASY_قلف_H
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

#تعریف_کړه GLOBAL_INIT_IS_THREADSAFE

#که_تعریف_شوی _WIN32

#تعریف_کړه curl_simple_قلف      SRWLOCK
#تعریف_کړه CURL_SIMPLE_قلف_INIT SRWLOCK_INIT

#تعریف_کړه curl_simple_قلف_قلف(m)   AcquireSRWLockExclusive(m)
#تعریف_کړه curl_simple_قلف_unlock(m) ReleaseSRWLockExclusive(m)

#وإلاإذا defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#ضمن <stdatomic.h>
#که_تعریف_شوی HAVE_SCHED_YIELD
#ضمن <sched.h>
#د_که_پای

#تعریف_کړه curl_simple_قلف      atomic_int
#تعریف_کړه CURL_SIMPLE_قلف_INIT 0

#که_نه_تعریف_شوی __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#إذا (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#تعریف_کړه HAVE_BUILTIN_IA32_PAUSE
#وإلاإذا defined(__has_builtin)  /* Keep this PP check separate from others */
#إذا __has_builtin(__builtin_ia32_pause)
#تعریف_کړه HAVE_BUILTIN_IA32_PAUSE
#د_که_پای
#د_که_پای

#د_که_پای /* !__INTEL_COMPILER */

ساكن CURL_INLINE فراغ curl_simple_قلف_قلف(curl_simple_قلف *قلف)
{
  لكل(;;) {
    إذا(!atomic_exchange_explicit(قلف, true, حافظه_ترتیب_acquire))
      اكسر;
    /* Reduce cache coherency traffic */
    طالما(atomic_load_explicit(قلف, حافظه_ترتیب_relaxed)) {
      /* Reduce load (not mandatory) */
#که_تعریف_شوی HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#وإلاإذا defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ متطاير("yield" ::: "memory");
#وإلاإذا defined(HAVE_SCHED_YIELD)
      sched_yield();
#د_که_پای
    }
  }
}

ساكن CURL_INLINE فراغ curl_simple_قلف_unlock(curl_simple_قلف *قلف)
{
  atomic_store_explicit(قلف, false, حافظه_ترتیب_release);
}

#وإلاإذا defined(HAVE_تارونه_POSIX)

#تعریف_کړه curl_simple_قلف           pthread_mutex_t
#تعریف_کړه CURL_SIMPLE_قلف_INIT      PTHREAD_MUTEX_INITIALIZER
#تعریف_کړه curl_simple_قلف_قلف(m)   pthread_mutex_قلف(m)
#تعریف_کړه curl_simple_قلف_unlock(m) pthread_mutex_unlock(m)

#وإلا

#تعریف_لغوه GLOBAL_INIT_IS_THREADSAFE

#د_که_پای

#د_که_پای /* HEADER_CURL_EASY_LOCK_H */
