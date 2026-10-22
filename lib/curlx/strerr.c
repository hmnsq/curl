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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_STRERROR_R
#  ಆದರೆ (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"
#ಅನ್ತರ್ಭಾವ "curlx/snprintf.h"
#ಅನ್ತರ್ಭಾವ "curlx/strerr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *get_winsock_error(ಪೂರ್ಣಾಂಕ err, ಅಕ್ಷರ *buf, size_t len)
{
  VERBOSE(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);

  ಆದರೆ(!len)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  *buf = '\0';

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURLVERBOSE
  (ಶೂನ್ಯ)err;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
  ಆಯ್ಕೆ(err) {
  ಸ್ಥಿತಿ WSAEINTR:
    p = "Call interrupted";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEBADF:
    p = "Bad file";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEACCES:
    p = "Bad access";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEFAULT:
    p = "Bad argument";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEINVAL:
    p = "Invalid arguments";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEMFILE:
    p = "Out of file descriptors";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEWOULDBLOCK:
    p = "Call would block";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEINPROGRESS:
  ಸ್ಥಿತಿ WSAEALREADY:
    p = "Blocking call in progress";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAENOTSOCK:
    p = "Descriptor is not a socket";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEDESTADDRREQ:
    p = "Need destination address";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEMSGSIZE:
    p = "Bad message size";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEPROTOTYPE:
    p = "Bad protocol";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEOPNOTSUPP:
    p = "Operation not supported";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEAFNOSUPPORT:
    p = "Address family not supported";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEADDRINUSE:
    p = "Address already in use";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEADDRNOTAVAIL:
    p = "Address not available";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAENETDOWN:
    p = "Network down";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAENETUNREACH:
    p = "Network unreachable";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAENETRESET:
    p = "Network has been reset";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAECONNABORTED:
    p = "Connection was aborted";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAECONNRESET:
    p = "Connection was reset";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAENOBUFS:
    p = "No buffer space";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEISCONN:
    p = "Socket is already connected";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAENOTCONN:
    p = "Socket is not connected";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAESHUTDOWN:
    p = "Socket has been shut down";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAETOOMANYREFS:
    p = "Too many references";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAETIMEDOUT:
    p = "Timed out";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAECONNREFUSED:
    p = "Connection refused";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAELOOP:
    p = "Loop??";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAENAMETOOLONG:
    p = "Name too long";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEHOSTDOWN:
    p = "Host down";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEHOSTUNREACH:
    p = "Host unreachable";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAENOTEMPTY:
    p = "Not empty";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEPROCLIM:
    p = "Process limit reached";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEUSERS:
    p = "Too many users";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEDQUOT:
    p = "Bad quota";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAESTALE:
    p = "Something is stale";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEREMOTE:
    p = "Remote error";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAEDISCON:
    p = "Disconnected";
    ನಿಲ್ಲಿಸು;
    /* Extended Winsock errors */
  ಸ್ಥಿತಿ WSASYSNOTREADY:
    p = "Winsock library is not ready";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSANOTINITIALISED:
    p = "Winsock library not initialized";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    ನಿಲ್ಲಿಸು;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  ಸ್ಥಿತಿ WSAHOST_NOT_ಸಿಕ್ಕಿತು:
    p = "Host not found";
    ನಿಲ್ಲಿಸು;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  ಸ್ಥಿತಿ WSATRY_AGAIN:
    p = "Host not found, try again";
    ನಿಲ್ಲಿಸು;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  ಸ್ಥಿತಿ WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    ನಿಲ್ಲಿಸು;

    /* Valid name, no data record of requested type */
  ಸ್ಥಿತಿ WSANO_ದತ್ತಾಂಶ:
    p = "No data record of requested type";
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  curlx_strcopy(buf, len, p, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(p));
  ಹಿಂತಿರುಗಿ buf;
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_WINSOCK */

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
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curlx_strerror(ಪೂರ್ಣಾಂಕ err, ಅಕ್ಷರ *buf, size_t buflen)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  DWORD old_win_err = Get_ಕೊನೆಯ_Error();
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ old_errno = errno;
  ಅಕ್ಷರ *p;

  ಆದರೆ(!buflen)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _WIN32
  DEBUGASSERT(err >= 0);
#ಆದರೆ_ಅಂತ್ಯ

  *buf = '\0';

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  ಆದರೆ((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#ಆದರೆ_ಅಂತ್ಯ
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)err);
#ಇಲ್ಲದಿದ್ದರೆ /* !_WIN32 */

#ಆದರೆ defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  ಆದರೆ(strerror_r(err, buf, buflen) &&
     buflen > ಗಾತ್ರ("Unknown error ") + 20) {
    ಆದರೆ(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ಅಥವಾಆದರೆ defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    ಅಕ್ಷರ ಬಫರ್[256];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ = strerror_r(err, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));
    ಆದರೆ(ಸಂದೇಶ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", ಸಂದೇಶ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(buflen > ಗಾತ್ರ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ಇಲ್ಲದಿದ್ದರೆ
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ = strerror(err);
    ಆದರೆ(ಸಂದೇಶ && buflen > 1)
      SNPRINTF(buf, buflen, "%s", ಸಂದೇಶ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(buflen > ಗಾತ್ರ("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  ಆದರೆ(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  ಆದರೆ(p && (p - buf) >= 1)
    *p = '\0';

  ಆದರೆ(errno != old_errno)
    errno = old_errno;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  ಆದರೆ(old_win_err != Get_ಕೊನೆಯ_Error())
    ಗಣ_ಕೊನೆಯ_Error(old_win_err);
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ buf;
}
