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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_STRERROR_R
#  ਜੇ (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/winapi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/snprintf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *get_winsock_error(ਪੂਰਨ err, ਅੱਖਰ *buf, size_t len)
{
  VERBOSE(ਅਚਲ ਅੱਖਰ *p);

  ਜੇ(!len)
    ਵਾਪਸ ਨਿਰਾ;

  *buf = '\0';

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  (ਖਾਲੀ)err;
  ਵਾਪਸ ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
  ਚੋਣ(err) {
  ਹਾਲਤ WSAEINTR:
    p = "Call interrupted";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEBADF:
    p = "Bad file";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEACCES:
    p = "Bad access";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEFAULT:
    p = "Bad argument";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEINVAL:
    p = "Invalid arguments";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEMFILE:
    p = "Out of file descriptors";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEWOULDBLOCK:
    p = "Call would block";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEINPROGRESS:
  ਹਾਲਤ WSAEALREADY:
    p = "Blocking call in progress";
    ਰੋਕੋ;
  ਹਾਲਤ WSAENOTSOCK:
    p = "Descriptor is not a socket";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEDESTADDRREQ:
    p = "Need destination address";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEMSGSIZE:
    p = "Bad message size";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEPROTOTYPE:
    p = "Bad protocol";
    ਰੋਕੋ;
  ਹਾਲਤ WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    ਰੋਕੋ;
  ਹਾਲਤ WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEOPNOTSUPP:
    p = "Operation not supported";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEAFNOSUPPORT:
    p = "Address family not supported";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEADDRINUSE:
    p = "Address already in use";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEADDRNOTAVAIL:
    p = "Address not available";
    ਰੋਕੋ;
  ਹਾਲਤ WSAENETDOWN:
    p = "Network down";
    ਰੋਕੋ;
  ਹਾਲਤ WSAENETUNREACH:
    p = "Network unreachable";
    ਰੋਕੋ;
  ਹਾਲਤ WSAENETRESET:
    p = "Network has been reset";
    ਰੋਕੋ;
  ਹਾਲਤ WSAECONNABORTED:
    p = "Connection was aborted";
    ਰੋਕੋ;
  ਹਾਲਤ WSAECONNRESET:
    p = "Connection was reset";
    ਰੋਕੋ;
  ਹਾਲਤ WSAENOBUFS:
    p = "No buffer space";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEISCONN:
    p = "Socket is already connected";
    ਰੋਕੋ;
  ਹਾਲਤ WSAENOTCONN:
    p = "Socket is not connected";
    ਰੋਕੋ;
  ਹਾਲਤ WSAESHUTDOWN:
    p = "Socket has been shut down";
    ਰੋਕੋ;
  ਹਾਲਤ WSAETOOMANYREFS:
    p = "Too many references";
    ਰੋਕੋ;
  ਹਾਲਤ WSAETIMEDOUT:
    p = "Timed out";
    ਰੋਕੋ;
  ਹਾਲਤ WSAECONNREFUSED:
    p = "Connection refused";
    ਰੋਕੋ;
  ਹਾਲਤ WSAELOOP:
    p = "Loop??";
    ਰੋਕੋ;
  ਹਾਲਤ WSAENAMETOOLONG:
    p = "Name too long";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEHOSTDOWN:
    p = "Host down";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEHOSTUNREACH:
    p = "Host unreachable";
    ਰੋਕੋ;
  ਹਾਲਤ WSAENOTEMPTY:
    p = "Not empty";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEPROCLIM:
    p = "Process limit reached";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEUSERS:
    p = "Too many users";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEDQUOT:
    p = "Bad quota";
    ਰੋਕੋ;
  ਹਾਲਤ WSAESTALE:
    p = "Something is stale";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEREMOTE:
    p = "Remote error";
    ਰੋਕੋ;
  ਹਾਲਤ WSAEDISCON:
    p = "Disconnected";
    ਰੋਕੋ;
    /* Extended Winsock errors */
  ਹਾਲਤ WSASYSNOTREADY:
    p = "Winsock library is not ready";
    ਰੋਕੋ;
  ਹਾਲਤ WSANOTINITIALISED:
    p = "Winsock library not initialized";
    ਰੋਕੋ;
  ਹਾਲਤ WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    ਰੋਕੋ;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  ਹਾਲਤ WSAHOST_NOT_ਮਿਲਿਆ:
    p = "Host not found";
    ਰੋਕੋ;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  ਹਾਲਤ WSATRY_AGAIN:
    p = "Host not found, try again";
    ਰੋਕੋ;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  ਹਾਲਤ WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    ਰੋਕੋ;

    /* Valid name, no data record of requested type */
  ਹਾਲਤ WSANO_ਅੰਕੜੇ:
    p = "No data record of requested type";
    ਰੋਕੋ;

  ਮੂਲ:
    ਵਾਪਸ ਨਿਰਾ;
  }
  curlx_strcopy(buf, len, p, ਲੜੀ_ਲੰਬਾਈ(p));
  ਵਾਪਸ buf;
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ /* USE_WINSOCK */

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
ਅਚਲ ਅੱਖਰ *curlx_strerror(ਪੂਰਨ err, ਅੱਖਰ *buf, size_t buflen)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  DWORD old_win_err = Get_ਆਖਰੀ_Error();
#ਜੇ_ਅੰਤ
  ਪੂਰਨ old_errno = errno;
  ਅੱਖਰ *p;

  ਜੇ(!buflen)
    ਵਾਪਸ ਨਿਰਾ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _WIN32
  DEBUGASSERT(err >= 0);
#ਜੇ_ਅੰਤ

  *buf = '\0';

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  ਜੇ((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#ਜੇ_ਅੰਤ
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)err);
#ਨਹੀਂਤਾਂ /* !_WIN32 */

#ਜੇ defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  ਜੇ(strerror_r(err, buf, buflen) &&
     buflen > ਆਕਾਰ("Unknown error ") + 20) {
    ਜੇ(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ਜਾਂਜੇ defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    ਅੱਖਰ ਬਫਰ[256];
    ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ = strerror_r(err, ਬਫਰ, ਆਕਾਰ(ਬਫਰ));
    ਜੇ(ਸੁਨੇਹਾ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", ਸੁਨੇਹਾ);
    ਨਹੀਂਤਾਂ ਜੇ(buflen > ਆਕਾਰ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ਨਹੀਂਤਾਂ
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ = strerror(err);
    ਜੇ(ਸੁਨੇਹਾ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", ਸੁਨੇਹਾ);
    ਨਹੀਂਤਾਂ ਜੇ(buflen > ਆਕਾਰ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  ਜੇ(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  ਜੇ(p && (p - buf) >= 1)
    *p = '\0';

  ਜੇ(errno != old_errno)
    errno = old_errno;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  ਜੇ(old_win_err != Get_ਆਖਰੀ_Error())
    ਸੈੱਟ_ਆਖਰੀ_Error(old_win_err);
#ਜੇ_ਅੰਤ

  ਵਾਪਸ buf;
}
