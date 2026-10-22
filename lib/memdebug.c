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
#অন্তর্ভাব "curl_setup.h"

#ifdef CURL_MEMDEBUG

#অন্তর্ভাব <stddef.h>  /* for offsetof() */

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_threads.h"
#অন্তর্ভাব "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#অন্তর্ভাব <backtrace.h>
#endif

কাঠামো memdebug {
  size_t আকার_2;
  সংঘ {
    curl_off_t o;
    দ্বিগুণ d;
    শূন্য *p;
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

FILE *curl_dbg_logfile = ফাঁকা;
স্থির bool registered_cleanup = FALSE; /* atexit registered cleanup */
স্থির bool memlimit = FALSE; /* enable memory limit */
স্থির long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
স্থির কাঠামো backtrace_state *btstate;
#endif

স্থির অক্ষর membuf[10000];
স্থির size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
স্থির bool dbg_mutex_init = 0;
স্থির curl_mutex_t dbg_mutex;
#endif

স্থির bool curl_dbg_lock(শূন্য)
{
#ifdef USE_MUTEX
  যদি(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    ফেরত TRUE;
  }
#endif
  ফেরত FALSE;
}

স্থির শূন্য curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  যদি(was_locked)
    Curl_mutex_release(&dbg_mutex);
#অন্যথা
  (শূন্য)was_locked;
#endif
}

স্থির শূন্য curl_dbg_log_locked(ধ্রুবক অক্ষর *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
স্থির শূন্য curl_dbg_cleanup(শূন্য)
{
  bool locked = curl_dbg_lock();
  যদি(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    যদি(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = ফাঁকা;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  যদি(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
স্থির শূন্য error_bt_callback(শূন্য *তথ্য, ধ্রুবক অক্ষর *message,
                              পূর্ণসংখ্যা error_সংখ্যা)
{
  (শূন্য)তথ্য;
  যদি(error_সংখ্যা == -1)
    curl_dbg_log("compile with -g\n\n");
  অন্যথা
    curl_dbg_log("Backtrace error %d: %s\n", error_সংখ্যা, message);
}

স্থির পূর্ণসংখ্যা full_callback(শূন্য *তথ্য, uintptr_t pc, ধ্রুবক অক্ষর *pathname,
                         পূর্ণসংখ্যা সারি_সংখ্যা, ধ্রুবক অক্ষর *function)
{
  (শূন্য)তথ্য;
  (শূন্য)pc;
  যদি(pathname || function || সারি_সংখ্যা)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, সারি_সংখ্যা, function);
  ফেরত 0;
}

স্থির শূন্য dump_bt(শূন্য)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, ফাঁকা);
}
#অন্যথা
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
শূন্য curl_dbg_memdebug(ধ্রুবক অক্ষর *logname)
{
  যদি(!curl_dbg_logfile) {
    যদি(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_নিচের(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    যদি(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (অক্ষর *)ফাঁকা);
#endif
  }
#ifdef USE_MUTEX
  যদি(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(ফাঁকা, 0, error_bt_callback, ফাঁকা);
#endif
  যদি(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
শূন্য curl_dbg_memlimit(long limit)
{
  যদি(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
স্থির bool countcheck(ধ্রুবক অক্ষর *func, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  যদি(memlimit && source) {
    যদি(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, সারি, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, সারি, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      ফেরত TRUE; /* RETURN ERROR! */
    }
    অন্যথা
      memsize--; /* countdown */
  }

  ফেরত FALSE; /* allow this */
}

ALLOC_FUNC
শূন্য *curl_dbg_malloc(size_t wantedsize, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  কাঠামো memdebug *mem;
  size_t আকার_2;

  DEBUGASSERT(wantedsize != 0);

  যদি(countcheck("malloc", সারি, source))
    ফেরত ফাঁকা;

  /* alloc at least 64 bytes */
  আকার_2 = আকার(কাঠামো memdebug) + wantedsize;

  mem = Curl_cmalloc(আকার_2);
  যদি(mem) {
    mem->আকার_2 = wantedsize;
  }

  যদি(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, সারি, wantedsize,
                 mem ? (শূন্য *)mem->mem : (শূন্য *)0);

  ফেরত mem ? mem->mem : ফাঁকা;
}

ALLOC_FUNC
শূন্য *curl_dbg_calloc(size_t wanted_elements, size_t wanted_আকার,
                      পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  কাঠামো memdebug *mem;
  size_t আকার_2, user_আকার;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_আকার != 0);

  যদি(countcheck("calloc", সারি, source))
    ফেরত ফাঁকা;

  /* alloc at least 64 bytes */
  user_আকার = wanted_আকার * wanted_elements;
  আকার_2 = আকার(কাঠামো memdebug) + user_আকার;

  mem = Curl_ccalloc(1, আকার_2);
  যদি(mem)
    mem->আকার_2 = user_আকার;

  যদি(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, সারি, wanted_elements, wanted_আকার,
                 mem ? (শূন্য *)mem->mem : (শূন্য *)0);

  ফেরত mem ? mem->mem : ফাঁকা;
}

ALLOC_FUNC
অক্ষর *curl_dbg_strdup(ধ্রুবক অক্ষর *str, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  অক্ষর *mem;
  size_t len;

  DEBUGASSERT(str);

  যদি(countcheck("strdup", সারি, source))
    ফেরত ফাঁকা;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, ফাঁকা); /* NULL prevents logging */
  যদি(mem)
    memcpy(mem, str, len);

  যদি(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, সারি, (ধ্রুবক শূন্য *)str, len, (ধ্রুবক শূন্য *)mem);

  ফেরত mem;
}

#যদি defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ধ্রুবক wchar_t *str, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  যদি(countcheck("wcsdup", সারি, source))
    ফেরত ফাঁকা;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * আকার(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, ফাঁকা); /* NULL prevents logging */
  যদি(mem)
    memcpy(mem, str, bsiz);

  যদি(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, সারি, (ধ্রুবক শূন্য *)str, bsiz, (শূন্য *)mem);

  ফেরত mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
শূন্য *curl_dbg_realloc(শূন্য *ptr, size_t wantedsize,
                       পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  কাঠামো memdebug *mem = ফাঁকা;
  bool was_locked;

  size_t আকার_2 = আকার(কাঠামো memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  যদি(countcheck("realloc", সারি, source))
    ফেরত ফাঁকা;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(ঠেলো)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  যদি(ptr)
    mem = (শূন্য *)((অক্ষর *)ptr - offsetof(কাঠামো memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(বের_করো)
#endif

  mem = Curl_crealloc(mem, আকার_2);
  যদি(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, সারি, (শূন্য *)ptr, wantedsize,
                        mem ? (শূন্য *)mem->mem : (শূন্য *)0);

  curl_dbg_unlock(was_locked);
  যদি(mem) {
    mem->আকার_2 = wantedsize;
    ফেরত mem->mem;
  }

  ফেরত ফাঁকা;
}

শূন্য curl_dbg_free(শূন্য *ptr, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  যদি(ptr) {
    কাঠামো memdebug *mem;

    যদি(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, সারি, (শূন্য *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(ঠেলো)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (শূন্য *)((অক্ষর *)ptr - offsetof(কাঠামো memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(বের_করো)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(পূর্ণসংখ্যা domain, পূর্ণসংখ্যা type, পূর্ণসংখ্যা protocol,
                              পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  curl_socket_t sockfd;

  যদি(countcheck("socket", সারি, source))
    ফেরত CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  যদি(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, সারি, sockfd);

  ফেরত sockfd;
}

#ifdef HAVE_SOCKETPAIR
পূর্ণসংখ্যা curl_dbg_socketpair(পূর্ণসংখ্যা domain, পূর্ণসংখ্যা type, পূর্ণসংখ্যা protocol,
                        curl_socket_t socket_vector[2],
                        পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  পূর্ণসংখ্যা res = socketpair(domain, type, protocol, socket_vector);

  যদি(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, সারি, socket_vector[0], socket_vector[1]);

  ফেরত res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, শূন্য *saddr, শূন্য *saddrlen,
                              পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  কাঠামো sockaddr *addr = (কাঠামো sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  যদি(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, সারি, sockfd);

  ফেরত sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, শূন্য *saddr, শূন্য *saddrlen,
                               পূর্ণসংখ্যা flags,
                               পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  কাঠামো sockaddr *addr = (কাঠামো sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  যদি(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, সারি, sockfd);

  ফেরত sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
শূন্য curl_dbg_mark_sclose(curl_socket_t sockfd, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  যদি(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, সারি, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
পূর্ণসংখ্যা curl_dbg_sclose(curl_socket_t sockfd, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  curl_dbg_mark_sclose(sockfd, সারি, source);
  ফেরত CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ধ্রুবক অক্ষর *file, ধ্রুবক অক্ষর *mode,
                     পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  FILE *res = CURLX_FOPEN_নিচের(file, mode);
  যদি(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, সারি, file, mode, (শূন্য *)res);

  ফেরত res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ধ্রুবক অক্ষর *file, ধ্রুবক অক্ষর *mode, FILE *fh,
                       পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  FILE *res = CURLX_FREOPEN_নিচের(file, mode, fh);
  যদি(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, সারি, file, mode, (শূন্য *)fh, (শূন্য *)res);

  ফেরত res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(পূর্ণসংখ্যা filedes, ধ্রুবক অক্ষর *mode,
                      পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  FILE *res = CURLX_FDOPEN_নিচের(filedes, mode);
  যদি(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, সারি, filedes, mode, (শূন্য *)res);
  ফেরত res;
}

পূর্ণসংখ্যা curl_dbg_fclose(FILE *file, পূর্ণসংখ্যা সারি, ধ্রুবক অক্ষর *source)
{
  পূর্ণসংখ্যা res;

  DEBUGASSERT(file);

  যদি(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, সারি, (শূন্য *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  ফেরত res;
}

স্থির শূন্য curl_dbg_vlog(ধ্রুবক অক্ষর * ধ্রুবক fmt,
                          va_list ap) CURL_PRINTF(1, 0);

স্থির শূন্য curl_dbg_vlog(ধ্রুবক অক্ষর * ধ্রুবক fmt, va_list ap)
{
  অক্ষর buf[1024];
  size_t nchars = curl_mvsnprintf(buf, আকার(buf), fmt, ap);

  যদি(nchars > (পূর্ণসংখ্যা)আকার(buf) - 1)
    nchars = (পূর্ণসংখ্যা)আকার(buf) - 1;

  যদি(nchars > 0) {
    যদি(আকার(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    যদি(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

স্থির শূন্য curl_dbg_log_locked(ধ্রুবক অক্ষর *format, ...)
{
  va_list ap;

  যদি(!curl_dbg_logfile)
    ফেরত;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
শূন্য curl_dbg_log(ধ্রুবক অক্ষর *format, ...)
{
  bool was_locked;
  va_list ap;

  যদি(!curl_dbg_logfile)
    ফেরত;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
