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

ধ্রুবক অক্ষর *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  নির্বাচন(error) {
  ক্ষেত্র CURLE_OK:
    ফেরত "No error";

  ক্ষেত্র CURLE_UNSUPPORTED_PROTOCOL:
    ফেরত "Unsupported protocol";

  ক্ষেত্র CURLE_FAILED_INIT:
    ফেরত "Failed initialization";

  ক্ষেত্র CURLE_URL_MALFORMAT:
    ফেরত "URL using bad/illegal format or missing URL";

  ক্ষেত্র CURLE_NOT_BUILT_IN:
    ফেরত "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  ক্ষেত্র CURLE_COULDNT_RESOLVE_PROXY:
    ফেরত "Could not resolve proxy name";

  ক্ষেত্র CURLE_COULDNT_RESOLVE_HOST:
    ফেরত "Could not resolve hostname";

  ক্ষেত্র CURLE_COULDNT_CONNECT:
    ফেরত "Could not connect to server";

  ক্ষেত্র CURLE_WEIRD_SERVER_REPLY:
    ফেরত "Weird server reply";

  ক্ষেত্র CURLE_REMOTE_ACCESS_DENIED:
    ফেরত "Access denied to remote resource";

  ক্ষেত্র CURLE_FTP_ACCEPT_FAILED:
    ফেরত "FTP: The server failed to connect to data port";

  ক্ষেত্র CURLE_FTP_ACCEPT_TIMEOUT:
    ফেরত "FTP: Accepting server connect has timed out";

  ক্ষেত্র CURLE_FTP_PRET_FAILED:
    ফেরত "FTP: The server did not accept the PRET command.";

  ক্ষেত্র CURLE_FTP_WEIRD_PASS_REPLY:
    ফেরত "FTP: unknown PASS reply";

  ক্ষেত্র CURLE_FTP_WEIRD_PASV_REPLY:
    ফেরত "FTP: unknown PASV reply";

  ক্ষেত্র CURLE_FTP_WEIRD_227_FORMAT:
    ফেরত "FTP: unknown 227 response format";

  ক্ষেত্র CURLE_FTP_CANT_GET_HOST:
    ফেরত "FTP: cannot figure out the host in the PASV response";

  ক্ষেত্র CURLE_HTTP2:
    ফেরত "Error in the HTTP2 framing layer";

  ক্ষেত্র CURLE_FTP_COULDNT_SET_TYPE:
    ফেরত "FTP: could not set file type";

  ক্ষেত্র CURLE_PARTIAL_FILE:
    ফেরত "Transferred a partial file";

  ক্ষেত্র CURLE_FTP_COULDNT_RETR_FILE:
    ফেরত "FTP: could not retrieve (RETR failed) the specified file";

  ক্ষেত্র CURLE_QUOTE_ERROR:
    ফেরত "Quote command returned error";

  ক্ষেত্র CURLE_HTTP_RETURNED_ERROR:
    ফেরত "HTTP response code said error";

  ক্ষেত্র CURLE_WRITE_ERROR:
    ফেরত "Failed writing received data to disk/application";

  ক্ষেত্র CURLE_UPLOAD_FAILED:
    ফেরত "Upload failed (at start/before it took off)";

  ক্ষেত্র CURLE_READ_ERROR:
    ফেরত "Failed to open/read local data from file/application";

  ক্ষেত্র CURLE_OUT_OF_MEMORY:
    ফেরত "Out of memory";

  ক্ষেত্র CURLE_OPERATION_TIMEDOUT:
    ফেরত "Timeout was reached";

  ক্ষেত্র CURLE_FTP_PORT_FAILED:
    ফেরত "FTP: command PORT failed";

  ক্ষেত্র CURLE_FTP_COULDNT_USE_REST:
    ফেরত "FTP: command REST failed";

  ক্ষেত্র CURLE_RANGE_ERROR:
    ফেরত "Requested range was not delivered by the server";

  ক্ষেত্র CURLE_SSL_CONNECT_ERROR:
    ফেরত "SSL connect error";

  ক্ষেত্র CURLE_BAD_DOWNLOAD_RESUME:
    ফেরত "Could not resume download";

  ক্ষেত্র CURLE_FILE_COULDNT_READ_FILE:
    ফেরত "Could not read a file:// file";

  ক্ষেত্র CURLE_LDAP_CANNOT_BIND:
    ফেরত "LDAP: cannot bind";

  ক্ষেত্র CURLE_LDAP_অনুসন্ধান_FAILED:
    ফেরত "LDAP: search failed";

  ক্ষেত্র CURLE_ABORTED_BY_CALLBACK:
    ফেরত "Operation was aborted by an application callback";

  ক্ষেত্র CURLE_BAD_FUNCTION_ARGUMENT:
    ফেরত "A libcurl function was given a bad argument";

  ক্ষেত্র CURLE_INTERFACE_FAILED:
    ফেরত "Failed binding local connection end";

  ক্ষেত্র CURLE_TOO_MANY_REDIRECTS:
    ফেরত "Number of redirects hit maximum amount";

  ক্ষেত্র CURLE_UNKNOWN_OPTION:
    ফেরত "An unknown option was passed in to libcurl";

  ক্ষেত্র CURLE_SETOPT_OPTION_SYNTAX:
    ফেরত "Malformed option provided in a setopt";

  ক্ষেত্র CURLE_GOT_NOTHING:
    ফেরত "Server returned nothing (no headers, no data)";

  ক্ষেত্র CURLE_SSL_ENGINE_NOTFOUND:
    ফেরত "SSL crypto engine not found";

  ক্ষেত্র CURLE_SSL_ENGINE_SETFAILED:
    ফেরত "Can not set SSL crypto engine as default";

  ক্ষেত্র CURLE_SSL_ENGINE_INITFAILED:
    ফেরত "Failed to initialize SSL crypto engine";

  ক্ষেত্র CURLE_SEND_ERROR:
    ফেরত "Failed sending data to the peer";

  ক্ষেত্র CURLE_RECV_ERROR:
    ফেরত "Failure when receiving data from the peer";

  ক্ষেত্র CURLE_SSL_CERTPROBLEM:
    ফেরত "Problem with the local SSL certificate";

  ক্ষেত্র CURLE_SSL_CIPHER:
    ফেরত "Could not use specified SSL cipher";

  ক্ষেত্র CURLE_PEER_FAILED_VERIFICATION:
    ফেরত "SSL peer certificate or SSH remote key was not OK";

  ক্ষেত্র CURLE_SSL_CACERT_BADFILE:
    ফেরত "Problem with the SSL CA cert (path? access rights?)";

  ক্ষেত্র CURLE_BAD_CONTENT_ENCODING:
    ফেরত "Unrecognized or bad HTTP Content or Transfer-Encoding";

  ক্ষেত্র CURLE_FILESIZE_EXCEEDED:
    ফেরত "Maximum file size exceeded";

  ক্ষেত্র CURLE_USE_SSL_FAILED:
    ফেরত "Requested SSL level failed";

  ক্ষেত্র CURLE_SSL_SHUTDOWN_FAILED:
    ফেরত "Failed to shut down the SSL connection";

  ক্ষেত্র CURLE_SSL_CRL_BADFILE:
    ফেরত "Failed to load CRL file (path? access rights?, format?)";

  ক্ষেত্র CURLE_SSL_ISSUER_ERROR:
    ফেরত "Issuer check against peer certificate failed";

  ক্ষেত্র CURLE_SEND_FAIL_REWIND:
    ফেরত "Send failed since rewinding of the data stream failed";

  ক্ষেত্র CURLE_LOGIN_DENIED:
    ফেরত "Login denied";

  ক্ষেত্র CURLE_TFTP_NOTFOUND:
    ফেরত "TFTP: File Not Found";

  ক্ষেত্র CURLE_TFTP_PERM:
    ফেরত "TFTP: Access Violation";

  ক্ষেত্র CURLE_REMOTE_DISK_FULL:
    ফেরত "Disk full or allocation exceeded";

  ক্ষেত্র CURLE_TFTP_ILLEGAL:
    ফেরত "TFTP: Illegal operation";

  ক্ষেত্র CURLE_TFTP_UNKNOWNID:
    ফেরত "TFTP: Unknown transfer ID";

  ক্ষেত্র CURLE_REMOTE_FILE_EXISTS:
    ফেরত "Remote file already exists";

  ক্ষেত্র CURLE_TFTP_NOSUCHUSER:
    ফেরত "TFTP: No such user";

  ক্ষেত্র CURLE_REMOTE_FILE_NOT_FOUND:
    ফেরত "Remote file not found";

  ক্ষেত্র CURLE_SSH:
    ফেরত "Error in the SSH layer";

  ক্ষেত্র CURLE_AGAIN:
    ফেরত "Socket not ready for send/recv";

  ক্ষেত্র CURLE_RTSP_CSEQ_ERROR:
    ফেরত "RTSP CSeq mismatch or invalid CSeq";

  ক্ষেত্র CURLE_RTSP_SESSION_ERROR:
    ফেরত "RTSP session error";

  ক্ষেত্র CURLE_FTP_BAD_FILE_LIST:
    ফেরত "Unable to parse FTP file list";

  ক্ষেত্র CURLE_CHUNK_FAILED:
    ফেরত "Chunk callback failed";

  ক্ষেত্র CURLE_NO_CONNECTION_AVAILABLE:
    ফেরত "The max connection limit is reached";

  ক্ষেত্র CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    ফেরত "SSL public key does not match pinned public key";

  ক্ষেত্র CURLE_SSL_INVALIDCERTSTATUS:
    ফেরত "SSL server certificate status verification FAILED";

  ক্ষেত্র CURLE_HTTP2_STREAM:
    ফেরত "Stream error in the HTTP/2 framing layer";

  ক্ষেত্র CURLE_RECURSIVE_API_CALL:
    ফেরত "API function called from within callback";

  ক্ষেত্র CURLE_AUTH_ERROR:
    ফেরত "An authentication function returned an error";

  ক্ষেত্র CURLE_HTTP3:
    ফেরত "HTTP/3 error";

  ক্ষেত্র CURLE_QUIC_CONNECT_ERROR:
    ফেরত "QUIC connection error";

  ক্ষেত্র CURLE_PROXY:
    ফেরত "proxy handshake error";

  ক্ষেত্র CURLE_SSL_CLIENTCERT:
    ফেরত "SSL Client Certificate required";

  ক্ষেত্র CURLE_UNRECOVERABLE_POLL:
    ফেরত "Unrecoverable error in select/poll";

  ক্ষেত্র CURLE_TOO_LARGE:
    ফেরত "A value or data field grew larger than allowed";

  ক্ষেত্র CURLE_ECH_REQUIRED:
    ফেরত "ECH attempted but failed";

    /* error codes not used by current libcurl */
  পূর্বনির্ধারিত:
    বিরতি;
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
  ফেরত "Unknown error";
#অন্যথা
  যদি(!error)
    ফেরত "No error";
  অন্যথা
    ফেরত "Error";
#endif
}

