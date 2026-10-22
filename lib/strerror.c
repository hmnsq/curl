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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_WINDOWS_SSPI
#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#endif

#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"
#ಅನ್ತರ್ಭಾವ "strerror.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

ಧ್ರುವ ವರ್ಣ *curl_easy_strerror(CURLcode error)
{
#ifdef CURLVERBOSE
  ವಿಕಲ್ಪ(error) {
  ಪಕ್ಷ CURLE_OK:
    ಪ್ರತ್ಯಾಗಮ "No error";

  ಪಕ್ಷ CURLE_UNSUPPORTED_PROTOCOL:
    ಪ್ರತ್ಯಾಗಮ "Unsupported protocol";

  ಪಕ್ಷ CURLE_FAILED_INIT:
    ಪ್ರತ್ಯಾಗಮ "Failed initialization";

  ಪಕ್ಷ CURLE_URL_MALFORMAT:
    ಪ್ರತ್ಯಾಗಮ "URL using bad/illegal format or missing URL";

  ಪಕ್ಷ CURLE_NOT_BUILT_IN:
    ಪ್ರತ್ಯಾಗಮ "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  ಪಕ್ಷ CURLE_COULDNT_RESOLVE_PROXY:
    ಪ್ರತ್ಯಾಗಮ "Could not resolve proxy name";

  ಪಕ್ಷ CURLE_COULDNT_RESOLVE_HOST:
    ಪ್ರತ್ಯಾಗಮ "Could not resolve hostname";

  ಪಕ್ಷ CURLE_COULDNT_CONNECT:
    ಪ್ರತ್ಯಾಗಮ "Could not connect to server";

  ಪಕ್ಷ CURLE_WEIRD_SERVER_REPLY:
    ಪ್ರತ್ಯಾಗಮ "Weird server reply";

  ಪಕ್ಷ CURLE_REMOTE_ACCESS_DENIED:
    ಪ್ರತ್ಯಾಗಮ "Access denied to remote resource";

  ಪಕ್ಷ CURLE_FTP_ACCEPT_FAILED:
    ಪ್ರತ್ಯಾಗಮ "FTP: The server failed to connect to data port";

  ಪಕ್ಷ CURLE_FTP_ACCEPT_TIMEOUT:
    ಪ್ರತ್ಯಾಗಮ "FTP: Accepting server connect has timed out";

  ಪಕ್ಷ CURLE_FTP_PRET_FAILED:
    ಪ್ರತ್ಯಾಗಮ "FTP: The server did not accept the PRET command.";

  ಪಕ್ಷ CURLE_FTP_WEIRD_PASS_REPLY:
    ಪ್ರತ್ಯಾಗಮ "FTP: unknown PASS reply";

  ಪಕ್ಷ CURLE_FTP_WEIRD_PASV_REPLY:
    ಪ್ರತ್ಯಾಗಮ "FTP: unknown PASV reply";

  ಪಕ್ಷ CURLE_FTP_WEIRD_227_FORMAT:
    ಪ್ರತ್ಯಾಗಮ "FTP: unknown 227 response format";

  ಪಕ್ಷ CURLE_FTP_CANT_GET_HOST:
    ಪ್ರತ್ಯಾಗಮ "FTP: cannot figure out the host in the PASV response";

  ಪಕ್ಷ CURLE_HTTP2:
    ಪ್ರತ್ಯಾಗಮ "Error in the HTTP2 framing layer";

  ಪಕ್ಷ CURLE_FTP_COULDNT_SET_TYPE:
    ಪ್ರತ್ಯಾಗಮ "FTP: could not set file type";

  ಪಕ್ಷ CURLE_PARTIAL_FILE:
    ಪ್ರತ್ಯಾಗಮ "Transferred a partial file";

  ಪಕ್ಷ CURLE_FTP_COULDNT_RETR_FILE:
    ಪ್ರತ್ಯಾಗಮ "FTP: could not retrieve (RETR failed) the specified file";

  ಪಕ್ಷ CURLE_QUOTE_ERROR:
    ಪ್ರತ್ಯಾಗಮ "Quote command returned error";

  ಪಕ್ಷ CURLE_HTTP_RETURNED_ERROR:
    ಪ್ರತ್ಯಾಗಮ "HTTP response code said error";

  ಪಕ್ಷ CURLE_WRITE_ERROR:
    ಪ್ರತ್ಯಾಗಮ "Failed writing received data to disk/application";

  ಪಕ್ಷ CURLE_UPLOAD_FAILED:
    ಪ್ರತ್ಯಾಗಮ "Upload failed (at start/before it took off)";

  ಪಕ್ಷ CURLE_READ_ERROR:
    ಪ್ರತ್ಯಾಗಮ "Failed to open/read local data from file/application";

  ಪಕ್ಷ CURLE_OUT_OF_MEMORY:
    ಪ್ರತ್ಯಾಗಮ "Out of memory";

  ಪಕ್ಷ CURLE_OPERATION_TIMEDOUT:
    ಪ್ರತ್ಯಾಗಮ "Timeout was reached";

  ಪಕ್ಷ CURLE_FTP_PORT_FAILED:
    ಪ್ರತ್ಯಾಗಮ "FTP: command PORT failed";

  ಪಕ್ಷ CURLE_FTP_COULDNT_USE_REST:
    ಪ್ರತ್ಯಾಗಮ "FTP: command REST failed";

  ಪಕ್ಷ CURLE_RANGE_ERROR:
    ಪ್ರತ್ಯಾಗಮ "Requested range was not delivered by the server";

  ಪಕ್ಷ CURLE_SSL_CONNECT_ERROR:
    ಪ್ರತ್ಯಾಗಮ "SSL connect error";

  ಪಕ್ಷ CURLE_BAD_DOWNLOAD_RESUME:
    ಪ್ರತ್ಯಾಗಮ "Could not resume download";

  ಪಕ್ಷ CURLE_FILE_COULDNT_READ_FILE:
    ಪ್ರತ್ಯಾಗಮ "Could not read a file:// file";

  ಪಕ್ಷ CURLE_LDAP_CANNOT_BIND:
    ಪ್ರತ್ಯಾಗಮ "LDAP: cannot bind";

  ಪಕ್ಷ CURLE_LDAP_ಹುಡುಕು_FAILED:
    ಪ್ರತ್ಯಾಗಮ "LDAP: search failed";

  ಪಕ್ಷ CURLE_ABORTED_BY_CALLBACK:
    ಪ್ರತ್ಯಾಗಮ "Operation was aborted by an application callback";

  ಪಕ್ಷ CURLE_BAD_FUNCTION_ARGUMENT:
    ಪ್ರತ್ಯಾಗಮ "A libcurl function was given a bad argument";

  ಪಕ್ಷ CURLE_INTERFACE_FAILED:
    ಪ್ರತ್ಯಾಗಮ "Failed binding local connection end";

  ಪಕ್ಷ CURLE_TOO_MANY_REDIRECTS:
    ಪ್ರತ್ಯಾಗಮ "Number of redirects hit maximum amount";

  ಪಕ್ಷ CURLE_UNKNOWN_OPTION:
    ಪ್ರತ್ಯಾಗಮ "An unknown option was passed in to libcurl";

  ಪಕ್ಷ CURLE_SETOPT_OPTION_SYNTAX:
    ಪ್ರತ್ಯಾಗಮ "Malformed option provided in a setopt";

  ಪಕ್ಷ CURLE_GOT_NOTHING:
    ಪ್ರತ್ಯಾಗಮ "Server returned nothing (no headers, no data)";

  ಪಕ್ಷ CURLE_SSL_ENGINE_NOTFOUND:
    ಪ್ರತ್ಯಾಗಮ "SSL crypto engine not found";

  ಪಕ್ಷ CURLE_SSL_ENGINE_SETFAILED:
    ಪ್ರತ್ಯಾಗಮ "Can not set SSL crypto engine as default";

  ಪಕ್ಷ CURLE_SSL_ENGINE_INITFAILED:
    ಪ್ರತ್ಯಾಗಮ "Failed to initialize SSL crypto engine";

  ಪಕ್ಷ CURLE_SEND_ERROR:
    ಪ್ರತ್ಯಾಗಮ "Failed sending data to the peer";

  ಪಕ್ಷ CURLE_RECV_ERROR:
    ಪ್ರತ್ಯಾಗಮ "Failure when receiving data from the peer";

  ಪಕ್ಷ CURLE_SSL_CERTPROBLEM:
    ಪ್ರತ್ಯಾಗಮ "Problem with the local SSL certificate";

  ಪಕ್ಷ CURLE_SSL_CIPHER:
    ಪ್ರತ್ಯಾಗಮ "Could not use specified SSL cipher";

  ಪಕ್ಷ CURLE_PEER_FAILED_VERIFICATION:
    ಪ್ರತ್ಯಾಗಮ "SSL peer certificate or SSH remote key was not OK";

  ಪಕ್ಷ CURLE_SSL_CACERT_BADFILE:
    ಪ್ರತ್ಯಾಗಮ "Problem with the SSL CA cert (path? access rights?)";

  ಪಕ್ಷ CURLE_BAD_CONTENT_ENCODING:
    ಪ್ರತ್ಯಾಗಮ "Unrecognized or bad HTTP Content or Transfer-Encoding";

  ಪಕ್ಷ CURLE_FILESIZE_EXCEEDED:
    ಪ್ರತ್ಯಾಗಮ "Maximum file size exceeded";

  ಪಕ್ಷ CURLE_USE_SSL_FAILED:
    ಪ್ರತ್ಯಾಗಮ "Requested SSL level failed";

  ಪಕ್ಷ CURLE_SSL_SHUTDOWN_FAILED:
    ಪ್ರತ್ಯಾಗಮ "Failed to shut down the SSL connection";

  ಪಕ್ಷ CURLE_SSL_CRL_BADFILE:
    ಪ್ರತ್ಯಾಗಮ "Failed to load CRL file (path? access rights?, format?)";

  ಪಕ್ಷ CURLE_SSL_ISSUER_ERROR:
    ಪ್ರತ್ಯಾಗಮ "Issuer check against peer certificate failed";

  ಪಕ್ಷ CURLE_SEND_FAIL_REWIND:
    ಪ್ರತ್ಯಾಗಮ "Send failed since rewinding of the data stream failed";

  ಪಕ್ಷ CURLE_LOGIN_DENIED:
    ಪ್ರತ್ಯಾಗಮ "Login denied";

  ಪಕ್ಷ CURLE_TFTP_NOTFOUND:
    ಪ್ರತ್ಯಾಗಮ "TFTP: File Not Found";

  ಪಕ್ಷ CURLE_TFTP_PERM:
    ಪ್ರತ್ಯಾಗಮ "TFTP: Access Violation";

  ಪಕ್ಷ CURLE_REMOTE_DISK_FULL:
    ಪ್ರತ್ಯಾಗಮ "Disk full or allocation exceeded";

  ಪಕ್ಷ CURLE_TFTP_ILLEGAL:
    ಪ್ರತ್ಯಾಗಮ "TFTP: Illegal operation";

  ಪಕ್ಷ CURLE_TFTP_UNKNOWNID:
    ಪ್ರತ್ಯಾಗಮ "TFTP: Unknown transfer ID";

  ಪಕ್ಷ CURLE_REMOTE_FILE_EXISTS:
    ಪ್ರತ್ಯಾಗಮ "Remote file already exists";

  ಪಕ್ಷ CURLE_TFTP_NOSUCHUSER:
    ಪ್ರತ್ಯಾಗಮ "TFTP: No such user";

  ಪಕ್ಷ CURLE_REMOTE_FILE_NOT_FOUND:
    ಪ್ರತ್ಯಾಗಮ "Remote file not found";

  ಪಕ್ಷ CURLE_SSH:
    ಪ್ರತ್ಯಾಗಮ "Error in the SSH layer";

  ಪಕ್ಷ CURLE_AGAIN:
    ಪ್ರತ್ಯಾಗಮ "Socket not ready for send/recv";

  ಪಕ್ಷ CURLE_RTSP_CSEQ_ERROR:
    ಪ್ರತ್ಯಾಗಮ "RTSP CSeq mismatch or invalid CSeq";

  ಪಕ್ಷ CURLE_RTSP_SESSION_ERROR:
    ಪ್ರತ್ಯಾಗಮ "RTSP session error";

  ಪಕ್ಷ CURLE_FTP_BAD_FILE_LIST:
    ಪ್ರತ್ಯಾಗಮ "Unable to parse FTP file list";

  ಪಕ್ಷ CURLE_CHUNK_FAILED:
    ಪ್ರತ್ಯಾಗಮ "Chunk callback failed";

  ಪಕ್ಷ CURLE_NO_CONNECTION_AVAILABLE:
    ಪ್ರತ್ಯಾಗಮ "The max connection limit is reached";

  ಪಕ್ಷ CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    ಪ್ರತ್ಯಾಗಮ "SSL public key does not match pinned public key";

  ಪಕ್ಷ CURLE_SSL_INVALIDCERTSTATUS:
    ಪ್ರತ್ಯಾಗಮ "SSL server certificate status verification FAILED";

  ಪಕ್ಷ CURLE_HTTP2_STREAM:
    ಪ್ರತ್ಯಾಗಮ "Stream error in the HTTP/2 framing layer";

  ಪಕ್ಷ CURLE_RECURSIVE_API_CALL:
    ಪ್ರತ್ಯಾಗಮ "API function called from within callback";

  ಪಕ್ಷ CURLE_AUTH_ERROR:
    ಪ್ರತ್ಯಾಗಮ "An authentication function returned an error";

  ಪಕ್ಷ CURLE_HTTP3:
    ಪ್ರತ್ಯಾಗಮ "HTTP/3 error";

  ಪಕ್ಷ CURLE_QUIC_CONNECT_ERROR:
    ಪ್ರತ್ಯಾಗಮ "QUIC connection error";

  ಪಕ್ಷ CURLE_PROXY:
    ಪ್ರತ್ಯಾಗಮ "proxy handshake error";

  ಪಕ್ಷ CURLE_SSL_CLIENTCERT:
    ಪ್ರತ್ಯಾಗಮ "SSL Client Certificate required";

  ಪಕ್ಷ CURLE_UNRECOVERABLE_POLL:
    ಪ್ರತ್ಯಾಗಮ "Unrecoverable error in select/poll";

  ಪಕ್ಷ CURLE_TOO_LARGE:
    ಪ್ರತ್ಯಾಗಮ "A value or data field grew larger than allowed";

  ಪಕ್ಷ CURLE_ECH_REQUIRED:
    ಪ್ರತ್ಯಾಗಮ "ECH attempted but failed";

    /* error codes not used by current libcurl */
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
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
  ಪ್ರತ್ಯಾಗಮ "Unknown error";
#ಅನ್ಯಥಾ
  ಯದಿ(!error)
    ಪ್ರತ್ಯಾಗಮ "No error";
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ "Error";
#endif
}

ಧ್ರುವ ವರ್ಣ *curl_multi_strerror(CURLMcode error)
{
#ifdef CURLVERBOSE
  ವಿಕಲ್ಪ(error) {
  ಪಕ್ಷ CURLM_CALL_MULTI_PERFORM:
    ಪ್ರತ್ಯಾಗಮ "Please call curl_multi_perform() soon";

  ಪಕ್ಷ CURLM_OK:
    ಪ್ರತ್ಯಾಗಮ "No error";

  ಪಕ್ಷ CURLM_BAD_HANDLE:
    ಪ್ರತ್ಯಾಗಮ "Invalid multi handle";

  ಪಕ್ಷ CURLM_BAD_EASY_HANDLE:
    ಪ್ರತ್ಯಾಗಮ "Invalid easy handle";

  ಪಕ್ಷ CURLM_OUT_OF_MEMORY:
    ಪ್ರತ್ಯಾಗಮ "Out of memory";

  ಪಕ್ಷ CURLM_INTERNAL_ERROR:
    ಪ್ರತ್ಯಾಗಮ "Internal error";

  ಪಕ್ಷ CURLM_BAD_SOCKET:
    ಪ್ರತ್ಯಾಗಮ "Invalid socket argument";

  ಪಕ್ಷ CURLM_UNKNOWN_OPTION:
    ಪ್ರತ್ಯಾಗಮ "Unknown option";

  ಪಕ್ಷ CURLM_ADDED_ALREADY:
    ಪ್ರತ್ಯಾಗಮ "The easy handle is already added to a multi handle";

  ಪಕ್ಷ CURLM_RECURSIVE_API_CALL:
    ಪ್ರತ್ಯಾಗಮ "API function called from within callback";

  ಪಕ್ಷ CURLM_WAKEUP_FAILURE:
    ಪ್ರತ್ಯಾಗಮ "Wakeup is unavailable or failed";

  ಪಕ್ಷ CURLM_BAD_FUNCTION_ARGUMENT:
    ಪ್ರತ್ಯಾಗಮ "A libcurl function was given a bad argument";

  ಪಕ್ಷ CURLM_ABORTED_BY_CALLBACK:
    ಪ್ರತ್ಯಾಗಮ "Operation was aborted by an application callback";

  ಪಕ್ಷ CURLM_UNRECOVERABLE_POLL:
    ಪ್ರತ್ಯಾಗಮ "Unrecoverable error in select/poll";

  ಪಕ್ಷ CURLM_LAST:
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ "Unknown error";
#ಅನ್ಯಥಾ
  ಯದಿ(error == CURLM_OK)
    ಪ್ರತ್ಯಾಗಮ "No error";
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ "Error";
#endif
}

ಧ್ರುವ ವರ್ಣ *curl_share_strerror(CURLSHcode error)
{
#ifdef CURLVERBOSE
  ವಿಕಲ್ಪ(error) {
  ಪಕ್ಷ CURLSHE_OK:
    ಪ್ರತ್ಯಾಗಮ "No error";

  ಪಕ್ಷ CURLSHE_BAD_OPTION:
    ಪ್ರತ್ಯಾಗಮ "Unknown share option";

  ಪಕ್ಷ CURLSHE_IN_USE:
    ಪ್ರತ್ಯಾಗಮ "Share currently in use";

  ಪಕ್ಷ CURLSHE_INVALID:
    ಪ್ರತ್ಯಾಗಮ "Invalid share handle";

  ಪಕ್ಷ CURLSHE_NOMEM:
    ಪ್ರತ್ಯಾಗಮ "Out of memory";

  ಪಕ್ಷ CURLSHE_NOT_BUILT_IN:
    ಪ್ರತ್ಯಾಗಮ "Feature not enabled in this library";

  ಪಕ್ಷ CURLSHE_LAST:
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ "CURLSHcode unknown";
#ಅನ್ಯಥಾ
  ಯದಿ(error == CURLSHE_OK)
    ಪ್ರತ್ಯಾಗಮ "No error";
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ "Error";
#endif
}

ಧ್ರುವ ವರ್ಣ *curl_url_strerror(CURLUcode error)
{
#ifdef CURLVERBOSE
  ವಿಕಲ್ಪ(error) {
  ಪಕ್ಷ CURLUE_OK:
    ಪ್ರತ್ಯಾಗಮ "No error";

  ಪಕ್ಷ CURLUE_BAD_HANDLE:
    ಪ್ರತ್ಯಾಗಮ "An invalid CURLU pointer was passed as argument";

  ಪಕ್ಷ CURLUE_BAD_PARTPOINTER:
    ಪ್ರತ್ಯಾಗಮ "An invalid 'part' argument was passed as argument";

  ಪಕ್ಷ CURLUE_MALFORMED_ಒಳಹರಿವು:
    ಪ್ರತ್ಯಾಗಮ "Malformed input to a URL function";

  ಪಕ್ಷ CURLUE_BAD_PORT_ಸಂಖ್ಯೆ:
    ಪ್ರತ್ಯಾಗಮ "Port number was not a decimal number between 0 and 65535";

  ಪಕ್ಷ CURLUE_UNSUPPORTED_SCHEME:
    ಪ್ರತ್ಯಾಗಮ "Unsupported URL scheme";

  ಪಕ್ಷ CURLUE_URLDECODE:
    ಪ್ರತ್ಯಾಗಮ "URL decode error, most likely because of rubbish in the input";

  ಪಕ್ಷ CURLUE_OUT_OF_MEMORY:
    ಪ್ರತ್ಯಾಗಮ "A memory function failed";

  ಪಕ್ಷ CURLUE_USER_NOT_ALLOWED:
    ಪ್ರತ್ಯಾಗಮ "Credentials was passed in the URL when prohibited";

  ಪಕ್ಷ CURLUE_UNKNOWN_PART:
    ಪ್ರತ್ಯಾಗಮ "An unknown part ID was passed to a URL API function";

  ಪಕ್ಷ CURLUE_NO_SCHEME:
    ಪ್ರತ್ಯಾಗಮ "No scheme present";

  ಪಕ್ಷ CURLUE_NO_USER:
    ಪ್ರತ್ಯಾಗಮ "No user present";

  ಪಕ್ಷ CURLUE_NO_PASSWORD:
    ಪ್ರತ್ಯಾಗಮ "No password present";

  ಪಕ್ಷ CURLUE_NO_OPTIONS:
    ಪ್ರತ್ಯಾಗಮ "No options present";

  ಪಕ್ಷ CURLUE_NO_HOST:
    ಪ್ರತ್ಯಾಗಮ "No host present";

  ಪಕ್ಷ CURLUE_NO_PORT:
    ಪ್ರತ್ಯಾಗಮ "No port number present";

  ಪಕ್ಷ CURLUE_NO_QUERY:
    ಪ್ರತ್ಯಾಗಮ "No query present";

  ಪಕ್ಷ CURLUE_NO_FRAGMENT:
    ಪ್ರತ್ಯಾಗಮ "No fragment present";

  ಪಕ್ಷ CURLUE_NO_ZONEID:
    ಪ್ರತ್ಯಾಗಮ "No zoneid present";

  ಪಕ್ಷ CURLUE_BAD_LOGIN:
    ಪ್ರತ್ಯಾಗಮ "Bad login";

  ಪಕ್ಷ CURLUE_BAD_IPV6:
    ಪ್ರತ್ಯಾಗಮ "Bad IPv6 address";

  ಪಕ್ಷ CURLUE_BAD_HOSTNAME:
    ಪ್ರತ್ಯಾಗಮ "Bad hostname";

  ಪಕ್ಷ CURLUE_BAD_FILE_URL:
    ಪ್ರತ್ಯಾಗಮ "Bad file:// URL";

  ಪಕ್ಷ CURLUE_BAD_SLASHES:
    ಪ್ರತ್ಯಾಗಮ "Unsupported number of slashes following scheme";

  ಪಕ್ಷ CURLUE_BAD_SCHEME:
    ಪ್ರತ್ಯಾಗಮ "Bad scheme";

  ಪಕ್ಷ CURLUE_BAD_PATH:
    ಪ್ರತ್ಯಾಗಮ "Bad path";

  ಪಕ್ಷ CURLUE_BAD_FRAGMENT:
    ಪ್ರತ್ಯಾಗಮ "Bad fragment";

  ಪಕ್ಷ CURLUE_BAD_QUERY:
    ಪ್ರತ್ಯಾಗಮ "Bad query";

  ಪಕ್ಷ CURLUE_BAD_PASSWORD:
    ಪ್ರತ್ಯಾಗಮ "Bad password";

  ಪಕ್ಷ CURLUE_BAD_USER:
    ಪ್ರತ್ಯಾಗಮ "Bad user";

  ಪಕ್ಷ CURLUE_LACKS_IDN:
    ಪ್ರತ್ಯಾಗಮ "libcurl lacks IDN support";

  ಪಕ್ಷ CURLUE_TOO_LARGE:
    ಪ್ರತ್ಯಾಗಮ "A value or data field is larger than allowed";

  ಪಕ್ಷ CURLUE_BACKSLASH:
    ಪ್ರತ್ಯಾಗಮ "Found a backslash where a forward slash was expected";

  ಪಕ್ಷ CURLUE_LAST:
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ "CURLUcode unknown";
#ಅನ್ಯಥಾ
  ಯದಿ(error == CURLUE_OK)
    ಪ್ರತ್ಯಾಗಮ "No error";
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ "Error";
#endif
}

#ifdef USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ಧ್ರುವ ವರ್ಣ *Curl_sspi_strerror(SECURITY_STATUS err, ವರ್ಣ *buf, size_t buflen)
{
#ifdef _WIN32
  DWORD old_win_err = GetLastError();
#endif
  ಪೂರ್ಣಾಙ್ಕ old_errno = errno;
  VERBOSE(ಧ್ರುವ ವರ್ಣ *txt);

  ಯದಿ(!buflen)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  *buf = '\0';

#ifdef CURLVERBOSE
  ವಿಕಲ್ಪ(err) {
  ಪಕ್ಷ SEC_E_OK:
    txt = "No error";
    ಭಙ್ಗ;
#define SEC2TXT(sec) ಪಕ್ಷ sec: txt = #sec; ಭಙ್ಗ
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
  SEC2TXT(SEC_E_NO_KERB_ಕೀಲಿ);
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
  SEC2TXT(SEC_E_UNTRUSTED_ಬೇರು);
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
  ಸ್ವಭಾವ:
    txt = "Unknown error";
  }

  ಯದಿ(err == SEC_E_ILLEGAL_MESSAGE) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (unsigned long)err);
  }
  ಅನ್ಯಥಾ {
    ವರ್ಣ msgbuf[256];
    ಯದಿ(curlx_get_winapi_error((DWORD)err, msgbuf, ಪರಿಮಾಣ(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (unsigned long)err,
                     msgbuf);
    ಅನ್ಯಥಾ
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (unsigned long)err);
  }
#ಅನ್ಯಥಾ /* CURLVERBOSE */
  ಯದಿ(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  ಅನ್ಯಥಾ
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#endif

  ಯದಿ(errno != old_errno)
    errno = old_errno;

#ifdef _WIN32
  ಯದಿ(old_win_err != GetLastError())
    SetLastError(old_win_err);
#endif

  ಪ್ರತ್ಯಾಗಮ buf;
}
#endif /* USE_WINDOWS_SSPI */
