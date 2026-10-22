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

#ifdef USE_WINDOWS_SSPI
#अन्तर्भाव "curl_sspi.h"
#endif

#अन्तर्भाव "curlx/winapi.h"
#अन्तर्भाव "strerror.h"
#अन्तर्भाव "curlx/strcopy.h"

ध्रुव वर्ण *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLE_OK:
    पटिनिवत्त "No error";

  पक्ष CURLE_UNSUPPORTED_PROTOCOL:
    पटिनिवत्त "Unsupported protocol";

  पक्ष CURLE_FAILED_INIT:
    पटिनिवत्त "Failed initialization";

  पक्ष CURLE_URL_MALFORMAT:
    पटिनिवत्त "URL using bad/illegal format or missing URL";

  पक्ष CURLE_NOT_BUILT_IN:
    पटिनिवत्त "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  पक्ष CURLE_COULDNT_RESOLVE_PROXY:
    पटिनिवत्त "Could not resolve proxy name";

  पक्ष CURLE_COULDNT_RESOLVE_HOST:
    पटिनिवत्त "Could not resolve hostname";

  पक्ष CURLE_COULDNT_CONNECT:
    पटिनिवत्त "Could not connect to server";

  पक्ष CURLE_WEIRD_SERVER_REPLY:
    पटिनिवत्त "Weird server reply";

  पक्ष CURLE_REMOTE_ACCESS_DENIED:
    पटिनिवत्त "Access denied to remote resource";

  पक्ष CURLE_FTP_ACCEPT_FAILED:
    पटिनिवत्त "FTP: The server failed to connect to data port";

  पक्ष CURLE_FTP_ACCEPT_TIMEOUT:
    पटिनिवत्त "FTP: Accepting server connect has timed out";

  पक्ष CURLE_FTP_PRET_FAILED:
    पटिनिवत्त "FTP: The server did not accept the PRET command.";

  पक्ष CURLE_FTP_WEIRD_PASS_REPLY:
    पटिनिवत्त "FTP: unknown PASS reply";

  पक्ष CURLE_FTP_WEIRD_PASV_REPLY:
    पटिनिवत्त "FTP: unknown PASV reply";

  पक्ष CURLE_FTP_WEIRD_227_FORMAT:
    पटिनिवत्त "FTP: unknown 227 response format";

  पक्ष CURLE_FTP_CANT_GET_HOST:
    पटिनिवत्त "FTP: cannot figure out the host in the PASV response";

  पक्ष CURLE_HTTP2:
    पटिनिवत्त "Error in the HTTP2 framing layer";

  पक्ष CURLE_FTP_COULDNT_SET_TYPE:
    पटिनिवत्त "FTP: could not set file type";

  पक्ष CURLE_PARTIAL_FILE:
    पटिनिवत्त "Transferred a partial file";

  पक्ष CURLE_FTP_COULDNT_RETR_FILE:
    पटिनिवत्त "FTP: could not retrieve (RETR failed) the specified file";

  पक्ष CURLE_QUOTE_ERROR:
    पटिनिवत्त "Quote command returned error";

  पक्ष CURLE_HTTP_RETURNED_ERROR:
    पटिनिवत्त "HTTP response code said error";

  पक्ष CURLE_WRITE_ERROR:
    पटिनिवत्त "Failed writing received data to disk/application";

  पक्ष CURLE_UPLOAD_FAILED:
    पटिनिवत्त "Upload failed (at start/before it took off)";

  पक्ष CURLE_READ_ERROR:
    पटिनिवत्त "Failed to open/read local data from file/application";

  पक्ष CURLE_OUT_OF_MEMORY:
    पटिनिवत्त "Out of memory";

  पक्ष CURLE_OPERATION_TIMEDOUT:
    पटिनिवत्त "Timeout was reached";

  पक्ष CURLE_FTP_PORT_FAILED:
    पटिनिवत्त "FTP: command PORT failed";

  पक्ष CURLE_FTP_COULDNT_USE_REST:
    पटिनिवत्त "FTP: command REST failed";

  पक्ष CURLE_RANGE_ERROR:
    पटिनिवत्त "Requested range was not delivered by the server";

  पक्ष CURLE_SSL_CONNECT_ERROR:
    पटिनिवत्त "SSL connect error";

  पक्ष CURLE_BAD_DOWNLOAD_RESUME:
    पटिनिवत्त "Could not resume download";

  पक्ष CURLE_FILE_COULDNT_READ_FILE:
    पटिनिवत्त "Could not read a file:// file";

  पक्ष CURLE_LDAP_CANNOT_BIND:
    पटिनिवत्त "LDAP: cannot bind";

  पक्ष CURLE_LDAP_अन्वेषणम्_FAILED:
    पटिनिवत्त "LDAP: search failed";

  पक्ष CURLE_ABORTED_BY_CALLBACK:
    पटिनिवत्त "Operation was aborted by an application callback";

  पक्ष CURLE_BAD_FUNCTION_ARGUMENT:
    पटिनिवत्त "A libcurl function was given a bad argument";

  पक्ष CURLE_INTERFACE_FAILED:
    पटिनिवत्त "Failed binding local connection end";

  पक्ष CURLE_TOO_MANY_REDIRECTS:
    पटिनिवत्त "Number of redirects hit maximum amount";

  पक्ष CURLE_UNKNOWN_OPTION:
    पटिनिवत्त "An unknown option was passed in to libcurl";

  पक्ष CURLE_SETOPT_OPTION_SYNTAX:
    पटिनिवत्त "Malformed option provided in a setopt";

  पक्ष CURLE_GOT_NOTHING:
    पटिनिवत्त "Server returned nothing (no headers, no data)";

  पक्ष CURLE_SSL_ENGINE_NOTFOUND:
    पटिनिवत्त "SSL crypto engine not found";

  पक्ष CURLE_SSL_ENGINE_SETFAILED:
    पटिनिवत्त "Can not set SSL crypto engine as default";

  पक्ष CURLE_SSL_ENGINE_INITFAILED:
    पटिनिवत्त "Failed to initialize SSL crypto engine";

  पक्ष CURLE_SEND_ERROR:
    पटिनिवत्त "Failed sending data to the peer";

  पक्ष CURLE_RECV_ERROR:
    पटिनिवत्त "Failure when receiving data from the peer";

  पक्ष CURLE_SSL_CERTPROBLEM:
    पटिनिवत्त "Problem with the local SSL certificate";

  पक्ष CURLE_SSL_CIPHER:
    पटिनिवत्त "Could not use specified SSL cipher";

  पक्ष CURLE_PEER_FAILED_VERIFICATION:
    पटिनिवत्त "SSL peer certificate or SSH remote key was not OK";

  पक्ष CURLE_SSL_CACERT_BADFILE:
    पटिनिवत्त "Problem with the SSL CA cert (path? access rights?)";

  पक्ष CURLE_BAD_CONTENT_ENCODING:
    पटिनिवत्त "Unrecognized or bad HTTP Content or Transfer-Encoding";

  पक्ष CURLE_FILESIZE_EXCEEDED:
    पटिनिवत्त "Maximum file size exceeded";

  पक्ष CURLE_USE_SSL_FAILED:
    पटिनिवत्त "Requested SSL level failed";

  पक्ष CURLE_SSL_SHUTDOWN_FAILED:
    पटिनिवत्त "Failed to shut down the SSL connection";

  पक्ष CURLE_SSL_CRL_BADFILE:
    पटिनिवत्त "Failed to load CRL file (path? access rights?, format?)";

  पक्ष CURLE_SSL_ISSUER_ERROR:
    पटिनिवत्त "Issuer check against peer certificate failed";

  पक्ष CURLE_SEND_FAIL_REWIND:
    पटिनिवत्त "Send failed since rewinding of the data stream failed";

  पक्ष CURLE_LOGIN_DENIED:
    पटिनिवत्त "Login denied";

  पक्ष CURLE_TFTP_NOTFOUND:
    पटिनिवत्त "TFTP: File Not Found";

  पक्ष CURLE_TFTP_PERM:
    पटिनिवत्त "TFTP: Access Violation";

  पक्ष CURLE_REMOTE_DISK_FULL:
    पटिनिवत्त "Disk full or allocation exceeded";

  पक्ष CURLE_TFTP_ILLEGAL:
    पटिनिवत्त "TFTP: Illegal operation";

  पक्ष CURLE_TFTP_UNKNOWNID:
    पटिनिवत्त "TFTP: Unknown transfer ID";

  पक्ष CURLE_REMOTE_FILE_EXISTS:
    पटिनिवत्त "Remote file already exists";

  पक्ष CURLE_TFTP_NOSUCHUSER:
    पटिनिवत्त "TFTP: No such user";

  पक्ष CURLE_REMOTE_FILE_NOT_FOUND:
    पटिनिवत्त "Remote file not found";

  पक्ष CURLE_SSH:
    पटिनिवत्त "Error in the SSH layer";

  पक्ष CURLE_AGAIN:
    पटिनिवत्त "Socket not ready for send/recv";

  पक्ष CURLE_RTSP_CSEQ_ERROR:
    पटिनिवत्त "RTSP CSeq mismatch or invalid CSeq";

  पक्ष CURLE_RTSP_SESSION_ERROR:
    पटिनिवत्त "RTSP session error";

  पक्ष CURLE_FTP_BAD_FILE_LIST:
    पटिनिवत्त "Unable to parse FTP file list";

  पक्ष CURLE_CHUNK_FAILED:
    पटिनिवत्त "Chunk callback failed";

  पक्ष CURLE_NO_CONNECTION_AVAILABLE:
    पटिनिवत्त "The max connection limit is reached";

  पक्ष CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    पटिनिवत्त "SSL public key does not match pinned public key";

  पक्ष CURLE_SSL_INVALIDCERTSTATUS:
    पटिनिवत्त "SSL server certificate status verification FAILED";

  पक्ष CURLE_HTTP2_STREAM:
    पटिनिवत्त "Stream error in the HTTP/2 framing layer";

  पक्ष CURLE_RECURSIVE_API_CALL:
    पटिनिवत्त "API function called from within callback";

  पक्ष CURLE_AUTH_ERROR:
    पटिनिवत्त "An authentication function returned an error";

  पक्ष CURLE_HTTP3:
    पटिनिवत्त "HTTP/3 error";

  पक्ष CURLE_QUIC_CONNECT_ERROR:
    पटिनिवत्त "QUIC connection error";

  पक्ष CURLE_PROXY:
    पटिनिवत्त "proxy handshake error";

  पक्ष CURLE_SSL_CLIENTCERT:
    पटिनिवत्त "SSL Client Certificate required";

  पक्ष CURLE_UNRECOVERABLE_POLL:
    पटिनिवत्त "Unrecoverable error in select/poll";

  पक्ष CURLE_TOO_LARGE:
    पटिनिवत्त "A value or data field grew larger than allowed";

  पक्ष CURLE_ECH_REQUIRED:
    पटिनिवत्त "ECH attempted but failed";

    /* error codes not used by current libcurl */
  स्वभाव:
    भङ्ग;
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
  पटिनिवत्त "Unknown error";
