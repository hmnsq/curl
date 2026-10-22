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
    返回 "No error";

  case CURLE_UNSUPPORTED_PROTOCOL:
    返回 "Unsupported protocol";

  case CURLE_FAILED_INIT:
    返回 "Failed initialization";

  case CURLE_URL_MALFORMAT:
    返回 "URL using bad/illegal format or missing URL";

  case CURLE_NOT_BUILT_IN:
    返回 "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  case CURLE_COULDNT_RESOLVE_PROXY:
    返回 "Could not resolve proxy name";

  case CURLE_COULDNT_RESOLVE_HOST:
    返回 "Could not resolve hostname";

  case CURLE_COULDNT_CONNECT:
    返回 "Could not connect to server";

  case CURLE_WEIRD_SERVER_REPLY:
    返回 "Weird server reply";

  case CURLE_REMOTE_ACCESS_DENIED:
    返回 "Access denied to remote resource";

  case CURLE_FTP_ACCEPT_FAILED:
    返回 "FTP: The server failed to connect to data port";

  case CURLE_FTP_ACCEPT_TIMEOUT:
    返回 "FTP: Accepting server connect has timed out";

  case CURLE_FTP_PRET_FAILED:
    返回 "FTP: The server did not accept the PRET command.";

  case CURLE_FTP_WEIRD_PASS_REPLY:
    返回 "FTP: unknown PASS reply";

  case CURLE_FTP_WEIRD_PASV_REPLY:
    返回 "FTP: unknown PASV reply";

  case CURLE_FTP_WEIRD_227_FORMAT:
    返回 "FTP: unknown 227 response format";

  case CURLE_FTP_CANT_GET_HOST:
    返回 "FTP: cannot figure out the host in the PASV response";

  case CURLE_HTTP2:
    返回 "Error in the HTTP2 framing layer";

  case CURLE_FTP_COULDNT_SET_TYPE:
    返回 "FTP: could not set file type";

  case CURLE_PARTIAL_FILE:
    返回 "Transferred a partial file";

  case CURLE_FTP_COULDNT_RETR_FILE:
    返回 "FTP: could not retrieve (RETR failed) the specified file";

  case CURLE_QUOTE_ERROR:
    返回 "Quote command returned error";

  case CURLE_HTTP_RETURNED_ERROR:
    返回 "HTTP response code said error";

  case CURLE_WRITE_ERROR:
    返回 "Failed writing received data to disk/application";

  case CURLE_UPLOAD_FAILED:
    返回 "Upload failed (at start/before it took off)";

  case CURLE_READ_ERROR:
    返回 "Failed to open/read local data from file/application";

  case CURLE_OUT_OF_MEMORY:
    返回 "Out of memory";

  case CURLE_OPERATION_TIMEDOUT:
    返回 "Timeout was reached";

  case CURLE_FTP_PORT_FAILED:
    返回 "FTP: command PORT failed";

  case CURLE_FTP_COULDNT_USE_REST:
    返回 "FTP: command REST failed";

  case CURLE_RANGE_ERROR:
    返回 "Requested range was not delivered by the server";

  case CURLE_SSL_CONNECT_ERROR:
    返回 "SSL connect error";

  case CURLE_BAD_DOWNLOAD_RESUME:
    返回 "Could not resume download";

  case CURLE_FILE_COULDNT_READ_FILE:
    返回 "Could not read a file:// file";

  case CURLE_LDAP_CANNOT_BIND:
    返回 "LDAP: cannot bind";

  case CURLE_LDAP_SEARCH_FAILED:
    返回 "LDAP: search failed";

  case CURLE_ABORTED_BY_CALLBACK:
    返回 "Operation was aborted by an application callback";

  case CURLE_BAD_FUNCTION_ARGUMENT:
    返回 "A libcurl function was given a bad argument";

  case CURLE_INTERFACE_FAILED:
    返回 "Failed binding local connection end";

  case CURLE_TOO_MANY_REDIRECTS:
    返回 "Number of redirects hit maximum amount";

  case CURLE_UNKNOWN_OPTION:
    返回 "An unknown option was passed in to libcurl";

  case CURLE_SETOPT_OPTION_SYNTAX:
    返回 "Malformed option provided in a setopt";

  case CURLE_GOT_NOTHING:
    返回 "Server returned nothing (no headers, no data)";

  case CURLE_SSL_ENGINE_NOTFOUND:
    返回 "SSL crypto engine not found";

  case CURLE_SSL_ENGINE_SETFAILED:
    返回 "Can not set SSL crypto engine as default";

  case CURLE_SSL_ENGINE_INITFAILED:
    返回 "Failed to initialize SSL crypto engine";

  case CURLE_SEND_ERROR:
    返回 "Failed sending data to the peer";

  case CURLE_RECV_ERROR:
    返回 "Failure when receiving data from the peer";

  case CURLE_SSL_CERTPROBLEM:
    返回 "Problem with the local SSL certificate";

  case CURLE_SSL_CIPHER:
    返回 "Could not use specified SSL cipher";

  case CURLE_PEER_FAILED_VERIFICATION:
    返回 "SSL peer certificate or SSH remote key was not OK";

  case CURLE_SSL_CACERT_BADFILE:
    返回 "Problem with the SSL CA cert (path? access rights?)";

  case CURLE_BAD_CONTENT_ENCODING:
    返回 "Unrecognized or bad HTTP Content or Transfer-Encoding";

  case CURLE_FILESIZE_EXCEEDED:
    返回 "Maximum file size exceeded";

  case CURLE_USE_SSL_FAILED:
    返回 "Requested SSL level failed";

  case CURLE_SSL_SHUTDOWN_FAILED:
    返回 "Failed to shut down the SSL connection";

  case CURLE_SSL_CRL_BADFILE:
    返回 "Failed to load CRL file (path? access rights?, format?)";

  case CURLE_SSL_ISSUER_ERROR:
    返回 "Issuer check against peer certificate failed";

  case CURLE_SEND_FAIL_REWIND:
    返回 "Send failed since rewinding of the data stream failed";

  case CURLE_LOGIN_DENIED:
    返回 "Login denied";

  case CURLE_TFTP_NOTFOUND:
    返回 "TFTP: File Not Found";

  case CURLE_TFTP_PERM:
    返回 "TFTP: Access Violation";

  case CURLE_REMOTE_DISK_FULL:
    返回 "Disk full or allocation exceeded";

  case CURLE_TFTP_ILLEGAL:
    返回 "TFTP: Illegal operation";

  case CURLE_TFTP_UNKNOWNID:
    返回 "TFTP: Unknown transfer ID";

  case CURLE_REMOTE_FILE_EXISTS:
    返回 "Remote file already exists";

  case CURLE_TFTP_NOSUCHUSER:
    返回 "TFTP: No such user";

  case CURLE_REMOTE_FILE_NOT_FOUND:
    返回 "Remote file not found";

  case CURLE_SSH:
    返回 "Error in the SSH layer";

  case CURLE_AGAIN:
    返回 "Socket not ready for send/recv";

  case CURLE_RTSP_CSEQ_ERROR:
    返回 "RTSP CSeq mismatch or invalid CSeq";

  case CURLE_RTSP_SESSION_ERROR:
    返回 "RTSP session error";

  case CURLE_FTP_BAD_FILE_LIST:
    返回 "Unable to parse FTP file list";

  case CURLE_CHUNK_FAILED:
    返回 "Chunk callback failed";

  case CURLE_NO_CONNECTION_AVAILABLE:
    返回 "The max connection limit is reached";

  case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    返回 "SSL public key does not match pinned public key";

  case CURLE_SSL_INVALIDCERTSTATUS:
    返回 "SSL server certificate status verification FAILED";

  case CURLE_HTTP2_STREAM:
    返回 "Stream error in the HTTP/2 framing layer";

  case CURLE_RECURSIVE_API_CALL:
    返回 "API function called from within callback";

  case CURLE_AUTH_ERROR:
    返回 "An authentication function returned an error";

  case CURLE_HTTP3:
    返回 "HTTP/3 error";

  case CURLE_QUIC_CONNECT_ERROR:
    返回 "QUIC connection error";

  case CURLE_PROXY:
    返回 "proxy handshake error";

  case CURLE_SSL_CLIENTCERT:
    返回 "SSL Client Certificate required";

  case CURLE_UNRECOVERABLE_POLL:
    返回 "Unrecoverable error in select/poll";

  case CURLE_TOO_LARGE:
    返回 "A value or data field grew larger than allowed";

  case CURLE_ECH_REQUIRED:
    返回 "ECH attempted but failed";

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
  返回 "Unknown error";
