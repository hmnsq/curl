/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Red Hat, Inc.
 *
 * Authors: Nikos Mavrogiannopoulos, Tomas Mraz, Stanislav Zidek,
 *          Robert Kolcun, Andreas Schneider
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH

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
#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h"          /* for the week day and month names */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/vssh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h" /* for curlx_base64_encode() */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_UNISTD_H
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCNTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਜੇ_ਅੰਤ

/* A recent macro provided by libssh. Or make our own. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SSH_STRING_FREE_CHAR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSH_STRING_FREE_CHAR(x) \
  ਕਰੋ {                          \
    ਜੇ(x) {                     \
      ssh_string_free_char(x);  \
      x = ਨਿਰਾ;                 \
    }                           \
  } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SSH_S_IFMT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSH_S_IFMT   00170000
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SSH_S_IFLNK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSH_S_IFLNK  0120000
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode sftp_error_ਮੰਜ਼ਿਲ_CURLE(ਪੂਰਨ err)
{
  ਚੋਣ(err) {
  ਹਾਲਤ SSH_FX_OK:
    ਵਾਪਸ CURLE_OK;

  ਹਾਲਤ SSH_FX_NO_SUCH_FILE:
  ਹਾਲਤ SSH_FX_NO_SUCH_ਰਸਤਾ:
    ਵਾਪਸ CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ;

  ਹਾਲਤ SSH_FX_PERMISSION_DENIED:
  ਹਾਲਤ SSH_FX_WRITE_PROTECT:
    ਵਾਪਸ CURLE_REMOTE_ACCESS_DENIED;

  ਹਾਲਤ SSH_FX_FILE_ALREADY_EXISTS:
    ਵਾਪਸ CURLE_REMOTE_FILE_EXISTS;

  ਮੂਲ:
    ਰੋਕੋ;
  }

  ਵਾਪਸ CURLE_SSH;
}

/* Multiple options:
 * 1. CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256) is set
 *    with a SHA256 hash.
 * 2. CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_MD5) is set
 *    with an MD5 hash (90s style auth, not sure we should have it here)
 * 3. data->set.ssh_keyfunc callback is set. Then we do trust on first
 *    use. We even save on knownhosts if CURLKHSTAT_FINE_ADD_TO_FILE
 *    is returned by it.
 * 4. none of the above. We only accept if it is present on known hosts.
 *
 * Returns SSH_OK or SSH_ERROR.
 */
ਸਥਿਰ ਪੂਰਨ myssh_is_ਗਿਆਤ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc;
  ssh_ਕੁੰਜੀ pubkey;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *hash_sha256 = ਨਿਰਾ;
  size_t hlen_sha256;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *hash_md5 = ਨਿਰਾ;
  size_t hlen_md5;
  ਅੱਖਰ *ਮਿਲਿਆ_base64 = ਨਿਰਾ;
  ਅੱਖਰ *ਗਿਆਤ_base64 = ਨਿਰਾ;
  ਪੂਰਨ vstate;
  ਗਿਣਤੀ curl_khmatch keymatch;
  ਬਣਤਰ curl_khkey foundkey;
  ਬਣਤਰ curl_khkey *knownkeyp = ਨਿਰਾ;
  curl_sshkeycallback func = ਅੰਕੜੇ->ਸੈੱਟ.ssh_keyfunc;
  ਬਣਤਰ ssh_knownhosts_ਇੰਦਰਾਜ *knownhostsentry = ਨਿਰਾ;
  ਬਣਤਰ curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  ਜੇ(rc != SSH_OK)
    ਵਾਪਸ rc;

  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_SHA256)) {
    ਅਚਲ ਅੱਖਰ *pubkey_sha256 =
      CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_SHA256);
    ਅੱਖਰ *fingerprint_b64 = ਨਿਰਾ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    ਜੇ(rc != SSH_OK || hlen_sha256 != 32) {
      failf(ਅੰਕੜੇ, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      ਜਾਓ cleanup;
    }

    ਜੇ(curlx_base64_encode((ਅਚਲ uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      ਜਾਓ cleanup;
    }

    infof(ਅੰਕੜੇ, "SSH SHA256 fingerprint: %s", fingerprint_b64);

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
      rc = SSH_ERROR;
      ਜਾਓ cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    ਜਾਓ cleanup;
  }

  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_MD5)) {
    ਅਚਲ ਅੱਖਰ *pubkey_md5 =
      CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_HOST_ਜਨਤਕ_ਕੁੰਜੀ_MD5);
    ਅੱਖਰ md5buffer[33];
    ਪੂਰਨ i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    ਜੇ(rc != SSH_OK || hlen_md5 != 16) {
      failf(ਅੰਕੜੇ,
            "Denied establishing ssh session: MD5 fingerprint not available");
      ਜਾਓ cleanup;
    }

    ਲਈ(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(ਅੰਕੜੇ, "SSH MD5 fingerprint: %s", md5buffer);

    ਜੇ(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(ਅੰਕੜੇ,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      ਜਾਓ cleanup;
    }

    rc = SSH_OK;
    ਜਾਓ cleanup;
  }

  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_ਗਿਆਤ_hosts_ਇੰਦਰਾਜ(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    ਜੇ(knownhostsentry) {
      ਜੇ(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &ਗਿਆਤ_base64);
        ਜੇ(rc != SSH_OK) {
          ਜਾਓ cleanup;
        }
        knownkey.ਕੁੰਜੀ = ਗਿਆਤ_base64;
        knownkey.len = ਲੜੀ_ਲੰਬਾਈ(ਗਿਆਤ_base64);

        ਚੋਣ(ssh_ਕੁੰਜੀ_type(knownhostsentry->publickey)) {
        ਹਾਲਤ SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          ਰੋਕੋ;
        ਹਾਲਤ SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          ਰੋਕੋ;
        ਹਾਲਤ SSH_KEYTYPE_ECDSA:
        ਹਾਲਤ SSH_KEYTYPE_ECDSA_P256:
        ਹਾਲਤ SSH_KEYTYPE_ECDSA_P384:
        ਹਾਲਤ SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          ਰੋਕੋ;
        ਹਾਲਤ SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          ਰੋਕੋ;
        ਹਾਲਤ SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          ਰੋਕੋ;
        ਮੂਲ:
          rc = SSH_ERROR;
          ਜਾਓ cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    ਚੋਣ(vstate) {
    ਹਾਲਤ SSH_ਗਿਆਤ_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      ਰੋਕੋ;
    ਹਾਲਤ SSH_ਗਿਆਤ_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      ਰੋਕੋ;
    ਹਾਲਤ SSH_ਗਿਆਤ_HOSTS_NOT_ਮਿਲਿਆ:
    ਹਾਲਤ SSH_ਗਿਆਤ_HOSTS_UNKNOWN:
    ਹਾਲਤ SSH_ਗਿਆਤ_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      ਰੋਕੋ;
    ਮੂਲ:
      keymatch = CURLKHMATCH_MISMATCH;
      ਰੋਕੋ;
    }

    ਜੇ(func) { /* use callback to determine action */
      ਬਣਤਰ Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &ਮਿਲਿਆ_base64);
      ਜੇ(rc != SSH_OK)
        ਜਾਓ cleanup;

      foundkey.ਕੁੰਜੀ = ਮਿਲਿਆ_base64;
      foundkey.len = ਲੜੀ_ਲੰਬਾਈ(ਮਿਲਿਆ_base64);

      ਚੋਣ(ssh_ਕੁੰਜੀ_type(pubkey)) {
      ਹਾਲਤ SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        ਰੋਕੋ;
      ਹਾਲਤ SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        ਰੋਕੋ;
      ਹਾਲਤ SSH_KEYTYPE_ECDSA:
      ਹਾਲਤ SSH_KEYTYPE_ECDSA_P256:
      ਹਾਲਤ SSH_KEYTYPE_ECDSA_P384:
      ਹਾਲਤ SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        ਰੋਕੋ;
      ਹਾਲਤ SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        ਰੋਕੋ;
      ਹਾਲਤ SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        ਰੋਕੋ;
      ਮੂਲ:
        rc = SSH_ERROR;
        ਜਾਓ cleanup;
      }

      CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_ssh_keyfunc);
      rc = func(ਅੰਕੜੇ, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, ਅੰਕੜੇ->ਸੈੱਟ.ssh_keyfunc_userp);
      CURL_CBAPI_ਸਿਰਾ(&guard);

      ਚੋਣ(rc) {
      ਹਾਲਤ CURLKHSTAT_FINE_ਜੋੜੋ_ਮੰਜ਼ਿਲ_FILE:
        rc = ssh_session_update_ਗਿਆਤ_hosts(sshc->ssh_session);
        ਜੇ(rc != SSH_OK) {
          ਜਾਓ cleanup;
        }
        ਰੋਕੋ;
      ਹਾਲਤ CURLKHSTAT_FINE:
        ਰੋਕੋ;
      ਮੂਲ: /* REJECT/DEFER */
        rc = SSH_ERROR;
        ਜਾਓ cleanup;
      }
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        ਜਾਓ cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  ਜੇ(ਮਿਲਿਆ_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    ਮੁਕਤ(ਮਿਲਿਆ_base64); /* allocated by libssh, deallocate with system free */
  }
  ਜੇ(ਗਿਆਤ_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    ਮੁਕਤ(ਗਿਆਤ_base64); /* allocated by libssh, deallocate with system free */
  }
  ਜੇ(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  ਜੇ(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_ਕੁੰਜੀ_free(pubkey);
  ਜੇ(knownhostsentry) {
    ssh_knownhosts_ਇੰਦਰਾਜ_free(knownhostsentry);
  }
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਮੰਜ਼ਿਲ_ERROR(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ ssh_conn *sshc,
                          CURLcode ਨਤੀਜਾ)
{
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = ਨਤੀਜਾ;
  ਵਾਪਸ SSH_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ssh_conn *sshc)
{
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_ਮੰਜ਼ਿਲ_CURLE(sftp_get_error(sshc->sftp_session));
  ਵਾਪਸ SSH_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਮੰਜ਼ਿਲ_PASSWD_AUTH(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_PASSWORD) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PASS_INIT);
    ਵਾਪਸ SSH_OK;
  }
  ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_LOGIN_DENIED);
}

ਸਥਿਰ ਪੂਰਨ myssh_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ_AUTH(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_INTERACTIVE) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਕੁੰਜੀ_INIT);
    ਵਾਪਸ SSH_OK;
  }
  ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_PASSWD_AUTH(ਅੰਕੜੇ, sshc);
}

