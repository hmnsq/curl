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
#شامل "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#شامل "curl_sspi.h"
#endif

#شامل "curlx/winapi.h"
#شامل "strerror.h"
#شامل "curlx/strcopy.h"

ثابت نویسه *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  انتخاب(error) {
  حالت CURLE_OK:
    بازگشت "No error";

  حالت CURLE_UNSUPPORTED_PROTOCOL:
    بازگشت "Unsupported protocol";

  حالت CURLE_FAILED_INIT:
    بازگشت "Failed initialization";

  حالت CURLE_URL_MALFORMAT:
    بازگشت "URL using bad/illegal format or missing URL";

  حالت CURLE_NOT_BUILT_IN:
    بازگشت "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  حالت CURLE_COULDNT_RESOLVE_PROXY:
    بازگشت "Could not resolve proxy name";

  حالت CURLE_COULDNT_RESOLVE_HOST:
    بازگشت "Could not resolve hostname";

  حالت CURLE_COULDNT_CONNECT:
    بازگشت "Could not connect to server";

  حالت CURLE_WEIRD_SERVER_REPLY:
    بازگشت "Weird server reply";

  حالت CURLE_REMOTE_ACCESS_DENIED:
    بازگشت "Access denied to remote resource";

  حالت CURLE_FTP_ACCEPT_FAILED:
    بازگشت "FTP: The server failed to connect to data port";

  حالت CURLE_FTP_ACCEPT_TIMEOUT:
    بازگشت "FTP: Accepting server connect has timed out";

  حالت CURLE_FTP_PRET_FAILED:
    بازگشت "FTP: The server did not accept the PRET command.";

  حالت CURLE_FTP_WEIRD_PASS_REPLY:
    بازگشت "FTP: unknown PASS reply";

  حالت CURLE_FTP_WEIRD_PASV_REPLY:
    بازگشت "FTP: unknown PASV reply";

  حالت CURLE_FTP_WEIRD_227_FORMAT:
    بازگشت "FTP: unknown 227 response format";

  حالت CURLE_FTP_CANT_GET_HOST:
    بازگشت "FTP: cannot figure out the host in the PASV response";

  حالت CURLE_HTTP2:
    بازگشت "Error in the HTTP2 framing layer";

  حالت CURLE_FTP_COULDNT_SET_TYPE:
    بازگشت "FTP: could not set file type";

  حالت CURLE_PARTIAL_FILE:
    بازگشت "Transferred a partial file";

  حالت CURLE_FTP_COULDNT_RETR_FILE:
    بازگشت "FTP: could not retrieve (RETR failed) the specified file";

  حالت CURLE_QUOTE_ERROR:
    بازگشت "Quote command returned error";

  حالت CURLE_HTTP_RETURNED_ERROR:
    بازگشت "HTTP response code said error";

  حالت CURLE_WRITE_ERROR:
    بازگشت "Failed writing received data to disk/application";

  حالت CURLE_UPLOAD_FAILED:
    بازگشت "Upload failed (at start/before it took off)";

  حالت CURLE_READ_ERROR:
    بازگشت "Failed to open/read local data from file/application";

  حالت CURLE_OUT_OF_MEMORY:
    بازگشت "Out of memory";

  حالت CURLE_OPERATION_TIMEDOUT:
    بازگشت "Timeout was reached";

  حالت CURLE_FTP_PORT_FAILED:
    بازگشت "FTP: command PORT failed";

  حالت CURLE_FTP_COULDNT_USE_REST:
    بازگشت "FTP: command REST failed";

  حالت CURLE_RANGE_ERROR:
    بازگشت "Requested range was not delivered by the server";

  حالت CURLE_SSL_CONNECT_ERROR:
    بازگشت "SSL connect error";

  حالت CURLE_BAD_DOWNLOAD_RESUME:
    بازگشت "Could not resume download";

  حالت CURLE_FILE_COULDNT_READ_FILE:
    بازگشت "Could not read a file:// file";

  حالت CURLE_LDAP_CANNOT_BIND:
    بازگشت "LDAP: cannot bind";

  حالت CURLE_LDAP_جستجو_FAILED:
    بازگشت "LDAP: search failed";

  حالت CURLE_ABORTED_BY_CALLBACK:
    بازگشت "Operation was aborted by an application callback";

  حالت CURLE_BAD_FUNCTION_ARGUMENT:
    بازگشت "A libcurl function was given a bad argument";

  حالت CURLE_INTERFACE_FAILED:
    بازگشت "Failed binding local connection end";

  حالت CURLE_TOO_MANY_REDIRECTS:
    بازگشت "Number of redirects hit maximum amount";

  حالت CURLE_UNKNOWN_OPTION:
    بازگشت "An unknown option was passed in to libcurl";

  حالت CURLE_SETOPT_OPTION_SYNTAX:
    بازگشت "Malformed option provided in a setopt";

  حالت CURLE_GOT_NOTHING:
    بازگشت "Server returned nothing (no headers, no data)";

  حالت CURLE_SSL_ENGINE_NOTFOUND:
    بازگشت "SSL crypto engine not found";

  حالت CURLE_SSL_ENGINE_SETFAILED:
    بازگشت "Can not set SSL crypto engine as default";

  حالت CURLE_SSL_ENGINE_INITFAILED:
    بازگشت "Failed to initialize SSL crypto engine";

  حالت CURLE_SEND_ERROR:
    بازگشت "Failed sending data to the peer";

  حالت CURLE_RECV_ERROR:
    بازگشت "Failure when receiving data from the peer";

  حالت CURLE_SSL_CERTPROBLEM:
    بازگشت "Problem with the local SSL certificate";

  حالت CURLE_SSL_CIPHER:
    بازگشت "Could not use specified SSL cipher";

  حالت CURLE_PEER_FAILED_VERIFICATION:
    بازگشت "SSL peer certificate or SSH remote key was not OK";

  حالت CURLE_SSL_CACERT_BADFILE:
    بازگشت "Problem with the SSL CA cert (path? access rights?)";

  حالت CURLE_BAD_CONTENT_ENCODING:
    بازگشت "Unrecognized or bad HTTP Content or Transfer-Encoding";

  حالت CURLE_FILESIZE_EXCEEDED:
    بازگشت "Maximum file size exceeded";

  حالت CURLE_USE_SSL_FAILED:
    بازگشت "Requested SSL level failed";

  حالت CURLE_SSL_SHUTDOWN_FAILED:
    بازگشت "Failed to shut down the SSL connection";

  حالت CURLE_SSL_CRL_BADFILE:
    بازگشت "Failed to load CRL file (path? access rights?, format?)";

  حالت CURLE_SSL_ISSUER_ERROR:
    بازگشت "Issuer check against peer certificate failed";

  حالت CURLE_SEND_FAIL_REWIND:
    بازگشت "Send failed since rewinding of the data stream failed";

  حالت CURLE_LOGIN_DENIED:
    بازگشت "Login denied";

  حالت CURLE_TFTP_NOTFOUND:
    بازگشت "TFTP: File Not Found";

  حالت CURLE_TFTP_PERM:
    بازگشت "TFTP: Access Violation";

  حالت CURLE_REMOTE_DISK_FULL:
    بازگشت "Disk full or allocation exceeded";

  حالت CURLE_TFTP_ILLEGAL:
    بازگشت "TFTP: Illegal operation";

  حالت CURLE_TFTP_UNKNOWNID:
    بازگشت "TFTP: Unknown transfer ID";

  حالت CURLE_REMOTE_FILE_EXISTS:
    بازگشت "Remote file already exists";

  حالت CURLE_TFTP_NOSUCHUSER:
    بازگشت "TFTP: No such user";

  حالت CURLE_REMOTE_FILE_NOT_FOUND:
    بازگشت "Remote file not found";

  حالت CURLE_SSH:
    بازگشت "Error in the SSH layer";

  حالت CURLE_AGAIN:
    بازگشت "Socket not ready for send/recv";

  حالت CURLE_RTSP_CSEQ_ERROR:
    بازگشت "RTSP CSeq mismatch or invalid CSeq";

  حالت CURLE_RTSP_SESSION_ERROR:
    بازگشت "RTSP session error";

  حالت CURLE_FTP_BAD_FILE_LIST:
    بازگشت "Unable to parse FTP file list";

  حالت CURLE_CHUNK_FAILED:
    بازگشت "Chunk callback failed";

  حالت CURLE_NO_CONNECTION_AVAILABLE:
    بازگشت "The max connection limit is reached";

  حالت CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    بازگشت "SSL public key does not match pinned public key";

  حالت CURLE_SSL_INVALIDCERTSTATUS:
    بازگشت "SSL server certificate status verification FAILED";

  حالت CURLE_HTTP2_STREAM:
    بازگشت "Stream error in the HTTP/2 framing layer";

  حالت CURLE_RECURSIVE_API_CALL:
    بازگشت "API function called from within callback";

  حالت CURLE_AUTH_ERROR:
    بازگشت "An authentication function returned an error";

  حالت CURLE_HTTP3:
    بازگشت "HTTP/3 error";

  حالت CURLE_QUIC_CONNECT_ERROR:
    بازگشت "QUIC connection error";

  حالت CURLE_PROXY:
    بازگشت "proxy handshake error";

  حالت CURLE_SSL_CLIENTCERT:
    بازگشت "SSL Client Certificate required";

  حالت CURLE_UNRECOVERABLE_POLL:
    بازگشت "Unrecoverable error in select/poll";

  حالت CURLE_TOO_LARGE:
    بازگشت "A value or data field grew larger than allowed";

  حالت CURLE_ECH_REQUIRED:
    بازگشت "ECH attempted but failed";

    /* error codes not used by current libcurl */
  پیشفرض:
    بشکن;
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
  بازگشت "Unknown error";
#وگرنه
  اگر(!error)
    بازگشت "No error";
  وگرنه
    بازگشت "Error";
#endif
}

