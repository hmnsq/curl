#अगर_अघोषित HEADER_CURL_EASY_LOCK_H
#घोषणा HEADER_CURL_EASY_LOCK_H
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
#समावेश "curl_setup.h"

#घोषणा GLOBAL_INIT_IS_THREADSAFE

#अगर_घोषित _WIN32

#घोषणा curl_simple_lock      SRWLOCK
#घोषणा CURL_SIMPLE_LOCK_INIT SRWLOCK_INIT

#घोषणा curl_simple_lock_lock(m)   AcquireSRWLockExclusive(m)
#घोषणा curl_simple_lock_unlock(m) ReleaseSRWLockExclusive(m)

#या_अगर defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#समावेश <stdatomic.h>
#अगर_घोषित HAVE_SCHED_YIELD
#समावेश <sched.h>
#पूर्ण_अगर

#घोषणा curl_simple_lock      atomic_int
#घोषणा CURL_SIMPLE_LOCK_INIT 0

#अगर_अघोषित __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#अगर (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#घोषणा HAVE_BUILTIN_IA32_PAUSE
#या_अगर defined(__has_builtin)  /* Keep this PP check separate from others */
#अगर __has_builtin(__builtin_ia32_pause)
#घोषणा HAVE_BUILTIN_IA32_PAUSE
#पूर्ण_अगर
#पूर्ण_अगर

#पूर्ण_अगर /* !__INTEL_COMPILER */

अटल CURL_INLINE व्योम curl_simple_lock_lock(curl_simple_lock *lock)
{
  क्रम(;;) {
    अगर(!atomic_exchange_explicit(lock, true, memory_order_acquire))
      अवरोध;
    /* Reduce cache coherency traffic */
    जबतक(atomic_load_explicit(lock, memory_order_relaxed)) {
      /* Reduce load (not mandatory) */
#अगर_घोषित HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#या_अगर defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ अस्थिर("yield" ::: "memory");
#या_अगर defined(HAVE_SCHED_YIELD)
      sched_yield();
#पूर्ण_अगर
    }
  }
}

अटल CURL_INLINE व्योम curl_simple_lock_unlock(curl_simple_lock *lock)
{
  atomic_store_explicit(lock, false, memory_order_release);
}

#या_अगर defined(HAVE_THREADS_POSIX)

#घोषणा curl_simple_lock           pthread_mutex_t
#घोषणा CURL_SIMPLE_LOCK_INIT      PTHREAD_MUTEX_INITIALIZER
#घोषणा curl_simple_lock_lock(m)   pthread_mutex_lock(m)
#घोषणा curl_simple_lock_unlock(m) pthread_mutex_unlock(m)

#अन्यथा

#अघोषित GLOBAL_INIT_IS_THREADSAFE

#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_EASY_LOCK_H */
