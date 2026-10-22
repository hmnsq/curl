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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#അന്തര്ഭാവ "curl_sspi.h"
#endif

#അന്തര്ഭാവ "curlx/winapi.h"
#അന്തര്ഭാവ "strerror.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

സ്ഥിരം അക്ഷരം *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  തിരഞ്ഞെടുപ്പ്(error) {
  അവസ്ഥ CURLE_OK:
    തിരികെ "No error";

  അവസ്ഥ CURLE_UNSUPPORTED_PROTOCOL:
    തിരികെ "Unsupported protocol";

  അവസ്ഥ CURLE_FAILED_INIT:
    തിരികെ "Failed initialization";

  അവസ്ഥ CURLE_URL_MALFORMAT:
    തിരികെ "URL using bad/illegal format or missing URL";

  അവസ്ഥ CURLE_NOT_BUILT_IN:
    തിരികെ "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  അവസ്ഥ CURLE_COULDNT_RESOLVE_PROXY:
    തിരികെ "Could not resolve proxy name";

  അവസ്ഥ CURLE_COULDNT_RESOLVE_HOST:
    തിരികെ "Could not resolve hostname";

  അവസ്ഥ CURLE_COULDNT_CONNECT:
    തിരികെ "Could not connect to server";

  അവസ്ഥ CURLE_WEIRD_SERVER_REPLY:
    തിരികെ "Weird server reply";

  അവസ്ഥ CURLE_REMOTE_ACCESS_DENIED:
    തിരികെ "Access denied to remote resource";

  അവസ്ഥ CURLE_FTP_ACCEPT_FAILED:
    തിരികെ "FTP: The server failed to connect to data port";

  അവസ്ഥ CURLE_FTP_ACCEPT_TIMEOUT:
    തിരികെ "FTP: Accepting server connect has timed out";

  അവസ്ഥ CURLE_FTP_PRET_FAILED:
    തിരികെ "FTP: The server did not accept the PRET command.";

  അവസ്ഥ CURLE_FTP_WEIRD_PASS_REPLY:
    തിരികെ "FTP: unknown PASS reply";

  അവസ്ഥ CURLE_FTP_WEIRD_PASV_REPLY:
    തിരികെ "FTP: unknown PASV reply";

  അവസ്ഥ CURLE_FTP_WEIRD_227_FORMAT:
    തിരികെ "FTP: unknown 227 response format";

  അവസ്ഥ CURLE_FTP_CANT_GET_HOST:
    തിരികെ "FTP: cannot figure out the host in the PASV response";

  അവസ്ഥ CURLE_HTTP2:
    തിരികെ "Error in the HTTP2 framing layer";

  അവസ്ഥ CURLE_FTP_COULDNT_SET_TYPE:
    തിരികെ "FTP: could not set file type";

  അവസ്ഥ CURLE_PARTIAL_FILE:
    തിരികെ "Transferred a partial file";

  അവസ്ഥ CURLE_FTP_COULDNT_RETR_FILE:
    തിരികെ "FTP: could not retrieve (RETR failed) the specified file";

  അവസ്ഥ CURLE_QUOTE_ERROR:
    തിരികെ "Quote command returned error";

  അവസ്ഥ CURLE_HTTP_RETURNED_ERROR:
    തിരികെ "HTTP response code said error";

  അവസ്ഥ CURLE_WRITE_ERROR:
    തിരികെ "Failed writing received data to disk/application";

  അവസ്ഥ CURLE_UPLOAD_FAILED:
    തിരികെ "Upload failed (at start/before it took off)";

  അവസ്ഥ CURLE_READ_ERROR:
    തിരികെ "Failed to open/read local data from file/application";

  അവസ്ഥ CURLE_OUT_OF_MEMORY:
    തിരികെ "Out of memory";

  അവസ്ഥ CURLE_OPERATION_TIMEDOUT:
    തിരികെ "Timeout was reached";

  അവസ്ഥ CURLE_FTP_PORT_FAILED:
    തിരികെ "FTP: command PORT failed";

  അവസ്ഥ CURLE_FTP_COULDNT_USE_REST:
    തിരികെ "FTP: command REST failed";

  അവസ്ഥ CURLE_RANGE_ERROR:
    തിരികെ "Requested range was not delivered by the server";

  അവസ്ഥ CURLE_SSL_CONNECT_ERROR:
    തിരികെ "SSL connect error";

  അവസ്ഥ CURLE_BAD_DOWNLOAD_RESUME:
    തിരികെ "Could not resume download";

  അവസ്ഥ CURLE_FILE_COULDNT_READ_FILE:
    തിരികെ "Could not read a file:// file";

  അവസ്ഥ CURLE_LDAP_CANNOT_BIND:
    തിരികെ "LDAP: cannot bind";

  അവസ്ഥ CURLE_LDAP_തിരയൽ_FAILED:
    തിരികെ "LDAP: search failed";

  അവസ്ഥ CURLE_ABORTED_BY_CALLBACK:
    തിരികെ "Operation was aborted by an application callback";

  അവസ്ഥ CURLE_BAD_FUNCTION_ARGUMENT:
    തിരികെ "A libcurl function was given a bad argument";

  അവസ്ഥ CURLE_INTERFACE_FAILED:
    തിരികെ "Failed binding local connection end";

  അവസ്ഥ CURLE_TOO_MANY_REDIRECTS:
    തിരികെ "Number of redirects hit maximum amount";

  അവസ്ഥ CURLE_UNKNOWN_OPTION:
    തിരികെ "An unknown option was passed in to libcurl";

  അവസ്ഥ CURLE_SETOPT_OPTION_SYNTAX:
    തിരികെ "Malformed option provided in a setopt";

  അവസ്ഥ CURLE_GOT_NOTHING:
    തിരികെ "Server returned nothing (no headers, no data)";

  അവസ്ഥ CURLE_SSL_ENGINE_NOTFOUND:
    തിരികെ "SSL crypto engine not found";

  അവസ്ഥ CURLE_SSL_ENGINE_SETFAILED:
    തിരികെ "Can not set SSL crypto engine as default";

  അവസ്ഥ CURLE_SSL_ENGINE_INITFAILED:
    തിരികെ "Failed to initialize SSL crypto engine";

  അവസ്ഥ CURLE_SEND_ERROR:
    തിരികെ "Failed sending data to the peer";

  അവസ്ഥ CURLE_RECV_ERROR:
    തിരികെ "Failure when receiving data from the peer";

  അവസ്ഥ CURLE_SSL_CERTPROBLEM:
    തിരികെ "Problem with the local SSL certificate";

  അവസ്ഥ CURLE_SSL_CIPHER:
    തിരികെ "Could not use specified SSL cipher";

  അവസ്ഥ CURLE_PEER_FAILED_VERIFICATION:
    തിരികെ "SSL peer certificate or SSH remote key was not OK";

  അവസ്ഥ CURLE_SSL_CACERT_BADFILE:
    തിരികെ "Problem with the SSL CA cert (path? access rights?)";

  അവസ്ഥ CURLE_BAD_CONTENT_ENCODING:
    തിരികെ "Unrecognized or bad HTTP Content or Transfer-Encoding";

  അവസ്ഥ CURLE_FILESIZE_EXCEEDED:
    തിരികെ "Maximum file size exceeded";

  അവസ്ഥ CURLE_USE_SSL_FAILED:
    തിരികെ "Requested SSL level failed";

  അവസ്ഥ CURLE_SSL_SHUTDOWN_FAILED:
    തിരികെ "Failed to shut down the SSL connection";

  അവസ്ഥ CURLE_SSL_CRL_BADFILE:
    തിരികെ "Failed to load CRL file (path? access rights?, format?)";

  അവസ്ഥ CURLE_SSL_ISSUER_ERROR:
    തിരികെ "Issuer check against peer certificate failed";

  അവസ്ഥ CURLE_SEND_FAIL_REWIND:
    തിരികെ "Send failed since rewinding of the data stream failed";

  അവസ്ഥ CURLE_LOGIN_DENIED:
    തിരികെ "Login denied";

  അവസ്ഥ CURLE_TFTP_NOTFOUND:
    തിരികെ "TFTP: File Not Found";

  അവസ്ഥ CURLE_TFTP_PERM:
    തിരികെ "TFTP: Access Violation";

  അവസ്ഥ CURLE_REMOTE_DISK_FULL:
    തിരികെ "Disk full or allocation exceeded";

  അവസ്ഥ CURLE_TFTP_ILLEGAL:
    തിരികെ "TFTP: Illegal operation";

  അവസ്ഥ CURLE_TFTP_UNKNOWNID:
    തിരികെ "TFTP: Unknown transfer ID";

  അവസ്ഥ CURLE_REMOTE_FILE_EXISTS:
    തിരികെ "Remote file already exists";

  അവസ്ഥ CURLE_TFTP_NOSUCHUSER:
    തിരികെ "TFTP: No such user";

  അവസ്ഥ CURLE_REMOTE_FILE_NOT_FOUND:
    തിരികെ "Remote file not found";

  അവസ്ഥ CURLE_SSH:
    തിരികെ "Error in the SSH layer";

  അവസ്ഥ CURLE_AGAIN:
    തിരികെ "Socket not ready for send/recv";

  അവസ്ഥ CURLE_RTSP_CSEQ_ERROR:
    തിരികെ "RTSP CSeq mismatch or invalid CSeq";

  അവസ്ഥ CURLE_RTSP_SESSION_ERROR:
    തിരികെ "RTSP session error";

  അവസ്ഥ CURLE_FTP_BAD_FILE_LIST:
    തിരികെ "Unable to parse FTP file list";

  അവസ്ഥ CURLE_CHUNK_FAILED:
    തിരികെ "Chunk callback failed";

  അവസ്ഥ CURLE_NO_CONNECTION_AVAILABLE:
    തിരികെ "The max connection limit is reached";

  അവസ്ഥ CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    തിരികെ "SSL public key does not match pinned public key";

  അവസ്ഥ CURLE_SSL_INVALIDCERTSTATUS:
    തിരികെ "SSL server certificate status verification FAILED";

  അവസ്ഥ CURLE_HTTP2_STREAM:
    തിരികെ "Stream error in the HTTP/2 framing layer";

  അവസ്ഥ CURLE_RECURSIVE_API_CALL:
    തിരികെ "API function called from within callback";

  അവസ്ഥ CURLE_AUTH_ERROR:
    തിരികെ "An authentication function returned an error";

  അവസ്ഥ CURLE_HTTP3:
    തിരികെ "HTTP/3 error";

  അവസ്ഥ CURLE_QUIC_CONNECT_ERROR:
    തിരികെ "QUIC connection error";

  അവസ്ഥ CURLE_PROXY:
    തിരികെ "proxy handshake error";

  അവസ്ഥ CURLE_SSL_CLIENTCERT:
    തിരികെ "SSL Client Certificate required";

  അവസ്ഥ CURLE_UNRECOVERABLE_POLL:
    തിരികെ "Unrecoverable error in select/poll";

  അവസ്ഥ CURLE_TOO_LARGE:
    തിരികെ "A value or data field grew larger than allowed";

  അവസ്ഥ CURLE_ECH_REQUIRED:
    തിരികെ "ECH attempted but failed";

    /* error codes not used by current libcurl */
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
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
  തിരികെ "Unknown error";
#അല്ലെങ്കിൽ
  എങ്കിൽ(!error)
    തിരികെ "No error";
  അല്ലെങ്കിൽ
    തിരികെ "Error";
#endif
}

