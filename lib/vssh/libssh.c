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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH

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
#ಅನ್ತರ್ಭಾವ "parsedate.h"          /* for the week day and month names */
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "vssh/vssh.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h" /* for curlx_base64_encode() */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_UNISTD_H
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCNTL_H
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಆದರೆ_ಅಂತ್ಯ

/* A recent macro provided by libssh. Or make our own. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SSH_STRING_FREE_CHAR
#ವ್ಯಾಖ್ಯಾನಿಸು SSH_STRING_FREE_CHAR(x) \
  ಮಾಡು {                          \
    ಆದರೆ(x) {                     \
      ssh_string_free_char(x);  \
      x = ಖಾಲಿ;                 \
    }                           \
  } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SSH_S_IFMT
#ವ್ಯಾಖ್ಯಾನಿಸು SSH_S_IFMT   00170000
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SSH_S_IFLNK
#ವ್ಯಾಖ್ಯಾನಿಸು SSH_S_IFLNK  0120000
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode sftp_error_ಗಮ್ಯ_CURLE(ಪೂರ್ಣಾಂಕ err)
{
  ಆಯ್ಕೆ(err) {
  ಸ್ಥಿತಿ SSH_FX_OK:
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಸ್ಥಿತಿ SSH_FX_NO_SUCH_FILE:
  ಸ್ಥಿತಿ SSH_FX_NO_SUCH_ಮಾರ್ಗ:
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು;

  ಸ್ಥಿತಿ SSH_FX_PERMISSION_DENIED:
  ಸ್ಥಿತಿ SSH_FX_WRITE_PROTECT:
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_ACCESS_DENIED;

  ಸ್ಥಿತಿ SSH_FX_FILE_ALREADY_EXISTS:
    ಹಿಂತಿರುಗಿ CURLE_REMOTE_FILE_EXISTS;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ CURLE_SSH;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_is_ತಿಳಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc;
  ssh_ಕೀಲಿ pubkey;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *hash_sha256 = ಖಾಲಿ;
  size_t hlen_sha256;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *hash_md5 = ಖಾಲಿ;
  size_t hlen_md5;
  ಅಕ್ಷರ *ಸಿಕ್ಕಿತು_base64 = ಖಾಲಿ;
  ಅಕ್ಷರ *ತಿಳಿದ_base64 = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ vstate;
  ಎಣಿಕೆ curl_khmatch keymatch;
  ರಚನೆ curl_khkey foundkey;
  ರಚನೆ curl_khkey *knownkeyp = ಖಾಲಿ;
  curl_sshkeycallback func = ದತ್ತಾಂಶ->ಗಣ.ssh_keyfunc;
  ರಚನೆ ssh_knownhosts_ನಮೂದು *knownhostsentry = ಖಾಲಿ;
  ರಚನೆ curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  ಆದರೆ(rc != SSH_OK)
    ಹಿಂತಿರುಗಿ rc;

  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_SHA256)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pubkey_sha256 =
      CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_SHA256);
    ಅಕ್ಷರ *fingerprint_b64 = ಖಾಲಿ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    ಆದರೆ(rc != SSH_OK || hlen_sha256 != 32) {
      failf(ದತ್ತಾಂಶ, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      ಹೋಗು cleanup;
    }

    ಆದರೆ(curlx_base64_encode((ಸ್ಥಿರಾಂಕ uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      ಹೋಗು cleanup;
    }

    infof(ದತ್ತಾಂಶ, "SSH SHA256 fingerprint: %s", fingerprint_b64);

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
      rc = SSH_ERROR;
      ಹೋಗು cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    ಹೋಗು cleanup;
  }

  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_MD5)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pubkey_md5 =
      CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_HOST_ಬಹಿರಂಗ_ಕೀಲಿ_MD5);
    ಅಕ್ಷರ md5buffer[33];
    ಪೂರ್ಣಾಂಕ i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    ಆದರೆ(rc != SSH_OK || hlen_md5 != 16) {
      failf(ದತ್ತಾಂಶ,
            "Denied establishing ssh session: MD5 fingerprint not available");
      ಹೋಗು cleanup;
    }

    ಗಾಗಿ(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(ದತ್ತಾಂಶ, "SSH MD5 fingerprint: %s", md5buffer);

    ಆದರೆ(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(ದತ್ತಾಂಶ,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      ಹೋಗು cleanup;
    }

    rc = SSH_OK;
    ಹೋಗು cleanup;
  }

  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_ತಿಳಿದ_hosts_ನಮೂದು(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    ಆದರೆ(knownhostsentry) {
      ಆದರೆ(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &ತಿಳಿದ_base64);
        ಆದರೆ(rc != SSH_OK) {
          ಹೋಗು cleanup;
        }
        knownkey.ಕೀಲಿ = ತಿಳಿದ_base64;
        knownkey.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಿಳಿದ_base64);

        ಆಯ್ಕೆ(ssh_ಕೀಲಿ_type(knownhostsentry->publickey)) {
        ಸ್ಥಿತಿ SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ SSH_KEYTYPE_ECDSA:
        ಸ್ಥಿತಿ SSH_KEYTYPE_ECDSA_P256:
        ಸ್ಥಿತಿ SSH_KEYTYPE_ECDSA_P384:
        ಸ್ಥಿತಿ SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
          rc = SSH_ERROR;
          ಹೋಗು cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    ಆಯ್ಕೆ(vstate) {
    ಸ್ಥಿತಿ SSH_ತಿಳಿದ_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_ತಿಳಿದ_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_ತಿಳಿದ_HOSTS_NOT_ಸಿಕ್ಕಿತು:
    ಸ್ಥಿತಿ SSH_ತಿಳಿದ_HOSTS_UNKNOWN:
    ಸ್ಥಿತಿ SSH_ತಿಳಿದ_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      keymatch = CURLKHMATCH_MISMATCH;
      ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(func) { /* use callback to determine action */
      ರಚನೆ Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &ಸಿಕ್ಕಿತು_base64);
      ಆದರೆ(rc != SSH_OK)
        ಹೋಗು cleanup;

      foundkey.ಕೀಲಿ = ಸಿಕ್ಕಿತು_base64;
      foundkey.len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಿಕ್ಕಿತು_base64);

      ಆಯ್ಕೆ(ssh_ಕೀಲಿ_type(pubkey)) {
      ಸ್ಥಿತಿ SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSH_KEYTYPE_ECDSA:
      ಸ್ಥಿತಿ SSH_KEYTYPE_ECDSA_P256:
      ಸ್ಥಿತಿ SSH_KEYTYPE_ECDSA_P384:
      ಸ್ಥಿತಿ SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        rc = SSH_ERROR;
        ಹೋಗು cleanup;
      }

      CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_ssh_keyfunc);
      rc = func(ದತ್ತಾಂಶ, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, ದತ್ತಾಂಶ->ಗಣ.ssh_keyfunc_userp);
      CURL_CBAPI_ಕೊನೆ(&guard);

      ಆಯ್ಕೆ(rc) {
      ಸ್ಥಿತಿ CURLKHSTAT_FINE_ಕೂಡಿಸು_ಗಮ್ಯ_FILE:
        rc = ssh_session_update_ತಿಳಿದ_hosts(sshc->ssh_session);
        ಆದರೆ(rc != SSH_OK) {
          ಹೋಗು cleanup;
        }
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURLKHSTAT_FINE:
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ: /* REJECT/DEFER */
        rc = SSH_ERROR;
        ಹೋಗು cleanup;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        ಹೋಗು cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  ಆದರೆ(ಸಿಕ್ಕಿತು_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    ಬಿಡುಗಡೆ(ಸಿಕ್ಕಿತು_base64); /* allocated by libssh, deallocate with system free */
  }
  ಆದರೆ(ತಿಳಿದ_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    ಬಿಡುಗಡೆ(ತಿಳಿದ_base64); /* allocated by libssh, deallocate with system free */
  }
  ಆದರೆ(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  ಆದರೆ(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_ಕೀಲಿ_free(pubkey);
  ಆದರೆ(knownhostsentry) {
    ssh_knownhosts_ನಮೂದು_free(knownhostsentry);
  }
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಗಮ್ಯ_ERROR(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ ssh_conn *sshc,
                          CURLcode ಫಲಿತಾಂಶ)
{
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ SSH_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಗಮ್ಯ_SFTP_CLOSE(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ssh_conn *sshc)
{
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_ಗಮ್ಯ_CURLE(sftp_get_error(sshc->sftp_session));
  ಹಿಂತಿರುಗಿ SSH_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಗಮ್ಯ_PASSWD_AUTH(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_PASSWORD) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PASS_INIT);
    ಹಿಂತಿರುಗಿ SSH_OK;
  }
  ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_LOGIN_DENIED);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಗಮ್ಯ_ಕೀಲಿ_AUTH(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_INTERACTIVE) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
    ಹಿಂತಿರುಗಿ SSH_OK;
  }
  ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_PASSWD_AUTH(ದತ್ತಾಂಶ, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಗಮ್ಯ_GSSAPI_AUTH(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_GSSAPI_MIC) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_GSSAPI);
    ಹಿಂತಿರುಗಿ SSH_OK;
  }
  ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ಕೀಲಿ_AUTH(ದತ್ತಾಂಶ, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_READDIR_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ssh_conn *sshc,
                                      ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);
  Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, -1);
  ಆದರೆ(ದತ್ತಾಂಶ->req.no_body) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->ಮಾರ್ಗ);
  ಆದರೆ(!sshc->sftp_dir) {
    failf(ದತ್ತಾಂಶ, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc);
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_READDIR(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ssh_conn *sshc,
                                 ರಚನೆ SSHPROTO *sshp)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  ಆದರೆ(sshc->readdir_attrs)
    sftp_ಗುಣಲಕ್ಷಣಗಳು_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  ಆದರೆ(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->readdir_filename);

    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.list_only) {
      ಅಕ್ಷರ *tmp_ಸಾಲು;

      tmp_ಸಾಲು = curl_maprintf("%s\n", sshc->readdir_filename);
      ಆದರೆ(!tmp_ಸಾಲು) {
        myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹಿಂತಿರುಗಿ SSH_ERROR;
      }
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                                 tmp_ಸಾಲು, sshc->readdir_len + 1);
      curlx_free(tmp_ಸಾಲು);

      ಆದರೆ(ಫಲಿತಾಂಶ) {
        myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
        sshc->actualcode = ಫಲಿತಾಂಶ;
        ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
        ಹಿಂತಿರುಗಿ SSH_ERROR;
      }

      ಆದರೆ((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_link_ಮಾರ್ಗ = curl_maprintf("%s%s", sshp->ಮಾರ್ಗ,
                                               sshc->readdir_filename);

        ಆದರೆ(!sshc->readdir_link_ಮಾರ್ಗ) {
          myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
          ಹಿಂತಿರುಗಿ SSH_ERROR;
        }

        myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_LINK);
        ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
      }
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_BOTTOM);
      ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_ಮುಗಿದ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc);
  }
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_READDIR_LINK(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->readdir_link_attrs)
    sftp_ಗುಣಲಕ್ಷಣಗಳು_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_link_ಮಾರ್ಗ);
  ಆದರೆ(!sshc->readdir_link_attrs) {
    failf(ದತ್ತಾಂಶ, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc);
  }

  ಆದರೆ(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_link_ಮಾರ್ಗ);
    ಆದರೆ(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    ಇಲ್ಲದಿದ್ದರೆ
      sshc->readdir_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->readdir_tmp);
    sshc->readdir_longentry = ಖಾಲಿ;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    sshc->readdir_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_link_ಮಾರ್ಗ);

  ಆದರೆ(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹಿಂತಿರುಗಿ SSH_ERROR;
  }

  sftp_ಗುಣಲಕ್ಷಣಗಳು_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = ಖಾಲಿ;
  sshc->readdir_filename = ಖಾಲಿ;
  sshc->readdir_longentry = ಖಾಲಿ;

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_BOTTOM);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_READDIR_BOTTOM(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ssh_conn *sshc)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = ಖಾಲಿ;

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
    sshc->actualcode = ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_READDIR_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = ಖಾಲಿ;

  /* no data to transfer */
  Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_quote_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ssh_conn *sshc,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd)
{
  ಆದರೆ(cmd)
    failf(ದತ್ತಾಂಶ, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE_STATVFS(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  ಆದರೆ(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(ದತ್ತಾಂಶ, sshc, "statvfs");
    ಹಿಂತಿರುಗಿ SSH_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(statvfs) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _MSC_VER
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "I64u"
#ಅಥವಾಆದರೆ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "llu"
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ PRIu64
#ಆದರೆ_ಅಂತ್ಯ
    CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
    ಅಕ್ಷರ *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_files: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_ಗಾತ್ರ_ಮುಖವಾಡ "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_ಖಂಡಗಳು, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    ಆದರೆ(!tmp)
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, tmp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmp));
      curlx_free(tmp);
    }
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ಫಲಿತಾಂಶ;
    }
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ SSH_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_auth_interactive(ರಚನೆ connectdata *conn,
                                  ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc;
  ಪೂರ್ಣಾಂಕ nprompts;

restart:
  ಆಯ್ಕೆ(sshc->kbd_state) {
  ಸ್ಥಿತಿ 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ(rc == SSH_AUTH_AGAIN)
      ಹಿಂತಿರುಗಿ SSH_AGAIN;

    ಆದರೆ(rc != SSH_AUTH_INFO)
      ಹಿಂತಿರುಗಿ SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    ಆದರೆ(nprompts != 1)
      ಹಿಂತಿರುಗಿ SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    ಆದರೆ(rc < 0)
      ಹಿಂತಿರುಗಿ SSH_ERROR;

    FALLTHROUGH();
  ಸ್ಥಿತಿ 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ(rc == SSH_AUTH_AGAIN)
      ಹಿಂತಿರುಗಿ SSH_AGAIN;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      ಆದರೆ(nprompts)
        ಹಿಂತಿರುಗಿ SSH_ERROR;

      sshc->kbd_state = 2;
      ಹೋಗು restart;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      rc = SSH_ERROR;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ(rc == SSH_AUTH_AGAIN)
      ಹಿಂತಿರುಗಿ SSH_AGAIN;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ಇಲ್ಲದಿದ್ದರೆ
      rc = SSH_ERROR;

    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ SSH_ERROR;
  }

  sshc->kbd_state = 0;
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_state_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ssh_conn *sshc)
{
  sshc->ಎರಡನೇ_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#ಆದರೆ 0
  ssh_ಗಣ_log_level(SSH_LOG_PROTOCOL);
#ಆದರೆ_ಅಂತ್ಯ

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_ಗಣ_blocking(sshc->ssh_session, 0);

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_S_STARTUP);
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_block2waitfor(ರಚನೆ connectdata *conn,
                                ರಚನೆ ssh_conn *sshc,
                                bool ಖಂಡ)
{
  (ಶೂನ್ಯ)conn;
  ಆದರೆ(ಖಂಡ) {
    ಪೂರ್ಣಾಂಕ dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    sshc->waitfor = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_S_STARTUP(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ssh_conn *sshc)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಪೂರ್ಣಾಂಕ rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  ಆದರೆ(rc == SSH_AGAIN) {
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "connect -> EAGAIN");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc != SSH_OK) {
    failf(ದತ್ತಾಂಶ, "Failure establishing ssh session");
    rc = myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_HOSTKEY);

  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_AUTHLIST(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, ಖಾಲಿ);
  ಆದರೆ(rc == SSH_AUTH_AGAIN)
    ಹಿಂತಿರುಗಿ SSH_AGAIN;

  ಆದರೆ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ದತ್ತಾಂಶ, "Authenticated with none");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
    ಹಿಂತಿರುಗಿ rc;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == SSH_AUTH_ERROR) {
    rc = myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_LOGIN_DENIED);
    ಹಿಂತಿರುಗಿ rc;
  }

  sshc->auth_methods =
    (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ssh_userauth_list(sshc->ssh_session, ಖಾಲಿ);
  ಆದರೆ(sshc->auth_methods)
    infof(ದತ್ತಾಂಶ, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  ಆದರೆ((sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_PUBLICKEY) &&
     ((sshc->priv_ಕೀಲಿ && sshc->priv_ಕೀಲಿ[0]) ||
      (ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PKEY_INIT);
    infof(ದತ್ತಾಂಶ, "Authentication using SSH public key file");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_GSSAPI_MIC) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_GSSAPI);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_INTERACTIVE) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sshc->auth_methods & SSH_AUTH_ವಿಧಾನ_PASSWORD) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PASS_INIT);
  }
  ಇಲ್ಲದಿದ್ದರೆ {                  /* unsupported authentication method */
    rc = myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_LOGIN_DENIED);
  }
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_AUTH_PKEY_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc;
  ಆದರೆ(!(ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_ಗಮ್ಯ_GSSAPI_AUTH(ದತ್ತಾಂಶ, sshc);
    ಹಿಂತಿರುಗಿ rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  ಆದರೆ(sshc->priv_ಕೀಲಿ && sshc->priv_ಕೀಲಿ[0]) {
    ಆದರೆ(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, ಖಾಲಿ, sshc->pubkey);
      ಆದರೆ(rc == SSH_AUTH_AGAIN)
        ಹಿಂತಿರುಗಿ SSH_AGAIN;

      ಆದರೆ(rc != SSH_OK) {
        rc = myssh_ಗಮ್ಯ_GSSAPI_AUTH(ದತ್ತಾಂಶ, sshc);
        ಹಿಂತಿರುಗಿ rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_ಕೀಲಿ,
                                     sshc->passphrase, ಖಾಲಿ,
                                     ಖಾಲಿ, &sshc->privkey);
    ಆದರೆ(rc != SSH_OK) {
      failf(ದತ್ತಾಂಶ, "Could not load private key file %s", sshc->priv_ಕೀಲಿ);
      rc = myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_LOGIN_DENIED);
      ಹಿಂತಿರುಗಿ rc;
    }

    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PKEY);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, ಖಾಲಿ,
                                     sshc->passphrase);
    ಆದರೆ(rc == SSH_AUTH_AGAIN)
      ಹಿಂತಿರುಗಿ SSH_AGAIN;

    ಆದರೆ(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(ದತ್ತಾಂಶ, "Completed public key authentication");
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
      ಹಿಂತಿರುಗಿ rc;
    }

    rc = myssh_ಗಮ್ಯ_GSSAPI_AUTH(ದತ್ತಾಂಶ, sshc);
  }
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_AUTH_PKEY(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = ssh_userauth_publickey(sshc->ssh_session, ಖಾಲಿ, sshc->privkey);
  ಆದರೆ(rc == SSH_AUTH_AGAIN)
    ಹಿಂತಿರುಗಿ SSH_AGAIN;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ದತ್ತಾಂಶ, "Completed public key authentication");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
    ಹಿಂತಿರುಗಿ SSH_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    infof(ದತ್ತಾಂಶ, "Failed public key authentication (rc: %d)", rc);
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_GSSAPI_AUTH(ದತ್ತಾಂಶ, sshc);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_AUTH_GSSAPI(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc;
  ಆದರೆ(!(ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ಕೀಲಿ_AUTH(ದತ್ತಾಂಶ, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  ಆದರೆ(rc == SSH_AUTH_AGAIN)
    ಹಿಂತಿರುಗಿ SSH_AGAIN;

  ಆದರೆ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ದತ್ತಾಂಶ, "Completed gssapi authentication");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
    ಹಿಂತಿರುಗಿ SSH_OK;
  }

  ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ಕೀಲಿ_AUTH(ದತ್ತಾಂಶ, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_AUTH_ಕೀಲಿ_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಕೀಲಿ);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_PASSWD_AUTH(ದತ್ತಾಂಶ, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_AUTH_ಕೀಲಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  ಪೂರ್ಣಾಂಕ rc = myssh_auth_interactive(ದತ್ತಾಂಶ->conn, sshc);
  ಆದರೆ(rc == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ rc;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(ದತ್ತಾಂಶ, "completed keyboard interactive authentication");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_PASSWD_AUTH(ದತ್ತಾಂಶ, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_AUTH_PASS_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(!(ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_LOGIN_DENIED);
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_PASS);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_AUTH_PASS(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = ssh_userauth_password(sshc->ssh_session, ಖಾಲಿ,
                                 Curl_creds_passwd(ದತ್ತಾಂಶ->conn->creds));
  ಆದರೆ(rc == SSH_AUTH_AGAIN)
    ಹಿಂತಿರುಗಿ SSH_AGAIN;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ದತ್ತಾಂಶ, "Completed password authentication");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTH_ಮುಗಿದ);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_LOGIN_DENIED);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_AUTH_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ssh_conn *sshc)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಆದರೆ(!sshc->authed) {
    failf(ದತ್ತಾಂಶ, "Authentication failure");
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(ದತ್ತಾಂಶ, "Authentication complete");
  Curl_pgrs_ಸಮಯ(ದತ್ತಾಂಶ, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ಆದರೆ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_INIT);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  infof(ದತ್ತಾಂಶ, "SSH CONNECT phase done");
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_UPLOAD_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ssh_conn *sshc,
                                ರಚನೆ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಂಕ flags;
  ಪೂರ್ಣಾಂಕ rc = 0;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);

  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from) {
    sftp_ಗುಣಲಕ್ಷಣಗಳು attrs;

    ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->ಮಾರ್ಗ);
      ಆದರೆ(attrs) {
        curl_off_t ಗಾತ್ರ_2 = attrs->ಗಾತ್ರ_2;
        sftp_ಗುಣಲಕ್ಷಣಗಳು_free(attrs);
        ಆದರೆ(ಗಾತ್ರ_2 < 0) {
          failf(ದತ್ತಾಂಶ, "Bad file size (%" FMT_OFF_T ")", ಗಾತ್ರ_2);
          rc = myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          ಹಿಂತಿರುಗಿ rc;
        }
        ದತ್ತಾಂಶ->state.resume_from = ಗಾತ್ರ_2;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ದತ್ತಾಂಶ->state.resume_from = 0;
      }
    }
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  ಆದರೆ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->ಮಾರ್ಗ,
              flags, (mode_t)ದತ್ತಾಂಶ->ಗಣ.new_file_perms);
  ಆದರೆ(!sshc->sftp_file) {
    ಪೂರ್ಣಾಂಕ err = sftp_get_error(sshc->sftp_session);

    ಆದರೆ((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_ಮಾರ್ಗ) &&
       ದತ್ತಾಂಶ->ಗಣ.ftp_create_missing_dirs &&
       (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->ಎರಡನೇ_Create_Dirs = 1;
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ಹಿಂತಿರುಗಿ rc;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      rc = myssh_ಗಮ್ಯ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc);
      ಹಿಂತಿರುಗಿ rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
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
        rc = myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FTP_COULDNT_USE_REST);
        ಹಿಂತಿರುಗಿ rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ಮಾಡು {
        ಅಕ್ಷರ scratch[4 * 1024];
        size_t readthisamountnow =
          (ದತ್ತಾಂಶ->state.resume_from - passed >
           (curl_off_t)ಗಾತ್ರ(scratch)) ?
          ಗಾತ್ರ(scratch) : curlx_sotouz(ದತ್ತಾಂಶ->state.resume_from - passed);

        size_t actuallyread =
          ದತ್ತಾಂಶ->state.fread_func(scratch, 1,
                                 readthisamountnow, ದತ್ತಾಂಶ->state.ಒಳ);

        passed += actuallyread;
        ಆದರೆ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(ದತ್ತಾಂಶ, "Failed to read data");
          rc = myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FTP_COULDNT_USE_REST);
          ಹಿಂತಿರುಗಿ rc;
        }
      } ಆಗ(passed < ದತ್ತಾಂಶ->state.resume_from);
    }

    /* now, decrease the size of the read */
    ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize > 0) {
      ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from > ದತ್ತಾಂಶ->state.infilesize) {
        failf(ದತ್ತಾಂಶ, "Resume point beyond size");
        ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_BAD_FUNCTION_ವಾದ);
      }
      ದತ್ತಾಂಶ->state.infilesize -= ದತ್ತಾಂಶ->state.resume_from;
      ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ದತ್ತಾಂಶ->state.infilesize;
      Curl_pgrs_ಗಣ_Upload_ಗಾತ್ರ(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, ದತ್ತಾಂಶ->state.resume_from);
    ಆದರೆ(rc) {
      rc = myssh_ಗಮ್ಯ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc);
      ಹಿಂತಿರುಗಿ rc;
    }
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
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(ದತ್ತಾಂಶ);
#ಆದರೆ LIBSSH_ಆವೃತ್ತಿ_INT > SSH_ಆವೃತ್ತಿ_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#ಆದರೆ_ಅಂತ್ಯ
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_DOWNLOAD_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc,
                                       ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  ಆದರೆ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->ಮಾರ್ಗ,
                              O_RDONLY, (mode_t)ದತ್ತಾಂಶ->ಗಣ.new_file_perms);
  ಆದರೆ(!sshc->sftp_file) {
    failf(ದತ್ತಾಂಶ, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc);
  }
  sftp_file_ಗಣ_nonblocking(sshc->sftp_file);
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_DOWNLOAD_STAT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc)
{
  curl_off_t ಗಾತ್ರ_2;
  ಪೂರ್ಣಾಂಕ rc = 0;
  sftp_ಗುಣಲಕ್ಷಣಗಳು attrs = sftp_fstat(sshc->sftp_file);
  ಆದರೆ(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_ಗಾತ್ರ) ||
     (attrs->ಗಾತ್ರ_2 == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1;
    ದತ್ತಾಂಶ->req.maxdownload = -1;
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, -1);
    ಗಾತ್ರ_2 = 0;
    ಆದರೆ(attrs)
      sftp_ಗುಣಲಕ್ಷಣಗಳು_free(attrs);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಗಾತ್ರ_2 = attrs->ಗಾತ್ರ_2;

    sftp_ಗುಣಲಕ್ಷಣಗಳು_free(attrs);

    ಆದರೆ(ಗಾತ್ರ_2 < 0) {
      failf(ದತ್ತಾಂಶ, "Bad file size (%" FMT_OFF_T ")", ಗಾತ್ರ_2);
      ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    ಆದರೆ(ದತ್ತಾಂಶ->state.use_range) {
      curl_off_t from;
      CURLcode ಫಲಿತಾಂಶ = Curl_ssh_range(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.range, ಗಾತ್ರ_2,
                                       &from, &ಗಾತ್ರ_2);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, ಫಲಿತಾಂಶ);

      rc = sftp_seek64(sshc->sftp_file, from);
      ಆದರೆ(rc)
        ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc);
    }
    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ಗಾತ್ರ_2;
    ದತ್ತಾಂಶ->req.maxdownload = ಗಾತ್ರ_2;
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, ಗಾತ್ರ_2);
  }

  /* We can resume if we can seek to the resume position */
  ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from) {
    ಆದರೆ(ದತ್ತಾಂಶ->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ಆದರೆ(ಗಾತ್ರ_2 < -ದತ್ತಾಂಶ->state.resume_from) {
        failf(ದತ್ತಾಂಶ, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", ದತ್ತಾಂಶ->state.resume_from, ಗಾತ್ರ_2);
        ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      ದತ್ತಾಂಶ->state.resume_from += ಗಾತ್ರ_2;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(ಗಾತ್ರ_2 < ದತ್ತಾಂಶ->state.resume_from) {
        failf(ದತ್ತಾಂಶ, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              ದತ್ತಾಂಶ->state.resume_from, ಗಾತ್ರ_2);
        ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = ಗಾತ್ರ_2 - ದತ್ತಾಂಶ->state.resume_from;
    ದತ್ತಾಂಶ->req.maxdownload = ಗಾತ್ರ_2 - ದತ್ತಾಂಶ->state.resume_from;
    Curl_pgrs_ಗಣ_Download_ಗಾತ್ರ(ದತ್ತಾಂಶ, ಗಾತ್ರ_2 - ದತ್ತಾಂಶ->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, ದತ್ತಾಂಶ->state.resume_from);
    ಆದರೆ(rc)
      ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc);
  }

  /* Setup the actual download */
  ಆದರೆ(ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 == 0) {
    /* no data to transfer */
    Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);
    infof(ದತ್ತಾಂಶ, "File already completely downloaded");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
    ಹಿಂತಿರುಗಿ rc;
  }
  Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ದತ್ತಾಂಶ->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);

  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_CLOSE(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ssh_conn *sshc,
                               ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);
  ಆದರೆ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ಖಾಲಿ;
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
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  }
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_SHUTDOWN(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_ಗಣ_blocking(sshc->ssh_session, 0);
#ಆದರೆ LIBSSH_ಆವೃತ್ತಿ_INT > SSH_ಆವೃತ್ತಿ_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ಖಾಲಿ;
  }

  ಆದರೆ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ಖಾಲಿ;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_DISCONNECT);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc;
  ssh_ಗಣ_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  ಆದರೆ(!sshc->sftp_session) {
    failf(ದತ್ತಾಂಶ, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  ಆದರೆ(rc != SSH_OK) {
    failf(ದತ್ತಾಂಶ, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, sftp_error_ಗಮ್ಯ_CURLE(SSH_FX_FAILURE));
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_REALPATH);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_REALPATH(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_ಮಾರ್ಗ(sshc->sftp_session, ".");
  ಆದರೆ(!sshc->homedir)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath);
  ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  ಆದರೆ(!ದತ್ತಾಂಶ->state.most_recent_ftp_entrypath)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(ದತ್ತಾಂಶ, "CONNECT phase done");
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssh_conn *sshc,
                                    ರಚನೆ SSHPROTO *sshp)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);

  ಫಲಿತಾಂಶ = Curl_getworkingpath(ದತ್ತಾಂಶ, sshc->homedir, &sshp->ಮಾರ್ಗ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    sshc->actualcode = ಫಲಿತಾಂಶ;
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.quote) {
    infof(ದತ್ತಾಂಶ, "Sending quote commands");
    sshc->quote_ಅಂಶ = ದತ್ತಾಂಶ->ಗಣ.quote;
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_GETINFO);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_POSTQUOTE_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.postquote) {
    infof(ದತ್ತಾಂಶ, "Sending quote commands");
    sshc->quote_ಅಂಶ = ದತ್ತಾಂಶ->ಗಣ.postquote;
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  }
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ quote_error(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       ರಚನೆ ssh_conn *sshc)
{
  failf(ದತ್ತಾಂಶ, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ssh_conn *sshc,
                               ರಚನೆ SSHPROTO *sshp)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cp;
  CURLcode ಫಲಿತಾಂಶ;

  /*
   * Support some of the "FTP" commands
   */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmd = sshc->quote_ಅಂಶ->ದತ್ತಾಂಶ;
  sshc->acceptfail = FALSE;

  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);

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
    ಆದರೆ(!tmp) {
      sshc->actualcode = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
    }
    Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಹೊರಹೋಗು, "PWD\n", 4);
    Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಶೀರ್ಷಿಕೆ_ಒಳ, tmp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ, CLIENTWRITE_ಶೀರ್ಷಿಕೆ, tmp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tmp));
    curlx_free(tmp);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ಫಲಿತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ಆದರೆ(!cp) {
    failf(ದತ್ತಾಂಶ, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  ಫಲಿತಾಂಶ = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
      failf(ದತ್ತಾಂಶ, "Out of memory");
    ಇಲ್ಲದಿದ್ದರೆ
      failf(ದತ್ತಾಂಶ, "Syntax error: Bad first parameter");
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = ಫಲಿತಾಂಶ;
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
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
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
        failf(ದತ್ತಾಂಶ, "Out of memory");
      ಇಲ್ಲದಿದ್ದರೆ
        failf(ದತ್ತಾಂಶ, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ಫಲಿತಾಂಶ;
      ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
    }
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    sshc->quote_attrs = ಖಾಲಿ;
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_STAT);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "ln ", 3) ||
     !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    ಫಲಿತಾಂಶ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
        failf(ದತ್ತಾಂಶ, "Out of memory");
      ಇಲ್ಲದಿದ್ದರೆ
        failf(ದತ್ತಾಂಶ, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ಫಲಿತಾಂಶ;
      ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
    }
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "mkdir ", 6)) {
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    /* create directory */
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_MKDIR);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    ಫಲಿತಾಂಶ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
        failf(ದತ್ತಾಂಶ, "Out of memory");
      ಇಲ್ಲದಿದ್ದರೆ
        failf(ದತ್ತಾಂಶ, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ಫಲಿತಾಂಶ;
      ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
    }
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_RENAME);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "rmdir ", 6)) {
    /* delete directory */
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_RMDIR);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "rm ", 3)) {
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_UNLINK);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_STATVFS_SUPPORT
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "statvfs ", 8)) {
    ಆದರೆ(*cp)
      ಹಿಂತಿರುಗಿ quote_error(ದತ್ತಾಂಶ, sshc);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_STATVFS);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
