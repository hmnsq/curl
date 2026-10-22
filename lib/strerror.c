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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sspi.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/winapi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strerror.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

ਅਚਲ ਅੱਖਰ *curl_easy_strerror(CURLcode error)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਚੋਣ(error) {
  ਹਾਲਤ CURLE_OK:
    ਵਾਪਸ "No error";

  ਹਾਲਤ CURLE_UNSUPPORTED_PROTOCOL:
    ਵਾਪਸ "Unsupported protocol";

  ਹਾਲਤ CURLE_FAILED_INIT:
    ਵਾਪਸ "Failed initialization";

  ਹਾਲਤ CURLE_URL_MALFORMAT:
    ਵਾਪਸ "URL using bad/illegal format or missing URL";

  ਹਾਲਤ CURLE_NOT_BUILT_ਵਿੱਚ:
    ਵਾਪਸ "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  ਹਾਲਤ CURLE_COULDNT_RESOLVE_PROXY:
    ਵਾਪਸ "Could not resolve proxy name";

  ਹਾਲਤ CURLE_COULDNT_RESOLVE_HOST:
    ਵਾਪਸ "Could not resolve hostname";

  ਹਾਲਤ CURLE_COULDNT_CONNECT:
    ਵਾਪਸ "Could not connect to server";

  ਹਾਲਤ CURLE_WEIRD_SERVER_ਜਵਾਬ:
    ਵਾਪਸ "Weird server reply";

  ਹਾਲਤ CURLE_REMOTE_ACCESS_DENIED:
    ਵਾਪਸ "Access denied to remote resource";

  ਹਾਲਤ CURLE_FTP_ACCEPT_FAILED:
    ਵਾਪਸ "FTP: The server failed to connect to data port";

  ਹਾਲਤ CURLE_FTP_ACCEPT_TIMEOUT:
    ਵਾਪਸ "FTP: Accepting server connect has timed out";

  ਹਾਲਤ CURLE_FTP_PRET_FAILED:
    ਵਾਪਸ "FTP: The server did not accept the PRET command.";

  ਹਾਲਤ CURLE_FTP_WEIRD_PASS_ਜਵਾਬ:
    ਵਾਪਸ "FTP: unknown PASS reply";

  ਹਾਲਤ CURLE_FTP_WEIRD_PASV_ਜਵਾਬ:
    ਵਾਪਸ "FTP: unknown PASV reply";

  ਹਾਲਤ CURLE_FTP_WEIRD_227_FORMAT:
    ਵਾਪਸ "FTP: unknown 227 response format";

  ਹਾਲਤ CURLE_FTP_CANT_GET_HOST:
    ਵਾਪਸ "FTP: cannot figure out the host in the PASV response";

  ਹਾਲਤ CURLE_HTTP2:
    ਵਾਪਸ "Error in the HTTP2 framing layer";

  ਹਾਲਤ CURLE_FTP_COULDNT_ਸੈੱਟ_TYPE:
    ਵਾਪਸ "FTP: could not set file type";

  ਹਾਲਤ CURLE_PARTIAL_FILE:
    ਵਾਪਸ "Transferred a partial file";

  ਹਾਲਤ CURLE_FTP_COULDNT_RETR_FILE:
    ਵਾਪਸ "FTP: could not retrieve (RETR failed) the specified file";

  ਹਾਲਤ CURLE_QUOTE_ERROR:
    ਵਾਪਸ "Quote command returned error";

  ਹਾਲਤ CURLE_HTTP_RETURNED_ERROR:
    ਵਾਪਸ "HTTP response code said error";

  ਹਾਲਤ CURLE_WRITE_ERROR:
    ਵਾਪਸ "Failed writing received data to disk/application";

  ਹਾਲਤ CURLE_UPLOAD_FAILED:
    ਵਾਪਸ "Upload failed (at start/before it took off)";

  ਹਾਲਤ CURLE_READ_ERROR:
    ਵਾਪਸ "Failed to open/read local data from file/application";

  ਹਾਲਤ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ:
    ਵਾਪਸ "Out of memory";

  ਹਾਲਤ CURLE_OPERATION_TIMEDOUT:
    ਵਾਪਸ "Timeout was reached";

  ਹਾਲਤ CURLE_FTP_ਪੋਰਟ_FAILED:
    ਵਾਪਸ "FTP: command PORT failed";

  ਹਾਲਤ CURLE_FTP_COULDNT_USE_REST:
    ਵਾਪਸ "FTP: command REST failed";

  ਹਾਲਤ CURLE_RANGE_ERROR:
    ਵਾਪਸ "Requested range was not delivered by the server";

  ਹਾਲਤ CURLE_SSL_CONNECT_ERROR:
    ਵਾਪਸ "SSL connect error";

  ਹਾਲਤ CURLE_BAD_DOWNLOAD_RESUME:
    ਵਾਪਸ "Could not resume download";

  ਹਾਲਤ CURLE_FILE_COULDNT_READ_FILE:
    ਵਾਪਸ "Could not read a file:// file";

  ਹਾਲਤ CURLE_LDAP_CANNOT_BIND:
    ਵਾਪਸ "LDAP: cannot bind";

  ਹਾਲਤ CURLE_LDAP_ਖੋਜ_FAILED:
    ਵਾਪਸ "LDAP: search failed";

  ਹਾਲਤ CURLE_ABORTED_BY_CALLBACK:
    ਵਾਪਸ "Operation was aborted by an application callback";

  ਹਾਲਤ CURLE_BAD_FUNCTION_ਦਲੀਲ:
    ਵਾਪਸ "A libcurl function was given a bad argument";

  ਹਾਲਤ CURLE_INTERFACE_FAILED:
    ਵਾਪਸ "Failed binding local connection end";

  ਹਾਲਤ CURLE_TOO_MANY_REDIRECTS:
    ਵਾਪਸ "Number of redirects hit maximum amount";

  ਹਾਲਤ CURLE_UNKNOWN_OPTION:
    ਵਾਪਸ "An unknown option was passed in to libcurl";

  ਹਾਲਤ CURLE_SETOPT_OPTION_SYNTAX:
    ਵਾਪਸ "Malformed option provided in a setopt";

  ਹਾਲਤ CURLE_GOT_NOTHING:
    ਵਾਪਸ "Server returned nothing (no headers, no data)";

  ਹਾਲਤ CURLE_SSL_ENGINE_NOTFOUND:
    ਵਾਪਸ "SSL crypto engine not found";

  ਹਾਲਤ CURLE_SSL_ENGINE_SETFAILED:
    ਵਾਪਸ "Can not set SSL crypto engine as default";

  ਹਾਲਤ CURLE_SSL_ENGINE_INITFAILED:
    ਵਾਪਸ "Failed to initialize SSL crypto engine";

  ਹਾਲਤ CURLE_SEND_ERROR:
    ਵਾਪਸ "Failed sending data to the peer";

  ਹਾਲਤ CURLE_RECV_ERROR:
    ਵਾਪਸ "Failure when receiving data from the peer";

  ਹਾਲਤ CURLE_SSL_CERTPROBLEM:
    ਵਾਪਸ "Problem with the local SSL certificate";

  ਹਾਲਤ CURLE_SSL_CIPHER:
    ਵਾਪਸ "Could not use specified SSL cipher";

  ਹਾਲਤ CURLE_PEER_FAILED_VERIFICATION:
    ਵਾਪਸ "SSL peer certificate or SSH remote key was not OK";

  ਹਾਲਤ CURLE_SSL_CACERT_BADFILE:
    ਵਾਪਸ "Problem with the SSL CA cert (path? access rights?)";

  ਹਾਲਤ CURLE_BAD_CONTENT_ENCODING:
    ਵਾਪਸ "Unrecognized or bad HTTP Content or Transfer-Encoding";

  ਹਾਲਤ CURLE_FILESIZE_EXCEEDED:
    ਵਾਪਸ "Maximum file size exceeded";

  ਹਾਲਤ CURLE_USE_SSL_FAILED:
    ਵਾਪਸ "Requested SSL level failed";

  ਹਾਲਤ CURLE_SSL_SHUTDOWN_FAILED:
    ਵਾਪਸ "Failed to shut down the SSL connection";

  ਹਾਲਤ CURLE_SSL_CRL_BADFILE:
    ਵਾਪਸ "Failed to load CRL file (path? access rights?, format?)";

  ਹਾਲਤ CURLE_SSL_ISSUER_ERROR:
    ਵਾਪਸ "Issuer check against peer certificate failed";

  ਹਾਲਤ CURLE_SEND_FAIL_REWIND:
    ਵਾਪਸ "Send failed since rewinding of the data stream failed";

  ਹਾਲਤ CURLE_LOGIN_DENIED:
    ਵਾਪਸ "Login denied";

  ਹਾਲਤ CURLE_TFTP_NOTFOUND:
    ਵਾਪਸ "TFTP: File Not Found";

  ਹਾਲਤ CURLE_TFTP_PERM:
    ਵਾਪਸ "TFTP: Access Violation";

  ਹਾਲਤ CURLE_REMOTE_DISK_ਭਰਿਆ:
    ਵਾਪਸ "Disk full or allocation exceeded";

  ਹਾਲਤ CURLE_TFTP_ILLEGAL:
    ਵਾਪਸ "TFTP: Illegal operation";

  ਹਾਲਤ CURLE_TFTP_UNKNOWNID:
    ਵਾਪਸ "TFTP: Unknown transfer ID";

  ਹਾਲਤ CURLE_REMOTE_FILE_EXISTS:
    ਵਾਪਸ "Remote file already exists";

  ਹਾਲਤ CURLE_TFTP_NOSUCHUSER:
    ਵਾਪਸ "TFTP: No such user";

  ਹਾਲਤ CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ:
    ਵਾਪਸ "Remote file not found";

  ਹਾਲਤ CURLE_SSH:
    ਵਾਪਸ "Error in the SSH layer";

  ਹਾਲਤ CURLE_AGAIN:
    ਵਾਪਸ "Socket not ready for send/recv";

  ਹਾਲਤ CURLE_RTSP_CSEQ_ERROR:
    ਵਾਪਸ "RTSP CSeq mismatch or invalid CSeq";

  ਹਾਲਤ CURLE_RTSP_SESSION_ERROR:
    ਵਾਪਸ "RTSP session error";

  ਹਾਲਤ CURLE_FTP_BAD_FILE_LIST:
    ਵਾਪਸ "Unable to parse FTP file list";

  ਹਾਲਤ CURLE_CHUNK_FAILED:
    ਵਾਪਸ "Chunk callback failed";

  ਹਾਲਤ CURLE_NO_CONNECTION_ਉਪਲਬਧ:
    ਵਾਪਸ "The max connection limit is reached";

  ਹਾਲਤ CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    ਵਾਪਸ "SSL public key does not match pinned public key";

  ਹਾਲਤ CURLE_SSL_INVALIDCERTSTATUS:
    ਵਾਪਸ "SSL server certificate status verification FAILED";

  ਹਾਲਤ CURLE_HTTP2_ਧਾਰਾ:
    ਵਾਪਸ "Stream error in the HTTP/2 framing layer";

  ਹਾਲਤ CURLE_RECURSIVE_API_CALL:
    ਵਾਪਸ "API function called from within callback";

  ਹਾਲਤ CURLE_AUTH_ERROR:
    ਵਾਪਸ "An authentication function returned an error";

  ਹਾਲਤ CURLE_HTTP3:
    ਵਾਪਸ "HTTP/3 error";

  ਹਾਲਤ CURLE_QUIC_CONNECT_ERROR:
    ਵਾਪਸ "QUIC connection error";

  ਹਾਲਤ CURLE_PROXY:
    ਵਾਪਸ "proxy handshake error";

  ਹਾਲਤ CURLE_SSL_CLIENTCERT:
    ਵਾਪਸ "SSL Client Certificate required";

  ਹਾਲਤ CURLE_UNRECOVERABLE_POLL:
    ਵਾਪਸ "Unrecoverable error in select/poll";

  ਹਾਲਤ CURLE_TOO_LARGE:
    ਵਾਪਸ "A value or data field grew larger than allowed";

  ਹਾਲਤ CURLE_ECH_REQUIRED:
    ਵਾਪਸ "ECH attempted but failed";

    /* error codes not used by current libcurl */
  ਮੂਲ:
    ਰੋਕੋ;
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
  ਵਾਪਸ "Unknown error";
#ਨਹੀਂਤਾਂ
  ਜੇ(!error)
    ਵਾਪਸ "No error";
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ "Error";
#ਜੇ_ਅੰਤ
}

