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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_SELECT_H
#ಅನ್ತರ್ಭಾವ <sys/select.h>
#ಅಥವಾಆದರೆ defined(HAVE_UNISTD_H)
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSDOS
#ಅನ್ತರ್ಭಾವ <dos.h>  /* delay() */
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curlx/timediff.h"
#ಅನ್ತರ್ಭಾವ "curlx/wait.h"

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
ಪೂರ್ಣಾಂಕ curlx_wait_ms(timediff_t timeout_ms)
{
  ಪೂರ್ಣಾಂಕ r = 0;

  ಆದರೆ(!timeout_ms)
    ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(timeout_ms < 0) {
    ಗಣ_SOCKERRNO(SOCKEINVAL);
    ಹಿಂತಿರುಗಿ -1;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSDOS
  delay((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)timeout_ms);
#ಅಥವಾಆದರೆ defined(_WIN32)
  /* prevent overflow, timeout_ms is typecast to ULONG/DWORD. */
#ಆದರೆ TIMEDIFF_T_MAX >= ULONG_MAX
  ಆದರೆ(timeout_ms >= ULONG_MAX)
    timeout_ms = ULONG_MAX - 1;
    /* do not use ULONG_MAX, because that is equal to INFINITE */
#ಆದರೆ_ಅಂತ್ಯ
  Sleep((DWORD)timeout_ms);
#ಇಲ್ಲದಿದ್ದರೆ
  /* avoid using poll() for this since it behaves incorrectly with no sockets
     on Apple operating systems */
  {
    ರಚನೆ timeval pending_tv;
    r = select(0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, curlx_mstotv(&pending_tv, timeout_ms));
  }
#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */
  ಆದರೆ(r) {
    ಆದರೆ((r == -1) && (SOCKERRNO == SOCKEINTR))
      /* make EINTR from select or poll not a "lethal" error */
      r = 0;
    ಇಲ್ಲದಿದ್ದರೆ
      r = -1;
  }
  ಹಿಂತಿರುಗಿ r;
}
