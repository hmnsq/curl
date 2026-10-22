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
    ܦܢܐ "No error";

  حالة CURLE_UNSUPPORTED_PROTOCOL:
    ܦܢܐ "Unsupported protocol";

  حالة CURLE_FAILED_INIT:
    ܦܢܐ "Failed initialization";

  حالة CURLE_URL_MALFORMAT:
    ܦܢܐ "URL using bad/illegal format or missing URL";

  حالة CURLE_NOT_BUILT_IN:
    ܦܢܐ "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  حالة CURLE_COULDNT_RESOLVE_PROXY:
    ܦܢܐ "Could not resolve proxy name";

  حالة CURLE_COULDNT_RESOLVE_HOST:
    ܦܢܐ "Could not resolve hostname";

  حالة CURLE_COULDNT_CONNECT:
    ܦܢܐ "Could not connect to server";

  حالة CURLE_WEIRD_SERVER_REPLY:
    ܦܢܐ "Weird server reply";

  حالة CURLE_REMOTE_ACCESS_DENIED:
    ܦܢܐ "Access denied to remote resource";

  حالة CURLE_FTP_ACCEPT_FAILED:
    ܦܢܐ "FTP: The server failed to connect to data port";

  حالة CURLE_FTP_ACCEPT_TIMEOUT:
    ܦܢܐ "FTP: Accepting server connect has timed out";

  حالة CURLE_FTP_PRET_FAILED:
    ܦܢܐ "FTP: The server did not accept the PRET command.";

  حالة CURLE_FTP_WEIRD_PASS_REPLY:
    ܦܢܐ "FTP: unknown PASS reply";

  حالة CURLE_FTP_WEIRD_PASV_REPLY:
    ܦܢܐ "FTP: unknown PASV reply";

  حالة CURLE_FTP_WEIRD_227_FORMAT:
    ܦܢܐ "FTP: unknown 227 response format";

  حالة CURLE_FTP_CANT_GET_HOST:
    ܦܢܐ "FTP: cannot figure out the host in the PASV response";

  حالة CURLE_HTTP2:
    ܦܢܐ "Error in the HTTP2 framing layer";

  حالة CURLE_FTP_COULDNT_SET_TYPE:
    ܦܢܐ "FTP: could not set file type";

  حالة CURLE_PARTIAL_FILE:
    ܦܢܐ "Transferred a partial file";

  حالة CURLE_FTP_COULDNT_RETR_FILE:
    ܦܢܐ "FTP: could not retrieve (RETR failed) the specified file";

  حالة CURLE_QUOTE_ERROR:
    ܦܢܐ "Quote command returned error";

  حالة CURLE_HTTP_RETURNED_ERROR:
    ܦܢܐ "HTTP response code said error";

  حالة CURLE_WRITE_ERROR:
    ܦܢܐ "Failed writing received data to disk/application";

  حالة CURLE_UPLOAD_FAILED:
    ܦܢܐ "Upload failed (at start/before it took off)";

  حالة CURLE_READ_ERROR:
    ܦܢܐ "Failed to open/read local data from file/application";

  حالة CURLE_OUT_OF_MEMORY:
    ܦܢܐ "Out of memory";

  حالة CURLE_OPERATION_TIMEDOUT:
    ܦܢܐ "Timeout was reached";

  حالة CURLE_FTP_PORT_FAILED:
    ܦܢܐ "FTP: command PORT failed";

  حالة CURLE_FTP_COULDNT_USE_REST:
    ܦܢܐ "FTP: command REST failed";

  حالة CURLE_RANGE_ERROR:
    ܦܢܐ "Requested range was not delivered by the server";

  حالة CURLE_SSL_CONNECT_ERROR:
    ܦܢܐ "SSL connect error";

  حالة CURLE_BAD_DOWNLOAD_RESUME:
    ܦܢܐ "Could not resume download";

  حالة CURLE_FILE_COULDNT_READ_FILE:
    ܦܢܐ "Could not read a file:// file";

  حالة CURLE_LDAP_CANNOT_BIND:
    ܦܢܐ "LDAP: cannot bind";

  حالة CURLE_LDAP_SEARCH_FAILED:
    ܦܢܐ "LDAP: search failed";

  حالة CURLE_ABORTED_BY_CALLBACK:
    ܦܢܐ "Operation was aborted by an application callback";

  حالة CURLE_BAD_FUNCTION_ARGUMENT:
    ܦܢܐ "A libcurl function was given a bad argument";

  حالة CURLE_INTERFACE_FAILED:
    ܦܢܐ "Failed binding local connection end";

  حالة CURLE_TOO_MANY_REDIRECTS:
    ܦܢܐ "Number of redirects hit maximum amount";

  حالة CURLE_UNKNOWN_OPTION:
    ܦܢܐ "An unknown option was passed in to libcurl";

  حالة CURLE_SETOPT_OPTION_SYNTAX:
    ܦܢܐ "Malformed option provided in a setopt";

  حالة CURLE_GOT_NOTHING:
    ܦܢܐ "Server returned nothing (no headers, no data)";

  حالة CURLE_SSL_ENGINE_NOTFOUND:
    ܦܢܐ "SSL crypto engine not found";

  حالة CURLE_SSL_ENGINE_SETFAILED:
    ܦܢܐ "Can not set SSL crypto engine as default";

  حالة CURLE_SSL_ENGINE_INITFAILED:
    ܦܢܐ "Failed to initialize SSL crypto engine";

  حالة CURLE_SEND_ERROR:
    ܦܢܐ "Failed sending data to the peer";

  حالة CURLE_RECV_ERROR:
    ܦܢܐ "Failure when receiving data from the peer";

  حالة CURLE_SSL_CERTPROBLEM:
    ܦܢܐ "Problem with the local SSL certificate";

  حالة CURLE_SSL_CIPHER:
    ܦܢܐ "Could not use specified SSL cipher";

  حالة CURLE_PEER_FAILED_VERIFICATION:
    ܦܢܐ "SSL peer certificate or SSH remote key was not OK";

  حالة CURLE_SSL_CACERT_BADFILE:
    ܦܢܐ "Problem with the SSL CA cert (path? access rights?)";

  حالة CURLE_BAD_CONTENT_ENCODING:
    ܦܢܐ "Unrecognized or bad HTTP Content or Transfer-Encoding";

  حالة CURLE_FILESIZE_EXCEEDED:
    ܦܢܐ "Maximum file size exceeded";

  حالة CURLE_USE_SSL_FAILED:
    ܦܢܐ "Requested SSL level failed";

  حالة CURLE_SSL_SHUTDOWN_FAILED:
    ܦܢܐ "Failed to shut down the SSL connection";

  حالة CURLE_SSL_CRL_BADFILE:
    ܦܢܐ "Failed to load CRL file (path? access rights?, format?)";

  حالة CURLE_SSL_ISSUER_ERROR:
    ܦܢܐ "Issuer check against peer certificate failed";

  حالة CURLE_SEND_FAIL_REWIND:
    ܦܢܐ "Send failed since rewinding of the data stream failed";

  حالة CURLE_LOGIN_DENIED:
    ܦܢܐ "Login denied";

  حالة CURLE_TFTP_NOTFOUND:
    ܦܢܐ "TFTP: File Not Found";

  حالة CURLE_TFTP_PERM:
    ܦܢܐ "TFTP: Access Violation";

  حالة CURLE_REMOTE_DISK_FULL:
    ܦܢܐ "Disk full or allocation exceeded";

  حالة CURLE_TFTP_ILLEGAL:
    ܦܢܐ "TFTP: Illegal operation";

  حالة CURLE_TFTP_UNKNOWNID:
    ܦܢܐ "TFTP: Unknown transfer ID";

  حالة CURLE_REMOTE_FILE_EXISTS:
    ܦܢܐ "Remote file already exists";

  حالة CURLE_TFTP_NOSUCHUSER:
    ܦܢܐ "TFTP: No such user";

  حالة CURLE_REMOTE_FILE_NOT_FOUND:
    ܦܢܐ "Remote file not found";

  حالة CURLE_SSH:
    ܦܢܐ "Error in the SSH layer";

  حالة CURLE_AGAIN:
    ܦܢܐ "Socket not ready for send/recv";

  حالة CURLE_RTSP_CSEQ_ERROR:
    ܦܢܐ "RTSP CSeq mismatch or invalid CSeq";

  حالة CURLE_RTSP_SESSION_ERROR:
    ܦܢܐ "RTSP session error";

  حالة CURLE_FTP_BAD_FILE_LIST:
    ܦܢܐ "Unable to parse FTP file list";

  حالة CURLE_CHUNK_FAILED:
    ܦܢܐ "Chunk callback failed";

  حالة CURLE_NO_CONNECTION_AVAILABLE:
    ܦܢܐ "The max connection limit is reached";

  حالة CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    ܦܢܐ "SSL public key does not match pinned public key";

  حالة CURLE_SSL_INVALIDCERTSTATUS:
    ܦܢܐ "SSL server certificate status verification FAILED";

  حالة CURLE_HTTP2_STREAM:
    ܦܢܐ "Stream error in the HTTP/2 framing layer";

  حالة CURLE_RECURSIVE_API_CALL:
    ܦܢܐ "API function called from within callback";

  حالة CURLE_AUTH_ERROR:
    ܦܢܐ "An authentication function returned an error";

  حالة CURLE_HTTP3:
    ܦܢܐ "HTTP/3 error";

  حالة CURLE_QUIC_CONNECT_ERROR:
    ܦܢܐ "QUIC connection error";

  حالة CURLE_PROXY:
    ܦܢܐ "proxy handshake error";

  حالة CURLE_SSL_CLIENTCERT:
    ܦܢܐ "SSL Client Certificate required";

  حالة CURLE_UNRECOVERABLE_POLL:
    ܦܢܐ "Unrecoverable error in select/poll";

  حالة CURLE_TOO_LARGE:
    ܦܢܐ "A value or data field grew larger than allowed";

  حالة CURLE_ECH_REQUIRED:
    ܦܢܐ "ECH attempted but failed";

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
  ܦܢܐ "Unknown error";
