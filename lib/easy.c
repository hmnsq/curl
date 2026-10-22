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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "api.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vquic/vquic.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "getinfo.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "easyif.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "multi_ev.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "connect.h" /* for Curl_getconnectinfo */
#ಅನ್ತರ್ಭಾವ "slist.h"
#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "amigaos.h"
#ಅನ್ತರ್ಭಾವ "macos.h"
#ಅನ್ತರ್ಭಾವ "curlx/wait.h"
#ಅನ್ತರ್ಭಾವ "sigpipe.h"
#ಅನ್ತರ್ಭಾವ "vssh/ssh.h"
#ಅನ್ತರ್ಭಾವ "setopt.h"
#ಅನ್ತರ್ಭಾವ "http_digest.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "altsvc.h"
#ಅನ್ತರ್ಭಾವ "hsts.h"
#ಅನ್ತರ್ಭಾವ "curl_sspi.h" /* Curl_sspi_global_init()/Curl_sspi_global_cleanup() */
#ಅನ್ತರ್ಭಾವ "curlx/timeval.h" /* for curlx_now_init() */
#ಅನ್ತರ್ಭಾವ "curlx/version_win32.h" /* for curlx_verify_windows_init() */

#ಅನ್ತರ್ಭಾವ "easy_lock.h"

/* true globals -- for curl_global_init() and curl_global_cleanup() */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ initialized;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
ಸ್ಥಿರ bool winsock_initialized;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GLOBAL_INIT_IS_THREADSAFE

ಸ್ಥಿರ curl_simple_ಬೀಗ s_ಬೀಗ = CURL_SIMPLE_ಬೀಗ_INIT;
#ವ್ಯಾಖ್ಯಾನಿಸು global_init_ಬೀಗ()   curl_simple_ಬೀಗ_ಬೀಗ(&s_ಬೀಗ)
#ವ್ಯಾಖ್ಯಾನಿಸು global_init_unlock() curl_simple_ಬೀಗ_unlock(&s_ಬೀಗ)

#ಇಲ್ಲದಿದ್ದರೆ

#ವ್ಯಾಖ್ಯಾನಿಸು global_init_ಬೀಗ()
#ವ್ಯಾಖ್ಯಾನಿಸು global_init_unlock()

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ತಳ್ಳು)
#  pragma warning(disable:4232) /* MSVC extension, dllimport identity */
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If a memory-using function (like curl_getenv) is used before
 * curl_global_init() is called, we need to have these pointers set already.
 */
curl_malloc_callback Curl_cmalloc = (curl_malloc_callback)ಸ್ಮರಣೆ_ಕೊಡು;
curl_free_callback Curl_cfree = (curl_free_callback)ಬಿಡುಗಡೆ;
curl_realloc_callback Curl_crealloc = (curl_realloc_callback)realloc;
curl_strdup_callback Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_ಕೆಳಗಿನ;
curl_calloc_callback Curl_ccalloc = (curl_calloc_callback)ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು;

