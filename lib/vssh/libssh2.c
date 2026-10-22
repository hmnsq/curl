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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __VMS
#ਅਨ੍ਤਰ੍ਭਾਵ <in.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <inet.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/ssh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h" /* for the week day and month names */
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/vssh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h" /* for curlx_base64_encode() */

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *sftp_libssh2_strerror(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ err)
{
  ਚੋਣ(err) {
  ਹਾਲਤ LIBSSH2_FX_NO_SUCH_FILE:
    ਵਾਪਸ "No such file or directory";

  ਹਾਲਤ LIBSSH2_FX_PERMISSION_DENIED:
    ਵਾਪਸ "Permission denied";

  ਹਾਲਤ LIBSSH2_FX_FAILURE:
    ਵਾਪਸ "Operation failed";

  ਹਾਲਤ LIBSSH2_FX_BAD_ਸੁਨੇਹਾ:
    ਵਾਪਸ "Bad message from SFTP server";

  ਹਾਲਤ LIBSSH2_FX_NO_CONNECTION:
    ਵਾਪਸ "Not connected to SFTP server";

  ਹਾਲਤ LIBSSH2_FX_CONNECTION_ਗੁਆਚਾ:
    ਵਾਪਸ "Connection to SFTP server lost";

  ਹਾਲਤ LIBSSH2_FX_OP_UNSUPPORTED:
    ਵਾਪਸ "Operation not supported by SFTP server";

  ਹਾਲਤ LIBSSH2_FX_INVALID_HANDLE:
    ਵਾਪਸ "Invalid handle";

  ਹਾਲਤ LIBSSH2_FX_NO_SUCH_ਰਸਤਾ:
    ਵਾਪਸ "No such file or directory";

  ਹਾਲਤ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ਵਾਪਸ "File already exists";

  ਹਾਲਤ LIBSSH2_FX_WRITE_PROTECT:
    ਵਾਪਸ "File is write protected";

  ਹਾਲਤ LIBSSH2_FX_NO_MEDIA:
    ਵਾਪਸ "No media";

  ਹਾਲਤ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    ਵਾਪਸ "Disk full";

  ਹਾਲਤ LIBSSH2_FX_QUOTA_EXCEEDED:
    ਵਾਪਸ "User quota exceeded";

  ਹਾਲਤ LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    ਵਾਪਸ "Unknown principal";

  ਹਾਲਤ LIBSSH2_FX_ਤਾਲਾ_CONFLICT:
    ਵਾਪਸ "File lock conflict";

  ਹਾਲਤ LIBSSH2_FX_DIR_NOT_ਖਾਲੀ_ਥਾਵਾਂ:
    ਵਾਪਸ "Directory not empty";

  ਹਾਲਤ LIBSSH2_FX_NOT_A_DIRECTORY:
    ਵਾਪਸ "Not a directory";

  ਹਾਲਤ LIBSSH2_FX_INVALID_FILENAME:
    ਵਾਪਸ "Invalid filename";

  ਹਾਲਤ LIBSSH2_FX_LINK_LOOP:
    ਵਾਪਸ "Link points to itself";
  }
  ਵਾਪਸ "Unknown error in libssh2";
}

ਸਥਿਰ ਖਾਲੀ kbd_callback(ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ name_len,
                         ਅਚਲ ਅੱਖਰ *instruction, ਪੂਰਨ instruction_len,
                         ਪੂਰਨ ਅੰਕ_prompts,
                         ਅਚਲ LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         ਖਾਲੀ **abstract)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = (ਬਣਤਰ Curl_easy *)*abstract;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", ਅੰਕ_prompts);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)name;
  (ਖਾਲੀ)name_len;
  (ਖਾਲੀ)instruction;
  (ਖਾਲੀ)instruction_len;
#ਜੇ_ਅੰਤ /* CURL_LIBSSH2_DEBUG */
  ਜੇ(ਅੰਕ_prompts == 1) {
    ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
    ਅਚਲ ਅੱਖਰ *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].ਪਾਠ_3 = Curl_cstrdup(passwd);
    responses[0].ਲੰਮਾਈ = responses[0].ਪਾਠ_3 ? curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(passwd)) : 0;
  }
  (ਖਾਲੀ)prompts;
} /* kbd_callback */

ਸਥਿਰ CURLcode sftp_libssh2_error_ਮੰਜ਼ਿਲ_CURLE(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ err)
{
  ਚੋਣ(err) {
  ਹਾਲਤ LIBSSH2_FX_OK:
    ਵਾਪਸ CURLE_OK;

  ਹਾਲਤ LIBSSH2_FX_NO_SUCH_FILE:
  ਹਾਲਤ LIBSSH2_FX_NO_SUCH_ਰਸਤਾ:
    ਵਾਪਸ CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ;

  ਹਾਲਤ LIBSSH2_FX_PERMISSION_DENIED:
  ਹਾਲਤ LIBSSH2_FX_WRITE_PROTECT:
  ਹਾਲਤ LIBSSH2_FX_ਤਾਲਾ_CONFLICT:
    ਵਾਪਸ CURLE_REMOTE_ACCESS_DENIED;

  ਹਾਲਤ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  ਹਾਲਤ LIBSSH2_FX_QUOTA_EXCEEDED:
    ਵਾਪਸ CURLE_REMOTE_DISK_ਭਰਿਆ;

  ਹਾਲਤ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ਵਾਪਸ CURLE_REMOTE_FILE_EXISTS;

  ਹਾਲਤ LIBSSH2_FX_DIR_NOT_ਖਾਲੀ_ਥਾਵਾਂ:
    ਵਾਪਸ CURLE_QUOTE_ERROR;

  ਮੂਲ:
    ਰੋਕੋ;
  }

  ਵਾਪਸ CURLE_SSH;
}

ਸਥਿਰ CURLcode libssh2_session_error_ਮੰਜ਼ਿਲ_CURLE(ਪੂਰਨ err)
{
  ਚੋਣ(err) {
  /* Ordered by order of appearance in libssh2.h */
  ਹਾਲਤ LIBSSH2_ERROR_NONE:
    ਵਾਪਸ CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  ਹਾਲਤ LIBSSH2_ERROR_SCP_PROTOCOL:
    ਵਾਪਸ CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ;

  ਹਾਲਤ LIBSSH2_ERROR_SOCKET_NONE:
    ਵਾਪਸ CURLE_COULDNT_CONNECT;

  ਹਾਲਤ LIBSSH2_ERROR_ALLOC:
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਹਾਲਤ LIBSSH2_ERROR_SOCKET_SEND:
    ਵਾਪਸ CURLE_SEND_ERROR;

  ਹਾਲਤ LIBSSH2_ERROR_HOSTKEY_INIT:
  ਹਾਲਤ LIBSSH2_ERROR_HOSTKEY_SIGN:
  ਹਾਲਤ LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  ਹਾਲਤ LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;

  ਹਾਲਤ LIBSSH2_ERROR_PASSWORD_EXPIRED:
    ਵਾਪਸ CURLE_LOGIN_DENIED;

  ਹਾਲਤ LIBSSH2_ERROR_SOCKET_TIMEOUT:
  ਹਾਲਤ LIBSSH2_ERROR_TIMEOUT:
    ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;

  ਹਾਲਤ LIBSSH2_ERROR_EAGAIN:
    ਵਾਪਸ CURLE_AGAIN;
  }

  ਵਾਪਸ CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

ਸਥਿਰ LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (ਖਾਲੀ)abstract;
  ਵਾਪਸ Curl_cmalloc(ਗਿਣਤੀ_2);
}

ਸਥਿਰ LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (ਖਾਲੀ)abstract;
  ਵਾਪਸ Curl_crealloc(ptr, ਗਿਣਤੀ_2);
}