#अञ्ञथा
  सचे(!error)
    पटिनिवत्त "No error";
  अञ्ञथा
    पटिनिवत्त "Error";
#endif
}

ध्रुव वर्ण *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLM_CALL_MULTI_PERFORM:
    पटिनिवत्त "Please call curl_multi_perform() soon";

  पक्ष CURLM_OK:
    पटिनिवत्त "No error";

  पक्ष CURLM_BAD_HANDLE:
    पटिनिवत्त "Invalid multi handle";

  पक्ष CURLM_BAD_EASY_HANDLE:
    पटिनिवत्त "Invalid easy handle";

  पक्ष CURLM_OUT_OF_MEMORY:
    पटिनिवत्त "Out of memory";

  पक्ष CURLM_INTERNAL_ERROR:
    पटिनिवत्त "Internal error";

  पक्ष CURLM_BAD_SOCKET:
    पटिनिवत्त "Invalid socket argument";

  पक्ष CURLM_UNKNOWN_OPTION:
    पटिनिवत्त "Unknown option";

  पक्ष CURLM_ADDED_ALREADY:
    पटिनिवत्त "The easy handle is already added to a multi handle";

  पक्ष CURLM_RECURSIVE_API_CALL:
    पटिनिवत्त "API function called from within callback";

  पक्ष CURLM_WAKEUP_FAILURE:
    पटिनिवत्त "Wakeup is unavailable or failed";

  पक्ष CURLM_BAD_FUNCTION_ARGUMENT:
    पटिनिवत्त "A libcurl function was given a bad argument";

  पक्ष CURLM_ABORTED_BY_CALLBACK:
    पटिनिवत्त "Operation was aborted by an application callback";

  पक्ष CURLM_UNRECOVERABLE_POLL:
    पटिनिवत्त "Unrecoverable error in select/poll";

  पक्ष CURLM_LAST:
    भङ्ग;
  }

  पटिनिवत्त "Unknown error";
