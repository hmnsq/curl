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
#समावेश "curl_setup.h"

#अगर_घोषित HAVE_STRERROR_R
#  अगर (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#पूर्ण_अगर

#समावेश "curlx/winapi.h"
#समावेश "curlx/snprintf.h"
#समावेश "curlx/strerr.h"
#समावेश "curlx/strcopy.h"

#अगर_घोषित USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
अटल स्थिर अक्षर *get_winsock_error(पूर्णांक err, अक्षर *buf, माप_प्रकार len)
{
  VERBOSE(स्थिर अक्षर *p);

  अगर(!len)
    वापस शून्य;

  *buf = '\0';

#अगर_अघोषित CURLVERBOSE
  (व्योम)err;
  वापस शून्य;
#अन्यथा
  चयन(err) {
  हाल WSAEINTR:
    p = "Call interrupted";
    अवरोध;
  हाल WSAEBADF:
    p = "Bad file";
    अवरोध;
  हाल WSAEACCES:
    p = "Bad access";
    अवरोध;
  हाल WSAEFAULT:
    p = "Bad argument";
    अवरोध;
  हाल WSAEINVAL:
    p = "Invalid arguments";
    अवरोध;
  हाल WSAEMFILE:
    p = "Out of file descriptors";
    अवरोध;
  हाल WSAEWOULDBLOCK:
    p = "Call would block";
    अवरोध;
  हाल WSAEINPROGRESS:
  हाल WSAEALREADY:
    p = "Blocking call in progress";
    अवरोध;
  हाल WSAENOTSOCK:
    p = "Descriptor is not a socket";
    अवरोध;
  हाल WSAEDESTADDRREQ:
    p = "Need destination address";
    अवरोध;
  हाल WSAEMSGSIZE:
    p = "Bad message size";
    अवरोध;
  हाल WSAEPROTOTYPE:
    p = "Bad protocol";
    अवरोध;
  हाल WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    अवरोध;
  हाल WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    अवरोध;
  हाल WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    अवरोध;
  हाल WSAEOPNOTSUPP:
    p = "Operation not supported";
    अवरोध;
  हाल WSAEAFNOSUPPORT:
    p = "Address family not supported";
    अवरोध;
  हाल WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    अवरोध;
  हाल WSAEADDRINUSE:
    p = "Address already in use";
    अवरोध;
  हाल WSAEADDRNOTAVAIL:
    p = "Address not available";
    अवरोध;
  हाल WSAENETDOWN:
    p = "Network down";
    अवरोध;
  हाल WSAENETUNREACH:
    p = "Network unreachable";
    अवरोध;
  हाल WSAENETRESET:
    p = "Network has been reset";
    अवरोध;
  हाल WSAECONNABORTED:
    p = "Connection was aborted";
    अवरोध;
  हाल WSAECONNRESET:
    p = "Connection was reset";
    अवरोध;
  हाल WSAENOBUFS:
    p = "No buffer space";
    अवरोध;
  हाल WSAEISCONN:
    p = "Socket is already connected";
    अवरोध;
  हाल WSAENOTCONN:
    p = "Socket is not connected";
    अवरोध;
  हाल WSAESHUTDOWN:
    p = "Socket has been shut down";
    अवरोध;
  हाल WSAETOOMANYREFS:
    p = "Too many references";
    अवरोध;
  हाल WSAETIMEDOUT:
    p = "Timed out";
    अवरोध;
  हाल WSAECONNREFUSED:
    p = "Connection refused";
    अवरोध;
  हाल WSAELOOP:
    p = "Loop??";
    अवरोध;
  हाल WSAENAMETOOLONG:
    p = "Name too long";
    अवरोध;
  हाल WSAEHOSTDOWN:
    p = "Host down";
    अवरोध;
  हाल WSAEHOSTUNREACH:
    p = "Host unreachable";
    अवरोध;
  हाल WSAENOTEMPTY:
    p = "Not empty";
    अवरोध;
  हाल WSAEPROCLIM:
    p = "Process limit reached";
    अवरोध;
  हाल WSAEUSERS:
    p = "Too many users";
    अवरोध;
  हाल WSAEDQUOT:
    p = "Bad quota";
    अवरोध;
  हाल WSAESTALE:
    p = "Something is stale";
    अवरोध;
  हाल WSAEREMOTE:
    p = "Remote error";
    अवरोध;
  हाल WSAEDISCON:
    p = "Disconnected";
    अवरोध;
    /* Extended Winsock errors */
  हाल WSASYSNOTREADY:
    p = "Winsock library is not ready";
    अवरोध;
  हाल WSANOTINITIALISED:
    p = "Winsock library not initialized";
    अवरोध;
  हाल WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    अवरोध;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  हाल WSAHOST_NOT_FOUND:
    p = "Host not found";
    अवरोध;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  हाल WSATRY_AGAIN:
    p = "Host not found, try again";
    अवरोध;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  हाल WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    अवरोध;

    /* Valid name, no data record of requested type */
  हाल WSANO_तथ्य:
    p = "No data record of requested type";
    अवरोध;

  शेष:
    वापस शून्य;
  }
  curlx_strcopy(buf, len, p, म_माप(p));
  वापस buf;
#पूर्ण_अगर
}
#पूर्ण_अगर /* USE_WINSOCK */

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
स्थिर अक्षर *curlx_strerror(पूर्णांक err, अक्षर *buf, माप_प्रकार buflen)
{
#अगर_घोषित _WIN32
  DWORD old_win_err = GetLastError();
#पूर्ण_अगर
  पूर्णांक old_errno = त्रुटि_सं;
  अक्षर *p;

  अगर(!buflen)
    वापस शून्य;

#अगर_अघोषित _WIN32
  DEBUGASSERT(err >= 0);
#पूर्ण_अगर

  *buf = '\0';

#अगर_घोषित _WIN32
  अगर((strerror_s(buf, buflen, err) || !म_भेद(buf, "Unknown error")) &&
#अगर_घोषित USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#पूर्ण_अगर
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (अचिन्हित पूर्णांक)err);
#अन्यथा /* !_WIN32 */

#अगर defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  अगर(strerror_r(err, buf, buflen) &&
     buflen > माप("Unknown error ") + 20) {
    अगर(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#या_अगर defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    अक्षर बफर[256];
    स्थिर अक्षर *संदेश = strerror_r(err, बफर, माप(बफर));
    अगर(संदेश && buflen > 1)
      SNPRINTF(buf, buflen, "%s", संदेश);
    अन्यथा अगर(buflen > माप("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#अन्यथा
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    स्थिर अक्षर *संदेश = म_त्रुटि(err);
    अगर(संदेश && buflen > 1)
      SNPRINTF(buf, buflen, "%s", संदेश);
    अन्यथा अगर(buflen > माप("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#पूर्ण_अगर

#पूर्ण_अगर /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = म_खोजप(buf, '\n');
  अगर(p && (p - buf) >= 2)
    *p = '\0';
  p = म_खोजप(buf, '\r');
  अगर(p && (p - buf) >= 1)
    *p = '\0';

  अगर(त्रुटि_सं != old_errno)
    त्रुटि_सं = old_errno;

#अगर_घोषित _WIN32
  अगर(old_win_err != GetLastError())
    SetLastError(old_win_err);
#पूर्ण_अगर

  वापस buf;
}
