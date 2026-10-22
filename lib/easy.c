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
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include "urldata.h"
#include "api.h"
#include "transfer.h"
#include "vdns/hostip.h"
#include "vtls/vtls.h"
#include "vtls/vtls_scache.h"
#include "vquic/vquic.h"
#include "url.h"
#include "getinfo.h"
#include "curlx/strdup.h"
#include "easyif.h"
#include "multiif.h"
#include "multi_ev.h"
#include "select.h"
#include "cfilters.h"
#include "sendf.h"
#include "curl_trc.h"
#include "connect.h" /* for Curl_getconnectinfo */
#include "slist.h"
#include "mime.h"
#include "amigaos.h"
#include "macos.h"
#include "curlx/wait.h"
#include "sigpipe.h"
#include "vssh/ssh.h"
#include "setopt.h"
#include "http_digest.h"
#include "curlx/dynbuf.h"
#include "bufref.h"
#include "altsvc.h"
#include "hsts.h"
#include "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#include "curlx/timeval.h" /* for curlx_now_init() */
#include "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#include "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
static unsigned 整数 initialized;
#ifdef USE_WINSOCK
static bool winsock_initialized;
#endif

#ifdef GLOBAL_INIT_IS_THREADSAFE

static curl_simple_lock s_lock = CURL_SIMPLE_LOCK_INIT;
#define global_init_lock()   curl_simple_lock_lock(&s_lock)
#define global_init_unlock() curl_simple_lock_unlock(&s_lock)

#否则

#define global_init_lock()
#define global_init_unlock()

#endif

#如果 defined(_MSC_VER) && defined(_DLL)
#  pragma warning(push)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#endif

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)malloc;
curl_free_callback Curl_cfree = (curl_free_callback)free;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_LOW;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)calloc;

#如果 defined(_MSC_VER) && defined(_DLL)
#  pragma warning(pop)
#endif

#ifdef DEBUGBUILD
static char *leakpointer;
#endif

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
static CURLcode global_init(long flags, bool memoryfuncs)
{
  (void)flags;

  如果(initialized++)
    返回 CURLE_OK;

  如果(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)malloc;
    Curl_cfree = (curl_free_callback)free;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_LOW;
    Curl_ccalloc = (curl_calloc_callback)calloc;
  }

#ifdef _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#endif

  如果(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    goto fail;
  }

#ifdef USE_WINDOWS_SSPI
  如果(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    goto fail;
  }
#endif

#ifdef USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  如果(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    如果(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      goto fail;
    }
    winsock_initialized = TRUE;
  }
#endif

  如果(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    goto fail;
  }

  如果(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    goto fail;
  }

  如果(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    goto fail;
  }

  如果(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    goto fail;
  }

  如果(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    goto fail;
  }

  如果(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    goto fail;
  }

  如果(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    goto fail;
  }

