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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#அந்தர்பாவ "curl_sspi.h"
#endif

#அந்தர்பாவ "curlx/winapi.h"
#அந்தர்பாவ "strerror.h"
#அந்தர்பாவ "curlx/strcopy.h"

மாறிலி எழுத்து *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  தேர்வு(error) {
  நிலை CURLE_OK:
    திரும்பு "No error";

  நிலை CURLE_UNSUPPORTED_PROTOCOL:
    திரும்பு "Unsupported protocol";

  நிலை CURLE_FAILED_INIT:
    திரும்பு "Failed initialization";

  நிலை CURLE_URL_MALFORMAT:
    திரும்பு "URL using bad/illegal format or missing URL";

  நிலை CURLE_NOT_BUILT_IN:
    திரும்பு "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  நிலை CURLE_COULDNT_RESOLVE_PROXY:
    திரும்பு "Could not resolve proxy name";

  நிலை CURLE_COULDNT_RESOLVE_HOST:
    திரும்பு "Could not resolve hostname";

  நிலை CURLE_COULDNT_CONNECT:
    திரும்பு "Could not connect to server";

  நிலை CURLE_WEIRD_SERVER_REPLY:
    திரும்பு "Weird server reply";

  நிலை CURLE_REMOTE_ACCESS_DENIED:
    திரும்பு "Access denied to remote resource";

  நிலை CURLE_FTP_ACCEPT_FAILED:
    திரும்பு "FTP: The server failed to connect to data port";

  நிலை CURLE_FTP_ACCEPT_TIMEOUT:
    திரும்பு "FTP: Accepting server connect has timed out";

  நிலை CURLE_FTP_PRET_FAILED:
    திரும்பு "FTP: The server did not accept the PRET command.";

  நிலை CURLE_FTP_WEIRD_PASS_REPLY:
    திரும்பு "FTP: unknown PASS reply";

  நிலை CURLE_FTP_WEIRD_PASV_REPLY:
    திரும்பு "FTP: unknown PASV reply";

  நிலை CURLE_FTP_WEIRD_227_FORMAT:
    திரும்பு "FTP: unknown 227 response format";

  நிலை CURLE_FTP_CANT_GET_HOST:
    திரும்பு "FTP: cannot figure out the host in the PASV response";

  நிலை CURLE_HTTP2:
    திரும்பு "Error in the HTTP2 framing layer";

  நிலை CURLE_FTP_COULDNT_SET_TYPE:
    திரும்பு "FTP: could not set file type";

  நிலை CURLE_PARTIAL_FILE:
    திரும்பு "Transferred a partial file";

  நிலை CURLE_FTP_COULDNT_RETR_FILE:
    திரும்பு "FTP: could not retrieve (RETR failed) the specified file";

  நிலை CURLE_QUOTE_ERROR:
    திரும்பு "Quote command returned error";

  நிலை CURLE_HTTP_RETURNED_ERROR:
    திரும்பு "HTTP response code said error";

  நிலை CURLE_WRITE_ERROR:
    திரும்பு "Failed writing received data to disk/application";

  நிலை CURLE_UPLOAD_FAILED:
    திரும்பு "Upload failed (at start/before it took off)";

  நிலை CURLE_READ_ERROR:
    திரும்பு "Failed to open/read local data from file/application";

  நிலை CURLE_OUT_OF_MEMORY:
    திரும்பு "Out of memory";

  நிலை CURLE_OPERATION_TIMEDOUT:
    திரும்பு "Timeout was reached";

  நிலை CURLE_FTP_PORT_FAILED:
    திரும்பு "FTP: command PORT failed";

  நிலை CURLE_FTP_COULDNT_USE_REST:
    திரும்பு "FTP: command REST failed";

  நிலை CURLE_RANGE_ERROR:
    திரும்பு "Requested range was not delivered by the server";

  நிலை CURLE_SSL_CONNECT_ERROR:
    திரும்பு "SSL connect error";

  நிலை CURLE_BAD_DOWNLOAD_RESUME:
    திரும்பு "Could not resume download";

  நிலை CURLE_FILE_COULDNT_READ_FILE:
    திரும்பு "Could not read a file:// file";

  நிலை CURLE_LDAP_CANNOT_BIND:
    திரும்பு "LDAP: cannot bind";

  நிலை CURLE_LDAP_தேடல்_FAILED:
    திரும்பு "LDAP: search failed";

  நிலை CURLE_ABORTED_BY_CALLBACK:
    திரும்பு "Operation was aborted by an application callback";

  நிலை CURLE_BAD_FUNCTION_ARGUMENT:
    திரும்பு "A libcurl function was given a bad argument";

  நிலை CURLE_INTERFACE_FAILED:
    திரும்பு "Failed binding local connection end";

  நிலை CURLE_TOO_MANY_REDIRECTS:
    திரும்பு "Number of redirects hit maximum amount";

  நிலை CURLE_UNKNOWN_OPTION:
    திரும்பு "An unknown option was passed in to libcurl";

  நிலை CURLE_SETOPT_OPTION_SYNTAX:
    திரும்பு "Malformed option provided in a setopt";

  நிலை CURLE_GOT_NOTHING:
    திரும்பு "Server returned nothing (no headers, no data)";

  நிலை CURLE_SSL_ENGINE_NOTFOUND:
    திரும்பு "SSL crypto engine not found";

  நிலை CURLE_SSL_ENGINE_SETFAILED:
    திரும்பு "Can not set SSL crypto engine as default";

  நிலை CURLE_SSL_ENGINE_INITFAILED:
    திரும்பு "Failed to initialize SSL crypto engine";

  நிலை CURLE_SEND_ERROR:
    திரும்பு "Failed sending data to the peer";

  நிலை CURLE_RECV_ERROR:
    திரும்பு "Failure when receiving data from the peer";

  நிலை CURLE_SSL_CERTPROBLEM:
    திரும்பு "Problem with the local SSL certificate";

  நிலை CURLE_SSL_CIPHER:
    திரும்பு "Could not use specified SSL cipher";

  நிலை CURLE_PEER_FAILED_VERIFICATION:
    திரும்பு "SSL peer certificate or SSH remote key was not OK";

  நிலை CURLE_SSL_CACERT_BADFILE:
    திரும்பு "Problem with the SSL CA cert (path? access rights?)";

  நிலை CURLE_BAD_CONTENT_ENCODING:
    திரும்பு "Unrecognized or bad HTTP Content or Transfer-Encoding";

  நிலை CURLE_FILESIZE_EXCEEDED:
    திரும்பு "Maximum file size exceeded";

  நிலை CURLE_USE_SSL_FAILED:
    திரும்பு "Requested SSL level failed";

  நிலை CURLE_SSL_SHUTDOWN_FAILED:
    திரும்பு "Failed to shut down the SSL connection";

  நிலை CURLE_SSL_CRL_BADFILE:
    திரும்பு "Failed to load CRL file (path? access rights?, format?)";

  நிலை CURLE_SSL_ISSUER_ERROR:
    திரும்பு "Issuer check against peer certificate failed";

  நிலை CURLE_SEND_FAIL_REWIND:
    திரும்பு "Send failed since rewinding of the data stream failed";

  நிலை CURLE_LOGIN_DENIED:
    திரும்பு "Login denied";

  நிலை CURLE_TFTP_NOTFOUND:
    திரும்பு "TFTP: File Not Found";

  நிலை CURLE_TFTP_PERM:
    திரும்பு "TFTP: Access Violation";

  நிலை CURLE_REMOTE_DISK_FULL:
    திரும்பு "Disk full or allocation exceeded";

  நிலை CURLE_TFTP_ILLEGAL:
    திரும்பு "TFTP: Illegal operation";

  நிலை CURLE_TFTP_UNKNOWNID:
    திரும்பு "TFTP: Unknown transfer ID";

  நிலை CURLE_REMOTE_FILE_EXISTS:
    திரும்பு "Remote file already exists";

  நிலை CURLE_TFTP_NOSUCHUSER:
    திரும்பு "TFTP: No such user";

  நிலை CURLE_REMOTE_FILE_NOT_FOUND:
    திரும்பு "Remote file not found";

  நிலை CURLE_SSH:
    திரும்பு "Error in the SSH layer";

  நிலை CURLE_AGAIN:
    திரும்பு "Socket not ready for send/recv";

  நிலை CURLE_RTSP_CSEQ_ERROR:
    திரும்பு "RTSP CSeq mismatch or invalid CSeq";

  நிலை CURLE_RTSP_SESSION_ERROR:
    திரும்பு "RTSP session error";

  நிலை CURLE_FTP_BAD_FILE_LIST:
    திரும்பு "Unable to parse FTP file list";

  நிலை CURLE_CHUNK_FAILED:
    திரும்பு "Chunk callback failed";

  நிலை CURLE_NO_CONNECTION_AVAILABLE:
    திரும்பு "The max connection limit is reached";

  நிலை CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    திரும்பு "SSL public key does not match pinned public key";

  நிலை CURLE_SSL_INVALIDCERTSTATUS:
    திரும்பு "SSL server certificate status verification FAILED";

  நிலை CURLE_HTTP2_STREAM:
    திரும்பு "Stream error in the HTTP/2 framing layer";

  நிலை CURLE_RECURSIVE_API_CALL:
    திரும்பு "API function called from within callback";

  நிலை CURLE_AUTH_ERROR:
    திரும்பு "An authentication function returned an error";

  நிலை CURLE_HTTP3:
    திரும்பு "HTTP/3 error";

  நிலை CURLE_QUIC_CONNECT_ERROR:
    திரும்பு "QUIC connection error";

  நிலை CURLE_PROXY:
    திரும்பு "proxy handshake error";

  நிலை CURLE_SSL_CLIENTCERT:
    திரும்பு "SSL Client Certificate required";

  நிலை CURLE_UNRECOVERABLE_POLL:
    திரும்பு "Unrecoverable error in select/poll";

  நிலை CURLE_TOO_LARGE:
    திரும்பு "A value or data field grew larger than allowed";

  நிலை CURLE_ECH_REQUIRED:
    திரும்பு "ECH attempted but failed";

    /* error codes not used by current libcurl */
  இயல்பு:
    நிறுத்து;
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
  திரும்பு "Unknown error";
#இல்லையெனில்
  எனில்(!error)
    திரும்பு "No error";
  இல்லையெனில்
    திரும்பு "Error";
#endif
}