സ്ഥിരം അക്ഷരം *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  തിരഞ്ഞെടുപ്പ്(error) {
  അവസ്ഥ CURLM_CALL_MULTI_PERFORM:
    തിരികെ "Please call curl_multi_perform() soon";

  അവസ്ഥ CURLM_OK:
    തിരികെ "No error";

  അവസ്ഥ CURLM_BAD_HANDLE:
    തിരികെ "Invalid multi handle";

  അവസ്ഥ CURLM_BAD_EASY_HANDLE:
    തിരികെ "Invalid easy handle";

  അവസ്ഥ CURLM_OUT_OF_MEMORY:
    തിരികെ "Out of memory";

  അവസ്ഥ CURLM_INTERNAL_ERROR:
    തിരികെ "Internal error";

  അവസ്ഥ CURLM_BAD_SOCKET:
    തിരികെ "Invalid socket argument";

  അവസ്ഥ CURLM_UNKNOWN_OPTION:
    തിരികെ "Unknown option";

  അവസ്ഥ CURLM_ADDED_ALREADY:
    തിരികെ "The easy handle is already added to a multi handle";

  അവസ്ഥ CURLM_RECURSIVE_API_CALL:
    തിരികെ "API function called from within callback";

  അവസ്ഥ CURLM_WAKEUP_FAILURE:
    തിരികെ "Wakeup is unavailable or failed";

  അവസ്ഥ CURLM_BAD_FUNCTION_ARGUMENT:
    തിരികെ "A libcurl function was given a bad argument";

  അവസ്ഥ CURLM_ABORTED_BY_CALLBACK:
    തിരികെ "Operation was aborted by an application callback";

  അവസ്ഥ CURLM_UNRECOVERABLE_POLL:
    തിരികെ "Unrecoverable error in select/poll";

  അവസ്ഥ CURLM_LAST:
    നിർത്തുക;
  }

  തിരികെ "Unknown error";
