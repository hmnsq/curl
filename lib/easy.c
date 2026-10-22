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

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "api.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "vdns/hostip.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "vquic/vquic.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "getinfo.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "easyif.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "multi_ev.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "connect.h" /* for Curl_getconnectinfo */
#அந்தர்பாவ "slist.h"
#அந்தர்பாவ "mime.h"
#அந்தர்பாவ "amigaos.h"
#அந்தர்பாவ "macos.h"
#அந்தர்பாவ "curlx/wait.h"
#அந்தர்பாவ "sigpipe.h"
#அந்தர்பாவ "vssh/ssh.h"
#அந்தர்பாவ "setopt.h"
#அந்தர்பாவ "http_digest.h"
#அந்தர்பாவ "curlx/dynbuf.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "altsvc.h"
#அந்தர்பாவ "hsts.h"
#அந்தர்பாவ "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#அந்தர்பாவ "curlx/timeval.h" /* for curlx_now_init() */
#அந்தர்பாவ "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#அந்தர்பாவ "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
நிலையான unsigned முழு initialized;
#ifdef USE_WINSOCK
நிலையான bool winsock_initialized;
#endif

#ifdef GLOBAL_INIT_IS_THREADSAFE

நிலையான curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#define global_init_lock()   curl_simple_lock_lock(&s_lock)
#define global_init_unlock() curl_simple_lock_unlock(&s_lock)

#இல்லையெனில்

#define global_init_lock()
#define global_init_unlock()

#endif

#எனில் defined(_MSC_VER) && defined(_DLL)
#  pragma warning(தள்ளு)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)malloc;
curl_free_callback Curl_cfree = (curl_free_callback)விடுவி;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_கீழ்;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#எனில் defined(_MSC_VER) && defined(_DLL)
#  pragma warning(எடு)
#endif

