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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাৱ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাৱ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাৱ <sys/param.h>
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "api.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "vdns/hostip.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "vtls/vtls_scache.h"
#অন্তর্ভাৱ "vquic/vquic.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "getinfo.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "easyif.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "multi_ev.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "connect.h" /* for Curl_getconnectinfo */
#অন্তর্ভাৱ "slist.h"
#অন্তর্ভাৱ "mime.h"
#অন্তর্ভাৱ "amigaos.h"
#অন্তর্ভাৱ "macos.h"
#অন্তর্ভাৱ "curlx/wait.h"
#অন্তর্ভাৱ "sigpipe.h"
#অন্তর্ভাৱ "vssh/ssh.h"
#অন্তর্ভাৱ "setopt.h"
#অন্তর্ভাৱ "http_digest.h"
#অন্তর্ভাৱ "curlx/dynbuf.h"
#অন্তর্ভাৱ "bufref.h"
#অন্তর্ভাৱ "altsvc.h"
#অন্তর্ভাৱ "hsts.h"
#অন্তর্ভাৱ "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#অন্তর্ভাৱ "curlx/timeval.h" /* for curlx_now_init() */
#অন্তর্ভাৱ "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#অন্তর্ভাৱ "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
স্থির unsigned পূর্ণাঙ্ক initialized;
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
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_নিচেৰ;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#যদি defined(_MSC_VER) && defined(_DLL)
#  pragma warning(বেৰ_কৰো)
#endif

