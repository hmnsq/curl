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

#که_تعریف_شوی CURL_MEMDEBUG

#ضمن <stddef.h>  /* for offsetof() */

#ضمن "urldata.h"
#ضمن "curl_threads.h"
#ضمن "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#که_تعریف_شوی USE_BACKTRACE
#ضمن <backtrace.h>
#د_که_پای

بنية memdebug {
  size_t اندازه;
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
ساكن اوږد memsize = 0;  /* set number of mallocs allowed */
#که_تعریف_شوی USE_BACKTRACE
ساكن بنية backtrace_state *btstate;
#د_که_پای

ساكن حرف membuf[10000];
ساكن size_t memwidx = 0; /* write index */

#که_تعریف_شوی USE_MUTEX
ساكن bool dbg_mutex_init = 0;
ساكن curl_mutex_t dbg_mutex;
#د_که_پای

ساكن bool curl_dbg_قلف(فراغ)
{
#که_تعریف_شوی USE_MUTEX
  إذا(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    أرجع TRUE;
  }
#د_که_پای
  أرجع FALSE;
}

ساكن فراغ curl_dbg_unlock(bool was_locked)
{
#که_تعریف_شوی USE_MUTEX
  إذا(was_locked)
    Curl_mutex_release(&dbg_mutex);
#وإلا
  (فراغ)was_locked;
#د_که_پای
}

ساكن فراغ curl_dbg_log_locked(ثابت حرف *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ساكن فراغ curl_dbg_cleanup(فراغ)
{
  bool locked = curl_dbg_قلف();
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
#که_تعریف_شوی USE_MUTEX
  إذا(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#د_که_پای
}

#که_تعریف_شوی USE_BACKTRACE
ساكن فراغ error_bt_callback(فراغ *معلومات, ثابت حرف *پیغام_2,
                              صحيح error_شمېره)
{
  (فراغ)معلومات;
  إذا(error_شمېره == -1)
    curl_dbg_log("compile with -g\n\n");
  وإلا
    curl_dbg_log("Backtrace error %d: %s\n", error_شمېره, پیغام_2);
}

ساكن صحيح ډک_callback(فراغ *معلومات, uintptr_t pc, ثابت حرف *pathname,
                         صحيح کرښه_شمېره, ثابت حرف *function)
{
  (فراغ)معلومات;
  (فراغ)pc;
  إذا(pathname || function || کرښه_شمېره)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, کرښه_شمېره, function);
  أرجع 0;
}

ساكن فراغ dump_bt(فراغ)
{
  backtrace_ډک(btstate, 0, ډک_callback, error_bt_callback, عدم);
}
#وإلا
#تعریف_کړه dump_bt() /* nothing to do */
#د_که_پای

/* this sets the log filename */
فراغ curl_dbg_memdebug(ثابت حرف *logname)
{
  إذا(!curl_dbg_logfile) {
    إذا(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_ټیټ(logname, FOPEN_WRITETEXT);
#که_تعریف_شوی MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    إذا(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (حرف *)عدم);
#د_که_پای
  }
#که_تعریف_شوی USE_MUTEX
  إذا(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#د_که_پای
#که_تعریف_شوی USE_BACKTRACE
  btstate = backtrace_create_state(عدم, 0, error_bt_callback, عدم);
#د_که_پای
  إذا(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
فراغ curl_dbg_memlimit(اوږد حد)
{
  إذا(!memlimit) {
    memlimit = TRUE;
    memsize = حد;
  }
}

/* returns TRUE if this is not allowed! */
ساكن bool countcheck(ثابت حرف *func, صحيح کرښه, ثابت حرف *سرچینه)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  إذا(memlimit && سرچینه) {
    إذا(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", سرچینه, کرښه, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    سرچینه, کرښه, func);
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
فراغ *curl_dbg_malloc(size_t wantedsize, صحيح کرښه, ثابت حرف *سرچینه)
{
  بنية memdebug *mem;
  size_t اندازه;

  DEBUGASSERT(wantedsize != 0);

  إذا(countcheck("malloc", کرښه, سرچینه))
    أرجع عدم;

  /* alloc at least 64 bytes */
  اندازه = حجم(بنية memdebug) + wantedsize;

  mem = Curl_cmalloc(اندازه);
  إذا(mem) {
    mem->اندازه = wantedsize;
  }

  إذا(سرچینه)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 سرچینه, کرښه, wantedsize,
                 mem ? (فراغ *)mem->mem : (فراغ *)0);

  أرجع mem ? mem->mem : عدم;
}

ALLOC_FUNC
فراغ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_اندازه,
                      صحيح کرښه, ثابت حرف *سرچینه)
{
  بنية memdebug *mem;
  size_t اندازه, user_اندازه;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_اندازه != 0);

  إذا(countcheck("calloc", کرښه, سرچینه))
    أرجع عدم;

  /* alloc at least 64 bytes */
  user_اندازه = wanted_اندازه * wanted_elements;
  اندازه = حجم(بنية memdebug) + user_اندازه;

  mem = Curl_ccalloc(1, اندازه);
  إذا(mem)
    mem->اندازه = user_اندازه;

  إذا(سرچینه)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 سرچینه, کرښه, wanted_elements, wanted_اندازه,
                 mem ? (فراغ *)mem->mem : (فراغ *)0);

  أرجع mem ? mem->mem : عدم;
}

ALLOC_FUNC
حرف *curl_dbg_strdup(ثابت حرف *str, صحيح کرښه, ثابت حرف *سرچینه)
{
  حرف *mem;
  size_t len;

  DEBUGASSERT(str);

  إذا(countcheck("strdup", کرښه, سرچینه))
    أرجع عدم;

  len = مزي_اوږدوالی(str) + 1;

  mem = curl_dbg_malloc(len, 0, عدم); /* NULL prevents logging */
  إذا(mem)
    حافظه_نقل(mem, str, len);

  إذا(سرچینه)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 سرچینه, کرښه, (ثابت فراغ *)str, len, (ثابت فراغ *)mem);

  أرجع mem;
}

#إذا defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحيح کرښه, ثابت حرف *سرچینه)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  إذا(countcheck("wcsdup", کرښه, سرچینه))
    أرجع عدم;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * حجم(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, عدم); /* NULL prevents logging */
  إذا(mem)
    حافظه_نقل(mem, str, bsiz);

  إذا(سرچینه)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                سرچینه, کرښه, (ثابت فراغ *)str, bsiz, (فراغ *)mem);

  أرجع mem;
}
#د_که_پای

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
فراغ *curl_dbg_realloc(فراغ *ptr, size_t wantedsize,
                       صحيح کرښه, ثابت حرف *سرچینه)
{
  بنية memdebug *mem = عدم;
  bool was_locked;

  size_t اندازه = حجم(بنية memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  إذا(countcheck("realloc", کرښه, سرچینه))
    أرجع عدم;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_قلف();
#که_تعریف_شوی __INTEL_COMPILER
#  pragma warning(ټېل)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#د_که_پای

  إذا(ptr)
    mem = (فراغ *)((حرف *)ptr - offsetof(بنية memdebug, mem));

#که_تعریف_شوی __INTEL_COMPILER
#  pragma warning(راوباسه)
#د_که_پای

  mem = Curl_crealloc(mem, اندازه);
  إذا(سرچینه)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        سرچینه, کرښه, (فراغ *)ptr, wantedsize,
                        mem ? (فراغ *)mem->mem : (فراغ *)0);

  curl_dbg_unlock(was_locked);
  إذا(mem) {
    mem->اندازه = wantedsize;
    أرجع mem->mem;
  }

  أرجع عدم;
}

فراغ curl_dbg_free(فراغ *ptr, صحيح کرښه, ثابت حرف *سرچینه)
{
  إذا(ptr) {
    بنية memdebug *mem;

    إذا(سرچینه)
      curl_dbg_log("MEM %s:%d free(%p)\n", سرچینه, کرښه, (فراغ *)ptr);

#که_تعریف_شوی __INTEL_COMPILER
#  pragma warning(ټېل)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#د_که_پای

    mem = (فراغ *)((حرف *)ptr - offsetof(بنية memdebug, mem));

#که_تعریف_شوی __INTEL_COMPILER
#  pragma warning(راوباسه)
#د_که_پای

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(صحيح domain, صحيح type, صحيح protocol,
                              صحيح کرښه, ثابت حرف *سرچینه)
{
  curl_socket_t sockfd;

  إذا(countcheck("socket", کرښه, سرچینه))
    أرجع CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  إذا(سرچینه && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 سرچینه, کرښه, sockfd);

  أرجع sockfd;
}

#که_تعریف_شوی HAVE_SOCKETPAIR
صحيح curl_dbg_socketpair(صحيح domain, صحيح type, صحيح protocol,
                        curl_socket_t socket_ویکټور[2],
                        صحيح کرښه, ثابت حرف *سرچینه)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  صحيح res = socketpair(domain, type, protocol, socket_ویکټور);

  إذا(سرچینه && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 سرچینه, کرښه, socket_ویکټور[0], socket_ویکټور[1]);

  أرجع res;
}
#د_که_پای

curl_socket_t curl_dbg_accept(curl_socket_t s, فراغ *saddr, فراغ *saddrlen,
                              صحيح کرښه, ثابت حرف *سرچینه)
{
  بنية sockaddr *addr = (بنية sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  إذا(سرچینه && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 سرچینه, کرښه, sockfd);

  أرجع sockfd;
}

#که_تعریف_شوی HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, فراغ *saddr, فراغ *saddrlen,
                               صحيح flags,
                               صحيح کرښه, ثابت حرف *سرچینه)
{
  بنية sockaddr *addr = (بنية sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  إذا(سرچینه && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 سرچینه, کرښه, sockfd);

  أرجع sockfd;
}
#د_که_پای

/* separate function to allow libcurl to mark a "faked" close */
فراغ curl_dbg_mark_sclose(curl_socket_t sockfd, صحيح کرښه, ثابت حرف *سرچینه)
{
  إذا(سرچینه)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 سرچینه, کرښه, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
صحيح curl_dbg_sclose(curl_socket_t sockfd, صحيح کرښه, ثابت حرف *سرچینه)
{
  curl_dbg_mark_sclose(sockfd, کرښه, سرچینه);
  أرجع CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ثابت حرف *file, ثابت حرف *ډېرپېښ,
                     صحيح کرښه, ثابت حرف *سرچینه)
{
  FILE *res = CURLX_FOPEN_ټیټ(file, ډېرپېښ);
  إذا(سرچینه)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 سرچینه, کرښه, file, ډېرپېښ, (فراغ *)res);

  أرجع res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ثابت حرف *file, ثابت حرف *ډېرپېښ, FILE *fh,
                       صحيح کرښه, ثابت حرف *سرچینه)
{
  FILE *res = CURLX_FREOPEN_ټیټ(file, ډېرپېښ, fh);
  إذا(سرچینه)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 سرچینه, کرښه, file, ډېرپېښ, (فراغ *)fh, (فراغ *)res);

  أرجع res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(صحيح filedes, ثابت حرف *ډېرپېښ,
                      صحيح کرښه, ثابت حرف *سرچینه)
{
  FILE *res = CURLX_FDOPEN_ټیټ(filedes, ډېرپېښ);
  إذا(سرچینه)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 سرچینه, کرښه, filedes, ډېرپېښ, (فراغ *)res);
  أرجع res;
}

صحيح curl_dbg_fclose(FILE *file, صحيح کرښه, ثابت حرف *سرچینه)
{
  صحيح res;

  DEBUGASSERT(file);

  إذا(سرچینه)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", سرچینه, کرښه, (فراغ *)file);

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
    حافظه_نقل(&membuf[memwidx], buf, nchars);
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

  was_locked = curl_dbg_قلف();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#د_که_پای /* CURL_MEMDEBUG */
