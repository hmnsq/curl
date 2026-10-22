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

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef __VMS
#ಅನ್ತರ್ಭಾವ <in.h>
#ಅನ್ತರ್ಭಾವ <inet.h>
#endif

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

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *sftp_libssh2_strerror(unsigned long err)
{
  ವಿಕಲ್ಪ(err) {
  ಪಕ್ಷ LIBSSH2_FX_NO_SUCH_FILE:
    ಪ್ರತ್ಯಾಗಮ "No such file or directory";

  ಪಕ್ಷ LIBSSH2_FX_PERMISSION_DENIED:
    ಪ್ರತ್ಯಾಗಮ "Permission denied";

  ಪಕ್ಷ LIBSSH2_FX_FAILURE:
    ಪ್ರತ್ಯಾಗಮ "Operation failed";

  ಪಕ್ಷ LIBSSH2_FX_BAD_MESSAGE:
    ಪ್ರತ್ಯಾಗಮ "Bad message from SFTP server";

  ಪಕ್ಷ LIBSSH2_FX_NO_CONNECTION:
    ಪ್ರತ್ಯಾಗಮ "Not connected to SFTP server";

  ಪಕ್ಷ LIBSSH2_FX_CONNECTION_LOST:
    ಪ್ರತ್ಯಾಗಮ "Connection to SFTP server lost";

  ಪಕ್ಷ LIBSSH2_FX_OP_UNSUPPORTED:
    ಪ್ರತ್ಯಾಗಮ "Operation not supported by SFTP server";

  ಪಕ್ಷ LIBSSH2_FX_INVALID_HANDLE:
    ಪ್ರತ್ಯಾಗಮ "Invalid handle";

  ಪಕ್ಷ LIBSSH2_FX_NO_SUCH_PATH:
    ಪ್ರತ್ಯಾಗಮ "No such file or directory";

  ಪಕ್ಷ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ಪ್ರತ್ಯಾಗಮ "File already exists";

  ಪಕ್ಷ LIBSSH2_FX_WRITE_PROTECT:
    ಪ್ರತ್ಯಾಗಮ "File is write protected";

  ಪಕ್ಷ LIBSSH2_FX_NO_MEDIA:
    ಪ್ರತ್ಯಾಗಮ "No media";

  ಪಕ್ಷ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    ಪ್ರತ್ಯಾಗಮ "Disk full";

  ಪಕ್ಷ LIBSSH2_FX_QUOTA_EXCEEDED:
    ಪ್ರತ್ಯಾಗಮ "User quota exceeded";

  ಪಕ್ಷ LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    ಪ್ರತ್ಯಾಗಮ "Unknown principal";

  ಪಕ್ಷ LIBSSH2_FX_LOCK_CONFLICT:
    ಪ್ರತ್ಯಾಗಮ "File lock conflict";

  ಪಕ್ಷ LIBSSH2_FX_DIR_NOT_EMPTY:
    ಪ್ರತ್ಯಾಗಮ "Directory not empty";

  ಪಕ್ಷ LIBSSH2_FX_NOT_A_DIRECTORY:
    ಪ್ರತ್ಯಾಗಮ "Not a directory";

  ಪಕ್ಷ LIBSSH2_FX_INVALID_FILENAME:
    ಪ್ರತ್ಯಾಗಮ "Invalid filename";

  ಪಕ್ಷ LIBSSH2_FX_LINK_LOOP:
    ಪ್ರತ್ಯಾಗಮ "Link points to itself";
  }
  ಪ್ರತ್ಯಾಗಮ "Unknown error in libssh2";
}

