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

#اگر_تعریف_شده CURL_MEMDEBUG

#ضمن <stddef.h>  /* for offsetof() */

#ضمن "urldata.h"
#ضمن "curl_threads.h"
#ضمن "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#اگر_تعریف_شده USE_BACKTRACE
#ضمن <backtrace.h>
#پایان_اگر

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
ساكن دراز memsize = 0;  /* set number of mallocs allowed */
#اگر_تعریف_شده USE_BACKTRACE
ساكن بنية backtrace_state *btstate;
#پایان_اگر

ساكن حرف membuf[10000];
ساكن size_t memwidx = 0; /* write index */

#اگر_تعریف_شده USE_MUTEX
ساكن bool dbg_mutex_init = 0;
ساكن curl_mutex_t dbg_mutex;
#پایان_اگر

ساكن bool curl_dbg_قفل(فراغ)
{
#اگر_تعریف_شده USE_MUTEX
  إذا(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    أرجع TRUE;
  }
#پایان_اگر
  أرجع FALSE;
}

ساكن فراغ curl_dbg_unlock(bool was_locked)
{
#اگر_تعریف_شده USE_MUTEX
  إذا(was_locked)
    Curl_mutex_release(&dbg_mutex);
#وإلا
  (فراغ)was_locked;
#پایان_اگر
}

ساكن فراغ curl_dbg_log_locked(ثابت حرف *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ساكن فراغ curl_dbg_cleanup(فراغ)
{
  bool locked = curl_dbg_قفل();
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
#اگر_تعریف_شده USE_MUTEX
  إذا(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#پایان_اگر
}

#اگر_تعریف_شده USE_BACKTRACE
ساكن فراغ error_bt_callback(فراغ *داده, ثابت حرف *پیام_2,
                              صحيح error_عدد)
{
  (فراغ)داده;
  إذا(error_عدد == -1)
    curl_dbg_log("compile with -g\n\n");
  وإلا
    curl_dbg_log("Backtrace error %d: %s\n", error_عدد, پیام_2);
}

ساكن صحيح پر_callback(فراغ *داده, uintptr_t pc, ثابت حرف *pathname,
                         صحيح سطر_عدد, ثابت حرف *function)
{
  (فراغ)داده;
  (فراغ)pc;
  إذا(pathname || function || سطر_عدد)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, سطر_عدد, function);
  أرجع 0;
}

ساكن فراغ dump_bt(فراغ)
{
  backtrace_پر(btstate, 0, پر_callback, error_bt_callback, عدم);
}
#وإلا
#تعریف_کن dump_bt() /* nothing to do */
#پایان_اگر

/* this sets the log filename */
فراغ curl_dbg_memdebug(ثابت حرف *logname)
{
  إذا(!curl_dbg_logfile) {
    إذا(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_کم(logname, FOPEN_WRITETEXT);
#اگر_تعریف_شده MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    إذا(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (حرف *)عدم);
#پایان_اگر
  }
#اگر_تعریف_شده USE_MUTEX
  إذا(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#پایان_اگر
#اگر_تعریف_شده USE_BACKTRACE
  btstate = backtrace_create_state(عدم, 0, error_bt_callback, عدم);
#پایان_اگر
  إذا(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
فراغ curl_dbg_memlimit(دراز حد)
{
  إذا(!memlimit) {
    memlimit = TRUE;
    memsize = حد;
  }
}

/* returns TRUE if this is not allowed! */
ساكن bool countcheck(ثابت حرف *func, صحيح سطر, ثابت حرف *منبع)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  إذا(memlimit && منبع) {
    إذا(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", منبع, سطر, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    منبع, سطر, func);
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
فراغ *curl_dbg_malloc(size_t wantedsize, صحيح سطر, ثابت حرف *منبع)
{
  بنية memdebug *mem;
  size_t اندازه;

  DEBUGASSERT(wantedsize != 0);

  إذا(countcheck("malloc", سطر, منبع))
    أرجع عدم;

  /* alloc at least 64 bytes */
  اندازه = حجم(بنية memdebug) + wantedsize;

  mem = Curl_cmalloc(اندازه);
  إذا(mem) {
    mem->اندازه = wantedsize;
  }

  إذا(منبع)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 منبع, سطر, wantedsize,
                 mem ? (فراغ *)mem->mem : (فراغ *)0);

  أرجع mem ? mem->mem : عدم;
}

ALLOC_FUNC
فراغ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_اندازه,
                      صحيح سطر, ثابت حرف *منبع)
{
  بنية memdebug *mem;
  size_t اندازه, user_اندازه;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_اندازه != 0);

  إذا(countcheck("calloc", سطر, منبع))
    أرجع عدم;

  /* alloc at least 64 bytes */
  user_اندازه = wanted_اندازه * wanted_elements;
  اندازه = حجم(بنية memdebug) + user_اندازه;

  mem = Curl_ccalloc(1, اندازه);
  إذا(mem)
    mem->اندازه = user_اندازه;

  إذا(منبع)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 منبع, سطر, wanted_elements, wanted_اندازه,
                 mem ? (فراغ *)mem->mem : (فراغ *)0);

  أرجع mem ? mem->mem : عدم;
}

ALLOC_FUNC
حرف *curl_dbg_strdup(ثابت حرف *str, صحيح سطر, ثابت حرف *منبع)
{
  حرف *mem;
  size_t len;

  DEBUGASSERT(str);

  إذا(countcheck("strdup", سطر, منبع))
    أرجع عدم;

  len = طول_رشته(str) + 1;

  mem = curl_dbg_malloc(len, 0, عدم); /* NULL prevents logging */
  إذا(mem)
    کپی_حافظه(mem, str, len);

  إذا(منبع)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 منبع, سطر, (ثابت فراغ *)str, len, (ثابت فراغ *)mem);

  أرجع mem;
}

#إذا defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحيح سطر, ثابت حرف *منبع)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  إذا(countcheck("wcsdup", سطر, منبع))
    أرجع عدم;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * حجم(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, عدم); /* NULL prevents logging */
  إذا(mem)
    کپی_حافظه(mem, str, bsiz);

  إذا(منبع)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                منبع, سطر, (ثابت فراغ *)str, bsiz, (فراغ *)mem);

  أرجع mem;
}
#پایان_اگر

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
فراغ *curl_dbg_realloc(فراغ *ptr, size_t wantedsize,
                       صحيح سطر, ثابت حرف *منبع)
{
  بنية memdebug *mem = عدم;
  bool was_locked;

  size_t اندازه = حجم(بنية memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  إذا(countcheck("realloc", سطر, منبع))
    أرجع عدم;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_قفل();
#اگر_تعریف_شده __INTEL_COMPILER
#  pragma warning(افزودن)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#پایان_اگر

  إذا(ptr)
    mem = (فراغ *)((حرف *)ptr - offsetof(بنية memdebug, mem));

#اگر_تعریف_شده __INTEL_COMPILER
#  pragma warning(برداشت)
#پایان_اگر

  mem = Curl_crealloc(mem, اندازه);
  إذا(منبع)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        منبع, سطر, (فراغ *)ptr, wantedsize,
                        mem ? (فراغ *)mem->mem : (فراغ *)0);

  curl_dbg_unlock(was_locked);
  إذا(mem) {
    mem->اندازه = wantedsize;
    أرجع mem->mem;
  }

  أرجع عدم;
}

