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

#నిర్వచితమైతే HAVE_STRERROR_R
#  ఒకవేళ (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#అయితే_ముగింపు

#అన్తర్భావ "curlx/winapi.h"
#అన్తర్భావ "curlx/snprintf.h"
#అన్తర్భావ "curlx/strerr.h"
#అన్తర్భావ "curlx/strcopy.h"

#నిర్వచితమైతే USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
స్థిర స్థిరరాశి అక్షరం *get_winsock_error(పూర్ణాంకం err, అక్షరం *buf, size_t len)
{
  VERBOSE(స్థిరరాశి అక్షరం *p);

  ఒకవేళ(!len)
    తిరిగి ఖాళీ;

  *buf = '\0';

#నిర్వచితం_కాకపోతే CURLVERBOSE
  (శూన్య)err;
  తిరిగి ఖాళీ;
#లేకపోతే
  ఎంపిక(err) {
  స్థితి WSAEINTR:
    p = "Call interrupted";
    ఆపు;
  స్థితి WSAEBADF:
    p = "Bad file";
    ఆపు;
  స్థితి WSAEACCES:
    p = "Bad access";
    ఆపు;
  స్థితి WSAEFAULT:
    p = "Bad argument";
    ఆపు;
  స్థితి WSAEINVAL:
    p = "Invalid arguments";
    ఆపు;
  స్థితి WSAEMFILE:
    p = "Out of file descriptors";
    ఆపు;
  స్థితి WSAEWOULDBLOCK:
    p = "Call would block";
    ఆపు;
  స్థితి WSAEINPROGRESS:
  స్థితి WSAEALREADY:
    p = "Blocking call in progress";
    ఆపు;
  స్థితి WSAENOTSOCK:
    p = "Descriptor is not a socket";
    ఆపు;
  స్థితి WSAEDESTADDRREQ:
    p = "Need destination address";
    ఆపు;
  స్థితి WSAEMSGSIZE:
    p = "Bad message size";
    ఆపు;
  స్థితి WSAEPROTOTYPE:
    p = "Bad protocol";
    ఆపు;
  స్థితి WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    ఆపు;
  స్థితి WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    ఆపు;
  స్థితి WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    ఆపు;
  స్థితి WSAEOPNOTSUPP:
    p = "Operation not supported";
    ఆపు;
  స్థితి WSAEAFNOSUPPORT:
    p = "Address family not supported";
    ఆపు;
  స్థితి WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    ఆపు;
  స్థితి WSAEADDRINUSE:
    p = "Address already in use";
    ఆపు;
  స్థితి WSAEADDRNOTAVAIL:
    p = "Address not available";
    ఆపు;
  స్థితి WSAENETDOWN:
    p = "Network down";
    ఆపు;
  స్థితి WSAENETUNREACH:
    p = "Network unreachable";
    ఆపు;
  స్థితి WSAENETRESET:
    p = "Network has been reset";
    ఆపు;
  స్థితి WSAECONNABORTED:
    p = "Connection was aborted";
    ఆపు;
  స్థితి WSAECONNRESET:
    p = "Connection was reset";
    ఆపు;
  స్థితి WSAENOBUFS:
    p = "No buffer space";
    ఆపు;
  స్థితి WSAEISCONN:
    p = "Socket is already connected";
    ఆపు;
  స్థితి WSAENOTCONN:
    p = "Socket is not connected";
    ఆపు;
  స్థితి WSAESHUTDOWN:
    p = "Socket has been shut down";
    ఆపు;
  స్థితి WSAETOOMANYREFS:
    p = "Too many references";
    ఆపు;
  స్థితి WSAETIMEDOUT:
    p = "Timed out";
    ఆపు;
  స్థితి WSAECONNREFUSED:
    p = "Connection refused";
    ఆపు;
  స్థితి WSAELOOP:
    p = "Loop??";
    ఆపు;
  స్థితి WSAENAMETOOLONG:
    p = "Name too long";
    ఆపు;
  స్థితి WSAEHOSTDOWN:
    p = "Host down";
    ఆపు;
  స్థితి WSAEHOSTUNREACH:
    p = "Host unreachable";
    ఆపు;
  స్థితి WSAENOTEMPTY:
    p = "Not empty";
    ఆపు;
  స్థితి WSAEPROCLIM:
    p = "Process limit reached";
    ఆపు;
  స్థితి WSAEUSERS:
    p = "Too many users";
    ఆపు;
  స్థితి WSAEDQUOT:
    p = "Bad quota";
    ఆపు;
  స్థితి WSAESTALE:
    p = "Something is stale";
    ఆపు;
  స్థితి WSAEREMOTE:
    p = "Remote error";
    ఆపు;
  స్థితి WSAEDISCON:
    p = "Disconnected";
    ఆపు;
    /* Extended Winsock errors */
  స్థితి WSASYSNOTREADY:
    p = "Winsock library is not ready";
    ఆపు;
  స్థితి WSANOTINITIALISED:
    p = "Winsock library not initialized";
    ఆపు;
  స్థితి WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    ఆపు;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  స్థితి WSAHOST_NOT_దొరికింది:
    p = "Host not found";
    ఆపు;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  స్థితి WSATRY_AGAIN:
    p = "Host not found, try again";
    ఆపు;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  స్థితి WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    ఆపు;

    /* Valid name, no data record of requested type */
  స్థితి WSANO_దత్తాంశం:
    p = "No data record of requested type";
    ఆపు;

  అప్రమేయ:
    తిరిగి ఖాళీ;
  }
  curlx_strcopy(buf, len, p, అక్షరశ్రేణి_పొడవు(p));
  తిరిగి buf;
#అయితే_ముగింపు
}
#అయితే_ముగింపు /* USE_WINSOCK */

