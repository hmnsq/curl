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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "vssh/ssh.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "parsedate.h" /* for the week day and month names */
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "vssh/vssh.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curlx/base64.h" /* for curlx_base64_encode() */

സ്ഥിതം സ്ഥിരം അക്ഷരം *sftp_libssh2_strerror(unsigned long err)
{
  തിരഞ്ഞെടുപ്പ്(err) {
  അവസ്ഥ LIBSSH2_FX_NO_SUCH_FILE:
    തിരികെ "No such file or directory";

  അവസ്ഥ LIBSSH2_FX_PERMISSION_DENIED:
    തിരികെ "Permission denied";

  അവസ്ഥ LIBSSH2_FX_FAILURE:
    തിരികെ "Operation failed";

  അവസ്ഥ LIBSSH2_FX_BAD_MESSAGE:
    തിരികെ "Bad message from SFTP server";

  അവസ്ഥ LIBSSH2_FX_NO_CONNECTION:
    തിരികെ "Not connected to SFTP server";

  അവസ്ഥ LIBSSH2_FX_CONNECTION_LOST:
    തിരികെ "Connection to SFTP server lost";

  അവസ്ഥ LIBSSH2_FX_OP_UNSUPPORTED:
    തിരികെ "Operation not supported by SFTP server";

  അവസ്ഥ LIBSSH2_FX_INVALID_HANDLE:
    തിരികെ "Invalid handle";

  അവസ്ഥ LIBSSH2_FX_NO_SUCH_PATH:
    തിരികെ "No such file or directory";

  അവസ്ഥ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    തിരികെ "File already exists";

  അവസ്ഥ LIBSSH2_FX_WRITE_PROTECT:
    തിരികെ "File is write protected";

  അവസ്ഥ LIBSSH2_FX_NO_MEDIA:
    തിരികെ "No media";

  അവസ്ഥ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    തിരികെ "Disk full";

  അവസ്ഥ LIBSSH2_FX_QUOTA_EXCEEDED:
    തിരികെ "User quota exceeded";

  അവസ്ഥ LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    തിരികെ "Unknown principal";

  അവസ്ഥ LIBSSH2_FX_LOCK_CONFLICT:
    തിരികെ "File lock conflict";

  അവസ്ഥ LIBSSH2_FX_DIR_NOT_EMPTY:
    തിരികെ "Directory not empty";

  അവസ്ഥ LIBSSH2_FX_NOT_A_DIRECTORY:
    തിരികെ "Not a directory";

  അവസ്ഥ LIBSSH2_FX_INVALID_FILENAME:
    തിരികെ "Invalid filename";

  അവസ്ഥ LIBSSH2_FX_LINK_LOOP:
    തിരികെ "Link points to itself";
  }
  തിരികെ "Unknown error in libssh2";
}

സ്ഥിതം ശൂന്യം kbd_callback(സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ name_len,
                         സ്ഥിരം അക്ഷരം *instruction, പൂർണ്ണസംഖ്യ instruction_len,
                         പൂർണ്ണസംഖ്യ അക്കം_prompts,
                         സ്ഥിരം LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         ശൂന്യം **abstract)
{
  ഘടന Curl_easy *data = (ഘടന Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", അക്കം_prompts);
#അല്ലെങ്കിൽ
  (ശൂന്യം)name;
  (ശൂന്യം)name_len;
  (ശൂന്യം)instruction;
  (ശൂന്യം)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  എങ്കിൽ(അക്കം_prompts == 1) {
    ഘടന connectdata *conn = data->conn;
    സ്ഥിരം അക്ഷരം *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (ശൂന്യം)prompts;
} /* kbd_callback */

സ്ഥിതം CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  തിരഞ്ഞെടുപ്പ്(err) {
  അവസ്ഥ LIBSSH2_FX_OK:
    തിരികെ CURLE_OK;

  അവസ്ഥ LIBSSH2_FX_NO_SUCH_FILE:
  അവസ്ഥ LIBSSH2_FX_NO_SUCH_PATH:
    തിരികെ CURLE_REMOTE_FILE_NOT_FOUND;

  അവസ്ഥ LIBSSH2_FX_PERMISSION_DENIED:
  അവസ്ഥ LIBSSH2_FX_WRITE_PROTECT:
  അവസ്ഥ LIBSSH2_FX_LOCK_CONFLICT:
    തിരികെ CURLE_REMOTE_ACCESS_DENIED;

  അവസ്ഥ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  അവസ്ഥ LIBSSH2_FX_QUOTA_EXCEEDED:
    തിരികെ CURLE_REMOTE_DISK_FULL;

  അവസ്ഥ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    തിരികെ CURLE_REMOTE_FILE_EXISTS;

  അവസ്ഥ LIBSSH2_FX_DIR_NOT_EMPTY:
    തിരികെ CURLE_QUOTE_ERROR;

  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }

  തിരികെ CURLE_SSH;
}

സ്ഥിതം CURLcode libssh2_session_error_to_CURLE(പൂർണ്ണസംഖ്യ err)
{
  തിരഞ്ഞെടുപ്പ്(err) {
  /* Ordered by order of appearance in libssh2.h */
  അവസ്ഥ LIBSSH2_ERROR_NONE:
    തിരികെ CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  അവസ്ഥ LIBSSH2_ERROR_SCP_PROTOCOL:
    തിരികെ CURLE_REMOTE_FILE_NOT_FOUND;

  അവസ്ഥ LIBSSH2_ERROR_SOCKET_NONE:
    തിരികെ CURLE_COULDNT_CONNECT;

  അവസ്ഥ LIBSSH2_ERROR_ALLOC:
    തിരികെ CURLE_OUT_OF_MEMORY;

  അവസ്ഥ LIBSSH2_ERROR_SOCKET_SEND:
    തിരികെ CURLE_SEND_ERROR;

  അവസ്ഥ LIBSSH2_ERROR_HOSTKEY_INIT:
  അവസ്ഥ LIBSSH2_ERROR_HOSTKEY_SIGN:
  അവസ്ഥ LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  അവസ്ഥ LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    തിരികെ CURLE_PEER_FAILED_VERIFICATION;

  അവസ്ഥ LIBSSH2_ERROR_PASSWORD_EXPIRED:
    തിരികെ CURLE_LOGIN_DENIED;

  അവസ്ഥ LIBSSH2_ERROR_SOCKET_TIMEOUT:
  അവസ്ഥ LIBSSH2_ERROR_TIMEOUT:
    തിരികെ CURLE_OPERATION_TIMEDOUT;

  അവസ്ഥ LIBSSH2_ERROR_EAGAIN:
    തിരികെ CURLE_AGAIN;
  }

  തിരികെ CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

സ്ഥിതം LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (ശൂന്യം)abstract;
  തിരികെ Curl_cmalloc(എണ്ണം);
}

സ്ഥിതം LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (ശൂന്യം)abstract;
  തിരികെ Curl_crealloc(ptr, എണ്ണം);
}

