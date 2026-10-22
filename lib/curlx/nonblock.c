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

#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_FCNTL_H
#అన్తర్భావ <fcntl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
పూర్ణాంకం curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   పూర్ణాంకం nonblock   /* TRUE or FALSE */)
{
#నిర్వచితమైతే HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  పూర్ణాంకం flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  ఒకవేళ(flags < 0)
    తిరిగి -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  ఒకవేళ(!!(flags & O_NONBLOCK) == !!nonblock)
    తిరిగి 0;
  ఒకవేళ(nonblock)
    flags |= O_NONBLOCK;
  లేకపోతే
    flags &= ~O_NONBLOCK;
  తిరిగి sfcntl(sockfd, F_SETFL, flags);

#లేదావేళ defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  దీర్ఘ flags = nonblock ? 1L : 0L;
  తిరిగి IoctlSocket(sockfd, FIONBIO, (అక్షరం *)&flags);

#లేదావేళ defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  పూర్ణాంకం flags = nonblock ? 1 : 0;
  తిరిగి ioctl(sockfd, FIONBIO, &flags);

#లేదావేళ defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  గుర్తులేని దీర్ఘ flags = nonblock ? 1UL : 0UL;
  తిరిగి ioctlsocket(sockfd, (దీర్ఘ)FIONBIO, &flags);

#లేదావేళ defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  దీర్ఘ b = nonblock ? 1L : 0L;
  తిరిగి setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, పరిమాణం(b));

#లేకపోతే
#దోషం "no non-blocking method was found/used/set"
#అయితే_ముగింపు
}
