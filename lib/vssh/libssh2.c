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

#اگر_تعریف_شده USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

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
#ضمن "parsedate.h" /* for the week day and month names */
#ضمن "multiif.h"
#ضمن "select.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "vssh/vssh.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/base64.h" /* for curlx_base64_encode() */

ساكن ثابت حرف *sftp_libssh2_strerror(بیعلامت دراز err)
{
  بدل(err) {
  حالة LIBSSH2_FX_NO_SUCH_FILE:
    أرجع "No such file or directory";

  حالة LIBSSH2_FX_PERMISSION_DENIED:
    أرجع "Permission denied";

  حالة LIBSSH2_FX_FAILURE:
    أرجع "Operation failed";

  حالة LIBSSH2_FX_BAD_پیام:
    أرجع "Bad message from SFTP server";

  حالة LIBSSH2_FX_NO_CONNECTION:
    أرجع "Not connected to SFTP server";

  حالة LIBSSH2_FX_CONNECTION_گمشده:
    أرجع "Connection to SFTP server lost";

  حالة LIBSSH2_FX_OP_UNSUPPORTED:
    أرجع "Operation not supported by SFTP server";

  حالة LIBSSH2_FX_INVALID_HANDLE:
    أرجع "Invalid handle";

  حالة LIBSSH2_FX_NO_SUCH_مسیر:
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

  حالة LIBSSH2_FX_قفل_CONFLICT:
    أرجع "File lock conflict";

  حالة LIBSSH2_FX_DIR_NOT_خالی:
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
                         صحيح رقم_prompts,
                         ثابت LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         فراغ **abstract)
{
  بنية Curl_easy *داده = (بنية Curl_easy *)*abstract;

#اگر_تعریف_شده CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", رقم_prompts);
#وإلا
  (فراغ)name;
  (فراغ)name_len;
  (فراغ)instruction;
  (فراغ)instruction_len;
#پایان_اگر /* CURL_LIBSSH2_DEBUG */
  إذا(رقم_prompts == 1) {
    بنية connectdata *conn = داده->conn;
    ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].متن_3 = Curl_cstrdup(passwd);
    responses[0].درازی = responses[0].متن_3 ? curlx_uztoui(طول_رشته(passwd)) : 0;
  }
  (فراغ)prompts;
} /* kbd_callback */

ساكن CURLcode sftp_libssh2_error_مقصد_CURLE(بیعلامت دراز err)
{
  بدل(err) {
  حالة LIBSSH2_FX_OK:
    أرجع CURLE_OK;

  حالة LIBSSH2_FX_NO_SUCH_FILE:
  حالة LIBSSH2_FX_NO_SUCH_مسیر:
    أرجع CURLE_REMOTE_FILE_NOT_یافت;

  حالة LIBSSH2_FX_PERMISSION_DENIED:
  حالة LIBSSH2_FX_WRITE_PROTECT:
  حالة LIBSSH2_FX_قفل_CONFLICT:
    أرجع CURLE_REMOTE_ACCESS_DENIED;

  حالة LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  حالة LIBSSH2_FX_QUOTA_EXCEEDED:
    أرجع CURLE_REMOTE_DISK_پر;

  حالة LIBSSH2_FX_FILE_ALREADY_EXISTS:
    أرجع CURLE_REMOTE_FILE_EXISTS;

  حالة LIBSSH2_FX_DIR_NOT_خالی:
    أرجع CURLE_QUOTE_ERROR;

  افتراضي:
    اكسر;
  }

  أرجع CURLE_SSH;
}

