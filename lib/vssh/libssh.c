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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_LIBSSH

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef __VMS
#અન્તર્ભાવ <in.h>
#અન્તર્ભાવ <inet.h>
#endif

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "sendf.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "vssh/ssh.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "parsedate.h"          /* for the week day and month names */
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "vssh/vssh.h"
#અન્તર્ભાવ "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#અન્તર્ભાવ <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#અન્તર્ભાવ <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  કરો {                          \
    જો(x) {                     \
      ssh_string_free_char(x);  \
      x = ખાલી;                 \
    }                           \
  } જ્યારે(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

સ્થિર CURLcode sftp_error_to_CURLE(પૂર્ણાંક err)
{
  પસંદગી(err) {
  સ્થિતિ SSH_FX_OK:
    પાછા CURLE_OK;

  સ્થિતિ SSH_FX_NO_SUCH_FILE:
  સ્થિતિ SSH_FX_NO_SUCH_PATH:
    પાછા CURLE_REMOTE_FILE_NOT_FOUND;

  સ્થિતિ SSH_FX_PERMISSION_DENIED:
  સ્થિતિ SSH_FX_WRITE_PROTECT:
    પાછા CURLE_REMOTE_ACCESS_DENIED;

  સ્થિતિ SSH_FX_FILE_ALREADY_EXISTS:
    પાછા CURLE_REMOTE_FILE_EXISTS;

  મૂળભૂત:
    તોડો;
  }

  પાછા CURLE_SSH;
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
સ્થિર પૂર્ણાંક myssh_is_known(સંરચના Curl_easy *data, સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc;
  ssh_ચાવી pubkey;
  unsigned અક્ષર *hash_sha256 = ખાલી;
  size_t hlen_sha256;
  unsigned અક્ષર *hash_md5 = ખાલી;
  size_t hlen_md5;
  અક્ષર *found_base64 = ખાલી;
  અક્ષર *known_base64 = ખાલી;
  પૂર્ણાંક vstate;
  ગણના curl_khmatch keymatch;
  સંરચના curl_khkey foundkey;
  સંરચના curl_khkey *knownkeyp = ખાલી;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  સંરચના ssh_knownhosts_entry *knownhostsentry = ખાલી;
  સંરચના curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  જો(rc != SSH_OK)
    પાછા rc;

  જો(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ચાવી_SHA256)) {
    અચલ અક્ષર *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ચાવી_SHA256);
    અક્ષર *fingerprint_b64 = ખાલી;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    જો(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      જાઓ cleanup;
    }

    જો(curlx_base64_encode((અચલ uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      જાઓ cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    જ્યારે((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    જ્યારે((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    જો((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      જાઓ cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    જાઓ cleanup;
  }

  જો(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ચાવી_MD5)) {
    અચલ અક્ષર *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ચાવી_MD5);
    અક્ષર md5buffer[33];
    પૂર્ણાંક i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    જો(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      જાઓ cleanup;
    }

    માટે(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    જો(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      જાઓ cleanup;
    }

    rc = SSH_OK;
    જાઓ cleanup;
  }

  જો(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    જો(knownhostsentry) {
      જો(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        જો(rc != SSH_OK) {
          જાઓ cleanup;
        }
        knownkey.ચાવી = known_base64;
        knownkey.len = strlen(known_base64);

        પસંદગી(ssh_ચાવી_type(knownhostsentry->publickey)) {
        સ્થિતિ SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          તોડો;
        સ્થિતિ SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          તોડો;
        સ્થિતિ SSH_KEYTYPE_ECDSA:
        સ્થિતિ SSH_KEYTYPE_ECDSA_P256:
        સ્થિતિ SSH_KEYTYPE_ECDSA_P384:
        સ્થિતિ SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          તોડો;
        સ્થિતિ SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          તોડો;
        સ્થિતિ SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          તોડો;
        મૂળભૂત:
          rc = SSH_ERROR;
          જાઓ cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    પસંદગી(vstate) {
    સ્થિતિ SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      તોડો;
    સ્થિતિ SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      તોડો;
    સ્થિતિ SSH_KNOWN_HOSTS_NOT_FOUND:
    સ્થિતિ SSH_KNOWN_HOSTS_UNKNOWN:
    સ્થિતિ SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      તોડો;
    મૂળભૂત:
      keymatch = CURLKHMATCH_MISMATCH;
      તોડો;
    }

    જો(func) { /* use callback to determine action */
      સંરચના Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      જો(rc != SSH_OK)
        જાઓ cleanup;

      foundkey.ચાવી = found_base64;
      foundkey.len = strlen(found_base64);

      પસંદગી(ssh_ચાવી_type(pubkey)) {
      સ્થિતિ SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        તોડો;
      સ્થિતિ SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        તોડો;
      સ્થિતિ SSH_KEYTYPE_ECDSA:
      સ્થિતિ SSH_KEYTYPE_ECDSA_P256:
      સ્થિતિ SSH_KEYTYPE_ECDSA_P384:
      સ્થિતિ SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        તોડો;
      સ્થિતિ SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        તોડો;
      સ્થિતિ SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        તોડો;
      મૂળભૂત:
        rc = SSH_ERROR;
        જાઓ cleanup;
      }

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      પસંદગી(rc) {
      સ્થિતિ CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        જો(rc != SSH_OK) {
          જાઓ cleanup;
        }
        તોડો;
      સ્થિતિ CURLKHSTAT_FINE:
        તોડો;
      મૂળભૂત: /* REJECT/DEFER */
        rc = SSH_ERROR;
        જાઓ cleanup;
      }
    }
    નહીંતર {
      જો(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        જાઓ cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  જો(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    મુક્ત(found_base64); /* allocated by libssh, deallocate with system free */
  }
  જો(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    મુક્ત(known_base64); /* allocated by libssh, deallocate with system free */
  }
  જો(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  જો(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_ચાવી_free(pubkey);
  જો(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  પાછા rc;
}

સ્થિર પૂર્ણાંક myssh_to_ERROR(સંરચના Curl_easy *data,
                          સંરચના ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  પાછા SSH_ERROR;
}

સ્થિર પૂર્ણાંક myssh_to_SFTP_CLOSE(સંરચના Curl_easy *data,
                               સંરચના ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  પાછા SSH_ERROR;
}

સ્થિર પૂર્ણાંક myssh_to_PASSWD_AUTH(સંરચના Curl_easy *data,
                                સંરચના ssh_conn *sshc)
{
  જો(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    પાછા SSH_OK;
  }
  પાછા myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

સ્થિર પૂર્ણાંક myssh_to_ચાવી_AUTH(સંરચના Curl_easy *data,
                             સંરચના ssh_conn *sshc)
{
  જો(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_ચાવી_INIT);
    પાછા SSH_OK;
  }
  પાછા myssh_to_PASSWD_AUTH(data, sshc);
}

સ્થિર પૂર્ણાંક myssh_to_GSSAPI_AUTH(સંરચના Curl_easy *data,
                                સંરચના ssh_conn *sshc)
{
  જો(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    પાછા SSH_OK;
  }
  પાછા myssh_to_ચાવી_AUTH(data, sshc);
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_READDIR_INIT(સંરચના Curl_easy *data,
                                      સંરચના ssh_conn *sshc,
                                      સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  જો(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    પાછા SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  જો(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    પાછા myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_READDIR(સંરચના Curl_easy *data,
                                 સંરચના ssh_conn *sshc,
                                 સંરચના SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  જો(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  જો(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    જો(data->set.list_only) {
      અક્ષર *tmp_લીટી;

      tmp_લીટી = curl_maprintf("%s\n", sshc->readdir_filename);
      જો(!tmp_લીટી) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        પાછા SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmp_લીટી, sshc->readdir_len + 1);
      curlx_free(tmp_લીટી);

      જો(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        પાછા SSH_NO_ERROR;
      }
    }
    નહીંતર {
      જો(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        પાછા SSH_ERROR;
      }

      જો((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        જો(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          પાછા SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        પાછા SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      પાછા SSH_NO_ERROR;
    }
  }
  નહીંતર જો(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  નહીંતર {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    પાછા myssh_to_SFTP_CLOSE(data, sshc);
  }
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_READDIR_LINK(સંરચના Curl_easy *data,
                                      સંરચના ssh_conn *sshc)
{
  જો(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  જો(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    પાછા myssh_to_SFTP_CLOSE(data, sshc);
  }

  જો(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    જો(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    નહીંતર
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = ખાલી;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  નહીંતર {
    sshc->readdir_len = strlen(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  જો(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    પાછા SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = ખાલી;
  sshc->readdir_filename = ખાલી;
  sshc->readdir_longentry = ખાલી;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_READDIR_BOTTOM(સંરચના Curl_easy *data,
                                        સંરચના ssh_conn *sshc)
{
  CURLcode result;

  જો(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  નહીંતર
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = ખાલી;

  જો(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  નહીંતર
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_READDIR_DONE(સંરચના Curl_easy *data,
                                      સંરચના ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = ખાલી;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  પાછા SSH_NO_ERROR;
}

સ્થિર શૂન્ય myssh_quote_error(સંરચના Curl_easy *data, સંરચના ssh_conn *sshc,
                              અચલ અક્ષર *cmd)
{
  જો(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE_STATVFS(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  જો(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    પાછા SSH_OK;
  }
  નહીંતર જો(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#અથવાજો defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#નહીંતર
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    અક્ષર *tmp = curl_maprintf("statvfs:\n"
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

    જો(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    જો(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    જો(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા SSH_OK;
}

સ્થિર પૂર્ણાંક myssh_auth_interactive(સંરચના connectdata *conn,
                                  સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc;
  પૂર્ણાંક nprompts;

restart:
  પસંદગી(sshc->kbd_state) {
  સ્થિતિ 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, ખાલી, ખાલી);
    જો(rc == SSH_AUTH_AGAIN)
      પાછા SSH_AGAIN;

    જો(rc != SSH_AUTH_INFO)
      પાછા SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    જો(nprompts != 1)
      પાછા SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    જો(rc < 0)
      પાછા SSH_ERROR;

    FALLTHROUGH();
  સ્થિતિ 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ખાલી, ખાલી);
    જો(rc == SSH_AUTH_AGAIN)
      પાછા SSH_AGAIN;
    નહીંતર જો(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    નહીંતર જો(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      જો(nprompts)
        પાછા SSH_ERROR;

      sshc->kbd_state = 2;
      જાઓ restart;
    }
    નહીંતર
      rc = SSH_ERROR;
    તોડો;
  સ્થિતિ 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ખાલી, ખાલી);
    જો(rc == SSH_AUTH_AGAIN)
      પાછા SSH_AGAIN;
    નહીંતર જો(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    નહીંતર
      rc = SSH_ERROR;

    તોડો;
  મૂળભૂત:
    પાછા SSH_ERROR;
  }

  sshc->kbd_state = 0;
  પાછા rc;
}

સ્થિર શૂન્ય myssh_state_init(સંરચના Curl_easy *data,
                             સંરચના ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#જો 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

સ્થિર શૂન્ય myssh_block2waitfor(સંરચના connectdata *conn,
                                સંરચના ssh_conn *sshc,
                                bool block)
{
  (શૂન્ય)conn;
  જો(block) {
    પૂર્ણાંક dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  નહીંતર
    sshc->waitfor = 0;
}

સ્થિર પૂર્ણાંક myssh_in_S_STARTUP(સંરચના Curl_easy *data,
                              સંરચના ssh_conn *sshc)
{
  સંરચના connectdata *conn = data->conn;
  પૂર્ણાંક rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  જો(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  નહીંતર જો(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  નહીંતર
    myssh_to(data, sshc, SSH_HOSTKEY);

  પાછા rc;
}

સ્થિર પૂર્ણાંક myssh_in_AUTHLIST(સંરચના Curl_easy *data,
                             સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, ખાલી);
  જો(rc == SSH_AUTH_AGAIN)
    પાછા SSH_AGAIN;

  જો(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    પાછા rc;
  }
  નહીંતર જો(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    પાછા rc;
  }

  sshc->auth_methods =
    (unsigned પૂર્ણાંક)ssh_userauth_list(sshc->ssh_session, ખાલી);
  જો(sshc->auth_methods)
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
  જો((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_ચાવી && sshc->priv_ચાવી[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  નહીંતર જો(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  નહીંતર જો(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_ચાવી_INIT);
  }
  નહીંતર જો(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  નહીંતર {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  પાછા rc;
}

સ્થિર પૂર્ણાંક myssh_in_AUTH_PKEY_INIT(સંરચના Curl_easy *data,
                                   સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc;
  જો(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    પાછા rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  જો(sshc->priv_ચાવી && sshc->priv_ચાવી[0]) {
    જો(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, ખાલી, sshc->pubkey);
      જો(rc == SSH_AUTH_AGAIN)
        પાછા SSH_AGAIN;

      જો(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        પાછા rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_ચાવી,
                                     sshc->passphrase, ખાલી,
                                     ખાલી, &sshc->privkey);
    જો(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_ચાવી);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      પાછા rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  નહીંતર {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, ખાલી,
                                     sshc->passphrase);
    જો(rc == SSH_AUTH_AGAIN)
      પાછા SSH_AGAIN;

    જો(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      પાછા rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  પાછા rc;
}

સ્થિર પૂર્ણાંક myssh_in_AUTH_PKEY(સંરચના Curl_easy *data,
                              સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = ssh_userauth_publickey(sshc->ssh_session, ખાલી, sshc->privkey);
  જો(rc == SSH_AUTH_AGAIN)
    પાછા SSH_AGAIN;
  નહીંતર જો(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    પાછા SSH_OK;
  }
  નહીંતર {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    પાછા myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

સ્થિર પૂર્ણાંક myssh_in_AUTH_GSSAPI(સંરચના Curl_easy *data,
                                સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc;
  જો(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    પાછા myssh_to_ચાવી_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  જો(rc == SSH_AUTH_AGAIN)
    પાછા SSH_AGAIN;

  જો(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    પાછા SSH_OK;
  }

  પાછા myssh_to_ચાવી_AUTH(data, sshc);
}

સ્થિર પૂર્ણાંક myssh_in_AUTH_ચાવી_INIT(સંરચના Curl_easy *data,
                                  સંરચના ssh_conn *sshc)
{
  જો(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_ચાવી);
    પાછા SSH_NO_ERROR;
  }
  પાછા myssh_to_PASSWD_AUTH(data, sshc);
}

સ્થિર પૂર્ણાંક myssh_in_AUTH_ચાવી(સંરચના Curl_easy *data,
                             સંરચના ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  પૂર્ણાંક rc = myssh_auth_interactive(data->conn, sshc);
  જો(rc == SSH_AGAIN)
    પાછા rc;
  નહીંતર જો(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    પાછા SSH_NO_ERROR;
  }
  નહીંતર
    પાછા myssh_to_PASSWD_AUTH(data, sshc);
}

સ્થિર પૂર્ણાંક myssh_in_AUTH_PASS_INIT(સંરચના Curl_easy *data,
                                   સંરચના ssh_conn *sshc)
{
  જો(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    પાછા myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_AUTH_PASS(સંરચના Curl_easy *data,
                              સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = ssh_userauth_password(sshc->ssh_session, ખાલી,
                                 Curl_creds_passwd(data->conn->creds));
  જો(rc == SSH_AUTH_AGAIN)
    પાછા SSH_AGAIN;
  નહીંતર જો(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    પાછા SSH_NO_ERROR;
  }
  પાછા myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

સ્થિર પૂર્ણાંક myssh_in_AUTH_DONE(સંરચના Curl_easy *data,
                              સંરચના ssh_conn *sshc)
{
  સંરચના connectdata *conn = data->conn;
  જો(!sshc->authed) {
    failf(data, "Authentication failure");
    પાછા myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  જો(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    પાછા SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_UPLOAD_INIT(સંરચના Curl_easy *data,
                                સંરચના ssh_conn *sshc,
                                સંરચના SSHPROTO *sshp)
{
  પૂર્ણાંક flags;
  પૂર્ણાંક rc = 0;

  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  જો(data->state.resume_from) {
    sftp_attributes attrs;

    જો(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      જો(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        જો(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          પાછા rc;
        }
        data->state.resume_from = size;
      }
      નહીંતર {
        data->state.resume_from = 0;
      }
    }
  }

  જો(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  નહીંતર જો(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  નહીંતર
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  જો(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  જો(!sshc->sftp_file) {
    પૂર્ણાંક err = sftp_get_error(sshc->sftp_session);

    જો((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      પાછા rc;
    }
    નહીંતર {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      પાછા rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  જો(data->state.resume_from > 0 && !data->set.remote_append) {
    પૂર્ણાંક seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    જો(data->set.seek_func) {
      સંરચના Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    જો(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      જો(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        પાછા rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      કરો {
        અક્ષર scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)કદ(scratch)) ?
          કદ(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        જો((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          પાછા rc;
        }
      } જ્યારે(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    જો(data->state.infilesize > 0) {
      જો(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        પાછા myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    જો(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      પાછા rc;
    }
  }
  જો(data->state.infilesize > 0) {
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
#જો LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  પાછા rc;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_DOWNLOAD_INIT(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc,
                                       સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  જો(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  જો(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    પાછા myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_DOWNLOAD_STAT(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc)
{
  curl_off_t size;
  પૂર્ણાંક rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  જો(!attrs ||
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
    જો(attrs)
      sftp_attributes_free(attrs);
  }
  નહીંતર {
    size = attrs->size;

    sftp_attributes_free(attrs);

    જો(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      પાછા myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    જો(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      જો(result)
        પાછા myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      જો(rc)
        પાછા myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  જો(data->state.resume_from) {
    જો(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      જો(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        પાછા myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    નહીંતર {
      જો(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        પાછા myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    જો(rc)
      પાછા myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  જો(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    પાછા rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  પાછા rc;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_CLOSE(સંરચના Curl_easy *data,
                               સંરચના ssh_conn *sshc,
                               સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  જો(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ખાલી;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  જો(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_STOP);
  }
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_SHUTDOWN(સંરચના Curl_easy *data,
                                  સંરચના ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#જો LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  જો(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ખાલી;
  }

  જો(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ખાલી;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_INIT(સંરચના Curl_easy *data,
                              સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  જો(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    પાછા myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  જો(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    પાછા myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_REALPATH(સંરચના Curl_easy *data,
                                  સંરચના ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  જો(!sshc->homedir)
    પાછા myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  જો(!data->state.most_recent_ftp_entrypath)
    પાછા myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE_INIT(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc,
                                    સંરચના SSHPROTO *sshp)
{
  CURLcode result;

  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  જો(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  નહીંતર જો(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  નહીંતર
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_POSTQUOTE_INIT(સંરચના Curl_easy *data,
                                        સંરચના ssh_conn *sshc)
{
  જો(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_STOP);
  }
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક quote_error(સંરચના Curl_easy *data,
                       સંરચના ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE(સંરચના Curl_easy *data,
                               સંરચના ssh_conn *sshc,
                               સંરચના SSHPROTO *sshp)
{
  અચલ અક્ષર *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  અચલ અક્ષર *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  જો(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  જો(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    અક્ષર *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    જો(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      પાછા SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    જો(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    નહીંતર
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    પાછા SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  જો(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    પાછા SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  જો(result) {
    જો(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    નહીંતર
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    પાછા SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  જો(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    જો(result) {
      જો(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      નહીંતર
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      પાછા SSH_NO_ERROR;
    }
    જો(*cp)
      પાછા quote_error(data, sshc);
    sshc->quote_attrs = ખાલી;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    પાછા SSH_NO_ERROR;
  }
  જો(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    જો(result) {
      જો(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      નહીંતર
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      પાછા SSH_NO_ERROR;
    }
    જો(*cp)
      પાછા quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    પાછા SSH_NO_ERROR;
  }
  નહીંતર જો(!strncmp(cmd, "mkdir ", 6)) {
    જો(*cp)
      પાછા quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    પાછા SSH_NO_ERROR;
  }
  નહીંતર જો(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    જો(result) {
      જો(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      નહીંતર
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      પાછા SSH_NO_ERROR;
    }
    જો(*cp)
      પાછા quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    પાછા SSH_NO_ERROR;
  }
  નહીંતર જો(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    જો(*cp)
      પાછા quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    પાછા SSH_NO_ERROR;
  }
  નહીંતર જો(!strncmp(cmd, "rm ", 3)) {
    જો(*cp)
      પાછા quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    પાછા SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  નહીંતર જો(!strncmp(cmd, "statvfs ", 8)) {
    જો(*cp)
      પાછા quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    પાછા SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_NEXT_QUOTE(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  જો(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  નહીંતર {
    જો(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    નહીંતર {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE_STAT(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc)
{
  અચલ અક્ષર *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  જો(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  જો(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  જો(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, ખાલી);
    પાછા SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  જો(!strncmp(cmd, "chgrp", 5)) {
    અચલ અક્ષર *p = sshc->quote_path1;
    curl_off_t gid;
    જો(curlx_str_સંખ્યા(&p, &gid, UINT_MAX)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, ખાલી);
      પાછા SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  નહીંતર જો(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    અચલ અક્ષર *p = sshc->quote_path1;
    જો(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, ખાલી);
      પાછા SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  નહીંતર જો(!strncmp(cmd, "chown", 5)) {
    અચલ અક્ષર *p = sshc->quote_path1;
    curl_off_t uid;
    જો(curlx_str_સંખ્યા(&p, &uid, UINT_MAX)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, ખાલી);
      પાછા SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  નહીંતર જો(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    જો(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#જો SIZEOF_TIME_T > 4
    નહીંતર જો(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    જો(fail) {
      myssh_quote_error(data, sshc, ખાલી);
      પાછા SSH_NO_ERROR;
    }
    જો(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    નહીંતર /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  પાછા SSH_NO_ERROR;
}

સ્થિર શૂન્ય conn_forget_socket(સંરચના Curl_easy *data, int8_t sockindex)
{
  સંરચના connectdata *conn = data->conn;
  જો(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    સંરચના Curl_cfilter *cf = conn->cfilter[sockindex];
    જો(cf)
      (શૂન્ય)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, ખાલી);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

સ્થિર શૂન્ય myssh_SESSION_DISCONNECT(સંરચના Curl_easy *data,
                                     સંરચના ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  જો(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ખાલી;
  }

  જો(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ખાલી;
  }
  જો(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ખાલી;
  }

  ssh_disconnect(sshc->ssh_session);
  જો(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

સ્થિર પૂર્ણાંક myssh_SSH_SCP_DOWNLOAD(સંરચના Curl_easy *data,
                                  સંરચના ssh_conn *sshc)
{
  uint64_t bytecount;
  પૂર્ણાંક rc = ssh_scp_pull_request(sshc->scp_session);
  જો(rc != SSH_SCP_REQUEST_NEWFILE) {
    અચલ અક્ષર *err_સંદેશ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_સંદેશ);
    પાછા myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  જો(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    પાછા myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  પાછા 0;
}

સ્થિર પૂર્ણાંક myssh_in_TRANS_INIT(સંરચના Curl_easy *data, સંરચના ssh_conn *sshc,
                               સંરચના SSHPROTO *sshp)
{
  CURLcode result;
  પૂર્ણાંક rc = 0;
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  જો(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    પાછા 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  જો(data->state.upload) {
    જો(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      પાછા myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  નહીંતર {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  જો(!sshc->scp_session) {
    અચલ અક્ષર *err_સંદેશ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_સંદેશ);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  પાછા rc;
}

સ્થિર શૂન્ય sshc_cleanup(સંરચના ssh_conn *sshc)
{
  જો(sshc->initialized) {
    જો(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = ખાલી;
    }
    જો(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = ખાલી;
    }
    જો(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = ખાલી;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    જો(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = ખાલી;
    }
    જો(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = ખાલી;
    }
    જો(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = ખાલી;
    }
    જો(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = ખાલી;
    }
    જો(sshc->privkey) {
      ssh_ચાવી_free(sshc->privkey);
      sshc->privkey = ખાલી;
    }
    જો(sshc->pubkey) {
      ssh_ચાવી_free(sshc->pubkey);
      sshc->pubkey = ખાલી;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_ચાવી);
    curlx_safefree(sshc->priv_ચાવી);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE_SETSTAT(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  જો(rc == SSH_AGAIN)
    પાછા rc;
  જો(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    પાછા rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE_SYMLINK(સંરચના Curl_easy *data,
                                       સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  જો(rc == SSH_AGAIN)
    પાછા rc;
  જો(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    પાછા rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE_MKDIR(સંરચના Curl_easy *data,
                                     સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  જો(rc == SSH_AGAIN)
    પાછા rc;
  જો(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    પાછા rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE_RENAME(સંરચના Curl_easy *data,
                                      સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  જો(rc == SSH_AGAIN)
    પાછા rc;
  જો(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    પાછા rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE_RMDIR(સંરચના Curl_easy *data,
                                     સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  જો(rc == SSH_AGAIN)
    પાછા rc;
  જો(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    પાછા rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_QUOTE_UNLINK(સંરચના Curl_easy *data,
                                      સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  જો(rc == SSH_AGAIN)
    પાછા rc;
  જો(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    પાછા rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_GETINFO(સંરચના Curl_easy *data,
                                 સંરચના ssh_conn *sshc)
{
  જો(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  નહીંતર
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_FILETIME(સંરચના Curl_easy *data,
                                  સંરચના ssh_conn *sshc,
                                  સંરચના SSHPROTO *sshp)
{
  sftp_attributes attrs;
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  જો(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_TRANS_INIT(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc,
                                    સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  જો(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  નહીંતર {
    size_t path_len = strlen(sshp->path);

    જો(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    નહીંતર
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_CREATE_DIRS_INIT(સંરચના Curl_easy *data,
                                          સંરચના ssh_conn *sshc,
                                          સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  જો(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  નહીંતર {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_CREATE_DIRS(સંરચના Curl_easy *data,
                                     સંરચના ssh_conn *sshc,
                                     સંરચના SSHPROTO *sshp)
{
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  જો(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    પાછા SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SFTP_CREATE_DIRS_MKDIR(સંરચના Curl_easy *data,
                                           સંરચના ssh_conn *sshc,
                                           સંરચના SSHPROTO *sshp)
{
  પૂર્ણાંક rc;
  પૂર્ણાંક err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  જો(rc == SSH_AGAIN)
    પાછા rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  જો(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    જો((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      પાછા myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  પાછા rc;
}

સ્થિર પૂર્ણાંક myssh_in_SCP_UPLOAD_INIT(સંરચના Curl_easy *data,
                                    સંરચના ssh_conn *sshc,
                                    સંરચના SSHPROTO *sshp)
{
  પૂર્ણાંક rc;
  જો(!sshp)
    પાછા myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  જો(rc != SSH_OK) {
    અચલ અક્ષર *err_સંદેશ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_સંદેશ);
    પાછા myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ધકેલો_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (પૂર્ણાંક)data->set.new_file_perms);

  જો(rc != SSH_OK) {
    અચલ અક્ષર *err_સંદેશ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_સંદેશ);
    પાછા myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SCP_DOWNLOAD_INIT(સંરચના Curl_easy *data,
                                      સંરચના ssh_conn *sshc)
{
  પૂર્ણાંક rc = ssh_scp_init(sshc->scp_session);
  જો(rc != SSH_OK) {
    અચલ અક્ષર *err_સંદેશ = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_સંદેશ);
    પાછા myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SCP_DONE(સંરચના Curl_easy *data,
                             સંરચના ssh_conn *sshc)
{
  જો(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  નહીંતર
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SCP_SEND_EOF(સંરચના Curl_easy *data,
                                 સંરચના ssh_conn *sshc)
{
  જો(sshc->scp_session) {
    પૂર્ણાંક rc = ssh_scp_close(sshc->scp_session);
    જો(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      પાછા SSH_AGAIN;
    }
    જો(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  પાછા SSH_NO_ERROR;
}

સ્થિર પૂર્ણાંક myssh_in_SCP_CHANNEL_FREE(સંરચના Curl_easy *data,
                                     સંરચના ssh_conn *sshc)
{
  જો(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ખાલી;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  પાછા SSH_NO_ERROR;
}

સ્થિર CURLcode myssh_in_SESSION_FREE(સંરચના Curl_easy *data,
                                      સંરચના ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  memset(sshc, 0, કદ(સંરચના ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  પાછા result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
સ્થિર CURLcode myssh_statemachine(સંરચના Curl_easy *data,
                                   સંરચના ssh_conn *sshc,
                                   સંરચના SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  પૂર્ણાંક rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  કરો {

    પસંદગી(sshc->state) {
    સ્થિતિ SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    સ્થિતિ SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      જો(rc)
        તોડો;
      FALLTHROUGH();
    સ્થિતિ SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      જો(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        તોડો;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    સ્થિતિ SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      તોડો;
    સ્થિતિ SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      તોડો;
    સ્થિતિ SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      તોડો;
    સ્થિતિ SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      તોડો;
    સ્થિતિ SSH_AUTH_ચાવી_INIT:
      rc = myssh_in_AUTH_ચાવી_INIT(data, sshc);
      તોડો;
    સ્થિતિ SSH_AUTH_ચાવી:
      rc = myssh_in_AUTH_ચાવી(data, sshc);
      તોડો;
    સ્થિતિ SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      તોડો;
    સ્થિતિ SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      તોડો;
    સ્થિતિ SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      તોડો;
    સ્થિતિ SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      તોડો;
    સ્થિતિ SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      તોડો;
    સ્થિતિ SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      જો(rc)
        તોડો;
      FALLTHROUGH();
    સ્થિતિ SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      તોડો;
    સ્થિતિ SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      તોડો;
    સ્થિતિ SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      તોડો;
    સ્થિતિ SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    સ્થિતિ SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    સ્થિતિ SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      તોડો;
    સ્થિતિ SSH_QUIT:
    મૂળભૂત:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      તોડો;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } જ્યારે((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  જો(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  જો(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (પૂર્ણાંક)result, *block);
  પાછા result;
}

/* called repeatedly until done from multi.c */
સ્થિર CURLcode myssh_multi_statemach(સંરચના Curl_easy *data,
                                      bool *done)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  સંરચના SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  જો(!sshc || !sshp)
    પાછા CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  પાછા result;
}

સ્થિર CURLcode myssh_block_statemach(સંરચના Curl_easy *data,
                                      સંરચના ssh_conn *sshc,
                                      સંરચના SSHPROTO *sshp,
                                      bool disconnect)
{
  સંરચના connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  જ્યારે((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t ડાબું_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
    જો(result)
      તોડો;

    જો(!disconnect) {
      result = Curl_pgrsCheck(data);
      જો(result)
        તોડો;

      ડાબું_ms = Curl_timeleft_ms(data);
      જો(ડાબું_ms < 0) {
        failf(data, "Operation timed out");
        પાછા CURLE_OPERATION_TIMEDOUT;
      }
    }

    જો(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (શૂન્ય)SOCKET_READABLE(fd_read,
                            (!ડાબું_ms || ડાબું_ms > 1000) ? 1000 : ડાબું_ms);
    }
  }

  પાછા result;
}

સ્થિર શૂન્ય myssh_easy_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના SSHPROTO *sshp = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

સ્થિર શૂન્ય myssh_conn_dtor(અચલ શૂન્ય *ચાવી, size_t klen, શૂન્ય *entry)
{
  સંરચના ssh_conn *sshc = entry;
  (શૂન્ય)ચાવી;
  (શૂન્ય)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
સ્થિર CURLcode myssh_setup_connection(સંરચના Curl_easy *data,
                                       સંરચના connectdata *conn)
{
  સંરચના SSHPROTO *sshp;
  સંરચના ssh_conn *sshc;

  sshc = curlx_calloc(1, કદ(*sshc));
  જો(!sshc)
    પાછા CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  જો(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, કદ(*sshp));
  જો(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    પાછા CURLE_OUT_OF_MEMORY;

  પાછા Curl_ssh_setup_pkey(data, sshc);
}

સ્થિર Curl_recv scp_recv, sftp_recv;
સ્થિર Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
સ્થિર CURLcode myssh_connect(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result;
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  સંરચના SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  પૂર્ણાંક rc;

  જો(!sshc || !ssh)
    પાછા CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  જો(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  નહીંતર {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  જો(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    પાછા CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  જો(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    પાછા CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, ખાલી);
  જો(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  જો(rc != SSH_OK) {
    failf(data, "Could not set socket");
    પાછા CURLE_FAILED_INIT;
  }

  જો(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    જો(rc != SSH_OK) {
      failf(data, "Could not set user");
      પાછા CURLE_FAILED_INIT;
    }
  }

  જો(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    જો(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    જો(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      પાછા CURLE_FAILED_INIT;
    }
  }

  જો(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    જો(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      પાછા CURLE_FAILED_INIT;
    }
  }

  જો(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    જો(rc != SSH_OK) {
      failf(data, "Could not set compression");
      પાછા CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = ખાલી;
  sshc->pubkey = ખાલી;

  જો(sshc->pub_ચાવી) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_ચાવી, &sshc->pubkey);
    જો(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      પાછા CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  પાછા result;
}

/* called from multi.c while DOing */
સ્થિર CURLcode scp_doing(સંરચના Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  જો(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  પાછા result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

સ્થિર CURLcode scp_perform(સંરચના Curl_easy *data,
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  જો(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  પાછા result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
સ્થિર CURLcode scp_disconnect(સંરચના Curl_easy *data,
                               સંરચના connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  સંરચના SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (શૂન્ય)dead_connection;

  જો(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  પાછા result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
સ્થિર CURLcode myssh_done(સંરચના Curl_easy *data,
                           સંરચના ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  સંરચના SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  જો(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
  નહીંતર
    result = status;

  જો(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    પાછા CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  પાછા result;
}

સ્થિર CURLcode scp_done(સંરચના Curl_easy *data, CURLcode status,
                         bool premature)
{
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (શૂન્ય)premature;

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;
  જો(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  પાછા myssh_done(data, sshc, status);
}

સ્થિર CURLcode scp_send(સંરચના Curl_easy *data, int8_t sockindex,
                         અચલ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  પૂર્ણાંક rc;
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (શૂન્ય)sockindex; /* we only support SCP on the fixed known primary socket */
  (શૂન્ય)eos;
  *pnwritten = 0;

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#જો 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  જો(rc == SSH_AGAIN)
    પાછા CURLE_AGAIN;
  નહીંતર
#endif
  જો(rc != SSH_OK)
    પાછા CURLE_SSH;

  *pnwritten = len;
  પાછા CURLE_OK;
}

સ્થિર CURLcode scp_recv(સંરચના Curl_easy *data, int8_t sockindex,
                         અક્ષર *mem, size_t len, size_t *pnread)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  પૂર્ણાંક nread;

  (શૂન્ય)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  જો(nread == SSH_ERROR)
    પાછા CURLE_SSH;
#જો 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  જો(nread == SSH_AGAIN)
    પાછા CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  પાછા CURLE_OK;
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

સ્થિર CURLcode sftp_perform(સંરચના Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  જો(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  પાછા result;
}

/* called from multi.c while DOing */
સ્થિર CURLcode sftp_doing(સંરચના Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  જો(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  પાછા result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
સ્થિર CURLcode sftp_disconnect(સંરચના Curl_easy *data,
                                સંરચના connectdata *conn,
                                bool dead_connection)
{
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  સંરચના SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (શૂન્ય)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  જો(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
  પાછા result;
}

સ્થિર CURLcode sftp_done(સંરચના Curl_easy *data, CURLcode status,
                          bool premature)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;
  જો(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    જો(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  પાછા myssh_done(data, sshc, status);
}

/* return number of sent bytes */
સ્થિર CURLcode sftp_send(સંરચના Curl_easy *data, int8_t sockindex,
                          અચલ uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (શૂન્ય)sockindex;
  (શૂન્ય)eos;
  *pnwritten = 0;

  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

#જો LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  પસંદગી(sshc->sftp_send_state) {
  સ્થિતિ 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    જો(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      પાછા CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  સ્થિતિ 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    જો(nwrite == SSH_AGAIN)
      પાછા CURLE_AGAIN;
    નહીંતર જો(nwrite < 0)
      પાછા CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    પાછા CURLE_OK;
  મૂળભૂત:
    /* we never reach here */
    પાછા CURLE_SEND_ERROR;
  }
#નહીંતર
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  જો(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#જો 0 /* not returned by libssh on write */
  જો(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  નહીંતર
#endif
  જો(nwrite < 0)
    પાછા CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  પાછા CURLE_OK;
#endif
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
સ્થિર CURLcode sftp_recv(સંરચના Curl_easy *data, int8_t sockindex,
                          અક્ષર *mem, size_t len, size_t *pnread)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (શૂન્ય)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  પસંદગી(sshc->sftp_recv_state) {
  સ્થિતિ 0:
#જો LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    જો(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      પાછા CURLE_RECV_ERROR;
    }
#નહીંતર
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    જો(sshc->sftp_file_index < 0)
      પાછા CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  સ્થિતિ 1:
    sshc->sftp_recv_state = 1;

#જો LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#નહીંતર
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    જો(nread == SSH_AGAIN)
      પાછા CURLE_AGAIN;
    નહીંતર જો(nread < 0)
      પાછા CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    પાછા CURLE_OK;

  મૂળભૂત:
    /* we never reach here */
    પાછા CURLE_RECV_ERROR;
  }
}

સ્થિર CURLcode myssh_do_it(સંરચના Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  જો(!sshc)
    પાછા CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  જો(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  નહીંતર
    result = sftp_perform(data, &connected, done);

  પાછા result;
}

CURLcode Curl_ssh_init(શૂન્ય)
{
  જો(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    પાછા CURLE_FAILED_INIT;
  }
  પાછા CURLE_OK;
}

શૂન્ય Curl_ssh_cleanup(શૂન્ય)
{
  (શૂન્ય)ssh_finalize();
}

શૂન્ય Curl_ssh_version(અક્ષર *buffer, size_t buflen)
{
  (શૂન્ય)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
અચલ સંરચના Curl_protocol Curl_protocol_scp = {
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
અચલ સંરચના Curl_protocol Curl_protocol_sftp = {
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
