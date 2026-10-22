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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ CURL_MEMDEBUG

#شامل <stddef.h>  /* for offsetof() */

#شامل "urldata.h"
#شامل "curl_threads.h"
#شامل "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#اگر_تعریف_شدہ USE_BACKTRACE
#شامل <backtrace.h>
#اگر_ختم

ساخت memdebug {
  size_t سائز;
  اتحاد {
    curl_off_t o;
    دوہرا d;
    خالی *p;
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

FILE *curl_dbg_logfile = تہی;
ساکن bool registered_cleanup = FALSE; /* atexit registered cleanup */
ساکن bool memlimit = FALSE; /* enable memory limit */
ساکن طویل memsize = 0;  /* set number of mallocs allowed */
#اگر_تعریف_شدہ USE_BACKTRACE
ساکن ساخت backtrace_state *btstate;
#اگر_ختم

ساکن حرف membuf[10000];
ساکن size_t memwidx = 0; /* write index */

#اگر_تعریف_شدہ USE_MUTEX
ساکن bool dbg_mutex_init = 0;
ساکن curl_mutex_t dbg_mutex;
#اگر_ختم

ساکن bool curl_dbg_تالا(خالی)
{
#اگر_تعریف_شدہ USE_MUTEX
  اگر(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    واپس TRUE;
  }
#اگر_ختم
  واپس FALSE;
}

ساکن خالی curl_dbg_unlock(bool was_locked)
{
#اگر_تعریف_شدہ USE_MUTEX
  اگر(was_locked)
    Curl_mutex_release(&dbg_mutex);
#ورنہ
  (خالی)was_locked;
#اگر_ختم
}

ساکن خالی curl_dbg_log_locked(ثابت حرف *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ساکن خالی curl_dbg_cleanup(خالی)
{
  bool locked = curl_dbg_تالا();
  اگر(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    اگر(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = تہی;
  curl_dbg_unlock(locked);
#اگر_تعریف_شدہ USE_MUTEX
  اگر(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#اگر_ختم
}

#اگر_تعریف_شدہ USE_BACKTRACE
ساکن خالی error_bt_callback(خالی *ڈیٹا, ثابت حرف *پیغام_2,
                              صحیح error_نمبر)
{
  (خالی)ڈیٹا;
  اگر(error_نمبر == -1)
    curl_dbg_log("compile with -g\n\n");
  ورنہ
    curl_dbg_log("Backtrace error %d: %s\n", error_نمبر, پیغام_2);
}

ساکن صحیح بھرا_callback(خالی *ڈیٹا, uintptr_t pc, ثابت حرف *pathname,
                         صحیح سطر_نمبر, ثابت حرف *function)
{
  (خالی)ڈیٹا;
  (خالی)pc;
  اگر(pathname || function || سطر_نمبر)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, سطر_نمبر, function);
  واپس 0;
}

ساکن خالی dump_bt(خالی)
{
  backtrace_بھرا(btstate, 0, بھرا_callback, error_bt_callback, تہی);
}
#ورنہ
#تعریف_کرو dump_bt() /* nothing to do */
#اگر_ختم

/* this sets the log filename */
خالی curl_dbg_memdebug(ثابت حرف *logname)
{
  اگر(!curl_dbg_logfile) {
    اگر(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_کم(logname, FOPEN_WRITETEXT);
#اگر_تعریف_شدہ MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    اگر(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (حرف *)تہی);
#اگر_ختم
  }
#اگر_تعریف_شدہ USE_MUTEX
  اگر(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#اگر_ختم
#اگر_تعریف_شدہ USE_BACKTRACE
  btstate = backtrace_create_state(تہی, 0, error_bt_callback, تہی);
#اگر_ختم
  اگر(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
خالی curl_dbg_memlimit(طویل حد_بندی)
{
  اگر(!memlimit) {
    memlimit = TRUE;
    memsize = حد_بندی;
  }
}

/* returns TRUE if this is not allowed! */
ساکن bool countcheck(ثابت حرف *func, صحیح سطر, ثابت حرف *ماخذ)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  اگر(memlimit && ماخذ) {
    اگر(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", ماخذ, سطر, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    ماخذ, سطر, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      واپس TRUE; /* RETURN ERROR! */
    }
    ورنہ
      memsize--; /* countdown */
  }

  واپس FALSE; /* allow this */
}

ALLOC_FUNC
خالی *curl_dbg_malloc(size_t wantedsize, صحیح سطر, ثابت حرف *ماخذ)
{
  ساخت memdebug *mem;
  size_t سائز;

  DEBUGASSERT(wantedsize != 0);

  اگر(countcheck("malloc", سطر, ماخذ))
    واپس تہی;

  /* alloc at least 64 bytes */
  سائز = حجم(ساخت memdebug) + wantedsize;

  mem = Curl_cmalloc(سائز);
  اگر(mem) {
    mem->سائز = wantedsize;
  }

  اگر(ماخذ)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 ماخذ, سطر, wantedsize,
                 mem ? (خالی *)mem->mem : (خالی *)0);

  واپس mem ? mem->mem : تہی;
}

ALLOC_FUNC
خالی *curl_dbg_calloc(size_t wanted_elements, size_t wanted_سائز,
                      صحیح سطر, ثابت حرف *ماخذ)
{
  ساخت memdebug *mem;
  size_t سائز, user_سائز;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_سائز != 0);

  اگر(countcheck("calloc", سطر, ماخذ))
    واپس تہی;

  /* alloc at least 64 bytes */
  user_سائز = wanted_سائز * wanted_elements;
  سائز = حجم(ساخت memdebug) + user_سائز;

  mem = Curl_ccalloc(1, سائز);
  اگر(mem)
    mem->سائز = user_سائز;

  اگر(ماخذ)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 ماخذ, سطر, wanted_elements, wanted_سائز,
                 mem ? (خالی *)mem->mem : (خالی *)0);

  واپس mem ? mem->mem : تہی;
}

ALLOC_FUNC
حرف *curl_dbg_strdup(ثابت حرف *str, صحیح سطر, ثابت حرف *ماخذ)
{
  حرف *mem;
  size_t len;

  DEBUGASSERT(str);

  اگر(countcheck("strdup", سطر, ماخذ))
    واپس تہی;

  len = لڑی_لمبائی(str) + 1;

  mem = curl_dbg_malloc(len, 0, تہی); /* NULL prevents logging */
  اگر(mem)
    حافظہ_نقل(mem, str, len);

  اگر(ماخذ)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 ماخذ, سطر, (ثابت خالی *)str, len, (ثابت خالی *)mem);

  واپس mem;
}

#اگر defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحیح سطر, ثابت حرف *ماخذ)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  اگر(countcheck("wcsdup", سطر, ماخذ))
    واپس تہی;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * حجم(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, تہی); /* NULL prevents logging */
  اگر(mem)
    حافظہ_نقل(mem, str, bsiz);

  اگر(ماخذ)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                ماخذ, سطر, (ثابت خالی *)str, bsiz, (خالی *)mem);

  واپس mem;
}
#اگر_ختم

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
خالی *curl_dbg_realloc(خالی *ptr, size_t wantedsize,
                       صحیح سطر, ثابت حرف *ماخذ)
{
  ساخت memdebug *mem = تہی;
  bool was_locked;

  size_t سائز = حجم(ساخت memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  اگر(countcheck("realloc", سطر, ماخذ))
    واپس تہی;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_تالا();
#اگر_تعریف_شدہ __INTEL_COMPILER
#  pragma warning(ڈالو)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#اگر_ختم

  اگر(ptr)
    mem = (خالی *)((حرف *)ptr - offsetof(ساخت memdebug, mem));

#اگر_تعریف_شدہ __INTEL_COMPILER
#  pragma warning(نکالو)
#اگر_ختم

  mem = Curl_crealloc(mem, سائز);
  اگر(ماخذ)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        ماخذ, سطر, (خالی *)ptr, wantedsize,
                        mem ? (خالی *)mem->mem : (خالی *)0);

  curl_dbg_unlock(was_locked);
  اگر(mem) {
    mem->سائز = wantedsize;
    واپس mem->mem;
  }

  واپس تہی;
}

خالی curl_dbg_free(خالی *ptr, صحیح سطر, ثابت حرف *ماخذ)
{
  اگر(ptr) {
    ساخت memdebug *mem;

    اگر(ماخذ)
      curl_dbg_log("MEM %s:%d free(%p)\n", ماخذ, سطر, (خالی *)ptr);

#اگر_تعریف_شدہ __INTEL_COMPILER
#  pragma warning(ڈالو)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#اگر_ختم

    mem = (خالی *)((حرف *)ptr - offsetof(ساخت memdebug, mem));

#اگر_تعریف_شدہ __INTEL_COMPILER
#  pragma warning(نکالو)
#اگر_ختم

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(صحیح domain, صحیح type, صحیح protocol,
                              صحیح سطر, ثابت حرف *ماخذ)
{
  curl_socket_t sockfd;

  اگر(countcheck("socket", سطر, ماخذ))
    واپس CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  اگر(ماخذ && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 ماخذ, سطر, sockfd);

  واپس sockfd;
}

#اگر_تعریف_شدہ HAVE_SOCKETPAIR
صحیح curl_dbg_socketpair(صحیح domain, صحیح type, صحیح protocol,
                        curl_socket_t socket_سمتیہ[2],
                        صحیح سطر, ثابت حرف *ماخذ)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  صحیح res = socketpair(domain, type, protocol, socket_سمتیہ);

  اگر(ماخذ && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 ماخذ, سطر, socket_سمتیہ[0], socket_سمتیہ[1]);

  واپس res;
}
#اگر_ختم

curl_socket_t curl_dbg_accept(curl_socket_t s, خالی *saddr, خالی *saddrlen,
                              صحیح سطر, ثابت حرف *ماخذ)
{
  ساخت sockaddr *addr = (ساخت sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  اگر(ماخذ && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ماخذ, سطر, sockfd);

  واپس sockfd;
}

#اگر_تعریف_شدہ HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, خالی *saddr, خالی *saddrlen,
                               صحیح flags,
                               صحیح سطر, ثابت حرف *ماخذ)
{
  ساخت sockaddr *addr = (ساخت sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  اگر(ماخذ && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ماخذ, سطر, sockfd);

  واپس sockfd;
}
#اگر_ختم

/* separate function to allow libcurl to mark a "faked" close */
خالی curl_dbg_mark_sclose(curl_socket_t sockfd, صحیح سطر, ثابت حرف *ماخذ)
{
  اگر(ماخذ)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 ماخذ, سطر, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
صحیح curl_dbg_sclose(curl_socket_t sockfd, صحیح سطر, ثابت حرف *ماخذ)
{
  curl_dbg_mark_sclose(sockfd, سطر, ماخذ);
  واپس CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ثابت حرف *file, ثابت حرف *منوال,
                     صحیح سطر, ثابت حرف *ماخذ)
{
  FILE *res = CURLX_FOPEN_کم(file, منوال);
  اگر(ماخذ)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 ماخذ, سطر, file, منوال, (خالی *)res);

  واپس res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ثابت حرف *file, ثابت حرف *منوال, FILE *fh,
                       صحیح سطر, ثابت حرف *ماخذ)
{
  FILE *res = CURLX_FREOPEN_کم(file, منوال, fh);
  اگر(ماخذ)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 ماخذ, سطر, file, منوال, (خالی *)fh, (خالی *)res);

  واپس res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(صحیح filedes, ثابت حرف *منوال,
                      صحیح سطر, ثابت حرف *ماخذ)
{
  FILE *res = CURLX_FDOPEN_کم(filedes, منوال);
  اگر(ماخذ)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 ماخذ, سطر, filedes, منوال, (خالی *)res);
  واپس res;
}

صحیح curl_dbg_fclose(FILE *file, صحیح سطر, ثابت حرف *ماخذ)
{
  صحیح res;

  DEBUGASSERT(file);

  اگر(ماخذ)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", ماخذ, سطر, (خالی *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  واپس res;
}

ساکن خالی curl_dbg_vlog(ثابت حرف * ثابت fmt,
                          va_list ap) CURL_PRINTF(1, 0);

ساکن خالی curl_dbg_vlog(ثابت حرف * ثابت fmt, va_list ap)
{
  حرف buf[1024];
  size_t nchars = curl_mvsnprintf(buf, حجم(buf), fmt, ap);

  اگر(nchars > (صحیح)حجم(buf) - 1)
    nchars = (صحیح)حجم(buf) - 1;

  اگر(nchars > 0) {
    اگر(حجم(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    اگر(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    حافظہ_نقل(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

ساکن خالی curl_dbg_log_locked(ثابت حرف *format, ...)
{
  va_list ap;

  اگر(!curl_dbg_logfile)
    واپس;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
خالی curl_dbg_log(ثابت حرف *format, ...)
{
  bool was_locked;
  va_list ap;

  اگر(!curl_dbg_logfile)
    واپس;

  was_locked = curl_dbg_تالا();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#اگر_ختم /* CURL_MEMDEBUG */
