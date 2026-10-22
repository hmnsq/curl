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

#ifdef CURL_MEMDEBUG

#ಅನ್ತರ್ಭಾವ <stddef.h>  /* for offsetof() */

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#ಅನ್ತರ್ಭಾವ <backtrace.h>
#endif

ಸಂರಚನಾ memdebug {
  size_t size;
  ಸಙ್ಘ {
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

FILE *curl_dbg_logfile = ರಿಕ್ತ;
ಸ್ಥಿರ bool registered_cleanup = FALSE; /* atexit registered cleanup */
ಸ್ಥಿರ bool memlimit = FALSE; /* enable memory limit */
ಸ್ಥಿರ long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
ಸ್ಥಿರ ಸಂರಚನಾ backtrace_state *btstate;
#endif

ಸ್ಥಿರ ವರ್ಣ membuf[10000];
ಸ್ಥಿರ size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
ಸ್ಥಿರ bool dbg_mutex_init = 0;
ಸ್ಥಿರ curl_mutex_t dbg_mutex;
#endif

ಸ್ಥಿರ bool curl_dbg_lock(ಶೂನ್ಯ)
{
#ifdef USE_MUTEX
  ಯದಿ(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
#endif
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  ಯದಿ(was_locked)
    Curl_mutex_release(&dbg_mutex);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)was_locked;
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_log_locked(ಧ್ರುವ ವರ್ಣ *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_cleanup(ಶೂನ್ಯ)
{
  bool locked = curl_dbg_lock();
  ಯದಿ(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    ಯದಿ(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = ರಿಕ್ತ;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  ಯದಿ(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
ಸ್ಥಿರ ಶೂನ್ಯ error_bt_callback(ಶೂನ್ಯ *data, ಧ್ರುವ ವರ್ಣ *message,
                              ಪೂರ್ಣಾಙ್ಕ error_ಸಂಖ್ಯೆ)
{
  (ಶೂನ್ಯ)data;
  ಯದಿ(error_ಸಂಖ್ಯೆ == -1)
    curl_dbg_log("compile with -g\n\n");
  ಅನ್ಯಥಾ
    curl_dbg_log("Backtrace error %d: %s\n", error_ಸಂಖ್ಯೆ, message);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ full_callback(ಶೂನ್ಯ *data, uintptr_t pc, ಧ್ರುವ ವರ್ಣ *pathname,
                         ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_ಸಂಖ್ಯೆ, ಧ್ರುವ ವರ್ಣ *function)
{
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)pc;
  ಯದಿ(pathname || function || ಸಾಲು_ಸಂಖ್ಯೆ)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, ಸಾಲು_ಸಂಖ್ಯೆ, function);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ dump_bt(ಶೂನ್ಯ)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, ರಿಕ್ತ);
}
#ಅನ್ಯಥಾ
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
ಶೂನ್ಯ curl_dbg_memdebug(ಧ್ರುವ ವರ್ಣ *logname)
{
  ಯದಿ(!curl_dbg_logfile) {
    ಯದಿ(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_ಕೆಳಗಿನ(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    ಯದಿ(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (ವರ್ಣ *)ರಿಕ್ತ);
#endif
  }
#ifdef USE_MUTEX
  ಯದಿ(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(ರಿಕ್ತ, 0, error_bt_callback, ರಿಕ್ತ);
#endif
  ಯದಿ(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
ಶೂನ್ಯ curl_dbg_memlimit(long limit)
{
  ಯದಿ(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
ಸ್ಥಿರ bool countcheck(ಧ್ರುವ ವರ್ಣ *func, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  ಯದಿ(memlimit && source) {
    ಯದಿ(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, ಸಾಲು, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, ಸಾಲು, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      ಪ್ರತ್ಯಾಗಮ TRUE; /* RETURN ERROR! */
    }
    ಅನ್ಯಥಾ
      memsize--; /* countdown */
  }

  ಪ್ರತ್ಯಾಗಮ FALSE; /* allow this */
}

ALLOC_FUNC
ಶೂನ್ಯ *curl_dbg_malloc(size_t wantedsize, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  ಸಂರಚನಾ memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  ಯದಿ(countcheck("malloc", ಸಾಲು, source))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* alloc at least 64 bytes */
  size = ಪರಿಮಾಣ(ಸಂರಚನಾ memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  ಯದಿ(mem) {
    mem->size = wantedsize;
  }

  ಯದಿ(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, ಸಾಲು, wantedsize,
                 mem ? (ಶೂನ್ಯ *)mem->mem : (ಶೂನ್ಯ *)0);

  ಪ್ರತ್ಯಾಗಮ mem ? mem->mem : ರಿಕ್ತ;
}

ALLOC_FUNC
ಶೂನ್ಯ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  ಸಂರಚನಾ memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  ಯದಿ(countcheck("calloc", ಸಾಲು, source))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = ಪರಿಮಾಣ(ಸಂರಚನಾ memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  ಯದಿ(mem)
    mem->size = user_size;

  ಯದಿ(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, ಸಾಲು, wanted_elements, wanted_size,
                 mem ? (ಶೂನ್ಯ *)mem->mem : (ಶೂನ್ಯ *)0);

  ಪ್ರತ್ಯಾಗಮ mem ? mem->mem : ರಿಕ್ತ;
}

ALLOC_FUNC
ವರ್ಣ *curl_dbg_strdup(ಧ್ರುವ ವರ್ಣ *str, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  ವರ್ಣ *mem;
  size_t len;

  DEBUGASSERT(str);

  ಯದಿ(countcheck("strdup", ಸಾಲು, source))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, ರಿಕ್ತ); /* NULL prevents logging */
  ಯದಿ(mem)
    memcpy(mem, str, len);

  ಯದಿ(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, ಸಾಲು, (ಧ್ರುವ ಶೂನ್ಯ *)str, len, (ಧ್ರುವ ಶೂನ್ಯ *)mem);

  ಪ್ರತ್ಯಾಗಮ mem;
}

#ಯದಿ defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ಧ್ರುವ wchar_t *str, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  ಯದಿ(countcheck("wcsdup", ಸಾಲು, source))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * ಪರಿಮಾಣ(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, ರಿಕ್ತ); /* NULL prevents logging */
  ಯದಿ(mem)
    memcpy(mem, str, bsiz);

  ಯದಿ(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, ಸಾಲು, (ಧ್ರುವ ಶೂನ್ಯ *)str, bsiz, (ಶೂನ್ಯ *)mem);

  ಪ್ರತ್ಯಾಗಮ mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
ಶೂನ್ಯ *curl_dbg_realloc(ಶೂನ್ಯ *ptr, size_t wantedsize,
                       ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  ಸಂರಚನಾ memdebug *mem = ರಿಕ್ತ;
  bool was_locked;

  size_t size = ಪರಿಮಾಣ(ಸಂರಚನಾ memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  ಯದಿ(countcheck("realloc", ಸಾಲು, source))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(ತಳ್ಳು)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  ಯದಿ(ptr)
    mem = (ಶೂನ್ಯ *)((ವರ್ಣ *)ptr - offsetof(ಸಂರಚನಾ memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(ತೆಗೆ)
#endif

  mem = Curl_crealloc(mem, size);
  ಯದಿ(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, ಸಾಲು, (ಶೂನ್ಯ *)ptr, wantedsize,
                        mem ? (ಶೂನ್ಯ *)mem->mem : (ಶೂನ್ಯ *)0);

  curl_dbg_unlock(was_locked);
  ಯದಿ(mem) {
    mem->size = wantedsize;
    ಪ್ರತ್ಯಾಗಮ mem->mem;
  }

  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ curl_dbg_free(ಶೂನ್ಯ *ptr, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  ಯದಿ(ptr) {
    ಸಂರಚನಾ memdebug *mem;

    ಯದಿ(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, ಸಾಲು, (ಶೂನ್ಯ *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(ತಳ್ಳು)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (ಶೂನ್ಯ *)((ವರ್ಣ *)ptr - offsetof(ಸಂರಚನಾ memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(ತೆಗೆ)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(ಪೂರ್ಣಾಙ್ಕ domain, ಪೂರ್ಣಾಙ್ಕ type, ಪೂರ್ಣಾಙ್ಕ protocol,
                              ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  curl_socket_t sockfd;

  ಯದಿ(countcheck("socket", ಸಾಲು, source))
    ಪ್ರತ್ಯಾಗಮ CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  ಯದಿ(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, ಸಾಲು, sockfd);

  ಪ್ರತ್ಯಾಗಮ sockfd;
}

#ifdef HAVE_SOCKETPAIR
ಪೂರ್ಣಾಙ್ಕ curl_dbg_socketpair(ಪೂರ್ಣಾಙ್ಕ domain, ಪೂರ್ಣಾಙ್ಕ type, ಪೂರ್ಣಾಙ್ಕ protocol,
                        curl_socket_t socket_vector[2],
                        ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  ಪೂರ್ಣಾಙ್ಕ res = socketpair(domain, type, protocol, socket_vector);

  ಯದಿ(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, ಸಾಲು, socket_vector[0], socket_vector[1]);

  ಪ್ರತ್ಯಾಗಮ res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, ಶೂನ್ಯ *saddr, ಶೂನ್ಯ *saddrlen,
                              ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  ಸಂರಚನಾ sockaddr *addr = (ಸಂರಚನಾ sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  ಯದಿ(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, ಸಾಲು, sockfd);

  ಪ್ರತ್ಯಾಗಮ sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, ಶೂನ್ಯ *saddr, ಶೂನ್ಯ *saddrlen,
                               ಪೂರ್ಣಾಙ್ಕ flags,
                               ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  ಸಂರಚನಾ sockaddr *addr = (ಸಂರಚನಾ sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  ಯದಿ(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, ಸಾಲು, sockfd);

  ಪ್ರತ್ಯಾಗಮ sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
ಶೂನ್ಯ curl_dbg_mark_sclose(curl_socket_t sockfd, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  ಯದಿ(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, ಸಾಲು, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
ಪೂರ್ಣಾಙ್ಕ curl_dbg_sclose(curl_socket_t sockfd, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  curl_dbg_mark_sclose(sockfd, ಸಾಲು, source);
  ಪ್ರತ್ಯಾಗಮ CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ಧ್ರುವ ವರ್ಣ *file, ಧ್ರುವ ವರ್ಣ *mode,
                     ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  FILE *res = CURLX_FOPEN_ಕೆಳಗಿನ(file, mode);
  ಯದಿ(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, ಸಾಲು, file, mode, (ಶೂನ್ಯ *)res);

  ಪ್ರತ್ಯಾಗಮ res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ಧ್ರುವ ವರ್ಣ *file, ಧ್ರುವ ವರ್ಣ *mode, FILE *fh,
                       ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  FILE *res = CURLX_FREOPEN_ಕೆಳಗಿನ(file, mode, fh);
  ಯದಿ(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, ಸಾಲು, file, mode, (ಶೂನ್ಯ *)fh, (ಶೂನ್ಯ *)res);

  ಪ್ರತ್ಯಾಗಮ res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(ಪೂರ್ಣಾಙ್ಕ filedes, ಧ್ರುವ ವರ್ಣ *mode,
                      ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  FILE *res = CURLX_FDOPEN_ಕೆಳಗಿನ(filedes, mode);
  ಯದಿ(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, ಸಾಲು, filedes, mode, (ಶೂನ್ಯ *)res);
  ಪ್ರತ್ಯಾಗಮ res;
}

ಪೂರ್ಣಾಙ್ಕ curl_dbg_fclose(FILE *file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *source)
{
  ಪೂರ್ಣಾಙ್ಕ res;

  DEBUGASSERT(file);

  ಯದಿ(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, ಸಾಲು, (ಶೂನ್ಯ *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  ಪ್ರತ್ಯಾಗಮ res;
}

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_vlog(ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ fmt,
                          va_list ap) CURL_PRINTF(1, 0);

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_vlog(ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ fmt, va_list ap)
{
  ವರ್ಣ buf[1024];
  size_t nchars = curl_mvsnprintf(buf, ಪರಿಮಾಣ(buf), fmt, ap);

  ಯದಿ(nchars > (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf) - 1)
    nchars = (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf) - 1;

  ಯದಿ(nchars > 0) {
    ಯದಿ(ಪರಿಮಾಣ(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    ಯದಿ(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ curl_dbg_log_locked(ಧ್ರುವ ವರ್ಣ *format, ...)
{
  va_list ap;

  ಯದಿ(!curl_dbg_logfile)
    ಪ್ರತ್ಯಾಗಮ;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
ಶೂನ್ಯ curl_dbg_log(ಧ್ರುವ ವರ್ಣ *format, ...)
{
  bool was_locked;
  va_list ap;

  ಯದಿ(!curl_dbg_logfile)
    ಪ್ರತ್ಯಾಗಮ;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
