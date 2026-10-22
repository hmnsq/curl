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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
#ಅನ್ತರ್ಭಾವ "curl_sspi.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curlx/winapi.h"
#ಅನ್ತರ್ಭಾವ "strerror.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curl_easy_strerror(CURLcode error)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆಯ್ಕೆ(error) {
  ಸ್ಥಿತಿ CURLE_OK:
    ಹಿಂತಿರುಗಿ "No error";

  ಸ್ಥಿತಿ CURLE_UNSUPPORTED_PROTOCOL:
    ಹಿಂತಿರುಗಿ "Unsupported protocol";

  ಸ್ಥಿತಿ CURLE_FAILED_INIT:
    ಹಿಂತಿರುಗಿ "Failed initialization";

  ಸ್ಥಿತಿ CURLE_URL_MALFORMAT:
    ಹಿಂತಿರುಗಿ "URL using bad/illegal format or missing URL";

  ಸ್ಥಿತಿ CURLE_NOT_BUILT_ಒಳ:
    ಹಿಂತಿರುಗಿ "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  ಸ್ಥಿತಿ CURLE_COULDNT_RESOLVE_PROXY:
    ಹಿಂತಿರುಗಿ "Could not resolve proxy name";

  ಸ್ಥಿತಿ CURLE_COULDNT_RESOLVE_HOST:
    ಹಿಂತಿರುಗಿ "Could not resolve hostname";

  ಸ್ಥಿತಿ CURLE_COULDNT_CONNECT:
    ಹಿಂತಿರುಗಿ "Could not connect to server";

  ಸ್ಥಿತಿ CURLE_WEIRD_SERVER_ಉತ್ತರ:
    ಹಿಂತಿರುಗಿ "Weird server reply";

  ಸ್ಥಿತಿ CURLE_REMOTE_ACCESS_DENIED:
    ಹಿಂತಿರುಗಿ "Access denied to remote resource";

  ಸ್ಥಿತಿ CURLE_FTP_ACCEPT_FAILED:
    ಹಿಂತಿರುಗಿ "FTP: The server failed to connect to data port";

  ಸ್ಥಿತಿ CURLE_FTP_ACCEPT_TIMEOUT:
    ಹಿಂತಿರುಗಿ "FTP: Accepting server connect has timed out";

  ಸ್ಥಿತಿ CURLE_FTP_PRET_FAILED:
    ಹಿಂತಿರುಗಿ "FTP: The server did not accept the PRET command.";

  ಸ್ಥಿತಿ CURLE_FTP_WEIRD_PASS_ಉತ್ತರ:
    ಹಿಂತಿರುಗಿ "FTP: unknown PASS reply";

  ಸ್ಥಿತಿ CURLE_FTP_WEIRD_PASV_ಉತ್ತರ:
    ಹಿಂತಿರುಗಿ "FTP: unknown PASV reply";

  ಸ್ಥಿತಿ CURLE_FTP_WEIRD_227_FORMAT:
    ಹಿಂತಿರುಗಿ "FTP: unknown 227 response format";

  ಸ್ಥಿತಿ CURLE_FTP_CANT_GET_HOST:
    ಹಿಂತಿರುಗಿ "FTP: cannot figure out the host in the PASV response";

  ಸ್ಥಿತಿ CURLE_HTTP2:
    ಹಿಂತಿರುಗಿ "Error in the HTTP2 framing layer";

  ಸ್ಥಿತಿ CURLE_FTP_COULDNT_ಗಣ_TYPE:
    ಹಿಂತಿರುಗಿ "FTP: could not set file type";

  ಸ್ಥಿತಿ CURLE_PARTIAL_FILE:
    ಹಿಂತಿರುಗಿ "Transferred a partial file";

  ಸ್ಥಿತಿ CURLE_FTP_COULDNT_RETR_FILE:
    ಹಿಂತಿರುಗಿ "FTP: could not retrieve (RETR failed) the specified file";

  ಸ್ಥಿತಿ CURLE_QUOTE_ERROR:
    ಹಿಂತಿರುಗಿ "Quote command returned error";

  ಸ್ಥಿತಿ CURLE_HTTP_RETURNED_ERROR:
    ಹಿಂತಿರುಗಿ "HTTP response code said error";

  ಸ್ಥಿತಿ CURLE_WRITE_ERROR:
    ಹಿಂತಿರುಗಿ "Failed writing received data to disk/application";

  ಸ್ಥಿತಿ CURLE_UPLOAD_FAILED:
    ಹಿಂತಿರುಗಿ "Upload failed (at start/before it took off)";

  ಸ್ಥಿತಿ CURLE_READ_ERROR:
    ಹಿಂತಿರುಗಿ "Failed to open/read local data from file/application";

  ಸ್ಥಿತಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ:
    ಹಿಂತಿರುಗಿ "Out of memory";

  ಸ್ಥಿತಿ CURLE_OPERATION_TIMEDOUT:
    ಹಿಂತಿರುಗಿ "Timeout was reached";

  ಸ್ಥಿತಿ CURLE_FTP_ಪೋರ್ಟ್_FAILED:
    ಹಿಂತಿರುಗಿ "FTP: command PORT failed";

  ಸ್ಥಿತಿ CURLE_FTP_COULDNT_USE_REST:
    ಹಿಂತಿರುಗಿ "FTP: command REST failed";

  ಸ್ಥಿತಿ CURLE_RANGE_ERROR:
    ಹಿಂತಿರುಗಿ "Requested range was not delivered by the server";

  ಸ್ಥಿತಿ CURLE_SSL_CONNECT_ERROR:
    ಹಿಂತಿರುಗಿ "SSL connect error";

  ಸ್ಥಿತಿ CURLE_BAD_DOWNLOAD_RESUME:
    ಹಿಂತಿರುಗಿ "Could not resume download";

  ಸ್ಥಿತಿ CURLE_FILE_COULDNT_READ_FILE:
    ಹಿಂತಿರುಗಿ "Could not read a file:// file";

  ಸ್ಥಿತಿ CURLE_LDAP_CANNOT_BIND:
    ಹಿಂತಿರುಗಿ "LDAP: cannot bind";

  ಸ್ಥಿತಿ CURLE_LDAP_ಹುಡುಕು_FAILED:
    ಹಿಂತಿರುಗಿ "LDAP: search failed";

  ಸ್ಥಿತಿ CURLE_ABORTED_BY_CALLBACK:
    ಹಿಂತಿರುಗಿ "Operation was aborted by an application callback";

  ಸ್ಥಿತಿ CURLE_BAD_FUNCTION_ವಾದ:
    ಹಿಂತಿರುಗಿ "A libcurl function was given a bad argument";

  ಸ್ಥಿತಿ CURLE_INTERFACE_FAILED:
    ಹಿಂತಿರುಗಿ "Failed binding local connection end";

  ಸ್ಥಿತಿ CURLE_TOO_MANY_REDIRECTS:
    ಹಿಂತಿರುಗಿ "Number of redirects hit maximum amount";

  ಸ್ಥಿತಿ CURLE_UNKNOWN_OPTION:
    ಹಿಂತಿರುಗಿ "An unknown option was passed in to libcurl";

  ಸ್ಥಿತಿ CURLE_SETOPT_OPTION_SYNTAX:
    ಹಿಂತಿರುಗಿ "Malformed option provided in a setopt";

  ಸ್ಥಿತಿ CURLE_GOT_NOTHING:
    ಹಿಂತಿರುಗಿ "Server returned nothing (no headers, no data)";

  ಸ್ಥಿತಿ CURLE_SSL_ENGINE_NOTFOUND:
    ಹಿಂತಿರುಗಿ "SSL crypto engine not found";

  ಸ್ಥಿತಿ CURLE_SSL_ENGINE_SETFAILED:
    ಹಿಂತಿರುಗಿ "Can not set SSL crypto engine as default";

  ಸ್ಥಿತಿ CURLE_SSL_ENGINE_INITFAILED:
    ಹಿಂತಿರುಗಿ "Failed to initialize SSL crypto engine";

  ಸ್ಥಿತಿ CURLE_SEND_ERROR:
    ಹಿಂತಿರುಗಿ "Failed sending data to the peer";

  ಸ್ಥಿತಿ CURLE_RECV_ERROR:
    ಹಿಂತಿರುಗಿ "Failure when receiving data from the peer";

  ಸ್ಥಿತಿ CURLE_SSL_CERTPROBLEM:
    ಹಿಂತಿರುಗಿ "Problem with the local SSL certificate";

  ಸ್ಥಿತಿ CURLE_SSL_CIPHER:
    ಹಿಂತಿರುಗಿ "Could not use specified SSL cipher";

  ಸ್ಥಿತಿ CURLE_PEER_FAILED_VERIFICATION:
    ಹಿಂತಿರುಗಿ "SSL peer certificate or SSH remote key was not OK";

  ಸ್ಥಿತಿ CURLE_SSL_CACERT_BADFILE:
    ಹಿಂತಿರುಗಿ "Problem with the SSL CA cert (path? access rights?)";

  ಸ್ಥಿತಿ CURLE_BAD_CONTENT_ENCODING:
    ಹಿಂತಿರುಗಿ "Unrecognized or bad HTTP Content or Transfer-Encoding";

  ಸ್ಥಿತಿ CURLE_FILESIZE_EXCEEDED:
    ಹಿಂತಿರುಗಿ "Maximum file size exceeded";

  ಸ್ಥಿತಿ CURLE_USE_SSL_FAILED:
    ಹಿಂತಿರುಗಿ "Requested SSL level failed";

  ಸ್ಥಿತಿ CURLE_SSL_SHUTDOWN_FAILED:
    ಹಿಂತಿರುಗಿ "Failed to shut down the SSL connection";

  ಸ್ಥಿತಿ CURLE_SSL_CRL_BADFILE:
    ಹಿಂತಿರುಗಿ "Failed to load CRL file (path? access rights?, format?)";

  ಸ್ಥಿತಿ CURLE_SSL_ISSUER_ERROR:
    ಹಿಂತಿರುಗಿ "Issuer check against peer certificate failed";

  ಸ್ಥಿತಿ CURLE_SEND_FAIL_REWIND:
    ಹಿಂತಿರುಗಿ "Send failed since rewinding of the data stream failed";

  ಸ್ಥಿತಿ CURLE_LOGIN_DENIED:
    ಹಿಂತಿರುಗಿ "Login denied";

  ಸ್ಥಿತಿ CURLE_TFTP_NOTFOUND:
    ಹಿಂತಿರುಗಿ "TFTP: File Not Found";

  ಸ್ಥಿತಿ CURLE_TFTP_PERM:
    ಹಿಂತಿರುಗಿ "TFTP: Access Violation";

  ಸ್ಥಿತಿ CURLE_REMOTE_DISK_ತುಂಬಿದ:
    ಹಿಂತಿರುಗಿ "Disk full or allocation exceeded";

  ಸ್ಥಿತಿ CURLE_TFTP_ILLEGAL:
    ಹಿಂತಿರುಗಿ "TFTP: Illegal operation";

  ಸ್ಥಿತಿ CURLE_TFTP_UNKNOWNID:
    ಹಿಂತಿರುಗಿ "TFTP: Unknown transfer ID";

  ಸ್ಥಿತಿ CURLE_REMOTE_FILE_EXISTS:
    ಹಿಂತಿರುಗಿ "Remote file already exists";

  ಸ್ಥಿತಿ CURLE_TFTP_NOSUCHUSER:
    ಹಿಂತಿರುಗಿ "TFTP: No such user";

  ಸ್ಥಿತಿ CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು:
    ಹಿಂತಿರುಗಿ "Remote file not found";

  ಸ್ಥಿತಿ CURLE_SSH:
    ಹಿಂತಿರುಗಿ "Error in the SSH layer";

  ಸ್ಥಿತಿ CURLE_AGAIN:
    ಹಿಂತಿರುಗಿ "Socket not ready for send/recv";

  ಸ್ಥಿತಿ CURLE_RTSP_CSEQ_ERROR:
    ಹಿಂತಿರುಗಿ "RTSP CSeq mismatch or invalid CSeq";

  ಸ್ಥಿತಿ CURLE_RTSP_SESSION_ERROR:
    ಹಿಂತಿರುಗಿ "RTSP session error";

  ಸ್ಥಿತಿ CURLE_FTP_BAD_FILE_LIST:
    ಹಿಂತಿರುಗಿ "Unable to parse FTP file list";

  ಸ್ಥಿತಿ CURLE_CHUNK_FAILED:
    ಹಿಂತಿರುಗಿ "Chunk callback failed";

  ಸ್ಥಿತಿ CURLE_NO_CONNECTION_ಲಭ್ಯ:
    ಹಿಂತಿರುಗಿ "The max connection limit is reached";

  ಸ್ಥಿತಿ CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    ಹಿಂತಿರುಗಿ "SSL public key does not match pinned public key";

  ಸ್ಥಿತಿ CURLE_SSL_INVALIDCERTSTATUS:
    ಹಿಂತಿರುಗಿ "SSL server certificate status verification FAILED";

  ಸ್ಥಿತಿ CURLE_HTTP2_ಹರಿವು:
    ಹಿಂತಿರುಗಿ "Stream error in the HTTP/2 framing layer";

  ಸ್ಥಿತಿ CURLE_RECURSIVE_API_CALL:
    ಹಿಂತಿರುಗಿ "API function called from within callback";

  ಸ್ಥಿತಿ CURLE_AUTH_ERROR:
    ಹಿಂತಿರುಗಿ "An authentication function returned an error";

  ಸ್ಥಿತಿ CURLE_HTTP3:
    ಹಿಂತಿರುಗಿ "HTTP/3 error";

  ಸ್ಥಿತಿ CURLE_QUIC_CONNECT_ERROR:
    ಹಿಂತಿರುಗಿ "QUIC connection error";

  ಸ್ಥಿತಿ CURLE_PROXY:
    ಹಿಂತಿರುಗಿ "proxy handshake error";

  ಸ್ಥಿತಿ CURLE_SSL_CLIENTCERT:
    ಹಿಂತಿರುಗಿ "SSL Client Certificate required";

  ಸ್ಥಿತಿ CURLE_UNRECOVERABLE_POLL:
    ಹಿಂತಿರುಗಿ "Unrecoverable error in select/poll";

  ಸ್ಥಿತಿ CURLE_TOO_LARGE:
    ಹಿಂತಿರುಗಿ "A value or data field grew larger than allowed";

  ಸ್ಥಿತಿ CURLE_ECH_REQUIRED:
    ಹಿಂತಿರುಗಿ "ECH attempted but failed";

    /* error codes not used by current libcurl */
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
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
  ಹಿಂತಿರುಗಿ "Unknown error";
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(!error)
    ಹಿಂತಿರುಗಿ "No error";
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ "Error";
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curl_multi_strerror(CURLMcode error)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆಯ್ಕೆ(error) {
  ಸ್ಥಿತಿ CURLM_CALL_MULTI_PERFORM:
    ಹಿಂತಿರುಗಿ "Please call curl_multi_perform() soon";

  ಸ್ಥಿತಿ CURLM_OK:
    ಹಿಂತಿರುಗಿ "No error";

  ಸ್ಥಿತಿ CURLM_BAD_HANDLE:
    ಹಿಂತಿರುಗಿ "Invalid multi handle";

  ಸ್ಥಿತಿ CURLM_BAD_EASY_HANDLE:
    ಹಿಂತಿರುಗಿ "Invalid easy handle";

  ಸ್ಥಿತಿ CURLM_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ:
    ಹಿಂತಿರುಗಿ "Out of memory";

  ಸ್ಥಿತಿ CURLM_ಆಂತರಿಕ_ERROR:
    ಹಿಂತಿರುಗಿ "Internal error";

  ಸ್ಥಿತಿ CURLM_BAD_SOCKET:
    ಹಿಂತಿರುಗಿ "Invalid socket argument";

  ಸ್ಥಿತಿ CURLM_UNKNOWN_OPTION:
    ಹಿಂತಿರುಗಿ "Unknown option";

  ಸ್ಥಿತಿ CURLM_ADDED_ALREADY:
    ಹಿಂತಿರುಗಿ "The easy handle is already added to a multi handle";

  ಸ್ಥಿತಿ CURLM_RECURSIVE_API_CALL:
    ಹಿಂತಿರುಗಿ "API function called from within callback";

  ಸ್ಥಿತಿ CURLM_WAKEUP_FAILURE:
    ಹಿಂತಿರುಗಿ "Wakeup is unavailable or failed";

  ಸ್ಥಿತಿ CURLM_BAD_FUNCTION_ವಾದ:
    ಹಿಂತಿರುಗಿ "A libcurl function was given a bad argument";

  ಸ್ಥಿತಿ CURLM_ABORTED_BY_CALLBACK:
    ಹಿಂತಿರುಗಿ "Operation was aborted by an application callback";

  ಸ್ಥಿತಿ CURLM_UNRECOVERABLE_POLL:
    ಹಿಂತಿರುಗಿ "Unrecoverable error in select/poll";

  ಸ್ಥಿತಿ CURLM_ಕೊನೆಯ:
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ "Unknown error";
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(error == CURLM_OK)
    ಹಿಂತಿರುಗಿ "No error";
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ "Error";
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curl_share_strerror(CURLSHcode error)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆಯ್ಕೆ(error) {
  ಸ್ಥಿತಿ CURLSHE_OK:
    ಹಿಂತಿರುಗಿ "No error";

  ಸ್ಥಿತಿ CURLSHE_BAD_OPTION:
    ಹಿಂತಿರುಗಿ "Unknown share option";

  ಸ್ಥಿತಿ CURLSHE_ಒಳ_USE:
    ಹಿಂತಿರುಗಿ "Share currently in use";

  ಸ್ಥಿತಿ CURLSHE_INVALID:
    ಹಿಂತಿರುಗಿ "Invalid share handle";

  ಸ್ಥಿತಿ CURLSHE_NOMEM:
    ಹಿಂತಿರುಗಿ "Out of memory";

  ಸ್ಥಿತಿ CURLSHE_NOT_BUILT_ಒಳ:
    ಹಿಂತಿರುಗಿ "Feature not enabled in this library";

  ಸ್ಥಿತಿ CURLSHE_ಕೊನೆಯ:
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ "CURLSHcode unknown";
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(error == CURLSHE_OK)
    ಹಿಂತಿರುಗಿ "No error";
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ "Error";
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curl_url_strerror(CURLUcode error)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆಯ್ಕೆ(error) {
  ಸ್ಥಿತಿ CURLUE_OK:
    ಹಿಂತಿರುಗಿ "No error";

  ಸ್ಥಿತಿ CURLUE_BAD_HANDLE:
    ಹಿಂತಿರುಗಿ "An invalid CURLU pointer was passed as argument";

  ಸ್ಥಿತಿ CURLUE_BAD_PARTPOINTER:
    ಹಿಂತಿರುಗಿ "An invalid 'part' argument was passed as argument";

  ಸ್ಥಿತಿ CURLUE_MALFORMED_ಒಳಹರಿವು:
    ಹಿಂತಿರುಗಿ "Malformed input to a URL function";

  ಸ್ಥಿತಿ CURLUE_BAD_ಪೋರ್ಟ್_ಸಂಖ್ಯೆ:
    ಹಿಂತಿರುಗಿ "Port number was not a decimal number between 0 and 65535";

  ಸ್ಥಿತಿ CURLUE_UNSUPPORTED_SCHEME:
    ಹಿಂತಿರುಗಿ "Unsupported URL scheme";

  ಸ್ಥಿತಿ CURLUE_URLDECODE:
    ಹಿಂತಿರುಗಿ "URL decode error, most likely because of rubbish in the input";

  ಸ್ಥಿತಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ:
    ಹಿಂತಿರುಗಿ "A memory function failed";

  ಸ್ಥಿತಿ CURLUE_USER_NOT_ALLOWED:
    ಹಿಂತಿರುಗಿ "Credentials was passed in the URL when prohibited";

  ಸ್ಥಿತಿ CURLUE_UNKNOWN_PART:
    ಹಿಂತಿರುಗಿ "An unknown part ID was passed to a URL API function";

  ಸ್ಥಿತಿ CURLUE_NO_SCHEME:
    ಹಿಂತಿರುಗಿ "No scheme present";

  ಸ್ಥಿತಿ CURLUE_NO_USER:
    ಹಿಂತಿರುಗಿ "No user present";

  ಸ್ಥಿತಿ CURLUE_NO_PASSWORD:
    ಹಿಂತಿರುಗಿ "No password present";

  ಸ್ಥಿತಿ CURLUE_NO_OPTIONS:
    ಹಿಂತಿರುಗಿ "No options present";

  ಸ್ಥಿತಿ CURLUE_NO_HOST:
    ಹಿಂತಿರುಗಿ "No host present";

  ಸ್ಥಿತಿ CURLUE_NO_ಪೋರ್ಟ್:
    ಹಿಂತಿರುಗಿ "No port number present";

  ಸ್ಥಿತಿ CURLUE_NO_QUERY:
    ಹಿಂತಿರುಗಿ "No query present";

  ಸ್ಥಿತಿ CURLUE_NO_FRAGMENT:
    ಹಿಂತಿರುಗಿ "No fragment present";

  ಸ್ಥಿತಿ CURLUE_NO_ZONEID:
    ಹಿಂತಿರುಗಿ "No zoneid present";

  ಸ್ಥಿತಿ CURLUE_BAD_LOGIN:
    ಹಿಂತಿರುಗಿ "Bad login";

  ಸ್ಥಿತಿ CURLUE_BAD_IPV6:
    ಹಿಂತಿರುಗಿ "Bad IPv6 address";

  ಸ್ಥಿತಿ CURLUE_BAD_HOSTNAME:
    ಹಿಂತಿರುಗಿ "Bad hostname";

  ಸ್ಥಿತಿ CURLUE_BAD_FILE_URL:
    ಹಿಂತಿರುಗಿ "Bad file:// URL";

  ಸ್ಥಿತಿ CURLUE_BAD_SLASHES:
    ಹಿಂತಿರುಗಿ "Unsupported number of slashes following scheme";

  ಸ್ಥಿತಿ CURLUE_BAD_SCHEME:
    ಹಿಂತಿರುಗಿ "Bad scheme";

  ಸ್ಥಿತಿ CURLUE_BAD_ಮಾರ್ಗ:
    ಹಿಂತಿರುಗಿ "Bad path";

  ಸ್ಥಿತಿ CURLUE_BAD_FRAGMENT:
    ಹಿಂತಿರುಗಿ "Bad fragment";

  ಸ್ಥಿತಿ CURLUE_BAD_QUERY:
    ಹಿಂತಿರುಗಿ "Bad query";

  ಸ್ಥಿತಿ CURLUE_BAD_PASSWORD:
    ಹಿಂತಿರುಗಿ "Bad password";

  ಸ್ಥಿತಿ CURLUE_BAD_USER:
    ಹಿಂತಿರುಗಿ "Bad user";

  ಸ್ಥಿತಿ CURLUE_LACKS_IDN:
    ಹಿಂತಿರುಗಿ "libcurl lacks IDN support";

  ಸ್ಥಿತಿ CURLUE_TOO_LARGE:
    ಹಿಂತಿರುಗಿ "A value or data field is larger than allowed";

  ಸ್ಥಿತಿ CURLUE_BACKSLASH:
    ಹಿಂತಿರುಗಿ "Found a backslash where a forward slash was expected";

  ಸ್ಥಿತಿ CURLUE_ಕೊನೆಯ:
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ "CURLUcode unknown";
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(error == CURLUE_OK)
    ಹಿಂತಿರುಗಿ "No error";
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ "Error";
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_sspi_strerror(SECURITY_ಸ್ಥಿತಿ err, ಅಕ್ಷರ *buf, size_t buflen)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  DWORD old_win_err = Get_ಕೊನೆಯ_Error();
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ old_errno = errno;
  VERBOSE(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *txt);

  ಆದರೆ(!buflen)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  *buf = '\0';

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆಯ್ಕೆ(err) {
  ಸ್ಥಿತಿ SEC_E_OK:
    txt = "No error";
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿಸು SEC2TXT(sec) ಸ್ಥಿತಿ sec: txt = #sec; ನಿಲ್ಲಿಸು
  SEC2TXT(CRYPT_E_REVOKED);
  SEC2TXT(CRYPT_E_NO_REVOCATION_DLL);
  SEC2TXT(CRYPT_E_NO_REVOCATION_CHECK);
  SEC2TXT(CRYPT_E_REVOCATION_OFFLINE);
  SEC2TXT(CRYPT_E_NOT_ಒಳ_REVOCATION_DATABASE);
  SEC2TXT(SEC_E_ALGORITHM_MISMATCH);
  SEC2TXT(SEC_E_BAD_BINDINGS);
  SEC2TXT(SEC_E_BAD_PKGID);
  SEC2TXT(SEC_E_ಬಫರ್_TOO_SMALL);
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
  SEC2TXT(SEC_E_ILLEGAL_ಸಂದೇಶ);
  SEC2TXT(SEC_E_INCOMPLETE_CREDENTIALS);
  SEC2TXT(SEC_E_INCOMPLETE_ಸಂದೇಶ);
  SEC2TXT(SEC_E_INSUFFICIENT_ಸ್ಮರಣೆ);
  SEC2TXT(SEC_E_ಆಂತರಿಕ_ERROR);
  SEC2TXT(SEC_E_INVALID_HANDLE);
  SEC2TXT(SEC_E_INVALID_PARAMETER);
  SEC2TXT(SEC_E_INVALID_TOKEN);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED_KDC);
  SEC2TXT(SEC_E_KDC_CERT_EXPIRED);
  SEC2TXT(SEC_E_KDC_CERT_REVOKED);
  SEC2TXT(SEC_E_KDC_INVALID_REQUEST);
  SEC2TXT(SEC_E_KDC_UNABLE_ಗಮ್ಯ_REFER);
  SEC2TXT(SEC_E_KDC_UNKNOWN_ETYPE);
  SEC2TXT(SEC_E_LOGON_DENIED);
  SEC2TXT(SEC_E_MAX_REFERRALS_EXCEEDED);
  SEC2TXT(SEC_E_ಸಂದೇಶ_ALTERED);
  SEC2TXT(SEC_E_ಅಪವರ್ತ್ಯ_ACCOUNTS);
  SEC2TXT(SEC_E_MUST_BE_KDC);
  SEC2TXT(SEC_E_NOT_OWNER);
  SEC2TXT(SEC_E_NO_AUTHENTICATING_AUTHORITY);
  SEC2TXT(SEC_E_NO_CREDENTIALS);
  SEC2TXT(SEC_E_NO_IMPERSONATION);
  SEC2TXT(SEC_E_NO_IP_ವಿಳಾಸಗಳು);
  SEC2TXT(SEC_E_NO_KERB_ಕೀಲಿ);
  SEC2TXT(SEC_E_NO_PA_ದತ್ತಾಂಶ);
  SEC2TXT(SEC_E_NO_S4U_PROT_SUPPORT);
  SEC2TXT(SEC_E_NO_TGT_ಉತ್ತರ);
  SEC2TXT(SEC_E_ಹೊರಹೋಗು_ದ_SEQUENCE);
  SEC2TXT(SEC_E_PKINIT_CLIENT_FAILURE);
  SEC2TXT(SEC_E_PKINIT_NAME_MISMATCH);
  SEC2TXT(SEC_E_POLICY_NLTM_ONLY);
  SEC2TXT(SEC_E_QOP_NOT_SUPPORTED);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_C);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_KDC);
  SEC2TXT(SEC_E_SECPKG_NOT_ಸಿಕ್ಕಿತು);
  SEC2TXT(SEC_E_SECURITY_QOS_FAILED);
  SEC2TXT(SEC_E_SHUTDOWN_ಒಳ_ಪ್ರಗತಿ);
  SEC2TXT(SEC_E_SMARTCARD_CERT_EXPIRED);
  SEC2TXT(SEC_E_SMARTCARD_CERT_REVOKED);
  SEC2TXT(SEC_E_SMARTCARD_LOGON_REQUIRED);
  SEC2TXT(SEC_E_STRONG_CRYPTO_NOT_SUPPORTED);
  SEC2TXT(SEC_E_ಗುರಿ_UNKNOWN);
  SEC2TXT(SEC_E_ಸಮಯ_SKEW);
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
  ಪೂರ್ವನಿಯೋಜಿತ:
    txt = "Unknown error";
  }

  ಆದರೆ(err == SEC_E_ILLEGAL_ಸಂದೇಶ) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)err);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಅಕ್ಷರ msgbuf[256];
    ಆದರೆ(curlx_get_winapi_error((DWORD)err, msgbuf, ಗಾತ್ರ(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)err,
                     msgbuf);
    ಇಲ್ಲದಿದ್ದರೆ
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)err);
  }
#ಇಲ್ಲದಿದ್ದರೆ /* CURLVERBOSE */
  ಆದರೆ(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  ಇಲ್ಲದಿದ್ದರೆ
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(errno != old_errno)
    errno = old_errno;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  ಆದರೆ(old_win_err != Get_ಕೊನೆಯ_Error())
    ಗಣ_ಕೊನೆಯ_Error(old_win_err);
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ buf;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_WINDOWS_SSPI */
