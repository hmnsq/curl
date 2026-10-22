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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "vssh/ssh.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h" /* for the week day and month names */
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "vssh/vssh.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h" /* for curlx_base64_encode() */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sftp_libssh2_strerror(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ err)
{
  ಆಯ್ಕೆ(err) {
  ಸ್ಥಿತಿ LIBSSH2_FX_NO_SUCH_FILE:
    ಹಿಂತಿರುಗಿ "No such file or directory";

  ಸ್ಥಿತಿ LIBSSH2_FX_PERMISSION_DENIED:
    ಹಿಂತಿರುಗಿ "Permission denied";

  ಸ್ಥಿತಿ LIBSSH2_FX_FAILURE:
    ಹಿಂತಿರುಗಿ "Operation failed";

  ಸ್ಥಿತಿ LIBSSH2_FX_BAD_ಸಂದೇಶ:
    ಹಿಂತಿರುಗಿ "Bad message from SFTP server";

  ಸ್ಥಿತಿ LIBSSH2_FX_NO_CONNECTION:
    ಹಿಂತಿರುಗಿ "Not connected to SFTP server";

  ಸ್ಥಿತಿ LIBSSH2_FX_CONNECTION_ಕಳೆದುಹೋದ:
    ಹಿಂತಿರುಗಿ "Connection to SFTP server lost";

  ಸ್ಥಿತಿ LIBSSH2_FX_OP_UNSUPPORTED:
    ಹಿಂತಿರುಗಿ "Operation not supported by SFTP server";

  ಸ್ಥಿತಿ LIBSSH2_FX_INVALID_HANDLE:
    ಹಿಂತಿರುಗಿ "Invalid handle";

  ಸ್ಥಿತಿ LIBSSH2_FX_NO_SUCH_ಮಾರ್ಗ:
    ಹಿಂತಿರುಗಿ "No such file or directory";

  ಸ್ಥಿತಿ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ಹಿಂತಿರುಗಿ "File already exists";

  ಸ್ಥಿತಿ LIBSSH2_FX_WRITE_PROTECT:
    ಹಿಂತಿರುಗಿ "File is write protected";

  ಸ್ಥಿತಿ LIBSSH2_FX_NO_MEDIA:
    ಹಿಂತಿರುಗಿ "No media";

  ಸ್ಥಿತಿ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    ಹಿಂತಿರುಗಿ "Disk full";

  ಸ್ಥಿತಿ LIBSSH2_FX_QUOTA_EXCEEDED:
    ಹಿಂತಿರುಗಿ "User quota exceeded";

  ಸ್ಥಿತಿ LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    ಹಿಂತಿರುಗಿ "Unknown principal";

  ಸ್ಥಿತಿ LIBSSH2_FX_ಬೀಗ_CONFLICT:
    ಹಿಂತಿರುಗಿ "File lock conflict";

  ಸ್ಥಿತಿ LIBSSH2_FX_DIR_NOT_ಖಾಲಿಗಳು:
    ಹಿಂತಿರುಗಿ "Directory not empty";

  ಸ್ಥಿತಿ LIBSSH2_FX_NOT_A_DIRECTORY:
    ಹಿಂತಿರುಗಿ "Not a directory";

  ಸ್ಥಿತಿ LIBSSH2_FX_INVALID_FILENAME:
    ಹಿಂತಿರುಗಿ "Invalid filename";

  ಸ್ಥಿತಿ LIBSSH2_FX_LINK_LOOP:
    ಹಿಂತಿರುಗಿ "Link points to itself";
  }
  ಹಿಂತಿರುಗಿ "Unknown error in libssh2";
}

ಸ್ಥಿರ ಶೂನ್ಯ kbd_callback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ name_len,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *instruction, ಪೂರ್ಣಾಂಕ instruction_len,
                         ಪೂರ್ಣಾಂಕ ಅಂಕೆ_prompts,
                         ಸ್ಥಿರಾಂಕ LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         ಶೂನ್ಯ **abstract)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = (ರಚನೆ Curl_easy *)*abstract;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", ಅಂಕೆ_prompts);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)name;
  (ಶೂನ್ಯ)name_len;
  (ಶೂನ್ಯ)instruction;
  (ಶೂನ್ಯ)instruction_len;
#ಆದರೆ_ಅಂತ್ಯ /* CURL_LIBSSH2_DEBUG */
  ಆದರೆ(ಅಂಕೆ_prompts == 1) {
    ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].ಪಠ್ಯ_3 = Curl_cstrdup(passwd);
    responses[0].ಉದ್ದಳತೆ = responses[0].ಪಠ್ಯ_3 ? curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(passwd)) : 0;
  }
  (ಶೂನ್ಯ)prompts;
} /* kbd_callback */

ಸ್ಥಿರ CURLcode sftp_libssh2_error_ಗಮ್ಯ_CURLE(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ err)
{
  ಆಯ್ಕೆ(err) {
  ಸ್ಥಿತಿ LIBSSH2_FX_OK:
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಸ್ಥಿತಿ LIBSSH2_FX_NO_SUCH_FILE:
  ಸ್ಥಿತಿ LIBSSH2_FX_NO_SUCH_ಮಾರ್ಗ:
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು;

  ಸ್ಥಿತಿ LIBSSH2_FX_PERMISSION_DENIED:
  ಸ್ಥಿತಿ LIBSSH2_FX_WRITE_PROTECT:
  ಸ್ಥಿತಿ LIBSSH2_FX_ಬೀಗ_CONFLICT:
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_ACCESS_DENIED;

  ಸ್ಥಿತಿ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  ಸ್ಥಿತಿ LIBSSH2_FX_QUOTA_EXCEEDED:
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_DISK_ತುಂಬಿದ;

  ಸ್ಥಿತಿ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_FILE_EXISTS;

  ಸ್ಥಿತಿ LIBSSH2_FX_DIR_NOT_ಖಾಲಿಗಳು:
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ CURLE_SSH;
}

ಸ್ಥಿರ CURLcode libssh2_session_error_ಗಮ್ಯ_CURLE(ಪೂರ್ಣಾಂಕ err)
{
  ಆಯ್ಕೆ(err) {
  /* Ordered by order of appearance in libssh2.h */
  ಸ್ಥಿತಿ LIBSSH2_ERROR_NONE:
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  ಸ್ಥಿತಿ LIBSSH2_ERROR_SCP_PROTOCOL:
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು;

  ಸ್ಥಿತಿ LIBSSH2_ERROR_SOCKET_NONE:
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;

  ಸ್ಥಿತಿ LIBSSH2_ERROR_ALLOC:
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಸ್ಥಿತಿ LIBSSH2_ERROR_SOCKET_SEND:
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;

  ಸ್ಥಿತಿ LIBSSH2_ERROR_HOSTKEY_INIT:
  ಸ್ಥಿತಿ LIBSSH2_ERROR_HOSTKEY_SIGN:
  ಸ್ಥಿತಿ LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  ಸ್ಥಿತಿ LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;

  ಸ್ಥಿತಿ LIBSSH2_ERROR_PASSWORD_EXPIRED:
    ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;

  ಸ್ಥಿತಿ LIBSSH2_ERROR_SOCKET_TIMEOUT:
  ಸ್ಥಿತಿ LIBSSH2_ERROR_TIMEOUT:
    ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;

  ಸ್ಥಿತಿ LIBSSH2_ERROR_EAGAIN:
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  }

  ಹಿಂತಿರುಗಿ CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

ಸ್ಥಿರ LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (ಶೂನ್ಯ)abstract;
  ಹಿಂತಿರುಗಿ Curl_cmalloc(ಎಣಿಕೆ_2);
}

ಸ್ಥಿರ LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (ಶೂನ್ಯ)abstract;
  ಹಿಂತಿರುಗಿ Curl_crealloc(ptr, ಎಣಿಕೆ_2);
}