#ifdef DEBUGBUILD
স্থির ৱর্ণ *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
স্থির CURLcode global_init(long flags, bool memoryfuncs)
{
  (শূন্য)flags;

  যদি(initialized++)
    ঘূৰাই CURLE_OK;

  যদি(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)মুক্ত;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_নিচেৰ;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  যদি(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    গচ্ছ fail;
  }

#ifdef USE_WINDOWS_SSPI
  যদি(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    গচ্ছ fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  যদি(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    যদি(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      গচ্ছ fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  যদি(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    গচ্ছ fail;
  }

  যদি(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    গচ্ছ fail;
  }

  যদি(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    গচ্ছ fail;
  }

  যদি(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    গচ্ছ fail;
  }

  যদি(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    গচ্ছ fail;
  }

  যদি(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    গচ্ছ fail;
  }

  যদি(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    গচ্ছ fail;
  }

#ifdef DEBUGBUILD
  যদি(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  ঘূৰাই CURLE_OK;

fail:
#ifdef USE_WINSOCK
  যদি(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  ঘূৰাই CURLE_FAILED_INIT;
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

  ঘূৰাই result;
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
  যদি(!m || !f || !r || !s || !c)
    ঘূৰাই CURLE_FAILED_INIT;

  global_init_lock();

  যদি(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    ঘূৰাই CURLE_OK;
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

  ঘূৰাই result;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
শূন্য curl_global_cleanup(শূন্য)
{
  global_init_lock();

  যদি(!initialized) {
    global_init_unlock();
    ঘূৰাই;
  }

  যদি(--initialized) {
    global_init_unlock();
    ঘূৰাই;
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
CURLcode curl_global_trace(ধ্রুৱ ৱর্ণ *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode result;
  global_init_lock();

  result = Curl_trc_opt(config);

  global_init_unlock();

  ঘূৰাই result;
#অন্যথা
  (শূন্য)config;
  ঘূৰাই CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ধ্রুৱ ৱর্ণ *name,
                              ধ্রুৱ curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  ঘূৰাই rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(শূন্য)
{
  CURLcode result;
  সংরচনা Curl_easy *data;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  যদি(!initialized) {
    result = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    যদি(result) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      ঘূৰাই রিক্ত;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  result = Curl_open(&data);
  যদি(result) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    ঘূৰাই রিক্ত;
  }

  ঘূৰাই data;
}

#ifdef DEBUGBUILD

সংরচনা socketmonitor {
  সংরচনা socketmonitor *next; /* the next node in the list or NULL */
  সংরচনা pollfd socket; /* socket info of what to monitor */
};

সংরচনা events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  পূর্ণাঙ্ক অঙ্ক_sockets;      /* number of nodes in the monitor list */
  সংরচনা socketmonitor *list; /* list of sockets to monitor */
  পূর্ণাঙ্ক running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
স্থির পূর্ণাঙ্ক events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        শূন্য *userp)     /* private callback pointer */
{
  সংরচনা events *ev = userp;
  (শূন্য)multi;
#যদি DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  ঘূৰাই 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
স্থির পূর্ণাঙ্ক poll2cselect(পূর্ণাঙ্ক pollmask)
{
  পূর্ণাঙ্ক omask = 0;
  যদি(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  যদি(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  যদি(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  ঘূৰাই omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
স্থির short socketcb2poll(পূর্ণাঙ্ক pollmask)
{
  short omask = 0;
  যদি(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  যদি(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  ঘূৰাই omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
স্থির পূর্ণাঙ্ক events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         পূর্ণাঙ্ক what,        /* see above */
                         শূন্য *userp,     /* private callback
                                             pointer */
                         শূন্য *socketp)   /* private socket
                                             pointer */
{
  সংরচনা events *ev = userp;
  সংরচনা socketmonitor *m;
  সংরচনা socketmonitor *prev = রিক্ত;
  bool found = FALSE;
  সংরচনা Curl_easy *data = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (শূন্য)easy;
#endif
  (শূন্য)socketp;

  m = ev->list;
  যেতিয়ালৈ(m) {
    যদি(m->socket.fd == s) {
      found = TRUE;
      যদি(what == CURL_POLL_REMOVE) {
        সংরচনা socketmonitor *nxt = m->next;
        /* remove this node from the list of monitored sockets */
        যদি(prev)
          prev->next = nxt;
        অন্যথা
          ev->list = nxt;
        curlx_free(m);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      অন্যথা {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      ভঙ্গ;
    }
    prev = m;
    m = m->next; /* move to next node */
  }

  যদি(!found) {
    যদি(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(data, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    অন্যথা {
      m = curlx_malloc(পরিমাণ(সংরচনা socketmonitor));
      যদি(m) {
        m->next = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(data, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      অন্যথা
        ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
  }

  ঘূৰাই 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
স্থির শূন্য events_setup(সংরচনা Curl_multi *multi, সংরচনা events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

স্থির CURLcode populate_cpfds(সংরচনা curl_pollfds *cpfds, সংরচনা events *ev)
{
  সংরচনা socketmonitor *m;
  CURLcode result = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  বাবে(m = ev->list; m && !result; m = m->next) {
    result = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#যদি DEBUG_EV_POLL
    যদি(!result)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  ঘূৰাই result;
}

স্থির CURLcode poll_cpfds(সংরচনা events *ev,
                           সংরচনা curl_pollfds *cpfds,
                           পূর্ণাঙ্ক *pollrc)
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
      ঘূৰাই CURLE_UNRECOVERABLE_POLL;
  }
  অন্যথা {
#যদি DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    যদি(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  ঘূৰাই CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
স্থির CURLcode wait_or_timeout(সংরচনা Curl_multi *multi, সংরচনা events *ev)
{
  সংরচনা pollfd fds_on_স্তূপ[8];
  সংরচনা curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_স্তূপ, CURL_ARRAYSIZE(fds_on_স্তূপ));
  যেতিয়ালৈ(!done) {
    CURLMsg *বাৰ্তা;
    পূর্ণাঙ্ক pollrc;
    সংরচনা curltime start;

    result = populate_cpfds(&cpfds, ev);
    যদি(result)
      গচ্ছ out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    result = poll_cpfds(ev, &cpfds, &pollrc);
    যদি(result)
      গচ্ছ out;

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
      unsigned পূর্ণাঙ্ক i;
      বাবে(i = 0; i < cpfds.n; i++) {
        যদি(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          পূর্ণাঙ্ক act = poll2cselect(cpfds.pfds[i].revents); /* convert */

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
      result = CURLE_URL_MALFORMAT;
      গচ্ছ out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    বাৰ্তা = curl_multi_info_read(multi, &pollrc);
    যদি(বাৰ্তা) {
      result = বাৰ্তা->data.result;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  ঘূৰাই result;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
স্থির CURLcode easy_events(সংরচনা Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  স্থির সংরচনা events evs = { -1, FALSE, 0, রিক্ত, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  ঘূৰাই wait_or_timeout(multi, &evs);
}
#অন্যথা /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

স্থির CURLcode easy_transfer(সংরচনা Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  যেতিয়ালৈ(!done && !mresult) {
    পূর্ণাঙ্ক still_running = 0;

    mresult = curl_multi_poll(multi, রিক্ত, 0, 1000, রিক্ত);

    যদি(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    যদি(!mresult && !still_running) {
      পূর্ণাঙ্ক rc;
      CURLMsg *বাৰ্তা = curl_multi_info_read(multi, &rc);
      যদি(বাৰ্তা) {
        result = বাৰ্তা->data.result;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  যদি(mresult) {
    result = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  ঘূৰাই result;
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
স্থির CURLcode easy_perform(সংরচনা Curl_easy *data, bool events)
{
  সংরচনা Curl_multi *multi;
  CURLMcode mresult;
  CURLcode result = CURLE_OK;
  সংরচনা Curl_sigpipe_ctx sigpipe_ctx;

  যদি(!data)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(data->set.errorbuffer)
    /* clear this as early as possible */
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  যদি(data->multi) {
    failf(data, "easy handle already used in multi handle");
    ঘূৰাই CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  যদি(data->conn) {
    সংরচনা connectdata *conn = data->conn;
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
    DEBUGASSERT(!data->conn);
  }

  যদি(data->multi_easy)
    multi = data->multi_easy;
  অন্যথা {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    যদি(!multi)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  যদি(Curl_api_multi_is_in_callback(multi))
    ঘূৰাই CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)data->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)data->set.quick_exit);

  data->multi_easy = রিক্ত; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, data);
  যদি(mresult) {
    curl_multi_cleanup(multi);
    যদি(mresult == CURLM_OUT_OF_MEMORY)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    ঘূৰাই CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  data->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(data, &sigpipe_ctx);

  /* run the transfer */
  result = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (শূন্য)Curl_multi_remove_handle(multi, data);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  ঘূৰাই result;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  সংরচনা Curl_eapi_guard guard = { 0 };
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_perform, &result)) {
    result = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই result;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(সংরচনা Curl_easy *easy)
{
  সংরচনা Curl_eapi_guard guard;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &result)) {
    result = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই result;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
শূন্য curl_easy_cleanup(CURL *curl)
{
  সংরচনা Curl_eapi_guard guard;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, রিক্ত)) {
    সংরচনা Curl_easy *data = curl;
    সংরচনা Curl_sigpipe_ctx sigpipe_ctx;
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
  সংরচনা Curl_eapi_guard guard;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &result)) {
    সংরচনা Curl_easy *data = curl;
    va_list arg;
    শূন্য *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, শূন্য *);

    result = Curl_getinfo(data, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই result;
}

স্থির CURLcode dupset(সংরচনা Curl_easy *dst, সংরচনা Curl_easy *src)
{
  CURLcode result = CURLE_OK;
  গণনা dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = রিক্ত;
#endif
  dst->set.str_copypostfields = রিক্ত;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * পরিমাণ(সংরচনা curl_blob *));

  /* duplicate all strings */
  result = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  যদি(result)
    ঘূৰাই result;

  /* duplicate all blobs */
  বাবে(j = (গণনা dupblob)0; j < BLOB_LAST; j++) {
    result = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    যদি(result)
      ঘূৰাই result;
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
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#যদি !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  যদি(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(পরিমাণ(*dst->set.mimepostp));
    যদি(!dst->set.mimepostp)
      ঘূৰাই CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    result = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    যদি(result)
      ঘূৰাই result;
  }
#endif

  যদি(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  ঘূৰাই result;
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
  সংরচনা Curl_eapi_guard guard;
  সংরচনা Curl_easy *outcurl = রিক্ত;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, রিক্ত)) {
    সংরচনা Curl_easy *data = curl;

    outcurl = curlx_calloc(1, পরিমাণ(সংরচনা Curl_easy));
    যদি(!outcurl)
      গচ্ছ fail;

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
    Curl_llist_init(&outcurl->state.httphdrs, রিক্ত);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    যদি(dupset(outcurl, data))
      গচ্ছ fail;

    outcurl->progress.hide     = data->progress.hide;
    outcurl->progress.callback = data->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = রিক্ত;
    যদি(data->cookies && data->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      যদি(!outcurl->cookies)
        গচ্ছ fail;
      outcurl->state.cookie_engine = TRUE;
    }

    যদি(data->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(data->state.cookielist);
      যদি(!outcurl->state.cookielist)
        গচ্ছ fail;
    }
#endif

    যদি(Curl_bufref_ptr(&data->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&data->state.url), 0,
                      curl_free);
      যদি(!Curl_bufref_ptr(&outcurl->state.url))
        গচ্ছ fail;
    }
    যদি(Curl_bufref_ptr(&data->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&data->state.referer), 0,
                      curl_free);
      যদি(!Curl_bufref_ptr(&outcurl->state.referer))
        গচ্ছ fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    যদি(data->asi) {
      ধ্রুৱ ৱর্ণ *str;
      outcurl->asi = Curl_altsvc_init();
      যদি(!outcurl->asi)
        গচ্ছ fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      যদি(str)
        (শূন্য)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    যদি(data->hsts) {
      ধ্রুৱ ৱর্ণ *str;
      outcurl->hsts = Curl_hsts_init();
      যদি(!outcurl->hsts)
        গচ্ছ fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      যদি(str)
        (শূন্য)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (শূন্য)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      যদি(Curl_hsts_copy(outcurl->hsts, data->hsts))
        গচ্ছ fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_সংখ্যা;
  }
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই outcurl;

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
  ঘূৰাই রিক্ত;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
শূন্য curl_easy_reset(CURL *curl)
{
  সংরচনা Curl_eapi_guard guard;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_reset, রিক্ত)) {
    সংরচনা Curl_easy *data = curl;

    data->state.last_conn_id = -1; /* clear remembered connection id */
    data->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&data->req, data);
    Curl_hash_clean(&data->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(data);
    /* zero out UserDefined data: */
    Curl_freeset(data);
    memset(&data->set, 0, পরিমাণ(সংরচনা UserDefined));
    Curl_init_userdefined(data);

    /* zero out Progress data: */
    memset(&data->progress, 0, পরিমাণ(সংরচনা Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(data);

    data->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&data->state.authhost, 0, পরিমাণ(সংরচনা auth));
    memset(&data->state.authproxy, 0, পরিমাণ(সংরচনা auth));

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
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
CURLcode curl_easy_pause(CURL *curl, পূর্ণাঙ্ক action)
{
  সংরচনা Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_pause, &result)) {
    bool changed = FALSE;
    সংরচনা Curl_easy *data = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    যদি(!data->conn) {
      /* crazy input, do not continue */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      গচ্ছ out;
    }

    recv_paused = Curl_xfer_recv_is_paused(data);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(data);
    send_paused_new = (action & CURLPAUSE_SEND);

    যদি((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(data))) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_send(data, send_paused_new));
    }

    যদি(recv_paused != recv_paused_new) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_recv(data, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    যদি(!Curl_xfer_is_blocked(data)) {
      /* reset the too-slow time keeper */
      data->state.keeps_speed.tv_sec = 0;
      যদি(data->multi) {
        Curl_multi_mark_dirty(data); /* make it run */
        /* On changes, tell application to update its timers. */
        যদি(changed && Curl_update_timer(data->multi) && !result)
          result = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    যদি(!result && changed && !data->state.done && data->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(data->multi, data))
      result = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই result;
}

স্থির CURLcode easy_connection(সংরচনা Curl_easy *data,
                                সংরচনা connectdata **connp)
{
  curl_socket_t sfd;

  যদি(!data)
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  যদি(!data->set.connect_only) {
    failf(data, "CONNECT_ONLY is required");
    ঘূৰাই CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(data, connp);

  যদি(sfd == CURL_SOCKET_BAD) {
    failf(data, "Failed to get last socket used for connection #%" FMT_OFF_T,
          data->state.last_conn_id);
    ঘূৰাই CURLE_UNSUPPORTED_PROTOCOL;
  }

  ঘূৰাই CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(সংরচনা Curl_easy *data,
                        শূন্য *buffer, size_t buflen, size_t *n)
{
  CURLcode result;
  সংরচনা connectdata *c;

  result = easy_connection(data, &c);
  যদি(result)
    ঘূৰাই result;

  যদি(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  *n = 0;
  ঘূৰাই Curl_conn_recv(data, FIRSTSOCKET, buffer, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, শূন্য *buffer, size_t buflen, size_t *n)
{
  সংরচনা Curl_eapi_guard guard;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_recv, &result)) {
    যদি(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      গচ্ছ out;
    }
    result = Curl_easy_recv(curl, buffer, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই result;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(সংরচনা Curl_easy *data)
{
  CURLcode result;
  সংরচনা connectdata *c = রিক্ত;

  result = easy_connection(data, &c);
  যদি(result)
    ঘূৰাই result;

  যদি(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  ঘূৰাই CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(সংরচনা Curl_easy *data, ধ্রুৱ শূন্য *buffer,
                       size_t buflen, size_t *n)
{
  CURLcode result;
  সংরচনা connectdata *c = রিক্ত;
  সংরচনা Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  result = easy_connection(data, &c);
  যদি(result)
    ঘূৰাই result;

  যদি(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  sigpipe_ignore(data, &sigpipe_ctx);
  result = Curl_conn_send(data, FIRSTSOCKET, buffer, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  যদি(result && result != CURLE_AGAIN)
    ঘূৰাই CURLE_SEND_ERROR;
  ঘূৰাই result;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ধ্রুৱ শূন্য *buffer, size_t buflen,
                        size_t *n)
{
  সংরচনা Curl_eapi_guard guard;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_send, &result)) {
    সংরচনা Curl_easy *data = curl;
    size_t written = 0;

    যদি(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      গচ্ছ out;
    }
    result = Curl_senddata(data, buffer, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই result;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  সংরচনা Curl_eapi_guard guard;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &result)) {
    /* Use the common function to keep connections alive. */
    result = Curl_cpool_upkeep((সংরচনা Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই result;
}

CURLcode curl_easy_ssls_import(CURL *curl, ধ্রুৱ ৱর্ণ *session_চাবি,
                               ধ্রুৱ unsigned ৱর্ণ *shmac, size_t shmac_len,
                               ধ্রুৱ unsigned ৱর্ণ *sdata, size_t sdata_len)
{
#যদি defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  সংরচনা Curl_eapi_guard guard;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &result)) {
    যদি(!sdata || !sdata_len)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      result = Curl_ssl_session_import((সংরচনা Curl_easy *)curl, session_চাবি,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই result;
#অন্যথা
  (শূন্য)curl;
  (শূন্য)session_চাবি;
  (শূন্য)shmac;
  (শূন্য)shmac_len;
  (শূন্য)sdata;
  (শূন্য)sdata_len;
  ঘূৰাই CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               শূন্য *userptr)
{
#যদি defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  সংরচনা Curl_eapi_guard guard;
  CURLcode result;

  যদি(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &result)) {
    যদি(!export_fn)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    অন্যথা
      result = Curl_ssl_session_export((সংরচনা Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  ঘূৰাই result;
#অন্যথা
  (শূন্য)curl;
  (শূন্য)export_fn;
  (শূন্য)userptr;
  ঘূৰাই CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(সংরচনা Curl_easy *data, ধ্রুৱ ৱর্ণ *চাবি,
                       শূন্য *meta_data, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_data); /* never set to NULL */
  যদি(!Curl_hash_add2(&data->meta_hash, চাবি, strlen(চাবি) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(চাবি, strlen(চাবি) + 1, meta_data);
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }
  ঘূৰাই CURLE_OK;
}

শূন্য Curl_meta_remove(সংরচনা Curl_easy *data, ধ্রুৱ ৱর্ণ *চাবি)
{
  Curl_hash_delete(&data->meta_hash, চাবি, strlen(চাবি) + 1);
}

শূন্য *Curl_meta_get(সংরচনা Curl_easy *data, ধ্রুৱ ৱর্ণ *চাবি)
{
  ঘূৰাই Curl_hash_pick(&data->meta_hash, চাবি, strlen(চাবি) + 1);
}

শূন্য Curl_meta_reset(সংরচনা Curl_easy *data)
{
  Curl_hash_clean(&data->meta_hash);
}
