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

const var_na *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  pak_sa CURLE_OK:
    ᱨᱩᱣᱟᱹᱲ "No error";

  pak_sa CURLE_UNSUPPORTED_PROTOCOL:
    ᱨᱩᱣᱟᱹᱲ "Unsupported protocol";

  pak_sa CURLE_FAILED_INIT:
    ᱨᱩᱣᱟᱹᱲ "Failed initialization";

  pak_sa CURLE_URL_MALFORMAT:
    ᱨᱩᱣᱟᱹᱲ "URL using bad/illegal format or missing URL";

  pak_sa CURLE_NOT_BUILT_IN:
    ᱨᱩᱣᱟᱹᱲ "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  pak_sa CURLE_COULDNT_RESOLVE_PROXY:
    ᱨᱩᱣᱟᱹᱲ "Could not resolve proxy name";

  pak_sa CURLE_COULDNT_RESOLVE_HOST:
    ᱨᱩᱣᱟᱹᱲ "Could not resolve hostname";

  pak_sa CURLE_COULDNT_CONNECT:
    ᱨᱩᱣᱟᱹᱲ "Could not connect to server";

  pak_sa CURLE_WEIRD_SERVER_REPLY:
    ᱨᱩᱣᱟᱹᱲ "Weird server reply";

  pak_sa CURLE_REMOTE_ACCESS_DENIED:
    ᱨᱩᱣᱟᱹᱲ "Access denied to remote resource";

  pak_sa CURLE_FTP_ACCEPT_FAILED:
    ᱨᱩᱣᱟᱹᱲ "FTP: The server failed to connect to data port";

  pak_sa CURLE_FTP_ACCEPT_TIMEOUT:
    ᱨᱩᱣᱟᱹᱲ "FTP: Accepting server connect has timed out";

  pak_sa CURLE_FTP_PRET_FAILED:
    ᱨᱩᱣᱟᱹᱲ "FTP: The server did not accept the PRET command.";

  pak_sa CURLE_FTP_WEIRD_PASS_REPLY:
    ᱨᱩᱣᱟᱹᱲ "FTP: unknown PASS reply";

  pak_sa CURLE_FTP_WEIRD_PASV_REPLY:
    ᱨᱩᱣᱟᱹᱲ "FTP: unknown PASV reply";

  pak_sa CURLE_FTP_WEIRD_227_FORMAT:
    ᱨᱩᱣᱟᱹᱲ "FTP: unknown 227 response format";

  pak_sa CURLE_FTP_CANT_GET_HOST:
    ᱨᱩᱣᱟᱹᱲ "FTP: cannot figure out the host in the PASV response";

  pak_sa CURLE_HTTP2:
    ᱨᱩᱣᱟᱹᱲ "Error in the HTTP2 framing layer";

  pak_sa CURLE_FTP_COULDNT_SET_TYPE:
    ᱨᱩᱣᱟᱹᱲ "FTP: could not set file type";

  pak_sa CURLE_PARTIAL_FILE:
    ᱨᱩᱣᱟᱹᱲ "Transferred a partial file";

  pak_sa CURLE_FTP_COULDNT_RETR_FILE:
    ᱨᱩᱣᱟᱹᱲ "FTP: could not retrieve (RETR failed) the specified file";

  pak_sa CURLE_QUOTE_ERROR:
    ᱨᱩᱣᱟᱹᱲ "Quote command returned error";

  pak_sa CURLE_HTTP_RETURNED_ERROR:
    ᱨᱩᱣᱟᱹᱲ "HTTP response code said error";

  pak_sa CURLE_WRITE_ERROR:
    ᱨᱩᱣᱟᱹᱲ "Failed writing received data to disk/application";

  pak_sa CURLE_UPLOAD_FAILED:
    ᱨᱩᱣᱟᱹᱲ "Upload failed (at start/before it took off)";

  pak_sa CURLE_READ_ERROR:
    ᱨᱩᱣᱟᱹᱲ "Failed to open/read local data from file/application";

  pak_sa CURLE_OUT_OF_MEMORY:
    ᱨᱩᱣᱟᱹᱲ "Out of memory";

  pak_sa CURLE_OPERATION_TIMEDOUT:
    ᱨᱩᱣᱟᱹᱲ "Timeout was reached";

  pak_sa CURLE_FTP_PORT_FAILED:
    ᱨᱩᱣᱟᱹᱲ "FTP: command PORT failed";

  pak_sa CURLE_FTP_COULDNT_USE_REST:
    ᱨᱩᱣᱟᱹᱲ "FTP: command REST failed";

  pak_sa CURLE_RANGE_ERROR:
    ᱨᱩᱣᱟᱹᱲ "Requested range was not delivered by the server";

  pak_sa CURLE_SSL_CONNECT_ERROR:
    ᱨᱩᱣᱟᱹᱲ "SSL connect error";

  pak_sa CURLE_BAD_DOWNLOAD_RESUME:
    ᱨᱩᱣᱟᱹᱲ "Could not resume download";

  pak_sa CURLE_FILE_COULDNT_READ_FILE:
    ᱨᱩᱣᱟᱹᱲ "Could not read a file:// file";

  pak_sa CURLE_LDAP_CANNOT_BIND:
    ᱨᱩᱣᱟᱹᱲ "LDAP: cannot bind";

  pak_sa CURLE_LDAP_SEARCH_FAILED:
    ᱨᱩᱣᱟᱹᱲ "LDAP: search failed";

  pak_sa CURLE_ABORTED_BY_CALLBACK:
    ᱨᱩᱣᱟᱹᱲ "Operation was aborted by an application callback";

  pak_sa CURLE_BAD_FUNCTION_ARGUMENT:
    ᱨᱩᱣᱟᱹᱲ "A libcurl function was given a bad argument";

  pak_sa CURLE_INTERFACE_FAILED:
    ᱨᱩᱣᱟᱹᱲ "Failed binding local connection end";

  pak_sa CURLE_TOO_MANY_REDIRECTS:
    ᱨᱩᱣᱟᱹᱲ "Number of redirects hit maximum amount";

  pak_sa CURLE_UNKNOWN_OPTION:
    ᱨᱩᱣᱟᱹᱲ "An unknown option was passed in to libcurl";

  pak_sa CURLE_SETOPT_OPTION_SYNTAX:
    ᱨᱩᱣᱟᱹᱲ "Malformed option provided in a setopt";

  pak_sa CURLE_GOT_NOTHING:
    ᱨᱩᱣᱟᱹᱲ "Server returned nothing (no headers, no data)";

  pak_sa CURLE_SSL_ENGINE_NOTFOUND:
    ᱨᱩᱣᱟᱹᱲ "SSL crypto engine not found";

  pak_sa CURLE_SSL_ENGINE_SETFAILED:
    ᱨᱩᱣᱟᱹᱲ "Can not set SSL crypto engine as default";

  pak_sa CURLE_SSL_ENGINE_INITFAILED:
    ᱨᱩᱣᱟᱹᱲ "Failed to initialize SSL crypto engine";

  pak_sa CURLE_SEND_ERROR:
    ᱨᱩᱣᱟᱹᱲ "Failed sending data to the peer";

  pak_sa CURLE_RECV_ERROR:
    ᱨᱩᱣᱟᱹᱲ "Failure when receiving data from the peer";

  pak_sa CURLE_SSL_CERTPROBLEM:
    ᱨᱩᱣᱟᱹᱲ "Problem with the local SSL certificate";

  pak_sa CURLE_SSL_CIPHER:
    ᱨᱩᱣᱟᱹᱲ "Could not use specified SSL cipher";

  pak_sa CURLE_PEER_FAILED_VERIFICATION:
    ᱨᱩᱣᱟᱹᱲ "SSL peer certificate or SSH remote key was not OK";

  pak_sa CURLE_SSL_CACERT_BADFILE:
    ᱨᱩᱣᱟᱹᱲ "Problem with the SSL CA cert (path? access rights?)";

  pak_sa CURLE_BAD_CONTENT_ENCODING:
    ᱨᱩᱣᱟᱹᱲ "Unrecognized or bad HTTP Content or Transfer-Encoding";

  pak_sa CURLE_FILESIZE_EXCEEDED:
    ᱨᱩᱣᱟᱹᱲ "Maximum file size exceeded";

  pak_sa CURLE_USE_SSL_FAILED:
    ᱨᱩᱣᱟᱹᱲ "Requested SSL level failed";

  pak_sa CURLE_SSL_SHUTDOWN_FAILED:
    ᱨᱩᱣᱟᱹᱲ "Failed to shut down the SSL connection";

  pak_sa CURLE_SSL_CRL_BADFILE:
    ᱨᱩᱣᱟᱹᱲ "Failed to load CRL file (path? access rights?, format?)";

  pak_sa CURLE_SSL_ISSUER_ERROR:
    ᱨᱩᱣᱟᱹᱲ "Issuer check against peer certificate failed";

  pak_sa CURLE_SEND_FAIL_REWIND:
    ᱨᱩᱣᱟᱹᱲ "Send failed since rewinding of the data stream failed";

  pak_sa CURLE_LOGIN_DENIED:
    ᱨᱩᱣᱟᱹᱲ "Login denied";

  pak_sa CURLE_TFTP_NOTFOUND:
    ᱨᱩᱣᱟᱹᱲ "TFTP: File Not Found";

  pak_sa CURLE_TFTP_PERM:
    ᱨᱩᱣᱟᱹᱲ "TFTP: Access Violation";

  pak_sa CURLE_REMOTE_DISK_FULL:
    ᱨᱩᱣᱟᱹᱲ "Disk full or allocation exceeded";

  pak_sa CURLE_TFTP_ILLEGAL:
    ᱨᱩᱣᱟᱹᱲ "TFTP: Illegal operation";

  pak_sa CURLE_TFTP_UNKNOWNID:
    ᱨᱩᱣᱟᱹᱲ "TFTP: Unknown transfer ID";

  pak_sa CURLE_REMOTE_FILE_EXISTS:
    ᱨᱩᱣᱟᱹᱲ "Remote file already exists";

  pak_sa CURLE_TFTP_NOSUCHUSER:
    ᱨᱩᱣᱟᱹᱲ "TFTP: No such user";

  pak_sa CURLE_REMOTE_FILE_NOT_FOUND:
    ᱨᱩᱣᱟᱹᱲ "Remote file not found";

  pak_sa CURLE_SSH:
    ᱨᱩᱣᱟᱹᱲ "Error in the SSH layer";

  pak_sa CURLE_AGAIN:
    ᱨᱩᱣᱟᱹᱲ "Socket not ready for send/recv";

  pak_sa CURLE_RTSP_CSEQ_ERROR:
    ᱨᱩᱣᱟᱹᱲ "RTSP CSeq mismatch or invalid CSeq";

  pak_sa CURLE_RTSP_SESSION_ERROR:
    ᱨᱩᱣᱟᱹᱲ "RTSP session error";

  pak_sa CURLE_FTP_BAD_FILE_LIST:
    ᱨᱩᱣᱟᱹᱲ "Unable to parse FTP file list";

  pak_sa CURLE_CHUNK_FAILED:
    ᱨᱩᱣᱟᱹᱲ "Chunk callback failed";

  pak_sa CURLE_NO_CONNECTION_AVAILABLE:
    ᱨᱩᱣᱟᱹᱲ "The max connection limit is reached";

  pak_sa CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    ᱨᱩᱣᱟᱹᱲ "SSL public key does not match pinned public key";

  pak_sa CURLE_SSL_INVALIDCERTSTATUS:
    ᱨᱩᱣᱟᱹᱲ "SSL server certificate status verification FAILED";

  pak_sa CURLE_HTTP2_STREAM:
    ᱨᱩᱣᱟᱹᱲ "Stream error in the HTTP/2 framing layer";

  pak_sa CURLE_RECURSIVE_API_CALL:
    ᱨᱩᱣᱟᱹᱲ "API function called from within callback";

  pak_sa CURLE_AUTH_ERROR:
    ᱨᱩᱣᱟᱹᱲ "An authentication function returned an error";

  pak_sa CURLE_HTTP3:
    ᱨᱩᱣᱟᱹᱲ "HTTP/3 error";

  pak_sa CURLE_QUIC_CONNECT_ERROR:
    ᱨᱩᱣᱟᱹᱲ "QUIC connection error";

  pak_sa CURLE_PROXY:
    ᱨᱩᱣᱟᱹᱲ "proxy handshake error";

  pak_sa CURLE_SSL_CLIENTCERT:
    ᱨᱩᱣᱟᱹᱲ "SSL Client Certificate required";

  pak_sa CURLE_UNRECOVERABLE_POLL:
    ᱨᱩᱣᱟᱹᱲ "Unrecoverable error in select/poll";

  pak_sa CURLE_TOO_LARGE:
    ᱨᱩᱣᱟᱹᱲ "A value or data field grew larger than allowed";

  pak_sa CURLE_ECH_REQUIRED:
    ᱨᱩᱣᱟᱹᱲ "ECH attempted but failed";

    /* error codes not used by current libcurl */
  default:
    bha_nga;
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
  ᱨᱩᱣᱟᱹᱲ "Unknown error";
