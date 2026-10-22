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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef HAVE_STRERROR_R
#  എങ്കിൽ (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#അന്തര്ഭാവ "curlx/winapi.h"
#അന്തര്ഭാവ "curlx/snprintf.h"
#അന്തര്ഭാവ "curlx/strerr.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം *get_winsock_error(പൂർണ്ണസംഖ്യ err, അക്ഷരം *buf, size_t len)
{
  VERBOSE(സ്ഥിരം അക്ഷരം *p);

  എങ്കിൽ(!len)
    തിരികെ ശൂന്യ;

  *buf = '\0';

#ifndef CURLVERBOSE
  (ശൂന്യം)err;
  തിരികെ ശൂന്യ;
#അല്ലെങ്കിൽ
  തിരഞ്ഞെടുപ്പ്(err) {
  അവസ്ഥ WSAEINTR:
    p = "Call interrupted";
    നിർത്തുക;
  അവസ്ഥ WSAEBADF:
    p = "Bad file";
    നിർത്തുക;
  അവസ്ഥ WSAEACCES:
    p = "Bad access";
    നിർത്തുക;
  അവസ്ഥ WSAEFAULT:
    p = "Bad argument";
    നിർത്തുക;
  അവസ്ഥ WSAEINVAL:
    p = "Invalid arguments";
    നിർത്തുക;
  അവസ്ഥ WSAEMFILE:
    p = "Out of file descriptors";
    നിർത്തുക;
  അവസ്ഥ WSAEWOULDBLOCK:
    p = "Call would block";
    നിർത്തുക;
  അവസ്ഥ WSAEINPROGRESS:
  അവസ്ഥ WSAEALREADY:
    p = "Blocking call in progress";
    നിർത്തുക;
  അവസ്ഥ WSAENOTSOCK:
    p = "Descriptor is not a socket";
    നിർത്തുക;
  അവസ്ഥ WSAEDESTADDRREQ:
    p = "Need destination address";
    നിർത്തുക;
  അവസ്ഥ WSAEMSGSIZE:
    p = "Bad message size";
    നിർത്തുക;
  അവസ്ഥ WSAEPROTOTYPE:
    p = "Bad protocol";
    നിർത്തുക;
  അവസ്ഥ WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    നിർത്തുക;
  അവസ്ഥ WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    നിർത്തുക;
  അവസ്ഥ WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    നിർത്തുക;
  അവസ്ഥ WSAEOPNOTSUPP:
    p = "Operation not supported";
    നിർത്തുക;
  അവസ്ഥ WSAEAFNOSUPPORT:
    p = "Address family not supported";
    നിർത്തുക;
  അവസ്ഥ WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    നിർത്തുക;
  അവസ്ഥ WSAEADDRINUSE:
    p = "Address already in use";
    നിർത്തുക;
  അവസ്ഥ WSAEADDRNOTAVAIL:
    p = "Address not available";
    നിർത്തുക;
  അവസ്ഥ WSAENETDOWN:
    p = "Network down";
    നിർത്തുക;
  അവസ്ഥ WSAENETUNREACH:
    p = "Network unreachable";
    നിർത്തുക;
  അവസ്ഥ WSAENETRESET:
    p = "Network has been reset";
    നിർത്തുക;
  അവസ്ഥ WSAECONNABORTED:
    p = "Connection was aborted";
    നിർത്തുക;
  അവസ്ഥ WSAECONNRESET:
    p = "Connection was reset";
    നിർത്തുക;
  അവസ്ഥ WSAENOBUFS:
    p = "No buffer space";
    നിർത്തുക;
  അവസ്ഥ WSAEISCONN:
    p = "Socket is already connected";
    നിർത്തുക;
  അവസ്ഥ WSAENOTCONN:
    p = "Socket is not connected";
    നിർത്തുക;
  അവസ്ഥ WSAESHUTDOWN:
    p = "Socket has been shut down";
    നിർത്തുക;
  അവസ്ഥ WSAETOOMANYREFS:
    p = "Too many references";
    നിർത്തുക;
  അവസ്ഥ WSAETIMEDOUT:
    p = "Timed out";
    നിർത്തുക;
  അവസ്ഥ WSAECONNREFUSED:
    p = "Connection refused";
    നിർത്തുക;
  അവസ്ഥ WSAELOOP:
    p = "Loop??";
    നിർത്തുക;
  അവസ്ഥ WSAENAMETOOLONG:
    p = "Name too long";
    നിർത്തുക;
  അവസ്ഥ WSAEHOSTDOWN:
    p = "Host down";
    നിർത്തുക;
  അവസ്ഥ WSAEHOSTUNREACH:
    p = "Host unreachable";
    നിർത്തുക;
  അവസ്ഥ WSAENOTEMPTY:
    p = "Not empty";
    നിർത്തുക;
  അവസ്ഥ WSAEPROCLIM:
    p = "Process limit reached";
    നിർത്തുക;
  അവസ്ഥ WSAEUSERS:
    p = "Too many users";
    നിർത്തുക;
  അവസ്ഥ WSAEDQUOT:
    p = "Bad quota";
    നിർത്തുക;
  അവസ്ഥ WSAESTALE:
    p = "Something is stale";
    നിർത്തുക;
  അവസ്ഥ WSAEREMOTE:
    p = "Remote error";
    നിർത്തുക;
  അവസ്ഥ WSAEDISCON:
    p = "Disconnected";
    നിർത്തുക;
    /* Extended Winsock errors */
  അവസ്ഥ WSASYSNOTREADY:
    p = "Winsock library is not ready";
    നിർത്തുക;
  അവസ്ഥ WSANOTINITIALISED:
    p = "Winsock library not initialized";
    നിർത്തുക;
  അവസ്ഥ WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    നിർത്തുക;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  അവസ്ഥ WSAHOST_NOT_FOUND:
    p = "Host not found";
    നിർത്തുക;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  അവസ്ഥ WSATRY_AGAIN:
    p = "Host not found, try again";
    നിർത്തുക;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  അവസ്ഥ WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    നിർത്തുക;

    /* Valid name, no data record of requested type */
  അവസ്ഥ WSANO_DATA:
    p = "No data record of requested type";
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    തിരികെ ശൂന്യ;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  തിരികെ buf;
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
സ്ഥിരം അക്ഷരം *curlx_strerror(പൂർണ്ണസംഖ്യ err, അക്ഷരം *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  പൂർണ്ണസംഖ്യ old_errno = errno;
  അക്ഷരം *p;

  എങ്കിൽ(!buflen)
    തിരികെ ശൂന്യ;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  എങ്കിൽ((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned പൂർണ്ണസംഖ്യ)err);
#അല്ലെങ്കിൽ /* !_WIN32 */

#എങ്കിൽ defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  എങ്കിൽ(strerror_r(err, buf, buflen) &&
     buflen > വലുപ്പം("Unknown error ") + 20) {
    എങ്കിൽ(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    അക്ഷരം buffer[256];
    സ്ഥിരം അക്ഷരം *സന്ദേശം = strerror_r(err, buffer, വലുപ്പം(buffer));
    എങ്കിൽ(സന്ദേശം && buflen > 1)
      SNPRINTF(buf, buflen, "%s", സന്ദേശം);
    അല്ലെങ്കിൽ എങ്കിൽ(buflen > വലുപ്പം("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#അല്ലെങ്കിൽ
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    സ്ഥിരം അക്ഷരം *സന്ദേശം = strerror(err);
    എങ്കിൽ(സന്ദേശം && buflen > 1)
      SNPRINTF(buf, buflen, "%s", സന്ദേശം);
    അല്ലെങ്കിൽ എങ്കിൽ(buflen > വലുപ്പം("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  എങ്കിൽ(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  എങ്കിൽ(p && (p - buf) >= 1)
    *p = '\0';

  എങ്കിൽ(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  എങ്കിൽ(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  തിരികെ buf;
}
