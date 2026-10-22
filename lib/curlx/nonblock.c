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

#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#شامل <fcntl.h>
#endif

#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

#شامل "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
صحیح curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   صحیح nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  صحیح flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  اگر(flags < 0)
    بازگشت -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  اگر(!!(flags & O_NONBLOCK) == !!nonblock)
    بازگشت 0;
  اگر(nonblock)
    flags |= O_NONBLOCK;
  وگرنه
    flags &= ~O_NONBLOCK;
  بازگشت sfcntl(sockfd, F_SETFL, flags);

#وگرنهاگر defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  بازگشت IoctlSocket(sockfd, FIONBIO, (نویسه *)&flags);

#وگرنهاگر defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  صحیح flags = nonblock ? 1 : 0;
  بازگشت ioctl(sockfd, FIONBIO, &flags);

#وگرنهاگر defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  بازگشت ioctlsocket(sockfd, (long)FIONBIO, &flags);

#وگرنهاگر defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  بازگشت setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, اندازه(b));

#وگرنه
#error "no non-blocking method was found/used/set"
#endif
}
