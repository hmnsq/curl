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
    врати "No error";

  case CURLE_UNSUPPORTED_PROTOCOL:
    врати "Unsupported protocol";

  case CURLE_FAILED_INIT:
    врати "Failed initialization";

  case CURLE_URL_MALFORMAT:
    врати "URL using bad/illegal format or missing URL";

  case CURLE_NOT_BUILT_IN:
    врати "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  case CURLE_COULDNT_RESOLVE_PROXY:
    врати "Could not resolve proxy name";

  case CURLE_COULDNT_RESOLVE_HOST:
    врати "Could not resolve hostname";

  case CURLE_COULDNT_CONNECT:
    врати "Could not connect to server";

  case CURLE_WEIRD_SERVER_REPLY:
    врати "Weird server reply";

  case CURLE_REMOTE_ACCESS_DENIED:
    врати "Access denied to remote resource";

  case CURLE_FTP_ACCEPT_FAILED:
    врати "FTP: The server failed to connect to data port";

  case CURLE_FTP_ACCEPT_TIMEOUT:
    врати "FTP: Accepting server connect has timed out";

  case CURLE_FTP_PRET_FAILED:
    врати "FTP: The server did not accept the PRET command.";

  case CURLE_FTP_WEIRD_PASS_REPLY:
    врати "FTP: unknown PASS reply";

  case CURLE_FTP_WEIRD_PASV_REPLY:
    врати "FTP: unknown PASV reply";

  case CURLE_FTP_WEIRD_227_FORMAT:
    врати "FTP: unknown 227 response format";

  case CURLE_FTP_CANT_GET_HOST:
    врати "FTP: cannot figure out the host in the PASV response";

  case CURLE_HTTP2:
    врати "Error in the HTTP2 framing layer";

  case CURLE_FTP_COULDNT_SET_TYPE:
    врати "FTP: could not set file type";

  case CURLE_PARTIAL_FILE:
    врати "Transferred a partial file";

  case CURLE_FTP_COULDNT_RETR_FILE:
    врати "FTP: could not retrieve (RETR failed) the specified file";

  case CURLE_QUOTE_ERROR:
    врати "Quote command returned error";

  case CURLE_HTTP_RETURNED_ERROR:
    врати "HTTP response code said error";

  case CURLE_WRITE_ERROR:
    врати "Failed writing received data to disk/application";

  case CURLE_UPLOAD_FAILED:
    врати "Upload failed (at start/before it took off)";

  case CURLE_READ_ERROR:
    врати "Failed to open/read local data from file/application";

  case CURLE_OUT_OF_MEMORY:
    врати "Out of memory";

  case CURLE_OPERATION_TIMEDOUT:
    врати "Timeout was reached";

  case CURLE_FTP_PORT_FAILED:
    врати "FTP: command PORT failed";

  case CURLE_FTP_COULDNT_USE_REST:
    врати "FTP: command REST failed";

  case CURLE_RANGE_ERROR:
    врати "Requested range was not delivered by the server";

  case CURLE_SSL_CONNECT_ERROR:
    врати "SSL connect error";

  case CURLE_BAD_DOWNLOAD_RESUME:
    врати "Could not resume download";

  case CURLE_FILE_COULDNT_READ_FILE:
    врати "Could not read a file:// file";

  case CURLE_LDAP_CANNOT_BIND:
    врати "LDAP: cannot bind";

  case CURLE_LDAP_SEARCH_FAILED:
    врати "LDAP: search failed";

  case CURLE_ABORTED_BY_CALLBACK:
    врати "Operation was aborted by an application callback";

  case CURLE_BAD_FUNCTION_ARGUMENT:
    врати "A libcurl function was given a bad argument";

  case CURLE_INTERFACE_FAILED:
    врати "Failed binding local connection end";

  case CURLE_TOO_MANY_REDIRECTS:
    врати "Number of redirects hit maximum amount";

  case CURLE_UNKNOWN_OPTION:
    врати "An unknown option was passed in to libcurl";

  case CURLE_SETOPT_OPTION_SYNTAX:
    врати "Malformed option provided in a setopt";

  case CURLE_GOT_NOTHING:
    врати "Server returned nothing (no headers, no data)";

  case CURLE_SSL_ENGINE_NOTFOUND:
    врати "SSL crypto engine not found";

  case CURLE_SSL_ENGINE_SETFAILED:
    врати "Can not set SSL crypto engine as default";

  case CURLE_SSL_ENGINE_INITFAILED:
    врати "Failed to initialize SSL crypto engine";

  case CURLE_SEND_ERROR:
    врати "Failed sending data to the peer";

  case CURLE_RECV_ERROR:
    врати "Failure when receiving data from the peer";

  case CURLE_SSL_CERTPROBLEM:
    врати "Problem with the local SSL certificate";

  case CURLE_SSL_CIPHER:
    врати "Could not use specified SSL cipher";

  case CURLE_PEER_FAILED_VERIFICATION:
    врати "SSL peer certificate or SSH remote key was not OK";

  case CURLE_SSL_CACERT_BADFILE:
    врати "Problem with the SSL CA cert (path? access rights?)";

  case CURLE_BAD_CONTENT_ENCODING:
    врати "Unrecognized or bad HTTP Content or Transfer-Encoding";

  case CURLE_FILESIZE_EXCEEDED:
    врати "Maximum file size exceeded";

  case CURLE_USE_SSL_FAILED:
    врати "Requested SSL level failed";

  case CURLE_SSL_SHUTDOWN_FAILED:
    врати "Failed to shut down the SSL connection";

  case CURLE_SSL_CRL_BADFILE:
    врати "Failed to load CRL file (path? access rights?, format?)";

  case CURLE_SSL_ISSUER_ERROR:
    врати "Issuer check against peer certificate failed";

  case CURLE_SEND_FAIL_REWIND:
    врати "Send failed since rewinding of the data stream failed";

  case CURLE_LOGIN_DENIED:
    врати "Login denied";

  case CURLE_TFTP_NOTFOUND:
    врати "TFTP: File Not Found";

  case CURLE_TFTP_PERM:
    врати "TFTP: Access Violation";

  case CURLE_REMOTE_DISK_FULL:
    врати "Disk full or allocation exceeded";

  case CURLE_TFTP_ILLEGAL:
    врати "TFTP: Illegal operation";

  case CURLE_TFTP_UNKNOWNID:
    врати "TFTP: Unknown transfer ID";

  case CURLE_REMOTE_FILE_EXISTS:
    врати "Remote file already exists";

  case CURLE_TFTP_NOSUCHUSER:
    врати "TFTP: No such user";

  case CURLE_REMOTE_FILE_NOT_FOUND:
    врати "Remote file not found";

  case CURLE_SSH:
    врати "Error in the SSH layer";

  case CURLE_AGAIN:
    врати "Socket not ready for send/recv";

  case CURLE_RTSP_CSEQ_ERROR:
    врати "RTSP CSeq mismatch or invalid CSeq";

  case CURLE_RTSP_SESSION_ERROR:
    врати "RTSP session error";

  case CURLE_FTP_BAD_FILE_LIST:
    врати "Unable to parse FTP file list";

  case CURLE_CHUNK_FAILED:
    врати "Chunk callback failed";

  case CURLE_NO_CONNECTION_AVAILABLE:
    врати "The max connection limit is reached";

  case CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    врати "SSL public key does not match pinned public key";

  case CURLE_SSL_INVALIDCERTSTATUS:
    врати "SSL server certificate status verification FAILED";

  case CURLE_HTTP2_STREAM:
    врати "Stream error in the HTTP/2 framing layer";

  case CURLE_RECURSIVE_API_CALL:
    врати "API function called from within callback";

  case CURLE_AUTH_ERROR:
    врати "An authentication function returned an error";

  case CURLE_HTTP3:
    врати "HTTP/3 error";

  case CURLE_QUIC_CONNECT_ERROR:
    врати "QUIC connection error";

  case CURLE_PROXY:
    врати "proxy handshake error";

  case CURLE_SSL_CLIENTCERT:
    врати "SSL Client Certificate required";

  case CURLE_UNRECOVERABLE_POLL:
    врати "Unrecoverable error in select/poll";

  case CURLE_TOO_LARGE:
    врати "A value or data field grew larger than allowed";

  case CURLE_ECH_REQUIRED:
    врати "ECH attempted but failed";

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
  врати "Unknown error";
