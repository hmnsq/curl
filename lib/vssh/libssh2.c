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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef __VMS
#அந்தர்பாவ <in.h>
#அந்தர்பாவ <inet.h>
#endif

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "sendf.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "vssh/ssh.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "parsedate.h" /* for the week day and month names */
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "vssh/vssh.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curlx/base64.h" /* for curlx_base64_encode() */

நிலையான மாறிலி எழுத்து *sftp_libssh2_strerror(unsigned long err)
{
  தேர்வு(err) {
  நிலை LIBSSH2_FX_NO_SUCH_FILE:
    திரும்பு "No such file or directory";

  நிலை LIBSSH2_FX_PERMISSION_DENIED:
    திரும்பு "Permission denied";

  நிலை LIBSSH2_FX_FAILURE:
    திரும்பு "Operation failed";

  நிலை LIBSSH2_FX_BAD_MESSAGE:
    திரும்பு "Bad message from SFTP server";

  நிலை LIBSSH2_FX_NO_CONNECTION:
    திரும்பு "Not connected to SFTP server";

  நிலை LIBSSH2_FX_CONNECTION_LOST:
    திரும்பு "Connection to SFTP server lost";

  நிலை LIBSSH2_FX_OP_UNSUPPORTED:
    திரும்பு "Operation not supported by SFTP server";

  நிலை LIBSSH2_FX_INVALID_HANDLE:
    திரும்பு "Invalid handle";

  நிலை LIBSSH2_FX_NO_SUCH_PATH:
    திரும்பு "No such file or directory";

  நிலை LIBSSH2_FX_FILE_ALREADY_EXISTS:
    திரும்பு "File already exists";

  நிலை LIBSSH2_FX_WRITE_PROTECT:
    திரும்பு "File is write protected";

  நிலை LIBSSH2_FX_NO_MEDIA:
    திரும்பு "No media";

  நிலை LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    திரும்பு "Disk full";

  நிலை LIBSSH2_FX_QUOTA_EXCEEDED:
    திரும்பு "User quota exceeded";

  நிலை LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    திரும்பு "Unknown principal";

  நிலை LIBSSH2_FX_LOCK_CONFLICT:
    திரும்பு "File lock conflict";

  நிலை LIBSSH2_FX_DIR_NOT_EMPTY:
    திரும்பு "Directory not empty";

  நிலை LIBSSH2_FX_NOT_A_DIRECTORY:
    திரும்பு "Not a directory";

  நிலை LIBSSH2_FX_INVALID_FILENAME:
    திரும்பு "Invalid filename";

  நிலை LIBSSH2_FX_LINK_LOOP:
    திரும்பு "Link points to itself";
  }
  திரும்பு "Unknown error in libssh2";
}

நிலையான வெறுமை kbd_callback(மாறிலி எழுத்து *name, முழு name_len,
                         மாறிலி எழுத்து *instruction, முழு instruction_len,
                         முழு இலக்கம்_prompts,
                         மாறிலி LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         வெறுமை **abstract)
{
  கட்டமைப்பு Curl_easy *data = (கட்டமைப்பு Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", இலக்கம்_prompts);
#இல்லையெனில்
  (வெறுமை)name;
  (வெறுமை)name_len;
  (வெறுமை)instruction;
  (வெறுமை)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  எனில்(இலக்கம்_prompts == 1) {
    கட்டமைப்பு connectdata *conn = data->conn;
    மாறிலி எழுத்து *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (வெறுமை)prompts;
} /* kbd_callback */

நிலையான CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  தேர்வு(err) {
  நிலை LIBSSH2_FX_OK:
    திரும்பு CURLE_OK;

  நிலை LIBSSH2_FX_NO_SUCH_FILE:
  நிலை LIBSSH2_FX_NO_SUCH_PATH:
    திரும்பு CURLE_REMOTE_FILE_NOT_FOUND;

  நிலை LIBSSH2_FX_PERMISSION_DENIED:
  நிலை LIBSSH2_FX_WRITE_PROTECT:
  நிலை LIBSSH2_FX_LOCK_CONFLICT:
    திரும்பு CURLE_REMOTE_ACCESS_DENIED;

  நிலை LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  நிலை LIBSSH2_FX_QUOTA_EXCEEDED:
    திரும்பு CURLE_REMOTE_DISK_FULL;

  நிலை LIBSSH2_FX_FILE_ALREADY_EXISTS:
    திரும்பு CURLE_REMOTE_FILE_EXISTS;

  நிலை LIBSSH2_FX_DIR_NOT_EMPTY:
    திரும்பு CURLE_QUOTE_ERROR;

  இயல்பு:
    நிறுத்து;
  }

  திரும்பு CURLE_SSH;
}

நிலையான CURLcode libssh2_session_error_to_CURLE(முழு err)
{
  தேர்வு(err) {
  /* Ordered by order of appearance in libssh2.h */
  நிலை LIBSSH2_ERROR_NONE:
    திரும்பு CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  நிலை LIBSSH2_ERROR_SCP_PROTOCOL:
    திரும்பு CURLE_REMOTE_FILE_NOT_FOUND;

  நிலை LIBSSH2_ERROR_SOCKET_NONE:
    திரும்பு CURLE_COULDNT_CONNECT;

  நிலை LIBSSH2_ERROR_ALLOC:
    திரும்பு CURLE_OUT_OF_MEMORY;

  நிலை LIBSSH2_ERROR_SOCKET_SEND:
    திரும்பு CURLE_SEND_ERROR;

  நிலை LIBSSH2_ERROR_HOSTKEY_INIT:
  நிலை LIBSSH2_ERROR_HOSTKEY_SIGN:
  நிலை LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  நிலை LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    திரும்பு CURLE_PEER_FAILED_VERIFICATION;

  நிலை LIBSSH2_ERROR_PASSWORD_EXPIRED:
    திரும்பு CURLE_LOGIN_DENIED;

  நிலை LIBSSH2_ERROR_SOCKET_TIMEOUT:
  நிலை LIBSSH2_ERROR_TIMEOUT:
    திரும்பு CURLE_OPERATION_TIMEDOUT;

  நிலை LIBSSH2_ERROR_EAGAIN:
    திரும்பு CURLE_AGAIN;
  }

  திரும்பு CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

நிலையான LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (வெறுமை)abstract;
  திரும்பு Curl_cmalloc(மொத்தம்);
}

நிலையான LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (வெறுமை)abstract;
  திரும்பு Curl_crealloc(ptr, மொத்தம்);
}

