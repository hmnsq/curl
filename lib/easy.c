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

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم

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
ساکن غیرعلامتی صحیح initialized;
#اگر_تعریف_شدہ USE_WINSOCK
ساکن bool winsock_initialized;
#اگر_ختم

#اگر_تعریف_شدہ GLOBAL_INIT_IS_THREADSAFE

ساکن curl_simple_تالا s_تالا = CURL_SIMPLE_تالا_INIT;
#تعریف_کرو global_init_تالا()   curl_simple_تالا_تالا(&s_تالا)
#تعریف_کرو global_init_unlock() curl_simple_تالا_unlock(&s_تالا)

#ورنہ

#تعریف_کرو global_init_تالا()
#تعریف_کرو global_init_unlock()

#اگر_ختم

#اگر defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ڈالو)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#اگر_ختم

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)حافظہ_دو;
curl_free_callback Curl_cfree = (curl_free_callback)آزاد;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_کم;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)صفر_حافظہ_دو;

#اگر defined(_MSC_VER) && defined(_DLL)
#  pragma warning(نکالو)
#اگر_ختم

#اگر_تعریف_شدہ DEBUGBUILD
ساکن حرف *leakpointer;
#اگر_ختم

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
ساکن CURLcode global_init(طویل flags, bool memoryfuncs)
{
  (خالی)flags;

  اگر(initialized++)
    واپس CURLE_OK;

  اگر(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)حافظہ_دو;
    Curl_cfree = (curl_free_callback)آزاد;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_کم;
    Curl_ccalloc = (curl_calloc_callback)صفر_حافظہ_دو;
  }

#اگر_تعریف_شدہ _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#اگر_ختم

  اگر(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    جاؤ fail;
  }

#اگر_تعریف_شدہ USE_WINDOWS_SSPI
  اگر(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    جاؤ fail;
  }
#اگر_ختم

#اگر_تعریف_شدہ USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  اگر(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    اگر(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      جاؤ fail;
    }
    winsock_initialized = TRUE;
  }
#اگر_ختم

  اگر(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    جاؤ fail;
  }

  اگر(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    جاؤ fail;
  }

  اگر(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    جاؤ fail;
  }

  اگر(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    جاؤ fail;
  }

  اگر(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    جاؤ fail;
  }

  اگر(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    جاؤ fail;
  }

  اگر(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    جاؤ fail;
  }