ساكن CURLcode libssh2_session_error_مقصد_CURLE(صحيح err)
{
  بدل(err) {
  /* Ordered by order of appearance in libssh2.h */
  حالة LIBSSH2_ERROR_NONE:
    أرجع CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  حالة LIBSSH2_ERROR_SCP_PROTOCOL:
    أرجع CURLE_REMOTE_FILE_NOT_یافت;

  حالة LIBSSH2_ERROR_SOCKET_NONE:
    أرجع CURLE_COULDNT_CONNECT;

  حالة LIBSSH2_ERROR_ALLOC:
    أرجع CURLE_بیرون_از_حافظه;

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
  أرجع Curl_cmalloc(شمار);
}

ساكن LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (فراغ)abstract;
  أرجع Curl_crealloc(ptr, شمار);
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
                          تعداد curl_khmatch تطابق,
                          فراغ *clientp)
{
  (فراغ)easy;
  (فراغ)knownkey;
  (فراغ)foundkey;
  (فراغ)clientp;

  /* we only allow perfect matches, and we reject everything else */
  أرجع (تطابق != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ساكن تعداد curl_khtype convert_ssh2_keytype(صحيح sshkeytype)
{
  تعداد curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  بدل(sshkeytype) {
  حالة LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    اكسر;
#اگر_تعریف_شده LIBSSH2_HOSTKEY_TYPE_DSS
  حالة LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    اكسر;
#پایان_اگر
#اگر_تعریف_شده LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#پایان_اگر
#اگر_تعریف_شده LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#پایان_اگر
#اگر_تعریف_شده LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#پایان_اگر
#اگر_تعریف_شده LIBSSH2_HOSTKEY_TYPE_ED25519
  حالة LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    اكسر;
#پایان_اگر
  }
  أرجع keytype;
}

ساكن CURLcode ssh_knownhost(بنية Curl_easy *داده,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = داده->conn;
  بنية libssh2_knownhost *host = عدم;
  ثابت حرف *remotekey = عدم;
  صحيح keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  صحيح keybit = 0;
  صحيح sshkeytype = 0;
  size_t keylen = 0;
  صحيح rc = 0;
  CURLcode نتیجه = CURLE_OK;

  إذا(!CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS)) {
    infof(داده, "SSH: no knownhosts file configured");
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
      داده->مجموعه.ssh_keyfunc ? داده->مجموعه.ssh_keyfunc : sshkeycallback;
    بنية curl_khkey knownkey;
    بنية curl_khkey *knownkeyp = عدم;
    بنية curl_khkey foundkey;

    بدل(sshkeytype) {
    حالة LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_کلید_SSHRSA;
      اكسر;
#اگر_تعریف_شده LIBSSH2_HOSTKEY_TYPE_DSS
    حالة LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_کلید_SSHDSS;
      اكسر;
#پایان_اگر
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
      infof(داده, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      اكسر;
    }
    إذا(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    وإلا {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->پورت != پورت_SSH) ?
                                          conn->origin->پورت : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(داده, "SSH: host check %d, key: %s", keycheck,
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
      CURL_CBAPI_شروع(&guard, داده, easy_ssh_keyfunc);
      rc = func(داده, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, داده->مجموعه.ssh_keyfunc_userp);
      CURL_CBAPI_انتها(&guard);
    }
  }
  وإلا {
    /* no remotekey means failure! */
    infof(داده, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  بدل(rc) {
  افتراضي: /* unknown return codes is the same as reject */
  حالة CURLKHSTAT_REJECT:
    infof(داده, "SSH: knownhost check failed");
    myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  حالة CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    اكسر;
  حالة CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    إذا(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  حالة CURLKHSTAT_FINE:
  حالة CURLKHSTAT_FINE_جمع_کن_مقصد_FILE:
    /* proceed */
    إذا(keycheck != LIBSSH2_KNOWNHOST_CHECK_تطابق) {
      صحيح addrc;
      ثابت حرف *hostbuf;
      حرف *hostport = عدم;
      إذا(conn->origin->پورت != پورت_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->پورت);
        إذا(!hostbuf)
          infof(داده, "WARNING: failed allocating buffer for [host]:port");
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
          infof(داده, "WARNING: adding the known host %s failed", hostbuf);
        وإلا إذا(rc == CURLKHSTAT_FINE_جمع_کن_مقصد_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          صحيح wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          إذا(wrc) {
            infof(داده, "WARNING: writing %s failed",
                  CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    وإلا
      infof(داده, "SSH: knownhost entry matches host key");
    اكسر;
  }
  أرجع نتیجه;
}

ساكن CURLcode ssh_check_fingerprint(بنية Curl_easy *داده,
                                      بنية ssh_conn *sshc)
{
  ثابت حرف *pubkey_md5 =
    CURL_EASY_STR(داده, STRING_SSH_HOST_عمومی_کلید_MD5);
  ثابت حرف *pubkey_sha256 =
    CURL_EASY_STR(داده, STRING_SSH_HOST_عمومی_کلید_SHA256);

  إذا(pubkey_sha256) {
    ثابت حرف *fingerprint = عدم;
    حرف *fingerprint_b64 = عدم;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(داده, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    إذا(!fingerprint) {
      failf(داده,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    إذا(curlx_base64_encode((ثابت uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    إذا(!fingerprint_b64) {
      failf(داده, "SHA256 fingerprint could not be encoded");
      myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(داده, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

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
      myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(داده, "SSH: SHA256 checksum match");
  }

  إذا(pubkey_md5) {
    حرف md5buffer[33];
    ثابت حرف *fingerprint;

    infof(داده, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    إذا(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      صحيح i;
      لكل(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (بیعلامت حرف)fingerprint[i]);
      }

      infof(داده, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    إذا(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      إذا(fingerprint) {
        failf(داده,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      وإلا {
        failf(داده,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(داده, "SSH: MD5 checksum match");
  }

  إذا(!pubkey_md5 && !pubkey_sha256) {
    إذا(داده->مجموعه.ssh_hostkeyfunc) {
      size_t keylen = 0;
      صحيح sshkeytype = 0;
      صحيح rc = 0;
      /* we handle the process to the callback */
      ثابت حرف *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      إذا(remotekey) {
        بنية Curl_mapi_guard guard;
        تعداد curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_شروع(&guard, داده, easy_ssh_hostkeyfunc);
        rc = داده->مجموعه.ssh_hostkeyfunc(داده->مجموعه.ssh_hostkeyfunc_userp,
                                       (صحيح)keytype, remotekey, keylen);
        CURL_CBAPI_انتها(&guard);
        إذا(rc != CURLKHMATCH_OK) {
          myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
          failf(داده, "SSH: callback failed host public key verification");
          أرجع CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(داده, "SSH: verified public key via callback");
      }
      وإلا {
        myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
        أرجع CURLE_PEER_FAILED_VERIFICATION;
      }
      أرجع CURLE_OK;
    }
    وإلا {
      CURL_TRC_SSH(داده, "no host key checksum given, checking knownhosts");
      أرجع ssh_knownhost(داده, sshc);
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
ساكن CURLcode ssh_force_knownhost_کلید_type(بنية Curl_easy *داده,
                                             بنية ssh_conn *sshc)
{
  CURLcode نتیجه = CURLE_OK;

  ساكن ثابت حرف hostkey_روش_ssh_ed25519[] = "ssh-ed25519";
  ساكن ثابت حرف hostkey_روش_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ساكن ثابت حرف hostkey_روش_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ساكن ثابت حرف hostkey_روش_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ساكن ثابت حرف hostkey_روش_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#اگر_تعریف_شده LIBSSH2_KNOWNHOST_کلید_SSHDSS
  ساكن ثابت حرف hostkey_روش_ssh_dss[] = "ssh-dss";
#پایان_اگر
  bool یافت = FALSE;

  إذا(sshc->kh &&
     !CURL_EASY_STR(داده, STRING_SSH_HOST_عمومی_کلید_MD5) &&
     !CURL_EASY_STR(داده, STRING_SSH_HOST_عمومی_کلید_SHA256)) {
    بنية libssh2_knownhost *store = عدم;
    بنية connectdata *conn = داده->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_کلید_ماسک >>
                      LIBSSH2_KNOWNHOST_کلید_لغزش) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    طالما(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      إذا(store) {
        إذا(store->name) {
          إذا(store->name[0] == '[') {
            curl_off_t پورت;
            size_t kh_name_اندازه = 0;
            ثابت حرف *p;
            ثابت حرف *kh_name_انتها = جستجوی_رشته(store->name, "]:");
            إذا(!kh_name_انتها) {
              infof(داده, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS));
              استمر;
            }
            p = kh_name_انتها + 2; /* start of port number */
            إذا(!curlx_str_عدد(&p, &پورت, 0xffff) &&
               (kh_name_انتها && (پورت == conn->origin->پورت))) {
              kh_name_اندازه = طول_رشته(store->name) - 1 - طول_رشته(kh_name_انتها);
              إذا((kh_name_اندازه == طول_رشته(conn->origin->hostname)) &&
                 !مقایسه_رشته(store->name + 1, conn->origin->hostname,
                          kh_name_اندازه)) {
                یافت = TRUE;
                اكسر;
              }
            }
          }
          وإلا إذا(!strcmp(store->name, conn->origin->hostname)) {
            یافت = TRUE;
            اكسر;
          }
        }
        وإلا إذا(store->کلید) {
          بیعلامت صحيح type = (store->typemask & LIBSSH2_KNOWNHOST_کلید_ماسک)
                              >> LIBSSH2_KNOWNHOST_کلید_لغزش;
          إذا(!absent_type[type]) {
            صحيح keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->پورت != پورت_SSH) ? conn->origin->پورت : -1,
              store->کلید, طول_رشته(store->کلید),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_کلید_ماسک),
              عدم);
            إذا(keycheck == LIBSSH2_KNOWNHOST_CHECK_تطابق) {
              یافت = TRUE;
              اكسر;
            }
            وإلا إذا(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    إذا(یافت) {
      صحيح rc;
      ثابت حرف *hostkey_روش = عدم;
      infof(داده, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS));

      بدل(store->typemask & LIBSSH2_KNOWNHOST_کلید_ماسک) {
      حالة LIBSSH2_KNOWNHOST_کلید_ED25519:
        hostkey_روش = hostkey_روش_ssh_ed25519;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلید_ECDSA_521:
        hostkey_روش = hostkey_روش_ssh_ecdsa_521;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلید_ECDSA_384:
        hostkey_روش = hostkey_روش_ssh_ecdsa_384;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلید_ECDSA_256:
        hostkey_روش = hostkey_روش_ssh_ecdsa_256;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلید_SSHRSA:
        hostkey_روش = hostkey_روش_ssh_rsa_all;
        اكسر;
#اگر_تعریف_شده LIBSSH2_KNOWNHOST_کلید_SSHDSS
      حالة LIBSSH2_KNOWNHOST_کلید_SSHDSS:  /* deprecated upstream */
        hostkey_روش = hostkey_روش_ssh_dss;
        اكسر;
#پایان_اگر
      حالة LIBSSH2_KNOWNHOST_کلید_RSA1:
        failf(داده, "Found host key type RSA1 which is not supported");
        أرجع CURLE_SSH;
      افتراضي:
        failf(داده, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_کلید_ماسک));
        أرجع CURLE_SSH;
      }

      infof(داده, "SSH: set '%s' as hostkey type", hostkey_روش);
      rc = libssh2_session_روش_pref(sshc->ssh_session,
                                       LIBSSH2_روش_HOSTKEY, hostkey_روش);
      إذا(rc) {
        حرف *err_پیام = عدم;
        صحيح errlen;
        libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, &errlen, 0);
        failf(داده, "libssh2 method '%s' failed: %s", hostkey_روش, err_پیام);
        نتیجه = libssh2_session_error_مقصد_CURLE(rc);
      }
    }
    وإلا {
      infof(داده, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS));
    }
  }

  أرجع نتیجه;
}

ساكن CURLcode quote_error(بنية Curl_easy *داده,
                            بنية ssh_conn *sshc)
{
  failf(داده, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_quote(بنية Curl_easy *داده,
                           بنية ssh_conn *sshc,
                           بنية SSHPROTO *sshp)
{
  ثابت حرف *cp;
  CURLcode نتیجه = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
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

  إذا(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    حرف *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->مسیر);
    إذا(!tmp)
      أرجع CURLE_بیرون_از_حافظه;
    Curl_debug(داده, CURLINFO_سرآیند_بیرون, "PWD\n", 4);
    Curl_debug(داده, CURLINFO_سرآیند_درون, tmp, طول_رشته(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, tmp, طول_رشته(tmp));
    curlx_free(tmp);
    إذا(!نتیجه)
      myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
    أرجع نتیجه;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  إذا(!cp) {
    failf(داده, "Syntax error command '%s', missing parameter", cmd);
    أرجع CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  نتیجه = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  إذا(نتیجه) {
    إذا(نتیجه != CURLE_بیرون_از_حافظه)
      failf(داده, "Syntax error: Bad first parameter to '%s'", cmd);
    أرجع نتیجه;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
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
      إذا(نتیجه != CURLE_بیرون_از_حافظه)
        failf(داده, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      أرجع نتیجه;
    }
    إذا(*cp)
      أرجع quote_error(داده, sshc);

    memset(&sshp->quote_attrs, 0, حجم(LIBSSH2_SFTP_صفات));
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_STAT);
    أرجع نتیجه;
  }
  إذا(!مقایسه_رشته(cmd, "ln ", 3) ||
     !مقایسه_رشته(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    نتیجه = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(نتیجه) {
      إذا(نتیجه != CURLE_بیرون_از_حافظه)
        failf(داده, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      أرجع نتیجه;
    }
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_SYMLINK);
    أرجع نتیجه;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "mkdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    /* create directory */
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_MKDIR);
    أرجع نتیجه;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    نتیجه = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(نتیجه) {
      إذا(نتیجه != CURLE_بیرون_از_حافظه)
        failf(داده, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      أرجع نتیجه;
    }
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_RENAME);
    أرجع نتیجه;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "rmdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    /* delete directory */
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_RMDIR);
    أرجع نتیجه;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "rm ", 3)) {
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_UNLINK);
    أرجع نتیجه;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "statvfs ", 8)) {
    إذا(*cp)
      أرجع quote_error(داده, sshc);
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_STATVFS);
    أرجع نتیجه;
  }

  failf(داده, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_upload_init(بنية Curl_easy *داده,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp,
                                 bool *blockp)
{
  بیعلامت دراز flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  إذا(داده->state.resume_from) {
    LIBSSH2_SFTP_صفات attrs;
    إذا(داده->state.resume_from < 0) {
      صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->مسیر,
                                    curlx_uztoui(طول_رشته(sshp->مسیر)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      إذا(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        أرجع CURLE_OK;
      }
      إذا(rc) {
        داده->state.resume_from = 0;
      }
      وإلا {
        curl_off_t اندازه = attrs.filesize;
        إذا(اندازه < 0) {
          failf(داده, "Bad file size (%" FMT_OFF_T ")", اندازه);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        داده->state.resume_from = attrs.filesize;
      }
    }
  }

  إذا(داده->مجموعه.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  وإلا إذا(داده->state.resume_from > 0) {
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
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->مسیر,
                         curlx_uztoui(طول_رشته(sshp->مسیر)),
                         flags, (دراز)داده->مجموعه.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  إذا(!sshc->sftp_handle) {
    CURLcode نتیجه;
    بیعلامت دراز sftperr;
    صحيح rc = libssh2_session_آخرین_errno(sshc->ssh_session);

    إذا(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      أرجع CURLE_OK;
    }

    إذا(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    وإلا
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    إذا(sshc->دومی_Create_Dirs) {
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      failf(داده, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      أرجع sftp_libssh2_error_مقصد_CURLE(sftperr);
    }
    إذا((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_مسیر) &&
       داده->مجموعه.ftp_create_missing_dirs &&
       (طول_رشته(sshp->مسیر) > 1)) {
      /* try to create the path remotely */
      sshc->دومی_Create_Dirs = 1;
      myssh_مقصد(داده, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      أرجع CURLE_OK;
    }
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    نتیجه = sftp_libssh2_error_مقصد_CURLE(sftperr);
    إذا(!نتیجه) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      نتیجه = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(داده, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    أرجع نتیجه;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
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
        أرجع CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      افعل {
        بنية Curl_mapi_guard guard;
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (داده->state.resume_from - passed >
           (curl_off_t)حجم(scratch)) ?
          حجم(scratch) : curlx_sotouz(داده->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_شروع(&guard, داده, easy_fread_func);
        actuallyread = داده->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              داده->state.درون);
        CURL_CBAPI_انتها(&guard);

        passed += actuallyread;
        إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(داده, "Failed to read data");
          أرجع CURLE_FTP_COULDNT_USE_REST;
        }
      } طالما(passed < داده->state.resume_from);
    }

    /* now, decrease the size of the read */
    إذا(داده->state.infilesize > 0) {
      داده->state.infilesize -= داده->state.resume_from;
      داده->req.اندازه = داده->state.infilesize;
      Curl_pgrs_مجموعه_Upload_اندازه(داده, داده->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)داده->state.resume_from);
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
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(داده);

  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن فراغ ssh_state_pkey_init(بنية Curl_easy *داده, بنية ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  إذا((داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     جستجوی_رشته(sshc->authlist, "publickey")) {
    إذا(sshc->pub_کلید)
      infof(داده, "SSH: trying public key file '%s'", sshc->pub_کلید);
    infof(داده, "SSH: trying private key file '%s'", sshc->priv_کلید);
    myssh_مقصد(داده, sshc, SSH_AUTH_PKEY);
  }
  وإلا
    myssh_مقصد(داده, sshc, SSH_AUTH_PASS_INIT);
}

ساكن CURLcode sftp_quote_stat(بنية Curl_easy *داده,
                                بنية ssh_conn *sshc,
                                بنية SSHPROTO *sshp,
                                bool *blockp)
{
  ثابت حرف *cmd = sshc->quote_قلم->داده;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  إذا(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  إذا(مقایسه_رشته(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(طول_رشته(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    إذا(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      أرجع CURLE_OK;
    }
    إذا(rc && !sshc->acceptfail) { /* get those attributes */
      بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
      failf(داده, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      اذهب fail;
    }
  }

  /* Now set the new attributes... */
  إذا(!مقایسه_رشته(cmd, "chgrp", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t gid;
    إذا(!curlx_str_عدد(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (بیعلامت دراز)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وإلا إذا(!sshc->acceptfail) {
      failf(داده, "Syntax error: chgrp gid not a number");
      اذهب fail;
    }
  }
  وإلا إذا(!مقایسه_رشته(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت حرف *p = sshc->quote_path1;
    /* permissions are octal */
    إذا(curlx_str_octal(&p, &perms, 07777)) {
      failf(داده, "Syntax error: chmod permissions not a number");
      اذهب fail;
    }

    sshp->quote_attrs.permissions = (بیعلامت دراز)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  وإلا إذا(!مقایسه_رشته(cmd, "chown", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t uid;
    إذا(!curlx_str_عدد(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (بیعلامت دراز)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وإلا إذا(!sshc->acceptfail) {
      failf(داده, "Syntax error: chown uid not a number");
      اذهب fail;
    }
  }
  وإلا إذا(!مقایسه_رشته(cmd, "atime", 5) ||
          !مقایسه_رشته(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    إذا(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(داده, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#إذا SIZEOF_زمان_T > SIZEOF_LONG
    إذا(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(داده, "date overflow");
      fail = TRUE;
    }
#پایان_اگر
    إذا(fail)
      اذهب fail;
    إذا(!مقایسه_رشته(cmd, "atime", 5))
      sshp->quote_attrs.atime = (بیعلامت دراز)date;
    وإلا /* mtime */
      sshp->quote_attrs.mtime = (بیعلامت دراز)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE_SETSTAT);
  أرجع CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_download_stat(بنية Curl_easy *داده,
                                   بنية ssh_conn *sshc,
                                   بنية SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_صفات attrs;
  صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->مسیر,
                                curlx_uztoui(طول_رشته(sshp->مسیر)),
                                LIBSSH2_SFTP_STAT, &attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    أرجع CURLE_OK;
  }
  إذا(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_اندازه) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    داده->req.اندازه = -1;
    داده->req.maxdownload = -1;
    Curl_pgrs_مجموعه_Download_اندازه(داده, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  وإلا {
    curl_off_t اندازه = attrs.filesize;

    إذا(اندازه < 0) {
      failf(داده, "Bad file size (%" FMT_OFF_T ")", اندازه);
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
    إذا(داده->state.use_range) {
      curl_off_t from;
      CURLcode نتیجه = Curl_ssh_range(داده, داده->state.range, اندازه,
                                       &from, &اندازه);
      إذا(نتیجه)
        أرجع نتیجه;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    داده->req.اندازه = اندازه;
    داده->req.maxdownload = اندازه;
    Curl_pgrs_مجموعه_Download_اندازه(داده, اندازه);
  }

  /* We can resume if we can seek to the resume position */
  إذا(داده->state.resume_from) {
    إذا(داده->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      إذا((curl_off_t)attrs.filesize < -داده->state.resume_from) {
        failf(داده, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              داده->state.resume_from, (curl_off_t)attrs.filesize);
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      داده->state.resume_from += attrs.filesize;
    }
    وإلا {
      إذا((curl_off_t)attrs.filesize < داده->state.resume_from) {
        failf(داده, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              داده->state.resume_from, (curl_off_t)attrs.filesize);
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    داده->req.اندازه = attrs.filesize - داده->state.resume_from;
    داده->req.maxdownload = attrs.filesize - داده->state.resume_from;
    Curl_pgrs_مجموعه_Download_اندازه(داده, attrs.filesize - داده->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)داده->state.resume_from);
  }

  /* Setup the actual download */
  إذا(داده->req.اندازه == 0) {
    /* no data to transfer */
    Curl_xfer_راهاندازی_nop(داده);
    infof(داده, "SSH: file already completely downloaded");
    myssh_مقصد(داده, sshc, SSH_STOP);
    أرجع CURLE_OK;
  }
  Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, داده->req.اندازه);

  /* not set by Curl_xfer_setup to preserve io_flags */
  داده->conn->send_idx = 0;

  myssh_مقصد(داده, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode sftp_readdir(بنية Curl_easy *داده,
                             بنية ssh_conn *sshc,
                             بنية SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode نتیجه = CURLE_OK;
  صحيح rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_مسیر_MAX,
                                   sshp->readdir_longentry, CURL_مسیر_MAX,
                                   &sshp->readdir_attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    أرجع نتیجه;
  }
  إذا(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    إذا(داده->مجموعه.list_only) {
      نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      إذا(!نتیجه)
        نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY, "\n", 1);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    وإلا {
      نتیجه = curlx_dyn_جمع_کن(&sshp->readdir, sshp->readdir_longentry);

      إذا(!نتیجه) {
        إذا((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          نتیجه = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->مسیر,
                                  sshp->readdir_filename);
          myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_LINK);
        }
        وإلا {
          myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      أرجع نتیجه;
    }
  }
  وإلا إذا(!rc) {
    myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_انجامشده);
  }
  وإلا {
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    نتیجه = sftperr ? sftp_libssh2_error_مقصد_CURLE(sftperr) : CURLE_SSH;
    failf(داده, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_آخرین_errno(sshc->ssh_session));
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
  }
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_init(بنية Curl_easy *داده,
                               بنية ssh_conn *sshc)
{
  CURLcode نتیجه;
  sshc->دومی_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_مجموعه_blocking(sshc->ssh_session, 0);

  نتیجه = ssh_force_knownhost_کلید_type(داده, sshc);
  إذا(نتیجه)
    myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
  وإلا
    myssh_مقصد(داده, sshc, SSH_S_STARTUP);
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_startup(بنية Curl_easy *داده,
                                  بنية ssh_conn *sshc)
{
  بنية connectdata *conn = داده->conn;
  صحيح rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc) {
    حرف *err_پیام = عدم;
    (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
    failf(داده, "Failure establishing ssh session: %d, %s", rc, err_پیام);

    myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
    أرجع CURLE_FAILED_INIT;
  }

  myssh_مقصد(داده, sshc, SSH_HOSTKEY);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_hostkey(بنية Curl_easy *داده,
                                  بنية ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode نتیجه = ssh_check_fingerprint(داده, sshc);
  إذا(!نتیجه)
    myssh_مقصد(داده, sshc, SSH_AUTHLIST);
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_authlist(بنية Curl_easy *داده,
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
  بنية connectdata *conn = داده->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(طول_رشته(user)));

  إذا(!sshc->authlist) {
    صحيح rc;
    إذا(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(داده, "SSH: user accepted with no authentication");
      myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
      أرجع CURLE_OK;
    }
    rc = libssh2_session_آخرین_errno(sshc->ssh_session);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
    أرجع libssh2_session_error_مقصد_CURLE(rc);
  }
  infof(داده, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_مقصد(داده, sshc, SSH_AUTH_PKEY_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pkey(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  بنية connectdata *conn = داده->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحيح rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(طول_رشته(user)),
    sshc->pub_کلید, sshc->priv_کلید, sshc->passphrase ? sshc->passphrase : "");
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(داده, "SSH: authenticated via publickey");
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
  }
  وإلا {
    حرف *err_پیام = عدم;
    حرف unknown[] = "Reason unknown (-1)";
    إذا(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_پیام = unknown;
    }
    وإلا {
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
    }
    infof(داده, "SSH: publickey authentication denied: %s", err_پیام);
    myssh_مقصد(داده, sshc, SSH_AUTH_PASS_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pass_init(بنية Curl_easy *داده,
                                         بنية ssh_conn *sshc)
{
  إذا((داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     جستجوی_رشته(sshc->authlist, "password")) {
    myssh_مقصد(داده, sshc, SSH_AUTH_PASS);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_AUTH_HOST_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pass(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc)
{
  بنية connectdata *conn = داده->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
  صحيح rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(طول_رشته(user)),
                                 passwd,
                                 curlx_uztoui(طول_رشته(passwd)),
                                 عدم);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    أرجع CURLE_AGAIN;
  }
  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(داده, "SSH: initialized password authentication");
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_AUTH_HOST_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_host_init(بنية Curl_easy *داده,
                                         بنية ssh_conn *sshc)
{
  إذا((داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     جستجوی_رشته(sshc->authlist, "hostbased")) {
    myssh_مقصد(داده, sshc, SSH_AUTH_HOST);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_AUTH_AGENT_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent_init(بنية Curl_easy *داده,
                                          بنية ssh_conn *sshc)
{
  صحيح rc = 0;
  إذا((داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     جستجوی_رشته(sshc->authlist, "publickey")) {

    infof(داده, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    إذا(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      إذا(!sshc->ssh_agent) {
        infof(داده, "SSH: could not create agent object");

        myssh_مقصد(داده, sshc, SSH_AUTH_کلید_INIT);
        أرجع CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc < 0) {
      infof(داده, "SSH: failure connecting to agent");
      myssh_مقصد(داده, sshc, SSH_AUTH_کلید_INIT);
    }
    وإلا {
      myssh_مقصد(داده, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  وإلا
    myssh_مقصد(داده, sshc, SSH_AUTH_کلید_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent_list(بنية Curl_easy *داده,
                                          بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_agent_list_identities(sshc->ssh_agent);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  إذا(rc < 0) {
    infof(داده, "SSH: failure requesting identities to agent");
    myssh_مقصد(داده, sshc, SSH_AUTH_کلید_INIT);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_عنصر_خنثی = عدم;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent(بنية Curl_easy *داده,
                                     بنية ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  صحيح rc = libssh2_agent_get_عنصر_خنثی(sshc->ssh_agent,
                                      &sshc->sshagent_عنصر_خنثی,
                                      sshc->sshagent_prev_عنصر_خنثی);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    CURL_TRC_SSH(داده, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(داده->conn->creds),
                 sshc->sshagent_عنصر_خنثی->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(داده->conn->creds),
                                sshc->sshagent_عنصر_خنثی);

    إذا(rc < 0) {
      إذا(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_عنصر_خنثی = sshc->sshagent_عنصر_خنثی;
        أرجع CURLE_OK;
      }
      أرجع CURLE_AGAIN;
    }
  }

  إذا(rc < 0)
    infof(داده, "SSH: failure requesting identities to agent");
  وإلا إذا(rc == 1)
    infof(داده, "SSH: no agent identity would match");

  إذا(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(داده, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(داده->conn->creds),
          sshc->sshagent_عنصر_خنثی->comment);
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_AUTH_کلید_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_کلید_init(بنية Curl_easy *داده,
                                        بنية ssh_conn *sshc)
{
  إذا((داده->مجموعه.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     جستجوی_رشته(sshc->authlist, "keyboard-interactive")) {
    myssh_مقصد(داده, sshc, SSH_AUTH_کلید);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_کلید(بنية Curl_easy *داده,
                                   بنية ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  بنية connectdata *conn = داده->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحيح rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(طول_رشته(user)),
                                             &kbd_callback);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(داده, "SSH: initialized keyboard interactive authentication");
    myssh_مقصد(داده, sshc, SSH_AUTH_انجامشده);
    أرجع CURLE_OK;
  }
  أرجع CURLE_LOGIN_DENIED;
}

ساكن CURLcode ssh_state_auth_انجامشده(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc)
{
  بنية connectdata *conn = داده->conn;
  إذا(!sshc->authed) {
    failf(داده, "Authentication failure");
    myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
    أرجع CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(داده, "SSH: authentication complete");

  Curl_pgrs_زمان(داده, TIMER_APPCONNECT); /* SSH is connected */

  داده->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  إذا(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_مقصد(داده, sshc, SSH_SFTP_INIT);
    أرجع CURLE_OK;
  }
  infof(داده, "SSH: connection established");
  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_init(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  إذا(!sshc->sftp_session) {
    حرف *err_پیام = عدم;
    إذا(libssh2_session_آخرین_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
    failf(داده, "Failure initializing sftp session: %s", err_پیام);
    myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
    أرجع CURLE_FAILED_INIT;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_REALPATH);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_realpath(بنية Curl_easy *داده,
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
                               sshp->readdir_filename, CURL_مسیر_MAX,
                               LIBSSH2_SFTP_REALPATH);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  myssh_مقصد(داده, sshc, SSH_STOP);
  إذا(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    إذا(!sshc->homedir)
      أرجع CURLE_بیرون_از_حافظه;
    curlx_free(داده->state.most_recent_ftp_entrypath);
    داده->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    إذا(!داده->state.most_recent_ftp_entrypath)
      أرجع CURLE_بیرون_از_حافظه;
  }
  وإلا {
    /* Return the error type */
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    CURLcode نتیجه;
    إذا(sftperr)
      نتیجه = sftp_libssh2_error_مقصد_CURLE(sftperr);
    وإلا
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      نتیجه = CURLE_SSH;
    CURL_TRC_SSH(داده, "error = %lu makes libcurl = %d", sftperr, (صحيح)نتیجه);
    أرجع نتیجه;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(داده, "CONNECT phase done");
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_init(بنية Curl_easy *داده,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  CURLcode نتیجه;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  نتیجه = Curl_getworkingpath(داده, sshc->homedir, &sshp->مسیر);
  إذا(نتیجه) {
    myssh_مقصد(داده, sshc, SSH_STOP);
    أرجع نتیجه;
  }

  إذا(داده->مجموعه.quote) {
    infof(داده, "SSH: sending quote commands");
    sshc->quote_قلم = داده->مجموعه.quote;
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_SFTP_GETINFO);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_postquote_init(بنية Curl_easy *داده,
                                              بنية ssh_conn *sshc)
{
  إذا(داده->مجموعه.postquote) {
    infof(داده, "SSH: sending quote commands");
    sshc->quote_قلم = داده->مجموعه.postquote;
    myssh_مقصد(داده, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_مقصد(داده, sshc, SSH_STOP);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote(بنية Curl_easy *داده,
                                     بنية ssh_conn *sshc,
                                     بنية SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode نتیجه;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  نتیجه = sftp_quote(داده, sshc, sshp);
  إذا(نتیجه) {
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_sftp_بعدی_quote(بنية Curl_easy *داده,
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
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_stat(بنية Curl_easy *داده,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode نتیجه;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  نتیجه = sftp_quote_stat(داده, sshc, sshp, blockp);
  إذا(نتیجه) {
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_sftp_quote_setstat(بنية Curl_easy *داده,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp)
{
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(طول_رشته(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    failf(داده, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_symlink(بنية Curl_easy *داده,
                                             بنية ssh_conn *sshc)
{
  صحيح rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(طول_رشته(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(طول_رشته(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    failf(داده, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_mkdir(بنية Curl_easy *داده,
                                           بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(طول_رشته(sshc->quote_path1)),
                                 (دراز)داده->مجموعه.new_directory_perms);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    failf(داده, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_rename(بنية Curl_easy *داده,
                                            بنية ssh_conn *sshc)
{
  صحيح rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(طول_رشته(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(طول_رشته(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    failf(داده, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_rmdir(بنية Curl_easy *داده,
                                           بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(طول_رشته(sshc->quote_path1)));
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    failf(داده, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_unlink(بنية Curl_easy *داده,
                                            بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(طول_رشته(sshc->quote_path1)));
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    failf(داده, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_statvfs(بنية Curl_easy *داده,
                                             بنية ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  صحيح rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(طول_رشته(sshc->quote_path1)),
                                &statvfs);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    failf(داده, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  وإلا إذا(rc == 0) {
#اگر_تعریف_شده _MSC_VER
#تعریف_کن CURL_LIBSSH2_VFS_اندازه_ماسک "I64u"
#وإلا
#تعریف_کن CURL_LIBSSH2_VFS_اندازه_ماسک "llu"
#پایان_اگر
    CURLcode نتیجه;
    حرف *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_اندازه_ماسک "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_بلوکها, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    إذا(!tmp) {
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع CURLE_بیرون_از_حافظه;
    }

    نتیجه = Curl_client_write(داده, CLIENTWRITE_سرآیند, tmp, طول_رشته(tmp));
    curlx_free(tmp);
    إذا(نتیجه) {
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع نتیجه;
    }
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_بعدی_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_create_dirs_mkdir(بنية Curl_easy *داده,
                                                 بنية ssh_conn *sshc,
                                                 بنية SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->مسیر,
                             curlx_uztoui(طول_رشته(sshp->مسیر)),
                             (دراز)داده->مجموعه.new_directory_perms);
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
    بیعلامت دراز sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    إذا((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
      أرجع sftp_libssh2_error_مقصد_CURLE(sftperr);
    }
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_CREATE_DIRS);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir_init(بنية Curl_easy *داده,
                                            بنية ssh_conn *sshc,
                                            بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  Curl_pgrs_مجموعه_Download_اندازه(داده, -1);
  إذا(داده->req.no_body) {
    myssh_مقصد(داده, sshc, SSH_STOP);
    أرجع CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->مسیر,
                         curlx_uztoui(طول_رشته(sshp->مسیر)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  إذا(!sshc->sftp_handle) {
    بیعلامت دراز sftperr;
    إذا(libssh2_session_آخرین_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    failf(داده, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    أرجع sftp_libssh2_error_مقصد_CURLE(sftperr);
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_READDIR);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir_link(بنية Curl_easy *داده,
                                            بنية ssh_conn *sshc,
                                            بنية SSHPROTO *sshp)
{
  CURLcode نتیجه;
  صحيح rc;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (بیعلامت صحيح)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_مسیر_MAX, LIBSSH2_SFTP_READLINK);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  إذا(rc < 0)
    أرجع CURLE_بیرون_از_حافظه;

  /* append filename and extra output */
  نتیجه = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  إذا(نتیجه)
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
  وإلا
    myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_BOTTOM);
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_scp_download_init(بنية Curl_easy *داده,
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
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->مسیر, &sb);
  إذا(!sshc->ssh_channel) {
    صحيح ssh_err;
    حرف *err_پیام = عدم;

    إذا(libssh2_session_آخرین_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    ssh_err = libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
    failf(داده, "%s", err_پیام);
    myssh_مقصد(داده, sshc, SSH_SCP_CHANNEL_FREE);
    أرجع libssh2_session_error_مقصد_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_اندازه;
  داده->req.maxdownload = (curl_off_t)sb.st_اندازه;
  Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  داده->conn->send_idx = 0;

  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_close(بنية Curl_easy *داده,
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
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
      infof(داده, "Failed to close libssh2 file: %d %s", rc, err_پیام);
    }
    sshc->sftp_handle = عدم;
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
  وإلا
    myssh_مقصد(داده, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_shutdown(بنية Curl_easy *داده,
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
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
      infof(داده, "Failed to close libssh2 file: %d %s", rc, err_پیام);
    }
    sshc->sftp_handle = عدم;
  }
  إذا(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      infof(داده, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = عدم;
  }

  curlx_safefree(sshc->homedir);

  myssh_مقصد(داده, sshc, SSH_SESSION_DISCONNECT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_download_init(بنية Curl_easy *داده,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->مسیر,
                         curlx_uztoui(طول_رشته(sshp->مسیر)),
                         LIBSSH2_FXF_READ, (دراز)داده->مجموعه.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  إذا(!sshc->sftp_handle) {
    بیعلامت دراز sftperr;
    إذا(libssh2_session_آخرین_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      أرجع CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_آخرین_error(sshc->sftp_session);
    failf(داده, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    أرجع sftp_libssh2_error_مقصد_CURLE(sftperr);
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_DOWNLOAD_STAT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_upload_init(بنية Curl_easy *داده,
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
    libssh2_scp_send64(sshc->ssh_session, sshp->مسیر,
                       (صحيح)داده->مجموعه.new_file_perms,
                       (libssh2_int64_t)داده->state.infilesize, 0, 0);
  إذا(!sshc->ssh_channel) {
    صحيح ssh_err;
    حرف *err_پیام = عدم;
    CURLcode نتیجه;
    إذا(libssh2_session_آخرین_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    ssh_err = libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
    failf(داده, "%s", err_پیام);
    myssh_مقصد(داده, sshc, SSH_SCP_CHANNEL_FREE);
    نتیجه = libssh2_session_error_مقصد_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    إذا(نتیجه == CURLE_SSH ||
       نتیجه == CURLE_REMOTE_FILE_NOT_یافت)
      نتیجه = CURLE_UPLOAD_FAILED;
    أرجع نتیجه;
  }

  /* upload data */
  داده->req.اندازه = داده->state.infilesize;
  Curl_pgrs_مجموعه_Upload_اندازه(داده, داده->state.infilesize);
  Curl_xfer_راهاندازی_send(داده, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  داده->conn->recv_idx = FIRSTSOCKET;

  myssh_مقصد(داده, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_session_disconnect(بنية Curl_easy *داده,
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
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
      infof(داده, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیام);
    }
    sshc->ssh_channel = عدم;
  }

  إذا(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
      infof(داده, "Failed to disconnect libssh2 session: %d %s", rc, err_پیام);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_مقصد(داده, sshc, SSH_SESSION_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode sshc_cleanup(بنية ssh_conn *sshc, بنية Curl_easy *داده,
                             bool بلوک)
{
  صحيح rc;

  إذا(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = عدم;
  }

  إذا(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    إذا((rc < 0) && داده) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
      infof(داده, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_پیام);
    }
    إذا(!بلوک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = عدم;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_عنصر_خنثی = عدم;
    sshc->sshagent_prev_عنصر_خنثی = عدم;
  }

  إذا(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    إذا((rc < 0) && داده) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
      infof(داده, "Failed to close libssh2 file: %d %s", rc, err_پیام);
    }
    إذا(!بلوک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->sftp_handle = عدم;
  }

  إذا(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    إذا((rc < 0) && داده) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
      infof(داده, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیام);
    }
    إذا(!بلوک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->ssh_channel = عدم;
  }

  إذا(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    إذا((rc < 0) && داده) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
      infof(داده, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_پیام);
    }
    إذا(!بلوک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->sftp_session = عدم;
  }

  إذا(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    إذا((rc < 0) && داده) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session, &err_پیام, عدم, 0);
      infof(داده, "Failed to free libssh2 session: %d %s", rc, err_پیام);
    }
    إذا(!بلوک && (rc == LIBSSH2_ERROR_EAGAIN))
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

ساكن CURLcode ssh_state_sftp_getinfo(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc)
{
  إذا(داده->مجموعه.get_filetime)
    myssh_مقصد(داده, sshc, SSH_SFTP_FILETIME);
  وإلا
    myssh_مقصد(داده, sshc, SSH_SFTP_TRANS_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_filetime(بنية Curl_easy *داده,
                                        بنية ssh_conn *sshc,
                                        بنية SSHPROTO *sshp)
{
  LIBSSH2_SFTP_صفات attrs;
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->مسیر,
                            curlx_uztoui(طول_رشته(sshp->مسیر)),
                            LIBSSH2_SFTP_STAT, &attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0)
    داده->info.filetime = (time_t)attrs.mtime;

  myssh_مقصد(داده, sshc, SSH_SFTP_TRANS_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_trans_init(بنية Curl_easy *داده,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  إذا(داده->state.upload)
    myssh_مقصد(داده, sshc, SSH_SFTP_UPLOAD_INIT);
  وإلا إذا(sshp) {
    size_t plen = طول_رشته(sshp->مسیر);
    إذا(plen && (sshp->مسیر[plen - 1] == '/'))
      myssh_مقصد(داده, sshc, SSH_SFTP_READDIR_INIT);
    وإلا
      myssh_مقصد(داده, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  وإلا
    أرجع CURLE_FAILED_INIT;
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_upload_init(بنية Curl_easy *داده,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp,
                                           bool *بلوک)
{
  CURLcode نتیجه;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  نتیجه = sftp_upload_init(داده, sshc, sshp, بلوک);
  إذا(نتیجه) {
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_sftp_create_dirs_init(بنية Curl_easy *داده,
                                                بنية ssh_conn *sshc,
                                                بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  إذا(طول_رشته(sshp->مسیر) > 1) {
    sshc->slash_pos = sshp->مسیر + 1; /* ignore the leading '/' */
    myssh_مقصد(داده, sshc, SSH_SFTP_CREATE_DIRS);
  }
  وإلا
    myssh_مقصد(داده, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_create_dirs(بنية Curl_easy *داده,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  إذا(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(داده, "SFTP: creating directory '%s'", sshp->مسیر);
    myssh_مقصد(داده, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    أرجع CURLE_OK;
  }
  myssh_مقصد(داده, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc,
                                       بنية SSHPROTO *sshp,
                                       bool *بلوک)
{
  CURLcode نتیجه;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  نتیجه = sftp_readdir(داده, sshc, sshp, بلوک);
  إذا(نتیجه)
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_sftp_readdir_bottom(بنية Curl_easy *داده,
                                              بنية ssh_conn *sshc,
                                              بنية SSHPROTO *sshp)
{
  CURLcode نتیجه;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  نتیجه = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  إذا(!نتیجه)
    نتیجه = Curl_client_write(داده, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  إذا(نتیجه) {
    curlx_dyn_free(&sshp->readdir);
    myssh_مقصد(داده, sshc, SSH_STOP);
  }
  وإلا {
    curlx_dyn_reset(&sshp->readdir);
    myssh_مقصد(داده, sshc, SSH_SFTP_READDIR);
  }
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_sftp_readdir_انجامشده(بنية Curl_easy *داده,
                                            بنية ssh_conn *sshc)
{
  إذا(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  sshc->sftp_handle = عدم;

  /* no data to transfer */
  Curl_xfer_راهاندازی_nop(داده);
  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_download_stat(بنية Curl_easy *داده,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp,
                                             bool *بلوک)
{
  CURLcode نتیجه;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  نتیجه = sftp_download_stat(داده, sshc, sshp, بلوک);
  إذا(نتیجه) {
    myssh_مقصد(داده, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع نتیجه;
}

ساكن CURLcode ssh_state_scp_trans_init(بنية Curl_easy *داده,
                                         بنية ssh_conn *sshc,
                                         بنية SSHPROTO *sshp)
{
  CURLcode نتیجه;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  نتیجه = Curl_getworkingpath(داده, sshc->homedir,
                               &sshp->مسیر);
  إذا(نتیجه) {
    myssh_مقصد(داده, sshc, SSH_STOP);
    أرجع نتیجه;
  }

  إذا(داده->state.upload) {
    إذا(داده->state.infilesize < 0) {
      failf(داده, "SCP requires a known file size for upload");
      myssh_مقصد(داده, sshc, SSH_SCP_CHANNEL_FREE);
      أرجع CURLE_UPLOAD_FAILED;
    }
    myssh_مقصد(داده, sshc, SSH_SCP_UPLOAD_INIT);
  }
  وإلا
    myssh_مقصد(داده, sshc, SSH_SCP_DOWNLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_انجامشده(بنية Curl_easy *داده,
                                   بنية ssh_conn *sshc)
{
  إذا(داده->state.upload)
    myssh_مقصد(داده, sshc, SSH_SCP_SEND_EOF);
  وإلا
    myssh_مقصد(داده, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_send_eof(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_send_eof(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session,
                                       &err_پیام, عدم, 0);
      infof(داده, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_پیام);
    }
  }
  myssh_مقصد(داده, sshc, SSH_SCP_WAIT_EOF);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_wait_eof(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session,
                                       &err_پیام, عدم, 0);
      infof(داده, "Failed to get channel EOF: %d %s", rc, err_پیام);
    }
  }
  myssh_مقصد(داده, sshc, SSH_SCP_WAIT_CLOSE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_wait_close(بنية Curl_easy *داده,
                                         بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session,
                                       &err_پیام, عدم, 0);
      infof(داده, "Channel failed to close: %d %s", rc, err_پیام);
    }
  }
  myssh_مقصد(داده, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_channel_free(
  بنية Curl_easy *داده,
  بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_free(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc < 0) {
      حرف *err_پیام = عدم;
      (فراغ)libssh2_session_آخرین_error(sshc->ssh_session,
                                       &err_پیام, عدم, 0);
      infof(داده, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیام);
    }
    sshc->ssh_channel = عدم;
  }
  CURL_TRC_SSH(داده, "SCP DONE phase complete");
  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_session_free(بنية Curl_easy *داده,
                                       بنية ssh_conn *sshc)
{
  CURLcode نتیجه = sshc_cleanup(sshc, داده, FALSE);
  بنية connectdata *conn = داده->conn;
  إذا(نتیجه)
    أرجع نتیجه;
  memset(sshc, 0, حجم(بنية ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_مقصد(داده, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ساكن CURLcode ssh_statemachine(بنية Curl_easy *داده,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp,
                                 bool *بلوک)
{
  CURLcode نتیجه = CURLE_OK;
  *بلوک = 0; /* we are not blocking by default */

  افعل {
    بدل(sshc->state) {
    حالة SSH_INIT:
      نتیجه = ssh_state_init(داده, sshc);
      إذا(نتیجه)
        اكسر;
      FALLTHROUGH();

    حالة SSH_S_STARTUP:
      نتیجه = ssh_state_startup(داده, sshc);
      إذا(نتیجه)
        اكسر;
      FALLTHROUGH();

    حالة SSH_HOSTKEY:
      نتیجه = ssh_state_hostkey(داده, sshc);
      اكسر;

    حالة SSH_AUTHLIST:
      نتیجه = ssh_state_authlist(داده, sshc);
      اكسر;

    حالة SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(داده, sshc);
      اكسر;

    حالة SSH_AUTH_PKEY:
      نتیجه = ssh_state_auth_pkey(داده, sshc);
      اكسر;

    حالة SSH_AUTH_PASS_INIT:
      نتیجه = ssh_state_auth_pass_init(داده, sshc);
      اكسر;

    حالة SSH_AUTH_PASS:
      نتیجه = ssh_state_auth_pass(داده, sshc);
      اكسر;

    حالة SSH_AUTH_HOST_INIT:
      نتیجه = ssh_state_auth_host_init(داده, sshc);
      اكسر;

    حالة SSH_AUTH_HOST:
      myssh_مقصد(داده, sshc, SSH_AUTH_AGENT_INIT);
      اكسر;

    حالة SSH_AUTH_AGENT_INIT:
      نتیجه = ssh_state_auth_agent_init(داده, sshc);
      اكسر;

    حالة SSH_AUTH_AGENT_LIST:
      نتیجه = ssh_state_auth_agent_list(داده, sshc);
      اكسر;

    حالة SSH_AUTH_AGENT:
      نتیجه = ssh_state_auth_agent(داده, sshc);
      اكسر;

    حالة SSH_AUTH_کلید_INIT:
      نتیجه = ssh_state_auth_کلید_init(داده, sshc);
      اكسر;

    حالة SSH_AUTH_کلید:
      نتیجه = ssh_state_auth_کلید(داده, sshc);
      اكسر;

    حالة SSH_AUTH_انجامشده:
      نتیجه = ssh_state_auth_انجامشده(داده, sshc);
      اكسر;

    حالة SSH_SFTP_INIT:
      نتیجه = ssh_state_sftp_init(داده, sshc);
      اكسر;

    حالة SSH_SFTP_REALPATH:
      نتیجه = ssh_state_sftp_realpath(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_QUOTE_INIT:
      نتیجه = ssh_state_sftp_quote_init(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_POSTQUOTE_INIT:
      نتیجه = ssh_state_sftp_postquote_init(داده, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE:
      نتیجه = ssh_state_sftp_quote(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_بعدی_QUOTE:
      نتیجه = ssh_state_sftp_بعدی_quote(داده, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_STAT:
      نتیجه = ssh_state_sftp_quote_stat(داده, sshc, sshp, بلوک);
      اكسر;

    حالة SSH_SFTP_QUOTE_SETSTAT:
      نتیجه = ssh_state_sftp_quote_setstat(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_QUOTE_SYMLINK:
      نتیجه = ssh_state_sftp_quote_symlink(داده, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_MKDIR:
      نتیجه = ssh_state_sftp_quote_mkdir(داده, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_RENAME:
      نتیجه = ssh_state_sftp_quote_rename(داده, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_RMDIR:
      نتیجه = ssh_state_sftp_quote_rmdir(داده, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_UNLINK:
      نتیجه = ssh_state_sftp_quote_unlink(داده, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_STATVFS:
      نتیجه = ssh_state_sftp_quote_statvfs(داده, sshc);
      اكسر;

    حالة SSH_SFTP_GETINFO:
      نتیجه = ssh_state_sftp_getinfo(داده, sshc);
      اكسر;

    حالة SSH_SFTP_FILETIME:
      نتیجه = ssh_state_sftp_filetime(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_TRANS_INIT:
      نتیجه = ssh_state_sftp_trans_init(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_UPLOAD_INIT:
      نتیجه = ssh_state_sftp_upload_init(داده, sshc, sshp, بلوک);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS_INIT:
      نتیجه = ssh_state_sftp_create_dirs_init(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS:
      نتیجه = ssh_state_sftp_create_dirs(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS_MKDIR:
      نتیجه = ssh_state_sftp_create_dirs_mkdir(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_INIT:
      نتیجه = ssh_state_sftp_readdir_init(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR:
      نتیجه = ssh_state_sftp_readdir(داده, sshc, sshp, بلوک);
      اكسر;

    حالة SSH_SFTP_READDIR_LINK:
      نتیجه = ssh_state_sftp_readdir_link(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_BOTTOM:
      نتیجه = ssh_state_sftp_readdir_bottom(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_انجامشده:
      نتیجه = ssh_state_sftp_readdir_انجامشده(داده, sshc);
      اكسر;

    حالة SSH_SFTP_DOWNLOAD_INIT:
      نتیجه = ssh_state_sftp_download_init(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_DOWNLOAD_STAT:
      نتیجه = ssh_state_sftp_download_stat(داده, sshc, sshp, بلوک);
      اكسر;

    حالة SSH_SFTP_CLOSE:
      نتیجه = ssh_state_sftp_close(داده, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_SHUTDOWN:
      نتیجه = ssh_state_sftp_shutdown(داده, sshc);
      اكسر;

    حالة SSH_SCP_TRANS_INIT:
      نتیجه = ssh_state_scp_trans_init(داده, sshc, sshp);
      اكسر;

    حالة SSH_SCP_UPLOAD_INIT:
      نتیجه = ssh_state_scp_upload_init(داده, sshc, sshp);
      اكسر;

    حالة SSH_SCP_DOWNLOAD_INIT:
      نتیجه = ssh_state_scp_download_init(داده, sshc, sshp);
      اكسر;

    حالة SSH_SCP_انجامشده:
      نتیجه = ssh_state_scp_انجامشده(داده, sshc);
      اكسر;

    حالة SSH_SCP_SEND_EOF:
      نتیجه = ssh_state_scp_send_eof(داده, sshc);
      اكسر;

    حالة SSH_SCP_WAIT_EOF:
      نتیجه = ssh_state_scp_wait_eof(داده, sshc);
      اكسر;

    حالة SSH_SCP_WAIT_CLOSE:
      نتیجه = ssh_state_scp_wait_close(داده, sshc);
      اكسر;

    حالة SSH_SCP_CHANNEL_FREE:
      نتیجه = ssh_state_scp_channel_free(داده, sshc);
      اكسر;

    حالة SSH_SESSION_DISCONNECT:
      نتیجه = ssh_state_session_disconnect(داده, sshc);
      اكسر;

    حالة SSH_SESSION_FREE:
      نتیجه = ssh_state_session_free(داده, sshc);
      اكسر;

    حالة SSH_QUIT:
    افتراضي:
      /* internal error */
      myssh_مقصد(داده, sshc, SSH_STOP);
      اكسر;
    }

  } طالما(!نتیجه && (sshc->state != SSH_STOP) && !*بلوک);

  إذا(نتیجه == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *بلوک = TRUE;
    نتیجه = CURLE_OK;
  }
  CURL_TRC_SSH(داده, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحيح)نتیجه, *بلوک);

  أرجع نتیجه;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ساكن فراغ ssh_block2waitfor(بنية Curl_easy *داده,
                              بنية ssh_conn *sshc,
                              bool بلوک)
{
  صحيح dir = 0;
  (فراغ)داده;
  إذا(بلوک) {
    dir = libssh2_session_بلوک_directions(sshc->ssh_session);
    إذا(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_بلوک_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_بلوک_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  إذا(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode ssh_multi_statemach(بنية Curl_easy *داده, bool *انجامشده)
{
  بنية connectdata *conn = داده->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(داده, CURL_META_SSH_EASY);
  CURLcode نتیجه = CURLE_OK;
  bool بلوک; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;

  افعل {
    نتیجه = ssh_statemachine(داده, sshc, sshp, &بلوک);
    *انجامشده = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } طالما(!نتیجه && !*انجامشده && !بلوک);
  ssh_block2waitfor(داده, sshc, بلوک);

  أرجع نتیجه;
}

ساكن CURLcode ssh_بلوک_statemach(بنية Curl_easy *داده,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode نتیجه = CURLE_OK;
  بنية curltime شروع = *Curl_pgrs_now(داده);

  طالما((sshc->state != SSH_STOP) && !نتیجه) {
    bool بلوک;
    timediff_t چپ_ms = 1000;

    نتیجه = ssh_statemachine(داده, sshc, sshp, &بلوک);
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
    وإلا إذا(curlx_ptimediff_ms(Curl_pgrs_now(داده), &شروع) > 1000) {
      /* disconnect timeout */
      failf(داده, "Disconnect timed out");
      نتیجه = CURLE_OK;
      اكسر;
    }

    إذا(بلوک) {
      صحيح dir = libssh2_session_بلوک_directions(sshc->ssh_session);
      curl_socket_t sock = داده->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      إذا(LIBSSH2_SESSION_بلوک_INBOUND & dir)
        fd_read = sock;
      إذا(LIBSSH2_SESSION_بلوک_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (فراغ)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
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
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ساكن فراغ myssh_conn_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية ssh_conn *sshc = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  sshc_cleanup(sshc, عدم, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ساكن CURLcode ssh_راهاندازی_connection(بنية Curl_easy *داده,
                                     بنية connectdata *conn)
{
  بنية ssh_conn *sshc;
  بنية SSHPROTO *sshp;
  (فراغ)conn;

  sshc = curlx_calloc(1, حجم(*sshc));
  إذا(!sshc)
    أرجع CURLE_بیرون_از_حافظه;

  إذا(Curl_conn_meta_مجموعه(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  sshp = curlx_calloc(1, حجم(*sshp));
  إذا(!sshp)
    أرجع CURLE_بیرون_از_حافظه;

  curlx_dyn_init(&sshp->readdir, CURL_مسیر_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_مسیر_MAX);
  إذا(Curl_meta_مجموعه(داده, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    أرجع CURLE_بیرون_از_حافظه;

  أرجع Curl_ssh_راهاندازی_pkey(داده, sshc);
}

ساكن Curl_recv scp_recv, sftp_recv;
ساكن Curl_send scp_send, sftp_send;

#اگر_تعریف_نشده CURL_DISABLE_PROXY
ساكن ssize_t ssh_tls_recv(libssh2_socket_t sock, فراغ *میانگیر,
                            size_t درازی, صحيح flags, فراغ **abstract)
{
  بنية Curl_easy *داده = (بنية Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(داده, sock);
  size_t nread;
  CURLcode نتیجه;
  بنية connectdata *conn = داده->conn;
  Curl_recv *backup = conn->recv[sockindex];
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (فراغ)flags;

  إذا(!sshc)
    أرجع -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  نتیجه = Curl_conn_recv(داده, sockindex, میانگیر, درازی, &nread);
  conn->recv[sockindex] = backup;
  إذا(نتیجه == CURLE_AGAIN)
    أرجع -EAGAIN; /* magic return code for libssh2 */
  وإلا إذا(نتیجه)
    أرجع -1; /* generic error */
  Curl_debug(داده, CURLINFO_داده_درون, (ثابت حرف *)میانگیر, nread);
  أرجع (ssize_t)nread;
}

ساكن ssize_t ssh_tls_send(libssh2_socket_t sock, ثابت فراغ *میانگیر,
                            size_t درازی, صحيح flags, فراغ **abstract)
{
  بنية Curl_easy *داده = (بنية Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(داده, sock);
  size_t nwrite;
  CURLcode نتیجه;
  بنية connectdata *conn = داده->conn;
  Curl_send *backup = conn->send[sockindex];
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (فراغ)flags;

  إذا(!sshc)
    أرجع -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  نتیجه = Curl_conn_send(داده, sockindex, میانگیر, درازی, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  إذا(نتیجه == CURLE_AGAIN)
    أرجع -EAGAIN; /* magic return code for libssh2 */
  وإلا إذا(نتیجه)
    أرجع -1; /* error */
  Curl_debug(داده, CURLINFO_داده_بیرون, (ثابت حرف *)میانگیر, nwrite);
  أرجع (ssize_t)nwrite;
}
#پایان_اگر

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ساكن CURLcode ssh_connect(بنية Curl_easy *داده, bool *انجامشده)
{
#اگر_تعریف_شده CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#پایان_اگر
  بنية connectdata *conn = داده->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode نتیجه;

#إذا LIBSSH2_نسخه_رقم >= 0x010b00
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
      infof(داده, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#پایان_اگر

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  infof(داده, "SSH: user '%s'", Curl_creds_user(conn->creds));
#اگر_تعریف_شده CURL_LIBSSH2_DEBUG
  infof(داده, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#پایان_اگر /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, داده);

  إذا(!sshc->ssh_session) {
    failf(داده, "Failure initializing ssh session");
    أرجع CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#إذا LIBSSH2_نسخه_رقم >= 0x010B00
  إذا(داده->مجموعه.server_response_timeout > 0) {
    libssh2_session_مجموعه_read_timeout(sshc->ssh_session,
                             (دراز)(داده->مجموعه.server_response_timeout / 1000));
  }
#پایان_اگر

#اگر_تعریف_نشده CURL_DISABLE_PROXY
  إذا(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#إذا LIBSSH2_نسخه_رقم >= 0x010b01
    infof(داده, "SSH: using HTTPS proxy");
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic افزودن
#رهنمود clang diagnostic ignored "-Wcast-function-type-strict"
#پایان_اگر
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic برداشت
#پایان_اگر
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

    infof(داده, "SSH: using HTTPS proxy");
    libssh2_session_callback_مجموعه(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_مجموعه(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#پایان_اگر

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#پایان_اگر /* CURL_DISABLE_PROXY */
  إذا(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  وإلا {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  إذا(داده->مجموعه.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(داده, "SSH: failed to enable compression for session");
  }

  إذا(CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS)) {
    صحيح rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    إذا(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = عدم;
      أرجع CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    إذا(rc < 0)
      infof(داده, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(داده, STRING_SSH_KNOWNHOSTS));
  }

#اگر_تعریف_شده CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(داده, "SSH: socket %d", (صحيح)sock);
#پایان_اگر

  myssh_مقصد(داده, sshc, SSH_INIT);

  نتیجه = ssh_multi_statemach(داده, انجامشده);

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
  myssh_مقصد(داده, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  نتیجه = ssh_multi_statemach(داده, dophase_انجامشده);

  *connected = Curl_conn_is_connected(داده->conn, FIRSTSOCKET);

  إذا(*dophase_انجامشده) {
    CURL_TRC_SSH(داده, "DO phase is complete");
  }

  أرجع نتیجه;
}

/* called from multi.c while DOing */
ساكن CURLcode scp_doing(بنية Curl_easy *داده,
                          bool *dophase_انجامشده)
{
  CURLcode نتیجه;
  نتیجه = ssh_multi_statemach(داده, dophase_انجامشده);

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
    نتیجه = ssh_بلوک_statemach(داده, sshc, sshp, TRUE);
  }

  إذا(sshc)
    أرجع sshc_cleanup(sshc, داده, TRUE);
  أرجع نتیجه;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ساكن CURLcode ssh_انجامشده(بنية Curl_easy *داده, CURLcode وضعیت)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(داده->conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(داده, CURL_META_SSH_EASY);
  CURLcode نتیجه = CURLE_OK;

  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;

  إذا(!وضعیت)
    /* run the state-machine */
    نتیجه = ssh_بلوک_statemach(داده, sshc, sshp, FALSE);
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

  إذا(sshc && !وضعیت)
    myssh_مقصد(داده, sshc, SSH_SCP_انجامشده);

  أرجع ssh_انجامشده(داده, وضعیت);
}

ساكن CURLcode scp_send(بنية Curl_easy *داده, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  بنية connectdata *conn = داده->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode نتیجه = CURLE_OK;
  ssize_t nwritten;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  (فراغ)eos;
  *pnwritten = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ثابت حرف *)mem, len);

  ssh_block2waitfor(داده, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  إذا(nwritten == LIBSSH2_ERROR_EAGAIN)
    نتیجه = CURLE_AGAIN;
  وإلا إذا(nwritten < LIBSSH2_ERROR_NONE)
    نتیجه = libssh2_session_error_مقصد_CURLE((صحيح)nwritten);
  وإلا
    *pnwritten = (size_t)nwritten;

  أرجع نتیجه;
}

ساكن CURLcode scp_recv(بنية Curl_easy *داده, int8_t sockindex,
                         حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = داده->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode نتیجه = CURLE_OK;
  ssize_t nread;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(داده, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  إذا(nread == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nread < LIBSSH2_ERROR_NONE)
    نتیجه = libssh2_session_error_مقصد_CURLE((صحيح)nread);
  وإلا
    *pnread = (size_t)nread;

  أرجع نتیجه;
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
  نتیجه = ssh_multi_statemach(داده, dophase_انجامشده);

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
  CURLcode نتیجه = ssh_multi_statemach(داده, dophase_انجامشده);

  إذا(*dophase_انجامشده) {
    CURL_TRC_SSH(داده, "DO phase is complete");
  }
  أرجع نتیجه;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode sftp_disconnect(بنية Curl_easy *داده,
                                بنية connectdata *conn, bool dead_connection)
{
  CURLcode نتیجه = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(داده, CURL_META_SSH_EASY);
  (فراغ)dead_connection;

  إذا(sshc) {
    إذا(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(داده, "DISCONNECT starts now");
      myssh_مقصد(داده, sshc, SSH_SFTP_SHUTDOWN);
      نتیجه = ssh_بلوک_statemach(داده, sshc, sshp, TRUE);
      CURL_TRC_SSH(داده, "DISCONNECT is done -> %d", (صحيح)نتیجه);
    }
    sshc_cleanup(sshc, داده, TRUE);
  }
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
  أرجع ssh_انجامشده(داده, وضعیت);
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

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ثابت حرف *)mem, len);

  ssh_block2waitfor(داده, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  إذا(nwrite == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nwrite < LIBSSH2_ERROR_NONE)
    أرجع libssh2_session_error_مقصد_CURLE((صحيح)nwrite);
  *pnwritten = (size_t)nwrite;
  أرجع CURLE_OK;
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

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(داده, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  إذا(nread == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nread < 0)
    أرجع libssh2_session_error_مقصد_CURLE((صحيح)nread);

  *pnread = (size_t)nread;
  أرجع CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
ساكن CURLcode ssh_do(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه;
  bool connected = FALSE;
  بنية connectdata *conn = داده->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *انجامشده = FALSE; /* default to false */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  داده->req.اندازه = -1; /* make sure this is unknown at this point */
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

فراغ Curl_ssh_نسخه(حرف *میانگیر, size_t buflen)
{
  (فراغ)curl_msnprintf(میانگیر, buflen, "libssh2/%s", libssh2_نسخه(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ساكن فراغ ssh_attach(بنية Curl_easy *داده, بنية connectdata *conn)
{
  DEBUGASSERT(داده);
  DEBUGASSERT(conn);
  إذا(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    إذا(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      فراغ **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = داده;
    }
  }
}

/*
 * SCP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_scp = {
  ssh_راهاندازی_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_انجامشده,                             /* done */
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
  ssh_راهاندازی_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  sftp_انجامشده,                            /* done */
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

#پایان_اگر /* USE_LIBSSH2 */
