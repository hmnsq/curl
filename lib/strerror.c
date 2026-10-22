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
    प्रत्यागम "No error";

  पक्ष CURLE_UNSUPPORTED_PROTOCOL:
    प्रत्यागम "Unsupported protocol";

  पक्ष CURLE_FAILED_INIT:
    प्रत्यागम "Failed initialization";

  पक्ष CURLE_URL_MALFORMAT:
    प्रत्यागम "URL using bad/illegal format or missing URL";

  पक्ष CURLE_NOT_BUILT_IN:
    प्रत्यागम "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  पक्ष CURLE_COULDNT_RESOLVE_PROXY:
    प्रत्यागम "Could not resolve proxy name";

  पक्ष CURLE_COULDNT_RESOLVE_HOST:
    प्रत्यागम "Could not resolve hostname";

  पक्ष CURLE_COULDNT_CONNECT:
    प्रत्यागम "Could not connect to server";

  पक्ष CURLE_WEIRD_SERVER_REPLY:
    प्रत्यागम "Weird server reply";

  पक्ष CURLE_REMOTE_ACCESS_DENIED:
    प्रत्यागम "Access denied to remote resource";

  पक्ष CURLE_FTP_ACCEPT_FAILED:
    प्रत्यागम "FTP: The server failed to connect to data port";

  पक्ष CURLE_FTP_ACCEPT_TIMEOUT:
    प्रत्यागम "FTP: Accepting server connect has timed out";

  पक्ष CURLE_FTP_PRET_FAILED:
    प्रत्यागम "FTP: The server did not accept the PRET command.";

  पक्ष CURLE_FTP_WEIRD_PASS_REPLY:
    प्रत्यागम "FTP: unknown PASS reply";

  पक्ष CURLE_FTP_WEIRD_PASV_REPLY:
    प्रत्यागम "FTP: unknown PASV reply";

  पक्ष CURLE_FTP_WEIRD_227_FORMAT:
    प्रत्यागम "FTP: unknown 227 response format";

  पक्ष CURLE_FTP_CANT_GET_HOST:
    प्रत्यागम "FTP: cannot figure out the host in the PASV response";

  पक्ष CURLE_HTTP2:
    प्रत्यागम "Error in the HTTP2 framing layer";

  पक्ष CURLE_FTP_COULDNT_SET_TYPE:
    प्रत्यागम "FTP: could not set file type";

  पक्ष CURLE_PARTIAL_FILE:
    प्रत्यागम "Transferred a partial file";

  पक्ष CURLE_FTP_COULDNT_RETR_FILE:
    प्रत्यागम "FTP: could not retrieve (RETR failed) the specified file";

  पक्ष CURLE_QUOTE_ERROR:
    प्रत्यागम "Quote command returned error";

  पक्ष CURLE_HTTP_RETURNED_ERROR:
    प्रत्यागम "HTTP response code said error";

  पक्ष CURLE_WRITE_ERROR:
    प्रत्यागम "Failed writing received data to disk/application";

  पक्ष CURLE_UPLOAD_FAILED:
    प्रत्यागम "Upload failed (at start/before it took off)";

  पक्ष CURLE_READ_ERROR:
    प्रत्यागम "Failed to open/read local data from file/application";

  पक्ष CURLE_OUT_OF_MEMORY:
    प्रत्यागम "Out of memory";

  पक्ष CURLE_OPERATION_TIMEDOUT:
    प्रत्यागम "Timeout was reached";

  पक्ष CURLE_FTP_PORT_FAILED:
    प्रत्यागम "FTP: command PORT failed";

  पक्ष CURLE_FTP_COULDNT_USE_REST:
    प्रत्यागम "FTP: command REST failed";

  पक्ष CURLE_RANGE_ERROR:
    प्रत्यागम "Requested range was not delivered by the server";

  पक्ष CURLE_SSL_CONNECT_ERROR:
    प्रत्यागम "SSL connect error";

  पक्ष CURLE_BAD_DOWNLOAD_RESUME:
    प्रत्यागम "Could not resume download";

  पक्ष CURLE_FILE_COULDNT_READ_FILE:
    प्रत्यागम "Could not read a file:// file";

  पक्ष CURLE_LDAP_CANNOT_BIND:
    प्रत्यागम "LDAP: cannot bind";

  पक्ष CURLE_LDAP_अन्वेषणम्_FAILED:
    प्रत्यागम "LDAP: search failed";

  पक्ष CURLE_ABORTED_BY_CALLBACK:
    प्रत्यागम "Operation was aborted by an application callback";

  पक्ष CURLE_BAD_FUNCTION_ARGUMENT:
    प्रत्यागम "A libcurl function was given a bad argument";

  पक्ष CURLE_INTERFACE_FAILED:
    प्रत्यागम "Failed binding local connection end";

  पक्ष CURLE_TOO_MANY_REDIRECTS:
    प्रत्यागम "Number of redirects hit maximum amount";

  पक्ष CURLE_UNKNOWN_OPTION:
    प्रत्यागम "An unknown option was passed in to libcurl";

  पक्ष CURLE_SETOPT_OPTION_SYNTAX:
    प्रत्यागम "Malformed option provided in a setopt";

  पक्ष CURLE_GOT_NOTHING:
    प्रत्यागम "Server returned nothing (no headers, no data)";

  पक्ष CURLE_SSL_ENGINE_NOTFOUND:
    प्रत्यागम "SSL crypto engine not found";

  पक्ष CURLE_SSL_ENGINE_SETFAILED:
    प्रत्यागम "Can not set SSL crypto engine as default";

  पक्ष CURLE_SSL_ENGINE_INITFAILED:
    प्रत्यागम "Failed to initialize SSL crypto engine";

  पक्ष CURLE_SEND_ERROR:
    प्रत्यागम "Failed sending data to the peer";

  पक्ष CURLE_RECV_ERROR:
    प्रत्यागम "Failure when receiving data from the peer";

  पक्ष CURLE_SSL_CERTPROBLEM:
    प्रत्यागम "Problem with the local SSL certificate";

  पक्ष CURLE_SSL_CIPHER:
    प्रत्यागम "Could not use specified SSL cipher";

  पक्ष CURLE_PEER_FAILED_VERIFICATION:
    प्रत्यागम "SSL peer certificate or SSH remote key was not OK";

  पक्ष CURLE_SSL_CACERT_BADFILE:
    प्रत्यागम "Problem with the SSL CA cert (path? access rights?)";

  पक्ष CURLE_BAD_CONTENT_ENCODING:
    प्रत्यागम "Unrecognized or bad HTTP Content or Transfer-Encoding";

  पक्ष CURLE_FILESIZE_EXCEEDED:
    प्रत्यागम "Maximum file size exceeded";

  पक्ष CURLE_USE_SSL_FAILED:
    प्रत्यागम "Requested SSL level failed";

  पक्ष CURLE_SSL_SHUTDOWN_FAILED:
    प्रत्यागम "Failed to shut down the SSL connection";

  पक्ष CURLE_SSL_CRL_BADFILE:
    प्रत्यागम "Failed to load CRL file (path? access rights?, format?)";

  पक्ष CURLE_SSL_ISSUER_ERROR:
    प्रत्यागम "Issuer check against peer certificate failed";

  पक्ष CURLE_SEND_FAIL_REWIND:
    प्रत्यागम "Send failed since rewinding of the data stream failed";

  पक्ष CURLE_LOGIN_DENIED:
    प्रत्यागम "Login denied";

  पक्ष CURLE_TFTP_NOTFOUND:
    प्रत्यागम "TFTP: File Not Found";

  पक्ष CURLE_TFTP_PERM:
    प्रत्यागम "TFTP: Access Violation";

  पक्ष CURLE_REMOTE_DISK_FULL:
    प्रत्यागम "Disk full or allocation exceeded";

  पक्ष CURLE_TFTP_ILLEGAL:
    प्रत्यागम "TFTP: Illegal operation";

  पक्ष CURLE_TFTP_UNKNOWNID:
    प्रत्यागम "TFTP: Unknown transfer ID";

  पक्ष CURLE_REMOTE_FILE_EXISTS:
    प्रत्यागम "Remote file already exists";

  पक्ष CURLE_TFTP_NOSUCHUSER:
    प्रत्यागम "TFTP: No such user";

  पक्ष CURLE_REMOTE_FILE_NOT_FOUND:
    प्रत्यागम "Remote file not found";

  पक्ष CURLE_SSH:
    प्रत्यागम "Error in the SSH layer";

  पक्ष CURLE_AGAIN:
    प्रत्यागम "Socket not ready for send/recv";

  पक्ष CURLE_RTSP_CSEQ_ERROR:
    प्रत्यागम "RTSP CSeq mismatch or invalid CSeq";

  पक्ष CURLE_RTSP_SESSION_ERROR:
    प्रत्यागम "RTSP session error";

  पक्ष CURLE_FTP_BAD_FILE_LIST:
    प्रत्यागम "Unable to parse FTP file list";

  पक्ष CURLE_CHUNK_FAILED:
    प्रत्यागम "Chunk callback failed";

  पक्ष CURLE_NO_CONNECTION_AVAILABLE:
    प्रत्यागम "The max connection limit is reached";

  पक्ष CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    प्रत्यागम "SSL public key does not match pinned public key";

  पक्ष CURLE_SSL_INVALIDCERTSTATUS:
    प्रत्यागम "SSL server certificate status verification FAILED";

  पक्ष CURLE_HTTP2_STREAM:
    प्रत्यागम "Stream error in the HTTP/2 framing layer";

  पक्ष CURLE_RECURSIVE_API_CALL:
    प्रत्यागम "API function called from within callback";

  पक्ष CURLE_AUTH_ERROR:
    प्रत्यागम "An authentication function returned an error";

  पक्ष CURLE_HTTP3:
    प्रत्यागम "HTTP/3 error";

  पक्ष CURLE_QUIC_CONNECT_ERROR:
    प्रत्यागम "QUIC connection error";

  पक्ष CURLE_PROXY:
    प्रत्यागम "proxy handshake error";

  पक्ष CURLE_SSL_CLIENTCERT:
    प्रत्यागम "SSL Client Certificate required";

  पक्ष CURLE_UNRECOVERABLE_POLL:
    प्रत्यागम "Unrecoverable error in select/poll";

  पक्ष CURLE_TOO_LARGE:
    प्रत्यागम "A value or data field grew larger than allowed";

  पक्ष CURLE_ECH_REQUIRED:
    प्रत्यागम "ECH attempted but failed";

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
  प्रत्यागम "Unknown error";
