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
#ضمن "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ضمن <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#endif

#ضمن "urldata.h"
#ضمن "api.h"
#ضمن "transfer.h"
#ضمن "vdns/hostip.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vquic/vquic.h"
#ضمن "url.h"
#ضمن "getinfo.h"
#ضمن "curlx/strdup.h"
#ضمن "easyif.h"
#ضمن "multiif.h"
#ضمن "multi_ev.h"
#ضمن "select.h"
#ضمن "cfilters.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "connect.h" /* for Curl_getconnectinfo */
#ضمن "slist.h"
#ضمن "mime.h"
#ضمن "amigaos.h"
#ضمن "macos.h"
#ضمن "curlx/wait.h"
#ضمن "sigpipe.h"
#ضمن "vssh/ssh.h"
#ضمن "setopt.h"
#ضمن "http_digest.h"
#ضمن "curlx/dynbuf.h"
#ضمن "bufref.h"
#ضمن "altsvc.h"
#ضمن "hsts.h"
#ضمن "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#ضمن "curlx/timeval.h" /* for curlx_now_init() */
#ضمن "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#ضمن "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
ساكن unsigned صحيح initialized;
#ifdef USE_WINSOCK
ساكن bool winsock_initialized;
#endif

#ifdef GLOBAL_INIT_IS_THREADSAFE

ساكن curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#define global_init_lock()   curl_simple_lock_lock(&s_lock)
#define global_init_unlock() curl_simple_lock_unlock(&s_lock)

#وإلا

#define global_init_lock()
#define global_init_unlock()

#endif

#𐤀𐤌 defined(_MSC_VER) && defined(_DLL)
#  pragma warning(push)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)malloc;
curl_free_callback Curl_cfree = (curl_free_callback)حرر;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_LOW;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#𐤀𐤌 defined(_MSC_VER) && defined(_DLL)
#  pragma warning(pop)
#endif

