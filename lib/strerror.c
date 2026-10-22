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

स्थिरांक अक्षर *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  निवड(error) {
  स्थिती CURLE_OK:
    परत "No error";

  स्थिती CURLE_UNSUPPORTED_PROTOCOL:
    परत "Unsupported protocol";

  स्थिती CURLE_FAILED_INIT:
    परत "Failed initialization";

  स्थिती CURLE_URL_MALFORMAT:
    परत "URL using bad/illegal format or missing URL";

  स्थिती CURLE_NOT_BUILT_IN:
    परत "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  स्थिती CURLE_COULDNT_RESOLVE_PROXY:
    परत "Could not resolve proxy name";

  स्थिती CURLE_COULDNT_RESOLVE_HOST:
    परत "Could not resolve hostname";

  स्थिती CURLE_COULDNT_CONNECT:
    परत "Could not connect to server";

  स्थिती CURLE_WEIRD_SERVER_REPLY:
    परत "Weird server reply";

  स्थिती CURLE_REMOTE_ACCESS_DENIED:
    परत "Access denied to remote resource";

  स्थिती CURLE_FTP_ACCEPT_FAILED:
    परत "FTP: The server failed to connect to data port";

  स्थिती CURLE_FTP_ACCEPT_TIMEOUT:
    परत "FTP: Accepting server connect has timed out";

  स्थिती CURLE_FTP_PRET_FAILED:
    परत "FTP: The server did not accept the PRET command.";

  स्थिती CURLE_FTP_WEIRD_PASS_REPLY:
    परत "FTP: unknown PASS reply";

  स्थिती CURLE_FTP_WEIRD_PASV_REPLY:
    परत "FTP: unknown PASV reply";

  स्थिती CURLE_FTP_WEIRD_227_FORMAT:
    परत "FTP: unknown 227 response format";

  स्थिती CURLE_FTP_CANT_GET_HOST:
    परत "FTP: cannot figure out the host in the PASV response";

  स्थिती CURLE_HTTP2:
    परत "Error in the HTTP2 framing layer";

  स्थिती CURLE_FTP_COULDNT_SET_TYPE:
    परत "FTP: could not set file type";

  स्थिती CURLE_PARTIAL_FILE:
    परत "Transferred a partial file";

  स्थिती CURLE_FTP_COULDNT_RETR_FILE:
    परत "FTP: could not retrieve (RETR failed) the specified file";

  स्थिती CURLE_QUOTE_ERROR:
    परत "Quote command returned error";

  स्थिती CURLE_HTTP_RETURNED_ERROR:
    परत "HTTP response code said error";

  स्थिती CURLE_WRITE_ERROR:
    परत "Failed writing received data to disk/application";

  स्थिती CURLE_UPLOAD_FAILED:
    परत "Upload failed (at start/before it took off)";

  स्थिती CURLE_READ_ERROR:
    परत "Failed to open/read local data from file/application";

  स्थिती CURLE_OUT_OF_MEMORY:
    परत "Out of memory";

  स्थिती CURLE_OPERATION_TIMEDOUT:
    परत "Timeout was reached";

  स्थिती CURLE_FTP_PORT_FAILED:
    परत "FTP: command PORT failed";

  स्थिती CURLE_FTP_COULDNT_USE_REST:
    परत "FTP: command REST failed";

  स्थिती CURLE_RANGE_ERROR:
    परत "Requested range was not delivered by the server";

  स्थिती CURLE_SSL_CONNECT_ERROR:
    परत "SSL connect error";

  स्थिती CURLE_BAD_DOWNLOAD_RESUME:
    परत "Could not resume download";

  स्थिती CURLE_FILE_COULDNT_READ_FILE:
    परत "Could not read a file:// file";

  स्थिती CURLE_LDAP_CANNOT_BIND:
    परत "LDAP: cannot bind";

  स्थिती CURLE_LDAP_शोध_FAILED:
    परत "LDAP: search failed";

  स्थिती CURLE_ABORTED_BY_CALLBACK:
    परत "Operation was aborted by an application callback";

  स्थिती CURLE_BAD_FUNCTION_ARGUMENT:
    परत "A libcurl function was given a bad argument";

  स्थिती CURLE_INTERFACE_FAILED:
    परत "Failed binding local connection end";

  स्थिती CURLE_TOO_MANY_REDIRECTS:
    परत "Number of redirects hit maximum amount";

  स्थिती CURLE_UNKNOWN_OPTION:
    परत "An unknown option was passed in to libcurl";

  स्थिती CURLE_SETOPT_OPTION_SYNTAX:
    परत "Malformed option provided in a setopt";

  स्थिती CURLE_GOT_NOTHING:
    परत "Server returned nothing (no headers, no data)";

  स्थिती CURLE_SSL_ENGINE_NOTFOUND:
    परत "SSL crypto engine not found";

  स्थिती CURLE_SSL_ENGINE_SETFAILED:
    परत "Can not set SSL crypto engine as default";

  स्थिती CURLE_SSL_ENGINE_INITFAILED:
    परत "Failed to initialize SSL crypto engine";

  स्थिती CURLE_SEND_ERROR:
    परत "Failed sending data to the peer";

  स्थिती CURLE_RECV_ERROR:
    परत "Failure when receiving data from the peer";

  स्थिती CURLE_SSL_CERTPROBLEM:
    परत "Problem with the local SSL certificate";

  स्थिती CURLE_SSL_CIPHER:
    परत "Could not use specified SSL cipher";

  स्थिती CURLE_PEER_FAILED_VERIFICATION:
    परत "SSL peer certificate or SSH remote key was not OK";

  स्थिती CURLE_SSL_CACERT_BADFILE:
    परत "Problem with the SSL CA cert (path? access rights?)";

  स्थिती CURLE_BAD_CONTENT_ENCODING:
    परत "Unrecognized or bad HTTP Content or Transfer-Encoding";

  स्थिती CURLE_FILESIZE_EXCEEDED:
    परत "Maximum file size exceeded";

  स्थिती CURLE_USE_SSL_FAILED:
    परत "Requested SSL level failed";

  स्थिती CURLE_SSL_SHUTDOWN_FAILED:
    परत "Failed to shut down the SSL connection";

  स्थिती CURLE_SSL_CRL_BADFILE:
    परत "Failed to load CRL file (path? access rights?, format?)";

  स्थिती CURLE_SSL_ISSUER_ERROR:
    परत "Issuer check against peer certificate failed";

  स्थिती CURLE_SEND_FAIL_REWIND:
    परत "Send failed since rewinding of the data stream failed";

  स्थिती CURLE_LOGIN_DENIED:
    परत "Login denied";

  स्थिती CURLE_TFTP_NOTFOUND:
    परत "TFTP: File Not Found";

  स्थिती CURLE_TFTP_PERM:
    परत "TFTP: Access Violation";

  स्थिती CURLE_REMOTE_DISK_FULL:
    परत "Disk full or allocation exceeded";

  स्थिती CURLE_TFTP_ILLEGAL:
    परत "TFTP: Illegal operation";

  स्थिती CURLE_TFTP_UNKNOWNID:
    परत "TFTP: Unknown transfer ID";

  स्थिती CURLE_REMOTE_FILE_EXISTS:
    परत "Remote file already exists";

  स्थिती CURLE_TFTP_NOSUCHUSER:
    परत "TFTP: No such user";

  स्थिती CURLE_REMOTE_FILE_NOT_FOUND:
    परत "Remote file not found";

  स्थिती CURLE_SSH:
    परत "Error in the SSH layer";

  स्थिती CURLE_AGAIN:
    परत "Socket not ready for send/recv";

  स्थिती CURLE_RTSP_CSEQ_ERROR:
    परत "RTSP CSeq mismatch or invalid CSeq";

  स्थिती CURLE_RTSP_SESSION_ERROR:
    परत "RTSP session error";

  स्थिती CURLE_FTP_BAD_FILE_LIST:
    परत "Unable to parse FTP file list";

  स्थिती CURLE_CHUNK_FAILED:
    परत "Chunk callback failed";

  स्थिती CURLE_NO_CONNECTION_AVAILABLE:
    परत "The max connection limit is reached";

  स्थिती CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    परत "SSL public key does not match pinned public key";

  स्थिती CURLE_SSL_INVALIDCERTSTATUS:
    परत "SSL server certificate status verification FAILED";

  स्थिती CURLE_HTTP2_STREAM:
    परत "Stream error in the HTTP/2 framing layer";

  स्थिती CURLE_RECURSIVE_API_CALL:
    परत "API function called from within callback";

  स्थिती CURLE_AUTH_ERROR:
    परत "An authentication function returned an error";

  स्थिती CURLE_HTTP3:
    परत "HTTP/3 error";

  स्थिती CURLE_QUIC_CONNECT_ERROR:
    परत "QUIC connection error";

  स्थिती CURLE_PROXY:
    परत "proxy handshake error";

  स्थिती CURLE_SSL_CLIENTCERT:
    परत "SSL Client Certificate required";

  स्थिती CURLE_UNRECOVERABLE_POLL:
    परत "Unrecoverable error in select/poll";

  स्थिती CURLE_TOO_LARGE:
    परत "A value or data field grew larger than allowed";

  स्थिती CURLE_ECH_REQUIRED:
    परत "ECH attempted but failed";

    /* error codes not used by current libcurl */
  पूर्वनिर्धारित:
    थांबा;
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
  परत "Unknown error";
#अन्यथा
  जर(!error)
    परत "No error";
  अन्यथा
    परत "Error";
#endif
}