ಸ್ಥಿರ LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (ಶೂನ್ಯ)abstract;
  ಆದರೆ(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sshkeycallback(CURL *easy,
                          ಸ್ಥಿರಾಂಕ ರಚನೆ curl_khkey *knownkey, /*ತಿಳಿದ*/
                          ಸ್ಥಿರಾಂಕ ರಚನೆ curl_khkey *foundkey, /* found */
                          ಎಣಿಕೆ curl_khmatch ಹೊಂದಿಕೆ,
                          ಶೂನ್ಯ *clientp)
{
  (ಶೂನ್ಯ)easy;
  (ಶೂನ್ಯ)knownkey;
  (ಶೂನ್ಯ)foundkey;
  (ಶೂನ್ಯ)clientp;

  /* we only allow perfect matches, and we reject everything else */
  ಹಿಂತಿರುಗಿ (ಹೊಂದಿಕೆ != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ಸ್ಥಿರ ಎಣಿಕೆ curl_khtype convert_ssh2_keytype(ಪೂರ್ಣಾಂಕ sshkeytype)
{
  ಎಣಿಕೆ curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  ಆಯ್ಕೆ(sshkeytype) {
  ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBSSH2_HOSTKEY_TYPE_DSS
  ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBSSH2_HOSTKEY_TYPE_ED25519
  ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಹಿಂತಿರುಗಿ keytype;
}

ಸ್ಥಿರ CURLcode ssh_knownhost(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ssh_conn *sshc)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ libssh2_knownhost *host = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *remotekey = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  ಪೂರ್ಣಾಂಕ keybit = 0;
  ಪೂರ್ಣಾಂಕ sshkeytype = 0;
  size_t keylen = 0;
  ಪೂರ್ಣಾಂಕ rc = 0;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS)) {
    infof(ದತ್ತಾಂಶ, "SSH: no knownhosts file configured");
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  ಆದರೆ(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    ರಚನೆ Curl_mapi_guard guard;
    ಎಣಿಕೆ curl_khmatch keymatch;
    curl_sshkeycallback func =
      ದತ್ತಾಂಶ->ಗಣ.ssh_keyfunc ? ದತ್ತಾಂಶ->ಗಣ.ssh_keyfunc : sshkeycallback;
    ರಚನೆ curl_khkey knownkey;
    ರಚನೆ curl_khkey *knownkeyp = ಖಾಲಿ;
    ರಚನೆ curl_khkey foundkey;

    ಆಯ್ಕೆ(sshkeytype) {
    ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHRSA;
      ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBSSH2_HOSTKEY_TYPE_DSS
    ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHDSS;
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_256;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_384;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_521;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_ED25519;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      infof(ದತ್ತಾಂಶ, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    ಇಲ್ಲದಿದ್ದರೆ {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->ಪೋರ್ಟ್ != ಪೋರ್ಟ್_SSH) ?
                                          conn->origin->ಪೋರ್ಟ್ : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(ದತ್ತಾಂಶ, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->ಕೀಲಿ : "<none>");

      /* setup 'knownkey' */
      ಆದರೆ(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.ಕೀಲಿ = host->ಕೀಲಿ;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.ಕೀಲಿ = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (ಎಣಿಕೆ curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_ssh_keyfunc);
      rc = func(ದತ್ತಾಂಶ, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, ದತ್ತಾಂಶ->ಗಣ.ssh_keyfunc_userp);
      CURL_CBAPI_ಕೊನೆ(&guard);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* no remotekey means failure! */
    infof(ದತ್ತಾಂಶ, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  ಆಯ್ಕೆ(rc) {
  ಪೂರ್ವನಿಯೋಜಿತ: /* unknown return codes is the same as reject */
  ಸ್ಥಿತಿ CURLKHSTAT_REJECT:
    infof(ದತ್ತಾಂಶ, "SSH: knownhost check failed");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  ಸ್ಥಿತಿ CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    ಆದರೆ(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  ಸ್ಥಿತಿ CURLKHSTAT_FINE:
  ಸ್ಥಿತಿ CURLKHSTAT_FINE_ಕೂಡಿಸು_ಗಮ್ಯ_FILE:
    /* proceed */
    ಆದರೆ(keycheck != LIBSSH2_KNOWNHOST_CHECK_ಹೊಂದಿಕೆ) {
      ಪೂರ್ಣಾಂಕ addrc;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostbuf;
      ಅಕ್ಷರ *hostport = ಖಾಲಿ;
      ಆದರೆ(conn->origin->ಪೋರ್ಟ್ != ಪೋರ್ಟ್_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->ಪೋರ್ಟ್);
        ಆದರೆ(!hostbuf)
          infof(ದತ್ತಾಂಶ, "WARNING: failed allocating buffer for [host]:port");
      }
      ಇಲ್ಲದಿದ್ದರೆ
        hostbuf = conn->origin->hostname;
      ಆದರೆ(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, ಖಾಲಿ,
                                       remotekey, keylen, ಖಾಲಿ, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, ಖಾಲಿ);
        ಆದರೆ(addrc)
          infof(ದತ್ತಾಂಶ, "WARNING: adding the known host %s failed", hostbuf);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == CURLKHSTAT_FINE_ಕೂಡಿಸು_ಗಮ್ಯ_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          ಪೂರ್ಣಾಂಕ wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          ಆದರೆ(wrc) {
            infof(ದತ್ತಾಂಶ, "WARNING: writing %s failed",
                  CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "SSH: knownhost entry matches host key");
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_check_fingerprint(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ssh_conn *sshc)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pubkey_md5 =
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_MD5);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pubkey_sha256 =
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_SHA256);

  ಆದರೆ(pubkey_sha256) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fingerprint = ಖಾಲಿ;
    ಅಕ್ಷರ *fingerprint_b64 = ಖಾಲಿ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(ದತ್ತಾಂಶ, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    ಆದರೆ(!fingerprint) {
      failf(ದತ್ತಾಂಶ,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
      ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    ಆದರೆ(curlx_base64_encode((ಸ್ಥಿರಾಂಕ uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
      ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
    }

    ಆದರೆ(!fingerprint_b64) {
      failf(ದತ್ತಾಂಶ, "SHA256 fingerprint could not be encoded");
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
      ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(ದತ್ತಾಂಶ, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    ಆಗ((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    ಆಗ((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    ಆದರೆ((pub_pos != b64_pos) ||
       ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(ದತ್ತಾಂಶ,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
      ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(ದತ್ತಾಂಶ, "SSH: SHA256 checksum match");
  }

  ಆದರೆ(pubkey_md5) {
    ಅಕ್ಷರ md5buffer[33];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fingerprint;

    infof(ದತ್ತಾಂಶ, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    ಆದರೆ(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      ಪೂರ್ಣಾಂಕ i;
      ಗಾಗಿ(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)fingerprint[i]);
      }

      infof(ದತ್ತಾಂಶ, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    ಆದರೆ(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      ಆದರೆ(fingerprint) {
        failf(ದತ್ತಾಂಶ,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        failf(ದತ್ತಾಂಶ,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
      ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(ದತ್ತಾಂಶ, "SSH: MD5 checksum match");
  }

  ಆದರೆ(!pubkey_md5 && !pubkey_sha256) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssh_hostkeyfunc) {
      size_t keylen = 0;
      ಪೂರ್ಣಾಂಕ sshkeytype = 0;
      ಪೂರ್ಣಾಂಕ rc = 0;
      /* we handle the process to the callback */
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      ಆದರೆ(remotekey) {
        ರಚನೆ Curl_mapi_guard guard;
        ಎಣಿಕೆ curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_ssh_hostkeyfunc);
        rc = ದತ್ತಾಂಶ->ಗಣ.ssh_hostkeyfunc(ದತ್ತಾಂಶ->ಗಣ.ssh_hostkeyfunc_userp,
                                       (ಪೂರ್ಣಾಂಕ)keytype, remotekey, keylen);
        CURL_CBAPI_ಕೊನೆ(&guard);
        ಆದರೆ(rc != CURLKHMATCH_OK) {
          myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
          failf(ದತ್ತಾಂಶ, "SSH: callback failed host public key verification");
          ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(ದತ್ತಾಂಶ, "SSH: verified public key via callback");
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
        ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
      }
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      CURL_TRC_SSH(ದತ್ತಾಂಶ, "no host key checksum given, checking knownhosts");
      ಹಿಂತಿರುಗಿ ssh_knownhost(ದತ್ತಾಂಶ, sshc);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* as we already matched, we skip the check for known hosts */
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
ಸ್ಥಿರ CURLcode ssh_force_knownhost_ಕೀಲಿ_type(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ ssh_conn *sshc)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hostkey_ವಿಧಾನ_ssh_ed25519[] = "ssh-ed25519";
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hostkey_ವಿಧಾನ_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hostkey_ವಿಧಾನ_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hostkey_ವಿಧಾನ_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hostkey_ವಿಧಾನ_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHDSS
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ hostkey_ವಿಧಾನ_ssh_dss[] = "ssh-dss";
#ಆದರೆ_ಅಂತ್ಯ
  bool ಸಿಕ್ಕಿತು = FALSE;

  ಆದರೆ(sshc->kh &&
     !CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_MD5) &&
     !CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_SHA256)) {
    ರಚನೆ libssh2_knownhost *store = ಖಾಲಿ;
    ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_ಕೀಲಿ_ಮುಖವಾಡ >>
                      LIBSSH2_KNOWNHOST_ಕೀಲಿ_ಸರಿಸು) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    ಆಗ(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      ಆದರೆ(store) {
        ಆದರೆ(store->name) {
          ಆದರೆ(store->name[0] == '[') {
            curl_off_t ಪೋರ್ಟ್;
            size_t kh_name_ಗಾತ್ರ = 0;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *kh_name_ಕೊನೆ = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(store->name, "]:");
            ಆದರೆ(!kh_name_ಕೊನೆ) {
              infof(ದತ್ತಾಂಶ, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS));
              ಮುಂದುವರಿ;
            }
            p = kh_name_ಕೊನೆ + 2; /* start of port number */
            ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ಪೋರ್ಟ್, 0xffff) &&
               (kh_name_ಕೊನೆ && (ಪೋರ್ಟ್ == conn->origin->ಪೋರ್ಟ್))) {
              kh_name_ಗಾತ್ರ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(store->name) - 1 - ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(kh_name_ಕೊನೆ);
              ಆದರೆ((kh_name_ಗಾತ್ರ == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(conn->origin->hostname)) &&
                 !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(store->name + 1, conn->origin->hostname,
                          kh_name_ಗಾತ್ರ)) {
                ಸಿಕ್ಕಿತು = TRUE;
                ನಿಲ್ಲಿಸು;
              }
            }
          }
          ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp(store->name, conn->origin->hostname)) {
            ಸಿಕ್ಕಿತು = TRUE;
            ನಿಲ್ಲಿಸು;
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(store->ಕೀಲಿ) {
          ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type = (store->typemask & LIBSSH2_KNOWNHOST_ಕೀಲಿ_ಮುಖವಾಡ)
                              >> LIBSSH2_KNOWNHOST_ಕೀಲಿ_ಸರಿಸು;
          ಆದರೆ(!absent_type[type]) {
            ಪೂರ್ಣಾಂಕ keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->ಪೋರ್ಟ್ != ಪೋರ್ಟ್_SSH) ? conn->origin->ಪೋರ್ಟ್ : -1,
              store->ಕೀಲಿ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(store->ಕೀಲಿ),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_ಕೀಲಿ_ಮುಖವಾಡ),
              ಖಾಲಿ);
            ಆದರೆ(keycheck == LIBSSH2_KNOWNHOST_CHECK_ಹೊಂದಿಕೆ) {
              ಸಿಕ್ಕಿತು = TRUE;
              ನಿಲ್ಲಿಸು;
            }
            ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    ಆದರೆ(ಸಿಕ್ಕಿತು) {
      ಪೂರ್ಣಾಂಕ rc;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostkey_ವಿಧಾನ = ಖಾಲಿ;
      infof(ದತ್ತಾಂಶ, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS));

      ಆಯ್ಕೆ(store->typemask & LIBSSH2_KNOWNHOST_ಕೀಲಿ_ಮುಖವಾಡ) {
      ಸ್ಥಿತಿ LIBSSH2_KNOWNHOST_ಕೀಲಿ_ED25519:
        hostkey_ವಿಧಾನ = hostkey_ವಿಧಾನ_ssh_ed25519;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_521:
        hostkey_ವಿಧಾನ = hostkey_ವಿಧಾನ_ssh_ecdsa_521;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_384:
        hostkey_ವಿಧಾನ = hostkey_ವಿಧಾನ_ssh_ecdsa_384;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_256:
        hostkey_ವಿಧಾನ = hostkey_ವಿಧಾನ_ssh_ecdsa_256;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHRSA:
        hostkey_ವಿಧಾನ = hostkey_ವಿಧಾನ_ssh_rsa_all;
        ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHDSS
      ಸ್ಥಿತಿ LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHDSS:  /* deprecated upstream */
        hostkey_ವಿಧಾನ = hostkey_ವಿಧಾನ_ssh_dss;
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
      ಸ್ಥಿತಿ LIBSSH2_KNOWNHOST_ಕೀಲಿ_RSA1:
        failf(ದತ್ತಾಂಶ, "Found host key type RSA1 which is not supported");
        ಹಿಂತಿರುಗಿ CURLE_SSH;
      ಪೂರ್ವನಿಯೋಜಿತ:
        failf(ದತ್ತಾಂಶ, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_ಕೀಲಿ_ಮುಖವಾಡ));
        ಹಿಂತಿರುಗಿ CURLE_SSH;
      }

      infof(ದತ್ತಾಂಶ, "SSH: set '%s' as hostkey type", hostkey_ವಿಧಾನ);
      rc = libssh2_session_ವಿಧಾನ_pref(sshc->ssh_session,
                                       LIBSSH2_ವಿಧಾನ_HOSTKEY, hostkey_ವಿಧಾನ);
      ಆದರೆ(rc) {
        ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ errlen;
        libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, &errlen, 0);
        failf(ದತ್ತಾಂಶ, "libssh2 method '%s' failed: %s", hostkey_ವಿಧಾನ, err_ಸಂದೇಶ);
        ಫಲಿತಾಂಶ = libssh2_session_error_ಗಮ್ಯ_CURLE(rc);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS));
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode quote_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ssh_conn *sshc)
{
  failf(ದತ್ತಾಂಶ, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
}

ಸ್ಥಿರ CURLcode sftp_quote(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ ssh_conn *sshc,
                           ರಚನೆ SSHPROTO *sshp)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cp;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd = sshc->quote_ಅಂಶ->ದತ್ತಾಂಶ;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ಆದರೆ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ಆದರೆ(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    ಅಕ್ಷರ *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->ಮಾರ್ಗ);
    ಆದರೆ(!tmp)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು, "PWD\n", 4);
    Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, tmp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, tmp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmp));
    curlx_free(tmp);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ಆದರೆ(!cp) {
    failf(ದತ್ತಾಂಶ, "Syntax error command '%s', missing parameter", cmd);
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  ಫಲಿತಾಂಶ = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
      failf(ದತ್ತಾಂಶ, "Syntax error: Bad first parameter to '%s'", cmd);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chgrp ", 6) ||
     !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chmod ", 6) ||
     !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chown ", 6) ||
     !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "atime ", 6) ||
     !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    ಫಲಿತಾಂಶ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
        failf(ದತ್ತಾಂಶ, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);

    memset(&sshp->quote_attrs, 0, ಗಾತ್ರ(LIBSSH2_SFTP_ಗುಣಲಕ್ಷಣಗಳು));
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_STAT);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "ln ", 3) ||
     !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    ಫಲಿತಾಂಶ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
        failf(ದತ್ತಾಂಶ, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "mkdir ", 6)) {
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    /* create directory */
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_MKDIR);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    ಫಲಿತಾಂಶ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
        failf(ದತ್ತಾಂಶ, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_RENAME);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "rmdir ", 6)) {
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    /* delete directory */
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_RMDIR);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "rm ", 3)) {
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_UNLINK);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "statvfs ", 8)) {
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_STATVFS);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  failf(ದತ್ತಾಂಶ, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
}

ಸ್ಥಿರ CURLcode sftp_upload_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ssh_conn *sshc,
                                 ರಚನೆ SSHPROTO *sshp,
                                 bool *blockp)
{
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from) {
    LIBSSH2_SFTP_ಗುಣಲಕ್ಷಣಗಳು attrs;
    ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from < 0) {
      ಪೂರ್ಣಾಂಕ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->ಮಾರ್ಗ,
                                    curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಆದರೆ(rc) {
        ದತ್ತಾಂಶ->state.resume_from = 0;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        curl_off_t ಗಾತ್ರ_2 = attrs.filesize;
        ಆದರೆ(ಗಾತ್ರ_2 < 0) {
          failf(ದತ್ತಾಂಶ, "Bad file size (%" FMT_OFF_T ")", ಗಾತ್ರ_2);
          ಹಿಂತಿರುಗಿ CURLE_BAD_DOWNLOAD_RESUME;
        }
        ದತ್ತಾಂಶ->state.resume_from = attrs.filesize;
      }
    }
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->ಮಾರ್ಗ,
                         curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ)),
                         flags, (ದೀರ್ಘ)ದತ್ತಾಂಶ->ಗಣ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  ಆದರೆ(!sshc->sftp_handle) {
    CURLcode ಫಲಿತಾಂಶ;
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr;
    ಪೂರ್ಣಾಂಕ rc = libssh2_session_ಕೊನೆಯ_errno(sshc->ssh_session);

    ಆದರೆ(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    ಆದರೆ(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    ಇಲ್ಲದಿದ್ದರೆ
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    ಆದರೆ(sshc->ಎರಡನೇ_Create_Dirs) {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      failf(ದತ್ತಾಂಶ, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      ಹಿಂತಿರುಗಿ sftp_libssh2_error_ಗಮ್ಯ_CURLE(sftperr);
    }
    ಆದರೆ((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_ಮಾರ್ಗ) &&
       ದತ್ತಾಂಶ->ಗಣ.ftp_create_missing_dirs &&
       (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ) > 1)) {
      /* try to create the path remotely */
      sshc->ಎರಡನೇ_Create_Dirs = 1;
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    ಫಲಿತಾಂಶ = sftp_libssh2_error_ಗಮ್ಯ_CURLE(sftperr);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      ಫಲಿತಾಂಶ = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(ದತ್ತಾಂಶ, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from > 0 && !ದತ್ತಾಂಶ->ಗಣ.remote_append) {
    ಪೂರ್ಣಾಂಕ seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.seek_func) {
      ರಚನೆ Curl_mapi_guard guard;
      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_seek_func);
      seekerr = ದತ್ತಾಂಶ->ಗಣ.seek_func(ದತ್ತಾಂಶ->ಗಣ.seek_client,
                                    ದತ್ತಾಂಶ->state.resume_from, SEEK_SET);
      CURL_CBAPI_ಕೊನೆ(&guard);
    }

    ಆದರೆ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      ಆದರೆ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(ದತ್ತಾಂಶ, "Could not seek stream");
        ಹಿಂತಿರುಗಿ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ಮಾಡು {
        ರಚನೆ Curl_mapi_guard guard;
        ಅಕ್ಷರ scratch[4 * 1024];
        size_t readthisamountnow =
          (ದತ್ತಾಂಶ->state.resume_from - passed >
           (curl_off_t)ಗಾತ್ರ(scratch)) ?
          ಗಾತ್ರ(scratch) : curlx_sotouz(ದತ್ತಾಂಶ->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fread_func);
        actuallyread = ದತ್ತಾಂಶ->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              ದತ್ತಾಂಶ->state.ಒಳ);
        CURL_CBAPI_ಕೊನೆ(&guard);

        passed += actuallyread;
        ಆದರೆ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(ದತ್ತಾಂಶ, "Failed to read data");
          ಹಿಂತಿರುಗಿ CURLE_FTP_COULDNT_USE_REST;
        }
      } ಆಗ(passed < ದತ್ತಾಂಶ->state.resume_from);
    }

    /* now, decrease the size of the read */
    ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize > 0) {
      ದತ್ತಾಂಶ->state.infilesize -= ದತ್ತಾಂಶ->state.resume_from;
      ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ದತ್ತಾಂಶ->state.infilesize;
      Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)ದತ್ತಾಂಶ->state.resume_from);
  }
  ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize > 0) {
    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ದತ್ತಾಂಶ->state.infilesize;
    Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);
  }
  /* upload data */
  Curl_xfer_ಸಿದ್ಧತೆ_send(ದತ್ತಾಂಶ, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ದತ್ತಾಂಶ->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(ದತ್ತಾಂಶ);

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ ssh_state_pkey_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(sshc->authlist, "publickey")) {
    ಆದರೆ(sshc->pub_ಕೀಲಿ)
      infof(ದತ್ತಾಂಶ, "SSH: trying public key file '%s'", sshc->pub_ಕೀಲಿ);
    infof(ದತ್ತಾಂಶ, "SSH: trying private key file '%s'", sshc->priv_ಕೀಲಿ);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PKEY);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PASS_INIT);
}

