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
#ضمن "curl_setup.h"

#ifdef USE_LIBSSH

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

#ضمن "urldata.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "transfer.h"
#ضمن "vssh/ssh.h"
#ضمن "url.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "parsedate.h"          /* for the week day and month names */
#ضمن "curlx/strparse.h"
#ضمن "multiif.h"
#ضمن "select.h"
#ضمن "vssh/vssh.h"
#ضمن "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#ضمن <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#ضمن <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  ܥܒܕ {                          \
    ܐܢ(x) {                     \
      ssh_string_free_char(x);  \
      x = عدم;                 \
    }                           \
  } ܟܡܐ(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

ساكن CURLcode sftp_error_to_CURLE(صحيح err)
{
  بدل(err) {
  حالة SSH_FX_OK:
    ܦܢܐ CURLE_OK;

  حالة SSH_FX_NO_SUCH_FILE:
  حالة SSH_FX_NO_SUCH_PATH:
    ܦܢܐ CURLE_REMOTE_FILE_NOT_FOUND;

  حالة SSH_FX_PERMISSION_DENIED:
  حالة SSH_FX_WRITE_PROTECT:
    ܦܢܐ CURLE_REMOTE_ACCESS_DENIED;

  حالة SSH_FX_FILE_ALREADY_EXISTS:
    ܦܢܐ CURLE_REMOTE_FILE_EXISTS;

  افتراضي:
    اكسر;
  }

  ܦܢܐ CURLE_SSH;
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
ساكن صحيح myssh_is_known(بنية Curl_easy *data, بنية ssh_conn *sshc)
{
  صحيح rc;
  ssh_key pubkey;
  unsigned حرف *hash_sha256 = عدم;
  size_t hlen_sha256;
  unsigned حرف *hash_md5 = عدم;
  size_t hlen_md5;
  حرف *found_base64 = عدم;
  حرف *known_base64 = عدم;
  صحيح vstate;
  تعداد curl_khmatch keymatch;
  بنية curl_khkey foundkey;
  بنية curl_khkey *knownkeyp = عدم;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  بنية ssh_knownhosts_entry *knownhostsentry = عدم;
  بنية curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  ܐܢ(rc != SSH_OK)
    ܦܢܐ rc;

  ܐܢ(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256)) {
    ثابت حرف *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256);
    حرف *fingerprint_b64 = عدم;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    ܐܢ(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      اذهب cleanup;
    }

    ܐܢ(curlx_base64_encode((ثابت uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      اذهب cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    ܟܡܐ((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    ܟܡܐ((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    ܐܢ((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      اذهب cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    اذهب cleanup;
  }

  ܐܢ(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_MD5)) {
    ثابت حرف *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_MD5);
    حرف md5buffer[33];
    صحيح i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    ܐܢ(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      اذهب cleanup;
    }

    لكل(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    ܐܢ(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      اذهب cleanup;
    }

    rc = SSH_OK;
    اذهب cleanup;
  }

  ܐܢ(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    ܐܢ(knownhostsentry) {
      ܐܢ(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        ܐܢ(rc != SSH_OK) {
          اذهب cleanup;
        }
        knownkey.key = known_base64;
        knownkey.len = strlen(known_base64);

        بدل(ssh_key_type(knownhostsentry->publickey)) {
        حالة SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          اكسر;
        حالة SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          اكسر;
        حالة SSH_KEYTYPE_ECDSA:
        حالة SSH_KEYTYPE_ECDSA_P256:
        حالة SSH_KEYTYPE_ECDSA_P384:
        حالة SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          اكسر;
        حالة SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          اكسر;
        حالة SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          اكسر;
        افتراضي:
          rc = SSH_ERROR;
          اذهب cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    بدل(vstate) {
    حالة SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      اكسر;
    حالة SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      اكسر;
    حالة SSH_KNOWN_HOSTS_NOT_FOUND:
    حالة SSH_KNOWN_HOSTS_UNKNOWN:
    حالة SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      اكسر;
    افتراضي:
      keymatch = CURLKHMATCH_MISMATCH;
      اكسر;
    }

    ܐܢ(func) { /* use callback to determine action */
      بنية Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      ܐܢ(rc != SSH_OK)
        اذهب cleanup;

      foundkey.key = found_base64;
      foundkey.len = strlen(found_base64);

      بدل(ssh_key_type(pubkey)) {
      حالة SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        اكسر;
      حالة SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        اكسر;
      حالة SSH_KEYTYPE_ECDSA:
      حالة SSH_KEYTYPE_ECDSA_P256:
      حالة SSH_KEYTYPE_ECDSA_P384:
      حالة SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        اكسر;
      حالة SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        اكسر;
      حالة SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        اكسر;
      افتراضي:
        rc = SSH_ERROR;
        اذهب cleanup;
      }

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      بدل(rc) {
      حالة CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        ܐܢ(rc != SSH_OK) {
          اذهب cleanup;
        }
        اكسر;
      حالة CURLKHSTAT_FINE:
        اكسر;
      افتراضي: /* REJECT/DEFER */
        rc = SSH_ERROR;
        اذهب cleanup;
      }
    }
    ܐܚܪܢܐ {
      ܐܢ(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        اذهب cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  ܐܢ(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    حرر(found_base64); /* allocated by libssh, deallocate with system free */
  }
  ܐܢ(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    حرر(known_base64); /* allocated by libssh, deallocate with system free */
  }
  ܐܢ(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  ܐܢ(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_key_free(pubkey);
  ܐܢ(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  ܦܢܐ rc;
}

ساكن صحيح myssh_to_ERROR(بنية Curl_easy *data,
                          بنية ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  ܦܢܐ SSH_ERROR;
}

ساكن صحيح myssh_to_SFTP_CLOSE(بنية Curl_easy *data,
                               بنية ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  ܦܢܐ SSH_ERROR;
}

ساكن صحيح myssh_to_PASSWD_AUTH(بنية Curl_easy *data,
                                بنية ssh_conn *sshc)
{
  ܐܢ(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    ܦܢܐ SSH_OK;
  }
  ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

ساكن صحيح myssh_to_KEY_AUTH(بنية Curl_easy *data,
                             بنية ssh_conn *sshc)
{
  ܐܢ(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_KEY_INIT);
    ܦܢܐ SSH_OK;
  }
  ܦܢܐ myssh_to_PASSWD_AUTH(data, sshc);
}

ساكن صحيح myssh_to_GSSAPI_AUTH(بنية Curl_easy *data,
                                بنية ssh_conn *sshc)
{
  ܐܢ(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    ܦܢܐ SSH_OK;
  }
  ܦܢܐ myssh_to_KEY_AUTH(data, sshc);
}

ساكن صحيح myssh_in_SFTP_READDIR_INIT(بنية Curl_easy *data,
                                      بنية ssh_conn *sshc,
                                      بنية SSHPROTO *sshp)
{
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  ܐܢ(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    ܦܢܐ SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  ܐܢ(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ܦܢܐ myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_READDIR(بنية Curl_easy *data,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  ܐܢ(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  ܐܢ(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    ܐܢ(data->set.list_only) {
      حرف *tmpLine;

      tmpLine = curl_maprintf("%s\n", sshc->readdir_filename);
      ܐܢ(!tmpLine) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        ܦܢܐ SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmpLine, sshc->readdir_len + 1);
      curlx_free(tmpLine);

      ܐܢ(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        ܦܢܐ SSH_NO_ERROR;
      }
    }
    ܐܚܪܢܐ {
      ܐܢ(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        ܦܢܐ SSH_ERROR;
      }

      ܐܢ((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        ܐܢ(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          ܦܢܐ SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        ܦܢܐ SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      ܦܢܐ SSH_NO_ERROR;
    }
  }
  ܐܚܪܢܐ ܐܢ(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  ܐܚܪܢܐ {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ܦܢܐ myssh_to_SFTP_CLOSE(data, sshc);
  }
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_READDIR_LINK(بنية Curl_easy *data,
                                      بنية ssh_conn *sshc)
{
  ܐܢ(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  ܐܢ(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ܦܢܐ myssh_to_SFTP_CLOSE(data, sshc);
  }

  ܐܢ(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    ܐܢ(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    ܐܚܪܢܐ
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = عدم;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  ܐܚܪܢܐ {
    sshc->readdir_len = strlen(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  ܐܢ(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    ܦܢܐ SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = عدم;
  sshc->readdir_filename = عدم;
  sshc->readdir_longentry = عدم;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_READDIR_BOTTOM(بنية Curl_easy *data,
                                        بنية ssh_conn *sshc)
{
  CURLcode result;

  ܐܢ(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  ܐܚܪܢܐ
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = عدم;

  ܐܢ(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  ܐܚܪܢܐ
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_READDIR_DONE(بنية Curl_easy *data,
                                      بنية ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = عدم;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن فراغ myssh_quote_error(بنية Curl_easy *data, بنية ssh_conn *sshc,
                              ثابت حرف *cmd)
{
  ܐܢ(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

ساكن صحيح myssh_in_SFTP_QUOTE_STATVFS(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  ܐܢ(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    ܦܢܐ SSH_OK;
  }
  ܐܚܪܢܐ ܐܢ(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#وإلاإذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#ܐܚܪܢܐ
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    حرف *tmp = curl_maprintf("statvfs:\n"
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

    ܐܢ(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    ܐܢ(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    ܐܢ(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ܦܢܐ SSH_OK;
}

ساكن صحيح myssh_auth_interactive(بنية connectdata *conn,
                                  بنية ssh_conn *sshc)
{
  صحيح rc;
  صحيح nprompts;

restart:
  بدل(sshc->kbd_state) {
  حالة 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, عدم, عدم);
    ܐܢ(rc == SSH_AUTH_AGAIN)
      ܦܢܐ SSH_AGAIN;

    ܐܢ(rc != SSH_AUTH_INFO)
      ܦܢܐ SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    ܐܢ(nprompts != 1)
      ܦܢܐ SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    ܐܢ(rc < 0)
      ܦܢܐ SSH_ERROR;

    FALLTHROUGH();
  حالة 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, عدم, عدم);
    ܐܢ(rc == SSH_AUTH_AGAIN)
      ܦܢܐ SSH_AGAIN;
    ܐܚܪܢܐ ܐܢ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ܐܚܪܢܐ ܐܢ(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      ܐܢ(nprompts)
        ܦܢܐ SSH_ERROR;

      sshc->kbd_state = 2;
      اذهب restart;
    }
    ܐܚܪܢܐ
      rc = SSH_ERROR;
    اكسر;
  حالة 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, عدم, عدم);
    ܐܢ(rc == SSH_AUTH_AGAIN)
      ܦܢܐ SSH_AGAIN;
    ܐܚܪܢܐ ܐܢ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ܐܚܪܢܐ
      rc = SSH_ERROR;

    اكسر;
  افتراضي:
    ܦܢܐ SSH_ERROR;
  }

  sshc->kbd_state = 0;
  ܦܢܐ rc;
}

ساكن فراغ myssh_state_init(بنية Curl_easy *data,
                             بنية ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#ܐܢ 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

ساكن فراغ myssh_block2waitfor(بنية connectdata *conn,
                                بنية ssh_conn *sshc,
                                bool block)
{
  (فراغ)conn;
  ܐܢ(block) {
    صحيح dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  ܐܚܪܢܐ
    sshc->waitfor = 0;
}

ساكن صحيح myssh_in_S_STARTUP(بنية Curl_easy *data,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = data->conn;
  صحيح rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  ܐܢ(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  ܐܚܪܢܐ ܐܢ(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  ܐܚܪܢܐ
    myssh_to(data, sshc, SSH_HOSTKEY);

  ܦܢܐ rc;
}

ساكن صحيح myssh_in_AUTHLIST(بنية Curl_easy *data,
                             بنية ssh_conn *sshc)
{
  صحيح rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, عدم);
  ܐܢ(rc == SSH_AUTH_AGAIN)
    ܦܢܐ SSH_AGAIN;

  ܐܢ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ܦܢܐ rc;
  }
  ܐܚܪܢܐ ܐܢ(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    ܦܢܐ rc;
  }

  sshc->auth_methods =
    (unsigned صحيح)ssh_userauth_list(sshc->ssh_session, عدم);
  ܐܢ(sshc->auth_methods)
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
  ܐܢ((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_key && sshc->priv_key[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  ܐܚܪܢܐ ܐܢ(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  ܐܚܪܢܐ ܐܢ(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_KEY_INIT);
  }
  ܐܚܪܢܐ ܐܢ(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  ܐܚܪܢܐ {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  ܦܢܐ rc;
}

ساكن صحيح myssh_in_AUTH_PKEY_INIT(بنية Curl_easy *data,
                                   بنية ssh_conn *sshc)
{
  صحيح rc;
  ܐܢ(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    ܦܢܐ rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  ܐܢ(sshc->priv_key && sshc->priv_key[0]) {
    ܐܢ(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, عدم, sshc->pubkey);
      ܐܢ(rc == SSH_AUTH_AGAIN)
        ܦܢܐ SSH_AGAIN;

      ܐܢ(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        ܦܢܐ rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_key,
                                     sshc->passphrase, عدم,
                                     عدم, &sshc->privkey);
    ܐܢ(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_key);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      ܦܢܐ rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  ܐܚܪܢܐ {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, عدم,
                                     sshc->passphrase);
    ܐܢ(rc == SSH_AUTH_AGAIN)
      ܦܢܐ SSH_AGAIN;

    ܐܢ(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      ܦܢܐ rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  ܦܢܐ rc;
}

ساكن صحيح myssh_in_AUTH_PKEY(بنية Curl_easy *data,
                              بنية ssh_conn *sshc)
{
  صحيح rc = ssh_userauth_publickey(sshc->ssh_session, عدم, sshc->privkey);
  ܐܢ(rc == SSH_AUTH_AGAIN)
    ܦܢܐ SSH_AGAIN;
  ܐܚܪܢܐ ܐܢ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ܦܢܐ SSH_OK;
  }
  ܐܚܪܢܐ {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    ܦܢܐ myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

ساكن صحيح myssh_in_AUTH_GSSAPI(بنية Curl_easy *data,
                                بنية ssh_conn *sshc)
{
  صحيح rc;
  ܐܢ(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    ܦܢܐ myssh_to_KEY_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  ܐܢ(rc == SSH_AUTH_AGAIN)
    ܦܢܐ SSH_AGAIN;

  ܐܢ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ܦܢܐ SSH_OK;
  }

  ܦܢܐ myssh_to_KEY_AUTH(data, sshc);
}

ساكن صحيح myssh_in_AUTH_KEY_INIT(بنية Curl_easy *data,
                                  بنية ssh_conn *sshc)
{
  ܐܢ(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_KEY);
    ܦܢܐ SSH_NO_ERROR;
  }
  ܦܢܐ myssh_to_PASSWD_AUTH(data, sshc);
}

ساكن صحيح myssh_in_AUTH_KEY(بنية Curl_easy *data,
                             بنية ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  صحيح rc = myssh_auth_interactive(data->conn, sshc);
  ܐܢ(rc == SSH_AGAIN)
    ܦܢܐ rc;
  ܐܚܪܢܐ ܐܢ(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ܦܢܐ SSH_NO_ERROR;
  }
  ܐܚܪܢܐ
    ܦܢܐ myssh_to_PASSWD_AUTH(data, sshc);
}

ساكن صحيح myssh_in_AUTH_PASS_INIT(بنية Curl_easy *data,
                                   بنية ssh_conn *sshc)
{
  ܐܢ(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_AUTH_PASS(بنية Curl_easy *data,
                              بنية ssh_conn *sshc)
{
  صحيح rc = ssh_userauth_password(sshc->ssh_session, عدم,
                                 Curl_creds_passwd(data->conn->creds));
  ܐܢ(rc == SSH_AUTH_AGAIN)
    ܦܢܐ SSH_AGAIN;
  ܐܚܪܢܐ ܐܢ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ܦܢܐ SSH_NO_ERROR;
  }
  ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

ساكن صحيح myssh_in_AUTH_DONE(بنية Curl_easy *data,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = data->conn;
  ܐܢ(!sshc->authed) {
    failf(data, "Authentication failure");
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ܐܢ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    ܦܢܐ SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_UPLOAD_INIT(بنية Curl_easy *data,
                                بنية ssh_conn *sshc,
                                بنية SSHPROTO *sshp)
{
  صحيح flags;
  صحيح rc = 0;

  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  ܐܢ(data->state.resume_from) {
    sftp_attributes attrs;

    ܐܢ(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      ܐܢ(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        ܐܢ(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          ܦܢܐ rc;
        }
        data->state.resume_from = size;
      }
      ܐܚܪܢܐ {
        data->state.resume_from = 0;
      }
    }
  }

  ܐܢ(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  ܐܚܪܢܐ ܐܢ(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  ܐܚܪܢܐ
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  ܐܢ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  ܐܢ(!sshc->sftp_file) {
    صحيح err = sftp_get_error(sshc->sftp_session);

    ܐܢ((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ܦܢܐ rc;
    }
    ܐܚܪܢܐ {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      ܦܢܐ rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  ܐܢ(data->state.resume_from > 0 && !data->set.remote_append) {
    صحيح seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    ܐܢ(data->set.seek_func) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    ܐܢ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      ܐܢ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        ܦܢܐ rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ܥܒܕ {
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)حجم(scratch)) ?
          حجم(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        ܐܢ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          ܦܢܐ rc;
        }
      } ܟܡܐ(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    ܐܢ(data->state.infilesize > 0) {
      ܐܢ(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    ܐܢ(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      ܦܢܐ rc;
    }
  }
  ܐܢ(data->state.infilesize > 0) {
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
#ܐܢ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  ܦܢܐ rc;
}

ساكن صحيح myssh_in_SFTP_DOWNLOAD_INIT(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc,
                                       بنية SSHPROTO *sshp)
{
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  ܐܢ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  ܐܢ(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    ܦܢܐ myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_DOWNLOAD_STAT(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc)
{
  curl_off_t size;
  صحيح rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  ܐܢ(!attrs ||
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
    ܐܢ(attrs)
      sftp_attributes_free(attrs);
  }
  ܐܚܪܢܐ {
    size = attrs->size;

    sftp_attributes_free(attrs);

    ܐܢ(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    ܐܢ(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      ܐܢ(result)
        ܦܢܐ myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      ܐܢ(rc)
        ܦܢܐ myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  ܐܢ(data->state.resume_from) {
    ܐܢ(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ܐܢ(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    ܐܚܪܢܐ {
      ܐܢ(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    ܐܢ(rc)
      ܦܢܐ myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  ܐܢ(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    ܦܢܐ rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  ܦܢܐ rc;
}

ساكن صحيح myssh_in_SFTP_CLOSE(بنية Curl_easy *data,
                               بنية ssh_conn *sshc,
                               بنية SSHPROTO *sshp)
{
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  ܐܢ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = عدم;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  ܐܢ(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  ܐܚܪܢܐ {
    myssh_to(data, sshc, SSH_STOP);
  }
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_SHUTDOWN(بنية Curl_easy *data,
                                  بنية ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#ܐܢ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  ܐܢ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = عدم;
  }

  ܐܢ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = عدم;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_INIT(بنية Curl_easy *data,
                              بنية ssh_conn *sshc)
{
  صحيح rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  ܐܢ(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  ܐܢ(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ܦܢܐ myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_REALPATH(بنية Curl_easy *data,
                                  بنية ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  ܐܢ(!sshc->homedir)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  ܐܢ(!data->state.most_recent_ftp_entrypath)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_QUOTE_INIT(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp)
{
  CURLcode result;

  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  ܐܢ(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  ܐܚܪܢܐ ܐܢ(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ܐܚܪܢܐ
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_POSTQUOTE_INIT(بنية Curl_easy *data,
                                        بنية ssh_conn *sshc)
{
  ܐܢ(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ܐܚܪܢܐ {
    myssh_to(data, sshc, SSH_STOP);
  }
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح quote_error(بنية Curl_easy *data,
                       بنية ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_QUOTE(بنية Curl_easy *data,
                               بنية ssh_conn *sshc,
                               بنية SSHPROTO *sshp)
{
  ثابت حرف *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  ثابت حرف *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ܐܢ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ܐܢ(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    حرف *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    ܐܢ(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ܦܢܐ SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    ܐܢ(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    ܐܚܪܢܐ
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    ܦܢܐ SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ܐܢ(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    ܦܢܐ SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ܐܢ(result) {
    ܐܢ(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    ܐܚܪܢܐ
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    ܦܢܐ SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  ܐܢ(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ܐܢ(result) {
      ܐܢ(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      ܐܚܪܢܐ
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ܦܢܐ SSH_NO_ERROR;
    }
    ܐܢ(*cp)
      ܦܢܐ quote_error(data, sshc);
    sshc->quote_attrs = عدم;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    ܦܢܐ SSH_NO_ERROR;
  }
  ܐܢ(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ܐܢ(result) {
      ܐܢ(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      ܐܚܪܢܐ
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ܦܢܐ SSH_NO_ERROR;
    }
    ܐܢ(*cp)
      ܦܢܐ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ܦܢܐ SSH_NO_ERROR;
  }
  ܐܚܪܢܐ ܐܢ(!strncmp(cmd, "mkdir ", 6)) {
    ܐܢ(*cp)
      ܦܢܐ quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    ܦܢܐ SSH_NO_ERROR;
  }
  ܐܚܪܢܐ ܐܢ(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ܐܢ(result) {
      ܐܢ(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      ܐܚܪܢܐ
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ܦܢܐ SSH_NO_ERROR;
    }
    ܐܢ(*cp)
      ܦܢܐ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    ܦܢܐ SSH_NO_ERROR;
  }
  ܐܚܪܢܐ ܐܢ(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    ܐܢ(*cp)
      ܦܢܐ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    ܦܢܐ SSH_NO_ERROR;
  }
  ܐܚܪܢܐ ܐܢ(!strncmp(cmd, "rm ", 3)) {
    ܐܢ(*cp)
      ܦܢܐ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    ܦܢܐ SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  ܐܚܪܢܐ ܐܢ(!strncmp(cmd, "statvfs ", 8)) {
    ܐܢ(*cp)
      ܦܢܐ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    ܦܢܐ SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_NEXT_QUOTE(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  ܐܢ(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ܐܚܪܢܐ {
    ܐܢ(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    ܐܚܪܢܐ {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_QUOTE_STAT(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc)
{
  ثابت حرف *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ܐܢ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  ܐܢ(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  ܐܢ(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, عدم);
    ܦܢܐ SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  ܐܢ(!strncmp(cmd, "chgrp", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t gid;
    ܐܢ(curlx_str_number(&p, &gid, UINT_MAX)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, عدم);
      ܦܢܐ SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ܐܚܪܢܐ ܐܢ(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت حرف *p = sshc->quote_path1;
    ܐܢ(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, عدم);
      ܦܢܐ SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  ܐܚܪܢܐ ܐܢ(!strncmp(cmd, "chown", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t uid;
    ܐܢ(curlx_str_number(&p, &uid, UINT_MAX)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, عدم);
      ܦܢܐ SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ܐܚܪܢܐ ܐܢ(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    ܐܢ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ܐܢ SIZEOF_TIME_T > 4
    ܐܚܪܢܐ ܐܢ(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    ܐܢ(fail) {
      myssh_quote_error(data, sshc, عدم);
      ܦܢܐ SSH_NO_ERROR;
    }
    ܐܢ(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    ܐܚܪܢܐ /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن فراغ conn_forget_socket(بنية Curl_easy *data, int8_t sockindex)
{
  بنية connectdata *conn = data->conn;
  ܐܢ(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    بنية Curl_cfilter *cf = conn->cfilter[sockindex];
    ܐܢ(cf)
      (فراغ)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, عدم);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

ساكن فراغ myssh_SESSION_DISCONNECT(بنية Curl_easy *data,
                                     بنية ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ܐܢ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = عدم;
  }

  ܐܢ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = عدم;
  }
  ܐܢ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = عدم;
  }

  ssh_disconnect(sshc->ssh_session);
  ܐܢ(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

ساكن صحيح myssh_SSH_SCP_DOWNLOAD(بنية Curl_easy *data,
                                  بنية ssh_conn *sshc)
{
  uint64_t bytecount;
  صحيح rc = ssh_scp_pull_request(sshc->scp_session);
  ܐܢ(rc != SSH_SCP_REQUEST_NEWFILE) {
    ثابت حرف *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  ܐܢ(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  ܦܢܐ 0;
}

ساكن صحيح myssh_in_TRANS_INIT(بنية Curl_easy *data, بنية ssh_conn *sshc,
                               بنية SSHPROTO *sshp)
{
  CURLcode result;
  صحيح rc = 0;
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  ܐܢ(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    ܦܢܐ 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  ܐܢ(data->state.upload) {
    ܐܢ(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ܐܚܪܢܐ {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  ܐܢ(!sshc->scp_session) {
    ثابت حرف *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  ܦܢܐ rc;
}

ساكن فراغ sshc_cleanup(بنية ssh_conn *sshc)
{
  ܐܢ(sshc->initialized) {
    ܐܢ(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = عدم;
    }
    ܐܢ(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = عدم;
    }
    ܐܢ(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = عدم;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    ܐܢ(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = عدم;
    }
    ܐܢ(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = عدم;
    }
    ܐܢ(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = عدم;
    }
    ܐܢ(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = عدم;
    }
    ܐܢ(sshc->privkey) {
      ssh_key_free(sshc->privkey);
      sshc->privkey = عدم;
    }
    ܐܢ(sshc->pubkey) {
      ssh_key_free(sshc->pubkey);
      sshc->pubkey = عدم;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_key);
    curlx_safefree(sshc->priv_key);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

ساكن صحيح myssh_in_SFTP_QUOTE_SETSTAT(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc)
{
  صحيح rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  ܐܢ(rc == SSH_AGAIN)
    ܦܢܐ rc;
  ܐܢ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    ܦܢܐ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_QUOTE_SYMLINK(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc)
{
  صحيح rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  ܐܢ(rc == SSH_AGAIN)
    ܦܢܐ rc;
  ܐܢ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    ܦܢܐ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_QUOTE_MKDIR(بنية Curl_easy *data,
                                     بنية ssh_conn *sshc)
{
  صحيح rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  ܐܢ(rc == SSH_AGAIN)
    ܦܢܐ rc;
  ܐܢ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    ܦܢܐ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_QUOTE_RENAME(بنية Curl_easy *data,
                                      بنية ssh_conn *sshc)
{
  صحيح rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  ܐܢ(rc == SSH_AGAIN)
    ܦܢܐ rc;
  ܐܢ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    ܦܢܐ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_QUOTE_RMDIR(بنية Curl_easy *data,
                                     بنية ssh_conn *sshc)
{
  صحيح rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  ܐܢ(rc == SSH_AGAIN)
    ܦܢܐ rc;
  ܐܢ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    ܦܢܐ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_QUOTE_UNLINK(بنية Curl_easy *data,
                                      بنية ssh_conn *sshc)
{
  صحيح rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  ܐܢ(rc == SSH_AGAIN)
    ܦܢܐ rc;
  ܐܢ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    ܦܢܐ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_GETINFO(بنية Curl_easy *data,
                                 بنية ssh_conn *sshc)
{
  ܐܢ(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  ܐܚܪܢܐ
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_FILETIME(بنية Curl_easy *data,
                                  بنية ssh_conn *sshc,
                                  بنية SSHPROTO *sshp)
{
  sftp_attributes attrs;
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  ܐܢ(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_TRANS_INIT(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp)
{
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  ܐܢ(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ܐܚܪܢܐ {
    size_t path_len = strlen(sshp->path);

    ܐܢ(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    ܐܚܪܢܐ
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_CREATE_DIRS_INIT(بنية Curl_easy *data,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  ܐܢ(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ܐܚܪܢܐ {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_CREATE_DIRS(بنية Curl_easy *data,
                                     بنية ssh_conn *sshc,
                                     بنية SSHPROTO *sshp)
{
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ܐܢ(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ܦܢܐ SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SFTP_CREATE_DIRS_MKDIR(بنية Curl_easy *data,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp)
{
  صحيح rc;
  صحيح err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  ܐܢ(rc == SSH_AGAIN)
    ܦܢܐ rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ܐܢ(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    ܐܢ((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      ܦܢܐ myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  ܦܢܐ rc;
}

ساكن صحيح myssh_in_SCP_UPLOAD_INIT(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp)
{
  صحيح rc;
  ܐܢ(!sshp)
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  ܐܢ(rc != SSH_OK) {
    ثابت حرف *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_push_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (صحيح)data->set.new_file_perms);

  ܐܢ(rc != SSH_OK) {
    ثابت حرف *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    ܦܢܐ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SCP_DOWNLOAD_INIT(بنية Curl_easy *data,
                                      بنية ssh_conn *sshc)
{
  صحيح rc = ssh_scp_init(sshc->scp_session);
  ܐܢ(rc != SSH_OK) {
    ثابت حرف *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    ܦܢܐ myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SCP_DONE(بنية Curl_easy *data,
                             بنية ssh_conn *sshc)
{
  ܐܢ(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  ܐܚܪܢܐ
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SCP_SEND_EOF(بنية Curl_easy *data,
                                 بنية ssh_conn *sshc)
{
  ܐܢ(sshc->scp_session) {
    صحيح rc = ssh_scp_close(sshc->scp_session);
    ܐܢ(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      ܦܢܐ SSH_AGAIN;
    }
    ܐܢ(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن صحيح myssh_in_SCP_CHANNEL_FREE(بنية Curl_easy *data,
                                     بنية ssh_conn *sshc)
{
  ܐܢ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = عدم;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ܦܢܐ SSH_NO_ERROR;
}

ساكن CURLcode myssh_in_SESSION_FREE(بنية Curl_easy *data,
                                      بنية ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  memset(sshc, 0, حجم(بنية ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  ܦܢܐ result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
ساكن CURLcode myssh_statemachine(بنية Curl_easy *data,
                                   بنية ssh_conn *sshc,
                                   بنية SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  صحيح rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  ܥܒܕ {

    بدل(sshc->state) {
    حالة SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    حالة SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      ܐܢ(rc)
        اكسر;
      FALLTHROUGH();
    حالة SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      ܐܢ(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        اكسر;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    حالة SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      اكسر;
    حالة SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      اكسر;
    حالة SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      اكسر;
    حالة SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      اكسر;
    حالة SSH_AUTH_KEY_INIT:
      rc = myssh_in_AUTH_KEY_INIT(data, sshc);
      اكسر;
    حالة SSH_AUTH_KEY:
      rc = myssh_in_AUTH_KEY(data, sshc);
      اكسر;
    حالة SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      اكسر;
    حالة SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      اكسر;
    حالة SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      اكسر;
    حالة SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      اكسر;
    حالة SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      اكسر;
    حالة SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      اكسر;
    حالة SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      اكسر;
    حالة SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      اكسر;
    حالة SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      اكسر;
    حالة SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      اكسر;
    حالة SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      اكسر;
    حالة SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      اكسر;
    حالة SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      اكسر;
    حالة SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      ܐܢ(rc)
        اكسر;
      FALLTHROUGH();
    حالة SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      اكسر;
    حالة SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      اكسر;
    حالة SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      اكسر;
    حالة SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    حالة SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    حالة SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      اكسر;
    حالة SSH_QUIT:
    افتراضي:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      اكسر;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } ܟܡܐ((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  ܐܢ(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  ܐܢ(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحيح)result, *block);
  ܦܢܐ result;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode myssh_multi_statemach(بنية Curl_easy *data,
                                      bool *done)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  ܐܢ(!sshc || !sshp)
    ܦܢܐ CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  ܦܢܐ result;
}

ساكن CURLcode myssh_block_statemach(بنية Curl_easy *data,
                                      بنية ssh_conn *sshc,
                                      بنية SSHPROTO *sshp,
                                      bool disconnect)
{
  بنية connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ܟܡܐ((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t left_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
    ܐܢ(result)
      اكسر;

    ܐܢ(!disconnect) {
      result = Curl_pgrsCheck(data);
      ܐܢ(result)
        اكسر;

      left_ms = Curl_timeleft_ms(data);
      ܐܢ(left_ms < 0) {
        failf(data, "Operation timed out");
        ܦܢܐ CURLE_OPERATION_TIMEDOUT;
      }
    }

    ܐܢ(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (فراغ)SOCKET_READABLE(fd_read,
                            (!left_ms || left_ms > 1000) ? 1000 : left_ms);
    }
  }

  ܦܢܐ result;
}

ساكن فراغ myssh_easy_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية SSHPROTO *sshp = entry;
  (فراغ)key;
  (فراغ)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

ساكن فراغ myssh_conn_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية ssh_conn *sshc = entry;
  (فراغ)key;
  (فراغ)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
ساكن CURLcode myssh_setup_connection(بنية Curl_easy *data,
                                       بنية connectdata *conn)
{
  بنية SSHPROTO *sshp;
  بنية ssh_conn *sshc;

  sshc = curlx_calloc(1, حجم(*sshc));
  ܐܢ(!sshc)
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  ܐܢ(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, حجم(*sshp));
  ܐܢ(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  ܦܢܐ Curl_ssh_setup_pkey(data, sshc);
}

ساكن Curl_recv scp_recv, sftp_recv;
ساكن Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ساكن CURLcode myssh_connect(بنية Curl_easy *data, bool *done)
{
  CURLcode result;
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح rc;

  ܐܢ(!sshc || !ssh)
    ܦܢܐ CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  ܐܢ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ܐܚܪܢܐ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  ܐܢ(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    ܦܢܐ CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  ܐܢ(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    ܦܢܐ CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, عدم);
  ܐܢ(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  ܐܢ(rc != SSH_OK) {
    failf(data, "Could not set socket");
    ܦܢܐ CURLE_FAILED_INIT;
  }

  ܐܢ(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    ܐܢ(rc != SSH_OK) {
      failf(data, "Could not set user");
      ܦܢܐ CURLE_FAILED_INIT;
    }
  }

  ܐܢ(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    ܐܢ(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    ܐܢ(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      ܦܢܐ CURLE_FAILED_INIT;
    }
  }

  ܐܢ(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    ܐܢ(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      ܦܢܐ CURLE_FAILED_INIT;
    }
  }

  ܐܢ(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    ܐܢ(rc != SSH_OK) {
      failf(data, "Could not set compression");
      ܦܢܐ CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = عدم;
  sshc->pubkey = عدم;

  ܐܢ(sshc->pub_key) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_key, &sshc->pubkey);
    ܐܢ(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      ܦܢܐ CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  ܦܢܐ result;
}

/* called from multi.c while DOing */
ساكن CURLcode scp_doing(بنية Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  ܐܢ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ܦܢܐ result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ساكن CURLcode scp_perform(بنية Curl_easy *data,
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  ܐܢ(!sshc)
    ܦܢܐ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ܐܢ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ܦܢܐ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode scp_disconnect(بنية Curl_easy *data,
                               بنية connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (فراغ)dead_connection;

  ܐܢ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  ܦܢܐ result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ساكن CURLcode myssh_done(بنية Curl_easy *data,
                           بنية ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  بنية SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  ܐܢ(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
  ܐܚܪܢܐ
    result = status;

  ܐܢ(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ܦܢܐ CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  ܦܢܐ result;
}

ساكن CURLcode scp_done(بنية Curl_easy *data, CURLcode status,
                         bool premature)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (فراغ)premature;

  ܐܢ(!sshc)
    ܦܢܐ CURLE_FAILED_INIT;
  ܐܢ(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  ܦܢܐ myssh_done(data, sshc, status);
}

ساكن CURLcode scp_send(بنية Curl_easy *data, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  صحيح rc;
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  (فراغ)eos;
  *pnwritten = 0;

  ܐܢ(!sshc)
    ܦܢܐ CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#ܐܢ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  ܐܢ(rc == SSH_AGAIN)
    ܦܢܐ CURLE_AGAIN;
  ܐܚܪܢܐ
#endif
  ܐܢ(rc != SSH_OK)
    ܦܢܐ CURLE_SSH;

  *pnwritten = len;
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode scp_recv(بنية Curl_easy *data, int8_t sockindex,
                         حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  صحيح nread;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ܐܢ(!sshc)
    ܦܢܐ CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  ܐܢ(nread == SSH_ERROR)
    ܦܢܐ CURLE_SSH;
#ܐܢ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  ܐܢ(nread == SSH_AGAIN)
    ܦܢܐ CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  ܦܢܐ CURLE_OK;
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

ساكن CURLcode sftp_perform(بنية Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  ܐܢ(!sshc)
    ܦܢܐ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ܐܢ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ܦܢܐ result;
}

/* called from multi.c while DOing */
ساكن CURLcode sftp_doing(بنية Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  ܐܢ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ܦܢܐ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode sftp_disconnect(بنية Curl_easy *data,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (فراغ)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  ܐܢ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
  ܦܢܐ result;
}

ساكن CURLcode sftp_done(بنية Curl_easy *data, CURLcode status,
                          bool premature)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  ܐܢ(!sshc)
    ܦܢܐ CURLE_FAILED_INIT;
  ܐܢ(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    ܐܢ(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  ܦܢܐ myssh_done(data, sshc, status);
}

/* return number of sent bytes */
ساكن CURLcode sftp_send(بنية Curl_easy *data, int8_t sockindex,
                          ثابت uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (فراغ)sockindex;
  (فراغ)eos;
  *pnwritten = 0;

  ܐܢ(!sshc)
    ܦܢܐ CURLE_FAILED_INIT;

#ܐܢ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  بدل(sshc->sftp_send_state) {
  حالة 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    ܐܢ(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      ܦܢܐ CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  حالة 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    ܐܢ(nwrite == SSH_AGAIN)
      ܦܢܐ CURLE_AGAIN;
    ܐܚܪܢܐ ܐܢ(nwrite < 0)
      ܦܢܐ CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    ܦܢܐ CURLE_OK;
  افتراضي:
    /* we never reach here */
    ܦܢܐ CURLE_SEND_ERROR;
  }
#ܐܚܪܢܐ
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  ܐܢ(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#ܐܢ 0 /* not returned by libssh on write */
  ܐܢ(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  ܐܚܪܢܐ
#endif
  ܐܢ(nwrite < 0)
    ܦܢܐ CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  ܦܢܐ CURLE_OK;
#endif
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ساكن CURLcode sftp_recv(بنية Curl_easy *data, int8_t sockindex,
                          حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (فراغ)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  ܐܢ(!sshc)
    ܦܢܐ CURLE_FAILED_INIT;

  بدل(sshc->sftp_recv_state) {
  حالة 0:
#ܐܢ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    ܐܢ(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      ܦܢܐ CURLE_RECV_ERROR;
    }
#ܐܚܪܢܐ
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    ܐܢ(sshc->sftp_file_index < 0)
      ܦܢܐ CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  حالة 1:
    sshc->sftp_recv_state = 1;

#ܐܢ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#ܐܚܪܢܐ
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    ܐܢ(nread == SSH_AGAIN)
      ܦܢܐ CURLE_AGAIN;
    ܐܚܪܢܐ ܐܢ(nread < 0)
      ܦܢܐ CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    ܦܢܐ CURLE_OK;

  افتراضي:
    /* we never reach here */
    ܦܢܐ CURLE_RECV_ERROR;
  }
}

ساكن CURLcode myssh_do_it(بنية Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  ܐܢ(!sshc)
    ܦܢܐ CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  ܐܢ(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  ܐܚܪܢܐ
    result = sftp_perform(data, &connected, done);

  ܦܢܐ result;
}

CURLcode Curl_ssh_init(فراغ)
{
  ܐܢ(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    ܦܢܐ CURLE_FAILED_INIT;
  }
  ܦܢܐ CURLE_OK;
}

فراغ Curl_ssh_cleanup(فراغ)
{
  (فراغ)ssh_finalize();
}

فراغ Curl_ssh_version(حرف *buffer, size_t buflen)
{
  (فراغ)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
ثابت بنية Curl_protocol Curl_protocol_scp = {
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
ثابت بنية Curl_protocol Curl_protocol_sftp = {
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