സ്ഥിതം LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (ശൂന്യം)abstract;
  എങ്കിൽ(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sshkeycallback(CURL *easy,
                          സ്ഥിരം ഘടന curl_khkey *knownkey, /* known */
                          സ്ഥിരം ഘടന curl_khkey *foundkey, /* found */
                          എണ്ണൽ curl_khmatch match,
                          ശൂന്യം *clientp)
{
  (ശൂന്യം)easy;
  (ശൂന്യം)knownkey;
  (ശൂന്യം)foundkey;
  (ശൂന്യം)clientp;

  /* we only allow perfect matches, and we reject everything else */
  തിരികെ (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

സ്ഥിതം എണ്ണൽ curl_khtype convert_ssh2_keytype(പൂർണ്ണസംഖ്യ sshkeytype)
{
  എണ്ണൽ curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  തിരഞ്ഞെടുപ്പ്(sshkeytype) {
  അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    നിർത്തുക;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    നിർത്തുക;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    നിർത്തുക;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    നിർത്തുക;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    നിർത്തുക;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    നിർത്തുക;
#endif
  }
  തിരികെ keytype;
}

സ്ഥിതം CURLcode ssh_knownhost(ഘടന Curl_easy *data,
                              ഘടന ssh_conn *sshc)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന libssh2_knownhost *host = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *remotekey = ശൂന്യ;
  പൂർണ്ണസംഖ്യ keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  പൂർണ്ണസംഖ്യ keybit = 0;
  പൂർണ്ണസംഖ്യ sshkeytype = 0;
  size_t keylen = 0;
  പൂർണ്ണസംഖ്യ rc = 0;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
    തിരികെ CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  എങ്കിൽ(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    ഘടന Curl_mapi_guard guard;
    എണ്ണൽ curl_khmatch keymatch;
    curl_sshkeycallback func =
      data->set.ssh_keyfunc ? data->set.ssh_keyfunc : sshkeycallback;
    ഘടന curl_khkey knownkey;
    ഘടന curl_khkey *knownkeyp = ശൂന്യ;
    ഘടന curl_khkey foundkey;

    തിരഞ്ഞെടുപ്പ്(sshkeytype) {
    അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_താക്കോൽ_SSHRSA;
      നിർത്തുക;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_താക്കോൽ_SSHDSS;
      നിർത്തുക;
#endif
    അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_താക്കോൽ_ECDSA_256;
      നിർത്തുക;
    അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_താക്കോൽ_ECDSA_384;
      നിർത്തുക;
    അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_താക്കോൽ_ECDSA_521;
      നിർത്തുക;
    അവസ്ഥ LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_താക്കോൽ_ED25519;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      നിർത്തുക;
    }
    എങ്കിൽ(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    അല്ലെങ്കിൽ {
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
            host->താക്കോൽ : "<none>");

      /* setup 'knownkey' */
      എങ്കിൽ(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.താക്കോൽ = host->താക്കോൽ;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.താക്കോൽ = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (എണ്ണൽ curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  അല്ലെങ്കിൽ {
    /* no remotekey means failure! */
    infof(data, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  തിരഞ്ഞെടുപ്പ്(rc) {
  സ്ഥിരസ്ഥിതി: /* unknown return codes is the same as reject */
  അവസ്ഥ CURLKHSTAT_REJECT:
    infof(data, "SSH: knownhost check failed");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  അവസ്ഥ CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    result = CURLE_PEER_FAILED_VERIFICATION;
    നിർത്തുക;
  അവസ്ഥ CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    എങ്കിൽ(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  അവസ്ഥ CURLKHSTAT_FINE:
  അവസ്ഥ CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    എങ്കിൽ(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      പൂർണ്ണസംഖ്യ addrc;
      സ്ഥിരം അക്ഷരം *hostbuf;
      അക്ഷരം *hostport = ശൂന്യ;
      എങ്കിൽ(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        എങ്കിൽ(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
      }
      അല്ലെങ്കിൽ
        hostbuf = conn->origin->hostname;
      എങ്കിൽ(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, ശൂന്യ,
                                       remotekey, keylen, ശൂന്യ, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, ശൂന്യ);
        എങ്കിൽ(addrc)
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        അല്ലെങ്കിൽ എങ്കിൽ(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          പൂർണ്ണസംഖ്യ wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          എങ്കിൽ(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    അല്ലെങ്കിൽ
      infof(data, "SSH: knownhost entry matches host key");
    നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_check_fingerprint(ഘടന Curl_easy *data,
                                      ഘടന ssh_conn *sshc)
{
  സ്ഥിരം അക്ഷരം *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_MD5);
  സ്ഥിരം അക്ഷരം *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_SHA256);

  എങ്കിൽ(pubkey_sha256) {
    സ്ഥിരം അക്ഷരം *fingerprint = ശൂന്യ;
    അക്ഷരം *fingerprint_b64 = ശൂന്യ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    എങ്കിൽ(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      തിരികെ CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    എങ്കിൽ(curlx_base64_encode((സ്ഥിരം uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      തിരികെ CURLE_PEER_FAILED_VERIFICATION;
    }

    എങ്കിൽ(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      തിരികെ CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    ആകുമ്പോൾ((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    ആകുമ്പോൾ((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    എങ്കിൽ((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      തിരികെ CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  എങ്കിൽ(pubkey_md5) {
    അക്ഷരം md5buffer[33];
    സ്ഥിരം അക്ഷരം *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    എങ്കിൽ(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      പൂർണ്ണസംഖ്യ i;
      വേണ്ടി(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned അക്ഷരം)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    എങ്കിൽ(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      എങ്കിൽ(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      അല്ലെങ്കിൽ {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      തിരികെ CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  എങ്കിൽ(!pubkey_md5 && !pubkey_sha256) {
    എങ്കിൽ(data->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      പൂർണ്ണസംഖ്യ sshkeytype = 0;
      പൂർണ്ണസംഖ്യ rc = 0;
      /* we handle the process to the callback */
      സ്ഥിരം അക്ഷരം *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      എങ്കിൽ(remotekey) {
        ഘടന Curl_mapi_guard guard;
        എണ്ണൽ curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (പൂർണ്ണസംഖ്യ)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        എങ്കിൽ(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          തിരികെ CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      അല്ലെങ്കിൽ {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        തിരികെ CURLE_PEER_FAILED_VERIFICATION;
      }
      തിരികെ CURLE_OK;
    }
    അല്ലെങ്കിൽ {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      തിരികെ ssh_knownhost(data, sshc);
    }
  }
  അല്ലെങ്കിൽ {
    /* as we already matched, we skip the check for known hosts */
    തിരികെ CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
സ്ഥിതം CURLcode ssh_force_knownhost_താക്കോൽ_type(ഘടന Curl_easy *data,
                                             ഘടന ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  സ്ഥിതം സ്ഥിരം അക്ഷരം hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  സ്ഥിതം സ്ഥിരം അക്ഷരം hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  സ്ഥിതം സ്ഥിരം അക്ഷരം hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  സ്ഥിതം സ്ഥിരം അക്ഷരം hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  സ്ഥിതം സ്ഥിരം അക്ഷരം hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_താക്കോൽ_SSHDSS
  സ്ഥിതം സ്ഥിരം അക്ഷരം hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  എങ്കിൽ(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_SHA256)) {
    ഘടന libssh2_knownhost *store = ശൂന്യ;
    ഘടന connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_താക്കോൽ_MASK >>
                      LIBSSH2_KNOWNHOST_താക്കോൽ_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    ആകുമ്പോൾ(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      എങ്കിൽ(store) {
        എങ്കിൽ(store->name) {
          എങ്കിൽ(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_size = 0;
            സ്ഥിരം അക്ഷരം *p;
            സ്ഥിരം അക്ഷരം *kh_name_end = strstr(store->name, "]:");
            എങ്കിൽ(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              തുടരുക;
            }
            p = kh_name_end + 2; /* start of port number */
            എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = strlen(store->name) - 1 - strlen(kh_name_end);
              എങ്കിൽ((kh_name_size == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                നിർത്തുക;
              }
            }
          }
          അല്ലെങ്കിൽ എങ്കിൽ(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            നിർത്തുക;
          }
        }
        അല്ലെങ്കിൽ എങ്കിൽ(store->താക്കോൽ) {
          unsigned പൂർണ്ണസംഖ്യ type = (store->typemask & LIBSSH2_KNOWNHOST_താക്കോൽ_MASK)
                              >> LIBSSH2_KNOWNHOST_താക്കോൽ_SHIFT;
          എങ്കിൽ(!absent_type[type]) {
            പൂർണ്ണസംഖ്യ keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->താക്കോൽ, strlen(store->താക്കോൽ),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_താക്കോൽ_MASK),
              ശൂന്യ);
            എങ്കിൽ(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              നിർത്തുക;
            }
            അല്ലെങ്കിൽ എങ്കിൽ(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    എങ്കിൽ(found) {
      പൂർണ്ണസംഖ്യ rc;
      സ്ഥിരം അക്ഷരം *hostkey_method = ശൂന്യ;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      തിരഞ്ഞെടുപ്പ്(store->typemask & LIBSSH2_KNOWNHOST_താക്കോൽ_MASK) {
      അവസ്ഥ LIBSSH2_KNOWNHOST_താക്കോൽ_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        നിർത്തുക;
      അവസ്ഥ LIBSSH2_KNOWNHOST_താക്കോൽ_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        നിർത്തുക;
      അവസ്ഥ LIBSSH2_KNOWNHOST_താക്കോൽ_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        നിർത്തുക;
      അവസ്ഥ LIBSSH2_KNOWNHOST_താക്കോൽ_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        നിർത്തുക;
      അവസ്ഥ LIBSSH2_KNOWNHOST_താക്കോൽ_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        നിർത്തുക;
#ifdef LIBSSH2_KNOWNHOST_താക്കോൽ_SSHDSS
      അവസ്ഥ LIBSSH2_KNOWNHOST_താക്കോൽ_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        നിർത്തുക;
#endif
      അവസ്ഥ LIBSSH2_KNOWNHOST_താക്കോൽ_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        തിരികെ CURLE_SSH;
      സ്ഥിരസ്ഥിതി:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_താക്കോൽ_MASK));
        തിരികെ CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      എങ്കിൽ(rc) {
        അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
        പൂർണ്ണസംഖ്യ errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_സന്ദേശം);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    അല്ലെങ്കിൽ {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode quote_error(ഘടന Curl_easy *data,
                            ഘടന ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  തിരികെ CURLE_QUOTE_ERROR;
}

സ്ഥിതം CURLcode sftp_quote(ഘടന Curl_easy *data,
                           ഘടന ssh_conn *sshc,
                           ഘടന SSHPROTO *sshp)
{
  സ്ഥിരം അക്ഷരം *cp;
  CURLcode result = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  സ്ഥിരം അക്ഷരം *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  എങ്കിൽ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  എങ്കിൽ(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    അക്ഷരം *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    എങ്കിൽ(!tmp)
      തിരികെ CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    എങ്കിൽ(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    തിരികെ result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  എങ്കിൽ(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    തിരികെ CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  എങ്കിൽ(result) {
    എങ്കിൽ(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    തിരികെ result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  എങ്കിൽ(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    എങ്കിൽ(result) {
      എങ്കിൽ(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      തിരികെ result;
    }
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);

    memset(&sshp->quote_attrs, 0, വലുപ്പം(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    തിരികെ result;
  }
  എങ്കിൽ(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    എങ്കിൽ(result) {
      എങ്കിൽ(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      തിരികെ result;
    }
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    തിരികെ result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "mkdir ", 6)) {
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    തിരികെ result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    എങ്കിൽ(result) {
      എങ്കിൽ(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      തിരികെ result;
    }
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    തിരികെ result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "rmdir ", 6)) {
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    തിരികെ result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "rm ", 3)) {
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    തിരികെ result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "statvfs ", 8)) {
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    തിരികെ result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  തിരികെ CURLE_QUOTE_ERROR;
}

സ്ഥിതം CURLcode sftp_upload_init(ഘടന Curl_easy *data,
                                 ഘടന ssh_conn *sshc,
                                 ഘടന SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  എങ്കിൽ(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    എങ്കിൽ(data->state.resume_from < 0) {
      പൂർണ്ണസംഖ്യ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        തിരികെ CURLE_OK;
      }
      എങ്കിൽ(rc) {
        data->state.resume_from = 0;
      }
      അല്ലെങ്കിൽ {
        curl_off_t size = attrs.filesize;
        എങ്കിൽ(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          തിരികെ CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  എങ്കിൽ(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  അല്ലെങ്കിൽ {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  എങ്കിൽ(!sshc->sftp_handle) {
    CURLcode result;
    unsigned long sftperr;
    പൂർണ്ണസംഖ്യ rc = libssh2_session_last_errno(sshc->ssh_session);

    എങ്കിൽ(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      തിരികെ CURLE_OK;
    }

    എങ്കിൽ(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    അല്ലെങ്കിൽ
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    എങ്കിൽ(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      തിരികെ sftp_libssh2_error_to_CURLE(sftperr);
    }
    എങ്കിൽ((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      തിരികെ CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    എങ്കിൽ(!result) {
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
    തിരികെ result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  എങ്കിൽ(data->state.resume_from > 0 && !data->set.remote_append) {
    പൂർണ്ണസംഖ്യ seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    എങ്കിൽ(data->set.seek_func) {
      ഘടന Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    എങ്കിൽ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      എങ്കിൽ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        തിരികെ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ചെയ്യുക {
        ഘടന Curl_mapi_guard guard;
        അക്ഷരം scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)വലുപ്പം(scratch)) ?
          വലുപ്പം(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        എങ്കിൽ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          തിരികെ CURLE_FTP_COULDNT_USE_REST;
        }
      } ആകുമ്പോൾ(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    എങ്കിൽ(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  എങ്കിൽ(data->state.infilesize > 0) {
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
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം ssh_state_pkey_init(ഘടന Curl_easy *data, ഘടന ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  എങ്കിൽ((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    എങ്കിൽ(sshc->pub_താക്കോൽ)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_താക്കോൽ);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_താക്കോൽ);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
}

സ്ഥിതം CURLcode sftp_quote_stat(ഘടന Curl_easy *data,
                                ഘടന ssh_conn *sshc,
                                ഘടന SSHPROTO *sshp,
                                bool *blockp)
{
  സ്ഥിരം അക്ഷരം *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  എങ്കിൽ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  എങ്കിൽ(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    പൂർണ്ണസംഖ്യ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      തിരികെ CURLE_OK;
    }
    എങ്കിൽ(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      പോകുക fail;
    }
  }

  /* Now set the new attributes... */
  എങ്കിൽ(!strncmp(cmd, "chgrp", 5)) {
    സ്ഥിരം അക്ഷരം *p = sshc->quote_path1;
    curl_off_t gid;
    എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      പോകുക fail;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    സ്ഥിരം അക്ഷരം *p = sshc->quote_path1;
    /* permissions are octal */
    എങ്കിൽ(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      പോകുക fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "chown", 5)) {
    സ്ഥിരം അക്ഷരം *p = sshc->quote_path1;
    curl_off_t uid;
    എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      പോകുക fail;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    എങ്കിൽ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#എങ്കിൽ SIZEOF_TIME_T > SIZEOF_LONG
    എങ്കിൽ(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    എങ്കിൽ(fail)
      പോകുക fail;
    എങ്കിൽ(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    അല്ലെങ്കിൽ /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  തിരികെ CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  തിരികെ CURLE_QUOTE_ERROR;
}

സ്ഥിതം CURLcode sftp_download_stat(ഘടന Curl_easy *data,
                                   ഘടന ssh_conn *sshc,
                                   ഘടന SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  പൂർണ്ണസംഖ്യ rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    തിരികെ CURLE_OK;
  }
  എങ്കിൽ(rc ||
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
  അല്ലെങ്കിൽ {
    curl_off_t size = attrs.filesize;

    എങ്കിൽ(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      തിരികെ CURLE_BAD_DOWNLOAD_RESUME;
    }
    എങ്കിൽ(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      എങ്കിൽ(result)
        തിരികെ result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  എങ്കിൽ(data->state.resume_from) {
    എങ്കിൽ(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      എങ്കിൽ((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        തിരികെ CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        തിരികെ CURLE_BAD_DOWNLOAD_RESUME;
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
  എങ്കിൽ(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    തിരികെ CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode sftp_readdir(ഘടന Curl_easy *data,
                             ഘടന ssh_conn *sshc,
                             ഘടന SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    തിരികെ result;
  }
  എങ്കിൽ(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    എങ്കിൽ(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      എങ്കിൽ(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    അല്ലെങ്കിൽ {
      result = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      എങ്കിൽ(!result) {
        എങ്കിൽ((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        അല്ലെങ്കിൽ {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      തിരികെ result;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  അല്ലെങ്കിൽ {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_init(ഘടന Curl_easy *data,
                               ഘടന ssh_conn *sshc)
{
  CURLcode result;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  result = ssh_force_knownhost_താക്കോൽ_type(data, sshc);
  എങ്കിൽ(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_S_STARTUP);
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_startup(ഘടന Curl_easy *data,
                                  ഘടന ssh_conn *sshc)
{
  ഘടന connectdata *conn = data->conn;
  പൂർണ്ണസംഖ്യ rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc) {
    അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
    (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_സന്ദേശം);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    തിരികെ CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_hostkey(ഘടന Curl_easy *data,
                                  ഘടന ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode result = ssh_check_fingerprint(data, sshc);
  എങ്കിൽ(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_authlist(ഘടന Curl_easy *data,
                                   ഘടന ssh_conn *sshc)
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
  ഘടന connectdata *conn = data->conn;
  സ്ഥിരം അക്ഷരം *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(strlen(user)));

  എങ്കിൽ(!sshc->authlist) {
    പൂർണ്ണസംഖ്യ rc;
    എങ്കിൽ(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      തിരികെ CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    തിരികെ libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_auth_pkey(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  ഘടന connectdata *conn = data->conn;
  സ്ഥിരം അക്ഷരം *user = Curl_creds_user(conn->creds);
  പൂർണ്ണസംഖ്യ rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_താക്കോൽ, sshc->priv_താക്കോൽ, sshc->passphrase ? sshc->passphrase : "");
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  അല്ലെങ്കിൽ {
    അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
    അക്ഷരം unknown[] = "Reason unknown (-1)";
    എങ്കിൽ(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_സന്ദേശം = unknown;
    }
    അല്ലെങ്കിൽ {
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_സന്ദേശം);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_auth_pass_init(ഘടന Curl_easy *data,
                                         ഘടന ssh_conn *sshc)
{
  എങ്കിൽ((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_auth_pass(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc)
{
  ഘടന connectdata *conn = data->conn;
  സ്ഥിരം അക്ഷരം *user = Curl_creds_user(conn->creds);
  സ്ഥിരം അക്ഷരം *passwd = Curl_creds_passwd(conn->creds);
  പൂർണ്ണസംഖ്യ rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 ശൂന്യ);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN) {
    തിരികെ CURLE_AGAIN;
  }
  എങ്കിൽ(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_auth_host_init(ഘടന Curl_easy *data,
                                         ഘടന ssh_conn *sshc)
{
  എങ്കിൽ((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_auth_agent_init(ഘടന Curl_easy *data,
                                          ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = 0;
  എങ്കിൽ((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    എങ്കിൽ(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      എങ്കിൽ(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_താക്കോൽ_INIT);
        തിരികെ CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;
    എങ്കിൽ(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_താക്കോൽ_INIT);
    }
    അല്ലെങ്കിൽ {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_AUTH_താക്കോൽ_INIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_auth_agent_list(ഘടന Curl_easy *data,
                                          ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = libssh2_agent_list_identities(sshc->ssh_agent);

  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;
  എങ്കിൽ(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_താക്കോൽ_INIT);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = ശൂന്യ;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_auth_agent(ഘടന Curl_easy *data,
                                     ഘടന ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  പൂർണ്ണസംഖ്യ rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_prev_identity);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    എങ്കിൽ(rc < 0) {
      എങ്കിൽ(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        തിരികെ CURLE_OK;
      }
      തിരികെ CURLE_AGAIN;
    }
  }

  എങ്കിൽ(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  അല്ലെങ്കിൽ എങ്കിൽ(rc == 1)
    infof(data, "SSH: no agent identity would match");

  എങ്കിൽ(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_AUTH_താക്കോൽ_INIT);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_auth_താക്കോൽ_init(ഘടന Curl_easy *data,
                                        ഘടന ssh_conn *sshc)
{
  എങ്കിൽ((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_താക്കോൽ);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_auth_താക്കോൽ(ഘടന Curl_easy *data,
                                   ഘടന ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  ഘടന connectdata *conn = data->conn;
  സ്ഥിരം അക്ഷരം *user = Curl_creds_user(conn->creds);
  പൂർണ്ണസംഖ്യ rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    തിരികെ CURLE_OK;
  }
  തിരികെ CURLE_LOGIN_DENIED;
}

സ്ഥിതം CURLcode ssh_state_auth_done(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc)
{
  ഘടന connectdata *conn = data->conn;
  എങ്കിൽ(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    തിരികെ CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  എങ്കിൽ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    തിരികെ CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_init(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  എങ്കിൽ(!sshc->sftp_session) {
    അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
    എങ്കിൽ(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
    failf(data, "Failure initializing sftp session: %s", err_സന്ദേശം);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    തിരികെ CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_realpath(ഘടന Curl_easy *data,
                                        ഘടന ssh_conn *sshc,
                                        ഘടന SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  എങ്കിൽ(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    എങ്കിൽ(!sshc->homedir)
      തിരികെ CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    എങ്കിൽ(!data->state.most_recent_ftp_entrypath)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }
  അല്ലെങ്കിൽ {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    എങ്കിൽ(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    അല്ലെങ്കിൽ
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (പൂർണ്ണസംഖ്യ)result);
    തിരികെ result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote_init(ഘടന Curl_easy *data,
                                          ഘടന ssh_conn *sshc,
                                          ഘടന SSHPROTO *sshp)
{
  CURLcode result;

  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  എങ്കിൽ(result) {
    myssh_to(data, sshc, SSH_STOP);
    തിരികെ result;
  }

  എങ്കിൽ(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_postquote_init(ഘടന Curl_easy *data,
                                              ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_STOP);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote(ഘടന Curl_easy *data,
                                     ഘടന ssh_conn *sshc,
                                     ഘടന SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  എങ്കിൽ(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_sftp_next_quote(ഘടന Curl_easy *data,
                                          ഘടന ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  എങ്കിൽ(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    അല്ലെങ്കിൽ {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote_stat(ഘടന Curl_easy *data,
                                          ഘടന ssh_conn *sshc,
                                          ഘടന SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  എങ്കിൽ(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote_setstat(ഘടന Curl_easy *data,
                                             ഘടന ssh_conn *sshc,
                                             ഘടന SSHPROTO *sshp)
{
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    തിരികെ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote_symlink(ഘടന Curl_easy *data,
                                             ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    തിരികെ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote_mkdir(ഘടന Curl_easy *data,
                                           ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)data->set.new_directory_perms);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    തിരികെ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote_rename(ഘടന Curl_easy *data,
                                            ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    തിരികെ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote_rmdir(ഘടന Curl_easy *data,
                                           ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    തിരികെ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote_unlink(ഘടന Curl_easy *data,
                                            ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    തിരികെ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_quote_statvfs(ഘടന Curl_easy *data,
                                             ഘടന ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  പൂർണ്ണസംഖ്യ rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    തിരികെ CURLE_QUOTE_ERROR;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#അല്ലെങ്കിൽ
#define CURL_LIBSSH2_VFS_SIZE_MASK "llu"
#endif
    CURLcode result;
    അക്ഷരം *tmp = curl_maprintf("statvfs:\n"
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
    എങ്കിൽ(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      തിരികെ CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    എങ്കിൽ(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      തിരികെ result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_create_dirs_mkdir(ഘടന Curl_easy *data,
                                                 ഘടന ssh_conn *sshc,
                                                 ഘടന SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)data->set.new_directory_perms);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  എങ്കിൽ(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    എങ്കിൽ((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      തിരികെ sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_readdir_init(ഘടന Curl_easy *data,
                                            ഘടന ssh_conn *sshc,
                                            ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  എങ്കിൽ(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    തിരികെ CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  എങ്കിൽ(!sshc->sftp_handle) {
    unsigned long sftperr;
    എങ്കിൽ(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    തിരികെ sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_readdir_link(ഘടന Curl_easy *data,
                                            ഘടന ssh_conn *sshc,
                                            ഘടന SSHPROTO *sshp)
{
  CURLcode result;
  പൂർണ്ണസംഖ്യ rc;
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned പൂർണ്ണസംഖ്യ)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  എങ്കിൽ(rc < 0)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  എങ്കിൽ(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_scp_download_init(ഘടന Curl_easy *data,
                                            ഘടന ssh_conn *sshc,
                                            ഘടന SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, വലുപ്പം(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  എങ്കിൽ(!sshc->ssh_channel) {
    പൂർണ്ണസംഖ്യ ssh_err;
    അക്ഷരം *err_സന്ദേശം = ശൂന്യ;

    എങ്കിൽ(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
    failf(data, "%s", err_സന്ദേശം);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    തിരികെ libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_close(ഘടന Curl_easy *data,
                                     ഘടന ssh_conn *sshc,
                                     ഘടന SSHPROTO *sshp)
{
  പൂർണ്ണസംഖ്യ rc = 0;
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    എങ്കിൽ(rc < 0) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_സന്ദേശം);
    }
    sshc->sftp_handle = ശൂന്യ;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  എങ്കിൽ(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_STOP);

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_shutdown(ഘടന Curl_easy *data,
                                        ഘടന ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  പൂർണ്ണസംഖ്യ rc = 0;
  എങ്കിൽ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    എങ്കിൽ(rc < 0) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_സന്ദേശം);
    }
    sshc->sftp_handle = ശൂന്യ;
  }
  എങ്കിൽ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    എങ്കിൽ(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = ശൂന്യ;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_download_init(ഘടന Curl_easy *data,
                                             ഘടന ssh_conn *sshc,
                                             ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  എങ്കിൽ(!sshc->sftp_handle) {
    unsigned long sftperr;
    എങ്കിൽ(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      തിരികെ CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    തിരികെ sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_scp_upload_init(ഘടന Curl_easy *data,
                                          ഘടന ssh_conn *sshc,
                                          ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (പൂർണ്ണസംഖ്യ)data->set.new_file_perms,
                       (libssh2_int64_t)data->state.infilesize, 0, 0);
  എങ്കിൽ(!sshc->ssh_channel) {
    പൂർണ്ണസംഖ്യ ssh_err;
    അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
    CURLcode result;
    എങ്കിൽ(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
    failf(data, "%s", err_സന്ദേശം);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    എങ്കിൽ(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    തിരികെ result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_session_disconnect(ഘടന Curl_easy *data,
                                             ഘടന ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  പൂർണ്ണസംഖ്യ rc = 0;
  എങ്കിൽ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    എങ്കിൽ(rc < 0) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_സന്ദേശം);
    }
    sshc->ssh_channel = ശൂന്യ;
  }

  എങ്കിൽ(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;

    എങ്കിൽ(rc < 0) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_സന്ദേശം);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode sshc_cleanup(ഘടന ssh_conn *sshc, ഘടന Curl_easy *data,
                             bool block)
{
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = ശൂന്യ;
  }

  എങ്കിൽ(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    എങ്കിൽ((rc < 0) && data) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_സന്ദേശം);
    }
    എങ്കിൽ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      തിരികെ CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = ശൂന്യ;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = ശൂന്യ;
    sshc->sshagent_prev_identity = ശൂന്യ;
  }

  എങ്കിൽ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    എങ്കിൽ((rc < 0) && data) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_സന്ദേശം);
    }
    എങ്കിൽ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      തിരികെ CURLE_AGAIN;

    sshc->sftp_handle = ശൂന്യ;
  }

  എങ്കിൽ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    എങ്കിൽ((rc < 0) && data) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_സന്ദേശം);
    }
    എങ്കിൽ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      തിരികെ CURLE_AGAIN;

    sshc->ssh_channel = ശൂന്യ;
  }

  എങ്കിൽ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    എങ്കിൽ((rc < 0) && data) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_സന്ദേശം);
    }
    എങ്കിൽ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      തിരികെ CURLE_AGAIN;

    sshc->sftp_session = ശൂന്യ;
  }

  എങ്കിൽ(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    എങ്കിൽ((rc < 0) && data) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session, &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_സന്ദേശം);
    }
    എങ്കിൽ(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      തിരികെ CURLE_AGAIN;

    sshc->ssh_session = ശൂന്യ;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_താക്കോൽ);
  curlx_safefree(sshc->priv_താക്കോൽ);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_getinfo(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_filetime(ഘടന Curl_easy *data,
                                        ഘടന ssh_conn *sshc,
                                        ഘടന SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  എങ്കിൽ(rc == 0)
    data->info.filetime = (time_t)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_trans_init(ഘടന Curl_easy *data,
                                          ഘടന ssh_conn *sshc,
                                          ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  അല്ലെങ്കിൽ എങ്കിൽ(sshp) {
    size_t plen = strlen(sshp->path);
    എങ്കിൽ(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    അല്ലെങ്കിൽ
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  അല്ലെങ്കിൽ
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_upload_init(ഘടന Curl_easy *data,
                                           ഘടന ssh_conn *sshc,
                                           ഘടന SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  എങ്കിൽ(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_sftp_create_dirs_init(ഘടന Curl_easy *data,
                                                ഘടന ssh_conn *sshc,
                                                ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_create_dirs(ഘടന Curl_easy *data,
                                           ഘടന ssh_conn *sshc,
                                           ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  എങ്കിൽ(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    തിരികെ CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_readdir(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc,
                                       ഘടന SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  എങ്കിൽ(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_sftp_readdir_bottom(ഘടന Curl_easy *data,
                                              ഘടന ssh_conn *sshc,
                                              ഘടന SSHPROTO *sshp)
{
  CURLcode result;
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  എങ്കിൽ(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  എങ്കിൽ(result) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(data, sshc, SSH_STOP);
  }
  അല്ലെങ്കിൽ {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_sftp_readdir_done(ഘടന Curl_easy *data,
                                            ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;

  sshc->sftp_handle = ശൂന്യ;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_sftp_download_stat(ഘടന Curl_easy *data,
                                             ഘടന ssh_conn *sshc,
                                             ഘടന SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  എങ്കിൽ(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_state_scp_trans_init(ഘടന Curl_easy *data,
                                         ഘടന ssh_conn *sshc,
                                         ഘടന SSHPROTO *sshp)
{
  CURLcode result;
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  എങ്കിൽ(result) {
    myssh_to(data, sshc, SSH_STOP);
    തിരികെ result;
  }

  എങ്കിൽ(data->state.upload) {
    എങ്കിൽ(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      തിരികെ CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_scp_done(ഘടന Curl_easy *data,
                                   ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_scp_send_eof(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->ssh_channel) {
    പൂർണ്ണസംഖ്യ rc = libssh2_channel_send_eof(sshc->ssh_channel);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;
    എങ്കിൽ(rc) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session,
                                       &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_സന്ദേശം);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_scp_wait_eof(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->ssh_channel) {
    പൂർണ്ണസംഖ്യ rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;
    എങ്കിൽ(rc) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session,
                                       &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_സന്ദേശം);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_scp_wait_close(ഘടന Curl_easy *data,
                                         ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->ssh_channel) {
    പൂർണ്ണസംഖ്യ rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;
    എങ്കിൽ(rc) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session,
                                       &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_സന്ദേശം);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_scp_channel_free(
  ഘടന Curl_easy *data,
  ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->ssh_channel) {
    പൂർണ്ണസംഖ്യ rc = libssh2_channel_free(sshc->ssh_channel);
    എങ്കിൽ(rc == LIBSSH2_ERROR_EAGAIN)
      തിരികെ CURLE_AGAIN;
    എങ്കിൽ(rc < 0) {
      അക്ഷരം *err_സന്ദേശം = ശൂന്യ;
      (ശൂന്യം)libssh2_session_last_error(sshc->ssh_session,
                                       &err_സന്ദേശം, ശൂന്യ, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_സന്ദേശം);
    }
    sshc->ssh_channel = ശൂന്യ;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssh_state_session_free(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  ഘടന connectdata *conn = data->conn;
  എങ്കിൽ(result)
    തിരികെ result;
  memset(sshc, 0, വലുപ്പം(ഘടന ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  തിരികെ CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
സ്ഥിതം CURLcode ssh_statemachine(ഘടന Curl_easy *data,
                                 ഘടന ssh_conn *sshc,
                                 ഘടന SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode result = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  ചെയ്യുക {
    തിരഞ്ഞെടുപ്പ്(sshc->state) {
    അവസ്ഥ SSH_INIT:
      result = ssh_state_init(data, sshc);
      എങ്കിൽ(result)
        നിർത്തുക;
      FALLTHROUGH();

    അവസ്ഥ SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      എങ്കിൽ(result)
        നിർത്തുക;
      FALLTHROUGH();

    അവസ്ഥ SSH_HOSTKEY:
      result = ssh_state_hostkey(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTHLIST:
      result = ssh_state_authlist(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_PKEY:
      result = ssh_state_auth_pkey(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_PASS_INIT:
      result = ssh_state_auth_pass_init(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_PASS:
      result = ssh_state_auth_pass(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_HOST_INIT:
      result = ssh_state_auth_host_init(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_HOST:
      myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_AGENT_INIT:
      result = ssh_state_auth_agent_init(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_AGENT_LIST:
      result = ssh_state_auth_agent_list(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_AGENT:
      result = ssh_state_auth_agent(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_താക്കോൽ_INIT:
      result = ssh_state_auth_താക്കോൽ_init(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_താക്കോൽ:
      result = ssh_state_auth_താക്കോൽ(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_AUTH_DONE:
      result = ssh_state_auth_done(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_INIT:
      result = ssh_state_sftp_init(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_REALPATH:
      result = ssh_state_sftp_realpath(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE_INIT:
      result = ssh_state_sftp_quote_init(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_POSTQUOTE_INIT:
      result = ssh_state_sftp_postquote_init(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE:
      result = ssh_state_sftp_quote(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_NEXT_QUOTE:
      result = ssh_state_sftp_next_quote(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE_STAT:
      result = ssh_state_sftp_quote_stat(data, sshc, sshp, block);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE_SETSTAT:
      result = ssh_state_sftp_quote_setstat(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE_SYMLINK:
      result = ssh_state_sftp_quote_symlink(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE_MKDIR:
      result = ssh_state_sftp_quote_mkdir(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE_RENAME:
      result = ssh_state_sftp_quote_rename(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE_RMDIR:
      result = ssh_state_sftp_quote_rmdir(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE_UNLINK:
      result = ssh_state_sftp_quote_unlink(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_QUOTE_STATVFS:
      result = ssh_state_sftp_quote_statvfs(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_GETINFO:
      result = ssh_state_sftp_getinfo(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_FILETIME:
      result = ssh_state_sftp_filetime(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_TRANS_INIT:
      result = ssh_state_sftp_trans_init(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_UPLOAD_INIT:
      result = ssh_state_sftp_upload_init(data, sshc, sshp, block);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_CREATE_DIRS_INIT:
      result = ssh_state_sftp_create_dirs_init(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_CREATE_DIRS:
      result = ssh_state_sftp_create_dirs(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_CREATE_DIRS_MKDIR:
      result = ssh_state_sftp_create_dirs_mkdir(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_READDIR_INIT:
      result = ssh_state_sftp_readdir_init(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_READDIR:
      result = ssh_state_sftp_readdir(data, sshc, sshp, block);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_READDIR_LINK:
      result = ssh_state_sftp_readdir_link(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_READDIR_BOTTOM:
      result = ssh_state_sftp_readdir_bottom(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_READDIR_DONE:
      result = ssh_state_sftp_readdir_done(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_DOWNLOAD_INIT:
      result = ssh_state_sftp_download_init(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_DOWNLOAD_STAT:
      result = ssh_state_sftp_download_stat(data, sshc, sshp, block);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_CLOSE:
      result = ssh_state_sftp_close(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SFTP_SHUTDOWN:
      result = ssh_state_sftp_shutdown(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SCP_TRANS_INIT:
      result = ssh_state_scp_trans_init(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SCP_UPLOAD_INIT:
      result = ssh_state_scp_upload_init(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SCP_DOWNLOAD_INIT:
      result = ssh_state_scp_download_init(data, sshc, sshp);
      നിർത്തുക;

    അവസ്ഥ SSH_SCP_DONE:
      result = ssh_state_scp_done(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SCP_SEND_EOF:
      result = ssh_state_scp_send_eof(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SCP_WAIT_EOF:
      result = ssh_state_scp_wait_eof(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SCP_WAIT_CLOSE:
      result = ssh_state_scp_wait_close(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SCP_CHANNEL_FREE:
      result = ssh_state_scp_channel_free(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SESSION_DISCONNECT:
      result = ssh_state_session_disconnect(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_SESSION_FREE:
      result = ssh_state_session_free(data, sshc);
      നിർത്തുക;

    അവസ്ഥ SSH_QUIT:
    സ്ഥിരസ്ഥിതി:
      /* internal error */
      myssh_to(data, sshc, SSH_STOP);
      നിർത്തുക;
    }

  } ആകുമ്പോൾ(!result && (sshc->state != SSH_STOP) && !*block);

  എങ്കിൽ(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (പൂർണ്ണസംഖ്യ)result, *block);

  തിരികെ result;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
സ്ഥിതം ശൂന്യം ssh_block2waitfor(ഘടന Curl_easy *data,
                              ഘടന ssh_conn *sshc,
                              bool block)
{
  പൂർണ്ണസംഖ്യ dir = 0;
  (ശൂന്യം)data;
  എങ്കിൽ(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    എങ്കിൽ(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  എങ്കിൽ(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
സ്ഥിതം CURLcode ssh_multi_statemach(ഘടന Curl_easy *data, bool *done)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ഘടന SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  എങ്കിൽ(!sshc || !sshp)
    തിരികെ CURLE_FAILED_INIT;

  ചെയ്യുക {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } ആകുമ്പോൾ(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  തിരികെ result;
}

സ്ഥിതം CURLcode ssh_block_statemach(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc,
                                    ഘടന SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  ഘടന curltime start = *Curl_pgrs_now(data);

  ആകുമ്പോൾ((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t ഇടത്_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    എങ്കിൽ(result)
      നിർത്തുക;

    എങ്കിൽ(!disconnect) {
      result = Curl_pgrsCheck(data);
      എങ്കിൽ(result)
        നിർത്തുക;

      ഇടത്_ms = Curl_timeleft_ms(data);
      എങ്കിൽ(ഇടത്_ms < 0) {
        failf(data, "Operation timed out");
        തിരികെ CURLE_OPERATION_TIMEDOUT;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      നിർത്തുക;
    }

    എങ്കിൽ(block) {
      പൂർണ്ണസംഖ്യ dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      എങ്കിൽ(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      എങ്കിൽ(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (ശൂന്യം)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!ഇടത്_ms || ഇടത്_ms > 1000) ? 1000 : ഇടത്_ms);
    }
  }

  തിരികെ result;
}

സ്ഥിതം ശൂന്യം myssh_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന SSHPROTO *sshp = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

സ്ഥിതം ശൂന്യം myssh_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന ssh_conn *sshc = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  sshc_cleanup(sshc, ശൂന്യ, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
സ്ഥിതം CURLcode ssh_setup_connection(ഘടന Curl_easy *data,
                                     ഘടന connectdata *conn)
{
  ഘടന ssh_conn *sshc;
  ഘടന SSHPROTO *sshp;
  (ശൂന്യം)conn;

  sshc = curlx_calloc(1, വലുപ്പം(*sshc));
  എങ്കിൽ(!sshc)
    തിരികെ CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, വലുപ്പം(*sshp));
  എങ്കിൽ(!sshp)
    തിരികെ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  എങ്കിൽ(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  തിരികെ Curl_ssh_setup_pkey(data, sshc);
}

സ്ഥിതം Curl_recv scp_recv, sftp_recv;
സ്ഥിതം Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
സ്ഥിതം ssize_t ssh_tls_recv(libssh2_socket_t sock, ശൂന്യം *buffer,
                            size_t length, പൂർണ്ണസംഖ്യ flags, ശൂന്യം **abstract)
{
  ഘടന Curl_easy *data = (ഘടന Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nread;
  CURLcode result;
  ഘടന connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ശൂന്യം)flags;

  എങ്കിൽ(!sshc)
    തിരികെ -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  എങ്കിൽ(result == CURLE_AGAIN)
    തിരികെ -EAGAIN; /* magic return code for libssh2 */
  അല്ലെങ്കിൽ എങ്കിൽ(result)
    തിരികെ -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (സ്ഥിരം അക്ഷരം *)buffer, nread);
  തിരികെ (ssize_t)nread;
}

സ്ഥിതം ssize_t ssh_tls_send(libssh2_socket_t sock, സ്ഥിരം ശൂന്യം *buffer,
                            size_t length, പൂർണ്ണസംഖ്യ flags, ശൂന്യം **abstract)
{
  ഘടന Curl_easy *data = (ഘടന Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nwrite;
  CURLcode result;
  ഘടന connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ശൂന്യം)flags;

  എങ്കിൽ(!sshc)
    തിരികെ -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  എങ്കിൽ(result == CURLE_AGAIN)
    തിരികെ -EAGAIN; /* magic return code for libssh2 */
  അല്ലെങ്കിൽ എങ്കിൽ(result)
    തിരികെ -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (സ്ഥിരം അക്ഷരം *)buffer, nwrite);
  തിരികെ (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
സ്ഥിതം CURLcode ssh_connect(ഘടന Curl_easy *data, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result;

#എങ്കിൽ LIBSSH2_VERSION_അക്കം >= 0x010b00
  {
    സ്ഥിരം അക്ഷരം *crypto_str;
    തിരഞ്ഞെടുപ്പ്(libssh2_crypto_engine()) {
    അവസ്ഥ libssh2_gcrypt:
      crypto_str = "libgcrypt";
      നിർത്തുക;
    അവസ്ഥ libssh2_mbedtls:
      crypto_str = "mbedTLS";
      നിർത്തുക;
    അവസ്ഥ libssh2_openssl:
      crypto_str = "openssl compatible";
      നിർത്തുക;
    അവസ്ഥ libssh2_os400qc3:
      crypto_str = "OS400QC3";
      നിർത്തുക;
    അവസ്ഥ libssh2_wincng:
      crypto_str = "WinCNG";
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      crypto_str = ശൂന്യ;
      നിർത്തുക;
    }
    എങ്കിൽ(crypto_str)
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

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

  എങ്കിൽ(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    തിരികെ CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#എങ്കിൽ LIBSSH2_VERSION_അക്കം >= 0x010B00
  എങ്കിൽ(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  എങ്കിൽ(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#എങ്കിൽ LIBSSH2_VERSION_അക്കം >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic എടുക്കുക
#endif
#അല്ലെങ്കിൽ
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    സംഘം receive {
      ശൂന്യം *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, ശൂന്യം *, size_t, പൂർണ്ണസംഖ്യ, ശൂന്യം **);
    };
    സംഘം transfer {
      ശൂന്യം *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, സ്ഥിരം ശൂന്യം *, size_t, പൂർണ്ണസംഖ്യ, ശൂന്യം **);
    };
    സംഘം receive sshrecv;
    സംഘം transfer sshsend;

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
  എങ്കിൽ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  അല്ലെങ്കിൽ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  എങ്കിൽ(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  എങ്കിൽ(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    പൂർണ്ണസംഖ്യ rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    എങ്കിൽ(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = ശൂന്യ;
      തിരികെ CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    എങ്കിൽ(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (പൂർണ്ണസംഖ്യ)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  തിരികെ result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

സ്ഥിതം CURLcode scp_perform(ഘടന Curl_easy *data,
                            bool *connected,
                            bool *dophase_done)
{
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  എങ്കിൽ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  തിരികെ result;
}

/* called from multi.c while DOing */
സ്ഥിതം CURLcode scp_doing(ഘടന Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  എങ്കിൽ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  തിരികെ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
സ്ഥിതം CURLcode scp_disconnect(ഘടന Curl_easy *data,
                               ഘടന connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ഘടന SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (ശൂന്യം)dead_connection;

  എങ്കിൽ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  എങ്കിൽ(sshc)
    തിരികെ sshc_cleanup(sshc, data, TRUE);
  തിരികെ result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
സ്ഥിതം CURLcode ssh_done(ഘടന Curl_easy *data, CURLcode status)
{
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  ഘടന SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!sshc || !sshp)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  അല്ലെങ്കിൽ
    result = status;

  എങ്കിൽ(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    തിരികെ CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  തിരികെ result;
}

സ്ഥിതം CURLcode scp_done(ഘടന Curl_easy *data, CURLcode status,
                         bool premature)
{
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (ശൂന്യം)premature;

  എങ്കിൽ(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  തിരികെ ssh_done(data, status);
}

സ്ഥിതം CURLcode scp_send(ഘടന Curl_easy *data, int8_t sockindex,
                         സ്ഥിരം uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (ശൂന്യം)sockindex; /* we only support SCP on the fixed known primary socket */
  (ശൂന്യം)eos;
  *pnwritten = 0;

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (സ്ഥിരം അക്ഷരം *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  എങ്കിൽ(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  അല്ലെങ്കിൽ എങ്കിൽ(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((പൂർണ്ണസംഖ്യ)nwritten);
  അല്ലെങ്കിൽ
    *pnwritten = (size_t)nwritten;

  തിരികെ result;
}

സ്ഥിതം CURLcode scp_recv(ഘടന Curl_easy *data, int8_t sockindex,
                         അക്ഷരം *mem, size_t len, size_t *pnread)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (ശൂന്യം)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  എങ്കിൽ(nread == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;
  അല്ലെങ്കിൽ എങ്കിൽ(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((പൂർണ്ണസംഖ്യ)nread);
  അല്ലെങ്കിൽ
    *pnread = (size_t)nread;

  തിരികെ result;
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

സ്ഥിതം CURLcode sftp_perform(ഘടന Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  എങ്കിൽ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  തിരികെ result;
}

/* called from multi.c while DOing */
സ്ഥിതം CURLcode sftp_doing(ഘടന Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  എങ്കിൽ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  തിരികെ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
സ്ഥിതം CURLcode sftp_disconnect(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn, bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ഘടന SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (ശൂന്യം)dead_connection;

  എങ്കിൽ(sshc) {
    എങ്കിൽ(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (പൂർണ്ണസംഖ്യ)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode sftp_done(ഘടന Curl_easy *data, CURLcode status,
                          bool premature)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  എങ്കിൽ(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    എങ്കിൽ(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  തിരികെ ssh_done(data, status);
}

/* return number of sent bytes */
സ്ഥിതം CURLcode sftp_send(ഘടന Curl_easy *data, int8_t sockindex,
                          സ്ഥിരം uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (ശൂന്യം)sockindex;
  (ശൂന്യം)eos;
  *pnwritten = 0;

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (സ്ഥിരം അക്ഷരം *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  എങ്കിൽ(nwrite == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;
  അല്ലെങ്കിൽ എങ്കിൽ(nwrite < LIBSSH2_ERROR_NONE)
    തിരികെ libssh2_session_error_to_CURLE((പൂർണ്ണസംഖ്യ)nwrite);
  *pnwritten = (size_t)nwrite;
  തിരികെ CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
സ്ഥിതം CURLcode sftp_recv(ഘടന Curl_easy *data, int8_t sockindex,
                          അക്ഷരം *mem, size_t len, size_t *pnread)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (ശൂന്യം)sockindex;
  *pnread = 0;

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  എങ്കിൽ(nread == LIBSSH2_ERROR_EAGAIN)
    തിരികെ CURLE_AGAIN;
  അല്ലെങ്കിൽ എങ്കിൽ(nread < 0)
    തിരികെ libssh2_session_error_to_CURLE((പൂർണ്ണസംഖ്യ)nread);

  *pnread = (size_t)nread;
  തിരികെ CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
സ്ഥിതം CURLcode ssh_do(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  എങ്കിൽ(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  അല്ലെങ്കിൽ
    result = sftp_perform(data, &connected, done);

  തിരികെ result;
}

CURLcode Curl_ssh_init(ശൂന്യം)
{
  എങ്കിൽ(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    തിരികെ CURLE_FAILED_INIT;
  }
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_ssh_cleanup(ശൂന്യം)
{
  libssh2_exit();
}

ശൂന്യം Curl_ssh_version(അക്ഷരം *buffer, size_t buflen)
{
  (ശൂന്യം)curl_msnprintf(buffer, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
സ്ഥിതം ശൂന്യം ssh_attach(ഘടന Curl_easy *data, ഘടന connectdata *conn)
{
  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  എങ്കിൽ(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    എങ്കിൽ(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      ശൂന്യം **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = data;
    }
  }
}

/*
 * SCP protocol handler.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_scp = {
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
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_sftp = {
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
