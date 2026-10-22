/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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

#جے_تعریف_شدہ USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ __VMS
#ضمن <in.h>
#ضمن <inet.h>
#جے_انت

#ضمن "urldata.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "transfer.h"
#ضمن "vssh/ssh.h"
#ضمن "url.h"
#ضمن "cfilters.h"
#ضمن "connect.h"
#ضمن "parsedate.h" /* for the week day and month names */
#ضمن "multiif.h"
#ضمن "select.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "vssh/vssh.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/base64.h" /* for curlx_base64_encode() */

ساكن ثابت حرف *sftp_libssh2_strerror(بنانشانی لما err)
{
  بدل(err) {
  حالة LIBSSH2_FX_NO_SUCH_FILE:
    أرجع "No such file or directory";

  حالة LIBSSH2_FX_PERMISSION_DENIED:
    أرجع "Permission denied";

  حالة LIBSSH2_FX_FAILURE:
    أرجع "Operation failed";

  حالة LIBSSH2_FX_BAD_سنیہا:
    أرجع "Bad message from SFTP server";

  حالة LIBSSH2_FX_NO_CONNECTION:
    أرجع "Not connected to SFTP server";

  حالة LIBSSH2_FX_CONNECTION_گواچا:
    أرجع "Connection to SFTP server lost";

  حالة LIBSSH2_FX_OP_UNSUPPORTED:
    أرجع "Operation not supported by SFTP server";

  حالة LIBSSH2_FX_INVALID_HANDLE:
    أرجع "Invalid handle";

  حالة LIBSSH2_FX_NO_SUCH_رستہ:
    أرجع "No such file or directory";

  حالة LIBSSH2_FX_FILE_ALREADY_EXISTS:
    أرجع "File already exists";

  حالة LIBSSH2_FX_WRITE_PROTECT:
    أرجع "File is write protected";

  حالة LIBSSH2_FX_NO_MEDIA:
    أرجع "No media";

  حالة LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    أرجع "Disk full";

  حالة LIBSSH2_FX_QUOTA_EXCEEDED:
    أرجع "User quota exceeded";

  حالة LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    أرجع "Unknown principal";

  حالة LIBSSH2_FX_جندرا_CONFLICT:
    أرجع "File lock conflict";

  حالة LIBSSH2_FX_DIR_NOT_خالی_تھاواں:
    أرجع "Directory not empty";

  حالة LIBSSH2_FX_NOT_A_DIRECTORY:
    أرجع "Not a directory";

  حالة LIBSSH2_FX_INVALID_FILENAME:
    أرجع "Invalid filename";

  حالة LIBSSH2_FX_LINK_LOOP:
    أرجع "Link points to itself";
  }
  أرجع "Unknown error in libssh2";
}

ساكن فراغ kbd_callback(ثابت حرف *name, صحيح name_len,
                         ثابت حرف *instruction, صحيح instruction_len,
                         صحيح عدد_prompts,
                         ثابت LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         فراغ **abstract)
{
  بنية Curl_easy *انکڑے = (بنية Curl_easy *)*abstract;

#جے_تعریف_شدہ CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", عدد_prompts);
#وإلا
  (فراغ)name;
  (فراغ)name_len;
  (فراغ)instruction;
  (فراغ)instruction_len;
#جے_انت /* CURL_LIBSSH2_DEBUG */
  إذا(عدد_prompts == 1) {
    بنية connectdata *conn = انکڑے->conn;
    ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].متن_3 = Curl_cstrdup(passwd);
    responses[0].لمائی = responses[0].متن_3 ? curlx_uztoui(لڑی_لمبائی(passwd)) : 0;
  }
  (فراغ)prompts;
} /* kbd_callback */

ساكن CURLcode sftp_libssh2_error_منزل_CURLE(بنانشانی لما err)
{
  بدل(err) {
  حالة LIBSSH2_FX_OK:
    أرجع CURLE_OK;

  حالة LIBSSH2_FX_NO_SUCH_FILE:
  حالة LIBSSH2_FX_NO_SUCH_رستہ:
    أرجع CURLE_REMOTE_FILE_NOT_لبھیا;

  حالة LIBSSH2_FX_PERMISSION_DENIED:
  حالة LIBSSH2_FX_WRITE_PROTECT:
  حالة LIBSSH2_FX_جندرا_CONFLICT:
    أرجع CURLE_REMOTE_ACCESS_DENIED;

  حالة LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  حالة LIBSSH2_FX_QUOTA_EXCEEDED:
    أرجع CURLE_REMOTE_DISK_بھریا;

  حالة LIBSSH2_FX_FILE_ALREADY_EXISTS:
    أرجع CURLE_REMOTE_FILE_EXISTS;

  حالة LIBSSH2_FX_DIR_NOT_خالی_تھاواں:
    أرجع CURLE_QUOTE_ERROR;

  افتراضي:
    اكسر;
  }

  أرجع CURLE_SSH;
}

ساكن CURLcode libssh2_session_error_منزل_CURLE(صحيح err)
{
  بدل(err) {
  /* Ordered by order of appearance in libssh2.h */
  حالة LIBSSH2_ERROR_NONE:
    أرجع CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  حالة LIBSSH2_ERROR_SCP_PROTOCOL:
    أرجع CURLE_REMOTE_FILE_NOT_لبھیا;

  حالة LIBSSH2_ERROR_SOCKET_NONE:
    أرجع CURLE_COULDNT_CONNECT;

  حالة LIBSSH2_ERROR_ALLOC:
    أرجع CURLE_باہرلا_دا_حافظہ;

  حالة LIBSSH2_ERROR_SOCKET_SEND:
    أرجع CURLE_SEND_ERROR;

  حالة LIBSSH2_ERROR_HOSTKEY_INIT:
  حالة LIBSSH2_ERROR_HOSTKEY_SIGN:
  حالة LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  حالة LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    أرجع CURLE_PEER_FAILED_VERIFICATION;

  حالة LIBSSH2_ERROR_PASSWORD_EXPIRED:
    أرجع CURLE_LOGIN_DENIED;

  حالة LIBSSH2_ERROR_SOCKET_TIMEOUT:
  حالة LIBSSH2_ERROR_TIMEOUT:
    أرجع CURLE_OPERATION_TIMEDOUT;

  حالة LIBSSH2_ERROR_EAGAIN:
    أرجع CURLE_AGAIN;
  }

  أرجع CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

ساكن LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (فراغ)abstract;
  أرجع Curl_cmalloc(گنتی);
}

ساكن LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (فراغ)abstract;
  أرجع Curl_crealloc(ptr, گنتی);
}

