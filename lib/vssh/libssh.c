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

#ifdef USE_LIBSSH

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

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

#ifdef HAVE_UNISTD_H
#अन्तर्भाव <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#अन्तर्भाव <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  करोहि {                          \
    सचे(x) {                     \
      ssh_string_free_char(x);  \
      x = रिक्त;                 \
    }                           \
  } याव(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

स्थिर CURLcode sftp_error_to_CURLE(पूर्णाङ्क err)
{
  विकल्प(err) {
  पक्ष SSH_FX_OK:
    पटिनिवत्त CURLE_OK;

  पक्ष SSH_FX_NO_SUCH_FILE:
  पक्ष SSH_FX_NO_SUCH_PATH:
    पटिनिवत्त CURLE_REMOTE_FILE_NOT_FOUND;

  पक्ष SSH_FX_PERMISSION_DENIED:
  पक्ष SSH_FX_WRITE_PROTECT:
    पटिनिवत्त CURLE_REMOTE_ACCESS_DENIED;

  पक्ष SSH_FX_FILE_ALREADY_EXISTS:
    पटिनिवत्त CURLE_REMOTE_FILE_EXISTS;

  स्वभाव:
    भङ्ग;
  }

  पटिनिवत्त CURLE_SSH;
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
स्थिर पूर्णाङ्क myssh_is_known(संरचना Curl_easy *data, संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  ssh_कुञ्चिका pubkey;
  unsigned वर्ण *hash_sha256 = रिक्त;
  size_t hlen_sha256;
  unsigned वर्ण *hash_md5 = रिक्त;
  size_t hlen_md5;
  वर्ण *found_base64 = रिक्त;
  वर्ण *known_base64 = रिक्त;
  पूर्णाङ्क vstate;
  गणना curl_khmatch keymatch;
  संरचना curl_khkey foundkey;
  संरचना curl_khkey *knownkeyp = रिक्त;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  संरचना ssh_knownhosts_entry *knownhostsentry = रिक्त;
  संरचना curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  सचे(rc != SSH_OK)
    पटिनिवत्त rc;

  सचे(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुञ्चिका_SHA256)) {
    ध्रुव वर्ण *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुञ्चिका_SHA256);
    वर्ण *fingerprint_b64 = रिक्त;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    सचे(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      गच्छ cleanup;
    }

    सचे(curlx_base64_encode((ध्रुव uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      गच्छ cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    याव((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    याव((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    सचे((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
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

  सचे(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुञ्चिका_MD5)) {
    ध्रुव वर्ण *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुञ्चिका_MD5);
    वर्ण md5buffer[33];
    पूर्णाङ्क i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    सचे(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      गच्छ cleanup;
    }

    अत्थाय(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    सचे(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      गच्छ cleanup;
    }

    rc = SSH_OK;
    गच्छ cleanup;
  }

  सचे(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    सचे(knownhostsentry) {
      सचे(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        सचे(rc != SSH_OK) {
          गच्छ cleanup;
        }
        knownkey.कुञ्चिका = known_base64;
        knownkey.len = strlen(known_base64);

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
    पक्ष SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      भङ्ग;
    पक्ष SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      भङ्ग;
    पक्ष SSH_KNOWN_HOSTS_NOT_FOUND:
    पक्ष SSH_KNOWN_HOSTS_UNKNOWN:
    पक्ष SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      भङ्ग;
    स्वभाव:
      keymatch = CURLKHMATCH_MISMATCH;
      भङ्ग;
    }

    सचे(func) { /* use callback to determine action */
      संरचना Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      सचे(rc != SSH_OK)
        गच्छ cleanup;

      foundkey.कुञ्चिका = found_base64;
      foundkey.len = strlen(found_base64);

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

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      विकल्प(rc) {
      पक्ष CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        सचे(rc != SSH_OK) {
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
    अञ्ञथा {
      सचे(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        गच्छ cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  सचे(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    मुक्त(found_base64); /* allocated by libssh, deallocate with system free */
  }
  सचे(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    मुक्त(known_base64); /* allocated by libssh, deallocate with system free */
  }
  सचे(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  सचे(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_कुञ्चिका_free(pubkey);
  सचे(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  पटिनिवत्त rc;
}

स्थिर पूर्णाङ्क myssh_to_ERROR(संरचना Curl_easy *data,
                          संरचना ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  पटिनिवत्त SSH_ERROR;
}

स्थिर पूर्णाङ्क myssh_to_SFTP_CLOSE(संरचना Curl_easy *data,
                               संरचना ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  पटिनिवत्त SSH_ERROR;
}

स्थिर पूर्णाङ्क myssh_to_PASSWD_AUTH(संरचना Curl_easy *data,
                                संरचना ssh_conn *sshc)
{
  सचे(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    पटिनिवत्त SSH_OK;
  }
  पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

स्थिर पूर्णाङ्क myssh_to_कुञ्चिका_AUTH(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  सचे(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_कुञ्चिका_INIT);
    पटिनिवत्त SSH_OK;
  }
  पटिनिवत्त myssh_to_PASSWD_AUTH(data, sshc);
}

स्थिर पूर्णाङ्क myssh_to_GSSAPI_AUTH(संरचना Curl_easy *data,
                                संरचना ssh_conn *sshc)
{
  सचे(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    पटिनिवत्त SSH_OK;
  }
  पटिनिवत्त myssh_to_कुञ्चिका_AUTH(data, sshc);
}

स्थिर पूर्णाङ्क myssh_in_SFTP_READDIR_INIT(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc,
                                      संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  सचे(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    पटिनिवत्त SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  सचे(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    पटिनिवत्त myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_READDIR(संरचना Curl_easy *data,
                                 संरचना ssh_conn *sshc,
                                 संरचना SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  सचे(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  सचे(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    सचे(data->set.list_only) {
      वर्ण *tmp_पङ्क्तिः;

      tmp_पङ्क्तिः = curl_maprintf("%s\n", sshc->readdir_filename);
      सचे(!tmp_पङ्क्तिः) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        पटिनिवत्त SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmp_पङ्क्तिः, sshc->readdir_len + 1);
      curlx_free(tmp_पङ्क्तिः);

      सचे(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        पटिनिवत्त SSH_NO_ERROR;
      }
    }
    अञ्ञथा {
      सचे(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        पटिनिवत्त SSH_ERROR;
      }

      सचे((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        सचे(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          पटिनिवत्त SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        पटिनिवत्त SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      पटिनिवत्त SSH_NO_ERROR;
    }
  }
  अञ्ञथा सचे(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  अञ्ञथा {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    पटिनिवत्त myssh_to_SFTP_CLOSE(data, sshc);
  }
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_READDIR_LINK(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  सचे(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  सचे(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    पटिनिवत्त myssh_to_SFTP_CLOSE(data, sshc);
  }

  सचे(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    सचे(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    अञ्ञथा
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = रिक्त;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  अञ्ञथा {
    sshc->readdir_len = strlen(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  सचे(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    पटिनिवत्त SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = रिक्त;
  sshc->readdir_filename = रिक्त;
  sshc->readdir_longentry = रिक्त;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_READDIR_BOTTOM(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc)
{
  CURLcode result;

  सचे(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  अञ्ञथा
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = रिक्त;

  सचे(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  अञ्ञथा
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_READDIR_DONE(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = रिक्त;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर शून्य myssh_quote_error(संरचना Curl_easy *data, संरचना ssh_conn *sshc,
                              ध्रुव वर्ण *cmd)
{
  सचे(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE_STATVFS(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  सचे(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    पटिनिवत्त SSH_OK;
  }
  अञ्ञथा सचे(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#नोचेद्यदि defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#अञ्ञथा
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    वर्ण *tmp = curl_maprintf("statvfs:\n"
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

    सचे(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    सचे(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    सचे(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त SSH_OK;
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
    सचे(rc == SSH_AUTH_AGAIN)
      पटिनिवत्त SSH_AGAIN;

    सचे(rc != SSH_AUTH_INFO)
      पटिनिवत्त SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    सचे(nprompts != 1)
      पटिनिवत्त SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    सचे(rc < 0)
      पटिनिवत्त SSH_ERROR;

    FALLTHROUGH();
  पक्ष 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, रिक्त, रिक्त);
    सचे(rc == SSH_AUTH_AGAIN)
      पटिनिवत्त SSH_AGAIN;
    अञ्ञथा सचे(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    अञ्ञथा सचे(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      सचे(nprompts)
        पटिनिवत्त SSH_ERROR;

      sshc->kbd_state = 2;
      गच्छ restart;
    }
    अञ्ञथा
      rc = SSH_ERROR;
    भङ्ग;
  पक्ष 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, रिक्त, रिक्त);
    सचे(rc == SSH_AUTH_AGAIN)
      पटिनिवत्त SSH_AGAIN;
    अञ्ञथा सचे(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    अञ्ञथा
      rc = SSH_ERROR;

    भङ्ग;
  स्वभाव:
    पटिनिवत्त SSH_ERROR;
  }

  sshc->kbd_state = 0;
  पटिनिवत्त rc;
}

स्थिर शून्य myssh_state_init(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#सचे 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

स्थिर शून्य myssh_block2waitfor(संरचना connectdata *conn,
                                संरचना ssh_conn *sshc,
                                bool block)
{
  (शून्य)conn;
  सचे(block) {
    पूर्णाङ्क dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  अञ्ञथा
    sshc->waitfor = 0;
}

स्थिर पूर्णाङ्क myssh_in_S_STARTUP(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  पूर्णाङ्क rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  सचे(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  अञ्ञथा सचे(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  अञ्ञथा
    myssh_to(data, sshc, SSH_HOSTKEY);

  पटिनिवत्त rc;
}

स्थिर पूर्णाङ्क myssh_in_AUTHLIST(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, रिक्त);
  सचे(rc == SSH_AUTH_AGAIN)
    पटिनिवत्त SSH_AGAIN;

  सचे(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    पटिनिवत्त rc;
  }
  अञ्ञथा सचे(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    पटिनिवत्त rc;
  }

  sshc->auth_methods =
    (unsigned पूर्णाङ्क)ssh_userauth_list(sshc->ssh_session, रिक्त);
  सचे(sshc->auth_methods)
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
  सचे((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_कुञ्चिका && sshc->priv_कुञ्चिका[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  अञ्ञथा सचे(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  अञ्ञथा सचे(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_कुञ्चिका_INIT);
  }
  अञ्ञथा सचे(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  अञ्ञथा {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  पटिनिवत्त rc;
}

स्थिर पूर्णाङ्क myssh_in_AUTH_PKEY_INIT(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  सचे(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    पटिनिवत्त rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  सचे(sshc->priv_कुञ्चिका && sshc->priv_कुञ्चिका[0]) {
    सचे(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, रिक्त, sshc->pubkey);
      सचे(rc == SSH_AUTH_AGAIN)
        पटिनिवत्त SSH_AGAIN;

      सचे(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        पटिनिवत्त rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_कुञ्चिका,
                                     sshc->passphrase, रिक्त,
                                     रिक्त, &sshc->privkey);
    सचे(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_कुञ्चिका);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      पटिनिवत्त rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  अञ्ञथा {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, रिक्त,
                                     sshc->passphrase);
    सचे(rc == SSH_AUTH_AGAIN)
      पटिनिवत्त SSH_AGAIN;

    सचे(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      पटिनिवत्त rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  पटिनिवत्त rc;
}

स्थिर पूर्णाङ्क myssh_in_AUTH_PKEY(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = ssh_userauth_publickey(sshc->ssh_session, रिक्त, sshc->privkey);
  सचे(rc == SSH_AUTH_AGAIN)
    पटिनिवत्त SSH_AGAIN;
  अञ्ञथा सचे(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    पटिनिवत्त SSH_OK;
  }
  अञ्ञथा {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    पटिनिवत्त myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

स्थिर पूर्णाङ्क myssh_in_AUTH_GSSAPI(संरचना Curl_easy *data,
                                संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  सचे(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    पटिनिवत्त myssh_to_कुञ्चिका_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  सचे(rc == SSH_AUTH_AGAIN)
    पटिनिवत्त SSH_AGAIN;

  सचे(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    पटिनिवत्त SSH_OK;
  }

  पटिनिवत्त myssh_to_कुञ्चिका_AUTH(data, sshc);
}

स्थिर पूर्णाङ्क myssh_in_AUTH_कुञ्चिका_INIT(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  सचे(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_कुञ्चिका);
    पटिनिवत्त SSH_NO_ERROR;
  }
  पटिनिवत्त myssh_to_PASSWD_AUTH(data, sshc);
}

स्थिर पूर्णाङ्क myssh_in_AUTH_कुञ्चिका(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  पूर्णाङ्क rc = myssh_auth_interactive(data->conn, sshc);
  सचे(rc == SSH_AGAIN)
    पटिनिवत्त rc;
  अञ्ञथा सचे(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    पटिनिवत्त SSH_NO_ERROR;
  }
  अञ्ञथा
    पटिनिवत्त myssh_to_PASSWD_AUTH(data, sshc);
}

स्थिर पूर्णाङ्क myssh_in_AUTH_PASS_INIT(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc)
{
  सचे(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_AUTH_PASS(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = ssh_userauth_password(sshc->ssh_session, रिक्त,
                                 Curl_creds_passwd(data->conn->creds));
  सचे(rc == SSH_AUTH_AGAIN)
    पटिनिवत्त SSH_AGAIN;
  अञ्ञथा सचे(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    पटिनिवत्त SSH_NO_ERROR;
  }
  पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

स्थिर पूर्णाङ्क myssh_in_AUTH_DONE(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  सचे(!sshc->authed) {
    failf(data, "Authentication failure");
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  सचे(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    पटिनिवत्त SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_UPLOAD_INIT(संरचना Curl_easy *data,
                                संरचना ssh_conn *sshc,
                                संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क flags;
  पूर्णाङ्क rc = 0;

  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  सचे(data->state.resume_from) {
    sftp_attributes attrs;

    सचे(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      सचे(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        सचे(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          पटिनिवत्त rc;
        }
        data->state.resume_from = size;
      }
      अञ्ञथा {
        data->state.resume_from = 0;
      }
    }
  }

  सचे(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  अञ्ञथा सचे(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  अञ्ञथा
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  सचे(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  सचे(!sshc->sftp_file) {
    पूर्णाङ्क err = sftp_get_error(sshc->sftp_session);

    सचे((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      पटिनिवत्त rc;
    }
    अञ्ञथा {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      पटिनिवत्त rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  सचे(data->state.resume_from > 0 && !data->set.remote_append) {
    पूर्णाङ्क seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    सचे(data->set.seek_func) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    सचे(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      सचे(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        पटिनिवत्त rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      करोहि {
        वर्ण scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)परिमाण(scratch)) ?
          परिमाण(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        सचे((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          पटिनिवत्त rc;
        }
      } याव(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    सचे(data->state.infilesize > 0) {
      सचे(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    सचे(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      पटिनिवत्त rc;
    }
  }
  सचे(data->state.infilesize > 0) {
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
#सचे LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त rc;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_DOWNLOAD_INIT(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc,
                                       संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  सचे(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  सचे(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    पटिनिवत्त myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_DOWNLOAD_STAT(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  curl_off_t size;
  पूर्णाङ्क rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  सचे(!attrs ||
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
    सचे(attrs)
      sftp_attributes_free(attrs);
  }
  अञ्ञथा {
    size = attrs->size;

    sftp_attributes_free(attrs);

    सचे(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    सचे(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      सचे(result)
        पटिनिवत्त myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      सचे(rc)
        पटिनिवत्त myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  सचे(data->state.resume_from) {
    सचे(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      सचे(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    अञ्ञथा {
      सचे(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    सचे(rc)
      पटिनिवत्त myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  सचे(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    पटिनिवत्त rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  पटिनिवत्त rc;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_CLOSE(संरचना Curl_easy *data,
                               संरचना ssh_conn *sshc,
                               संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  सचे(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = रिक्त;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  सचे(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_STOP);
  }
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_SHUTDOWN(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#सचे LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  सचे(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = रिक्त;
  }

  सचे(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = रिक्त;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_INIT(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  सचे(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  सचे(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    पटिनिवत्त myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_REALPATH(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  सचे(!sshc->homedir)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  सचे(!data->state.most_recent_ftp_entrypath)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE_INIT(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp)
{
  CURLcode result;

  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  सचे(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  अञ्ञथा सचे(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अञ्ञथा
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_POSTQUOTE_INIT(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc)
{
  सचे(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_STOP);
  }
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क quote_error(संरचना Curl_easy *data,
                       संरचना ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE(संरचना Curl_easy *data,
                               संरचना ssh_conn *sshc,
                               संरचना SSHPROTO *sshp)
{
  ध्रुव वर्ण *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  ध्रुव वर्ण *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  सचे(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  सचे(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    वर्ण *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    सचे(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      पटिनिवत्त SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    सचे(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    अञ्ञथा
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    पटिनिवत्त SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  सचे(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    पटिनिवत्त SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  सचे(result) {
    सचे(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    अञ्ञथा
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    पटिनिवत्त SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  सचे(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    सचे(result) {
      सचे(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      अञ्ञथा
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      पटिनिवत्त SSH_NO_ERROR;
    }
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    sshc->quote_attrs = रिक्त;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    पटिनिवत्त SSH_NO_ERROR;
  }
  सचे(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    सचे(result) {
      सचे(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      अञ्ञथा
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      पटिनिवत्त SSH_NO_ERROR;
    }
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    पटिनिवत्त SSH_NO_ERROR;
  }
  अञ्ञथा सचे(!strncmp(cmd, "mkdir ", 6)) {
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    पटिनिवत्त SSH_NO_ERROR;
  }
  अञ्ञथा सचे(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    सचे(result) {
      सचे(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      अञ्ञथा
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      पटिनिवत्त SSH_NO_ERROR;
    }
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    पटिनिवत्त SSH_NO_ERROR;
  }
  अञ्ञथा सचे(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    पटिनिवत्त SSH_NO_ERROR;
  }
  अञ्ञथा सचे(!strncmp(cmd, "rm ", 3)) {
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    पटिनिवत्त SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  अञ्ञथा सचे(!strncmp(cmd, "statvfs ", 8)) {
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    पटिनिवत्त SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_NEXT_QUOTE(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  सचे(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अञ्ञथा {
    सचे(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    अञ्ञथा {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE_STAT(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  ध्रुव वर्ण *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  सचे(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  सचे(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  सचे(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, रिक्त);
    पटिनिवत्त SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  सचे(!strncmp(cmd, "chgrp", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t gid;
    सचे(curlx_str_सङ्ख्या(&p, &gid, UINT_MAX)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, रिक्त);
      पटिनिवत्त SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  अञ्ञथा सचे(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ध्रुव वर्ण *p = sshc->quote_path1;
    सचे(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, रिक्त);
      पटिनिवत्त SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  अञ्ञथा सचे(!strncmp(cmd, "chown", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t uid;
    सचे(curlx_str_सङ्ख्या(&p, &uid, UINT_MAX)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, रिक्त);
      पटिनिवत्त SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  अञ्ञथा सचे(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    सचे(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#सचे SIZEOF_TIME_T > 4
    अञ्ञथा सचे(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    सचे(fail) {
      myssh_quote_error(data, sshc, रिक्त);
      पटिनिवत्त SSH_NO_ERROR;
    }
    सचे(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    अञ्ञथा /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर शून्य conn_forget_socket(संरचना Curl_easy *data, int8_t sockindex)
{
  संरचना connectdata *conn = data->conn;
  सचे(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    संरचना Curl_cfilter *cf = conn->cfilter[sockindex];
    सचे(cf)
      (शून्य)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, रिक्त);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

स्थिर शून्य myssh_SESSION_DISCONNECT(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  सचे(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = रिक्त;
  }

  सचे(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = रिक्त;
  }
  सचे(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = रिक्त;
  }

  ssh_disconnect(sshc->ssh_session);
  सचे(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

स्थिर पूर्णाङ्क myssh_SSH_SCP_DOWNLOAD(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  uint64_t bytecount;
  पूर्णाङ्क rc = ssh_scp_pull_request(sshc->scp_session);
  सचे(rc != SSH_SCP_REQUEST_NEWFILE) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_सन्देशः);
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  सचे(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क myssh_in_TRANS_INIT(संरचना Curl_easy *data, संरचना ssh_conn *sshc,
                               संरचना SSHPROTO *sshp)
{
  CURLcode result;
  पूर्णाङ्क rc = 0;
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  सचे(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    पटिनिवत्त 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  सचे(data->state.upload) {
    सचे(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  अञ्ञथा {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  सचे(!sshc->scp_session) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_सन्देशः);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  पटिनिवत्त rc;
}

स्थिर शून्य sshc_cleanup(संरचना ssh_conn *sshc)
{
  सचे(sshc->initialized) {
    सचे(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = रिक्त;
    }
    सचे(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = रिक्त;
    }
    सचे(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = रिक्त;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    सचे(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = रिक्त;
    }
    सचे(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = रिक्त;
    }
    सचे(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = रिक्त;
    }
    सचे(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = रिक्त;
    }
    सचे(sshc->privkey) {
      ssh_कुञ्चिका_free(sshc->privkey);
      sshc->privkey = रिक्त;
    }
    सचे(sshc->pubkey) {
      ssh_कुञ्चिका_free(sshc->pubkey);
      sshc->pubkey = रिक्त;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_कुञ्चिका);
    curlx_safefree(sshc->priv_कुञ्चिका);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE_SETSTAT(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  सचे(rc == SSH_AGAIN)
    पटिनिवत्त rc;
  सचे(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    पटिनिवत्त rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE_SYMLINK(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  सचे(rc == SSH_AGAIN)
    पटिनिवत्त rc;
  सचे(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    पटिनिवत्त rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE_MKDIR(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  सचे(rc == SSH_AGAIN)
    पटिनिवत्त rc;
  सचे(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    पटिनिवत्त rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE_RENAME(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  सचे(rc == SSH_AGAIN)
    पटिनिवत्त rc;
  सचे(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    पटिनिवत्त rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE_RMDIR(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  सचे(rc == SSH_AGAIN)
    पटिनिवत्त rc;
  सचे(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    पटिनिवत्त rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_QUOTE_UNLINK(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  सचे(rc == SSH_AGAIN)
    पटिनिवत्त rc;
  सचे(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    पटिनिवत्त rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_GETINFO(संरचना Curl_easy *data,
                                 संरचना ssh_conn *sshc)
{
  सचे(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  अञ्ञथा
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_FILETIME(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc,
                                  संरचना SSHPROTO *sshp)
{
  sftp_attributes attrs;
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  सचे(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_TRANS_INIT(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  सचे(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  अञ्ञथा {
    size_t path_len = strlen(sshp->path);

    सचे(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    अञ्ञथा
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_CREATE_DIRS_INIT(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  सचे(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_CREATE_DIRS(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc,
                                     संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  सचे(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    पटिनिवत्त SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SFTP_CREATE_DIRS_MKDIR(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc,
                                           संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क rc;
  पूर्णाङ्क err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  सचे(rc == SSH_AGAIN)
    पटिनिवत्त rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  सचे(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    सचे((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      पटिनिवत्त myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  पटिनिवत्त rc;
}

स्थिर पूर्णाङ्क myssh_in_SCP_UPLOAD_INIT(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क rc;
  सचे(!sshp)
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  सचे(rc != SSH_OK) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_सन्देशः);
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_प्रक्षिप_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (पूर्णाङ्क)data->set.new_file_perms);

  सचे(rc != SSH_OK) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_सन्देशः);
    पटिनिवत्त myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SCP_DOWNLOAD_INIT(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = ssh_scp_init(sshc->scp_session);
  सचे(rc != SSH_OK) {
    ध्रुव वर्ण *err_सन्देशः = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_सन्देशः);
    पटिनिवत्त myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SCP_DONE(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  सचे(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  अञ्ञथा
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SCP_SEND_EOF(संरचना Curl_easy *data,
                                 संरचना ssh_conn *sshc)
{
  सचे(sshc->scp_session) {
    पूर्णाङ्क rc = ssh_scp_close(sshc->scp_session);
    सचे(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      पटिनिवत्त SSH_AGAIN;
    }
    सचे(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर पूर्णाङ्क myssh_in_SCP_CHANNEL_FREE(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc)
{
  सचे(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = रिक्त;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  पटिनिवत्त SSH_NO_ERROR;
}

स्थिर CURLcode myssh_in_SESSION_FREE(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  memset(sshc, 0, परिमाण(संरचना ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
स्थिर CURLcode myssh_statemachine(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc,
                                   संरचना SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  पूर्णाङ्क rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  करोहि {

    विकल्प(sshc->state) {
    पक्ष SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    पक्ष SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      सचे(rc)
        भङ्ग;
      FALLTHROUGH();
    पक्ष SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      सचे(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        भङ्ग;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    पक्ष SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_कुञ्चिका_INIT:
      rc = myssh_in_AUTH_कुञ्चिका_INIT(data, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_कुञ्चिका:
      rc = myssh_in_AUTH_कुञ्चिका(data, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      भङ्ग;
    पक्ष SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      भङ्ग;
    पक्ष SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      भङ्ग;
    पक्ष SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      भङ्ग;
    पक्ष SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      सचे(rc)
        भङ्ग;
      FALLTHROUGH();
    पक्ष SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      भङ्ग;
    पक्ष SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      भङ्ग;
    पक्ष SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      भङ्ग;
    पक्ष SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    पक्ष SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    पक्ष SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      भङ्ग;
    पक्ष SSH_QUIT:
    स्वभाव:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      भङ्ग;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } याव((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  सचे(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  सचे(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (पूर्णाङ्क)result, *block);
  पटिनिवत्त result;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode myssh_multi_statemach(संरचना Curl_easy *data,
                                      bool *done)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  सचे(!sshc || !sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  पटिनिवत्त result;
}

स्थिर CURLcode myssh_block_statemach(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc,
                                      संरचना SSHPROTO *sshp,
                                      bool disconnect)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  याव((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t वामः_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
    सचे(result)
      भङ्ग;

    सचे(!disconnect) {
      result = Curl_pgrsCheck(data);
      सचे(result)
        भङ्ग;

      वामः_ms = Curl_timeleft_ms(data);
      सचे(वामः_ms < 0) {
        failf(data, "Operation timed out");
        पटिनिवत्त CURLE_OPERATION_TIMEDOUT;
      }
    }

    सचे(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (शून्य)SOCKET_READABLE(fd_read,
                            (!वामः_ms || वामः_ms > 1000) ? 1000 : वामः_ms);
    }
  }

  पटिनिवत्त result;
}

स्थिर शून्य myssh_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना SSHPROTO *sshp = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

स्थिर शून्य myssh_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना ssh_conn *sshc = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
स्थिर CURLcode myssh_setup_connection(संरचना Curl_easy *data,
                                       संरचना connectdata *conn)
{
  संरचना SSHPROTO *sshp;
  संरचना ssh_conn *sshc;

  sshc = curlx_calloc(1, परिमाण(*sshc));
  सचे(!sshc)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  सचे(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, परिमाण(*sshp));
  सचे(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  पटिनिवत्त Curl_ssh_setup_pkey(data, sshc);
}

स्थिर Curl_recv scp_recv, sftp_recv;
स्थिर Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
स्थिर CURLcode myssh_connect(संरचना Curl_easy *data, bool *done)
{
  CURLcode result;
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णाङ्क rc;

  सचे(!sshc || !ssh)
    पटिनिवत्त CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  सचे(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  अञ्ञथा {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  सचे(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  सचे(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, रिक्त);
  सचे(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  सचे(rc != SSH_OK) {
    failf(data, "Could not set socket");
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  सचे(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    सचे(rc != SSH_OK) {
      failf(data, "Could not set user");
      पटिनिवत्त CURLE_FAILED_INIT;
    }
  }

  सचे(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    सचे(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    सचे(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      पटिनिवत्त CURLE_FAILED_INIT;
    }
  }

  सचे(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    सचे(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      पटिनिवत्त CURLE_FAILED_INIT;
    }
  }

  सचे(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    सचे(rc != SSH_OK) {
      failf(data, "Could not set compression");
      पटिनिवत्त CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = रिक्त;
  sshc->pubkey = रिक्त;

  सचे(sshc->pub_कुञ्चिका) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_कुञ्चिका, &sshc->pubkey);
    सचे(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      पटिनिवत्त CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  पटिनिवत्त result;
}

/* called from multi.c while DOing */
स्थिर CURLcode scp_doing(संरचना Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  सचे(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  पटिनिवत्त result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

स्थिर CURLcode scp_perform(संरचना Curl_easy *data,
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  सचे(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  पटिनिवत्त result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode scp_disconnect(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (शून्य)dead_connection;

  सचे(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  पटिनिवत्त result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
स्थिर CURLcode myssh_done(संरचना Curl_easy *data,
                           संरचना ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  सचे(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
  अञ्ञथा
    result = status;

  सचे(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    पटिनिवत्त CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  पटिनिवत्त result;
}

स्थिर CURLcode scp_done(संरचना Curl_easy *data, CURLcode status,
                         bool premature)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (शून्य)premature;

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;
  सचे(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  पटिनिवत्त myssh_done(data, sshc, status);
}

स्थिर CURLcode scp_send(संरचना Curl_easy *data, int8_t sockindex,
                         ध्रुव uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  पूर्णाङ्क rc;
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  (शून्य)eos;
  *pnwritten = 0;

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#सचे 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  सचे(rc == SSH_AGAIN)
    पटिनिवत्त CURLE_AGAIN;
  अञ्ञथा
#endif
  सचे(rc != SSH_OK)
    पटिनिवत्त CURLE_SSH;

  *pnwritten = len;
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode scp_recv(संरचना Curl_easy *data, int8_t sockindex,
                         वर्ण *mem, size_t len, size_t *pnread)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  पूर्णाङ्क nread;

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  सचे(nread == SSH_ERROR)
    पटिनिवत्त CURLE_SSH;
#सचे 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  सचे(nread == SSH_AGAIN)
    पटिनिवत्त CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  पटिनिवत्त CURLE_OK;
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

स्थिर CURLcode sftp_perform(संरचना Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  सचे(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  पटिनिवत्त result;
}

/* called from multi.c while DOing */
स्थिर CURLcode sftp_doing(संरचना Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  सचे(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  पटिनिवत्त result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode sftp_disconnect(संरचना Curl_easy *data,
                                संरचना connectdata *conn,
                                bool dead_connection)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (शून्य)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  सचे(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
  पटिनिवत्त result;
}

स्थिर CURLcode sftp_done(संरचना Curl_easy *data, CURLcode status,
                          bool premature)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;
  सचे(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    सचे(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  पटिनिवत्त myssh_done(data, sshc, status);
}

/* return number of sent bytes */
स्थिर CURLcode sftp_send(संरचना Curl_easy *data, int8_t sockindex,
                          ध्रुव uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (शून्य)sockindex;
  (शून्य)eos;
  *pnwritten = 0;

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

#सचे LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  विकल्प(sshc->sftp_send_state) {
  पक्ष 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    सचे(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      पटिनिवत्त CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  पक्ष 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    सचे(nwrite == SSH_AGAIN)
      पटिनिवत्त CURLE_AGAIN;
    अञ्ञथा सचे(nwrite < 0)
      पटिनिवत्त CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    पटिनिवत्त CURLE_OK;
  स्वभाव:
    /* we never reach here */
    पटिनिवत्त CURLE_SEND_ERROR;
  }
#अञ्ञथा
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  सचे(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#सचे 0 /* not returned by libssh on write */
  सचे(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  अञ्ञथा
#endif
  सचे(nwrite < 0)
    पटिनिवत्त CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  पटिनिवत्त CURLE_OK;
#endif
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
स्थिर CURLcode sftp_recv(संरचना Curl_easy *data, int8_t sockindex,
                          वर्ण *mem, size_t len, size_t *pnread)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (शून्य)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  विकल्प(sshc->sftp_recv_state) {
  पक्ष 0:
#सचे LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    सचे(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      पटिनिवत्त CURLE_RECV_ERROR;
    }
#अञ्ञथा
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    सचे(sshc->sftp_file_index < 0)
      पटिनिवत्त CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  पक्ष 1:
    sshc->sftp_recv_state = 1;

#सचे LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#अञ्ञथा
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    सचे(nread == SSH_AGAIN)
      पटिनिवत्त CURLE_AGAIN;
    अञ्ञथा सचे(nread < 0)
      पटिनिवत्त CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    पटिनिवत्त CURLE_OK;

  स्वभाव:
    /* we never reach here */
    पटिनिवत्त CURLE_RECV_ERROR;
  }
}

स्थिर CURLcode myssh_do_it(संरचना Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  सचे(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  अञ्ञथा
    result = sftp_perform(data, &connected, done);

  पटिनिवत्त result;
}

CURLcode Curl_ssh_init(शून्य)
{
  सचे(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    पटिनिवत्त CURLE_FAILED_INIT;
  }
  पटिनिवत्त CURLE_OK;
}

शून्य Curl_ssh_cleanup(शून्य)
{
  (शून्य)ssh_finalize();
}

शून्य Curl_ssh_version(वर्ण *buffer, size_t buflen)
{
  (शून्य)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_scp = {
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
ध्रुव संरचना Curl_protocol Curl_protocol_sftp = {
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
