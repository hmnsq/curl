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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#অন্তর্ভাৱ "curl_sspi.h"
#endif

#অন্তর্ভাৱ "curlx/winapi.h"
#অন্তর্ভাৱ "strerror.h"
#অন্তর্ভাৱ "curlx/strcopy.h"

ধ্রুৱ ৱর্ণ *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  ৱিকল্প(error) {
  পক্ষ CURLE_OK:
    ঘূৰাই "No error";

  পক্ষ CURLE_UNSUPPORTED_PROTOCOL:
    ঘূৰাই "Unsupported protocol";

  পক্ষ CURLE_FAILED_INIT:
    ঘূৰাই "Failed initialization";

  পক্ষ CURLE_URL_MALFORMAT:
    ঘূৰাই "URL using bad/illegal format or missing URL";

  পক্ষ CURLE_NOT_BUILT_IN:
    ঘূৰাই "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  পক্ষ CURLE_COULDNT_RESOLVE_PROXY:
    ঘূৰাই "Could not resolve proxy name";

  পক্ষ CURLE_COULDNT_RESOLVE_HOST:
    ঘূৰাই "Could not resolve hostname";

  পক্ষ CURLE_COULDNT_CONNECT:
    ঘূৰাই "Could not connect to server";

  পক্ষ CURLE_WEIRD_SERVER_REPLY:
    ঘূৰাই "Weird server reply";

  পক্ষ CURLE_REMOTE_ACCESS_DENIED:
    ঘূৰাই "Access denied to remote resource";

  পক্ষ CURLE_FTP_ACCEPT_FAILED:
    ঘূৰাই "FTP: The server failed to connect to data port";

  পক্ষ CURLE_FTP_ACCEPT_TIMEOUT:
    ঘূৰাই "FTP: Accepting server connect has timed out";

  পক্ষ CURLE_FTP_PRET_FAILED:
    ঘূৰাই "FTP: The server did not accept the PRET command.";

  পক্ষ CURLE_FTP_WEIRD_PASS_REPLY:
    ঘূৰাই "FTP: unknown PASS reply";

  পক্ষ CURLE_FTP_WEIRD_PASV_REPLY:
    ঘূৰাই "FTP: unknown PASV reply";

  পক্ষ CURLE_FTP_WEIRD_227_FORMAT:
    ঘূৰাই "FTP: unknown 227 response format";

  পক্ষ CURLE_FTP_CANT_GET_HOST:
    ঘূৰাই "FTP: cannot figure out the host in the PASV response";

  পক্ষ CURLE_HTTP2:
    ঘূৰাই "Error in the HTTP2 framing layer";

  পক্ষ CURLE_FTP_COULDNT_SET_TYPE:
    ঘূৰাই "FTP: could not set file type";

  পক্ষ CURLE_PARTIAL_FILE:
    ঘূৰাই "Transferred a partial file";

  পক্ষ CURLE_FTP_COULDNT_RETR_FILE:
    ঘূৰাই "FTP: could not retrieve (RETR failed) the specified file";

  পক্ষ CURLE_QUOTE_ERROR:
    ঘূৰাই "Quote command returned error";

  পক্ষ CURLE_HTTP_RETURNED_ERROR:
    ঘূৰাই "HTTP response code said error";

  পক্ষ CURLE_WRITE_ERROR:
    ঘূৰাই "Failed writing received data to disk/application";

  পক্ষ CURLE_UPLOAD_FAILED:
    ঘূৰাই "Upload failed (at start/before it took off)";

  পক্ষ CURLE_READ_ERROR:
    ঘূৰাই "Failed to open/read local data from file/application";

  পক্ষ CURLE_OUT_OF_MEMORY:
    ঘূৰাই "Out of memory";

  পক্ষ CURLE_OPERATION_TIMEDOUT:
    ঘূৰাই "Timeout was reached";

  পক্ষ CURLE_FTP_PORT_FAILED:
    ঘূৰাই "FTP: command PORT failed";

  পক্ষ CURLE_FTP_COULDNT_USE_REST:
    ঘূৰাই "FTP: command REST failed";

  পক্ষ CURLE_RANGE_ERROR:
    ঘূৰাই "Requested range was not delivered by the server";

  পক্ষ CURLE_SSL_CONNECT_ERROR:
    ঘূৰাই "SSL connect error";

  পক্ষ CURLE_BAD_DOWNLOAD_RESUME:
    ঘূৰাই "Could not resume download";

  পক্ষ CURLE_FILE_COULDNT_READ_FILE:
    ঘূৰাই "Could not read a file:// file";

  পক্ষ CURLE_LDAP_CANNOT_BIND:
    ঘূৰাই "LDAP: cannot bind";

  পক্ষ CURLE_LDAP_অনুসন্ধান_FAILED:
    ঘূৰাই "LDAP: search failed";

  পক্ষ CURLE_ABORTED_BY_CALLBACK:
    ঘূৰাই "Operation was aborted by an application callback";

  পক্ষ CURLE_BAD_FUNCTION_ARGUMENT:
    ঘূৰাই "A libcurl function was given a bad argument";

  পক্ষ CURLE_INTERFACE_FAILED:
    ঘূৰাই "Failed binding local connection end";

  পক্ষ CURLE_TOO_MANY_REDIRECTS:
    ঘূৰাই "Number of redirects hit maximum amount";

  পক্ষ CURLE_UNKNOWN_OPTION:
    ঘূৰাই "An unknown option was passed in to libcurl";

  পক্ষ CURLE_SETOPT_OPTION_SYNTAX:
    ঘূৰাই "Malformed option provided in a setopt";

  পক্ষ CURLE_GOT_NOTHING:
    ঘূৰাই "Server returned nothing (no headers, no data)";

  পক্ষ CURLE_SSL_ENGINE_NOTFOUND:
    ঘূৰাই "SSL crypto engine not found";

  পক্ষ CURLE_SSL_ENGINE_SETFAILED:
    ঘূৰাই "Can not set SSL crypto engine as default";

  পক্ষ CURLE_SSL_ENGINE_INITFAILED:
    ঘূৰাই "Failed to initialize SSL crypto engine";

  পক্ষ CURLE_SEND_ERROR:
    ঘূৰাই "Failed sending data to the peer";

  পক্ষ CURLE_RECV_ERROR:
    ঘূৰাই "Failure when receiving data from the peer";

  পক্ষ CURLE_SSL_CERTPROBLEM:
    ঘূৰাই "Problem with the local SSL certificate";

  পক্ষ CURLE_SSL_CIPHER:
    ঘূৰাই "Could not use specified SSL cipher";

  পক্ষ CURLE_PEER_FAILED_VERIFICATION:
    ঘূৰাই "SSL peer certificate or SSH remote key was not OK";

  পক্ষ CURLE_SSL_CACERT_BADFILE:
    ঘূৰাই "Problem with the SSL CA cert (path? access rights?)";

  পক্ষ CURLE_BAD_CONTENT_ENCODING:
    ঘূৰাই "Unrecognized or bad HTTP Content or Transfer-Encoding";

  পক্ষ CURLE_FILESIZE_EXCEEDED:
    ঘূৰাই "Maximum file size exceeded";

  পক্ষ CURLE_USE_SSL_FAILED:
    ঘূৰাই "Requested SSL level failed";

  পক্ষ CURLE_SSL_SHUTDOWN_FAILED:
    ঘূৰাই "Failed to shut down the SSL connection";

  পক্ষ CURLE_SSL_CRL_BADFILE:
    ঘূৰাই "Failed to load CRL file (path? access rights?, format?)";

  পক্ষ CURLE_SSL_ISSUER_ERROR:
    ঘূৰাই "Issuer check against peer certificate failed";

  পক্ষ CURLE_SEND_FAIL_REWIND:
    ঘূৰাই "Send failed since rewinding of the data stream failed";

  পক্ষ CURLE_LOGIN_DENIED:
    ঘূৰাই "Login denied";

  পক্ষ CURLE_TFTP_NOTFOUND:
    ঘূৰাই "TFTP: File Not Found";

  পক্ষ CURLE_TFTP_PERM:
    ঘূৰাই "TFTP: Access Violation";

  পক্ষ CURLE_REMOTE_DISK_FULL:
    ঘূৰাই "Disk full or allocation exceeded";

  পক্ষ CURLE_TFTP_ILLEGAL:
    ঘূৰাই "TFTP: Illegal operation";

  পক্ষ CURLE_TFTP_UNKNOWNID:
    ঘূৰাই "TFTP: Unknown transfer ID";

  পক্ষ CURLE_REMOTE_FILE_EXISTS:
    ঘূৰাই "Remote file already exists";

  পক্ষ CURLE_TFTP_NOSUCHUSER:
    ঘূৰাই "TFTP: No such user";

  পক্ষ CURLE_REMOTE_FILE_NOT_FOUND:
    ঘূৰাই "Remote file not found";

  পক্ষ CURLE_SSH:
    ঘূৰাই "Error in the SSH layer";

  পক্ষ CURLE_AGAIN:
    ঘূৰাই "Socket not ready for send/recv";

  পক্ষ CURLE_RTSP_CSEQ_ERROR:
    ঘূৰাই "RTSP CSeq mismatch or invalid CSeq";

  পক্ষ CURLE_RTSP_SESSION_ERROR:
    ঘূৰাই "RTSP session error";

  পক্ষ CURLE_FTP_BAD_FILE_LIST:
    ঘূৰাই "Unable to parse FTP file list";

  পক্ষ CURLE_CHUNK_FAILED:
    ঘূৰাই "Chunk callback failed";

  পক্ষ CURLE_NO_CONNECTION_AVAILABLE:
    ঘূৰাই "The max connection limit is reached";

  পক্ষ CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    ঘূৰাই "SSL public key does not match pinned public key";

  পক্ষ CURLE_SSL_INVALIDCERTSTATUS:
    ঘূৰাই "SSL server certificate status verification FAILED";

  পক্ষ CURLE_HTTP2_STREAM:
    ঘূৰাই "Stream error in the HTTP/2 framing layer";

  পক্ষ CURLE_RECURSIVE_API_CALL:
    ঘূৰাই "API function called from within callback";

  পক্ষ CURLE_AUTH_ERROR:
    ঘূৰাই "An authentication function returned an error";

  পক্ষ CURLE_HTTP3:
    ঘূৰাই "HTTP/3 error";

  পক্ষ CURLE_QUIC_CONNECT_ERROR:
    ঘূৰাই "QUIC connection error";

  পক্ষ CURLE_PROXY:
    ঘূৰাই "proxy handshake error";

  পক্ষ CURLE_SSL_CLIENTCERT:
    ঘূৰাই "SSL Client Certificate required";

  পক্ষ CURLE_UNRECOVERABLE_POLL:
    ঘূৰাই "Unrecoverable error in select/poll";

  পক্ষ CURLE_TOO_LARGE:
    ঘূৰাই "A value or data field grew larger than allowed";

  পক্ষ CURLE_ECH_REQUIRED:
    ঘূৰাই "ECH attempted but failed";

    /* error codes not used by current libcurl */
  স্ৱভাৱ:
    ভঙ্গ;
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
  ঘূৰাই "Unknown error";
#অন্যথা
  যদি(!error)
    ঘূৰাই "No error";
  অন্যথা
    ঘূৰাই "Error";
#endif
}

