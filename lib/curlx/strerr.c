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
#شامل "curl_setup.h"

#ifdef HAVE_STRERROR_R
#  اگر (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#شامل "curlx/winapi.h"
#شامل "curlx/snprintf.h"
#شامل "curlx/strerr.h"
#شامل "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ایستا ثابت نویسه *get_winsock_error(صحیح err, نویسه *buf, size_t len)
{
  VERBOSE(ثابت نویسه *p);

  اگر(!len)
    بازگشت پوچ;

  *buf = '\0';

#ifndef CURLVERBOSE
  (تهی)err;
  بازگشت پوچ;
#وگرنه
  انتخاب(err) {
  حالت WSAEINTR:
    p = "Call interrupted";
    بشکن;
  حالت WSAEBADF:
    p = "Bad file";
    بشکن;
  حالت WSAEACCES:
    p = "Bad access";
    بشکن;
  حالت WSAEFAULT:
    p = "Bad argument";
    بشکن;
  حالت WSAEINVAL:
    p = "Invalid arguments";
    بشکن;
  حالت WSAEMFILE:
    p = "Out of file descriptors";
    بشکن;
  حالت WSAEWOULDBLOCK:
    p = "Call would block";
    بشکن;
  حالت WSAEINPROGRESS:
  حالت WSAEALREADY:
    p = "Blocking call in progress";
    بشکن;
  حالت WSAENOTSOCK:
    p = "Descriptor is not a socket";
    بشکن;
  حالت WSAEDESTADDRREQ:
    p = "Need destination address";
    بشکن;
  حالت WSAEMSGSIZE:
    p = "Bad message size";
    بشکن;
  حالت WSAEPROTOTYPE:
    p = "Bad protocol";
    بشکن;
  حالت WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    بشکن;
  حالت WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    بشکن;
  حالت WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    بشکن;
  حالت WSAEOPNOTSUPP:
    p = "Operation not supported";
    بشکن;
  حالت WSAEAFNOSUPPORT:
    p = "Address family not supported";
    بشکن;
  حالت WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    بشکن;
  حالت WSAEADDRINUSE:
    p = "Address already in use";
    بشکن;
  حالت WSAEADDRNOTAVAIL:
    p = "Address not available";
    بشکن;
  حالت WSAENETDOWN:
    p = "Network down";
    بشکن;
  حالت WSAENETUNREACH:
    p = "Network unreachable";
    بشکن;
  حالت WSAENETRESET:
    p = "Network has been reset";
    بشکن;
  حالت WSAECONNABORTED:
    p = "Connection was aborted";
    بشکن;
  حالت WSAECONNRESET:
    p = "Connection was reset";
    بشکن;
  حالت WSAENOBUFS:
    p = "No buffer space";
    بشکن;
  حالت WSAEISCONN:
    p = "Socket is already connected";
    بشکن;
  حالت WSAENOTCONN:
    p = "Socket is not connected";
    بشکن;
  حالت WSAESHUTDOWN:
    p = "Socket has been shut down";
    بشکن;
  حالت WSAETOOMANYREFS:
    p = "Too many references";
    بشکن;
  حالت WSAETIMEDOUT:
    p = "Timed out";
    بشکن;
  حالت WSAECONNREFUSED:
    p = "Connection refused";
    بشکن;
  حالت WSAELOOP:
    p = "Loop??";
    بشکن;
  حالت WSAENAMETOOLONG:
    p = "Name too long";
    بشکن;
  حالت WSAEHOSTDOWN:
    p = "Host down";
    بشکن;
  حالت WSAEHOSTUNREACH:
    p = "Host unreachable";
    بشکن;
  حالت WSAENOTEMPTY:
    p = "Not empty";
    بشکن;
  حالت WSAEPROCLIM:
    p = "Process limit reached";
    بشکن;
  حالت WSAEUSERS:
    p = "Too many users";
    بشکن;
  حالت WSAEDQUOT:
    p = "Bad quota";
    بشکن;
  حالت WSAESTALE:
    p = "Something is stale";
    بشکن;
  حالت WSAEREMOTE:
    p = "Remote error";
    بشکن;
  حالت WSAEDISCON:
    p = "Disconnected";
    بشکن;
    /* Extended Winsock errors */
  حالت WSASYSNOTREADY:
    p = "Winsock library is not ready";
    بشکن;
  حالت WSANOTINITIALISED:
    p = "Winsock library not initialized";
    بشکن;
  حالت WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    بشکن;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  حالت WSAHOST_NOT_FOUND:
    p = "Host not found";
    بشکن;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  حالت WSATRY_AGAIN:
    p = "Host not found, try again";
    بشکن;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  حالت WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    بشکن;

    /* Valid name, no data record of requested type */
  حالت WSANO_DATA:
    p = "No data record of requested type";
    بشکن;

  پیشفرض:
    بازگشت پوچ;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  بازگشت buf;
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
ثابت نویسه *curlx_strerror(صحیح err, نویسه *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  صحیح old_errno = errno;
  نویسه *p;

  اگر(!buflen)
    بازگشت پوچ;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  اگر((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned صحیح)err);
#وگرنه /* !_WIN32 */

#اگر defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  اگر(strerror_r(err, buf, buflen) &&
     buflen > اندازه("Unknown error ") + 20) {
    اگر(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#وگرنهاگر defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    نویسه buffer[256];
    ثابت نویسه *پیام = strerror_r(err, buffer, اندازه(buffer));
    اگر(پیام && buflen > 1)
      SNPRINTF(buf, buflen, "%s", پیام);
    وگرنه اگر(buflen > اندازه("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#وگرنه
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ثابت نویسه *پیام = strerror(err);
    اگر(پیام && buflen > 1)
      SNPRINTF(buf, buflen, "%s", پیام);
    وگرنه اگر(buflen > اندازه("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  اگر(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  اگر(p && (p - buf) >= 1)
    *p = '\0';

  اگر(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  اگر(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  بازگشت buf;
}
