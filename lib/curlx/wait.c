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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_SYS_SELECT_H
#ଅନ୍ତର୍ଭାଵ <sys/select.h>
#ନଚେତଯଦି defined(HAVE_UNISTD_H)
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#endif

#ifdef MSDOS
#ଅନ୍ତର୍ଭାଵ <dos.h>  /* delay() */
#endif

#ଅନ୍ତର୍ଭାଵ "curlx/timediff.h"
#ଅନ୍ତର୍ଭାଵ "curlx/wait.h"

/*
 * Internal function used for waiting a specific amount of ms in
 * Curl_socket_check() and Curl_poll() when no file descriptor is provided to
 * wait on, being used to delay execution. Winsock select() and poll() timeout
 * mechanisms need a valid socket descriptor in a not null file descriptor set
 * to work. Waiting indefinitely with this function is not allowed, a zero or
 * negative timeout value is returned immediately. Timeout resolution,
 * accuracy, as well as maximum supported value is system dependent, neither
 * factor is a critical issue for the intended use of this function in the
 * library.
 *
 * Return values:
 *   -1 = system call error, or invalid timeout value
 *    0 = specified timeout has elapsed, or interrupted
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_wait_ms(timediff_t timeout_ms)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0;

  ଯଦି(!timeout_ms)
    ଫେରନ୍ତୁ 0;
  ଯଦି(timeout_ms < 0) {
    SET_SOCKERRNO(SOCKEINVAL);
    ଫେରନ୍ତୁ -1;
  }
#ifdef MSDOS
  delay((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)timeout_ms);
#ନଚେତଯଦି defined(_WIN32)
  /* prevent overflow, timeout_ms is typecast to ULONG/DWORD. */
#ଯଦି TIMEDIFF_T_MAX >= ULONG_MAX
  ଯଦି(timeout_ms >= ULONG_MAX)
    timeout_ms = ULONG_MAX - 1;
    /* do not use ULONG_MAX, because that is equal to INFINITE */
#endif
  Sleep((DWORD)timeout_ms);
#ଅନ୍ୟଥା
  /* avoid using poll() for this since it behaves incorrectly with no sockets
     on Apple operating systems */
  {
    ସଂରଚନା timeval pending_tv;
    r = select(0, ଖାଲି, ଖାଲି, ଖାଲି, curlx_mstotv(&pending_tv, timeout_ms));
  }
#endif /* _WIN32 */
  ଯଦି(r) {
    ଯଦି((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ଅନ୍ୟଥା
      r = -1;
  }
  ଫେରନ୍ତୁ r;
}
