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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "api.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vquic/vquic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "getinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "easyif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multi_ev.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h" /* for Curl_getconnectinfo */
#ਅਨ੍ਤਰ੍ਭਾਵ "slist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mime.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "amigaos.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "macos.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/wait.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sigpipe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/ssh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "setopt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http_digest.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/dynbuf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "altsvc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hsts.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/timeval.h" /* for curlx_now_init() */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#ਅਨ੍ਤਰ੍ਭਾਵ "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ initialized;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
ਸਥਿਰ bool winsock_initialized;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GLOBAL_INIT_IS_THREADSAFE

ਸਥਿਰ curl_simple_ਤਾਲਾ s_ਤਾਲਾ = CURL_SIMPLE_ਤਾਲਾ_INIT;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ global_init_ਤਾਲਾ()   curl_simple_ਤਾਲਾ_ਤਾਲਾ(&s_ਤਾਲਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ global_init_unlock() curl_simple_ਤਾਲਾ_unlock(&s_ਤਾਲਾ)

#ਨਹੀਂਤਾਂ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ global_init_ਤਾਲਾ()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ global_init_unlock()

#ਜੇ_ਅੰਤ

#ਜੇ defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ਧੱਕੋ)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#ਜੇ_ਅੰਤ

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)ਸਮ੍ਰਿਤੀ_ਦਿਓ;
curl_free_callback Curl_cfree = (curl_free_callback)ਮੁਕਤ;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_ਨੀਵਾਂ;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ;