स्थिरांक अक्षर *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  निवड(error) {
  स्थिती CURLM_CALL_MULTI_PERFORM:
    परत "Please call curl_multi_perform() soon";

  स्थिती CURLM_OK:
    परत "No error";

  स्थिती CURLM_BAD_HANDLE:
    परत "Invalid multi handle";

  स्थिती CURLM_BAD_EASY_HANDLE:
    परत "Invalid easy handle";

  स्थिती CURLM_OUT_OF_MEMORY:
    परत "Out of memory";

  स्थिती CURLM_INTERNAL_ERROR:
    परत "Internal error";

  स्थिती CURLM_BAD_SOCKET:
    परत "Invalid socket argument";

  स्थिती CURLM_UNKNOWN_OPTION:
    परत "Unknown option";

  स्थिती CURLM_ADDED_ALREADY:
    परत "The easy handle is already added to a multi handle";

  स्थिती CURLM_RECURSIVE_API_CALL:
    परत "API function called from within callback";

  स्थिती CURLM_WAKEUP_FAILURE:
    परत "Wakeup is unavailable or failed";

  स्थिती CURLM_BAD_FUNCTION_ARGUMENT:
    परत "A libcurl function was given a bad argument";

  स्थिती CURLM_ABORTED_BY_CALLBACK:
    परत "Operation was aborted by an application callback";

  स्थिती CURLM_UNRECOVERABLE_POLL:
    परत "Unrecoverable error in select/poll";

  स्थिती CURLM_LAST:
    थांबा;
  }

  परत "Unknown error";
