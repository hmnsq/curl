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
#અન્તર્ભાવ "curl_setup.h"

#ifdef HAVE_STRERROR_R
#  જો (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#અન્તર્ભાવ "curlx/winapi.h"
#અન્તર્ભાવ "curlx/snprintf.h"
#અન્તર્ભાવ "curlx/strerr.h"
#અન્તર્ભાવ "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
સ્થિર અચલ અક્ષર *get_winsock_error(પૂર્ણાંક err, અક્ષર *buf, size_t len)
{
  VERBOSE(અચલ અક્ષર *p);

  જો(!len)
    પાછા ખાલી;

  *buf = '\0';

#ifndef CURLVERBOSE
  (શૂન્ય)err;
  પાછા ખાલી;
#નહીંતર
  પસંદગી(err) {
  સ્થિતિ WSAEINTR:
    p = "Call interrupted";
    તોડો;
  સ્થિતિ WSAEBADF:
    p = "Bad file";
    તોડો;
  સ્થિતિ WSAEACCES:
    p = "Bad access";
    તોડો;
  સ્થિતિ WSAEFAULT:
    p = "Bad argument";
    તોડો;
  સ્થિતિ WSAEINVAL:
    p = "Invalid arguments";
    તોડો;
  સ્થિતિ WSAEMFILE:
    p = "Out of file descriptors";
    તોડો;
  સ્થિતિ WSAEWOULDBLOCK:
    p = "Call would block";
    તોડો;
  સ્થિતિ WSAEINPROGRESS:
  સ્થિતિ WSAEALREADY:
    p = "Blocking call in progress";
    તોડો;
  સ્થિતિ WSAENOTSOCK:
    p = "Descriptor is not a socket";
    તોડો;
  સ્થિતિ WSAEDESTADDRREQ:
    p = "Need destination address";
    તોડો;
  સ્થિતિ WSAEMSGSIZE:
    p = "Bad message size";
    તોડો;
  સ્થિતિ WSAEPROTOTYPE:
    p = "Bad protocol";
    તોડો;
  સ્થિતિ WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    તોડો;
  સ્થિતિ WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    તોડો;
  સ્થિતિ WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    તોડો;
  સ્થિતિ WSAEOPNOTSUPP:
    p = "Operation not supported";
    તોડો;
  સ્થિતિ WSAEAFNOSUPPORT:
    p = "Address family not supported";
    તોડો;
  સ્થિતિ WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    તોડો;
  સ્થિતિ WSAEADDRINUSE:
    p = "Address already in use";
    તોડો;
  સ્થિતિ WSAEADDRNOTAVAIL:
    p = "Address not available";
    તોડો;
  સ્થિતિ WSAENETDOWN:
    p = "Network down";
    તોડો;
  સ્થિતિ WSAENETUNREACH:
    p = "Network unreachable";
    તોડો;
  સ્થિતિ WSAENETRESET:
    p = "Network has been reset";
    તોડો;
  સ્થિતિ WSAECONNABORTED:
    p = "Connection was aborted";
    તોડો;
  સ્થિતિ WSAECONNRESET:
    p = "Connection was reset";
    તોડો;
  સ્થિતિ WSAENOBUFS:
    p = "No buffer space";
    તોડો;
  સ્થિતિ WSAEISCONN:
    p = "Socket is already connected";
    તોડો;
  સ્થિતિ WSAENOTCONN:
    p = "Socket is not connected";
    તોડો;
  સ્થિતિ WSAESHUTDOWN:
    p = "Socket has been shut down";
    તોડો;
  સ્થિતિ WSAETOOMANYREFS:
    p = "Too many references";
    તોડો;
  સ્થિતિ WSAETIMEDOUT:
    p = "Timed out";
    તોડો;
  સ્થિતિ WSAECONNREFUSED:
    p = "Connection refused";
    તોડો;
  સ્થિતિ WSAELOOP:
    p = "Loop??";
    તોડો;
  સ્થિતિ WSAENAMETOOLONG:
    p = "Name too long";
    તોડો;
  સ્થિતિ WSAEHOSTDOWN:
    p = "Host down";
    તોડો;
  સ્થિતિ WSAEHOSTUNREACH:
    p = "Host unreachable";
    તોડો;
  સ્થિતિ WSAENOTEMPTY:
    p = "Not empty";
    તોડો;
  સ્થિતિ WSAEPROCLIM:
    p = "Process limit reached";
    તોડો;
  સ્થિતિ WSAEUSERS:
    p = "Too many users";
    તોડો;
  સ્થિતિ WSAEDQUOT:
    p = "Bad quota";
    તોડો;
  સ્થિતિ WSAESTALE:
    p = "Something is stale";
    તોડો;
  સ્થિતિ WSAEREMOTE:
    p = "Remote error";
    તોડો;
  સ્થિતિ WSAEDISCON:
    p = "Disconnected";
    તોડો;
    /* Extended Winsock errors */
  સ્થિતિ WSASYSNOTREADY:
    p = "Winsock library is not ready";
    તોડો;
  સ્થિતિ WSANOTINITIALISED:
    p = "Winsock library not initialized";
    તોડો;
  સ્થિતિ WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    તોડો;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  સ્થિતિ WSAHOST_NOT_FOUND:
    p = "Host not found";
    તોડો;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  સ્થિતિ WSATRY_AGAIN:
    p = "Host not found, try again";
    તોડો;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  સ્થિતિ WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    તોડો;

    /* Valid name, no data record of requested type */
  સ્થિતિ WSANO_DATA:
    p = "No data record of requested type";
    તોડો;

  મૂળભૂત:
    પાછા ખાલી;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  પાછા buf;
#endif
}
#endif /* USE_WINSOCK */

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
અચલ અક્ષર *curlx_strerror(પૂર્ણાંક err, અક્ષર *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  પૂર્ણાંક old_errno = errno;
  અક્ષર *p;

  જો(!buflen)
    પાછા ખાલી;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  જો((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned પૂર્ણાંક)err);
#નહીંતર /* !_WIN32 */

#જો defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  જો(strerror_r(err, buf, buflen) &&
     buflen > કદ("Unknown error ") + 20) {
    જો(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#અથવાજો defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    અક્ષર buffer[256];
    અચલ અક્ષર *સંદેશ = strerror_r(err, buffer, કદ(buffer));
    જો(સંદેશ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", સંદેશ);
    નહીંતર જો(buflen > કદ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#નહીંતર
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    અચલ અક્ષર *સંદેશ = strerror(err);
    જો(સંદેશ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", સંદેશ);
    નહીંતર જો(buflen > કદ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  જો(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  જો(p && (p - buf) >= 1)
    *p = '\0';

  જો(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  જો(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  પાછા buf;
}