#अञ्ञथा
  सचे(error == CURLM_OK)
    पटिनिवत्त "No error";
  अञ्ञथा
    पटिनिवत्त "Error";
#endif
}

ध्रुव वर्ण *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLSHE_OK:
    पटिनिवत्त "No error";

  पक्ष CURLSHE_BAD_OPTION:
    पटिनिवत्त "Unknown share option";

  पक्ष CURLSHE_IN_USE:
    पटिनिवत्त "Share currently in use";

  पक्ष CURLSHE_INVALID:
    पटिनिवत्त "Invalid share handle";

  पक्ष CURLSHE_NOMEM:
    पटिनिवत्त "Out of memory";

  पक्ष CURLSHE_NOT_BUILT_IN:
    पटिनिवत्त "Feature not enabled in this library";

  पक्ष CURLSHE_LAST:
    भङ्ग;
  }

  पटिनिवत्त "CURLSHcode unknown";
#अञ्ञथा
  सचे(error == CURLSHE_OK)
    पटिनिवत्त "No error";
  अञ्ञथा
    पटिनिवत्त "Error";
#endif
}

ध्रुव वर्ण *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLUE_OK:
    पटिनिवत्त "No error";

  पक्ष CURLUE_BAD_HANDLE:
    पटिनिवत्त "An invalid CURLU pointer was passed as argument";

  पक्ष CURLUE_BAD_PARTPOINTER:
    पटिनिवत्त "An invalid 'part' argument was passed as argument";

  पक्ष CURLUE_MALFORMED_निवेशः:
    पटिनिवत्त "Malformed input to a URL function";

  पक्ष CURLUE_BAD_PORT_सङ्ख्या:
    पटिनिवत्त "Port number was not a decimal number between 0 and 65535";

  पक्ष CURLUE_UNSUPPORTED_SCHEME:
    पटिनिवत्त "Unsupported URL scheme";

  पक्ष CURLUE_URLDECODE:
    पटिनिवत्त "URL decode error, most likely because of rubbish in the input";

  पक्ष CURLUE_OUT_OF_MEMORY:
    पटिनिवत्त "A memory function failed";

  पक्ष CURLUE_USER_NOT_ALLOWED:
    पटिनिवत्त "Credentials was passed in the URL when prohibited";

  पक्ष CURLUE_UNKNOWN_PART:
    पटिनिवत्त "An unknown part ID was passed to a URL API function";

  पक्ष CURLUE_NO_SCHEME:
    पटिनिवत्त "No scheme present";

  पक्ष CURLUE_NO_USER:
    पटिनिवत्त "No user present";

  पक्ष CURLUE_NO_PASSWORD:
    पटिनिवत्त "No password present";

  पक्ष CURLUE_NO_OPTIONS:
    पटिनिवत्त "No options present";

  पक्ष CURLUE_NO_HOST:
    पटिनिवत्त "No host present";

  पक्ष CURLUE_NO_PORT:
    पटिनिवत्त "No port number present";

  पक्ष CURLUE_NO_QUERY:
    पटिनिवत्त "No query present";

  पक्ष CURLUE_NO_FRAGMENT:
    पटिनिवत्त "No fragment present";

  पक्ष CURLUE_NO_ZONEID:
    पटिनिवत्त "No zoneid present";

  पक्ष CURLUE_BAD_LOGIN:
    पटिनिवत्त "Bad login";

  पक्ष CURLUE_BAD_IPV6:
    पटिनिवत्त "Bad IPv6 address";

  पक्ष CURLUE_BAD_HOSTNAME:
    पटिनिवत्त "Bad hostname";

  पक्ष CURLUE_BAD_FILE_URL:
    पटिनिवत्त "Bad file:// URL";

  पक्ष CURLUE_BAD_SLASHES:
    पटिनिवत्त "Unsupported number of slashes following scheme";

  पक्ष CURLUE_BAD_SCHEME:
    पटिनिवत्त "Bad scheme";

  पक्ष CURLUE_BAD_PATH:
    पटिनिवत्त "Bad path";

  पक्ष CURLUE_BAD_FRAGMENT:
    पटिनिवत्त "Bad fragment";

  पक्ष CURLUE_BAD_QUERY:
    पटिनिवत्त "Bad query";

  पक्ष CURLUE_BAD_PASSWORD:
    पटिनिवत्त "Bad password";

  पक्ष CURLUE_BAD_USER:
    पटिनिवत्त "Bad user";

  पक्ष CURLUE_LACKS_IDN:
    पटिनिवत्त "libcurl lacks IDN support";

  पक्ष CURLUE_TOO_LARGE:
    पटिनिवत्त "A value or data field is larger than allowed";

  पक्ष CURLUE_BACKSLASH:
    पटिनिवत्त "Found a backslash where a forward slash was expected";

  पक्ष CURLUE_LAST:
    भङ्ग;
  }

  पटिनिवत्त "CURLUcode unknown";
