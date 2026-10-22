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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef CURL_MEMDEBUG

#ଅନ୍ତର୍ଭାଵ <stddef.h>  /* for offsetof() */

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_threads.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#ଅନ୍ତର୍ଭାଵ <backtrace.h>
#endif

ସଂରଚନା memdebug {
  size_t size;
  ସଂଘ {
    curl_off_t o;
    ଦ୍ୱିଗୁଣ d;
    ଶୂନ୍ୟ *p;
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

FILE *curl_dbg_logfile = ଖାଲି;
ସ୍ଥିର bool registered_cleanup = FALSE; /* atexit registered cleanup */
ସ୍ଥିର bool memlimit = FALSE; /* enable memory limit */
ସ୍ଥିର long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
ସ୍ଥିର ସଂରଚନା backtrace_state *btstate;
#endif

ସ୍ଥିର ଅକ୍ଷର membuf[10000];
ସ୍ଥିର size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
ସ୍ଥିର bool dbg_mutex_init = 0;
ସ୍ଥିର curl_mutex_t dbg_mutex;
#endif

ସ୍ଥିର bool curl_dbg_lock(ଶୂନ୍ୟ)
{
#ifdef USE_MUTEX
  ଯଦି(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    ଫେରନ୍ତୁ TRUE;
  }
#endif
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  ଯଦି(was_locked)
    Curl_mutex_release(&dbg_mutex);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)was_locked;
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ curl_dbg_log_locked(ଅଚଳ ଅକ୍ଷର *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
ସ୍ଥିର ଶୂନ୍ୟ curl_dbg_cleanup(ଶୂନ୍ୟ)
{
  bool locked = curl_dbg_lock();
  ଯଦି(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    ଯଦି(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = ଖାଲି;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  ଯଦି(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
ସ୍ଥିର ଶୂନ୍ୟ error_bt_callback(ଶୂନ୍ୟ *data, ଅଚଳ ଅକ୍ଷର *message,
                              ପୂର୍ଣ୍ଣସଂଖ୍ୟା error_ସଂଖ୍ୟା)
{
  (ଶୂନ୍ୟ)data;
  ଯଦି(error_ସଂଖ୍ୟା == -1)
    curl_dbg_log("compile with -g\n\n");
  ଅନ୍ୟଥା
    curl_dbg_log("Backtrace error %d: %s\n", error_ସଂଖ୍ୟା, message);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା full_callback(ଶୂନ୍ୟ *data, uintptr_t pc, ଅଚଳ ଅକ୍ଷର *pathname,
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି_ସଂଖ୍ୟା, ଅଚଳ ଅକ୍ଷର *function)
{
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)pc;
  ଯଦି(pathname || function || ଧାଡ଼ି_ସଂଖ୍ୟା)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, ଧାଡ଼ି_ସଂଖ୍ୟା, function);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ dump_bt(ଶୂନ୍ୟ)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, ଖାଲି);
}
#ଅନ୍ୟଥା
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
ଶୂନ୍ୟ curl_dbg_memdebug(ଅଚଳ ଅକ୍ଷର *logname)
{
  ଯଦି(!curl_dbg_logfile) {
    ଯଦି(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_ତଳ(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    ଯଦି(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (ଅକ୍ଷର *)ଖାଲି);
#endif
  }
#ifdef USE_MUTEX
  ଯଦି(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(ଖାଲି, 0, error_bt_callback, ଖାଲି);
#endif
  ଯଦି(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
ଶୂନ୍ୟ curl_dbg_memlimit(long limit)
{
  ଯଦି(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
ସ୍ଥିର bool countcheck(ଅଚଳ ଅକ୍ଷର *func, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  ଯଦି(memlimit && source) {
    ଯଦି(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, ଧାଡ଼ି, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, ଧାଡ଼ି, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      ଫେରନ୍ତୁ TRUE; /* RETURN ERROR! */
    }
    ଅନ୍ୟଥା
      memsize--; /* countdown */
  }

  ଫେରନ୍ତୁ FALSE; /* allow this */
}

ALLOC_FUNC
ଶୂନ୍ୟ *curl_dbg_malloc(size_t wantedsize, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  ସଂରଚନା memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  ଯଦି(countcheck("malloc", ଧାଡ଼ି, source))
    ଫେରନ୍ତୁ ଖାଲି;

  /* alloc at least 64 bytes */
  size = ଆକାର(ସଂରଚନା memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  ଯଦି(mem) {
    mem->size = wantedsize;
  }

  ଯଦି(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, ଧାଡ଼ି, wantedsize,
                 mem ? (ଶୂନ୍ୟ *)mem->mem : (ଶୂନ୍ୟ *)0);

  ଫେରନ୍ତୁ mem ? mem->mem : ଖାଲି;
}

ALLOC_FUNC
ଶୂନ୍ୟ *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  ସଂରଚନା memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  ଯଦି(countcheck("calloc", ଧାଡ଼ି, source))
    ଫେରନ୍ତୁ ଖାଲି;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = ଆକାର(ସଂରଚନା memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  ଯଦି(mem)
    mem->size = user_size;

  ଯଦି(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, ଧାଡ଼ି, wanted_elements, wanted_size,
                 mem ? (ଶୂନ୍ୟ *)mem->mem : (ଶୂନ୍ୟ *)0);

  ଫେରନ୍ତୁ mem ? mem->mem : ଖାଲି;
}

ALLOC_FUNC
ଅକ୍ଷର *curl_dbg_strdup(ଅଚଳ ଅକ୍ଷର *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  ଅକ୍ଷର *mem;
  size_t len;

  DEBUGASSERT(str);

  ଯଦି(countcheck("strdup", ଧାଡ଼ି, source))
    ଫେରନ୍ତୁ ଖାଲି;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, ଖାଲି); /* NULL prevents logging */
  ଯଦି(mem)
    memcpy(mem, str, len);

  ଯଦି(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, ଧାଡ଼ି, (ଅଚଳ ଶୂନ୍ୟ *)str, len, (ଅଚଳ ଶୂନ୍ୟ *)mem);

  ଫେରନ୍ତୁ mem;
}

#ଯଦି defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ଅଚଳ wchar_t *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  ଯଦି(countcheck("wcsdup", ଧାଡ଼ି, source))
    ଫେରନ୍ତୁ ଖାଲି;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * ଆକାର(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, ଖାଲି); /* NULL prevents logging */
  ଯଦି(mem)
    memcpy(mem, str, bsiz);

  ଯଦି(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, ଧାଡ଼ି, (ଅଚଳ ଶୂନ୍ୟ *)str, bsiz, (ଶୂନ୍ୟ *)mem);

  ଫେରନ୍ତୁ mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
ଶୂନ୍ୟ *curl_dbg_realloc(ଶୂନ୍ୟ *ptr, size_t wantedsize,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  ସଂରଚନା memdebug *mem = ଖାଲି;
  bool was_locked;

  size_t size = ଆକାର(ସଂରଚନା memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  ଯଦି(countcheck("realloc", ଧାଡ଼ି, source))
    ଫେରନ୍ତୁ ଖାଲି;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(ଠେଲ)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  ଯଦି(ptr)
    mem = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ptr - offsetof(ସଂରଚନା memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(କାଢ଼)
#endif

  mem = Curl_crealloc(mem, size);
  ଯଦି(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, ଧାଡ଼ି, (ଶୂନ୍ୟ *)ptr, wantedsize,
                        mem ? (ଶୂନ୍ୟ *)mem->mem : (ଶୂନ୍ୟ *)0);

  curl_dbg_unlock(was_locked);
  ଯଦି(mem) {
    mem->size = wantedsize;
    ଫେରନ୍ତୁ mem->mem;
  }

  ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ curl_dbg_free(ଶୂନ୍ୟ *ptr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  ଯଦି(ptr) {
    ସଂରଚନା memdebug *mem;

    ଯଦି(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, ଧାଡ଼ି, (ଶୂନ୍ୟ *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(ଠେଲ)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (ଶୂନ୍ୟ *)((ଅକ୍ଷର *)ptr - offsetof(ସଂରଚନା memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(କାଢ଼)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା protocol,
                              ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  curl_socket_t sockfd;

  ଯଦି(countcheck("socket", ଧାଡ଼ି, source))
    ଫେରନ୍ତୁ CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  ଯଦି(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, ଧାଡ଼ି, sockfd);

  ଫେରନ୍ତୁ sockfd;
}

#ifdef HAVE_SOCKETPAIR
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_dbg_socketpair(ପୂର୍ଣ୍ଣସଂଖ୍ୟା domain, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା protocol,
                        curl_socket_t socket_vector[2],
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = socketpair(domain, type, protocol, socket_vector);

  ଯଦି(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, ଧାଡ଼ି, socket_vector[0], socket_vector[1]);

  ଫେରନ୍ତୁ res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, ଶୂନ୍ୟ *saddr, ଶୂନ୍ୟ *saddrlen,
                              ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  ସଂରଚନା sockaddr *addr = (ସଂରଚନା sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  ଯଦି(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, ଧାଡ଼ି, sockfd);

  ଫେରନ୍ତୁ sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, ଶୂନ୍ୟ *saddr, ଶୂନ୍ୟ *saddrlen,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  ସଂରଚନା sockaddr *addr = (ସଂରଚନା sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  ଯଦି(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, ଧାଡ଼ି, sockfd);

  ଫେରନ୍ତୁ sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
ଶୂନ୍ୟ curl_dbg_mark_sclose(curl_socket_t sockfd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  ଯଦି(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, ଧାଡ଼ି, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_dbg_sclose(curl_socket_t sockfd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  curl_dbg_mark_sclose(sockfd, ଧାଡ଼ି, source);
  ଫେରନ୍ତୁ CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ଅଚଳ ଅକ୍ଷର *file, ଅଚଳ ଅକ୍ଷର *mode,
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  FILE *res = CURLX_FOPEN_ତଳ(file, mode);
  ଯଦି(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, ଧାଡ଼ି, file, mode, (ଶୂନ୍ୟ *)res);

  ଫେରନ୍ତୁ res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ଅଚଳ ଅକ୍ଷର *file, ଅଚଳ ଅକ୍ଷର *mode, FILE *fh,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  FILE *res = CURLX_FREOPEN_ତଳ(file, mode, fh);
  ଯଦି(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, ଧାଡ଼ି, file, mode, (ଶୂନ୍ୟ *)fh, (ଶୂନ୍ୟ *)res);

  ଫେରନ୍ତୁ res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା filedes, ଅଚଳ ଅକ୍ଷର *mode,
                      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  FILE *res = CURLX_FDOPEN_ତଳ(filedes, mode);
  ଯଦି(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, ଧାଡ଼ି, filedes, mode, (ଶୂନ୍ୟ *)res);
  ଫେରନ୍ତୁ res;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_dbg_fclose(FILE *file, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ଅଚଳ ଅକ୍ଷର *source)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;

  DEBUGASSERT(file);

  ଯଦି(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, ଧାଡ଼ି, (ଶୂନ୍ୟ *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  ଫେରନ୍ତୁ res;
}

ସ୍ଥିର ଶୂନ୍ୟ curl_dbg_vlog(ଅଚଳ ଅକ୍ଷର * ଅଚଳ fmt,
                          va_list ap) CURL_PRINTF(1, 0);

ସ୍ଥିର ଶୂନ୍ୟ curl_dbg_vlog(ଅଚଳ ଅକ୍ଷର * ଅଚଳ fmt, va_list ap)
{
  ଅକ୍ଷର buf[1024];
  size_t nchars = curl_mvsnprintf(buf, ଆକାର(buf), fmt, ap);

  ଯଦି(nchars > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(buf) - 1)
    nchars = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(buf) - 1;

  ଯଦି(nchars > 0) {
    ଯଦି(ଆକାର(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    ଯଦି(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ curl_dbg_log_locked(ଅଚଳ ଅକ୍ଷର *format, ...)
{
  va_list ap;

  ଯଦି(!curl_dbg_logfile)
    ଫେରନ୍ତୁ;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
ଶୂନ୍ୟ curl_dbg_log(ଅଚଳ ଅକ୍ଷର *format, ...)
{
  bool was_locked;
  va_list ap;

  ଯଦି(!curl_dbg_logfile)
    ଫେରନ୍ତୁ;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
