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
স্থির ধ্রুবক অক্ষর *get_winsock_error(পূর্ণসংখ্যা err, অক্ষর *buf, size_t len)
{
  VERBOSE(ধ্রুবক অক্ষর *p);

  যদি(!len)
    ফেরত ফাঁকা;

  *buf = '\0';

#ifndef CURLVERBOSE
  (শূন্য)err;
  ফেরত ফাঁকা;
#অন্যথা
  নির্বাচন(err) {
  ক্ষেত্র WSAEINTR:
    p = "Call interrupted";
    বিরতি;
  ক্ষেত্র WSAEBADF:
    p = "Bad file";
    বিরতি;
  ক্ষেত্র WSAEACCES:
    p = "Bad access";
    বিরতি;
  ক্ষেত্র WSAEFAULT:
    p = "Bad argument";
    বিরতি;
  ক্ষেত্র WSAEINVAL:
    p = "Invalid arguments";
    বিরতি;
  ক্ষেত্র WSAEMFILE:
    p = "Out of file descriptors";
    বিরতি;
  ক্ষেত্র WSAEWOULDBLOCK:
    p = "Call would block";
    বিরতি;
  ক্ষেত্র WSAEINPROGRESS:
  ক্ষেত্র WSAEALREADY:
    p = "Blocking call in progress";
    বিরতি;
  ক্ষেত্র WSAENOTSOCK:
    p = "Descriptor is not a socket";
    বিরতি;
  ক্ষেত্র WSAEDESTADDRREQ:
    p = "Need destination address";
    বিরতি;
  ক্ষেত্র WSAEMSGSIZE:
    p = "Bad message size";
    বিরতি;
  ক্ষেত্র WSAEPROTOTYPE:
    p = "Bad protocol";
    বিরতি;
  ক্ষেত্র WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    বিরতি;
  ক্ষেত্র WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    বিরতি;
  ক্ষেত্র WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    বিরতি;
  ক্ষেত্র WSAEOPNOTSUPP:
    p = "Operation not supported";
    বিরতি;
  ক্ষেত্র WSAEAFNOSUPPORT:
    p = "Address family not supported";
    বিরতি;
  ক্ষেত্র WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    বিরতি;
  ক্ষেত্র WSAEADDRINUSE:
    p = "Address already in use";
    বিরতি;
  ক্ষেত্র WSAEADDRNOTAVAIL:
    p = "Address not available";
    বিরতি;
  ক্ষেত্র WSAENETDOWN:
    p = "Network down";
    বিরতি;
  ক্ষেত্র WSAENETUNREACH:
    p = "Network unreachable";
    বিরতি;
  ক্ষেত্র WSAENETRESET:
    p = "Network has been reset";
    বিরতি;
  ক্ষেত্র WSAECONNABORTED:
    p = "Connection was aborted";
    বিরতি;
  ক্ষেত্র WSAECONNRESET:
    p = "Connection was reset";
    বিরতি;
  ক্ষেত্র WSAENOBUFS:
    p = "No buffer space";
    বিরতি;
  ক্ষেত্র WSAEISCONN:
    p = "Socket is already connected";
    বিরতি;
  ক্ষেত্র WSAENOTCONN:
    p = "Socket is not connected";
    বিরতি;
  ক্ষেত্র WSAESHUTDOWN:
    p = "Socket has been shut down";
    বিরতি;
  ক্ষেত্র WSAETOOMANYREFS:
    p = "Too many references";
    বিরতি;
  ক্ষেত্র WSAETIMEDOUT:
    p = "Timed out";
    বিরতি;
  ক্ষেত্র WSAECONNREFUSED:
    p = "Connection refused";
    বিরতি;
  ক্ষেত্র WSAELOOP:
    p = "Loop??";
    বিরতি;
  ক্ষেত্র WSAENAMETOOLONG:
    p = "Name too long";
    বিরতি;
  ক্ষেত্র WSAEHOSTDOWN:
    p = "Host down";
    বিরতি;
  ক্ষেত্র WSAEHOSTUNREACH:
    p = "Host unreachable";
    বিরতি;
  ক্ষেত্র WSAENOTEMPTY:
    p = "Not empty";
    বিরতি;
  ক্ষেত্র WSAEPROCLIM:
    p = "Process limit reached";
    বিরতি;
  ক্ষেত্র WSAEUSERS:
    p = "Too many users";
    বিরতি;
  ক্ষেত্র WSAEDQUOT:
    p = "Bad quota";
    বিরতি;
  ক্ষেত্র WSAESTALE:
    p = "Something is stale";
    বিরতি;
  ক্ষেত্র WSAEREMOTE:
    p = "Remote error";
    বিরতি;
  ক্ষেত্র WSAEDISCON:
    p = "Disconnected";
    বিরতি;
    /* Extended Winsock errors */
  ক্ষেত্র WSASYSNOTREADY:
    p = "Winsock library is not ready";
    বিরতি;
  ক্ষেত্র WSANOTINITIALISED:
    p = "Winsock library not initialized";
    বিরতি;
  ক্ষেত্র WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    বিরতি;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  ক্ষেত্র WSAHOST_NOT_FOUND:
    p = "Host not found";
    বিরতি;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  ক্ষেত্র WSATRY_AGAIN:
    p = "Host not found, try again";
    বিরতি;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  ক্ষেত্র WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    বিরতি;

    /* Valid name, no data record of requested type */
  ক্ষেত্র WSANO_তথ্য:
    p = "No data record of requested type";
    বিরতি;

  পূর্বনির্ধারিত:
    ফেরত ফাঁকা;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  ফেরত buf;
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
ধ্রুবক অক্ষর *curlx_strerror(পূর্ণসংখ্যা err, অক্ষর *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  পূর্ণসংখ্যা old_errno = errno;
  অক্ষর *p;

  যদি(!buflen)
    ফেরত ফাঁকা;

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
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned পূর্ণসংখ্যা)err);
#অন্যথা /* !_WIN32 */

#যদি defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  যদি(strerror_r(err, buf, buflen) &&
     buflen > আকার("Unknown error ") + 20) {
    যদি(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#নতুবা_যদি defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    অক্ষর বাফার[256];
    ধ্রুবক অক্ষর *বার্তা = strerror_r(err, বাফার, আকার(বাফার));
    যদি(বার্তা && buflen > 1)
      SNPRINTF(buf, buflen, "%s", বার্তা);
    অন্যথা যদি(buflen > আকার("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#অন্যথা
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ধ্রুবক অক্ষর *বার্তা = strerror(err);
    যদি(বার্তা && buflen > 1)
      SNPRINTF(buf, buflen, "%s", বার্তা);
    অন্যথা যদি(buflen > আকার("Unknown error ") + 20)
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

  ফেরত buf;
}