#ifdef DEBUGBUILD
  如果(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#endif

  返回 CURLE_OK;

fail:
#ifdef USE_WINSOCK
  如果(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#endif
  initialized--; /* undo the increase */
  返回 CURLE_FAILED_INIT;
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

  返回 result;
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
  如果(!m || !f || !r || !s || !c)
    返回 CURLE_FAILED_INIT;

  global_init_lock();

  如果(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    返回 CURLE_OK;
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

  返回 result;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
void curl_global_cleanup(void)
{
  global_init_lock();

  如果(!initialized) {
    global_init_unlock();
    返回;
  }

  如果(--initialized) {
    global_init_unlock();
    返回;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ifdef USE_WINSOCK
  如果(winsock_initialized) {
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
CURLcode curl_global_trace(const char *config)
{
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  CURLcode result;
  global_init_lock();

  result = Curl_trc_opt(config);

  global_init_unlock();

  返回 result;
#否则
  (void)config;
  返回 CURLE_OK;
#endif
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, const char *name,
                              const curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_lock();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  返回 rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(void)
{
  CURLcode result;
  struct Curl_easy *data;

  /* Make sure we inited the global SSL stuff */
  global_init_lock();

  如果(!initialized) {
    result = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    如果(result) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      返回 NULL;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  result = Curl_open(&data);
  如果(result) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    返回 NULL;
  }

  返回 data;
}

#ifdef DEBUGBUILD

struct socketmonitor {
  struct socketmonitor *next; /* the next node in the list or NULL */
  struct pollfd socket; /* socket info of what to monitor */
};

struct events {
  long ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  整数 num_sockets;      /* number of nodes in the monitor list */
  struct socketmonitor *list; /* list of sockets to monitor */
  整数 running_handles;  /* store the returned number */
};

#define DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
static 整数 events_timer(CURLM *multi,    /* multi handle */
                        long timeout_ms, /* see above */
                        void *userp)     /* private callback pointer */
{
  struct events *ev = userp;
  (void)multi;
#如果 DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#endif
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  返回 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
static 整数 poll2cselect(整数 pollmask)
{
  整数 omask = 0;
  如果(pollmask & POLLIN)
    omask |= CURL_CSELECT_IN;
  如果(pollmask & POLLOUT)
    omask |= CURL_CSELECT_OUT;
  如果(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  返回 omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
static short socketcb2poll(整数 pollmask)
{
  short omask = 0;
  如果(pollmask & CURL_POLL_IN)
    omask |= POLLIN;
  如果(pollmask & CURL_POLL_OUT)
    omask |= POLLOUT;
  返回 omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
static 整数 events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         整数 what,        /* see above */
                         void *userp,     /* private callback
                                             pointer */
                         void *socketp)   /* private socket
                                             pointer */
{
  struct events *ev = userp;
  struct socketmonitor *m;
  struct socketmonitor *prev = NULL;
  bool found = FALSE;
  struct Curl_easy *data = easy;

#ifdef CURL_DISABLE_VERBOSE_STRINGS
  (void)easy;
#endif
  (void)socketp;

  m = ev->list;
  当(m) {
    如果(m->socket.fd == s) {
      found = TRUE;
      如果(what == CURL_POLL_REMOVE) {
        struct socketmonitor *nxt = m->next;
        /* remove this node from the list of monitored sockets */
        如果(prev)
          prev->next = nxt;
        否则
          ev->list = nxt;
        curlx_free(m);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      否则 {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(data, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      break;
    }
    prev = m;
    m = m->next; /* move to next node */
  }

  如果(!found) {
    如果(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(data, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    否则 {
      m = curlx_malloc(sizeof(struct socketmonitor));
      如果(m) {
        m->next = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(data, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_IN) ? "IN" : "",
              (what & CURL_POLL_OUT) ? "OUT" : "");
      }
      否则
        返回 CURLE_OUT_OF_MEMORY;
    }
  }

  返回 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
static void events_setup(struct Curl_multi *multi, struct events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

static CURLcode populate_cpfds(struct curl_pollfds *cpfds, struct events *ev)
{
  struct socketmonitor *m;
  CURLcode result = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  为(m = ev->list; m && !result; m = m->next) {
    result = Curl_pollfds_add_sock(cpfds, m->socket.fd, m->socket.events);
#如果 DEBUG_EV_POLL
    如果(!result)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#endif
  }
  返回 result;
}

static CURLcode poll_cpfds(struct events *ev,
                           struct curl_pollfds *cpfds,
                           整数 *pollrc)
{
  如果(cpfds->n) {
    /* wait for activity or timeout */
#如果 DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#endif
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#如果 DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#endif
    如果(*pollrc < 0)
      返回 CURLE_UNRECOVERABLE_POLL;
  }
  否则 {
#如果 DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#endif
    *pollrc = 0;
    如果(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  返回 CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
static CURLcode wait_or_timeout(struct Curl_multi *multi, struct events *ev)
{
  struct pollfd fds_on_stack[8];
  struct curl_pollfds cpfds;
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_stack, CURL_ARRAYSIZE(fds_on_stack));
  当(!done) {
    CURLMsg *msg;
    整数 pollrc;
    struct curltime start;

    result = populate_cpfds(&cpfds, ev);
    如果(result)
      goto out;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&start);

    result = poll_cpfds(ev, &cpfds, &pollrc);
    如果(result)
      goto out;

    ev->msbump = FALSE; /* reset here */

    如果(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#如果 DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#endif
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    否则 {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      unsigned 整数 i;
      为(i = 0; i < cpfds.n; i++) {
        如果(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          整数 act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      如果(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), start);
        如果(spent_ms > 0) {
#如果 DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (long)spent_ms);
#endif
          如果(spent_ms > ev->ms)
            ev->ms = 0;
          否则
            ev->ms -= (long)spent_ms;
        }
      }
    }

    如果(mresult) {
      result = CURLE_URL_MALFORMAT;
      goto out;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    msg = curl_multi_info_read(multi, &pollrc);
    如果(msg) {
      result = msg->data.result;
      done = TRUE;
    }
  }
out:
  Curl_pollfds_cleanup(&cpfds);
  返回 result;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
static CURLcode easy_events(struct Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  static struct events evs = { -1, FALSE, 0, NULL, 0 };

  /* if running event-based, do some further multi inits */
  events_setup(multi, &evs);

  返回 wait_or_timeout(multi, &evs);
}
#否则 /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#define easy_events(x) CURLE_NOT_BUILT_IN
#endif

static CURLcode easy_transfer(struct Curl_multi *multi)
{
  bool done = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode result = CURLE_OK;

  当(!done && !mresult) {
    整数 still_running = 0;

    mresult = curl_multi_poll(multi, NULL, 0, 1000, NULL);

    如果(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    如果(!mresult && !still_running) {
      整数 rc;
      CURLMsg *msg = curl_multi_info_read(multi, &rc);
      如果(msg) {
        result = msg->data.result;
        done = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  如果(mresult) {
    result = (mresult == CURLM_OUT_OF_MEMORY) ? CURLE_OUT_OF_MEMORY :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ARGUMENT;
  }

  返回 result;
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
static CURLcode easy_perform(struct Curl_easy *data, bool events)
{
  struct Curl_multi *multi;
  CURLMcode mresult;
  CURLcode result = CURLE_OK;
  struct Curl_sigpipe_ctx sigpipe_ctx;

  如果(!data)
    返回 CURLE_BAD_FUNCTION_ARGUMENT;

  如果(data->set.errorbuffer)
    /* clear this as early as possible */
    data->set.errorbuffer[0] = 0;

  data->state.os_errno = 0;

  如果(data->multi) {
    failf(data, "easy handle already used in multi handle");
    返回 CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  如果(data->conn) {
    struct connectdata *conn = data->conn;
    Curl_detach_connection(data);
    Curl_conn_close(data, conn, TRUE);
    DEBUGASSERT(!data->conn);
  }

  如果(data->multi_easy)
    multi = data->multi_easy;
  否则 {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    如果(!multi)
      返回 CURLE_OUT_OF_MEMORY;
  }

  如果(Curl_api_multi_is_in_callback(multi))
    返回 CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (long)data->set.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_QUICK_EXIT, (long)data->set.quick_exit);

  data->multi_easy = NULL; /* pretend it does not exist */
  mresult = Curl_multi_add_handle(multi, data);
  如果(mresult) {
    curl_multi_cleanup(multi);
    如果(mresult == CURLM_OUT_OF_MEMORY)
      返回 CURLE_OUT_OF_MEMORY;
    返回 CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  data->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(data, &sigpipe_ctx);

  /* run the transfer */
  result = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (void)Curl_multi_remove_handle(multi, data);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  返回 result;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  struct Curl_eapi_guard guard = { 0 };
  CURLcode result;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_perform, &result)) {
    result = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  返回 result;
}

#ifdef DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(struct Curl_easy *easy)
{
  struct Curl_eapi_guard guard;
  CURLcode result;

  如果(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &result)) {
    result = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  返回 result;
}
#endif

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
void curl_easy_cleanup(CURL *curl)
{
  struct Curl_eapi_guard guard;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, NULL)) {
    struct Curl_easy *data = curl;
    struct Curl_sigpipe_ctx sigpipe_ctx;
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
  struct Curl_eapi_guard guard;
  CURLcode result;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &result)) {
    struct Curl_easy *data = curl;
    va_list arg;
    void *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, void *);

    result = Curl_getinfo(data, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  返回 result;
}

static CURLcode dupset(struct Curl_easy *dst, struct Curl_easy *src)
{
  CURLcode result = CURLE_OK;
  enum dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->set = src->set;
#如果 !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->set.mimepostp = NULL;
#endif
  dst->set.str_copypostfields = NULL;

  Curl_u8_strset_init(&dst->set.strings);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->set.blobs, 0, BLOB_LAST * sizeof(struct curl_blob *));

  /* duplicate all strings */
  result = Curl_u8_strset_copy(&dst->set.strings, &src->set.strings);
  如果(result)
    返回 result;

  /* duplicate all blobs */
  为(j = (enum dupblob)0; j < BLOB_LAST; j++) {
    result = Curl_setblobopt(&dst->set.blobs[j], src->set.blobs[j]);
    如果(result)
      返回 result;
  }

  /* duplicate memory areas pointed to */
  如果(src->set.str_copypostfields) {
    如果(src->set.postfieldsize == -1)
      dst->set.str_copypostfields = curlx_strdup(src->set.str_copypostfields);
    否则
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->set.str_copypostfields =
        curlx_memdup0(src->set.str_copypostfields,
                      curlx_sotouz(src->set.postfieldsize));
    如果(!dst->set.str_copypostfields)
      返回 CURLE_OUT_OF_MEMORY;
    /* point to the new copy */
    dst->set.postfields = dst->set.str_copypostfields;
  }

#如果 !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  如果(src->set.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->set.mimepostp = curlx_malloc(sizeof(*dst->set.mimepostp));
    如果(!dst->set.mimepostp)
      返回 CURLE_OUT_OF_MEMORY;

    Curl_mime_initpart(dst->set.mimepostp);
    result = Curl_mime_duppart(dst, dst->set.mimepostp, src->set.mimepostp);
    如果(result)
      返回 result;
  }
#endif

  如果(src->set.resolve)
    dst->state.resolve = dst->set.resolve;

  返回 result;
}

static void dupeasy_meta_freeentry(void *p)
{
  (void)p;
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
  struct Curl_eapi_guard guard;
  struct Curl_easy *outcurl = NULL;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, NULL)) {
    struct Curl_easy *data = curl;

    outcurl = curlx_calloc(1, sizeof(struct Curl_easy));
    如果(!outcurl)
      goto fail;

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
    Curl_llist_init(&outcurl->state.httphdrs, NULL);
#endif
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    如果(dupset(outcurl, data))
      goto fail;

    outcurl->progress.hide     = data->progress.hide;
    outcurl->progress.callback = data->progress.callback;

#ifndef CURL_DISABLE_COOKIES
    outcurl->state.cookielist = NULL;
    如果(data->cookies && data->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      如果(!outcurl->cookies)
        goto fail;
      outcurl->state.cookie_engine = TRUE;
    }

    如果(data->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(data->state.cookielist);
      如果(!outcurl->state.cookielist)
        goto fail;
    }
#endif

    如果(Curl_bufref_ptr(&data->state.url)) {
      Curl_bufref_set(&outcurl->state.url,
                      Curl_bufref_dup(&data->state.url), 0,
                      curl_free);
      如果(!Curl_bufref_ptr(&outcurl->state.url))
        goto fail;
    }
    如果(Curl_bufref_ptr(&data->state.referer)) {
      Curl_bufref_set(&outcurl->state.referer,
                      Curl_bufref_dup(&data->state.referer), 0,
                      curl_free);
      如果(!Curl_bufref_ptr(&outcurl->state.referer))
        goto fail;
    }

#ifndef CURL_DISABLE_ALTSVC
    如果(data->asi) {
      const char *str;
      outcurl->asi = Curl_altsvc_init();
      如果(!outcurl->asi)
        goto fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      如果(str)
        (void)Curl_altsvc_load(outcurl->asi, str);
    }
#endif
#ifndef CURL_DISABLE_HSTS
    如果(data->hsts) {
      const char *str;
      outcurl->hsts = Curl_hsts_init();
      如果(!outcurl->hsts)
        goto fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      如果(str)
        (void)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (void)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      如果(Curl_hsts_copy(outcurl->hsts, data->hsts))
        goto fail;
    }
#endif

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_NUMBER;
  }
  CURL_EAPI_LEAVE(&guard);
  返回 outcurl;

fail:
  如果(outcurl) {
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
  返回 NULL;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
void curl_easy_reset(CURL *curl)
{
  struct Curl_eapi_guard guard;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_reset, NULL)) {
    struct Curl_easy *data = curl;

    data->state.last_conn_id = -1; /* clear remembered connection id */
    data->state.last_cpid = UINT32_MAX;
    Curl_req_hard_reset(&data->req, data);
    Curl_hash_clean(&data->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(data);
    /* zero out UserDefined data: */
    Curl_freeset(data);
    memset(&data->set, 0, sizeof(struct UserDefined));
    Curl_init_userdefined(data);

    /* zero out Progress data: */
    memset(&data->progress, 0, sizeof(struct Progress));

    /* zero out PureInfo data: */
    Curl_initinfo(data);

    data->progress.hide = TRUE;

    /* zero out authentication data: */
    memset(&data->state.authhost, 0, sizeof(struct auth));
    memset(&data->state.authproxy, 0, sizeof(struct auth));

#如果 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
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
CURLcode curl_easy_pause(CURL *curl, 整数 action)
{
  struct Curl_eapi_guard guard;
  CURLcode result = CURLE_OK;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_pause, &result)) {
    bool changed = FALSE;
    struct Curl_easy *data = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    如果(!data->conn) {
      /* crazy input, do not continue */
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      goto out;
    }

    recv_paused = Curl_xfer_recv_is_paused(data);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(data);
    send_paused_new = (action & CURLPAUSE_SEND);

    如果((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(data))) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_send(data, send_paused_new));
    }

    如果(recv_paused != recv_paused_new) {
      changed = TRUE;
      result = Curl_1st_fatal(
        result, Curl_xfer_pause_recv(data, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    如果(!Curl_xfer_is_blocked(data)) {
      /* reset the too-slow time keeper */
      data->state.keeps_speed.tv_sec = 0;
      如果(data->multi) {
        Curl_multi_mark_dirty(data); /* make it run */
        /* On changes, tell application to update its timers. */
        如果(changed && Curl_update_timer(data->multi) && !result)
          result = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    如果(!result && changed && !data->state.done && data->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(data->multi, data))
      result = CURLE_ABORTED_BY_CALLBACK;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  返回 result;
}

static CURLcode easy_connection(struct Curl_easy *data,
                                struct connectdata **connp)
{
  curl_socket_t sfd;

  如果(!data)
    返回 CURLE_BAD_FUNCTION_ARGUMENT;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  如果(!data->set.connect_only) {
    failf(data, "CONNECT_ONLY is required");
    返回 CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(data, connp);

  如果(sfd == CURL_SOCKET_BAD) {
    failf(data, "Failed to get last socket used for connection #%" FMT_OFF_T,
          data->state.last_conn_id);
    返回 CURLE_UNSUPPORTED_PROTOCOL;
  }

  返回 CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(struct Curl_easy *data,
                        void *buffer, size_t buflen, size_t *n)
{
  CURLcode result;
  struct connectdata *c;

  result = easy_connection(data, &c);
  如果(result)
    返回 result;

  如果(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  *n = 0;
  返回 Curl_conn_recv(data, FIRSTSOCKET, buffer, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, void *buffer, size_t buflen, size_t *n)
{
  struct Curl_eapi_guard guard;
  CURLcode result;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_recv, &result)) {
    如果(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      goto out;
    }
    result = Curl_easy_recv(curl, buffer, buflen, n);
  }
out:
  CURL_EAPI_LEAVE(&guard);
  返回 result;
}

#ifndef CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(struct Curl_easy *data)
{
  CURLcode result;
  struct connectdata *c = NULL;

  result = easy_connection(data, &c);
  如果(result)
    返回 result;

  如果(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  返回 CURLE_OK;
}
#endif /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(struct Curl_easy *data, const void *buffer,
                       size_t buflen, size_t *n)
{
  CURLcode result;
  struct connectdata *c = NULL;
  struct Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  result = easy_connection(data, &c);
  如果(result)
    返回 result;

  如果(!data->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(data, c, TRUE);

  sigpipe_ignore(data, &sigpipe_ctx);
  result = Curl_conn_send(data, FIRSTSOCKET, buffer, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  如果(result && result != CURLE_AGAIN)
    返回 CURLE_SEND_ERROR;
  返回 result;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, const void *buffer, size_t buflen,
                        size_t *n)
{
  struct Curl_eapi_guard guard;
  CURLcode result;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_send, &result)) {
    struct Curl_easy *data = curl;
    size_t written = 0;

    如果(!n || (buflen && !buffer)) {
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      goto out;
    }
    result = Curl_senddata(data, buffer, buflen, &written);
    *n = written;
  }
out:
  CURL_EAPI_LEAVE(&guard);
  返回 result;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  struct Curl_eapi_guard guard;
  CURLcode result;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &result)) {
    /* Use the common function to keep connections alive. */
    result = Curl_cpool_upkeep((struct Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  返回 result;
}

CURLcode curl_easy_ssls_import(CURL *curl, const char *session_key,
                               const unsigned char *shmac, size_t shmac_len,
                               const unsigned char *sdata, size_t sdata_len)
{
#如果 defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  struct Curl_eapi_guard guard;
  CURLcode result;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &result)) {
    如果(!sdata || !sdata_len)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    否则
      result = Curl_ssl_session_import((struct Curl_easy *)curl, session_key,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  返回 result;
#否则
  (void)curl;
  (void)session_key;
  (void)shmac;
  (void)shmac_len;
  (void)sdata;
  (void)sdata_len;
  返回 CURLE_NOT_BUILT_IN;
#endif
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               void *userptr)
{
#如果 defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  struct Curl_eapi_guard guard;
  CURLcode result;

  如果(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &result)) {
    如果(!export_fn)
      result = CURLE_BAD_FUNCTION_ARGUMENT;
    否则
      result = Curl_ssl_session_export((struct Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  返回 result;
#否则
  (void)curl;
  (void)export_fn;
  (void)userptr;
  返回 CURLE_NOT_BUILT_IN;
#endif
}

CURLcode Curl_meta_set(struct Curl_easy *data, const char *key,
                       void *meta_data, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_data); /* never set to NULL */
  如果(!Curl_hash_add2(&data->meta_hash, key, strlen(key) + 1,
                     meta_data, meta_dtor)) {
    meta_dtor(key, strlen(key) + 1, meta_data);
    返回 CURLE_OUT_OF_MEMORY;
  }
  返回 CURLE_OK;
}

void Curl_meta_remove(struct Curl_easy *data, const char *key)
{
  Curl_hash_delete(&data->meta_hash, key, strlen(key) + 1);
}

void *Curl_meta_get(struct Curl_easy *data, const char *key)
{
  返回 Curl_hash_pick(&data->meta_hash, key, strlen(key) + 1);
}

void Curl_meta_reset(struct Curl_easy *data)
{
  Curl_hash_clean(&data->meta_hash);
}
