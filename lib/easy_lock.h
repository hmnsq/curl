#यदि_अपरिभाषितम् शीर्षकम्_CURL_EASY_तालकम्_H
#परिभाषय शीर्षकम्_CURL_EASY_तालकम्_H
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
#अन्तर्भाव "curl_setup.h"

#परिभाषय GLOBAL_INIT_IS_THREADSAFE

#यदि_परिभाषितम् _WIN32

#परिभाषय curl_simple_तालकम्      SRWLOCK
#परिभाषय CURL_SIMPLE_तालकम्_INIT SRWLOCK_INIT

#परिभाषय curl_simple_तालकम्_तालकम्(m)   AcquireSRWLockExclusive(m)
#परिभाषय curl_simple_तालकम्_unlock(m) ReleaseSRWLockExclusive(m)

#नोचेद्यदि defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#अन्तर्भाव <stdatomic.h>
#यदि_परिभाषितम् HAVE_SCHED_YIELD
#अन्तर्भाव <sched.h>
#यदि_अन्तः

#परिभाषय curl_simple_तालकम्      atomic_int
#परिभाषय CURL_SIMPLE_तालकम्_INIT 0

#यदि_अपरिभाषितम् __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#यदि (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#परिभाषय HAVE_BUILTIN_IA32_PAUSE
#नोचेद्यदि defined(__has_builtin)  /* Keep this PP check separate from others */
#यदि __has_builtin(__builtin_ia32_pause)
#परिभाषय HAVE_BUILTIN_IA32_PAUSE
#यदि_अन्तः
#यदि_अन्तः

#यदि_अन्तः /* !__INTEL_COMPILER */

स्थिर CURL_INLINE शून्य curl_simple_तालकम्_तालकम्(curl_simple_तालकम् *तालकम्)
{
  प्रति(;;) {
    यदि(!atomic_exchange_explicit(तालकम्, true, स्मृतिः_क्रमः_acquire))
      भङ्ग;
    /* Reduce cache coherency traffic */
    यावत्(atomic_load_explicit(तालकम्, स्मृतिः_क्रमः_relaxed)) {
      /* Reduce load (not mandatory) */
#यदि_परिभाषितम् HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#नोचेद्यदि defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ चञ्चल("yield" ::: "memory");
#नोचेद्यदि defined(HAVE_SCHED_YIELD)
      sched_yield();
#यदि_अन्तः
    }
  }
}

स्थिर CURL_INLINE शून्य curl_simple_तालकम्_unlock(curl_simple_तालकम् *तालकम्)
{
  atomic_store_explicit(तालकम्, false, स्मृतिः_क्रमः_release);
}

#नोचेद्यदि defined(HAVE_तन्तवः_POSIX)

#परिभाषय curl_simple_तालकम्           pthread_mutex_t
#परिभाषय CURL_SIMPLE_तालकम्_INIT      PTHREAD_MUTEX_INITIALIZER
#परिभाषय curl_simple_तालकम्_तालकम्(m)   pthread_mutex_तालकम्(m)
#परिभाषय curl_simple_तालकम्_unlock(m) pthread_mutex_unlock(m)

#अन्यथा

#अपरिभाषय GLOBAL_INIT_IS_THREADSAFE

#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_EASY_LOCK_H */