#اگر_تعریف_شدہ DEBUGBUILD
  اگر(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#اگر_ختم

  واپس CURLE_OK;

fail:
#اگر_تعریف_شدہ USE_WINSOCK
  اگر(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#اگر_ختم
  initialized--; /* undo the increase */
  واپس CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(طویل flags)
{
  CURLcode نتیجہ;
  global_init_تالا();

  نتیجہ = global_init(flags, TRUE);

  global_init_unlock();

  واپس نتیجہ;
}

/*
 * curl_global_init_mem() globally initializes curl and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(طویل flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode نتیجہ;

  /* Invalid input, return immediately */
  اگر(!m || !f || !r || !s || !c)
    واپس CURLE_FAILED_INIT;

  global_init_تالا();

  اگر(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    واپس CURLE_OK;
  }

  /* set memory functions before global_init() in case it wants memory
     functions */
  Curl_cmalloc = m;
  Curl_cfree = f;
  Curl_cstrdup = s;
  Curl_crealloc = r;
  Curl_ccalloc = c;

  /* Call the actual init function, but without setting */
  نتیجہ = global_init(flags, FALSE);

  global_init_unlock();

  واپس نتیجہ;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
خالی curl_global_cleanup(خالی)
{
  global_init_تالا();

  اگر(!initialized) {
    global_init_unlock();
    واپس;
  }

  اگر(--initialized) {
    global_init_unlock();
    واپس;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#اگر_تعریف_شدہ USE_WINSOCK
  اگر(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#اگر_ختم

#اگر_تعریف_شدہ USE_WINDOWS_SSPI
  Curl_sspi_global_cleanup();
#اگر_ختم

#اگر_تعریف_شدہ DEBUGBUILD
  curlx_free(leakpointer);
#اگر_ختم

  global_init_unlock();
}

/**
 * curl_global_trace() globally initializes curl logging.
 */
CURLcode curl_global_trace(ثابت حرف *config)
{
#اگر_غیر_تعریف_شدہ CURL_DISABLE_VERBOSE_لڑیاں
  CURLcode نتیجہ;
  global_init_تالا();

  نتیجہ = Curl_trc_opt(config);

  global_init_unlock();

  واپس نتیجہ;
#ورنہ
  (خالی)config;
  واپس CURLE_OK;
#اگر_ختم
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ثابت حرف *name,
                              ثابت curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_تالا();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  واپس rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(خالی)
{
  CURLcode نتیجہ;
  ساخت Curl_easy *ڈیٹا;

  /* Make sure we inited the global SSL stuff */
  global_init_تالا();

  اگر(!initialized) {
    نتیجہ = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    اگر(نتیجہ) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      واپس تہی;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  نتیجہ = Curl_open(&ڈیٹا);
  اگر(نتیجہ) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    واپس تہی;
  }

  واپس ڈیٹا;
}

#اگر_تعریف_شدہ DEBUGBUILD

ساخت socketmonitor {
  ساخت socketmonitor *اگلا; /* the next node in the list or NULL */
  ساخت pollfd socket; /* socket info of what to monitor */
};

ساخت events {
  طویل ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  صحیح عدد_sockets;      /* number of nodes in the monitor list */
  ساخت socketmonitor *list; /* list of sockets to monitor */
  صحیح running_handles;  /* store the returned number */
};

#تعریف_کرو DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
ساکن صحیح events_timer(CURLM *multi,    /* multi handle */
                        طویل timeout_ms, /* see above */
                        خالی *userp)     /* private callback pointer */
{
  ساخت events *ev = userp;
  (خالی)multi;
#اگر DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#اگر_ختم
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  واپس 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
ساکن صحیح poll2cselect(صحیح pollmask)
{
  صحیح omask = 0;
  اگر(pollmask & POLLIN)
    omask |= CURL_CSELECT_اندر;
  اگر(pollmask & POLLOUT)
    omask |= CURL_CSELECT_برآمد;
  اگر(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  واپس omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
ساکن مختصر socketcb2poll(صحیح pollmask)
{
  مختصر omask = 0;
  اگر(pollmask & CURL_POLL_اندر)
    omask |= POLLIN;
  اگر(pollmask & CURL_POLL_برآمد)
    omask |= POLLOUT;
  واپس omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
ساکن صحیح events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         صحیح what,        /* see above */
                         خالی *userp,     /* private callback
اشارہ_گر*/
                         خالی *socketp)   /* private socket
اشارہ_گر*/
{
  ساخت events *ev = userp;
  ساخت socketmonitor *m;
  ساخت socketmonitor *پچھلا = تہی;
  bool ملا = FALSE;
  ساخت Curl_easy *ڈیٹا = easy;

#اگر_تعریف_شدہ CURL_DISABLE_VERBOSE_لڑیاں
  (خالی)easy;
#اگر_ختم
  (خالی)socketp;

  m = ev->list;
  جب(m) {
    اگر(m->socket.fd == s) {
      ملا = TRUE;
      اگر(what == CURL_POLL_REMOVE) {
        ساخت socketmonitor *nxt = m->اگلا;
        /* remove this node from the list of monitored sockets */
        اگر(پچھلا)
          پچھلا->اگلا = nxt;
        ورنہ
          ev->list = nxt;
        curlx_free(m);
        infof(ڈیٹا, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      ورنہ {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(ڈیٹا, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_اندر) ? "IN" : "",
              (what & CURL_POLL_برآمد) ? "OUT" : "");
      }
      توڑو;
    }
    پچھلا = m;
    m = m->اگلا; /* move to next node */
  }

  اگر(!ملا) {
    اگر(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(ڈیٹا, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    ورنہ {
      m = curlx_malloc(حجم(ساخت socketmonitor));
      اگر(m) {
        m->اگلا = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(ڈیٹا, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_اندر) ? "IN" : "",
              (what & CURL_POLL_برآمد) ? "OUT" : "");
      }
      ورنہ
        واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  واپس 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
ساکن خالی events_تیاری(ساخت Curl_multi *multi, ساخت events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

ساکن CURLcode populate_cpfds(ساخت curl_pollfds *cpfds, ساخت events *ev)
{
  ساخت socketmonitor *m;
  CURLcode نتیجہ = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  برائے(m = ev->list; m && !نتیجہ; m = m->اگلا) {
    نتیجہ = Curl_pollfds_جوڑو_sock(cpfds, m->socket.fd, m->socket.events);
#اگر DEBUG_EV_POLL
    اگر(!نتیجہ)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#اگر_ختم
  }
  واپس نتیجہ;
}

ساکن CURLcode poll_cpfds(ساخت events *ev,
                           ساخت curl_pollfds *cpfds,
                           صحیح *pollrc)
{
  اگر(cpfds->n) {
    /* wait for activity or timeout */
#اگر DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#اگر_ختم
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#اگر DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#اگر_ختم
    اگر(*pollrc < 0)
      واپس CURLE_UNRECOVERABLE_POLL;
  }
  ورنہ {
#اگر DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#اگر_ختم
    *pollrc = 0;
    اگر(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  واپس CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
ساکن CURLcode wait_or_timeout(ساخت Curl_multi *multi, ساخت events *ev)
{
  ساخت pollfd fds_on_ڈھیر[8];
  ساخت curl_pollfds cpfds;
  bool مکمل = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_ڈھیر, CURL_ARRAYSIZE(fds_on_ڈھیر));
  جب(!مکمل) {
    CURLMsg *پیغام;
    صحیح pollrc;
    ساخت curltime آغاز;

    نتیجہ = populate_cpfds(&cpfds, ev);
    اگر(نتیجہ)
      جاؤ برآمد;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&آغاز);

    نتیجہ = poll_cpfds(ev, &cpfds, &pollrc);
    اگر(نتیجہ)
      جاؤ برآمد;

    ev->msbump = FALSE; /* reset here */

    اگر(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#اگر DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#اگر_ختم
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    ورنہ {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      غیرعلامتی صحیح i;
      برائے(i = 0; i < cpfds.n; i++) {
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
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), آغاز);
        اگر(spent_ms > 0) {
#اگر DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (طویل)spent_ms);
#اگر_ختم
          اگر(spent_ms > ev->ms)
            ev->ms = 0;
          ورنہ
            ev->ms -= (طویل)spent_ms;
        }
      }
    }

    اگر(mresult) {
      نتیجہ = CURLE_URL_MALFORMAT;
      جاؤ برآمد;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    پیغام = curl_multi_info_read(multi, &pollrc);
    اگر(پیغام) {
      نتیجہ = پیغام->ڈیٹا.نتیجہ;
      مکمل = TRUE;
    }
  }
برآمد:
  Curl_pollfds_cleanup(&cpfds);
  واپس نتیجہ;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
ساکن CURLcode easy_events(ساخت Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  ساکن ساخت events evs = { -1, FALSE, 0, تہی, 0 };

  /* if running event-based, do some further multi inits */
  events_تیاری(multi, &evs);

  واپس wait_or_timeout(multi, &evs);
}
#ورنہ /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#تعریف_کرو easy_events(x) CURLE_NOT_BUILT_اندر
#اگر_ختم

ساکن CURLcode easy_transfer(ساخت Curl_multi *multi)
{
  bool مکمل = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode نتیجہ = CURLE_OK;

  جب(!مکمل && !mresult) {
    صحیح still_running = 0;

    mresult = curl_multi_poll(multi, تہی, 0, 1000, تہی);

    اگر(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    اگر(!mresult && !still_running) {
      صحیح rc;
      CURLMsg *پیغام = curl_multi_info_read(multi, &rc);
      اگر(پیغام) {
        نتیجہ = پیغام->ڈیٹا.نتیجہ;
        مکمل = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  اگر(mresult) {
    نتیجہ = (mresult == CURLM_برآمد_کا_حافظہ) ? CURLE_برآمد_کا_حافظہ :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_دلیل;
  }

  واپس نتیجہ;
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
ساکن CURLcode easy_perform(ساخت Curl_easy *ڈیٹا, bool events)
{
  ساخت Curl_multi *multi;
  CURLMcode mresult;
  CURLcode نتیجہ = CURLE_OK;
  ساخت Curl_sigpipe_ctx sigpipe_ctx;

  اگر(!ڈیٹا)
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(ڈیٹا->سیٹ.errorbuffer)
    /* clear this as early as possible */
    ڈیٹا->سیٹ.errorbuffer[0] = 0;

  ڈیٹا->state.os_errno = 0;

  اگر(ڈیٹا->multi) {
    failf(ڈیٹا, "easy handle already used in multi handle");
    واپس CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  اگر(ڈیٹا->conn) {
    ساخت connectdata *conn = ڈیٹا->conn;
    Curl_detach_connection(ڈیٹا);
    Curl_conn_close(ڈیٹا, conn, TRUE);
    DEBUGASSERT(!ڈیٹا->conn);
  }

  اگر(ڈیٹا->multi_easy)
    multi = ڈیٹا->multi_easy;
  ورنہ {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    اگر(!multi)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(Curl_api_multi_is_اندر_callback(multi))
    واپس CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (طویل)ڈیٹا->سیٹ.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_تیز_EXIT, (طویل)ڈیٹا->سیٹ.quick_exit);

  ڈیٹا->multi_easy = تہی; /* pretend it does not exist */
  mresult = Curl_multi_جوڑو_handle(multi, ڈیٹا);
  اگر(mresult) {
    curl_multi_cleanup(multi);
    اگر(mresult == CURLM_برآمد_کا_حافظہ)
      واپس CURLE_برآمد_کا_حافظہ;
    واپس CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  ڈیٹا->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(ڈیٹا, &sigpipe_ctx);

  /* run the transfer */
  نتیجہ = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (خالی)Curl_multi_remove_handle(multi, ڈیٹا);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  واپس نتیجہ;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  ساخت Curl_eapi_guard guard = { 0 };
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_perform, &نتیجہ)) {
    نتیجہ = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}

#اگر_تعریف_شدہ DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(ساخت Curl_easy *easy)
{
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &نتیجہ)) {
    نتیجہ = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}
#اگر_ختم

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
خالی curl_easy_cleanup(CURL *curl)
{
  ساخت Curl_eapi_guard guard;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, تہی)) {
    ساخت Curl_easy *ڈیٹا = curl;
    ساخت Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(ڈیٹا, &sigpipe_ctx);
    Curl_close(&ڈیٹا);
    sigpipe_restore(&sigpipe_ctx);
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#تعریف_منسوخ curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &نتیجہ)) {
    ساخت Curl_easy *ڈیٹا = curl;
    va_list arg;
    خالی *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, خالی *);

    نتیجہ = Curl_getinfo(ڈیٹا, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}

ساکن CURLcode dupset(ساخت Curl_easy *dst, ساخت Curl_easy *src)
{
  CURLcode نتیجہ = CURLE_OK;
  شمار dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->سیٹ = src->سیٹ;
#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->سیٹ.mimepostp = تہی;
#اگر_ختم
  dst->سیٹ.str_copypostfields = تہی;

  Curl_u8_strset_init(&dst->سیٹ.لڑیاں);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->سیٹ.blobs, 0, BLOB_آخری * حجم(ساخت curl_blob *));

  /* duplicate all strings */
  نتیجہ = Curl_u8_strset_copy(&dst->سیٹ.لڑیاں, &src->سیٹ.لڑیاں);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* duplicate all blobs */
  برائے(j = (شمار dupblob)0; j < BLOB_آخری; j++) {
    نتیجہ = Curl_setblobopt(&dst->سیٹ.blobs[j], src->سیٹ.blobs[j]);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  /* duplicate memory areas pointed to */
  اگر(src->سیٹ.str_copypostfields) {
    اگر(src->سیٹ.postfieldsize == -1)
      dst->سیٹ.str_copypostfields = curlx_strdup(src->سیٹ.str_copypostfields);
    ورنہ
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->سیٹ.str_copypostfields =
        curlx_memdup0(src->سیٹ.str_copypostfields,
                      curlx_sotouz(src->سیٹ.postfieldsize));
    اگر(!dst->سیٹ.str_copypostfields)
      واپس CURLE_برآمد_کا_حافظہ;
    /* point to the new copy */
    dst->سیٹ.postfields = dst->سیٹ.str_copypostfields;
  }

#اگر !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  اگر(src->سیٹ.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->سیٹ.mimepostp = curlx_malloc(حجم(*dst->سیٹ.mimepostp));
    اگر(!dst->سیٹ.mimepostp)
      واپس CURLE_برآمد_کا_حافظہ;

    Curl_mime_initpart(dst->سیٹ.mimepostp);
    نتیجہ = Curl_mime_duppart(dst, dst->سیٹ.mimepostp, src->سیٹ.mimepostp);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
#اگر_ختم

  اگر(src->سیٹ.resolve)
    dst->state.resolve = dst->سیٹ.resolve;

  واپس نتیجہ;
}

ساکن خالی dupeasy_meta_freeentry(خالی *p)
{
  (خالی)p;
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
  ساخت Curl_eapi_guard guard;
  ساخت Curl_easy *outcurl = تہی;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, تہی)) {
    ساخت Curl_easy *ڈیٹا = curl;

    outcurl = curlx_calloc(1, حجم(ساخت Curl_easy));
    اگر(!outcurl)
      جاؤ fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->سیٹ.بفر_سائز = ڈیٹا->سیٹ.بفر_سائز;

    Curl_hash_init(&outcurl->meta_hash, 23, CURL_HASH_TYPE_بائٹیں,
                   dupeasy_meta_freeentry);
    curlx_dyn_init(&outcurl->state.headerb, CURL_MAX_HTTP_سرخی);
    Curl_bufref_init(&outcurl->state.url);
    Curl_bufref_init(&outcurl->state.referer);
    Curl_netrc_init(&outcurl->state.netrc);

    /* the connection pool is setup on demand */
    outcurl->state.آخری_conn_id = -1;
    outcurl->state.آخری_cpid = UINT32_MAX;
    outcurl->id = -1;
    outcurl->mid = UINT32_MAX;
    outcurl->master_mid = UINT32_MAX;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
    Curl_llist_init(&outcurl->state.httphdrs, تہی);
#اگر_ختم
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    اگر(dupset(outcurl, ڈیٹا))
      جاؤ fail;

    outcurl->پیش_رفت.hide     = ڈیٹا->پیش_رفت.hide;
    outcurl->پیش_رفت.callback = ڈیٹا->پیش_رفت.callback;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
    outcurl->state.cookielist = تہی;
    اگر(ڈیٹا->cookies && ڈیٹا->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      اگر(!outcurl->cookies)
        جاؤ fail;
      outcurl->state.cookie_engine = TRUE;
    }

    اگر(ڈیٹا->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(ڈیٹا->state.cookielist);
      اگر(!outcurl->state.cookielist)
        جاؤ fail;
    }
#اگر_ختم

    اگر(Curl_bufref_ptr(&ڈیٹا->state.url)) {
      Curl_bufref_سیٹ(&outcurl->state.url,
                      Curl_bufref_dup(&ڈیٹا->state.url), 0,
                      curl_free);
      اگر(!Curl_bufref_ptr(&outcurl->state.url))
        جاؤ fail;
    }
    اگر(Curl_bufref_ptr(&ڈیٹا->state.referer)) {
      Curl_bufref_سیٹ(&outcurl->state.referer,
                      Curl_bufref_dup(&ڈیٹا->state.referer), 0,
                      curl_free);
      اگر(!Curl_bufref_ptr(&outcurl->state.referer))
        جاؤ fail;
    }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_ALTSVC
    اگر(ڈیٹا->asi) {
      ثابت حرف *str;
      outcurl->asi = Curl_altsvc_init();
      اگر(!outcurl->asi)
        جاؤ fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      اگر(str)
        (خالی)Curl_altsvc_بھرو(outcurl->asi, str);
    }
#اگر_ختم
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HSTS
    اگر(ڈیٹا->hsts) {
      ثابت حرف *str;
      outcurl->hsts = Curl_hsts_init();
      اگر(!outcurl->hsts)
        جاؤ fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      اگر(str)
        (خالی)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (خالی)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      اگر(Curl_hsts_copy(outcurl->hsts, ڈیٹا->hsts))
        جاؤ fail;
    }
#اگر_ختم

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_نمبر;
  }
  CURL_EAPI_LEAVE(&guard);
  واپس outcurl;

fail:
  اگر(outcurl) {
#اگر_غیر_تعریف_شدہ CURL_DISABLE_COOKIES
    curlx_free(outcurl->cookies);
#اگر_ختم
    curlx_dyn_free(&outcurl->state.headerb);
    Curl_altsvc_cleanup(&outcurl->asi);
    Curl_hsts_cleanup(&outcurl->hsts);
    Curl_freeset(outcurl);
    curlx_free(outcurl);
  }

  CURL_EAPI_LEAVE(&guard);
  واپس تہی;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
خالی curl_easy_reset(CURL *curl)
{
  ساخت Curl_eapi_guard guard;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_reset, تہی)) {
    ساخت Curl_easy *ڈیٹا = curl;

    ڈیٹا->state.آخری_conn_id = -1; /* clear remembered connection id */
    ڈیٹا->state.آخری_cpid = UINT32_MAX;
    Curl_req_hard_reset(&ڈیٹا->req, ڈیٹا);
    Curl_hash_clean(&ڈیٹا->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(ڈیٹا);
    /* zero out UserDefined data: */
    Curl_freeset(ڈیٹا);
    memset(&ڈیٹا->سیٹ, 0, حجم(ساخت UserDefined));
    Curl_init_userdefined(ڈیٹا);

    /* zero out Progress data: */
    memset(&ڈیٹا->پیش_رفت, 0, حجم(ساخت پیش_رفت_2));

    /* zero out PureInfo data: */
    Curl_initinfo(ڈیٹا);

    ڈیٹا->پیش_رفت.hide = TRUE;

    /* zero out authentication data: */
    memset(&ڈیٹا->state.authhost, 0, حجم(ساخت auth));
    memset(&ڈیٹا->state.authproxy, 0, حجم(ساخت auth));

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(ڈیٹا);
#اگر_ختم
    ڈیٹا->master_mid = UINT32_MAX;
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
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ = CURLE_OK;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_pause, &نتیجہ)) {
    bool بدلا = FALSE;
    ساخت Curl_easy *ڈیٹا = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    اگر(!ڈیٹا->conn) {
      /* crazy input, do not continue */
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }

    recv_paused = Curl_xfer_recv_is_paused(ڈیٹا);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(ڈیٹا);
    send_paused_new = (action & CURLPAUSE_SEND);

    اگر((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(ڈیٹا))) {
      بدلا = TRUE;
      نتیجہ = Curl_1st_fatal(
        نتیجہ, Curl_xfer_pause_send(ڈیٹا, send_paused_new));
    }

    اگر(recv_paused != recv_paused_new) {
      بدلا = TRUE;
      نتیجہ = Curl_1st_fatal(
        نتیجہ, Curl_xfer_pause_recv(ڈیٹا, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    اگر(!Curl_xfer_is_blocked(ڈیٹا)) {
      /* reset the too-slow time keeper */
      ڈیٹا->state.keeps_رفتار.tv_sec = 0;
      اگر(ڈیٹا->multi) {
        Curl_multi_mark_dirty(ڈیٹا); /* make it run */
        /* On changes, tell application to update its timers. */
        اگر(بدلا && Curl_update_timer(ڈیٹا->multi) && !نتیجہ)
          نتیجہ = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    اگر(!نتیجہ && بدلا && !ڈیٹا->state.مکمل && ڈیٹا->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(ڈیٹا->multi, ڈیٹا))
      نتیجہ = CURLE_ABORTED_BY_CALLBACK;
  }
برآمد:
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}

ساکن CURLcode easy_connection(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata **connp)
{
  curl_socket_t sfd;

  اگر(!ڈیٹا)
    واپس CURLE_BAD_FUNCTION_دلیل;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  اگر(!ڈیٹا->سیٹ.connect_only) {
    failf(ڈیٹا, "CONNECT_ONLY is required");
    واپس CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(ڈیٹا, connp);

  اگر(sfd == CURL_SOCKET_BAD) {
    failf(ڈیٹا, "Failed to get last socket used for connection #%" FMT_OFF_T,
          ڈیٹا->state.آخری_conn_id);
    واپس CURLE_UNSUPPORTED_PROTOCOL;
  }

  واپس CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(ساخت Curl_easy *ڈیٹا,
                        خالی *بفر, size_t buflen, size_t *n)
{
  CURLcode نتیجہ;
  ساخت connectdata *c;

  نتیجہ = easy_connection(ڈیٹا, &c);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(!ڈیٹا->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(ڈیٹا, c, TRUE);

  *n = 0;
  واپس Curl_conn_recv(ڈیٹا, FIRSTSOCKET, بفر, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, خالی *بفر, size_t buflen, size_t *n)
{
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_recv, &نتیجہ)) {
    اگر(!n || (buflen && !بفر)) {
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }
    نتیجہ = Curl_easy_recv(curl, بفر, buflen, n);
  }
برآمد:
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ;
  ساخت connectdata *c = تہی;

  نتیجہ = easy_connection(ڈیٹا, &c);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(!ڈیٹا->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(ڈیٹا, c, TRUE);

  واپس CURLE_OK;
}
#اگر_ختم /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(ساخت Curl_easy *ڈیٹا, ثابت خالی *بفر,
                       size_t buflen, size_t *n)
{
  CURLcode نتیجہ;
  ساخت connectdata *c = تہی;
  ساخت Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  نتیجہ = easy_connection(ڈیٹا, &c);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(!ڈیٹا->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(ڈیٹا, c, TRUE);

  sigpipe_ignore(ڈیٹا, &sigpipe_ctx);
  نتیجہ = Curl_conn_send(ڈیٹا, FIRSTSOCKET, بفر, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  اگر(نتیجہ && نتیجہ != CURLE_AGAIN)
    واپس CURLE_SEND_ERROR;
  واپس نتیجہ;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ثابت خالی *بفر, size_t buflen,
                        size_t *n)
{
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_send, &نتیجہ)) {
    ساخت Curl_easy *ڈیٹا = curl;
    size_t written = 0;

    اگر(!n || (buflen && !بفر)) {
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }
    نتیجہ = Curl_senddata(ڈیٹا, بفر, buflen, &written);
    *n = written;
  }
برآمد:
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &نتیجہ)) {
    /* Use the common function to keep connections alive. */
    نتیجہ = Curl_cpool_upkeep((ساخت Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
}

CURLcode curl_easy_ssls_import(CURL *curl, ثابت حرف *session_کلید,
                               ثابت غیرعلامتی حرف *shmac, size_t shmac_len,
                               ثابت غیرعلامتی حرف *sdata, size_t sdata_len)
{
#اگر defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &نتیجہ)) {
    اگر(!sdata || !sdata_len)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      نتیجہ = Curl_ssl_session_import((ساخت Curl_easy *)curl, session_کلید,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
#ورنہ
  (خالی)curl;
  (خالی)session_کلید;
  (خالی)shmac;
  (خالی)shmac_len;
  (خالی)sdata;
  (خالی)sdata_len;
  واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               خالی *userptr)
{
#اگر defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ساخت Curl_eapi_guard guard;
  CURLcode نتیجہ;

  اگر(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &نتیجہ)) {
    اگر(!export_fn)
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    ورنہ
      نتیجہ = Curl_ssl_session_export((ساخت Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  واپس نتیجہ;
#ورنہ
  (خالی)curl;
  (خالی)export_fn;
  (خالی)userptr;
  واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
}

CURLcode Curl_meta_سیٹ(ساخت Curl_easy *ڈیٹا, ثابت حرف *کلید,
                       خالی *meta_ڈیٹا, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_ڈیٹا); /* never set to NULL */
  اگر(!Curl_hash_add2(&ڈیٹا->meta_hash, کلید, لڑی_لمبائی(کلید) + 1,
                     meta_ڈیٹا, meta_dtor)) {
    meta_dtor(کلید, لڑی_لمبائی(کلید) + 1, meta_ڈیٹا);
    واپس CURLE_برآمد_کا_حافظہ;
  }
  واپس CURLE_OK;
}

خالی Curl_meta_remove(ساخت Curl_easy *ڈیٹا, ثابت حرف *کلید)
{
  Curl_hash_delete(&ڈیٹا->meta_hash, کلید, لڑی_لمبائی(کلید) + 1);
}

خالی *Curl_meta_get(ساخت Curl_easy *ڈیٹا, ثابت حرف *کلید)
{
  واپس Curl_hash_pick(&ڈیٹا->meta_hash, کلید, لڑی_لمبائی(کلید) + 1);
}

خالی Curl_meta_reset(ساخت Curl_easy *ڈیٹا)
{
  Curl_hash_clean(&ڈیٹا->meta_hash);
}
