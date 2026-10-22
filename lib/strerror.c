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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#અન્તર્ભાવ "curl_sspi.h"
#endif

#અન્તર્ભાવ "curlx/winapi.h"
#અન્તર્ભાવ "strerror.h"
#અન્તર્ભાવ "curlx/strcopy.h"

અચલ અક્ષર *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  પસંદગી(error) {
  સ્થિતિ CURLE_OK:
    પાછા "No error";

  સ્થિતિ CURLE_UNSUPPORTED_PROTOCOL:
    પાછા "Unsupported protocol";

  સ્થિતિ CURLE_FAILED_INIT:
    પાછા "Failed initialization";

  સ્થિતિ CURLE_URL_MALFORMAT:
    પાછા "URL using bad/illegal format or missing URL";

  સ્થિતિ CURLE_NOT_BUILT_IN:
    પાછા "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  સ્થિતિ CURLE_COULDNT_RESOLVE_PROXY:
    પાછા "Could not resolve proxy name";

  સ્થિતિ CURLE_COULDNT_RESOLVE_HOST:
    પાછા "Could not resolve hostname";

  સ્થિતિ CURLE_COULDNT_CONNECT:
    પાછા "Could not connect to server";

  સ્થિતિ CURLE_WEIRD_SERVER_REPLY:
    પાછા "Weird server reply";

  સ્થિતિ CURLE_REMOTE_ACCESS_DENIED:
    પાછા "Access denied to remote resource";

  સ્થિતિ CURLE_FTP_ACCEPT_FAILED:
    પાછા "FTP: The server failed to connect to data port";

  સ્થિતિ CURLE_FTP_ACCEPT_TIMEOUT:
    પાછા "FTP: Accepting server connect has timed out";

  સ્થિતિ CURLE_FTP_PRET_FAILED:
    પાછા "FTP: The server did not accept the PRET command.";

  સ્થિતિ CURLE_FTP_WEIRD_PASS_REPLY:
    પાછા "FTP: unknown PASS reply";

  સ્થિતિ CURLE_FTP_WEIRD_PASV_REPLY:
    પાછા "FTP: unknown PASV reply";

  સ્થિતિ CURLE_FTP_WEIRD_227_FORMAT:
    પાછા "FTP: unknown 227 response format";

  સ્થિતિ CURLE_FTP_CANT_GET_HOST:
    પાછા "FTP: cannot figure out the host in the PASV response";

  સ્થિતિ CURLE_HTTP2:
    પાછા "Error in the HTTP2 framing layer";

  સ્થિતિ CURLE_FTP_COULDNT_SET_TYPE:
    પાછા "FTP: could not set file type";

  સ્થિતિ CURLE_PARTIAL_FILE:
    પાછા "Transferred a partial file";

  સ્થિતિ CURLE_FTP_COULDNT_RETR_FILE:
    પાછા "FTP: could not retrieve (RETR failed) the specified file";

  સ્થિતિ CURLE_QUOTE_ERROR:
    પાછા "Quote command returned error";

  સ્થિતિ CURLE_HTTP_RETURNED_ERROR:
    પાછા "HTTP response code said error";

  સ્થિતિ CURLE_WRITE_ERROR:
    પાછા "Failed writing received data to disk/application";

  સ્થિતિ CURLE_UPLOAD_FAILED:
    પાછા "Upload failed (at start/before it took off)";

  સ્થિતિ CURLE_READ_ERROR:
    પાછા "Failed to open/read local data from file/application";

  સ્થિતિ CURLE_OUT_OF_MEMORY:
    પાછા "Out of memory";

  સ્થિતિ CURLE_OPERATION_TIMEDOUT:
    પાછા "Timeout was reached";

  સ્થિતિ CURLE_FTP_PORT_FAILED:
    પાછા "FTP: command PORT failed";

  સ્થિતિ CURLE_FTP_COULDNT_USE_REST:
    પાછા "FTP: command REST failed";

  સ્થિતિ CURLE_RANGE_ERROR:
    પાછા "Requested range was not delivered by the server";

  સ્થિતિ CURLE_SSL_CONNECT_ERROR:
    પાછા "SSL connect error";

  સ્થિતિ CURLE_BAD_DOWNLOAD_RESUME:
    પાછા "Could not resume download";

  સ્થિતિ CURLE_FILE_COULDNT_READ_FILE:
    પાછા "Could not read a file:// file";

  સ્થિતિ CURLE_LDAP_CANNOT_BIND:
    પાછા "LDAP: cannot bind";

  સ્થિતિ CURLE_LDAP_શોધ_FAILED:
    પાછા "LDAP: search failed";

  સ્થિતિ CURLE_ABORTED_BY_CALLBACK:
    પાછા "Operation was aborted by an application callback";

  સ્થિતિ CURLE_BAD_FUNCTION_ARGUMENT:
    પાછા "A libcurl function was given a bad argument";

  સ્થિતિ CURLE_INTERFACE_FAILED:
    પાછા "Failed binding local connection end";

  સ્થિતિ CURLE_TOO_MANY_REDIRECTS:
    પાછા "Number of redirects hit maximum amount";

  સ્થિતિ CURLE_UNKNOWN_OPTION:
    પાછા "An unknown option was passed in to libcurl";

  સ્થિતિ CURLE_SETOPT_OPTION_SYNTAX:
    પાછા "Malformed option provided in a setopt";

  સ્થિતિ CURLE_GOT_NOTHING:
    પાછા "Server returned nothing (no headers, no data)";

  સ્થિતિ CURLE_SSL_ENGINE_NOTFOUND:
    પાછા "SSL crypto engine not found";

  સ્થિતિ CURLE_SSL_ENGINE_SETFAILED:
    પાછા "Can not set SSL crypto engine as default";

  સ્થિતિ CURLE_SSL_ENGINE_INITFAILED:
    પાછા "Failed to initialize SSL crypto engine";

  સ્થિતિ CURLE_SEND_ERROR:
    પાછા "Failed sending data to the peer";

  સ્થિતિ CURLE_RECV_ERROR:
    પાછા "Failure when receiving data from the peer";

  સ્થિતિ CURLE_SSL_CERTPROBLEM:
    પાછા "Problem with the local SSL certificate";

  સ્થિતિ CURLE_SSL_CIPHER:
    પાછા "Could not use specified SSL cipher";

  સ્થિતિ CURLE_PEER_FAILED_VERIFICATION:
    પાછા "SSL peer certificate or SSH remote key was not OK";

  સ્થિતિ CURLE_SSL_CACERT_BADFILE:
    પાછા "Problem with the SSL CA cert (path? access rights?)";

  સ્થિતિ CURLE_BAD_CONTENT_ENCODING:
    પાછા "Unrecognized or bad HTTP Content or Transfer-Encoding";

  સ્થિતિ CURLE_FILESIZE_EXCEEDED:
    પાછા "Maximum file size exceeded";

  સ્થિતિ CURLE_USE_SSL_FAILED:
    પાછા "Requested SSL level failed";

  સ્થિતિ CURLE_SSL_SHUTDOWN_FAILED:
    પાછા "Failed to shut down the SSL connection";

  સ્થિતિ CURLE_SSL_CRL_BADFILE:
    પાછા "Failed to load CRL file (path? access rights?, format?)";

  સ્થિતિ CURLE_SSL_ISSUER_ERROR:
    પાછા "Issuer check against peer certificate failed";

  સ્થિતિ CURLE_SEND_FAIL_REWIND:
    પાછા "Send failed since rewinding of the data stream failed";

  સ્થિતિ CURLE_LOGIN_DENIED:
    પાછા "Login denied";

  સ્થિતિ CURLE_TFTP_NOTFOUND:
    પાછા "TFTP: File Not Found";

  સ્થિતિ CURLE_TFTP_PERM:
    પાછા "TFTP: Access Violation";

  સ્થિતિ CURLE_REMOTE_DISK_FULL:
    પાછા "Disk full or allocation exceeded";

  સ્થિતિ CURLE_TFTP_ILLEGAL:
    પાછા "TFTP: Illegal operation";

  સ્થિતિ CURLE_TFTP_UNKNOWNID:
    પાછા "TFTP: Unknown transfer ID";

  સ્થિતિ CURLE_REMOTE_FILE_EXISTS:
    પાછા "Remote file already exists";

  સ્થિતિ CURLE_TFTP_NOSUCHUSER:
    પાછા "TFTP: No such user";

  સ્થિતિ CURLE_REMOTE_FILE_NOT_FOUND:
    પાછા "Remote file not found";

  સ્થિતિ CURLE_SSH:
    પાછા "Error in the SSH layer";

  સ્થિતિ CURLE_AGAIN:
    પાછા "Socket not ready for send/recv";

  સ્થિતિ CURLE_RTSP_CSEQ_ERROR:
    પાછા "RTSP CSeq mismatch or invalid CSeq";

  સ્થિતિ CURLE_RTSP_SESSION_ERROR:
    પાછા "RTSP session error";

  સ્થિતિ CURLE_FTP_BAD_FILE_LIST:
    પાછા "Unable to parse FTP file list";

  સ્થિતિ CURLE_CHUNK_FAILED:
    પાછા "Chunk callback failed";

  સ્થિતિ CURLE_NO_CONNECTION_AVAILABLE:
    પાછા "The max connection limit is reached";

  સ્થિતિ CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    પાછા "SSL public key does not match pinned public key";

  સ્થિતિ CURLE_SSL_INVALIDCERTSTATUS:
    પાછા "SSL server certificate status verification FAILED";

  સ્થિતિ CURLE_HTTP2_STREAM:
    પાછા "Stream error in the HTTP/2 framing layer";

  સ્થિતિ CURLE_RECURSIVE_API_CALL:
    પાછા "API function called from within callback";

  સ્થિતિ CURLE_AUTH_ERROR:
    પાછા "An authentication function returned an error";

  સ્થિતિ CURLE_HTTP3:
    પાછા "HTTP/3 error";

  સ્થિતિ CURLE_QUIC_CONNECT_ERROR:
    પાછા "QUIC connection error";

  સ્થિતિ CURLE_PROXY:
    પાછા "proxy handshake error";

  સ્થિતિ CURLE_SSL_CLIENTCERT:
    પાછા "SSL Client Certificate required";

  સ્થિતિ CURLE_UNRECOVERABLE_POLL:
    પાછા "Unrecoverable error in select/poll";

  સ્થિતિ CURLE_TOO_LARGE:
    પાછા "A value or data field grew larger than allowed";

  સ્થિતિ CURLE_ECH_REQUIRED:
    પાછા "ECH attempted but failed";

    /* error codes not used by current libcurl */
  મૂળભૂત:
    તોડો;
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
  પાછા "Unknown error";
#નહીંતર
  જો(!error)
    પાછા "No error";
  નહીંતર
    પાછા "Error";
#endif
}

