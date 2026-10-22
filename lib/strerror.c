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
#समावेश "curl_setup.h"

#अगर_घोषित USE_WINDOWS_SSPI
#समावेश "curl_sspi.h"
#पूर्ण_अगर

#समावेश "curlx/winapi.h"
#समावेश "strerror.h"
#समावेश "curlx/strcopy.h"

स्थिर अक्षर *curl_easy_strerror(CURLcode error)
{
#अगर_घोषित CURLVERBOSE
  चयन(error) {
  हाल CURLE_OK:
    वापस "No error";

  हाल CURLE_UNSUPPORTED_PROTOCOL:
    वापस "Unsupported protocol";

  हाल CURLE_FAILED_INIT:
    वापस "Failed initialization";

  हाल CURLE_URL_MALFORMAT:
    वापस "URL using bad/illegal format or missing URL";

  हाल CURLE_NOT_BUILT_IN:
    वापस "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  हाल CURLE_COULDNT_RESOLVE_PROXY:
    वापस "Could not resolve proxy name";

  हाल CURLE_COULDNT_RESOLVE_HOST:
    वापस "Could not resolve hostname";

  हाल CURLE_COULDNT_CONNECT:
    वापस "Could not connect to server";

  हाल CURLE_WEIRD_SERVER_REPLY:
    वापस "Weird server reply";

  हाल CURLE_REMOTE_ACCESS_DENIED:
    वापस "Access denied to remote resource";

  हाल CURLE_FTP_ACCEPT_FAILED:
    वापस "FTP: The server failed to connect to data port";

  हाल CURLE_FTP_ACCEPT_TIMEOUT:
    वापस "FTP: Accepting server connect has timed out";

  हाल CURLE_FTP_PRET_FAILED:
    वापस "FTP: The server did not accept the PRET command.";

  हाल CURLE_FTP_WEIRD_PASS_REPLY:
    वापस "FTP: unknown PASS reply";

  हाल CURLE_FTP_WEIRD_PASV_REPLY:
    वापस "FTP: unknown PASV reply";

  हाल CURLE_FTP_WEIRD_227_FORMAT:
    वापस "FTP: unknown 227 response format";

  हाल CURLE_FTP_CANT_GET_HOST:
    वापस "FTP: cannot figure out the host in the PASV response";

  हाल CURLE_HTTP2:
    वापस "Error in the HTTP2 framing layer";

  हाल CURLE_FTP_COULDNT_SET_TYPE:
    वापस "FTP: could not set file type";

  हाल CURLE_PARTIAL_FILE:
    वापस "Transferred a partial file";

  हाल CURLE_FTP_COULDNT_RETR_FILE:
    वापस "FTP: could not retrieve (RETR failed) the specified file";

  हाल CURLE_QUOTE_ERROR:
    वापस "Quote command returned error";

  हाल CURLE_HTTP_RETURNED_ERROR:
    वापस "HTTP response code said error";

  हाल CURLE_WRITE_ERROR:
    वापस "Failed writing received data to disk/application";

  हाल CURLE_UPLOAD_FAILED:
    वापस "Upload failed (at start/before it took off)";

  हाल CURLE_READ_ERROR:
    वापस "Failed to open/read local data from file/application";

  हाल CURLE_OUT_OF_MEMORY:
    वापस "Out of memory";

  हाल CURLE_OPERATION_TIMEDOUT:
    वापस "Timeout was reached";

  हाल CURLE_FTP_PORT_FAILED:
    वापस "FTP: command PORT failed";

  हाल CURLE_FTP_COULDNT_USE_REST:
    वापस "FTP: command REST failed";

  हाल CURLE_RANGE_ERROR:
    वापस "Requested range was not delivered by the server";

  हाल CURLE_SSL_CONNECT_ERROR:
    वापस "SSL connect error";

  हाल CURLE_BAD_DOWNLOAD_RESUME:
    वापस "Could not resume download";

  हाल CURLE_FILE_COULDNT_READ_FILE:
    वापस "Could not read a file:// file";

  हाल CURLE_LDAP_CANNOT_BIND:
    वापस "LDAP: cannot bind";

  हाल CURLE_LDAP_खोज_FAILED:
    वापस "LDAP: search failed";

  हाल CURLE_ABORTED_BY_CALLBACK:
    वापस "Operation was aborted by an application callback";

  हाल CURLE_BAD_FUNCTION_ARGUMENT:
    वापस "A libcurl function was given a bad argument";

  हाल CURLE_INTERFACE_FAILED:
    वापस "Failed binding local connection end";

  हाल CURLE_TOO_MANY_REDIRECTS:
    वापस "Number of redirects hit maximum amount";

  हाल CURLE_UNKNOWN_OPTION:
    वापस "An unknown option was passed in to libcurl";

  हाल CURLE_SETOPT_OPTION_SYNTAX:
    वापस "Malformed option provided in a setopt";

  हाल CURLE_GOT_NOTHING:
    वापस "Server returned nothing (no headers, no data)";

  हाल CURLE_SSL_ENGINE_NOTFOUND:
    वापस "SSL crypto engine not found";

  हाल CURLE_SSL_ENGINE_SETFAILED:
    वापस "Can not set SSL crypto engine as default";

  हाल CURLE_SSL_ENGINE_INITFAILED:
    वापस "Failed to initialize SSL crypto engine";

  हाल CURLE_SEND_ERROR:
    वापस "Failed sending data to the peer";

  हाल CURLE_RECV_ERROR:
    वापस "Failure when receiving data from the peer";

  हाल CURLE_SSL_CERTPROBLEM:
    वापस "Problem with the local SSL certificate";

  हाल CURLE_SSL_CIPHER:
    वापस "Could not use specified SSL cipher";

  हाल CURLE_PEER_FAILED_VERIFICATION:
    वापस "SSL peer certificate or SSH remote key was not OK";

  हाल CURLE_SSL_CACERT_BADFILE:
    वापस "Problem with the SSL CA cert (path? access rights?)";

  हाल CURLE_BAD_CONTENT_ENCODING:
    वापस "Unrecognized or bad HTTP Content or Transfer-Encoding";

  हाल CURLE_FILESIZE_EXCEEDED:
    वापस "Maximum file size exceeded";

  हाल CURLE_USE_SSL_FAILED:
    वापस "Requested SSL level failed";

  हाल CURLE_SSL_SHUTDOWN_FAILED:
    वापस "Failed to shut down the SSL connection";

  हाल CURLE_SSL_CRL_BADFILE:
    वापस "Failed to load CRL file (path? access rights?, format?)";

  हाल CURLE_SSL_ISSUER_ERROR:
    वापस "Issuer check against peer certificate failed";

  हाल CURLE_SEND_FAIL_REWIND:
    वापस "Send failed since rewinding of the data stream failed";

  हाल CURLE_LOGIN_DENIED:
    वापस "Login denied";

  हाल CURLE_TFTP_NOTFOUND:
    वापस "TFTP: File Not Found";

  हाल CURLE_TFTP_PERM:
    वापस "TFTP: Access Violation";

  हाल CURLE_REMOTE_DISK_FULL:
    वापस "Disk full or allocation exceeded";

  हाल CURLE_TFTP_ILLEGAL:
    वापस "TFTP: Illegal operation";

  हाल CURLE_TFTP_UNKNOWNID:
    वापस "TFTP: Unknown transfer ID";

  हाल CURLE_REMOTE_FILE_EXISTS:
    वापस "Remote file already exists";

  हाल CURLE_TFTP_NOSUCHUSER:
    वापस "TFTP: No such user";

  हाल CURLE_REMOTE_FILE_NOT_FOUND:
    वापस "Remote file not found";

  हाल CURLE_SSH:
    वापस "Error in the SSH layer";

  हाल CURLE_AGAIN:
    वापस "Socket not ready for send/recv";

  हाल CURLE_RTSP_CSEQ_ERROR:
    वापस "RTSP CSeq mismatch or invalid CSeq";

  हाल CURLE_RTSP_SESSION_ERROR:
    वापस "RTSP session error";

  हाल CURLE_FTP_BAD_FILE_LIST:
    वापस "Unable to parse FTP file list";

  हाल CURLE_CHUNK_FAILED:
    वापस "Chunk callback failed";

  हाल CURLE_NO_CONNECTION_AVAILABLE:
    वापस "The max connection limit is reached";

  हाल CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    वापस "SSL public key does not match pinned public key";

  हाल CURLE_SSL_INVALIDCERTSTATUS:
    वापस "SSL server certificate status verification FAILED";

  हाल CURLE_HTTP2_STREAM:
    वापस "Stream error in the HTTP/2 framing layer";

  हाल CURLE_RECURSIVE_API_CALL:
    वापस "API function called from within callback";

  हाल CURLE_AUTH_ERROR:
    वापस "An authentication function returned an error";

  हाल CURLE_HTTP3:
    वापस "HTTP/3 error";

  हाल CURLE_QUIC_CONNECT_ERROR:
    वापस "QUIC connection error";

  हाल CURLE_PROXY:
    वापस "proxy handshake error";

  हाल CURLE_SSL_CLIENTCERT:
    वापस "SSL Client Certificate required";

  हाल CURLE_UNRECOVERABLE_POLL:
    वापस "Unrecoverable error in select/poll";

  हाल CURLE_TOO_LARGE:
    वापस "A value or data field grew larger than allowed";

  हाल CURLE_ECH_REQUIRED:
    वापस "ECH attempted but failed";

    /* error codes not used by current libcurl */
  शेष:
    अवरोध;
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
  वापस "Unknown error";
#अन्यथा
  अगर(!error)
    वापस "No error";
  अन्यथा
    वापस "Error";
#पूर्ण_अगर
}

स्थिर अक्षर *curl_multi_strerror(CURLMcode error)
{
#अगर_घोषित CURLVERBOSE
  चयन(error) {
  हाल CURLM_CALL_MULTI_PERFORM:
    वापस "Please call curl_multi_perform() soon";

  हाल CURLM_OK:
    वापस "No error";

  हाल CURLM_BAD_HANDLE:
    वापस "Invalid multi handle";

  हाल CURLM_BAD_EASY_HANDLE:
    वापस "Invalid easy handle";

  हाल CURLM_OUT_OF_MEMORY:
    वापस "Out of memory";

  हाल CURLM_INTERNAL_ERROR:
    वापस "Internal error";

  हाल CURLM_BAD_SOCKET:
    वापस "Invalid socket argument";

  हाल CURLM_UNKNOWN_OPTION:
    वापस "Unknown option";

  हाल CURLM_ADDED_ALREADY:
    वापस "The easy handle is already added to a multi handle";

  हाल CURLM_RECURSIVE_API_CALL:
    वापस "API function called from within callback";

  हाल CURLM_WAKEUP_FAILURE:
    वापस "Wakeup is unavailable or failed";

  हाल CURLM_BAD_FUNCTION_ARGUMENT:
    वापस "A libcurl function was given a bad argument";

  हाल CURLM_ABORTED_BY_CALLBACK:
    वापस "Operation was aborted by an application callback";

  हाल CURLM_UNRECOVERABLE_POLL:
    वापस "Unrecoverable error in select/poll";

  हाल CURLM_LAST:
    अवरोध;
  }

  वापस "Unknown error";
#अन्यथा
  अगर(error == CURLM_OK)
    वापस "No error";
  अन्यथा
    वापस "Error";
#पूर्ण_अगर
}

स्थिर अक्षर *curl_share_strerror(CURLSHcode error)
{
#अगर_घोषित CURLVERBOSE
  चयन(error) {
  हाल CURLSHE_OK:
    वापस "No error";

  हाल CURLSHE_BAD_OPTION:
    वापस "Unknown share option";

  हाल CURLSHE_IN_USE:
    वापस "Share currently in use";

  हाल CURLSHE_INVALID:
    वापस "Invalid share handle";

  हाल CURLSHE_NOMEM:
    वापस "Out of memory";

  हाल CURLSHE_NOT_BUILT_IN:
    वापस "Feature not enabled in this library";

  हाल CURLSHE_LAST:
    अवरोध;
  }

  वापस "CURLSHcode unknown";
#अन्यथा
  अगर(error == CURLSHE_OK)
    वापस "No error";
  अन्यथा
    वापस "Error";
#पूर्ण_अगर
}

स्थिर अक्षर *curl_url_strerror(CURLUcode error)
{
#अगर_घोषित CURLVERBOSE
  चयन(error) {
  हाल CURLUE_OK:
    वापस "No error";

  हाल CURLUE_BAD_HANDLE:
    वापस "An invalid CURLU pointer was passed as argument";

  हाल CURLUE_BAD_PARTPOINTER:
    वापस "An invalid 'part' argument was passed as argument";

  हाल CURLUE_MALFORMED_निवेश:
    वापस "Malformed input to a URL function";

  हाल CURLUE_BAD_PORT_संख्या:
    वापस "Port number was not a decimal number between 0 and 65535";

  हाल CURLUE_UNSUPPORTED_SCHEME:
    वापस "Unsupported URL scheme";

  हाल CURLUE_URLDECODE:
    वापस "URL decode error, most likely because of rubbish in the input";

  हाल CURLUE_OUT_OF_MEMORY:
    वापस "A memory function failed";

  हाल CURLUE_USER_NOT_ALLOWED:
    वापस "Credentials was passed in the URL when prohibited";

  हाल CURLUE_UNKNOWN_PART:
    वापस "An unknown part ID was passed to a URL API function";

  हाल CURLUE_NO_SCHEME:
    वापस "No scheme present";

  हाल CURLUE_NO_USER:
    वापस "No user present";

  हाल CURLUE_NO_PASSWORD:
    वापस "No password present";

  हाल CURLUE_NO_OPTIONS:
    वापस "No options present";

  हाल CURLUE_NO_HOST:
    वापस "No host present";

  हाल CURLUE_NO_PORT:
    वापस "No port number present";

  हाल CURLUE_NO_QUERY:
    वापस "No query present";

  हाल CURLUE_NO_FRAGMENT:
    वापस "No fragment present";

  हाल CURLUE_NO_ZONEID:
    वापस "No zoneid present";

  हाल CURLUE_BAD_LOGIN:
    वापस "Bad login";

  हाल CURLUE_BAD_IPV6:
    वापस "Bad IPv6 address";

  हाल CURLUE_BAD_HOSTNAME:
    वापस "Bad hostname";

  हाल CURLUE_BAD_FILE_URL:
    वापस "Bad file:// URL";

  हाल CURLUE_BAD_SLASHES:
    वापस "Unsupported number of slashes following scheme";

  हाल CURLUE_BAD_SCHEME:
    वापस "Bad scheme";

  हाल CURLUE_BAD_PATH:
    वापस "Bad path";

  हाल CURLUE_BAD_FRAGMENT:
    वापस "Bad fragment";

  हाल CURLUE_BAD_QUERY:
    वापस "Bad query";

  हाल CURLUE_BAD_PASSWORD:
    वापस "Bad password";

  हाल CURLUE_BAD_USER:
    वापस "Bad user";

  हाल CURLUE_LACKS_IDN:
    वापस "libcurl lacks IDN support";

  हाल CURLUE_TOO_LARGE:
    वापस "A value or data field is larger than allowed";

  हाल CURLUE_BACKSLASH:
    वापस "Found a backslash where a forward slash was expected";

  हाल CURLUE_LAST:
    अवरोध;
  }

  वापस "CURLUcode unknown";
#अन्यथा
  अगर(error == CURLUE_OK)
    वापस "No error";
  अन्यथा
    वापस "Error";
#पूर्ण_अगर
}

#अगर_घोषित USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
स्थिर अक्षर *Curl_sspi_strerror(SECURITY_STATUS err, अक्षर *buf, माप_प्रकार buflen)
{
#अगर_घोषित _WIN32
  DWORD old_win_err = GetLastError();
#पूर्ण_अगर
  पूर्णांक old_errno = त्रुटि_सं;
  VERBOSE(स्थिर अक्षर *txt);

  अगर(!buflen)
    वापस शून्य;

  *buf = '\0';

#अगर_घोषित CURLVERBOSE
  चयन(err) {
  हाल SEC_E_OK:
    txt = "No error";
    अवरोध;
#घोषणा SEC2TXT(sec) हाल sec: txt = #sec; अवरोध
  SEC2TXT(CRYPT_E_REVOKED);
  SEC2TXT(CRYPT_E_NO_REVOCATION_DLL);
  SEC2TXT(CRYPT_E_NO_REVOCATION_CHECK);
  SEC2TXT(CRYPT_E_REVOCATION_OFFLINE);
  SEC2TXT(CRYPT_E_NOT_IN_REVOCATION_DATABASE);
  SEC2TXT(SEC_E_ALGORITHM_MISMATCH);
  SEC2TXT(SEC_E_BAD_BINDINGS);
  SEC2TXT(SEC_E_BAD_PKGID);
  SEC2TXT(SEC_E_बफर_TOO_SMALL);
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
  SEC2TXT(SEC_E_NO_KERB_कुंजी);
  SEC2TXT(SEC_E_NO_PA_तथ्य);
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
  SEC2TXT(SEC_E_UNTRUSTED_मूल);
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
  शेष:
    txt = "Unknown error";
  }

  अगर(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (अचिन्हित दीर्घ)err);
  }
  अन्यथा {
    अक्षर msgbuf[256];
    अगर(curlx_get_winapi_error((DWORD)err, msgbuf, माप(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (अचिन्हित दीर्घ)err,
                     msgbuf);
    अन्यथा
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (अचिन्हित दीर्घ)err);
  }
#अन्यथा /* CURLVERBOSE */
  अगर(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  अन्यथा
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#पूर्ण_अगर

  अगर(त्रुटि_सं != old_errno)
    त्रुटि_सं = old_errno;

#अगर_घोषित _WIN32
  अगर(old_win_err != GetLastError())
    SetLastError(old_win_err);
#पूर्ण_अगर

  वापस buf;
}
#पूर्ण_अगर /* USE_WINDOWS_SSPI */
