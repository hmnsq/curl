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

#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif
#ifdef HAVE_FCNTL_H
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#endif

#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonblock   /* TRUE or FALSE */)
{
#ifdef HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  ଯଦି(flags < 0)
    ଫେରନ୍ତୁ -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  ଯଦି(!!(flags & O_NONBLOCK) == !!nonblock)
    ଫେରନ୍ତୁ 0;
  ଯଦି(nonblock)
    flags |= O_NONBLOCK;
  ଅନ୍ୟଥା
    flags &= ~O_NONBLOCK;
  ଫେରନ୍ତୁ sfcntl(sockfd, F_SETFL, flags);

#ନଚେତଯଦି defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  long flags = nonblock ? 1L : 0L;
  ଫେରନ୍ତୁ IoctlSocket(sockfd, FIONBIO, (ଅକ୍ଷର *)&flags);

#ନଚେତଯଦି defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = nonblock ? 1 : 0;
  ଫେରନ୍ତୁ ioctl(sockfd, FIONBIO, &flags);

#ନଚେତଯଦି defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  unsigned long flags = nonblock ? 1UL : 0UL;
  ଫେରନ୍ତୁ ioctlsocket(sockfd, (long)FIONBIO, &flags);

#ନଚେତଯଦି defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  long b = nonblock ? 1L : 0L;
  ଫେରନ୍ତୁ setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, ଆକାର(b));

#ଅନ୍ୟଥା
#error "no non-blocking method was found/used/set"
#endif
}
