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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_LIBSSH

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "vssh/ssh.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "parsedate.h"          /* for the week day and month names */
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "vssh/vssh.h"
#अन्तर्भाव "curlx/base64.h" /* for curlx_base64_encode() */

#यदि_परिभाषितम् HAVE_UNISTD_H
#अन्तर्भाव <unistd.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_FCNTL_H
#अन्तर्भाव <fcntl.h>
#यदि_अन्तः

/* A recent macro provided by libssh. Or make our own. */
#यदि_अपरिभाषितम् SSH_STRING_FREE_CHAR
#परिभाषय SSH_STRING_FREE_CHAR(x) \
  कुरु {                          \
    यदि(x) {                     \
      ssh_string_free_char(x);  \
      x = रिक्त;                 \
    }                           \
  } यावत्(0)
#यदि_अन्तः

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#यदि_अपरिभाषितम् SSH_S_IFMT
#परिभाषय SSH_S_IFMT   00170000
#यदि_अन्तः
#यदि_अपरिभाषितम् SSH_S_IFLNK
#परिभाषय SSH_S_IFLNK  0120000
#यदि_अन्तः

स्थिर CURLcode sftp_error_गन्तव्यम्_CURLE(पूर्णाङ्क err)
{
  विकल्प(err) {
  पक्ष SSH_FX_OK:
    प्रत्यागम CURLE_OK;

  पक्ष SSH_FX_NO_SUCH_FILE:
  पक्ष SSH_FX_NO_SUCH_मार्गः:
    प्रत्यागम CURLE_REMOTE_FILE_NOT_प्राप्तम्;

  पक्ष SSH_FX_PERMISSION_DENIED:
  पक्ष SSH_FX_WRITE_PROTECT:
    प्रत्यागम CURLE_REMOTE_ACCESS_DENIED;

  पक्ष SSH_FX_FILE_ALREADY_EXISTS:
    प्रत्यागम CURLE_REMOTE_FILE_EXISTS;

  स्वभाव:
    भङ्ग;
  }

  प्रत्यागम CURLE_SSH;
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
स्थिर पूर्णाङ्क myssh_is_ज्ञातम्(संरचना Curl_easy *दत्त, संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  ssh_कुञ्चिका pubkey;
  अचिह्नित वर्ण *hash_sha256 = रिक्त;
  size_t hlen_sha256;
  अचिह्नित वर्ण *hash_md5 = रिक्त;
  size_t hlen_md5;
  वर्ण *प्राप्तम्_base64 = रिक्त;
  वर्ण *ज्ञातम्_base64 = रिक्त;
  पूर्णाङ्क vstate;
  गणना curl_khmatch keymatch;
  संरचना curl_khkey foundkey;
  संरचना curl_khkey *knownkeyp = रिक्त;
  curl_sshkeycallback func = दत्त->समुच्चयः.ssh_keyfunc;
  संरचना ssh_knownhosts_प्रविष्टिः *knownhostsentry = रिक्त;
  संरचना curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  यदि(rc != SSH_OK)
    प्रत्यागम rc;

  यदि(CURL_EASY_STR(दत्त, STRING_SSH_HOST_लौकिकम्_कुञ्चिका_SHA256)) {
    ध्रुव वर्ण *pubkey_sha256 =
      CURL_EASY_STR(दत्त, STRING_SSH_HOST_लौकिकम्_कुञ्चिका_SHA256);
    वर्ण *fingerprint_b64 = रिक्त;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    यदि(rc != SSH_OK || hlen_sha256 != 32) {
      failf(दत्त, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      गच्छ cleanup;
    }

    यदि(curlx_base64_encode((ध्रुव uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      गच्छ cleanup;
    }

    infof(दत्त, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    यावत्((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    यावत्((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    यदि((pub_pos != b64_pos) ||
       वाक्ये_तुलय(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(दत्त,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      गच्छ cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    गच्छ cleanup;
  }

  यदि(CURL_EASY_STR(दत्त, STRING_SSH_HOST_लौकिकम्_कुञ्चिका_MD5)) {
    ध्रुव वर्ण *pubkey_md5 =
      CURL_EASY_STR(दत्त, STRING_SSH_HOST_लौकिकम्_कुञ्चिका_MD5);
    वर्ण md5buffer[33];
    पूर्णाङ्क i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    यदि(rc != SSH_OK || hlen_md5 != 16) {
      failf(दत्त,
            "Denied establishing ssh session: MD5 fingerprint not available");
      गच्छ cleanup;
    }

    प्रति(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(दत्त, "SSH MD5 fingerprint: %s", md5buffer);

    यदि(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(दत्त,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      गच्छ cleanup;
    }

    rc = SSH_OK;
    गच्छ cleanup;
  }

  यदि(CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_ज्ञातम्_hosts_प्रविष्टिः(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    यदि(knownhostsentry) {
      यदि(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &ज्ञातम्_base64);
        यदि(rc != SSH_OK) {
          गच्छ cleanup;
        }
        knownkey.कुञ्चिका = ज्ञातम्_base64;
        knownkey.len = वाक्यदैर्घ्यम्(ज्ञातम्_base64);

        विकल्प(ssh_कुञ्चिका_type(knownhostsentry->publickey)) {
        पक्ष SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          भङ्ग;
        पक्ष SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          भङ्ग;
        पक्ष SSH_KEYTYPE_ECDSA:
        पक्ष SSH_KEYTYPE_ECDSA_P256:
        पक्ष SSH_KEYTYPE_ECDSA_P384:
        पक्ष SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          भङ्ग;
        पक्ष SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          भङ्ग;
        पक्ष SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          भङ्ग;
        स्वभाव:
          rc = SSH_ERROR;
          गच्छ cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    विकल्प(vstate) {
    पक्ष SSH_ज्ञातम्_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      भङ्ग;
    पक्ष SSH_ज्ञातम्_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      भङ्ग;
    पक्ष SSH_ज्ञातम्_HOSTS_NOT_प्राप्तम्:
    पक्ष SSH_ज्ञातम्_HOSTS_UNKNOWN:
    पक्ष SSH_ज्ञातम्_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      भङ्ग;
    स्वभाव:
      keymatch = CURLKHMATCH_MISMATCH;
      भङ्ग;
    }

    यदि(func) { /* use callback to determine action */
      संरचना Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &प्राप्तम्_base64);
      यदि(rc != SSH_OK)
        गच्छ cleanup;

      foundkey.कुञ्चिका = प्राप्तम्_base64;
      foundkey.len = वाक्यदैर्घ्यम्(प्राप्तम्_base64);

      विकल्प(ssh_कुञ्चिका_type(pubkey)) {
      पक्ष SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        भङ्ग;
      पक्ष SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        भङ्ग;
      पक्ष SSH_KEYTYPE_ECDSA:
      पक्ष SSH_KEYTYPE_ECDSA_P256:
      पक्ष SSH_KEYTYPE_ECDSA_P384:
      पक्ष SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        भङ्ग;
      पक्ष SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        भङ्ग;
      पक्ष SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        भङ्ग;
      स्वभाव:
        rc = SSH_ERROR;
        गच्छ cleanup;
      }

      CURL_CBAPI_START(&guard, दत्त, easy_ssh_keyfunc);
      rc = func(दत्त, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, दत्त->समुच्चयः.ssh_keyfunc_userp);
      CURL_CBAPI_अन्तः(&guard);

      विकल्प(rc) {
      पक्ष CURLKHSTAT_FINE_सङ्कलय_गन्तव्यम्_FILE:
        rc = ssh_session_update_ज्ञातम्_hosts(sshc->ssh_session);
        यदि(rc != SSH_OK) {
          गच्छ cleanup;
        }
        भङ्ग;
      पक्ष CURLKHSTAT_FINE:
        भङ्ग;
      स्वभाव: /* REJECT/DEFER */
        rc = SSH_ERROR;
        गच्छ cleanup;
      }
    }
    अन्यथा {
      यदि(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        गच्छ cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  यदि(प्राप्तम्_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    मुक्त(प्राप्तम्_base64); /* allocated by libssh, deallocate with system free */
  }
  यदि(ज्ञातम्_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    मुक्त(ज्ञातम्_base64); /* allocated by libssh, deallocate with system free */
  }
  यदि(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  यदि(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_कुञ्चिका_free(pubkey);
  यदि(knownhostsentry) {
    ssh_knownhosts_प्रविष्टिः_free(knownhostsentry);
  }
  प्रत्यागम rc;
}

स्थिर पूर्णाङ्क myssh_गन्तव्यम्_ERROR(संरचना Curl_easy *दत्त,
                          संरचना ssh_conn *sshc,
                          CURLcode परिणाम)
{
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = परिणाम;
  प्रत्यागम SSH_ERROR;
}

स्थिर पूर्णाङ्क myssh_गन्तव्यम्_SFTP_CLOSE(संरचना Curl_easy *दत्त,
                               संरचना ssh_conn *sshc)
{
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_गन्तव्यम्_CURLE(sftp_get_error(sshc->sftp_session));
  प्रत्यागम SSH_ERROR;
}

स्थिर पूर्णाङ्क myssh_गन्तव्यम्_PASSWD_AUTH(संरचना Curl_easy *दत्त,
                                संरचना ssh_conn *sshc)
{
  यदि(sshc->auth_methods & SSH_AUTH_विधानम्_PASSWORD) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PASS_INIT);
    प्रत्यागम SSH_OK;
  }
  प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_LOGIN_DENIED);
}

स्थिर पूर्णाङ्क myssh_गन्तव्यम्_कुञ्चिका_AUTH(संरचना Curl_easy *दत्त,
                             संरचना ssh_conn *sshc)
{
  यदि(sshc->auth_methods & SSH_AUTH_विधानम्_INTERACTIVE) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_कुञ्चिका_INIT);
    प्रत्यागम SSH_OK;
  }
  प्रत्यागम myssh_गन्तव्यम्_PASSWD_AUTH(दत्त, sshc);
}

स्थिर पूर्णाङ्क myssh_गन्तव्यम्_GSSAPI_AUTH(संरचना Curl_easy *दत्त,
                                संरचना ssh_conn *sshc)
{
  यदि(sshc->auth_methods & SSH_AUTH_विधानम्_GSSAPI_MIC) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_GSSAPI);
    प्रत्यागम SSH_OK;
  }
  प्रत्यागम myssh_गन्तव्यम्_कुञ्चिका_AUTH(दत्त, sshc);
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_READDIR_INIT(संरचना Curl_easy *दत्त,
                                      संरचना ssh_conn *sshc,
                                      संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);
  Curl_pgrs_समुच्चयः_Download_आकार(दत्त, -1);
  यदि(दत्त->req.no_body) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
    प्रत्यागम SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->मार्गः);
  यदि(!sshc->sftp_dir) {
    failf(दत्त, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    प्रत्यागम myssh_गन्तव्यम्_SFTP_CLOSE(दत्त, sshc);
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_READDIR(संरचना Curl_easy *दत्त,
                                 संरचना ssh_conn *sshc,
                                 संरचना SSHPROTO *sshp)
{
  CURLcode परिणाम = CURLE_OK;

  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  यदि(sshc->readdir_attrs)
    sftp_गुणाः_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  यदि(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = वाक्यदैर्घ्यम्(sshc->readdir_filename);

    यदि(दत्त->समुच्चयः.list_only) {
      वर्ण *tmp_पङ्क्तिः;

      tmp_पङ्क्तिः = curl_maprintf("%s\n", sshc->readdir_filename);
      यदि(!tmp_पङ्क्तिः) {
        myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        प्रत्यागम SSH_ERROR;
      }
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                                 tmp_पङ्क्तिः, sshc->readdir_len + 1);
      curlx_free(tmp_पङ्क्तिः);

      यदि(परिणाम) {
        myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
        sshc->actualcode = परिणाम;
        प्रत्यागम SSH_NO_ERROR;
      }
    }
    अन्यथा {
      यदि(curlx_dyn_सङ्कलय(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
        प्रत्यागम SSH_ERROR;
      }

      यदि((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_link_मार्गः = curl_maprintf("%s%s", sshp->मार्गः,
                                               sshc->readdir_filename);

        यदि(!sshc->readdir_link_मार्गः) {
          myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
          प्रत्यागम SSH_ERROR;
        }

        myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_LINK);
        प्रत्यागम SSH_NO_ERROR;
      }
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_BOTTOM);
      प्रत्यागम SSH_NO_ERROR;
    }
  }
  अन्यथा यदि(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_समाप्तम्);
  }
  अन्यथा {
    failf(दत्त, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    प्रत्यागम myssh_गन्तव्यम्_SFTP_CLOSE(दत्त, sshc);
  }
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_READDIR_LINK(संरचना Curl_easy *दत्त,
                                      संरचना ssh_conn *sshc)
{
  यदि(sshc->readdir_link_attrs)
    sftp_गुणाः_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_link_मार्गः);
  यदि(!sshc->readdir_link_attrs) {
    failf(दत्त, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    प्रत्यागम myssh_गन्तव्यम्_SFTP_CLOSE(दत्त, sshc);
  }

  यदि(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_link_मार्गः);
    यदि(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    अन्यथा
      sshc->readdir_len = वाक्यदैर्घ्यम्(sshc->readdir_tmp);
    sshc->readdir_longentry = रिक्त;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  अन्यथा {
    sshc->readdir_len = वाक्यदैर्घ्यम्(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_link_मार्गः);

  यदि(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    प्रत्यागम SSH_ERROR;
  }

  sftp_गुणाः_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = रिक्त;
  sshc->readdir_filename = रिक्त;
  sshc->readdir_longentry = रिक्त;

  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_BOTTOM);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_READDIR_BOTTOM(संरचना Curl_easy *दत्त,
                                        संरचना ssh_conn *sshc)
{
  CURLcode परिणाम;

  यदि(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  अन्यथा
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = रिक्त;

  यदि(परिणाम) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
    sshc->actualcode = परिणाम;
  }
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_READDIR_समाप्तम्(संरचना Curl_easy *दत्त,
                                      संरचना ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = रिक्त;

  /* no data to transfer */
  Curl_xfer_सज्जता_nop(दत्त);
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर शून्य myssh_quote_error(संरचना Curl_easy *दत्त, संरचना ssh_conn *sshc,
                              ध्रुव वर्ण *cmd)
{
  यदि(cmd)
    failf(दत्त, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE_STATVFS(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  यदि(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(दत्त, sshc, "statvfs");
    प्रत्यागम SSH_OK;
  }
  अन्यथा यदि(statvfs) {
#यदि_परिभाषितम् _MSC_VER
#परिभाषय CURL_LIBSSH_VFS_आकार_आवरणम् "I64u"
#नोचेद्यदि defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#परिभाषय CURL_LIBSSH_VFS_आकार_आवरणम् "llu"
#अन्यथा
#परिभाषय CURL_LIBSSH_VFS_आकार_आवरणम् PRIu64
#यदि_अन्तः
    CURLcode परिणाम = CURLE_OK;
    वर्ण *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_files: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_आकार_आवरणम् "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_खण्डानि, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    यदि(!tmp)
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    यदि(!परिणाम) {
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, tmp, वाक्यदैर्घ्यम्(tmp));
      curlx_free(tmp);
    }
    यदि(परिणाम) {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
    }
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम SSH_OK;
}

स्थिर पूर्णाङ्क myssh_auth_interactive(संरचना connectdata *conn,
                                  संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  पूर्णाङ्क nprompts;

restart:
  विकल्प(sshc->kbd_state) {
  पक्ष 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, रिक्त, रिक्त);
    यदि(rc == SSH_AUTH_AGAIN)
      प्रत्यागम SSH_AGAIN;

    यदि(rc != SSH_AUTH_INFO)
      प्रत्यागम SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    यदि(nprompts != 1)
      प्रत्यागम SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    यदि(rc < 0)
      प्रत्यागम SSH_ERROR;

    FALLTHROUGH();
  पक्ष 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, रिक्त, रिक्त);
    यदि(rc == SSH_AUTH_AGAIN)
      प्रत्यागम SSH_AGAIN;
    अन्यथा यदि(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    अन्यथा यदि(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      यदि(nprompts)
        प्रत्यागम SSH_ERROR;

      sshc->kbd_state = 2;
      गच्छ restart;
    }
    अन्यथा
      rc = SSH_ERROR;
    भङ्ग;
  पक्ष 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, रिक्त, रिक्त);
    यदि(rc == SSH_AUTH_AGAIN)
      प्रत्यागम SSH_AGAIN;
    अन्यथा यदि(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    अन्यथा
      rc = SSH_ERROR;

    भङ्ग;
  स्वभाव:
    प्रत्यागम SSH_ERROR;
  }

  sshc->kbd_state = 0;
  प्रत्यागम rc;
}

स्थिर शून्य myssh_state_init(संरचना Curl_easy *दत्त,
                             संरचना ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#यदि 0
  ssh_समुच्चयः_log_level(SSH_LOG_PROTOCOL);
#यदि_अन्तः

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_समुच्चयः_blocking(sshc->ssh_session, 0);

  myssh_गन्तव्यम्(दत्त, sshc, SSH_S_STARTUP);
}

स्थिर शून्य myssh_block2waitfor(संरचना connectdata *conn,
                                संरचना ssh_conn *sshc,
                                bool खण्डम्)
{
  (शून्य)conn;
  यदि(खण्डम्) {
    पूर्णाङ्क dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  अन्यथा
    sshc->waitfor = 0;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_S_STARTUP(संरचना Curl_easy *दत्त,
                              संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = दत्त->conn;
  पूर्णाङ्क rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  यदि(rc == SSH_AGAIN) {
    CURL_TRC_SSH(दत्त, "connect -> EAGAIN");
  }
  अन्यथा यदि(rc != SSH_OK) {
    failf(दत्त, "Failure establishing ssh session");
    rc = myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);
  }
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_HOSTKEY);

  प्रत्यागम rc;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_AUTHLIST(संरचना Curl_easy *दत्त,
                             संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, रिक्त);
  यदि(rc == SSH_AUTH_AGAIN)
    प्रत्यागम SSH_AGAIN;

  यदि(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(दत्त, "Authenticated with none");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
    प्रत्यागम rc;
  }
  अन्यथा यदि(rc == SSH_AUTH_ERROR) {
    rc = myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_LOGIN_DENIED);
    प्रत्यागम rc;
  }

  sshc->auth_methods =
    (अचिह्नित पूर्णाङ्क)ssh_userauth_list(sshc->ssh_session, रिक्त);
  यदि(sshc->auth_methods)
    infof(दत्त, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_विधानम्_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_विधानम्_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_विधानम्_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_विधानम्_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  यदि((sshc->auth_methods & SSH_AUTH_विधानम्_PUBLICKEY) &&
     ((sshc->priv_कुञ्चिका && sshc->priv_कुञ्चिका[0]) ||
      (दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PKEY_INIT);
    infof(दत्त, "Authentication using SSH public key file");
  }
  अन्यथा यदि(sshc->auth_methods & SSH_AUTH_विधानम्_GSSAPI_MIC) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_GSSAPI);
  }
  अन्यथा यदि(sshc->auth_methods & SSH_AUTH_विधानम्_INTERACTIVE) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_कुञ्चिका_INIT);
  }
  अन्यथा यदि(sshc->auth_methods & SSH_AUTH_विधानम्_PASSWORD) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PASS_INIT);
  }
  अन्यथा {                  /* unsupported authentication method */
    rc = myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_LOGIN_DENIED);
  }
  प्रत्यागम rc;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_AUTH_PKEY_INIT(संरचना Curl_easy *दत्त,
                                   संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  यदि(!(दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_गन्तव्यम्_GSSAPI_AUTH(दत्त, sshc);
    प्रत्यागम rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  यदि(sshc->priv_कुञ्चिका && sshc->priv_कुञ्चिका[0]) {
    यदि(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, रिक्त, sshc->pubkey);
      यदि(rc == SSH_AUTH_AGAIN)
        प्रत्यागम SSH_AGAIN;

      यदि(rc != SSH_OK) {
        rc = myssh_गन्तव्यम्_GSSAPI_AUTH(दत्त, sshc);
        प्रत्यागम rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_कुञ्चिका,
                                     sshc->passphrase, रिक्त,
                                     रिक्त, &sshc->privkey);
    यदि(rc != SSH_OK) {
      failf(दत्त, "Could not load private key file %s", sshc->priv_कुञ्चिका);
      rc = myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_LOGIN_DENIED);
      प्रत्यागम rc;
    }

    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PKEY);
  }
  अन्यथा {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, रिक्त,
                                     sshc->passphrase);
    यदि(rc == SSH_AUTH_AGAIN)
      प्रत्यागम SSH_AGAIN;

    यदि(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(दत्त, "Completed public key authentication");
      myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
      प्रत्यागम rc;
    }

    rc = myssh_गन्तव्यम्_GSSAPI_AUTH(दत्त, sshc);
  }
  प्रत्यागम rc;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_AUTH_PKEY(संरचना Curl_easy *दत्त,
                              संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = ssh_userauth_publickey(sshc->ssh_session, रिक्त, sshc->privkey);
  यदि(rc == SSH_AUTH_AGAIN)
    प्रत्यागम SSH_AGAIN;
  अन्यथा यदि(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(दत्त, "Completed public key authentication");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
    प्रत्यागम SSH_OK;
  }
  अन्यथा {
    infof(दत्त, "Failed public key authentication (rc: %d)", rc);
    प्रत्यागम myssh_गन्तव्यम्_GSSAPI_AUTH(दत्त, sshc);
  }
}

स्थिर पूर्णाङ्क myssh_अन्तर्_AUTH_GSSAPI(संरचना Curl_easy *दत्त,
                                संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  यदि(!(दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    प्रत्यागम myssh_गन्तव्यम्_कुञ्चिका_AUTH(दत्त, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  यदि(rc == SSH_AUTH_AGAIN)
    प्रत्यागम SSH_AGAIN;

  यदि(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(दत्त, "Completed gssapi authentication");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
    प्रत्यागम SSH_OK;
  }

  प्रत्यागम myssh_गन्तव्यम्_कुञ्चिका_AUTH(दत्त, sshc);
}

स्थिर पूर्णाङ्क myssh_अन्तर्_AUTH_कुञ्चिका_INIT(संरचना Curl_easy *दत्त,
                                  संरचना ssh_conn *sshc)
{
  यदि(दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_कुञ्चिका);
    प्रत्यागम SSH_NO_ERROR;
  }
  प्रत्यागम myssh_गन्तव्यम्_PASSWD_AUTH(दत्त, sshc);
}

स्थिर पूर्णाङ्क myssh_अन्तर्_AUTH_कुञ्चिका(संरचना Curl_easy *दत्त,
                             संरचना ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  पूर्णाङ्क rc = myssh_auth_interactive(दत्त->conn, sshc);
  यदि(rc == SSH_AGAIN)
    प्रत्यागम rc;
  अन्यथा यदि(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(दत्त, "completed keyboard interactive authentication");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
    प्रत्यागम SSH_NO_ERROR;
  }
  अन्यथा
    प्रत्यागम myssh_गन्तव्यम्_PASSWD_AUTH(दत्त, sshc);
}

स्थिर पूर्णाङ्क myssh_अन्तर्_AUTH_PASS_INIT(संरचना Curl_easy *दत्त,
                                   संरचना ssh_conn *sshc)
{
  यदि(!(दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_LOGIN_DENIED);
  myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PASS);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_AUTH_PASS(संरचना Curl_easy *दत्त,
                              संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = ssh_userauth_password(sshc->ssh_session, रिक्त,
                                 Curl_creds_passwd(दत्त->conn->creds));
  यदि(rc == SSH_AUTH_AGAIN)
    प्रत्यागम SSH_AGAIN;
  अन्यथा यदि(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(दत्त, "Completed password authentication");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
    प्रत्यागम SSH_NO_ERROR;
  }
  प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_LOGIN_DENIED);
}

स्थिर पूर्णाङ्क myssh_अन्तर्_AUTH_समाप्तम्(संरचना Curl_easy *दत्त,
                              संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = दत्त->conn;
  यदि(!sshc->authed) {
    failf(दत्त, "Authentication failure");
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(दत्त, "Authentication complete");
  Curl_pgrsTime(दत्त, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  यदि(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_INIT);
    प्रत्यागम SSH_NO_ERROR;
  }
  infof(दत्त, "SSH CONNECT phase done");
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_UPLOAD_INIT(संरचना Curl_easy *दत्त,
                                संरचना ssh_conn *sshc,
                                संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क flags;
  पूर्णाङ्क rc = 0;

  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);

  यदि(दत्त->state.resume_from) {
    sftp_गुणाः attrs;

    यदि(दत्त->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->मार्गः);
      यदि(attrs) {
        curl_off_t आकार = attrs->आकार;
        sftp_गुणाः_free(attrs);
        यदि(आकार < 0) {
          failf(दत्त, "Bad file size (%" FMT_OFF_T ")", आकार);
          rc = myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          प्रत्यागम rc;
        }
        दत्त->state.resume_from = आकार;
      }
      अन्यथा {
        दत्त->state.resume_from = 0;
      }
    }
  }

  यदि(दत्त->समुच्चयः.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  अन्यथा यदि(दत्त->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  अन्यथा
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  यदि(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->मार्गः,
              flags, (mode_t)दत्त->समुच्चयः.new_file_perms);
  यदि(!sshc->sftp_file) {
    पूर्णाङ्क err = sftp_get_error(sshc->sftp_session);

    यदि((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_मार्गः) &&
       दत्त->समुच्चयः.ftp_create_missing_dirs &&
       (वाक्यदैर्घ्यम्(sshp->मार्गः) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      प्रत्यागम rc;
    }
    अन्यथा {
      rc = myssh_गन्तव्यम्_SFTP_CLOSE(दत्त, sshc);
      प्रत्यागम rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  यदि(दत्त->state.resume_from > 0 && !दत्त->समुच्चयः.remote_append) {
    पूर्णाङ्क seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    यदि(दत्त->समुच्चयः.seek_func) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, दत्त, easy_seek_func);
      seekerr = दत्त->समुच्चयः.seek_func(दत्त->समुच्चयः.seek_client,
                                    दत्त->state.resume_from, SEEK_SET);
      CURL_CBAPI_अन्तः(&guard);
    }

    यदि(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      यदि(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(दत्त, "Could not seek stream");
        rc = myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FTP_COULDNT_USE_REST);
        प्रत्यागम rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      कुरु {
        वर्ण scratch[4 * 1024];
        size_t readthisamountnow =
          (दत्त->state.resume_from - passed >
           (curl_off_t)परिमाण(scratch)) ?
          परिमाण(scratch) : curlx_sotouz(दत्त->state.resume_from - passed);

        size_t actuallyread =
          दत्त->state.fread_func(scratch, 1,
                                 readthisamountnow, दत्त->state.अन्तर्);

        passed += actuallyread;
        यदि((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(दत्त, "Failed to read data");
          rc = myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FTP_COULDNT_USE_REST);
          प्रत्यागम rc;
        }
      } यावत्(passed < दत्त->state.resume_from);
    }

    /* now, decrease the size of the read */
    यदि(दत्त->state.infilesize > 0) {
      यदि(दत्त->state.resume_from > दत्त->state.infilesize) {
        failf(दत्त, "Resume point beyond size");
        प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_BAD_FUNCTION_तर्कः);
      }
      दत्त->state.infilesize -= दत्त->state.resume_from;
      दत्त->req.आकार = दत्त->state.infilesize;
      Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, दत्त->state.resume_from);
    यदि(rc) {
      rc = myssh_गन्तव्यम्_SFTP_CLOSE(दत्त, sshc);
      प्रत्यागम rc;
    }
  }
  यदि(दत्त->state.infilesize > 0) {
    दत्त->req.आकार = दत्त->state.infilesize;
    Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);
  }
  /* upload data */
  Curl_xfer_सज्जता_send(दत्त, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  दत्त->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(दत्त);
#यदि LIBSSH_संस्करणम्_INT > SSH_संस्करणम्_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#यदि_अन्तः
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम rc;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_DOWNLOAD_INIT(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc,
                                       संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  यदि(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->मार्गः,
                              O_RDONLY, (mode_t)दत्त->समुच्चयः.new_file_perms);
  यदि(!sshc->sftp_file) {
    failf(दत्त, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    प्रत्यागम myssh_गन्तव्यम्_SFTP_CLOSE(दत्त, sshc);
  }
  sftp_file_समुच्चयः_nonblocking(sshc->sftp_file);
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_DOWNLOAD_STAT);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_DOWNLOAD_STAT(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc)
{
  curl_off_t आकार;
  पूर्णाङ्क rc = 0;
  sftp_गुणाः attrs = sftp_fstat(sshc->sftp_file);
  यदि(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_आकार) ||
     (attrs->आकार == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    दत्त->req.आकार = -1;
    दत्त->req.maxdownload = -1;
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, -1);
    आकार = 0;
    यदि(attrs)
      sftp_गुणाः_free(attrs);
  }
  अन्यथा {
    आकार = attrs->आकार;

    sftp_गुणाः_free(attrs);

    यदि(आकार < 0) {
      failf(दत्त, "Bad file size (%" FMT_OFF_T ")", आकार);
      प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    यदि(दत्त->state.use_range) {
      curl_off_t from;
      CURLcode परिणाम = Curl_ssh_range(दत्त, दत्त->state.range, आकार,
                                       &from, &आकार);
      यदि(परिणाम)
        प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, परिणाम);

      rc = sftp_seek64(sshc->sftp_file, from);
      यदि(rc)
        प्रत्यागम myssh_गन्तव्यम्_SFTP_CLOSE(दत्त, sshc);
    }
    दत्त->req.आकार = आकार;
    दत्त->req.maxdownload = आकार;
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, आकार);
  }

  /* We can resume if we can seek to the resume position */
  यदि(दत्त->state.resume_from) {
    यदि(दत्त->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      यदि(आकार < -दत्त->state.resume_from) {
        failf(दत्त, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", दत्त->state.resume_from, आकार);
        प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      दत्त->state.resume_from += आकार;
    }
    अन्यथा {
      यदि(आकार < दत्त->state.resume_from) {
        failf(दत्त, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              दत्त->state.resume_from, आकार);
        प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    दत्त->req.आकार = आकार - दत्त->state.resume_from;
    दत्त->req.maxdownload = आकार - दत्त->state.resume_from;
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, आकार - दत्त->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, दत्त->state.resume_from);
    यदि(rc)
      प्रत्यागम myssh_गन्तव्यम्_SFTP_CLOSE(दत्त, sshc);
  }

  /* Setup the actual download */
  यदि(दत्त->req.आकार == 0) {
    /* no data to transfer */
    Curl_xfer_सज्जता_nop(दत्त);
    infof(दत्त, "File already completely downloaded");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
    प्रत्यागम rc;
  }
  Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, दत्त->req.आकार);

  /* not set by Curl_xfer_setup to preserve io_flags */
  दत्त->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);

  प्रत्यागम rc;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_CLOSE(संरचना Curl_easy *दत्त,
                               संरचना ssh_conn *sshc,
                               संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);
  यदि(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = रिक्त;
  }
  curlx_safefree(sshp->मार्गः);

  CURL_TRC_SSH(दत्त, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  यदि(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_गन्तव्यम्(दत्त, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  }
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_SHUTDOWN(संरचना Curl_easy *दत्त,
                                  संरचना ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_समुच्चयः_blocking(sshc->ssh_session, 0);
#यदि LIBSSH_संस्करणम्_INT > SSH_संस्करणम्_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#यदि_अन्तः

  यदि(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = रिक्त;
  }

  यदि(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = रिक्त;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_DISCONNECT);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_INIT(संरचना Curl_easy *दत्त,
                              संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  ssh_समुच्चयः_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  यदि(!sshc->sftp_session) {
    failf(दत्त, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  यदि(rc != SSH_OK) {
    failf(दत्त, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, sftp_error_गन्तव्यम्_CURLE(SSH_FX_FAILURE));
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_REALPATH);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_REALPATH(संरचना Curl_easy *दत्त,
                                  संरचना ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_मार्गः(sshc->sftp_session, ".");
  यदि(!sshc->homedir)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(दत्त->state.most_recent_ftp_entrypath);
  दत्त->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  यदि(!दत्त->state.most_recent_ftp_entrypath)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(दत्त, "CONNECT phase done");
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE_INIT(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp)
{
  CURLcode परिणाम;

  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);

  परिणाम = Curl_getworkingpath(दत्त, sshc->homedir, &sshp->मार्गः);
  यदि(परिणाम) {
    sshc->actualcode = परिणाम;
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  }
  अन्यथा यदि(दत्त->समुच्चयः.quote) {
    infof(दत्त, "Sending quote commands");
    sshc->quote_item = दत्त->समुच्चयः.quote;
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_GETINFO);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_POSTQUOTE_INIT(संरचना Curl_easy *दत्त,
                                        संरचना ssh_conn *sshc)
{
  यदि(दत्त->समुच्चयः.postquote) {
    infof(दत्त, "Sending quote commands");
    sshc->quote_item = दत्त->समुच्चयः.postquote;
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  }
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क quote_error(संरचना Curl_easy *दत्त,
                       संरचना ssh_conn *sshc)
{
  failf(दत्त, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE(संरचना Curl_easy *दत्त,
                               संरचना ssh_conn *sshc,
                               संरचना SSHPROTO *sshp)
{
  ध्रुव वर्ण *cp;
  CURLcode परिणाम;

  /*
   * Support some of the "FTP" commands
   */
  ध्रुव वर्ण *cmd = sshc->quote_item->दत्त;
  sshc->acceptfail = FALSE;

  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  यदि(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  यदि(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    वर्ण *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->मार्गः);
    यदि(!tmp) {
      sshc->actualcode = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      प्रत्यागम SSH_NO_ERROR;
    }
    Curl_debug(दत्त, CURLINFO_शीर्षकम्_बहिर्गमनम्, "PWD\n", 4);
    Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, tmp, वाक्यदैर्घ्यम्(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, tmp, वाक्यदैर्घ्यम्(tmp));
    curlx_free(tmp);
    यदि(परिणाम) {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
    }
    अन्यथा
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
    प्रत्यागम SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  यदि(!cp) {
    failf(दत्त, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    प्रत्यागम SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  परिणाम = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
      failf(दत्त, "Out of memory");
    अन्यथा
      failf(दत्त, "Syntax error: Bad first parameter");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = परिणाम;
    प्रत्यागम SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  यदि(!वाक्ये_तुलय(cmd, "chgrp ", 6) ||
     !वाक्ये_तुलय(cmd, "chmod ", 6) ||
     !वाक्ये_तुलय(cmd, "chown ", 6) ||
     !वाक्ये_तुलय(cmd, "atime ", 6) ||
     !वाक्ये_तुलय(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
        failf(दत्त, "Out of memory");
      अन्यथा
        failf(दत्त, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
      प्रत्यागम SSH_NO_ERROR;
    }
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    sshc->quote_attrs = रिक्त;
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_STAT);
    प्रत्यागम SSH_NO_ERROR;
  }
  यदि(!वाक्ये_तुलय(cmd, "ln ", 3) ||
     !वाक्ये_तुलय(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
        failf(दत्त, "Out of memory");
      अन्यथा
        failf(दत्त, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
      प्रत्यागम SSH_NO_ERROR;
    }
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_SYMLINK);
    प्रत्यागम SSH_NO_ERROR;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "mkdir ", 6)) {
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    /* create directory */
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_MKDIR);
    प्रत्यागम SSH_NO_ERROR;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    यदि(परिणाम) {
      यदि(परिणाम == CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
        failf(दत्त, "Out of memory");
      अन्यथा
        failf(दत्त, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
      प्रत्यागम SSH_NO_ERROR;
    }
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_RENAME);
    प्रत्यागम SSH_NO_ERROR;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "rmdir ", 6)) {
    /* delete directory */
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_RMDIR);
    प्रत्यागम SSH_NO_ERROR;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "rm ", 3)) {
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_UNLINK);
    प्रत्यागम SSH_NO_ERROR;
  }
#यदि_परिभाषितम् HAS_STATVFS_SUPPORT
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "statvfs ", 8)) {
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_STATVFS);
    प्रत्यागम SSH_NO_ERROR;
  }
#यदि_अन्तः

  failf(दत्त, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_अग्र_QUOTE(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->अग्र;

  यदि(sshc->quote_item) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    यदि(sshc->nextstate != SSH_NO_STATE) {
      myssh_गन्तव्यम्(दत्त, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    अन्यथा {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_GETINFO);
    }
  }
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE_STAT(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc)
{
  ध्रुव वर्ण *cmd = sshc->quote_item->दत्त;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  यदि(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  यदि(sshc->quote_attrs)
    sftp_गुणाः_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  यदि(!sshc->quote_attrs) {
    failf(दत्त, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(दत्त, sshc, रिक्त);
    प्रत्यागम SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  यदि(!वाक्ये_तुलय(cmd, "chgrp", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t gid;
    यदि(curlx_str_सङ्ख्या(&p, &gid, UINT_MAX)) {
      failf(दत्त, "Syntax error: chgrp gid not a number");
      myssh_quote_error(दत्त, sshc, रिक्त);
      प्रत्यागम SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "chmod", 5)) {
    curl_off_t perms;
    ध्रुव वर्ण *p = sshc->quote_path1;
    यदि(curlx_str_octal(&p, &perms, 07777)) {
      failf(दत्त, "Syntax error: chmod permissions not a number");
      myssh_quote_error(दत्त, sshc, रिक्त);
      प्रत्यागम SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "chown", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t uid;
    यदि(curlx_str_सङ्ख्या(&p, &uid, UINT_MAX)) {
      failf(दत्त, "Syntax error: chown uid not a number");
      myssh_quote_error(दत्त, sshc, रिक्त);
      प्रत्यागम SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "atime", 5) ||
          !वाक्ये_तुलय(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    यदि(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(दत्त, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#यदि SIZEOF_TIME_T > 4
    अन्यथा यदि(date > 0xffffffff) {
      failf(दत्त, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#यदि_अन्तः
    यदि(fail) {
      myssh_quote_error(दत्त, sshc, रिक्त);
      प्रत्यागम SSH_NO_ERROR;
    }
    यदि(!वाक्ये_तुलय(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    अन्यथा /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_SETSTAT);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर शून्य conn_forget_socket(संरचना Curl_easy *दत्त, int8_t sockindex)
{
  संरचना connectdata *conn = दत्त->conn;
  यदि(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    संरचना Curl_cfilter *cf = conn->cfilter[sockindex];
    यदि(cf)
      (शून्य)Curl_conn_cf_cntrl(cf, दत्त, TRUE, CF_CTRL_FORGET_SOCKET, 0, रिक्त);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

स्थिर शून्य myssh_SESSION_DISCONNECT(संरचना Curl_easy *दत्त,
                                     संरचना ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  यदि(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = रिक्त;
  }

  यदि(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = रिक्त;
  }
  यदि(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = रिक्त;
  }

  ssh_disconnect(sshc->ssh_session);
  यदि(!ssh_संस्करणम्(SSH_संस्करणम्_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(दत्त, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
}

स्थिर पूर्णाङ्क myssh_SSH_SCP_DOWNLOAD(संरचना Curl_easy *दत्त,
                                  संरचना ssh_conn *sshc)
{
  uint64_t bytecount;
  पूर्णाङ्क rc = ssh_scp_pull_request(sshc->scp_session);
  यदि(rc != SSH_SCP_REQUEST_NEWFILE) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(दत्त, "%s", err_सन्देशः);
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_REMOTE_FILE_NOT_प्राप्तम्);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  यदि(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_TOO_LARGE);
  दत्त->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, दत्त->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  दत्त->conn->send_idx = 0;

  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_TRANS_INIT(संरचना Curl_easy *दत्त, संरचना ssh_conn *sshc,
                               संरचना SSHPROTO *sshp)
{
  CURLcode परिणाम;
  पूर्णाङ्क rc = 0;
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);

  परिणाम = Curl_getworkingpath(दत्त, sshc->homedir, &sshp->मार्गः);
  यदि(परिणाम) {
    sshc->actualcode = परिणाम;
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
    प्रत्यागम 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_समुच्चयः_blocking(sshc->ssh_session, 1);

  यदि(दत्त->state.upload) {
    यदि(दत्त->state.infilesize < 0) {
      failf(दत्त, "SCP requires a known file size for upload");
      प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->मार्गः);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_UPLOAD_INIT);
  }
  अन्यथा {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->मार्गः);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  यदि(!sshc->scp_session) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(दत्त, "%s", err_सन्देशः);
    rc = myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_UPLOAD_FAILED);
  }
  प्रत्यागम rc;
}

स्थिर शून्य sshc_cleanup(संरचना ssh_conn *sshc)
{
  यदि(sshc->initialized) {
    यदि(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = रिक्त;
    }
    यदि(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = रिक्त;
    }
    यदि(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = रिक्त;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    यदि(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = रिक्त;
    }
    यदि(sshc->quote_attrs) {
      sftp_गुणाः_free(sshc->quote_attrs);
      sshc->quote_attrs = रिक्त;
    }
    यदि(sshc->readdir_attrs) {
      sftp_गुणाः_free(sshc->readdir_attrs);
      sshc->readdir_attrs = रिक्त;
    }
    यदि(sshc->readdir_link_attrs) {
      sftp_गुणाः_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = रिक्त;
    }
    यदि(sshc->privkey) {
      ssh_कुञ्चिका_free(sshc->privkey);
      sshc->privkey = रिक्त;
    }
    यदि(sshc->pubkey) {
      ssh_कुञ्चिका_free(sshc->pubkey);
      sshc->pubkey = रिक्त;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_कुञ्चिका);
    curlx_safefree(sshc->priv_कुञ्चिका);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_link_मार्गः);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE_SETSTAT(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  यदि(rc == SSH_AGAIN)
    प्रत्यागम rc;
  यदि(rc && !sshc->acceptfail) {
    myssh_quote_error(दत्त, sshc, "setstat");
    प्रत्यागम rc;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE_SYMLINK(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  यदि(rc == SSH_AGAIN)
    प्रत्यागम rc;
  यदि(rc && !sshc->acceptfail) {
    myssh_quote_error(दत्त, sshc, "symlink");
    प्रत्यागम rc;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE_MKDIR(संरचना Curl_easy *दत्त,
                                     संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)दत्त->समुच्चयः.new_directory_perms);
  यदि(rc == SSH_AGAIN)
    प्रत्यागम rc;
  यदि(rc && !sshc->acceptfail) {
    myssh_quote_error(दत्त, sshc, "mkdir");
    प्रत्यागम rc;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE_RENAME(संरचना Curl_easy *दत्त,
                                      संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  यदि(rc == SSH_AGAIN)
    प्रत्यागम rc;
  यदि(rc && !sshc->acceptfail) {
    myssh_quote_error(दत्त, sshc, "rename");
    प्रत्यागम rc;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE_RMDIR(संरचना Curl_easy *दत्त,
                                     संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  यदि(rc == SSH_AGAIN)
    प्रत्यागम rc;
  यदि(rc && !sshc->acceptfail) {
    myssh_quote_error(दत्त, sshc, "rmdir");
    प्रत्यागम rc;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_QUOTE_UNLINK(संरचना Curl_easy *दत्त,
                                      संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  यदि(rc == SSH_AGAIN)
    प्रत्यागम rc;
  यदि(rc && !sshc->acceptfail) {
    myssh_quote_error(दत्त, sshc, "rm");
    प्रत्यागम rc;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_GETINFO(संरचना Curl_easy *दत्त,
                                 संरचना ssh_conn *sshc)
{
  यदि(दत्त->समुच्चयः.get_filetime)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_FILETIME);
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_TRANS_INIT);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_FILETIME(संरचना Curl_easy *दत्त,
                                  संरचना ssh_conn *sshc,
                                  संरचना SSHPROTO *sshp)
{
  sftp_गुणाः attrs;
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->मार्गः);
  यदि(attrs) {
    दत्त->info.filetime = attrs->mtime;
    sftp_गुणाः_free(attrs);
  }

  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_TRANS_INIT);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_TRANS_INIT(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);
  यदि(दत्त->state.upload)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_UPLOAD_INIT);
  अन्यथा {
    size_t मार्गः_len = वाक्यदैर्घ्यम्(sshp->मार्गः);

    यदि(मार्गः_len && sshp->मार्गः[मार्गः_len - 1] == '/')
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_INIT);
    अन्यथा
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_CREATE_DIRS_INIT(संरचना Curl_easy *दत्त,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);

  यदि(वाक्यदैर्घ्यम्(sshp->मार्गः) > 1) {
    sshc->slash_pos = sshp->मार्गः + 1; /* ignore the leading '/' */
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CREATE_DIRS);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_CREATE_DIRS(संरचना Curl_easy *दत्त,
                                     संरचना ssh_conn *sshc,
                                     संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  यदि(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(दत्त, "Creating directory '%s'", sshp->मार्गः);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    प्रत्यागम SSH_NO_ERROR;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_UPLOAD_INIT);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SFTP_CREATE_DIRS_MKDIR(संरचना Curl_easy *दत्त,
                                           संरचना ssh_conn *sshc,
                                           संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क rc;
  पूर्णाङ्क err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->मार्गः,
                  (mode_t)दत्त->समुच्चयः.new_directory_perms);
  यदि(rc == SSH_AGAIN)
    प्रत्यागम rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  यदि(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    यदि((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      प्रत्यागम myssh_गन्तव्यम्_SFTP_CLOSE(दत्त, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CREATE_DIRS);
  प्रत्यागम rc;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SCP_UPLOAD_INIT(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क rc;
  यदि(!sshp)
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  यदि(rc != SSH_OK) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(दत्त, "%s", err_सन्देशः);
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_प्रक्षिप_file64(sshc->scp_session, sshp->मार्गः,
                           (uint64_t)दत्त->state.infilesize,
                           (पूर्णाङ्क)दत्त->समुच्चयः.new_file_perms);

  यदि(rc != SSH_OK) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(दत्त, "%s", err_सन्देशः);
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_सज्जता_send(दत्त, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  दत्त->conn->recv_idx = FIRSTSOCKET;

  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SCP_DOWNLOAD_INIT(संरचना Curl_easy *दत्त,
                                      संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = ssh_scp_init(sshc->scp_session);
  यदि(rc != SSH_OK) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(दत्त, "%s", err_सन्देशः);
    प्रत्यागम myssh_गन्तव्यम्_ERROR(दत्त, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_DOWNLOAD);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SCP_समाप्तम्(संरचना Curl_easy *दत्त,
                             संरचना ssh_conn *sshc)
{
  यदि(दत्त->state.upload)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_SEND_EOF);
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_CHANNEL_FREE);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SCP_SEND_EOF(संरचना Curl_easy *दत्त,
                                 संरचना ssh_conn *sshc)
{
  यदि(sshc->scp_session) {
    पूर्णाङ्क rc = ssh_scp_close(sshc->scp_session);
    यदि(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      प्रत्यागम SSH_AGAIN;
    }
    यदि(rc != SSH_OK) {
      infof(दत्त,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_CHANNEL_FREE);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_अन्तर्_SCP_CHANNEL_FREE(संरचना Curl_easy *दत्त,
                                     संरचना ssh_conn *sshc)
{
  यदि(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = रिक्त;
  }
  CURL_TRC_SSH(दत्त, "SCP DONE phase complete");

  ssh_समुच्चयः_blocking(sshc->ssh_session, 0);

  myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_DISCONNECT);
  प्रत्यागम SSH_NO_ERROR;
}

स्थिर CURLcode myssh_अन्तर्_SESSION_FREE(संरचना Curl_easy *दत्त,
                                      संरचना ssh_conn *sshc)
{
  CURLcode परिणाम;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  परिणाम = sshc->actualcode;
  स्मृतिं_पूरय(sshc, 0, परिमाण(संरचना ssh_conn));
  connclose(दत्त->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम परिणाम;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
स्थिर CURLcode myssh_statemachine(संरचना Curl_easy *दत्त,
                                   संरचना ssh_conn *sshc,
                                   संरचना SSHPROTO *sshp,
                                   bool *खण्डम्)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क rc = SSH_NO_ERROR;

  *खण्डम् = FALSE;                   /* we are not blocking by default */
  कुरु {

    विकल्प(sshc->state) {
    पक्ष SSH_INIT:
      myssh_state_init(दत्त, sshc);
      FALLTHROUGH();
    पक्ष SSH_S_STARTUP:
      rc = myssh_अन्तर्_S_STARTUP(दत्त, sshc);
      यदि(rc)
        भङ्ग;
      FALLTHROUGH();
    पक्ष SSH_HOSTKEY:
      rc = myssh_is_ज्ञातम्(दत्त, sshc);
      यदि(rc != SSH_OK) {
        rc = myssh_गन्तव्यम्_ERROR(दत्त, sshc, CURLE_PEER_FAILED_VERIFICATION);
        भङ्ग;
      }
      myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    पक्ष SSH_AUTHLIST:
      rc = myssh_अन्तर्_AUTHLIST(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_PKEY_INIT:
      rc = myssh_अन्तर्_AUTH_PKEY_INIT(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_PKEY:
      rc = myssh_अन्तर्_AUTH_PKEY(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_GSSAPI:
      rc = myssh_अन्तर्_AUTH_GSSAPI(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_कुञ्चिका_INIT:
      rc = myssh_अन्तर्_AUTH_कुञ्चिका_INIT(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_कुञ्चिका:
      rc = myssh_अन्तर्_AUTH_कुञ्चिका(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_PASS_INIT:
      rc = myssh_अन्तर्_AUTH_PASS_INIT(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_PASS:
      rc = myssh_अन्तर्_AUTH_PASS(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_समाप्तम्:
      rc = myssh_अन्तर्_AUTH_समाप्तम्(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_INIT:
      rc = myssh_अन्तर्_SFTP_INIT(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_REALPATH:
      rc = myssh_अन्तर्_SFTP_REALPATH(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_INIT:
      rc = myssh_अन्तर्_SFTP_QUOTE_INIT(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_अन्तर्_SFTP_POSTQUOTE_INIT(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE:
      rc = myssh_अन्तर्_SFTP_QUOTE(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_अग्र_QUOTE:
      rc = myssh_अन्तर्_SFTP_अग्र_QUOTE(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_STAT:
      rc = myssh_अन्तर्_SFTP_QUOTE_STAT(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_अन्तर्_SFTP_QUOTE_SETSTAT(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_अन्तर्_SFTP_QUOTE_SYMLINK(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_अन्तर्_SFTP_QUOTE_MKDIR(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_RENAME:
      rc = myssh_अन्तर्_SFTP_QUOTE_RENAME(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_अन्तर्_SFTP_QUOTE_RMDIR(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_अन्तर्_SFTP_QUOTE_UNLINK(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_अन्तर्_SFTP_QUOTE_STATVFS(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_GETINFO:
      rc = myssh_अन्तर्_SFTP_GETINFO(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_FILETIME:
      rc = myssh_अन्तर्_SFTP_FILETIME(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_TRANS_INIT:
      rc = myssh_अन्तर्_SFTP_TRANS_INIT(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_UPLOAD_INIT:
      rc = myssh_अन्तर्_UPLOAD_INIT(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_अन्तर्_SFTP_CREATE_DIRS_INIT(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_CREATE_DIRS:
      rc = myssh_अन्तर्_SFTP_CREATE_DIRS(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_अन्तर्_SFTP_CREATE_DIRS_MKDIR(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR_INIT:
      rc = myssh_अन्तर्_SFTP_READDIR_INIT(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR:
      rc = myssh_अन्तर्_SFTP_READDIR(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR_LINK:
      rc = myssh_अन्तर्_SFTP_READDIR_LINK(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_अन्तर्_SFTP_READDIR_BOTTOM(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR_समाप्तम्:
      rc = myssh_अन्तर्_SFTP_READDIR_समाप्तम्(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_अन्तर्_SFTP_DOWNLOAD_INIT(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_अन्तर्_SFTP_DOWNLOAD_STAT(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_CLOSE:
      rc = myssh_अन्तर्_SFTP_CLOSE(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_SHUTDOWN:
      rc = myssh_अन्तर्_SFTP_SHUTDOWN(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SCP_TRANS_INIT:
      rc = myssh_अन्तर्_TRANS_INIT(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SCP_UPLOAD_INIT:
      rc = myssh_अन्तर्_SCP_UPLOAD_INIT(दत्त, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_अन्तर्_SCP_DOWNLOAD_INIT(दत्त, sshc);
      यदि(rc)
        भङ्ग;
      FALLTHROUGH();
    पक्ष SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SCP_समाप्तम्:
      rc = myssh_अन्तर्_SCP_समाप्तम्(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SCP_SEND_EOF:
      rc = myssh_अन्तर्_SCP_SEND_EOF(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_SCP_CHANNEL_FREE:
      myssh_अन्तर्_SCP_CHANNEL_FREE(दत्त, sshc);
      FALLTHROUGH();
    पक्ष SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(दत्त, sshc);
      FALLTHROUGH();
    पक्ष SSH_SESSION_FREE:
      परिणाम = myssh_अन्तर्_SESSION_FREE(दत्त, sshc);
      भङ्ग;
    पक्ष SSH_QUIT:
    स्वभाव:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
      भङ्ग;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } यावत्((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  यदि(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *खण्डम् = TRUE;
  }
  यदि(!परिणाम && (sshc->state == SSH_STOP))
    परिणाम = sshc->actualcode;
  CURL_TRC_SSH(दत्त, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (पूर्णाङ्क)परिणाम, *खण्डम्);
  प्रत्यागम परिणाम;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode myssh_multi_statemach(संरचना Curl_easy *दत्त,
                                      bool *समाप्तम्)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(दत्त, CURL_META_SSH_EASY);
  bool खण्डम्;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode परिणाम;

  यदि(!sshc || !sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = myssh_statemachine(दत्त, sshc, sshp, &खण्डम्);
  *समाप्तम् = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, खण्डम्);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode myssh_खण्डम्_statemach(संरचना Curl_easy *दत्त,
                                      संरचना ssh_conn *sshc,
                                      संरचना SSHPROTO *sshp,
                                      bool disconnect)
{
  संरचना connectdata *conn = दत्त->conn;
  CURLcode परिणाम = CURLE_OK;

  यावत्((sshc->state != SSH_STOP) && !परिणाम) {
    bool खण्डम्;
    timediff_t वामः_ms = 1000;

    परिणाम = myssh_statemachine(दत्त, sshc, sshp, &खण्डम्);
    यदि(परिणाम)
      भङ्ग;

    यदि(!disconnect) {
      परिणाम = Curl_pgrsCheck(दत्त);
      यदि(परिणाम)
        भङ्ग;

      वामः_ms = Curl_timeleft_ms(दत्त);
      यदि(वामः_ms < 0) {
        failf(दत्त, "Operation timed out");
        प्रत्यागम CURLE_OPERATION_TIMEDOUT;
      }
    }

    यदि(खण्डम्) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (शून्य)SOCKET_READABLE(fd_read,
                            (!वामः_ms || वामः_ms > 1000) ? 1000 : वामः_ms);
    }
  }

  प्रत्यागम परिणाम;
}

स्थिर शून्य myssh_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना SSHPROTO *sshp = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_safefree(sshp->मार्गः);
  curlx_free(sshp);
}

स्थिर शून्य myssh_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना ssh_conn *sshc = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
स्थिर CURLcode myssh_सज्जता_connection(संरचना Curl_easy *दत्त,
                                       संरचना connectdata *conn)
{
  संरचना SSHPROTO *sshp;
  संरचना ssh_conn *sshc;

  sshc = curlx_calloc(1, परिमाण(*sshc));
  यदि(!sshc)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  curlx_dyn_init(&sshc->readdir_buf, CURL_मार्गः_MAX * 2);
  sshc->initialized = TRUE;
  यदि(Curl_conn_meta_समुच्चयः(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  sshp = curlx_calloc(1, परिमाण(*sshp));
  यदि(!sshp ||
     Curl_meta_समुच्चयः(दत्त, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम Curl_ssh_सज्जता_pkey(दत्त, sshc);
}

स्थिर Curl_recv scp_recv, sftp_recv;
स्थिर Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
स्थिर CURLcode myssh_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम;
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *ssh = Curl_meta_get(दत्त, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णाङ्क rc;

  यदि(!sshc || !ssh)
    प्रत्यागम CURLE_FAILED_INIT;

  CURL_TRC_SSH(दत्त, "myssh_connect");
  यदि(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  अन्यथा {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  यदि(!sshc->ssh_session) {
    failf(दत्त, "Failure initializing ssh session");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_समुच्चयः(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  यदि(rc != SSH_OK) {
    failf(दत्त, "Could not set remote host");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, रिक्त);
  यदि(rc != SSH_OK) {
    infof(दत्त, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(दत्त, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_समुच्चयः(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  यदि(rc != SSH_OK) {
    failf(दत्त, "Could not set socket");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  यदि(Curl_creds_has_user(conn->creds)) {
    infof(दत्त, "User: %s", conn->creds->user);
    rc = ssh_options_समुच्चयः(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    यदि(rc != SSH_OK) {
      failf(दत्त, "Could not set user");
      प्रत्यागम CURLE_FAILED_INIT;
    }
  }

  यदि(CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS)) {
    infof(दत्त, "Known hosts: %s",
          CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_समुच्चयः(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS));
    यदि(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_समुच्चयः(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS));
    यदि(rc != SSH_OK) {
      failf(दत्त, "Could not set known hosts file path");
      प्रत्यागम CURLE_FAILED_INIT;
    }
  }

  यदि(conn->origin->पत्तनम्) {
    rc = ssh_options_समुच्चयः(sshc->ssh_session, SSH_OPTIONS_पत्तनम्,
                         &conn->origin->पत्तनम्);
    यदि(rc != SSH_OK) {
      failf(दत्त, "Could not set remote port");
      प्रत्यागम CURLE_FAILED_INIT;
    }
  }

  यदि(दत्त->समुच्चयः.ssh_compression) {
    rc = ssh_options_समुच्चयः(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    यदि(rc != SSH_OK) {
      failf(दत्त, "Could not set compression");
      प्रत्यागम CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = रिक्त;
  sshc->pubkey = रिक्त;

  यदि(sshc->pub_कुञ्चिका) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_कुञ्चिका, &sshc->pubkey);
    यदि(rc != SSH_OK) {
      failf(दत्त, "Could not load public key file");
      प्रत्यागम CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_गन्तव्यम्(दत्त, sshc, SSH_INIT);

  परिणाम = myssh_multi_statemach(दत्त, समाप्तम्);

  प्रत्यागम परिणाम;
}

/* called from multi.c while DOing */
स्थिर CURLcode scp_doing(संरचना Curl_easy *दत्त, bool *dophase_समाप्तम्)
{
  CURLcode परिणाम;

  परिणाम = myssh_multi_statemach(दत्त, dophase_समाप्तम्);

  यदि(*dophase_समाप्तम्) {
    CURL_TRC_SSH(दत्त, "DO phase is complete");
  }
  प्रत्यागम परिणाम;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

स्थिर CURLcode scp_perform(संरचना Curl_easy *दत्त,
                            bool *connected, bool *dophase_समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(दत्त->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(दत्त, "DO phase starts");

  *dophase_समाप्तम् = FALSE;        /* not done yet */
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_TRANS_INIT);

  परिणाम = myssh_multi_statemach(दत्त, dophase_समाप्तम्);

  *connected = Curl_conn_is_connected(दत्त->conn, FIRSTSOCKET);

  यदि(*dophase_समाप्तम्) {
    CURL_TRC_SSH(दत्त, "DO phase is complete");
  }

  प्रत्यागम परिणाम;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode scp_disconnect(संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn,
                               bool dead_connection)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(दत्त, CURL_META_SSH_EASY);
  (शून्य)dead_connection;

  यदि(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_DISCONNECT);
    परिणाम = myssh_खण्डम्_statemach(दत्त, sshc, sshp, TRUE);
  }

  प्रत्यागम परिणाम;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
स्थिर CURLcode myssh_समाप्तम्(संरचना Curl_easy *दत्त,
                           संरचना ssh_conn *sshc,
                           CURLcode स्थितिः)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना SSHPROTO *sshp = Curl_meta_get(दत्त, CURL_META_SSH_EASY);

  यदि(!स्थितिः && sshp) {
    /* run the state-machine */
    परिणाम = myssh_खण्डम्_statemach(दत्त, sshc, sshp, FALSE);
  }
  अन्यथा
    परिणाम = स्थितिः;

  यदि(Curl_pgrs_समाप्तम्(दत्त, Curl_pgrs_now(दत्त)))
    प्रत्यागम CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(दत्त);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode scp_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः,
                         bool premature)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(दत्त->conn, CURL_META_SSH_CONN);
  (शून्य)premature;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(!स्थितिः)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_समाप्तम्);

  प्रत्यागम myssh_समाप्तम्(दत्त, sshc, स्थितिः);
}

स्थिर CURLcode scp_send(संरचना Curl_easy *दत्त, int8_t sockindex,
                         ध्रुव uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  पूर्णाङ्क rc;
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  (शून्य)eos;
  *pnwritten = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#यदि 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  यदि(rc == SSH_AGAIN)
    प्रत्यागम CURLE_AGAIN;
  अन्यथा
#यदि_अन्तः
  यदि(rc != SSH_OK)
    प्रत्यागम CURLE_SSH;

  *pnwritten = len;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode scp_recv(संरचना Curl_easy *दत्त, int8_t sockindex,
                         वर्ण *mem, size_t len, size_t *pnread)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  पूर्णाङ्क nread;

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  यदि(nread == SSH_ERROR)
    प्रत्यागम CURLE_SSH;
#यदि 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  यदि(nread == SSH_AGAIN)
    प्रत्यागम CURLE_AGAIN;
#यदि_अन्तः
  *pnread = (size_t)nread;
  प्रत्यागम CURLE_OK;
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

स्थिर CURLcode sftp_perform(संरचना Curl_easy *दत्त,
                             bool *connected,
                             bool *dophase_समाप्तम्)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(दत्त->conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_SSH(दत्त, "DO phase starts");

  *dophase_समाप्तम् = FALSE; /* not done yet */
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  परिणाम = myssh_multi_statemach(दत्त, dophase_समाप्तम्);

  *connected = Curl_conn_is_connected(दत्त->conn, FIRSTSOCKET);

  यदि(*dophase_समाप्तम्) {
    CURL_TRC_SSH(दत्त, "DO phase is complete");
  }

  प्रत्यागम परिणाम;
}

/* called from multi.c while DOing */
स्थिर CURLcode sftp_doing(संरचना Curl_easy *दत्त,
                           bool *dophase_समाप्तम्)
{
  CURLcode परिणाम = myssh_multi_statemach(दत्त, dophase_समाप्तम्);
  यदि(*dophase_समाप्तम्) {
    CURL_TRC_SSH(दत्त, "DO phase is complete");
  }
  प्रत्यागम परिणाम;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode sftp_disconnect(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn,
                                bool dead_connection)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(दत्त, CURL_META_SSH_EASY);
  CURLcode परिणाम = CURLE_OK;
  (शून्य)dead_connection;

  CURL_TRC_SSH(दत्त, "DISCONNECT starts now");

  यदि(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_SHUTDOWN);
    परिणाम = myssh_खण्डम्_statemach(दत्त, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(दत्त, "DISCONNECT is done");
  प्रत्यागम परिणाम;
}

स्थिर CURLcode sftp_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः,
                          bool premature)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(!स्थितिः) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    यदि(!premature && दत्त->समुच्चयः.postquote && !conn->द्वयङ्काः.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
  }
  प्रत्यागम myssh_समाप्तम्(दत्त, sshc, स्थितिः);
}

/* return number of sent bytes */
स्थिर CURLcode sftp_send(संरचना Curl_easy *दत्त, int8_t sockindex,
                          ध्रुव uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (शून्य)sockindex;
  (शून्य)eos;
  *pnwritten = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

#यदि LIBSSH_संस्करणम्_INT > SSH_संस्करणम्_INT(0, 11, 0)
  विकल्प(sshc->sftp_send_state) {
  पक्ष 0:
    sftp_file_समुच्चयः_nonblocking(sshc->sftp_file);
    यदि(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      प्रत्यागम CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  पक्ष 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    यदि(nwrite == SSH_AGAIN)
      प्रत्यागम CURLE_AGAIN;
    अन्यथा यदि(nwrite < 0)
      प्रत्यागम CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    प्रत्यागम CURLE_OK;
  स्वभाव:
    /* we never reach here */
    प्रत्यागम CURLE_SEND_ERROR;
  }
#अन्यथा
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  यदि(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#यदि 0 /* not returned by libssh on write */
  यदि(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  अन्यथा
#यदि_अन्तः
  यदि(nwrite < 0)
    प्रत्यागम CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  प्रत्यागम CURLE_OK;
#यदि_अन्तः
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
स्थिर CURLcode sftp_recv(संरचना Curl_easy *दत्त, int8_t sockindex,
                          वर्ण *mem, size_t len, size_t *pnread)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (शून्य)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_आकार);
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  विकल्प(sshc->sftp_recv_state) {
  पक्ष 0:
#यदि LIBSSH_संस्करणम्_INT > SSH_संस्करणम्_INT(0, 11, 0)
    यदि(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      प्रत्यागम CURLE_RECV_ERROR;
    }
#अन्यथा
    sshc->sftp_file_सूचकांक =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    यदि(sshc->sftp_file_सूचकांक < 0)
      प्रत्यागम CURLE_RECV_ERROR;
#यदि_अन्तः

    FALLTHROUGH();
  पक्ष 1:
    sshc->sftp_recv_state = 1;

#यदि LIBSSH_संस्करणम्_INT > SSH_संस्करणम्_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#अन्यथा
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_सूचकांक);
#यदि_अन्तः

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    यदि(nread == SSH_AGAIN)
      प्रत्यागम CURLE_AGAIN;
    अन्यथा यदि(nread < 0)
      प्रत्यागम CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    प्रत्यागम CURLE_OK;

  स्वभाव:
    /* we never reach here */
    प्रत्यागम CURLE_RECV_ERROR;
  }
}

स्थिर CURLcode myssh_do_it(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम;
  bool connected = FALSE;
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *समाप्तम् = FALSE;                /* default to false */
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  दत्त->req.आकार = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(दत्त);

  यदि(conn->scheme->protocol & CURLPROTO_SCP)
    परिणाम = scp_perform(दत्त, &connected, समाप्तम्);
  अन्यथा
    परिणाम = sftp_perform(दत्त, &connected, समाप्तम्);

  प्रत्यागम परिणाम;
}

CURLcode Curl_ssh_init(शून्य)
{
  यदि(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    प्रत्यागम CURLE_FAILED_INIT;
  }
  प्रत्यागम CURLE_OK;
}

शून्य Curl_ssh_cleanup(शून्य)
{
  (शून्य)ssh_finalize();
}

शून्य Curl_ssh_संस्करणम्(वर्ण *बफर, size_t buflen)
{
  (शून्य)curl_msnprintf(बफर, buflen, "libssh/%s", ssh_संस्करणम्(0));
}

/*
 * SCP.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_scp = {
  myssh_सज्जता_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_समाप्तम्,                             /* done */
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
ध्रुव संरचना Curl_protocol Curl_protocol_sftp = {
  myssh_सज्जता_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_समाप्तम्,                            /* done */
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

#यदि_अन्तः /* USE_LIBSSH */