#അല്ലെങ്കിൽ
  എങ്കിൽ(error == CURLM_OK)
    തിരികെ "No error";
  അല്ലെങ്കിൽ
    തിരികെ "Error";
#endif
}

സ്ഥിരം അക്ഷരം *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  തിരഞ്ഞെടുപ്പ്(error) {
  അവസ്ഥ CURLSHE_OK:
    തിരികെ "No error";

  അവസ്ഥ CURLSHE_BAD_OPTION:
    തിരികെ "Unknown share option";

  അവസ്ഥ CURLSHE_IN_USE:
    തിരികെ "Share currently in use";

  അവസ്ഥ CURLSHE_INVALID:
    തിരികെ "Invalid share handle";

  അവസ്ഥ CURLSHE_NOMEM:
    തിരികെ "Out of memory";

  അവസ്ഥ CURLSHE_NOT_BUILT_IN:
    തിരികെ "Feature not enabled in this library";

  അവസ്ഥ CURLSHE_LAST:
    നിർത്തുക;
  }

  തിരികെ "CURLSHcode unknown";
#അല്ലെങ്കിൽ
  എങ്കിൽ(error == CURLSHE_OK)
    തിരികെ "No error";
  അല്ലെങ്കിൽ
    തിരികെ "Error";
#endif
}

