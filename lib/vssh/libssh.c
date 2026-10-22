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

#که_تعریف_شوی USE_LIBSSH

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی __VMS
#ضمن <in.h>
#ضمن <inet.h>
#د_که_پای

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

#که_تعریف_شوی HAVE_UNISTD_H
#ضمن <unistd.h>
#د_که_پای
#که_تعریف_شوی HAVE_FCNTL_H
#ضمن <fcntl.h>
#د_که_پای

/* A recent macro provided by libssh. Or make our own. */
#که_نه_تعریف_شوی SSH_STRING_FREE_CHAR
#تعریف_کړه SSH_STRING_FREE_CHAR(x) \
  افعل {                          \
    إذا(x) {                     \
      ssh_string_free_char(x);  \
      x = عدم;                 \
    }                           \
  } طالما(0)
#د_که_پای

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#که_نه_تعریف_شوی SSH_S_IFMT
#تعریف_کړه SSH_S_IFMT   00170000
#د_که_پای
#که_نه_تعریف_شوی SSH_S_IFLNK
#تعریف_کړه SSH_S_IFLNK  0120000
#د_که_پای

ساكن CURLcode sftp_error_منزل_CURLE(صحيح err)
{
  بدل(err) {
  حالة SSH_FX_OK:
    أرجع CURLE_OK;

  حالة SSH_FX_NO_SUCH_FILE:
  حالة SSH_FX_NO_SUCH_لار:
    أرجع CURLE_REMOTE_FILE_NOT_وموندل;

  حالة SSH_FX_PERMISSION_DENIED:
  حالة SSH_FX_WRITE_PROTECT:
    أرجع CURLE_REMOTE_ACCESS_DENIED;

  حالة SSH_FX_FILE_ALREADY_EXISTS:
    أرجع CURLE_REMOTE_FILE_EXISTS;

  افتراضي:
    اكسر;
  }

  أرجع CURLE_SSH;
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
ساكن صحيح myssh_is_معلوم(بنية Curl_easy *معلومات, بنية ssh_conn *sshc)
{
  صحيح rc;
  ssh_کلی pubkey;
  بینښه حرف *hash_sha256 = عدم;
  size_t hlen_sha256;
  بینښه حرف *hash_md5 = عدم;
  size_t hlen_md5;
  حرف *وموندل_base64 = عدم;
  حرف *معلوم_base64 = عدم;
  صحيح vstate;
  تعداد curl_khmatch keymatch;
  بنية curl_khkey foundkey;
  بنية curl_khkey *knownkeyp = عدم;
  curl_sshkeycallback func = معلومات->سېټ.ssh_keyfunc;
  بنية ssh_knownhosts_ثبت *knownhostsentry = عدم;
  بنية curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  إذا(rc != SSH_OK)
    أرجع rc;

  إذا(CURL_EASY_STR(معلومات, STRING_SSH_HOST_عامه_کلی_SHA256)) {
    ثابت حرف *pubkey_sha256 =
      CURL_EASY_STR(معلومات, STRING_SSH_HOST_عامه_کلی_SHA256);
    حرف *fingerprint_b64 = عدم;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    إذا(rc != SSH_OK || hlen_sha256 != 32) {
      failf(معلومات, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      اذهب cleanup;
    }

    إذا(curlx_base64_encode((ثابت uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      اذهب cleanup;
    }

    infof(معلومات, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    طالما((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    طالما((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    إذا((pub_pos != b64_pos) ||
       مزي_پرتله(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(معلومات,
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

  إذا(CURL_EASY_STR(معلومات, STRING_SSH_HOST_عامه_کلی_MD5)) {
    ثابت حرف *pubkey_md5 =
      CURL_EASY_STR(معلومات, STRING_SSH_HOST_عامه_کلی_MD5);
    حرف md5buffer[33];
    صحيح i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    إذا(rc != SSH_OK || hlen_md5 != 16) {
      failf(معلومات,
            "Denied establishing ssh session: MD5 fingerprint not available");
      اذهب cleanup;
    }

    لكل(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(معلومات, "SSH MD5 fingerprint: %s", md5buffer);

    إذا(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(معلومات,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      اذهب cleanup;
    }

    rc = SSH_OK;
    اذهب cleanup;
  }

  إذا(CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_معلوم_hosts_ثبت(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    إذا(knownhostsentry) {
      إذا(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &معلوم_base64);
        إذا(rc != SSH_OK) {
          اذهب cleanup;
        }
        knownkey.کلی = معلوم_base64;
        knownkey.len = مزي_اوږدوالی(معلوم_base64);

        بدل(ssh_کلی_type(knownhostsentry->publickey)) {
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
    حالة SSH_معلوم_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      اكسر;
    حالة SSH_معلوم_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      اكسر;
    حالة SSH_معلوم_HOSTS_NOT_وموندل:
    حالة SSH_معلوم_HOSTS_UNKNOWN:
    حالة SSH_معلوم_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      اكسر;
    افتراضي:
      keymatch = CURLKHMATCH_MISMATCH;
      اكسر;
    }

    إذا(func) { /* use callback to determine action */
      بنية Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &وموندل_base64);
      إذا(rc != SSH_OK)
        اذهب cleanup;

      foundkey.کلی = وموندل_base64;
      foundkey.len = مزي_اوږدوالی(وموندل_base64);

      بدل(ssh_کلی_type(pubkey)) {
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

      CURL_CBAPI_پیل(&guard, معلومات, easy_ssh_keyfunc);
      rc = func(معلومات, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, معلومات->سېټ.ssh_keyfunc_userp);
      CURL_CBAPI_پای_ټکی(&guard);

      بدل(rc) {
      حالة CURLKHSTAT_FINE_جمع_کړه_منزل_FILE:
        rc = ssh_session_update_معلوم_hosts(sshc->ssh_session);
        إذا(rc != SSH_OK) {
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
    وإلا {
      إذا(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        اذهب cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  إذا(وموندل_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    حرر(وموندل_base64); /* allocated by libssh, deallocate with system free */
  }
  إذا(معلوم_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    حرر(معلوم_base64); /* allocated by libssh, deallocate with system free */
  }
  إذا(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  إذا(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_کلی_free(pubkey);
  إذا(knownhostsentry) {
    ssh_knownhosts_ثبت_free(knownhostsentry);
  }
  أرجع rc;
}

ساكن صحيح myssh_منزل_ERROR(بنية Curl_easy *معلومات,
                          بنية ssh_conn *sshc,
                          CURLcode پایله)
{
  myssh_منزل(معلومات, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = پایله;
  أرجع SSH_ERROR;
}

ساكن صحيح myssh_منزل_SFTP_CLOSE(بنية Curl_easy *معلومات,
                               بنية ssh_conn *sshc)
{
  myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_منزل_CURLE(sftp_get_error(sshc->sftp_session));
  أرجع SSH_ERROR;
}

ساكن صحيح myssh_منزل_PASSWD_AUTH(بنية Curl_easy *معلومات,
                                بنية ssh_conn *sshc)
{
  إذا(sshc->auth_methods & SSH_AUTH_لاره_PASSWORD) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_PASS_INIT);
    أرجع SSH_OK;
  }
  أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_LOGIN_DENIED);
}

ساكن صحيح myssh_منزل_کلی_AUTH(بنية Curl_easy *معلومات,
                             بنية ssh_conn *sshc)
{
  إذا(sshc->auth_methods & SSH_AUTH_لاره_INTERACTIVE) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_کلی_INIT);
    أرجع SSH_OK;
  }
  أرجع myssh_منزل_PASSWD_AUTH(معلومات, sshc);
}

ساكن صحيح myssh_منزل_GSSAPI_AUTH(بنية Curl_easy *معلومات,
                                بنية ssh_conn *sshc)
{
  إذا(sshc->auth_methods & SSH_AUTH_لاره_GSSAPI_MIC) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_GSSAPI);
    أرجع SSH_OK;
  }
  أرجع myssh_منزل_کلی_AUTH(معلومات, sshc);
}

ساكن صحيح myssh_دننه_SFTP_READDIR_INIT(بنية Curl_easy *معلومات,
                                      بنية ssh_conn *sshc,
                                      بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);
  Curl_pgrs_سېټ_Download_اندازه(معلومات, -1);
  إذا(معلومات->req.no_body) {
    myssh_منزل(معلومات, sshc, SSH_STOP);
    أرجع SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->لار);
  إذا(!sshc->sftp_dir) {
    failf(معلومات, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_منزل_SFTP_CLOSE(معلومات, sshc);
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_READDIR(بنية Curl_easy *معلومات,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp)
{
  CURLcode پایله = CURLE_OK;

  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  إذا(sshc->readdir_attrs)
    sftp_ځانګړتیاوې_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  إذا(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = مزي_اوږدوالی(sshc->readdir_filename);

    إذا(معلومات->سېټ.list_only) {
      حرف *tmp_کرښه;

      tmp_کرښه = curl_maprintf("%s\n", sshc->readdir_filename);
      إذا(!tmp_کرښه) {
        myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_بهر_د_حافظه;
        أرجع SSH_ERROR;
      }
      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                                 tmp_کرښه, sshc->readdir_len + 1);
      curlx_free(tmp_کرښه);

      إذا(پایله) {
        myssh_منزل(معلومات, sshc, SSH_STOP);
        sshc->actualcode = پایله;
        أرجع SSH_NO_ERROR;
      }
    }
    وإلا {
      إذا(curlx_dyn_جمع_کړه(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_بهر_د_حافظه;
        myssh_منزل(معلومات, sshc, SSH_STOP);
        أرجع SSH_ERROR;
      }

      إذا((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_link_لار = curl_maprintf("%s%s", sshp->لار,
                                               sshc->readdir_filename);

        إذا(!sshc->readdir_link_لار) {
          myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_بهر_د_حافظه;
          أرجع SSH_ERROR;
        }

        myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_LINK);
        أرجع SSH_NO_ERROR;
      }
      myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_BOTTOM);
      أرجع SSH_NO_ERROR;
    }
  }
  وإلا إذا(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_بشپړ);
  }
  وإلا {
    failf(معلومات, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_منزل_SFTP_CLOSE(معلومات, sshc);
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_READDIR_LINK(بنية Curl_easy *معلومات,
                                      بنية ssh_conn *sshc)
{
  إذا(sshc->readdir_link_attrs)
    sftp_ځانګړتیاوې_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_link_لار);
  إذا(!sshc->readdir_link_attrs) {
    failf(معلومات, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_منزل_SFTP_CLOSE(معلومات, sshc);
  }

  إذا(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_link_لار);
    إذا(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    وإلا
      sshc->readdir_len = مزي_اوږدوالی(sshc->readdir_tmp);
    sshc->readdir_longentry = عدم;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  وإلا {
    sshc->readdir_len = مزي_اوږدوالی(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_link_لار);

  إذا(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_بهر_د_حافظه;
    أرجع SSH_ERROR;
  }

  sftp_ځانګړتیاوې_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = عدم;
  sshc->readdir_filename = عدم;
  sshc->readdir_longentry = عدم;

  myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_BOTTOM);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_READDIR_BOTTOM(بنية Curl_easy *معلومات,
                                        بنية ssh_conn *sshc)
{
  CURLcode پایله;

  إذا(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    پایله = CURLE_بهر_د_حافظه;
  وإلا
    پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = عدم;

  إذا(پایله) {
    myssh_منزل(معلومات, sshc, SSH_STOP);
    sshc->actualcode = پایله;
  }
  وإلا
    myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_READDIR_بشپړ(بنية Curl_easy *معلومات,
                                      بنية ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = عدم;

  /* no data to transfer */
  Curl_xfer_تیاری_nop(معلومات);
  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع SSH_NO_ERROR;
}

ساكن فراغ myssh_quote_error(بنية Curl_easy *معلومات, بنية ssh_conn *sshc,
                              ثابت حرف *cmd)
{
  إذا(cmd)
    failf(معلومات, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_QUOTE_STATVFS(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  إذا(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(معلومات, sshc, "statvfs");
    أرجع SSH_OK;
  }
  وإلا إذا(statvfs) {
#که_تعریف_شوی _MSC_VER
#تعریف_کړه CURL_LIBSSH_VFS_اندازه_ماسک "I64u"
#وإلاإذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#تعریف_کړه CURL_LIBSSH_VFS_اندازه_ماسک "llu"
#وإلا
#تعریف_کړه CURL_LIBSSH_VFS_اندازه_ماسک PRIu64
#د_که_پای
    CURLcode پایله = CURLE_OK;
    حرف *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_files: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_اندازه_ماسک "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_بلاکونه, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    إذا(!tmp)
      پایله = CURLE_بهر_د_حافظه;

    إذا(!پایله) {
      پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک, tmp, مزي_اوږدوالی(tmp));
      curlx_free(tmp);
    }
    إذا(پایله) {
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = پایله;
    }
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع SSH_OK;
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
    إذا(rc == SSH_AUTH_AGAIN)
      أرجع SSH_AGAIN;

    إذا(rc != SSH_AUTH_INFO)
      أرجع SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    إذا(nprompts != 1)
      أرجع SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    إذا(rc < 0)
      أرجع SSH_ERROR;

    FALLTHROUGH();
  حالة 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, عدم, عدم);
    إذا(rc == SSH_AUTH_AGAIN)
      أرجع SSH_AGAIN;
    وإلا إذا(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    وإلا إذا(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      إذا(nprompts)
        أرجع SSH_ERROR;

      sshc->kbd_state = 2;
      اذهب restart;
    }
    وإلا
      rc = SSH_ERROR;
    اكسر;
  حالة 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, عدم, عدم);
    إذا(rc == SSH_AUTH_AGAIN)
      أرجع SSH_AGAIN;
    وإلا إذا(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    وإلا
      rc = SSH_ERROR;

    اكسر;
  افتراضي:
    أرجع SSH_ERROR;
  }

  sshc->kbd_state = 0;
  أرجع rc;
}

ساكن فراغ myssh_state_init(بنية Curl_easy *معلومات,
                             بنية ssh_conn *sshc)
{
  sshc->دوهم_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#إذا 0
  ssh_سېټ_log_level(SSH_LOG_PROTOCOL);
#د_که_پای

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_سېټ_blocking(sshc->ssh_session, 0);

  myssh_منزل(معلومات, sshc, SSH_S_STARTUP);
}

ساكن فراغ myssh_block2waitfor(بنية connectdata *conn,
                                بنية ssh_conn *sshc,
                                bool بلاک)
{
  (فراغ)conn;
  إذا(بلاک) {
    صحيح dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  وإلا
    sshc->waitfor = 0;
}

ساكن صحيح myssh_دننه_S_STARTUP(بنية Curl_easy *معلومات,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = معلومات->conn;
  صحيح rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  إذا(rc == SSH_AGAIN) {
    CURL_TRC_SSH(معلومات, "connect -> EAGAIN");
  }
  وإلا إذا(rc != SSH_OK) {
    failf(معلومات, "Failure establishing ssh session");
    rc = myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);
  }
  وإلا
    myssh_منزل(معلومات, sshc, SSH_HOSTKEY);

  أرجع rc;
}

ساكن صحيح myssh_دننه_AUTHLIST(بنية Curl_easy *معلومات,
                             بنية ssh_conn *sshc)
{
  صحيح rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, عدم);
  إذا(rc == SSH_AUTH_AGAIN)
    أرجع SSH_AGAIN;

  إذا(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(معلومات, "Authenticated with none");
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
    أرجع rc;
  }
  وإلا إذا(rc == SSH_AUTH_ERROR) {
    rc = myssh_منزل_ERROR(معلومات, sshc, CURLE_LOGIN_DENIED);
    أرجع rc;
  }

  sshc->auth_methods =
    (بینښه صحيح)ssh_userauth_list(sshc->ssh_session, عدم);
  إذا(sshc->auth_methods)
    infof(معلومات, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_لاره_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_لاره_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_لاره_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_لاره_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  إذا((sshc->auth_methods & SSH_AUTH_لاره_PUBLICKEY) &&
     ((sshc->priv_کلی && sshc->priv_کلی[0]) ||
      (معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_PKEY_INIT);
    infof(معلومات, "Authentication using SSH public key file");
  }
  وإلا إذا(sshc->auth_methods & SSH_AUTH_لاره_GSSAPI_MIC) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_GSSAPI);
  }
  وإلا إذا(sshc->auth_methods & SSH_AUTH_لاره_INTERACTIVE) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_کلی_INIT);
  }
  وإلا إذا(sshc->auth_methods & SSH_AUTH_لاره_PASSWORD) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_PASS_INIT);
  }
  وإلا {                  /* unsupported authentication method */
    rc = myssh_منزل_ERROR(معلومات, sshc, CURLE_LOGIN_DENIED);
  }
  أرجع rc;
}

ساكن صحيح myssh_دننه_AUTH_PKEY_INIT(بنية Curl_easy *معلومات,
                                   بنية ssh_conn *sshc)
{
  صحيح rc;
  إذا(!(معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_منزل_GSSAPI_AUTH(معلومات, sshc);
    أرجع rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  إذا(sshc->priv_کلی && sshc->priv_کلی[0]) {
    إذا(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, عدم, sshc->pubkey);
      إذا(rc == SSH_AUTH_AGAIN)
        أرجع SSH_AGAIN;

      إذا(rc != SSH_OK) {
        rc = myssh_منزل_GSSAPI_AUTH(معلومات, sshc);
        أرجع rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_کلی,
                                     sshc->passphrase, عدم,
                                     عدم, &sshc->privkey);
    إذا(rc != SSH_OK) {
      failf(معلومات, "Could not load private key file %s", sshc->priv_کلی);
      rc = myssh_منزل_ERROR(معلومات, sshc, CURLE_LOGIN_DENIED);
      أرجع rc;
    }

    myssh_منزل(معلومات, sshc, SSH_AUTH_PKEY);
  }
  وإلا {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, عدم,
                                     sshc->passphrase);
    إذا(rc == SSH_AUTH_AGAIN)
      أرجع SSH_AGAIN;

    إذا(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(معلومات, "Completed public key authentication");
      myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
      أرجع rc;
    }

    rc = myssh_منزل_GSSAPI_AUTH(معلومات, sshc);
  }
  أرجع rc;
}

ساكن صحيح myssh_دننه_AUTH_PKEY(بنية Curl_easy *معلومات,
                              بنية ssh_conn *sshc)
{
  صحيح rc = ssh_userauth_publickey(sshc->ssh_session, عدم, sshc->privkey);
  إذا(rc == SSH_AUTH_AGAIN)
    أرجع SSH_AGAIN;
  وإلا إذا(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(معلومات, "Completed public key authentication");
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
    أرجع SSH_OK;
  }
  وإلا {
    infof(معلومات, "Failed public key authentication (rc: %d)", rc);
    أرجع myssh_منزل_GSSAPI_AUTH(معلومات, sshc);
  }
}

ساكن صحيح myssh_دننه_AUTH_GSSAPI(بنية Curl_easy *معلومات,
                                بنية ssh_conn *sshc)
{
  صحيح rc;
  إذا(!(معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    أرجع myssh_منزل_کلی_AUTH(معلومات, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  إذا(rc == SSH_AUTH_AGAIN)
    أرجع SSH_AGAIN;

  إذا(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(معلومات, "Completed gssapi authentication");
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
    أرجع SSH_OK;
  }

  أرجع myssh_منزل_کلی_AUTH(معلومات, sshc);
}

ساكن صحيح myssh_دننه_AUTH_کلی_INIT(بنية Curl_easy *معلومات,
                                  بنية ssh_conn *sshc)
{
  إذا(معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_کلی);
    أرجع SSH_NO_ERROR;
  }
  أرجع myssh_منزل_PASSWD_AUTH(معلومات, sshc);
}

ساكن صحيح myssh_دننه_AUTH_کلی(بنية Curl_easy *معلومات,
                             بنية ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  صحيح rc = myssh_auth_interactive(معلومات->conn, sshc);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  وإلا إذا(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(معلومات, "completed keyboard interactive authentication");
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
    أرجع SSH_NO_ERROR;
  }
  وإلا
    أرجع myssh_منزل_PASSWD_AUTH(معلومات, sshc);
}

ساكن صحيح myssh_دننه_AUTH_PASS_INIT(بنية Curl_easy *معلومات,
                                   بنية ssh_conn *sshc)
{
  إذا(!(معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_LOGIN_DENIED);
  myssh_منزل(معلومات, sshc, SSH_AUTH_PASS);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_AUTH_PASS(بنية Curl_easy *معلومات,
                              بنية ssh_conn *sshc)
{
  صحيح rc = ssh_userauth_password(sshc->ssh_session, عدم,
                                 Curl_creds_passwd(معلومات->conn->creds));
  إذا(rc == SSH_AUTH_AGAIN)
    أرجع SSH_AGAIN;
  وإلا إذا(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(معلومات, "Completed password authentication");
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
    أرجع SSH_NO_ERROR;
  }
  أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_LOGIN_DENIED);
}

ساكن صحيح myssh_دننه_AUTH_بشپړ(بنية Curl_easy *معلومات,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = معلومات->conn;
  إذا(!sshc->authed) {
    failf(معلومات, "Authentication failure");
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(معلومات, "Authentication complete");
  Curl_pgrs_وخت(معلومات, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  إذا(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_منزل(معلومات, sshc, SSH_SFTP_INIT);
    أرجع SSH_NO_ERROR;
  }
  infof(معلومات, "SSH CONNECT phase done");
  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_UPLOAD_INIT(بنية Curl_easy *معلومات,
                                بنية ssh_conn *sshc,
                                بنية SSHPROTO *sshp)
{
  صحيح flags;
  صحيح rc = 0;

  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);

  إذا(معلومات->state.resume_from) {
    sftp_ځانګړتیاوې attrs;

    إذا(معلومات->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->لار);
      إذا(attrs) {
        curl_off_t اندازه = attrs->اندازه;
        sftp_ځانګړتیاوې_free(attrs);
        إذا(اندازه < 0) {
          failf(معلومات, "Bad file size (%" FMT_OFF_T ")", اندازه);
          rc = myssh_منزل_ERROR(معلومات, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          أرجع rc;
        }
        معلومات->state.resume_from = اندازه;
      }
      وإلا {
        معلومات->state.resume_from = 0;
      }
    }
  }

  إذا(معلومات->سېټ.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  وإلا إذا(معلومات->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  وإلا
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  إذا(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->لار,
              flags, (mode_t)معلومات->سېټ.new_file_perms);
  إذا(!sshc->sftp_file) {
    صحيح err = sftp_get_error(sshc->sftp_session);

    إذا((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_لار) &&
       معلومات->سېټ.ftp_create_missing_dirs &&
       (مزي_اوږدوالی(sshp->لار) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->دوهم_Create_Dirs = 1;
      myssh_منزل(معلومات, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      أرجع rc;
    }
    وإلا {
      rc = myssh_منزل_SFTP_CLOSE(معلومات, sshc);
      أرجع rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  إذا(معلومات->state.resume_from > 0 && !معلومات->سېټ.remote_append) {
    صحيح seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    إذا(معلومات->سېټ.seek_func) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_پیل(&guard, معلومات, easy_seek_func);
      seekerr = معلومات->سېټ.seek_func(معلومات->سېټ.seek_client,
                                    معلومات->state.resume_from, SEEK_SET);
      CURL_CBAPI_پای_ټکی(&guard);
    }

    إذا(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(معلومات, "Could not seek stream");
        rc = myssh_منزل_ERROR(معلومات, sshc, CURLE_FTP_COULDNT_USE_REST);
        أرجع rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      افعل {
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (معلومات->state.resume_from - passed >
           (curl_off_t)حجم(scratch)) ?
          حجم(scratch) : curlx_sotouz(معلومات->state.resume_from - passed);

        size_t actuallyread =
          معلومات->state.fread_func(scratch, 1,
                                 readthisamountnow, معلومات->state.دننه);

        passed += actuallyread;
        إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(معلومات, "Failed to read data");
          rc = myssh_منزل_ERROR(معلومات, sshc, CURLE_FTP_COULDNT_USE_REST);
          أرجع rc;
        }
      } طالما(passed < معلومات->state.resume_from);
    }

    /* now, decrease the size of the read */
    إذا(معلومات->state.infilesize > 0) {
      إذا(معلومات->state.resume_from > معلومات->state.infilesize) {
        failf(معلومات, "Resume point beyond size");
        أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_BAD_FUNCTION_دلیل);
      }
      معلومات->state.infilesize -= معلومات->state.resume_from;
      معلومات->req.اندازه = معلومات->state.infilesize;
      Curl_pgrs_سېټ_Upload_اندازه(معلومات, معلومات->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, معلومات->state.resume_from);
    إذا(rc) {
      rc = myssh_منزل_SFTP_CLOSE(معلومات, sshc);
      أرجع rc;
    }
  }
  إذا(معلومات->state.infilesize > 0) {
    معلومات->req.اندازه = معلومات->state.infilesize;
    Curl_pgrs_سېټ_Upload_اندازه(معلومات, معلومات->state.infilesize);
  }
  /* upload data */
  Curl_xfer_تیاری_send(معلومات, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  معلومات->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(معلومات);
#إذا LIBSSH_نسخه_INT > SSH_نسخه_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#د_که_پای
  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع rc;
}

ساكن صحيح myssh_دننه_SFTP_DOWNLOAD_INIT(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc,
                                       بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  إذا(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->لار,
                              O_RDONLY, (mode_t)معلومات->سېټ.new_file_perms);
  إذا(!sshc->sftp_file) {
    failf(معلومات, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    أرجع myssh_منزل_SFTP_CLOSE(معلومات, sshc);
  }
  sftp_file_سېټ_nonblocking(sshc->sftp_file);
  myssh_منزل(معلومات, sshc, SSH_SFTP_DOWNLOAD_STAT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_DOWNLOAD_STAT(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc)
{
  curl_off_t اندازه;
  صحيح rc = 0;
  sftp_ځانګړتیاوې attrs = sftp_fstat(sshc->sftp_file);
  إذا(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_اندازه) ||
     (attrs->اندازه == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    معلومات->req.اندازه = -1;
    معلومات->req.maxdownload = -1;
    Curl_pgrs_سېټ_Download_اندازه(معلومات, -1);
    اندازه = 0;
    إذا(attrs)
      sftp_ځانګړتیاوې_free(attrs);
  }
  وإلا {
    اندازه = attrs->اندازه;

    sftp_ځانګړتیاوې_free(attrs);

    إذا(اندازه < 0) {
      failf(معلومات, "Bad file size (%" FMT_OFF_T ")", اندازه);
      أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    إذا(معلومات->state.use_range) {
      curl_off_t from;
      CURLcode پایله = Curl_ssh_range(معلومات, معلومات->state.range, اندازه,
                                       &from, &اندازه);
      إذا(پایله)
        أرجع myssh_منزل_ERROR(معلومات, sshc, پایله);

      rc = sftp_seek64(sshc->sftp_file, from);
      إذا(rc)
        أرجع myssh_منزل_SFTP_CLOSE(معلومات, sshc);
    }
    معلومات->req.اندازه = اندازه;
    معلومات->req.maxdownload = اندازه;
    Curl_pgrs_سېټ_Download_اندازه(معلومات, اندازه);
  }

  /* We can resume if we can seek to the resume position */
  إذا(معلومات->state.resume_from) {
    إذا(معلومات->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      إذا(اندازه < -معلومات->state.resume_from) {
        failf(معلومات, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", معلومات->state.resume_from, اندازه);
        أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      معلومات->state.resume_from += اندازه;
    }
    وإلا {
      إذا(اندازه < معلومات->state.resume_from) {
        failf(معلومات, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              معلومات->state.resume_from, اندازه);
        أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    معلومات->req.اندازه = اندازه - معلومات->state.resume_from;
    معلومات->req.maxdownload = اندازه - معلومات->state.resume_from;
    Curl_pgrs_سېټ_Download_اندازه(معلومات, اندازه - معلومات->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, معلومات->state.resume_from);
    إذا(rc)
      أرجع myssh_منزل_SFTP_CLOSE(معلومات, sshc);
  }

  /* Setup the actual download */
  إذا(معلومات->req.اندازه == 0) {
    /* no data to transfer */
    Curl_xfer_تیاری_nop(معلومات);
    infof(معلومات, "File already completely downloaded");
    myssh_منزل(معلومات, sshc, SSH_STOP);
    أرجع rc;
  }
  Curl_xfer_تیاری_recv(معلومات, FIRSTSOCKET, معلومات->req.اندازه);

  /* not set by Curl_xfer_setup to preserve io_flags */
  معلومات->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_منزل(معلومات, sshc, SSH_STOP);

  أرجع rc;
}

ساكن صحيح myssh_دننه_SFTP_CLOSE(بنية Curl_easy *معلومات,
                               بنية ssh_conn *sshc,
                               بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);
  إذا(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = عدم;
  }
  curlx_safefree(sshp->لار);

  CURL_TRC_SSH(معلومات, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  إذا(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_منزل(معلومات, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_STOP);
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_SHUTDOWN(بنية Curl_easy *معلومات,
                                  بنية ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_سېټ_blocking(sshc->ssh_session, 0);
#إذا LIBSSH_نسخه_INT > SSH_نسخه_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#د_که_پای

  إذا(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = عدم;
  }

  إذا(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = عدم;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_منزل(معلومات, sshc, SSH_SESSION_DISCONNECT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_INIT(بنية Curl_easy *معلومات,
                              بنية ssh_conn *sshc)
{
  صحيح rc;
  ssh_سېټ_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  إذا(!sshc->sftp_session) {
    failf(معلومات, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  إذا(rc != SSH_OK) {
    failf(معلومات, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_منزل_ERROR(معلومات, sshc, sftp_error_منزل_CURLE(SSH_FX_FAILURE));
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_REALPATH);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_REALPATH(بنية Curl_easy *معلومات,
                                  بنية ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_لار(sshc->sftp_session, ".");
  إذا(!sshc->homedir)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(معلومات->state.most_recent_ftp_entrypath);
  معلومات->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  إذا(!معلومات->state.most_recent_ftp_entrypath)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_بهر_د_حافظه);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(معلومات, "CONNECT phase done");
  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_QUOTE_INIT(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp)
{
  CURLcode پایله;

  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);

  پایله = Curl_getworkingpath(معلومات, sshc->homedir, &sshp->لار);
  إذا(پایله) {
    sshc->actualcode = پایله;
    myssh_منزل(معلومات, sshc, SSH_STOP);
  }
  وإلا إذا(معلومات->سېټ.quote) {
    infof(معلومات, "Sending quote commands");
    sshc->quote_توکی = معلومات->سېټ.quote;
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE);
  }
  وإلا
    myssh_منزل(معلومات, sshc, SSH_SFTP_GETINFO);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_POSTQUOTE_INIT(بنية Curl_easy *معلومات,
                                        بنية ssh_conn *sshc)
{
  إذا(معلومات->سېټ.postquote) {
    infof(معلومات, "Sending quote commands");
    sshc->quote_توکی = معلومات->سېټ.postquote;
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_STOP);
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح quote_error(بنية Curl_easy *معلومات,
                       بنية ssh_conn *sshc)
{
  failf(معلومات, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_QUOTE(بنية Curl_easy *معلومات,
                               بنية ssh_conn *sshc,
                               بنية SSHPROTO *sshp)
{
  ثابت حرف *cp;
  CURLcode پایله;

  /*
   * Support some of the "FTP" commands
   */
  ثابت حرف *cmd = sshc->quote_توکی->معلومات;
  sshc->acceptfail = FALSE;

  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  إذا(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  إذا(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    حرف *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->لار);
    إذا(!tmp) {
      sshc->actualcode = CURLE_بهر_د_حافظه;
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع SSH_NO_ERROR;
    }
    Curl_debug(معلومات, CURLINFO_سرلیک_بهر, "PWD\n", 4);
    Curl_debug(معلومات, CURLINFO_سرلیک_دننه, tmp, مزي_اوږدوالی(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک, tmp, مزي_اوږدوالی(tmp));
    curlx_free(tmp);
    إذا(پایله) {
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = پایله;
    }
    وإلا
      myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
    أرجع SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  إذا(!cp) {
    failf(معلومات, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    أرجع SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  پایله = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  إذا(پایله) {
    إذا(پایله == CURLE_بهر_د_حافظه)
      failf(معلومات, "Out of memory");
    وإلا
      failf(معلومات, "Syntax error: Bad first parameter");
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = پایله;
    أرجع SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  إذا(!مزي_پرتله(cmd, "chgrp ", 6) ||
     !مزي_پرتله(cmd, "chmod ", 6) ||
     !مزي_پرتله(cmd, "chown ", 6) ||
     !مزي_پرتله(cmd, "atime ", 6) ||
     !مزي_پرتله(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    پایله = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(پایله) {
      إذا(پایله == CURLE_بهر_د_حافظه)
        failf(معلومات, "Out of memory");
      وإلا
        failf(معلومات, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = پایله;
      أرجع SSH_NO_ERROR;
    }
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    sshc->quote_attrs = عدم;
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_STAT);
    أرجع SSH_NO_ERROR;
  }
  إذا(!مزي_پرتله(cmd, "ln ", 3) ||
     !مزي_پرتله(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    پایله = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(پایله) {
      إذا(پایله == CURLE_بهر_د_حافظه)
        failf(معلومات, "Out of memory");
      وإلا
        failf(معلومات, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = پایله;
      أرجع SSH_NO_ERROR;
    }
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_SYMLINK);
    أرجع SSH_NO_ERROR;
  }
  وإلا إذا(!مزي_پرتله(cmd, "mkdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    /* create directory */
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_MKDIR);
    أرجع SSH_NO_ERROR;
  }
  وإلا إذا(!مزي_پرتله(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    پایله = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(پایله) {
      إذا(پایله == CURLE_بهر_د_حافظه)
        failf(معلومات, "Out of memory");
      وإلا
        failf(معلومات, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = پایله;
      أرجع SSH_NO_ERROR;
    }
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_RENAME);
    أرجع SSH_NO_ERROR;
  }
  وإلا إذا(!مزي_پرتله(cmd, "rmdir ", 6)) {
    /* delete directory */
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_RMDIR);
    أرجع SSH_NO_ERROR;
  }
  وإلا إذا(!مزي_پرتله(cmd, "rm ", 3)) {
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_UNLINK);
    أرجع SSH_NO_ERROR;
  }
#که_تعریف_شوی HAS_STATVFS_SUPPORT
  وإلا إذا(!مزي_پرتله(cmd, "statvfs ", 8)) {
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_STATVFS);
    أرجع SSH_NO_ERROR;
  }
#د_که_پای

  failf(معلومات, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_بل_QUOTE(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_توکی = sshc->quote_توکی->بل;

  إذا(sshc->quote_توکی) {
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    إذا(sshc->nextstate != SSH_NO_STATE) {
      myssh_منزل(معلومات, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    وإلا {
      myssh_منزل(معلومات, sshc, SSH_SFTP_GETINFO);
    }
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_QUOTE_STAT(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc)
{
  ثابت حرف *cmd = sshc->quote_توکی->معلومات;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  إذا(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  إذا(sshc->quote_attrs)
    sftp_ځانګړتیاوې_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  إذا(!sshc->quote_attrs) {
    failf(معلومات, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(معلومات, sshc, عدم);
    أرجع SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  إذا(!مزي_پرتله(cmd, "chgrp", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t gid;
    إذا(curlx_str_شمېره(&p, &gid, UINT_MAX)) {
      failf(معلومات, "Syntax error: chgrp gid not a number");
      myssh_quote_error(معلومات, sshc, عدم);
      أرجع SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  وإلا إذا(!مزي_پرتله(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت حرف *p = sshc->quote_path1;
    إذا(curlx_str_octal(&p, &perms, 07777)) {
      failf(معلومات, "Syntax error: chmod permissions not a number");
      myssh_quote_error(معلومات, sshc, عدم);
      أرجع SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  وإلا إذا(!مزي_پرتله(cmd, "chown", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t uid;
    إذا(curlx_str_شمېره(&p, &uid, UINT_MAX)) {
      failf(معلومات, "Syntax error: chown uid not a number");
      myssh_quote_error(معلومات, sshc, عدم);
      أرجع SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  وإلا إذا(!مزي_پرتله(cmd, "atime", 5) ||
          !مزي_پرتله(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    إذا(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(معلومات, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#إذا SIZEOF_وخت_T > 4
    وإلا إذا(date > 0xffffffff) {
      failf(معلومات, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#د_که_پای
    إذا(fail) {
      myssh_quote_error(معلومات, sshc, عدم);
      أرجع SSH_NO_ERROR;
    }
    إذا(!مزي_پرتله(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    وإلا /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_SETSTAT);
  أرجع SSH_NO_ERROR;
}

ساكن فراغ conn_forget_socket(بنية Curl_easy *معلومات, int8_t sockindex)
{
  بنية connectdata *conn = معلومات->conn;
  إذا(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    بنية Curl_cfilter *cf = conn->cfilter[sockindex];
    إذا(cf)
      (فراغ)Curl_conn_cf_cntrl(cf, معلومات, TRUE, CF_CTRL_FORGET_SOCKET, 0, عدم);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

ساكن فراغ myssh_SESSION_DISCONNECT(بنية Curl_easy *معلومات,
                                     بنية ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  إذا(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = عدم;
  }

  إذا(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = عدم;
  }
  إذا(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = عدم;
  }

  ssh_disconnect(sshc->ssh_session);
  إذا(!ssh_نسخه(SSH_نسخه_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(معلومات, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
}

ساكن صحيح myssh_SSH_SCP_DOWNLOAD(بنية Curl_easy *معلومات,
                                  بنية ssh_conn *sshc)
{
  uint64_t bytecount;
  صحيح rc = ssh_scp_pull_request(sshc->scp_session);
  إذا(rc != SSH_SCP_REQUEST_NEWFILE) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(معلومات, "%s", err_پیغام);
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_REMOTE_FILE_NOT_وموندل);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  إذا(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_TOO_LARGE);
  معلومات->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_تیاری_recv(معلومات, FIRSTSOCKET, معلومات->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  معلومات->conn->send_idx = 0;

  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع 0;
}

ساكن صحيح myssh_دننه_TRANS_INIT(بنية Curl_easy *معلومات, بنية ssh_conn *sshc,
                               بنية SSHPROTO *sshp)
{
  CURLcode پایله;
  صحيح rc = 0;
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);

  پایله = Curl_getworkingpath(معلومات, sshc->homedir, &sshp->لار);
  إذا(پایله) {
    sshc->actualcode = پایله;
    myssh_منزل(معلومات, sshc, SSH_STOP);
    أرجع 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_سېټ_blocking(sshc->ssh_session, 1);

  إذا(معلومات->state.upload) {
    إذا(معلومات->state.infilesize < 0) {
      failf(معلومات, "SCP requires a known file size for upload");
      أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->لار);
    myssh_منزل(معلومات, sshc, SSH_SCP_UPLOAD_INIT);
  }
  وإلا {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->لار);
    myssh_منزل(معلومات, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  إذا(!sshc->scp_session) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(معلومات, "%s", err_پیغام);
    rc = myssh_منزل_ERROR(معلومات, sshc, CURLE_UPLOAD_FAILED);
  }
  أرجع rc;
}

ساكن فراغ sshc_cleanup(بنية ssh_conn *sshc)
{
  إذا(sshc->initialized) {
    إذا(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = عدم;
    }
    إذا(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = عدم;
    }
    إذا(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = عدم;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    إذا(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = عدم;
    }
    إذا(sshc->quote_attrs) {
      sftp_ځانګړتیاوې_free(sshc->quote_attrs);
      sshc->quote_attrs = عدم;
    }
    إذا(sshc->readdir_attrs) {
      sftp_ځانګړتیاوې_free(sshc->readdir_attrs);
      sshc->readdir_attrs = عدم;
    }
    إذا(sshc->readdir_link_attrs) {
      sftp_ځانګړتیاوې_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = عدم;
    }
    إذا(sshc->privkey) {
      ssh_کلی_free(sshc->privkey);
      sshc->privkey = عدم;
    }
    إذا(sshc->pubkey) {
      ssh_کلی_free(sshc->pubkey);
      sshc->pubkey = عدم;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_کلی);
    curlx_safefree(sshc->priv_کلی);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_link_لار);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

ساكن صحيح myssh_دننه_SFTP_QUOTE_SETSTAT(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc)
{
  صحيح rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(معلومات, sshc, "setstat");
    أرجع rc;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_QUOTE_SYMLINK(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc)
{
  صحيح rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(معلومات, sshc, "symlink");
    أرجع rc;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_QUOTE_MKDIR(بنية Curl_easy *معلومات,
                                     بنية ssh_conn *sshc)
{
  صحيح rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)معلومات->سېټ.new_directory_perms);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(معلومات, sshc, "mkdir");
    أرجع rc;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_QUOTE_RENAME(بنية Curl_easy *معلومات,
                                      بنية ssh_conn *sshc)
{
  صحيح rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(معلومات, sshc, "rename");
    أرجع rc;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_QUOTE_RMDIR(بنية Curl_easy *معلومات,
                                     بنية ssh_conn *sshc)
{
  صحيح rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(معلومات, sshc, "rmdir");
    أرجع rc;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_QUOTE_UNLINK(بنية Curl_easy *معلومات,
                                      بنية ssh_conn *sshc)
{
  صحيح rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(معلومات, sshc, "rm");
    أرجع rc;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_GETINFO(بنية Curl_easy *معلومات,
                                 بنية ssh_conn *sshc)
{
  إذا(معلومات->سېټ.get_filetime)
    myssh_منزل(معلومات, sshc, SSH_SFTP_FILETIME);
  وإلا
    myssh_منزل(معلومات, sshc, SSH_SFTP_TRANS_INIT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_FILETIME(بنية Curl_easy *معلومات,
                                  بنية ssh_conn *sshc,
                                  بنية SSHPROTO *sshp)
{
  sftp_ځانګړتیاوې attrs;
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->لار);
  إذا(attrs) {
    معلومات->info.filetime = attrs->mtime;
    sftp_ځانګړتیاوې_free(attrs);
  }

  myssh_منزل(معلومات, sshc, SSH_SFTP_TRANS_INIT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_TRANS_INIT(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);
  إذا(معلومات->state.upload)
    myssh_منزل(معلومات, sshc, SSH_SFTP_UPLOAD_INIT);
  وإلا {
    size_t لار_len = مزي_اوږدوالی(sshp->لار);

    إذا(لار_len && sshp->لار[لار_len - 1] == '/')
      myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_INIT);
    وإلا
      myssh_منزل(معلومات, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_CREATE_DIRS_INIT(بنية Curl_easy *معلومات,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);

  إذا(مزي_اوږدوالی(sshp->لار) > 1) {
    sshc->slash_pos = sshp->لار + 1; /* ignore the leading '/' */
    myssh_منزل(معلومات, sshc, SSH_SFTP_CREATE_DIRS);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_CREATE_DIRS(بنية Curl_easy *معلومات,
                                     بنية ssh_conn *sshc,
                                     بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  إذا(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(معلومات, "Creating directory '%s'", sshp->لار);
    myssh_منزل(معلومات, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    أرجع SSH_NO_ERROR;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SFTP_CREATE_DIRS_MKDIR(بنية Curl_easy *معلومات,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp)
{
  صحيح rc;
  صحيح err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->لار,
                  (mode_t)معلومات->سېټ.new_directory_perms);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  إذا(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    إذا((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      أرجع myssh_منزل_SFTP_CLOSE(معلومات, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_CREATE_DIRS);
  أرجع rc;
}

ساكن صحيح myssh_دننه_SCP_UPLOAD_INIT(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp)
{
  صحيح rc;
  إذا(!sshp)
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  إذا(rc != SSH_OK) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(معلومات, "%s", err_پیغام);
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ټېل_file64(sshc->scp_session, sshp->لار,
                           (uint64_t)معلومات->state.infilesize,
                           (صحيح)معلومات->سېټ.new_file_perms);

  إذا(rc != SSH_OK) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(معلومات, "%s", err_پیغام);
    أرجع myssh_منزل_ERROR(معلومات, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_تیاری_send(معلومات, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  معلومات->conn->recv_idx = FIRSTSOCKET;

  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SCP_DOWNLOAD_INIT(بنية Curl_easy *معلومات,
                                      بنية ssh_conn *sshc)
{
  صحيح rc = ssh_scp_init(sshc->scp_session);
  إذا(rc != SSH_OK) {
    ثابت حرف *err_پیغام = ssh_get_error(sshc->ssh_session);
    failf(معلومات, "%s", err_پیغام);
    أرجع myssh_منزل_ERROR(معلومات, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_منزل(معلومات, sshc, SSH_SCP_DOWNLOAD);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SCP_بشپړ(بنية Curl_easy *معلومات,
                             بنية ssh_conn *sshc)
{
  إذا(معلومات->state.upload)
    myssh_منزل(معلومات, sshc, SSH_SCP_SEND_EOF);
  وإلا
    myssh_منزل(معلومات, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SCP_SEND_EOF(بنية Curl_easy *معلومات,
                                 بنية ssh_conn *sshc)
{
  إذا(sshc->scp_session) {
    صحيح rc = ssh_scp_close(sshc->scp_session);
    إذا(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      أرجع SSH_AGAIN;
    }
    إذا(rc != SSH_OK) {
      infof(معلومات,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_منزل(معلومات, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_دننه_SCP_CHANNEL_FREE(بنية Curl_easy *معلومات,
                                     بنية ssh_conn *sshc)
{
  إذا(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = عدم;
  }
  CURL_TRC_SSH(معلومات, "SCP DONE phase complete");

  ssh_سېټ_blocking(sshc->ssh_session, 0);

  myssh_منزل(معلومات, sshc, SSH_SESSION_DISCONNECT);
  أرجع SSH_NO_ERROR;
}

ساكن CURLcode myssh_دننه_SESSION_FREE(بنية Curl_easy *معلومات,
                                      بنية ssh_conn *sshc)
{
  CURLcode پایله;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  پایله = sshc->actualcode;
  memset(sshc, 0, حجم(بنية ssh_conn));
  connclose(معلومات->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع پایله;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
ساكن CURLcode myssh_statemachine(بنية Curl_easy *معلومات,
                                   بنية ssh_conn *sshc,
                                   بنية SSHPROTO *sshp,
                                   bool *بلاک)
{
  CURLcode پایله = CURLE_OK;
  صحيح rc = SSH_NO_ERROR;

  *بلاک = FALSE;                   /* we are not blocking by default */
  افعل {

    بدل(sshc->state) {
    حالة SSH_INIT:
      myssh_state_init(معلومات, sshc);
      FALLTHROUGH();
    حالة SSH_S_STARTUP:
      rc = myssh_دننه_S_STARTUP(معلومات, sshc);
      إذا(rc)
        اكسر;
      FALLTHROUGH();
    حالة SSH_HOSTKEY:
      rc = myssh_is_معلوم(معلومات, sshc);
      إذا(rc != SSH_OK) {
        rc = myssh_منزل_ERROR(معلومات, sshc, CURLE_PEER_FAILED_VERIFICATION);
        اكسر;
      }
      myssh_منزل(معلومات, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    حالة SSH_AUTHLIST:
      rc = myssh_دننه_AUTHLIST(معلومات, sshc);
      اكسر;
    حالة SSH_AUTH_PKEY_INIT:
      rc = myssh_دننه_AUTH_PKEY_INIT(معلومات, sshc);
      اكسر;
    حالة SSH_AUTH_PKEY:
      rc = myssh_دننه_AUTH_PKEY(معلومات, sshc);
      اكسر;
    حالة SSH_AUTH_GSSAPI:
      rc = myssh_دننه_AUTH_GSSAPI(معلومات, sshc);
      اكسر;
    حالة SSH_AUTH_کلی_INIT:
      rc = myssh_دننه_AUTH_کلی_INIT(معلومات, sshc);
      اكسر;
    حالة SSH_AUTH_کلی:
      rc = myssh_دننه_AUTH_کلی(معلومات, sshc);
      اكسر;
    حالة SSH_AUTH_PASS_INIT:
      rc = myssh_دننه_AUTH_PASS_INIT(معلومات, sshc);
      اكسر;
    حالة SSH_AUTH_PASS:
      rc = myssh_دننه_AUTH_PASS(معلومات, sshc);
      اكسر;
    حالة SSH_AUTH_بشپړ:
      rc = myssh_دننه_AUTH_بشپړ(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_INIT:
      rc = myssh_دننه_SFTP_INIT(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_REALPATH:
      rc = myssh_دننه_SFTP_REALPATH(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_INIT:
      rc = myssh_دننه_SFTP_QUOTE_INIT(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_دننه_SFTP_POSTQUOTE_INIT(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE:
      rc = myssh_دننه_SFTP_QUOTE(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_بل_QUOTE:
      rc = myssh_دننه_SFTP_بل_QUOTE(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_STAT:
      rc = myssh_دننه_SFTP_QUOTE_STAT(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_دننه_SFTP_QUOTE_SETSTAT(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_دننه_SFTP_QUOTE_SYMLINK(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_دننه_SFTP_QUOTE_MKDIR(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_RENAME:
      rc = myssh_دننه_SFTP_QUOTE_RENAME(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_دننه_SFTP_QUOTE_RMDIR(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_دننه_SFTP_QUOTE_UNLINK(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_دننه_SFTP_QUOTE_STATVFS(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_GETINFO:
      rc = myssh_دننه_SFTP_GETINFO(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_FILETIME:
      rc = myssh_دننه_SFTP_FILETIME(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_TRANS_INIT:
      rc = myssh_دننه_SFTP_TRANS_INIT(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_UPLOAD_INIT:
      rc = myssh_دننه_UPLOAD_INIT(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_دننه_SFTP_CREATE_DIRS_INIT(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_CREATE_DIRS:
      rc = myssh_دننه_SFTP_CREATE_DIRS(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_دننه_SFTP_CREATE_DIRS_MKDIR(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_READDIR_INIT:
      rc = myssh_دننه_SFTP_READDIR_INIT(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_READDIR:
      rc = myssh_دننه_SFTP_READDIR(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_READDIR_LINK:
      rc = myssh_دننه_SFTP_READDIR_LINK(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_دننه_SFTP_READDIR_BOTTOM(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_READDIR_بشپړ:
      rc = myssh_دننه_SFTP_READDIR_بشپړ(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_دننه_SFTP_DOWNLOAD_INIT(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_دننه_SFTP_DOWNLOAD_STAT(معلومات, sshc);
      اكسر;
    حالة SSH_SFTP_CLOSE:
      rc = myssh_دننه_SFTP_CLOSE(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_SHUTDOWN:
      rc = myssh_دننه_SFTP_SHUTDOWN(معلومات, sshc);
      اكسر;
    حالة SSH_SCP_TRANS_INIT:
      rc = myssh_دننه_TRANS_INIT(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SCP_UPLOAD_INIT:
      rc = myssh_دننه_SCP_UPLOAD_INIT(معلومات, sshc, sshp);
      اكسر;
    حالة SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_دننه_SCP_DOWNLOAD_INIT(معلومات, sshc);
      إذا(rc)
        اكسر;
      FALLTHROUGH();
    حالة SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(معلومات, sshc);
      اكسر;
    حالة SSH_SCP_بشپړ:
      rc = myssh_دننه_SCP_بشپړ(معلومات, sshc);
      اكسر;
    حالة SSH_SCP_SEND_EOF:
      rc = myssh_دننه_SCP_SEND_EOF(معلومات, sshc);
      اكسر;
    حالة SSH_SCP_CHANNEL_FREE:
      myssh_دننه_SCP_CHANNEL_FREE(معلومات, sshc);
      FALLTHROUGH();
    حالة SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(معلومات, sshc);
      FALLTHROUGH();
    حالة SSH_SESSION_FREE:
      پایله = myssh_دننه_SESSION_FREE(معلومات, sshc);
      اكسر;
    حالة SSH_QUIT:
    افتراضي:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_منزل(معلومات, sshc, SSH_STOP);
      اكسر;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } طالما((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  إذا(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *بلاک = TRUE;
  }
  إذا(!پایله && (sshc->state == SSH_STOP))
    پایله = sshc->actualcode;
  CURL_TRC_SSH(معلومات, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحيح)پایله, *بلاک);
  أرجع پایله;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode myssh_multi_statemach(بنية Curl_easy *معلومات,
                                      bool *بشپړ)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(معلومات, CURL_META_SSH_EASY);
  bool بلاک;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode پایله;

  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;
  پایله = myssh_statemachine(معلومات, sshc, sshp, &بلاک);
  *بشپړ = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, بلاک);

  أرجع پایله;
}

ساكن CURLcode myssh_بلاک_statemach(بنية Curl_easy *معلومات,
                                      بنية ssh_conn *sshc,
                                      بنية SSHPROTO *sshp,
                                      bool disconnect)
{
  بنية connectdata *conn = معلومات->conn;
  CURLcode پایله = CURLE_OK;

  طالما((sshc->state != SSH_STOP) && !پایله) {
    bool بلاک;
    timediff_t کیڼ_ms = 1000;

    پایله = myssh_statemachine(معلومات, sshc, sshp, &بلاک);
    إذا(پایله)
      اكسر;

    إذا(!disconnect) {
      پایله = Curl_pgrsCheck(معلومات);
      إذا(پایله)
        اكسر;

      کیڼ_ms = Curl_timeleft_ms(معلومات);
      إذا(کیڼ_ms < 0) {
        failf(معلومات, "Operation timed out");
        أرجع CURLE_OPERATION_TIMEDOUT;
      }
    }

    إذا(بلاک) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (فراغ)SOCKET_READABLE(fd_read,
                            (!کیڼ_ms || کیڼ_ms > 1000) ? 1000 : کیڼ_ms);
    }
  }

  أرجع پایله;
}

ساكن فراغ myssh_easy_dtor(ثابت فراغ *کلی, size_t klen, فراغ *ثبت)
{
  بنية SSHPROTO *sshp = ثبت;
  (فراغ)کلی;
  (فراغ)klen;
  curlx_safefree(sshp->لار);
  curlx_free(sshp);
}

ساكن فراغ myssh_conn_dtor(ثابت فراغ *کلی, size_t klen, فراغ *ثبت)
{
  بنية ssh_conn *sshc = ثبت;
  (فراغ)کلی;
  (فراغ)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
ساكن CURLcode myssh_تیاری_connection(بنية Curl_easy *معلومات,
                                       بنية connectdata *conn)
{
  بنية SSHPROTO *sshp;
  بنية ssh_conn *sshc;

  sshc = curlx_calloc(1, حجم(*sshc));
  إذا(!sshc)
    أرجع CURLE_بهر_د_حافظه;

  curlx_dyn_init(&sshc->readdir_buf, CURL_لار_MAX * 2);
  sshc->initialized = TRUE;
  إذا(Curl_conn_meta_سېټ(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    أرجع CURLE_بهر_د_حافظه;

  sshp = curlx_calloc(1, حجم(*sshp));
  إذا(!sshp ||
     Curl_meta_سېټ(معلومات, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    أرجع CURLE_بهر_د_حافظه;

  أرجع Curl_ssh_تیاری_pkey(معلومات, sshc);
}

ساكن Curl_recv scp_recv, sftp_recv;
ساكن Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ساكن CURLcode myssh_connect(بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله;
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *ssh = Curl_meta_get(معلومات, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح rc;

  إذا(!sshc || !ssh)
    أرجع CURLE_FAILED_INIT;

  CURL_TRC_SSH(معلومات, "myssh_connect");
  إذا(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  وإلا {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  إذا(!sshc->ssh_session) {
    failf(معلومات, "Failure initializing ssh session");
    أرجع CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_سېټ(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  إذا(rc != SSH_OK) {
    failf(معلومات, "Could not set remote host");
    أرجع CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, عدم);
  إذا(rc != SSH_OK) {
    infof(معلومات, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(معلومات, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_سېټ(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  إذا(rc != SSH_OK) {
    failf(معلومات, "Could not set socket");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(Curl_creds_has_user(conn->creds)) {
    infof(معلومات, "User: %s", conn->creds->user);
    rc = ssh_options_سېټ(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    إذا(rc != SSH_OK) {
      failf(معلومات, "Could not set user");
      أرجع CURLE_FAILED_INIT;
    }
  }

  إذا(CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS)) {
    infof(معلومات, "Known hosts: %s",
          CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_سېټ(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS));
    إذا(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_سېټ(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS));
    إذا(rc != SSH_OK) {
      failf(معلومات, "Could not set known hosts file path");
      أرجع CURLE_FAILED_INIT;
    }
  }

  إذا(conn->origin->پورټ) {
    rc = ssh_options_سېټ(sshc->ssh_session, SSH_OPTIONS_پورټ,
                         &conn->origin->پورټ);
    إذا(rc != SSH_OK) {
      failf(معلومات, "Could not set remote port");
      أرجع CURLE_FAILED_INIT;
    }
  }

  إذا(معلومات->سېټ.ssh_compression) {
    rc = ssh_options_سېټ(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    إذا(rc != SSH_OK) {
      failf(معلومات, "Could not set compression");
      أرجع CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = عدم;
  sshc->pubkey = عدم;

  إذا(sshc->pub_کلی) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_کلی, &sshc->pubkey);
    إذا(rc != SSH_OK) {
      failf(معلومات, "Could not load public key file");
      أرجع CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_منزل(معلومات, sshc, SSH_INIT);

  پایله = myssh_multi_statemach(معلومات, بشپړ);

  أرجع پایله;
}

/* called from multi.c while DOing */
ساكن CURLcode scp_doing(بنية Curl_easy *معلومات, bool *dophase_بشپړ)
{
  CURLcode پایله;

  پایله = myssh_multi_statemach(معلومات, dophase_بشپړ);

  إذا(*dophase_بشپړ) {
    CURL_TRC_SSH(معلومات, "DO phase is complete");
  }
  أرجع پایله;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ساكن CURLcode scp_perform(بنية Curl_easy *معلومات,
                            bool *connected, bool *dophase_بشپړ)
{
  CURLcode پایله = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(معلومات->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(معلومات, "DO phase starts");

  *dophase_بشپړ = FALSE;        /* not done yet */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_منزل(معلومات, sshc, SSH_SCP_TRANS_INIT);

  پایله = myssh_multi_statemach(معلومات, dophase_بشپړ);

  *connected = Curl_conn_is_connected(معلومات->conn, FIRSTSOCKET);

  إذا(*dophase_بشپړ) {
    CURL_TRC_SSH(معلومات, "DO phase is complete");
  }

  أرجع پایله;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode scp_disconnect(بنية Curl_easy *معلومات,
                               بنية connectdata *conn,
                               bool dead_connection)
{
  CURLcode پایله = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(معلومات, CURL_META_SSH_EASY);
  (فراغ)dead_connection;

  إذا(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_منزل(معلومات, sshc, SSH_SESSION_DISCONNECT);
    پایله = myssh_بلاک_statemach(معلومات, sshc, sshp, TRUE);
  }

  أرجع پایله;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ساكن CURLcode myssh_بشپړ(بنية Curl_easy *معلومات,
                           بنية ssh_conn *sshc,
                           CURLcode حالت)
{
  CURLcode پایله = CURLE_OK;
  بنية SSHPROTO *sshp = Curl_meta_get(معلومات, CURL_META_SSH_EASY);

  إذا(!حالت && sshp) {
    /* run the state-machine */
    پایله = myssh_بلاک_statemach(معلومات, sshc, sshp, FALSE);
  }
  وإلا
    پایله = حالت;

  إذا(Curl_pgrs_بشپړ(معلومات, Curl_pgrs_now(معلومات)))
    أرجع CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(معلومات);
  أرجع پایله;
}

ساكن CURLcode scp_بشپړ(بنية Curl_easy *معلومات, CURLcode حالت,
                         bool premature)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(معلومات->conn, CURL_META_SSH_CONN);
  (فراغ)premature;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;
  إذا(!حالت)
    myssh_منزل(معلومات, sshc, SSH_SCP_بشپړ);

  أرجع myssh_بشپړ(معلومات, sshc, حالت);
}

ساكن CURLcode scp_send(بنية Curl_easy *معلومات, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  صحيح rc;
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  (فراغ)eos;
  *pnwritten = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#إذا 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  إذا(rc == SSH_AGAIN)
    أرجع CURLE_AGAIN;
  وإلا
#د_که_پای
  إذا(rc != SSH_OK)
    أرجع CURLE_SSH;

  *pnwritten = len;
  أرجع CURLE_OK;
}

ساكن CURLcode scp_recv(بنية Curl_easy *معلومات, int8_t sockindex,
                         حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  صحيح nread;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  إذا(nread == SSH_ERROR)
    أرجع CURLE_SSH;
#إذا 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  إذا(nread == SSH_AGAIN)
    أرجع CURLE_AGAIN;
#د_که_پای
  *pnread = (size_t)nread;
  أرجع CURLE_OK;
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

ساكن CURLcode sftp_perform(بنية Curl_easy *معلومات,
                             bool *connected,
                             bool *dophase_بشپړ)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(معلومات->conn, CURL_META_SSH_CONN);
  CURLcode پایله = CURLE_OK;

  CURL_TRC_SSH(معلومات, "DO phase starts");

  *dophase_بشپړ = FALSE; /* not done yet */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  پایله = myssh_multi_statemach(معلومات, dophase_بشپړ);

  *connected = Curl_conn_is_connected(معلومات->conn, FIRSTSOCKET);

  إذا(*dophase_بشپړ) {
    CURL_TRC_SSH(معلومات, "DO phase is complete");
  }

  أرجع پایله;
}

/* called from multi.c while DOing */
ساكن CURLcode sftp_doing(بنية Curl_easy *معلومات,
                           bool *dophase_بشپړ)
{
  CURLcode پایله = myssh_multi_statemach(معلومات, dophase_بشپړ);
  إذا(*dophase_بشپړ) {
    CURL_TRC_SSH(معلومات, "DO phase is complete");
  }
  أرجع پایله;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode sftp_disconnect(بنية Curl_easy *معلومات,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(معلومات, CURL_META_SSH_EASY);
  CURLcode پایله = CURLE_OK;
  (فراغ)dead_connection;

  CURL_TRC_SSH(معلومات, "DISCONNECT starts now");

  إذا(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_منزل(معلومات, sshc, SSH_SFTP_SHUTDOWN);
    پایله = myssh_بلاک_statemach(معلومات, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(معلومات, "DISCONNECT is done");
  أرجع پایله;
}

ساكن CURLcode sftp_بشپړ(بنية Curl_easy *معلومات, CURLcode حالت,
                          bool premature)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;
  إذا(!حالت) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    إذا(!premature && معلومات->سېټ.postquote && !conn->بټونه.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
  }
  أرجع myssh_بشپړ(معلومات, sshc, حالت);
}

/* return number of sent bytes */
ساكن CURLcode sftp_send(بنية Curl_easy *معلومات, int8_t sockindex,
                          ثابت uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (فراغ)sockindex;
  (فراغ)eos;
  *pnwritten = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

#إذا LIBSSH_نسخه_INT > SSH_نسخه_INT(0, 11, 0)
  بدل(sshc->sftp_send_state) {
  حالة 0:
    sftp_file_سېټ_nonblocking(sshc->sftp_file);
    إذا(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      أرجع CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  حالة 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    إذا(nwrite == SSH_AGAIN)
      أرجع CURLE_AGAIN;
    وإلا إذا(nwrite < 0)
      أرجع CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    أرجع CURLE_OK;
  افتراضي:
    /* we never reach here */
    أرجع CURLE_SEND_ERROR;
  }
#وإلا
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  إذا(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#إذا 0 /* not returned by libssh on write */
  إذا(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  وإلا
#د_که_پای
  إذا(nwrite < 0)
    أرجع CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  أرجع CURLE_OK;
#د_که_پای
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ساكن CURLcode sftp_recv(بنية Curl_easy *معلومات, int8_t sockindex,
                          حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (فراغ)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_اندازه);
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  بدل(sshc->sftp_recv_state) {
  حالة 0:
#إذا LIBSSH_نسخه_INT > SSH_نسخه_INT(0, 11, 0)
    إذا(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      أرجع CURLE_RECV_ERROR;
    }
#وإلا
    sshc->sftp_file_شاخص =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    إذا(sshc->sftp_file_شاخص < 0)
      أرجع CURLE_RECV_ERROR;
#د_که_پای

    FALLTHROUGH();
  حالة 1:
    sshc->sftp_recv_state = 1;

#إذا LIBSSH_نسخه_INT > SSH_نسخه_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#وإلا
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_شاخص);
#د_که_پای

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    إذا(nread == SSH_AGAIN)
      أرجع CURLE_AGAIN;
    وإلا إذا(nread < 0)
      أرجع CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    أرجع CURLE_OK;

  افتراضي:
    /* we never reach here */
    أرجع CURLE_RECV_ERROR;
  }
}

ساكن CURLcode myssh_do_it(بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله;
  bool connected = FALSE;
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *بشپړ = FALSE;                /* default to false */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  معلومات->req.اندازه = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->دوهم_Create_Dirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(معلومات);

  إذا(conn->scheme->protocol & CURLPROTO_SCP)
    پایله = scp_perform(معلومات, &connected, بشپړ);
  وإلا
    پایله = sftp_perform(معلومات, &connected, بشپړ);

  أرجع پایله;
}

CURLcode Curl_ssh_init(فراغ)
{
  إذا(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    أرجع CURLE_FAILED_INIT;
  }
  أرجع CURLE_OK;
}

فراغ Curl_ssh_cleanup(فراغ)
{
  (فراغ)ssh_finalize();
}

فراغ Curl_ssh_نسخه(حرف *بفر, size_t buflen)
{
  (فراغ)curl_msnprintf(بفر, buflen, "libssh/%s", ssh_نسخه(0));
}

/*
 * SCP.
 */
ثابت بنية Curl_protocol Curl_protocol_scp = {
  myssh_تیاری_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_بشپړ,                             /* done */
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
  myssh_تیاری_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_بشپړ,                            /* done */
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

#د_که_پای /* USE_LIBSSH */
