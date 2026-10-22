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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "api.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "vdns/hostip.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vquic/vquic.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "getinfo.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "easyif.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "multi_ev.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "connect.h" /* for Curl_getconnectinfo */
#অন্তর্ভাব "slist.h"
#অন্তর্ভাব "mime.h"
#অন্তর্ভাব "amigaos.h"
#অন্তর্ভাব "macos.h"
#অন্তর্ভাব "curlx/wait.h"
#অন্তর্ভাব "sigpipe.h"
#অন্তর্ভাব "vssh/ssh.h"
#অন্তর্ভাব "setopt.h"
#অন্তর্ভাব "http_digest.h"
#অন্তর্ভাব "curlx/dynbuf.h"
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "altsvc.h"
#অন্তর্ভাব "hsts.h"
#অন্তর্ভাব "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#অন্তর্ভাব "curlx/timeval.h" /* for curlx_now_init() */
#অন্তর্ভাব "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#অন্তর্ভাব "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
স্থির unsigned পূর্ণসংখ্যা initialized;
#ifdef USE_WINSOCK
স্থির bool winsock_initialized;
#endif

#ifdef GLOBAL_INIT_IS_THREADSAFE

স্থির curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#define global_init_lock()   curl_simple_lock_lock(&s_lock)
#define global_init_unlock() curl_simple_lock_unlock(&s_lock)

#অন্যথা

#define global_init_lock()
#define global_init_unlock()

#endif

#যদি defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ঠেলো)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)malloc;
curl_free_callback Curl_cfree = (curl_free_callback)মুক্ত;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_নিচের;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#যদি defined(_MSC_VER) && defined(_DLL)
#  pragma warning(বের_করো)
#endif