സ്ഥിരം അക്ഷരം *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  തിരഞ്ഞെടുപ്പ്(error) {
  അവസ്ഥ CURLUE_OK:
    തിരികെ "No error";

  അവസ്ഥ CURLUE_BAD_HANDLE:
    തിരികെ "An invalid CURLU pointer was passed as argument";

  അവസ്ഥ CURLUE_BAD_PARTPOINTER:
    തിരികെ "An invalid 'part' argument was passed as argument";

  അവസ്ഥ CURLUE_MALFORMED_നിവേശം:
    തിരികെ "Malformed input to a URL function";

  അവസ്ഥ CURLUE_BAD_PORT_സംഖ്യ:
    തിരികെ "Port number was not a decimal number between 0 and 65535";

  അവസ്ഥ CURLUE_UNSUPPORTED_SCHEME:
    തിരികെ "Unsupported URL scheme";

  അവസ്ഥ CURLUE_URLDECODE:
    തിരികെ "URL decode error, most likely because of rubbish in the input";

  അവസ്ഥ CURLUE_OUT_OF_MEMORY:
    തിരികെ "A memory function failed";

  അവസ്ഥ CURLUE_USER_NOT_ALLOWED:
    തിരികെ "Credentials was passed in the URL when prohibited";

  അവസ്ഥ CURLUE_UNKNOWN_PART:
    തിരികെ "An unknown part ID was passed to a URL API function";

  അവസ്ഥ CURLUE_NO_SCHEME:
    തിരികെ "No scheme present";

  അവസ്ഥ CURLUE_NO_USER:
    തിരികെ "No user present";

  അവസ്ഥ CURLUE_NO_PASSWORD:
    തിരികെ "No password present";

  അവസ്ഥ CURLUE_NO_OPTIONS:
    തിരികെ "No options present";

  അവസ്ഥ CURLUE_NO_HOST:
    തിരികെ "No host present";

  അവസ്ഥ CURLUE_NO_PORT:
    തിരികെ "No port number present";

  അവസ്ഥ CURLUE_NO_QUERY:
    തിരികെ "No query present";

  അവസ്ഥ CURLUE_NO_FRAGMENT:
    തിരികെ "No fragment present";

  അവസ്ഥ CURLUE_NO_ZONEID:
    തിരികെ "No zoneid present";

  അവസ്ഥ CURLUE_BAD_LOGIN:
    തിരികെ "Bad login";

  അവസ്ഥ CURLUE_BAD_IPV6:
    തിരികെ "Bad IPv6 address";

  അവസ്ഥ CURLUE_BAD_HOSTNAME:
    തിരികെ "Bad hostname";

  അവസ്ഥ CURLUE_BAD_FILE_URL:
    തിരികെ "Bad file:// URL";

  അവസ്ഥ CURLUE_BAD_SLASHES:
    തിരികെ "Unsupported number of slashes following scheme";

  അവസ്ഥ CURLUE_BAD_SCHEME:
    തിരികെ "Bad scheme";

  അവസ്ഥ CURLUE_BAD_PATH:
    തിരികെ "Bad path";

  അവസ്ഥ CURLUE_BAD_FRAGMENT:
    തിരികെ "Bad fragment";

  അവസ്ഥ CURLUE_BAD_QUERY:
    തിരികെ "Bad query";

  അവസ്ഥ CURLUE_BAD_PASSWORD:
    തിരികെ "Bad password";

  അവസ്ഥ CURLUE_BAD_USER:
    തിരികെ "Bad user";

  അവസ്ഥ CURLUE_LACKS_IDN:
    തിരികെ "libcurl lacks IDN support";

  അവസ്ഥ CURLUE_TOO_LARGE:
    തിരികെ "A value or data field is larger than allowed";

  അവസ്ഥ CURLUE_BACKSLASH:
    തിരികെ "Found a backslash where a forward slash was expected";

  അവസ്ഥ CURLUE_LAST:
    നിർത്തുക;
  }

  തിരികെ "CURLUcode unknown";