#अन्यथा
  जर(error == CURLM_OK)
    परत "No error";
  अन्यथा
    परत "Error";
#endif
}

स्थिरांक अक्षर *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  निवड(error) {
  स्थिती CURLSHE_OK:
    परत "No error";

  स्थिती CURLSHE_BAD_OPTION:
    परत "Unknown share option";

  स्थिती CURLSHE_IN_USE:
    परत "Share currently in use";

  स्थिती CURLSHE_INVALID:
    परत "Invalid share handle";

  स्थिती CURLSHE_NOMEM:
    परत "Out of memory";

  स्थिती CURLSHE_NOT_BUILT_IN:
    परत "Feature not enabled in this library";

  स्थिती CURLSHE_LAST:
    थांबा;
  }

  परत "CURLSHcode unknown";
#अन्यथा
  जर(error == CURLSHE_OK)
    परत "No error";
  अन्यथा
    परत "Error";
#endif
}

स्थिरांक अक्षर *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  निवड(error) {
  स्थिती CURLUE_OK:
    परत "No error";

  स्थिती CURLUE_BAD_HANDLE:
    परत "An invalid CURLU pointer was passed as argument";

  स्थिती CURLUE_BAD_PARTPOINTER:
    परत "An invalid 'part' argument was passed as argument";

  स्थिती CURLUE_MALFORMED_आदान:
    परत "Malformed input to a URL function";

  स्थिती CURLUE_BAD_PORT_संख्या:
    परत "Port number was not a decimal number between 0 and 65535";

  स्थिती CURLUE_UNSUPPORTED_SCHEME:
    परत "Unsupported URL scheme";

  स्थिती CURLUE_URLDECODE:
    परत "URL decode error, most likely because of rubbish in the input";

  स्थिती CURLUE_OUT_OF_MEMORY:
    परत "A memory function failed";

  स्थिती CURLUE_USER_NOT_ALLOWED:
    परत "Credentials was passed in the URL when prohibited";

  स्थिती CURLUE_UNKNOWN_PART:
    परत "An unknown part ID was passed to a URL API function";

  स्थिती CURLUE_NO_SCHEME:
    परत "No scheme present";

  स्थिती CURLUE_NO_USER:
    परत "No user present";

  स्थिती CURLUE_NO_PASSWORD:
    परत "No password present";

  स्थिती CURLUE_NO_OPTIONS:
    परत "No options present";

  स्थिती CURLUE_NO_HOST:
    परत "No host present";

  स्थिती CURLUE_NO_PORT:
    परत "No port number present";

  स्थिती CURLUE_NO_QUERY:
    परत "No query present";

  स्थिती CURLUE_NO_FRAGMENT:
    परत "No fragment present";

  स्थिती CURLUE_NO_ZONEID:
    परत "No zoneid present";

  स्थिती CURLUE_BAD_LOGIN:
    परत "Bad login";

  स्थिती CURLUE_BAD_IPV6:
    परत "Bad IPv6 address";

  स्थिती CURLUE_BAD_HOSTNAME:
    परत "Bad hostname";

  स्थिती CURLUE_BAD_FILE_URL:
    परत "Bad file:// URL";

  स्थिती CURLUE_BAD_SLASHES:
    परत "Unsupported number of slashes following scheme";

  स्थिती CURLUE_BAD_SCHEME:
    परत "Bad scheme";

  स्थिती CURLUE_BAD_PATH:
    परत "Bad path";

  स्थिती CURLUE_BAD_FRAGMENT:
    परत "Bad fragment";

  स्थिती CURLUE_BAD_QUERY:
    परत "Bad query";

  स्थिती CURLUE_BAD_PASSWORD:
    परत "Bad password";

  स्थिती CURLUE_BAD_USER:
    परत "Bad user";

  स्थिती CURLUE_LACKS_IDN:
    परत "libcurl lacks IDN support";

  स्थिती CURLUE_TOO_LARGE:
    परत "A value or data field is larger than allowed";

  स्थिती CURLUE_BACKSLASH:
    परत "Found a backslash where a forward slash was expected";

  स्थिती CURLUE_LAST:
    थांबा;
  }

  परत "CURLUcode unknown";
