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

#ifdef USE_LIBSSH

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
#ಅನ್ತರ್ಭಾವ "parsedate.h"          /* for the week day and month names */
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "vssh/vssh.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#ಅನ್ತರ್ಭಾವ <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  ಕುರು {                          \
    ಯದಿ(x) {                     \
      ssh_string_free_char(x);  \
      x = ರಿಕ್ತ;                 \
    }                           \
  } ಯಾವತ್(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

ಸ್ಥಿರ CURLcode sftp_error_to_CURLE(ಪೂರ್ಣಾಙ್ಕ err)
{
  ವಿಕಲ್ಪ(err) {
  ಪಕ್ಷ SSH_FX_OK:
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಪಕ್ಷ SSH_FX_NO_SUCH_FILE:
  ಪಕ್ಷ SSH_FX_NO_SUCH_PATH:
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_FILE_NOT_FOUND;

  ಪಕ್ಷ SSH_FX_PERMISSION_DENIED:
  ಪಕ್ಷ SSH_FX_WRITE_PROTECT:
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_ACCESS_DENIED;

  ಪಕ್ಷ SSH_FX_FILE_ALREADY_EXISTS:
    ಪ್ರತ್ಯಾಗಮ CURLE_REMOTE_FILE_EXISTS;

  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_SSH;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_is_known(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ssh_ಕೀಲಿ pubkey;
  unsigned ವರ್ಣ *hash_sha256 = ರಿಕ್ತ;
  size_t hlen_sha256;
  unsigned ವರ್ಣ *hash_md5 = ರಿಕ್ತ;
  size_t hlen_md5;
  ವರ್ಣ *found_base64 = ರಿಕ್ತ;
  ವರ್ಣ *known_base64 = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ vstate;
  ಗಣನಾ curl_khmatch keymatch;
  ಸಂರಚನಾ curl_khkey foundkey;
  ಸಂರಚನಾ curl_khkey *knownkeyp = ರಿಕ್ತ;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  ಸಂರಚನಾ ssh_knownhosts_entry *knownhostsentry = ರಿಕ್ತ;
  ಸಂರಚನಾ curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  ಯದಿ(rc != SSH_OK)
    ಪ್ರತ್ಯಾಗಮ rc;

  ಯದಿ(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_SHA256)) {
    ಧ್ರುವ ವರ್ಣ *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_SHA256);
    ವರ್ಣ *fingerprint_b64 = ರಿಕ್ತ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    ಯದಿ(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      ಗಚ್ಛ cleanup;
    }

    ಯದಿ(curlx_base64_encode((ಧ್ರುವ uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      ಗಚ್ಛ cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

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
      rc = SSH_ERROR;
      ಗಚ್ಛ cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    ಗಚ್ಛ cleanup;
  }

  ಯದಿ(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_MD5)) {
    ಧ್ರುವ ವರ್ಣ *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ಕೀಲಿ_MD5);
    ವರ್ಣ md5buffer[33];
    ಪೂರ್ಣಾಙ್ಕ i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    ಯದಿ(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      ಗಚ್ಛ cleanup;
    }

    ಪ್ರತಿ(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    ಯದಿ(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      ಗಚ್ಛ cleanup;
    }

    rc = SSH_OK;
    ಗಚ್ಛ cleanup;
  }

  ಯದಿ(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    ಯದಿ(knownhostsentry) {
      ಯದಿ(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        ಯದಿ(rc != SSH_OK) {
          ಗಚ್ಛ cleanup;
        }
        knownkey.ಕೀಲಿ = known_base64;
        knownkey.len = strlen(known_base64);

        ವಿಕಲ್ಪ(ssh_ಕೀಲಿ_type(knownhostsentry->publickey)) {
        ಪಕ್ಷ SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          ಭಙ್ಗ;
        ಪಕ್ಷ SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          ಭಙ್ಗ;
        ಪಕ್ಷ SSH_KEYTYPE_ECDSA:
        ಪಕ್ಷ SSH_KEYTYPE_ECDSA_P256:
        ಪಕ್ಷ SSH_KEYTYPE_ECDSA_P384:
        ಪಕ್ಷ SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          ಭಙ್ಗ;
        ಪಕ್ಷ SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          ಭಙ್ಗ;
        ಪಕ್ಷ SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          ಭಙ್ಗ;
        ಸ್ವಭಾವ:
          rc = SSH_ERROR;
          ಗಚ್ಛ cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    ವಿಕಲ್ಪ(vstate) {
    ಪಕ್ಷ SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_KNOWN_HOSTS_NOT_FOUND:
    ಪಕ್ಷ SSH_KNOWN_HOSTS_UNKNOWN:
    ಪಕ್ಷ SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      keymatch = CURLKHMATCH_MISMATCH;
      ಭಙ್ಗ;
    }

    ಯದಿ(func) { /* use callback to determine action */
      ಸಂರಚನಾ Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      ಯದಿ(rc != SSH_OK)
        ಗಚ್ಛ cleanup;

      foundkey.ಕೀಲಿ = found_base64;
      foundkey.len = strlen(found_base64);

      ವಿಕಲ್ಪ(ssh_ಕೀಲಿ_type(pubkey)) {
      ಪಕ್ಷ SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        ಭಙ್ಗ;
      ಪಕ್ಷ SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        ಭಙ್ಗ;
      ಪಕ್ಷ SSH_KEYTYPE_ECDSA:
      ಪಕ್ಷ SSH_KEYTYPE_ECDSA_P256:
      ಪಕ್ಷ SSH_KEYTYPE_ECDSA_P384:
      ಪಕ್ಷ SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        ಭಙ್ಗ;
      ಪಕ್ಷ SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        ಭಙ್ಗ;
      ಪಕ್ಷ SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        rc = SSH_ERROR;
        ಗಚ್ಛ cleanup;
      }

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      ವಿಕಲ್ಪ(rc) {
      ಪಕ್ಷ CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        ಯದಿ(rc != SSH_OK) {
          ಗಚ್ಛ cleanup;
        }
        ಭಙ್ಗ;
      ಪಕ್ಷ CURLKHSTAT_FINE:
        ಭಙ್ಗ;
      ಸ್ವಭಾವ: /* REJECT/DEFER */
        rc = SSH_ERROR;
        ಗಚ್ಛ cleanup;
      }
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        ಗಚ್ಛ cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  ಯದಿ(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    ಮುಕ್ತ(found_base64); /* allocated by libssh, deallocate with system free */
  }
  ಯದಿ(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    ಮುಕ್ತ(known_base64); /* allocated by libssh, deallocate with system free */
  }
  ಯದಿ(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  ಯದಿ(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_ಕೀಲಿ_free(pubkey);
  ಯದಿ(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_to_ERROR(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  ಪ್ರತ್ಯಾಗಮ SSH_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_to_SFTP_CLOSE(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  ಪ್ರತ್ಯಾಗಮ SSH_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_to_PASSWD_AUTH(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    ಪ್ರತ್ಯಾಗಮ SSH_OK;
  }
  ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_to_ಕೀಲಿ_AUTH(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
    ಪ್ರತ್ಯಾಗಮ SSH_OK;
  }
  ಪ್ರತ್ಯಾಗಮ myssh_to_PASSWD_AUTH(data, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_to_GSSAPI_AUTH(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    ಪ್ರತ್ಯಾಗಮ SSH_OK;
  }
  ಪ್ರತ್ಯಾಗಮ myssh_to_ಕೀಲಿ_AUTH(data, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_READDIR_INIT(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ssh_conn *sshc,
                                      ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  ಯದಿ(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  ಯದಿ(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ಪ್ರತ್ಯಾಗಮ myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_READDIR(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ssh_conn *sshc,
                                 ಸಂರಚನಾ SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  ಯದಿ(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  ಯದಿ(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    ಯದಿ(data->set.list_only) {
      ವರ್ಣ *tmp_ಸಾಲು;

      tmp_ಸಾಲು = curl_maprintf("%s\n", sshc->readdir_filename);
      ಯದಿ(!tmp_ಸಾಲು) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        ಪ್ರತ್ಯಾಗಮ SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmp_ಸಾಲು, sshc->readdir_len + 1);
      curlx_free(tmp_ಸಾಲು);

      ಯದಿ(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
      }
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        ಪ್ರತ್ಯಾಗಮ SSH_ERROR;
      }

      ಯದಿ((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        ಯದಿ(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          ಪ್ರತ್ಯಾಗಮ SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  ಅನ್ಯಥಾ {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ಪ್ರತ್ಯಾಗಮ myssh_to_SFTP_CLOSE(data, sshc);
  }
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_READDIR_LINK(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  ಯದಿ(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ಪ್ರತ್ಯಾಗಮ myssh_to_SFTP_CLOSE(data, sshc);
  }

  ಯದಿ(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    ಯದಿ(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    ಅನ್ಯಥಾ
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = ರಿಕ್ತ;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  ಅನ್ಯಥಾ {
    sshc->readdir_len = strlen(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  ಯದಿ(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    ಪ್ರತ್ಯಾಗಮ SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = ರಿಕ್ತ;
  sshc->readdir_filename = ರಿಕ್ತ;
  sshc->readdir_longentry = ರಿಕ್ತ;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_READDIR_BOTTOM(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ssh_conn *sshc)
{
  CURLcode result;

  ಯದಿ(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  ಅನ್ಯಥಾ
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = ರಿಕ್ತ;

  ಯದಿ(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_READDIR_DONE(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = ರಿಕ್ತ;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_quote_error(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ssh_conn *sshc,
                              ಧ್ರುವ ವರ್ಣ *cmd)
{
  ಯದಿ(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE_STATVFS(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  ಯದಿ(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    ಪ್ರತ್ಯಾಗಮ SSH_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#ನೋಚೇದ್ಯದಿ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#ಅನ್ಯಥಾ
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    ವರ್ಣ *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_files: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_SIZE_MASK "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_blocks, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    ಯದಿ(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    ಯದಿ(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    ಯದಿ(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ SSH_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_auth_interactive(ಸಂರಚನಾ connectdata *conn,
                                  ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಪೂರ್ಣಾಙ್ಕ nprompts;

restart:
  ವಿಕಲ್ಪ(sshc->kbd_state) {
  ಪಕ್ಷ 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ(rc == SSH_AUTH_AGAIN)
      ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;

    ಯದಿ(rc != SSH_AUTH_INFO)
      ಪ್ರತ್ಯಾಗಮ SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    ಯದಿ(nprompts != 1)
      ಪ್ರತ್ಯಾಗಮ SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    ಯದಿ(rc < 0)
      ಪ್ರತ್ಯಾಗಮ SSH_ERROR;

    FALLTHROUGH();
  ಪಕ್ಷ 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ(rc == SSH_AUTH_AGAIN)
      ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;
    ಅನ್ಯಥಾ ಯದಿ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ಅನ್ಯಥಾ ಯದಿ(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      ಯದಿ(nprompts)
        ಪ್ರತ್ಯಾಗಮ SSH_ERROR;

      sshc->kbd_state = 2;
      ಗಚ್ಛ restart;
    }
    ಅನ್ಯಥಾ
      rc = SSH_ERROR;
    ಭಙ್ಗ;
  ಪಕ್ಷ 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ(rc == SSH_AUTH_AGAIN)
      ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;
    ಅನ್ಯಥಾ ಯದಿ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ಅನ್ಯಥಾ
      rc = SSH_ERROR;

    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ SSH_ERROR;
  }

  sshc->kbd_state = 0;
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_state_init(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#ಯದಿ 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_block2waitfor(ಸಂರಚನಾ connectdata *conn,
                                ಸಂರಚನಾ ssh_conn *sshc,
                                bool block)
{
  (ಶೂನ್ಯ)conn;
  ಯದಿ(block) {
    ಪೂರ್ಣಾಙ್ಕ dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  ಅನ್ಯಥಾ
    sshc->waitfor = 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_S_STARTUP(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ssh_conn *sshc)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಪೂರ್ಣಾಙ್ಕ rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  ಯದಿ(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  ಅನ್ಯಥಾ ಯದಿ(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_HOSTKEY);

  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_AUTHLIST(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, ರಿಕ್ತ);
  ಯದಿ(rc == SSH_AUTH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;

  ಯದಿ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ಪ್ರತ್ಯಾಗಮ rc;
  }
  ಅನ್ಯಥಾ ಯದಿ(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    ಪ್ರತ್ಯಾಗಮ rc;
  }

  sshc->auth_methods =
    (unsigned ಪೂರ್ಣಾಙ್ಕ)ssh_userauth_list(sshc->ssh_session, ರಿಕ್ತ);
  ಯದಿ(sshc->auth_methods)
    infof(data, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  ಯದಿ((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_ಕೀಲಿ && sshc->priv_ಕೀಲಿ[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  ಅನ್ಯಥಾ ಯದಿ(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  ಅನ್ಯಥಾ ಯದಿ(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_ಕೀಲಿ_INIT);
  }
  ಅನ್ಯಥಾ ಯದಿ(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  ಅನ್ಯಥಾ {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_AUTH_PKEY_INIT(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಯದಿ(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    ಪ್ರತ್ಯಾಗಮ rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  ಯದಿ(sshc->priv_ಕೀಲಿ && sshc->priv_ಕೀಲಿ[0]) {
    ಯದಿ(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, ರಿಕ್ತ, sshc->pubkey);
      ಯದಿ(rc == SSH_AUTH_AGAIN)
        ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;

      ಯದಿ(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        ಪ್ರತ್ಯಾಗಮ rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_ಕೀಲಿ,
                                     sshc->passphrase, ರಿಕ್ತ,
                                     ರಿಕ್ತ, &sshc->privkey);
    ಯದಿ(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_ಕೀಲಿ);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      ಪ್ರತ್ಯಾಗಮ rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  ಅನ್ಯಥಾ {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, ರಿಕ್ತ,
                                     sshc->passphrase);
    ಯದಿ(rc == SSH_AUTH_AGAIN)
      ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;

    ಯದಿ(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      ಪ್ರತ್ಯಾಗಮ rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_AUTH_PKEY(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = ssh_userauth_publickey(sshc->ssh_session, ರಿಕ್ತ, sshc->privkey);
  ಯದಿ(rc == SSH_AUTH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;
  ಅನ್ಯಥಾ ಯದಿ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ಪ್ರತ್ಯಾಗಮ SSH_OK;
  }
  ಅನ್ಯಥಾ {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    ಪ್ರತ್ಯಾಗಮ myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_AUTH_GSSAPI(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಯದಿ(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    ಪ್ರತ್ಯಾಗಮ myssh_to_ಕೀಲಿ_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  ಯದಿ(rc == SSH_AUTH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;

  ಯದಿ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ಪ್ರತ್ಯಾಗಮ SSH_OK;
  }

  ಪ್ರತ್ಯಾಗಮ myssh_to_ಕೀಲಿ_AUTH(data, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_AUTH_ಕೀಲಿ_INIT(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_ಕೀಲಿ);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ myssh_to_PASSWD_AUTH(data, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_AUTH_ಕೀಲಿ(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  ಪೂರ್ಣಾಙ್ಕ rc = myssh_auth_interactive(data->conn, sshc);
  ಯದಿ(rc == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ rc;
  ಅನ್ಯಥಾ ಯದಿ(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ myssh_to_PASSWD_AUTH(data, sshc);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_AUTH_PASS_INIT(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_AUTH_PASS(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = ssh_userauth_password(sshc->ssh_session, ರಿಕ್ತ,
                                 Curl_creds_passwd(data->conn->creds));
  ಯದಿ(rc == SSH_AUTH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;
  ಅನ್ಯಥಾ ಯದಿ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_AUTH_DONE(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ssh_conn *sshc)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಯದಿ(!sshc->authed) {
    failf(data, "Authentication failure");
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ಯದಿ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_UPLOAD_INIT(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ ssh_conn *sshc,
                                ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಙ್ಕ flags;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  ಯದಿ(data->state.resume_from) {
    sftp_attributes attrs;

    ಯದಿ(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      ಯದಿ(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        ಯದಿ(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          ಪ್ರತ್ಯಾಗಮ rc;
        }
        data->state.resume_from = size;
      }
      ಅನ್ಯಥಾ {
        data->state.resume_from = 0;
      }
    }
  }

  ಯದಿ(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  ಅನ್ಯಥಾ ಯದಿ(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  ಅನ್ಯಥಾ
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  ಯದಿ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  ಯದಿ(!sshc->sftp_file) {
    ಪೂರ್ಣಾಙ್ಕ err = sftp_get_error(sshc->sftp_session);

    ಯದಿ((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ಪ್ರತ್ಯಾಗಮ rc;
    }
    ಅನ್ಯಥಾ {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      ಪ್ರತ್ಯಾಗಮ rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
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
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        ಪ್ರತ್ಯಾಗಮ rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ಕುರು {
        ವರ್ಣ scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)ಪರಿಮಾಣ(scratch)) ?
          ಪರಿಮಾಣ(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        ಯದಿ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          ಪ್ರತ್ಯಾಗಮ rc;
        }
      } ಯಾವತ್(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    ಯದಿ(data->state.infilesize > 0) {
      ಯದಿ(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    ಯದಿ(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      ಪ್ರತ್ಯಾಗಮ rc;
    }
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
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(data);
#ಯದಿ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_DOWNLOAD_INIT(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc,
                                       ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  ಯದಿ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  ಯದಿ(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    ಪ್ರತ್ಯಾಗಮ myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_DOWNLOAD_STAT(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc)
{
  curl_off_t size;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  ಯದಿ(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_SIZE) ||
     (attrs->size == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    data->req.size = -1;
    data->req.maxdownload = -1;
    Curl_pgrsSetDownloadSize(data, -1);
    size = 0;
    ಯದಿ(attrs)
      sftp_attributes_free(attrs);
  }
  ಅನ್ಯಥಾ {
    size = attrs->size;

    sftp_attributes_free(attrs);

    ಯದಿ(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    ಯದಿ(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      ಯದಿ(rc)
        ಪ್ರತ್ಯಾಗಮ myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  ಯದಿ(data->state.resume_from) {
    ಯದಿ(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ಯದಿ(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    ಯದಿ(rc)
      ಪ್ರತ್ಯಾಗಮ myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  ಯದಿ(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    ಪ್ರತ್ಯಾಗಮ rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_CLOSE(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ssh_conn *sshc,
                               ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  ಯದಿ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ರಿಕ್ತ;
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
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_STOP);
  }
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_SHUTDOWN(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#ಯದಿ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  ಯದಿ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ರಿಕ್ತ;
  }

  ಯದಿ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ರಿಕ್ತ;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_INIT(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  ಯದಿ(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  ಯದಿ(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_REALPATH(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  ಯದಿ(!sshc->homedir)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  ಯದಿ(!data->state.most_recent_ftp_entrypath)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE_INIT(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssh_conn *sshc,
                                    ಸಂರಚನಾ SSHPROTO *sshp)
{
  CURLcode result;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  ಯದಿ(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  ಅನ್ಯಥಾ ಯದಿ(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_POSTQUOTE_INIT(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_STOP);
  }
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ quote_error(ಸಂರಚನಾ Curl_easy *data,
                       ಸಂರಚನಾ ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE(ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ssh_conn *sshc,
                               ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಧ್ರುವ ವರ್ಣ *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  ಧ್ರುವ ವರ್ಣ *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

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
    ಯದಿ(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    ಯದಿ(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    ಅನ್ಯಥಾ
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ಯದಿ(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    ಅನ್ಯಥಾ
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
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
      ಯದಿ(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      ಅನ್ಯಥಾ
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
    }
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    sshc->quote_attrs = ರಿಕ್ತ;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  ಯದಿ(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಯದಿ(result) {
      ಯದಿ(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      ಅನ್ಯಥಾ
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
    }
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "mkdir ", 6)) {
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ಯದಿ(result) {
      ಯದಿ(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      ಅನ್ಯಥಾ
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
    }
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "rm ", 3)) {
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "statvfs ", 8)) {
    ಯದಿ(*cp)
      ಪ್ರತ್ಯಾಗಮ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_NEXT_QUOTE(ಸಂರಚನಾ Curl_easy *data,
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
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE_STAT(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssh_conn *sshc)
{
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

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  ಯದಿ(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  ಯದಿ(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  ಯದಿ(!strncmp(cmd, "chgrp", 5)) {
    ಧ್ರುವ ವರ್ಣ *p = sshc->quote_path1;
    curl_off_t gid;
    ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &gid, UINT_MAX)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, ರಿಕ್ತ);
      ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ಧ್ರುವ ವರ್ಣ *p = sshc->quote_path1;
    ಯದಿ(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, ರಿಕ್ತ);
      ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "chown", 5)) {
    ಧ್ರುವ ವರ್ಣ *p = sshc->quote_path1;
    curl_off_t uid;
    ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &uid, UINT_MAX)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, ರಿಕ್ತ);
      ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ಅನ್ಯಥಾ ಯದಿ(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    ಯದಿ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ಯದಿ SIZEOF_TIME_T > 4
    ಅನ್ಯಥಾ ಯದಿ(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    ಯದಿ(fail) {
      myssh_quote_error(data, sshc, ರಿಕ್ತ);
      ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
    }
    ಯದಿ(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    ಅನ್ಯಥಾ /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಶೂನ್ಯ conn_forget_socket(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಯದಿ(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    ಸಂರಚನಾ Curl_cfilter *cf = conn->cfilter[sockindex];
    ಯದಿ(cf)
      (ಶೂನ್ಯ)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, ರಿಕ್ತ);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_SESSION_DISCONNECT(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ಯದಿ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ರಿಕ್ತ;
  }

  ಯದಿ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ರಿಕ್ತ;
  }
  ಯದಿ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ರಿಕ್ತ;
  }

  ssh_disconnect(sshc->ssh_session);
  ಯದಿ(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_SSH_SCP_DOWNLOAD(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ssh_conn *sshc)
{
  uint64_t bytecount;
  ಪೂರ್ಣಾಙ್ಕ rc = ssh_scp_pull_request(sshc->scp_session);
  ಯದಿ(rc != SSH_SCP_REQUEST_NEWFILE) {
    ಧ್ರುವ ವರ್ಣ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ಸಂದೇಶ);
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  ಯದಿ(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_TRANS_INIT(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ssh_conn *sshc,
                               ಸಂರಚನಾ SSHPROTO *sshp)
{
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  ಯದಿ(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  ಯದಿ(data->state.upload) {
    ಯದಿ(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ಅನ್ಯಥಾ {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  ಯದಿ(!sshc->scp_session) {
    ಧ್ರುವ ವರ್ಣ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ಸಂದೇಶ);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಶೂನ್ಯ sshc_cleanup(ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->initialized) {
    ಯದಿ(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = ರಿಕ್ತ;
    }
    ಯದಿ(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = ರಿಕ್ತ;
    }
    ಯದಿ(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = ರಿಕ್ತ;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    ಯದಿ(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = ರಿಕ್ತ;
    }
    ಯದಿ(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = ರಿಕ್ತ;
    }
    ಯದಿ(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = ರಿಕ್ತ;
    }
    ಯದಿ(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = ರಿಕ್ತ;
    }
    ಯದಿ(sshc->privkey) {
      ssh_ಕೀಲಿ_free(sshc->privkey);
      sshc->privkey = ರಿಕ್ತ;
    }
    ಯದಿ(sshc->pubkey) {
      ssh_ಕೀಲಿ_free(sshc->pubkey);
      sshc->pubkey = ರಿಕ್ತ;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_ಕೀಲಿ);
    curlx_safefree(sshc->priv_ಕೀಲಿ);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE_SETSTAT(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  ಯದಿ(rc == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ rc;
  ಯದಿ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    ಪ್ರತ್ಯಾಗಮ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE_SYMLINK(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  ಯದಿ(rc == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ rc;
  ಯದಿ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    ಪ್ರತ್ಯಾಗಮ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE_MKDIR(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  ಯದಿ(rc == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ rc;
  ಯದಿ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    ಪ್ರತ್ಯಾಗಮ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE_RENAME(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  ಯದಿ(rc == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ rc;
  ಯದಿ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    ಪ್ರತ್ಯಾಗಮ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE_RMDIR(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  ಯದಿ(rc == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ rc;
  ಯದಿ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    ಪ್ರತ್ಯಾಗಮ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_QUOTE_UNLINK(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  ಯದಿ(rc == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ rc;
  ಯದಿ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    ಪ್ರತ್ಯಾಗಮ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_GETINFO(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_FILETIME(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ssh_conn *sshc,
                                  ಸಂರಚನಾ SSHPROTO *sshp)
{
  sftp_attributes attrs;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  ಯದಿ(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_TRANS_INIT(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssh_conn *sshc,
                                    ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  ಯದಿ(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ಅನ್ಯಥಾ {
    size_t path_len = strlen(sshp->path);

    ಯದಿ(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    ಅನ್ಯಥಾ
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_CREATE_DIRS_INIT(ಸಂರಚನಾ Curl_easy *data,
                                          ಸಂರಚನಾ ssh_conn *sshc,
                                          ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  ಯದಿ(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ಅನ್ಯಥಾ {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_CREATE_DIRS(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ ssh_conn *sshc,
                                     ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ಯದಿ(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SFTP_CREATE_DIRS_MKDIR(ಸಂರಚನಾ Curl_easy *data,
                                           ಸಂರಚನಾ ssh_conn *sshc,
                                           ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಪೂರ್ಣಾಙ್ಕ err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  ಯದಿ(rc == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ಯದಿ(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    ಯದಿ((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      ಪ್ರತ್ಯಾಗಮ myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  ಪ್ರತ್ಯಾಗಮ rc;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SCP_UPLOAD_INIT(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ssh_conn *sshc,
                                    ಸಂರಚನಾ SSHPROTO *sshp)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಯದಿ(!sshp)
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  ಯದಿ(rc != SSH_OK) {
    ಧ್ರುವ ವರ್ಣ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ಸಂದೇಶ);
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ತಳ್ಳು_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (ಪೂರ್ಣಾಙ್ಕ)data->set.new_file_perms);

  ಯದಿ(rc != SSH_OK) {
    ಧ್ರುವ ವರ್ಣ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ಸಂದೇಶ);
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SCP_DOWNLOAD_INIT(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ssh_conn *sshc)
{
  ಪೂರ್ಣಾಙ್ಕ rc = ssh_scp_init(sshc->scp_session);
  ಯದಿ(rc != SSH_OK) {
    ಧ್ರುವ ವರ್ಣ *err_ಸಂದೇಶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ಸಂದೇಶ);
    ಪ್ರತ್ಯಾಗಮ myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SCP_DONE(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  ಅನ್ಯಥಾ
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SCP_SEND_EOF(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->scp_session) {
    ಪೂರ್ಣಾಙ್ಕ rc = ssh_scp_close(sshc->scp_session);
    ಯದಿ(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      ಪ್ರತ್ಯಾಗಮ SSH_AGAIN;
    }
    ಯದಿ(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ myssh_in_SCP_CHANNEL_FREE(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ ssh_conn *sshc)
{
  ಯದಿ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ರಿಕ್ತ;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ಪ್ರತ್ಯಾಗಮ SSH_NO_ERROR;
}

ಸ್ಥಿರ CURLcode myssh_in_SESSION_FREE(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  memset(sshc, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
ಸ್ಥಿರ CURLcode myssh_statemachine(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ssh_conn *sshc,
                                   ಸಂರಚನಾ SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  ಕುರು {

    ವಿಕಲ್ಪ(sshc->state) {
    ಪಕ್ಷ SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    ಪಕ್ಷ SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      ಯದಿ(rc)
        ಭಙ್ಗ;
      FALLTHROUGH();
    ಪಕ್ಷ SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      ಯದಿ(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        ಭಙ್ಗ;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    ಪಕ್ಷ SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_AUTH_ಕೀಲಿ_INIT:
      rc = myssh_in_AUTH_ಕೀಲಿ_INIT(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_AUTH_ಕೀಲಿ:
      rc = myssh_in_AUTH_ಕೀಲಿ(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      ಯದಿ(rc)
        ಭಙ್ಗ;
      FALLTHROUGH();
    ಪಕ್ಷ SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    ಪಕ್ಷ SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    ಪಕ್ಷ SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      ಭಙ್ಗ;
    ಪಕ್ಷ SSH_QUIT:
    ಸ್ವಭಾವ:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      ಭಙ್ಗ;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } ಯಾವತ್((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  ಯದಿ(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  ಯದಿ(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (ಪೂರ್ಣಾಙ್ಕ)result, *block);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* called repeatedly until done from multi.c */
ಸ್ಥಿರ CURLcode myssh_multi_statemach(ಸಂರಚನಾ Curl_easy *data,
                                      bool *done)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ಸಂರಚನಾ SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  ಯದಿ(!sshc || !sshp)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode myssh_block_statemach(ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ ssh_conn *sshc,
                                      ಸಂರಚನಾ SSHPROTO *sshp,
                                      bool disconnect)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ಯಾವತ್((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t ಎಡ_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
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

    ಯದಿ(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (ಶೂನ್ಯ)SOCKET_READABLE(fd_read,
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
  curlx_free(sshp);
}

ಸ್ಥಿರ ಶೂನ್ಯ myssh_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ ssh_conn *sshc = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
ಸ್ಥಿರ CURLcode myssh_setup_connection(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ SSHPROTO *sshp;
  ಸಂರಚನಾ ssh_conn *sshc;

  sshc = curlx_calloc(1, ಪರಿಮಾಣ(*sshc));
  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  ಯದಿ(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, ಪರಿಮಾಣ(*sshp));
  ಯದಿ(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಪ್ರತ್ಯಾಗಮ Curl_ssh_setup_pkey(data, sshc);
}

ಸ್ಥಿರ Curl_recv scp_recv, sftp_recv;
ಸ್ಥಿರ Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ಸ್ಥಿರ CURLcode myssh_connect(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ಸಂರಚನಾ SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!sshc || !ssh)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  ಯದಿ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ಅನ್ಯಥಾ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  ಯದಿ(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  ಯದಿ(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, ರಿಕ್ತ);
  ಯದಿ(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  ಯದಿ(rc != SSH_OK) {
    failf(data, "Could not set socket");
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  ಯದಿ(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    ಯದಿ(rc != SSH_OK) {
      failf(data, "Could not set user");
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    }
  }

  ಯದಿ(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    ಯದಿ(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    ಯದಿ(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    }
  }

  ಯದಿ(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    ಯದಿ(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    }
  }

  ಯದಿ(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    ಯದಿ(rc != SSH_OK) {
      failf(data, "Could not set compression");
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = ರಿಕ್ತ;
  sshc->pubkey = ರಿಕ್ತ;

  ಯದಿ(sshc->pub_ಕೀಲಿ) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_ಕೀಲಿ, &sshc->pubkey);
    ಯದಿ(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* called from multi.c while DOing */
ಸ್ಥಿರ CURLcode scp_doing(ಸಂರಚನಾ Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  ಯದಿ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
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
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

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
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ಸ್ಥಿರ CURLcode myssh_done(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  ಯದಿ(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
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

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಯದಿ(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  ಪ್ರತ್ಯಾಗಮ myssh_done(data, sshc, status);
}

ಸ್ಥಿರ CURLcode scp_send(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                         ಧ್ರುವ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (ಶೂನ್ಯ)sockindex; /* we only support SCP on the fixed known primary socket */
  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#ಯದಿ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  ಯದಿ(rc == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  ಅನ್ಯಥಾ
#endif
  ಯದಿ(rc != SSH_OK)
    ಪ್ರತ್ಯಾಗಮ CURLE_SSH;

  *pnwritten = len;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode scp_recv(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                         ವರ್ಣ *mem, size_t len, size_t *pnread)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ಪೂರ್ಣಾಙ್ಕ nread;

  (ಶೂನ್ಯ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  ಯದಿ(nread == SSH_ERROR)
    ಪ್ರತ್ಯಾಗಮ CURLE_SSH;
#ಯದಿ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  ಯದಿ(nread == SSH_AGAIN)
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
  result = myssh_multi_statemach(data, dophase_done);

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
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  ಯದಿ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ಸ್ಥಿರ CURLcode sftp_disconnect(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn,
                                bool dead_connection)
{
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ಸಂರಚನಾ SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  ಯದಿ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
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
  ಪ್ರತ್ಯಾಗಮ myssh_done(data, sshc, status);
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

#ಯದಿ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  ವಿಕಲ್ಪ(sshc->sftp_send_state) {
  ಪಕ್ಷ 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    ಯದಿ(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  ಪಕ್ಷ 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    ಯದಿ(nwrite == SSH_AGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    ಅನ್ಯಥಾ ಯದಿ(nwrite < 0)
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಸ್ವಭಾವ:
    /* we never reach here */
    ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
  }
#ಅನ್ಯಥಾ
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  ಯದಿ(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#ಯದಿ 0 /* not returned by libssh on write */
  ಯದಿ(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  ಅನ್ಯಥಾ
#endif
  ಯದಿ(nwrite < 0)
    ಪ್ರತ್ಯಾಗಮ CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#endif
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

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  ವಿಕಲ್ಪ(sshc->sftp_recv_state) {
  ಪಕ್ಷ 0:
#ಯದಿ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    ಯದಿ(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
    }
#ಅನ್ಯಥಾ
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    ಯದಿ(sshc->sftp_file_index < 0)
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  ಪಕ್ಷ 1:
    sshc->sftp_recv_state = 1;

#ಯದಿ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#ಅನ್ಯಥಾ
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    ಯದಿ(nread == SSH_AGAIN)
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    ಅನ್ಯಥಾ ಯದಿ(nread < 0)
      ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಸ್ವಭಾವ:
    /* we never reach here */
    ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
  }
}

ಸ್ಥಿರ CURLcode myssh_do_it(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  ಯದಿ(!sshc)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
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
  ಯದಿ(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssh_cleanup(ಶೂನ್ಯ)
{
  (ಶೂನ್ಯ)ssh_finalize();
}

ಶೂನ್ಯ Curl_ssh_version(ವರ್ಣ *buffer, size_t buflen)
{
  (ಶೂನ್ಯ)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_scp = {
  myssh_setup_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_done,                             /* done */
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
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_sftp = {
  myssh_setup_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_done,                            /* done */
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

#endif /* USE_LIBSSH */