மாறிலி எழுத்து *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  தேர்வு(error) {
  நிலை CURLM_CALL_MULTI_PERFORM:
    திரும்பு "Please call curl_multi_perform() soon";

  நிலை CURLM_OK:
    திரும்பு "No error";

  நிலை CURLM_BAD_HANDLE:
    திரும்பு "Invalid multi handle";

  நிலை CURLM_BAD_EASY_HANDLE:
    திரும்பு "Invalid easy handle";

  நிலை CURLM_OUT_OF_MEMORY:
    திரும்பு "Out of memory";

  நிலை CURLM_INTERNAL_ERROR:
    திரும்பு "Internal error";

  நிலை CURLM_BAD_SOCKET:
    திரும்பு "Invalid socket argument";

  நிலை CURLM_UNKNOWN_OPTION:
    திரும்பு "Unknown option";

  நிலை CURLM_ADDED_ALREADY:
    திரும்பு "The easy handle is already added to a multi handle";

  நிலை CURLM_RECURSIVE_API_CALL:
    திரும்பு "API function called from within callback";

  நிலை CURLM_WAKEUP_FAILURE:
    திரும்பு "Wakeup is unavailable or failed";

  நிலை CURLM_BAD_FUNCTION_ARGUMENT:
    திரும்பு "A libcurl function was given a bad argument";

  நிலை CURLM_ABORTED_BY_CALLBACK:
    திரும்பு "Operation was aborted by an application callback";

  நிலை CURLM_UNRECOVERABLE_POLL:
    திரும்பு "Unrecoverable error in select/poll";

  நிலை CURLM_LAST:
    நிறுத்து;
  }

  திரும்பு "Unknown error";