ਸਥਿਰ ਪੂਰਨ myssh_ਮੰਜ਼ਿਲ_GSSAPI_AUTH(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_GSSAPI_MIC) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_GSSAPI);
    ਵਾਪਸ SSH_OK;
  }
  ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ_AUTH(ਅੰਕੜੇ, sshc);
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_READDIR_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ssh_conn *sshc,
                                      ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);
  Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, -1);
  ਜੇ(ਅੰਕੜੇ->req.no_body) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
    ਵਾਪਸ SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->ਰਸਤਾ);
  ਜੇ(!sshc->sftp_dir) {
    failf(ਅੰਕੜੇ, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਅੰਕੜੇ, sshc);
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_READDIR(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ssh_conn *sshc,
                                 ਬਣਤਰ SSHPROTO *sshp)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  ਜੇ(sshc->readdir_attrs)
    sftp_ਗੁਣ_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  ਜੇ(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = ਲੜੀ_ਲੰਬਾਈ(sshc->readdir_filename);

    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.list_only) {
      ਅੱਖਰ *tmp_ਸਤਰ;

      tmp_ਸਤਰ = curl_maprintf("%s\n", sshc->readdir_filename);
      ਜੇ(!tmp_ਸਤਰ) {
        myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਵਾਪਸ SSH_ERROR;
      }
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                                 tmp_ਸਤਰ, sshc->readdir_len + 1);
      curlx_free(tmp_ਸਤਰ);

      ਜੇ(ਨਤੀਜਾ) {
        myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
        sshc->actualcode = ਨਤੀਜਾ;
        ਵਾਪਸ SSH_NO_ERROR;
      }
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(curlx_dyn_ਜੋੜੋ(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
        ਵਾਪਸ SSH_ERROR;
      }

      ਜੇ((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_link_ਰਸਤਾ = curl_maprintf("%s%s", sshp->ਰਸਤਾ,
                                               sshc->readdir_filename);

        ਜੇ(!sshc->readdir_link_ਰਸਤਾ) {
          myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
          ਵਾਪਸ SSH_ERROR;
        }

        myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_LINK);
        ਵਾਪਸ SSH_NO_ERROR;
      }
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_BOTTOM);
      ਵਾਪਸ SSH_NO_ERROR;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_ਪੂਰਾ);
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਅੰਕੜੇ, sshc);
  }
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_READDIR_LINK(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->readdir_link_attrs)
    sftp_ਗੁਣ_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_link_ਰਸਤਾ);
  ਜੇ(!sshc->readdir_link_attrs) {
    failf(ਅੰਕੜੇ, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਅੰਕੜੇ, sshc);
  }

  ਜੇ(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_link_ਰਸਤਾ);
    ਜੇ(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    ਨਹੀਂਤਾਂ
      sshc->readdir_len = ਲੜੀ_ਲੰਬਾਈ(sshc->readdir_tmp);
    sshc->readdir_longentry = ਨਿਰਾ;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  ਨਹੀਂਤਾਂ {
    sshc->readdir_len = ਲੜੀ_ਲੰਬਾਈ(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_link_ਰਸਤਾ);

  ਜੇ(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਵਾਪਸ SSH_ERROR;
  }

  sftp_ਗੁਣ_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = ਨਿਰਾ;
  sshc->readdir_filename = ਨਿਰਾ;
  sshc->readdir_longentry = ਨਿਰਾ;

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_BOTTOM);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_READDIR_BOTTOM(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ssh_conn *sshc)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = ਨਿਰਾ;

  ਜੇ(ਨਤੀਜਾ) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
    sshc->actualcode = ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_READDIR_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = ਨਿਰਾ;

  /* no data to transfer */
  Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਖਾਲੀ myssh_quote_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ssh_conn *sshc,
                              ਅਚਲ ਅੱਖਰ *cmd)
{
  ਜੇ(cmd)
    failf(ਅੰਕੜੇ, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE_STATVFS(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  ਜੇ(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(ਅੰਕੜੇ, sshc, "statvfs");
    ਵਾਪਸ SSH_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(statvfs) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _MSC_VER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "I64u"
#ਜਾਂਜੇ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "llu"
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ PRIu64
#ਜੇ_ਅੰਤ
    CURLcode ਨਤੀਜਾ = CURLE_OK;
    ਅੱਖਰ *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_files: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_ਮਾਪ_ਨਕਾਬ "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_ਖੰਡਾਂ, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    ਜੇ(!tmp)
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    ਜੇ(!ਨਤੀਜਾ) {
      ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, tmp, ਲੜੀ_ਲੰਬਾਈ(tmp));
      curlx_free(tmp);
    }
    ਜੇ(ਨਤੀਜਾ) {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ਨਤੀਜਾ;
    }
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ SSH_OK;
}

ਸਥਿਰ ਪੂਰਨ myssh_auth_interactive(ਬਣਤਰ connectdata *conn,
                                  ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc;
  ਪੂਰਨ nprompts;

restart:
  ਚੋਣ(sshc->kbd_state) {
  ਹਾਲਤ 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ(rc == SSH_AUTH_AGAIN)
      ਵਾਪਸ SSH_AGAIN;

    ਜੇ(rc != SSH_AUTH_INFO)
      ਵਾਪਸ SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    ਜੇ(nprompts != 1)
      ਵਾਪਸ SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    ਜੇ(rc < 0)
      ਵਾਪਸ SSH_ERROR;

    FALLTHROUGH();
  ਹਾਲਤ 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ(rc == SSH_AUTH_AGAIN)
      ਵਾਪਸ SSH_AGAIN;
    ਨਹੀਂਤਾਂ ਜੇ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ਨਹੀਂਤਾਂ ਜੇ(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      ਜੇ(nprompts)
        ਵਾਪਸ SSH_ERROR;

      sshc->kbd_state = 2;
      ਜਾਓ restart;
    }
    ਨਹੀਂਤਾਂ
      rc = SSH_ERROR;
    ਰੋਕੋ;
  ਹਾਲਤ 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ(rc == SSH_AUTH_AGAIN)
      ਵਾਪਸ SSH_AGAIN;
    ਨਹੀਂਤਾਂ ਜੇ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ਨਹੀਂਤਾਂ
      rc = SSH_ERROR;

    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ SSH_ERROR;
  }

  sshc->kbd_state = 0;
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਖਾਲੀ myssh_state_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ssh_conn *sshc)
{
  sshc->ਦੂਜਾ_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#ਜੇ 0
  ssh_ਸੈੱਟ_log_level(SSH_LOG_PROTOCOL);
#ਜੇ_ਅੰਤ

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_ਸੈੱਟ_blocking(sshc->ssh_session, 0);

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_S_STARTUP);
}

ਸਥਿਰ ਖਾਲੀ myssh_block2waitfor(ਬਣਤਰ connectdata *conn,
                                ਬਣਤਰ ssh_conn *sshc,
                                bool ਖੰਡ)
{
  (ਖਾਲੀ)conn;
  ਜੇ(ਖੰਡ) {
    ਪੂਰਨ dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  ਨਹੀਂਤਾਂ
    sshc->waitfor = 0;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_S_STARTUP(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ssh_conn *sshc)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਪੂਰਨ rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  ਜੇ(rc == SSH_AGAIN) {
    CURL_TRC_SSH(ਅੰਕੜੇ, "connect -> EAGAIN");
  }
  ਨਹੀਂਤਾਂ ਜੇ(rc != SSH_OK) {
    failf(ਅੰਕੜੇ, "Failure establishing ssh session");
    rc = myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);
  }
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_HOSTKEY);

  ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_AUTHLIST(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, ਨਿਰਾ);
  ਜੇ(rc == SSH_AUTH_AGAIN)
    ਵਾਪਸ SSH_AGAIN;

  ਜੇ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ਅੰਕੜੇ, "Authenticated with none");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
    ਵਾਪਸ rc;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rc == SSH_AUTH_ERROR) {
    rc = myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_LOGIN_DENIED);
    ਵਾਪਸ rc;
  }

  sshc->auth_methods =
    (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ssh_userauth_list(sshc->ssh_session, ਨਿਰਾ);
  ਜੇ(sshc->auth_methods)
    infof(ਅੰਕੜੇ, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  ਜੇ((sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_PUBLICKEY) &&
     ((sshc->priv_ਕੁੰਜੀ && sshc->priv_ਕੁੰਜੀ[0]) ||
      (ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PKEY_INIT);
    infof(ਅੰਕੜੇ, "Authentication using SSH public key file");
  }
  ਨਹੀਂਤਾਂ ਜੇ(sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_GSSAPI_MIC) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_GSSAPI);
  }
  ਨਹੀਂਤਾਂ ਜੇ(sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_INTERACTIVE) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਕੁੰਜੀ_INIT);
  }
  ਨਹੀਂਤਾਂ ਜੇ(sshc->auth_methods & SSH_AUTH_ਵਿਧੀ_PASSWORD) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PASS_INIT);
  }
  ਨਹੀਂਤਾਂ {                  /* unsupported authentication method */
    rc = myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_LOGIN_DENIED);
  }
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_AUTH_PKEY_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc;
  ਜੇ(!(ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_ਮੰਜ਼ਿਲ_GSSAPI_AUTH(ਅੰਕੜੇ, sshc);
    ਵਾਪਸ rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  ਜੇ(sshc->priv_ਕੁੰਜੀ && sshc->priv_ਕੁੰਜੀ[0]) {
    ਜੇ(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, ਨਿਰਾ, sshc->pubkey);
      ਜੇ(rc == SSH_AUTH_AGAIN)
        ਵਾਪਸ SSH_AGAIN;

      ਜੇ(rc != SSH_OK) {
        rc = myssh_ਮੰਜ਼ਿਲ_GSSAPI_AUTH(ਅੰਕੜੇ, sshc);
        ਵਾਪਸ rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_ਕੁੰਜੀ,
                                     sshc->passphrase, ਨਿਰਾ,
                                     ਨਿਰਾ, &sshc->privkey);
    ਜੇ(rc != SSH_OK) {
      failf(ਅੰਕੜੇ, "Could not load private key file %s", sshc->priv_ਕੁੰਜੀ);
      rc = myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_LOGIN_DENIED);
      ਵਾਪਸ rc;
    }

    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PKEY);
  }
  ਨਹੀਂਤਾਂ {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, ਨਿਰਾ,
                                     sshc->passphrase);
    ਜੇ(rc == SSH_AUTH_AGAIN)
      ਵਾਪਸ SSH_AGAIN;

    ਜੇ(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(ਅੰਕੜੇ, "Completed public key authentication");
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
      ਵਾਪਸ rc;
    }

    rc = myssh_ਮੰਜ਼ਿਲ_GSSAPI_AUTH(ਅੰਕੜੇ, sshc);
  }
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_AUTH_PKEY(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = ssh_userauth_publickey(sshc->ssh_session, ਨਿਰਾ, sshc->privkey);
  ਜੇ(rc == SSH_AUTH_AGAIN)
    ਵਾਪਸ SSH_AGAIN;
  ਨਹੀਂਤਾਂ ਜੇ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ਅੰਕੜੇ, "Completed public key authentication");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
    ਵਾਪਸ SSH_OK;
  }
  ਨਹੀਂਤਾਂ {
    infof(ਅੰਕੜੇ, "Failed public key authentication (rc: %d)", rc);
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_GSSAPI_AUTH(ਅੰਕੜੇ, sshc);
  }
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_AUTH_GSSAPI(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc;
  ਜੇ(!(ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ_AUTH(ਅੰਕੜੇ, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  ਜੇ(rc == SSH_AUTH_AGAIN)
    ਵਾਪਸ SSH_AGAIN;

  ਜੇ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ਅੰਕੜੇ, "Completed gssapi authentication");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
    ਵਾਪਸ SSH_OK;
  }

  ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ_AUTH(ਅੰਕੜੇ, sshc);
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_AUTH_ਕੁੰਜੀ_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਕੁੰਜੀ);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_PASSWD_AUTH(ਅੰਕੜੇ, sshc);
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_AUTH_ਕੁੰਜੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  ਪੂਰਨ rc = myssh_auth_interactive(ਅੰਕੜੇ->conn, sshc);
  ਜੇ(rc == SSH_AGAIN)
    ਵਾਪਸ rc;
  ਨਹੀਂਤਾਂ ਜੇ(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(ਅੰਕੜੇ, "completed keyboard interactive authentication");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_PASSWD_AUTH(ਅੰਕੜੇ, sshc);
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_AUTH_PASS_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(!(ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_LOGIN_DENIED);
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_PASS);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_AUTH_PASS(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = ssh_userauth_password(sshc->ssh_session, ਨਿਰਾ,
                                 Curl_creds_passwd(ਅੰਕੜੇ->conn->creds));
  ਜੇ(rc == SSH_AUTH_AGAIN)
    ਵਾਪਸ SSH_AGAIN;
  ਨਹੀਂਤਾਂ ਜੇ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ਅੰਕੜੇ, "Completed password authentication");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTH_ਪੂਰਾ);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_LOGIN_DENIED);
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_AUTH_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ssh_conn *sshc)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਜੇ(!sshc->authed) {
    failf(ਅੰਕੜੇ, "Authentication failure");
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(ਅੰਕੜੇ, "Authentication complete");
  Curl_pgrs_ਸਮਾਂ(ਅੰਕੜੇ, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ਜੇ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_INIT);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  infof(ਅੰਕੜੇ, "SSH CONNECT phase done");
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_UPLOAD_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ssh_conn *sshc,
                                ਬਣਤਰ SSHPROTO *sshp)
{
  ਪੂਰਨ flags;
  ਪੂਰਨ rc = 0;

  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);

  ਜੇ(ਅੰਕੜੇ->state.resume_from) {
    sftp_ਗੁਣ attrs;

    ਜੇ(ਅੰਕੜੇ->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->ਰਸਤਾ);
      ਜੇ(attrs) {
        curl_off_t ਮਾਪ = attrs->ਮਾਪ;
        sftp_ਗੁਣ_free(attrs);
        ਜੇ(ਮਾਪ < 0) {
          failf(ਅੰਕੜੇ, "Bad file size (%" FMT_OFF_T ")", ਮਾਪ);
          rc = myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          ਵਾਪਸ rc;
        }
        ਅੰਕੜੇ->state.resume_from = ਮਾਪ;
      }
      ਨਹੀਂਤਾਂ {
        ਅੰਕੜੇ->state.resume_from = 0;
      }
    }
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  ਨਹੀਂਤਾਂ
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  ਜੇ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->ਰਸਤਾ,
              flags, (mode_t)ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms);
  ਜੇ(!sshc->sftp_file) {
    ਪੂਰਨ err = sftp_get_error(sshc->sftp_session);

    ਜੇ((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_ਰਸਤਾ) &&
       ਅੰਕੜੇ->ਸੈੱਟ.ftp_create_missing_dirs &&
       (ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->ਦੂਜਾ_Create_Dirs = 1;
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ਵਾਪਸ rc;
    }
    ਨਹੀਂਤਾਂ {
      rc = myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਅੰਕੜੇ, sshc);
      ਵਾਪਸ rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
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
        rc = myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FTP_COULDNT_USE_REST);
        ਵਾਪਸ rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ਕਰੋ {
        ਅੱਖਰ scratch[4 * 1024];
        size_t readthisamountnow =
          (ਅੰਕੜੇ->state.resume_from - passed >
           (curl_off_t)ਆਕਾਰ(scratch)) ?
          ਆਕਾਰ(scratch) : curlx_sotouz(ਅੰਕੜੇ->state.resume_from - passed);

        size_t actuallyread =
          ਅੰਕੜੇ->state.fread_func(scratch, 1,
                                 readthisamountnow, ਅੰਕੜੇ->state.ਵਿੱਚ);

        passed += actuallyread;
        ਜੇ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(ਅੰਕੜੇ, "Failed to read data");
          rc = myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FTP_COULDNT_USE_REST);
          ਵਾਪਸ rc;
        }
      } ਜਦੋਂ(passed < ਅੰਕੜੇ->state.resume_from);
    }

    /* now, decrease the size of the read */
    ਜੇ(ਅੰਕੜੇ->state.infilesize > 0) {
      ਜੇ(ਅੰਕੜੇ->state.resume_from > ਅੰਕੜੇ->state.infilesize) {
        failf(ਅੰਕੜੇ, "Resume point beyond size");
        ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_BAD_FUNCTION_ਦਲੀਲ);
      }
      ਅੰਕੜੇ->state.infilesize -= ਅੰਕੜੇ->state.resume_from;
      ਅੰਕੜੇ->req.ਮਾਪ = ਅੰਕੜੇ->state.infilesize;
      Curl_pgrs_ਸੈੱਟ_Upload_ਮਾਪ(ਅੰਕੜੇ, ਅੰਕੜੇ->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, ਅੰਕੜੇ->state.resume_from);
    ਜੇ(rc) {
      rc = myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਅੰਕੜੇ, sshc);
      ਵਾਪਸ rc;
    }
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
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(ਅੰਕੜੇ);
#ਜੇ LIBSSH_ਸੰਸਕਰਣ_INT > SSH_ਸੰਸਕਰਣ_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#ਜੇ_ਅੰਤ
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_DOWNLOAD_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc,
                                       ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  ਜੇ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->ਰਸਤਾ,
                              O_RDONLY, (mode_t)ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms);
  ਜੇ(!sshc->sftp_file) {
    failf(ਅੰਕੜੇ, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਅੰਕੜੇ, sshc);
  }
  sftp_file_ਸੈੱਟ_nonblocking(sshc->sftp_file);
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_DOWNLOAD_STAT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc)
{
  curl_off_t ਮਾਪ;
  ਪੂਰਨ rc = 0;
  sftp_ਗੁਣ attrs = sftp_fstat(sshc->sftp_file);
  ਜੇ(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_ਮਾਪ) ||
     (attrs->ਮਾਪ == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    ਅੰਕੜੇ->req.ਮਾਪ = -1;
    ਅੰਕੜੇ->req.maxdownload = -1;
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, -1);
    ਮਾਪ = 0;
    ਜੇ(attrs)
      sftp_ਗੁਣ_free(attrs);
  }
  ਨਹੀਂਤਾਂ {
    ਮਾਪ = attrs->ਮਾਪ;

    sftp_ਗੁਣ_free(attrs);

    ਜੇ(ਮਾਪ < 0) {
      failf(ਅੰਕੜੇ, "Bad file size (%" FMT_OFF_T ")", ਮਾਪ);
      ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    ਜੇ(ਅੰਕੜੇ->state.use_range) {
      curl_off_t from;
      CURLcode ਨਤੀਜਾ = Curl_ssh_range(ਅੰਕੜੇ, ਅੰਕੜੇ->state.range, ਮਾਪ,
                                       &from, &ਮਾਪ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, ਨਤੀਜਾ);

      rc = sftp_seek64(sshc->sftp_file, from);
      ਜੇ(rc)
        ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਅੰਕੜੇ, sshc);
    }
    ਅੰਕੜੇ->req.ਮਾਪ = ਮਾਪ;
    ਅੰਕੜੇ->req.maxdownload = ਮਾਪ;
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, ਮਾਪ);
  }

  /* We can resume if we can seek to the resume position */
  ਜੇ(ਅੰਕੜੇ->state.resume_from) {
    ਜੇ(ਅੰਕੜੇ->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ਜੇ(ਮਾਪ < -ਅੰਕੜੇ->state.resume_from) {
        failf(ਅੰਕੜੇ, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", ਅੰਕੜੇ->state.resume_from, ਮਾਪ);
        ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      ਅੰਕੜੇ->state.resume_from += ਮਾਪ;
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(ਮਾਪ < ਅੰਕੜੇ->state.resume_from) {
        failf(ਅੰਕੜੇ, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              ਅੰਕੜੇ->state.resume_from, ਮਾਪ);
        ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    ਅੰਕੜੇ->req.ਮਾਪ = ਮਾਪ - ਅੰਕੜੇ->state.resume_from;
    ਅੰਕੜੇ->req.maxdownload = ਮਾਪ - ਅੰਕੜੇ->state.resume_from;
    Curl_pgrs_ਸੈੱਟ_Download_ਮਾਪ(ਅੰਕੜੇ, ਮਾਪ - ਅੰਕੜੇ->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, ਅੰਕੜੇ->state.resume_from);
    ਜੇ(rc)
      ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਅੰਕੜੇ, sshc);
  }

  /* Setup the actual download */
  ਜੇ(ਅੰਕੜੇ->req.ਮਾਪ == 0) {
    /* no data to transfer */
    Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);
    infof(ਅੰਕੜੇ, "File already completely downloaded");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
    ਵਾਪਸ rc;
  }
  Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, ਅੰਕੜੇ->req.ਮਾਪ);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ਅੰਕੜੇ->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);

  ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_CLOSE(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ssh_conn *sshc,
                               ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);
  ਜੇ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ਨਿਰਾ;
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
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  }
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_SHUTDOWN(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_ਸੈੱਟ_blocking(sshc->ssh_session, 0);
#ਜੇ LIBSSH_ਸੰਸਕਰਣ_INT > SSH_ਸੰਸਕਰਣ_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#ਜੇ_ਅੰਤ

  ਜੇ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ਨਿਰਾ;
  }

  ਜੇ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ਨਿਰਾ;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_DISCONNECT);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc;
  ssh_ਸੈੱਟ_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  ਜੇ(!sshc->sftp_session) {
    failf(ਅੰਕੜੇ, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  ਜੇ(rc != SSH_OK) {
    failf(ਅੰਕੜੇ, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, sftp_error_ਮੰਜ਼ਿਲ_CURLE(SSH_FX_FAILURE));
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_REALPATH);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_REALPATH(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_ਰਸਤਾ(sshc->sftp_session, ".");
  ਜੇ(!sshc->homedir)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(ਅੰਕੜੇ->state.most_recent_ftp_entrypath);
  ਅੰਕੜੇ->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  ਜੇ(!ਅੰਕੜੇ->state.most_recent_ftp_entrypath)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(ਅੰਕੜੇ, "CONNECT phase done");
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssh_conn *sshc,
                                    ਬਣਤਰ SSHPROTO *sshp)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);

  ਨਤੀਜਾ = Curl_getworkingpath(ਅੰਕੜੇ, sshc->homedir, &sshp->ਰਸਤਾ);
  ਜੇ(ਨਤੀਜਾ) {
    sshc->actualcode = ਨਤੀਜਾ;
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.quote) {
    infof(ਅੰਕੜੇ, "Sending quote commands");
    sshc->quote_ਵਸਤੂ = ਅੰਕੜੇ->ਸੈੱਟ.quote;
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE);
  }
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_GETINFO);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_POSTQUOTE_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.postquote) {
    infof(ਅੰਕੜੇ, "Sending quote commands");
    sshc->quote_ਵਸਤੂ = ਅੰਕੜੇ->ਸੈੱਟ.postquote;
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  }
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ quote_error(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ ssh_conn *sshc)
{
  failf(ਅੰਕੜੇ, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ssh_conn *sshc,
                               ਬਣਤਰ SSHPROTO *sshp)
{
  ਅਚਲ ਅੱਖਰ *cp;
  CURLcode ਨਤੀਜਾ;

  /*
   * Support some of the "FTP" commands
   */
  ਅਚਲ ਅੱਖਰ *cmd = sshc->quote_ਵਸਤੂ->ਅੰਕੜੇ;
  sshc->acceptfail = FALSE;

  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);

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
    ਜੇ(!tmp) {
      sshc->actualcode = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ਵਾਪਸ SSH_NO_ERROR;
    }
    Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਬਾਹਰਲਾ, "PWD\n", 4);
    Curl_debug(ਅੰਕੜੇ, CURLINFO_ਸਿਰਲੇਖ_ਵਿੱਚ, tmp, ਲੜੀ_ਲੰਬਾਈ(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ, CLIENTWRITE_ਸਿਰਲੇਖ, tmp, ਲੜੀ_ਲੰਬਾਈ(tmp));
    curlx_free(tmp);
    ਜੇ(ਨਤੀਜਾ) {
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ਨਤੀਜਾ;
    }
    ਨਹੀਂਤਾਂ
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
    ਵਾਪਸ SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ਜੇ(!cp) {
    failf(ਅੰਕੜੇ, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    ਵਾਪਸ SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  ਨਤੀਜਾ = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
      failf(ਅੰਕੜੇ, "Out of memory");
    ਨਹੀਂਤਾਂ
      failf(ਅੰਕੜੇ, "Syntax error: Bad first parameter");
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = ਨਤੀਜਾ;
    ਵਾਪਸ SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
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
      ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
        failf(ਅੰਕੜੇ, "Out of memory");
      ਨਹੀਂਤਾਂ
        failf(ਅੰਕੜੇ, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ਨਤੀਜਾ;
      ਵਾਪਸ SSH_NO_ERROR;
    }
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    sshc->quote_attrs = ਨਿਰਾ;
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_STAT);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "ln ", 3) ||
     !ਲੜੀ_ਤੁਲਨਾ(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    ਨਤੀਜਾ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
        failf(ਅੰਕੜੇ, "Out of memory");
      ਨਹੀਂਤਾਂ
        failf(ਅੰਕੜੇ, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ਨਤੀਜਾ;
      ਵਾਪਸ SSH_NO_ERROR;
    }
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "mkdir ", 6)) {
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    /* create directory */
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_MKDIR);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    ਨਤੀਜਾ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ਜੇ(ਨਤੀਜਾ) {
      ਜੇ(ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
        failf(ਅੰਕੜੇ, "Out of memory");
      ਨਹੀਂਤਾਂ
        failf(ਅੰਕੜੇ, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ਨਤੀਜਾ;
      ਵਾਪਸ SSH_NO_ERROR;
    }
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_RENAME);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "rmdir ", 6)) {
    /* delete directory */
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_RMDIR);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "rm ", 3)) {
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_UNLINK);
    ਵਾਪਸ SSH_NO_ERROR;
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_STATVFS_SUPPORT
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "statvfs ", 8)) {
    ਜੇ(*cp)
      ਵਾਪਸ quote_error(ਅੰਕੜੇ, sshc);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_STATVFS);
    ਵਾਪਸ SSH_NO_ERROR;
  }
