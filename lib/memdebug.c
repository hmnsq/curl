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

#جيڪڏهن_وصف_ٿيل CURL_MEMDEBUG

#ضمن <stddef.h>  /* for offsetof() */

#ضمن "urldata.h"
#ضمن "curl_threads.h"
#ضمن "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#جيڪڏهن_وصف_ٿيل USE_BACKTRACE
#ضمن <backtrace.h>
#جيڪڏهن_پورو

بنية memdebug {
  size_t سائيز;
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
ساكن ڊگهو memsize = 0;  /* set number of mallocs allowed */
#جيڪڏهن_وصف_ٿيل USE_BACKTRACE
ساكن بنية backtrace_state *btstate;
#جيڪڏهن_پورو

ساكن حرف membuf[10000];
ساكن size_t memwidx = 0; /* write index */

#جيڪڏهن_وصف_ٿيل USE_MUTEX
ساكن bool dbg_mutex_init = 0;
ساكن curl_mutex_t dbg_mutex;
#جيڪڏهن_پورو

ساكن bool curl_dbg_تالو(فراغ)
{
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  إذا(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    أرجع TRUE;
  }
#جيڪڏهن_پورو
  أرجع FALSE;
}

ساكن فراغ curl_dbg_unlock(bool was_locked)
{
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  إذا(was_locked)
    Curl_mutex_release(&dbg_mutex);
#وإلا
  (فراغ)was_locked;
#جيڪڏهن_پورو
}

ساكن فراغ curl_dbg_log_locked(ثابت حرف *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ساكن فراغ curl_dbg_cleanup(فراغ)
{
  bool locked = curl_dbg_تالو();
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
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  إذا(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل USE_BACKTRACE
ساكن فراغ error_bt_callback(فراغ *معلومات, ثابت حرف *پيغام,
                              صحيح error_عدد)
{
  (فراغ)معلومات;
  إذا(error_عدد == -1)
    curl_dbg_log("compile with -g\n\n");
  وإلا
    curl_dbg_log("Backtrace error %d: %s\n", error_عدد, پيغام);
}

ساكن صحيح ڀريل_callback(فراغ *معلومات, uintptr_t pc, ثابت حرف *pathname,
                         صحيح سٽ_عدد, ثابت حرف *function)
{
  (فراغ)معلومات;
  (فراغ)pc;
  إذا(pathname || function || سٽ_عدد)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, سٽ_عدد, function);
  أرجع 0;
}

ساكن فراغ dump_bt(فراغ)
{
  backtrace_ڀريل(btstate, 0, ڀريل_callback, error_bt_callback, عدم);
}
#وإلا
#وصف_ڪريو dump_bt() /* nothing to do */
#جيڪڏهن_پورو

/* this sets the log filename */
فراغ curl_dbg_memdebug(ثابت حرف *logname)
{
  إذا(!curl_dbg_logfile) {
    إذا(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_هيٺيون(logname, FOPEN_WRITETEXT);
#جيڪڏهن_وصف_ٿيل MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    إذا(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (حرف *)عدم);
#جيڪڏهن_پورو
  }
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  إذا(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_BACKTRACE
  btstate = backtrace_create_state(عدم, 0, error_bt_callback, عدم);
#جيڪڏهن_پورو
  إذا(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
فراغ curl_dbg_memlimit(ڊگهو حد_بندي)
{
  إذا(!memlimit) {
    memlimit = TRUE;
    memsize = حد_بندي;
  }
}

/* returns TRUE if this is not allowed! */
ساكن bool countcheck(ثابت حرف *func, صحيح سٽ, ثابت حرف *ذريعو)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  إذا(memlimit && ذريعو) {
    إذا(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", ذريعو, سٽ, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    ذريعو, سٽ, func);
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
فراغ *curl_dbg_malloc(size_t wantedsize, صحيح سٽ, ثابت حرف *ذريعو)
{
  بنية memdebug *mem;
  size_t سائيز;

  DEBUGASSERT(wantedsize != 0);

  إذا(countcheck("malloc", سٽ, ذريعو))
    أرجع عدم;

  /* alloc at least 64 bytes */
  سائيز = حجم(بنية memdebug) + wantedsize;

  mem = Curl_cmalloc(سائيز);
  إذا(mem) {
    mem->سائيز = wantedsize;
  }

  إذا(ذريعو)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 ذريعو, سٽ, wantedsize,
                 mem ? (فراغ *)mem->mem : (فراغ *)0);

  أرجع mem ? mem->mem : عدم;
}

ALLOC_FUNC
فراغ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_سائيز,
                      صحيح سٽ, ثابت حرف *ذريعو)
{
  بنية memdebug *mem;
  size_t سائيز, user_سائيز;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_سائيز != 0);

  إذا(countcheck("calloc", سٽ, ذريعو))
    أرجع عدم;

  /* alloc at least 64 bytes */
  user_سائيز = wanted_سائيز * wanted_elements;
  سائيز = حجم(بنية memdebug) + user_سائيز;

  mem = Curl_ccalloc(1, سائيز);
  إذا(mem)
    mem->سائيز = user_سائيز;

  إذا(ذريعو)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 ذريعو, سٽ, wanted_elements, wanted_سائيز,
                 mem ? (فراغ *)mem->mem : (فراغ *)0);

  أرجع mem ? mem->mem : عدم;
}

ALLOC_FUNC
حرف *curl_dbg_strdup(ثابت حرف *str, صحيح سٽ, ثابت حرف *ذريعو)
{
  حرف *mem;
  size_t len;

  DEBUGASSERT(str);

  إذا(countcheck("strdup", سٽ, ذريعو))
    أرجع عدم;

  len = لڙي_ڊيگهه(str) + 1;

  mem = curl_dbg_malloc(len, 0, عدم); /* NULL prevents logging */
  إذا(mem)
    يادگيري_نقل(mem, str, len);

  إذا(ذريعو)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 ذريعو, سٽ, (ثابت فراغ *)str, len, (ثابت فراغ *)mem);

  أرجع mem;
}

#إذا defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحيح سٽ, ثابت حرف *ذريعو)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  إذا(countcheck("wcsdup", سٽ, ذريعو))
    أرجع عدم;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * حجم(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, عدم); /* NULL prevents logging */
  إذا(mem)
    يادگيري_نقل(mem, str, bsiz);

  إذا(ذريعو)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                ذريعو, سٽ, (ثابت فراغ *)str, bsiz, (فراغ *)mem);

  أرجع mem;
}
#جيڪڏهن_پورو

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
فراغ *curl_dbg_realloc(فراغ *ptr, size_t wantedsize,
                       صحيح سٽ, ثابت حرف *ذريعو)
{
  بنية memdebug *mem = عدم;
  bool was_locked;

  size_t سائيز = حجم(بنية memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  إذا(countcheck("realloc", سٽ, ذريعو))
    أرجع عدم;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_تالو();
#جيڪڏهن_وصف_ٿيل __INTEL_COMPILER
#  pragma warning(ڌڪو)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#جيڪڏهن_پورو

  إذا(ptr)
    mem = (فراغ *)((حرف *)ptr - offsetof(بنية memdebug, mem));

#جيڪڏهن_وصف_ٿيل __INTEL_COMPILER
#  pragma warning(ڪڍو)
#جيڪڏهن_پورو

  mem = Curl_crealloc(mem, سائيز);
  إذا(ذريعو)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        ذريعو, سٽ, (فراغ *)ptr, wantedsize,
                        mem ? (فراغ *)mem->mem : (فراغ *)0);

  curl_dbg_unlock(was_locked);
  إذا(mem) {
    mem->سائيز = wantedsize;
    أرجع mem->mem;
  }

  أرجع عدم;
}

فراغ curl_dbg_free(فراغ *ptr, صحيح سٽ, ثابت حرف *ذريعو)
{
  إذا(ptr) {
    بنية memdebug *mem;

    إذا(ذريعو)
      curl_dbg_log("MEM %s:%d free(%p)\n", ذريعو, سٽ, (فراغ *)ptr);

#جيڪڏهن_وصف_ٿيل __INTEL_COMPILER
#  pragma warning(ڌڪو)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#جيڪڏهن_پورو

    mem = (فراغ *)((حرف *)ptr - offsetof(بنية memdebug, mem));

#جيڪڏهن_وصف_ٿيل __INTEL_COMPILER
#  pragma warning(ڪڍو)
#جيڪڏهن_پورو

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(صحيح domain, صحيح type, صحيح protocol,
                              صحيح سٽ, ثابت حرف *ذريعو)
{
  curl_socket_t sockfd;

  إذا(countcheck("socket", سٽ, ذريعو))
    أرجع CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  إذا(ذريعو && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 ذريعو, سٽ, sockfd);

  أرجع sockfd;
}

#جيڪڏهن_وصف_ٿيل HAVE_SOCKETPAIR
صحيح curl_dbg_socketpair(صحيح domain, صحيح type, صحيح protocol,
                        curl_socket_t socket_ويڪٽر[2],
                        صحيح سٽ, ثابت حرف *ذريعو)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  صحيح res = socketpair(domain, type, protocol, socket_ويڪٽر);

  إذا(ذريعو && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 ذريعو, سٽ, socket_ويڪٽر[0], socket_ويڪٽر[1]);

  أرجع res;
}
#جيڪڏهن_پورو

curl_socket_t curl_dbg_accept(curl_socket_t s, فراغ *saddr, فراغ *saddrlen,
                              صحيح سٽ, ثابت حرف *ذريعو)
{
  بنية sockaddr *addr = (بنية sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  إذا(ذريعو && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ذريعو, سٽ, sockfd);

  أرجع sockfd;
}

#جيڪڏهن_وصف_ٿيل HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, فراغ *saddr, فراغ *saddrlen,
                               صحيح flags,
                               صحيح سٽ, ثابت حرف *ذريعو)
{
  بنية sockaddr *addr = (بنية sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  إذا(ذريعو && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ذريعو, سٽ, sockfd);

  أرجع sockfd;
}
#جيڪڏهن_پورو

/* separate function to allow libcurl to mark a "faked" close */
فراغ curl_dbg_mark_sclose(curl_socket_t sockfd, صحيح سٽ, ثابت حرف *ذريعو)
{
  إذا(ذريعو)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 ذريعو, سٽ, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
صحيح curl_dbg_sclose(curl_socket_t sockfd, صحيح سٽ, ثابت حرف *ذريعو)
{
  curl_dbg_mark_sclose(sockfd, سٽ, ذريعو);
  أرجع CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ثابت حرف *file, ثابت حرف *گهڻائي,
                     صحيح سٽ, ثابت حرف *ذريعو)
{
  FILE *res = CURLX_FOPEN_هيٺيون(file, گهڻائي);
  إذا(ذريعو)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 ذريعو, سٽ, file, گهڻائي, (فراغ *)res);

  أرجع res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ثابت حرف *file, ثابت حرف *گهڻائي, FILE *fh,
                       صحيح سٽ, ثابت حرف *ذريعو)
{
  FILE *res = CURLX_FREOPEN_هيٺيون(file, گهڻائي, fh);
  إذا(ذريعو)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 ذريعو, سٽ, file, گهڻائي, (فراغ *)fh, (فراغ *)res);

  أرجع res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(صحيح filedes, ثابت حرف *گهڻائي,
                      صحيح سٽ, ثابت حرف *ذريعو)
{
  FILE *res = CURLX_FDOPEN_هيٺيون(filedes, گهڻائي);
  إذا(ذريعو)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 ذريعو, سٽ, filedes, گهڻائي, (فراغ *)res);
  أرجع res;
}

صحيح curl_dbg_fclose(FILE *file, صحيح سٽ, ثابت حرف *ذريعو)
{
  صحيح res;

  DEBUGASSERT(file);

  إذا(ذريعو)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", ذريعو, سٽ, (فراغ *)file);

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
    يادگيري_نقل(&membuf[memwidx], buf, nchars);
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

  was_locked = curl_dbg_تالو();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#جيڪڏهن_پورو /* CURL_MEMDEBUG */