#இல்லையெனில்
  எனில்(error == CURLM_OK)
    திரும்பு "No error";
  இல்லையெனில்
    திரும்பு "Error";
#endif
}

மாறிலி எழுத்து *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  தேர்வு(error) {
  நிலை CURLSHE_OK:
    திரும்பு "No error";

  நிலை CURLSHE_BAD_OPTION:
    திரும்பு "Unknown share option";

  நிலை CURLSHE_IN_USE:
    திரும்பு "Share currently in use";

  நிலை CURLSHE_INVALID:
    திரும்பு "Invalid share handle";

  நிலை CURLSHE_NOMEM:
    திரும்பு "Out of memory";

  நிலை CURLSHE_NOT_BUILT_IN:
    திரும்பு "Feature not enabled in this library";

  நிலை CURLSHE_LAST:
    நிறுத்து;
  }

  திரும்பு "CURLSHcode unknown";
#இல்லையெனில்
  எனில்(error == CURLSHE_OK)
    திரும்பு "No error";
  இல்லையெனில்
    திரும்பு "Error";
#endif
}

மாறிலி எழுத்து *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  தேர்வு(error) {
  நிலை CURLUE_OK:
    திரும்பு "No error";

  நிலை CURLUE_BAD_HANDLE:
    திரும்பு "An invalid CURLU pointer was passed as argument";

  நிலை CURLUE_BAD_PARTPOINTER:
    திரும்பு "An invalid 'part' argument was passed as argument";

  நிலை CURLUE_MALFORMED_உள்ளீடு:
    திரும்பு "Malformed input to a URL function";

  நிலை CURLUE_BAD_PORT_எண்:
    திரும்பு "Port number was not a decimal number between 0 and 65535";

  நிலை CURLUE_UNSUPPORTED_SCHEME:
    திரும்பு "Unsupported URL scheme";

  நிலை CURLUE_URLDECODE:
    திரும்பு "URL decode error, most likely because of rubbish in the input";

  நிலை CURLUE_OUT_OF_MEMORY:
    திரும்பு "A memory function failed";

  நிலை CURLUE_USER_NOT_ALLOWED:
    திரும்பு "Credentials was passed in the URL when prohibited";

  நிலை CURLUE_UNKNOWN_PART:
    திரும்பு "An unknown part ID was passed to a URL API function";

  நிலை CURLUE_NO_SCHEME:
    திரும்பு "No scheme present";

  நிலை CURLUE_NO_USER:
    திரும்பு "No user present";

  நிலை CURLUE_NO_PASSWORD:
    திரும்பு "No password present";

  நிலை CURLUE_NO_OPTIONS:
    திரும்பு "No options present";

  நிலை CURLUE_NO_HOST:
    திரும்பு "No host present";

  நிலை CURLUE_NO_PORT:
    திரும்பு "No port number present";

  நிலை CURLUE_NO_QUERY:
    திரும்பு "No query present";

  நிலை CURLUE_NO_FRAGMENT:
    திரும்பு "No fragment present";

  நிலை CURLUE_NO_ZONEID:
    திரும்பு "No zoneid present";

  நிலை CURLUE_BAD_LOGIN:
    திரும்பு "Bad login";

  நிலை CURLUE_BAD_IPV6:
    திரும்பு "Bad IPv6 address";

  நிலை CURLUE_BAD_HOSTNAME:
    திரும்பு "Bad hostname";

  நிலை CURLUE_BAD_FILE_URL:
    திரும்பு "Bad file:// URL";

  நிலை CURLUE_BAD_SLASHES:
    திரும்பு "Unsupported number of slashes following scheme";

  நிலை CURLUE_BAD_SCHEME:
    திரும்பு "Bad scheme";

  நிலை CURLUE_BAD_PATH:
    திரும்பு "Bad path";

  நிலை CURLUE_BAD_FRAGMENT:
    திரும்பு "Bad fragment";

  நிலை CURLUE_BAD_QUERY:
    திரும்பு "Bad query";

  நிலை CURLUE_BAD_PASSWORD:
    திரும்பு "Bad password";

  நிலை CURLUE_BAD_USER:
    திரும்பு "Bad user";

  நிலை CURLUE_LACKS_IDN:
    திரும்பு "libcurl lacks IDN support";

  நிலை CURLUE_TOO_LARGE:
    திரும்பு "A value or data field is larger than allowed";

  நிலை CURLUE_BACKSLASH:
    திரும்பு "Found a backslash where a forward slash was expected";

  நிலை CURLUE_LAST:
    நிறுத்து;
  }

  திரும்பு "CURLUcode unknown";
