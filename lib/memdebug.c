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

#ifdef CURL_MEMDEBUG

#شامل <stddef.h>  /* for offsetof() */

#شامل "urldata.h"
#شامل "curl_threads.h"
#شامل "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#شامل <backtrace.h>
#endif

ساختار memdebug {
  size_t size;
  اجتماع {
    curl_off_t o;
    دوگانه d;
    تهی *p;
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

FILE *curl_dbg_logfile = پوچ;
ایستا bool registered_cleanup = FALSE; /* atexit registered cleanup */
ایستا bool memlimit = FALSE; /* enable memory limit */
ایستا long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
ایستا ساختار backtrace_state *btstate;
#endif

ایستا نویسه membuf[10000];
ایستا size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
ایستا bool dbg_mutex_init = 0;
ایستا curl_mutex_t dbg_mutex;
#endif

ایستا bool curl_dbg_lock(تهی)
{
#ifdef USE_MUTEX
  اگر(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    بازگشت TRUE;
  }
#endif
  بازگشت FALSE;
}

ایستا تهی curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  اگر(was_locked)
    Curl_mutex_release(&dbg_mutex);
#وگرنه
  (تهی)was_locked;
#endif
}

ایستا تهی curl_dbg_log_locked(ثابت نویسه *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ایستا تهی curl_dbg_cleanup(تهی)
{
  bool locked = curl_dbg_lock();
  اگر(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    اگر(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = پوچ;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  اگر(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
ایستا تهی error_bt_callback(تهی *data, ثابت نویسه *message,
                              صحیح error_عدد)
{
  (تهی)data;
  اگر(error_عدد == -1)
    curl_dbg_log("compile with -g\n\n");
  وگرنه
    curl_dbg_log("Backtrace error %d: %s\n", error_عدد, message);
}

ایستا صحیح full_callback(تهی *data, uintptr_t pc, ثابت نویسه *pathname,
                         صحیح سطر_عدد, ثابت نویسه *function)
{
  (تهی)data;
  (تهی)pc;
  اگر(pathname || function || سطر_عدد)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, سطر_عدد, function);
  بازگشت 0;
}

ایستا تهی dump_bt(تهی)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, پوچ);
}
#وگرنه
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
تهی curl_dbg_memdebug(ثابت نویسه *logname)
{
  اگر(!curl_dbg_logfile) {
    اگر(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_کم(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    اگر(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (نویسه *)پوچ);
#endif
  }
#ifdef USE_MUTEX
  اگر(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(پوچ, 0, error_bt_callback, پوچ);
#endif
  اگر(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
تهی curl_dbg_memlimit(long limit)
{
  اگر(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
ایستا bool countcheck(ثابت نویسه *func, صحیح سطر, ثابت نویسه *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  اگر(memlimit && source) {
    اگر(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, سطر, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, سطر, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      بازگشت TRUE; /* RETURN ERROR! */
    }
    وگرنه
      memsize--; /* countdown */
  }

  بازگشت FALSE; /* allow this */
}

ALLOC_FUNC
تهی *curl_dbg_malloc(size_t wantedsize, صحیح سطر, ثابت نویسه *source)
{
  ساختار memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  اگر(countcheck("malloc", سطر, source))
    بازگشت پوچ;

  /* alloc at least 64 bytes */
  size = اندازه(ساختار memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  اگر(mem) {
    mem->size = wantedsize;
  }

  اگر(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, سطر, wantedsize,
                 mem ? (تهی *)mem->mem : (تهی *)0);

  بازگشت mem ? mem->mem : پوچ;
}

ALLOC_FUNC
تهی *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      صحیح سطر, ثابت نویسه *source)
{
  ساختار memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  اگر(countcheck("calloc", سطر, source))
    بازگشت پوچ;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = اندازه(ساختار memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  اگر(mem)
    mem->size = user_size;

  اگر(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, سطر, wanted_elements, wanted_size,
                 mem ? (تهی *)mem->mem : (تهی *)0);

  بازگشت mem ? mem->mem : پوچ;
}

ALLOC_FUNC
نویسه *curl_dbg_strdup(ثابت نویسه *str, صحیح سطر, ثابت نویسه *source)
{
  نویسه *mem;
  size_t len;

  DEBUGASSERT(str);

  اگر(countcheck("strdup", سطر, source))
    بازگشت پوچ;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, پوچ); /* NULL prevents logging */
  اگر(mem)
    memcpy(mem, str, len);

  اگر(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, سطر, (ثابت تهی *)str, len, (ثابت تهی *)mem);

  بازگشت mem;
}

#اگر defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ثابت wchar_t *str, صحیح سطر, ثابت نویسه *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  اگر(countcheck("wcsdup", سطر, source))
    بازگشت پوچ;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * اندازه(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, پوچ); /* NULL prevents logging */
  اگر(mem)
    memcpy(mem, str, bsiz);

  اگر(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, سطر, (ثابت تهی *)str, bsiz, (تهی *)mem);

  بازگشت mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
تهی *curl_dbg_realloc(تهی *ptr, size_t wantedsize,
                       صحیح سطر, ثابت نویسه *source)
{
  ساختار memdebug *mem = پوچ;
  bool was_locked;

  size_t size = اندازه(ساختار memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  اگر(countcheck("realloc", سطر, source))
    بازگشت پوچ;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(افزودن)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  اگر(ptr)
    mem = (تهی *)((نویسه *)ptr - offsetof(ساختار memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(برداشت)
#endif

  mem = Curl_crealloc(mem, size);
  اگر(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, سطر, (تهی *)ptr, wantedsize,
                        mem ? (تهی *)mem->mem : (تهی *)0);

  curl_dbg_unlock(was_locked);
  اگر(mem) {
    mem->size = wantedsize;
    بازگشت mem->mem;
  }

  بازگشت پوچ;
}

تهی curl_dbg_free(تهی *ptr, صحیح سطر, ثابت نویسه *source)
{
  اگر(ptr) {
    ساختار memdebug *mem;

    اگر(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, سطر, (تهی *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(افزودن)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (تهی *)((نویسه *)ptr - offsetof(ساختار memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(برداشت)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(صحیح domain, صحیح type, صحیح protocol,
                              صحیح سطر, ثابت نویسه *source)
{
  curl_socket_t sockfd;

  اگر(countcheck("socket", سطر, source))
    بازگشت CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  اگر(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, سطر, sockfd);

  بازگشت sockfd;
}

#ifdef HAVE_SOCKETPAIR
صحیح curl_dbg_socketpair(صحیح domain, صحیح type, صحیح protocol,
                        curl_socket_t socket_vector[2],
                        صحیح سطر, ثابت نویسه *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  صحیح res = socketpair(domain, type, protocol, socket_vector);

  اگر(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, سطر, socket_vector[0], socket_vector[1]);

  بازگشت res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, تهی *saddr, تهی *saddrlen,
                              صحیح سطر, ثابت نویسه *source)
{
  ساختار sockaddr *addr = (ساختار sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  اگر(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, سطر, sockfd);

  بازگشت sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, تهی *saddr, تهی *saddrlen,
                               صحیح flags,
                               صحیح سطر, ثابت نویسه *source)
{
  ساختار sockaddr *addr = (ساختار sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  اگر(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, سطر, sockfd);

  بازگشت sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
تهی curl_dbg_mark_sclose(curl_socket_t sockfd, صحیح سطر, ثابت نویسه *source)
{
  اگر(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, سطر, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
صحیح curl_dbg_sclose(curl_socket_t sockfd, صحیح سطر, ثابت نویسه *source)
{
  curl_dbg_mark_sclose(sockfd, سطر, source);
  بازگشت CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ثابت نویسه *file, ثابت نویسه *mode,
                     صحیح سطر, ثابت نویسه *source)
{
  FILE *res = CURLX_FOPEN_کم(file, mode);
  اگر(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, سطر, file, mode, (تهی *)res);

  بازگشت res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ثابت نویسه *file, ثابت نویسه *mode, FILE *fh,
                       صحیح سطر, ثابت نویسه *source)
{
  FILE *res = CURLX_FREOPEN_کم(file, mode, fh);
  اگر(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, سطر, file, mode, (تهی *)fh, (تهی *)res);

  بازگشت res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(صحیح filedes, ثابت نویسه *mode,
                      صحیح سطر, ثابت نویسه *source)
{
  FILE *res = CURLX_FDOPEN_کم(filedes, mode);
  اگر(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, سطر, filedes, mode, (تهی *)res);
  بازگشت res;
}

صحیح curl_dbg_fclose(FILE *file, صحیح سطر, ثابت نویسه *source)
{
  صحیح res;

  DEBUGASSERT(file);

  اگر(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, سطر, (تهی *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  بازگشت res;
}

ایستا تهی curl_dbg_vlog(ثابت نویسه * ثابت fmt,
                          va_list ap) CURL_PRINTF(1, 0);

ایستا تهی curl_dbg_vlog(ثابت نویسه * ثابت fmt, va_list ap)
{
  نویسه buf[1024];
  size_t nchars = curl_mvsnprintf(buf, اندازه(buf), fmt, ap);

  اگر(nchars > (صحیح)اندازه(buf) - 1)
    nchars = (صحیح)اندازه(buf) - 1;

  اگر(nchars > 0) {
    اگر(اندازه(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    اگر(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

ایستا تهی curl_dbg_log_locked(ثابت نویسه *format, ...)
{
  va_list ap;

  اگر(!curl_dbg_logfile)
    بازگشت;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
تهی curl_dbg_log(ثابت نویسه *format, ...)
{
  bool was_locked;
  va_list ap;

  اگر(!curl_dbg_logfile)
    بازگشت;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
