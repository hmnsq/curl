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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef CURL_MEMDEBUG

#অন্তর্ভাৱ <stddef.h>  /* for offsetof() */

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "curl_threads.h"
#অন্তর্ভাৱ "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#অন্তর্ভাৱ <backtrace.h>
#endif

সংরচনা memdebug {
  size_t size;
  সঙ্ঘ {
    curl_off_t o;
    দ্ৱিগুণ d;
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

FILE *curl_dbg_logfile = রিক্ত;
স্থির bool registered_cleanup = FALSE; /* atexit registered cleanup */
স্থির bool memlimit = FALSE; /* enable memory limit */
স্থির long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
স্থির সংরচনা backtrace_state *btstate;
#endif

স্থির ৱর্ণ membuf[10000];
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
    ঘূৰাই TRUE;
  }
#endif
  ঘূৰাই FALSE;
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

স্থির শূন্য curl_dbg_log_locked(ধ্রুৱ ৱর্ণ *format, ...) CURL_PRINTF(1, 2);

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
  curl_dbg_logfile = রিক্ত;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  যদি(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
স্থির শূন্য error_bt_callback(শূন্য *data, ধ্রুৱ ৱর্ণ *message,
                              পূর্ণাঙ্ক error_সংখ্যা)
{
  (শূন্য)data;
  যদি(error_সংখ্যা == -1)
    curl_dbg_log("compile with -g\n\n");
  অন্যথা
    curl_dbg_log("Backtrace error %d: %s\n", error_সংখ্যা, message);
}

স্থির পূর্ণাঙ্ক full_callback(শূন্য *data, uintptr_t pc, ধ্রুৱ ৱর্ণ *pathname,
                         পূর্ণাঙ্ক সাৰি_সংখ্যা, ধ্রুৱ ৱর্ণ *function)
{
  (শূন্য)data;
  (শূন্য)pc;
  যদি(pathname || function || সাৰি_সংখ্যা)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, সাৰি_সংখ্যা, function);
  ঘূৰাই 0;
}

স্থির শূন্য dump_bt(শূন্য)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, রিক্ত);
}
#অন্যথা
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
শূন্য curl_dbg_memdebug(ধ্রুৱ ৱর্ণ *logname)
{
  যদি(!curl_dbg_logfile) {
    যদি(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_নিচেৰ(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    যদি(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (ৱর্ণ *)রিক্ত);
#endif
  }
#ifdef USE_MUTEX
  যদি(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(রিক্ত, 0, error_bt_callback, রিক্ত);
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
স্থির bool countcheck(ধ্রুৱ ৱর্ণ *func, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  যদি(memlimit && source) {
    যদি(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, সাৰি, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, সাৰি, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      ঘূৰাই TRUE; /* RETURN ERROR! */
    }
    অন্যথা
      memsize--; /* countdown */
  }

  ঘূৰাই FALSE; /* allow this */
}

ALLOC_FUNC
শূন্য *curl_dbg_malloc(size_t wantedsize, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  সংরচনা memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  যদি(countcheck("malloc", সাৰি, source))
    ঘূৰাই রিক্ত;

  /* alloc at least 64 bytes */
  size = পরিমাণ(সংরচনা memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  যদি(mem) {
    mem->size = wantedsize;
  }

  যদি(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, সাৰি, wantedsize,
                 mem ? (শূন্য *)mem->mem : (শূন্য *)0);

  ঘূৰাই mem ? mem->mem : রিক্ত;
}

ALLOC_FUNC
শূন্য *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  সংরচনা memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  যদি(countcheck("calloc", সাৰি, source))
    ঘূৰাই রিক্ত;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = পরিমাণ(সংরচনা memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  যদি(mem)
    mem->size = user_size;

  যদি(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, সাৰি, wanted_elements, wanted_size,
                 mem ? (শূন্য *)mem->mem : (শূন্য *)0);

  ঘূৰাই mem ? mem->mem : রিক্ত;
}

ALLOC_FUNC
ৱর্ণ *curl_dbg_strdup(ধ্রুৱ ৱর্ণ *str, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  ৱর্ণ *mem;
  size_t len;

  DEBUGASSERT(str);

  যদি(countcheck("strdup", সাৰি, source))
    ঘূৰাই রিক্ত;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, রিক্ত); /* NULL prevents logging */
  যদি(mem)
    memcpy(mem, str, len);

  যদি(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, সাৰি, (ধ্রুৱ শূন্য *)str, len, (ধ্রুৱ শূন্য *)mem);

  ঘূৰাই mem;
}

#যদি defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ধ্রুৱ wchar_t *str, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  যদি(countcheck("wcsdup", সাৰি, source))
    ঘূৰাই রিক্ত;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * পরিমাণ(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, রিক্ত); /* NULL prevents logging */
  যদি(mem)
    memcpy(mem, str, bsiz);

  যদি(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, সাৰি, (ধ্রুৱ শূন্য *)str, bsiz, (শূন্য *)mem);

  ঘূৰাই mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
শূন্য *curl_dbg_realloc(শূন্য *ptr, size_t wantedsize,
                       পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  সংরচনা memdebug *mem = রিক্ত;
  bool was_locked;

  size_t size = পরিমাণ(সংরচনা memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  যদি(countcheck("realloc", সাৰি, source))
    ঘূৰাই রিক্ত;

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
    mem = (শূন্য *)((ৱর্ণ *)ptr - offsetof(সংরচনা memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(বেৰ_কৰো)
#endif

  mem = Curl_crealloc(mem, size);
  যদি(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, সাৰি, (শূন্য *)ptr, wantedsize,
                        mem ? (শূন্য *)mem->mem : (শূন্য *)0);

  curl_dbg_unlock(was_locked);
  যদি(mem) {
    mem->size = wantedsize;
    ঘূৰাই mem->mem;
  }

  ঘূৰাই রিক্ত;
}

শূন্য curl_dbg_free(শূন্য *ptr, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  যদি(ptr) {
    সংরচনা memdebug *mem;

    যদি(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, সাৰি, (শূন্য *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(ঠেলো)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (শূন্য *)((ৱর্ণ *)ptr - offsetof(সংরচনা memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(বেৰ_কৰো)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(পূর্ণাঙ্ক domain, পূর্ণাঙ্ক type, পূর্ণাঙ্ক protocol,
                              পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  curl_socket_t sockfd;

  যদি(countcheck("socket", সাৰি, source))
    ঘূৰাই CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  যদি(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, সাৰি, sockfd);

  ঘূৰাই sockfd;
}

#ifdef HAVE_SOCKETPAIR
পূর্ণাঙ্ক curl_dbg_socketpair(পূর্ণাঙ্ক domain, পূর্ণাঙ্ক type, পূর্ণাঙ্ক protocol,
                        curl_socket_t socket_vector[2],
                        পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  পূর্ণাঙ্ক res = socketpair(domain, type, protocol, socket_vector);

  যদি(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, সাৰি, socket_vector[0], socket_vector[1]);

  ঘূৰাই res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, শূন্য *saddr, শূন্য *saddrlen,
                              পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  সংরচনা sockaddr *addr = (সংরচনা sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  যদি(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, সাৰি, sockfd);

  ঘূৰাই sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, শূন্য *saddr, শূন্য *saddrlen,
                               পূর্ণাঙ্ক flags,
                               পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  সংরচনা sockaddr *addr = (সংরচনা sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  যদি(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, সাৰি, sockfd);

  ঘূৰাই sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
শূন্য curl_dbg_mark_sclose(curl_socket_t sockfd, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  যদি(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, সাৰি, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
পূর্ণাঙ্ক curl_dbg_sclose(curl_socket_t sockfd, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  curl_dbg_mark_sclose(sockfd, সাৰি, source);
  ঘূৰাই CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ধ্রুৱ ৱর্ণ *file, ধ্রুৱ ৱর্ণ *mode,
                     পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  FILE *res = CURLX_FOPEN_নিচেৰ(file, mode);
  যদি(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, সাৰি, file, mode, (শূন্য *)res);

  ঘূৰাই res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ধ্রুৱ ৱর্ণ *file, ধ্রুৱ ৱর্ণ *mode, FILE *fh,
                       পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  FILE *res = CURLX_FREOPEN_নিচেৰ(file, mode, fh);
  যদি(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, সাৰি, file, mode, (শূন্য *)fh, (শূন্য *)res);

  ঘূৰাই res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(পূর্ণাঙ্ক filedes, ধ্রুৱ ৱর্ণ *mode,
                      পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  FILE *res = CURLX_FDOPEN_নিচেৰ(filedes, mode);
  যদি(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, সাৰি, filedes, mode, (শূন্য *)res);
  ঘূৰাই res;
}

পূর্ণাঙ্ক curl_dbg_fclose(FILE *file, পূর্ণাঙ্ক সাৰি, ধ্রুৱ ৱর্ণ *source)
{
  পূর্ণাঙ্ক res;

  DEBUGASSERT(file);

  যদি(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, সাৰি, (শূন্য *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  ঘূৰাই res;
}

স্থির শূন্য curl_dbg_vlog(ধ্রুৱ ৱর্ণ * ধ্রুৱ fmt,
                          va_list ap) CURL_PRINTF(1, 0);

স্থির শূন্য curl_dbg_vlog(ধ্রুৱ ৱর্ণ * ধ্রুৱ fmt, va_list ap)
{
  ৱর্ণ buf[1024];
  size_t nchars = curl_mvsnprintf(buf, পরিমাণ(buf), fmt, ap);

  যদি(nchars > (পূর্ণাঙ্ক)পরিমাণ(buf) - 1)
    nchars = (পূর্ণাঙ্ক)পরিমাণ(buf) - 1;

  যদি(nchars > 0) {
    যদি(পরিমাণ(membuf) - nchars < memwidx) {
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

স্থির শূন্য curl_dbg_log_locked(ধ্রুৱ ৱর্ণ *format, ...)
{
  va_list ap;

  যদি(!curl_dbg_logfile)
    ঘূৰাই;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
শূন্য curl_dbg_log(ধ্রুৱ ৱর্ণ *format, ...)
{
  bool was_locked;
  va_list ap;

  যদি(!curl_dbg_logfile)
    ঘূৰাই;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
