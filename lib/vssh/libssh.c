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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_LIBSSH

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "vssh/ssh.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "parsedate.h"          /* for the week day and month names */
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "vssh/vssh.h"
#అన్తర్భావ "curlx/base64.h" /* for curlx_base64_encode() */

#నిర్వచితమైతే HAVE_UNISTD_H
#అన్తర్భావ <unistd.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_FCNTL_H
#అన్తర్భావ <fcntl.h>
#అయితే_ముగింపు

/* A recent macro provided by libssh. Or make our own. */
#నిర్వచితం_కాకపోతే SSH_STRING_FREE_CHAR
#నిర్వచించు SSH_STRING_FREE_CHAR(x) \
  చేయి {                          \
    ఒకవేళ(x) {                     \
      ssh_string_free_char(x);  \
      x = ఖాళీ;                 \
    }                           \
  } అయితే(0)
#అయితే_ముగింపు

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#నిర్వచితం_కాకపోతే SSH_S_IFMT
#నిర్వచించు SSH_S_IFMT   00170000
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SSH_S_IFLNK
#నిర్వచించు SSH_S_IFLNK  0120000
#అయితే_ముగింపు

స్థిర CURLcode sftp_error_గమ్యం_CURLE(పూర్ణాంకం err)
{
  ఎంపిక(err) {
  స్థితి SSH_FX_OK:
    తిరిగి CURLE_OK;

  స్థితి SSH_FX_NO_SUCH_FILE:
  స్థితి SSH_FX_NO_SUCH_మార్గం:
    తిరిగి CURLE_REMOTE_FILE_NOT_దొరికింది;

  స్థితి SSH_FX_PERMISSION_DENIED:
  స్థితి SSH_FX_WRITE_PROTECT:
    తిరిగి CURLE_REMOTE_ACCESS_DENIED;

  స్థితి SSH_FX_FILE_ALREADY_EXISTS:
    తిరిగి CURLE_REMOTE_FILE_EXISTS;

  అప్రమేయ:
    ఆపు;
  }

  తిరిగి CURLE_SSH;
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
స్థిర పూర్ణాంకం myssh_is_తెలిసిన(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc;
  ssh_కీలకం pubkey;
  గుర్తులేని అక్షరం *hash_sha256 = ఖాళీ;
  size_t hlen_sha256;
  గుర్తులేని అక్షరం *hash_md5 = ఖాళీ;
  size_t hlen_md5;
  అక్షరం *దొరికింది_base64 = ఖాళీ;
  అక్షరం *తెలిసిన_base64 = ఖాళీ;
  పూర్ణాంకం vstate;
  గణన curl_khmatch keymatch;
  నిర్మాణం curl_khkey foundkey;
  నిర్మాణం curl_khkey *knownkeyp = ఖాళీ;
  curl_sshkeycallback func = దత్తాంశం->సమితి.ssh_keyfunc;
  నిర్మాణం ssh_knownhosts_నమోదు *knownhostsentry = ఖాళీ;
  నిర్మాణం curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  ఒకవేళ(rc != SSH_OK)
    తిరిగి rc;

  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_SHA256)) {
    స్థిరరాశి అక్షరం *pubkey_sha256 =
      CURL_EASY_STR(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_SHA256);
    అక్షరం *fingerprint_b64 = ఖాళీ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    ఒకవేళ(rc != SSH_OK || hlen_sha256 != 32) {
      failf(దత్తాంశం, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      వెళ్ళు cleanup;
    }

    ఒకవేళ(curlx_base64_encode((స్థిరరాశి uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      వెళ్ళు cleanup;
    }

    infof(దత్తాంశం, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    అయితే((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    అయితే((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    ఒకవేళ((pub_pos != b64_pos) ||
       అక్షరశ్రేణి_పోలిక(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(దత్తాంశం,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      వెళ్ళు cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    వెళ్ళు cleanup;
  }

  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_MD5)) {
    స్థిరరాశి అక్షరం *pubkey_md5 =
      CURL_EASY_STR(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_MD5);
    అక్షరం md5buffer[33];
    పూర్ణాంకం i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    ఒకవేళ(rc != SSH_OK || hlen_md5 != 16) {
      failf(దత్తాంశం,
            "Denied establishing ssh session: MD5 fingerprint not available");
      వెళ్ళు cleanup;
    }

    కొరకు(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(దత్తాంశం, "SSH MD5 fingerprint: %s", md5buffer);

    ఒకవేళ(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(దత్తాంశం,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      వెళ్ళు cleanup;
    }

    rc = SSH_OK;
    వెళ్ళు cleanup;
  }

  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_తెలిసిన_hosts_నమోదు(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    ఒకవేళ(knownhostsentry) {
      ఒకవేళ(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &తెలిసిన_base64);
        ఒకవేళ(rc != SSH_OK) {
          వెళ్ళు cleanup;
        }
        knownkey.కీలకం = తెలిసిన_base64;
        knownkey.len = అక్షరశ్రేణి_పొడవు(తెలిసిన_base64);

        ఎంపిక(ssh_కీలకం_type(knownhostsentry->publickey)) {
        స్థితి SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          ఆపు;
        స్థితి SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          ఆపు;
        స్థితి SSH_KEYTYPE_ECDSA:
        స్థితి SSH_KEYTYPE_ECDSA_P256:
        స్థితి SSH_KEYTYPE_ECDSA_P384:
        స్థితి SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          ఆపు;
        స్థితి SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          ఆపు;
        స్థితి SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          ఆపు;
        అప్రమేయ:
          rc = SSH_ERROR;
          వెళ్ళు cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    ఎంపిక(vstate) {
    స్థితి SSH_తెలిసిన_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      ఆపు;
    స్థితి SSH_తెలిసిన_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      ఆపు;
    స్థితి SSH_తెలిసిన_HOSTS_NOT_దొరికింది:
    స్థితి SSH_తెలిసిన_HOSTS_UNKNOWN:
    స్థితి SSH_తెలిసిన_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      ఆపు;
    అప్రమేయ:
      keymatch = CURLKHMATCH_MISMATCH;
      ఆపు;
    }

    ఒకవేళ(func) { /* use callback to determine action */
      నిర్మాణం Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &దొరికింది_base64);
      ఒకవేళ(rc != SSH_OK)
        వెళ్ళు cleanup;

      foundkey.కీలకం = దొరికింది_base64;
      foundkey.len = అక్షరశ్రేణి_పొడవు(దొరికింది_base64);

      ఎంపిక(ssh_కీలకం_type(pubkey)) {
      స్థితి SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        ఆపు;
      స్థితి SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        ఆపు;
      స్థితి SSH_KEYTYPE_ECDSA:
      స్థితి SSH_KEYTYPE_ECDSA_P256:
      స్థితి SSH_KEYTYPE_ECDSA_P384:
      స్థితి SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        ఆపు;
      స్థితి SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        ఆపు;
      స్థితి SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        ఆపు;
      అప్రమేయ:
        rc = SSH_ERROR;
        వెళ్ళు cleanup;
      }

      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_ssh_keyfunc);
      rc = func(దత్తాంశం, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, దత్తాంశం->సమితి.ssh_keyfunc_userp);
      CURL_CBAPI_చివర(&guard);

      ఎంపిక(rc) {
      స్థితి CURLKHSTAT_FINE_కూడు_గమ్యం_FILE:
        rc = ssh_session_update_తెలిసిన_hosts(sshc->ssh_session);
        ఒకవేళ(rc != SSH_OK) {
          వెళ్ళు cleanup;
        }
        ఆపు;
      స్థితి CURLKHSTAT_FINE:
        ఆపు;
      అప్రమేయ: /* REJECT/DEFER */
        rc = SSH_ERROR;
        వెళ్ళు cleanup;
      }
    }
    లేకపోతే {
      ఒకవేళ(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        వెళ్ళు cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  ఒకవేళ(దొరికింది_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    విడుదల(దొరికింది_base64); /* allocated by libssh, deallocate with system free */
  }
  ఒకవేళ(తెలిసిన_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    విడుదల(తెలిసిన_base64); /* allocated by libssh, deallocate with system free */
  }
  ఒకవేళ(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  ఒకవేళ(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_కీలకం_free(pubkey);
  ఒకవేళ(knownhostsentry) {
    ssh_knownhosts_నమోదు_free(knownhostsentry);
  }
  తిరిగి rc;
}

స్థిర పూర్ణాంకం myssh_గమ్యం_ERROR(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం ssh_conn *sshc,
                          CURLcode ఫలితం)
{
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = ఫలితం;
  తిరిగి SSH_ERROR;
}

స్థిర పూర్ణాంకం myssh_గమ్యం_SFTP_CLOSE(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ssh_conn *sshc)
{
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_గమ్యం_CURLE(sftp_get_error(sshc->sftp_session));
  తిరిగి SSH_ERROR;
}

స్థిర పూర్ణాంకం myssh_గమ్యం_PASSWD_AUTH(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->auth_methods & SSH_AUTH_పద్ధతి_PASSWORD) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PASS_INIT);
    తిరిగి SSH_OK;
  }
  తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_LOGIN_DENIED);
}

స్థిర పూర్ణాంకం myssh_గమ్యం_కీలకం_AUTH(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->auth_methods & SSH_AUTH_పద్ధతి_INTERACTIVE) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_కీలకం_INIT);
    తిరిగి SSH_OK;
  }
  తిరిగి myssh_గమ్యం_PASSWD_AUTH(దత్తాంశం, sshc);
}

స్థిర పూర్ణాంకం myssh_గమ్యం_GSSAPI_AUTH(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->auth_methods & SSH_AUTH_పద్ధతి_GSSAPI_MIC) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_GSSAPI);
    తిరిగి SSH_OK;
  }
  తిరిగి myssh_గమ్యం_కీలకం_AUTH(దత్తాంశం, sshc);
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_READDIR_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ssh_conn *sshc,
                                      నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);
  Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, -1);
  ఒకవేళ(దత్తాంశం->req.no_body) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
    తిరిగి SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->మార్గం);
  ఒకవేళ(!sshc->sftp_dir) {
    failf(దత్తాంశం, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    తిరిగి myssh_గమ్యం_SFTP_CLOSE(దత్తాంశం, sshc);
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_READDIR(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ssh_conn *sshc,
                                 నిర్మాణం SSHPROTO *sshp)
{
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  ఒకవేళ(sshc->readdir_attrs)
    sftp_లక్షణాలు_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  ఒకవేళ(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = అక్షరశ్రేణి_పొడవు(sshc->readdir_filename);

    ఒకవేళ(దత్తాంశం->సమితి.list_only) {
      అక్షరం *tmp_పంక్తి;

      tmp_పంక్తి = curl_maprintf("%s\n", sshc->readdir_filename);
      ఒకవేళ(!tmp_పంక్తి) {
        myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        తిరిగి SSH_ERROR;
      }
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                                 tmp_పంక్తి, sshc->readdir_len + 1);
      curlx_free(tmp_పంక్తి);

      ఒకవేళ(ఫలితం) {
        myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
        sshc->actualcode = ఫలితం;
        తిరిగి SSH_NO_ERROR;
      }
    }
    లేకపోతే {
      ఒకవేళ(curlx_dyn_కూడు(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
        తిరిగి SSH_ERROR;
      }

      ఒకవేళ((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_link_మార్గం = curl_maprintf("%s%s", sshp->మార్గం,
                                               sshc->readdir_filename);

        ఒకవేళ(!sshc->readdir_link_మార్గం) {
          myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
          తిరిగి SSH_ERROR;
        }

        myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_LINK);
        తిరిగి SSH_NO_ERROR;
      }
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_BOTTOM);
      తిరిగి SSH_NO_ERROR;
    }
  }
  లేకపోతే ఒకవేళ(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_పూర్తి);
  }
  లేకపోతే {
    failf(దత్తాంశం, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    తిరిగి myssh_గమ్యం_SFTP_CLOSE(దత్తాంశం, sshc);
  }
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_READDIR_LINK(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->readdir_link_attrs)
    sftp_లక్షణాలు_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_link_మార్గం);
  ఒకవేళ(!sshc->readdir_link_attrs) {
    failf(దత్తాంశం, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    తిరిగి myssh_గమ్యం_SFTP_CLOSE(దత్తాంశం, sshc);
  }

  ఒకవేళ(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_link_మార్గం);
    ఒకవేళ(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    లేకపోతే
      sshc->readdir_len = అక్షరశ్రేణి_పొడవు(sshc->readdir_tmp);
    sshc->readdir_longentry = ఖాళీ;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  లేకపోతే {
    sshc->readdir_len = అక్షరశ్రేణి_పొడవు(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_link_మార్గం);

  ఒకవేళ(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    తిరిగి SSH_ERROR;
  }

  sftp_లక్షణాలు_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = ఖాళీ;
  sshc->readdir_filename = ఖాళీ;
  sshc->readdir_longentry = ఖాళీ;

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_BOTTOM);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_READDIR_BOTTOM(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ssh_conn *sshc)
{
  CURLcode ఫలితం;

  ఒకవేళ(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  లేకపోతే
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = ఖాళీ;

  ఒకవేళ(ఫలితం) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
    sshc->actualcode = ఫలితం;
  }
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_READDIR_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = ఖాళీ;

  /* no data to transfer */
  Curl_xfer_అమరిక_nop(దత్తాంశం);
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి SSH_NO_ERROR;
}

స్థిర శూన్య myssh_quote_error(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ssh_conn *sshc,
                              స్థిరరాశి అక్షరం *cmd)
{
  ఒకవేళ(cmd)
    failf(దత్తాంశం, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE_STATVFS(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  ఒకవేళ(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(దత్తాంశం, sshc, "statvfs");
    తిరిగి SSH_OK;
  }
  లేకపోతే ఒకవేళ(statvfs) {
#నిర్వచితమైతే _MSC_VER
#నిర్వచించు CURL_LIBSSH_VFS_కొలత_ముసుగు "I64u"
#లేదావేళ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#నిర్వచించు CURL_LIBSSH_VFS_కొలత_ముసుగు "llu"
#లేకపోతే
#నిర్వచించు CURL_LIBSSH_VFS_కొలత_ముసుగు PRIu64
#అయితే_ముగింపు
    CURLcode ఫలితం = CURLE_OK;
    అక్షరం *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_files: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_కొలత_ముసుగు "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_ఖండాలు, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    ఒకవేళ(!tmp)
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    ఒకవేళ(!ఫలితం) {
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, tmp, అక్షరశ్రేణి_పొడవు(tmp));
      curlx_free(tmp);
    }
    ఒకవేళ(ఫలితం) {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ఫలితం;
    }
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి SSH_OK;
}

స్థిర పూర్ణాంకం myssh_auth_interactive(నిర్మాణం connectdata *conn,
                                  నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc;
  పూర్ణాంకం nprompts;

restart:
  ఎంపిక(sshc->kbd_state) {
  స్థితి 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, ఖాళీ, ఖాళీ);
    ఒకవేళ(rc == SSH_AUTH_AGAIN)
      తిరిగి SSH_AGAIN;

    ఒకవేళ(rc != SSH_AUTH_INFO)
      తిరిగి SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    ఒకవేళ(nprompts != 1)
      తిరిగి SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    ఒకవేళ(rc < 0)
      తిరిగి SSH_ERROR;

    FALLTHROUGH();
  స్థితి 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ఖాళీ, ఖాళీ);
    ఒకవేళ(rc == SSH_AUTH_AGAIN)
      తిరిగి SSH_AGAIN;
    లేకపోతే ఒకవేళ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    లేకపోతే ఒకవేళ(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      ఒకవేళ(nprompts)
        తిరిగి SSH_ERROR;

      sshc->kbd_state = 2;
      వెళ్ళు restart;
    }
    లేకపోతే
      rc = SSH_ERROR;
    ఆపు;
  స్థితి 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ఖాళీ, ఖాళీ);
    ఒకవేళ(rc == SSH_AUTH_AGAIN)
      తిరిగి SSH_AGAIN;
    లేకపోతే ఒకవేళ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    లేకపోతే
      rc = SSH_ERROR;

    ఆపు;
  అప్రమేయ:
    తిరిగి SSH_ERROR;
  }

  sshc->kbd_state = 0;
  తిరిగి rc;
}

స్థిర శూన్య myssh_state_init(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ssh_conn *sshc)
{
  sshc->రెండవ_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#ఒకవేళ 0
  ssh_సమితి_log_level(SSH_LOG_PROTOCOL);
#అయితే_ముగింపు

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_సమితి_blocking(sshc->ssh_session, 0);

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_S_STARTUP);
}

స్థిర శూన్య myssh_block2waitfor(నిర్మాణం connectdata *conn,
                                నిర్మాణం ssh_conn *sshc,
                                bool ఖండం)
{
  (శూన్య)conn;
  ఒకవేళ(ఖండం) {
    పూర్ణాంకం dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  లేకపోతే
    sshc->waitfor = 0;
}

స్థిర పూర్ణాంకం myssh_లోపల_S_STARTUP(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ssh_conn *sshc)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  పూర్ణాంకం rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  ఒకవేళ(rc == SSH_AGAIN) {
    CURL_TRC_SSH(దత్తాంశం, "connect -> EAGAIN");
  }
  లేకపోతే ఒకవేళ(rc != SSH_OK) {
    failf(దత్తాంశం, "Failure establishing ssh session");
    rc = myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);
  }
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_HOSTKEY);

  తిరిగి rc;
}

స్థిర పూర్ణాంకం myssh_లోపల_AUTHLIST(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, ఖాళీ);
  ఒకవేళ(rc == SSH_AUTH_AGAIN)
    తిరిగి SSH_AGAIN;

  ఒకవేళ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(దత్తాంశం, "Authenticated with none");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
    తిరిగి rc;
  }
  లేకపోతే ఒకవేళ(rc == SSH_AUTH_ERROR) {
    rc = myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_LOGIN_DENIED);
    తిరిగి rc;
  }

  sshc->auth_methods =
    (గుర్తులేని పూర్ణాంకం)ssh_userauth_list(sshc->ssh_session, ఖాళీ);
  ఒకవేళ(sshc->auth_methods)
    infof(దత్తాంశం, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_పద్ధతి_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_పద్ధతి_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_పద్ధతి_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_పద్ధతి_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  ఒకవేళ((sshc->auth_methods & SSH_AUTH_పద్ధతి_PUBLICKEY) &&
     ((sshc->priv_కీలకం && sshc->priv_కీలకం[0]) ||
      (దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PKEY_INIT);
    infof(దత్తాంశం, "Authentication using SSH public key file");
  }
  లేకపోతే ఒకవేళ(sshc->auth_methods & SSH_AUTH_పద్ధతి_GSSAPI_MIC) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_GSSAPI);
  }
  లేకపోతే ఒకవేళ(sshc->auth_methods & SSH_AUTH_పద్ధతి_INTERACTIVE) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_కీలకం_INIT);
  }
  లేకపోతే ఒకవేళ(sshc->auth_methods & SSH_AUTH_పద్ధతి_PASSWORD) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PASS_INIT);
  }
  లేకపోతే {                  /* unsupported authentication method */
    rc = myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_LOGIN_DENIED);
  }
  తిరిగి rc;
}