فراغ curl_dbg_free(فراغ *ptr, صحيح سطر, ثابت حرف *منبع)
{
  إذا(ptr) {
    بنية memdebug *mem;

    إذا(منبع)
      curl_dbg_log("MEM %s:%d free(%p)\n", منبع, سطر, (فراغ *)ptr);

#اگر_تعریف_شده __INTEL_COMPILER
#  pragma warning(افزودن)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#پایان_اگر

    mem = (فراغ *)((حرف *)ptr - offsetof(بنية memdebug, mem));

#اگر_تعریف_شده __INTEL_COMPILER
#  pragma warning(برداشت)
#پایان_اگر

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(صحيح domain, صحيح type, صحيح protocol,
                              صحيح سطر, ثابت حرف *منبع)
{
  curl_socket_t sockfd;

  إذا(countcheck("socket", سطر, منبع))
    أرجع CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  إذا(منبع && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 منبع, سطر, sockfd);

  أرجع sockfd;
}

#اگر_تعریف_شده HAVE_SOCKETPAIR
صحيح curl_dbg_socketpair(صحيح domain, صحيح type, صحيح protocol,
                        curl_socket_t socket_وکتور[2],
                        صحيح سطر, ثابت حرف *منبع)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  صحيح res = socketpair(domain, type, protocol, socket_وکتور);

  إذا(منبع && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 منبع, سطر, socket_وکتور[0], socket_وکتور[1]);

  أرجع res;
}
#پایان_اگر

curl_socket_t curl_dbg_accept(curl_socket_t s, فراغ *saddr, فراغ *saddrlen,
                              صحيح سطر, ثابت حرف *منبع)
{
  بنية sockaddr *addr = (بنية sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  إذا(منبع && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 منبع, سطر, sockfd);

  أرجع sockfd;
}

#اگر_تعریف_شده HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, فراغ *saddr, فراغ *saddrlen,
                               صحيح flags,
                               صحيح سطر, ثابت حرف *منبع)
{
  بنية sockaddr *addr = (بنية sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  إذا(منبع && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 منبع, سطر, sockfd);

  أرجع sockfd;
}
#پایان_اگر

/* separate function to allow libcurl to mark a "faked" close */
فراغ curl_dbg_mark_sclose(curl_socket_t sockfd, صحيح سطر, ثابت حرف *منبع)
{
  إذا(منبع)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 منبع, سطر, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
صحيح curl_dbg_sclose(curl_socket_t sockfd, صحيح سطر, ثابت حرف *منبع)
{
  curl_dbg_mark_sclose(sockfd, سطر, منبع);
  أرجع CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ثابت حرف *file, ثابت حرف *نما,
                     صحيح سطر, ثابت حرف *منبع)
{
  FILE *res = CURLX_FOPEN_کم(file, نما);
  إذا(منبع)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 منبع, سطر, file, نما, (فراغ *)res);

  أرجع res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ثابت حرف *file, ثابت حرف *نما, FILE *fh,
                       صحيح سطر, ثابت حرف *منبع)
{
  FILE *res = CURLX_FREOPEN_کم(file, نما, fh);
  إذا(منبع)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 منبع, سطر, file, نما, (فراغ *)fh, (فراغ *)res);

  أرجع res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(صحيح filedes, ثابت حرف *نما,
                      صحيح سطر, ثابت حرف *منبع)
{
  FILE *res = CURLX_FDOPEN_کم(filedes, نما);
  إذا(منبع)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 منبع, سطر, filedes, نما, (فراغ *)res);
  أرجع res;
}

صحيح curl_dbg_fclose(FILE *file, صحيح سطر, ثابت حرف *منبع)
{
  صحيح res;

  DEBUGASSERT(file);

  إذا(منبع)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", منبع, سطر, (فراغ *)file);

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
    کپی_حافظه(&membuf[memwidx], buf, nchars);
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

  was_locked = curl_dbg_قفل();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#پایان_اگر /* CURL_MEMDEBUG */
