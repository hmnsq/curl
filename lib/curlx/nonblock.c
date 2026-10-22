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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#अन्तर्भाव <fcntl.h>
#endif

#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
पूर्णाङ्क curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   पूर्णाङ्क nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  पूर्णाङ्क flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  जइ(flags < 0)
    प्रत्यागम -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  जइ(!!(flags & O_NONBLOCK) == !!nonblock)
    प्रत्यागम 0;
  जइ(nonblock)
    flags |= O_NONBLOCK;
  अण्णहा
    flags &= ~O_NONBLOCK;
  प्रत्यागम sfcntl(sockfd, F_SETFL, flags);

#नोचेद्यदि defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  प्रत्यागम IoctlSocket(sockfd, FIONBIO, (वर्ण *)&flags);

#नोचेद्यदि defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  पूर्णाङ्क flags = nonblock ? 1 : 0;
  प्रत्यागम ioctl(sockfd, FIONBIO, &flags);

#नोचेद्यदि defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  प्रत्यागम ioctlsocket(sockfd, (long)FIONBIO, &flags);

#नोचेद्यदि defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  प्रत्यागम setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, परिमाण(b));

#अण्णहा
#error "no non-blocking method was found/used/set"
#endif
}
