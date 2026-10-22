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
#अन्तर्भाव "curl_setup.h"

#ifdef CURL_MEMDEBUG

#अन्तर्भाव <stddef.h>  /* for offsetof() */

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#अन्तर्भाव <backtrace.h>
#endif

संरचना memdebug {
  size_t size;
  सङ्घ {
    curl_off_t o;
    द्विगुण d;
    शून्य *p;
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

FILE *curl_dbg_logfile = रिक्त;
स्थिर bool registered_cleanup = FALSE; /* atexit registered cleanup */
स्थिर bool memlimit = FALSE; /* enable memory limit */
स्थिर long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
स्थिर संरचना backtrace_state *btstate;
#endif

स्थिर वर्ण membuf[10000];
स्थिर size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
स्थिर bool dbg_mutex_init = 0;
स्थिर curl_mutex_t dbg_mutex;
#endif

स्थिर bool curl_dbg_lock(शून्य)
{
#ifdef USE_MUTEX
  यदि(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    प्रत्यागम TRUE;
  }
#endif
  प्रत्यागम FALSE;
}

स्थिर शून्य curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  यदि(was_locked)
    Curl_mutex_release(&dbg_mutex);
#अन्यथा
  (शून्य)was_locked;
#endif
}

स्थिर शून्य curl_dbg_log_locked(ध्रुव वर्ण *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
स्थिर शून्य curl_dbg_cleanup(शून्य)
{
  bool locked = curl_dbg_lock();
  यदि(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    यदि(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = रिक्त;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  यदि(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
स्थिर शून्य error_bt_callback(शून्य *data, ध्रुव वर्ण *message,
                              पूर्णाङ्क error_संख्या)
{
  (शून्य)data;
  यदि(error_संख्या == -1)
    curl_dbg_log("compile with -g\n\n");
  अन्यथा
    curl_dbg_log("Backtrace error %d: %s\n", error_संख्या, message);
}

स्थिर पूर्णाङ्क full_callback(शून्य *data, uintptr_t pc, ध्रुव वर्ण *pathname,
                         पूर्णाङ्क पंक्ति_संख्या, ध्रुव वर्ण *function)
{
  (शून्य)data;
  (शून्य)pc;
  यदि(pathname || function || पंक्ति_संख्या)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, पंक्ति_संख्या, function);
  प्रत्यागम 0;
}

स्थिर शून्य dump_bt(शून्य)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, रिक्त);
}
#अन्यथा
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
शून्य curl_dbg_memdebug(ध्रुव वर्ण *logname)
{
  यदि(!curl_dbg_logfile) {
    यदि(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_निचला(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    यदि(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (वर्ण *)रिक्त);
#endif
  }
#ifdef USE_MUTEX
  यदि(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(रिक्त, 0, error_bt_callback, रिक्त);
#endif
  यदि(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
शून्य curl_dbg_memlimit(long limit)
{
  यदि(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
स्थिर bool countcheck(ध्रुव वर्ण *func, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  यदि(memlimit && source) {
    यदि(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, पंक्ति, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, पंक्ति, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      प्रत्यागम TRUE; /* RETURN ERROR! */
    }
    अन्यथा
      memsize--; /* countdown */
  }

  प्रत्यागम FALSE; /* allow this */
}

ALLOC_FUNC
शून्य *curl_dbg_malloc(size_t wantedsize, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  संरचना memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  यदि(countcheck("malloc", पंक्ति, source))
    प्रत्यागम रिक्त;

  /* alloc at least 64 bytes */
  size = परिमाण(संरचना memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  यदि(mem) {
    mem->size = wantedsize;
  }

  यदि(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, पंक्ति, wantedsize,
                 mem ? (शून्य *)mem->mem : (शून्य *)0);

  प्रत्यागम mem ? mem->mem : रिक्त;
}

ALLOC_FUNC
शून्य *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  संरचना memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  यदि(countcheck("calloc", पंक्ति, source))
    प्रत्यागम रिक्त;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = परिमाण(संरचना memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  यदि(mem)
    mem->size = user_size;

  यदि(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, पंक्ति, wanted_elements, wanted_size,
                 mem ? (शून्य *)mem->mem : (शून्य *)0);

  प्रत्यागम mem ? mem->mem : रिक्त;
}

ALLOC_FUNC
वर्ण *curl_dbg_strdup(ध्रुव वर्ण *str, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  वर्ण *mem;
  size_t len;

  DEBUGASSERT(str);

  यदि(countcheck("strdup", पंक्ति, source))
    प्रत्यागम रिक्त;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, रिक्त); /* NULL prevents logging */
  यदि(mem)
    memcpy(mem, str, len);

  यदि(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, पंक्ति, (ध्रुव शून्य *)str, len, (ध्रुव शून्य *)mem);

  प्रत्यागम mem;
}

#यदि defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(ध्रुव wchar_t *str, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  यदि(countcheck("wcsdup", पंक्ति, source))
    प्रत्यागम रिक्त;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * परिमाण(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, रिक्त); /* NULL prevents logging */
  यदि(mem)
    memcpy(mem, str, bsiz);

  यदि(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, पंक्ति, (ध्रुव शून्य *)str, bsiz, (शून्य *)mem);

  प्रत्यागम mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
शून्य *curl_dbg_realloc(शून्य *ptr, size_t wantedsize,
                       पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  संरचना memdebug *mem = रिक्त;
  bool was_locked;

  size_t size = परिमाण(संरचना memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  यदि(countcheck("realloc", पंक्ति, source))
    प्रत्यागम रिक्त;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(धकेलो)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  यदि(ptr)
    mem = (शून्य *)((वर्ण *)ptr - offsetof(संरचना memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(निकालो)
#endif

  mem = Curl_crealloc(mem, size);
  यदि(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, पंक्ति, (शून्य *)ptr, wantedsize,
                        mem ? (शून्य *)mem->mem : (शून्य *)0);

  curl_dbg_unlock(was_locked);
  यदि(mem) {
    mem->size = wantedsize;
    प्रत्यागम mem->mem;
  }

  प्रत्यागम रिक्त;
}

शून्य curl_dbg_free(शून्य *ptr, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  यदि(ptr) {
    संरचना memdebug *mem;

    यदि(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, पंक्ति, (शून्य *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(धकेलो)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (शून्य *)((वर्ण *)ptr - offsetof(संरचना memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(निकालो)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(पूर्णाङ्क domain, पूर्णाङ्क type, पूर्णाङ्क protocol,
                              पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  curl_socket_t sockfd;

  यदि(countcheck("socket", पंक्ति, source))
    प्रत्यागम CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  यदि(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, पंक्ति, sockfd);

  प्रत्यागम sockfd;
}

#ifdef HAVE_SOCKETPAIR
पूर्णाङ्क curl_dbg_socketpair(पूर्णाङ्क domain, पूर्णाङ्क type, पूर्णाङ्क protocol,
                        curl_socket_t socket_vector[2],
                        पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  पूर्णाङ्क res = socketpair(domain, type, protocol, socket_vector);

  यदि(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, पंक्ति, socket_vector[0], socket_vector[1]);

  प्रत्यागम res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, शून्य *saddr, शून्य *saddrlen,
                              पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  संरचना sockaddr *addr = (संरचना sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  यदि(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, पंक्ति, sockfd);

  प्रत्यागम sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, शून्य *saddr, शून्य *saddrlen,
                               पूर्णाङ्क flags,
                               पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  संरचना sockaddr *addr = (संरचना sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  यदि(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, पंक्ति, sockfd);

  प्रत्यागम sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
शून्य curl_dbg_mark_sclose(curl_socket_t sockfd, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  यदि(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, पंक्ति, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
पूर्णाङ्क curl_dbg_sclose(curl_socket_t sockfd, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  curl_dbg_mark_sclose(sockfd, पंक्ति, source);
  प्रत्यागम CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(ध्रुव वर्ण *file, ध्रुव वर्ण *mode,
                     पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  FILE *res = CURLX_FOPEN_निचला(file, mode);
  यदि(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, पंक्ति, file, mode, (शून्य *)res);

  प्रत्यागम res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(ध्रुव वर्ण *file, ध्रुव वर्ण *mode, FILE *fh,
                       पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  FILE *res = CURLX_FREOPEN_निचला(file, mode, fh);
  यदि(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, पंक्ति, file, mode, (शून्य *)fh, (शून्य *)res);

  प्रत्यागम res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(पूर्णाङ्क filedes, ध्रुव वर्ण *mode,
                      पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  FILE *res = CURLX_FDOPEN_निचला(filedes, mode);
  यदि(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, पंक्ति, filedes, mode, (शून्य *)res);
  प्रत्यागम res;
}

पूर्णाङ्क curl_dbg_fclose(FILE *file, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *source)
{
  पूर्णाङ्क res;

  DEBUGASSERT(file);

  यदि(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, पंक्ति, (शून्य *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  प्रत्यागम res;
}

स्थिर शून्य curl_dbg_vlog(ध्रुव वर्ण * ध्रुव fmt,
                          va_list ap) CURL_PRINTF(1, 0);

स्थिर शून्य curl_dbg_vlog(ध्रुव वर्ण * ध्रुव fmt, va_list ap)
{
  वर्ण buf[1024];
  size_t nchars = curl_mvsnprintf(buf, परिमाण(buf), fmt, ap);

  यदि(nchars > (पूर्णाङ्क)परिमाण(buf) - 1)
    nchars = (पूर्णाङ्क)परिमाण(buf) - 1;

  यदि(nchars > 0) {
    यदि(परिमाण(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    यदि(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

स्थिर शून्य curl_dbg_log_locked(ध्रुव वर्ण *format, ...)
{
  va_list ap;

  यदि(!curl_dbg_logfile)
    प्रत्यागम;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
शून्य curl_dbg_log(ध्रुव वर्ण *format, ...)
{
  bool was_locked;
  va_list ap;

  यदि(!curl_dbg_logfile)
    प्रत्यागम;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