#ಆದರೆ defined(_MSC_VER) && defined(_DLL)
#  pragma warning(ತೆಗೆ)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
ಸ್ಥಿರ ಅಕ್ಷರ *leakpointer;
#ಆದರೆ_ಅಂತ್ಯ

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
ಸ್ಥಿರ CURLcode global_init(ದೀರ್ಘ flags, bool memoryfuncs)
{
  (ಶೂನ್ಯ)flags;

  ಆದರೆ(initialized++)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(memoryfuncs) {
    /* Setup the default memory functions here (again) */
    Curl_cmalloc = (curl_malloc_callback)ಸ್ಮರಣೆ_ಕೊಡು;
    Curl_cfree = (curl_free_callback)ಬಿಡುಗಡೆ;
    Curl_crealloc = (curl_realloc_callback)realloc;
    Curl_cstrdup = (curl_strdup_callback)CURLX_STRDUP_ಕೆಳಗಿನ;
    Curl_ccalloc = (curl_calloc_callback)ಸೊನ್ನೆ_ಸ್ಮರಣೆ_ಕೊಡು;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  curlx_verify_windows_init();
  curlx_now_init();
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(Curl_trc_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_trc_init failed\n"));
    ಹೋಗು fail;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  ಆದರೆ(Curl_sspi_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_sspi_global_init() failed\n"));
    ಹೋಗು fail;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  /* CURL_GLOBAL_WINSOCK controls the *optional* part of the Windows
     initialization which is for the Windows sockets library (Winsock). */
  ಆದರೆ(flags & CURL_GLOBAL_WINSOCK) {
    WSADATA wsa;
    ಆದರೆ(WSAStartup(MAKEWORD(2, 2), &wsa)) {
      DEBUGF(curl_mfprintf(stderr, "Error: WSAStartup() failed\n"));
      ಹೋಗು fail;
    }
    winsock_initialized = TRUE;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!Curl_ssl_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssl_init failed\n"));
    ಹೋಗು fail;
  }

  ಆದರೆ(Curl_hash_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_hash_global_init failed\n"));
    ಹೋಗು fail;
  }

  ಆದರೆ(!Curl_vquic_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_vquic_init failed\n"));
    ಹೋಗು fail;
  }

  ಆದರೆ(Curl_amiga_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_amiga_init failed\n"));
    ಹೋಗು fail;
  }

  ಆದರೆ(Curl_macos_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_macos_init failed\n"));
    ಹೋಗು fail;
  }

  ಆದರೆ(Curl_async_global_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_async_global_init failed\n"));
    ಹೋಗು fail;
  }

  ಆದರೆ(Curl_ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_ssh_init failed\n"));
    ಹೋಗು fail;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಆದರೆ(getenv("CURL_GLOBAL_INIT"))
    /* alloc data that will leak if *cleanup() is not called! */
    leakpointer = curlx_malloc(1);
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ CURLE_OK;

fail:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  ಆದರೆ(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  initialized--; /* undo the increase */
  ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
}

/**
 * curl_global_init() globally initializes curl given a bitwise set of the
 * different features of what to initialize.
 */
CURLcode curl_global_init(ದೀರ್ಘ flags)
{
  CURLcode ಫಲಿತಾಂಶ;
  global_init_ಬೀಗ();

  ಫಲಿತಾಂಶ = global_init(flags, TRUE);

  global_init_unlock();

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * curl_global_init_mem() globally initializes curl and also registers the
 * user provided callback routines.
 */
CURLcode curl_global_init_mem(ದೀರ್ಘ flags, curl_malloc_callback m,
                              curl_free_callback f, curl_realloc_callback r,
                              curl_strdup_callback s, curl_calloc_callback c)
{
  CURLcode ಫಲಿತಾಂಶ;

  /* Invalid input, return immediately */
  ಆದರೆ(!m || !f || !r || !s || !c)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  global_init_ಬೀಗ();

  ಆದರೆ(initialized) {
    /* Already initialized, do not do it again, but bump the variable anyway to
       work like curl_global_init() and require the same amount of cleanup
       calls. */
    initialized++;
    global_init_unlock();
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /* set memory functions before global_init() in case it wants memory
     functions */
  Curl_cmalloc = m;
  Curl_cfree = f;
  Curl_cstrdup = s;
  Curl_crealloc = r;
  Curl_ccalloc = c;

  /* Call the actual init function, but without setting */
  ಫಲಿತಾಂಶ = global_init(flags, FALSE);

  global_init_unlock();

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/**
 * curl_global_cleanup() globally cleanups curl.
 */
ಶೂನ್ಯ curl_global_cleanup(ಶೂನ್ಯ)
{
  global_init_ಬೀಗ();

  ಆದರೆ(!initialized) {
    global_init_unlock();
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(--initialized) {
    global_init_unlock();
    ಹಿಂತಿರುಗಿ;
  }

  Curl_ssh_cleanup();
  Curl_ssl_cleanup();
  Curl_vquic_cleanup();
  Curl_async_global_cleanup();
  Curl_amiga_cleanup();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  ಆದರೆ(winsock_initialized) {
    WSACleanup();
    winsock_initialized = FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
  Curl_sspi_global_cleanup();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  curlx_free(leakpointer);
#ಆದರೆ_ಅಂತ್ಯ

  global_init_unlock();
}

/**
 * curl_global_trace() globally initializes curl logging.
 */
CURLcode curl_global_trace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_VERBOSE_ಅಕ್ಷರಸರಣಿಗಳು
  CURLcode ಫಲಿತಾಂಶ;
  global_init_ಬೀಗ();

  ಫಲಿತಾಂಶ = Curl_trc_opt(config);

  global_init_unlock();

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)config;
  ಹಿಂತಿರುಗಿ CURLE_OK;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * curl_global_sslset() globally initializes the SSL backend to use.
 */
CURLsslset curl_global_sslset(curl_sslbackend id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                              ಸ್ಥಿರಾಂಕ curl_ssl_backend ***avail)
{
  CURLsslset rc;

  global_init_ಬೀಗ();

  rc = Curl_init_sslset_nolock(id, name, avail);

  global_init_unlock();

  ಹಿಂತಿರುಗಿ rc;
}

/*
 * curl_easy_init() is the external interface to alloc, setup and init an
 * easy handle that is returned. If anything goes wrong, NULL is returned.
 */
CURL *curl_easy_init(ಶೂನ್ಯ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;

  /* Make sure we inited the global SSL stuff */
  global_init_ಬೀಗ();

  ಆದರೆ(!initialized) {
    ಫಲಿತಾಂಶ = global_init(CURL_GLOBAL_DEFAULT, TRUE);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      /* something in the global init failed, return nothing */
      DEBUGF(curl_mfprintf(stderr, "Error: curl_global_init failed\n"));
      global_init_unlock();
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
  }
  global_init_unlock();

  /* We use Curl_open() with undefined URL so far */
  ಫಲಿತಾಂಶ = Curl_open(&ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    DEBUGF(curl_mfprintf(stderr, "Error: Curl_open failed\n"));
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }

  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD

ರಚನೆ socketmonitor {
  ರಚನೆ socketmonitor *ಮುಂದಿನ; /* the next node in the list or NULL */
  ರಚನೆ pollfd socket; /* socket info of what to monitor */
};

ರಚನೆ events {
  ದೀರ್ಘ ms;              /* timeout, run the timeout function when reached */
  bool msbump;          /* set TRUE when timeout is set by callback */
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ_sockets;      /* number of nodes in the monitor list */
  ರಚನೆ socketmonitor *list; /* list of sockets to monitor */
  ಪೂರ್ಣಾಂಕ running_handles;  /* store the returned number */
};

#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_EV_POLL   0

/* events_timer
 *
 * Callback that gets called with a new value when the timeout should be
 * updated.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ events_timer(CURLM *multi,    /* multi handle */
                        ದೀರ್ಘ timeout_ms, /* see above */
                        ಶೂನ್ಯ *userp)     /* private callback pointer */
{
  ರಚನೆ events *ev = userp;
  (ಶೂನ್ಯ)multi;
#ಆದರೆ DEBUG_EV_POLL
  curl_mfprintf(stderr, "events_timer: set timeout %ldms\n", timeout_ms);
#ಆದರೆ_ಅಂತ್ಯ
  ev->ms = timeout_ms;
  ev->msbump = TRUE;
  ಹಿಂತಿರುಗಿ 0;
}

/* poll2cselect
 *
 * convert from poll() bit definitions to libcurl's CURL_CSELECT_* ones
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ poll2cselect(ಪೂರ್ಣಾಂಕ pollmask)
{
  ಪೂರ್ಣಾಂಕ omask = 0;
  ಆದರೆ(pollmask & POLLIN)
    omask |= CURL_CSELECT_ಒಳ;
  ಆದರೆ(pollmask & POLLOUT)
    omask |= CURL_CSELECT_ಹೊರಹೋಗು;
  ಆದರೆ(pollmask & POLLERR)
    omask |= CURL_CSELECT_ERR;
  ಹಿಂತಿರುಗಿ omask;
}

/* socketcb2poll
 *
 * convert from libcurl' CURL_POLL_* bit definitions to poll()'s
 */
ಸ್ಥಿರ ಗಿಡ್ಡ socketcb2poll(ಪೂರ್ಣಾಂಕ pollmask)
{
  ಗಿಡ್ಡ omask = 0;
  ಆದರೆ(pollmask & CURL_POLL_ಒಳ)
    omask |= POLLIN;
  ಆದರೆ(pollmask & CURL_POLL_ಹೊರಹೋಗು)
    omask |= POLLOUT;
  ಹಿಂತಿರುಗಿ omask;
}

/* events_socket
 *
 * Callback that gets called with information about socket activity to
 * monitor.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ events_socket(CURL *easy,      /* easy handle */
                         curl_socket_t s, /* socket */
                         ಪೂರ್ಣಾಂಕ what,        /* see above */
                         ಶೂನ್ಯ *userp,     /* private callback
ಸೂಚಕ*/
                         ಶೂನ್ಯ *socketp)   /* private socket
ಸೂಚಕ*/
{
  ರಚನೆ events *ev = userp;
  ರಚನೆ socketmonitor *m;
  ರಚನೆ socketmonitor *prev = ಖಾಲಿ;
  bool ಸಿಕ್ಕಿತು = FALSE;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = easy;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_VERBOSE_ಅಕ್ಷರಸರಣಿಗಳು
  (ಶೂನ್ಯ)easy;
#ಆದರೆ_ಅಂತ್ಯ
  (ಶೂನ್ಯ)socketp;

  m = ev->list;
  ಆಗ(m) {
    ಆದರೆ(m->socket.fd == s) {
      ಸಿಕ್ಕಿತು = TRUE;
      ಆದರೆ(what == CURL_POLL_REMOVE) {
        ರಚನೆ socketmonitor *nxt = m->ಮುಂದಿನ;
        /* remove this node from the list of monitored sockets */
        ಆದರೆ(prev)
          prev->ಮುಂದಿನ = nxt;
        ಇಲ್ಲದಿದ್ದರೆ
          ev->list = nxt;
        curlx_free(m);
        infof(ದತ್ತಾಂಶ, "socket cb: socket %" FMT_SOCKET_T " REMOVED", s);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* The socket 's' is already being monitored, update the activity
           mask. Convert from libcurl bitmask to the poll one. */
        m->socket.events = socketcb2poll(what);
        infof(ದತ್ತಾಂಶ, "socket cb: socket %" FMT_SOCKET_T " UPDATED as %s%s", s,
              (what & CURL_POLL_ಒಳ) ? "IN" : "",
              (what & CURL_POLL_ಹೊರಹೋಗು) ? "OUT" : "");
      }
      ನಿಲ್ಲಿಸು;
    }
    prev = m;
    m = m->ಮುಂದಿನ; /* move to next node */
  }

  ಆದರೆ(!ಸಿಕ್ಕಿತು) {
    ಆದರೆ(what == CURL_POLL_REMOVE) {
      /* should not happen if our logic is correct, but is no drama. */
      DEBUGF(infof(ದತ್ತಾಂಶ, "socket cb: asked to REMOVE socket %"
                   FMT_SOCKET_T "but not present!", s));
      DEBUGASSERT(0);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      m = curlx_malloc(ಗಾತ್ರ(ರಚನೆ socketmonitor));
      ಆದರೆ(m) {
        m->ಮುಂದಿನ = ev->list;
        m->socket.fd = s;
        m->socket.events = socketcb2poll(what);
        m->socket.revents = 0;
        ev->list = m;
        infof(ದತ್ತಾಂಶ, "socket cb: socket %" FMT_SOCKET_T " ADDED as %s%s", s,
              (what & CURL_POLL_ಒಳ) ? "IN" : "",
              (what & CURL_POLL_ಹೊರಹೋಗು) ? "OUT" : "");
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  ಹಿಂತಿರುಗಿ 0;
}

/*
 * events_setup()
 *
 * Do the multi handle setups that only event-based transfers need.
 */
ಸ್ಥಿರ ಶೂನ್ಯ events_ಸಿದ್ಧತೆ(ರಚನೆ Curl_multi *multi, ರಚನೆ events *ev)
{
  /* timer callback */
  curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, events_timer);
  curl_multi_setopt(multi, CURLMOPT_TIMERDATA, ev);

  /* socket callback */
  curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, events_socket);
  curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, ev);
}

ಸ್ಥಿರ CURLcode populate_cpfds(ರಚನೆ curl_pollfds *cpfds, ರಚನೆ events *ev)
{
  ರಚನೆ socketmonitor *m;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  Curl_pollfds_reset(cpfds);
  ಗಾಗಿ(m = ev->list; m && !ಫಲಿತಾಂಶ; m = m->ಮುಂದಿನ) {
    ಫಲಿತಾಂಶ = Curl_pollfds_ಕೂಡಿಸು_sock(cpfds, m->socket.fd, m->socket.events);
#ಆದರೆ DEBUG_EV_POLL
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      curl_mfprintf(stderr, "poll() %d check socket %d\n",
                    cpfds->n - 1, cpfds->pfds[cpfds->n - 1].fd);
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode poll_cpfds(ರಚನೆ events *ev,
                           ರಚನೆ curl_pollfds *cpfds,
                           ಪೂರ್ಣಾಂಕ *pollrc)
{
  ಆದರೆ(cpfds->n) {
    /* wait for activity or timeout */
#ಆದರೆ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms)\n",
                  cpfds->n, ev->ms);
#ಆದರೆ_ಅಂತ್ಯ
    *pollrc = Curl_poll(cpfds->pfds, cpfds->n, ev->ms);
#ಆದರೆ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll(numfds=%u, timeout=%ldms) -> %d\n",
                  cpfds->n, ev->ms, *pollrc);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(*pollrc < 0)
      ಹಿಂತಿರುಗಿ CURLE_UNRECOVERABLE_POLL;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ DEBUG_EV_POLL
    curl_mfprintf(stderr, "poll, but no fds, wait timeout=%ldms\n", ev->ms);
#ಆದರೆ_ಅಂತ್ಯ
    *pollrc = 0;
    ಆದರೆ(ev->ms > 0)
      curlx_wait_ms(ev->ms);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* wait_or_timeout()
 *
 * waits for activity on any of the given sockets, or the timeout to trigger.
 */
ಸ್ಥಿರ CURLcode wait_or_timeout(ರಚನೆ Curl_multi *multi, ರಚನೆ events *ev)
{
  ರಚನೆ pollfd fds_on_ರಾಶಿ[8];
  ರಚನೆ curl_pollfds cpfds;
  bool ಮುಗಿದ = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  Curl_pollfds_init(&cpfds, fds_on_ರಾಶಿ, CURL_ARRAYSIZE(fds_on_ರಾಶಿ));
  ಆಗ(!ಮುಗಿದ) {
    CURLMsg *ಸಂದೇಶ;
    ಪೂರ್ಣಾಂಕ pollrc;
    ರಚನೆ curltime ಆರಂಭ;

    ಫಲಿತಾಂಶ = populate_cpfds(&cpfds, ev);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    /* get the time stamp to use to figure out how long poll takes */
    curlx_pnow(&ಆರಂಭ);

    ಫಲಿತಾಂಶ = poll_cpfds(ev, &cpfds, &pollrc);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    ev->msbump = FALSE; /* reset here */

    ಆದರೆ(!pollrc) {
      /* timeout! */
      ev->ms = 0;
#ಆದರೆ DEBUG_EV_POLL
      curl_mfprintf(stderr, "call curl_multi_socket_action(TIMEOUT)\n");
#ಆದರೆ_ಅಂತ್ಯ
      mresult = curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0,
                                         &ev->running_handles);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* here pollrc is > 0 */
      /* loop over the monitored sockets to see which ones had activity */
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
      ಗಾಗಿ(i = 0; i < cpfds.n; i++) {
        ಆದರೆ(cpfds.pfds[i].revents) {
          /* socket activity, tell libcurl */
          ಪೂರ್ಣಾಂಕ act = poll2cselect(cpfds.pfds[i].revents); /* convert */

          /* sending infof "randomly" to the first easy handle */
          infof(multi->admin, "call curl_multi_socket_action(socket "
                "%" FMT_SOCKET_T ")", (curl_socket_t)cpfds.pfds[i].fd);
          mresult = curl_multi_socket_action(multi, cpfds.pfds[i].fd, act,
                                             &ev->running_handles);
        }
      }

      ಆದರೆ(!ev->msbump && ev->ms >= 0) {
        /* If nothing updated the timeout, we decrease it by the spent time.
         * If it was updated, it has the new timeout time stored already.
         */
        timediff_t spent_ms = curlx_timediff_ms(curlx_now(), ಆರಂಭ);
        ಆದರೆ(spent_ms > 0) {
#ಆದರೆ DEBUG_EV_POLL
        curl_mfprintf(stderr, "poll timeout %ldms not updated, decrease by "
                      "time spent %ldms\n", ev->ms, (ದೀರ್ಘ)spent_ms);
#ಆದರೆ_ಅಂತ್ಯ
          ಆದರೆ(spent_ms > ev->ms)
            ev->ms = 0;
          ಇಲ್ಲದಿದ್ದರೆ
            ev->ms -= (ದೀರ್ಘ)spent_ms;
        }
      }
    }

    ಆದರೆ(mresult) {
      ಫಲಿತಾಂಶ = CURLE_URL_MALFORMAT;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    /* we do not really care about the "msgs_in_queue" value returned in the
       second argument */
    ಸಂದೇಶ = curl_multi_info_read(multi, &pollrc);
    ಆದರೆ(ಸಂದೇಶ) {
      ಫಲಿತಾಂಶ = ಸಂದೇಶ->ದತ್ತಾಂಶ.ಫಲಿತಾಂಶ;
      ಮುಗಿದ = TRUE;
    }
  }
ಹೊರಹೋಗು:
  Curl_pollfds_cleanup(&cpfds);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* easy_events()
 *
 * Runs a transfer in a blocking manner using the events-based API
 */
ಸ್ಥಿರ CURLcode easy_events(ರಚನೆ Curl_multi *multi)
{
  /* this struct is made static to allow it to be used after this function
     returns and curl_multi_remove_handle() is called */
  ಸ್ಥಿರ ರಚನೆ events evs = { -1, FALSE, 0, ಖಾಲಿ, 0 };

  /* if running event-based, do some further multi inits */
  events_ಸಿದ್ಧತೆ(multi, &evs);

  ಹಿಂತಿರುಗಿ wait_or_timeout(multi, &evs);
}
#ಇಲ್ಲದಿದ್ದರೆ /* DEBUGBUILD */
/* when not built with debug, this function does not exist */
#ವ್ಯಾಖ್ಯಾನಿಸು easy_events(x) CURLE_NOT_BUILT_ಒಳ
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode easy_transfer(ರಚನೆ Curl_multi *multi)
{
  bool ಮುಗಿದ = FALSE;
  CURLMcode mresult = CURLM_OK;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಗ(!ಮುಗಿದ && !mresult) {
    ಪೂರ್ಣಾಂಕ still_running = 0;

    mresult = curl_multi_poll(multi, ಖಾಲಿ, 0, 1000, ಖಾಲಿ);

    ಆದರೆ(!mresult)
      mresult = curl_multi_perform(multi, &still_running);

    /* only read 'still_running' if curl_multi_perform() return OK */
    ಆದರೆ(!mresult && !still_running) {
      ಪೂರ್ಣಾಂಕ rc;
      CURLMsg *ಸಂದೇಶ = curl_multi_info_read(multi, &rc);
      ಆದರೆ(ಸಂದೇಶ) {
        ಫಲಿತಾಂಶ = ಸಂದೇಶ->ದತ್ತಾಂಶ.ಫಲಿತಾಂಶ;
        ಮುಗಿದ = TRUE;
      }
    }
  }

  /* Make sure to return some kind of error if there was a multi problem */
  ಆದರೆ(mresult) {
    ಫಲಿತಾಂಶ = (mresult == CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) ? CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ :
      /* The other multi errors should never happen, so return
         something suitably generic */
      CURLE_BAD_FUNCTION_ವಾದ;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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
ಸ್ಥಿರ CURLcode easy_perform(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool events)
{
  ರಚನೆ Curl_multi *multi;
  CURLMcode mresult;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ Curl_sigpipe_ctx sigpipe_ctx;

  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.errorbuffer)
    /* clear this as early as possible */
    ದತ್ತಾಂಶ->ಗಣ.errorbuffer[0] = 0;

  ದತ್ತಾಂಶ->state.os_errno = 0;

  ಆದರೆ(ದತ್ತಾಂಶ->multi) {
    failf(ದತ್ತಾಂಶ, "easy handle already used in multi handle");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  /* if the handle has a connection still attached (it is/was a connect-only
     handle) then disconnect before performing */
  ಆದರೆ(ದತ್ತಾಂಶ->conn) {
    ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
    Curl_detach_connection(ದತ್ತಾಂಶ);
    Curl_conn_close(ದತ್ತಾಂಶ, conn, TRUE);
    DEBUGASSERT(!ದತ್ತಾಂಶ->conn);
  }

  ಆದರೆ(ದತ್ತಾಂಶ->multi_easy)
    multi = ದತ್ತಾಂಶ->multi_easy;
  ಇಲ್ಲದಿದ್ದರೆ {
    /* this multi handle will only ever have a single easy handle attached to
       it, so make it use minimal hash sizes */
    multi = Curl_multi_handle(16, 1, 3, 7, 3);
    ಆದರೆ(!multi)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(Curl_api_multi_is_ಒಳ_callback(multi))
    ಹಿಂತಿರುಗಿ CURLE_RECURSIVE_API_CALL;

  /* Copy relevant easy options to the multi handle */
  curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, (ದೀರ್ಘ)ದತ್ತಾಂಶ->ಗಣ.maxconnects);
  curl_multi_setopt(multi, CURLMOPT_ಶೀಘ್ರ_EXIT, (ದೀರ್ಘ)ದತ್ತಾಂಶ->ಗಣ.quick_exit);

  ದತ್ತಾಂಶ->multi_easy = ಖಾಲಿ; /* pretend it does not exist */
  mresult = Curl_multi_ಕೂಡಿಸು_handle(multi, ದತ್ತಾಂಶ);
  ಆದರೆ(mresult) {
    curl_multi_cleanup(multi);
    ಆದರೆ(mresult == CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  /* assign this after curl_multi_add_handle() */
  ದತ್ತಾಂಶ->multi_easy = multi;

  sigpipe_init(&sigpipe_ctx);
  sigpipe_apply(ದತ್ತಾಂಶ, &sigpipe_ctx);

  /* run the transfer */
  ಫಲಿತಾಂಶ = events ? easy_events(multi) : easy_transfer(multi);

  /* ignoring the return code is not nice, but atm we cannot really handle
     a failure here, room for future improvement! */
  (ಶೂನ್ಯ)Curl_multi_remove_handle(multi, ದತ್ತಾಂಶ);

  sigpipe_restore(&sigpipe_ctx);

  /* The multi handle is kept alive, owned by the easy handle */
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * curl_easy_perform() is the external interface that performs a blocking
 * transfer as previously setup.
 */
CURLcode curl_easy_perform(CURL *curl)
{
  ರಚನೆ Curl_eapi_guard guard = { 0 };
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_perform, &ಫಲಿತಾಂಶ)) {
    ಫಲಿತಾಂಶ = easy_perform(curl, FALSE);
  }
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
/*
 * curl_easy_perform_ev() is the external interface that performs a blocking
 * transfer using the event-based API internally.
 */
CURLcode curl_easy_perform_ev(ರಚನೆ Curl_easy *easy)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, easy, easy_perform_ev, &ಫಲಿತಾಂಶ)) {
    ಫಲಿತಾಂಶ = easy_perform(easy, TRUE);
  }
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * curl_easy_cleanup() is the external interface to cleaning/freeing the given
 * easy handle.
 */
ಶೂನ್ಯ curl_easy_cleanup(CURL *curl)
{
  ರಚನೆ Curl_eapi_guard guard;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_cleanup, ಖಾಲಿ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    ರಚನೆ Curl_sigpipe_ctx sigpipe_ctx;
    sigpipe_ignore(ದತ್ತಾಂಶ, &sigpipe_ctx);
    Curl_close(&ದತ್ತಾಂಶ);
    sigpipe_restore(&sigpipe_ctx);
  }
  CURL_EAPI_LEAVE(&guard);
}

/*
 * curl_easy_getinfo() is an external interface that allows an app to retrieve
 * information from a performed transfer and similar.
 */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು curl_easy_getinfo
CURLcode curl_easy_getinfo(CURL *curl, CURLINFO info, ...)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_getinfo, &ಫಲಿತಾಂಶ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    va_list arg;
    ಶೂನ್ಯ *paramp;

    va_start(arg, info);
    paramp = va_arg(arg, ಶೂನ್ಯ *);

    ಫಲಿತಾಂಶ = Curl_getinfo(ದತ್ತಾಂಶ, info, paramp);

    va_end(arg);
  }
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode dupset(ರಚನೆ Curl_easy *dst, ರಚನೆ Curl_easy *src)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಎಣಿಕೆ dupblob j;

  /* Copy src->set into dst->set first, then deal with the strings
     afterwards */
  dst->ಗಣ = src->ಗಣ;
#ಆದರೆ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  dst->ಗಣ.mimepostp = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
  dst->ಗಣ.str_copypostfields = ಖಾಲಿ;

  Curl_u8_strset_init(&dst->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು);
  /* clear all dest string and blob pointers first, in case we error out
     mid-function */
  memset(dst->ಗಣ.blobs, 0, BLOB_ಕೊನೆಯ * ಗಾತ್ರ(ರಚನೆ curl_blob *));

  /* duplicate all strings */
  ಫಲಿತಾಂಶ = Curl_u8_strset_copy(&dst->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು, &src->ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* duplicate all blobs */
  ಗಾಗಿ(j = (ಎಣಿಕೆ dupblob)0; j < BLOB_ಕೊನೆಯ; j++) {
    ಫಲಿತಾಂಶ = Curl_setblobopt(&dst->ಗಣ.blobs[j], src->ಗಣ.blobs[j]);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* duplicate memory areas pointed to */
  ಆದರೆ(src->ಗಣ.str_copypostfields) {
    ಆದರೆ(src->ಗಣ.postfieldsize == -1)
      dst->ಗಣ.str_copypostfields = curlx_strdup(src->ಗಣ.str_copypostfields);
    ಇಲ್ಲದಿದ್ದರೆ
      /* postfieldsize is curl_off_t, curlx_memdup0() takes a size_t ... */
      dst->ಗಣ.str_copypostfields =
        curlx_memdup0(src->ಗಣ.str_copypostfields,
                      curlx_sotouz(src->ಗಣ.postfieldsize));
    ಆದರೆ(!dst->ಗಣ.str_copypostfields)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    /* point to the new copy */
    dst->ಗಣ.postfields = dst->ಗಣ.str_copypostfields;
  }

#ಆದರೆ !defined(CURL_DISABLE_MIME) || !defined(CURL_DISABLE_FORM_API)
  ಆದರೆ(src->ಗಣ.mimepostp) {
    /* Duplicate mime data. Get a mimepost struct for the clone as well */
    dst->ಗಣ.mimepostp = curlx_malloc(ಗಾತ್ರ(*dst->ಗಣ.mimepostp));
    ಆದರೆ(!dst->ಗಣ.mimepostp)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    Curl_mime_initpart(dst->ಗಣ.mimepostp);
    ಫಲಿತಾಂಶ = Curl_mime_duppart(dst, dst->ಗಣ.mimepostp, src->ಗಣ.mimepostp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(src->ಗಣ.resolve)
    dst->state.resolve = dst->ಗಣ.resolve;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ dupeasy_meta_freeentry(ಶೂನ್ಯ *p)
{
  (ಶೂನ್ಯ)p;
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
  ರಚನೆ Curl_eapi_guard guard;
  ರಚನೆ Curl_easy *outcurl = ಖಾಲಿ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_duphandle, ಖಾಲಿ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;

    outcurl = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_easy));
    ಆದರೆ(!outcurl)
      ಹೋಗು fail;

    /*
     * We setup a few buffers we need. We should probably make them
     * get setup on-demand in the code, as that would probably decrease
     * the likeliness of us forgetting to init a buffer here in the future.
     */
    outcurl->ಗಣ.ಬಫರ್_ಗಾತ್ರ = ದತ್ತಾಂಶ->ಗಣ.ಬಫರ್_ಗಾತ್ರ;

    Curl_hash_init(&outcurl->meta_hash, 23, CURL_HASH_TYPE_ಬೈಟುಗಳು,
                   dupeasy_meta_freeentry);
    curlx_dyn_init(&outcurl->state.headerb, CURL_MAX_HTTP_ಶೀರ್ಷಿಕೆ);
    Curl_bufref_init(&outcurl->state.url);
    Curl_bufref_init(&outcurl->state.referer);
    Curl_netrc_init(&outcurl->state.netrc);

    /* the connection pool is setup on demand */
    outcurl->state.ಕೊನೆಯ_conn_id = -1;
    outcurl->state.ಕೊನೆಯ_cpid = UINT32_MAX;
    outcurl->id = -1;
    outcurl->mid = UINT32_MAX;
    outcurl->master_mid = UINT32_MAX;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HTTP
    Curl_llist_init(&outcurl->state.httphdrs, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    Curl_initinfo(outcurl);

    /* copy all userdefined values */
    ಆದರೆ(dupset(outcurl, ದತ್ತಾಂಶ))
      ಹೋಗು fail;

    outcurl->ಪ್ರಗತಿ.hide     = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.hide;
    outcurl->ಪ್ರಗತಿ.callback = ದತ್ತಾಂಶ->ಪ್ರಗತಿ.callback;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
    outcurl->state.cookielist = ಖಾಲಿ;
    ಆದರೆ(ದತ್ತಾಂಶ->cookies && ದತ್ತಾಂಶ->state.cookie_engine) {
      /* If cookies are enabled in the parent handle, we enable them
         in the clone as well! */
      outcurl->cookies = Curl_cookie_init();
      ಆದರೆ(!outcurl->cookies)
        ಹೋಗು fail;
      outcurl->state.cookie_engine = TRUE;
    }

    ಆದರೆ(ದತ್ತಾಂಶ->state.cookielist) {
      outcurl->state.cookielist = Curl_slist_duplicate(ದತ್ತಾಂಶ->state.cookielist);
      ಆದರೆ(!outcurl->state.cookielist)
        ಹೋಗು fail;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.url)) {
      Curl_bufref_ಗಣ(&outcurl->state.url,
                      Curl_bufref_dup(&ದತ್ತಾಂಶ->state.url), 0,
                      curl_free);
      ಆದರೆ(!Curl_bufref_ptr(&outcurl->state.url))
        ಹೋಗು fail;
    }
    ಆದರೆ(Curl_bufref_ptr(&ದತ್ತಾಂಶ->state.referer)) {
      Curl_bufref_ಗಣ(&outcurl->state.referer,
                      Curl_bufref_dup(&ದತ್ತಾಂಶ->state.referer), 0,
                      curl_free);
      ಆದರೆ(!Curl_bufref_ptr(&outcurl->state.referer))
        ಹೋಗು fail;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_ALTSVC
    ಆದರೆ(ದತ್ತಾಂಶ->asi) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;
      outcurl->asi = Curl_altsvc_init();
      ಆದರೆ(!outcurl->asi)
        ಹೋಗು fail;
      str = CURL_EASY_STR(outcurl, STRING_ALTSVC);
      ಆದರೆ(str)
        (ಶೂನ್ಯ)Curl_altsvc_ತುಂಬು(outcurl->asi, str);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_HSTS
    ಆದರೆ(ದತ್ತಾಂಶ->hsts) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;
      outcurl->hsts = Curl_hsts_init();
      ಆದರೆ(!outcurl->hsts)
        ಹೋಗು fail;
      str = CURL_EASY_STR(outcurl, STRING_HSTS);
      ಆದರೆ(str)
        (ಶೂನ್ಯ)Curl_hsts_loadfile(outcurl, outcurl->hsts, str);
      (ಶೂನ್ಯ)Curl_hsts_loadcb(outcurl, outcurl->hsts);

      /* Copy entries learned at runtime. (E.g. Strict-Transport-Security
         headers.) */
      ಆದರೆ(Curl_hsts_copy(outcurl->hsts, ದತ್ತಾಂಶ->hsts))
        ಹೋಗು fail;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* we reach this point and thus we are OK */
    outcurl->magic = CURLEASY_MAGIC_ಸಂಖ್ಯೆ;
  }
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ outcurl;

fail:
  ಆದರೆ(outcurl) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_COOKIES
    curlx_free(outcurl->cookies);
#ಆದರೆ_ಅಂತ್ಯ
    curlx_dyn_free(&outcurl->state.headerb);
    Curl_altsvc_cleanup(&outcurl->asi);
    Curl_hsts_cleanup(&outcurl->hsts);
    Curl_freeset(outcurl);
    curlx_free(outcurl);
  }

  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * curl_easy_reset() is an external interface that allows an app to re-
 * initialize a session handle to the default values.
 */
ಶೂನ್ಯ curl_easy_reset(CURL *curl)
{
  ರಚನೆ Curl_eapi_guard guard;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_reset, ಖಾಲಿ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;

    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id = -1; /* clear remembered connection id */
    ದತ್ತಾಂಶ->state.ಕೊನೆಯ_cpid = UINT32_MAX;
    Curl_req_hard_reset(&ದತ್ತಾಂಶ->req, ದತ್ತಾಂಶ);
    Curl_hash_clean(&ದತ್ತಾಂಶ->meta_hash);

    /* clear all meta data */
    Curl_meta_reset(ದತ್ತಾಂಶ);
    /* zero out UserDefined data: */
    Curl_freeset(ದತ್ತಾಂಶ);
    memset(&ದತ್ತಾಂಶ->ಗಣ, 0, ಗಾತ್ರ(ರಚನೆ UserDefined));
    Curl_init_userdefined(ದತ್ತಾಂಶ);

    /* zero out Progress data: */
    memset(&ದತ್ತಾಂಶ->ಪ್ರಗತಿ, 0, ಗಾತ್ರ(ರಚನೆ ಪ್ರಗತಿ_2));

    /* zero out PureInfo data: */
    Curl_initinfo(ದತ್ತಾಂಶ);

    ದತ್ತಾಂಶ->ಪ್ರಗತಿ.hide = TRUE;

    /* zero out authentication data: */
    memset(&ದತ್ತಾಂಶ->state.authhost, 0, ಗಾತ್ರ(ರಚನೆ auth));
    memset(&ದತ್ತಾಂಶ->state.authproxy, 0, ಗಾತ್ರ(ರಚನೆ auth));

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_DIGEST_AUTH)
    Curl_http_auth_cleanup_digest(ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ
    ದತ್ತಾಂಶ->master_mid = UINT32_MAX;
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
CURLcode curl_easy_pause(CURL *curl, ಪೂರ್ಣಾಂಕ action)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_pause, &ಫಲಿತಾಂಶ)) {
    bool ಬದಲಾಯಿತು = FALSE;
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    bool recv_paused, recv_paused_new;
    bool send_paused, send_paused_new;

    ಆದರೆ(!ದತ್ತಾಂಶ->conn) {
      /* crazy input, do not continue */
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    recv_paused = Curl_xfer_recv_is_paused(ದತ್ತಾಂಶ);
    recv_paused_new = (action & CURLPAUSE_RECV);
    send_paused = Curl_xfer_send_is_paused(ದತ್ತಾಂಶ);
    send_paused_new = (action & CURLPAUSE_SEND);

    ಆದರೆ((send_paused != send_paused_new) ||
       (send_paused_new != Curl_creader_is_paused(ದತ್ತಾಂಶ))) {
      ಬದಲಾಯಿತು = TRUE;
      ಫಲಿತಾಂಶ = Curl_1st_fatal(
        ಫಲಿತಾಂಶ, Curl_xfer_pause_send(ದತ್ತಾಂಶ, send_paused_new));
    }

    ಆದರೆ(recv_paused != recv_paused_new) {
      ಬದಲಾಯಿತು = TRUE;
      ಫಲಿತಾಂಶ = Curl_1st_fatal(
        ಫಲಿತಾಂಶ, Curl_xfer_pause_recv(ದತ್ತಾಂಶ, recv_paused_new));
    }

    /* If not completely pausing both directions, run again in any case. */
    ಆದರೆ(!Curl_xfer_is_blocked(ದತ್ತಾಂಶ)) {
      /* reset the too-slow time keeper */
      ದತ್ತಾಂಶ->state.keeps_ವೇಗ.tv_sec = 0;
      ಆದರೆ(ದತ್ತಾಂಶ->multi) {
        Curl_multi_mark_dirty(ದತ್ತಾಂಶ); /* make it run */
        /* On changes, tell application to update its timers. */
        ಆದರೆ(ಬದಲಾಯಿತು && Curl_update_timer(ದತ್ತಾಂಶ->multi) && !ಫಲಿತಾಂಶ)
          ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
      }
    }

    ಆದರೆ(!ಫಲಿತಾಂಶ && ಬದಲಾಯಿತು && !ದತ್ತಾಂಶ->state.ಮುಗಿದ && ದತ್ತಾಂಶ->multi &&
       /* pause/unpausing may result in multi event changes */
       Curl_multi_ev_assess_xfer(ದತ್ತಾಂಶ->multi, ದತ್ತಾಂಶ))
      ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
  }
ಹೊರಹೋಗು:
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode easy_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata **connp)
{
  curl_socket_t sfd;

  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  /* only allow these to be called on handles with CURLOPT_CONNECT_ONLY */
  ಆದರೆ(!ದತ್ತಾಂಶ->ಗಣ.connect_only) {
    failf(ದತ್ತಾಂಶ, "CONNECT_ONLY is required");
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
  }

  sfd = Curl_getconnectinfo(ದತ್ತಾಂಶ, connp);

  ಆದರೆ(sfd == CURL_SOCKET_BAD) {
    failf(ದತ್ತಾಂಶ, "Failed to get last socket used for connection #%" FMT_OFF_T,
          ದತ್ತಾಂಶ->state.ಕೊನೆಯ_conn_id);
    ಹಿಂತಿರುಗಿ CURLE_UNSUPPORTED_PROTOCOL;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Receives data from the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 * Returns CURLE_OK on success, error code on error.
 */
CURLcode Curl_easy_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಶೂನ್ಯ *ಬಫರ್, size_t buflen, size_t *n)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *c;

  ಫಲಿತಾಂಶ = easy_connection(ದತ್ತಾಂಶ, &c);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(!ದತ್ತಾಂಶ->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(ದತ್ತಾಂಶ, c, TRUE);

  *n = 0;
  ಹಿಂತಿರುಗಿ Curl_conn_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, ಬಫರ್, buflen, n);
}

CURLcode curl_easy_recv(CURL *curl, ಶೂನ್ಯ *ಬಫರ್, size_t buflen, size_t *n)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_recv, &ಫಲಿತಾಂಶ)) {
    ಆದರೆ(!n || (buflen && !ಬಫರ್)) {
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಫಲಿತಾಂಶ = Curl_easy_recv(curl, ಬಫರ್, buflen, n);
  }
ಹೊರಹೋಗು:
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_WEBSOCKETS
CURLcode Curl_connect_only_attach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *c = ಖಾಲಿ;

  ಫಲಿತಾಂಶ = easy_connection(ದತ್ತಾಂಶ, &c);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(!ದತ್ತಾಂಶ->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(ದತ್ತಾಂಶ, c, TRUE);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_WEBSOCKETS */

/*
 * Sends data over the connected socket.
 *
 * This is the private internal version of curl_easy_send()
 */
CURLcode Curl_senddata(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್,
                       size_t buflen, size_t *n)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *c = ಖಾಲಿ;
  ರಚನೆ Curl_sigpipe_ctx sigpipe_ctx;

  *n = 0;
  ಫಲಿತಾಂಶ = easy_connection(ದತ್ತಾಂಶ, &c);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(!ದತ್ತಾಂಶ->conn)
    /* on first invoke, the transfer has been detached from the connection and
       needs to be reattached */
    Curl_attach_connection(ದತ್ತಾಂಶ, c, TRUE);

  sigpipe_ignore(ದತ್ತಾಂಶ, &sigpipe_ctx);
  ಫಲಿತಾಂಶ = Curl_conn_send(ದತ್ತಾಂಶ, FIRSTSOCKET, ಬಫರ್, buflen, FALSE, n);
  sigpipe_restore(&sigpipe_ctx);

  ಆದರೆ(ಫಲಿತಾಂಶ && ಫಲಿತಾಂಶ != CURLE_AGAIN)
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Sends data over the connected socket. Use after successful
 * curl_easy_perform() with CURLOPT_CONNECT_ONLY option.
 */
CURLcode curl_easy_send(CURL *curl, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್, size_t buflen,
                        size_t *n)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_send, &ಫಲಿತಾಂಶ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    size_t written = 0;

    ಆದರೆ(!n || (buflen && !ಬಫರ್)) {
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಫಲಿತಾಂಶ = Curl_senddata(ದತ್ತಾಂಶ, ಬಫರ್, buflen, &written);
    *n = written;
  }
ಹೊರಹೋಗು:
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Performs connection upkeep for the given session handle.
 */
CURLcode curl_easy_upkeep(CURL *curl)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_upkeep, &ಫಲಿತಾಂಶ)) {
    /* Use the common function to keep connections alive. */
    ಫಲಿತಾಂಶ = Curl_cpool_upkeep((ರಚನೆ Curl_easy *)curl);
  }
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode curl_easy_ssls_import(CURL *curl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *session_ಕೀಲಿ,
                               ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *shmac, size_t shmac_len,
                               ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sdata, size_t sdata_len)
{
#ಆದರೆ defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_ssls_import, &ಫಲಿತಾಂಶ)) {
    ಆದರೆ(!sdata || !sdata_len)
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = Curl_ssl_session_import((ರಚನೆ Curl_easy *)curl, session_ಕೀಲಿ,
                                       shmac, shmac_len, sdata, sdata_len);
  }
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)curl;
  (ಶೂನ್ಯ)session_ಕೀಲಿ;
  (ಶೂನ್ಯ)shmac;
  (ಶೂನ್ಯ)shmac_len;
  (ಶೂನ್ಯ)sdata;
  (ಶೂನ್ಯ)sdata_len;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
}

CURLcode curl_easy_ssls_export(CURL *curl,
                               curl_ssls_export_cb *export_fn,
                               ಶೂನ್ಯ *userptr)
{
#ಆದರೆ defined(USE_SSL) && defined(USE_SSLS_EXPORT)
  ರಚನೆ Curl_eapi_guard guard;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_ssls_export, &ಫಲಿತಾಂಶ)) {
    ಆದರೆ(!export_fn)
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = Curl_ssl_session_export((ರಚನೆ Curl_easy *)curl,
                                       export_fn, userptr);
  }
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)curl;
  (ಶೂನ್ಯ)export_fn;
  (ಶೂನ್ಯ)userptr;
  ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
}

CURLcode Curl_meta_ಗಣ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                       ಶೂನ್ಯ *meta_ದತ್ತಾಂಶ, Curl_meta_dtor *meta_dtor)
{
  DEBUGASSERT(meta_ದತ್ತಾಂಶ); /* never set to NULL */
  ಆದರೆ(!Curl_hash_add2(&ದತ್ತಾಂಶ->meta_hash, ಕೀಲಿ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕೀಲಿ) + 1,
                     meta_ದತ್ತಾಂಶ, meta_dtor)) {
    meta_dtor(ಕೀಲಿ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕೀಲಿ) + 1, meta_ದತ್ತಾಂಶ);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_meta_remove(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ)
{
  Curl_hash_delete(&ದತ್ತಾಂಶ->meta_hash, ಕೀಲಿ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕೀಲಿ) + 1);
}

ಶೂನ್ಯ *Curl_meta_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ)
{
  ಹಿಂತಿರುಗಿ Curl_hash_pick(&ದತ್ತಾಂಶ->meta_hash, ಕೀಲಿ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕೀಲಿ) + 1);
}

ಶೂನ್ಯ Curl_meta_reset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  Curl_hash_clean(&ದತ್ತಾಂಶ->meta_hash);
}