ধ্রুৱ ৱর্ণ *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  ৱিকল্প(error) {
  পক্ষ CURLM_CALL_MULTI_PERFORM:
    ঘূৰাই "Please call curl_multi_perform() soon";

  পক্ষ CURLM_OK:
    ঘূৰাই "No error";

  পক্ষ CURLM_BAD_HANDLE:
    ঘূৰাই "Invalid multi handle";

  পক্ষ CURLM_BAD_EASY_HANDLE:
    ঘূৰাই "Invalid easy handle";

  পক্ষ CURLM_OUT_OF_MEMORY:
    ঘূৰাই "Out of memory";

  পক্ষ CURLM_INTERNAL_ERROR:
    ঘূৰাই "Internal error";

  পক্ষ CURLM_BAD_SOCKET:
    ঘূৰাই "Invalid socket argument";

  পক্ষ CURLM_UNKNOWN_OPTION:
    ঘূৰাই "Unknown option";

  পক্ষ CURLM_ADDED_ALREADY:
    ঘূৰাই "The easy handle is already added to a multi handle";

  পক্ষ CURLM_RECURSIVE_API_CALL:
    ঘূৰাই "API function called from within callback";

  পক্ষ CURLM_WAKEUP_FAILURE:
    ঘূৰাই "Wakeup is unavailable or failed";

  পক্ষ CURLM_BAD_FUNCTION_ARGUMENT:
    ঘূৰাই "A libcurl function was given a bad argument";

  পক্ষ CURLM_ABORTED_BY_CALLBACK:
    ঘূৰাই "Operation was aborted by an application callback";

  পক্ষ CURLM_UNRECOVERABLE_POLL:
    ঘূৰাই "Unrecoverable error in select/poll";

  পক্ষ CURLM_LAST:
    ভঙ্গ;
  }

  ঘূৰাই "Unknown error";
