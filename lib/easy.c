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

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#پایان_اگر

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
ساكن بیعلامت صحيح initialized;
#اگر_تعریف_شده USE_WINSOCK
ساكن bool winsock_initialized;
#پایان_اگر

#اگر_تعریف_شده GLOBAL_INIT_IS_THREADSAFE

ساكن curl_simple_قفل s_قفل = CURL_SIMPLE_قفل_INIT;
#تعریف_کن global_init_قفل()   curl_simple_قفل_قفل(&s_قفل)
#تعریف_کن global_init_unlock() curl_simple_قفل_unlock(&s_قفل)

#وإلا

#تعریف_کن global_init_قفل()
#تعریف_کن global_init_unlock()

#پایان_اگر

#إذا defined(_MSC_VER) && defined(_DLL)
#  pragma warning(افزودن)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#پایان_اگر

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)حافظه_بده;
curl_free_callback Curl_cfree = (curl_free_callback)حرر;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_کم;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)حافظه_صفر_بده;

#إذا defined(_MSC_VER) && defined(_DLL)
#  pragma warning(برداشت)
#پایان_اگر

#اگر_تعریف_شده DEBUGBUILD
ساكن حرف *leakpointer;
#پایان_اگر

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
ساكن CURLcode global_init(دراز flags, bool memoryfuncs)
{
  (فراغ)flags;

  إذا(initialized++)
    أرجع CURLE_OK;

  إذا(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)حافظه_بده;
    Curl_cfree = (curl_free_callback)حرر;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_کم;
    Curl_ccalloc = (curl_calloc_callback)حافظه_صفر_بده;
  }

#اگر_تعریف_شده _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#پایان_اگر

  إذا(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    اذهب fail;
  }

#اگر_تعریف_شده USE_WINDOWS_SSPI
  إذا(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    اذهب fail;
  }
#پایان_اگر

#اگر_تعریف_شده USE_WINSOCK
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
#پایان_اگر

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