#अण्णहा
  जइ(!error)
    प्रत्यागम "No error";
  अण्णहा
    प्रत्यागम "Error";
#endif
}

ध्रुव वर्ण *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLM_CALL_MULTI_PERFORM:
    प्रत्यागम "Please call curl_multi_perform() soon";

  पक्ष CURLM_OK:
    प्रत्यागम "No error";

  पक्ष CURLM_BAD_HANDLE:
    प्रत्यागम "Invalid multi handle";

  पक्ष CURLM_BAD_EASY_HANDLE:
    प्रत्यागम "Invalid easy handle";

  पक्ष CURLM_OUT_OF_MEMORY:
    प्रत्यागम "Out of memory";

  पक्ष CURLM_INTERNAL_ERROR:
    प्रत्यागम "Internal error";

  पक्ष CURLM_BAD_SOCKET:
    प्रत्यागम "Invalid socket argument";

  पक्ष CURLM_UNKNOWN_OPTION:
    प्रत्यागम "Unknown option";

  पक्ष CURLM_ADDED_ALREADY:
    प्रत्यागम "The easy handle is already added to a multi handle";

  पक्ष CURLM_RECURSIVE_API_CALL:
    प्रत्यागम "API function called from within callback";

  पक्ष CURLM_WAKEUP_FAILURE:
    प्रत्यागम "Wakeup is unavailable or failed";

  पक्ष CURLM_BAD_FUNCTION_ARGUMENT:
    प्रत्यागम "A libcurl function was given a bad argument";

  पक्ष CURLM_ABORTED_BY_CALLBACK:
    प्रत्यागम "Operation was aborted by an application callback";

  पक्ष CURLM_UNRECOVERABLE_POLL:
    प्रत्यागम "Unrecoverable error in select/poll";

  पक्ष CURLM_LAST:
    भङ्ग;
  }

  प्रत्यागम "Unknown error";
