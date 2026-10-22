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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_LIBSSH

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
#அந்தர்பாவ "parsedate.h"          /* for the week day and month names */
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "vssh/vssh.h"
#அந்தர்பாவ "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#அந்தர்பாவ <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#அந்தர்பாவ <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  செய் {                          \
    எனில்(x) {                     \
      ssh_string_free_char(x);  \
      x = வெற்று;                 \
    }                           \
  } போது(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

நிலையான CURLcode sftp_error_to_CURLE(முழு err)
{
  தேர்வு(err) {
  நிலை SSH_FX_OK:
    திரும்பு CURLE_OK;

  நிலை SSH_FX_NO_SUCH_FILE:
  நிலை SSH_FX_NO_SUCH_PATH:
    திரும்பு CURLE_REMOTE_FILE_NOT_FOUND;

  நிலை SSH_FX_PERMISSION_DENIED:
  நிலை SSH_FX_WRITE_PROTECT:
    திரும்பு CURLE_REMOTE_ACCESS_DENIED;

  நிலை SSH_FX_FILE_ALREADY_EXISTS:
    திரும்பு CURLE_REMOTE_FILE_EXISTS;

  இயல்பு:
    நிறுத்து;
  }

  திரும்பு CURLE_SSH;
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
நிலையான முழு myssh_is_known(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc;
  ssh_சாவி pubkey;
  unsigned எழுத்து *hash_sha256 = வெற்று;
  size_t hlen_sha256;
  unsigned எழுத்து *hash_md5 = வெற்று;
  size_t hlen_md5;
  எழுத்து *found_base64 = வெற்று;
  எழுத்து *known_base64 = வெற்று;
  முழு vstate;
  எண்ணிக்கை curl_khmatch keymatch;
  கட்டமைப்பு curl_khkey foundkey;
  கட்டமைப்பு curl_khkey *knownkeyp = வெற்று;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  கட்டமைப்பு ssh_knownhosts_entry *knownhostsentry = வெற்று;
  கட்டமைப்பு curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  எனில்(rc != SSH_OK)
    திரும்பு rc;

  எனில்(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_சாவி_SHA256)) {
    மாறிலி எழுத்து *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_சாவி_SHA256);
    எழுத்து *fingerprint_b64 = வெற்று;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    எனில்(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      செல் cleanup;
    }

    எனில்(curlx_base64_encode((மாறிலி uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      செல் cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

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
      rc = SSH_ERROR;
      செல் cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    செல் cleanup;
  }

  எனில்(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_சாவி_MD5)) {
    மாறிலி எழுத்து *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_சாவி_MD5);
    எழுத்து md5buffer[33];
    முழு i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    எனில்(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      செல் cleanup;
    }

    க்கு(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    எனில்(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      செல் cleanup;
    }

    rc = SSH_OK;
    செல் cleanup;
  }

  எனில்(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    எனில்(knownhostsentry) {
      எனில்(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        எனில்(rc != SSH_OK) {
          செல் cleanup;
        }
        knownkey.சாவி = known_base64;
        knownkey.len = strlen(known_base64);

        தேர்வு(ssh_சாவி_type(knownhostsentry->publickey)) {
        நிலை SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          நிறுத்து;
        நிலை SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          நிறுத்து;
        நிலை SSH_KEYTYPE_ECDSA:
        நிலை SSH_KEYTYPE_ECDSA_P256:
        நிலை SSH_KEYTYPE_ECDSA_P384:
        நிலை SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          நிறுத்து;
        நிலை SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          நிறுத்து;
        நிலை SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          நிறுத்து;
        இயல்பு:
          rc = SSH_ERROR;
          செல் cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    தேர்வு(vstate) {
    நிலை SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      நிறுத்து;
    நிலை SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      நிறுத்து;
    நிலை SSH_KNOWN_HOSTS_NOT_FOUND:
    நிலை SSH_KNOWN_HOSTS_UNKNOWN:
    நிலை SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      நிறுத்து;
    இயல்பு:
      keymatch = CURLKHMATCH_MISMATCH;
      நிறுத்து;
    }

    எனில்(func) { /* use callback to determine action */
      கட்டமைப்பு Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      எனில்(rc != SSH_OK)
        செல் cleanup;

      foundkey.சாவி = found_base64;
      foundkey.len = strlen(found_base64);

      தேர்வு(ssh_சாவி_type(pubkey)) {
      நிலை SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        நிறுத்து;
      நிலை SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        நிறுத்து;
      நிலை SSH_KEYTYPE_ECDSA:
      நிலை SSH_KEYTYPE_ECDSA_P256:
      நிலை SSH_KEYTYPE_ECDSA_P384:
      நிலை SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        நிறுத்து;
      நிலை SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        நிறுத்து;
      நிலை SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        நிறுத்து;
      இயல்பு:
        rc = SSH_ERROR;
        செல் cleanup;
      }

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      தேர்வு(rc) {
      நிலை CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        எனில்(rc != SSH_OK) {
          செல் cleanup;
        }
        நிறுத்து;
      நிலை CURLKHSTAT_FINE:
        நிறுத்து;
      இயல்பு: /* REJECT/DEFER */
        rc = SSH_ERROR;
        செல் cleanup;
      }
    }
    இல்லையெனில் {
      எனில்(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        செல் cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  எனில்(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    விடுவி(found_base64); /* allocated by libssh, deallocate with system free */
  }
  எனில்(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    விடுவி(known_base64); /* allocated by libssh, deallocate with system free */
  }
  எனில்(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  எனில்(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_சாவி_free(pubkey);
  எனில்(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  திரும்பு rc;
}

நிலையான முழு myssh_to_ERROR(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  திரும்பு SSH_ERROR;
}

நிலையான முழு myssh_to_SFTP_CLOSE(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  திரும்பு SSH_ERROR;
}

நிலையான முழு myssh_to_PASSWD_AUTH(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    திரும்பு SSH_OK;
  }
  திரும்பு myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

நிலையான முழு myssh_to_சாவி_AUTH(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_சாவி_INIT);
    திரும்பு SSH_OK;
  }
  திரும்பு myssh_to_PASSWD_AUTH(data, sshc);
}

நிலையான முழு myssh_to_GSSAPI_AUTH(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    திரும்பு SSH_OK;
  }
  திரும்பு myssh_to_சாவி_AUTH(data, sshc);
}

நிலையான முழு myssh_in_SFTP_READDIR_INIT(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு ssh_conn *sshc,
                                      கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  எனில்(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    திரும்பு SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  எனில்(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    திரும்பு myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_READDIR(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு ssh_conn *sshc,
                                 கட்டமைப்பு SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  எனில்(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  எனில்(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    எனில்(data->set.list_only) {
      எழுத்து *tmp_வரி;

      tmp_வரி = curl_maprintf("%s\n", sshc->readdir_filename);
      எனில்(!tmp_வரி) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        திரும்பு SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmp_வரி, sshc->readdir_len + 1);
      curlx_free(tmp_வரி);

      எனில்(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        திரும்பு SSH_NO_ERROR;
      }
    }
    இல்லையெனில் {
      எனில்(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        திரும்பு SSH_ERROR;
      }

      எனில்((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        எனில்(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          திரும்பு SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        திரும்பு SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      திரும்பு SSH_NO_ERROR;
    }
  }
  இல்லையெனில் எனில்(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  இல்லையெனில் {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    திரும்பு myssh_to_SFTP_CLOSE(data, sshc);
  }
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_READDIR_LINK(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  எனில்(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    திரும்பு myssh_to_SFTP_CLOSE(data, sshc);
  }

  எனில்(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    எனில்(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    இல்லையெனில்
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = வெற்று;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  இல்லையெனில் {
    sshc->readdir_len = strlen(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  எனில்(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    திரும்பு SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = வெற்று;
  sshc->readdir_filename = வெற்று;
  sshc->readdir_longentry = வெற்று;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_READDIR_BOTTOM(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு ssh_conn *sshc)
{
  CURLcode result;

  எனில்(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  இல்லையெனில்
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = வெற்று;

  எனில்(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  இல்லையெனில்
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_READDIR_DONE(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = வெற்று;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  திரும்பு SSH_NO_ERROR;
}

நிலையான வெறுமை myssh_quote_error(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு ssh_conn *sshc,
                              மாறிலி எழுத்து *cmd)
{
  எனில்(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

நிலையான முழு myssh_in_SFTP_QUOTE_STATVFS(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  எனில்(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    திரும்பு SSH_OK;
  }
  இல்லையெனில் எனில்(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#அல்லதெனில் defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#இல்லையெனில்
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    எழுத்து *tmp = curl_maprintf("statvfs:\n"
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

    எனில்(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    எனில்(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    எனில்(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு SSH_OK;
}

நிலையான முழு myssh_auth_interactive(கட்டமைப்பு connectdata *conn,
                                  கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc;
  முழு nprompts;

restart:
  தேர்வு(sshc->kbd_state) {
  நிலை 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, வெற்று, வெற்று);
    எனில்(rc == SSH_AUTH_AGAIN)
      திரும்பு SSH_AGAIN;

    எனில்(rc != SSH_AUTH_INFO)
      திரும்பு SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    எனில்(nprompts != 1)
      திரும்பு SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    எனில்(rc < 0)
      திரும்பு SSH_ERROR;

    FALLTHROUGH();
  நிலை 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, வெற்று, வெற்று);
    எனில்(rc == SSH_AUTH_AGAIN)
      திரும்பு SSH_AGAIN;
    இல்லையெனில் எனில்(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    இல்லையெனில் எனில்(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      எனில்(nprompts)
        திரும்பு SSH_ERROR;

      sshc->kbd_state = 2;
      செல் restart;
    }
    இல்லையெனில்
      rc = SSH_ERROR;
    நிறுத்து;
  நிலை 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, வெற்று, வெற்று);
    எனில்(rc == SSH_AUTH_AGAIN)
      திரும்பு SSH_AGAIN;
    இல்லையெனில் எனில்(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    இல்லையெனில்
      rc = SSH_ERROR;

    நிறுத்து;
  இயல்பு:
    திரும்பு SSH_ERROR;
  }

  sshc->kbd_state = 0;
  திரும்பு rc;
}

நிலையான வெறுமை myssh_state_init(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#எனில் 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

நிலையான வெறுமை myssh_block2waitfor(கட்டமைப்பு connectdata *conn,
                                கட்டமைப்பு ssh_conn *sshc,
                                bool block)
{
  (வெறுமை)conn;
  எனில்(block) {
    முழு dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  இல்லையெனில்
    sshc->waitfor = 0;
}

நிலையான முழு myssh_in_S_STARTUP(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு ssh_conn *sshc)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  முழு rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  எனில்(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  இல்லையெனில் எனில்(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  இல்லையெனில்
    myssh_to(data, sshc, SSH_HOSTKEY);

  திரும்பு rc;
}

நிலையான முழு myssh_in_AUTHLIST(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, வெற்று);
  எனில்(rc == SSH_AUTH_AGAIN)
    திரும்பு SSH_AGAIN;

  எனில்(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    திரும்பு rc;
  }
  இல்லையெனில் எனில்(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    திரும்பு rc;
  }

  sshc->auth_methods =
    (unsigned முழு)ssh_userauth_list(sshc->ssh_session, வெற்று);
  எனில்(sshc->auth_methods)
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
  எனில்((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_சாவி && sshc->priv_சாவி[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  இல்லையெனில் எனில்(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  இல்லையெனில் எனில்(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_சாவி_INIT);
  }
  இல்லையெனில் எனில்(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  இல்லையெனில் {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  திரும்பு rc;
}

நிலையான முழு myssh_in_AUTH_PKEY_INIT(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc;
  எனில்(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    திரும்பு rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  எனில்(sshc->priv_சாவி && sshc->priv_சாவி[0]) {
    எனில்(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, வெற்று, sshc->pubkey);
      எனில்(rc == SSH_AUTH_AGAIN)
        திரும்பு SSH_AGAIN;

      எனில்(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        திரும்பு rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_சாவி,
                                     sshc->passphrase, வெற்று,
                                     வெற்று, &sshc->privkey);
    எனில்(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_சாவி);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      திரும்பு rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  இல்லையெனில் {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, வெற்று,
                                     sshc->passphrase);
    எனில்(rc == SSH_AUTH_AGAIN)
      திரும்பு SSH_AGAIN;

    எனில்(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      திரும்பு rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  திரும்பு rc;
}

நிலையான முழு myssh_in_AUTH_PKEY(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = ssh_userauth_publickey(sshc->ssh_session, வெற்று, sshc->privkey);
  எனில்(rc == SSH_AUTH_AGAIN)
    திரும்பு SSH_AGAIN;
  இல்லையெனில் எனில்(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    திரும்பு SSH_OK;
  }
  இல்லையெனில் {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    திரும்பு myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

நிலையான முழு myssh_in_AUTH_GSSAPI(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc;
  எனில்(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    திரும்பு myssh_to_சாவி_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  எனில்(rc == SSH_AUTH_AGAIN)
    திரும்பு SSH_AGAIN;

  எனில்(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    திரும்பு SSH_OK;
  }

  திரும்பு myssh_to_சாவி_AUTH(data, sshc);
}

நிலையான முழு myssh_in_AUTH_சாவி_INIT(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_சாவி);
    திரும்பு SSH_NO_ERROR;
  }
  திரும்பு myssh_to_PASSWD_AUTH(data, sshc);
}

நிலையான முழு myssh_in_AUTH_சாவி(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  முழு rc = myssh_auth_interactive(data->conn, sshc);
  எனில்(rc == SSH_AGAIN)
    திரும்பு rc;
  இல்லையெனில் எனில்(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    திரும்பு SSH_NO_ERROR;
  }
  இல்லையெனில்
    திரும்பு myssh_to_PASSWD_AUTH(data, sshc);
}

நிலையான முழு myssh_in_AUTH_PASS_INIT(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_AUTH_PASS(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = ssh_userauth_password(sshc->ssh_session, வெற்று,
                                 Curl_creds_passwd(data->conn->creds));
  எனில்(rc == SSH_AUTH_AGAIN)
    திரும்பு SSH_AGAIN;
  இல்லையெனில் எனில்(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    திரும்பு SSH_NO_ERROR;
  }
  திரும்பு myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

நிலையான முழு myssh_in_AUTH_DONE(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு ssh_conn *sshc)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  எனில்(!sshc->authed) {
    failf(data, "Authentication failure");
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  எனில்(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    திரும்பு SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_UPLOAD_INIT(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு ssh_conn *sshc,
                                கட்டமைப்பு SSHPROTO *sshp)
{
  முழு flags;
  முழு rc = 0;

  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  எனில்(data->state.resume_from) {
    sftp_attributes attrs;

    எனில்(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      எனில்(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        எனில்(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          திரும்பு rc;
        }
        data->state.resume_from = size;
      }
      இல்லையெனில் {
        data->state.resume_from = 0;
      }
    }
  }

  எனில்(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  இல்லையெனில் எனில்(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  இல்லையெனில்
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  எனில்(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  எனில்(!sshc->sftp_file) {
    முழு err = sftp_get_error(sshc->sftp_session);

    எனில்((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      திரும்பு rc;
    }
    இல்லையெனில் {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      திரும்பு rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
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
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        திரும்பு rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      செய் {
        எழுத்து scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)அளவு(scratch)) ?
          அளவு(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        எனில்((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          திரும்பு rc;
        }
      } போது(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    எனில்(data->state.infilesize > 0) {
      எனில்(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        திரும்பு myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    எனில்(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      திரும்பு rc;
    }
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
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(data);
#எனில் LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  திரும்பு rc;
}

நிலையான முழு myssh_in_SFTP_DOWNLOAD_INIT(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc,
                                       கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  எனில்(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  எனில்(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    திரும்பு myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_DOWNLOAD_STAT(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc)
{
  curl_off_t size;
  முழு rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  எனில்(!attrs ||
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
    எனில்(attrs)
      sftp_attributes_free(attrs);
  }
  இல்லையெனில் {
    size = attrs->size;

    sftp_attributes_free(attrs);

    எனில்(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      திரும்பு myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    எனில்(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      எனில்(result)
        திரும்பு myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      எனில்(rc)
        திரும்பு myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  எனில்(data->state.resume_from) {
    எனில்(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      எனில்(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        திரும்பு myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    இல்லையெனில் {
      எனில்(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        திரும்பு myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    எனில்(rc)
      திரும்பு myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  எனில்(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    திரும்பு rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  திரும்பு rc;
}

நிலையான முழு myssh_in_SFTP_CLOSE(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு ssh_conn *sshc,
                               கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  எனில்(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = வெற்று;
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
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_STOP);
  }
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_SHUTDOWN(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#எனில் LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  எனில்(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = வெற்று;
  }

  எனில்(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = வெற்று;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_INIT(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  எனில்(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  எனில்(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    திரும்பு myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_REALPATH(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  எனில்(!sshc->homedir)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  எனில்(!data->state.most_recent_ftp_entrypath)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_QUOTE_INIT(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssh_conn *sshc,
                                    கட்டமைப்பு SSHPROTO *sshp)
{
  CURLcode result;

  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  எனில்(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  இல்லையெனில் எனில்(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  இல்லையெனில்
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_POSTQUOTE_INIT(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_STOP);
  }
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு quote_error(கட்டமைப்பு Curl_easy *data,
                       கட்டமைப்பு ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_QUOTE(கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு ssh_conn *sshc,
                               கட்டமைப்பு SSHPROTO *sshp)
{
  மாறிலி எழுத்து *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  மாறிலி எழுத்து *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

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
    எனில்(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      திரும்பு SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    எனில்(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    இல்லையெனில்
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    திரும்பு SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  எனில்(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    திரும்பு SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  எனில்(result) {
    எனில்(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    இல்லையெனில்
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    திரும்பு SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
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
      எனில்(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      இல்லையெனில்
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      திரும்பு SSH_NO_ERROR;
    }
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    sshc->quote_attrs = வெற்று;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    திரும்பு SSH_NO_ERROR;
  }
  எனில்(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    எனில்(result) {
      எனில்(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      இல்லையெனில்
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      திரும்பு SSH_NO_ERROR;
    }
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    திரும்பு SSH_NO_ERROR;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "mkdir ", 6)) {
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    திரும்பு SSH_NO_ERROR;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    எனில்(result) {
      எனில்(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      இல்லையெனில்
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      திரும்பு SSH_NO_ERROR;
    }
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    திரும்பு SSH_NO_ERROR;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    திரும்பு SSH_NO_ERROR;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "rm ", 3)) {
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    திரும்பு SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  இல்லையெனில் எனில்(!strncmp(cmd, "statvfs ", 8)) {
    எனில்(*cp)
      திரும்பு quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    திரும்பு SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_NEXT_QUOTE(கட்டமைப்பு Curl_easy *data,
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
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_QUOTE_STAT(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssh_conn *sshc)
{
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

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  எனில்(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  எனில்(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, வெற்று);
    திரும்பு SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  எனில்(!strncmp(cmd, "chgrp", 5)) {
    மாறிலி எழுத்து *p = sshc->quote_path1;
    curl_off_t gid;
    எனில்(curlx_str_எண்(&p, &gid, UINT_MAX)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, வெற்று);
      திரும்பு SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    மாறிலி எழுத்து *p = sshc->quote_path1;
    எனில்(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, வெற்று);
      திரும்பு SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "chown", 5)) {
    மாறிலி எழுத்து *p = sshc->quote_path1;
    curl_off_t uid;
    எனில்(curlx_str_எண்(&p, &uid, UINT_MAX)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, வெற்று);
      திரும்பு SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  இல்லையெனில் எனில்(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    எனில்(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#எனில் SIZEOF_TIME_T > 4
    இல்லையெனில் எனில்(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    எனில்(fail) {
      myssh_quote_error(data, sshc, வெற்று);
      திரும்பு SSH_NO_ERROR;
    }
    எனில்(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    இல்லையெனில் /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  திரும்பு SSH_NO_ERROR;
}

நிலையான வெறுமை conn_forget_socket(கட்டமைப்பு Curl_easy *data, int8_t sockindex)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  எனில்(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    கட்டமைப்பு Curl_cfilter *cf = conn->cfilter[sockindex];
    எனில்(cf)
      (வெறுமை)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, வெற்று);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

நிலையான வெறுமை myssh_SESSION_DISCONNECT(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  எனில்(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = வெற்று;
  }

  எனில்(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = வெற்று;
  }
  எனில்(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = வெற்று;
  }

  ssh_disconnect(sshc->ssh_session);
  எனில்(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

நிலையான முழு myssh_SSH_SCP_DOWNLOAD(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ssh_conn *sshc)
{
  uint64_t bytecount;
  முழு rc = ssh_scp_pull_request(sshc->scp_session);
  எனில்(rc != SSH_SCP_REQUEST_NEWFILE) {
    மாறிலி எழுத்து *err_செய்தி = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_செய்தி);
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  எனில்(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  திரும்பு 0;
}

நிலையான முழு myssh_in_TRANS_INIT(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு ssh_conn *sshc,
                               கட்டமைப்பு SSHPROTO *sshp)
{
  CURLcode result;
  முழு rc = 0;
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  எனில்(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    திரும்பு 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  எனில்(data->state.upload) {
    எனில்(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      திரும்பு myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  இல்லையெனில் {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  எனில்(!sshc->scp_session) {
    மாறிலி எழுத்து *err_செய்தி = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_செய்தி);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  திரும்பு rc;
}

நிலையான வெறுமை sshc_cleanup(கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->initialized) {
    எனில்(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = வெற்று;
    }
    எனில்(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = வெற்று;
    }
    எனில்(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = வெற்று;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    எனில்(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = வெற்று;
    }
    எனில்(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = வெற்று;
    }
    எனில்(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = வெற்று;
    }
    எனில்(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = வெற்று;
    }
    எனில்(sshc->privkey) {
      ssh_சாவி_free(sshc->privkey);
      sshc->privkey = வெற்று;
    }
    எனில்(sshc->pubkey) {
      ssh_சாவி_free(sshc->pubkey);
      sshc->pubkey = வெற்று;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_சாவி);
    curlx_safefree(sshc->priv_சாவி);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

நிலையான முழு myssh_in_SFTP_QUOTE_SETSTAT(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  எனில்(rc == SSH_AGAIN)
    திரும்பு rc;
  எனில்(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    திரும்பு rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_QUOTE_SYMLINK(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  எனில்(rc == SSH_AGAIN)
    திரும்பு rc;
  எனில்(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    திரும்பு rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_QUOTE_MKDIR(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  எனில்(rc == SSH_AGAIN)
    திரும்பு rc;
  எனில்(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    திரும்பு rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_QUOTE_RENAME(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  எனில்(rc == SSH_AGAIN)
    திரும்பு rc;
  எனில்(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    திரும்பு rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_QUOTE_RMDIR(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  எனில்(rc == SSH_AGAIN)
    திரும்பு rc;
  எனில்(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    திரும்பு rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_QUOTE_UNLINK(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  எனில்(rc == SSH_AGAIN)
    திரும்பு rc;
  எனில்(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    திரும்பு rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_GETINFO(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  இல்லையெனில்
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_FILETIME(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ssh_conn *sshc,
                                  கட்டமைப்பு SSHPROTO *sshp)
{
  sftp_attributes attrs;
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  எனில்(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_TRANS_INIT(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssh_conn *sshc,
                                    கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  எனில்(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  இல்லையெனில் {
    size_t path_len = strlen(sshp->path);

    எனில்(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    இல்லையெனில்
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_CREATE_DIRS_INIT(கட்டமைப்பு Curl_easy *data,
                                          கட்டமைப்பு ssh_conn *sshc,
                                          கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  எனில்(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  இல்லையெனில் {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_CREATE_DIRS(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு ssh_conn *sshc,
                                     கட்டமைப்பு SSHPROTO *sshp)
{
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  எனில்(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    திரும்பு SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SFTP_CREATE_DIRS_MKDIR(கட்டமைப்பு Curl_easy *data,
                                           கட்டமைப்பு ssh_conn *sshc,
                                           கட்டமைப்பு SSHPROTO *sshp)
{
  முழு rc;
  முழு err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  எனில்(rc == SSH_AGAIN)
    திரும்பு rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  எனில்(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    எனில்((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      திரும்பு myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  திரும்பு rc;
}

நிலையான முழு myssh_in_SCP_UPLOAD_INIT(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ssh_conn *sshc,
                                    கட்டமைப்பு SSHPROTO *sshp)
{
  முழு rc;
  எனில்(!sshp)
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  எனில்(rc != SSH_OK) {
    மாறிலி எழுத்து *err_செய்தி = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_செய்தி);
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_தள்ளு_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (முழு)data->set.new_file_perms);

  எனில்(rc != SSH_OK) {
    மாறிலி எழுத்து *err_செய்தி = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_செய்தி);
    திரும்பு myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SCP_DOWNLOAD_INIT(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு ssh_conn *sshc)
{
  முழு rc = ssh_scp_init(sshc->scp_session);
  எனில்(rc != SSH_OK) {
    மாறிலி எழுத்து *err_செய்தி = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_செய்தி);
    திரும்பு myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SCP_DONE(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  இல்லையெனில்
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SCP_SEND_EOF(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->scp_session) {
    முழு rc = ssh_scp_close(sshc->scp_session);
    எனில்(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      திரும்பு SSH_AGAIN;
    }
    எனில்(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  திரும்பு SSH_NO_ERROR;
}

நிலையான முழு myssh_in_SCP_CHANNEL_FREE(கட்டமைப்பு Curl_easy *data,
                                     கட்டமைப்பு ssh_conn *sshc)
{
  எனில்(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = வெற்று;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  திரும்பு SSH_NO_ERROR;
}

நிலையான CURLcode myssh_in_SESSION_FREE(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  memset(sshc, 0, அளவு(கட்டமைப்பு ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  திரும்பு result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
நிலையான CURLcode myssh_statemachine(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ssh_conn *sshc,
                                   கட்டமைப்பு SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  முழு rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  செய் {

    தேர்வு(sshc->state) {
    நிலை SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    நிலை SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      எனில்(rc)
        நிறுத்து;
      FALLTHROUGH();
    நிலை SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      எனில்(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        நிறுத்து;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    நிலை SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      நிறுத்து;
    நிலை SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      நிறுத்து;
    நிலை SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      நிறுத்து;
    நிலை SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      நிறுத்து;
    நிலை SSH_AUTH_சாவி_INIT:
      rc = myssh_in_AUTH_சாவி_INIT(data, sshc);
      நிறுத்து;
    நிலை SSH_AUTH_சாவி:
      rc = myssh_in_AUTH_சாவி(data, sshc);
      நிறுத்து;
    நிலை SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      நிறுத்து;
    நிலை SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      நிறுத்து;
    நிலை SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      நிறுத்து;
    நிலை SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      நிறுத்து;
    நிலை SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      நிறுத்து;
    நிலை SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      எனில்(rc)
        நிறுத்து;
      FALLTHROUGH();
    நிலை SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      நிறுத்து;
    நிலை SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      நிறுத்து;
    நிலை SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      நிறுத்து;
    நிலை SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    நிலை SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    நிலை SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      நிறுத்து;
    நிலை SSH_QUIT:
    இயல்பு:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      நிறுத்து;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } போது((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  எனில்(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  எனில்(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (முழு)result, *block);
  திரும்பு result;
}

/* called repeatedly until done from multi.c */
நிலையான CURLcode myssh_multi_statemach(கட்டமைப்பு Curl_easy *data,
                                      bool *done)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  கட்டமைப்பு SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  எனில்(!sshc || !sshp)
    திரும்பு CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  திரும்பு result;
}

நிலையான CURLcode myssh_block_statemach(கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு ssh_conn *sshc,
                                      கட்டமைப்பு SSHPROTO *sshp,
                                      bool disconnect)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  போது((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t இடது_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
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

    எனில்(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (வெறுமை)SOCKET_READABLE(fd_read,
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
  curlx_free(sshp);
}

நிலையான வெறுமை myssh_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு ssh_conn *sshc = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
நிலையான CURLcode myssh_setup_connection(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு SSHPROTO *sshp;
  கட்டமைப்பு ssh_conn *sshc;

  sshc = curlx_calloc(1, அளவு(*sshc));
  எனில்(!sshc)
    திரும்பு CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  எனில்(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, அளவு(*sshp));
  எனில்(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    திரும்பு CURLE_OUT_OF_MEMORY;

  திரும்பு Curl_ssh_setup_pkey(data, sshc);
}

நிலையான Curl_recv scp_recv, sftp_recv;
நிலையான Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
நிலையான CURLcode myssh_connect(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  கட்டமைப்பு SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  முழு rc;

  எனில்(!sshc || !ssh)
    திரும்பு CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  எனில்(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  இல்லையெனில் {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  எனில்(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    திரும்பு CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  எனில்(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    திரும்பு CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, வெற்று);
  எனில்(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  எனில்(rc != SSH_OK) {
    failf(data, "Could not set socket");
    திரும்பு CURLE_FAILED_INIT;
  }

  எனில்(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    எனில்(rc != SSH_OK) {
      failf(data, "Could not set user");
      திரும்பு CURLE_FAILED_INIT;
    }
  }

  எனில்(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    எனில்(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    எனில்(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      திரும்பு CURLE_FAILED_INIT;
    }
  }

  எனில்(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    எனில்(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      திரும்பு CURLE_FAILED_INIT;
    }
  }

  எனில்(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    எனில்(rc != SSH_OK) {
      failf(data, "Could not set compression");
      திரும்பு CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = வெற்று;
  sshc->pubkey = வெற்று;

  எனில்(sshc->pub_சாவி) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_சாவி, &sshc->pubkey);
    எனில்(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      திரும்பு CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  திரும்பு result;
}

/* called from multi.c while DOing */
நிலையான CURLcode scp_doing(கட்டமைப்பு Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  எனில்(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
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
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

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
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  திரும்பு result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
நிலையான CURLcode myssh_done(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  எனில்(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
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

  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;
  எனில்(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  திரும்பு myssh_done(data, sshc, status);
}

நிலையான CURLcode scp_send(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                         மாறிலி uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  முழு rc;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (வெறுமை)sockindex; /* we only support SCP on the fixed known primary socket */
  (வெறுமை)eos;
  *pnwritten = 0;

  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#எனில் 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  எனில்(rc == SSH_AGAIN)
    திரும்பு CURLE_AGAIN;
  இல்லையெனில்
#endif
  எனில்(rc != SSH_OK)
    திரும்பு CURLE_SSH;

  *pnwritten = len;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode scp_recv(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                         எழுத்து *mem, size_t len, size_t *pnread)
{
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  முழு nread;

  (வெறுமை)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  எனில்(nread == SSH_ERROR)
    திரும்பு CURLE_SSH;
#எனில் 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  எனில்(nread == SSH_AGAIN)
    திரும்பு CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  திரும்பு CURLE_OK;
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
  result = myssh_multi_statemach(data, dophase_done);

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
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  எனில்(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  திரும்பு result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
நிலையான CURLcode sftp_disconnect(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn,
                                bool dead_connection)
{
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  கட்டமைப்பு SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (வெறுமை)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  எனில்(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
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
  திரும்பு myssh_done(data, sshc, status);
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

#எனில் LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  தேர்வு(sshc->sftp_send_state) {
  நிலை 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    எனில்(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      திரும்பு CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  நிலை 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    எனில்(nwrite == SSH_AGAIN)
      திரும்பு CURLE_AGAIN;
    இல்லையெனில் எனில்(nwrite < 0)
      திரும்பு CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    திரும்பு CURLE_OK;
  இயல்பு:
    /* we never reach here */
    திரும்பு CURLE_SEND_ERROR;
  }
#இல்லையெனில்
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  எனில்(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#எனில் 0 /* not returned by libssh on write */
  எனில்(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  இல்லையெனில்
#endif
  எனில்(nwrite < 0)
    திரும்பு CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  திரும்பு CURLE_OK;
#endif
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

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  தேர்வு(sshc->sftp_recv_state) {
  நிலை 0:
#எனில் LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    எனில்(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      திரும்பு CURLE_RECV_ERROR;
    }
#இல்லையெனில்
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    எனில்(sshc->sftp_file_index < 0)
      திரும்பு CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  நிலை 1:
    sshc->sftp_recv_state = 1;

#எனில் LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#இல்லையெனில்
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    எனில்(nread == SSH_AGAIN)
      திரும்பு CURLE_AGAIN;
    இல்லையெனில் எனில்(nread < 0)
      திரும்பு CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    திரும்பு CURLE_OK;

  இயல்பு:
    /* we never reach here */
    திரும்பு CURLE_RECV_ERROR;
  }
}

நிலையான CURLcode myssh_do_it(கட்டமைப்பு Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  எனில்(!sshc)
    திரும்பு CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
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
  எனில்(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    திரும்பு CURLE_FAILED_INIT;
  }
  திரும்பு CURLE_OK;
}

வெறுமை Curl_ssh_cleanup(வெறுமை)
{
  (வெறுமை)ssh_finalize();
}

வெறுமை Curl_ssh_version(எழுத்து *buffer, size_t buflen)
{
  (வெறுமை)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_scp = {
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
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_sftp = {
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
