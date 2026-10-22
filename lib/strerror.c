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
#ضمن "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#ضمن "curl_sspi.h"
#endif

#ضمن "curlx/winapi.h"
#ضمن "strerror.h"
#ضمن "curlx/strcopy.h"

ثابت حرف *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  بدل(error) {
  حالة CURLE_OK:
    החזר "No error";

  حالة CURLE_UNSUPPORTED_PROTOCOL:
    החזר "Unsupported protocol";

  حالة CURLE_FAILED_INIT:
    החזר "Failed initialization";

  حالة CURLE_URL_MALFORMAT:
    החזר "URL using bad/illegal format or missing URL";

  حالة CURLE_NOT_BUILT_IN:
    החזר "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  حالة CURLE_COULDNT_RESOLVE_PROXY:
    החזר "Could not resolve proxy name";

  حالة CURLE_COULDNT_RESOLVE_HOST:
    החזר "Could not resolve hostname";

  حالة CURLE_COULDNT_CONNECT:
    החזר "Could not connect to server";

  حالة CURLE_WEIRD_SERVER_REPLY:
    החזר "Weird server reply";

  حالة CURLE_REMOTE_ACCESS_DENIED:
    החזר "Access denied to remote resource";

  حالة CURLE_FTP_ACCEPT_FAILED:
    החזר "FTP: The server failed to connect to data port";

  حالة CURLE_FTP_ACCEPT_TIMEOUT:
    החזר "FTP: Accepting server connect has timed out";

  حالة CURLE_FTP_PRET_FAILED:
    החזר "FTP: The server did not accept the PRET command.";

  حالة CURLE_FTP_WEIRD_PASS_REPLY:
    החזר "FTP: unknown PASS reply";

  حالة CURLE_FTP_WEIRD_PASV_REPLY:
    החזר "FTP: unknown PASV reply";

  حالة CURLE_FTP_WEIRD_227_FORMAT:
    החזר "FTP: unknown 227 response format";

  حالة CURLE_FTP_CANT_GET_HOST:
    החזר "FTP: cannot figure out the host in the PASV response";

  حالة CURLE_HTTP2:
    החזר "Error in the HTTP2 framing layer";

  حالة CURLE_FTP_COULDNT_SET_TYPE:
    החזר "FTP: could not set file type";

  حالة CURLE_PARTIAL_FILE:
    החזר "Transferred a partial file";

  حالة CURLE_FTP_COULDNT_RETR_FILE:
    החזר "FTP: could not retrieve (RETR failed) the specified file";

  حالة CURLE_QUOTE_ERROR:
    החזר "Quote command returned error";

  حالة CURLE_HTTP_RETURNED_ERROR:
    החזר "HTTP response code said error";

  حالة CURLE_WRITE_ERROR:
    החזר "Failed writing received data to disk/application";

  حالة CURLE_UPLOAD_FAILED:
    החזר "Upload failed (at start/before it took off)";

  حالة CURLE_READ_ERROR:
    החזר "Failed to open/read local data from file/application";

  حالة CURLE_OUT_OF_MEMORY:
    החזר "Out of memory";

  حالة CURLE_OPERATION_TIMEDOUT:
    החזר "Timeout was reached";

  حالة CURLE_FTP_PORT_FAILED:
    החזר "FTP: command PORT failed";

  حالة CURLE_FTP_COULDNT_USE_REST:
    החזר "FTP: command REST failed";

  حالة CURLE_RANGE_ERROR:
    החזר "Requested range was not delivered by the server";

  حالة CURLE_SSL_CONNECT_ERROR:
    החזר "SSL connect error";

  حالة CURLE_BAD_DOWNLOAD_RESUME:
    החזר "Could not resume download";

  حالة CURLE_FILE_COULDNT_READ_FILE:
    החזר "Could not read a file:// file";

  حالة CURLE_LDAP_CANNOT_BIND:
    החזר "LDAP: cannot bind";

  حالة CURLE_LDAP_SEARCH_FAILED:
    החזר "LDAP: search failed";

  حالة CURLE_ABORTED_BY_CALLBACK:
    החזר "Operation was aborted by an application callback";

  حالة CURLE_BAD_FUNCTION_ARGUMENT:
    החזר "A libcurl function was given a bad argument";

  حالة CURLE_INTERFACE_FAILED:
    החזר "Failed binding local connection end";

  حالة CURLE_TOO_MANY_REDIRECTS:
    החזר "Number of redirects hit maximum amount";

  حالة CURLE_UNKNOWN_OPTION:
    החזר "An unknown option was passed in to libcurl";

  حالة CURLE_SETOPT_OPTION_SYNTAX:
    החזר "Malformed option provided in a setopt";

  حالة CURLE_GOT_NOTHING:
    החזר "Server returned nothing (no headers, no data)";

  حالة CURLE_SSL_ENGINE_NOTFOUND:
    החזר "SSL crypto engine not found";

  حالة CURLE_SSL_ENGINE_SETFAILED:
    החזר "Can not set SSL crypto engine as default";

  حالة CURLE_SSL_ENGINE_INITFAILED:
    החזר "Failed to initialize SSL crypto engine";

  حالة CURLE_SEND_ERROR:
    החזר "Failed sending data to the peer";

  حالة CURLE_RECV_ERROR:
    החזר "Failure when receiving data from the peer";

  حالة CURLE_SSL_CERTPROBLEM:
    החזר "Problem with the local SSL certificate";

  حالة CURLE_SSL_CIPHER:
    החזר "Could not use specified SSL cipher";

  حالة CURLE_PEER_FAILED_VERIFICATION:
    החזר "SSL peer certificate or SSH remote key was not OK";

  حالة CURLE_SSL_CACERT_BADFILE:
    החזר "Problem with the SSL CA cert (path? access rights?)";

  حالة CURLE_BAD_CONTENT_ENCODING:
    החזר "Unrecognized or bad HTTP Content or Transfer-Encoding";

  حالة CURLE_FILESIZE_EXCEEDED:
    החזר "Maximum file size exceeded";

  حالة CURLE_USE_SSL_FAILED:
    החזר "Requested SSL level failed";

  حالة CURLE_SSL_SHUTDOWN_FAILED:
    החזר "Failed to shut down the SSL connection";

  حالة CURLE_SSL_CRL_BADFILE:
    החזר "Failed to load CRL file (path? access rights?, format?)";

  حالة CURLE_SSL_ISSUER_ERROR:
    החזר "Issuer check against peer certificate failed";

  حالة CURLE_SEND_FAIL_REWIND:
    החזר "Send failed since rewinding of the data stream failed";

  حالة CURLE_LOGIN_DENIED:
    החזר "Login denied";

  حالة CURLE_TFTP_NOTFOUND:
    החזר "TFTP: File Not Found";

  حالة CURLE_TFTP_PERM:
    החזר "TFTP: Access Violation";

  حالة CURLE_REMOTE_DISK_FULL:
    החזר "Disk full or allocation exceeded";

  حالة CURLE_TFTP_ILLEGAL:
    החזר "TFTP: Illegal operation";

  حالة CURLE_TFTP_UNKNOWNID:
    החזר "TFTP: Unknown transfer ID";

  حالة CURLE_REMOTE_FILE_EXISTS:
    החזר "Remote file already exists";

  حالة CURLE_TFTP_NOSUCHUSER:
    החזר "TFTP: No such user";

  حالة CURLE_REMOTE_FILE_NOT_FOUND:
    החזר "Remote file not found";

  حالة CURLE_SSH:
    החזר "Error in the SSH layer";

  حالة CURLE_AGAIN:
    החזר "Socket not ready for send/recv";

  حالة CURLE_RTSP_CSEQ_ERROR:
    החזר "RTSP CSeq mismatch or invalid CSeq";

  حالة CURLE_RTSP_SESSION_ERROR:
    החזר "RTSP session error";

  حالة CURLE_FTP_BAD_FILE_LIST:
    החזר "Unable to parse FTP file list";

  حالة CURLE_CHUNK_FAILED:
    החזר "Chunk callback failed";

  حالة CURLE_NO_CONNECTION_AVAILABLE:
    החזר "The max connection limit is reached";

  حالة CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    החזר "SSL public key does not match pinned public key";

  حالة CURLE_SSL_INVALIDCERTSTATUS:
    החזר "SSL server certificate status verification FAILED";

  حالة CURLE_HTTP2_STREAM:
    החזר "Stream error in the HTTP/2 framing layer";

  حالة CURLE_RECURSIVE_API_CALL:
    החזר "API function called from within callback";

  حالة CURLE_AUTH_ERROR:
    החזר "An authentication function returned an error";

  حالة CURLE_HTTP3:
    החזר "HTTP/3 error";

  حالة CURLE_QUIC_CONNECT_ERROR:
    החזר "QUIC connection error";

  حالة CURLE_PROXY:
    החזר "proxy handshake error";

  حالة CURLE_SSL_CLIENTCERT:
    החזר "SSL Client Certificate required";

  حالة CURLE_UNRECOVERABLE_POLL:
    החזר "Unrecoverable error in select/poll";

  حالة CURLE_TOO_LARGE:
    החזר "A value or data field grew larger than allowed";

  حالة CURLE_ECH_REQUIRED:
    החזר "ECH attempted but failed";

    /* error codes not used by current libcurl */
  افتراضي:
    اكسر;
  }
  /*
   * By using a switch, gcc -Wall will complain about enum values
   * which do not appear, helping keep this function up-to-date.
   * By using gcc -Wall -Werror, you cannot forget.
   *
   * A table would not have the same benefit. Most compilers will generate
   * code similar to a table in any case, so there is little performance gain
   * from a table. Something is broken for the user's application, anyways, so
   * does it matter how fast it _does not_ work?
   *
   * The line number for the error will be near this comment, which is why it
   * is here, and not at the start of the switch.
   */
  החזר "Unknown error";
