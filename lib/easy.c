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
#شامل "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#شامل <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#endif

#شامل "urldata.h"
#شامل "api.h"
#شامل "transfer.h"
#شامل "vdns/hostip.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_scache.h"
#شامل "vquic/vquic.h"
#شامل "url.h"
#شامل "getinfo.h"
#شامل "curlx/strdup.h"
#شامل "easyif.h"
#شامل "multiif.h"
#شامل "multi_ev.h"
#شامل "select.h"
#شامل "cfilters.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "connect.h" /* for Curl_getconnectinfo */
#شامل "slist.h"
#شامل "mime.h"
#شامل "amigaos.h"
#شامل "macos.h"
#شامل "curlx/wait.h"
#شامل "sigpipe.h"
#شامل "vssh/ssh.h"
#شامل "setopt.h"
#شامل "http_digest.h"
#شامل "curlx/dynbuf.h"
#شامل "bufref.h"
#شامل "altsvc.h"
#شامل "hsts.h"
#شامل "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#شامل "curlx/timeval.h" /* for curlx_now_init() */
#شامل "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#شامل "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
ایستا unsigned صحیح initialized;
#ifdef USE_WINSOCK
ایستا bool winsock_initialized;
#endif

#ifdef GLOBAL_INIT_IS_THREADSAFE

ایستا curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#define global_init_lock()   curl_simple_lock_lock(&s_lock)
#define global_init_unlock() curl_simple_lock_unlock(&s_lock)

#وگرنه

#define global_init_lock()
#define global_init_unlock()

#endif

#اگر defined(_MSC_VER) && defined(_DLL)
#  pragma warning(افزودن)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)malloc;
curl_free_callback Curl_cfree = (curl_free_callback)آزاد;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_کم;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#اگر defined(_MSC_VER) && defined(_DLL)
#  pragma warning(برداشت)
#endif