/*
 * Our thread-safe and smart strerror() replacement.
 *
 * The 'err' argument passed in to this function MUST be a true errno number
 * as reported on this system. We do no range checking on the number before
 * we pass it to the "number-to-message" conversion function and there might
 * be systems that do not do proper range checking in there themselves.
 *
 * We do not do range checking (on systems other than Windows) since there is
 * no good reliable and portable way to do it.
 *
 * On Windows different types of error codes overlap. This function has an
 * order of preference when trying to match error codes:
 * CRT (errno), Winsock (WSAGetLastError), Windows API (GetLastError).
 *
 * It may be more correct to call one of the variant functions instead:
 * Call Curl_sspi_strerror if the error code is definitely Windows SSPI.
 * Call curlx_winapi_strerror if the error code is definitely Windows API.
 */
స్థిరరాశి అక్షరం *curlx_strerror(పూర్ణాంకం err, అక్షరం *buf, size_t buflen)
{
#నిర్వచితమైతే _WIN32
  DWORD old_win_err = Get_చివరి_Error();
#అయితే_ముగింపు
  పూర్ణాంకం old_errno = errno;
  అక్షరం *p;

  ఒకవేళ(!buflen)
    తిరిగి ఖాళీ;

#నిర్వచితం_కాకపోతే _WIN32
  DEBUGASSERT(err >= 0);
#అయితే_ముగింపు

  *buf = '\0';

#నిర్వచితమైతే _WIN32
  ఒకవేళ((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#నిర్వచితమైతే USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#అయితే_ముగింపు
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (గుర్తులేని పూర్ణాంకం)err);
#లేకపోతే /* !_WIN32 */

#ఒకవేళ defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  ఒకవేళ(strerror_r(err, buf, buflen) &&
     buflen > పరిమాణం("Unknown error ") + 20) {
    ఒకవేళ(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#లేదావేళ defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    అక్షరం బఫరు[256];
    స్థిరరాశి అక్షరం *సందేశం = strerror_r(err, బఫరు, పరిమాణం(బఫరు));
    ఒకవేళ(సందేశం && buflen > 1)
      SNPRINTF(buf, buflen, "%s", సందేశం);
    లేకపోతే ఒకవేళ(buflen > పరిమాణం("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#లేకపోతే
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    స్థిరరాశి అక్షరం *సందేశం = strerror(err);
    ఒకవేళ(సందేశం && buflen > 1)
      SNPRINTF(buf, buflen, "%s", సందేశం);
    లేకపోతే ఒకవేళ(buflen > పరిమాణం("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#అయితే_ముగింపు

#అయితే_ముగింపు /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  ఒకవేళ(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  ఒకవేళ(p && (p - buf) >= 1)
    *p = '\0';

  ఒకవేళ(errno != old_errno)
    errno = old_errno;

#నిర్వచితమైతే _WIN32
  ఒకవేళ(old_win_err != Get_చివరి_Error())
    సమితి_చివరి_Error(old_win_err);
#అయితే_ముగింపు

  తిరిగి buf;
}
