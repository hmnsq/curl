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
#अन्तर्भाव "curl_setup.h"

#ifdef HAVE_STRERROR_R
#  सचे (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#अन्तर्भाव "curlx/winapi.h"
#अन्तर्भाव "curlx/snprintf.h"
#अन्तर्भाव "curlx/strerr.h"
#अन्तर्भाव "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
स्थिर ध्रुव वर्ण *get_winsock_error(पूर्णाङ्क err, वर्ण *buf, size_t len)
{
  VERBOSE(ध्रुव वर्ण *p);

  सचे(!len)
    पटिनिवत्त रिक्त;

  *buf = '\0';

#ifndef CURLVERBOSE
  (शून्य)err;
  पटिनिवत्त रिक्त;
#अञ्ञथा
  विकल्प(err) {
  पक्ष WSAEINTR:
    p = "Call interrupted";
    भङ्ग;
  पक्ष WSAEBADF:
    p = "Bad file";
    भङ्ग;
  पक्ष WSAEACCES:
    p = "Bad access";
    भङ्ग;
  पक्ष WSAEFAULT:
    p = "Bad argument";
    भङ्ग;
  पक्ष WSAEINVAL:
    p = "Invalid arguments";
    भङ्ग;
  पक्ष WSAEMFILE:
    p = "Out of file descriptors";
    भङ्ग;
  पक्ष WSAEWOULDBLOCK:
    p = "Call would block";
    भङ्ग;
  पक्ष WSAEINPROGRESS:
  पक्ष WSAEALREADY:
    p = "Blocking call in progress";
    भङ्ग;
  पक्ष WSAENOTSOCK:
    p = "Descriptor is not a socket";
    भङ्ग;
  पक्ष WSAEDESTADDRREQ:
    p = "Need destination address";
    भङ्ग;
  पक्ष WSAEMSGSIZE:
    p = "Bad message size";
    भङ्ग;
  पक्ष WSAEPROTOTYPE:
    p = "Bad protocol";
    भङ्ग;
  पक्ष WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    भङ्ग;
  पक्ष WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    भङ्ग;
  पक्ष WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    भङ्ग;
  पक्ष WSAEOPNOTSUPP:
    p = "Operation not supported";
    भङ्ग;
  पक्ष WSAEAFNOSUPPORT:
    p = "Address family not supported";
    भङ्ग;
  पक्ष WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    भङ्ग;
  पक्ष WSAEADDRINUSE:
    p = "Address already in use";
    भङ्ग;
  पक्ष WSAEADDRNOTAVAIL:
    p = "Address not available";
    भङ्ग;
  पक्ष WSAENETDOWN:
    p = "Network down";
    भङ्ग;
  पक्ष WSAENETUNREACH:
    p = "Network unreachable";
    भङ्ग;
  पक्ष WSAENETRESET:
    p = "Network has been reset";
    भङ्ग;
  पक्ष WSAECONNABORTED:
    p = "Connection was aborted";
    भङ्ग;
  पक्ष WSAECONNRESET:
    p = "Connection was reset";
    भङ्ग;
  पक्ष WSAENOBUFS:
    p = "No buffer space";
    भङ्ग;
  पक्ष WSAEISCONN:
    p = "Socket is already connected";
    भङ्ग;
  पक्ष WSAENOTCONN:
    p = "Socket is not connected";
    भङ्ग;
  पक्ष WSAESHUTDOWN:
    p = "Socket has been shut down";
    भङ्ग;
  पक्ष WSAETOOMANYREFS:
    p = "Too many references";
    भङ्ग;
  पक्ष WSAETIMEDOUT:
    p = "Timed out";
    भङ्ग;
  पक्ष WSAECONNREFUSED:
    p = "Connection refused";
    भङ्ग;
  पक्ष WSAELOOP:
    p = "Loop??";
    भङ्ग;
  पक्ष WSAENAMETOOLONG:
    p = "Name too long";
    भङ्ग;
  पक्ष WSAEHOSTDOWN:
    p = "Host down";
    भङ्ग;
  पक्ष WSAEHOSTUNREACH:
    p = "Host unreachable";
    भङ्ग;
  पक्ष WSAENOTEMPTY:
    p = "Not empty";
    भङ्ग;
  पक्ष WSAEPROCLIM:
    p = "Process limit reached";
    भङ्ग;
  पक्ष WSAEUSERS:
    p = "Too many users";
    भङ्ग;
  पक्ष WSAEDQUOT:
    p = "Bad quota";
    भङ्ग;
  पक्ष WSAESTALE:
    p = "Something is stale";
    भङ्ग;
  पक्ष WSAEREMOTE:
    p = "Remote error";
    भङ्ग;
  पक्ष WSAEDISCON:
    p = "Disconnected";
    भङ्ग;
    /* Extended Winsock errors */
  पक्ष WSASYSNOTREADY:
    p = "Winsock library is not ready";
    भङ्ग;
  पक्ष WSANOTINITIALISED:
    p = "Winsock library not initialized";
    भङ्ग;
  पक्ष WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    भङ्ग;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  पक्ष WSAHOST_NOT_FOUND:
    p = "Host not found";
    भङ्ग;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  पक्ष WSATRY_AGAIN:
    p = "Host not found, try again";
    भङ्ग;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  पक्ष WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    भङ्ग;

    /* Valid name, no data record of requested type */
  पक्ष WSANO_DATA:
    p = "No data record of requested type";
    भङ्ग;

  स्वभाव:
    पटिनिवत्त रिक्त;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  पटिनिवत्त buf;
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
ध्रुव वर्ण *curlx_strerror(पूर्णाङ्क err, वर्ण *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  पूर्णाङ्क old_errno = errno;
  वर्ण *p;

  सचे(!buflen)
    पटिनिवत्त रिक्त;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  सचे((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned पूर्णाङ्क)err);
#अञ्ञथा /* !_WIN32 */

#सचे defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  सचे(strerror_r(err, buf, buflen) &&
     buflen > परिमाण("Unknown error ") + 20) {
    सचे(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#नोचेद्यदि defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    वर्ण buffer[256];
    ध्रुव वर्ण *सन्देशः = strerror_r(err, buffer, परिमाण(buffer));
    सचे(सन्देशः && buflen > 1)
      SNPRINTF(buf, buflen, "%s", सन्देशः);
    अञ्ञथा सचे(buflen > परिमाण("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#अञ्ञथा
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ध्रुव वर्ण *सन्देशः = strerror(err);
    सचे(सन्देशः && buflen > 1)
      SNPRINTF(buf, buflen, "%s", सन्देशः);
    अञ्ञथा सचे(buflen > परिमाण("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  सचे(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  सचे(p && (p - buf) >= 1)
    *p = '\0';

  सचे(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  सचे(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  पटिनिवत्त buf;
}
