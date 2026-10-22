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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#અન્તર્ભાવ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#અન્તર્ભાવ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#અન્તર્ભાવ <sys/param.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "api.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "vdns/hostip.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "vquic/vquic.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "getinfo.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "easyif.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "multi_ev.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "connect.h" /* for Curl_getconnectinfo */
#અન્તર્ભાવ "slist.h"
#અન્તર્ભાવ "mime.h"
#અન્તર્ભાવ "amigaos.h"
#અન્તર્ભાવ "macos.h"
#અન્તર્ભાવ "curlx/wait.h"
#અન્તર્ભાવ "sigpipe.h"
#અન્તર્ભાવ "vssh/ssh.h"
#અન્તર્ભાવ "setopt.h"
#અન્તર્ભાવ "http_digest.h"
#અન્તર્ભાવ "curlx/dynbuf.h"
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "altsvc.h"
#અન્તર્ભાવ "hsts.h"
#અન્તર્ભાવ "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#અન્તર્ભાવ "curlx/timeval.h" /* for curlx_now_init() */
#અન્તર્ભાવ "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#અન્તર્ભાવ "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
સ્થિર unsigned પૂર્ણાંક initialized;
#ifdef USE_WINSOCK
સ્થિર bool winsock_initialized;
#endif

#ifdef GLOBAL_INIT_IS_THREADSAFE

સ્થિર curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#define global_init_lock()   curl_simple_lock_lock(&s_lock)
#define global_init_unlock() curl_simple_lock_unlock(&s_lock)

#નહીંતર

#define global_init_lock()
#define global_init_unlock()

#endif

#જો defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ધકેલો)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)malloc;
curl_free_callback Curl_cfree = (curl_free_callback)મુક્ત;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_નીચલું;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#જો defined(_MSC_VER) && defined(_DLL)
#  pragma warning(કાઢો)
#endif