#ਜੇ_ਅੰਤ

  failf(ਅੰਕੜੇ, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_ਅਗਲਾ_QUOTE(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
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
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE_STAT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssh_conn *sshc)
{
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

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  ਜੇ(sshc->quote_attrs)
    sftp_ਗੁਣ_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  ਜੇ(!sshc->quote_attrs) {
    failf(ਅੰਕੜੇ, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(ਅੰਕੜੇ, sshc, ਨਿਰਾ);
    ਵਾਪਸ SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "chgrp", 5)) {
    ਅਚਲ ਅੱਖਰ *p = sshc->quote_path1;
    curl_off_t gid;
    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &gid, UINT_MAX)) {
      failf(ਅੰਕੜੇ, "Syntax error: chgrp gid not a number");
      myssh_quote_error(ਅੰਕੜੇ, sshc, ਨਿਰਾ);
      ਵਾਪਸ SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "chmod", 5)) {
    curl_off_t perms;
    ਅਚਲ ਅੱਖਰ *p = sshc->quote_path1;
    ਜੇ(curlx_str_octal(&p, &perms, 07777)) {
      failf(ਅੰਕੜੇ, "Syntax error: chmod permissions not a number");
      myssh_quote_error(ਅੰਕੜੇ, sshc, ਨਿਰਾ);
      ਵਾਪਸ SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "chown", 5)) {
    ਅਚਲ ਅੱਖਰ *p = sshc->quote_path1;
    curl_off_t uid;
    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &uid, UINT_MAX)) {
      failf(ਅੰਕੜੇ, "Syntax error: chown uid not a number");
      myssh_quote_error(ਅੰਕੜੇ, sshc, ਨਿਰਾ);
      ਵਾਪਸ SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "atime", 5) ||
          !ਲੜੀ_ਤੁਲਨਾ(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    ਜੇ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(ਅੰਕੜੇ, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ਜੇ SIZEOF_ਸਮਾਂ_T > 4
    ਨਹੀਂਤਾਂ ਜੇ(date > 0xffffffff) {
      failf(ਅੰਕੜੇ, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#ਜੇ_ਅੰਤ
    ਜੇ(fail) {
      myssh_quote_error(ਅੰਕੜੇ, sshc, ਨਿਰਾ);
      ਵਾਪਸ SSH_NO_ERROR;
    }
    ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    ਨਹੀਂਤਾਂ /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਖਾਲੀ conn_forget_socket(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਜੇ(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    ਬਣਤਰ Curl_cfilter *cf = conn->cfilter[sockindex];
    ਜੇ(cf)
      (ਖਾਲੀ)Curl_conn_cf_cntrl(cf, ਅੰਕੜੇ, TRUE, CF_CTRL_FORGET_SOCKET, 0, ਨਿਰਾ);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

ਸਥਿਰ ਖਾਲੀ myssh_SESSION_DISCONNECT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ਜੇ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ਨਿਰਾ;
  }

  ਜੇ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ਨਿਰਾ;
  }
  ਜੇ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ਨਿਰਾ;
  }

  ssh_disconnect(sshc->ssh_session);
  ਜੇ(!ssh_ਸੰਸਕਰਣ(SSH_ਸੰਸਕਰਣ_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(ਅੰਕੜੇ, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_FREE);
}

ਸਥਿਰ ਪੂਰਨ myssh_SSH_SCP_DOWNLOAD(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ssh_conn *sshc)
{
  uint64_t bytecount;
  ਪੂਰਨ rc = ssh_scp_pull_request(sshc->scp_session);
  ਜੇ(rc != SSH_SCP_REQUEST_NEWFILE) {
    ਅਚਲ ਅੱਖਰ *err_ਸੁਨੇਹਾ = ssh_get_error(sshc->ssh_session);
    failf(ਅੰਕੜੇ, "%s", err_ਸੁਨੇਹਾ);
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_REMOTE_FILE_NOT_ਮਿਲਿਆ);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  ਜੇ(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_TOO_LARGE);
  ਅੰਕੜੇ->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, ਅੰਕੜੇ->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ਅੰਕੜੇ->conn->send_idx = 0;

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_TRANS_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ssh_conn *sshc,
                               ਬਣਤਰ SSHPROTO *sshp)
{
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ rc = 0;
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);

  ਨਤੀਜਾ = Curl_getworkingpath(ਅੰਕੜੇ, sshc->homedir, &sshp->ਰਸਤਾ);
  ਜੇ(ਨਤੀਜਾ) {
    sshc->actualcode = ਨਤੀਜਾ;
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
    ਵਾਪਸ 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_ਸੈੱਟ_blocking(sshc->ssh_session, 1);

  ਜੇ(ਅੰਕੜੇ->state.upload) {
    ਜੇ(ਅੰਕੜੇ->state.infilesize < 0) {
      failf(ਅੰਕੜੇ, "SCP requires a known file size for upload");
      ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->ਰਸਤਾ);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ਨਹੀਂਤਾਂ {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->ਰਸਤਾ);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  ਜੇ(!sshc->scp_session) {
    ਅਚਲ ਅੱਖਰ *err_ਸੁਨੇਹਾ = ssh_get_error(sshc->ssh_session);
    failf(ਅੰਕੜੇ, "%s", err_ਸੁਨੇਹਾ);
    rc = myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_UPLOAD_FAILED);
  }
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਖਾਲੀ sshc_cleanup(ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->initialized) {
    ਜੇ(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = ਨਿਰਾ;
    }
    ਜੇ(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = ਨਿਰਾ;
    }
    ਜੇ(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = ਨਿਰਾ;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    ਜੇ(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = ਨਿਰਾ;
    }
    ਜੇ(sshc->quote_attrs) {
      sftp_ਗੁਣ_free(sshc->quote_attrs);
      sshc->quote_attrs = ਨਿਰਾ;
    }
    ਜੇ(sshc->readdir_attrs) {
      sftp_ਗੁਣ_free(sshc->readdir_attrs);
      sshc->readdir_attrs = ਨਿਰਾ;
    }
    ਜੇ(sshc->readdir_link_attrs) {
      sftp_ਗੁਣ_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = ਨਿਰਾ;
    }
    ਜੇ(sshc->privkey) {
      ssh_ਕੁੰਜੀ_free(sshc->privkey);
      sshc->privkey = ਨਿਰਾ;
    }
    ਜੇ(sshc->pubkey) {
      ssh_ਕੁੰਜੀ_free(sshc->pubkey);
      sshc->pubkey = ਨਿਰਾ;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_ਕੁੰਜੀ);
    curlx_safefree(sshc->priv_ਕੁੰਜੀ);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_link_ਰਸਤਾ);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE_SETSTAT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  ਜੇ(rc == SSH_AGAIN)
    ਵਾਪਸ rc;
  ਜੇ(rc && !sshc->acceptfail) {
    myssh_quote_error(ਅੰਕੜੇ, sshc, "setstat");
    ਵਾਪਸ rc;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE_SYMLINK(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  ਜੇ(rc == SSH_AGAIN)
    ਵਾਪਸ rc;
  ਜੇ(rc && !sshc->acceptfail) {
    myssh_quote_error(ਅੰਕੜੇ, sshc, "symlink");
    ਵਾਪਸ rc;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE_MKDIR(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)ਅੰਕੜੇ->ਸੈੱਟ.new_directory_perms);
  ਜੇ(rc == SSH_AGAIN)
    ਵਾਪਸ rc;
  ਜੇ(rc && !sshc->acceptfail) {
    myssh_quote_error(ਅੰਕੜੇ, sshc, "mkdir");
    ਵਾਪਸ rc;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE_RENAME(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  ਜੇ(rc == SSH_AGAIN)
    ਵਾਪਸ rc;
  ਜੇ(rc && !sshc->acceptfail) {
    myssh_quote_error(ਅੰਕੜੇ, sshc, "rename");
    ਵਾਪਸ rc;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE_RMDIR(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  ਜੇ(rc == SSH_AGAIN)
    ਵਾਪਸ rc;
  ਜੇ(rc && !sshc->acceptfail) {
    myssh_quote_error(ਅੰਕੜੇ, sshc, "rmdir");
    ਵਾਪਸ rc;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_QUOTE_UNLINK(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  ਜੇ(rc == SSH_AGAIN)
    ਵਾਪਸ rc;
  ਜੇ(rc && !sshc->acceptfail) {
    myssh_quote_error(ਅੰਕੜੇ, sshc, "rm");
    ਵਾਪਸ rc;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_ਅਗਲਾ_QUOTE);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_GETINFO(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.get_filetime)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_FILETIME);
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_TRANS_INIT);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_FILETIME(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ssh_conn *sshc,
                                  ਬਣਤਰ SSHPROTO *sshp)
{
  sftp_ਗੁਣ attrs;
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->ਰਸਤਾ);
  ਜੇ(attrs) {
    ਅੰਕੜੇ->info.filetime = attrs->mtime;
    sftp_ਗੁਣ_free(attrs);
  }

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_TRANS_INIT);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_TRANS_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssh_conn *sshc,
                                    ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);
  ਜੇ(ਅੰਕੜੇ->state.upload)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_UPLOAD_INIT);
  ਨਹੀਂਤਾਂ {
    size_t ਰਸਤਾ_len = ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ);

    ਜੇ(ਰਸਤਾ_len && sshp->ਰਸਤਾ[ਰਸਤਾ_len - 1] == '/')
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_READDIR_INIT);
    ਨਹੀਂਤਾਂ
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_CREATE_DIRS_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          ਬਣਤਰ ssh_conn *sshc,
                                          ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);

  ਜੇ(ਲੜੀ_ਲੰਬਾਈ(sshp->ਰਸਤਾ) > 1) {
    sshc->slash_pos = sshp->ਰਸਤਾ + 1; /* ignore the leading '/' */
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ਨਹੀਂਤਾਂ {
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_CREATE_DIRS(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ssh_conn *sshc,
                                     ਬਣਤਰ SSHPROTO *sshp)
{
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ਜੇ(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(ਅੰਕੜੇ, "Creating directory '%s'", sshp->ਰਸਤਾ);
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ਵਾਪਸ SSH_NO_ERROR;
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_UPLOAD_INIT);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SFTP_CREATE_DIRS_MKDIR(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਬਣਤਰ ssh_conn *sshc,
                                           ਬਣਤਰ SSHPROTO *sshp)
{
  ਪੂਰਨ rc;
  ਪੂਰਨ err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->ਰਸਤਾ,
                  (mode_t)ਅੰਕੜੇ->ਸੈੱਟ.new_directory_perms);
  ਜੇ(rc == SSH_AGAIN)
    ਵਾਪਸ rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ਜੇ(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    ਜੇ((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_SFTP_CLOSE(ਅੰਕੜੇ, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_CREATE_DIRS);
  ਵਾਪਸ rc;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SCP_UPLOAD_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ssh_conn *sshc,
                                    ਬਣਤਰ SSHPROTO *sshp)
{
  ਪੂਰਨ rc;
  ਜੇ(!sshp)
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  ਜੇ(rc != SSH_OK) {
    ਅਚਲ ਅੱਖਰ *err_ਸੁਨੇਹਾ = ssh_get_error(sshc->ssh_session);
    failf(ਅੰਕੜੇ, "%s", err_ਸੁਨੇਹਾ);
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ਧੱਕੋ_file64(sshc->scp_session, sshp->ਰਸਤਾ,
                           (uint64_t)ਅੰਕੜੇ->state.infilesize,
                           (ਪੂਰਨ)ਅੰਕੜੇ->ਸੈੱਟ.new_file_perms);

  ਜੇ(rc != SSH_OK) {
    ਅਚਲ ਅੱਖਰ *err_ਸੁਨੇਹਾ = ssh_get_error(sshc->ssh_session);
    failf(ਅੰਕੜੇ, "%s", err_ਸੁਨੇਹਾ);
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_ਤਿਆਰੀ_send(ਅੰਕੜੇ, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ਅੰਕੜੇ->conn->recv_idx = FIRSTSOCKET;

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SCP_DOWNLOAD_INIT(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ssh_conn *sshc)
{
  ਪੂਰਨ rc = ssh_scp_init(sshc->scp_session);
  ਜੇ(rc != SSH_OK) {
    ਅਚਲ ਅੱਖਰ *err_ਸੁਨੇਹਾ = ssh_get_error(sshc->ssh_session);
    failf(ਅੰਕੜੇ, "%s", err_ਸੁਨੇਹਾ);
    ਵਾਪਸ myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_DOWNLOAD);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SCP_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(ਅੰਕੜੇ->state.upload)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_SEND_EOF);
  ਨਹੀਂਤਾਂ
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_CHANNEL_FREE);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SCP_SEND_EOF(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->scp_session) {
    ਪੂਰਨ rc = ssh_scp_close(sshc->scp_session);
    ਜੇ(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      ਵਾਪਸ SSH_AGAIN;
    }
    ਜੇ(rc != SSH_OK) {
      infof(ਅੰਕੜੇ,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_CHANNEL_FREE);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ ਪੂਰਨ myssh_ਵਿੱਚ_SCP_CHANNEL_FREE(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ ssh_conn *sshc)
{
  ਜੇ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ਨਿਰਾ;
  }
  CURL_TRC_SSH(ਅੰਕੜੇ, "SCP DONE phase complete");

  ssh_ਸੈੱਟ_blocking(sshc->ssh_session, 0);

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SESSION_DISCONNECT);
  ਵਾਪਸ SSH_NO_ERROR;
}

ਸਥਿਰ CURLcode myssh_ਵਿੱਚ_SESSION_FREE(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ssh_conn *sshc)
{
  CURLcode ਨਤੀਜਾ;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  ਨਤੀਜਾ = sshc->actualcode;
  memset(sshc, 0, ਆਕਾਰ(ਬਣਤਰ ssh_conn));
  connclose(ਅੰਕੜੇ->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
ਸਥਿਰ CURLcode myssh_statemachine(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ssh_conn *sshc,
                                   ਬਣਤਰ SSHPROTO *sshp,
                                   bool *ਖੰਡ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ rc = SSH_NO_ERROR;

  *ਖੰਡ = FALSE;                   /* we are not blocking by default */
  ਕਰੋ {

    ਚੋਣ(sshc->state) {
    ਹਾਲਤ SSH_INIT:
      myssh_state_init(ਅੰਕੜੇ, sshc);
      FALLTHROUGH();
    ਹਾਲਤ SSH_S_STARTUP:
      rc = myssh_ਵਿੱਚ_S_STARTUP(ਅੰਕੜੇ, sshc);
      ਜੇ(rc)
        ਰੋਕੋ;
      FALLTHROUGH();
    ਹਾਲਤ SSH_HOSTKEY:
      rc = myssh_is_ਗਿਆਤ(ਅੰਕੜੇ, sshc);
      ਜੇ(rc != SSH_OK) {
        rc = myssh_ਮੰਜ਼ਿਲ_ERROR(ਅੰਕੜੇ, sshc, CURLE_PEER_FAILED_VERIFICATION);
        ਰੋਕੋ;
      }
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    ਹਾਲਤ SSH_AUTHLIST:
      rc = myssh_ਵਿੱਚ_AUTHLIST(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_AUTH_PKEY_INIT:
      rc = myssh_ਵਿੱਚ_AUTH_PKEY_INIT(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_AUTH_PKEY:
      rc = myssh_ਵਿੱਚ_AUTH_PKEY(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_AUTH_GSSAPI:
      rc = myssh_ਵਿੱਚ_AUTH_GSSAPI(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_AUTH_ਕੁੰਜੀ_INIT:
      rc = myssh_ਵਿੱਚ_AUTH_ਕੁੰਜੀ_INIT(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_AUTH_ਕੁੰਜੀ:
      rc = myssh_ਵਿੱਚ_AUTH_ਕੁੰਜੀ(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_AUTH_PASS_INIT:
      rc = myssh_ਵਿੱਚ_AUTH_PASS_INIT(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_AUTH_PASS:
      rc = myssh_ਵਿੱਚ_AUTH_PASS(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_AUTH_ਪੂਰਾ:
      rc = myssh_ਵਿੱਚ_AUTH_ਪੂਰਾ(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_INIT:
      rc = myssh_ਵਿੱਚ_SFTP_INIT(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_REALPATH:
      rc = myssh_ਵਿੱਚ_SFTP_REALPATH(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE_INIT:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE_INIT(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_ਵਿੱਚ_SFTP_POSTQUOTE_INIT(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_ਅਗਲਾ_QUOTE:
      rc = myssh_ਵਿੱਚ_SFTP_ਅਗਲਾ_QUOTE(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE_STAT:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE_STAT(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE_SETSTAT(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE_SYMLINK(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE_MKDIR(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE_RENAME:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE_RENAME(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE_RMDIR(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE_UNLINK(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_ਵਿੱਚ_SFTP_QUOTE_STATVFS(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_GETINFO:
      rc = myssh_ਵਿੱਚ_SFTP_GETINFO(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_FILETIME:
      rc = myssh_ਵਿੱਚ_SFTP_FILETIME(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_TRANS_INIT:
      rc = myssh_ਵਿੱਚ_SFTP_TRANS_INIT(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_UPLOAD_INIT:
      rc = myssh_ਵਿੱਚ_UPLOAD_INIT(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_ਵਿੱਚ_SFTP_CREATE_DIRS_INIT(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_CREATE_DIRS:
      rc = myssh_ਵਿੱਚ_SFTP_CREATE_DIRS(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_ਵਿੱਚ_SFTP_CREATE_DIRS_MKDIR(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_READDIR_INIT:
      rc = myssh_ਵਿੱਚ_SFTP_READDIR_INIT(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_READDIR:
      rc = myssh_ਵਿੱਚ_SFTP_READDIR(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_READDIR_LINK:
      rc = myssh_ਵਿੱਚ_SFTP_READDIR_LINK(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_ਵਿੱਚ_SFTP_READDIR_BOTTOM(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_READDIR_ਪੂਰਾ:
      rc = myssh_ਵਿੱਚ_SFTP_READDIR_ਪੂਰਾ(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_ਵਿੱਚ_SFTP_DOWNLOAD_INIT(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_ਵਿੱਚ_SFTP_DOWNLOAD_STAT(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_CLOSE:
      rc = myssh_ਵਿੱਚ_SFTP_CLOSE(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SFTP_SHUTDOWN:
      rc = myssh_ਵਿੱਚ_SFTP_SHUTDOWN(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SCP_TRANS_INIT:
      rc = myssh_ਵਿੱਚ_TRANS_INIT(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SCP_UPLOAD_INIT:
      rc = myssh_ਵਿੱਚ_SCP_UPLOAD_INIT(ਅੰਕੜੇ, sshc, sshp);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_ਵਿੱਚ_SCP_DOWNLOAD_INIT(ਅੰਕੜੇ, sshc);
      ਜੇ(rc)
        ਰੋਕੋ;
      FALLTHROUGH();
    ਹਾਲਤ SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SCP_ਪੂਰਾ:
      rc = myssh_ਵਿੱਚ_SCP_ਪੂਰਾ(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SCP_SEND_EOF:
      rc = myssh_ਵਿੱਚ_SCP_SEND_EOF(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_SCP_CHANNEL_FREE:
      myssh_ਵਿੱਚ_SCP_CHANNEL_FREE(ਅੰਕੜੇ, sshc);
      FALLTHROUGH();
    ਹਾਲਤ SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(ਅੰਕੜੇ, sshc);
      FALLTHROUGH();
    ਹਾਲਤ SSH_SESSION_FREE:
      ਨਤੀਜਾ = myssh_ਵਿੱਚ_SESSION_FREE(ਅੰਕੜੇ, sshc);
      ਰੋਕੋ;
    ਹਾਲਤ SSH_QUIT:
    ਮੂਲ:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_STOP);
      ਰੋਕੋ;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } ਜਦੋਂ((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  ਜੇ(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *ਖੰਡ = TRUE;
  }
  ਜੇ(!ਨਤੀਜਾ && (sshc->state == SSH_STOP))
    ਨਤੀਜਾ = sshc->actualcode;
  CURL_TRC_SSH(ਅੰਕੜੇ, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (ਪੂਰਨ)ਨਤੀਜਾ, *ਖੰਡ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* called repeatedly until done from multi.c */
ਸਥਿਰ CURLcode myssh_multi_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      bool *ਪੂਰਾ)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ਬਣਤਰ SSHPROTO *sshp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SSH_EASY);
  bool ਖੰਡ;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!sshc || !sshp)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਨਤੀਜਾ = myssh_statemachine(ਅੰਕੜੇ, sshc, sshp, &ਖੰਡ);
  *ਪੂਰਾ = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, ਖੰਡ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode myssh_ਖੰਡ_statemach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ ssh_conn *sshc,
                                      ਬਣਤਰ SSHPROTO *sshp,
                                      bool disconnect)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜਦੋਂ((sshc->state != SSH_STOP) && !ਨਤੀਜਾ) {
    bool ਖੰਡ;
    timediff_t ਖੱਬਾ_ms = 1000;

    ਨਤੀਜਾ = myssh_statemachine(ਅੰਕੜੇ, sshc, sshp, &ਖੰਡ);
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

    ਜੇ(ਖੰਡ) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (ਖਾਲੀ)SOCKET_READABLE(fd_read,
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
  curlx_free(sshp);
}

ਸਥਿਰ ਖਾਲੀ myssh_conn_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ ssh_conn *sshc = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
ਸਥਿਰ CURLcode myssh_ਤਿਆਰੀ_connection(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ SSHPROTO *sshp;
  ਬਣਤਰ ssh_conn *sshc;

  sshc = curlx_calloc(1, ਆਕਾਰ(*sshc));
  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  curlx_dyn_init(&sshc->readdir_buf, CURL_ਰਸਤਾ_MAX * 2);
  sshc->initialized = TRUE;
  ਜੇ(Curl_conn_meta_ਸੈੱਟ(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  sshp = curlx_calloc(1, ਆਕਾਰ(*sshp));
  ਜੇ(!sshp ||
     Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਵਾਪਸ Curl_ssh_ਤਿਆਰੀ_pkey(ਅੰਕੜੇ, sshc);
}

ਸਥਿਰ Curl_recv scp_recv, sftp_recv;
ਸਥਿਰ Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ਸਥਿਰ CURLcode myssh_connect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ਬਣਤਰ SSHPROTO *ssh = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ਪੂਰਨ rc;

  ਜੇ(!sshc || !ssh)
    ਵਾਪਸ CURLE_FAILED_INIT;

  CURL_TRC_SSH(ਅੰਕੜੇ, "myssh_connect");
  ਜੇ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ਨਹੀਂਤਾਂ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  ਜੇ(!sshc->ssh_session) {
    failf(ਅੰਕੜੇ, "Failure initializing ssh session");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_ਸੈੱਟ(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  ਜੇ(rc != SSH_OK) {
    failf(ਅੰਕੜੇ, "Could not set remote host");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, ਨਿਰਾ);
  ਜੇ(rc != SSH_OK) {
    infof(ਅੰਕੜੇ, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(ਅੰਕੜੇ, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_ਸੈੱਟ(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  ਜੇ(rc != SSH_OK) {
    failf(ਅੰਕੜੇ, "Could not set socket");
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਜੇ(Curl_creds_has_user(conn->creds)) {
    infof(ਅੰਕੜੇ, "User: %s", conn->creds->user);
    rc = ssh_options_ਸੈੱਟ(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    ਜੇ(rc != SSH_OK) {
      failf(ਅੰਕੜੇ, "Could not set user");
      ਵਾਪਸ CURLE_FAILED_INIT;
    }
  }

  ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS)) {
    infof(ਅੰਕੜੇ, "Known hosts: %s",
          CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_ਸੈੱਟ(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS));
    ਜੇ(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_ਸੈੱਟ(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_KNOWNHOSTS));
    ਜੇ(rc != SSH_OK) {
      failf(ਅੰਕੜੇ, "Could not set known hosts file path");
      ਵਾਪਸ CURLE_FAILED_INIT;
    }
  }

  ਜੇ(conn->origin->ਪੋਰਟ) {
    rc = ssh_options_ਸੈੱਟ(sshc->ssh_session, SSH_OPTIONS_ਪੋਰਟ,
                         &conn->origin->ਪੋਰਟ);
    ਜੇ(rc != SSH_OK) {
      failf(ਅੰਕੜੇ, "Could not set remote port");
      ਵਾਪਸ CURLE_FAILED_INIT;
    }
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssh_compression) {
    rc = ssh_options_ਸੈੱਟ(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    ਜੇ(rc != SSH_OK) {
      failf(ਅੰਕੜੇ, "Could not set compression");
      ਵਾਪਸ CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = ਨਿਰਾ;
  sshc->pubkey = ਨਿਰਾ;

  ਜੇ(sshc->pub_ਕੁੰਜੀ) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_ਕੁੰਜੀ, &sshc->pubkey);
    ਜੇ(rc != SSH_OK) {
      failf(ਅੰਕੜੇ, "Could not load public key file");
      ਵਾਪਸ CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_INIT);

  ਨਤੀਜਾ = myssh_multi_statemach(ਅੰਕੜੇ, ਪੂਰਾ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* called from multi.c while DOing */
ਸਥਿਰ CURLcode scp_doing(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = myssh_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  ਜੇ(*dophase_ਪੂਰਾ) {
    CURL_TRC_SSH(ਅੰਕੜੇ, "DO phase is complete");
  }
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
                            bool *connected, bool *dophase_ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(ਅੰਕੜੇ->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(ਅੰਕੜੇ, "DO phase starts");

  *dophase_ਪੂਰਾ = FALSE;        /* not done yet */
  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_TRANS_INIT);

  ਨਤੀਜਾ = myssh_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

  *connected = Curl_conn_is_connected(ਅੰਕੜੇ->conn, FIRSTSOCKET);

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
    ਨਤੀਜਾ = myssh_ਖੰਡ_statemach(ਅੰਕੜੇ, sshc, sshp, TRUE);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ਸਥਿਰ CURLcode myssh_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ ssh_conn *sshc,
                           CURLcode ਸਥਿਤੀ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ SSHPROTO *sshp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SSH_EASY);

  ਜੇ(!ਸਥਿਤੀ && sshp) {
    /* run the state-machine */
    ਨਤੀਜਾ = myssh_ਖੰਡ_statemach(ਅੰਕੜੇ, sshc, sshp, FALSE);
  }
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

  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਜੇ(!ਸਥਿਤੀ)
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SCP_ਪੂਰਾ);

  ਵਾਪਸ myssh_ਪੂਰਾ(ਅੰਕੜੇ, sshc, ਸਥਿਤੀ);
}

ਸਥਿਰ CURLcode scp_send(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                         ਅਚਲ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ਪੂਰਨ rc;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (ਖਾਲੀ)sockindex; /* we only support SCP on the fixed known primary socket */
  (ਖਾਲੀ)eos;
  *pnwritten = 0;

  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#ਜੇ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  ਜੇ(rc == SSH_AGAIN)
    ਵਾਪਸ CURLE_AGAIN;
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  ਜੇ(rc != SSH_OK)
    ਵਾਪਸ CURLE_SSH;

  *pnwritten = len;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode scp_recv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                         ਅੱਖਰ *mem, size_t len, size_t *pnread)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ਪੂਰਨ nread;

  (ਖਾਲੀ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  ਜੇ(nread == SSH_ERROR)
    ਵਾਪਸ CURLE_SSH;
#ਜੇ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  ਜੇ(nread == SSH_AGAIN)
    ਵਾਪਸ CURLE_AGAIN;
#ਜੇ_ਅੰਤ
  *pnread = (size_t)nread;
  ਵਾਪਸ CURLE_OK;
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
  ਨਤੀਜਾ = myssh_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);

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
  CURLcode ਨਤੀਜਾ = myssh_multi_statemach(ਅੰਕੜੇ, dophase_ਪੂਰਾ);
  ਜੇ(*dophase_ਪੂਰਾ) {
    CURL_TRC_SSH(ਅੰਕੜੇ, "DO phase is complete");
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ਸਥਿਰ CURLcode sftp_disconnect(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn,
                                bool dead_connection)
{
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ਬਣਤਰ SSHPROTO *sshp = Curl_meta_get(ਅੰਕੜੇ, CURL_META_SSH_EASY);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)dead_connection;

  CURL_TRC_SSH(ਅੰਕੜੇ, "DISCONNECT starts now");

  ਜੇ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_ਮੰਜ਼ਿਲ(ਅੰਕੜੇ, sshc, SSH_SFTP_SHUTDOWN);
    ਨਤੀਜਾ = myssh_ਖੰਡ_statemach(ਅੰਕੜੇ, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(ਅੰਕੜੇ, "DISCONNECT is done");
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
  ਵਾਪਸ myssh_ਪੂਰਾ(ਅੰਕੜੇ, sshc, ਸਥਿਤੀ);
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

#ਜੇ LIBSSH_ਸੰਸਕਰਣ_INT > SSH_ਸੰਸਕਰਣ_INT(0, 11, 0)
  ਚੋਣ(sshc->sftp_send_state) {
  ਹਾਲਤ 0:
    sftp_file_ਸੈੱਟ_nonblocking(sshc->sftp_file);
    ਜੇ(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  ਹਾਲਤ 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    ਜੇ(nwrite == SSH_AGAIN)
      ਵਾਪਸ CURLE_AGAIN;
    ਨਹੀਂਤਾਂ ਜੇ(nwrite < 0)
      ਵਾਪਸ CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    ਵਾਪਸ CURLE_OK;
  ਮੂਲ:
    /* we never reach here */
    ਵਾਪਸ CURLE_SEND_ERROR;
  }
#ਨਹੀਂਤਾਂ
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  ਜੇ(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#ਜੇ 0 /* not returned by libssh on write */
  ਜੇ(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
  ਜੇ(nwrite < 0)
    ਵਾਪਸ CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  ਵਾਪਸ CURLE_OK;
#ਜੇ_ਅੰਤ
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

  DEBUGASSERT(len < CURL_MAX_READ_ਮਾਪ);
  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਚੋਣ(sshc->sftp_recv_state) {
  ਹਾਲਤ 0:
#ਜੇ LIBSSH_ਸੰਸਕਰਣ_INT > SSH_ਸੰਸਕਰਣ_INT(0, 11, 0)
    ਜੇ(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      ਵਾਪਸ CURLE_RECV_ERROR;
    }
#ਨਹੀਂਤਾਂ
    sshc->sftp_file_ਸੂਚਕਾਂਕ =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    ਜੇ(sshc->sftp_file_ਸੂਚਕਾਂਕ < 0)
      ਵਾਪਸ CURLE_RECV_ERROR;
#ਜੇ_ਅੰਤ

    FALLTHROUGH();
  ਹਾਲਤ 1:
    sshc->sftp_recv_state = 1;

#ਜੇ LIBSSH_ਸੰਸਕਰਣ_INT > SSH_ਸੰਸਕਰਣ_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#ਨਹੀਂਤਾਂ
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_ਸੂਚਕਾਂਕ);
#ਜੇ_ਅੰਤ

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    ਜੇ(nread == SSH_AGAIN)
      ਵਾਪਸ CURLE_AGAIN;
    ਨਹੀਂਤਾਂ ਜੇ(nread < 0)
      ਵਾਪਸ CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    ਵਾਪਸ CURLE_OK;

  ਮੂਲ:
    /* we never reach here */
    ਵਾਪਸ CURLE_RECV_ERROR;
  }
}

ਸਥਿਰ CURLcode myssh_do_it(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  bool connected = FALSE;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *ਪੂਰਾ = FALSE;                /* default to false */
  ਜੇ(!sshc)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਅੰਕੜੇ->req.ਮਾਪ = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
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
  ਜੇ(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_ssh_cleanup(ਖਾਲੀ)
{
  (ਖਾਲੀ)ssh_finalize();
}

ਖਾਲੀ Curl_ssh_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t buflen)
{
  (ਖਾਲੀ)curl_msnprintf(ਬਫਰ, buflen, "libssh/%s", ssh_ਸੰਸਕਰਣ(0));
}

/*
 * SCP.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_scp = {
  myssh_ਤਿਆਰੀ_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_ਪੂਰਾ,                             /* done */
  ZERO_NULL,                            /* do_more */
  myssh_connect,                        /* connect_it */
  myssh_multi_statemach,                /* connecting */
  scp_doing,                            /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  scp_disconnect,                       /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

/*
 * SFTP.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_sftp = {
  myssh_ਤਿਆਰੀ_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_ਪੂਰਾ,                            /* done */
  ZERO_NULL,                            /* do_more */
  myssh_connect,                        /* connect_it */
  myssh_multi_statemach,                /* connecting */
  sftp_doing,                           /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  sftp_disconnect,                      /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#ਜੇ_ਅੰਤ /* USE_LIBSSH */