ਅਚਲ ਅੱਖਰ *curl_multi_strerror(CURLMcode error)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਚੋਣ(error) {
  ਹਾਲਤ CURLM_CALL_MULTI_PERFORM:
    ਵਾਪਸ "Please call curl_multi_perform() soon";

  ਹਾਲਤ CURLM_OK:
    ਵਾਪਸ "No error";

  ਹਾਲਤ CURLM_BAD_HANDLE:
    ਵਾਪਸ "Invalid multi handle";

  ਹਾਲਤ CURLM_BAD_EASY_HANDLE:
    ਵਾਪਸ "Invalid easy handle";

  ਹਾਲਤ CURLM_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ:
    ਵਾਪਸ "Out of memory";

  ਹਾਲਤ CURLM_ਅੰਦਰੂਨੀ_ERROR:
    ਵਾਪਸ "Internal error";

  ਹਾਲਤ CURLM_BAD_SOCKET:
    ਵਾਪਸ "Invalid socket argument";

  ਹਾਲਤ CURLM_UNKNOWN_OPTION:
    ਵਾਪਸ "Unknown option";

  ਹਾਲਤ CURLM_ADDED_ALREADY:
    ਵਾਪਸ "The easy handle is already added to a multi handle";

  ਹਾਲਤ CURLM_RECURSIVE_API_CALL:
    ਵਾਪਸ "API function called from within callback";

  ਹਾਲਤ CURLM_WAKEUP_FAILURE:
    ਵਾਪਸ "Wakeup is unavailable or failed";

  ਹਾਲਤ CURLM_BAD_FUNCTION_ਦਲੀਲ:
    ਵਾਪਸ "A libcurl function was given a bad argument";

  ਹਾਲਤ CURLM_ABORTED_BY_CALLBACK:
    ਵਾਪਸ "Operation was aborted by an application callback";

  ਹਾਲਤ CURLM_UNRECOVERABLE_POLL:
    ਵਾਪਸ "Unrecoverable error in select/poll";

  ਹਾਲਤ CURLM_ਆਖਰੀ:
    ਰੋਕੋ;
  }

  ਵਾਪਸ "Unknown error";