ধ্রুবক অক্ষর *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  নির্বাচন(error) {
  ক্ষেত্র CURLM_CALL_MULTI_PERFORM:
    ফেরত "Please call curl_multi_perform() soon";

  ক্ষেত্র CURLM_OK:
    ফেরত "No error";

  ক্ষেত্র CURLM_BAD_HANDLE:
    ফেরত "Invalid multi handle";

  ক্ষেত্র CURLM_BAD_EASY_HANDLE:
    ফেরত "Invalid easy handle";

  ক্ষেত্র CURLM_OUT_OF_MEMORY:
    ফেরত "Out of memory";

  ক্ষেত্র CURLM_INTERNAL_ERROR:
    ফেরত "Internal error";

  ক্ষেত্র CURLM_BAD_SOCKET:
    ফেরত "Invalid socket argument";

  ক্ষেত্র CURLM_UNKNOWN_OPTION:
    ফেরত "Unknown option";

  ক্ষেত্র CURLM_ADDED_ALREADY:
    ফেরত "The easy handle is already added to a multi handle";

  ক্ষেত্র CURLM_RECURSIVE_API_CALL:
    ফেরত "API function called from within callback";

  ক্ষেত্র CURLM_WAKEUP_FAILURE:
    ফেরত "Wakeup is unavailable or failed";

  ক্ষেত্র CURLM_BAD_FUNCTION_ARGUMENT:
    ফেরত "A libcurl function was given a bad argument";

  ক্ষেত্র CURLM_ABORTED_BY_CALLBACK:
    ফেরত "Operation was aborted by an application callback";

  ক্ষেত্র CURLM_UNRECOVERABLE_POLL:
    ফেরত "Unrecoverable error in select/poll";

  ক্ষেত্র CURLM_LAST:
    বিরতি;
  }

  ফেরত "Unknown error";