#ifdef DEBUGBUILD
ساكن حرف *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
ساكن CURLcode global_init(long flags, bool memoryfuncs)
{
  (فراغ)flags;

  𐤀𐤌(initialized++)
    أرجع CURLE_OK;

  𐤀𐤌(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)حرر;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_LOW;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  𐤀𐤌(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    اذهب fail;
  }

#ifdef USE_WINDOWS_SSPI
  𐤀𐤌(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    اذهب fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  𐤀𐤌(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    𐤀𐤌(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      اذهب fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  𐤀𐤌(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    اذهب fail;
  }

  𐤀𐤌(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    اذهب fail;
  }

  𐤀𐤌(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    اذهب fail;
  }

  𐤀𐤌(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    اذهب fail;
  }

  𐤀𐤌(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    اذهب fail;
  }

  𐤀𐤌(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    اذهب fail;
  }

  𐤀𐤌(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    اذهب fail;
  }

#ifdef DEBUGBUILD
  𐤀𐤌(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  أرجع CURLE_OK;

fail:
#ifdef USE_WINSOCK
  𐤀𐤌(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  أرجع CURLE_FAILED_INIT;
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

  أرجع result;
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
  𐤀𐤌(!m || !f || !r || !s || !c)
    أرجع CURLE_FAILED_INIT;

  global_init_lock();

  𐤀𐤌(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    أرجع CURLE_OK;
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

  أرجع result;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
فراغ curl_global_cleanup(فراغ)
{
  global_init_lock();

  𐤀𐤌(!initialized) {
    global_init_unlock();
    أرجع;
  }

  𐤀𐤌(--initialized) {
    global_init_unlock();
    أرجع;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ifdef USE_WINSOCK
  𐤀𐤌(winsock_initialized) {
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
CURLcode curl_global_trace(ثابت حرف *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode result;
  global_init_lock();

  result = Curl_trc_opt(config);

  global_init_unlock();

  أرجع result;
#وإلا
  (فراغ)config;
  أرجع CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ثابت حرف *name,
                              ثابت curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  أرجع rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(فراغ)
{
  CURLcode result;
  بنية Curl_easy *data;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  𐤀𐤌(!initialized) {
    result = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    𐤀𐤌(result) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      أرجع عدم;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  result = Curl_open(&data);
  𐤀𐤌(result) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    أرجع عدم;
  }

  أرجع data;
}

#ifdef DEBUGBUILD

بنية socketmonitor {
  بنية socketmonitor *next; /* the next node in the list or NULL */
  بنية pollfd socket; /* socket info of what to monitor */
};

بنية events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  صحيح num_sockets;      /* number of nodes in the monitor list */
  بنية socketmonitor *list; /* list of sockets to monitor */
  صحيح running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
ساكن صحيح events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        فراغ *userp)     /* private callback pointer */
{
  بنية events *ev = userp;
  (فراغ)multi;
#𐤀𐤌 DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  أرجع 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
ساكن صحيح poll2cselect(صحيح pollmask)
{
  صحيح omask = 0;
  𐤀𐤌(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  𐤀𐤌(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  𐤀𐤌(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  أرجع omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
ساكن short socketcb2poll(صحيح pollmask)
{
  short omask = 0;
  𐤀𐤌(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  𐤀𐤌(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  أرجع omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
ساكن صحيح events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         صحيح what,        /* see above */
                         فراغ *userp,     /* private callback
                                             pointer */
                         فراغ *socketp)   /* private socket
                                             pointer */
{
  بنية events *ev = userp;
  بنية socketmonitor *m;
  بنية socketmonitor *prev = عدم;
  bool found = FALSE;
  بنية Curl_easy *data = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (فراغ)easy;
#endif
  (فراغ)socketp;

  m = ev->list;
  طالما(m) {
    𐤀𐤌(m->socket.fd == s) {
      found = TRUE;
      𐤀𐤌(what == CURL_POLL_REMOVE) {
        بنية socketmonitor *nxt = m->next;
        /* remove this node from the list of monitored sockets */
        𐤀𐤌(prev)
          prev->next = nxt;
        وإلا
          ev->list = nxt;
        curlx_free(m);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      وإلا {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      اكسر;
    }
    prev = m;
    m = m->next; /* move to next node */
  }

  𐤀𐤌(!found) {
    𐤀𐤌(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(data, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    وإلا {
      m = curlx_malloc(حجم(بنية socketmonitor));
      𐤀𐤌(m) {
        m->next = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(data, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      وإلا
        أرجع CURLE_OUT_OF_MEMORY;
    }
  }

  أرجع 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
ساكن فراغ events_setup(بنية Curl_multi *multi, بنية events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

ساكن CURLcode populate_cpfds(بنية curl_pollfds *cpfds, بنية events *ev)
{
  بنية socketmonitor *m;
  CURLcode result = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  لكل(m = ev->list; m && !result; m = m->next) {
    result = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#𐤀𐤌 DEBUG_EV_POLL
    𐤀𐤌(!result)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  أرجع result;
}

ساكن CURLcode poll_cpfds(بنية events *ev,
                           بنية curl_pollfds *cpfds,
                           صحيح *pollrc)
{
  𐤀𐤌(cpfds->n) {
    /* wait for activity or timeout */
#𐤀𐤌 DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#endif
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#𐤀𐤌 DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#endif
    𐤀𐤌(*pollrc < 0)
      أرجع CURLE_UNRECOVERABLE_POLL;
  }
  وإلا {
#𐤀𐤌 DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    𐤀𐤌(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  أرجع CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
ساكن CURLcode wait_or_timeout(بنية Curl_multi *multi, بنية events *ev)
{
  بنية pollfd fds_on_stack[8];
  بنية curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_stack, CURL_ARRAYSIZE(fds_on_stack));
  طالما(!done) {
    CURLMsg *msg;
    صحيح pollrc;
    بنية curltime start;

    result = populate_cpfds(&cpfds, ev);
    𐤀𐤌(result)
      اذهب out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    result = poll_cpfds(ev, &cpfds, &pollrc);
    𐤀𐤌(result)
      اذهب out;

    ev->msbump = FALSE; /* reset here */

    𐤀𐤌(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#𐤀𐤌 DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#endif
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    وإلا {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      unsigned صحيح i;
      لكل(i = 0; i < cpfds.n; i++) {
        𐤀𐤌(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          صحيح act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      𐤀𐤌(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        𐤀𐤌(spent_ms > 0) {
#𐤀𐤌 DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (long)spent_ms);
#endif
          𐤀𐤌(spent_ms > ev->ms)
            ev->ms = 0;
          وإلا
            ev->ms -= (long)spent_ms;
        }
      }
    }

    𐤀𐤌(mresult) {
      result = CURLE_URL_MALFORMAT;
      اذهب out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    msg = curl_multi_info_read(multi, &pollrc);
    𐤀𐤌(msg) {
      result = msg->data.result;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  أرجع result;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
ساكن CURLcode easy_events(بنية Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  ساكن بنية events evs = { -1, FALSE, 0, عدم, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  أرجع wait_or_timeout(multi, &evs);
}
#وإلا /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

ساكن CURLcode easy_transfer(بنية Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  طالما(!done && !mresult) {
    صحيح still_running = 0;

    mresult = curl_multi_poll(multi, عدم, 0, 1000, عدم);

    𐤀𐤌(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    𐤀𐤌(!mresult && !still_running) {
      صحيح rc;
      CURLMsg *msg = curl_multi_info_read(multi, &rc);
      𐤀𐤌(msg) {
        result = msg->data.result;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  𐤀𐤌(mresult) {
    result = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  أرجع result;
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
ساكن CURLcode easy_perform(بنية Curl_easy *data, bool events)
{
  بنية Curl_multi *multi;
  CURLMcode mresult;
  CURLcode result = CURLE_OK;
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  𐤀𐤌(!data)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  𐤀𐤌(data->set.errorbuffer)
    /* clear this as early as possible */
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  𐤀𐤌(data->multi) {
    failf(data, "easy handle already used in multi handle");
    أرجع CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  𐤀𐤌(data->conn) {
    بنية connectdata *conn = data->conn;
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
    DEBUGASSERT(!data->conn);
  }

  𐤀𐤌(data->multi_easy)
    multi = data->multi_easy;
  وإلا {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    𐤀𐤌(!multi)
      أرجع CURLE_OUT_OF_MEMORY;
  }

  𐤀𐤌(Curl_api_multi_is_in_callback(multi))
    أرجع CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)data->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)data->set.quick_exit);

  data->multi_easy = عدم; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, data);
  𐤀𐤌(mresult) {
    curl_multi_cleanup(multi);
    𐤀𐤌(mresult == CURLM_OUT_OF_MEMORY)
      أرجع CURLE_OUT_OF_MEMORY;
    أرجع CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  data->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(data, &sigpipe_ctx);

  /* run the transfer */
  result = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (فراغ)Curl_multi_remove_handle(multi, data);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  أرجع result;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  بنية Curl_eapi_guard guard = { 0 };
  CURLcode result;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_perform, &result)) {
    result = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(بنية Curl_easy *easy)
{
  بنية Curl_eapi_guard guard;
  CURLcode result;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &result)) {
    result = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
فراغ curl_easy_cleanup(CURL *curl)
{
  بنية Curl_eapi_guard guard;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, عدم)) {
    بنية Curl_easy *data = curl;
    بنية Curl_sigpipe_ctx sigpipe_ctx;
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
  بنية Curl_eapi_guard guard;
  CURLcode result;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &result)) {
    بنية Curl_easy *data = curl;
    va_list arg;
    فراغ *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, فراغ *);

    result = Curl_getinfo(data, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}

ساكن CURLcode dupset(بنية Curl_easy *dst, بنية Curl_easy *src)
{
  CURLcode result = CURLE_OK;
  تعداد dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#𐤀𐤌 !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = عدم;
#endif
  dst->set.str_copypostfields = عدم;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * حجم(بنية curl_blob *));

  /* duplicate all strings */
  result = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  𐤀𐤌(result)
    أرجع result;

  /* duplicate all blobs */
  لكل(j = (تعداد dupblob)0; j < BLOB_LAST; j++) {
    result = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    𐤀𐤌(result)
      أرجع result;
  }

  /* duplicate memory areas pointed to */
  𐤀𐤌(src->set.str_copypostfields) {
    𐤀𐤌(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    وإلا
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    𐤀𐤌(!dst->set.str_copypostfields)
      أرجع CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#𐤀𐤌 !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  𐤀𐤌(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(حجم(*dst->set.mimepostp));
    𐤀𐤌(!dst->set.mimepostp)
      أرجع CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    result = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    𐤀𐤌(result)
      أرجع result;
  }
#endif

  𐤀𐤌(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  أرجع result;
}

ساكن فراغ dupeasy_meta_freeentry(فراغ *p)
{
  (فراغ)p;
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
  بنية Curl_eapi_guard guard;
  بنية Curl_easy *outcurl = عدم;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, عدم)) {
    بنية Curl_easy *data = curl;

    outcurl = curlx_calloc(1, حجم(بنية Curl_easy));
    𐤀𐤌(!outcurl)
      اذهب fail;

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
    Curl_llist_init(&outcurl->state.httphdrs, عدم);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    𐤀𐤌(dupset(outcurl, data))
      اذهب fail;

    outcurl->progress.hide     = data->progress.hide;
    outcurl->progress.callback = data->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = عدم;
    𐤀𐤌(data->cookies && data->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      𐤀𐤌(!outcurl->cookies)
        اذهب fail;
      outcurl->state.cookie_engine = TRUE;
    }

    𐤀𐤌(data->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(data->state.cookielist);
      𐤀𐤌(!outcurl->state.cookielist)
        اذهب fail;
    }
#endif

    𐤀𐤌(Curl_bufref_ptr(&data->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&data->state.url), 0,
                      curl_free);
      𐤀𐤌(!Curl_bufref_ptr(&outcurl->state.url))
        اذهب fail;
    }
    𐤀𐤌(Curl_bufref_ptr(&data->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&data->state.referer), 0,
                      curl_free);
      𐤀𐤌(!Curl_bufref_ptr(&outcurl->state.referer))
        اذهب fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    𐤀𐤌(data->asi) {
      ثابت حرف *str;
      outcurl->asi = Curl_altsvc_init();
      𐤀𐤌(!outcurl->asi)
        اذهب fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      𐤀𐤌(str)
        (فراغ)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    𐤀𐤌(data->hsts) {
      ثابت حرف *str;
      outcurl->hsts = Curl_hsts_init();
      𐤀𐤌(!outcurl->hsts)
        اذهب fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      𐤀𐤌(str)
        (فراغ)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (فراغ)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      𐤀𐤌(Curl_hsts_copy(outcurl->hsts, data->hsts))
        اذهب fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_NUMBER;
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع outcurl;

fail:
  𐤀𐤌(outcurl) {
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
  أرجع عدم;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
فراغ curl_easy_reset(CURL *curl)
{
  بنية Curl_eapi_guard guard;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_reset, عدم)) {
    بنية Curl_easy *data = curl;

    data->state.last_conn_id = -1; /* clear remembered connection id */
    data->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&data->req, data);
    Curl_hash_clean(&data->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(data);
    /* zero out UserDefined data: */
    Curl_freeset(data);
    memset(&data->set, 0, حجم(بنية UserDefined));
    Curl_init_userdefined(data);

    /* zero out Progress data: */
    memset(&data->progress, 0, حجم(بنية Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(data);

    data->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&data->state.authhost, 0, حجم(بنية auth));
    memset(&data->state.authproxy, 0, حجم(بنية auth));

#𐤀𐤌 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
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
CURLcode curl_easy_pause(CURL *curl, صحيح action)
{
  بنية Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_pause, &result)) {
    bool changed = FALSE;
    بنية Curl_easy *data = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    𐤀𐤌(!data->conn) {
      /* crazy input, do not continue */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      اذهب out;
    }

    recv_paused = Curl_xfer_recv_is_paused(data);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(data);
    send_paused_new = (action & CURLPAUSE_SEND);

    𐤀𐤌((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(data))) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_send(data, send_paused_new));
    }

    𐤀𐤌(recv_paused != recv_paused_new) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_recv(data, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    𐤀𐤌(!Curl_xfer_is_blocked(data)) {
      /* reset the too-slow time keeper */
      data->state.keeps_speed.tv_sec = 0;
      𐤀𐤌(data->multi) {
        Curl_multi_mark_dirty(data); /* make it run */
        /* On changes, tell application to update its timers. */
        𐤀𐤌(changed && Curl_update_timer(data->multi) && !result)
          result = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    𐤀𐤌(!result && changed && !data->state.done && data->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(data->multi, data))
      result = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}

ساكن CURLcode easy_connection(بنية Curl_easy *data,
                                بنية connectdata **connp)
{
  curl_socket_t sfd;

  𐤀𐤌(!data)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  𐤀𐤌(!data->set.connect_only) {
    failf(data, "CONNECT_ONLY is required");
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(data, connp);

  𐤀𐤌(sfd == CURL_SOCKET_BAD) {
    failf(data, "Failed to get last socket used for connection #%" FMT_OFF_T,
          data->state.last_conn_id);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  أرجع CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(بنية Curl_easy *data,
                        فراغ *buffer, size_t buflen, size_t *n)
{
  CURLcode result;
  بنية connectdata *c;

  result = easy_connection(data, &c);
  𐤀𐤌(result)
    أرجع result;

  𐤀𐤌(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  *n = 0;
  أرجع Curl_conn_recv(data, FIRSTSOCKET, buffer, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, فراغ *buffer, size_t buflen, size_t *n)
{
  بنية Curl_eapi_guard guard;
  CURLcode result;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_recv, &result)) {
    𐤀𐤌(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      اذهب out;
    }
    result = Curl_easy_recv(curl, buffer, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(بنية Curl_easy *data)
{
  CURLcode result;
  بنية connectdata *c = عدم;

  result = easy_connection(data, &c);
  𐤀𐤌(result)
    أرجع result;

  𐤀𐤌(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  أرجع CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(بنية Curl_easy *data, ثابت فراغ *buffer,
                       size_t buflen, size_t *n)
{
  CURLcode result;
  بنية connectdata *c = عدم;
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  result = easy_connection(data, &c);
  𐤀𐤌(result)
    أرجع result;

  𐤀𐤌(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  sigpipe_ignore(data, &sigpipe_ctx);
  result = Curl_conn_send(data, FIRSTSOCKET, buffer, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  𐤀𐤌(result && result != CURLE_AGAIN)
    أرجع CURLE_SEND_ERROR;
  أرجع result;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ثابت فراغ *buffer, size_t buflen,
                        size_t *n)
{
  بنية Curl_eapi_guard guard;
  CURLcode result;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_send, &result)) {
    بنية Curl_easy *data = curl;
    size_t written = 0;

    𐤀𐤌(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      اذهب out;
    }
    result = Curl_senddata(data, buffer, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  بنية Curl_eapi_guard guard;
  CURLcode result;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &result)) {
    /* Use the common function to keep connections alive. */
    result = Curl_cpool_upkeep((بنية Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
}

CURLcode curl_easy_ssls_import(CURL *curl, ثابت حرف *session_key,
                               ثابت unsigned حرف *shmac, size_t shmac_len,
                               ثابت unsigned حرف *sdata, size_t sdata_len)
{
#𐤀𐤌 defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  بنية Curl_eapi_guard guard;
  CURLcode result;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &result)) {
    𐤀𐤌(!sdata || !sdata_len)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وإلا
      result = Curl_ssl_session_import((بنية Curl_easy *)curl, session_key,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
#وإلا
  (فراغ)curl;
  (فراغ)session_key;
  (فراغ)shmac;
  (فراغ)shmac_len;
  (فراغ)sdata;
  (فراغ)sdata_len;
  أرجع CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               فراغ *userptr)
{
#𐤀𐤌 defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  بنية Curl_eapi_guard guard;
  CURLcode result;

  𐤀𐤌(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &result)) {
    𐤀𐤌(!export_fn)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    وإلا
      result = Curl_ssl_session_export((بنية Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع result;
#وإلا
  (فراغ)curl;
  (فراغ)export_fn;
  (فراغ)userptr;
  أرجع CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(بنية Curl_easy *data, ثابت حرف *key,
                       فراغ *meta_data, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_data); /* never set to NULL */
  𐤀𐤌(!Curl_hash_add2(&data->meta_hash, key, strlen(key) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(key, strlen(key) + 1, meta_data);
    أرجع CURLE_OUT_OF_MEMORY;
  }
  أرجع CURLE_OK;
}

فراغ Curl_meta_remove(بنية Curl_easy *data, ثابت حرف *key)
{
  Curl_hash_delete(&data->meta_hash, key, strlen(key) + 1);
}

فراغ *Curl_meta_get(بنية Curl_easy *data, ثابت حرف *key)
{
  أرجع Curl_hash_pick(&data->meta_hash, key, strlen(key) + 1);
}

فراغ Curl_meta_reset(بنية Curl_easy *data)
{
  Curl_hash_clean(&data->meta_hash);
}
