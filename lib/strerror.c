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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#অন্তর্ভাব "curl_sspi.h"
#endif

#অন্তর্ভাব "curlx/winapi.h"
#অন্তর্ভাব "strerror.h"
#অন্তর্ভাব "curlx/strcopy.h"

ধ্রুব বর্ণ *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  বিকল্প(error) {
  পক্ষ CURLE_OK:
    প্রত্যাগম "No error";

  পক্ষ CURLE_UNSUPPORTED_PROTOCOL:
    প্রত্যাগম "Unsupported protocol";

  পক্ষ CURLE_FAILED_INIT:
    প্রত্যাগম "Failed initialization";

  পক্ষ CURLE_URL_MALFORMAT:
    প্রত্যাগম "URL using bad/illegal format or missing URL";

  পক্ষ CURLE_NOT_BUILT_IN:
    প্রত্যাগম "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  পক্ষ CURLE_COULDNT_RESOLVE_PROXY:
    প্রত্যাগম "Could not resolve proxy name";

  পক্ষ CURLE_COULDNT_RESOLVE_HOST:
    প্রত্যাগম "Could not resolve hostname";

  পক্ষ CURLE_COULDNT_CONNECT:
    প্রত্যাগম "Could not connect to server";

  পক্ষ CURLE_WEIRD_SERVER_REPLY:
    প্রত্যাগম "Weird server reply";

  পক্ষ CURLE_REMOTE_ACCESS_DENIED:
    প্রত্যাগম "Access denied to remote resource";

  পক্ষ CURLE_FTP_ACCEPT_FAILED:
    প্রত্যাগম "FTP: The server failed to connect to data port";

  পক্ষ CURLE_FTP_ACCEPT_TIMEOUT:
    প্রত্যাগম "FTP: Accepting server connect has timed out";

  পক্ষ CURLE_FTP_PRET_FAILED:
    প্রত্যাগম "FTP: The server did not accept the PRET command.";

  পক্ষ CURLE_FTP_WEIRD_PASS_REPLY:
    প্রত্যাগম "FTP: unknown PASS reply";

  পক্ষ CURLE_FTP_WEIRD_PASV_REPLY:
    প্রত্যাগম "FTP: unknown PASV reply";

  পক্ষ CURLE_FTP_WEIRD_227_FORMAT:
    প্রত্যাগম "FTP: unknown 227 response format";

  পক্ষ CURLE_FTP_CANT_GET_HOST:
    প্রত্যাগম "FTP: cannot figure out the host in the PASV response";

  পক্ষ CURLE_HTTP2:
    প্রত্যাগম "Error in the HTTP2 framing layer";

  পক্ষ CURLE_FTP_COULDNT_SET_TYPE:
    প্রত্যাগম "FTP: could not set file type";

  পক্ষ CURLE_PARTIAL_FILE:
    প্রত্যাগম "Transferred a partial file";

  পক্ষ CURLE_FTP_COULDNT_RETR_FILE:
    প্রত্যাগম "FTP: could not retrieve (RETR failed) the specified file";

  পক্ষ CURLE_QUOTE_ERROR:
    প্রত্যাগম "Quote command returned error";

  পক্ষ CURLE_HTTP_RETURNED_ERROR:
    প্রত্যাগম "HTTP response code said error";

  পক্ষ CURLE_WRITE_ERROR:
    প্রত্যাগম "Failed writing received data to disk/application";

  পক্ষ CURLE_UPLOAD_FAILED:
    প্রত্যাগম "Upload failed (at start/before it took off)";

  পক্ষ CURLE_READ_ERROR:
    প্রত্যাগম "Failed to open/read local data from file/application";

  পক্ষ CURLE_OUT_OF_MEMORY:
    প্রত্যাগম "Out of memory";

  পক্ষ CURLE_OPERATION_TIMEDOUT:
    প্রত্যাগম "Timeout was reached";

  পক্ষ CURLE_FTP_PORT_FAILED:
    প্রত্যাগম "FTP: command PORT failed";

  পক্ষ CURLE_FTP_COULDNT_USE_REST:
    প্রত্যাগম "FTP: command REST failed";

  পক্ষ CURLE_RANGE_ERROR:
    প্রত্যাগম "Requested range was not delivered by the server";

  পক্ষ CURLE_SSL_CONNECT_ERROR:
    প্রত্যাগম "SSL connect error";

  পক্ষ CURLE_BAD_DOWNLOAD_RESUME:
    প্রত্যাগম "Could not resume download";

  পক্ষ CURLE_FILE_COULDNT_READ_FILE:
    প্রত্যাগম "Could not read a file:// file";

  পক্ষ CURLE_LDAP_CANNOT_BIND:
    প্রত্যাগম "LDAP: cannot bind";

  পক্ষ CURLE_LDAP_অনুসন্ধান_FAILED:
    প্রত্যাগম "LDAP: search failed";

  পক্ষ CURLE_ABORTED_BY_CALLBACK:
    প্রত্যাগম "Operation was aborted by an application callback";

  পক্ষ CURLE_BAD_FUNCTION_ARGUMENT:
    প্রত্যাগম "A libcurl function was given a bad argument";

  পক্ষ CURLE_INTERFACE_FAILED:
    প্রত্যাগম "Failed binding local connection end";

  পক্ষ CURLE_TOO_MANY_REDIRECTS:
    প্রত্যাগম "Number of redirects hit maximum amount";

  পক্ষ CURLE_UNKNOWN_OPTION:
    প্রত্যাগম "An unknown option was passed in to libcurl";

  পক্ষ CURLE_SETOPT_OPTION_SYNTAX:
    প্রত্যাগম "Malformed option provided in a setopt";

  পক্ষ CURLE_GOT_NOTHING:
    প্রত্যাগম "Server returned nothing (no headers, no data)";

  পক্ষ CURLE_SSL_ENGINE_NOTFOUND:
    প্রত্যাগম "SSL crypto engine not found";

  পক্ষ CURLE_SSL_ENGINE_SETFAILED:
    প্রত্যাগম "Can not set SSL crypto engine as default";

  পক্ষ CURLE_SSL_ENGINE_INITFAILED:
    প্রত্যাগম "Failed to initialize SSL crypto engine";

  পক্ষ CURLE_SEND_ERROR:
    প্রত্যাগম "Failed sending data to the peer";

  পক্ষ CURLE_RECV_ERROR:
    প্রত্যাগম "Failure when receiving data from the peer";

  পক্ষ CURLE_SSL_CERTPROBLEM:
    প্রত্যাগম "Problem with the local SSL certificate";

  পক্ষ CURLE_SSL_CIPHER:
    প্রত্যাগম "Could not use specified SSL cipher";

  পক্ষ CURLE_PEER_FAILED_VERIFICATION:
    প্রত্যাগম "SSL peer certificate or SSH remote key was not OK";

  পক্ষ CURLE_SSL_CACERT_BADFILE:
    প্রত্যাগম "Problem with the SSL CA cert (path? access rights?)";

  পক্ষ CURLE_BAD_CONTENT_ENCODING:
    প্রত্যাগম "Unrecognized or bad HTTP Content or Transfer-Encoding";

  পক্ষ CURLE_FILESIZE_EXCEEDED:
    প্রত্যাগম "Maximum file size exceeded";

  পক্ষ CURLE_USE_SSL_FAILED:
    প্রত্যাগম "Requested SSL level failed";

  পক্ষ CURLE_SSL_SHUTDOWN_FAILED:
    প্রত্যাগম "Failed to shut down the SSL connection";

  পক্ষ CURLE_SSL_CRL_BADFILE:
    প্রত্যাগম "Failed to load CRL file (path? access rights?, format?)";

  পক্ষ CURLE_SSL_ISSUER_ERROR:
    প্রত্যাগম "Issuer check against peer certificate failed";

  পক্ষ CURLE_SEND_FAIL_REWIND:
    প্রত্যাগম "Send failed since rewinding of the data stream failed";

  পক্ষ CURLE_LOGIN_DENIED:
    প্রত্যাগম "Login denied";

  পক্ষ CURLE_TFTP_NOTFOUND:
    প্রত্যাগম "TFTP: File Not Found";

  পক্ষ CURLE_TFTP_PERM:
    প্রত্যাগম "TFTP: Access Violation";

  পক্ষ CURLE_REMOTE_DISK_FULL:
    প্রত্যাগম "Disk full or allocation exceeded";

  পক্ষ CURLE_TFTP_ILLEGAL:
    প্রত্যাগম "TFTP: Illegal operation";

  পক্ষ CURLE_TFTP_UNKNOWNID:
    প্রত্যাগম "TFTP: Unknown transfer ID";

  পক্ষ CURLE_REMOTE_FILE_EXISTS:
    প্রত্যাগম "Remote file already exists";

  পক্ষ CURLE_TFTP_NOSUCHUSER:
    প্রত্যাগম "TFTP: No such user";

  পক্ষ CURLE_REMOTE_FILE_NOT_FOUND:
    প্রত্যাগম "Remote file not found";

  পক্ষ CURLE_SSH:
    প্রত্যাগম "Error in the SSH layer";

  পক্ষ CURLE_AGAIN:
    প্রত্যাগম "Socket not ready for send/recv";

  পক্ষ CURLE_RTSP_CSEQ_ERROR:
    প্রত্যাগম "RTSP CSeq mismatch or invalid CSeq";

  পক্ষ CURLE_RTSP_SESSION_ERROR:
    প্রত্যাগম "RTSP session error";

  পক্ষ CURLE_FTP_BAD_FILE_LIST:
    প্রত্যাগম "Unable to parse FTP file list";

  পক্ষ CURLE_CHUNK_FAILED:
    প্রত্যাগম "Chunk callback failed";

  পক্ষ CURLE_NO_CONNECTION_AVAILABLE:
    প্রত্যাগম "The max connection limit is reached";

  পক্ষ CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    প্রত্যাগম "SSL public key does not match pinned public key";

  পক্ষ CURLE_SSL_INVALIDCERTSTATUS:
    প্রত্যাগম "SSL server certificate status verification FAILED";

  পক্ষ CURLE_HTTP2_STREAM:
    প্রত্যাগম "Stream error in the HTTP/2 framing layer";

  পক্ষ CURLE_RECURSIVE_API_CALL:
    প্রত্যাগম "API function called from within callback";

  পক্ষ CURLE_AUTH_ERROR:
    প্রত্যাগম "An authentication function returned an error";

  পক্ষ CURLE_HTTP3:
    প্রত্যাগম "HTTP/3 error";

  পক্ষ CURLE_QUIC_CONNECT_ERROR:
    প্রত্যাগম "QUIC connection error";

  পক্ষ CURLE_PROXY:
    প্রত্যাগম "proxy handshake error";

  পক্ষ CURLE_SSL_CLIENTCERT:
    প্রত্যাগম "SSL Client Certificate required";

  পক্ষ CURLE_UNRECOVERABLE_POLL:
    প্রত্যাগম "Unrecoverable error in select/poll";

  পক্ষ CURLE_TOO_LARGE:
    প্রত্যাগম "A value or data field grew larger than allowed";

  পক্ষ CURLE_ECH_REQUIRED:
    প্রত্যাগম "ECH attempted but failed";

    /* error codes not used by current libcurl */
  স্বভাব:
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
  প্রত্যাগম "Unknown error";
#অন্যথা
  যদি(!error)
    প্রত্যাগম "No error";
  অন্যথা
    প্রত্যাগম "Error";
#endif
}

