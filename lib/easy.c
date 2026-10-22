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

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "api.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "vdns/hostip.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "vquic/vquic.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "getinfo.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "easyif.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "multi_ev.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "connect.h" /* for Curl_getconnectinfo */
#ଅନ୍ତର୍ଭାଵ "slist.h"
#ଅନ୍ତର୍ଭାଵ "mime.h"
#ଅନ୍ତର୍ଭାଵ "amigaos.h"
#ଅନ୍ତର୍ଭାଵ "macos.h"
#ଅନ୍ତର୍ଭାଵ "curlx/wait.h"
#ଅନ୍ତର୍ଭାଵ "sigpipe.h"
#ଅନ୍ତର୍ଭାଵ "vssh/ssh.h"
#ଅନ୍ତର୍ଭାଵ "setopt.h"
#ଅନ୍ତର୍ଭାଵ "http_digest.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "altsvc.h"
#ଅନ୍ତର୍ଭାଵ "hsts.h"
#ଅନ୍ତର୍ଭାଵ "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#ଅନ୍ତର୍ଭାଵ "curlx/timeval.h" /* for curlx_now_init() */
#ଅନ୍ତର୍ଭାଵ "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#ଅନ୍ତର୍ଭାଵ "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା initialized;
#ifdef USE_WINSOCK
ସ୍ଥିର bool winsock_initialized;
#endif

#ifdef GLOBAL_INIT_IS_THREADSAFE

ସ୍ଥିର curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#define global_init_lock()   curl_simple_lock_lock(&s_lock)
#define global_init_unlock() curl_simple_lock_unlock(&s_lock)

#ଅନ୍ୟଥା

#define global_init_lock()
#define global_init_unlock()

#endif

#ଯଦି defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ଠେଲ)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)malloc;
curl_free_callback Curl_cfree = (curl_free_callback)ମୁକ୍ତ;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_ତଳ;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#ଯଦି defined(_MSC_VER) && defined(_DLL)
#  pragma warning(କାଢ଼)
#endif

#ifdef DEBUGBUILD
ସ୍ଥିର ଅକ୍ଷର *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
ସ୍ଥିର CURLcode global_init(long flags, bool memoryfuncs)
{
  (ଶୂନ୍ୟ)flags;

  ଯଦି(initialized++)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)ମୁକ୍ତ;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_ତଳ;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  ଯଦି(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    ଯାଅ fail;
  }