#अञ्ञथा
  सचे(error == CURLUE_OK)
    पटिनिवत्त "No error";
  अञ्ञथा
    पटिनिवत्त "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ध्रुव वर्ण *Curl_sspi_strerror(SECURITY_STATUS err, वर्ण *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  पूर्णाङ्क old_errno = errno;
  VERBOSE(ध्रुव वर्ण *txt);

  सचे(!buflen)
    पटिनिवत्त रिक्त;

  *buf = '\0';

#ifdef CURLVERBOSE
  विकल्प(err) {
  पक्ष SEC_E_OK:
    txt = "No error";
    भङ्ग;
#define SEC2TXT(sec) पक्ष sec: txt = #sec; भङ्ग
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
  SEC2TXT(SEC_E_NO_KERB_कुञ्चिका);
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
  SEC2TXT(SEC_E_UNTRUSTED_मूलम्);
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
  स्वभाव:
    txt = "Unknown error";
  }

  सचे(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  अञ्ञथा {
    वर्ण msgbuf[256];
    सचे(curlx_get_winapi_error((DWORD)err, msgbuf, परिमाण(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    अञ्ञथा
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#अञ्ञथा /* CURLVERBOSE */
  सचे(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  अञ्ञथा
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  सचे(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  सचे(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  पटिनिवत्त buf;
}
#endif /* USE_WINDOWS_SSPI */
