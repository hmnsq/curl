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
#include "curl_setup.h"

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "urldata.h"
#include "sendf.h"
#include "curl_trc.h"
#include "progress.h"
#include "transfer.h"
#include "vssh/ssh.h"
#include "url.h"
#include "cfilters.h"
#include "connect.h"
#include "parsedate.h" /* for the week day and month names */
#include "multiif.h"
#include "select.h"
#include "curlx/win32-fopen.h"
#include "vssh/vssh.h"
#include "curlx/strparse.h"
#include "curlx/base64.h" /* for curlx_base64_encode() */

static const var_na *sftp_libssh2_strerror(unsigned long err)
{
  switch(err) {
  pak_sa LIBSSH2_FX_NO_SUCH_FILE:
    ᮝᮜᮤ "No such file or directory";

  pak_sa LIBSSH2_FX_PERMISSION_DENIED:
    ᮝᮜᮤ "Permission denied";

  pak_sa LIBSSH2_FX_FAILURE:
    ᮝᮜᮤ "Operation failed";

  pak_sa LIBSSH2_FX_BAD_MESSAGE:
    ᮝᮜᮤ "Bad message from SFTP server";

  pak_sa LIBSSH2_FX_NO_CONNECTION:
    ᮝᮜᮤ "Not connected to SFTP server";

  pak_sa LIBSSH2_FX_CONNECTION_LOST:
    ᮝᮜᮤ "Connection to SFTP server lost";

  pak_sa LIBSSH2_FX_OP_UNSUPPORTED:
    ᮝᮜᮤ "Operation not supported by SFTP server";

  pak_sa LIBSSH2_FX_INVALID_HANDLE:
    ᮝᮜᮤ "Invalid handle";

  pak_sa LIBSSH2_FX_NO_SUCH_PATH:
    ᮝᮜᮤ "No such file or directory";

  pak_sa LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ᮝᮜᮤ "File already exists";

  pak_sa LIBSSH2_FX_WRITE_PROTECT:
    ᮝᮜᮤ "File is write protected";

  pak_sa LIBSSH2_FX_NO_MEDIA:
    ᮝᮜᮤ "No media";

  pak_sa LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    ᮝᮜᮤ "Disk full";

  pak_sa LIBSSH2_FX_QUOTA_EXCEEDED:
    ᮝᮜᮤ "User quota exceeded";

  pak_sa LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    ᮝᮜᮤ "Unknown principal";

  pak_sa LIBSSH2_FX_LOCK_CONFLICT:
    ᮝᮜᮤ "File lock conflict";

  pak_sa LIBSSH2_FX_DIR_NOT_EMPTY:
    ᮝᮜᮤ "Directory not empty";

  pak_sa LIBSSH2_FX_NOT_A_DIRECTORY:
    ᮝᮜᮤ "Not a directory";

  pak_sa LIBSSH2_FX_INVALID_FILENAME:
    ᮝᮜᮤ "Invalid filename";

  pak_sa LIBSSH2_FX_LINK_LOOP:
    ᮝᮜᮤ "Link points to itself";
  }
  ᮝᮜᮤ "Unknown error in libssh2";
}

static void kbd_callback(const var_na *name, puur_naa_nka name_len,
                         const var_na *instruction, puur_naa_nka instruction_len,
                         puur_naa_nka num_prompts,
                         const LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         void **abstract)
{
  sa_nracanaa Curl_easy *data = (sa_nracanaa Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", num_prompts);
#ᮜᮤᮚ
  (void)name;
  (void)name_len;
  (void)instruction;
  (void)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  ᮚᮦᮔ᮪(num_prompts == 1) {
    sa_nracanaa connectdata *conn = data->conn;
    const var_na *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (void)prompts;
} /* kbd_callback */

static CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  switch(err) {
  pak_sa LIBSSH2_FX_OK:
    ᮝᮜᮤ CURLE_OK;

  pak_sa LIBSSH2_FX_NO_SUCH_FILE:
  pak_sa LIBSSH2_FX_NO_SUCH_PATH:
    ᮝᮜᮤ CURLE_REMOTE_FILE_NOT_FOUND;

  pak_sa LIBSSH2_FX_PERMISSION_DENIED:
  pak_sa LIBSSH2_FX_WRITE_PROTECT:
  pak_sa LIBSSH2_FX_LOCK_CONFLICT:
    ᮝᮜᮤ CURLE_REMOTE_ACCESS_DENIED;

  pak_sa LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  pak_sa LIBSSH2_FX_QUOTA_EXCEEDED:
    ᮝᮜᮤ CURLE_REMOTE_DISK_FULL;

  pak_sa LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ᮝᮜᮤ CURLE_REMOTE_FILE_EXISTS;

  pak_sa LIBSSH2_FX_DIR_NOT_EMPTY:
    ᮝᮜᮤ CURLE_QUOTE_ERROR;

  default:
    bha_nga;
  }

  ᮝᮜᮤ CURLE_SSH;
}

static CURLcode libssh2_session_error_to_CURLE(puur_naa_nka err)
{
  switch(err) {
  /* Ordered by order of appearance in libssh2.h */
  pak_sa LIBSSH2_ERROR_NONE:
    ᮝᮜᮤ CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  pak_sa LIBSSH2_ERROR_SCP_PROTOCOL:
    ᮝᮜᮤ CURLE_REMOTE_FILE_NOT_FOUND;

  pak_sa LIBSSH2_ERROR_SOCKET_NONE:
    ᮝᮜᮤ CURLE_COULDNT_CONNECT;

  pak_sa LIBSSH2_ERROR_ALLOC:
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

  pak_sa LIBSSH2_ERROR_SOCKET_SEND:
    ᮝᮜᮤ CURLE_SEND_ERROR;

  pak_sa LIBSSH2_ERROR_HOSTKEY_INIT:
  pak_sa LIBSSH2_ERROR_HOSTKEY_SIGN:
  pak_sa LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  pak_sa LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    ᮝᮜᮤ CURLE_PEER_FAILED_VERIFICATION;

  pak_sa LIBSSH2_ERROR_PASSWORD_EXPIRED:
    ᮝᮜᮤ CURLE_LOGIN_DENIED;

  pak_sa LIBSSH2_ERROR_SOCKET_TIMEOUT:
  pak_sa LIBSSH2_ERROR_TIMEOUT:
    ᮝᮜᮤ CURLE_OPERATION_TIMEDOUT;

  pak_sa LIBSSH2_ERROR_EAGAIN:
    ᮝᮜᮤ CURLE_AGAIN;
  }

  ᮝᮜᮤ CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

static LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (void)abstract;
  ᮝᮜᮤ Curl_cmalloc(count);
}

static LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (void)abstract;
  ᮝᮜᮤ Curl_crealloc(ptr, count);
}

static LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (void)abstract;
  ᮚᮦᮔ᮪(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

static puur_naa_nka sshkeycallback(CURL *easy,
                          const sa_nracanaa curl_khkey *knownkey, /* known */
                          const sa_nracanaa curl_khkey *foundkey, /* found */
                          ga_nanaa curl_khmatch match,
                          void *clientp)
{
  (void)easy;
  (void)knownkey;
  (void)foundkey;
  (void)clientp;

  /* we only allow perfect matches, and we reject everything else */
  ᮝᮜᮤ (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

static ga_nanaa curl_khtype convert_ssh2_keytype(puur_naa_nka sshkeytype)
{
  ga_nanaa curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  switch(sshkeytype) {
  pak_sa LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    bha_nga;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  pak_sa LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    bha_nga;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  pak_sa LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    bha_nga;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  pak_sa LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    bha_nga;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  pak_sa LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    bha_nga;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  pak_sa LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    bha_nga;
#endif
  }
  ᮝᮜᮤ keytype;
}

static CURLcode ssh_knownhost(sa_nracanaa Curl_easy *data,
                              sa_nracanaa ssh_conn *sshc)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa libssh2_knownhost *host = NULL;
  const var_na *remotekey = NULL;
  puur_naa_nka keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  puur_naa_nka keybit = 0;
  puur_naa_nka sshkeytype = 0;
  size_t keylen = 0;
  puur_naa_nka rc = 0;
  CURLcode result = CURLE_OK;

  ᮚᮦᮔ᮪(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
    ᮝᮜᮤ CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  ᮚᮦᮔ᮪(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    sa_nracanaa Curl_mapi_guard guard;
    ga_nanaa curl_khmatch keymatch;
    curl_sshkeycallback func =
      data->set.ssh_keyfunc ? data->set.ssh_keyfunc : sshkeycallback;
    sa_nracanaa curl_khkey knownkey;
    sa_nracanaa curl_khkey *knownkeyp = NULL;
    sa_nracanaa curl_khkey foundkey;

    switch(sshkeytype) {
    pak_sa LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_KEY_SSHRSA;
      bha_nga;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    pak_sa LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_KEY_SSHDSS;
      bha_nga;
#endif
    pak_sa LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_KEY_ECDSA_256;
      bha_nga;
    pak_sa LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_KEY_ECDSA_384;
      bha_nga;
    pak_sa LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_KEY_ECDSA_521;
      bha_nga;
    pak_sa LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_KEY_ED25519;
      bha_nga;
    default:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      bha_nga;
    }
    ᮚᮦᮔ᮪(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    ᮜᮤᮚ {
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
            host->key : "<none>");

      /* setup 'knownkey' */
      ᮚᮦᮔ᮪(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.key = host->key;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.key = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (ga_nanaa curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  ᮜᮤᮚ {
    /* no remotekey means failure! */
    infof(data, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  switch(rc) {
  default: /* unknown return codes is the same as reject */
  pak_sa CURLKHSTAT_REJECT:
    infof(data, "SSH: knownhost check failed");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  pak_sa CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    result = CURLE_PEER_FAILED_VERIFICATION;
    bha_nga;
  pak_sa CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    ᮚᮦᮔ᮪(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  pak_sa CURLKHSTAT_FINE:
  pak_sa CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    ᮚᮦᮔ᮪(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      puur_naa_nka addrc;
      const var_na *hostbuf;
      var_na *hostport = NULL;
      ᮚᮦᮔ᮪(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        ᮚᮦᮔ᮪(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
      }
      ᮜᮤᮚ
        hostbuf = conn->origin->hostname;
      ᮚᮦᮔ᮪(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, NULL,
                                       remotekey, keylen, NULL, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, NULL);
        ᮚᮦᮔ᮪(addrc)
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        ᮜᮤᮚ ᮚᮦᮔ᮪(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          puur_naa_nka wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          ᮚᮦᮔ᮪(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    ᮜᮤᮚ
      infof(data, "SSH: knownhost entry matches host key");
    bha_nga;
  }
  ᮝᮜᮤ result;
}

static CURLcode ssh_check_fingerprint(sa_nracanaa Curl_easy *data,
                                      sa_nracanaa ssh_conn *sshc)
{
  const var_na *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_MD5);
  const var_na *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256);

  ᮚᮦᮔ᮪(pubkey_sha256) {
    const var_na *fingerprint = NULL;
    var_na *fingerprint_b64 = NULL;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    ᮚᮦᮔ᮪(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ᮝᮜᮤ CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    ᮚᮦᮔ᮪(curlx_base64_encode((const uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ᮝᮜᮤ CURLE_PEER_FAILED_VERIFICATION;
    }

    ᮚᮦᮔ᮪(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ᮝᮜᮤ CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    ᮞᮨᮓᮨᮜ((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    ᮞᮨᮓᮨᮜ((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    ᮚᮦᮔ᮪((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ᮝᮜᮤ CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  ᮚᮦᮔ᮪(pubkey_md5) {
    var_na md5buffer[33];
    const var_na *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    ᮚᮦᮔ᮪(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      puur_naa_nka i;
      for(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned var_na)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    ᮚᮦᮔ᮪(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      ᮚᮦᮔ᮪(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      ᮜᮤᮚ {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ᮝᮜᮤ CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  ᮚᮦᮔ᮪(!pubkey_md5 && !pubkey_sha256) {
    ᮚᮦᮔ᮪(data->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      puur_naa_nka sshkeytype = 0;
      puur_naa_nka rc = 0;
      /* we handle the process to the callback */
      const var_na *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      ᮚᮦᮔ᮪(remotekey) {
        sa_nracanaa Curl_mapi_guard guard;
        ga_nanaa curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (puur_naa_nka)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        ᮚᮦᮔ᮪(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          ᮝᮜᮤ CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      ᮜᮤᮚ {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        ᮝᮜᮤ CURLE_PEER_FAILED_VERIFICATION;
      }
      ᮝᮜᮤ CURLE_OK;
    }
    ᮜᮤᮚ {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      ᮝᮜᮤ ssh_knownhost(data, sshc);
    }
  }
  ᮜᮤᮚ {
    /* as we already matched, we skip the check for known hosts */
    ᮝᮜᮤ CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
static CURLcode ssh_force_knownhost_key_type(sa_nracanaa Curl_easy *data,
                                             sa_nracanaa ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  static const var_na hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  static const var_na hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  static const var_na hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  static const var_na hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  static const var_na hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_KEY_SSHDSS
  static const var_na hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  ᮚᮦᮔ᮪(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256)) {
    sa_nracanaa libssh2_knownhost *store = NULL;
    sa_nracanaa connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_KEY_MASK >>
                      LIBSSH2_KNOWNHOST_KEY_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    ᮞᮨᮓᮨᮜ(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      ᮚᮦᮔ᮪(store) {
        ᮚᮦᮔ᮪(store->name) {
          ᮚᮦᮔ᮪(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_size = 0;
            const var_na *p;
            const var_na *kh_name_end = strstr(store->name, "]:");
            ᮚᮦᮔ᮪(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              continue;
            }
            p = kh_name_end + 2; /* start of port number */
            ᮚᮦᮔ᮪(!curlx_str_number(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = strlen(store->name) - 1 - strlen(kh_name_end);
              ᮚᮦᮔ᮪((kh_name_size == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                bha_nga;
              }
            }
          }
          ᮜᮤᮚ ᮚᮦᮔ᮪(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            bha_nga;
          }
        }
        ᮜᮤᮚ ᮚᮦᮔ᮪(store->key) {
          unsigned puur_naa_nka type = (store->typemask & LIBSSH2_KNOWNHOST_KEY_MASK)
                              >> LIBSSH2_KNOWNHOST_KEY_SHIFT;
          ᮚᮦᮔ᮪(!absent_type[type]) {
            puur_naa_nka keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->key, strlen(store->key),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_KEY_MASK),
              NULL);
            ᮚᮦᮔ᮪(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              bha_nga;
            }
            ᮜᮤᮚ ᮚᮦᮔ᮪(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    ᮚᮦᮔ᮪(found) {
      puur_naa_nka rc;
      const var_na *hostkey_method = NULL;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      switch(store->typemask & LIBSSH2_KNOWNHOST_KEY_MASK) {
      pak_sa LIBSSH2_KNOWNHOST_KEY_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        bha_nga;
      pak_sa LIBSSH2_KNOWNHOST_KEY_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        bha_nga;
      pak_sa LIBSSH2_KNOWNHOST_KEY_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        bha_nga;
      pak_sa LIBSSH2_KNOWNHOST_KEY_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        bha_nga;
      pak_sa LIBSSH2_KNOWNHOST_KEY_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        bha_nga;
#ifdef LIBSSH2_KNOWNHOST_KEY_SSHDSS
      pak_sa LIBSSH2_KNOWNHOST_KEY_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        bha_nga;
#endif
      pak_sa LIBSSH2_KNOWNHOST_KEY_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        ᮝᮜᮤ CURLE_SSH;
      default:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_KEY_MASK));
        ᮝᮜᮤ CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      ᮚᮦᮔ᮪(rc) {
        var_na *err_msg = NULL;
        puur_naa_nka errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_msg, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_msg);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    ᮜᮤᮚ {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  ᮝᮜᮤ result;
}

static CURLcode quote_error(sa_nracanaa Curl_easy *data,
                            sa_nracanaa ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ᮝᮜᮤ CURLE_QUOTE_ERROR;
}

static CURLcode sftp_quote(sa_nracanaa Curl_easy *data,
                           sa_nracanaa ssh_conn *sshc,
                           sa_nracanaa SSHPROTO *sshp)
{
  const var_na *cp;
  CURLcode result = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  const var_na *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ᮚᮦᮔ᮪(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ᮚᮦᮔ᮪(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    var_na *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    ᮚᮦᮔ᮪(!tmp)
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    ᮚᮦᮔ᮪(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    ᮝᮜᮤ result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ᮚᮦᮔ᮪(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    ᮝᮜᮤ CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ᮚᮦᮔ᮪(result) {
    ᮚᮦᮔ᮪(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    ᮝᮜᮤ result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  ᮚᮦᮔ᮪(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ᮚᮦᮔ᮪(result) {
      ᮚᮦᮔ᮪(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      ᮝᮜᮤ result;
    }
    ᮚᮦᮔ᮪(*cp)
      ᮝᮜᮤ quote_error(data, sshc);

    memset(&sshp->quote_attrs, 0, parimaa_na(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    ᮝᮜᮤ result;
  }
  ᮚᮦᮔ᮪(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ᮚᮦᮔ᮪(result) {
      ᮚᮦᮔ᮪(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ᮝᮜᮤ result;
    }
    ᮚᮦᮔ᮪(*cp)
      ᮝᮜᮤ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ᮝᮜᮤ result;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strncmp(cmd, "mkdir ", 6)) {
    ᮚᮦᮔ᮪(*cp)
      ᮝᮜᮤ quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    ᮝᮜᮤ result;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ᮚᮦᮔ᮪(result) {
      ᮚᮦᮔ᮪(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ᮝᮜᮤ result;
    }
    ᮚᮦᮔ᮪(*cp)
      ᮝᮜᮤ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    ᮝᮜᮤ result;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strncmp(cmd, "rmdir ", 6)) {
    ᮚᮦᮔ᮪(*cp)
      ᮝᮜᮤ quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    ᮝᮜᮤ result;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strncmp(cmd, "rm ", 3)) {
    ᮚᮦᮔ᮪(*cp)
      ᮝᮜᮤ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    ᮝᮜᮤ result;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strncmp(cmd, "statvfs ", 8)) {
    ᮚᮦᮔ᮪(*cp)
      ᮝᮜᮤ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    ᮝᮜᮤ result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ᮝᮜᮤ CURLE_QUOTE_ERROR;
}

static CURLcode sftp_upload_init(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa ssh_conn *sshc,
                                 sa_nracanaa SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  ᮚᮦᮔ᮪(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    ᮚᮦᮔ᮪(data->state.resume_from < 0) {
      puur_naa_nka rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        ᮝᮜᮤ CURLE_OK;
      }
      ᮚᮦᮔ᮪(rc) {
        data->state.resume_from = 0;
      }
      ᮜᮤᮚ {
        curl_off_t size = attrs.filesize;
        ᮚᮦᮔ᮪(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          ᮝᮜᮤ CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  ᮚᮦᮔ᮪(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  ᮜᮤᮚ {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  ᮚᮦᮔ᮪(!sshc->sftp_handle) {
    CURLcode result;
    unsigned long sftperr;
    puur_naa_nka rc = libssh2_session_last_errno(sshc->ssh_session);

    ᮚᮦᮔ᮪(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      ᮝᮜᮤ CURLE_OK;
    }

    ᮚᮦᮔ᮪(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    ᮜᮤᮚ
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    ᮚᮦᮔ᮪(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      ᮝᮜᮤ sftp_libssh2_error_to_CURLE(sftperr);
    }
    ᮚᮦᮔ᮪((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ᮝᮜᮤ CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    ᮚᮦᮔ᮪(!result) {
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
    ᮝᮜᮤ result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  ᮚᮦᮔ᮪(data->state.resume_from > 0 && !data->set.remote_append) {
    puur_naa_nka seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    ᮚᮦᮔ᮪(data->set.seek_func) {
      sa_nracanaa Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    ᮚᮦᮔ᮪(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      ᮚᮦᮔ᮪(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        ᮝᮜᮤ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      do {
        sa_nracanaa Curl_mapi_guard guard;
        var_na scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)parimaa_na(scratch)) ?
          parimaa_na(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        ᮚᮦᮔ᮪((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          ᮝᮜᮤ CURLE_FTP_COULDNT_USE_REST;
        }
      } ᮞᮨᮓᮨᮜ(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    ᮚᮦᮔ᮪(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  ᮚᮦᮔ᮪(data->state.infilesize > 0) {
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
  ᮝᮜᮤ CURLE_OK;
}

static void ssh_state_pkey_init(sa_nracanaa Curl_easy *data, sa_nracanaa ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  ᮚᮦᮔ᮪((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    ᮚᮦᮔ᮪(sshc->pub_key)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_key);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_key);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  ᮜᮤᮚ
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
}

static CURLcode sftp_quote_stat(sa_nracanaa Curl_easy *data,
                                sa_nracanaa ssh_conn *sshc,
                                sa_nracanaa SSHPROTO *sshp,
                                bool *blockp)
{
  const var_na *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  ᮚᮦᮔ᮪(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ᮚᮦᮔ᮪(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    puur_naa_nka rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      ᮝᮜᮤ CURLE_OK;
    }
    ᮚᮦᮔ᮪(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      goto fail;
    }
  }

  /* Now set the new attributes... */
  ᮚᮦᮔ᮪(!strncmp(cmd, "chgrp", 5)) {
    const var_na *p = sshc->quote_path1;
    curl_off_t gid;
    ᮚᮦᮔ᮪(!curlx_str_number(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      goto fail;
    }
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    const var_na *p = sshc->quote_path1;
    /* permissions are octal */
    ᮚᮦᮔ᮪(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      goto fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strncmp(cmd, "chown", 5)) {
    const var_na *p = sshc->quote_path1;
    curl_off_t uid;
    ᮚᮦᮔ᮪(!curlx_str_number(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      goto fail;
    }
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    ᮚᮦᮔ᮪(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ᮚᮦᮔ᮪ SIZEOF_TIME_T > SIZEOF_LONG
    ᮚᮦᮔ᮪(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    ᮚᮦᮔ᮪(fail)
      goto fail;
    ᮚᮦᮔ᮪(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    ᮜᮤᮚ /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ᮝᮜᮤ CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ᮝᮜᮤ CURLE_QUOTE_ERROR;
}

static CURLcode sftp_download_stat(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ssh_conn *sshc,
                                   sa_nracanaa SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  puur_naa_nka rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ᮝᮜᮤ CURLE_OK;
  }
  ᮚᮦᮔ᮪(rc ||
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
  ᮜᮤᮚ {
    curl_off_t size = attrs.filesize;

    ᮚᮦᮔ᮪(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      ᮝᮜᮤ CURLE_BAD_DOWNLOAD_RESUME;
    }
    ᮚᮦᮔ᮪(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      ᮚᮦᮔ᮪(result)
        ᮝᮜᮤ result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  ᮚᮦᮔ᮪(data->state.resume_from) {
    ᮚᮦᮔ᮪(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ᮚᮦᮔ᮪((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        ᮝᮜᮤ CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    ᮜᮤᮚ {
      ᮚᮦᮔ᮪((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        ᮝᮜᮤ CURLE_BAD_DOWNLOAD_RESUME;
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
  ᮚᮦᮔ᮪(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    ᮝᮜᮤ CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  ᮝᮜᮤ CURLE_OK;
}

static CURLcode sftp_readdir(sa_nracanaa Curl_easy *data,
                             sa_nracanaa ssh_conn *sshc,
                             sa_nracanaa SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode result = CURLE_OK;
  puur_naa_nka rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ᮝᮜᮤ result;
  }
  ᮚᮦᮔ᮪(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    ᮚᮦᮔ᮪(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      ᮚᮦᮔ᮪(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      ᮚᮦᮔ᮪(result)
        ᮝᮜᮤ result;
    }
    ᮜᮤᮚ {
      result = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      ᮚᮦᮔ᮪(!result) {
        ᮚᮦᮔ᮪((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        ᮜᮤᮚ {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      ᮝᮜᮤ result;
    }
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  ᮜᮤᮚ {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_init(sa_nracanaa Curl_easy *data,
                               sa_nracanaa ssh_conn *sshc)
{
  CURLcode result;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  result = ssh_force_knownhost_key_type(data, sshc);
  ᮚᮦᮔ᮪(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  ᮜᮤᮚ
    myssh_to(data, sshc, SSH_S_STARTUP);
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_startup(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa ssh_conn *sshc)
{
  sa_nracanaa connectdata *conn = data->conn;
  puur_naa_nka rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc) {
    var_na *err_msg = NULL;
    (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_msg);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    ᮝᮜᮤ CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_hostkey(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode result = ssh_check_fingerprint(data, sshc);
  ᮚᮦᮔ᮪(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_authlist(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ssh_conn *sshc)
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
  sa_nracanaa connectdata *conn = data->conn;
  const var_na *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(strlen(user)));

  ᮚᮦᮔ᮪(!sshc->authlist) {
    puur_naa_nka rc;
    ᮚᮦᮔ᮪(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      ᮝᮜᮤ CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    ᮝᮜᮤ libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_auth_pkey(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  sa_nracanaa connectdata *conn = data->conn;
  const var_na *user = Curl_creds_user(conn->creds);
  puur_naa_nka rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_key, sshc->priv_key, sshc->passphrase ? sshc->passphrase : "");
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ᮜᮤᮚ {
    var_na *err_msg = NULL;
    var_na unknown[] = "Reason unknown (-1)";
    ᮚᮦᮔ᮪(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_msg = unknown;
    }
    ᮜᮤᮚ {
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_msg);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_auth_pass_init(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  ᮜᮤᮚ {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_auth_pass(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ssh_conn *sshc)
{
  sa_nracanaa connectdata *conn = data->conn;
  const var_na *user = Curl_creds_user(conn->creds);
  const var_na *passwd = Curl_creds_passwd(conn->creds);
  puur_naa_nka rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 NULL);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN) {
    ᮝᮜᮤ CURLE_AGAIN;
  }
  ᮚᮦᮔ᮪(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ᮜᮤᮚ {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_auth_host_init(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  ᮜᮤᮚ {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_auth_agent_init(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa ssh_conn *sshc)
{
  puur_naa_nka rc = 0;
  ᮚᮦᮔ᮪((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    ᮚᮦᮔ᮪(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      ᮚᮦᮔ᮪(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_KEY_INIT);
        ᮝᮜᮤ CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;
    ᮚᮦᮔ᮪(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_KEY_INIT);
    }
    ᮜᮤᮚ {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  ᮜᮤᮚ
    myssh_to(data, sshc, SSH_AUTH_KEY_INIT);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_auth_agent_list(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa ssh_conn *sshc)
{
  puur_naa_nka rc = libssh2_agent_list_identities(sshc->ssh_agent);

  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;
  ᮚᮦᮔ᮪(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_KEY_INIT);
  }
  ᮜᮤᮚ {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = NULL;
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_auth_agent(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  puur_naa_nka rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_prev_identity);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    ᮚᮦᮔ᮪(rc < 0) {
      ᮚᮦᮔ᮪(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        ᮝᮜᮤ CURLE_OK;
      }
      ᮝᮜᮤ CURLE_AGAIN;
    }
  }

  ᮚᮦᮔ᮪(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  ᮜᮤᮚ ᮚᮦᮔ᮪(rc == 1)
    infof(data, "SSH: no agent identity would match");

  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ᮜᮤᮚ {
    myssh_to(data, sshc, SSH_AUTH_KEY_INIT);
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_auth_key_init(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_KEY);
  }
  ᮜᮤᮚ {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_auth_key(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  sa_nracanaa connectdata *conn = data->conn;
  const var_na *user = Curl_creds_user(conn->creds);
  puur_naa_nka rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ᮝᮜᮤ CURLE_OK;
  }
  ᮝᮜᮤ CURLE_LOGIN_DENIED;
}

static CURLcode ssh_state_auth_done(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ssh_conn *sshc)
{
  sa_nracanaa connectdata *conn = data->conn;
  ᮚᮦᮔ᮪(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    ᮝᮜᮤ CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ᮚᮦᮔ᮪(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    ᮝᮜᮤ CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_init(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  ᮚᮦᮔ᮪(!sshc->sftp_session) {
    var_na *err_msg = NULL;
    ᮚᮦᮔ᮪(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
    failf(data, "Failure initializing sftp session: %s", err_msg);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    ᮝᮜᮤ CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_realpath(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa ssh_conn *sshc,
                                        sa_nracanaa SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  puur_naa_nka rc;

  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  ᮚᮦᮔ᮪(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    ᮚᮦᮔ᮪(!sshc->homedir)
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    ᮚᮦᮔ᮪(!data->state.most_recent_ftp_entrypath)
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  }
  ᮜᮤᮚ {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    ᮚᮦᮔ᮪(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    ᮜᮤᮚ
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (puur_naa_nka)result);
    ᮝᮜᮤ result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_quote_init(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa ssh_conn *sshc,
                                          sa_nracanaa SSHPROTO *sshp)
{
  CURLcode result;

  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  ᮚᮦᮔ᮪(result) {
    myssh_to(data, sshc, SSH_STOP);
    ᮝᮜᮤ result;
  }

  ᮚᮦᮔ᮪(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ᮜᮤᮚ {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_postquote_init(sa_nracanaa Curl_easy *data,
                                              sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ᮜᮤᮚ {
    myssh_to(data, sshc, SSH_STOP);
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_quote(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa ssh_conn *sshc,
                                     sa_nracanaa SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  ᮚᮦᮔ᮪(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_sftp_next_quote(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  ᮚᮦᮔ᮪(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ᮜᮤᮚ {
    ᮚᮦᮔ᮪(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    ᮜᮤᮚ {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_quote_stat(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa ssh_conn *sshc,
                                          sa_nracanaa SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  ᮚᮦᮔ᮪(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_sftp_quote_setstat(sa_nracanaa Curl_easy *data,
                                             sa_nracanaa ssh_conn *sshc,
                                             sa_nracanaa SSHPROTO *sshp)
{
  puur_naa_nka rc;

  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ᮝᮜᮤ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_quote_symlink(sa_nracanaa Curl_easy *data,
                                             sa_nracanaa ssh_conn *sshc)
{
  puur_naa_nka rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ᮝᮜᮤ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_quote_mkdir(sa_nracanaa Curl_easy *data,
                                           sa_nracanaa ssh_conn *sshc)
{
  puur_naa_nka rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)data->set.new_directory_perms);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ᮝᮜᮤ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_quote_rename(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa ssh_conn *sshc)
{
  puur_naa_nka rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ᮝᮜᮤ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_quote_rmdir(sa_nracanaa Curl_easy *data,
                                           sa_nracanaa ssh_conn *sshc)
{
  puur_naa_nka rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ᮝᮜᮤ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_quote_unlink(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa ssh_conn *sshc)
{
  puur_naa_nka rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ᮝᮜᮤ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_quote_statvfs(sa_nracanaa Curl_easy *data,
                                             sa_nracanaa ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  puur_naa_nka rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ᮝᮜᮤ CURLE_QUOTE_ERROR;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#ᮜᮤᮚ
#define CURL_LIBSSH2_VFS_SIZE_MASK "llu"
#endif
    CURLcode result;
    var_na *tmp = curl_maprintf("statvfs:\n"
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
    ᮚᮦᮔ᮪(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    ᮚᮦᮔ᮪(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ᮝᮜᮤ result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_create_dirs_mkdir(sa_nracanaa Curl_easy *data,
                                                 sa_nracanaa ssh_conn *sshc,
                                                 sa_nracanaa SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  puur_naa_nka rc;

  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)data->set.new_directory_perms);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ᮚᮦᮔ᮪(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    ᮚᮦᮔ᮪((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      ᮝᮜᮤ sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_readdir_init(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa ssh_conn *sshc,
                                            sa_nracanaa SSHPROTO *sshp)
{
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  ᮚᮦᮔ᮪(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    ᮝᮜᮤ CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  ᮚᮦᮔ᮪(!sshc->sftp_handle) {
    unsigned long sftperr;
    ᮚᮦᮔ᮪(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    ᮝᮜᮤ sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_readdir_link(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa ssh_conn *sshc,
                                            sa_nracanaa SSHPROTO *sshp)
{
  CURLcode result;
  puur_naa_nka rc;
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned puur_naa_nka)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  ᮚᮦᮔ᮪(rc < 0)
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  ᮚᮦᮔ᮪(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  ᮜᮤᮚ
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_scp_download_init(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa ssh_conn *sshc,
                                            sa_nracanaa SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, parimaa_na(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  ᮚᮦᮔ᮪(!sshc->ssh_channel) {
    puur_naa_nka ssh_err;
    var_na *err_msg = NULL;

    ᮚᮦᮔ᮪(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
    failf(data, "%s", err_msg);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    ᮝᮜᮤ libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_close(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa ssh_conn *sshc,
                                     sa_nracanaa SSHPROTO *sshp)
{
  puur_naa_nka rc = 0;
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  ᮚᮦᮔ᮪(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    ᮚᮦᮔ᮪(rc < 0) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_msg);
    }
    sshc->sftp_handle = NULL;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  ᮚᮦᮔ᮪(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  ᮜᮤᮚ
    myssh_to(data, sshc, SSH_STOP);

  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_shutdown(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  puur_naa_nka rc = 0;
  ᮚᮦᮔ᮪(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    ᮚᮦᮔ᮪(rc < 0) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_msg);
    }
    sshc->sftp_handle = NULL;
  }
  ᮚᮦᮔ᮪(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    ᮚᮦᮔ᮪(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = NULL;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_download_init(sa_nracanaa Curl_easy *data,
                                             sa_nracanaa ssh_conn *sshc,
                                             sa_nracanaa SSHPROTO *sshp)
{
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  ᮚᮦᮔ᮪(!sshc->sftp_handle) {
    unsigned long sftperr;
    ᮚᮦᮔ᮪(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      ᮝᮜᮤ CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    ᮝᮜᮤ sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_scp_upload_init(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa ssh_conn *sshc,
                                          sa_nracanaa SSHPROTO *sshp)
{
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (puur_naa_nka)data->set.new_file_perms,
                       (libssh2_int64_t)data->state.infilesize, 0, 0);
  ᮚᮦᮔ᮪(!sshc->ssh_channel) {
    puur_naa_nka ssh_err;
    var_na *err_msg = NULL;
    CURLcode result;
    ᮚᮦᮔ᮪(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
    failf(data, "%s", err_msg);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    ᮚᮦᮔ᮪(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    ᮝᮜᮤ result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_session_disconnect(sa_nracanaa Curl_easy *data,
                                             sa_nracanaa ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  puur_naa_nka rc = 0;
  ᮚᮦᮔ᮪(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    ᮚᮦᮔ᮪(rc < 0) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_msg);
    }
    sshc->ssh_channel = NULL;
  }

  ᮚᮦᮔ᮪(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;

    ᮚᮦᮔ᮪(rc < 0) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_msg);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode sshc_cleanup(sa_nracanaa ssh_conn *sshc, sa_nracanaa Curl_easy *data,
                             bool block)
{
  puur_naa_nka rc;

  ᮚᮦᮔ᮪(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = NULL;
  }

  ᮚᮦᮔ᮪(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    ᮚᮦᮔ᮪((rc < 0) && data) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_msg);
    }
    ᮚᮦᮔ᮪(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ᮝᮜᮤ CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = NULL;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = NULL;
    sshc->sshagent_prev_identity = NULL;
  }

  ᮚᮦᮔ᮪(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ᮚᮦᮔ᮪((rc < 0) && data) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_msg);
    }
    ᮚᮦᮔ᮪(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ᮝᮜᮤ CURLE_AGAIN;

    sshc->sftp_handle = NULL;
  }

  ᮚᮦᮔ᮪(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ᮚᮦᮔ᮪((rc < 0) && data) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_msg);
    }
    ᮚᮦᮔ᮪(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ᮝᮜᮤ CURLE_AGAIN;

    sshc->ssh_channel = NULL;
  }

  ᮚᮦᮔ᮪(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ᮚᮦᮔ᮪((rc < 0) && data) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_msg);
    }
    ᮚᮦᮔ᮪(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ᮝᮜᮤ CURLE_AGAIN;

    sshc->sftp_session = NULL;
  }

  ᮚᮦᮔ᮪(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    ᮚᮦᮔ᮪((rc < 0) && data) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session, &err_msg, NULL, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_msg);
    }
    ᮚᮦᮔ᮪(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ᮝᮜᮤ CURLE_AGAIN;

    sshc->ssh_session = NULL;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_key);
  curlx_safefree(sshc->priv_key);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_getinfo(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  ᮜᮤᮚ
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_filetime(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa ssh_conn *sshc,
                                        sa_nracanaa SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  puur_naa_nka rc;

  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  ᮚᮦᮔ᮪(rc == 0)
    data->info.filetime = (time_t)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_trans_init(sa_nracanaa Curl_easy *data,
                                          sa_nracanaa ssh_conn *sshc,
                                          sa_nracanaa SSHPROTO *sshp)
{
  ᮚᮦᮔ᮪(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ᮜᮤᮚ ᮚᮦᮔ᮪(sshp) {
    size_t plen = strlen(sshp->path);
    ᮚᮦᮔ᮪(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    ᮜᮤᮚ
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  ᮜᮤᮚ
    ᮝᮜᮤ CURLE_FAILED_INIT;
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_upload_init(sa_nracanaa Curl_easy *data,
                                           sa_nracanaa ssh_conn *sshc,
                                           sa_nracanaa SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  ᮚᮦᮔ᮪(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_sftp_create_dirs_init(sa_nracanaa Curl_easy *data,
                                                sa_nracanaa ssh_conn *sshc,
                                                sa_nracanaa SSHPROTO *sshp)
{
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  ᮚᮦᮔ᮪(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ᮜᮤᮚ
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_create_dirs(sa_nracanaa Curl_easy *data,
                                           sa_nracanaa ssh_conn *sshc,
                                           sa_nracanaa SSHPROTO *sshp)
{
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ᮚᮦᮔ᮪(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ᮝᮜᮤ CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_readdir(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa ssh_conn *sshc,
                                       sa_nracanaa SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  ᮚᮦᮔ᮪(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_sftp_readdir_bottom(sa_nracanaa Curl_easy *data,
                                              sa_nracanaa ssh_conn *sshc,
                                              sa_nracanaa SSHPROTO *sshp)
{
  CURLcode result;
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  ᮚᮦᮔ᮪(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  ᮚᮦᮔ᮪(result) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(data, sshc, SSH_STOP);
  }
  ᮜᮤᮚ {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_sftp_readdir_done(sa_nracanaa Curl_easy *data,
                                            sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;

  sshc->sftp_handle = NULL;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_sftp_download_stat(sa_nracanaa Curl_easy *data,
                                             sa_nracanaa ssh_conn *sshc,
                                             sa_nracanaa SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  ᮚᮦᮔ᮪(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ᮝᮜᮤ result;
}

static CURLcode ssh_state_scp_trans_init(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa ssh_conn *sshc,
                                         sa_nracanaa SSHPROTO *sshp)
{
  CURLcode result;
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  ᮚᮦᮔ᮪(result) {
    myssh_to(data, sshc, SSH_STOP);
    ᮝᮜᮤ result;
  }

  ᮚᮦᮔ᮪(data->state.upload) {
    ᮚᮦᮔ᮪(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      ᮝᮜᮤ CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ᮜᮤᮚ
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_scp_done(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  ᮜᮤᮚ
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_scp_send_eof(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪(sshc->ssh_channel) {
    puur_naa_nka rc = libssh2_channel_send_eof(sshc->ssh_channel);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;
    ᮚᮦᮔ᮪(rc) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session,
                                       &err_msg, NULL, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_msg);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_scp_wait_eof(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪(sshc->ssh_channel) {
    puur_naa_nka rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;
    ᮚᮦᮔ᮪(rc) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session,
                                       &err_msg, NULL, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_msg);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_scp_wait_close(sa_nracanaa Curl_easy *data,
                                         sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪(sshc->ssh_channel) {
    puur_naa_nka rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;
    ᮚᮦᮔ᮪(rc) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session,
                                       &err_msg, NULL, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_msg);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_scp_channel_free(
  sa_nracanaa Curl_easy *data,
  sa_nracanaa ssh_conn *sshc)
{
  ᮚᮦᮔ᮪(sshc->ssh_channel) {
    puur_naa_nka rc = libssh2_channel_free(sshc->ssh_channel);
    ᮚᮦᮔ᮪(rc == LIBSSH2_ERROR_EAGAIN)
      ᮝᮜᮤ CURLE_AGAIN;
    ᮚᮦᮔ᮪(rc < 0) {
      var_na *err_msg = NULL;
      (void)libssh2_session_last_error(sshc->ssh_session,
                                       &err_msg, NULL, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_msg);
    }
    sshc->ssh_channel = NULL;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  ᮝᮜᮤ CURLE_OK;
}

static CURLcode ssh_state_session_free(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  sa_nracanaa connectdata *conn = data->conn;
  ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ result;
  memset(sshc, 0, parimaa_na(sa_nracanaa ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  ᮝᮜᮤ CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
static CURLcode ssh_statemachine(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa ssh_conn *sshc,
                                 sa_nracanaa SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode result = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  do {
    switch(sshc->state) {
    pak_sa SSH_INIT:
      result = ssh_state_init(data, sshc);
      ᮚᮦᮔ᮪(result)
        bha_nga;
      FALLTHROUGH();

    pak_sa SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      ᮚᮦᮔ᮪(result)
        bha_nga;
      FALLTHROUGH();

    pak_sa SSH_HOSTKEY:
      result = ssh_state_hostkey(data, sshc);
      bha_nga;

    pak_sa SSH_AUTHLIST:
      result = ssh_state_authlist(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_PKEY:
      result = ssh_state_auth_pkey(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_PASS_INIT:
      result = ssh_state_auth_pass_init(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_PASS:
      result = ssh_state_auth_pass(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_HOST_INIT:
      result = ssh_state_auth_host_init(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_HOST:
      myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
      bha_nga;

    pak_sa SSH_AUTH_AGENT_INIT:
      result = ssh_state_auth_agent_init(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_AGENT_LIST:
      result = ssh_state_auth_agent_list(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_AGENT:
      result = ssh_state_auth_agent(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_KEY_INIT:
      result = ssh_state_auth_key_init(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_KEY:
      result = ssh_state_auth_key(data, sshc);
      bha_nga;

    pak_sa SSH_AUTH_DONE:
      result = ssh_state_auth_done(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_INIT:
      result = ssh_state_sftp_init(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_REALPATH:
      result = ssh_state_sftp_realpath(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE_INIT:
      result = ssh_state_sftp_quote_init(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_POSTQUOTE_INIT:
      result = ssh_state_sftp_postquote_init(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE:
      result = ssh_state_sftp_quote(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_NEXT_QUOTE:
      result = ssh_state_sftp_next_quote(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE_STAT:
      result = ssh_state_sftp_quote_stat(data, sshc, sshp, block);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE_SETSTAT:
      result = ssh_state_sftp_quote_setstat(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE_SYMLINK:
      result = ssh_state_sftp_quote_symlink(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE_MKDIR:
      result = ssh_state_sftp_quote_mkdir(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE_RENAME:
      result = ssh_state_sftp_quote_rename(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE_RMDIR:
      result = ssh_state_sftp_quote_rmdir(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE_UNLINK:
      result = ssh_state_sftp_quote_unlink(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_QUOTE_STATVFS:
      result = ssh_state_sftp_quote_statvfs(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_GETINFO:
      result = ssh_state_sftp_getinfo(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_FILETIME:
      result = ssh_state_sftp_filetime(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_TRANS_INIT:
      result = ssh_state_sftp_trans_init(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_UPLOAD_INIT:
      result = ssh_state_sftp_upload_init(data, sshc, sshp, block);
      bha_nga;

    pak_sa SSH_SFTP_CREATE_DIRS_INIT:
      result = ssh_state_sftp_create_dirs_init(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_CREATE_DIRS:
      result = ssh_state_sftp_create_dirs(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_CREATE_DIRS_MKDIR:
      result = ssh_state_sftp_create_dirs_mkdir(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_READDIR_INIT:
      result = ssh_state_sftp_readdir_init(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_READDIR:
      result = ssh_state_sftp_readdir(data, sshc, sshp, block);
      bha_nga;

    pak_sa SSH_SFTP_READDIR_LINK:
      result = ssh_state_sftp_readdir_link(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_READDIR_BOTTOM:
      result = ssh_state_sftp_readdir_bottom(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_READDIR_DONE:
      result = ssh_state_sftp_readdir_done(data, sshc);
      bha_nga;

    pak_sa SSH_SFTP_DOWNLOAD_INIT:
      result = ssh_state_sftp_download_init(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_DOWNLOAD_STAT:
      result = ssh_state_sftp_download_stat(data, sshc, sshp, block);
      bha_nga;

    pak_sa SSH_SFTP_CLOSE:
      result = ssh_state_sftp_close(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SFTP_SHUTDOWN:
      result = ssh_state_sftp_shutdown(data, sshc);
      bha_nga;

    pak_sa SSH_SCP_TRANS_INIT:
      result = ssh_state_scp_trans_init(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SCP_UPLOAD_INIT:
      result = ssh_state_scp_upload_init(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SCP_DOWNLOAD_INIT:
      result = ssh_state_scp_download_init(data, sshc, sshp);
      bha_nga;

    pak_sa SSH_SCP_DONE:
      result = ssh_state_scp_done(data, sshc);
      bha_nga;

    pak_sa SSH_SCP_SEND_EOF:
      result = ssh_state_scp_send_eof(data, sshc);
      bha_nga;

    pak_sa SSH_SCP_WAIT_EOF:
      result = ssh_state_scp_wait_eof(data, sshc);
      bha_nga;

    pak_sa SSH_SCP_WAIT_CLOSE:
      result = ssh_state_scp_wait_close(data, sshc);
      bha_nga;

    pak_sa SSH_SCP_CHANNEL_FREE:
      result = ssh_state_scp_channel_free(data, sshc);
      bha_nga;

    pak_sa SSH_SESSION_DISCONNECT:
      result = ssh_state_session_disconnect(data, sshc);
      bha_nga;

    pak_sa SSH_SESSION_FREE:
      result = ssh_state_session_free(data, sshc);
      bha_nga;

    pak_sa SSH_QUIT:
    default:
      /* internal error */
      myssh_to(data, sshc, SSH_STOP);
      bha_nga;
    }

  } ᮞᮨᮓᮨᮜ(!result && (sshc->state != SSH_STOP) && !*block);

  ᮚᮦᮔ᮪(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (puur_naa_nka)result, *block);

  ᮝᮜᮤ result;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
static void ssh_block2waitfor(sa_nracanaa Curl_easy *data,
                              sa_nracanaa ssh_conn *sshc,
                              bool block)
{
  puur_naa_nka dir = 0;
  (void)data;
  ᮚᮦᮔ᮪(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    ᮚᮦᮔ᮪(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  ᮚᮦᮔ᮪(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
static CURLcode ssh_multi_statemach(sa_nracanaa Curl_easy *data, bool *done)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  sa_nracanaa SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  ᮚᮦᮔ᮪(!sshc || !sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  do {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } ᮞᮨᮓᮨᮜ(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  ᮝᮜᮤ result;
}

static CURLcode ssh_block_statemach(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ssh_conn *sshc,
                                    sa_nracanaa SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa curltime start = *Curl_pgrs_now(data);

  ᮞᮨᮓᮨᮜ((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t left_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    ᮚᮦᮔ᮪(result)
      bha_nga;

    ᮚᮦᮔ᮪(!disconnect) {
      result = Curl_pgrsCheck(data);
      ᮚᮦᮔ᮪(result)
        bha_nga;

      left_ms = Curl_timeleft_ms(data);
      ᮚᮦᮔ᮪(left_ms < 0) {
        failf(data, "Operation timed out");
        ᮝᮜᮤ CURLE_OPERATION_TIMEDOUT;
      }
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      bha_nga;
    }

    ᮚᮦᮔ᮪(block) {
      puur_naa_nka dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      ᮚᮦᮔ᮪(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      ᮚᮦᮔ᮪(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (void)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!left_ms || left_ms > 1000) ? 1000 : left_ms);
    }
  }

  ᮝᮜᮤ result;
}

static void myssh_easy_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa SSHPROTO *sshp = entry;
  (void)key;
  (void)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

static void myssh_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa ssh_conn *sshc = entry;
  (void)key;
  (void)klen;
  sshc_cleanup(sshc, NULL, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
static CURLcode ssh_setup_connection(sa_nracanaa Curl_easy *data,
                                     sa_nracanaa connectdata *conn)
{
  sa_nracanaa ssh_conn *sshc;
  sa_nracanaa SSHPROTO *sshp;
  (void)conn;

  sshc = curlx_calloc(1, parimaa_na(*sshc));
  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

  ᮚᮦᮔ᮪(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, parimaa_na(*sshp));
  ᮚᮦᮔ᮪(!sshp)
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  ᮚᮦᮔ᮪(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

  ᮝᮜᮤ Curl_ssh_setup_pkey(data, sshc);
}

static Curl_recv scp_recv, sftp_recv;
static Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
static ssize_t ssh_tls_recv(libssh2_socket_t sock, void *buffer,
                            size_t length, puur_naa_nka flags, void **abstract)
{
  sa_nracanaa Curl_easy *data = (sa_nracanaa Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nread;
  CURLcode result;
  sa_nracanaa connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (void)flags;

  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  ᮚᮦᮔ᮪(result == CURLE_AGAIN)
    ᮝᮜᮤ -EAGAIN; /* magic return code for libssh2 */
  ᮜᮤᮚ ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (const var_na *)buffer, nread);
  ᮝᮜᮤ (ssize_t)nread;
}

static ssize_t ssh_tls_send(libssh2_socket_t sock, const void *buffer,
                            size_t length, puur_naa_nka flags, void **abstract)
{
  sa_nracanaa Curl_easy *data = (sa_nracanaa Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nwrite;
  CURLcode result;
  sa_nracanaa connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (void)flags;

  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  ᮚᮦᮔ᮪(result == CURLE_AGAIN)
    ᮝᮜᮤ -EAGAIN; /* magic return code for libssh2 */
  ᮜᮤᮚ ᮚᮦᮔ᮪(result)
    ᮝᮜᮤ -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (const var_na *)buffer, nwrite);
  ᮝᮜᮤ (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
static CURLcode ssh_connect(sa_nracanaa Curl_easy *data, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result;

#ᮚᮦᮔ᮪ LIBSSH2_VERSION_NUM >= 0x010b00
  {
    const var_na *crypto_str;
    switch(libssh2_crypto_engine()) {
    pak_sa libssh2_gcrypt:
      crypto_str = "libgcrypt";
      bha_nga;
    pak_sa libssh2_mbedtls:
      crypto_str = "mbedTLS";
      bha_nga;
    pak_sa libssh2_openssl:
      crypto_str = "openssl compatible";
      bha_nga;
    pak_sa libssh2_os400qc3:
      crypto_str = "OS400QC3";
      bha_nga;
    pak_sa libssh2_wincng:
      crypto_str = "WinCNG";
      bha_nga;
    default:
      crypto_str = NULL;
      bha_nga;
    }
    ᮚᮦᮔ᮪(crypto_str)
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_FAILED_INIT;

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

  ᮚᮦᮔ᮪(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    ᮝᮜᮤ CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#ᮚᮦᮔ᮪ LIBSSH2_VERSION_NUM >= 0x010B00
  ᮚᮦᮔ᮪(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  ᮚᮦᮔ᮪(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#ᮚᮦᮔ᮪ LIBSSH2_VERSION_NUM >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#ᮚᮦᮔ᮪ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#ᮚᮦᮔ᮪ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
#ᮜᮤᮚ
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    sa_ngha receive {
      void *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, void *, size_t, puur_naa_nka, void **);
    };
    sa_ngha transfer {
      void *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, const void *, size_t, puur_naa_nka, void **);
    };
    sa_ngha receive sshrecv;
    sa_ngha transfer sshsend;

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
  ᮚᮦᮔ᮪(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ᮜᮤᮚ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  ᮚᮦᮔ᮪(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  ᮚᮦᮔ᮪(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    puur_naa_nka rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    ᮚᮦᮔ᮪(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = NULL;
      ᮝᮜᮤ CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    ᮚᮦᮔ᮪(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (puur_naa_nka)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  ᮝᮜᮤ result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

static CURLcode scp_perform(sa_nracanaa Curl_easy *data,
                            bool *connected,
                            bool *dophase_done)
{
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ᮚᮦᮔ᮪(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ᮝᮜᮤ result;
}

/* called from multi.c while DOing */
static CURLcode scp_doing(sa_nracanaa Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  ᮚᮦᮔ᮪(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ᮝᮜᮤ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
static CURLcode scp_disconnect(sa_nracanaa Curl_easy *data,
                               sa_nracanaa connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  sa_nracanaa SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (void)dead_connection;

  ᮚᮦᮔ᮪(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  ᮚᮦᮔ᮪(sshc)
    ᮝᮜᮤ sshc_cleanup(sshc, data, TRUE);
  ᮝᮜᮤ result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
static CURLcode ssh_done(sa_nracanaa Curl_easy *data, CURLcode status)
{
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  sa_nracanaa SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  ᮚᮦᮔ᮪(!sshc || !sshp)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  ᮚᮦᮔ᮪(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  ᮜᮤᮚ
    result = status;

  ᮚᮦᮔ᮪(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ᮝᮜᮤ CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  ᮝᮜᮤ result;
}

static CURLcode scp_done(sa_nracanaa Curl_easy *data, CURLcode status,
                         bool premature)
{
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (void)premature;

  ᮚᮦᮔ᮪(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  ᮝᮜᮤ ssh_done(data, status);
}

static CURLcode scp_send(sa_nracanaa Curl_easy *data, int8_t sockindex,
                         const uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (void)sockindex; /* we only support SCP on the fixed known primary socket */
  (void)eos;
  *pnwritten = 0;

  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (const var_na *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  ᮚᮦᮔ᮪(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  ᮜᮤᮚ ᮚᮦᮔ᮪(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((puur_naa_nka)nwritten);
  ᮜᮤᮚ
    *pnwritten = (size_t)nwritten;

  ᮝᮜᮤ result;
}

static CURLcode scp_recv(sa_nracanaa Curl_easy *data, int8_t sockindex,
                         var_na *mem, size_t len, size_t *pnread)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (void)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  ᮚᮦᮔ᮪(nread == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;
  ᮜᮤᮚ ᮚᮦᮔ᮪(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((puur_naa_nka)nread);
  ᮜᮤᮚ
    *pnread = (size_t)nread;

  ᮝᮜᮤ result;
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

static CURLcode sftp_perform(sa_nracanaa Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ᮚᮦᮔ᮪(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ᮝᮜᮤ result;
}

/* called from multi.c while DOing */
static CURLcode sftp_doing(sa_nracanaa Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  ᮚᮦᮔ᮪(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ᮝᮜᮤ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
static CURLcode sftp_disconnect(sa_nracanaa Curl_easy *data,
                                sa_nracanaa connectdata *conn, bool dead_connection)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  sa_nracanaa SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (void)dead_connection;

  ᮚᮦᮔ᮪(sshc) {
    ᮚᮦᮔ᮪(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (puur_naa_nka)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  ᮝᮜᮤ result;
}

static CURLcode sftp_done(sa_nracanaa Curl_easy *data, CURLcode status,
                          bool premature)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  ᮚᮦᮔ᮪(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    ᮚᮦᮔ᮪(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  ᮝᮜᮤ ssh_done(data, status);
}

/* return number of sent bytes */
static CURLcode sftp_send(sa_nracanaa Curl_easy *data, int8_t sockindex,
                          const uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (void)sockindex;
  (void)eos;
  *pnwritten = 0;

  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (const var_na *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  ᮚᮦᮔ᮪(nwrite == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;
  ᮜᮤᮚ ᮚᮦᮔ᮪(nwrite < LIBSSH2_ERROR_NONE)
    ᮝᮜᮤ libssh2_session_error_to_CURLE((puur_naa_nka)nwrite);
  *pnwritten = (size_t)nwrite;
  ᮝᮜᮤ CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
static CURLcode sftp_recv(sa_nracanaa Curl_easy *data, int8_t sockindex,
                          var_na *mem, size_t len, size_t *pnread)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (void)sockindex;
  *pnread = 0;

  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  ᮚᮦᮔ᮪(nread == LIBSSH2_ERROR_EAGAIN)
    ᮝᮜᮤ CURLE_AGAIN;
  ᮜᮤᮚ ᮚᮦᮔ᮪(nread < 0)
    ᮝᮜᮤ libssh2_session_error_to_CURLE((puur_naa_nka)nread);

  *pnread = (size_t)nread;
  ᮝᮜᮤ CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
static CURLcode ssh_do(sa_nracanaa Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  ᮚᮦᮔ᮪(!sshc)
    ᮝᮜᮤ CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  ᮚᮦᮔ᮪(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  ᮜᮤᮚ
    result = sftp_perform(data, &connected, done);

  ᮝᮜᮤ result;
}

CURLcode Curl_ssh_init(void)
{
  ᮚᮦᮔ᮪(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    ᮝᮜᮤ CURLE_FAILED_INIT;
  }
  ᮝᮜᮤ CURLE_OK;
}

void Curl_ssh_cleanup(void)
{
  libssh2_exit();
}

void Curl_ssh_version(var_na *buffer, size_t buflen)
{
  (void)curl_msnprintf(buffer, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
static void ssh_attach(sa_nracanaa Curl_easy *data, sa_nracanaa connectdata *conn)
{
  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  ᮚᮦᮔ᮪(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    ᮚᮦᮔ᮪(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      void **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = data;
    }
  }
}

/*
 * SCP protocol handler.
 */
const sa_nracanaa Curl_protocol Curl_protocol_scp = {
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
const sa_nracanaa Curl_protocol Curl_protocol_sftp = {
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
