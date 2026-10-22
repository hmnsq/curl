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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "api.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "vdns/hostip.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vquic/vquic.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "getinfo.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "easyif.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "multi_ev.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "connect.h" /* for Curl_getconnectinfo */
#अन्तर्भाव "slist.h"
#अन्तर्भाव "mime.h"
#अन्तर्भाव "amigaos.h"
#अन्तर्भाव "macos.h"
#अन्तर्भाव "curlx/wait.h"
#अन्तर्भाव "sigpipe.h"
#अन्तर्भाव "vssh/ssh.h"
#अन्तर्भाव "setopt.h"
#अन्तर्भाव "http_digest.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "altsvc.h"
#अन्तर्भाव "hsts.h"
#अन्तर्भाव "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#अन्तर्भाव "curlx/timeval.h" /* for curlx_now_init() */
#अन्तर्भाव "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#अन्तर्भाव "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
स्थिर unsigned पूर्णाङ्क initialized;
#ifdef USE_WINSOCK
स्थिर bool winsock_initialized;
#endif

#ifdef GLOBAL_INIT_IS_THREADSAFE

स्थिर curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#define global_init_lock()   curl_simple_lock_lock(&s_lock)
#define global_init_unlock() curl_simple_lock_unlock(&s_lock)

#अञ्ञथा

#define global_init_lock()
#define global_init_unlock()

#endif

#सचे defined(_MSC_VER) && defined(_DLL)
#  pragma warning(प्रक्षिप)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)malloc;
curl_free_callback Curl_cfree = (curl_free_callback)मुक्त;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_नीचः;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#सचे defined(_MSC_VER) && defined(_DLL)
#  pragma warning(निष्कासय)
#endif

