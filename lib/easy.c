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

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NET_IF_H
#ضمن <net/if.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جيڪڏهن_پورو

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
ساكن بنانشان صحيح initialized;
#جيڪڏهن_وصف_ٿيل USE_WINSOCK
ساكن bool winsock_initialized;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل GLOBAL_INIT_IS_THREADSAFE

ساكن curl_simple_تالو s_تالو = CURL_SIMPLE_تالو_INIT;
#وصف_ڪريو global_init_تالو()   curl_simple_تالو_تالو(&s_تالو)
#وصف_ڪريو global_init_unlock() curl_simple_تالو_unlock(&s_تالو)

#وإلا

#وصف_ڪريو global_init_تالو()
#وصف_ڪريو global_init_unlock()

#جيڪڏهن_پورو

#إذا defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ڌڪو)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#جيڪڏهن_پورو

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)يادگيري_ڏيو;
curl_free_callback Curl_cfree = (curl_free_callback)حرر;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_هيٺيون;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)صفر_يادگيري_ڏيو;

#إذا defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ڪڍو)
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
ساكن حرف *leakpointer;
#جيڪڏهن_پورو

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
ساكن CURLcode global_init(ڊگهو flags, bool memoryfuncs)
{
  (فراغ)flags;

  إذا(initialized++)
    أرجع CURLE_OK;

  إذا(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)يادگيري_ڏيو;
    Curl_cfree = (curl_free_callback)حرر;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_هيٺيون;
    Curl_ccalloc = (curl_calloc_callback)صفر_يادگيري_ڏيو;
  }

#جيڪڏهن_وصف_ٿيل _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#جيڪڏهن_پورو

  إذا(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    اذهب fail;
  }

#جيڪڏهن_وصف_ٿيل USE_WINDOWS_SSPI
  إذا(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    اذهب fail;
  }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  إذا(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    إذا(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      اذهب fail;
    }
    winsock_initialized = TRUE;
  }