#ܐܚܪܢܐ
  ܐܢ(!error)
    ܦܢܐ "No error";
  ܐܚܪܢܐ
    ܦܢܐ "Error";
#endif
}

ثابت حرف *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  بدل(error) {
  حالة CURLM_CALL_MULTI_PERFORM:
    ܦܢܐ "Please call curl_multi_perform() soon";

  حالة CURLM_OK:
    ܦܢܐ "No error";

  حالة CURLM_BAD_HANDLE:
    ܦܢܐ "Invalid multi handle";

  حالة CURLM_BAD_EASY_HANDLE:
    ܦܢܐ "Invalid easy handle";

  حالة CURLM_OUT_OF_MEMORY:
    ܦܢܐ "Out of memory";

  حالة CURLM_INTERNAL_ERROR:
    ܦܢܐ "Internal error";

  حالة CURLM_BAD_SOCKET:
    ܦܢܐ "Invalid socket argument";

  حالة CURLM_UNKNOWN_OPTION:
    ܦܢܐ "Unknown option";

  حالة CURLM_ADDED_ALREADY:
    ܦܢܐ "The easy handle is already added to a multi handle";

  حالة CURLM_RECURSIVE_API_CALL:
    ܦܢܐ "API function called from within callback";

  حالة CURLM_WAKEUP_FAILURE:
    ܦܢܐ "Wakeup is unavailable or failed";

  حالة CURLM_BAD_FUNCTION_ARGUMENT:
    ܦܢܐ "A libcurl function was given a bad argument";

  حالة CURLM_ABORTED_BY_CALLBACK:
    ܦܢܐ "Operation was aborted by an application callback";

  حالة CURLM_UNRECOVERABLE_POLL:
    ܦܢܐ "Unrecoverable error in select/poll";

  حالة CURLM_LAST:
    اكسر;
  }

  ܦܢܐ "Unknown error";