#অন্যথা
  যদি(error == CURLM_OK)
    ফেরত "No error";
  অন্যথা
    ফেরত "Error";
#endif
}

ধ্রুবক অক্ষর *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  নির্বাচন(error) {
  ক্ষেত্র CURLSHE_OK:
    ফেরত "No error";

  ক্ষেত্র CURLSHE_BAD_OPTION:
    ফেরত "Unknown share option";

  ক্ষেত্র CURLSHE_IN_USE:
    ফেরত "Share currently in use";

  ক্ষেত্র CURLSHE_INVALID:
    ফেরত "Invalid share handle";

  ক্ষেত্র CURLSHE_NOMEM:
    ফেরত "Out of memory";

  ক্ষেত্র CURLSHE_NOT_BUILT_IN:
    ফেরত "Feature not enabled in this library";

  ক্ষেত্র CURLSHE_LAST:
    বিরতি;
  }

  ফেরত "CURLSHcode unknown";
#অন্যথা
  যদি(error == CURLSHE_OK)
    ফেরত "No error";
  অন্যথা
    ফেরত "Error";
#endif
}

ধ্রুবক অক্ষর *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  নির্বাচন(error) {
  ক্ষেত্র CURLUE_OK:
    ফেরত "No error";

  ক্ষেত্র CURLUE_BAD_HANDLE:
    ফেরত "An invalid CURLU pointer was passed as argument";

  ক্ষেত্র CURLUE_BAD_PARTPOINTER:
    ফেরত "An invalid 'part' argument was passed as argument";

  ক্ষেত্র CURLUE_MALFORMED_প্রবেশ:
    ফেরত "Malformed input to a URL function";

  ক্ষেত্র CURLUE_BAD_PORT_সংখ্যা:
    ফেরত "Port number was not a decimal number between 0 and 65535";

  ক্ষেত্র CURLUE_UNSUPPORTED_SCHEME:
    ফেরত "Unsupported URL scheme";

  ক্ষেত্র CURLUE_URLDECODE:
    ফেরত "URL decode error, most likely because of rubbish in the input";

  ক্ষেত্র CURLUE_OUT_OF_MEMORY:
    ফেরত "A memory function failed";

  ক্ষেত্র CURLUE_USER_NOT_ALLOWED:
    ফেরত "Credentials was passed in the URL when prohibited";

  ক্ষেত্র CURLUE_UNKNOWN_PART:
    ফেরত "An unknown part ID was passed to a URL API function";

  ক্ষেত্র CURLUE_NO_SCHEME:
    ফেরত "No scheme present";

  ক্ষেত্র CURLUE_NO_USER:
    ফেরত "No user present";

  ক্ষেত্র CURLUE_NO_PASSWORD:
    ফেরত "No password present";

  ক্ষেত্র CURLUE_NO_OPTIONS:
    ফেরত "No options present";

  ক্ষেত্র CURLUE_NO_HOST:
    ফেরত "No host present";

  ক্ষেত্র CURLUE_NO_PORT:
    ফেরত "No port number present";

  ক্ষেত্র CURLUE_NO_QUERY:
    ফেরত "No query present";

  ক্ষেত্র CURLUE_NO_FRAGMENT:
    ফেরত "No fragment present";

  ক্ষেত্র CURLUE_NO_ZONEID:
    ফেরত "No zoneid present";

  ক্ষেত্র CURLUE_BAD_LOGIN:
    ফেরত "Bad login";

  ক্ষেত্র CURLUE_BAD_IPV6:
    ফেরত "Bad IPv6 address";

  ক্ষেত্র CURLUE_BAD_HOSTNAME:
    ফেরত "Bad hostname";

  ক্ষেত্র CURLUE_BAD_FILE_URL:
    ফেরত "Bad file:// URL";

  ক্ষেত্র CURLUE_BAD_SLASHES:
    ফেরত "Unsupported number of slashes following scheme";

  ক্ষেত্র CURLUE_BAD_SCHEME:
    ফেরত "Bad scheme";

  ক্ষেত্র CURLUE_BAD_PATH:
    ফেরত "Bad path";

  ক্ষেত্র CURLUE_BAD_FRAGMENT:
    ফেরত "Bad fragment";

  ক্ষেত্র CURLUE_BAD_QUERY:
    ফেরত "Bad query";

  ক্ষেত্র CURLUE_BAD_PASSWORD:
    ফেরত "Bad password";

  ক্ষেত্র CURLUE_BAD_USER:
    ফেরত "Bad user";

  ক্ষেত্র CURLUE_LACKS_IDN:
    ফেরত "libcurl lacks IDN support";

  ক্ষেত্র CURLUE_TOO_LARGE:
    ফেরত "A value or data field is larger than allowed";

  ক্ষেত্র CURLUE_BACKSLASH:
    ফেরত "Found a backslash where a forward slash was expected";

  ক্ষেত্র CURLUE_LAST:
    বিরতি;
  }

  ফেরত "CURLUcode unknown";
