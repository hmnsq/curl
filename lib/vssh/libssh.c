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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_LIBSSH

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ __VMS
#شامل <in.h>
#شامل <inet.h>
#اگر_ختم

#شامل "urldata.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "transfer.h"
#شامل "vssh/ssh.h"
#شامل "url.h"
#شامل "cfilters.h"
#شامل "connect.h"
#شامل "parsedate.h"          /* for the week day and month names */
#شامل "curlx/strparse.h"
#شامل "multiif.h"
#شامل "select.h"
#شامل "vssh/vssh.h"
#شامل "curlx/base64.h" /* for curlx_base64_encode() */

#اگر_تعریف_شدہ HAVE_UNISTD_H
#شامل <unistd.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_FCNTL_H
#شامل <fcntl.h>
#اگر_ختم

/* A recent macro provided by libssh. Or make our own. */
#اگر_غیر_تعریف_شدہ SSH_STRING_FREE_CHAR
#تعریف_کرو SSH_STRING_FREE_CHAR(x) \
  کرو {                          \
    اگر(x) {                     \
      ssh_string_free_char(x);  \
      x = تہی;                 \
    }                           \
  } جب(0)
#اگر_ختم

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#اگر_غیر_تعریف_شدہ SSH_S_IFMT
#تعریف_کرو SSH_S_IFMT   00170000
#اگر_ختم
#اگر_غیر_تعریف_شدہ SSH_S_IFLNK
#تعریف_کرو SSH_S_IFLNK  0120000
#اگر_ختم

