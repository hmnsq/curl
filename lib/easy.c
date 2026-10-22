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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#അന്തര്ഭാവ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#അന്തര്ഭാവ <sys/param.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "api.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "vdns/hostip.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "vquic/vquic.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "getinfo.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "easyif.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "multi_ev.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "connect.h" /* for Curl_getconnectinfo */
#അന്തര്ഭാവ "slist.h"
#അന്തര്ഭാവ "mime.h"
#അന്തര്ഭാവ "amigaos.h"
#അന്തര്ഭാവ "macos.h"
#അന്തര്ഭാവ "curlx/wait.h"
#അന്തര്ഭാവ "sigpipe.h"
#അന്തര്ഭാവ "vssh/ssh.h"
#അന്തര്ഭാവ "setopt.h"
#അന്തര്ഭാവ "http_digest.h"
#അന്തര്ഭാവ "curlx/dynbuf.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "altsvc.h"
#അന്തര്ഭാവ "hsts.h"
#അന്തര്ഭാവ "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#അന്തര്ഭാവ "curlx/timeval.h" /* for curlx_now_init() */
#അന്തര്ഭാവ "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#അന്തര്ഭാവ "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ initialized;
#ifdef USE_WINSOCK
സ്ഥിതം bool winsock_initialized;
#endif

#ifdef GLOBAL_INIT_IS_THREADSAFE

സ്ഥിതം curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#define global_init_lock()   curl_simple_lock_lock(&s_lock)
#define global_init_unlock() curl_simple_lock_unlock(&s_lock)

#അല്ലെങ്കിൽ

#define global_init_lock()
#define global_init_unlock()

#endif

#എങ്കിൽ defined(_MSC_VER) && defined(_DLL)
#  pragma warning(തള്ളുക)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)malloc;
curl_free_callback Curl_cfree = (curl_free_callback)സ്വതന്ത്രം;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_താഴെയുള്ള;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#എങ്കിൽ defined(_MSC_VER) && defined(_DLL)
#  pragma warning(എടുക്കുക)
#endif

