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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_LIBSSH

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "vssh/ssh.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "parsedate.h"          /* for the week day and month names */
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "vssh/vssh.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  କର {                          \
    ଯଦି(x) {                     \
      ssh_string_free_char(x);  \
      x = ଖାଲି;                 \
    }                           \
  } ଯେତେବେଳେ(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

ସ୍ଥିର CURLcode sftp_error_to_CURLE(ପୂର୍ଣ୍ଣସଂଖ୍ୟା err)
{
  ବାଛ(err) {
  ସ୍ଥିତି SSH_FX_OK:
    ଫେରନ୍ତୁ CURLE_OK;

  ସ୍ଥିତି SSH_FX_NO_SUCH_FILE:
  ସ୍ଥିତି SSH_FX_NO_SUCH_PATH:
    ଫେରନ୍ତୁ CURLE_REMOTE_FILE_NOT_FOUND;

  ସ୍ଥିତି SSH_FX_PERMISSION_DENIED:
  ସ୍ଥିତି SSH_FX_WRITE_PROTECT:
    ଫେରନ୍ତୁ CURLE_REMOTE_ACCESS_DENIED;

  ସ୍ଥିତି SSH_FX_FILE_ALREADY_EXISTS:
    ଫେରନ୍ତୁ CURLE_REMOTE_FILE_EXISTS;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ CURLE_SSH;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_is_known(ସଂରଚନା Curl_easy *data, ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ssh_ଚାବି pubkey;
  unsigned ଅକ୍ଷର *hash_sha256 = ଖାଲି;
  size_t hlen_sha256;
  unsigned ଅକ୍ଷର *hash_md5 = ଖାଲି;
  size_t hlen_md5;
  ଅକ୍ଷର *found_base64 = ଖାଲି;
  ଅକ୍ଷର *known_base64 = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା vstate;
  ଗଣନା curl_khmatch keymatch;
  ସଂରଚନା curl_khkey foundkey;
  ସଂରଚନା curl_khkey *knownkeyp = ଖାଲି;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  ସଂରଚନା ssh_knownhosts_entry *knownhostsentry = ଖାଲି;
  ସଂରଚନା curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  ଯଦି(rc != SSH_OK)
    ଫେରନ୍ତୁ rc;

  ଯଦି(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ଚାବି_SHA256)) {
    ଅଚଳ ଅକ୍ଷର *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ଚାବି_SHA256);
    ଅକ୍ଷର *fingerprint_b64 = ଖାଲି;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    ଯଦି(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      ଯାଅ cleanup;
    }

    ଯଦି(curlx_base64_encode((ଅଚଳ uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      ଯାଅ cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    ଯେତେବେଳେ((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    ଯେତେବେଳେ((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    ଯଦି((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      ଯାଅ cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    ଯାଅ cleanup;
  }

  ଯଦି(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ଚାବି_MD5)) {
    ଅଚଳ ଅକ୍ଷର *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ଚାବି_MD5);
    ଅକ୍ଷର md5buffer[33];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    ଯଦି(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      ଯାଅ cleanup;
    }

    ପାଇଁ(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    ଯଦି(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      ଯାଅ cleanup;
    }

    rc = SSH_OK;
    ଯାଅ cleanup;
  }

  ଯଦି(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    ଯଦି(knownhostsentry) {
      ଯଦି(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        ଯଦି(rc != SSH_OK) {
          ଯାଅ cleanup;
        }
        knownkey.ଚାବି = known_base64;
        knownkey.len = strlen(known_base64);

        ବାଛ(ssh_ଚାବି_type(knownhostsentry->publickey)) {
        ସ୍ଥିତି SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି SSH_KEYTYPE_ECDSA:
        ସ୍ଥିତି SSH_KEYTYPE_ECDSA_P256:
        ସ୍ଥିତି SSH_KEYTYPE_ECDSA_P384:
        ସ୍ଥିତି SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
          rc = SSH_ERROR;
          ଯାଅ cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    ବାଛ(vstate) {
    ସ୍ଥିତି SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_KNOWN_HOSTS_NOT_FOUND:
    ସ୍ଥିତି SSH_KNOWN_HOSTS_UNKNOWN:
    ସ୍ଥିତି SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      keymatch = CURLKHMATCH_MISMATCH;
      ଭାଙ୍ଗ;
    }

    ଯଦି(func) { /* use callback to determine action */
      ସଂରଚନା Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      ଯଦି(rc != SSH_OK)
        ଯାଅ cleanup;

      foundkey.ଚାବି = found_base64;
      foundkey.len = strlen(found_base64);

      ବାଛ(ssh_ଚାବି_type(pubkey)) {
      ସ୍ଥିତି SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSH_KEYTYPE_ECDSA:
      ସ୍ଥିତି SSH_KEYTYPE_ECDSA_P256:
      ସ୍ଥିତି SSH_KEYTYPE_ECDSA_P384:
      ସ୍ଥିତି SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        rc = SSH_ERROR;
        ଯାଅ cleanup;
      }

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      ବାଛ(rc) {
      ସ୍ଥିତି CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        ଯଦି(rc != SSH_OK) {
          ଯାଅ cleanup;
        }
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURLKHSTAT_FINE:
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* REJECT/DEFER */
        rc = SSH_ERROR;
        ଯାଅ cleanup;
      }
    }
    ଅନ୍ୟଥା {
      ଯଦି(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        ଯାଅ cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  ଯଦି(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    ମୁକ୍ତ(found_base64); /* allocated by libssh, deallocate with system free */
  }
  ଯଦି(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    ମୁକ୍ତ(known_base64); /* allocated by libssh, deallocate with system free */
  }
  ଯଦି(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  ଯଦି(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_ଚାବି_free(pubkey);
  ଯଦି(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_to_ERROR(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  ଫେରନ୍ତୁ SSH_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_to_SFTP_CLOSE(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  ଫେରନ୍ତୁ SSH_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_to_PASSWD_AUTH(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    ଫେରନ୍ତୁ SSH_OK;
  }
  ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_to_ଚାବି_AUTH(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_ଚାବି_INIT);
    ଫେରନ୍ତୁ SSH_OK;
  }
  ଫେରନ୍ତୁ myssh_to_PASSWD_AUTH(data, sshc);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_to_GSSAPI_AUTH(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    ଫେରନ୍ତୁ SSH_OK;
  }
  ଫେରନ୍ତୁ myssh_to_ଚାବି_AUTH(data, sshc);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_READDIR_INIT(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ssh_conn *sshc,
                                      ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  ଯଦି(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  ଯଦି(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ଫେରନ୍ତୁ myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_READDIR(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ssh_conn *sshc,
                                 ସଂରଚନା SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  ଯଦି(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  ଯଦି(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    ଯଦି(data->set.list_only) {
      ଅକ୍ଷର *tmp_ଧାଡ଼ି;

      tmp_ଧାଡ଼ି = curl_maprintf("%s\n", sshc->readdir_filename);
      ଯଦି(!tmp_ଧାଡ଼ି) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        ଫେରନ୍ତୁ SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmp_ଧାଡ଼ି, sshc->readdir_len + 1);
      curlx_free(tmp_ଧାଡ଼ି);

      ଯଦି(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        ଫେରନ୍ତୁ SSH_NO_ERROR;
      }
    }
    ଅନ୍ୟଥା {
      ଯଦି(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        ଫେରନ୍ତୁ SSH_ERROR;
      }

      ଯଦି((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        ଯଦି(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          ଫେରନ୍ତୁ SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        ଫେରନ୍ତୁ SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      ଫେରନ୍ତୁ SSH_NO_ERROR;
    }
  }
  ଅନ୍ୟଥା ଯଦି(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  ଅନ୍ୟଥା {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ଫେରନ୍ତୁ myssh_to_SFTP_CLOSE(data, sshc);
  }
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_READDIR_LINK(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  ଯଦି(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    ଫେରନ୍ତୁ myssh_to_SFTP_CLOSE(data, sshc);
  }

  ଯଦି(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    ଯଦି(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    ଅନ୍ୟଥା
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = ଖାଲି;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  ଅନ୍ୟଥା {
    sshc->readdir_len = strlen(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  ଯଦି(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    ଫେରନ୍ତୁ SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = ଖାଲି;
  sshc->readdir_filename = ଖାଲି;
  sshc->readdir_longentry = ଖାଲି;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_READDIR_BOTTOM(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ssh_conn *sshc)
{
  CURLcode result;

  ଯଦି(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  ଅନ୍ୟଥା
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = ଖାଲି;

  ଯଦି(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_READDIR_DONE(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = ଖାଲି;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ଶୂନ୍ୟ myssh_quote_error(ସଂରଚନା Curl_easy *data, ସଂରଚନା ssh_conn *sshc,
                              ଅଚଳ ଅକ୍ଷର *cmd)
{
  ଯଦି(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE_STATVFS(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  ଯଦି(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    ଫେରନ୍ତୁ SSH_OK;
  }
  ଅନ୍ୟଥା ଯଦି(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#ନଚେତଯଦି defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#ଅନ୍ୟଥା
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    ଅକ୍ଷର *tmp = curl_maprintf("statvfs:\n"
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

    ଯଦି(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    ଯଦି(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    ଯଦି(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ SSH_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_auth_interactive(ସଂରଚନା connectdata *conn,
                                  ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nprompts;

restart:
  ବାଛ(sshc->kbd_state) {
  ସ୍ଥିତି 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, ଖାଲି, ଖାଲି);
    ଯଦି(rc == SSH_AUTH_AGAIN)
      ଫେରନ୍ତୁ SSH_AGAIN;

    ଯଦି(rc != SSH_AUTH_INFO)
      ଫେରନ୍ତୁ SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    ଯଦି(nprompts != 1)
      ଫେରନ୍ତୁ SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    ଯଦି(rc < 0)
      ଫେରନ୍ତୁ SSH_ERROR;

    FALLTHROUGH();
  ସ୍ଥିତି 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ଖାଲି, ଖାଲି);
    ଯଦି(rc == SSH_AUTH_AGAIN)
      ଫେରନ୍ତୁ SSH_AGAIN;
    ଅନ୍ୟଥା ଯଦି(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ଅନ୍ୟଥା ଯଦି(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      ଯଦି(nprompts)
        ଫେରନ୍ତୁ SSH_ERROR;

      sshc->kbd_state = 2;
      ଯାଅ restart;
    }
    ଅନ୍ୟଥା
      rc = SSH_ERROR;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ଖାଲି, ଖାଲି);
    ଯଦି(rc == SSH_AUTH_AGAIN)
      ଫେରନ୍ତୁ SSH_AGAIN;
    ଅନ୍ୟଥା ଯଦି(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ଅନ୍ୟଥା
      rc = SSH_ERROR;

    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ SSH_ERROR;
  }

  sshc->kbd_state = 0;
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଶୂନ୍ୟ myssh_state_init(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#ଯଦି 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

ସ୍ଥିର ଶୂନ୍ୟ myssh_block2waitfor(ସଂରଚନା connectdata *conn,
                                ସଂରଚନା ssh_conn *sshc,
                                bool block)
{
  (ଶୂନ୍ୟ)conn;
  ଯଦି(block) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  ଅନ୍ୟଥା
    sshc->waitfor = 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_S_STARTUP(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ssh_conn *sshc)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  ଯଦି(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  ଅନ୍ୟଥା ଯଦି(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_HOSTKEY);

  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_AUTHLIST(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, ଖାଲି);
  ଯଦି(rc == SSH_AUTH_AGAIN)
    ଫେରନ୍ତୁ SSH_AGAIN;

  ଯଦି(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ଫେରନ୍ତୁ rc;
  }
  ଅନ୍ୟଥା ଯଦି(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    ଫେରନ୍ତୁ rc;
  }

  sshc->auth_methods =
    (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ssh_userauth_list(sshc->ssh_session, ଖାଲି);
  ଯଦି(sshc->auth_methods)
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
  ଯଦି((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_ଚାବି && sshc->priv_ଚାବି[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  ଅନ୍ୟଥା ଯଦି(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  ଅନ୍ୟଥା ଯଦି(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_ଚାବି_INIT);
  }
  ଅନ୍ୟଥା ଯଦି(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  ଅନ୍ୟଥା {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_AUTH_PKEY_INIT(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ଯଦି(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    ଫେରନ୍ତୁ rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  ଯଦି(sshc->priv_ଚାବି && sshc->priv_ଚାବି[0]) {
    ଯଦି(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, ଖାଲି, sshc->pubkey);
      ଯଦି(rc == SSH_AUTH_AGAIN)
        ଫେରନ୍ତୁ SSH_AGAIN;

      ଯଦି(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        ଫେରନ୍ତୁ rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_ଚାବି,
                                     sshc->passphrase, ଖାଲି,
                                     ଖାଲି, &sshc->privkey);
    ଯଦି(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_ଚାବି);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      ଫେରନ୍ତୁ rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  ଅନ୍ୟଥା {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, ଖାଲି,
                                     sshc->passphrase);
    ଯଦି(rc == SSH_AUTH_AGAIN)
      ଫେରନ୍ତୁ SSH_AGAIN;

    ଯଦି(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      ଫେରନ୍ତୁ rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_AUTH_PKEY(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = ssh_userauth_publickey(sshc->ssh_session, ଖାଲି, sshc->privkey);
  ଯଦି(rc == SSH_AUTH_AGAIN)
    ଫେରନ୍ତୁ SSH_AGAIN;
  ଅନ୍ୟଥା ଯଦି(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ଫେରନ୍ତୁ SSH_OK;
  }
  ଅନ୍ୟଥା {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    ଫେରନ୍ତୁ myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_AUTH_GSSAPI(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ଯଦି(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    ଫେରନ୍ତୁ myssh_to_ଚାବି_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  ଯଦି(rc == SSH_AUTH_AGAIN)
    ଫେରନ୍ତୁ SSH_AGAIN;

  ଯଦି(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ଫେରନ୍ତୁ SSH_OK;
  }

  ଫେରନ୍ତୁ myssh_to_ଚାବି_AUTH(data, sshc);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_AUTH_ଚାବି_INIT(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_ଚାବି);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  ଫେରନ୍ତୁ myssh_to_PASSWD_AUTH(data, sshc);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_AUTH_ଚାବି(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = myssh_auth_interactive(data->conn, sshc);
  ଯଦି(rc == SSH_AGAIN)
    ଫେରନ୍ତୁ rc;
  ଅନ୍ୟଥା ଯଦି(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ myssh_to_PASSWD_AUTH(data, sshc);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_AUTH_PASS_INIT(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_AUTH_PASS(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = ssh_userauth_password(sshc->ssh_session, ଖାଲି,
                                 Curl_creds_passwd(data->conn->creds));
  ଯଦି(rc == SSH_AUTH_AGAIN)
    ଫେରନ୍ତୁ SSH_AGAIN;
  ଅନ୍ୟଥା ଯଦି(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_AUTH_DONE(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ssh_conn *sshc)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ଯଦି(!sshc->authed) {
    failf(data, "Authentication failure");
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ଯଦି(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_UPLOAD_INIT(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ssh_conn *sshc,
                                ସଂରଚନା SSHPROTO *sshp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  ଯଦି(data->state.resume_from) {
    sftp_attributes attrs;

    ଯଦି(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      ଯଦି(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        ଯଦି(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          ଫେରନ୍ତୁ rc;
        }
        data->state.resume_from = size;
      }
      ଅନ୍ୟଥା {
        data->state.resume_from = 0;
      }
    }
  }

  ଯଦି(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  ଅନ୍ୟଥା ଯଦି(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  ଅନ୍ୟଥା
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  ଯଦି(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  ଯଦି(!sshc->sftp_file) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = sftp_get_error(sshc->sftp_session);

    ଯଦି((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ଫେରନ୍ତୁ rc;
    }
    ଅନ୍ୟଥା {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      ଫେରନ୍ତୁ rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  ଯଦି(data->state.resume_from > 0 && !data->set.remote_append) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    ଯଦି(data->set.seek_func) {
      ସଂରଚନା Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    ଯଦି(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      ଯଦି(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        ଫେରନ୍ତୁ rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      କର {
        ଅକ୍ଷର scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)ଆକାର(scratch)) ?
          ଆକାର(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        ଯଦି((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          ଫେରନ୍ତୁ rc;
        }
      } ଯେତେବେଳେ(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    ଯଦି(data->state.infilesize > 0) {
      ଯଦି(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    ଯଦି(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      ଫେରନ୍ତୁ rc;
    }
  }
  ଯଦି(data->state.infilesize > 0) {
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
#ଯଦି LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_DOWNLOAD_INIT(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc,
                                       ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  ଯଦି(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  ଯଦି(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    ଫେରନ୍ତୁ myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_DOWNLOAD_STAT(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc)
{
  curl_off_t size;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  ଯଦି(!attrs ||
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
    ଯଦି(attrs)
      sftp_attributes_free(attrs);
  }
  ଅନ୍ୟଥା {
    size = attrs->size;

    sftp_attributes_free(attrs);

    ଯଦି(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    ଯଦି(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      ଯଦି(result)
        ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      ଯଦି(rc)
        ଫେରନ୍ତୁ myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  ଯଦି(data->state.resume_from) {
    ଯଦି(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ଯଦି(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    ଅନ୍ୟଥା {
      ଯଦି(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    ଯଦି(rc)
      ଫେରନ୍ତୁ myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  ଯଦି(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    ଫେରନ୍ତୁ rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_CLOSE(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ssh_conn *sshc,
                               ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  ଯଦି(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ଖାଲି;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  ଯଦି(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_STOP);
  }
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_SHUTDOWN(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#ଯଦି LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  ଯଦି(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ଖାଲି;
  }

  ଯଦି(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ଖାଲି;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_INIT(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  ଯଦି(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  ଯଦି(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_REALPATH(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  ଯଦି(!sshc->homedir)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  ଯଦି(!data->state.most_recent_ftp_entrypath)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE_INIT(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc,
                                    ସଂରଚନା SSHPROTO *sshp)
{
  CURLcode result;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  ଯଦି(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  ଅନ୍ୟଥା ଯଦି(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_POSTQUOTE_INIT(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_STOP);
  }
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା quote_error(ସଂରଚନା Curl_easy *data,
                       ସଂରଚନା ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ssh_conn *sshc,
                               ସଂରଚନା SSHPROTO *sshp)
{
  ଅଚଳ ଅକ୍ଷର *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  ଅଚଳ ଅକ୍ଷର *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ଯଦି(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ଯଦି(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    ଅକ୍ଷର *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    ଯଦି(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ଫେରନ୍ତୁ SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    ଯଦି(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    ଅନ୍ୟଥା
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ଯଦି(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ଯଦି(result) {
    ଯଦି(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    ଅନ୍ୟଥା
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  ଯଦି(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ଯଦି(result) {
      ଯଦି(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      ଅନ୍ୟଥା
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ଫେରନ୍ତୁ SSH_NO_ERROR;
    }
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    sshc->quote_attrs = ଖାଲି;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  ଯଦି(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ଯଦି(result) {
      ଯଦି(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      ଅନ୍ୟଥା
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ଫେରନ୍ତୁ SSH_NO_ERROR;
    }
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "mkdir ", 6)) {
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ଯଦି(result) {
      ଯଦି(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      ଅନ୍ୟଥା
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      ଫେରନ୍ତୁ SSH_NO_ERROR;
    }
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "rm ", 3)) {
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "statvfs ", 8)) {
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_NEXT_QUOTE(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  ଯଦି(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ଅନ୍ୟଥା {
    ଯଦି(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    ଅନ୍ୟଥା {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE_STAT(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc)
{
  ଅଚଳ ଅକ୍ଷର *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ଯଦି(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  ଯଦି(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  ଯଦି(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, ଖାଲି);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  ଯଦି(!strncmp(cmd, "chgrp", 5)) {
    ଅଚଳ ଅକ୍ଷର *p = sshc->quote_path1;
    curl_off_t gid;
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &gid, UINT_MAX)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, ଖାଲି);
      ଫେରନ୍ତୁ SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ଅଚଳ ଅକ୍ଷର *p = sshc->quote_path1;
    ଯଦି(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, ଖାଲି);
      ଫେରନ୍ତୁ SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "chown", 5)) {
    ଅଚଳ ଅକ୍ଷର *p = sshc->quote_path1;
    curl_off_t uid;
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &uid, UINT_MAX)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, ଖାଲି);
      ଫେରନ୍ତୁ SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    ଯଦି(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ଯଦି SIZEOF_TIME_T > 4
    ଅନ୍ୟଥା ଯଦି(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    ଯଦି(fail) {
      myssh_quote_error(data, sshc, ଖାଲି);
      ଫେରନ୍ତୁ SSH_NO_ERROR;
    }
    ଯଦି(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    ଅନ୍ୟଥା /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ଶୂନ୍ୟ conn_forget_socket(ସଂରଚନା Curl_easy *data, int8_t sockindex)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ଯଦି(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    ସଂରଚନା Curl_cfilter *cf = conn->cfilter[sockindex];
    ଯଦି(cf)
      (ଶୂନ୍ୟ)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, ଖାଲି);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ myssh_SESSION_DISCONNECT(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ଯଦି(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ଖାଲି;
  }

  ଯଦି(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ଖାଲି;
  }
  ଯଦି(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ଖାଲି;
  }

  ssh_disconnect(sshc->ssh_session);
  ଯଦି(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_SSH_SCP_DOWNLOAD(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ssh_conn *sshc)
{
  uint64_t bytecount;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = ssh_scp_pull_request(sshc->scp_session);
  ଯଦି(rc != SSH_SCP_REQUEST_NEWFILE) {
    ଅଚଳ ଅକ୍ଷର *err_ସନ୍ଦେଶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ସନ୍ଦେଶ);
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  ଯଦି(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_TRANS_INIT(ସଂରଚନା Curl_easy *data, ସଂରଚନା ssh_conn *sshc,
                               ସଂରଚନା SSHPROTO *sshp)
{
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  ଯଦି(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    ଫେରନ୍ତୁ 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  ଯଦି(data->state.upload) {
    ଯଦି(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ଅନ୍ୟଥା {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  ଯଦି(!sshc->scp_session) {
    ଅଚଳ ଅକ୍ଷର *err_ସନ୍ଦେଶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ସନ୍ଦେଶ);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ଶୂନ୍ୟ sshc_cleanup(ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->initialized) {
    ଯଦି(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = ଖାଲି;
    }
    ଯଦି(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = ଖାଲି;
    }
    ଯଦି(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = ଖାଲି;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    ଯଦି(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = ଖାଲି;
    }
    ଯଦି(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = ଖାଲି;
    }
    ଯଦି(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = ଖାଲି;
    }
    ଯଦି(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = ଖାଲି;
    }
    ଯଦି(sshc->privkey) {
      ssh_ଚାବି_free(sshc->privkey);
      sshc->privkey = ଖାଲି;
    }
    ଯଦି(sshc->pubkey) {
      ssh_ଚାବି_free(sshc->pubkey);
      sshc->pubkey = ଖାଲି;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_ଚାବି);
    curlx_safefree(sshc->priv_ଚାବି);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE_SETSTAT(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  ଯଦି(rc == SSH_AGAIN)
    ଫେରନ୍ତୁ rc;
  ଯଦି(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    ଫେରନ୍ତୁ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE_SYMLINK(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  ଯଦି(rc == SSH_AGAIN)
    ଫେରନ୍ତୁ rc;
  ଯଦି(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    ଫେରନ୍ତୁ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE_MKDIR(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  ଯଦି(rc == SSH_AGAIN)
    ଫେରନ୍ତୁ rc;
  ଯଦି(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    ଫେରନ୍ତୁ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE_RENAME(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  ଯଦି(rc == SSH_AGAIN)
    ଫେରନ୍ତୁ rc;
  ଯଦି(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    ଫେରନ୍ତୁ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE_RMDIR(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  ଯଦି(rc == SSH_AGAIN)
    ଫେରନ୍ତୁ rc;
  ଯଦି(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    ଫେରନ୍ତୁ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_QUOTE_UNLINK(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  ଯଦି(rc == SSH_AGAIN)
    ଫେରନ୍ତୁ rc;
  ଯଦି(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    ଫେରନ୍ତୁ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_GETINFO(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_FILETIME(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ssh_conn *sshc,
                                  ସଂରଚନା SSHPROTO *sshp)
{
  sftp_attributes attrs;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  ଯଦି(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_TRANS_INIT(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc,
                                    ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  ଯଦି(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ଅନ୍ୟଥା {
    size_t path_len = strlen(sshp->path);

    ଯଦି(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    ଅନ୍ୟଥା
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_CREATE_DIRS_INIT(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା ssh_conn *sshc,
                                          ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  ଯଦି(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_CREATE_DIRS(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ssh_conn *sshc,
                                     ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ଯଦି(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ଫେରନ୍ତୁ SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SFTP_CREATE_DIRS_MKDIR(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା ssh_conn *sshc,
                                           ସଂରଚନା SSHPROTO *sshp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  ଯଦି(rc == SSH_AGAIN)
    ଫେରନ୍ତୁ rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ଯଦି(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    ଯଦି((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      ଫେରନ୍ତୁ myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  ଫେରନ୍ତୁ rc;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SCP_UPLOAD_INIT(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc,
                                    ସଂରଚନା SSHPROTO *sshp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  ଯଦି(rc != SSH_OK) {
    ଅଚଳ ଅକ୍ଷର *err_ସନ୍ଦେଶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ସନ୍ଦେଶ);
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ଠେଲ_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)data->set.new_file_perms);

  ଯଦି(rc != SSH_OK) {
    ଅଚଳ ଅକ୍ଷର *err_ସନ୍ଦେଶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ସନ୍ଦେଶ);
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SCP_DOWNLOAD_INIT(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = ssh_scp_init(sshc->scp_session);
  ଯଦି(rc != SSH_OK) {
    ଅଚଳ ଅକ୍ଷର *err_ସନ୍ଦେଶ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_ସନ୍ଦେଶ);
    ଫେରନ୍ତୁ myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SCP_DONE(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SCP_SEND_EOF(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->scp_session) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = ssh_scp_close(sshc->scp_session);
    ଯଦି(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      ଫେରନ୍ତୁ SSH_AGAIN;
    }
    ଯଦି(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା myssh_in_SCP_CHANNEL_FREE(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ଖାଲି;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ଫେରନ୍ତୁ SSH_NO_ERROR;
}

ସ୍ଥିର CURLcode myssh_in_SESSION_FREE(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  memset(sshc, 0, ଆକାର(ସଂରଚନା ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
ସ୍ଥିର CURLcode myssh_statemachine(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ssh_conn *sshc,
                                   ସଂରଚନା SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  କର {

    ବାଛ(sshc->state) {
    ସ୍ଥିତି SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    ସ୍ଥିତି SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      ଯଦି(rc)
        ଭାଙ୍ଗ;
      FALLTHROUGH();
    ସ୍ଥିତି SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      ଯଦି(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        ଭାଙ୍ଗ;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    ସ୍ଥିତି SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_AUTH_ଚାବି_INIT:
      rc = myssh_in_AUTH_ଚାବି_INIT(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_AUTH_ଚାବି:
      rc = myssh_in_AUTH_ଚାବି(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      ଯଦି(rc)
        ଭାଙ୍ଗ;
      FALLTHROUGH();
    ସ୍ଥିତି SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    ସ୍ଥିତି SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    ସ୍ଥିତି SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSH_QUIT:
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      ଭାଙ୍ଗ;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } ଯେତେବେଳେ((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  ଯଦି(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  ଯଦି(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *block);
  ଫେରନ୍ତୁ result;
}

/* called repeatedly until done from multi.c */
ସ୍ଥିର CURLcode myssh_multi_statemach(ସଂରଚନା Curl_easy *data,
                                      bool *done)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ସଂରଚନା SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  ଯଦି(!sshc || !sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode myssh_block_statemach(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ssh_conn *sshc,
                                      ସଂରଚନା SSHPROTO *sshp,
                                      bool disconnect)
{
  ସଂରଚନା connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ଯେତେବେଳେ((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t ବାମ_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    ଯଦି(!disconnect) {
      result = Curl_pgrsCheck(data);
      ଯଦି(result)
        ଭାଙ୍ଗ;

      ବାମ_ms = Curl_timeleft_ms(data);
      ଯଦି(ବାମ_ms < 0) {
        failf(data, "Operation timed out");
        ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
      }
    }

    ଯଦି(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (ଶୂନ୍ୟ)SOCKET_READABLE(fd_read,
                            (!ବାମ_ms || ବାମ_ms > 1000) ? 1000 : ବାମ_ms);
    }
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ myssh_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା SSHPROTO *sshp = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

ସ୍ଥିର ଶୂନ୍ୟ myssh_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା ssh_conn *sshc = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
ସ୍ଥିର CURLcode myssh_setup_connection(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା SSHPROTO *sshp;
  ସଂରଚନା ssh_conn *sshc;

  sshc = curlx_calloc(1, ଆକାର(*sshc));
  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  ଯଦି(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, ଆକାର(*sshp));
  ଯଦି(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଫେରନ୍ତୁ Curl_ssh_setup_pkey(data, sshc);
}

ସ୍ଥିର Curl_recv scp_recv, sftp_recv;
ସ୍ଥିର Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ସ୍ଥିର CURLcode myssh_connect(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ସଂରଚନା SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!sshc || !ssh)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  ଯଦି(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ଅନ୍ୟଥା {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  ଯଦି(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  ଯଦି(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, ଖାଲି);
  ଯଦି(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  ଯଦି(rc != SSH_OK) {
    failf(data, "Could not set socket");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  ଯଦି(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    ଯଦି(rc != SSH_OK) {
      failf(data, "Could not set user");
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    }
  }

  ଯଦି(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    ଯଦି(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    ଯଦି(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    }
  }

  ଯଦି(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    ଯଦି(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    }
  }

  ଯଦି(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    ଯଦି(rc != SSH_OK) {
      failf(data, "Could not set compression");
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = ଖାଲି;
  sshc->pubkey = ଖାଲି;

  ଯଦି(sshc->pub_ଚାବି) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_ଚାବି, &sshc->pubkey);
    ଯଦି(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  ଫେରନ୍ତୁ result;
}

/* called from multi.c while DOing */
ସ୍ଥିର CURLcode scp_doing(ସଂରଚନା Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  ଯଦି(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ଫେରନ୍ତୁ result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ସ୍ଥିର CURLcode scp_perform(ସଂରଚନା Curl_easy *data,
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ଯଦି(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ଫେରନ୍ତୁ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ସ୍ଥିର CURLcode scp_disconnect(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ସଂରଚନା SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (ଶୂନ୍ୟ)dead_connection;

  ଯଦି(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  ଫେରନ୍ତୁ result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ସ୍ଥିର CURLcode myssh_done(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  ଯଦି(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
  ଅନ୍ୟଥା
    result = status;

  ଯଦି(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ଫେରନ୍ତୁ CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode scp_done(ସଂରଚନା Curl_easy *data, CURLcode status,
                         bool premature)
{
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (ଶୂନ୍ୟ)premature;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  ଫେରନ୍ତୁ myssh_done(data, sshc, status);
}

ସ୍ଥିର CURLcode scp_send(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                         ଅଚଳ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (ଶୂନ୍ୟ)sockindex; /* we only support SCP on the fixed known primary socket */
  (ଶୂନ୍ୟ)eos;
  *pnwritten = 0;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#ଯଦି 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  ଯଦି(rc == SSH_AGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;
  ଅନ୍ୟଥା
#endif
  ଯଦି(rc != SSH_OK)
    ଫେରନ୍ତୁ CURLE_SSH;

  *pnwritten = len;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode scp_recv(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                         ଅକ୍ଷର *mem, size_t len, size_t *pnread)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nread;

  (ଶୂନ୍ୟ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  ଯଦି(nread == SSH_ERROR)
    ଫେରନ୍ତୁ CURLE_SSH;
#ଯଦି 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  ଯଦି(nread == SSH_AGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  ଫେରନ୍ତୁ CURLE_OK;
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

ସ୍ଥିର CURLcode sftp_perform(ସଂରଚନା Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ଯଦି(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ଫେରନ୍ତୁ result;
}

/* called from multi.c while DOing */
ସ୍ଥିର CURLcode sftp_doing(ସଂରଚନା Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  ଯଦି(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ଫେରନ୍ତୁ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ସ୍ଥିର CURLcode sftp_disconnect(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn,
                                bool dead_connection)
{
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ସଂରଚନା SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  ଯଦି(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode sftp_done(ସଂରଚନା Curl_easy *data, CURLcode status,
                          bool premature)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    ଯଦି(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  ଫେରନ୍ତୁ myssh_done(data, sshc, status);
}

/* return number of sent bytes */
ସ୍ଥିର CURLcode sftp_send(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                          ଅଚଳ uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (ଶୂନ୍ୟ)sockindex;
  (ଶୂନ୍ୟ)eos;
  *pnwritten = 0;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

#ଯଦି LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  ବାଛ(sshc->sftp_send_state) {
  ସ୍ଥିତି 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    ଯଦି(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  ସ୍ଥିତି 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    ଯଦି(nwrite == SSH_AGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;
    ଅନ୍ୟଥା ଯଦି(nwrite < 0)
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    ଫେରନ୍ତୁ CURLE_OK;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* we never reach here */
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;
  }
#ଅନ୍ୟଥା
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  ଯଦି(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#ଯଦି 0 /* not returned by libssh on write */
  ଯଦି(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  ଅନ୍ୟଥା
#endif
  ଯଦି(nwrite < 0)
    ଫେରନ୍ତୁ CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  ଫେରନ୍ତୁ CURLE_OK;
#endif
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ସ୍ଥିର CURLcode sftp_recv(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                          ଅକ୍ଷର *mem, size_t len, size_t *pnread)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (ଶୂନ୍ୟ)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ବାଛ(sshc->sftp_recv_state) {
  ସ୍ଥିତି 0:
#ଯଦି LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    ଯଦି(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
    }
#ଅନ୍ୟଥା
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    ଯଦି(sshc->sftp_file_index < 0)
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  ସ୍ଥିତି 1:
    sshc->sftp_recv_state = 1;

#ଯଦି LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#ଅନ୍ୟଥା
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    ଯଦି(nread == SSH_AGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;
    ଅନ୍ୟଥା ଯଦି(nread < 0)
      ଫେରନ୍ତୁ CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    ଫେରନ୍ତୁ CURLE_OK;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* we never reach here */
    ଫେରନ୍ତୁ CURLE_RECV_ERROR;
  }
}

ସ୍ଥିର CURLcode myssh_do_it(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  ଯଦି(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  ଅନ୍ୟଥା
    result = sftp_perform(data, &connected, done);

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_ssh_init(ଶୂନ୍ୟ)
{
  ଯଦି(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_ssh_cleanup(ଶୂନ୍ୟ)
{
  (ଶୂନ୍ୟ)ssh_finalize();
}

ଶୂନ୍ୟ Curl_ssh_version(ଅକ୍ଷର *buffer, size_t buflen)
{
  (ଶୂନ୍ୟ)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_scp = {
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
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_sftp = {
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
