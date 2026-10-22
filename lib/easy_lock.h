#اگر_غیر_تعریف_شدہ سرخی_CURL_EASY_تالا_H
#تعریف_کرو سرخی_CURL_EASY_تالا_H
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
#شامل "curl_setup.h"

#تعریف_کرو GLOBAL_INIT_IS_THREADSAFE

#اگر_تعریف_شدہ _WIN32

#تعریف_کرو curl_simple_تالا      SRWLOCK
#تعریف_کرو CURL_SIMPLE_تالا_INIT SRWLOCK_INIT

#تعریف_کرو curl_simple_تالا_تالا(m)   AcquireSRWLockExclusive(m)
#تعریف_کرو curl_simple_تالا_unlock(m) ReleaseSRWLockExclusive(m)

#وگرنہ_اگر defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#شامل <stdatomic.h>
#اگر_تعریف_شدہ HAVE_SCHED_YIELD
#شامل <sched.h>
#اگر_ختم

#تعریف_کرو curl_simple_تالا      atomic_int
#تعریف_کرو CURL_SIMPLE_تالا_INIT 0

#اگر_غیر_تعریف_شدہ __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#اگر (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#تعریف_کرو HAVE_BUILTIN_IA32_PAUSE
#وگرنہ_اگر defined(__has_builtin)  /* Keep this PP check separate from others */
#اگر __has_builtin(__builtin_ia32_pause)
#تعریف_کرو HAVE_BUILTIN_IA32_PAUSE
#اگر_ختم
#اگر_ختم

#اگر_ختم /* !__INTEL_COMPILER */

ساکن CURL_INLINE خالی curl_simple_تالا_تالا(curl_simple_تالا *تالا)
{
  برائے(;;) {
    اگر(!atomic_exchange_explicit(تالا, true, حافظہ_ترتیب_acquire))
      توڑو;
    /* Reduce cache coherency traffic */
    جب(atomic_load_explicit(تالا, حافظہ_ترتیب_relaxed)) {
      /* Reduce load (not mandatory) */
#اگر_تعریف_شدہ HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#وگرنہ_اگر defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ متلون("yield" ::: "memory");
#وگرنہ_اگر defined(HAVE_SCHED_YIELD)
      sched_yield();
#اگر_ختم
    }
  }
}

ساکن CURL_INLINE خالی curl_simple_تالا_unlock(curl_simple_تالا *تالا)
{
  atomic_store_explicit(تالا, false, حافظہ_ترتیب_release);
}

#وگرنہ_اگر defined(HAVE_دھاگے_POSIX)

#تعریف_کرو curl_simple_تالا           pthread_mutex_t
#تعریف_کرو CURL_SIMPLE_تالا_INIT      PTHREAD_MUTEX_INITIALIZER
#تعریف_کرو curl_simple_تالا_تالا(m)   pthread_mutex_تالا(m)
#تعریف_کرو curl_simple_تالا_unlock(m) pthread_mutex_unlock(m)

#ورنہ

#تعریف_منسوخ GLOBAL_INIT_IS_THREADSAFE

#اگر_ختم

#اگر_ختم /* HEADER_CURL_EASY_LOCK_H */
