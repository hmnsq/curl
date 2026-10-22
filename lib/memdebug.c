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

struct memdebug {
  size_t size;
  union {
    curl_off_t o;
    double d;
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
static struct backtrace_state *btstate;
#endif

static char membuf[10000];
static size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
static bool dbg_mutex_init = 0;
static curl_mutex_t dbg_mutex;
#endif

static bool curl_dbg_lock(void)
{
#ifdef USE_MUTEX
  nếu(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    trả_về TRUE;
  }
#endif
  trả_về FALSE;
}

static void curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  nếu(was_locked)
    Curl_mutex_release(&dbg_mutex);
#không_thì
  (void)was_locked;
#endif
}

static void curl_dbg_log_locked(const char *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
static void curl_dbg_cleanup(void)
{
  bool locked = curl_dbg_lock();
  nếu(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    nếu(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = NULL;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  nếu(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
static void error_bt_callback(void *data, const char *message,
                              số_nguyên error_number)
{
  (void)data;
  nếu(error_number == -1)
    curl_dbg_log("compile with -g\n\n");
  không_thì
    curl_dbg_log("Backtrace error %d: %s\n", error_number, message);
}

static số_nguyên full_callback(void *data, uintptr_t pc, const char *pathname,
                         số_nguyên line_number, const char *function)
{
  (void)data;
  (void)pc;
  nếu(pathname || function || line_number)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, line_number, function);
  trả_về 0;
}

static void dump_bt(void)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, NULL);
}
#không_thì
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
void curl_dbg_memdebug(const char *logname)
{
  nếu(!curl_dbg_logfile) {
    nếu(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_LOW(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    nếu(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (char *)NULL);
#endif
  }
#ifdef USE_MUTEX
  nếu(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(NULL, 0, error_bt_callback, NULL);
#endif
  nếu(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
void curl_dbg_memlimit(long limit)
{
  nếu(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
static bool countcheck(const char *func, số_nguyên line, const char *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  nếu(memlimit && source) {
    nếu(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, line, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, line, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      trả_về TRUE; /* RETURN ERROR! */
    }
    không_thì
      memsize--; /* countdown */
  }

  trả_về FALSE; /* allow this */
}

ALLOC_FUNC
void *curl_dbg_malloc(size_t wantedsize, số_nguyên line, const char *source)
{
  struct memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  nếu(countcheck("malloc", line, source))
    trả_về NULL;

  /* alloc at least 64 bytes */
  size = sizeof(struct memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  nếu(mem) {
    mem->size = wantedsize;
  }

  nếu(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, line, wantedsize,
                 mem ? (void *)mem->mem : (void *)0);

  trả_về mem ? mem->mem : NULL;
}

ALLOC_FUNC
void *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      số_nguyên line, const char *source)
{
  struct memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  nếu(countcheck("calloc", line, source))
    trả_về NULL;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = sizeof(struct memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  nếu(mem)
    mem->size = user_size;

  nếu(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, line, wanted_elements, wanted_size,
                 mem ? (void *)mem->mem : (void *)0);

  trả_về mem ? mem->mem : NULL;
}

ALLOC_FUNC
char *curl_dbg_strdup(const char *str, số_nguyên line, const char *source)
{
  char *mem;
  size_t len;

  DEBUGASSERT(str);

  nếu(countcheck("strdup", line, source))
    trả_về NULL;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, NULL); /* NULL prevents logging */
  nếu(mem)
    memcpy(mem, str, len);

  nếu(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, line, (const void *)str, len, (const void *)mem);

  trả_về mem;
}

#nếu defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(const wchar_t *str, số_nguyên line, const char *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  nếu(countcheck("wcsdup", line, source))
    trả_về NULL;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * sizeof(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, NULL); /* NULL prevents logging */
  nếu(mem)
    memcpy(mem, str, bsiz);

  nếu(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, line, (const void *)str, bsiz, (void *)mem);

  trả_về mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
void *curl_dbg_realloc(void *ptr, size_t wantedsize,
                       số_nguyên line, const char *source)
{
  struct memdebug *mem = NULL;
  bool was_locked;

  size_t size = sizeof(struct memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  nếu(countcheck("realloc", line, source))
    trả_về NULL;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(push)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  nếu(ptr)
    mem = (void *)((char *)ptr - offsetof(struct memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(pop)
#endif

  mem = Curl_crealloc(mem, size);
  nếu(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, line, (void *)ptr, wantedsize,
                        mem ? (void *)mem->mem : (void *)0);

  curl_dbg_unlock(was_locked);
  nếu(mem) {
    mem->size = wantedsize;
    trả_về mem->mem;
  }

  trả_về NULL;
}

void curl_dbg_free(void *ptr, số_nguyên line, const char *source)
{
  nếu(ptr) {
    struct memdebug *mem;

    nếu(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, line, (void *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(push)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (void *)((char *)ptr - offsetof(struct memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(pop)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(số_nguyên domain, số_nguyên type, số_nguyên protocol,
                              số_nguyên line, const char *source)
{
  curl_socket_t sockfd;

  nếu(countcheck("socket", line, source))
    trả_về CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  nếu(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, line, sockfd);

  trả_về sockfd;
}

#ifdef HAVE_SOCKETPAIR
số_nguyên curl_dbg_socketpair(số_nguyên domain, số_nguyên type, số_nguyên protocol,
                        curl_socket_t socket_vector[2],
                        số_nguyên line, const char *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  số_nguyên res = socketpair(domain, type, protocol, socket_vector);

  nếu(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, line, socket_vector[0], socket_vector[1]);

  trả_về res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, void *saddr, void *saddrlen,
                              số_nguyên line, const char *source)
{
  struct sockaddr *addr = (struct sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  nếu(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, line, sockfd);

  trả_về sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, void *saddr, void *saddrlen,
                               số_nguyên flags,
                               số_nguyên line, const char *source)
{
  struct sockaddr *addr = (struct sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  nếu(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, line, sockfd);

  trả_về sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
void curl_dbg_mark_sclose(curl_socket_t sockfd, số_nguyên line, const char *source)
{
  nếu(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, line, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
số_nguyên curl_dbg_sclose(curl_socket_t sockfd, số_nguyên line, const char *source)
{
  curl_dbg_mark_sclose(sockfd, line, source);
  trả_về CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(const char *file, const char *mode,
                     số_nguyên line, const char *source)
{
  FILE *res = CURLX_FOPEN_LOW(file, mode);
  nếu(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, line, file, mode, (void *)res);

  trả_về res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(const char *file, const char *mode, FILE *fh,
                       số_nguyên line, const char *source)
{
  FILE *res = CURLX_FREOPEN_LOW(file, mode, fh);
  nếu(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, line, file, mode, (void *)fh, (void *)res);

  trả_về res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(số_nguyên filedes, const char *mode,
                      số_nguyên line, const char *source)
{
  FILE *res = CURLX_FDOPEN_LOW(filedes, mode);
  nếu(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, line, filedes, mode, (void *)res);
  trả_về res;
}

số_nguyên curl_dbg_fclose(FILE *file, số_nguyên line, const char *source)
{
  số_nguyên res;

  DEBUGASSERT(file);

  nếu(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, line, (void *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  trả_về res;
}

static void curl_dbg_vlog(const char * const fmt,
                          va_list ap) CURL_PRINTF(1, 0);

static void curl_dbg_vlog(const char * const fmt, va_list ap)
{
  char buf[1024];
  size_t nchars = curl_mvsnprintf(buf, sizeof(buf), fmt, ap);

  nếu(nchars > (số_nguyên)sizeof(buf) - 1)
    nchars = (số_nguyên)sizeof(buf) - 1;

  nếu(nchars > 0) {
    nếu(sizeof(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    nếu(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

static void curl_dbg_log_locked(const char *format, ...)
{
  va_list ap;

  nếu(!curl_dbg_logfile)
    trả_về;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
void curl_dbg_log(const char *format, ...)
{
  bool was_locked;
  va_list ap;

  nếu(!curl_dbg_logfile)
    trả_về;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
