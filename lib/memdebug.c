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
#ضمن "curl_setup.h"

#ifdef CURL_MEMDEBUG

#ضمن <stddef.h>  /* for offsetof() */

#ضمن "urldata.h"
#ضمن "curl_threads.h"
#ضمن "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#ضمن <backtrace.h>
#endif

بنية memdebug {
  size_t size;
  اتحاد {
    curl_off_t o;
    مزدوج d;
    فراغ *p;
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

FILE *curl_dbg_logfile = عدم;
ساكن bool registered_cleanup = FALSE; /* atexit registered cleanup */
ساكن bool memlimit = FALSE; /* enable memory limit */
ساكن long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
ساكن بنية backtrace_state *btstate;
#endif

ساكن حرف membuf[10000];
ساكن size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
ساكن bool dbg_mutex_init = 0;
ساكن curl_mutex_t dbg_mutex;
#endif

ساكن bool curl_dbg_lock(فراغ)
{
#ifdef USE_MUTEX
  إذا(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    أرجع TRUE;
  }
#endif
  أرجع FALSE;
}

ساكن فراغ curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  إذا(was_locked)
    Curl_mutex_release(&dbg_mutex);
#وإلا
  (فراغ)was_locked;
#endif
}

ساكن فراغ curl_dbg_log_locked(ثابت حرف *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ساكن فراغ curl_dbg_cleanup(فراغ)
{
  bool locked = curl_dbg_lock();
  إذا(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    إذا(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = عدم;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  إذا(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
ساكن فراغ error_bt_callback(فراغ *data, ثابت حرف *message,
                              صحيح error_نومبور)
{
  (فراغ)data;
  إذا(error_نومبور == -1)
    curl_dbg_log("compile with -g\n\n");
  وإلا
    curl_dbg_log("Backtrace error %d: %s\n", error_نومبور, message);
}

ساكن صحيح full_callback(فراغ *data, uintptr_t pc, ثابت حرف *pathname,
                         صحيح باريس_نومبور, ثابت حرف *function)
{
  (فراغ)data;
  (فراغ)pc;
  إذا(pathname || function || باريس_نومبور)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, باريس_نومبور, function);
  أرجع 0;
}

ساكن فراغ dump_bt(فراغ)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, عدم);
}
#وإلا
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
فراغ curl_dbg_memdebug(ثابت حرف *logname)
{
  إذا(!curl_dbg_logfile) {
    إذا(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_رنده(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    إذا(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (حرف *)عدم);
#endif
  }
#ifdef USE_MUTEX
  إذا(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(عدم, 0, error_bt_callback, عدم);
#endif
  إذا(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
فراغ curl_dbg_memlimit(long limit)
{
  إذا(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
ساكن bool countcheck(ثابت حرف *func, صحيح باريس, ثابت حرف *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  إذا(memlimit && source) {
    إذا(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, باريس, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, باريس, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      أرجع TRUE; /* RETURN ERROR! */
    }
    وإلا
      memsize--; /* countdown */
  }

  أرجع FALSE; /* allow this */
}

ALLOC_FUNC
فراغ *curl_dbg_malloc(size_t wantedsize, صحيح باريس, ثابت حرف *source)
{
  بنية memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  إذا(countcheck("malloc", باريس, source))
    أرجع عدم;

  /* alloc at least 64 bytes */
  size = حجم(بنية memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  إذا(mem) {
    mem->size = wantedsize;
  }

  إذا(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, باريس, wantedsize,
                 mem ? (فراغ *)mem->mem : (فراغ *)0);

  أرجع mem ? mem->mem : عدم;
}

ALLOC_FUNC
فراغ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      صحيح باريس, ثابت حرف *source)
{
  بنية memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  إذا(countcheck("calloc", باريس, source))
    أرجع عدم;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = حجم(بنية memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  إذا(mem)
    mem->size = user_size;

  إذا(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, باريس, wanted_elements, wanted_size,
                 mem ? (فراغ *)mem->mem : (فراغ *)0);

  أرجع mem ? mem->mem : عدم;
}

ALLOC_FUNC
حرف *curl_dbg_strdup(ثابت حرف *str, صحيح باريس, ثابت حرف *source)
{
  حرف *mem;
  size_t len;

  DEBUGASSERT(str);

  إذا(countcheck("strdup", باريس, source))
    أرجع عدم;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, عدم); /* NULL prevents logging */
  إذا(mem)
    memcpy(mem, str, len);

  إذا(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, باريس, (ثابت فراغ *)str, len, (ثابت فراغ *)mem);

  أرجع mem;
}

#إذا defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحيح باريس, ثابت حرف *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  إذا(countcheck("wcsdup", باريس, source))
    أرجع عدم;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * حجم(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, عدم); /* NULL prevents logging */
  إذا(mem)
    memcpy(mem, str, bsiz);

  إذا(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, باريس, (ثابت فراغ *)str, bsiz, (فراغ *)mem);

  أرجع mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
فراغ *curl_dbg_realloc(فراغ *ptr, size_t wantedsize,
                       صحيح باريس, ثابت حرف *source)
{
  بنية memdebug *mem = عدم;
  bool was_locked;

  size_t size = حجم(بنية memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  إذا(countcheck("realloc", باريس, source))
    أرجع عدم;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(تولق)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  إذا(ptr)
    mem = (فراغ *)((حرف *)ptr - offsetof(بنية memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(کلوار)
#endif

  mem = Curl_crealloc(mem, size);
  إذا(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, باريس, (فراغ *)ptr, wantedsize,
                        mem ? (فراغ *)mem->mem : (فراغ *)0);

  curl_dbg_unlock(was_locked);
  إذا(mem) {
    mem->size = wantedsize;
    أرجع mem->mem;
  }

  أرجع عدم;
}

فراغ curl_dbg_free(فراغ *ptr, صحيح باريس, ثابت حرف *source)
{
  إذا(ptr) {
    بنية memdebug *mem;

    إذا(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, باريس, (فراغ *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(تولق)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (فراغ *)((حرف *)ptr - offsetof(بنية memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(کلوار)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(صحيح domain, صحيح type, صحيح protocol,
                              صحيح باريس, ثابت حرف *source)
{
  curl_socket_t sockfd;

  إذا(countcheck("socket", باريس, source))
    أرجع CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  إذا(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, باريس, sockfd);

  أرجع sockfd;
}

#ifdef HAVE_SOCKETPAIR
صحيح curl_dbg_socketpair(صحيح domain, صحيح type, صحيح protocol,
                        curl_socket_t socket_vector[2],
                        صحيح باريس, ثابت حرف *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  صحيح res = socketpair(domain, type, protocol, socket_vector);

  إذا(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, باريس, socket_vector[0], socket_vector[1]);

  أرجع res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, فراغ *saddr, فراغ *saddrlen,
                              صحيح باريس, ثابت حرف *source)
{
  بنية sockaddr *addr = (بنية sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  إذا(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, باريس, sockfd);

  أرجع sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, فراغ *saddr, فراغ *saddrlen,
                               صحيح flags,
                               صحيح باريس, ثابت حرف *source)
{
  بنية sockaddr *addr = (بنية sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  إذا(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, باريس, sockfd);

  أرجع sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
فراغ curl_dbg_mark_sclose(curl_socket_t sockfd, صحيح باريس, ثابت حرف *source)
{
  إذا(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, باريس, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
صحيح curl_dbg_sclose(curl_socket_t sockfd, صحيح باريس, ثابت حرف *source)
{
  curl_dbg_mark_sclose(sockfd, باريس, source);
  أرجع CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ثابت حرف *file, ثابت حرف *mode,
                     صحيح باريس, ثابت حرف *source)
{
  FILE *res = CURLX_FOPEN_رنده(file, mode);
  إذا(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, باريس, file, mode, (فراغ *)res);

  أرجع res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ثابت حرف *file, ثابت حرف *mode, FILE *fh,
                       صحيح باريس, ثابت حرف *source)
{
  FILE *res = CURLX_FREOPEN_رنده(file, mode, fh);
  إذا(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, باريس, file, mode, (فراغ *)fh, (فراغ *)res);

  أرجع res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(صحيح filedes, ثابت حرف *mode,
                      صحيح باريس, ثابت حرف *source)
{
  FILE *res = CURLX_FDOPEN_رنده(filedes, mode);
  إذا(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, باريس, filedes, mode, (فراغ *)res);
  أرجع res;
}

صحيح curl_dbg_fclose(FILE *file, صحيح باريس, ثابت حرف *source)
{
  صحيح res;

  DEBUGASSERT(file);

  إذا(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, باريس, (فراغ *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  أرجع res;
}

ساكن فراغ curl_dbg_vlog(ثابت حرف * ثابت fmt,
                          va_list ap) CURL_PRINTF(1, 0);

ساكن فراغ curl_dbg_vlog(ثابت حرف * ثابت fmt, va_list ap)
{
  حرف buf[1024];
  size_t nchars = curl_mvsnprintf(buf, حجم(buf), fmt, ap);

  إذا(nchars > (صحيح)حجم(buf) - 1)
    nchars = (صحيح)حجم(buf) - 1;

  إذا(nchars > 0) {
    إذا(حجم(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    إذا(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

ساكن فراغ curl_dbg_log_locked(ثابت حرف *format, ...)
{
  va_list ap;

  إذا(!curl_dbg_logfile)
    أرجع;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
فراغ curl_dbg_log(ثابت حرف *format, ...)
{
  bool was_locked;
  va_list ap;

  إذا(!curl_dbg_logfile)
    أرجع;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
