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

अचर वर्ण *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLE_OK:
    फर्काऊ "No error";

  पक्ष CURLE_UNSUPPORTED_PROTOCOL:
    फर्काऊ "Unsupported protocol";

  पक्ष CURLE_FAILED_INIT:
    फर्काऊ "Failed initialization";

  पक्ष CURLE_URL_MALFORMAT:
    फर्काऊ "URL using bad/illegal format or missing URL";

  पक्ष CURLE_NOT_BUILT_IN:
    फर्काऊ "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  पक्ष CURLE_COULDNT_RESOLVE_PROXY:
    फर्काऊ "Could not resolve proxy name";

  पक्ष CURLE_COULDNT_RESOLVE_HOST:
    फर्काऊ "Could not resolve hostname";

  पक्ष CURLE_COULDNT_CONNECT:
    फर्काऊ "Could not connect to server";

  पक्ष CURLE_WEIRD_SERVER_REPLY:
    फर्काऊ "Weird server reply";

  पक्ष CURLE_REMOTE_ACCESS_DENIED:
    फर्काऊ "Access denied to remote resource";

  पक्ष CURLE_FTP_ACCEPT_FAILED:
    फर्काऊ "FTP: The server failed to connect to data port";

  पक्ष CURLE_FTP_ACCEPT_TIMEOUT:
    फर्काऊ "FTP: Accepting server connect has timed out";

  पक्ष CURLE_FTP_PRET_FAILED:
    फर्काऊ "FTP: The server did not accept the PRET command.";

  पक्ष CURLE_FTP_WEIRD_PASS_REPLY:
    फर्काऊ "FTP: unknown PASS reply";

  पक्ष CURLE_FTP_WEIRD_PASV_REPLY:
    फर्काऊ "FTP: unknown PASV reply";

  पक्ष CURLE_FTP_WEIRD_227_FORMAT:
    फर्काऊ "FTP: unknown 227 response format";

  पक्ष CURLE_FTP_CANT_GET_HOST:
    फर्काऊ "FTP: cannot figure out the host in the PASV response";

  पक्ष CURLE_HTTP2:
    फर्काऊ "Error in the HTTP2 framing layer";

  पक्ष CURLE_FTP_COULDNT_SET_TYPE:
    फर्काऊ "FTP: could not set file type";

  पक्ष CURLE_PARTIAL_FILE:
    फर्काऊ "Transferred a partial file";

  पक्ष CURLE_FTP_COULDNT_RETR_FILE:
    फर्काऊ "FTP: could not retrieve (RETR failed) the specified file";

  पक्ष CURLE_QUOTE_ERROR:
    फर्काऊ "Quote command returned error";

  पक्ष CURLE_HTTP_RETURNED_ERROR:
    फर्काऊ "HTTP response code said error";

  पक्ष CURLE_WRITE_ERROR:
    फर्काऊ "Failed writing received data to disk/application";

  पक्ष CURLE_UPLOAD_FAILED:
    फर्काऊ "Upload failed (at start/before it took off)";

  पक्ष CURLE_READ_ERROR:
    फर्काऊ "Failed to open/read local data from file/application";

  पक्ष CURLE_OUT_OF_MEMORY:
    फर्काऊ "Out of memory";

  पक्ष CURLE_OPERATION_TIMEDOUT:
    फर्काऊ "Timeout was reached";

  पक्ष CURLE_FTP_PORT_FAILED:
    फर्काऊ "FTP: command PORT failed";

  पक्ष CURLE_FTP_COULDNT_USE_REST:
    फर्काऊ "FTP: command REST failed";

  पक्ष CURLE_RANGE_ERROR:
    फर्काऊ "Requested range was not delivered by the server";

  पक्ष CURLE_SSL_CONNECT_ERROR:
    फर्काऊ "SSL connect error";

  पक्ष CURLE_BAD_DOWNLOAD_RESUME:
    फर्काऊ "Could not resume download";

  पक्ष CURLE_FILE_COULDNT_READ_FILE:
    फर्काऊ "Could not read a file:// file";

  पक्ष CURLE_LDAP_CANNOT_BIND:
    फर्काऊ "LDAP: cannot bind";

  पक्ष CURLE_LDAP_खोज_FAILED:
    फर्काऊ "LDAP: search failed";

  पक्ष CURLE_ABORTED_BY_CALLBACK:
    फर्काऊ "Operation was aborted by an application callback";

  पक्ष CURLE_BAD_FUNCTION_ARGUMENT:
    फर्काऊ "A libcurl function was given a bad argument";

  पक्ष CURLE_INTERFACE_FAILED:
    फर्काऊ "Failed binding local connection end";

  पक्ष CURLE_TOO_MANY_REDIRECTS:
    फर्काऊ "Number of redirects hit maximum amount";

  पक्ष CURLE_UNKNOWN_OPTION:
    फर्काऊ "An unknown option was passed in to libcurl";

  पक्ष CURLE_SETOPT_OPTION_SYNTAX:
    फर्काऊ "Malformed option provided in a setopt";

  पक्ष CURLE_GOT_NOTHING:
    फर्काऊ "Server returned nothing (no headers, no data)";

  पक्ष CURLE_SSL_ENGINE_NOTFOUND:
    फर्काऊ "SSL crypto engine not found";

  पक्ष CURLE_SSL_ENGINE_SETFAILED:
    फर्काऊ "Can not set SSL crypto engine as default";

  पक्ष CURLE_SSL_ENGINE_INITFAILED:
    फर्काऊ "Failed to initialize SSL crypto engine";

  पक्ष CURLE_SEND_ERROR:
    फर्काऊ "Failed sending data to the peer";

  पक्ष CURLE_RECV_ERROR:
    फर्काऊ "Failure when receiving data from the peer";

  पक्ष CURLE_SSL_CERTPROBLEM:
    फर्काऊ "Problem with the local SSL certificate";

  पक्ष CURLE_SSL_CIPHER:
    फर्काऊ "Could not use specified SSL cipher";

  पक्ष CURLE_PEER_FAILED_VERIFICATION:
    फर्काऊ "SSL peer certificate or SSH remote key was not OK";

  पक्ष CURLE_SSL_CACERT_BADFILE:
    फर्काऊ "Problem with the SSL CA cert (path? access rights?)";

  पक्ष CURLE_BAD_CONTENT_ENCODING:
    फर्काऊ "Unrecognized or bad HTTP Content or Transfer-Encoding";

  पक्ष CURLE_FILESIZE_EXCEEDED:
    फर्काऊ "Maximum file size exceeded";

  पक्ष CURLE_USE_SSL_FAILED:
    फर्काऊ "Requested SSL level failed";

  पक्ष CURLE_SSL_SHUTDOWN_FAILED:
    फर्काऊ "Failed to shut down the SSL connection";

  पक्ष CURLE_SSL_CRL_BADFILE:
    फर्काऊ "Failed to load CRL file (path? access rights?, format?)";

  पक्ष CURLE_SSL_ISSUER_ERROR:
    फर्काऊ "Issuer check against peer certificate failed";

  पक्ष CURLE_SEND_FAIL_REWIND:
    फर्काऊ "Send failed since rewinding of the data stream failed";

  पक्ष CURLE_LOGIN_DENIED:
    फर्काऊ "Login denied";

  पक्ष CURLE_TFTP_NOTFOUND:
    फर्काऊ "TFTP: File Not Found";

  पक्ष CURLE_TFTP_PERM:
    फर्काऊ "TFTP: Access Violation";

  पक्ष CURLE_REMOTE_DISK_FULL:
    फर्काऊ "Disk full or allocation exceeded";

  पक्ष CURLE_TFTP_ILLEGAL:
    फर्काऊ "TFTP: Illegal operation";

  पक्ष CURLE_TFTP_UNKNOWNID:
    फर्काऊ "TFTP: Unknown transfer ID";

  पक्ष CURLE_REMOTE_FILE_EXISTS:
    फर्काऊ "Remote file already exists";

  पक्ष CURLE_TFTP_NOSUCHUSER:
    फर्काऊ "TFTP: No such user";

  पक्ष CURLE_REMOTE_FILE_NOT_FOUND:
    फर्काऊ "Remote file not found";

  पक्ष CURLE_SSH:
    फर्काऊ "Error in the SSH layer";

  पक्ष CURLE_AGAIN:
    फर्काऊ "Socket not ready for send/recv";

  पक्ष CURLE_RTSP_CSEQ_ERROR:
    फर्काऊ "RTSP CSeq mismatch or invalid CSeq";

  पक्ष CURLE_RTSP_SESSION_ERROR:
    फर्काऊ "RTSP session error";

  पक्ष CURLE_FTP_BAD_FILE_LIST:
    फर्काऊ "Unable to parse FTP file list";

  पक्ष CURLE_CHUNK_FAILED:
    फर्काऊ "Chunk callback failed";

  पक्ष CURLE_NO_CONNECTION_AVAILABLE:
    फर्काऊ "The max connection limit is reached";

  पक्ष CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    फर्काऊ "SSL public key does not match pinned public key";

  पक्ष CURLE_SSL_INVALIDCERTSTATUS:
    फर्काऊ "SSL server certificate status verification FAILED";

  पक्ष CURLE_HTTP2_STREAM:
    फर्काऊ "Stream error in the HTTP/2 framing layer";

  पक्ष CURLE_RECURSIVE_API_CALL:
    फर्काऊ "API function called from within callback";

  पक्ष CURLE_AUTH_ERROR:
    फर्काऊ "An authentication function returned an error";

  पक्ष CURLE_HTTP3:
    फर्काऊ "HTTP/3 error";

  पक्ष CURLE_QUIC_CONNECT_ERROR:
    फर्काऊ "QUIC connection error";

  पक्ष CURLE_PROXY:
    फर्काऊ "proxy handshake error";

  पक्ष CURLE_SSL_CLIENTCERT:
    फर्काऊ "SSL Client Certificate required";

  पक्ष CURLE_UNRECOVERABLE_POLL:
    फर्काऊ "Unrecoverable error in select/poll";

  पक्ष CURLE_TOO_LARGE:
    फर्काऊ "A value or data field grew larger than allowed";

  पक्ष CURLE_ECH_REQUIRED:
    फर्काऊ "ECH attempted but failed";

    /* error codes not used by current libcurl */
  स्वभाव:
    रोक;
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
  फर्काऊ "Unknown error";
