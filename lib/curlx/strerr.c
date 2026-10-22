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
#include "curl_setup.h"

#ifdef HAVE_STRERROR_R
#  ᯀᯮᯝ (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#endif

#include "curlx/winapi.h"
#include "curlx/snprintf.h"
#include "curlx/strerr.h"
#include "curlx/strcopy.h"

#ifdef USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
static const var_na *get_winsock_error(puur_naa_nka err, var_na *buf, size_t len)
{
  VERBOSE(const var_na *p);

  ᯀᯮᯝ(!len)
    ᯀᯮᯞ NULL;

  *buf = '\0';

#ifndef CURLVERBOSE
  (void)err;
  ᯀᯮᯞ NULL;
#ᯀᯮᯝᯂᯪ
  switch(err) {
  pak_sa WSAEINTR:
    p = "Call interrupted";
    bha_nga;
  pak_sa WSAEBADF:
    p = "Bad file";
    bha_nga;
  pak_sa WSAEACCES:
    p = "Bad access";
    bha_nga;
  pak_sa WSAEFAULT:
    p = "Bad argument";
    bha_nga;
  pak_sa WSAEINVAL:
    p = "Invalid arguments";
    bha_nga;
  pak_sa WSAEMFILE:
    p = "Out of file descriptors";
    bha_nga;
  pak_sa WSAEWOULDBLOCK:
    p = "Call would block";
    bha_nga;
  pak_sa WSAEINPROGRESS:
  pak_sa WSAEALREADY:
    p = "Blocking call in progress";
    bha_nga;
  pak_sa WSAENOTSOCK:
    p = "Descriptor is not a socket";
    bha_nga;
  pak_sa WSAEDESTADDRREQ:
    p = "Need destination address";
    bha_nga;
  pak_sa WSAEMSGSIZE:
    p = "Bad message size";
    bha_nga;
  pak_sa WSAEPROTOTYPE:
    p = "Bad protocol";
    bha_nga;
  pak_sa WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    bha_nga;
  pak_sa WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    bha_nga;
  pak_sa WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    bha_nga;
  pak_sa WSAEOPNOTSUPP:
    p = "Operation not supported";
    bha_nga;
  pak_sa WSAEAFNOSUPPORT:
    p = "Address family not supported";
    bha_nga;
  pak_sa WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    bha_nga;
  pak_sa WSAEADDRINUSE:
    p = "Address already in use";
    bha_nga;
  pak_sa WSAEADDRNOTAVAIL:
    p = "Address not available";
    bha_nga;
  pak_sa WSAENETDOWN:
    p = "Network down";
    bha_nga;
  pak_sa WSAENETUNREACH:
    p = "Network unreachable";
    bha_nga;
  pak_sa WSAENETRESET:
    p = "Network has been reset";
    bha_nga;
  pak_sa WSAECONNABORTED:
    p = "Connection was aborted";
    bha_nga;
  pak_sa WSAECONNRESET:
    p = "Connection was reset";
    bha_nga;
  pak_sa WSAENOBUFS:
    p = "No buffer space";
    bha_nga;
  pak_sa WSAEISCONN:
    p = "Socket is already connected";
    bha_nga;
  pak_sa WSAENOTCONN:
    p = "Socket is not connected";
    bha_nga;
  pak_sa WSAESHUTDOWN:
    p = "Socket has been shut down";
    bha_nga;
  pak_sa WSAETOOMANYREFS:
    p = "Too many references";
    bha_nga;
  pak_sa WSAETIMEDOUT:
    p = "Timed out";
    bha_nga;
  pak_sa WSAECONNREFUSED:
    p = "Connection refused";
    bha_nga;
  pak_sa WSAELOOP:
    p = "Loop??";
    bha_nga;
  pak_sa WSAENAMETOOLONG:
    p = "Name too long";
    bha_nga;
  pak_sa WSAEHOSTDOWN:
    p = "Host down";
    bha_nga;
  pak_sa WSAEHOSTUNREACH:
    p = "Host unreachable";
    bha_nga;
  pak_sa WSAENOTEMPTY:
    p = "Not empty";
    bha_nga;
  pak_sa WSAEPROCLIM:
    p = "Process limit reached";
    bha_nga;
  pak_sa WSAEUSERS:
    p = "Too many users";
    bha_nga;
  pak_sa WSAEDQUOT:
    p = "Bad quota";
    bha_nga;
  pak_sa WSAESTALE:
    p = "Something is stale";
    bha_nga;
  pak_sa WSAEREMOTE:
    p = "Remote error";
    bha_nga;
  pak_sa WSAEDISCON:
    p = "Disconnected";
    bha_nga;
    /* Extended Winsock errors */
  pak_sa WSASYSNOTREADY:
    p = "Winsock library is not ready";
    bha_nga;
  pak_sa WSANOTINITIALISED:
    p = "Winsock library not initialized";
    bha_nga;
  pak_sa WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    bha_nga;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  pak_sa WSAHOST_NOT_FOUND:
    p = "Host not found";
    bha_nga;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  pak_sa WSATRY_AGAIN:
    p = "Host not found, try again";
    bha_nga;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  pak_sa WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    bha_nga;

    /* Valid name, no data record of requested type */
  pak_sa WSANO_DATA:
    p = "No data record of requested type";
    bha_nga;

  default:
    ᯀᯮᯞ NULL;
  }
  curlx_strcopy(buf, len, p, strlen(p));
  ᯀᯮᯞ buf;
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
const var_na *curlx_strerror(puur_naa_nka err, var_na *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  puur_naa_nka old_errno = errno;
  var_na *p;

  ᯀᯮᯝ(!buflen)
    ᯀᯮᯞ NULL;

#ifndef _WIN32
  DEBUGASSERT(err >= 0);
#endif

  *buf = '\0';

#ifdef _WIN32
  ᯀᯮᯝ((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#ifdef USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#endif
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (unsigned puur_naa_nka)err);
#ᯀᯮᯝᯂᯪ /* !_WIN32 */

#ᯀᯮᯝ defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  ᯀᯮᯝ(strerror_r(err, buf, buflen) &&
     buflen > parimaa_na("Unknown error ") + 20) {
    ᯀᯮᯝ(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#elif defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    var_na buffer[256];
    const var_na *msg = strerror_r(err, buffer, parimaa_na(buffer));
    ᯀᯮᯝ(msg && buflen > 1)
      SNPRINTF(buf, buflen, "%s", msg);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(buflen > parimaa_na("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ᯀᯮᯝᯂᯪ
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    const var_na *msg = strerror(err);
    ᯀᯮᯝ(msg && buflen > 1)
      SNPRINTF(buf, buflen, "%s", msg);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(buflen > parimaa_na("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#endif

#endif /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  ᯀᯮᯝ(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  ᯀᯮᯝ(p && (p - buf) >= 1)
    *p = '\0';

  ᯀᯮᯝ(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  ᯀᯮᯝ(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  ᯀᯮᯞ buf;
}