#अन्यथा
  जर(error == CURLUE_OK)
    परत "No error";
  अन्यथा
    परत "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
स्थिरांक अक्षर *Curl_sspi_strerror(SECURITY_STATUS err, अक्षर *buf, माप_प्रकार buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  पूर्णांक old_errno = त्रुटि_सं;
  VERBOSE(स्थिरांक अक्षर *txt);

  जर(!buflen)
    परत रिक्त;

  *buf = '\0';

#ifdef CURLVERBOSE
  निवड(err) {
  स्थिती SEC_E_OK:
    txt = "No error";
    थांबा;
#define SEC2TXT(sec) स्थिती sec: txt = #sec; थांबा
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
  SEC2TXT(SEC_E_NO_KERB_किल्ली);
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
  SEC2TXT(SEC_E_UNTRUSTED_मूळ);
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
  पूर्वनिर्धारित:
    txt = "Unknown error";
  }

  जर(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (अचिन्हित दीर्घ)err);
  }
  अन्यथा {
    अक्षर msgbuf[256];
    जर(curlx_get_winapi_error((DWORD)err, msgbuf, आकार(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (अचिन्हित दीर्घ)err,
                     msgbuf);
    अन्यथा
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (अचिन्हित दीर्घ)err);
  }
#अन्यथा /* CURLVERBOSE */
  जर(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  अन्यथा
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  जर(त्रुटि_सं != old_errno)
    त्रुटि_सं = old_errno;

#ifdef _WIN32
  जर(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  परत buf;
}
#endif /* USE_WINDOWS_SSPI */
