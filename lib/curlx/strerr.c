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
#  यदि (!defined(HAVE_POSIX_STRERROR_R) && \
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
स्थिर अचर वर्ण *get_winsock_error(पूर्णाङ्क err, वर्ण *buf, माप_प्रकार len)
{
  VERBOSE(अचर वर्ण *p);

  यदि(!len)
    फर्काऊ रिक्त;

  *buf = '\0';

#ifndef CURLVERBOSE
  (शून्य)err;
  फर्काऊ रिक्त;
#अन्यथा
  विकल्प(err) {
  पक्ष WSAEINTR:
    p = "Call interrupted";
    रोक;
  पक्ष WSAEBADF:
    p = "Bad file";
    रोक;
  पक्ष WSAEACCES:
    p = "Bad access";
    रोक;
  पक्ष WSAEFAULT:
    p = "Bad argument";
    रोक;
  पक्ष WSAEINVAL:
    p = "Invalid arguments";
    रोक;
  पक्ष WSAEMFILE:
    p = "Out of file descriptors";
    रोक;
  पक्ष WSAEWOULDBLOCK:
    p = "Call would block";
    रोक;
  पक्ष WSAEINPROGRESS:
  पक्ष WSAEALREADY:
    p = "Blocking call in progress";
    रोक;
  पक्ष WSAENOTSOCK:
    p = "Descriptor is not a socket";
    रोक;
  पक्ष WSAEDESTADDRREQ:
    p = "Need destination address";
    रोक;
  पक्ष WSAEMSGSIZE:
    p = "Bad message size";
    रोक;
  पक्ष WSAEPROTOTYPE:
    p = "Bad protocol";
    रोक;
  पक्ष WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    रोक;
  पक्ष WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    रोक;
  पक्ष WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    रोक;
  पक्ष WSAEOPNOTSUPP:
    p = "Operation not supported";
    रोक;
  पक्ष WSAEAFNOSUPPORT:
    p = "Address family not supported";
    रोक;
  पक्ष WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    रोक;
  पक्ष WSAEADDRINUSE:
    p = "Address already in use";
    रोक;
  पक्ष WSAEADDRNOTAVAIL:
    p = "Address not available";
    रोक;
  पक्ष WSAENETDOWN:
    p = "Network down";
    रोक;
  पक्ष WSAENETUNREACH:
    p = "Network unreachable";
    रोक;
  पक्ष WSAENETRESET:
    p = "Network has been reset";
    रोक;
  पक्ष WSAECONNABORTED:
    p = "Connection was aborted";
    रोक;
  पक्ष WSAECONNRESET:
    p = "Connection was reset";
    रोक;
  पक्ष WSAENOBUFS:
    p = "No buffer space";
    रोक;
  पक्ष WSAEISCONN:
    p = "Socket is already connected";
    रोक;
  पक्ष WSAENOTCONN:
    p = "Socket is not connected";
    रोक;
  पक्ष WSAESHUTDOWN:
    p = "Socket has been shut down";
    रोक;
  पक्ष WSAETOOMANYREFS:
    p = "Too many references";
    रोक;
  पक्ष WSAETIMEDOUT:
    p = "Timed out";
    रोक;
  पक्ष WSAECONNREFUSED:
    p = "Connection refused";
    रोक;
  पक्ष WSAELOOP:
    p = "Loop??";
    रोक;
  पक्ष WSAENAMETOOLONG:
    p = "Name too long";
    रोक;
  पक्ष WSAEHOSTDOWN:
    p = "Host down";
    रोक;
  पक्ष WSAEHOSTUNREACH:
    p = "Host unreachable";
    रोक;
  पक्ष WSAENOTEMPTY:
    p = "Not empty";
    रोक;
  पक्ष WSAEPROCLIM:
    p = "Process limit reached";
    रोक;
  पक्ष WSAEUSERS:
    p = "Too many users";
    रोक;
  पक्ष WSAEDQUOT:
    p = "Bad quota";
    रोक;
  पक्ष WSAESTALE:
    p = "Something is stale";
    रोक;
  पक्ष WSAEREMOTE:
    p = "Remote error";
    रोक;
  पक्ष WSAEDISCON:
    p = "Disconnected";
    रोक;
    /* Extended Winsock errors */
  पक्ष WSASYSNOTREADY:
    p = "Winsock library is not ready";
    रोक;
  पक्ष WSANOTINITIALISED:
    p = "Winsock library not initialized";
    रोक;
  पक्ष WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    रोक;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  पक्ष WSAHOST_NOT_FOUND:
    p = "Host not found";
    रोक;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  पक्ष WSATRY_AGAIN:
    p = "Host not found, try again";
    रोक;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  पक्ष WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    रोक;

    /* Valid name, no data record of requested type */
  पक्ष WSANO_DATA:
    p = "No data record of requested type";
    रोक;

  स्वभाव:
    फर्काऊ रिक्त;
  }
  curlx_strcopy(buf, len, p, म_माप(p));
  फर्काऊ buf;
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
अचर वर्ण *curlx_strerror(पूर्णाङ्क err, वर्ण *buf, माप_प्रकार buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  पूर्णाङ्क old_errno = त्रुटि_सं;
  वर्ण *p;

  यदि(!buflen)
    फर्काऊ रिक्त;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  यदि((strerror_s(buf, buflen, err) || !म_भेद(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (अचिन्हित पूर्णाङ्क)err);
#अन्यथा /* !_WIN32 */

#यदि defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  यदि(strerror_r(err, buf, buflen) &&
     buflen > परिमाण("Unknown error ") + 20) {
    यदि(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#अथवायदि defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    वर्ण buffer[256];
    अचर वर्ण *सन्देश = strerror_r(err, buffer, परिमाण(buffer));
    यदि(सन्देश && buflen > 1)
      SNPRINTF(buf, buflen, "%s", सन्देश);
    अन्यथा यदि(buflen > परिमाण("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#अन्यथा
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    अचर वर्ण *सन्देश = म_त्रुटि(err);
    यदि(सन्देश && buflen > 1)
      SNPRINTF(buf, buflen, "%s", सन्देश);
    अन्यथा यदि(buflen > परिमाण("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = म_खोजप(buf, '\n');
  यदि(p && (p - buf) >= 2)
    *p = '\0';
  p = म_खोजप(buf, '\r');
  यदि(p && (p - buf) >= 1)
    *p = '\0';

  यदि(त्रुटि_सं != old_errno)
    त्रुटि_सं = old_errno;

#ifdef _WIN32
  यदि(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  फर्काऊ buf;
}
