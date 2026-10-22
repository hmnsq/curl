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

#ifdef HAVE_STRERROR_R
#  𐡄𐡍 (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#ضمن "curlx/winapi.h"
#ضمن "curlx/snprintf.h"
#ضمن "curlx/strerr.h"
#ضمن "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ساكن ثابت حرف *get_winsock_error(صحيح err, حرف *buf, size_t len)
{
  VERBOSE(ثابت حرف *p);

  𐡄𐡍(!len)
    أرجع عدم;

  *buf = '\0';

#ifndef CURLVERBOSE
  (فراغ)err;
  أرجع عدم;
#وإلا
  بدل(err) {
  حالة WSAEINTR:
    p = "Call interrupted";
    اكسر;
  حالة WSAEBADF:
    p = "Bad file";
    اكسر;
  حالة WSAEACCES:
    p = "Bad access";
    اكسر;
  حالة WSAEFAULT:
    p = "Bad argument";
    اكسر;
  حالة WSAEINVAL:
    p = "Invalid arguments";
    اكسر;
  حالة WSAEMFILE:
    p = "Out of file descriptors";
    اكسر;
  حالة WSAEWOULDBLOCK:
    p = "Call would block";
    اكسر;
  حالة WSAEINPROGRESS:
  حالة WSAEALREADY:
    p = "Blocking call in progress";
    اكسر;
  حالة WSAENOTSOCK:
    p = "Descriptor is not a socket";
    اكسر;
  حالة WSAEDESTADDRREQ:
    p = "Need destination address";
    اكسر;
  حالة WSAEMSGSIZE:
    p = "Bad message size";
    اكسر;
  حالة WSAEPROTOTYPE:
    p = "Bad protocol";
    اكسر;
  حالة WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    اكسر;
  حالة WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    اكسر;
  حالة WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    اكسر;
  حالة WSAEOPNOTSUPP:
    p = "Operation not supported";
    اكسر;
  حالة WSAEAFNOSUPPORT:
    p = "Address family not supported";
    اكسر;
  حالة WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    اكسر;
  حالة WSAEADDRINUSE:
    p = "Address already in use";
    اكسر;
  حالة WSAEADDRNOTAVAIL:
    p = "Address not available";
    اكسر;
  حالة WSAENETDOWN:
    p = "Network down";
    اكسر;
  حالة WSAENETUNREACH:
    p = "Network unreachable";
    اكسر;
  حالة WSAENETRESET:
    p = "Network has been reset";
    اكسر;
  حالة WSAECONNABORTED:
    p = "Connection was aborted";
    اكسر;
  حالة WSAECONNRESET:
    p = "Connection was reset";
    اكسر;
  حالة WSAENOBUFS:
    p = "No buffer space";
    اكسر;
  حالة WSAEISCONN:
    p = "Socket is already connected";
    اكسر;
  حالة WSAENOTCONN:
    p = "Socket is not connected";
    اكسر;
  حالة WSAESHUTDOWN:
    p = "Socket has been shut down";
    اكسر;
  حالة WSAETOOMANYREFS:
    p = "Too many references";
    اكسر;
  حالة WSAETIMEDOUT:
    p = "Timed out";
    اكسر;
  حالة WSAECONNREFUSED:
    p = "Connection refused";
    اكسر;
  حالة WSAELOOP:
    p = "Loop??";
    اكسر;
  حالة WSAENAMETOOLONG:
    p = "Name too long";
    اكسر;
  حالة WSAEHOSTDOWN:
    p = "Host down";
    اكسر;
  حالة WSAEHOSTUNREACH:
    p = "Host unreachable";
    اكسر;
  حالة WSAENOTEMPTY:
    p = "Not empty";
    اكسر;
  حالة WSAEPROCLIM:
    p = "Process limit reached";
    اكسر;
  حالة WSAEUSERS:
    p = "Too many users";
    اكسر;
  حالة WSAEDQUOT:
    p = "Bad quota";
    اكسر;
  حالة WSAESTALE:
    p = "Something is stale";
    اكسر;
  حالة WSAEREMOTE:
    p = "Remote error";
    اكسر;
  حالة WSAEDISCON:
    p = "Disconnected";
    اكسر;
    /* Extended Winsock errors */
  حالة WSASYSNOTREADY:
    p = "Winsock library is not ready";
    اكسر;
  حالة WSANOTINITIALISED:
    p = "Winsock library not initialized";
    اكسر;
  حالة WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    اكسر;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  حالة WSAHOST_NOT_FOUND:
    p = "Host not found";
    اكسر;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  حالة WSATRY_AGAIN:
    p = "Host not found, try again";
    اكسر;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  حالة WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    اكسر;

    /* Valid name, no data record of requested type */
  حالة WSANO_DATA:
    p = "No data record of requested type";
    اكسر;

  افتراضي:
    أرجع عدم;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  أرجع buf;
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
ثابت حرف *curlx_strerror(صحيح err, حرف *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  صحيح old_errno = errno;
  حرف *p;

  𐡄𐡍(!buflen)
    أرجع عدم;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  𐡄𐡍((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned صحيح)err);
#وإلا /* !_WIN32 */

#𐡄𐡍 defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  𐡄𐡍(strerror_r(err, buf, buflen) &&
     buflen > حجم("Unknown error ") + 20) {
    𐡄𐡍(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#وإلاإذا defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    حرف buffer[256];
    ثابت حرف *msg = strerror_r(err, buffer, حجم(buffer));
    𐡄𐡍(msg && buflen > 1)
      SNPRINTF(buf, buflen, "%s", msg);
    وإلا 𐡄𐡍(buflen > حجم("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#وإلا
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ثابت حرف *msg = strerror(err);
    𐡄𐡍(msg && buflen > 1)
      SNPRINTF(buf, buflen, "%s", msg);
    وإلا 𐡄𐡍(buflen > حجم("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  𐡄𐡍(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  𐡄𐡍(p && (p - buf) >= 1)
    *p = '\0';

  𐡄𐡍(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  𐡄𐡍(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  أرجع buf;
}