અચલ અક્ષર *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  પસંદગી(error) {
  સ્થિતિ CURLM_CALL_MULTI_PERFORM:
    પાછા "Please call curl_multi_perform() soon";

  સ્થિતિ CURLM_OK:
    પાછા "No error";

  સ્થિતિ CURLM_BAD_HANDLE:
    પાછા "Invalid multi handle";

  સ્થિતિ CURLM_BAD_EASY_HANDLE:
    પાછા "Invalid easy handle";

  સ્થિતિ CURLM_OUT_OF_MEMORY:
    પાછા "Out of memory";

  સ્થિતિ CURLM_INTERNAL_ERROR:
    પાછા "Internal error";

  સ્થિતિ CURLM_BAD_SOCKET:
    પાછા "Invalid socket argument";

  સ્થિતિ CURLM_UNKNOWN_OPTION:
    પાછા "Unknown option";

  સ્થિતિ CURLM_ADDED_ALREADY:
    પાછા "The easy handle is already added to a multi handle";

  સ્થિતિ CURLM_RECURSIVE_API_CALL:
    પાછા "API function called from within callback";

  સ્થિતિ CURLM_WAKEUP_FAILURE:
    પાછા "Wakeup is unavailable or failed";

  સ્થિતિ CURLM_BAD_FUNCTION_ARGUMENT:
    પાછા "A libcurl function was given a bad argument";

  સ્થિતિ CURLM_ABORTED_BY_CALLBACK:
    પાછા "Operation was aborted by an application callback";

  સ્થિતિ CURLM_UNRECOVERABLE_POLL:
    પાછા "Unrecoverable error in select/poll";

  સ્થિતિ CURLM_LAST:
    તોડો;
  }

  પાછા "Unknown error";
