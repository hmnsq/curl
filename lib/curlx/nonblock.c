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

#ifdef HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#endif

#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

#ಅನ್ತರ್ಭಾವ "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
ಪೂರ್ಣಾಙ್ಕ curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   ಪೂರ್ಣಾಙ್ಕ nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  ಪೂರ್ಣಾಙ್ಕ flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  ಯದಿ(flags < 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  ಯದಿ(!!(flags & O_NONBLOCK) == !!nonblock)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ(nonblock)
    flags |= O_NONBLOCK;
  ಅನ್ಯಥಾ
    flags &= ~O_NONBLOCK;
  ಪ್ರತ್ಯಾಗಮ sfcntl(sockfd, F_SETFL, flags);

#ನೋಚೇದ್ಯದಿ defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  ಪ್ರತ್ಯಾಗಮ IoctlSocket(sockfd, FIONBIO, (ವರ್ಣ *)&flags);

#ನೋಚೇದ್ಯದಿ defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  ಪೂರ್ಣಾಙ್ಕ flags = nonblock ? 1 : 0;
  ಪ್ರತ್ಯಾಗಮ ioctl(sockfd, FIONBIO, &flags);

#ನೋಚೇದ್ಯದಿ defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  ಪ್ರತ್ಯಾಗಮ ioctlsocket(sockfd, (long)FIONBIO, &flags);

#ನೋಚೇದ್ಯದಿ defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  ಪ್ರತ್ಯಾಗಮ setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, ಪರಿಮಾಣ(b));

#ಅನ್ಯಥಾ
#error "no non-blocking method was found/used/set"
#endif
}