#ifdef DEBUGBUILD
நிலையான எழுத்து *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
நிலையான CURLcode global_init(long flags, bool memoryfuncs)
{
  (வெறுமை)flags;

  எனில்(initialized++)
    திரும்பு CURLE_OK;

  எனில்(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)விடுவி;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_கீழ்;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  எனில்(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    செல் fail;
  }

#ifdef USE_WINDOWS_SSPI
  எனில்(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    செல் fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  எனில்(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    எனில்(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      செல் fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  எனில்(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    செல் fail;
  }

  எனில்(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    செல் fail;
  }

  எனில்(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    செல் fail;
  }

  எனில்(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    செல் fail;
  }

  எனில்(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    செல் fail;
  }

  எனில்(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    செல் fail;
  }

  எனில்(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    செல் fail;
  }

#ifdef DEBUGBUILD
  எனில்(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  திரும்பு CURLE_OK;

fail:
#ifdef USE_WINSOCK
  எனில்(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  திரும்பு CURLE_FAILED_INIT;
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

  திரும்பு result;
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
  எனில்(!m || !f || !r || !s || !c)
    திரும்பு CURLE_FAILED_INIT;

  global_init_lock();

  எனில்(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    திரும்பு CURLE_OK;
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

  திரும்பு result;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
வெறுமை curl_global_cleanup(வெறுமை)
{
  global_init_lock();

  எனில்(!initialized) {
    global_init_unlock();
    திரும்பு;
  }

  எனில்(--initialized) {
    global_init_unlock();
    திரும்பு;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ifdef USE_WINSOCK
  எனில்(winsock_initialized) {
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
CURLcode curl_global_trace(மாறிலி எழுத்து *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode result;
  global_init_lock();

  result = Curl_trc_opt(config);

  global_init_unlock();

  திரும்பு result;
#இல்லையெனில்
  (வெறுமை)config;
  திரும்பு CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, மாறிலி எழுத்து *name,
                              மாறிலி curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  திரும்பு rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(வெறுமை)
{
  CURLcode result;
  கட்டமைப்பு Curl_easy *data;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  எனில்(!initialized) {
    result = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    எனில்(result) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      திரும்பு வெற்று;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  result = Curl_open(&data);
  எனில்(result) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    திரும்பு வெற்று;
  }

  திரும்பு data;
}

#ifdef DEBUGBUILD

கட்டமைப்பு socketmonitor {
  கட்டமைப்பு socketmonitor *next; /* the next node in the list or NULL */
  கட்டமைப்பு pollfd socket; /* socket info of what to monitor */
};

கட்டமைப்பு events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  முழு இலக்கம்_sockets;      /* number of nodes in the monitor list */
  கட்டமைப்பு socketmonitor *list; /* list of sockets to monitor */
  முழு running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
நிலையான முழு events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        வெறுமை *userp)     /* private callback pointer */
{
  கட்டமைப்பு events *ev = userp;
  (வெறுமை)multi;
#எனில் DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  திரும்பு 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
நிலையான முழு poll2cselect(முழு pollmask)
{
  முழு omask = 0;
  எனில்(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  எனில்(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  எனில்(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  திரும்பு omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
நிலையான short socketcb2poll(முழு pollmask)
{
  short omask = 0;
  எனில்(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  எனில்(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  திரும்பு omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
நிலையான முழு events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         முழு what,        /* see above */
                         வெறுமை *userp,     /* private callback
                                             pointer */
                         வெறுமை *socketp)   /* private socket
                                             pointer */
{
  கட்டமைப்பு events *ev = userp;
  கட்டமைப்பு socketmonitor *m;
  கட்டமைப்பு socketmonitor *prev = வெற்று;
  bool found = FALSE;
  கட்டமைப்பு Curl_easy *data = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (வெறுமை)easy;
#endif
  (வெறுமை)socketp;

  m = ev->list;
  போது(m) {
    எனில்(m->socket.fd == s) {
      found = TRUE;
      எனில்(what == CURL_POLL_REMOVE) {
        கட்டமைப்பு socketmonitor *nxt = m->next;
        /* remove this node from the list of monitored sockets */
        எனில்(prev)
          prev->next = nxt;
        இல்லையெனில்
          ev->list = nxt;
        curlx_free(m);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      இல்லையெனில் {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      நிறுத்து;
    }
    prev = m;
    m = m->next; /* move to next node */
  }

  எனில்(!found) {
    எனில்(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(data, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    இல்லையெனில் {
      m = curlx_malloc(அளவு(கட்டமைப்பு socketmonitor));
      எனில்(m) {
        m->next = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(data, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      இல்லையெனில்
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
  }

  திரும்பு 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
நிலையான வெறுமை events_setup(கட்டமைப்பு Curl_multi *multi, கட்டமைப்பு events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

நிலையான CURLcode populate_cpfds(கட்டமைப்பு curl_pollfds *cpfds, கட்டமைப்பு events *ev)
{
  கட்டமைப்பு socketmonitor *m;
  CURLcode result = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  க்கு(m = ev->list; m && !result; m = m->next) {
    result = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#எனில் DEBUG_EV_POLL
    எனில்(!result)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  திரும்பு result;
}

நிலையான CURLcode poll_cpfds(கட்டமைப்பு events *ev,
                           கட்டமைப்பு curl_pollfds *cpfds,
                           முழு *pollrc)
{
  எனில்(cpfds->n) {
    /* wait for activity or timeout */
#எனில் DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#endif
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#எனில் DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#endif
    எனில்(*pollrc < 0)
      திரும்பு CURLE_UNRECOVERABLE_POLL;
  }
  இல்லையெனில் {
#எனில் DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    எனில்(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  திரும்பு CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
நிலையான CURLcode wait_or_timeout(கட்டமைப்பு Curl_multi *multi, கட்டமைப்பு events *ev)
{
  கட்டமைப்பு pollfd fds_on_அடுக்கு[8];
  கட்டமைப்பு curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_அடுக்கு, CURL_ARRAYSIZE(fds_on_அடுக்கு));
  போது(!done) {
    CURLMsg *செய்தி;
    முழு pollrc;
    கட்டமைப்பு curltime start;

    result = populate_cpfds(&cpfds, ev);
    எனில்(result)
      செல் out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    result = poll_cpfds(ev, &cpfds, &pollrc);
    எனில்(result)
      செல் out;

    ev->msbump = FALSE; /* reset here */

    எனில்(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#எனில் DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#endif
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    இல்லையெனில் {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      unsigned முழு i;
      க்கு(i = 0; i < cpfds.n; i++) {
        எனில்(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          முழு act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      எனில்(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        எனில்(spent_ms > 0) {
#எனில் DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (long)spent_ms);
#endif
          எனில்(spent_ms > ev->ms)
            ev->ms = 0;
          இல்லையெனில்
            ev->ms -= (long)spent_ms;
        }
      }
    }

    எனில்(mresult) {
      result = CURLE_URL_MALFORMAT;
      செல் out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    செய்தி = curl_multi_info_read(multi, &pollrc);
    எனில்(செய்தி) {
      result = செய்தி->data.result;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  திரும்பு result;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
நிலையான CURLcode easy_events(கட்டமைப்பு Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  நிலையான கட்டமைப்பு events evs = { -1, FALSE, 0, வெற்று, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  திரும்பு wait_or_timeout(multi, &evs);
}
#இல்லையெனில் /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

நிலையான CURLcode easy_transfer(கட்டமைப்பு Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  போது(!done && !mresult) {
    முழு still_running = 0;

    mresult = curl_multi_poll(multi, வெற்று, 0, 1000, வெற்று);

    எனில்(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    எனில்(!mresult && !still_running) {
      முழு rc;
      CURLMsg *செய்தி = curl_multi_info_read(multi, &rc);
      எனில்(செய்தி) {
        result = செய்தி->data.result;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  எனில்(mresult) {
    result = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  திரும்பு result;
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
நிலையான CURLcode easy_perform(கட்டமைப்பு Curl_easy *data, bool events)
{
  கட்டமைப்பு Curl_multi *multi;
  CURLMcode mresult;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு Curl_sigpipe_ctx sigpipe_ctx;

  எனில்(!data)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(data->set.errorbuffer)
    /* clear this as early as possible */
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  எனில்(data->multi) {
    failf(data, "easy handle already used in multi handle");
    திரும்பு CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  எனில்(data->conn) {
    கட்டமைப்பு connectdata *conn = data->conn;
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
    DEBUGASSERT(!data->conn);
  }

  எனில்(data->multi_easy)
    multi = data->multi_easy;
  இல்லையெனில் {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    எனில்(!multi)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  எனில்(Curl_api_multi_is_in_callback(multi))
    திரும்பு CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)data->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)data->set.quick_exit);

  data->multi_easy = வெற்று; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, data);
  எனில்(mresult) {
    curl_multi_cleanup(multi);
    எனில்(mresult == CURLM_OUT_OF_MEMORY)
      திரும்பு CURLE_OUT_OF_MEMORY;
    திரும்பு CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  data->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(data, &sigpipe_ctx);

  /* run the transfer */
  result = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (வெறுமை)Curl_multi_remove_handle(multi, data);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  திரும்பு result;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  கட்டமைப்பு Curl_eapi_guard guard = { 0 };
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_perform, &result)) {
    result = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(கட்டமைப்பு Curl_easy *easy)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &result)) {
    result = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
வெறுமை curl_easy_cleanup(CURL *curl)
{
  கட்டமைப்பு Curl_eapi_guard guard;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, வெற்று)) {
    கட்டமைப்பு Curl_easy *data = curl;
    கட்டமைப்பு Curl_sigpipe_ctx sigpipe_ctx;
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
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &result)) {
    கட்டமைப்பு Curl_easy *data = curl;
    va_list arg;
    வெறுமை *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, வெறுமை *);

    result = Curl_getinfo(data, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}

நிலையான CURLcode dupset(கட்டமைப்பு Curl_easy *dst, கட்டமைப்பு Curl_easy *src)
{
  CURLcode result = CURLE_OK;
  எண்ணிக்கை dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#எனில் !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = வெற்று;
#endif
  dst->set.str_copypostfields = வெற்று;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * அளவு(கட்டமைப்பு curl_blob *));

  /* duplicate all strings */
  result = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  எனில்(result)
    திரும்பு result;

  /* duplicate all blobs */
  க்கு(j = (எண்ணிக்கை dupblob)0; j < BLOB_LAST; j++) {
    result = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    எனில்(result)
      திரும்பு result;
  }

  /* duplicate memory areas pointed to */
  எனில்(src->set.str_copypostfields) {
    எனில்(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    இல்லையெனில்
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    எனில்(!dst->set.str_copypostfields)
      திரும்பு CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#எனில் !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  எனில்(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(அளவு(*dst->set.mimepostp));
    எனில்(!dst->set.mimepostp)
      திரும்பு CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    result = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    எனில்(result)
      திரும்பு result;
  }
#endif

  எனில்(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  திரும்பு result;
}

நிலையான வெறுமை dupeasy_meta_freeentry(வெறுமை *p)
{
  (வெறுமை)p;
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
  கட்டமைப்பு Curl_eapi_guard guard;
  கட்டமைப்பு Curl_easy *outcurl = வெற்று;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, வெற்று)) {
    கட்டமைப்பு Curl_easy *data = curl;

    outcurl = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_easy));
    எனில்(!outcurl)
      செல் fail;

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
    Curl_llist_init(&outcurl->state.httphdrs, வெற்று);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    எனில்(dupset(outcurl, data))
      செல் fail;

    outcurl->progress.hide     = data->progress.hide;
    outcurl->progress.callback = data->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = வெற்று;
    எனில்(data->cookies && data->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      எனில்(!outcurl->cookies)
        செல் fail;
      outcurl->state.cookie_engine = TRUE;
    }

    எனில்(data->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(data->state.cookielist);
      எனில்(!outcurl->state.cookielist)
        செல் fail;
    }
#endif

    எனில்(Curl_bufref_ptr(&data->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&data->state.url), 0,
                      curl_free);
      எனில்(!Curl_bufref_ptr(&outcurl->state.url))
        செல் fail;
    }
    எனில்(Curl_bufref_ptr(&data->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&data->state.referer), 0,
                      curl_free);
      எனில்(!Curl_bufref_ptr(&outcurl->state.referer))
        செல் fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    எனில்(data->asi) {
      மாறிலி எழுத்து *str;
      outcurl->asi = Curl_altsvc_init();
      எனில்(!outcurl->asi)
        செல் fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      எனில்(str)
        (வெறுமை)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    எனில்(data->hsts) {
      மாறிலி எழுத்து *str;
      outcurl->hsts = Curl_hsts_init();
      எனில்(!outcurl->hsts)
        செல் fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      எனில்(str)
        (வெறுமை)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (வெறுமை)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      எனில்(Curl_hsts_copy(outcurl->hsts, data->hsts))
        செல் fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_எண்;
  }
  CURL_EAPI_LEAVE(&guard);
  திரும்பு outcurl;

fail:
  எனில்(outcurl) {
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
  திரும்பு வெற்று;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
வெறுமை curl_easy_reset(CURL *curl)
{
  கட்டமைப்பு Curl_eapi_guard guard;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_reset, வெற்று)) {
    கட்டமைப்பு Curl_easy *data = curl;

    data->state.last_conn_id = -1; /* clear remembered connection id */
    data->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&data->req, data);
    Curl_hash_clean(&data->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(data);
    /* zero out UserDefined data: */
    Curl_freeset(data);
    memset(&data->set, 0, அளவு(கட்டமைப்பு UserDefined));
    Curl_init_userdefined(data);

    /* zero out Progress data: */
    memset(&data->progress, 0, அளவு(கட்டமைப்பு Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(data);

    data->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&data->state.authhost, 0, அளவு(கட்டமைப்பு auth));
    memset(&data->state.authproxy, 0, அளவு(கட்டமைப்பு auth));

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
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
CURLcode curl_easy_pause(CURL *curl, முழு action)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_pause, &result)) {
    bool changed = FALSE;
    கட்டமைப்பு Curl_easy *data = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    எனில்(!data->conn) {
      /* crazy input, do not continue */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் out;
    }

    recv_paused = Curl_xfer_recv_is_paused(data);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(data);
    send_paused_new = (action & CURLPAUSE_SEND);

    எனில்((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(data))) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_send(data, send_paused_new));
    }

    எனில்(recv_paused != recv_paused_new) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_recv(data, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    எனில்(!Curl_xfer_is_blocked(data)) {
      /* reset the too-slow time keeper */
      data->state.keeps_speed.tv_sec = 0;
      எனில்(data->multi) {
        Curl_multi_mark_dirty(data); /* make it run */
        /* On changes, tell application to update its timers. */
        எனில்(changed && Curl_update_timer(data->multi) && !result)
          result = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    எனில்(!result && changed && !data->state.done && data->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(data->multi, data))
      result = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}

நிலையான CURLcode easy_connection(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata **connp)
{
  curl_socket_t sfd;

  எனில்(!data)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  எனில்(!data->set.connect_only) {
    failf(data, "CONNECT_ONLY is required");
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(data, connp);

  எனில்(sfd == CURL_SOCKET_BAD) {
    failf(data, "Failed to get last socket used for connection #%" FMT_OFF_T,
          data->state.last_conn_id);
    திரும்பு CURLE_UNSUPPORTED_PROTOCOL;
  }

  திரும்பு CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(கட்டமைப்பு Curl_easy *data,
                        வெறுமை *buffer, size_t buflen, size_t *n)
{
  CURLcode result;
  கட்டமைப்பு connectdata *c;

  result = easy_connection(data, &c);
  எனில்(result)
    திரும்பு result;

  எனில்(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  *n = 0;
  திரும்பு Curl_conn_recv(data, FIRSTSOCKET, buffer, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, வெறுமை *buffer, size_t buflen, size_t *n)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_recv, &result)) {
    எனில்(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் out;
    }
    result = Curl_easy_recv(curl, buffer, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(கட்டமைப்பு Curl_easy *data)
{
  CURLcode result;
  கட்டமைப்பு connectdata *c = வெற்று;

  result = easy_connection(data, &c);
  எனில்(result)
    திரும்பு result;

  எனில்(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  திரும்பு CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(கட்டமைப்பு Curl_easy *data, மாறிலி வெறுமை *buffer,
                       size_t buflen, size_t *n)
{
  CURLcode result;
  கட்டமைப்பு connectdata *c = வெற்று;
  கட்டமைப்பு Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  result = easy_connection(data, &c);
  எனில்(result)
    திரும்பு result;

  எனில்(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  sigpipe_ignore(data, &sigpipe_ctx);
  result = Curl_conn_send(data, FIRSTSOCKET, buffer, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  எனில்(result && result != CURLE_AGAIN)
    திரும்பு CURLE_SEND_ERROR;
  திரும்பு result;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, மாறிலி வெறுமை *buffer, size_t buflen,
                        size_t *n)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_send, &result)) {
    கட்டமைப்பு Curl_easy *data = curl;
    size_t written = 0;

    எனில்(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் out;
    }
    result = Curl_senddata(data, buffer, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &result)) {
    /* Use the common function to keep connections alive. */
    result = Curl_cpool_upkeep((கட்டமைப்பு Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
}

CURLcode curl_easy_ssls_import(CURL *curl, மாறிலி எழுத்து *session_சாவி,
                               மாறிலி unsigned எழுத்து *shmac, size_t shmac_len,
                               மாறிலி unsigned எழுத்து *sdata, size_t sdata_len)
{
#எனில் defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &result)) {
    எனில்(!sdata || !sdata_len)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      result = Curl_ssl_session_import((கட்டமைப்பு Curl_easy *)curl, session_சாவி,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
#இல்லையெனில்
  (வெறுமை)curl;
  (வெறுமை)session_சாவி;
  (வெறுமை)shmac;
  (வெறுமை)shmac_len;
  (வெறுமை)sdata;
  (வெறுமை)sdata_len;
  திரும்பு CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               வெறுமை *userptr)
{
#எனில் defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  கட்டமைப்பு Curl_eapi_guard guard;
  CURLcode result;

  எனில்(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &result)) {
    எனில்(!export_fn)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    இல்லையெனில்
      result = Curl_ssl_session_export((கட்டமைப்பு Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  திரும்பு result;
#இல்லையெனில்
  (வெறுமை)curl;
  (வெறுமை)export_fn;
  (வெறுமை)userptr;
  திரும்பு CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *சாவி,
                       வெறுமை *meta_data, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_data); /* never set to NULL */
  எனில்(!Curl_hash_add2(&data->meta_hash, சாவி, strlen(சாவி) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(சாவி, strlen(சாவி) + 1, meta_data);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }
  திரும்பு CURLE_OK;
}

வெறுமை Curl_meta_remove(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *சாவி)
{
  Curl_hash_delete(&data->meta_hash, சாவி, strlen(சாவி) + 1);
}

வெறுமை *Curl_meta_get(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *சாவி)
{
  திரும்பு Curl_hash_pick(&data->meta_hash, சாவி, strlen(சாவி) + 1);
}

வெறுமை Curl_meta_reset(கட்டமைப்பு Curl_easy *data)
{
  Curl_hash_clean(&data->meta_hash);
}
