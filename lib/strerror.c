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
#include "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#include "curl_sspi.h"
#endif

#include "curlx/winapi.h"
#include "strerror.h"
#include "curlx/strcopy.h"

const char *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLE_OK:
    반환 "No error";

  case CURLE_UNSUPPORTED_PROTOCOL:
    반환 "Unsupported protocol";

  case CURLE_FAILED_INIT:
    반환 "Failed initialization";

  case CURLE_URL_MALFORMAT:
    반환 "URL using bad/illegal format or missing URL";

  case CURLE_NOT_BUILT_IN:
    반환 "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  case CURLE_COULDNT_RESOLVE_PROXY:
    반환 "Could not resolve proxy name";

  case CURLE_COULDNT_RESOLVE_HOST:
    반환 "Could not resolve hostname";

  case CURLE_COULDNT_CONNECT:
    반환 "Could not connect to server";

  case CURLE_WEIRD_SERVER_REPLY:
    반환 "Weird server reply";

  case CURLE_REMOTE_ACCESS_DENIED:
    반환 "Access denied to remote resource";

  case CURLE_FTP_ACCEPT_FAILED:
    반환 "FTP: The server failed to connect to data port";

  case CURLE_FTP_ACCEPT_TIMEOUT:
    반환 "FTP: Accepting server connect has timed out";

  case CURLE_FTP_PRET_FAILED:
    반환 "FTP: The server did not accept the PRET command.";

  case CURLE_FTP_WEIRD_PASS_REPLY:
    반환 "FTP: unknown PASS reply";

  case CURLE_FTP_WEIRD_PASV_REPLY:
    반환 "FTP: unknown PASV reply";

  case CURLE_FTP_WEIRD_227_FORMAT:
    반환 "FTP: unknown 227 response format";

  case CURLE_FTP_CANT_GET_HOST:
    반환 "FTP: cannot figure out the host in the PASV response";

  case CURLE_HTTP2:
    반환 "Error in the HTTP2 framing layer";

  case CURLE_FTP_COULDNT_SET_TYPE:
    반환 "FTP: could not set file type";

  case CURLE_PARTIAL_FILE:
    반환 "Transferred a partial file";

  case CURLE_FTP_COULDNT_RETR_FILE:
    반환 "FTP: could not retrieve (RETR failed) the specified file";

  case CURLE_QUOTE_ERROR:
    반환 "Quote command returned error";

  case CURLE_HTTP_RETURNED_ERROR:
    반환 "HTTP response code said error";

  case CURLE_WRITE_ERROR:
    반환 "Failed writing received data to disk/application";

  case CURLE_UPLOAD_FAILED:
    반환 "Upload failed (at start/before it took off)";

  case CURLE_READ_ERROR:
    반환 "Failed to open/read local data from file/application";

  case CURLE_OUT_OF_MEMORY:
    반환 "Out of memory";

  case CURLE_OPERATION_TIMEDOUT:
    반환 "Timeout was reached";

  case CURLE_FTP_PORT_FAILED:
    반환 "FTP: command PORT failed";

  case CURLE_FTP_COULDNT_USE_REST:
    반환 "FTP: command REST failed";

  case CURLE_RANGE_ERROR:
    반환 "Requested range was not delivered by the server";

  case CURLE_SSL_CONNECT_ERROR:
    반환 "SSL connect error";

  case CURLE_BAD_DOWNLOAD_RESUME:
    반환 "Could not resume download";

  case CURLE_FILE_COULDNT_READ_FILE:
    반환 "Could not read a file:// file";

  case CURLE_LDAP_CANNOT_BIND:
    반환 "LDAP: cannot bind";

  case CURLE_LDAP_SEARCH_FAILED:
    반환 "LDAP: search failed";

  case CURLE_ABORTED_BY_CALLBACK:
    반환 "Operation was aborted by an application callback";

  case CURLE_BAD_FUNCTION_ARGUMENT:
    반환 "A libcurl function was given a bad argument";

  case CURLE_INTERFACE_FAILED:
    반환 "Failed binding local connection end";

  case CURLE_TOO_MANY_REDIRECTS:
    반환 "Number of redirects hit maximum amount";

  case CURLE_UNKNOWN_OPTION:
    반환 "An unknown option was passed in to libcurl";

  case CURLE_SETOPT_OPTION_SYNTAX:
    반환 "Malformed option provided in a setopt";

  case CURLE_GOT_NOTHING:
    반환 "Server returned nothing (no headers, no data)";

  case CURLE_SSL_ENGINE_NOTFOUND:
    반환 "SSL crypto engine not found";

  case CURLE_SSL_ENGINE_SETFAILED:
    반환 "Can not set SSL crypto engine as default";

  case CURLE_SSL_ENGINE_INITFAILED:
    반환 "Failed to initialize SSL crypto engine";

  case CURLE_SEND_ERROR:
    반환 "Failed sending data to the peer";

  case CURLE_RECV_ERROR:
    반환 "Failure when receiving data from the peer";

  case CURLE_SSL_CERTPROBLEM:
    반환 "Problem with the local SSL certificate";

  case CURLE_SSL_CIPHER:
    반환 "Could not use specified SSL cipher";

  case CURLE_PEER_FAILED_VERIFICATION:
    반환 "SSL peer certificate or SSH remote key was not OK";

  case CURLE_SSL_CACERT_BADFILE:
    반환 "Problem with the SSL CA cert (path? access rights?)";

  case CURLE_BAD_CONTENT_ENCODING:
    반환 "Unrecognized or bad HTTP Content or Transfer-Encoding";

  case CURLE_FILESIZE_EXCEEDED:
    반환 "Maximum file size exceeded";

  case CURLE_USE_SSL_FAILED:
    반환 "Requested SSL level failed";

  case CURLE_SSL_SHUTDOWN_FAILED:
    반환 "Failed to shut down the SSL connection";

  case CURLE_SSL_CRL_BADFILE:
    반환 "Failed to load CRL file (path? access rights?, format?)";

  case CURLE_SSL_ISSUER_ERROR:
    반환 "Issuer check against peer certificate failed";

  case CURLE_SEND_FAIL_REWIND:
    반환 "Send failed since rewinding of the data stream failed";

  case CURLE_LOGIN_DENIED:
    반환 "Login denied";

  case CURLE_TFTP_NOTFOUND:
    반환 "TFTP: File Not Found";

  case CURLE_TFTP_PERM:
    반환 "TFTP: Access Violation";

  case CURLE_REMOTE_DISK_FULL:
    반환 "Disk full or allocation exceeded";

  case CURLE_TFTP_ILLEGAL:
    반환 "TFTP: Illegal operation";

  case CURLE_TFTP_UNKNOWNID:
    반환 "TFTP: Unknown transfer ID";

  case CURLE_REMOTE_FILE_EXISTS:
    반환 "Remote file already exists";

  case CURLE_TFTP_NOSUCHUSER:
    반환 "TFTP: No such user";

  case CURLE_REMOTE_FILE_NOT_FOUND:
    반환 "Remote file not found";

  case CURLE_SSH:
    반환 "Error in the SSH layer";

  case CURLE_AGAIN:
    반환 "Socket not ready for send/recv";

  case CURLE_RTSP_CSEQ_ERROR:
    반환 "RTSP CSeq mismatch or invalid CSeq";

  case CURLE_RTSP_SESSION_ERROR:
    반환 "RTSP session error";

  case CURLE_FTP_BAD_FILE_LIST:
    반환 "Unable to parse FTP file list";

  case CURLE_CHUNK_FAILED:
    반환 "Chunk callback failed";

  case CURLE_NO_CONNECTION_AVAILABLE:
    반환 "The max connection limit is reached";

  case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    반환 "SSL public key does not match pinned public key";

  case CURLE_SSL_INVALIDCERTSTATUS:
    반환 "SSL server certificate status verification FAILED";

  case CURLE_HTTP2_STREAM:
    반환 "Stream error in the HTTP/2 framing layer";

  case CURLE_RECURSIVE_API_CALL:
    반환 "API function called from within callback";

  case CURLE_AUTH_ERROR:
    반환 "An authentication function returned an error";

  case CURLE_HTTP3:
    반환 "HTTP/3 error";

  case CURLE_QUIC_CONNECT_ERROR:
    반환 "QUIC connection error";

  case CURLE_PROXY:
    반환 "proxy handshake error";

  case CURLE_SSL_CLIENTCERT:
    반환 "SSL Client Certificate required";

  case CURLE_UNRECOVERABLE_POLL:
    반환 "Unrecoverable error in select/poll";

  case CURLE_TOO_LARGE:
    반환 "A value or data field grew larger than allowed";

  case CURLE_ECH_REQUIRED:
    반환 "ECH attempted but failed";

    /* error codes not used by current libcurl */
  default:
    break;
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
  반환 "Unknown error";
