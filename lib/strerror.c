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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#ଅନ୍ତର୍ଭାଵ "curl_sspi.h"
#endif

#ଅନ୍ତର୍ଭାଵ "curlx/winapi.h"
#ଅନ୍ତର୍ଭାଵ "strerror.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

ଅଚଳ ଅକ୍ଷର *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  ବାଛ(error) {
  ସ୍ଥିତି CURLE_OK:
    ଫେରନ୍ତୁ "No error";

  ସ୍ଥିତି CURLE_UNSUPPORTED_PROTOCOL:
    ଫେରନ୍ତୁ "Unsupported protocol";

  ସ୍ଥିତି CURLE_FAILED_INIT:
    ଫେରନ୍ତୁ "Failed initialization";

  ସ୍ଥିତି CURLE_URL_MALFORMAT:
    ଫେରନ୍ତୁ "URL using bad/illegal format or missing URL";

  ସ୍ଥିତି CURLE_NOT_BUILT_IN:
    ଫେରନ୍ତୁ "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  ସ୍ଥିତି CURLE_COULDNT_RESOLVE_PROXY:
    ଫେରନ୍ତୁ "Could not resolve proxy name";

  ସ୍ଥିତି CURLE_COULDNT_RESOLVE_HOST:
    ଫେରନ୍ତୁ "Could not resolve hostname";

  ସ୍ଥିତି CURLE_COULDNT_CONNECT:
    ଫେରନ୍ତୁ "Could not connect to server";

  ସ୍ଥିତି CURLE_WEIRD_SERVER_REPLY:
    ଫେରନ୍ତୁ "Weird server reply";

  ସ୍ଥିତି CURLE_REMOTE_ACCESS_DENIED:
    ଫେରନ୍ତୁ "Access denied to remote resource";

  ସ୍ଥିତି CURLE_FTP_ACCEPT_FAILED:
    ଫେରନ୍ତୁ "FTP: The server failed to connect to data port";

  ସ୍ଥିତି CURLE_FTP_ACCEPT_TIMEOUT:
    ଫେରନ୍ତୁ "FTP: Accepting server connect has timed out";

  ସ୍ଥିତି CURLE_FTP_PRET_FAILED:
    ଫେରନ୍ତୁ "FTP: The server did not accept the PRET command.";

  ସ୍ଥିତି CURLE_FTP_WEIRD_PASS_REPLY:
    ଫେରନ୍ତୁ "FTP: unknown PASS reply";

  ସ୍ଥିତି CURLE_FTP_WEIRD_PASV_REPLY:
    ଫେରନ୍ତୁ "FTP: unknown PASV reply";

  ସ୍ଥିତି CURLE_FTP_WEIRD_227_FORMAT:
    ଫେରନ୍ତୁ "FTP: unknown 227 response format";

  ସ୍ଥିତି CURLE_FTP_CANT_GET_HOST:
    ଫେରନ୍ତୁ "FTP: cannot figure out the host in the PASV response";

  ସ୍ଥିତି CURLE_HTTP2:
    ଫେରନ୍ତୁ "Error in the HTTP2 framing layer";

  ସ୍ଥିତି CURLE_FTP_COULDNT_SET_TYPE:
    ଫେରନ୍ତୁ "FTP: could not set file type";

  ସ୍ଥିତି CURLE_PARTIAL_FILE:
    ଫେରନ୍ତୁ "Transferred a partial file";

  ସ୍ଥିତି CURLE_FTP_COULDNT_RETR_FILE:
    ଫେରନ୍ତୁ "FTP: could not retrieve (RETR failed) the specified file";

  ସ୍ଥିତି CURLE_QUOTE_ERROR:
    ଫେରନ୍ତୁ "Quote command returned error";

  ସ୍ଥିତି CURLE_HTTP_RETURNED_ERROR:
    ଫେରନ୍ତୁ "HTTP response code said error";

  ସ୍ଥିତି CURLE_WRITE_ERROR:
    ଫେରନ୍ତୁ "Failed writing received data to disk/application";

  ସ୍ଥିତି CURLE_UPLOAD_FAILED:
    ଫେରନ୍ତୁ "Upload failed (at start/before it took off)";

  ସ୍ଥିତି CURLE_READ_ERROR:
    ଫେରନ୍ତୁ "Failed to open/read local data from file/application";

  ସ୍ଥିତି CURLE_OUT_OF_MEMORY:
    ଫେରନ୍ତୁ "Out of memory";

  ସ୍ଥିତି CURLE_OPERATION_TIMEDOUT:
    ଫେରନ୍ତୁ "Timeout was reached";

  ସ୍ଥିତି CURLE_FTP_PORT_FAILED:
    ଫେରନ୍ତୁ "FTP: command PORT failed";

  ସ୍ଥିତି CURLE_FTP_COULDNT_USE_REST:
    ଫେରନ୍ତୁ "FTP: command REST failed";

  ସ୍ଥିତି CURLE_RANGE_ERROR:
    ଫେରନ୍ତୁ "Requested range was not delivered by the server";

  ସ୍ଥିତି CURLE_SSL_CONNECT_ERROR:
    ଫେରନ୍ତୁ "SSL connect error";

  ସ୍ଥିତି CURLE_BAD_DOWNLOAD_RESUME:
    ଫେରନ୍ତୁ "Could not resume download";

  ସ୍ଥିତି CURLE_FILE_COULDNT_READ_FILE:
    ଫେରନ୍ତୁ "Could not read a file:// file";

  ସ୍ଥିତି CURLE_LDAP_CANNOT_BIND:
    ଫେରନ୍ତୁ "LDAP: cannot bind";

  ସ୍ଥିତି CURLE_LDAP_ସନ୍ଧାନ_FAILED:
    ଫେରନ୍ତୁ "LDAP: search failed";

  ସ୍ଥିତି CURLE_ABORTED_BY_CALLBACK:
    ଫେରନ୍ତୁ "Operation was aborted by an application callback";

  ସ୍ଥିତି CURLE_BAD_FUNCTION_ARGUMENT:
    ଫେରନ୍ତୁ "A libcurl function was given a bad argument";

  ସ୍ଥିତି CURLE_INTERFACE_FAILED:
    ଫେରନ୍ତୁ "Failed binding local connection end";

  ସ୍ଥିତି CURLE_TOO_MANY_REDIRECTS:
    ଫେରନ୍ତୁ "Number of redirects hit maximum amount";

  ସ୍ଥିତି CURLE_UNKNOWN_OPTION:
    ଫେରନ୍ତୁ "An unknown option was passed in to libcurl";

  ସ୍ଥିତି CURLE_SETOPT_OPTION_SYNTAX:
    ଫେରନ୍ତୁ "Malformed option provided in a setopt";

  ସ୍ଥିତି CURLE_GOT_NOTHING:
    ଫେରନ୍ତୁ "Server returned nothing (no headers, no data)";

  ସ୍ଥିତି CURLE_SSL_ENGINE_NOTFOUND:
    ଫେରନ୍ତୁ "SSL crypto engine not found";

  ସ୍ଥିତି CURLE_SSL_ENGINE_SETFAILED:
    ଫେରନ୍ତୁ "Can not set SSL crypto engine as default";

  ସ୍ଥିତି CURLE_SSL_ENGINE_INITFAILED:
    ଫେରନ୍ତୁ "Failed to initialize SSL crypto engine";

  ସ୍ଥିତି CURLE_SEND_ERROR:
    ଫେରନ୍ତୁ "Failed sending data to the peer";

  ସ୍ଥିତି CURLE_RECV_ERROR:
    ଫେରନ୍ତୁ "Failure when receiving data from the peer";

  ସ୍ଥିତି CURLE_SSL_CERTPROBLEM:
    ଫେରନ୍ତୁ "Problem with the local SSL certificate";

  ସ୍ଥିତି CURLE_SSL_CIPHER:
    ଫେରନ୍ତୁ "Could not use specified SSL cipher";

  ସ୍ଥିତି CURLE_PEER_FAILED_VERIFICATION:
    ଫେରନ୍ତୁ "SSL peer certificate or SSH remote key was not OK";

  ସ୍ଥିତି CURLE_SSL_CACERT_BADFILE:
    ଫେରନ୍ତୁ "Problem with the SSL CA cert (path? access rights?)";

  ସ୍ଥିତି CURLE_BAD_CONTENT_ENCODING:
    ଫେରନ୍ତୁ "Unrecognized or bad HTTP Content or Transfer-Encoding";

  ସ୍ଥିତି CURLE_FILESIZE_EXCEEDED:
    ଫେରନ୍ତୁ "Maximum file size exceeded";

  ସ୍ଥିତି CURLE_USE_SSL_FAILED:
    ଫେରନ୍ତୁ "Requested SSL level failed";

  ସ୍ଥିତି CURLE_SSL_SHUTDOWN_FAILED:
    ଫେରନ୍ତୁ "Failed to shut down the SSL connection";

  ସ୍ଥିତି CURLE_SSL_CRL_BADFILE:
    ଫେରନ୍ତୁ "Failed to load CRL file (path? access rights?, format?)";

  ସ୍ଥିତି CURLE_SSL_ISSUER_ERROR:
    ଫେରନ୍ତୁ "Issuer check against peer certificate failed";

  ସ୍ଥିତି CURLE_SEND_FAIL_REWIND:
    ଫେରନ୍ତୁ "Send failed since rewinding of the data stream failed";

  ସ୍ଥିତି CURLE_LOGIN_DENIED:
    ଫେରନ୍ତୁ "Login denied";

  ସ୍ଥିତି CURLE_TFTP_NOTFOUND:
    ଫେରନ୍ତୁ "TFTP: File Not Found";

  ସ୍ଥିତି CURLE_TFTP_PERM:
    ଫେରନ୍ତୁ "TFTP: Access Violation";

  ସ୍ଥିତି CURLE_REMOTE_DISK_FULL:
    ଫେରନ୍ତୁ "Disk full or allocation exceeded";

  ସ୍ଥିତି CURLE_TFTP_ILLEGAL:
    ଫେରନ୍ତୁ "TFTP: Illegal operation";

  ସ୍ଥିତି CURLE_TFTP_UNKNOWNID:
    ଫେରନ୍ତୁ "TFTP: Unknown transfer ID";

  ସ୍ଥିତି CURLE_REMOTE_FILE_EXISTS:
    ଫେରନ୍ତୁ "Remote file already exists";

  ସ୍ଥିତି CURLE_TFTP_NOSUCHUSER:
    ଫେରନ୍ତୁ "TFTP: No such user";

  ସ୍ଥିତି CURLE_REMOTE_FILE_NOT_FOUND:
    ଫେରନ୍ତୁ "Remote file not found";

  ସ୍ଥିତି CURLE_SSH:
    ଫେରନ୍ତୁ "Error in the SSH layer";

  ସ୍ଥିତି CURLE_AGAIN:
    ଫେରନ୍ତୁ "Socket not ready for send/recv";

  ସ୍ଥିତି CURLE_RTSP_CSEQ_ERROR:
    ଫେରନ୍ତୁ "RTSP CSeq mismatch or invalid CSeq";

  ସ୍ଥିତି CURLE_RTSP_SESSION_ERROR:
    ଫେରନ୍ତୁ "RTSP session error";

  ସ୍ଥିତି CURLE_FTP_BAD_FILE_LIST:
    ଫେରନ୍ତୁ "Unable to parse FTP file list";

  ସ୍ଥିତି CURLE_CHUNK_FAILED:
    ଫେରନ୍ତୁ "Chunk callback failed";

  ସ୍ଥିତି CURLE_NO_CONNECTION_AVAILABLE:
    ଫେରନ୍ତୁ "The max connection limit is reached";

  ସ୍ଥିତି CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    ଫେରନ୍ତୁ "SSL public key does not match pinned public key";

  ସ୍ଥିତି CURLE_SSL_INVALIDCERTSTATUS:
    ଫେରନ୍ତୁ "SSL server certificate status verification FAILED";

  ସ୍ଥିତି CURLE_HTTP2_STREAM:
    ଫେରନ୍ତୁ "Stream error in the HTTP/2 framing layer";

  ସ୍ଥିତି CURLE_RECURSIVE_API_CALL:
    ଫେରନ୍ତୁ "API function called from within callback";

  ସ୍ଥିତି CURLE_AUTH_ERROR:
    ଫେରନ୍ତୁ "An authentication function returned an error";

  ସ୍ଥିତି CURLE_HTTP3:
    ଫେରନ୍ତୁ "HTTP/3 error";

  ସ୍ଥିତି CURLE_QUIC_CONNECT_ERROR:
    ଫେରନ୍ତୁ "QUIC connection error";

  ସ୍ଥିତି CURLE_PROXY:
    ଫେରନ୍ତୁ "proxy handshake error";

  ସ୍ଥିତି CURLE_SSL_CLIENTCERT:
    ଫେରନ୍ତୁ "SSL Client Certificate required";

  ସ୍ଥିତି CURLE_UNRECOVERABLE_POLL:
    ଫେରନ୍ତୁ "Unrecoverable error in select/poll";

  ସ୍ଥିତି CURLE_TOO_LARGE:
    ଫେରନ୍ତୁ "A value or data field grew larger than allowed";

  ସ୍ଥିତି CURLE_ECH_REQUIRED:
    ଫେରନ୍ତୁ "ECH attempted but failed";

    /* error codes not used by current libcurl */
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
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
  ଫେରନ୍ତୁ "Unknown error";
#ଅନ୍ୟଥା
  ଯଦି(!error)
    ଫେରନ୍ତୁ "No error";
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ "Error";
#endif
}