ಸ್ಥಿರ ಶೂನ್ಯ kbd_callback(ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ name_len,
                         ಧ್ರುವ ವರ್ಣ *instruction, ಪೂರ್ಣಾಙ್ಕ instruction_len,
                         ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_prompts,
                         ಧ್ರುವ LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         ಶೂನ್ಯ **abstract)
{
  ಸಂರಚನಾ Curl_easy *data = (ಸಂರಚನಾ Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", ಅಂಕೆ_prompts);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)name;
  (ಶೂನ್ಯ)name_len;
  (ಶೂನ್ಯ)instruction;
  (ಶೂನ್ಯ)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  ಯದಿ(ಅಂಕೆ_prompts == 1) {
    ಸಂರಚನಾ connectdata *conn = data->conn;
    ಧ್ರುವ ವರ್ಣ *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (ಶೂನ್ಯ)prompts;
} /* kbd_callback */

ಸ್ಥಿರ CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  ವಿಕಲ್ಪ(err) {
  ಪಕ್ಷ LIBSSH2_FX_OK:
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಪಕ್ಷ LIBSSH2_FX_NO_SUCH_FILE:
  ಪಕ್ಷ LIBSSH2_FX_NO_SUCH_PATH:
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_FILE_NOT_FOUND;

  ಪಕ್ಷ LIBSSH2_FX_PERMISSION_DENIED:
  ಪಕ್ಷ LIBSSH2_FX_WRITE_PROTECT:
  ಪಕ್ಷ LIBSSH2_FX_LOCK_CONFLICT:
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_ACCESS_DENIED;

  ಪಕ್ಷ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  ಪಕ್ಷ LIBSSH2_FX_QUOTA_EXCEEDED:
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_DISK_FULL;

  ಪಕ್ಷ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_FILE_EXISTS;

  ಪಕ್ಷ LIBSSH2_FX_DIR_NOT_EMPTY:
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;

  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_SSH;
}

ಸ್ಥಿರ CURLcode libssh2_session_error_to_CURLE(ಪೂರ್ಣಾಙ್ಕ err)
{
  ವಿಕಲ್ಪ(err) {
  /* Ordered by order of appearance in libssh2.h */
  ಪಕ್ಷ LIBSSH2_ERROR_NONE:
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  ಪಕ್ಷ LIBSSH2_ERROR_SCP_PROTOCOL:
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_FILE_NOT_FOUND;

  ಪಕ್ಷ LIBSSH2_ERROR_SOCKET_NONE:
    ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;

  ಪಕ್ಷ LIBSSH2_ERROR_ALLOC:
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪಕ್ಷ LIBSSH2_ERROR_SOCKET_SEND:
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;

  ಪಕ್ಷ LIBSSH2_ERROR_HOSTKEY_INIT:
  ಪಕ್ಷ LIBSSH2_ERROR_HOSTKEY_SIGN:
  ಪಕ್ಷ LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  ಪಕ್ಷ LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;

  ಪಕ್ಷ LIBSSH2_ERROR_PASSWORD_EXPIRED:
    ಪ್ರತ್ಯಾಗಮ CURLE_LOGIN_DENIED;

  ಪಕ್ಷ LIBSSH2_ERROR_SOCKET_TIMEOUT:
  ಪಕ್ಷ LIBSSH2_ERROR_TIMEOUT:
    ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;

  ಪಕ್ಷ LIBSSH2_ERROR_EAGAIN:
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

ಸ್ಥಿರ LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (ಶೂನ್ಯ)abstract;
  ಪ್ರತ್ಯಾಗಮ Curl_cmalloc(ಎಣಿಕೆ);
}

ಸ್ಥಿರ LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (ಶೂನ್ಯ)abstract;
  ಪ್ರತ್ಯಾಗಮ Curl_crealloc(ptr, ಎಣಿಕೆ);
}

ಸ್ಥಿರ LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (ಶೂನ್ಯ)abstract;
  ಯದಿ(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sshkeycallback(CURL *easy,
                          ಧ್ರುವ ಸಂರಚನಾ curl_khkey *knownkey, /* known */
                          ಧ್ರುವ ಸಂರಚನಾ curl_khkey *foundkey, /* found */
                          ಗಣನಾ curl_khmatch match,
                          ಶೂನ್ಯ *clientp)
{
  (ಶೂನ್ಯ)easy;
  (ಶೂನ್ಯ)knownkey;
  (ಶೂನ್ಯ)foundkey;
  (ಶೂನ್ಯ)clientp;

  /* we only allow perfect matches, and we reject everything else */
  ಪ್ರತ್ಯಾಗಮ (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ಸ್ಥಿರ ಗಣನಾ curl_khtype convert_ssh2_keytype(ಪೂರ್ಣಾಙ್ಕ sshkeytype)
{
  ಗಣನಾ curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  ವಿಕಲ್ಪ(sshkeytype) {
  ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    ಭಙ್ಗ;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    ಭಙ್ಗ;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    ಭಙ್ಗ;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    ಭಙ್ಗ;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    ಭಙ್ಗ;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    ಭಙ್ಗ;
#endif
  }
  ಪ್ರತ್ಯಾಗಮ keytype;
}

ಸ್ಥಿರ CURLcode ssh_knownhost(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ssh_conn *sshc)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ libssh2_knownhost *host = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *remotekey = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  ಪೂರ್ಣಾಙ್ಕ keybit = 0;
  ಪೂರ್ಣಾಙ್ಕ sshkeytype = 0;
  size_t keylen = 0;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  CURLcode result = CURLE_OK;

  ಯದಿ(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  ಯದಿ(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    ಸಂರಚನಾ Curl_mapi_guard guard;
    ಗಣನಾ curl_khmatch keymatch;
    curl_sshkeycallback func =
      data->set.ssh_keyfunc ? data->set.ssh_keyfunc : sshkeycallback;
    ಸಂರಚನಾ curl_khkey knownkey;
    ಸಂರಚನಾ curl_khkey *knownkeyp = ರಿಕ್ತ;
    ಸಂರಚನಾ curl_khkey foundkey;

    ವಿಕಲ್ಪ(sshkeytype) {
    ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHRSA;
      ಭಙ್ಗ;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHDSS;
      ಭಙ್ಗ;
#endif
    ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_256;
      ಭಙ್ಗ;
    ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_384;
      ಭಙ್ಗ;
    ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_521;
      ಭಙ್ಗ;
    ಪಕ್ಷ LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_ಕೀಲಿ_ED25519;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      ಭಙ್ಗ;
    }
    ಯದಿ(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    ಅನ್ಯಥಾ {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->port != PORT_SSH) ?
                                          conn->origin->port : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(data, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->ಕೀಲಿ : "<none>");

      /* setup 'knownkey' */
      ಯದಿ(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
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
      keymatch = (ಗಣನಾ curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  ಅನ್ಯಥಾ {
    /* no remotekey means failure! */
    infof(data, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  ವಿಕಲ್ಪ(rc) {
  ಸ್ವಭಾವ: /* unknown return codes is the same as reject */
  ಪಕ್ಷ CURLKHSTAT_REJECT:
    infof(data, "SSH: knownhost check failed");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  ಪಕ್ಷ CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    result = CURLE_PEER_FAILED_VERIFICATION;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    ಯದಿ(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  ಪಕ್ಷ CURLKHSTAT_FINE:
  ಪಕ್ಷ CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    ಯದಿ(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      ಪೂರ್ಣಾಙ್ಕ addrc;
      ಧ್ರುವ ವರ್ಣ *hostbuf;
      ವರ್ಣ *hostport = ರಿಕ್ತ;
      ಯದಿ(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        ಯದಿ(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
      }
      ಅನ್ಯಥಾ
        hostbuf = conn->origin->hostname;
      ಯದಿ(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, ರಿಕ್ತ,
                                       remotekey, keylen, ರಿಕ್ತ, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, ರಿಕ್ತ);
        ಯದಿ(addrc)
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        ಅನ್ಯಥಾ ಯದಿ(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          ಪೂರ್ಣಾಙ್ಕ wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          ಯದಿ(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    ಅನ್ಯಥಾ
      infof(data, "SSH: knownhost entry matches host key");
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_check_fingerprint(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ssh_conn *sshc)
{
  ಧ್ರುವ ವರ್ಣ *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_MD5);
  ಧ್ರುವ ವರ್ಣ *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_SHA256);

  ಯದಿ(pubkey_sha256) {
    ಧ್ರುವ ವರ್ಣ *fingerprint = ರಿಕ್ತ;
    ವರ್ಣ *fingerprint_b64 = ರಿಕ್ತ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    ಯದಿ(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    ಯದಿ(curlx_base64_encode((ಧ್ರುವ uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
    }

    ಯದಿ(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    ಯಾವತ್((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    ಯಾವತ್((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    ಯದಿ((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  ಯದಿ(pubkey_md5) {
    ವರ್ಣ md5buffer[33];
    ಧ್ರುವ ವರ್ಣ *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    ಯದಿ(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      ಪೂರ್ಣಾಙ್ಕ i;
      ಪ್ರತಿ(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned ವರ್ಣ)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    ಯದಿ(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      ಯದಿ(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      ಅನ್ಯಥಾ {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  ಯದಿ(!pubkey_md5 && !pubkey_sha256) {
    ಯದಿ(data->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      ಪೂರ್ಣಾಙ್ಕ sshkeytype = 0;
      ಪೂರ್ಣಾಙ್ಕ rc = 0;
      /* we handle the process to the callback */
      ಧ್ರುವ ವರ್ಣ *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      ಯದಿ(remotekey) {
        ಸಂರಚನಾ Curl_mapi_guard guard;
        ಗಣನಾ curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (ಪೂರ್ಣಾಙ್ಕ)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        ಯದಿ(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      ಅನ್ಯಥಾ {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
      }
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಅನ್ಯಥಾ {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      ಪ್ರತ್ಯಾಗಮ ssh_knownhost(data, sshc);
    }
  }
  ಅನ್ಯಥಾ {
    /* as we already matched, we skip the check for known hosts */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
ಸ್ಥಿರ CURLcode ssh_force_knownhost_ಕೀಲಿ_type(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHDSS
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  ಯದಿ(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_SHA256)) {
    ಸಂರಚನಾ libssh2_knownhost *store = ರಿಕ್ತ;
    ಸಂರಚನಾ connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_ಕೀಲಿ_MASK >>
                      LIBSSH2_KNOWNHOST_ಕೀಲಿ_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    ಯಾವತ್(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      ಯದಿ(store) {
        ಯದಿ(store->name) {
          ಯದಿ(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_size = 0;
            ಧ್ರುವ ವರ್ಣ *p;
            ಧ್ರುವ ವರ್ಣ *kh_name_end = strstr(store->name, "]:");
            ಯದಿ(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              ಅನುವರ್ತ;
            }
            p = kh_name_end + 2; /* start of port number */
            ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = strlen(store->name) - 1 - strlen(kh_name_end);
              ಯದಿ((kh_name_size == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                ಭಙ್ಗ;
              }
            }
          }
          ಅನ್ಯಥಾ ಯದಿ(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            ಭಙ್ಗ;
          }
        }
        ಅನ್ಯಥಾ ಯದಿ(store->ಕೀಲಿ) {
          unsigned ಪೂರ್ಣಾಙ್ಕ type = (store->typemask & LIBSSH2_KNOWNHOST_ಕೀಲಿ_MASK)
                              >> LIBSSH2_KNOWNHOST_ಕೀಲಿ_SHIFT;
          ಯದಿ(!absent_type[type]) {
            ಪೂರ್ಣಾಙ್ಕ keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->ಕೀಲಿ, strlen(store->ಕೀಲಿ),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_ಕೀಲಿ_MASK),
              ರಿಕ್ತ);
            ಯದಿ(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              ಭಙ್ಗ;
            }
            ಅನ್ಯಥಾ ಯದಿ(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    ಯದಿ(found) {
      ಪೂರ್ಣಾಙ್ಕ rc;
      ಧ್ರುವ ವರ್ಣ *hostkey_method = ರಿಕ್ತ;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      ವಿಕಲ್ಪ(store->typemask & LIBSSH2_KNOWNHOST_ಕೀಲಿ_MASK) {
      ಪಕ್ಷ LIBSSH2_KNOWNHOST_ಕೀಲಿ_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        ಭಙ್ಗ;
      ಪಕ್ಷ LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        ಭಙ್ಗ;
      ಪಕ್ಷ LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        ಭಙ್ಗ;
      ಪಕ್ಷ LIBSSH2_KNOWNHOST_ಕೀಲಿ_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        ಭಙ್ಗ;
      ಪಕ್ಷ LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        ಭಙ್ಗ;
#ifdef LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHDSS
      ಪಕ್ಷ LIBSSH2_KNOWNHOST_ಕೀಲಿ_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        ಭಙ್ಗ;
#endif
      ಪಕ್ಷ LIBSSH2_KNOWNHOST_ಕೀಲಿ_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        ಪ್ರತ್ಯಾಗಮ CURLE_SSH;
      ಸ್ವಭಾವ:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_ಕೀಲಿ_MASK));
        ಪ್ರತ್ಯಾಗಮ CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      ಯದಿ(rc) {
        ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_ಸಂದೇಶ);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    ಅನ್ಯಥಾ {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode quote_error(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
}

ಸ್ಥಿರ CURLcode sftp_quote(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ ssh_conn *sshc,
                           ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಧ್ರುವ ವರ್ಣ *cp;
  CURLcode result = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ಧ್ರುವ ವರ್ಣ *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ಯದಿ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ಯದಿ(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    ವರ್ಣ *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    ಯದಿ(!tmp)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    ಯದಿ(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ಯದಿ(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ಯದಿ(result) {
    ಯದಿ(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  ಯದಿ(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಯದಿ(result) {
      ಯದಿ(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      ಪ್ರತ್ಯಾಗಮ result;
    }
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);

    memset(&sshp->quote_attrs, 0, ಪರಿಮಾಣ(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಯದಿ(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಯದಿ(result) {
      ಯದಿ(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ಪ್ರತ್ಯಾಗಮ result;
    }
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "mkdir ", 6)) {
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಯದಿ(result) {
      ಯದಿ(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ಪ್ರತ್ಯಾಗಮ result;
    }
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "rmdir ", 6)) {
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "rm ", 3)) {
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "statvfs ", 8)) {
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
}

ಸ್ಥಿರ CURLcode sftp_upload_init(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ssh_conn *sshc,
                                 ಸಂರಚನಾ SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  ಯದಿ(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    ಯದಿ(data->state.resume_from < 0) {
      ಪೂರ್ಣಾಙ್ಕ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಯದಿ(rc) {
        data->state.resume_from = 0;
      }
      ಅನ್ಯಥಾ {
        curl_off_t size = attrs.filesize;
        ಯದಿ(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          ಪ್ರತ್ಯಾಗಮ CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  ಯದಿ(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  ಅನ್ಯಥಾ ಯದಿ(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  ಅನ್ಯಥಾ {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  ಯದಿ(!sshc->sftp_handle) {
    CURLcode result;
    unsigned long sftperr;
    ಪೂರ್ಣಾಙ್ಕ rc = libssh2_session_last_errno(sshc->ssh_session);

    ಯದಿ(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }

    ಯದಿ(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    ಅನ್ಯಥಾ
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    ಯದಿ(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      ಪ್ರತ್ಯಾಗಮ sftp_libssh2_error_to_CURLE(sftperr);
    }
    ಯದಿ((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    ಯದಿ(!result) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      result = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(data, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  ಯದಿ(data->state.resume_from > 0 && !data->set.remote_append) {
    ಪೂರ್ಣಾಙ್ಕ seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    ಯದಿ(data->set.seek_func) {
      ಸಂರಚನಾ Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    ಯದಿ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      ಯದಿ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ಕುರು {
        ಸಂರಚನಾ Curl_mapi_guard guard;
        ವರ್ಣ scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)ಪರಿಮಾಣ(scratch)) ?
          ಪರಿಮಾಣ(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        ಯದಿ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          ಪ್ರತ್ಯಾಗಮ CURLE_FTP_COULDNT_USE_REST;
        }
      } ಯಾವತ್(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    ಯದಿ(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  ಯದಿ(data->state.infilesize > 0) {
    data->req.size = data->state.infilesize;
    Curl_pgrsSetUploadSize(data, data->state.infilesize);
  }
  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(data);

  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ ssh_state_pkey_init(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  ಯದಿ((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    ಯದಿ(sshc->pub_ಕೀಲಿ)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_ಕೀಲಿ);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_ಕೀಲಿ);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
}

ಸ್ಥಿರ CURLcode sftp_quote_stat(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ ssh_conn *sshc,
                                ಸಂರಚನಾ SSHPROTO *sshp,
                                bool *blockp)
{
  ಧ್ರುವ ವರ್ಣ *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  ಯದಿ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ಯದಿ(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    ಪೂರ್ಣಾಙ್ಕ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಯದಿ(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      ಗಚ್ಛ fail;
    }
  }

  /* Now set the new attributes... */
  ಯದಿ(!strncmp(cmd, "chgrp", 5)) {
    ಧ್ರುವ ವರ್ಣ *p = sshc->quote_path1;
    curl_off_t gid;
    ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ಅನ್ಯಥಾ ಯದಿ(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      ಗಚ್ಛ fail;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ಧ್ರುವ ವರ್ಣ *p = sshc->quote_path1;
    /* permissions are octal */
    ಯದಿ(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      ಗಚ್ಛ fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "chown", 5)) {
    ಧ್ರುವ ವರ್ಣ *p = sshc->quote_path1;
    curl_off_t uid;
    ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ಅನ್ಯಥಾ ಯದಿ(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      ಗಚ್ಛ fail;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    ಯದಿ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ಯದಿ SIZEOF_TIME_T > SIZEOF_LONG
    ಯದಿ(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    ಯದಿ(fail)
      ಗಚ್ಛ fail;
    ಯದಿ(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    ಅನ್ಯಥಾ /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
}

ಸ್ಥಿರ CURLcode sftp_download_stat(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ssh_conn *sshc,
                                   ಸಂರಚನಾ SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಯದಿ(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_SIZE) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    data->req.size = -1;
    data->req.maxdownload = -1;
    Curl_pgrsSetDownloadSize(data, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  ಅನ್ಯಥಾ {
    curl_off_t size = attrs.filesize;

    ಯದಿ(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_DOWNLOAD_RESUME;
    }
    ಯದಿ(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  ಯದಿ(data->state.resume_from) {
    ಯದಿ(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ಯದಿ((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    ಅನ್ಯಥಾ {
      ಯದಿ((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = attrs.filesize - data->state.resume_from;
    data->req.maxdownload = attrs.filesize - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, attrs.filesize - data->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }

  /* Setup the actual download */
  ಯದಿ(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode sftp_readdir(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ssh_conn *sshc,
                             ಸಂರಚನಾ SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಯದಿ(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    ಯದಿ(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      ಯದಿ(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಅನ್ಯಥಾ {
      result = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      ಯದಿ(!result) {
        ಯದಿ((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        ಅನ್ಯಥಾ {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  ಅನ್ಯಥಾ {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_init(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ssh_conn *sshc)
{
  CURLcode result;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  result = ssh_force_knownhost_ಕೀಲಿ_type(data, sshc);
  ಯದಿ(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_S_STARTUP);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_startup(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ssh_conn *sshc)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc) {
    ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
    (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_ಸಂದೇಶ);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_hostkey(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode result = ssh_check_fingerprint(data, sshc);
  ಯದಿ(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_authlist(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ssh_conn *sshc)
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
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ವರ್ಣ *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(strlen(user)));

  ಯದಿ(!sshc->authlist) {
    ಪೂರ್ಣಾಙ್ಕ rc;
    ಯದಿ(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    ಪ್ರತ್ಯಾಗಮ libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_pkey(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ವರ್ಣ *user = Curl_creds_user(conn->creds);
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_ಕೀಲಿ, sshc->priv_ಕೀಲಿ, sshc->passphrase ? sshc->passphrase : "");
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ಅನ್ಯಥಾ {
    ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
    ವರ್ಣ unknown[] = "Reason unknown (-1)";
    ಯದಿ(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_ಸಂದೇಶ = unknown;
    }
    ಅನ್ಯಥಾ {
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_ಸಂದೇಶ);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_pass_init(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_pass(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssh_conn *sshc)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ವರ್ಣ *user = Curl_creds_user(conn->creds);
  ಧ್ರುವ ವರ್ಣ *passwd = Curl_creds_passwd(conn->creds);
  ಪೂರ್ಣಾಙ್ಕ rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 ರಿಕ್ತ);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN) {
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  }
  ಯದಿ(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_host_init(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_agent_init(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  ಯದಿ((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    ಯದಿ(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      ಯದಿ(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    ಯದಿ(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
    }
    ಅನ್ಯಥಾ {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_agent_list(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_agent_list_identities(sshc->ssh_agent);

  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  ಯದಿ(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_agent(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_prev_identity);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    ಯದಿ(rc < 0) {
      ಯದಿ(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
  }

  ಯದಿ(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  ಅನ್ಯಥಾ ಯದಿ(rc == 1)
    infof(data, "SSH: no agent identity would match");

  ಯದಿ(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_ಕೀಲಿ_init(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_ಕೀಲಿ);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_ಕೀಲಿ(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಧ್ರುವ ವರ್ಣ *user = Curl_creds_user(conn->creds);
  ಪೂರ್ಣಾಙ್ಕ rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_LOGIN_DENIED;
}

ಸ್ಥಿರ CURLcode ssh_state_auth_done(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssh_conn *sshc)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಯದಿ(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    ಪ್ರತ್ಯಾಗಮ CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ಯದಿ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_init(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  ಯದಿ(!sshc->sftp_session) {
    ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
    ಯದಿ(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
    failf(data, "Failure initializing sftp session: %s", err_ಸಂದೇಶ);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_realpath(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ssh_conn *sshc,
                                        ಸಂರಚನಾ SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  ಯದಿ(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    ಯದಿ(!sshc->homedir)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    ಯದಿ(!data->state.most_recent_ftp_entrypath)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  ಅನ್ಯಥಾ {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    ಯದಿ(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    ಅನ್ಯಥಾ
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (ಪೂರ್ಣಾಙ್ಕ)result);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_init(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ ssh_conn *sshc,
                                          ಸಂರಚನಾ SSHPROTO *sshp)
{
  CURLcode result;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  ಯದಿ(result) {
    myssh_to(data, sshc, SSH_STOP);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_postquote_init(ಸಂರಚನಾ Curl_easy *data,
                                              ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_STOP);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ ssh_conn *sshc,
                                     ಸಂರಚನಾ SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  ಯದಿ(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_next_quote(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  ಯದಿ(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    ಅನ್ಯಥಾ {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_stat(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ ssh_conn *sshc,
                                          ಸಂರಚನಾ SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  ಯದಿ(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_setstat(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ ssh_conn *sshc,
                                             ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_symlink(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_mkdir(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)data->set.new_directory_perms);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_rename(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_rmdir(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_unlink(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_quote_statvfs(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  ಪೂರ್ಣಾಙ್ಕ rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
  }
  ಅನ್ಯಥಾ ಯದಿ(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#ಅನ್ಯಥಾ
#define CURL_LIBSSH2_VFS_SIZE_MASK "llu"
#endif
    CURLcode result;
    ವರ್ಣ *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_blocks, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    ಯದಿ(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    ಯದಿ(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_create_dirs_mkdir(ಸಂರಚನಾ Curl_easy *data,
                                                 ಸಂರಚನಾ ssh_conn *sshc,
                                                 ಸಂರಚನಾ SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)data->set.new_directory_perms);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ಯದಿ(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    ಯದಿ((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      ಪ್ರತ್ಯಾಗಮ sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir_init(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ ssh_conn *sshc,
                                            ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  ಯದಿ(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  ಯದಿ(!sshc->sftp_handle) {
    unsigned long sftperr;
    ಯದಿ(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    ಪ್ರತ್ಯಾಗಮ sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir_link(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ ssh_conn *sshc,
                                            ಸಂರಚನಾ SSHPROTO *sshp)
{
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned ಪೂರ್ಣಾಙ್ಕ)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  ಯದಿ(rc < 0)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  ಯದಿ(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_download_init(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ ssh_conn *sshc,
                                            ಸಂರಚನಾ SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, ಪರಿಮಾಣ(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  ಯದಿ(!sshc->ssh_channel) {
    ಪೂರ್ಣಾಙ್ಕ ssh_err;
    ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;

    ಯದಿ(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
    failf(data, "%s", err_ಸಂದೇಶ);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    ಪ್ರತ್ಯಾಗಮ libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_close(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ ssh_conn *sshc,
                                     ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    ಯದಿ(rc < 0) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_ಸಂದೇಶ);
    }
    sshc->sftp_handle = ರಿಕ್ತ;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  ಯದಿ(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_STOP);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_shutdown(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  ಯದಿ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    ಯದಿ(rc < 0) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_ಸಂದೇಶ);
    }
    sshc->sftp_handle = ರಿಕ್ತ;
  }
  ಯದಿ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    ಯದಿ(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = ರಿಕ್ತ;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_download_init(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ ssh_conn *sshc,
                                             ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  ಯದಿ(!sshc->sftp_handle) {
    unsigned long sftperr;
    ಯದಿ(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    ಪ್ರತ್ಯಾಗಮ sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_upload_init(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ ssh_conn *sshc,
                                          ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (ಪೂರ್ಣಾಙ್ಕ)data->set.new_file_perms,
                       (libssh2_int64_t)data->state.infilesize, 0, 0);
  ಯದಿ(!sshc->ssh_channel) {
    ಪೂರ್ಣಾಙ್ಕ ssh_err;
    ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
    CURLcode result;
    ಯದಿ(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
    failf(data, "%s", err_ಸಂದೇಶ);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    ಯದಿ(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    ಪ್ರತ್ಯಾಗಮ result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_session_disconnect(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  ಯದಿ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    ಯದಿ(rc < 0) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ಸಂದೇಶ);
    }
    sshc->ssh_channel = ರಿಕ್ತ;
  }

  ಯದಿ(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    ಯದಿ(rc < 0) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_ಸಂದೇಶ);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode sshc_cleanup(ಸಂರಚನಾ ssh_conn *sshc, ಸಂರಚನಾ Curl_easy *data,
                             bool block)
{
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = ರಿಕ್ತ;
  }

  ಯದಿ(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    ಯದಿ((rc < 0) && data) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_ಸಂದೇಶ);
    }
    ಯದಿ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = ರಿಕ್ತ;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = ರಿಕ್ತ;
    sshc->sshagent_prev_identity = ರಿಕ್ತ;
  }

  ಯದಿ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ಯದಿ((rc < 0) && data) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_ಸಂದೇಶ);
    }
    ಯದಿ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    sshc->sftp_handle = ರಿಕ್ತ;
  }

  ಯದಿ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ಯದಿ((rc < 0) && data) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ಸಂದೇಶ);
    }
    ಯದಿ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    sshc->ssh_channel = ರಿಕ್ತ;
  }

  ಯದಿ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ಯದಿ((rc < 0) && data) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_ಸಂದೇಶ);
    }
    ಯದಿ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    sshc->sftp_session = ರಿಕ್ತ;
  }

  ಯದಿ(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    ಯದಿ((rc < 0) && data) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session, &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_ಸಂದೇಶ);
    }
    ಯದಿ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

    sshc->ssh_session = ರಿಕ್ತ;
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

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_getinfo(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_filetime(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ssh_conn *sshc,
                                        ಸಂರಚನಾ SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  ಯದಿ(rc == 0)
    data->info.filetime = (time_t)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_trans_init(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ ssh_conn *sshc,
                                          ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ಅನ್ಯಥಾ ಯದಿ(sshp) {
    size_t plen = strlen(sshp->path);
    ಯದಿ(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    ಅನ್ಯಥಾ
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_upload_init(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ ssh_conn *sshc,
                                           ಸಂರಚನಾ SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  ಯದಿ(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_create_dirs_init(ಸಂರಚನಾ Curl_easy *data,
                                                ಸಂರಚನಾ ssh_conn *sshc,
                                                ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_create_dirs(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ ssh_conn *sshc,
                                           ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ಯದಿ(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc,
                                       ಸಂರಚನಾ SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  ಯದಿ(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir_bottom(ಸಂರಚನಾ Curl_easy *data,
                                              ಸಂರಚನಾ ssh_conn *sshc,
                                              ಸಂರಚನಾ SSHPROTO *sshp)
{
  CURLcode result;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  ಯದಿ(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  ಯದಿ(result) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(data, sshc, SSH_STOP);
  }
  ಅನ್ಯಥಾ {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_readdir_done(ಸಂರಚನಾ Curl_easy *data,
                                            ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;

  sshc->sftp_handle = ರಿಕ್ತ;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_sftp_download_stat(ಸಂರಚನಾ Curl_easy *data,
                                             ಸಂರಚನಾ ssh_conn *sshc,
                                             ಸಂರಚನಾ SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  ಯದಿ(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_trans_init(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ ssh_conn *sshc,
                                         ಸಂರಚನಾ SSHPROTO *sshp)
{
  CURLcode result;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  ಯದಿ(result) {
    myssh_to(data, sshc, SSH_STOP);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(data->state.upload) {
    ಯದಿ(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      ಪ್ರತ್ಯಾಗಮ CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_done(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_send_eof(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->ssh_channel) {
    ಪೂರ್ಣಾಙ್ಕ rc = libssh2_channel_send_eof(sshc->ssh_channel);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    ಯದಿ(rc) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_ಸಂದೇಶ);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_wait_eof(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->ssh_channel) {
    ಪೂರ್ಣಾಙ್ಕ rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    ಯದಿ(rc) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_ಸಂದೇಶ);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_wait_close(ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->ssh_channel) {
    ಪೂರ್ಣಾಙ್ಕ rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    ಯದಿ(rc) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_ಸಂದೇಶ);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_scp_channel_free(
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->ssh_channel) {
    ಪೂರ್ಣಾಙ್ಕ rc = libssh2_channel_free(sshc->ssh_channel);
    ಯದಿ(rc == LIBSSH2_ERROR_EAGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    ಯದಿ(rc < 0) {
      ವರ್ಣ *err_ಸಂದೇಶ = ರಿಕ್ತ;
      (ಶೂನ್ಯ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ಸಂದೇಶ, ರಿಕ್ತ, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ಸಂದೇಶ);
    }
    sshc->ssh_channel = ರಿಕ್ತ;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssh_state_session_free(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  memset(sshc, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ಸ್ಥಿರ CURLcode ssh_statemachine(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ssh_conn *sshc,
                                 ಸಂರಚನಾ SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode result = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  ಕುರು {
    ವಿಕಲ್ಪ(sshc->state) {
    ಪಕ್ಷ SSH_INIT:
      result = ssh_state_init(data, sshc);
      ಯದಿ(result)
        ಭಙ್ಗ;
      FALLTHROUGH();

    ಪಕ್ಷ SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      ಯದಿ(result)
        ಭಙ್ಗ;
      FALLTHROUGH();

    ಪಕ್ಷ SSH_HOSTKEY:
      result = ssh_state_hostkey(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTHLIST:
      result = ssh_state_authlist(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_PKEY:
      result = ssh_state_auth_pkey(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_PASS_INIT:
      result = ssh_state_auth_pass_init(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_PASS:
      result = ssh_state_auth_pass(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_HOST_INIT:
      result = ssh_state_auth_host_init(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_HOST:
      myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_AGENT_INIT:
      result = ssh_state_auth_agent_init(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_AGENT_LIST:
      result = ssh_state_auth_agent_list(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_AGENT:
      result = ssh_state_auth_agent(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_ಕೀಲಿ_INIT:
      result = ssh_state_auth_ಕೀಲಿ_init(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_ಕೀಲಿ:
      result = ssh_state_auth_ಕೀಲಿ(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_AUTH_DONE:
      result = ssh_state_auth_done(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_INIT:
      result = ssh_state_sftp_init(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_REALPATH:
      result = ssh_state_sftp_realpath(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE_INIT:
      result = ssh_state_sftp_quote_init(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_POSTQUOTE_INIT:
      result = ssh_state_sftp_postquote_init(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE:
      result = ssh_state_sftp_quote(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_NEXT_QUOTE:
      result = ssh_state_sftp_next_quote(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE_STAT:
      result = ssh_state_sftp_quote_stat(data, sshc, sshp, block);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE_SETSTAT:
      result = ssh_state_sftp_quote_setstat(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE_SYMLINK:
      result = ssh_state_sftp_quote_symlink(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE_MKDIR:
      result = ssh_state_sftp_quote_mkdir(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE_RENAME:
      result = ssh_state_sftp_quote_rename(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE_RMDIR:
      result = ssh_state_sftp_quote_rmdir(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE_UNLINK:
      result = ssh_state_sftp_quote_unlink(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_QUOTE_STATVFS:
      result = ssh_state_sftp_quote_statvfs(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_GETINFO:
      result = ssh_state_sftp_getinfo(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_FILETIME:
      result = ssh_state_sftp_filetime(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_TRANS_INIT:
      result = ssh_state_sftp_trans_init(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_UPLOAD_INIT:
      result = ssh_state_sftp_upload_init(data, sshc, sshp, block);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_CREATE_DIRS_INIT:
      result = ssh_state_sftp_create_dirs_init(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_CREATE_DIRS:
      result = ssh_state_sftp_create_dirs(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_CREATE_DIRS_MKDIR:
      result = ssh_state_sftp_create_dirs_mkdir(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_READDIR_INIT:
      result = ssh_state_sftp_readdir_init(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_READDIR:
      result = ssh_state_sftp_readdir(data, sshc, sshp, block);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_READDIR_LINK:
      result = ssh_state_sftp_readdir_link(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_READDIR_BOTTOM:
      result = ssh_state_sftp_readdir_bottom(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_READDIR_DONE:
      result = ssh_state_sftp_readdir_done(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_DOWNLOAD_INIT:
      result = ssh_state_sftp_download_init(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_DOWNLOAD_STAT:
      result = ssh_state_sftp_download_stat(data, sshc, sshp, block);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_CLOSE:
      result = ssh_state_sftp_close(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SFTP_SHUTDOWN:
      result = ssh_state_sftp_shutdown(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SCP_TRANS_INIT:
      result = ssh_state_scp_trans_init(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SCP_UPLOAD_INIT:
      result = ssh_state_scp_upload_init(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SCP_DOWNLOAD_INIT:
      result = ssh_state_scp_download_init(data, sshc, sshp);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SCP_DONE:
      result = ssh_state_scp_done(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SCP_SEND_EOF:
      result = ssh_state_scp_send_eof(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SCP_WAIT_EOF:
      result = ssh_state_scp_wait_eof(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SCP_WAIT_CLOSE:
      result = ssh_state_scp_wait_close(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SCP_CHANNEL_FREE:
      result = ssh_state_scp_channel_free(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SESSION_DISCONNECT:
      result = ssh_state_session_disconnect(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_SESSION_FREE:
      result = ssh_state_session_free(data, sshc);
      ಭಙ್ಗ;

    ಪಕ್ಷ SSH_QUIT:
    ಸ್ವಭಾವ:
      /* internal error */
      myssh_to(data, sshc, SSH_STOP);
      ಭಙ್ಗ;
    }

  } ಯಾವತ್(!result && (sshc->state != SSH_STOP) && !*block);

  ಯದಿ(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (ಪೂರ್ಣಾಙ್ಕ)result, *block);

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ssh_block2waitfor(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ssh_conn *sshc,
                              bool block)
{
  ಪೂರ್ಣಾಙ್ಕ dir = 0;
  (ಶೂನ್ಯ)data;
  ಯದಿ(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    ಯದಿ(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  ಯದಿ(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode ssh_multi_statemach(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ಸಂರಚನಾ SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  ಯದಿ(!sshc || !sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಕುರು {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } ಯಾವತ್(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssh_block_statemach(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssh_conn *sshc,
                                    ಸಂರಚನಾ SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ curltime start = *Curl_pgrs_now(data);

  ಯಾವತ್((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t ಎಡ_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    ಯದಿ(result)
      ಭಙ್ಗ;

    ಯದಿ(!disconnect) {
      result = Curl_pgrsCheck(data);
      ಯದಿ(result)
        ಭಙ್ಗ;

      ಎಡ_ms = Curl_timeleft_ms(data);
      ಯದಿ(ಎಡ_ms < 0) {
        failf(data, "Operation timed out");
        ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      ಭಙ್ಗ;
    }

    ಯದಿ(block) {
      ಪೂರ್ಣಾಙ್ಕ dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      ಯದಿ(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      ಯದಿ(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (ಶೂನ್ಯ)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!ಎಡ_ms || ಎಡ_ms > 1000) ? 1000 : ಎಡ_ms);
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_easy_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ SSHPROTO *sshp = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ ssh_conn *sshc = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  sshc_cleanup(sshc, ರಿಕ್ತ, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ಸ್ಥಿರ CURLcode ssh_setup_connection(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ ssh_conn *sshc;
  ಸಂರಚನಾ SSHPROTO *sshp;
  (ಶೂನ್ಯ)conn;

  sshc = curlx_calloc(1, ಪರಿಮಾಣ(*sshc));
  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಯದಿ(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, ಪರಿಮಾಣ(*sshp));
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  ಯದಿ(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತ್ಯಾಗಮ Curl_ssh_setup_pkey(data, sshc);
}

ಸ್ಥಿರ Curl_recv scp_recv, sftp_recv;
ಸ್ಥಿರ Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
ಸ್ಥಿರ ssize_t ssh_tls_recv(libssh2_socket_t sock, ಶೂನ್ಯ *buffer,
                            size_t length, ಪೂರ್ಣಾಙ್ಕ flags, ಶೂನ್ಯ **abstract)
{
  ಸಂರಚನಾ Curl_easy *data = (ಸಂರಚನಾ Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nread;
  CURLcode result;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ಶೂನ್ಯ)flags;

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  ಯದಿ(result == CURLE_AGAIN)
    ಪ್ರತ್ಯಾಗಮ -EAGAIN; /* magic return code for libssh2 */
  ಅನ್ಯಥಾ ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (ಧ್ರುವ ವರ್ಣ *)buffer, nread);
  ಪ್ರತ್ಯಾಗಮ (ssize_t)nread;
}

ಸ್ಥಿರ ssize_t ssh_tls_send(libssh2_socket_t sock, ಧ್ರುವ ಶೂನ್ಯ *buffer,
                            size_t length, ಪೂರ್ಣಾಙ್ಕ flags, ಶೂನ್ಯ **abstract)
{
  ಸಂರಚನಾ Curl_easy *data = (ಸಂರಚನಾ Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nwrite;
  CURLcode result;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ಶೂನ್ಯ)flags;

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  ಯದಿ(result == CURLE_AGAIN)
    ಪ್ರತ್ಯಾಗಮ -EAGAIN; /* magic return code for libssh2 */
  ಅನ್ಯಥಾ ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (ಧ್ರುವ ವರ್ಣ *)buffer, nwrite);
  ಪ್ರತ್ಯಾಗಮ (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ಸ್ಥಿರ CURLcode ssh_connect(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result;

#ಯದಿ LIBSSH2_VERSION_ಅಂಕೆ >= 0x010b00
  {
    ಧ್ರುವ ವರ್ಣ *crypto_str;
    ವಿಕಲ್ಪ(libssh2_crypto_engine()) {
    ಪಕ್ಷ libssh2_gcrypt:
      crypto_str = "libgcrypt";
      ಭಙ್ಗ;
    ಪಕ್ಷ libssh2_mbedtls:
      crypto_str = "mbedTLS";
      ಭಙ್ಗ;
    ಪಕ್ಷ libssh2_openssl:
      crypto_str = "openssl compatible";
      ಭಙ್ಗ;
    ಪಕ್ಷ libssh2_os400qc3:
      crypto_str = "OS400QC3";
      ಭಙ್ಗ;
    ಪಕ್ಷ libssh2_wincng:
      crypto_str = "WinCNG";
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      crypto_str = ರಿಕ್ತ;
      ಭಙ್ಗ;
    }
    ಯದಿ(crypto_str)
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  infof(data, "SSH: user '%s'", Curl_creds_user(conn->creds));
#ifdef CURL_LIBSSH2_DEBUG
  infof(data, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#endif /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, data);

  ಯದಿ(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#ಯದಿ LIBSSH2_VERSION_ಅಂಕೆ >= 0x010B00
  ಯದಿ(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  ಯದಿ(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#ಯದಿ LIBSSH2_VERSION_ಅಂಕೆ >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತೆಗೆ
#endif
#ಅನ್ಯಥಾ
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    ಸಙ್ಘ receive {
      ಶೂನ್ಯ *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, ಶೂನ್ಯ *, size_t, ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ **);
    };
    ಸಙ್ಘ transfer {
      ಶೂನ್ಯ *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ಧ್ರುವ ಶೂನ್ಯ *, size_t, ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ **);
    };
    ಸಙ್ಘ receive sshrecv;
    ಸಙ್ಘ transfer sshsend;

    sshrecv.recvptr = ssh_tls_recv;
    sshsend.sendptr = ssh_tls_send;

    infof(data, "SSH: using HTTPS proxy");
    libssh2_session_callback_set(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_set(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#endif

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#endif /* CURL_DISABLE_PROXY */
  ಯದಿ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ಅನ್ಯಥಾ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  ಯದಿ(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  ಯದಿ(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    ಪೂರ್ಣಾಙ್ಕ rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    ಯದಿ(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = ರಿಕ್ತ;
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    ಯದಿ(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (ಪೂರ್ಣಾಙ್ಕ)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ಸ್ಥಿರ CURLcode scp_perform(ಸಂರಚನಾ Curl_easy *data,
                            bool *connected,
                            bool *dophase_done)
{
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ಯದಿ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* called from multi.c while DOing */
ಸ್ಥಿರ CURLcode scp_doing(ಸಂರಚನಾ Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  ಯದಿ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ಸ್ಥಿರ CURLcode scp_disconnect(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ಸಂರಚನಾ SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (ಶೂನ್ಯ)dead_connection;

  ಯದಿ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  ಯದಿ(sshc)
    ಪ್ರತ್ಯಾಗಮ sshc_cleanup(sshc, data, TRUE);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ಸ್ಥಿರ CURLcode ssh_done(ಸಂರಚನಾ Curl_easy *data, CURLcode status)
{
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  ಸಂರಚನಾ SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  ಯದಿ(!sshc || !sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  ಅನ್ಯಥಾ
    result = status;

  ಯದಿ(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ಪ್ರತ್ಯಾಗಮ CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode scp_done(ಸಂರಚನಾ Curl_easy *data, CURLcode status,
                         bool premature)
{
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (ಶೂನ್ಯ)premature;

  ಯದಿ(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  ಪ್ರತ್ಯಾಗಮ ssh_done(data, status);
}

ಸ್ಥಿರ CURLcode scp_send(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                         ಧ್ರುವ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (ಶೂನ್ಯ)sockindex; /* we only support SCP on the fixed known primary socket */
  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ಧ್ರುವ ವರ್ಣ *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  ಯದಿ(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  ಅನ್ಯಥಾ ಯದಿ(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((ಪೂರ್ಣಾಙ್ಕ)nwritten);
  ಅನ್ಯಥಾ
    *pnwritten = (size_t)nwritten;

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode scp_recv(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                         ವರ್ಣ *mem, size_t len, size_t *pnread)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (ಶೂನ್ಯ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  ಯದಿ(nread == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  ಅನ್ಯಥಾ ಯದಿ(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((ಪೂರ್ಣಾಙ್ಕ)nread);
  ಅನ್ಯಥಾ
    *pnread = (size_t)nread;

  ಪ್ರತ್ಯಾಗಮ result;
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

ಸ್ಥಿರ CURLcode sftp_perform(ಸಂರಚನಾ Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ಯದಿ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* called from multi.c while DOing */
ಸ್ಥಿರ CURLcode sftp_doing(ಸಂರಚನಾ Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  ಯದಿ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ಸ್ಥಿರ CURLcode sftp_disconnect(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn, bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ಸಂರಚನಾ SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (ಶೂನ್ಯ)dead_connection;

  ಯದಿ(sshc) {
    ಯದಿ(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode sftp_done(ಸಂರಚನಾ Curl_easy *data, CURLcode status,
                          bool premature)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ಯದಿ(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    ಯದಿ(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  ಪ್ರತ್ಯಾಗಮ ssh_done(data, status);
}

/* return number of sent bytes */
ಸ್ಥಿರ CURLcode sftp_send(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                          ಧ್ರುವ uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (ಶೂನ್ಯ)sockindex;
  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ಧ್ರುವ ವರ್ಣ *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  ಯದಿ(nwrite == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  ಅನ್ಯಥಾ ಯದಿ(nwrite < LIBSSH2_ERROR_NONE)
    ಪ್ರತ್ಯಾಗಮ libssh2_session_error_to_CURLE((ಪೂರ್ಣಾಙ್ಕ)nwrite);
  *pnwritten = (size_t)nwrite;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ಸ್ಥಿರ CURLcode sftp_recv(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                          ವರ್ಣ *mem, size_t len, size_t *pnread)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (ಶೂನ್ಯ)sockindex;
  *pnread = 0;

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  ಯದಿ(nread == LIBSSH2_ERROR_EAGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  ಅನ್ಯಥಾ ಯದಿ(nread < 0)
    ಪ್ರತ್ಯಾಗಮ libssh2_session_error_to_CURLE((ಪೂರ್ಣಾಙ್ಕ)nread);

  *pnread = (size_t)nread;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
ಸ್ಥಿರ CURLcode ssh_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  ಯದಿ(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  ಅನ್ಯಥಾ
    result = sftp_perform(data, &connected, done);

  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_ssh_init(ಶೂನ್ಯ)
{
  ಯದಿ(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssh_cleanup(ಶೂನ್ಯ)
{
  libssh2_exit();
}

ಶೂನ್ಯ Curl_ssh_version(ವರ್ಣ *buffer, size_t buflen)
{
  (ಶೂನ್ಯ)curl_msnprintf(buffer, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ssh_attach(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ connectdata *conn)
{
  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  ಯದಿ(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    ಯದಿ(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      ಶೂನ್ಯ **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = data;
    }
  }
}

/*
 * SCP protocol handler.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_scp = {
  ssh_setup_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_done,                             /* done */
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
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_sftp = {
  ssh_setup_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  sftp_done,                            /* done */
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

#endif /* USE_LIBSSH2 */