#ifdef DEBUGBUILD
સ્થિર અક્ષર *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
સ્થિર CURLcode global_init(long flags, bool memoryfuncs)
{
  (શૂન્ય)flags;

  જો(initialized++)
    પાછા CURLE_OK;

  જો(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)મુક્ત;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_નીચલું;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  જો(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    જાઓ fail;
  }

#ifdef USE_WINDOWS_SSPI
  જો(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    જાઓ fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  જો(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    જો(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      જાઓ fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  જો(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    જાઓ fail;
  }

  જો(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    જાઓ fail;
  }

  જો(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    જાઓ fail;
  }

  જો(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    જાઓ fail;
  }

  જો(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    જાઓ fail;
  }

  જો(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    જાઓ fail;
  }

  જો(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    જાઓ fail;
  }

#ifdef DEBUGBUILD
  જો(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  પાછા CURLE_OK;

fail:
#ifdef USE_WINSOCK
  જો(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  પાછા CURLE_FAILED_INIT;
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

  પાછા result;
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
  જો(!m || !f || !r || !s || !c)
    પાછા CURLE_FAILED_INIT;

  global_init_lock();

  જો(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    પાછા CURLE_OK;
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

  પાછા result;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
શૂન્ય curl_global_cleanup(શૂન્ય)
{
  global_init_lock();

  જો(!initialized) {
    global_init_unlock();
    પાછા;
  }

  જો(--initialized) {
    global_init_unlock();
    પાછા;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ifdef USE_WINSOCK
  જો(winsock_initialized) {
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
CURLcode curl_global_trace(અચલ અક્ષર *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode result;
  global_init_lock();

  result = Curl_trc_opt(config);

  global_init_unlock();

  પાછા result;
#નહીંતર
  (શૂન્ય)config;
  પાછા CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, અચલ અક્ષર *name,
                              અચલ curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  પાછા rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(શૂન્ય)
{
  CURLcode result;
  સંરચના Curl_easy *data;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  જો(!initialized) {
    result = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    જો(result) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      પાછા ખાલી;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  result = Curl_open(&data);
  જો(result) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    પાછા ખાલી;
  }

  પાછા data;
}

#ifdef DEBUGBUILD

સંરચના socketmonitor {
  સંરચના socketmonitor *next; /* the next node in the list or NULL */
  સંરચના pollfd socket; /* socket info of what to monitor */
};

સંરચના events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  પૂર્ણાંક અંક_sockets;      /* number of nodes in the monitor list */
  સંરચના socketmonitor *list; /* list of sockets to monitor */
  પૂર્ણાંક running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
સ્થિર પૂર્ણાંક events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        શૂન્ય *userp)     /* private callback pointer */
{
  સંરચના events *ev = userp;
  (શૂન્ય)multi;
#જો DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  પાછા 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
સ્થિર પૂર્ણાંક poll2cselect(પૂર્ણાંક pollmask)
{
  પૂર્ણાંક omask = 0;
  જો(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  જો(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  જો(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  પાછા omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
સ્થિર short socketcb2poll(પૂર્ણાંક pollmask)
{
  short omask = 0;
  જો(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  જો(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  પાછા omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
સ્થિર પૂર્ણાંક events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         પૂર્ણાંક what,        /* see above */
                         શૂન્ય *userp,     /* private callback
                                             pointer */
                         શૂન્ય *socketp)   /* private socket
                                             pointer */
{
  સંરચના events *ev = userp;
  સંરચના socketmonitor *m;
  સંરચના socketmonitor *prev = ખાલી;
  bool found = FALSE;
  સંરચના Curl_easy *data = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (શૂન્ય)easy;
#endif
  (શૂન્ય)socketp;

  m = ev->list;
  જ્યારે(m) {
    જો(m->socket.fd == s) {
      found = TRUE;
      જો(what == CURL_POLL_REMOVE) {
        સંરચના socketmonitor *nxt = m->next;
        /* remove this node from the list of monitored sockets */
        જો(prev)
          prev->next = nxt;
        નહીંતર
          ev->list = nxt;
        curlx_free(m);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      નહીંતર {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      તોડો;
    }
    prev = m;
    m = m->next; /* move to next node */
  }

  જો(!found) {
    જો(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(data, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    નહીંતર {
      m = curlx_malloc(કદ(સંરચના socketmonitor));
      જો(m) {
        m->next = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(data, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      નહીંતર
        પાછા CURLE_OUT_OF_MEMORY;
    }
  }

  પાછા 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
સ્થિર શૂન્ય events_setup(સંરચના Curl_multi *multi, સંરચના events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

સ્થિર CURLcode populate_cpfds(સંરચના curl_pollfds *cpfds, સંરચના events *ev)
{
  સંરચના socketmonitor *m;
  CURLcode result = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  માટે(m = ev->list; m && !result; m = m->next) {
    result = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#જો DEBUG_EV_POLL
    જો(!result)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  પાછા result;
}

સ્થિર CURLcode poll_cpfds(સંરચના events *ev,
                           સંરચના curl_pollfds *cpfds,
                           પૂર્ણાંક *pollrc)
{
  જો(cpfds->n) {
    /* wait for activity or timeout */
#જો DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#endif
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#જો DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#endif
    જો(*pollrc < 0)
      પાછા CURLE_UNRECOVERABLE_POLL;
  }
  નહીંતર {
#જો DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    જો(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  પાછા CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
સ્થિર CURLcode wait_or_timeout(સંરચના Curl_multi *multi, સંરચના events *ev)
{
  સંરચના pollfd fds_on_ઢગલો[8];
  સંરચના curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_ઢગલો, CURL_ARRAYSIZE(fds_on_ઢગલો));
  જ્યારે(!done) {
    CURLMsg *સંદેશ;
    પૂર્ણાંક pollrc;
    સંરચના curltime start;

    result = populate_cpfds(&cpfds, ev);
    જો(result)
      જાઓ out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    result = poll_cpfds(ev, &cpfds, &pollrc);
    જો(result)
      જાઓ out;

    ev->msbump = FALSE; /* reset here */

    જો(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#જો DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#endif
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    નહીંતર {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      unsigned પૂર્ણાંક i;
      માટે(i = 0; i < cpfds.n; i++) {
        જો(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          પૂર્ણાંક act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      જો(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        જો(spent_ms > 0) {
#જો DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (long)spent_ms);
#endif
          જો(spent_ms > ev->ms)
            ev->ms = 0;
          નહીંતર
            ev->ms -= (long)spent_ms;
        }
      }
    }

    જો(mresult) {
      result = CURLE_URL_MALFORMAT;
      જાઓ out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    સંદેશ = curl_multi_info_read(multi, &pollrc);
    જો(સંદેશ) {
      result = સંદેશ->data.result;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  પાછા result;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
સ્થિર CURLcode easy_events(સંરચના Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  સ્થિર સંરચના events evs = { -1, FALSE, 0, ખાલી, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  પાછા wait_or_timeout(multi, &evs);
}
#નહીંતર /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

સ્થિર CURLcode easy_transfer(સંરચના Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  જ્યારે(!done && !mresult) {
    પૂર્ણાંક still_running = 0;

    mresult = curl_multi_poll(multi, ખાલી, 0, 1000, ખાલી);

    જો(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    જો(!mresult && !still_running) {
      પૂર્ણાંક rc;
      CURLMsg *સંદેશ = curl_multi_info_read(multi, &rc);
      જો(સંદેશ) {
        result = સંદેશ->data.result;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  જો(mresult) {
    result = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  પાછા result;
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
સ્થિર CURLcode easy_perform(સંરચના Curl_easy *data, bool events)
{
  સંરચના Curl_multi *multi;
  CURLMcode mresult;
  CURLcode result = CURLE_OK;
  સંરચના Curl_sigpipe_ctx sigpipe_ctx;

  જો(!data)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(data->set.errorbuffer)
    /* clear this as early as possible */
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  જો(data->multi) {
    failf(data, "easy handle already used in multi handle");
    પાછા CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  જો(data->conn) {
    સંરચના connectdata *conn = data->conn;
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
    DEBUGASSERT(!data->conn);
  }

  જો(data->multi_easy)
    multi = data->multi_easy;
  નહીંતર {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    જો(!multi)
      પાછા CURLE_OUT_OF_MEMORY;
  }

  જો(Curl_api_multi_is_in_callback(multi))
    પાછા CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)data->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)data->set.quick_exit);

  data->multi_easy = ખાલી; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, data);
  જો(mresult) {
    curl_multi_cleanup(multi);
    જો(mresult == CURLM_OUT_OF_MEMORY)
      પાછા CURLE_OUT_OF_MEMORY;
    પાછા CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  data->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(data, &sigpipe_ctx);

  /* run the transfer */
  result = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (શૂન્ય)Curl_multi_remove_handle(multi, data);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  પાછા result;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  સંરચના Curl_eapi_guard guard = { 0 };
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_perform, &result)) {
    result = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(સંરચના Curl_easy *easy)
{
  સંરચના Curl_eapi_guard guard;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &result)) {
    result = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
શૂન્ય curl_easy_cleanup(CURL *curl)
{
  સંરચના Curl_eapi_guard guard;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, ખાલી)) {
    સંરચના Curl_easy *data = curl;
    સંરચના Curl_sigpipe_ctx sigpipe_ctx;
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
  સંરચના Curl_eapi_guard guard;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &result)) {
    સંરચના Curl_easy *data = curl;
    va_list arg;
    શૂન્ય *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, શૂન્ય *);

    result = Curl_getinfo(data, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}

સ્થિર CURLcode dupset(સંરચના Curl_easy *dst, સંરચના Curl_easy *src)
{
  CURLcode result = CURLE_OK;
  ગણના dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#જો !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = ખાલી;
#endif
  dst->set.str_copypostfields = ખાલી;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * કદ(સંરચના curl_blob *));

  /* duplicate all strings */
  result = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  જો(result)
    પાછા result;

  /* duplicate all blobs */
  માટે(j = (ગણના dupblob)0; j < BLOB_LAST; j++) {
    result = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    જો(result)
      પાછા result;
  }

  /* duplicate memory areas pointed to */
  જો(src->set.str_copypostfields) {
    જો(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    નહીંતર
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    જો(!dst->set.str_copypostfields)
      પાછા CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#જો !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  જો(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(કદ(*dst->set.mimepostp));
    જો(!dst->set.mimepostp)
      પાછા CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    result = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    જો(result)
      પાછા result;
  }
#endif

  જો(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  પાછા result;
}

સ્થિર શૂન્ય dupeasy_meta_freeentry(શૂન્ય *p)
{
  (શૂન્ય)p;
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
  સંરચના Curl_eapi_guard guard;
  સંરચના Curl_easy *outcurl = ખાલી;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, ખાલી)) {
    સંરચના Curl_easy *data = curl;

    outcurl = curlx_calloc(1, કદ(સંરચના Curl_easy));
    જો(!outcurl)
      જાઓ fail;

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
    Curl_llist_init(&outcurl->state.httphdrs, ખાલી);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    જો(dupset(outcurl, data))
      જાઓ fail;

    outcurl->progress.hide     = data->progress.hide;
    outcurl->progress.callback = data->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = ખાલી;
    જો(data->cookies && data->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      જો(!outcurl->cookies)
        જાઓ fail;
      outcurl->state.cookie_engine = TRUE;
    }

    જો(data->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(data->state.cookielist);
      જો(!outcurl->state.cookielist)
        જાઓ fail;
    }
#endif

    જો(Curl_bufref_ptr(&data->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&data->state.url), 0,
                      curl_free);
      જો(!Curl_bufref_ptr(&outcurl->state.url))
        જાઓ fail;
    }
    જો(Curl_bufref_ptr(&data->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&data->state.referer), 0,
                      curl_free);
      જો(!Curl_bufref_ptr(&outcurl->state.referer))
        જાઓ fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    જો(data->asi) {
      અચલ અક્ષર *str;
      outcurl->asi = Curl_altsvc_init();
      જો(!outcurl->asi)
        જાઓ fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      જો(str)
        (શૂન્ય)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    જો(data->hsts) {
      અચલ અક્ષર *str;
      outcurl->hsts = Curl_hsts_init();
      જો(!outcurl->hsts)
        જાઓ fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      જો(str)
        (શૂન્ય)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (શૂન્ય)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      જો(Curl_hsts_copy(outcurl->hsts, data->hsts))
        જાઓ fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_સંખ્યા;
  }
  CURL_EAPI_LEAVE(&guard);
  પાછા outcurl;

fail:
  જો(outcurl) {
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
  પાછા ખાલી;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
શૂન્ય curl_easy_reset(CURL *curl)
{
  સંરચના Curl_eapi_guard guard;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_reset, ખાલી)) {
    સંરચના Curl_easy *data = curl;

    data->state.last_conn_id = -1; /* clear remembered connection id */
    data->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&data->req, data);
    Curl_hash_clean(&data->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(data);
    /* zero out UserDefined data: */
    Curl_freeset(data);
    memset(&data->set, 0, કદ(સંરચના UserDefined));
    Curl_init_userdefined(data);

    /* zero out Progress data: */
    memset(&data->progress, 0, કદ(સંરચના Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(data);

    data->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&data->state.authhost, 0, કદ(સંરચના auth));
    memset(&data->state.authproxy, 0, કદ(સંરચના auth));

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
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
CURLcode curl_easy_pause(CURL *curl, પૂર્ણાંક action)
{
  સંરચના Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_pause, &result)) {
    bool changed = FALSE;
    સંરચના Curl_easy *data = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    જો(!data->conn) {
      /* crazy input, do not continue */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      જાઓ out;
    }

    recv_paused = Curl_xfer_recv_is_paused(data);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(data);
    send_paused_new = (action & CURLPAUSE_SEND);

    જો((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(data))) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_send(data, send_paused_new));
    }

    જો(recv_paused != recv_paused_new) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_recv(data, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    જો(!Curl_xfer_is_blocked(data)) {
      /* reset the too-slow time keeper */
      data->state.keeps_speed.tv_sec = 0;
      જો(data->multi) {
        Curl_multi_mark_dirty(data); /* make it run */
        /* On changes, tell application to update its timers. */
        જો(changed && Curl_update_timer(data->multi) && !result)
          result = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    જો(!result && changed && !data->state.done && data->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(data->multi, data))
      result = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}

સ્થિર CURLcode easy_connection(સંરચના Curl_easy *data,
                                સંરચના connectdata **connp)
{
  curl_socket_t sfd;

  જો(!data)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  જો(!data->set.connect_only) {
    failf(data, "CONNECT_ONLY is required");
    પાછા CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(data, connp);

  જો(sfd == CURL_SOCKET_BAD) {
    failf(data, "Failed to get last socket used for connection #%" FMT_OFF_T,
          data->state.last_conn_id);
    પાછા CURLE_UNSUPPORTED_PROTOCOL;
  }

  પાછા CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(સંરચના Curl_easy *data,
                        શૂન્ય *buffer, size_t buflen, size_t *n)
{
  CURLcode result;
  સંરચના connectdata *c;

  result = easy_connection(data, &c);
  જો(result)
    પાછા result;

  જો(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  *n = 0;
  પાછા Curl_conn_recv(data, FIRSTSOCKET, buffer, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, શૂન્ય *buffer, size_t buflen, size_t *n)
{
  સંરચના Curl_eapi_guard guard;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_recv, &result)) {
    જો(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      જાઓ out;
    }
    result = Curl_easy_recv(curl, buffer, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(સંરચના Curl_easy *data)
{
  CURLcode result;
  સંરચના connectdata *c = ખાલી;

  result = easy_connection(data, &c);
  જો(result)
    પાછા result;

  જો(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  પાછા CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(સંરચના Curl_easy *data, અચલ શૂન્ય *buffer,
                       size_t buflen, size_t *n)
{
  CURLcode result;
  સંરચના connectdata *c = ખાલી;
  સંરચના Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  result = easy_connection(data, &c);
  જો(result)
    પાછા result;

  જો(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  sigpipe_ignore(data, &sigpipe_ctx);
  result = Curl_conn_send(data, FIRSTSOCKET, buffer, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  જો(result && result != CURLE_AGAIN)
    પાછા CURLE_SEND_ERROR;
  પાછા result;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, અચલ શૂન્ય *buffer, size_t buflen,
                        size_t *n)
{
  સંરચના Curl_eapi_guard guard;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_send, &result)) {
    સંરચના Curl_easy *data = curl;
    size_t written = 0;

    જો(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      જાઓ out;
    }
    result = Curl_senddata(data, buffer, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  સંરચના Curl_eapi_guard guard;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &result)) {
    /* Use the common function to keep connections alive. */
    result = Curl_cpool_upkeep((સંરચના Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
}

CURLcode curl_easy_ssls_import(CURL *curl, અચલ અક્ષર *session_ચાવી,
                               અચલ unsigned અક્ષર *shmac, size_t shmac_len,
                               અચલ unsigned અક્ષર *sdata, size_t sdata_len)
{
#જો defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  સંરચના Curl_eapi_guard guard;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &result)) {
    જો(!sdata || !sdata_len)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      result = Curl_ssl_session_import((સંરચના Curl_easy *)curl, session_ચાવી,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
#નહીંતર
  (શૂન્ય)curl;
  (શૂન્ય)session_ચાવી;
  (શૂન્ય)shmac;
  (શૂન્ય)shmac_len;
  (શૂન્ય)sdata;
  (શૂન્ય)sdata_len;
  પાછા CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               શૂન્ય *userptr)
{
#જો defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  સંરચના Curl_eapi_guard guard;
  CURLcode result;

  જો(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &result)) {
    જો(!export_fn)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    નહીંતર
      result = Curl_ssl_session_export((સંરચના Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  પાછા result;
#નહીંતર
  (શૂન્ય)curl;
  (શૂન્ય)export_fn;
  (શૂન્ય)userptr;
  પાછા CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(સંરચના Curl_easy *data, અચલ અક્ષર *ચાવી,
                       શૂન્ય *meta_data, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_data); /* never set to NULL */
  જો(!Curl_hash_add2(&data->meta_hash, ચાવી, strlen(ચાવી) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(ચાવી, strlen(ચાવી) + 1, meta_data);
    પાછા CURLE_OUT_OF_MEMORY;
  }
  પાછા CURLE_OK;
}

શૂન્ય Curl_meta_remove(સંરચના Curl_easy *data, અચલ અક્ષર *ચાવી)
{
  Curl_hash_delete(&data->meta_hash, ચાવી, strlen(ચાવી) + 1);
}

શૂન્ય *Curl_meta_get(સંરચના Curl_easy *data, અચલ અક્ષર *ચાવી)
{
  પાછા Curl_hash_pick(&data->meta_hash, ચાવી, strlen(ચાવી) + 1);
}

શૂન્ય Curl_meta_reset(સંરચના Curl_easy *data)
{
  Curl_hash_clean(&data->meta_hash);
}
