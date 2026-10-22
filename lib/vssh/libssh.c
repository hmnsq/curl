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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_LIBSSH

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef __VMS
#അന്തര്ഭാവ <in.h>
#അന്തര്ഭാവ <inet.h>
#endif

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "vssh/ssh.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "cfilters.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "parsedate.h"          /* for the week day and month names */
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "vssh/vssh.h"
#അന്തര്ഭാവ "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#അന്തര്ഭാവ <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#അന്തര്ഭാവ <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  ചെയ്യുക {                          \
    എങ്കിൽ(x) {                     \
      ssh_string_free_char(x);  \
      x = ശൂന്യ;                 \
    }                           \
  } ആകുമ്പോൾ(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

സ്ഥിതം CURLcode sftp_error_to_CURLE(പൂർണ്ണസംഖ്യ err)
{
  തിരഞ്ഞെടുപ്പ്(err) {
  അവസ്ഥ SSH_FX_OK:
    തിരികെ CURLE_OK;

  അവസ്ഥ SSH_FX_NO_SUCH_FILE:
  അവസ്ഥ SSH_FX_NO_SUCH_PATH:
    തിരികെ CURLE_REMOTE_FILE_NOT_FOUND;

  അവസ്ഥ SSH_FX_PERMISSION_DENIED:
  അവസ്ഥ SSH_FX_WRITE_PROTECT:
    തിരികെ CURLE_REMOTE_ACCESS_DENIED;

  അവസ്ഥ SSH_FX_FILE_ALREADY_EXISTS:
    തിരികെ CURLE_REMOTE_FILE_EXISTS;

  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }

  തിരികെ CURLE_SSH;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_is_known(ഘടന Curl_easy *data, ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc;
  ssh_താക്കോൽ pubkey;
  unsigned അക്ഷരം *hash_sha256 = ശൂന്യ;
  size_t hlen_sha256;
  unsigned അക്ഷരം *hash_md5 = ശൂന്യ;
  size_t hlen_md5;
  അക്ഷരം *found_base64 = ശൂന്യ;
  അക്ഷരം *known_base64 = ശൂന്യ;
  പൂർണ്ണസംഖ്യ vstate;
  എണ്ണൽ curl_khmatch keymatch;
  ഘടന curl_khkey foundkey;
  ഘടന curl_khkey *knownkeyp = ശൂന്യ;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  ഘടന ssh_knownhosts_entry *knownhostsentry = ശൂന്യ;
  ഘടന curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  എങ്കിൽ(rc != SSH_OK)
    തിരികെ rc;

  എങ്കിൽ(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_SHA256)) {
    സ്ഥിരം അക്ഷരം *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_SHA256);
    അക്ഷരം *fingerprint_b64 = ശൂന്യ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    എങ്കിൽ(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      പോകുക cleanup;
    }

    എങ്കിൽ(curlx_base64_encode((സ്ഥിരം uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      പോകുക cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    ആകുമ്പോൾ((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    ആകുമ്പോൾ((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    എങ്കിൽ((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      പോകുക cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    പോകുക cleanup;
  }

  എങ്കിൽ(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_MD5)) {
    സ്ഥിരം അക്ഷരം *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_താക്കോൽ_MD5);
    അക്ഷരം md5buffer[33];
    പൂർണ്ണസംഖ്യ i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    എങ്കിൽ(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      പോകുക cleanup;
    }

    വേണ്ടി(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    എങ്കിൽ(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      പോകുക cleanup;
    }

    rc = SSH_OK;
    പോകുക cleanup;
  }

  എങ്കിൽ(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    എങ്കിൽ(knownhostsentry) {
      എങ്കിൽ(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        എങ്കിൽ(rc != SSH_OK) {
          പോകുക cleanup;
        }
        knownkey.താക്കോൽ = known_base64;
        knownkey.len = strlen(known_base64);

        തിരഞ്ഞെടുപ്പ്(ssh_താക്കോൽ_type(knownhostsentry->publickey)) {
        അവസ്ഥ SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          നിർത്തുക;
        അവസ്ഥ SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          നിർത്തുക;
        അവസ്ഥ SSH_KEYTYPE_ECDSA:
        അവസ്ഥ SSH_KEYTYPE_ECDSA_P256:
        അവസ്ഥ SSH_KEYTYPE_ECDSA_P384:
        അവസ്ഥ SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          നിർത്തുക;
        അവസ്ഥ SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          നിർത്തുക;
        അവസ്ഥ SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
          rc = SSH_ERROR;
          പോകുക cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    തിരഞ്ഞെടുപ്പ്(vstate) {
    അവസ്ഥ SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      നിർത്തുക;
    അവസ്ഥ SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      നിർത്തുക;
    അവസ്ഥ SSH_KNOWN_HOSTS_NOT_FOUND:
    അവസ്ഥ SSH_KNOWN_HOSTS_UNKNOWN:
    അവസ്ഥ SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      keymatch = CURLKHMATCH_MISMATCH;
      നിർത്തുക;
    }

    എങ്കിൽ(func) { /* use callback to determine action */
      ഘടന Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      എങ്കിൽ(rc != SSH_OK)
        പോകുക cleanup;

      foundkey.താക്കോൽ = found_base64;
      foundkey.len = strlen(found_base64);

      തിരഞ്ഞെടുപ്പ്(ssh_താക്കോൽ_type(pubkey)) {
      അവസ്ഥ SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        നിർത്തുക;
      അവസ്ഥ SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        നിർത്തുക;
      അവസ്ഥ SSH_KEYTYPE_ECDSA:
      അവസ്ഥ SSH_KEYTYPE_ECDSA_P256:
      അവസ്ഥ SSH_KEYTYPE_ECDSA_P384:
      അവസ്ഥ SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        നിർത്തുക;
      അവസ്ഥ SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        നിർത്തുക;
      അവസ്ഥ SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        rc = SSH_ERROR;
        പോകുക cleanup;
      }

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      തിരഞ്ഞെടുപ്പ്(rc) {
      അവസ്ഥ CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        എങ്കിൽ(rc != SSH_OK) {
          പോകുക cleanup;
        }
        നിർത്തുക;
      അവസ്ഥ CURLKHSTAT_FINE:
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി: /* REJECT/DEFER */
        rc = SSH_ERROR;
        പോകുക cleanup;
      }
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        പോകുക cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  എങ്കിൽ(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    സ്വതന്ത്രം(found_base64); /* allocated by libssh, deallocate with system free */
  }
  എങ്കിൽ(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    സ്വതന്ത്രം(known_base64); /* allocated by libssh, deallocate with system free */
  }
  എങ്കിൽ(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  എങ്കിൽ(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_താക്കോൽ_free(pubkey);
  എങ്കിൽ(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_to_ERROR(ഘടന Curl_easy *data,
                          ഘടന ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  തിരികെ SSH_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_to_SFTP_CLOSE(ഘടന Curl_easy *data,
                               ഘടന ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  തിരികെ SSH_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_to_PASSWD_AUTH(ഘടന Curl_easy *data,
                                ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    തിരികെ SSH_OK;
  }
  തിരികെ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_to_താക്കോൽ_AUTH(ഘടന Curl_easy *data,
                             ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_താക്കോൽ_INIT);
    തിരികെ SSH_OK;
  }
  തിരികെ myssh_to_PASSWD_AUTH(data, sshc);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_to_GSSAPI_AUTH(ഘടന Curl_easy *data,
                                ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    തിരികെ SSH_OK;
  }
  തിരികെ myssh_to_താക്കോൽ_AUTH(data, sshc);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_READDIR_INIT(ഘടന Curl_easy *data,
                                      ഘടന ssh_conn *sshc,
                                      ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  എങ്കിൽ(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    തിരികെ SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  എങ്കിൽ(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    തിരികെ myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_READDIR(ഘടന Curl_easy *data,
                                 ഘടന ssh_conn *sshc,
                                 ഘടന SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  എങ്കിൽ(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  എങ്കിൽ(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    എങ്കിൽ(data->set.list_only) {
      അക്ഷരം *tmp_വരി;

      tmp_വരി = curl_maprintf("%s\n", sshc->readdir_filename);
      എങ്കിൽ(!tmp_വരി) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        തിരികെ SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmp_വരി, sshc->readdir_len + 1);
      curlx_free(tmp_വരി);

      എങ്കിൽ(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        തിരികെ SSH_NO_ERROR;
      }
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        തിരികെ SSH_ERROR;
      }

      എങ്കിൽ((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        എങ്കിൽ(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          തിരികെ SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        തിരികെ SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      തിരികെ SSH_NO_ERROR;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  അല്ലെങ്കിൽ {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    തിരികെ myssh_to_SFTP_CLOSE(data, sshc);
  }
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_READDIR_LINK(ഘടന Curl_easy *data,
                                      ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  എങ്കിൽ(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    തിരികെ myssh_to_SFTP_CLOSE(data, sshc);
  }

  എങ്കിൽ(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    എങ്കിൽ(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    അല്ലെങ്കിൽ
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = ശൂന്യ;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  അല്ലെങ്കിൽ {
    sshc->readdir_len = strlen(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  എങ്കിൽ(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    തിരികെ SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = ശൂന്യ;
  sshc->readdir_filename = ശൂന്യ;
  sshc->readdir_longentry = ശൂന്യ;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_READDIR_BOTTOM(ഘടന Curl_easy *data,
                                        ഘടന ssh_conn *sshc)
{
  CURLcode result;

  എങ്കിൽ(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  അല്ലെങ്കിൽ
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = ശൂന്യ;

  എങ്കിൽ(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_READDIR_DONE(ഘടന Curl_easy *data,
                                      ഘടന ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = ശൂന്യ;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം ശൂന്യം myssh_quote_error(ഘടന Curl_easy *data, ഘടന ssh_conn *sshc,
                              സ്ഥിരം അക്ഷരം *cmd)
{
  എങ്കിൽ(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE_STATVFS(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  എങ്കിൽ(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    തിരികെ SSH_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#അല്ലെങ്കിലെങ്കിൽ defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#അല്ലെങ്കിൽ
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    അക്ഷരം *tmp = curl_maprintf("statvfs:\n"
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

    എങ്കിൽ(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    എങ്കിൽ(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    എങ്കിൽ(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ SSH_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_auth_interactive(ഘടന connectdata *conn,
                                  ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc;
  പൂർണ്ണസംഖ്യ nprompts;

restart:
  തിരഞ്ഞെടുപ്പ്(sshc->kbd_state) {
  അവസ്ഥ 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ(rc == SSH_AUTH_AGAIN)
      തിരികെ SSH_AGAIN;

    എങ്കിൽ(rc != SSH_AUTH_INFO)
      തിരികെ SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    എങ്കിൽ(nprompts != 1)
      തിരികെ SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    എങ്കിൽ(rc < 0)
      തിരികെ SSH_ERROR;

    FALLTHROUGH();
  അവസ്ഥ 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ(rc == SSH_AUTH_AGAIN)
      തിരികെ SSH_AGAIN;
    അല്ലെങ്കിൽ എങ്കിൽ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    അല്ലെങ്കിൽ എങ്കിൽ(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      എങ്കിൽ(nprompts)
        തിരികെ SSH_ERROR;

      sshc->kbd_state = 2;
      പോകുക restart;
    }
    അല്ലെങ്കിൽ
      rc = SSH_ERROR;
    നിർത്തുക;
  അവസ്ഥ 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, ശൂന്യ, ശൂന്യ);
    എങ്കിൽ(rc == SSH_AUTH_AGAIN)
      തിരികെ SSH_AGAIN;
    അല്ലെങ്കിൽ എങ്കിൽ(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    അല്ലെങ്കിൽ
      rc = SSH_ERROR;

    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ SSH_ERROR;
  }

  sshc->kbd_state = 0;
  തിരികെ rc;
}

സ്ഥിതം ശൂന്യം myssh_state_init(ഘടന Curl_easy *data,
                             ഘടന ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#എങ്കിൽ 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

സ്ഥിതം ശൂന്യം myssh_block2waitfor(ഘടന connectdata *conn,
                                ഘടന ssh_conn *sshc,
                                bool block)
{
  (ശൂന്യം)conn;
  എങ്കിൽ(block) {
    പൂർണ്ണസംഖ്യ dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  അല്ലെങ്കിൽ
    sshc->waitfor = 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_S_STARTUP(ഘടന Curl_easy *data,
                              ഘടന ssh_conn *sshc)
{
  ഘടന connectdata *conn = data->conn;
  പൂർണ്ണസംഖ്യ rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  എങ്കിൽ(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_HOSTKEY);

  തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_AUTHLIST(ഘടന Curl_easy *data,
                             ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, ശൂന്യ);
  എങ്കിൽ(rc == SSH_AUTH_AGAIN)
    തിരികെ SSH_AGAIN;

  എങ്കിൽ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    തിരികെ rc;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    തിരികെ rc;
  }

  sshc->auth_methods =
    (unsigned പൂർണ്ണസംഖ്യ)ssh_userauth_list(sshc->ssh_session, ശൂന്യ);
  എങ്കിൽ(sshc->auth_methods)
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
  എങ്കിൽ((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_താക്കോൽ && sshc->priv_താക്കോൽ[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_താക്കോൽ_INIT);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  അല്ലെങ്കിൽ {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_AUTH_PKEY_INIT(ഘടന Curl_easy *data,
                                   ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc;
  എങ്കിൽ(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    തിരികെ rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  എങ്കിൽ(sshc->priv_താക്കോൽ && sshc->priv_താക്കോൽ[0]) {
    എങ്കിൽ(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, ശൂന്യ, sshc->pubkey);
      എങ്കിൽ(rc == SSH_AUTH_AGAIN)
        തിരികെ SSH_AGAIN;

      എങ്കിൽ(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        തിരികെ rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_താക്കോൽ,
                                     sshc->passphrase, ശൂന്യ,
                                     ശൂന്യ, &sshc->privkey);
    എങ്കിൽ(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_താക്കോൽ);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      തിരികെ rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  അല്ലെങ്കിൽ {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, ശൂന്യ,
                                     sshc->passphrase);
    എങ്കിൽ(rc == SSH_AUTH_AGAIN)
      തിരികെ SSH_AGAIN;

    എങ്കിൽ(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      തിരികെ rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_AUTH_PKEY(ഘടന Curl_easy *data,
                              ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = ssh_userauth_publickey(sshc->ssh_session, ശൂന്യ, sshc->privkey);
  എങ്കിൽ(rc == SSH_AUTH_AGAIN)
    തിരികെ SSH_AGAIN;
  അല്ലെങ്കിൽ എങ്കിൽ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    തിരികെ SSH_OK;
  }
  അല്ലെങ്കിൽ {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    തിരികെ myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_AUTH_GSSAPI(ഘടന Curl_easy *data,
                                ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc;
  എങ്കിൽ(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    തിരികെ myssh_to_താക്കോൽ_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  എങ്കിൽ(rc == SSH_AUTH_AGAIN)
    തിരികെ SSH_AGAIN;

  എങ്കിൽ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    തിരികെ SSH_OK;
  }

  തിരികെ myssh_to_താക്കോൽ_AUTH(data, sshc);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_AUTH_താക്കോൽ_INIT(ഘടന Curl_easy *data,
                                  ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_താക്കോൽ);
    തിരികെ SSH_NO_ERROR;
  }
  തിരികെ myssh_to_PASSWD_AUTH(data, sshc);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_AUTH_താക്കോൽ(ഘടന Curl_easy *data,
                             ഘടന ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  പൂർണ്ണസംഖ്യ rc = myssh_auth_interactive(data->conn, sshc);
  എങ്കിൽ(rc == SSH_AGAIN)
    തിരികെ rc;
  അല്ലെങ്കിൽ എങ്കിൽ(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    തിരികെ SSH_NO_ERROR;
  }
  അല്ലെങ്കിൽ
    തിരികെ myssh_to_PASSWD_AUTH(data, sshc);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_AUTH_PASS_INIT(ഘടന Curl_easy *data,
                                   ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_AUTH_PASS(ഘടന Curl_easy *data,
                              ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = ssh_userauth_password(sshc->ssh_session, ശൂന്യ,
                                 Curl_creds_passwd(data->conn->creds));
  എങ്കിൽ(rc == SSH_AUTH_AGAIN)
    തിരികെ SSH_AGAIN;
  അല്ലെങ്കിൽ എങ്കിൽ(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    തിരികെ SSH_NO_ERROR;
  }
  തിരികെ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_AUTH_DONE(ഘടന Curl_easy *data,
                              ഘടന ssh_conn *sshc)
{
  ഘടന connectdata *conn = data->conn;
  എങ്കിൽ(!sshc->authed) {
    failf(data, "Authentication failure");
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  എങ്കിൽ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    തിരികെ SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_UPLOAD_INIT(ഘടന Curl_easy *data,
                                ഘടന ssh_conn *sshc,
                                ഘടന SSHPROTO *sshp)
{
  പൂർണ്ണസംഖ്യ flags;
  പൂർണ്ണസംഖ്യ rc = 0;

  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  എങ്കിൽ(data->state.resume_from) {
    sftp_attributes attrs;

    എങ്കിൽ(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      എങ്കിൽ(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        എങ്കിൽ(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          തിരികെ rc;
        }
        data->state.resume_from = size;
      }
      അല്ലെങ്കിൽ {
        data->state.resume_from = 0;
      }
    }
  }

  എങ്കിൽ(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  അല്ലെങ്കിൽ
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  എങ്കിൽ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  എങ്കിൽ(!sshc->sftp_file) {
    പൂർണ്ണസംഖ്യ err = sftp_get_error(sshc->sftp_session);

    എങ്കിൽ((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      തിരികെ rc;
    }
    അല്ലെങ്കിൽ {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      തിരികെ rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  എങ്കിൽ(data->state.resume_from > 0 && !data->set.remote_append) {
    പൂർണ്ണസംഖ്യ seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    എങ്കിൽ(data->set.seek_func) {
      ഘടന Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    എങ്കിൽ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      എങ്കിൽ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        തിരികെ rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      ചെയ്യുക {
        അക്ഷരം scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)വലുപ്പം(scratch)) ?
          വലുപ്പം(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        എങ്കിൽ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          തിരികെ rc;
        }
      } ആകുമ്പോൾ(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    എങ്കിൽ(data->state.infilesize > 0) {
      എങ്കിൽ(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        തിരികെ myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    എങ്കിൽ(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      തിരികെ rc;
    }
  }
  എങ്കിൽ(data->state.infilesize > 0) {
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
#എങ്കിൽ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_DOWNLOAD_INIT(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc,
                                       ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  എങ്കിൽ(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  എങ്കിൽ(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    തിരികെ myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_DOWNLOAD_STAT(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc)
{
  curl_off_t size;
  പൂർണ്ണസംഖ്യ rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  എങ്കിൽ(!attrs ||
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
    എങ്കിൽ(attrs)
      sftp_attributes_free(attrs);
  }
  അല്ലെങ്കിൽ {
    size = attrs->size;

    sftp_attributes_free(attrs);

    എങ്കിൽ(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      തിരികെ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    എങ്കിൽ(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      എങ്കിൽ(result)
        തിരികെ myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      എങ്കിൽ(rc)
        തിരികെ myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  എങ്കിൽ(data->state.resume_from) {
    എങ്കിൽ(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      എങ്കിൽ(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        തിരികെ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        തിരികെ myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    എങ്കിൽ(rc)
      തിരികെ myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  എങ്കിൽ(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    തിരികെ rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_CLOSE(ഘടന Curl_easy *data,
                               ഘടന ssh_conn *sshc,
                               ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  എങ്കിൽ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ശൂന്യ;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  എങ്കിൽ(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_STOP);
  }
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_SHUTDOWN(ഘടന Curl_easy *data,
                                  ഘടന ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#എങ്കിൽ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  എങ്കിൽ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ശൂന്യ;
  }

  എങ്കിൽ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ശൂന്യ;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_INIT(ഘടന Curl_easy *data,
                              ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  എങ്കിൽ(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  എങ്കിൽ(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    തിരികെ myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_REALPATH(ഘടന Curl_easy *data,
                                  ഘടന ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  എങ്കിൽ(!sshc->homedir)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  എങ്കിൽ(!data->state.most_recent_ftp_entrypath)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE_INIT(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc,
                                    ഘടന SSHPROTO *sshp)
{
  CURLcode result;

  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  എങ്കിൽ(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_POSTQUOTE_INIT(ഘടന Curl_easy *data,
                                        ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_STOP);
  }
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ quote_error(ഘടന Curl_easy *data,
                       ഘടന ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE(ഘടന Curl_easy *data,
                               ഘടന ssh_conn *sshc,
                               ഘടന SSHPROTO *sshp)
{
  സ്ഥിരം അക്ഷരം *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  സ്ഥിരം അക്ഷരം *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  എങ്കിൽ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  എങ്കിൽ(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    അക്ഷരം *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    എങ്കിൽ(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      തിരികെ SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    എങ്കിൽ(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    അല്ലെങ്കിൽ
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    തിരികെ SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  എങ്കിൽ(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    തിരികെ SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    അല്ലെങ്കിൽ
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    തിരികെ SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  എങ്കിൽ(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      അല്ലെങ്കിൽ
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      തിരികെ SSH_NO_ERROR;
    }
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    sshc->quote_attrs = ശൂന്യ;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    തിരികെ SSH_NO_ERROR;
  }
  എങ്കിൽ(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      അല്ലെങ്കിൽ
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      തിരികെ SSH_NO_ERROR;
    }
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    തിരികെ SSH_NO_ERROR;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "mkdir ", 6)) {
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    തിരികെ SSH_NO_ERROR;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    എങ്കിൽ(result) {
      എങ്കിൽ(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      അല്ലെങ്കിൽ
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      തിരികെ SSH_NO_ERROR;
    }
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    തിരികെ SSH_NO_ERROR;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    തിരികെ SSH_NO_ERROR;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "rm ", 3)) {
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    തിരികെ SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "statvfs ", 8)) {
    എങ്കിൽ(*cp)
      തിരികെ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    തിരികെ SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_NEXT_QUOTE(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  എങ്കിൽ(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    അല്ലെങ്കിൽ {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE_STAT(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc)
{
  സ്ഥിരം അക്ഷരം *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  എങ്കിൽ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  എങ്കിൽ(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  എങ്കിൽ(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, ശൂന്യ);
    തിരികെ SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  എങ്കിൽ(!strncmp(cmd, "chgrp", 5)) {
    സ്ഥിരം അക്ഷരം *p = sshc->quote_path1;
    curl_off_t gid;
    എങ്കിൽ(curlx_str_സംഖ്യ(&p, &gid, UINT_MAX)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, ശൂന്യ);
      തിരികെ SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    സ്ഥിരം അക്ഷരം *p = sshc->quote_path1;
    എങ്കിൽ(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, ശൂന്യ);
      തിരികെ SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "chown", 5)) {
    സ്ഥിരം അക്ഷരം *p = sshc->quote_path1;
    curl_off_t uid;
    എങ്കിൽ(curlx_str_സംഖ്യ(&p, &uid, UINT_MAX)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, ശൂന്യ);
      തിരികെ SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    എങ്കിൽ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#എങ്കിൽ SIZEOF_TIME_T > 4
    അല്ലെങ്കിൽ എങ്കിൽ(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    എങ്കിൽ(fail) {
      myssh_quote_error(data, sshc, ശൂന്യ);
      തിരികെ SSH_NO_ERROR;
    }
    എങ്കിൽ(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    അല്ലെങ്കിൽ /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം ശൂന്യം conn_forget_socket(ഘടന Curl_easy *data, int8_t sockindex)
{
  ഘടന connectdata *conn = data->conn;
  എങ്കിൽ(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    ഘടന Curl_cfilter *cf = conn->cfilter[sockindex];
    എങ്കിൽ(cf)
      (ശൂന്യം)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, ശൂന്യ);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

സ്ഥിതം ശൂന്യം myssh_SESSION_DISCONNECT(ഘടന Curl_easy *data,
                                     ഘടന ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  എങ്കിൽ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ശൂന്യ;
  }

  എങ്കിൽ(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = ശൂന്യ;
  }
  എങ്കിൽ(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = ശൂന്യ;
  }

  ssh_disconnect(sshc->ssh_session);
  എങ്കിൽ(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_SSH_SCP_DOWNLOAD(ഘടന Curl_easy *data,
                                  ഘടന ssh_conn *sshc)
{
  uint64_t bytecount;
  പൂർണ്ണസംഖ്യ rc = ssh_scp_pull_request(sshc->scp_session);
  എങ്കിൽ(rc != SSH_SCP_REQUEST_NEWFILE) {
    സ്ഥിരം അക്ഷരം *err_സന്ദേശം = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_സന്ദേശം);
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  എങ്കിൽ(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_TRANS_INIT(ഘടന Curl_easy *data, ഘടന ssh_conn *sshc,
                               ഘടന SSHPROTO *sshp)
{
  CURLcode result;
  പൂർണ്ണസംഖ്യ rc = 0;
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  എങ്കിൽ(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    തിരികെ 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  എങ്കിൽ(data->state.upload) {
    എങ്കിൽ(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      തിരികെ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  അല്ലെങ്കിൽ {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  എങ്കിൽ(!sshc->scp_session) {
    സ്ഥിരം അക്ഷരം *err_സന്ദേശം = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_സന്ദേശം);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  തിരികെ rc;
}

സ്ഥിതം ശൂന്യം sshc_cleanup(ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->initialized) {
    എങ്കിൽ(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = ശൂന്യ;
    }
    എങ്കിൽ(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = ശൂന്യ;
    }
    എങ്കിൽ(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = ശൂന്യ;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    എങ്കിൽ(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = ശൂന്യ;
    }
    എങ്കിൽ(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = ശൂന്യ;
    }
    എങ്കിൽ(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = ശൂന്യ;
    }
    എങ്കിൽ(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = ശൂന്യ;
    }
    എങ്കിൽ(sshc->privkey) {
      ssh_താക്കോൽ_free(sshc->privkey);
      sshc->privkey = ശൂന്യ;
    }
    എങ്കിൽ(sshc->pubkey) {
      ssh_താക്കോൽ_free(sshc->pubkey);
      sshc->pubkey = ശൂന്യ;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_താക്കോൽ);
    curlx_safefree(sshc->priv_താക്കോൽ);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE_SETSTAT(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  എങ്കിൽ(rc == SSH_AGAIN)
    തിരികെ rc;
  എങ്കിൽ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    തിരികെ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE_SYMLINK(ഘടന Curl_easy *data,
                                       ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  എങ്കിൽ(rc == SSH_AGAIN)
    തിരികെ rc;
  എങ്കിൽ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    തിരികെ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE_MKDIR(ഘടന Curl_easy *data,
                                     ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  എങ്കിൽ(rc == SSH_AGAIN)
    തിരികെ rc;
  എങ്കിൽ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    തിരികെ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE_RENAME(ഘടന Curl_easy *data,
                                      ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  എങ്കിൽ(rc == SSH_AGAIN)
    തിരികെ rc;
  എങ്കിൽ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    തിരികെ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE_RMDIR(ഘടന Curl_easy *data,
                                     ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  എങ്കിൽ(rc == SSH_AGAIN)
    തിരികെ rc;
  എങ്കിൽ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    തിരികെ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_QUOTE_UNLINK(ഘടന Curl_easy *data,
                                      ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  എങ്കിൽ(rc == SSH_AGAIN)
    തിരികെ rc;
  എങ്കിൽ(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    തിരികെ rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_GETINFO(ഘടന Curl_easy *data,
                                 ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_FILETIME(ഘടന Curl_easy *data,
                                  ഘടന ssh_conn *sshc,
                                  ഘടന SSHPROTO *sshp)
{
  sftp_attributes attrs;
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  എങ്കിൽ(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_TRANS_INIT(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc,
                                    ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  എങ്കിൽ(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  അല്ലെങ്കിൽ {
    size_t path_len = strlen(sshp->path);

    എങ്കിൽ(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    അല്ലെങ്കിൽ
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_CREATE_DIRS_INIT(ഘടന Curl_easy *data,
                                          ഘടന ssh_conn *sshc,
                                          ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  എങ്കിൽ(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  അല്ലെങ്കിൽ {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_CREATE_DIRS(ഘടന Curl_easy *data,
                                     ഘടന ssh_conn *sshc,
                                     ഘടന SSHPROTO *sshp)
{
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  എങ്കിൽ(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    തിരികെ SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SFTP_CREATE_DIRS_MKDIR(ഘടന Curl_easy *data,
                                           ഘടന ssh_conn *sshc,
                                           ഘടന SSHPROTO *sshp)
{
  പൂർണ്ണസംഖ്യ rc;
  പൂർണ്ണസംഖ്യ err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  എങ്കിൽ(rc == SSH_AGAIN)
    തിരികെ rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  എങ്കിൽ(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    എങ്കിൽ((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      തിരികെ myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  തിരികെ rc;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SCP_UPLOAD_INIT(ഘടന Curl_easy *data,
                                    ഘടന ssh_conn *sshc,
                                    ഘടന SSHPROTO *sshp)
{
  പൂർണ്ണസംഖ്യ rc;
  എങ്കിൽ(!sshp)
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  എങ്കിൽ(rc != SSH_OK) {
    സ്ഥിരം അക്ഷരം *err_സന്ദേശം = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_സന്ദേശം);
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_തള്ളുക_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (പൂർണ്ണസംഖ്യ)data->set.new_file_perms);

  എങ്കിൽ(rc != SSH_OK) {
    സ്ഥിരം അക്ഷരം *err_സന്ദേശം = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_സന്ദേശം);
    തിരികെ myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SCP_DOWNLOAD_INIT(ഘടന Curl_easy *data,
                                      ഘടന ssh_conn *sshc)
{
  പൂർണ്ണസംഖ്യ rc = ssh_scp_init(sshc->scp_session);
  എങ്കിൽ(rc != SSH_OK) {
    സ്ഥിരം അക്ഷരം *err_സന്ദേശം = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_സന്ദേശം);
    തിരികെ myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SCP_DONE(ഘടന Curl_easy *data,
                             ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  അല്ലെങ്കിൽ
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SCP_SEND_EOF(ഘടന Curl_easy *data,
                                 ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->scp_session) {
    പൂർണ്ണസംഖ്യ rc = ssh_scp_close(sshc->scp_session);
    എങ്കിൽ(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      തിരികെ SSH_AGAIN;
    }
    എങ്കിൽ(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ myssh_in_SCP_CHANNEL_FREE(ഘടന Curl_easy *data,
                                     ഘടന ssh_conn *sshc)
{
  എങ്കിൽ(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = ശൂന്യ;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  തിരികെ SSH_NO_ERROR;
}

സ്ഥിതം CURLcode myssh_in_SESSION_FREE(ഘടന Curl_easy *data,
                                      ഘടന ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  memset(sshc, 0, വലുപ്പം(ഘടന ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  തിരികെ result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
സ്ഥിതം CURLcode myssh_statemachine(ഘടന Curl_easy *data,
                                   ഘടന ssh_conn *sshc,
                                   ഘടന SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  ചെയ്യുക {

    തിരഞ്ഞെടുപ്പ്(sshc->state) {
    അവസ്ഥ SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    അവസ്ഥ SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      എങ്കിൽ(rc)
        നിർത്തുക;
      FALLTHROUGH();
    അവസ്ഥ SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      എങ്കിൽ(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        നിർത്തുക;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    അവസ്ഥ SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_AUTH_താക്കോൽ_INIT:
      rc = myssh_in_AUTH_താക്കോൽ_INIT(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_AUTH_താക്കോൽ:
      rc = myssh_in_AUTH_താക്കോൽ(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      നിർത്തുക;
    അവസ്ഥ SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      എങ്കിൽ(rc)
        നിർത്തുക;
      FALLTHROUGH();
    അവസ്ഥ SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    അവസ്ഥ SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    അവസ്ഥ SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      നിർത്തുക;
    അവസ്ഥ SSH_QUIT:
    സ്ഥിരസ്ഥിതി:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      നിർത്തുക;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } ആകുമ്പോൾ((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  എങ്കിൽ(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  എങ്കിൽ(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (പൂർണ്ണസംഖ്യ)result, *block);
  തിരികെ result;
}

/* called repeatedly until done from multi.c */
സ്ഥിതം CURLcode myssh_multi_statemach(ഘടന Curl_easy *data,
                                      bool *done)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ഘടന SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  എങ്കിൽ(!sshc || !sshp)
    തിരികെ CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  തിരികെ result;
}

സ്ഥിതം CURLcode myssh_block_statemach(ഘടന Curl_easy *data,
                                      ഘടന ssh_conn *sshc,
                                      ഘടന SSHPROTO *sshp,
                                      bool disconnect)
{
  ഘടന connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  ആകുമ്പോൾ((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t ഇടത്_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
    എങ്കിൽ(result)
      നിർത്തുക;

    എങ്കിൽ(!disconnect) {
      result = Curl_pgrsCheck(data);
      എങ്കിൽ(result)
        നിർത്തുക;

      ഇടത്_ms = Curl_timeleft_ms(data);
      എങ്കിൽ(ഇടത്_ms < 0) {
        failf(data, "Operation timed out");
        തിരികെ CURLE_OPERATION_TIMEDOUT;
      }
    }

    എങ്കിൽ(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (ശൂന്യം)SOCKET_READABLE(fd_read,
                            (!ഇടത്_ms || ഇടത്_ms > 1000) ? 1000 : ഇടത്_ms);
    }
  }

  തിരികെ result;
}

സ്ഥിതം ശൂന്യം myssh_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന SSHPROTO *sshp = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

സ്ഥിതം ശൂന്യം myssh_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന ssh_conn *sshc = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
സ്ഥിതം CURLcode myssh_setup_connection(ഘടന Curl_easy *data,
                                       ഘടന connectdata *conn)
{
  ഘടന SSHPROTO *sshp;
  ഘടന ssh_conn *sshc;

  sshc = curlx_calloc(1, വലുപ്പം(*sshc));
  എങ്കിൽ(!sshc)
    തിരികെ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  എങ്കിൽ(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, വലുപ്പം(*sshp));
  എങ്കിൽ(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    തിരികെ CURLE_OUT_OF_MEMORY;

  തിരികെ Curl_ssh_setup_pkey(data, sshc);
}

സ്ഥിതം Curl_recv scp_recv, sftp_recv;
സ്ഥിതം Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
സ്ഥിതം CURLcode myssh_connect(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result;
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ഘടന SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!sshc || !ssh)
    തിരികെ CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  എങ്കിൽ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  അല്ലെങ്കിൽ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  എങ്കിൽ(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    തിരികെ CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  എങ്കിൽ(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    തിരികെ CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, ശൂന്യ);
  എങ്കിൽ(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  എങ്കിൽ(rc != SSH_OK) {
    failf(data, "Could not set socket");
    തിരികെ CURLE_FAILED_INIT;
  }

  എങ്കിൽ(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    എങ്കിൽ(rc != SSH_OK) {
      failf(data, "Could not set user");
      തിരികെ CURLE_FAILED_INIT;
    }
  }

  എങ്കിൽ(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    എങ്കിൽ(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    എങ്കിൽ(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      തിരികെ CURLE_FAILED_INIT;
    }
  }

  എങ്കിൽ(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    എങ്കിൽ(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      തിരികെ CURLE_FAILED_INIT;
    }
  }

  എങ്കിൽ(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    എങ്കിൽ(rc != SSH_OK) {
      failf(data, "Could not set compression");
      തിരികെ CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = ശൂന്യ;
  sshc->pubkey = ശൂന്യ;

  എങ്കിൽ(sshc->pub_താക്കോൽ) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_താക്കോൽ, &sshc->pubkey);
    എങ്കിൽ(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      തിരികെ CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  തിരികെ result;
}

/* called from multi.c while DOing */
സ്ഥിതം CURLcode scp_doing(ഘടന Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  എങ്കിൽ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  തിരികെ result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

സ്ഥിതം CURLcode scp_perform(ഘടന Curl_easy *data,
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  എങ്കിൽ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  തിരികെ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
സ്ഥിതം CURLcode scp_disconnect(ഘടന Curl_easy *data,
                               ഘടന connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ഘടന SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (ശൂന്യം)dead_connection;

  എങ്കിൽ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  തിരികെ result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
സ്ഥിതം CURLcode myssh_done(ഘടന Curl_easy *data,
                           ഘടന ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  ഘടന SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  എങ്കിൽ(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
  അല്ലെങ്കിൽ
    result = status;

  എങ്കിൽ(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    തിരികെ CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  തിരികെ result;
}

സ്ഥിതം CURLcode scp_done(ഘടന Curl_easy *data, CURLcode status,
                         bool premature)
{
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (ശൂന്യം)premature;

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  തിരികെ myssh_done(data, sshc, status);
}

സ്ഥിതം CURLcode scp_send(ഘടന Curl_easy *data, int8_t sockindex,
                         സ്ഥിരം uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  പൂർണ്ണസംഖ്യ rc;
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (ശൂന്യം)sockindex; /* we only support SCP on the fixed known primary socket */
  (ശൂന്യം)eos;
  *pnwritten = 0;

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#എങ്കിൽ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  എങ്കിൽ(rc == SSH_AGAIN)
    തിരികെ CURLE_AGAIN;
  അല്ലെങ്കിൽ
#endif
  എങ്കിൽ(rc != SSH_OK)
    തിരികെ CURLE_SSH;

  *pnwritten = len;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode scp_recv(ഘടന Curl_easy *data, int8_t sockindex,
                         അക്ഷരം *mem, size_t len, size_t *pnread)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  പൂർണ്ണസംഖ്യ nread;

  (ശൂന്യം)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  എങ്കിൽ(nread == SSH_ERROR)
    തിരികെ CURLE_SSH;
#എങ്കിൽ 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  എങ്കിൽ(nread == SSH_AGAIN)
    തിരികെ CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  തിരികെ CURLE_OK;
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

സ്ഥിതം CURLcode sftp_perform(ഘടന Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  എങ്കിൽ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  തിരികെ result;
}

/* called from multi.c while DOing */
സ്ഥിതം CURLcode sftp_doing(ഘടന Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  എങ്കിൽ(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  തിരികെ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
സ്ഥിതം CURLcode sftp_disconnect(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn,
                                bool dead_connection)
{
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ഘടന SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (ശൂന്യം)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  എങ്കിൽ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
  തിരികെ result;
}

സ്ഥിതം CURLcode sftp_done(ഘടന Curl_easy *data, CURLcode status,
                          bool premature)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;
  എങ്കിൽ(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    എങ്കിൽ(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  തിരികെ myssh_done(data, sshc, status);
}

/* return number of sent bytes */
സ്ഥിതം CURLcode sftp_send(ഘടന Curl_easy *data, int8_t sockindex,
                          സ്ഥിരം uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (ശൂന്യം)sockindex;
  (ശൂന്യം)eos;
  *pnwritten = 0;

  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

#എങ്കിൽ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  തിരഞ്ഞെടുപ്പ്(sshc->sftp_send_state) {
  അവസ്ഥ 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    എങ്കിൽ(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      തിരികെ CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  അവസ്ഥ 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    എങ്കിൽ(nwrite == SSH_AGAIN)
      തിരികെ CURLE_AGAIN;
    അല്ലെങ്കിൽ എങ്കിൽ(nwrite < 0)
      തിരികെ CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    തിരികെ CURLE_OK;
  സ്ഥിരസ്ഥിതി:
    /* we never reach here */
    തിരികെ CURLE_SEND_ERROR;
  }
#അല്ലെങ്കിൽ
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  എങ്കിൽ(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#എങ്കിൽ 0 /* not returned by libssh on write */
  എങ്കിൽ(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  അല്ലെങ്കിൽ
#endif
  എങ്കിൽ(nwrite < 0)
    തിരികെ CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  തിരികെ CURLE_OK;
#endif
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
സ്ഥിതം CURLcode sftp_recv(ഘടന Curl_easy *data, int8_t sockindex,
                          അക്ഷരം *mem, size_t len, size_t *pnread)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (ശൂന്യം)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  തിരഞ്ഞെടുപ്പ്(sshc->sftp_recv_state) {
  അവസ്ഥ 0:
#എങ്കിൽ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    എങ്കിൽ(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      തിരികെ CURLE_RECV_ERROR;
    }
#അല്ലെങ്കിൽ
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    എങ്കിൽ(sshc->sftp_file_index < 0)
      തിരികെ CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  അവസ്ഥ 1:
    sshc->sftp_recv_state = 1;

#എങ്കിൽ LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#അല്ലെങ്കിൽ
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    എങ്കിൽ(nread == SSH_AGAIN)
      തിരികെ CURLE_AGAIN;
    അല്ലെങ്കിൽ എങ്കിൽ(nread < 0)
      തിരികെ CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    തിരികെ CURLE_OK;

  സ്ഥിരസ്ഥിതി:
    /* we never reach here */
    തിരികെ CURLE_RECV_ERROR;
  }
}

സ്ഥിതം CURLcode myssh_do_it(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  എങ്കിൽ(!sshc)
    തിരികെ CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  എങ്കിൽ(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  അല്ലെങ്കിൽ
    result = sftp_perform(data, &connected, done);

  തിരികെ result;
}

CURLcode Curl_ssh_init(ശൂന്യം)
{
  എങ്കിൽ(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    തിരികെ CURLE_FAILED_INIT;
  }
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_ssh_cleanup(ശൂന്യം)
{
  (ശൂന്യം)ssh_finalize();
}

ശൂന്യം Curl_ssh_version(അക്ഷരം *buffer, size_t buflen)
{
  (ശൂന്യം)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_scp = {
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
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_sftp = {
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