#अन्यथा
  यदि(!error)
    फर्काऊ "No error";
  अन्यथा
    फर्काऊ "Error";
#endif
}

अचर वर्ण *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLM_CALL_MULTI_PERFORM:
    फर्काऊ "Please call curl_multi_perform() soon";

  पक्ष CURLM_OK:
    फर्काऊ "No error";

  पक्ष CURLM_BAD_HANDLE:
    फर्काऊ "Invalid multi handle";

  पक्ष CURLM_BAD_EASY_HANDLE:
    फर्काऊ "Invalid easy handle";

  पक्ष CURLM_OUT_OF_MEMORY:
    फर्काऊ "Out of memory";

  पक्ष CURLM_INTERNAL_ERROR:
    फर्काऊ "Internal error";

  पक्ष CURLM_BAD_SOCKET:
    फर्काऊ "Invalid socket argument";

  पक्ष CURLM_UNKNOWN_OPTION:
    फर्काऊ "Unknown option";

  पक्ष CURLM_ADDED_ALREADY:
    फर्काऊ "The easy handle is already added to a multi handle";

  पक्ष CURLM_RECURSIVE_API_CALL:
    फर्काऊ "API function called from within callback";

  पक्ष CURLM_WAKEUP_FAILURE:
    फर्काऊ "Wakeup is unavailable or failed";

  पक्ष CURLM_BAD_FUNCTION_ARGUMENT:
    फर्काऊ "A libcurl function was given a bad argument";

  पक्ष CURLM_ABORTED_BY_CALLBACK:
    फर्काऊ "Operation was aborted by an application callback";

  पक्ष CURLM_UNRECOVERABLE_POLL:
    फर्काऊ "Unrecoverable error in select/poll";

  पक्ष CURLM_LAST:
    रोक;
  }

  फर्काऊ "Unknown error";
