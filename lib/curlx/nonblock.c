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

#که_تعریف_شوی HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#د_که_پای
#که_تعریف_شوی HAVE_FCNTL_H
#ضمن <fcntl.h>
#د_که_پای

#که_تعریف_شوی __VMS
#ضمن <in.h>
#ضمن <inet.h>
#د_که_پای

#ضمن "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
صحيح curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   صحيح nonblock   /* TRUE or FALSE */)
{
#که_تعریف_شوی HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  صحيح flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  إذا(flags < 0)
    أرجع -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  إذا(!!(flags & O_NONBLOCK) == !!nonblock)
    أرجع 0;
  إذا(nonblock)
    flags |= O_NONBLOCK;
  وإلا
    flags &= ~O_NONBLOCK;
  أرجع sfcntl(sockfd, F_SETFL, flags);

#وإلاإذا defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  اوږد flags = nonblock ? 1L : 0L;
  أرجع IoctlSocket(sockfd, FIONBIO, (حرف *)&flags);

#وإلاإذا defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  صحيح flags = nonblock ? 1 : 0;
  أرجع ioctl(sockfd, FIONBIO, &flags);

#وإلاإذا defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  بینښه اوږد flags = nonblock ? 1UL : 0UL;
  أرجع ioctlsocket(sockfd, (اوږد)FIONBIO, &flags);

#وإلاإذا defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  اوږد b = nonblock ? 1L : 0L;
  أرجع setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, حجم(b));

#وإلا
#تېروتنه "no non-blocking method was found/used/set"
#د_که_پای
}
