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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef HAVE_STRERROR_R
#  ଯଦି (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#ଅନ୍ତର୍ଭାଵ "curlx/winapi.h"
#ଅନ୍ତର୍ଭାଵ "curlx/snprintf.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strerr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *get_winsock_error(ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, ଅକ୍ଷର *buf, size_t len)
{
  VERBOSE(ଅଚଳ ଅକ୍ଷର *p);

  ଯଦି(!len)
    ଫେରନ୍ତୁ ଖାଲି;

  *buf = '\0';

#ifndef CURLVERBOSE
  (ଶୂନ୍ୟ)err;
  ଫେରନ୍ତୁ ଖାଲି;
#ଅନ୍ୟଥା
  ବାଛ(err) {
  ସ୍ଥିତି WSAEINTR:
    p = "Call interrupted";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEBADF:
    p = "Bad file";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEACCES:
    p = "Bad access";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEFAULT:
    p = "Bad argument";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEINVAL:
    p = "Invalid arguments";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEMFILE:
    p = "Out of file descriptors";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEWOULDBLOCK:
    p = "Call would block";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEINPROGRESS:
  ସ୍ଥିତି WSAEALREADY:
    p = "Blocking call in progress";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAENOTSOCK:
    p = "Descriptor is not a socket";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEDESTADDRREQ:
    p = "Need destination address";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEMSGSIZE:
    p = "Bad message size";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEPROTOTYPE:
    p = "Bad protocol";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEOPNOTSUPP:
    p = "Operation not supported";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEAFNOSUPPORT:
    p = "Address family not supported";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEADDRINUSE:
    p = "Address already in use";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEADDRNOTAVAIL:
    p = "Address not available";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAENETDOWN:
    p = "Network down";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAENETUNREACH:
    p = "Network unreachable";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAENETRESET:
    p = "Network has been reset";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAECONNABORTED:
    p = "Connection was aborted";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAECONNRESET:
    p = "Connection was reset";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAENOBUFS:
    p = "No buffer space";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEISCONN:
    p = "Socket is already connected";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAENOTCONN:
    p = "Socket is not connected";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAESHUTDOWN:
    p = "Socket has been shut down";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAETOOMANYREFS:
    p = "Too many references";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAETIMEDOUT:
    p = "Timed out";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAECONNREFUSED:
    p = "Connection refused";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAELOOP:
    p = "Loop??";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAENAMETOOLONG:
    p = "Name too long";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEHOSTDOWN:
    p = "Host down";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEHOSTUNREACH:
    p = "Host unreachable";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAENOTEMPTY:
    p = "Not empty";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEPROCLIM:
    p = "Process limit reached";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEUSERS:
    p = "Too many users";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEDQUOT:
    p = "Bad quota";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAESTALE:
    p = "Something is stale";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEREMOTE:
    p = "Remote error";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAEDISCON:
    p = "Disconnected";
    ଭାଙ୍ଗ;
    /* Extended Winsock errors */
  ସ୍ଥିତି WSASYSNOTREADY:
    p = "Winsock library is not ready";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSANOTINITIALISED:
    p = "Winsock library not initialized";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    ଭାଙ୍ଗ;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  ସ୍ଥିତି WSAHOST_NOT_FOUND:
    p = "Host not found";
    ଭାଙ୍ଗ;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  ସ୍ଥିତି WSATRY_AGAIN:
    p = "Host not found, try again";
    ଭାଙ୍ଗ;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  ସ୍ଥିତି WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    ଭାଙ୍ଗ;

    /* Valid name, no data record of requested type */
  ସ୍ଥିତି WSANO_DATA:
    p = "No data record of requested type";
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ ଖାଲି;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  ଫେରନ୍ତୁ buf;
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
ଅଚଳ ଅକ୍ଷର *curlx_strerror(ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, ଅକ୍ଷର *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_errno = errno;
  ଅକ୍ଷର *p;

  ଯଦି(!buflen)
    ଫେରନ୍ତୁ ଖାଲି;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  ଯଦି((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)err);
#ଅନ୍ୟଥା /* !_WIN32 */

#ଯଦି defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  ଯଦି(strerror_r(err, buf, buflen) &&
     buflen > ଆକାର("Unknown error ") + 20) {
    ଯଦି(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ନଚେତଯଦି defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    ଅକ୍ଷର buffer[256];
    ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ = strerror_r(err, buffer, ଆକାର(buffer));
    ଯଦି(ସନ୍ଦେଶ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", ସନ୍ଦେଶ);
    ଅନ୍ୟଥା ଯଦି(buflen > ଆକାର("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ଅନ୍ୟଥା
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ = strerror(err);
    ଯଦି(ସନ୍ଦେଶ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", ସନ୍ଦେଶ);
    ଅନ୍ୟଥା ଯଦି(buflen > ଆକାର("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  ଯଦି(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  ଯଦି(p && (p - buf) >= 1)
    *p = '\0';

  ଯଦି(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  ଯଦି(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  ଫେରନ୍ତୁ buf;
}