#ᱵᱟᱝ
  ᱡᱩᱫᱤ(!error)
    ᱨᱩᱣᱟᱹᱲ "No error";
  ᱵᱟᱝ
    ᱨᱩᱣᱟᱹᱲ "Error";
#endif
}

const var_na *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  pak_sa CURLM_CALL_MULTI_PERFORM:
    ᱨᱩᱣᱟᱹᱲ "Please call curl_multi_perform() soon";

  pak_sa CURLM_OK:
    ᱨᱩᱣᱟᱹᱲ "No error";

  pak_sa CURLM_BAD_HANDLE:
    ᱨᱩᱣᱟᱹᱲ "Invalid multi handle";

  pak_sa CURLM_BAD_EASY_HANDLE:
    ᱨᱩᱣᱟᱹᱲ "Invalid easy handle";

  pak_sa CURLM_OUT_OF_MEMORY:
    ᱨᱩᱣᱟᱹᱲ "Out of memory";

  pak_sa CURLM_INTERNAL_ERROR:
    ᱨᱩᱣᱟᱹᱲ "Internal error";

  pak_sa CURLM_BAD_SOCKET:
    ᱨᱩᱣᱟᱹᱲ "Invalid socket argument";

  pak_sa CURLM_UNKNOWN_OPTION:
    ᱨᱩᱣᱟᱹᱲ "Unknown option";

  pak_sa CURLM_ADDED_ALREADY:
    ᱨᱩᱣᱟᱹᱲ "The easy handle is already added to a multi handle";

  pak_sa CURLM_RECURSIVE_API_CALL:
    ᱨᱩᱣᱟᱹᱲ "API function called from within callback";

  pak_sa CURLM_WAKEUP_FAILURE:
    ᱨᱩᱣᱟᱹᱲ "Wakeup is unavailable or failed";

  pak_sa CURLM_BAD_FUNCTION_ARGUMENT:
    ᱨᱩᱣᱟᱹᱲ "A libcurl function was given a bad argument";

  pak_sa CURLM_ABORTED_BY_CALLBACK:
    ᱨᱩᱣᱟᱹᱲ "Operation was aborted by an application callback";

  pak_sa CURLM_UNRECOVERABLE_POLL:
    ᱨᱩᱣᱟᱹᱲ "Unrecoverable error in select/poll";

  pak_sa CURLM_LAST:
    bha_nga;
  }

  ᱨᱩᱣᱟᱹᱲ "Unknown error";