#अन्यथा
  यदि(error == CURLM_OK)
    फर्काऊ "No error";
  अन्यथा
    फर्काऊ "Error";
#endif
}

अचर वर्ण *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLSHE_OK:
    फर्काऊ "No error";

  पक्ष CURLSHE_BAD_OPTION:
    फर्काऊ "Unknown share option";

  पक्ष CURLSHE_IN_USE:
    फर्काऊ "Share currently in use";

  पक्ष CURLSHE_INVALID:
    फर्काऊ "Invalid share handle";

  पक्ष CURLSHE_NOMEM:
    फर्काऊ "Out of memory";

  पक्ष CURLSHE_NOT_BUILT_IN:
    फर्काऊ "Feature not enabled in this library";

  पक्ष CURLSHE_LAST:
    रोक;
  }

  फर्काऊ "CURLSHcode unknown";
#अन्यथा
  यदि(error == CURLSHE_OK)
    फर्काऊ "No error";
  अन्यथा
    फर्काऊ "Error";
#endif
}

अचर वर्ण *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  विकल्प(error) {
  पक्ष CURLUE_OK:
    फर्काऊ "No error";

  पक्ष CURLUE_BAD_HANDLE:
    फर्काऊ "An invalid CURLU pointer was passed as argument";

  पक्ष CURLUE_BAD_PARTPOINTER:
    फर्काऊ "An invalid 'part' argument was passed as argument";

  पक्ष CURLUE_MALFORMED_आगत:
    फर्काऊ "Malformed input to a URL function";

  पक्ष CURLUE_BAD_PORT_सङ्ख्या:
    फर्काऊ "Port number was not a decimal number between 0 and 65535";

  पक्ष CURLUE_UNSUPPORTED_SCHEME:
    फर्काऊ "Unsupported URL scheme";

  पक्ष CURLUE_URLDECODE:
    फर्काऊ "URL decode error, most likely because of rubbish in the input";

  पक्ष CURLUE_OUT_OF_MEMORY:
    फर्काऊ "A memory function failed";

  पक्ष CURLUE_USER_NOT_ALLOWED:
    फर्काऊ "Credentials was passed in the URL when prohibited";

  पक्ष CURLUE_UNKNOWN_PART:
    फर्काऊ "An unknown part ID was passed to a URL API function";

  पक्ष CURLUE_NO_SCHEME:
    फर्काऊ "No scheme present";

  पक्ष CURLUE_NO_USER:
    फर्काऊ "No user present";

  पक्ष CURLUE_NO_PASSWORD:
    फर्काऊ "No password present";

  पक्ष CURLUE_NO_OPTIONS:
    फर्काऊ "No options present";

  पक्ष CURLUE_NO_HOST:
    फर्काऊ "No host present";

  पक्ष CURLUE_NO_PORT:
    फर्काऊ "No port number present";

  पक्ष CURLUE_NO_QUERY:
    फर्काऊ "No query present";

  पक्ष CURLUE_NO_FRAGMENT:
    फर्काऊ "No fragment present";

  पक्ष CURLUE_NO_ZONEID:
    फर्काऊ "No zoneid present";

  पक्ष CURLUE_BAD_LOGIN:
    फर्काऊ "Bad login";

  पक्ष CURLUE_BAD_IPV6:
    फर्काऊ "Bad IPv6 address";

  पक्ष CURLUE_BAD_HOSTNAME:
    फर्काऊ "Bad hostname";

  पक्ष CURLUE_BAD_FILE_URL:
    फर्काऊ "Bad file:// URL";

  पक्ष CURLUE_BAD_SLASHES:
    फर्काऊ "Unsupported number of slashes following scheme";

  पक्ष CURLUE_BAD_SCHEME:
    फर्काऊ "Bad scheme";

  पक्ष CURLUE_BAD_PATH:
    फर्काऊ "Bad path";

  पक्ष CURLUE_BAD_FRAGMENT:
    फर्काऊ "Bad fragment";

  पक्ष CURLUE_BAD_QUERY:
    फर्काऊ "Bad query";

  पक्ष CURLUE_BAD_PASSWORD:
    फर्काऊ "Bad password";

  पक्ष CURLUE_BAD_USER:
    फर्काऊ "Bad user";

  पक्ष CURLUE_LACKS_IDN:
    फर्काऊ "libcurl lacks IDN support";

  पक्ष CURLUE_TOO_LARGE:
    फर्काऊ "A value or data field is larger than allowed";

  पक्ष CURLUE_BACKSLASH:
    फर्काऊ "Found a backslash where a forward slash was expected";

  पक्ष CURLUE_LAST:
    रोक;
  }

  फर्काऊ "CURLUcode unknown";