ಸ್ಥಿರ CURLcode sftp_quote_stat(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ssh_conn *sshc,
                                ರಚನೆ SSHPROTO *sshp,
                                bool *blockp)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd = sshc->quote_ಅಂಶ->ದತ್ತಾಂಶ;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  ಆದರೆ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    ಪೂರ್ಣಾಂಕ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ಆದರೆ(rc && !sshc->acceptfail) { /* get those attributes */
      ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
      failf(ದತ್ತಾಂಶ, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      ಹೋಗು fail;
    }
  }

  /* Now set the new attributes... */
  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chgrp", 5)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = sshc->quote_path1;
    curl_off_t gid;
    ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!sshc->acceptfail) {
      failf(ದತ್ತಾಂಶ, "Syntax error: chgrp gid not a number");
      ಹೋಗು fail;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chmod", 5)) {
    curl_off_t perms;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = sshc->quote_path1;
    /* permissions are octal */
    ಆದರೆ(curlx_str_octal(&p, &perms, 07777)) {
      failf(ದತ್ತಾಂಶ, "Syntax error: chmod permissions not a number");
      ಹೋಗು fail;
    }

    sshp->quote_attrs.permissions = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chown", 5)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = sshc->quote_path1;
    curl_off_t uid;
    ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!sshc->acceptfail) {
      failf(ದತ್ತಾಂಶ, "Syntax error: chown uid not a number");
      ಹೋಗು fail;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "atime", 5) ||
          !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    ಆದರೆ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(ದತ್ತಾಂಶ, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ಆದರೆ SIZEOF_ಸಮಯ_T > SIZEOF_LONG
    ಆದರೆ(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(ದತ್ತಾಂಶ, "date overflow");
      fail = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(fail)
      ಹೋಗು fail;
    ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "atime", 5))
      sshp->quote_attrs.atime = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)date;
    ಇಲ್ಲದಿದ್ದರೆ /* mtime */
      sshp->quote_attrs.mtime = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
}

