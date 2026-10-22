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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే CURL_MEMDEBUG

#అన్తర్భావ <stddef.h>  /* for offsetof() */

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_threads.h"
#అన్తర్భావ "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#నిర్వచితమైతే USE_BACKTRACE
#అన్తర్భావ <backtrace.h>
#అయితే_ముగింపు

నిర్మాణం memdebug {
  size_t కొలత;
  సంఘం {
    curl_off_t o;
    ద్విగుణ d;
    శూన్య *p;
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

FILE *curl_dbg_logfile = ఖాళీ;
స్థిర bool registered_cleanup = FALSE; /* atexit registered cleanup */
స్థిర bool memlimit = FALSE; /* enable memory limit */
స్థిర దీర్ఘ memsize = 0;  /* set number of mallocs allowed */
#నిర్వచితమైతే USE_BACKTRACE
స్థిర నిర్మాణం backtrace_state *btstate;
#అయితే_ముగింపు

స్థిర అక్షరం membuf[10000];
స్థిర size_t memwidx = 0; /* write index */

#నిర్వచితమైతే USE_MUTEX
స్థిర bool dbg_mutex_init = 0;
స్థిర curl_mutex_t dbg_mutex;
#అయితే_ముగింపు

స్థిర bool curl_dbg_తాళం(శూన్య)
{
#నిర్వచితమైతే USE_MUTEX
  ఒకవేళ(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    తిరిగి TRUE;
  }
#అయితే_ముగింపు
  తిరిగి FALSE;
}

స్థిర శూన్య curl_dbg_unlock(bool was_locked)
{
#నిర్వచితమైతే USE_MUTEX
  ఒకవేళ(was_locked)
    Curl_mutex_release(&dbg_mutex);
#లేకపోతే
  (శూన్య)was_locked;
#అయితే_ముగింపు
}

స్థిర శూన్య curl_dbg_log_locked(స్థిరరాశి అక్షరం *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
స్థిర శూన్య curl_dbg_cleanup(శూన్య)
{
  bool locked = curl_dbg_తాళం();
  ఒకవేళ(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    ఒకవేళ(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = ఖాళీ;
  curl_dbg_unlock(locked);
#నిర్వచితమైతే USE_MUTEX
  ఒకవేళ(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#అయితే_ముగింపు
}

#నిర్వచితమైతే USE_BACKTRACE
స్థిర శూన్య error_bt_callback(శూన్య *దత్తాంశం, స్థిరరాశి అక్షరం *సందేశం_2,
                              పూర్ణాంకం error_సంఖ్య)
{
  (శూన్య)దత్తాంశం;
  ఒకవేళ(error_సంఖ్య == -1)
    curl_dbg_log("compile with -g\n\n");
  లేకపోతే
    curl_dbg_log("Backtrace error %d: %s\n", error_సంఖ్య, సందేశం_2);
}

స్థిర పూర్ణాంకం నిండిన_callback(శూన్య *దత్తాంశం, uintptr_t pc, స్థిరరాశి అక్షరం *pathname,
                         పూర్ణాంకం పంక్తి_సంఖ్య, స్థిరరాశి అక్షరం *function)
{
  (శూన్య)దత్తాంశం;
  (శూన్య)pc;
  ఒకవేళ(pathname || function || పంక్తి_సంఖ్య)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, పంక్తి_సంఖ్య, function);
  తిరిగి 0;
}

స్థిర శూన్య dump_bt(శూన్య)
{
  backtrace_నిండిన(btstate, 0, నిండిన_callback, error_bt_callback, ఖాళీ);
}
#లేకపోతే
#నిర్వచించు dump_bt() /* nothing to do */
#అయితే_ముగింపు

/* this sets the log filename */
శూన్య curl_dbg_memdebug(స్థిరరాశి అక్షరం *logname)
{
  ఒకవేళ(!curl_dbg_logfile) {
    ఒకవేళ(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_దిగువ(logname, FOPEN_WRITETEXT);
#నిర్వచితమైతే MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    ఒకవేళ(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (అక్షరం *)ఖాళీ);
#అయితే_ముగింపు
  }
#నిర్వచితమైతే USE_MUTEX
  ఒకవేళ(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#అయితే_ముగింపు
#నిర్వచితమైతే USE_BACKTRACE
  btstate = backtrace_create_state(ఖాళీ, 0, error_bt_callback, ఖాళీ);
#అయితే_ముగింపు
  ఒకవేళ(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
శూన్య curl_dbg_memlimit(దీర్ఘ హద్దు)
{
  ఒకవేళ(!memlimit) {
    memlimit = TRUE;
    memsize = హద్దు;
  }
}

/* returns TRUE if this is not allowed! */
స్థిర bool countcheck(స్థిరరాశి అక్షరం *func, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  ఒకవేళ(memlimit && ఆధారం) {
    ఒకవేళ(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", ఆధారం, పంక్తి, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    ఆధారం, పంక్తి, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      తిరిగి TRUE; /* RETURN ERROR! */
    }
    లేకపోతే
      memsize--; /* countdown */
  }

  తిరిగి FALSE; /* allow this */
}

ALLOC_FUNC
శూన్య *curl_dbg_malloc(size_t wantedsize, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  నిర్మాణం memdebug *mem;
  size_t కొలత;

  DEBUGASSERT(wantedsize != 0);

  ఒకవేళ(countcheck("malloc", పంక్తి, ఆధారం))
    తిరిగి ఖాళీ;

  /* alloc at least 64 bytes */
  కొలత = పరిమాణం(నిర్మాణం memdebug) + wantedsize;

  mem = Curl_cmalloc(కొలత);
  ఒకవేళ(mem) {
    mem->కొలత = wantedsize;
  }

  ఒకవేళ(ఆధారం)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 ఆధారం, పంక్తి, wantedsize,
                 mem ? (శూన్య *)mem->mem : (శూన్య *)0);

  తిరిగి mem ? mem->mem : ఖాళీ;
}

ALLOC_FUNC
శూన్య *curl_dbg_calloc(size_t wanted_elements, size_t wanted_కొలత,
                      పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  నిర్మాణం memdebug *mem;
  size_t కొలత, user_కొలత;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_కొలత != 0);

  ఒకవేళ(countcheck("calloc", పంక్తి, ఆధారం))
    తిరిగి ఖాళీ;

  /* alloc at least 64 bytes */
  user_కొలత = wanted_కొలత * wanted_elements;
  కొలత = పరిమాణం(నిర్మాణం memdebug) + user_కొలత;

  mem = Curl_ccalloc(1, కొలత);
  ఒకవేళ(mem)
    mem->కొలత = user_కొలత;

  ఒకవేళ(ఆధారం)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 ఆధారం, పంక్తి, wanted_elements, wanted_కొలత,
                 mem ? (శూన్య *)mem->mem : (శూన్య *)0);

  తిరిగి mem ? mem->mem : ఖాళీ;
}

ALLOC_FUNC
అక్షరం *curl_dbg_strdup(స్థిరరాశి అక్షరం *str, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  అక్షరం *mem;
  size_t len;

  DEBUGASSERT(str);

  ఒకవేళ(countcheck("strdup", పంక్తి, ఆధారం))
    తిరిగి ఖాళీ;

  len = అక్షరశ్రేణి_పొడవు(str) + 1;

  mem = curl_dbg_malloc(len, 0, ఖాళీ); /* NULL prevents logging */
  ఒకవేళ(mem)
    జ్ఞాపకం_నకలు(mem, str, len);

  ఒకవేళ(ఆధారం)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 ఆధారం, పంక్తి, (స్థిరరాశి శూన్య *)str, len, (స్థిరరాశి శూన్య *)mem);

  తిరిగి mem;
}

#ఒకవేళ defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(స్థిరరాశి wchar_t *str, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  ఒకవేళ(countcheck("wcsdup", పంక్తి, ఆధారం))
    తిరిగి ఖాళీ;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * పరిమాణం(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, ఖాళీ); /* NULL prevents logging */
  ఒకవేళ(mem)
    జ్ఞాపకం_నకలు(mem, str, bsiz);

  ఒకవేళ(ఆధారం)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                ఆధారం, పంక్తి, (స్థిరరాశి శూన్య *)str, bsiz, (శూన్య *)mem);

  తిరిగి mem;
}
#అయితే_ముగింపు

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
శూన్య *curl_dbg_realloc(శూన్య *ptr, size_t wantedsize,
                       పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  నిర్మాణం memdebug *mem = ఖాళీ;
  bool was_locked;

  size_t కొలత = పరిమాణం(నిర్మాణం memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  ఒకవేళ(countcheck("realloc", పంక్తి, ఆధారం))
    తిరిగి ఖాళీ;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_తాళం();
#నిర్వచితమైతే __INTEL_COMPILER
#  pragma warning(నెట్టు)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#అయితే_ముగింపు

  ఒకవేళ(ptr)
    mem = (శూన్య *)((అక్షరం *)ptr - offsetof(నిర్మాణం memdebug, mem));

#నిర్వచితమైతే __INTEL_COMPILER
#  pragma warning(తీయి)
#అయితే_ముగింపు

  mem = Curl_crealloc(mem, కొలత);
  ఒకవేళ(ఆధారం)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        ఆధారం, పంక్తి, (శూన్య *)ptr, wantedsize,
                        mem ? (శూన్య *)mem->mem : (శూన్య *)0);

  curl_dbg_unlock(was_locked);
  ఒకవేళ(mem) {
    mem->కొలత = wantedsize;
    తిరిగి mem->mem;
  }

  తిరిగి ఖాళీ;
}

శూన్య curl_dbg_free(శూన్య *ptr, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  ఒకవేళ(ptr) {
    నిర్మాణం memdebug *mem;

    ఒకవేళ(ఆధారం)
      curl_dbg_log("MEM %s:%d free(%p)\n", ఆధారం, పంక్తి, (శూన్య *)ptr);

#నిర్వచితమైతే __INTEL_COMPILER
#  pragma warning(నెట్టు)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#అయితే_ముగింపు

    mem = (శూన్య *)((అక్షరం *)ptr - offsetof(నిర్మాణం memdebug, mem));

#నిర్వచితమైతే __INTEL_COMPILER
#  pragma warning(తీయి)
#అయితే_ముగింపు

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(పూర్ణాంకం domain, పూర్ణాంకం type, పూర్ణాంకం protocol,
                              పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  curl_socket_t sockfd;

  ఒకవేళ(countcheck("socket", పంక్తి, ఆధారం))
    తిరిగి CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  ఒకవేళ(ఆధారం && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 ఆధారం, పంక్తి, sockfd);

  తిరిగి sockfd;
}

#నిర్వచితమైతే HAVE_SOCKETPAIR
పూర్ణాంకం curl_dbg_socketpair(పూర్ణాంకం domain, పూర్ణాంకం type, పూర్ణాంకం protocol,
                        curl_socket_t socket_సదిశ[2],
                        పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  పూర్ణాంకం res = socketpair(domain, type, protocol, socket_సదిశ);

  ఒకవేళ(ఆధారం && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 ఆధారం, పంక్తి, socket_సదిశ[0], socket_సదిశ[1]);

  తిరిగి res;
}
#అయితే_ముగింపు

curl_socket_t curl_dbg_accept(curl_socket_t s, శూన్య *saddr, శూన్య *saddrlen,
                              పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  నిర్మాణం sockaddr *addr = (నిర్మాణం sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  ఒకవేళ(ఆధారం && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ఆధారం, పంక్తి, sockfd);

  తిరిగి sockfd;
}

#నిర్వచితమైతే HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, శూన్య *saddr, శూన్య *saddrlen,
                               పూర్ణాంకం flags,
                               పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  నిర్మాణం sockaddr *addr = (నిర్మాణం sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  ఒకవేళ(ఆధారం && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 ఆధారం, పంక్తి, sockfd);

  తిరిగి sockfd;
}
#అయితే_ముగింపు

/* separate function to allow libcurl to mark a "faked" close */
శూన్య curl_dbg_mark_sclose(curl_socket_t sockfd, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  ఒకవేళ(ఆధారం)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 ఆధారం, పంక్తి, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
పూర్ణాంకం curl_dbg_sclose(curl_socket_t sockfd, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  curl_dbg_mark_sclose(sockfd, పంక్తి, ఆధారం);
  తిరిగి CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(స్థిరరాశి అక్షరం *file, స్థిరరాశి అక్షరం *బహుళకం,
                     పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  FILE *res = CURLX_FOPEN_దిగువ(file, బహుళకం);
  ఒకవేళ(ఆధారం)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 ఆధారం, పంక్తి, file, బహుళకం, (శూన్య *)res);

  తిరిగి res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(స్థిరరాశి అక్షరం *file, స్థిరరాశి అక్షరం *బహుళకం, FILE *fh,
                       పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  FILE *res = CURLX_FREOPEN_దిగువ(file, బహుళకం, fh);
  ఒకవేళ(ఆధారం)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 ఆధారం, పంక్తి, file, బహుళకం, (శూన్య *)fh, (శూన్య *)res);

  తిరిగి res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(పూర్ణాంకం filedes, స్థిరరాశి అక్షరం *బహుళకం,
                      పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  FILE *res = CURLX_FDOPEN_దిగువ(filedes, బహుళకం);
  ఒకవేళ(ఆధారం)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 ఆధారం, పంక్తి, filedes, బహుళకం, (శూన్య *)res);
  తిరిగి res;
}

పూర్ణాంకం curl_dbg_fclose(FILE *file, పూర్ణాంకం పంక్తి, స్థిరరాశి అక్షరం *ఆధారం)
{
  పూర్ణాంకం res;

  DEBUGASSERT(file);

  ఒకవేళ(ఆధారం)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", ఆధారం, పంక్తి, (శూన్య *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  తిరిగి res;
}

స్థిర శూన్య curl_dbg_vlog(స్థిరరాశి అక్షరం * స్థిరరాశి fmt,
                          va_list ap) CURL_PRINTF(1, 0);

స్థిర శూన్య curl_dbg_vlog(స్థిరరాశి అక్షరం * స్థిరరాశి fmt, va_list ap)
{
  అక్షరం buf[1024];
  size_t nchars = curl_mvsnprintf(buf, పరిమాణం(buf), fmt, ap);

  ఒకవేళ(nchars > (పూర్ణాంకం)పరిమాణం(buf) - 1)
    nchars = (పూర్ణాంకం)పరిమాణం(buf) - 1;

  ఒకవేళ(nchars > 0) {
    ఒకవేళ(పరిమాణం(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    ఒకవేళ(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    జ్ఞాపకం_నకలు(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

స్థిర శూన్య curl_dbg_log_locked(స్థిరరాశి అక్షరం *format, ...)
{
  va_list ap;

  ఒకవేళ(!curl_dbg_logfile)
    తిరిగి;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
శూన్య curl_dbg_log(స్థిరరాశి అక్షరం *format, ...)
{
  bool was_locked;
  va_list ap;

  ఒకవేళ(!curl_dbg_logfile)
    తిరిగి;

  was_locked = curl_dbg_తాళం();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#అయితే_ముగింపు /* CURL_MEMDEBUG */