#অন্যথা
  যদি(error == CURLM_OK)
    ঘূৰাই "No error";
  অন্যথা
    ঘূৰাই "Error";
#endif
}

ধ্রুৱ ৱর্ণ *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  ৱিকল্প(error) {
  পক্ষ CURLSHE_OK:
    ঘূৰাই "No error";

  পক্ষ CURLSHE_BAD_OPTION:
    ঘূৰাই "Unknown share option";

  পক্ষ CURLSHE_IN_USE:
    ঘূৰাই "Share currently in use";

  পক্ষ CURLSHE_INVALID:
    ঘূৰাই "Invalid share handle";

  পক্ষ CURLSHE_NOMEM:
    ঘূৰাই "Out of memory";

  পক্ষ CURLSHE_NOT_BUILT_IN:
    ঘূৰাই "Feature not enabled in this library";

  পক্ষ CURLSHE_LAST:
    ভঙ্গ;
  }

  ঘূৰাই "CURLSHcode unknown";
#অন্যথা
  যদি(error == CURLSHE_OK)
    ঘূৰাই "No error";
  অন্যথা
    ঘূৰাই "Error";
#endif
}

ধ্রুৱ ৱর্ণ *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  ৱিকল্প(error) {
  পক্ষ CURLUE_OK:
    ঘূৰাই "No error";

  পক্ষ CURLUE_BAD_HANDLE:
    ঘূৰাই "An invalid CURLU pointer was passed as argument";

  পক্ষ CURLUE_BAD_PARTPOINTER:
    ঘূৰাই "An invalid 'part' argument was passed as argument";

  পক্ষ CURLUE_MALFORMED_প্ৰবেশ:
    ঘূৰাই "Malformed input to a URL function";

  পক্ষ CURLUE_BAD_PORT_সংখ্যা:
    ঘূৰাই "Port number was not a decimal number between 0 and 65535";

  পক্ষ CURLUE_UNSUPPORTED_SCHEME:
    ঘূৰাই "Unsupported URL scheme";

  পক্ষ CURLUE_URLDECODE:
    ঘূৰাই "URL decode error, most likely because of rubbish in the input";

  পক্ষ CURLUE_OUT_OF_MEMORY:
    ঘূৰাই "A memory function failed";

  পক্ষ CURLUE_USER_NOT_ALLOWED:
    ঘূৰাই "Credentials was passed in the URL when prohibited";

  পক্ষ CURLUE_UNKNOWN_PART:
    ঘূৰাই "An unknown part ID was passed to a URL API function";

  পক্ষ CURLUE_NO_SCHEME:
    ঘূৰাই "No scheme present";

  পক্ষ CURLUE_NO_USER:
    ঘূৰাই "No user present";

  পক্ষ CURLUE_NO_PASSWORD:
    ঘূৰাই "No password present";

  পক্ষ CURLUE_NO_OPTIONS:
    ঘূৰাই "No options present";

  পক্ষ CURLUE_NO_HOST:
    ঘূৰাই "No host present";

  পক্ষ CURLUE_NO_PORT:
    ঘূৰাই "No port number present";

  পক্ষ CURLUE_NO_QUERY:
    ঘূৰাই "No query present";

  পক্ষ CURLUE_NO_FRAGMENT:
    ঘূৰাই "No fragment present";

  পক্ষ CURLUE_NO_ZONEID:
    ঘূৰাই "No zoneid present";

  পক্ষ CURLUE_BAD_LOGIN:
    ঘূৰাই "Bad login";

  পক্ষ CURLUE_BAD_IPV6:
    ঘূৰাই "Bad IPv6 address";

  পক্ষ CURLUE_BAD_HOSTNAME:
    ঘূৰাই "Bad hostname";

  পক্ষ CURLUE_BAD_FILE_URL:
    ঘূৰাই "Bad file:// URL";

  পক্ষ CURLUE_BAD_SLASHES:
    ঘূৰাই "Unsupported number of slashes following scheme";

  পক্ষ CURLUE_BAD_SCHEME:
    ঘূৰাই "Bad scheme";

  পক্ষ CURLUE_BAD_PATH:
    ঘূৰাই "Bad path";

  পক্ষ CURLUE_BAD_FRAGMENT:
    ঘূৰাই "Bad fragment";

  পক্ষ CURLUE_BAD_QUERY:
    ঘূৰাই "Bad query";

  পক্ষ CURLUE_BAD_PASSWORD:
    ঘূৰাই "Bad password";

  পক্ষ CURLUE_BAD_USER:
    ঘূৰাই "Bad user";

  পক্ষ CURLUE_LACKS_IDN:
    ঘূৰাই "libcurl lacks IDN support";

  পক্ষ CURLUE_TOO_LARGE:
    ঘূৰাই "A value or data field is larger than allowed";

  পক্ষ CURLUE_BACKSLASH:
    ঘূৰাই "Found a backslash where a forward slash was expected";

  পক্ষ CURLUE_LAST:
    ভঙ্গ;
  }

  ঘূৰাই "CURLUcode unknown";