#अण्णहा
  जइ(error == CURLM_OK)
    प्रत्यागम "No error";
  अण्णहा
    प्रत्यागम "Error";
#endif
}

ध्रुव वर्ण *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLSHE_OK:
    प्रत्यागम "No error";

  पक्ष CURLSHE_BAD_OPTION:
    प्रत्यागम "Unknown share option";

  पक्ष CURLSHE_IN_USE:
    प्रत्यागम "Share currently in use";

  पक्ष CURLSHE_INVALID:
    प्रत्यागम "Invalid share handle";

  पक्ष CURLSHE_NOMEM:
    प्रत्यागम "Out of memory";

  पक्ष CURLSHE_NOT_BUILT_IN:
    प्रत्यागम "Feature not enabled in this library";

  पक्ष CURLSHE_LAST:
    भङ्ग;
  }

  प्रत्यागम "CURLSHcode unknown";
#अण्णहा
  जइ(error == CURLSHE_OK)
    प्रत्यागम "No error";
  अण्णहा
    प्रत्यागम "Error";
#endif
}

ध्रुव वर्ण *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLUE_OK:
    प्रत्यागम "No error";

  पक्ष CURLUE_BAD_HANDLE:
    प्रत्यागम "An invalid CURLU pointer was passed as argument";

  पक्ष CURLUE_BAD_PARTPOINTER:
    प्रत्यागम "An invalid 'part' argument was passed as argument";

  पक्ष CURLUE_MALFORMED_निवेशः:
    प्रत्यागम "Malformed input to a URL function";

  पक्ष CURLUE_BAD_PORT_सङ्ख्या:
    प्रत्यागम "Port number was not a decimal number between 0 and 65535";

  पक्ष CURLUE_UNSUPPORTED_SCHEME:
    प्रत्यागम "Unsupported URL scheme";

  पक्ष CURLUE_URLDECODE:
    प्रत्यागम "URL decode error, most likely because of rubbish in the input";

  पक्ष CURLUE_OUT_OF_MEMORY:
    प्रत्यागम "A memory function failed";

  पक्ष CURLUE_USER_NOT_ALLOWED:
    प्रत्यागम "Credentials was passed in the URL when prohibited";

  पक्ष CURLUE_UNKNOWN_PART:
    प्रत्यागम "An unknown part ID was passed to a URL API function";

  पक्ष CURLUE_NO_SCHEME:
    प्रत्यागम "No scheme present";

  पक्ष CURLUE_NO_USER:
    प्रत्यागम "No user present";

  पक्ष CURLUE_NO_PASSWORD:
    प्रत्यागम "No password present";

  पक्ष CURLUE_NO_OPTIONS:
    प्रत्यागम "No options present";

  पक्ष CURLUE_NO_HOST:
    प्रत्यागम "No host present";

  पक्ष CURLUE_NO_PORT:
    प्रत्यागम "No port number present";

  पक्ष CURLUE_NO_QUERY:
    प्रत्यागम "No query present";

  पक्ष CURLUE_NO_FRAGMENT:
    प्रत्यागम "No fragment present";

  पक्ष CURLUE_NO_ZONEID:
    प्रत्यागम "No zoneid present";

  पक्ष CURLUE_BAD_LOGIN:
    प्रत्यागम "Bad login";

  पक्ष CURLUE_BAD_IPV6:
    प्रत्यागम "Bad IPv6 address";

  पक्ष CURLUE_BAD_HOSTNAME:
    प्रत्यागम "Bad hostname";

  पक्ष CURLUE_BAD_FILE_URL:
    प्रत्यागम "Bad file:// URL";

  पक्ष CURLUE_BAD_SLASHES:
    प्रत्यागम "Unsupported number of slashes following scheme";

  पक्ष CURLUE_BAD_SCHEME:
    प्रत्यागम "Bad scheme";

  पक्ष CURLUE_BAD_PATH:
    प्रत्यागम "Bad path";

  पक्ष CURLUE_BAD_FRAGMENT:
    प्रत्यागम "Bad fragment";

  पक्ष CURLUE_BAD_QUERY:
    प्रत्यागम "Bad query";

  पक्ष CURLUE_BAD_PASSWORD:
    प्रत्यागम "Bad password";

  पक्ष CURLUE_BAD_USER:
    प्रत्यागम "Bad user";

  पक्ष CURLUE_LACKS_IDN:
    प्रत्यागम "libcurl lacks IDN support";

  पक्ष CURLUE_TOO_LARGE:
    प्रत्यागम "A value or data field is larger than allowed";

  पक्ष CURLUE_BACKSLASH:
    प्रत्यागम "Found a backslash where a forward slash was expected";

  पक्ष CURLUE_LAST:
    भङ्ग;
  }

  प्रत्यागम "CURLUcode unknown";
#अण्णहा
  जइ(error == CURLUE_OK)
    प्रत्यागम "No error";
  अण्णहा
    प्रत्यागम "Error";
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

  जइ(!buflen)
    प्रत्यागम रिक्त;

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

  जइ(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  अण्णहा {
    वर्ण msgbuf[256];
    जइ(curlx_get_winapi_error((DWORD)err, msgbuf, परिमाण(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    अण्णहा
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#अण्णहा /* CURLVERBOSE */
  जइ(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  अण्णहा
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  जइ(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  जइ(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  प्रत्यागम buf;
}
#endif /* USE_WINDOWS_SSPI */