#ifdef USE_WINDOWS_SSPI
  ଯଦି(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    ଯାଅ fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  ଯଦି(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    ଯଦି(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      ଯାଅ fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  ଯଦି(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    ଯାଅ fail;
  }

  ଯଦି(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    ଯାଅ fail;
  }

  ଯଦି(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    ଯାଅ fail;
  }

  ଯଦି(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    ଯାଅ fail;
  }

  ଯଦି(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    ଯାଅ fail;
  }

  ଯଦି(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    ଯାଅ fail;
  }

  ଯଦି(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    ଯାଅ fail;
  }

#ifdef DEBUGBUILD
  ଯଦି(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  ଫେରନ୍ତୁ CURLE_OK;

fail:
#ifdef USE_WINSOCK
  ଯଦି(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  ଫେରନ୍ତୁ CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(long flags)
{
  CURLcode result;
  global_init_lock();

  result = global_init(flags, TRUE);

  global_init_unlock();

  ଫେରନ୍ତୁ result;
}

/*
 * curl_global_init_mem() globally initializes curl and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(long flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode result;

  /* Invalid input, return immediately */
  ଯଦି(!m || !f || !r || !s || !c)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  global_init_lock();

  ଯଦି(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /* set memory functions before global_init() in case it wants memory
     functions */
  Curl_cmalloc = m;
  Curl_cfree = f;
  Curl_cstrdup = s;
  Curl_crealloc = r;
  Curl_ccalloc = c;

  /* Call the actual init function, but without setting */
  result = global_init(flags, FALSE);

  global_init_unlock();

  ଫେରନ୍ତୁ result;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
ଶୂନ୍ୟ curl_global_cleanup(ଶୂନ୍ୟ)
{
  global_init_lock();

  ଯଦି(!initialized) {
    global_init_unlock();
    ଫେରନ୍ତୁ;
  }

  ଯଦି(--initialized) {
    global_init_unlock();
    ଫେରନ୍ତୁ;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ifdef USE_WINSOCK
  ଯଦି(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif

#ifdef USE_WINDOWS_SSPI
  Curl_sspi_global_cleanup();
#endif

#ifdef DEBUGBUILD
  curlx_free(leakpointer);
#endif

  global_init_unlock();
}

/**
 * curl_global_trace() globally initializes curl logging.
 */
CURLcode curl_global_trace(ଅଚଳ ଅକ୍ଷର *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode result;
  global_init_lock();

  result = Curl_trc_opt(config);

  global_init_unlock();

  ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)config;
  ଫେରନ୍ତୁ CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ଅଚଳ ଅକ୍ଷର *name,
                              ଅଚଳ curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  ଫେରନ୍ତୁ rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(ଶୂନ୍ୟ)
{
  CURLcode result;
  ସଂରଚନା Curl_easy *data;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  ଯଦି(!initialized) {
    result = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    ଯଦି(result) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      ଫେରନ୍ତୁ ଖାଲି;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  result = Curl_open(&data);
  ଯଦି(result) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    ଫେରନ୍ତୁ ଖାଲି;
  }

  ଫେରନ୍ତୁ data;
}

#ifdef DEBUGBUILD

ସଂରଚନା socketmonitor {
  ସଂରଚନା socketmonitor *next; /* the next node in the list or NULL */
  ସଂରଚନା pollfd socket; /* socket info of what to monitor */
};

ସଂରଚନା events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_sockets;      /* number of nodes in the monitor list */
  ସଂରଚନା socketmonitor *list; /* list of sockets to monitor */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        ଶୂନ୍ୟ *userp)     /* private callback pointer */
{
  ସଂରଚନା events *ev = userp;
  (ଶୂନ୍ୟ)multi;
#ଯଦି DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  ଫେରନ୍ତୁ 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା poll2cselect(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pollmask)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା omask = 0;
  ଯଦି(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  ଯଦି(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  ଯଦି(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  ଫେରନ୍ତୁ omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
ସ୍ଥିର short socketcb2poll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pollmask)
{
  short omask = 0;
  ଯଦି(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  ଯଦି(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  ଫେରନ୍ତୁ omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା what,        /* see above */
                         ଶୂନ୍ୟ *userp,     /* private callback
                                             pointer */
                         ଶୂନ୍ୟ *socketp)   /* private socket
                                             pointer */
{
  ସଂରଚନା events *ev = userp;
  ସଂରଚନା socketmonitor *m;
  ସଂରଚନା socketmonitor *prev = ଖାଲି;
  bool found = FALSE;
  ସଂରଚନା Curl_easy *data = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (ଶୂନ୍ୟ)easy;
#endif
  (ଶୂନ୍ୟ)socketp;

  m = ev->list;
  ଯେତେବେଳେ(m) {
    ଯଦି(m->socket.fd == s) {
      found = TRUE;
      ଯଦି(what == CURL_POLL_REMOVE) {
        ସଂରଚନା socketmonitor *nxt = m->next;
        /* remove this node from the list of monitored sockets */
        ଯଦି(prev)
          prev->next = nxt;
        ଅନ୍ୟଥା
          ev->list = nxt;
        curlx_free(m);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      ଅନ୍ୟଥା {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      ଭାଙ୍ଗ;
    }
    prev = m;
    m = m->next; /* move to next node */
  }

  ଯଦି(!found) {
    ଯଦି(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(data, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    ଅନ୍ୟଥା {
      m = curlx_malloc(ଆକାର(ସଂରଚନା socketmonitor));
      ଯଦି(m) {
        m->next = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(data, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
  }

  ଫେରନ୍ତୁ 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
ସ୍ଥିର ଶୂନ୍ୟ events_setup(ସଂରଚନା Curl_multi *multi, ସଂରଚନା events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

ସ୍ଥିର CURLcode populate_cpfds(ସଂରଚନା curl_pollfds *cpfds, ସଂରଚନା events *ev)
{
  ସଂରଚନା socketmonitor *m;
  CURLcode result = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  ପାଇଁ(m = ev->list; m && !result; m = m->next) {
    result = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#ଯଦି DEBUG_EV_POLL
    ଯଦି(!result)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode poll_cpfds(ସଂରଚନା events *ev,
                           ସଂରଚନା curl_pollfds *cpfds,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pollrc)
{
  ଯଦି(cpfds->n) {
    /* wait for activity or timeout */
#ଯଦି DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#endif
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#ଯଦି DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#endif
    ଯଦି(*pollrc < 0)
      ଫେରନ୍ତୁ CURLE_UNRECOVERABLE_POLL;
  }
  ଅନ୍ୟଥା {
#ଯଦି DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    ଯଦି(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
ସ୍ଥିର CURLcode wait_or_timeout(ସଂରଚନା Curl_multi *multi, ସଂରଚନା events *ev)
{
  ସଂରଚନା pollfd fds_on_ଗଦା[8];
  ସଂରଚନା curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_ଗଦା, CURL_ARRAYSIZE(fds_on_ଗଦା));
  ଯେତେବେଳେ(!done) {
    CURLMsg *ସନ୍ଦେଶ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pollrc;
    ସଂରଚନା curltime start;

    result = populate_cpfds(&cpfds, ev);
    ଯଦି(result)
      ଯାଅ out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    result = poll_cpfds(ev, &cpfds, &pollrc);
    ଯଦି(result)
      ଯାଅ out;

    ev->msbump = FALSE; /* reset here */

    ଯଦି(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#ଯଦି DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#endif
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    ଅନ୍ୟଥା {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
      ପାଇଁ(i = 0; i < cpfds.n; i++) {
        ଯଦି(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          ପୂର୍ଣ୍ଣସଂଖ୍ୟା act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      ଯଦି(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        ଯଦି(spent_ms > 0) {
#ଯଦି DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (long)spent_ms);
#endif
          ଯଦି(spent_ms > ev->ms)
            ev->ms = 0;
          ଅନ୍ୟଥା
            ev->ms -= (long)spent_ms;
        }
      }
    }

    ଯଦି(mresult) {
      result = CURLE_URL_MALFORMAT;
      ଯାଅ out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    ସନ୍ଦେଶ = curl_multi_info_read(multi, &pollrc);
    ଯଦି(ସନ୍ଦେଶ) {
      result = ସନ୍ଦେଶ->data.result;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  ଫେରନ୍ତୁ result;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
ସ୍ଥିର CURLcode easy_events(ସଂରଚନା Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  ସ୍ଥିର ସଂରଚନା events evs = { -1, FALSE, 0, ଖାଲି, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  ଫେରନ୍ତୁ wait_or_timeout(multi, &evs);
}
#ଅନ୍ୟଥା /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

ସ୍ଥିର CURLcode easy_transfer(ସଂରଚନା Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  ଯେତେବେଳେ(!done && !mresult) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା still_running = 0;

    mresult = curl_multi_poll(multi, ଖାଲି, 0, 1000, ଖାଲି);

    ଯଦି(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    ଯଦି(!mresult && !still_running) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
      CURLMsg *ସନ୍ଦେଶ = curl_multi_info_read(multi, &rc);
      ଯଦି(ସନ୍ଦେଶ) {
        result = ସନ୍ଦେଶ->data.result;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  ଯଦି(mresult) {
    result = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode easy_perform(ସଂରଚନା Curl_easy *data, bool events)
{
  ସଂରଚନା Curl_multi *multi;
  CURLMcode mresult;
  CURLcode result = CURLE_OK;
  ସଂରଚନା Curl_sigpipe_ctx sigpipe_ctx;

  ଯଦି(!data)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(data->set.errorbuffer)
    /* clear this as early as possible */
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  ଯଦି(data->multi) {
    failf(data, "easy handle already used in multi handle");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  ଯଦି(data->conn) {
    ସଂରଚନା connectdata *conn = data->conn;
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
    DEBUGASSERT(!data->conn);
  }

  ଯଦି(data->multi_easy)
    multi = data->multi_easy;
  ଅନ୍ୟଥା {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    ଯଦି(!multi)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଯଦି(Curl_api_multi_is_in_callback(multi))
    ଫେରନ୍ତୁ CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)data->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)data->set.quick_exit);

  data->multi_easy = ଖାଲି; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, data);
  ଯଦି(mresult) {
    curl_multi_cleanup(multi);
    ଯଦି(mresult == CURLM_OUT_OF_MEMORY)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  data->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(data, &sigpipe_ctx);

  /* run the transfer */
  result = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (ଶୂନ୍ୟ)Curl_multi_remove_handle(multi, data);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  ଫେରନ୍ତୁ result;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  ସଂରଚନା Curl_eapi_guard guard = { 0 };
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_perform, &result)) {
    result = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(ସଂରଚନା Curl_easy *easy)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &result)) {
    result = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
ଶୂନ୍ୟ curl_easy_cleanup(CURL *curl)
{
  ସଂରଚନା Curl_eapi_guard guard;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, ଖାଲି)) {
    ସଂରଚନା Curl_easy *data = curl;
    ସଂରଚନା Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(data, &sigpipe_ctx);
    Curl_close(&data);
    sigpipe_restore(&sigpipe_ctx);
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#undef curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &result)) {
    ସଂରଚନା Curl_easy *data = curl;
    va_list arg;
    ଶୂନ୍ୟ *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, ଶୂନ୍ୟ *);

    result = Curl_getinfo(data, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode dupset(ସଂରଚନା Curl_easy *dst, ସଂରଚନା Curl_easy *src)
{
  CURLcode result = CURLE_OK;
  ଗଣନା dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#ଯଦି !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = ଖାଲି;
#endif
  dst->set.str_copypostfields = ଖାଲି;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * ଆକାର(ସଂରଚନା curl_blob *));

  /* duplicate all strings */
  result = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* duplicate all blobs */
  ପାଇଁ(j = (ଗଣନା dupblob)0; j < BLOB_LAST; j++) {
    result = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  /* duplicate memory areas pointed to */
  ଯଦି(src->set.str_copypostfields) {
    ଯଦି(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    ଅନ୍ୟଥା
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    ଯଦି(!dst->set.str_copypostfields)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#ଯଦି !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ଯଦି(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(ଆକାର(*dst->set.mimepostp));
    ଯଦି(!dst->set.mimepostp)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    result = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
#endif

  ଯଦି(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ dupeasy_meta_freeentry(ଶୂନ୍ୟ *p)
{
  (ଶୂନ୍ୟ)p;
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
  ସଂରଚନା Curl_eapi_guard guard;
  ସଂରଚନା Curl_easy *outcurl = ଖାଲି;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, ଖାଲି)) {
    ସଂରଚନା Curl_easy *data = curl;

    outcurl = curlx_calloc(1, ଆକାର(ସଂରଚନା Curl_easy));
    ଯଦି(!outcurl)
      ଯାଅ fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->set.buffer_size = data->set.buffer_size;

    Curl_hash_init(&outcurl->meta_hash, 23, CURL_HASH_TYPE_BYTES,
                   dupeasy_meta_freeentry);
    curlx_dyn_init(&outcurl->state.headerb, CURL_MAX_HTTP_HEADER);
    Curl_bufref_init(&outcurl->state.url);
    Curl_bufref_init(&outcurl->state.referer);
    Curl_netrc_init(&outcurl->state.netrc);

    /* the connection pool is setup on demand */
    outcurl->state.last_conn_id = -1;
    outcurl->state.last_cpid = UINT32_MAX;
    outcurl->id = -1;
    outcurl->mid = UINT32_MAX;
    outcurl->master_mid = UINT32_MAX;

#ifndef CURL_DISABLE_HTTP
    Curl_llist_init(&outcurl->state.httphdrs, ଖାଲି);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    ଯଦି(dupset(outcurl, data))
      ଯାଅ fail;

    outcurl->progress.hide     = data->progress.hide;
    outcurl->progress.callback = data->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = ଖାଲି;
    ଯଦି(data->cookies && data->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      ଯଦି(!outcurl->cookies)
        ଯାଅ fail;
      outcurl->state.cookie_engine = TRUE;
    }

    ଯଦି(data->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(data->state.cookielist);
      ଯଦି(!outcurl->state.cookielist)
        ଯାଅ fail;
    }
#endif

    ଯଦି(Curl_bufref_ptr(&data->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&data->state.url), 0,
                      curl_free);
      ଯଦି(!Curl_bufref_ptr(&outcurl->state.url))
        ଯାଅ fail;
    }
    ଯଦି(Curl_bufref_ptr(&data->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&data->state.referer), 0,
                      curl_free);
      ଯଦି(!Curl_bufref_ptr(&outcurl->state.referer))
        ଯାଅ fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    ଯଦି(data->asi) {
      ଅଚଳ ଅକ୍ଷର *str;
      outcurl->asi = Curl_altsvc_init();
      ଯଦି(!outcurl->asi)
        ଯାଅ fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      ଯଦି(str)
        (ଶୂନ୍ୟ)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    ଯଦି(data->hsts) {
      ଅଚଳ ଅକ୍ଷର *str;
      outcurl->hsts = Curl_hsts_init();
      ଯଦି(!outcurl->hsts)
        ଯାଅ fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      ଯଦି(str)
        (ଶୂନ୍ୟ)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (ଶୂନ୍ୟ)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      ଯଦି(Curl_hsts_copy(outcurl->hsts, data->hsts))
        ଯାଅ fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_ସଂଖ୍ୟା;
  }
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ outcurl;

fail:
  ଯଦି(outcurl) {
#ifndef CURL_DISABLE_COOKIES
    curlx_free(outcurl->cookies);
#endif
    curlx_dyn_free(&outcurl->state.headerb);
    Curl_altsvc_cleanup(&outcurl->asi);
    Curl_hsts_cleanup(&outcurl->hsts);
    Curl_freeset(outcurl);
    curlx_free(outcurl);
  }

  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
ଶୂନ୍ୟ curl_easy_reset(CURL *curl)
{
  ସଂରଚନା Curl_eapi_guard guard;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_reset, ଖାଲି)) {
    ସଂରଚନା Curl_easy *data = curl;

    data->state.last_conn_id = -1; /* clear remembered connection id */
    data->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&data->req, data);
    Curl_hash_clean(&data->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(data);
    /* zero out UserDefined data: */
    Curl_freeset(data);
    memset(&data->set, 0, ଆକାର(ସଂରଚନା UserDefined));
    Curl_init_userdefined(data);

    /* zero out Progress data: */
    memset(&data->progress, 0, ଆକାର(ସଂରଚନା Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(data);

    data->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&data->state.authhost, 0, ଆକାର(ସଂରଚନା auth));
    memset(&data->state.authproxy, 0, ଆକାର(ସଂରଚନା auth));

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(data);
#endif
    data->master_mid = UINT32_MAX;
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
CURLcode curl_easy_pause(CURL *curl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା action)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_pause, &result)) {
    bool changed = FALSE;
    ସଂରଚନା Curl_easy *data = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    ଯଦି(!data->conn) {
      /* crazy input, do not continue */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ଯାଅ out;
    }

    recv_paused = Curl_xfer_recv_is_paused(data);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(data);
    send_paused_new = (action & CURLPAUSE_SEND);

    ଯଦି((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(data))) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_send(data, send_paused_new));
    }

    ଯଦି(recv_paused != recv_paused_new) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_recv(data, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    ଯଦି(!Curl_xfer_is_blocked(data)) {
      /* reset the too-slow time keeper */
      data->state.keeps_speed.tv_sec = 0;
      ଯଦି(data->multi) {
        Curl_multi_mark_dirty(data); /* make it run */
        /* On changes, tell application to update its timers. */
        ଯଦି(changed && Curl_update_timer(data->multi) && !result)
          result = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    ଯଦି(!result && changed && !data->state.done && data->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(data->multi, data))
      result = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode easy_connection(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata **connp)
{
  curl_socket_t sfd;

  ଯଦି(!data)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  ଯଦି(!data->set.connect_only) {
    failf(data, "CONNECT_ONLY is required");
    ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(data, connp);

  ଯଦି(sfd == CURL_SOCKET_BAD) {
    failf(data, "Failed to get last socket used for connection #%" FMT_OFF_T,
          data->state.last_conn_id);
    ଫେରନ୍ତୁ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(ସଂରଚନା Curl_easy *data,
                        ଶୂନ୍ୟ *buffer, size_t buflen, size_t *n)
{
  CURLcode result;
  ସଂରଚନା connectdata *c;

  result = easy_connection(data, &c);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  *n = 0;
  ଫେରନ୍ତୁ Curl_conn_recv(data, FIRSTSOCKET, buffer, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, ଶୂନ୍ୟ *buffer, size_t buflen, size_t *n)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_recv, &result)) {
    ଯଦି(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ଯାଅ out;
    }
    result = Curl_easy_recv(curl, buffer, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(ସଂରଚନା Curl_easy *data)
{
  CURLcode result;
  ସଂରଚନା connectdata *c = ଖାଲି;

  result = easy_connection(data, &c);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(ସଂରଚନା Curl_easy *data, ଅଚଳ ଶୂନ୍ୟ *buffer,
                       size_t buflen, size_t *n)
{
  CURLcode result;
  ସଂରଚନା connectdata *c = ଖାଲି;
  ସଂରଚନା Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  result = easy_connection(data, &c);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  sigpipe_ignore(data, &sigpipe_ctx);
  result = Curl_conn_send(data, FIRSTSOCKET, buffer, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  ଯଦି(result && result != CURLE_AGAIN)
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  ଫେରନ୍ତୁ result;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ଅଚଳ ଶୂନ୍ୟ *buffer, size_t buflen,
                        size_t *n)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_send, &result)) {
    ସଂରଚନା Curl_easy *data = curl;
    size_t written = 0;

    ଯଦି(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ଯାଅ out;
    }
    result = Curl_senddata(data, buffer, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &result)) {
    /* Use the common function to keep connections alive. */
    result = Curl_cpool_upkeep((ସଂରଚନା Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
}

CURLcode curl_easy_ssls_import(CURL *curl, ଅଚଳ ଅକ୍ଷର *session_ଚାବି,
                               ଅଚଳ unsigned ଅକ୍ଷର *shmac, size_t shmac_len,
                               ଅଚଳ unsigned ଅକ୍ଷର *sdata, size_t sdata_len)
{
#ଯଦି defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &result)) {
    ଯଦି(!sdata || !sdata_len)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      result = Curl_ssl_session_import((ସଂରଚନା Curl_easy *)curl, session_ଚାବି,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)curl;
  (ଶୂନ୍ୟ)session_ଚାବି;
  (ଶୂନ୍ୟ)shmac;
  (ଶୂନ୍ୟ)shmac_len;
  (ଶୂନ୍ୟ)sdata;
  (ଶୂନ୍ୟ)sdata_len;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               ଶୂନ୍ୟ *userptr)
{
#ଯଦି defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ସଂରଚନା Curl_eapi_guard guard;
  CURLcode result;

  ଯଦି(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &result)) {
    ଯଦି(!export_fn)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଅନ୍ୟଥା
      result = Curl_ssl_session_export((ସଂରଚନା Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  ଫେରନ୍ତୁ result;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)curl;
  (ଶୂନ୍ୟ)export_fn;
  (ଶୂନ୍ୟ)userptr;
  ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *ଚାବି,
                       ଶୂନ୍ୟ *meta_data, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_data); /* never set to NULL */
  ଯଦି(!Curl_hash_add2(&data->meta_hash, ଚାବି, strlen(ଚାବି) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(ଚାବି, strlen(ଚାବି) + 1, meta_data);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_meta_remove(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *ଚାବି)
{
  Curl_hash_delete(&data->meta_hash, ଚାବି, strlen(ଚାବି) + 1);
}

ଶୂନ୍ୟ *Curl_meta_get(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *ଚାବି)
{
  ଫେରନ୍ତୁ Curl_hash_pick(&data->meta_hash, ଚାବି, strlen(ଚାବି) + 1);
}

ଶୂନ୍ୟ Curl_meta_reset(ସଂରଚନା Curl_easy *data)
{
  Curl_hash_clean(&data->meta_hash);
}