ଅଚଳ ଅକ୍ଷର *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  ବାଛ(error) {
  ସ୍ଥିତି CURLM_CALL_MULTI_PERFORM:
    ଫେରନ୍ତୁ "Please call curl_multi_perform() soon";

  ସ୍ଥିତି CURLM_OK:
    ଫେରନ୍ତୁ "No error";

  ସ୍ଥିତି CURLM_BAD_HANDLE:
    ଫେରନ୍ତୁ "Invalid multi handle";

  ସ୍ଥିତି CURLM_BAD_EASY_HANDLE:
    ଫେରନ୍ତୁ "Invalid easy handle";

  ସ୍ଥିତି CURLM_OUT_OF_MEMORY:
    ଫେରନ୍ତୁ "Out of memory";

  ସ୍ଥିତି CURLM_INTERNAL_ERROR:
    ଫେରନ୍ତୁ "Internal error";

  ସ୍ଥିତି CURLM_BAD_SOCKET:
    ଫେରନ୍ତୁ "Invalid socket argument";

  ସ୍ଥିତି CURLM_UNKNOWN_OPTION:
    ଫେରନ୍ତୁ "Unknown option";

  ସ୍ଥିତି CURLM_ADDED_ALREADY:
    ଫେରନ୍ତୁ "The easy handle is already added to a multi handle";

  ସ୍ଥିତି CURLM_RECURSIVE_API_CALL:
    ଫେରନ୍ତୁ "API function called from within callback";

  ସ୍ଥିତି CURLM_WAKEUP_FAILURE:
    ଫେରନ୍ତୁ "Wakeup is unavailable or failed";

  ସ୍ଥିତି CURLM_BAD_FUNCTION_ARGUMENT:
    ଫେରନ୍ତୁ "A libcurl function was given a bad argument";

  ସ୍ଥିତି CURLM_ABORTED_BY_CALLBACK:
    ଫେରନ୍ତୁ "Operation was aborted by an application callback";

  ସ୍ଥିତି CURLM_UNRECOVERABLE_POLL:
    ଫେରନ୍ତୁ "Unrecoverable error in select/poll";

  ସ୍ଥିତି CURLM_LAST:
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ "Unknown error";
#ଅନ୍ୟଥା
  ଯଦି(error == CURLM_OK)
    ଫେରନ୍ତୁ "No error";
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ "Error";
#endif
}

