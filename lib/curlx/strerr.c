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
#অন্তর্ভাব "curl_setup.h"

#ifdef HAVE_STRERROR_R
#  যদি (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#অন্তর্ভাব "curlx/winapi.h"
#অন্তর্ভাব "curlx/snprintf.h"
#অন্তর্ভাব "curlx/strerr.h"
#অন্তর্ভাব "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
স্থির ধ্রুব বর্ণ *get_winsock_error(পূর্ণাঙ্ক err, বর্ণ *buf, size_t len)
{
  VERBOSE(ধ্রুব বর্ণ *p);

  যদি(!len)
    প্রত্যাগম রিক্ত;

  *buf = '\0';

#ifndef CURLVERBOSE
  (শূন্য)err;
  প্রত্যাগম রিক্ত;
#অন্যথা
  বিকল্প(err) {
  পক্ষ WSAEINTR:
    p = "Call interrupted";
    ভঙ্গ;
  পক্ষ WSAEBADF:
    p = "Bad file";
    ভঙ্গ;
  পক্ষ WSAEACCES:
    p = "Bad access";
    ভঙ্গ;
  পক্ষ WSAEFAULT:
    p = "Bad argument";
    ভঙ্গ;
  পক্ষ WSAEINVAL:
    p = "Invalid arguments";
    ভঙ্গ;
  পক্ষ WSAEMFILE:
    p = "Out of file descriptors";
    ভঙ্গ;
  পক্ষ WSAEWOULDBLOCK:
    p = "Call would block";
    ভঙ্গ;
  পক্ষ WSAEINPROGRESS:
  পক্ষ WSAEALREADY:
    p = "Blocking call in progress";
    ভঙ্গ;
  পক্ষ WSAENOTSOCK:
    p = "Descriptor is not a socket";
    ভঙ্গ;
  পক্ষ WSAEDESTADDRREQ:
    p = "Need destination address";
    ভঙ্গ;
  পক্ষ WSAEMSGSIZE:
    p = "Bad message size";
    ভঙ্গ;
  পক্ষ WSAEPROTOTYPE:
    p = "Bad protocol";
    ভঙ্গ;
  পক্ষ WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    ভঙ্গ;
  পক্ষ WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    ভঙ্গ;
  পক্ষ WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    ভঙ্গ;
  পক্ষ WSAEOPNOTSUPP:
    p = "Operation not supported";
    ভঙ্গ;
  পক্ষ WSAEAFNOSUPPORT:
    p = "Address family not supported";
    ভঙ্গ;
  পক্ষ WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    ভঙ্গ;
  পক্ষ WSAEADDRINUSE:
    p = "Address already in use";
    ভঙ্গ;
  পক্ষ WSAEADDRNOTAVAIL:
    p = "Address not available";
    ভঙ্গ;
  পক্ষ WSAENETDOWN:
    p = "Network down";
    ভঙ্গ;
  পক্ষ WSAENETUNREACH:
    p = "Network unreachable";
    ভঙ্গ;
  পক্ষ WSAENETRESET:
    p = "Network has been reset";
    ভঙ্গ;
  পক্ষ WSAECONNABORTED:
    p = "Connection was aborted";
    ভঙ্গ;
  পক্ষ WSAECONNRESET:
    p = "Connection was reset";
    ভঙ্গ;
  পক্ষ WSAENOBUFS:
    p = "No buffer space";
    ভঙ্গ;
  পক্ষ WSAEISCONN:
    p = "Socket is already connected";
    ভঙ্গ;
  পক্ষ WSAENOTCONN:
    p = "Socket is not connected";
    ভঙ্গ;
  পক্ষ WSAESHUTDOWN:
    p = "Socket has been shut down";
    ভঙ্গ;
  পক্ষ WSAETOOMANYREFS:
    p = "Too many references";
    ভঙ্গ;
  পক্ষ WSAETIMEDOUT:
    p = "Timed out";
    ভঙ্গ;
  পক্ষ WSAECONNREFUSED:
    p = "Connection refused";
    ভঙ্গ;
  পক্ষ WSAELOOP:
    p = "Loop??";
    ভঙ্গ;
  পক্ষ WSAENAMETOOLONG:
    p = "Name too long";
    ভঙ্গ;
  পক্ষ WSAEHOSTDOWN:
    p = "Host down";
    ভঙ্গ;
  পক্ষ WSAEHOSTUNREACH:
    p = "Host unreachable";
    ভঙ্গ;
  পক্ষ WSAENOTEMPTY:
    p = "Not empty";
    ভঙ্গ;
  পক্ষ WSAEPROCLIM:
    p = "Process limit reached";
    ভঙ্গ;
  পক্ষ WSAEUSERS:
    p = "Too many users";
    ভঙ্গ;
  পক্ষ WSAEDQUOT:
    p = "Bad quota";
    ভঙ্গ;
  পক্ষ WSAESTALE:
    p = "Something is stale";
    ভঙ্গ;
  পক্ষ WSAEREMOTE:
    p = "Remote error";
    ভঙ্গ;
  পক্ষ WSAEDISCON:
    p = "Disconnected";
    ভঙ্গ;
    /* Extended Winsock errors */
  পক্ষ WSASYSNOTREADY:
    p = "Winsock library is not ready";
    ভঙ্গ;
  পক্ষ WSANOTINITIALISED:
    p = "Winsock library not initialized";
    ভঙ্গ;
  পক্ষ WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    ভঙ্গ;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  পক্ষ WSAHOST_NOT_FOUND:
    p = "Host not found";
    ভঙ্গ;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  পক্ষ WSATRY_AGAIN:
    p = "Host not found, try again";
    ভঙ্গ;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  পক্ষ WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    ভঙ্গ;

    /* Valid name, no data record of requested type */
  পক্ষ WSANO_DATA:
    p = "No data record of requested type";
    ভঙ্গ;

  স্বভাব:
    প্রত্যাগম রিক্ত;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  প্রত্যাগম buf;
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
ধ্রুব বর্ণ *curlx_strerror(পূর্ণাঙ্ক err, বর্ণ *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  পূর্ণাঙ্ক old_errno = errno;
  বর্ণ *p;

  যদি(!buflen)
    প্রত্যাগম রিক্ত;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  যদি((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned পূর্ণাঙ্ক)err);
#অন্যথা /* !_WIN32 */

#যদি defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  যদি(strerror_r(err, buf, buflen) &&
     buflen > পরিমাণ("Unknown error ") + 20) {
    যদি(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#নোচেদ্যদি defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    বর্ণ buffer[256];
    ধ্রুব বর্ণ *বার্তা = strerror_r(err, buffer, পরিমাণ(buffer));
    যদি(বার্তা && buflen > 1)
      SNPRINTF(buf, buflen, "%s", বার্তা);
    অন্যথা যদি(buflen > পরিমাণ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#অন্যথা
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ধ্রুব বর্ণ *বার্তা = strerror(err);
    যদি(বার্তা && buflen > 1)
      SNPRINTF(buf, buflen, "%s", বার্তা);
    অন্যথা যদি(buflen > পরিমাণ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  যদি(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  যদি(p && (p - buf) >= 1)
    *p = '\0';

  যদি(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  যদি(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  প্রত্যাগম buf;
}