ধ্রুব বর্ণ *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  বিকল্প(error) {
  পক্ষ CURLM_CALL_MULTI_PERFORM:
    প্রত্যাগম "Please call curl_multi_perform() soon";

  পক্ষ CURLM_OK:
    প্রত্যাগম "No error";

  পক্ষ CURLM_BAD_HANDLE:
    প্রত্যাগম "Invalid multi handle";

  পক্ষ CURLM_BAD_EASY_HANDLE:
    প্রত্যাগম "Invalid easy handle";

  পক্ষ CURLM_OUT_OF_MEMORY:
    প্রত্যাগম "Out of memory";

  পক্ষ CURLM_INTERNAL_ERROR:
    প্রত্যাগম "Internal error";

  পক্ষ CURLM_BAD_SOCKET:
    প্রত্যাগম "Invalid socket argument";

  পক্ষ CURLM_UNKNOWN_OPTION:
    প্রত্যাগম "Unknown option";

  পক্ষ CURLM_ADDED_ALREADY:
    প্রত্যাগম "The easy handle is already added to a multi handle";

  পক্ষ CURLM_RECURSIVE_API_CALL:
    প্রত্যাগম "API function called from within callback";

  পক্ষ CURLM_WAKEUP_FAILURE:
    প্রত্যাগম "Wakeup is unavailable or failed";

  পক্ষ CURLM_BAD_FUNCTION_ARGUMENT:
    প্রত্যাগম "A libcurl function was given a bad argument";

  পক্ষ CURLM_ABORTED_BY_CALLBACK:
    প্রত্যাগম "Operation was aborted by an application callback";

  পক্ষ CURLM_UNRECOVERABLE_POLL:
    প্রত্যাগম "Unrecoverable error in select/poll";

  পক্ষ CURLM_LAST:
    ভঙ্গ;
  }

  প্রত্যাগম "Unknown error";
