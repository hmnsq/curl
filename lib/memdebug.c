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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_MEMDEBUG

#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>  /* for offsetof() */

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_threads.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_BACKTRACE
#ਅਨ੍ਤਰ੍ਭਾਵ <backtrace.h>
#ਜੇ_ਅੰਤ

ਬਣਤਰ memdebug {
  size_t ਮਾਪ;
  ਸੰਘ {
    curl_off_t o;
    ਦੁੱਗਣਾ d;
    ਖਾਲੀ *p;
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

FILE *curl_dbg_logfile = ਨਿਰਾ;
ਸਥਿਰ bool registered_cleanup = FALSE; /* atexit registered cleanup */
ਸਥਿਰ bool memlimit = FALSE; /* enable memory limit */
ਸਥਿਰ ਲੰਮਾ memsize = 0;  /* set number of mallocs allowed */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_BACKTRACE
ਸਥਿਰ ਬਣਤਰ backtrace_state *btstate;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅੱਖਰ membuf[10000];
ਸਥਿਰ size_t memwidx = 0; /* write index */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
ਸਥਿਰ bool dbg_mutex_init = 0;
ਸਥਿਰ curl_mutex_t dbg_mutex;
#ਜੇ_ਅੰਤ

ਸਥਿਰ bool curl_dbg_ਤਾਲਾ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  ਜੇ(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    ਵਾਪਸ TRUE;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਖਾਲੀ curl_dbg_unlock(bool was_locked)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  ਜੇ(was_locked)
    Curl_mutex_release(&dbg_mutex);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)was_locked;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ curl_dbg_log_locked(ਅਚਲ ਅੱਖਰ *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ਸਥਿਰ ਖਾਲੀ curl_dbg_cleanup(ਖਾਲੀ)
{
  bool locked = curl_dbg_ਤਾਲਾ();
  ਜੇ(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    ਜੇ(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = ਨਿਰਾ;
  curl_dbg_unlock(locked);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  ਜੇ(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_BACKTRACE
ਸਥਿਰ ਖਾਲੀ error_bt_callback(ਖਾਲੀ *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ_2,
                              ਪੂਰਨ error_ਸੰਖਿਆ)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(error_ਸੰਖਿਆ == -1)
    curl_dbg_log("compile with -g\n\n");
  ਨਹੀਂਤਾਂ
    curl_dbg_log("Backtrace error %d: %s\n", error_ਸੰਖਿਆ, ਸੁਨੇਹਾ_2);
}

ਸਥਿਰ ਪੂਰਨ ਭਰਿਆ_callback(ਖਾਲੀ *ਅੰਕੜੇ, uintptr_t pc, ਅਚਲ ਅੱਖਰ *pathname,
                         ਪੂਰਨ ਸਤਰ_ਸੰਖਿਆ, ਅਚਲ ਅੱਖਰ *function)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)pc;
  ਜੇ(pathname || function || ਸਤਰ_ਸੰਖਿਆ)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, ਸਤਰ_ਸੰਖਿਆ, function);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ dump_bt(ਖਾਲੀ)
{
  backtrace_ਭਰਿਆ(btstate, 0, ਭਰਿਆ_callback, error_bt_callback, ਨਿਰਾ);
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dump_bt() /* nothing to do */
#ਜੇ_ਅੰਤ

/* this sets the log filename */
ਖਾਲੀ curl_dbg_memdebug(ਅਚਲ ਅੱਖਰ *logname)
{
  ਜੇ(!curl_dbg_logfile) {
    ਜੇ(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_ਨੀਵਾਂ(logname, FOPEN_WRITETEXT);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    ਜੇ(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (ਅੱਖਰ *)ਨਿਰਾ);
#ਜੇ_ਅੰਤ
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  ਜੇ(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_BACKTRACE
  btstate = backtrace_create_state(ਨਿਰਾ, 0, error_bt_callback, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
  ਜੇ(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
ਖਾਲੀ curl_dbg_memlimit(ਲੰਮਾ ਹੱਦਬੰਦੀ)
{
  ਜੇ(!memlimit) {
    memlimit = TRUE;
    memsize = ਹੱਦਬੰਦੀ;
  }
}

/* returns TRUE if this is not allowed! */
ਸਥਿਰ bool countcheck(ਅਚਲ ਅੱਖਰ *func, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  ਜੇ(memlimit && ਸਰੋਤ) {
    ਜੇ(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", ਸਰੋਤ, ਸਤਰ, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    ਸਰੋਤ, ਸਤਰ, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      ਵਾਪਸ TRUE; /* RETURN ERROR! */
    }
    ਨਹੀਂਤਾਂ
      memsize--; /* countdown */
  }

  ਵਾਪਸ FALSE; /* allow this */
}

ALLOC_FUNC
ਖਾਲੀ *curl_dbg_malloc(size_t wantedsize, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  ਬਣਤਰ memdebug *mem;
  size_t ਮਾਪ;

  DEBUGASSERT(wantedsize != 0);

  ਜੇ(countcheck("malloc", ਸਤਰ, ਸਰੋਤ))
    ਵਾਪਸ ਨਿਰਾ;

  /* alloc at least 64 bytes */
  ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ memdebug) + wantedsize;

  mem = Curl_cmalloc(ਮਾਪ);
  ਜੇ(mem) {
    mem->ਮਾਪ = wantedsize;
  }

  ਜੇ(ਸਰੋਤ)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 ਸਰੋਤ, ਸਤਰ, wantedsize,
                 mem ? (ਖਾਲੀ *)mem->mem : (ਖਾਲੀ *)0);

  ਵਾਪਸ mem ? mem->mem : ਨਿਰਾ;
}

ALLOC_FUNC
ਖਾਲੀ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_ਮਾਪ,
                      ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  ਬਣਤਰ memdebug *mem;
  size_t ਮਾਪ, user_ਮਾਪ;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_ਮਾਪ != 0);

  ਜੇ(countcheck("calloc", ਸਤਰ, ਸਰੋਤ))
    ਵਾਪਸ ਨਿਰਾ;

  /* alloc at least 64 bytes */
  user_ਮਾਪ = wanted_ਮਾਪ * wanted_elements;
  ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ memdebug) + user_ਮਾਪ;

  mem = Curl_ccalloc(1, ਮਾਪ);
  ਜੇ(mem)
    mem->ਮਾਪ = user_ਮਾਪ;

  ਜੇ(ਸਰੋਤ)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 ਸਰੋਤ, ਸਤਰ, wanted_elements, wanted_ਮਾਪ,
                 mem ? (ਖਾਲੀ *)mem->mem : (ਖਾਲੀ *)0);

  ਵਾਪਸ mem ? mem->mem : ਨਿਰਾ;
}

ALLOC_FUNC
ਅੱਖਰ *curl_dbg_strdup(ਅਚਲ ਅੱਖਰ *str, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  ਅੱਖਰ *mem;
  size_t len;

  DEBUGASSERT(str);

  ਜੇ(countcheck("strdup", ਸਤਰ, ਸਰੋਤ))
    ਵਾਪਸ ਨਿਰਾ;

  len = ਲੜੀ_ਲੰਬਾਈ(str) + 1;

  mem = curl_dbg_malloc(len, 0, ਨਿਰਾ); /* NULL prevents logging */
  ਜੇ(mem)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(mem, str, len);

  ਜੇ(ਸਰੋਤ)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 ਸਰੋਤ, ਸਤਰ, (ਅਚਲ ਖਾਲੀ *)str, len, (ਅਚਲ ਖਾਲੀ *)mem);

  ਵਾਪਸ mem;
}

#ਜੇ defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ਅਚਲ wchar_t *str, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  ਜੇ(countcheck("wcsdup", ਸਤਰ, ਸਰੋਤ))
    ਵਾਪਸ ਨਿਰਾ;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * ਆਕਾਰ(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, ਨਿਰਾ); /* NULL prevents logging */
  ਜੇ(mem)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(mem, str, bsiz);

  ਜੇ(ਸਰੋਤ)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                ਸਰੋਤ, ਸਤਰ, (ਅਚਲ ਖਾਲੀ *)str, bsiz, (ਖਾਲੀ *)mem);

  ਵਾਪਸ mem;
}
#ਜੇ_ਅੰਤ

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
ਖਾਲੀ *curl_dbg_realloc(ਖਾਲੀ *ptr, size_t wantedsize,
                       ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  ਬਣਤਰ memdebug *mem = ਨਿਰਾ;
  bool was_locked;

  size_t ਮਾਪ = ਆਕਾਰ(ਬਣਤਰ memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  ਜੇ(countcheck("realloc", ਸਤਰ, ਸਰੋਤ))
    ਵਾਪਸ ਨਿਰਾ;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_ਤਾਲਾ();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __INTEL_COMPILER
#  pragma warning(ਧੱਕੋ)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#ਜੇ_ਅੰਤ

  ਜੇ(ptr)
    mem = (ਖਾਲੀ *)((ਅੱਖਰ *)ptr - offsetof(ਬਣਤਰ memdebug, mem));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __INTEL_COMPILER
#  pragma warning(ਕੱਢੋ)
#ਜੇ_ਅੰਤ

  mem = Curl_crealloc(mem, ਮਾਪ);
  ਜੇ(ਸਰੋਤ)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        ਸਰੋਤ, ਸਤਰ, (ਖਾਲੀ *)ptr, wantedsize,
                        mem ? (ਖਾਲੀ *)mem->mem : (ਖਾਲੀ *)0);

  curl_dbg_unlock(was_locked);
  ਜੇ(mem) {
    mem->ਮਾਪ = wantedsize;
    ਵਾਪਸ mem->mem;
  }

  ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ curl_dbg_free(ਖਾਲੀ *ptr, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  ਜੇ(ptr) {
    ਬਣਤਰ memdebug *mem;

    ਜੇ(ਸਰੋਤ)
      curl_dbg_log("MEM %s:%d free(%p)\n", ਸਰੋਤ, ਸਤਰ, (ਖਾਲੀ *)ptr);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __INTEL_COMPILER
#  pragma warning(ਧੱਕੋ)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#ਜੇ_ਅੰਤ

    mem = (ਖਾਲੀ *)((ਅੱਖਰ *)ptr - offsetof(ਬਣਤਰ memdebug, mem));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __INTEL_COMPILER
#  pragma warning(ਕੱਢੋ)
#ਜੇ_ਅੰਤ

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(ਪੂਰਨ domain, ਪੂਰਨ type, ਪੂਰਨ protocol,
                              ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  curl_socket_t sockfd;

  ਜੇ(countcheck("socket", ਸਤਰ, ਸਰੋਤ))
    ਵਾਪਸ CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  ਜੇ(ਸਰੋਤ && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 ਸਰੋਤ, ਸਤਰ, sockfd);

  ਵਾਪਸ sockfd;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SOCKETPAIR
ਪੂਰਨ curl_dbg_socketpair(ਪੂਰਨ domain, ਪੂਰਨ type, ਪੂਰਨ protocol,
                        curl_socket_t socket_ਸਦਿਸ਼[2],
                        ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  ਪੂਰਨ res = socketpair(domain, type, protocol, socket_ਸਦਿਸ਼);

  ਜੇ(ਸਰੋਤ && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 ਸਰੋਤ, ਸਤਰ, socket_ਸਦਿਸ਼[0], socket_ਸਦਿਸ਼[1]);

  ਵਾਪਸ res;
}
#ਜੇ_ਅੰਤ

curl_socket_t curl_dbg_accept(curl_socket_t s, ਖਾਲੀ *saddr, ਖਾਲੀ *saddrlen,
                              ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  ਬਣਤਰ sockaddr *addr = (ਬਣਤਰ sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  ਜੇ(ਸਰੋਤ && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ਸਰੋਤ, ਸਤਰ, sockfd);

  ਵਾਪਸ sockfd;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, ਖਾਲੀ *saddr, ਖਾਲੀ *saddrlen,
                               ਪੂਰਨ flags,
                               ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  ਬਣਤਰ sockaddr *addr = (ਬਣਤਰ sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  ਜੇ(ਸਰੋਤ && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ਸਰੋਤ, ਸਤਰ, sockfd);

  ਵਾਪਸ sockfd;
}
#ਜੇ_ਅੰਤ

/* separate function to allow libcurl to mark a "faked" close */
ਖਾਲੀ curl_dbg_mark_sclose(curl_socket_t sockfd, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  ਜੇ(ਸਰੋਤ)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 ਸਰੋਤ, ਸਤਰ, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
ਪੂਰਨ curl_dbg_sclose(curl_socket_t sockfd, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  curl_dbg_mark_sclose(sockfd, ਸਤਰ, ਸਰੋਤ);
  ਵਾਪਸ CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ਅਚਲ ਅੱਖਰ *file, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ,
                     ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  FILE *res = CURLX_FOPEN_ਨੀਵਾਂ(file, ਬਹੁਲਕ);
  ਜੇ(ਸਰੋਤ)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 ਸਰੋਤ, ਸਤਰ, file, ਬਹੁਲਕ, (ਖਾਲੀ *)res);

  ਵਾਪਸ res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ਅਚਲ ਅੱਖਰ *file, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE *fh,
                       ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  FILE *res = CURLX_FREOPEN_ਨੀਵਾਂ(file, ਬਹੁਲਕ, fh);
  ਜੇ(ਸਰੋਤ)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 ਸਰੋਤ, ਸਤਰ, file, ਬਹੁਲਕ, (ਖਾਲੀ *)fh, (ਖਾਲੀ *)res);

  ਵਾਪਸ res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(ਪੂਰਨ filedes, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ,
                      ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  FILE *res = CURLX_FDOPEN_ਨੀਵਾਂ(filedes, ਬਹੁਲਕ);
  ਜੇ(ਸਰੋਤ)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 ਸਰੋਤ, ਸਤਰ, filedes, ਬਹੁਲਕ, (ਖਾਲੀ *)res);
  ਵਾਪਸ res;
}

ਪੂਰਨ curl_dbg_fclose(FILE *file, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *ਸਰੋਤ)
{
  ਪੂਰਨ res;

  DEBUGASSERT(file);

  ਜੇ(ਸਰੋਤ)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", ਸਰੋਤ, ਸਤਰ, (ਖਾਲੀ *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  ਵਾਪਸ res;
}

ਸਥਿਰ ਖਾਲੀ curl_dbg_vlog(ਅਚਲ ਅੱਖਰ * ਅਚਲ fmt,
                          va_list ap) CURL_PRINTF(1, 0);

ਸਥਿਰ ਖਾਲੀ curl_dbg_vlog(ਅਚਲ ਅੱਖਰ * ਅਚਲ fmt, va_list ap)
{
  ਅੱਖਰ buf[1024];
  size_t nchars = curl_mvsnprintf(buf, ਆਕਾਰ(buf), fmt, ap);

  ਜੇ(nchars > (ਪੂਰਨ)ਆਕਾਰ(buf) - 1)
    nchars = (ਪੂਰਨ)ਆਕਾਰ(buf) - 1;

  ਜੇ(nchars > 0) {
    ਜੇ(ਆਕਾਰ(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    ਜੇ(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

ਸਥਿਰ ਖਾਲੀ curl_dbg_log_locked(ਅਚਲ ਅੱਖਰ *format, ...)
{
  va_list ap;

  ਜੇ(!curl_dbg_logfile)
    ਵਾਪਸ;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
ਖਾਲੀ curl_dbg_log(ਅਚਲ ਅੱਖਰ *format, ...)
{
  bool was_locked;
  va_list ap;

  ਜੇ(!curl_dbg_logfile)
    ਵਾਪਸ;

  was_locked = curl_dbg_ਤਾਲਾ();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#ਜੇ_ਅੰਤ /* CURL_MEMDEBUG */