#ifdef DEBUGBUILD
സ്ഥിതം അക്ഷരം *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
സ്ഥിതം CURLcode global_init(long flags, bool memoryfuncs)
{
  (ശൂന്യം)flags;

  എങ്കിൽ(initialized++)
    തിരികെ CURLE_OK;

  എങ്കിൽ(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)സ്വതന്ത്രം;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_താഴെയുള്ള;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  എങ്കിൽ(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    പോകുക fail;
  }

#ifdef USE_WINDOWS_SSPI
  എങ്കിൽ(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    പോകുക fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  എങ്കിൽ(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    എങ്കിൽ(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      പോകുക fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  എങ്കിൽ(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    പോകുക fail;
  }

  എങ്കിൽ(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    പോകുക fail;
  }

  എങ്കിൽ(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    പോകുക fail;
  }

  എങ്കിൽ(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    പോകുക fail;
  }

  എങ്കിൽ(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    പോകുക fail;
  }

  എങ്കിൽ(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    പോകുക fail;
  }

  എങ്കിൽ(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    പോകുക fail;
  }

#ifdef DEBUGBUILD
  എങ്കിൽ(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  തിരികെ CURLE_OK;

fail:
#ifdef USE_WINSOCK
  എങ്കിൽ(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  തിരികെ CURLE_FAILED_INIT;
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

  തിരികെ result;
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
  എങ്കിൽ(!m || !f || !r || !s || !c)
    തിരികെ CURLE_FAILED_INIT;

  global_init_lock();

  എങ്കിൽ(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    തിരികെ CURLE_OK;
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

  തിരികെ result;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
ശൂന്യം curl_global_cleanup(ശൂന്യം)
{
  global_init_lock();

  എങ്കിൽ(!initialized) {
    global_init_unlock();
    തിരികെ;
  }

  എങ്കിൽ(--initialized) {
    global_init_unlock();
    തിരികെ;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ifdef USE_WINSOCK
  എങ്കിൽ(winsock_initialized) {
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
CURLcode curl_global_trace(സ്ഥിരം അക്ഷരം *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode result;
  global_init_lock();

  result = Curl_trc_opt(config);

  global_init_unlock();

  തിരികെ result;
#അല്ലെങ്കിൽ
  (ശൂന്യം)config;
  തിരികെ CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, സ്ഥിരം അക്ഷരം *name,
                              സ്ഥിരം curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  തിരികെ rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(ശൂന്യം)
{
  CURLcode result;
  ഘടന Curl_easy *data;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  എങ്കിൽ(!initialized) {
    result = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    എങ്കിൽ(result) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      തിരികെ ശൂന്യ;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  result = Curl_open(&data);
  എങ്കിൽ(result) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    തിരികെ ശൂന്യ;
  }

  തിരികെ data;
}

#ifdef DEBUGBUILD

ഘടന socketmonitor {
  ഘടന socketmonitor *next; /* the next node in the list or NULL */
  ഘടന pollfd socket; /* socket info of what to monitor */
};

ഘടന events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  പൂർണ്ണസംഖ്യ അക്കം_sockets;      /* number of nodes in the monitor list */
  ഘടന socketmonitor *list; /* list of sockets to monitor */
  പൂർണ്ണസംഖ്യ running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        ശൂന്യം *userp)     /* private callback pointer */
{
  ഘടന events *ev = userp;
  (ശൂന്യം)multi;
#എങ്കിൽ DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  തിരികെ 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ poll2cselect(പൂർണ്ണസംഖ്യ pollmask)
{
  പൂർണ്ണസംഖ്യ omask = 0;
  എങ്കിൽ(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  എങ്കിൽ(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  എങ്കിൽ(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  തിരികെ omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
സ്ഥിതം short socketcb2poll(പൂർണ്ണസംഖ്യ pollmask)
{
  short omask = 0;
  എങ്കിൽ(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  എങ്കിൽ(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  തിരികെ omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         പൂർണ്ണസംഖ്യ what,        /* see above */
                         ശൂന്യം *userp,     /* private callback
                                             pointer */
                         ശൂന്യം *socketp)   /* private socket
                                             pointer */
{
  ഘടന events *ev = userp;
  ഘടന socketmonitor *m;
  ഘടന socketmonitor *prev = ശൂന്യ;
  bool found = FALSE;
  ഘടന Curl_easy *data = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (ശൂന്യം)easy;
#endif
  (ശൂന്യം)socketp;

  m = ev->list;
  ആകുമ്പോൾ(m) {
    എങ്കിൽ(m->socket.fd == s) {
      found = TRUE;
      എങ്കിൽ(what == CURL_POLL_REMOVE) {
        ഘടന socketmonitor *nxt = m->next;
        /* remove this node from the list of monitored sockets */
        എങ്കിൽ(prev)
          prev->next = nxt;
        അല്ലെങ്കിൽ
          ev->list = nxt;
        curlx_free(m);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      അല്ലെങ്കിൽ {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      നിർത്തുക;
    }
    prev = m;
    m = m->next; /* move to next node */
  }

  എങ്കിൽ(!found) {
    എങ്കിൽ(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(data, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    അല്ലെങ്കിൽ {
      m = curlx_malloc(വലുപ്പം(ഘടന socketmonitor));
      എങ്കിൽ(m) {
        m->next = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(data, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      അല്ലെങ്കിൽ
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
  }

  തിരികെ 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
സ്ഥിതം ശൂന്യം events_setup(ഘടന Curl_multi *multi, ഘടന events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

സ്ഥിതം CURLcode populate_cpfds(ഘടന curl_pollfds *cpfds, ഘടന events *ev)
{
  ഘടന socketmonitor *m;
  CURLcode result = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  വേണ്ടി(m = ev->list; m && !result; m = m->next) {
    result = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#എങ്കിൽ DEBUG_EV_POLL
    എങ്കിൽ(!result)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode poll_cpfds(ഘടന events *ev,
                           ഘടന curl_pollfds *cpfds,
                           പൂർണ്ണസംഖ്യ *pollrc)
{
  എങ്കിൽ(cpfds->n) {
    /* wait for activity or timeout */
#എങ്കിൽ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#endif
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#എങ്കിൽ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#endif
    എങ്കിൽ(*pollrc < 0)
      തിരികെ CURLE_UNRECOVERABLE_POLL;
  }
  അല്ലെങ്കിൽ {
#എങ്കിൽ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    എങ്കിൽ(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  തിരികെ CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
സ്ഥിതം CURLcode wait_or_timeout(ഘടന Curl_multi *multi, ഘടന events *ev)
{
  ഘടന pollfd fds_on_അട്ടി[8];
  ഘടന curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_അട്ടി, CURL_ARRAYSIZE(fds_on_അട്ടി));
  ആകുമ്പോൾ(!done) {
    CURLMsg *സന്ദേശം;
    പൂർണ്ണസംഖ്യ pollrc;
    ഘടന curltime start;

    result = populate_cpfds(&cpfds, ev);
    എങ്കിൽ(result)
      പോകുക out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    result = poll_cpfds(ev, &cpfds, &pollrc);
    എങ്കിൽ(result)
      പോകുക out;

    ev->msbump = FALSE; /* reset here */

    എങ്കിൽ(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#എങ്കിൽ DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#endif
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    അല്ലെങ്കിൽ {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      unsigned പൂർണ്ണസംഖ്യ i;
      വേണ്ടി(i = 0; i < cpfds.n; i++) {
        എങ്കിൽ(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          പൂർണ്ണസംഖ്യ act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      എങ്കിൽ(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        എങ്കിൽ(spent_ms > 0) {
#എങ്കിൽ DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (long)spent_ms);
#endif
          എങ്കിൽ(spent_ms > ev->ms)
            ev->ms = 0;
          അല്ലെങ്കിൽ
            ev->ms -= (long)spent_ms;
        }
      }
    }

    എങ്കിൽ(mresult) {
      result = CURLE_URL_MALFORMAT;
      പോകുക out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    സന്ദേശം = curl_multi_info_read(multi, &pollrc);
    എങ്കിൽ(സന്ദേശം) {
      result = സന്ദേശം->data.result;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  തിരികെ result;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
സ്ഥിതം CURLcode easy_events(ഘടന Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  സ്ഥിതം ഘടന events evs = { -1, FALSE, 0, ശൂന്യ, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  തിരികെ wait_or_timeout(multi, &evs);
}
#അല്ലെങ്കിൽ /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

സ്ഥിതം CURLcode easy_transfer(ഘടന Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  ആകുമ്പോൾ(!done && !mresult) {
    പൂർണ്ണസംഖ്യ still_running = 0;

    mresult = curl_multi_poll(multi, ശൂന്യ, 0, 1000, ശൂന്യ);

    എങ്കിൽ(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    എങ്കിൽ(!mresult && !still_running) {
      പൂർണ്ണസംഖ്യ rc;
      CURLMsg *സന്ദേശം = curl_multi_info_read(multi, &rc);
      എങ്കിൽ(സന്ദേശം) {
        result = സന്ദേശം->data.result;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  എങ്കിൽ(mresult) {
    result = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  തിരികെ result;
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
സ്ഥിതം CURLcode easy_perform(ഘടന Curl_easy *data, bool events)
{
  ഘടന Curl_multi *multi;
  CURLMcode mresult;
  CURLcode result = CURLE_OK;
  ഘടന Curl_sigpipe_ctx sigpipe_ctx;

  എങ്കിൽ(!data)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(data->set.errorbuffer)
    /* clear this as early as possible */
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  എങ്കിൽ(data->multi) {
    failf(data, "easy handle already used in multi handle");
    തിരികെ CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  എങ്കിൽ(data->conn) {
    ഘടന connectdata *conn = data->conn;
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
    DEBUGASSERT(!data->conn);
  }

  എങ്കിൽ(data->multi_easy)
    multi = data->multi_easy;
  അല്ലെങ്കിൽ {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    എങ്കിൽ(!multi)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(Curl_api_multi_is_in_callback(multi))
    തിരികെ CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)data->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)data->set.quick_exit);

  data->multi_easy = ശൂന്യ; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, data);
  എങ്കിൽ(mresult) {
    curl_multi_cleanup(multi);
    എങ്കിൽ(mresult == CURLM_OUT_OF_MEMORY)
      തിരികെ CURLE_OUT_OF_MEMORY;
    തിരികെ CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  data->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(data, &sigpipe_ctx);

  /* run the transfer */
  result = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (ശൂന്യം)Curl_multi_remove_handle(multi, data);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  തിരികെ result;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  ഘടന Curl_eapi_guard guard = { 0 };
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_perform, &result)) {
    result = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(ഘടന Curl_easy *easy)
{
  ഘടന Curl_eapi_guard guard;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &result)) {
    result = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
ശൂന്യം curl_easy_cleanup(CURL *curl)
{
  ഘടന Curl_eapi_guard guard;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, ശൂന്യ)) {
    ഘടന Curl_easy *data = curl;
    ഘടന Curl_sigpipe_ctx sigpipe_ctx;
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
  ഘടന Curl_eapi_guard guard;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &result)) {
    ഘടന Curl_easy *data = curl;
    va_list arg;
    ശൂന്യം *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, ശൂന്യം *);

    result = Curl_getinfo(data, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
}

സ്ഥിതം CURLcode dupset(ഘടന Curl_easy *dst, ഘടന Curl_easy *src)
{
  CURLcode result = CURLE_OK;
  എണ്ണൽ dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#എങ്കിൽ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = ശൂന്യ;
#endif
  dst->set.str_copypostfields = ശൂന്യ;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * വലുപ്പം(ഘടന curl_blob *));

  /* duplicate all strings */
  result = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  എങ്കിൽ(result)
    തിരികെ result;

  /* duplicate all blobs */
  വേണ്ടി(j = (എണ്ണൽ dupblob)0; j < BLOB_LAST; j++) {
    result = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  /* duplicate memory areas pointed to */
  എങ്കിൽ(src->set.str_copypostfields) {
    എങ്കിൽ(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    അല്ലെങ്കിൽ
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    എങ്കിൽ(!dst->set.str_copypostfields)
      തിരികെ CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#എങ്കിൽ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  എങ്കിൽ(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(വലുപ്പം(*dst->set.mimepostp));
    എങ്കിൽ(!dst->set.mimepostp)
      തിരികെ CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    result = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    എങ്കിൽ(result)
      തിരികെ result;
  }
#endif

  എങ്കിൽ(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  തിരികെ result;
}

സ്ഥിതം ശൂന്യം dupeasy_meta_freeentry(ശൂന്യം *p)
{
  (ശൂന്യം)p;
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
  ഘടന Curl_eapi_guard guard;
  ഘടന Curl_easy *outcurl = ശൂന്യ;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, ശൂന്യ)) {
    ഘടന Curl_easy *data = curl;

    outcurl = curlx_calloc(1, വലുപ്പം(ഘടന Curl_easy));
    എങ്കിൽ(!outcurl)
      പോകുക fail;

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
    Curl_llist_init(&outcurl->state.httphdrs, ശൂന്യ);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    എങ്കിൽ(dupset(outcurl, data))
      പോകുക fail;

    outcurl->progress.hide     = data->progress.hide;
    outcurl->progress.callback = data->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = ശൂന്യ;
    എങ്കിൽ(data->cookies && data->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      എങ്കിൽ(!outcurl->cookies)
        പോകുക fail;
      outcurl->state.cookie_engine = TRUE;
    }

    എങ്കിൽ(data->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(data->state.cookielist);
      എങ്കിൽ(!outcurl->state.cookielist)
        പോകുക fail;
    }
#endif

    എങ്കിൽ(Curl_bufref_ptr(&data->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&data->state.url), 0,
                      curl_free);
      എങ്കിൽ(!Curl_bufref_ptr(&outcurl->state.url))
        പോകുക fail;
    }
    എങ്കിൽ(Curl_bufref_ptr(&data->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&data->state.referer), 0,
                      curl_free);
      എങ്കിൽ(!Curl_bufref_ptr(&outcurl->state.referer))
        പോകുക fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    എങ്കിൽ(data->asi) {
      സ്ഥിരം അക്ഷരം *str;
      outcurl->asi = Curl_altsvc_init();
      എങ്കിൽ(!outcurl->asi)
        പോകുക fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      എങ്കിൽ(str)
        (ശൂന്യം)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    എങ്കിൽ(data->hsts) {
      സ്ഥിരം അക്ഷരം *str;
      outcurl->hsts = Curl_hsts_init();
      എങ്കിൽ(!outcurl->hsts)
        പോകുക fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      എങ്കിൽ(str)
        (ശൂന്യം)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (ശൂന്യം)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      എങ്കിൽ(Curl_hsts_copy(outcurl->hsts, data->hsts))
        പോകുക fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_സംഖ്യ;
  }
  CURL_EAPI_LEAVE(&guard);
  തിരികെ outcurl;

fail:
  എങ്കിൽ(outcurl) {
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
  തിരികെ ശൂന്യ;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
ശൂന്യം curl_easy_reset(CURL *curl)
{
  ഘടന Curl_eapi_guard guard;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_reset, ശൂന്യ)) {
    ഘടന Curl_easy *data = curl;

    data->state.last_conn_id = -1; /* clear remembered connection id */
    data->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&data->req, data);
    Curl_hash_clean(&data->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(data);
    /* zero out UserDefined data: */
    Curl_freeset(data);
    memset(&data->set, 0, വലുപ്പം(ഘടന UserDefined));
    Curl_init_userdefined(data);

    /* zero out Progress data: */
    memset(&data->progress, 0, വലുപ്പം(ഘടന Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(data);

    data->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&data->state.authhost, 0, വലുപ്പം(ഘടന auth));
    memset(&data->state.authproxy, 0, വലുപ്പം(ഘടന auth));

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
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
CURLcode curl_easy_pause(CURL *curl, പൂർണ്ണസംഖ്യ action)
{
  ഘടന Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_pause, &result)) {
    bool changed = FALSE;
    ഘടന Curl_easy *data = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    എങ്കിൽ(!data->conn) {
      /* crazy input, do not continue */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      പോകുക out;
    }

    recv_paused = Curl_xfer_recv_is_paused(data);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(data);
    send_paused_new = (action & CURLPAUSE_SEND);

    എങ്കിൽ((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(data))) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_send(data, send_paused_new));
    }

    എങ്കിൽ(recv_paused != recv_paused_new) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_recv(data, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    എങ്കിൽ(!Curl_xfer_is_blocked(data)) {
      /* reset the too-slow time keeper */
      data->state.keeps_speed.tv_sec = 0;
      എങ്കിൽ(data->multi) {
        Curl_multi_mark_dirty(data); /* make it run */
        /* On changes, tell application to update its timers. */
        എങ്കിൽ(changed && Curl_update_timer(data->multi) && !result)
          result = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    എങ്കിൽ(!result && changed && !data->state.done && data->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(data->multi, data))
      result = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
}

സ്ഥിതം CURLcode easy_connection(ഘടന Curl_easy *data,
                                ഘടന connectdata **connp)
{
  curl_socket_t sfd;

  എങ്കിൽ(!data)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  എങ്കിൽ(!data->set.connect_only) {
    failf(data, "CONNECT_ONLY is required");
    തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(data, connp);

  എങ്കിൽ(sfd == CURL_SOCKET_BAD) {
    failf(data, "Failed to get last socket used for connection #%" FMT_OFF_T,
          data->state.last_conn_id);
    തിരികെ CURLE_UNSUPPORTED_PROTOCOL;
  }

  തിരികെ CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(ഘടന Curl_easy *data,
                        ശൂന്യം *buffer, size_t buflen, size_t *n)
{
  CURLcode result;
  ഘടന connectdata *c;

  result = easy_connection(data, &c);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  *n = 0;
  തിരികെ Curl_conn_recv(data, FIRSTSOCKET, buffer, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, ശൂന്യം *buffer, size_t buflen, size_t *n)
{
  ഘടന Curl_eapi_guard guard;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_recv, &result)) {
    എങ്കിൽ(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      പോകുക out;
    }
    result = Curl_easy_recv(curl, buffer, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(ഘടന Curl_easy *data)
{
  CURLcode result;
  ഘടന connectdata *c = ശൂന്യ;

  result = easy_connection(data, &c);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  തിരികെ CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(ഘടന Curl_easy *data, സ്ഥിരം ശൂന്യം *buffer,
                       size_t buflen, size_t *n)
{
  CURLcode result;
  ഘടന connectdata *c = ശൂന്യ;
  ഘടന Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  result = easy_connection(data, &c);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  sigpipe_ignore(data, &sigpipe_ctx);
  result = Curl_conn_send(data, FIRSTSOCKET, buffer, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  എങ്കിൽ(result && result != CURLE_AGAIN)
    തിരികെ CURLE_SEND_ERROR;
  തിരികെ result;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, സ്ഥിരം ശൂന്യം *buffer, size_t buflen,
                        size_t *n)
{
  ഘടന Curl_eapi_guard guard;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_send, &result)) {
    ഘടന Curl_easy *data = curl;
    size_t written = 0;

    എങ്കിൽ(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      പോകുക out;
    }
    result = Curl_senddata(data, buffer, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  ഘടന Curl_eapi_guard guard;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &result)) {
    /* Use the common function to keep connections alive. */
    result = Curl_cpool_upkeep((ഘടന Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
}

CURLcode curl_easy_ssls_import(CURL *curl, സ്ഥിരം അക്ഷരം *session_താക്കോൽ,
                               സ്ഥിരം unsigned അക്ഷരം *shmac, size_t shmac_len,
                               സ്ഥിരം unsigned അക്ഷരം *sdata, size_t sdata_len)
{
#എങ്കിൽ defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ഘടന Curl_eapi_guard guard;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &result)) {
    എങ്കിൽ(!sdata || !sdata_len)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      result = Curl_ssl_session_import((ഘടന Curl_easy *)curl, session_താക്കോൽ,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
#അല്ലെങ്കിൽ
  (ശൂന്യം)curl;
  (ശൂന്യം)session_താക്കോൽ;
  (ശൂന്യം)shmac;
  (ശൂന്യം)shmac_len;
  (ശൂന്യം)sdata;
  (ശൂന്യം)sdata_len;
  തിരികെ CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               ശൂന്യം *userptr)
{
#എങ്കിൽ defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ഘടന Curl_eapi_guard guard;
  CURLcode result;

  എങ്കിൽ(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &result)) {
    എങ്കിൽ(!export_fn)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    അല്ലെങ്കിൽ
      result = Curl_ssl_session_export((ഘടന Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  തിരികെ result;
#അല്ലെങ്കിൽ
  (ശൂന്യം)curl;
  (ശൂന്യം)export_fn;
  (ശൂന്യം)userptr;
  തിരികെ CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *താക്കോൽ,
                       ശൂന്യം *meta_data, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_data); /* never set to NULL */
  എങ്കിൽ(!Curl_hash_add2(&data->meta_hash, താക്കോൽ, strlen(താക്കോൽ) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(താക്കോൽ, strlen(താക്കോൽ) + 1, meta_data);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_meta_remove(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *താക്കോൽ)
{
  Curl_hash_delete(&data->meta_hash, താക്കോൽ, strlen(താക്കോൽ) + 1);
}

ശൂന്യം *Curl_meta_get(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *താക്കോൽ)
{
  തിരികെ Curl_hash_pick(&data->meta_hash, താക്കോൽ, strlen(താക്കോൽ) + 1);
}

ശൂന്യം Curl_meta_reset(ഘടന Curl_easy *data)
{
  Curl_hash_clean(&data->meta_hash);
}