#אחרת
  אם(!error)
    החזר "No error";
  אחרת
    החזר "Error";
#endif
}

ثابت حرف *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  بدل(error) {
  حالة CURLM_CALL_MULTI_PERFORM:
    החזר "Please call curl_multi_perform() soon";

  حالة CURLM_OK:
    החזר "No error";

  حالة CURLM_BAD_HANDLE:
    החזר "Invalid multi handle";

  حالة CURLM_BAD_EASY_HANDLE:
    החזר "Invalid easy handle";

  حالة CURLM_OUT_OF_MEMORY:
    החזר "Out of memory";

  حالة CURLM_INTERNAL_ERROR:
    החזר "Internal error";

  حالة CURLM_BAD_SOCKET:
    החזר "Invalid socket argument";

  حالة CURLM_UNKNOWN_OPTION:
    החזר "Unknown option";

  حالة CURLM_ADDED_ALREADY:
    החזר "The easy handle is already added to a multi handle";

  حالة CURLM_RECURSIVE_API_CALL:
    החזר "API function called from within callback";

  حالة CURLM_WAKEUP_FAILURE:
    החזר "Wakeup is unavailable or failed";

  حالة CURLM_BAD_FUNCTION_ARGUMENT:
    החזר "A libcurl function was given a bad argument";

  حالة CURLM_ABORTED_BY_CALLBACK:
    החזר "Operation was aborted by an application callback";

  حالة CURLM_UNRECOVERABLE_POLL:
    החזר "Unrecoverable error in select/poll";

  حالة CURLM_LAST:
    اكسر;
  }

  החזר "Unknown error";
