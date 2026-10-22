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
#include "curl_setup.h"

#ifdef CURL_MEMDEBUG

#include <stddef.h>  /* for offsetof() */

#include "urldata.h"
#include "curl_threads.h"
#include "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#include <backtrace.h>
#endif

sa_nracanaa memdebug {
  size_t size;
  sa_ngha {
    curl_off_t o;
    dviguu_na d;
    void *p;
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

FILE *curl_dbg_logfile = NULL;
static bool registered_cleanup = FALSE; /* atexit registered cleanup */
static bool memlimit = FALSE; /* enable memory limit */
static long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
static sa_nracanaa backtrace_state *btstate;
#endif

static var_na membuf[10000];
static size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
static bool dbg_mutex_init = 0;
static curl_mutex_t dbg_mutex;
#endif

static bool curl_dbg_lock(void)
{
#ifdef USE_MUTEX
  ᰀᰧ(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    ᰀᰧᰌ TRUE;
  }
#endif
  ᰀᰧᰌ FALSE;
}

static void curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  ᰀᰧ(was_locked)
    Curl_mutex_release(&dbg_mutex);
#ᰀᰧᰀ
  (void)was_locked;
#endif
}

static void curl_dbg_log_locked(const var_na *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
static void curl_dbg_cleanup(void)
{
  bool locked = curl_dbg_lock();
  ᰀᰧ(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    ᰀᰧ(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = NULL;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  ᰀᰧ(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
static void error_bt_callback(void *data, const var_na *message,
                              puur_naa_nka error_number)
{
  (void)data;
  ᰀᰧ(error_number == -1)
    curl_dbg_log("compile with -g\n\n");
  ᰀᰧᰀ
    curl_dbg_log("Backtrace error %d: %s\n", error_number, message);
}

static puur_naa_nka full_callback(void *data, uintptr_t pc, const var_na *pathname,
                         puur_naa_nka line_number, const var_na *function)
{
  (void)data;
  (void)pc;
  ᰀᰧ(pathname || function || line_number)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, line_number, function);
  ᰀᰧᰌ 0;
}

static void dump_bt(void)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, NULL);
}
#ᰀᰧᰀ
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
void curl_dbg_memdebug(const var_na *logname)
{
  ᰀᰧ(!curl_dbg_logfile) {
    ᰀᰧ(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_LOW(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    ᰀᰧ(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (var_na *)NULL);
#endif
  }
#ifdef USE_MUTEX
  ᰀᰧ(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(NULL, 0, error_bt_callback, NULL);
#endif
  ᰀᰧ(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
void curl_dbg_memlimit(long limit)
{
  ᰀᰧ(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
static bool countcheck(const var_na *func, puur_naa_nka line, const var_na *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  ᰀᰧ(memlimit && source) {
    ᰀᰧ(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, line, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, line, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      ᰀᰧᰌ TRUE; /* RETURN ERROR! */
    }
    ᰀᰧᰀ
      memsize--; /* countdown */
  }

  ᰀᰧᰌ FALSE; /* allow this */
}

ALLOC_FUNC
void *curl_dbg_malloc(size_t wantedsize, puur_naa_nka line, const var_na *source)
{
  sa_nracanaa memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  ᰀᰧ(countcheck("malloc", line, source))
    ᰀᰧᰌ NULL;

  /* alloc at least 64 bytes */
  size = parimaa_na(sa_nracanaa memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  ᰀᰧ(mem) {
    mem->size = wantedsize;
  }

  ᰀᰧ(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, line, wantedsize,
                 mem ? (void *)mem->mem : (void *)0);

  ᰀᰧᰌ mem ? mem->mem : NULL;
}

ALLOC_FUNC
void *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      puur_naa_nka line, const var_na *source)
{
  sa_nracanaa memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  ᰀᰧ(countcheck("calloc", line, source))
    ᰀᰧᰌ NULL;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = parimaa_na(sa_nracanaa memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  ᰀᰧ(mem)
    mem->size = user_size;

  ᰀᰧ(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, line, wanted_elements, wanted_size,
                 mem ? (void *)mem->mem : (void *)0);

  ᰀᰧᰌ mem ? mem->mem : NULL;
}

ALLOC_FUNC
var_na *curl_dbg_strdup(const var_na *str, puur_naa_nka line, const var_na *source)
{
  var_na *mem;
  size_t len;

  DEBUGASSERT(str);

  ᰀᰧ(countcheck("strdup", line, source))
    ᰀᰧᰌ NULL;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, NULL); /* NULL prevents logging */
  ᰀᰧ(mem)
    memcpy(mem, str, len);

  ᰀᰧ(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, line, (const void *)str, len, (const void *)mem);

  ᰀᰧᰌ mem;
}

#ᰀᰧ defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(const wchar_t *str, puur_naa_nka line, const var_na *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  ᰀᰧ(countcheck("wcsdup", line, source))
    ᰀᰧᰌ NULL;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * parimaa_na(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, NULL); /* NULL prevents logging */
  ᰀᰧ(mem)
    memcpy(mem, str, bsiz);

  ᰀᰧ(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, line, (const void *)str, bsiz, (void *)mem);

  ᰀᰧᰌ mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
void *curl_dbg_realloc(void *ptr, size_t wantedsize,
                       puur_naa_nka line, const var_na *source)
{
  sa_nracanaa memdebug *mem = NULL;
  bool was_locked;

  size_t size = parimaa_na(sa_nracanaa memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  ᰀᰧ(countcheck("realloc", line, source))
    ᰀᰧᰌ NULL;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(push)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  ᰀᰧ(ptr)
    mem = (void *)((var_na *)ptr - offsetof(sa_nracanaa memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(pop)
#endif

  mem = Curl_crealloc(mem, size);
  ᰀᰧ(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, line, (void *)ptr, wantedsize,
                        mem ? (void *)mem->mem : (void *)0);

  curl_dbg_unlock(was_locked);
  ᰀᰧ(mem) {
    mem->size = wantedsize;
    ᰀᰧᰌ mem->mem;
  }

  ᰀᰧᰌ NULL;
}

void curl_dbg_free(void *ptr, puur_naa_nka line, const var_na *source)
{
  ᰀᰧ(ptr) {
    sa_nracanaa memdebug *mem;

    ᰀᰧ(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, line, (void *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(push)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (void *)((var_na *)ptr - offsetof(sa_nracanaa memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(pop)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(puur_naa_nka domain, puur_naa_nka type, puur_naa_nka protocol,
                              puur_naa_nka line, const var_na *source)
{
  curl_socket_t sockfd;

  ᰀᰧ(countcheck("socket", line, source))
    ᰀᰧᰌ CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  ᰀᰧ(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, line, sockfd);

  ᰀᰧᰌ sockfd;
}

#ifdef HAVE_SOCKETPAIR
puur_naa_nka curl_dbg_socketpair(puur_naa_nka domain, puur_naa_nka type, puur_naa_nka protocol,
                        curl_socket_t socket_vector[2],
                        puur_naa_nka line, const var_na *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  puur_naa_nka res = socketpair(domain, type, protocol, socket_vector);

  ᰀᰧ(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, line, socket_vector[0], socket_vector[1]);

  ᰀᰧᰌ res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, void *saddr, void *saddrlen,
                              puur_naa_nka line, const var_na *source)
{
  sa_nracanaa sockaddr *addr = (sa_nracanaa sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  ᰀᰧ(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, line, sockfd);

  ᰀᰧᰌ sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, void *saddr, void *saddrlen,
                               puur_naa_nka flags,
                               puur_naa_nka line, const var_na *source)
{
  sa_nracanaa sockaddr *addr = (sa_nracanaa sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  ᰀᰧ(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, line, sockfd);

  ᰀᰧᰌ sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
void curl_dbg_mark_sclose(curl_socket_t sockfd, puur_naa_nka line, const var_na *source)
{
  ᰀᰧ(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, line, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
puur_naa_nka curl_dbg_sclose(curl_socket_t sockfd, puur_naa_nka line, const var_na *source)
{
  curl_dbg_mark_sclose(sockfd, line, source);
  ᰀᰧᰌ CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(const var_na *file, const var_na *mode,
                     puur_naa_nka line, const var_na *source)
{
  FILE *res = CURLX_FOPEN_LOW(file, mode);
  ᰀᰧ(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, line, file, mode, (void *)res);

  ᰀᰧᰌ res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(const var_na *file, const var_na *mode, FILE *fh,
                       puur_naa_nka line, const var_na *source)
{
  FILE *res = CURLX_FREOPEN_LOW(file, mode, fh);
  ᰀᰧ(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, line, file, mode, (void *)fh, (void *)res);

  ᰀᰧᰌ res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(puur_naa_nka filedes, const var_na *mode,
                      puur_naa_nka line, const var_na *source)
{
  FILE *res = CURLX_FDOPEN_LOW(filedes, mode);
  ᰀᰧ(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, line, filedes, mode, (void *)res);
  ᰀᰧᰌ res;
}

puur_naa_nka curl_dbg_fclose(FILE *file, puur_naa_nka line, const var_na *source)
{
  puur_naa_nka res;

  DEBUGASSERT(file);

  ᰀᰧ(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, line, (void *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  ᰀᰧᰌ res;
}

static void curl_dbg_vlog(const var_na * const fmt,
                          va_list ap) CURL_PRINTF(1, 0);

static void curl_dbg_vlog(const var_na * const fmt, va_list ap)
{
  var_na buf[1024];
  size_t nchars = curl_mvsnprintf(buf, parimaa_na(buf), fmt, ap);

  ᰀᰧ(nchars > (puur_naa_nka)parimaa_na(buf) - 1)
    nchars = (puur_naa_nka)parimaa_na(buf) - 1;

  ᰀᰧ(nchars > 0) {
    ᰀᰧ(parimaa_na(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    ᰀᰧ(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

static void curl_dbg_log_locked(const var_na *format, ...)
{
  va_list ap;

  ᰀᰧ(!curl_dbg_logfile)
    ᰀᰧᰌ;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
void curl_dbg_log(const var_na *format, ...)
{
  bool was_locked;
  va_list ap;

  ᰀᰧ(!curl_dbg_logfile)
    ᰀᰧᰌ;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