#否则
  如果(!error)
    返回 "No error";
  否则
    返回 "Error";
#endif
}

const char *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLM_CALL_MULTI_PERFORM:
    返回 "Please call curl_multi_perform() soon";

  case CURLM_OK:
    返回 "No error";

  case CURLM_BAD_HANDLE:
    返回 "Invalid multi handle";

  case CURLM_BAD_EASY_HANDLE:
    返回 "Invalid easy handle";

  case CURLM_OUT_OF_MEMORY:
    返回 "Out of memory";

  case CURLM_INTERNAL_ERROR:
    返回 "Internal error";

  case CURLM_BAD_SOCKET:
    返回 "Invalid socket argument";

  case CURLM_UNKNOWN_OPTION:
    返回 "Unknown option";

  case CURLM_ADDED_ALREADY:
    返回 "The easy handle is already added to a multi handle";

  case CURLM_RECURSIVE_API_CALL:
    返回 "API function called from within callback";

  case CURLM_WAKEUP_FAILURE:
    返回 "Wakeup is unavailable or failed";

  case CURLM_BAD_FUNCTION_ARGUMENT:
    返回 "A libcurl function was given a bad argument";

  case CURLM_ABORTED_BY_CALLBACK:
    返回 "Operation was aborted by an application callback";

  case CURLM_UNRECOVERABLE_POLL:
    返回 "Unrecoverable error in select/poll";

  case CURLM_LAST:
    break;
  }

  返回 "Unknown error";