ساکن CURLcode sftp_error_مقصد_CURLE(صحیح err)
{
  انتخاب(err) {
  حالت SSH_FX_OK:
    واپس CURLE_OK;

  حالت SSH_FX_NO_SUCH_FILE:
  حالت SSH_FX_NO_SUCH_راستہ:
    واپس CURLE_REMOTE_FILE_NOT_ملا;

  حالت SSH_FX_PERMISSION_DENIED:
  حالت SSH_FX_WRITE_PROTECT:
    واپس CURLE_REMOTE_ACCESS_DENIED;

  حالت SSH_FX_FILE_ALREADY_EXISTS:
    واپس CURLE_REMOTE_FILE_EXISTS;

  طےشدہ:
    توڑو;
  }

  واپس CURLE_SSH;
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
ساکن صحیح myssh_is_معلوم(ساخت Curl_easy *ڈیٹا, ساخت ssh_conn *sshc)
{
  صحیح rc;
  ssh_کلید pubkey;
  غیرعلامتی حرف *hash_sha256 = تہی;
  size_t hlen_sha256;
  غیرعلامتی حرف *hash_md5 = تہی;
  size_t hlen_md5;
  حرف *ملا_base64 = تہی;
  حرف *معلوم_base64 = تہی;
  صحیح vstate;
  شمار curl_khmatch keymatch;
  ساخت curl_khkey foundkey;
  ساخت curl_khkey *knownkeyp = تہی;
  curl_sshkeycallback func = ڈیٹا->سیٹ.ssh_keyfunc;
  ساخت ssh_knownhosts_اندراج *knownhostsentry = تہی;
  ساخت curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  اگر(rc != SSH_OK)
    واپس rc;

  اگر(CURL_EASY_STR(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_SHA256)) {
    ثابت حرف *pubkey_sha256 =
      CURL_EASY_STR(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_SHA256);
    حرف *fingerprint_b64 = تہی;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    اگر(rc != SSH_OK || hlen_sha256 != 32) {
      failf(ڈیٹا, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      جاؤ cleanup;
    }

    اگر(curlx_base64_encode((ثابت uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      جاؤ cleanup;
    }

    infof(ڈیٹا, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    جب((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    جب((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    اگر((pub_pos != b64_pos) ||
       لڑی_موازنہ(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(ڈیٹا,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      جاؤ cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    جاؤ cleanup;
  }

  اگر(CURL_EASY_STR(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_MD5)) {
    ثابت حرف *pubkey_md5 =
      CURL_EASY_STR(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_MD5);
    حرف md5buffer[33];
    صحیح i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    اگر(rc != SSH_OK || hlen_md5 != 16) {
      failf(ڈیٹا,
            "Denied establishing ssh session: MD5 fingerprint not available");
      جاؤ cleanup;
    }

    برائے(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(ڈیٹا, "SSH MD5 fingerprint: %s", md5buffer);

    اگر(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(ڈیٹا,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      جاؤ cleanup;
    }

    rc = SSH_OK;
    جاؤ cleanup;
  }

  اگر(CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_معلوم_hosts_اندراج(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    اگر(knownhostsentry) {
      اگر(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &معلوم_base64);
        اگر(rc != SSH_OK) {
          جاؤ cleanup;
        }
        knownkey.کلید = معلوم_base64;
        knownkey.len = لڑی_لمبائی(معلوم_base64);

        انتخاب(ssh_کلید_type(knownhostsentry->publickey)) {
        حالت SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          توڑو;
        حالت SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          توڑو;
        حالت SSH_KEYTYPE_ECDSA:
        حالت SSH_KEYTYPE_ECDSA_P256:
        حالت SSH_KEYTYPE_ECDSA_P384:
        حالت SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          توڑو;
        حالت SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          توڑو;
        حالت SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          توڑو;
        طےشدہ:
          rc = SSH_ERROR;
          جاؤ cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    انتخاب(vstate) {
    حالت SSH_معلوم_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      توڑو;
    حالت SSH_معلوم_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      توڑو;
    حالت SSH_معلوم_HOSTS_NOT_ملا:
    حالت SSH_معلوم_HOSTS_UNKNOWN:
    حالت SSH_معلوم_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      توڑو;
    طےشدہ:
      keymatch = CURLKHMATCH_MISMATCH;
      توڑو;
    }

    اگر(func) { /* use callback to determine action */
      ساخت Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &ملا_base64);
      اگر(rc != SSH_OK)
        جاؤ cleanup;

      foundkey.کلید = ملا_base64;
      foundkey.len = لڑی_لمبائی(ملا_base64);

      انتخاب(ssh_کلید_type(pubkey)) {
      حالت SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        توڑو;
      حالت SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        توڑو;
      حالت SSH_KEYTYPE_ECDSA:
      حالت SSH_KEYTYPE_ECDSA_P256:
      حالت SSH_KEYTYPE_ECDSA_P384:
      حالت SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        توڑو;
      حالت SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        توڑو;
      حالت SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        توڑو;
      طےشدہ:
        rc = SSH_ERROR;
        جاؤ cleanup;
      }

      CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_ssh_keyfunc);
      rc = func(ڈیٹا, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, ڈیٹا->سیٹ.ssh_keyfunc_userp);
      CURL_CBAPI_انتہا(&guard);

      انتخاب(rc) {
      حالت CURLKHSTAT_FINE_جوڑو_مقصد_FILE:
        rc = ssh_session_update_معلوم_hosts(sshc->ssh_session);
        اگر(rc != SSH_OK) {
          جاؤ cleanup;
        }
        توڑو;
      حالت CURLKHSTAT_FINE:
        توڑو;
      طےشدہ: /* REJECT/DEFER */
        rc = SSH_ERROR;
        جاؤ cleanup;
      }
    }
    ورنہ {
      اگر(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        جاؤ cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  اگر(ملا_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    آزاد(ملا_base64); /* allocated by libssh, deallocate with system free */
  }
  اگر(معلوم_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    آزاد(معلوم_base64); /* allocated by libssh, deallocate with system free */
  }
  اگر(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  اگر(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_کلید_free(pubkey);
  اگر(knownhostsentry) {
    ssh_knownhosts_اندراج_free(knownhostsentry);
  }
  واپس rc;
}

ساکن صحیح myssh_مقصد_ERROR(ساخت Curl_easy *ڈیٹا,
                          ساخت ssh_conn *sshc,
                          CURLcode نتیجہ)
{
  myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = نتیجہ;
  واپس SSH_ERROR;
}

ساکن صحیح myssh_مقصد_SFTP_CLOSE(ساخت Curl_easy *ڈیٹا,
                               ساخت ssh_conn *sshc)
{
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_مقصد_CURLE(sftp_get_error(sshc->sftp_session));
  واپس SSH_ERROR;
}

ساکن صحیح myssh_مقصد_PASSWD_AUTH(ساخت Curl_easy *ڈیٹا,
                                ساخت ssh_conn *sshc)
{
  اگر(sshc->auth_methods & SSH_AUTH_طریقہ_PASSWORD) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PASS_INIT);
    واپس SSH_OK;
  }
  واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_LOGIN_DENIED);
}

ساکن صحیح myssh_مقصد_کلید_AUTH(ساخت Curl_easy *ڈیٹا,
                             ساخت ssh_conn *sshc)
{
  اگر(sshc->auth_methods & SSH_AUTH_طریقہ_INTERACTIVE) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_کلید_INIT);
    واپس SSH_OK;
  }
  واپس myssh_مقصد_PASSWD_AUTH(ڈیٹا, sshc);
}

ساکن صحیح myssh_مقصد_GSSAPI_AUTH(ساخت Curl_easy *ڈیٹا,
                                ساخت ssh_conn *sshc)
{
  اگر(sshc->auth_methods & SSH_AUTH_طریقہ_GSSAPI_MIC) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_GSSAPI);
    واپس SSH_OK;
  }
  واپس myssh_مقصد_کلید_AUTH(ڈیٹا, sshc);
}

ساکن صحیح myssh_اندر_SFTP_READDIR_INIT(ساخت Curl_easy *ڈیٹا,
                                      ساخت ssh_conn *sshc,
                                      ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);
  Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, -1);
  اگر(ڈیٹا->req.no_body) {
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
    واپس SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->راستہ);
  اگر(!sshc->sftp_dir) {
    failf(ڈیٹا, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    واپس myssh_مقصد_SFTP_CLOSE(ڈیٹا, sshc);
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_READDIR(ساخت Curl_easy *ڈیٹا,
                                 ساخت ssh_conn *sshc,
                                 ساخت SSHPROTO *sshp)
{
  CURLcode نتیجہ = CURLE_OK;

  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  اگر(sshc->readdir_attrs)
    sftp_اوصاف_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  اگر(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = لڑی_لمبائی(sshc->readdir_filename);

    اگر(ڈیٹا->سیٹ.list_only) {
      حرف *tmp_سطر;

      tmp_سطر = curl_maprintf("%s\n", sshc->readdir_filename);
      اگر(!tmp_سطر) {
        myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_برآمد_کا_حافظہ;
        واپس SSH_ERROR;
      }
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                                 tmp_سطر, sshc->readdir_len + 1);
      curlx_free(tmp_سطر);

      اگر(نتیجہ) {
        myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
        sshc->actualcode = نتیجہ;
        واپس SSH_NO_ERROR;
      }
    }
    ورنہ {
      اگر(curlx_dyn_جوڑو(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_برآمد_کا_حافظہ;
        myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
        واپس SSH_ERROR;
      }

      اگر((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_link_راستہ = curl_maprintf("%s%s", sshp->راستہ,
                                               sshc->readdir_filename);

        اگر(!sshc->readdir_link_راستہ) {
          myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_برآمد_کا_حافظہ;
          واپس SSH_ERROR;
        }

        myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_LINK);
        واپس SSH_NO_ERROR;
      }
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_BOTTOM);
      واپس SSH_NO_ERROR;
    }
  }
  ورنہ اگر(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_مکمل);
  }
  ورنہ {
    failf(ڈیٹا, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    واپس myssh_مقصد_SFTP_CLOSE(ڈیٹا, sshc);
  }
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_READDIR_LINK(ساخت Curl_easy *ڈیٹا,
                                      ساخت ssh_conn *sshc)
{
  اگر(sshc->readdir_link_attrs)
    sftp_اوصاف_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_link_راستہ);
  اگر(!sshc->readdir_link_attrs) {
    failf(ڈیٹا, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    واپس myssh_مقصد_SFTP_CLOSE(ڈیٹا, sshc);
  }

  اگر(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_link_راستہ);
    اگر(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    ورنہ
      sshc->readdir_len = لڑی_لمبائی(sshc->readdir_tmp);
    sshc->readdir_longentry = تہی;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  ورنہ {
    sshc->readdir_len = لڑی_لمبائی(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_link_راستہ);

  اگر(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_برآمد_کا_حافظہ;
    واپس SSH_ERROR;
  }

  sftp_اوصاف_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = تہی;
  sshc->readdir_filename = تہی;
  sshc->readdir_longentry = تہی;

  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_BOTTOM);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_READDIR_BOTTOM(ساخت Curl_easy *ڈیٹا,
                                        ساخت ssh_conn *sshc)
{
  CURLcode نتیجہ;

  اگر(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    نتیجہ = CURLE_برآمد_کا_حافظہ;
  ورنہ
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = تہی;

  اگر(نتیجہ) {
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
    sshc->actualcode = نتیجہ;
  }
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_READDIR_مکمل(ساخت Curl_easy *ڈیٹا,
                                      ساخت ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = تہی;

  /* no data to transfer */
  Curl_xfer_تیاری_nop(ڈیٹا);
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس SSH_NO_ERROR;
}

ساکن خالی myssh_quote_error(ساخت Curl_easy *ڈیٹا, ساخت ssh_conn *sshc,
                              ثابت حرف *cmd)
{
  اگر(cmd)
    failf(ڈیٹا, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_QUOTE_STATVFS(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  اگر(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(ڈیٹا, sshc, "statvfs");
    واپس SSH_OK;
  }
  ورنہ اگر(statvfs) {
#اگر_تعریف_شدہ _MSC_VER
#تعریف_کرو CURL_LIBSSH_VFS_سائز_نقاب "I64u"
#وگرنہ_اگر defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#تعریف_کرو CURL_LIBSSH_VFS_سائز_نقاب "llu"
#ورنہ
#تعریف_کرو CURL_LIBSSH_VFS_سائز_نقاب PRIu64
#اگر_ختم
    CURLcode نتیجہ = CURLE_OK;
    حرف *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_files: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_سائز_نقاب "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_سائز_نقاب "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_بلاکیں, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    اگر(!tmp)
      نتیجہ = CURLE_برآمد_کا_حافظہ;

    اگر(!نتیجہ) {
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, tmp, لڑی_لمبائی(tmp));
      curlx_free(tmp);
    }
    اگر(نتیجہ) {
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجہ;
    }
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس SSH_OK;
}

ساکن صحیح myssh_auth_interactive(ساخت connectdata *conn,
                                  ساخت ssh_conn *sshc)
{
  صحیح rc;
  صحیح nprompts;

restart:
  انتخاب(sshc->kbd_state) {
  حالت 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, تہی, تہی);
    اگر(rc == SSH_AUTH_AGAIN)
      واپس SSH_AGAIN;

    اگر(rc != SSH_AUTH_INFO)
      واپس SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    اگر(nprompts != 1)
      واپس SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    اگر(rc < 0)
      واپس SSH_ERROR;

    FALLTHROUGH();
  حالت 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, تہی, تہی);
    اگر(rc == SSH_AUTH_AGAIN)
      واپس SSH_AGAIN;
    ورنہ اگر(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ورنہ اگر(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      اگر(nprompts)
        واپس SSH_ERROR;

      sshc->kbd_state = 2;
      جاؤ restart;
    }
    ورنہ
      rc = SSH_ERROR;
    توڑو;
  حالت 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, تہی, تہی);
    اگر(rc == SSH_AUTH_AGAIN)
      واپس SSH_AGAIN;
    ورنہ اگر(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    ورنہ
      rc = SSH_ERROR;

    توڑو;
  طےشدہ:
    واپس SSH_ERROR;
  }

  sshc->kbd_state = 0;
  واپس rc;
}

ساکن خالی myssh_state_init(ساخت Curl_easy *ڈیٹا,
                             ساخت ssh_conn *sshc)
{
  sshc->دوسرا_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#اگر 0
  ssh_سیٹ_log_level(SSH_LOG_PROTOCOL);
#اگر_ختم

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_سیٹ_blocking(sshc->ssh_session, 0);

  myssh_مقصد(ڈیٹا, sshc, SSH_S_STARTUP);
}

ساکن خالی myssh_block2waitfor(ساخت connectdata *conn,
                                ساخت ssh_conn *sshc,
                                bool بلاک)
{
  (خالی)conn;
  اگر(بلاک) {
    صحیح dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  ورنہ
    sshc->waitfor = 0;
}

ساکن صحیح myssh_اندر_S_STARTUP(ساخت Curl_easy *ڈیٹا,
                              ساخت ssh_conn *sshc)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  صحیح rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  اگر(rc == SSH_AGAIN) {
    CURL_TRC_SSH(ڈیٹا, "connect -> EAGAIN");
  }
  ورنہ اگر(rc != SSH_OK) {
    failf(ڈیٹا, "Failure establishing ssh session");
    rc = myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);
  }
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_HOSTKEY);

  واپس rc;
}

ساکن صحیح myssh_اندر_AUTHLIST(ساخت Curl_easy *ڈیٹا,
                             ساخت ssh_conn *sshc)
{
  صحیح rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, تہی);
  اگر(rc == SSH_AUTH_AGAIN)
    واپس SSH_AGAIN;

  اگر(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ڈیٹا, "Authenticated with none");
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
    واپس rc;
  }
  ورنہ اگر(rc == SSH_AUTH_ERROR) {
    rc = myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_LOGIN_DENIED);
    واپس rc;
  }

  sshc->auth_methods =
    (غیرعلامتی صحیح)ssh_userauth_list(sshc->ssh_session, تہی);
  اگر(sshc->auth_methods)
    infof(ڈیٹا, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_طریقہ_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_طریقہ_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_طریقہ_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_طریقہ_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  اگر((sshc->auth_methods & SSH_AUTH_طریقہ_PUBLICKEY) &&
     ((sshc->priv_کلید && sshc->priv_کلید[0]) ||
      (ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PKEY_INIT);
    infof(ڈیٹا, "Authentication using SSH public key file");
  }
  ورنہ اگر(sshc->auth_methods & SSH_AUTH_طریقہ_GSSAPI_MIC) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_GSSAPI);
  }
  ورنہ اگر(sshc->auth_methods & SSH_AUTH_طریقہ_INTERACTIVE) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_کلید_INIT);
  }
  ورنہ اگر(sshc->auth_methods & SSH_AUTH_طریقہ_PASSWORD) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PASS_INIT);
  }
  ورنہ {                  /* unsupported authentication method */
    rc = myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_LOGIN_DENIED);
  }
  واپس rc;
}

ساکن صحیح myssh_اندر_AUTH_PKEY_INIT(ساخت Curl_easy *ڈیٹا,
                                   ساخت ssh_conn *sshc)
{
  صحیح rc;
  اگر(!(ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_مقصد_GSSAPI_AUTH(ڈیٹا, sshc);
    واپس rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  اگر(sshc->priv_کلید && sshc->priv_کلید[0]) {
    اگر(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, تہی, sshc->pubkey);
      اگر(rc == SSH_AUTH_AGAIN)
        واپس SSH_AGAIN;

      اگر(rc != SSH_OK) {
        rc = myssh_مقصد_GSSAPI_AUTH(ڈیٹا, sshc);
        واپس rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_کلید,
                                     sshc->passphrase, تہی,
                                     تہی, &sshc->privkey);
    اگر(rc != SSH_OK) {
      failf(ڈیٹا, "Could not load private key file %s", sshc->priv_کلید);
      rc = myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_LOGIN_DENIED);
      واپس rc;
    }

    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PKEY);
  }
  ورنہ {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, تہی,
                                     sshc->passphrase);
    اگر(rc == SSH_AUTH_AGAIN)
      واپس SSH_AGAIN;

    اگر(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(ڈیٹا, "Completed public key authentication");
      myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
      واپس rc;
    }

    rc = myssh_مقصد_GSSAPI_AUTH(ڈیٹا, sshc);
  }
  واپس rc;
}

ساکن صحیح myssh_اندر_AUTH_PKEY(ساخت Curl_easy *ڈیٹا,
                              ساخت ssh_conn *sshc)
{
  صحیح rc = ssh_userauth_publickey(sshc->ssh_session, تہی, sshc->privkey);
  اگر(rc == SSH_AUTH_AGAIN)
    واپس SSH_AGAIN;
  ورنہ اگر(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ڈیٹا, "Completed public key authentication");
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
    واپس SSH_OK;
  }
  ورنہ {
    infof(ڈیٹا, "Failed public key authentication (rc: %d)", rc);
    واپس myssh_مقصد_GSSAPI_AUTH(ڈیٹا, sshc);
  }
}

ساکن صحیح myssh_اندر_AUTH_GSSAPI(ساخت Curl_easy *ڈیٹا,
                                ساخت ssh_conn *sshc)
{
  صحیح rc;
  اگر(!(ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    واپس myssh_مقصد_کلید_AUTH(ڈیٹا, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  اگر(rc == SSH_AUTH_AGAIN)
    واپس SSH_AGAIN;

  اگر(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ڈیٹا, "Completed gssapi authentication");
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
    واپس SSH_OK;
  }

  واپس myssh_مقصد_کلید_AUTH(ڈیٹا, sshc);
}

ساکن صحیح myssh_اندر_AUTH_کلید_INIT(ساخت Curl_easy *ڈیٹا,
                                  ساخت ssh_conn *sshc)
{
  اگر(ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_کلید);
    واپس SSH_NO_ERROR;
  }
  واپس myssh_مقصد_PASSWD_AUTH(ڈیٹا, sshc);
}

ساکن صحیح myssh_اندر_AUTH_کلید(ساخت Curl_easy *ڈیٹا,
                             ساخت ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  صحیح rc = myssh_auth_interactive(ڈیٹا->conn, sshc);
  اگر(rc == SSH_AGAIN)
    واپس rc;
  ورنہ اگر(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(ڈیٹا, "completed keyboard interactive authentication");
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
    واپس SSH_NO_ERROR;
  }
  ورنہ
    واپس myssh_مقصد_PASSWD_AUTH(ڈیٹا, sshc);
}

ساکن صحیح myssh_اندر_AUTH_PASS_INIT(ساخت Curl_easy *ڈیٹا,
                                   ساخت ssh_conn *sshc)
{
  اگر(!(ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_LOGIN_DENIED);
  myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PASS);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_AUTH_PASS(ساخت Curl_easy *ڈیٹا,
                              ساخت ssh_conn *sshc)
{
  صحیح rc = ssh_userauth_password(sshc->ssh_session, تہی,
                                 Curl_creds_passwd(ڈیٹا->conn->creds));
  اگر(rc == SSH_AUTH_AGAIN)
    واپس SSH_AGAIN;
  ورنہ اگر(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(ڈیٹا, "Completed password authentication");
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
    واپس SSH_NO_ERROR;
  }
  واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_LOGIN_DENIED);
}

ساکن صحیح myssh_اندر_AUTH_مکمل(ساخت Curl_easy *ڈیٹا,
                              ساخت ssh_conn *sshc)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  اگر(!sshc->authed) {
    failf(ڈیٹا, "Authentication failure");
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(ڈیٹا, "Authentication complete");
  Curl_pgrs_وقت(ڈیٹا, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  اگر(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_INIT);
    واپس SSH_NO_ERROR;
  }
  infof(ڈیٹا, "SSH CONNECT phase done");
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_UPLOAD_INIT(ساخت Curl_easy *ڈیٹا,
                                ساخت ssh_conn *sshc,
                                ساخت SSHPROTO *sshp)
{
  صحیح flags;
  صحیح rc = 0;

  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);

  اگر(ڈیٹا->state.resume_from) {
    sftp_اوصاف attrs;

    اگر(ڈیٹا->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->راستہ);
      اگر(attrs) {
        curl_off_t سائز = attrs->سائز;
        sftp_اوصاف_free(attrs);
        اگر(سائز < 0) {
          failf(ڈیٹا, "Bad file size (%" FMT_OFF_T ")", سائز);
          rc = myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          واپس rc;
        }
        ڈیٹا->state.resume_from = سائز;
      }
      ورنہ {
        ڈیٹا->state.resume_from = 0;
      }
    }
  }

  اگر(ڈیٹا->سیٹ.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  ورنہ اگر(ڈیٹا->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  ورنہ
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  اگر(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->راستہ,
              flags, (mode_t)ڈیٹا->سیٹ.new_file_perms);
  اگر(!sshc->sftp_file) {
    صحیح err = sftp_get_error(sshc->sftp_session);

    اگر((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_راستہ) &&
       ڈیٹا->سیٹ.ftp_create_missing_dirs &&
       (لڑی_لمبائی(sshp->راستہ) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->دوسرا_Create_Dirs = 1;
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      واپس rc;
    }
    ورنہ {
      rc = myssh_مقصد_SFTP_CLOSE(ڈیٹا, sshc);
      واپس rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  اگر(ڈیٹا->state.resume_from > 0 && !ڈیٹا->سیٹ.remote_append) {
    صحیح seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    اگر(ڈیٹا->سیٹ.seek_func) {
      ساخت Curl_mapi_guard guard;
      CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_seek_func);
      seekerr = ڈیٹا->سیٹ.seek_func(ڈیٹا->سیٹ.seek_client,
                                    ڈیٹا->state.resume_from, SEEK_SET);
      CURL_CBAPI_انتہا(&guard);
    }

    اگر(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      اگر(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(ڈیٹا, "Could not seek stream");
        rc = myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FTP_COULDNT_USE_REST);
        واپس rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      کرو {
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (ڈیٹا->state.resume_from - passed >
           (curl_off_t)حجم(scratch)) ?
          حجم(scratch) : curlx_sotouz(ڈیٹا->state.resume_from - passed);

        size_t actuallyread =
          ڈیٹا->state.fread_func(scratch, 1,
                                 readthisamountnow, ڈیٹا->state.اندر);

        passed += actuallyread;
        اگر((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(ڈیٹا, "Failed to read data");
          rc = myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FTP_COULDNT_USE_REST);
          واپس rc;
        }
      } جب(passed < ڈیٹا->state.resume_from);
    }

    /* now, decrease the size of the read */
    اگر(ڈیٹا->state.infilesize > 0) {
      اگر(ڈیٹا->state.resume_from > ڈیٹا->state.infilesize) {
        failf(ڈیٹا, "Resume point beyond size");
        واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_BAD_FUNCTION_دلیل);
      }
      ڈیٹا->state.infilesize -= ڈیٹا->state.resume_from;
      ڈیٹا->req.سائز = ڈیٹا->state.infilesize;
      Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, ڈیٹا->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, ڈیٹا->state.resume_from);
    اگر(rc) {
      rc = myssh_مقصد_SFTP_CLOSE(ڈیٹا, sshc);
      واپس rc;
    }
  }
  اگر(ڈیٹا->state.infilesize > 0) {
    ڈیٹا->req.سائز = ڈیٹا->state.infilesize;
    Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, ڈیٹا->state.infilesize);
  }
  /* upload data */
  Curl_xfer_تیاری_send(ڈیٹا, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ڈیٹا->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(ڈیٹا);
#اگر LIBSSH_نسخہ_INT > SSH_نسخہ_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#اگر_ختم
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس rc;
}

ساکن صحیح myssh_اندر_SFTP_DOWNLOAD_INIT(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc,
                                       ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  اگر(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->راستہ,
                              O_RDONLY, (mode_t)ڈیٹا->سیٹ.new_file_perms);
  اگر(!sshc->sftp_file) {
    failf(ڈیٹا, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    واپس myssh_مقصد_SFTP_CLOSE(ڈیٹا, sshc);
  }
  sftp_file_سیٹ_nonblocking(sshc->sftp_file);
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_DOWNLOAD_STAT);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_DOWNLOAD_STAT(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc)
{
  curl_off_t سائز;
  صحیح rc = 0;
  sftp_اوصاف attrs = sftp_fstat(sshc->sftp_file);
  اگر(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_سائز) ||
     (attrs->سائز == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    ڈیٹا->req.سائز = -1;
    ڈیٹا->req.maxdownload = -1;
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, -1);
    سائز = 0;
    اگر(attrs)
      sftp_اوصاف_free(attrs);
  }
  ورنہ {
    سائز = attrs->سائز;

    sftp_اوصاف_free(attrs);

    اگر(سائز < 0) {
      failf(ڈیٹا, "Bad file size (%" FMT_OFF_T ")", سائز);
      واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    اگر(ڈیٹا->state.use_range) {
      curl_off_t from;
      CURLcode نتیجہ = Curl_ssh_range(ڈیٹا, ڈیٹا->state.range, سائز,
                                       &from, &سائز);
      اگر(نتیجہ)
        واپس myssh_مقصد_ERROR(ڈیٹا, sshc, نتیجہ);

      rc = sftp_seek64(sshc->sftp_file, from);
      اگر(rc)
        واپس myssh_مقصد_SFTP_CLOSE(ڈیٹا, sshc);
    }
    ڈیٹا->req.سائز = سائز;
    ڈیٹا->req.maxdownload = سائز;
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, سائز);
  }

  /* We can resume if we can seek to the resume position */
  اگر(ڈیٹا->state.resume_from) {
    اگر(ڈیٹا->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      اگر(سائز < -ڈیٹا->state.resume_from) {
        failf(ڈیٹا, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", ڈیٹا->state.resume_from, سائز);
        واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      ڈیٹا->state.resume_from += سائز;
    }
    ورنہ {
      اگر(سائز < ڈیٹا->state.resume_from) {
        failf(ڈیٹا, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              ڈیٹا->state.resume_from, سائز);
        واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    ڈیٹا->req.سائز = سائز - ڈیٹا->state.resume_from;
    ڈیٹا->req.maxdownload = سائز - ڈیٹا->state.resume_from;
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, سائز - ڈیٹا->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, ڈیٹا->state.resume_from);
    اگر(rc)
      واپس myssh_مقصد_SFTP_CLOSE(ڈیٹا, sshc);
  }

  /* Setup the actual download */
  اگر(ڈیٹا->req.سائز == 0) {
    /* no data to transfer */
    Curl_xfer_تیاری_nop(ڈیٹا);
    infof(ڈیٹا, "File already completely downloaded");
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
    واپس rc;
  }
  Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, ڈیٹا->req.سائز);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ڈیٹا->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);

  واپس rc;
}

ساکن صحیح myssh_اندر_SFTP_CLOSE(ساخت Curl_easy *ڈیٹا,
                               ساخت ssh_conn *sshc,
                               ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);
  اگر(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = تہی;
  }
  curlx_safefree(sshp->راستہ);

  CURL_TRC_SSH(ڈیٹا, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  اگر(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_مقصد(ڈیٹا, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  }
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_SHUTDOWN(ساخت Curl_easy *ڈیٹا,
                                  ساخت ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_سیٹ_blocking(sshc->ssh_session, 0);
#اگر LIBSSH_نسخہ_INT > SSH_نسخہ_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#اگر_ختم

  اگر(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = تہی;
  }

  اگر(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = تہی;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_DISCONNECT);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_INIT(ساخت Curl_easy *ڈیٹا,
                              ساخت ssh_conn *sshc)
{
  صحیح rc;
  ssh_سیٹ_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  اگر(!sshc->sftp_session) {
    failf(ڈیٹا, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  اگر(rc != SSH_OK) {
    failf(ڈیٹا, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, sftp_error_مقصد_CURLE(SSH_FX_FAILURE));
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_REALPATH);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_REALPATH(ساخت Curl_easy *ڈیٹا,
                                  ساخت ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_راستہ(sshc->sftp_session, ".");
  اگر(!sshc->homedir)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(ڈیٹا->state.most_recent_ftp_entrypath);
  ڈیٹا->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  اگر(!ڈیٹا->state.most_recent_ftp_entrypath)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_برآمد_کا_حافظہ);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(ڈیٹا, "CONNECT phase done");
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_QUOTE_INIT(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc,
                                    ساخت SSHPROTO *sshp)
{
  CURLcode نتیجہ;

  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);

  نتیجہ = Curl_getworkingpath(ڈیٹا, sshc->homedir, &sshp->راستہ);
  اگر(نتیجہ) {
    sshc->actualcode = نتیجہ;
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  }
  ورنہ اگر(ڈیٹا->سیٹ.quote) {
    infof(ڈیٹا, "Sending quote commands");
    sshc->quote_شے = ڈیٹا->سیٹ.quote;
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE);
  }
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_GETINFO);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_POSTQUOTE_INIT(ساخت Curl_easy *ڈیٹا,
                                        ساخت ssh_conn *sshc)
{
  اگر(ڈیٹا->سیٹ.postquote) {
    infof(ڈیٹا, "Sending quote commands");
    sshc->quote_شے = ڈیٹا->سیٹ.postquote;
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  }
  واپس SSH_NO_ERROR;
}

ساکن صحیح quote_error(ساخت Curl_easy *ڈیٹا,
                       ساخت ssh_conn *sshc)
{
  failf(ڈیٹا, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_QUOTE(ساخت Curl_easy *ڈیٹا,
                               ساخت ssh_conn *sshc,
                               ساخت SSHPROTO *sshp)
{
  ثابت حرف *cp;
  CURLcode نتیجہ;

  /*
   * Support some of the "FTP" commands
   */
  ثابت حرف *cmd = sshc->quote_شے->ڈیٹا;
  sshc->acceptfail = FALSE;

  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  اگر(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  اگر(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    حرف *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->راستہ);
    اگر(!tmp) {
      sshc->actualcode = CURLE_برآمد_کا_حافظہ;
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      واپس SSH_NO_ERROR;
    }
    Curl_debug(ڈیٹا, CURLINFO_سرخی_برآمد, "PWD\n", 4);
    Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, tmp, لڑی_لمبائی(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, tmp, لڑی_لمبائی(tmp));
    curlx_free(tmp);
    اگر(نتیجہ) {
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجہ;
    }
    ورنہ
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
    واپس SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  اگر(!cp) {
    failf(ڈیٹا, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    واپس SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
      failf(ڈیٹا, "Out of memory");
    ورنہ
      failf(ڈیٹا, "Syntax error: Bad first parameter");
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = نتیجہ;
    واپس SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  اگر(!لڑی_موازنہ(cmd, "chgrp ", 6) ||
     !لڑی_موازنہ(cmd, "chmod ", 6) ||
     !لڑی_موازنہ(cmd, "chown ", 6) ||
     !لڑی_موازنہ(cmd, "atime ", 6) ||
     !لڑی_موازنہ(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
        failf(ڈیٹا, "Out of memory");
      ورنہ
        failf(ڈیٹا, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجہ;
      واپس SSH_NO_ERROR;
    }
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    sshc->quote_attrs = تہی;
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_STAT);
    واپس SSH_NO_ERROR;
  }
  اگر(!لڑی_موازنہ(cmd, "ln ", 3) ||
     !لڑی_موازنہ(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
        failf(ڈیٹا, "Out of memory");
      ورنہ
        failf(ڈیٹا, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجہ;
      واپس SSH_NO_ERROR;
    }
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_SYMLINK);
    واپس SSH_NO_ERROR;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "mkdir ", 6)) {
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    /* create directory */
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_MKDIR);
    واپس SSH_NO_ERROR;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    اگر(نتیجہ) {
      اگر(نتیجہ == CURLE_برآمد_کا_حافظہ)
        failf(ڈیٹا, "Out of memory");
      ورنہ
        failf(ڈیٹا, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجہ;
      واپس SSH_NO_ERROR;
    }
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_RENAME);
    واپس SSH_NO_ERROR;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "rmdir ", 6)) {
    /* delete directory */
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_RMDIR);
    واپس SSH_NO_ERROR;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "rm ", 3)) {
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_UNLINK);
    واپس SSH_NO_ERROR;
  }
#اگر_تعریف_شدہ HAS_STATVFS_SUPPORT
  ورنہ اگر(!لڑی_موازنہ(cmd, "statvfs ", 8)) {
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_STATVFS);
    واپس SSH_NO_ERROR;
  }
#اگر_ختم

  failf(ڈیٹا, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_اگلا_QUOTE(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_شے = sshc->quote_شے->اگلا;

  اگر(sshc->quote_شے) {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE);
  }
  ورنہ {
    اگر(sshc->nextstate != SSH_NO_STATE) {
      myssh_مقصد(ڈیٹا, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    ورنہ {
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_GETINFO);
    }
  }
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_QUOTE_STAT(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc)
{
  ثابت حرف *cmd = sshc->quote_شے->ڈیٹا;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  اگر(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  اگر(sshc->quote_attrs)
    sftp_اوصاف_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  اگر(!sshc->quote_attrs) {
    failf(ڈیٹا, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(ڈیٹا, sshc, تہی);
    واپس SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  اگر(!لڑی_موازنہ(cmd, "chgrp", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t gid;
    اگر(curlx_str_نمبر(&p, &gid, UINT_MAX)) {
      failf(ڈیٹا, "Syntax error: chgrp gid not a number");
      myssh_quote_error(ڈیٹا, sshc, تہی);
      واپس SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت حرف *p = sshc->quote_path1;
    اگر(curlx_str_octal(&p, &perms, 07777)) {
      failf(ڈیٹا, "Syntax error: chmod permissions not a number");
      myssh_quote_error(ڈیٹا, sshc, تہی);
      واپس SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "chown", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t uid;
    اگر(curlx_str_نمبر(&p, &uid, UINT_MAX)) {
      failf(ڈیٹا, "Syntax error: chown uid not a number");
      myssh_quote_error(ڈیٹا, sshc, تہی);
      واپس SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "atime", 5) ||
          !لڑی_موازنہ(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    اگر(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(ڈیٹا, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#اگر SIZEOF_وقت_T > 4
    ورنہ اگر(date > 0xffffffff) {
      failf(ڈیٹا, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#اگر_ختم
    اگر(fail) {
      myssh_quote_error(ڈیٹا, sshc, تہی);
      واپس SSH_NO_ERROR;
    }
    اگر(!لڑی_موازنہ(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    ورنہ /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_SETSTAT);
  واپس SSH_NO_ERROR;
}

ساکن خالی conn_forget_socket(ساخت Curl_easy *ڈیٹا, int8_t sockindex)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  اگر(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    ساخت Curl_cfilter *cf = conn->cfilter[sockindex];
    اگر(cf)
      (خالی)Curl_conn_cf_cntrl(cf, ڈیٹا, TRUE, CF_CTRL_FORGET_SOCKET, 0, تہی);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

ساکن خالی myssh_SESSION_DISCONNECT(ساخت Curl_easy *ڈیٹا,
                                     ساخت ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  اگر(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = تہی;
  }

  اگر(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = تہی;
  }
  اگر(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = تہی;
  }

  ssh_disconnect(sshc->ssh_session);
  اگر(!ssh_نسخہ(SSH_نسخہ_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(ڈیٹا, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
}

ساکن صحیح myssh_SSH_SCP_DOWNLOAD(ساخت Curl_easy *ڈیٹا,
                                  ساخت ssh_conn *sshc)
{
  uint64_t bytecount;
  صحیح rc = ssh_scp_pull_request(sshc->scp_session);
  اگر(rc != SSH_SCP_REQUEST_NEWFILE) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(ڈیٹا, "%s", err_پیغام);
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_REMOTE_FILE_NOT_ملا);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  اگر(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_TOO_LARGE);
  ڈیٹا->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, ڈیٹا->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ڈیٹا->conn->send_idx = 0;

  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس 0;
}

ساکن صحیح myssh_اندر_TRANS_INIT(ساخت Curl_easy *ڈیٹا, ساخت ssh_conn *sshc,
                               ساخت SSHPROTO *sshp)
{
  CURLcode نتیجہ;
  صحیح rc = 0;
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);

  نتیجہ = Curl_getworkingpath(ڈیٹا, sshc->homedir, &sshp->راستہ);
  اگر(نتیجہ) {
    sshc->actualcode = نتیجہ;
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
    واپس 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_سیٹ_blocking(sshc->ssh_session, 1);

  اگر(ڈیٹا->state.upload) {
    اگر(ڈیٹا->state.infilesize < 0) {
      failf(ڈیٹا, "SCP requires a known file size for upload");
      واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->راستہ);
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ورنہ {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->راستہ);
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  اگر(!sshc->scp_session) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(ڈیٹا, "%s", err_پیغام);
    rc = myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_UPLOAD_FAILED);
  }
  واپس rc;
}

ساکن خالی sshc_cleanup(ساخت ssh_conn *sshc)
{
  اگر(sshc->initialized) {
    اگر(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = تہی;
    }
    اگر(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = تہی;
    }
    اگر(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = تہی;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    اگر(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = تہی;
    }
    اگر(sshc->quote_attrs) {
      sftp_اوصاف_free(sshc->quote_attrs);
      sshc->quote_attrs = تہی;
    }
    اگر(sshc->readdir_attrs) {
      sftp_اوصاف_free(sshc->readdir_attrs);
      sshc->readdir_attrs = تہی;
    }
    اگر(sshc->readdir_link_attrs) {
      sftp_اوصاف_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = تہی;
    }
    اگر(sshc->privkey) {
      ssh_کلید_free(sshc->privkey);
      sshc->privkey = تہی;
    }
    اگر(sshc->pubkey) {
      ssh_کلید_free(sshc->pubkey);
      sshc->pubkey = تہی;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_کلید);
    curlx_safefree(sshc->priv_کلید);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_link_راستہ);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

ساکن صحیح myssh_اندر_SFTP_QUOTE_SETSTAT(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc)
{
  صحیح rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  اگر(rc == SSH_AGAIN)
    واپس rc;
  اگر(rc && !sshc->acceptfail) {
    myssh_quote_error(ڈیٹا, sshc, "setstat");
    واپس rc;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_QUOTE_SYMLINK(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc)
{
  صحیح rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  اگر(rc == SSH_AGAIN)
    واپس rc;
  اگر(rc && !sshc->acceptfail) {
    myssh_quote_error(ڈیٹا, sshc, "symlink");
    واپس rc;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_QUOTE_MKDIR(ساخت Curl_easy *ڈیٹا,
                                     ساخت ssh_conn *sshc)
{
  صحیح rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)ڈیٹا->سیٹ.new_directory_perms);
  اگر(rc == SSH_AGAIN)
    واپس rc;
  اگر(rc && !sshc->acceptfail) {
    myssh_quote_error(ڈیٹا, sshc, "mkdir");
    واپس rc;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_QUOTE_RENAME(ساخت Curl_easy *ڈیٹا,
                                      ساخت ssh_conn *sshc)
{
  صحیح rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  اگر(rc == SSH_AGAIN)
    واپس rc;
  اگر(rc && !sshc->acceptfail) {
    myssh_quote_error(ڈیٹا, sshc, "rename");
    واپس rc;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_QUOTE_RMDIR(ساخت Curl_easy *ڈیٹا,
                                     ساخت ssh_conn *sshc)
{
  صحیح rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  اگر(rc == SSH_AGAIN)
    واپس rc;
  اگر(rc && !sshc->acceptfail) {
    myssh_quote_error(ڈیٹا, sshc, "rmdir");
    واپس rc;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_QUOTE_UNLINK(ساخت Curl_easy *ڈیٹا,
                                      ساخت ssh_conn *sshc)
{
  صحیح rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  اگر(rc == SSH_AGAIN)
    واپس rc;
  اگر(rc && !sshc->acceptfail) {
    myssh_quote_error(ڈیٹا, sshc, "rm");
    واپس rc;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_GETINFO(ساخت Curl_easy *ڈیٹا,
                                 ساخت ssh_conn *sshc)
{
  اگر(ڈیٹا->سیٹ.get_filetime)
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_FILETIME);
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_TRANS_INIT);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_FILETIME(ساخت Curl_easy *ڈیٹا,
                                  ساخت ssh_conn *sshc,
                                  ساخت SSHPROTO *sshp)
{
  sftp_اوصاف attrs;
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->راستہ);
  اگر(attrs) {
    ڈیٹا->info.filetime = attrs->mtime;
    sftp_اوصاف_free(attrs);
  }

  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_TRANS_INIT);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_TRANS_INIT(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc,
                                    ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);
  اگر(ڈیٹا->state.upload)
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_UPLOAD_INIT);
  ورنہ {
    size_t راستہ_len = لڑی_لمبائی(sshp->راستہ);

    اگر(راستہ_len && sshp->راستہ[راستہ_len - 1] == '/')
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_INIT);
    ورنہ
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_CREATE_DIRS_INIT(ساخت Curl_easy *ڈیٹا,
                                          ساخت ssh_conn *sshc,
                                          ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);

  اگر(لڑی_لمبائی(sshp->راستہ) > 1) {
    sshc->slash_pos = sshp->راستہ + 1; /* ignore the leading '/' */
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_CREATE_DIRS(ساخت Curl_easy *ڈیٹا,
                                     ساخت ssh_conn *sshc,
                                     ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  اگر(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(ڈیٹا, "Creating directory '%s'", sshp->راستہ);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    واپس SSH_NO_ERROR;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_UPLOAD_INIT);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SFTP_CREATE_DIRS_MKDIR(ساخت Curl_easy *ڈیٹا,
                                           ساخت ssh_conn *sshc,
                                           ساخت SSHPROTO *sshp)
{
  صحیح rc;
  صحیح err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->راستہ,
                  (mode_t)ڈیٹا->سیٹ.new_directory_perms);
  اگر(rc == SSH_AGAIN)
    واپس rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  اگر(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    اگر((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      واپس myssh_مقصد_SFTP_CLOSE(ڈیٹا, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CREATE_DIRS);
  واپس rc;
}

ساکن صحیح myssh_اندر_SCP_UPLOAD_INIT(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc,
                                    ساخت SSHPROTO *sshp)
{
  صحیح rc;
  اگر(!sshp)
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  اگر(rc != SSH_OK) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(ڈیٹا, "%s", err_پیغام);
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ڈالو_file64(sshc->scp_session, sshp->راستہ,
                           (uint64_t)ڈیٹا->state.infilesize,
                           (صحیح)ڈیٹا->سیٹ.new_file_perms);

  اگر(rc != SSH_OK) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(ڈیٹا, "%s", err_پیغام);
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_تیاری_send(ڈیٹا, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ڈیٹا->conn->recv_idx = FIRSTSOCKET;

  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SCP_DOWNLOAD_INIT(ساخت Curl_easy *ڈیٹا,
                                      ساخت ssh_conn *sshc)
{
  صحیح rc = ssh_scp_init(sshc->scp_session);
  اگر(rc != SSH_OK) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(ڈیٹا, "%s", err_پیغام);
    واپس myssh_مقصد_ERROR(ڈیٹا, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SCP_DOWNLOAD);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SCP_مکمل(ساخت Curl_easy *ڈیٹا,
                             ساخت ssh_conn *sshc)
{
  اگر(ڈیٹا->state.upload)
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_SEND_EOF);
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_CHANNEL_FREE);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SCP_SEND_EOF(ساخت Curl_easy *ڈیٹا,
                                 ساخت ssh_conn *sshc)
{
  اگر(sshc->scp_session) {
    صحیح rc = ssh_scp_close(sshc->scp_session);
    اگر(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      واپس SSH_AGAIN;
    }
    اگر(rc != SSH_OK) {
      infof(ڈیٹا,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_مقصد(ڈیٹا, sshc, SSH_SCP_CHANNEL_FREE);
  واپس SSH_NO_ERROR;
}

ساکن صحیح myssh_اندر_SCP_CHANNEL_FREE(ساخت Curl_easy *ڈیٹا,
                                     ساخت ssh_conn *sshc)
{
  اگر(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = تہی;
  }
  CURL_TRC_SSH(ڈیٹا, "SCP DONE phase complete");

  ssh_سیٹ_blocking(sshc->ssh_session, 0);

  myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_DISCONNECT);
  واپس SSH_NO_ERROR;
}

ساکن CURLcode myssh_اندر_SESSION_FREE(ساخت Curl_easy *ڈیٹا,
                                      ساخت ssh_conn *sshc)
{
  CURLcode نتیجہ;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  نتیجہ = sshc->actualcode;
  memset(sshc, 0, حجم(ساخت ssh_conn));
  connclose(ڈیٹا->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس نتیجہ;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
ساکن CURLcode myssh_statemachine(ساخت Curl_easy *ڈیٹا,
                                   ساخت ssh_conn *sshc,
                                   ساخت SSHPROTO *sshp,
                                   bool *بلاک)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح rc = SSH_NO_ERROR;

  *بلاک = FALSE;                   /* we are not blocking by default */
  کرو {

    انتخاب(sshc->state) {
    حالت SSH_INIT:
      myssh_state_init(ڈیٹا, sshc);
      FALLTHROUGH();
    حالت SSH_S_STARTUP:
      rc = myssh_اندر_S_STARTUP(ڈیٹا, sshc);
      اگر(rc)
        توڑو;
      FALLTHROUGH();
    حالت SSH_HOSTKEY:
      rc = myssh_is_معلوم(ڈیٹا, sshc);
      اگر(rc != SSH_OK) {
        rc = myssh_مقصد_ERROR(ڈیٹا, sshc, CURLE_PEER_FAILED_VERIFICATION);
        توڑو;
      }
      myssh_مقصد(ڈیٹا, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    حالت SSH_AUTHLIST:
      rc = myssh_اندر_AUTHLIST(ڈیٹا, sshc);
      توڑو;
    حالت SSH_AUTH_PKEY_INIT:
      rc = myssh_اندر_AUTH_PKEY_INIT(ڈیٹا, sshc);
      توڑو;
    حالت SSH_AUTH_PKEY:
      rc = myssh_اندر_AUTH_PKEY(ڈیٹا, sshc);
      توڑو;
    حالت SSH_AUTH_GSSAPI:
      rc = myssh_اندر_AUTH_GSSAPI(ڈیٹا, sshc);
      توڑو;
    حالت SSH_AUTH_کلید_INIT:
      rc = myssh_اندر_AUTH_کلید_INIT(ڈیٹا, sshc);
      توڑو;
    حالت SSH_AUTH_کلید:
      rc = myssh_اندر_AUTH_کلید(ڈیٹا, sshc);
      توڑو;
    حالت SSH_AUTH_PASS_INIT:
      rc = myssh_اندر_AUTH_PASS_INIT(ڈیٹا, sshc);
      توڑو;
    حالت SSH_AUTH_PASS:
      rc = myssh_اندر_AUTH_PASS(ڈیٹا, sshc);
      توڑو;
    حالت SSH_AUTH_مکمل:
      rc = myssh_اندر_AUTH_مکمل(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_INIT:
      rc = myssh_اندر_SFTP_INIT(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_REALPATH:
      rc = myssh_اندر_SFTP_REALPATH(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE_INIT:
      rc = myssh_اندر_SFTP_QUOTE_INIT(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_اندر_SFTP_POSTQUOTE_INIT(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE:
      rc = myssh_اندر_SFTP_QUOTE(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_اگلا_QUOTE:
      rc = myssh_اندر_SFTP_اگلا_QUOTE(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE_STAT:
      rc = myssh_اندر_SFTP_QUOTE_STAT(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_اندر_SFTP_QUOTE_SETSTAT(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_اندر_SFTP_QUOTE_SYMLINK(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_اندر_SFTP_QUOTE_MKDIR(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE_RENAME:
      rc = myssh_اندر_SFTP_QUOTE_RENAME(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_اندر_SFTP_QUOTE_RMDIR(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_اندر_SFTP_QUOTE_UNLINK(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_اندر_SFTP_QUOTE_STATVFS(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_GETINFO:
      rc = myssh_اندر_SFTP_GETINFO(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_FILETIME:
      rc = myssh_اندر_SFTP_FILETIME(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_TRANS_INIT:
      rc = myssh_اندر_SFTP_TRANS_INIT(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_UPLOAD_INIT:
      rc = myssh_اندر_UPLOAD_INIT(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_اندر_SFTP_CREATE_DIRS_INIT(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_CREATE_DIRS:
      rc = myssh_اندر_SFTP_CREATE_DIRS(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_اندر_SFTP_CREATE_DIRS_MKDIR(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_READDIR_INIT:
      rc = myssh_اندر_SFTP_READDIR_INIT(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_READDIR:
      rc = myssh_اندر_SFTP_READDIR(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_READDIR_LINK:
      rc = myssh_اندر_SFTP_READDIR_LINK(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_اندر_SFTP_READDIR_BOTTOM(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_READDIR_مکمل:
      rc = myssh_اندر_SFTP_READDIR_مکمل(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_اندر_SFTP_DOWNLOAD_INIT(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_اندر_SFTP_DOWNLOAD_STAT(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SFTP_CLOSE:
      rc = myssh_اندر_SFTP_CLOSE(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SFTP_SHUTDOWN:
      rc = myssh_اندر_SFTP_SHUTDOWN(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SCP_TRANS_INIT:
      rc = myssh_اندر_TRANS_INIT(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SCP_UPLOAD_INIT:
      rc = myssh_اندر_SCP_UPLOAD_INIT(ڈیٹا, sshc, sshp);
      توڑو;
    حالت SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_اندر_SCP_DOWNLOAD_INIT(ڈیٹا, sshc);
      اگر(rc)
        توڑو;
      FALLTHROUGH();
    حالت SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SCP_مکمل:
      rc = myssh_اندر_SCP_مکمل(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SCP_SEND_EOF:
      rc = myssh_اندر_SCP_SEND_EOF(ڈیٹا, sshc);
      توڑو;
    حالت SSH_SCP_CHANNEL_FREE:
      myssh_اندر_SCP_CHANNEL_FREE(ڈیٹا, sshc);
      FALLTHROUGH();
    حالت SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(ڈیٹا, sshc);
      FALLTHROUGH();
    حالت SSH_SESSION_FREE:
      نتیجہ = myssh_اندر_SESSION_FREE(ڈیٹا, sshc);
      توڑو;
    حالت SSH_QUIT:
    طےشدہ:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
      توڑو;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } جب((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  اگر(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *بلاک = TRUE;
  }
  اگر(!نتیجہ && (sshc->state == SSH_STOP))
    نتیجہ = sshc->actualcode;
  CURL_TRC_SSH(ڈیٹا, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحیح)نتیجہ, *بلاک);
  واپس نتیجہ;
}

/* called repeatedly until done from multi.c */
ساکن CURLcode myssh_multi_statemach(ساخت Curl_easy *ڈیٹا,
                                      bool *مکمل)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ساخت SSHPROTO *sshp = Curl_meta_get(ڈیٹا, CURL_META_SSH_EASY);
  bool بلاک;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode نتیجہ;

  اگر(!sshc || !sshp)
    واپس CURLE_FAILED_INIT;
  نتیجہ = myssh_statemachine(ڈیٹا, sshc, sshp, &بلاک);
  *مکمل = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, بلاک);

  واپس نتیجہ;
}

ساکن CURLcode myssh_بلاک_statemach(ساخت Curl_easy *ڈیٹا,
                                      ساخت ssh_conn *sshc,
                                      ساخت SSHPROTO *sshp,
                                      bool disconnect)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  CURLcode نتیجہ = CURLE_OK;

  جب((sshc->state != SSH_STOP) && !نتیجہ) {
    bool بلاک;
    timediff_t بایاں_ms = 1000;

    نتیجہ = myssh_statemachine(ڈیٹا, sshc, sshp, &بلاک);
    اگر(نتیجہ)
      توڑو;

    اگر(!disconnect) {
      نتیجہ = Curl_pgrsCheck(ڈیٹا);
      اگر(نتیجہ)
        توڑو;

      بایاں_ms = Curl_timeleft_ms(ڈیٹا);
      اگر(بایاں_ms < 0) {
        failf(ڈیٹا, "Operation timed out");
        واپس CURLE_OPERATION_TIMEDOUT;
      }
    }

    اگر(بلاک) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (خالی)SOCKET_READABLE(fd_read,
                            (!بایاں_ms || بایاں_ms > 1000) ? 1000 : بایاں_ms);
    }
  }

  واپس نتیجہ;
}

ساکن خالی myssh_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت SSHPROTO *sshp = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  curlx_safefree(sshp->راستہ);
  curlx_free(sshp);
}

ساکن خالی myssh_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت ssh_conn *sshc = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
ساکن CURLcode myssh_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                       ساخت connectdata *conn)
{
  ساخت SSHPROTO *sshp;
  ساخت ssh_conn *sshc;

  sshc = curlx_calloc(1, حجم(*sshc));
  اگر(!sshc)
    واپس CURLE_برآمد_کا_حافظہ;

  curlx_dyn_init(&sshc->readdir_buf, CURL_راستہ_MAX * 2);
  sshc->initialized = TRUE;
  اگر(Curl_conn_meta_سیٹ(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  sshp = curlx_calloc(1, حجم(*sshp));
  اگر(!sshp ||
     Curl_meta_سیٹ(ڈیٹا, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  واپس Curl_ssh_تیاری_pkey(ڈیٹا, sshc);
}

ساکن Curl_recv scp_recv, sftp_recv;
ساکن Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ساکن CURLcode myssh_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ساخت SSHPROTO *ssh = Curl_meta_get(ڈیٹا, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحیح rc;

  اگر(!sshc || !ssh)
    واپس CURLE_FAILED_INIT;

  CURL_TRC_SSH(ڈیٹا, "myssh_connect");
  اگر(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ورنہ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  اگر(!sshc->ssh_session) {
    failf(ڈیٹا, "Failure initializing ssh session");
    واپس CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_سیٹ(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  اگر(rc != SSH_OK) {
    failf(ڈیٹا, "Could not set remote host");
    واپس CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, تہی);
  اگر(rc != SSH_OK) {
    infof(ڈیٹا, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(ڈیٹا, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_سیٹ(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  اگر(rc != SSH_OK) {
    failf(ڈیٹا, "Could not set socket");
    واپس CURLE_FAILED_INIT;
  }

  اگر(Curl_creds_has_user(conn->creds)) {
    infof(ڈیٹا, "User: %s", conn->creds->user);
    rc = ssh_options_سیٹ(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    اگر(rc != SSH_OK) {
      failf(ڈیٹا, "Could not set user");
      واپس CURLE_FAILED_INIT;
    }
  }

  اگر(CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS)) {
    infof(ڈیٹا, "Known hosts: %s",
          CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_سیٹ(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS));
    اگر(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_سیٹ(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS));
    اگر(rc != SSH_OK) {
      failf(ڈیٹا, "Could not set known hosts file path");
      واپس CURLE_FAILED_INIT;
    }
  }

  اگر(conn->origin->پورٹ) {
    rc = ssh_options_سیٹ(sshc->ssh_session, SSH_OPTIONS_پورٹ,
                         &conn->origin->پورٹ);
    اگر(rc != SSH_OK) {
      failf(ڈیٹا, "Could not set remote port");
      واپس CURLE_FAILED_INIT;
    }
  }

  اگر(ڈیٹا->سیٹ.ssh_compression) {
    rc = ssh_options_سیٹ(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    اگر(rc != SSH_OK) {
      failf(ڈیٹا, "Could not set compression");
      واپس CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = تہی;
  sshc->pubkey = تہی;

  اگر(sshc->pub_کلید) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_کلید, &sshc->pubkey);
    اگر(rc != SSH_OK) {
      failf(ڈیٹا, "Could not load public key file");
      واپس CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_مقصد(ڈیٹا, sshc, SSH_INIT);

  نتیجہ = myssh_multi_statemach(ڈیٹا, مکمل);

  واپس نتیجہ;
}

/* called from multi.c while DOing */
ساکن CURLcode scp_doing(ساخت Curl_easy *ڈیٹا, bool *dophase_مکمل)
{
  CURLcode نتیجہ;

  نتیجہ = myssh_multi_statemach(ڈیٹا, dophase_مکمل);

  اگر(*dophase_مکمل) {
    CURL_TRC_SSH(ڈیٹا, "DO phase is complete");
  }
  واپس نتیجہ;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ساکن CURLcode scp_perform(ساخت Curl_easy *ڈیٹا,
                            bool *connected, bool *dophase_مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(ڈیٹا, "DO phase starts");

  *dophase_مکمل = FALSE;        /* not done yet */
  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_مقصد(ڈیٹا, sshc, SSH_SCP_TRANS_INIT);

  نتیجہ = myssh_multi_statemach(ڈیٹا, dophase_مکمل);

  *connected = Curl_conn_is_connected(ڈیٹا->conn, FIRSTSOCKET);

  اگر(*dophase_مکمل) {
    CURL_TRC_SSH(ڈیٹا, "DO phase is complete");
  }

  واپس نتیجہ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساکن CURLcode scp_disconnect(ساخت Curl_easy *ڈیٹا,
                               ساخت connectdata *conn,
                               bool dead_connection)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ساخت SSHPROTO *sshp = Curl_meta_get(ڈیٹا, CURL_META_SSH_EASY);
  (خالی)dead_connection;

  اگر(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_DISCONNECT);
    نتیجہ = myssh_بلاک_statemach(ڈیٹا, sshc, sshp, TRUE);
  }

  واپس نتیجہ;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ساکن CURLcode myssh_مکمل(ساخت Curl_easy *ڈیٹا,
                           ساخت ssh_conn *sshc,
                           CURLcode حالت_2)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت SSHPROTO *sshp = Curl_meta_get(ڈیٹا, CURL_META_SSH_EASY);

  اگر(!حالت_2 && sshp) {
    /* run the state-machine */
    نتیجہ = myssh_بلاک_statemach(ڈیٹا, sshc, sshp, FALSE);
  }
  ورنہ
    نتیجہ = حالت_2;

  اگر(Curl_pgrs_مکمل(ڈیٹا, Curl_pgrs_now(ڈیٹا)))
    واپس CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(ڈیٹا);
  واپس نتیجہ;
}

ساکن CURLcode scp_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode حالت_2,
                         bool premature)
{
  ساخت ssh_conn *sshc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SSH_CONN);
  (خالی)premature;

  اگر(!sshc)
    واپس CURLE_FAILED_INIT;
  اگر(!حالت_2)
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_مکمل);

  واپس myssh_مکمل(ڈیٹا, sshc, حالت_2);
}

ساکن CURLcode scp_send(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  صحیح rc;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (خالی)sockindex; /* we only support SCP on the fixed known primary socket */
  (خالی)eos;
  *pnwritten = 0;

  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#اگر 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  اگر(rc == SSH_AGAIN)
    واپس CURLE_AGAIN;
  ورنہ
#اگر_ختم
  اگر(rc != SSH_OK)
    واپس CURLE_SSH;

  *pnwritten = len;
  واپس CURLE_OK;
}

ساکن CURLcode scp_recv(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                         حرف *mem, size_t len, size_t *pnread)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  صحیح nread;

  (خالی)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  اگر(nread == SSH_ERROR)
    واپس CURLE_SSH;
#اگر 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  اگر(nread == SSH_AGAIN)
    واپس CURLE_AGAIN;
#اگر_ختم
  *pnread = (size_t)nread;
  واپس CURLE_OK;
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

ساکن CURLcode sftp_perform(ساخت Curl_easy *ڈیٹا,
                             bool *connected,
                             bool *dophase_مکمل)
{
  ساخت ssh_conn *sshc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SSH_CONN);
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_SSH(ڈیٹا, "DO phase starts");

  *dophase_مکمل = FALSE; /* not done yet */
  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  نتیجہ = myssh_multi_statemach(ڈیٹا, dophase_مکمل);

  *connected = Curl_conn_is_connected(ڈیٹا->conn, FIRSTSOCKET);

  اگر(*dophase_مکمل) {
    CURL_TRC_SSH(ڈیٹا, "DO phase is complete");
  }

  واپس نتیجہ;
}

/* called from multi.c while DOing */
ساکن CURLcode sftp_doing(ساخت Curl_easy *ڈیٹا,
                           bool *dophase_مکمل)
{
  CURLcode نتیجہ = myssh_multi_statemach(ڈیٹا, dophase_مکمل);
  اگر(*dophase_مکمل) {
    CURL_TRC_SSH(ڈیٹا, "DO phase is complete");
  }
  واپس نتیجہ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساکن CURLcode sftp_disconnect(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn,
                                bool dead_connection)
{
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ساخت SSHPROTO *sshp = Curl_meta_get(ڈیٹا, CURL_META_SSH_EASY);
  CURLcode نتیجہ = CURLE_OK;
  (خالی)dead_connection;

  CURL_TRC_SSH(ڈیٹا, "DISCONNECT starts now");

  اگر(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_SHUTDOWN);
    نتیجہ = myssh_بلاک_statemach(ڈیٹا, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(ڈیٹا, "DISCONNECT is done");
  واپس نتیجہ;
}

ساکن CURLcode sftp_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode حالت_2,
                          bool premature)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  اگر(!sshc)
    واپس CURLE_FAILED_INIT;
  اگر(!حالت_2) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    اگر(!premature && ڈیٹا->سیٹ.postquote && !conn->بٹ.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
  }
  واپس myssh_مکمل(ڈیٹا, sshc, حالت_2);
}

/* return number of sent bytes */
ساکن CURLcode sftp_send(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                          ثابت uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (خالی)sockindex;
  (خالی)eos;
  *pnwritten = 0;

  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

#اگر LIBSSH_نسخہ_INT > SSH_نسخہ_INT(0, 11, 0)
  انتخاب(sshc->sftp_send_state) {
  حالت 0:
    sftp_file_سیٹ_nonblocking(sshc->sftp_file);
    اگر(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      واپس CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  حالت 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    اگر(nwrite == SSH_AGAIN)
      واپس CURLE_AGAIN;
    ورنہ اگر(nwrite < 0)
      واپس CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    واپس CURLE_OK;
  طےشدہ:
    /* we never reach here */
    واپس CURLE_SEND_ERROR;
  }
#ورنہ
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  اگر(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#اگر 0 /* not returned by libssh on write */
  اگر(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  ورنہ
#اگر_ختم
  اگر(nwrite < 0)
    واپس CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  واپس CURLE_OK;
#اگر_ختم
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ساکن CURLcode sftp_recv(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                          حرف *mem, size_t len, size_t *pnread)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (خالی)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_سائز);
  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  انتخاب(sshc->sftp_recv_state) {
  حالت 0:
#اگر LIBSSH_نسخہ_INT > SSH_نسخہ_INT(0, 11, 0)
    اگر(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      واپس CURLE_RECV_ERROR;
    }
#ورنہ
    sshc->sftp_file_اشاریہ =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    اگر(sshc->sftp_file_اشاریہ < 0)
      واپس CURLE_RECV_ERROR;
#اگر_ختم

    FALLTHROUGH();
  حالت 1:
    sshc->sftp_recv_state = 1;

#اگر LIBSSH_نسخہ_INT > SSH_نسخہ_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#ورنہ
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_اشاریہ);
#اگر_ختم

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    اگر(nread == SSH_AGAIN)
      واپس CURLE_AGAIN;
    ورنہ اگر(nread < 0)
      واپس CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    واپس CURLE_OK;

  طےشدہ:
    /* we never reach here */
    واپس CURLE_RECV_ERROR;
  }
}

ساکن CURLcode myssh_do_it(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ;
  bool connected = FALSE;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *مکمل = FALSE;                /* default to false */
  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  ڈیٹا->req.سائز = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->دوسرا_Create_Dirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(ڈیٹا);

  اگر(conn->scheme->protocol & CURLPROTO_SCP)
    نتیجہ = scp_perform(ڈیٹا, &connected, مکمل);
  ورنہ
    نتیجہ = sftp_perform(ڈیٹا, &connected, مکمل);

  واپس نتیجہ;
}

CURLcode Curl_ssh_init(خالی)
{
  اگر(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    واپس CURLE_FAILED_INIT;
  }
  واپس CURLE_OK;
}

خالی Curl_ssh_cleanup(خالی)
{
  (خالی)ssh_finalize();
}

خالی Curl_ssh_نسخہ(حرف *بفر, size_t buflen)
{
  (خالی)curl_msnprintf(بفر, buflen, "libssh/%s", ssh_نسخہ(0));
}

/*
 * SCP.
 */
ثابت ساخت Curl_protocol Curl_protocol_scp = {
  myssh_تیاری_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_مکمل,                             /* done */
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
ثابت ساخت Curl_protocol Curl_protocol_sftp = {
  myssh_تیاری_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_مکمل,                            /* done */
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

#اگر_ختم /* USE_LIBSSH */