#ܐܚܪܢܐ
  ܐܢ(error == CURLM_OK)
    ܦܢܐ "No error";
  ܐܚܪܢܐ
    ܦܢܐ "Error";
#endif
}

ثابت حرف *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  بدل(error) {
  حالة CURLSHE_OK:
    ܦܢܐ "No error";

  حالة CURLSHE_BAD_OPTION:
    ܦܢܐ "Unknown share option";

  حالة CURLSHE_IN_USE:
    ܦܢܐ "Share currently in use";

  حالة CURLSHE_INVALID:
    ܦܢܐ "Invalid share handle";

  حالة CURLSHE_NOMEM:
    ܦܢܐ "Out of memory";

  حالة CURLSHE_NOT_BUILT_IN:
    ܦܢܐ "Feature not enabled in this library";

  حالة CURLSHE_LAST:
    اكسر;
  }

  ܦܢܐ "CURLSHcode unknown";
#ܐܚܪܢܐ
  ܐܢ(error == CURLSHE_OK)
    ܦܢܐ "No error";
  ܐܚܪܢܐ
    ܦܢܐ "Error";
#endif
}

ثابت حرف *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  بدل(error) {
  حالة CURLUE_OK:
    ܦܢܐ "No error";

  حالة CURLUE_BAD_HANDLE:
    ܦܢܐ "An invalid CURLU pointer was passed as argument";

  حالة CURLUE_BAD_PARTPOINTER:
    ܦܢܐ "An invalid 'part' argument was passed as argument";

  حالة CURLUE_MALFORMED_INPUT:
    ܦܢܐ "Malformed input to a URL function";

  حالة CURLUE_BAD_PORT_NUMBER:
    ܦܢܐ "Port number was not a decimal number between 0 and 65535";

  حالة CURLUE_UNSUPPORTED_SCHEME:
    ܦܢܐ "Unsupported URL scheme";

  حالة CURLUE_URLDECODE:
    ܦܢܐ "URL decode error, most likely because of rubbish in the input";

  حالة CURLUE_OUT_OF_MEMORY:
    ܦܢܐ "A memory function failed";

  حالة CURLUE_USER_NOT_ALLOWED:
    ܦܢܐ "Credentials was passed in the URL when prohibited";

  حالة CURLUE_UNKNOWN_PART:
    ܦܢܐ "An unknown part ID was passed to a URL API function";

  حالة CURLUE_NO_SCHEME:
    ܦܢܐ "No scheme present";

  حالة CURLUE_NO_USER:
    ܦܢܐ "No user present";

  حالة CURLUE_NO_PASSWORD:
    ܦܢܐ "No password present";

  حالة CURLUE_NO_OPTIONS:
    ܦܢܐ "No options present";

  حالة CURLUE_NO_HOST:
    ܦܢܐ "No host present";

  حالة CURLUE_NO_PORT:
    ܦܢܐ "No port number present";

  حالة CURLUE_NO_QUERY:
    ܦܢܐ "No query present";

  حالة CURLUE_NO_FRAGMENT:
    ܦܢܐ "No fragment present";

  حالة CURLUE_NO_ZONEID:
    ܦܢܐ "No zoneid present";

  حالة CURLUE_BAD_LOGIN:
    ܦܢܐ "Bad login";

  حالة CURLUE_BAD_IPV6:
    ܦܢܐ "Bad IPv6 address";

  حالة CURLUE_BAD_HOSTNAME:
    ܦܢܐ "Bad hostname";

  حالة CURLUE_BAD_FILE_URL:
    ܦܢܐ "Bad file:// URL";

  حالة CURLUE_BAD_SLASHES:
    ܦܢܐ "Unsupported number of slashes following scheme";

  حالة CURLUE_BAD_SCHEME:
    ܦܢܐ "Bad scheme";

  حالة CURLUE_BAD_PATH:
    ܦܢܐ "Bad path";

  حالة CURLUE_BAD_FRAGMENT:
    ܦܢܐ "Bad fragment";

  حالة CURLUE_BAD_QUERY:
    ܦܢܐ "Bad query";

  حالة CURLUE_BAD_PASSWORD:
    ܦܢܐ "Bad password";

  حالة CURLUE_BAD_USER:
    ܦܢܐ "Bad user";

  حالة CURLUE_LACKS_IDN:
    ܦܢܐ "libcurl lacks IDN support";

  حالة CURLUE_TOO_LARGE:
    ܦܢܐ "A value or data field is larger than allowed";

  حالة CURLUE_BACKSLASH:
    ܦܢܐ "Found a backslash where a forward slash was expected";

  حالة CURLUE_LAST:
    اكسر;
  }

  ܦܢܐ "CURLUcode unknown";
#ܐܚܪܢܐ
  ܐܢ(error == CURLUE_OK)
    ܦܢܐ "No error";
  ܐܚܪܢܐ
    ܦܢܐ "Error";
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

  ܐܢ(!buflen)
    ܦܢܐ عدم;

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

  ܐܢ(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  ܐܚܪܢܐ {
    حرف msgbuf[256];
    ܐܢ(curlx_get_winapi_error((DWORD)err, msgbuf, حجم(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    ܐܚܪܢܐ
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#ܐܚܪܢܐ /* CURLVERBOSE */
  ܐܢ(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  ܐܚܪܢܐ
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  ܐܢ(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  ܐܢ(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  ܦܢܐ buf;
}
#endif /* USE_WINDOWS_SSPI */