#инако
  ако(!error)
    врати "No error";
  инако
    врати "Error";
#endif
}

const char *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLM_CALL_MULTI_PERFORM:
    врати "Please call curl_multi_perform() soon";

  case CURLM_OK:
    врати "No error";

  case CURLM_BAD_HANDLE:
    врати "Invalid multi handle";

  case CURLM_BAD_EASY_HANDLE:
    врати "Invalid easy handle";

  case CURLM_OUT_OF_MEMORY:
    врати "Out of memory";

  case CURLM_INTERNAL_ERROR:
    врати "Internal error";

  case CURLM_BAD_SOCKET:
    врати "Invalid socket argument";

  case CURLM_UNKNOWN_OPTION:
    врати "Unknown option";

  case CURLM_ADDED_ALREADY:
    врати "The easy handle is already added to a multi handle";

  case CURLM_RECURSIVE_API_CALL:
    врати "API function called from within callback";

  case CURLM_WAKEUP_FAILURE:
    врати "Wakeup is unavailable or failed";

  case CURLM_BAD_FUNCTION_ARGUMENT:
    врати "A libcurl function was given a bad argument";

  case CURLM_ABORTED_BY_CALLBACK:
    врати "Operation was aborted by an application callback";

  case CURLM_UNRECOVERABLE_POLL:
    врати "Unrecoverable error in select/poll";

  case CURLM_LAST:
    break;
  }

  врати "Unknown error";