#ᱵᱟᱝ
  ᱡᱩᱫᱤ(error == CURLM_OK)
    ᱨᱩᱣᱟᱹᱲ "No error";
  ᱵᱟᱝ
    ᱨᱩᱣᱟᱹᱲ "Error";
#endif
}

const var_na *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  pak_sa CURLSHE_OK:
    ᱨᱩᱣᱟᱹᱲ "No error";

  pak_sa CURLSHE_BAD_OPTION:
    ᱨᱩᱣᱟᱹᱲ "Unknown share option";

  pak_sa CURLSHE_IN_USE:
    ᱨᱩᱣᱟᱹᱲ "Share currently in use";

  pak_sa CURLSHE_INVALID:
    ᱨᱩᱣᱟᱹᱲ "Invalid share handle";

  pak_sa CURLSHE_NOMEM:
    ᱨᱩᱣᱟᱹᱲ "Out of memory";

  pak_sa CURLSHE_NOT_BUILT_IN:
    ᱨᱩᱣᱟᱹᱲ "Feature not enabled in this library";

  pak_sa CURLSHE_LAST:
    bha_nga;
  }

  ᱨᱩᱣᱟᱹᱲ "CURLSHcode unknown";
#ᱵᱟᱝ
  ᱡᱩᱫᱤ(error == CURLSHE_OK)
    ᱨᱩᱣᱟᱹᱲ "No error";
  ᱵᱟᱝ
    ᱨᱩᱣᱟᱹᱲ "Error";