#অন্যথা
  যদি(error == CURLM_OK)
    প্রত্যাগম "No error";
  অন্যথা
    প্রত্যাগম "Error";
#endif
}

ধ্রুব বর্ণ *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  বিকল্প(error) {
  পক্ষ CURLSHE_OK:
    প্রত্যাগম "No error";

  পক্ষ CURLSHE_BAD_OPTION:
    প্রত্যাগম "Unknown share option";

  পক্ষ CURLSHE_IN_USE:
    প্রত্যাগম "Share currently in use";

  পক্ষ CURLSHE_INVALID:
    প্রত্যাগম "Invalid share handle";

  পক্ষ CURLSHE_NOMEM:
    প্রত্যাগম "Out of memory";

  পক্ষ CURLSHE_NOT_BUILT_IN:
    প্রত্যাগম "Feature not enabled in this library";

  পক্ষ CURLSHE_LAST:
    ভঙ্গ;
  }

  প্রত্যাগম "CURLSHcode unknown";
#অন্যথা
  যদি(error == CURLSHE_OK)
    প্রত্যাগম "No error";
  অন্যথা
    প্রত্যাগম "Error";
#endif
}

ধ্রুব বর্ণ *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  বিকল্প(error) {
  পক্ষ CURLUE_OK:
    প্রত্যাগম "No error";

  পক্ষ CURLUE_BAD_HANDLE:
    প্রত্যাগম "An invalid CURLU pointer was passed as argument";

  পক্ষ CURLUE_BAD_PARTPOINTER:
    প্রত্যাগম "An invalid 'part' argument was passed as argument";

  পক্ষ CURLUE_MALFORMED_প্রবেশ:
    প্রত্যাগম "Malformed input to a URL function";

  পক্ষ CURLUE_BAD_PORT_সংখ্যা:
    প্রত্যাগম "Port number was not a decimal number between 0 and 65535";

  পক্ষ CURLUE_UNSUPPORTED_SCHEME:
    প্রত্যাগম "Unsupported URL scheme";

  পক্ষ CURLUE_URLDECODE:
    প্রত্যাগম "URL decode error, most likely because of rubbish in the input";

  পক্ষ CURLUE_OUT_OF_MEMORY:
    প্রত্যাগম "A memory function failed";

  পক্ষ CURLUE_USER_NOT_ALLOWED:
    প্রত্যাগম "Credentials was passed in the URL when prohibited";

  পক্ষ CURLUE_UNKNOWN_PART:
    প্রত্যাগম "An unknown part ID was passed to a URL API function";

  পক্ষ CURLUE_NO_SCHEME:
    প্রত্যাগম "No scheme present";

  পক্ষ CURLUE_NO_USER:
    প্রত্যাগম "No user present";

  পক্ষ CURLUE_NO_PASSWORD:
    প্রত্যাগম "No password present";

  পক্ষ CURLUE_NO_OPTIONS:
    প্রত্যাগম "No options present";

  পক্ষ CURLUE_NO_HOST:
    প্রত্যাগম "No host present";

  পক্ষ CURLUE_NO_PORT:
    প্রত্যাগম "No port number present";

  পক্ষ CURLUE_NO_QUERY:
    প্রত্যাগম "No query present";

  পক্ষ CURLUE_NO_FRAGMENT:
    প্রত্যাগম "No fragment present";

  পক্ষ CURLUE_NO_ZONEID:
    প্রত্যাগম "No zoneid present";

  পক্ষ CURLUE_BAD_LOGIN:
    প্রত্যাগম "Bad login";

  পক্ষ CURLUE_BAD_IPV6:
    প্রত্যাগম "Bad IPv6 address";

  পক্ষ CURLUE_BAD_HOSTNAME:
    প্রত্যাগম "Bad hostname";

  পক্ষ CURLUE_BAD_FILE_URL:
    প্রত্যাগম "Bad file:// URL";

  পক্ষ CURLUE_BAD_SLASHES:
    প্রত্যাগম "Unsupported number of slashes following scheme";

  পক্ষ CURLUE_BAD_SCHEME:
    প্রত্যাগম "Bad scheme";

  পক্ষ CURLUE_BAD_PATH:
    প্রত্যাগম "Bad path";

  পক্ষ CURLUE_BAD_FRAGMENT:
    প্রত্যাগম "Bad fragment";

  পক্ষ CURLUE_BAD_QUERY:
    প্রত্যাগম "Bad query";

  পক্ষ CURLUE_BAD_PASSWORD:
    প্রত্যাগম "Bad password";

  পক্ষ CURLUE_BAD_USER:
    প্রত্যাগম "Bad user";

  পক্ষ CURLUE_LACKS_IDN:
    প্রত্যাগম "libcurl lacks IDN support";

  পক্ষ CURLUE_TOO_LARGE:
    প্রত্যাগম "A value or data field is larger than allowed";

  পক্ষ CURLUE_BACKSLASH:
    প্রত্যাগম "Found a backslash where a forward slash was expected";

  পক্ষ CURLUE_LAST:
    ভঙ্গ;
  }

  প্রত্যাগম "CURLUcode unknown";
#অন্যথা
  যদি(error == CURLUE_OK)
    প্রত্যাগম "No error";
  অন্যথা
    প্রত্যাগম "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ধ্রুব বর্ণ *Curl_sspi_strerror(SECURITY_STATUS err, বর্ণ *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  পূর্ণাঙ্ক old_errno = errno;
  VERBOSE(ধ্রুব বর্ণ *txt);

  যদি(!buflen)
    প্রত্যাগম রিক্ত;

  *buf = '\0';

#ifdef CURLVERBOSE
  বিকল্প(err) {
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
  স্বভাব:
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
    বর্ণ msgbuf[256];
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

  প্রত্যাগম buf;
}
#endif /* USE_WINDOWS_SSPI */