#جيڪڏهن_پورو

  إذا(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    اذهب fail;
  }

  إذا(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    اذهب fail;
  }

  إذا(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    اذهب fail;
  }

  إذا(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    اذهب fail;
  }

  إذا(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    اذهب fail;
  }

  إذا(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    اذهب fail;
  }

  إذا(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    اذهب fail;
  }

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  إذا(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#جيڪڏهن_پورو

  أرجع CURLE_OK;

fail:
#جيڪڏهن_وصف_ٿيل USE_WINSOCK
  إذا(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#جيڪڏهن_پورو
  initialized--; /* undo the increase */
  أرجع CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(ڊگهو flags)
{
  CURLcode نتيجو;
  global_init_تالو();

  نتيجو = global_init(flags, TRUE);

  global_init_unlock();

  أرجع نتيجو;
}

/*
 * curl_global_init_mem() globally initializes curl and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(ڊگهو flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode نتيجو;

  /* Invalid input, return immediately */
  إذا(!m || !f || !r || !s || !c)
    أرجع CURLE_FAILED_INIT;

  global_init_تالو();

  إذا(initialized) {
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
  نتيجو = global_init(flags, FALSE);

  global_init_unlock();

  أرجع نتيجو;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
فراغ curl_global_cleanup(فراغ)
{
  global_init_تالو();

  إذا(!initialized) {
    global_init_unlock();
    أرجع;
  }

  إذا(--initialized) {
    global_init_unlock();
    أرجع;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#جيڪڏهن_وصف_ٿيل USE_WINSOCK
  إذا(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_WINDOWS_SSPI
  Curl_sspi_global_cleanup();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  curlx_free(leakpointer);
#جيڪڏهن_پورو

  global_init_unlock();
}

/**
 * curl_global_trace() globally initializes curl logging.
 */
CURLcode curl_global_trace(ثابت حرف *config)
{
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_VERBOSE_لڙيون
  CURLcode نتيجو;
  global_init_تالو();

  نتيجو = Curl_trc_opt(config);

  global_init_unlock();

  أرجع نتيجو;
#وإلا
  (فراغ)config;
  أرجع CURLE_OK;
#جيڪڏهن_پورو
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ثابت حرف *name,
                              ثابت curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_تالو();

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
  CURLcode نتيجو;
  بنية Curl_easy *معلومات;

  /* Make sure we inited the global SSL stuff */
  global_init_تالو();

  إذا(!initialized) {
    نتيجو = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    إذا(نتيجو) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      أرجع عدم;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  نتيجو = Curl_open(&معلومات);
  إذا(نتيجو) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    أرجع عدم;
  }

  أرجع معلومات;
}

#جيڪڏهن_وصف_ٿيل DEBUGBUILD

بنية socketmonitor {
  بنية socketmonitor *ايندڙ; /* the next node in the list or NULL */
  بنية pollfd socket; /* socket info of what to monitor */
};

بنية events {
  ڊگهو ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  صحيح انگ_sockets;      /* number of nodes in the monitor list */
  بنية socketmonitor *list; /* list of sockets to monitor */
  صحيح running_handles;  /* store the returned number */
};

#وصف_ڪريو DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
ساكن صحيح events_timer(CURLM *multi,    /* multi handle */
                        ڊگهو timeout_ms, /* see above */
                        فراغ *userp)     /* private callback pointer */
{
  بنية events *ev = userp;
  (فراغ)multi;
#إذا DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#جيڪڏهن_پورو
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
  إذا(pollmask & POLLIN)
    omask |= CURL_CSELECT_اندريون;
  إذا(pollmask & POLLOUT)
    omask |= CURL_CSELECT_ٻاهريون;
  إذا(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  أرجع omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
ساكن ننڍو socketcb2poll(صحيح pollmask)
{
  ننڍو omask = 0;
  إذا(pollmask & CURL_POLL_اندريون)
    omask |= POLLIN;
  إذا(pollmask & CURL_POLL_ٻاهريون)
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
اشارو*/
                         فراغ *socketp)   /* private socket
اشارو*/
{
  بنية events *ev = userp;
  بنية socketmonitor *m;
  بنية socketmonitor *prev = عدم;
  bool مليو = FALSE;
  بنية Curl_easy *معلومات = easy;

#جيڪڏهن_وصف_ٿيل CURL_DISABLE_VERBOSE_لڙيون
  (فراغ)easy;
#جيڪڏهن_پورو
  (فراغ)socketp;

  m = ev->list;
  طالما(m) {
    إذا(m->socket.fd == s) {
      مليو = TRUE;
      إذا(what == CURL_POLL_REMOVE) {
        بنية socketmonitor *nxt = m->ايندڙ;
        /* remove this node from the list of monitored sockets */
        إذا(prev)
          prev->ايندڙ = nxt;
        وإلا
          ev->list = nxt;
        curlx_free(m);
        infof(معلومات, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      وإلا {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(معلومات, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_اندريون) ? "IN" : "",
              (what & CURL_POLL_ٻاهريون) ? "OUT" : "");
      }
      اكسر;
    }
    prev = m;
    m = m->ايندڙ; /* move to next node */
  }

  إذا(!مليو) {
    إذا(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(معلومات, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    وإلا {
      m = curlx_malloc(حجم(بنية socketmonitor));
      إذا(m) {
        m->ايندڙ = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(معلومات, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_اندريون) ? "IN" : "",
              (what & CURL_POLL_ٻاهريون) ? "OUT" : "");
      }
      وإلا
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }

  أرجع 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
ساكن فراغ events_تياري(بنية Curl_multi *multi, بنية events *ev)
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
  CURLcode نتيجو = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  لكل(m = ev->list; m && !نتيجو; m = m->ايندڙ) {
    نتيجو = Curl_pollfds_جوڙيو_sock(cpfds, m->socket.fd, m->socket.events);
#إذا DEBUG_EV_POLL
    إذا(!نتيجو)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#جيڪڏهن_پورو
  }
  أرجع نتيجو;
}

ساكن CURLcode poll_cpfds(بنية events *ev,
                           بنية curl_pollfds *cpfds,
                           صحيح *pollrc)
{
  إذا(cpfds->n) {
    /* wait for activity or timeout */
#إذا DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#جيڪڏهن_پورو
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#إذا DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#جيڪڏهن_پورو
    إذا(*pollrc < 0)
      أرجع CURLE_UNRECOVERABLE_POLL;
  }
  وإلا {
#إذا DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#جيڪڏهن_پورو
    *pollrc = 0;
    إذا(ev->ms > 0)
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
  بنية pollfd fds_on_ڍير[8];
  بنية curl_pollfds cpfds;
  bool پورو = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتيجو = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_ڍير, CURL_ARRAYSIZE(fds_on_ڍير));
  طالما(!پورو) {
    CURLMsg *نياپو;
    صحيح pollrc;
    بنية curltime شروعات;

    نتيجو = populate_cpfds(&cpfds, ev);
    إذا(نتيجو)
      اذهب ٻاهريون;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&شروعات);

    نتيجو = poll_cpfds(ev, &cpfds, &pollrc);
    إذا(نتيجو)
      اذهب ٻاهريون;

    ev->msbump = FALSE; /* reset here */

    إذا(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#إذا DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#جيڪڏهن_پورو
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    وإلا {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      بنانشان صحيح i;
      لكل(i = 0; i < cpfds.n; i++) {
        إذا(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          صحيح act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      إذا(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), شروعات);
        إذا(spent_ms > 0) {
#إذا DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (ڊگهو)spent_ms);
#جيڪڏهن_پورو
          إذا(spent_ms > ev->ms)
            ev->ms = 0;
          وإلا
            ev->ms -= (ڊگهو)spent_ms;
        }
      }
    }

    إذا(mresult) {
      نتيجو = CURLE_URL_MALFORMAT;
      اذهب ٻاهريون;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    نياپو = curl_multi_info_read(multi, &pollrc);
    إذا(نياپو) {
      نتيجو = نياپو->معلومات.نتيجو;
      پورو = TRUE;
    }
  }
ٻاهريون:
  Curl_pollfds_cleanup(&cpfds);
  أرجع نتيجو;
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
  events_تياري(multi, &evs);

  أرجع wait_or_timeout(multi, &evs);
}
#وإلا /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#وصف_ڪريو easy_events(x) CURLE_NOT_BUILT_اندريون
#جيڪڏهن_پورو

ساكن CURLcode easy_transfer(بنية Curl_multi *multi)
{
  bool پورو = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتيجو = CURLE_OK;

  طالما(!پورو && !mresult) {
    صحيح still_running = 0;

    mresult = curl_multi_poll(multi, عدم, 0, 1000, عدم);

    إذا(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    إذا(!mresult && !still_running) {
      صحيح rc;
      CURLMsg *نياپو = curl_multi_info_read(multi, &rc);
      إذا(نياپو) {
        نتيجو = نياپو->معلومات.نتيجو;
        پورو = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  إذا(mresult) {
    نتيجو = (mresult == CURLM_ٻاهريون_جو_يادگيري) ? CURLE_ٻاهريون_جو_يادگيري :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_دليل;
  }

  أرجع نتيجو;
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
ساكن CURLcode easy_perform(بنية Curl_easy *معلومات, bool events)
{
  بنية Curl_multi *multi;
  CURLMcode mresult;
  CURLcode نتيجو = CURLE_OK;
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  إذا(!معلومات)
    أرجع CURLE_BAD_FUNCTION_دليل;

  إذا(معلومات->سيٽ.errorbuffer)
    /* clear this as early as possible */
    معلومات->سيٽ.errorbuffer[0] = 0;

  معلومات->state.os_errno = 0;

  إذا(معلومات->multi) {
    failf(معلومات, "easy handle already used in multi handle");
    أرجع CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  إذا(معلومات->conn) {
    بنية connectdata *conn = معلومات->conn;
    Curl_detach_connection(معلومات);
    Curl_conn_close(معلومات, conn, TRUE);
    DEBUGASSERT(!معلومات->conn);
  }

  إذا(معلومات->multi_easy)
    multi = معلومات->multi_easy;
  وإلا {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    إذا(!multi)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  إذا(Curl_api_multi_is_اندريون_callback(multi))
    أرجع CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (ڊگهو)معلومات->سيٽ.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_تيز_EXIT, (ڊگهو)معلومات->سيٽ.quick_exit);

  معلومات->multi_easy = عدم; /* pretend it does not exist */
  mresult = Curl_multi_جوڙيو_handle(multi, معلومات);
  إذا(mresult) {
    curl_multi_cleanup(multi);
    إذا(mresult == CURLM_ٻاهريون_جو_يادگيري)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    أرجع CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  معلومات->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(معلومات, &sigpipe_ctx);

  /* run the transfer */
  نتيجو = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (فراغ)Curl_multi_remove_handle(multi, معلومات);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  أرجع نتيجو;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  بنية Curl_eapi_guard guard = { 0 };
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_perform, &نتيجو)) {
    نتيجو = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(بنية Curl_easy *easy)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &نتيجو)) {
    نتيجو = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}
#جيڪڏهن_پورو

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
فراغ curl_easy_cleanup(CURL *curl)
{
  بنية Curl_eapi_guard guard;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, عدم)) {
    بنية Curl_easy *معلومات = curl;
    بنية Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(معلومات, &sigpipe_ctx);
    Curl_close(&معلومات);
    sigpipe_restore(&sigpipe_ctx);
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#وصف_رد curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &نتيجو)) {
    بنية Curl_easy *معلومات = curl;
    va_list arg;
    فراغ *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, فراغ *);

    نتيجو = Curl_getinfo(معلومات, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}

ساكن CURLcode dupset(بنية Curl_easy *dst, بنية Curl_easy *src)
{
  CURLcode نتيجو = CURLE_OK;
  تعداد dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->سيٽ = src->سيٽ;
#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->سيٽ.mimepostp = عدم;
#جيڪڏهن_پورو
  dst->سيٽ.str_copypostfields = عدم;

  Curl_u8_strset_init(&dst->سيٽ.لڙيون);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->سيٽ.blobs, 0, BLOB_آخري * حجم(بنية curl_blob *));

  /* duplicate all strings */
  نتيجو = Curl_u8_strset_copy(&dst->سيٽ.لڙيون, &src->سيٽ.لڙيون);
  إذا(نتيجو)
    أرجع نتيجو;

  /* duplicate all blobs */
  لكل(j = (تعداد dupblob)0; j < BLOB_آخري; j++) {
    نتيجو = Curl_setblobopt(&dst->سيٽ.blobs[j], src->سيٽ.blobs[j]);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  /* duplicate memory areas pointed to */
  إذا(src->سيٽ.str_copypostfields) {
    إذا(src->سيٽ.postfieldsize == -1)
      dst->سيٽ.str_copypostfields = curlx_strdup(src->سيٽ.str_copypostfields);
    وإلا
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->سيٽ.str_copypostfields =
        curlx_memdup0(src->سيٽ.str_copypostfields,
                      curlx_sotouz(src->سيٽ.postfieldsize));
    إذا(!dst->سيٽ.str_copypostfields)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    /* point to the new copy */
    dst->سيٽ.postfields = dst->سيٽ.str_copypostfields;
  }

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  إذا(src->سيٽ.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->سيٽ.mimepostp = curlx_malloc(حجم(*dst->سيٽ.mimepostp));
    إذا(!dst->سيٽ.mimepostp)
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    Curl_mime_initpart(dst->سيٽ.mimepostp);
    نتيجو = Curl_mime_duppart(dst, dst->سيٽ.mimepostp, src->سيٽ.mimepostp);
    إذا(نتيجو)
      أرجع نتيجو;
  }
#جيڪڏهن_پورو

  إذا(src->سيٽ.resolve)
    dst->state.resolve = dst->سيٽ.resolve;

  أرجع نتيجو;
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

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, عدم)) {
    بنية Curl_easy *معلومات = curl;

    outcurl = curlx_calloc(1, حجم(بنية Curl_easy));
    إذا(!outcurl)
      اذهب fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->سيٽ.بفر_سائيز = معلومات->سيٽ.بفر_سائيز;

    Curl_hash_init(&outcurl->meta_hash, 23, CURL_HASH_TYPE_بائيٽون,
                   dupeasy_meta_freeentry);
    curlx_dyn_init(&outcurl->state.headerb, CURL_MAX_HTTP_سرو);
    Curl_bufref_init(&outcurl->state.url);
    Curl_bufref_init(&outcurl->state.referer);
    Curl_netrc_init(&outcurl->state.netrc);

    /* the connection pool is setup on demand */
    outcurl->state.آخري_conn_id = -1;
    outcurl->state.آخري_cpid = UINT32_MAX;
    outcurl->id = -1;
    outcurl->mid = UINT32_MAX;
    outcurl->master_mid = UINT32_MAX;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
    Curl_llist_init(&outcurl->state.httphdrs, عدم);
#جيڪڏهن_پورو
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    إذا(dupset(outcurl, معلومات))
      اذهب fail;

    outcurl->ترقي.hide     = معلومات->ترقي.hide;
    outcurl->ترقي.callback = معلومات->ترقي.callback;

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
    outcurl->state.cookielist = عدم;
    إذا(معلومات->cookies && معلومات->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      إذا(!outcurl->cookies)
        اذهب fail;
      outcurl->state.cookie_engine = TRUE;
    }

    إذا(معلومات->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(معلومات->state.cookielist);
      إذا(!outcurl->state.cookielist)
        اذهب fail;
    }
#جيڪڏهن_پورو

    إذا(Curl_bufref_ptr(&معلومات->state.url)) {
      Curl_bufref_سيٽ(&outcurl->state.url,
                      Curl_bufref_dup(&معلومات->state.url), 0,
                      curl_free);
      إذا(!Curl_bufref_ptr(&outcurl->state.url))
        اذهب fail;
    }
    إذا(Curl_bufref_ptr(&معلومات->state.referer)) {
      Curl_bufref_سيٽ(&outcurl->state.referer,
                      Curl_bufref_dup(&معلومات->state.referer), 0,
                      curl_free);
      إذا(!Curl_bufref_ptr(&outcurl->state.referer))
        اذهب fail;
    }

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_ALTSVC
    إذا(معلومات->asi) {
      ثابت حرف *str;
      outcurl->asi = Curl_altsvc_init();
      إذا(!outcurl->asi)
        اذهب fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      إذا(str)
        (فراغ)Curl_altsvc_ڀريو(outcurl->asi, str);
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HSTS
    إذا(معلومات->hsts) {
      ثابت حرف *str;
      outcurl->hsts = Curl_hsts_init();
      إذا(!outcurl->hsts)
        اذهب fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      إذا(str)
        (فراغ)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (فراغ)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      إذا(Curl_hsts_copy(outcurl->hsts, معلومات->hsts))
        اذهب fail;
    }
#جيڪڏهن_پورو

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_عدد;
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع outcurl;

fail:
  إذا(outcurl) {
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_COOKIES
    curlx_free(outcurl->cookies);
#جيڪڏهن_پورو
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

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_reset, عدم)) {
    بنية Curl_easy *معلومات = curl;

    معلومات->state.آخري_conn_id = -1; /* clear remembered connection id */
    معلومات->state.آخري_cpid = UINT32_MAX;
    Curl_req_hard_reset(&معلومات->req, معلومات);
    Curl_hash_clean(&معلومات->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(معلومات);
    /* zero out UserDefined data: */
    Curl_freeset(معلومات);
    memset(&معلومات->سيٽ, 0, حجم(بنية UserDefined));
    Curl_init_userdefined(معلومات);

    /* zero out Progress data: */
    memset(&معلومات->ترقي, 0, حجم(بنية ترقي_2));

    /* zero out PureInfo data: */
    Curl_initinfo(معلومات);

    معلومات->ترقي.hide = TRUE;

    /* zero out authentication data: */
    memset(&معلومات->state.authhost, 0, حجم(بنية auth));
    memset(&معلومات->state.authproxy, 0, حجم(بنية auth));

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(معلومات);
#جيڪڏهن_پورو
    معلومات->master_mid = UINT32_MAX;
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
  CURLcode نتيجو = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_pause, &نتيجو)) {
    bool بدليو = FALSE;
    بنية Curl_easy *معلومات = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    إذا(!معلومات->conn) {
      /* crazy input, do not continue */
      نتيجو = CURLE_BAD_FUNCTION_دليل;
      اذهب ٻاهريون;
    }

    recv_paused = Curl_xfer_recv_is_paused(معلومات);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(معلومات);
    send_paused_new = (action & CURLPAUSE_SEND);

    إذا((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(معلومات))) {
      بدليو = TRUE;
      نتيجو = Curl_1st_fatal(
        نتيجو, Curl_xfer_pause_send(معلومات, send_paused_new));
    }

    إذا(recv_paused != recv_paused_new) {
      بدليو = TRUE;
      نتيجو = Curl_1st_fatal(
        نتيجو, Curl_xfer_pause_recv(معلومات, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    إذا(!Curl_xfer_is_blocked(معلومات)) {
      /* reset the too-slow time keeper */
      معلومات->state.keeps_رفتار.tv_sec = 0;
      إذا(معلومات->multi) {
        Curl_multi_mark_dirty(معلومات); /* make it run */
        /* On changes, tell application to update its timers. */
        إذا(بدليو && Curl_update_timer(معلومات->multi) && !نتيجو)
          نتيجو = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    إذا(!نتيجو && بدليو && !معلومات->state.پورو && معلومات->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(معلومات->multi, معلومات))
      نتيجو = CURLE_ABORTED_BY_CALLBACK;
  }
ٻاهريون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}

ساكن CURLcode easy_connection(بنية Curl_easy *معلومات,
                                بنية connectdata **connp)
{
  curl_socket_t sfd;

  إذا(!معلومات)
    أرجع CURLE_BAD_FUNCTION_دليل;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  إذا(!معلومات->سيٽ.connect_only) {
    failf(معلومات, "CONNECT_ONLY is required");
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(معلومات, connp);

  إذا(sfd == CURL_SOCKET_BAD) {
    failf(معلومات, "Failed to get last socket used for connection #%" FMT_OFF_T,
          معلومات->state.آخري_conn_id);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  أرجع CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(بنية Curl_easy *معلومات,
                        فراغ *بفر, size_t buflen, size_t *n)
{
  CURLcode نتيجو;
  بنية connectdata *c;

  نتيجو = easy_connection(معلومات, &c);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(!معلومات->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(معلومات, c, TRUE);

  *n = 0;
  أرجع Curl_conn_recv(معلومات, FIRSTSOCKET, بفر, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, فراغ *بفر, size_t buflen, size_t *n)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_recv, &نتيجو)) {
    إذا(!n || (buflen && !بفر)) {
      نتيجو = CURLE_BAD_FUNCTION_دليل;
      اذهب ٻاهريون;
    }
    نتيجو = Curl_easy_recv(curl, بفر, buflen, n);
  }
ٻاهريون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(بنية Curl_easy *معلومات)
{
  CURLcode نتيجو;
  بنية connectdata *c = عدم;

  نتيجو = easy_connection(معلومات, &c);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(!معلومات->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(معلومات, c, TRUE);

  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(بنية Curl_easy *معلومات, ثابت فراغ *بفر,
                       size_t buflen, size_t *n)
{
  CURLcode نتيجو;
  بنية connectdata *c = عدم;
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  نتيجو = easy_connection(معلومات, &c);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(!معلومات->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(معلومات, c, TRUE);

  sigpipe_ignore(معلومات, &sigpipe_ctx);
  نتيجو = Curl_conn_send(معلومات, FIRSTSOCKET, بفر, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  إذا(نتيجو && نتيجو != CURLE_AGAIN)
    أرجع CURLE_SEND_ERROR;
  أرجع نتيجو;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ثابت فراغ *بفر, size_t buflen,
                        size_t *n)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_send, &نتيجو)) {
    بنية Curl_easy *معلومات = curl;
    size_t written = 0;

    إذا(!n || (buflen && !بفر)) {
      نتيجو = CURLE_BAD_FUNCTION_دليل;
      اذهب ٻاهريون;
    }
    نتيجو = Curl_senddata(معلومات, بفر, buflen, &written);
    *n = written;
  }
ٻاهريون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &نتيجو)) {
    /* Use the common function to keep connections alive. */
    نتيجو = Curl_cpool_upkeep((بنية Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
}

CURLcode curl_easy_ssls_import(CURL *curl, ثابت حرف *session_ڪنجي,
                               ثابت بنانشان حرف *shmac, size_t shmac_len,
                               ثابت بنانشان حرف *sdata, size_t sdata_len)
{
#إذا defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &نتيجو)) {
    إذا(!sdata || !sdata_len)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      نتيجو = Curl_ssl_session_import((بنية Curl_easy *)curl, session_ڪنجي,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
#وإلا
  (فراغ)curl;
  (فراغ)session_ڪنجي;
  (فراغ)shmac;
  (فراغ)shmac_len;
  (فراغ)sdata;
  (فراغ)sdata_len;
  أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               فراغ *userptr)
{
#إذا defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  بنية Curl_eapi_guard guard;
  CURLcode نتيجو;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &نتيجو)) {
    إذا(!export_fn)
      نتيجو = CURLE_BAD_FUNCTION_دليل;
    وإلا
      نتيجو = Curl_ssl_session_export((بنية Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتيجو;
#وإلا
  (فراغ)curl;
  (فراغ)export_fn;
  (فراغ)userptr;
  أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
}

CURLcode Curl_meta_سيٽ(بنية Curl_easy *معلومات, ثابت حرف *ڪنجي,
                       فراغ *meta_معلومات, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_معلومات); /* never set to NULL */
  إذا(!Curl_hash_add2(&معلومات->meta_hash, ڪنجي, لڙي_ڊيگهه(ڪنجي) + 1,
                     meta_معلومات, meta_dtor)) {
    meta_dtor(ڪنجي, لڙي_ڊيگهه(ڪنجي) + 1, meta_معلومات);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  أرجع CURLE_OK;
}

فراغ Curl_meta_remove(بنية Curl_easy *معلومات, ثابت حرف *ڪنجي)
{
  Curl_hash_delete(&معلومات->meta_hash, ڪنجي, لڙي_ڊيگهه(ڪنجي) + 1);
}

فراغ *Curl_meta_get(بنية Curl_easy *معلومات, ثابت حرف *ڪنجي)
{
  أرجع Curl_hash_pick(&معلومات->meta_hash, ڪنجي, لڙي_ڊيگهه(ڪنجي) + 1);
}

فراغ Curl_meta_reset(بنية Curl_easy *معلومات)
{
  Curl_hash_clean(&معلومات->meta_hash);
}