#অন্যথা
  যদি(error == CURLUE_OK)
    ঘূৰাই "No error";
  অন্যথা
    ঘূৰাই "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ধ্রুৱ ৱর্ণ *Curl_sspi_strerror(SECURITY_STATUS err, ৱর্ণ *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  পূর্ণাঙ্ক old_errno = errno;
  VERBOSE(ধ্রুৱ ৱর্ণ *txt);

  যদি(!buflen)
    ঘূৰাই রিক্ত;

  *buf = '\0';

#ifdef CURLVERBOSE
  ৱিকল্প(err) {
  পক্ষ SEC_E_OK:
    txt = "No error";
    ভঙ্গ;
#define SEC2TXT(sec) পক্ষ sec: txt = #sec; ভঙ্গ
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
  SEC2TXT(SEC_E_NO_KERB_চাবি);
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
  SEC2TXT(SEC_E_UNTRUSTED_মূল);
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
  স্ৱভাৱ:
    txt = "Unknown error";
  }

  যদি(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  অন্যথা {
    ৱর্ণ msgbuf[256];
    যদি(curlx_get_winapi_error((DWORD)err, msgbuf, পরিমাণ(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    অন্যথা
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#অন্যথা /* CURLVERBOSE */
  যদি(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  অন্যথা
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  যদি(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  যদি(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  ঘূৰাই buf;
}
#endif /* USE_WINDOWS_SSPI */