ଅଚଳ ଅକ୍ଷର *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  ବାଛ(error) {
  ସ୍ଥିତି CURLSHE_OK:
    ଫେରନ୍ତୁ "No error";

  ସ୍ଥିତି CURLSHE_BAD_OPTION:
    ଫେରନ୍ତୁ "Unknown share option";

  ସ୍ଥିତି CURLSHE_IN_USE:
    ଫେରନ୍ତୁ "Share currently in use";

  ସ୍ଥିତି CURLSHE_INVALID:
    ଫେରନ୍ତୁ "Invalid share handle";

  ସ୍ଥିତି CURLSHE_NOMEM:
    ଫେରନ୍ତୁ "Out of memory";

  ସ୍ଥିତି CURLSHE_NOT_BUILT_IN:
    ଫେରନ୍ତୁ "Feature not enabled in this library";

  ସ୍ଥିତି CURLSHE_LAST:
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ "CURLSHcode unknown";
#ଅନ୍ୟଥା
  ଯଦି(error == CURLSHE_OK)
    ଫେରନ୍ତୁ "No error";
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ "Error";
#endif
}

ଅଚଳ ଅକ୍ଷର *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  ବାଛ(error) {
  ସ୍ଥିତି CURLUE_OK:
    ଫେରନ୍ତୁ "No error";

  ସ୍ଥିତି CURLUE_BAD_HANDLE:
    ଫେରନ୍ତୁ "An invalid CURLU pointer was passed as argument";

  ସ୍ଥିତି CURLUE_BAD_PARTPOINTER:
    ଫେରନ୍ତୁ "An invalid 'part' argument was passed as argument";

  ସ୍ଥିତି CURLUE_MALFORMED_ନିବେଶ:
    ଫେରନ୍ତୁ "Malformed input to a URL function";

  ସ୍ଥିତି CURLUE_BAD_PORT_ସଂଖ୍ୟା:
    ଫେରନ୍ତୁ "Port number was not a decimal number between 0 and 65535";

  ସ୍ଥିତି CURLUE_UNSUPPORTED_SCHEME:
    ଫେରନ୍ତୁ "Unsupported URL scheme";

  ସ୍ଥିତି CURLUE_URLDECODE:
    ଫେରନ୍ତୁ "URL decode error, most likely because of rubbish in the input";

  ସ୍ଥିତି CURLUE_OUT_OF_MEMORY:
    ଫେରନ୍ତୁ "A memory function failed";

  ସ୍ଥିତି CURLUE_USER_NOT_ALLOWED:
    ଫେରନ୍ତୁ "Credentials was passed in the URL when prohibited";

  ସ୍ଥିତି CURLUE_UNKNOWN_PART:
    ଫେରନ୍ତୁ "An unknown part ID was passed to a URL API function";

  ସ୍ଥିତି CURLUE_NO_SCHEME:
    ଫେରନ୍ତୁ "No scheme present";

  ସ୍ଥିତି CURLUE_NO_USER:
    ଫେରନ୍ତୁ "No user present";

  ସ୍ଥିତି CURLUE_NO_PASSWORD:
    ଫେରନ୍ତୁ "No password present";

  ସ୍ଥିତି CURLUE_NO_OPTIONS:
    ଫେରନ୍ତୁ "No options present";

  ସ୍ଥିତି CURLUE_NO_HOST:
    ଫେରନ୍ତୁ "No host present";

  ସ୍ଥିତି CURLUE_NO_PORT:
    ଫେରନ୍ତୁ "No port number present";

  ସ୍ଥିତି CURLUE_NO_QUERY:
    ଫେରନ୍ତୁ "No query present";

  ସ୍ଥିତି CURLUE_NO_FRAGMENT:
    ଫେରନ୍ତୁ "No fragment present";

  ସ୍ଥିତି CURLUE_NO_ZONEID:
    ଫେରନ୍ତୁ "No zoneid present";

  ସ୍ଥିତି CURLUE_BAD_LOGIN:
    ଫେରନ୍ତୁ "Bad login";

  ସ୍ଥିତି CURLUE_BAD_IPV6:
    ଫେରନ୍ତୁ "Bad IPv6 address";

  ସ୍ଥିତି CURLUE_BAD_HOSTNAME:
    ଫେରନ୍ତୁ "Bad hostname";

  ସ୍ଥିତି CURLUE_BAD_FILE_URL:
    ଫେରନ୍ତୁ "Bad file:// URL";

  ସ୍ଥିତି CURLUE_BAD_SLASHES:
    ଫେରନ୍ତୁ "Unsupported number of slashes following scheme";

  ସ୍ଥିତି CURLUE_BAD_SCHEME:
    ଫେରନ୍ତୁ "Bad scheme";

  ସ୍ଥିତି CURLUE_BAD_PATH:
    ଫେରନ୍ତୁ "Bad path";

  ସ୍ଥିତି CURLUE_BAD_FRAGMENT:
    ଫେରନ୍ତୁ "Bad fragment";

  ସ୍ଥିତି CURLUE_BAD_QUERY:
    ଫେରନ୍ତୁ "Bad query";

  ସ୍ଥିତି CURLUE_BAD_PASSWORD:
    ଫେରନ୍ତୁ "Bad password";

  ସ୍ଥିତି CURLUE_BAD_USER:
    ଫେରନ୍ତୁ "Bad user";

  ସ୍ଥିତି CURLUE_LACKS_IDN:
    ଫେରନ୍ତୁ "libcurl lacks IDN support";

  ସ୍ଥିତି CURLUE_TOO_LARGE:
    ଫେରନ୍ତୁ "A value or data field is larger than allowed";

  ସ୍ଥିତି CURLUE_BACKSLASH:
    ଫେରନ୍ତୁ "Found a backslash where a forward slash was expected";

  ସ୍ଥିତି CURLUE_LAST:
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ "CURLUcode unknown";
#ଅନ୍ୟଥା
  ଯଦି(error == CURLUE_OK)
    ଫେରନ୍ତୁ "No error";
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ଅଚଳ ଅକ୍ଷର *Curl_sspi_strerror(SECURITY_STATUS err, ଅକ୍ଷର *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_errno = errno;
  VERBOSE(ଅଚଳ ଅକ୍ଷର *txt);

  ଯଦି(!buflen)
    ଫେରନ୍ତୁ ଖାଲି;

  *buf = '\0';

#ifdef CURLVERBOSE
  ବାଛ(err) {
  ସ୍ଥିତି SEC_E_OK:
    txt = "No error";
    ଭାଙ୍ଗ;
#define SEC2TXT(sec) ସ୍ଥିତି sec: txt = #sec; ଭାଙ୍ଗ
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
  SEC2TXT(SEC_E_NO_KERB_ଚାବି);
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
  SEC2TXT(SEC_E_UNTRUSTED_ମୂଳ);
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
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    txt = "Unknown error";
  }

  ଯଦି(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  ଅନ୍ୟଥା {
    ଅକ୍ଷର msgbuf[256];
    ଯଦି(curlx_get_winapi_error((DWORD)err, msgbuf, ଆକାର(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    ଅନ୍ୟଥା
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#ଅନ୍ୟଥା /* CURLVERBOSE */
  ଯଦି(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  ଅନ୍ୟଥା
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  ଯଦି(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  ଯଦି(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  ଫେରନ୍ତୁ buf;
}
#endif /* USE_WINDOWS_SSPI */