స్థిర పూర్ణాంకం myssh_లోపల_AUTH_PKEY_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc;
  ఒకవేళ(!(దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_గమ్యం_GSSAPI_AUTH(దత్తాంశం, sshc);
    తిరిగి rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  ఒకవేళ(sshc->priv_కీలకం && sshc->priv_కీలకం[0]) {
    ఒకవేళ(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, ఖాళీ, sshc->pubkey);
      ఒకవేళ(rc == SSH_AUTH_AGAIN)
        తిరిగి SSH_AGAIN;

      ఒకవేళ(rc != SSH_OK) {
        rc = myssh_గమ్యం_GSSAPI_AUTH(దత్తాంశం, sshc);
        తిరిగి rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_కీలకం,
                                     sshc->passphrase, ఖాళీ,
                                     ఖాళీ, &sshc->privkey);
    ఒకవేళ(rc != SSH_OK) {
      failf(దత్తాంశం, "Could not load private key file %s", sshc->priv_కీలకం);
      rc = myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_LOGIN_DENIED);
      తిరిగి rc;
    }

    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PKEY);
  }
  లేకపోతే {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, ఖాళీ,
                                     sshc->passphrase);
    ఒకవేళ(rc == SSH_AUTH_AGAIN)
      తిరిగి SSH_AGAIN;

    ఒకవేళ(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(దత్తాంశం, "Completed public key authentication");
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
      తిరిగి rc;
    }

    rc = myssh_గమ్యం_GSSAPI_AUTH(దత్తాంశం, sshc);
  }
  తిరిగి rc;
}

