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

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "api.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "vdns/hostip.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "vquic/vquic.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "getinfo.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "easyif.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "multi_ev.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "connect.h" /* for Curl_getconnectinfo */
#అన్తర్భావ "slist.h"
#అన్తర్భావ "mime.h"
#అన్తర్భావ "amigaos.h"
#అన్తర్భావ "macos.h"
#అన్తర్భావ "curlx/wait.h"
#అన్తర్భావ "sigpipe.h"
#అన్తర్భావ "vssh/ssh.h"
#అన్తర్భావ "setopt.h"
#అన్తర్భావ "http_digest.h"
#అన్తర్భావ "curlx/dynbuf.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "altsvc.h"
#అన్తర్భావ "hsts.h"
#అన్తర్భావ "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#అన్తర్భావ "curlx/timeval.h" /* for curlx_now_init() */
#అన్తర్భావ "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#అన్తర్భావ "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
స్థిర గుర్తులేని పూర్ణాంకం initialized;
#నిర్వచితమైతే USE_WINSOCK
స్థిర bool winsock_initialized;
#అయితే_ముగింపు

#నిర్వచితమైతే GLOBAL_INIT_IS_THREADSAFE

స్థిర curl_simple_తాళం s_తాళం = CURL_SIMPLE_తాళం_INIT;
#నిర్వచించు global_init_తాళం()   curl_simple_తాళం_తాళం(&s_తాళం)
#నిర్వచించు global_init_unlock() curl_simple_తాళం_unlock(&s_తాళం)

#లేకపోతే

#నిర్వచించు global_init_తాళం()
#నిర్వచించు global_init_unlock()

#అయితే_ముగింపు

#ఒకవేళ defined(_MSC_VER) && defined(_DLL)
#  pragma warning(నెట్టు)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#అయితే_ముగింపు

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)జ్ఞాపకం_ఇవ్వు;
curl_free_callback Curl_cfree = (curl_free_callback)విడుదల;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_దిగువ;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)సున్నా_జ్ఞాపకం_ఇవ్వు;

#ఒకవేళ defined(_MSC_VER) && defined(_DLL)
#  pragma warning(తీయి)
#అయితే_ముగింపు

#నిర్వచితమైతే DEBUGBUILD
స్థిర అక్షరం *leakpointer;
#అయితే_ముగింపు

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
స్థిర CURLcode global_init(దీర్ఘ flags, bool memoryfuncs)
{
  (శూన్య)flags;

  ఒకవేళ(initialized++)
    తిరిగి CURLE_OK;

  ఒకవేళ(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)జ్ఞాపకం_ఇవ్వు;
    Curl_cfree = (curl_free_callback)విడుదల;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_దిగువ;
    Curl_ccalloc = (curl_calloc_callback)సున్నా_జ్ఞాపకం_ఇవ్వు;
  }

#నిర్వచితమైతే _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#అయితే_ముగింపు

  ఒకవేళ(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    వెళ్ళు fail;
  }

#నిర్వచితమైతే USE_WINDOWS_SSPI
  ఒకవేళ(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    వెళ్ళు fail;
  }
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  ఒకవేళ(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    ఒకవేళ(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      వెళ్ళు fail;
    }
    winsock_initialized = TRUE;
  }
#అయితే_ముగింపు

  ఒకవేళ(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    వెళ్ళు fail;
  }

  ఒకవేళ(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    వెళ్ళు fail;
  }

  ఒకవేళ(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    వెళ్ళు fail;
  }

  ఒకవేళ(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    వెళ్ళు fail;
  }

  ఒకవేళ(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    వెళ్ళు fail;
  }

  ఒకవేళ(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    వెళ్ళు fail;
  }

  ఒకవేళ(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    వెళ్ళు fail;
  }