#инако
  ако(error == CURLM_OK)
    врати "No error";
  инако
    врати "Error";
#endif
}

const char *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLSHE_OK:
    врати "No error";

  case CURLSHE_BAD_OPTION:
    врати "Unknown share option";

  case CURLSHE_IN_USE:
    врати "Share currently in use";

  case CURLSHE_INVALID:
    врати "Invalid share handle";

  case CURLSHE_NOMEM:
    врати "Out of memory";

  case CURLSHE_NOT_BUILT_IN:
    врати "Feature not enabled in this library";

  case CURLSHE_LAST:
    break;
  }

  врати "CURLSHcode unknown";
#инако
  ако(error == CURLSHE_OK)
    врати "No error";
  инако
    врати "Error";
#endif
}

const char *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  case CURLUE_OK:
    врати "No error";

  case CURLUE_BAD_HANDLE:
    врати "An invalid CURLU pointer was passed as argument";

  case CURLUE_BAD_PARTPOINTER:
    врати "An invalid 'part' argument was passed as argument";

  case CURLUE_MALFORMED_INPUT:
    врати "Malformed input to a URL function";

  case CURLUE_BAD_PORT_NUMBER:
    врати "Port number was not a decimal number between 0 and 65535";

  case CURLUE_UNSUPPORTED_SCHEME:
    врати "Unsupported URL scheme";

  case CURLUE_URLDECODE:
    врати "URL decode error, most likely because of rubbish in the input";

  case CURLUE_OUT_OF_MEMORY:
    врати "A memory function failed";

  case CURLUE_USER_NOT_ALLOWED:
    врати "Credentials was passed in the URL when prohibited";

  case CURLUE_UNKNOWN_PART:
    врати "An unknown part ID was passed to a URL API function";

  case CURLUE_NO_SCHEME:
    врати "No scheme present";

  case CURLUE_NO_USER:
    врати "No user present";

  case CURLUE_NO_PASSWORD:
    врати "No password present";

  case CURLUE_NO_OPTIONS:
    врати "No options present";

  case CURLUE_NO_HOST:
    врати "No host present";

  case CURLUE_NO_PORT:
    врати "No port number present";

  case CURLUE_NO_QUERY:
    врати "No query present";

  case CURLUE_NO_FRAGMENT:
    врати "No fragment present";

  case CURLUE_NO_ZONEID:
    врати "No zoneid present";

  case CURLUE_BAD_LOGIN:
    врати "Bad login";

  case CURLUE_BAD_IPV6:
    врати "Bad IPv6 address";

  case CURLUE_BAD_HOSTNAME:
    врати "Bad hostname";

  case CURLUE_BAD_FILE_URL:
    врати "Bad file:// URL";

  case CURLUE_BAD_SLASHES:
    врати "Unsupported number of slashes following scheme";

  case CURLUE_BAD_SCHEME:
    врати "Bad scheme";

  case CURLUE_BAD_PATH:
    врати "Bad path";

  case CURLUE_BAD_FRAGMENT:
    врати "Bad fragment";

  case CURLUE_BAD_QUERY:
    врати "Bad query";

  case CURLUE_BAD_PASSWORD:
    врати "Bad password";

  case CURLUE_BAD_USER:
    врати "Bad user";

  case CURLUE_LACKS_IDN:
    врати "libcurl lacks IDN support";

  case CURLUE_TOO_LARGE:
    врати "A value or data field is larger than allowed";

  case CURLUE_BACKSLASH:
    врати "Found a backslash where a forward slash was expected";

  case CURLUE_LAST:
    break;
  }

  врати "CURLUcode unknown";
#инако
  ако(error == CURLUE_OK)
    врати "No error";
  инако
    врати "Error";
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
  цео old_errno = errno;
  VERBOSE(const char *txt);

  ако(!buflen)
    врати NULL;

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

  ако(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  инако {
    char msgbuf[256];
    ако(curlx_get_winapi_error((DWORD)err, msgbuf, sizeof(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    инако
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#инако /* CURLVERBOSE */
  ако(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  инако
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  ако(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  ако(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  врати buf;
}
#endif /* USE_WINDOWS_SSPI */
