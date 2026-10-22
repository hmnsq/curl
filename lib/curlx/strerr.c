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
#  जर (!defined(HAVE_POSIX_STRERROR_R) && \
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
स्थिर स्थिरांक अक्षर *get_winsock_error(पूर्णांक err, अक्षर *buf, माप_प्रकार len)
{
  VERBOSE(स्थिरांक अक्षर *p);

  जर(!len)
    परत रिक्त;

  *buf = '\0';

#ifndef CURLVERBOSE
  (शून्य)err;
  परत रिक्त;
#अन्यथा
  निवड(err) {
  स्थिती WSAEINTR:
    p = "Call interrupted";
    थांबा;
  स्थिती WSAEBADF:
    p = "Bad file";
    थांबा;
  स्थिती WSAEACCES:
    p = "Bad access";
    थांबा;
  स्थिती WSAEFAULT:
    p = "Bad argument";
    थांबा;
  स्थिती WSAEINVAL:
    p = "Invalid arguments";
    थांबा;
  स्थिती WSAEMFILE:
    p = "Out of file descriptors";
    थांबा;
  स्थिती WSAEWOULDBLOCK:
    p = "Call would block";
    थांबा;
  स्थिती WSAEINPROGRESS:
  स्थिती WSAEALREADY:
    p = "Blocking call in progress";
    थांबा;
  स्थिती WSAENOTSOCK:
    p = "Descriptor is not a socket";
    थांबा;
  स्थिती WSAEDESTADDRREQ:
    p = "Need destination address";
    थांबा;
  स्थिती WSAEMSGSIZE:
    p = "Bad message size";
    थांबा;
  स्थिती WSAEPROTOTYPE:
    p = "Bad protocol";
    थांबा;
  स्थिती WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    थांबा;
  स्थिती WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    थांबा;
  स्थिती WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    थांबा;
  स्थिती WSAEOPNOTSUPP:
    p = "Operation not supported";
    थांबा;
  स्थिती WSAEAFNOSUPPORT:
    p = "Address family not supported";
    थांबा;
  स्थिती WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    थांबा;
  स्थिती WSAEADDRINUSE:
    p = "Address already in use";
    थांबा;
  स्थिती WSAEADDRNOTAVAIL:
    p = "Address not available";
    थांबा;
  स्थिती WSAENETDOWN:
    p = "Network down";
    थांबा;
  स्थिती WSAENETUNREACH:
    p = "Network unreachable";
    थांबा;
  स्थिती WSAENETRESET:
    p = "Network has been reset";
    थांबा;
  स्थिती WSAECONNABORTED:
    p = "Connection was aborted";
    थांबा;
  स्थिती WSAECONNRESET:
    p = "Connection was reset";
    थांबा;
  स्थिती WSAENOBUFS:
    p = "No buffer space";
    थांबा;
  स्थिती WSAEISCONN:
    p = "Socket is already connected";
    थांबा;
  स्थिती WSAENOTCONN:
    p = "Socket is not connected";
    थांबा;
  स्थिती WSAESHUTDOWN:
    p = "Socket has been shut down";
    थांबा;
  स्थिती WSAETOOMANYREFS:
    p = "Too many references";
    थांबा;
  स्थिती WSAETIMEDOUT:
    p = "Timed out";
    थांबा;
  स्थिती WSAECONNREFUSED:
    p = "Connection refused";
    थांबा;
  स्थिती WSAELOOP:
    p = "Loop??";
    थांबा;
  स्थिती WSAENAMETOOLONG:
    p = "Name too long";
    थांबा;
  स्थिती WSAEHOSTDOWN:
    p = "Host down";
    थांबा;
  स्थिती WSAEHOSTUNREACH:
    p = "Host unreachable";
    थांबा;
  स्थिती WSAENOTEMPTY:
    p = "Not empty";
    थांबा;
  स्थिती WSAEPROCLIM:
    p = "Process limit reached";
    थांबा;
  स्थिती WSAEUSERS:
    p = "Too many users";
    थांबा;
  स्थिती WSAEDQUOT:
    p = "Bad quota";
    थांबा;
  स्थिती WSAESTALE:
    p = "Something is stale";
    थांबा;
  स्थिती WSAEREMOTE:
    p = "Remote error";
    थांबा;
  स्थिती WSAEDISCON:
    p = "Disconnected";
    थांबा;
    /* Extended Winsock errors */
  स्थिती WSASYSNOTREADY:
    p = "Winsock library is not ready";
    थांबा;
  स्थिती WSANOTINITIALISED:
    p = "Winsock library not initialized";
    थांबा;
  स्थिती WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    थांबा;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  स्थिती WSAHOST_NOT_FOUND:
    p = "Host not found";
    थांबा;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  स्थिती WSATRY_AGAIN:
    p = "Host not found, try again";
    थांबा;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  स्थिती WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    थांबा;

    /* Valid name, no data record of requested type */
  स्थिती WSANO_DATA:
    p = "No data record of requested type";
    थांबा;

  पूर्वनिर्धारित:
    परत रिक्त;
  }
  curlx_strcopy(buf, len, p, म_माप(p));
  परत buf;
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
स्थिरांक अक्षर *curlx_strerror(पूर्णांक err, अक्षर *buf, माप_प्रकार buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  पूर्णांक old_errno = त्रुटि_सं;
  अक्षर *p;

  जर(!buflen)
    परत रिक्त;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  जर((strerror_s(buf, buflen, err) || !म_भेद(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (अचिन्हित पूर्णांक)err);
#अन्यथा /* !_WIN32 */

#जर defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  जर(strerror_r(err, buf, buflen) &&
     buflen > आकार("Unknown error ") + 20) {
    जर(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#किंवाजर defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    अक्षर buffer[256];
    स्थिरांक अक्षर *संदेश = strerror_r(err, buffer, आकार(buffer));
    जर(संदेश && buflen > 1)
      SNPRINTF(buf, buflen, "%s", संदेश);
    अन्यथा जर(buflen > आकार("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#अन्यथा
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    स्थिरांक अक्षर *संदेश = म_त्रुटि(err);
    जर(संदेश && buflen > 1)
      SNPRINTF(buf, buflen, "%s", संदेश);
    अन्यथा जर(buflen > आकार("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = म_खोजप(buf, '\n');
  जर(p && (p - buf) >= 2)
    *p = '\0';
  p = म_खोजप(buf, '\r');
  जर(p && (p - buf) >= 1)
    *p = '\0';

  जर(त्रुटि_सं != old_errno)
    त्रुटि_सं = old_errno;

#ifdef _WIN32
  जर(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  परत buf;
}