ਸਥਿਰ LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (ਖਾਲੀ)abstract;
  ਜੇ(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

ਸਥਿਰ ਪੂਰਨ sshkeycallback(CURL *easy,
                          ਅਚਲ ਬਣਤਰ curl_khkey *knownkey, /*ਗਿਆਤ*/
                          ਅਚਲ ਬਣਤਰ curl_khkey *foundkey, /* found */
                          ਗਿਣਤੀ curl_khmatch ਮੇਲ,
                          ਖਾਲੀ *clientp)
{
  (ਖਾਲੀ)easy;
  (ਖਾਲੀ)knownkey;
  (ਖਾਲੀ)foundkey;
  (ਖਾਲੀ)clientp;

  /* we only allow perfect matches, and we reject everything else */
  ਵਾਪਸ (ਮੇਲ != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ਸਥਿਰ ਗਿਣਤੀ curl_khtype convert_ssh2_keytype(ਪੂਰਨ sshkeytype)
{
  ਗਿਣਤੀ curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  ਚੋਣ(sshkeytype) {
  ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBSSH2_HOSTKEY_TYPE_DSS
  ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBSSH2_HOSTKEY_TYPE_ED25519
  ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  }
  ਵਾਪਸ keytype;
}

ਸਥਿਰ CURLcode ssh_knownhost(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ssh_conn *sshc)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ libssh2_knownhost *host = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *remotekey = ਨਿਰਾ;
  ਪੂਰਨ keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  ਪੂਰਨ keybit = 0;
  ਪੂਰਨ sshkeytype = 0;
  size_t keylen = 0;
  ਪੂਰਨ rc = 0;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS)) {
    infof(ਅੰਕੜੇ, "SSH: no knownhosts file configured");
    ਵਾਪਸ CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  ਜੇ(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    ਬਣਤਰ Curl_mapi_guard guard;
    ਗਿਣਤੀ curl_khmatch keymatch;
    curl_sshkeycallback func =
      ਅੰਕੜੇ->ਸੈੱਟ.ssh_keyfunc ? ਅੰਕੜੇ->ਸੈੱਟ.ssh_keyfunc : sshkeycallback;
    ਬਣਤਰ curl_khkey knownkey;
    ਬਣਤਰ curl_khkey *knownkeyp = ਨਿਰਾ;
    ਬਣਤਰ curl_khkey foundkey;

    ਚੋਣ(sshkeytype) {
    ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_SSHRSA;
      ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBSSH2_HOSTKEY_TYPE_DSS
    ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_SSHDSS;
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ECDSA_256;
      ਰੋਕੋ;
    ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ECDSA_384;
      ਰੋਕੋ;
    ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ECDSA_521;
      ਰੋਕੋ;
    ਹਾਲਤ LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ED25519;
      ਰੋਕੋ;
    ਮੂਲ:
      infof(ਅੰਕੜੇ, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      ਰੋਕੋ;
    }
    ਜੇ(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    ਨਹੀਂਤਾਂ {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->ਪੋਰਟ != ਪੋਰਟ_SSH) ?
                                          conn->origin->ਪੋਰਟ : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(ਅੰਕੜੇ, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->ਕੁੰਜੀ : "<none>");

      /* setup 'knownkey' */
      ਜੇ(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.ਕੁੰਜੀ = host->ਕੁੰਜੀ;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.ਕੁੰਜੀ = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (ਗਿਣਤੀ curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_ssh_keyfunc);
      rc = func(ਅੰਕੜੇ, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, ਅੰਕੜੇ->ਸੈੱਟ.ssh_keyfunc_userp);
      CURL_CBAPI_ਸਿਰਾ(&guard);
    }
  }
  ਨਹੀਂਤਾਂ {
    /* no remotekey means failure! */
    infof(ਅੰਕੜੇ, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  ਚੋਣ(rc) {
  ਮੂਲ: /* unknown return codes is the same as reject */
  ਹਾਲਤ CURLKHSTAT_REJECT:
    infof(ਅੰਕੜੇ, "SSH: knownhost check failed");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  ਹਾਲਤ CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    ਰੋਕੋ;
  ਹਾਲਤ CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    ਜੇ(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  ਹਾਲਤ CURLKHSTAT_FINE:
  ਹਾਲਤ CURLKHSTAT_FINE_ਜੋੜੋ_ਮੰਜ਼ਿਲ_FILE:
    /* proceed */
    ਜੇ(keycheck != LIBSSH2_KNOWNHOST_CHECK_ਮੇਲ) {
      ਪੂਰਨ addrc;
      ਅਚਲ ਅੱਖਰ *hostbuf;
      ਅੱਖਰ *hostport = ਨਿਰਾ;
      ਜੇ(conn->origin->ਪੋਰਟ != ਪੋਰਟ_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->ਪੋਰਟ);
        ਜੇ(!hostbuf)
          infof(ਅੰਕੜੇ, "WARNING: failed allocating buffer for [host]:port");
      }
      ਨਹੀਂਤਾਂ
        hostbuf = conn->origin->hostname;
      ਜੇ(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, ਨਿਰਾ,
                                       remotekey, keylen, ਨਿਰਾ, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, ਨਿਰਾ);
        ਜੇ(addrc)
          infof(ਅੰਕੜੇ, "WARNING: adding the known host %s failed", hostbuf);
        ਨਹੀਂਤਾਂ ਜੇ(rc == CURLKHSTAT_FINE_ਜੋੜੋ_ਮੰਜ਼ਿਲ_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          ਪੂਰਨ wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          ਜੇ(wrc) {
            infof(ਅੰਕੜੇ, "WARNING: writing %s failed",
                  CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "SSH: knownhost entry matches host key");
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_check_fingerprint(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ssh_conn *sshc)
{
  ਅਚਲ ਅੱਖਰ *pubkey_md5 =
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_MD5);
  ਅਚਲ ਅੱਖਰ *pubkey_sha256 =
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_SHA256);

  ਜੇ(pubkey_sha256) {
    ਅਚਲ ਅੱਖਰ *fingerprint = ਨਿਰਾ;
    ਅੱਖਰ *fingerprint_b64 = ਨਿਰਾ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(ਅੰਕੜੇ, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    ਜੇ(!fingerprint) {
      failf(ਅੰਕੜੇ,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
      ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    ਜੇ(curlx_base64_encode((ਅਚਲ uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
      ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
    }

    ਜੇ(!fingerprint_b64) {
      failf(ਅੰਕੜੇ, "SHA256 fingerprint could not be encoded");
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
      ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(ਅੰਕੜੇ, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    ਜਦੋਂ((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    ਜਦੋਂ((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    ਜੇ((pub_pos != b64_pos) ||
       ਲੜੀ_ਤੁਲਨਾ(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(ਅੰਕੜੇ,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
      ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(ਅੰਕੜੇ, "SSH: SHA256 checksum match");
  }

  ਜੇ(pubkey_md5) {
    ਅੱਖਰ md5buffer[33];
    ਅਚਲ ਅੱਖਰ *fingerprint;

    infof(ਅੰਕੜੇ, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    ਜੇ(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      ਪੂਰਨ i;
      ਲਈ(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)fingerprint[i]);
      }

      infof(ਅੰਕੜੇ, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    ਜੇ(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      ਜੇ(fingerprint) {
        failf(ਅੰਕੜੇ,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      ਨਹੀਂਤਾਂ {
        failf(ਅੰਕੜੇ,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
      ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(ਅੰਕੜੇ, "SSH: MD5 checksum match");
  }

  ਜੇ(!pubkey_md5 && !pubkey_sha256) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssh_hostkeyfunc) {
      size_t keylen = 0;
      ਪੂਰਨ sshkeytype = 0;
      ਪੂਰਨ rc = 0;
      /* we handle the process to the callback */
      ਅਚਲ ਅੱਖਰ *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      ਜੇ(remotekey) {
        ਬਣਤਰ Curl_mapi_guard guard;
        ਗਿਣਤੀ curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_ssh_hostkeyfunc);
        rc = ਅੰਕੜੇ->ਸੈੱਟ.ssh_hostkeyfunc(ਅੰਕੜੇ->ਸੈੱਟ.ssh_hostkeyfunc_userp,
                                       (ਪੂਰਨ)keytype, remotekey, keylen);
        CURL_CBAPI_ਸਿਰਾ(&guard);
        ਜੇ(rc != CURLKHMATCH_OK) {
          myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
          failf(ਅੰਕੜੇ, "SSH: callback failed host public key verification");
          ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(ਅੰਕੜੇ, "SSH: verified public key via callback");
      }
      ਨਹੀਂਤਾਂ {
        myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
        ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
      }
      ਵਾਪਸ CURLE_OK;
    }
    ਨਹੀਂਤਾਂ {
      CURL_TRC_SSH(ਅੰਕੜੇ, "no host key checksum given, checking knownhosts");
      ਵਾਪਸ ssh_knownhost(ਅੰਕੜੇ, sshc);
    }
  }
  ਨਹੀਂਤਾਂ {
    /* as we already matched, we skip the check for known hosts */
    ਵਾਪਸ CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
ਸਥਿਰ CURLcode ssh_force_knownhost_ਕੁੰਜੀ_type(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ ssh_conn *sshc)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਸਥਿਰ ਅਚਲ ਅੱਖਰ hostkey_ਵਿਧੀ_ssh_ed25519[] = "ssh-ed25519";
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ hostkey_ਵਿਧੀ_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ hostkey_ਵਿਧੀ_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ hostkey_ਵਿਧੀ_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ hostkey_ਵਿਧੀ_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_SSHDSS
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ hostkey_ਵਿਧੀ_ssh_dss[] = "ssh-dss";
#ਜੇ_ਅੰਤ
  bool ਮਿਲਿਆ = FALSE;

  ਜੇ(sshc->kh &&
     !CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_MD5) &&
     !CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_SHA256)) {
    ਬਣਤਰ libssh2_knownhost *store = ਨਿਰਾ;
    ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ਨਕਾਬ >>
                      LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ਖਿਸਕਾਓ) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    ਜਦੋਂ(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      ਜੇ(store) {
        ਜੇ(store->name) {
          ਜੇ(store->name[0] == '[') {
            curl_off_t ਪੋਰਟ;
            size_t kh_name_ਮਾਪ = 0;
            ਅਚਲ ਅੱਖਰ *p;
            ਅਚਲ ਅੱਖਰ *kh_name_ਸਿਰਾ = ਲੜੀ_ਖੋਜ(store->name, "]:");
            ਜੇ(!kh_name_ਸਿਰਾ) {
              infof(ਅੰਕੜੇ, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS));
              ਜਾਰੀ;
            }
            p = kh_name_ਸਿਰਾ + 2; /* start of port number */
            ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &ਪੋਰਟ, 0xffff) &&
               (kh_name_ਸਿਰਾ && (ਪੋਰਟ == conn->origin->ਪੋਰਟ))) {
              kh_name_ਮਾਪ = ਲੜੀ_ਲੰਬਾਈ(store->name) - 1 - ਲੜੀ_ਲੰਬਾਈ(kh_name_ਸਿਰਾ);
              ਜੇ((kh_name_ਮਾਪ == ਲੜੀ_ਲੰਬਾਈ(conn->origin->hostname)) &&
                 !ਲੜੀ_ਤੁਲਨਾ(store->name + 1, conn->origin->hostname,
                          kh_name_ਮਾਪ)) {
                ਮਿਲਿਆ = TRUE;
                ਰੋਕੋ;
              }
            }
          }
          ਨਹੀਂਤਾਂ ਜੇ(!strcmp(store->name, conn->origin->hostname)) {
            ਮਿਲਿਆ = TRUE;
            ਰੋਕੋ;
          }
        }
        ਨਹੀਂਤਾਂ ਜੇ(store->ਕੁੰਜੀ) {
          ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type = (store->typemask & LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ਨਕਾਬ)
                              >> LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ਖਿਸਕਾਓ;
          ਜੇ(!absent_type[type]) {
            ਪੂਰਨ keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->ਪੋਰਟ != ਪੋਰਟ_SSH) ? conn->origin->ਪੋਰਟ : -1,
              store->ਕੁੰਜੀ, ਲੜੀ_ਲੰਬਾਈ(store->ਕੁੰਜੀ),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ਨਕਾਬ),
              ਨਿਰਾ);
            ਜੇ(keycheck == LIBSSH2_KNOWNHOST_CHECK_ਮੇਲ) {
              ਮਿਲਿਆ = TRUE;
              ਰੋਕੋ;
            }
            ਨਹੀਂਤਾਂ ਜੇ(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    ਜੇ(ਮਿਲਿਆ) {
      ਪੂਰਨ rc;
      ਅਚਲ ਅੱਖਰ *hostkey_ਵਿਧੀ = ਨਿਰਾ;
      infof(ਅੰਕੜੇ, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS));

      ਚੋਣ(store->typemask & LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ਨਕਾਬ) {
      ਹਾਲਤ LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ED25519:
        hostkey_ਵਿਧੀ = hostkey_ਵਿਧੀ_ssh_ed25519;
        ਰੋਕੋ;
      ਹਾਲਤ LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ECDSA_521:
        hostkey_ਵਿਧੀ = hostkey_ਵਿਧੀ_ssh_ecdsa_521;
        ਰੋਕੋ;
      ਹਾਲਤ LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ECDSA_384:
        hostkey_ਵਿਧੀ = hostkey_ਵਿਧੀ_ssh_ecdsa_384;
        ਰੋਕੋ;
      ਹਾਲਤ LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ECDSA_256:
        hostkey_ਵਿਧੀ = hostkey_ਵਿਧੀ_ssh_ecdsa_256;
        ਰੋਕੋ;
      ਹਾਲਤ LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_SSHRSA:
        hostkey_ਵਿਧੀ = hostkey_ਵਿਧੀ_ssh_rsa_all;
        ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_SSHDSS
      ਹਾਲਤ LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_SSHDSS:  /* deprecated upstream */
        hostkey_ਵਿਧੀ = hostkey_ਵਿਧੀ_ssh_dss;
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
      ਹਾਲਤ LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_RSA1:
        failf(ਅੰਕੜੇ, "Found host key type RSA1 which is not supported");
        ਵਾਪਸ CURLE_SSH;
      ਮੂਲ:
        failf(ਅੰਕੜੇ, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_ਕੁੰਜੀ_ਨਕਾਬ));
        ਵਾਪਸ CURLE_SSH;
      }

      infof(ਅੰਕੜੇ, "SSH: set '%s' as hostkey type", hostkey_ਵਿਧੀ);
      rc = libssh2_session_ਵਿਧੀ_pref(sshc->ssh_session,
                                       LIBSSH2_ਵਿਧੀ_HOSTKEY, hostkey_ਵਿਧੀ);
      ਜੇ(rc) {
        ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
        ਪੂਰਨ errlen;
        libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, &errlen, 0);
        failf(ਅੰਕੜੇ, "libssh2 method '%s' failed: %s", hostkey_ਵਿਧੀ, err_ਸੁਨੇਹਾ);
        ਨਤੀਜਾ = libssh2_session_error_ਮੰਜ਼ਿਲ_CURLE(rc);
      }
    }
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS));
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode quote_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ssh_conn *sshc)
{
  failf(ਅੰਕੜੇ, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ਵਾਪਸ CURLE_QUOTE_ERROR;
}

ਸਥਿਰ CURLcode sftp_quote(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ ssh_conn *sshc,
                           ਬਣਤਰ SSHPROTO *sshp)
{
  ਅਚਲ ਅੱਖਰ *cp;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ਅਚਲ ਅੱਖਰ *cmd = sshc->quote_ਵਸਤੂ->ਅੰਕੜੇ;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ਜੇ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ਜੇ(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    ਅੱਖਰ *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->ਰਸਤਾ);
    ਜੇ(!tmp)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ, "PWD\n", 4);
    Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, tmp, ਲੜੀ_ਲੰਬਾਈ(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, tmp, ਲੜੀ_ਲੰਬਾਈ(tmp));
    curlx_free(tmp);
    ਜੇ(!ਨਤੀਜਾ)
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ਜੇ(!cp) {
    failf(ਅੰਕੜੇ, "Syntax error command '%s', missing parameter", cmd);
    ਵਾਪਸ CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  ਨਤੀਜਾ = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ != CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
      failf(ਅੰਕੜੇ, "Syntax error: Bad first parameter to '%s'", cmd);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "chgrp ", 6) ||
     !ਲੜੀ_ਤੁਲਨਾ(cmd, "chmod ", 6) ||
     !ਲੜੀ_ਤੁਲਨਾ(cmd, "chown ", 6) ||
     !ਲੜੀ_ਤੁਲਨਾ(cmd, "atime ", 6) ||
     !ਲੜੀ_ਤੁਲਨਾ(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    ਨਤੀਜਾ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ != CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
        failf(ਅੰਕੜੇ, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);

    memset(&sshp->quote_attrs, 0, ਆਕਾਰ(LIBSSH2_SFTP_ਗੁਣ));
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_STAT);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "ln ", 3) ||
     !ਲੜੀ_ਤੁਲਨਾ(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    ਨਤੀਜਾ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ != CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
        failf(ਅੰਕੜੇ, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "mkdir ", 6)) {
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    /* create directory */
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_MKDIR);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    ਨਤੀਜਾ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ != CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
        failf(ਅੰਕੜੇ, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_RENAME);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "rmdir ", 6)) {
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    /* delete directory */
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_RMDIR);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "rm ", 3)) {
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_UNLINK);
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "statvfs ", 8)) {
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_STATVFS);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  failf(ਅੰਕੜੇ, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ਵਾਪਸ CURLE_QUOTE_ERROR;
}

ਸਥਿਰ CURLcode sftp_upload_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ssh_conn *sshc,
                                 ਬਣਤਰ SSHPROTO *sshp,
                                 bool *blockp)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  ਜੇ(ਅੰਕੜੇ->state.resume_from) {
    LIBSSH2_SFTP_ਗੁਣ attrs;
    ਜੇ(ਅੰਕੜੇ->state.resume_from < 0) {
      ਪੂਰਨ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->ਰਸਤਾ,
                                    curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      ਜੇ(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        ਵਾਪਸ CURLE_OK;
      }
      ਜੇ(rc) {
        ਅੰਕੜੇ->state.resume_from = 0;
      }
      ਨਹੀਂਤਾਂ {
        curl_off_t ਮਾਪ = attrs.filesize;
        ਜੇ(ਮਾਪ < 0) {
          failf(ਅੰਕੜੇ, "Bad file size (%" FMT_OFF_T ")", ਮਾਪ);
          ਵਾਪਸ CURLE_BAD_DOWNLOAD_RESUME;
        }
        ਅੰਕੜੇ->state.resume_from = attrs.filesize;
      }
    }
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  ਨਹੀਂਤਾਂ {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->ਰਸਤਾ,
                         curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ)),
                         flags, (ਲੰਮਾ)ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  ਜੇ(!sshc->sftp_handle) {
    CURLcode ਨਤੀਜਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr;
    ਪੂਰਨ rc = libssh2_session_ਆਖਰੀ_errno(sshc->ssh_session);

    ਜੇ(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      ਵਾਪਸ CURLE_OK;
    }

    ਜੇ(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    ਨਹੀਂਤਾਂ
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    ਜੇ(sshc->ਦੂਜਾ_Create_Dirs) {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      failf(ਅੰਕੜੇ, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      ਵਾਪਸ sftp_libssh2_error_ਮੰਜ਼ਿਲ_CURLE(sftperr);
    }
    ਜੇ((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_ਰਸਤਾ) &&
       ਅੰਕੜੇ->ਸੈੱਟ.ftp_create_missing_dirs &&
       (ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ) > 1)) {
      /* try to create the path remotely */
      sshc->ਦੂਜਾ_Create_Dirs = 1;
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ਵਾਪਸ CURLE_OK;
    }
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    ਨਤੀਜਾ = sftp_libssh2_error_ਮੰਜ਼ਿਲ_CURLE(sftperr);
    ਜੇ(!ਨਤੀਜਾ) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      ਨਤੀਜਾ = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(ਅੰਕੜੇ, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  ਜੇ(ਅੰਕੜੇ->state.resume_from > 0 && !ਅੰਕੜੇ->ਸੈੱਟ.remote_append) {
    ਪੂਰਨ seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.seek_func) {
      ਬਣਤਰ Curl_mapi_guard guard;
      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_seek_func);
      seekerr = ਅੰਕੜੇ->ਸੈੱਟ.seek_func(ਅੰਕੜੇ->ਸੈੱਟ.seek_client,
                                    ਅੰਕੜੇ->state.resume_from, SEEK_SET);
      CURL_CBAPI_ਸਿਰਾ(&guard);
    }

    ਜੇ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      ਜੇ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(ਅੰਕੜੇ, "Could not seek stream");
        ਵਾਪਸ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ਕਰੋ {
        ਬਣਤਰ Curl_mapi_guard guard;
        ਅੱਖਰ scratch[4 * 1024];
        size_t readthisamountnow =
          (ਅੰਕੜੇ->state.resume_from - passed >
           (curl_off_t)ਆਕਾਰ(scratch)) ?
          ਆਕਾਰ(scratch) : curlx_sotouz(ਅੰਕੜੇ->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fread_func);
        actuallyread = ਅੰਕੜੇ->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              ਅੰਕੜੇ->state.ਵਿੱਚ);
        CURL_CBAPI_ਸਿਰਾ(&guard);

        passed += actuallyread;
        ਜੇ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(ਅੰਕੜੇ, "Failed to read data");
          ਵਾਪਸ CURLE_FTP_COULDNT_USE_REST;
        }
      } ਜਦੋਂ(passed < ਅੰਕੜੇ->state.resume_from);
    }

    /* now, decrease the size of the read */
    ਜੇ(ਅੰਕੜੇ->state.infilesize > 0) {
      ਅੰਕੜੇ->state.infilesize -= ਅੰਕੜੇ->state.resume_from;
      ਅੰਕੜੇ->req.ਮਾਪ = ਅੰਕੜੇ->state.infilesize;
      Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)ਅੰਕੜੇ->state.resume_from);
  }
  ਜੇ(ਅੰਕੜੇ->state.infilesize > 0) {
    ਅੰਕੜੇ->req.ਮਾਪ = ਅੰਕੜੇ->state.infilesize;
    Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);
  }
  /* upload data */
  Curl_xfer_ਤਿਆਰੀ_send(ਅੰਕੜੇ, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ਅੰਕੜੇ->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(ਅੰਕੜੇ);

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ ssh_state_pkey_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     ਲੜੀ_ਖੋਜ(sshc->authlist, "publickey")) {
    ਜੇ(sshc->pub_ਕੁੰਜੀ)
      infof(ਅੰਕੜੇ, "SSH: trying public key file '%s'", sshc->pub_ਕੁੰਜੀ);
    infof(ਅੰਕੜੇ, "SSH: trying private key file '%s'", sshc->priv_ਕੁੰਜੀ);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PKEY);
  }
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PASS_INIT);
}

ਸਥਿਰ CURLcode sftp_quote_stat(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ssh_conn *sshc,
                                ਬਣਤਰ SSHPROTO *sshp,
                                bool *blockp)
{
  ਅਚਲ ਅੱਖਰ *cmd = sshc->quote_ਵਸਤੂ->ਅੰਕੜੇ;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  ਜੇ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ਜੇ(ਲੜੀ_ਤੁਲਨਾ(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    ਪੂਰਨ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      ਵਾਪਸ CURLE_OK;
    }
    ਜੇ(rc && !sshc->acceptfail) { /* get those attributes */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
      failf(ਅੰਕੜੇ, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      ਜਾਓ fail;
    }
  }

  /* Now set the new attributes... */
  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "chgrp", 5)) {
    ਅਚਲ ਅੱਖਰ *p = sshc->quote_path1;
    curl_off_t gid;
    ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!sshc->acceptfail) {
      failf(ਅੰਕੜੇ, "Syntax error: chgrp gid not a number");
      ਜਾਓ fail;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "chmod", 5)) {
    curl_off_t perms;
    ਅਚਲ ਅੱਖਰ *p = sshc->quote_path1;
    /* permissions are octal */
    ਜੇ(curlx_str_octal(&p, &perms, 07777)) {
      failf(ਅੰਕੜੇ, "Syntax error: chmod permissions not a number");
      ਜਾਓ fail;
    }

    sshp->quote_attrs.permissions = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "chown", 5)) {
    ਅਚਲ ਅੱਖਰ *p = sshc->quote_path1;
    curl_off_t uid;
    ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!sshc->acceptfail) {
      failf(ਅੰਕੜੇ, "Syntax error: chown uid not a number");
      ਜਾਓ fail;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "atime", 5) ||
          !ਲੜੀ_ਤੁਲਨਾ(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    ਜੇ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(ਅੰਕੜੇ, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ਜੇ SIZEOF_ਸਮਾਂ_T > SIZEOF_LONG
    ਜੇ(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(ਅੰਕੜੇ, "date overflow");
      fail = TRUE;
    }
#ਜੇ_ਅੰਤ
    ਜੇ(fail)
      ਜਾਓ fail;
    ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "atime", 5))
      sshp->quote_attrs.atime = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)date;
    ਨਹੀਂਤਾਂ /* mtime */
      sshp->quote_attrs.mtime = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ਵਾਪਸ CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ਵਾਪਸ CURLE_QUOTE_ERROR;
}

ਸਥਿਰ CURLcode sftp_download_stat(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ssh_conn *sshc,
                                   ਬਣਤਰ SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ਗੁਣ attrs;
  ਪੂਰਨ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->ਰਸਤਾ,
                                curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ)),
                                LIBSSH2_SFTP_STAT, &attrs);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ਵਾਪਸ CURLE_OK;
  }
  ਜੇ(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_ਮਾਪ) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    ਅੰਕੜੇ->req.ਮਾਪ = -1;
    ਅੰਕੜੇ->req.maxdownload = -1;
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  ਨਹੀਂਤਾਂ {
    curl_off_t ਮਾਪ = attrs.filesize;

    ਜੇ(ਮਾਪ < 0) {
      failf(ਅੰਕੜੇ, "Bad file size (%" FMT_OFF_T ")", ਮਾਪ);
      ਵਾਪਸ CURLE_BAD_DOWNLOAD_RESUME;
    }
    ਜੇ(ਅੰਕੜੇ->state.use_range) {
      curl_off_t from;
      CURLcode ਨਤੀਜਾ = Curl_ssh_range(ਅੰਕੜੇ, ਅੰਕੜੇ->state.range, ਮਾਪ,
                                       &from, &ਮਾਪ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    ਅੰਕੜੇ->req.ਮਾਪ = ਮਾਪ;
    ਅੰਕੜੇ->req.maxdownload = ਮਾਪ;
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, ਮਾਪ);
  }

  /* We can resume if we can seek to the resume position */
  ਜੇ(ਅੰਕੜੇ->state.resume_from) {
    ਜੇ(ਅੰਕੜੇ->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ਜੇ((curl_off_t)attrs.filesize < -ਅੰਕੜੇ->state.resume_from) {
        failf(ਅੰਕੜੇ, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              ਅੰਕੜੇ->state.resume_from, (curl_off_t)attrs.filesize);
        ਵਾਪਸ CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      ਅੰਕੜੇ->state.resume_from += attrs.filesize;
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ((curl_off_t)attrs.filesize < ਅੰਕੜੇ->state.resume_from) {
        failf(ਅੰਕੜੇ, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              ਅੰਕੜੇ->state.resume_from, (curl_off_t)attrs.filesize);
        ਵਾਪਸ CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    ਅੰਕੜੇ->req.ਮਾਪ = attrs.filesize - ਅੰਕੜੇ->state.resume_from;
    ਅੰਕੜੇ->req.maxdownload = attrs.filesize - ਅੰਕੜੇ->state.resume_from;
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, attrs.filesize - ਅੰਕੜੇ->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)ਅੰਕੜੇ->state.resume_from);
  }

  /* Setup the actual download */
  ਜੇ(ਅੰਕੜੇ->req.ਮਾਪ == 0) {
    /* no data to transfer */
    Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
    infof(ਅੰਕੜੇ, "SSH: file already completely downloaded");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
    ਵਾਪਸ CURLE_OK;
  }
  Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, ਅੰਕੜੇ->req.ਮਾਪ);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ਅੰਕੜੇ->conn->send_idx = 0;

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode sftp_readdir(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ssh_conn *sshc,
                             ਬਣਤਰ SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_ਰਸਤਾ_MAX,
                                   sshp->readdir_longentry, CURL_ਰਸਤਾ_MAX,
                                   &sshp->readdir_attrs);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਜੇ(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.list_only) {
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY, "\n", 1);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&sshp->readdir, sshp->readdir_longentry);

      ਜੇ(!ਨਤੀਜਾ) {
        ਜੇ((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          ਨਤੀਜਾ = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->ਰਸਤਾ,
                                  sshp->readdir_filename);
          myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_LINK);
        }
        ਨਹੀਂਤਾਂ {
          myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(!rc) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_ਪੂਰਾ);
  }
  ਨਹੀਂਤਾਂ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    ਨਤੀਜਾ = sftperr ? sftp_libssh2_error_ਮੰਜ਼ਿਲ_CURLE(sftperr) : CURLE_SSH;
    failf(ਅੰਕੜੇ, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_ਆਖਰੀ_errno(sshc->ssh_session));
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ssh_conn *sshc)
{
  CURLcode ਨਤੀਜਾ;
  sshc->ਦੂਜਾ_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_ਸੈੱਟ_blocking(sshc->ssh_session, 0);

  ਨਤੀਜਾ = ssh_force_knownhost_ਕੁੰਜੀ_type(ਅੰਕੜੇ, sshc);
  ਜੇ(ਨਤੀਜਾ)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_S_STARTUP);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_startup(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ssh_conn *sshc)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਪੂਰਨ rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc) {
    ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
    (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
    failf(ਅੰਕੜੇ, "Failure establishing ssh session: %d, %s", rc, err_ਸੁਨੇਹਾ);

    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_HOSTKEY);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_hostkey(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode ਨਤੀਜਾ = ssh_check_fingerprint(ਅੰਕੜੇ, sshc);
  ਜੇ(!ਨਤੀਜਾ)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTHLIST);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_authlist(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ssh_conn *sshc)
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
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਅੱਖਰ *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(user)));

  ਜੇ(!sshc->authlist) {
    ਪੂਰਨ rc;
    ਜੇ(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(ਅੰਕੜੇ, "SSH: user accepted with no authentication");
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
      ਵਾਪਸ CURLE_OK;
    }
    rc = libssh2_session_ਆਖਰੀ_errno(sshc->ssh_session);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
    ਵਾਪਸ libssh2_session_error_ਮੰਜ਼ਿਲ_CURLE(rc);
  }
  infof(ਅੰਕੜੇ, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PKEY_INIT);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_auth_pkey(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਅੱਖਰ *user = Curl_creds_user(conn->creds);
  ਪੂਰਨ rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(user)),
    sshc->pub_ਕੁੰਜੀ, sshc->priv_ਕੁੰਜੀ, sshc->passphrase ? sshc->passphrase : "");
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc == 0) {
    sshc->authed = TRUE;
    infof(ਅੰਕੜੇ, "SSH: authenticated via publickey");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
  }
  ਨਹੀਂਤਾਂ {
    ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
    ਅੱਖਰ unknown[] = "Reason unknown (-1)";
    ਜੇ(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_ਸੁਨੇਹਾ = unknown;
    }
    ਨਹੀਂਤਾਂ {
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
    }
    infof(ਅੰਕੜੇ, "SSH: publickey authentication denied: %s", err_ਸੁਨੇਹਾ);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PASS_INIT);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_auth_pass_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     ਲੜੀ_ਖੋਜ(sshc->authlist, "password")) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PASS);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_HOST_INIT);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_auth_pass(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssh_conn *sshc)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਅੱਖਰ *user = Curl_creds_user(conn->creds);
  ਅਚਲ ਅੱਖਰ *passwd = Curl_creds_passwd(conn->creds);
  ਪੂਰਨ rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(user)),
                                 passwd,
                                 curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(passwd)),
                                 ਨਿਰਾ);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN) {
    ਵਾਪਸ CURLE_AGAIN;
  }
  ਜੇ(rc == 0) {
    sshc->authed = TRUE;
    infof(ਅੰਕੜੇ, "SSH: initialized password authentication");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_HOST_INIT);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_auth_host_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     ਲੜੀ_ਖੋਜ(sshc->authlist, "hostbased")) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_HOST);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_AGENT_INIT);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_auth_agent_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = 0;
  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     ਲੜੀ_ਖੋਜ(sshc->authlist, "publickey")) {

    infof(ਅੰਕੜੇ, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    ਜੇ(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      ਜੇ(!sshc->ssh_agent) {
        infof(ਅੰਕੜੇ, "SSH: could not create agent object");

        myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਕੁੰਜੀ_INIT);
        ਵਾਪਸ CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;
    ਜੇ(rc < 0) {
      infof(ਅੰਕੜੇ, "SSH: failure connecting to agent");
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਕੁੰਜੀ_INIT);
    }
    ਨਹੀਂਤਾਂ {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਕੁੰਜੀ_INIT);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_auth_agent_list(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = libssh2_agent_list_identities(sshc->ssh_agent);

  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;
  ਜੇ(rc < 0) {
    infof(ਅੰਕੜੇ, "SSH: failure requesting identities to agent");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਕੁੰਜੀ_INIT);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_auth_agent(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  ਪੂਰਨ rc = libssh2_agent_get_ਪਛਾਣ_ਤੱਤ(sshc->ssh_agent,
                                      &sshc->sshagent_ਪਛਾਣ_ਤੱਤ,
                                      sshc->sshagent_prev_ਪਛਾਣ_ਤੱਤ);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc == 0) {
    CURL_TRC_SSH(ਅੰਕੜੇ, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(ਅੰਕੜੇ->conn->creds),
                 sshc->sshagent_ਪਛਾਣ_ਤੱਤ->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(ਅੰਕੜੇ->conn->creds),
                                sshc->sshagent_ਪਛਾਣ_ਤੱਤ);

    ਜੇ(rc < 0) {
      ਜੇ(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_ਪਛਾਣ_ਤੱਤ = sshc->sshagent_ਪਛਾਣ_ਤੱਤ;
        ਵਾਪਸ CURLE_OK;
      }
      ਵਾਪਸ CURLE_AGAIN;
    }
  }

  ਜੇ(rc < 0)
    infof(ਅੰਕੜੇ, "SSH: failure requesting identities to agent");
  ਨਹੀਂਤਾਂ ਜੇ(rc == 1)
    infof(ਅੰਕੜੇ, "SSH: no agent identity would match");

  ਜੇ(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(ਅੰਕੜੇ, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(ਅੰਕੜੇ->conn->creds),
          sshc->sshagent_ਪਛਾਣ_ਤੱਤ->comment);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਕੁੰਜੀ_INIT);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_auth_ਕੁੰਜੀ_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ((ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     ਲੜੀ_ਖੋਜ(sshc->authlist, "keyboard-interactive")) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਕੁੰਜੀ);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_auth_ਕੁੰਜੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਅਚਲ ਅੱਖਰ *user = Curl_creds_user(conn->creds);
  ਪੂਰਨ rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(user)),
                                             &kbd_callback);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc == 0) {
    sshc->authed = TRUE;
    infof(ਅੰਕੜੇ, "SSH: initialized keyboard interactive authentication");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
    ਵਾਪਸ CURLE_OK;
  }
  ਵਾਪਸ CURLE_LOGIN_DENIED;
}