స్థిర పూర్ణాంకం myssh_లోపల_AUTH_PKEY(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = ssh_userauth_publickey(sshc->ssh_session, ఖాళీ, sshc->privkey);
  ఒకవేళ(rc == SSH_AUTH_AGAIN)
    తిరిగి SSH_AGAIN;
  లేకపోతే ఒకవేళ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(దత్తాంశం, "Completed public key authentication");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
    తిరిగి SSH_OK;
  }
  లేకపోతే {
    infof(దత్తాంశం, "Failed public key authentication (rc: %d)", rc);
    తిరిగి myssh_గమ్యం_GSSAPI_AUTH(దత్తాంశం, sshc);
  }
}

స్థిర పూర్ణాంకం myssh_లోపల_AUTH_GSSAPI(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc;
  ఒకవేళ(!(దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    తిరిగి myssh_గమ్యం_కీలకం_AUTH(దత్తాంశం, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  ఒకవేళ(rc == SSH_AUTH_AGAIN)
    తిరిగి SSH_AGAIN;

  ఒకవేళ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(దత్తాంశం, "Completed gssapi authentication");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
    తిరిగి SSH_OK;
  }

  తిరిగి myssh_గమ్యం_కీలకం_AUTH(దత్తాంశం, sshc);
}

స్థిర పూర్ణాంకం myssh_లోపల_AUTH_కీలకం_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_కీలకం);
    తిరిగి SSH_NO_ERROR;
  }
  తిరిగి myssh_గమ్యం_PASSWD_AUTH(దత్తాంశం, sshc);
}

స్థిర పూర్ణాంకం myssh_లోపల_AUTH_కీలకం(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  పూర్ణాంకం rc = myssh_auth_interactive(దత్తాంశం->conn, sshc);
  ఒకవేళ(rc == SSH_AGAIN)
    తిరిగి rc;
  లేకపోతే ఒకవేళ(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(దత్తాంశం, "completed keyboard interactive authentication");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
    తిరిగి SSH_NO_ERROR;
  }
  లేకపోతే
    తిరిగి myssh_గమ్యం_PASSWD_AUTH(దత్తాంశం, sshc);
}

స్థిర పూర్ణాంకం myssh_లోపల_AUTH_PASS_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(!(దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_LOGIN_DENIED);
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PASS);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_AUTH_PASS(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = ssh_userauth_password(sshc->ssh_session, ఖాళీ,
                                 Curl_creds_passwd(దత్తాంశం->conn->creds));
  ఒకవేళ(rc == SSH_AUTH_AGAIN)
    తిరిగి SSH_AGAIN;
  లేకపోతే ఒకవేళ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(దత్తాంశం, "Completed password authentication");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
    తిరిగి SSH_NO_ERROR;
  }
  తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_LOGIN_DENIED);
}

