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
#અન્તર્ભાવ "curl_setup.h"

#ifdef CURL_MEMDEBUG

#અન્તર્ભાવ <stddef.h>  /* for offsetof() */

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_threads.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#અન્તર્ભાવ <backtrace.h>
#endif

સંરચના memdebug {
  size_t size;
  સંઘ {
    curl_off_t o;
    દ્વિગુણ d;
    શૂન્ય *p;
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

FILE *curl_dbg_logfile = ખાલી;
સ્થિર bool registered_cleanup = FALSE; /* atexit registered cleanup */
સ્થિર bool memlimit = FALSE; /* enable memory limit */
સ્થિર long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
સ્થિર સંરચના backtrace_state *btstate;
#endif

સ્થિર અક્ષર membuf[10000];
સ્થિર size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
સ્થિર bool dbg_mutex_init = 0;
સ્થિર curl_mutex_t dbg_mutex;
#endif

સ્થિર bool curl_dbg_lock(શૂન્ય)
{
#ifdef USE_MUTEX
  જો(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    પાછા TRUE;
  }
#endif
  પાછા FALSE;
}

સ્થિર શૂન્ય curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  જો(was_locked)
    Curl_mutex_release(&dbg_mutex);
#નહીંતર
  (શૂન્ય)was_locked;
#endif
}

સ્થિર શૂન્ય curl_dbg_log_locked(અચલ અક્ષર *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
સ્થિર શૂન્ય curl_dbg_cleanup(શૂન્ય)
{
  bool locked = curl_dbg_lock();
  જો(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    જો(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = ખાલી;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  જો(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
સ્થિર શૂન્ય error_bt_callback(શૂન્ય *data, અચલ અક્ષર *message,
                              પૂર્ણાંક error_સંખ્યા)
{
  (શૂન્ય)data;
  જો(error_સંખ્યા == -1)
    curl_dbg_log("compile with -g\n\n");
  નહીંતર
    curl_dbg_log("Backtrace error %d: %s\n", error_સંખ્યા, message);
}

સ્થિર પૂર્ણાંક full_callback(શૂન્ય *data, uintptr_t pc, અચલ અક્ષર *pathname,
                         પૂર્ણાંક લીટી_સંખ્યા, અચલ અક્ષર *function)
{
  (શૂન્ય)data;
  (શૂન્ય)pc;
  જો(pathname || function || લીટી_સંખ્યા)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, લીટી_સંખ્યા, function);
  પાછા 0;
}

સ્થિર શૂન્ય dump_bt(શૂન્ય)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, ખાલી);
}
#નહીંતર
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
શૂન્ય curl_dbg_memdebug(અચલ અક્ષર *logname)
{
  જો(!curl_dbg_logfile) {
    જો(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_નીચલું(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    જો(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (અક્ષર *)ખાલી);
#endif
  }
#ifdef USE_MUTEX
  જો(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(ખાલી, 0, error_bt_callback, ખાલી);
#endif
  જો(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
શૂન્ય curl_dbg_memlimit(long limit)
{
  જો(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
સ્થિર bool countcheck(અચલ અક્ષર *func, પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  જો(memlimit && source) {
    જો(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, લીટી, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, લીટી, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      પાછા TRUE; /* RETURN ERROR! */
    }
    નહીંતર
      memsize--; /* countdown */
  }

  પાછા FALSE; /* allow this */
}

ALLOC_FUNC
શૂન્ય *curl_dbg_malloc(size_t wantedsize, પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  સંરચના memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  જો(countcheck("malloc", લીટી, source))
    પાછા ખાલી;

  /* alloc at least 64 bytes */
  size = કદ(સંરચના memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  જો(mem) {
    mem->size = wantedsize;
  }

  જો(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, લીટી, wantedsize,
                 mem ? (શૂન્ય *)mem->mem : (શૂન્ય *)0);

  પાછા mem ? mem->mem : ખાલી;
}

ALLOC_FUNC
શૂન્ય *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  સંરચના memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  જો(countcheck("calloc", લીટી, source))
    પાછા ખાલી;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = કદ(સંરચના memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  જો(mem)
    mem->size = user_size;

  જો(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, લીટી, wanted_elements, wanted_size,
                 mem ? (શૂન્ય *)mem->mem : (શૂન્ય *)0);

  પાછા mem ? mem->mem : ખાલી;
}

ALLOC_FUNC
અક્ષર *curl_dbg_strdup(અચલ અક્ષર *str, પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  અક્ષર *mem;
  size_t len;

  DEBUGASSERT(str);

  જો(countcheck("strdup", લીટી, source))
    પાછા ખાલી;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, ખાલી); /* NULL prevents logging */
  જો(mem)
    memcpy(mem, str, len);

  જો(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, લીટી, (અચલ શૂન્ય *)str, len, (અચલ શૂન્ય *)mem);

  પાછા mem;
}

#જો defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(અચલ wchar_t *str, પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  જો(countcheck("wcsdup", લીટી, source))
    પાછા ખાલી;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * કદ(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, ખાલી); /* NULL prevents logging */
  જો(mem)
    memcpy(mem, str, bsiz);

  જો(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, લીટી, (અચલ શૂન્ય *)str, bsiz, (શૂન્ય *)mem);

  પાછા mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
શૂન્ય *curl_dbg_realloc(શૂન્ય *ptr, size_t wantedsize,
                       પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  સંરચના memdebug *mem = ખાલી;
  bool was_locked;

  size_t size = કદ(સંરચના memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  જો(countcheck("realloc", લીટી, source))
    પાછા ખાલી;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(ધકેલો)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  જો(ptr)
    mem = (શૂન્ય *)((અક્ષર *)ptr - offsetof(સંરચના memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(કાઢો)
#endif

  mem = Curl_crealloc(mem, size);
  જો(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, લીટી, (શૂન્ય *)ptr, wantedsize,
                        mem ? (શૂન્ય *)mem->mem : (શૂન્ય *)0);

  curl_dbg_unlock(was_locked);
  જો(mem) {
    mem->size = wantedsize;
    પાછા mem->mem;
  }

  પાછા ખાલી;
}

શૂન્ય curl_dbg_free(શૂન્ય *ptr, પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  જો(ptr) {
    સંરચના memdebug *mem;

    જો(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, લીટી, (શૂન્ય *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(ધકેલો)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (શૂન્ય *)((અક્ષર *)ptr - offsetof(સંરચના memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(કાઢો)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(પૂર્ણાંક domain, પૂર્ણાંક type, પૂર્ણાંક protocol,
                              પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  curl_socket_t sockfd;

  જો(countcheck("socket", લીટી, source))
    પાછા CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  જો(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, લીટી, sockfd);

  પાછા sockfd;
}

#ifdef HAVE_SOCKETPAIR
પૂર્ણાંક curl_dbg_socketpair(પૂર્ણાંક domain, પૂર્ણાંક type, પૂર્ણાંક protocol,
                        curl_socket_t socket_vector[2],
                        પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  પૂર્ણાંક res = socketpair(domain, type, protocol, socket_vector);

  જો(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, લીટી, socket_vector[0], socket_vector[1]);

  પાછા res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, શૂન્ય *saddr, શૂન્ય *saddrlen,
                              પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  સંરચના sockaddr *addr = (સંરચના sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  જો(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, લીટી, sockfd);

  પાછા sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, શૂન્ય *saddr, શૂન્ય *saddrlen,
                               પૂર્ણાંક flags,
                               પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  સંરચના sockaddr *addr = (સંરચના sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  જો(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, લીટી, sockfd);

  પાછા sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
શૂન્ય curl_dbg_mark_sclose(curl_socket_t sockfd, પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  જો(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, લીટી, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
પૂર્ણાંક curl_dbg_sclose(curl_socket_t sockfd, પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  curl_dbg_mark_sclose(sockfd, લીટી, source);
  પાછા CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(અચલ અક્ષર *file, અચલ અક્ષર *mode,
                     પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  FILE *res = CURLX_FOPEN_નીચલું(file, mode);
  જો(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, લીટી, file, mode, (શૂન્ય *)res);

  પાછા res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(અચલ અક્ષર *file, અચલ અક્ષર *mode, FILE *fh,
                       પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  FILE *res = CURLX_FREOPEN_નીચલું(file, mode, fh);
  જો(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, લીટી, file, mode, (શૂન્ય *)fh, (શૂન્ય *)res);

  પાછા res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(પૂર્ણાંક filedes, અચલ અક્ષર *mode,
                      પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  FILE *res = CURLX_FDOPEN_નીચલું(filedes, mode);
  જો(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, લીટી, filedes, mode, (શૂન્ય *)res);
  પાછા res;
}

પૂર્ણાંક curl_dbg_fclose(FILE *file, પૂર્ણાંક લીટી, અચલ અક્ષર *source)
{
  પૂર્ણાંક res;

  DEBUGASSERT(file);

  જો(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, લીટી, (શૂન્ય *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  પાછા res;
}

સ્થિર શૂન્ય curl_dbg_vlog(અચલ અક્ષર * અચલ fmt,
                          va_list ap) CURL_PRINTF(1, 0);

સ્થિર શૂન્ય curl_dbg_vlog(અચલ અક્ષર * અચલ fmt, va_list ap)
{
  અક્ષર buf[1024];
  size_t nchars = curl_mvsnprintf(buf, કદ(buf), fmt, ap);

  જો(nchars > (પૂર્ણાંક)કદ(buf) - 1)
    nchars = (પૂર્ણાંક)કદ(buf) - 1;

  જો(nchars > 0) {
    જો(કદ(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    જો(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

સ્થિર શૂન્ય curl_dbg_log_locked(અચલ અક્ષર *format, ...)
{
  va_list ap;

  જો(!curl_dbg_logfile)
    પાછા;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
શૂન્ય curl_dbg_log(અચલ અક્ષર *format, ...)
{
  bool was_locked;
  va_list ap;

  જો(!curl_dbg_logfile)
    પાછા;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
