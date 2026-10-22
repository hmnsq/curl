#నిర్వచితం_కాకపోతే శీర్షిక_CURL_EASY_తాళం_H
#నిర్వచించు శీర్షిక_CURL_EASY_తాళం_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచించు GLOBAL_INIT_IS_THREADSAFE

#నిర్వచితమైతే _WIN32

#నిర్వచించు curl_simple_తాళం      SRWLOCK
#నిర్వచించు CURL_SIMPLE_తాళం_INIT SRWLOCK_INIT

#నిర్వచించు curl_simple_తాళం_తాళం(m)   AcquireSRWLockExclusive(m)
#నిర్వచించు curl_simple_తాళం_unlock(m) ReleaseSRWLockExclusive(m)

#లేదావేళ defined(HAVE_ATOMIC) && defined(HAVE_STDATOMIC_H)
#అన్తర్భావ <stdatomic.h>
#నిర్వచితమైతే HAVE_SCHED_YIELD
#అన్తర్భావ <sched.h>
#అయితే_ముగింపు

#నిర్వచించు curl_simple_తాళం      atomic_int
#నిర్వచించు CURL_SIMPLE_తాళం_INIT 0

#నిర్వచితం_కాకపోతే __INTEL_COMPILER
/* The Intel compiler tries to look like GCC *and* clang *and* lies in its
   __has_builtin() function, so override it. */

/* if GCC on i386/x86_64 or if the built-in is present */
#ఒకవేళ (defined(__GNUC__) && !defined(__clang__)) && \
  (defined(__i386__) || defined(__x86_64__))
#నిర్వచించు HAVE_BUILTIN_IA32_PAUSE
#లేదావేళ defined(__has_builtin)  /* Keep this PP check separate from others */
#ఒకవేళ __has_builtin(__builtin_ia32_pause)
#నిర్వచించు HAVE_BUILTIN_IA32_PAUSE
#అయితే_ముగింపు
#అయితే_ముగింపు

#అయితే_ముగింపు /* !__INTEL_COMPILER */

స్థిర CURL_INLINE శూన్య curl_simple_తాళం_తాళం(curl_simple_తాళం *తాళం)
{
  కొరకు(;;) {
    ఒకవేళ(!atomic_exchange_explicit(తాళం, true, జ్ఞాపకం_క్రమం_acquire))
      ఆపు;
    /* Reduce cache coherency traffic */
    అయితే(atomic_load_explicit(తాళం, జ్ఞాపకం_క్రమం_relaxed)) {
      /* Reduce load (not mandatory) */
#నిర్వచితమైతే HAVE_BUILTIN_IA32_PAUSE
      __builtin_ia32_pause();
#లేదావేళ defined(__aarch64__)
      /* NOLINTNEXTLINE(portability-no-assembler) */
      __asm__ అస్థిర("yield" ::: "memory");
#లేదావేళ defined(HAVE_SCHED_YIELD)
      sched_yield();
#అయితే_ముగింపు
    }
  }
}

స్థిర CURL_INLINE శూన్య curl_simple_తాళం_unlock(curl_simple_తాళం *తాళం)
{
  atomic_store_explicit(తాళం, false, జ్ఞాపకం_క్రమం_release);
}

#లేదావేళ defined(HAVE_దారాలు_POSIX)

#నిర్వచించు curl_simple_తాళం           pthread_mutex_t
#నిర్వచించు CURL_SIMPLE_తాళం_INIT      PTHREAD_MUTEX_INITIALIZER
#నిర్వచించు curl_simple_తాళం_తాళం(m)   pthread_mutex_తాళం(m)
#నిర్వచించు curl_simple_తాళం_unlock(m) pthread_mutex_unlock(m)

#లేకపోతే

#నిర్వచనం_రద్దు GLOBAL_INIT_IS_THREADSAFE

#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_EASY_LOCK_H */