#ifdef DEBUGBUILD
ایستا نویسه *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
ایستا CURLcode global_init(long flags, bool memoryfuncs)
{
  (تهی)flags;

  اگر(initialized++)
    بازگشت CURLE_OK;

  اگر(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)آزاد;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_کم;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  اگر(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    برو fail;
  }

#ifdef USE_WINDOWS_SSPI
  اگر(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    برو fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  اگر(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    اگر(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      برو fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  اگر(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    برو fail;
  }

  اگر(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    برو fail;
  }

  اگر(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    برو fail;
  }

  اگر(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    برو fail;
  }

  اگر(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    برو fail;
  }

  اگر(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    برو fail;
  }

  اگر(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    برو fail;
  }

#ifdef DEBUGBUILD
  اگر(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  بازگشت CURLE_OK;

fail:
#ifdef USE_WINSOCK
  اگر(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  بازگشت CURLE_FAILED_INIT;
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

  بازگشت result;
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
  اگر(!m || !f || !r || !s || !c)
    بازگشت CURLE_FAILED_INIT;

  global_init_lock();

  اگر(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    بازگشت CURLE_OK;
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

  بازگشت result;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
تهی curl_global_cleanup(تهی)
{
  global_init_lock();

  اگر(!initialized) {
    global_init_unlock();
    بازگشت;
  }

  اگر(--initialized) {
    global_init_unlock();
    بازگشت;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ifdef USE_WINSOCK
  اگر(winsock_initialized) {
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
CURLcode curl_global_trace(ثابت نویسه *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode result;
  global_init_lock();

  result = Curl_trc_opt(config);

  global_init_unlock();

  بازگشت result;
#وگرنه
  (تهی)config;
  بازگشت CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ثابت نویسه *name,
                              ثابت curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  بازگشت rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(تهی)
{
  CURLcode result;
  ساختار Curl_easy *data;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  اگر(!initialized) {
    result = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    اگر(result) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      بازگشت پوچ;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  result = Curl_open(&data);
  اگر(result) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    بازگشت پوچ;
  }

  بازگشت data;
}

#ifdef DEBUGBUILD

ساختار socketmonitor {
  ساختار socketmonitor *next; /* the next node in the list or NULL */
  ساختار pollfd socket; /* socket info of what to monitor */
};

ساختار events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  صحیح رقم_sockets;      /* number of nodes in the monitor list */
  ساختار socketmonitor *list; /* list of sockets to monitor */
  صحیح running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
ایستا صحیح events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        تهی *userp)     /* private callback pointer */
{
  ساختار events *ev = userp;
  (تهی)multi;
#اگر DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  بازگشت 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
ایستا صحیح poll2cselect(صحیح pollmask)
{
  صحیح omask = 0;
  اگر(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  اگر(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  اگر(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  بازگشت omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
ایستا short socketcb2poll(صحیح pollmask)
{
  short omask = 0;
  اگر(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  اگر(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  بازگشت omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
ایستا صحیح events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         صحیح what,        /* see above */
                         تهی *userp,     /* private callback
                                             pointer */
                         تهی *socketp)   /* private socket
                                             pointer */
{
  ساختار events *ev = userp;
  ساختار socketmonitor *m;
  ساختار socketmonitor *prev = پوچ;
  bool found = FALSE;
  ساختار Curl_easy *data = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (تهی)easy;
#endif
  (تهی)socketp;

  m = ev->list;
  وقتی(m) {
    اگر(m->socket.fd == s) {
      found = TRUE;
      اگر(what == CURL_POLL_REMOVE) {
        ساختار socketmonitor *nxt = m->next;
        /* remove this node from the list of monitored sockets */
        اگر(prev)
          prev->next = nxt;
        وگرنه
          ev->list = nxt;
        curlx_free(m);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      وگرنه {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      بشکن;
    }
    prev = m;
    m = m->next; /* move to next node */
  }

  اگر(!found) {
    اگر(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(data, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    وگرنه {
      m = curlx_malloc(اندازه(ساختار socketmonitor));
      اگر(m) {
        m->next = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(data, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      وگرنه
        بازگشت CURLE_OUT_OF_MEMORY;
    }
  }

  بازگشت 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
ایستا تهی events_setup(ساختار Curl_multi *multi, ساختار events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

ایستا CURLcode populate_cpfds(ساختار curl_pollfds *cpfds, ساختار events *ev)
{
  ساختار socketmonitor *m;
  CURLcode result = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  برای(m = ev->list; m && !result; m = m->next) {
    result = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#اگر DEBUG_EV_POLL
    اگر(!result)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  بازگشت result;
}

ایستا CURLcode poll_cpfds(ساختار events *ev,
                           ساختار curl_pollfds *cpfds,
                           صحیح *pollrc)
{
  اگر(cpfds->n) {
    /* wait for activity or timeout */
#اگر DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#endif
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#اگر DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#endif
    اگر(*pollrc < 0)
      بازگشت CURLE_UNRECOVERABLE_POLL;
  }
  وگرنه {
#اگر DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    اگر(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  بازگشت CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
ایستا CURLcode wait_or_timeout(ساختار Curl_multi *multi, ساختار events *ev)
{
  ساختار pollfd fds_on_پشته[8];
  ساختار curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_پشته, CURL_ARRAYSIZE(fds_on_پشته));
  وقتی(!done) {
    CURLMsg *پیام;
    صحیح pollrc;
    ساختار curltime start;

    result = populate_cpfds(&cpfds, ev);
    اگر(result)
      برو out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    result = poll_cpfds(ev, &cpfds, &pollrc);
    اگر(result)
      برو out;

    ev->msbump = FALSE; /* reset here */

    اگر(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#اگر DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#endif
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    وگرنه {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      unsigned صحیح i;
      برای(i = 0; i < cpfds.n; i++) {
        اگر(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          صحیح act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      اگر(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        اگر(spent_ms > 0) {
#اگر DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (long)spent_ms);
#endif
          اگر(spent_ms > ev->ms)
            ev->ms = 0;
          وگرنه
            ev->ms -= (long)spent_ms;
        }
      }
    }

    اگر(mresult) {
      result = CURLE_URL_MALFORMAT;
      برو out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    پیام = curl_multi_info_read(multi, &pollrc);
    اگر(پیام) {
      result = پیام->data.result;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  بازگشت result;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
ایستا CURLcode easy_events(ساختار Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  ایستا ساختار events evs = { -1, FALSE, 0, پوچ, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  بازگشت wait_or_timeout(multi, &evs);
}
#وگرنه /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

ایستا CURLcode easy_transfer(ساختار Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  وقتی(!done && !mresult) {
    صحیح still_running = 0;

    mresult = curl_multi_poll(multi, پوچ, 0, 1000, پوچ);

    اگر(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    اگر(!mresult && !still_running) {
      صحیح rc;
      CURLMsg *پیام = curl_multi_info_read(multi, &rc);
      اگر(پیام) {
        result = پیام->data.result;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  اگر(mresult) {
    result = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  بازگشت result;
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
ایستا CURLcode easy_perform(ساختار Curl_easy *data, bool events)
{
  ساختار Curl_multi *multi;
  CURLMcode mresult;
  CURLcode result = CURLE_OK;
  ساختار Curl_sigpipe_ctx sigpipe_ctx;

  اگر(!data)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  اگر(data->set.errorbuffer)
    /* clear this as early as possible */
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  اگر(data->multi) {
    failf(data, "easy handle already used in multi handle");
    بازگشت CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  اگر(data->conn) {
    ساختار connectdata *conn = data->conn;
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
    DEBUGASSERT(!data->conn);
  }

  اگر(data->multi_easy)
    multi = data->multi_easy;
  وگرنه {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    اگر(!multi)
      بازگشت CURLE_OUT_OF_MEMORY;
  }

  اگر(Curl_api_multi_is_in_callback(multi))
    بازگشت CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)data->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)data->set.quick_exit);

  data->multi_easy = پوچ; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, data);
  اگر(mresult) {
    curl_multi_cleanup(multi);
    اگر(mresult == CURLM_OUT_OF_MEMORY)
      بازگشت CURLE_OUT_OF_MEMORY;
    بازگشت CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  data->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(data, &sigpipe_ctx);

  /* run the transfer */
  result = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (تهی)Curl_multi_remove_handle(multi, data);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  بازگشت result;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  ساختار Curl_eapi_guard guard = { 0 };
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_perform, &result)) {
    result = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(ساختار Curl_easy *easy)
{
  ساختار Curl_eapi_guard guard;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &result)) {
    result = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
تهی curl_easy_cleanup(CURL *curl)
{
  ساختار Curl_eapi_guard guard;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, پوچ)) {
    ساختار Curl_easy *data = curl;
    ساختار Curl_sigpipe_ctx sigpipe_ctx;
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
  ساختار Curl_eapi_guard guard;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &result)) {
    ساختار Curl_easy *data = curl;
    va_list arg;
    تهی *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, تهی *);

    result = Curl_getinfo(data, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}

ایستا CURLcode dupset(ساختار Curl_easy *dst, ساختار Curl_easy *src)
{
  CURLcode result = CURLE_OK;
  شمارش dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = پوچ;
#endif
  dst->set.str_copypostfields = پوچ;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * اندازه(ساختار curl_blob *));

  /* duplicate all strings */
  result = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  اگر(result)
    بازگشت result;

  /* duplicate all blobs */
  برای(j = (شمارش dupblob)0; j < BLOB_LAST; j++) {
    result = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    اگر(result)
      بازگشت result;
  }

  /* duplicate memory areas pointed to */
  اگر(src->set.str_copypostfields) {
    اگر(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    وگرنه
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    اگر(!dst->set.str_copypostfields)
      بازگشت CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  اگر(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(اندازه(*dst->set.mimepostp));
    اگر(!dst->set.mimepostp)
      بازگشت CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    result = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    اگر(result)
      بازگشت result;
  }
#endif

  اگر(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  بازگشت result;
}

ایستا تهی dupeasy_meta_freeentry(تهی *p)
{
  (تهی)p;
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
  ساختار Curl_eapi_guard guard;
  ساختار Curl_easy *outcurl = پوچ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, پوچ)) {
    ساختار Curl_easy *data = curl;

    outcurl = curlx_calloc(1, اندازه(ساختار Curl_easy));
    اگر(!outcurl)
      برو fail;

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
    Curl_llist_init(&outcurl->state.httphdrs, پوچ);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    اگر(dupset(outcurl, data))
      برو fail;

    outcurl->progress.hide     = data->progress.hide;
    outcurl->progress.callback = data->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = پوچ;
    اگر(data->cookies && data->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      اگر(!outcurl->cookies)
        برو fail;
      outcurl->state.cookie_engine = TRUE;
    }

    اگر(data->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(data->state.cookielist);
      اگر(!outcurl->state.cookielist)
        برو fail;
    }
#endif

    اگر(Curl_bufref_ptr(&data->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&data->state.url), 0,
                      curl_free);
      اگر(!Curl_bufref_ptr(&outcurl->state.url))
        برو fail;
    }
    اگر(Curl_bufref_ptr(&data->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&data->state.referer), 0,
                      curl_free);
      اگر(!Curl_bufref_ptr(&outcurl->state.referer))
        برو fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    اگر(data->asi) {
      ثابت نویسه *str;
      outcurl->asi = Curl_altsvc_init();
      اگر(!outcurl->asi)
        برو fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      اگر(str)
        (تهی)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    اگر(data->hsts) {
      ثابت نویسه *str;
      outcurl->hsts = Curl_hsts_init();
      اگر(!outcurl->hsts)
        برو fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      اگر(str)
        (تهی)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (تهی)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      اگر(Curl_hsts_copy(outcurl->hsts, data->hsts))
        برو fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_عدد;
  }
  CURL_EAPI_LEAVE(&guard);
  بازگشت outcurl;

fail:
  اگر(outcurl) {
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
  بازگشت پوچ;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
تهی curl_easy_reset(CURL *curl)
{
  ساختار Curl_eapi_guard guard;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_reset, پوچ)) {
    ساختار Curl_easy *data = curl;

    data->state.last_conn_id = -1; /* clear remembered connection id */
    data->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&data->req, data);
    Curl_hash_clean(&data->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(data);
    /* zero out UserDefined data: */
    Curl_freeset(data);
    memset(&data->set, 0, اندازه(ساختار UserDefined));
    Curl_init_userdefined(data);

    /* zero out Progress data: */
    memset(&data->progress, 0, اندازه(ساختار Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(data);

    data->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&data->state.authhost, 0, اندازه(ساختار auth));
    memset(&data->state.authproxy, 0, اندازه(ساختار auth));

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
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
CURLcode curl_easy_pause(CURL *curl, صحیح action)
{
  ساختار Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_pause, &result)) {
    bool changed = FALSE;
    ساختار Curl_easy *data = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    اگر(!data->conn) {
      /* crazy input, do not continue */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      برو out;
    }

    recv_paused = Curl_xfer_recv_is_paused(data);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(data);
    send_paused_new = (action & CURLPAUSE_SEND);

    اگر((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(data))) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_send(data, send_paused_new));
    }

    اگر(recv_paused != recv_paused_new) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_recv(data, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    اگر(!Curl_xfer_is_blocked(data)) {
      /* reset the too-slow time keeper */
      data->state.keeps_speed.tv_sec = 0;
      اگر(data->multi) {
        Curl_multi_mark_dirty(data); /* make it run */
        /* On changes, tell application to update its timers. */
        اگر(changed && Curl_update_timer(data->multi) && !result)
          result = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    اگر(!result && changed && !data->state.done && data->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(data->multi, data))
      result = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}

ایستا CURLcode easy_connection(ساختار Curl_easy *data,
                                ساختار connectdata **connp)
{
  curl_socket_t sfd;

  اگر(!data)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  اگر(!data->set.connect_only) {
    failf(data, "CONNECT_ONLY is required");
    بازگشت CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(data, connp);

  اگر(sfd == CURL_SOCKET_BAD) {
    failf(data, "Failed to get last socket used for connection #%" FMT_OFF_T,
          data->state.last_conn_id);
    بازگشت CURLE_UNSUPPORTED_PROTOCOL;
  }

  بازگشت CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(ساختار Curl_easy *data,
                        تهی *buffer, size_t buflen, size_t *n)
{
  CURLcode result;
  ساختار connectdata *c;

  result = easy_connection(data, &c);
  اگر(result)
    بازگشت result;

  اگر(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  *n = 0;
  بازگشت Curl_conn_recv(data, FIRSTSOCKET, buffer, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, تهی *buffer, size_t buflen, size_t *n)
{
  ساختار Curl_eapi_guard guard;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_recv, &result)) {
    اگر(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      برو out;
    }
    result = Curl_easy_recv(curl, buffer, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(ساختار Curl_easy *data)
{
  CURLcode result;
  ساختار connectdata *c = پوچ;

  result = easy_connection(data, &c);
  اگر(result)
    بازگشت result;

  اگر(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  بازگشت CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(ساختار Curl_easy *data, ثابت تهی *buffer,
                       size_t buflen, size_t *n)
{
  CURLcode result;
  ساختار connectdata *c = پوچ;
  ساختار Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  result = easy_connection(data, &c);
  اگر(result)
    بازگشت result;

  اگر(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  sigpipe_ignore(data, &sigpipe_ctx);
  result = Curl_conn_send(data, FIRSTSOCKET, buffer, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  اگر(result && result != CURLE_AGAIN)
    بازگشت CURLE_SEND_ERROR;
  بازگشت result;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ثابت تهی *buffer, size_t buflen,
                        size_t *n)
{
  ساختار Curl_eapi_guard guard;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_send, &result)) {
    ساختار Curl_easy *data = curl;
    size_t written = 0;

    اگر(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      برو out;
    }
    result = Curl_senddata(data, buffer, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  ساختار Curl_eapi_guard guard;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &result)) {
    /* Use the common function to keep connections alive. */
    result = Curl_cpool_upkeep((ساختار Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
}

CURLcode curl_easy_ssls_import(CURL *curl, ثابت نویسه *session_کلید,
                               ثابت unsigned نویسه *shmac, size_t shmac_len,
                               ثابت unsigned نویسه *sdata, size_t sdata_len)
{
#اگر defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ساختار Curl_eapi_guard guard;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &result)) {
    اگر(!sdata || !sdata_len)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      result = Curl_ssl_session_import((ساختار Curl_easy *)curl, session_کلید,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
#وگرنه
  (تهی)curl;
  (تهی)session_کلید;
  (تهی)shmac;
  (تهی)shmac_len;
  (تهی)sdata;
  (تهی)sdata_len;
  بازگشت CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               تهی *userptr)
{
#اگر defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ساختار Curl_eapi_guard guard;
  CURLcode result;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &result)) {
    اگر(!export_fn)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وگرنه
      result = Curl_ssl_session_export((ساختار Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  بازگشت result;
#وگرنه
  (تهی)curl;
  (تهی)export_fn;
  (تهی)userptr;
  بازگشت CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(ساختار Curl_easy *data, ثابت نویسه *کلید,
                       تهی *meta_data, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_data); /* never set to NULL */
  اگر(!Curl_hash_add2(&data->meta_hash, کلید, strlen(کلید) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(کلید, strlen(کلید) + 1, meta_data);
    بازگشت CURLE_OUT_OF_MEMORY;
  }
  بازگشت CURLE_OK;
}

تهی Curl_meta_remove(ساختار Curl_easy *data, ثابت نویسه *کلید)
{
  Curl_hash_delete(&data->meta_hash, کلید, strlen(کلید) + 1);
}

تهی *Curl_meta_get(ساختار Curl_easy *data, ثابت نویسه *کلید)
{
  بازگشت Curl_hash_pick(&data->meta_hash, کلید, strlen(کلید) + 1);
}

تهی Curl_meta_reset(ساختار Curl_easy *data)
{
  Curl_hash_clean(&data->meta_hash);
}
