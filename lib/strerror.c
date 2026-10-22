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

#اگر_تعریف_شدہ USE_WINDOWS_SSPI
#شامل "curl_sspi.h"
#اگر_ختم

#شامل "curlx/winapi.h"
#شامل "strerror.h"
#شامل "curlx/strcopy.h"

ثابت حرف *curl_easy_strerror(CURLcode error)
{
#اگر_تعریف_شدہ CURLVERBOSE
  انتخاب(error) {
  حالت CURLE_OK:
    واپس "No error";

  حالت CURLE_UNSUPPORTED_PROTOCOL:
    واپس "Unsupported protocol";

  حالت CURLE_FAILED_INIT:
    واپس "Failed initialization";

  حالت CURLE_URL_MALFORMAT:
    واپس "URL using bad/illegal format or missing URL";

  حالت CURLE_NOT_BUILT_اندر:
    واپس "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  حالت CURLE_COULDNT_RESOLVE_PROXY:
    واپس "Could not resolve proxy name";

  حالت CURLE_COULDNT_RESOLVE_HOST:
    واپس "Could not resolve hostname";

  حالت CURLE_COULDNT_CONNECT:
    واپس "Could not connect to server";

  حالت CURLE_WEIRD_SERVER_جواب:
    واپس "Weird server reply";

  حالت CURLE_REMOTE_ACCESS_DENIED:
    واپس "Access denied to remote resource";

  حالت CURLE_FTP_ACCEPT_FAILED:
    واپس "FTP: The server failed to connect to data port";

  حالت CURLE_FTP_ACCEPT_TIMEOUT:
    واپس "FTP: Accepting server connect has timed out";

  حالت CURLE_FTP_PRET_FAILED:
    واپس "FTP: The server did not accept the PRET command.";

  حالت CURLE_FTP_WEIRD_PASS_جواب:
    واپس "FTP: unknown PASS reply";

  حالت CURLE_FTP_WEIRD_PASV_جواب:
    واپس "FTP: unknown PASV reply";

  حالت CURLE_FTP_WEIRD_227_FORMAT:
    واپس "FTP: unknown 227 response format";

  حالت CURLE_FTP_CANT_GET_HOST:
    واپس "FTP: cannot figure out the host in the PASV response";

  حالت CURLE_HTTP2:
    واپس "Error in the HTTP2 framing layer";

  حالت CURLE_FTP_COULDNT_سیٹ_TYPE:
    واپس "FTP: could not set file type";

  حالت CURLE_PARTIAL_FILE:
    واپس "Transferred a partial file";

  حالت CURLE_FTP_COULDNT_RETR_FILE:
    واپس "FTP: could not retrieve (RETR failed) the specified file";

  حالت CURLE_QUOTE_ERROR:
    واپس "Quote command returned error";

  حالت CURLE_HTTP_RETURNED_ERROR:
    واپس "HTTP response code said error";

  حالت CURLE_WRITE_ERROR:
    واپس "Failed writing received data to disk/application";

  حالت CURLE_UPLOAD_FAILED:
    واپس "Upload failed (at start/before it took off)";

  حالت CURLE_READ_ERROR:
    واپس "Failed to open/read local data from file/application";

  حالت CURLE_برآمد_کا_حافظہ:
    واپس "Out of memory";

  حالت CURLE_OPERATION_TIMEDOUT:
    واپس "Timeout was reached";

  حالت CURLE_FTP_پورٹ_FAILED:
    واپس "FTP: command PORT failed";

  حالت CURLE_FTP_COULDNT_USE_REST:
    واپس "FTP: command REST failed";

  حالت CURLE_RANGE_ERROR:
    واپس "Requested range was not delivered by the server";

  حالت CURLE_SSL_CONNECT_ERROR:
    واپس "SSL connect error";

  حالت CURLE_BAD_DOWNLOAD_RESUME:
    واپس "Could not resume download";

  حالت CURLE_FILE_COULDNT_READ_FILE:
    واپس "Could not read a file:// file";

  حالت CURLE_LDAP_CANNOT_BIND:
    واپس "LDAP: cannot bind";

  حالت CURLE_LDAP_تلاش_FAILED:
    واپس "LDAP: search failed";

  حالت CURLE_ABORTED_BY_CALLBACK:
    واپس "Operation was aborted by an application callback";

  حالت CURLE_BAD_FUNCTION_دلیل:
    واپس "A libcurl function was given a bad argument";

  حالت CURLE_INTERFACE_FAILED:
    واپس "Failed binding local connection end";

  حالت CURLE_TOO_MANY_REDIRECTS:
    واپس "Number of redirects hit maximum amount";

  حالت CURLE_UNKNOWN_OPTION:
    واپس "An unknown option was passed in to libcurl";

  حالت CURLE_SETOPT_OPTION_SYNTAX:
    واپس "Malformed option provided in a setopt";

  حالت CURLE_GOT_NOTHING:
    واپس "Server returned nothing (no headers, no data)";

  حالت CURLE_SSL_ENGINE_NOTFOUND:
    واپس "SSL crypto engine not found";

  حالت CURLE_SSL_ENGINE_SETFAILED:
    واپس "Can not set SSL crypto engine as default";

  حالت CURLE_SSL_ENGINE_INITFAILED:
    واپس "Failed to initialize SSL crypto engine";

  حالت CURLE_SEND_ERROR:
    واپس "Failed sending data to the peer";

  حالت CURLE_RECV_ERROR:
    واپس "Failure when receiving data from the peer";

  حالت CURLE_SSL_CERTPROBLEM:
    واپس "Problem with the local SSL certificate";

  حالت CURLE_SSL_CIPHER:
    واپس "Could not use specified SSL cipher";

  حالت CURLE_PEER_FAILED_VERIFICATION:
    واپس "SSL peer certificate or SSH remote key was not OK";

  حالت CURLE_SSL_CACERT_BADFILE:
    واپس "Problem with the SSL CA cert (path? access rights?)";

  حالت CURLE_BAD_CONTENT_ENCODING:
    واپس "Unrecognized or bad HTTP Content or Transfer-Encoding";

  حالت CURLE_FILESIZE_EXCEEDED:
    واپس "Maximum file size exceeded";

  حالت CURLE_USE_SSL_FAILED:
    واپس "Requested SSL level failed";

  حالت CURLE_SSL_SHUTDOWN_FAILED:
    واپس "Failed to shut down the SSL connection";

  حالت CURLE_SSL_CRL_BADFILE:
    واپس "Failed to load CRL file (path? access rights?, format?)";

  حالت CURLE_SSL_ISSUER_ERROR:
    واپس "Issuer check against peer certificate failed";

  حالت CURLE_SEND_FAIL_REWIND:
    واپس "Send failed since rewinding of the data stream failed";

  حالت CURLE_LOGIN_DENIED:
    واپس "Login denied";

  حالت CURLE_TFTP_NOTFOUND:
    واپس "TFTP: File Not Found";

  حالت CURLE_TFTP_PERM:
    واپس "TFTP: Access Violation";

  حالت CURLE_REMOTE_DISK_بھرا:
    واپس "Disk full or allocation exceeded";

  حالت CURLE_TFTP_ILLEGAL:
    واپس "TFTP: Illegal operation";

  حالت CURLE_TFTP_UNKNOWNID:
    واپس "TFTP: Unknown transfer ID";

  حالت CURLE_REMOTE_FILE_EXISTS:
    واپس "Remote file already exists";

  حالت CURLE_TFTP_NOSUCHUSER:
    واپس "TFTP: No such user";

  حالت CURLE_REMOTE_FILE_NOT_ملا:
    واپس "Remote file not found";

  حالت CURLE_SSH:
    واپس "Error in the SSH layer";

  حالت CURLE_AGAIN:
    واپس "Socket not ready for send/recv";

  حالت CURLE_RTSP_CSEQ_ERROR:
    واپس "RTSP CSeq mismatch or invalid CSeq";

  حالت CURLE_RTSP_SESSION_ERROR:
    واپس "RTSP session error";

  حالت CURLE_FTP_BAD_FILE_LIST:
    واپس "Unable to parse FTP file list";

  حالت CURLE_CHUNK_FAILED:
    واپس "Chunk callback failed";

  حالت CURLE_NO_CONNECTION_دستیاب:
    واپس "The max connection limit is reached";

  حالت CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    واپس "SSL public key does not match pinned public key";

  حالت CURLE_SSL_INVALIDCERTSTATUS:
    واپس "SSL server certificate status verification FAILED";

  حالت CURLE_HTTP2_دھارا:
    واپس "Stream error in the HTTP/2 framing layer";

  حالت CURLE_RECURSIVE_API_CALL:
    واپس "API function called from within callback";

  حالت CURLE_AUTH_ERROR:
    واپس "An authentication function returned an error";

  حالت CURLE_HTTP3:
    واپس "HTTP/3 error";

  حالت CURLE_QUIC_CONNECT_ERROR:
    واپس "QUIC connection error";

  حالت CURLE_PROXY:
    واپس "proxy handshake error";

  حالت CURLE_SSL_CLIENTCERT:
    واپس "SSL Client Certificate required";

  حالت CURLE_UNRECOVERABLE_POLL:
    واپس "Unrecoverable error in select/poll";

  حالت CURLE_TOO_LARGE:
    واپس "A value or data field grew larger than allowed";

  حالت CURLE_ECH_REQUIRED:
    واپس "ECH attempted but failed";

    /* error codes not used by current libcurl */
  طےشدہ:
    توڑو;
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
  واپس "Unknown error";
#ورنہ
  اگر(!error)
    واپس "No error";
  ورنہ
    واپس "Error";
#اگر_ختم
}

ثابت حرف *curl_multi_strerror(CURLMcode error)
{
#اگر_تعریف_شدہ CURLVERBOSE
  انتخاب(error) {
  حالت CURLM_CALL_MULTI_PERFORM:
    واپس "Please call curl_multi_perform() soon";

  حالت CURLM_OK:
    واپس "No error";

  حالت CURLM_BAD_HANDLE:
    واپس "Invalid multi handle";

  حالت CURLM_BAD_EASY_HANDLE:
    واپس "Invalid easy handle";

  حالت CURLM_برآمد_کا_حافظہ:
    واپس "Out of memory";

  حالت CURLM_اندرونی_ERROR:
    واپس "Internal error";

  حالت CURLM_BAD_SOCKET:
    واپس "Invalid socket argument";

  حالت CURLM_UNKNOWN_OPTION:
    واپس "Unknown option";

  حالت CURLM_ADDED_ALREADY:
    واپس "The easy handle is already added to a multi handle";

  حالت CURLM_RECURSIVE_API_CALL:
    واپس "API function called from within callback";

  حالت CURLM_WAKEUP_FAILURE:
    واپس "Wakeup is unavailable or failed";

  حالت CURLM_BAD_FUNCTION_دلیل:
    واپس "A libcurl function was given a bad argument";

  حالت CURLM_ABORTED_BY_CALLBACK:
    واپس "Operation was aborted by an application callback";

  حالت CURLM_UNRECOVERABLE_POLL:
    واپس "Unrecoverable error in select/poll";

  حالت CURLM_آخری:
    توڑو;
  }

  واپس "Unknown error";
#ورنہ
  اگر(error == CURLM_OK)
    واپس "No error";
  ورنہ
    واپس "Error";
#اگر_ختم
}

ثابت حرف *curl_share_strerror(CURLSHcode error)
{
#اگر_تعریف_شدہ CURLVERBOSE
  انتخاب(error) {
  حالت CURLSHE_OK:
    واپس "No error";

  حالت CURLSHE_BAD_OPTION:
    واپس "Unknown share option";

  حالت CURLSHE_اندر_USE:
    واپس "Share currently in use";

  حالت CURLSHE_INVALID:
    واپس "Invalid share handle";

  حالت CURLSHE_NOMEM:
    واپس "Out of memory";

  حالت CURLSHE_NOT_BUILT_اندر:
    واپس "Feature not enabled in this library";

  حالت CURLSHE_آخری:
    توڑو;
  }

  واپس "CURLSHcode unknown";
#ورنہ
  اگر(error == CURLSHE_OK)
    واپس "No error";
  ورنہ
    واپس "Error";
#اگر_ختم
}

ثابت حرف *curl_url_strerror(CURLUcode error)
{
#اگر_تعریف_شدہ CURLVERBOSE
  انتخاب(error) {
  حالت CURLUE_OK:
    واپس "No error";

  حالت CURLUE_BAD_HANDLE:
    واپس "An invalid CURLU pointer was passed as argument";

  حالت CURLUE_BAD_PARTPOINTER:
    واپس "An invalid 'part' argument was passed as argument";

  حالت CURLUE_MALFORMED_داخلہ:
    واپس "Malformed input to a URL function";

  حالت CURLUE_BAD_پورٹ_نمبر:
    واپس "Port number was not a decimal number between 0 and 65535";

  حالت CURLUE_UNSUPPORTED_SCHEME:
    واپس "Unsupported URL scheme";

  حالت CURLUE_URLDECODE:
    واپس "URL decode error, most likely because of rubbish in the input";

  حالت CURLUE_برآمد_کا_حافظہ:
    واپس "A memory function failed";

  حالت CURLUE_USER_NOT_ALLOWED:
    واپس "Credentials was passed in the URL when prohibited";

  حالت CURLUE_UNKNOWN_PART:
    واپس "An unknown part ID was passed to a URL API function";

  حالت CURLUE_NO_SCHEME:
    واپس "No scheme present";

  حالت CURLUE_NO_USER:
    واپس "No user present";

  حالت CURLUE_NO_PASSWORD:
    واپس "No password present";

  حالت CURLUE_NO_OPTIONS:
    واپس "No options present";

  حالت CURLUE_NO_HOST:
    واپس "No host present";

  حالت CURLUE_NO_پورٹ:
    واپس "No port number present";

  حالت CURLUE_NO_QUERY:
    واپس "No query present";

  حالت CURLUE_NO_FRAGMENT:
    واپس "No fragment present";

  حالت CURLUE_NO_ZONEID:
    واپس "No zoneid present";

  حالت CURLUE_BAD_LOGIN:
    واپس "Bad login";

  حالت CURLUE_BAD_IPV6:
    واپس "Bad IPv6 address";

  حالت CURLUE_BAD_HOSTNAME:
    واپس "Bad hostname";

  حالت CURLUE_BAD_FILE_URL:
    واپس "Bad file:// URL";

  حالت CURLUE_BAD_SLASHES:
    واپس "Unsupported number of slashes following scheme";

  حالت CURLUE_BAD_SCHEME:
    واپس "Bad scheme";

  حالت CURLUE_BAD_راستہ:
    واپس "Bad path";

  حالت CURLUE_BAD_FRAGMENT:
    واپس "Bad fragment";

  حالت CURLUE_BAD_QUERY:
    واپس "Bad query";

  حالت CURLUE_BAD_PASSWORD:
    واپس "Bad password";

  حالت CURLUE_BAD_USER:
    واپس "Bad user";

  حالت CURLUE_LACKS_IDN:
    واپس "libcurl lacks IDN support";

  حالت CURLUE_TOO_LARGE:
    واپس "A value or data field is larger than allowed";

  حالت CURLUE_BACKSLASH:
    واپس "Found a backslash where a forward slash was expected";

  حالت CURLUE_آخری:
    توڑو;
  }

  واپس "CURLUcode unknown";
#ورنہ
  اگر(error == CURLUE_OK)
    واپس "No error";
  ورنہ
    واپس "Error";
#اگر_ختم
}

#اگر_تعریف_شدہ USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ثابت حرف *Curl_sspi_strerror(SECURITY_حالت err, حرف *buf, size_t buflen)
{
#اگر_تعریف_شدہ _WIN32
  DWORD old_win_err = Get_آخری_Error();
#اگر_ختم
  صحیح old_errno = errno;
  VERBOSE(ثابت حرف *txt);

  اگر(!buflen)
    واپس تہی;

  *buf = '\0';

#اگر_تعریف_شدہ CURLVERBOSE
  انتخاب(err) {
  حالت SEC_E_OK:
    txt = "No error";
    توڑو;
#تعریف_کرو SEC2TXT(sec) حالت sec: txt = #sec; توڑو
  SEC2TXT(CRYPT_E_REVOKED);
  SEC2TXT(CRYPT_E_NO_REVOCATION_DLL);
  SEC2TXT(CRYPT_E_NO_REVOCATION_CHECK);
  SEC2TXT(CRYPT_E_REVOCATION_OFFLINE);
  SEC2TXT(CRYPT_E_NOT_اندر_REVOCATION_DATABASE);
  SEC2TXT(SEC_E_ALGORITHM_MISMATCH);
  SEC2TXT(SEC_E_BAD_BINDINGS);
  SEC2TXT(SEC_E_BAD_PKGID);
  SEC2TXT(SEC_E_بفر_TOO_SMALL);
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
  SEC2TXT(SEC_E_ILLEGAL_پیغام);
  SEC2TXT(SEC_E_INCOMPLETE_CREDENTIALS);
  SEC2TXT(SEC_E_INCOMPLETE_پیغام);
  SEC2TXT(SEC_E_INSUFFICIENT_حافظہ);
  SEC2TXT(SEC_E_اندرونی_ERROR);
  SEC2TXT(SEC_E_INVALID_HANDLE);
  SEC2TXT(SEC_E_INVALID_PARAMETER);
  SEC2TXT(SEC_E_INVALID_TOKEN);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED_KDC);
  SEC2TXT(SEC_E_KDC_CERT_EXPIRED);
  SEC2TXT(SEC_E_KDC_CERT_REVOKED);
  SEC2TXT(SEC_E_KDC_INVALID_REQUEST);
  SEC2TXT(SEC_E_KDC_UNABLE_مقصد_REFER);
  SEC2TXT(SEC_E_KDC_UNKNOWN_ETYPE);
  SEC2TXT(SEC_E_LOGON_DENIED);
  SEC2TXT(SEC_E_MAX_REFERRALS_EXCEEDED);
  SEC2TXT(SEC_E_پیغام_ALTERED);
  SEC2TXT(SEC_E_مضرب_ACCOUNTS);
  SEC2TXT(SEC_E_MUST_BE_KDC);
  SEC2TXT(SEC_E_NOT_OWNER);
  SEC2TXT(SEC_E_NO_AUTHENTICATING_AUTHORITY);
  SEC2TXT(SEC_E_NO_CREDENTIALS);
  SEC2TXT(SEC_E_NO_IMPERSONATION);
  SEC2TXT(SEC_E_NO_IP_پتے);
  SEC2TXT(SEC_E_NO_KERB_کلید);
  SEC2TXT(SEC_E_NO_PA_ڈیٹا);
  SEC2TXT(SEC_E_NO_S4U_PROT_SUPPORT);
  SEC2TXT(SEC_E_NO_TGT_جواب);
  SEC2TXT(SEC_E_برآمد_کا_SEQUENCE);
  SEC2TXT(SEC_E_PKINIT_CLIENT_FAILURE);
  SEC2TXT(SEC_E_PKINIT_NAME_MISMATCH);
  SEC2TXT(SEC_E_POLICY_NLTM_ONLY);
  SEC2TXT(SEC_E_QOP_NOT_SUPPORTED);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_C);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_KDC);
  SEC2TXT(SEC_E_SECPKG_NOT_ملا);
  SEC2TXT(SEC_E_SECURITY_QOS_FAILED);
  SEC2TXT(SEC_E_SHUTDOWN_اندر_پیش_رفت);
  SEC2TXT(SEC_E_SMARTCARD_CERT_EXPIRED);
  SEC2TXT(SEC_E_SMARTCARD_CERT_REVOKED);
  SEC2TXT(SEC_E_SMARTCARD_LOGON_REQUIRED);
  SEC2TXT(SEC_E_STRONG_CRYPTO_NOT_SUPPORTED);
  SEC2TXT(SEC_E_ہدف_UNKNOWN);
  SEC2TXT(SEC_E_وقت_SKEW);
  SEC2TXT(SEC_E_TOO_MANY_PRINCIPALS);
  SEC2TXT(SEC_E_UNFINISHED_CONTEXT_DELETED);
  SEC2TXT(SEC_E_UNKNOWN_CREDENTIALS);
  SEC2TXT(SEC_E_UNSUPPORTED_FUNCTION);
  SEC2TXT(SEC_E_UNSUPPORTED_PREAUTH);
  SEC2TXT(SEC_E_UNTRUSTED_جڑ);
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
  طےشدہ:
    txt = "Unknown error";
  }

  اگر(err == SEC_E_ILLEGAL_پیغام) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (غیرعلامتی طویل)err);
  }
  ورنہ {
    حرف msgbuf[256];
    اگر(curlx_get_winapi_error((DWORD)err, msgbuf, حجم(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (غیرعلامتی طویل)err,
                     msgbuf);
    ورنہ
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (غیرعلامتی طویل)err);
  }
#ورنہ /* CURLVERBOSE */
  اگر(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  ورنہ
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#اگر_ختم

  اگر(errno != old_errno)
    errno = old_errno;

#اگر_تعریف_شدہ _WIN32
  اگر(old_win_err != Get_آخری_Error())
    سیٹ_آخری_Error(old_win_err);
#اگر_ختم

  واپس buf;
}
#اگر_ختم /* USE_WINDOWS_SSPI */