#아니면
  만약(!error)
    반환 "No error";
  아니면
    반환 "Error";
#endif
}

const char *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLM_CALL_MULTI_PERFORM:
    반환 "Please call curl_multi_perform() soon";

  case CURLM_OK:
    반환 "No error";

  case CURLM_BAD_HANDLE:
    반환 "Invalid multi handle";

  case CURLM_BAD_EASY_HANDLE:
    반환 "Invalid easy handle";

  case CURLM_OUT_OF_MEMORY:
    반환 "Out of memory";

  case CURLM_INTERNAL_ERROR:
    반환 "Internal error";

  case CURLM_BAD_SOCKET:
    반환 "Invalid socket argument";

  case CURLM_UNKNOWN_OPTION:
    반환 "Unknown option";

  case CURLM_ADDED_ALREADY:
    반환 "The easy handle is already added to a multi handle";

  case CURLM_RECURSIVE_API_CALL:
    반환 "API function called from within callback";

  case CURLM_WAKEUP_FAILURE:
    반환 "Wakeup is unavailable or failed";

  case CURLM_BAD_FUNCTION_ARGUMENT:
    반환 "A libcurl function was given a bad argument";

  case CURLM_ABORTED_BY_CALLBACK:
    반환 "Operation was aborted by an application callback";

  case CURLM_UNRECOVERABLE_POLL:
    반환 "Unrecoverable error in select/poll";

  case CURLM_LAST:
    break;
  }

  반환 "Unknown error";