#ਜੇ defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ਕੱਢੋ)
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
ਸਥਿਰ ਅੱਖਰ *leakpointer;
#ਜੇ_ਅੰਤ

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
ਸਥਿਰ CURLcode global_init(ਲੰਮਾ flags, bool memoryfuncs)
{
  (ਖਾਲੀ)flags;

  ਜੇ(initialized++)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)ਸਮ੍ਰਿਤੀ_ਦਿਓ;
    Curl_cfree = (curl_free_callback)ਮੁਕਤ;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_ਨੀਵਾਂ;
    Curl_ccalloc = (curl_calloc_callback)ਸਿਫਰ_ਸਮ੍ਰਿਤੀ_ਦਿਓ;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#ਜੇ_ਅੰਤ

  ਜੇ(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    ਜਾਓ fail;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  ਜੇ(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    ਜਾਓ fail;
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  ਜੇ(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    ਜੇ(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      ਜਾਓ fail;
    }
    winsock_initialized = TRUE;
  }
#ਜੇ_ਅੰਤ

  ਜੇ(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    ਜਾਓ fail;
  }

  ਜੇ(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    ਜਾਓ fail;
  }

  ਜੇ(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    ਜਾਓ fail;
  }

  ਜੇ(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    ਜਾਓ fail;
  }

  ਜੇ(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    ਜਾਓ fail;
  }

  ਜੇ(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    ਜਾਓ fail;
  }

  ਜੇ(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    ਜਾਓ fail;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਜੇ(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#ਜੇ_ਅੰਤ

  ਵਾਪਸ CURLE_OK;

fail:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  ਜੇ(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#ਜੇ_ਅੰਤ
  initialized--; /* undo the increase */
  ਵਾਪਸ CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(ਲੰਮਾ flags)
{
  CURLcode ਨਤੀਜਾ;
  global_init_ਤਾਲਾ();

  ਨਤੀਜਾ = global_init(flags, TRUE);

  global_init_unlock();

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * curl_global_init_mem() globally initializes curl and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(ਲੰਮਾ flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode ਨਤੀਜਾ;

  /* Invalid input, return immediately */
  ਜੇ(!m || !f || !r || !s || !c)
    ਵਾਪਸ CURLE_FAILED_INIT;

  global_init_ਤਾਲਾ();

  ਜੇ(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    ਵਾਪਸ CURLE_OK;
  }

  /* set memory functions before global_init() in case it wants memory
     functions */
  Curl_cmalloc = m;
  Curl_cfree = f;
  Curl_cstrdup = s;
  Curl_crealloc = r;
  Curl_ccalloc = c;

  /* Call the actual init function, but without setting */
  ਨਤੀਜਾ = global_init(flags, FALSE);

  global_init_unlock();

  ਵਾਪਸ ਨਤੀਜਾ;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
ਖਾਲੀ curl_global_cleanup(ਖਾਲੀ)
{
  global_init_ਤਾਲਾ();

  ਜੇ(!initialized) {
    global_init_unlock();
    ਵਾਪਸ;
  }

  ਜੇ(--initialized) {
    global_init_unlock();
    ਵਾਪਸ;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  ਜੇ(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
  Curl_sspi_global_cleanup();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  curlx_free(leakpointer);
#ਜੇ_ਅੰਤ

  global_init_unlock();
}

/**
 * curl_global_trace() globally initializes curl logging.
 */
CURLcode curl_global_trace(ਅਚਲ ਅੱਖਰ *config)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_VERBOSE_ਲੜੀਆਂ
  CURLcode ਨਤੀਜਾ;
  global_init_ਤਾਲਾ();

  ਨਤੀਜਾ = Curl_trc_opt(config);

  global_init_unlock();

  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)config;
  ਵਾਪਸ CURLE_OK;
#ਜੇ_ਅੰਤ
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ਅਚਲ ਅੱਖਰ *name,
                              ਅਚਲ curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_ਤਾਲਾ();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  ਵਾਪਸ rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(ਖਾਲੀ)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;

  /* Make sure we inited the global SSL stuff */
  global_init_ਤਾਲਾ();

  ਜੇ(!initialized) {
    ਨਤੀਜਾ = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    ਜੇ(ਨਤੀਜਾ) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      ਵਾਪਸ ਨਿਰਾ;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  ਨਤੀਜਾ = Curl_open(&ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    ਵਾਪਸ ਨਿਰਾ;
  }

  ਵਾਪਸ ਅੰਕੜੇ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD

ਬਣਤਰ socketmonitor {
  ਬਣਤਰ socketmonitor *ਅਗਲਾ; /* the next node in the list or NULL */
  ਬਣਤਰ pollfd socket; /* socket info of what to monitor */
};

ਬਣਤਰ events {
  ਲੰਮਾ ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  ਪੂਰਨ ਅੰਕ_sockets;      /* number of nodes in the monitor list */
  ਬਣਤਰ socketmonitor *list; /* list of sockets to monitor */
  ਪੂਰਨ running_handles;  /* store the returned number */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
ਸਥਿਰ ਪੂਰਨ events_timer(CURLM *multi,    /* multi handle */
                        ਲੰਮਾ timeout_ms, /* see above */
                        ਖਾਲੀ *userp)     /* private callback pointer */
{
  ਬਣਤਰ events *ev = userp;
  (ਖਾਲੀ)multi;
#ਜੇ DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#ਜੇ_ਅੰਤ
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  ਵਾਪਸ 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
ਸਥਿਰ ਪੂਰਨ poll2cselect(ਪੂਰਨ pollmask)
{
  ਪੂਰਨ omask = 0;
  ਜੇ(pollmask & POLLIN)
    omask |= CURL_CSELECT_ਵਿੱਚ;
  ਜੇ(pollmask & POLLOUT)
    omask |= CURL_CSELECT_ਬਾਹਰਲਾ;
  ਜੇ(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  ਵਾਪਸ omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
ਸਥਿਰ ਛੋਟਾ socketcb2poll(ਪੂਰਨ pollmask)
{
  ਛੋਟਾ omask = 0;
  ਜੇ(pollmask & CURL_POLL_ਵਿੱਚ)
    omask |= POLLIN;
  ਜੇ(pollmask & CURL_POLL_ਬਾਹਰਲਾ)
    omask |= POLLOUT;
  ਵਾਪਸ omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
ਸਥਿਰ ਪੂਰਨ events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         ਪੂਰਨ what,        /* see above */
                         ਖਾਲੀ *userp,     /* private callback
ਸੂਚਕ*/
                         ਖਾਲੀ *socketp)   /* private socket
ਸੂਚਕ*/
{
  ਬਣਤਰ events *ev = userp;
  ਬਣਤਰ socketmonitor *m;
  ਬਣਤਰ socketmonitor *prev = ਨਿਰਾ;
  bool ਮਿਲਿਆ = FALSE;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = easy;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_VERBOSE_ਲੜੀਆਂ
  (ਖਾਲੀ)easy;
#ਜੇ_ਅੰਤ
  (ਖਾਲੀ)socketp;

  m = ev->list;
  ਜਦੋਂ(m) {
    ਜੇ(m->socket.fd == s) {
      ਮਿਲਿਆ = TRUE;
      ਜੇ(what == CURL_POLL_REMOVE) {
        ਬਣਤਰ socketmonitor *nxt = m->ਅਗਲਾ;
        /* remove this node from the list of monitored sockets */
        ਜੇ(prev)
          prev->ਅਗਲਾ = nxt;
        ਨਹੀਂਤਾਂ
          ev->list = nxt;
        curlx_free(m);
        infof(ਅੰਕੜੇ, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      ਨਹੀਂਤਾਂ {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(ਅੰਕੜੇ, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_ਵਿੱਚ) ? "IN" : "",
              (what & CURL_POLL_ਬਾਹਰਲਾ) ? "OUT" : "");
      }
      ਰੋਕੋ;
    }
    prev = m;
    m = m->ਅਗਲਾ; /* move to next node */
  }

  ਜੇ(!ਮਿਲਿਆ) {
    ਜੇ(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(ਅੰਕੜੇ, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    ਨਹੀਂਤਾਂ {
      m = curlx_malloc(ਆਕਾਰ(ਬਣਤਰ socketmonitor));
      ਜੇ(m) {
        m->ਅਗਲਾ = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(ਅੰਕੜੇ, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_ਵਿੱਚ) ? "IN" : "",
              (what & CURL_POLL_ਬਾਹਰਲਾ) ? "OUT" : "");
      }
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  ਵਾਪਸ 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
ਸਥਿਰ ਖਾਲੀ events_ਤਿਆਰੀ(ਬਣਤਰ Curl_multi *multi, ਬਣਤਰ events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

ਸਥਿਰ CURLcode populate_cpfds(ਬਣਤਰ curl_pollfds *cpfds, ਬਣਤਰ events *ev)
{
  ਬਣਤਰ socketmonitor *m;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  ਲਈ(m = ev->list; m && !ਨਤੀਜਾ; m = m->ਅਗਲਾ) {
    ਨਤੀਜਾ = Curl_pollfds_ਜੋੜੋ_sock(cpfds, m->socket.fd, m->socket.events);
#ਜੇ DEBUG_EV_POLL
    ਜੇ(!ਨਤੀਜਾ)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#ਜੇ_ਅੰਤ
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode poll_cpfds(ਬਣਤਰ events *ev,
                           ਬਣਤਰ curl_pollfds *cpfds,
                           ਪੂਰਨ *pollrc)
{
  ਜੇ(cpfds->n) {
    /* wait for activity or timeout */
#ਜੇ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#ਜੇ_ਅੰਤ
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#ਜੇ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#ਜੇ_ਅੰਤ
    ਜੇ(*pollrc < 0)
      ਵਾਪਸ CURLE_UNRECOVERABLE_POLL;
  }
  ਨਹੀਂਤਾਂ {
#ਜੇ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#ਜੇ_ਅੰਤ
    *pollrc = 0;
    ਜੇ(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  ਵਾਪਸ CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
ਸਥਿਰ CURLcode wait_or_timeout(ਬਣਤਰ Curl_multi *multi, ਬਣਤਰ events *ev)
{
  ਬਣਤਰ pollfd fds_on_ਢੇਰ[8];
  ਬਣਤਰ curl_pollfds cpfds;
  bool ਪੂਰਾ = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_ਢੇਰ, CURL_ARRAYSIZE(fds_on_ਢੇਰ));
  ਜਦੋਂ(!ਪੂਰਾ) {
    CURLMsg *ਸੁਨੇਹਾ;
    ਪੂਰਨ pollrc;
    ਬਣਤਰ curltime ਸ਼ੁਰੂ;

    ਨਤੀਜਾ = populate_cpfds(&cpfds, ev);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&ਸ਼ੁਰੂ);

    ਨਤੀਜਾ = poll_cpfds(ev, &cpfds, &pollrc);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ev->msbump = FALSE; /* reset here */

    ਜੇ(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#ਜੇ DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#ਜੇ_ਅੰਤ
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    ਨਹੀਂਤਾਂ {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
      ਲਈ(i = 0; i < cpfds.n; i++) {
        ਜੇ(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          ਪੂਰਨ act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      ਜੇ(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), ਸ਼ੁਰੂ);
        ਜੇ(spent_ms > 0) {
#ਜੇ DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (ਲੰਮਾ)spent_ms);
#ਜੇ_ਅੰਤ
          ਜੇ(spent_ms > ev->ms)
            ev->ms = 0;
          ਨਹੀਂਤਾਂ
            ev->ms -= (ਲੰਮਾ)spent_ms;
        }
      }
    }

    ਜੇ(mresult) {
      ਨਤੀਜਾ = CURLE_URL_MALFORMAT;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    ਸੁਨੇਹਾ = curl_multi_info_read(multi, &pollrc);
    ਜੇ(ਸੁਨੇਹਾ) {
      ਨਤੀਜਾ = ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਨਤੀਜਾ;
      ਪੂਰਾ = TRUE;
    }
  }
ਬਾਹਰਲਾ:
  Curl_pollfds_cleanup(&cpfds);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
ਸਥਿਰ CURLcode easy_events(ਬਣਤਰ Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  ਸਥਿਰ ਬਣਤਰ events evs = { -1, FALSE, 0, ਨਿਰਾ, 0 };

  /* if running event-based, do some further multi inits */
  events_ਤਿਆਰੀ(multi, &evs);

  ਵਾਪਸ wait_or_timeout(multi, &evs);
}
#ਨਹੀਂਤਾਂ /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ easy_events(x) CURLE_NOT_BUILT_ਵਿੱਚ
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode easy_transfer(ਬਣਤਰ Curl_multi *multi)
{
  bool ਪੂਰਾ = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜਦੋਂ(!ਪੂਰਾ && !mresult) {
    ਪੂਰਨ still_running = 0;

    mresult = curl_multi_poll(multi, ਨਿਰਾ, 0, 1000, ਨਿਰਾ);

    ਜੇ(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    ਜੇ(!mresult && !still_running) {
      ਪੂਰਨ rc;
      CURLMsg *ਸੁਨੇਹਾ = curl_multi_info_read(multi, &rc);
      ਜੇ(ਸੁਨੇਹਾ) {
        ਨਤੀਜਾ = ਸੁਨੇਹਾ->ਅੰਕੜੇ.ਨਤੀਜਾ;
        ਪੂਰਾ = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  ਜੇ(mresult) {
    ਨਤੀਜਾ = (mresult == CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) ? CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * easy_perform() is the internal interface that performs a blocking
 * transfer as previously setup.
 *
 * CONCEPT: This function creates a multi handle, adds the easy handle to it,
 * runs curl_multi_perform() until the transfer is done, then detaches the
 * easy handle, destroys the multi handle and returns the easy handle's return
 * code.
 *
 * REALITY: it cannot create and destroy the multi handle that easily. It
 * needs to keep it around since if this easy handle is used again by this
 * function, the same multi handle must be reused so that the same pools and
 * caches can be used.
 *
 * DEBUG: if 'events' is set TRUE, this function will use a replacement engine
 * instead of curl_multi_perform() and use curl_multi_socket_action().
 */
ਸਥਿਰ CURLcode easy_perform(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool events)
{
  ਬਣਤਰ Curl_multi *multi;
  CURLMcode mresult;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ Curl_sigpipe_ctx sigpipe_ctx;

  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.errorbuffer)
    /* clear this as early as possible */
    ਅੰਕੜੇ->ਸੈੱਟ.errorbuffer[0] = 0;

  ਅੰਕੜੇ->state.os_errno = 0;

  ਜੇ(ਅੰਕੜੇ->multi) {
    failf(ਅੰਕੜੇ, "easy handle already used in multi handle");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  ਜੇ(ਅੰਕੜੇ->conn) {
    ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
    Curl_detach_connection(ਅੰਕੜੇ);
    Curl_conn_close(ਅੰਕੜੇ, conn, TRUE);
    DEBUGASSERT(!ਅੰਕੜੇ->conn);
  }

  ਜੇ(ਅੰਕੜੇ->multi_easy)
    multi = ਅੰਕੜੇ->multi_easy;
  ਨਹੀਂਤਾਂ {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    ਜੇ(!multi)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(Curl_api_multi_is_ਵਿੱਚ_callback(multi))
    ਵਾਪਸ CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (ਲੰਮਾ)ਅੰਕੜੇ->ਸੈੱਟ.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_ਤੇਜ਼_EXIT, (ਲੰਮਾ)ਅੰਕੜੇ->ਸੈੱਟ.quick_exit);

  ਅੰਕੜੇ->multi_easy = ਨਿਰਾ; /* pretend it does not exist */
  mresult = Curl_multi_ਜੋੜੋ_handle(multi, ਅੰਕੜੇ);
  ਜੇ(mresult) {
    curl_multi_cleanup(multi);
    ਜੇ(mresult == CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  ਅੰਕੜੇ->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(ਅੰਕੜੇ, &sigpipe_ctx);

  /* run the transfer */
  ਨਤੀਜਾ = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (ਖਾਲੀ)Curl_multi_remove_handle(multi, ਅੰਕੜੇ);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  ਬਣਤਰ Curl_eapi_guard guard = { 0 };
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_perform, &ਨਤੀਜਾ)) {
    ਨਤੀਜਾ = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(ਬਣਤਰ Curl_easy *easy)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &ਨਤੀਜਾ)) {
    ਨਤੀਜਾ = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
ਖਾਲੀ curl_easy_cleanup(CURL *curl)
{
  ਬਣਤਰ Curl_eapi_guard guard;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, ਨਿਰਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    ਬਣਤਰ Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(ਅੰਕੜੇ, &sigpipe_ctx);
    Curl_close(&ਅੰਕੜੇ);
    sigpipe_restore(&sigpipe_ctx);
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &ਨਤੀਜਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    va_list arg;
    ਖਾਲੀ *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, ਖਾਲੀ *);

    ਨਤੀਜਾ = Curl_getinfo(ਅੰਕੜੇ, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode dupset(ਬਣਤਰ Curl_easy *dst, ਬਣਤਰ Curl_easy *src)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਗਿਣਤੀ dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->ਸੈੱਟ = src->ਸੈੱਟ;
#ਜੇ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->ਸੈੱਟ.mimepostp = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
  dst->ਸੈੱਟ.str_copypostfields = ਨਿਰਾ;

  Curl_u8_strset_init(&dst->ਸੈੱਟ.ਲੜੀਆਂ);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->ਸੈੱਟ.blobs, 0, BLOB_ਆਖਰੀ * ਆਕਾਰ(ਬਣਤਰ curl_blob *));

  /* duplicate all strings */
  ਨਤੀਜਾ = Curl_u8_strset_copy(&dst->ਸੈੱਟ.ਲੜੀਆਂ, &src->ਸੈੱਟ.ਲੜੀਆਂ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* duplicate all blobs */
  ਲਈ(j = (ਗਿਣਤੀ dupblob)0; j < BLOB_ਆਖਰੀ; j++) {
    ਨਤੀਜਾ = Curl_setblobopt(&dst->ਸੈੱਟ.blobs[j], src->ਸੈੱਟ.blobs[j]);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* duplicate memory areas pointed to */
  ਜੇ(src->ਸੈੱਟ.str_copypostfields) {
    ਜੇ(src->ਸੈੱਟ.postfieldsize == -1)
      dst->ਸੈੱਟ.str_copypostfields = curlx_strdup(src->ਸੈੱਟ.str_copypostfields);
    ਨਹੀਂਤਾਂ
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->ਸੈੱਟ.str_copypostfields =
        curlx_memdup0(src->ਸੈੱਟ.str_copypostfields,
                      curlx_sotouz(src->ਸੈੱਟ.postfieldsize));
    ਜੇ(!dst->ਸੈੱਟ.str_copypostfields)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    /* point to the new copy */
    dst->ਸੈੱਟ.postfields = dst->ਸੈੱਟ.str_copypostfields;
  }

#ਜੇ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ਜੇ(src->ਸੈੱਟ.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->ਸੈੱਟ.mimepostp = curlx_malloc(ਆਕਾਰ(*dst->ਸੈੱਟ.mimepostp));
    ਜੇ(!dst->ਸੈੱਟ.mimepostp)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    Curl_mime_initpart(dst->ਸੈੱਟ.mimepostp);
    ਨਤੀਜਾ = Curl_mime_duppart(dst, dst->ਸੈੱਟ.mimepostp, src->ਸੈੱਟ.mimepostp);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਅੰਤ

  ਜੇ(src->ਸੈੱਟ.resolve)
    dst->state.resolve = dst->ਸੈੱਟ.resolve;

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ dupeasy_meta_freeentry(ਖਾਲੀ *p)
{
  (ਖਾਲੀ)p;
  /* Always FALSE. Cannot use a 0 assert here since compilers
   * are not in agreement if they then want a NORETURN attribute or
   * not. *sigh* */
  DEBUGASSERT(!p);
}

/*
 * curl_easy_duphandle() is an external interface to allow duplication of a
 * given input easy handle. The returned handle will be a new working handle
 * with all options set exactly as the input source handle.
 */
CURL *curl_easy_duphandle(CURL *curl)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  ਬਣਤਰ Curl_easy *outcurl = ਨਿਰਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, ਨਿਰਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;

    outcurl = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_easy));
    ਜੇ(!outcurl)
      ਜਾਓ fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->ਸੈੱਟ.ਬਫਰ_ਮਾਪ = ਅੰਕੜੇ->ਸੈੱਟ.ਬਫਰ_ਮਾਪ;

    Curl_hash_init(&outcurl->meta_hash, 23, CURL_HASH_TYPE_ਬਾਈਟਾਂ,
                   dupeasy_meta_freeentry);
    curlx_dyn_init(&outcurl->state.headerb, CURL_MAX_HTTP_ਸਿਰਲੇਖ);
    Curl_bufref_init(&outcurl->state.url);
    Curl_bufref_init(&outcurl->state.referer);
    Curl_netrc_init(&outcurl->state.netrc);

    /* the connection pool is setup on demand */
    outcurl->state.ਆਖਰੀ_conn_id = -1;
    outcurl->state.ਆਖਰੀ_cpid = UINT32_MAX;
    outcurl->id = -1;
    outcurl->mid = UINT32_MAX;
    outcurl->master_mid = UINT32_MAX;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
    Curl_llist_init(&outcurl->state.httphdrs, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    ਜੇ(dupset(outcurl, ਅੰਕੜੇ))
      ਜਾਓ fail;

    outcurl->ਤਰੱਕੀ.hide     = ਅੰਕੜੇ->ਤਰੱਕੀ.hide;
    outcurl->ਤਰੱਕੀ.callback = ਅੰਕੜੇ->ਤਰੱਕੀ.callback;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
    outcurl->state.cookielist = ਨਿਰਾ;
    ਜੇ(ਅੰਕੜੇ->cookies && ਅੰਕੜੇ->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      ਜੇ(!outcurl->cookies)
        ਜਾਓ fail;
      outcurl->state.cookie_engine = TRUE;
    }

    ਜੇ(ਅੰਕੜੇ->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(ਅੰਕੜੇ->state.cookielist);
      ਜੇ(!outcurl->state.cookielist)
        ਜਾਓ fail;
    }
#ਜੇ_ਅੰਤ

    ਜੇ(Curl_bufref_ptr(&ਅੰਕੜੇ->state.url)) {
      Curl_bufref_ਸੈੱਟ(&outcurl->state.url,
                      Curl_bufref_dup(&ਅੰਕੜੇ->state.url), 0,
                      curl_free);
      ਜੇ(!Curl_bufref_ptr(&outcurl->state.url))
        ਜਾਓ fail;
    }
    ਜੇ(Curl_bufref_ptr(&ਅੰਕੜੇ->state.referer)) {
      Curl_bufref_ਸੈੱਟ(&outcurl->state.referer,
                      Curl_bufref_dup(&ਅੰਕੜੇ->state.referer), 0,
                      curl_free);
      ਜੇ(!Curl_bufref_ptr(&outcurl->state.referer))
        ਜਾਓ fail;
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_ALTSVC
    ਜੇ(ਅੰਕੜੇ->asi) {
      ਅਚਲ ਅੱਖਰ *str;
      outcurl->asi = Curl_altsvc_init();
      ਜੇ(!outcurl->asi)
        ਜਾਓ fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      ਜੇ(str)
        (ਖਾਲੀ)Curl_altsvc_ਲੱਦੋ(outcurl->asi, str);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HSTS
    ਜੇ(ਅੰਕੜੇ->hsts) {
      ਅਚਲ ਅੱਖਰ *str;
      outcurl->hsts = Curl_hsts_init();
      ਜੇ(!outcurl->hsts)
        ਜਾਓ fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      ਜੇ(str)
        (ਖਾਲੀ)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (ਖਾਲੀ)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      ਜੇ(Curl_hsts_copy(outcurl->hsts, ਅੰਕੜੇ->hsts))
        ਜਾਓ fail;
    }
#ਜੇ_ਅੰਤ

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_ਸੰਖਿਆ;
  }
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ outcurl;

fail:
  ਜੇ(outcurl) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_COOKIES
    curlx_free(outcurl->cookies);
#ਜੇ_ਅੰਤ
    curlx_dyn_free(&outcurl->state.headerb);
    Curl_altsvc_cleanup(&outcurl->asi);
    Curl_hsts_cleanup(&outcurl->hsts);
    Curl_freeset(outcurl);
    curlx_free(outcurl);
  }

  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਿਰਾ;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
ਖਾਲੀ curl_easy_reset(CURL *curl)
{
  ਬਣਤਰ Curl_eapi_guard guard;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_reset, ਨਿਰਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;

    ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id = -1; /* clear remembered connection id */
    ਅੰਕੜੇ->state.ਆਖਰੀ_cpid = UINT32_MAX;
    Curl_req_hard_reset(&ਅੰਕੜੇ->req, ਅੰਕੜੇ);
    Curl_hash_clean(&ਅੰਕੜੇ->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(ਅੰਕੜੇ);
    /* zero out UserDefined data: */
    Curl_freeset(ਅੰਕੜੇ);
    memset(&ਅੰਕੜੇ->ਸੈੱਟ, 0, ਆਕਾਰ(ਬਣਤਰ UserDefined));
    Curl_init_userdefined(ਅੰਕੜੇ);

    /* zero out Progress data: */
    memset(&ਅੰਕੜੇ->ਤਰੱਕੀ, 0, ਆਕਾਰ(ਬਣਤਰ ਤਰੱਕੀ_2));

    /* zero out PureInfo data: */
    Curl_initinfo(ਅੰਕੜੇ);

    ਅੰਕੜੇ->ਤਰੱਕੀ.hide = TRUE;

    /* zero out authentication data: */
    memset(&ਅੰਕੜੇ->state.authhost, 0, ਆਕਾਰ(ਬਣਤਰ auth));
    memset(&ਅੰਕੜੇ->state.authproxy, 0, ਆਕਾਰ(ਬਣਤਰ auth));

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ
    ਅੰਕੜੇ->master_mid = UINT32_MAX;
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_pause() allows an application to pause or unpause a specific
 * transfer and direction. This function sets the full new state for the
 * current connection this easy handle operates on.
 *
 * NOTE: if you have the receiving paused and you call this function to remove
 * the pausing, you may get your write callback called at this point.
 *
 * Action is a bitmask consisting of CURLPAUSE_* bits in curl/curl.h
 *
 * NOTE: This is one of few API functions that are allowed to be called from
 * within a callback.
 */
CURLcode curl_easy_pause(CURL *curl, ਪੂਰਨ action)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_pause, &ਨਤੀਜਾ)) {
    bool ਬਦਲਿਆ = FALSE;
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    ਜੇ(!ਅੰਕੜੇ->conn) {
      /* crazy input, do not continue */
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    recv_paused = Curl_xfer_recv_is_paused(ਅੰਕੜੇ);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(ਅੰਕੜੇ);
    send_paused_new = (action & CURLPAUSE_SEND);

    ਜੇ((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(ਅੰਕੜੇ))) {
      ਬਦਲਿਆ = TRUE;
      ਨਤੀਜਾ = Curl_1st_fatal(
        ਨਤੀਜਾ, Curl_xfer_pause_send(ਅੰਕੜੇ, send_paused_new));
    }

    ਜੇ(recv_paused != recv_paused_new) {
      ਬਦਲਿਆ = TRUE;
      ਨਤੀਜਾ = Curl_1st_fatal(
        ਨਤੀਜਾ, Curl_xfer_pause_recv(ਅੰਕੜੇ, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    ਜੇ(!Curl_xfer_is_blocked(ਅੰਕੜੇ)) {
      /* reset the too-slow time keeper */
      ਅੰਕੜੇ->state.keeps_ਰਫਤਾਰ.tv_sec = 0;
      ਜੇ(ਅੰਕੜੇ->multi) {
        Curl_multi_mark_dirty(ਅੰਕੜੇ); /* make it run */
        /* On changes, tell application to update its timers. */
        ਜੇ(ਬਦਲਿਆ && Curl_update_timer(ਅੰਕੜੇ->multi) && !ਨਤੀਜਾ)
          ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    ਜੇ(!ਨਤੀਜਾ && ਬਦਲਿਆ && !ਅੰਕੜੇ->state.ਪੂਰਾ && ਅੰਕੜੇ->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(ਅੰਕੜੇ->multi, ਅੰਕੜੇ))
      ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
  }
ਬਾਹਰਲਾ:
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode easy_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata **connp)
{
  curl_socket_t sfd;

  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  ਜੇ(!ਅੰਕੜੇ->ਸੈੱਟ.connect_only) {
    failf(ਅੰਕੜੇ, "CONNECT_ONLY is required");
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(ਅੰਕੜੇ, connp);

  ਜੇ(sfd == CURL_SOCKET_BAD) {
    failf(ਅੰਕੜੇ, "Failed to get last socket used for connection #%" FMT_OFF_T,
          ਅੰਕੜੇ->state.ਆਖਰੀ_conn_id);
    ਵਾਪਸ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ਵਾਪਸ CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਖਾਲੀ *ਬਫਰ, size_t buflen, size_t *n)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *c;

  ਨਤੀਜਾ = easy_connection(ਅੰਕੜੇ, &c);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(!ਅੰਕੜੇ->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(ਅੰਕੜੇ, c, TRUE);

  *n = 0;
  ਵਾਪਸ Curl_conn_recv(ਅੰਕੜੇ, FIRSTSOCKET, ਬਫਰ, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, ਖਾਲੀ *ਬਫਰ, size_t buflen, size_t *n)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_recv, &ਨਤੀਜਾ)) {
    ਜੇ(!n || (buflen && !ਬਫਰ)) {
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਤੀਜਾ = Curl_easy_recv(curl, ਬਫਰ, buflen, n);
  }
ਬਾਹਰਲਾ:
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *c = ਨਿਰਾ;

  ਨਤੀਜਾ = easy_connection(ਅੰਕੜੇ, &c);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(!ਅੰਕੜੇ->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(ਅੰਕੜੇ, c, TRUE);

  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਖਾਲੀ *ਬਫਰ,
                       size_t buflen, size_t *n)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *c = ਨਿਰਾ;
  ਬਣਤਰ Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  ਨਤੀਜਾ = easy_connection(ਅੰਕੜੇ, &c);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(!ਅੰਕੜੇ->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(ਅੰਕੜੇ, c, TRUE);

  sigpipe_ignore(ਅੰਕੜੇ, &sigpipe_ctx);
  ਨਤੀਜਾ = Curl_conn_send(ਅੰਕੜੇ, FIRSTSOCKET, ਬਫਰ, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  ਜੇ(ਨਤੀਜਾ && ਨਤੀਜਾ != CURLE_AGAIN)
    ਵਾਪਸ CURLE_SEND_ERROR;
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ਅਚਲ ਖਾਲੀ *ਬਫਰ, size_t buflen,
                        size_t *n)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_send, &ਨਤੀਜਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    size_t written = 0;

    ਜੇ(!n || (buflen && !ਬਫਰ)) {
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਤੀਜਾ = Curl_senddata(ਅੰਕੜੇ, ਬਫਰ, buflen, &written);
    *n = written;
  }
ਬਾਹਰਲਾ:
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &ਨਤੀਜਾ)) {
    /* Use the common function to keep connections alive. */
    ਨਤੀਜਾ = Curl_cpool_upkeep((ਬਣਤਰ Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode curl_easy_ssls_import(CURL *curl, ਅਚਲ ਅੱਖਰ *session_ਕੁੰਜੀ,
                               ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *shmac, size_t shmac_len,
                               ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sdata, size_t sdata_len)
{
#ਜੇ defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &ਨਤੀਜਾ)) {
    ਜੇ(!sdata || !sdata_len)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = Curl_ssl_session_import((ਬਣਤਰ Curl_easy *)curl, session_ਕੁੰਜੀ,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)curl;
  (ਖਾਲੀ)session_ਕੁੰਜੀ;
  (ਖਾਲੀ)shmac;
  (ਖਾਲੀ)shmac_len;
  (ਖਾਲੀ)sdata;
  (ਖਾਲੀ)sdata_len;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               ਖਾਲੀ *userptr)
{
#ਜੇ defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &ਨਤੀਜਾ)) {
    ਜੇ(!export_fn)
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = Curl_ssl_session_export((ਬਣਤਰ Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ ਨਤੀਜਾ;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)curl;
  (ਖਾਲੀ)export_fn;
  (ਖਾਲੀ)userptr;
  ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
}

CURLcode Curl_meta_ਸੈੱਟ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                       ਖਾਲੀ *meta_ਅੰਕੜੇ, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_ਅੰਕੜੇ); /* never set to NULL */
  ਜੇ(!Curl_hash_add2(&ਅੰਕੜੇ->meta_hash, ਕੁੰਜੀ, ਲੜੀ_ਲੰਬਾਈ(ਕੁੰਜੀ) + 1,
                     meta_ਅੰਕੜੇ, meta_dtor)) {
    meta_dtor(ਕੁੰਜੀ, ਲੜੀ_ਲੰਬਾਈ(ਕੁੰਜੀ) + 1, meta_ਅੰਕੜੇ);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_meta_remove(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ)
{
  Curl_hash_delete(&ਅੰਕੜੇ->meta_hash, ਕੁੰਜੀ, ਲੜੀ_ਲੰਬਾਈ(ਕੁੰਜੀ) + 1);
}

ਖਾਲੀ *Curl_meta_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ)
{
  ਵਾਪਸ Curl_hash_pick(&ਅੰਕੜੇ->meta_hash, ਕੁੰਜੀ, ਲੜੀ_ਲੰਬਾਈ(ਕੁੰਜੀ) + 1);
}

ਖਾਲੀ Curl_meta_reset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  Curl_hash_clean(&ਅੰਕੜੇ->meta_hash);
}
