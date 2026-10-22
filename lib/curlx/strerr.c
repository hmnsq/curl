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
#அந்தர்பாவ "curl_setup.h"

#ifdef HAVE_STRERROR_R
#  எனில் (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#அந்தர்பாவ "curlx/winapi.h"
#அந்தர்பாவ "curlx/snprintf.h"
#அந்தர்பாவ "curlx/strerr.h"
#அந்தர்பாவ "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
நிலையான மாறிலி எழுத்து *get_winsock_error(முழு err, எழுத்து *buf, size_t len)
{
  VERBOSE(மாறிலி எழுத்து *p);

  எனில்(!len)
    திரும்பு வெற்று;

  *buf = '\0';

#ifndef CURLVERBOSE
  (வெறுமை)err;
  திரும்பு வெற்று;
#இல்லையெனில்
  தேர்வு(err) {
  நிலை WSAEINTR:
    p = "Call interrupted";
    நிறுத்து;
  நிலை WSAEBADF:
    p = "Bad file";
    நிறுத்து;
  நிலை WSAEACCES:
    p = "Bad access";
    நிறுத்து;
  நிலை WSAEFAULT:
    p = "Bad argument";
    நிறுத்து;
  நிலை WSAEINVAL:
    p = "Invalid arguments";
    நிறுத்து;
  நிலை WSAEMFILE:
    p = "Out of file descriptors";
    நிறுத்து;
  நிலை WSAEWOULDBLOCK:
    p = "Call would block";
    நிறுத்து;
  நிலை WSAEINPROGRESS:
  நிலை WSAEALREADY:
    p = "Blocking call in progress";
    நிறுத்து;
  நிலை WSAENOTSOCK:
    p = "Descriptor is not a socket";
    நிறுத்து;
  நிலை WSAEDESTADDRREQ:
    p = "Need destination address";
    நிறுத்து;
  நிலை WSAEMSGSIZE:
    p = "Bad message size";
    நிறுத்து;
  நிலை WSAEPROTOTYPE:
    p = "Bad protocol";
    நிறுத்து;
  நிலை WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    நிறுத்து;
  நிலை WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    நிறுத்து;
  நிலை WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    நிறுத்து;
  நிலை WSAEOPNOTSUPP:
    p = "Operation not supported";
    நிறுத்து;
  நிலை WSAEAFNOSUPPORT:
    p = "Address family not supported";
    நிறுத்து;
  நிலை WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    நிறுத்து;
  நிலை WSAEADDRINUSE:
    p = "Address already in use";
    நிறுத்து;
  நிலை WSAEADDRNOTAVAIL:
    p = "Address not available";
    நிறுத்து;
  நிலை WSAENETDOWN:
    p = "Network down";
    நிறுத்து;
  நிலை WSAENETUNREACH:
    p = "Network unreachable";
    நிறுத்து;
  நிலை WSAENETRESET:
    p = "Network has been reset";
    நிறுத்து;
  நிலை WSAECONNABORTED:
    p = "Connection was aborted";
    நிறுத்து;
  நிலை WSAECONNRESET:
    p = "Connection was reset";
    நிறுத்து;
  நிலை WSAENOBUFS:
    p = "No buffer space";
    நிறுத்து;
  நிலை WSAEISCONN:
    p = "Socket is already connected";
    நிறுத்து;
  நிலை WSAENOTCONN:
    p = "Socket is not connected";
    நிறுத்து;
  நிலை WSAESHUTDOWN:
    p = "Socket has been shut down";
    நிறுத்து;
  நிலை WSAETOOMANYREFS:
    p = "Too many references";
    நிறுத்து;
  நிலை WSAETIMEDOUT:
    p = "Timed out";
    நிறுத்து;
  நிலை WSAECONNREFUSED:
    p = "Connection refused";
    நிறுத்து;
  நிலை WSAELOOP:
    p = "Loop??";
    நிறுத்து;
  நிலை WSAENAMETOOLONG:
    p = "Name too long";
    நிறுத்து;
  நிலை WSAEHOSTDOWN:
    p = "Host down";
    நிறுத்து;
  நிலை WSAEHOSTUNREACH:
    p = "Host unreachable";
    நிறுத்து;
  நிலை WSAENOTEMPTY:
    p = "Not empty";
    நிறுத்து;
  நிலை WSAEPROCLIM:
    p = "Process limit reached";
    நிறுத்து;
  நிலை WSAEUSERS:
    p = "Too many users";
    நிறுத்து;
  நிலை WSAEDQUOT:
    p = "Bad quota";
    நிறுத்து;
  நிலை WSAESTALE:
    p = "Something is stale";
    நிறுத்து;
  நிலை WSAEREMOTE:
    p = "Remote error";
    நிறுத்து;
  நிலை WSAEDISCON:
    p = "Disconnected";
    நிறுத்து;
    /* Extended Winsock errors */
  நிலை WSASYSNOTREADY:
    p = "Winsock library is not ready";
    நிறுத்து;
  நிலை WSANOTINITIALISED:
    p = "Winsock library not initialized";
    நிறுத்து;
  நிலை WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    நிறுத்து;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  நிலை WSAHOST_NOT_FOUND:
    p = "Host not found";
    நிறுத்து;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  நிலை WSATRY_AGAIN:
    p = "Host not found, try again";
    நிறுத்து;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  நிலை WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    நிறுத்து;

    /* Valid name, no data record of requested type */
  நிலை WSANO_DATA:
    p = "No data record of requested type";
    நிறுத்து;

  இயல்பு:
    திரும்பு வெற்று;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  திரும்பு buf;
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
மாறிலி எழுத்து *curlx_strerror(முழு err, எழுத்து *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  முழு old_errno = errno;
  எழுத்து *p;

  எனில்(!buflen)
    திரும்பு வெற்று;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  எனில்((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned முழு)err);
#இல்லையெனில் /* !_WIN32 */

#எனில் defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  எனில்(strerror_r(err, buf, buflen) &&
     buflen > அளவு("Unknown error ") + 20) {
    எனில்(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#அல்லதெனில் defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    எழுத்து buffer[256];
    மாறிலி எழுத்து *செய்தி = strerror_r(err, buffer, அளவு(buffer));
    எனில்(செய்தி && buflen > 1)
      SNPRINTF(buf, buflen, "%s", செய்தி);
    இல்லையெனில் எனில்(buflen > அளவு("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#இல்லையெனில்
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    மாறிலி எழுத்து *செய்தி = strerror(err);
    எனில்(செய்தி && buflen > 1)
      SNPRINTF(buf, buflen, "%s", செய்தி);
    இல்லையெனில் எனில்(buflen > அளவு("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  எனில்(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  எனில்(p && (p - buf) >= 1)
    *p = '\0';

  எனில்(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  எனில்(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  திரும்பு buf;
}
