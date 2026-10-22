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

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#यदि_अन्तः

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
स्थिर अचिह्नित पूर्णाङ्क initialized;
#यदि_परिभाषितम् USE_WINSOCK
स्थिर bool winsock_initialized;
#यदि_अन्तः

#यदि_परिभाषितम् GLOBAL_INIT_IS_THREADSAFE

स्थिर curl_simple_तालकम् s_तालकम् = CURL_SIMPLE_तालकम्_INIT;
#परिभाषय global_init_तालकम्()   curl_simple_तालकम्_तालकम्(&s_तालकम्)
#परिभाषय global_init_unlock() curl_simple_तालकम्_unlock(&s_तालकम्)

#अन्यथा

#परिभाषय global_init_तालकम्()
#परिभाषय global_init_unlock()

#यदि_अन्तः

#यदि defined(_MSC_VER) && defined(_DLL)
#  pragma warning(प्रक्षिप)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#यदि_अन्तः

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)स्मृतिं_देहि;
curl_free_callback Curl_cfree = (curl_free_callback)मुक्त;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_नीचः;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)शून्यस्मृतिं_देहि;

#यदि defined(_MSC_VER) && defined(_DLL)
#  pragma warning(निष्कासय)
#यदि_अन्तः

#यदि_परिभाषितम् DEBUGBUILD
स्थिर वर्ण *leakpointer;
#यदि_अन्तः

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
स्थिर CURLcode global_init(दीर्घ flags, bool memoryfuncs)
{
  (शून्य)flags;

  यदि(initialized++)
    प्रत्यागम CURLE_OK;

  यदि(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)स्मृतिं_देहि;
    Curl_cfree = (curl_free_callback)मुक्त;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_नीचः;
    Curl_ccalloc = (curl_calloc_callback)शून्यस्मृतिं_देहि;
  }

#यदि_परिभाषितम् _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#यदि_अन्तः

  यदि(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    गच्छ fail;
  }

#यदि_परिभाषितम् USE_WINDOWS_SSPI
  यदि(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    गच्छ fail;
  }
#यदि_अन्तः

#यदि_परिभाषितम् USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  यदि(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    यदि(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      गच्छ fail;
    }
    winsock_initialized = TRUE;
  }
#यदि_अन्तः

  यदि(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    गच्छ fail;
  }

  यदि(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    गच्छ fail;
  }

  यदि(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    गच्छ fail;
  }

  यदि(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    गच्छ fail;
  }

  यदि(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    गच्छ fail;
  }

  यदि(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    गच्छ fail;
  }

  यदि(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    गच्छ fail;
  }