#నిర్వచితమైతే DEBUGBUILD
  ఒకవేళ(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#అయితే_ముగింపు

  తిరిగి CURLE_OK;

fail:
#నిర్వచితమైతే USE_WINSOCK
  ఒకవేళ(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#అయితే_ముగింపు
  initialized--; /* undo the increase */
  తిరిగి CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(దీర్ఘ flags)
{
  CURLcode ఫలితం;
  global_init_తాళం();

  ఫలితం = global_init(flags, TRUE);

  global_init_unlock();

  తిరిగి ఫలితం;
}

/*
 * curl_global_init_mem() globally initializes curl and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(దీర్ఘ flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode ఫలితం;

  /* Invalid input, return immediately */
  ఒకవేళ(!m || !f || !r || !s || !c)
    తిరిగి CURLE_FAILED_INIT;

  global_init_తాళం();

  ఒకవేళ(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    తిరిగి CURLE_OK;
  }

  /* set memory functions before global_init() in case it wants memory
     functions */
  Curl_cmalloc = m;
  Curl_cfree = f;
  Curl_cstrdup = s;
  Curl_crealloc = r;
  Curl_ccalloc = c;

  /* Call the actual init function, but without setting */
  ఫలితం = global_init(flags, FALSE);

  global_init_unlock();

  తిరిగి ఫలితం;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
శూన్య curl_global_cleanup(శూన్య)
{
  global_init_తాళం();

  ఒకవేళ(!initialized) {
    global_init_unlock();
    తిరిగి;
  }

  ఒకవేళ(--initialized) {
    global_init_unlock();
    తిరిగి;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#నిర్వచితమైతే USE_WINSOCK
  ఒకవేళ(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WINDOWS_SSPI
  Curl_sspi_global_cleanup();
#అయితే_ముగింపు

#నిర్వచితమైతే DEBUGBUILD
  curlx_free(leakpointer);
#అయితే_ముగింపు

  global_init_unlock();
}

/**
 * curl_global_trace() globally initializes curl logging.
 */
CURLcode curl_global_trace(స్థిరరాశి అక్షరం *config)
{
#నిర్వచితం_కాకపోతే CURL_DISABLE_VERBOSE_అక్షరశ్రేణులు
  CURLcode ఫలితం;
  global_init_తాళం();

  ఫలితం = Curl_trc_opt(config);

  global_init_unlock();

  తిరిగి ఫలితం;
#లేకపోతే
  (శూన్య)config;
  తిరిగి CURLE_OK;
#అయితే_ముగింపు
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, స్థిరరాశి అక్షరం *name,
                              స్థిరరాశి curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_తాళం();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  తిరిగి rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(శూన్య)
{
  CURLcode ఫలితం;
  నిర్మాణం Curl_easy *దత్తాంశం;

  /* Make sure we inited the global SSL stuff */
  global_init_తాళం();

  ఒకవేళ(!initialized) {
    ఫలితం = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    ఒకవేళ(ఫలితం) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      తిరిగి ఖాళీ;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  ఫలితం = Curl_open(&దత్తాంశం);
  ఒకవేళ(ఫలితం) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    తిరిగి ఖాళీ;
  }

  తిరిగి దత్తాంశం;
}

#నిర్వచితమైతే DEBUGBUILD

నిర్మాణం socketmonitor {
  నిర్మాణం socketmonitor *తదుపరి; /* the next node in the list or NULL */
  నిర్మాణం pollfd socket; /* socket info of what to monitor */
};

నిర్మాణం events {
  దీర్ఘ ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  పూర్ణాంకం అంకె_sockets;      /* number of nodes in the monitor list */
  నిర్మాణం socketmonitor *list; /* list of sockets to monitor */
  పూర్ణాంకం running_handles;  /* store the returned number */
};

#నిర్వచించు DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
స్థిర పూర్ణాంకం events_timer(CURLM *multi,    /* multi handle */
                        దీర్ఘ timeout_ms, /* see above */
                        శూన్య *userp)     /* private callback pointer */
{
  నిర్మాణం events *ev = userp;
  (శూన్య)multi;
#ఒకవేళ DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#అయితే_ముగింపు
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  తిరిగి 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
స్థిర పూర్ణాంకం poll2cselect(పూర్ణాంకం pollmask)
{
  పూర్ణాంకం omask = 0;
  ఒకవేళ(pollmask & POLLIN)
    omask |= CURL_CSELECT_లోపల;
  ఒకవేళ(pollmask & POLLOUT)
    omask |= CURL_CSELECT_బహిర్గతం;
  ఒకవేళ(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  తిరిగి omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
స్థిర పొట్టి socketcb2poll(పూర్ణాంకం pollmask)
{
  పొట్టి omask = 0;
  ఒకవేళ(pollmask & CURL_POLL_లోపల)
    omask |= POLLIN;
  ఒకవేళ(pollmask & CURL_POLL_బహిర్గతం)
    omask |= POLLOUT;
  తిరిగి omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
స్థిర పూర్ణాంకం events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         పూర్ణాంకం what,        /* see above */
                         శూన్య *userp,     /* private callback
సూచిక*/
                         శూన్య *socketp)   /* private socket
సూచిక*/
{
  నిర్మాణం events *ev = userp;
  నిర్మాణం socketmonitor *m;
  నిర్మాణం socketmonitor *prev = ఖాళీ;
  bool దొరికింది = FALSE;
  నిర్మాణం Curl_easy *దత్తాంశం = easy;

#నిర్వచితమైతే CURL_DISABLE_VERBOSE_అక్షరశ్రేణులు
  (శూన్య)easy;
#అయితే_ముగింపు
  (శూన్య)socketp;

  m = ev->list;
  అయితే(m) {
    ఒకవేళ(m->socket.fd == s) {
      దొరికింది = TRUE;
      ఒకవేళ(what == CURL_POLL_REMOVE) {
        నిర్మాణం socketmonitor *nxt = m->తదుపరి;
        /* remove this node from the list of monitored sockets */
        ఒకవేళ(prev)
          prev->తదుపరి = nxt;
        లేకపోతే
          ev->list = nxt;
        curlx_free(m);
        infof(దత్తాంశం, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      లేకపోతే {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(దత్తాంశం, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_లోపల) ? "IN" : "",
              (what & CURL_POLL_బహిర్గతం) ? "OUT" : "");
      }
      ఆపు;
    }
    prev = m;
    m = m->తదుపరి; /* move to next node */
  }

  ఒకవేళ(!దొరికింది) {
    ఒకవేళ(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(దత్తాంశం, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    లేకపోతే {
      m = curlx_malloc(పరిమాణం(నిర్మాణం socketmonitor));
      ఒకవేళ(m) {
        m->తదుపరి = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(దత్తాంశం, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_లోపల) ? "IN" : "",
              (what & CURL_POLL_బహిర్గతం) ? "OUT" : "");
      }
      లేకపోతే
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  తిరిగి 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
స్థిర శూన్య events_అమరిక(నిర్మాణం Curl_multi *multi, నిర్మాణం events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

స్థిర CURLcode populate_cpfds(నిర్మాణం curl_pollfds *cpfds, నిర్మాణం events *ev)
{
  నిర్మాణం socketmonitor *m;
  CURLcode ఫలితం = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  కొరకు(m = ev->list; m && !ఫలితం; m = m->తదుపరి) {
    ఫలితం = Curl_pollfds_కూడు_sock(cpfds, m->socket.fd, m->socket.events);
#ఒకవేళ DEBUG_EV_POLL
    ఒకవేళ(!ఫలితం)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#అయితే_ముగింపు
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode poll_cpfds(నిర్మాణం events *ev,
                           నిర్మాణం curl_pollfds *cpfds,
                           పూర్ణాంకం *pollrc)
{
  ఒకవేళ(cpfds->n) {
    /* wait for activity or timeout */
#ఒకవేళ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#అయితే_ముగింపు
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#ఒకవేళ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#అయితే_ముగింపు
    ఒకవేళ(*pollrc < 0)
      తిరిగి CURLE_UNRECOVERABLE_POLL;
  }
  లేకపోతే {
#ఒకవేళ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#అయితే_ముగింపు
    *pollrc = 0;
    ఒకవేళ(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  తిరిగి CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
స్థిర CURLcode wait_or_timeout(నిర్మాణం Curl_multi *multi, నిర్మాణం events *ev)
{
  నిర్మాణం pollfd fds_on_దొంతర[8];
  నిర్మాణం curl_pollfds cpfds;
  bool పూర్తి = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ఫలితం = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_దొంతర, CURL_ARRAYSIZE(fds_on_దొంతర));
  అయితే(!పూర్తి) {
    CURLMsg *సందేశం;
    పూర్ణాంకం pollrc;
    నిర్మాణం curltime ప్రారంభం;

    ఫలితం = populate_cpfds(&cpfds, ev);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&ప్రారంభం);

    ఫలితం = poll_cpfds(ev, &cpfds, &pollrc);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    ev->msbump = FALSE; /* reset here */

    ఒకవేళ(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#ఒకవేళ DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#అయితే_ముగింపు
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    లేకపోతే {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      గుర్తులేని పూర్ణాంకం i;
      కొరకు(i = 0; i < cpfds.n; i++) {
        ఒకవేళ(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          పూర్ణాంకం act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      ఒకవేళ(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), ప్రారంభం);
        ఒకవేళ(spent_ms > 0) {
#ఒకవేళ DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (దీర్ఘ)spent_ms);
#అయితే_ముగింపు
          ఒకవేళ(spent_ms > ev->ms)
            ev->ms = 0;
          లేకపోతే
            ev->ms -= (దీర్ఘ)spent_ms;
        }
      }
    }

    ఒకవేళ(mresult) {
      ఫలితం = CURLE_URL_MALFORMAT;
      వెళ్ళు బహిర్గతం;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    సందేశం = curl_multi_info_read(multi, &pollrc);
    ఒకవేళ(సందేశం) {
      ఫలితం = సందేశం->దత్తాంశం.ఫలితం;
      పూర్తి = TRUE;
    }
  }
బహిర్గతం:
  Curl_pollfds_cleanup(&cpfds);
  తిరిగి ఫలితం;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
స్థిర CURLcode easy_events(నిర్మాణం Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  స్థిర నిర్మాణం events evs = { -1, FALSE, 0, ఖాళీ, 0 };

  /* if running event-based, do some further multi inits */
  events_అమరిక(multi, &evs);

  తిరిగి wait_or_timeout(multi, &evs);
}
#లేకపోతే /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#నిర్వచించు easy_events(x) CURLE_NOT_BUILT_లోపల
#అయితే_ముగింపు

స్థిర CURLcode easy_transfer(నిర్మాణం Curl_multi *multi)
{
  bool పూర్తి = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ఫలితం = CURLE_OK;

  అయితే(!పూర్తి && !mresult) {
    పూర్ణాంకం still_running = 0;

    mresult = curl_multi_poll(multi, ఖాళీ, 0, 1000, ఖాళీ);

    ఒకవేళ(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    ఒకవేళ(!mresult && !still_running) {
      పూర్ణాంకం rc;
      CURLMsg *సందేశం = curl_multi_info_read(multi, &rc);
      ఒకవేళ(సందేశం) {
        ఫలితం = సందేశం->దత్తాంశం.ఫలితం;
        పూర్తి = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  ఒకవేళ(mresult) {
    ఫలితం = (mresult == CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం) ? CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_వాదం;
  }

  తిరిగి ఫలితం;
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
స్థిర CURLcode easy_perform(నిర్మాణం Curl_easy *దత్తాంశం, bool events)
{
  నిర్మాణం Curl_multi *multi;
  CURLMcode mresult;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం Curl_sigpipe_ctx sigpipe_ctx;

  ఒకవేళ(!దత్తాంశం)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(దత్తాంశం->సమితి.errorbuffer)
    /* clear this as early as possible */
    దత్తాంశం->సమితి.errorbuffer[0] = 0;

  దత్తాంశం->state.os_errno = 0;

  ఒకవేళ(దత్తాంశం->multi) {
    failf(దత్తాంశం, "easy handle already used in multi handle");
    తిరిగి CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  ఒకవేళ(దత్తాంశం->conn) {
    నిర్మాణం connectdata *conn = దత్తాంశం->conn;
    Curl_detach_connection(దత్తాంశం);
    Curl_conn_close(దత్తాంశం, conn, TRUE);
    DEBUGASSERT(!దత్తాంశం->conn);
  }

  ఒకవేళ(దత్తాంశం->multi_easy)
    multi = దత్తాంశం->multi_easy;
  లేకపోతే {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    ఒకవేళ(!multi)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(Curl_api_multi_is_లోపల_callback(multi))
    తిరిగి CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (దీర్ఘ)దత్తాంశం->సమితి.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_శీఘ్ర_EXIT, (దీర్ఘ)దత్తాంశం->సమితి.quick_exit);

  దత్తాంశం->multi_easy = ఖాళీ; /* pretend it does not exist */
  mresult = Curl_multi_కూడు_handle(multi, దత్తాంశం);
  ఒకవేళ(mresult) {
    curl_multi_cleanup(multi);
    ఒకవేళ(mresult == CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    తిరిగి CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  దత్తాంశం->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(దత్తాంశం, &sigpipe_ctx);

  /* run the transfer */
  ఫలితం = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (శూన్య)Curl_multi_remove_handle(multi, దత్తాంశం);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  తిరిగి ఫలితం;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  నిర్మాణం Curl_eapi_guard guard = { 0 };
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_perform, &ఫలితం)) {
    ఫలితం = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(నిర్మాణం Curl_easy *easy)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &ఫలితం)) {
    ఫలితం = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
శూన్య curl_easy_cleanup(CURL *curl)
{
  నిర్మాణం Curl_eapi_guard guard;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, ఖాళీ)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    నిర్మాణం Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(దత్తాంశం, &sigpipe_ctx);
    Curl_close(&దత్తాంశం);
    sigpipe_restore(&sigpipe_ctx);
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#నిర్వచనం_రద్దు curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &ఫలితం)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    va_list arg;
    శూన్య *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, శూన్య *);

    ఫలితం = Curl_getinfo(దత్తాంశం, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}

స్థిర CURLcode dupset(నిర్మాణం Curl_easy *dst, నిర్మాణం Curl_easy *src)
{
  CURLcode ఫలితం = CURLE_OK;
  గణన dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->సమితి = src->సమితి;
#ఒకవేళ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->సమితి.mimepostp = ఖాళీ;
#అయితే_ముగింపు
  dst->సమితి.str_copypostfields = ఖాళీ;

  Curl_u8_strset_init(&dst->సమితి.అక్షరశ్రేణులు);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->సమితి.blobs, 0, BLOB_చివరి * పరిమాణం(నిర్మాణం curl_blob *));

  /* duplicate all strings */
  ఫలితం = Curl_u8_strset_copy(&dst->సమితి.అక్షరశ్రేణులు, &src->సమితి.అక్షరశ్రేణులు);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* duplicate all blobs */
  కొరకు(j = (గణన dupblob)0; j < BLOB_చివరి; j++) {
    ఫలితం = Curl_setblobopt(&dst->సమితి.blobs[j], src->సమితి.blobs[j]);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  /* duplicate memory areas pointed to */
  ఒకవేళ(src->సమితి.str_copypostfields) {
    ఒకవేళ(src->సమితి.postfieldsize == -1)
      dst->సమితి.str_copypostfields = curlx_strdup(src->సమితి.str_copypostfields);
    లేకపోతే
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->సమితి.str_copypostfields =
        curlx_memdup0(src->సమితి.str_copypostfields,
                      curlx_sotouz(src->సమితి.postfieldsize));
    ఒకవేళ(!dst->సమితి.str_copypostfields)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    /* point to the new copy */
    dst->సమితి.postfields = dst->సమితి.str_copypostfields;
  }

#ఒకవేళ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ఒకవేళ(src->సమితి.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->సమితి.mimepostp = curlx_malloc(పరిమాణం(*dst->సమితి.mimepostp));
    ఒకవేళ(!dst->సమితి.mimepostp)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    Curl_mime_initpart(dst->సమితి.mimepostp);
    ఫలితం = Curl_mime_duppart(dst, dst->సమితి.mimepostp, src->సమితి.mimepostp);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
#అయితే_ముగింపు

  ఒకవేళ(src->సమితి.resolve)
    dst->state.resolve = dst->సమితి.resolve;

  తిరిగి ఫలితం;
}

స్థిర శూన్య dupeasy_meta_freeentry(శూన్య *p)
{
  (శూన్య)p;
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
  నిర్మాణం Curl_eapi_guard guard;
  నిర్మాణం Curl_easy *outcurl = ఖాళీ;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, ఖాళీ)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;

    outcurl = curlx_calloc(1, పరిమాణం(నిర్మాణం Curl_easy));
    ఒకవేళ(!outcurl)
      వెళ్ళు fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->సమితి.బఫరు_కొలత = దత్తాంశం->సమితి.బఫరు_కొలత;

    Curl_hash_init(&outcurl->meta_hash, 23, CURL_HASH_TYPE_బైట్లు,
                   dupeasy_meta_freeentry);
    curlx_dyn_init(&outcurl->state.headerb, CURL_MAX_HTTP_శీర్షిక);
    Curl_bufref_init(&outcurl->state.url);
    Curl_bufref_init(&outcurl->state.referer);
    Curl_netrc_init(&outcurl->state.netrc);

    /* the connection pool is setup on demand */
    outcurl->state.చివరి_conn_id = -1;
    outcurl->state.చివరి_cpid = UINT32_MAX;
    outcurl->id = -1;
    outcurl->mid = UINT32_MAX;
    outcurl->master_mid = UINT32_MAX;

#నిర్వచితం_కాకపోతే CURL_DISABLE_HTTP
    Curl_llist_init(&outcurl->state.httphdrs, ఖాళీ);
#అయితే_ముగింపు
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    ఒకవేళ(dupset(outcurl, దత్తాంశం))
      వెళ్ళు fail;

    outcurl->పురోగతి.hide     = దత్తాంశం->పురోగతి.hide;
    outcurl->పురోగతి.callback = దత్తాంశం->పురోగతి.callback;

#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
    outcurl->state.cookielist = ఖాళీ;
    ఒకవేళ(దత్తాంశం->cookies && దత్తాంశం->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      ఒకవేళ(!outcurl->cookies)
        వెళ్ళు fail;
      outcurl->state.cookie_engine = TRUE;
    }

    ఒకవేళ(దత్తాంశం->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(దత్తాంశం->state.cookielist);
      ఒకవేళ(!outcurl->state.cookielist)
        వెళ్ళు fail;
    }
#అయితే_ముగింపు

    ఒకవేళ(Curl_bufref_ptr(&దత్తాంశం->state.url)) {
      Curl_bufref_సమితి(&outcurl->state.url,
                      Curl_bufref_dup(&దత్తాంశం->state.url), 0,
                      curl_free);
      ఒకవేళ(!Curl_bufref_ptr(&outcurl->state.url))
        వెళ్ళు fail;
    }
    ఒకవేళ(Curl_bufref_ptr(&దత్తాంశం->state.referer)) {
      Curl_bufref_సమితి(&outcurl->state.referer,
                      Curl_bufref_dup(&దత్తాంశం->state.referer), 0,
                      curl_free);
      ఒకవేళ(!Curl_bufref_ptr(&outcurl->state.referer))
        వెళ్ళు fail;
    }

#నిర్వచితం_కాకపోతే CURL_DISABLE_ALTSVC
    ఒకవేళ(దత్తాంశం->asi) {
      స్థిరరాశి అక్షరం *str;
      outcurl->asi = Curl_altsvc_init();
      ఒకవేళ(!outcurl->asi)
        వెళ్ళు fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      ఒకవేళ(str)
        (శూన్య)Curl_altsvc_ఎక్కించు(outcurl->asi, str);
    }
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CURL_DISABLE_HSTS
    ఒకవేళ(దత్తాంశం->hsts) {
      స్థిరరాశి అక్షరం *str;
      outcurl->hsts = Curl_hsts_init();
      ఒకవేళ(!outcurl->hsts)
        వెళ్ళు fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      ఒకవేళ(str)
        (శూన్య)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (శూన్య)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      ఒకవేళ(Curl_hsts_copy(outcurl->hsts, దత్తాంశం->hsts))
        వెళ్ళు fail;
    }
#అయితే_ముగింపు

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_సంఖ్య;
  }
  CURL_EAPI_LEAVE(&guard);
  తిరిగి outcurl;

fail:
  ఒకవేళ(outcurl) {
#నిర్వచితం_కాకపోతే CURL_DISABLE_COOKIES
    curlx_free(outcurl->cookies);
#అయితే_ముగింపు
    curlx_dyn_free(&outcurl->state.headerb);
    Curl_altsvc_cleanup(&outcurl->asi);
    Curl_hsts_cleanup(&outcurl->hsts);
    Curl_freeset(outcurl);
    curlx_free(outcurl);
  }

  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఖాళీ;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
శూన్య curl_easy_reset(CURL *curl)
{
  నిర్మాణం Curl_eapi_guard guard;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_reset, ఖాళీ)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;

    దత్తాంశం->state.చివరి_conn_id = -1; /* clear remembered connection id */
    దత్తాంశం->state.చివరి_cpid = UINT32_MAX;
    Curl_req_hard_reset(&దత్తాంశం->req, దత్తాంశం);
    Curl_hash_clean(&దత్తాంశం->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(దత్తాంశం);
    /* zero out UserDefined data: */
    Curl_freeset(దత్తాంశం);
    memset(&దత్తాంశం->సమితి, 0, పరిమాణం(నిర్మాణం UserDefined));
    Curl_init_userdefined(దత్తాంశం);

    /* zero out Progress data: */
    memset(&దత్తాంశం->పురోగతి, 0, పరిమాణం(నిర్మాణం పురోగతి_2));

    /* zero out PureInfo data: */
    Curl_initinfo(దత్తాంశం);

    దత్తాంశం->పురోగతి.hide = TRUE;

    /* zero out authentication data: */
    memset(&దత్తాంశం->state.authhost, 0, పరిమాణం(నిర్మాణం auth));
    memset(&దత్తాంశం->state.authproxy, 0, పరిమాణం(నిర్మాణం auth));

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(దత్తాంశం);
#అయితే_ముగింపు
    దత్తాంశం->master_mid = UINT32_MAX;
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
CURLcode curl_easy_pause(CURL *curl, పూర్ణాంకం action)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_pause, &ఫలితం)) {
    bool మారింది = FALSE;
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    ఒకవేళ(!దత్తాంశం->conn) {
      /* crazy input, do not continue */
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }

    recv_paused = Curl_xfer_recv_is_paused(దత్తాంశం);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(దత్తాంశం);
    send_paused_new = (action & CURLPAUSE_SEND);

    ఒకవేళ((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(దత్తాంశం))) {
      మారింది = TRUE;
      ఫలితం = Curl_1st_fatal(
        ఫలితం, Curl_xfer_pause_send(దత్తాంశం, send_paused_new));
    }

    ఒకవేళ(recv_paused != recv_paused_new) {
      మారింది = TRUE;
      ఫలితం = Curl_1st_fatal(
        ఫలితం, Curl_xfer_pause_recv(దత్తాంశం, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    ఒకవేళ(!Curl_xfer_is_blocked(దత్తాంశం)) {
      /* reset the too-slow time keeper */
      దత్తాంశం->state.keeps_వేగం.tv_sec = 0;
      ఒకవేళ(దత్తాంశం->multi) {
        Curl_multi_mark_dirty(దత్తాంశం); /* make it run */
        /* On changes, tell application to update its timers. */
        ఒకవేళ(మారింది && Curl_update_timer(దత్తాంశం->multi) && !ఫలితం)
          ఫలితం = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    ఒకవేళ(!ఫలితం && మారింది && !దత్తాంశం->state.పూర్తి && దత్తాంశం->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(దత్తాంశం->multi, దత్తాంశం))
      ఫలితం = CURLE_ABORTED_BY_CALLBACK;
  }
బహిర్గతం:
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}

స్థిర CURLcode easy_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata **connp)
{
  curl_socket_t sfd;

  ఒకవేళ(!దత్తాంశం)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  ఒకవేళ(!దత్తాంశం->సమితి.connect_only) {
    failf(దత్తాంశం, "CONNECT_ONLY is required");
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(దత్తాంశం, connp);

  ఒకవేళ(sfd == CURL_SOCKET_BAD) {
    failf(దత్తాంశం, "Failed to get last socket used for connection #%" FMT_OFF_T,
          దత్తాంశం->state.చివరి_conn_id);
    తిరిగి CURLE_UNSUPPORTED_PROTOCOL;
  }

  తిరిగి CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(నిర్మాణం Curl_easy *దత్తాంశం,
                        శూన్య *బఫరు, size_t buflen, size_t *n)
{
  CURLcode ఫలితం;
  నిర్మాణం connectdata *c;

  ఫలితం = easy_connection(దత్తాంశం, &c);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(!దత్తాంశం->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(దత్తాంశం, c, TRUE);

  *n = 0;
  తిరిగి Curl_conn_recv(దత్తాంశం, FIRSTSOCKET, బఫరు, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, శూన్య *బఫరు, size_t buflen, size_t *n)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_recv, &ఫలితం)) {
    ఒకవేళ(!n || (buflen && !బఫరు)) {
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }
    ఫలితం = Curl_easy_recv(curl, బఫరు, buflen, n);
  }
బహిర్గతం:
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం;
  నిర్మాణం connectdata *c = ఖాళీ;

  ఫలితం = easy_connection(దత్తాంశం, &c);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(!దత్తాంశం->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(దత్తాంశం, c, TRUE);

  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి శూన్య *బఫరు,
                       size_t buflen, size_t *n)
{
  CURLcode ఫలితం;
  నిర్మాణం connectdata *c = ఖాళీ;
  నిర్మాణం Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  ఫలితం = easy_connection(దత్తాంశం, &c);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(!దత్తాంశం->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(దత్తాంశం, c, TRUE);

  sigpipe_ignore(దత్తాంశం, &sigpipe_ctx);
  ఫలితం = Curl_conn_send(దత్తాంశం, FIRSTSOCKET, బఫరు, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  ఒకవేళ(ఫలితం && ఫలితం != CURLE_AGAIN)
    తిరిగి CURLE_SEND_ERROR;
  తిరిగి ఫలితం;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, స్థిరరాశి శూన్య *బఫరు, size_t buflen,
                        size_t *n)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_send, &ఫలితం)) {
    నిర్మాణం Curl_easy *దత్తాంశం = curl;
    size_t written = 0;

    ఒకవేళ(!n || (buflen && !బఫరు)) {
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }
    ఫలితం = Curl_senddata(దత్తాంశం, బఫరు, buflen, &written);
    *n = written;
  }
బహిర్గతం:
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &ఫలితం)) {
    /* Use the common function to keep connections alive. */
    ఫలితం = Curl_cpool_upkeep((నిర్మాణం Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
}

CURLcode curl_easy_ssls_import(CURL *curl, స్థిరరాశి అక్షరం *session_కీలకం,
                               స్థిరరాశి గుర్తులేని అక్షరం *shmac, size_t shmac_len,
                               స్థిరరాశి గుర్తులేని అక్షరం *sdata, size_t sdata_len)
{
#ఒకవేళ defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &ఫలితం)) {
    ఒకవేళ(!sdata || !sdata_len)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      ఫలితం = Curl_ssl_session_import((నిర్మాణం Curl_easy *)curl, session_కీలకం,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
#లేకపోతే
  (శూన్య)curl;
  (శూన్య)session_కీలకం;
  (శూన్య)shmac;
  (శూన్య)shmac_len;
  (శూన్య)sdata;
  (శూన్య)sdata_len;
  తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               శూన్య *userptr)
{
#ఒకవేళ defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  నిర్మాణం Curl_eapi_guard guard;
  CURLcode ఫలితం;

  ఒకవేళ(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &ఫలితం)) {
    ఒకవేళ(!export_fn)
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    లేకపోతే
      ఫలితం = Curl_ssl_session_export((నిర్మాణం Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  తిరిగి ఫలితం;
#లేకపోతే
  (శూన్య)curl;
  (శూన్య)export_fn;
  (శూన్య)userptr;
  తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
}

CURLcode Curl_meta_సమితి(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *కీలకం,
                       శూన్య *meta_దత్తాంశం, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_దత్తాంశం); /* never set to NULL */
  ఒకవేళ(!Curl_hash_add2(&దత్తాంశం->meta_hash, కీలకం, అక్షరశ్రేణి_పొడవు(కీలకం) + 1,
                     meta_దత్తాంశం, meta_dtor)) {
    meta_dtor(కీలకం, అక్షరశ్రేణి_పొడవు(కీలకం) + 1, meta_దత్తాంశం);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  తిరిగి CURLE_OK;
}

శూన్య Curl_meta_remove(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *కీలకం)
{
  Curl_hash_delete(&దత్తాంశం->meta_hash, కీలకం, అక్షరశ్రేణి_పొడవు(కీలకం) + 1);
}

శూన్య *Curl_meta_get(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *కీలకం)
{
  తిరిగి Curl_hash_pick(&దత్తాంశం->meta_hash, కీలకం, అక్షరశ్రేణి_పొడవు(కీలకం) + 1);
}

శూన్య Curl_meta_reset(నిర్మాణం Curl_easy *దత్తాంశం)
{
  Curl_hash_clean(&దత్తాంశం->meta_hash);
}
