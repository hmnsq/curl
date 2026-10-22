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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef CURL_MEMDEBUG

#അന്തര്ഭാവ <stddef.h>  /* for offsetof() */

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_threads.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#അന്തര്ഭാവ <backtrace.h>
#endif

ഘടന memdebug {
  size_t size;
  സംഘം {
    curl_off_t o;
    ഇരട്ട d;
    ശൂന്യം *p;
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

FILE *curl_dbg_logfile = ശൂന്യ;
സ്ഥിതം bool registered_cleanup = FALSE; /* atexit registered cleanup */
സ്ഥിതം bool memlimit = FALSE; /* enable memory limit */
സ്ഥിതം long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
സ്ഥിതം ഘടന backtrace_state *btstate;
#endif

സ്ഥിതം അക്ഷരം membuf[10000];
സ്ഥിതം size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
സ്ഥിതം bool dbg_mutex_init = 0;
സ്ഥിതം curl_mutex_t dbg_mutex;
#endif

സ്ഥിതം bool curl_dbg_lock(ശൂന്യം)
{
#ifdef USE_MUTEX
  എങ്കിൽ(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    തിരികെ TRUE;
  }
#endif
  തിരികെ FALSE;
}

സ്ഥിതം ശൂന്യം curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  എങ്കിൽ(was_locked)
    Curl_mutex_release(&dbg_mutex);
#അല്ലെങ്കിൽ
  (ശൂന്യം)was_locked;
#endif
}

സ്ഥിതം ശൂന്യം curl_dbg_log_locked(സ്ഥിരം അക്ഷരം *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
സ്ഥിതം ശൂന്യം curl_dbg_cleanup(ശൂന്യം)
{
  bool locked = curl_dbg_lock();
  എങ്കിൽ(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    എങ്കിൽ(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = ശൂന്യ;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  എങ്കിൽ(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
സ്ഥിതം ശൂന്യം error_bt_callback(ശൂന്യം *data, സ്ഥിരം അക്ഷരം *message,
                              പൂർണ്ണസംഖ്യ error_സംഖ്യ)
{
  (ശൂന്യം)data;
  എങ്കിൽ(error_സംഖ്യ == -1)
    curl_dbg_log("compile with -g\n\n");
  അല്ലെങ്കിൽ
    curl_dbg_log("Backtrace error %d: %s\n", error_സംഖ്യ, message);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ full_callback(ശൂന്യം *data, uintptr_t pc, സ്ഥിരം അക്ഷരം *pathname,
                         പൂർണ്ണസംഖ്യ വരി_സംഖ്യ, സ്ഥിരം അക്ഷരം *function)
{
  (ശൂന്യം)data;
  (ശൂന്യം)pc;
  എങ്കിൽ(pathname || function || വരി_സംഖ്യ)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, വരി_സംഖ്യ, function);
  തിരികെ 0;
}

സ്ഥിതം ശൂന്യം dump_bt(ശൂന്യം)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, ശൂന്യ);
}
#അല്ലെങ്കിൽ
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
ശൂന്യം curl_dbg_memdebug(സ്ഥിരം അക്ഷരം *logname)
{
  എങ്കിൽ(!curl_dbg_logfile) {
    എങ്കിൽ(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_താഴെയുള്ള(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    എങ്കിൽ(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (അക്ഷരം *)ശൂന്യ);
#endif
  }
#ifdef USE_MUTEX
  എങ്കിൽ(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(ശൂന്യ, 0, error_bt_callback, ശൂന്യ);
#endif
  എങ്കിൽ(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
ശൂന്യം curl_dbg_memlimit(long limit)
{
  എങ്കിൽ(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
സ്ഥിതം bool countcheck(സ്ഥിരം അക്ഷരം *func, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  എങ്കിൽ(memlimit && source) {
    എങ്കിൽ(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, വരി, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, വരി, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      തിരികെ TRUE; /* RETURN ERROR! */
    }
    അല്ലെങ്കിൽ
      memsize--; /* countdown */
  }

  തിരികെ FALSE; /* allow this */
}

ALLOC_FUNC
ശൂന്യം *curl_dbg_malloc(size_t wantedsize, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  ഘടന memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  എങ്കിൽ(countcheck("malloc", വരി, source))
    തിരികെ ശൂന്യ;

  /* alloc at least 64 bytes */
  size = വലുപ്പം(ഘടന memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  എങ്കിൽ(mem) {
    mem->size = wantedsize;
  }

  എങ്കിൽ(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, വരി, wantedsize,
                 mem ? (ശൂന്യം *)mem->mem : (ശൂന്യം *)0);

  തിരികെ mem ? mem->mem : ശൂന്യ;
}

ALLOC_FUNC
ശൂന്യം *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  ഘടന memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  എങ്കിൽ(countcheck("calloc", വരി, source))
    തിരികെ ശൂന്യ;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = വലുപ്പം(ഘടന memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  എങ്കിൽ(mem)
    mem->size = user_size;

  എങ്കിൽ(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, വരി, wanted_elements, wanted_size,
                 mem ? (ശൂന്യം *)mem->mem : (ശൂന്യം *)0);

  തിരികെ mem ? mem->mem : ശൂന്യ;
}

ALLOC_FUNC
അക്ഷരം *curl_dbg_strdup(സ്ഥിരം അക്ഷരം *str, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  അക്ഷരം *mem;
  size_t len;

  DEBUGASSERT(str);

  എങ്കിൽ(countcheck("strdup", വരി, source))
    തിരികെ ശൂന്യ;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, ശൂന്യ); /* NULL prevents logging */
  എങ്കിൽ(mem)
    memcpy(mem, str, len);

  എങ്കിൽ(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, വരി, (സ്ഥിരം ശൂന്യം *)str, len, (സ്ഥിരം ശൂന്യം *)mem);

  തിരികെ mem;
}

#എങ്കിൽ defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(സ്ഥിരം wchar_t *str, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  എങ്കിൽ(countcheck("wcsdup", വരി, source))
    തിരികെ ശൂന്യ;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * വലുപ്പം(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, ശൂന്യ); /* NULL prevents logging */
  എങ്കിൽ(mem)
    memcpy(mem, str, bsiz);

  എങ്കിൽ(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, വരി, (സ്ഥിരം ശൂന്യം *)str, bsiz, (ശൂന്യം *)mem);

  തിരികെ mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
ശൂന്യം *curl_dbg_realloc(ശൂന്യം *ptr, size_t wantedsize,
                       പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  ഘടന memdebug *mem = ശൂന്യ;
  bool was_locked;

  size_t size = വലുപ്പം(ഘടന memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  എങ്കിൽ(countcheck("realloc", വരി, source))
    തിരികെ ശൂന്യ;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(തള്ളുക)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  എങ്കിൽ(ptr)
    mem = (ശൂന്യം *)((അക്ഷരം *)ptr - offsetof(ഘടന memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(എടുക്കുക)
#endif

  mem = Curl_crealloc(mem, size);
  എങ്കിൽ(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, വരി, (ശൂന്യം *)ptr, wantedsize,
                        mem ? (ശൂന്യം *)mem->mem : (ശൂന്യം *)0);

  curl_dbg_unlock(was_locked);
  എങ്കിൽ(mem) {
    mem->size = wantedsize;
    തിരികെ mem->mem;
  }

  തിരികെ ശൂന്യ;
}

ശൂന്യം curl_dbg_free(ശൂന്യം *ptr, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  എങ്കിൽ(ptr) {
    ഘടന memdebug *mem;

    എങ്കിൽ(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, വരി, (ശൂന്യം *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(തള്ളുക)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (ശൂന്യം *)((അക്ഷരം *)ptr - offsetof(ഘടന memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(എടുക്കുക)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(പൂർണ്ണസംഖ്യ domain, പൂർണ്ണസംഖ്യ type, പൂർണ്ണസംഖ്യ protocol,
                              പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  curl_socket_t sockfd;

  എങ്കിൽ(countcheck("socket", വരി, source))
    തിരികെ CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  എങ്കിൽ(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, വരി, sockfd);

  തിരികെ sockfd;
}

#ifdef HAVE_SOCKETPAIR
പൂർണ്ണസംഖ്യ curl_dbg_socketpair(പൂർണ്ണസംഖ്യ domain, പൂർണ്ണസംഖ്യ type, പൂർണ്ണസംഖ്യ protocol,
                        curl_socket_t socket_vector[2],
                        പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  പൂർണ്ണസംഖ്യ res = socketpair(domain, type, protocol, socket_vector);

  എങ്കിൽ(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, വരി, socket_vector[0], socket_vector[1]);

  തിരികെ res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, ശൂന്യം *saddr, ശൂന്യം *saddrlen,
                              പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  ഘടന sockaddr *addr = (ഘടന sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  എങ്കിൽ(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, വരി, sockfd);

  തിരികെ sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, ശൂന്യം *saddr, ശൂന്യം *saddrlen,
                               പൂർണ്ണസംഖ്യ flags,
                               പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  ഘടന sockaddr *addr = (ഘടന sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  എങ്കിൽ(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, വരി, sockfd);

  തിരികെ sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
ശൂന്യം curl_dbg_mark_sclose(curl_socket_t sockfd, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  എങ്കിൽ(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, വരി, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
പൂർണ്ണസംഖ്യ curl_dbg_sclose(curl_socket_t sockfd, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  curl_dbg_mark_sclose(sockfd, വരി, source);
  തിരികെ CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(സ്ഥിരം അക്ഷരം *file, സ്ഥിരം അക്ഷരം *mode,
                     പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  FILE *res = CURLX_FOPEN_താഴെയുള്ള(file, mode);
  എങ്കിൽ(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, വരി, file, mode, (ശൂന്യം *)res);

  തിരികെ res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(സ്ഥിരം അക്ഷരം *file, സ്ഥിരം അക്ഷരം *mode, FILE *fh,
                       പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  FILE *res = CURLX_FREOPEN_താഴെയുള്ള(file, mode, fh);
  എങ്കിൽ(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, വരി, file, mode, (ശൂന്യം *)fh, (ശൂന്യം *)res);

  തിരികെ res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(പൂർണ്ണസംഖ്യ filedes, സ്ഥിരം അക്ഷരം *mode,
                      പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  FILE *res = CURLX_FDOPEN_താഴെയുള്ള(filedes, mode);
  എങ്കിൽ(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, വരി, filedes, mode, (ശൂന്യം *)res);
  തിരികെ res;
}

പൂർണ്ണസംഖ്യ curl_dbg_fclose(FILE *file, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *source)
{
  പൂർണ്ണസംഖ്യ res;

  DEBUGASSERT(file);

  എങ്കിൽ(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, വരി, (ശൂന്യം *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  തിരികെ res;
}

സ്ഥിതം ശൂന്യം curl_dbg_vlog(സ്ഥിരം അക്ഷരം * സ്ഥിരം fmt,
                          va_list ap) CURL_PRINTF(1, 0);

സ്ഥിതം ശൂന്യം curl_dbg_vlog(സ്ഥിരം അക്ഷരം * സ്ഥിരം fmt, va_list ap)
{
  അക്ഷരം buf[1024];
  size_t nchars = curl_mvsnprintf(buf, വലുപ്പം(buf), fmt, ap);

  എങ്കിൽ(nchars > (പൂർണ്ണസംഖ്യ)വലുപ്പം(buf) - 1)
    nchars = (പൂർണ്ണസംഖ്യ)വലുപ്പം(buf) - 1;

  എങ്കിൽ(nchars > 0) {
    എങ്കിൽ(വലുപ്പം(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    എങ്കിൽ(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

സ്ഥിതം ശൂന്യം curl_dbg_log_locked(സ്ഥിരം അക്ഷരം *format, ...)
{
  va_list ap;

  എങ്കിൽ(!curl_dbg_logfile)
    തിരികെ;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
ശൂന്യം curl_dbg_log(സ്ഥിരം അക്ഷരം *format, ...)
{
  bool was_locked;
  va_list ap;

  എങ്കിൽ(!curl_dbg_logfile)
    തിരികെ;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