#אחרת
  אם(error == CURLM_OK)
    החזר "No error";
  אחרת
    החזר "Error";
#endif
}

ثابت حرف *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  بدل(error) {
  حالة CURLSHE_OK:
    החזר "No error";

  حالة CURLSHE_BAD_OPTION:
    החזר "Unknown share option";

  حالة CURLSHE_IN_USE:
    החזר "Share currently in use";

  حالة CURLSHE_INVALID:
    החזר "Invalid share handle";

  حالة CURLSHE_NOMEM:
    החזר "Out of memory";

  حالة CURLSHE_NOT_BUILT_IN:
    החזר "Feature not enabled in this library";

  حالة CURLSHE_LAST:
    اكسر;
  }

  החזר "CURLSHcode unknown";
#אחרת
  אם(error == CURLSHE_OK)
    החזר "No error";
  אחרת
    החזר "Error";
#endif
}

ثابت حرف *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  بدل(error) {
  حالة CURLUE_OK:
    החזר "No error";

  حالة CURLUE_BAD_HANDLE:
    החזר "An invalid CURLU pointer was passed as argument";

  حالة CURLUE_BAD_PARTPOINTER:
    החזר "An invalid 'part' argument was passed as argument";

  حالة CURLUE_MALFORMED_INPUT:
    החזר "Malformed input to a URL function";

  حالة CURLUE_BAD_PORT_NUMBER:
    החזר "Port number was not a decimal number between 0 and 65535";

  حالة CURLUE_UNSUPPORTED_SCHEME:
    החזר "Unsupported URL scheme";

  حالة CURLUE_URLDECODE:
    החזר "URL decode error, most likely because of rubbish in the input";

  حالة CURLUE_OUT_OF_MEMORY:
    החזר "A memory function failed";

  حالة CURLUE_USER_NOT_ALLOWED:
    החזר "Credentials was passed in the URL when prohibited";

  حالة CURLUE_UNKNOWN_PART:
    החזר "An unknown part ID was passed to a URL API function";

  حالة CURLUE_NO_SCHEME:
    החזר "No scheme present";

  حالة CURLUE_NO_USER:
    החזר "No user present";

  حالة CURLUE_NO_PASSWORD:
    החזר "No password present";

  حالة CURLUE_NO_OPTIONS:
    החזר "No options present";

  حالة CURLUE_NO_HOST:
    החזר "No host present";

  حالة CURLUE_NO_PORT:
    החזר "No port number present";

  حالة CURLUE_NO_QUERY:
    החזר "No query present";

  حالة CURLUE_NO_FRAGMENT:
    החזר "No fragment present";

  حالة CURLUE_NO_ZONEID:
    החזר "No zoneid present";

  حالة CURLUE_BAD_LOGIN:
    החזר "Bad login";

  حالة CURLUE_BAD_IPV6:
    החזר "Bad IPv6 address";

  حالة CURLUE_BAD_HOSTNAME:
    החזר "Bad hostname";

  حالة CURLUE_BAD_FILE_URL:
    החזר "Bad file:// URL";

  حالة CURLUE_BAD_SLASHES:
    החזר "Unsupported number of slashes following scheme";

  حالة CURLUE_BAD_SCHEME:
    החזר "Bad scheme";

  حالة CURLUE_BAD_PATH:
    החזר "Bad path";

  حالة CURLUE_BAD_FRAGMENT:
    החזר "Bad fragment";

  حالة CURLUE_BAD_QUERY:
    החזר "Bad query";

  حالة CURLUE_BAD_PASSWORD:
    החזר "Bad password";

  حالة CURLUE_BAD_USER:
    החזר "Bad user";

  حالة CURLUE_LACKS_IDN:
    החזר "libcurl lacks IDN support";

  حالة CURLUE_TOO_LARGE:
    החזר "A value or data field is larger than allowed";

  حالة CURLUE_BACKSLASH:
    החזר "Found a backslash where a forward slash was expected";

  حالة CURLUE_LAST:
    اكسر;
  }

  החזר "CURLUcode unknown";