#ਨਹੀਂਤਾਂ
  ਜੇ(error == CURLM_OK)
    ਵਾਪਸ "No error";
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ "Error";
#ਜੇ_ਅੰਤ
}

ਅਚਲ ਅੱਖਰ *curl_share_strerror(CURLSHcode error)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਚੋਣ(error) {
  ਹਾਲਤ CURLSHE_OK:
    ਵਾਪਸ "No error";

  ਹਾਲਤ CURLSHE_BAD_OPTION:
    ਵਾਪਸ "Unknown share option";

  ਹਾਲਤ CURLSHE_ਵਿੱਚ_USE:
    ਵਾਪਸ "Share currently in use";

  ਹਾਲਤ CURLSHE_INVALID:
    ਵਾਪਸ "Invalid share handle";

  ਹਾਲਤ CURLSHE_NOMEM:
    ਵਾਪਸ "Out of memory";

  ਹਾਲਤ CURLSHE_NOT_BUILT_ਵਿੱਚ:
    ਵਾਪਸ "Feature not enabled in this library";

  ਹਾਲਤ CURLSHE_ਆਖਰੀ:
    ਰੋਕੋ;
  }

  ਵਾਪਸ "CURLSHcode unknown";
#ਨਹੀਂਤਾਂ
  ਜੇ(error == CURLSHE_OK)
    ਵਾਪਸ "No error";
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ "Error";
#ਜੇ_ਅੰਤ
}

