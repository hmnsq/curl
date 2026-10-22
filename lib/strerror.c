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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_WINDOWS_SSPI
#అన్తర్భావ "curl_sspi.h"
#అయితే_ముగింపు

#అన్తర్భావ "curlx/winapi.h"
#అన్తర్భావ "strerror.h"
#అన్తర్భావ "curlx/strcopy.h"

స్థిరరాశి అక్షరం *curl_easy_strerror(CURLcode error)
{
#నిర్వచితమైతే CURLVERBOSE
  ఎంపిక(error) {
  స్థితి CURLE_OK:
    తిరిగి "No error";

  స్థితి CURLE_UNSUPPORTED_PROTOCOL:
    తిరిగి "Unsupported protocol";

  స్థితి CURLE_FAILED_INIT:
    తిరిగి "Failed initialization";

  స్థితి CURLE_URL_MALFORMAT:
    తిరిగి "URL using bad/illegal format or missing URL";

  స్థితి CURLE_NOT_BUILT_లోపల:
    తిరిగి "A requested feature, protocol or option was not found built-in in"
           " this libcurl due to a build-time decision.";

  స్థితి CURLE_COULDNT_RESOLVE_PROXY:
    తిరిగి "Could not resolve proxy name";

  స్థితి CURLE_COULDNT_RESOLVE_HOST:
    తిరిగి "Could not resolve hostname";

  స్థితి CURLE_COULDNT_CONNECT:
    తిరిగి "Could not connect to server";

  స్థితి CURLE_WEIRD_SERVER_జవాబు:
    తిరిగి "Weird server reply";

  స్థితి CURLE_REMOTE_ACCESS_DENIED:
    తిరిగి "Access denied to remote resource";

  స్థితి CURLE_FTP_ACCEPT_FAILED:
    తిరిగి "FTP: The server failed to connect to data port";

  స్థితి CURLE_FTP_ACCEPT_TIMEOUT:
    తిరిగి "FTP: Accepting server connect has timed out";

  స్థితి CURLE_FTP_PRET_FAILED:
    తిరిగి "FTP: The server did not accept the PRET command.";

  స్థితి CURLE_FTP_WEIRD_PASS_జవాబు:
    తిరిగి "FTP: unknown PASS reply";

  స్థితి CURLE_FTP_WEIRD_PASV_జవాబు:
    తిరిగి "FTP: unknown PASV reply";

  స్థితి CURLE_FTP_WEIRD_227_FORMAT:
    తిరిగి "FTP: unknown 227 response format";

  స్థితి CURLE_FTP_CANT_GET_HOST:
    తిరిగి "FTP: cannot figure out the host in the PASV response";

  స్థితి CURLE_HTTP2:
    తిరిగి "Error in the HTTP2 framing layer";

  స్థితి CURLE_FTP_COULDNT_సమితి_TYPE:
    తిరిగి "FTP: could not set file type";

  స్థితి CURLE_PARTIAL_FILE:
    తిరిగి "Transferred a partial file";

  స్థితి CURLE_FTP_COULDNT_RETR_FILE:
    తిరిగి "FTP: could not retrieve (RETR failed) the specified file";

  స్థితి CURLE_QUOTE_ERROR:
    తిరిగి "Quote command returned error";

  స్థితి CURLE_HTTP_RETURNED_ERROR:
    తిరిగి "HTTP response code said error";

  స్థితి CURLE_WRITE_ERROR:
    తిరిగి "Failed writing received data to disk/application";

  స్థితి CURLE_UPLOAD_FAILED:
    తిరిగి "Upload failed (at start/before it took off)";

  స్థితి CURLE_READ_ERROR:
    తిరిగి "Failed to open/read local data from file/application";

  స్థితి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం:
    తిరిగి "Out of memory";

  స్థితి CURLE_OPERATION_TIMEDOUT:
    తిరిగి "Timeout was reached";

  స్థితి CURLE_FTP_పోర్టు_FAILED:
    తిరిగి "FTP: command PORT failed";

  స్థితి CURLE_FTP_COULDNT_USE_REST:
    తిరిగి "FTP: command REST failed";

  స్థితి CURLE_RANGE_ERROR:
    తిరిగి "Requested range was not delivered by the server";

  స్థితి CURLE_SSL_CONNECT_ERROR:
    తిరిగి "SSL connect error";

  స్థితి CURLE_BAD_DOWNLOAD_RESUME:
    తిరిగి "Could not resume download";

  స్థితి CURLE_FILE_COULDNT_READ_FILE:
    తిరిగి "Could not read a file:// file";

  స్థితి CURLE_LDAP_CANNOT_BIND:
    తిరిగి "LDAP: cannot bind";

  స్థితి CURLE_LDAP_శోధన_FAILED:
    తిరిగి "LDAP: search failed";

  స్థితి CURLE_ABORTED_BY_CALLBACK:
    తిరిగి "Operation was aborted by an application callback";

  స్థితి CURLE_BAD_FUNCTION_వాదం:
    తిరిగి "A libcurl function was given a bad argument";

  స్థితి CURLE_INTERFACE_FAILED:
    తిరిగి "Failed binding local connection end";

  స్థితి CURLE_TOO_MANY_REDIRECTS:
    తిరిగి "Number of redirects hit maximum amount";

  స్థితి CURLE_UNKNOWN_OPTION:
    తిరిగి "An unknown option was passed in to libcurl";

  స్థితి CURLE_SETOPT_OPTION_SYNTAX:
    తిరిగి "Malformed option provided in a setopt";

  స్థితి CURLE_GOT_NOTHING:
    తిరిగి "Server returned nothing (no headers, no data)";

  స్థితి CURLE_SSL_ENGINE_NOTFOUND:
    తిరిగి "SSL crypto engine not found";

  స్థితి CURLE_SSL_ENGINE_SETFAILED:
    తిరిగి "Can not set SSL crypto engine as default";

  స్థితి CURLE_SSL_ENGINE_INITFAILED:
    తిరిగి "Failed to initialize SSL crypto engine";

  స్థితి CURLE_SEND_ERROR:
    తిరిగి "Failed sending data to the peer";

  స్థితి CURLE_RECV_ERROR:
    తిరిగి "Failure when receiving data from the peer";

  స్థితి CURLE_SSL_CERTPROBLEM:
    తిరిగి "Problem with the local SSL certificate";

  స్థితి CURLE_SSL_CIPHER:
    తిరిగి "Could not use specified SSL cipher";

  స్థితి CURLE_PEER_FAILED_VERIFICATION:
    తిరిగి "SSL peer certificate or SSH remote key was not OK";

  స్థితి CURLE_SSL_CACERT_BADFILE:
    తిరిగి "Problem with the SSL CA cert (path? access rights?)";

  స్థితి CURLE_BAD_CONTENT_ENCODING:
    తిరిగి "Unrecognized or bad HTTP Content or Transfer-Encoding";

  స్థితి CURLE_FILESIZE_EXCEEDED:
    తిరిగి "Maximum file size exceeded";

  స్థితి CURLE_USE_SSL_FAILED:
    తిరిగి "Requested SSL level failed";

  స్థితి CURLE_SSL_SHUTDOWN_FAILED:
    తిరిగి "Failed to shut down the SSL connection";

  స్థితి CURLE_SSL_CRL_BADFILE:
    తిరిగి "Failed to load CRL file (path? access rights?, format?)";

  స్థితి CURLE_SSL_ISSUER_ERROR:
    తిరిగి "Issuer check against peer certificate failed";

  స్థితి CURLE_SEND_FAIL_REWIND:
    తిరిగి "Send failed since rewinding of the data stream failed";

  స్థితి CURLE_LOGIN_DENIED:
    తిరిగి "Login denied";

  స్థితి CURLE_TFTP_NOTFOUND:
    తిరిగి "TFTP: File Not Found";

  స్థితి CURLE_TFTP_PERM:
    తిరిగి "TFTP: Access Violation";

  స్థితి CURLE_REMOTE_DISK_నిండిన:
    తిరిగి "Disk full or allocation exceeded";

  స్థితి CURLE_TFTP_ILLEGAL:
    తిరిగి "TFTP: Illegal operation";

  స్థితి CURLE_TFTP_UNKNOWNID:
    తిరిగి "TFTP: Unknown transfer ID";

  స్థితి CURLE_REMOTE_FILE_EXISTS:
    తిరిగి "Remote file already exists";

  స్థితి CURLE_TFTP_NOSUCHUSER:
    తిరిగి "TFTP: No such user";

  స్థితి CURLE_REMOTE_FILE_NOT_దొరికింది:
    తిరిగి "Remote file not found";

  స్థితి CURLE_SSH:
    తిరిగి "Error in the SSH layer";

  స్థితి CURLE_AGAIN:
    తిరిగి "Socket not ready for send/recv";

  స్థితి CURLE_RTSP_CSEQ_ERROR:
    తిరిగి "RTSP CSeq mismatch or invalid CSeq";

  స్థితి CURLE_RTSP_SESSION_ERROR:
    తిరిగి "RTSP session error";

  స్థితి CURLE_FTP_BAD_FILE_LIST:
    తిరిగి "Unable to parse FTP file list";

  స్థితి CURLE_CHUNK_FAILED:
    తిరిగి "Chunk callback failed";

  స్థితి CURLE_NO_CONNECTION_అందుబాటు:
    తిరిగి "The max connection limit is reached";

  స్థితి CURLE_SSL_PINNEDPUBKEYNOTMATCH:
    తిరిగి "SSL public key does not match pinned public key";

  స్థితి CURLE_SSL_INVALIDCERTSTATUS:
    తిరిగి "SSL server certificate status verification FAILED";

  స్థితి CURLE_HTTP2_ప్రవాహం:
    తిరిగి "Stream error in the HTTP/2 framing layer";

  స్థితి CURLE_RECURSIVE_API_CALL:
    తిరిగి "API function called from within callback";

  స్థితి CURLE_AUTH_ERROR:
    తిరిగి "An authentication function returned an error";

  స్థితి CURLE_HTTP3:
    తిరిగి "HTTP/3 error";

  స్థితి CURLE_QUIC_CONNECT_ERROR:
    తిరిగి "QUIC connection error";

  స్థితి CURLE_PROXY:
    తిరిగి "proxy handshake error";

  స్థితి CURLE_SSL_CLIENTCERT:
    తిరిగి "SSL Client Certificate required";

  స్థితి CURLE_UNRECOVERABLE_POLL:
    తిరిగి "Unrecoverable error in select/poll";

  స్థితి CURLE_TOO_LARGE:
    తిరిగి "A value or data field grew larger than allowed";

  స్థితి CURLE_ECH_REQUIRED:
    తిరిగి "ECH attempted but failed";

    /* error codes not used by current libcurl */
  అప్రమేయ:
    ఆపు;
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
  తిరిగి "Unknown error";
#లేకపోతే
  ఒకవేళ(!error)
    తిరిగి "No error";
  లేకపోతే
    తిరిగి "Error";
#అయితే_ముగింపు
}

స్థిరరాశి అక్షరం *curl_multi_strerror(CURLMcode error)
{
#నిర్వచితమైతే CURLVERBOSE
  ఎంపిక(error) {
  స్థితి CURLM_CALL_MULTI_PERFORM:
    తిరిగి "Please call curl_multi_perform() soon";

  స్థితి CURLM_OK:
    తిరిగి "No error";

  స్థితి CURLM_BAD_HANDLE:
    తిరిగి "Invalid multi handle";

  స్థితి CURLM_BAD_EASY_HANDLE:
    తిరిగి "Invalid easy handle";

  స్థితి CURLM_బహిర్గతం_యొక్క_జ్ఞాపకం:
    తిరిగి "Out of memory";

  స్థితి CURLM_అంతర్గత_ERROR:
    తిరిగి "Internal error";

  స్థితి CURLM_BAD_SOCKET:
    తిరిగి "Invalid socket argument";

  స్థితి CURLM_UNKNOWN_OPTION:
    తిరిగి "Unknown option";

  స్థితి CURLM_ADDED_ALREADY:
    తిరిగి "The easy handle is already added to a multi handle";

  స్థితి CURLM_RECURSIVE_API_CALL:
    తిరిగి "API function called from within callback";

  స్థితి CURLM_WAKEUP_FAILURE:
    తిరిగి "Wakeup is unavailable or failed";

  స్థితి CURLM_BAD_FUNCTION_వాదం:
    తిరిగి "A libcurl function was given a bad argument";

  స్థితి CURLM_ABORTED_BY_CALLBACK:
    తిరిగి "Operation was aborted by an application callback";

  స్థితి CURLM_UNRECOVERABLE_POLL:
    తిరిగి "Unrecoverable error in select/poll";

  స్థితి CURLM_చివరి:
    ఆపు;
  }

  తిరిగి "Unknown error";
#లేకపోతే
  ఒకవేళ(error == CURLM_OK)
    తిరిగి "No error";
  లేకపోతే
    తిరిగి "Error";
#అయితే_ముగింపు
}

స్థిరరాశి అక్షరం *curl_share_strerror(CURLSHcode error)
{
#నిర్వచితమైతే CURLVERBOSE
  ఎంపిక(error) {
  స్థితి CURLSHE_OK:
    తిరిగి "No error";

  స్థితి CURLSHE_BAD_OPTION:
    తిరిగి "Unknown share option";

  స్థితి CURLSHE_లోపల_USE:
    తిరిగి "Share currently in use";

  స్థితి CURLSHE_INVALID:
    తిరిగి "Invalid share handle";

  స్థితి CURLSHE_NOMEM:
    తిరిగి "Out of memory";

  స్థితి CURLSHE_NOT_BUILT_లోపల:
    తిరిగి "Feature not enabled in this library";

  స్థితి CURLSHE_చివరి:
    ఆపు;
  }

  తిరిగి "CURLSHcode unknown";
#లేకపోతే
  ఒకవేళ(error == CURLSHE_OK)
    తిరిగి "No error";
  లేకపోతే
    తిరిగి "Error";
#అయితే_ముగింపు
}

స్థిరరాశి అక్షరం *curl_url_strerror(CURLUcode error)
{
#నిర్వచితమైతే CURLVERBOSE
  ఎంపిక(error) {
  స్థితి CURLUE_OK:
    తిరిగి "No error";

  స్థితి CURLUE_BAD_HANDLE:
    తిరిగి "An invalid CURLU pointer was passed as argument";

  స్థితి CURLUE_BAD_PARTPOINTER:
    తిరిగి "An invalid 'part' argument was passed as argument";

  స్థితి CURLUE_MALFORMED_ఆదానం:
    తిరిగి "Malformed input to a URL function";

  స్థితి CURLUE_BAD_పోర్టు_సంఖ్య:
    తిరిగి "Port number was not a decimal number between 0 and 65535";

  స్థితి CURLUE_UNSUPPORTED_SCHEME:
    తిరిగి "Unsupported URL scheme";

  స్థితి CURLUE_URLDECODE:
    తిరిగి "URL decode error, most likely because of rubbish in the input";

  స్థితి CURLUE_బహిర్గతం_యొక్క_జ్ఞాపకం:
    తిరిగి "A memory function failed";

  స్థితి CURLUE_USER_NOT_ALLOWED:
    తిరిగి "Credentials was passed in the URL when prohibited";

  స్థితి CURLUE_UNKNOWN_PART:
    తిరిగి "An unknown part ID was passed to a URL API function";

  స్థితి CURLUE_NO_SCHEME:
    తిరిగి "No scheme present";

  స్థితి CURLUE_NO_USER:
    తిరిగి "No user present";

  స్థితి CURLUE_NO_PASSWORD:
    తిరిగి "No password present";

  స్థితి CURLUE_NO_OPTIONS:
    తిరిగి "No options present";

  స్థితి CURLUE_NO_HOST:
    తిరిగి "No host present";

  స్థితి CURLUE_NO_పోర్టు:
    తిరిగి "No port number present";

  స్థితి CURLUE_NO_QUERY:
    తిరిగి "No query present";

  స్థితి CURLUE_NO_FRAGMENT:
    తిరిగి "No fragment present";

  స్థితి CURLUE_NO_ZONEID:
    తిరిగి "No zoneid present";

  స్థితి CURLUE_BAD_LOGIN:
    తిరిగి "Bad login";

  స్థితి CURLUE_BAD_IPV6:
    తిరిగి "Bad IPv6 address";

  స్థితి CURLUE_BAD_HOSTNAME:
    తిరిగి "Bad hostname";

  స్థితి CURLUE_BAD_FILE_URL:
    తిరిగి "Bad file:// URL";

  స్థితి CURLUE_BAD_SLASHES:
    తిరిగి "Unsupported number of slashes following scheme";

  స్థితి CURLUE_BAD_SCHEME:
    తిరిగి "Bad scheme";

  స్థితి CURLUE_BAD_మార్గం:
    తిరిగి "Bad path";

  స్థితి CURLUE_BAD_FRAGMENT:
    తిరిగి "Bad fragment";

  స్థితి CURLUE_BAD_QUERY:
    తిరిగి "Bad query";

  స్థితి CURLUE_BAD_PASSWORD:
    తిరిగి "Bad password";

  స్థితి CURLUE_BAD_USER:
    తిరిగి "Bad user";

  స్థితి CURLUE_LACKS_IDN:
    తిరిగి "libcurl lacks IDN support";

  స్థితి CURLUE_TOO_LARGE:
    తిరిగి "A value or data field is larger than allowed";

  స్థితి CURLUE_BACKSLASH:
    తిరిగి "Found a backslash where a forward slash was expected";

  స్థితి CURLUE_చివరి:
    ఆపు;
  }

  తిరిగి "CURLUcode unknown";
#లేకపోతే
  ఒకవేళ(error == CURLUE_OK)
    తిరిగి "No error";
  లేకపోతే
    తిరిగి "Error";
#అయితే_ముగింపు
}

#నిర్వచితమైతే USE_WINDOWS_SSPI
/*
 * Curl_sspi_strerror:
 * Variant of curlx_strerror if the error code is definitely Windows SSPI.
 */
స్థిరరాశి అక్షరం *Curl_sspi_strerror(SECURITY_స్థితి err, అక్షరం *buf, size_t buflen)
{
#నిర్వచితమైతే _WIN32
  DWORD old_win_err = Get_చివరి_Error();
#అయితే_ముగింపు
  పూర్ణాంకం old_errno = errno;
  VERBOSE(స్థిరరాశి అక్షరం *txt);

  ఒకవేళ(!buflen)
    తిరిగి ఖాళీ;

  *buf = '\0';

#నిర్వచితమైతే CURLVERBOSE
  ఎంపిక(err) {
  స్థితి SEC_E_OK:
    txt = "No error";
    ఆపు;
#నిర్వచించు SEC2TXT(sec) స్థితి sec: txt = #sec; ఆపు
  SEC2TXT(CRYPT_E_REVOKED);
  SEC2TXT(CRYPT_E_NO_REVOCATION_DLL);
  SEC2TXT(CRYPT_E_NO_REVOCATION_CHECK);
  SEC2TXT(CRYPT_E_REVOCATION_OFFLINE);
  SEC2TXT(CRYPT_E_NOT_లోపల_REVOCATION_DATABASE);
  SEC2TXT(SEC_E_ALGORITHM_MISMATCH);
  SEC2TXT(SEC_E_BAD_BINDINGS);
  SEC2TXT(SEC_E_BAD_PKGID);
  SEC2TXT(SEC_E_బఫరు_TOO_SMALL);
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
  SEC2TXT(SEC_E_ILLEGAL_సందేశం);
  SEC2TXT(SEC_E_INCOMPLETE_CREDENTIALS);
  SEC2TXT(SEC_E_INCOMPLETE_సందేశం);
  SEC2TXT(SEC_E_INSUFFICIENT_జ్ఞాపకం);
  SEC2TXT(SEC_E_అంతర్గత_ERROR);
  SEC2TXT(SEC_E_INVALID_HANDLE);
  SEC2TXT(SEC_E_INVALID_PARAMETER);
  SEC2TXT(SEC_E_INVALID_TOKEN);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED);
  SEC2TXT(SEC_E_ISSUING_CA_UNTRUSTED_KDC);
  SEC2TXT(SEC_E_KDC_CERT_EXPIRED);
  SEC2TXT(SEC_E_KDC_CERT_REVOKED);
  SEC2TXT(SEC_E_KDC_INVALID_REQUEST);
  SEC2TXT(SEC_E_KDC_UNABLE_గమ్యం_REFER);
  SEC2TXT(SEC_E_KDC_UNKNOWN_ETYPE);
  SEC2TXT(SEC_E_LOGON_DENIED);
  SEC2TXT(SEC_E_MAX_REFERRALS_EXCEEDED);
  SEC2TXT(SEC_E_సందేశం_ALTERED);
  SEC2TXT(SEC_E_గుణిజం_ACCOUNTS);
  SEC2TXT(SEC_E_MUST_BE_KDC);
  SEC2TXT(SEC_E_NOT_OWNER);
  SEC2TXT(SEC_E_NO_AUTHENTICATING_AUTHORITY);
  SEC2TXT(SEC_E_NO_CREDENTIALS);
  SEC2TXT(SEC_E_NO_IMPERSONATION);
  SEC2TXT(SEC_E_NO_IP_చిరునామాలు);
  SEC2TXT(SEC_E_NO_KERB_కీలకం);
  SEC2TXT(SEC_E_NO_PA_దత్తాంశం);
  SEC2TXT(SEC_E_NO_S4U_PROT_SUPPORT);
  SEC2TXT(SEC_E_NO_TGT_జవాబు);
  SEC2TXT(SEC_E_బహిర్గతం_యొక్క_SEQUENCE);
  SEC2TXT(SEC_E_PKINIT_CLIENT_FAILURE);
  SEC2TXT(SEC_E_PKINIT_NAME_MISMATCH);
  SEC2TXT(SEC_E_POLICY_NLTM_ONLY);
  SEC2TXT(SEC_E_QOP_NOT_SUPPORTED);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_C);
  SEC2TXT(SEC_E_REVOCATION_OFFLINE_KDC);
  SEC2TXT(SEC_E_SECPKG_NOT_దొరికింది);
  SEC2TXT(SEC_E_SECURITY_QOS_FAILED);
  SEC2TXT(SEC_E_SHUTDOWN_లోపల_పురోగతి);
  SEC2TXT(SEC_E_SMARTCARD_CERT_EXPIRED);
  SEC2TXT(SEC_E_SMARTCARD_CERT_REVOKED);
  SEC2TXT(SEC_E_SMARTCARD_LOGON_REQUIRED);
  SEC2TXT(SEC_E_STRONG_CRYPTO_NOT_SUPPORTED);
  SEC2TXT(SEC_E_లక్ష్యం_UNKNOWN);
  SEC2TXT(SEC_E_సమయం_SKEW);
  SEC2TXT(SEC_E_TOO_MANY_PRINCIPALS);
  SEC2TXT(SEC_E_UNFINISHED_CONTEXT_DELETED);
  SEC2TXT(SEC_E_UNKNOWN_CREDENTIALS);
  SEC2TXT(SEC_E_UNSUPPORTED_FUNCTION);
  SEC2TXT(SEC_E_UNSUPPORTED_PREAUTH);
  SEC2TXT(SEC_E_UNTRUSTED_మూలం);
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
  అప్రమేయ:
    txt = "Unknown error";
  }

  ఒకవేళ(err == SEC_E_ILLEGAL_సందేశం) {
    curl_msnprintf(buf, buflen,
                   "SEC_E_ILLEGAL_MESSAGE (0x%08lx) - This error usually "
                   "occurs when a fatal SSL/TLS alert is received (e.g. "
                   "handshake failed). More detail may be available in "
                   "the Windows System event log.", (గుర్తులేని దీర్ఘ)err);
  }
  లేకపోతే {
    అక్షరం msgbuf[256];
    ఒకవేళ(curlx_get_winapi_error((DWORD)err, msgbuf, పరిమాణం(msgbuf)))
      curl_msnprintf(buf, buflen, "%s (0x%08lx) - %s", txt, (గుర్తులేని దీర్ఘ)err,
                     msgbuf);
    లేకపోతే
      curl_msnprintf(buf, buflen, "%s (0x%08lx)", txt, (గుర్తులేని దీర్ఘ)err);
  }
#లేకపోతే /* CURLVERBOSE */
  ఒకవేళ(err == SEC_E_OK)
    curlx_strcopy(buf, buflen, STRCONST("No error"));
  లేకపోతే
    curlx_strcopy(buf, buflen, STRCONST("Error"));
#అయితే_ముగింపు

  ఒకవేళ(errno != old_errno)
    errno = old_errno;

#నిర్వచితమైతే _WIN32
  ఒకవేళ(old_win_err != Get_చివరి_Error())
    సమితి_చివరి_Error(old_win_err);
#అయితే_ముగింపు

  తిరిగి buf;
}
#అయితే_ముగింపు /* USE_WINDOWS_SSPI */