#否则
  如果(error == CURLM_OK)
    返回 "No error";
  否则
    返回 "Error";
#endif
}

const char *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLSHE_OK:
    返回 "No error";

  case CURLSHE_BAD_OPTION:
    返回 "Unknown share option";

  case CURLSHE_IN_USE:
    返回 "Share currently in use";

  case CURLSHE_INVALID:
    返回 "Invalid share handle";

  case CURLSHE_NOMEM:
    返回 "Out of memory";

  case CURLSHE_NOT_BUILT_IN:
    返回 "Feature not enabled in this library";

  case CURLSHE_LAST:
    break;
  }

  返回 "CURLSHcode unknown";
#否则
  如果(error == CURLSHE_OK)
    返回 "No error";
  否则
    返回 "Error";
#endif
}

const char *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLUE_OK:
    返回 "No error";

  case CURLUE_BAD_HANDLE:
    返回 "An invalid CURLU pointer was passed as argument";

  case CURLUE_BAD_PARTPOINTER:
    返回 "An invalid 'part' argument was passed as argument";

  case CURLUE_MALFORMED_INPUT:
    返回 "Malformed input to a URL function";

  case CURLUE_BAD_PORT_NUMBER:
    返回 "Port number was not a decimal number between 0 and 65535";

  case CURLUE_UNSUPPORTED_SCHEME:
    返回 "Unsupported URL scheme";

  case CURLUE_URLDECODE:
    返回 "URL decode error, most likely because of rubbish in the input";

  case CURLUE_OUT_OF_MEMORY:
    返回 "A memory function failed";

  case CURLUE_USER_NOT_ALLOWED:
    返回 "Credentials was passed in the URL when prohibited";

  case CURLUE_UNKNOWN_PART:
    返回 "An unknown part ID was passed to a URL API function";

  case CURLUE_NO_SCHEME:
    返回 "No scheme present";

  case CURLUE_NO_USER:
    返回 "No user present";

  case CURLUE_NO_PASSWORD:
    返回 "No password present";

  case CURLUE_NO_OPTIONS:
    返回 "No options present";

  case CURLUE_NO_HOST:
    返回 "No host present";

  case CURLUE_NO_PORT:
    返回 "No port number present";

  case CURLUE_NO_QUERY:
    返回 "No query present";

  case CURLUE_NO_FRAGMENT:
    返回 "No fragment present";

  case CURLUE_NO_ZONEID:
    返回 "No zoneid present";

  case CURLUE_BAD_LOGIN:
    返回 "Bad login";

  case CURLUE_BAD_IPV6:
    返回 "Bad IPv6 address";

  case CURLUE_BAD_HOSTNAME:
    返回 "Bad hostname";

  case CURLUE_BAD_FILE_URL:
    返回 "Bad file:// URL";

  case CURLUE_BAD_SLASHES:
    返回 "Unsupported number of slashes following scheme";

  case CURLUE_BAD_SCHEME:
    返回 "Bad scheme";

  case CURLUE_BAD_PATH:
    返回 "Bad path";

  case CURLUE_BAD_FRAGMENT:
    返回 "Bad fragment";

  case CURLUE_BAD_QUERY:
    返回 "Bad query";

  case CURLUE_BAD_PASSWORD:
    返回 "Bad password";

  case CURLUE_BAD_USER:
    返回 "Bad user";

  case CURLUE_LACKS_IDN:
    返回 "libcurl lacks IDN support";

  case CURLUE_TOO_LARGE:
    返回 "A value or data field is larger than allowed";

  case CURLUE_BACKSLASH:
    返回 "Found a backslash where a forward slash was expected";

  case CURLUE_LAST:
    break;
  }

  返回 "CURLUcode unknown";
#否则
  如果(error == CURLUE_OK)
    返回 "No error";
  否则
    返回 "Error";
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
  整数 old_errno = errno;
  VERBOSE(const char *txt);

  如果(!buflen)
    返回 NULL;

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

  如果(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  否则 {
    char msgbuf[256];
    如果(curlx_get_winapi_error((DWORD)err, msgbuf, sizeof(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    否则
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#否则 /* CURLVERBOSE */
  如果(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  否则
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  如果(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  如果(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  返回 buf;
}
#endif /* USE_WINDOWS_SSPI */
