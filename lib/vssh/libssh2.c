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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "vssh/ssh.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "parsedate.h" /* for the week day and month names */
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"
#અન્તર્ભાવ "vssh/vssh.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curlx/base64.h" /* for curlx_base64_encode() */

સ્થિર અચલ અક્ષર *sftp_libssh2_strerror(unsigned long err)
{
  પસંદગી(err) {
  સ્થિતિ LIBSSH2_FX_NO_SUCH_FILE:
    પાછા "No such file or directory";

  સ્થિતિ LIBSSH2_FX_PERMISSION_DENIED:
    પાછા "Permission denied";

  સ્થિતિ LIBSSH2_FX_FAILURE:
    પાછા "Operation failed";

  સ્થિતિ LIBSSH2_FX_BAD_MESSAGE:
    પાછા "Bad message from SFTP server";

  સ્થિતિ LIBSSH2_FX_NO_CONNECTION:
    પાછા "Not connected to SFTP server";

  સ્થિતિ LIBSSH2_FX_CONNECTION_LOST:
    પાછા "Connection to SFTP server lost";

  સ્થિતિ LIBSSH2_FX_OP_UNSUPPORTED:
    પાછા "Operation not supported by SFTP server";

  સ્થિતિ LIBSSH2_FX_INVALID_HANDLE:
    પાછા "Invalid handle";

  સ્થિતિ LIBSSH2_FX_NO_SUCH_PATH:
    પાછા "No such file or directory";

  સ્થિતિ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    પાછા "File already exists";

  સ્થિતિ LIBSSH2_FX_WRITE_PROTECT:
    પાછા "File is write protected";

  સ્થિતિ LIBSSH2_FX_NO_MEDIA:
    પાછા "No media";

  સ્થિતિ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    પાછા "Disk full";

  સ્થિતિ LIBSSH2_FX_QUOTA_EXCEEDED:
    પાછા "User quota exceeded";

  સ્થિતિ LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    પાછા "Unknown principal";

  સ્થિતિ LIBSSH2_FX_LOCK_CONFLICT:
    પાછા "File lock conflict";

  સ્થિતિ LIBSSH2_FX_DIR_NOT_EMPTY:
    પાછા "Directory not empty";

  સ્થિતિ LIBSSH2_FX_NOT_A_DIRECTORY:
    પાછા "Not a directory";

  સ્થિતિ LIBSSH2_FX_INVALID_FILENAME:
    પાછા "Invalid filename";

  સ્થિતિ LIBSSH2_FX_LINK_LOOP:
    પાછા "Link points to itself";
  }
  પાછા "Unknown error in libssh2";
}

સ્થિર શૂન્ય kbd_callback(અચલ અક્ષર *name, પૂર્ણાંક name_len,
                         અચલ અક્ષર *instruction, પૂર્ણાંક instruction_len,
                         પૂર્ણાંક અંક_prompts,
                         અચલ LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         શૂન્ય **abstract)
{
  સંરચના Curl_easy *data = (સંરચના Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", અંક_prompts);
#નહીંતર
  (શૂન્ય)name;
  (શૂન્ય)name_len;
  (શૂન્ય)instruction;
  (શૂન્ય)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  જો(અંક_prompts == 1) {
    સંરચના connectdata *conn = data->conn;
    અચલ અક્ષર *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (શૂન્ય)prompts;
} /* kbd_callback */

સ્થિર CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  પસંદગી(err) {
  સ્થિતિ LIBSSH2_FX_OK:
    પાછા CURLE_OK;

  સ્થિતિ LIBSSH2_FX_NO_SUCH_FILE:
  સ્થિતિ LIBSSH2_FX_NO_SUCH_PATH:
    પાછા CURLE_REMOTE_FILE_NOT_FOUND;

  સ્થિતિ LIBSSH2_FX_PERMISSION_DENIED:
  સ્થિતિ LIBSSH2_FX_WRITE_PROTECT:
  સ્થિતિ LIBSSH2_FX_LOCK_CONFLICT:
    પાછા CURLE_REMOTE_ACCESS_DENIED;

  સ્થિતિ LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  સ્થિતિ LIBSSH2_FX_QUOTA_EXCEEDED:
    પાછા CURLE_REMOTE_DISK_FULL;

  સ્થિતિ LIBSSH2_FX_FILE_ALREADY_EXISTS:
    પાછા CURLE_REMOTE_FILE_EXISTS;

  સ્થિતિ LIBSSH2_FX_DIR_NOT_EMPTY:
    પાછા CURLE_QUOTE_ERROR;

  મૂળભૂત:
    તોડો;
  }

  પાછા CURLE_SSH;
}

સ્થિર CURLcode libssh2_session_error_to_CURLE(પૂર્ણાંક err)
{
  પસંદગી(err) {
  /* Ordered by order of appearance in libssh2.h */
  સ્થિતિ LIBSSH2_ERROR_NONE:
    પાછા CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  સ્થિતિ LIBSSH2_ERROR_SCP_PROTOCOL:
    પાછા CURLE_REMOTE_FILE_NOT_FOUND;

  સ્થિતિ LIBSSH2_ERROR_SOCKET_NONE:
    પાછા CURLE_COULDNT_CONNECT;

  સ્થિતિ LIBSSH2_ERROR_ALLOC:
    પાછા CURLE_OUT_OF_MEMORY;

  સ્થિતિ LIBSSH2_ERROR_SOCKET_SEND:
    પાછા CURLE_SEND_ERROR;

  સ્થિતિ LIBSSH2_ERROR_HOSTKEY_INIT:
  સ્થિતિ LIBSSH2_ERROR_HOSTKEY_SIGN:
  સ્થિતિ LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  સ્થિતિ LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    પાછા CURLE_PEER_FAILED_VERIFICATION;

  સ્થિતિ LIBSSH2_ERROR_PASSWORD_EXPIRED:
    પાછા CURLE_LOGIN_DENIED;

  સ્થિતિ LIBSSH2_ERROR_SOCKET_TIMEOUT:
  સ્થિતિ LIBSSH2_ERROR_TIMEOUT:
    પાછા CURLE_OPERATION_TIMEDOUT;

  સ્થિતિ LIBSSH2_ERROR_EAGAIN:
    પાછા CURLE_AGAIN;
  }

  પાછા CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

સ્થિર LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (શૂન્ય)abstract;
  પાછા Curl_cmalloc(ગણતરી);
}

સ્થિર LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (શૂન્ય)abstract;
  પાછા Curl_crealloc(ptr, ગણતરી);
}

