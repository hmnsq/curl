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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "api.h"
#समावेश "transfer.h"
#समावेश "vdns/hostip.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "vquic/vquic.h"
#समावेश "url.h"
#समावेश "getinfo.h"
#समावेश "curlx/strdup.h"
#समावेश "easyif.h"
#समावेश "multiif.h"
#समावेश "multi_ev.h"
#समावेश "select.h"
#समावेश "cfilters.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "connect.h" /* for Curl_getconnectinfo */
#समावेश "slist.h"
#समावेश "mime.h"
#समावेश "amigaos.h"
#समावेश "macos.h"
#समावेश "curlx/wait.h"
#समावेश "sigpipe.h"
#समावेश "vssh/ssh.h"
#समावेश "setopt.h"
#समावेश "http_digest.h"
#समावेश "curlx/dynbuf.h"
#समावेश "bufref.h"
#समावेश "altsvc.h"
#समावेश "hsts.h"
#समावेश "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#समावेश "curlx/timeval.h" /* for curlx_now_init() */
#समावेश "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#समावेश "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
अटल अचिन्हित पूर्णांक initialized;
#अगर_घोषित USE_WINSOCK
अटल bool winsock_initialized;
#पूर्ण_अगर

#अगर_घोषित GLOBAL_INIT_IS_THREADSAFE

अटल curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#घोषणा global_init_lock()   curl_simple_lock_lock(&s_lock)
#घोषणा global_init_unlock() curl_simple_lock_unlock(&s_lock)

#अन्यथा

#घोषणा global_init_lock()
#घोषणा global_init_unlock()

#पूर्ण_अगर

#अगर defined(_MSC_VER) && defined(_DLL)
#  pragma warning(धकेलो)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#पूर्ण_अगर

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)दो_स्मृति;
curl_free_callback Curl_cfree = (curl_free_callback)मुक्त;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)पुनः_स्मृति;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_निचला;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)सुस्मृति;

#अगर defined(_MSC_VER) && defined(_DLL)
#  pragma warning(निकालो)
#पूर्ण_अगर

#अगर_घोषित DEBUGBUILD
अटल अक्षर *leakpointer;
#पूर्ण_अगर

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
अटल CURLcode global_init(दीर्घ flags, bool memoryfuncs)
{
  (व्योम)flags;

  अगर(initialized++)
    वापस CURLE_OK;

  अगर(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)दो_स्मृति;
    Curl_cfree = (curl_free_callback)मुक्त;
    Curl_crealloc = (curl_realloc_callback)पुनः_स्मृति;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_निचला;
    Curl_ccalloc = (curl_calloc_callback)सुस्मृति;
  }

#अगर_घोषित _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#पूर्ण_अगर

  अगर(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_trc_init failed\n"));
    जाओ fail;
  }

#अगर_घोषित USE_WINDOWS_SSPI
  अगर(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_sspi_global_init() failed\n"));
    जाओ fail;
  }
#पूर्ण_अगर

#अगर_घोषित USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  अगर(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    अगर(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: WSAStartup() failed\n"));
      जाओ fail;
    }
    winsock_initialized = TRUE;
  }
#पूर्ण_अगर

  अगर(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_ssl_init failed\n"));
    जाओ fail;
  }

  अगर(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_hash_global_init failed\n"));
    जाओ fail;
  }

  अगर(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_vquic_init failed\n"));
    जाओ fail;
  }

  अगर(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_amiga_init failed\n"));
    जाओ fail;
  }

  अगर(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_macos_init failed\n"));
    जाओ fail;
  }

  अगर(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_async_global_init failed\n"));
    जाओ fail;
  }

  अगर(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_ssh_init failed\n"));
    जाओ fail;
  }

