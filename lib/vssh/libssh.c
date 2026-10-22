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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_LIBSSH

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "vssh/ssh.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "parsedate.h"          /* for the week day and month names */
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "vssh/vssh.h"
#অন্তর্ভাব "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#অন্তর্ভাব <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#অন্তর্ভাব <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  করো {                          \
    যদি(x) {                     \
      ssh_string_free_char(x);  \
      x = ফাঁকা;                 \
    }                           \
  } যখন(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

স্থির CURLcode sftp_error_to_CURLE(পূর্ণসংখ্যা err)
{
  নির্বাচন(err) {
  ক্ষেত্র SSH_FX_OK:
    ফেরত CURLE_OK;

  ক্ষেত্র SSH_FX_NO_SUCH_FILE:
  ক্ষেত্র SSH_FX_NO_SUCH_PATH:
    ফেরত CURLE_REMOTE_FILE_NOT_FOUND;

  ক্ষেত্র SSH_FX_PERMISSION_DENIED:
  ক্ষেত্র SSH_FX_WRITE_PROTECT:
    ফেরত CURLE_REMOTE_ACCESS_DENIED;

  ক্ষেত্র SSH_FX_FILE_ALREADY_EXISTS:
    ফেরত CURLE_REMOTE_FILE_EXISTS;

  পূর্বনির্ধারিত:
    বিরতি;
  }

  ফেরত CURLE_SSH;
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
স্থির পূর্ণসংখ্যা myssh_is_known(কাঠামো Curl_easy *তথ্য, কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc;
  ssh_চাবি pubkey;
  unsigned অক্ষর *hash_sha256 = ফাঁকা;
  size_t hlen_sha256;
  unsigned অক্ষর *hash_md5 = ফাঁকা;
  size_t hlen_md5;
  অক্ষর *found_base64 = ফাঁকা;
  অক্ষর *known_base64 = ফাঁকা;
  পূর্ণসংখ্যা vstate;
  গণনা curl_khmatch keymatch;
  কাঠামো curl_khkey foundkey;
  কাঠামো curl_khkey *knownkeyp = ফাঁকা;
  curl_sshkeycallback func = তথ্য->set.ssh_keyfunc;
  কাঠামো ssh_knownhosts_entry *knownhostsentry = ফাঁকা;
  কাঠামো curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  যদি(rc != SSH_OK)
    ফেরত rc;

  যদি(CURL_EASY_STR(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_SHA256)) {
    ধ্রুবক অক্ষর *pubkey_sha256 =
      CURL_EASY_STR(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_SHA256);
    অক্ষর *fingerprint_b64 = ফাঁকা;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    যদি(rc != SSH_OK || hlen_sha256 != 32) {
      failf(তথ্য, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      যাও cleanup;
    }

    যদি(curlx_base64_encode((ধ্রুবক uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      যাও cleanup;
    }

    infof(তথ্য, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    যখন((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    যখন((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    যদি((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(তথ্য,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      যাও cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    যাও cleanup;
  }

  যদি(CURL_EASY_STR(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_MD5)) {
    ধ্রুবক অক্ষর *pubkey_md5 =
      CURL_EASY_STR(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_MD5);
    অক্ষর md5buffer[33];
    পূর্ণসংখ্যা i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    যদি(rc != SSH_OK || hlen_md5 != 16) {
      failf(তথ্য,
            "Denied establishing ssh session: MD5 fingerprint not available");
      যাও cleanup;
    }

    জন্য(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(তথ্য, "SSH MD5 fingerprint: %s", md5buffer);

    যদি(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(তথ্য,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      যাও cleanup;
    }

    rc = SSH_OK;
    যাও cleanup;
  }

  যদি(CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    যদি(knownhostsentry) {
      যদি(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        যদি(rc != SSH_OK) {
          যাও cleanup;
        }
        knownkey.চাবি = known_base64;
        knownkey.len = strlen(known_base64);

        নির্বাচন(ssh_চাবি_type(knownhostsentry->publickey)) {
        ক্ষেত্র SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          বিরতি;
        ক্ষেত্র SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          বিরতি;
        ক্ষেত্র SSH_KEYTYPE_ECDSA:
        ক্ষেত্র SSH_KEYTYPE_ECDSA_P256:
        ক্ষেত্র SSH_KEYTYPE_ECDSA_P384:
        ক্ষেত্র SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          বিরতি;
        ক্ষেত্র SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          বিরতি;
        ক্ষেত্র SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          বিরতি;
        পূর্বনির্ধারিত:
          rc = SSH_ERROR;
          যাও cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    নির্বাচন(vstate) {
    ক্ষেত্র SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      বিরতি;
    ক্ষেত্র SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      বিরতি;
    ক্ষেত্র SSH_KNOWN_HOSTS_NOT_FOUND:
    ক্ষেত্র SSH_KNOWN_HOSTS_UNKNOWN:
    ক্ষেত্র SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      বিরতি;
    পূর্বনির্ধারিত:
      keymatch = CURLKHMATCH_MISMATCH;
      বিরতি;
    }

    যদি(func) { /* use callback to determine action */
      কাঠামো Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      যদি(rc != SSH_OK)
        যাও cleanup;

      foundkey.চাবি = found_base64;
      foundkey.len = strlen(found_base64);

      নির্বাচন(ssh_চাবি_type(pubkey)) {
      ক্ষেত্র SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        বিরতি;
      ক্ষেত্র SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        বিরতি;
      ক্ষেত্র SSH_KEYTYPE_ECDSA:
      ক্ষেত্র SSH_KEYTYPE_ECDSA_P256:
      ক্ষেত্র SSH_KEYTYPE_ECDSA_P384:
      ক্ষেত্র SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        বিরতি;
      ক্ষেত্র SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        বিরতি;
      ক্ষেত্র SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        বিরতি;
      পূর্বনির্ধারিত:
        rc = SSH_ERROR;
        যাও cleanup;
      }

      CURL_CBAPI_START(&guard, তথ্য, easy_ssh_keyfunc);
      rc = func(তথ্য, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, তথ্য->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      নির্বাচন(rc) {
      ক্ষেত্র CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        যদি(rc != SSH_OK) {
          যাও cleanup;
        }
        বিরতি;
      ক্ষেত্র CURLKHSTAT_FINE:
        বিরতি;
      পূর্বনির্ধারিত: /* REJECT/DEFER */
        rc = SSH_ERROR;
        যাও cleanup;
      }
    }
    অন্যথা {
      যদি(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        যাও cleanup;
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
  ফেরত rc;
}

স্থির পূর্ণসংখ্যা myssh_to_ERROR(কাঠামো Curl_easy *তথ্য,
                          কাঠামো ssh_conn *sshc,
                          CURLcode ফলাফল)
{
  myssh_to(তথ্য, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = ফলাফল;
  ফেরত SSH_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_to_SFTP_CLOSE(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ssh_conn *sshc)
{
  myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  ফেরত SSH_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_to_PASSWD_AUTH(কাঠামো Curl_easy *তথ্য,
                                কাঠামো ssh_conn *sshc)
{
  যদি(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(তথ্য, sshc, SSH_AUTH_PASS_INIT);
    ফেরত SSH_OK;
  }
  ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_LOGIN_DENIED);
}

স্থির পূর্ণসংখ্যা myssh_to_চাবি_AUTH(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ssh_conn *sshc)
{
  যদি(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(তথ্য, sshc, SSH_AUTH_চাবি_INIT);
    ফেরত SSH_OK;
  }
  ফেরত myssh_to_PASSWD_AUTH(তথ্য, sshc);
}

স্থির পূর্ণসংখ্যা myssh_to_GSSAPI_AUTH(কাঠামো Curl_easy *তথ্য,
                                কাঠামো ssh_conn *sshc)
{
  যদি(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(তথ্য, sshc, SSH_AUTH_GSSAPI);
    ফেরত SSH_OK;
  }
  ফেরত myssh_to_চাবি_AUTH(তথ্য, sshc);
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_READDIR_INIT(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ssh_conn *sshc,
                                      কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);
  Curl_pgrs_Set_Download_আকার(তথ্য, -1);
  যদি(তথ্য->req.no_body) {
    myssh_to(তথ্য, sshc, SSH_STOP);
    ফেরত SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  যদি(!sshc->sftp_dir) {
    failf(তথ্য, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ফেরত myssh_to_SFTP_CLOSE(তথ্য, sshc);
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_READDIR);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_READDIR(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ssh_conn *sshc,
                                 কাঠামো SSHPROTO *sshp)
{
  CURLcode ফলাফল = CURLE_OK;

  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  যদি(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  যদি(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    যদি(তথ্য->set.list_only) {
      অক্ষর *tmp_সারি;

      tmp_সারি = curl_maprintf("%s\n", sshc->readdir_filename);
      যদি(!tmp_সারি) {
        myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        ফেরত SSH_ERROR;
      }
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                                 tmp_সারি, sshc->readdir_len + 1);
      curlx_free(tmp_সারি);

      যদি(ফলাফল) {
        myssh_to(তথ্য, sshc, SSH_STOP);
        sshc->actualcode = ফলাফল;
        ফেরত SSH_NO_ERROR;
      }
    }
    অন্যথা {
      যদি(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(তথ্য, sshc, SSH_STOP);
        ফেরত SSH_ERROR;
      }

      যদি((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        যদি(!sshc->readdir_linkPath) {
          myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          ফেরত SSH_ERROR;
        }

        myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_LINK);
        ফেরত SSH_NO_ERROR;
      }
      myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_BOTTOM);
      ফেরত SSH_NO_ERROR;
    }
  }
  অন্যথা যদি(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_DONE);
  }
  অন্যথা {
    failf(তথ্য, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ফেরত myssh_to_SFTP_CLOSE(তথ্য, sshc);
  }
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_READDIR_LINK(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ssh_conn *sshc)
{
  যদি(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  যদি(!sshc->readdir_link_attrs) {
    failf(তথ্য, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ফেরত myssh_to_SFTP_CLOSE(তথ্য, sshc);
  }

  যদি(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    যদি(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    অন্যথা
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = ফাঁকা;
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
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    ফেরত SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = ফাঁকা;
  sshc->readdir_filename = ফাঁকা;
  sshc->readdir_longentry = ফাঁকা;

  myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_BOTTOM);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_READDIR_BOTTOM(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ssh_conn *sshc)
{
  CURLcode ফলাফল;

  যদি(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    ফলাফল = CURLE_OUT_OF_MEMORY;
  অন্যথা
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = ফাঁকা;

  যদি(ফলাফল) {
    myssh_to(তথ্য, sshc, SSH_STOP);
    sshc->actualcode = ফলাফল;
  }
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_SFTP_READDIR);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_READDIR_DONE(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = ফাঁকা;

  /* no data to transfer */
  Curl_xfer_setup_nop(তথ্য);
  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত SSH_NO_ERROR;
}

স্থির শূন্য myssh_quote_error(কাঠামো Curl_easy *তথ্য, কাঠামো ssh_conn *sshc,
                              ধ্রুবক অক্ষর *cmd)
{
  যদি(cmd)
    failf(তথ্য, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE_STATVFS(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  যদি(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(তথ্য, sshc, "statvfs");
    ফেরত SSH_OK;
  }
  অন্যথা যদি(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_আকার_MASK "I64u"
#নতুবা_যদি defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_আকার_MASK "llu"
#অন্যথা
#define CURL_LIBSSH_VFS_আকার_MASK PRIu64
#endif
    CURLcode ফলাফল = CURLE_OK;
    অক্ষর *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_files: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_আকার_MASK "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_আকার_MASK "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_blocks, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    যদি(!tmp)
      ফলাফল = CURLE_OUT_OF_MEMORY;

    যদি(!ফলাফল) {
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    যদি(ফলাফল) {
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ফলাফল;
    }
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত SSH_OK;
}

স্থির পূর্ণসংখ্যা myssh_auth_interactive(কাঠামো connectdata *conn,
                                  কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc;
  পূর্ণসংখ্যা nprompts;

restart:
  নির্বাচন(sshc->kbd_state) {
  ক্ষেত্র 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, ফাঁকা, ফাঁকা);
    যদি(rc == SSH_AUTH_AGAIN)
      ফেরত SSH_AGAIN;

    যদি(rc != SSH_AUTH_INFO)
      ফেরত SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    যদি(nprompts != 1)
      ফেরত SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    যদি(rc < 0)
      ফেরত SSH_ERROR;

    FALLTHROUGH();
  ক্ষেত্র 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ফাঁকা, ফাঁকা);
    যদি(rc == SSH_AUTH_AGAIN)
      ফেরত SSH_AGAIN;
    অন্যথা যদি(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    অন্যথা যদি(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      যদি(nprompts)
        ফেরত SSH_ERROR;

      sshc->kbd_state = 2;
      যাও restart;
    }
    অন্যথা
      rc = SSH_ERROR;
    বিরতি;
  ক্ষেত্র 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ফাঁকা, ফাঁকা);
    যদি(rc == SSH_AUTH_AGAIN)
      ফেরত SSH_AGAIN;
    অন্যথা যদি(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    অন্যথা
      rc = SSH_ERROR;

    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত SSH_ERROR;
  }

  sshc->kbd_state = 0;
  ফেরত rc;
}

স্থির শূন্য myssh_state_init(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ssh_conn *sshc)
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

  myssh_to(তথ্য, sshc, SSH_S_STARTUP);
}

স্থির শূন্য myssh_block2waitfor(কাঠামো connectdata *conn,
                                কাঠামো ssh_conn *sshc,
                                bool block)
{
  (শূন্য)conn;
  যদি(block) {
    পূর্ণসংখ্যা dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  অন্যথা
    sshc->waitfor = 0;
}

স্থির পূর্ণসংখ্যা myssh_in_S_STARTUP(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ssh_conn *sshc)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  পূর্ণসংখ্যা rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  যদি(rc == SSH_AGAIN) {
    CURL_TRC_SSH(তথ্য, "connect -> EAGAIN");
  }
  অন্যথা যদি(rc != SSH_OK) {
    failf(তথ্য, "Failure establishing ssh session");
    rc = myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);
  }
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_HOSTKEY);

  ফেরত rc;
}

স্থির পূর্ণসংখ্যা myssh_in_AUTHLIST(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, ফাঁকা);
  যদি(rc == SSH_AUTH_AGAIN)
    ফেরত SSH_AGAIN;

  যদি(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(তথ্য, "Authenticated with none");
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
    ফেরত rc;
  }
  অন্যথা যদি(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(তথ্য, sshc, CURLE_LOGIN_DENIED);
    ফেরত rc;
  }

  sshc->auth_methods =
    (unsigned পূর্ণসংখ্যা)ssh_userauth_list(sshc->ssh_session, ফাঁকা);
  যদি(sshc->auth_methods)
    infof(তথ্য, "SSH authentication methods available: %s%s%s%s",
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
      (তথ্য->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(তথ্য, sshc, SSH_AUTH_PKEY_INIT);
    infof(তথ্য, "Authentication using SSH public key file");
  }
  অন্যথা যদি(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(তথ্য, sshc, SSH_AUTH_GSSAPI);
  }
  অন্যথা যদি(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(তথ্য, sshc, SSH_AUTH_চাবি_INIT);
  }
  অন্যথা যদি(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(তথ্য, sshc, SSH_AUTH_PASS_INIT);
  }
  অন্যথা {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(তথ্য, sshc, CURLE_LOGIN_DENIED);
  }
  ফেরত rc;
}

স্থির পূর্ণসংখ্যা myssh_in_AUTH_PKEY_INIT(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc;
  যদি(!(তথ্য->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(তথ্য, sshc);
    ফেরত rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  যদি(sshc->priv_চাবি && sshc->priv_চাবি[0]) {
    যদি(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, ফাঁকা, sshc->pubkey);
      যদি(rc == SSH_AUTH_AGAIN)
        ফেরত SSH_AGAIN;

      যদি(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(তথ্য, sshc);
        ফেরত rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_চাবি,
                                     sshc->passphrase, ফাঁকা,
                                     ফাঁকা, &sshc->privkey);
    যদি(rc != SSH_OK) {
      failf(তথ্য, "Could not load private key file %s", sshc->priv_চাবি);
      rc = myssh_to_ERROR(তথ্য, sshc, CURLE_LOGIN_DENIED);
      ফেরত rc;
    }

    myssh_to(তথ্য, sshc, SSH_AUTH_PKEY);
  }
  অন্যথা {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, ফাঁকা,
                                     sshc->passphrase);
    যদি(rc == SSH_AUTH_AGAIN)
      ফেরত SSH_AGAIN;

    যদি(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(তথ্য, "Completed public key authentication");
      myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
      ফেরত rc;
    }

    rc = myssh_to_GSSAPI_AUTH(তথ্য, sshc);
  }
  ফেরত rc;
}

স্থির পূর্ণসংখ্যা myssh_in_AUTH_PKEY(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = ssh_userauth_publickey(sshc->ssh_session, ফাঁকা, sshc->privkey);
  যদি(rc == SSH_AUTH_AGAIN)
    ফেরত SSH_AGAIN;
  অন্যথা যদি(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(তথ্য, "Completed public key authentication");
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
    ফেরত SSH_OK;
  }
  অন্যথা {
    infof(তথ্য, "Failed public key authentication (rc: %d)", rc);
    ফেরত myssh_to_GSSAPI_AUTH(তথ্য, sshc);
  }
}

স্থির পূর্ণসংখ্যা myssh_in_AUTH_GSSAPI(কাঠামো Curl_easy *তথ্য,
                                কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc;
  যদি(!(তথ্য->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    ফেরত myssh_to_চাবি_AUTH(তথ্য, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  যদি(rc == SSH_AUTH_AGAIN)
    ফেরত SSH_AGAIN;

  যদি(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(তথ্য, "Completed gssapi authentication");
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
    ফেরত SSH_OK;
  }

  ফেরত myssh_to_চাবি_AUTH(তথ্য, sshc);
}

স্থির পূর্ণসংখ্যা myssh_in_AUTH_চাবি_INIT(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ssh_conn *sshc)
{
  যদি(তথ্য->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(তথ্য, sshc, SSH_AUTH_চাবি);
    ফেরত SSH_NO_ERROR;
  }
  ফেরত myssh_to_PASSWD_AUTH(তথ্য, sshc);
}

স্থির পূর্ণসংখ্যা myssh_in_AUTH_চাবি(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  পূর্ণসংখ্যা rc = myssh_auth_interactive(তথ্য->conn, sshc);
  যদি(rc == SSH_AGAIN)
    ফেরত rc;
  অন্যথা যদি(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(তথ্য, "completed keyboard interactive authentication");
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
    ফেরত SSH_NO_ERROR;
  }
  অন্যথা
    ফেরত myssh_to_PASSWD_AUTH(তথ্য, sshc);
}

স্থির পূর্ণসংখ্যা myssh_in_AUTH_PASS_INIT(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ssh_conn *sshc)
{
  যদি(!(তথ্য->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_LOGIN_DENIED);
  myssh_to(তথ্য, sshc, SSH_AUTH_PASS);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_AUTH_PASS(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = ssh_userauth_password(sshc->ssh_session, ফাঁকা,
                                 Curl_creds_passwd(তথ্য->conn->creds));
  যদি(rc == SSH_AUTH_AGAIN)
    ফেরত SSH_AGAIN;
  অন্যথা যদি(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(তথ্য, "Completed password authentication");
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
    ফেরত SSH_NO_ERROR;
  }
  ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_LOGIN_DENIED);
}

স্থির পূর্ণসংখ্যা myssh_in_AUTH_DONE(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ssh_conn *sshc)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  যদি(!sshc->authed) {
    failf(তথ্য, "Authentication failure");
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(তথ্য, "Authentication complete");
  Curl_pgrsTime(তথ্য, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  যদি(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(তথ্য, sshc, SSH_SFTP_INIT);
    ফেরত SSH_NO_ERROR;
  }
  infof(তথ্য, "SSH CONNECT phase done");
  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_UPLOAD_INIT(কাঠামো Curl_easy *তথ্য,
                                কাঠামো ssh_conn *sshc,
                                কাঠামো SSHPROTO *sshp)
{
  পূর্ণসংখ্যা flags;
  পূর্ণসংখ্যা rc = 0;

  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);

  যদি(তথ্য->state.resume_from) {
    sftp_attributes attrs;

    যদি(তথ্য->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      যদি(attrs) {
        curl_off_t আকার_2 = attrs->আকার_2;
        sftp_attributes_free(attrs);
        যদি(আকার_2 < 0) {
          failf(তথ্য, "Bad file size (%" FMT_OFF_T ")", আকার_2);
          rc = myssh_to_ERROR(তথ্য, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          ফেরত rc;
        }
        তথ্য->state.resume_from = আকার_2;
      }
      অন্যথা {
        তথ্য->state.resume_from = 0;
      }
    }
  }

  যদি(তথ্য->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  অন্যথা যদি(তথ্য->state.resume_from > 0) {
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
              flags, (mode_t)তথ্য->set.new_file_perms);
  যদি(!sshc->sftp_file) {
    পূর্ণসংখ্যা err = sftp_get_error(sshc->sftp_session);

    যদি((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       তথ্য->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(তথ্য, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ফেরত rc;
    }
    অন্যথা {
      rc = myssh_to_SFTP_CLOSE(তথ্য, sshc);
      ফেরত rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  যদি(তথ্য->state.resume_from > 0 && !তথ্য->set.remote_append) {
    পূর্ণসংখ্যা seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    যদি(তথ্য->set.seek_func) {
      কাঠামো Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, তথ্য, easy_seek_func);
      seekerr = তথ্য->set.seek_func(তথ্য->set.seek_client,
                                    তথ্য->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    যদি(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      যদি(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(তথ্য, "Could not seek stream");
        rc = myssh_to_ERROR(তথ্য, sshc, CURLE_FTP_COULDNT_USE_REST);
        ফেরত rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      করো {
        অক্ষর scratch[4 * 1024];
        size_t readthisamountnow =
          (তথ্য->state.resume_from - passed >
           (curl_off_t)আকার(scratch)) ?
          আকার(scratch) : curlx_sotouz(তথ্য->state.resume_from - passed);

        size_t actuallyread =
          তথ্য->state.fread_func(scratch, 1,
                                 readthisamountnow, তথ্য->state.in);

        passed += actuallyread;
        যদি((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(তথ্য, "Failed to read data");
          rc = myssh_to_ERROR(তথ্য, sshc, CURLE_FTP_COULDNT_USE_REST);
          ফেরত rc;
        }
      } যখন(passed < তথ্য->state.resume_from);
    }

    /* now, decrease the size of the read */
    যদি(তথ্য->state.infilesize > 0) {
      যদি(তথ্য->state.resume_from > তথ্য->state.infilesize) {
        failf(তথ্য, "Resume point beyond size");
        ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      তথ্য->state.infilesize -= তথ্য->state.resume_from;
      তথ্য->req.আকার_2 = তথ্য->state.infilesize;
      Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, তথ্য->state.resume_from);
    যদি(rc) {
      rc = myssh_to_SFTP_CLOSE(তথ্য, sshc);
      ফেরত rc;
    }
  }
  যদি(তথ্য->state.infilesize > 0) {
    তথ্য->req.আকার_2 = তথ্য->state.infilesize;
    Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);
  }
  /* upload data */
  Curl_xfer_setup_send(তথ্য, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  তথ্য->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(তথ্য);
#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত rc;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_DOWNLOAD_INIT(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc,
                                       কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  যদি(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)তথ্য->set.new_file_perms);
  যদি(!sshc->sftp_file) {
    failf(তথ্য, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    ফেরত myssh_to_SFTP_CLOSE(তথ্য, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(তথ্য, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_DOWNLOAD_STAT(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc)
{
  curl_off_t আকার_2;
  পূর্ণসংখ্যা rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  যদি(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_আকার) ||
     (attrs->আকার_2 == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    তথ্য->req.আকার_2 = -1;
    তথ্য->req.maxdownload = -1;
    Curl_pgrs_Set_Download_আকার(তথ্য, -1);
    আকার_2 = 0;
    যদি(attrs)
      sftp_attributes_free(attrs);
  }
  অন্যথা {
    আকার_2 = attrs->আকার_2;

    sftp_attributes_free(attrs);

    যদি(আকার_2 < 0) {
      failf(তথ্য, "Bad file size (%" FMT_OFF_T ")", আকার_2);
      ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    যদি(তথ্য->state.use_range) {
      curl_off_t from;
      CURLcode ফলাফল = Curl_ssh_range(তথ্য, তথ্য->state.range, আকার_2,
                                       &from, &আকার_2);
      যদি(ফলাফল)
        ফেরত myssh_to_ERROR(তথ্য, sshc, ফলাফল);

      rc = sftp_seek64(sshc->sftp_file, from);
      যদি(rc)
        ফেরত myssh_to_SFTP_CLOSE(তথ্য, sshc);
    }
    তথ্য->req.আকার_2 = আকার_2;
    তথ্য->req.maxdownload = আকার_2;
    Curl_pgrs_Set_Download_আকার(তথ্য, আকার_2);
  }

  /* We can resume if we can seek to the resume position */
  যদি(তথ্য->state.resume_from) {
    যদি(তথ্য->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      যদি(আকার_2 < -তথ্য->state.resume_from) {
        failf(তথ্য, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", তথ্য->state.resume_from, আকার_2);
        ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      তথ্য->state.resume_from += আকার_2;
    }
    অন্যথা {
      যদি(আকার_2 < তথ্য->state.resume_from) {
        failf(তথ্য, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              তথ্য->state.resume_from, আকার_2);
        ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    তথ্য->req.আকার_2 = আকার_2 - তথ্য->state.resume_from;
    তথ্য->req.maxdownload = আকার_2 - তথ্য->state.resume_from;
    Curl_pgrs_Set_Download_আকার(তথ্য, আকার_2 - তথ্য->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, তথ্য->state.resume_from);
    যদি(rc)
      ফেরত myssh_to_SFTP_CLOSE(তথ্য, sshc);
  }

  /* Setup the actual download */
  যদি(তথ্য->req.আকার_2 == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(তথ্য);
    infof(তথ্য, "File already completely downloaded");
    myssh_to(তথ্য, sshc, SSH_STOP);
    ফেরত rc;
  }
  Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, তথ্য->req.আকার_2);

  /* not set by Curl_xfer_setup to preserve io_flags */
  তথ্য->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(তথ্য, sshc, SSH_STOP);

  ফেরত rc;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_CLOSE(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ssh_conn *sshc,
                               কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);
  যদি(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ফাঁকা;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(তথ্য, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  যদি(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(তথ্য, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_STOP);
  }
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_SHUTDOWN(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ssh_conn *sshc)
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
    sshc->sftp_file = ফাঁকা;
  }

  যদি(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ফাঁকা;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(তথ্য, sshc, SSH_SESSION_DISCONNECT);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_INIT(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  যদি(!sshc->sftp_session) {
    failf(তথ্য, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  যদি(rc != SSH_OK) {
    failf(তথ্য, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ফেরত myssh_to_ERROR(তথ্য, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_REALPATH);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_REALPATH(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  যদি(!sshc->homedir)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(তথ্য->state.most_recent_ftp_entrypath);
  তথ্য->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  যদি(!তথ্য->state.most_recent_ftp_entrypath)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(তথ্য, "CONNECT phase done");
  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE_INIT(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc,
                                    কাঠামো SSHPROTO *sshp)
{
  CURLcode ফলাফল;

  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);

  ফলাফল = Curl_getworkingpath(তথ্য, sshc->homedir, &sshp->path);
  যদি(ফলাফল) {
    sshc->actualcode = ফলাফল;
    myssh_to(তথ্য, sshc, SSH_STOP);
  }
  অন্যথা যদি(তথ্য->set.quote) {
    infof(তথ্য, "Sending quote commands");
    sshc->quote_item = তথ্য->set.quote;
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE);
  }
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_SFTP_GETINFO);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_POSTQUOTE_INIT(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ssh_conn *sshc)
{
  যদি(তথ্য->set.postquote) {
    infof(তথ্য, "Sending quote commands");
    sshc->quote_item = তথ্য->set.postquote;
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_STOP);
  }
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা quote_error(কাঠামো Curl_easy *তথ্য,
                       কাঠামো ssh_conn *sshc)
{
  failf(তথ্য, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ssh_conn *sshc,
                               কাঠামো SSHPROTO *sshp)
{
  ধ্রুবক অক্ষর *cp;
  CURLcode ফলাফল;

  /*
   * Support some of the "FTP" commands
   */
  ধ্রুবক অক্ষর *cmd = sshc->quote_item->তথ্য;
  sshc->acceptfail = FALSE;

  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);

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
    অক্ষর *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    যদি(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ফেরত SSH_NO_ERROR;
    }
    Curl_debug(তথ্য, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(তথ্য, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    যদি(ফলাফল) {
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ফলাফল;
    }
    অন্যথা
      myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
    ফেরত SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  যদি(!cp) {
    failf(তথ্য, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    ফেরত SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  ফলাফল = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
      failf(তথ্য, "Out of memory");
    অন্যথা
      failf(তথ্য, "Syntax error: Bad first parameter");
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = ফলাফল;
    ফেরত SSH_NO_ERROR;
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
    ফলাফল = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
        failf(তথ্য, "Out of memory");
      অন্যথা
        failf(তথ্য, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ফলাফল;
      ফেরত SSH_NO_ERROR;
    }
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    sshc->quote_attrs = ফাঁকা;
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_STAT);
    ফেরত SSH_NO_ERROR;
  }
  যদি(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    ফলাফল = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
        failf(তথ্য, "Out of memory");
      অন্যথা
        failf(তথ্য, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ফলাফল;
      ফেরত SSH_NO_ERROR;
    }
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ফেরত SSH_NO_ERROR;
  }
  অন্যথা যদি(!strncmp(cmd, "mkdir ", 6)) {
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    /* create directory */
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_MKDIR);
    ফেরত SSH_NO_ERROR;
  }
  অন্যথা যদি(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    ফলাফল = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    যদি(ফলাফল) {
      যদি(ফলাফল == CURLE_OUT_OF_MEMORY)
        failf(তথ্য, "Out of memory");
      অন্যথা
        failf(তথ্য, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ফলাফল;
      ফেরত SSH_NO_ERROR;
    }
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_RENAME);
    ফেরত SSH_NO_ERROR;
  }
  অন্যথা যদি(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_RMDIR);
    ফেরত SSH_NO_ERROR;
  }
  অন্যথা যদি(!strncmp(cmd, "rm ", 3)) {
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_UNLINK);
    ফেরত SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  অন্যথা যদি(!strncmp(cmd, "statvfs ", 8)) {
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_STATVFS);
    ফেরত SSH_NO_ERROR;
  }
#endif

  failf(তথ্য, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_পরবর্তী_QUOTE(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->পরবর্তী;

  যদি(sshc->quote_item) {
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE);
  }
  অন্যথা {
    যদি(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(তথ্য, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    অন্যথা {
      myssh_to(তথ্য, sshc, SSH_SFTP_GETINFO);
    }
  }
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE_STAT(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc)
{
  ধ্রুবক অক্ষর *cmd = sshc->quote_item->তথ্য;
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
    failf(তথ্য, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(তথ্য, sshc, ফাঁকা);
    ফেরত SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  যদি(!strncmp(cmd, "chgrp", 5)) {
    ধ্রুবক অক্ষর *p = sshc->quote_path1;
    curl_off_t gid;
    যদি(curlx_str_সংখ্যা(&p, &gid, UINT_MAX)) {
      failf(তথ্য, "Syntax error: chgrp gid not a number");
      myssh_quote_error(তথ্য, sshc, ফাঁকা);
      ফেরত SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  অন্যথা যদি(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ধ্রুবক অক্ষর *p = sshc->quote_path1;
    যদি(curlx_str_octal(&p, &perms, 07777)) {
      failf(তথ্য, "Syntax error: chmod permissions not a number");
      myssh_quote_error(তথ্য, sshc, ফাঁকা);
      ফেরত SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  অন্যথা যদি(!strncmp(cmd, "chown", 5)) {
    ধ্রুবক অক্ষর *p = sshc->quote_path1;
    curl_off_t uid;
    যদি(curlx_str_সংখ্যা(&p, &uid, UINT_MAX)) {
      failf(তথ্য, "Syntax error: chown uid not a number");
      myssh_quote_error(তথ্য, sshc, ফাঁকা);
      ফেরত SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  অন্যথা যদি(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    যদি(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(তথ্য, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#যদি SIZEOF_TIME_T > 4
    অন্যথা যদি(date > 0xffffffff) {
      failf(তথ্য, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    যদি(fail) {
      myssh_quote_error(তথ্য, sshc, ফাঁকা);
      ফেরত SSH_NO_ERROR;
    }
    যদি(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    অন্যথা /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ফেরত SSH_NO_ERROR;
}

স্থির শূন্য conn_forget_socket(কাঠামো Curl_easy *তথ্য, int8_t sockindex)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  যদি(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    কাঠামো Curl_cfilter *cf = conn->cfilter[sockindex];
    যদি(cf)
      (শূন্য)Curl_conn_cf_cntrl(cf, তথ্য, TRUE, CF_CTRL_FORGET_SOCKET, 0, ফাঁকা);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

স্থির শূন্য myssh_SESSION_DISCONNECT(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  যদি(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ফাঁকা;
  }

  যদি(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ফাঁকা;
  }
  যদি(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ফাঁকা;
  }

  ssh_disconnect(sshc->ssh_session);
  যদি(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(তথ্য, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
}

স্থির পূর্ণসংখ্যা myssh_SSH_SCP_DOWNLOAD(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ssh_conn *sshc)
{
  uint64_t bytecount;
  পূর্ণসংখ্যা rc = ssh_scp_pull_request(sshc->scp_session);
  যদি(rc != SSH_SCP_REQUEST_NEWFILE) {
    ধ্রুবক অক্ষর *err_বার্তা = ssh_get_error(sshc->ssh_session);
    failf(তথ্য, "%s", err_বার্তা);
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  যদি(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_TOO_LARGE);
  তথ্য->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, তথ্য->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  তথ্য->conn->send_idx = 0;

  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা myssh_in_TRANS_INIT(কাঠামো Curl_easy *তথ্য, কাঠামো ssh_conn *sshc,
                               কাঠামো SSHPROTO *sshp)
{
  CURLcode ফলাফল;
  পূর্ণসংখ্যা rc = 0;
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);

  ফলাফল = Curl_getworkingpath(তথ্য, sshc->homedir, &sshp->path);
  যদি(ফলাফল) {
    sshc->actualcode = ফলাফল;
    myssh_to(তথ্য, sshc, SSH_STOP);
    ফেরত 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  যদি(তথ্য->state.upload) {
    যদি(তথ্য->state.infilesize < 0) {
      failf(তথ্য, "SCP requires a known file size for upload");
      ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(তথ্য, sshc, SSH_SCP_UPLOAD_INIT);
  }
  অন্যথা {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(তথ্য, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  যদি(!sshc->scp_session) {
    ধ্রুবক অক্ষর *err_বার্তা = ssh_get_error(sshc->ssh_session);
    failf(তথ্য, "%s", err_বার্তা);
    rc = myssh_to_ERROR(তথ্য, sshc, CURLE_UPLOAD_FAILED);
  }
  ফেরত rc;
}

স্থির শূন্য sshc_cleanup(কাঠামো ssh_conn *sshc)
{
  যদি(sshc->initialized) {
    যদি(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = ফাঁকা;
    }
    যদি(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = ফাঁকা;
    }
    যদি(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = ফাঁকা;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    যদি(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = ফাঁকা;
    }
    যদি(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = ফাঁকা;
    }
    যদি(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = ফাঁকা;
    }
    যদি(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = ফাঁকা;
    }
    যদি(sshc->privkey) {
      ssh_চাবি_free(sshc->privkey);
      sshc->privkey = ফাঁকা;
    }
    যদি(sshc->pubkey) {
      ssh_চাবি_free(sshc->pubkey);
      sshc->pubkey = ফাঁকা;
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

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE_SETSTAT(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  যদি(rc == SSH_AGAIN)
    ফেরত rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(তথ্য, sshc, "setstat");
    ফেরত rc;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE_SYMLINK(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  যদি(rc == SSH_AGAIN)
    ফেরত rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(তথ্য, sshc, "symlink");
    ফেরত rc;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE_MKDIR(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)তথ্য->set.new_directory_perms);
  যদি(rc == SSH_AGAIN)
    ফেরত rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(তথ্য, sshc, "mkdir");
    ফেরত rc;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE_RENAME(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  যদি(rc == SSH_AGAIN)
    ফেরত rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(তথ্য, sshc, "rename");
    ফেরত rc;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE_RMDIR(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  যদি(rc == SSH_AGAIN)
    ফেরত rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(তথ্য, sshc, "rmdir");
    ফেরত rc;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_QUOTE_UNLINK(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  যদি(rc == SSH_AGAIN)
    ফেরত rc;
  যদি(rc && !sshc->acceptfail) {
    myssh_quote_error(তথ্য, sshc, "rm");
    ফেরত rc;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_GETINFO(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ssh_conn *sshc)
{
  যদি(তথ্য->set.get_filetime)
    myssh_to(তথ্য, sshc, SSH_SFTP_FILETIME);
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_SFTP_TRANS_INIT);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_FILETIME(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ssh_conn *sshc,
                                  কাঠামো SSHPROTO *sshp)
{
  sftp_attributes attrs;
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  যদি(attrs) {
    তথ্য->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(তথ্য, sshc, SSH_SFTP_TRANS_INIT);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_TRANS_INIT(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc,
                                    কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);
  যদি(তথ্য->state.upload)
    myssh_to(তথ্য, sshc, SSH_SFTP_UPLOAD_INIT);
  অন্যথা {
    size_t path_len = strlen(sshp->path);

    যদি(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_INIT);
    অন্যথা
      myssh_to(তথ্য, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_CREATE_DIRS_INIT(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো ssh_conn *sshc,
                                          কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);

  যদি(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(তথ্য, sshc, SSH_SFTP_CREATE_DIRS);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_CREATE_DIRS(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো ssh_conn *sshc,
                                     কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  যদি(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(তথ্য, "Creating directory '%s'", sshp->path);
    myssh_to(তথ্য, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ফেরত SSH_NO_ERROR;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_UPLOAD_INIT);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SFTP_CREATE_DIRS_MKDIR(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো ssh_conn *sshc,
                                           কাঠামো SSHPROTO *sshp)
{
  পূর্ণসংখ্যা rc;
  পূর্ণসংখ্যা err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)তথ্য->set.new_directory_perms);
  যদি(rc == SSH_AGAIN)
    ফেরত rc;
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
      ফেরত myssh_to_SFTP_CLOSE(তথ্য, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_CREATE_DIRS);
  ফেরত rc;
}

স্থির পূর্ণসংখ্যা myssh_in_SCP_UPLOAD_INIT(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc,
                                    কাঠামো SSHPROTO *sshp)
{
  পূর্ণসংখ্যা rc;
  যদি(!sshp)
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  যদি(rc != SSH_OK) {
    ধ্রুবক অক্ষর *err_বার্তা = ssh_get_error(sshc->ssh_session);
    failf(তথ্য, "%s", err_বার্তা);
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ঠেলো_file64(sshc->scp_session, sshp->path,
                           (uint64_t)তথ্য->state.infilesize,
                           (পূর্ণসংখ্যা)তথ্য->set.new_file_perms);

  যদি(rc != SSH_OK) {
    ধ্রুবক অক্ষর *err_বার্তা = ssh_get_error(sshc->ssh_session);
    failf(তথ্য, "%s", err_বার্তা);
    ফেরত myssh_to_ERROR(তথ্য, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(তথ্য, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  তথ্য->conn->recv_idx = FIRSTSOCKET;

  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SCP_DOWNLOAD_INIT(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = ssh_scp_init(sshc->scp_session);
  যদি(rc != SSH_OK) {
    ধ্রুবক অক্ষর *err_বার্তা = ssh_get_error(sshc->ssh_session);
    failf(তথ্য, "%s", err_বার্তা);
    ফেরত myssh_to_ERROR(তথ্য, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(তথ্য, sshc, SSH_SCP_DOWNLOAD);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SCP_DONE(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ssh_conn *sshc)
{
  যদি(তথ্য->state.upload)
    myssh_to(তথ্য, sshc, SSH_SCP_SEND_EOF);
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_SCP_CHANNEL_FREE);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SCP_SEND_EOF(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ssh_conn *sshc)
{
  যদি(sshc->scp_session) {
    পূর্ণসংখ্যা rc = ssh_scp_close(sshc->scp_session);
    যদি(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      ফেরত SSH_AGAIN;
    }
    যদি(rc != SSH_OK) {
      infof(তথ্য,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(তথ্য, sshc, SSH_SCP_CHANNEL_FREE);
  ফেরত SSH_NO_ERROR;
}

স্থির পূর্ণসংখ্যা myssh_in_SCP_CHANNEL_FREE(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো ssh_conn *sshc)
{
  যদি(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ফাঁকা;
  }
  CURL_TRC_SSH(তথ্য, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(তথ্য, sshc, SSH_SESSION_DISCONNECT);
  ফেরত SSH_NO_ERROR;
}

স্থির CURLcode myssh_in_SESSION_FREE(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ssh_conn *sshc)
{
  CURLcode ফলাফল;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  ফলাফল = sshc->actualcode;
  memset(sshc, 0, আকার(কাঠামো ssh_conn));
  connclose(তথ্য->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত ফলাফল;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
স্থির CURLcode myssh_statemachine(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ssh_conn *sshc,
                                   কাঠামো SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  করো {

    নির্বাচন(sshc->state) {
    ক্ষেত্র SSH_INIT:
      myssh_state_init(তথ্য, sshc);
      FALLTHROUGH();
    ক্ষেত্র SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(তথ্য, sshc);
      যদি(rc)
        বিরতি;
      FALLTHROUGH();
    ক্ষেত্র SSH_HOSTKEY:
      rc = myssh_is_known(তথ্য, sshc);
      যদি(rc != SSH_OK) {
        rc = myssh_to_ERROR(তথ্য, sshc, CURLE_PEER_FAILED_VERIFICATION);
        বিরতি;
      }
      myssh_to(তথ্য, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    ক্ষেত্র SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_AUTH_চাবি_INIT:
      rc = myssh_in_AUTH_চাবি_INIT(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_AUTH_চাবি:
      rc = myssh_in_AUTH_চাবি(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_পরবর্তী_QUOTE:
      rc = myssh_in_SFTP_পরবর্তী_QUOTE(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(তথ্য, sshc, sshp);
      বিরতি;
    ক্ষেত্র SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(তথ্য, sshc);
      যদি(rc)
        বিরতি;
      FALLTHROUGH();
    ক্ষেত্র SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(তথ্য, sshc);
      FALLTHROUGH();
    ক্ষেত্র SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(তথ্য, sshc);
      FALLTHROUGH();
    ক্ষেত্র SSH_SESSION_FREE:
      ফলাফল = myssh_in_SESSION_FREE(তথ্য, sshc);
      বিরতি;
    ক্ষেত্র SSH_QUIT:
    পূর্বনির্ধারিত:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(তথ্য, sshc, SSH_STOP);
      বিরতি;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } যখন((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  যদি(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  যদি(!ফলাফল && (sshc->state == SSH_STOP))
    ফলাফল = sshc->actualcode;
  CURL_TRC_SSH(তথ্য, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (পূর্ণসংখ্যা)ফলাফল, *block);
  ফেরত ফলাফল;
}

/* called repeatedly until done from multi.c */
স্থির CURLcode myssh_multi_statemach(কাঠামো Curl_easy *তথ্য,
                                      bool *done)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  কাঠামো SSHPROTO *sshp = Curl_meta_get(তথ্য, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode ফলাফল;

  যদি(!sshc || !sshp)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = myssh_statemachine(তথ্য, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  ফেরত ফলাফল;
}

স্থির CURLcode myssh_block_statemach(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ssh_conn *sshc,
                                      কাঠামো SSHPROTO *sshp,
                                      bool disconnect)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  CURLcode ফলাফল = CURLE_OK;

  যখন((sshc->state != SSH_STOP) && !ফলাফল) {
    bool block;
    timediff_t বাম_ms = 1000;

    ফলাফল = myssh_statemachine(তথ্য, sshc, sshp, &block);
    যদি(ফলাফল)
      বিরতি;

    যদি(!disconnect) {
      ফলাফল = Curl_pgrsCheck(তথ্য);
      যদি(ফলাফল)
        বিরতি;

      বাম_ms = Curl_timeleft_ms(তথ্য);
      যদি(বাম_ms < 0) {
        failf(তথ্য, "Operation timed out");
        ফেরত CURLE_OPERATION_TIMEDOUT;
      }
    }

    যদি(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (শূন্য)SOCKET_READABLE(fd_read,
                            (!বাম_ms || বাম_ms > 1000) ? 1000 : বাম_ms);
    }
  }

  ফেরত ফলাফল;
}

স্থির শূন্য myssh_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো SSHPROTO *sshp = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

স্থির শূন্য myssh_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো ssh_conn *sshc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
স্থির CURLcode myssh_setup_connection(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো connectdata *conn)
{
  কাঠামো SSHPROTO *sshp;
  কাঠামো ssh_conn *sshc;

  sshc = curlx_calloc(1, আকার(*sshc));
  যদি(!sshc)
    ফেরত CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  যদি(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, আকার(*sshp));
  যদি(!sshp ||
     Curl_meta_set(তথ্য, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  ফেরত Curl_ssh_setup_pkey(তথ্য, sshc);
}

স্থির Curl_recv scp_recv, sftp_recv;
স্থির Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
স্থির CURLcode myssh_connect(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  কাঠামো SSHPROTO *ssh = Curl_meta_get(তথ্য, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  পূর্ণসংখ্যা rc;

  যদি(!sshc || !ssh)
    ফেরত CURLE_FAILED_INIT;

  CURL_TRC_SSH(তথ্য, "myssh_connect");
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
    failf(তথ্য, "Failure initializing ssh session");
    ফেরত CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  যদি(rc != SSH_OK) {
    failf(তথ্য, "Could not set remote host");
    ফেরত CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, ফাঁকা);
  যদি(rc != SSH_OK) {
    infof(তথ্য, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(তথ্য, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  যদি(rc != SSH_OK) {
    failf(তথ্য, "Could not set socket");
    ফেরত CURLE_FAILED_INIT;
  }

  যদি(Curl_creds_has_user(conn->creds)) {
    infof(তথ্য, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    যদি(rc != SSH_OK) {
      failf(তথ্য, "Could not set user");
      ফেরত CURLE_FAILED_INIT;
    }
  }

  যদি(CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS)) {
    infof(তথ্য, "Known hosts: %s",
          CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS));
    যদি(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS));
    যদি(rc != SSH_OK) {
      failf(তথ্য, "Could not set known hosts file path");
      ফেরত CURLE_FAILED_INIT;
    }
  }

  যদি(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    যদি(rc != SSH_OK) {
      failf(তথ্য, "Could not set remote port");
      ফেরত CURLE_FAILED_INIT;
    }
  }

  যদি(তথ্য->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    যদি(rc != SSH_OK) {
      failf(তথ্য, "Could not set compression");
      ফেরত CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = ফাঁকা;
  sshc->pubkey = ফাঁকা;

  যদি(sshc->pub_চাবি) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_চাবি, &sshc->pubkey);
    যদি(rc != SSH_OK) {
      failf(তথ্য, "Could not load public key file");
      ফেরত CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(তথ্য, sshc, SSH_INIT);

  ফলাফল = myssh_multi_statemach(তথ্য, done);

  ফেরত ফলাফল;
}

/* called from multi.c while DOing */
স্থির CURLcode scp_doing(কাঠামো Curl_easy *তথ্য, bool *dophase_done)
{
  CURLcode ফলাফল;

  ফলাফল = myssh_multi_statemach(তথ্য, dophase_done);

  যদি(*dophase_done) {
    CURL_TRC_SSH(তথ্য, "DO phase is complete");
  }
  ফেরত ফলাফল;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

স্থির CURLcode scp_perform(কাঠামো Curl_easy *তথ্য,
                            bool *connected, bool *dophase_done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(তথ্য->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(তথ্য, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(তথ্য, sshc, SSH_SCP_TRANS_INIT);

  ফলাফল = myssh_multi_statemach(তথ্য, dophase_done);

  *connected = Curl_conn_is_connected(তথ্য->conn, FIRSTSOCKET);

  যদি(*dophase_done) {
    CURL_TRC_SSH(তথ্য, "DO phase is complete");
  }

  ফেরত ফলাফল;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
স্থির CURLcode scp_disconnect(কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *conn,
                               bool dead_connection)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  কাঠামো SSHPROTO *sshp = Curl_meta_get(তথ্য, CURL_META_SSH_EASY);
  (শূন্য)dead_connection;

  যদি(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(তথ্য, sshc, SSH_SESSION_DISCONNECT);
    ফলাফল = myssh_block_statemach(তথ্য, sshc, sshp, TRUE);
  }

  ফেরত ফলাফল;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
স্থির CURLcode myssh_done(কাঠামো Curl_easy *তথ্য,
                           কাঠামো ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো SSHPROTO *sshp = Curl_meta_get(তথ্য, CURL_META_SSH_EASY);

  যদি(!status && sshp) {
    /* run the state-machine */
    ফলাফল = myssh_block_statemach(তথ্য, sshc, sshp, FALSE);
  }
  অন্যথা
    ফলাফল = status;

  যদি(Curl_pgrsDone(তথ্য, Curl_pgrs_now(তথ্য)))
    ফেরত CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(তথ্য);
  ফেরত ফলাফল;
}

স্থির CURLcode scp_done(কাঠামো Curl_easy *তথ্য, CURLcode status,
                         bool premature)
{
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(তথ্য->conn, CURL_META_SSH_CONN);
  (শূন্য)premature;

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;
  যদি(!status)
    myssh_to(তথ্য, sshc, SSH_SCP_DONE);

  ফেরত myssh_done(তথ্য, sshc, status);
}

স্থির CURLcode scp_send(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                         ধ্রুবক uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  পূর্ণসংখ্যা rc;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (শূন্য)sockindex; /* we only support SCP on the fixed known primary socket */
  (শূন্য)eos;
  *pnwritten = 0;

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#যদি 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  যদি(rc == SSH_AGAIN)
    ফেরত CURLE_AGAIN;
  অন্যথা
#endif
  যদি(rc != SSH_OK)
    ফেরত CURLE_SSH;

  *pnwritten = len;
  ফেরত CURLE_OK;
}

স্থির CURLcode scp_recv(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                         অক্ষর *mem, size_t len, size_t *pnread)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  পূর্ণসংখ্যা nread;

  (শূন্য)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  যদি(nread == SSH_ERROR)
    ফেরত CURLE_SSH;
#যদি 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  যদি(nread == SSH_AGAIN)
    ফেরত CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  ফেরত CURLE_OK;
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

স্থির CURLcode sftp_perform(কাঠামো Curl_easy *তথ্য,
                             bool *connected,
                             bool *dophase_done)
{
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(তথ্য->conn, CURL_META_SSH_CONN);
  CURLcode ফলাফল = CURLE_OK;

  CURL_TRC_SSH(তথ্য, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  ফলাফল = myssh_multi_statemach(তথ্য, dophase_done);

  *connected = Curl_conn_is_connected(তথ্য->conn, FIRSTSOCKET);

  যদি(*dophase_done) {
    CURL_TRC_SSH(তথ্য, "DO phase is complete");
  }

  ফেরত ফলাফল;
}

/* called from multi.c while DOing */
স্থির CURLcode sftp_doing(কাঠামো Curl_easy *তথ্য,
                           bool *dophase_done)
{
  CURLcode ফলাফল = myssh_multi_statemach(তথ্য, dophase_done);
  যদি(*dophase_done) {
    CURL_TRC_SSH(তথ্য, "DO phase is complete");
  }
  ফেরত ফলাফল;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
স্থির CURLcode sftp_disconnect(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn,
                                bool dead_connection)
{
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  কাঠামো SSHPROTO *sshp = Curl_meta_get(তথ্য, CURL_META_SSH_EASY);
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)dead_connection;

  CURL_TRC_SSH(তথ্য, "DISCONNECT starts now");

  যদি(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(তথ্য, sshc, SSH_SFTP_SHUTDOWN);
    ফলাফল = myssh_block_statemach(তথ্য, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(তথ্য, "DISCONNECT is done");
  ফেরত ফলাফল;
}

স্থির CURLcode sftp_done(কাঠামো Curl_easy *তথ্য, CURLcode status,
                          bool premature)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;
  যদি(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    যদি(!premature && তথ্য->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
  }
  ফেরত myssh_done(তথ্য, sshc, status);
}

/* return number of sent bytes */
স্থির CURLcode sftp_send(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                          ধ্রুবক uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (শূন্য)sockindex;
  (শূন্য)eos;
  *pnwritten = 0;

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  নির্বাচন(sshc->sftp_send_state) {
  ক্ষেত্র 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    যদি(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      ফেরত CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  ক্ষেত্র 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    যদি(nwrite == SSH_AGAIN)
      ফেরত CURLE_AGAIN;
    অন্যথা যদি(nwrite < 0)
      ফেরত CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    ফেরত CURLE_OK;
  পূর্বনির্ধারিত:
    /* we never reach here */
    ফেরত CURLE_SEND_ERROR;
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
    ফেরত CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  ফেরত CURLE_OK;
#endif
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
স্থির CURLcode sftp_recv(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                          অক্ষর *mem, size_t len, size_t *pnread)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (শূন্য)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_আকার);
  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  নির্বাচন(sshc->sftp_recv_state) {
  ক্ষেত্র 0:
#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    যদি(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      ফেরত CURLE_RECV_ERROR;
    }
#অন্যথা
    sshc->sftp_file_সূচক =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    যদি(sshc->sftp_file_সূচক < 0)
      ফেরত CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  ক্ষেত্র 1:
    sshc->sftp_recv_state = 1;

#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#অন্যথা
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_সূচক);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    যদি(nread == SSH_AGAIN)
      ফেরত CURLE_AGAIN;
    অন্যথা যদি(nread < 0)
      ফেরত CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    ফেরত CURLE_OK;

  পূর্বনির্ধারিত:
    /* we never reach here */
    ফেরত CURLE_RECV_ERROR;
  }
}

স্থির CURLcode myssh_do_it(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল;
  bool connected = FALSE;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  তথ্য->req.আকার_2 = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(তথ্য);

  যদি(conn->scheme->protocol & CURLPROTO_SCP)
    ফলাফল = scp_perform(তথ্য, &connected, done);
  অন্যথা
    ফলাফল = sftp_perform(তথ্য, &connected, done);

  ফেরত ফলাফল;
}

CURLcode Curl_ssh_init(শূন্য)
{
  যদি(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    ফেরত CURLE_FAILED_INIT;
  }
  ফেরত CURLE_OK;
}

শূন্য Curl_ssh_cleanup(শূন্য)
{
  (শূন্য)ssh_finalize();
}

শূন্য Curl_ssh_version(অক্ষর *বাফার, size_t buflen)
{
  (শূন্য)curl_msnprintf(বাফার, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_scp = {
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
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_sftp = {
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