ثابت نویسه *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  انتخاب(error) {
  حالت CURLM_CALL_MULTI_PERFORM:
    بازگشت "Please call curl_multi_perform() soon";

  حالت CURLM_OK:
    بازگشت "No error";

  حالت CURLM_BAD_HANDLE:
    بازگشت "Invalid multi handle";

  حالت CURLM_BAD_EASY_HANDLE:
    بازگشت "Invalid easy handle";

  حالت CURLM_OUT_OF_MEMORY:
    بازگشت "Out of memory";

  حالت CURLM_INTERNAL_ERROR:
    بازگشت "Internal error";

  حالت CURLM_BAD_SOCKET:
    بازگشت "Invalid socket argument";

  حالت CURLM_UNKNOWN_OPTION:
    بازگشت "Unknown option";

  حالت CURLM_ADDED_ALREADY:
    بازگشت "The easy handle is already added to a multi handle";

  حالت CURLM_RECURSIVE_API_CALL:
    بازگشت "API function called from within callback";

  حالت CURLM_WAKEUP_FAILURE:
    بازگشت "Wakeup is unavailable or failed";

  حالت CURLM_BAD_FUNCTION_ARGUMENT:
    بازگشت "A libcurl function was given a bad argument";

  حالت CURLM_ABORTED_BY_CALLBACK:
    بازگشت "Operation was aborted by an application callback";

  حالت CURLM_UNRECOVERABLE_POLL:
    بازگشت "Unrecoverable error in select/poll";

  حالت CURLM_LAST:
    بشکن;
  }

  بازگشت "Unknown error";
