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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_LIBSSH

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাৱ <in.h>
#অন্তর্ভাৱ <inet.h>
#endif

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "vssh/ssh.h"
#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "cfilters.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "parsedate.h"          /* for the week day and month names */
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "vssh/vssh.h"
#অন্তর্ভাৱ "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#অন্তর্ভাৱ <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#অন্তর্ভাৱ <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  কৰা {                          \
    যদি(x) {                     \
      ssh_string_free_char(x);  \
      x = রিক্ত;                 \
    }                           \
  } যেতিয়ালৈ(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

স্থির CURLcode sftp_error_to_CURLE(পূর্ণাঙ্ক err)
{
  ৱিকল্প(err) {
  পক্ষ SSH_FX_OK:
    ঘূৰাই CURLE_OK;

  পক্ষ SSH_FX_NO_SUCH_FILE:
  পক্ষ SSH_FX_NO_SUCH_PATH:
    ঘূৰাই CURLE_REMOTE_FILE_NOT_FOUND;

  পক্ষ SSH_FX_PERMISSION_DENIED:
  পক্ষ SSH_FX_WRITE_PROTECT:
    ঘূৰাই CURLE_REMOTE_ACCESS_DENIED;

  পক্ষ SSH_FX_FILE_ALREADY_EXISTS:
    ঘূৰাই CURLE_REMOTE_FILE_EXISTS;

  স্ৱভাৱ:
    ভঙ্গ;
  }

  ঘূৰাই CURLE_SSH;
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
স্থির পূর্ণাঙ্ক myssh_is_known(সংরচনা Curl_easy *data, সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc;
  ssh_চাবি pubkey;
  unsigned ৱর্ণ *hash_sha256 = রিক্ত;
  size_t hlen_sha256;
  unsigned ৱর্ণ *hash_md5 = রিক্ত;
  size_t hlen_md5;
  ৱর্ণ *found_base64 = রিক্ত;
  ৱর্ণ *known_base64 = রিক্ত;
  পূর্ণাঙ্ক vstate;
  গণনা curl_khmatch keymatch;
  সংরচনা curl_khkey foundkey;
  সংরচনা curl_khkey *knownkeyp = রিক্ত;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  সংরচনা ssh_knownhosts_entry *knownhostsentry = রিক্ত;
  সংরচনা curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  যদি(rc != SSH_OK)
    ঘূৰাই rc;

  যদি(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_চাবি_SHA256)) {
    ধ্রুৱ ৱর্ণ *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_চাবি_SHA256);
    ৱর্ণ *fingerprint_b64 = রিক্ত;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    যদি(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      গচ্ছ cleanup;
    }

    যদি(curlx_base64_encode((ধ্রুৱ uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      গচ্ছ cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    যেতিয়ালৈ((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    যেতিয়ালৈ((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    যদি((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      গচ্ছ cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    গচ্ছ cleanup;
  }

  যদি(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_চাবি_MD5)) {
    ধ্রুৱ ৱর্ণ *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_চাবি_MD5);
    ৱর্ণ md5buffer[33];
    পূর্ণাঙ্ক i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    যদি(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      গচ্ছ cleanup;
    }

    বাবে(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    যদি(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      গচ্ছ cleanup;
    }

    rc = SSH_OK;
    গচ্ছ cleanup;
  }

  যদি(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    যদি(knownhostsentry) {
      যদি(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        যদি(rc != SSH_OK) {
          গচ্ছ cleanup;
        }
        knownkey.চাবি = known_base64;
        knownkey.len = strlen(known_base64);

        ৱিকল্প(ssh_চাবি_type(knownhostsentry->publickey)) {
        পক্ষ SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          ভঙ্গ;
        পক্ষ SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          ভঙ্গ;
        পক্ষ SSH_KEYTYPE_ECDSA:
        পক্ষ SSH_KEYTYPE_ECDSA_P256:
        পক্ষ SSH_KEYTYPE_ECDSA_P384:
        পক্ষ SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          ভঙ্গ;
        পক্ষ SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          ভঙ্গ;
        পক্ষ SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          ভঙ্গ;
        স্ৱভাৱ:
          rc = SSH_ERROR;
          গচ্ছ cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    ৱিকল্প(vstate) {
    পক্ষ SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      ভঙ্গ;
    পক্ষ SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      ভঙ্গ;
    পক্ষ SSH_KNOWN_HOSTS_NOT_FOUND:
    পক্ষ SSH_KNOWN_HOSTS_UNKNOWN:
    পক্ষ SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      ভঙ্গ;
    স্ৱভাৱ:
      keymatch = CURLKHMATCH_MISMATCH;
      ভঙ্গ;
    }

    যদি(func) { /* use callback to determine action */
      সংরচনা Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      যদি(rc != SSH_OK)
        গচ্ছ cleanup;

      foundkey.চাবি = found_base64;
      foundkey.len = strlen(found_base64);

      ৱিকল্প(ssh_চাবি_type(pubkey)) {
      পক্ষ SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        ভঙ্গ;
      পক্ষ SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        ভঙ্গ;
      পক্ষ SSH_KEYTYPE_ECDSA:
      পক্ষ SSH_KEYTYPE_ECDSA_P256:
      পক্ষ SSH_KEYTYPE_ECDSA_P384:
      পক্ষ SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        ভঙ্গ;
      পক্ষ SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        ভঙ্গ;
      পক্ষ SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        ভঙ্গ;
      স্ৱভাৱ:
        rc = SSH_ERROR;
        গচ্ছ cleanup;
      }

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      ৱিকল্প(rc) {
      পক্ষ CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        যদি(rc != SSH_OK) {
          গচ্ছ cleanup;
        }
        ভঙ্গ;
      পক্ষ CURLKHSTAT_FINE:
        ভঙ্গ;
      স্ৱভাৱ: /* REJECT/DEFER */
        rc = SSH_ERROR;
        গচ্ছ cleanup;
      }
    }
    অন্যথা {
      যদি(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        গচ্ছ cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  যদি(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    মুক্ত(found_base64); /* allocated by libssh, deallocate with system free */
  }
  যদি(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    মুক্ত(known_base64); /* allocated by libssh, deallocate with system free */
  }
  যদি(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  যদি(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_চাবি_free(pubkey);
  যদি(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  ঘূৰাই rc;
}

স্থির পূর্ণাঙ্ক myssh_to_ERROR(সংরচনা Curl_easy *data,
                          সংরচনা ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  ঘূৰাই SSH_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_to_SFTP_CLOSE(সংরচনা Curl_easy *data,
                               সংরচনা ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  ঘূৰাই SSH_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_to_PASSWD_AUTH(সংরচনা Curl_easy *data,
                                সংরচনা ssh_conn *sshc)
{
  যদি(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    ঘূৰাই SSH_OK;
  }
  ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

স্থির পূর্ণাঙ্ক myssh_to_চাবি_AUTH(সংরচনা Curl_easy *data,
                             সংরচনা ssh_conn *sshc)
{
  যদি(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_চাবি_INIT);
    ঘূৰাই SSH_OK;
  }
  ঘূৰাই myssh_to_PASSWD_AUTH(data, sshc);
}

স্থির পূর্ণাঙ্ক myssh_to_GSSAPI_AUTH(সংরচনা Curl_easy *data,
                                সংরচনা ssh_conn *sshc)
{
  যদি(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    ঘূৰাই SSH_OK;
  }
  ঘূৰাই myssh_to_চাবি_AUTH(data, sshc);
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_READDIR_INIT(সংরচনা Curl_easy *data,
                                      সংরচনা ssh_conn *sshc,
                                      সংরচনা SSHPROTO *sshp)
{
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  যদি(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    ঘূৰাই SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  যদি(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ঘূৰাই myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_READDIR(সংরচনা Curl_easy *data,
                                 সংরচনা ssh_conn *sshc,
                                 সংরচনা SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  যদি(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  যদি(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    যদি(data->set.list_only) {
      ৱর্ণ *tmp_সাৰি;

      tmp_সাৰি = curl_maprintf("%s\n", sshc->readdir_filename);
      যদি(!tmp_সাৰি) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        ঘূৰাই SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmp_সাৰি, sshc->readdir_len + 1);
      curlx_free(tmp_সাৰি);

      যদি(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        ঘূৰাই SSH_NO_ERROR;
      }
    }
    অন্যথা {
      যদি(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        ঘূৰাই SSH_ERROR;
      }

      যদি((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        যদি(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          ঘূৰাই SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        ঘূৰাই SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      ঘূৰাই SSH_NO_ERROR;
    }
  }
  অন্যথা যদি(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  অন্যথা {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ঘূৰাই myssh_to_SFTP_CLOSE(data, sshc);
  }
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_READDIR_LINK(সংরচনা Curl_easy *data,
                                      সংরচনা ssh_conn *sshc)
{
  যদি(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  যদি(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ঘূৰাই myssh_to_SFTP_CLOSE(data, sshc);
  }

  যদি(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    যদি(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    অন্যথা
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = রিক্ত;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  অন্যথা {
    sshc->readdir_len = strlen(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  যদি(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    ঘূৰাই SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = রিক্ত;
  sshc->readdir_filename = রিক্ত;
  sshc->readdir_longentry = রিক্ত;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_READDIR_BOTTOM(সংরচনা Curl_easy *data,
                                        সংরচনা ssh_conn *sshc)
{
  CURLcode result;

  যদি(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  অন্যথা
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = রিক্ত;

  যদি(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  অন্যথা
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_READDIR_DONE(সংরচনা Curl_easy *data,
                                      সংরচনা ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = রিক্ত;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির শূন্য myssh_quote_error(সংরচনা Curl_easy *data, সংরচনা ssh_conn *sshc,
                              ধ্রুৱ ৱর্ণ *cmd)
{
  যদি(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE_STATVFS(সংরচনা Curl_easy *data,
                                       সংরচনা ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  যদি(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    ঘূৰাই SSH_OK;
  }
  অন্যথা যদি(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#নোচেদ্যদি defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#অন্যথা
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    ৱর্ণ *tmp = curl_maprintf("statvfs:\n"
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

    যদি(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    যদি(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    যদি(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ঘূৰাই SSH_OK;
}

স্থির পূর্ণাঙ্ক myssh_auth_interactive(সংরচনা connectdata *conn,
                                  সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc;
  পূর্ণাঙ্ক nprompts;

restart:
  ৱিকল্প(sshc->kbd_state) {
  পক্ষ 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, রিক্ত, রিক্ত);
    যদি(rc == SSH_AUTH_AGAIN)
      ঘূৰাই SSH_AGAIN;

    যদি(rc != SSH_AUTH_INFO)
      ঘূৰাই SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    যদি(nprompts != 1)
      ঘূৰাই SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    যদি(rc < 0)
      ঘূৰাই SSH_ERROR;

    FALLTHROUGH();
  পক্ষ 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, রিক্ত, রিক্ত);
    যদি(rc == SSH_AUTH_AGAIN)
      ঘূৰাই SSH_AGAIN;
    অন্যথা যদি(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    অন্যথা যদি(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      যদি(nprompts)
        ঘূৰাই SSH_ERROR;

      sshc->kbd_state = 2;
      গচ্ছ restart;
    }
    অন্যথা
      rc = SSH_ERROR;
    ভঙ্গ;
  পক্ষ 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, রিক্ত, রিক্ত);
    যদি(rc == SSH_AUTH_AGAIN)
      ঘূৰাই SSH_AGAIN;
    অন্যথা যদি(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    অন্যথা
      rc = SSH_ERROR;

    ভঙ্গ;
  স্ৱভাৱ:
    ঘূৰাই SSH_ERROR;
  }

  sshc->kbd_state = 0;
  ঘূৰাই rc;
}

স্থির শূন্য myssh_state_init(সংরচনা Curl_easy *data,
                             সংরচনা ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#যদি 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

স্থির শূন্য myssh_block2waitfor(সংরচনা connectdata *conn,
                                সংরচনা ssh_conn *sshc,
                                bool block)
{
  (শূন্য)conn;
  যদি(block) {
    পূর্ণাঙ্ক dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  অন্যথা
    sshc->waitfor = 0;
}

স্থির পূর্ণাঙ্ক myssh_in_S_STARTUP(সংরচনা Curl_easy *data,
                              সংরচনা ssh_conn *sshc)
{
  সংরচনা connectdata *conn = data->conn;
  পূর্ণাঙ্ক rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  যদি(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  অন্যথা যদি(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  অন্যথা
    myssh_to(data, sshc, SSH_HOSTKEY);

  ঘূৰাই rc;
}

স্থির পূর্ণাঙ্ক myssh_in_AUTHLIST(সংরচনা Curl_easy *data,
                             সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, রিক্ত);
  যদি(rc == SSH_AUTH_AGAIN)
    ঘূৰাই SSH_AGAIN;

  যদি(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ঘূৰাই rc;
  }
  অন্যথা যদি(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    ঘূৰাই rc;
  }

  sshc->auth_methods =
    (unsigned পূর্ণাঙ্ক)ssh_userauth_list(sshc->ssh_session, রিক্ত);
  যদি(sshc->auth_methods)
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
  যদি((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_চাবি && sshc->priv_চাবি[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  অন্যথা যদি(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  অন্যথা যদি(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_চাবি_INIT);
  }
  অন্যথা যদি(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  অন্যথা {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  ঘূৰাই rc;
}

স্থির পূর্ণাঙ্ক myssh_in_AUTH_PKEY_INIT(সংরচনা Curl_easy *data,
                                   সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc;
  যদি(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    ঘূৰাই rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  যদি(sshc->priv_চাবি && sshc->priv_চাবি[0]) {
    যদি(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, রিক্ত, sshc->pubkey);
      যদি(rc == SSH_AUTH_AGAIN)
        ঘূৰাই SSH_AGAIN;

      যদি(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        ঘূৰাই rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_চাবি,
                                     sshc->passphrase, রিক্ত,
                                     রিক্ত, &sshc->privkey);
    যদি(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_চাবি);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      ঘূৰাই rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  অন্যথা {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, রিক্ত,
                                     sshc->passphrase);
    যদি(rc == SSH_AUTH_AGAIN)
      ঘূৰাই SSH_AGAIN;

    যদি(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      ঘূৰাই rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  ঘূৰাই rc;
}

স্থির পূর্ণাঙ্ক myssh_in_AUTH_PKEY(সংরচনা Curl_easy *data,
                              সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc = ssh_userauth_publickey(sshc->ssh_session, রিক্ত, sshc->privkey);
  যদি(rc == SSH_AUTH_AGAIN)
    ঘূৰাই SSH_AGAIN;
  অন্যথা যদি(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ঘূৰাই SSH_OK;
  }
  অন্যথা {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    ঘূৰাই myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

স্থির পূর্ণাঙ্ক myssh_in_AUTH_GSSAPI(সংরচনা Curl_easy *data,
                                সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc;
  যদি(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    ঘূৰাই myssh_to_চাবি_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  যদি(rc == SSH_AUTH_AGAIN)
    ঘূৰাই SSH_AGAIN;

  যদি(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ঘূৰাই SSH_OK;
  }

  ঘূৰাই myssh_to_চাবি_AUTH(data, sshc);
}

স্থির পূর্ণাঙ্ক myssh_in_AUTH_চাবি_INIT(সংরচনা Curl_easy *data,
                                  সংরচনা ssh_conn *sshc)
{
  যদি(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_চাবি);
    ঘূৰাই SSH_NO_ERROR;
  }
  ঘূৰাই myssh_to_PASSWD_AUTH(data, sshc);
}

স্থির পূর্ণাঙ্ক myssh_in_AUTH_চাবি(সংরচনা Curl_easy *data,
                             সংরচনা ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  পূর্ণাঙ্ক rc = myssh_auth_interactive(data->conn, sshc);
  যদি(rc == SSH_AGAIN)
    ঘূৰাই rc;
  অন্যথা যদি(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ঘূৰাই SSH_NO_ERROR;
  }
  অন্যথা
    ঘূৰাই myssh_to_PASSWD_AUTH(data, sshc);
}

স্থির পূর্ণাঙ্ক myssh_in_AUTH_PASS_INIT(সংরচনা Curl_easy *data,
                                   সংরচনা ssh_conn *sshc)
{
  যদি(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_AUTH_PASS(সংরচনা Curl_easy *data,
                              সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc = ssh_userauth_password(sshc->ssh_session, রিক্ত,
                                 Curl_creds_passwd(data->conn->creds));
  যদি(rc == SSH_AUTH_AGAIN)
    ঘূৰাই SSH_AGAIN;
  অন্যথা যদি(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ঘূৰাই SSH_NO_ERROR;
  }
  ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

স্থির পূর্ণাঙ্ক myssh_in_AUTH_DONE(সংরচনা Curl_easy *data,
                              সংরচনা ssh_conn *sshc)
{
  সংরচনা connectdata *conn = data->conn;
  যদি(!sshc->authed) {
    failf(data, "Authentication failure");
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  যদি(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    ঘূৰাই SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_UPLOAD_INIT(সংরচনা Curl_easy *data,
                                সংরচনা ssh_conn *sshc,
                                সংরচনা SSHPROTO *sshp)
{
  পূর্ণাঙ্ক flags;
  পূর্ণাঙ্ক rc = 0;

  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  যদি(data->state.resume_from) {
    sftp_attributes attrs;

    যদি(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      যদি(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        যদি(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          ঘূৰাই rc;
        }
        data->state.resume_from = size;
      }
      অন্যথা {
        data->state.resume_from = 0;
      }
    }
  }

  যদি(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  অন্যথা যদি(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  অন্যথা
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  যদি(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  যদি(!sshc->sftp_file) {
    পূর্ণাঙ্ক err = sftp_get_error(sshc->sftp_session);

    যদি((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ঘূৰাই rc;
    }
    অন্যথা {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      ঘূৰাই rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  যদি(data->state.resume_from > 0 && !data->set.remote_append) {
    পূর্ণাঙ্ক seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    যদি(data->set.seek_func) {
      সংরচনা Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    যদি(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      যদি(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        ঘূৰাই rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      কৰা {
        ৱর্ণ scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)পরিমাণ(scratch)) ?
          পরিমাণ(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        যদি((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          ঘূৰাই rc;
        }
      } যেতিয়ালৈ(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    যদি(data->state.infilesize > 0) {
      যদি(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    যদি(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      ঘূৰাই rc;
    }
  }
  যদি(data->state.infilesize > 0) {
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
#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  ঘূৰাই rc;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_DOWNLOAD_INIT(সংরচনা Curl_easy *data,
                                       সংরচনা ssh_conn *sshc,
                                       সংরচনা SSHPROTO *sshp)
{
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  যদি(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  যদি(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    ঘূৰাই myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_DOWNLOAD_STAT(সংরচনা Curl_easy *data,
                                       সংরচনা ssh_conn *sshc)
{
  curl_off_t size;
  পূর্ণাঙ্ক rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  যদি(!attrs ||
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
    যদি(attrs)
      sftp_attributes_free(attrs);
  }
  অন্যথা {
    size = attrs->size;

    sftp_attributes_free(attrs);

    যদি(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    যদি(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      যদি(result)
        ঘূৰাই myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      যদি(rc)
        ঘূৰাই myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  যদি(data->state.resume_from) {
    যদি(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      যদি(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    অন্যথা {
      যদি(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    যদি(rc)
      ঘূৰাই myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  যদি(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    ঘূৰাই rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  ঘূৰাই rc;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_CLOSE(সংরচনা Curl_easy *data,
                               সংরচনা ssh_conn *sshc,
                               সংরচনা SSHPROTO *sshp)
{
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  যদি(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = রিক্ত;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  যদি(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  অন্যথা {
    myssh_to(data, sshc, SSH_STOP);
  }
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_SHUTDOWN(সংরচনা Curl_easy *data,
                                  সংরচনা ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  যদি(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = রিক্ত;
  }

  যদি(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = রিক্ত;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_INIT(সংরচনা Curl_easy *data,
                              সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  যদি(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  যদি(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ঘূৰাই myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_REALPATH(সংরচনা Curl_easy *data,
                                  সংরচনা ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  যদি(!sshc->homedir)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  যদি(!data->state.most_recent_ftp_entrypath)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE_INIT(সংরচনা Curl_easy *data,
                                    সংরচনা ssh_conn *sshc,
                                    সংরচনা SSHPROTO *sshp)
{
  CURLcode result;

  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  যদি(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  অন্যথা যদি(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  অন্যথা
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_POSTQUOTE_INIT(সংরচনা Curl_easy *data,
                                        সংরচনা ssh_conn *sshc)
{
  যদি(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  অন্যথা {
    myssh_to(data, sshc, SSH_STOP);
  }
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক quote_error(সংরচনা Curl_easy *data,
                       সংরচনা ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE(সংরচনা Curl_easy *data,
                               সংরচনা ssh_conn *sshc,
                               সংরচনা SSHPROTO *sshp)
{
  ধ্রুৱ ৱর্ণ *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  ধ্রুৱ ৱর্ণ *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  যদি(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  যদি(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    ৱর্ণ *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    যদি(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ঘূৰাই SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    যদি(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    অন্যথা
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    ঘূৰাই SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  যদি(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    ঘূৰাই SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  যদি(result) {
    যদি(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    অন্যথা
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    ঘূৰাই SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  যদি(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    যদি(result) {
      যদি(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      অন্যথা
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ঘূৰাই SSH_NO_ERROR;
    }
    যদি(*cp)
      ঘূৰাই quote_error(data, sshc);
    sshc->quote_attrs = রিক্ত;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    ঘূৰাই SSH_NO_ERROR;
  }
  যদি(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    যদি(result) {
      যদি(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      অন্যথা
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ঘূৰাই SSH_NO_ERROR;
    }
    যদি(*cp)
      ঘূৰাই quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ঘূৰাই SSH_NO_ERROR;
  }
  অন্যথা যদি(!strncmp(cmd, "mkdir ", 6)) {
    যদি(*cp)
      ঘূৰাই quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    ঘূৰাই SSH_NO_ERROR;
  }
  অন্যথা যদি(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    যদি(result) {
      যদি(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      অন্যথা
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ঘূৰাই SSH_NO_ERROR;
    }
    যদি(*cp)
      ঘূৰাই quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    ঘূৰাই SSH_NO_ERROR;
  }
  অন্যথা যদি(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    যদি(*cp)
      ঘূৰাই quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    ঘূৰাই SSH_NO_ERROR;
  }
  অন্যথা যদি(!strncmp(cmd, "rm ", 3)) {
    যদি(*cp)
      ঘূৰাই quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    ঘূৰাই SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  অন্যথা যদি(!strncmp(cmd, "statvfs ", 8)) {
    যদি(*cp)
      ঘূৰাই quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    ঘূৰাই SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_NEXT_QUOTE(সংরচনা Curl_easy *data,
                                    সংরচনা ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  যদি(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  অন্যথা {
    যদি(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    অন্যথা {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE_STAT(সংরচনা Curl_easy *data,
                                    সংরচনা ssh_conn *sshc)
{
  ধ্রুৱ ৱর্ণ *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  যদি(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  যদি(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  যদি(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, রিক্ত);
    ঘূৰাই SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  যদি(!strncmp(cmd, "chgrp", 5)) {
    ধ্রুৱ ৱর্ণ *p = sshc->quote_path1;
    curl_off_t gid;
    যদি(curlx_str_সংখ্যা(&p, &gid, UINT_MAX)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, রিক্ত);
      ঘূৰাই SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  অন্যথা যদি(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ধ্রুৱ ৱর্ণ *p = sshc->quote_path1;
    যদি(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, রিক্ত);
      ঘূৰাই SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  অন্যথা যদি(!strncmp(cmd, "chown", 5)) {
    ধ্রুৱ ৱর্ণ *p = sshc->quote_path1;
    curl_off_t uid;
    যদি(curlx_str_সংখ্যা(&p, &uid, UINT_MAX)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, রিক্ত);
      ঘূৰাই SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  অন্যথা যদি(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    যদি(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#যদি SIZEOF_TIME_T > 4
    অন্যথা যদি(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    যদি(fail) {
      myssh_quote_error(data, sshc, রিক্ত);
      ঘূৰাই SSH_NO_ERROR;
    }
    যদি(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    অন্যথা /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির শূন্য conn_forget_socket(সংরচনা Curl_easy *data, int8_t sockindex)
{
  সংরচনা connectdata *conn = data->conn;
  যদি(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    সংরচনা Curl_cfilter *cf = conn->cfilter[sockindex];
    যদি(cf)
      (শূন্য)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, রিক্ত);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

স্থির শূন্য myssh_SESSION_DISCONNECT(সংরচনা Curl_easy *data,
                                     সংরচনা ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  যদি(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = রিক্ত;
  }

  যদি(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = রিক্ত;
  }
  যদি(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = রিক্ত;
  }

  ssh_disconnect(sshc->ssh_session);
  যদি(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

স্থির পূর্ণাঙ্ক myssh_SSH_SCP_DOWNLOAD(সংরচনা Curl_easy *data,
                                  সংরচনা ssh_conn *sshc)
{
  uint64_t bytecount;
  পূর্ণাঙ্ক rc = ssh_scp_pull_request(sshc->scp_session);
  যদি(rc != SSH_SCP_REQUEST_NEWFILE) {
    ধ্রুৱ ৱর্ণ *err_বাৰ্তা = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_বাৰ্তা);
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  যদি(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক myssh_in_TRANS_INIT(সংরচনা Curl_easy *data, সংরচনা ssh_conn *sshc,
                               সংরচনা SSHPROTO *sshp)
{
  CURLcode result;
  পূর্ণাঙ্ক rc = 0;
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  যদি(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    ঘূৰাই 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  যদি(data->state.upload) {
    যদি(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  অন্যথা {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  যদি(!sshc->scp_session) {
    ধ্রুৱ ৱর্ণ *err_বাৰ্তা = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_বাৰ্তা);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  ঘূৰাই rc;
}

স্থির শূন্য sshc_cleanup(সংরচনা ssh_conn *sshc)
{
  যদি(sshc->initialized) {
    যদি(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = রিক্ত;
    }
    যদি(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = রিক্ত;
    }
    যদি(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = রিক্ত;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    যদি(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = রিক্ত;
    }
    যদি(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = রিক্ত;
    }
    যদি(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = রিক্ত;
    }
    যদি(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = রিক্ত;
    }
    যদি(sshc->privkey) {
      ssh_চাবি_free(sshc->privkey);
      sshc->privkey = রিক্ত;
    }
    যদি(sshc->pubkey) {
      ssh_চাবি_free(sshc->pubkey);
      sshc->pubkey = রিক্ত;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_চাবি);
    curlx_safefree(sshc->priv_চাবি);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE_SETSTAT(সংরচনা Curl_easy *data,
                                       সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  যদি(rc == SSH_AGAIN)
    ঘূৰাই rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    ঘূৰাই rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE_SYMLINK(সংরচনা Curl_easy *data,
                                       সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  যদি(rc == SSH_AGAIN)
    ঘূৰাই rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    ঘূৰাই rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE_MKDIR(সংরচনা Curl_easy *data,
                                     সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  যদি(rc == SSH_AGAIN)
    ঘূৰাই rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    ঘূৰাই rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE_RENAME(সংরচনা Curl_easy *data,
                                      সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  যদি(rc == SSH_AGAIN)
    ঘূৰাই rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    ঘূৰাই rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE_RMDIR(সংরচনা Curl_easy *data,
                                     সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  যদি(rc == SSH_AGAIN)
    ঘূৰাই rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    ঘূৰাই rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_QUOTE_UNLINK(সংরচনা Curl_easy *data,
                                      সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  যদি(rc == SSH_AGAIN)
    ঘূৰাই rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    ঘূৰাই rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_GETINFO(সংরচনা Curl_easy *data,
                                 সংরচনা ssh_conn *sshc)
{
  যদি(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  অন্যথা
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_FILETIME(সংরচনা Curl_easy *data,
                                  সংরচনা ssh_conn *sshc,
                                  সংরচনা SSHPROTO *sshp)
{
  sftp_attributes attrs;
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  যদি(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_TRANS_INIT(সংরচনা Curl_easy *data,
                                    সংরচনা ssh_conn *sshc,
                                    সংরচনা SSHPROTO *sshp)
{
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  যদি(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  অন্যথা {
    size_t path_len = strlen(sshp->path);

    যদি(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    অন্যথা
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_CREATE_DIRS_INIT(সংরচনা Curl_easy *data,
                                          সংরচনা ssh_conn *sshc,
                                          সংরচনা SSHPROTO *sshp)
{
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  যদি(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  অন্যথা {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_CREATE_DIRS(সংরচনা Curl_easy *data,
                                     সংরচনা ssh_conn *sshc,
                                     সংরচনা SSHPROTO *sshp)
{
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  যদি(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ঘূৰাই SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SFTP_CREATE_DIRS_MKDIR(সংরচনা Curl_easy *data,
                                           সংরচনা ssh_conn *sshc,
                                           সংরচনা SSHPROTO *sshp)
{
  পূর্ণাঙ্ক rc;
  পূর্ণাঙ্ক err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  যদি(rc == SSH_AGAIN)
    ঘূৰাই rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  যদি(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    যদি((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      ঘূৰাই myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  ঘূৰাই rc;
}

স্থির পূর্ণাঙ্ক myssh_in_SCP_UPLOAD_INIT(সংরচনা Curl_easy *data,
                                    সংরচনা ssh_conn *sshc,
                                    সংরচনা SSHPROTO *sshp)
{
  পূর্ণাঙ্ক rc;
  যদি(!sshp)
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  যদি(rc != SSH_OK) {
    ধ্রুৱ ৱর্ণ *err_বাৰ্তা = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_বাৰ্তা);
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ঠেলো_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (পূর্ণাঙ্ক)data->set.new_file_perms);

  যদি(rc != SSH_OK) {
    ধ্রুৱ ৱর্ণ *err_বাৰ্তা = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_বাৰ্তা);
    ঘূৰাই myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SCP_DOWNLOAD_INIT(সংরচনা Curl_easy *data,
                                      সংরচনা ssh_conn *sshc)
{
  পূর্ণাঙ্ক rc = ssh_scp_init(sshc->scp_session);
  যদি(rc != SSH_OK) {
    ধ্রুৱ ৱর্ণ *err_বাৰ্তা = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_বাৰ্তা);
    ঘূৰাই myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SCP_DONE(সংরচনা Curl_easy *data,
                             সংরচনা ssh_conn *sshc)
{
  যদি(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  অন্যথা
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SCP_SEND_EOF(সংরচনা Curl_easy *data,
                                 সংরচনা ssh_conn *sshc)
{
  যদি(sshc->scp_session) {
    পূর্ণাঙ্ক rc = ssh_scp_close(sshc->scp_session);
    যদি(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      ঘূৰাই SSH_AGAIN;
    }
    যদি(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির পূর্ণাঙ্ক myssh_in_SCP_CHANNEL_FREE(সংরচনা Curl_easy *data,
                                     সংরচনা ssh_conn *sshc)
{
  যদি(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = রিক্ত;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ঘূৰাই SSH_NO_ERROR;
}

স্থির CURLcode myssh_in_SESSION_FREE(সংরচনা Curl_easy *data,
                                      সংরচনা ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  memset(sshc, 0, পরিমাণ(সংরচনা ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  ঘূৰাই result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
স্থির CURLcode myssh_statemachine(সংরচনা Curl_easy *data,
                                   সংরচনা ssh_conn *sshc,
                                   সংরচনা SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  কৰা {

    ৱিকল্প(sshc->state) {
    পক্ষ SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    পক্ষ SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      যদি(rc)
        ভঙ্গ;
      FALLTHROUGH();
    পক্ষ SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      যদি(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        ভঙ্গ;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    পক্ষ SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_AUTH_চাবি_INIT:
      rc = myssh_in_AUTH_চাবি_INIT(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_AUTH_চাবি:
      rc = myssh_in_AUTH_চাবি(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      ভঙ্গ;
    পক্ষ SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      যদি(rc)
        ভঙ্গ;
      FALLTHROUGH();
    পক্ষ SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    পক্ষ SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    পক্ষ SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      ভঙ্গ;
    পক্ষ SSH_QUIT:
    স্ৱভাৱ:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      ভঙ্গ;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } যেতিয়ালৈ((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  যদি(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  যদি(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (পূর্ণাঙ্ক)result, *block);
  ঘূৰাই result;
}

/* called repeatedly until done from multi.c */
স্থির CURLcode myssh_multi_statemach(সংরচনা Curl_easy *data,
                                      bool *done)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  সংরচনা SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  যদি(!sshc || !sshp)
    ঘূৰাই CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  ঘূৰাই result;
}

স্থির CURLcode myssh_block_statemach(সংরচনা Curl_easy *data,
                                      সংরচনা ssh_conn *sshc,
                                      সংরচনা SSHPROTO *sshp,
                                      bool disconnect)
{
  সংরচনা connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  যেতিয়ালৈ((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t বাম_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
    যদি(result)
      ভঙ্গ;

    যদি(!disconnect) {
      result = Curl_pgrsCheck(data);
      যদি(result)
        ভঙ্গ;

      বাম_ms = Curl_timeleft_ms(data);
      যদি(বাম_ms < 0) {
        failf(data, "Operation timed out");
        ঘূৰাই CURLE_OPERATION_TIMEDOUT;
      }
    }

    যদি(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (শূন্য)SOCKET_READABLE(fd_read,
                            (!বাম_ms || বাম_ms > 1000) ? 1000 : বাম_ms);
    }
  }

  ঘূৰাই result;
}

স্থির শূন্য myssh_easy_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা SSHPROTO *sshp = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

স্থির শূন্য myssh_conn_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা ssh_conn *sshc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
স্থির CURLcode myssh_setup_connection(সংরচনা Curl_easy *data,
                                       সংরচনা connectdata *conn)
{
  সংরচনা SSHPROTO *sshp;
  সংরচনা ssh_conn *sshc;

  sshc = curlx_calloc(1, পরিমাণ(*sshc));
  যদি(!sshc)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  যদি(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, পরিমাণ(*sshp));
  যদি(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  ঘূৰাই Curl_ssh_setup_pkey(data, sshc);
}

স্থির Curl_recv scp_recv, sftp_recv;
স্থির Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
স্থির CURLcode myssh_connect(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  সংরচনা SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  পূর্ণাঙ্ক rc;

  যদি(!sshc || !ssh)
    ঘূৰাই CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  যদি(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  অন্যথা {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  যদি(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    ঘূৰাই CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  যদি(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    ঘূৰাই CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, রিক্ত);
  যদি(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  যদি(rc != SSH_OK) {
    failf(data, "Could not set socket");
    ঘূৰাই CURLE_FAILED_INIT;
  }

  যদি(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    যদি(rc != SSH_OK) {
      failf(data, "Could not set user");
      ঘূৰাই CURLE_FAILED_INIT;
    }
  }

  যদি(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    যদি(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    যদি(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      ঘূৰাই CURLE_FAILED_INIT;
    }
  }

  যদি(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    যদি(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      ঘূৰাই CURLE_FAILED_INIT;
    }
  }

  যদি(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    যদি(rc != SSH_OK) {
      failf(data, "Could not set compression");
      ঘূৰাই CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = রিক্ত;
  sshc->pubkey = রিক্ত;

  যদি(sshc->pub_চাবি) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_চাবি, &sshc->pubkey);
    যদি(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      ঘূৰাই CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  ঘূৰাই result;
}

/* called from multi.c while DOing */
স্থির CURLcode scp_doing(সংরচনা Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  যদি(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ঘূৰাই result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

স্থির CURLcode scp_perform(সংরচনা Curl_easy *data,
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  যদি(!sshc)
    ঘূৰাই CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  যদি(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ঘূৰাই result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
স্থির CURLcode scp_disconnect(সংরচনা Curl_easy *data,
                               সংরচনা connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  সংরচনা SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (শূন্য)dead_connection;

  যদি(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  ঘূৰাই result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
স্থির CURLcode myssh_done(সংরচনা Curl_easy *data,
                           সংরচনা ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  সংরচনা SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  যদি(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
  অন্যথা
    result = status;

  যদি(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ঘূৰাই CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  ঘূৰাই result;
}

স্থির CURLcode scp_done(সংরচনা Curl_easy *data, CURLcode status,
                         bool premature)
{
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (শূন্য)premature;

  যদি(!sshc)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  ঘূৰাই myssh_done(data, sshc, status);
}

স্থির CURLcode scp_send(সংরচনা Curl_easy *data, int8_t sockindex,
                         ধ্রুৱ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  পূর্ণাঙ্ক rc;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (শূন্য)sockindex; /* we only support SCP on the fixed known primary socket */
  (শূন্য)eos;
  *pnwritten = 0;

  যদি(!sshc)
    ঘূৰাই CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#যদি 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  যদি(rc == SSH_AGAIN)
    ঘূৰাই CURLE_AGAIN;
  অন্যথা
#endif
  যদি(rc != SSH_OK)
    ঘূৰাই CURLE_SSH;

  *pnwritten = len;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode scp_recv(সংরচনা Curl_easy *data, int8_t sockindex,
                         ৱর্ণ *mem, size_t len, size_t *pnread)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  পূর্ণাঙ্ক nread;

  (শূন্য)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  যদি(!sshc)
    ঘূৰাই CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  যদি(nread == SSH_ERROR)
    ঘূৰাই CURLE_SSH;
#যদি 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  যদি(nread == SSH_AGAIN)
    ঘূৰাই CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  ঘূৰাই CURLE_OK;
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

স্থির CURLcode sftp_perform(সংরচনা Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  যদি(!sshc)
    ঘূৰাই CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  যদি(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ঘূৰাই result;
}

/* called from multi.c while DOing */
স্থির CURLcode sftp_doing(সংরচনা Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  যদি(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ঘূৰাই result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
স্থির CURLcode sftp_disconnect(সংরচনা Curl_easy *data,
                                সংরচনা connectdata *conn,
                                bool dead_connection)
{
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  সংরচনা SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (শূন্য)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  যদি(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
  ঘূৰাই result;
}

স্থির CURLcode sftp_done(সংরচনা Curl_easy *data, CURLcode status,
                          bool premature)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  যদি(!sshc)
    ঘূৰাই CURLE_FAILED_INIT;
  যদি(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    যদি(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  ঘূৰাই myssh_done(data, sshc, status);
}

/* return number of sent bytes */
স্থির CURLcode sftp_send(সংরচনা Curl_easy *data, int8_t sockindex,
                          ধ্রুৱ uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (শূন্য)sockindex;
  (শূন্য)eos;
  *pnwritten = 0;

  যদি(!sshc)
    ঘূৰাই CURLE_FAILED_INIT;

#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  ৱিকল্প(sshc->sftp_send_state) {
  পক্ষ 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    যদি(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      ঘূৰাই CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  পক্ষ 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    যদি(nwrite == SSH_AGAIN)
      ঘূৰাই CURLE_AGAIN;
    অন্যথা যদি(nwrite < 0)
      ঘূৰাই CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    ঘূৰাই CURLE_OK;
  স্ৱভাৱ:
    /* we never reach here */
    ঘূৰাই CURLE_SEND_ERROR;
  }
#অন্যথা
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  যদি(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#যদি 0 /* not returned by libssh on write */
  যদি(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  অন্যথা
#endif
  যদি(nwrite < 0)
    ঘূৰাই CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  ঘূৰাই CURLE_OK;
#endif
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
স্থির CURLcode sftp_recv(সংরচনা Curl_easy *data, int8_t sockindex,
                          ৱর্ণ *mem, size_t len, size_t *pnread)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (শূন্য)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  যদি(!sshc)
    ঘূৰাই CURLE_FAILED_INIT;

  ৱিকল্প(sshc->sftp_recv_state) {
  পক্ষ 0:
#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    যদি(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      ঘূৰাই CURLE_RECV_ERROR;
    }
#অন্যথা
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    যদি(sshc->sftp_file_index < 0)
      ঘূৰাই CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  পক্ষ 1:
    sshc->sftp_recv_state = 1;

#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#অন্যথা
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    যদি(nread == SSH_AGAIN)
      ঘূৰাই CURLE_AGAIN;
    অন্যথা যদি(nread < 0)
      ঘূৰাই CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    ঘূৰাই CURLE_OK;

  স্ৱভাৱ:
    /* we never reach here */
    ঘূৰাই CURLE_RECV_ERROR;
  }
}

স্থির CURLcode myssh_do_it(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  যদি(!sshc)
    ঘূৰাই CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  যদি(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  অন্যথা
    result = sftp_perform(data, &connected, done);

  ঘূৰাই result;
}

CURLcode Curl_ssh_init(শূন্য)
{
  যদি(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    ঘূৰাই CURLE_FAILED_INIT;
  }
  ঘূৰাই CURLE_OK;
}

শূন্য Curl_ssh_cleanup(শূন্য)
{
  (শূন্য)ssh_finalize();
}

শূন্য Curl_ssh_version(ৱর্ণ *buffer, size_t buflen)
{
  (শূন্য)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_scp = {
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
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_sftp = {
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