#അല്ലെങ്കിൽ
  എങ്കിൽ(error == CURLUE_OK)
    തിരികെ "No error";
  അല്ലെങ്കിൽ
    തിരികെ "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
സ്ഥിരം അക്ഷരം *Curl_sspi_strerror(SECURITY_STATUS err, അക്ഷരം *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  പൂർണ്ണസംഖ്യ old_errno = errno;
  VERBOSE(സ്ഥിരം അക്ഷരം *txt);

  എങ്കിൽ(!buflen)
    തിരികെ ശൂന്യ;

  *buf = '\0';

#ifdef CURLVERBOSE
  തിരഞ്ഞെടുപ്പ്(err) {
  അവസ്ഥ SEC_E_OK:
    txt = "No error";
    നിർത്തുക;
#define SEC2TXT(sec) അവസ്ഥ sec: txt = #sec; നിർത്തുക
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
  SEC2TXT(SEC_E_NO_KERB_താക്കോൽ);
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
  SEC2TXT(SEC_E_UNTRUSTED_വേര്);
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
  സ്ഥിരസ്ഥിതി:
    txt = "Unknown error";
  }

  എങ്കിൽ(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  അല്ലെങ്കിൽ {
    അക്ഷരം msgbuf[256];
    എങ്കിൽ(curlx_get_winapi_error((DWORD)err, msgbuf, വലുപ്പം(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    അല്ലെങ്കിൽ
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#അല്ലെങ്കിൽ /* CURLVERBOSE */
  എങ്കിൽ(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  അല്ലെങ്കിൽ
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  എങ്കിൽ(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  എങ്കിൽ(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  തിരികെ buf;
}
#endif /* USE_WINDOWS_SSPI */
