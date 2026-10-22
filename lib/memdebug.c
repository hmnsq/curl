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
#समावेश "curl_setup.h"

#अगर_घोषित CURL_MEMDEBUG

#समावेश <stddef.h>  /* for offsetof() */

#समावेश "urldata.h"
#समावेश "curl_threads.h"
#समावेश "curlx/win32-fopen.h"  /* for CURLX_FOPEN_*() */

#अगर_घोषित USE_BACKTRACE
#समावेश <backtrace.h>
#पूर्ण_अगर

काष्ठा memdebug {
  माप_प्रकार आकार;
  जोड़ {
    curl_off_t o;
    द्विगुन d;
    व्योम *p;
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

खाता *curl_dbg_logfile = शून्य;
अटल bool registered_cleanup = FALSE; /* atexit registered cleanup */
अटल bool memlimit = FALSE; /* enable memory limit */
अटल दीर्घ memsize = 0;  /* set number of mallocs allowed */
#अगर_घोषित USE_BACKTRACE
अटल काष्ठा backtrace_state *btstate;
#पूर्ण_अगर

अटल अक्षर membuf[10000];
अटल माप_प्रकार memwidx = 0; /* write index */

#अगर_घोषित USE_MUTEX
अटल bool dbg_mutex_init = 0;
अटल curl_mutex_t dbg_mutex;
#पूर्ण_अगर

अटल bool curl_dbg_lock(व्योम)
{
#अगर_घोषित USE_MUTEX
  अगर(dbg_mutex_init) {
    Curl_mutex_acquire(&dbg_mutex);
    वापस TRUE;
  }
#पूर्ण_अगर
  वापस FALSE;
}

अटल व्योम curl_dbg_unlock(bool was_locked)
{
#अगर_घोषित USE_MUTEX
  अगर(was_locked)
    Curl_mutex_release(&dbg_mutex);
#अन्यथा
  (व्योम)was_locked;
#पूर्ण_अगर
}

अटल व्योम curl_dbg_log_locked(स्थिर अक्षर *format, ...) CURL_PRINTF(1, 2);

/* LeakSantizier (LSAN) calls _exit() instead of exit() when a leak is detected
   on exit so the logfile must be closed explicitly or data could be lost.
   Though _exit() does not call atexit handlers such as this, LSAN's call to
   _exit() comes after the atexit handlers are called. curl/curl#6620 */
अटल व्योम curl_dbg_cleanup(व्योम)
{
  bool locked = curl_dbg_lock();
  अगर(curl_dbg_logfile &&
     curl_dbg_logfile != मानक_त्रुटि &&
     curl_dbg_logfile != मानक_निकास) {
    अगर(memwidx)
      ख_डालो(membuf, 1, memwidx, curl_dbg_logfile);
    /* !checksrc! disable BANNEDFUNC 1 */
    ख_बंद(curl_dbg_logfile);
  }
  curl_dbg_logfile = शून्य;
  curl_dbg_unlock(locked);
#अगर_घोषित USE_MUTEX
  अगर(dbg_mutex_init) {
    Curl_mutex_destroy(&dbg_mutex);
    dbg_mutex_init = FALSE;
  }
#पूर्ण_अगर
}

#अगर_घोषित USE_BACKTRACE
अटल व्योम error_bt_callback(व्योम *तथ्य, स्थिर अक्षर *message,
                              पूर्णांक error_संख्या)
{
  (व्योम)तथ्य;
  अगर(error_संख्या == -1)
    curl_dbg_log("compile with -g\n\n");
  अन्यथा
    curl_dbg_log("Backtrace error %d: %s\n", error_संख्या, message);
}

अटल पूर्णांक full_callback(व्योम *तथ्य, uintptr_t pc, स्थिर अक्षर *pathname,
                         पूर्णांक पंक्ति_संख्या, स्थिर अक्षर *function)
{
  (व्योम)तथ्य;
  (व्योम)pc;
  अगर(pathname || function || पंक्ति_संख्या)
    curl_dbg_log("BT %s:%d -- %s\n", pathname, पंक्ति_संख्या, function);
  वापस 0;
}

अटल व्योम dump_bt(व्योम)
{
  backtrace_full(btstate, 0, full_callback, error_bt_callback, शून्य);
}
#अन्यथा
#घोषणा dump_bt() /* nothing to do */
#पूर्ण_अगर

/* this sets the log filename */
व्योम curl_dbg_memdebug(स्थिर अक्षर *logname)
{
  अगर(!curl_dbg_logfile) {
    अगर(logname && *logname)
      curl_dbg_logfile = CURLX_FOPEN_निचला(logname, FOPEN_WRITETEXT);
#अगर_घोषित MEMDEBUG_LOG_SYNC
    /* Flush the log file after every line so the log is not lost in a crash */
    अगर(curl_dbg_logfile)
      रखो_बफ(curl_dbg_logfile, (अक्षर *)शून्य);
#पूर्ण_अगर
  }
#अगर_घोषित USE_MUTEX
  अगर(!dbg_mutex_init) {
    dbg_mutex_init = TRUE;
    Curl_mutex_init(&dbg_mutex);
  }
#पूर्ण_अगर
#अगर_घोषित USE_BACKTRACE
  btstate = backtrace_create_state(शून्य, 0, error_bt_callback, शून्य);
#पूर्ण_अगर
  अगर(!registered_cleanup)
    registered_cleanup = !निकास_पर(curl_dbg_cleanup);
}

/* This function sets the number of malloc() calls that should return
   successfully! */
व्योम curl_dbg_memlimit(दीर्घ limit)
{
  अगर(!memlimit) {
    memlimit = TRUE;
    memsize = limit;
  }
}

/* returns TRUE if this is not allowed! */
अटल bool countcheck(स्थिर अक्षर *func, पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  /* if source is NULL, then the call is made internally and this check
     should not be made */
  अगर(memlimit && source) {
    अगर(!memsize) {
      /* log to file */
      curl_dbg_log("LIMIT %s:%d %s reached memlimit\n", source, पंक्ति, func);
      /* log to stderr also */
      curl_mfprintf(मानक_त्रुटि, "LIMIT %s:%d %s reached memlimit\n",
                    source, पंक्ति, func);
      dump_bt();
      ख_साफ(curl_dbg_logfile); /* because it might crash now */
      /* !checksrc! disable ERRNOVAR 1 */
      त्रुटि_सं = ENOMEM;
      वापस TRUE; /* RETURN ERROR! */
    }
    अन्यथा
      memsize--; /* countdown */
  }

  वापस FALSE; /* allow this */
}

ALLOC_FUNC
व्योम *curl_dbg_malloc(माप_प्रकार wantedsize, पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  काष्ठा memdebug *mem;
  माप_प्रकार आकार;

  DEBUGASSERT(wantedsize != 0);

  अगर(countcheck("malloc", पंक्ति, source))
    वापस शून्य;

  /* alloc at least 64 bytes */
  आकार = माप(काष्ठा memdebug) + wantedsize;

  mem = Curl_cmalloc(आकार);
  अगर(mem) {
    mem->आकार = wantedsize;
  }

  अगर(source)
    curl_dbg_log("MEM %s:%d malloc(%zu) = %p\n",
                 source, पंक्ति, wantedsize,
                 mem ? (व्योम *)mem->mem : (व्योम *)0);

  वापस mem ? mem->mem : शून्य;
}

ALLOC_FUNC
व्योम *curl_dbg_calloc(माप_प्रकार wanted_elements, माप_प्रकार wanted_आकार,
                      पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  काष्ठा memdebug *mem;
  माप_प्रकार आकार, user_आकार;

  DEBUGASSERT(wanted_elements != 0);
  DEBUGASSERT(wanted_आकार != 0);

  अगर(countcheck("calloc", पंक्ति, source))
    वापस शून्य;

  /* alloc at least 64 bytes */
  user_आकार = wanted_आकार * wanted_elements;
  आकार = माप(काष्ठा memdebug) + user_आकार;

  mem = Curl_ccalloc(1, आकार);
  अगर(mem)
    mem->आकार = user_आकार;

  अगर(source)
    curl_dbg_log("MEM %s:%d calloc(%zu,%zu) = %p\n",
                 source, पंक्ति, wanted_elements, wanted_आकार,
                 mem ? (व्योम *)mem->mem : (व्योम *)0);

  वापस mem ? mem->mem : शून्य;
}

ALLOC_FUNC
अक्षर *curl_dbg_strdup(स्थिर अक्षर *str, पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  अक्षर *mem;
  माप_प्रकार len;

  DEBUGASSERT(str);

  अगर(countcheck("strdup", पंक्ति, source))
    वापस शून्य;

  len = म_माप(str) + 1;

  mem = curl_dbg_malloc(len, 0, शून्य); /* NULL prevents logging */
  अगर(mem)
    स_नकल(mem, str, len);

  अगर(source)
    curl_dbg_log("MEM %s:%d strdup(%p) (%zu) = %p\n",
                 source, पंक्ति, (स्थिर व्योम *)str, len, (स्थिर व्योम *)mem);

  वापस mem;
}

#अगर defined(_WIN32) && defined(UNICODE)
ALLOC_FUNC
ब_अक्षर_प्रकार *curl_dbg_wcsdup(स्थिर ब_अक्षर_प्रकार *str, पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  ब_अक्षर_प्रकार *mem;
  माप_प्रकार wsiz, bsiz;

  DEBUGASSERT(str);

  अगर(countcheck("wcsdup", पंक्ति, source))
    वापस शून्य;

  wsiz = wcslen(str) + 1;
  bsiz = wsiz * माप(ब_अक्षर_प्रकार);

  mem = curl_dbg_malloc(bsiz, 0, शून्य); /* NULL prevents logging */
  अगर(mem)
    स_नकल(mem, str, bsiz);

  अगर(source)
    curl_dbg_log("MEM %s:%d wcsdup(%p) (%zu) = %p\n",
                source, पंक्ति, (स्थिर व्योम *)str, bsiz, (व्योम *)mem);

  वापस mem;
}
#पूर्ण_अगर

/* We provide a realloc() that accepts a NULL as pointer, which then
   performs a malloc(). In order to work with ares. */
व्योम *curl_dbg_realloc(व्योम *ptr, माप_प्रकार wantedsize,
                       पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  काष्ठा memdebug *mem = शून्य;
  bool was_locked;

  माप_प्रकार आकार = माप(काष्ठा memdebug) + wantedsize;

  DEBUGASSERT(wantedsize != 0);

  अगर(countcheck("realloc", पंक्ति, source))
    वापस शून्य;

  /* need to realloc under lock, as we get out-of-order log
   * entries otherwise, since another thread might alloc the
   * memory released by realloc() before otherwise would log it. */
  was_locked = curl_dbg_lock();
#अगर_घोषित __INTEL_COMPILER
#  pragma warning(धकेलो)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#पूर्ण_अगर

  अगर(ptr)
    mem = (व्योम *)((अक्षर *)ptr - दुरत्व(काष्ठा memdebug, mem));

#अगर_घोषित __INTEL_COMPILER
#  pragma warning(निकालो)
#पूर्ण_अगर

  mem = Curl_crealloc(mem, आकार);
  अगर(source)
    curl_dbg_log_locked("MEM %s:%d realloc(%p, %zu) = %p\n",
                        source, पंक्ति, (व्योम *)ptr, wantedsize,
                        mem ? (व्योम *)mem->mem : (व्योम *)0);

  curl_dbg_unlock(was_locked);
  अगर(mem) {
    mem->आकार = wantedsize;
    वापस mem->mem;
  }

  वापस शून्य;
}

व्योम curl_dbg_free(व्योम *ptr, पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  अगर(ptr) {
    काष्ठा memdebug *mem;

    अगर(source)
      curl_dbg_log("MEM %s:%d free(%p)\n", source, पंक्ति, (व्योम *)ptr);

#अगर_घोषित __INTEL_COMPILER
#  pragma warning(धकेलो)
#  pragma warning(disable:1684)
   /* 1684: conversion from pointer to same-sized integral type */
#पूर्ण_अगर

    mem = (व्योम *)((अक्षर *)ptr - दुरत्व(काष्ठा memdebug, mem));

#अगर_घोषित __INTEL_COMPILER
#  pragma warning(निकालो)
#पूर्ण_अगर

    /* free for real */
    Curl_cfree(mem);
  }
}

curl_socket_t curl_dbg_socket(पूर्णांक domain, पूर्णांक type, पूर्णांक protocol,
                              पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  curl_socket_t sockfd;

  अगर(countcheck("socket", पंक्ति, source))
    वापस CURL_SOCKET_BAD;

  /* !checksrc! disable BANNEDFUNC 1 */
  sockfd = socket(domain, type, protocol);

  अगर(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d socket() = %" FMT_SOCKET_T "\n",
                 source, पंक्ति, sockfd);

  वापस sockfd;
}

#अगर_घोषित HAVE_SOCKETPAIR
पूर्णांक curl_dbg_socketpair(पूर्णांक domain, पूर्णांक type, पूर्णांक protocol,
                        curl_socket_t socket_vector[2],
                        पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  /* !checksrc! disable BANNEDFUNC 1 */
  पूर्णांक res = socketpair(domain, type, protocol, socket_vector);

  अगर(source && (res == 0))
    curl_dbg_log("FD %s:%d socketpair() = "
                 "%" FMT_SOCKET_T " %" FMT_SOCKET_T "\n",
                 source, पंक्ति, socket_vector[0], socket_vector[1]);

  वापस res;
}
#पूर्ण_अगर

curl_socket_t curl_dbg_accept(curl_socket_t s, व्योम *saddr, व्योम *saddrlen,
                              पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  काष्ठा sockaddr *addr = (काष्ठा sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept(s, addr, addrlen);

  अगर(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, पंक्ति, sockfd);

  वापस sockfd;
}

#अगर_घोषित HAVE_ACCEPT4
curl_socket_t curl_dbg_accept4(curl_socket_t s, व्योम *saddr, व्योम *saddrlen,
                               पूर्णांक flags,
                               पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  काष्ठा sockaddr *addr = (काष्ठा sockaddr *)saddr;
  curl_socklen_t *addrlen = (curl_socklen_t *)saddrlen;

  /* !checksrc! disable BANNEDFUNC 1 */
  curl_socket_t sockfd = accept4(s, addr, addrlen, flags);

  अगर(source && (sockfd != CURL_SOCKET_BAD))
    curl_dbg_log("FD %s:%d accept() = %" FMT_SOCKET_T "\n",
                 source, पंक्ति, sockfd);

  वापस sockfd;
}
#पूर्ण_अगर

/* separate function to allow libcurl to mark a "faked" close */
व्योम curl_dbg_mark_sclose(curl_socket_t sockfd, पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  अगर(source)
    curl_dbg_log("FD %s:%d sclose(%" FMT_SOCKET_T ")\n",
                 source, पंक्ति, sockfd);
}

/* this is our own defined way to close sockets on *ALL* platforms */
पूर्णांक curl_dbg_sclose(curl_socket_t sockfd, पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  curl_dbg_mark_sclose(sockfd, पंक्ति, source);
  वापस CURL_SCLOSE(sockfd);
}

ALLOC_FUNC
खाता *curl_dbg_fopen(स्थिर अक्षर *file, स्थिर अक्षर *mode,
                     पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  खाता *res = CURLX_FOPEN_निचला(file, mode);
  अगर(source)
    curl_dbg_log("FILE %s:%d fopen(\"%s\",\"%s\") = %p\n",
                 source, पंक्ति, file, mode, (व्योम *)res);

  वापस res;
}

ALLOC_FUNC
खाता *curl_dbg_freopen(स्थिर अक्षर *file, स्थिर अक्षर *mode, खाता *fh,
                       पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  खाता *res = CURLX_FREOPEN_निचला(file, mode, fh);
  अगर(source)
    curl_dbg_log("FILE %s:%d freopen(\"%s\",\"%s\",%p) = %p\n",
                 source, पंक्ति, file, mode, (व्योम *)fh, (व्योम *)res);

  वापस res;
}

ALLOC_FUNC
खाता *curl_dbg_fdopen(पूर्णांक filedes, स्थिर अक्षर *mode,
                      पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  खाता *res = CURLX_FDOPEN_निचला(filedes, mode);
  अगर(source)
    curl_dbg_log("FILE %s:%d fdopen(\"%d\",\"%s\") = %p\n",
                 source, पंक्ति, filedes, mode, (व्योम *)res);
  वापस res;
}

पूर्णांक curl_dbg_fclose(खाता *file, पूर्णांक पंक्ति, स्थिर अक्षर *source)
{
  पूर्णांक res;

  DEBUGASSERT(file);

  अगर(source)
    curl_dbg_log("FILE %s:%d fclose(%p)\n", source, पंक्ति, (व्योम *)file);

  /* !checksrc! disable BANNEDFUNC 1 */
  res = ख_बंद(file);

  वापस res;
}

अटल व्योम curl_dbg_vlog(स्थिर अक्षर * स्थिर fmt,
                          बहु_सूची ap) CURL_PRINTF(1, 0);

अटल व्योम curl_dbg_vlog(स्थिर अक्षर * स्थिर fmt, बहु_सूची ap)
{
  अक्षर buf[1024];
  माप_प्रकार nchars = curl_mvsnprintf(buf, माप(buf), fmt, ap);

  अगर(nchars > (पूर्णांक)माप(buf) - 1)
    nchars = (पूर्णांक)माप(buf) - 1;

  अगर(nchars > 0) {
    अगर(माप(membuf) - nchars < memwidx) {
      /* flush */
      ख_डालो(membuf, 1, memwidx, curl_dbg_logfile);
      ख_साफ(curl_dbg_logfile);
      memwidx = 0;
    }
    अगर(memwidx) {
      /* the previous line ends with a newline */
      DEBUGASSERT(membuf[memwidx - 1] == '\n');
    }
    स_नकल(&membuf[memwidx], buf, nchars);
    memwidx += nchars;
  }
}

अटल व्योम curl_dbg_log_locked(स्थिर अक्षर *format, ...)
{
  बहु_सूची ap;

  अगर(!curl_dbg_logfile)
    वापस;

  बहु_शुरू(ap, format);
  curl_dbg_vlog(format, ap);
  बहु_पूर्ण(ap);
}

/* this does the writing to the memory tracking log file */
व्योम curl_dbg_log(स्थिर अक्षर *format, ...)
{
  bool was_locked;
  बहु_सूची ap;

  अगर(!curl_dbg_logfile)
    वापस;

  was_locked = curl_dbg_lock();
  बहु_शुरू(ap, format);
  curl_dbg_vlog(format, ap);
  बहु_पूर्ण(ap);
  curl_dbg_unlock(was_locked);
}

#पूर्ण_अगर /* CURL_MEMDEBUG */