#ifdef DEBUGBUILD
স্থির অক্ষর *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
স্থির CURLcode global_init(long flags, bool memoryfuncs)
{
  (শূন্য)flags;

  যদি(initialized++)
    ফেরত CURLE_OK;

  যদি(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)মুক্ত;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_নিচের;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  যদি(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    যাও fail;
  }

#ifdef USE_WINDOWS_SSPI
  যদি(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    যাও fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  যদি(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    যদি(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      যাও fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  যদি(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    যাও fail;
  }

  যদি(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    যাও fail;
  }

  যদি(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    যাও fail;
  }

  যদি(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    যাও fail;
  }

  যদি(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    যাও fail;
  }

  যদি(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    যাও fail;
  }

  যদি(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    যাও fail;
  }

#ifdef DEBUGBUILD
  যদি(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  ফেরত CURLE_OK;

fail:
#ifdef USE_WINSOCK
  যদি(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  ফেরত CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(long flags)
{
  CURLcode ফলাফল;
  global_init_lock();

  ফলাফল = global_init(flags, TRUE);

  global_init_unlock();

  ফেরত ফলাফল;
}

/*
 * curl_global_init_mem() globally initializes curl and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(long flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode ফলাফল;

  /* Invalid input, return immediately */
  যদি(!m || !f || !r || !s || !c)
    ফেরত CURLE_FAILED_INIT;

  global_init_lock();

  যদি(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    ফেরত CURLE_OK;
  }

  /* set memory functions before global_init() in case it wants memory
     functions */
  Curl_cmalloc = m;
  Curl_cfree = f;
  Curl_cstrdup = s;
  Curl_crealloc = r;
  Curl_ccalloc = c;

  /* Call the actual init function, but without setting */
  ফলাফল = global_init(flags, FALSE);

  global_init_unlock();

  ফেরত ফলাফল;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
শূন্য curl_global_cleanup(শূন্য)
{
  global_init_lock();

  যদি(!initialized) {
    global_init_unlock();
    ফেরত;
  }

  যদি(--initialized) {
    global_init_unlock();
    ফেরত;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ifdef USE_WINSOCK
  যদি(winsock_initialized) {
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
CURLcode curl_global_trace(ধ্রুবক অক্ষর *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode ফলাফল;
  global_init_lock();

  ফলাফল = Curl_trc_opt(config);

  global_init_unlock();

  ফেরত ফলাফল;
#অন্যথা
  (শূন্য)config;
  ফেরত CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ধ্রুবক অক্ষর *name,
                              ধ্রুবক curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  ফেরত rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(শূন্য)
{
  CURLcode ফলাফল;
  কাঠামো Curl_easy *তথ্য;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  যদি(!initialized) {
    ফলাফল = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    যদি(ফলাফল) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      ফেরত ফাঁকা;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  ফলাফল = Curl_open(&তথ্য);
  যদি(ফলাফল) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    ফেরত ফাঁকা;
  }

  ফেরত তথ্য;
}

#ifdef DEBUGBUILD

কাঠামো socketmonitor {
  কাঠামো socketmonitor *পরবর্তী; /* the next node in the list or NULL */
  কাঠামো pollfd socket; /* socket info of what to monitor */
};

কাঠামো events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  পূর্ণসংখ্যা অঙ্ক_sockets;      /* number of nodes in the monitor list */
  কাঠামো socketmonitor *list; /* list of sockets to monitor */
  পূর্ণসংখ্যা running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
স্থির পূর্ণসংখ্যা events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        শূন্য *userp)     /* private callback pointer */
{
  কাঠামো events *ev = userp;
  (শূন্য)multi;
#যদি DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  ফেরত 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
স্থির পূর্ণসংখ্যা poll2cselect(পূর্ণসংখ্যা pollmask)
{
  পূর্ণসংখ্যা omask = 0;
  যদি(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  যদি(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  যদি(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  ফেরত omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
স্থির short socketcb2poll(পূর্ণসংখ্যা pollmask)
{
  short omask = 0;
  যদি(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  যদি(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  ফেরত omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
স্থির পূর্ণসংখ্যা events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         পূর্ণসংখ্যা what,        /* see above */
                         শূন্য *userp,     /* private callback
                                             pointer */
                         শূন্য *socketp)   /* private socket
                                             pointer */
{
  কাঠামো events *ev = userp;
  কাঠামো socketmonitor *m;
  কাঠামো socketmonitor *পূর্ব = ফাঁকা;
  bool found = FALSE;
  কাঠামো Curl_easy *তথ্য = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (শূন্য)easy;
#endif
  (শূন্য)socketp;

  m = ev->list;
  যখন(m) {
    যদি(m->socket.fd == s) {
      found = TRUE;
      যদি(what == CURL_POLL_REMOVE) {
        কাঠামো socketmonitor *nxt = m->পরবর্তী;
        /* remove this node from the list of monitored sockets */
        যদি(পূর্ব)
          পূর্ব->পরবর্তী = nxt;
        অন্যথা
          ev->list = nxt;
        curlx_free(m);
        infof(তথ্য, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      অন্যথা {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(তথ্য, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      বিরতি;
    }
    পূর্ব = m;
    m = m->পরবর্তী; /* move to next node */
  }

  যদি(!found) {
    যদি(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(তথ্য, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    অন্যথা {
      m = curlx_malloc(আকার(কাঠামো socketmonitor));
      যদি(m) {
        m->পরবর্তী = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(তথ্য, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      অন্যথা
        ফেরত CURLE_OUT_OF_MEMORY;
    }
  }

  ফেরত 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
স্থির শূন্য events_setup(কাঠামো Curl_multi *multi, কাঠামো events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

স্থির CURLcode populate_cpfds(কাঠামো curl_pollfds *cpfds, কাঠামো events *ev)
{
  কাঠামো socketmonitor *m;
  CURLcode ফলাফল = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  জন্য(m = ev->list; m && !ফলাফল; m = m->পরবর্তী) {
    ফলাফল = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#যদি DEBUG_EV_POLL
    যদি(!ফলাফল)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  ফেরত ফলাফল;
}

স্থির CURLcode poll_cpfds(কাঠামো events *ev,
                           কাঠামো curl_pollfds *cpfds,
                           পূর্ণসংখ্যা *pollrc)
{
  যদি(cpfds->n) {
    /* wait for activity or timeout */
#যদি DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#endif
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#যদি DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#endif
    যদি(*pollrc < 0)
      ফেরত CURLE_UNRECOVERABLE_POLL;
  }
  অন্যথা {
#যদি DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    যদি(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  ফেরত CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
স্থির CURLcode wait_or_timeout(কাঠামো Curl_multi *multi, কাঠামো events *ev)
{
  কাঠামো pollfd fds_on_স্তূপ[8];
  কাঠামো curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ফলাফল = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_স্তূপ, CURL_ARRAYSIZE(fds_on_স্তূপ));
  যখন(!done) {
    CURLMsg *বার্তা;
    পূর্ণসংখ্যা pollrc;
    কাঠামো curltime start;

    ফলাফল = populate_cpfds(&cpfds, ev);
    যদি(ফলাফল)
      যাও out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    ফলাফল = poll_cpfds(ev, &cpfds, &pollrc);
    যদি(ফলাফল)
      যাও out;

    ev->msbump = FALSE; /* reset here */

    যদি(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#যদি DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#endif
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    অন্যথা {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      unsigned পূর্ণসংখ্যা i;
      জন্য(i = 0; i < cpfds.n; i++) {
        যদি(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          পূর্ণসংখ্যা act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      যদি(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        যদি(spent_ms > 0) {
#যদি DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (long)spent_ms);
#endif
          যদি(spent_ms > ev->ms)
            ev->ms = 0;
          অন্যথা
            ev->ms -= (long)spent_ms;
        }
      }
    }

    যদি(mresult) {
      ফলাফল = CURLE_URL_MALFORMAT;
      যাও out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    বার্তা = curl_multi_info_read(multi, &pollrc);
    যদি(বার্তা) {
      ফলাফল = বার্তা->তথ্য.ফলাফল;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  ফেরত ফলাফল;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
স্থির CURLcode easy_events(কাঠামো Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  স্থির কাঠামো events evs = { -1, FALSE, 0, ফাঁকা, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  ফেরত wait_or_timeout(multi, &evs);
}
#অন্যথা /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

স্থির CURLcode easy_transfer(কাঠামো Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ফলাফল = CURLE_OK;

  যখন(!done && !mresult) {
    পূর্ণসংখ্যা still_running = 0;

    mresult = curl_multi_poll(multi, ফাঁকা, 0, 1000, ফাঁকা);

    যদি(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    যদি(!mresult && !still_running) {
      পূর্ণসংখ্যা rc;
      CURLMsg *বার্তা = curl_multi_info_read(multi, &rc);
      যদি(বার্তা) {
        ফলাফল = বার্তা->তথ্য.ফলাফল;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  যদি(mresult) {
    ফলাফল = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ফেরত ফলাফল;
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
স্থির CURLcode easy_perform(কাঠামো Curl_easy *তথ্য, bool events)
{
  কাঠামো Curl_multi *multi;
  CURLMcode mresult;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো Curl_sigpipe_ctx sigpipe_ctx;

  যদি(!তথ্য)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(তথ্য->set.errorbuffer)
    /* clear this as early as possible */
    তথ্য->set.errorbuffer[0] = 0;

  তথ্য->state.os_errno = 0;

  যদি(তথ্য->multi) {
    failf(তথ্য, "easy handle already used in multi handle");
    ফেরত CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  যদি(তথ্য->conn) {
    কাঠামো connectdata *conn = তথ্য->conn;
    Curl_detach_connection(তথ্য);
    Curl_conn_close(তথ্য, conn, TRUE);
    DEBUGASSERT(!তথ্য->conn);
  }

  যদি(তথ্য->multi_easy)
    multi = তথ্য->multi_easy;
  অন্যথা {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    যদি(!multi)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(Curl_api_multi_is_in_callback(multi))
    ফেরত CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)তথ্য->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)তথ্য->set.quick_exit);

  তথ্য->multi_easy = ফাঁকা; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, তথ্য);
  যদি(mresult) {
    curl_multi_cleanup(multi);
    যদি(mresult == CURLM_OUT_OF_MEMORY)
      ফেরত CURLE_OUT_OF_MEMORY;
    ফেরত CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  তথ্য->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(তথ্য, &sigpipe_ctx);

  /* run the transfer */
  ফলাফল = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (শূন্য)Curl_multi_remove_handle(multi, তথ্য);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  ফেরত ফলাফল;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  কাঠামো Curl_eapi_guard guard = { 0 };
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_perform, &ফলাফল)) {
    ফলাফল = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(কাঠামো Curl_easy *easy)
{
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &ফলাফল)) {
    ফলাফল = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
শূন্য curl_easy_cleanup(CURL *curl)
{
  কাঠামো Curl_eapi_guard guard;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, ফাঁকা)) {
    কাঠামো Curl_easy *তথ্য = curl;
    কাঠামো Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(তথ্য, &sigpipe_ctx);
    Curl_close(&তথ্য);
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
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &ফলাফল)) {
    কাঠামো Curl_easy *তথ্য = curl;
    va_list arg;
    শূন্য *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, শূন্য *);

    ফলাফল = Curl_getinfo(তথ্য, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}

স্থির CURLcode dupset(কাঠামো Curl_easy *dst, কাঠামো Curl_easy *src)
{
  CURLcode ফলাফল = CURLE_OK;
  গণনা dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = ফাঁকা;
#endif
  dst->set.str_copypostfields = ফাঁকা;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * আকার(কাঠামো curl_blob *));

  /* duplicate all strings */
  ফলাফল = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* duplicate all blobs */
  জন্য(j = (গণনা dupblob)0; j < BLOB_LAST; j++) {
    ফলাফল = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  /* duplicate memory areas pointed to */
  যদি(src->set.str_copypostfields) {
    যদি(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    অন্যথা
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    যদি(!dst->set.str_copypostfields)
      ফেরত CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  যদি(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(আকার(*dst->set.mimepostp));
    যদি(!dst->set.mimepostp)
      ফেরত CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    ফলাফল = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
#endif

  যদি(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  ফেরত ফলাফল;
}

স্থির শূন্য dupeasy_meta_freeentry(শূন্য *p)
{
  (শূন্য)p;
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
  কাঠামো Curl_eapi_guard guard;
  কাঠামো Curl_easy *outcurl = ফাঁকা;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, ফাঁকা)) {
    কাঠামো Curl_easy *তথ্য = curl;

    outcurl = curlx_calloc(1, আকার(কাঠামো Curl_easy));
    যদি(!outcurl)
      যাও fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->set.বাফার_আকার = তথ্য->set.বাফার_আকার;

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
    Curl_llist_init(&outcurl->state.httphdrs, ফাঁকা);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    যদি(dupset(outcurl, তথ্য))
      যাও fail;

    outcurl->progress.hide     = তথ্য->progress.hide;
    outcurl->progress.callback = তথ্য->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = ফাঁকা;
    যদি(তথ্য->cookies && তথ্য->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      যদি(!outcurl->cookies)
        যাও fail;
      outcurl->state.cookie_engine = TRUE;
    }

    যদি(তথ্য->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(তথ্য->state.cookielist);
      যদি(!outcurl->state.cookielist)
        যাও fail;
    }
#endif

    যদি(Curl_bufref_ptr(&তথ্য->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&তথ্য->state.url), 0,
                      curl_free);
      যদি(!Curl_bufref_ptr(&outcurl->state.url))
        যাও fail;
    }
    যদি(Curl_bufref_ptr(&তথ্য->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&তথ্য->state.referer), 0,
                      curl_free);
      যদি(!Curl_bufref_ptr(&outcurl->state.referer))
        যাও fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    যদি(তথ্য->asi) {
      ধ্রুবক অক্ষর *str;
      outcurl->asi = Curl_altsvc_init();
      যদি(!outcurl->asi)
        যাও fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      যদি(str)
        (শূন্য)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    যদি(তথ্য->hsts) {
      ধ্রুবক অক্ষর *str;
      outcurl->hsts = Curl_hsts_init();
      যদি(!outcurl->hsts)
        যাও fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      যদি(str)
        (শূন্য)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (শূন্য)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      যদি(Curl_hsts_copy(outcurl->hsts, তথ্য->hsts))
        যাও fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_সংখ্যা;
  }
  CURL_EAPI_LEAVE(&guard);
  ফেরত outcurl;

fail:
  যদি(outcurl) {
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
  ফেরত ফাঁকা;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
শূন্য curl_easy_reset(CURL *curl)
{
  কাঠামো Curl_eapi_guard guard;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_reset, ফাঁকা)) {
    কাঠামো Curl_easy *তথ্য = curl;

    তথ্য->state.last_conn_id = -1; /* clear remembered connection id */
    তথ্য->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&তথ্য->req, তথ্য);
    Curl_hash_clean(&তথ্য->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(তথ্য);
    /* zero out UserDefined data: */
    Curl_freeset(তথ্য);
    memset(&তথ্য->set, 0, আকার(কাঠামো UserDefined));
    Curl_init_userdefined(তথ্য);

    /* zero out Progress data: */
    memset(&তথ্য->progress, 0, আকার(কাঠামো Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(তথ্য);

    তথ্য->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&তথ্য->state.authhost, 0, আকার(কাঠামো auth));
    memset(&তথ্য->state.authproxy, 0, আকার(কাঠামো auth));

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(তথ্য);
#endif
    তথ্য->master_mid = UINT32_MAX;
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
CURLcode curl_easy_pause(CURL *curl, পূর্ণসংখ্যা action)
{
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল = CURLE_OK;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_pause, &ফলাফল)) {
    bool changed = FALSE;
    কাঠামো Curl_easy *তথ্য = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    যদি(!তথ্য->conn) {
      /* crazy input, do not continue */
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }

    recv_paused = Curl_xfer_recv_is_paused(তথ্য);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(তথ্য);
    send_paused_new = (action & CURLPAUSE_SEND);

    যদি((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(তথ্য))) {
      changed = TRUE;
      ফলাফল = Curl_1st_fatal(
        ফলাফল, Curl_xfer_pause_send(তথ্য, send_paused_new));
    }

    যদি(recv_paused != recv_paused_new) {
      changed = TRUE;
      ফলাফল = Curl_1st_fatal(
        ফলাফল, Curl_xfer_pause_recv(তথ্য, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    যদি(!Curl_xfer_is_blocked(তথ্য)) {
      /* reset the too-slow time keeper */
      তথ্য->state.keeps_speed.tv_sec = 0;
      যদি(তথ্য->multi) {
        Curl_multi_mark_dirty(তথ্য); /* make it run */
        /* On changes, tell application to update its timers. */
        যদি(changed && Curl_update_timer(তথ্য->multi) && !ফলাফল)
          ফলাফল = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    যদি(!ফলাফল && changed && !তথ্য->state.done && তথ্য->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(তথ্য->multi, তথ্য))
      ফলাফল = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}

স্থির CURLcode easy_connection(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata **connp)
{
  curl_socket_t sfd;

  যদি(!তথ্য)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  যদি(!তথ্য->set.connect_only) {
    failf(তথ্য, "CONNECT_ONLY is required");
    ফেরত CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(তথ্য, connp);

  যদি(sfd == CURL_SOCKET_BAD) {
    failf(তথ্য, "Failed to get last socket used for connection #%" FMT_OFF_T,
          তথ্য->state.last_conn_id);
    ফেরত CURLE_UNSUPPORTED_PROTOCOL;
  }

  ফেরত CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(কাঠামো Curl_easy *তথ্য,
                        শূন্য *বাফার, size_t buflen, size_t *n)
{
  CURLcode ফলাফল;
  কাঠামো connectdata *c;

  ফলাফল = easy_connection(তথ্য, &c);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(!তথ্য->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(তথ্য, c, TRUE);

  *n = 0;
  ফেরত Curl_conn_recv(তথ্য, FIRSTSOCKET, বাফার, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, শূন্য *বাফার, size_t buflen, size_t *n)
{
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_recv, &ফলাফল)) {
    যদি(!n || (buflen && !বাফার)) {
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }
    ফলাফল = Curl_easy_recv(curl, বাফার, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল;
  কাঠামো connectdata *c = ফাঁকা;

  ফলাফল = easy_connection(তথ্য, &c);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(!তথ্য->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(তথ্য, c, TRUE);

  ফেরত CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(কাঠামো Curl_easy *তথ্য, ধ্রুবক শূন্য *বাফার,
                       size_t buflen, size_t *n)
{
  CURLcode ফলাফল;
  কাঠামো connectdata *c = ফাঁকা;
  কাঠামো Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  ফলাফল = easy_connection(তথ্য, &c);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(!তথ্য->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(তথ্য, c, TRUE);

  sigpipe_ignore(তথ্য, &sigpipe_ctx);
  ফলাফল = Curl_conn_send(তথ্য, FIRSTSOCKET, বাফার, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  যদি(ফলাফল && ফলাফল != CURLE_AGAIN)
    ফেরত CURLE_SEND_ERROR;
  ফেরত ফলাফল;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ধ্রুবক শূন্য *বাফার, size_t buflen,
                        size_t *n)
{
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_send, &ফলাফল)) {
    কাঠামো Curl_easy *তথ্য = curl;
    size_t written = 0;

    যদি(!n || (buflen && !বাফার)) {
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }
    ফলাফল = Curl_senddata(তথ্য, বাফার, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &ফলাফল)) {
    /* Use the common function to keep connections alive. */
    ফলাফল = Curl_cpool_upkeep((কাঠামো Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
}

CURLcode curl_easy_ssls_import(CURL *curl, ধ্রুবক অক্ষর *session_চাবি,
                               ধ্রুবক unsigned অক্ষর *shmac, size_t shmac_len,
                               ধ্রুবক unsigned অক্ষর *sdata, size_t sdata_len)
{
#যদি defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &ফলাফল)) {
    যদি(!sdata || !sdata_len)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      ফলাফল = Curl_ssl_session_import((কাঠামো Curl_easy *)curl, session_চাবি,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
#অন্যথা
  (শূন্য)curl;
  (শূন্য)session_চাবি;
  (শূন্য)shmac;
  (শূন্য)shmac_len;
  (শূন্য)sdata;
  (শূন্য)sdata_len;
  ফেরত CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               শূন্য *userptr)
{
#যদি defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  কাঠামো Curl_eapi_guard guard;
  CURLcode ফলাফল;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &ফলাফল)) {
    যদি(!export_fn)
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      ফলাফল = Curl_ssl_session_export((কাঠামো Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  ফেরত ফলাফল;
#অন্যথা
  (শূন্য)curl;
  (শূন্য)export_fn;
  (শূন্য)userptr;
  ফেরত CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *চাবি,
                       শূন্য *meta_তথ্য, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_তথ্য); /* never set to NULL */
  যদি(!Curl_hash_add2(&তথ্য->meta_hash, চাবি, strlen(চাবি) + 1,
                     meta_তথ্য, meta_dtor)) {
    meta_dtor(চাবি, strlen(চাবি) + 1, meta_তথ্য);
    ফেরত CURLE_OUT_OF_MEMORY;
  }
  ফেরত CURLE_OK;
}

শূন্য Curl_meta_remove(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *চাবি)
{
  Curl_hash_delete(&তথ্য->meta_hash, চাবি, strlen(চাবি) + 1);
}

শূন্য *Curl_meta_get(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *চাবি)
{
  ফেরত Curl_hash_pick(&তথ্য->meta_hash, চাবি, strlen(চাবি) + 1);
}

শূন্য Curl_meta_reset(কাঠামো Curl_easy *তথ্য)
{
  Curl_hash_clean(&তথ্য->meta_hash);
}