ساكن LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (فراغ)abstract;
  إذا(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

ساكن صحيح sshkeycallback(CURL *easy,
                          ثابت بنية curl_khkey *knownkey, /*معلوم*/
                          ثابت بنية curl_khkey *foundkey, /* found */
                          تعداد curl_khmatch میل,
                          فراغ *clientp)
{
  (فراغ)easy;
  (فراغ)knownkey;
  (فراغ)foundkey;
  (فراغ)clientp;

  /* we only allow perfect matches, and we reject everything else */
  أرجع (میل != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ساكن تعداد curl_khtype convert_ssh2_keytype(صحيح sshkeytype)
{
  تعداد curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  بدل(sshkeytype) {
  حالة LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    اكسر;
#جے_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_DSS
  حالة LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    اكسر;
#جے_انت
#جے_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#جے_انت
#جے_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#جے_انت
#جے_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#جے_انت
#جے_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_ED25519
  حالة LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    اكسر;
#جے_انت
  }
  أرجع keytype;
}

ساكن CURLcode ssh_knownhost(بنية Curl_easy *انکڑے,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية libssh2_knownhost *host = عدم;
  ثابت حرف *remotekey = عدم;
  صحيح keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  صحيح keybit = 0;
  صحيح sshkeytype = 0;
  size_t keylen = 0;
  صحيح rc = 0;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!CURL_EASY_STR(انکڑے, STRING_SSH_KNOWNHOSTS)) {
    infof(انکڑے, "SSH: no knownhosts file configured");
    أرجع CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  إذا(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    بنية Curl_mapi_guard guard;
    تعداد curl_khmatch keymatch;
    curl_sshkeycallback func =
      انکڑے->سیٹ.ssh_keyfunc ? انکڑے->سیٹ.ssh_keyfunc : sshkeycallback;
    بنية curl_khkey knownkey;
    بنية curl_khkey *knownkeyp = عدم;
    بنية curl_khkey foundkey;

    بدل(sshkeytype) {
    حالة LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_کلید_SSHRSA;
      اكسر;
#جے_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_DSS
    حالة LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_کلید_SSHDSS;
      اكسر;
#جے_انت
    حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_کلید_ECDSA_256;
      اكسر;
    حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_کلید_ECDSA_384;
      اكسر;
    حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_کلید_ECDSA_521;
      اكسر;
    حالة LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_کلید_ED25519;
      اكسر;
    افتراضي:
      infof(انکڑے, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      اكسر;
    }
    إذا(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    وإلا {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->پورٹ != پورٹ_SSH) ?
                                          conn->origin->پورٹ : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(انکڑے, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->کلید : "<none>");

      /* setup 'knownkey' */
      إذا(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.کلید = host->کلید;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.کلید = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (تعداد curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_شروع(&guard, انکڑے, easy_ssh_keyfunc);
      rc = func(انکڑے, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, انکڑے->سیٹ.ssh_keyfunc_userp);
      CURL_CBAPI_سرا(&guard);
    }
  }
  وإلا {
    /* no remotekey means failure! */
    infof(انکڑے, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  بدل(rc) {
  افتراضي: /* unknown return codes is the same as reject */
  حالة CURLKHSTAT_REJECT:
    infof(انکڑے, "SSH: knownhost check failed");
    myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  حالة CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    اكسر;
  حالة CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    إذا(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  حالة CURLKHSTAT_FINE:
  حالة CURLKHSTAT_FINE_جوڑو_منزل_FILE:
    /* proceed */
    إذا(keycheck != LIBSSH2_KNOWNHOST_CHECK_میل) {
      صحيح addrc;
      ثابت حرف *hostbuf;
      حرف *hostport = عدم;
      إذا(conn->origin->پورٹ != پورٹ_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->پورٹ);
        إذا(!hostbuf)
          infof(انکڑے, "WARNING: failed allocating buffer for [host]:port");
      }
      وإلا
        hostbuf = conn->origin->hostname;
      إذا(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, عدم,
                                       remotekey, keylen, عدم, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, عدم);
        إذا(addrc)
          infof(انکڑے, "WARNING: adding the known host %s failed", hostbuf);
        وإلا إذا(rc == CURLKHSTAT_FINE_جوڑو_منزل_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          صحيح wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(انکڑے, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          إذا(wrc) {
            infof(انکڑے, "WARNING: writing %s failed",
                  CURL_EASY_STR(انکڑے, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    وإلا
      infof(انکڑے, "SSH: knownhost entry matches host key");
    اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode ssh_check_fingerprint(بنية Curl_easy *انکڑے,
                                      بنية ssh_conn *sshc)
{
  ثابت حرف *pubkey_md5 =
    CURL_EASY_STR(انکڑے, STRING_SSH_HOST_عوامی_کلید_MD5);
  ثابت حرف *pubkey_sha256 =
    CURL_EASY_STR(انکڑے, STRING_SSH_HOST_عوامی_کلید_SHA256);

  إذا(pubkey_sha256) {
    ثابت حرف *fingerprint = عدم;
    حرف *fingerprint_b64 = عدم;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(انکڑے, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    إذا(!fingerprint) {
      failf(انکڑے,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    إذا(curlx_base64_encode((ثابت uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    إذا(!fingerprint_b64) {
      failf(انکڑے, "SHA256 fingerprint could not be encoded");
      myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(انکڑے, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

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
       لڑی_مقابلہ(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(انکڑے,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(انکڑے, "SSH: SHA256 checksum match");
  }

  إذا(pubkey_md5) {
    حرف md5buffer[33];
    ثابت حرف *fingerprint;

    infof(انکڑے, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    إذا(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      صحيح i;
      لكل(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (بنانشانی حرف)fingerprint[i]);
      }

      infof(انکڑے, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    إذا(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      إذا(fingerprint) {
        failf(انکڑے,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      وإلا {
        failf(انکڑے,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(انکڑے, "SSH: MD5 checksum match");
  }

  إذا(!pubkey_md5 && !pubkey_sha256) {
    إذا(انکڑے->سیٹ.ssh_hostkeyfunc) {
      size_t keylen = 0;
      صحيح sshkeytype = 0;
      صحيح rc = 0;
      /* we handle the process to the callback */
      ثابت حرف *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      إذا(remotekey) {
        بنية Curl_mapi_guard guard;
        تعداد curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_شروع(&guard, انکڑے, easy_ssh_hostkeyfunc);
        rc = انکڑے->سیٹ.ssh_hostkeyfunc(انکڑے->سیٹ.ssh_hostkeyfunc_userp,
                                       (صحيح)keytype, remotekey, keylen);
        CURL_CBAPI_سرا(&guard);
        إذا(rc != CURLKHMATCH_OK) {
          myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
          failf(انکڑے, "SSH: callback failed host public key verification");
          أرجع CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(انکڑے, "SSH: verified public key via callback");
      }
      وإلا {
        myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
        أرجع CURLE_PEER_FAILED_VERIFICATION;
      }
      أرجع CURLE_OK;
    }
    وإلا {
      CURL_TRC_SSH(انکڑے, "no host key checksum given, checking knownhosts");
      أرجع ssh_knownhost(انکڑے, sshc);
    }
  }
  وإلا {
    /* as we already matched, we skip the check for known hosts */
    أرجع CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
ساكن CURLcode ssh_force_knownhost_کلید_type(بنية Curl_easy *انکڑے,
                                             بنية ssh_conn *sshc)
{
  CURLcode نتیجہ = CURLE_OK;

  ساكن ثابت حرف hostkey_طریقہ_ssh_ed25519[] = "ssh-ed25519";
  ساكن ثابت حرف hostkey_طریقہ_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ساكن ثابت حرف hostkey_طریقہ_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ساكن ثابت حرف hostkey_طریقہ_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ساكن ثابت حرف hostkey_طریقہ_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#جے_تعریف_شدہ LIBSSH2_KNOWNHOST_کلید_SSHDSS
  ساكن ثابت حرف hostkey_طریقہ_ssh_dss[] = "ssh-dss";
#جے_انت
  bool لبھیا = FALSE;

  إذا(sshc->kh &&
     !CURL_EASY_STR(انکڑے, STRING_SSH_HOST_عوامی_کلید_MD5) &&
     !CURL_EASY_STR(انکڑے, STRING_SSH_HOST_عوامی_کلید_SHA256)) {
    بنية libssh2_knownhost *store = عدم;
    بنية connectdata *conn = انکڑے->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_کلید_نقاب >>
                      LIBSSH2_KNOWNHOST_کلید_کھسکاؤ) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    طالما(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      إذا(store) {
        إذا(store->name) {
          إذا(store->name[0] == '[') {
            curl_off_t پورٹ;
            size_t kh_name_ناپ = 0;
            ثابت حرف *p;
            ثابت حرف *kh_name_سرا = لڑی_کھوج(store->name, "]:");
            إذا(!kh_name_سرا) {
              infof(انکڑے, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(انکڑے, STRING_SSH_KNOWNHOSTS));
              استمر;
            }
            p = kh_name_سرا + 2; /* start of port number */
            إذا(!curlx_str_نمبر(&p, &پورٹ, 0xffff) &&
               (kh_name_سرا && (پورٹ == conn->origin->پورٹ))) {
              kh_name_ناپ = لڑی_لمبائی(store->name) - 1 - لڑی_لمبائی(kh_name_سرا);
              إذا((kh_name_ناپ == لڑی_لمبائی(conn->origin->hostname)) &&
                 !لڑی_مقابلہ(store->name + 1, conn->origin->hostname,
                          kh_name_ناپ)) {
                لبھیا = TRUE;
                اكسر;
              }
            }
          }
          وإلا إذا(!strcmp(store->name, conn->origin->hostname)) {
            لبھیا = TRUE;
            اكسر;
          }
        }
        وإلا إذا(store->کلید) {
          بنانشانی صحيح type = (store->typemask & LIBSSH2_KNOWNHOST_کلید_نقاب)
                              >> LIBSSH2_KNOWNHOST_کلید_کھسکاؤ;
          إذا(!absent_type[type]) {
            صحيح keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->پورٹ != پورٹ_SSH) ? conn->origin->پورٹ : -1,
              store->کلید, لڑی_لمبائی(store->کلید),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_کلید_نقاب),
              عدم);
            إذا(keycheck == LIBSSH2_KNOWNHOST_CHECK_میل) {
              لبھیا = TRUE;
              اكسر;
            }
            وإلا إذا(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    إذا(لبھیا) {
      صحيح rc;
      ثابت حرف *hostkey_طریقہ = عدم;
      infof(انکڑے, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(انکڑے, STRING_SSH_KNOWNHOSTS));

      بدل(store->typemask & LIBSSH2_KNOWNHOST_کلید_نقاب) {
      حالة LIBSSH2_KNOWNHOST_کلید_ED25519:
        hostkey_طریقہ = hostkey_طریقہ_ssh_ed25519;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلید_ECDSA_521:
        hostkey_طریقہ = hostkey_طریقہ_ssh_ecdsa_521;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلید_ECDSA_384:
        hostkey_طریقہ = hostkey_طریقہ_ssh_ecdsa_384;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلید_ECDSA_256:
        hostkey_طریقہ = hostkey_طریقہ_ssh_ecdsa_256;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلید_SSHRSA:
        hostkey_طریقہ = hostkey_طریقہ_ssh_rsa_all;
        اكسر;
#جے_تعریف_شدہ LIBSSH2_KNOWNHOST_کلید_SSHDSS
      حالة LIBSSH2_KNOWNHOST_کلید_SSHDSS:  /* deprecated upstream */
        hostkey_طریقہ = hostkey_طریقہ_ssh_dss;
        اكسر;
#جے_انت
      حالة LIBSSH2_KNOWNHOST_کلید_RSA1:
        failf(انکڑے, "Found host key type RSA1 which is not supported");
        أرجع CURLE_SSH;
      افتراضي:
        failf(انکڑے, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_کلید_نقاب));
        أرجع CURLE_SSH;
      }

      infof(انکڑے, "SSH: set '%s' as hostkey type", hostkey_طریقہ);
      rc = libssh2_session_طریقہ_pref(sshc->ssh_session,
                                       LIBSSH2_طریقہ_HOSTKEY, hostkey_طریقہ);
      إذا(rc) {
        حرف *err_پیغام = عدم;
        صحيح errlen;
        libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, &errlen, 0);
        failf(انکڑے, "libssh2 method '%s' failed: %s", hostkey_طریقہ, err_پیغام);
        نتیجہ = libssh2_session_error_منزل_CURLE(rc);
      }
    }
    وإلا {
      infof(انکڑے, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(انکڑے, STRING_SSH_KNOWNHOSTS));
    }
  }

  أرجع نتیجہ;
}

ساكن CURLcode quote_error(بنية Curl_easy *انکڑے,
                            بنية ssh_conn *sshc)
{
  failf(انکڑے, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_quote(بنية Curl_easy *انکڑے,
                           بنية ssh_conn *sshc,
                           بنية SSHPROTO *sshp)
{
  ثابت حرف *cp;
  CURLcode نتیجہ = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ثابت حرف *cmd = sshc->quote_شے->انکڑے;
  sshc->acceptfail = FALSE;

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
                              sshp->رستہ);
    إذا(!tmp)
      أرجع CURLE_باہرلا_دا_حافظہ;
    Curl_debug(انکڑے, CURLINFO_سرخی_باہرلا, "PWD\n", 4);
    Curl_debug(انکڑے, CURLINFO_سرخی_وچ, tmp, لڑی_لمبائی(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_سرخی, tmp, لڑی_لمبائی(tmp));
    curlx_free(tmp);
    إذا(!نتیجہ)
      myssh_منزل(انکڑے, sshc, SSH_SFTP_اگلا_QUOTE);
    أرجع نتیجہ;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  إذا(!cp) {
    failf(انکڑے, "Syntax error command '%s', missing parameter", cmd);
    أرجع CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  إذا(نتیجہ) {
    إذا(نتیجہ != CURLE_باہرلا_دا_حافظہ)
      failf(انکڑے, "Syntax error: Bad first parameter to '%s'", cmd);
    أرجع نتیجہ;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  إذا(!لڑی_مقابلہ(cmd, "chgrp ", 6) ||
     !لڑی_مقابلہ(cmd, "chmod ", 6) ||
     !لڑی_مقابلہ(cmd, "chown ", 6) ||
     !لڑی_مقابلہ(cmd, "atime ", 6) ||
     !لڑی_مقابلہ(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(نتیجہ) {
      إذا(نتیجہ != CURLE_باہرلا_دا_حافظہ)
        failf(انکڑے, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      أرجع نتیجہ;
    }
    إذا(*cp)
      أرجع quote_error(انکڑے, sshc);

    memset(&sshp->quote_attrs, 0, حجم(LIBSSH2_SFTP_گن));
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE_STAT);
    أرجع نتیجہ;
  }
  إذا(!لڑی_مقابلہ(cmd, "ln ", 3) ||
     !لڑی_مقابلہ(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(نتیجہ) {
      إذا(نتیجہ != CURLE_باہرلا_دا_حافظہ)
        failf(انکڑے, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      أرجع نتیجہ;
    }
    إذا(*cp)
      أرجع quote_error(انکڑے, sshc);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE_SYMLINK);
    أرجع نتیجہ;
  }
  وإلا إذا(!لڑی_مقابلہ(cmd, "mkdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(انکڑے, sshc);
    /* create directory */
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE_MKDIR);
    أرجع نتیجہ;
  }
  وإلا إذا(!لڑی_مقابلہ(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(نتیجہ) {
      إذا(نتیجہ != CURLE_باہرلا_دا_حافظہ)
        failf(انکڑے, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      أرجع نتیجہ;
    }
    إذا(*cp)
      أرجع quote_error(انکڑے, sshc);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE_RENAME);
    أرجع نتیجہ;
  }
  وإلا إذا(!لڑی_مقابلہ(cmd, "rmdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(انکڑے, sshc);
    /* delete directory */
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE_RMDIR);
    أرجع نتیجہ;
  }
  وإلا إذا(!لڑی_مقابلہ(cmd, "rm ", 3)) {
    إذا(*cp)
      أرجع quote_error(انکڑے, sshc);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE_UNLINK);
    أرجع نتیجہ;
  }
  وإلا إذا(!لڑی_مقابلہ(cmd, "statvfs ", 8)) {
    إذا(*cp)
      أرجع quote_error(انکڑے, sshc);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE_STATVFS);
    أرجع نتیجہ;
  }

  failf(انکڑے, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_upload_init(بنية Curl_easy *انکڑے,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp,
                                 bool *blockp)
{
  بنانشانی لما flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  إذا(انکڑے->state.resume_from) {
    LIBSSH2_SFTP_گن attrs;
    إذا(انکڑے->state.resume_from < 0) {
      صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->رستہ,
                                    curlx_uztoui(لڑی_لمبائی(sshp->رستہ)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      إذا(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        أرجع CURLE_OK;
      }
      إذا(rc) {
        انکڑے->state.resume_from = 0;
      }
      وإلا {
        curl_off_t ناپ = attrs.filesize;
        إذا(ناپ < 0) {
          failf(انکڑے, "Bad file size (%" FMT_OFF_T ")", ناپ);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        انکڑے->state.resume_from = attrs.filesize;
      }
    }
  }

  إذا(انکڑے->سیٹ.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  وإلا إذا(انکڑے->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  وإلا {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->رستہ,
                         curlx_uztoui(لڑی_لمبائی(sshp->رستہ)),
                         flags, (لما)انکڑے->سیٹ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  إذا(!sshc->sftp_handle) {
    CURLcode نتیجہ;
    بنانشانی لما sftperr;
    صحيح rc = libssh2_session_آخری_errno(sshc->ssh_session);

    إذا(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      أرجع CURLE_OK;
    }

    إذا(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    وإلا
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    إذا(sshc->دوجا_Create_Dirs) {
      myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
      failf(انکڑے, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      أرجع sftp_libssh2_error_منزل_CURLE(sftperr);
    }
    إذا((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_رستہ) &&
       انکڑے->سیٹ.ftp_create_missing_dirs &&
       (لڑی_لمبائی(sshp->رستہ) > 1)) {
      /* try to create the path remotely */
      sshc->دوجا_Create_Dirs = 1;
      myssh_منزل(انکڑے, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      أرجع CURLE_OK;
    }
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    نتیجہ = sftp_libssh2_error_منزل_CURLE(sftperr);
    إذا(!نتیجہ) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      نتیجہ = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(انکڑے, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    أرجع نتیجہ;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  إذا(انکڑے->state.resume_from > 0 && !انکڑے->سیٹ.remote_append) {
    صحيح seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    إذا(انکڑے->سیٹ.seek_func) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_شروع(&guard, انکڑے, easy_seek_func);
      seekerr = انکڑے->سیٹ.seek_func(انکڑے->سیٹ.seek_client,
                                    انکڑے->state.resume_from, SEEK_SET);
      CURL_CBAPI_سرا(&guard);
    }

    إذا(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(انکڑے, "Could not seek stream");
        أرجع CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      افعل {
        بنية Curl_mapi_guard guard;
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (انکڑے->state.resume_from - passed >
           (curl_off_t)حجم(scratch)) ?
          حجم(scratch) : curlx_sotouz(انکڑے->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_شروع(&guard, انکڑے, easy_fread_func);
        actuallyread = انکڑے->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              انکڑے->state.وچ);
        CURL_CBAPI_سرا(&guard);

        passed += actuallyread;
        إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(انکڑے, "Failed to read data");
          أرجع CURLE_FTP_COULDNT_USE_REST;
        }
      } طالما(passed < انکڑے->state.resume_from);
    }

    /* now, decrease the size of the read */
    إذا(انکڑے->state.infilesize > 0) {
      انکڑے->state.infilesize -= انکڑے->state.resume_from;
      انکڑے->req.ناپ = انکڑے->state.infilesize;
      Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, انکڑے->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)انکڑے->state.resume_from);
  }
  إذا(انکڑے->state.infilesize > 0) {
    انکڑے->req.ناپ = انکڑے->state.infilesize;
    Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, انکڑے->state.infilesize);
  }
  /* upload data */
  Curl_xfer_تیاری_send(انکڑے, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  انکڑے->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(انکڑے);

  myssh_منزل(انکڑے, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن فراغ ssh_state_pkey_init(بنية Curl_easy *انکڑے, بنية ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  إذا((انکڑے->سیٹ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     لڑی_کھوج(sshc->authlist, "publickey")) {
    إذا(sshc->pub_کلید)
      infof(انکڑے, "SSH: trying public key file '%s'", sshc->pub_کلید);
    infof(انکڑے, "SSH: trying private key file '%s'", sshc->priv_کلید);
    myssh_منزل(انکڑے, sshc, SSH_AUTH_PKEY);
  }
  وإلا
    myssh_منزل(انکڑے, sshc, SSH_AUTH_PASS_INIT);
}

ساكن CURLcode sftp_quote_stat(بنية Curl_easy *انکڑے,
                                بنية ssh_conn *sshc,
                                بنية SSHPROTO *sshp,
                                bool *blockp)
{
  ثابت حرف *cmd = sshc->quote_شے->انکڑے;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  إذا(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  إذا(لڑی_مقابلہ(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(لڑی_لمبائی(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    إذا(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      أرجع CURLE_OK;
    }
    إذا(rc && !sshc->acceptfail) { /* get those attributes */
      بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
      failf(انکڑے, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      اذهب fail;
    }
  }

  /* Now set the new attributes... */
  إذا(!لڑی_مقابلہ(cmd, "chgrp", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t gid;
    إذا(!curlx_str_نمبر(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (بنانشانی لما)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وإلا إذا(!sshc->acceptfail) {
      failf(انکڑے, "Syntax error: chgrp gid not a number");
      اذهب fail;
    }
  }
  وإلا إذا(!لڑی_مقابلہ(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت حرف *p = sshc->quote_path1;
    /* permissions are octal */
    إذا(curlx_str_octal(&p, &perms, 07777)) {
      failf(انکڑے, "Syntax error: chmod permissions not a number");
      اذهب fail;
    }

    sshp->quote_attrs.permissions = (بنانشانی لما)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  وإلا إذا(!لڑی_مقابلہ(cmd, "chown", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t uid;
    إذا(!curlx_str_نمبر(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (بنانشانی لما)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وإلا إذا(!sshc->acceptfail) {
      failf(انکڑے, "Syntax error: chown uid not a number");
      اذهب fail;
    }
  }
  وإلا إذا(!لڑی_مقابلہ(cmd, "atime", 5) ||
          !لڑی_مقابلہ(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    إذا(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(انکڑے, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#إذا SIZEOF_ویلا_T > SIZEOF_LONG
    إذا(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(انکڑے, "date overflow");
      fail = TRUE;
    }
#جے_انت
    إذا(fail)
      اذهب fail;
    إذا(!لڑی_مقابلہ(cmd, "atime", 5))
      sshp->quote_attrs.atime = (بنانشانی لما)date;
    وإلا /* mtime */
      sshp->quote_attrs.mtime = (بنانشانی لما)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE_SETSTAT);
  أرجع CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_download_stat(بنية Curl_easy *انکڑے,
                                   بنية ssh_conn *sshc,
                                   بنية SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_گن attrs;
  صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->رستہ,
                                curlx_uztoui(لڑی_لمبائی(sshp->رستہ)),
                                LIBSSH2_SFTP_STAT, &attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    أرجع CURLE_OK;
  }
  إذا(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_ناپ) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    انکڑے->req.ناپ = -1;
    انکڑے->req.maxdownload = -1;
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  وإلا {
    curl_off_t ناپ = attrs.filesize;

    إذا(ناپ < 0) {
      failf(انکڑے, "Bad file size (%" FMT_OFF_T ")", ناپ);
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
    إذا(انکڑے->state.use_range) {
      curl_off_t from;
      CURLcode نتیجہ = Curl_ssh_range(انکڑے, انکڑے->state.range, ناپ,
                                       &from, &ناپ);
      إذا(نتیجہ)
        أرجع نتیجہ;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    انکڑے->req.ناپ = ناپ;
    انکڑے->req.maxdownload = ناپ;
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, ناپ);
  }

  /* We can resume if we can seek to the resume position */
  إذا(انکڑے->state.resume_from) {
    إذا(انکڑے->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      إذا((curl_off_t)attrs.filesize < -انکڑے->state.resume_from) {
        failf(انکڑے, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              انکڑے->state.resume_from, (curl_off_t)attrs.filesize);
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      انکڑے->state.resume_from += attrs.filesize;
    }
    وإلا {
      إذا((curl_off_t)attrs.filesize < انکڑے->state.resume_from) {
        failf(انکڑے, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              انکڑے->state.resume_from, (curl_off_t)attrs.filesize);
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    انکڑے->req.ناپ = attrs.filesize - انکڑے->state.resume_from;
    انکڑے->req.maxdownload = attrs.filesize - انکڑے->state.resume_from;
    Curl_pgrs_سیٹ_Download_ناپ(انکڑے, attrs.filesize - انکڑے->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)انکڑے->state.resume_from);
  }

  /* Setup the actual download */
  إذا(انکڑے->req.ناپ == 0) {
    /* no data to transfer */
    Curl_xfer_تیاری_nop(انکڑے);
    infof(انکڑے, "SSH: file already completely downloaded");
    myssh_منزل(انکڑے, sshc, SSH_STOP);
    أرجع CURLE_OK;
  }
  Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, انکڑے->req.ناپ);

  /* not set by Curl_xfer_setup to preserve io_flags */
  انکڑے->conn->send_idx = 0;

  myssh_منزل(انکڑے, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode sftp_readdir(بنية Curl_easy *انکڑے,
                             بنية ssh_conn *sshc,
                             بنية SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode نتیجہ = CURLE_OK;
  صحيح rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_رستہ_MAX,
                                   sshp->readdir_longentry, CURL_رستہ_MAX,
                                   &sshp->readdir_attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    أرجع نتیجہ;
  }
  إذا(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    إذا(انکڑے->سیٹ.list_only) {
      نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      إذا(!نتیجہ)
        نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY, "\n", 1);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    وإلا {
      نتیجہ = curlx_dyn_جوڑو(&sshp->readdir, sshp->readdir_longentry);

      إذا(!نتیجہ) {
        إذا((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          نتیجہ = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->رستہ,
                                  sshp->readdir_filename);
          myssh_منزل(انکڑے, sshc, SSH_SFTP_READDIR_LINK);
        }
        وإلا {
          myssh_منزل(انکڑے, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      أرجع نتیجہ;
    }
  }
  وإلا إذا(!rc) {
    myssh_منزل(انکڑے, sshc, SSH_SFTP_READDIR_پورا);
  }
  وإلا {
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    نتیجہ = sftperr ? sftp_libssh2_error_منزل_CURLE(sftperr) : CURLE_SSH;
    failf(انکڑے, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_آخری_errno(sshc->ssh_session));
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
  }
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_init(بنية Curl_easy *انکڑے,
                               بنية ssh_conn *sshc)
{
  CURLcode نتیجہ;
  sshc->دوجا_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_سیٹ_blocking(sshc->ssh_session, 0);

  نتیجہ = ssh_force_knownhost_کلید_type(انکڑے, sshc);
  إذا(نتیجہ)
    myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
  وإلا
    myssh_منزل(انکڑے, sshc, SSH_S_STARTUP);
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_startup(بنية Curl_easy *انکڑے,
                                  بنية ssh_conn *sshc)
{
  بنية connectdata *conn = انکڑے->conn;
  صحيح rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc) {
    حرف *err_پیغام = عدم;
    (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    failf(انکڑے, "Failure establishing ssh session: %d, %s", rc, err_پیغام);

    myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
    أرجع CURLE_FAILED_INIT;
  }

  myssh_منزل(انکڑے, sshc, SSH_HOSTKEY);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_hostkey(بنية Curl_easy *انکڑے,
                                  بنية ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode نتیجہ = ssh_check_fingerprint(انکڑے, sshc);
  إذا(!نتیجہ)
    myssh_منزل(انکڑے, sshc, SSH_AUTHLIST);
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_authlist(بنية Curl_easy *انکڑے,
                                   بنية ssh_conn *sshc)
{
  /*
   * Figure out authentication methods
   * NB: As soon as we have provided a username to an openssh server we
   * must never change it later. Thus, always specify the correct username
   * here, even though the libssh2 docs kind of indicate that it should be
   * possible to get a 'generic' list (not user-specific) of authentication
   * methods, presumably with a blank username. That does not work in my
   * experience.
   * Therefore always specify it here.
   */
  بنية connectdata *conn = انکڑے->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(لڑی_لمبائی(user)));

  إذا(!sshc->authlist) {
    صحيح rc;
    إذا(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(انکڑے, "SSH: user accepted with no authentication");
      myssh_منزل(انکڑے, sshc, SSH_AUTH_پورا);
      أرجع CURLE_OK;
    }
    rc = libssh2_session_آخری_errno(sshc->ssh_session);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
    أرجع libssh2_session_error_منزل_CURLE(rc);
  }
  infof(انکڑے, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_منزل(انکڑے, sshc, SSH_AUTH_PKEY_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pkey(بنية Curl_easy *انکڑے,
                                    بنية ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  بنية connectdata *conn = انکڑے->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحيح rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(لڑی_لمبائی(user)),
    sshc->pub_کلید, sshc->priv_کلید, sshc->passphrase ? sshc->passphrase : "");
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(انکڑے, "SSH: authenticated via publickey");
    myssh_منزل(انکڑے, sshc, SSH_AUTH_پورا);
  }
  وإلا {
    حرف *err_پیغام = عدم;
    حرف unknown[] = "Reason unknown (-1)";
    إذا(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_پیغام = unknown;
    }
    وإلا {
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    }
    infof(انکڑے, "SSH: publickey authentication denied: %s", err_پیغام);
    myssh_منزل(انکڑے, sshc, SSH_AUTH_PASS_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pass_init(بنية Curl_easy *انکڑے,
                                         بنية ssh_conn *sshc)
{
  إذا((انکڑے->سیٹ.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     لڑی_کھوج(sshc->authlist, "password")) {
    myssh_منزل(انکڑے, sshc, SSH_AUTH_PASS);
  }
  وإلا {
    myssh_منزل(انکڑے, sshc, SSH_AUTH_HOST_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pass(بنية Curl_easy *انکڑے,
                                    بنية ssh_conn *sshc)
{
  بنية connectdata *conn = انکڑے->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
  صحيح rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(لڑی_لمبائی(user)),
                                 passwd,
                                 curlx_uztoui(لڑی_لمبائی(passwd)),
                                 عدم);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    أرجع CURLE_AGAIN;
  }
  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(انکڑے, "SSH: initialized password authentication");
    myssh_منزل(انکڑے, sshc, SSH_AUTH_پورا);
  }
  وإلا {
    myssh_منزل(انکڑے, sshc, SSH_AUTH_HOST_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_host_init(بنية Curl_easy *انکڑے,
                                         بنية ssh_conn *sshc)
{
  إذا((انکڑے->سیٹ.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     لڑی_کھوج(sshc->authlist, "hostbased")) {
    myssh_منزل(انکڑے, sshc, SSH_AUTH_HOST);
  }
  وإلا {
    myssh_منزل(انکڑے, sshc, SSH_AUTH_AGENT_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent_init(بنية Curl_easy *انکڑے,
                                          بنية ssh_conn *sshc)
{
  صحيح rc = 0;
  إذا((انکڑے->سیٹ.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     لڑی_کھوج(sshc->authlist, "publickey")) {

    infof(انکڑے, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    إذا(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      إذا(!sshc->ssh_agent) {
        infof(انکڑے, "SSH: could not create agent object");

        myssh_منزل(انکڑے, sshc, SSH_AUTH_کلید_INIT);
        أرجع CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc < 0) {
      infof(انکڑے, "SSH: failure connecting to agent");
      myssh_منزل(انکڑے, sshc, SSH_AUTH_کلید_INIT);
    }
    وإلا {
      myssh_منزل(انکڑے, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  وإلا
    myssh_منزل(انکڑے, sshc, SSH_AUTH_کلید_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent_list(بنية Curl_easy *انکڑے,
                                          بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_agent_list_identities(sshc->ssh_agent);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  إذا(rc < 0) {
    infof(انکڑے, "SSH: failure requesting identities to agent");
    myssh_منزل(انکڑے, sshc, SSH_AUTH_کلید_INIT);
  }
  وإلا {
    myssh_منزل(انکڑے, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_پچھان_تت = عدم;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent(بنية Curl_easy *انکڑے,
                                     بنية ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  صحيح rc = libssh2_agent_get_پچھان_تت(sshc->ssh_agent,
                                      &sshc->sshagent_پچھان_تت,
                                      sshc->sshagent_prev_پچھان_تت);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    CURL_TRC_SSH(انکڑے, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(انکڑے->conn->creds),
                 sshc->sshagent_پچھان_تت->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(انکڑے->conn->creds),
                                sshc->sshagent_پچھان_تت);

    إذا(rc < 0) {
      إذا(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_پچھان_تت = sshc->sshagent_پچھان_تت;
        أرجع CURLE_OK;
      }
      أرجع CURLE_AGAIN;
    }
  }

  إذا(rc < 0)
    infof(انکڑے, "SSH: failure requesting identities to agent");
  وإلا إذا(rc == 1)
    infof(انکڑے, "SSH: no agent identity would match");

  إذا(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(انکڑے, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(انکڑے->conn->creds),
          sshc->sshagent_پچھان_تت->comment);
    myssh_منزل(انکڑے, sshc, SSH_AUTH_پورا);
  }
  وإلا {
    myssh_منزل(انکڑے, sshc, SSH_AUTH_کلید_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_کلید_init(بنية Curl_easy *انکڑے,
                                        بنية ssh_conn *sshc)
{
  إذا((انکڑے->سیٹ.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     لڑی_کھوج(sshc->authlist, "keyboard-interactive")) {
    myssh_منزل(انکڑے, sshc, SSH_AUTH_کلید);
  }
  وإلا {
    myssh_منزل(انکڑے, sshc, SSH_AUTH_پورا);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_کلید(بنية Curl_easy *انکڑے,
                                   بنية ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  بنية connectdata *conn = انکڑے->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحيح rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(لڑی_لمبائی(user)),
                                             &kbd_callback);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(انکڑے, "SSH: initialized keyboard interactive authentication");
    myssh_منزل(انکڑے, sshc, SSH_AUTH_پورا);
    أرجع CURLE_OK;
  }
  أرجع CURLE_LOGIN_DENIED;
}

ساكن CURLcode ssh_state_auth_پورا(بنية Curl_easy *انکڑے,
                                    بنية ssh_conn *sshc)
{
  بنية connectdata *conn = انکڑے->conn;
  إذا(!sshc->authed) {
    failf(انکڑے, "Authentication failure");
    myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
    أرجع CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(انکڑے, "SSH: authentication complete");

  Curl_pgrs_ویلا(انکڑے, TIMER_APPCONNECT); /* SSH is connected */

  انکڑے->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  إذا(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_منزل(انکڑے, sshc, SSH_SFTP_INIT);
    أرجع CURLE_OK;
  }
  infof(انکڑے, "SSH: connection established");
  myssh_منزل(انکڑے, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_init(بنية Curl_easy *انکڑے,
                                    بنية ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  إذا(!sshc->sftp_session) {
    حرف *err_پیغام = عدم;
    إذا(libssh2_session_آخری_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    failf(انکڑے, "Failure initializing sftp session: %s", err_پیغام);
    myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
    أرجع CURLE_FAILED_INIT;
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_REALPATH);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_realpath(بنية Curl_easy *انکڑے,
                                        بنية ssh_conn *sshc,
                                        بنية SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_رستہ_MAX,
                               LIBSSH2_SFTP_REALPATH);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  myssh_منزل(انکڑے, sshc, SSH_STOP);
  إذا(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    إذا(!sshc->homedir)
      أرجع CURLE_باہرلا_دا_حافظہ;
    curlx_free(انکڑے->state.most_recent_ftp_entrypath);
    انکڑے->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    إذا(!انکڑے->state.most_recent_ftp_entrypath)
      أرجع CURLE_باہرلا_دا_حافظہ;
  }
  وإلا {
    /* Return the error type */
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    CURLcode نتیجہ;
    إذا(sftperr)
      نتیجہ = sftp_libssh2_error_منزل_CURLE(sftperr);
    وإلا
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      نتیجہ = CURLE_SSH;
    CURL_TRC_SSH(انکڑے, "error = %lu makes libcurl = %d", sftperr, (صحيح)نتیجہ);
    أرجع نتیجہ;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(انکڑے, "CONNECT phase done");
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_init(بنية Curl_easy *انکڑے,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  CURLcode نتیجہ;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  نتیجہ = Curl_getworkingpath(انکڑے, sshc->homedir, &sshp->رستہ);
  إذا(نتیجہ) {
    myssh_منزل(انکڑے, sshc, SSH_STOP);
    أرجع نتیجہ;
  }

  إذا(انکڑے->سیٹ.quote) {
    infof(انکڑے, "SSH: sending quote commands");
    sshc->quote_شے = انکڑے->سیٹ.quote;
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_منزل(انکڑے, sshc, SSH_SFTP_GETINFO);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_postquote_init(بنية Curl_easy *انکڑے,
                                              بنية ssh_conn *sshc)
{
  إذا(انکڑے->سیٹ.postquote) {
    infof(انکڑے, "SSH: sending quote commands");
    sshc->quote_شے = انکڑے->سیٹ.postquote;
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_منزل(انکڑے, sshc, SSH_STOP);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote(بنية Curl_easy *انکڑے,
                                     بنية ssh_conn *sshc,
                                     بنية SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode نتیجہ;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  نتیجہ = sftp_quote(انکڑے, sshc, sshp);
  إذا(نتیجہ) {
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_sftp_اگلا_quote(بنية Curl_easy *انکڑے,
                                          بنية ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_شے = sshc->quote_شے->اگلا;

  إذا(sshc->quote_شے) {
    myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    إذا(sshc->nextstate != SSH_NO_STATE) {
      myssh_منزل(انکڑے, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    وإلا {
      myssh_منزل(انکڑے, sshc, SSH_SFTP_GETINFO);
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_stat(بنية Curl_easy *انکڑے,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode نتیجہ;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  نتیجہ = sftp_quote_stat(انکڑے, sshc, sshp, blockp);
  إذا(نتیجہ) {
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_sftp_quote_setstat(بنية Curl_easy *انکڑے,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp)
{
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(لڑی_لمبائی(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(انکڑے, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_اگلا_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_symlink(بنية Curl_easy *انکڑے,
                                             بنية ssh_conn *sshc)
{
  صحيح rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(لڑی_لمبائی(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(انکڑے, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_اگلا_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_mkdir(بنية Curl_easy *انکڑے,
                                           بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)),
                                 (لما)انکڑے->سیٹ.new_directory_perms);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(انکڑے, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_اگلا_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_rename(بنية Curl_easy *انکڑے,
                                            بنية ssh_conn *sshc)
{
  صحيح rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(لڑی_لمبائی(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(انکڑے, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_اگلا_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_rmdir(بنية Curl_easy *انکڑے,
                                           بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)));
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(انکڑے, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_اگلا_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_unlink(بنية Curl_easy *انکڑے,
                                            بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)));
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(انکڑے, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_اگلا_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_statvfs(بنية Curl_easy *انکڑے,
                                             بنية ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  صحيح rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)),
                                &statvfs);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(انکڑے, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  وإلا إذا(rc == 0) {
#جے_تعریف_شدہ _MSC_VER
#تعریف_کرو CURL_LIBSSH2_VFS_ناپ_نقاب "I64u"
#وإلا
#تعریف_کرو CURL_LIBSSH2_VFS_ناپ_نقاب "llu"
#جے_انت
    CURLcode نتیجہ;
    حرف *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_ناپ_نقاب "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_بلاکاں, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    إذا(!tmp) {
      myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع CURLE_باہرلا_دا_حافظہ;
    }

    نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_سرخی, tmp, لڑی_لمبائی(tmp));
    curlx_free(tmp);
    إذا(نتیجہ) {
      myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع نتیجہ;
    }
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_اگلا_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_create_dirs_mkdir(بنية Curl_easy *انکڑے,
                                                 بنية ssh_conn *sshc,
                                                 بنية SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->رستہ,
                             curlx_uztoui(لڑی_لمبائی(sshp->رستہ)),
                             (لما)انکڑے->سیٹ.new_directory_perms);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  إذا(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    بنانشانی لما sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    إذا((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
      أرجع sftp_libssh2_error_منزل_CURLE(sftperr);
    }
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_CREATE_DIRS);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir_init(بنية Curl_easy *انکڑے,
                                            بنية ssh_conn *sshc,
                                            بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  Curl_pgrs_سیٹ_Download_ناپ(انکڑے, -1);
  إذا(انکڑے->req.no_body) {
    myssh_منزل(انکڑے, sshc, SSH_STOP);
    أرجع CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->رستہ,
                         curlx_uztoui(لڑی_لمبائی(sshp->رستہ)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  إذا(!sshc->sftp_handle) {
    بنانشانی لما sftperr;
    إذا(libssh2_session_آخری_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(انکڑے, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    أرجع sftp_libssh2_error_منزل_CURLE(sftperr);
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_READDIR);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir_link(بنية Curl_easy *انکڑے,
                                            بنية ssh_conn *sshc,
                                            بنية SSHPROTO *sshp)
{
  CURLcode نتیجہ;
  صحيح rc;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (بنانشانی صحيح)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_رستہ_MAX, LIBSSH2_SFTP_READLINK);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  إذا(rc < 0)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* append filename and extra output */
  نتیجہ = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  إذا(نتیجہ)
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
  وإلا
    myssh_منزل(انکڑے, sshc, SSH_SFTP_READDIR_BOTTOM);
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_scp_download_init(بنية Curl_easy *انکڑے,
                                            بنية ssh_conn *sshc,
                                            بنية SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, حجم(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->رستہ, &sb);
  إذا(!sshc->ssh_channel) {
    صحيح ssh_err;
    حرف *err_پیغام = عدم;

    إذا(libssh2_session_آخری_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    ssh_err = libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    failf(انکڑے, "%s", err_پیغام);
    myssh_منزل(انکڑے, sshc, SSH_SCP_CHANNEL_FREE);
    أرجع libssh2_session_error_منزل_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_ناپ;
  انکڑے->req.maxdownload = (curl_off_t)sb.st_ناپ;
  Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  انکڑے->conn->send_idx = 0;

  myssh_منزل(انکڑے, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_close(بنية Curl_easy *انکڑے,
                                     بنية ssh_conn *sshc,
                                     بنية SSHPROTO *sshp)
{
  صحيح rc = 0;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  إذا(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to close libssh2 file: %d %s", rc, err_پیغام);
    }
    sshc->sftp_handle = عدم;
  }

  curlx_safefree(sshp->رستہ);

  CURL_TRC_SSH(انکڑے, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  إذا(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_منزل(انکڑے, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  وإلا
    myssh_منزل(انکڑے, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_shutdown(بنية Curl_easy *انکڑے,
                                        بنية ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  صحيح rc = 0;
  إذا(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to close libssh2 file: %d %s", rc, err_پیغام);
    }
    sshc->sftp_handle = عدم;
  }
  إذا(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      infof(انکڑے, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = عدم;
  }

  curlx_safefree(sshc->homedir);

  myssh_منزل(انکڑے, sshc, SSH_SESSION_DISCONNECT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_download_init(بنية Curl_easy *انکڑے,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->رستہ,
                         curlx_uztoui(لڑی_لمبائی(sshp->رستہ)),
                         LIBSSH2_FXF_READ, (لما)انکڑے->سیٹ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  إذا(!sshc->sftp_handle) {
    بنانشانی لما sftperr;
    إذا(libssh2_session_آخری_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      أرجع CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(انکڑے, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    أرجع sftp_libssh2_error_منزل_CURLE(sftperr);
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_DOWNLOAD_STAT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_upload_init(بنية Curl_easy *انکڑے,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->رستہ,
                       (صحيح)انکڑے->سیٹ.new_file_perms,
                       (libssh2_int64_t)انکڑے->state.infilesize, 0, 0);
  إذا(!sshc->ssh_channel) {
    صحيح ssh_err;
    حرف *err_پیغام = عدم;
    CURLcode نتیجہ;
    إذا(libssh2_session_آخری_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    ssh_err = libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    failf(انکڑے, "%s", err_پیغام);
    myssh_منزل(انکڑے, sshc, SSH_SCP_CHANNEL_FREE);
    نتیجہ = libssh2_session_error_منزل_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    إذا(نتیجہ == CURLE_SSH ||
       نتیجہ == CURLE_REMOTE_FILE_NOT_لبھیا)
      نتیجہ = CURLE_UPLOAD_FAILED;
    أرجع نتیجہ;
  }

  /* upload data */
  انکڑے->req.ناپ = انکڑے->state.infilesize;
  Curl_pgrs_سیٹ_Upload_ناپ(انکڑے, انکڑے->state.infilesize);
  Curl_xfer_تیاری_send(انکڑے, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  انکڑے->conn->recv_idx = FIRSTSOCKET;

  myssh_منزل(انکڑے, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_session_disconnect(بنية Curl_easy *انکڑے,
                                             بنية ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  صحيح rc = 0;
  إذا(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیغام);
    }
    sshc->ssh_channel = عدم;
  }

  إذا(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to disconnect libssh2 session: %d %s", rc, err_پیغام);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_منزل(انکڑے, sshc, SSH_SESSION_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode sshc_cleanup(بنية ssh_conn *sshc, بنية Curl_easy *انکڑے,
                             bool بلاک)
{
  صحيح rc;

  إذا(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = عدم;
  }

  إذا(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    إذا((rc < 0) && انکڑے) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_پیغام);
    }
    إذا(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = عدم;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_پچھان_تت = عدم;
    sshc->sshagent_prev_پچھان_تت = عدم;
  }

  إذا(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    إذا((rc < 0) && انکڑے) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to close libssh2 file: %d %s", rc, err_پیغام);
    }
    إذا(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->sftp_handle = عدم;
  }

  إذا(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    إذا((rc < 0) && انکڑے) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیغام);
    }
    إذا(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->ssh_channel = عدم;
  }

  إذا(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    إذا((rc < 0) && انکڑے) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_پیغام);
    }
    إذا(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->sftp_session = عدم;
  }

  إذا(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    إذا((rc < 0) && انکڑے) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to free libssh2 session: %d %s", rc, err_پیغام);
    }
    إذا(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->ssh_session = عدم;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_کلید);
  curlx_safefree(sshc->priv_کلید);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_getinfo(بنية Curl_easy *انکڑے,
                                       بنية ssh_conn *sshc)
{
  إذا(انکڑے->سیٹ.get_filetime)
    myssh_منزل(انکڑے, sshc, SSH_SFTP_FILETIME);
  وإلا
    myssh_منزل(انکڑے, sshc, SSH_SFTP_TRANS_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_filetime(بنية Curl_easy *انکڑے,
                                        بنية ssh_conn *sshc,
                                        بنية SSHPROTO *sshp)
{
  LIBSSH2_SFTP_گن attrs;
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->رستہ,
                            curlx_uztoui(لڑی_لمبائی(sshp->رستہ)),
                            LIBSSH2_SFTP_STAT, &attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0)
    انکڑے->info.filetime = (time_t)attrs.mtime;

  myssh_منزل(انکڑے, sshc, SSH_SFTP_TRANS_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_trans_init(بنية Curl_easy *انکڑے,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  إذا(انکڑے->state.upload)
    myssh_منزل(انکڑے, sshc, SSH_SFTP_UPLOAD_INIT);
  وإلا إذا(sshp) {
    size_t plen = لڑی_لمبائی(sshp->رستہ);
    إذا(plen && (sshp->رستہ[plen - 1] == '/'))
      myssh_منزل(انکڑے, sshc, SSH_SFTP_READDIR_INIT);
    وإلا
      myssh_منزل(انکڑے, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  وإلا
    أرجع CURLE_FAILED_INIT;
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_upload_init(بنية Curl_easy *انکڑے,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp,
                                           bool *بلاک)
{
  CURLcode نتیجہ;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  نتیجہ = sftp_upload_init(انکڑے, sshc, sshp, بلاک);
  إذا(نتیجہ) {
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_sftp_create_dirs_init(بنية Curl_easy *انکڑے,
                                                بنية ssh_conn *sshc,
                                                بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  إذا(لڑی_لمبائی(sshp->رستہ) > 1) {
    sshc->slash_pos = sshp->رستہ + 1; /* ignore the leading '/' */
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CREATE_DIRS);
  }
  وإلا
    myssh_منزل(انکڑے, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_create_dirs(بنية Curl_easy *انکڑے,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  إذا(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(انکڑے, "SFTP: creating directory '%s'", sshp->رستہ);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    أرجع CURLE_OK;
  }
  myssh_منزل(انکڑے, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir(بنية Curl_easy *انکڑے,
                                       بنية ssh_conn *sshc,
                                       بنية SSHPROTO *sshp,
                                       bool *بلاک)
{
  CURLcode نتیجہ;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  نتیجہ = sftp_readdir(انکڑے, sshc, sshp, بلاک);
  إذا(نتیجہ)
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_sftp_readdir_bottom(بنية Curl_easy *انکڑے,
                                              بنية ssh_conn *sshc,
                                              بنية SSHPROTO *sshp)
{
  CURLcode نتیجہ;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  نتیجہ = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  إذا(!نتیجہ)
    نتیجہ = Curl_client_write(انکڑے, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  إذا(نتیجہ) {
    curlx_dyn_free(&sshp->readdir);
    myssh_منزل(انکڑے, sshc, SSH_STOP);
  }
  وإلا {
    curlx_dyn_reset(&sshp->readdir);
    myssh_منزل(انکڑے, sshc, SSH_SFTP_READDIR);
  }
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_sftp_readdir_پورا(بنية Curl_easy *انکڑے,
                                            بنية ssh_conn *sshc)
{
  إذا(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  sshc->sftp_handle = عدم;

  /* no data to transfer */
  Curl_xfer_تیاری_nop(انکڑے);
  myssh_منزل(انکڑے, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_download_stat(بنية Curl_easy *انکڑے,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp,
                                             bool *بلاک)
{
  CURLcode نتیجہ;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  نتیجہ = sftp_download_stat(انکڑے, sshc, sshp, بلاک);
  إذا(نتیجہ) {
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع نتیجہ;
}

ساكن CURLcode ssh_state_scp_trans_init(بنية Curl_easy *انکڑے,
                                         بنية ssh_conn *sshc,
                                         بنية SSHPROTO *sshp)
{
  CURLcode نتیجہ;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  نتیجہ = Curl_getworkingpath(انکڑے, sshc->homedir,
                               &sshp->رستہ);
  إذا(نتیجہ) {
    myssh_منزل(انکڑے, sshc, SSH_STOP);
    أرجع نتیجہ;
  }

  إذا(انکڑے->state.upload) {
    إذا(انکڑے->state.infilesize < 0) {
      failf(انکڑے, "SCP requires a known file size for upload");
      myssh_منزل(انکڑے, sshc, SSH_SCP_CHANNEL_FREE);
      أرجع CURLE_UPLOAD_FAILED;
    }
    myssh_منزل(انکڑے, sshc, SSH_SCP_UPLOAD_INIT);
  }
  وإلا
    myssh_منزل(انکڑے, sshc, SSH_SCP_DOWNLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_پورا(بنية Curl_easy *انکڑے,
                                   بنية ssh_conn *sshc)
{
  إذا(انکڑے->state.upload)
    myssh_منزل(انکڑے, sshc, SSH_SCP_SEND_EOF);
  وإلا
    myssh_منزل(انکڑے, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_send_eof(بنية Curl_easy *انکڑے,
                                       بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_send_eof(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session,
                                       &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_پیغام);
    }
  }
  myssh_منزل(انکڑے, sshc, SSH_SCP_WAIT_EOF);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_wait_eof(بنية Curl_easy *انکڑے,
                                       بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session,
                                       &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to get channel EOF: %d %s", rc, err_پیغام);
    }
  }
  myssh_منزل(انکڑے, sshc, SSH_SCP_WAIT_CLOSE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_wait_close(بنية Curl_easy *انکڑے,
                                         بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session,
                                       &err_پیغام, عدم, 0);
      infof(انکڑے, "Channel failed to close: %d %s", rc, err_پیغام);
    }
  }
  myssh_منزل(انکڑے, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_channel_free(
  بنية Curl_easy *انکڑے,
  بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_free(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc < 0) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_آخری_error(sshc->ssh_session,
                                       &err_پیغام, عدم, 0);
      infof(انکڑے, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیغام);
    }
    sshc->ssh_channel = عدم;
  }
  CURL_TRC_SSH(انکڑے, "SCP DONE phase complete");
  myssh_منزل(انکڑے, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_session_free(بنية Curl_easy *انکڑے,
                                       بنية ssh_conn *sshc)
{
  CURLcode نتیجہ = sshc_cleanup(sshc, انکڑے, FALSE);
  بنية connectdata *conn = انکڑے->conn;
  إذا(نتیجہ)
    أرجع نتیجہ;
  memset(sshc, 0, حجم(بنية ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_منزل(انکڑے, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ساكن CURLcode ssh_statemachine(بنية Curl_easy *انکڑے,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp,
                                 bool *بلاک)
{
  CURLcode نتیجہ = CURLE_OK;
  *بلاک = 0; /* we are not blocking by default */

  افعل {
    بدل(sshc->state) {
    حالة SSH_INIT:
      نتیجہ = ssh_state_init(انکڑے, sshc);
      إذا(نتیجہ)
        اكسر;
      FALLTHROUGH();

    حالة SSH_S_STARTUP:
      نتیجہ = ssh_state_startup(انکڑے, sshc);
      إذا(نتیجہ)
        اكسر;
      FALLTHROUGH();

    حالة SSH_HOSTKEY:
      نتیجہ = ssh_state_hostkey(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTHLIST:
      نتیجہ = ssh_state_authlist(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_PKEY:
      نتیجہ = ssh_state_auth_pkey(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_PASS_INIT:
      نتیجہ = ssh_state_auth_pass_init(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_PASS:
      نتیجہ = ssh_state_auth_pass(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_HOST_INIT:
      نتیجہ = ssh_state_auth_host_init(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_HOST:
      myssh_منزل(انکڑے, sshc, SSH_AUTH_AGENT_INIT);
      اكسر;

    حالة SSH_AUTH_AGENT_INIT:
      نتیجہ = ssh_state_auth_agent_init(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_AGENT_LIST:
      نتیجہ = ssh_state_auth_agent_list(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_AGENT:
      نتیجہ = ssh_state_auth_agent(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_کلید_INIT:
      نتیجہ = ssh_state_auth_کلید_init(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_کلید:
      نتیجہ = ssh_state_auth_کلید(انکڑے, sshc);
      اكسر;

    حالة SSH_AUTH_پورا:
      نتیجہ = ssh_state_auth_پورا(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_INIT:
      نتیجہ = ssh_state_sftp_init(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_REALPATH:
      نتیجہ = ssh_state_sftp_realpath(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_QUOTE_INIT:
      نتیجہ = ssh_state_sftp_quote_init(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_POSTQUOTE_INIT:
      نتیجہ = ssh_state_sftp_postquote_init(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE:
      نتیجہ = ssh_state_sftp_quote(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_اگلا_QUOTE:
      نتیجہ = ssh_state_sftp_اگلا_quote(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_STAT:
      نتیجہ = ssh_state_sftp_quote_stat(انکڑے, sshc, sshp, بلاک);
      اكسر;

    حالة SSH_SFTP_QUOTE_SETSTAT:
      نتیجہ = ssh_state_sftp_quote_setstat(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_QUOTE_SYMLINK:
      نتیجہ = ssh_state_sftp_quote_symlink(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_MKDIR:
      نتیجہ = ssh_state_sftp_quote_mkdir(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_RENAME:
      نتیجہ = ssh_state_sftp_quote_rename(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_RMDIR:
      نتیجہ = ssh_state_sftp_quote_rmdir(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_UNLINK:
      نتیجہ = ssh_state_sftp_quote_unlink(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_STATVFS:
      نتیجہ = ssh_state_sftp_quote_statvfs(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_GETINFO:
      نتیجہ = ssh_state_sftp_getinfo(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_FILETIME:
      نتیجہ = ssh_state_sftp_filetime(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_TRANS_INIT:
      نتیجہ = ssh_state_sftp_trans_init(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_UPLOAD_INIT:
      نتیجہ = ssh_state_sftp_upload_init(انکڑے, sshc, sshp, بلاک);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS_INIT:
      نتیجہ = ssh_state_sftp_create_dirs_init(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS:
      نتیجہ = ssh_state_sftp_create_dirs(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS_MKDIR:
      نتیجہ = ssh_state_sftp_create_dirs_mkdir(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_INIT:
      نتیجہ = ssh_state_sftp_readdir_init(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR:
      نتیجہ = ssh_state_sftp_readdir(انکڑے, sshc, sshp, بلاک);
      اكسر;

    حالة SSH_SFTP_READDIR_LINK:
      نتیجہ = ssh_state_sftp_readdir_link(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_BOTTOM:
      نتیجہ = ssh_state_sftp_readdir_bottom(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_پورا:
      نتیجہ = ssh_state_sftp_readdir_پورا(انکڑے, sshc);
      اكسر;

    حالة SSH_SFTP_DOWNLOAD_INIT:
      نتیجہ = ssh_state_sftp_download_init(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_DOWNLOAD_STAT:
      نتیجہ = ssh_state_sftp_download_stat(انکڑے, sshc, sshp, بلاک);
      اكسر;

    حالة SSH_SFTP_CLOSE:
      نتیجہ = ssh_state_sftp_close(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_SHUTDOWN:
      نتیجہ = ssh_state_sftp_shutdown(انکڑے, sshc);
      اكسر;

    حالة SSH_SCP_TRANS_INIT:
      نتیجہ = ssh_state_scp_trans_init(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SCP_UPLOAD_INIT:
      نتیجہ = ssh_state_scp_upload_init(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SCP_DOWNLOAD_INIT:
      نتیجہ = ssh_state_scp_download_init(انکڑے, sshc, sshp);
      اكسر;

    حالة SSH_SCP_پورا:
      نتیجہ = ssh_state_scp_پورا(انکڑے, sshc);
      اكسر;

    حالة SSH_SCP_SEND_EOF:
      نتیجہ = ssh_state_scp_send_eof(انکڑے, sshc);
      اكسر;

    حالة SSH_SCP_WAIT_EOF:
      نتیجہ = ssh_state_scp_wait_eof(انکڑے, sshc);
      اكسر;

    حالة SSH_SCP_WAIT_CLOSE:
      نتیجہ = ssh_state_scp_wait_close(انکڑے, sshc);
      اكسر;

    حالة SSH_SCP_CHANNEL_FREE:
      نتیجہ = ssh_state_scp_channel_free(انکڑے, sshc);
      اكسر;

    حالة SSH_SESSION_DISCONNECT:
      نتیجہ = ssh_state_session_disconnect(انکڑے, sshc);
      اكسر;

    حالة SSH_SESSION_FREE:
      نتیجہ = ssh_state_session_free(انکڑے, sshc);
      اكسر;

    حالة SSH_QUIT:
    افتراضي:
      /* internal error */
      myssh_منزل(انکڑے, sshc, SSH_STOP);
      اكسر;
    }

  } طالما(!نتیجہ && (sshc->state != SSH_STOP) && !*بلاک);

  إذا(نتیجہ == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *بلاک = TRUE;
    نتیجہ = CURLE_OK;
  }
  CURL_TRC_SSH(انکڑے, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحيح)نتیجہ, *بلاک);

  أرجع نتیجہ;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ساكن فراغ ssh_block2waitfor(بنية Curl_easy *انکڑے,
                              بنية ssh_conn *sshc,
                              bool بلاک)
{
  صحيح dir = 0;
  (فراغ)انکڑے;
  إذا(بلاک) {
    dir = libssh2_session_بلاک_directions(sshc->ssh_session);
    إذا(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_بلاک_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_بلاک_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  إذا(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode ssh_multi_statemach(بنية Curl_easy *انکڑے, bool *پورا)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(انکڑے, CURL_META_SSH_EASY);
  CURLcode نتیجہ = CURLE_OK;
  bool بلاک; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;

  افعل {
    نتیجہ = ssh_statemachine(انکڑے, sshc, sshp, &بلاک);
    *پورا = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } طالما(!نتیجہ && !*پورا && !بلاک);
  ssh_block2waitfor(انکڑے, sshc, بلاک);

  أرجع نتیجہ;
}

ساكن CURLcode ssh_بلاک_statemach(بنية Curl_easy *انکڑے,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية curltime شروع = *Curl_pgrs_now(انکڑے);

  طالما((sshc->state != SSH_STOP) && !نتیجہ) {
    bool بلاک;
    timediff_t بایاں_ms = 1000;

    نتیجہ = ssh_statemachine(انکڑے, sshc, sshp, &بلاک);
    إذا(نتیجہ)
      اكسر;

    إذا(!disconnect) {
      نتیجہ = Curl_pgrsCheck(انکڑے);
      إذا(نتیجہ)
        اكسر;

      بایاں_ms = Curl_timeleft_ms(انکڑے);
      إذا(بایاں_ms < 0) {
        failf(انکڑے, "Operation timed out");
        أرجع CURLE_OPERATION_TIMEDOUT;
      }
    }
    وإلا إذا(curlx_ptimediff_ms(Curl_pgrs_now(انکڑے), &شروع) > 1000) {
      /* disconnect timeout */
      failf(انکڑے, "Disconnect timed out");
      نتیجہ = CURLE_OK;
      اكسر;
    }

    إذا(بلاک) {
      صحيح dir = libssh2_session_بلاک_directions(sshc->ssh_session);
      curl_socket_t sock = انکڑے->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      إذا(LIBSSH2_SESSION_بلاک_INBOUND & dir)
        fd_read = sock;
      إذا(LIBSSH2_SESSION_بلاک_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (فراغ)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!بایاں_ms || بایاں_ms > 1000) ? 1000 : بایاں_ms);
    }
  }

  أرجع نتیجہ;
}

ساكن فراغ myssh_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية SSHPROTO *sshp = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  curlx_safefree(sshp->رستہ);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ساكن فراغ myssh_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية ssh_conn *sshc = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  sshc_cleanup(sshc, عدم, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ساكن CURLcode ssh_تیاری_connection(بنية Curl_easy *انکڑے,
                                     بنية connectdata *conn)
{
  بنية ssh_conn *sshc;
  بنية SSHPROTO *sshp;
  (فراغ)conn;

  sshc = curlx_calloc(1, حجم(*sshc));
  إذا(!sshc)
    أرجع CURLE_باہرلا_دا_حافظہ;

  إذا(Curl_conn_meta_سیٹ(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  sshp = curlx_calloc(1, حجم(*sshp));
  إذا(!sshp)
    أرجع CURLE_باہرلا_دا_حافظہ;

  curlx_dyn_init(&sshp->readdir, CURL_رستہ_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_رستہ_MAX);
  إذا(Curl_meta_سیٹ(انکڑے, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    أرجع CURLE_باہرلا_دا_حافظہ;

  أرجع Curl_ssh_تیاری_pkey(انکڑے, sshc);
}

ساكن Curl_recv scp_recv, sftp_recv;
ساكن Curl_send scp_send, sftp_send;

#جے_بنا_تعریف CURL_DISABLE_PROXY
ساكن ssize_t ssh_tls_recv(libssh2_socket_t sock, فراغ *بفر,
                            size_t لمائی, صحيح flags, فراغ **abstract)
{
  بنية Curl_easy *انکڑے = (بنية Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(انکڑے, sock);
  size_t nread;
  CURLcode نتیجہ;
  بنية connectdata *conn = انکڑے->conn;
  Curl_recv *backup = conn->recv[sockindex];
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (فراغ)flags;

  إذا(!sshc)
    أرجع -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  نتیجہ = Curl_conn_recv(انکڑے, sockindex, بفر, لمائی, &nread);
  conn->recv[sockindex] = backup;
  إذا(نتیجہ == CURLE_AGAIN)
    أرجع -EAGAIN; /* magic return code for libssh2 */
  وإلا إذا(نتیجہ)
    أرجع -1; /* generic error */
  Curl_debug(انکڑے, CURLINFO_انکڑے_وچ, (ثابت حرف *)بفر, nread);
  أرجع (ssize_t)nread;
}

ساكن ssize_t ssh_tls_send(libssh2_socket_t sock, ثابت فراغ *بفر,
                            size_t لمائی, صحيح flags, فراغ **abstract)
{
  بنية Curl_easy *انکڑے = (بنية Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(انکڑے, sock);
  size_t nwrite;
  CURLcode نتیجہ;
  بنية connectdata *conn = انکڑے->conn;
  Curl_send *backup = conn->send[sockindex];
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (فراغ)flags;

  إذا(!sshc)
    أرجع -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  نتیجہ = Curl_conn_send(انکڑے, sockindex, بفر, لمائی, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  إذا(نتیجہ == CURLE_AGAIN)
    أرجع -EAGAIN; /* magic return code for libssh2 */
  وإلا إذا(نتیجہ)
    أرجع -1; /* error */
  Curl_debug(انکڑے, CURLINFO_انکڑے_باہرلا, (ثابت حرف *)بفر, nwrite);
  أرجع (ssize_t)nwrite;
}
#جے_انت

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ساكن CURLcode ssh_connect(بنية Curl_easy *انکڑے, bool *پورا)
{
#جے_تعریف_شدہ CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#جے_انت
  بنية connectdata *conn = انکڑے->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode نتیجہ;

#إذا LIBSSH2_نسخہ_عدد >= 0x010b00
  {
    ثابت حرف *crypto_str;
    بدل(libssh2_crypto_engine()) {
    حالة libssh2_gcrypt:
      crypto_str = "libgcrypt";
      اكسر;
    حالة libssh2_mbedtls:
      crypto_str = "mbedTLS";
      اكسر;
    حالة libssh2_openssl:
      crypto_str = "openssl compatible";
      اكسر;
    حالة libssh2_os400qc3:
      crypto_str = "OS400QC3";
      اكسر;
    حالة libssh2_wincng:
      crypto_str = "WinCNG";
      اكسر;
    افتراضي:
      crypto_str = عدم;
      اكسر;
    }
    إذا(crypto_str)
      infof(انکڑے, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#جے_انت

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  infof(انکڑے, "SSH: user '%s'", Curl_creds_user(conn->creds));
#جے_تعریف_شدہ CURL_LIBSSH2_DEBUG
  infof(انکڑے, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#جے_انت /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, انکڑے);

  إذا(!sshc->ssh_session) {
    failf(انکڑے, "Failure initializing ssh session");
    أرجع CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#إذا LIBSSH2_نسخہ_عدد >= 0x010B00
  إذا(انکڑے->سیٹ.server_response_timeout > 0) {
    libssh2_session_سیٹ_read_timeout(sshc->ssh_session,
                             (لما)(انکڑے->سیٹ.server_response_timeout / 1000));
  }
#جے_انت

#جے_بنا_تعریف CURL_DISABLE_PROXY
  إذا(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#إذا LIBSSH2_نسخہ_عدد >= 0x010b01
    infof(انکڑے, "SSH: using HTTPS proxy");
#إذا defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#جے_انت
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#إذا defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#جے_انت
#وإلا
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    اتحاد receive {
      فراغ *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, فراغ *, size_t, صحيح, فراغ **);
    };
    اتحاد transfer {
      فراغ *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ثابت فراغ *, size_t, صحيح, فراغ **);
    };
    اتحاد receive sshrecv;
    اتحاد transfer sshsend;

    sshrecv.recvptr = ssh_tls_recv;
    sshsend.sendptr = ssh_tls_send;

    infof(انکڑے, "SSH: using HTTPS proxy");
    libssh2_session_callback_سیٹ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_سیٹ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#جے_انت

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#جے_انت /* CURL_DISABLE_PROXY */
  إذا(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  وإلا {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  إذا(انکڑے->سیٹ.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(انکڑے, "SSH: failed to enable compression for session");
  }

  إذا(CURL_EASY_STR(انکڑے, STRING_SSH_KNOWNHOSTS)) {
    صحيح rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    إذا(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = عدم;
      أرجع CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(انکڑے, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    إذا(rc < 0)
      infof(انکڑے, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(انکڑے, STRING_SSH_KNOWNHOSTS));
  }

#جے_تعریف_شدہ CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(انکڑے, "SSH: socket %d", (صحيح)sock);
#جے_انت

  myssh_منزل(انکڑے, sshc, SSH_INIT);

  نتیجہ = ssh_multi_statemach(انکڑے, پورا);

  أرجع نتیجہ;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ساكن CURLcode scp_perform(بنية Curl_easy *انکڑے,
                            bool *connected,
                            bool *dophase_پورا)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(انکڑے->conn, CURL_META_SSH_CONN);
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_SSH(انکڑے, "DO phase starts");

  *dophase_پورا = FALSE; /* not done yet */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_منزل(انکڑے, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  نتیجہ = ssh_multi_statemach(انکڑے, dophase_پورا);

  *connected = Curl_conn_is_connected(انکڑے->conn, FIRSTSOCKET);

  إذا(*dophase_پورا) {
    CURL_TRC_SSH(انکڑے, "DO phase is complete");
  }

  أرجع نتیجہ;
}

/* called from multi.c while DOing */
ساكن CURLcode scp_doing(بنية Curl_easy *انکڑے,
                          bool *dophase_پورا)
{
  CURLcode نتیجہ;
  نتیجہ = ssh_multi_statemach(انکڑے, dophase_پورا);

  إذا(*dophase_پورا) {
    CURL_TRC_SSH(انکڑے, "DO phase is complete");
  }
  أرجع نتیجہ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode scp_disconnect(بنية Curl_easy *انکڑے,
                               بنية connectdata *conn,
                               bool dead_connection)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(انکڑے, CURL_META_SSH_EASY);
  (فراغ)dead_connection;

  إذا(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_منزل(انکڑے, sshc, SSH_SESSION_DISCONNECT);
    نتیجہ = ssh_بلاک_statemach(انکڑے, sshc, sshp, TRUE);
  }

  إذا(sshc)
    أرجع sshc_cleanup(sshc, انکڑے, TRUE);
  أرجع نتیجہ;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ساكن CURLcode ssh_پورا(بنية Curl_easy *انکڑے, CURLcode حالت)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(انکڑے->conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(انکڑے, CURL_META_SSH_EASY);
  CURLcode نتیجہ = CURLE_OK;

  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;

  إذا(!حالت)
    /* run the state-machine */
    نتیجہ = ssh_بلاک_statemach(انکڑے, sshc, sshp, FALSE);
  وإلا
    نتیجہ = حالت;

  إذا(Curl_pgrs_پورا(انکڑے, Curl_pgrs_now(انکڑے)))
    أرجع CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(انکڑے);
  أرجع نتیجہ;
}

ساكن CURLcode scp_پورا(بنية Curl_easy *انکڑے, CURLcode حالت,
                         bool premature)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(انکڑے->conn, CURL_META_SSH_CONN);
  (فراغ)premature;

  إذا(sshc && !حالت)
    myssh_منزل(انکڑے, sshc, SSH_SCP_پورا);

  أرجع ssh_پورا(انکڑے, حالت);
}

ساكن CURLcode scp_send(بنية Curl_easy *انکڑے, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nwritten;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  (فراغ)eos;
  *pnwritten = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ثابت حرف *)mem, len);

  ssh_block2waitfor(انکڑے, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  إذا(nwritten == LIBSSH2_ERROR_EAGAIN)
    نتیجہ = CURLE_AGAIN;
  وإلا إذا(nwritten < LIBSSH2_ERROR_NONE)
    نتیجہ = libssh2_session_error_منزل_CURLE((صحيح)nwritten);
  وإلا
    *pnwritten = (size_t)nwritten;

  أرجع نتیجہ;
}

ساكن CURLcode scp_recv(بنية Curl_easy *انکڑے, int8_t sockindex,
                         حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nread;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(انکڑے, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  إذا(nread == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nread < LIBSSH2_ERROR_NONE)
    نتیجہ = libssh2_session_error_منزل_CURLE((صحيح)nread);
  وإلا
    *pnread = (size_t)nread;

  أرجع نتیجہ;
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

ساكن CURLcode sftp_perform(بنية Curl_easy *انکڑے,
                             bool *connected,
                             bool *dophase_پورا)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(انکڑے->conn, CURL_META_SSH_CONN);
  CURLcode نتیجہ = CURLE_OK;

  CURL_TRC_SSH(انکڑے, "DO phase starts");

  *dophase_پورا = FALSE; /* not done yet */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_منزل(انکڑے, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  نتیجہ = ssh_multi_statemach(انکڑے, dophase_پورا);

  *connected = Curl_conn_is_connected(انکڑے->conn, FIRSTSOCKET);

  إذا(*dophase_پورا) {
    CURL_TRC_SSH(انکڑے, "DO phase is complete");
  }

  أرجع نتیجہ;
}

/* called from multi.c while DOing */
ساكن CURLcode sftp_doing(بنية Curl_easy *انکڑے,
                           bool *dophase_پورا)
{
  CURLcode نتیجہ = ssh_multi_statemach(انکڑے, dophase_پورا);

  إذا(*dophase_پورا) {
    CURL_TRC_SSH(انکڑے, "DO phase is complete");
  }
  أرجع نتیجہ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode sftp_disconnect(بنية Curl_easy *انکڑے,
                                بنية connectdata *conn, bool dead_connection)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(انکڑے, CURL_META_SSH_EASY);
  (فراغ)dead_connection;

  إذا(sshc) {
    إذا(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(انکڑے, "DISCONNECT starts now");
      myssh_منزل(انکڑے, sshc, SSH_SFTP_SHUTDOWN);
      نتیجہ = ssh_بلاک_statemach(انکڑے, sshc, sshp, TRUE);
      CURL_TRC_SSH(انکڑے, "DISCONNECT is done -> %d", (صحيح)نتیجہ);
    }
    sshc_cleanup(sshc, انکڑے, TRUE);
  }
  أرجع نتیجہ;
}

ساكن CURLcode sftp_پورا(بنية Curl_easy *انکڑے, CURLcode حالت,
                          bool premature)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  إذا(!حالت) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    إذا(!premature && انکڑے->سیٹ.postquote && !conn->بٹ.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_منزل(انکڑے, sshc, SSH_SFTP_CLOSE);
  }
  أرجع ssh_پورا(انکڑے, حالت);
}

/* return number of sent bytes */
ساكن CURLcode sftp_send(بنية Curl_easy *انکڑے, int8_t sockindex,
                          ثابت uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (فراغ)sockindex;
  (فراغ)eos;
  *pnwritten = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ثابت حرف *)mem, len);

  ssh_block2waitfor(انکڑے, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  إذا(nwrite == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nwrite < LIBSSH2_ERROR_NONE)
    أرجع libssh2_session_error_منزل_CURLE((صحيح)nwrite);
  *pnwritten = (size_t)nwrite;
  أرجع CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ساكن CURLcode sftp_recv(بنية Curl_easy *انکڑے, int8_t sockindex,
                          حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (فراغ)sockindex;
  *pnread = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(انکڑے, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  إذا(nread == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nread < 0)
    أرجع libssh2_session_error_منزل_CURLE((صحيح)nread);

  *pnread = (size_t)nread;
  أرجع CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
ساكن CURLcode ssh_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ;
  bool connected = FALSE;
  بنية connectdata *conn = انکڑے->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *پورا = FALSE; /* default to false */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  انکڑے->req.ناپ = -1; /* make sure this is unknown at this point */
  sshc->دوجا_Create_Dirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(انکڑے);

  إذا(conn->scheme->protocol & CURLPROTO_SCP)
    نتیجہ = scp_perform(انکڑے, &connected, پورا);
  وإلا
    نتیجہ = sftp_perform(انکڑے, &connected, پورا);

  أرجع نتیجہ;
}

CURLcode Curl_ssh_init(فراغ)
{
  إذا(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    أرجع CURLE_FAILED_INIT;
  }
  أرجع CURLE_OK;
}

فراغ Curl_ssh_cleanup(فراغ)
{
  libssh2_exit();
}

فراغ Curl_ssh_نسخہ(حرف *بفر, size_t buflen)
{
  (فراغ)curl_msnprintf(بفر, buflen, "libssh2/%s", libssh2_نسخہ(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ساكن فراغ ssh_attach(بنية Curl_easy *انکڑے, بنية connectdata *conn)
{
  DEBUGASSERT(انکڑے);
  DEBUGASSERT(conn);
  إذا(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    إذا(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      فراغ **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = انکڑے;
    }
  }
}

/*
 * SCP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_scp = {
  ssh_تیاری_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_پورا,                             /* done */
  ZERO_NULL,                            /* do_more */
  ssh_connect,                          /* connect_it */
  ssh_multi_statemach,                  /* connecting */
  scp_doing,                            /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  scp_disconnect,                       /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ssh_attach,                           /* attach */
  ZERO_NULL,                            /* follow */
};

/*
 * SFTP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_sftp = {
  ssh_تیاری_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  sftp_پورا,                            /* done */
  ZERO_NULL,                            /* do_more */
  ssh_connect,                          /* connect_it */
  ssh_multi_statemach,                  /* connecting */
  sftp_doing,                           /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  sftp_disconnect,                      /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ssh_attach,                           /* attach */
  ZERO_NULL,                            /* follow */
};

#جے_انت /* USE_LIBSSH2 */