#નહીંતર
  જો(error == CURLM_OK)
    પાછા "No error";
  નહીંતર
    પાછા "Error";
#endif
}

અચલ અક્ષર *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  પસંદગી(error) {
  સ્થિતિ CURLSHE_OK:
    પાછા "No error";

  સ્થિતિ CURLSHE_BAD_OPTION:
    પાછા "Unknown share option";

  સ્થિતિ CURLSHE_IN_USE:
    પાછા "Share currently in use";

  સ્થિતિ CURLSHE_INVALID:
    પાછા "Invalid share handle";

  સ્થિતિ CURLSHE_NOMEM:
    પાછા "Out of memory";

  સ્થિતિ CURLSHE_NOT_BUILT_IN:
    પાછા "Feature not enabled in this library";

  સ્થિતિ CURLSHE_LAST:
    તોડો;
  }

  પાછા "CURLSHcode unknown";
#નહીંતર
  જો(error == CURLSHE_OK)
    પાછા "No error";
  નહીંતર
    પાછા "Error";
#endif
}

અચલ અક્ષર *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  પસંદગી(error) {
  સ્થિતિ CURLUE_OK:
    પાછા "No error";

  સ્થિતિ CURLUE_BAD_HANDLE:
    પાછા "An invalid CURLU pointer was passed as argument";

  સ્થિતિ CURLUE_BAD_PARTPOINTER:
    પાછા "An invalid 'part' argument was passed as argument";

  સ્થિતિ CURLUE_MALFORMED_આગત:
    પાછા "Malformed input to a URL function";

  સ્થિતિ CURLUE_BAD_PORT_સંખ્યા:
    પાછા "Port number was not a decimal number between 0 and 65535";

  સ્થિતિ CURLUE_UNSUPPORTED_SCHEME:
    પાછા "Unsupported URL scheme";

  સ્થિતિ CURLUE_URLDECODE:
    પાછા "URL decode error, most likely because of rubbish in the input";

  સ્થિતિ CURLUE_OUT_OF_MEMORY:
    પાછા "A memory function failed";

  સ્થિતિ CURLUE_USER_NOT_ALLOWED:
    પાછા "Credentials was passed in the URL when prohibited";

  સ્થિતિ CURLUE_UNKNOWN_PART:
    પાછા "An unknown part ID was passed to a URL API function";

  સ્થિતિ CURLUE_NO_SCHEME:
    પાછા "No scheme present";

  સ્થિતિ CURLUE_NO_USER:
    પાછા "No user present";

  સ્થિતિ CURLUE_NO_PASSWORD:
    પાછા "No password present";

  સ્થિતિ CURLUE_NO_OPTIONS:
    પાછા "No options present";

  સ્થિતિ CURLUE_NO_HOST:
    પાછા "No host present";

  સ્થિતિ CURLUE_NO_PORT:
    પાછા "No port number present";

  સ્થિતિ CURLUE_NO_QUERY:
    પાછા "No query present";

  સ્થિતિ CURLUE_NO_FRAGMENT:
    પાછા "No fragment present";

  સ્થિતિ CURLUE_NO_ZONEID:
    પાછા "No zoneid present";

  સ્થિતિ CURLUE_BAD_LOGIN:
    પાછા "Bad login";

  સ્થિતિ CURLUE_BAD_IPV6:
    પાછા "Bad IPv6 address";

  સ્થિતિ CURLUE_BAD_HOSTNAME:
    પાછા "Bad hostname";

  સ્થિતિ CURLUE_BAD_FILE_URL:
    પાછા "Bad file:// URL";

  સ્થિતિ CURLUE_BAD_SLASHES:
    પાછા "Unsupported number of slashes following scheme";

  સ્થિતિ CURLUE_BAD_SCHEME:
    પાછા "Bad scheme";

  સ્થિતિ CURLUE_BAD_PATH:
    પાછા "Bad path";

  સ્થિતિ CURLUE_BAD_FRAGMENT:
    પાછા "Bad fragment";

  સ્થિતિ CURLUE_BAD_QUERY:
    પાછા "Bad query";

  સ્થિતિ CURLUE_BAD_PASSWORD:
    પાછા "Bad password";

  સ્થિતિ CURLUE_BAD_USER:
    પાછા "Bad user";

  સ્થિતિ CURLUE_LACKS_IDN:
    પાછા "libcurl lacks IDN support";

  સ્થિતિ CURLUE_TOO_LARGE:
    પાછા "A value or data field is larger than allowed";

  સ્થિતિ CURLUE_BACKSLASH:
    પાછા "Found a backslash where a forward slash was expected";

  સ્થિતિ CURLUE_LAST:
    તોડો;
  }

  પાછા "CURLUcode unknown";
#નહીંતર
  જો(error == CURLUE_OK)
    પાછા "No error";
  નહીંતર
    પાછા "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
અચલ અક્ષર *Curl_sspi_strerror(SECURITY_STATUS err, અક્ષર *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  પૂર્ણાંક old_errno = errno;
  VERBOSE(અચલ અક્ષર *txt);

  જો(!buflen)
    પાછા ખાલી;

  *buf = '\0';

#ifdef CURLVERBOSE
  પસંદગી(err) {
  સ્થિતિ SEC_E_OK:
    txt = "No error";
    તોડો;
#define SEC2TXT(sec) સ્થિતિ sec: txt = #sec; તોડો
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
  SEC2TXT(SEC_E_NO_KERB_ચાવી);
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
  SEC2TXT(SEC_E_UNTRUSTED_મૂળ);
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
  મૂળભૂત:
    txt = "Unknown error";
  }

  જો(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  નહીંતર {
    અક્ષર msgbuf[256];
    જો(curlx_get_winapi_error((DWORD)err, msgbuf, કદ(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    નહીંતર
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#નહીંતર /* CURLVERBOSE */
  જો(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  નહીંતર
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  જો(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  જો(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  પાછા buf;
}
#endif /* USE_WINDOWS_SSPI */