#இல்லையெனில்
  எனில்(error == CURLUE_OK)
    திரும்பு "No error";
  இல்லையெனில்
    திரும்பு "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
மாறிலி எழுத்து *Curl_sspi_strerror(SECURITY_STATUS err, எழுத்து *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  முழு old_errno = errno;
  VERBOSE(மாறிலி எழுத்து *txt);

  எனில்(!buflen)
    திரும்பு வெற்று;

  *buf = '\0';

#ifdef CURLVERBOSE
  தேர்வு(err) {
  நிலை SEC_E_OK:
    txt = "No error";
    நிறுத்து;
#define SEC2TXT(sec) நிலை sec: txt = #sec; நிறுத்து
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
  SEC2TXT(SEC_E_NO_KERB_சாவி);
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
  SEC2TXT(SEC_E_UNTRUSTED_வேர்);
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
  இயல்பு:
    txt = "Unknown error";
  }

  எனில்(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  இல்லையெனில் {
    எழுத்து msgbuf[256];
    எனில்(curlx_get_winapi_error((DWORD)err, msgbuf, அளவு(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    இல்லையெனில்
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#இல்லையெனில் /* CURLVERBOSE */
  எனில்(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  இல்லையெனில்
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  எனில்(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  எனில்(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  திரும்பு buf;
}
#endif /* USE_WINDOWS_SSPI */
