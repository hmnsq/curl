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
#समावेश "curl_setup.h"

#अगर_घोषित USE_LIBSSH

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "transfer.h"
#समावेश "vssh/ssh.h"
#समावेश "url.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "parsedate.h"          /* for the week day and month names */
#समावेश "curlx/strparse.h"
#समावेश "multiif.h"
#समावेश "select.h"
#समावेश "vssh/vssh.h"
#समावेश "curlx/base64.h" /* for curlx_base64_encode() */

#अगर_घोषित HAVE_UNISTD_H
#समावेश <unistd.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_FCNTL_H
#समावेश <fcntl.h>
#पूर्ण_अगर

/* A recent macro provided by libssh. Or make our own. */
#अगर_अघोषित SSH_STRING_FREE_CHAR
#घोषणा SSH_STRING_FREE_CHAR(x) \
  करो {                          \
    अगर(x) {                     \
      ssh_string_free_char(x);  \
      x = शून्य;                 \
    }                           \
  } जबतक(0)
#पूर्ण_अगर

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#अगर_अघोषित SSH_S_IFMT
#घोषणा SSH_S_IFMT   00170000
#पूर्ण_अगर
#अगर_अघोषित SSH_S_IFLNK
#घोषणा SSH_S_IFLNK  0120000
#पूर्ण_अगर

