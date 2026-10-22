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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_FCNTL_H
#समावेश <fcntl.h>
#पूर्ण_अगर

#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "curlx/nonblock.h"

/*
 * curlx_nonblock() set the given socket to either blocking or non-blocking
 * mode based on the 'nonblock' boolean argument. This function is highly
 * portable.
 */
पूर्णांक curlx_nonblock(curl_socket_t sockfd,    /* operate on this */
                   पूर्णांक nonblock   /* TRUE or FALSE */)
{
#अगर_घोषित HAVE_FCNTL_O_NONBLOCK
  /* most recent Unix versions */
  पूर्णांक flags;
  flags = sfcntl(sockfd, F_GETFL, 0);
  अगर(flags < 0)
    वापस -1;
  /* Check if the current file status flags have already satisfied
   * the request, if so, it is no need to call fcntl() to replicate it.
   */
  अगर(!!(flags & O_NONBLOCK) == !!nonblock)
    वापस 0;
  अगर(nonblock)
    flags |= O_NONBLOCK;
  अन्यथा
    flags &= ~O_NONBLOCK;
  वापस sfcntl(sockfd, F_SETFL, flags);

#या_अगर defined(HAVE_IOCTLSOCKET_CAMEL_FIONBIO)

  /* Amiga */
  दीर्घ flags = nonblock ? 1L : 0L;
  वापस IoctlSocket(sockfd, FIONBIO, (अक्षर *)&flags);

#या_अगर defined(HAVE_IOCTL_FIONBIO)

  /* older Unix versions */
  पूर्णांक flags = nonblock ? 1 : 0;
  वापस ioctl(sockfd, FIONBIO, &flags);

#या_अगर defined(HAVE_IOCTLSOCKET_FIONBIO)

  /* Windows */
  अचिन्हित दीर्घ flags = nonblock ? 1UL : 0UL;
  वापस ioctlsocket(sockfd, (दीर्घ)FIONBIO, &flags);

#या_अगर defined(HAVE_SETSOCKOPT_SO_NONBLOCK)

  /* Orbis OS */
  दीर्घ b = nonblock ? 1L : 0L;
  वापस setsockopt(sockfd, SOL_SOCKET, SO_NONBLOCK, &b, माप(b));

#अन्यथा
#त्रुटि "no non-blocking method was found/used/set"
#पूर्ण_अगर
}