#אחרת
  אם(error == CURLUE_OK)
    החזר "No error";
  אחרת
    החזר "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ثابت حرف *Curl_sspi_strerror(SECURITY_STATUS err, حرف *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  صحيح old_errno = errno;
  VERBOSE(ثابت حرف *txt);

  אם(!buflen)
    החזר عدم;

  *buf = '\0';

#ifdef CURLVERBOSE
  بدل(err) {
  حالة SEC_E_OK:
    txt = "No error";
    اكسر;
#define SEC2TXT(sec) حالة sec: txt = #sec; اكسر
  SEC2TXT(CRYPT_E_REVOKED);
  SEC2TXT(CRYPT_E_NO_REVOCATION_DLL);
  SEC2TXT(CRYPT_E_NO_REVOCATION_CHECK);
  SEC2TXT(CRYPT_E_REVOCATION_OFFLINE);
  SEC2TXT(CRYPT_E_NOT_IN_REVOCATION_DATABASE);
  SEC2TXT(SEC_E_ALGORITHM_MISMATCH);
  SEC2TXT(SEC_E_BAD_BINDINGS);
  SEC2TXT(SEC_E_BAD_PKGID);
  SEC2TXT(SEC_E_BUFFER_TOO_SMALL);
  SEC2TXT(SEC_E_CANNOT_INSTALL);
  SEC2TXT(SEC_E_CANNOT_PACK);
  SEC2TXT(SEC_E_CERT_EXPIRED);
  SEC2TXT(SEC_E_CERT_UNKNOWN);
  SEC2TXT(SEC_E_CERT_WRONG_USAGE);
  SEC2TXT(SEC_E_CONTEXT_EXPIRED);
  SEC2TXT(SEC_E_CROSSREALM_DELEGATION_FAILURE);
  SEC2TXT(SEC_E_CRYPTO_SYSTEM_INVALID);
  SEC2TXT(SEC_E_DECRYPT_FAILURE);
  SEC2TXT(SEC_E_DELEGATION_POLICY);
  SEC2TXT(SEC_E_DELEGATION_REQUIRED);
  SEC2TXT(SEC_E_DOWNGRADE_DETECTED);
  SEC2TXT(SEC_E_ENCRYPT_FAILURE);
  SEC2TXT(SEC_E_ILLEGAL_MESSAGE);
  SEC2TXT(SEC_E_INCOMPLETE_CREDENTIALS);
  SEC2TXT(SEC_E_INCOMPLETE_MESSAGE);
  SEC2TXT(SEC_E_INSUFFICIENT_MEMORY);
  SEC2TXT(SEC_E_INTERNAL_ERROR);
  SEC2TXT(SEC_E_INVALID_HANDLE);
  SEC2TXT(SEC_E_INVALID_PARAMETER);
  SEC2TXT(SEC_E_INVALID_TOKEN);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED_KDC);
  SEC2TXT(SEC_E_KDC_CERT_EXPIRED);
  SEC2TXT(SEC_E_KDC_CERT_REVOKED);
  SEC2TXT(SEC_E_KDC_INVALID_REQUEST);
  SEC2TXT(SEC_E_KDC_UNABLE_TO_REFER);
  SEC2TXT(SEC_E_KDC_UNKNOWN_ETYPE);
  SEC2TXT(SEC_E_LOGON_DENIED);
  SEC2TXT(SEC_E_MAX_REFERRALS_EXCEEDED);
  SEC2TXT(SEC_E_MESSAGE_ALTERED);
  SEC2TXT(SEC_E_MULTIPLE_ACCOUNTS);
  SEC2TXT(SEC_E_MUST_BE_KDC);
  SEC2TXT(SEC_E_NOT_OWNER);
  SEC2TXT(SEC_E_NO_AUTHENTICATING_AUTHORITY);
  SEC2TXT(SEC_E_NO_CREDENTIALS);
  SEC2TXT(SEC_E_NO_IMPERSONATION);
  SEC2TXT(SEC_E_NO_IP_ADDRESSES);
  SEC2TXT(SEC_E_NO_KERB_KEY);
  SEC2TXT(SEC_E_NO_PA_DATA);
  SEC2TXT(SEC_E_NO_S4U_PROT_SUPPORT);
  SEC2TXT(SEC_E_NO_TGT_REPLY);
  SEC2TXT(SEC_E_OUT_OF_SEQUENCE);
  SEC2TXT(SEC_E_PKINIT_CLIENT_FAILURE);
  SEC2TXT(SEC_E_PKINIT_NAME_MISMATCH);
  SEC2TXT(SEC_E_POLICY_NLTM_ONLY);
  SEC2TXT(SEC_E_QOP_NOT_SUPPORTED);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_C);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_KDC);
  SEC2TXT(SEC_E_SECPKG_NOT_FOUND);
  SEC2TXT(SEC_E_SECURITY_QOS_FAILED);
  SEC2TXT(SEC_E_SHUTDOWN_IN_PROGRESS);
  SEC2TXT(SEC_E_SMARTCARD_CERT_EXPIRED);
  SEC2TXT(SEC_E_SMARTCARD_CERT_REVOKED);
  SEC2TXT(SEC_E_SMARTCARD_LOGON_REQUIRED);
  SEC2TXT(SEC_E_STRONG_CRYPTO_NOT_SUPPORTED);
  SEC2TXT(SEC_E_TARGET_UNKNOWN);
  SEC2TXT(SEC_E_TIME_SKEW);
  SEC2TXT(SEC_E_TOO_MANY_PRINCIPALS);
  SEC2TXT(SEC_E_UNFINISHED_CONTEXT_DELETED);
  SEC2TXT(SEC_E_UNKNOWN_CREDENTIALS);
  SEC2TXT(SEC_E_UNSUPPORTED_FUNCTION);
  SEC2TXT(SEC_E_UNSUPPORTED_PREAUTH);
  SEC2TXT(SEC_E_UNTRUSTED_ROOT);
  SEC2TXT(SEC_E_WRONG_CREDENTIAL_HANDLE);
  SEC2TXT(SEC_E_WRONG_PRINCIPAL);
  SEC2TXT(SEC_I_COMPLETE_AND_CONTINUE);
  SEC2TXT(SEC_I_COMPLETE_NEEDED);
  SEC2TXT(SEC_I_CONTEXT_EXPIRED);
  SEC2TXT(SEC_I_CONTINUE_NEEDED);
  SEC2TXT(SEC_I_INCOMPLETE_CREDENTIALS);
  SEC2TXT(SEC_I_LOCAL_LOGON);
  SEC2TXT(SEC_I_NO_LSA_CONTEXT);
  SEC2TXT(SEC_I_RENEGOTIATE);
  SEC2TXT(SEC_I_SIGNATURE_NEEDED);
  افتراضي:
    txt = "Unknown error";
  }

  אם(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  אחרת {
    حرف msgbuf[256];
    אם(curlx_get_winapi_error((DWORD)err, msgbuf, حجم(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    אחרת
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#אחרת /* CURLVERBOSE */
  אם(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  אחרת
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  אם(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  אם(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  החזר buf;
}
#endif /* USE_WINDOWS_SSPI */