#ಆದರೆ_ಅಂತ್ಯ

  failf(ದತ್ತಾಂಶ, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_ಮುಂದಿನ_QUOTE(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
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
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE_STAT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssh_conn *sshc)
{
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

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  ಆದರೆ(sshc->quote_attrs)
    sftp_ಗುಣಲಕ್ಷಣಗಳು_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  ಆದರೆ(!sshc->quote_attrs) {
    failf(ದತ್ತಾಂಶ, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(ದತ್ತಾಂಶ, sshc, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chgrp", 5)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = sshc->quote_path1;
    curl_off_t gid;
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &gid, UINT_MAX)) {
      failf(ದತ್ತಾಂಶ, "Syntax error: chgrp gid not a number");
      myssh_quote_error(ದತ್ತಾಂಶ, sshc, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chmod", 5)) {
    curl_off_t perms;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = sshc->quote_path1;
    ಆದರೆ(curlx_str_octal(&p, &perms, 07777)) {
      failf(ದತ್ತಾಂಶ, "Syntax error: chmod permissions not a number");
      myssh_quote_error(ದತ್ತಾಂಶ, sshc, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "chown", 5)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = sshc->quote_path1;
    curl_off_t uid;
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &uid, UINT_MAX)) {
      failf(ದತ್ತಾಂಶ, "Syntax error: chown uid not a number");
      myssh_quote_error(ದತ್ತಾಂಶ, sshc, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "atime", 5) ||
          !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    ಆದರೆ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(ದತ್ತಾಂಶ, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ಆದರೆ SIZEOF_ಸಮಯ_T > 4
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(date > 0xffffffff) {
      failf(ದತ್ತಾಂಶ, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(fail) {
      myssh_quote_error(ದತ್ತಾಂಶ, sshc, ಖಾಲಿ);
      ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
    }
    ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    ಇಲ್ಲದಿದ್ದರೆ /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಶೂನ್ಯ conn_forget_socket(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ಆದರೆ(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    ರಚನೆ Curl_cfilter *cf = conn->cfilter[sockindex];
    ಆದರೆ(cf)
      (ಶೂನ್ಯ)Curl_conn_cf_cntrl(cf, ದತ್ತಾಂಶ, TRUE, CF_CTRL_FORGET_SOCKET, 0, ಖಾಲಿ);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_SESSION_DISCONNECT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ಆದರೆ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ಖಾಲಿ;
  }

  ಆದರೆ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ಖಾಲಿ;
  }
  ಆದರೆ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ಖಾಲಿ;
  }

  ssh_disconnect(sshc->ssh_session);
  ಆದರೆ(!ssh_ಆವೃತ್ತಿ(SSH_ಆವೃತ್ತಿ_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(ದತ್ತಾಂಶ, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_FREE);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_SSH_SCP_DOWNLOAD(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ssh_conn *sshc)
{
  uint64_t bytecount;
  ಪೂರ್ಣಾಂಕ rc = ssh_scp_pull_request(sshc->scp_session);
  ಆದರೆ(rc != SSH_SCP_REQUEST_NEWFILE) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(ದತ್ತಾಂಶ, "%s", err_ಸಂದೇಶ);
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_REMOTE_FILE_NOT_ಸಿಕ್ಕಿತು);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  ಆದರೆ(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_TOO_LARGE);
  ದತ್ತಾಂಶ->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, ದತ್ತಾಂಶ->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ದತ್ತಾಂಶ->conn->send_idx = 0;

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_TRANS_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ssh_conn *sshc,
                               ರಚನೆ SSHPROTO *sshp)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ rc = 0;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);

  ಫಲಿತಾಂಶ = Curl_getworkingpath(ದತ್ತಾಂಶ, sshc->homedir, &sshp->ಮಾರ್ಗ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    sshc->actualcode = ಫಲಿತಾಂಶ;
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
    ಹಿಂತಿರುಗಿ 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_ಗಣ_blocking(sshc->ssh_session, 1);

  ಆದರೆ(ದತ್ತಾಂಶ->state.upload) {
    ಆದರೆ(ದತ್ತಾಂಶ->state.infilesize < 0) {
      failf(ದತ್ತಾಂಶ, "SCP requires a known file size for upload");
      ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->ಮಾರ್ಗ);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->ಮಾರ್ಗ);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  ಆದರೆ(!sshc->scp_session) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(ದತ್ತಾಂಶ, "%s", err_ಸಂದೇಶ);
    rc = myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_UPLOAD_FAILED);
  }
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ sshc_cleanup(ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->initialized) {
    ಆದರೆ(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = ಖಾಲಿ;
    }
    ಆದರೆ(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = ಖಾಲಿ;
    }
    ಆದರೆ(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = ಖಾಲಿ;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    ಆದರೆ(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = ಖಾಲಿ;
    }
    ಆದರೆ(sshc->quote_attrs) {
      sftp_ಗುಣಲಕ್ಷಣಗಳು_free(sshc->quote_attrs);
      sshc->quote_attrs = ಖಾಲಿ;
    }
    ಆದರೆ(sshc->readdir_attrs) {
      sftp_ಗುಣಲಕ್ಷಣಗಳು_free(sshc->readdir_attrs);
      sshc->readdir_attrs = ಖಾಲಿ;
    }
    ಆದರೆ(sshc->readdir_link_attrs) {
      sftp_ಗುಣಲಕ್ಷಣಗಳು_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = ಖಾಲಿ;
    }
    ಆದರೆ(sshc->privkey) {
      ssh_ಕೀಲಿ_free(sshc->privkey);
      sshc->privkey = ಖಾಲಿ;
    }
    ಆದರೆ(sshc->pubkey) {
      ssh_ಕೀಲಿ_free(sshc->pubkey);
      sshc->pubkey = ಖಾಲಿ;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_ಕೀಲಿ);
    curlx_safefree(sshc->priv_ಕೀಲಿ);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_link_ಮಾರ್ಗ);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE_SETSTAT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  ಆದರೆ(rc == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ rc;
  ಆದರೆ(rc && !sshc->acceptfail) {
    myssh_quote_error(ದತ್ತಾಂಶ, sshc, "setstat");
    ಹಿಂತಿರುಗಿ rc;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE_SYMLINK(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  ಆದರೆ(rc == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ rc;
  ಆದರೆ(rc && !sshc->acceptfail) {
    myssh_quote_error(ದತ್ತಾಂಶ, sshc, "symlink");
    ಹಿಂತಿರುಗಿ rc;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE_MKDIR(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)ದತ್ತಾಂಶ->ಗಣ.new_directory_perms);
  ಆದರೆ(rc == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ rc;
  ಆದರೆ(rc && !sshc->acceptfail) {
    myssh_quote_error(ದತ್ತಾಂಶ, sshc, "mkdir");
    ಹಿಂತಿರುಗಿ rc;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE_RENAME(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  ಆದರೆ(rc == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ rc;
  ಆದರೆ(rc && !sshc->acceptfail) {
    myssh_quote_error(ದತ್ತಾಂಶ, sshc, "rename");
    ಹಿಂತಿರುಗಿ rc;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE_RMDIR(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  ಆದರೆ(rc == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ rc;
  ಆದರೆ(rc && !sshc->acceptfail) {
    myssh_quote_error(ದತ್ತಾಂಶ, sshc, "rmdir");
    ಹಿಂತಿರುಗಿ rc;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_QUOTE_UNLINK(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  ಆದರೆ(rc == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ rc;
  ಆದರೆ(rc && !sshc->acceptfail) {
    myssh_quote_error(ದತ್ತಾಂಶ, sshc, "rm");
    ಹಿಂತಿರುಗಿ rc;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_ಮುಂದಿನ_QUOTE);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_GETINFO(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.get_filetime)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_FILETIME);
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_TRANS_INIT);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_FILETIME(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ssh_conn *sshc,
                                  ರಚನೆ SSHPROTO *sshp)
{
  sftp_ಗುಣಲಕ್ಷಣಗಳು attrs;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->ಮಾರ್ಗ);
  ಆದರೆ(attrs) {
    ದತ್ತಾಂಶ->info.filetime = attrs->mtime;
    sftp_ಗುಣಲಕ್ಷಣಗಳು_free(attrs);
  }

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_TRANS_INIT);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_TRANS_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssh_conn *sshc,
                                    ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);
  ಆದರೆ(ದತ್ತಾಂಶ->state.upload)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_UPLOAD_INIT);
  ಇಲ್ಲದಿದ್ದರೆ {
    size_t ಮಾರ್ಗ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ);

    ಆದರೆ(ಮಾರ್ಗ_len && sshp->ಮಾರ್ಗ[ಮಾರ್ಗ_len - 1] == '/')
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_READDIR_INIT);
    ಇಲ್ಲದಿದ್ದರೆ
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_CREATE_DIRS_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          ರಚನೆ ssh_conn *sshc,
                                          ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);

  ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(sshp->ಮಾರ್ಗ) > 1) {
    sshc->slash_pos = sshp->ಮಾರ್ಗ + 1; /* ignore the leading '/' */
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_CREATE_DIRS(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ssh_conn *sshc,
                                     ರಚನೆ SSHPROTO *sshp)
{
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ಆದರೆ(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(ದತ್ತಾಂಶ, "Creating directory '%s'", sshp->ಮಾರ್ಗ);
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_UPLOAD_INIT);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SFTP_CREATE_DIRS_MKDIR(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ರಚನೆ ssh_conn *sshc,
                                           ರಚನೆ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಂಕ rc;
  ಪೂರ್ಣಾಂಕ err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->ಮಾರ್ಗ,
                  (mode_t)ದತ್ತಾಂಶ->ಗಣ.new_directory_perms);
  ಆದರೆ(rc == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ಆದರೆ(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    ಆದರೆ((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_CREATE_DIRS);
  ಹಿಂತಿರುಗಿ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SCP_UPLOAD_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ssh_conn *sshc,
                                    ರಚನೆ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಂಕ rc;
  ಆದರೆ(!sshp)
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  ಆದರೆ(rc != SSH_OK) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(ದತ್ತಾಂಶ, "%s", err_ಸಂದೇಶ);
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ತಳ್ಳು_file64(sshc->scp_session, sshp->ಮಾರ್ಗ,
                           (uint64_t)ದತ್ತಾಂಶ->state.infilesize,
                           (ಪೂರ್ಣಾಂಕ)ದತ್ತಾಂಶ->ಗಣ.new_file_perms);

  ಆದರೆ(rc != SSH_OK) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(ದತ್ತಾಂಶ, "%s", err_ಸಂದೇಶ);
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_ಸಿದ್ಧತೆ_send(ದತ್ತಾಂಶ, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ದತ್ತಾಂಶ->conn->recv_idx = FIRSTSOCKET;

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SCP_DOWNLOAD_INIT(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಂಕ rc = ssh_scp_init(sshc->scp_session);
  ಆದರೆ(rc != SSH_OK) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(ದತ್ತಾಂಶ, "%s", err_ಸಂದೇಶ);
    ಹಿಂತಿರುಗಿ myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_DOWNLOAD);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SCP_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(ದತ್ತಾಂಶ->state.upload)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_SEND_EOF);
  ಇಲ್ಲದಿದ್ದರೆ
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_CHANNEL_FREE);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SCP_SEND_EOF(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->scp_session) {
    ಪೂರ್ಣಾಂಕ rc = ssh_scp_close(sshc->scp_session);
    ಆದರೆ(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      ಹಿಂತಿರುಗಿ SSH_AGAIN;
    }
    ಆದರೆ(rc != SSH_OK) {
      infof(ದತ್ತಾಂಶ,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_CHANNEL_FREE);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ myssh_ಒಳ_SCP_CHANNEL_FREE(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ ssh_conn *sshc)
{
  ಆದರೆ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ಖಾಲಿ;
  }
  CURL_TRC_SSH(ದತ್ತಾಂಶ, "SCP DONE phase complete");

  ssh_ಗಣ_blocking(sshc->ssh_session, 0);

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SESSION_DISCONNECT);
  ಹಿಂತಿರುಗಿ SSH_NO_ERROR;
}

ಸ್ಥಿರ CURLcode myssh_ಒಳ_SESSION_FREE(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ssh_conn *sshc)
{
  CURLcode ಫಲಿತಾಂಶ;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  ಫಲಿತಾಂಶ = sshc->actualcode;
  memset(sshc, 0, ಗಾತ್ರ(ರಚನೆ ssh_conn));
  connclose(ದತ್ತಾಂಶ->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
ಸ್ಥಿರ CURLcode myssh_statemachine(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ssh_conn *sshc,
                                   ರಚನೆ SSHPROTO *sshp,
                                   bool *ಖಂಡ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ rc = SSH_NO_ERROR;

  *ಖಂಡ = FALSE;                   /* we are not blocking by default */
  ಮಾಡು {

    ಆಯ್ಕೆ(sshc->state) {
    ಸ್ಥಿತಿ SSH_INIT:
      myssh_state_init(ದತ್ತಾಂಶ, sshc);
      FALLTHROUGH();
    ಸ್ಥಿತಿ SSH_S_STARTUP:
      rc = myssh_ಒಳ_S_STARTUP(ದತ್ತಾಂಶ, sshc);
      ಆದರೆ(rc)
        ನಿಲ್ಲಿಸು;
      FALLTHROUGH();
    ಸ್ಥಿತಿ SSH_HOSTKEY:
      rc = myssh_is_ತಿಳಿದ(ದತ್ತಾಂಶ, sshc);
      ಆದರೆ(rc != SSH_OK) {
        rc = myssh_ಗಮ್ಯ_ERROR(ದತ್ತಾಂಶ, sshc, CURLE_PEER_FAILED_VERIFICATION);
        ನಿಲ್ಲಿಸು;
      }
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    ಸ್ಥಿತಿ SSH_AUTHLIST:
      rc = myssh_ಒಳ_AUTHLIST(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_AUTH_PKEY_INIT:
      rc = myssh_ಒಳ_AUTH_PKEY_INIT(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_AUTH_PKEY:
      rc = myssh_ಒಳ_AUTH_PKEY(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_AUTH_GSSAPI:
      rc = myssh_ಒಳ_AUTH_GSSAPI(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_AUTH_ಕೀಲಿ_INIT:
      rc = myssh_ಒಳ_AUTH_ಕೀಲಿ_INIT(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_AUTH_ಕೀಲಿ:
      rc = myssh_ಒಳ_AUTH_ಕೀಲಿ(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_AUTH_PASS_INIT:
      rc = myssh_ಒಳ_AUTH_PASS_INIT(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_AUTH_PASS:
      rc = myssh_ಒಳ_AUTH_PASS(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_AUTH_ಮುಗಿದ:
      rc = myssh_ಒಳ_AUTH_ಮುಗಿದ(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_INIT:
      rc = myssh_ಒಳ_SFTP_INIT(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_REALPATH:
      rc = myssh_ಒಳ_SFTP_REALPATH(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_INIT:
      rc = myssh_ಒಳ_SFTP_QUOTE_INIT(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_ಒಳ_SFTP_POSTQUOTE_INIT(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE:
      rc = myssh_ಒಳ_SFTP_QUOTE(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_ಮುಂದಿನ_QUOTE:
      rc = myssh_ಒಳ_SFTP_ಮುಂದಿನ_QUOTE(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_STAT:
      rc = myssh_ಒಳ_SFTP_QUOTE_STAT(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_ಒಳ_SFTP_QUOTE_SETSTAT(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_ಒಳ_SFTP_QUOTE_SYMLINK(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_ಒಳ_SFTP_QUOTE_MKDIR(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_RENAME:
      rc = myssh_ಒಳ_SFTP_QUOTE_RENAME(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_ಒಳ_SFTP_QUOTE_RMDIR(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_ಒಳ_SFTP_QUOTE_UNLINK(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_ಒಳ_SFTP_QUOTE_STATVFS(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_GETINFO:
      rc = myssh_ಒಳ_SFTP_GETINFO(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_FILETIME:
      rc = myssh_ಒಳ_SFTP_FILETIME(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_TRANS_INIT:
      rc = myssh_ಒಳ_SFTP_TRANS_INIT(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_UPLOAD_INIT:
      rc = myssh_ಒಳ_UPLOAD_INIT(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_ಒಳ_SFTP_CREATE_DIRS_INIT(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_CREATE_DIRS:
      rc = myssh_ಒಳ_SFTP_CREATE_DIRS(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_ಒಳ_SFTP_CREATE_DIRS_MKDIR(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_READDIR_INIT:
      rc = myssh_ಒಳ_SFTP_READDIR_INIT(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_READDIR:
      rc = myssh_ಒಳ_SFTP_READDIR(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_READDIR_LINK:
      rc = myssh_ಒಳ_SFTP_READDIR_LINK(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_ಒಳ_SFTP_READDIR_BOTTOM(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_READDIR_ಮುಗಿದ:
      rc = myssh_ಒಳ_SFTP_READDIR_ಮುಗಿದ(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_ಒಳ_SFTP_DOWNLOAD_INIT(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_ಒಳ_SFTP_DOWNLOAD_STAT(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_CLOSE:
      rc = myssh_ಒಳ_SFTP_CLOSE(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SFTP_SHUTDOWN:
      rc = myssh_ಒಳ_SFTP_SHUTDOWN(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SCP_TRANS_INIT:
      rc = myssh_ಒಳ_TRANS_INIT(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SCP_UPLOAD_INIT:
      rc = myssh_ಒಳ_SCP_UPLOAD_INIT(ದತ್ತಾಂಶ, sshc, sshp);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_ಒಳ_SCP_DOWNLOAD_INIT(ದತ್ತಾಂಶ, sshc);
      ಆದರೆ(rc)
        ನಿಲ್ಲಿಸು;
      FALLTHROUGH();
    ಸ್ಥಿತಿ SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SCP_ಮುಗಿದ:
      rc = myssh_ಒಳ_SCP_ಮುಗಿದ(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SCP_SEND_EOF:
      rc = myssh_ಒಳ_SCP_SEND_EOF(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_SCP_CHANNEL_FREE:
      myssh_ಒಳ_SCP_CHANNEL_FREE(ದತ್ತಾಂಶ, sshc);
      FALLTHROUGH();
    ಸ್ಥಿತಿ SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(ದತ್ತಾಂಶ, sshc);
      FALLTHROUGH();
    ಸ್ಥಿತಿ SSH_SESSION_FREE:
      ಫಲಿತಾಂಶ = myssh_ಒಳ_SESSION_FREE(ದತ್ತಾಂಶ, sshc);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSH_QUIT:
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_STOP);
      ನಿಲ್ಲಿಸು;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } ಆಗ((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  ಆದರೆ(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *ಖಂಡ = TRUE;
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && (sshc->state == SSH_STOP))
    ಫಲಿತಾಂಶ = sshc->actualcode;
  CURL_TRC_SSH(ದತ್ತಾಂಶ, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಖಂಡ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode myssh_multi_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      bool *ಮುಗಿದ)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ರಚನೆ SSHPROTO *sshp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SSH_EASY);
  bool ಖಂಡ;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!sshc || !sshp)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಫಲಿತಾಂಶ = myssh_statemachine(ದತ್ತಾಂಶ, sshc, sshp, &ಖಂಡ);
  *ಮುಗಿದ = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, ಖಂಡ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode myssh_ಖಂಡ_statemach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ ssh_conn *sshc,
                                      ರಚನೆ SSHPROTO *sshp,
                                      bool disconnect)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಗ((sshc->state != SSH_STOP) && !ಫಲಿತಾಂಶ) {
    bool ಖಂಡ;
    timediff_t ಎಡ_ms = 1000;

    ಫಲಿತಾಂಶ = myssh_statemachine(ದತ್ತಾಂಶ, sshc, sshp, &ಖಂಡ);
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

    ಆದರೆ(ಖಂಡ) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (ಶೂನ್ಯ)SOCKET_READABLE(fd_read,
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
  curlx_free(sshp);
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ ssh_conn *sshc = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
ಸ್ಥಿರ CURLcode myssh_ಸಿದ್ಧತೆ_connection(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ connectdata *conn)
{
  ರಚನೆ SSHPROTO *sshp;
  ರಚನೆ ssh_conn *sshc;

  sshc = curlx_calloc(1, ಗಾತ್ರ(*sshc));
  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  curlx_dyn_init(&sshc->readdir_buf, CURL_ಮಾರ್ಗ_MAX * 2);
  sshc->initialized = TRUE;
  ಆದರೆ(Curl_conn_meta_ಗಣ(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  sshp = curlx_calloc(1, ಗಾತ್ರ(*sshp));
  ಆದರೆ(!sshp ||
     Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಹಿಂತಿರುಗಿ Curl_ssh_ಸಿದ್ಧತೆ_pkey(ದತ್ತಾಂಶ, sshc);
}

ಸ್ಥಿರ Curl_recv scp_recv, sftp_recv;
ಸ್ಥಿರ Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ಸ್ಥಿರ CURLcode myssh_connect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ರಚನೆ SSHPROTO *ssh = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!sshc || !ssh)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  CURL_TRC_SSH(ದತ್ತಾಂಶ, "myssh_connect");
  ಆದರೆ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  ಆದರೆ(!sshc->ssh_session) {
    failf(ದತ್ತಾಂಶ, "Failure initializing ssh session");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_ಗಣ(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  ಆದರೆ(rc != SSH_OK) {
    failf(ದತ್ತಾಂಶ, "Could not set remote host");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, ಖಾಲಿ);
  ಆದರೆ(rc != SSH_OK) {
    infof(ದತ್ತಾಂಶ, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(ದತ್ತಾಂಶ, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_ಗಣ(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  ಆದರೆ(rc != SSH_OK) {
    failf(ದತ್ತಾಂಶ, "Could not set socket");
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಆದರೆ(Curl_creds_has_user(conn->creds)) {
    infof(ದತ್ತಾಂಶ, "User: %s", conn->creds->user);
    rc = ssh_options_ಗಣ(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    ಆದರೆ(rc != SSH_OK) {
      failf(ದತ್ತಾಂಶ, "Could not set user");
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }
  }

  ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS)) {
    infof(ದತ್ತಾಂಶ, "Known hosts: %s",
          CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_ಗಣ(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS));
    ಆದರೆ(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_ಗಣ(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_KNOWNHOSTS));
    ಆದರೆ(rc != SSH_OK) {
      failf(ದತ್ತಾಂಶ, "Could not set known hosts file path");
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }
  }

  ಆದರೆ(conn->origin->ಪೋರ್ಟ್) {
    rc = ssh_options_ಗಣ(sshc->ssh_session, SSH_OPTIONS_ಪೋರ್ಟ್,
                         &conn->origin->ಪೋರ್ಟ್);
    ಆದರೆ(rc != SSH_OK) {
      failf(ದತ್ತಾಂಶ, "Could not set remote port");
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssh_compression) {
    rc = ssh_options_ಗಣ(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    ಆದರೆ(rc != SSH_OK) {
      failf(ದತ್ತಾಂಶ, "Could not set compression");
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = ಖಾಲಿ;
  sshc->pubkey = ಖಾಲಿ;

  ಆದರೆ(sshc->pub_ಕೀಲಿ) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_ಕೀಲಿ, &sshc->pubkey);
    ಆದರೆ(rc != SSH_OK) {
      failf(ದತ್ತಾಂಶ, "Could not load public key file");
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_INIT);

  ಫಲಿತಾಂಶ = myssh_multi_statemach(ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* called from multi.c while DOing */
ಸ್ಥಿರ CURLcode scp_doing(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = myssh_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  ಆದರೆ(*dophase_ಮುಗಿದ) {
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "DO phase is complete");
  }
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
                            bool *connected, bool *dophase_ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(ದತ್ತಾಂಶ->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(ದತ್ತಾಂಶ, "DO phase starts");

  *dophase_ಮುಗಿದ = FALSE;        /* not done yet */
  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_TRANS_INIT);

  ಫಲಿತಾಂಶ = myssh_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

  *connected = Curl_conn_is_connected(ದತ್ತಾಂಶ->conn, FIRSTSOCKET);

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
    ಫಲಿತಾಂಶ = myssh_ಖಂಡ_statemach(ದತ್ತಾಂಶ, sshc, sshp, TRUE);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ಸ್ಥಿರ CURLcode myssh_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ ssh_conn *sshc,
                           CURLcode ಸ್ಥಿತಿ_2)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ SSHPROTO *sshp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SSH_EASY);

  ಆದರೆ(!ಸ್ಥಿತಿ_2 && sshp) {
    /* run the state-machine */
    ಫಲಿತಾಂಶ = myssh_ಖಂಡ_statemach(ದತ್ತಾಂಶ, sshc, sshp, FALSE);
  }
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

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಆದರೆ(!ಸ್ಥಿತಿ_2)
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SCP_ಮುಗಿದ);

  ಹಿಂತಿರುಗಿ myssh_ಮುಗಿದ(ದತ್ತಾಂಶ, sshc, ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರ CURLcode scp_send(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                         ಸ್ಥಿರಾಂಕ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ಪೂರ್ಣಾಂಕ rc;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (ಶೂನ್ಯ)sockindex; /* we only support SCP on the fixed known primary socket */
  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#ಆದರೆ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  ಆದರೆ(rc == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(rc != SSH_OK)
    ಹಿಂತಿರುಗಿ CURLE_SSH;

  *pnwritten = len;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode scp_recv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                         ಅಕ್ಷರ *mem, size_t len, size_t *pnread)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ಪೂರ್ಣಾಂಕ nread;

  (ಶೂನ್ಯ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  ಆದರೆ(nread == SSH_ERROR)
    ಹಿಂತಿರುಗಿ CURLE_SSH;
#ಆದರೆ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  ಆದರೆ(nread == SSH_AGAIN)
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
#ಆದರೆ_ಅಂತ್ಯ
  *pnread = (size_t)nread;
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
  ಫಲಿತಾಂಶ = myssh_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);

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
  CURLcode ಫಲಿತಾಂಶ = myssh_multi_statemach(ದತ್ತಾಂಶ, dophase_ಮುಗಿದ);
  ಆದರೆ(*dophase_ಮುಗಿದ) {
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "DO phase is complete");
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ಸ್ಥಿರ CURLcode sftp_disconnect(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn,
                                bool dead_connection)
{
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ರಚನೆ SSHPROTO *sshp = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_SSH_EASY);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)dead_connection;

  CURL_TRC_SSH(ದತ್ತಾಂಶ, "DISCONNECT starts now");

  ಆದರೆ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_ಗಮ್ಯ(ದತ್ತಾಂಶ, sshc, SSH_SFTP_SHUTDOWN);
    ಫಲಿತಾಂಶ = myssh_ಖಂಡ_statemach(ದತ್ತಾಂಶ, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(ದತ್ತಾಂಶ, "DISCONNECT is done");
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
  ಹಿಂತಿರುಗಿ myssh_ಮುಗಿದ(ದತ್ತಾಂಶ, sshc, ಸ್ಥಿತಿ_2);
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

#ಆದರೆ LIBSSH_ಆವೃತ್ತಿ_INT > SSH_ಆವೃತ್ತಿ_INT(0, 11, 0)
  ಆಯ್ಕೆ(sshc->sftp_send_state) {
  ಸ್ಥಿತಿ 0:
    sftp_file_ಗಣ_nonblocking(sshc->sftp_file);
    ಆದರೆ(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  ಸ್ಥಿತಿ 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    ಆದರೆ(nwrite == SSH_AGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nwrite < 0)
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* we never reach here */
    ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  ಆದರೆ(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#ಆದರೆ 0 /* not returned by libssh on write */
  ಆದರೆ(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(nwrite < 0)
    ಹಿಂತಿರುಗಿ CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  ಹಿಂತಿರುಗಿ CURLE_OK;
#ಆದರೆ_ಅಂತ್ಯ
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

  DEBUGASSERT(len < CURL_MAX_READ_ಗಾತ್ರ);
  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಆಯ್ಕೆ(sshc->sftp_recv_state) {
  ಸ್ಥಿತಿ 0:
#ಆದರೆ LIBSSH_ಆವೃತ್ತಿ_INT > SSH_ಆವೃತ್ತಿ_INT(0, 11, 0)
    ಆದರೆ(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    sshc->sftp_file_ಸೂಚ್ಯಂಕ =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    ಆದರೆ(sshc->sftp_file_ಸೂಚ್ಯಂಕ < 0)
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
#ಆದರೆ_ಅಂತ್ಯ

    FALLTHROUGH();
  ಸ್ಥಿತಿ 1:
    sshc->sftp_recv_state = 1;

#ಆದರೆ LIBSSH_ಆವೃತ್ತಿ_INT > SSH_ಆವೃತ್ತಿ_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#ಇಲ್ಲದಿದ್ದರೆ
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_ಸೂಚ್ಯಂಕ);
#ಆದರೆ_ಅಂತ್ಯ

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    ಆದರೆ(nread == SSH_AGAIN)
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread < 0)
      ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಪೂರ್ವನಿಯೋಜಿತ:
    /* we never reach here */
    ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
  }
}

ಸ್ಥಿರ CURLcode myssh_do_it(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  bool connected = FALSE;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *ಮುಗಿದ = FALSE;                /* default to false */
  ಆದರೆ(!sshc)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ದತ್ತಾಂಶ->req.ಗಾತ್ರ_2 = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
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
  ಆದರೆ(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssh_cleanup(ಶೂನ್ಯ)
{
  (ಶೂನ್ಯ)ssh_finalize();
}

ಶೂನ್ಯ Curl_ssh_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t buflen)
{
  (ಶೂನ್ಯ)curl_msnprintf(ಬಫರ್, buflen, "libssh/%s", ssh_ಆವೃತ್ತಿ(0));
}

/*
 * SCP.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_scp = {
  myssh_ಸಿದ್ಧತೆ_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_ಮುಗಿದ,                             /* done */
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
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_sftp = {
  myssh_ಸಿದ್ಧತೆ_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_ಮುಗಿದ,                            /* done */
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

#ಆದರೆ_ಅಂತ್ಯ /* USE_LIBSSH */