#অন্যথা
  যদি(error == CURLUE_OK)
    ফেরত "No error";
  অন্যথা
    ফেরত "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ধ্রুবক অক্ষর *Curl_sspi_strerror(SECURITY_STATUS err, অক্ষর *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  পূর্ণসংখ্যা old_errno = errno;
  VERBOSE(ধ্রুবক অক্ষর *txt);

  যদি(!buflen)
    ফেরত ফাঁকা;

  *buf = '\0';

#ifdef CURLVERBOSE
  নির্বাচন(err) {
  ক্ষেত্র SEC_E_OK:
    txt = "No error";
    বিরতি;
#define SEC2TXT(sec) ক্ষেত্র sec: txt = #sec; বিরতি
  SEC2TXT(CRYPT_E_REVOKED);
  SEC2TXT(CRYPT_E_NO_REVOCATION_DLL);
  SEC2TXT(CRYPT_E_NO_REVOCATION_CHECK);
  SEC2TXT(CRYPT_E_REVOCATION_OFFLINE);
  SEC2TXT(CRYPT_E_NOT_IN_REVOCATION_DATABASE);
  SEC2TXT(SEC_E_ALGORITHM_MISMATCH);
  SEC2TXT(SEC_E_BAD_BINDINGS);
  SEC2TXT(SEC_E_BAD_PKGID);
  SEC2TXT(SEC_E_বাফার_TOO_SMALL);
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
  SEC2TXT(SEC_E_NO_PA_তথ্য);
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
  পূর্বনির্ধারিত:
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
    অক্ষর msgbuf[256];
    যদি(curlx_get_winapi_error((DWORD)err, msgbuf, আকার(msgbuf)))
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

  ফেরত buf;
}
#endif /* USE_WINDOWS_SSPI */