#अगर_घोषित DEBUGBUILD
  अगर(दो_पर्या("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#पूर्ण_अगर

  वापस CURLE_OK;

fail:
#अगर_घोषित USE_WINSOCK
  अगर(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#पूर्ण_अगर
  initialized--; /* undo the increase */
  वापस CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(दीर्घ flags)
{
  CURLcode परिणाम;
  global_init_lock();

  परिणाम = global_init(flags, TRUE);

  global_init_unlock();

  वापस परिणाम;
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
  अगर(!m || !f || !r || !s || !c)
    वापस CURLE_FAILED_INIT;

  global_init_lock();

  अगर(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    वापस CURLE_OK;
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

  वापस परिणाम;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
व्योम curl_global_cleanup(व्योम)
{
  global_init_lock();

  अगर(!initialized) {
    global_init_unlock();
    वापस;
  }

  अगर(--initialized) {
    global_init_unlock();
    वापस;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#अगर_घोषित USE_WINSOCK
  अगर(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#पूर्ण_अगर

#अगर_घोषित USE_WINDOWS_SSPI
  Curl_sspi_global_cleanup();
#पूर्ण_अगर

#अगर_घोषित DEBUGBUILD
  curlx_free(leakpointer);
#पूर्ण_अगर

  global_init_unlock();
}

/**
 * curl_global_trace() globally initializes curl logging.
 */
CURLcode curl_global_trace(स्थिर अक्षर *config)
{
#अगर_अघोषित CURL_DISABLE_VERBOSE_STRINGS
  CURLcode परिणाम;
  global_init_lock();

  परिणाम = Curl_trc_opt(config);

  global_init_unlock();

  वापस परिणाम;
#अन्यथा
  (व्योम)config;
  वापस CURLE_OK;
#पूर्ण_अगर
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, स्थिर अक्षर *name,
                              स्थिर curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  वापस rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(व्योम)
{
  CURLcode परिणाम;
  काष्ठा Curl_easy *तथ्य;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  अगर(!initialized) {
    परिणाम = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    अगर(परिणाम) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: curl_global_init failed\n"));
      global_init_unlock();
      वापस शून्य;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  परिणाम = Curl_open(&तथ्य);
  अगर(परिणाम) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: Curl_open failed\n"));
    वापस शून्य;
  }

  वापस तथ्य;
}

#अगर_घोषित DEBUGBUILD

काष्ठा socketmonitor {
  काष्ठा socketmonitor *अग्र; /* the next node in the list or NULL */
  काष्ठा pollfd socket; /* socket info of what to monitor */
};

काष्ठा events {
  दीर्घ ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  पूर्णांक अंक_sockets;      /* number of nodes in the monitor list */
  काष्ठा socketmonitor *list; /* list of sockets to monitor */
  पूर्णांक running_handles;  /* store the returned number */
};

#घोषणा DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
अटल पूर्णांक events_timer(CURLM *multi,    /* multi handle */
                        दीर्घ timeout_ms, /* see above */
                        व्योम *userp)     /* private callback pointer */
{
  काष्ठा events *ev = userp;
  (व्योम)multi;
#अगर DEBUG_EV_POLL
  curl_mfprintf(मानक_त्रुटि, "events_timer: set timeout %ldms\n", timeout_ms);
#पूर्ण_अगर
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  वापस 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
अटल पूर्णांक poll2cselect(पूर्णांक pollmask)
{
  पूर्णांक omask = 0;
  अगर(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  अगर(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  अगर(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  वापस omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
अटल लघु socketcb2poll(पूर्णांक pollmask)
{
  लघु omask = 0;
  अगर(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  अगर(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  वापस omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
अटल पूर्णांक events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         पूर्णांक what,        /* see above */
                         व्योम *userp,     /* private callback
                                             pointer */
                         व्योम *socketp)   /* private socket
                                             pointer */
{
  काष्ठा events *ev = userp;
  काष्ठा socketmonitor *m;
  काष्ठा socketmonitor *पूर्व = शून्य;
  bool found = FALSE;
  काष्ठा Curl_easy *तथ्य = easy;

#अगर_घोषित CURL_DISABLE_VERBOSE_STRINGS
  (व्योम)easy;
#पूर्ण_अगर
  (व्योम)socketp;

  m = ev->list;
  जबतक(m) {
    अगर(m->socket.fd == s) {
      found = TRUE;
      अगर(what == CURL_POLL_REMOVE) {
        काष्ठा socketmonitor *nxt = m->अग्र;
        /* remove this node from the list of monitored sockets */
        अगर(पूर्व)
          पूर्व->अग्र = nxt;
        अन्यथा
          ev->list = nxt;
        curlx_free(m);
        infof(तथ्य, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      अन्यथा {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(तथ्य, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      अवरोध;
    }
    पूर्व = m;
    m = m->अग्र; /* move to next node */
  }

  अगर(!found) {
    अगर(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(तथ्य, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    अन्यथा {
      m = curlx_malloc(माप(काष्ठा socketmonitor));
      अगर(m) {
        m->अग्र = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(तथ्य, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      अन्यथा
        वापस CURLE_OUT_OF_MEMORY;
    }
  }

  वापस 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
अटल व्योम events_setup(काष्ठा Curl_multi *multi, काष्ठा events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

अटल CURLcode populate_cpfds(काष्ठा curl_pollfds *cpfds, काष्ठा events *ev)
{
  काष्ठा socketmonitor *m;
  CURLcode परिणाम = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  क्रम(m = ev->list; m && !परिणाम; m = m->अग्र) {
    परिणाम = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#अगर DEBUG_EV_POLL
    अगर(!परिणाम)
      curl_mfprintf(मानक_त्रुटि, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#पूर्ण_अगर
  }
  वापस परिणाम;
}

अटल CURLcode poll_cpfds(काष्ठा events *ev,
                           काष्ठा curl_pollfds *cpfds,
                           पूर्णांक *pollrc)
{
  अगर(cpfds->n) {
    /* wait for activity or timeout */
#अगर DEBUG_EV_POLL
    curl_mfprintf(मानक_त्रुटि, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#पूर्ण_अगर
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#अगर DEBUG_EV_POLL
    curl_mfprintf(मानक_त्रुटि, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#पूर्ण_अगर
    अगर(*pollrc < 0)
      वापस CURLE_UNRECOVERABLE_POLL;
  }
  अन्यथा {
#अगर DEBUG_EV_POLL
    curl_mfprintf(मानक_त्रुटि, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#पूर्ण_अगर
    *pollrc = 0;
    अगर(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  वापस CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
अटल CURLcode wait_or_timeout(काष्ठा Curl_multi *multi, काष्ठा events *ev)
{
  काष्ठा pollfd fds_on_ढेर[8];
  काष्ठा curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_ढेर, CURL_ARRAYSIZE(fds_on_ढेर));
  जबतक(!done) {
    CURLMsg *संदेश;
    पूर्णांक pollrc;
    काष्ठा curltime start;

    परिणाम = populate_cpfds(&cpfds, ev);
    अगर(परिणाम)
      जाओ out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    परिणाम = poll_cpfds(ev, &cpfds, &pollrc);
    अगर(परिणाम)
      जाओ out;

    ev->msbump = FALSE; /* reset here */

    अगर(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#अगर DEBUG_EV_POLL
      curl_mfprintf(मानक_त्रुटि, "call curl_multi_socket_action(TIMEOUT)\n");
#पूर्ण_अगर
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    अन्यथा {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      अचिन्हित पूर्णांक i;
      क्रम(i = 0; i < cpfds.n; i++) {
        अगर(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          पूर्णांक act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      अगर(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        अगर(spent_ms > 0) {
#अगर DEBUG_EV_POLL
        curl_mfprintf(मानक_त्रुटि, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (दीर्घ)spent_ms);
#पूर्ण_अगर
          अगर(spent_ms > ev->ms)
            ev->ms = 0;
          अन्यथा
            ev->ms -= (दीर्घ)spent_ms;
        }
      }
    }

    अगर(mresult) {
      परिणाम = CURLE_URL_MALFORMAT;
      जाओ out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    संदेश = curl_multi_info_read(multi, &pollrc);
    अगर(संदेश) {
      परिणाम = संदेश->तथ्य.परिणाम;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  वापस परिणाम;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
अटल CURLcode easy_events(काष्ठा Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  अटल काष्ठा events evs = { -1, FALSE, 0, शून्य, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  वापस wait_or_timeout(multi, &evs);
}
#अन्यथा /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#घोषणा easy_events(x) CURLE_NOT_BUILT_IN
#पूर्ण_अगर

अटल CURLcode easy_transfer(काष्ठा Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode परिणाम = CURLE_OK;

  जबतक(!done && !mresult) {
    पूर्णांक still_running = 0;

    mresult = curl_multi_poll(multi, शून्य, 0, 1000, शून्य);

    अगर(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    अगर(!mresult && !still_running) {
      पूर्णांक rc;
      CURLMsg *संदेश = curl_multi_info_read(multi, &rc);
      अगर(संदेश) {
        परिणाम = संदेश->तथ्य.परिणाम;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  अगर(mresult) {
    परिणाम = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  वापस परिणाम;
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
अटल CURLcode easy_perform(काष्ठा Curl_easy *तथ्य, bool events)
{
  काष्ठा Curl_multi *multi;
  CURLMcode mresult;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा Curl_sigpipe_ctx sigpipe_ctx;

  अगर(!तथ्य)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(तथ्य->set.errorbuffer)
    /* clear this as early as possible */
    तथ्य->set.errorbuffer[0] = 0;

  तथ्य->state.os_errno = 0;

  अगर(तथ्य->multi) {
    failf(तथ्य, "easy handle already used in multi handle");
    वापस CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  अगर(तथ्य->conn) {
    काष्ठा connectdata *conn = तथ्य->conn;
    Curl_detach_connection(तथ्य);
    Curl_conn_close(तथ्य, conn, TRUE);
    DEBUGASSERT(!तथ्य->conn);
  }

  अगर(तथ्य->multi_easy)
    multi = तथ्य->multi_easy;
  अन्यथा {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    अगर(!multi)
      वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(Curl_api_multi_is_in_callback(multi))
    वापस CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (दीर्घ)तथ्य->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (दीर्घ)तथ्य->set.quick_exit);

  तथ्य->multi_easy = शून्य; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, तथ्य);
  अगर(mresult) {
    curl_multi_cleanup(multi);
    अगर(mresult == CURLM_OUT_OF_MEMORY)
      वापस CURLE_OUT_OF_MEMORY;
    वापस CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  तथ्य->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(तथ्य, &sigpipe_ctx);

  /* run the transfer */
  परिणाम = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (व्योम)Curl_multi_remove_handle(multi, तथ्य);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  वापस परिणाम;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  काष्ठा Curl_eapi_guard guard = { 0 };
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_perform, &परिणाम)) {
    परिणाम = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}

#अगर_घोषित DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(काष्ठा Curl_easy *easy)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &परिणाम)) {
    परिणाम = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}
#पूर्ण_अगर

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
व्योम curl_easy_cleanup(CURL *curl)
{
  काष्ठा Curl_eapi_guard guard;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, शून्य)) {
    काष्ठा Curl_easy *तथ्य = curl;
    काष्ठा Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(तथ्य, &sigpipe_ctx);
    Curl_close(&तथ्य);
    sigpipe_restore(&sigpipe_ctx);
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#अघोषित curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &परिणाम)) {
    काष्ठा Curl_easy *तथ्य = curl;
    बहु_सूची arg;
    व्योम *paramp;

    बहु_शुरू(arg, info);
    paramp = बहु_तर्क(arg, व्योम *);

    परिणाम = Curl_getinfo(तथ्य, info, paramp);

    बहु_पूर्ण(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}

अटल CURLcode dupset(काष्ठा Curl_easy *dst, काष्ठा Curl_easy *src)
{
  CURLcode परिणाम = CURLE_OK;
  क्रमागत dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#अगर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = शून्य;
#पूर्ण_अगर
  dst->set.str_copypostfields = शून्य;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  स_रखो(dst->set.blobs, 0, BLOB_LAST * माप(काष्ठा curl_blob *));

  /* duplicate all strings */
  परिणाम = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  अगर(परिणाम)
    वापस परिणाम;

  /* duplicate all blobs */
  क्रम(j = (क्रमागत dupblob)0; j < BLOB_LAST; j++) {
    परिणाम = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    अगर(परिणाम)
      वापस परिणाम;
  }

  /* duplicate memory areas pointed to */
  अगर(src->set.str_copypostfields) {
    अगर(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    अन्यथा
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    अगर(!dst->set.str_copypostfields)
      वापस CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#अगर !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  अगर(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(माप(*dst->set.mimepostp));
    अगर(!dst->set.mimepostp)
      वापस CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    परिणाम = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    अगर(परिणाम)
      वापस परिणाम;
  }
#पूर्ण_अगर

  अगर(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  वापस परिणाम;
}

अटल व्योम dupeasy_meta_freeentry(व्योम *p)
{
  (व्योम)p;
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
  काष्ठा Curl_eapi_guard guard;
  काष्ठा Curl_easy *outcurl = शून्य;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, शून्य)) {
    काष्ठा Curl_easy *तथ्य = curl;

    outcurl = curlx_calloc(1, माप(काष्ठा Curl_easy));
    अगर(!outcurl)
      जाओ fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->set.बफर_आकार = तथ्य->set.बफर_आकार;

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

#अगर_अघोषित CURL_DISABLE_HTTP
    Curl_llist_init(&outcurl->state.httphdrs, शून्य);
#पूर्ण_अगर
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    अगर(dupset(outcurl, तथ्य))
      जाओ fail;

    outcurl->progress.hide     = तथ्य->progress.hide;
    outcurl->progress.callback = तथ्य->progress.callback;

#अगर_अघोषित CURL_DISABLE_COOKIES
    outcurl->state.cookielist = शून्य;
    अगर(तथ्य->cookies && तथ्य->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      अगर(!outcurl->cookies)
        जाओ fail;
      outcurl->state.cookie_engine = TRUE;
    }

    अगर(तथ्य->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(तथ्य->state.cookielist);
      अगर(!outcurl->state.cookielist)
        जाओ fail;
    }
#पूर्ण_अगर

    अगर(Curl_bufref_ptr(&तथ्य->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&तथ्य->state.url), 0,
                      curl_free);
      अगर(!Curl_bufref_ptr(&outcurl->state.url))
        जाओ fail;
    }
    अगर(Curl_bufref_ptr(&तथ्य->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&तथ्य->state.referer), 0,
                      curl_free);
      अगर(!Curl_bufref_ptr(&outcurl->state.referer))
        जाओ fail;
    }

#अगर_अघोषित CURL_DISABLE_ALTSVC
    अगर(तथ्य->asi) {
      स्थिर अक्षर *str;
      outcurl->asi = Curl_altsvc_init();
      अगर(!outcurl->asi)
        जाओ fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      अगर(str)
        (व्योम)Curl_altsvc_load(outcurl->asi, str);
    }
#पूर्ण_अगर
#अगर_अघोषित CURL_DISABLE_HSTS
    अगर(तथ्य->hsts) {
      स्थिर अक्षर *str;
      outcurl->hsts = Curl_hsts_init();
      अगर(!outcurl->hsts)
        जाओ fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      अगर(str)
        (व्योम)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (व्योम)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      अगर(Curl_hsts_copy(outcurl->hsts, तथ्य->hsts))
        जाओ fail;
    }
#पूर्ण_अगर

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_संख्या;
  }
  CURL_EAPI_LEAVE(&guard);
  वापस outcurl;

fail:
  अगर(outcurl) {
#अगर_अघोषित CURL_DISABLE_COOKIES
    curlx_free(outcurl->cookies);
#पूर्ण_अगर
    curlx_dyn_free(&outcurl->state.headerb);
    Curl_altsvc_cleanup(&outcurl->asi);
    Curl_hsts_cleanup(&outcurl->hsts);
    Curl_freeset(outcurl);
    curlx_free(outcurl);
  }

  CURL_EAPI_LEAVE(&guard);
  वापस शून्य;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
व्योम curl_easy_reset(CURL *curl)
{
  काष्ठा Curl_eapi_guard guard;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_reset, शून्य)) {
    काष्ठा Curl_easy *तथ्य = curl;

    तथ्य->state.last_conn_id = -1; /* clear remembered connection id */
    तथ्य->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&तथ्य->req, तथ्य);
    Curl_hash_clean(&तथ्य->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(तथ्य);
    /* zero out UserDefined data: */
    Curl_freeset(तथ्य);
    स_रखो(&तथ्य->set, 0, माप(काष्ठा UserDefined));
    Curl_init_userdefined(तथ्य);

    /* zero out Progress data: */
    स_रखो(&तथ्य->progress, 0, माप(काष्ठा Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(तथ्य);

    तथ्य->progress.hide = TRUE;

    /* zero out authentication data: */
    स_रखो(&तथ्य->state.authhost, 0, माप(काष्ठा auth));
    स_रखो(&तथ्य->state.authproxy, 0, माप(काष्ठा auth));

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(तथ्य);
#पूर्ण_अगर
    तथ्य->master_mid = UINT32_MAX;
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
CURLcode curl_easy_pause(CURL *curl, पूर्णांक action)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम = CURLE_OK;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_pause, &परिणाम)) {
    bool changed = FALSE;
    काष्ठा Curl_easy *तथ्य = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    अगर(!तथ्य->conn) {
      /* crazy input, do not continue */
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }

    recv_paused = Curl_xfer_recv_is_paused(तथ्य);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(तथ्य);
    send_paused_new = (action & CURLPAUSE_SEND);

    अगर((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(तथ्य))) {
      changed = TRUE;
      परिणाम = Curl_1st_fatal(
        परिणाम, Curl_xfer_pause_send(तथ्य, send_paused_new));
    }

    अगर(recv_paused != recv_paused_new) {
      changed = TRUE;
      परिणाम = Curl_1st_fatal(
        परिणाम, Curl_xfer_pause_recv(तथ्य, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    अगर(!Curl_xfer_is_blocked(तथ्य)) {
      /* reset the too-slow time keeper */
      तथ्य->state.keeps_speed.tv_sec = 0;
      अगर(तथ्य->multi) {
        Curl_multi_mark_dirty(तथ्य); /* make it run */
        /* On changes, tell application to update its timers. */
        अगर(changed && Curl_update_timer(तथ्य->multi) && !परिणाम)
          परिणाम = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    अगर(!परिणाम && changed && !तथ्य->state.done && तथ्य->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(तथ्य->multi, तथ्य))
      परिणाम = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}

अटल CURLcode easy_connection(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata **connp)
{
  curl_socket_t sfd;

  अगर(!तथ्य)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  अगर(!तथ्य->set.connect_only) {
    failf(तथ्य, "CONNECT_ONLY is required");
    वापस CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(तथ्य, connp);

  अगर(sfd == CURL_SOCKET_BAD) {
    failf(तथ्य, "Failed to get last socket used for connection #%" FMT_OFF_T,
          तथ्य->state.last_conn_id);
    वापस CURLE_UNSUPPORTED_PROTOCOL;
  }

  वापस CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(काष्ठा Curl_easy *तथ्य,
                        व्योम *बफर, माप_प्रकार buflen, माप_प्रकार *n)
{
  CURLcode परिणाम;
  काष्ठा connectdata *c;

  परिणाम = easy_connection(तथ्य, &c);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(!तथ्य->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(तथ्य, c, TRUE);

  *n = 0;
  वापस Curl_conn_recv(तथ्य, FIRSTSOCKET, बफर, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, व्योम *बफर, माप_प्रकार buflen, माप_प्रकार *n)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_recv, &परिणाम)) {
    अगर(!n || (buflen && !बफर)) {
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }
    परिणाम = Curl_easy_recv(curl, बफर, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}

#अगर_अघोषित CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम;
  काष्ठा connectdata *c = शून्य;

  परिणाम = easy_connection(तथ्य, &c);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(!तथ्य->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(तथ्य, c, TRUE);

  वापस CURLE_OK;
}
#पूर्ण_अगर /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(काष्ठा Curl_easy *तथ्य, स्थिर व्योम *बफर,
                       माप_प्रकार buflen, माप_प्रकार *n)
{
  CURLcode परिणाम;
  काष्ठा connectdata *c = शून्य;
  काष्ठा Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  परिणाम = easy_connection(तथ्य, &c);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(!तथ्य->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(तथ्य, c, TRUE);

  sigpipe_ignore(तथ्य, &sigpipe_ctx);
  परिणाम = Curl_conn_send(तथ्य, FIRSTSOCKET, बफर, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  अगर(परिणाम && परिणाम != CURLE_AGAIN)
    वापस CURLE_SEND_ERROR;
  वापस परिणाम;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, स्थिर व्योम *बफर, माप_प्रकार buflen,
                        माप_प्रकार *n)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_send, &परिणाम)) {
    काष्ठा Curl_easy *तथ्य = curl;
    माप_प्रकार written = 0;

    अगर(!n || (buflen && !बफर)) {
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }
    परिणाम = Curl_senddata(तथ्य, बफर, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &परिणाम)) {
    /* Use the common function to keep connections alive. */
    परिणाम = Curl_cpool_upkeep((काष्ठा Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
}

CURLcode curl_easy_ssls_import(CURL *curl, स्थिर अक्षर *session_कुंजी,
                               स्थिर अचिन्हित अक्षर *shmac, माप_प्रकार shmac_len,
                               स्थिर अचिन्हित अक्षर *sdata, माप_प्रकार sdata_len)
{
#अगर defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &परिणाम)) {
    अगर(!sdata || !sdata_len)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      परिणाम = Curl_ssl_session_import((काष्ठा Curl_easy *)curl, session_कुंजी,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
#अन्यथा
  (व्योम)curl;
  (व्योम)session_कुंजी;
  (व्योम)shmac;
  (व्योम)shmac_len;
  (व्योम)sdata;
  (व्योम)sdata_len;
  वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               व्योम *userptr)
{
#अगर defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  काष्ठा Curl_eapi_guard guard;
  CURLcode परिणाम;

  अगर(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &परिणाम)) {
    अगर(!export_fn)
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    अन्यथा
      परिणाम = Curl_ssl_session_export((काष्ठा Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  वापस परिणाम;
#अन्यथा
  (व्योम)curl;
  (व्योम)export_fn;
  (व्योम)userptr;
  वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
}

CURLcode Curl_meta_set(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *कुंजी,
                       व्योम *meta_तथ्य, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_तथ्य); /* never set to NULL */
  अगर(!Curl_hash_add2(&तथ्य->meta_hash, कुंजी, म_माप(कुंजी) + 1,
                     meta_तथ्य, meta_dtor)) {
    meta_dtor(कुंजी, म_माप(कुंजी) + 1, meta_तथ्य);
    वापस CURLE_OUT_OF_MEMORY;
  }
  वापस CURLE_OK;
}

व्योम Curl_meta_remove(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *कुंजी)
{
  Curl_hash_delete(&तथ्य->meta_hash, कुंजी, म_माप(कुंजी) + 1);
}

व्योम *Curl_meta_get(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *कुंजी)
{
  वापस Curl_hash_pick(&तथ्य->meta_hash, कुंजी, म_माप(कुंजी) + 1);
}

व्योम Curl_meta_reset(काष्ठा Curl_easy *तथ्य)
{
  Curl_hash_clean(&तथ्य->meta_hash);
}