நிலையான LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (வெறுமை)abstract;
  எனில்(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

நிலையான முழு sshkeycallback(CURL *easy,
                          மாறிலி கட்டமைப்பு curl_khkey *knownkey, /* known */
                          மாறிலி கட்டமைப்பு curl_khkey *foundkey, /* found */
                          எண்ணிக்கை curl_khmatch match,
                          வெறுமை *clientp)
{
  (வெறுமை)easy;
  (வெறுமை)knownkey;
  (வெறுமை)foundkey;
  (வெறுமை)clientp;

  /* we only allow perfect matches, and we reject everything else */
  திரும்பு (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

நிலையான எண்ணிக்கை curl_khtype convert_ssh2_keytype(முழு sshkeytype)
{
  எண்ணிக்கை curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  தேர்வு(sshkeytype) {
  நிலை LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    நிறுத்து;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  நிலை LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    நிறுத்து;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  நிலை LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    நிறுத்து;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  நிலை LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    நிறுத்து;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  நிலை LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    நிறுத்து;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  நிலை LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    நிறுத்து;
#endif
  }
  திரும்பு keytype;
}

நிலையான CURLcode ssh_knownhost(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு ssh_conn *sshc)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு libssh2_knownhost *host = வெற்று;
  மாறிலி எழுத்து *remotekey = வெற்று;
  முழு keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  முழு keybit = 0;
  முழு sshkeytype = 0;
  size_t keylen = 0;
  முழு rc = 0;
  CURLcode result = CURLE_OK;

  எனில்(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
    திரும்பு CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  எனில்(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    கட்டமைப்பு Curl_mapi_guard guard;
    எண்ணிக்கை curl_khmatch keymatch;
    curl_sshkeycallback func =
      data->set.ssh_keyfunc ? data->set.ssh_keyfunc : sshkeycallback;
    கட்டமைப்பு curl_khkey knownkey;
    கட்டமைப்பு curl_khkey *knownkeyp = வெற்று;
    கட்டமைப்பு curl_khkey foundkey;

    தேர்வு(sshkeytype) {
    நிலை LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_சாவி_SSHRSA;
      நிறுத்து;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    நிலை LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_சாவி_SSHDSS;
      நிறுத்து;
#endif
    நிலை LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_சாவி_ECDSA_256;
      நிறுத்து;
    நிலை LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_சாவி_ECDSA_384;
      நிறுத்து;
    நிலை LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_சாவி_ECDSA_521;
      நிறுத்து;
    நிலை LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_சாவி_ED25519;
      நிறுத்து;
    இயல்பு:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      நிறுத்து;
    }
    எனில்(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    இல்லையெனில் {
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
            host->சாவி : "<none>");

      /* setup 'knownkey' */
      எனில்(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.சாவி = host->சாவி;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.சாவி = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (எண்ணிக்கை curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  இல்லையெனில் {
    /* no remotekey means failure! */
    infof(data, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  தேர்வு(rc) {
  இயல்பு: /* unknown return codes is the same as reject */
  நிலை CURLKHSTAT_REJECT:
    infof(data, "SSH: knownhost check failed");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  நிலை CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    result = CURLE_PEER_FAILED_VERIFICATION;
    நிறுத்து;
  நிலை CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    எனில்(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  நிலை CURLKHSTAT_FINE:
  நிலை CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    எனில்(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      முழு addrc;
      மாறிலி எழுத்து *hostbuf;
      எழுத்து *hostport = வெற்று;
      எனில்(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        எனில்(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
      }
      இல்லையெனில்
        hostbuf = conn->origin->hostname;
      எனில்(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, வெற்று,
                                       remotekey, keylen, வெற்று, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, வெற்று);
        எனில்(addrc)
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        இல்லையெனில் எனில்(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          முழு wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          எனில்(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    இல்லையெனில்
      infof(data, "SSH: knownhost entry matches host key");
    நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode ssh_check_fingerprint(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு ssh_conn *sshc)
{
  மாறிலி எழுத்து *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_சாவி_MD5);
  மாறிலி எழுத்து *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_சாவி_SHA256);

  எனில்(pubkey_sha256) {
    மாறிலி எழுத்து *fingerprint = வெற்று;
    எழுத்து *fingerprint_b64 = வெற்று;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    எனில்(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      திரும்பு CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    எனில்(curlx_base64_encode((மாறிலி uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      திரும்பு CURLE_PEER_FAILED_VERIFICATION;
    }

    எனில்(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      திரும்பு CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    போது((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    போது((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    எனில்((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      திரும்பு CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  எனில்(pubkey_md5) {
    எழுத்து md5buffer[33];
    மாறிலி எழுத்து *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    எனில்(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      முழு i;
      க்கு(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned எழுத்து)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    எனில்(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      எனில்(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      இல்லையெனில் {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      திரும்பு CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  எனில்(!pubkey_md5 && !pubkey_sha256) {
    எனில்(data->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      முழு sshkeytype = 0;
      முழு rc = 0;
      /* we handle the process to the callback */
      மாறிலி எழுத்து *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      எனில்(remotekey) {
        கட்டமைப்பு Curl_mapi_guard guard;
        எண்ணிக்கை curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (முழு)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        எனில்(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          திரும்பு CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      இல்லையெனில் {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        திரும்பு CURLE_PEER_FAILED_VERIFICATION;
      }
      திரும்பு CURLE_OK;
    }
    இல்லையெனில் {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      திரும்பு ssh_knownhost(data, sshc);
    }
  }
  இல்லையெனில் {
    /* as we already matched, we skip the check for known hosts */
    திரும்பு CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
நிலையான CURLcode ssh_force_knownhost_சாவி_type(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  நிலையான மாறிலி எழுத்து hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  நிலையான மாறிலி எழுத்து hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  நிலையான மாறிலி எழுத்து hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  நிலையான மாறிலி எழுத்து hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  நிலையான மாறிலி எழுத்து hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_சாவி_SSHDSS
  நிலையான மாறிலி எழுத்து hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  எனில்(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_சாவி_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_சாவி_SHA256)) {
    கட்டமைப்பு libssh2_knownhost *store = வெற்று;
    கட்டமைப்பு connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_சாவி_MASK >>
                      LIBSSH2_KNOWNHOST_சாவி_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    போது(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      எனில்(store) {
        எனில்(store->name) {
          எனில்(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_size = 0;
            மாறிலி எழுத்து *p;
            மாறிலி எழுத்து *kh_name_end = strstr(store->name, "]:");
            எனில்(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              தொடர்;
            }
            p = kh_name_end + 2; /* start of port number */
            எனில்(!curlx_str_எண்(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = strlen(store->name) - 1 - strlen(kh_name_end);
              எனில்((kh_name_size == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                நிறுத்து;
              }
            }
          }
          இல்லையெனில் எனில்(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            நிறுத்து;
          }
        }
        இல்லையெனில் எனில்(store->சாவி) {
          unsigned முழு type = (store->typemask & LIBSSH2_KNOWNHOST_சாவி_MASK)
                              >> LIBSSH2_KNOWNHOST_சாவி_SHIFT;
          எனில்(!absent_type[type]) {
            முழு keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->சாவி, strlen(store->சாவி),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_சாவி_MASK),
              வெற்று);
            எனில்(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              நிறுத்து;
            }
            இல்லையெனில் எனில்(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    எனில்(found) {
      முழு rc;
      மாறிலி எழுத்து *hostkey_method = வெற்று;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      தேர்வு(store->typemask & LIBSSH2_KNOWNHOST_சாவி_MASK) {
      நிலை LIBSSH2_KNOWNHOST_சாவி_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        நிறுத்து;
      நிலை LIBSSH2_KNOWNHOST_சாவி_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        நிறுத்து;
      நிலை LIBSSH2_KNOWNHOST_சாவி_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        நிறுத்து;
      நிலை LIBSSH2_KNOWNHOST_சாவி_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        நிறுத்து;
      நிலை LIBSSH2_KNOWNHOST_சாவி_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        நிறுத்து;
#ifdef LIBSSH2_KNOWNHOST_சாவி_SSHDSS
      நிலை LIBSSH2_KNOWNHOST_சாவி_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        நிறுத்து;
#endif
      நிலை LIBSSH2_KNOWNHOST_சாவி_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        திரும்பு CURLE_SSH;
      இயல்பு:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_சாவி_MASK));
        திரும்பு CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      எனில்(rc) {
        எழுத்து *err_செய்தி = வெற்று;
        முழு errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_செய்தி);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    இல்லையெனில் {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  திரும்பு result;
}

நிலையான CURLcode quote_error(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  திரும்பு CURLE_QUOTE_ERROR;
}

நிலையான CURLcode sftp_quote(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு ssh_conn *sshc,
                           கட்டமைப்பு SSHPROTO *sshp)
{
  மாறிலி எழுத்து *cp;
  CURLcode result = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  மாறிலி எழுத்து *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  எனில்(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  எனில்(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    எழுத்து *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    எனில்(!tmp)
      திரும்பு CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    எனில்(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    திரும்பு result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  எனில்(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    திரும்பு CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  எனில்(result) {
    எனில்(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    திரும்பு result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  எனில்(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    எனில்(result) {
      எனில்(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      திரும்பு result;
    }
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);

    memset(&sshp->quote_attrs, 0, அளவு(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    திரும்பு result;
  }
  எனில்(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    எனில்(result) {
      எனில்(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      திரும்பு result;
    }
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    திரும்பு result;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "mkdir ", 6)) {
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    திரும்பு result;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    எனில்(result) {
      எனில்(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      திரும்பு result;
    }
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    திரும்பு result;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "rmdir ", 6)) {
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    திரும்பு result;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "rm ", 3)) {
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    திரும்பு result;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "statvfs ", 8)) {
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    திரும்பு result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  திரும்பு CURLE_QUOTE_ERROR;
}

நிலையான CURLcode sftp_upload_init(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு ssh_conn *sshc,
                                 கட்டமைப்பு SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  எனில்(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    எனில்(data->state.resume_from < 0) {
      முழு rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      எனில்(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        திரும்பு CURLE_OK;
      }
      எனில்(rc) {
        data->state.resume_from = 0;
      }
      இல்லையெனில் {
        curl_off_t size = attrs.filesize;
        எனில்(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          திரும்பு CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  எனில்(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  இல்லையெனில் எனில்(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  இல்லையெனில் {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  எனில்(!sshc->sftp_handle) {
    CURLcode result;
    unsigned long sftperr;
    முழு rc = libssh2_session_last_errno(sshc->ssh_session);

    எனில்(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      திரும்பு CURLE_OK;
    }

    எனில்(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    இல்லையெனில்
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    எனில்(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      திரும்பு sftp_libssh2_error_to_CURLE(sftperr);
    }
    எனில்((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      திரும்பு CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    எனில்(!result) {
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
    திரும்பு result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  எனில்(data->state.resume_from > 0 && !data->set.remote_append) {
    முழு seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    எனில்(data->set.seek_func) {
      கட்டமைப்பு Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    எனில்(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      எனில்(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        திரும்பு CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      செய் {
        கட்டமைப்பு Curl_mapi_guard guard;
        எழுத்து scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)அளவு(scratch)) ?
          அளவு(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        எனில்((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          திரும்பு CURLE_FTP_COULDNT_USE_REST;
        }
      } போது(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    எனில்(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  எனில்(data->state.infilesize > 0) {
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
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை ssh_state_pkey_init(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  எனில்((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    எனில்(sshc->pub_சாவி)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_சாவி);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_சாவி);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  இல்லையெனில்
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
}

நிலையான CURLcode sftp_quote_stat(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு ssh_conn *sshc,
                                கட்டமைப்பு SSHPROTO *sshp,
                                bool *blockp)
{
  மாறிலி எழுத்து *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  எனில்(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  எனில்(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    முழு rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      திரும்பு CURLE_OK;
    }
    எனில்(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      செல் fail;
    }
  }

  /* Now set the new attributes... */
  எனில்(!strncmp(cmd, "chgrp", 5)) {
    மாறிலி எழுத்து *p = sshc->quote_path1;
    curl_off_t gid;
    எனில்(!curlx_str_எண்(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    இல்லையெனில் எனில்(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      செல் fail;
    }
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    மாறிலி எழுத்து *p = sshc->quote_path1;
    /* permissions are octal */
    எனில்(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      செல் fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "chown", 5)) {
    மாறிலி எழுத்து *p = sshc->quote_path1;
    curl_off_t uid;
    எனில்(!curlx_str_எண்(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    இல்லையெனில் எனில்(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      செல் fail;
    }
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    எனில்(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#எனில் SIZEOF_TIME_T > SIZEOF_LONG
    எனில்(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    எனில்(fail)
      செல் fail;
    எனில்(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    இல்லையெனில் /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  திரும்பு CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  திரும்பு CURLE_QUOTE_ERROR;
}

நிலையான CURLcode sftp_download_stat(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ssh_conn *sshc,
                                   கட்டமைப்பு SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  முழு rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    திரும்பு CURLE_OK;
  }
  எனில்(rc ||
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
  இல்லையெனில் {
    curl_off_t size = attrs.filesize;

    எனில்(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      திரும்பு CURLE_BAD_DOWNLOAD_RESUME;
    }
    எனில்(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      எனில்(result)
        திரும்பு result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  எனில்(data->state.resume_from) {
    எனில்(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      எனில்((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        திரும்பு CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    இல்லையெனில் {
      எனில்((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        திரும்பு CURLE_BAD_DOWNLOAD_RESUME;
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
  எனில்(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    திரும்பு CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  திரும்பு CURLE_OK;
}

நிலையான CURLcode sftp_readdir(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு ssh_conn *sshc,
                             கட்டமைப்பு SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode result = CURLE_OK;
  முழு rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    திரும்பு result;
  }
  எனில்(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    எனில்(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      எனில்(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      எனில்(result)
        திரும்பு result;
    }
    இல்லையெனில் {
      result = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      எனில்(!result) {
        எனில்((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        இல்லையெனில் {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      திரும்பு result;
    }
  }
  இல்லையெனில் எனில்(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  இல்லையெனில் {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  திரும்பு result;
}

நிலையான CURLcode ssh_state_init(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு ssh_conn *sshc)
{
  CURLcode result;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  result = ssh_force_knownhost_சாவி_type(data, sshc);
  எனில்(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  இல்லையெனில்
    myssh_to(data, sshc, SSH_S_STARTUP);
  திரும்பு result;
}

நிலையான CURLcode ssh_state_startup(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ssh_conn *sshc)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  முழு rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc) {
    எழுத்து *err_செய்தி = வெற்று;
    (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_செய்தி);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    திரும்பு CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_hostkey(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode result = ssh_check_fingerprint(data, sshc);
  எனில்(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  திரும்பு result;
}

நிலையான CURLcode ssh_state_authlist(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ssh_conn *sshc)
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
  கட்டமைப்பு connectdata *conn = data->conn;
  மாறிலி எழுத்து *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(strlen(user)));

  எனில்(!sshc->authlist) {
    முழு rc;
    எனில்(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      திரும்பு CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    திரும்பு libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_auth_pkey(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  கட்டமைப்பு connectdata *conn = data->conn;
  மாறிலி எழுத்து *user = Curl_creds_user(conn->creds);
  முழு rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_சாவி, sshc->priv_சாவி, sshc->passphrase ? sshc->passphrase : "");
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  இல்லையெனில் {
    எழுத்து *err_செய்தி = வெற்று;
    எழுத்து unknown[] = "Reason unknown (-1)";
    எனில்(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_செய்தி = unknown;
    }
    இல்லையெனில் {
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_செய்தி);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_auth_pass_init(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு ssh_conn *sshc)
{
  எனில்((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_auth_pass(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssh_conn *sshc)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  மாறிலி எழுத்து *user = Curl_creds_user(conn->creds);
  மாறிலி எழுத்து *passwd = Curl_creds_passwd(conn->creds);
  முழு rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 வெற்று);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN) {
    திரும்பு CURLE_AGAIN;
  }
  எனில்(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_auth_host_init(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு ssh_conn *sshc)
{
  எனில்((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_auth_agent_init(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = 0;
  எனில்((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    எனில்(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      எனில்(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_சாவி_INIT);
        திரும்பு CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;
    எனில்(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_சாவி_INIT);
    }
    இல்லையெனில் {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  இல்லையெனில்
    myssh_to(data, sshc, SSH_AUTH_சாவி_INIT);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_auth_agent_list(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = libssh2_agent_list_identities(sshc->ssh_agent);

  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;
  எனில்(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_சாவி_INIT);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = வெற்று;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_auth_agent(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  முழு rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_prev_identity);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    எனில்(rc < 0) {
      எனில்(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        திரும்பு CURLE_OK;
      }
      திரும்பு CURLE_AGAIN;
    }
  }

  எனில்(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  இல்லையெனில் எனில்(rc == 1)
    infof(data, "SSH: no agent identity would match");

  எனில்(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_AUTH_சாவி_INIT);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_auth_சாவி_init(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு ssh_conn *sshc)
{
  எனில்((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_சாவி);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_auth_சாவி(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  கட்டமைப்பு connectdata *conn = data->conn;
  மாறிலி எழுத்து *user = Curl_creds_user(conn->creds);
  முழு rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    திரும்பு CURLE_OK;
  }
  திரும்பு CURLE_LOGIN_DENIED;
}

நிலையான CURLcode ssh_state_auth_done(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssh_conn *sshc)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  எனில்(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    திரும்பு CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  எனில்(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    திரும்பு CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_init(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  எனில்(!sshc->sftp_session) {
    எழுத்து *err_செய்தி = வெற்று;
    எனில்(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
    failf(data, "Failure initializing sftp session: %s", err_செய்தி);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    திரும்பு CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_realpath(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு ssh_conn *sshc,
                                        கட்டமைப்பு SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  முழு rc;

  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  எனில்(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    எனில்(!sshc->homedir)
      திரும்பு CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    எனில்(!data->state.most_recent_ftp_entrypath)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }
  இல்லையெனில் {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    எனில்(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    இல்லையெனில்
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (முழு)result);
    திரும்பு result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_quote_init(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு ssh_conn *sshc,
                                          கட்டமைப்பு SSHPROTO *sshp)
{
  CURLcode result;

  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  எனில்(result) {
    myssh_to(data, sshc, SSH_STOP);
    திரும்பு result;
  }

  எனில்(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_postquote_init(கட்டமைப்பு Curl_easy *data,
                                              கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_STOP);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_quote(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு ssh_conn *sshc,
                                     கட்டமைப்பு SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  எனில்(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  திரும்பு result;
}

நிலையான CURLcode ssh_state_sftp_next_quote(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  எனில்(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  இல்லையெனில் {
    எனில்(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    இல்லையெனில் {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_quote_stat(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு ssh_conn *sshc,
                                          கட்டமைப்பு SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  எனில்(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  திரும்பு result;
}

நிலையான CURLcode ssh_state_sftp_quote_setstat(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு ssh_conn *sshc,
                                             கட்டமைப்பு SSHPROTO *sshp)
{
  முழு rc;

  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    திரும்பு CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_quote_symlink(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    திரும்பு CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_quote_mkdir(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)data->set.new_directory_perms);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    திரும்பு CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_quote_rename(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    திரும்பு CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_quote_rmdir(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    திரும்பு CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_quote_unlink(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    திரும்பு CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_quote_statvfs(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  முழு rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    திரும்பு CURLE_QUOTE_ERROR;
  }
  இல்லையெனில் எனில்(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#இல்லையெனில்
#define CURL_LIBSSH2_VFS_SIZE_MASK "llu"
#endif
    CURLcode result;
    எழுத்து *tmp = curl_maprintf("statvfs:\n"
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
    எனில்(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      திரும்பு CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    எனில்(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      திரும்பு result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_create_dirs_mkdir(கட்டமைப்பு Curl_easy *data,
                                                 கட்டமைப்பு ssh_conn *sshc,
                                                 கட்டமைப்பு SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  முழு rc;

  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)data->set.new_directory_perms);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  எனில்(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    எனில்((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      திரும்பு sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_readdir_init(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு ssh_conn *sshc,
                                            கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  எனில்(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    திரும்பு CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  எனில்(!sshc->sftp_handle) {
    unsigned long sftperr;
    எனில்(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    திரும்பு sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_readdir_link(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு ssh_conn *sshc,
                                            கட்டமைப்பு SSHPROTO *sshp)
{
  CURLcode result;
  முழு rc;
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned முழு)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  எனில்(rc < 0)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  எனில்(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  இல்லையெனில்
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  திரும்பு result;
}

நிலையான CURLcode ssh_state_scp_download_init(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு ssh_conn *sshc,
                                            கட்டமைப்பு SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, அளவு(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  எனில்(!sshc->ssh_channel) {
    முழு ssh_err;
    எழுத்து *err_செய்தி = வெற்று;

    எனில்(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
    failf(data, "%s", err_செய்தி);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    திரும்பு libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_close(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு ssh_conn *sshc,
                                     கட்டமைப்பு SSHPROTO *sshp)
{
  முழு rc = 0;
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    எனில்(rc < 0) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_செய்தி);
    }
    sshc->sftp_handle = வெற்று;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  எனில்(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  இல்லையெனில்
    myssh_to(data, sshc, SSH_STOP);

  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_shutdown(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  முழு rc = 0;
  எனில்(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    எனில்(rc < 0) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_செய்தி);
    }
    sshc->sftp_handle = வெற்று;
  }
  எனில்(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    எனில்(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = வெற்று;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_download_init(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு ssh_conn *sshc,
                                             கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  எனில்(!sshc->sftp_handle) {
    unsigned long sftperr;
    எனில்(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      திரும்பு CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    திரும்பு sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_scp_upload_init(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு ssh_conn *sshc,
                                          கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (முழு)data->set.new_file_perms,
                       (libssh2_int64_t)data->state.infilesize, 0, 0);
  எனில்(!sshc->ssh_channel) {
    முழு ssh_err;
    எழுத்து *err_செய்தி = வெற்று;
    CURLcode result;
    எனில்(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
    failf(data, "%s", err_செய்தி);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    எனில்(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    திரும்பு result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_session_disconnect(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  முழு rc = 0;
  எனில்(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    எனில்(rc < 0) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_செய்தி);
    }
    sshc->ssh_channel = வெற்று;
  }

  எனில்(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;

    எனில்(rc < 0) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_செய்தி);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode sshc_cleanup(கட்டமைப்பு ssh_conn *sshc, கட்டமைப்பு Curl_easy *data,
                             bool block)
{
  முழு rc;

  எனில்(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = வெற்று;
  }

  எனில்(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    எனில்((rc < 0) && data) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_செய்தி);
    }
    எனில்(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      திரும்பு CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = வெற்று;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = வெற்று;
    sshc->sshagent_prev_identity = வெற்று;
  }

  எனில்(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    எனில்((rc < 0) && data) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_செய்தி);
    }
    எனில்(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      திரும்பு CURLE_AGAIN;

    sshc->sftp_handle = வெற்று;
  }

  எனில்(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    எனில்((rc < 0) && data) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_செய்தி);
    }
    எனில்(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      திரும்பு CURLE_AGAIN;

    sshc->ssh_channel = வெற்று;
  }

  எனில்(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    எனில்((rc < 0) && data) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_செய்தி);
    }
    எனில்(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      திரும்பு CURLE_AGAIN;

    sshc->sftp_session = வெற்று;
  }

  எனில்(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    எனில்((rc < 0) && data) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session, &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_செய்தி);
    }
    எனில்(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      திரும்பு CURLE_AGAIN;

    sshc->ssh_session = வெற்று;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_சாவி);
  curlx_safefree(sshc->priv_சாவி);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_getinfo(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  இல்லையெனில்
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_filetime(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு ssh_conn *sshc,
                                        கட்டமைப்பு SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  முழு rc;

  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  எனில்(rc == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  எனில்(rc == 0)
    data->info.filetime = (time_t)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_trans_init(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு ssh_conn *sshc,
                                          கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  இல்லையெனில் எனில்(sshp) {
    size_t plen = strlen(sshp->path);
    எனில்(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    இல்லையெனில்
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  இல்லையெனில்
    திரும்பு CURLE_FAILED_INIT;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_upload_init(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு ssh_conn *sshc,
                                           கட்டமைப்பு SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  எனில்(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  திரும்பு result;
}

நிலையான CURLcode ssh_state_sftp_create_dirs_init(கட்டமைப்பு Curl_easy *data,
                                                கட்டமைப்பு ssh_conn *sshc,
                                                கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  இல்லையெனில்
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_create_dirs(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு ssh_conn *sshc,
                                           கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  எனில்(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    திரும்பு CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_readdir(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc,
                                       கட்டமைப்பு SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  எனில்(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  திரும்பு result;
}

நிலையான CURLcode ssh_state_sftp_readdir_bottom(கட்டமைப்பு Curl_easy *data,
                                              கட்டமைப்பு ssh_conn *sshc,
                                              கட்டமைப்பு SSHPROTO *sshp)
{
  CURLcode result;
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  எனில்(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  எனில்(result) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(data, sshc, SSH_STOP);
  }
  இல்லையெனில் {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  திரும்பு result;
}

நிலையான CURLcode ssh_state_sftp_readdir_done(கட்டமைப்பு Curl_easy *data,
                                            கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;

  sshc->sftp_handle = வெற்று;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_sftp_download_stat(கட்டமைப்பு Curl_easy *data,
                                             கட்டமைப்பு ssh_conn *sshc,
                                             கட்டமைப்பு SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  எனில்(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  திரும்பு result;
}

நிலையான CURLcode ssh_state_scp_trans_init(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு ssh_conn *sshc,
                                         கட்டமைப்பு SSHPROTO *sshp)
{
  CURLcode result;
  எனில்(!sshp)
    திரும்பு CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  எனில்(result) {
    myssh_to(data, sshc, SSH_STOP);
    திரும்பு result;
  }

  எனில்(data->state.upload) {
    எனில்(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      திரும்பு CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  இல்லையெனில்
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_scp_done(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  இல்லையெனில்
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_scp_send_eof(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->ssh_channel) {
    முழு rc = libssh2_channel_send_eof(sshc->ssh_channel);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;
    எனில்(rc) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session,
                                       &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_செய்தி);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_scp_wait_eof(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->ssh_channel) {
    முழு rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;
    எனில்(rc) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session,
                                       &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_செய்தி);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_scp_wait_close(கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->ssh_channel) {
    முழு rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;
    எனில்(rc) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session,
                                       &err_செய்தி, வெற்று, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_செய்தி);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_scp_channel_free(
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->ssh_channel) {
    முழு rc = libssh2_channel_free(sshc->ssh_channel);
    எனில்(rc == LIBSSH2_ERROR_EAGAIN)
      திரும்பு CURLE_AGAIN;
    எனில்(rc < 0) {
      எழுத்து *err_செய்தி = வெற்று;
      (வெறுமை)libssh2_session_last_error(sshc->ssh_session,
                                       &err_செய்தி, வெற்று, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_செய்தி);
    }
    sshc->ssh_channel = வெற்று;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssh_state_session_free(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  கட்டமைப்பு connectdata *conn = data->conn;
  எனில்(result)
    திரும்பு result;
  memset(sshc, 0, அளவு(கட்டமைப்பு ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  திரும்பு CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
நிலையான CURLcode ssh_statemachine(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு ssh_conn *sshc,
                                 கட்டமைப்பு SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode result = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  செய் {
    தேர்வு(sshc->state) {
    நிலை SSH_INIT:
      result = ssh_state_init(data, sshc);
      எனில்(result)
        நிறுத்து;
      FALLTHROUGH();

    நிலை SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      எனில்(result)
        நிறுத்து;
      FALLTHROUGH();

    நிலை SSH_HOSTKEY:
      result = ssh_state_hostkey(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTHLIST:
      result = ssh_state_authlist(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_PKEY:
      result = ssh_state_auth_pkey(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_PASS_INIT:
      result = ssh_state_auth_pass_init(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_PASS:
      result = ssh_state_auth_pass(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_HOST_INIT:
      result = ssh_state_auth_host_init(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_HOST:
      myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
      நிறுத்து;

    நிலை SSH_AUTH_AGENT_INIT:
      result = ssh_state_auth_agent_init(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_AGENT_LIST:
      result = ssh_state_auth_agent_list(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_AGENT:
      result = ssh_state_auth_agent(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_சாவி_INIT:
      result = ssh_state_auth_சாவி_init(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_சாவி:
      result = ssh_state_auth_சாவி(data, sshc);
      நிறுத்து;

    நிலை SSH_AUTH_DONE:
      result = ssh_state_auth_done(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_INIT:
      result = ssh_state_sftp_init(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_REALPATH:
      result = ssh_state_sftp_realpath(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE_INIT:
      result = ssh_state_sftp_quote_init(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_POSTQUOTE_INIT:
      result = ssh_state_sftp_postquote_init(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE:
      result = ssh_state_sftp_quote(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_NEXT_QUOTE:
      result = ssh_state_sftp_next_quote(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE_STAT:
      result = ssh_state_sftp_quote_stat(data, sshc, sshp, block);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE_SETSTAT:
      result = ssh_state_sftp_quote_setstat(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE_SYMLINK:
      result = ssh_state_sftp_quote_symlink(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE_MKDIR:
      result = ssh_state_sftp_quote_mkdir(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE_RENAME:
      result = ssh_state_sftp_quote_rename(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE_RMDIR:
      result = ssh_state_sftp_quote_rmdir(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE_UNLINK:
      result = ssh_state_sftp_quote_unlink(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_QUOTE_STATVFS:
      result = ssh_state_sftp_quote_statvfs(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_GETINFO:
      result = ssh_state_sftp_getinfo(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_FILETIME:
      result = ssh_state_sftp_filetime(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_TRANS_INIT:
      result = ssh_state_sftp_trans_init(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_UPLOAD_INIT:
      result = ssh_state_sftp_upload_init(data, sshc, sshp, block);
      நிறுத்து;

    நிலை SSH_SFTP_CREATE_DIRS_INIT:
      result = ssh_state_sftp_create_dirs_init(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_CREATE_DIRS:
      result = ssh_state_sftp_create_dirs(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_CREATE_DIRS_MKDIR:
      result = ssh_state_sftp_create_dirs_mkdir(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_READDIR_INIT:
      result = ssh_state_sftp_readdir_init(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_READDIR:
      result = ssh_state_sftp_readdir(data, sshc, sshp, block);
      நிறுத்து;

    நிலை SSH_SFTP_READDIR_LINK:
      result = ssh_state_sftp_readdir_link(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_READDIR_BOTTOM:
      result = ssh_state_sftp_readdir_bottom(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_READDIR_DONE:
      result = ssh_state_sftp_readdir_done(data, sshc);
      நிறுத்து;

    நிலை SSH_SFTP_DOWNLOAD_INIT:
      result = ssh_state_sftp_download_init(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_DOWNLOAD_STAT:
      result = ssh_state_sftp_download_stat(data, sshc, sshp, block);
      நிறுத்து;

    நிலை SSH_SFTP_CLOSE:
      result = ssh_state_sftp_close(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SFTP_SHUTDOWN:
      result = ssh_state_sftp_shutdown(data, sshc);
      நிறுத்து;

    நிலை SSH_SCP_TRANS_INIT:
      result = ssh_state_scp_trans_init(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SCP_UPLOAD_INIT:
      result = ssh_state_scp_upload_init(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SCP_DOWNLOAD_INIT:
      result = ssh_state_scp_download_init(data, sshc, sshp);
      நிறுத்து;

    நிலை SSH_SCP_DONE:
      result = ssh_state_scp_done(data, sshc);
      நிறுத்து;

    நிலை SSH_SCP_SEND_EOF:
      result = ssh_state_scp_send_eof(data, sshc);
      நிறுத்து;

    நிலை SSH_SCP_WAIT_EOF:
      result = ssh_state_scp_wait_eof(data, sshc);
      நிறுத்து;

    நிலை SSH_SCP_WAIT_CLOSE:
      result = ssh_state_scp_wait_close(data, sshc);
      நிறுத்து;

    நிலை SSH_SCP_CHANNEL_FREE:
      result = ssh_state_scp_channel_free(data, sshc);
      நிறுத்து;

    நிலை SSH_SESSION_DISCONNECT:
      result = ssh_state_session_disconnect(data, sshc);
      நிறுத்து;

    நிலை SSH_SESSION_FREE:
      result = ssh_state_session_free(data, sshc);
      நிறுத்து;

    நிலை SSH_QUIT:
    இயல்பு:
      /* internal error */
      myssh_to(data, sshc, SSH_STOP);
      நிறுத்து;
    }

  } போது(!result && (sshc->state != SSH_STOP) && !*block);

  எனில்(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (முழு)result, *block);

  திரும்பு result;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
நிலையான வெறுமை ssh_block2waitfor(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு ssh_conn *sshc,
                              bool block)
{
  முழு dir = 0;
  (வெறுமை)data;
  எனில்(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    எனில்(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  எனில்(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
நிலையான CURLcode ssh_multi_statemach(கட்டமைப்பு Curl_easy *data, bool *done)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  கட்டமைப்பு SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  எனில்(!sshc || !sshp)
    திரும்பு CURLE_FAILED_INIT;

  செய் {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } போது(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  திரும்பு result;
}

நிலையான CURLcode ssh_block_statemach(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssh_conn *sshc,
                                    கட்டமைப்பு SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு curltime start = *Curl_pgrs_now(data);

  போது((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t இடது_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    எனில்(result)
      நிறுத்து;

    எனில்(!disconnect) {
      result = Curl_pgrsCheck(data);
      எனில்(result)
        நிறுத்து;

      இடது_ms = Curl_timeleft_ms(data);
      எனில்(இடது_ms < 0) {
        failf(data, "Operation timed out");
        திரும்பு CURLE_OPERATION_TIMEDOUT;
      }
    }
    இல்லையெனில் எனில்(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      நிறுத்து;
    }

    எனில்(block) {
      முழு dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      எனில்(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      எனில்(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (வெறுமை)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!இடது_ms || இடது_ms > 1000) ? 1000 : இடது_ms);
    }
  }

  திரும்பு result;
}

நிலையான வெறுமை myssh_easy_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு SSHPROTO *sshp = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

நிலையான வெறுமை myssh_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு ssh_conn *sshc = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  sshc_cleanup(sshc, வெற்று, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
நிலையான CURLcode ssh_setup_connection(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு ssh_conn *sshc;
  கட்டமைப்பு SSHPROTO *sshp;
  (வெறுமை)conn;

  sshc = curlx_calloc(1, அளவு(*sshc));
  எனில்(!sshc)
    திரும்பு CURLE_OUT_OF_MEMORY;

  எனில்(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, அளவு(*sshp));
  எனில்(!sshp)
    திரும்பு CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  எனில்(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு Curl_ssh_setup_pkey(data, sshc);
}

நிலையான Curl_recv scp_recv, sftp_recv;
நிலையான Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
நிலையான ssize_t ssh_tls_recv(libssh2_socket_t sock, வெறுமை *buffer,
                            size_t length, முழு flags, வெறுமை **abstract)
{
  கட்டமைப்பு Curl_easy *data = (கட்டமைப்பு Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nread;
  CURLcode result;
  கட்டமைப்பு connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (வெறுமை)flags;

  எனில்(!sshc)
    திரும்பு -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  எனில்(result == CURLE_AGAIN)
    திரும்பு -EAGAIN; /* magic return code for libssh2 */
  இல்லையெனில் எனில்(result)
    திரும்பு -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (மாறிலி எழுத்து *)buffer, nread);
  திரும்பு (ssize_t)nread;
}

நிலையான ssize_t ssh_tls_send(libssh2_socket_t sock, மாறிலி வெறுமை *buffer,
                            size_t length, முழு flags, வெறுமை **abstract)
{
  கட்டமைப்பு Curl_easy *data = (கட்டமைப்பு Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nwrite;
  CURLcode result;
  கட்டமைப்பு connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (வெறுமை)flags;

  எனில்(!sshc)
    திரும்பு -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  எனில்(result == CURLE_AGAIN)
    திரும்பு -EAGAIN; /* magic return code for libssh2 */
  இல்லையெனில் எனில்(result)
    திரும்பு -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (மாறிலி எழுத்து *)buffer, nwrite);
  திரும்பு (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
நிலையான CURLcode ssh_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result;

#எனில் LIBSSH2_VERSION_இலக்கம் >= 0x010b00
  {
    மாறிலி எழுத்து *crypto_str;
    தேர்வு(libssh2_crypto_engine()) {
    நிலை libssh2_gcrypt:
      crypto_str = "libgcrypt";
      நிறுத்து;
    நிலை libssh2_mbedtls:
      crypto_str = "mbedTLS";
      நிறுத்து;
    நிலை libssh2_openssl:
      crypto_str = "openssl compatible";
      நிறுத்து;
    நிலை libssh2_os400qc3:
      crypto_str = "OS400QC3";
      நிறுத்து;
    நிலை libssh2_wincng:
      crypto_str = "WinCNG";
      நிறுத்து;
    இயல்பு:
      crypto_str = வெற்று;
      நிறுத்து;
    }
    எனில்(crypto_str)
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

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

  எனில்(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    திரும்பு CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#எனில் LIBSSH2_VERSION_இலக்கம் >= 0x010B00
  எனில்(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  எனில்(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#எனில் LIBSSH2_VERSION_இலக்கம் >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic எடு
#endif
#இல்லையெனில்
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    ஒன்றிப்பு receive {
      வெறுமை *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, வெறுமை *, size_t, முழு, வெறுமை **);
    };
    ஒன்றிப்பு transfer {
      வெறுமை *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, மாறிலி வெறுமை *, size_t, முழு, வெறுமை **);
    };
    ஒன்றிப்பு receive sshrecv;
    ஒன்றிப்பு transfer sshsend;

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
  எனில்(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  இல்லையெனில் {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  எனில்(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  எனில்(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    முழு rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    எனில்(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = வெற்று;
      திரும்பு CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    எனில்(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (முழு)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  திரும்பு result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

நிலையான CURLcode scp_perform(கட்டமைப்பு Curl_easy *data,
                            bool *connected,
                            bool *dophase_done)
{
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  எனில்(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  திரும்பு result;
}

/* called from multi.c while DOing */
நிலையான CURLcode scp_doing(கட்டமைப்பு Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  எனில்(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  திரும்பு result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
நிலையான CURLcode scp_disconnect(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  கட்டமைப்பு SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (வெறுமை)dead_connection;

  எனில்(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  எனில்(sshc)
    திரும்பு sshc_cleanup(sshc, data, TRUE);
  திரும்பு result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
நிலையான CURLcode ssh_done(கட்டமைப்பு Curl_easy *data, CURLcode status)
{
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  கட்டமைப்பு SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  எனில்(!sshc || !sshp)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  இல்லையெனில்
    result = status;

  எனில்(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    திரும்பு CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  திரும்பு result;
}

நிலையான CURLcode scp_done(கட்டமைப்பு Curl_easy *data, CURLcode status,
                         bool premature)
{
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (வெறுமை)premature;

  எனில்(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  திரும்பு ssh_done(data, status);
}

நிலையான CURLcode scp_send(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                         மாறிலி uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (வெறுமை)sockindex; /* we only support SCP on the fixed known primary socket */
  (வெறுமை)eos;
  *pnwritten = 0;

  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (மாறிலி எழுத்து *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  எனில்(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  இல்லையெனில் எனில்(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((முழு)nwritten);
  இல்லையெனில்
    *pnwritten = (size_t)nwritten;

  திரும்பு result;
}

நிலையான CURLcode scp_recv(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                         எழுத்து *mem, size_t len, size_t *pnread)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (வெறுமை)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  எனில்(nread == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;
  இல்லையெனில் எனில்(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((முழு)nread);
  இல்லையெனில்
    *pnread = (size_t)nread;

  திரும்பு result;
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

நிலையான CURLcode sftp_perform(கட்டமைப்பு Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  எனில்(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  திரும்பு result;
}

/* called from multi.c while DOing */
நிலையான CURLcode sftp_doing(கட்டமைப்பு Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  எனில்(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  திரும்பு result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
நிலையான CURLcode sftp_disconnect(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn, bool dead_connection)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  கட்டமைப்பு SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (வெறுமை)dead_connection;

  எனில்(sshc) {
    எனில்(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (முழு)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  திரும்பு result;
}

நிலையான CURLcode sftp_done(கட்டமைப்பு Curl_easy *data, CURLcode status,
                          bool premature)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  எனில்(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    எனில்(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  திரும்பு ssh_done(data, status);
}

/* return number of sent bytes */
நிலையான CURLcode sftp_send(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                          மாறிலி uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (வெறுமை)sockindex;
  (வெறுமை)eos;
  *pnwritten = 0;

  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (மாறிலி எழுத்து *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  எனில்(nwrite == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;
  இல்லையெனில் எனில்(nwrite < LIBSSH2_ERROR_NONE)
    திரும்பு libssh2_session_error_to_CURLE((முழு)nwrite);
  *pnwritten = (size_t)nwrite;
  திரும்பு CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
நிலையான CURLcode sftp_recv(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                          எழுத்து *mem, size_t len, size_t *pnread)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (வெறுமை)sockindex;
  *pnread = 0;

  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  எனில்(nread == LIBSSH2_ERROR_EAGAIN)
    திரும்பு CURLE_AGAIN;
  இல்லையெனில் எனில்(nread < 0)
    திரும்பு libssh2_session_error_to_CURLE((முழு)nread);

  *pnread = (size_t)nread;
  திரும்பு CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
நிலையான CURLcode ssh_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  எனில்(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  இல்லையெனில்
    result = sftp_perform(data, &connected, done);

  திரும்பு result;
}

CURLcode Curl_ssh_init(வெறுமை)
{
  எனில்(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    திரும்பு CURLE_FAILED_INIT;
  }
  திரும்பு CURLE_OK;
}

வெறுமை Curl_ssh_cleanup(வெறுமை)
{
  libssh2_exit();
}

வெறுமை Curl_ssh_version(எழுத்து *buffer, size_t buflen)
{
  (வெறுமை)curl_msnprintf(buffer, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
நிலையான வெறுமை ssh_attach(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு connectdata *conn)
{
  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  எனில்(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    எனில்(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      வெறுமை **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = data;
    }
  }
}

/*
 * SCP protocol handler.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_scp = {
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
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_sftp = {
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