સ્થિર LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (શૂન્ય)abstract;
  જો(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

સ્થિર પૂર્ણાંક sshkeycallback(CURL *easy,
                          અચલ સંરચના curl_khkey *knownkey, /* known */
                          અચલ સંરચના curl_khkey *foundkey, /* found */
                          ગણના curl_khmatch match,
                          શૂન્ય *clientp)
{
  (શૂન્ય)easy;
  (શૂન્ય)knownkey;
  (શૂન્ય)foundkey;
  (શૂન્ય)clientp;

  /* we only allow perfect matches, and we reject everything else */
  પાછા (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

સ્થિર ગણના curl_khtype convert_ssh2_keytype(પૂર્ણાંક sshkeytype)
{
  ગણના curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  પસંદગી(sshkeytype) {
  સ્થિતિ LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    તોડો;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  સ્થિતિ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    તોડો;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  સ્થિતિ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    તોડો;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  સ્થિતિ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    તોડો;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  સ્થિતિ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    તોડો;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  સ્થિતિ LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    તોડો;
#endif
  }
  પાછા keytype;
}

સ્થિર CURLcode ssh_knownhost(સંરચના Curl_easy *data,
                              સંરચના ssh_conn *sshc)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના libssh2_knownhost *host = ખાલી;
  અચલ અક્ષર *remotekey = ખાલી;
  પૂર્ણાંક keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  પૂર્ણાંક keybit = 0;
  પૂર્ણાંક sshkeytype = 0;
  size_t keylen = 0;
  પૂર્ણાંક rc = 0;
  CURLcode result = CURLE_OK;

  જો(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
    પાછા CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  જો(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    સંરચના Curl_mapi_guard guard;
    ગણના curl_khmatch keymatch;
    curl_sshkeycallback func =
      data->set.ssh_keyfunc ? data->set.ssh_keyfunc : sshkeycallback;
    સંરચના curl_khkey knownkey;
    સંરચના curl_khkey *knownkeyp = ખાલી;
    સંરચના curl_khkey foundkey;

    પસંદગી(sshkeytype) {
    સ્થિતિ LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_ચાવી_SSHRSA;
      તોડો;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    સ્થિતિ LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_ચાવી_SSHDSS;
      તોડો;
#endif
    સ્થિતિ LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_ચાવી_ECDSA_256;
      તોડો;
    સ્થિતિ LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_ચાવી_ECDSA_384;
      તોડો;
    સ્થિતિ LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_ચાવી_ECDSA_521;
      તોડો;
    સ્થિતિ LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_ચાવી_ED25519;
      તોડો;
    મૂળભૂત:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      તોડો;
    }
    જો(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    નહીંતર {
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
            host->ચાવી : "<none>");

      /* setup 'knownkey' */
      જો(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.ચાવી = host->ચાવી;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.ચાવી = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (ગણના curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  નહીંતર {
    /* no remotekey means failure! */
    infof(data, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  પસંદગી(rc) {
  મૂળભૂત: /* unknown return codes is the same as reject */
  સ્થિતિ CURLKHSTAT_REJECT:
    infof(data, "SSH: knownhost check failed");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  સ્થિતિ CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    result = CURLE_PEER_FAILED_VERIFICATION;
    તોડો;
  સ્થિતિ CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    જો(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  સ્થિતિ CURLKHSTAT_FINE:
  સ્થિતિ CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    જો(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      પૂર્ણાંક addrc;
      અચલ અક્ષર *hostbuf;
      અક્ષર *hostport = ખાલી;
      જો(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        જો(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
      }
      નહીંતર
        hostbuf = conn->origin->hostname;
      જો(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, ખાલી,
                                       remotekey, keylen, ખાલી, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, ખાલી);
        જો(addrc)
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        નહીંતર જો(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          પૂર્ણાંક wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          જો(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    નહીંતર
      infof(data, "SSH: knownhost entry matches host key");
    તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode ssh_check_fingerprint(સંરચના Curl_easy *data,
                                      સંરચના ssh_conn *sshc)
{
  અચલ અક્ષર *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ચાવી_MD5);
  અચલ અક્ષર *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ચાવી_SHA256);

  જો(pubkey_sha256) {
    અચલ અક્ષર *fingerprint = ખાલી;
    અક્ષર *fingerprint_b64 = ખાલી;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    જો(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      પાછા CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    જો(curlx_base64_encode((અચલ uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      પાછા CURLE_PEER_FAILED_VERIFICATION;
    }

    જો(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      પાછા CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    જ્યારે((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    જ્યારે((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    જો((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      પાછા CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  જો(pubkey_md5) {
    અક્ષર md5buffer[33];
    અચલ અક્ષર *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    જો(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      પૂર્ણાંક i;
      માટે(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned અક્ષર)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    જો(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      જો(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      નહીંતર {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      પાછા CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  જો(!pubkey_md5 && !pubkey_sha256) {
    જો(data->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      પૂર્ણાંક sshkeytype = 0;
      પૂર્ણાંક rc = 0;
      /* we handle the process to the callback */
      અચલ અક્ષર *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      જો(remotekey) {
        સંરચના Curl_mapi_guard guard;
        ગણના curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (પૂર્ણાંક)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        જો(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          પાછા CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      નહીંતર {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        પાછા CURLE_PEER_FAILED_VERIFICATION;
      }
      પાછા CURLE_OK;
    }
    નહીંતર {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      પાછા ssh_knownhost(data, sshc);
    }
  }
  નહીંતર {
    /* as we already matched, we skip the check for known hosts */
    પાછા CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
સ્થિર CURLcode ssh_force_knownhost_ચાવી_type(સંરચના Curl_easy *data,
                                             સંરચના ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  સ્થિર અચલ અક્ષર hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  સ્થિર અચલ અક્ષર hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  સ્થિર અચલ અક્ષર hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  સ્થિર અચલ અક્ષર hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  સ્થિર અચલ અક્ષર hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_ચાવી_SSHDSS
  સ્થિર અચલ અક્ષર hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  જો(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ચાવી_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ચાવી_SHA256)) {
    સંરચના libssh2_knownhost *store = ખાલી;
    સંરચના connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_ચાવી_MASK >>
                      LIBSSH2_KNOWNHOST_ચાવી_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    જ્યારે(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      જો(store) {
        જો(store->name) {
          જો(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_size = 0;
            અચલ અક્ષર *p;
            અચલ અક્ષર *kh_name_end = strstr(store->name, "]:");
            જો(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              ચાલુ;
            }
            p = kh_name_end + 2; /* start of port number */
            જો(!curlx_str_સંખ્યા(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = strlen(store->name) - 1 - strlen(kh_name_end);
              જો((kh_name_size == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                તોડો;
              }
            }
          }
          નહીંતર જો(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            તોડો;
          }
        }
        નહીંતર જો(store->ચાવી) {
          unsigned પૂર્ણાંક type = (store->typemask & LIBSSH2_KNOWNHOST_ચાવી_MASK)
                              >> LIBSSH2_KNOWNHOST_ચાવી_SHIFT;
          જો(!absent_type[type]) {
            પૂર્ણાંક keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->ચાવી, strlen(store->ચાવી),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_ચાવી_MASK),
              ખાલી);
            જો(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              તોડો;
            }
            નહીંતર જો(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    જો(found) {
      પૂર્ણાંક rc;
      અચલ અક્ષર *hostkey_method = ખાલી;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      પસંદગી(store->typemask & LIBSSH2_KNOWNHOST_ચાવી_MASK) {
      સ્થિતિ LIBSSH2_KNOWNHOST_ચાવી_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        તોડો;
      સ્થિતિ LIBSSH2_KNOWNHOST_ચાવી_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        તોડો;
      સ્થિતિ LIBSSH2_KNOWNHOST_ચાવી_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        તોડો;
      સ્થિતિ LIBSSH2_KNOWNHOST_ચાવી_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        તોડો;
      સ્થિતિ LIBSSH2_KNOWNHOST_ચાવી_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        તોડો;
#ifdef LIBSSH2_KNOWNHOST_ચાવી_SSHDSS
      સ્થિતિ LIBSSH2_KNOWNHOST_ચાવી_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        તોડો;
#endif
      સ્થિતિ LIBSSH2_KNOWNHOST_ચાવી_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        પાછા CURLE_SSH;
      મૂળભૂત:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_ચાવી_MASK));
        પાછા CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      જો(rc) {
        અક્ષર *err_સંદેશ = ખાલી;
        પૂર્ણાંક errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_સંદેશ);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    નહીંતર {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  પાછા result;
}

સ્થિર CURLcode quote_error(સંરચના Curl_easy *data,
                            સંરચના ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  પાછા CURLE_QUOTE_ERROR;
}

સ્થિર CURLcode sftp_quote(સંરચના Curl_easy *data,
                           સંરચના ssh_conn *sshc,
                           સંરચના SSHPROTO *sshp)
{
  અચલ અક્ષર *cp;
  CURLcode result = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  અચલ અક્ષર *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  જો(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  જો(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    અક્ષર *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    જો(!tmp)
      પાછા CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    જો(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    પાછા result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  જો(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    પાછા CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  જો(result) {
    જો(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    પાછા result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  જો(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    જો(result) {
      જો(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      પાછા result;
    }
    જો(*cp)
      પાછા quote_error(data, sshc);

    memset(&sshp->quote_attrs, 0, કદ(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    પાછા result;
  }
  જો(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    જો(result) {
      જો(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      પાછા result;
    }
    જો(*cp)
      પાછા quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    પાછા result;
  }
  નહીંતર જો(!strncmp(cmd, "mkdir ", 6)) {
    જો(*cp)
      પાછા quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    પાછા result;
  }
  નહીંતર જો(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    જો(result) {
      જો(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      પાછા result;
    }
    જો(*cp)
      પાછા quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    પાછા result;
  }
  નહીંતર જો(!strncmp(cmd, "rmdir ", 6)) {
    જો(*cp)
      પાછા quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    પાછા result;
  }
  નહીંતર જો(!strncmp(cmd, "rm ", 3)) {
    જો(*cp)
      પાછા quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    પાછા result;
  }
  નહીંતર જો(!strncmp(cmd, "statvfs ", 8)) {
    જો(*cp)
      પાછા quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    પાછા result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  પાછા CURLE_QUOTE_ERROR;
}

સ્થિર CURLcode sftp_upload_init(સંરચના Curl_easy *data,
                                 સંરચના ssh_conn *sshc,
                                 સંરચના SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  જો(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    જો(data->state.resume_from < 0) {
      પૂર્ણાંક rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      જો(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        પાછા CURLE_OK;
      }
      જો(rc) {
        data->state.resume_from = 0;
      }
      નહીંતર {
        curl_off_t size = attrs.filesize;
        જો(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          પાછા CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  જો(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  નહીંતર જો(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  નહીંતર {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  જો(!sshc->sftp_handle) {
    CURLcode result;
    unsigned long sftperr;
    પૂર્ણાંક rc = libssh2_session_last_errno(sshc->ssh_session);

    જો(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      પાછા CURLE_OK;
    }

    જો(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    નહીંતર
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    જો(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      પાછા sftp_libssh2_error_to_CURLE(sftperr);
    }
    જો((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      પાછા CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    જો(!result) {
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
    પાછા result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  જો(data->state.resume_from > 0 && !data->set.remote_append) {
    પૂર્ણાંક seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    જો(data->set.seek_func) {
      સંરચના Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    જો(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      જો(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        પાછા CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      કરો {
        સંરચના Curl_mapi_guard guard;
        અક્ષર scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)કદ(scratch)) ?
          કદ(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        જો((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          પાછા CURLE_FTP_COULDNT_USE_REST;
        }
      } જ્યારે(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    જો(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  જો(data->state.infilesize > 0) {
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
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય ssh_state_pkey_init(સંરચના Curl_easy *data, સંરચના ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  જો((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    જો(sshc->pub_ચાવી)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_ચાવી);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_ચાવી);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  નહીંતર
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
}

સ્થિર CURLcode sftp_quote_stat(સંરચના Curl_easy *data,
                                સંરચના ssh_conn *sshc,
                                સંરચના SSHPROTO *sshp,
                                bool *blockp)
{
  અચલ અક્ષર *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  જો(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  જો(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    પૂર્ણાંક rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    જો(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      પાછા CURLE_OK;
    }
    જો(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      જાઓ fail;
    }
  }

  /* Now set the new attributes... */
  જો(!strncmp(cmd, "chgrp", 5)) {
    અચલ અક્ષર *p = sshc->quote_path1;
    curl_off_t gid;
    જો(!curlx_str_સંખ્યા(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    નહીંતર જો(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      જાઓ fail;
    }
  }
  નહીંતર જો(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    અચલ અક્ષર *p = sshc->quote_path1;
    /* permissions are octal */
    જો(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      જાઓ fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  નહીંતર જો(!strncmp(cmd, "chown", 5)) {
    અચલ અક્ષર *p = sshc->quote_path1;
    curl_off_t uid;
    જો(!curlx_str_સંખ્યા(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    નહીંતર જો(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      જાઓ fail;
    }
  }
  નહીંતર જો(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    જો(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#જો SIZEOF_TIME_T > SIZEOF_LONG
    જો(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    જો(fail)
      જાઓ fail;
    જો(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    નહીંતર /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  પાછા CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  પાછા CURLE_QUOTE_ERROR;
}

સ્થિર CURLcode sftp_download_stat(સંરચના Curl_easy *data,
                                   સંરચના ssh_conn *sshc,
                                   સંરચના SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  પૂર્ણાંક rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  જો(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    પાછા CURLE_OK;
  }
  જો(rc ||
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
  નહીંતર {
    curl_off_t size = attrs.filesize;

    જો(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      પાછા CURLE_BAD_DOWNLOAD_RESUME;
    }
    જો(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      જો(result)
        પાછા result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  જો(data->state.resume_from) {
    જો(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      જો((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        પાછા CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    નહીંતર {
      જો((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        પાછા CURLE_BAD_DOWNLOAD_RESUME;
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
  જો(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    પાછા CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  પાછા CURLE_OK;
}

સ્થિર CURLcode sftp_readdir(સંરચના Curl_easy *data,
                             સંરચના ssh_conn *sshc,
                             સંરચના SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode result = CURLE_OK;
  પૂર્ણાંક rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  જો(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    પાછા result;
  }
  જો(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    જો(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      જો(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      જો(result)
        પાછા result;
    }
    નહીંતર {
      result = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      જો(!result) {
        જો((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        નહીંતર {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      પાછા result;
    }
  }
  નહીંતર જો(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  નહીંતર {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  પાછા result;
}

સ્થિર CURLcode ssh_state_init(સંરચના Curl_easy *data,
                               સંરચના ssh_conn *sshc)
{
  CURLcode result;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  result = ssh_force_knownhost_ચાવી_type(data, sshc);
  જો(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  નહીંતર
    myssh_to(data, sshc, SSH_S_STARTUP);
  પાછા result;
}

સ્થિર CURLcode ssh_state_startup(સંરચના Curl_easy *data,
                                  સંરચના ssh_conn *sshc)
{
  સંરચના connectdata *conn = data->conn;
  પૂર્ણાંક rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc) {
    અક્ષર *err_સંદેશ = ખાલી;
    (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_સંદેશ);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    પાછા CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_hostkey(સંરચના Curl_easy *data,
                                  સંરચના ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode result = ssh_check_fingerprint(data, sshc);
  જો(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  પાછા result;
}

સ્થિર CURLcode ssh_state_authlist(સંરચના Curl_easy *data,
                                   સંરચના ssh_conn *sshc)
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
  સંરચના connectdata *conn = data->conn;
  અચલ અક્ષર *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(strlen(user)));

  જો(!sshc->authlist) {
    પૂર્ણાંક rc;
    જો(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      પાછા CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    પાછા libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_auth_pkey(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  સંરચના connectdata *conn = data->conn;
  અચલ અક્ષર *user = Curl_creds_user(conn->creds);
  પૂર્ણાંક rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_ચાવી, sshc->priv_ચાવી, sshc->passphrase ? sshc->passphrase : "");
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  નહીંતર {
    અક્ષર *err_સંદેશ = ખાલી;
    અક્ષર unknown[] = "Reason unknown (-1)";
    જો(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_સંદેશ = unknown;
    }
    નહીંતર {
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_સંદેશ);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_auth_pass_init(સંરચના Curl_easy *data,
                                         સંરચના ssh_conn *sshc)
{
  જો((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_auth_pass(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc)
{
  સંરચના connectdata *conn = data->conn;
  અચલ અક્ષર *user = Curl_creds_user(conn->creds);
  અચલ અક્ષર *passwd = Curl_creds_passwd(conn->creds);
  પૂર્ણાંક rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 ખાલી);
  જો(rc == LIBSSH2_ERROR_EAGAIN) {
    પાછા CURLE_AGAIN;
  }
  જો(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_auth_host_init(સંરચના Curl_easy *data,
                                         સંરચના ssh_conn *sshc)
{
  જો((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_auth_agent_init(સંરચના Curl_easy *data,
                                          સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = 0;
  જો((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    જો(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      જો(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_ચાવી_INIT);
        પાછા CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;
    જો(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_ચાવી_INIT);
    }
    નહીંતર {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  નહીંતર
    myssh_to(data, sshc, SSH_AUTH_ચાવી_INIT);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_auth_agent_list(સંરચના Curl_easy *data,
                                          સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = libssh2_agent_list_identities(sshc->ssh_agent);

  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;
  જો(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_ચાવી_INIT);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = ખાલી;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_auth_agent(સંરચના Curl_easy *data,
                                     સંરચના ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  પૂર્ણાંક rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_prev_identity);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    જો(rc < 0) {
      જો(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        પાછા CURLE_OK;
      }
      પાછા CURLE_AGAIN;
    }
  }

  જો(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  નહીંતર જો(rc == 1)
    infof(data, "SSH: no agent identity would match");

  જો(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_AUTH_ચાવી_INIT);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_auth_ચાવી_init(સંરચના Curl_easy *data,
                                        સંરચના ssh_conn *sshc)
{
  જો((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_ચાવી);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_auth_ચાવી(સંરચના Curl_easy *data,
                                   સંરચના ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  સંરચના connectdata *conn = data->conn;
  અચલ અક્ષર *user = Curl_creds_user(conn->creds);
  પૂર્ણાંક rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    પાછા CURLE_OK;
  }
  પાછા CURLE_LOGIN_DENIED;
}

સ્થિર CURLcode ssh_state_auth_done(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc)
{
  સંરચના connectdata *conn = data->conn;
  જો(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    પાછા CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  જો(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    પાછા CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_init(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  જો(!sshc->sftp_session) {
    અક્ષર *err_સંદેશ = ખાલી;
    જો(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
    failf(data, "Failure initializing sftp session: %s", err_સંદેશ);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    પાછા CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_realpath(સંરચના Curl_easy *data,
                                        સંરચના ssh_conn *sshc,
                                        સંરચના SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  પૂર્ણાંક rc;

  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  જો(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    જો(!sshc->homedir)
      પાછા CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    જો(!data->state.most_recent_ftp_entrypath)
      પાછા CURLE_OUT_OF_MEMORY;
  }
  નહીંતર {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    જો(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    નહીંતર
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (પૂર્ણાંક)result);
    પાછા result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_quote_init(સંરચના Curl_easy *data,
                                          સંરચના ssh_conn *sshc,
                                          સંરચના SSHPROTO *sshp)
{
  CURLcode result;

  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  જો(result) {
    myssh_to(data, sshc, SSH_STOP);
    પાછા result;
  }

  જો(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_postquote_init(સંરચના Curl_easy *data,
                                              સંરચના ssh_conn *sshc)
{
  જો(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_STOP);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_quote(સંરચના Curl_easy *data,
                                     સંરચના ssh_conn *sshc,
                                     સંરચના SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  જો(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  પાછા result;
}

સ્થિર CURLcode ssh_state_sftp_next_quote(સંરચના Curl_easy *data,
                                          સંરચના ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  જો(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  નહીંતર {
    જો(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    નહીંતર {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_quote_stat(સંરચના Curl_easy *data,
                                          સંરચના ssh_conn *sshc,
                                          સંરચના SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  જો(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  પાછા result;
}

સ્થિર CURLcode ssh_state_sftp_quote_setstat(સંરચના Curl_easy *data,
                                             સંરચના ssh_conn *sshc,
                                             સંરચના SSHPROTO *sshp)
{
  પૂર્ણાંક rc;

  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    પાછા CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_quote_symlink(સંરચના Curl_easy *data,
                                             સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    પાછા CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_quote_mkdir(સંરચના Curl_easy *data,
                                           સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)data->set.new_directory_perms);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    પાછા CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_quote_rename(સંરચના Curl_easy *data,
                                            સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    પાછા CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_quote_rmdir(સંરચના Curl_easy *data,
                                           સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    પાછા CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_quote_unlink(સંરચના Curl_easy *data,
                                            સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    પાછા CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_quote_statvfs(સંરચના Curl_easy *data,
                                             સંરચના ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  પૂર્ણાંક rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    પાછા CURLE_QUOTE_ERROR;
  }
  નહીંતર જો(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#નહીંતર
#define CURL_LIBSSH2_VFS_SIZE_MASK "llu"
#endif
    CURLcode result;
    અક્ષર *tmp = curl_maprintf("statvfs:\n"
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
    જો(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      પાછા CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    જો(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      પાછા result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_create_dirs_mkdir(સંરચના Curl_easy *data,
                                                 સંરચના ssh_conn *sshc,
                                                 સંરચના SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  પૂર્ણાંક rc;

  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)data->set.new_directory_perms);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  જો(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    જો((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      પાછા sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_readdir_init(સંરચના Curl_easy *data,
                                            સંરચના ssh_conn *sshc,
                                            સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  જો(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    પાછા CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  જો(!sshc->sftp_handle) {
    unsigned long sftperr;
    જો(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    પાછા sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_readdir_link(સંરચના Curl_easy *data,
                                            સંરચના ssh_conn *sshc,
                                            સંરચના SSHPROTO *sshp)
{
  CURLcode result;
  પૂર્ણાંક rc;
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned પૂર્ણાંક)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  જો(rc < 0)
    પાછા CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  જો(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  નહીંતર
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  પાછા result;
}

સ્થિર CURLcode ssh_state_scp_download_init(સંરચના Curl_easy *data,
                                            સંરચના ssh_conn *sshc,
                                            સંરચના SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, કદ(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  જો(!sshc->ssh_channel) {
    પૂર્ણાંક ssh_err;
    અક્ષર *err_સંદેશ = ખાલી;

    જો(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
    failf(data, "%s", err_સંદેશ);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    પાછા libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_close(સંરચના Curl_easy *data,
                                     સંરચના ssh_conn *sshc,
                                     સંરચના SSHPROTO *sshp)
{
  પૂર્ણાંક rc = 0;
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  જો(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    જો(rc < 0) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_સંદેશ);
    }
    sshc->sftp_handle = ખાલી;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  જો(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  નહીંતર
    myssh_to(data, sshc, SSH_STOP);

  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_shutdown(સંરચના Curl_easy *data,
                                        સંરચના ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  પૂર્ણાંક rc = 0;
  જો(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    જો(rc < 0) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_સંદેશ);
    }
    sshc->sftp_handle = ખાલી;
  }
  જો(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    જો(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = ખાલી;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_download_init(સંરચના Curl_easy *data,
                                             સંરચના ssh_conn *sshc,
                                             સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  જો(!sshc->sftp_handle) {
    unsigned long sftperr;
    જો(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      પાછા CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    પાછા sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_scp_upload_init(સંરચના Curl_easy *data,
                                          સંરચના ssh_conn *sshc,
                                          સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (પૂર્ણાંક)data->set.new_file_perms,
                       (libssh2_int64_t)data->state.infilesize, 0, 0);
  જો(!sshc->ssh_channel) {
    પૂર્ણાંક ssh_err;
    અક્ષર *err_સંદેશ = ખાલી;
    CURLcode result;
    જો(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
    failf(data, "%s", err_સંદેશ);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    જો(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    પાછા result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_session_disconnect(સંરચના Curl_easy *data,
                                             સંરચના ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  પૂર્ણાંક rc = 0;
  જો(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    જો(rc < 0) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_સંદેશ);
    }
    sshc->ssh_channel = ખાલી;
  }

  જો(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;

    જો(rc < 0) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_સંદેશ);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode sshc_cleanup(સંરચના ssh_conn *sshc, સંરચના Curl_easy *data,
                             bool block)
{
  પૂર્ણાંક rc;

  જો(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = ખાલી;
  }

  જો(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    જો((rc < 0) && data) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_સંદેશ);
    }
    જો(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      પાછા CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = ખાલી;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = ખાલી;
    sshc->sshagent_prev_identity = ખાલી;
  }

  જો(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    જો((rc < 0) && data) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_સંદેશ);
    }
    જો(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      પાછા CURLE_AGAIN;

    sshc->sftp_handle = ખાલી;
  }

  જો(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    જો((rc < 0) && data) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_સંદેશ);
    }
    જો(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      પાછા CURLE_AGAIN;

    sshc->ssh_channel = ખાલી;
  }

  જો(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    જો((rc < 0) && data) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_સંદેશ);
    }
    જો(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      પાછા CURLE_AGAIN;

    sshc->sftp_session = ખાલી;
  }

  જો(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    જો((rc < 0) && data) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session, &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_સંદેશ);
    }
    જો(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      પાછા CURLE_AGAIN;

    sshc->ssh_session = ખાલી;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_ચાવી);
  curlx_safefree(sshc->priv_ચાવી);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_getinfo(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc)
{
  જો(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  નહીંતર
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_filetime(સંરચના Curl_easy *data,
                                        સંરચના ssh_conn *sshc,
                                        સંરચના SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  પૂર્ણાંક rc;

  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  જો(rc == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  જો(rc == 0)
    data->info.filetime = (time_t)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_trans_init(સંરચના Curl_easy *data,
                                          સંરચના ssh_conn *sshc,
                                          સંરચના SSHPROTO *sshp)
{
  જો(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  નહીંતર જો(sshp) {
    size_t plen = strlen(sshp->path);
    જો(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    નહીંતર
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  નહીંતર
    પાછા CURLE_FAILED_INIT;
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_upload_init(સંરચના Curl_easy *data,
                                           સંરચના ssh_conn *sshc,
                                           સંરચના SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  જો(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  પાછા result;
}

સ્થિર CURLcode ssh_state_sftp_create_dirs_init(સંરચના Curl_easy *data,
                                                સંરચના ssh_conn *sshc,
                                                સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  જો(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  નહીંતર
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_create_dirs(સંરચના Curl_easy *data,
                                           સંરચના ssh_conn *sshc,
                                           સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  જો(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    પાછા CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_readdir(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc,
                                       સંરચના SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  જો(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  પાછા result;
}

સ્થિર CURLcode ssh_state_sftp_readdir_bottom(સંરચના Curl_easy *data,
                                              સંરચના ssh_conn *sshc,
                                              સંરચના SSHPROTO *sshp)
{
  CURLcode result;
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  જો(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  જો(result) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(data, sshc, SSH_STOP);
  }
  નહીંતર {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  પાછા result;
}

સ્થિર CURLcode ssh_state_sftp_readdir_done(સંરચના Curl_easy *data,
                                            સંરચના ssh_conn *sshc)
{
  જો(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;

  sshc->sftp_handle = ખાલી;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_sftp_download_stat(સંરચના Curl_easy *data,
                                             સંરચના ssh_conn *sshc,
                                             સંરચના SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  જો(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  પાછા result;
}

સ્થિર CURLcode ssh_state_scp_trans_init(સંરચના Curl_easy *data,
                                         સંરચના ssh_conn *sshc,
                                         સંરચના SSHPROTO *sshp)
{
  CURLcode result;
  જો(!sshp)
    પાછા CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  જો(result) {
    myssh_to(data, sshc, SSH_STOP);
    પાછા result;
  }

  જો(data->state.upload) {
    જો(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      પાછા CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  નહીંતર
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_scp_done(સંરચના Curl_easy *data,
                                   સંરચના ssh_conn *sshc)
{
  જો(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  નહીંતર
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_scp_send_eof(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc)
{
  જો(sshc->ssh_channel) {
    પૂર્ણાંક rc = libssh2_channel_send_eof(sshc->ssh_channel);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;
    જો(rc) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session,
                                       &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_સંદેશ);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_scp_wait_eof(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc)
{
  જો(sshc->ssh_channel) {
    પૂર્ણાંક rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;
    જો(rc) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session,
                                       &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_સંદેશ);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_scp_wait_close(સંરચના Curl_easy *data,
                                         સંરચના ssh_conn *sshc)
{
  જો(sshc->ssh_channel) {
    પૂર્ણાંક rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;
    જો(rc) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session,
                                       &err_સંદેશ, ખાલી, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_સંદેશ);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_scp_channel_free(
  સંરચના Curl_easy *data,
  સંરચના ssh_conn *sshc)
{
  જો(sshc->ssh_channel) {
    પૂર્ણાંક rc = libssh2_channel_free(sshc->ssh_channel);
    જો(rc == LIBSSH2_ERROR_EAGAIN)
      પાછા CURLE_AGAIN;
    જો(rc < 0) {
      અક્ષર *err_સંદેશ = ખાલી;
      (શૂન્ય)libssh2_session_last_error(sshc->ssh_session,
                                       &err_સંદેશ, ખાલી, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_સંદેશ);
    }
    sshc->ssh_channel = ખાલી;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssh_state_session_free(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  સંરચના connectdata *conn = data->conn;
  જો(result)
    પાછા result;
  memset(sshc, 0, કદ(સંરચના ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  પાછા CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
સ્થિર CURLcode ssh_statemachine(સંરચના Curl_easy *data,
                                 સંરચના ssh_conn *sshc,
                                 સંરચના SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode result = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  કરો {
    પસંદગી(sshc->state) {
    સ્થિતિ SSH_INIT:
      result = ssh_state_init(data, sshc);
      જો(result)
        તોડો;
      FALLTHROUGH();

    સ્થિતિ SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      જો(result)
        તોડો;
      FALLTHROUGH();

    સ્થિતિ SSH_HOSTKEY:
      result = ssh_state_hostkey(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTHLIST:
      result = ssh_state_authlist(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_PKEY:
      result = ssh_state_auth_pkey(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_PASS_INIT:
      result = ssh_state_auth_pass_init(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_PASS:
      result = ssh_state_auth_pass(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_HOST_INIT:
      result = ssh_state_auth_host_init(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_HOST:
      myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
      તોડો;

    સ્થિતિ SSH_AUTH_AGENT_INIT:
      result = ssh_state_auth_agent_init(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_AGENT_LIST:
      result = ssh_state_auth_agent_list(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_AGENT:
      result = ssh_state_auth_agent(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_ચાવી_INIT:
      result = ssh_state_auth_ચાવી_init(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_ચાવી:
      result = ssh_state_auth_ચાવી(data, sshc);
      તોડો;

    સ્થિતિ SSH_AUTH_DONE:
      result = ssh_state_auth_done(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_INIT:
      result = ssh_state_sftp_init(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_REALPATH:
      result = ssh_state_sftp_realpath(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE_INIT:
      result = ssh_state_sftp_quote_init(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_POSTQUOTE_INIT:
      result = ssh_state_sftp_postquote_init(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE:
      result = ssh_state_sftp_quote(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_NEXT_QUOTE:
      result = ssh_state_sftp_next_quote(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE_STAT:
      result = ssh_state_sftp_quote_stat(data, sshc, sshp, block);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE_SETSTAT:
      result = ssh_state_sftp_quote_setstat(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE_SYMLINK:
      result = ssh_state_sftp_quote_symlink(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE_MKDIR:
      result = ssh_state_sftp_quote_mkdir(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE_RENAME:
      result = ssh_state_sftp_quote_rename(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE_RMDIR:
      result = ssh_state_sftp_quote_rmdir(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE_UNLINK:
      result = ssh_state_sftp_quote_unlink(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_QUOTE_STATVFS:
      result = ssh_state_sftp_quote_statvfs(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_GETINFO:
      result = ssh_state_sftp_getinfo(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_FILETIME:
      result = ssh_state_sftp_filetime(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_TRANS_INIT:
      result = ssh_state_sftp_trans_init(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_UPLOAD_INIT:
      result = ssh_state_sftp_upload_init(data, sshc, sshp, block);
      તોડો;

    સ્થિતિ SSH_SFTP_CREATE_DIRS_INIT:
      result = ssh_state_sftp_create_dirs_init(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_CREATE_DIRS:
      result = ssh_state_sftp_create_dirs(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_CREATE_DIRS_MKDIR:
      result = ssh_state_sftp_create_dirs_mkdir(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_READDIR_INIT:
      result = ssh_state_sftp_readdir_init(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_READDIR:
      result = ssh_state_sftp_readdir(data, sshc, sshp, block);
      તોડો;

    સ્થિતિ SSH_SFTP_READDIR_LINK:
      result = ssh_state_sftp_readdir_link(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_READDIR_BOTTOM:
      result = ssh_state_sftp_readdir_bottom(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_READDIR_DONE:
      result = ssh_state_sftp_readdir_done(data, sshc);
      તોડો;

    સ્થિતિ SSH_SFTP_DOWNLOAD_INIT:
      result = ssh_state_sftp_download_init(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_DOWNLOAD_STAT:
      result = ssh_state_sftp_download_stat(data, sshc, sshp, block);
      તોડો;

    સ્થિતિ SSH_SFTP_CLOSE:
      result = ssh_state_sftp_close(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SFTP_SHUTDOWN:
      result = ssh_state_sftp_shutdown(data, sshc);
      તોડો;

    સ્થિતિ SSH_SCP_TRANS_INIT:
      result = ssh_state_scp_trans_init(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SCP_UPLOAD_INIT:
      result = ssh_state_scp_upload_init(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SCP_DOWNLOAD_INIT:
      result = ssh_state_scp_download_init(data, sshc, sshp);
      તોડો;

    સ્થિતિ SSH_SCP_DONE:
      result = ssh_state_scp_done(data, sshc);
      તોડો;

    સ્થિતિ SSH_SCP_SEND_EOF:
      result = ssh_state_scp_send_eof(data, sshc);
      તોડો;

    સ્થિતિ SSH_SCP_WAIT_EOF:
      result = ssh_state_scp_wait_eof(data, sshc);
      તોડો;

    સ્થિતિ SSH_SCP_WAIT_CLOSE:
      result = ssh_state_scp_wait_close(data, sshc);
      તોડો;

    સ્થિતિ SSH_SCP_CHANNEL_FREE:
      result = ssh_state_scp_channel_free(data, sshc);
      તોડો;

    સ્થિતિ SSH_SESSION_DISCONNECT:
      result = ssh_state_session_disconnect(data, sshc);
      તોડો;

    સ્થિતિ SSH_SESSION_FREE:
      result = ssh_state_session_free(data, sshc);
      તોડો;

    સ્થિતિ SSH_QUIT:
    મૂળભૂત:
      /* internal error */
      myssh_to(data, sshc, SSH_STOP);
      તોડો;
    }

  } જ્યારે(!result && (sshc->state != SSH_STOP) && !*block);

  જો(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (પૂર્ણાંક)result, *block);

  પાછા result;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
સ્થિર શૂન્ય ssh_block2waitfor(સંરચના Curl_easy *data,
                              સંરચના ssh_conn *sshc,
                              bool block)
{
  પૂર્ણાંક dir = 0;
  (શૂન્ય)data;
  જો(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    જો(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  જો(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
સ્થિર CURLcode ssh_multi_statemach(સંરચના Curl_easy *data, bool *done)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  સંરચના SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  જો(!sshc || !sshp)
    પાછા CURLE_FAILED_INIT;

  કરો {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } જ્યારે(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  પાછા result;
}

સ્થિર CURLcode ssh_block_statemach(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc,
                                    સંરચના SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  સંરચના curltime start = *Curl_pgrs_now(data);

  જ્યારે((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t ડાબું_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    જો(result)
      તોડો;

    જો(!disconnect) {
      result = Curl_pgrsCheck(data);
      જો(result)
        તોડો;

      ડાબું_ms = Curl_timeleft_ms(data);
      જો(ડાબું_ms < 0) {
        failf(data, "Operation timed out");
        પાછા CURLE_OPERATION_TIMEDOUT;
      }
    }
    નહીંતર જો(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      તોડો;
    }

    જો(block) {
      પૂર્ણાંક dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      જો(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      જો(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (શૂન્ય)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!ડાબું_ms || ડાબું_ms > 1000) ? 1000 : ડાબું_ms);
    }
  }

  પાછા result;
}

સ્થિર શૂન્ય myssh_easy_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના SSHPROTO *sshp = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

સ્થિર શૂન્ય myssh_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના ssh_conn *sshc = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  sshc_cleanup(sshc, ખાલી, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
સ્થિર CURLcode ssh_setup_connection(સંરચના Curl_easy *data,
                                     સંરચના connectdata *conn)
{
  સંરચના ssh_conn *sshc;
  સંરચના SSHPROTO *sshp;
  (શૂન્ય)conn;

  sshc = curlx_calloc(1, કદ(*sshc));
  જો(!sshc)
    પાછા CURLE_OUT_OF_MEMORY;

  જો(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, કદ(*sshp));
  જો(!sshp)
    પાછા CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  જો(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  પાછા Curl_ssh_setup_pkey(data, sshc);
}

સ્થિર Curl_recv scp_recv, sftp_recv;
સ્થિર Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
સ્થિર ssize_t ssh_tls_recv(libssh2_socket_t sock, શૂન્ય *buffer,
                            size_t length, પૂર્ણાંક flags, શૂન્ય **abstract)
{
  સંરચના Curl_easy *data = (સંરચના Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nread;
  CURLcode result;
  સંરચના connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (શૂન્ય)flags;

  જો(!sshc)
    પાછા -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  જો(result == CURLE_AGAIN)
    પાછા -EAGAIN; /* magic return code for libssh2 */
  નહીંતર જો(result)
    પાછા -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (અચલ અક્ષર *)buffer, nread);
  પાછા (ssize_t)nread;
}

સ્થિર ssize_t ssh_tls_send(libssh2_socket_t sock, અચલ શૂન્ય *buffer,
                            size_t length, પૂર્ણાંક flags, શૂન્ય **abstract)
{
  સંરચના Curl_easy *data = (સંરચના Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nwrite;
  CURLcode result;
  સંરચના connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (શૂન્ય)flags;

  જો(!sshc)
    પાછા -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  જો(result == CURLE_AGAIN)
    પાછા -EAGAIN; /* magic return code for libssh2 */
  નહીંતર જો(result)
    પાછા -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (અચલ અક્ષર *)buffer, nwrite);
  પાછા (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
સ્થિર CURLcode ssh_connect(સંરચના Curl_easy *data, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result;

#જો LIBSSH2_VERSION_અંક >= 0x010b00
  {
    અચલ અક્ષર *crypto_str;
    પસંદગી(libssh2_crypto_engine()) {
    સ્થિતિ libssh2_gcrypt:
      crypto_str = "libgcrypt";
      તોડો;
    સ્થિતિ libssh2_mbedtls:
      crypto_str = "mbedTLS";
      તોડો;
    સ્થિતિ libssh2_openssl:
      crypto_str = "openssl compatible";
      તોડો;
    સ્થિતિ libssh2_os400qc3:
      crypto_str = "OS400QC3";
      તોડો;
    સ્થિતિ libssh2_wincng:
      crypto_str = "WinCNG";
      તોડો;
    મૂળભૂત:
      crypto_str = ખાલી;
      તોડો;
    }
    જો(crypto_str)
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

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

  જો(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    પાછા CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#જો LIBSSH2_VERSION_અંક >= 0x010B00
  જો(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  જો(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#જો LIBSSH2_VERSION_અંક >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic કાઢો
#endif
#નહીંતર
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    સંઘ receive {
      શૂન્ય *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, શૂન્ય *, size_t, પૂર્ણાંક, શૂન્ય **);
    };
    સંઘ transfer {
      શૂન્ય *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, અચલ શૂન્ય *, size_t, પૂર્ણાંક, શૂન્ય **);
    };
    સંઘ receive sshrecv;
    સંઘ transfer sshsend;

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
  જો(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  નહીંતર {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  જો(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  જો(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    પૂર્ણાંક rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    જો(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = ખાલી;
      પાછા CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    જો(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (પૂર્ણાંક)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  પાછા result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

સ્થિર CURLcode scp_perform(સંરચના Curl_easy *data,
                            bool *connected,
                            bool *dophase_done)
{
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  જો(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  પાછા result;
}

/* called from multi.c while DOing */
સ્થિર CURLcode scp_doing(સંરચના Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  જો(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  પાછા result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
સ્થિર CURLcode scp_disconnect(સંરચના Curl_easy *data,
                               સંરચના connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  સંરચના SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (શૂન્ય)dead_connection;

  જો(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  જો(sshc)
    પાછા sshc_cleanup(sshc, data, TRUE);
  પાછા result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
સ્થિર CURLcode ssh_done(સંરચના Curl_easy *data, CURLcode status)
{
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  સંરચના SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  જો(!sshc || !sshp)
    પાછા CURLE_FAILED_INIT;

  જો(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  નહીંતર
    result = status;

  જો(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    પાછા CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  પાછા result;
}

સ્થિર CURLcode scp_done(સંરચના Curl_easy *data, CURLcode status,
                         bool premature)
{
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (શૂન્ય)premature;

  જો(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  પાછા ssh_done(data, status);
}

સ્થિર CURLcode scp_send(સંરચના Curl_easy *data, int8_t sockindex,
                         અચલ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (શૂન્ય)sockindex; /* we only support SCP on the fixed known primary socket */
  (શૂન્ય)eos;
  *pnwritten = 0;

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (અચલ અક્ષર *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  જો(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  નહીંતર જો(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((પૂર્ણાંક)nwritten);
  નહીંતર
    *pnwritten = (size_t)nwritten;

  પાછા result;
}

સ્થિર CURLcode scp_recv(સંરચના Curl_easy *data, int8_t sockindex,
                         અક્ષર *mem, size_t len, size_t *pnread)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (શૂન્ય)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  જો(nread == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;
  નહીંતર જો(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((પૂર્ણાંક)nread);
  નહીંતર
    *pnread = (size_t)nread;

  પાછા result;
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

સ્થિર CURLcode sftp_perform(સંરચના Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  જો(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  પાછા result;
}

/* called from multi.c while DOing */
સ્થિર CURLcode sftp_doing(સંરચના Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  જો(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  પાછા result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
સ્થિર CURLcode sftp_disconnect(સંરચના Curl_easy *data,
                                સંરચના connectdata *conn, bool dead_connection)
{
  CURLcode result = CURLE_OK;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  સંરચના SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (શૂન્ય)dead_connection;

  જો(sshc) {
    જો(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (પૂર્ણાંક)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  પાછા result;
}

સ્થિર CURLcode sftp_done(સંરચના Curl_easy *data, CURLcode status,
                          bool premature)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  જો(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    જો(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  પાછા ssh_done(data, status);
}

/* return number of sent bytes */
સ્થિર CURLcode sftp_send(સંરચના Curl_easy *data, int8_t sockindex,
                          અચલ uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (શૂન્ય)sockindex;
  (શૂન્ય)eos;
  *pnwritten = 0;

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (અચલ અક્ષર *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  જો(nwrite == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;
  નહીંતર જો(nwrite < LIBSSH2_ERROR_NONE)
    પાછા libssh2_session_error_to_CURLE((પૂર્ણાંક)nwrite);
  *pnwritten = (size_t)nwrite;
  પાછા CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
સ્થિર CURLcode sftp_recv(સંરચના Curl_easy *data, int8_t sockindex,
                          અક્ષર *mem, size_t len, size_t *pnread)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (શૂન્ય)sockindex;
  *pnread = 0;

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  જો(nread == LIBSSH2_ERROR_EAGAIN)
    પાછા CURLE_AGAIN;
  નહીંતર જો(nread < 0)
    પાછા libssh2_session_error_to_CURLE((પૂર્ણાંક)nread);

  *pnread = (size_t)nread;
  પાછા CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
સ્થિર CURLcode ssh_do(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  જો(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  નહીંતર
    result = sftp_perform(data, &connected, done);

  પાછા result;
}

CURLcode Curl_ssh_init(શૂન્ય)
{
  જો(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    પાછા CURLE_FAILED_INIT;
  }
  પાછા CURLE_OK;
}

શૂન્ય Curl_ssh_cleanup(શૂન્ય)
{
  libssh2_exit();
}

શૂન્ય Curl_ssh_version(અક્ષર *buffer, size_t buflen)
{
  (શૂન્ય)curl_msnprintf(buffer, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
સ્થિર શૂન્ય ssh_attach(સંરચના Curl_easy *data, સંરચના connectdata *conn)
{
  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  જો(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    જો(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      શૂન્ય **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = data;
    }
  }
}

/*
 * SCP protocol handler.
 */
અચલ સંરચના Curl_protocol Curl_protocol_scp = {
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
અચલ સંરચના Curl_protocol Curl_protocol_sftp = {
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