స్థిర పూర్ణాంకం myssh_లోపల_AUTH_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ssh_conn *sshc)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  ఒకవేళ(!sshc->authed) {
    failf(దత్తాంశం, "Authentication failure");
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(దత్తాంశం, "Authentication complete");
  Curl_pgrs_సమయం(దత్తాంశం, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ఒకవేళ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_INIT);
    తిరిగి SSH_NO_ERROR;
  }
  infof(దత్తాంశం, "SSH CONNECT phase done");
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_UPLOAD_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ssh_conn *sshc,
                                నిర్మాణం SSHPROTO *sshp)
{
  పూర్ణాంకం flags;
  పూర్ణాంకం rc = 0;

  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);

  ఒకవేళ(దత్తాంశం->state.resume_from) {
    sftp_లక్షణాలు attrs;

    ఒకవేళ(దత్తాంశం->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->మార్గం);
      ఒకవేళ(attrs) {
        curl_off_t కొలత = attrs->కొలత;
        sftp_లక్షణాలు_free(attrs);
        ఒకవేళ(కొలత < 0) {
          failf(దత్తాంశం, "Bad file size (%" FMT_OFF_T ")", కొలత);
          rc = myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          తిరిగి rc;
        }
        దత్తాంశం->state.resume_from = కొలత;
      }
      లేకపోతే {
        దత్తాంశం->state.resume_from = 0;
      }
    }
  }

  ఒకవేళ(దత్తాంశం->సమితి.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  లేకపోతే
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  ఒకవేళ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->మార్గం,
              flags, (mode_t)దత్తాంశం->సమితి.new_file_perms);
  ఒకవేళ(!sshc->sftp_file) {
    పూర్ణాంకం err = sftp_get_error(sshc->sftp_session);

    ఒకవేళ((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_మార్గం) &&
       దత్తాంశం->సమితి.ftp_create_missing_dirs &&
       (అక్షరశ్రేణి_పొడవు(sshp->మార్గం) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->రెండవ_Create_Dirs = 1;
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      తిరిగి rc;
    }
    లేకపోతే {
      rc = myssh_గమ్యం_SFTP_CLOSE(దత్తాంశం, sshc);
      తిరిగి rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  ఒకవేళ(దత్తాంశం->state.resume_from > 0 && !దత్తాంశం->సమితి.remote_append) {
    పూర్ణాంకం seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    ఒకవేళ(దత్తాంశం->సమితి.seek_func) {
      నిర్మాణం Curl_mapi_guard guard;
      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_seek_func);
      seekerr = దత్తాంశం->సమితి.seek_func(దత్తాంశం->సమితి.seek_client,
                                    దత్తాంశం->state.resume_from, SEEK_SET);
      CURL_CBAPI_చివర(&guard);
    }

    ఒకవేళ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      ఒకవేళ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(దత్తాంశం, "Could not seek stream");
        rc = myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FTP_COULDNT_USE_REST);
        తిరిగి rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      చేయి {
        అక్షరం scratch[4 * 1024];
        size_t readthisamountnow =
          (దత్తాంశం->state.resume_from - passed >
           (curl_off_t)పరిమాణం(scratch)) ?
          పరిమాణం(scratch) : curlx_sotouz(దత్తాంశం->state.resume_from - passed);

        size_t actuallyread =
          దత్తాంశం->state.fread_func(scratch, 1,
                                 readthisamountnow, దత్తాంశం->state.లోపల);

        passed += actuallyread;
        ఒకవేళ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(దత్తాంశం, "Failed to read data");
          rc = myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FTP_COULDNT_USE_REST);
          తిరిగి rc;
        }
      } అయితే(passed < దత్తాంశం->state.resume_from);
    }

    /* now, decrease the size of the read */
    ఒకవేళ(దత్తాంశం->state.infilesize > 0) {
      ఒకవేళ(దత్తాంశం->state.resume_from > దత్తాంశం->state.infilesize) {
        failf(దత్తాంశం, "Resume point beyond size");
        తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_BAD_FUNCTION_వాదం);
      }
      దత్తాంశం->state.infilesize -= దత్తాంశం->state.resume_from;
      దత్తాంశం->req.కొలత = దత్తాంశం->state.infilesize;
      Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, దత్తాంశం->state.resume_from);
    ఒకవేళ(rc) {
      rc = myssh_గమ్యం_SFTP_CLOSE(దత్తాంశం, sshc);
      తిరిగి rc;
    }
  }
  ఒకవేళ(దత్తాంశం->state.infilesize > 0) {
    దత్తాంశం->req.కొలత = దత్తాంశం->state.infilesize;
    Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);
  }
  /* upload data */
  Curl_xfer_అమరిక_send(దత్తాంశం, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  దత్తాంశం->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(దత్తాంశం);
#ఒకవేళ LIBSSH_సంస్కరణ_INT > SSH_సంస్కరణ_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#అయితే_ముగింపు
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి rc;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_DOWNLOAD_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc,
                                       నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  ఒకవేళ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->మార్గం,
                              O_RDONLY, (mode_t)దత్తాంశం->సమితి.new_file_perms);
  ఒకవేళ(!sshc->sftp_file) {
    failf(దత్తాంశం, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    తిరిగి myssh_గమ్యం_SFTP_CLOSE(దత్తాంశం, sshc);
  }
  sftp_file_సమితి_nonblocking(sshc->sftp_file);
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_DOWNLOAD_STAT);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_DOWNLOAD_STAT(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc)
{
  curl_off_t కొలత;
  పూర్ణాంకం rc = 0;
  sftp_లక్షణాలు attrs = sftp_fstat(sshc->sftp_file);
  ఒకవేళ(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_కొలత) ||
     (attrs->కొలత == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    దత్తాంశం->req.కొలత = -1;
    దత్తాంశం->req.maxdownload = -1;
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, -1);
    కొలత = 0;
    ఒకవేళ(attrs)
      sftp_లక్షణాలు_free(attrs);
  }
  లేకపోతే {
    కొలత = attrs->కొలత;

    sftp_లక్షణాలు_free(attrs);

    ఒకవేళ(కొలత < 0) {
      failf(దత్తాంశం, "Bad file size (%" FMT_OFF_T ")", కొలత);
      తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    ఒకవేళ(దత్తాంశం->state.use_range) {
      curl_off_t from;
      CURLcode ఫలితం = Curl_ssh_range(దత్తాంశం, దత్తాంశం->state.range, కొలత,
                                       &from, &కొలత);
      ఒకవేళ(ఫలితం)
        తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, ఫలితం);

      rc = sftp_seek64(sshc->sftp_file, from);
      ఒకవేళ(rc)
        తిరిగి myssh_గమ్యం_SFTP_CLOSE(దత్తాంశం, sshc);
    }
    దత్తాంశం->req.కొలత = కొలత;
    దత్తాంశం->req.maxdownload = కొలత;
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, కొలత);
  }

  /* We can resume if we can seek to the resume position */
  ఒకవేళ(దత్తాంశం->state.resume_from) {
    ఒకవేళ(దత్తాంశం->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ఒకవేళ(కొలత < -దత్తాంశం->state.resume_from) {
        failf(దత్తాంశం, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", దత్తాంశం->state.resume_from, కొలత);
        తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      దత్తాంశం->state.resume_from += కొలత;
    }
    లేకపోతే {
      ఒకవేళ(కొలత < దత్తాంశం->state.resume_from) {
        failf(దత్తాంశం, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              దత్తాంశం->state.resume_from, కొలత);
        తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    దత్తాంశం->req.కొలత = కొలత - దత్తాంశం->state.resume_from;
    దత్తాంశం->req.maxdownload = కొలత - దత్తాంశం->state.resume_from;
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, కొలత - దత్తాంశం->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, దత్తాంశం->state.resume_from);
    ఒకవేళ(rc)
      తిరిగి myssh_గమ్యం_SFTP_CLOSE(దత్తాంశం, sshc);
  }

  /* Setup the actual download */
  ఒకవేళ(దత్తాంశం->req.కొలత == 0) {
    /* no data to transfer */
    Curl_xfer_అమరిక_nop(దత్తాంశం);
    infof(దత్తాంశం, "File already completely downloaded");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
    తిరిగి rc;
  }
  Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, దత్తాంశం->req.కొలత);

  /* not set by Curl_xfer_setup to preserve io_flags */
  దత్తాంశం->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);

  తిరిగి rc;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_CLOSE(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ssh_conn *sshc,
                               నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);
  ఒకవేళ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ఖాళీ;
  }
  curlx_safefree(sshp->మార్గం);

  CURL_TRC_SSH(దత్తాంశం, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  ఒకవేళ(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_గమ్యం(దత్తాంశం, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  }
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_SHUTDOWN(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_సమితి_blocking(sshc->ssh_session, 0);
#ఒకవేళ LIBSSH_సంస్కరణ_INT > SSH_సంస్కరణ_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#అయితే_ముగింపు

  ఒకవేళ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ఖాళీ;
  }

  ఒకవేళ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ఖాళీ;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_DISCONNECT);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc;
  ssh_సమితి_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  ఒకవేళ(!sshc->sftp_session) {
    failf(దత్తాంశం, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  ఒకవేళ(rc != SSH_OK) {
    failf(దత్తాంశం, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, sftp_error_గమ్యం_CURLE(SSH_FX_FAILURE));
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_REALPATH);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_REALPATH(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_మార్గం(sshc->sftp_session, ".");
  ఒకవేళ(!sshc->homedir)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(దత్తాంశం->state.most_recent_ftp_entrypath);
  దత్తాంశం->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  ఒకవేళ(!దత్తాంశం->state.most_recent_ftp_entrypath)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(దత్తాంశం, "CONNECT phase done");
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc,
                                    నిర్మాణం SSHPROTO *sshp)
{
  CURLcode ఫలితం;

  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);

  ఫలితం = Curl_getworkingpath(దత్తాంశం, sshc->homedir, &sshp->మార్గం);
  ఒకవేళ(ఫలితం) {
    sshc->actualcode = ఫలితం;
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->సమితి.quote) {
    infof(దత్తాంశం, "Sending quote commands");
    sshc->quote_అంశం = దత్తాంశం->సమితి.quote;
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE);
  }
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_GETINFO);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_POSTQUOTE_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(దత్తాంశం->సమితి.postquote) {
    infof(దత్తాంశం, "Sending quote commands");
    sshc->quote_అంశం = దత్తాంశం->సమితి.postquote;
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  }
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం quote_error(నిర్మాణం Curl_easy *దత్తాంశం,
                       నిర్మాణం ssh_conn *sshc)
{
  failf(దత్తాంశం, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ssh_conn *sshc,
                               నిర్మాణం SSHPROTO *sshp)
{
  స్థిరరాశి అక్షరం *cp;
  CURLcode ఫలితం;

  /*
   * Support some of the "FTP" commands
   */
  స్థిరరాశి అక్షరం *cmd = sshc->quote_అంశం->దత్తాంశం;
  sshc->acceptfail = FALSE;

  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ఒకవేళ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ఒకవేళ(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    అక్షరం *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->మార్గం);
    ఒకవేళ(!tmp) {
      sshc->actualcode = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      తిరిగి SSH_NO_ERROR;
    }
    Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_బహిర్గతం, "PWD\n", 4);
    Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, tmp, అక్షరశ్రేణి_పొడవు(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, tmp, అక్షరశ్రేణి_పొడవు(tmp));
    curlx_free(tmp);
    ఒకవేళ(ఫలితం) {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ఫలితం;
    }
    లేకపోతే
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
    తిరిగి SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ఒకవేళ(!cp) {
    failf(దత్తాంశం, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    తిరిగి SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  ఫలితం = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
      failf(దత్తాంశం, "Out of memory");
    లేకపోతే
      failf(దత్తాంశం, "Syntax error: Bad first parameter");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = ఫలితం;
    తిరిగి SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "chgrp ", 6) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "chmod ", 6) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "chown ", 6) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "atime ", 6) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    ఫలితం = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
        failf(దత్తాంశం, "Out of memory");
      లేకపోతే
        failf(దత్తాంశం, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ఫలితం;
      తిరిగి SSH_NO_ERROR;
    }
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    sshc->quote_attrs = ఖాళీ;
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_STAT);
    తిరిగి SSH_NO_ERROR;
  }
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "ln ", 3) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    ఫలితం = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
        failf(దత్తాంశం, "Out of memory");
      లేకపోతే
        failf(దత్తాంశం, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ఫలితం;
      తిరిగి SSH_NO_ERROR;
    }
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_SYMLINK);
    తిరిగి SSH_NO_ERROR;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "mkdir ", 6)) {
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    /* create directory */
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_MKDIR);
    తిరిగి SSH_NO_ERROR;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    ఫలితం = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం == CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
        failf(దత్తాంశం, "Out of memory");
      లేకపోతే
        failf(దత్తాంశం, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = ఫలితం;
      తిరిగి SSH_NO_ERROR;
    }
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_RENAME);
    తిరిగి SSH_NO_ERROR;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "rmdir ", 6)) {
    /* delete directory */
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_RMDIR);
    తిరిగి SSH_NO_ERROR;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "rm ", 3)) {
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_UNLINK);
    తిరిగి SSH_NO_ERROR;
  }