#اگر_تعریف_شده DEBUGBUILD
  إذا(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#پایان_اگر

  أرجع CURLE_OK;

fail:
#اگر_تعریف_شده USE_WINSOCK
  إذا(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#پایان_اگر
  initialized--; /* undo the increase */
  أرجع CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(دراز flags)
{
  CURLcode نتیجه;
  global_init_قفل();

  نتیجه = global_init(flags, TRUE);

  global_init_unlock();

  أرجع نتیجه;
}

/*
 * curl_global_init_mem() globally initializes curl and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(دراز flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode نتیجه;

  /* Invalid input, return immediately */
  إذا(!m || !f || !r || !s || !c)
    أرجع CURLE_FAILED_INIT;

  global_init_قفل();

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
  نتیجه = global_init(flags, FALSE);

  global_init_unlock();

  أرجع نتیجه;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
فراغ curl_global_cleanup(فراغ)
{
  global_init_قفل();

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

#اگر_تعریف_شده USE_WINSOCK
  إذا(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#پایان_اگر

#اگر_تعریف_شده USE_WINDOWS_SSPI
  Curl_sspi_global_cleanup();
#پایان_اگر

#اگر_تعریف_شده DEBUGBUILD
  curlx_free(leakpointer);
#پایان_اگر

  global_init_unlock();
}

/**
 * curl_global_trace() globally initializes curl logging.
 */
CURLcode curl_global_trace(ثابت حرف *config)
{
#اگر_تعریف_نشده CURL_DISABLE_VERBOSE_رشتهها
  CURLcode نتیجه;
  global_init_قفل();

  نتیجه = Curl_trc_opt(config);

  global_init_unlock();

  أرجع نتیجه;
#وإلا
  (فراغ)config;
  أرجع CURLE_OK;
#پایان_اگر
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ثابت حرف *name,
                              ثابت curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_قفل();

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
  CURLcode نتیجه;
  بنية Curl_easy *داده;

  /* Make sure we inited the global SSL stuff */
  global_init_قفل();

  إذا(!initialized) {
    نتیجه = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    إذا(نتیجه) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      أرجع عدم;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  نتیجه = Curl_open(&داده);
  إذا(نتیجه) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    أرجع عدم;
  }

  أرجع داده;
}

#اگر_تعریف_شده DEBUGBUILD

بنية socketmonitor {
  بنية socketmonitor *بعدی; /* the next node in the list or NULL */
  بنية pollfd socket; /* socket info of what to monitor */
};

بنية events {
  دراز ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  صحيح رقم_sockets;      /* number of nodes in the monitor list */
  بنية socketmonitor *list; /* list of sockets to monitor */
  صحيح running_handles;  /* store the returned number */
};

#تعریف_کن DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
ساكن صحيح events_timer(CURLM *multi,    /* multi handle */
                        دراز timeout_ms, /* see above */
                        فراغ *userp)     /* private callback pointer */
{
  بنية events *ev = userp;
  (فراغ)multi;
#إذا DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#پایان_اگر
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
    omask |= CURL_CSELECT_درون;
  إذا(pollmask & POLLOUT)
    omask |= CURL_CSELECT_بیرون;
  إذا(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  أرجع omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
ساكن کوتاه socketcb2poll(صحيح pollmask)
{
  کوتاه omask = 0;
  إذا(pollmask & CURL_POLL_درون)
    omask |= POLLIN;
  إذا(pollmask & CURL_POLL_بیرون)
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
اشاره_گر*/
                         فراغ *socketp)   /* private socket
اشاره_گر*/
{
  بنية events *ev = userp;
  بنية socketmonitor *m;
  بنية socketmonitor *prev = عدم;
  bool یافت = FALSE;
  بنية Curl_easy *داده = easy;

#اگر_تعریف_شده CURL_DISABLE_VERBOSE_رشتهها
  (فراغ)easy;
#پایان_اگر
  (فراغ)socketp;

  m = ev->list;
  طالما(m) {
    إذا(m->socket.fd == s) {
      یافت = TRUE;
      إذا(what == CURL_POLL_REMOVE) {
        بنية socketmonitor *nxt = m->بعدی;
        /* remove this node from the list of monitored sockets */
        إذا(prev)
          prev->بعدی = nxt;
        وإلا
          ev->list = nxt;
        curlx_free(m);
        infof(داده, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      وإلا {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(داده, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_درون) ? "IN" : "",
              (what & CURL_POLL_بیرون) ? "OUT" : "");
      }
      اكسر;
    }
    prev = m;
    m = m->بعدی; /* move to next node */
  }

  إذا(!یافت) {
    إذا(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(داده, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    وإلا {
      m = curlx_malloc(حجم(بنية socketmonitor));
      إذا(m) {
        m->بعدی = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(داده, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_درون) ? "IN" : "",
              (what & CURL_POLL_بیرون) ? "OUT" : "");
      }
      وإلا
        أرجع CURLE_بیرون_از_حافظه;
    }
  }

  أرجع 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
ساكن فراغ events_راهاندازی(بنية Curl_multi *multi, بنية events *ev)
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
  CURLcode نتیجه = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  لكل(m = ev->list; m && !نتیجه; m = m->بعدی) {
    نتیجه = Curl_pollfds_جمع_کن_sock(cpfds, m->socket.fd, m->socket.events);
#إذا DEBUG_EV_POLL
    إذا(!نتیجه)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#پایان_اگر
  }
  أرجع نتیجه;
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
#پایان_اگر
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#إذا DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#پایان_اگر
    إذا(*pollrc < 0)
      أرجع CURLE_UNRECOVERABLE_POLL;
  }
  وإلا {
#إذا DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#پایان_اگر
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
  بنية pollfd fds_on_پشته[8];
  بنية curl_pollfds cpfds;
  bool انجامشده = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجه = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_پشته, CURL_ARRAYSIZE(fds_on_پشته));
  طالما(!انجامشده) {
    CURLMsg *پیام;
    صحيح pollrc;
    بنية curltime شروع;

    نتیجه = populate_cpfds(&cpfds, ev);
    إذا(نتیجه)
      اذهب بیرون;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&شروع);

    نتیجه = poll_cpfds(ev, &cpfds, &pollrc);
    إذا(نتیجه)
      اذهب بیرون;

    ev->msbump = FALSE; /* reset here */

    إذا(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#إذا DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#پایان_اگر
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    وإلا {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      بیعلامت صحيح i;
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
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), شروع);
        إذا(spent_ms > 0) {
#إذا DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (دراز)spent_ms);
#پایان_اگر
          إذا(spent_ms > ev->ms)
            ev->ms = 0;
          وإلا
            ev->ms -= (دراز)spent_ms;
        }
      }
    }

    إذا(mresult) {
      نتیجه = CURLE_URL_MALFORMAT;
      اذهب بیرون;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    پیام = curl_multi_info_read(multi, &pollrc);
    إذا(پیام) {
      نتیجه = پیام->داده.نتیجه;
      انجامشده = TRUE;
    }
  }
بیرون:
  Curl_pollfds_cleanup(&cpfds);
  أرجع نتیجه;
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
  events_راهاندازی(multi, &evs);

  أرجع wait_or_timeout(multi, &evs);
}
#وإلا /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#تعریف_کن easy_events(x) CURLE_NOT_BUILT_درون
#پایان_اگر

ساكن CURLcode easy_transfer(بنية Curl_multi *multi)
{
  bool انجامشده = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجه = CURLE_OK;

  طالما(!انجامشده && !mresult) {
    صحيح still_running = 0;

    mresult = curl_multi_poll(multi, عدم, 0, 1000, عدم);

    إذا(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    إذا(!mresult && !still_running) {
      صحيح rc;
      CURLMsg *پیام = curl_multi_info_read(multi, &rc);
      إذا(پیام) {
        نتیجه = پیام->داده.نتیجه;
        انجامشده = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  إذا(mresult) {
    نتیجه = (mresult == CURLM_بیرون_از_حافظه) ? CURLE_بیرون_از_حافظه :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_آرگومان;
  }

  أرجع نتیجه;
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
ساكن CURLcode easy_perform(بنية Curl_easy *داده, bool events)
{
  بنية Curl_multi *multi;
  CURLMcode mresult;
  CURLcode نتیجه = CURLE_OK;
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  إذا(!داده)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  إذا(داده->مجموعه.errorbuffer)
    /* clear this as early as possible */
    داده->مجموعه.errorbuffer[0] = 0;

  داده->state.os_errno = 0;

  إذا(داده->multi) {
    failf(داده, "easy handle already used in multi handle");
    أرجع CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  إذا(داده->conn) {
    بنية connectdata *conn = داده->conn;
    Curl_detach_connection(داده);
    Curl_conn_close(داده, conn, TRUE);
    DEBUGASSERT(!داده->conn);
  }

  إذا(داده->multi_easy)
    multi = داده->multi_easy;
  وإلا {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    إذا(!multi)
      أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(Curl_api_multi_is_درون_callback(multi))
    أرجع CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (دراز)داده->مجموعه.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_سریع_EXIT, (دراز)داده->مجموعه.quick_exit);

  داده->multi_easy = عدم; /* pretend it does not exist */
  mresult = Curl_multi_جمع_کن_handle(multi, داده);
  إذا(mresult) {
    curl_multi_cleanup(multi);
    إذا(mresult == CURLM_بیرون_از_حافظه)
      أرجع CURLE_بیرون_از_حافظه;
    أرجع CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  داده->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(داده, &sigpipe_ctx);

  /* run the transfer */
  نتیجه = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (فراغ)Curl_multi_remove_handle(multi, داده);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  أرجع نتیجه;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  بنية Curl_eapi_guard guard = { 0 };
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_perform, &نتیجه)) {
    نتیجه = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}

#اگر_تعریف_شده DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(بنية Curl_easy *easy)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &نتیجه)) {
    نتیجه = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}
#پایان_اگر

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
فراغ curl_easy_cleanup(CURL *curl)
{
  بنية Curl_eapi_guard guard;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, عدم)) {
    بنية Curl_easy *داده = curl;
    بنية Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(داده, &sigpipe_ctx);
    Curl_close(&داده);
    sigpipe_restore(&sigpipe_ctx);
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#لغو_تعریف curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &نتیجه)) {
    بنية Curl_easy *داده = curl;
    va_list arg;
    فراغ *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, فراغ *);

    نتیجه = Curl_getinfo(داده, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}

ساكن CURLcode dupset(بنية Curl_easy *dst, بنية Curl_easy *src)
{
  CURLcode نتیجه = CURLE_OK;
  تعداد dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->مجموعه = src->مجموعه;
#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->مجموعه.mimepostp = عدم;
#پایان_اگر
  dst->مجموعه.str_copypostfields = عدم;

  Curl_u8_strset_init(&dst->مجموعه.رشتهها);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->مجموعه.blobs, 0, BLOB_آخرین * حجم(بنية curl_blob *));

  /* duplicate all strings */
  نتیجه = Curl_u8_strset_copy(&dst->مجموعه.رشتهها, &src->مجموعه.رشتهها);
  إذا(نتیجه)
    أرجع نتیجه;

  /* duplicate all blobs */
  لكل(j = (تعداد dupblob)0; j < BLOB_آخرین; j++) {
    نتیجه = Curl_setblobopt(&dst->مجموعه.blobs[j], src->مجموعه.blobs[j]);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  /* duplicate memory areas pointed to */
  إذا(src->مجموعه.str_copypostfields) {
    إذا(src->مجموعه.postfieldsize == -1)
      dst->مجموعه.str_copypostfields = curlx_strdup(src->مجموعه.str_copypostfields);
    وإلا
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->مجموعه.str_copypostfields =
        curlx_memdup0(src->مجموعه.str_copypostfields,
                      curlx_sotouz(src->مجموعه.postfieldsize));
    إذا(!dst->مجموعه.str_copypostfields)
      أرجع CURLE_بیرون_از_حافظه;
    /* point to the new copy */
    dst->مجموعه.postfields = dst->مجموعه.str_copypostfields;
  }

#إذا !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  إذا(src->مجموعه.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->مجموعه.mimepostp = curlx_malloc(حجم(*dst->مجموعه.mimepostp));
    إذا(!dst->مجموعه.mimepostp)
      أرجع CURLE_بیرون_از_حافظه;

    Curl_mime_initpart(dst->مجموعه.mimepostp);
    نتیجه = Curl_mime_duppart(dst, dst->مجموعه.mimepostp, src->مجموعه.mimepostp);
    إذا(نتیجه)
      أرجع نتیجه;
  }
#پایان_اگر

  إذا(src->مجموعه.resolve)
    dst->state.resolve = dst->مجموعه.resolve;

  أرجع نتیجه;
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
    بنية Curl_easy *داده = curl;

    outcurl = curlx_calloc(1, حجم(بنية Curl_easy));
    إذا(!outcurl)
      اذهب fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->مجموعه.میانگیر_اندازه = داده->مجموعه.میانگیر_اندازه;

    Curl_hash_init(&outcurl->meta_hash, 23, CURL_HASH_TYPE_بایتها,
                   dupeasy_meta_freeentry);
    curlx_dyn_init(&outcurl->state.headerb, CURL_MAX_HTTP_سرآیند);
    Curl_bufref_init(&outcurl->state.url);
    Curl_bufref_init(&outcurl->state.referer);
    Curl_netrc_init(&outcurl->state.netrc);

    /* the connection pool is setup on demand */
    outcurl->state.آخرین_conn_id = -1;
    outcurl->state.آخرین_cpid = UINT32_MAX;
    outcurl->id = -1;
    outcurl->mid = UINT32_MAX;
    outcurl->master_mid = UINT32_MAX;

#اگر_تعریف_نشده CURL_DISABLE_HTTP
    Curl_llist_init(&outcurl->state.httphdrs, عدم);
#پایان_اگر
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    إذا(dupset(outcurl, داده))
      اذهب fail;

    outcurl->پیشرفت.hide     = داده->پیشرفت.hide;
    outcurl->پیشرفت.callback = داده->پیشرفت.callback;

#اگر_تعریف_نشده CURL_DISABLE_COOKIES
    outcurl->state.cookielist = عدم;
    إذا(داده->cookies && داده->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      إذا(!outcurl->cookies)
        اذهب fail;
      outcurl->state.cookie_engine = TRUE;
    }

    إذا(داده->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(داده->state.cookielist);
      إذا(!outcurl->state.cookielist)
        اذهب fail;
    }
#پایان_اگر

    إذا(Curl_bufref_ptr(&داده->state.url)) {
      Curl_bufref_مجموعه(&outcurl->state.url,
                      Curl_bufref_dup(&داده->state.url), 0,
                      curl_free);
      إذا(!Curl_bufref_ptr(&outcurl->state.url))
        اذهب fail;
    }
    إذا(Curl_bufref_ptr(&داده->state.referer)) {
      Curl_bufref_مجموعه(&outcurl->state.referer,
                      Curl_bufref_dup(&داده->state.referer), 0,
                      curl_free);
      إذا(!Curl_bufref_ptr(&outcurl->state.referer))
        اذهب fail;
    }

#اگر_تعریف_نشده CURL_DISABLE_ALTSVC
    إذا(داده->asi) {
      ثابت حرف *str;
      outcurl->asi = Curl_altsvc_init();
      إذا(!outcurl->asi)
        اذهب fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      إذا(str)
        (فراغ)Curl_altsvc_بارگیری(outcurl->asi, str);
    }
#پایان_اگر
#اگر_تعریف_نشده CURL_DISABLE_HSTS
    إذا(داده->hsts) {
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
      إذا(Curl_hsts_copy(outcurl->hsts, داده->hsts))
        اذهب fail;
    }
#پایان_اگر

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_عدد;
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع outcurl;

fail:
  إذا(outcurl) {
#اگر_تعریف_نشده CURL_DISABLE_COOKIES
    curlx_free(outcurl->cookies);
#پایان_اگر
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
    بنية Curl_easy *داده = curl;

    داده->state.آخرین_conn_id = -1; /* clear remembered connection id */
    داده->state.آخرین_cpid = UINT32_MAX;
    Curl_req_hard_reset(&داده->req, داده);
    Curl_hash_clean(&داده->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(داده);
    /* zero out UserDefined data: */
    Curl_freeset(داده);
    memset(&داده->مجموعه, 0, حجم(بنية UserDefined));
    Curl_init_userdefined(داده);

    /* zero out Progress data: */
    memset(&داده->پیشرفت, 0, حجم(بنية پیشرفت_2));

    /* zero out PureInfo data: */
    Curl_initinfo(داده);

    داده->پیشرفت.hide = TRUE;

    /* zero out authentication data: */
    memset(&داده->state.authhost, 0, حجم(بنية auth));
    memset(&داده->state.authproxy, 0, حجم(بنية auth));

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(داده);
#پایان_اگر
    داده->master_mid = UINT32_MAX;
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
  CURLcode نتیجه = CURLE_OK;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_pause, &نتیجه)) {
    bool تغییریافته = FALSE;
    بنية Curl_easy *داده = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    إذا(!داده->conn) {
      /* crazy input, do not continue */
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
    }

    recv_paused = Curl_xfer_recv_is_paused(داده);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(داده);
    send_paused_new = (action & CURLPAUSE_SEND);

    إذا((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(داده))) {
      تغییریافته = TRUE;
      نتیجه = Curl_1st_fatal(
        نتیجه, Curl_xfer_pause_send(داده, send_paused_new));
    }

    إذا(recv_paused != recv_paused_new) {
      تغییریافته = TRUE;
      نتیجه = Curl_1st_fatal(
        نتیجه, Curl_xfer_pause_recv(داده, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    إذا(!Curl_xfer_is_blocked(داده)) {
      /* reset the too-slow time keeper */
      داده->state.keeps_سرعت.tv_sec = 0;
      إذا(داده->multi) {
        Curl_multi_mark_dirty(داده); /* make it run */
        /* On changes, tell application to update its timers. */
        إذا(تغییریافته && Curl_update_timer(داده->multi) && !نتیجه)
          نتیجه = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    إذا(!نتیجه && تغییریافته && !داده->state.انجامشده && داده->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(داده->multi, داده))
      نتیجه = CURLE_ABORTED_BY_CALLBACK;
  }
بیرون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}

ساكن CURLcode easy_connection(بنية Curl_easy *داده,
                                بنية connectdata **connp)
{
  curl_socket_t sfd;

  إذا(!داده)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  إذا(!داده->مجموعه.connect_only) {
    failf(داده, "CONNECT_ONLY is required");
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(داده, connp);

  إذا(sfd == CURL_SOCKET_BAD) {
    failf(داده, "Failed to get last socket used for connection #%" FMT_OFF_T,
          داده->state.آخرین_conn_id);
    أرجع CURLE_UNSUPPORTED_PROTOCOL;
  }

  أرجع CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(بنية Curl_easy *داده,
                        فراغ *میانگیر, size_t buflen, size_t *n)
{
  CURLcode نتیجه;
  بنية connectdata *c;

  نتیجه = easy_connection(داده, &c);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(!داده->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(داده, c, TRUE);

  *n = 0;
  أرجع Curl_conn_recv(داده, FIRSTSOCKET, میانگیر, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, فراغ *میانگیر, size_t buflen, size_t *n)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_recv, &نتیجه)) {
    إذا(!n || (buflen && !میانگیر)) {
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
    }
    نتیجه = Curl_easy_recv(curl, میانگیر, buflen, n);
  }
بیرون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}

#اگر_تعریف_نشده CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(بنية Curl_easy *داده)
{
  CURLcode نتیجه;
  بنية connectdata *c = عدم;

  نتیجه = easy_connection(داده, &c);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(!داده->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(داده, c, TRUE);

  أرجع CURLE_OK;
}
#پایان_اگر /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(بنية Curl_easy *داده, ثابت فراغ *میانگیر,
                       size_t buflen, size_t *n)
{
  CURLcode نتیجه;
  بنية connectdata *c = عدم;
  بنية Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  نتیجه = easy_connection(داده, &c);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(!داده->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(داده, c, TRUE);

  sigpipe_ignore(داده, &sigpipe_ctx);
  نتیجه = Curl_conn_send(داده, FIRSTSOCKET, میانگیر, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  إذا(نتیجه && نتیجه != CURLE_AGAIN)
    أرجع CURLE_SEND_ERROR;
  أرجع نتیجه;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ثابت فراغ *میانگیر, size_t buflen,
                        size_t *n)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_send, &نتیجه)) {
    بنية Curl_easy *داده = curl;
    size_t written = 0;

    إذا(!n || (buflen && !میانگیر)) {
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
    }
    نتیجه = Curl_senddata(داده, میانگیر, buflen, &written);
    *n = written;
  }
بیرون:
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &نتیجه)) {
    /* Use the common function to keep connections alive. */
    نتیجه = Curl_cpool_upkeep((بنية Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
}

CURLcode curl_easy_ssls_import(CURL *curl, ثابت حرف *session_کلید,
                               ثابت بیعلامت حرف *shmac, size_t shmac_len,
                               ثابت بیعلامت حرف *sdata, size_t sdata_len)
{
#إذا defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &نتیجه)) {
    إذا(!sdata || !sdata_len)
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    وإلا
      نتیجه = Curl_ssl_session_import((بنية Curl_easy *)curl, session_کلید,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
#وإلا
  (فراغ)curl;
  (فراغ)session_کلید;
  (فراغ)shmac;
  (فراغ)shmac_len;
  (فراغ)sdata;
  (فراغ)sdata_len;
  أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               فراغ *userptr)
{
#إذا defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  بنية Curl_eapi_guard guard;
  CURLcode نتیجه;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &نتیجه)) {
    إذا(!export_fn)
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    وإلا
      نتیجه = Curl_ssl_session_export((بنية Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  أرجع نتیجه;
#وإلا
  (فراغ)curl;
  (فراغ)export_fn;
  (فراغ)userptr;
  أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
}

CURLcode Curl_meta_مجموعه(بنية Curl_easy *داده, ثابت حرف *کلید,
                       فراغ *meta_داده, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_داده); /* never set to NULL */
  إذا(!Curl_hash_add2(&داده->meta_hash, کلید, طول_رشته(کلید) + 1,
                     meta_داده, meta_dtor)) {
    meta_dtor(کلید, طول_رشته(کلید) + 1, meta_داده);
    أرجع CURLE_بیرون_از_حافظه;
  }
  أرجع CURLE_OK;
}

فراغ Curl_meta_remove(بنية Curl_easy *داده, ثابت حرف *کلید)
{
  Curl_hash_delete(&داده->meta_hash, کلید, طول_رشته(کلید) + 1);
}

فراغ *Curl_meta_get(بنية Curl_easy *داده, ثابت حرف *کلید)
{
  أرجع Curl_hash_pick(&داده->meta_hash, کلید, طول_رشته(کلید) + 1);
}

فراغ Curl_meta_reset(بنية Curl_easy *داده)
{
  Curl_hash_clean(&داده->meta_hash);
}