ਅਚਲ ਅੱਖਰ *curl_url_strerror(CURLUcode error)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਚੋਣ(error) {
  ਹਾਲਤ CURLUE_OK:
    ਵਾਪਸ "No error";

  ਹਾਲਤ CURLUE_BAD_HANDLE:
    ਵਾਪਸ "An invalid CURLU pointer was passed as argument";

  ਹਾਲਤ CURLUE_BAD_PARTPOINTER:
    ਵਾਪਸ "An invalid 'part' argument was passed as argument";

  ਹਾਲਤ CURLUE_MALFORMED_ਨਿਵੇਸ਼:
    ਵਾਪਸ "Malformed input to a URL function";

  ਹਾਲਤ CURLUE_BAD_ਪੋਰਟ_ਸੰਖਿਆ:
    ਵਾਪਸ "Port number was not a decimal number between 0 and 65535";

  ਹਾਲਤ CURLUE_UNSUPPORTED_SCHEME:
    ਵਾਪਸ "Unsupported URL scheme";

  ਹਾਲਤ CURLUE_URLDECODE:
    ਵਾਪਸ "URL decode error, most likely because of rubbish in the input";

  ਹਾਲਤ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ:
    ਵਾਪਸ "A memory function failed";

  ਹਾਲਤ CURLUE_USER_NOT_ALLOWED:
    ਵਾਪਸ "Credentials was passed in the URL when prohibited";

  ਹਾਲਤ CURLUE_UNKNOWN_PART:
    ਵਾਪਸ "An unknown part ID was passed to a URL API function";

  ਹਾਲਤ CURLUE_NO_SCHEME:
    ਵਾਪਸ "No scheme present";

  ਹਾਲਤ CURLUE_NO_USER:
    ਵਾਪਸ "No user present";

  ਹਾਲਤ CURLUE_NO_PASSWORD:
    ਵਾਪਸ "No password present";

  ਹਾਲਤ CURLUE_NO_OPTIONS:
    ਵਾਪਸ "No options present";

  ਹਾਲਤ CURLUE_NO_HOST:
    ਵਾਪਸ "No host present";

  ਹਾਲਤ CURLUE_NO_ਪੋਰਟ:
    ਵਾਪਸ "No port number present";

  ਹਾਲਤ CURLUE_NO_QUERY:
    ਵਾਪਸ "No query present";

  ਹਾਲਤ CURLUE_NO_FRAGMENT:
    ਵਾਪਸ "No fragment present";

  ਹਾਲਤ CURLUE_NO_ZONEID:
    ਵਾਪਸ "No zoneid present";

  ਹਾਲਤ CURLUE_BAD_LOGIN:
    ਵਾਪਸ "Bad login";

  ਹਾਲਤ CURLUE_BAD_IPV6:
    ਵਾਪਸ "Bad IPv6 address";

  ਹਾਲਤ CURLUE_BAD_HOSTNAME:
    ਵਾਪਸ "Bad hostname";

  ਹਾਲਤ CURLUE_BAD_FILE_URL:
    ਵਾਪਸ "Bad file:// URL";

  ਹਾਲਤ CURLUE_BAD_SLASHES:
    ਵਾਪਸ "Unsupported number of slashes following scheme";

  ਹਾਲਤ CURLUE_BAD_SCHEME:
    ਵਾਪਸ "Bad scheme";

  ਹਾਲਤ CURLUE_BAD_ਰਸਤਾ:
    ਵਾਪਸ "Bad path";

  ਹਾਲਤ CURLUE_BAD_FRAGMENT:
    ਵਾਪਸ "Bad fragment";

  ਹਾਲਤ CURLUE_BAD_QUERY:
    ਵਾਪਸ "Bad query";

  ਹਾਲਤ CURLUE_BAD_PASSWORD:
    ਵਾਪਸ "Bad password";

  ਹਾਲਤ CURLUE_BAD_USER:
    ਵਾਪਸ "Bad user";

  ਹਾਲਤ CURLUE_LACKS_IDN:
    ਵਾਪਸ "libcurl lacks IDN support";

  ਹਾਲਤ CURLUE_TOO_LARGE:
    ਵਾਪਸ "A value or data field is larger than allowed";

  ਹਾਲਤ CURLUE_BACKSLASH:
    ਵਾਪਸ "Found a backslash where a forward slash was expected";

  ਹਾਲਤ CURLUE_ਆਖਰੀ:
    ਰੋਕੋ;
  }

  ਵਾਪਸ "CURLUcode unknown";
