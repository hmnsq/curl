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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే HAVE_SYS_SELECT_H
#అన్తర్భావ <sys/select.h>
#లేదావేళ defined(HAVE_UNISTD_H)
#అన్తర్భావ <unistd.h>
#అయితే_ముగింపు

#నిర్వచితమైతే MSDOS
#అన్తర్భావ <dos.h>  /* delay() */
#అయితే_ముగింపు

#అన్తర్భావ "curlx/timediff.h"
#అన్తర్భావ "curlx/wait.h"

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
పూర్ణాంకం curlx_wait_ms(timediff_t timeout_ms)
{
  పూర్ణాంకం r = 0;

  ఒకవేళ(!timeout_ms)
    తిరిగి 0;
  ఒకవేళ(timeout_ms < 0) {
    సమితి_SOCKERRNO(SOCKEINVAL);
    తిరిగి -1;
  }
#నిర్వచితమైతే MSDOS
  delay((గుర్తులేని పూర్ణాంకం)timeout_ms);
#లేదావేళ defined(_WIN32)
  /* prevent overflow, timeout_ms is typecast to ULONG/DWORD. */
#ఒకవేళ TIMEDIFF_T_MAX >= ULONG_MAX
  ఒకవేళ(timeout_ms >= ULONG_MAX)
    timeout_ms = ULONG_MAX - 1;
    /* do not use ULONG_MAX, because that is equal to INFINITE */
#అయితే_ముగింపు
  Sleep((DWORD)timeout_ms);
#లేకపోతే
  /* avoid using poll() for this since it behaves incorrectly with no sockets
     on Apple operating systems */
  {
    నిర్మాణం timeval pending_tv;
    r = select(0, ఖాళీ, ఖాళీ, ఖాళీ, curlx_mstotv(&pending_tv, timeout_ms));
  }
#అయితే_ముగింపు /* _WIN32 */
  ఒకవేళ(r) {
    ఒకవేళ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    లేకపోతే
      r = -1;
  }
  తిరిగి r;
}
