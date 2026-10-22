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
#அந்தர்பாவ "curl_setup.h"

#ifdef CURL_MEMDEBUG

#அந்தர்பாவ <stddef.h>  /* for offsetof() */

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_threads.h"
#அந்தர்பாவ "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#ifdef USE_BACKTRACE
#அந்தர்பாவ <backtrace.h>
#endif

கட்டமைப்பு memdebug {
  size_t size;
  ஒன்றிப்பு {
    curl_off_t o;
    இரட்டை d;
    வெறுமை *p;
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

FILE *curl_dbg_logfile = வெற்று;
நிலையான bool registered_cleanup = FALSE; /* atexit registered cleanup */
நிலையான bool memlimit = FALSE; /* enable memory limit */
நிலையான long memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
நிலையான கட்டமைப்பு backtrace_state *btstate;
#endif

நிலையான எழுத்து membuf[10000];
நிலையான size_t memwidx = 0; /* write index */

#ifdef USE_MUTEX
நிலையான bool dbg_mutex_init = 0;
நிலையான curl_mutex_t dbg_mutex;
#endif

நிலையான bool curl_dbg_lock(வெறுமை)
{
#ifdef USE_MUTEX
  எனில்(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    திரும்பு TRUE;
  }
#endif
  திரும்பு FALSE;
}

நிலையான வெறுமை curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  எனில்(was_locked)
    Curl_mutex_release(&dbg_mutex);
#இல்லையெனில்
  (வெறுமை)was_locked;
#endif
}

நிலையான வெறுமை curl_dbg_log_locked(மாறிலி எழுத்து *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
நிலையான வெறுமை curl_dbg_cleanup(வெறுமை)
{
  bool locked = curl_dbg_lock();
  எனில்(curl_dbg_logfile &&
     curl_dbg_logfile != stderr &&
     curl_dbg_logfile != stdout) {
    எனில்(memwidx)
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    fclose(curl_dbg_logfile);
  }
  curl_dbg_logfile = வெற்று;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  எனில்(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
நிலையான வெறுமை error_bt_callback(வெறுமை *data, மாறிலி எழுத்து *message,
                              முழு error_எண்)
{
  (வெறுமை)data;
  எனில்(error_எண் == -1)
    curl_dbg_log("compile with -g\n\n");
  இல்லையெனில்
    curl_dbg_log("Backtrace error %d: %s\n", error_எண், message);
}

நிலையான முழு full_callback(வெறுமை *data, uintptr_t pc, மாறிலி எழுத்து *pathname,
                         முழு வரி_எண், மாறிலி எழுத்து *function)
{
  (வெறுமை)data;
  (வெறுமை)pc;
  எனில்(pathname || function || வரி_எண்)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, வரி_எண், function);
  திரும்பு 0;
}

நிலையான வெறுமை dump_bt(வெறுமை)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, வெற்று);
}
#இல்லையெனில்
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
வெறுமை curl_dbg_memdebug(மாறிலி எழுத்து *logname)
{
  எனில்(!curl_dbg_logfile) {
    எனில்(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_கீழ்(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    எனில்(curl_dbg_logfile)
      setbuf(curl_dbg_logfile, (எழுத்து *)வெற்று);
#endif
  }
#ifdef USE_MUTEX
  எனில்(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(வெற்று, 0, error_bt_callback, வெற்று);
#endif
  எனில்(!registered_cleanup)
    registered_cleanup = !atexit(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
வெறுமை curl_dbg_memlimit(long limit)
{
  எனில்(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
நிலையான bool countcheck(மாறிலி எழுத்து *func, முழு வரி, மாறிலி எழுத்து *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  எனில்(memlimit && source) {
    எனில்(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, வரி, func);
      /* log to stderr also */
      curl_mfprintf(stderr, "LIMIT %s:%d %s reached memlimit\n",
                    source, வரி, func);
      dump_bt();
      fflush(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      errno = ENOMEM;
      திரும்பு TRUE; /* RETURN ERROR! */
    }
    இல்லையெனில்
      memsize--; /* countdown */
  }

  திரும்பு FALSE; /* allow this */
}

ALLOC_FUNC
வெறுமை *curl_dbg_malloc(size_t wantedsize, முழு வரி, மாறிலி எழுத்து *source)
{
  கட்டமைப்பு memdebug *mem;
  size_t size;

  DEBUGASSERT(wantedsize != 0);

  எனில்(countcheck("malloc", வரி, source))
    திரும்பு வெற்று;

  /* alloc at least 64 bytes */
  size = அளவு(கட்டமைப்பு memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  எனில்(mem) {
    mem->size = wantedsize;
  }

  எனில்(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, வரி, wantedsize,
                 mem ? (வெறுமை *)mem->mem : (வெறுமை *)0);

  திரும்பு mem ? mem->mem : வெற்று;
}

ALLOC_FUNC
வெறுமை *curl_dbg_calloc(size_t wanted_elements, size_t wanted_size,
                      முழு வரி, மாறிலி எழுத்து *source)
{
  கட்டமைப்பு memdebug *mem;
  size_t size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  எனில்(countcheck("calloc", வரி, source))
    திரும்பு வெற்று;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = அளவு(கட்டமைப்பு memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  எனில்(mem)
    mem->size = user_size;

  எனில்(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, வரி, wanted_elements, wanted_size,
                 mem ? (வெறுமை *)mem->mem : (வெறுமை *)0);

  திரும்பு mem ? mem->mem : வெற்று;
}

ALLOC_FUNC
எழுத்து *curl_dbg_strdup(மாறிலி எழுத்து *str, முழு வரி, மாறிலி எழுத்து *source)
{
  எழுத்து *mem;
  size_t len;

  DEBUGASSERT(str);

  எனில்(countcheck("strdup", வரி, source))
    திரும்பு வெற்று;

  len = strlen(str) + 1;

  mem = curl_dbg_malloc(len, 0, வெற்று); /* NULL prevents logging */
  எனில்(mem)
    memcpy(mem, str, len);

  எனில்(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, வரி, (மாறிலி வெறுமை *)str, len, (மாறிலி வெறுமை *)mem);

  திரும்பு mem;
}

#எனில் defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
wchar_t *curl_dbg_wcsdup(மாறிலி wchar_t *str, முழு வரி, மாறிலி எழுத்து *source)
{
  wchar_t *mem;
  size_t wsiz, bsiz;

  DEBUGASSERT(str);

  எனில்(countcheck("wcsdup", வரி, source))
    திரும்பு வெற்று;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * அளவு(wchar_t);

  mem = curl_dbg_malloc(bsiz, 0, வெற்று); /* NULL prevents logging */
  எனில்(mem)
    memcpy(mem, str, bsiz);

  எனில்(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, வரி, (மாறிலி வெறுமை *)str, bsiz, (வெறுமை *)mem);

  திரும்பு mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
வெறுமை *curl_dbg_realloc(வெறுமை *ptr, size_t wantedsize,
                       முழு வரி, மாறிலி எழுத்து *source)
{
  கட்டமைப்பு memdebug *mem = வெற்று;
  bool was_locked;

  size_t size = அளவு(கட்டமைப்பு memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  எனில்(countcheck("realloc", வரி, source))
    திரும்பு வெற்று;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(தள்ளு)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  எனில்(ptr)
    mem = (வெறுமை *)((எழுத்து *)ptr - offsetof(கட்டமைப்பு memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(எடு)
#endif

  mem = Curl_crealloc(mem, size);
  எனில்(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, வரி, (வெறுமை *)ptr, wantedsize,
                        mem ? (வெறுமை *)mem->mem : (வெறுமை *)0);

  curl_dbg_unlock(was_locked);
  எனில்(mem) {
    mem->size = wantedsize;
    திரும்பு mem->mem;
  }

  திரும்பு வெற்று;
}

வெறுமை curl_dbg_free(வெறுமை *ptr, முழு வரி, மாறிலி எழுத்து *source)
{
  எனில்(ptr) {
    கட்டமைப்பு memdebug *mem;

    எனில்(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, வரி, (வெறுமை *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(தள்ளு)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (வெறுமை *)((எழுத்து *)ptr - offsetof(கட்டமைப்பு memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(எடு)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(முழு domain, முழு type, முழு protocol,
                              முழு வரி, மாறிலி எழுத்து *source)
{
  curl_socket_t sockfd;

  எனில்(countcheck("socket", வரி, source))
    திரும்பு CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  எனில்(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, வரி, sockfd);

  திரும்பு sockfd;
}

#ifdef HAVE_SOCKETPAIR
முழு curl_dbg_socketpair(முழு domain, முழு type, முழு protocol,
                        curl_socket_t socket_vector[2],
                        முழு வரி, மாறிலி எழுத்து *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  முழு res = socketpair(domain, type, protocol, socket_vector);

  எனில்(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, வரி, socket_vector[0], socket_vector[1]);

  திரும்பு res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, வெறுமை *saddr, வெறுமை *saddrlen,
                              முழு வரி, மாறிலி எழுத்து *source)
{
  கட்டமைப்பு sockaddr *addr = (கட்டமைப்பு sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  எனில்(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, வரி, sockfd);

  திரும்பு sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, வெறுமை *saddr, வெறுமை *saddrlen,
                               முழு flags,
                               முழு வரி, மாறிலி எழுத்து *source)
{
  கட்டமைப்பு sockaddr *addr = (கட்டமைப்பு sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  எனில்(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, வரி, sockfd);

  திரும்பு sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
வெறுமை curl_dbg_mark_sclose(curl_socket_t sockfd, முழு வரி, மாறிலி எழுத்து *source)
{
  எனில்(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, வரி, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
முழு curl_dbg_sclose(curl_socket_t sockfd, முழு வரி, மாறிலி எழுத்து *source)
{
  curl_dbg_mark_sclose(sockfd, வரி, source);
  திரும்பு CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
FILE *curl_dbg_fopen(மாறிலி எழுத்து *file, மாறிலி எழுத்து *mode,
                     முழு வரி, மாறிலி எழுத்து *source)
{
  FILE *res = CURLX_FOPEN_கீழ்(file, mode);
  எனில்(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, வரி, file, mode, (வெறுமை *)res);

  திரும்பு res;
}

ALLOC_FUNC
FILE *curl_dbg_freopen(மாறிலி எழுத்து *file, மாறிலி எழுத்து *mode, FILE *fh,
                       முழு வரி, மாறிலி எழுத்து *source)
{
  FILE *res = CURLX_FREOPEN_கீழ்(file, mode, fh);
  எனில்(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, வரி, file, mode, (வெறுமை *)fh, (வெறுமை *)res);

  திரும்பு res;
}

ALLOC_FUNC
FILE *curl_dbg_fdopen(முழு filedes, மாறிலி எழுத்து *mode,
                      முழு வரி, மாறிலி எழுத்து *source)
{
  FILE *res = CURLX_FDOPEN_கீழ்(filedes, mode);
  எனில்(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, வரி, filedes, mode, (வெறுமை *)res);
  திரும்பு res;
}

முழு curl_dbg_fclose(FILE *file, முழு வரி, மாறிலி எழுத்து *source)
{
  முழு res;

  DEBUGASSERT(file);

  எனில்(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, வரி, (வெறுமை *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = fclose(file);

  திரும்பு res;
}

நிலையான வெறுமை curl_dbg_vlog(மாறிலி எழுத்து * மாறிலி fmt,
                          va_list ap) CURL_PRINTF(1, 0);

நிலையான வெறுமை curl_dbg_vlog(மாறிலி எழுத்து * மாறிலி fmt, va_list ap)
{
  எழுத்து buf[1024];
  size_t nchars = curl_mvsnprintf(buf, அளவு(buf), fmt, ap);

  எனில்(nchars > (முழு)அளவு(buf) - 1)
    nchars = (முழு)அளவு(buf) - 1;

  எனில்(nchars > 0) {
    எனில்(அளவு(membuf) - nchars < memwidx) {
      /* flush */
      fwrite(membuf, 1, memwidx, curl_dbg_logfile);
      fflush(curl_dbg_logfile);
      memwidx = 0;
    }
    எனில்(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    memcpy(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

நிலையான வெறுமை curl_dbg_log_locked(மாறிலி எழுத்து *format, ...)
{
  va_list ap;

  எனில்(!curl_dbg_logfile)
    திரும்பு;

  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
}

/* this does the writing to the memory tracking log file */
வெறுமை curl_dbg_log(மாறிலி எழுத்து *format, ...)
{
  bool was_locked;
  va_list ap;

  எனில்(!curl_dbg_logfile)
    திரும்பு;

  was_locked = curl_dbg_lock();
  va_start(ap, format);
  curl_dbg_vlog(format, ap);
  va_end(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