#아니면
  만약(error == CURLM_OK)
    반환 "No error";
  아니면
    반환 "Error";
#endif
}

const char *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLSHE_OK:
    반환 "No error";

  case CURLSHE_BAD_OPTION:
    반환 "Unknown share option";

  case CURLSHE_IN_USE:
    반환 "Share currently in use";

  case CURLSHE_INVALID:
    반환 "Invalid share handle";

  case CURLSHE_NOMEM:
    반환 "Out of memory";

  case CURLSHE_NOT_BUILT_IN:
    반환 "Feature not enabled in this library";

  case CURLSHE_LAST:
    break;
  }

  반환 "CURLSHcode unknown";
#아니면
  만약(error == CURLSHE_OK)
    반환 "No error";
  아니면
    반환 "Error";
#endif
}

const char *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLUE_OK:
    반환 "No error";

  case CURLUE_BAD_HANDLE:
    반환 "An invalid CURLU pointer was passed as argument";

  case CURLUE_BAD_PARTPOINTER:
    반환 "An invalid 'part' argument was passed as argument";

  case CURLUE_MALFORMED_INPUT:
    반환 "Malformed input to a URL function";

  case CURLUE_BAD_PORT_NUMBER:
    반환 "Port number was not a decimal number between 0 and 65535";

  case CURLUE_UNSUPPORTED_SCHEME:
    반환 "Unsupported URL scheme";

  case CURLUE_URLDECODE:
    반환 "URL decode error, most likely because of rubbish in the input";

  case CURLUE_OUT_OF_MEMORY:
    반환 "A memory function failed";

  case CURLUE_USER_NOT_ALLOWED:
    반환 "Credentials was passed in the URL when prohibited";

  case CURLUE_UNKNOWN_PART:
    반환 "An unknown part ID was passed to a URL API function";

  case CURLUE_NO_SCHEME:
    반환 "No scheme present";

  case CURLUE_NO_USER:
    반환 "No user present";

  case CURLUE_NO_PASSWORD:
    반환 "No password present";

  case CURLUE_NO_OPTIONS:
    반환 "No options present";

  case CURLUE_NO_HOST:
    반환 "No host present";

  case CURLUE_NO_PORT:
    반환 "No port number present";

  case CURLUE_NO_QUERY:
    반환 "No query present";

  case CURLUE_NO_FRAGMENT:
    반환 "No fragment present";

  case CURLUE_NO_ZONEID:
    반환 "No zoneid present";

  case CURLUE_BAD_LOGIN:
    반환 "Bad login";

  case CURLUE_BAD_IPV6:
    반환 "Bad IPv6 address";

  case CURLUE_BAD_HOSTNAME:
    반환 "Bad hostname";

  case CURLUE_BAD_FILE_URL:
    반환 "Bad file:// URL";

  case CURLUE_BAD_SLASHES:
    반환 "Unsupported number of slashes following scheme";

  case CURLUE_BAD_SCHEME:
    반환 "Bad scheme";

  case CURLUE_BAD_PATH:
    반환 "Bad path";

  case CURLUE_BAD_FRAGMENT:
    반환 "Bad fragment";

  case CURLUE_BAD_QUERY:
    반환 "Bad query";

  case CURLUE_BAD_PASSWORD:
    반환 "Bad password";

  case CURLUE_BAD_USER:
    반환 "Bad user";

  case CURLUE_LACKS_IDN:
    반환 "libcurl lacks IDN support";

  case CURLUE_TOO_LARGE:
    반환 "A value or data field is larger than allowed";

  case CURLUE_BACKSLASH:
    반환 "Found a backslash where a forward slash was expected";

  case CURLUE_LAST:
    break;
  }

  반환 "CURLUcode unknown";
#아니면
  만약(error == CURLUE_OK)
    반환 "No error";
  아니면
    반환 "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
const char *Curl_sspi_strerror(SECURITY_STATUS err, char *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  정수 old_errno = errno;
  VERBOSE(const char *txt);

  만약(!buflen)
    반환 NULL;

  *buf = '\0';

#ifdef CURLVERBOSE
  switch(err) {
  case SEC_E_OK:
    txt = "No error";
    break;
#define SEC2TXT(sec) case sec: txt = #sec; break
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
  SEC2TXT(SEC_E_NO_KERB_KEY);
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
  SEC2TXT(SEC_E_UNTRUSTED_ROOT);
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
  default:
    txt = "Unknown error";
  }

  만약(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  아니면 {
    char msgbuf[256];
    만약(curlx_get_winapi_error((DWORD)err, msgbuf, sizeof(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    아니면
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#아니면 /* CURLVERBOSE */
  만약(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  아니면
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  만약(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  만약(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  반환 buf;
}
#endif /* USE_WINDOWS_SSPI */