#నిర్వచితమైతే HAS_STATVFS_SUPPORT
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "statvfs ", 8)) {
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_STATVFS);
    తిరిగి SSH_NO_ERROR;
  }
#అయితే_ముగింపు

  failf(దత్తాంశం, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_తదుపరి_QUOTE(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_అంశం = sshc->quote_అంశం->తదుపరి;

  ఒకవేళ(sshc->quote_అంశం) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE);
  }
  లేకపోతే {
    ఒకవేళ(sshc->nextstate != SSH_NO_STATE) {
      myssh_గమ్యం(దత్తాంశం, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    లేకపోతే {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_GETINFO);
    }
  }
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE_STAT(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc)
{
  స్థిరరాశి అక్షరం *cmd = sshc->quote_అంశం->దత్తాంశం;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ఒకవేళ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  ఒకవేళ(sshc->quote_attrs)
    sftp_లక్షణాలు_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  ఒకవేళ(!sshc->quote_attrs) {
    failf(దత్తాంశం, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(దత్తాంశం, sshc, ఖాళీ);
    తిరిగి SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "chgrp", 5)) {
    స్థిరరాశి అక్షరం *p = sshc->quote_path1;
    curl_off_t gid;
    ఒకవేళ(curlx_str_సంఖ్య(&p, &gid, UINT_MAX)) {
      failf(దత్తాంశం, "Syntax error: chgrp gid not a number");
      myssh_quote_error(దత్తాంశం, sshc, ఖాళీ);
      తిరిగి SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "chmod", 5)) {
    curl_off_t perms;
    స్థిరరాశి అక్షరం *p = sshc->quote_path1;
    ఒకవేళ(curlx_str_octal(&p, &perms, 07777)) {
      failf(దత్తాంశం, "Syntax error: chmod permissions not a number");
      myssh_quote_error(దత్తాంశం, sshc, ఖాళీ);
      తిరిగి SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "chown", 5)) {
    స్థిరరాశి అక్షరం *p = sshc->quote_path1;
    curl_off_t uid;
    ఒకవేళ(curlx_str_సంఖ్య(&p, &uid, UINT_MAX)) {
      failf(దత్తాంశం, "Syntax error: chown uid not a number");
      myssh_quote_error(దత్తాంశం, sshc, ఖాళీ);
      తిరిగి SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "atime", 5) ||
          !అక్షరశ్రేణి_పోలిక(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    ఒకవేళ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(దత్తాంశం, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ఒకవేళ SIZEOF_సమయం_T > 4
    లేకపోతే ఒకవేళ(date > 0xffffffff) {
      failf(దత్తాంశం, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#అయితే_ముగింపు
    ఒకవేళ(fail) {
      myssh_quote_error(దత్తాంశం, sshc, ఖాళీ);
      తిరిగి SSH_NO_ERROR;
    }
    ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    లేకపోతే /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_SETSTAT);
  తిరిగి SSH_NO_ERROR;
}

స్థిర శూన్య conn_forget_socket(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  ఒకవేళ(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    నిర్మాణం Curl_cfilter *cf = conn->cfilter[sockindex];
    ఒకవేళ(cf)
      (శూన్య)Curl_conn_cf_cntrl(cf, దత్తాంశం, TRUE, CF_CTRL_FORGET_SOCKET, 0, ఖాళీ);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

స్థిర శూన్య myssh_SESSION_DISCONNECT(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ఒకవేళ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ఖాళీ;
  }

  ఒకవేళ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ఖాళీ;
  }
  ఒకవేళ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ఖాళీ;
  }

  ssh_disconnect(sshc->ssh_session);
  ఒకవేళ(!ssh_సంస్కరణ(SSH_సంస్కరణ_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(దత్తాంశం, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
}

స్థిర పూర్ణాంకం myssh_SSH_SCP_DOWNLOAD(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ssh_conn *sshc)
{
  uint64_t bytecount;
  పూర్ణాంకం rc = ssh_scp_pull_request(sshc->scp_session);
  ఒకవేళ(rc != SSH_SCP_REQUEST_NEWFILE) {
    స్థిరరాశి అక్షరం *err_సందేశం = ssh_get_error(sshc->ssh_session);
    failf(దత్తాంశం, "%s", err_సందేశం);
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_REMOTE_FILE_NOT_దొరికింది);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  ఒకవేళ(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_TOO_LARGE);
  దత్తాంశం->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, దత్తాంశం->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  దత్తాంశం->conn->send_idx = 0;

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి 0;
}

స్థిర పూర్ణాంకం myssh_లోపల_TRANS_INIT(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ssh_conn *sshc,
                               నిర్మాణం SSHPROTO *sshp)
{
  CURLcode ఫలితం;
  పూర్ణాంకం rc = 0;
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);

  ఫలితం = Curl_getworkingpath(దత్తాంశం, sshc->homedir, &sshp->మార్గం);
  ఒకవేళ(ఫలితం) {
    sshc->actualcode = ఫలితం;
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
    తిరిగి 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_సమితి_blocking(sshc->ssh_session, 1);

  ఒకవేళ(దత్తాంశం->state.upload) {
    ఒకవేళ(దత్తాంశం->state.infilesize < 0) {
      failf(దత్తాంశం, "SCP requires a known file size for upload");
      తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->మార్గం);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_UPLOAD_INIT);
  }
  లేకపోతే {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->మార్గం);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  ఒకవేళ(!sshc->scp_session) {
    స్థిరరాశి అక్షరం *err_సందేశం = ssh_get_error(sshc->ssh_session);
    failf(దత్తాంశం, "%s", err_సందేశం);
    rc = myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_UPLOAD_FAILED);
  }
  తిరిగి rc;
}

స్థిర శూన్య sshc_cleanup(నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->initialized) {
    ఒకవేళ(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = ఖాళీ;
    }
    ఒకవేళ(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = ఖాళీ;
    }
    ఒకవేళ(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = ఖాళీ;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    ఒకవేళ(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = ఖాళీ;
    }
    ఒకవేళ(sshc->quote_attrs) {
      sftp_లక్షణాలు_free(sshc->quote_attrs);
      sshc->quote_attrs = ఖాళీ;
    }
    ఒకవేళ(sshc->readdir_attrs) {
      sftp_లక్షణాలు_free(sshc->readdir_attrs);
      sshc->readdir_attrs = ఖాళీ;
    }
    ఒకవేళ(sshc->readdir_link_attrs) {
      sftp_లక్షణాలు_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = ఖాళీ;
    }
    ఒకవేళ(sshc->privkey) {
      ssh_కీలకం_free(sshc->privkey);
      sshc->privkey = ఖాళీ;
    }
    ఒకవేళ(sshc->pubkey) {
      ssh_కీలకం_free(sshc->pubkey);
      sshc->pubkey = ఖాళీ;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_కీలకం);
    curlx_safefree(sshc->priv_కీలకం);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_link_మార్గం);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE_SETSTAT(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  ఒకవేళ(rc == SSH_AGAIN)
    తిరిగి rc;
  ఒకవేళ(rc && !sshc->acceptfail) {
    myssh_quote_error(దత్తాంశం, sshc, "setstat");
    తిరిగి rc;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE_SYMLINK(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  ఒకవేళ(rc == SSH_AGAIN)
    తిరిగి rc;
  ఒకవేళ(rc && !sshc->acceptfail) {
    myssh_quote_error(దత్తాంశం, sshc, "symlink");
    తిరిగి rc;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE_MKDIR(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)దత్తాంశం->సమితి.new_directory_perms);
  ఒకవేళ(rc == SSH_AGAIN)
    తిరిగి rc;
  ఒకవేళ(rc && !sshc->acceptfail) {
    myssh_quote_error(దత్తాంశం, sshc, "mkdir");
    తిరిగి rc;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE_RENAME(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  ఒకవేళ(rc == SSH_AGAIN)
    తిరిగి rc;
  ఒకవేళ(rc && !sshc->acceptfail) {
    myssh_quote_error(దత్తాంశం, sshc, "rename");
    తిరిగి rc;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE_RMDIR(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  ఒకవేళ(rc == SSH_AGAIN)
    తిరిగి rc;
  ఒకవేళ(rc && !sshc->acceptfail) {
    myssh_quote_error(దత్తాంశం, sshc, "rmdir");
    తిరిగి rc;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_QUOTE_UNLINK(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  ఒకవేళ(rc == SSH_AGAIN)
    తిరిగి rc;
  ఒకవేళ(rc && !sshc->acceptfail) {
    myssh_quote_error(దత్తాంశం, sshc, "rm");
    తిరిగి rc;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_GETINFO(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(దత్తాంశం->సమితి.get_filetime)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_FILETIME);
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_TRANS_INIT);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_FILETIME(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ssh_conn *sshc,
                                  నిర్మాణం SSHPROTO *sshp)
{
  sftp_లక్షణాలు attrs;
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->మార్గం);
  ఒకవేళ(attrs) {
    దత్తాంశం->info.filetime = attrs->mtime;
    sftp_లక్షణాలు_free(attrs);
  }

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_TRANS_INIT);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_TRANS_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc,
                                    నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);
  ఒకవేళ(దత్తాంశం->state.upload)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_UPLOAD_INIT);
  లేకపోతే {
    size_t మార్గం_len = అక్షరశ్రేణి_పొడవు(sshp->మార్గం);

    ఒకవేళ(మార్గం_len && sshp->మార్గం[మార్గం_len - 1] == '/')
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_INIT);
    లేకపోతే
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_CREATE_DIRS_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం ssh_conn *sshc,
                                          నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);

  ఒకవేళ(అక్షరశ్రేణి_పొడవు(sshp->మార్గం) > 1) {
    sshc->slash_pos = sshp->మార్గం + 1; /* ignore the leading '/' */
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CREATE_DIRS);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_CREATE_DIRS(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ssh_conn *sshc,
                                     నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ఒకవేళ(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(దత్తాంశం, "Creating directory '%s'", sshp->మార్గం);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    తిరిగి SSH_NO_ERROR;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_UPLOAD_INIT);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SFTP_CREATE_DIRS_MKDIR(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం ssh_conn *sshc,
                                           నిర్మాణం SSHPROTO *sshp)
{
  పూర్ణాంకం rc;
  పూర్ణాంకం err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->మార్గం,
                  (mode_t)దత్తాంశం->సమితి.new_directory_perms);
  ఒకవేళ(rc == SSH_AGAIN)
    తిరిగి rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ఒకవేళ(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    ఒకవేళ((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      తిరిగి myssh_గమ్యం_SFTP_CLOSE(దత్తాంశం, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CREATE_DIRS);
  తిరిగి rc;
}

స్థిర పూర్ణాంకం myssh_లోపల_SCP_UPLOAD_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc,
                                    నిర్మాణం SSHPROTO *sshp)
{
  పూర్ణాంకం rc;
  ఒకవేళ(!sshp)
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  ఒకవేళ(rc != SSH_OK) {
    స్థిరరాశి అక్షరం *err_సందేశం = ssh_get_error(sshc->ssh_session);
    failf(దత్తాంశం, "%s", err_సందేశం);
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_నెట్టు_file64(sshc->scp_session, sshp->మార్గం,
                           (uint64_t)దత్తాంశం->state.infilesize,
                           (పూర్ణాంకం)దత్తాంశం->సమితి.new_file_perms);

  ఒకవేళ(rc != SSH_OK) {
    స్థిరరాశి అక్షరం *err_సందేశం = ssh_get_error(sshc->ssh_session);
    failf(దత్తాంశం, "%s", err_సందేశం);
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_అమరిక_send(దత్తాంశం, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  దత్తాంశం->conn->recv_idx = FIRSTSOCKET;

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SCP_DOWNLOAD_INIT(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = ssh_scp_init(sshc->scp_session);
  ఒకవేళ(rc != SSH_OK) {
    స్థిరరాశి అక్షరం *err_సందేశం = ssh_get_error(sshc->ssh_session);
    failf(దత్తాంశం, "%s", err_సందేశం);
    తిరిగి myssh_గమ్యం_ERROR(దత్తాంశం, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_DOWNLOAD);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SCP_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(దత్తాంశం->state.upload)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_SEND_EOF);
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_CHANNEL_FREE);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SCP_SEND_EOF(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->scp_session) {
    పూర్ణాంకం rc = ssh_scp_close(sshc->scp_session);
    ఒకవేళ(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      తిరిగి SSH_AGAIN;
    }
    ఒకవేళ(rc != SSH_OK) {
      infof(దత్తాంశం,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_CHANNEL_FREE);
  తిరిగి SSH_NO_ERROR;
}

స్థిర పూర్ణాంకం myssh_లోపల_SCP_CHANNEL_FREE(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ఖాళీ;
  }
  CURL_TRC_SSH(దత్తాంశం, "SCP DONE phase complete");

  ssh_సమితి_blocking(sshc->ssh_session, 0);

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_DISCONNECT);
  తిరిగి SSH_NO_ERROR;
}

స్థిర CURLcode myssh_లోపల_SESSION_FREE(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ssh_conn *sshc)
{
  CURLcode ఫలితం;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  ఫలితం = sshc->actualcode;
  memset(sshc, 0, పరిమాణం(నిర్మాణం ssh_conn));
  connclose(దత్తాంశం->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి ఫలితం;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
స్థిర CURLcode myssh_statemachine(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ssh_conn *sshc,
                                   నిర్మాణం SSHPROTO *sshp,
                                   bool *ఖండం)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం rc = SSH_NO_ERROR;

  *ఖండం = FALSE;                   /* we are not blocking by default */
  చేయి {

    ఎంపిక(sshc->state) {
    స్థితి SSH_INIT:
      myssh_state_init(దత్తాంశం, sshc);
      FALLTHROUGH();
    స్థితి SSH_S_STARTUP:
      rc = myssh_లోపల_S_STARTUP(దత్తాంశం, sshc);
      ఒకవేళ(rc)
        ఆపు;
      FALLTHROUGH();
    స్థితి SSH_HOSTKEY:
      rc = myssh_is_తెలిసిన(దత్తాంశం, sshc);
      ఒకవేళ(rc != SSH_OK) {
        rc = myssh_గమ్యం_ERROR(దత్తాంశం, sshc, CURLE_PEER_FAILED_VERIFICATION);
        ఆపు;
      }
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    స్థితి SSH_AUTHLIST:
      rc = myssh_లోపల_AUTHLIST(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_AUTH_PKEY_INIT:
      rc = myssh_లోపల_AUTH_PKEY_INIT(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_AUTH_PKEY:
      rc = myssh_లోపల_AUTH_PKEY(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_AUTH_GSSAPI:
      rc = myssh_లోపల_AUTH_GSSAPI(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_AUTH_కీలకం_INIT:
      rc = myssh_లోపల_AUTH_కీలకం_INIT(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_AUTH_కీలకం:
      rc = myssh_లోపల_AUTH_కీలకం(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_AUTH_PASS_INIT:
      rc = myssh_లోపల_AUTH_PASS_INIT(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_AUTH_PASS:
      rc = myssh_లోపల_AUTH_PASS(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_AUTH_పూర్తి:
      rc = myssh_లోపల_AUTH_పూర్తి(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_INIT:
      rc = myssh_లోపల_SFTP_INIT(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_REALPATH:
      rc = myssh_లోపల_SFTP_REALPATH(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE_INIT:
      rc = myssh_లోపల_SFTP_QUOTE_INIT(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_లోపల_SFTP_POSTQUOTE_INIT(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE:
      rc = myssh_లోపల_SFTP_QUOTE(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_తదుపరి_QUOTE:
      rc = myssh_లోపల_SFTP_తదుపరి_QUOTE(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE_STAT:
      rc = myssh_లోపల_SFTP_QUOTE_STAT(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_లోపల_SFTP_QUOTE_SETSTAT(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_లోపల_SFTP_QUOTE_SYMLINK(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_లోపల_SFTP_QUOTE_MKDIR(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE_RENAME:
      rc = myssh_లోపల_SFTP_QUOTE_RENAME(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_లోపల_SFTP_QUOTE_RMDIR(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_లోపల_SFTP_QUOTE_UNLINK(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_లోపల_SFTP_QUOTE_STATVFS(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_GETINFO:
      rc = myssh_లోపల_SFTP_GETINFO(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_FILETIME:
      rc = myssh_లోపల_SFTP_FILETIME(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_TRANS_INIT:
      rc = myssh_లోపల_SFTP_TRANS_INIT(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_UPLOAD_INIT:
      rc = myssh_లోపల_UPLOAD_INIT(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_లోపల_SFTP_CREATE_DIRS_INIT(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_CREATE_DIRS:
      rc = myssh_లోపల_SFTP_CREATE_DIRS(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_లోపల_SFTP_CREATE_DIRS_MKDIR(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_READDIR_INIT:
      rc = myssh_లోపల_SFTP_READDIR_INIT(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_READDIR:
      rc = myssh_లోపల_SFTP_READDIR(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_READDIR_LINK:
      rc = myssh_లోపల_SFTP_READDIR_LINK(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_లోపల_SFTP_READDIR_BOTTOM(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_READDIR_పూర్తి:
      rc = myssh_లోపల_SFTP_READDIR_పూర్తి(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_లోపల_SFTP_DOWNLOAD_INIT(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_లోపల_SFTP_DOWNLOAD_STAT(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SFTP_CLOSE:
      rc = myssh_లోపల_SFTP_CLOSE(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SFTP_SHUTDOWN:
      rc = myssh_లోపల_SFTP_SHUTDOWN(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SCP_TRANS_INIT:
      rc = myssh_లోపల_TRANS_INIT(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SCP_UPLOAD_INIT:
      rc = myssh_లోపల_SCP_UPLOAD_INIT(దత్తాంశం, sshc, sshp);
      ఆపు;
    స్థితి SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_లోపల_SCP_DOWNLOAD_INIT(దత్తాంశం, sshc);
      ఒకవేళ(rc)
        ఆపు;
      FALLTHROUGH();
    స్థితి SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SCP_పూర్తి:
      rc = myssh_లోపల_SCP_పూర్తి(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SCP_SEND_EOF:
      rc = myssh_లోపల_SCP_SEND_EOF(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_SCP_CHANNEL_FREE:
      myssh_లోపల_SCP_CHANNEL_FREE(దత్తాంశం, sshc);
      FALLTHROUGH();
    స్థితి SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(దత్తాంశం, sshc);
      FALLTHROUGH();
    స్థితి SSH_SESSION_FREE:
      ఫలితం = myssh_లోపల_SESSION_FREE(దత్తాంశం, sshc);
      ఆపు;
    స్థితి SSH_QUIT:
    అప్రమేయ:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
      ఆపు;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } అయితే((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  ఒకవేళ(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *ఖండం = TRUE;
  }
  ఒకవేళ(!ఫలితం && (sshc->state == SSH_STOP))
    ఫలితం = sshc->actualcode;
  CURL_TRC_SSH(దత్తాంశం, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (పూర్ణాంకం)ఫలితం, *ఖండం);
  తిరిగి ఫలితం;
}

/* called repeatedly until done from multi.c */
స్థిర CURLcode myssh_multi_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                                      bool *పూర్తి)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  నిర్మాణం SSHPROTO *sshp = Curl_meta_get(దత్తాంశం, CURL_META_SSH_EASY);
  bool ఖండం;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode ఫలితం;

  ఒకవేళ(!sshc || !sshp)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = myssh_statemachine(దత్తాంశం, sshc, sshp, &ఖండం);
  *పూర్తి = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, ఖండం);

  తిరిగి ఫలితం;
}

స్థిర CURLcode myssh_ఖండం_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ssh_conn *sshc,
                                      నిర్మాణం SSHPROTO *sshp,
                                      bool disconnect)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  CURLcode ఫలితం = CURLE_OK;

  అయితే((sshc->state != SSH_STOP) && !ఫలితం) {
    bool ఖండం;
    timediff_t ఎడమ_ms = 1000;

    ఫలితం = myssh_statemachine(దత్తాంశం, sshc, sshp, &ఖండం);
    ఒకవేళ(ఫలితం)
      ఆపు;

    ఒకవేళ(!disconnect) {
      ఫలితం = Curl_pgrsCheck(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        ఆపు;

      ఎడమ_ms = Curl_timeleft_ms(దత్తాంశం);
      ఒకవేళ(ఎడమ_ms < 0) {
        failf(దత్తాంశం, "Operation timed out");
        తిరిగి CURLE_OPERATION_TIMEDOUT;
      }
    }

    ఒకవేళ(ఖండం) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (శూన్య)SOCKET_READABLE(fd_read,
                            (!ఎడమ_ms || ఎడమ_ms > 1000) ? 1000 : ఎడమ_ms);
    }
  }

  తిరిగి ఫలితం;
}

స్థిర శూన్య myssh_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం SSHPROTO *sshp = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_safefree(sshp->మార్గం);
  curlx_free(sshp);
}

స్థిర శూన్య myssh_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం ssh_conn *sshc = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
స్థిర CURLcode myssh_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం connectdata *conn)
{
  నిర్మాణం SSHPROTO *sshp;
  నిర్మాణం ssh_conn *sshc;

  sshc = curlx_calloc(1, పరిమాణం(*sshc));
  ఒకవేళ(!sshc)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  curlx_dyn_init(&sshc->readdir_buf, CURL_మార్గం_MAX * 2);
  sshc->initialized = TRUE;
  ఒకవేళ(Curl_conn_meta_సమితి(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  sshp = curlx_calloc(1, పరిమాణం(*sshp));
  ఒకవేళ(!sshp ||
     Curl_meta_సమితి(దత్తాంశం, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి Curl_ssh_అమరిక_pkey(దత్తాంశం, sshc);
}

స్థిర Curl_recv scp_recv, sftp_recv;
స్థిర Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
స్థిర CURLcode myssh_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  నిర్మాణం SSHPROTO *ssh = Curl_meta_get(దత్తాంశం, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  పూర్ణాంకం rc;

  ఒకవేళ(!sshc || !ssh)
    తిరిగి CURLE_FAILED_INIT;

  CURL_TRC_SSH(దత్తాంశం, "myssh_connect");
  ఒకవేళ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  లేకపోతే {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  ఒకవేళ(!sshc->ssh_session) {
    failf(దత్తాంశం, "Failure initializing ssh session");
    తిరిగి CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_సమితి(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  ఒకవేళ(rc != SSH_OK) {
    failf(దత్తాంశం, "Could not set remote host");
    తిరిగి CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, ఖాళీ);
  ఒకవేళ(rc != SSH_OK) {
    infof(దత్తాంశం, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(దత్తాంశం, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_సమితి(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  ఒకవేళ(rc != SSH_OK) {
    failf(దత్తాంశం, "Could not set socket");
    తిరిగి CURLE_FAILED_INIT;
  }

  ఒకవేళ(Curl_creds_has_user(conn->creds)) {
    infof(దత్తాంశం, "User: %s", conn->creds->user);
    rc = ssh_options_సమితి(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    ఒకవేళ(rc != SSH_OK) {
      failf(దత్తాంశం, "Could not set user");
      తిరిగి CURLE_FAILED_INIT;
    }
  }

  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS)) {
    infof(దత్తాంశం, "Known hosts: %s",
          CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_సమితి(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS));
    ఒకవేళ(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_సమితి(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS));
    ఒకవేళ(rc != SSH_OK) {
      failf(దత్తాంశం, "Could not set known hosts file path");
      తిరిగి CURLE_FAILED_INIT;
    }
  }

  ఒకవేళ(conn->origin->పోర్టు) {
    rc = ssh_options_సమితి(sshc->ssh_session, SSH_OPTIONS_పోర్టు,
                         &conn->origin->పోర్టు);
    ఒకవేళ(rc != SSH_OK) {
      failf(దత్తాంశం, "Could not set remote port");
      తిరిగి CURLE_FAILED_INIT;
    }
  }

  ఒకవేళ(దత్తాంశం->సమితి.ssh_compression) {
    rc = ssh_options_సమితి(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    ఒకవేళ(rc != SSH_OK) {
      failf(దత్తాంశం, "Could not set compression");
      తిరిగి CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = ఖాళీ;
  sshc->pubkey = ఖాళీ;

  ఒకవేళ(sshc->pub_కీలకం) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_కీలకం, &sshc->pubkey);
    ఒకవేళ(rc != SSH_OK) {
      failf(దత్తాంశం, "Could not load public key file");
      తిరిగి CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_INIT);

  ఫలితం = myssh_multi_statemach(దత్తాంశం, పూర్తి);

  తిరిగి ఫలితం;
}

/* called from multi.c while DOing */
స్థిర CURLcode scp_doing(నిర్మాణం Curl_easy *దత్తాంశం, bool *dophase_పూర్తి)
{
  CURLcode ఫలితం;

  ఫలితం = myssh_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  ఒకవేళ(*dophase_పూర్తి) {
    CURL_TRC_SSH(దత్తాంశం, "DO phase is complete");
  }
  తిరిగి ఫలితం;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

స్థిర CURLcode scp_perform(నిర్మాణం Curl_easy *దత్తాంశం,
                            bool *connected, bool *dophase_పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(దత్తాంశం, "DO phase starts");

  *dophase_పూర్తి = FALSE;        /* not done yet */
  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_TRANS_INIT);

  ఫలితం = myssh_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  *connected = Curl_conn_is_connected(దత్తాంశం->conn, FIRSTSOCKET);

  ఒకవేళ(*dophase_పూర్తి) {
    CURL_TRC_SSH(దత్తాంశం, "DO phase is complete");
  }

  తిరిగి ఫలితం;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
స్థిర CURLcode scp_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               bool dead_connection)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  నిర్మాణం SSHPROTO *sshp = Curl_meta_get(దత్తాంశం, CURL_META_SSH_EASY);
  (శూన్య)dead_connection;

  ఒకవేళ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_DISCONNECT);
    ఫలితం = myssh_ఖండం_statemach(దత్తాంశం, sshc, sshp, TRUE);
  }

  తిరిగి ఫలితం;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
స్థిర CURLcode myssh_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం ssh_conn *sshc,
                           CURLcode స్థితి_2)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం SSHPROTO *sshp = Curl_meta_get(దత్తాంశం, CURL_META_SSH_EASY);

  ఒకవేళ(!స్థితి_2 && sshp) {
    /* run the state-machine */
    ఫలితం = myssh_ఖండం_statemach(దత్తాంశం, sshc, sshp, FALSE);
  }
  లేకపోతే
    ఫలితం = స్థితి_2;

  ఒకవేళ(Curl_pgrs_పూర్తి(దత్తాంశం, Curl_pgrs_now(దత్తాంశం)))
    తిరిగి CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(దత్తాంశం);
  తిరిగి ఫలితం;
}

స్థిర CURLcode scp_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2,
                         bool premature)
{
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SSH_CONN);
  (శూన్య)premature;

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(!స్థితి_2)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_పూర్తి);

  తిరిగి myssh_పూర్తి(దత్తాంశం, sshc, స్థితి_2);
}

స్థిర CURLcode scp_send(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                         స్థిరరాశి uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  పూర్ణాంకం rc;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (శూన్య)sockindex; /* we only support SCP on the fixed known primary socket */
  (శూన్య)eos;
  *pnwritten = 0;

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#ఒకవేళ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  ఒకవేళ(rc == SSH_AGAIN)
    తిరిగి CURLE_AGAIN;
  లేకపోతే
#అయితే_ముగింపు
  ఒకవేళ(rc != SSH_OK)
    తిరిగి CURLE_SSH;

  *pnwritten = len;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode scp_recv(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                         అక్షరం *mem, size_t len, size_t *pnread)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  పూర్ణాంకం nread;

  (శూన్య)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  ఒకవేళ(nread == SSH_ERROR)
    తిరిగి CURLE_SSH;
#ఒకవేళ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  ఒకవేళ(nread == SSH_AGAIN)
    తిరిగి CURLE_AGAIN;
#అయితే_ముగింపు
  *pnread = (size_t)nread;
  తిరిగి CURLE_OK;
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

స్థిర CURLcode sftp_perform(నిర్మాణం Curl_easy *దత్తాంశం,
                             bool *connected,
                             bool *dophase_పూర్తి)
{
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SSH_CONN);
  CURLcode ఫలితం = CURLE_OK;

  CURL_TRC_SSH(దత్తాంశం, "DO phase starts");

  *dophase_పూర్తి = FALSE; /* not done yet */
  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  ఫలితం = myssh_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  *connected = Curl_conn_is_connected(దత్తాంశం->conn, FIRSTSOCKET);

  ఒకవేళ(*dophase_పూర్తి) {
    CURL_TRC_SSH(దత్తాంశం, "DO phase is complete");
  }

  తిరిగి ఫలితం;
}

/* called from multi.c while DOing */
స్థిర CURLcode sftp_doing(నిర్మాణం Curl_easy *దత్తాంశం,
                           bool *dophase_పూర్తి)
{
  CURLcode ఫలితం = myssh_multi_statemach(దత్తాంశం, dophase_పూర్తి);
  ఒకవేళ(*dophase_పూర్తి) {
    CURL_TRC_SSH(దత్తాంశం, "DO phase is complete");
  }
  తిరిగి ఫలితం;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
స్థిర CURLcode sftp_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn,
                                bool dead_connection)
{
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  నిర్మాణం SSHPROTO *sshp = Curl_meta_get(దత్తాంశం, CURL_META_SSH_EASY);
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)dead_connection;

  CURL_TRC_SSH(దత్తాంశం, "DISCONNECT starts now");

  ఒకవేళ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_SHUTDOWN);
    ఫలితం = myssh_ఖండం_statemach(దత్తాంశం, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(దత్తాంశం, "DISCONNECT is done");
  తిరిగి ఫలితం;
}

స్థిర CURLcode sftp_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2,
                          bool premature)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(!స్థితి_2) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    ఒకవేళ(!premature && దత్తాంశం->సమితి.postquote && !conn->బిట్లు.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
  }
  తిరిగి myssh_పూర్తి(దత్తాంశం, sshc, స్థితి_2);
}

/* return number of sent bytes */
స్థిర CURLcode sftp_send(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                          స్థిరరాశి uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (శూన్య)sockindex;
  (శూన్య)eos;
  *pnwritten = 0;

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

#ఒకవేళ LIBSSH_సంస్కరణ_INT > SSH_సంస్కరణ_INT(0, 11, 0)
  ఎంపిక(sshc->sftp_send_state) {
  స్థితి 0:
    sftp_file_సమితి_nonblocking(sshc->sftp_file);
    ఒకవేళ(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      తిరిగి CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  స్థితి 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    ఒకవేళ(nwrite == SSH_AGAIN)
      తిరిగి CURLE_AGAIN;
    లేకపోతే ఒకవేళ(nwrite < 0)
      తిరిగి CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    తిరిగి CURLE_OK;
  అప్రమేయ:
    /* we never reach here */
    తిరిగి CURLE_SEND_ERROR;
  }
#లేకపోతే
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  ఒకవేళ(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#ఒకవేళ 0 /* not returned by libssh on write */
  ఒకవేళ(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  లేకపోతే
#అయితే_ముగింపు
  ఒకవేళ(nwrite < 0)
    తిరిగి CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  తిరిగి CURLE_OK;
#అయితే_ముగింపు
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
స్థిర CURLcode sftp_recv(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                          అక్షరం *mem, size_t len, size_t *pnread)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (శూన్య)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_కొలత);
  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  ఎంపిక(sshc->sftp_recv_state) {
  స్థితి 0:
#ఒకవేళ LIBSSH_సంస్కరణ_INT > SSH_సంస్కరణ_INT(0, 11, 0)
    ఒకవేళ(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      తిరిగి CURLE_RECV_ERROR;
    }
#లేకపోతే
    sshc->sftp_file_సూచీ =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    ఒకవేళ(sshc->sftp_file_సూచీ < 0)
      తిరిగి CURLE_RECV_ERROR;
#అయితే_ముగింపు

    FALLTHROUGH();
  స్థితి 1:
    sshc->sftp_recv_state = 1;

#ఒకవేళ LIBSSH_సంస్కరణ_INT > SSH_సంస్కరణ_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#లేకపోతే
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_సూచీ);
#అయితే_ముగింపు

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    ఒకవేళ(nread == SSH_AGAIN)
      తిరిగి CURLE_AGAIN;
    లేకపోతే ఒకవేళ(nread < 0)
      తిరిగి CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    తిరిగి CURLE_OK;

  అప్రమేయ:
    /* we never reach here */
    తిరిగి CURLE_RECV_ERROR;
  }
}

స్థిర CURLcode myssh_do_it(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం;
  bool connected = FALSE;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *పూర్తి = FALSE;                /* default to false */
  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  దత్తాంశం->req.కొలత = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->రెండవ_Create_Dirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(దత్తాంశం);

  ఒకవేళ(conn->scheme->protocol & CURLPROTO_SCP)
    ఫలితం = scp_perform(దత్తాంశం, &connected, పూర్తి);
  లేకపోతే
    ఫలితం = sftp_perform(దత్తాంశం, &connected, పూర్తి);

  తిరిగి ఫలితం;
}

CURLcode Curl_ssh_init(శూన్య)
{
  ఒకవేళ(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    తిరిగి CURLE_FAILED_INIT;
  }
  తిరిగి CURLE_OK;
}

శూన్య Curl_ssh_cleanup(శూన్య)
{
  (శూన్య)ssh_finalize();
}

శూన్య Curl_ssh_సంస్కరణ(అక్షరం *బఫరు, size_t buflen)
{
  (శూన్య)curl_msnprintf(బఫరు, buflen, "libssh/%s", ssh_సంస్కరణ(0));
}

/*
 * SCP.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_scp = {
  myssh_అమరిక_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_పూర్తి,                             /* done */
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
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_sftp = {
  myssh_అమరిక_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_పూర్తి,                            /* done */
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

#అయితే_ముగింపు /* USE_LIBSSH */