अटल CURLcode sftp_error_to_CURLE(पूर्णांक err)
{
  चयन(err) {
  हाल SSH_FX_OK:
    वापस CURLE_OK;

  हाल SSH_FX_NO_SUCH_FILE:
  हाल SSH_FX_NO_SUCH_PATH:
    वापस CURLE_REMOTE_FILE_NOT_FOUND;

  हाल SSH_FX_PERMISSION_DENIED:
  हाल SSH_FX_WRITE_PROTECT:
    वापस CURLE_REMOTE_ACCESS_DENIED;

  हाल SSH_FX_FILE_ALREADY_EXISTS:
    वापस CURLE_REMOTE_FILE_EXISTS;

  शेष:
    अवरोध;
  }

  वापस CURLE_SSH;
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
अटल पूर्णांक myssh_is_known(काष्ठा Curl_easy *तथ्य, काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc;
  ssh_कुंजी pubkey;
  अचिन्हित अक्षर *hash_sha256 = शून्य;
  माप_प्रकार hlen_sha256;
  अचिन्हित अक्षर *hash_md5 = शून्य;
  माप_प्रकार hlen_md5;
  अक्षर *found_base64 = शून्य;
  अक्षर *known_base64 = शून्य;
  पूर्णांक vstate;
  क्रमागत curl_khmatch keymatch;
  काष्ठा curl_khkey foundkey;
  काष्ठा curl_khkey *knownkeyp = शून्य;
  curl_sshkeycallback func = तथ्य->set.ssh_keyfunc;
  काष्ठा ssh_knownhosts_entry *knownhostsentry = शून्य;
  काष्ठा curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  अगर(rc != SSH_OK)
    वापस rc;

  अगर(CURL_EASY_STR(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_SHA256)) {
    स्थिर अक्षर *pubkey_sha256 =
      CURL_EASY_STR(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_SHA256);
    अक्षर *fingerprint_b64 = शून्य;
    माप_प्रकार fingerprint_b64_len;
    माप_प्रकार pub_pos = 0;
    माप_प्रकार b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    अगर(rc != SSH_OK || hlen_sha256 != 32) {
      failf(तथ्य, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      जाओ cleanup;
    }

    अगर(curlx_base64_encode((स्थिर uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      जाओ cleanup;
    }

    infof(तथ्य, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    जबतक((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    जबतक((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    अगर((pub_pos != b64_pos) ||
       म_भेदन(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(तथ्य,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      जाओ cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    जाओ cleanup;
  }

  अगर(CURL_EASY_STR(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_MD5)) {
    स्थिर अक्षर *pubkey_md5 =
      CURL_EASY_STR(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_MD5);
    अक्षर md5buffer[33];
    पूर्णांक i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    अगर(rc != SSH_OK || hlen_md5 != 16) {
      failf(तथ्य,
            "Denied establishing ssh session: MD5 fingerprint not available");
      जाओ cleanup;
    }

    क्रम(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(तथ्य, "SSH MD5 fingerprint: %s", md5buffer);

    अगर(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(तथ्य,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      जाओ cleanup;
    }

    rc = SSH_OK;
    जाओ cleanup;
  }

  अगर(CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    अगर(knownhostsentry) {
      अगर(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        अगर(rc != SSH_OK) {
          जाओ cleanup;
        }
        knownkey.कुंजी = known_base64;
        knownkey.len = म_माप(known_base64);

        चयन(ssh_कुंजी_type(knownhostsentry->publickey)) {
        हाल SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          अवरोध;
        हाल SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          अवरोध;
        हाल SSH_KEYTYPE_ECDSA:
        हाल SSH_KEYTYPE_ECDSA_P256:
        हाल SSH_KEYTYPE_ECDSA_P384:
        हाल SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          अवरोध;
        हाल SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          अवरोध;
        हाल SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          अवरोध;
        शेष:
          rc = SSH_ERROR;
          जाओ cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    चयन(vstate) {
    हाल SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      अवरोध;
    हाल SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      अवरोध;
    हाल SSH_KNOWN_HOSTS_NOT_FOUND:
    हाल SSH_KNOWN_HOSTS_UNKNOWN:
    हाल SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      अवरोध;
    शेष:
      keymatch = CURLKHMATCH_MISMATCH;
      अवरोध;
    }

    अगर(func) { /* use callback to determine action */
      काष्ठा Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      अगर(rc != SSH_OK)
        जाओ cleanup;

      foundkey.कुंजी = found_base64;
      foundkey.len = म_माप(found_base64);

      चयन(ssh_कुंजी_type(pubkey)) {
      हाल SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        अवरोध;
      हाल SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        अवरोध;
      हाल SSH_KEYTYPE_ECDSA:
      हाल SSH_KEYTYPE_ECDSA_P256:
      हाल SSH_KEYTYPE_ECDSA_P384:
      हाल SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        अवरोध;
      हाल SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        अवरोध;
      हाल SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        अवरोध;
      शेष:
        rc = SSH_ERROR;
        जाओ cleanup;
      }

      CURL_CBAPI_START(&guard, तथ्य, easy_ssh_keyfunc);
      rc = func(तथ्य, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, तथ्य->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      चयन(rc) {
      हाल CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        अगर(rc != SSH_OK) {
          जाओ cleanup;
        }
        अवरोध;
      हाल CURLKHSTAT_FINE:
        अवरोध;
      शेष: /* REJECT/DEFER */
        rc = SSH_ERROR;
        जाओ cleanup;
      }
    }
    अन्यथा {
      अगर(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        जाओ cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  अगर(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    मुक्त(found_base64); /* allocated by libssh, deallocate with system free */
  }
  अगर(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    मुक्त(known_base64); /* allocated by libssh, deallocate with system free */
  }
  अगर(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  अगर(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_कुंजी_free(pubkey);
  अगर(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  वापस rc;
}

अटल पूर्णांक myssh_to_ERROR(काष्ठा Curl_easy *तथ्य,
                          काष्ठा ssh_conn *sshc,
                          CURLcode परिणाम)
{
  myssh_to(तथ्य, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = परिणाम;
  वापस SSH_ERROR;
}

अटल पूर्णांक myssh_to_SFTP_CLOSE(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ssh_conn *sshc)
{
  myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  वापस SSH_ERROR;
}

अटल पूर्णांक myssh_to_PASSWD_AUTH(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ssh_conn *sshc)
{
  अगर(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(तथ्य, sshc, SSH_AUTH_PASS_INIT);
    वापस SSH_OK;
  }
  वापस myssh_to_ERROR(तथ्य, sshc, CURLE_LOGIN_DENIED);
}

अटल पूर्णांक myssh_to_कुंजी_AUTH(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ssh_conn *sshc)
{
  अगर(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(तथ्य, sshc, SSH_AUTH_कुंजी_INIT);
    वापस SSH_OK;
  }
  वापस myssh_to_PASSWD_AUTH(तथ्य, sshc);
}

अटल पूर्णांक myssh_to_GSSAPI_AUTH(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ssh_conn *sshc)
{
  अगर(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(तथ्य, sshc, SSH_AUTH_GSSAPI);
    वापस SSH_OK;
  }
  वापस myssh_to_कुंजी_AUTH(तथ्य, sshc);
}

अटल पूर्णांक myssh_in_SFTP_READDIR_INIT(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ssh_conn *sshc,
                                      काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);
  Curl_pgrs_Set_Download_आकार(तथ्य, -1);
  अगर(तथ्य->req.no_body) {
    myssh_to(तथ्य, sshc, SSH_STOP);
    वापस SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  अगर(!sshc->sftp_dir) {
    failf(तथ्य, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    वापस myssh_to_SFTP_CLOSE(तथ्य, sshc);
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_READDIR);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_READDIR(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ssh_conn *sshc,
                                 काष्ठा SSHPROTO *sshp)
{
  CURLcode परिणाम = CURLE_OK;

  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  अगर(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  अगर(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = म_माप(sshc->readdir_filename);

    अगर(तथ्य->set.list_only) {
      अक्षर *tmp_पंक्ति;

      tmp_पंक्ति = curl_maprintf("%s\n", sshc->readdir_filename);
      अगर(!tmp_पंक्ति) {
        myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        वापस SSH_ERROR;
      }
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                                 tmp_पंक्ति, sshc->readdir_len + 1);
      curlx_free(tmp_पंक्ति);

      अगर(परिणाम) {
        myssh_to(तथ्य, sshc, SSH_STOP);
        sshc->actualcode = परिणाम;
        वापस SSH_NO_ERROR;
      }
    }
    अन्यथा {
      अगर(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(तथ्य, sshc, SSH_STOP);
        वापस SSH_ERROR;
      }

      अगर((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        अगर(!sshc->readdir_linkPath) {
          myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          वापस SSH_ERROR;
        }

        myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_LINK);
        वापस SSH_NO_ERROR;
      }
      myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_BOTTOM);
      वापस SSH_NO_ERROR;
    }
  }
  अन्यथा अगर(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_DONE);
  }
  अन्यथा {
    failf(तथ्य, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    वापस myssh_to_SFTP_CLOSE(तथ्य, sshc);
  }
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_READDIR_LINK(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ssh_conn *sshc)
{
  अगर(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  अगर(!sshc->readdir_link_attrs) {
    failf(तथ्य, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    वापस myssh_to_SFTP_CLOSE(तथ्य, sshc);
  }

  अगर(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    अगर(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    अन्यथा
      sshc->readdir_len = म_माप(sshc->readdir_tmp);
    sshc->readdir_longentry = शून्य;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  अन्यथा {
    sshc->readdir_len = म_माप(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  अगर(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    वापस SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = शून्य;
  sshc->readdir_filename = शून्य;
  sshc->readdir_longentry = शून्य;

  myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_BOTTOM);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_READDIR_BOTTOM(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ssh_conn *sshc)
{
  CURLcode परिणाम;

  अगर(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    परिणाम = CURLE_OUT_OF_MEMORY;
  अन्यथा
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = शून्य;

  अगर(परिणाम) {
    myssh_to(तथ्य, sshc, SSH_STOP);
    sshc->actualcode = परिणाम;
  }
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_SFTP_READDIR);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_READDIR_DONE(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = शून्य;

  /* no data to transfer */
  Curl_xfer_setup_nop(तथ्य);
  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस SSH_NO_ERROR;
}

अटल व्योम myssh_quote_error(काष्ठा Curl_easy *तथ्य, काष्ठा ssh_conn *sshc,
                              स्थिर अक्षर *cmd)
{
  अगर(cmd)
    failf(तथ्य, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_QUOTE_STATVFS(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  अगर(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(तथ्य, sshc, "statvfs");
    वापस SSH_OK;
  }
  अन्यथा अगर(statvfs) {
#अगर_घोषित _MSC_VER
#घोषणा CURL_LIBSSH_VFS_आकार_MASK "I64u"
#या_अगर defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#घोषणा CURL_LIBSSH_VFS_आकार_MASK "llu"
#अन्यथा
#घोषणा CURL_LIBSSH_VFS_आकार_MASK PRIu64
#पूर्ण_अगर
    CURLcode परिणाम = CURLE_OK;
    अक्षर *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_files: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_आकार_MASK "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_आकार_MASK "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_blocks, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    अगर(!tmp)
      परिणाम = CURLE_OUT_OF_MEMORY;

    अगर(!परिणाम) {
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, tmp, म_माप(tmp));
      curlx_free(tmp);
    }
    अगर(परिणाम) {
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
    }
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस SSH_OK;
}

अटल पूर्णांक myssh_auth_interactive(काष्ठा connectdata *conn,
                                  काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc;
  पूर्णांक nprompts;

restart:
  चयन(sshc->kbd_state) {
  हाल 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, शून्य, शून्य);
    अगर(rc == SSH_AUTH_AGAIN)
      वापस SSH_AGAIN;

    अगर(rc != SSH_AUTH_INFO)
      वापस SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    अगर(nprompts != 1)
      वापस SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    अगर(rc < 0)
      वापस SSH_ERROR;

    FALLTHROUGH();
  हाल 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, शून्य, शून्य);
    अगर(rc == SSH_AUTH_AGAIN)
      वापस SSH_AGAIN;
    अन्यथा अगर(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    अन्यथा अगर(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      अगर(nprompts)
        वापस SSH_ERROR;

      sshc->kbd_state = 2;
      जाओ restart;
    }
    अन्यथा
      rc = SSH_ERROR;
    अवरोध;
  हाल 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, शून्य, शून्य);
    अगर(rc == SSH_AUTH_AGAIN)
      वापस SSH_AGAIN;
    अन्यथा अगर(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    अन्यथा
      rc = SSH_ERROR;

    अवरोध;
  शेष:
    वापस SSH_ERROR;
  }

  sshc->kbd_state = 0;
  वापस rc;
}

अटल व्योम myssh_state_init(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#अगर 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#पूर्ण_अगर

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(तथ्य, sshc, SSH_S_STARTUP);
}

अटल व्योम myssh_block2waitfor(काष्ठा connectdata *conn,
                                काष्ठा ssh_conn *sshc,
                                bool block)
{
  (व्योम)conn;
  अगर(block) {
    पूर्णांक dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  अन्यथा
    sshc->waitfor = 0;
}

अटल पूर्णांक myssh_in_S_STARTUP(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ssh_conn *sshc)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  पूर्णांक rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  अगर(rc == SSH_AGAIN) {
    CURL_TRC_SSH(तथ्य, "connect -> EAGAIN");
  }
  अन्यथा अगर(rc != SSH_OK) {
    failf(तथ्य, "Failure establishing ssh session");
    rc = myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);
  }
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_HOSTKEY);

  वापस rc;
}

अटल पूर्णांक myssh_in_AUTHLIST(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, शून्य);
  अगर(rc == SSH_AUTH_AGAIN)
    वापस SSH_AGAIN;

  अगर(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(तथ्य, "Authenticated with none");
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
    वापस rc;
  }
  अन्यथा अगर(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(तथ्य, sshc, CURLE_LOGIN_DENIED);
    वापस rc;
  }

  sshc->auth_methods =
    (अचिन्हित पूर्णांक)ssh_userauth_list(sshc->ssh_session, शून्य);
  अगर(sshc->auth_methods)
    infof(तथ्य, "SSH authentication methods available: %s%s%s%s",
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
  अगर((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_कुंजी && sshc->priv_कुंजी[0]) ||
      (तथ्य->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(तथ्य, sshc, SSH_AUTH_PKEY_INIT);
    infof(तथ्य, "Authentication using SSH public key file");
  }
  अन्यथा अगर(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(तथ्य, sshc, SSH_AUTH_GSSAPI);
  }
  अन्यथा अगर(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(तथ्य, sshc, SSH_AUTH_कुंजी_INIT);
  }
  अन्यथा अगर(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(तथ्य, sshc, SSH_AUTH_PASS_INIT);
  }
  अन्यथा {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(तथ्य, sshc, CURLE_LOGIN_DENIED);
  }
  वापस rc;
}

अटल पूर्णांक myssh_in_AUTH_PKEY_INIT(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc;
  अगर(!(तथ्य->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(तथ्य, sshc);
    वापस rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  अगर(sshc->priv_कुंजी && sshc->priv_कुंजी[0]) {
    अगर(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, शून्य, sshc->pubkey);
      अगर(rc == SSH_AUTH_AGAIN)
        वापस SSH_AGAIN;

      अगर(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(तथ्य, sshc);
        वापस rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_कुंजी,
                                     sshc->passphrase, शून्य,
                                     शून्य, &sshc->privkey);
    अगर(rc != SSH_OK) {
      failf(तथ्य, "Could not load private key file %s", sshc->priv_कुंजी);
      rc = myssh_to_ERROR(तथ्य, sshc, CURLE_LOGIN_DENIED);
      वापस rc;
    }

    myssh_to(तथ्य, sshc, SSH_AUTH_PKEY);
  }
  अन्यथा {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, शून्य,
                                     sshc->passphrase);
    अगर(rc == SSH_AUTH_AGAIN)
      वापस SSH_AGAIN;

    अगर(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(तथ्य, "Completed public key authentication");
      myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
      वापस rc;
    }

    rc = myssh_to_GSSAPI_AUTH(तथ्य, sshc);
  }
  वापस rc;
}

अटल पूर्णांक myssh_in_AUTH_PKEY(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = ssh_userauth_publickey(sshc->ssh_session, शून्य, sshc->privkey);
  अगर(rc == SSH_AUTH_AGAIN)
    वापस SSH_AGAIN;
  अन्यथा अगर(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(तथ्य, "Completed public key authentication");
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
    वापस SSH_OK;
  }
  अन्यथा {
    infof(तथ्य, "Failed public key authentication (rc: %d)", rc);
    वापस myssh_to_GSSAPI_AUTH(तथ्य, sshc);
  }
}

अटल पूर्णांक myssh_in_AUTH_GSSAPI(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc;
  अगर(!(तथ्य->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    वापस myssh_to_कुंजी_AUTH(तथ्य, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  अगर(rc == SSH_AUTH_AGAIN)
    वापस SSH_AGAIN;

  अगर(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(तथ्य, "Completed gssapi authentication");
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
    वापस SSH_OK;
  }

  वापस myssh_to_कुंजी_AUTH(तथ्य, sshc);
}

अटल पूर्णांक myssh_in_AUTH_कुंजी_INIT(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ssh_conn *sshc)
{
  अगर(तथ्य->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(तथ्य, sshc, SSH_AUTH_कुंजी);
    वापस SSH_NO_ERROR;
  }
  वापस myssh_to_PASSWD_AUTH(तथ्य, sshc);
}

अटल पूर्णांक myssh_in_AUTH_कुंजी(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  पूर्णांक rc = myssh_auth_interactive(तथ्य->conn, sshc);
  अगर(rc == SSH_AGAIN)
    वापस rc;
  अन्यथा अगर(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(तथ्य, "completed keyboard interactive authentication");
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
    वापस SSH_NO_ERROR;
  }
  अन्यथा
    वापस myssh_to_PASSWD_AUTH(तथ्य, sshc);
}

अटल पूर्णांक myssh_in_AUTH_PASS_INIT(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ssh_conn *sshc)
{
  अगर(!(तथ्य->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_LOGIN_DENIED);
  myssh_to(तथ्य, sshc, SSH_AUTH_PASS);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_AUTH_PASS(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = ssh_userauth_password(sshc->ssh_session, शून्य,
                                 Curl_creds_passwd(तथ्य->conn->creds));
  अगर(rc == SSH_AUTH_AGAIN)
    वापस SSH_AGAIN;
  अन्यथा अगर(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(तथ्य, "Completed password authentication");
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
    वापस SSH_NO_ERROR;
  }
  वापस myssh_to_ERROR(तथ्य, sshc, CURLE_LOGIN_DENIED);
}

अटल पूर्णांक myssh_in_AUTH_DONE(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ssh_conn *sshc)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  अगर(!sshc->authed) {
    failf(तथ्य, "Authentication failure");
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(तथ्य, "Authentication complete");
  Curl_pgrsTime(तथ्य, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  अगर(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(तथ्य, sshc, SSH_SFTP_INIT);
    वापस SSH_NO_ERROR;
  }
  infof(तथ्य, "SSH CONNECT phase done");
  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_UPLOAD_INIT(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ssh_conn *sshc,
                                काष्ठा SSHPROTO *sshp)
{
  पूर्णांक flags;
  पूर्णांक rc = 0;

  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);

  अगर(तथ्य->state.resume_from) {
    sftp_attributes attrs;

    अगर(तथ्य->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      अगर(attrs) {
        curl_off_t आकार = attrs->आकार;
        sftp_attributes_free(attrs);
        अगर(आकार < 0) {
          failf(तथ्य, "Bad file size (%" FMT_OFF_T ")", आकार);
          rc = myssh_to_ERROR(तथ्य, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          वापस rc;
        }
        तथ्य->state.resume_from = आकार;
      }
      अन्यथा {
        तथ्य->state.resume_from = 0;
      }
    }
  }

  अगर(तथ्य->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  अन्यथा अगर(तथ्य->state.resume_from > 0) {
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

  अगर(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)तथ्य->set.new_file_perms);
  अगर(!sshc->sftp_file) {
    पूर्णांक err = sftp_get_error(sshc->sftp_session);

    अगर((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       तथ्य->set.ftp_create_missing_dirs &&
       (म_माप(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(तथ्य, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      वापस rc;
    }
    अन्यथा {
      rc = myssh_to_SFTP_CLOSE(तथ्य, sshc);
      वापस rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  अगर(तथ्य->state.resume_from > 0 && !तथ्य->set.remote_append) {
    पूर्णांक seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    अगर(तथ्य->set.seek_func) {
      काष्ठा Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, तथ्य, easy_seek_func);
      seekerr = तथ्य->set.seek_func(तथ्य->set.seek_client,
                                    तथ्य->state.resume_from, शुरू_से);
      CURL_CBAPI_END(&guard);
    }

    अगर(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      अगर(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(तथ्य, "Could not seek stream");
        rc = myssh_to_ERROR(तथ्य, sshc, CURLE_FTP_COULDNT_USE_REST);
        वापस rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      करो {
        अक्षर scratch[4 * 1024];
        माप_प्रकार readthisamountnow =
          (तथ्य->state.resume_from - passed >
           (curl_off_t)माप(scratch)) ?
          माप(scratch) : curlx_sotouz(तथ्य->state.resume_from - passed);

        माप_प्रकार actuallyread =
          तथ्य->state.fread_func(scratch, 1,
                                 readthisamountnow, तथ्य->state.in);

        passed += actuallyread;
        अगर((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(तथ्य, "Failed to read data");
          rc = myssh_to_ERROR(तथ्य, sshc, CURLE_FTP_COULDNT_USE_REST);
          वापस rc;
        }
      } जबतक(passed < तथ्य->state.resume_from);
    }

    /* now, decrease the size of the read */
    अगर(तथ्य->state.infilesize > 0) {
      अगर(तथ्य->state.resume_from > तथ्य->state.infilesize) {
        failf(तथ्य, "Resume point beyond size");
        वापस myssh_to_ERROR(तथ्य, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      तथ्य->state.infilesize -= तथ्य->state.resume_from;
      तथ्य->req.आकार = तथ्य->state.infilesize;
      Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, तथ्य->state.resume_from);
    अगर(rc) {
      rc = myssh_to_SFTP_CLOSE(तथ्य, sshc);
      वापस rc;
    }
  }
  अगर(तथ्य->state.infilesize > 0) {
    तथ्य->req.आकार = तथ्य->state.infilesize;
    Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);
  }
  /* upload data */
  Curl_xfer_setup_send(तथ्य, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  तथ्य->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(तथ्य);
#अगर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#पूर्ण_अगर
  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस rc;
}

अटल पूर्णांक myssh_in_SFTP_DOWNLOAD_INIT(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc,
                                       काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  अगर(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)तथ्य->set.new_file_perms);
  अगर(!sshc->sftp_file) {
    failf(तथ्य, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    वापस myssh_to_SFTP_CLOSE(तथ्य, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(तथ्य, sshc, SSH_SFTP_DOWNLOAD_STAT);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_DOWNLOAD_STAT(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc)
{
  curl_off_t आकार;
  पूर्णांक rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  अगर(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_आकार) ||
     (attrs->आकार == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    तथ्य->req.आकार = -1;
    तथ्य->req.maxdownload = -1;
    Curl_pgrs_Set_Download_आकार(तथ्य, -1);
    आकार = 0;
    अगर(attrs)
      sftp_attributes_free(attrs);
  }
  अन्यथा {
    आकार = attrs->आकार;

    sftp_attributes_free(attrs);

    अगर(आकार < 0) {
      failf(तथ्य, "Bad file size (%" FMT_OFF_T ")", आकार);
      वापस myssh_to_ERROR(तथ्य, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    अगर(तथ्य->state.use_range) {
      curl_off_t from;
      CURLcode परिणाम = Curl_ssh_range(तथ्य, तथ्य->state.range, आकार,
                                       &from, &आकार);
      अगर(परिणाम)
        वापस myssh_to_ERROR(तथ्य, sshc, परिणाम);

      rc = sftp_seek64(sshc->sftp_file, from);
      अगर(rc)
        वापस myssh_to_SFTP_CLOSE(तथ्य, sshc);
    }
    तथ्य->req.आकार = आकार;
    तथ्य->req.maxdownload = आकार;
    Curl_pgrs_Set_Download_आकार(तथ्य, आकार);
  }

  /* We can resume if we can seek to the resume position */
  अगर(तथ्य->state.resume_from) {
    अगर(तथ्य->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      अगर(आकार < -तथ्य->state.resume_from) {
        failf(तथ्य, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", तथ्य->state.resume_from, आकार);
        वापस myssh_to_ERROR(तथ्य, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      तथ्य->state.resume_from += आकार;
    }
    अन्यथा {
      अगर(आकार < तथ्य->state.resume_from) {
        failf(तथ्य, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              तथ्य->state.resume_from, आकार);
        वापस myssh_to_ERROR(तथ्य, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    तथ्य->req.आकार = आकार - तथ्य->state.resume_from;
    तथ्य->req.maxdownload = आकार - तथ्य->state.resume_from;
    Curl_pgrs_Set_Download_आकार(तथ्य, आकार - तथ्य->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, तथ्य->state.resume_from);
    अगर(rc)
      वापस myssh_to_SFTP_CLOSE(तथ्य, sshc);
  }

  /* Setup the actual download */
  अगर(तथ्य->req.आकार == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(तथ्य);
    infof(तथ्य, "File already completely downloaded");
    myssh_to(तथ्य, sshc, SSH_STOP);
    वापस rc;
  }
  Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, तथ्य->req.आकार);

  /* not set by Curl_xfer_setup to preserve io_flags */
  तथ्य->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(तथ्य, sshc, SSH_STOP);

  वापस rc;
}

अटल पूर्णांक myssh_in_SFTP_CLOSE(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ssh_conn *sshc,
                               काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);
  अगर(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = शून्य;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(तथ्य, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  अगर(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(तथ्य, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_STOP);
  }
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_SHUTDOWN(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#अगर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#पूर्ण_अगर

  अगर(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = शून्य;
  }

  अगर(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = शून्य;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(तथ्य, sshc, SSH_SESSION_DISCONNECT);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_INIT(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  अगर(!sshc->sftp_session) {
    failf(तथ्य, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  अगर(rc != SSH_OK) {
    failf(तथ्य, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    वापस myssh_to_ERROR(तथ्य, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_REALPATH);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_REALPATH(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  अगर(!sshc->homedir)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(तथ्य->state.most_recent_ftp_entrypath);
  तथ्य->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  अगर(!तथ्य->state.most_recent_ftp_entrypath)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(तथ्य, "CONNECT phase done");
  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_QUOTE_INIT(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc,
                                    काष्ठा SSHPROTO *sshp)
{
  CURLcode परिणाम;

  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);

  परिणाम = Curl_getworkingpath(तथ्य, sshc->homedir, &sshp->path);
  अगर(परिणाम) {
    sshc->actualcode = परिणाम;
    myssh_to(तथ्य, sshc, SSH_STOP);
  }
  अन्यथा अगर(तथ्य->set.quote) {
    infof(तथ्य, "Sending quote commands");
    sshc->quote_item = तथ्य->set.quote;
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_SFTP_GETINFO);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_POSTQUOTE_INIT(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ssh_conn *sshc)
{
  अगर(तथ्य->set.postquote) {
    infof(तथ्य, "Sending quote commands");
    sshc->quote_item = तथ्य->set.postquote;
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_STOP);
  }
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक quote_error(काष्ठा Curl_easy *तथ्य,
                       काष्ठा ssh_conn *sshc)
{
  failf(तथ्य, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_QUOTE(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ssh_conn *sshc,
                               काष्ठा SSHPROTO *sshp)
{
  स्थिर अक्षर *cp;
  CURLcode परिणाम;

  /*
   * Support some of the "FTP" commands
   */
  स्थिर अक्षर *cmd = sshc->quote_item->तथ्य;
  sshc->acceptfail = FALSE;

  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  अगर(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  अगर(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    अक्षर *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    अगर(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      वापस SSH_NO_ERROR;
    }
    Curl_debug(तथ्य, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(तथ्य, CURLINFO_HEADER_IN, tmp, म_माप(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, tmp, म_माप(tmp));
    curlx_free(tmp);
    अगर(परिणाम) {
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
    }
    अन्यथा
      myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
    वापस SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = म_अक्षर(cmd, ' ');
  अगर(!cp) {
    failf(तथ्य, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    वापस SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  परिणाम = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_OUT_OF_MEMORY)
      failf(तथ्य, "Out of memory");
    अन्यथा
      failf(तथ्य, "Syntax error: Bad first parameter");
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = परिणाम;
    वापस SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  अगर(!म_भेदन(cmd, "chgrp ", 6) ||
     !म_भेदन(cmd, "chmod ", 6) ||
     !म_भेदन(cmd, "chown ", 6) ||
     !म_भेदन(cmd, "atime ", 6) ||
     !म_भेदन(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_OUT_OF_MEMORY)
        failf(तथ्य, "Out of memory");
      अन्यथा
        failf(तथ्य, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
      वापस SSH_NO_ERROR;
    }
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    sshc->quote_attrs = शून्य;
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_STAT);
    वापस SSH_NO_ERROR;
  }
  अगर(!म_भेदन(cmd, "ln ", 3) ||
     !म_भेदन(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_OUT_OF_MEMORY)
        failf(तथ्य, "Out of memory");
      अन्यथा
        failf(तथ्य, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
      वापस SSH_NO_ERROR;
    }
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_SYMLINK);
    वापस SSH_NO_ERROR;
  }
  अन्यथा अगर(!म_भेदन(cmd, "mkdir ", 6)) {
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    /* create directory */
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_MKDIR);
    वापस SSH_NO_ERROR;
  }
  अन्यथा अगर(!म_भेदन(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    अगर(परिणाम) {
      अगर(परिणाम == CURLE_OUT_OF_MEMORY)
        failf(तथ्य, "Out of memory");
      अन्यथा
        failf(तथ्य, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = परिणाम;
      वापस SSH_NO_ERROR;
    }
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_RENAME);
    वापस SSH_NO_ERROR;
  }
  अन्यथा अगर(!म_भेदन(cmd, "rmdir ", 6)) {
    /* delete directory */
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_RMDIR);
    वापस SSH_NO_ERROR;
  }
  अन्यथा अगर(!म_भेदन(cmd, "rm ", 3)) {
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_UNLINK);
    वापस SSH_NO_ERROR;
  }
#अगर_घोषित HAS_STATVFS_SUPPORT
  अन्यथा अगर(!म_भेदन(cmd, "statvfs ", 8)) {
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_STATVFS);
    वापस SSH_NO_ERROR;
  }
#पूर्ण_अगर

  failf(तथ्य, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_अग्र_QUOTE(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->अग्र;

  अगर(sshc->quote_item) {
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    अगर(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(तथ्य, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    अन्यथा {
      myssh_to(तथ्य, sshc, SSH_SFTP_GETINFO);
    }
  }
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_QUOTE_STAT(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc)
{
  स्थिर अक्षर *cmd = sshc->quote_item->तथ्य;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  अगर(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  अगर(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  अगर(!sshc->quote_attrs) {
    failf(तथ्य, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(तथ्य, sshc, शून्य);
    वापस SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  अगर(!म_भेदन(cmd, "chgrp", 5)) {
    स्थिर अक्षर *p = sshc->quote_path1;
    curl_off_t gid;
    अगर(curlx_str_संख्या(&p, &gid, अच_पूर्णांक_उच्च)) {
      failf(तथ्य, "Syntax error: chgrp gid not a number");
      myssh_quote_error(तथ्य, sshc, शून्य);
      वापस SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  अन्यथा अगर(!म_भेदन(cmd, "chmod", 5)) {
    curl_off_t perms;
    स्थिर अक्षर *p = sshc->quote_path1;
    अगर(curlx_str_octal(&p, &perms, 07777)) {
      failf(तथ्य, "Syntax error: chmod permissions not a number");
      myssh_quote_error(तथ्य, sshc, शून्य);
      वापस SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  अन्यथा अगर(!म_भेदन(cmd, "chown", 5)) {
    स्थिर अक्षर *p = sshc->quote_path1;
    curl_off_t uid;
    अगर(curlx_str_संख्या(&p, &uid, अच_पूर्णांक_उच्च)) {
      failf(तथ्य, "Syntax error: chown uid not a number");
      myssh_quote_error(तथ्य, sshc, शून्य);
      वापस SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  अन्यथा अगर(!म_भेदन(cmd, "atime", 5) ||
          !म_भेदन(cmd, "mtime", 5)) {
    समय_प्रकार date;
    bool fail = FALSE;
    अगर(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(तथ्य, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#अगर SIZEOF_TIME_T > 4
    अन्यथा अगर(date > 0xffffffff) {
      failf(तथ्य, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#पूर्ण_अगर
    अगर(fail) {
      myssh_quote_error(तथ्य, sshc, शून्य);
      वापस SSH_NO_ERROR;
    }
    अगर(!म_भेदन(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    अन्यथा /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_SETSTAT);
  वापस SSH_NO_ERROR;
}

अटल व्योम conn_forget_socket(काष्ठा Curl_easy *तथ्य, int8_t sockindex)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  अगर(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    काष्ठा Curl_cfilter *cf = conn->cfilter[sockindex];
    अगर(cf)
      (व्योम)Curl_conn_cf_cntrl(cf, तथ्य, TRUE, CF_CTRL_FORGET_SOCKET, 0, शून्य);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

अटल व्योम myssh_SESSION_DISCONNECT(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  अगर(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = शून्य;
  }

  अगर(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = शून्य;
  }
  अगर(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = शून्य;
  }

  ssh_disconnect(sshc->ssh_session);
  अगर(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(तथ्य, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
}

अटल पूर्णांक myssh_SSH_SCP_DOWNLOAD(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ssh_conn *sshc)
{
  uint64_t bytecount;
  पूर्णांक rc = ssh_scp_pull_request(sshc->scp_session);
  अगर(rc != SSH_SCP_REQUEST_NEWFILE) {
    स्थिर अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(तथ्य, "%s", err_संदेश);
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  अगर(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_TOO_LARGE);
  तथ्य->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, तथ्य->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  तथ्य->conn->send_idx = 0;

  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस 0;
}

अटल पूर्णांक myssh_in_TRANS_INIT(काष्ठा Curl_easy *तथ्य, काष्ठा ssh_conn *sshc,
                               काष्ठा SSHPROTO *sshp)
{
  CURLcode परिणाम;
  पूर्णांक rc = 0;
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);

  परिणाम = Curl_getworkingpath(तथ्य, sshc->homedir, &sshp->path);
  अगर(परिणाम) {
    sshc->actualcode = परिणाम;
    myssh_to(तथ्य, sshc, SSH_STOP);
    वापस 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  अगर(तथ्य->state.upload) {
    अगर(तथ्य->state.infilesize < 0) {
      failf(तथ्य, "SCP requires a known file size for upload");
      वापस myssh_to_ERROR(तथ्य, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(तथ्य, sshc, SSH_SCP_UPLOAD_INIT);
  }
  अन्यथा {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(तथ्य, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  अगर(!sshc->scp_session) {
    स्थिर अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(तथ्य, "%s", err_संदेश);
    rc = myssh_to_ERROR(तथ्य, sshc, CURLE_UPLOAD_FAILED);
  }
  वापस rc;
}

अटल व्योम sshc_cleanup(काष्ठा ssh_conn *sshc)
{
  अगर(sshc->initialized) {
    अगर(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = शून्य;
    }
    अगर(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = शून्य;
    }
    अगर(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = शून्य;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    अगर(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = शून्य;
    }
    अगर(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = शून्य;
    }
    अगर(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = शून्य;
    }
    अगर(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = शून्य;
    }
    अगर(sshc->privkey) {
      ssh_कुंजी_free(sshc->privkey);
      sshc->privkey = शून्य;
    }
    अगर(sshc->pubkey) {
      ssh_कुंजी_free(sshc->pubkey);
      sshc->pubkey = शून्य;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_कुंजी);
    curlx_safefree(sshc->priv_कुंजी);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

अटल पूर्णांक myssh_in_SFTP_QUOTE_SETSTAT(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  अगर(rc == SSH_AGAIN)
    वापस rc;
  अगर(rc && !sshc->acceptfail) {
    myssh_quote_error(तथ्य, sshc, "setstat");
    वापस rc;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_QUOTE_SYMLINK(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  अगर(rc == SSH_AGAIN)
    वापस rc;
  अगर(rc && !sshc->acceptfail) {
    myssh_quote_error(तथ्य, sshc, "symlink");
    वापस rc;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_QUOTE_MKDIR(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)तथ्य->set.new_directory_perms);
  अगर(rc == SSH_AGAIN)
    वापस rc;
  अगर(rc && !sshc->acceptfail) {
    myssh_quote_error(तथ्य, sshc, "mkdir");
    वापस rc;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_QUOTE_RENAME(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  अगर(rc == SSH_AGAIN)
    वापस rc;
  अगर(rc && !sshc->acceptfail) {
    myssh_quote_error(तथ्य, sshc, "rename");
    वापस rc;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_QUOTE_RMDIR(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  अगर(rc == SSH_AGAIN)
    वापस rc;
  अगर(rc && !sshc->acceptfail) {
    myssh_quote_error(तथ्य, sshc, "rmdir");
    वापस rc;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_QUOTE_UNLINK(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  अगर(rc == SSH_AGAIN)
    वापस rc;
  अगर(rc && !sshc->acceptfail) {
    myssh_quote_error(तथ्य, sshc, "rm");
    वापस rc;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_GETINFO(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ssh_conn *sshc)
{
  अगर(तथ्य->set.get_filetime)
    myssh_to(तथ्य, sshc, SSH_SFTP_FILETIME);
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_SFTP_TRANS_INIT);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_FILETIME(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ssh_conn *sshc,
                                  काष्ठा SSHPROTO *sshp)
{
  sftp_attributes attrs;
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  अगर(attrs) {
    तथ्य->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(तथ्य, sshc, SSH_SFTP_TRANS_INIT);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_TRANS_INIT(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc,
                                    काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);
  अगर(तथ्य->state.upload)
    myssh_to(तथ्य, sshc, SSH_SFTP_UPLOAD_INIT);
  अन्यथा {
    माप_प्रकार path_len = म_माप(sshp->path);

    अगर(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_INIT);
    अन्यथा
      myssh_to(तथ्य, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_CREATE_DIRS_INIT(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा ssh_conn *sshc,
                                          काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);

  अगर(म_माप(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(तथ्य, sshc, SSH_SFTP_CREATE_DIRS);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_CREATE_DIRS(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ssh_conn *sshc,
                                     काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = म_अक्षर(sshc->slash_pos, '/');
  अगर(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(तथ्य, "Creating directory '%s'", sshp->path);
    myssh_to(तथ्य, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    वापस SSH_NO_ERROR;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_UPLOAD_INIT);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SFTP_CREATE_DIRS_MKDIR(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा ssh_conn *sshc,
                                           काष्ठा SSHPROTO *sshp)
{
  पूर्णांक rc;
  पूर्णांक err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)तथ्य->set.new_directory_perms);
  अगर(rc == SSH_AGAIN)
    वापस rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  अगर(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    अगर((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      वापस myssh_to_SFTP_CLOSE(तथ्य, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_CREATE_DIRS);
  वापस rc;
}

अटल पूर्णांक myssh_in_SCP_UPLOAD_INIT(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc,
                                    काष्ठा SSHPROTO *sshp)
{
  पूर्णांक rc;
  अगर(!sshp)
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  अगर(rc != SSH_OK) {
    स्थिर अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(तथ्य, "%s", err_संदेश);
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_धकेलो_file64(sshc->scp_session, sshp->path,
                           (uint64_t)तथ्य->state.infilesize,
                           (पूर्णांक)तथ्य->set.new_file_perms);

  अगर(rc != SSH_OK) {
    स्थिर अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(तथ्य, "%s", err_संदेश);
    वापस myssh_to_ERROR(तथ्य, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(तथ्य, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  तथ्य->conn->recv_idx = FIRSTSOCKET;

  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SCP_DOWNLOAD_INIT(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = ssh_scp_init(sshc->scp_session);
  अगर(rc != SSH_OK) {
    स्थिर अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(तथ्य, "%s", err_संदेश);
    वापस myssh_to_ERROR(तथ्य, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(तथ्य, sshc, SSH_SCP_DOWNLOAD);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SCP_DONE(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ssh_conn *sshc)
{
  अगर(तथ्य->state.upload)
    myssh_to(तथ्य, sshc, SSH_SCP_SEND_EOF);
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_SCP_CHANNEL_FREE);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SCP_SEND_EOF(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ssh_conn *sshc)
{
  अगर(sshc->scp_session) {
    पूर्णांक rc = ssh_scp_close(sshc->scp_session);
    अगर(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      वापस SSH_AGAIN;
    }
    अगर(rc != SSH_OK) {
      infof(तथ्य,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(तथ्य, sshc, SSH_SCP_CHANNEL_FREE);
  वापस SSH_NO_ERROR;
}

अटल पूर्णांक myssh_in_SCP_CHANNEL_FREE(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ssh_conn *sshc)
{
  अगर(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = शून्य;
  }
  CURL_TRC_SSH(तथ्य, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(तथ्य, sshc, SSH_SESSION_DISCONNECT);
  वापस SSH_NO_ERROR;
}

अटल CURLcode myssh_in_SESSION_FREE(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ssh_conn *sshc)
{
  CURLcode परिणाम;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  परिणाम = sshc->actualcode;
  स_रखो(sshc, 0, माप(काष्ठा ssh_conn));
  connclose(तथ्य->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस परिणाम;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
अटल CURLcode myssh_statemachine(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ssh_conn *sshc,
                                   काष्ठा SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  करो {

    चयन(sshc->state) {
    हाल SSH_INIT:
      myssh_state_init(तथ्य, sshc);
      FALLTHROUGH();
    हाल SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(तथ्य, sshc);
      अगर(rc)
        अवरोध;
      FALLTHROUGH();
    हाल SSH_HOSTKEY:
      rc = myssh_is_known(तथ्य, sshc);
      अगर(rc != SSH_OK) {
        rc = myssh_to_ERROR(तथ्य, sshc, CURLE_PEER_FAILED_VERIFICATION);
        अवरोध;
      }
      myssh_to(तथ्य, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    हाल SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(तथ्य, sshc);
      अवरोध;
    हाल SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(तथ्य, sshc);
      अवरोध;
    हाल SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(तथ्य, sshc);
      अवरोध;
    हाल SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(तथ्य, sshc);
      अवरोध;
    हाल SSH_AUTH_कुंजी_INIT:
      rc = myssh_in_AUTH_कुंजी_INIT(तथ्य, sshc);
      अवरोध;
    हाल SSH_AUTH_कुंजी:
      rc = myssh_in_AUTH_कुंजी(तथ्य, sshc);
      अवरोध;
    हाल SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(तथ्य, sshc);
      अवरोध;
    हाल SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(तथ्य, sshc);
      अवरोध;
    हाल SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_अग्र_QUOTE:
      rc = myssh_in_SFTP_अग्र_QUOTE(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(तथ्य, sshc);
      अवरोध;
    हाल SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(तथ्य, sshc);
      अवरोध;
    हाल SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(तथ्य, sshc, sshp);
      अवरोध;
    हाल SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(तथ्य, sshc);
      अगर(rc)
        अवरोध;
      FALLTHROUGH();
    हाल SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(तथ्य, sshc);
      अवरोध;
    हाल SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(तथ्य, sshc);
      अवरोध;
    हाल SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(तथ्य, sshc);
      अवरोध;
    हाल SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(तथ्य, sshc);
      FALLTHROUGH();
    हाल SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(तथ्य, sshc);
      FALLTHROUGH();
    हाल SSH_SESSION_FREE:
      परिणाम = myssh_in_SESSION_FREE(तथ्य, sshc);
      अवरोध;
    हाल SSH_QUIT:
    शेष:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(तथ्य, sshc, SSH_STOP);
      अवरोध;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } जबतक((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  अगर(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  अगर(!परिणाम && (sshc->state == SSH_STOP))
    परिणाम = sshc->actualcode;
  CURL_TRC_SSH(तथ्य, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (पूर्णांक)परिणाम, *block);
  वापस परिणाम;
}

/* called repeatedly until done from multi.c */
अटल CURLcode myssh_multi_statemach(काष्ठा Curl_easy *तथ्य,
                                      bool *done)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  काष्ठा SSHPROTO *sshp = Curl_meta_get(तथ्य, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode परिणाम;

  अगर(!sshc || !sshp)
    वापस CURLE_FAILED_INIT;
  परिणाम = myssh_statemachine(तथ्य, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  वापस परिणाम;
}

अटल CURLcode myssh_block_statemach(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ssh_conn *sshc,
                                      काष्ठा SSHPROTO *sshp,
                                      bool disconnect)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  CURLcode परिणाम = CURLE_OK;

  जबतक((sshc->state != SSH_STOP) && !परिणाम) {
    bool block;
    timediff_t बायाँ_ms = 1000;

    परिणाम = myssh_statemachine(तथ्य, sshc, sshp, &block);
    अगर(परिणाम)
      अवरोध;

    अगर(!disconnect) {
      परिणाम = Curl_pgrsCheck(तथ्य);
      अगर(परिणाम)
        अवरोध;

      बायाँ_ms = Curl_timeleft_ms(तथ्य);
      अगर(बायाँ_ms < 0) {
        failf(तथ्य, "Operation timed out");
        वापस CURLE_OPERATION_TIMEDOUT;
      }
    }

    अगर(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (व्योम)SOCKET_READABLE(fd_read,
                            (!बायाँ_ms || बायाँ_ms > 1000) ? 1000 : बायाँ_ms);
    }
  }

  वापस परिणाम;
}

अटल व्योम myssh_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा SSHPROTO *sshp = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

अटल व्योम myssh_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा ssh_conn *sshc = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
अटल CURLcode myssh_setup_connection(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा connectdata *conn)
{
  काष्ठा SSHPROTO *sshp;
  काष्ठा ssh_conn *sshc;

  sshc = curlx_calloc(1, माप(*sshc));
  अगर(!sshc)
    वापस CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  अगर(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, माप(*sshp));
  अगर(!sshp ||
     Curl_meta_set(तथ्य, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  वापस Curl_ssh_setup_pkey(तथ्य, sshc);
}

अटल Curl_recv scp_recv, sftp_recv;
अटल Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
अटल CURLcode myssh_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  काष्ठा SSHPROTO *ssh = Curl_meta_get(तथ्य, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णांक rc;

  अगर(!sshc || !ssh)
    वापस CURLE_FAILED_INIT;

  CURL_TRC_SSH(तथ्य, "myssh_connect");
  अगर(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  अन्यथा {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  अगर(!sshc->ssh_session) {
    failf(तथ्य, "Failure initializing ssh session");
    वापस CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  अगर(rc != SSH_OK) {
    failf(तथ्य, "Could not set remote host");
    वापस CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, शून्य);
  अगर(rc != SSH_OK) {
    infof(तथ्य, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(तथ्य, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  अगर(rc != SSH_OK) {
    failf(तथ्य, "Could not set socket");
    वापस CURLE_FAILED_INIT;
  }

  अगर(Curl_creds_has_user(conn->creds)) {
    infof(तथ्य, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    अगर(rc != SSH_OK) {
      failf(तथ्य, "Could not set user");
      वापस CURLE_FAILED_INIT;
    }
  }

  अगर(CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS)) {
    infof(तथ्य, "Known hosts: %s",
          CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS));
    अगर(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS));
    अगर(rc != SSH_OK) {
      failf(तथ्य, "Could not set known hosts file path");
      वापस CURLE_FAILED_INIT;
    }
  }

  अगर(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    अगर(rc != SSH_OK) {
      failf(तथ्य, "Could not set remote port");
      वापस CURLE_FAILED_INIT;
    }
  }

  अगर(तथ्य->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    अगर(rc != SSH_OK) {
      failf(तथ्य, "Could not set compression");
      वापस CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = शून्य;
  sshc->pubkey = शून्य;

  अगर(sshc->pub_कुंजी) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_कुंजी, &sshc->pubkey);
    अगर(rc != SSH_OK) {
      failf(तथ्य, "Could not load public key file");
      वापस CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(तथ्य, sshc, SSH_INIT);

  परिणाम = myssh_multi_statemach(तथ्य, done);

  वापस परिणाम;
}

/* called from multi.c while DOing */
अटल CURLcode scp_doing(काष्ठा Curl_easy *तथ्य, bool *dophase_done)
{
  CURLcode परिणाम;

  परिणाम = myssh_multi_statemach(तथ्य, dophase_done);

  अगर(*dophase_done) {
    CURL_TRC_SSH(तथ्य, "DO phase is complete");
  }
  वापस परिणाम;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

अटल CURLcode scp_perform(काष्ठा Curl_easy *तथ्य,
                            bool *connected, bool *dophase_done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(तथ्य->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(तथ्य, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(तथ्य, sshc, SSH_SCP_TRANS_INIT);

  परिणाम = myssh_multi_statemach(तथ्य, dophase_done);

  *connected = Curl_conn_is_connected(तथ्य->conn, FIRSTSOCKET);

  अगर(*dophase_done) {
    CURL_TRC_SSH(तथ्य, "DO phase is complete");
  }

  वापस परिणाम;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
अटल CURLcode scp_disconnect(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn,
                               bool dead_connection)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  काष्ठा SSHPROTO *sshp = Curl_meta_get(तथ्य, CURL_META_SSH_EASY);
  (व्योम)dead_connection;

  अगर(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(तथ्य, sshc, SSH_SESSION_DISCONNECT);
    परिणाम = myssh_block_statemach(तथ्य, sshc, sshp, TRUE);
  }

  वापस परिणाम;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
अटल CURLcode myssh_done(काष्ठा Curl_easy *तथ्य,
                           काष्ठा ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा SSHPROTO *sshp = Curl_meta_get(तथ्य, CURL_META_SSH_EASY);

  अगर(!status && sshp) {
    /* run the state-machine */
    परिणाम = myssh_block_statemach(तथ्य, sshc, sshp, FALSE);
  }
  अन्यथा
    परिणाम = status;

  अगर(Curl_pgrsDone(तथ्य, Curl_pgrs_now(तथ्य)))
    वापस CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(तथ्य);
  वापस परिणाम;
}

अटल CURLcode scp_done(काष्ठा Curl_easy *तथ्य, CURLcode status,
                         bool premature)
{
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(तथ्य->conn, CURL_META_SSH_CONN);
  (व्योम)premature;

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;
  अगर(!status)
    myssh_to(तथ्य, sshc, SSH_SCP_DONE);

  वापस myssh_done(तथ्य, sshc, status);
}

अटल CURLcode scp_send(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                         स्थिर uint8_t *mem, माप_प्रकार len, bool eos,
                         माप_प्रकार *pnwritten)
{
  पूर्णांक rc;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (व्योम)sockindex; /* we only support SCP on the fixed known primary socket */
  (व्योम)eos;
  *pnwritten = 0;

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#अगर 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  अगर(rc == SSH_AGAIN)
    वापस CURLE_AGAIN;
  अन्यथा
#पूर्ण_अगर
  अगर(rc != SSH_OK)
    वापस CURLE_SSH;

  *pnwritten = len;
  वापस CURLE_OK;
}

अटल CURLcode scp_recv(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                         अक्षर *mem, माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  पूर्णांक nread;

  (व्योम)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  अगर(nread == SSH_ERROR)
    वापस CURLE_SSH;
#अगर 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  अगर(nread == SSH_AGAIN)
    वापस CURLE_AGAIN;
#पूर्ण_अगर
  *pnread = (माप_प्रकार)nread;
  वापस CURLE_OK;
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

अटल CURLcode sftp_perform(काष्ठा Curl_easy *तथ्य,
                             bool *connected,
                             bool *dophase_done)
{
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(तथ्य->conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_SSH(तथ्य, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  परिणाम = myssh_multi_statemach(तथ्य, dophase_done);

  *connected = Curl_conn_is_connected(तथ्य->conn, FIRSTSOCKET);

  अगर(*dophase_done) {
    CURL_TRC_SSH(तथ्य, "DO phase is complete");
  }

  वापस परिणाम;
}

/* called from multi.c while DOing */
अटल CURLcode sftp_doing(काष्ठा Curl_easy *तथ्य,
                           bool *dophase_done)
{
  CURLcode परिणाम = myssh_multi_statemach(तथ्य, dophase_done);
  अगर(*dophase_done) {
    CURL_TRC_SSH(तथ्य, "DO phase is complete");
  }
  वापस परिणाम;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
अटल CURLcode sftp_disconnect(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn,
                                bool dead_connection)
{
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  काष्ठा SSHPROTO *sshp = Curl_meta_get(तथ्य, CURL_META_SSH_EASY);
  CURLcode परिणाम = CURLE_OK;
  (व्योम)dead_connection;

  CURL_TRC_SSH(तथ्य, "DISCONNECT starts now");

  अगर(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(तथ्य, sshc, SSH_SFTP_SHUTDOWN);
    परिणाम = myssh_block_statemach(तथ्य, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(तथ्य, "DISCONNECT is done");
  वापस परिणाम;
}

अटल CURLcode sftp_done(काष्ठा Curl_easy *तथ्य, CURLcode status,
                          bool premature)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;
  अगर(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    अगर(!premature && तथ्य->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
  }
  वापस myssh_done(तथ्य, sshc, status);
}

/* return number of sent bytes */
अटल CURLcode sftp_send(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                          स्थिर uint8_t *mem, माप_प्रकार len, bool eos,
                          माप_प्रकार *pnwritten)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (व्योम)sockindex;
  (व्योम)eos;
  *pnwritten = 0;

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

#अगर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  चयन(sshc->sftp_send_state) {
  हाल 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    अगर(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      वापस CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  हाल 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    अगर(nwrite == SSH_AGAIN)
      वापस CURLE_AGAIN;
    अन्यथा अगर(nwrite < 0)
      वापस CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (माप_प्रकार)nwrite;
    वापस CURLE_OK;
  शेष:
    /* we never reach here */
    वापस CURLE_SEND_ERROR;
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
  अगर(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#अगर 0 /* not returned by libssh on write */
  अगर(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  अन्यथा
#पूर्ण_अगर
  अगर(nwrite < 0)
    वापस CURLE_SSH;

  *pnwritten = (माप_प्रकार)nwrite;
  वापस CURLE_OK;
#पूर्ण_अगर
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
अटल CURLcode sftp_recv(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                          अक्षर *mem, माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (व्योम)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_आकार);
  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  चयन(sshc->sftp_recv_state) {
  हाल 0:
#अगर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    अगर(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      वापस CURLE_RECV_ERROR;
    }
#अन्यथा
    sshc->sftp_file_सूचकांक =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    अगर(sshc->sftp_file_सूचकांक < 0)
      वापस CURLE_RECV_ERROR;
#पूर्ण_अगर

    FALLTHROUGH();
  हाल 1:
    sshc->sftp_recv_state = 1;

#अगर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#अन्यथा
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_सूचकांक);
#पूर्ण_अगर

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    अगर(nread == SSH_AGAIN)
      वापस CURLE_AGAIN;
    अन्यथा अगर(nread < 0)
      वापस CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (माप_प्रकार)nread;
    वापस CURLE_OK;

  शेष:
    /* we never reach here */
    वापस CURLE_RECV_ERROR;
  }
}

अटल CURLcode myssh_do_it(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम;
  bool connected = FALSE;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  तथ्य->req.आकार = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(तथ्य);

  अगर(conn->scheme->protocol & CURLPROTO_SCP)
    परिणाम = scp_perform(तथ्य, &connected, done);
  अन्यथा
    परिणाम = sftp_perform(तथ्य, &connected, done);

  वापस परिणाम;
}

CURLcode Curl_ssh_init(व्योम)
{
  अगर(ssh_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: libssh_init failed\n"));
    वापस CURLE_FAILED_INIT;
  }
  वापस CURLE_OK;
}

व्योम Curl_ssh_cleanup(व्योम)
{
  (व्योम)ssh_finalize();
}

व्योम Curl_ssh_version(अक्षर *बफर, माप_प्रकार buflen)
{
  (व्योम)curl_msnprintf(बफर, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_scp = {
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
स्थिर काष्ठा Curl_protocol Curl_protocol_sftp = {
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

#पूर्ण_अगर /* USE_LIBSSH */