#ਨਹੀਂਤਾਂ
  ਜੇ(error == CURLUE_OK)
    ਵਾਪਸ "No error";
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ "Error";
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
ਅਚਲ ਅੱਖਰ *Curl_sspi_strerror(SECURITY_ਸਥਿਤੀ err, ਅੱਖਰ *buf, size_t buflen)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  DWORD old_win_err = Get_ਆਖਰੀ_Error();
#ਜੇ_ਅੰਤ
  ਪੂਰਨ old_errno = errno;
  VERBOSE(ਅਚਲ ਅੱਖਰ *txt);

  ਜੇ(!buflen)
    ਵਾਪਸ ਨਿਰਾ;

  *buf = '\0';

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਚੋਣ(err) {
  ਹਾਲਤ SEC_E_OK:
    txt = "No error";
    ਰੋਕੋ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEC2TXT(sec) ਹਾਲਤ sec: txt = #sec; ਰੋਕੋ
  SEC2TXT(CRYPT_E_REVOKED);
  SEC2TXT(CRYPT_E_NO_REVOCATION_DLL);
  SEC2TXT(CRYPT_E_NO_REVOCATION_CHECK);
  SEC2TXT(CRYPT_E_REVOCATION_OFFLINE);
  SEC2TXT(CRYPT_E_NOT_ਵਿੱਚ_REVOCATION_DATABASE);
  SEC2TXT(SEC_E_ALGORITHM_MISMATCH);
  SEC2TXT(SEC_E_BAD_BINDINGS);
  SEC2TXT(SEC_E_BAD_PKGID);
  SEC2TXT(SEC_E_ਬਫਰ_TOO_SMALL);
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
  SEC2TXT(SEC_E_ILLEGAL_ਸੁਨੇਹਾ);
  SEC2TXT(SEC_E_INCOMPLETE_CREDENTIALS);
  SEC2TXT(SEC_E_INCOMPLETE_ਸੁਨੇਹਾ);
  SEC2TXT(SEC_E_INSUFFICIENT_ਸਮ੍ਰਿਤੀ);
  SEC2TXT(SEC_E_ਅੰਦਰੂਨੀ_ERROR);
  SEC2TXT(SEC_E_INVALID_HANDLE);
  SEC2TXT(SEC_E_INVALID_PARAMETER);
  SEC2TXT(SEC_E_INVALID_TOKEN);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED_KDC);
  SEC2TXT(SEC_E_KDC_CERT_EXPIRED);
  SEC2TXT(SEC_E_KDC_CERT_REVOKED);
  SEC2TXT(SEC_E_KDC_INVALID_REQUEST);
  SEC2TXT(SEC_E_KDC_UNABLE_ਮੰਜ਼ਿਲ_REFER);
  SEC2TXT(SEC_E_KDC_UNKNOWN_ETYPE);
  SEC2TXT(SEC_E_LOGON_DENIED);
  SEC2TXT(SEC_E_MAX_REFERRALS_EXCEEDED);
  SEC2TXT(SEC_E_ਸੁਨੇਹਾ_ALTERED);
  SEC2TXT(SEC_E_ਗੁਣਜ_ACCOUNTS);
  SEC2TXT(SEC_E_MUST_BE_KDC);
  SEC2TXT(SEC_E_NOT_OWNER);
  SEC2TXT(SEC_E_NO_AUTHENTICATING_AUTHORITY);
  SEC2TXT(SEC_E_NO_CREDENTIALS);
  SEC2TXT(SEC_E_NO_IMPERSONATION);
  SEC2TXT(SEC_E_NO_IP_ਪਤੇ);
  SEC2TXT(SEC_E_NO_KERB_ਕੁੰਜੀ);
  SEC2TXT(SEC_E_NO_PA_ਅੰਕੜੇ);
  SEC2TXT(SEC_E_NO_S4U_PROT_SUPPORT);
  SEC2TXT(SEC_E_NO_TGT_ਜਵਾਬ);
  SEC2TXT(SEC_E_ਬਾਹਰਲਾ_ਦਾ_SEQUENCE);
  SEC2TXT(SEC_E_PKINIT_CLIENT_FAILURE);
  SEC2TXT(SEC_E_PKINIT_NAME_MISMATCH);
  SEC2TXT(SEC_E_POLICY_NLTM_ONLY);
  SEC2TXT(SEC_E_QOP_NOT_SUPPORTED);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_C);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_KDC);
  SEC2TXT(SEC_E_SECPKG_NOT_ਮਿਲਿਆ);
  SEC2TXT(SEC_E_SECURITY_QOS_FAILED);
  SEC2TXT(SEC_E_SHUTDOWN_ਵਿੱਚ_ਤਰੱਕੀ);
  SEC2TXT(SEC_E_SMARTCARD_CERT_EXPIRED);
  SEC2TXT(SEC_E_SMARTCARD_CERT_REVOKED);
  SEC2TXT(SEC_E_SMARTCARD_LOGON_REQUIRED);
  SEC2TXT(SEC_E_STRONG_CRYPTO_NOT_SUPPORTED);
  SEC2TXT(SEC_E_ਨਿਸ਼ਾਨਾ_UNKNOWN);
  SEC2TXT(SEC_E_ਸਮਾਂ_SKEW);
  SEC2TXT(SEC_E_TOO_MANY_PRINCIPALS);
  SEC2TXT(SEC_E_UNFINISHED_CONTEXT_DELETED);
  SEC2TXT(SEC_E_UNKNOWN_CREDENTIALS);
  SEC2TXT(SEC_E_UNSUPPORTED_FUNCTION);
  SEC2TXT(SEC_E_UNSUPPORTED_PREAUTH);
  SEC2TXT(SEC_E_UNTRUSTED_ਜੜ੍ਹ);
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
  ਮੂਲ:
    txt = "Unknown error";
  }

  ਜੇ(err == SEC_E_ILLEGAL_ਸੁਨੇਹਾ) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)err);
  }
  ਨਹੀਂਤਾਂ {
    ਅੱਖਰ msgbuf[256];
    ਜੇ(curlx_get_winapi_error((DWORD)err, msgbuf, ਆਕਾਰ(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)err,
                     msgbuf);
    ਨਹੀਂਤਾਂ
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)err);
  }
#ਨਹੀਂਤਾਂ /* CURLVERBOSE */
  ਜੇ(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  ਨਹੀਂਤਾਂ
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#ਜੇ_ਅੰਤ

  ਜੇ(errno != old_errno)
    errno = old_errno;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  ਜੇ(old_win_err != Get_ਆਖਰੀ_Error())
    ਸੈੱਟ_ਆਖਰੀ_Error(old_win_err);
#ਜੇ_ਅੰਤ

  ਵਾਪਸ buf;
}
#ਜੇ_ਅੰਤ /* USE_WINDOWS_SSPI */
