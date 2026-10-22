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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_MEMDEBUG

#ಅನ್ತರ್ಭಾವ <stddef.h>  /* for offsetof() */

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_BACKTRACE
#ಅನ್ತರ್ಭಾವ <backtrace.h>
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ memdebug {
  size_t ಗಾತ್ರ_2;
  ಒಕ್ಕೂಟ {
    curl_off_t o;
    ದ್ವಿಗುಣ d;
    ಶೂನ್ಯ *p;
  } mem[1];
  /* I am hoping this is the thing with the strictest alignment
   * requirements. That also means we waste some space :-( */
};

/*
 * Note that these debug functions are simple and they are meant to remain so.
 * For advanced analysis, record a log file and write perl scripts to analyze
 * them!
 *
 * Do not use these with multi-threaded test programs!
 */

FILE *curl_dbg_logfile = ಖಾಲಿ;
ಸ್ಥಿರ bool registered_cleanup = FALSE; /* atexit registered cleanup */
ಸ್ಥಿರ bool memlimit = FALSE; /* enable memory limit */
ಸ್ಥಿರ ದೀರ್ಘ memsize = 0;  /* set number of mallocs allowed */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_BACKTRACE
ಸ್ಥಿರ ರಚನೆ backtrace_state *btstate;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಅಕ್ಷರ membuf[10000];
ಸ್ಥಿರ size_t memwidx = 0; /* write index */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
ಸ್ಥಿರ bool dbg_mutex_init = 0;
ಸ್ಥಿರ curl_mutex_t dbg_mutex;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ bool curl_dbg_ಬೀಗ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  ಆದರೆ(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    ಹಿಂತಿರುಗಿ TRUE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_unlock(bool was_locked)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  ಆದರೆ(was_locked)
    Curl_mutex_release(&dbg_mutex);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)was_locked;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_log_locked(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_cleanup(ಶೂನ್ಯ)
{
  bool locked = curl_dbg_ಬೀಗ();
  ಆದರೆ(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    ಆದರೆ(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = ಖಾಲಿ;
  curl_dbg_unlock(locked);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  ಆದರೆ(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_BACKTRACE
ಸ್ಥಿರ ಶೂನ್ಯ error_bt_callback(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ_2,
                              ಪೂರ್ಣಾಂಕ error_ಸಂಖ್ಯೆ)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(error_ಸಂಖ್ಯೆ == -1)
    curl_dbg_log("compile with -g\n\n");
  ಇಲ್ಲದಿದ್ದರೆ
    curl_dbg_log("Backtrace error %d: %s\n", error_ಸಂಖ್ಯೆ, ಸಂದೇಶ_2);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತುಂಬಿದ_callback(ಶೂನ್ಯ *ದತ್ತಾಂಶ, uintptr_t pc, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathname,
                         ಪೂರ್ಣಾಂಕ ಸಾಲು_ಸಂಖ್ಯೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *function)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)pc;
  ಆದರೆ(pathname || function || ಸಾಲು_ಸಂಖ್ಯೆ)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, ಸಾಲು_ಸಂಖ್ಯೆ, function);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ dump_bt(ಶೂನ್ಯ)
{
  backtrace_ತುಂಬಿದ(btstate, 0, ತುಂಬಿದ_callback, error_bt_callback, ಖಾಲಿ);
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು dump_bt() /* nothing to do */
#ಆದರೆ_ಅಂತ್ಯ

/* this sets the log filename */
ಶೂನ್ಯ curl_dbg_memdebug(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *logname)
{
  ಆದರೆ(!curl_dbg_logfile) {
    ಆದರೆ(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_ಕೆಳಗಿನ(logname, FOPEN_WRITETEXT);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    ಆದರೆ(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (ಅಕ್ಷರ *)ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  ಆದರೆ(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_BACKTRACE
  btstate = backtrace_create_state(ಖಾಲಿ, 0, error_bt_callback, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
ಶೂನ್ಯ curl_dbg_memlimit(ದೀರ್ಘ ಎಲ್ಲೆ)
{
  ಆದರೆ(!memlimit) {
    memlimit = TRUE;
    memsize = ಎಲ್ಲೆ;
  }
}

/* returns TRUE if this is not allowed! */
ಸ್ಥಿರ bool countcheck(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  ಆದರೆ(memlimit && ಮೂಲ) {
    ಆದರೆ(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", ಮೂಲ, ಸಾಲು, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    ಮೂಲ, ಸಾಲು, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      ಹಿಂತಿರುಗಿ TRUE; /* RETURN ERROR! */
    }
    ಇಲ್ಲದಿದ್ದರೆ
      memsize--; /* countdown */
  }

  ಹಿಂತಿರುಗಿ FALSE; /* allow this */
}

ALLOC_FUNC
ಶೂನ್ಯ *curl_dbg_malloc(size_t wantedsize, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  ರಚನೆ memdebug *mem;
  size_t ಗಾತ್ರ_2;

  DEBUGASSERT(wantedsize != 0);

  ಆದರೆ(countcheck("malloc", ಸಾಲು, ಮೂಲ))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* alloc at least 64 bytes */
  ಗಾತ್ರ_2 = ಗಾತ್ರ(ರಚನೆ memdebug) + wantedsize;

  mem = Curl_cmalloc(ಗಾತ್ರ_2);
  ಆದರೆ(mem) {
    mem->ಗಾತ್ರ_2 = wantedsize;
  }

  ಆದರೆ(ಮೂಲ)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 ಮೂಲ, ಸಾಲು, wantedsize,
                 mem ? (ಶೂನ್ಯ *)mem->mem : (ಶೂನ್ಯ *)0);

  ಹಿಂತಿರುಗಿ mem ? mem->mem : ಖಾಲಿ;
}

ALLOC_FUNC
ಶೂನ್ಯ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_ಗಾತ್ರ,
                      ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  ರಚನೆ memdebug *mem;
  size_t ಗಾತ್ರ_2, user_ಗಾತ್ರ;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_ಗಾತ್ರ != 0);

  ಆದರೆ(countcheck("calloc", ಸಾಲು, ಮೂಲ))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* alloc at least 64 bytes */
  user_ಗಾತ್ರ = wanted_ಗಾತ್ರ * wanted_elements;
  ಗಾತ್ರ_2 = ಗಾತ್ರ(ರಚನೆ memdebug) + user_ಗಾತ್ರ;

  mem = Curl_ccalloc(1, ಗಾತ್ರ_2);
  ಆದರೆ(mem)
    mem->ಗಾತ್ರ_2 = user_ಗಾತ್ರ;

  ಆದರೆ(ಮೂಲ)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 ಮೂಲ, ಸಾಲು, wanted_elements, wanted_ಗಾತ್ರ,
                 mem ? (ಶೂನ್ಯ *)mem->mem : (ಶೂನ್ಯ *)0);

  ಹಿಂತಿರುಗಿ mem ? mem->mem : ಖಾಲಿ;
}

ALLOC_FUNC
ಅಕ್ಷರ *curl_dbg_strdup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  ಅಕ್ಷರ *mem;
  size_t len;

  DEBUGASSERT(str);

  ಆದರೆ(countcheck("strdup", ಸಾಲು, ಮೂಲ))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str) + 1;

  mem = curl_dbg_malloc(len, 0, ಖಾಲಿ); /* NULL prevents logging */
  ಆದರೆ(mem)
    ಸ್ಮರಣೆ_ನಕಲು(mem, str, len);

  ಆದರೆ(ಮೂಲ)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 ಮೂಲ, ಸಾಲು, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)str, len, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)mem);

  ಹಿಂತಿರುಗಿ mem;
}

#ಆದರೆ defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ಸ್ಥಿರಾಂಕ wchar_t *str, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  ಆದರೆ(countcheck("wcsdup", ಸಾಲು, ಮೂಲ))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * ಗಾತ್ರ(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, ಖಾಲಿ); /* NULL prevents logging */
  ಆದರೆ(mem)
    ಸ್ಮರಣೆ_ನಕಲು(mem, str, bsiz);

  ಆದರೆ(ಮೂಲ)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                ಮೂಲ, ಸಾಲು, (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)str, bsiz, (ಶೂನ್ಯ *)mem);

  ಹಿಂತಿರುಗಿ mem;
}
#ಆದರೆ_ಅಂತ್ಯ

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
ಶೂನ್ಯ *curl_dbg_realloc(ಶೂನ್ಯ *ptr, size_t wantedsize,
                       ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  ರಚನೆ memdebug *mem = ಖಾಲಿ;
  bool was_locked;

  size_t ಗಾತ್ರ_2 = ಗಾತ್ರ(ರಚನೆ memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  ಆದರೆ(countcheck("realloc", ಸಾಲು, ಮೂಲ))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_ಬೀಗ();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __INTEL_COMPILER
#  pragma warning(ತಳ್ಳು)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(ptr)
    mem = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ptr - offsetof(ರಚನೆ memdebug, mem));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __INTEL_COMPILER
#  pragma warning(ತೆಗೆ)
#ಆದರೆ_ಅಂತ್ಯ

  mem = Curl_crealloc(mem, ಗಾತ್ರ_2);
  ಆದರೆ(ಮೂಲ)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        ಮೂಲ, ಸಾಲು, (ಶೂನ್ಯ *)ptr, wantedsize,
                        mem ? (ಶೂನ್ಯ *)mem->mem : (ಶೂನ್ಯ *)0);

  curl_dbg_unlock(was_locked);
  ಆದರೆ(mem) {
    mem->ಗಾತ್ರ_2 = wantedsize;
    ಹಿಂತಿರುಗಿ mem->mem;
  }

  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ curl_dbg_free(ಶೂನ್ಯ *ptr, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  ಆದರೆ(ptr) {
    ರಚನೆ memdebug *mem;

    ಆದರೆ(ಮೂಲ)
      curl_dbg_log("MEM %s:%d free(%p)\n", ಮೂಲ, ಸಾಲು, (ಶೂನ್ಯ *)ptr);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __INTEL_COMPILER
#  pragma warning(ತಳ್ಳು)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#ಆದರೆ_ಅಂತ್ಯ

    mem = (ಶೂನ್ಯ *)((ಅಕ್ಷರ *)ptr - offsetof(ರಚನೆ memdebug, mem));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __INTEL_COMPILER
#  pragma warning(ತೆಗೆ)
#ಆದರೆ_ಅಂತ್ಯ

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(ಪೂರ್ಣಾಂಕ domain, ಪೂರ್ಣಾಂಕ type, ಪೂರ್ಣಾಂಕ protocol,
                              ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  curl_socket_t sockfd;

  ಆದರೆ(countcheck("socket", ಸಾಲು, ಮೂಲ))
    ಹಿಂತಿರುಗಿ CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  ಆದರೆ(ಮೂಲ && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 ಮೂಲ, ಸಾಲು, sockfd);

  ಹಿಂತಿರುಗಿ sockfd;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SOCKETPAIR
ಪೂರ್ಣಾಂಕ curl_dbg_socketpair(ಪೂರ್ಣಾಂಕ domain, ಪೂರ್ಣಾಂಕ type, ಪೂರ್ಣಾಂಕ protocol,
                        curl_socket_t socket_ಸದಿಶ[2],
                        ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  ಪೂರ್ಣಾಂಕ res = socketpair(domain, type, protocol, socket_ಸದಿಶ);

  ಆದರೆ(ಮೂಲ && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 ಮೂಲ, ಸಾಲು, socket_ಸದಿಶ[0], socket_ಸದಿಶ[1]);

  ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ

curl_socket_t curl_dbg_accept(curl_socket_t s, ಶೂನ್ಯ *saddr, ಶೂನ್ಯ *saddrlen,
                              ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  ರಚನೆ sockaddr *addr = (ರಚನೆ sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  ಆದರೆ(ಮೂಲ && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ಮೂಲ, ಸಾಲು, sockfd);

  ಹಿಂತಿರುಗಿ sockfd;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, ಶೂನ್ಯ *saddr, ಶೂನ್ಯ *saddrlen,
                               ಪೂರ್ಣಾಂಕ flags,
                               ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  ರಚನೆ sockaddr *addr = (ರಚನೆ sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  ಆದರೆ(ಮೂಲ && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ಮೂಲ, ಸಾಲು, sockfd);

  ಹಿಂತಿರುಗಿ sockfd;
}
#ಆದರೆ_ಅಂತ್ಯ

/* separate function to allow libcurl to mark a "faked" close */
ಶೂನ್ಯ curl_dbg_mark_sclose(curl_socket_t sockfd, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  ಆದರೆ(ಮೂಲ)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 ಮೂಲ, ಸಾಲು, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
ಪೂರ್ಣಾಂಕ curl_dbg_sclose(curl_socket_t sockfd, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  curl_dbg_mark_sclose(sockfd, ಸಾಲು, ಮೂಲ);
  ಹಿಂತಿರುಗಿ CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ,
                     ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  FILE *res = CURLX_FOPEN_ಕೆಳಗಿನ(file, ಬಹುಲಕ);
  ಆದರೆ(ಮೂಲ)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 ಮೂಲ, ಸಾಲು, file, ಬಹುಲಕ, (ಶೂನ್ಯ *)res);

  ಹಿಂತಿರುಗಿ res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE *fh,
                       ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  FILE *res = CURLX_FREOPEN_ಕೆಳಗಿನ(file, ಬಹುಲಕ, fh);
  ಆದರೆ(ಮೂಲ)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 ಮೂಲ, ಸಾಲು, file, ಬಹುಲಕ, (ಶೂನ್ಯ *)fh, (ಶೂನ್ಯ *)res);

  ಹಿಂತಿರುಗಿ res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(ಪೂರ್ಣಾಂಕ filedes, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ,
                      ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  FILE *res = CURLX_FDOPEN_ಕೆಳಗಿನ(filedes, ಬಹುಲಕ);
  ಆದರೆ(ಮೂಲ)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 ಮೂಲ, ಸಾಲು, filedes, ಬಹುಲಕ, (ಶೂನ್ಯ *)res);
  ಹಿಂತಿರುಗಿ res;
}

ಪೂರ್ಣಾಂಕ curl_dbg_fclose(FILE *file, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ)
{
  ಪೂರ್ಣಾಂಕ res;

  DEBUGASSERT(file);

  ಆದರೆ(ಮೂಲ)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", ಮೂಲ, ಸಾಲು, (ಶೂನ್ಯ *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  ಹಿಂತಿರುಗಿ res;
}

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_vlog(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ fmt,
                          va_list ap) CURL_PRINTF(1, 0);

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_vlog(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ fmt, va_list ap)
{
  ಅಕ್ಷರ buf[1024];
  size_t nchars = curl_mvsnprintf(buf, ಗಾತ್ರ(buf), fmt, ap);

  ಆದರೆ(nchars > (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf) - 1)
    nchars = (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf) - 1;

  ಆದರೆ(nchars > 0) {
    ಆದರೆ(ಗಾತ್ರ(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    ಆದರೆ(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    ಸ್ಮರಣೆ_ನಕಲು(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_log_locked(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
  va_list ap;

  ಆದರೆ(!curl_dbg_logfile)
    ಹಿಂತಿರುಗಿ;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
ಶೂನ್ಯ curl_dbg_log(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...)
{
  bool was_locked;
  va_list ap;

  ಆದರೆ(!curl_dbg_logfile)
    ಹಿಂತಿರುಗಿ;

  was_locked = curl_dbg_ಬೀಗ();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#ಆದರೆ_ಅಂತ್ಯ /* CURL_MEMDEBUG */
