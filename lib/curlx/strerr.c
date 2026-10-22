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

#ifdef HAVE_STRERROR_R
#  ಯದಿ (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"
#ಅನ್ತರ್ಭಾವ "curlx/snprintf.h"
#ಅನ್ತರ್ಭಾವ "curlx/strerr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *get_winsock_error(ಪೂರ್ಣಾಙ್ಕ err, ವರ್ಣ *buf, size_t len)
{
  VERBOSE(ಧ್ರುವ ವರ್ಣ *p);

  ಯದಿ(!len)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  *buf = '\0';

#ifndef CURLVERBOSE
  (ಶೂನ್ಯ)err;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#ಅನ್ಯಥಾ
  ವಿಕಲ್ಪ(err) {
  ಪಕ್ಷ WSAEINTR:
    p = "Call interrupted";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEBADF:
    p = "Bad file";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEACCES:
    p = "Bad access";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEFAULT:
    p = "Bad argument";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEINVAL:
    p = "Invalid arguments";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEMFILE:
    p = "Out of file descriptors";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEWOULDBLOCK:
    p = "Call would block";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEINPROGRESS:
  ಪಕ್ಷ WSAEALREADY:
    p = "Blocking call in progress";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAENOTSOCK:
    p = "Descriptor is not a socket";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEDESTADDRREQ:
    p = "Need destination address";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEMSGSIZE:
    p = "Bad message size";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEPROTOTYPE:
    p = "Bad protocol";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEOPNOTSUPP:
    p = "Operation not supported";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEAFNOSUPPORT:
    p = "Address family not supported";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEADDRINUSE:
    p = "Address already in use";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEADDRNOTAVAIL:
    p = "Address not available";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAENETDOWN:
    p = "Network down";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAENETUNREACH:
    p = "Network unreachable";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAENETRESET:
    p = "Network has been reset";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAECONNABORTED:
    p = "Connection was aborted";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAECONNRESET:
    p = "Connection was reset";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAENOBUFS:
    p = "No buffer space";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEISCONN:
    p = "Socket is already connected";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAENOTCONN:
    p = "Socket is not connected";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAESHUTDOWN:
    p = "Socket has been shut down";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAETOOMANYREFS:
    p = "Too many references";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAETIMEDOUT:
    p = "Timed out";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAECONNREFUSED:
    p = "Connection refused";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAELOOP:
    p = "Loop??";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAENAMETOOLONG:
    p = "Name too long";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEHOSTDOWN:
    p = "Host down";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEHOSTUNREACH:
    p = "Host unreachable";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAENOTEMPTY:
    p = "Not empty";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEPROCLIM:
    p = "Process limit reached";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEUSERS:
    p = "Too many users";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEDQUOT:
    p = "Bad quota";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAESTALE:
    p = "Something is stale";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEREMOTE:
    p = "Remote error";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAEDISCON:
    p = "Disconnected";
    ಭಙ್ಗ;
    /* Extended Winsock errors */
  ಪಕ್ಷ WSASYSNOTREADY:
    p = "Winsock library is not ready";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSANOTINITIALISED:
    p = "Winsock library not initialized";
    ಭಙ್ಗ;
  ಪಕ್ಷ WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    ಭಙ್ಗ;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  ಪಕ್ಷ WSAHOST_NOT_FOUND:
    p = "Host not found";
    ಭಙ್ಗ;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  ಪಕ್ಷ WSATRY_AGAIN:
    p = "Host not found, try again";
    ಭಙ್ಗ;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  ಪಕ್ಷ WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    ಭಙ್ಗ;

    /* Valid name, no data record of requested type */
  ಪಕ್ಷ WSANO_DATA:
    p = "No data record of requested type";
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  ಪ್ರತ್ಯಾಗಮ buf;
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
ಧ್ರುವ ವರ್ಣ *curlx_strerror(ಪೂರ್ಣಾಙ್ಕ err, ವರ್ಣ *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  ಪೂರ್ಣಾಙ್ಕ old_errno = errno;
  ವರ್ಣ *p;

  ಯದಿ(!buflen)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  ಯದಿ((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned ಪೂರ್ಣಾಙ್ಕ)err);
#ಅನ್ಯಥಾ /* !_WIN32 */

#ಯದಿ defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  ಯದಿ(strerror_r(err, buf, buflen) &&
     buflen > ಪರಿಮಾಣ("Unknown error ") + 20) {
    ಯದಿ(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ನೋಚೇದ್ಯದಿ defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    ವರ್ಣ buffer[256];
    ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ = strerror_r(err, buffer, ಪರಿಮಾಣ(buffer));
    ಯದಿ(ಸಂದೇಶ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", ಸಂದೇಶ);
    ಅನ್ಯಥಾ ಯದಿ(buflen > ಪರಿಮಾಣ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ಅನ್ಯಥಾ
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ = strerror(err);
    ಯದಿ(ಸಂದೇಶ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", ಸಂದೇಶ);
    ಅನ್ಯಥಾ ಯದಿ(buflen > ಪರಿಮಾಣ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  ಯದಿ(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  ಯದಿ(p && (p - buf) >= 1)
    *p = '\0';

  ಯದಿ(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  ಯದಿ(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  ಪ್ರತ್ಯಾಗಮ buf;
}