#अन्यथा
  यदि(error == CURLUE_OK)
    फर्काऊ "No error";
  अन्यथा
    फर्काऊ "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
अचर वर्ण *Curl_sspi_strerror(SECURITY_STATUS err, वर्ण *buf, माप_प्रकार buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  पूर्णाङ्क old_errno = त्रुटि_सं;
  VERBOSE(अचर वर्ण *txt);

  यदि(!buflen)
    फर्काऊ रिक्त;

  *buf = '\0';

#ifdef CURLVERBOSE
  विकल्प(err) {
  पक्ष SEC_E_OK:
    txt = "No error";
    रोक;
#define SEC2TXT(sec) पक्ष sec: txt = #sec; रोक
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
  SEC2TXT(SEC_E_NO_KERB_साँचो);
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
  SEC2TXT(SEC_E_UNTRUSTED_जरा);
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

  यदि(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (अचिन्हित दीर्घ)err);
  }
  अन्यथा {
    वर्ण msgbuf[256];
    यदि(curlx_get_winapi_error((DWORD)err, msgbuf, परिमाण(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (अचिन्हित दीर्घ)err,
                     msgbuf);
    अन्यथा
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (अचिन्हित दीर्घ)err);
  }
#अन्यथा /* CURLVERBOSE */
  यदि(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  अन्यथा
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  यदि(त्रुटि_सं != old_errno)
    त्रुटि_सं = old_errno;

#ifdef _WIN32
  यदि(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  फर्काऊ buf;
}
#endif /* USE_WINDOWS_SSPI */