#यदि_परिभाषितम् DEBUGBUILD
  यदि(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#यदि_अन्तः

  प्रत्यागम CURLE_OK;

fail:
#यदि_परिभाषितम् USE_WINSOCK
  यदि(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#यदि_अन्तः
  initialized--; /* undo the increase */
  प्रत्यागम CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(दीर्घ flags)
{
  CURLcode परिणाम;
  global_init_तालकम्();

  परिणाम = global_init(flags, TRUE);

  global_init_unlock();

  प्रत्यागम परिणाम;
}

/*
 * curl_global_init_mem() globally initializes curl and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(दीर्घ flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode परिणाम;

  /* Invalid input, return immediately */
  यदि(!m || !f || !r || !s || !c)
    प्रत्यागम CURLE_FAILED_INIT;

  global_init_तालकम्();

  यदि(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    प्रत्यागम CURLE_OK;
  }

  /* set memory functions before global_init() in case it wants memory
     functions */
  Curl_cmalloc = m;
  Curl_cfree = f;
  Curl_cstrdup = s;
  Curl_crealloc = r;
  Curl_ccalloc = c;

  /* Call the actual init function, but without setting */
  परिणाम = global_init(flags, FALSE);

  global_init_unlock();

  प्रत्यागम परिणाम;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
शून्य curl_global_cleanup(शून्य)
{
  global_init_तालकम्();

  यदि(!initialized) {
    global_init_unlock();
    प्रत्यागम;
  }

  यदि(--initialized) {
    global_init_unlock();
    प्रत्यागम;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#यदि_परिभाषितम् USE_WINSOCK
  यदि(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#यदि_अन्तः

#यदि_परिभाषितम् USE_WINDOWS_SSPI
  Curl_sspi_global_cleanup();
#यदि_अन्तः

#यदि_परिभाषितम् DEBUGBUILD
  curlx_free(leakpointer);
#यदि_अन्तः

  global_init_unlock();
}

/**
 * curl_global_trace() globally initializes curl logging.
 */
CURLcode curl_global_trace(ध्रुव वर्ण *config)
{
#यदि_अपरिभाषितम् CURL_DISABLE_VERBOSE_वाक्यानि
  CURLcode परिणाम;
  global_init_तालकम्();

  परिणाम = Curl_trc_opt(config);

  global_init_unlock();

  प्रत्यागम परिणाम;
#अन्यथा
  (शून्य)config;
  प्रत्यागम CURLE_OK;
#यदि_अन्तः
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ध्रुव वर्ण *name,
                              ध्रुव curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_तालकम्();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  प्रत्यागम rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(शून्य)
{
  CURLcode परिणाम;
  संरचना Curl_easy *दत्त;

  /* Make sure we inited the global SSL stuff */
  global_init_तालकम्();

  यदि(!initialized) {
    परिणाम = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    यदि(परिणाम) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      प्रत्यागम रिक्त;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  परिणाम = Curl_open(&दत्त);
  यदि(परिणाम) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    प्रत्यागम रिक्त;
  }

  प्रत्यागम दत्त;
}

#यदि_परिभाषितम् DEBUGBUILD

संरचना socketmonitor {
  संरचना socketmonitor *अग्र; /* the next node in the list or NULL */
  संरचना pollfd socket; /* socket info of what to monitor */
};

संरचना events {
  दीर्घ ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  पूर्णाङ्क अङ्कः_sockets;      /* number of nodes in the monitor list */
  संरचना socketmonitor *list; /* list of sockets to monitor */
  पूर्णाङ्क running_handles;  /* store the returned number */
};

#परिभाषय DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
स्थिर पूर्णाङ्क events_timer(CURLM *multi,    /* multi handle */
                        दीर्घ timeout_ms, /* see above */
                        शून्य *userp)     /* private callback pointer */
{
  संरचना events *ev = userp;
  (शून्य)multi;
#यदि DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#यदि_अन्तः
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  प्रत्यागम 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
स्थिर पूर्णाङ्क poll2cselect(पूर्णाङ्क pollmask)
{
  पूर्णाङ्क omask = 0;
  यदि(pollmask & POLLIN)
    omask |= CURL_CSELECT_अन्तर्;
  यदि(pollmask & POLLOUT)
    omask |= CURL_CSELECT_बहिर्गमनम्;
  यदि(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  प्रत्यागम omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
स्थिर ह्रस्व socketcb2poll(पूर्णाङ्क pollmask)
{
  ह्रस्व omask = 0;
  यदि(pollmask & CURL_POLL_अन्तर्)
    omask |= POLLIN;
  यदि(pollmask & CURL_POLL_बहिर्गमनम्)
    omask |= POLLOUT;
  प्रत्यागम omask;
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
  संरचना socketmonitor *पूर्व = रिक्त;
  bool प्राप्तम् = FALSE;
  संरचना Curl_easy *दत्त = easy;

#यदि_परिभाषितम् CURL_DISABLE_VERBOSE_वाक्यानि
  (शून्य)easy;
#यदि_अन्तः
  (शून्य)socketp;

  m = ev->list;
  यावत्(m) {
    यदि(m->socket.fd == s) {
      प्राप्तम् = TRUE;
      यदि(what == CURL_POLL_REMOVE) {
        संरचना socketmonitor *nxt = m->अग्र;
        /* remove this node from the list of monitored sockets */
        यदि(पूर्व)
          पूर्व->अग्र = nxt;
        अन्यथा
          ev->list = nxt;
        curlx_free(m);
        infof(दत्त, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      अन्यथा {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(दत्त, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_अन्तर्) ? "IN" : "",
              (what & CURL_POLL_बहिर्गमनम्) ? "OUT" : "");
      }
      भङ्ग;
    }
    पूर्व = m;
    m = m->अग्र; /* move to next node */
  }

  यदि(!प्राप्तम्) {
    यदि(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(दत्त, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    अन्यथा {
      m = curlx_malloc(परिमाण(संरचना socketmonitor));
      यदि(m) {
        m->अग्र = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(दत्त, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_अन्तर्) ? "IN" : "",
              (what & CURL_POLL_बहिर्गमनम्) ? "OUT" : "");
      }
      अन्यथा
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  प्रत्यागम 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
स्थिर शून्य events_सज्जता(संरचना Curl_multi *multi, संरचना events *ev)
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
  CURLcode परिणाम = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  प्रति(m = ev->list; m && !परिणाम; m = m->अग्र) {
    परिणाम = Curl_pollfds_सङ्कलय_sock(cpfds, m->socket.fd, m->socket.events);
#यदि DEBUG_EV_POLL
    यदि(!परिणाम)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#यदि_अन्तः
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode poll_cpfds(संरचना events *ev,
                           संरचना curl_pollfds *cpfds,
                           पूर्णाङ्क *pollrc)
{
  यदि(cpfds->n) {
    /* wait for activity or timeout */
#यदि DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#यदि_अन्तः
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#यदि DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#यदि_अन्तः
    यदि(*pollrc < 0)
      प्रत्यागम CURLE_UNRECOVERABLE_POLL;
  }
  अन्यथा {
#यदि DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#यदि_अन्तः
    *pollrc = 0;
    यदि(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  प्रत्यागम CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
स्थिर CURLcode wait_or_timeout(संरचना Curl_multi *multi, संरचना events *ev)
{
  संरचना pollfd fds_on_राशिः[8];
  संरचना curl_pollfds cpfds;
  bool समाप्तम् = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_राशिः, CURL_ARRAYSIZE(fds_on_राशिः));
  यावत्(!समाप्तम्) {
    CURLMsg *सन्देशः;
    पूर्णाङ्क pollrc;
    संरचना curltime start;

    परिणाम = populate_cpfds(&cpfds, ev);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    परिणाम = poll_cpfds(ev, &cpfds, &pollrc);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    ev->msbump = FALSE; /* reset here */

    यदि(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#यदि DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#यदि_अन्तः
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    अन्यथा {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      अचिह्नित पूर्णाङ्क i;
      प्रति(i = 0; i < cpfds.n; i++) {
        यदि(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          पूर्णाङ्क act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      यदि(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        यदि(spent_ms > 0) {
#यदि DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (दीर्घ)spent_ms);
#यदि_अन्तः
          यदि(spent_ms > ev->ms)
            ev->ms = 0;
          अन्यथा
            ev->ms -= (दीर्घ)spent_ms;
        }
      }
    }

    यदि(mresult) {
      परिणाम = CURLE_URL_MALFORMAT;
      गच्छ बहिर्गमनम्;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    सन्देशः = curl_multi_info_read(multi, &pollrc);
    यदि(सन्देशः) {
      परिणाम = सन्देशः->दत्त.परिणाम;
      समाप्तम् = TRUE;
    }
  }
बहिर्गमनम्:
  Curl_pollfds_cleanup(&cpfds);
  प्रत्यागम परिणाम;
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
  events_सज्जता(multi, &evs);

  प्रत्यागम wait_or_timeout(multi, &evs);
}
#अन्यथा /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#परिभाषय easy_events(x) CURLE_NOT_BUILT_अन्तर्
#यदि_अन्तः

स्थिर CURLcode easy_transfer(संरचना Curl_multi *multi)
{
  bool समाप्तम् = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = CURLE_OK;

  यावत्(!समाप्तम् && !mresult) {
    पूर्णाङ्क still_running = 0;

    mresult = curl_multi_poll(multi, रिक्त, 0, 1000, रिक्त);

    यदि(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    यदि(!mresult && !still_running) {
      पूर्णाङ्क rc;
      CURLMsg *सन्देशः = curl_multi_info_read(multi, &rc);
      यदि(सन्देशः) {
        परिणाम = सन्देशः->दत्त.परिणाम;
        समाप्तम् = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  यदि(mresult) {
    परिणाम = (mresult == CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः) ? CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_तर्कः;
  }

  प्रत्यागम परिणाम;
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
स्थिर CURLcode easy_perform(संरचना Curl_easy *दत्त, bool events)
{
  संरचना Curl_multi *multi;
  CURLMcode mresult;
  CURLcode परिणाम = CURLE_OK;
  संरचना Curl_sigpipe_ctx sigpipe_ctx;

  यदि(!दत्त)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यदि(दत्त->समुच्चयः.errorbuffer)
    /* clear this as early as possible */
    दत्त->समुच्चयः.errorbuffer[0] = 0;

  दत्त->state.os_errno = 0;

  यदि(दत्त->multi) {
    failf(दत्त, "easy handle already used in multi handle");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  यदि(दत्त->conn) {
    संरचना connectdata *conn = दत्त->conn;
    Curl_detach_connection(दत्त);
    Curl_conn_close(दत्त, conn, TRUE);
    DEBUGASSERT(!दत्त->conn);
  }

  यदि(दत्त->multi_easy)
    multi = दत्त->multi_easy;
  अन्यथा {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    यदि(!multi)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(Curl_api_multi_is_अन्तर्_callback(multi))
    प्रत्यागम CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (दीर्घ)दत्त->समुच्चयः.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_शीघ्र_EXIT, (दीर्घ)दत्त->समुच्चयः.quick_exit);

  दत्त->multi_easy = रिक्त; /* pretend it does not exist */
  mresult = Curl_multi_सङ्कलय_handle(multi, दत्त);
  यदि(mresult) {
    curl_multi_cleanup(multi);
    यदि(mresult == CURLM_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    प्रत्यागम CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  दत्त->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(दत्त, &sigpipe_ctx);

  /* run the transfer */
  परिणाम = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (शून्य)Curl_multi_remove_handle(multi, दत्त);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  प्रत्यागम परिणाम;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  संरचना Curl_eapi_guard guard = { 0 };
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_perform, &परिणाम)) {
    परिणाम = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(संरचना Curl_easy *easy)
{
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &परिणाम)) {
    परिणाम = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
शून्य curl_easy_cleanup(CURL *curl)
{
  संरचना Curl_eapi_guard guard;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, रिक्त)) {
    संरचना Curl_easy *दत्त = curl;
    संरचना Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(दत्त, &sigpipe_ctx);
    Curl_close(&दत्त);
    sigpipe_restore(&sigpipe_ctx);
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#अपरिभाषय curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &परिणाम)) {
    संरचना Curl_easy *दत्त = curl;
    va_list arg;
    शून्य *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, शून्य *);

    परिणाम = Curl_getinfo(दत्त, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode dupset(संरचना Curl_easy *dst, संरचना Curl_easy *src)
{
  CURLcode परिणाम = CURLE_OK;
  गणना dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->समुच्चयः = src->समुच्चयः;
#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->समुच्चयः.mimepostp = रिक्त;
#यदि_अन्तः
  dst->समुच्चयः.str_copypostfields = रिक्त;

  Curl_u8_strset_init(&dst->समुच्चयः.वाक्यानि);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  स्मृतिं_पूरय(dst->समुच्चयः.blobs, 0, BLOB_चरमः * परिमाण(संरचना curl_blob *));

  /* duplicate all strings */
  परिणाम = Curl_u8_strset_copy(&dst->समुच्चयः.वाक्यानि, &src->समुच्चयः.वाक्यानि);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* duplicate all blobs */
  प्रति(j = (गणना dupblob)0; j < BLOB_चरमः; j++) {
    परिणाम = Curl_setblobopt(&dst->समुच्चयः.blobs[j], src->समुच्चयः.blobs[j]);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  /* duplicate memory areas pointed to */
  यदि(src->समुच्चयः.str_copypostfields) {
    यदि(src->समुच्चयः.postfieldsize == -1)
      dst->समुच्चयः.str_copypostfields = curlx_strdup(src->समुच्चयः.str_copypostfields);
    अन्यथा
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->समुच्चयः.str_copypostfields =
        curlx_memdup0(src->समुच्चयः.str_copypostfields,
                      curlx_sotouz(src->समुच्चयः.postfieldsize));
    यदि(!dst->समुच्चयः.str_copypostfields)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    /* point to the new copy */
    dst->समुच्चयः.postfields = dst->समुच्चयः.str_copypostfields;
  }

#यदि !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  यदि(src->समुच्चयः.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->समुच्चयः.mimepostp = curlx_malloc(परिमाण(*dst->समुच्चयः.mimepostp));
    यदि(!dst->समुच्चयः.mimepostp)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    Curl_mime_initpart(dst->समुच्चयः.mimepostp);
    परिणाम = Curl_mime_duppart(dst, dst->समुच्चयः.mimepostp, src->समुच्चयः.mimepostp);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
#यदि_अन्तः

  यदि(src->समुच्चयः.resolve)
    dst->state.resolve = dst->समुच्चयः.resolve;

  प्रत्यागम परिणाम;
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

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, रिक्त)) {
    संरचना Curl_easy *दत्त = curl;

    outcurl = curlx_calloc(1, परिमाण(संरचना Curl_easy));
    यदि(!outcurl)
      गच्छ fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->समुच्चयः.बफर_आकार = दत्त->समुच्चयः.बफर_आकार;

    Curl_hash_init(&outcurl->meta_hash, 23, CURL_HASH_TYPE_अष्टकानि,
                   dupeasy_meta_freeentry);
    curlx_dyn_init(&outcurl->state.headerb, CURL_MAX_HTTP_शीर्षकम्);
    Curl_bufref_init(&outcurl->state.url);
    Curl_bufref_init(&outcurl->state.referer);
    Curl_netrc_init(&outcurl->state.netrc);

    /* the connection pool is setup on demand */
    outcurl->state.चरमः_conn_id = -1;
    outcurl->state.चरमः_cpid = UINT32_MAX;
    outcurl->id = -1;
    outcurl->mid = UINT32_MAX;
    outcurl->master_mid = UINT32_MAX;

#यदि_अपरिभाषितम् CURL_DISABLE_HTTP
    Curl_llist_init(&outcurl->state.httphdrs, रिक्त);
#यदि_अन्तः
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    यदि(dupset(outcurl, दत्त))
      गच्छ fail;

    outcurl->प्रगतिः.hide     = दत्त->प्रगतिः.hide;
    outcurl->प्रगतिः.callback = दत्त->प्रगतिः.callback;

#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
    outcurl->state.cookielist = रिक्त;
    यदि(दत्त->cookies && दत्त->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      यदि(!outcurl->cookies)
        गच्छ fail;
      outcurl->state.cookie_engine = TRUE;
    }

    यदि(दत्त->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(दत्त->state.cookielist);
      यदि(!outcurl->state.cookielist)
        गच्छ fail;
    }
#यदि_अन्तः

    यदि(Curl_bufref_ptr(&दत्त->state.url)) {
      Curl_bufref_समुच्चयः(&outcurl->state.url,
                      Curl_bufref_dup(&दत्त->state.url), 0,
                      curl_free);
      यदि(!Curl_bufref_ptr(&outcurl->state.url))
        गच्छ fail;
    }
    यदि(Curl_bufref_ptr(&दत्त->state.referer)) {
      Curl_bufref_समुच्चयः(&outcurl->state.referer,
                      Curl_bufref_dup(&दत्त->state.referer), 0,
                      curl_free);
      यदि(!Curl_bufref_ptr(&outcurl->state.referer))
        गच्छ fail;
    }

#यदि_अपरिभाषितम् CURL_DISABLE_ALTSVC
    यदि(दत्त->asi) {
      ध्रुव वर्ण *str;
      outcurl->asi = Curl_altsvc_init();
      यदि(!outcurl->asi)
        गच्छ fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      यदि(str)
        (शून्य)Curl_altsvc_आरोपय(outcurl->asi, str);
    }
#यदि_अन्तः
#यदि_अपरिभाषितम् CURL_DISABLE_HSTS
    यदि(दत्त->hsts) {
      ध्रुव वर्ण *str;
      outcurl->hsts = Curl_hsts_init();
      यदि(!outcurl->hsts)
        गच्छ fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      यदि(str)
        (शून्य)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (शून्य)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      यदि(Curl_hsts_copy(outcurl->hsts, दत्त->hsts))
        गच्छ fail;
    }
#यदि_अन्तः

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_सङ्ख्या;
  }
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम outcurl;

fail:
  यदि(outcurl) {
#यदि_अपरिभाषितम् CURL_DISABLE_COOKIES
    curlx_free(outcurl->cookies);
#यदि_अन्तः
    curlx_dyn_free(&outcurl->state.headerb);
    Curl_altsvc_cleanup(&outcurl->asi);
    Curl_hsts_cleanup(&outcurl->hsts);
    Curl_freeset(outcurl);
    curlx_free(outcurl);
  }

  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम रिक्त;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
शून्य curl_easy_reset(CURL *curl)
{
  संरचना Curl_eapi_guard guard;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_reset, रिक्त)) {
    संरचना Curl_easy *दत्त = curl;

    दत्त->state.चरमः_conn_id = -1; /* clear remembered connection id */
    दत्त->state.चरमः_cpid = UINT32_MAX;
    Curl_req_hard_reset(&दत्त->req, दत्त);
    Curl_hash_clean(&दत्त->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(दत्त);
    /* zero out UserDefined data: */
    Curl_freeset(दत्त);
    स्मृतिं_पूरय(&दत्त->समुच्चयः, 0, परिमाण(संरचना UserDefined));
    Curl_init_userdefined(दत्त);

    /* zero out Progress data: */
    स्मृतिं_पूरय(&दत्त->प्रगतिः, 0, परिमाण(संरचना प्रगतिः_2));

    /* zero out PureInfo data: */
    Curl_initinfo(दत्त);

    दत्त->प्रगतिः.hide = TRUE;

    /* zero out authentication data: */
    स्मृतिं_पूरय(&दत्त->state.authhost, 0, परिमाण(संरचना auth));
    स्मृतिं_पूरय(&दत्त->state.authproxy, 0, परिमाण(संरचना auth));

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(दत्त);
#यदि_अन्तः
    दत्त->master_mid = UINT32_MAX;
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
  CURLcode परिणाम = CURLE_OK;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_pause, &परिणाम)) {
    bool परिवर्तितम् = FALSE;
    संरचना Curl_easy *दत्त = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    यदि(!दत्त->conn) {
      /* crazy input, do not continue */
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }

    recv_paused = Curl_xfer_recv_is_paused(दत्त);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(दत्त);
    send_paused_new = (action & CURLPAUSE_SEND);

    यदि((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(दत्त))) {
      परिवर्तितम् = TRUE;
      परिणाम = Curl_1st_fatal(
        परिणाम, Curl_xfer_pause_send(दत्त, send_paused_new));
    }

    यदि(recv_paused != recv_paused_new) {
      परिवर्तितम् = TRUE;
      परिणाम = Curl_1st_fatal(
        परिणाम, Curl_xfer_pause_recv(दत्त, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    यदि(!Curl_xfer_is_blocked(दत्त)) {
      /* reset the too-slow time keeper */
      दत्त->state.keeps_जवः.tv_sec = 0;
      यदि(दत्त->multi) {
        Curl_multi_mark_dirty(दत्त); /* make it run */
        /* On changes, tell application to update its timers. */
        यदि(परिवर्तितम् && Curl_update_timer(दत्त->multi) && !परिणाम)
          परिणाम = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    यदि(!परिणाम && परिवर्तितम् && !दत्त->state.समाप्तम् && दत्त->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(दत्त->multi, दत्त))
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
  }
बहिर्गमनम्:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode easy_connection(संरचना Curl_easy *दत्त,
                                संरचना connectdata **connp)
{
  curl_socket_t sfd;

  यदि(!दत्त)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  यदि(!दत्त->समुच्चयः.connect_only) {
    failf(दत्त, "CONNECT_ONLY is required");
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(दत्त, connp);

  यदि(sfd == CURL_SOCKET_BAD) {
    failf(दत्त, "Failed to get last socket used for connection #%" FMT_OFF_T,
          दत्त->state.चरमः_conn_id);
    प्रत्यागम CURLE_UNSUPPORTED_PROTOCOL;
  }

  प्रत्यागम CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(संरचना Curl_easy *दत्त,
                        शून्य *बफर, size_t buflen, size_t *n)
{
  CURLcode परिणाम;
  संरचना connectdata *c;

  परिणाम = easy_connection(दत्त, &c);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(!दत्त->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(दत्त, c, TRUE);

  *n = 0;
  प्रत्यागम Curl_conn_recv(दत्त, FIRSTSOCKET, बफर, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, शून्य *बफर, size_t buflen, size_t *n)
{
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_recv, &परिणाम)) {
    यदि(!n || (buflen && !बफर)) {
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }
    परिणाम = Curl_easy_recv(curl, बफर, buflen, n);
  }
बहिर्गमनम्:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम;
  संरचना connectdata *c = रिक्त;

  परिणाम = easy_connection(दत्त, &c);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(!दत्त->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(दत्त, c, TRUE);

  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(संरचना Curl_easy *दत्त, ध्रुव शून्य *बफर,
                       size_t buflen, size_t *n)
{
  CURLcode परिणाम;
  संरचना connectdata *c = रिक्त;
  संरचना Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  परिणाम = easy_connection(दत्त, &c);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(!दत्त->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(दत्त, c, TRUE);

  sigpipe_ignore(दत्त, &sigpipe_ctx);
  परिणाम = Curl_conn_send(दत्त, FIRSTSOCKET, बफर, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  यदि(परिणाम && परिणाम != CURLE_AGAIN)
    प्रत्यागम CURLE_SEND_ERROR;
  प्रत्यागम परिणाम;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ध्रुव शून्य *बफर, size_t buflen,
                        size_t *n)
{
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_send, &परिणाम)) {
    संरचना Curl_easy *दत्त = curl;
    size_t written = 0;

    यदि(!n || (buflen && !बफर)) {
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }
    परिणाम = Curl_senddata(दत्त, बफर, buflen, &written);
    *n = written;
  }
बहिर्गमनम्:
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &परिणाम)) {
    /* Use the common function to keep connections alive. */
    परिणाम = Curl_cpool_upkeep((संरचना Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
}

CURLcode curl_easy_ssls_import(CURL *curl, ध्रुव वर्ण *session_कुञ्चिका,
                               ध्रुव अचिह्नित वर्ण *shmac, size_t shmac_len,
                               ध्रुव अचिह्नित वर्ण *sdata, size_t sdata_len)
{
#यदि defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &परिणाम)) {
    यदि(!sdata || !sdata_len)
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    अन्यथा
      परिणाम = Curl_ssl_session_import((संरचना Curl_easy *)curl, session_कुञ्चिका,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
#अन्यथा
  (शून्य)curl;
  (शून्य)session_कुञ्चिका;
  (शून्य)shmac;
  (शून्य)shmac_len;
  (शून्य)sdata;
  (शून्य)sdata_len;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               शून्य *userptr)
{
#यदि defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  संरचना Curl_eapi_guard guard;
  CURLcode परिणाम;

  यदि(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &परिणाम)) {
    यदि(!export_fn)
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    अन्यथा
      परिणाम = Curl_ssl_session_export((संरचना Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  प्रत्यागम परिणाम;
#अन्यथा
  (शून्य)curl;
  (शून्य)export_fn;
  (शून्य)userptr;
  प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
}

CURLcode Curl_meta_समुच्चयः(संरचना Curl_easy *दत्त, ध्रुव वर्ण *कुञ्चिका,
                       शून्य *meta_दत्त, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_दत्त); /* never set to NULL */
  यदि(!Curl_hash_add2(&दत्त->meta_hash, कुञ्चिका, वाक्यदैर्घ्यम्(कुञ्चिका) + 1,
                     meta_दत्त, meta_dtor)) {
    meta_dtor(कुञ्चिका, वाक्यदैर्घ्यम्(कुञ्चिका) + 1, meta_दत्त);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  प्रत्यागम CURLE_OK;
}

शून्य Curl_meta_remove(संरचना Curl_easy *दत्त, ध्रुव वर्ण *कुञ्चिका)
{
  Curl_hash_delete(&दत्त->meta_hash, कुञ्चिका, वाक्यदैर्घ्यम्(कुञ्चिका) + 1);
}

शून्य *Curl_meta_get(संरचना Curl_easy *दत्त, ध्रुव वर्ण *कुञ्चिका)
{
  प्रत्यागम Curl_hash_pick(&दत्त->meta_hash, कुञ्चिका, वाक्यदैर्घ्यम्(कुञ्चिका) + 1);
}

शून्य Curl_meta_reset(संरचना Curl_easy *दत्त)
{
  Curl_hash_clean(&दत्त->meta_hash);
}
