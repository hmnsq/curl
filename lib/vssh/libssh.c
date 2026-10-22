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

#اگر_تعریف_شده USE_LIBSSH

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده __VMS
#ضمن <in.h>
#ضمن <inet.h>
#پایان_اگر

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

#اگر_تعریف_شده HAVE_UNISTD_H
#ضمن <unistd.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_FCNTL_H
#ضمن <fcntl.h>
#پایان_اگر

/* A recent macro provided by libssh. Or make our own. */
#اگر_تعریف_نشده SSH_STRING_FREE_CHAR
#تعریف_کن SSH_STRING_FREE_CHAR(x) \
  افعل {                          \
    إذا(x) {                     \
      ssh_string_free_char(x);  \
      x = عدم;                 \
    }                           \
  } طالما(0)
#پایان_اگر

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#اگر_تعریف_نشده SSH_S_IFMT
#تعریف_کن SSH_S_IFMT   00170000
#پایان_اگر
#اگر_تعریف_نشده SSH_S_IFLNK
#تعریف_کن SSH_S_IFLNK  0120000
#پایان_اگر

ساكن CURLcode sftp_error_مقصد_CURLE(صحيح err)
{
  بدل(err) {
  حالة SSH_FX_OK:
    أرجع CURLE_OK;

  حالة SSH_FX_NO_SUCH_FILE:
  حالة SSH_FX_NO_SUCH_مسیر:
    أرجع CURLE_REMOTE_FILE_NOT_یافت;

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
ساكن صحيح myssh_is_معلوم(بنية Curl_easy *داده, بنية ssh_conn *sshc)
{
  صحيح rc;
  ssh_کلید pubkey;
  بیعلامت حرف *hash_sha256 = عدم;
  size_t hlen_sha256;
  بیعلامت حرف *hash_md5 = عدم;
  size_t hlen_md5;
  حرف *یافت_base64 = عدم;
  حرف *معلوم_base64 = عدم;
  صحيح vstate;
  تعداد curl_khmatch keymatch;
  بنية curl_khkey foundkey;
  بنية curl_khkey *knownkeyp = عدم;
  curl_sshkeycallback func = داده->مجموعه.ssh_keyfunc;
  بنية ssh_knownhosts_مدخل *knownhostsentry = عدم;
  بنية curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  إذا(rc != SSH_OK)
    أرجع rc;

  إذا(CURL_EASY_STR(داده, STRING_SSH_HOST_عمومی_کلید_SHA256)) {
    ثابت حرف *pubkey_sha256 =
      CURL_EASY_STR(داده, STRING_SSH_HOST_عمومی_کلید_SHA256);
    حرف *fingerprint_b64 = عدم;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    إذا(rc != SSH_OK || hlen_sha256 != 32) {
      failf(داده, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      اذهب cleanup;
    }

    إذا(curlx_base64_encode((ثابت uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      اذهب cleanup;
    }

    infof(داده, "SSH SHA256 fingerprint: %s", fingerprint_b64);

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
       مقایسه_رشته(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(داده,
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

  إذا(CURL_EASY_STR(داده, STRING_SSH_HOST_عمومی_کلید_MD5)) {
    ثابت حرف *pubkey_md5 =
      CURL_EASY_STR(داده, STRING_SSH_HOST_عمومی_کلید_MD5);
    حرف md5buffer[33];
    صحيح i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    إذا(rc != SSH_OK || hlen_md5 != 16) {
      failf(داده,
            "Denied establishing ssh session: MD5 fingerprint not available");
      اذهب cleanup;
    }

    لكل(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(داده, "SSH MD5 fingerprint: %s", md5buffer);

    إذا(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(داده,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      اذهب cleanup;
    }

    rc = SSH_OK;
    اذهب cleanup;
  }

  إذا(CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_معلوم_hosts_مدخل(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    إذا(knownhostsentry) {
      إذا(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &معلوم_base64);
        إذا(rc != SSH_OK) {
          اذهب cleanup;
        }
        knownkey.کلید = معلوم_base64;
        knownkey.len = طول_رشته(معلوم_base64);

        بدل(ssh_کلید_type(knownhostsentry->publickey)) {
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
    حالة SSH_معلوم_HOSTS_NOT_یافت:
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

      rc = ssh_pki_export_pubkey_base64(pubkey, &یافت_base64);
      إذا(rc != SSH_OK)
        اذهب cleanup;

      foundkey.کلید = یافت_base64;
      foundkey.len = طول_رشته(یافت_base64);

      بدل(ssh_کلید_type(pubkey)) {
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

      CURL_CBAPI_شروع(&guard, داده, easy_ssh_keyfunc);
      rc = func(داده, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, داده->مجموعه.ssh_keyfunc_userp);
      CURL_CBAPI_انتها(&guard);

      بدل(rc) {
      حالة CURLKHSTAT_FINE_جمع_کن_مقصد_FILE:
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
  إذا(یافت_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    حرر(یافت_base64); /* allocated by libssh, deallocate with system free */
  }
  إذا(معلوم_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    حرر(معلوم_base64); /* allocated by libssh, deallocate with system free */
  }
  إذا(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  إذا(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_کلید_free(pubkey);
  إذا(knownhostsentry) {
    ssh_knownhosts_مدخل_free(knownhostsentry);
  }
  أرجع rc;
}

ساكن صحيح myssh_مقصد_ERROR(بنية Curl_easy *داده,
                          بنية ssh_conn *sshc,
                          CURLcode نتیجه)
{
  myssh_مقصد(داده, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = نتیجه;
  أرجع SSH_ERROR;
}

ساكن صحيح myssh_مقصد_SFTP_CLOSE(بنية Curl_easy *داده,
                               بنية ssh_conn *sshc)
{
  myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_مقصد_CURLE(sftp_get_error(sshc->sftp_session));
  أرجع SSH_ERROR;
}

ساكن صحيح myssh_مقصد_PASSWD_AUTH(بنية Curl_easy *داده,
                                بنية ssh_conn *sshc)
{
  إذا(sshc->auth_methods & SSH_AUTH_روش_PASSWORD) {
    myssh_مقصد(داده, sshc, SSH_AUTH_PASS_INIT);
    أرجع SSH_OK;
  }
  أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_LOGIN_DENIED);
}

ساكن صحيح myssh_مقصد_کلید_AUTH(بنية Curl_easy *داده,
                             بنية ssh_conn *sshc)
{
  إذا(sshc->auth_methods & SSH_AUTH_روش_INTERACTIVE) {
    myssh_مقصد(داده, sshc, SSH_AUTH_کلید_INIT);
    أرجع SSH_OK;
  }
  أرجع myssh_مقصد_PASSWD_AUTH(داده, sshc);
}

ساكن صحيح myssh_مقصد_GSSAPI_AUTH(بنية Curl_easy *داده,
                                بنية ssh_conn *sshc)
{
  إذا(sshc->auth_methods & SSH_AUTH_روش_GSSAPI_MIC) {
    myssh_مقصد(داده, sshc, SSH_AUTH_GSSAPI);
    أرجع SSH_OK;
  }
  أرجع myssh_مقصد_کلید_AUTH(داده, sshc);
}

ساكن صحيح myssh_درون_SFTP_READDIR_INIT(بنية Curl_easy *داده,
                                      بنية ssh_conn *sshc,
                                      بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);
  Curl_pgrs_مجموعه_Download_اندازه(داده, -1);
  إذا(داده->req.no_body) {
    myssh_مقصد(داده, sshc, SSH_STOP);
    أرجع SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->مسیر);
  إذا(!sshc->sftp_dir) {
    failf(داده, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_مقصد_SFTP_CLOSE(داده, sshc);
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_READDIR);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_READDIR(بنية Curl_easy *داده,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp)
{
  CURLcode نتیجه = CURLE_OK;

  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  إذا(sshc->readdir_attrs)
    sftp_صفات_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  إذا(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = طول_رشته(sshc->readdir_filename);

    إذا(داده->مجموعه.list_only) {
      حرف *tmp_سطر;

      tmp_سطر = curl_maprintf("%s\n", sshc->readdir_filename);
      إذا(!tmp_سطر) {
        myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_بیرون_از_حافظه;
        أرجع SSH_ERROR;
      }
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                                 tmp_سطر, sshc->readdir_len + 1);
      curlx_free(tmp_سطر);

      إذا(نتیجه) {
        myssh_مقصد(داده, sshc, SSH_STOP);
        sshc->actualcode = نتیجه;
        أرجع SSH_NO_ERROR;
      }
    }
    وإلا {
      إذا(curlx_dyn_جمع_کن(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_بیرون_از_حافظه;
        myssh_مقصد(داده, sshc, SSH_STOP);
        أرجع SSH_ERROR;
      }

      إذا((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_link_مسیر = curl_maprintf("%s%s", sshp->مسیر,
                                               sshc->readdir_filename);

        إذا(!sshc->readdir_link_مسیر) {
          myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_بیرون_از_حافظه;
          أرجع SSH_ERROR;
        }

        myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_LINK);
        أرجع SSH_NO_ERROR;
      }
      myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_BOTTOM);
      أرجع SSH_NO_ERROR;
    }
  }
  وإلا إذا(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_انجامشده);
  }
  وإلا {
    failf(داده, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_مقصد_SFTP_CLOSE(داده, sshc);
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_READDIR_LINK(بنية Curl_easy *داده,
                                      بنية ssh_conn *sshc)
{
  إذا(sshc->readdir_link_attrs)
    sftp_صفات_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_link_مسیر);
  إذا(!sshc->readdir_link_attrs) {
    failf(داده, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_مقصد_SFTP_CLOSE(داده, sshc);
  }

  إذا(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_link_مسیر);
    إذا(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    وإلا
      sshc->readdir_len = طول_رشته(sshc->readdir_tmp);
    sshc->readdir_longentry = عدم;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  وإلا {
    sshc->readdir_len = طول_رشته(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_link_مسیر);

  إذا(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_بیرون_از_حافظه;
    أرجع SSH_ERROR;
  }

  sftp_صفات_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = عدم;
  sshc->readdir_filename = عدم;
  sshc->readdir_longentry = عدم;

  myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_BOTTOM);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_READDIR_BOTTOM(بنية Curl_easy *داده,
                                        بنية ssh_conn *sshc)
{
  CURLcode نتیجه;

  إذا(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    نتیجه = CURLE_بیرون_از_حافظه;
  وإلا
    نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = عدم;

  إذا(نتیجه) {
    myssh_مقصد(داده, sshc, SSH_STOP);
    sshc->actualcode = نتیجه;
  }
  وإلا
    myssh_مقصد(داده, sshc, SSH_SFTP_READDIR);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_READDIR_انجامشده(بنية Curl_easy *داده,
                                      بنية ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = عدم;

  /* no data to transfer */
  Curl_xfer_راهاندازی_nop(داده);
  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع SSH_NO_ERROR;
}

ساكن فراغ myssh_quote_error(بنية Curl_easy *داده, بنية ssh_conn *sshc,
                              ثابت حرف *cmd)
{
  إذا(cmd)
    failf(داده, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

ساكن صحيح myssh_درون_SFTP_QUOTE_STATVFS(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  إذا(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(داده, sshc, "statvfs");
    أرجع SSH_OK;
  }
  وإلا إذا(statvfs) {
#اگر_تعریف_شده _MSC_VER
#تعریف_کن CURL_LIBSSH_VFS_اندازه_ماسک "I64u"
#وإلاإذا defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#تعریف_کن CURL_LIBSSH_VFS_اندازه_ماسک "llu"
#وإلا
#تعریف_کن CURL_LIBSSH_VFS_اندازه_ماسک PRIu64
#پایان_اگر
    CURLcode نتیجه = CURLE_OK;
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
                              statvfs->f_بلوکها, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    إذا(!tmp)
      نتیجه = CURLE_بیرون_از_حافظه;

    إذا(!نتیجه) {
      نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, tmp, طول_رشته(tmp));
      curlx_free(tmp);
    }
    إذا(نتیجه) {
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجه;
    }
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
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

ساكن فراغ myssh_state_init(بنية Curl_easy *داده,
                             بنية ssh_conn *sshc)
{
  sshc->دومی_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#إذا 0
  ssh_مجموعه_log_level(SSH_LOG_PROTOCOL);
#پایان_اگر

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_مجموعه_blocking(sshc->ssh_session, 0);

  myssh_مقصد(داده, sshc, SSH_S_STARTUP);
}

ساكن فراغ myssh_block2waitfor(بنية connectdata *conn,
                                بنية ssh_conn *sshc,
                                bool بلوک)
{
  (فراغ)conn;
  إذا(بلوک) {
    صحيح dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  وإلا
    sshc->waitfor = 0;
}

ساكن صحيح myssh_درون_S_STARTUP(بنية Curl_easy *داده,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = داده->conn;
  صحيح rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  إذا(rc == SSH_AGAIN) {
    CURL_TRC_SSH(داده, "connect -> EAGAIN");
  }
  وإلا إذا(rc != SSH_OK) {
    failf(داده, "Failure establishing ssh session");
    rc = myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);
  }
  وإلا
    myssh_مقصد(داده, sshc, SSH_HOSTKEY);

  أرجع rc;
}

ساكن صحيح myssh_درون_AUTHLIST(بنية Curl_easy *داده,
                             بنية ssh_conn *sshc)
{
  صحيح rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, عدم);
  إذا(rc == SSH_AUTH_AGAIN)
    أرجع SSH_AGAIN;

  إذا(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(داده, "Authenticated with none");
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
    أرجع rc;
  }
  وإلا إذا(rc == SSH_AUTH_ERROR) {
    rc = myssh_مقصد_ERROR(داده, sshc, CURLE_LOGIN_DENIED);
    أرجع rc;
  }

  sshc->auth_methods =
    (بیعلامت صحيح)ssh_userauth_list(sshc->ssh_session, عدم);
  إذا(sshc->auth_methods)
    infof(داده, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_روش_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_روش_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_روش_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_روش_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  إذا((sshc->auth_methods & SSH_AUTH_روش_PUBLICKEY) &&
     ((sshc->priv_کلید && sshc->priv_کلید[0]) ||
      (داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_مقصد(داده, sshc, SSH_AUTH_PKEY_INIT);
    infof(داده, "Authentication using SSH public key file");
  }
  وإلا إذا(sshc->auth_methods & SSH_AUTH_روش_GSSAPI_MIC) {
    myssh_مقصد(داده, sshc, SSH_AUTH_GSSAPI);
  }
  وإلا إذا(sshc->auth_methods & SSH_AUTH_روش_INTERACTIVE) {
    myssh_مقصد(داده, sshc, SSH_AUTH_کلید_INIT);
  }
  وإلا إذا(sshc->auth_methods & SSH_AUTH_روش_PASSWORD) {
    myssh_مقصد(داده, sshc, SSH_AUTH_PASS_INIT);
  }
  وإلا {                  /* unsupported authentication method */
    rc = myssh_مقصد_ERROR(داده, sshc, CURLE_LOGIN_DENIED);
  }
  أرجع rc;
}

ساكن صحيح myssh_درون_AUTH_PKEY_INIT(بنية Curl_easy *داده,
                                   بنية ssh_conn *sshc)
{
  صحيح rc;
  إذا(!(داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_مقصد_GSSAPI_AUTH(داده, sshc);
    أرجع rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  إذا(sshc->priv_کلید && sshc->priv_کلید[0]) {
    إذا(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, عدم, sshc->pubkey);
      إذا(rc == SSH_AUTH_AGAIN)
        أرجع SSH_AGAIN;

      إذا(rc != SSH_OK) {
        rc = myssh_مقصد_GSSAPI_AUTH(داده, sshc);
        أرجع rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_کلید,
                                     sshc->passphrase, عدم,
                                     عدم, &sshc->privkey);
    إذا(rc != SSH_OK) {
      failf(داده, "Could not load private key file %s", sshc->priv_کلید);
      rc = myssh_مقصد_ERROR(داده, sshc, CURLE_LOGIN_DENIED);
      أرجع rc;
    }

    myssh_مقصد(داده, sshc, SSH_AUTH_PKEY);
  }
  وإلا {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, عدم,
                                     sshc->passphrase);
    إذا(rc == SSH_AUTH_AGAIN)
      أرجع SSH_AGAIN;

    إذا(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(داده, "Completed public key authentication");
      myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
      أرجع rc;
    }

    rc = myssh_مقصد_GSSAPI_AUTH(داده, sshc);
  }
  أرجع rc;
}

ساكن صحيح myssh_درون_AUTH_PKEY(بنية Curl_easy *داده,
                              بنية ssh_conn *sshc)
{
  صحيح rc = ssh_userauth_publickey(sshc->ssh_session, عدم, sshc->privkey);
  إذا(rc == SSH_AUTH_AGAIN)
    أرجع SSH_AGAIN;
  وإلا إذا(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(داده, "Completed public key authentication");
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
    أرجع SSH_OK;
  }
  وإلا {
    infof(داده, "Failed public key authentication (rc: %d)", rc);
    أرجع myssh_مقصد_GSSAPI_AUTH(داده, sshc);
  }
}

ساكن صحيح myssh_درون_AUTH_GSSAPI(بنية Curl_easy *داده,
                                بنية ssh_conn *sshc)
{
  صحيح rc;
  إذا(!(داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    أرجع myssh_مقصد_کلید_AUTH(داده, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  إذا(rc == SSH_AUTH_AGAIN)
    أرجع SSH_AGAIN;

  إذا(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(داده, "Completed gssapi authentication");
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
    أرجع SSH_OK;
  }

  أرجع myssh_مقصد_کلید_AUTH(داده, sshc);
}

ساكن صحيح myssh_درون_AUTH_کلید_INIT(بنية Curl_easy *داده,
                                  بنية ssh_conn *sshc)
{
  إذا(داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_مقصد(داده, sshc, SSH_AUTH_کلید);
    أرجع SSH_NO_ERROR;
  }
  أرجع myssh_مقصد_PASSWD_AUTH(داده, sshc);
}

ساكن صحيح myssh_درون_AUTH_کلید(بنية Curl_easy *داده,
                             بنية ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  صحيح rc = myssh_auth_interactive(داده->conn, sshc);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  وإلا إذا(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(داده, "completed keyboard interactive authentication");
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
    أرجع SSH_NO_ERROR;
  }
  وإلا
    أرجع myssh_مقصد_PASSWD_AUTH(داده, sshc);
}

ساكن صحيح myssh_درون_AUTH_PASS_INIT(بنية Curl_easy *داده,
                                   بنية ssh_conn *sshc)
{
  إذا(!(داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_LOGIN_DENIED);
  myssh_مقصد(داده, sshc, SSH_AUTH_PASS);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_AUTH_PASS(بنية Curl_easy *داده,
                              بنية ssh_conn *sshc)
{
  صحيح rc = ssh_userauth_password(sshc->ssh_session, عدم,
                                 Curl_creds_passwd(داده->conn->creds));
  إذا(rc == SSH_AUTH_AGAIN)
    أرجع SSH_AGAIN;
  وإلا إذا(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(داده, "Completed password authentication");
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
    أرجع SSH_NO_ERROR;
  }
  أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_LOGIN_DENIED);
}

ساكن صحيح myssh_درون_AUTH_انجامشده(بنية Curl_easy *داده,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = داده->conn;
  إذا(!sshc->authed) {
    failf(داده, "Authentication failure");
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(داده, "Authentication complete");
  Curl_pgrs_زمان(داده, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  إذا(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_مقصد(داده, sshc, SSH_SFTP_INIT);
    أرجع SSH_NO_ERROR;
  }
  infof(داده, "SSH CONNECT phase done");
  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_UPLOAD_INIT(بنية Curl_easy *داده,
                                بنية ssh_conn *sshc,
                                بنية SSHPROTO *sshp)
{
  صحيح flags;
  صحيح rc = 0;

  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);

  إذا(داده->state.resume_from) {
    sftp_صفات attrs;

    إذا(داده->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->مسیر);
      إذا(attrs) {
        curl_off_t اندازه = attrs->اندازه;
        sftp_صفات_free(attrs);
        إذا(اندازه < 0) {
          failf(داده, "Bad file size (%" FMT_OFF_T ")", اندازه);
          rc = myssh_مقصد_ERROR(داده, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          أرجع rc;
        }
        داده->state.resume_from = اندازه;
      }
      وإلا {
        داده->state.resume_from = 0;
      }
    }
  }

  إذا(داده->مجموعه.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  وإلا إذا(داده->state.resume_from > 0) {
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
    sftp_open(sshc->sftp_session, sshp->مسیر,
              flags, (mode_t)داده->مجموعه.new_file_perms);
  إذا(!sshc->sftp_file) {
    صحيح err = sftp_get_error(sshc->sftp_session);

    إذا((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_مسیر) &&
       داده->مجموعه.ftp_create_missing_dirs &&
       (طول_رشته(sshp->مسیر) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->دومی_Create_Dirs = 1;
      myssh_مقصد(داده, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      أرجع rc;
    }
    وإلا {
      rc = myssh_مقصد_SFTP_CLOSE(داده, sshc);
      أرجع rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  إذا(داده->state.resume_from > 0 && !داده->مجموعه.remote_append) {
    صحيح seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    إذا(داده->مجموعه.seek_func) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, داده, easy_seek_func);
      seekerr = داده->مجموعه.seek_func(داده->مجموعه.seek_client,
                                    داده->state.resume_from, SEEK_SET);
      CURL_CBAPI_انتها(&guard);
    }

    إذا(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(داده, "Could not seek stream");
        rc = myssh_مقصد_ERROR(داده, sshc, CURLE_FTP_COULDNT_USE_REST);
        أرجع rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      افعل {
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (داده->state.resume_from - passed >
           (curl_off_t)حجم(scratch)) ?
          حجم(scratch) : curlx_sotouz(داده->state.resume_from - passed);

        size_t actuallyread =
          داده->state.fread_func(scratch, 1,
                                 readthisamountnow, داده->state.درون);

        passed += actuallyread;
        إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(داده, "Failed to read data");
          rc = myssh_مقصد_ERROR(داده, sshc, CURLE_FTP_COULDNT_USE_REST);
          أرجع rc;
        }
      } طالما(passed < داده->state.resume_from);
    }

    /* now, decrease the size of the read */
    إذا(داده->state.infilesize > 0) {
      إذا(داده->state.resume_from > داده->state.infilesize) {
        failf(داده, "Resume point beyond size");
        أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_BAD_FUNCTION_آرگومان);
      }
      داده->state.infilesize -= داده->state.resume_from;
      داده->req.اندازه = داده->state.infilesize;
      Curl_pgrs_مجموعه_Upload_اندازه(داده, داده->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, داده->state.resume_from);
    إذا(rc) {
      rc = myssh_مقصد_SFTP_CLOSE(داده, sshc);
      أرجع rc;
    }
  }
  إذا(داده->state.infilesize > 0) {
    داده->req.اندازه = داده->state.infilesize;
    Curl_pgrs_مجموعه_Upload_اندازه(داده, داده->state.infilesize);
  }
  /* upload data */
  Curl_xfer_راهاندازی_send(داده, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  داده->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(داده);
#إذا LIBSSH_نسخه_INT > SSH_نسخه_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#پایان_اگر
  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع rc;
}

ساكن صحيح myssh_درون_SFTP_DOWNLOAD_INIT(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc,
                                       بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  إذا(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->مسیر,
                              O_RDONLY, (mode_t)داده->مجموعه.new_file_perms);
  إذا(!sshc->sftp_file) {
    failf(داده, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    أرجع myssh_مقصد_SFTP_CLOSE(داده, sshc);
  }
  sftp_file_مجموعه_nonblocking(sshc->sftp_file);
  myssh_مقصد(داده, sshc, SSH_SFTP_DOWNLOAD_STAT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_DOWNLOAD_STAT(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc)
{
  curl_off_t اندازه;
  صحيح rc = 0;
  sftp_صفات attrs = sftp_fstat(sshc->sftp_file);
  إذا(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_اندازه) ||
     (attrs->اندازه == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    داده->req.اندازه = -1;
    داده->req.maxdownload = -1;
    Curl_pgrs_مجموعه_Download_اندازه(داده, -1);
    اندازه = 0;
    إذا(attrs)
      sftp_صفات_free(attrs);
  }
  وإلا {
    اندازه = attrs->اندازه;

    sftp_صفات_free(attrs);

    إذا(اندازه < 0) {
      failf(داده, "Bad file size (%" FMT_OFF_T ")", اندازه);
      أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    إذا(داده->state.use_range) {
      curl_off_t from;
      CURLcode نتیجه = Curl_ssh_range(داده, داده->state.range, اندازه,
                                       &from, &اندازه);
      إذا(نتیجه)
        أرجع myssh_مقصد_ERROR(داده, sshc, نتیجه);

      rc = sftp_seek64(sshc->sftp_file, from);
      إذا(rc)
        أرجع myssh_مقصد_SFTP_CLOSE(داده, sshc);
    }
    داده->req.اندازه = اندازه;
    داده->req.maxdownload = اندازه;
    Curl_pgrs_مجموعه_Download_اندازه(داده, اندازه);
  }

  /* We can resume if we can seek to the resume position */
  إذا(داده->state.resume_from) {
    إذا(داده->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      إذا(اندازه < -داده->state.resume_from) {
        failf(داده, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", داده->state.resume_from, اندازه);
        أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      داده->state.resume_from += اندازه;
    }
    وإلا {
      إذا(اندازه < داده->state.resume_from) {
        failf(داده, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              داده->state.resume_from, اندازه);
        أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    داده->req.اندازه = اندازه - داده->state.resume_from;
    داده->req.maxdownload = اندازه - داده->state.resume_from;
    Curl_pgrs_مجموعه_Download_اندازه(داده, اندازه - داده->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, داده->state.resume_from);
    إذا(rc)
      أرجع myssh_مقصد_SFTP_CLOSE(داده, sshc);
  }

  /* Setup the actual download */
  إذا(داده->req.اندازه == 0) {
    /* no data to transfer */
    Curl_xfer_راهاندازی_nop(داده);
    infof(داده, "File already completely downloaded");
    myssh_مقصد(داده, sshc, SSH_STOP);
    أرجع rc;
  }
  Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, داده->req.اندازه);

  /* not set by Curl_xfer_setup to preserve io_flags */
  داده->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_مقصد(داده, sshc, SSH_STOP);

  أرجع rc;
}

ساكن صحيح myssh_درون_SFTP_CLOSE(بنية Curl_easy *داده,
                               بنية ssh_conn *sshc,
                               بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);
  إذا(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = عدم;
  }
  curlx_safefree(sshp->مسیر);

  CURL_TRC_SSH(داده, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  إذا(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_مقصد(داده, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_STOP);
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_SHUTDOWN(بنية Curl_easy *داده,
                                  بنية ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_مجموعه_blocking(sshc->ssh_session, 0);
#إذا LIBSSH_نسخه_INT > SSH_نسخه_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#پایان_اگر

  إذا(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = عدم;
  }

  إذا(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = عدم;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_مقصد(داده, sshc, SSH_SESSION_DISCONNECT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_INIT(بنية Curl_easy *داده,
                              بنية ssh_conn *sshc)
{
  صحيح rc;
  ssh_مجموعه_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  إذا(!sshc->sftp_session) {
    failf(داده, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  إذا(rc != SSH_OK) {
    failf(داده, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    أرجع myssh_مقصد_ERROR(داده, sshc, sftp_error_مقصد_CURLE(SSH_FX_FAILURE));
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_REALPATH);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_REALPATH(بنية Curl_easy *داده,
                                  بنية ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_مسیر(sshc->sftp_session, ".");
  إذا(!sshc->homedir)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(داده->state.most_recent_ftp_entrypath);
  داده->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  إذا(!داده->state.most_recent_ftp_entrypath)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_بیرون_از_حافظه);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(داده, "CONNECT phase done");
  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_QUOTE_INIT(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp)
{
  CURLcode نتیجه;

  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);

  نتیجه = Curl_getworkingpath(داده, sshc->homedir, &sshp->مسیر);
  إذا(نتیجه) {
    sshc->actualcode = نتیجه;
    myssh_مقصد(داده, sshc, SSH_STOP);
  }
  وإلا إذا(داده->مجموعه.quote) {
    infof(داده, "Sending quote commands");
    sshc->quote_قلم = داده->مجموعه.quote;
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE);
  }
  وإلا
    myssh_مقصد(داده, sshc, SSH_SFTP_GETINFO);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_POSTQUOTE_INIT(بنية Curl_easy *داده,
                                        بنية ssh_conn *sshc)
{
  إذا(داده->مجموعه.postquote) {
    infof(داده, "Sending quote commands");
    sshc->quote_قلم = داده->مجموعه.postquote;
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_STOP);
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح quote_error(بنية Curl_easy *داده,
                       بنية ssh_conn *sshc)
{
  failf(داده, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_QUOTE(بنية Curl_easy *داده,
                               بنية ssh_conn *sshc,
                               بنية SSHPROTO *sshp)
{
  ثابت حرف *cp;
  CURLcode نتیجه;

  /*
   * Support some of the "FTP" commands
   */
  ثابت حرف *cmd = sshc->quote_قلم->داده;
  sshc->acceptfail = FALSE;

  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);

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
                              sshp->مسیر);
    إذا(!tmp) {
      sshc->actualcode = CURLE_بیرون_از_حافظه;
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع SSH_NO_ERROR;
    }
    Curl_debug(داده, CURLINFO_سرآیند_بیرون, "PWD\n", 4);
    Curl_debug(داده, CURLINFO_سرآیند_درون, tmp, طول_رشته(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, tmp, طول_رشته(tmp));
    curlx_free(tmp);
    إذا(نتیجه) {
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجه;
    }
    وإلا
      myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
    أرجع SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  إذا(!cp) {
    failf(داده, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    أرجع SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  نتیجه = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_بیرون_از_حافظه)
      failf(داده, "Out of memory");
    وإلا
      failf(داده, "Syntax error: Bad first parameter");
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = نتیجه;
    أرجع SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  إذا(!مقایسه_رشته(cmd, "chgrp ", 6) ||
     !مقایسه_رشته(cmd, "chmod ", 6) ||
     !مقایسه_رشته(cmd, "chown ", 6) ||
     !مقایسه_رشته(cmd, "atime ", 6) ||
     !مقایسه_رشته(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    نتیجه = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_بیرون_از_حافظه)
        failf(داده, "Out of memory");
      وإلا
        failf(داده, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجه;
      أرجع SSH_NO_ERROR;
    }
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    sshc->quote_attrs = عدم;
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_STAT);
    أرجع SSH_NO_ERROR;
  }
  إذا(!مقایسه_رشته(cmd, "ln ", 3) ||
     !مقایسه_رشته(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    نتیجه = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_بیرون_از_حافظه)
        failf(داده, "Out of memory");
      وإلا
        failf(داده, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجه;
      أرجع SSH_NO_ERROR;
    }
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_SYMLINK);
    أرجع SSH_NO_ERROR;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "mkdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    /* create directory */
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_MKDIR);
    أرجع SSH_NO_ERROR;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    نتیجه = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(نتیجه) {
      إذا(نتیجه == CURLE_بیرون_از_حافظه)
        failf(داده, "Out of memory");
      وإلا
        failf(داده, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = نتیجه;
      أرجع SSH_NO_ERROR;
    }
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_RENAME);
    أرجع SSH_NO_ERROR;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "rmdir ", 6)) {
    /* delete directory */
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_RMDIR);
    أرجع SSH_NO_ERROR;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "rm ", 3)) {
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_UNLINK);
    أرجع SSH_NO_ERROR;
  }
#اگر_تعریف_شده HAS_STATVFS_SUPPORT
  وإلا إذا(!مقایسه_رشته(cmd, "statvfs ", 8)) {
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_STATVFS);
    أرجع SSH_NO_ERROR;
  }
#پایان_اگر

  failf(داده, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_بعدی_QUOTE(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_قلم = sshc->quote_قلم->بعدی;

  إذا(sshc->quote_قلم) {
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    إذا(sshc->nextstate != SSH_NO_STATE) {
      myssh_مقصد(داده, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    وإلا {
      myssh_مقصد(داده, sshc, SSH_SFTP_GETINFO);
    }
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_QUOTE_STAT(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc)
{
  ثابت حرف *cmd = sshc->quote_قلم->داده;
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
    sftp_صفات_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  إذا(!sshc->quote_attrs) {
    failf(داده, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(داده, sshc, عدم);
    أرجع SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  إذا(!مقایسه_رشته(cmd, "chgrp", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t gid;
    إذا(curlx_str_عدد(&p, &gid, UINT_MAX)) {
      failf(داده, "Syntax error: chgrp gid not a number");
      myssh_quote_error(داده, sshc, عدم);
      أرجع SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت حرف *p = sshc->quote_path1;
    إذا(curlx_str_octal(&p, &perms, 07777)) {
      failf(داده, "Syntax error: chmod permissions not a number");
      myssh_quote_error(داده, sshc, عدم);
      أرجع SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "chown", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t uid;
    إذا(curlx_str_عدد(&p, &uid, UINT_MAX)) {
      failf(داده, "Syntax error: chown uid not a number");
      myssh_quote_error(داده, sshc, عدم);
      أرجع SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "atime", 5) ||
          !مقایسه_رشته(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    إذا(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(داده, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#إذا SIZEOF_زمان_T > 4
    وإلا إذا(date > 0xffffffff) {
      failf(داده, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#پایان_اگر
    إذا(fail) {
      myssh_quote_error(داده, sshc, عدم);
      أرجع SSH_NO_ERROR;
    }
    إذا(!مقایسه_رشته(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    وإلا /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_SETSTAT);
  أرجع SSH_NO_ERROR;
}

ساكن فراغ conn_forget_socket(بنية Curl_easy *داده, int8_t sockindex)
{
  بنية connectdata *conn = داده->conn;
  إذا(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    بنية Curl_cfilter *cf = conn->cfilter[sockindex];
    إذا(cf)
      (فراغ)Curl_conn_cf_cntrl(cf, داده, TRUE, CF_CTRL_FORGET_SOCKET, 0, عدم);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

ساكن فراغ myssh_SESSION_DISCONNECT(بنية Curl_easy *داده,
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
    conn_forget_socket(داده, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
}

ساكن صحيح myssh_SSH_SCP_DOWNLOAD(بنية Curl_easy *داده,
                                  بنية ssh_conn *sshc)
{
  uint64_t bytecount;
  صحيح rc = ssh_scp_pull_request(sshc->scp_session);
  إذا(rc != SSH_SCP_REQUEST_NEWFILE) {
    ثابت حرف *err_پیام = ssh_get_error(sshc->ssh_session);
    failf(داده, "%s", err_پیام);
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_REMOTE_FILE_NOT_یافت);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  إذا(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_TOO_LARGE);
  داده->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, داده->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  داده->conn->send_idx = 0;

  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع 0;
}

ساكن صحيح myssh_درون_TRANS_INIT(بنية Curl_easy *داده, بنية ssh_conn *sshc,
                               بنية SSHPROTO *sshp)
{
  CURLcode نتیجه;
  صحيح rc = 0;
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);

  نتیجه = Curl_getworkingpath(داده, sshc->homedir, &sshp->مسیر);
  إذا(نتیجه) {
    sshc->actualcode = نتیجه;
    myssh_مقصد(داده, sshc, SSH_STOP);
    أرجع 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_مجموعه_blocking(sshc->ssh_session, 1);

  إذا(داده->state.upload) {
    إذا(داده->state.infilesize < 0) {
      failf(داده, "SCP requires a known file size for upload");
      أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->مسیر);
    myssh_مقصد(داده, sshc, SSH_SCP_UPLOAD_INIT);
  }
  وإلا {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->مسیر);
    myssh_مقصد(داده, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  إذا(!sshc->scp_session) {
    ثابت حرف *err_پیام = ssh_get_error(sshc->ssh_session);
    failf(داده, "%s", err_پیام);
    rc = myssh_مقصد_ERROR(داده, sshc, CURLE_UPLOAD_FAILED);
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
      sftp_صفات_free(sshc->quote_attrs);
      sshc->quote_attrs = عدم;
    }
    إذا(sshc->readdir_attrs) {
      sftp_صفات_free(sshc->readdir_attrs);
      sshc->readdir_attrs = عدم;
    }
    إذا(sshc->readdir_link_attrs) {
      sftp_صفات_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = عدم;
    }
    إذا(sshc->privkey) {
      ssh_کلید_free(sshc->privkey);
      sshc->privkey = عدم;
    }
    إذا(sshc->pubkey) {
      ssh_کلید_free(sshc->pubkey);
      sshc->pubkey = عدم;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_کلید);
    curlx_safefree(sshc->priv_کلید);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_link_مسیر);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

ساكن صحيح myssh_درون_SFTP_QUOTE_SETSTAT(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc)
{
  صحيح rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(داده, sshc, "setstat");
    أرجع rc;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_QUOTE_SYMLINK(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc)
{
  صحيح rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(داده, sshc, "symlink");
    أرجع rc;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_QUOTE_MKDIR(بنية Curl_easy *داده,
                                     بنية ssh_conn *sshc)
{
  صحيح rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)داده->مجموعه.new_directory_perms);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(داده, sshc, "mkdir");
    أرجع rc;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_QUOTE_RENAME(بنية Curl_easy *داده,
                                      بنية ssh_conn *sshc)
{
  صحيح rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(داده, sshc, "rename");
    أرجع rc;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_QUOTE_RMDIR(بنية Curl_easy *داده,
                                     بنية ssh_conn *sshc)
{
  صحيح rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(داده, sshc, "rmdir");
    أرجع rc;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_QUOTE_UNLINK(بنية Curl_easy *داده,
                                      بنية ssh_conn *sshc)
{
  صحيح rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  إذا(rc == SSH_AGAIN)
    أرجع rc;
  إذا(rc && !sshc->acceptfail) {
    myssh_quote_error(داده, sshc, "rm");
    أرجع rc;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_GETINFO(بنية Curl_easy *داده,
                                 بنية ssh_conn *sshc)
{
  إذا(داده->مجموعه.get_filetime)
    myssh_مقصد(داده, sshc, SSH_SFTP_FILETIME);
  وإلا
    myssh_مقصد(داده, sshc, SSH_SFTP_TRANS_INIT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_FILETIME(بنية Curl_easy *داده,
                                  بنية ssh_conn *sshc,
                                  بنية SSHPROTO *sshp)
{
  sftp_صفات attrs;
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->مسیر);
  إذا(attrs) {
    داده->info.filetime = attrs->mtime;
    sftp_صفات_free(attrs);
  }

  myssh_مقصد(داده, sshc, SSH_SFTP_TRANS_INIT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_TRANS_INIT(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);
  إذا(داده->state.upload)
    myssh_مقصد(داده, sshc, SSH_SFTP_UPLOAD_INIT);
  وإلا {
    size_t مسیر_len = طول_رشته(sshp->مسیر);

    إذا(مسیر_len && sshp->مسیر[مسیر_len - 1] == '/')
      myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_INIT);
    وإلا
      myssh_مقصد(داده, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_CREATE_DIRS_INIT(بنية Curl_easy *داده,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);

  إذا(طول_رشته(sshp->مسیر) > 1) {
    sshc->slash_pos = sshp->مسیر + 1; /* ignore the leading '/' */
    myssh_مقصد(داده, sshc, SSH_SFTP_CREATE_DIRS);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_CREATE_DIRS(بنية Curl_easy *داده,
                                     بنية ssh_conn *sshc,
                                     بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  إذا(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(داده, "Creating directory '%s'", sshp->مسیر);
    myssh_مقصد(داده, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    أرجع SSH_NO_ERROR;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SFTP_CREATE_DIRS_MKDIR(بنية Curl_easy *داده,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp)
{
  صحيح rc;
  صحيح err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->مسیر,
                  (mode_t)داده->مجموعه.new_directory_perms);
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
      أرجع myssh_مقصد_SFTP_CLOSE(داده, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_CREATE_DIRS);
  أرجع rc;
}

ساكن صحيح myssh_درون_SCP_UPLOAD_INIT(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp)
{
  صحيح rc;
  إذا(!sshp)
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  إذا(rc != SSH_OK) {
    ثابت حرف *err_پیام = ssh_get_error(sshc->ssh_session);
    failf(داده, "%s", err_پیام);
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_افزودن_file64(sshc->scp_session, sshp->مسیر,
                           (uint64_t)داده->state.infilesize,
                           (صحيح)داده->مجموعه.new_file_perms);

  إذا(rc != SSH_OK) {
    ثابت حرف *err_پیام = ssh_get_error(sshc->ssh_session);
    failf(داده, "%s", err_پیام);
    أرجع myssh_مقصد_ERROR(داده, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_راهاندازی_send(داده, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  داده->conn->recv_idx = FIRSTSOCKET;

  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SCP_DOWNLOAD_INIT(بنية Curl_easy *داده,
                                      بنية ssh_conn *sshc)
{
  صحيح rc = ssh_scp_init(sshc->scp_session);
  إذا(rc != SSH_OK) {
    ثابت حرف *err_پیام = ssh_get_error(sshc->ssh_session);
    failf(داده, "%s", err_پیام);
    أرجع myssh_مقصد_ERROR(داده, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_مقصد(داده, sshc, SSH_SCP_DOWNLOAD);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SCP_انجامشده(بنية Curl_easy *داده,
                             بنية ssh_conn *sshc)
{
  إذا(داده->state.upload)
    myssh_مقصد(داده, sshc, SSH_SCP_SEND_EOF);
  وإلا
    myssh_مقصد(داده, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SCP_SEND_EOF(بنية Curl_easy *داده,
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
      infof(داده,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_مقصد(داده, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع SSH_NO_ERROR;
}

ساكن صحيح myssh_درون_SCP_CHANNEL_FREE(بنية Curl_easy *داده,
                                     بنية ssh_conn *sshc)
{
  إذا(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = عدم;
  }
  CURL_TRC_SSH(داده, "SCP DONE phase complete");

  ssh_مجموعه_blocking(sshc->ssh_session, 0);

  myssh_مقصد(داده, sshc, SSH_SESSION_DISCONNECT);
  أرجع SSH_NO_ERROR;
}

ساكن CURLcode myssh_درون_SESSION_FREE(بنية Curl_easy *داده,
                                      بنية ssh_conn *sshc)
{
  CURLcode نتیجه;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  نتیجه = sshc->actualcode;
  memset(sshc, 0, حجم(بنية ssh_conn));
  connclose(داده->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع نتیجه;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
ساكن CURLcode myssh_statemachine(بنية Curl_easy *داده,
                                   بنية ssh_conn *sshc,
                                   بنية SSHPROTO *sshp,
                                   bool *بلوک)
{
  CURLcode نتیجه = CURLE_OK;
  صحيح rc = SSH_NO_ERROR;

  *بلوک = FALSE;                   /* we are not blocking by default */
  افعل {

    بدل(sshc->state) {
    حالة SSH_INIT:
      myssh_state_init(داده, sshc);
      FALLTHROUGH();
    حالة SSH_S_STARTUP:
      rc = myssh_درون_S_STARTUP(داده, sshc);
      إذا(rc)
        اكسر;
      FALLTHROUGH();
    حالة SSH_HOSTKEY:
      rc = myssh_is_معلوم(داده, sshc);
      إذا(rc != SSH_OK) {
        rc = myssh_مقصد_ERROR(داده, sshc, CURLE_PEER_FAILED_VERIFICATION);
        اكسر;
      }
      myssh_مقصد(داده, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    حالة SSH_AUTHLIST:
      rc = myssh_درون_AUTHLIST(داده, sshc);
      اكسر;
    حالة SSH_AUTH_PKEY_INIT:
      rc = myssh_درون_AUTH_PKEY_INIT(داده, sshc);
      اكسر;
    حالة SSH_AUTH_PKEY:
      rc = myssh_درون_AUTH_PKEY(داده, sshc);
      اكسر;
    حالة SSH_AUTH_GSSAPI:
      rc = myssh_درون_AUTH_GSSAPI(داده, sshc);
      اكسر;
    حالة SSH_AUTH_کلید_INIT:
      rc = myssh_درون_AUTH_کلید_INIT(داده, sshc);
      اكسر;
    حالة SSH_AUTH_کلید:
      rc = myssh_درون_AUTH_کلید(داده, sshc);
      اكسر;
    حالة SSH_AUTH_PASS_INIT:
      rc = myssh_درون_AUTH_PASS_INIT(داده, sshc);
      اكسر;
    حالة SSH_AUTH_PASS:
      rc = myssh_درون_AUTH_PASS(داده, sshc);
      اكسر;
    حالة SSH_AUTH_انجامشده:
      rc = myssh_درون_AUTH_انجامشده(داده, sshc);
      اكسر;
    حالة SSH_SFTP_INIT:
      rc = myssh_درون_SFTP_INIT(داده, sshc);
      اكسر;
    حالة SSH_SFTP_REALPATH:
      rc = myssh_درون_SFTP_REALPATH(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_INIT:
      rc = myssh_درون_SFTP_QUOTE_INIT(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_درون_SFTP_POSTQUOTE_INIT(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE:
      rc = myssh_درون_SFTP_QUOTE(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_بعدی_QUOTE:
      rc = myssh_درون_SFTP_بعدی_QUOTE(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_STAT:
      rc = myssh_درون_SFTP_QUOTE_STAT(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_درون_SFTP_QUOTE_SETSTAT(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_درون_SFTP_QUOTE_SYMLINK(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_درون_SFTP_QUOTE_MKDIR(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_RENAME:
      rc = myssh_درون_SFTP_QUOTE_RENAME(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_درون_SFTP_QUOTE_RMDIR(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_درون_SFTP_QUOTE_UNLINK(داده, sshc);
      اكسر;
    حالة SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_درون_SFTP_QUOTE_STATVFS(داده, sshc);
      اكسر;
    حالة SSH_SFTP_GETINFO:
      rc = myssh_درون_SFTP_GETINFO(داده, sshc);
      اكسر;
    حالة SSH_SFTP_FILETIME:
      rc = myssh_درون_SFTP_FILETIME(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_TRANS_INIT:
      rc = myssh_درون_SFTP_TRANS_INIT(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_UPLOAD_INIT:
      rc = myssh_درون_UPLOAD_INIT(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_درون_SFTP_CREATE_DIRS_INIT(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_CREATE_DIRS:
      rc = myssh_درون_SFTP_CREATE_DIRS(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_درون_SFTP_CREATE_DIRS_MKDIR(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_READDIR_INIT:
      rc = myssh_درون_SFTP_READDIR_INIT(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_READDIR:
      rc = myssh_درون_SFTP_READDIR(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_READDIR_LINK:
      rc = myssh_درون_SFTP_READDIR_LINK(داده, sshc);
      اكسر;
    حالة SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_درون_SFTP_READDIR_BOTTOM(داده, sshc);
      اكسر;
    حالة SSH_SFTP_READDIR_انجامشده:
      rc = myssh_درون_SFTP_READDIR_انجامشده(داده, sshc);
      اكسر;
    حالة SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_درون_SFTP_DOWNLOAD_INIT(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_درون_SFTP_DOWNLOAD_STAT(داده, sshc);
      اكسر;
    حالة SSH_SFTP_CLOSE:
      rc = myssh_درون_SFTP_CLOSE(داده, sshc, sshp);
      اكسر;
    حالة SSH_SFTP_SHUTDOWN:
      rc = myssh_درون_SFTP_SHUTDOWN(داده, sshc);
      اكسر;
    حالة SSH_SCP_TRANS_INIT:
      rc = myssh_درون_TRANS_INIT(داده, sshc, sshp);
      اكسر;
    حالة SSH_SCP_UPLOAD_INIT:
      rc = myssh_درون_SCP_UPLOAD_INIT(داده, sshc, sshp);
      اكسر;
    حالة SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_درون_SCP_DOWNLOAD_INIT(داده, sshc);
      إذا(rc)
        اكسر;
      FALLTHROUGH();
    حالة SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(داده, sshc);
      اكسر;
    حالة SSH_SCP_انجامشده:
      rc = myssh_درون_SCP_انجامشده(داده, sshc);
      اكسر;
    حالة SSH_SCP_SEND_EOF:
      rc = myssh_درون_SCP_SEND_EOF(داده, sshc);
      اكسر;
    حالة SSH_SCP_CHANNEL_FREE:
      myssh_درون_SCP_CHANNEL_FREE(داده, sshc);
      FALLTHROUGH();
    حالة SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(داده, sshc);
      FALLTHROUGH();
    حالة SSH_SESSION_FREE:
      نتیجه = myssh_درون_SESSION_FREE(داده, sshc);
      اكسر;
    حالة SSH_QUIT:
    افتراضي:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_مقصد(داده, sshc, SSH_STOP);
      اكسر;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } طالما((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  إذا(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *بلوک = TRUE;
  }
  إذا(!نتیجه && (sshc->state == SSH_STOP))
    نتیجه = sshc->actualcode;
  CURL_TRC_SSH(داده, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحيح)نتیجه, *بلوک);
  أرجع نتیجه;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode myssh_multi_statemach(بنية Curl_easy *داده,
                                      bool *انجامشده)
{
  بنية connectdata *conn = داده->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(داده, CURL_META_SSH_EASY);
  bool بلوک;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode نتیجه;

  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;
  نتیجه = myssh_statemachine(داده, sshc, sshp, &بلوک);
  *انجامشده = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, بلوک);

  أرجع نتیجه;
}

ساكن CURLcode myssh_بلوک_statemach(بنية Curl_easy *داده,
                                      بنية ssh_conn *sshc,
                                      بنية SSHPROTO *sshp,
                                      bool disconnect)
{
  بنية connectdata *conn = داده->conn;
  CURLcode نتیجه = CURLE_OK;

  طالما((sshc->state != SSH_STOP) && !نتیجه) {
    bool بلوک;
    timediff_t چپ_ms = 1000;

    نتیجه = myssh_statemachine(داده, sshc, sshp, &بلوک);
    إذا(نتیجه)
      اكسر;

    إذا(!disconnect) {
      نتیجه = Curl_pgrsCheck(داده);
      إذا(نتیجه)
        اكسر;

      چپ_ms = Curl_timeleft_ms(داده);
      إذا(چپ_ms < 0) {
        failf(داده, "Operation timed out");
        أرجع CURLE_OPERATION_TIMEDOUT;
      }
    }

    إذا(بلوک) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (فراغ)SOCKET_READABLE(fd_read,
                            (!چپ_ms || چپ_ms > 1000) ? 1000 : چپ_ms);
    }
  }

  أرجع نتیجه;
}

ساكن فراغ myssh_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية SSHPROTO *sshp = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_safefree(sshp->مسیر);
  curlx_free(sshp);
}

ساكن فراغ myssh_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية ssh_conn *sshc = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
ساكن CURLcode myssh_راهاندازی_connection(بنية Curl_easy *داده,
                                       بنية connectdata *conn)
{
  بنية SSHPROTO *sshp;
  بنية ssh_conn *sshc;

  sshc = curlx_calloc(1, حجم(*sshc));
  إذا(!sshc)
    أرجع CURLE_بیرون_از_حافظه;

  curlx_dyn_init(&sshc->readdir_buf, CURL_مسیر_MAX * 2);
  sshc->initialized = TRUE;
  إذا(Curl_conn_meta_مجموعه(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  sshp = curlx_calloc(1, حجم(*sshp));
  إذا(!sshp ||
     Curl_meta_مجموعه(داده, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  أرجع Curl_ssh_راهاندازی_pkey(داده, sshc);
}

ساكن Curl_recv scp_recv, sftp_recv;
ساكن Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ساكن CURLcode myssh_connect(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه;
  بنية connectdata *conn = داده->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *ssh = Curl_meta_get(داده, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح rc;

  إذا(!sshc || !ssh)
    أرجع CURLE_FAILED_INIT;

  CURL_TRC_SSH(داده, "myssh_connect");
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
    failf(داده, "Failure initializing ssh session");
    أرجع CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_مجموعه(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  إذا(rc != SSH_OK) {
    failf(داده, "Could not set remote host");
    أرجع CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, عدم);
  إذا(rc != SSH_OK) {
    infof(داده, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(داده, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_مجموعه(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  إذا(rc != SSH_OK) {
    failf(داده, "Could not set socket");
    أرجع CURLE_FAILED_INIT;
  }

  إذا(Curl_creds_has_user(conn->creds)) {
    infof(داده, "User: %s", conn->creds->user);
    rc = ssh_options_مجموعه(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    إذا(rc != SSH_OK) {
      failf(داده, "Could not set user");
      أرجع CURLE_FAILED_INIT;
    }
  }

  إذا(CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS)) {
    infof(داده, "Known hosts: %s",
          CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_مجموعه(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS));
    إذا(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_مجموعه(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS));
    إذا(rc != SSH_OK) {
      failf(داده, "Could not set known hosts file path");
      أرجع CURLE_FAILED_INIT;
    }
  }

  إذا(conn->origin->پورت) {
    rc = ssh_options_مجموعه(sshc->ssh_session, SSH_OPTIONS_پورت,
                         &conn->origin->پورت);
    إذا(rc != SSH_OK) {
      failf(داده, "Could not set remote port");
      أرجع CURLE_FAILED_INIT;
    }
  }

  إذا(داده->مجموعه.ssh_compression) {
    rc = ssh_options_مجموعه(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    إذا(rc != SSH_OK) {
      failf(داده, "Could not set compression");
      أرجع CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = عدم;
  sshc->pubkey = عدم;

  إذا(sshc->pub_کلید) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_کلید, &sshc->pubkey);
    إذا(rc != SSH_OK) {
      failf(داده, "Could not load public key file");
      أرجع CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_مقصد(داده, sshc, SSH_INIT);

  نتیجه = myssh_multi_statemach(داده, انجامشده);

  أرجع نتیجه;
}

/* called from multi.c while DOing */
ساكن CURLcode scp_doing(بنية Curl_easy *داده, bool *dophase_انجامشده)
{
  CURLcode نتیجه;

  نتیجه = myssh_multi_statemach(داده, dophase_انجامشده);

  إذا(*dophase_انجامشده) {
    CURL_TRC_SSH(داده, "DO phase is complete");
  }
  أرجع نتیجه;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ساكن CURLcode scp_perform(بنية Curl_easy *داده,
                            bool *connected, bool *dophase_انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(داده->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(داده, "DO phase starts");

  *dophase_انجامشده = FALSE;        /* not done yet */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_مقصد(داده, sshc, SSH_SCP_TRANS_INIT);

  نتیجه = myssh_multi_statemach(داده, dophase_انجامشده);

  *connected = Curl_conn_is_connected(داده->conn, FIRSTSOCKET);

  إذا(*dophase_انجامشده) {
    CURL_TRC_SSH(داده, "DO phase is complete");
  }

  أرجع نتیجه;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode scp_disconnect(بنية Curl_easy *داده,
                               بنية connectdata *conn,
                               bool dead_connection)
{
  CURLcode نتیجه = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(داده, CURL_META_SSH_EASY);
  (فراغ)dead_connection;

  إذا(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_مقصد(داده, sshc, SSH_SESSION_DISCONNECT);
    نتیجه = myssh_بلوک_statemach(داده, sshc, sshp, TRUE);
  }

  أرجع نتیجه;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ساكن CURLcode myssh_انجامشده(بنية Curl_easy *داده,
                           بنية ssh_conn *sshc,
                           CURLcode وضعیت)
{
  CURLcode نتیجه = CURLE_OK;
  بنية SSHPROTO *sshp = Curl_meta_get(داده, CURL_META_SSH_EASY);

  إذا(!وضعیت && sshp) {
    /* run the state-machine */
    نتیجه = myssh_بلوک_statemach(داده, sshc, sshp, FALSE);
  }
  وإلا
    نتیجه = وضعیت;

  إذا(Curl_pgrs_انجامشده(داده, Curl_pgrs_now(داده)))
    أرجع CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(داده);
  أرجع نتیجه;
}

ساكن CURLcode scp_انجامشده(بنية Curl_easy *داده, CURLcode وضعیت,
                         bool premature)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(داده->conn, CURL_META_SSH_CONN);
  (فراغ)premature;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;
  إذا(!وضعیت)
    myssh_مقصد(داده, sshc, SSH_SCP_انجامشده);

  أرجع myssh_انجامشده(داده, sshc, وضعیت);
}

ساكن CURLcode scp_send(بنية Curl_easy *داده, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  صحيح rc;
  بنية connectdata *conn = داده->conn;
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
#پایان_اگر
  إذا(rc != SSH_OK)
    أرجع CURLE_SSH;

  *pnwritten = len;
  أرجع CURLE_OK;
}

ساكن CURLcode scp_recv(بنية Curl_easy *داده, int8_t sockindex,
                         حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = داده->conn;
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
#پایان_اگر
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

ساكن CURLcode sftp_perform(بنية Curl_easy *داده,
                             bool *connected,
                             bool *dophase_انجامشده)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(داده->conn, CURL_META_SSH_CONN);
  CURLcode نتیجه = CURLE_OK;

  CURL_TRC_SSH(داده, "DO phase starts");

  *dophase_انجامشده = FALSE; /* not done yet */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  نتیجه = myssh_multi_statemach(داده, dophase_انجامشده);

  *connected = Curl_conn_is_connected(داده->conn, FIRSTSOCKET);

  إذا(*dophase_انجامشده) {
    CURL_TRC_SSH(داده, "DO phase is complete");
  }

  أرجع نتیجه;
}

/* called from multi.c while DOing */
ساكن CURLcode sftp_doing(بنية Curl_easy *داده,
                           bool *dophase_انجامشده)
{
  CURLcode نتیجه = myssh_multi_statemach(داده, dophase_انجامشده);
  إذا(*dophase_انجامشده) {
    CURL_TRC_SSH(داده, "DO phase is complete");
  }
  أرجع نتیجه;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode sftp_disconnect(بنية Curl_easy *داده,
                                بنية connectdata *conn,
                                bool dead_connection)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(داده, CURL_META_SSH_EASY);
  CURLcode نتیجه = CURLE_OK;
  (فراغ)dead_connection;

  CURL_TRC_SSH(داده, "DISCONNECT starts now");

  إذا(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_مقصد(داده, sshc, SSH_SFTP_SHUTDOWN);
    نتیجه = myssh_بلوک_statemach(داده, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(داده, "DISCONNECT is done");
  أرجع نتیجه;
}

ساكن CURLcode sftp_انجامشده(بنية Curl_easy *داده, CURLcode وضعیت,
                          bool premature)
{
  بنية connectdata *conn = داده->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;
  إذا(!وضعیت) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    إذا(!premature && داده->مجموعه.postquote && !conn->بیتها.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
  }
  أرجع myssh_انجامشده(داده, sshc, وضعیت);
}

/* return number of sent bytes */
ساكن CURLcode sftp_send(بنية Curl_easy *داده, int8_t sockindex,
                          ثابت uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  بنية connectdata *conn = داده->conn;
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
    sftp_file_مجموعه_nonblocking(sshc->sftp_file);
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
#پایان_اگر
  إذا(nwrite < 0)
    أرجع CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  أرجع CURLE_OK;
#پایان_اگر
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ساكن CURLcode sftp_recv(بنية Curl_easy *داده, int8_t sockindex,
                          حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = داده->conn;
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
#پایان_اگر

    FALLTHROUGH();
  حالة 1:
    sshc->sftp_recv_state = 1;

#إذا LIBSSH_نسخه_INT > SSH_نسخه_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#وإلا
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_شاخص);
#پایان_اگر

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

ساكن CURLcode myssh_do_it(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه;
  bool connected = FALSE;
  بنية connectdata *conn = داده->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *انجامشده = FALSE;                /* default to false */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  داده->req.اندازه = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->دومی_Create_Dirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(داده);

  إذا(conn->scheme->protocol & CURLPROTO_SCP)
    نتیجه = scp_perform(داده, &connected, انجامشده);
  وإلا
    نتیجه = sftp_perform(داده, &connected, انجامشده);

  أرجع نتیجه;
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

فراغ Curl_ssh_نسخه(حرف *میانگیر, size_t buflen)
{
  (فراغ)curl_msnprintf(میانگیر, buflen, "libssh/%s", ssh_نسخه(0));
}

/*
 * SCP.
 */
ثابت بنية Curl_protocol Curl_protocol_scp = {
  myssh_راهاندازی_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_انجامشده,                             /* done */
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
  myssh_راهاندازی_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_انجامشده,                            /* done */
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

#پایان_اگر /* USE_LIBSSH */