#وگرنه
  اگر(error == CURLM_OK)
    بازگشت "No error";
  وگرنه
    بازگشت "Error";
#endif
}

ثابت نویسه *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  انتخاب(error) {
  حالت CURLSHE_OK:
    بازگشت "No error";

  حالت CURLSHE_BAD_OPTION:
    بازگشت "Unknown share option";

  حالت CURLSHE_IN_USE:
    بازگشت "Share currently in use";

  حالت CURLSHE_INVALID:
    بازگشت "Invalid share handle";

  حالت CURLSHE_NOMEM:
    بازگشت "Out of memory";

  حالت CURLSHE_NOT_BUILT_IN:
    بازگشت "Feature not enabled in this library";

  حالت CURLSHE_LAST:
    بشکن;
  }

  بازگشت "CURLSHcode unknown";
#وگرنه
  اگر(error == CURLSHE_OK)
    بازگشت "No error";
  وگرنه
    بازگشت "Error";
#endif
}

ثابت نویسه *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  انتخاب(error) {
  حالت CURLUE_OK:
    بازگشت "No error";

  حالت CURLUE_BAD_HANDLE:
    بازگشت "An invalid CURLU pointer was passed as argument";

  حالت CURLUE_BAD_PARTPOINTER:
    بازگشت "An invalid 'part' argument was passed as argument";

  حالت CURLUE_MALFORMED_درون_داد:
    بازگشت "Malformed input to a URL function";

  حالت CURLUE_BAD_PORT_عدد:
    بازگشت "Port number was not a decimal number between 0 and 65535";

  حالت CURLUE_UNSUPPORTED_SCHEME:
    بازگشت "Unsupported URL scheme";

  حالت CURLUE_URLDECODE:
    بازگشت "URL decode error, most likely because of rubbish in the input";

  حالت CURLUE_OUT_OF_MEMORY:
    بازگشت "A memory function failed";

  حالت CURLUE_USER_NOT_ALLOWED:
    بازگشت "Credentials was passed in the URL when prohibited";

  حالت CURLUE_UNKNOWN_PART:
    بازگشت "An unknown part ID was passed to a URL API function";

  حالت CURLUE_NO_SCHEME:
    بازگشت "No scheme present";

  حالت CURLUE_NO_USER:
    بازگشت "No user present";

  حالت CURLUE_NO_PASSWORD:
    بازگشت "No password present";

  حالت CURLUE_NO_OPTIONS:
    بازگشت "No options present";

  حالت CURLUE_NO_HOST:
    بازگشت "No host present";

  حالت CURLUE_NO_PORT:
    بازگشت "No port number present";

  حالت CURLUE_NO_QUERY:
    بازگشت "No query present";

  حالت CURLUE_NO_FRAGMENT:
    بازگشت "No fragment present";

  حالت CURLUE_NO_ZONEID:
    بازگشت "No zoneid present";

  حالت CURLUE_BAD_LOGIN:
    بازگشت "Bad login";

  حالت CURLUE_BAD_IPV6:
    بازگشت "Bad IPv6 address";

  حالت CURLUE_BAD_HOSTNAME:
    بازگشت "Bad hostname";

  حالت CURLUE_BAD_FILE_URL:
    بازگشت "Bad file:// URL";

  حالت CURLUE_BAD_SLASHES:
    بازگشت "Unsupported number of slashes following scheme";

  حالت CURLUE_BAD_SCHEME:
    بازگشت "Bad scheme";

  حالت CURLUE_BAD_PATH:
    بازگشت "Bad path";

  حالت CURLUE_BAD_FRAGMENT:
    بازگشت "Bad fragment";

  حالت CURLUE_BAD_QUERY:
    بازگشت "Bad query";

  حالت CURLUE_BAD_PASSWORD:
    بازگشت "Bad password";

  حالت CURLUE_BAD_USER:
    بازگشت "Bad user";

  حالت CURLUE_LACKS_IDN:
    بازگشت "libcurl lacks IDN support";

  حالت CURLUE_TOO_LARGE:
    بازگشت "A value or data field is larger than allowed";

  حالت CURLUE_BACKSLASH:
    بازگشت "Found a backslash where a forward slash was expected";

  حالت CURLUE_LAST:
    بشکن;
  }

  بازگشت "CURLUcode unknown";
#وگرنه
  اگر(error == CURLUE_OK)
    بازگشت "No error";
  وگرنه
    بازگشت "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ثابت نویسه *Curl_sspi_strerror(SECURITY_STATUS err, نویسه *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  صحیح old_errno = errno;
  VERBOSE(ثابت نویسه *txt);

  اگر(!buflen)
    بازگشت پوچ;

  *buf = '\0';

#ifdef CURLVERBOSE
  انتخاب(err) {
  حالت SEC_E_OK:
    txt = "No error";
    بشکن;
#define SEC2TXT(sec) حالت sec: txt = #sec; بشکن
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
  SEC2TXT(SEC_E_NO_KERB_کلید);
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
  SEC2TXT(SEC_E_UNTRUSTED_ریشه);
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
  پیشفرض:
    txt = "Unknown error";
  }

  اگر(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  وگرنه {
    نویسه msgbuf[256];
    اگر(curlx_get_winapi_error((DWORD)err, msgbuf, اندازه(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    وگرنه
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#وگرنه /* CURLVERBOSE */
  اگر(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  وگرنه
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  اگر(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  اگر(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  بازگشت buf;
}
#endif /* USE_WINDOWS_SSPI */