#ifdef DEBUGBUILD
स्थिर वर्ण *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
स्थिर CURLcode global_init(long flags, bool memoryfuncs)
{
  (शून्य)flags;

  सचे(initialized++)
    पटिनिवत्त CURLE_OK;

  सचे(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)मुक्त;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_नीचः;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  सचे(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    गच्छ fail;
  }

#ifdef USE_WINDOWS_SSPI
  सचे(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    गच्छ fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  सचे(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    सचे(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      गच्छ fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  सचे(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    गच्छ fail;
  }

  सचे(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    गच्छ fail;
  }

  सचे(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    गच्छ fail;
  }

  सचे(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    गच्छ fail;
  }

  सचे(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    गच्छ fail;
  }

  सचे(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    गच्छ fail;
  }

  सचे(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    गच्छ fail;
  }

#ifdef DEBUGBUILD
  सचे(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  पटिनिवत्त CURLE_OK;

fail:
#ifdef USE_WINSOCK
  सचे(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  पटिनिवत्त CURLE_FAILED_INIT;
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

  पटिनिवत्त result;
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
  सचे(!m || !f || !r || !s || !c)
    पटिनिवत्त CURLE_FAILED_INIT;

  global_init_lock();

  सचे(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    पटिनिवत्त CURLE_OK;
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

  पटिनिवत्त result;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
शून्य curl_global_cleanup(शून्य)
{
  global_init_lock();

  सचे(!initialized) {
    global_init_unlock();
    पटिनिवत्त;
  }

  सचे(--initialized) {
    global_init_unlock();
    पटिनिवत्त;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ifdef USE_WINSOCK
  सचे(winsock_initialized) {
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
CURLcode curl_global_trace(ध्रुव वर्ण *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode result;
  global_init_lock();

  result = Curl_trc_opt(config);

  global_init_unlock();

  पटिनिवत्त result;
#अञ्ञथा
  (शून्य)config;
  पटिनिवत्त CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ध्रुव वर्ण *name,
                              ध्रुव curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  पटिनिवत्त rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(शून्य)
{
  CURLcode result;
  संरचना Curl_easy *data;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  सचे(!initialized) {
    result = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    सचे(result) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      पटिनिवत्त रिक्त;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  result = Curl_open(&data);
  सचे(result) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    पटिनिवत्त रिक्त;
  }

  पटिनिवत्त data;
}

#ifdef DEBUGBUILD

संरचना socketmonitor {
  संरचना socketmonitor *next; /* the next node in the list or NULL */
  संरचना pollfd socket; /* socket info of what to monitor */
};

संरचना events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  पूर्णाङ्क अङ्कः_sockets;      /* number of nodes in the monitor list */
  संरचना socketmonitor *list; /* list of sockets to monitor */
  पूर्णाङ्क running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
स्थिर पूर्णाङ्क events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        शून्य *userp)     /* private callback pointer */
{
  संरचना events *ev = userp;
  (शून्य)multi;
#सचे DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  पटिनिवत्त 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
स्थिर पूर्णाङ्क poll2cselect(पूर्णाङ्क pollmask)
{
  पूर्णाङ्क omask = 0;
  सचे(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  सचे(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  सचे(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  पटिनिवत्त omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
स्थिर short socketcb2poll(पूर्णाङ्क pollmask)
{
  short omask = 0;
  सचे(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  सचे(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  पटिनिवत्त omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
स्थिर पूर्णाङ्क events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         पूर्णाङ्क what,        /* see above */
                         शून्य *userp,     /* private callback
                                             pointer */
                         शून्य *socketp)   /* private socket
                                             pointer */
{
  संरचना events *ev = userp;
  संरचना socketmonitor *m;
  संरचना socketmonitor *prev = रिक्त;
  bool found = FALSE;
  संरचना Curl_easy *data = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (शून्य)easy;
#endif
  (शून्य)socketp;

  m = ev->list;
  याव(m) {
    सचे(m->socket.fd == s) {
      found = TRUE;
      सचे(what == CURL_POLL_REMOVE) {
        संरचना socketmonitor *nxt = m->next;
        /* remove this node from the list of monitored sockets */
        सचे(prev)
          prev->next = nxt;
        अञ्ञथा
          ev->list = nxt;
        curlx_free(m);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      अञ्ञथा {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      भङ्ग;
    }
    prev = m;
    m = m->next; /* move to next node */
  }

  सचे(!found) {
    सचे(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(data, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    अञ्ञथा {
      m = curlx_malloc(परिमाण(संरचना socketmonitor));
      सचे(m) {
        m->next = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(data, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      अञ्ञथा
        पटिनिवत्त CURLE_OUT_OF_MEMORY;
    }
  }

  पटिनिवत्त 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
स्थिर शून्य events_setup(संरचना Curl_multi *multi, संरचना events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

स्थिर CURLcode populate_cpfds(संरचना curl_pollfds *cpfds, संरचना events *ev)
{
  संरचना socketmonitor *m;
  CURLcode result = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  अत्थाय(m = ev->list; m && !result; m = m->next) {
    result = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#सचे DEBUG_EV_POLL
    सचे(!result)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  पटिनिवत्त result;
}

स्थिर CURLcode poll_cpfds(संरचना events *ev,
                           संरचना curl_pollfds *cpfds,
                           पूर्णाङ्क *pollrc)
{
  सचे(cpfds->n) {
    /* wait for activity or timeout */
#सचे DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#endif
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#सचे DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#endif
    सचे(*pollrc < 0)
      पटिनिवत्त CURLE_UNRECOVERABLE_POLL;
  }
  अञ्ञथा {
#सचे DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    सचे(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  पटिनिवत्त CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
स्थिर CURLcode wait_or_timeout(संरचना Curl_multi *multi, संरचना events *ev)
{
  संरचना pollfd fds_on_राशिः[8];
  संरचना curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_राशिः, CURL_ARRAYSIZE(fds_on_राशिः));
  याव(!done) {
    CURLMsg *सन्देशः;
    पूर्णाङ्क pollrc;
    संरचना curltime start;

    result = populate_cpfds(&cpfds, ev);
    सचे(result)
      गच्छ out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    result = poll_cpfds(ev, &cpfds, &pollrc);
    सचे(result)
      गच्छ out;

    ev->msbump = FALSE; /* reset here */

    सचे(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#सचे DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#endif
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    अञ्ञथा {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      unsigned पूर्णाङ्क i;
      अत्थाय(i = 0; i < cpfds.n; i++) {
        सचे(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          पूर्णाङ्क act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      सचे(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        सचे(spent_ms > 0) {
#सचे DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (long)spent_ms);
#endif
          सचे(spent_ms > ev->ms)
            ev->ms = 0;
          अञ्ञथा
            ev->ms -= (long)spent_ms;
        }
      }
    }

    सचे(mresult) {
      result = CURLE_URL_MALFORMAT;
      गच्छ out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    सन्देशः = curl_multi_info_read(multi, &pollrc);
    सचे(सन्देशः) {
      result = सन्देशः->data.result;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  पटिनिवत्त result;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
स्थिर CURLcode easy_events(संरचना Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  स्थिर संरचना events evs = { -1, FALSE, 0, रिक्त, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  पटिनिवत्त wait_or_timeout(multi, &evs);
}
#अञ्ञथा /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

स्थिर CURLcode easy_transfer(संरचना Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  याव(!done && !mresult) {
    पूर्णाङ्क still_running = 0;

    mresult = curl_multi_poll(multi, रिक्त, 0, 1000, रिक्त);

    सचे(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    सचे(!mresult && !still_running) {
      पूर्णाङ्क rc;
      CURLMsg *सन्देशः = curl_multi_info_read(multi, &rc);
      सचे(सन्देशः) {
        result = सन्देशः->data.result;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  सचे(mresult) {
    result = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  पटिनिवत्त result;
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
स्थिर CURLcode easy_perform(संरचना Curl_easy *data, bool events)
{
  संरचना Curl_multi *multi;
  CURLMcode mresult;
  CURLcode result = CURLE_OK;
  संरचना Curl_sigpipe_ctx sigpipe_ctx;

  सचे(!data)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  सचे(data->set.errorbuffer)
    /* clear this as early as possible */
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  सचे(data->multi) {
    failf(data, "easy handle already used in multi handle");
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  सचे(data->conn) {
    संरचना connectdata *conn = data->conn;
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
    DEBUGASSERT(!data->conn);
  }

  सचे(data->multi_easy)
    multi = data->multi_easy;
  अञ्ञथा {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    सचे(!multi)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }

  सचे(Curl_api_multi_is_in_callback(multi))
    पटिनिवत्त CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)data->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)data->set.quick_exit);

  data->multi_easy = रिक्त; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, data);
  सचे(mresult) {
    curl_multi_cleanup(multi);
    सचे(mresult == CURLM_OUT_OF_MEMORY)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  data->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(data, &sigpipe_ctx);

  /* run the transfer */
  result = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (शून्य)Curl_multi_remove_handle(multi, data);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  पटिनिवत्त result;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  संरचना Curl_eapi_guard guard = { 0 };
  CURLcode result;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_perform, &result)) {
    result = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त result;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(संरचना Curl_easy *easy)
{
  संरचना Curl_eapi_guard guard;
  CURLcode result;

  सचे(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &result)) {
    result = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त result;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
शून्य curl_easy_cleanup(CURL *curl)
{
  संरचना Curl_eapi_guard guard;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, रिक्त)) {
    संरचना Curl_easy *data = curl;
    संरचना Curl_sigpipe_ctx sigpipe_ctx;
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
  संरचना Curl_eapi_guard guard;
  CURLcode result;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &result)) {
    संरचना Curl_easy *data = curl;
    va_list arg;
    शून्य *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, शून्य *);

    result = Curl_getinfo(data, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त result;
}

स्थिर CURLcode dupset(संरचना Curl_easy *dst, संरचना Curl_easy *src)
{
  CURLcode result = CURLE_OK;
  गणना dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#सचे !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = रिक्त;
#endif
  dst->set.str_copypostfields = रिक्त;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * परिमाण(संरचना curl_blob *));

  /* duplicate all strings */
  result = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  सचे(result)
    पटिनिवत्त result;

  /* duplicate all blobs */
  अत्थाय(j = (गणना dupblob)0; j < BLOB_LAST; j++) {
    result = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    सचे(result)
      पटिनिवत्त result;
  }

  /* duplicate memory areas pointed to */
  सचे(src->set.str_copypostfields) {
    सचे(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    अञ्ञथा
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    सचे(!dst->set.str_copypostfields)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#सचे !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  सचे(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(परिमाण(*dst->set.mimepostp));
    सचे(!dst->set.mimepostp)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    result = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    सचे(result)
      पटिनिवत्त result;
  }
#endif

  सचे(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  पटिनिवत्त result;
}

स्थिर शून्य dupeasy_meta_freeentry(शून्य *p)
{
  (शून्य)p;
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
  संरचना Curl_eapi_guard guard;
  संरचना Curl_easy *outcurl = रिक्त;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, रिक्त)) {
    संरचना Curl_easy *data = curl;

    outcurl = curlx_calloc(1, परिमाण(संरचना Curl_easy));
    सचे(!outcurl)
      गच्छ fail;

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
    Curl_llist_init(&outcurl->state.httphdrs, रिक्त);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    सचे(dupset(outcurl, data))
      गच्छ fail;

    outcurl->progress.hide     = data->progress.hide;
    outcurl->progress.callback = data->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = रिक्त;
    सचे(data->cookies && data->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      सचे(!outcurl->cookies)
        गच्छ fail;
      outcurl->state.cookie_engine = TRUE;
    }

    सचे(data->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(data->state.cookielist);
      सचे(!outcurl->state.cookielist)
        गच्छ fail;
    }
#endif

    सचे(Curl_bufref_ptr(&data->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&data->state.url), 0,
                      curl_free);
      सचे(!Curl_bufref_ptr(&outcurl->state.url))
        गच्छ fail;
    }
    सचे(Curl_bufref_ptr(&data->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&data->state.referer), 0,
                      curl_free);
      सचे(!Curl_bufref_ptr(&outcurl->state.referer))
        गच्छ fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    सचे(data->asi) {
      ध्रुव वर्ण *str;
      outcurl->asi = Curl_altsvc_init();
      सचे(!outcurl->asi)
        गच्छ fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      सचे(str)
        (शून्य)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    सचे(data->hsts) {
      ध्रुव वर्ण *str;
      outcurl->hsts = Curl_hsts_init();
      सचे(!outcurl->hsts)
        गच्छ fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      सचे(str)
        (शून्य)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (शून्य)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      सचे(Curl_hsts_copy(outcurl->hsts, data->hsts))
        गच्छ fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_सङ्ख्या;
  }
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त outcurl;

fail:
  सचे(outcurl) {
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
  पटिनिवत्त रिक्त;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
शून्य curl_easy_reset(CURL *curl)
{
  संरचना Curl_eapi_guard guard;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_reset, रिक्त)) {
    संरचना Curl_easy *data = curl;

    data->state.last_conn_id = -1; /* clear remembered connection id */
    data->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&data->req, data);
    Curl_hash_clean(&data->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(data);
    /* zero out UserDefined data: */
    Curl_freeset(data);
    memset(&data->set, 0, परिमाण(संरचना UserDefined));
    Curl_init_userdefined(data);

    /* zero out Progress data: */
    memset(&data->progress, 0, परिमाण(संरचना Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(data);

    data->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&data->state.authhost, 0, परिमाण(संरचना auth));
    memset(&data->state.authproxy, 0, परिमाण(संरचना auth));

#सचे !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
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
CURLcode curl_easy_pause(CURL *curl, पूर्णाङ्क action)
{
  संरचना Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_pause, &result)) {
    bool changed = FALSE;
    संरचना Curl_easy *data = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    सचे(!data->conn) {
      /* crazy input, do not continue */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      गच्छ out;
    }

    recv_paused = Curl_xfer_recv_is_paused(data);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(data);
    send_paused_new = (action & CURLPAUSE_SEND);

    सचे((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(data))) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_send(data, send_paused_new));
    }

    सचे(recv_paused != recv_paused_new) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_recv(data, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    सचे(!Curl_xfer_is_blocked(data)) {
      /* reset the too-slow time keeper */
      data->state.keeps_speed.tv_sec = 0;
      सचे(data->multi) {
        Curl_multi_mark_dirty(data); /* make it run */
        /* On changes, tell application to update its timers. */
        सचे(changed && Curl_update_timer(data->multi) && !result)
          result = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    सचे(!result && changed && !data->state.done && data->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(data->multi, data))
      result = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त result;
}

स्थिर CURLcode easy_connection(संरचना Curl_easy *data,
                                संरचना connectdata **connp)
{
  curl_socket_t sfd;

  सचे(!data)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  सचे(!data->set.connect_only) {
    failf(data, "CONNECT_ONLY is required");
    पटिनिवत्त CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(data, connp);

  सचे(sfd == CURL_SOCKET_BAD) {
    failf(data, "Failed to get last socket used for connection #%" FMT_OFF_T,
          data->state.last_conn_id);
    पटिनिवत्त CURLE_UNSUPPORTED_PROTOCOL;
  }

  पटिनिवत्त CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(संरचना Curl_easy *data,
                        शून्य *buffer, size_t buflen, size_t *n)
{
  CURLcode result;
  संरचना connectdata *c;

  result = easy_connection(data, &c);
  सचे(result)
    पटिनिवत्त result;

  सचे(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  *n = 0;
  पटिनिवत्त Curl_conn_recv(data, FIRSTSOCKET, buffer, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, शून्य *buffer, size_t buflen, size_t *n)
{
  संरचना Curl_eapi_guard guard;
  CURLcode result;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_recv, &result)) {
    सचे(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      गच्छ out;
    }
    result = Curl_easy_recv(curl, buffer, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त result;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(संरचना Curl_easy *data)
{
  CURLcode result;
  संरचना connectdata *c = रिक्त;

  result = easy_connection(data, &c);
  सचे(result)
    पटिनिवत्त result;

  सचे(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  पटिनिवत्त CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(संरचना Curl_easy *data, ध्रुव शून्य *buffer,
                       size_t buflen, size_t *n)
{
  CURLcode result;
  संरचना connectdata *c = रिक्त;
  संरचना Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  result = easy_connection(data, &c);
  सचे(result)
    पटिनिवत्त result;

  सचे(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  sigpipe_ignore(data, &sigpipe_ctx);
  result = Curl_conn_send(data, FIRSTSOCKET, buffer, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  सचे(result && result != CURLE_AGAIN)
    पटिनिवत्त CURLE_SEND_ERROR;
  पटिनिवत्त result;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ध्रुव शून्य *buffer, size_t buflen,
                        size_t *n)
{
  संरचना Curl_eapi_guard guard;
  CURLcode result;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_send, &result)) {
    संरचना Curl_easy *data = curl;
    size_t written = 0;

    सचे(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      गच्छ out;
    }
    result = Curl_senddata(data, buffer, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त result;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  संरचना Curl_eapi_guard guard;
  CURLcode result;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &result)) {
    /* Use the common function to keep connections alive. */
    result = Curl_cpool_upkeep((संरचना Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त result;
}

CURLcode curl_easy_ssls_import(CURL *curl, ध्रुव वर्ण *session_कुञ्चिका,
                               ध्रुव unsigned वर्ण *shmac, size_t shmac_len,
                               ध्रुव unsigned वर्ण *sdata, size_t sdata_len)
{
#सचे defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  संरचना Curl_eapi_guard guard;
  CURLcode result;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &result)) {
    सचे(!sdata || !sdata_len)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अञ्ञथा
      result = Curl_ssl_session_import((संरचना Curl_easy *)curl, session_कुञ्चिका,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त result;
#अञ्ञथा
  (शून्य)curl;
  (शून्य)session_कुञ्चिका;
  (शून्य)shmac;
  (शून्य)shmac_len;
  (शून्य)sdata;
  (शून्य)sdata_len;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               शून्य *userptr)
{
#सचे defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  संरचना Curl_eapi_guard guard;
  CURLcode result;

  सचे(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &result)) {
    सचे(!export_fn)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    अञ्ञथा
      result = Curl_ssl_session_export((संरचना Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  पटिनिवत्त result;
#अञ्ञथा
  (शून्य)curl;
  (शून्य)export_fn;
  (शून्य)userptr;
  पटिनिवत्त CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(संरचना Curl_easy *data, ध्रुव वर्ण *कुञ्चिका,
                       शून्य *meta_data, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_data); /* never set to NULL */
  सचे(!Curl_hash_add2(&data->meta_hash, कुञ्चिका, strlen(कुञ्चिका) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(कुञ्चिका, strlen(कुञ्चिका) + 1, meta_data);
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }
  पटिनिवत्त CURLE_OK;
}

शून्य Curl_meta_remove(संरचना Curl_easy *data, ध्रुव वर्ण *कुञ्चिका)
{
  Curl_hash_delete(&data->meta_hash, कुञ्चिका, strlen(कुञ्चिका) + 1);
}

शून्य *Curl_meta_get(संरचना Curl_easy *data, ध्रुव वर्ण *कुञ्चिका)
{
  पटिनिवत्त Curl_hash_pick(&data->meta_hash, कुञ्चिका, strlen(कुञ्चिका) + 1);
}

शून्य Curl_meta_reset(संरचना Curl_easy *data)
{
  Curl_hash_clean(&data->meta_hash);
}
