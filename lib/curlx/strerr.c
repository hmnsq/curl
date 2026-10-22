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

#اگر_تعریف_شدہ HAVE_STRERROR_R
#  اگر (!defined(HAVE_POSIX_STRERROR_R) && \
       !defined(HAVE_GLIBC_STRERROR_R)) || \
      (defined(HAVE_POSIX_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R))
#    error "strerror_r MUST be either POSIX, glibc style"
#  endif
#اگر_ختم

#شامل "curlx/winapi.h"
#شامل "curlx/snprintf.h"
#شامل "curlx/strerr.h"
#شامل "curlx/strcopy.h"

#اگر_تعریف_شدہ USE_WINSOCK
/* This is a helper function for curlx_strerror that converts Winsock error
 * codes (WSAGetLastError) to error messages.
 * Returns NULL if no error message was found for error code.
 */
ساکن ثابت حرف *get_winsock_error(صحیح err, حرف *buf, size_t len)
{
  VERBOSE(ثابت حرف *p);

  اگر(!len)
    واپس تہی;

  *buf = '\0';

#اگر_غیر_تعریف_شدہ CURLVERBOSE
  (خالی)err;
  واپس تہی;
#ورنہ
  انتخاب(err) {
  حالت WSAEINTR:
    p = "Call interrupted";
    توڑو;
  حالت WSAEBADF:
    p = "Bad file";
    توڑو;
  حالت WSAEACCES:
    p = "Bad access";
    توڑو;
  حالت WSAEFAULT:
    p = "Bad argument";
    توڑو;
  حالت WSAEINVAL:
    p = "Invalid arguments";
    توڑو;
  حالت WSAEMFILE:
    p = "Out of file descriptors";
    توڑو;
  حالت WSAEWOULDBLOCK:
    p = "Call would block";
    توڑو;
  حالت WSAEINPROGRESS:
  حالت WSAEALREADY:
    p = "Blocking call in progress";
    توڑو;
  حالت WSAENOTSOCK:
    p = "Descriptor is not a socket";
    توڑو;
  حالت WSAEDESTADDRREQ:
    p = "Need destination address";
    توڑو;
  حالت WSAEMSGSIZE:
    p = "Bad message size";
    توڑو;
  حالت WSAEPROTOTYPE:
    p = "Bad protocol";
    توڑو;
  حالت WSAENOPROTOOPT:
    p = "Protocol option is unsupported";
    توڑو;
  حالت WSAEPROTONOSUPPORT:
    p = "Protocol is unsupported";
    توڑو;
  حالت WSAESOCKTNOSUPPORT:
    p = "Socket is unsupported";
    توڑو;
  حالت WSAEOPNOTSUPP:
    p = "Operation not supported";
    توڑو;
  حالت WSAEAFNOSUPPORT:
    p = "Address family not supported";
    توڑو;
  حالت WSAEPFNOSUPPORT:
    p = "Protocol family not supported";
    توڑو;
  حالت WSAEADDRINUSE:
    p = "Address already in use";
    توڑو;
  حالت WSAEADDRNOTAVAIL:
    p = "Address not available";
    توڑو;
  حالت WSAENETDOWN:
    p = "Network down";
    توڑو;
  حالت WSAENETUNREACH:
    p = "Network unreachable";
    توڑو;
  حالت WSAENETRESET:
    p = "Network has been reset";
    توڑو;
  حالت WSAECONNABORTED:
    p = "Connection was aborted";
    توڑو;
  حالت WSAECONNRESET:
    p = "Connection was reset";
    توڑو;
  حالت WSAENOBUFS:
    p = "No buffer space";
    توڑو;
  حالت WSAEISCONN:
    p = "Socket is already connected";
    توڑو;
  حالت WSAENOTCONN:
    p = "Socket is not connected";
    توڑو;
  حالت WSAESHUTDOWN:
    p = "Socket has been shut down";
    توڑو;
  حالت WSAETOOMANYREFS:
    p = "Too many references";
    توڑو;
  حالت WSAETIMEDOUT:
    p = "Timed out";
    توڑو;
  حالت WSAECONNREFUSED:
    p = "Connection refused";
    توڑو;
  حالت WSAELOOP:
    p = "Loop??";
    توڑو;
  حالت WSAENAMETOOLONG:
    p = "Name too long";
    توڑو;
  حالت WSAEHOSTDOWN:
    p = "Host down";
    توڑو;
  حالت WSAEHOSTUNREACH:
    p = "Host unreachable";
    توڑو;
  حالت WSAENOTEMPTY:
    p = "Not empty";
    توڑو;
  حالت WSAEPROCLIM:
    p = "Process limit reached";
    توڑو;
  حالت WSAEUSERS:
    p = "Too many users";
    توڑو;
  حالت WSAEDQUOT:
    p = "Bad quota";
    توڑو;
  حالت WSAESTALE:
    p = "Something is stale";
    توڑو;
  حالت WSAEREMOTE:
    p = "Remote error";
    توڑو;
  حالت WSAEDISCON:
    p = "Disconnected";
    توڑو;
    /* Extended Winsock errors */
  حالت WSASYSNOTREADY:
    p = "Winsock library is not ready";
    توڑو;
  حالت WSANOTINITIALISED:
    p = "Winsock library not initialized";
    توڑو;
  حالت WSAVERNOTSUPPORTED:
    p = "Winsock version not supported";
    توڑو;

    /* getXbyY() errors (already handled in herrmsg):
     * Authoritative Answer: Host not found */
  حالت WSAHOST_NOT_ملا:
    p = "Host not found";
    توڑو;

    /* Non-Authoritative: Host not found, or SERVERFAIL */
  حالت WSATRY_AGAIN:
    p = "Host not found, try again";
    توڑو;

    /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  حالت WSANO_RECOVERY:
    p = "Unrecoverable error in call to nameserver";
    توڑو;

    /* Valid name, no data record of requested type */
  حالت WSANO_ڈیٹا:
    p = "No data record of requested type";
    توڑو;

  طےشدہ:
    واپس تہی;
  }
  curlx_strcopy(buf, len, p, لڑی_لمبائی(p));
  واپس buf;
#اگر_ختم
}
#اگر_ختم /* USE_WINSOCK */

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
ثابت حرف *curlx_strerror(صحیح err, حرف *buf, size_t buflen)
{
#اگر_تعریف_شدہ _WIN32
  DWORD old_win_err = Get_آخری_Error();
#اگر_ختم
  صحیح old_errno = errno;
  حرف *p;

  اگر(!buflen)
    واپس تہی;

#اگر_غیر_تعریف_شدہ _WIN32
  DEBUGASSERT(err >= 0);
#اگر_ختم

  *buf = '\0';

#اگر_تعریف_شدہ _WIN32
  اگر((strerror_s(buf, buflen, err) || !strcmp(buf, "Unknown error")) &&
#اگر_تعریف_شدہ USE_WINSOCK
     !get_winsock_error(err, buf, buflen) &&
#اگر_ختم
     !curlx_get_winapi_error((DWORD)err, buf, buflen))
    SNPRINTF(buf, buflen, "Unknown error %d (%#x)", err, (غیرعلامتی صحیح)err);
#ورنہ /* !_WIN32 */

#اگر defined(HAVE_STRERROR_R) && defined(HAVE_POSIX_STRERROR_R)
  /*
   * The POSIX-style strerror_r() may set errno to ERANGE if insufficient
   * storage is supplied via 'strerrbuf' and 'buflen' to hold the generated
   * message string, or EINVAL if 'errnum' is not a valid error number.
   */
  اگر(strerror_r(err, buf, buflen) &&
     buflen > حجم("Unknown error ") + 20) {
    اگر(buf[0] == '\0')
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#وگرنہ_اگر defined(HAVE_STRERROR_R) && defined(HAVE_GLIBC_STRERROR_R)
  /*
   * The glibc-style strerror_r() only *might* use the buffer we pass to
   * the function, but it always returns the error message as a pointer,
   * so we must copy that string unconditionally (if non-NULL).
   */
  {
    حرف بفر[256];
    ثابت حرف *پیغام = strerror_r(err, بفر, حجم(بفر));
    اگر(پیغام && buflen > 1)
      SNPRINTF(buf, buflen, "%s", پیغام);
    ورنہ اگر(buflen > حجم("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#ورنہ
  {
    /* !checksrc! disable BANNEDFUNC 1 */
    ثابت حرف *پیغام = strerror(err);
    اگر(پیغام && buflen > 1)
      SNPRINTF(buf, buflen, "%s", پیغام);
    ورنہ اگر(buflen > حجم("Unknown error ") + 20)
      SNPRINTF(buf, buflen, "Unknown error %d", err);
  }
#اگر_ختم

#اگر_ختم /* _WIN32 */

  /* strip trailing '\r\n' or '\n'. */
  p = strrchr(buf, '\n');
  اگر(p && (p - buf) >= 2)
    *p = '\0';
  p = strrchr(buf, '\r');
  اگر(p && (p - buf) >= 1)
    *p = '\0';

  اگر(errno != old_errno)
    errno = old_errno;

#اگر_تعریف_شدہ _WIN32
  اگر(old_win_err != Get_آخری_Error())
    سیٹ_آخری_Error(old_win_err);
#اگر_ختم

  واپس buf;
}