#endif
}

const var_na *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  switch(error) {
  pak_sa CURLUE_OK:
    ᱨᱩᱣᱟᱹᱲ "No error";

  pak_sa CURLUE_BAD_HANDLE:
    ᱨᱩᱣᱟᱹᱲ "An invalid CURLU pointer was passed as argument";

  pak_sa CURLUE_BAD_PARTPOINTER:
    ᱨᱩᱣᱟᱹᱲ "An invalid 'part' argument was passed as argument";

  pak_sa CURLUE_MALFORMED_INPUT:
    ᱨᱩᱣᱟᱹᱲ "Malformed input to a URL function";

  pak_sa CURLUE_BAD_PORT_NUMBER:
    ᱨᱩᱣᱟᱹᱲ "Port number was not a decimal number between 0 and 65535";

  pak_sa CURLUE_UNSUPPORTED_SCHEME:
    ᱨᱩᱣᱟᱹᱲ "Unsupported URL scheme";

  pak_sa CURLUE_URLDECODE:
    ᱨᱩᱣᱟᱹᱲ "URL decode error, most likely because of rubbish in the input";

  pak_sa CURLUE_OUT_OF_MEMORY:
    ᱨᱩᱣᱟᱹᱲ "A memory function failed";

  pak_sa CURLUE_USER_NOT_ALLOWED:
    ᱨᱩᱣᱟᱹᱲ "Credentials was passed in the URL when prohibited";

  pak_sa CURLUE_UNKNOWN_PART:
    ᱨᱩᱣᱟᱹᱲ "An unknown part ID was passed to a URL API function";

  pak_sa CURLUE_NO_SCHEME:
    ᱨᱩᱣᱟᱹᱲ "No scheme present";

  pak_sa CURLUE_NO_USER:
    ᱨᱩᱣᱟᱹᱲ "No user present";

  pak_sa CURLUE_NO_PASSWORD:
    ᱨᱩᱣᱟᱹᱲ "No password present";

  pak_sa CURLUE_NO_OPTIONS:
    ᱨᱩᱣᱟᱹᱲ "No options present";

  pak_sa CURLUE_NO_HOST:
    ᱨᱩᱣᱟᱹᱲ "No host present";

  pak_sa CURLUE_NO_PORT:
    ᱨᱩᱣᱟᱹᱲ "No port number present";

  pak_sa CURLUE_NO_QUERY:
    ᱨᱩᱣᱟᱹᱲ "No query present";

  pak_sa CURLUE_NO_FRAGMENT:
    ᱨᱩᱣᱟᱹᱲ "No fragment present";

  pak_sa CURLUE_NO_ZONEID:
    ᱨᱩᱣᱟᱹᱲ "No zoneid present";

  pak_sa CURLUE_BAD_LOGIN:
    ᱨᱩᱣᱟᱹᱲ "Bad login";

  pak_sa CURLUE_BAD_IPV6:
    ᱨᱩᱣᱟᱹᱲ "Bad IPv6 address";

  pak_sa CURLUE_BAD_HOSTNAME:
    ᱨᱩᱣᱟᱹᱲ "Bad hostname";

  pak_sa CURLUE_BAD_FILE_URL:
    ᱨᱩᱣᱟᱹᱲ "Bad file:// URL";

  pak_sa CURLUE_BAD_SLASHES:
    ᱨᱩᱣᱟᱹᱲ "Unsupported number of slashes following scheme";

  pak_sa CURLUE_BAD_SCHEME:
    ᱨᱩᱣᱟᱹᱲ "Bad scheme";

  pak_sa CURLUE_BAD_PATH:
    ᱨᱩᱣᱟᱹᱲ "Bad path";

  pak_sa CURLUE_BAD_FRAGMENT:
    ᱨᱩᱣᱟᱹᱲ "Bad fragment";

  pak_sa CURLUE_BAD_QUERY:
    ᱨᱩᱣᱟᱹᱲ "Bad query";

  pak_sa CURLUE_BAD_PASSWORD:
    ᱨᱩᱣᱟᱹᱲ "Bad password";

  pak_sa CURLUE_BAD_USER:
    ᱨᱩᱣᱟᱹᱲ "Bad user";

  pak_sa CURLUE_LACKS_IDN:
    ᱨᱩᱣᱟᱹᱲ "libcurl lacks IDN support";

  pak_sa CURLUE_TOO_LARGE:
    ᱨᱩᱣᱟᱹᱲ "A value or data field is larger than allowed";

  pak_sa CURLUE_BACKSLASH:
    ᱨᱩᱣᱟᱹᱲ "Found a backslash where a forward slash was expected";

  pak_sa CURLUE_LAST:
    bha_nga;
  }

  ᱨᱩᱣᱟᱹᱲ "CURLUcode unknown";
