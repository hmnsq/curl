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
  माप_प्रकार size;
  संघ {
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

खाता *curl_dbg_logfile = रिक्त;
स्थिर bool registered_cleanup = FALSE; /* atexit registered cleanup */
स्थिर bool memlimit = FALSE; /* enable memory limit */
स्थिर दीर्घ memsize = 0;  /* set number of mallocs allowed */
#ifdef USE_BACKTRACE
स्थिर संरचना backtrace_state *btstate;
#endif

स्थिर अक्षर membuf[10000];
स्थिर माप_प्रकार memwidx = 0; /* write index */

#ifdef USE_MUTEX
स्थिर bool dbg_mutex_init = 0;
स्थिर curl_mutex_t dbg_mutex;
#endif

स्थिर bool curl_dbg_lock(शून्य)
{
#ifdef USE_MUTEX
  जर(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    परत TRUE;
  }
#endif
  परत FALSE;
}

स्थिर शून्य curl_dbg_unlock(bool was_locked)
{
#ifdef USE_MUTEX
  जर(was_locked)
    Curl_mutex_release(&dbg_mutex);
#अन्यथा
  (शून्य)was_locked;
#endif
}

स्थिर शून्य curl_dbg_log_locked(स्थिरांक अक्षर *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
स्थिर शून्य curl_dbg_cleanup(शून्य)
{
  bool locked = curl_dbg_lock();
  जर(curl_dbg_logfile &&
     curl_dbg_logfile != मानक_त्रुटि &&
     curl_dbg_logfile != मानक_निकास) {
    जर(memwidx)
      ख_डालो(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    ख_बंद(curl_dbg_logfile);
  }
  curl_dbg_logfile = रिक्त;
  curl_dbg_unlock(locked);
#ifdef USE_MUTEX
  जर(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#endif
}

#ifdef USE_BACKTRACE
स्थिर शून्य error_bt_callback(शून्य *data, स्थिरांक अक्षर *message,
                              पूर्णांक error_संख्या)
{
  (शून्य)data;
  जर(error_संख्या == -1)
    curl_dbg_log("compile with -g\n\n");
  अन्यथा
    curl_dbg_log("Backtrace error %d: %s\n", error_संख्या, message);
}

स्थिर पूर्णांक full_callback(शून्य *data, uintptr_t pc, स्थिरांक अक्षर *pathname,
                         पूर्णांक ओळ_संख्या, स्थिरांक अक्षर *function)
{
  (शून्य)data;
  (शून्य)pc;
  जर(pathname || function || ओळ_संख्या)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, ओळ_संख्या, function);
  परत 0;
}

स्थिर शून्य dump_bt(शून्य)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, रिक्त);
}
#अन्यथा
#define dump_bt() /* nothing to do */
#endif

/* this sets the log filename */
शून्य curl_dbg_memdebug(स्थिरांक अक्षर *logname)
{
  जर(!curl_dbg_logfile) {
    जर(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_खालचा(logname, FOPEN_WRITETEXT);
#ifdef MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    जर(curl_dbg_logfile)
      रखो_बफ(curl_dbg_logfile, (अक्षर *)रिक्त);
#endif
  }
#ifdef USE_MUTEX
  जर(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#endif
#ifdef USE_BACKTRACE
  btstate = backtrace_create_state(रिक्त, 0, error_bt_callback, रिक्त);
#endif
  जर(!registered_cleanup)
    registered_cleanup = !निकास_पर(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
शून्य curl_dbg_memlimit(दीर्घ limit)
{
  जर(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
स्थिर bool countcheck(स्थिरांक अक्षर *func, पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  जर(memlimit && source) {
    जर(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, ओळ, func);
      /* log to stderr also */
      curl_mfprintf(मानक_त्रुटि, "LIMIT %s:%d %s reached memlimit\n",
                    source, ओळ, func);
      dump_bt();
      ख_साफ(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      त्रुटि_सं = ENOMEM;
      परत TRUE; /* RETURN ERROR! */
    }
    अन्यथा
      memsize--; /* countdown */
  }

  परत FALSE; /* allow this */
}

ALLOC_FUNC
शून्य *curl_dbg_malloc(माप_प्रकार wantedsize, पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  संरचना memdebug *mem;
  माप_प्रकार size;

  DEBUGASSERT(wantedsize != 0);

  जर(countcheck("malloc", ओळ, source))
    परत रिक्त;

  /* alloc at least 64 bytes */
  size = आकार(संरचना memdebug) + wantedsize;

  mem = Curl_cmalloc(size);
  जर(mem) {
    mem->size = wantedsize;
  }

  जर(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, ओळ, wantedsize,
                 mem ? (शून्य *)mem->mem : (शून्य *)0);

  परत mem ? mem->mem : रिक्त;
}

ALLOC_FUNC
शून्य *curl_dbg_calloc(माप_प्रकार wanted_elements, माप_प्रकार wanted_size,
                      पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  संरचना memdebug *mem;
  माप_प्रकार size, user_size;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_size != 0);

  जर(countcheck("calloc", ओळ, source))
    परत रिक्त;

  /* alloc at least 64 bytes */
  user_size = wanted_size * wanted_elements;
  size = आकार(संरचना memdebug) + user_size;

  mem = Curl_ccalloc(1, size);
  जर(mem)
    mem->size = user_size;

  जर(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, ओळ, wanted_elements, wanted_size,
                 mem ? (शून्य *)mem->mem : (शून्य *)0);

  परत mem ? mem->mem : रिक्त;
}

ALLOC_FUNC
अक्षर *curl_dbg_strdup(स्थिरांक अक्षर *str, पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  अक्षर *mem;
  माप_प्रकार len;

  DEBUGASSERT(str);

  जर(countcheck("strdup", ओळ, source))
    परत रिक्त;

  len = म_माप(str) + 1;

  mem = curl_dbg_malloc(len, 0, रिक्त); /* NULL prevents logging */
  जर(mem)
    स_नकल(mem, str, len);

  जर(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, ओळ, (स्थिरांक शून्य *)str, len, (स्थिरांक शून्य *)mem);

  परत mem;
}

#जर defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
ब_अक्षर_प्रकार *curl_dbg_wcsdup(स्थिरांक ब_अक्षर_प्रकार *str, पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  ब_अक्षर_प्रकार *mem;
  माप_प्रकार wsiz, bsiz;

  DEBUGASSERT(str);

  जर(countcheck("wcsdup", ओळ, source))
    परत रिक्त;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * आकार(ब_अक्षर_प्रकार);

  mem = curl_dbg_malloc(bsiz, 0, रिक्त); /* NULL prevents logging */
  जर(mem)
    स_नकल(mem, str, bsiz);

  जर(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, ओळ, (स्थिरांक शून्य *)str, bsiz, (शून्य *)mem);

  परत mem;
}
#endif

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
शून्य *curl_dbg_realloc(शून्य *ptr, माप_प्रकार wantedsize,
                       पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  संरचना memdebug *mem = रिक्त;
  bool was_locked;

  माप_प्रकार size = आकार(संरचना memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  जर(countcheck("realloc", ओळ, source))
    परत रिक्त;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#ifdef __INTEL_COMPILER
#  pragma warning(ढकला)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

  जर(ptr)
    mem = (शून्य *)((अक्षर *)ptr - दुरत्व(संरचना memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(काढा)
#endif

  mem = Curl_crealloc(mem, size);
  जर(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, ओळ, (शून्य *)ptr, wantedsize,
                        mem ? (शून्य *)mem->mem : (शून्य *)0);

  curl_dbg_unlock(was_locked);
  जर(mem) {
    mem->size = wantedsize;
    परत mem->mem;
  }

  परत रिक्त;
}

शून्य curl_dbg_free(शून्य *ptr, पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  जर(ptr) {
    संरचना memdebug *mem;

    जर(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, ओळ, (शून्य *)ptr);

#ifdef __INTEL_COMPILER
#  pragma warning(ढकला)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#endif

    mem = (शून्य *)((अक्षर *)ptr - दुरत्व(संरचना memdebug, mem));

#ifdef __INTEL_COMPILER
#  pragma warning(काढा)
#endif

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(पूर्णांक domain, पूर्णांक type, पूर्णांक protocol,
                              पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  curl_socket_t sockfd;

  जर(countcheck("socket", ओळ, source))
    परत CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  जर(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, ओळ, sockfd);

  परत sockfd;
}

#ifdef HAVE_SOCKETPAIR
पूर्णांक curl_dbg_socketpair(पूर्णांक domain, पूर्णांक type, पूर्णांक protocol,
                        curl_socket_t socket_vector[2],
                        पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  पूर्णांक res = socketpair(domain, type, protocol, socket_vector);

  जर(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, ओळ, socket_vector[0], socket_vector[1]);

  परत res;
}
#endif

curl_socket_t curl_dbg_accept(curl_socket_t s, शून्य *saddr, शून्य *saddrlen,
                              पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  संरचना sockaddr *addr = (संरचना sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  जर(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, ओळ, sockfd);

  परत sockfd;
}

#ifdef HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, शून्य *saddr, शून्य *saddrlen,
                               पूर्णांक flags,
                               पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  संरचना sockaddr *addr = (संरचना sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  जर(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, ओळ, sockfd);

  परत sockfd;
}
#endif

/* separate function to allow libcurl to mark a "faked" close */
शून्य curl_dbg_mark_sclose(curl_socket_t sockfd, पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  जर(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, ओळ, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
पूर्णांक curl_dbg_sclose(curl_socket_t sockfd, पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  curl_dbg_mark_sclose(sockfd, ओळ, source);
  परत CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
खाता *curl_dbg_fopen(स्थिरांक अक्षर *file, स्थिरांक अक्षर *mode,
                     पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  खाता *res = CURLX_FOPEN_खालचा(file, mode);
  जर(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, ओळ, file, mode, (शून्य *)res);

  परत res;
}

ALLOC_FUNC
खाता *curl_dbg_freopen(स्थिरांक अक्षर *file, स्थिरांक अक्षर *mode, खाता *fh,
                       पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  खाता *res = CURLX_FREOPEN_खालचा(file, mode, fh);
  जर(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, ओळ, file, mode, (शून्य *)fh, (शून्य *)res);

  परत res;
}

ALLOC_FUNC
खाता *curl_dbg_fdopen(पूर्णांक filedes, स्थिरांक अक्षर *mode,
                      पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  खाता *res = CURLX_FDOPEN_खालचा(filedes, mode);
  जर(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, ओळ, filedes, mode, (शून्य *)res);
  परत res;
}

पूर्णांक curl_dbg_fclose(खाता *file, पूर्णांक ओळ, स्थिरांक अक्षर *source)
{
  पूर्णांक res;

  DEBUGASSERT(file);

  जर(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, ओळ, (शून्य *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = ख_बंद(file);

  परत res;
}

स्थिर शून्य curl_dbg_vlog(स्थिरांक अक्षर * स्थिरांक fmt,
                          बहु_सूची ap) CURL_PRINTF(1, 0);

स्थिर शून्य curl_dbg_vlog(स्थिरांक अक्षर * स्थिरांक fmt, बहु_सूची ap)
{
  अक्षर buf[1024];
  माप_प्रकार nchars = curl_mvsnprintf(buf, आकार(buf), fmt, ap);

  जर(nchars > (पूर्णांक)आकार(buf) - 1)
    nchars = (पूर्णांक)आकार(buf) - 1;

  जर(nchars > 0) {
    जर(आकार(membuf) - nchars < memwidx) {
      /* flush */
      ख_डालो(membuf, 1, memwidx, curl_dbg_logfile);
      ख_साफ(curl_dbg_logfile);
      memwidx = 0;
    }
    जर(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    स_नकल(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

स्थिर शून्य curl_dbg_log_locked(स्थिरांक अक्षर *format, ...)
{
  बहु_सूची ap;

  जर(!curl_dbg_logfile)
    परत;

  बहु_शुरू(ap, format);
  curl_dbg_vlog(format, ap);
  बहु_पूर्ण(ap);
}

/* this does the writing to the memory tracking log file */
शून्य curl_dbg_log(स्थिरांक अक्षर *format, ...)
{
  bool was_locked;
  बहु_सूची ap;

  जर(!curl_dbg_logfile)
    परत;

  was_locked = curl_dbg_lock();
  बहु_शुरू(ap, format);
  curl_dbg_vlog(format, ap);
  बहु_पूर्ण(ap);
  curl_dbg_unlock(was_locked);
}

#endif /* CURL_MEMDEBUG */