ਸਥਿਰ CURLcode ssh_state_auth_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssh_conn *sshc)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਜੇ(!sshc->authed) {
    failf(ਅੰਕੜੇ, "Authentication failure");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
    ਵਾਪਸ CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(ਅੰਕੜੇ, "SSH: authentication complete");

  Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_APPCONNECT); /* SSH is connected */

  ਅੰਕੜੇ->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ਜੇ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_INIT);
    ਵਾਪਸ CURLE_OK;
  }
  infof(ਅੰਕੜੇ, "SSH: connection established");
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  ਜੇ(!sshc->sftp_session) {
    ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
    ਜੇ(libssh2_session_ਆਖਰੀ_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
    failf(ਅੰਕੜੇ, "Failure initializing sftp session: %s", err_ਸੁਨੇਹਾ);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_REALPATH);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_realpath(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ssh_conn *sshc,
                                        ਬਣਤਰ SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  ਪੂਰਨ rc;

  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_ਰਸਤਾ_MAX,
                               LIBSSH2_SFTP_REALPATH);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਜੇ(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    ਜੇ(!sshc->homedir)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    curlx_free(ਅੰਕੜੇ->state.most_recent_ftp_entrypath);
    ਅੰਕੜੇ->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    ਜੇ(!ਅੰਕੜੇ->state.most_recent_ftp_entrypath)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਨਹੀਂਤਾਂ {
    /* Return the error type */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    CURLcode ਨਤੀਜਾ;
    ਜੇ(sftperr)
      ਨਤੀਜਾ = sftp_libssh2_error_ਮੰਜ਼ਿਲ_CURLE(sftperr);
    ਨਹੀਂਤਾਂ
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      ਨਤੀਜਾ = CURLE_SSH;
    CURL_TRC_SSH(ਅੰਕੜੇ, "error = %lu makes libcurl = %d", sftperr, (ਪੂਰਨ)ਨਤੀਜਾ);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(ਅੰਕੜੇ, "CONNECT phase done");
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ ssh_conn *sshc,
                                          ਬਣਤਰ SSHPROTO *sshp)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = Curl_getworkingpath(ਅੰਕੜੇ, sshc->homedir, &sshp->ਰਸਤਾ);
  ਜੇ(ਨਤੀਜਾ) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.quote) {
    infof(ਅੰਕੜੇ, "SSH: sending quote commands");
    sshc->quote_ਵਸਤੂ = ਅੰਕੜੇ->ਸੈੱਟ.quote;
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_GETINFO);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_postquote_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.postquote) {
    infof(ਅੰਕੜੇ, "SSH: sending quote commands");
    sshc->quote_ਵਸਤੂ = ਅੰਕੜੇ->ਸੈੱਟ.postquote;
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ssh_conn *sshc,
                                     ਬਣਤਰ SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = sftp_quote(ਅੰਕੜੇ, sshc, sshp);
  ਜੇ(ਨਤੀਜਾ) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_sftp_ਅਗਲਾ_quote(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_ਵਸਤੂ = sshc->quote_ਵਸਤੂ->ਅਗਲਾ;

  ਜੇ(sshc->quote_ਵਸਤੂ) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(sshc->nextstate != SSH_NO_STATE) {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    ਨਹੀਂਤਾਂ {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_GETINFO);
    }
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote_stat(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ ssh_conn *sshc,
                                          ਬਣਤਰ SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = sftp_quote_stat(ਅੰਕੜੇ, sshc, sshp, blockp);
  ਜੇ(ਨਤੀਜਾ) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote_setstat(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ ssh_conn *sshc,
                                             ਬਣਤਰ SSHPROTO *sshp)
{
  ਪੂਰਨ rc;

  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc && !sshc->acceptfail) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    failf(ਅੰਕੜੇ, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ਵਾਪਸ CURLE_QUOTE_ERROR;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote_symlink(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc && !sshc->acceptfail) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    failf(ਅੰਕੜੇ, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ਵਾਪਸ CURLE_QUOTE_ERROR;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote_mkdir(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path1)),
                                 (ਲੰਮਾ)ਅੰਕੜੇ->ਸੈੱਟ.new_directory_perms);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc && !sshc->acceptfail) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    failf(ਅੰਕੜੇ, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ਵਾਪਸ CURLE_QUOTE_ERROR;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote_rename(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc && !sshc->acceptfail) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    failf(ਅੰਕੜੇ, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ਵਾਪਸ CURLE_QUOTE_ERROR;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote_rmdir(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path1)));
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc && !sshc->acceptfail) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    failf(ਅੰਕੜੇ, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ਵਾਪਸ CURLE_QUOTE_ERROR;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote_unlink(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path1)));
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc && !sshc->acceptfail) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    failf(ਅੰਕੜੇ, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ਵਾਪਸ CURLE_QUOTE_ERROR;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_quote_statvfs(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  ਪੂਰਨ rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshc->quote_path1)),
                                &statvfs);

  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc && !sshc->acceptfail) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    failf(ਅੰਕੜੇ, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ਵਾਪਸ CURLE_QUOTE_ERROR;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rc == 0) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _MSC_VER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "I64u"
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "llu"
#ਜੇ_ਅੰਤ
    CURLcode ਨਤੀਜਾ;
    ਅੱਖਰ *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_ਮਾਪ_ਨਕਾਬ "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_ਖੰਡਾਂ, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    ਜੇ(!tmp) {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, tmp, ਲੜੀ_ਲੰਬਾਈ(tmp));
    curlx_free(tmp);
    ਜੇ(ਨਤੀਜਾ) {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_create_dirs_mkdir(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                 ਬਣਤਰ ssh_conn *sshc,
                                                 ਬਣਤਰ SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  ਪੂਰਨ rc;

  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->ਰਸਤਾ,
                             curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ)),
                             (ਲੰਮਾ)ਅੰਕੜੇ->ਸੈੱਟ.new_directory_perms);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ਜੇ(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    ਜੇ((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      ਵਾਪਸ sftp_libssh2_error_ਮੰਜ਼ਿਲ_CURLE(sftperr);
    }
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CREATE_DIRS);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_readdir_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ ssh_conn *sshc,
                                            ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, -1);
  ਜੇ(ਅੰਕੜੇ->req.no_body) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
    ਵਾਪਸ CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->ਰਸਤਾ,
                         curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  ਜੇ(!sshc->sftp_handle) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr;
    ਜੇ(libssh2_session_ਆਖਰੀ_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    failf(ਅੰਕੜੇ, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    ਵਾਪਸ sftp_libssh2_error_ਮੰਜ਼ਿਲ_CURLE(sftperr);
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_readdir_link(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ ssh_conn *sshc,
                                            ਬਣਤਰ SSHPROTO *sshp)
{
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ rc;
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_ਰਸਤਾ_MAX, LIBSSH2_SFTP_READLINK);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  ਜੇ(rc < 0)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* append filename and extra output */
  ਨਤੀਜਾ = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  ਜੇ(ਨਤੀਜਾ)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_BOTTOM);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_scp_download_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ ssh_conn *sshc,
                                            ਬਣਤਰ SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, ਆਕਾਰ(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->ਰਸਤਾ, &sb);
  ਜੇ(!sshc->ssh_channel) {
    ਪੂਰਨ ssh_err;
    ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;

    ਜੇ(libssh2_session_ਆਖਰੀ_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    ssh_err = libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
    failf(ਅੰਕੜੇ, "%s", err_ਸੁਨੇਹਾ);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_CHANNEL_FREE);
    ਵਾਪਸ libssh2_session_error_ਮੰਜ਼ਿਲ_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_ਮਾਪ;
  ਅੰਕੜੇ->req.maxdownload = (curl_off_t)sb.st_ਮਾਪ;
  Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ਅੰਕੜੇ->conn->send_idx = 0;

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ssh_conn *sshc,
                                     ਬਣਤਰ SSHPROTO *sshp)
{
  ਪੂਰਨ rc = 0;
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    ਜੇ(rc < 0) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to close libssh2 file: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
    sshc->sftp_handle = ਨਿਰਾ;
  }

  curlx_safefree(sshp->ਰਸਤਾ);

  CURL_TRC_SSH(ਅੰਕੜੇ, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  ਜੇ(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_shutdown(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ਪੂਰਨ rc = 0;
  ਜੇ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    ਜੇ(rc < 0) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to close libssh2 file: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
    sshc->sftp_handle = ਨਿਰਾ;
  }
  ਜੇ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    ਜੇ(rc < 0) {
      infof(ਅੰਕੜੇ, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = ਨਿਰਾ;
  }

  curlx_safefree(sshc->homedir);

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_DISCONNECT);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_download_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ ssh_conn *sshc,
                                             ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->ਰਸਤਾ,
                         curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ)),
                         LIBSSH2_FXF_READ, (ਲੰਮਾ)ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  ਜੇ(!sshc->sftp_handle) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sftperr;
    ਜੇ(libssh2_session_ਆਖਰੀ_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      ਵਾਪਸ CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_ਆਖਰੀ_error(sshc->sftp_session);
    failf(ਅੰਕੜੇ, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    ਵਾਪਸ sftp_libssh2_error_ਮੰਜ਼ਿਲ_CURLE(sftperr);
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_scp_upload_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ ssh_conn *sshc,
                                          ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->ਰਸਤਾ,
                       (ਪੂਰਨ)ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms,
                       (libssh2_int64_t)ਅੰਕੜੇ->state.infilesize, 0, 0);
  ਜੇ(!sshc->ssh_channel) {
    ਪੂਰਨ ssh_err;
    ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
    CURLcode ਨਤੀਜਾ;
    ਜੇ(libssh2_session_ਆਖਰੀ_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    ssh_err = libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
    failf(ਅੰਕੜੇ, "%s", err_ਸੁਨੇਹਾ);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_CHANNEL_FREE);
    ਨਤੀਜਾ = libssh2_session_error_ਮੰਜ਼ਿਲ_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    ਜੇ(ਨਤੀਜਾ == CURLE_SSH ||
       ਨਤੀਜਾ == CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ)
      ਨਤੀਜਾ = CURLE_UPLOAD_FAILED;
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  /* upload data */
  ਅੰਕੜੇ->req.ਮਾਪ = ਅੰਕੜੇ->state.infilesize;
  Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);
  Curl_xfer_ਤਿਆਰੀ_send(ਅੰਕੜੇ, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ਅੰਕੜੇ->conn->recv_idx = FIRSTSOCKET;

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_session_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ਪੂਰਨ rc = 0;
  ਜੇ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    ਜੇ(rc < 0) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
    sshc->ssh_channel = ਨਿਰਾ;
  }

  ਜੇ(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;

    ਜੇ(rc < 0) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to disconnect libssh2 session: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode sshc_cleanup(ਬਣਤਰ ssh_conn *sshc, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             bool ਖੰਡ)
{
  ਪੂਰਨ rc;

  ਜੇ(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = ਨਿਰਾ;
  }

  ਜੇ(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    ਜੇ((rc < 0) && ਅੰਕੜੇ) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_ਸੁਨੇਹਾ);
    }
    ਜੇ(!ਖੰਡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ਵਾਪਸ CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = ਨਿਰਾ;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;
    sshc->sshagent_prev_ਪਛਾਣ_ਤੱਤ = ਨਿਰਾ;
  }

  ਜੇ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ਜੇ((rc < 0) && ਅੰਕੜੇ) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to close libssh2 file: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
    ਜੇ(!ਖੰਡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ਵਾਪਸ CURLE_AGAIN;

    sshc->sftp_handle = ਨਿਰਾ;
  }

  ਜੇ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ਜੇ((rc < 0) && ਅੰਕੜੇ) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
    ਜੇ(!ਖੰਡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ਵਾਪਸ CURLE_AGAIN;

    sshc->ssh_channel = ਨਿਰਾ;
  }

  ਜੇ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ਜੇ((rc < 0) && ਅੰਕੜੇ) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
    ਜੇ(!ਖੰਡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ਵਾਪਸ CURLE_AGAIN;

    sshc->sftp_session = ਨਿਰਾ;
  }

  ਜੇ(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    ਜੇ((rc < 0) && ਅੰਕੜੇ) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session, &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to free libssh2 session: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
    ਜੇ(!ਖੰਡ && (rc == LIBSSH2_ERROR_EAGAIN))
      ਵਾਪਸ CURLE_AGAIN;

    sshc->ssh_session = ਨਿਰਾ;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_ਕੁੰਜੀ);
  curlx_safefree(sshc->priv_ਕੁੰਜੀ);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_getinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.get_filetime)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_FILETIME);
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_TRANS_INIT);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_filetime(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ssh_conn *sshc,
                                        ਬਣਤਰ SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ਗੁਣ attrs;
  ਪੂਰਨ rc;

  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->ਰਸਤਾ,
                            curlx_uztoui(ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ)),
                            LIBSSH2_SFTP_STAT, &attrs);
  ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  ਜੇ(rc == 0)
    ਅੰਕੜੇ->info.filetime = (time_t)attrs.mtime;

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_TRANS_INIT);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_trans_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ ssh_conn *sshc,
                                          ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(ਅੰਕੜੇ->state.upload)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_UPLOAD_INIT);
  ਨਹੀਂਤਾਂ ਜੇ(sshp) {
    size_t plen = ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ);
    ਜੇ(plen && (sshp->ਰਸਤਾ[plen - 1] == '/'))
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_INIT);
    ਨਹੀਂਤਾਂ
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_upload_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ ssh_conn *sshc,
                                           ਬਣਤਰ SSHPROTO *sshp,
                                           bool *ਖੰਡ)
{
  CURLcode ਨਤੀਜਾ;
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = sftp_upload_init(ਅੰਕੜੇ, sshc, sshp, ਖੰਡ);
  ਜੇ(ਨਤੀਜਾ) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_sftp_create_dirs_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                ਬਣਤਰ ssh_conn *sshc,
                                                ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ) > 1) {
    sshc->slash_pos = sshp->ਰਸਤਾ + 1; /* ignore the leading '/' */
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_UPLOAD_INIT);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_create_dirs(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ ssh_conn *sshc,
                                           ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ਜੇ(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(ਅੰਕੜੇ, "SFTP: creating directory '%s'", sshp->ਰਸਤਾ);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ਵਾਪਸ CURLE_OK;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_UPLOAD_INIT);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_readdir(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc,
                                       ਬਣਤਰ SSHPROTO *sshp,
                                       bool *ਖੰਡ)
{
  CURLcode ਨਤੀਜਾ;
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = sftp_readdir(ਅੰਕੜੇ, sshc, sshp, ਖੰਡ);
  ਜੇ(ਨਤੀਜਾ)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_sftp_readdir_bottom(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              ਬਣਤਰ ssh_conn *sshc,
                                              ਬਣਤਰ SSHPROTO *sshp)
{
  CURLcode ਨਤੀਜਾ;
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  ਜੇ(ਨਤੀਜਾ) {
    curlx_dyn_free(&sshp->readdir);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  }
  ਨਹੀਂਤਾਂ {
    curlx_dyn_reset(&sshp->readdir);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_sftp_readdir_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                            ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;

  sshc->sftp_handle = ਨਿਰਾ;

  /* no data to transfer */
  Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_sftp_download_stat(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਬਣਤਰ ssh_conn *sshc,
                                             ਬਣਤਰ SSHPROTO *sshp,
                                             bool *ਖੰਡ)
{
  CURLcode ਨਤੀਜਾ;
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = sftp_download_stat(ਅੰਕੜੇ, sshc, sshp, ਖੰਡ);
  ਜੇ(ਨਤੀਜਾ) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_state_scp_trans_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ ssh_conn *sshc,
                                         ਬਣਤਰ SSHPROTO *sshp)
{
  CURLcode ਨਤੀਜਾ;
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = Curl_getworkingpath(ਅੰਕੜੇ, sshc->homedir,
                               &sshp->ਰਸਤਾ);
  ਜੇ(ਨਤੀਜਾ) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(ਅੰਕੜੇ->state.upload) {
    ਜੇ(ਅੰਕੜੇ->state.infilesize < 0) {
      failf(ਅੰਕੜੇ, "SCP requires a known file size for upload");
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_CHANNEL_FREE);
      ਵਾਪਸ CURLE_UPLOAD_FAILED;
    }
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_DOWNLOAD_INIT);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_scp_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(ਅੰਕੜੇ->state.upload)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_SEND_EOF);
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_CHANNEL_FREE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_scp_send_eof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->ssh_channel) {
    ਪੂਰਨ rc = libssh2_channel_send_eof(sshc->ssh_channel);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;
    ਜੇ(rc) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session,
                                       &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_ਸੁਨੇਹਾ);
    }
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_WAIT_EOF);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_scp_wait_eof(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->ssh_channel) {
    ਪੂਰਨ rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;
    ਜੇ(rc) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session,
                                       &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to get channel EOF: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_WAIT_CLOSE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_scp_wait_close(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->ssh_channel) {
    ਪੂਰਨ rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;
    ਜੇ(rc) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session,
                                       &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Channel failed to close: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_CHANNEL_FREE);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_scp_channel_free(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->ssh_channel) {
    ਪੂਰਨ rc = libssh2_channel_free(sshc->ssh_channel);
    ਜੇ(rc == LIBSSH2_ERROR_EAGAIN)
      ਵਾਪਸ CURLE_AGAIN;
    ਜੇ(rc < 0) {
      ਅੱਖਰ *err_ਸੁਨੇਹਾ = ਨਿਰਾ;
      (ਖਾਲੀ)libssh2_session_ਆਖਰੀ_error(sshc->ssh_session,
                                       &err_ਸੁਨੇਹਾ, ਨਿਰਾ, 0);
      infof(ਅੰਕੜੇ, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ਸੁਨੇਹਾ);
    }
    sshc->ssh_channel = ਨਿਰਾ;
  }
  CURL_TRC_SSH(ਅੰਕੜੇ, "SCP DONE phase complete");
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssh_state_session_free(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc)
{
  CURLcode ਨਤੀਜਾ = sshc_cleanup(sshc, ਅੰਕੜੇ, FALSE);
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  memset(sshc, 0, ਆਕਾਰ(ਬਣਤਰ ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ਸਥਿਰ CURLcode ssh_statemachine(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ssh_conn *sshc,
                                 ਬਣਤਰ SSHPROTO *sshp,
                                 bool *ਖੰਡ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  *ਖੰਡ = 0; /* we are not blocking by default */

  ਕਰੋ {
    ਚੋਣ(sshc->state) {
    ਹਾਲਤ SSH_INIT:
      ਨਤੀਜਾ = ssh_state_init(ਅੰਕੜੇ, sshc);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
      FALLTHROUGH();

    ਹਾਲਤ SSH_S_STARTUP:
      ਨਤੀਜਾ = ssh_state_startup(ਅੰਕੜੇ, sshc);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
      FALLTHROUGH();

    ਹਾਲਤ SSH_HOSTKEY:
      ਨਤੀਜਾ = ssh_state_hostkey(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTHLIST:
      ਨਤੀਜਾ = ssh_state_authlist(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_PKEY:
      ਨਤੀਜਾ = ssh_state_auth_pkey(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_PASS_INIT:
      ਨਤੀਜਾ = ssh_state_auth_pass_init(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_PASS:
      ਨਤੀਜਾ = ssh_state_auth_pass(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_HOST_INIT:
      ਨਤੀਜਾ = ssh_state_auth_host_init(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_HOST:
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_AGENT_INIT);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_AGENT_INIT:
      ਨਤੀਜਾ = ssh_state_auth_agent_init(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_AGENT_LIST:
      ਨਤੀਜਾ = ssh_state_auth_agent_list(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_AGENT:
      ਨਤੀਜਾ = ssh_state_auth_agent(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_ਕੁੰਜੀ_INIT:
      ਨਤੀਜਾ = ssh_state_auth_ਕੁੰਜੀ_init(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_ਕੁੰਜੀ:
      ਨਤੀਜਾ = ssh_state_auth_ਕੁੰਜੀ(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_AUTH_ਪੂਰਾ:
      ਨਤੀਜਾ = ssh_state_auth_ਪੂਰਾ(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_INIT:
      ਨਤੀਜਾ = ssh_state_sftp_init(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_REALPATH:
      ਨਤੀਜਾ = ssh_state_sftp_realpath(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE_INIT:
      ਨਤੀਜਾ = ssh_state_sftp_quote_init(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_POSTQUOTE_INIT:
      ਨਤੀਜਾ = ssh_state_sftp_postquote_init(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE:
      ਨਤੀਜਾ = ssh_state_sftp_quote(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_ਅਗਲਾ_QUOTE:
      ਨਤੀਜਾ = ssh_state_sftp_ਅਗਲਾ_quote(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE_STAT:
      ਨਤੀਜਾ = ssh_state_sftp_quote_stat(ਅੰਕੜੇ, sshc, sshp, ਖੰਡ);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE_SETSTAT:
      ਨਤੀਜਾ = ssh_state_sftp_quote_setstat(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE_SYMLINK:
      ਨਤੀਜਾ = ssh_state_sftp_quote_symlink(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE_MKDIR:
      ਨਤੀਜਾ = ssh_state_sftp_quote_mkdir(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE_RENAME:
      ਨਤੀਜਾ = ssh_state_sftp_quote_rename(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE_RMDIR:
      ਨਤੀਜਾ = ssh_state_sftp_quote_rmdir(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE_UNLINK:
      ਨਤੀਜਾ = ssh_state_sftp_quote_unlink(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_QUOTE_STATVFS:
      ਨਤੀਜਾ = ssh_state_sftp_quote_statvfs(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_GETINFO:
      ਨਤੀਜਾ = ssh_state_sftp_getinfo(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_FILETIME:
      ਨਤੀਜਾ = ssh_state_sftp_filetime(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_TRANS_INIT:
      ਨਤੀਜਾ = ssh_state_sftp_trans_init(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_UPLOAD_INIT:
      ਨਤੀਜਾ = ssh_state_sftp_upload_init(ਅੰਕੜੇ, sshc, sshp, ਖੰਡ);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_CREATE_DIRS_INIT:
      ਨਤੀਜਾ = ssh_state_sftp_create_dirs_init(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_CREATE_DIRS:
      ਨਤੀਜਾ = ssh_state_sftp_create_dirs(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_CREATE_DIRS_MKDIR:
      ਨਤੀਜਾ = ssh_state_sftp_create_dirs_mkdir(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_READDIR_INIT:
      ਨਤੀਜਾ = ssh_state_sftp_readdir_init(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_READDIR:
      ਨਤੀਜਾ = ssh_state_sftp_readdir(ਅੰਕੜੇ, sshc, sshp, ਖੰਡ);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_READDIR_LINK:
      ਨਤੀਜਾ = ssh_state_sftp_readdir_link(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_READDIR_BOTTOM:
      ਨਤੀਜਾ = ssh_state_sftp_readdir_bottom(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_READDIR_ਪੂਰਾ:
      ਨਤੀਜਾ = ssh_state_sftp_readdir_ਪੂਰਾ(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_DOWNLOAD_INIT:
      ਨਤੀਜਾ = ssh_state_sftp_download_init(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_DOWNLOAD_STAT:
      ਨਤੀਜਾ = ssh_state_sftp_download_stat(ਅੰਕੜੇ, sshc, sshp, ਖੰਡ);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_CLOSE:
      ਨਤੀਜਾ = ssh_state_sftp_close(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SFTP_SHUTDOWN:
      ਨਤੀਜਾ = ssh_state_sftp_shutdown(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SCP_TRANS_INIT:
      ਨਤੀਜਾ = ssh_state_scp_trans_init(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SCP_UPLOAD_INIT:
      ਨਤੀਜਾ = ssh_state_scp_upload_init(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SCP_DOWNLOAD_INIT:
      ਨਤੀਜਾ = ssh_state_scp_download_init(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SCP_ਪੂਰਾ:
      ਨਤੀਜਾ = ssh_state_scp_ਪੂਰਾ(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SCP_SEND_EOF:
      ਨਤੀਜਾ = ssh_state_scp_send_eof(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SCP_WAIT_EOF:
      ਨਤੀਜਾ = ssh_state_scp_wait_eof(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SCP_WAIT_CLOSE:
      ਨਤੀਜਾ = ssh_state_scp_wait_close(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SCP_CHANNEL_FREE:
      ਨਤੀਜਾ = ssh_state_scp_channel_free(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SESSION_DISCONNECT:
      ਨਤੀਜਾ = ssh_state_session_disconnect(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_SESSION_FREE:
      ਨਤੀਜਾ = ssh_state_session_free(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;

    ਹਾਲਤ SSH_QUIT:
    ਮੂਲ:
      /* internal error */
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
      ਰੋਕੋ;
    }

  } ਜਦੋਂ(!ਨਤੀਜਾ && (sshc->state != SSH_STOP) && !*ਖੰਡ);

  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *ਖੰਡ = TRUE;
    ਨਤੀਜਾ = CURLE_OK;
  }
  CURL_TRC_SSH(ਅੰਕੜੇ, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (ਪੂਰਨ)ਨਤੀਜਾ, *ਖੰਡ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ਸਥਿਰ ਖਾਲੀ ssh_block2waitfor(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ssh_conn *sshc,
                              bool ਖੰਡ)
{
  ਪੂਰਨ dir = 0;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(ਖੰਡ) {
    dir = libssh2_session_ਖੰਡ_directions(sshc->ssh_session);
    ਜੇ(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_ਖੰਡ_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_ਖੰਡ_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  ਜੇ(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
ਸਥਿਰ CURLcode ssh_multi_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ਬਣਤਰ SSHPROTO *sshp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SSH_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool ਖੰਡ; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  ਜੇ(!sshc || !sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਕਰੋ {
    ਨਤੀਜਾ = ssh_statemachine(ਅੰਕੜੇ, sshc, sshp, &ਖੰਡ);
    *ਪੂਰਾ = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } ਜਦੋਂ(!ਨਤੀਜਾ && !*ਪੂਰਾ && !ਖੰਡ);
  ssh_block2waitfor(ਅੰਕੜੇ, sshc, ਖੰਡ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssh_ਖੰਡ_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssh_conn *sshc,
                                    ਬਣਤਰ SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ curltime ਸ਼ੁਰੂ = *Curl_pgrs_now(ਅੰਕੜੇ);

  ਜਦੋਂ((sshc->state != SSH_STOP) && !ਨਤੀਜਾ) {
    bool ਖੰਡ;
    timediff_t ਖੱਬਾ_ms = 1000;

    ਨਤੀਜਾ = ssh_statemachine(ਅੰਕੜੇ, sshc, sshp, &ਖੰਡ);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    ਜੇ(!disconnect) {
      ਨਤੀਜਾ = Curl_pgrsCheck(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;

      ਖੱਬਾ_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
      ਜੇ(ਖੱਬਾ_ms < 0) {
        failf(ਅੰਕੜੇ, "Operation timed out");
        ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &ਸ਼ੁਰੂ) > 1000) {
      /* disconnect timeout */
      failf(ਅੰਕੜੇ, "Disconnect timed out");
      ਨਤੀਜਾ = CURLE_OK;
      ਰੋਕੋ;
    }

    ਜੇ(ਖੰਡ) {
      ਪੂਰਨ dir = libssh2_session_ਖੰਡ_directions(sshc->ssh_session);
      curl_socket_t sock = ਅੰਕੜੇ->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      ਜੇ(LIBSSH2_SESSION_ਖੰਡ_INBOUND & dir)
        fd_read = sock;
      ਜੇ(LIBSSH2_SESSION_ਖੰਡ_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (ਖਾਲੀ)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!ਖੱਬਾ_ms || ਖੱਬਾ_ms > 1000) ? 1000 : ਖੱਬਾ_ms);
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ myssh_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ SSHPROTO *sshp = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curlx_safefree(sshp->ਰਸਤਾ);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ਸਥਿਰ ਖਾਲੀ myssh_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ ssh_conn *sshc = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  sshc_cleanup(sshc, ਨਿਰਾ, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ਸਥਿਰ CURLcode ssh_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ ssh_conn *sshc;
  ਬਣਤਰ SSHPROTO *sshp;
  (ਖਾਲੀ)conn;

  sshc = curlx_calloc(1, ਆਕਾਰ(*sshc));
  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  sshp = curlx_calloc(1, ਆਕਾਰ(*sshp));
  ਜੇ(!sshp)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  curlx_dyn_init(&sshp->readdir, CURL_ਰਸਤਾ_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_ਰਸਤਾ_MAX);
  ਜੇ(Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ Curl_ssh_ਤਿਆਰੀ_pkey(ਅੰਕੜੇ, sshc);
}

ਸਥਿਰ Curl_recv scp_recv, sftp_recv;
ਸਥਿਰ Curl_send scp_send, sftp_send;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
ਸਥਿਰ ssize_t ssh_tls_recv(libssh2_socket_t sock, ਖਾਲੀ *ਬਫਰ,
                            size_t ਲੰਮਾਈ, ਪੂਰਨ flags, ਖਾਲੀ **abstract)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = (ਬਣਤਰ Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(ਅੰਕੜੇ, sock);
  size_t nread;
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  Curl_recv *backup = conn->recv[sockindex];
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ਖਾਲੀ)flags;

  ਜੇ(!sshc)
    ਵਾਪਸ -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  ਨਤੀਜਾ = Curl_conn_recv(ਅੰਕੜੇ, sockindex, ਬਫਰ, ਲੰਮਾਈ, &nread);
  conn->recv[sockindex] = backup;
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
    ਵਾਪਸ -EAGAIN; /* magic return code for libssh2 */
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ -1; /* generic error */
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਅੰਕੜੇ_ਵਿੱਚ, (ਅਚਲ ਅੱਖਰ *)ਬਫਰ, nread);
  ਵਾਪਸ (ssize_t)nread;
}

ਸਥਿਰ ssize_t ssh_tls_send(libssh2_socket_t sock, ਅਚਲ ਖਾਲੀ *ਬਫਰ,
                            size_t ਲੰਮਾਈ, ਪੂਰਨ flags, ਖਾਲੀ **abstract)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = (ਬਣਤਰ Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(ਅੰਕੜੇ, sock);
  size_t nwrite;
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  Curl_send *backup = conn->send[sockindex];
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ਖਾਲੀ)flags;

  ਜੇ(!sshc)
    ਵਾਪਸ -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  ਨਤੀਜਾ = Curl_conn_send(ਅੰਕੜੇ, sockindex, ਬਫਰ, ਲੰਮਾਈ, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
    ਵਾਪਸ -EAGAIN; /* magic return code for libssh2 */
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ -1; /* error */
  Curl_debug(ਅੰਕੜੇ, CURLINFO_ਅੰਕੜੇ_ਬਾਹਰਲਾ, (ਅਚਲ ਅੱਖਰ *)ਬਫਰ, nwrite);
  ਵਾਪਸ (ssize_t)nwrite;
}
#ਜੇ_ਅੰਤ

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ਸਥਿਰ CURLcode ssh_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#ਜੇ_ਅੰਤ
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ਨਤੀਜਾ;

#ਜੇ LIBSSH2_ਸੰਸਕਰਣ_ਅੰਕ >= 0x010b00
  {
    ਅਚਲ ਅੱਖਰ *crypto_str;
    ਚੋਣ(libssh2_crypto_engine()) {
    ਹਾਲਤ libssh2_gcrypt:
      crypto_str = "libgcrypt";
      ਰੋਕੋ;
    ਹਾਲਤ libssh2_mbedtls:
      crypto_str = "mbedTLS";
      ਰੋਕੋ;
    ਹਾਲਤ libssh2_openssl:
      crypto_str = "openssl compatible";
      ਰੋਕੋ;
    ਹਾਲਤ libssh2_os400qc3:
      crypto_str = "OS400QC3";
      ਰੋਕੋ;
    ਹਾਲਤ libssh2_wincng:
      crypto_str = "WinCNG";
      ਰੋਕੋ;
    ਮੂਲ:
      crypto_str = ਨਿਰਾ;
      ਰੋਕੋ;
    }
    ਜੇ(crypto_str)
      infof(ਅੰਕੜੇ, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#ਜੇ_ਅੰਤ

  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  infof(ਅੰਕੜੇ, "SSH: user '%s'", Curl_creds_user(conn->creds));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_LIBSSH2_DEBUG
  infof(ਅੰਕੜੇ, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#ਜੇ_ਅੰਤ /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, ਅੰਕੜੇ);

  ਜੇ(!sshc->ssh_session) {
    failf(ਅੰਕੜੇ, "Failure initializing ssh session");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#ਜੇ LIBSSH2_ਸੰਸਕਰਣ_ਅੰਕ >= 0x010B00
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.server_response_timeout > 0) {
    libssh2_session_ਸੈੱਟ_read_timeout(sshc->ssh_session,
                             (ਲੰਮਾ)(ਅੰਕੜੇ->ਸੈੱਟ.server_response_timeout / 1000));
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਜੇ(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#ਜੇ LIBSSH2_ਸੰਸਕਰਣ_ਅੰਕ >= 0x010b01
    infof(ਅੰਕੜੇ, "SSH: using HTTPS proxy");
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    ਸੰਘ receive {
      ਖਾਲੀ *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, ਖਾਲੀ *, size_t, ਪੂਰਨ, ਖਾਲੀ **);
    };
    ਸੰਘ transfer {
      ਖਾਲੀ *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ਅਚਲ ਖਾਲੀ *, size_t, ਪੂਰਨ, ਖਾਲੀ **);
    };
    ਸੰਘ receive sshrecv;
    ਸੰਘ transfer sshsend;

    sshrecv.recvptr = ssh_tls_recv;
    sshsend.sendptr = ssh_tls_send;

    infof(ਅੰਕੜੇ, "SSH: using HTTPS proxy");
    libssh2_session_callback_ਸੈੱਟ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_ਸੈੱਟ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#ਜੇ_ਅੰਤ

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#ਜੇ_ਅੰਤ /* CURL_DISABLE_PROXY */
  ਜੇ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ਨਹੀਂਤਾਂ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(ਅੰਕੜੇ, "SSH: failed to enable compression for session");
  }

  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS)) {
    ਪੂਰਨ rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    ਜੇ(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = ਨਿਰਾ;
      ਵਾਪਸ CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    ਜੇ(rc < 0)
      infof(ਅੰਕੜੇ, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS));
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(ਅੰਕੜੇ, "SSH: socket %d", (ਪੂਰਨ)sock);
#ਜੇ_ਅੰਤ

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_INIT);

  ਨਤੀਜਾ = ssh_multi_statemach(ਅੰਕੜੇ, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ਸਥਿਰ CURLcode scp_perform(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            bool *connected,
                            bool *dophase_ਪੂਰਾ)
{
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SSH_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CURL_TRC_SSH(ਅੰਕੜੇ, "DO phase starts");

  *dophase_ਪੂਰਾ = FALSE; /* not done yet */
  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  ਨਤੀਜਾ = ssh_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  *connected = Curl_conn_is_connected(ਅੰਕੜੇ->conn, FIRSTSOCKET);

  ਜੇ(*dophase_ਪੂਰਾ) {
    CURL_TRC_SSH(ਅੰਕੜੇ, "DO phase is complete");
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* called from multi.c while DOing */
ਸਥਿਰ CURLcode scp_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  ਨਤੀਜਾ = ssh_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  ਜੇ(*dophase_ਪੂਰਾ) {
    CURL_TRC_SSH(ਅੰਕੜੇ, "DO phase is complete");
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ਸਥਿਰ CURLcode scp_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ connectdata *conn,
                               bool dead_connection)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ਬਣਤਰ SSHPROTO *sshp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SSH_EASY);
  (ਖਾਲੀ)dead_connection;

  ਜੇ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_DISCONNECT);
    ਨਤੀਜਾ = ssh_ਖੰਡ_statemach(ਅੰਕੜੇ, sshc, sshp, TRUE);
  }

  ਜੇ(sshc)
    ਵਾਪਸ sshc_cleanup(sshc, ਅੰਕੜੇ, TRUE);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ਸਥਿਰ CURLcode ssh_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode ਸਥਿਤੀ)
{
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SSH_CONN);
  ਬਣਤਰ SSHPROTO *sshp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SSH_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!sshc || !sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(!ਸਥਿਤੀ)
    /* run the state-machine */
    ਨਤੀਜਾ = ssh_ਖੰਡ_statemach(ਅੰਕੜੇ, sshc, sshp, FALSE);
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = ਸਥਿਤੀ;

  ਜੇ(Curl_pgrs_ਪੂਰਾ(ਅੰਕੜੇ, Curl_pgrs_now(ਅੰਕੜੇ)))
    ਵਾਪਸ CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode scp_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode ਸਥਿਤੀ,
                         bool premature)
{
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SSH_CONN);
  (ਖਾਲੀ)premature;

  ਜੇ(sshc && !ਸਥਿਤੀ)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_ਪੂਰਾ);

  ਵਾਪਸ ssh_ਪੂਰਾ(ਅੰਕੜੇ, ਸਥਿਤੀ);
}

ਸਥਿਰ CURLcode scp_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                         ਅਚਲ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ssize_t nwritten;

  (ਖਾਲੀ)sockindex; /* we only support SCP on the fixed known primary socket */
  (ਖਾਲੀ)eos;
  *pnwritten = 0;

  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ਅਚਲ ਅੱਖਰ *)mem, len);

  ssh_block2waitfor(ਅੰਕੜੇ, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  ਜੇ(nwritten == LIBSSH2_ERROR_EAGAIN)
    ਨਤੀਜਾ = CURLE_AGAIN;
  ਨਹੀਂਤਾਂ ਜੇ(nwritten < LIBSSH2_ERROR_NONE)
    ਨਤੀਜਾ = libssh2_session_error_ਮੰਜ਼ਿਲ_CURLE((ਪੂਰਨ)nwritten);
  ਨਹੀਂਤਾਂ
    *pnwritten = (size_t)nwritten;

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode scp_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                         ਅੱਖਰ *mem, size_t len, size_t *pnread)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ssize_t nread;

  (ਖਾਲੀ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(ਅੰਕੜੇ, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  ਜੇ(nread == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;
  ਨਹੀਂਤਾਂ ਜੇ(nread < LIBSSH2_ERROR_NONE)
    ਨਤੀਜਾ = libssh2_session_error_ਮੰਜ਼ਿਲ_CURLE((ਪੂਰਨ)nread);
  ਨਹੀਂਤਾਂ
    *pnread = (size_t)nread;

  ਵਾਪਸ ਨਤੀਜਾ;
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

ਸਥਿਰ CURLcode sftp_perform(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             bool *connected,
                             bool *dophase_ਪੂਰਾ)
{
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SSH_CONN);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CURL_TRC_SSH(ਅੰਕੜੇ, "DO phase starts");

  *dophase_ਪੂਰਾ = FALSE; /* not done yet */
  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  ਨਤੀਜਾ = ssh_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  *connected = Curl_conn_is_connected(ਅੰਕੜੇ->conn, FIRSTSOCKET);

  ਜੇ(*dophase_ਪੂਰਾ) {
    CURL_TRC_SSH(ਅੰਕੜੇ, "DO phase is complete");
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* called from multi.c while DOing */
ਸਥਿਰ CURLcode sftp_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = ssh_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  ਜੇ(*dophase_ਪੂਰਾ) {
    CURL_TRC_SSH(ਅੰਕੜੇ, "DO phase is complete");
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ਸਥਿਰ CURLcode sftp_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn, bool dead_connection)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ਬਣਤਰ SSHPROTO *sshp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SSH_EASY);
  (ਖਾਲੀ)dead_connection;

  ਜੇ(sshc) {
    ਜੇ(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(ਅੰਕੜੇ, "DISCONNECT starts now");
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_SHUTDOWN);
      ਨਤੀਜਾ = ssh_ਖੰਡ_statemach(ਅੰਕੜੇ, sshc, sshp, TRUE);
      CURL_TRC_SSH(ਅੰਕੜੇ, "DISCONNECT is done -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    }
    sshc_cleanup(sshc, ਅੰਕੜੇ, TRUE);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode sftp_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode ਸਥਿਤੀ,
                          bool premature)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਜੇ(!ਸਥਿਤੀ) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    ਜੇ(!premature && ਅੰਕੜੇ->ਸੈੱਟ.postquote && !conn->ਬਿੱਟ.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
  }
  ਵਾਪਸ ssh_ਪੂਰਾ(ਅੰਕੜੇ, ਸਥਿਤੀ);
}

/* return number of sent bytes */
ਸਥਿਰ CURLcode sftp_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                          ਅਚਲ uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (ਖਾਲੀ)sockindex;
  (ਖਾਲੀ)eos;
  *pnwritten = 0;

  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ਅਚਲ ਅੱਖਰ *)mem, len);

  ssh_block2waitfor(ਅੰਕੜੇ, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  ਜੇ(nwrite == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;
  ਨਹੀਂਤਾਂ ਜੇ(nwrite < LIBSSH2_ERROR_NONE)
    ਵਾਪਸ libssh2_session_error_ਮੰਜ਼ਿਲ_CURLE((ਪੂਰਨ)nwrite);
  *pnwritten = (size_t)nwrite;
  ਵਾਪਸ CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ਸਥਿਰ CURLcode sftp_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                          ਅੱਖਰ *mem, size_t len, size_t *pnread)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (ਖਾਲੀ)sockindex;
  *pnread = 0;

  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(ਅੰਕੜੇ, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  ਜੇ(nread == LIBSSH2_ERROR_EAGAIN)
    ਵਾਪਸ CURLE_AGAIN;
  ਨਹੀਂਤਾਂ ਜੇ(nread < 0)
    ਵਾਪਸ libssh2_session_error_ਮੰਜ਼ਿਲ_CURLE((ਪੂਰਨ)nread);

  *pnread = (size_t)nread;
  ਵਾਪਸ CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
ਸਥਿਰ CURLcode ssh_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  bool connected = FALSE;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *ਪੂਰਾ = FALSE; /* default to false */
  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਅੰਕੜੇ->req.ਮਾਪ = -1; /* make sure this is unknown at this point */
  sshc->ਦੂਜਾ_Create_Dirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(ਅੰਕੜੇ);

  ਜੇ(conn->scheme->protocol & CURLPROTO_SCP)
    ਨਤੀਜਾ = scp_perform(ਅੰਕੜੇ, &connected, ਪੂਰਾ);
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = sftp_perform(ਅੰਕੜੇ, &connected, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_ssh_init(ਖਾਲੀ)
{
  ਜੇ(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_ssh_cleanup(ਖਾਲੀ)
{
  libssh2_exit();
}

ਖਾਲੀ Curl_ssh_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t buflen)
{
  (ਖਾਲੀ)curl_msnprintf(ਬਫਰ, buflen, "libssh2/%s", libssh2_ਸੰਸਕਰਣ(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ਸਥਿਰ ਖਾਲੀ ssh_attach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ connectdata *conn)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  DEBUGASSERT(conn);
  ਜੇ(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    ਜੇ(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      ਖਾਲੀ **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = ਅੰਕੜੇ;
    }
  }
}

/*
 * SCP protocol handler.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_scp = {
  ssh_ਤਿਆਰੀ_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_ਪੂਰਾ,                             /* done */
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
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_sftp = {
  ssh_ਤਿਆਰੀ_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  sftp_ਪੂਰਾ,                            /* done */
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

#ਜੇ_ਅੰਤ /* USE_LIBSSH2 */