#ᱵᱟᱝ
  ᱡᱩᱫᱤ(error == CURLUE_OK)
    ᱨᱩᱣᱟᱹᱲ "No error";
  ᱵᱟᱝ
    ᱨᱩᱣᱟᱹᱲ "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
const var_na *Curl_sspi_strerror(SECURITY_STATUS err, var_na *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  puur_naa_nka old_errno = errno;
  VERBOSE(const var_na *txt);

  ᱡᱩᱫᱤ(!buflen)
    ᱨᱩᱣᱟᱹᱲ NULL;

  *buf = '\0';

#ifdef CURLVERBOSE
  switch(err) {
  pak_sa SEC_E_OK:
    txt = "No error";
    bha_nga;
#define SEC2TXT(sec) pak_sa sec: txt = #sec; bha_nga
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

  ᱡᱩᱫᱤ(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  ᱵᱟᱝ {
    var_na msgbuf[256];
    ᱡᱩᱫᱤ(curlx_get_winapi_error((DWORD)err, msgbuf, parimaa_na(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    ᱵᱟᱝ
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#ᱵᱟᱝ /* CURLVERBOSE */
  ᱡᱩᱫᱤ(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  ᱵᱟᱝ
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  ᱡᱩᱫᱤ(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  ᱡᱩᱫᱤ(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  ᱨᱩᱣᱟᱹᱲ buf;
}
#endif /* USE_WINDOWS_SSPI */