ಸ್ಥಿರ CURLcode sftp_download_stat(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ssh_conn *sshc,
                                   ರಚನೆ SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ಗುಣಲಕ್ಷಣಗಳು attrs;
  ಪೂರ್ಣಾಂಕ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->ಮಾರ್ಗ,
                                curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ)),
                                LIBSSH2_SFTP_STAT, &attrs);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಆದರೆ(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_ಗಾತ್ರ) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1;
    ದತ್ತಾಂಶ->req.maxdownload = -1;
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    curl_off_t ಗಾತ್ರ_2 = attrs.filesize;

    ಆದರೆ(ಗಾತ್ರ_2 < 0) {
      failf(ದತ್ತಾಂಶ, "Bad file size (%" FMT_OFF_T ")", ಗಾತ್ರ_2);
      ಹಿಂತಿರುಗಿ CURLE_BAD_DOWNLOAD_RESUME;
    }
    ಆದರೆ(ದತ್ತಾಂಶ->state.use_range) {
      curl_off_t from;
      CURLcode ಫಲಿತಾಂಶ = Curl_ssh_range(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.range, ಗಾತ್ರ_2,
                                       &from, &ಗಾತ್ರ_2);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
    ದತ್ತಾಂಶ->req.maxdownload = ಗಾತ್ರ_2;
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, ಗಾತ್ರ_2);
  }

  /* We can resume if we can seek to the resume position */
  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from) {
    ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ಆದರೆ((curl_off_t)attrs.filesize < -ದತ್ತಾಂಶ->state.resume_from) {
        failf(ದತ್ತಾಂಶ, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              ದತ್ತಾಂಶ->state.resume_from, (curl_off_t)attrs.filesize);
        ಹಿಂತಿರುಗಿ CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      ದತ್ತಾಂಶ->state.resume_from += attrs.filesize;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ((curl_off_t)attrs.filesize < ದತ್ತಾಂಶ->state.resume_from) {
        failf(ದತ್ತಾಂಶ, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              ದತ್ತಾಂಶ->state.resume_from, (curl_off_t)attrs.filesize);
        ಹಿಂತಿರುಗಿ CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = attrs.filesize - ದತ್ತಾಂಶ->state.resume_from;
    ದತ್ತಾಂಶ->req.maxdownload = attrs.filesize - ದತ್ತಾಂಶ->state.resume_from;
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, attrs.filesize - ದತ್ತಾಂಶ->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)ದತ್ತಾಂಶ->state.resume_from);
  }

  /* Setup the actual download */
  ಆದರೆ(ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 == 0) {
    /* no data to transfer */
    Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
    infof(ದತ್ತಾಂಶ, "SSH: file already completely downloaded");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ದತ್ತಾಂಶ->conn->send_idx = 0;

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode sftp_readdir(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ssh_conn *sshc,
                             ರಚನೆ SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_ಮಾರ್ಗ_MAX,
                                   sshp->readdir_longentry, CURL_ಮಾರ್ಗ_MAX,
                                   &sshp->readdir_attrs);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಆದರೆ(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.list_only) {
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY, "\n", 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&sshp->readdir, sshp->readdir_longentry);

      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        ಆದರೆ((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          ಫಲಿತಾಂಶ = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->ಮಾರ್ಗ,
                                  sshp->readdir_filename);
          myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_LINK);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!rc) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_ಮುಗಿದ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    ಫಲಿತಾಂಶ = sftperr ? sftp_libssh2_error_ಗಮ್ಯ_CURLE(sftperr) : CURLE_SSH;
    failf(ದತ್ತಾಂಶ, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_ಕೊನೆಯ_errno(sshc->ssh_session));
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ssh_conn *sshc)
{
  CURLcode ಫಲಿತಾಂಶ;
  sshc->ಎರಡನೇ_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_ಗಣ_blocking(sshc->ssh_session, 0);

  ಫಲಿತಾಂಶ = ssh_force_knownhost_ಕೀಲಿ_type(ದತ್ತಾಂಶ, sshc);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_S_STARTUP);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_startup(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ssh_conn *sshc)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಪೂರ್ಣಾಂಕ rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc) {
    ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
    (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
    failf(ದತ್ತಾಂಶ, "Failure establishing ssh session: %d, %s", rc, err_ಸಂದೇಶ);

    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_HOSTKEY);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_hostkey(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode ಫಲಿತಾಂಶ = ssh_check_fingerprint(ದತ್ತಾಂಶ, sshc);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTHLIST);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_authlist(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ssh_conn *sshc)
{
  /*
   * Figure out authentication methods
   * NB: As soon as we have provided a username to an openssh server we
   * must never change it later. Thus, always specify the correct username
   * here, even though the libssh2 docs kind of indicate that it should be
   * possible to get a 'generic' list (not user-specific) of authentication
   * methods, presumably with a blank username. That does not work in my
   * experience.
   * Therefore always specify it here.
   */
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(user)));

  ಆದರೆ(!sshc->authlist) {
    ಪೂರ್ಣಾಂಕ rc;
    ಆದರೆ(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(ದತ್ತಾಂಶ, "SSH: user accepted with no authentication");
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    rc = libssh2_session_ಕೊನೆಯ_errno(sshc->ssh_session);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
    ಹಿಂತಿರುಗಿ libssh2_session_error_ಗಮ್ಯ_CURLE(rc);
  }
  infof(ದತ್ತಾಂಶ, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PKEY_INIT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_pkey(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user = Curl_creds_user(conn->creds);
  ಪೂರ್ಣಾಂಕ rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(user)),
    sshc->pub_ಕೀಲಿ, sshc->priv_ಕೀಲಿ, sshc->passphrase ? sshc->passphrase : "");
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc == 0) {
    sshc->authed = TRUE;
    infof(ದತ್ತಾಂಶ, "SSH: authenticated via publickey");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
    ಅಕ್ಷರ unknown[] = "Reason unknown (-1)";
    ಆದರೆ(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_ಸಂದೇಶ = unknown;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
    }
    infof(ದತ್ತಾಂಶ, "SSH: publickey authentication denied: %s", err_ಸಂದೇಶ);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PASS_INIT);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_pass_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(sshc->authlist, "password")) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PASS);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_HOST_INIT);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_pass(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssh_conn *sshc)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user = Curl_creds_user(conn->creds);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd = Curl_creds_passwd(conn->creds);
  ಪೂರ್ಣಾಂಕ rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(user)),
                                 passwd,
                                 curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(passwd)),
                                 ಖಾಲಿ);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN) {
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  }
  ಆದರೆ(rc == 0) {
    sshc->authed = TRUE;
    infof(ದತ್ತಾಂಶ, "SSH: initialized password authentication");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_HOST_INIT);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_host_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(sshc->authlist, "hostbased")) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_HOST);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_AGENT_INIT);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_agent_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = 0;
  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(sshc->authlist, "publickey")) {

    infof(ದತ್ತಾಂಶ, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    ಆದರೆ(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      ಆದರೆ(!sshc->ssh_agent) {
        infof(ದತ್ತಾಂಶ, "SSH: could not create agent object");

        myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    ಆದರೆ(rc < 0) {
      infof(ದತ್ತಾಂಶ, "SSH: failure connecting to agent");
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_agent_list(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = libssh2_agent_list_identities(sshc->ssh_agent);

  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  ಆದರೆ(rc < 0) {
    infof(ದತ್ತಾಂಶ, "SSH: failure requesting identities to agent");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_ತಟಸ್ಥ = ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_agent(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  ಪೂರ್ಣಾಂಕ rc = libssh2_agent_get_ತಟಸ್ಥ(sshc->ssh_agent,
                                      &sshc->sshagent_ತಟಸ್ಥ,
                                      sshc->sshagent_prev_ತಟಸ್ಥ);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc == 0) {
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(ದತ್ತಾಂಶ->conn->creds),
                 sshc->sshagent_ತಟಸ್ಥ->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(ದತ್ತಾಂಶ->conn->creds),
                                sshc->sshagent_ತಟಸ್ಥ);

    ಆದರೆ(rc < 0) {
      ಆದರೆ(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_ತಟಸ್ಥ = sshc->sshagent_ತಟಸ್ಥ;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
  }

  ಆದರೆ(rc < 0)
    infof(ದತ್ತಾಂಶ, "SSH: failure requesting identities to agent");
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == 1)
    infof(ದತ್ತಾಂಶ, "SSH: no agent identity would match");

  ಆದರೆ(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(ದತ್ತಾಂಶ, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(ದತ್ತಾಂಶ->conn->creds),
          sshc->sshagent_ತಟಸ್ಥ->comment);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_ಕೀಲಿ_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ((ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(sshc->authlist, "keyboard-interactive")) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಕೀಲಿ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_ಕೀಲಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user = Curl_creds_user(conn->creds);
  ಪೂರ್ಣಾಂಕ rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(user)),
                                             &kbd_callback);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc == 0) {
    sshc->authed = TRUE;
    infof(ದತ್ತಾಂಶ, "SSH: initialized keyboard interactive authentication");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssh_conn *sshc)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಆದರೆ(!sshc->authed) {
    failf(ದತ್ತಾಂಶ, "Authentication failure");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
    ಹಿಂತಿರುಗಿ CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(ದತ್ತಾಂಶ, "SSH: authentication complete");

  Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_APPCONNECT); /* SSH is connected */

  ದತ್ತಾಂಶ->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ಆದರೆ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_INIT);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  infof(ದತ್ತಾಂಶ, "SSH: connection established");
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  ಆದರೆ(!sshc->sftp_session) {
    ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
    ಆದರೆ(libssh2_session_ಕೊನೆಯ_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
    failf(ದತ್ತಾಂಶ, "Failure initializing sftp session: %s", err_ಸಂದೇಶ);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_REALPATH);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_realpath(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ssh_conn *sshc,
                                        ರಚನೆ SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_ಮಾರ್ಗ_MAX,
                               LIBSSH2_SFTP_REALPATH);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಆದರೆ(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    ಆದರೆ(!sshc->homedir)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    curlx_free(ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath);
    ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    ಆದರೆ(!ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Return the error type */
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    CURLcode ಫಲಿತಾಂಶ;
    ಆದರೆ(sftperr)
      ಫಲಿತಾಂಶ = sftp_libssh2_error_ಗಮ್ಯ_CURLE(sftperr);
    ಇಲ್ಲದಿದ್ದರೆ
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      ಫಲಿತಾಂಶ = CURLE_SSH;
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "error = %lu makes libcurl = %d", sftperr, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(ದತ್ತಾಂಶ, "CONNECT phase done");
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ ssh_conn *sshc,
                                          ರಚನೆ SSHPROTO *sshp)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = Curl_getworkingpath(ದತ್ತಾಂಶ, sshc->homedir, &sshp->ಮಾರ್ಗ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.quote) {
    infof(ದತ್ತಾಂಶ, "SSH: sending quote commands");
    sshc->quote_ಅಂಶ = ದತ್ತಾಂಶ->ಗಣ.quote;
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_GETINFO);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_postquote_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.postquote) {
    infof(ದತ್ತಾಂಶ, "SSH: sending quote commands");
    sshc->quote_ಅಂಶ = ದತ್ತಾಂಶ->ಗಣ.postquote;
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ssh_conn *sshc,
                                     ರಚನೆ SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = sftp_quote(ದತ್ತಾಂಶ, sshc, sshp);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_ಮುಂದಿನ_quote(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_ಅಂಶ = sshc->quote_ಅಂಶ->ಮುಂದಿನ;

  ಆದರೆ(sshc->quote_ಅಂಶ) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(sshc->nextstate != SSH_NO_STATE) {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_GETINFO);
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_stat(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ ssh_conn *sshc,
                                          ರಚನೆ SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = sftp_quote_stat(ದತ್ತಾಂಶ, sshc, sshp, blockp);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_setstat(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ ssh_conn *sshc,
                                             ರಚನೆ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc && !sshc->acceptfail) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    failf(ದತ್ತಾಂಶ, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_symlink(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc && !sshc->acceptfail) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    failf(ದತ್ತಾಂಶ, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_mkdir(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path1)),
                                 (ದೀರ್ಘ)ದತ್ತಾಂಶ->ಗಣ.new_directory_perms);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc && !sshc->acceptfail) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    failf(ದತ್ತಾಂಶ, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_rename(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc && !sshc->acceptfail) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    failf(ದತ್ತಾಂಶ, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_rmdir(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path1)));
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc && !sshc->acceptfail) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    failf(ದತ್ತಾಂಶ, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_unlink(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path1)));
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc && !sshc->acceptfail) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    failf(ದತ್ತಾಂಶ, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_statvfs(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  ಪೂರ್ಣಾಂಕ rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->quote_path1)),
                                &statvfs);

  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc && !sshc->acceptfail) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    failf(ದತ್ತಾಂಶ, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == 0) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _MSC_VER
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "I64u"
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "llu"
#ಆದರೆ_ಅಂತ್ಯ
    CURLcode ಫಲಿತಾಂಶ;
    ಅಕ್ಷರ *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_ಖಂಡಗಳು, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    ಆದರೆ(!tmp) {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, tmp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmp));
    curlx_free(tmp);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_create_dirs_mkdir(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                 ರಚನೆ ssh_conn *sshc,
                                                 ರಚನೆ SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->ಮಾರ್ಗ,
                             curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ)),
                             (ದೀರ್ಘ)ದತ್ತಾಂಶ->ಗಣ.new_directory_perms);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ಆದರೆ(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    ಆದರೆ((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      ಹಿಂತಿರುಗಿ sftp_libssh2_error_ಗಮ್ಯ_CURLE(sftperr);
    }
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CREATE_DIRS);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ ssh_conn *sshc,
                                            ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, -1);
  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->ಮಾರ್ಗ,
                         curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  ಆದರೆ(!sshc->sftp_handle) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr;
    ಆದರೆ(libssh2_session_ಕೊನೆಯ_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    failf(ದತ್ತಾಂಶ, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    ಹಿಂತಿರುಗಿ sftp_libssh2_error_ಗಮ್ಯ_CURLE(sftperr);
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir_link(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ ssh_conn *sshc,
                                            ರಚನೆ SSHPROTO *sshp)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ rc;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_ಮಾರ್ಗ_MAX, LIBSSH2_SFTP_READLINK);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  ಆದರೆ(rc < 0)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* append filename and extra output */
  ಫಲಿತಾಂಶ = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_BOTTOM);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_download_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ ssh_conn *sshc,
                                            ರಚನೆ SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, ಗಾತ್ರ(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->ಮಾರ್ಗ, &sb);
  ಆದರೆ(!sshc->ssh_channel) {
    ಪೂರ್ಣಾಂಕ ssh_err;
    ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;

    ಆದರೆ(libssh2_session_ಕೊನೆಯ_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    ssh_err = libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
    failf(ದತ್ತಾಂಶ, "%s", err_ಸಂದೇಶ);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_CHANNEL_FREE);
    ಹಿಂತಿರುಗಿ libssh2_session_error_ಗಮ್ಯ_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_ಗಾತ್ರ;
  ದತ್ತಾಂಶ->req.maxdownload = (curl_off_t)sb.st_ಗಾತ್ರ;
  Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ದತ್ತಾಂಶ->conn->send_idx = 0;

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ssh_conn *sshc,
                                     ರಚನೆ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಂಕ rc = 0;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    ಆದರೆ(rc < 0) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to close libssh2 file: %d %s", rc, err_ಸಂದೇಶ);
    }
    sshc->sftp_handle = ಖಾಲಿ;
  }

  curlx_safefree(sshp->ಮಾರ್ಗ);

  CURL_TRC_SSH(ದತ್ತಾಂಶ, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  ಆದರೆ(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_shutdown(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ಪೂರ್ಣಾಂಕ rc = 0;
  ಆದರೆ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    ಆದರೆ(rc < 0) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to close libssh2 file: %d %s", rc, err_ಸಂದೇಶ);
    }
    sshc->sftp_handle = ಖಾಲಿ;
  }
  ಆದರೆ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    ಆದರೆ(rc < 0) {
      infof(ದತ್ತಾಂಶ, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = ಖಾಲಿ;
  }

  curlx_safefree(sshc->homedir);

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_DISCONNECT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_download_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ ssh_conn *sshc,
                                             ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->ಮಾರ್ಗ,
                         curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ)),
                         LIBSSH2_FXF_READ, (ದೀರ್ಘ)ದತ್ತಾಂಶ->ಗಣ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  ಆದರೆ(!sshc->sftp_handle) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sftperr;
    ಆದರೆ(libssh2_session_ಕೊನೆಯ_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_ಕೊನೆಯ_error(sshc->sftp_session);
    failf(ದತ್ತಾಂಶ, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    ಹಿಂತಿರುಗಿ sftp_libssh2_error_ಗಮ್ಯ_CURLE(sftperr);
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_upload_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ ssh_conn *sshc,
                                          ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->ಮಾರ್ಗ,
                       (ಪೂರ್ಣಾಂಕ)ದತ್ತಾಂಶ->ಗಣ.new_file_perms,
                       (libssh2_int64_t)ದತ್ತಾಂಶ->state.infilesize, 0, 0);
  ಆದರೆ(!sshc->ssh_channel) {
    ಪೂರ್ಣಾಂಕ ssh_err;
    ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
    CURLcode ಫಲಿತಾಂಶ;
    ಆದರೆ(libssh2_session_ಕೊನೆಯ_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    ssh_err = libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
    failf(ದತ್ತಾಂಶ, "%s", err_ಸಂದೇಶ);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_CHANNEL_FREE);
    ಫಲಿತಾಂಶ = libssh2_session_error_ಗಮ್ಯ_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_SSH ||
       ಫಲಿತಾಂಶ == CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು)
      ಫಲಿತಾಂಶ = CURLE_UPLOAD_FAILED;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  /* upload data */
  ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ದತ್ತಾಂಶ->state.infilesize;
  Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);
  Curl_xfer_ಸಿದ್ಧತೆ_send(ದತ್ತಾಂಶ, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ದತ್ತಾಂಶ->conn->recv_idx = FIRSTSOCKET;

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_session_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ಪೂರ್ಣಾಂಕ rc = 0;
  ಆದರೆ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    ಆದರೆ(rc < 0) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ಸಂದೇಶ);
    }
    sshc->ssh_channel = ಖಾಲಿ;
  }

  ಆದರೆ(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    ಆದರೆ(rc < 0) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to disconnect libssh2 session: %d %s", rc, err_ಸಂದೇಶ);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode sshc_cleanup(ರಚನೆ ssh_conn *sshc, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             bool ಖಂಡ)
{
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = ಖಾಲಿ;
  }

  ಆದರೆ(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    ಆದರೆ((rc < 0) && ದತ್ತಾಂಶ) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_ಸಂದೇಶ);
    }
    ಆದರೆ(!ಖಂಡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = ಖಾಲಿ;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_ತಟಸ್ಥ = ಖಾಲಿ;
    sshc->sshagent_prev_ತಟಸ್ಥ = ಖಾಲಿ;
  }

  ಆದರೆ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ಆದರೆ((rc < 0) && ದತ್ತಾಂಶ) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to close libssh2 file: %d %s", rc, err_ಸಂದೇಶ);
    }
    ಆದರೆ(!ಖಂಡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    sshc->sftp_handle = ಖಾಲಿ;
  }

  ಆದರೆ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ಆದರೆ((rc < 0) && ದತ್ತಾಂಶ) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ಸಂದೇಶ);
    }
    ಆದರೆ(!ಖಂಡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    sshc->ssh_channel = ಖಾಲಿ;
  }

  ಆದರೆ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ಆದರೆ((rc < 0) && ದತ್ತಾಂಶ) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_ಸಂದೇಶ);
    }
    ಆದರೆ(!ಖಂಡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    sshc->sftp_session = ಖಾಲಿ;
  }

  ಆದರೆ(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    ಆದರೆ((rc < 0) && ದತ್ತಾಂಶ) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session, &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to free libssh2 session: %d %s", rc, err_ಸಂದೇಶ);
    }
    ಆದರೆ(!ಖಂಡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;

    sshc->ssh_session = ಖಾಲಿ;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_ಕೀಲಿ);
  curlx_safefree(sshc->priv_ಕೀಲಿ);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_getinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.get_filetime)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_FILETIME);
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_TRANS_INIT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_filetime(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ssh_conn *sshc,
                                        ರಚನೆ SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ಗುಣಲಕ್ಷಣಗಳು attrs;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->ಮಾರ್ಗ,
                            curlx_uztoui(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ)),
                            LIBSSH2_SFTP_STAT, &attrs);
  ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  ಆದರೆ(rc == 0)
    ದತ್ತಾಂಶ->info.filetime = (time_t)attrs.mtime;

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_TRANS_INIT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_trans_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ ssh_conn *sshc,
                                          ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(ದತ್ತಾಂಶ->state.upload)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_UPLOAD_INIT);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sshp) {
    size_t plen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ);
    ಆದರೆ(plen && (sshp->ಮಾರ್ಗ[plen - 1] == '/'))
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_INIT);
    ಇಲ್ಲದಿದ್ದರೆ
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_upload_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ ssh_conn *sshc,
                                           ರಚನೆ SSHPROTO *sshp,
                                           bool *ಖಂಡ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = sftp_upload_init(ದತ್ತಾಂಶ, sshc, sshp, ಖಂಡ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_create_dirs_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                ರಚನೆ ssh_conn *sshc,
                                                ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ) > 1) {
    sshc->slash_pos = sshp->ಮಾರ್ಗ + 1; /* ignore the leading '/' */
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_UPLOAD_INIT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_create_dirs(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ ssh_conn *sshc,
                                           ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ಆದರೆ(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(ದತ್ತಾಂಶ, "SFTP: creating directory '%s'", sshp->ಮಾರ್ಗ);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_UPLOAD_INIT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc,
                                       ರಚನೆ SSHPROTO *sshp,
                                       bool *ಖಂಡ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = sftp_readdir(ದತ್ತಾಂಶ, sshc, sshp, ಖಂಡ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir_bottom(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              ರಚನೆ ssh_conn *sshc,
                                              ರಚನೆ SSHPROTO *sshp)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curlx_dyn_free(&sshp->readdir);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    curlx_dyn_reset(&sshp->readdir);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                            ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;

  sshc->sftp_handle = ಖಾಲಿ;

  /* no data to transfer */
  Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_download_stat(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ರಚನೆ ssh_conn *sshc,
                                             ರಚನೆ SSHPROTO *sshp,
                                             bool *ಖಂಡ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = sftp_download_stat(ದತ್ತಾಂಶ, sshc, sshp, ಖಂಡ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_trans_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ ssh_conn *sshc,
                                         ರಚನೆ SSHPROTO *sshp)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = Curl_getworkingpath(ದತ್ತಾಂಶ, sshc->homedir,
                               &sshp->ಮಾರ್ಗ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->state.upload) {
    ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize < 0) {
      failf(ದತ್ತಾಂಶ, "SCP requires a known file size for upload");
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_CHANNEL_FREE);
      ಹಿಂತಿರುಗಿ CURLE_UPLOAD_FAILED;
    }
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_DOWNLOAD_INIT);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(ದತ್ತಾಂಶ->state.upload)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_SEND_EOF);
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_CHANNEL_FREE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_send_eof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->ssh_channel) {
    ಪೂರ್ಣಾಂಕ rc = libssh2_channel_send_eof(sshc->ssh_channel);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    ಆದರೆ(rc) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session,
                                       &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_ಸಂದೇಶ);
    }
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_WAIT_EOF);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_wait_eof(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->ssh_channel) {
    ಪೂರ್ಣಾಂಕ rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    ಆದರೆ(rc) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session,
                                       &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to get channel EOF: %d %s", rc, err_ಸಂದೇಶ);
    }
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_WAIT_CLOSE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_wait_close(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->ssh_channel) {
    ಪೂರ್ಣಾಂಕ rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    ಆದರೆ(rc) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session,
                                       &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Channel failed to close: %d %s", rc, err_ಸಂದೇಶ);
    }
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_CHANNEL_FREE);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_channel_free(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->ssh_channel) {
    ಪೂರ್ಣಾಂಕ rc = libssh2_channel_free(sshc->ssh_channel);
    ಆದರೆ(rc == LIBSSH2_ERROR_EAGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    ಆದರೆ(rc < 0) {
      ಅಕ್ಷರ *err_ಸಂದೇಶ = ಖಾಲಿ;
      (ಶೂನ್ಯ)libssh2_session_ಕೊನೆಯ_error(sshc->ssh_session,
                                       &err_ಸಂದೇಶ, ಖಾಲಿ, 0);
      infof(ದತ್ತಾಂಶ, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ಸಂದೇಶ);
    }
    sshc->ssh_channel = ಖಾಲಿ;
  }
  CURL_TRC_SSH(ದತ್ತಾಂಶ, "SCP DONE phase complete");
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_session_free(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc)
{
  CURLcode ಫಲಿತಾಂಶ = sshc_cleanup(sshc, ದತ್ತಾಂಶ, FALSE);
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  memset(sshc, 0, ಗಾತ್ರ(ರಚನೆ ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ಸ್ಥಿರ CURLcode ssh_statemachine(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ssh_conn *sshc,
                                 ರಚನೆ SSHPROTO *sshp,
                                 bool *ಖಂಡ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  *ಖಂಡ = 0; /* we are not blocking by default */

  ಮಾಡು {
    ಆಯ್ಕೆ(sshc->state) {
    ಸ್ಥಿತಿ SSH_INIT:
      ಫಲಿತಾಂಶ = ssh_state_init(ದತ್ತಾಂಶ, sshc);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
      FALLTHROUGH();

    ಸ್ಥಿತಿ SSH_S_STARTUP:
      ಫಲಿತಾಂಶ = ssh_state_startup(ದತ್ತಾಂಶ, sshc);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
      FALLTHROUGH();

    ಸ್ಥಿತಿ SSH_HOSTKEY:
      ಫಲಿತಾಂಶ = ssh_state_hostkey(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTHLIST:
      ಫಲಿತಾಂಶ = ssh_state_authlist(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_PKEY:
      ಫಲಿತಾಂಶ = ssh_state_auth_pkey(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_PASS_INIT:
      ಫಲಿತಾಂಶ = ssh_state_auth_pass_init(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_PASS:
      ಫಲಿತಾಂಶ = ssh_state_auth_pass(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_HOST_INIT:
      ಫಲಿತಾಂಶ = ssh_state_auth_host_init(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_HOST:
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_AGENT_INIT);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_AGENT_INIT:
      ಫಲಿತಾಂಶ = ssh_state_auth_agent_init(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_AGENT_LIST:
      ಫಲಿತಾಂಶ = ssh_state_auth_agent_list(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_AGENT:
      ಫಲಿತಾಂಶ = ssh_state_auth_agent(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_ಕೀಲಿ_INIT:
      ಫಲಿತಾಂಶ = ssh_state_auth_ಕೀಲಿ_init(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_ಕೀಲಿ:
      ಫಲಿತಾಂಶ = ssh_state_auth_ಕೀಲಿ(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_AUTH_ಮುಗಿದ:
      ಫಲಿತಾಂಶ = ssh_state_auth_ಮುಗಿದ(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_INIT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_init(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_REALPATH:
      ಫಲಿತಾಂಶ = ssh_state_sftp_realpath(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_INIT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote_init(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_POSTQUOTE_INIT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_postquote_init(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_ಮುಂದಿನ_QUOTE:
      ಫಲಿತಾಂಶ = ssh_state_sftp_ಮುಂದಿನ_quote(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_STAT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote_stat(ದತ್ತಾಂಶ, sshc, sshp, ಖಂಡ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_SETSTAT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote_setstat(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_SYMLINK:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote_symlink(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_MKDIR:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote_mkdir(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_RENAME:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote_rename(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_RMDIR:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote_rmdir(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_UNLINK:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote_unlink(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_STATVFS:
      ಫಲಿತಾಂಶ = ssh_state_sftp_quote_statvfs(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_GETINFO:
      ಫಲಿತಾಂಶ = ssh_state_sftp_getinfo(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_FILETIME:
      ಫಲಿತಾಂಶ = ssh_state_sftp_filetime(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_TRANS_INIT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_trans_init(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_UPLOAD_INIT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_upload_init(ದತ್ತಾಂಶ, sshc, sshp, ಖಂಡ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_CREATE_DIRS_INIT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_create_dirs_init(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_CREATE_DIRS:
      ಫಲಿತಾಂಶ = ssh_state_sftp_create_dirs(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_CREATE_DIRS_MKDIR:
      ಫಲಿತಾಂಶ = ssh_state_sftp_create_dirs_mkdir(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_READDIR_INIT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_readdir_init(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_READDIR:
      ಫಲಿತಾಂಶ = ssh_state_sftp_readdir(ದತ್ತಾಂಶ, sshc, sshp, ಖಂಡ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_READDIR_LINK:
      ಫಲಿತಾಂಶ = ssh_state_sftp_readdir_link(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_READDIR_BOTTOM:
      ಫಲಿತಾಂಶ = ssh_state_sftp_readdir_bottom(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_READDIR_ಮುಗಿದ:
      ಫಲಿತಾಂಶ = ssh_state_sftp_readdir_ಮುಗಿದ(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_DOWNLOAD_INIT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_download_init(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_DOWNLOAD_STAT:
      ಫಲಿತಾಂಶ = ssh_state_sftp_download_stat(ದತ್ತಾಂಶ, sshc, sshp, ಖಂಡ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_CLOSE:
      ಫಲಿತಾಂಶ = ssh_state_sftp_close(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SFTP_SHUTDOWN:
      ಫಲಿತಾಂಶ = ssh_state_sftp_shutdown(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SCP_TRANS_INIT:
      ಫಲಿತಾಂಶ = ssh_state_scp_trans_init(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SCP_UPLOAD_INIT:
      ಫಲಿತಾಂಶ = ssh_state_scp_upload_init(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SCP_DOWNLOAD_INIT:
      ಫಲಿತಾಂಶ = ssh_state_scp_download_init(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SCP_ಮುಗಿದ:
      ಫಲಿತಾಂಶ = ssh_state_scp_ಮುಗಿದ(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SCP_SEND_EOF:
      ಫಲಿತಾಂಶ = ssh_state_scp_send_eof(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SCP_WAIT_EOF:
      ಫಲಿತಾಂಶ = ssh_state_scp_wait_eof(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SCP_WAIT_CLOSE:
      ಫಲಿತಾಂಶ = ssh_state_scp_wait_close(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SCP_CHANNEL_FREE:
      ಫಲಿತಾಂಶ = ssh_state_scp_channel_free(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SESSION_DISCONNECT:
      ಫಲಿತಾಂಶ = ssh_state_session_disconnect(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_SESSION_FREE:
      ಫಲಿತಾಂಶ = ssh_state_session_free(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSH_QUIT:
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* internal error */
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
      ನಿಲ್ಲಿಸು;
    }

  } ಆಗ(!ಫಲಿತಾಂಶ && (sshc->state != SSH_STOP) && !*ಖಂಡ);

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *ಖಂಡ = TRUE;
    ಫಲಿತಾಂಶ = CURLE_OK;
  }
  CURL_TRC_SSH(ದತ್ತಾಂಶ, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಖಂಡ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ssh_block2waitfor(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ssh_conn *sshc,
                              bool ಖಂಡ)
{
  ಪೂರ್ಣಾಂಕ dir = 0;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(ಖಂಡ) {
    dir = libssh2_session_ಖಂಡ_directions(sshc->ssh_session);
    ಆದರೆ(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_ಖಂಡ_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_ಖಂಡ_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  ಆದರೆ(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode ssh_multi_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ರಚನೆ SSHPROTO *sshp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SSH_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool ಖಂಡ; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  ಆದರೆ(!sshc || !sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಮಾಡು {
    ಫಲಿತಾಂಶ = ssh_statemachine(ದತ್ತಾಂಶ, sshc, sshp, &ಖಂಡ);
    *ಮುಗಿದ = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } ಆಗ(!ಫಲಿತಾಂಶ && !*ಮುಗಿದ && !ಖಂಡ);
  ssh_block2waitfor(ದತ್ತಾಂಶ, sshc, ಖಂಡ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssh_ಖಂಡ_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssh_conn *sshc,
                                    ರಚನೆ SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ curltime ಆರಂಭ = *Curl_pgrs_now(ದತ್ತಾಂಶ);

  ಆಗ((sshc->state != SSH_STOP) && !ಫಲಿತಾಂಶ) {
    bool ಖಂಡ;
    timediff_t ಎಡ_ms = 1000;

    ಫಲಿತಾಂಶ = ssh_statemachine(ದತ್ತಾಂಶ, sshc, sshp, &ಖಂಡ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    ಆದರೆ(!disconnect) {
      ಫಲಿತಾಂಶ = Curl_pgrsCheck(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;

      ಎಡ_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
      ಆದರೆ(ಎಡ_ms < 0) {
        failf(ದತ್ತಾಂಶ, "Operation timed out");
        ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &ಆರಂಭ) > 1000) {
      /* disconnect timeout */
      failf(ದತ್ತಾಂಶ, "Disconnect timed out");
      ಫಲಿತಾಂಶ = CURLE_OK;
      ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(ಖಂಡ) {
      ಪೂರ್ಣಾಂಕ dir = libssh2_session_ಖಂಡ_directions(sshc->ssh_session);
      curl_socket_t sock = ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      ಆದರೆ(LIBSSH2_SESSION_ಖಂಡ_INBOUND & dir)
        fd_read = sock;
      ಆದರೆ(LIBSSH2_SESSION_ಖಂಡ_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (ಶೂನ್ಯ)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!ಎಡ_ms || ಎಡ_ms > 1000) ? 1000 : ಎಡ_ms);
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ SSHPROTO *sshp = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_safefree(sshp->ಮಾರ್ಗ);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ ssh_conn *sshc = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  sshc_cleanup(sshc, ಖಾಲಿ, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ಸ್ಥಿರ CURLcode ssh_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ connectdata *conn)
{
  ರಚನೆ ssh_conn *sshc;
  ರಚನೆ SSHPROTO *sshp;
  (ಶೂನ್ಯ)conn;

  sshc = curlx_calloc(1, ಗಾತ್ರ(*sshc));
  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(Curl_conn_meta_ಗಣ(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  sshp = curlx_calloc(1, ಗಾತ್ರ(*sshp));
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  curlx_dyn_init(&sshp->readdir, CURL_ಮಾರ್ಗ_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_ಮಾರ್ಗ_MAX);
  ಆದರೆ(Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ Curl_ssh_ಸಿದ್ಧತೆ_pkey(ದತ್ತಾಂಶ, sshc);
}

ಸ್ಥಿರ Curl_recv scp_recv, sftp_recv;
ಸ್ಥಿರ Curl_send scp_send, sftp_send;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
ಸ್ಥಿರ ssize_t ssh_tls_recv(libssh2_socket_t sock, ಶೂನ್ಯ *ಬಫರ್,
                            size_t ಉದ್ದಳತೆ, ಪೂರ್ಣಾಂಕ flags, ಶೂನ್ಯ **abstract)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = (ರಚನೆ Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(ದತ್ತಾಂಶ, sock);
  size_t nread;
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  Curl_recv *backup = conn->recv[sockindex];
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ಶೂನ್ಯ)flags;

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  ಫಲಿತಾಂಶ = Curl_conn_recv(ದತ್ತಾಂಶ, sockindex, ಬಫರ್, ಉದ್ದಳತೆ, &nread);
  conn->recv[sockindex] = backup;
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
    ಹಿಂತಿರುಗಿ -EAGAIN; /* magic return code for libssh2 */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ -1; /* generic error */
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ದತ್ತಾಂಶ_ಒಳ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಬಫರ್, nread);
  ಹಿಂತಿರುಗಿ (ssize_t)nread;
}

ಸ್ಥಿರ ssize_t ssh_tls_send(libssh2_socket_t sock, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್,
                            size_t ಉದ್ದಳತೆ, ಪೂರ್ಣಾಂಕ flags, ಶೂನ್ಯ **abstract)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = (ರಚನೆ Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(ದತ್ತಾಂಶ, sock);
  size_t nwrite;
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  Curl_send *backup = conn->send[sockindex];
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ಶೂನ್ಯ)flags;

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  ಫಲಿತಾಂಶ = Curl_conn_send(ದತ್ತಾಂಶ, sockindex, ಬಫರ್, ಉದ್ದಳತೆ, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
    ಹಿಂತಿರುಗಿ -EAGAIN; /* magic return code for libssh2 */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ -1; /* error */
  Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಬಫರ್, nwrite);
  ಹಿಂತಿರುಗಿ (ssize_t)nwrite;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ಸ್ಥಿರ CURLcode ssh_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ಫಲಿತಾಂಶ;

#ಆದರೆ LIBSSH2_ಆವೃತ್ತಿ_ಅಂಕೆ >= 0x010b00
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *crypto_str;
    ಆಯ್ಕೆ(libssh2_crypto_engine()) {
    ಸ್ಥಿತಿ libssh2_gcrypt:
      crypto_str = "libgcrypt";
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ libssh2_mbedtls:
      crypto_str = "mbedTLS";
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ libssh2_openssl:
      crypto_str = "openssl compatible";
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ libssh2_os400qc3:
      crypto_str = "OS400QC3";
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ libssh2_wincng:
      crypto_str = "WinCNG";
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      crypto_str = ಖಾಲಿ;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(crypto_str)
      infof(ದತ್ತಾಂಶ, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  infof(ದತ್ತಾಂಶ, "SSH: user '%s'", Curl_creds_user(conn->creds));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_LIBSSH2_DEBUG
  infof(ದತ್ತಾಂಶ, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#ಆದರೆ_ಅಂತ್ಯ /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, ದತ್ತಾಂಶ);

  ಆದರೆ(!sshc->ssh_session) {
    failf(ದತ್ತಾಂಶ, "Failure initializing ssh session");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#ಆದರೆ LIBSSH2_ಆವೃತ್ತಿ_ಅಂಕೆ >= 0x010B00
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.server_response_timeout > 0) {
    libssh2_session_ಗಣ_read_timeout(sshc->ssh_session,
                             (ದೀರ್ಘ)(ದತ್ತಾಂಶ->ಗಣ.server_response_timeout / 1000));
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಆದರೆ(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#ಆದರೆ LIBSSH2_ಆವೃತ್ತಿ_ಅಂಕೆ >= 0x010b01
    infof(ದತ್ತಾಂಶ, "SSH: using HTTPS proxy");
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    ಒಕ್ಕೂಟ receive {
      ಶೂನ್ಯ *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, ಶೂನ್ಯ *, size_t, ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ **);
    };
    ಒಕ್ಕೂಟ transfer {
      ಶೂನ್ಯ *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, size_t, ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ **);
    };
    ಒಕ್ಕೂಟ receive sshrecv;
    ಒಕ್ಕೂಟ transfer sshsend;

    sshrecv.recvptr = ssh_tls_recv;
    sshsend.sendptr = ssh_tls_send;

    infof(ದತ್ತಾಂಶ, "SSH: using HTTPS proxy");
    libssh2_session_callback_ಗಣ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_ಗಣ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#ಆದರೆ_ಅಂತ್ಯ

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_PROXY */
  ಆದರೆ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(ದತ್ತಾಂಶ, "SSH: failed to enable compression for session");
  }

  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS)) {
    ಪೂರ್ಣಾಂಕ rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    ಆದರೆ(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = ಖಾಲಿ;
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    ಆದರೆ(rc < 0)
      infof(ದತ್ತಾಂಶ, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS));
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(ದತ್ತಾಂಶ, "SSH: socket %d", (ಪೂರ್ಣಾಂಕ)sock);
#ಆದರೆ_ಅಂತ್ಯ

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_INIT);

  ಫಲಿತಾಂಶ = ssh_multi_statemach(ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ಸ್ಥಿರ CURLcode scp_perform(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            bool *connected,
                            bool *dophase_ಮುಗಿದ)
{
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SSH_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CURL_TRC_SSH(ದತ್ತಾಂಶ, "DO phase starts");

  *dophase_ಮುಗಿದ = FALSE; /* not done yet */
  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  ಫಲಿತಾಂಶ = ssh_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  *connected = Curl_conn_is_connected(ದತ್ತಾಂಶ->conn, FIRSTSOCKET);

  ಆದರೆ(*dophase_ಮುಗಿದ) {
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "DO phase is complete");
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* called from multi.c while DOing */
ಸ್ಥಿರ CURLcode scp_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಫಲಿತಾಂಶ = ssh_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  ಆದರೆ(*dophase_ಮುಗಿದ) {
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "DO phase is complete");
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ಸ್ಥಿರ CURLcode scp_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ connectdata *conn,
                               bool dead_connection)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ರಚನೆ SSHPROTO *sshp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SSH_EASY);
  (ಶೂನ್ಯ)dead_connection;

  ಆದರೆ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_DISCONNECT);
    ಫಲಿತಾಂಶ = ssh_ಖಂಡ_statemach(ದತ್ತಾಂಶ, sshc, sshp, TRUE);
  }

  ಆದರೆ(sshc)
    ಹಿಂತಿರುಗಿ sshc_cleanup(sshc, ದತ್ತಾಂಶ, TRUE);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ಸ್ಥಿರ CURLcode ssh_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode ಸ್ಥಿತಿ_2)
{
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SSH_CONN);
  ರಚನೆ SSHPROTO *sshp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SSH_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!sshc || !sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(!ಸ್ಥಿತಿ_2)
    /* run the state-machine */
    ಫಲಿತಾಂಶ = ssh_ಖಂಡ_statemach(ದತ್ತಾಂಶ, sshc, sshp, FALSE);
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = ಸ್ಥಿತಿ_2;

  ಆದರೆ(Curl_pgrs_ಮುಗಿದ(ದತ್ತಾಂಶ, Curl_pgrs_now(ದತ್ತಾಂಶ)))
    ಹಿಂತಿರುಗಿ CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode scp_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode ಸ್ಥಿತಿ_2,
                         bool premature)
{
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SSH_CONN);
  (ಶೂನ್ಯ)premature;

  ಆದರೆ(sshc && !ಸ್ಥಿತಿ_2)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ssh_ಮುಗಿದ(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರ CURLcode scp_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                         ಸ್ಥಿರಾಂಕ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ssize_t nwritten;

  (ಶೂನ್ಯ)sockindex; /* we only support SCP on the fixed known primary socket */
  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)mem, len);

  ssh_block2waitfor(ದತ್ತಾಂಶ, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  ಆದರೆ(nwritten == LIBSSH2_ERROR_EAGAIN)
    ಫಲಿತಾಂಶ = CURLE_AGAIN;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nwritten < LIBSSH2_ERROR_NONE)
    ಫಲಿತಾಂಶ = libssh2_session_error_ಗಮ್ಯ_CURLE((ಪೂರ್ಣಾಂಕ)nwritten);
  ಇಲ್ಲದಿದ್ದರೆ
    *pnwritten = (size_t)nwritten;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode scp_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                         ಅಕ್ಷರ *mem, size_t len, size_t *pnread)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ssize_t nread;

  (ಶೂನ್ಯ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(ದತ್ತಾಂಶ, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  ಆದರೆ(nread == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread < LIBSSH2_ERROR_NONE)
    ಫಲಿತಾಂಶ = libssh2_session_error_ಗಮ್ಯ_CURLE((ಪೂರ್ಣಾಂಕ)nread);
  ಇಲ್ಲದಿದ್ದರೆ
    *pnread = (size_t)nread;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * =============== SFTP ===============
 */

/*
 ***********************************************************************
 *
 * sftp_perform()
 *
 * This is the actual DO function for SFTP. Get a file/directory according to
 * the options previously setup.
 */

ಸ್ಥಿರ CURLcode sftp_perform(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             bool *connected,
                             bool *dophase_ಮುಗಿದ)
{
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SSH_CONN);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CURL_TRC_SSH(ದತ್ತಾಂಶ, "DO phase starts");

  *dophase_ಮುಗಿದ = FALSE; /* not done yet */
  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  ಫಲಿತಾಂಶ = ssh_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  *connected = Curl_conn_is_connected(ದತ್ತಾಂಶ->conn, FIRSTSOCKET);

  ಆದರೆ(*dophase_ಮುಗಿದ) {
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "DO phase is complete");
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* called from multi.c while DOing */
ಸ್ಥಿರ CURLcode sftp_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = ssh_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  ಆದರೆ(*dophase_ಮುಗಿದ) {
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "DO phase is complete");
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ಸ್ಥಿರ CURLcode sftp_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn, bool dead_connection)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ರಚನೆ SSHPROTO *sshp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SSH_EASY);
  (ಶೂನ್ಯ)dead_connection;

  ಆದರೆ(sshc) {
    ಆದರೆ(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(ದತ್ತಾಂಶ, "DISCONNECT starts now");
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_SHUTDOWN);
      ಫಲಿತಾಂಶ = ssh_ಖಂಡ_statemach(ದತ್ತಾಂಶ, sshc, sshp, TRUE);
      CURL_TRC_SSH(ದತ್ತಾಂಶ, "DISCONNECT is done -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    }
    sshc_cleanup(sshc, ದತ್ತಾಂಶ, TRUE);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode sftp_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode ಸ್ಥಿತಿ_2,
                          bool premature)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆದರೆ(!ಸ್ಥಿತಿ_2) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    ಆದರೆ(!premature && ದತ್ತಾಂಶ->ಗಣ.postquote && !conn->ಬಿಟ್ಟುಗಳು.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
  }
  ಹಿಂತಿರುಗಿ ssh_ಮುಗಿದ(ದತ್ತಾಂಶ, ಸ್ಥಿತಿ_2);
}

/* return number of sent bytes */
ಸ್ಥಿರ CURLcode sftp_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                          ಸ್ಥಿರಾಂಕ uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (ಶೂನ್ಯ)sockindex;
  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)mem, len);

  ssh_block2waitfor(ದತ್ತಾಂಶ, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  ಆದರೆ(nwrite == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nwrite < LIBSSH2_ERROR_NONE)
    ಹಿಂತಿರುಗಿ libssh2_session_error_ಗಮ್ಯ_CURLE((ಪೂರ್ಣಾಂಕ)nwrite);
  *pnwritten = (size_t)nwrite;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ಸ್ಥಿರ CURLcode sftp_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                          ಅಕ್ಷರ *mem, size_t len, size_t *pnread)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (ಶೂನ್ಯ)sockindex;
  *pnread = 0;

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(ದತ್ತಾಂಶ, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  ಆದರೆ(nread == LIBSSH2_ERROR_EAGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread < 0)
    ಹಿಂತಿರುಗಿ libssh2_session_error_ಗಮ್ಯ_CURLE((ಪೂರ್ಣಾಂಕ)nread);

  *pnread = (size_t)nread;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
ಸ್ಥಿರ CURLcode ssh_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  bool connected = FALSE;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *ಮುಗಿದ = FALSE; /* default to false */
  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1; /* make sure this is unknown at this point */
  sshc->ಎರಡನೇ_Create_Dirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(ದತ್ತಾಂಶ);

  ಆದರೆ(conn->scheme->protocol & CURLPROTO_SCP)
    ಫಲಿತಾಂಶ = scp_perform(ದತ್ತಾಂಶ, &connected, ಮುಗಿದ);
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = sftp_perform(ದತ್ತಾಂಶ, &connected, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_ssh_init(ಶೂನ್ಯ)
{
  ಆದರೆ(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssh_cleanup(ಶೂನ್ಯ)
{
  libssh2_exit();
}

ಶೂನ್ಯ Curl_ssh_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t buflen)
{
  (ಶೂನ್ಯ)curl_msnprintf(ಬಫರ್, buflen, "libssh2/%s", libssh2_ಆವೃತ್ತಿ(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ssh_attach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ connectdata *conn)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  DEBUGASSERT(conn);
  ಆದರೆ(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    ಆದರೆ(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      ಶೂನ್ಯ **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = ದತ್ತಾಂಶ;
    }
  }
}

/*
 * SCP protocol handler.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_scp = {
  ssh_ಸಿದ್ಧತೆ_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_ಮುಗಿದ,                             /* done */
  ZERO_NULL,                            /* do_more */
  ssh_connect,                          /* connect_it */
  ssh_multi_statemach,                  /* connecting */
  scp_doing,                            /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  scp_disconnect,                       /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ssh_attach,                           /* attach */
  ZERO_NULL,                            /* follow */
};

/*
 * SFTP protocol handler.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_sftp = {
  ssh_ಸಿದ್ಧತೆ_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  sftp_ಮುಗಿದ,                            /* done */
  ZERO_NULL,                            /* do_more */
  ssh_connect,                          /* connect_it */
  ssh_multi_statemach,                  /* connecting */
  sftp_doing,                           /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  sftp_disconnect,                      /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ssh_attach,                           /* attach */
  ZERO_NULL,                            /* follow */
};

#ಆದರೆ_ಅಂತ್ಯ /* USE_LIBSSH2 */
