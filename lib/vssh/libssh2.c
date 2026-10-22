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

#که_تعریف_شوی USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

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
#ضمن "parsedate.h" /* for the week day and month names */
#ضمن "multiif.h"
#ضمن "select.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "vssh/vssh.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/base64.h" /* for curlx_base64_encode() */

ساكن ثابت حرف *sftp_libssh2_strerror(بینښه اوږد err)
{
  بدل(err) {
  حالة LIBSSH2_FX_NO_SUCH_FILE:
    أرجع "No such file or directory";

  حالة LIBSSH2_FX_PERMISSION_DENIED:
    أرجع "Permission denied";

  حالة LIBSSH2_FX_FAILURE:
    أرجع "Operation failed";

  حالة LIBSSH2_FX_BAD_پیغام:
    أرجع "Bad message from SFTP server";

  حالة LIBSSH2_FX_NO_CONNECTION:
    أرجع "Not connected to SFTP server";

  حالة LIBSSH2_FX_CONNECTION_ورک:
    أرجع "Connection to SFTP server lost";

  حالة LIBSSH2_FX_OP_UNSUPPORTED:
    أرجع "Operation not supported by SFTP server";

  حالة LIBSSH2_FX_INVALID_HANDLE:
    أرجع "Invalid handle";

  حالة LIBSSH2_FX_NO_SUCH_لار:
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

  حالة LIBSSH2_FX_قلف_CONFLICT:
    أرجع "File lock conflict";

  حالة LIBSSH2_FX_DIR_NOT_تشه:
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
  بنية Curl_easy *معلومات = (بنية Curl_easy *)*abstract;

#که_تعریف_شوی CURL_LIBSSH2_DEBUG
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
#د_که_پای /* CURL_LIBSSH2_DEBUG */
  إذا(عدد_prompts == 1) {
    بنية connectdata *conn = معلومات->conn;
    ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].متن_3 = Curl_cstrdup(passwd);
    responses[0].اوږدتیا = responses[0].متن_3 ? curlx_uztoui(مزي_اوږدوالی(passwd)) : 0;
  }
  (فراغ)prompts;
} /* kbd_callback */

ساكن CURLcode sftp_libssh2_error_منزل_CURLE(بینښه اوږد err)
{
  بدل(err) {
  حالة LIBSSH2_FX_OK:
    أرجع CURLE_OK;

  حالة LIBSSH2_FX_NO_SUCH_FILE:
  حالة LIBSSH2_FX_NO_SUCH_لار:
    أرجع CURLE_REMOTE_FILE_NOT_وموندل;

  حالة LIBSSH2_FX_PERMISSION_DENIED:
  حالة LIBSSH2_FX_WRITE_PROTECT:
  حالة LIBSSH2_FX_قلف_CONFLICT:
    أرجع CURLE_REMOTE_ACCESS_DENIED;

  حالة LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  حالة LIBSSH2_FX_QUOTA_EXCEEDED:
    أرجع CURLE_REMOTE_DISK_ډک;

  حالة LIBSSH2_FX_FILE_ALREADY_EXISTS:
    أرجع CURLE_REMOTE_FILE_EXISTS;

  حالة LIBSSH2_FX_DIR_NOT_تشه:
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
    أرجع CURLE_REMOTE_FILE_NOT_وموندل;

  حالة LIBSSH2_ERROR_SOCKET_NONE:
    أرجع CURLE_COULDNT_CONNECT;

  حالة LIBSSH2_ERROR_ALLOC:
    أرجع CURLE_بهر_د_حافظه;

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
  أرجع Curl_cmalloc(شمېر);
}

ساكن LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (فراغ)abstract;
  أرجع Curl_crealloc(ptr, شمېر);
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
                          تعداد curl_khmatch سمون,
                          فراغ *clientp)
{
  (فراغ)easy;
  (فراغ)knownkey;
  (فراغ)foundkey;
  (فراغ)clientp;

  /* we only allow perfect matches, and we reject everything else */
  أرجع (سمون != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ساكن تعداد curl_khtype convert_ssh2_keytype(صحيح sshkeytype)
{
  تعداد curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  بدل(sshkeytype) {
  حالة LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    اكسر;
#که_تعریف_شوی LIBSSH2_HOSTKEY_TYPE_DSS
  حالة LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    اكسر;
#د_که_پای
#که_تعریف_شوی LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#د_که_پای
#که_تعریف_شوی LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#د_که_پای
#که_تعریف_شوی LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#د_که_پای
#که_تعریف_شوی LIBSSH2_HOSTKEY_TYPE_ED25519
  حالة LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    اكسر;
#د_که_پای
  }
  أرجع keytype;
}

ساكن CURLcode ssh_knownhost(بنية Curl_easy *معلومات,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = معلومات->conn;
  بنية libssh2_knownhost *host = عدم;
  ثابت حرف *remotekey = عدم;
  صحيح keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  صحيح keybit = 0;
  صحيح sshkeytype = 0;
  size_t keylen = 0;
  صحيح rc = 0;
  CURLcode پایله = CURLE_OK;

  إذا(!CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS)) {
    infof(معلومات, "SSH: no knownhosts file configured");
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
      معلومات->سېټ.ssh_keyfunc ? معلومات->سېټ.ssh_keyfunc : sshkeycallback;
    بنية curl_khkey knownkey;
    بنية curl_khkey *knownkeyp = عدم;
    بنية curl_khkey foundkey;

    بدل(sshkeytype) {
    حالة LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_کلی_SSHRSA;
      اكسر;
#که_تعریف_شوی LIBSSH2_HOSTKEY_TYPE_DSS
    حالة LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_کلی_SSHDSS;
      اكسر;
#د_که_پای
    حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_کلی_ECDSA_256;
      اكسر;
    حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_کلی_ECDSA_384;
      اكسر;
    حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_کلی_ECDSA_521;
      اكسر;
    حالة LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_کلی_ED25519;
      اكسر;
    افتراضي:
      infof(معلومات, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      اكسر;
    }
    إذا(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    وإلا {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->پورټ != پورټ_SSH) ?
                                          conn->origin->پورټ : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(معلومات, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->کلی : "<none>");

      /* setup 'knownkey' */
      إذا(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.کلی = host->کلی;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.کلی = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (تعداد curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_پیل(&guard, معلومات, easy_ssh_keyfunc);
      rc = func(معلومات, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, معلومات->سېټ.ssh_keyfunc_userp);
      CURL_CBAPI_پای_ټکی(&guard);
    }
  }
  وإلا {
    /* no remotekey means failure! */
    infof(معلومات, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  بدل(rc) {
  افتراضي: /* unknown return codes is the same as reject */
  حالة CURLKHSTAT_REJECT:
    infof(معلومات, "SSH: knownhost check failed");
    myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  حالة CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    پایله = CURLE_PEER_FAILED_VERIFICATION;
    اكسر;
  حالة CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    إذا(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  حالة CURLKHSTAT_FINE:
  حالة CURLKHSTAT_FINE_جمع_کړه_منزل_FILE:
    /* proceed */
    إذا(keycheck != LIBSSH2_KNOWNHOST_CHECK_سمون) {
      صحيح addrc;
      ثابت حرف *hostbuf;
      حرف *hostport = عدم;
      إذا(conn->origin->پورټ != پورټ_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->پورټ);
        إذا(!hostbuf)
          infof(معلومات, "WARNING: failed allocating buffer for [host]:port");
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
          infof(معلومات, "WARNING: adding the known host %s failed", hostbuf);
        وإلا إذا(rc == CURLKHSTAT_FINE_جمع_کړه_منزل_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          صحيح wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          إذا(wrc) {
            infof(معلومات, "WARNING: writing %s failed",
                  CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    وإلا
      infof(معلومات, "SSH: knownhost entry matches host key");
    اكسر;
  }
  أرجع پایله;
}

ساكن CURLcode ssh_check_fingerprint(بنية Curl_easy *معلومات,
                                      بنية ssh_conn *sshc)
{
  ثابت حرف *pubkey_md5 =
    CURL_EASY_STR(معلومات, STRING_SSH_HOST_عامه_کلی_MD5);
  ثابت حرف *pubkey_sha256 =
    CURL_EASY_STR(معلومات, STRING_SSH_HOST_عامه_کلی_SHA256);

  إذا(pubkey_sha256) {
    ثابت حرف *fingerprint = عدم;
    حرف *fingerprint_b64 = عدم;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(معلومات, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    إذا(!fingerprint) {
      failf(معلومات,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    إذا(curlx_base64_encode((ثابت uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    إذا(!fingerprint_b64) {
      failf(معلومات, "SHA256 fingerprint could not be encoded");
      myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(معلومات, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

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
      myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(معلومات, "SSH: SHA256 checksum match");
  }

  إذا(pubkey_md5) {
    حرف md5buffer[33];
    ثابت حرف *fingerprint;

    infof(معلومات, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    إذا(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      صحيح i;
      لكل(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (بینښه حرف)fingerprint[i]);
      }

      infof(معلومات, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    إذا(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      إذا(fingerprint) {
        failf(معلومات,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      وإلا {
        failf(معلومات,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(معلومات, "SSH: MD5 checksum match");
  }

  إذا(!pubkey_md5 && !pubkey_sha256) {
    إذا(معلومات->سېټ.ssh_hostkeyfunc) {
      size_t keylen = 0;
      صحيح sshkeytype = 0;
      صحيح rc = 0;
      /* we handle the process to the callback */
      ثابت حرف *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      إذا(remotekey) {
        بنية Curl_mapi_guard guard;
        تعداد curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_پیل(&guard, معلومات, easy_ssh_hostkeyfunc);
        rc = معلومات->سېټ.ssh_hostkeyfunc(معلومات->سېټ.ssh_hostkeyfunc_userp,
                                       (صحيح)keytype, remotekey, keylen);
        CURL_CBAPI_پای_ټکی(&guard);
        إذا(rc != CURLKHMATCH_OK) {
          myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
          failf(معلومات, "SSH: callback failed host public key verification");
          أرجع CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(معلومات, "SSH: verified public key via callback");
      }
      وإلا {
        myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
        أرجع CURLE_PEER_FAILED_VERIFICATION;
      }
      أرجع CURLE_OK;
    }
    وإلا {
      CURL_TRC_SSH(معلومات, "no host key checksum given, checking knownhosts");
      أرجع ssh_knownhost(معلومات, sshc);
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
ساكن CURLcode ssh_force_knownhost_کلی_type(بنية Curl_easy *معلومات,
                                             بنية ssh_conn *sshc)
{
  CURLcode پایله = CURLE_OK;

  ساكن ثابت حرف hostkey_لاره_ssh_ed25519[] = "ssh-ed25519";
  ساكن ثابت حرف hostkey_لاره_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ساكن ثابت حرف hostkey_لاره_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ساكن ثابت حرف hostkey_لاره_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ساكن ثابت حرف hostkey_لاره_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#که_تعریف_شوی LIBSSH2_KNOWNHOST_کلی_SSHDSS
  ساكن ثابت حرف hostkey_لاره_ssh_dss[] = "ssh-dss";
#د_که_پای
  bool وموندل = FALSE;

  إذا(sshc->kh &&
     !CURL_EASY_STR(معلومات, STRING_SSH_HOST_عامه_کلی_MD5) &&
     !CURL_EASY_STR(معلومات, STRING_SSH_HOST_عامه_کلی_SHA256)) {
    بنية libssh2_knownhost *store = عدم;
    بنية connectdata *conn = معلومات->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_کلی_ماسک >>
                      LIBSSH2_KNOWNHOST_کلی_ښويدنه) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    طالما(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      إذا(store) {
        إذا(store->name) {
          إذا(store->name[0] == '[') {
            curl_off_t پورټ;
            size_t kh_name_اندازه = 0;
            ثابت حرف *p;
            ثابت حرف *kh_name_پای_ټکی = مزي_پلټنه(store->name, "]:");
            إذا(!kh_name_پای_ټکی) {
              infof(معلومات, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS));
              استمر;
            }
            p = kh_name_پای_ټکی + 2; /* start of port number */
            إذا(!curlx_str_شمېره(&p, &پورټ, 0xffff) &&
               (kh_name_پای_ټکی && (پورټ == conn->origin->پورټ))) {
              kh_name_اندازه = مزي_اوږدوالی(store->name) - 1 - مزي_اوږدوالی(kh_name_پای_ټکی);
              إذا((kh_name_اندازه == مزي_اوږدوالی(conn->origin->hostname)) &&
                 !مزي_پرتله(store->name + 1, conn->origin->hostname,
                          kh_name_اندازه)) {
                وموندل = TRUE;
                اكسر;
              }
            }
          }
          وإلا إذا(!strcmp(store->name, conn->origin->hostname)) {
            وموندل = TRUE;
            اكسر;
          }
        }
        وإلا إذا(store->کلی) {
          بینښه صحيح type = (store->typemask & LIBSSH2_KNOWNHOST_کلی_ماسک)
                              >> LIBSSH2_KNOWNHOST_کلی_ښويدنه;
          إذا(!absent_type[type]) {
            صحيح keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->پورټ != پورټ_SSH) ? conn->origin->پورټ : -1,
              store->کلی, مزي_اوږدوالی(store->کلی),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_کلی_ماسک),
              عدم);
            إذا(keycheck == LIBSSH2_KNOWNHOST_CHECK_سمون) {
              وموندل = TRUE;
              اكسر;
            }
            وإلا إذا(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    إذا(وموندل) {
      صحيح rc;
      ثابت حرف *hostkey_لاره = عدم;
      infof(معلومات, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS));

      بدل(store->typemask & LIBSSH2_KNOWNHOST_کلی_ماسک) {
      حالة LIBSSH2_KNOWNHOST_کلی_ED25519:
        hostkey_لاره = hostkey_لاره_ssh_ed25519;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلی_ECDSA_521:
        hostkey_لاره = hostkey_لاره_ssh_ecdsa_521;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلی_ECDSA_384:
        hostkey_لاره = hostkey_لاره_ssh_ecdsa_384;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلی_ECDSA_256:
        hostkey_لاره = hostkey_لاره_ssh_ecdsa_256;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کلی_SSHRSA:
        hostkey_لاره = hostkey_لاره_ssh_rsa_all;
        اكسر;
#که_تعریف_شوی LIBSSH2_KNOWNHOST_کلی_SSHDSS
      حالة LIBSSH2_KNOWNHOST_کلی_SSHDSS:  /* deprecated upstream */
        hostkey_لاره = hostkey_لاره_ssh_dss;
        اكسر;
#د_که_پای
      حالة LIBSSH2_KNOWNHOST_کلی_RSA1:
        failf(معلومات, "Found host key type RSA1 which is not supported");
        أرجع CURLE_SSH;
      افتراضي:
        failf(معلومات, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_کلی_ماسک));
        أرجع CURLE_SSH;
      }

      infof(معلومات, "SSH: set '%s' as hostkey type", hostkey_لاره);
      rc = libssh2_session_لاره_pref(sshc->ssh_session,
                                       LIBSSH2_لاره_HOSTKEY, hostkey_لاره);
      إذا(rc) {
        حرف *err_پیغام = عدم;
        صحيح errlen;
        libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, &errlen, 0);
        failf(معلومات, "libssh2 method '%s' failed: %s", hostkey_لاره, err_پیغام);
        پایله = libssh2_session_error_منزل_CURLE(rc);
      }
    }
    وإلا {
      infof(معلومات, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS));
    }
  }

  أرجع پایله;
}

ساكن CURLcode quote_error(بنية Curl_easy *معلومات,
                            بنية ssh_conn *sshc)
{
  failf(معلومات, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_quote(بنية Curl_easy *معلومات,
                           بنية ssh_conn *sshc,
                           بنية SSHPROTO *sshp)
{
  ثابت حرف *cp;
  CURLcode پایله = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
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

  إذا(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    حرف *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->لار);
    إذا(!tmp)
      أرجع CURLE_بهر_د_حافظه;
    Curl_debug(معلومات, CURLINFO_سرلیک_بهر, "PWD\n", 4);
    Curl_debug(معلومات, CURLINFO_سرلیک_دننه, tmp, مزي_اوږدوالی(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک, tmp, مزي_اوږدوالی(tmp));
    curlx_free(tmp);
    إذا(!پایله)
      myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
    أرجع پایله;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  إذا(!cp) {
    failf(معلومات, "Syntax error command '%s', missing parameter", cmd);
    أرجع CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  پایله = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  إذا(پایله) {
    إذا(پایله != CURLE_بهر_د_حافظه)
      failf(معلومات, "Syntax error: Bad first parameter to '%s'", cmd);
    أرجع پایله;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
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
      إذا(پایله != CURLE_بهر_د_حافظه)
        failf(معلومات, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      أرجع پایله;
    }
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);

    memset(&sshp->quote_attrs, 0, حجم(LIBSSH2_SFTP_ځانګړتیاوې));
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_STAT);
    أرجع پایله;
  }
  إذا(!مزي_پرتله(cmd, "ln ", 3) ||
     !مزي_پرتله(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    پایله = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(پایله) {
      إذا(پایله != CURLE_بهر_د_حافظه)
        failf(معلومات, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      أرجع پایله;
    }
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_SYMLINK);
    أرجع پایله;
  }
  وإلا إذا(!مزي_پرتله(cmd, "mkdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    /* create directory */
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_MKDIR);
    أرجع پایله;
  }
  وإلا إذا(!مزي_پرتله(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    پایله = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(پایله) {
      إذا(پایله != CURLE_بهر_د_حافظه)
        failf(معلومات, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      أرجع پایله;
    }
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_RENAME);
    أرجع پایله;
  }
  وإلا إذا(!مزي_پرتله(cmd, "rmdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    /* delete directory */
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_RMDIR);
    أرجع پایله;
  }
  وإلا إذا(!مزي_پرتله(cmd, "rm ", 3)) {
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_UNLINK);
    أرجع پایله;
  }
  وإلا إذا(!مزي_پرتله(cmd, "statvfs ", 8)) {
    إذا(*cp)
      أرجع quote_error(معلومات, sshc);
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_STATVFS);
    أرجع پایله;
  }

  failf(معلومات, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_upload_init(بنية Curl_easy *معلومات,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp,
                                 bool *blockp)
{
  بینښه اوږد flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  إذا(معلومات->state.resume_from) {
    LIBSSH2_SFTP_ځانګړتیاوې attrs;
    إذا(معلومات->state.resume_from < 0) {
      صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->لار,
                                    curlx_uztoui(مزي_اوږدوالی(sshp->لار)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      إذا(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        أرجع CURLE_OK;
      }
      إذا(rc) {
        معلومات->state.resume_from = 0;
      }
      وإلا {
        curl_off_t اندازه = attrs.filesize;
        إذا(اندازه < 0) {
          failf(معلومات, "Bad file size (%" FMT_OFF_T ")", اندازه);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        معلومات->state.resume_from = attrs.filesize;
      }
    }
  }

  إذا(معلومات->سېټ.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  وإلا إذا(معلومات->state.resume_from > 0) {
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
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->لار,
                         curlx_uztoui(مزي_اوږدوالی(sshp->لار)),
                         flags, (اوږد)معلومات->سېټ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  إذا(!sshc->sftp_handle) {
    CURLcode پایله;
    بینښه اوږد sftperr;
    صحيح rc = libssh2_session_وروستی_errno(sshc->ssh_session);

    إذا(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      أرجع CURLE_OK;
    }

    إذا(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    وإلا
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    إذا(sshc->دوهم_Create_Dirs) {
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      failf(معلومات, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      أرجع sftp_libssh2_error_منزل_CURLE(sftperr);
    }
    إذا((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_لار) &&
       معلومات->سېټ.ftp_create_missing_dirs &&
       (مزي_اوږدوالی(sshp->لار) > 1)) {
      /* try to create the path remotely */
      sshc->دوهم_Create_Dirs = 1;
      myssh_منزل(معلومات, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      أرجع CURLE_OK;
    }
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    پایله = sftp_libssh2_error_منزل_CURLE(sftperr);
    إذا(!پایله) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      پایله = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(معلومات, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    أرجع پایله;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
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
        أرجع CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      افعل {
        بنية Curl_mapi_guard guard;
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (معلومات->state.resume_from - passed >
           (curl_off_t)حجم(scratch)) ?
          حجم(scratch) : curlx_sotouz(معلومات->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_پیل(&guard, معلومات, easy_fread_func);
        actuallyread = معلومات->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              معلومات->state.دننه);
        CURL_CBAPI_پای_ټکی(&guard);

        passed += actuallyread;
        إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(معلومات, "Failed to read data");
          أرجع CURLE_FTP_COULDNT_USE_REST;
        }
      } طالما(passed < معلومات->state.resume_from);
    }

    /* now, decrease the size of the read */
    إذا(معلومات->state.infilesize > 0) {
      معلومات->state.infilesize -= معلومات->state.resume_from;
      معلومات->req.اندازه = معلومات->state.infilesize;
      Curl_pgrs_سېټ_Upload_اندازه(معلومات, معلومات->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)معلومات->state.resume_from);
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
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(معلومات);

  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن فراغ ssh_state_pkey_init(بنية Curl_easy *معلومات, بنية ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  إذا((معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     مزي_پلټنه(sshc->authlist, "publickey")) {
    إذا(sshc->pub_کلی)
      infof(معلومات, "SSH: trying public key file '%s'", sshc->pub_کلی);
    infof(معلومات, "SSH: trying private key file '%s'", sshc->priv_کلی);
    myssh_منزل(معلومات, sshc, SSH_AUTH_PKEY);
  }
  وإلا
    myssh_منزل(معلومات, sshc, SSH_AUTH_PASS_INIT);
}

ساكن CURLcode sftp_quote_stat(بنية Curl_easy *معلومات,
                                بنية ssh_conn *sshc,
                                بنية SSHPROTO *sshp,
                                bool *blockp)
{
  ثابت حرف *cmd = sshc->quote_توکی->معلومات;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  إذا(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  إذا(مزي_پرتله(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(مزي_اوږدوالی(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    إذا(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      أرجع CURLE_OK;
    }
    إذا(rc && !sshc->acceptfail) { /* get those attributes */
      بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
      failf(معلومات, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      اذهب fail;
    }
  }

  /* Now set the new attributes... */
  إذا(!مزي_پرتله(cmd, "chgrp", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t gid;
    إذا(!curlx_str_شمېره(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (بینښه اوږد)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وإلا إذا(!sshc->acceptfail) {
      failf(معلومات, "Syntax error: chgrp gid not a number");
      اذهب fail;
    }
  }
  وإلا إذا(!مزي_پرتله(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت حرف *p = sshc->quote_path1;
    /* permissions are octal */
    إذا(curlx_str_octal(&p, &perms, 07777)) {
      failf(معلومات, "Syntax error: chmod permissions not a number");
      اذهب fail;
    }

    sshp->quote_attrs.permissions = (بینښه اوږد)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  وإلا إذا(!مزي_پرتله(cmd, "chown", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t uid;
    إذا(!curlx_str_شمېره(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (بینښه اوږد)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وإلا إذا(!sshc->acceptfail) {
      failf(معلومات, "Syntax error: chown uid not a number");
      اذهب fail;
    }
  }
  وإلا إذا(!مزي_پرتله(cmd, "atime", 5) ||
          !مزي_پرتله(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    إذا(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(معلومات, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#إذا SIZEOF_وخت_T > SIZEOF_LONG
    إذا(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(معلومات, "date overflow");
      fail = TRUE;
    }
#د_که_پای
    إذا(fail)
      اذهب fail;
    إذا(!مزي_پرتله(cmd, "atime", 5))
      sshp->quote_attrs.atime = (بینښه اوږد)date;
    وإلا /* mtime */
      sshp->quote_attrs.mtime = (بینښه اوږد)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE_SETSTAT);
  أرجع CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_download_stat(بنية Curl_easy *معلومات,
                                   بنية ssh_conn *sshc,
                                   بنية SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ځانګړتیاوې attrs;
  صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->لار,
                                curlx_uztoui(مزي_اوږدوالی(sshp->لار)),
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
    معلومات->req.اندازه = -1;
    معلومات->req.maxdownload = -1;
    Curl_pgrs_سېټ_Download_اندازه(معلومات, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  وإلا {
    curl_off_t اندازه = attrs.filesize;

    إذا(اندازه < 0) {
      failf(معلومات, "Bad file size (%" FMT_OFF_T ")", اندازه);
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
    إذا(معلومات->state.use_range) {
      curl_off_t from;
      CURLcode پایله = Curl_ssh_range(معلومات, معلومات->state.range, اندازه,
                                       &from, &اندازه);
      إذا(پایله)
        أرجع پایله;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    معلومات->req.اندازه = اندازه;
    معلومات->req.maxdownload = اندازه;
    Curl_pgrs_سېټ_Download_اندازه(معلومات, اندازه);
  }

  /* We can resume if we can seek to the resume position */
  إذا(معلومات->state.resume_from) {
    إذا(معلومات->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      إذا((curl_off_t)attrs.filesize < -معلومات->state.resume_from) {
        failf(معلومات, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              معلومات->state.resume_from, (curl_off_t)attrs.filesize);
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      معلومات->state.resume_from += attrs.filesize;
    }
    وإلا {
      إذا((curl_off_t)attrs.filesize < معلومات->state.resume_from) {
        failf(معلومات, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              معلومات->state.resume_from, (curl_off_t)attrs.filesize);
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    معلومات->req.اندازه = attrs.filesize - معلومات->state.resume_from;
    معلومات->req.maxdownload = attrs.filesize - معلومات->state.resume_from;
    Curl_pgrs_سېټ_Download_اندازه(معلومات, attrs.filesize - معلومات->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)معلومات->state.resume_from);
  }

  /* Setup the actual download */
  إذا(معلومات->req.اندازه == 0) {
    /* no data to transfer */
    Curl_xfer_تیاری_nop(معلومات);
    infof(معلومات, "SSH: file already completely downloaded");
    myssh_منزل(معلومات, sshc, SSH_STOP);
    أرجع CURLE_OK;
  }
  Curl_xfer_تیاری_recv(معلومات, FIRSTSOCKET, معلومات->req.اندازه);

  /* not set by Curl_xfer_setup to preserve io_flags */
  معلومات->conn->send_idx = 0;

  myssh_منزل(معلومات, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode sftp_readdir(بنية Curl_easy *معلومات,
                             بنية ssh_conn *sshc,
                             بنية SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode پایله = CURLE_OK;
  صحيح rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_لار_MAX,
                                   sshp->readdir_longentry, CURL_لار_MAX,
                                   &sshp->readdir_attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    أرجع پایله;
  }
  إذا(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    إذا(معلومات->سېټ.list_only) {
      پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      إذا(!پایله)
        پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY, "\n", 1);
      إذا(پایله)
        أرجع پایله;
    }
    وإلا {
      پایله = curlx_dyn_جمع_کړه(&sshp->readdir, sshp->readdir_longentry);

      إذا(!پایله) {
        إذا((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          پایله = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->لار,
                                  sshp->readdir_filename);
          myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_LINK);
        }
        وإلا {
          myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      أرجع پایله;
    }
  }
  وإلا إذا(!rc) {
    myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_بشپړ);
  }
  وإلا {
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    پایله = sftperr ? sftp_libssh2_error_منزل_CURLE(sftperr) : CURLE_SSH;
    failf(معلومات, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_وروستی_errno(sshc->ssh_session));
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
  }
  أرجع پایله;
}

ساكن CURLcode ssh_state_init(بنية Curl_easy *معلومات,
                               بنية ssh_conn *sshc)
{
  CURLcode پایله;
  sshc->دوهم_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_سېټ_blocking(sshc->ssh_session, 0);

  پایله = ssh_force_knownhost_کلی_type(معلومات, sshc);
  إذا(پایله)
    myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
  وإلا
    myssh_منزل(معلومات, sshc, SSH_S_STARTUP);
  أرجع پایله;
}

ساكن CURLcode ssh_state_startup(بنية Curl_easy *معلومات,
                                  بنية ssh_conn *sshc)
{
  بنية connectdata *conn = معلومات->conn;
  صحيح rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc) {
    حرف *err_پیغام = عدم;
    (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    failf(معلومات, "Failure establishing ssh session: %d, %s", rc, err_پیغام);

    myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
    أرجع CURLE_FAILED_INIT;
  }

  myssh_منزل(معلومات, sshc, SSH_HOSTKEY);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_hostkey(بنية Curl_easy *معلومات,
                                  بنية ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode پایله = ssh_check_fingerprint(معلومات, sshc);
  إذا(!پایله)
    myssh_منزل(معلومات, sshc, SSH_AUTHLIST);
  أرجع پایله;
}

ساكن CURLcode ssh_state_authlist(بنية Curl_easy *معلومات,
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
  بنية connectdata *conn = معلومات->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(مزي_اوږدوالی(user)));

  إذا(!sshc->authlist) {
    صحيح rc;
    إذا(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(معلومات, "SSH: user accepted with no authentication");
      myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
      أرجع CURLE_OK;
    }
    rc = libssh2_session_وروستی_errno(sshc->ssh_session);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
    أرجع libssh2_session_error_منزل_CURLE(rc);
  }
  infof(معلومات, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_منزل(معلومات, sshc, SSH_AUTH_PKEY_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pkey(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  بنية connectdata *conn = معلومات->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحيح rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(مزي_اوږدوالی(user)),
    sshc->pub_کلی, sshc->priv_کلی, sshc->passphrase ? sshc->passphrase : "");
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(معلومات, "SSH: authenticated via publickey");
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
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
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    }
    infof(معلومات, "SSH: publickey authentication denied: %s", err_پیغام);
    myssh_منزل(معلومات, sshc, SSH_AUTH_PASS_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pass_init(بنية Curl_easy *معلومات,
                                         بنية ssh_conn *sshc)
{
  إذا((معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     مزي_پلټنه(sshc->authlist, "password")) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_PASS);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_AUTH_HOST_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pass(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc)
{
  بنية connectdata *conn = معلومات->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
  صحيح rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(مزي_اوږدوالی(user)),
                                 passwd,
                                 curlx_uztoui(مزي_اوږدوالی(passwd)),
                                 عدم);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    أرجع CURLE_AGAIN;
  }
  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(معلومات, "SSH: initialized password authentication");
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_AUTH_HOST_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_host_init(بنية Curl_easy *معلومات,
                                         بنية ssh_conn *sshc)
{
  إذا((معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     مزي_پلټنه(sshc->authlist, "hostbased")) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_HOST);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_AUTH_AGENT_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent_init(بنية Curl_easy *معلومات,
                                          بنية ssh_conn *sshc)
{
  صحيح rc = 0;
  إذا((معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     مزي_پلټنه(sshc->authlist, "publickey")) {

    infof(معلومات, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    إذا(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      إذا(!sshc->ssh_agent) {
        infof(معلومات, "SSH: could not create agent object");

        myssh_منزل(معلومات, sshc, SSH_AUTH_کلی_INIT);
        أرجع CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc < 0) {
      infof(معلومات, "SSH: failure connecting to agent");
      myssh_منزل(معلومات, sshc, SSH_AUTH_کلی_INIT);
    }
    وإلا {
      myssh_منزل(معلومات, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  وإلا
    myssh_منزل(معلومات, sshc, SSH_AUTH_کلی_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent_list(بنية Curl_easy *معلومات,
                                          بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_agent_list_identities(sshc->ssh_agent);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  إذا(rc < 0) {
    infof(معلومات, "SSH: failure requesting identities to agent");
    myssh_منزل(معلومات, sshc, SSH_AUTH_کلی_INIT);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_بې_اغېزه_عنصر = عدم;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent(بنية Curl_easy *معلومات,
                                     بنية ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  صحيح rc = libssh2_agent_get_بې_اغېزه_عنصر(sshc->ssh_agent,
                                      &sshc->sshagent_بې_اغېزه_عنصر,
                                      sshc->sshagent_prev_بې_اغېزه_عنصر);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    CURL_TRC_SSH(معلومات, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(معلومات->conn->creds),
                 sshc->sshagent_بې_اغېزه_عنصر->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(معلومات->conn->creds),
                                sshc->sshagent_بې_اغېزه_عنصر);

    إذا(rc < 0) {
      إذا(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_بې_اغېزه_عنصر = sshc->sshagent_بې_اغېزه_عنصر;
        أرجع CURLE_OK;
      }
      أرجع CURLE_AGAIN;
    }
  }

  إذا(rc < 0)
    infof(معلومات, "SSH: failure requesting identities to agent");
  وإلا إذا(rc == 1)
    infof(معلومات, "SSH: no agent identity would match");

  إذا(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(معلومات, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(معلومات->conn->creds),
          sshc->sshagent_بې_اغېزه_عنصر->comment);
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_AUTH_کلی_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_کلی_init(بنية Curl_easy *معلومات,
                                        بنية ssh_conn *sshc)
{
  إذا((معلومات->سېټ.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     مزي_پلټنه(sshc->authlist, "keyboard-interactive")) {
    myssh_منزل(معلومات, sshc, SSH_AUTH_کلی);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_کلی(بنية Curl_easy *معلومات,
                                   بنية ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  بنية connectdata *conn = معلومات->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحيح rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(مزي_اوږدوالی(user)),
                                             &kbd_callback);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(معلومات, "SSH: initialized keyboard interactive authentication");
    myssh_منزل(معلومات, sshc, SSH_AUTH_بشپړ);
    أرجع CURLE_OK;
  }
  أرجع CURLE_LOGIN_DENIED;
}

ساكن CURLcode ssh_state_auth_بشپړ(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc)
{
  بنية connectdata *conn = معلومات->conn;
  إذا(!sshc->authed) {
    failf(معلومات, "Authentication failure");
    myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
    أرجع CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(معلومات, "SSH: authentication complete");

  Curl_pgrs_وخت(معلومات, TIMER_APPCONNECT); /* SSH is connected */

  معلومات->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  إذا(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_منزل(معلومات, sshc, SSH_SFTP_INIT);
    أرجع CURLE_OK;
  }
  infof(معلومات, "SSH: connection established");
  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_init(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  إذا(!sshc->sftp_session) {
    حرف *err_پیغام = عدم;
    إذا(libssh2_session_وروستی_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    failf(معلومات, "Failure initializing sftp session: %s", err_پیغام);
    myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
    أرجع CURLE_FAILED_INIT;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_REALPATH);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_realpath(بنية Curl_easy *معلومات,
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
                               sshp->readdir_filename, CURL_لار_MAX,
                               LIBSSH2_SFTP_REALPATH);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  myssh_منزل(معلومات, sshc, SSH_STOP);
  إذا(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    إذا(!sshc->homedir)
      أرجع CURLE_بهر_د_حافظه;
    curlx_free(معلومات->state.most_recent_ftp_entrypath);
    معلومات->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    إذا(!معلومات->state.most_recent_ftp_entrypath)
      أرجع CURLE_بهر_د_حافظه;
  }
  وإلا {
    /* Return the error type */
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    CURLcode پایله;
    إذا(sftperr)
      پایله = sftp_libssh2_error_منزل_CURLE(sftperr);
    وإلا
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      پایله = CURLE_SSH;
    CURL_TRC_SSH(معلومات, "error = %lu makes libcurl = %d", sftperr, (صحيح)پایله);
    أرجع پایله;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(معلومات, "CONNECT phase done");
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_init(بنية Curl_easy *معلومات,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  CURLcode پایله;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  پایله = Curl_getworkingpath(معلومات, sshc->homedir, &sshp->لار);
  إذا(پایله) {
    myssh_منزل(معلومات, sshc, SSH_STOP);
    أرجع پایله;
  }

  إذا(معلومات->سېټ.quote) {
    infof(معلومات, "SSH: sending quote commands");
    sshc->quote_توکی = معلومات->سېټ.quote;
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_SFTP_GETINFO);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_postquote_init(بنية Curl_easy *معلومات,
                                              بنية ssh_conn *sshc)
{
  إذا(معلومات->سېټ.postquote) {
    infof(معلومات, "SSH: sending quote commands");
    sshc->quote_توکی = معلومات->سېټ.postquote;
    myssh_منزل(معلومات, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_منزل(معلومات, sshc, SSH_STOP);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote(بنية Curl_easy *معلومات,
                                     بنية ssh_conn *sshc,
                                     بنية SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode پایله;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  پایله = sftp_quote(معلومات, sshc, sshp);
  إذا(پایله) {
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع پایله;
}

ساكن CURLcode ssh_state_sftp_بل_quote(بنية Curl_easy *معلومات,
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
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_stat(بنية Curl_easy *معلومات,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode پایله;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  پایله = sftp_quote_stat(معلومات, sshc, sshp, blockp);
  إذا(پایله) {
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع پایله;
}

ساكن CURLcode ssh_state_sftp_quote_setstat(بنية Curl_easy *معلومات,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp)
{
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(مزي_اوږدوالی(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    failf(معلومات, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_symlink(بنية Curl_easy *معلومات,
                                             بنية ssh_conn *sshc)
{
  صحيح rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(مزي_اوږدوالی(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(مزي_اوږدوالی(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    failf(معلومات, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_mkdir(بنية Curl_easy *معلومات,
                                           بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(مزي_اوږدوالی(sshc->quote_path1)),
                                 (اوږد)معلومات->سېټ.new_directory_perms);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    failf(معلومات, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_rename(بنية Curl_easy *معلومات,
                                            بنية ssh_conn *sshc)
{
  صحيح rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(مزي_اوږدوالی(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(مزي_اوږدوالی(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    failf(معلومات, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_rmdir(بنية Curl_easy *معلومات,
                                           بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(مزي_اوږدوالی(sshc->quote_path1)));
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    failf(معلومات, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_unlink(بنية Curl_easy *معلومات,
                                            بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(مزي_اوږدوالی(sshc->quote_path1)));
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    failf(معلومات, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_statvfs(بنية Curl_easy *معلومات,
                                             بنية ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  صحيح rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(مزي_اوږدوالی(sshc->quote_path1)),
                                &statvfs);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    failf(معلومات, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  وإلا إذا(rc == 0) {
#که_تعریف_شوی _MSC_VER
#تعریف_کړه CURL_LIBSSH2_VFS_اندازه_ماسک "I64u"
#وإلا
#تعریف_کړه CURL_LIBSSH2_VFS_اندازه_ماسک "llu"
#د_که_پای
    CURLcode پایله;
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
                              statvfs.f_بلاکونه, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    إذا(!tmp) {
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع CURLE_بهر_د_حافظه;
    }

    پایله = Curl_client_write(معلومات, CLIENTWRITE_سرلیک, tmp, مزي_اوږدوالی(tmp));
    curlx_free(tmp);
    إذا(پایله) {
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع پایله;
    }
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_بل_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_create_dirs_mkdir(بنية Curl_easy *معلومات,
                                                 بنية ssh_conn *sshc,
                                                 بنية SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->لار,
                             curlx_uztoui(مزي_اوږدوالی(sshp->لار)),
                             (اوږد)معلومات->سېټ.new_directory_perms);
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
    بینښه اوږد sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    إذا((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
      أرجع sftp_libssh2_error_منزل_CURLE(sftperr);
    }
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_CREATE_DIRS);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir_init(بنية Curl_easy *معلومات,
                                            بنية ssh_conn *sshc,
                                            بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  Curl_pgrs_سېټ_Download_اندازه(معلومات, -1);
  إذا(معلومات->req.no_body) {
    myssh_منزل(معلومات, sshc, SSH_STOP);
    أرجع CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->لار,
                         curlx_uztoui(مزي_اوږدوالی(sshp->لار)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  إذا(!sshc->sftp_handle) {
    بینښه اوږد sftperr;
    إذا(libssh2_session_وروستی_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    failf(معلومات, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    أرجع sftp_libssh2_error_منزل_CURLE(sftperr);
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir_link(بنية Curl_easy *معلومات,
                                            بنية ssh_conn *sshc,
                                            بنية SSHPROTO *sshp)
{
  CURLcode پایله;
  صحيح rc;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (بینښه صحيح)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_لار_MAX, LIBSSH2_SFTP_READLINK);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  إذا(rc < 0)
    أرجع CURLE_بهر_د_حافظه;

  /* append filename and extra output */
  پایله = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  إذا(پایله)
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
  وإلا
    myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_BOTTOM);
  أرجع پایله;
}

ساكن CURLcode ssh_state_scp_download_init(بنية Curl_easy *معلومات,
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
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->لار, &sb);
  إذا(!sshc->ssh_channel) {
    صحيح ssh_err;
    حرف *err_پیغام = عدم;

    إذا(libssh2_session_وروستی_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    ssh_err = libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    failf(معلومات, "%s", err_پیغام);
    myssh_منزل(معلومات, sshc, SSH_SCP_CHANNEL_FREE);
    أرجع libssh2_session_error_منزل_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_اندازه;
  معلومات->req.maxdownload = (curl_off_t)sb.st_اندازه;
  Curl_xfer_تیاری_recv(معلومات, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  معلومات->conn->send_idx = 0;

  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_close(بنية Curl_easy *معلومات,
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
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to close libssh2 file: %d %s", rc, err_پیغام);
    }
    sshc->sftp_handle = عدم;
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
  وإلا
    myssh_منزل(معلومات, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_shutdown(بنية Curl_easy *معلومات,
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
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to close libssh2 file: %d %s", rc, err_پیغام);
    }
    sshc->sftp_handle = عدم;
  }
  إذا(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      infof(معلومات, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = عدم;
  }

  curlx_safefree(sshc->homedir);

  myssh_منزل(معلومات, sshc, SSH_SESSION_DISCONNECT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_download_init(بنية Curl_easy *معلومات,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->لار,
                         curlx_uztoui(مزي_اوږدوالی(sshp->لار)),
                         LIBSSH2_FXF_READ, (اوږد)معلومات->سېټ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  إذا(!sshc->sftp_handle) {
    بینښه اوږد sftperr;
    إذا(libssh2_session_وروستی_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      أرجع CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_وروستی_error(sshc->sftp_session);
    failf(معلومات, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    أرجع sftp_libssh2_error_منزل_CURLE(sftperr);
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_DOWNLOAD_STAT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_upload_init(بنية Curl_easy *معلومات,
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
    libssh2_scp_send64(sshc->ssh_session, sshp->لار,
                       (صحيح)معلومات->سېټ.new_file_perms,
                       (libssh2_int64_t)معلومات->state.infilesize, 0, 0);
  إذا(!sshc->ssh_channel) {
    صحيح ssh_err;
    حرف *err_پیغام = عدم;
    CURLcode پایله;
    إذا(libssh2_session_وروستی_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    ssh_err = libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
    failf(معلومات, "%s", err_پیغام);
    myssh_منزل(معلومات, sshc, SSH_SCP_CHANNEL_FREE);
    پایله = libssh2_session_error_منزل_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    إذا(پایله == CURLE_SSH ||
       پایله == CURLE_REMOTE_FILE_NOT_وموندل)
      پایله = CURLE_UPLOAD_FAILED;
    أرجع پایله;
  }

  /* upload data */
  معلومات->req.اندازه = معلومات->state.infilesize;
  Curl_pgrs_سېټ_Upload_اندازه(معلومات, معلومات->state.infilesize);
  Curl_xfer_تیاری_send(معلومات, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  معلومات->conn->recv_idx = FIRSTSOCKET;

  myssh_منزل(معلومات, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_session_disconnect(بنية Curl_easy *معلومات,
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
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیغام);
    }
    sshc->ssh_channel = عدم;
  }

  إذا(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to disconnect libssh2 session: %d %s", rc, err_پیغام);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_منزل(معلومات, sshc, SSH_SESSION_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode sshc_cleanup(بنية ssh_conn *sshc, بنية Curl_easy *معلومات,
                             bool بلاک)
{
  صحيح rc;

  إذا(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = عدم;
  }

  إذا(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    إذا((rc < 0) && معلومات) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_پیغام);
    }
    إذا(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = عدم;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_بې_اغېزه_عنصر = عدم;
    sshc->sshagent_prev_بې_اغېزه_عنصر = عدم;
  }

  إذا(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    إذا((rc < 0) && معلومات) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to close libssh2 file: %d %s", rc, err_پیغام);
    }
    إذا(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->sftp_handle = عدم;
  }

  إذا(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    إذا((rc < 0) && معلومات) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیغام);
    }
    إذا(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->ssh_channel = عدم;
  }

  إذا(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    إذا((rc < 0) && معلومات) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_پیغام);
    }
    إذا(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->sftp_session = عدم;
  }

  إذا(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    إذا((rc < 0) && معلومات) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session, &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to free libssh2 session: %d %s", rc, err_پیغام);
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
  curlx_safefree(sshc->pub_کلی);
  curlx_safefree(sshc->priv_کلی);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_getinfo(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc)
{
  إذا(معلومات->سېټ.get_filetime)
    myssh_منزل(معلومات, sshc, SSH_SFTP_FILETIME);
  وإلا
    myssh_منزل(معلومات, sshc, SSH_SFTP_TRANS_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_filetime(بنية Curl_easy *معلومات,
                                        بنية ssh_conn *sshc,
                                        بنية SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ځانګړتیاوې attrs;
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->لار,
                            curlx_uztoui(مزي_اوږدوالی(sshp->لار)),
                            LIBSSH2_SFTP_STAT, &attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0)
    معلومات->info.filetime = (time_t)attrs.mtime;

  myssh_منزل(معلومات, sshc, SSH_SFTP_TRANS_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_trans_init(بنية Curl_easy *معلومات,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  إذا(معلومات->state.upload)
    myssh_منزل(معلومات, sshc, SSH_SFTP_UPLOAD_INIT);
  وإلا إذا(sshp) {
    size_t plen = مزي_اوږدوالی(sshp->لار);
    إذا(plen && (sshp->لار[plen - 1] == '/'))
      myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR_INIT);
    وإلا
      myssh_منزل(معلومات, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  وإلا
    أرجع CURLE_FAILED_INIT;
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_upload_init(بنية Curl_easy *معلومات,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp,
                                           bool *بلاک)
{
  CURLcode پایله;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  پایله = sftp_upload_init(معلومات, sshc, sshp, بلاک);
  إذا(پایله) {
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع پایله;
}

ساكن CURLcode ssh_state_sftp_create_dirs_init(بنية Curl_easy *معلومات,
                                                بنية ssh_conn *sshc,
                                                بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  إذا(مزي_اوږدوالی(sshp->لار) > 1) {
    sshc->slash_pos = sshp->لار + 1; /* ignore the leading '/' */
    myssh_منزل(معلومات, sshc, SSH_SFTP_CREATE_DIRS);
  }
  وإلا
    myssh_منزل(معلومات, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_create_dirs(بنية Curl_easy *معلومات,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  إذا(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(معلومات, "SFTP: creating directory '%s'", sshp->لار);
    myssh_منزل(معلومات, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    أرجع CURLE_OK;
  }
  myssh_منزل(معلومات, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc,
                                       بنية SSHPROTO *sshp,
                                       bool *بلاک)
{
  CURLcode پایله;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  پایله = sftp_readdir(معلومات, sshc, sshp, بلاک);
  إذا(پایله)
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
  أرجع پایله;
}

ساكن CURLcode ssh_state_sftp_readdir_bottom(بنية Curl_easy *معلومات,
                                              بنية ssh_conn *sshc,
                                              بنية SSHPROTO *sshp)
{
  CURLcode پایله;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  پایله = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  إذا(!پایله)
    پایله = Curl_client_write(معلومات, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  إذا(پایله) {
    curlx_dyn_free(&sshp->readdir);
    myssh_منزل(معلومات, sshc, SSH_STOP);
  }
  وإلا {
    curlx_dyn_reset(&sshp->readdir);
    myssh_منزل(معلومات, sshc, SSH_SFTP_READDIR);
  }
  أرجع پایله;
}

ساكن CURLcode ssh_state_sftp_readdir_بشپړ(بنية Curl_easy *معلومات,
                                            بنية ssh_conn *sshc)
{
  إذا(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  sshc->sftp_handle = عدم;

  /* no data to transfer */
  Curl_xfer_تیاری_nop(معلومات);
  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_download_stat(بنية Curl_easy *معلومات,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp,
                                             bool *بلاک)
{
  CURLcode پایله;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  پایله = sftp_download_stat(معلومات, sshc, sshp, بلاک);
  إذا(پایله) {
    myssh_منزل(معلومات, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع پایله;
}

ساكن CURLcode ssh_state_scp_trans_init(بنية Curl_easy *معلومات,
                                         بنية ssh_conn *sshc,
                                         بنية SSHPROTO *sshp)
{
  CURLcode پایله;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  پایله = Curl_getworkingpath(معلومات, sshc->homedir,
                               &sshp->لار);
  إذا(پایله) {
    myssh_منزل(معلومات, sshc, SSH_STOP);
    أرجع پایله;
  }

  إذا(معلومات->state.upload) {
    إذا(معلومات->state.infilesize < 0) {
      failf(معلومات, "SCP requires a known file size for upload");
      myssh_منزل(معلومات, sshc, SSH_SCP_CHANNEL_FREE);
      أرجع CURLE_UPLOAD_FAILED;
    }
    myssh_منزل(معلومات, sshc, SSH_SCP_UPLOAD_INIT);
  }
  وإلا
    myssh_منزل(معلومات, sshc, SSH_SCP_DOWNLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_بشپړ(بنية Curl_easy *معلومات,
                                   بنية ssh_conn *sshc)
{
  إذا(معلومات->state.upload)
    myssh_منزل(معلومات, sshc, SSH_SCP_SEND_EOF);
  وإلا
    myssh_منزل(معلومات, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_send_eof(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_send_eof(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session,
                                       &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_پیغام);
    }
  }
  myssh_منزل(معلومات, sshc, SSH_SCP_WAIT_EOF);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_wait_eof(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session,
                                       &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to get channel EOF: %d %s", rc, err_پیغام);
    }
  }
  myssh_منزل(معلومات, sshc, SSH_SCP_WAIT_CLOSE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_wait_close(بنية Curl_easy *معلومات,
                                         بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session,
                                       &err_پیغام, عدم, 0);
      infof(معلومات, "Channel failed to close: %d %s", rc, err_پیغام);
    }
  }
  myssh_منزل(معلومات, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_channel_free(
  بنية Curl_easy *معلومات,
  بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_free(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc < 0) {
      حرف *err_پیغام = عدم;
      (فراغ)libssh2_session_وروستی_error(sshc->ssh_session,
                                       &err_پیغام, عدم, 0);
      infof(معلومات, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیغام);
    }
    sshc->ssh_channel = عدم;
  }
  CURL_TRC_SSH(معلومات, "SCP DONE phase complete");
  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_session_free(بنية Curl_easy *معلومات,
                                       بنية ssh_conn *sshc)
{
  CURLcode پایله = sshc_cleanup(sshc, معلومات, FALSE);
  بنية connectdata *conn = معلومات->conn;
  إذا(پایله)
    أرجع پایله;
  memset(sshc, 0, حجم(بنية ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_منزل(معلومات, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ساكن CURLcode ssh_statemachine(بنية Curl_easy *معلومات,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp,
                                 bool *بلاک)
{
  CURLcode پایله = CURLE_OK;
  *بلاک = 0; /* we are not blocking by default */

  افعل {
    بدل(sshc->state) {
    حالة SSH_INIT:
      پایله = ssh_state_init(معلومات, sshc);
      إذا(پایله)
        اكسر;
      FALLTHROUGH();

    حالة SSH_S_STARTUP:
      پایله = ssh_state_startup(معلومات, sshc);
      إذا(پایله)
        اكسر;
      FALLTHROUGH();

    حالة SSH_HOSTKEY:
      پایله = ssh_state_hostkey(معلومات, sshc);
      اكسر;

    حالة SSH_AUTHLIST:
      پایله = ssh_state_authlist(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_PKEY:
      پایله = ssh_state_auth_pkey(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_PASS_INIT:
      پایله = ssh_state_auth_pass_init(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_PASS:
      پایله = ssh_state_auth_pass(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_HOST_INIT:
      پایله = ssh_state_auth_host_init(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_HOST:
      myssh_منزل(معلومات, sshc, SSH_AUTH_AGENT_INIT);
      اكسر;

    حالة SSH_AUTH_AGENT_INIT:
      پایله = ssh_state_auth_agent_init(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_AGENT_LIST:
      پایله = ssh_state_auth_agent_list(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_AGENT:
      پایله = ssh_state_auth_agent(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_کلی_INIT:
      پایله = ssh_state_auth_کلی_init(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_کلی:
      پایله = ssh_state_auth_کلی(معلومات, sshc);
      اكسر;

    حالة SSH_AUTH_بشپړ:
      پایله = ssh_state_auth_بشپړ(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_INIT:
      پایله = ssh_state_sftp_init(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_REALPATH:
      پایله = ssh_state_sftp_realpath(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_QUOTE_INIT:
      پایله = ssh_state_sftp_quote_init(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_POSTQUOTE_INIT:
      پایله = ssh_state_sftp_postquote_init(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE:
      پایله = ssh_state_sftp_quote(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_بل_QUOTE:
      پایله = ssh_state_sftp_بل_quote(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_STAT:
      پایله = ssh_state_sftp_quote_stat(معلومات, sshc, sshp, بلاک);
      اكسر;

    حالة SSH_SFTP_QUOTE_SETSTAT:
      پایله = ssh_state_sftp_quote_setstat(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_QUOTE_SYMLINK:
      پایله = ssh_state_sftp_quote_symlink(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_MKDIR:
      پایله = ssh_state_sftp_quote_mkdir(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_RENAME:
      پایله = ssh_state_sftp_quote_rename(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_RMDIR:
      پایله = ssh_state_sftp_quote_rmdir(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_UNLINK:
      پایله = ssh_state_sftp_quote_unlink(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_STATVFS:
      پایله = ssh_state_sftp_quote_statvfs(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_GETINFO:
      پایله = ssh_state_sftp_getinfo(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_FILETIME:
      پایله = ssh_state_sftp_filetime(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_TRANS_INIT:
      پایله = ssh_state_sftp_trans_init(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_UPLOAD_INIT:
      پایله = ssh_state_sftp_upload_init(معلومات, sshc, sshp, بلاک);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS_INIT:
      پایله = ssh_state_sftp_create_dirs_init(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS:
      پایله = ssh_state_sftp_create_dirs(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS_MKDIR:
      پایله = ssh_state_sftp_create_dirs_mkdir(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_INIT:
      پایله = ssh_state_sftp_readdir_init(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR:
      پایله = ssh_state_sftp_readdir(معلومات, sshc, sshp, بلاک);
      اكسر;

    حالة SSH_SFTP_READDIR_LINK:
      پایله = ssh_state_sftp_readdir_link(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_BOTTOM:
      پایله = ssh_state_sftp_readdir_bottom(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_بشپړ:
      پایله = ssh_state_sftp_readdir_بشپړ(معلومات, sshc);
      اكسر;

    حالة SSH_SFTP_DOWNLOAD_INIT:
      پایله = ssh_state_sftp_download_init(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_DOWNLOAD_STAT:
      پایله = ssh_state_sftp_download_stat(معلومات, sshc, sshp, بلاک);
      اكسر;

    حالة SSH_SFTP_CLOSE:
      پایله = ssh_state_sftp_close(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_SHUTDOWN:
      پایله = ssh_state_sftp_shutdown(معلومات, sshc);
      اكسر;

    حالة SSH_SCP_TRANS_INIT:
      پایله = ssh_state_scp_trans_init(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SCP_UPLOAD_INIT:
      پایله = ssh_state_scp_upload_init(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SCP_DOWNLOAD_INIT:
      پایله = ssh_state_scp_download_init(معلومات, sshc, sshp);
      اكسر;

    حالة SSH_SCP_بشپړ:
      پایله = ssh_state_scp_بشپړ(معلومات, sshc);
      اكسر;

    حالة SSH_SCP_SEND_EOF:
      پایله = ssh_state_scp_send_eof(معلومات, sshc);
      اكسر;

    حالة SSH_SCP_WAIT_EOF:
      پایله = ssh_state_scp_wait_eof(معلومات, sshc);
      اكسر;

    حالة SSH_SCP_WAIT_CLOSE:
      پایله = ssh_state_scp_wait_close(معلومات, sshc);
      اكسر;

    حالة SSH_SCP_CHANNEL_FREE:
      پایله = ssh_state_scp_channel_free(معلومات, sshc);
      اكسر;

    حالة SSH_SESSION_DISCONNECT:
      پایله = ssh_state_session_disconnect(معلومات, sshc);
      اكسر;

    حالة SSH_SESSION_FREE:
      پایله = ssh_state_session_free(معلومات, sshc);
      اكسر;

    حالة SSH_QUIT:
    افتراضي:
      /* internal error */
      myssh_منزل(معلومات, sshc, SSH_STOP);
      اكسر;
    }

  } طالما(!پایله && (sshc->state != SSH_STOP) && !*بلاک);

  إذا(پایله == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *بلاک = TRUE;
    پایله = CURLE_OK;
  }
  CURL_TRC_SSH(معلومات, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحيح)پایله, *بلاک);

  أرجع پایله;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ساكن فراغ ssh_block2waitfor(بنية Curl_easy *معلومات,
                              بنية ssh_conn *sshc,
                              bool بلاک)
{
  صحيح dir = 0;
  (فراغ)معلومات;
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
ساكن CURLcode ssh_multi_statemach(بنية Curl_easy *معلومات, bool *بشپړ)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(معلومات, CURL_META_SSH_EASY);
  CURLcode پایله = CURLE_OK;
  bool بلاک; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;

  افعل {
    پایله = ssh_statemachine(معلومات, sshc, sshp, &بلاک);
    *بشپړ = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } طالما(!پایله && !*بشپړ && !بلاک);
  ssh_block2waitfor(معلومات, sshc, بلاک);

  أرجع پایله;
}

ساكن CURLcode ssh_بلاک_statemach(بنية Curl_easy *معلومات,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode پایله = CURLE_OK;
  بنية curltime پیل = *Curl_pgrs_now(معلومات);

  طالما((sshc->state != SSH_STOP) && !پایله) {
    bool بلاک;
    timediff_t کیڼ_ms = 1000;

    پایله = ssh_statemachine(معلومات, sshc, sshp, &بلاک);
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
    وإلا إذا(curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &پیل) > 1000) {
      /* disconnect timeout */
      failf(معلومات, "Disconnect timed out");
      پایله = CURLE_OK;
      اكسر;
    }

    إذا(بلاک) {
      صحيح dir = libssh2_session_بلاک_directions(sshc->ssh_session);
      curl_socket_t sock = معلومات->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      إذا(LIBSSH2_SESSION_بلاک_INBOUND & dir)
        fd_read = sock;
      إذا(LIBSSH2_SESSION_بلاک_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (فراغ)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
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
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ساكن فراغ myssh_conn_dtor(ثابت فراغ *کلی, size_t klen, فراغ *ثبت)
{
  بنية ssh_conn *sshc = ثبت;
  (فراغ)کلی;
  (فراغ)klen;
  sshc_cleanup(sshc, عدم, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ساكن CURLcode ssh_تیاری_connection(بنية Curl_easy *معلومات,
                                     بنية connectdata *conn)
{
  بنية ssh_conn *sshc;
  بنية SSHPROTO *sshp;
  (فراغ)conn;

  sshc = curlx_calloc(1, حجم(*sshc));
  إذا(!sshc)
    أرجع CURLE_بهر_د_حافظه;

  إذا(Curl_conn_meta_سېټ(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    أرجع CURLE_بهر_د_حافظه;

  sshp = curlx_calloc(1, حجم(*sshp));
  إذا(!sshp)
    أرجع CURLE_بهر_د_حافظه;

  curlx_dyn_init(&sshp->readdir, CURL_لار_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_لار_MAX);
  إذا(Curl_meta_سېټ(معلومات, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    أرجع CURLE_بهر_د_حافظه;

  أرجع Curl_ssh_تیاری_pkey(معلومات, sshc);
}

ساكن Curl_recv scp_recv, sftp_recv;
ساكن Curl_send scp_send, sftp_send;

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
ساكن ssize_t ssh_tls_recv(libssh2_socket_t sock, فراغ *بفر,
                            size_t اوږدتیا, صحيح flags, فراغ **abstract)
{
  بنية Curl_easy *معلومات = (بنية Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(معلومات, sock);
  size_t nread;
  CURLcode پایله;
  بنية connectdata *conn = معلومات->conn;
  Curl_recv *backup = conn->recv[sockindex];
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (فراغ)flags;

  إذا(!sshc)
    أرجع -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  پایله = Curl_conn_recv(معلومات, sockindex, بفر, اوږدتیا, &nread);
  conn->recv[sockindex] = backup;
  إذا(پایله == CURLE_AGAIN)
    أرجع -EAGAIN; /* magic return code for libssh2 */
  وإلا إذا(پایله)
    أرجع -1; /* generic error */
  Curl_debug(معلومات, CURLINFO_معلومات_دننه, (ثابت حرف *)بفر, nread);
  أرجع (ssize_t)nread;
}

ساكن ssize_t ssh_tls_send(libssh2_socket_t sock, ثابت فراغ *بفر,
                            size_t اوږدتیا, صحيح flags, فراغ **abstract)
{
  بنية Curl_easy *معلومات = (بنية Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(معلومات, sock);
  size_t nwrite;
  CURLcode پایله;
  بنية connectdata *conn = معلومات->conn;
  Curl_send *backup = conn->send[sockindex];
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (فراغ)flags;

  إذا(!sshc)
    أرجع -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  پایله = Curl_conn_send(معلومات, sockindex, بفر, اوږدتیا, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  إذا(پایله == CURLE_AGAIN)
    أرجع -EAGAIN; /* magic return code for libssh2 */
  وإلا إذا(پایله)
    أرجع -1; /* error */
  Curl_debug(معلومات, CURLINFO_معلومات_بهر, (ثابت حرف *)بفر, nwrite);
  أرجع (ssize_t)nwrite;
}
#د_که_پای

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ساكن CURLcode ssh_connect(بنية Curl_easy *معلومات, bool *بشپړ)
{
#که_تعریف_شوی CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#د_که_پای
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode پایله;

#إذا LIBSSH2_نسخه_عدد >= 0x010b00
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
      infof(معلومات, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#د_که_پای

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  infof(معلومات, "SSH: user '%s'", Curl_creds_user(conn->creds));
#که_تعریف_شوی CURL_LIBSSH2_DEBUG
  infof(معلومات, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#د_که_پای /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, معلومات);

  إذا(!sshc->ssh_session) {
    failf(معلومات, "Failure initializing ssh session");
    أرجع CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#إذا LIBSSH2_نسخه_عدد >= 0x010B00
  إذا(معلومات->سېټ.server_response_timeout > 0) {
    libssh2_session_سېټ_read_timeout(sshc->ssh_session,
                             (اوږد)(معلومات->سېټ.server_response_timeout / 1000));
  }
#د_که_پای

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  إذا(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#إذا LIBSSH2_نسخه_عدد >= 0x010b01
    infof(معلومات, "SSH: using HTTPS proxy");
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic ټېل
#لارښوونه clang diagnostic ignored "-Wcast-function-type-strict"
#د_که_پای
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#إذا defined(__clang__) && __clang_major__ >= 16
#لارښوونه clang diagnostic راوباسه
#د_که_پای
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

    infof(معلومات, "SSH: using HTTPS proxy");
    libssh2_session_callback_سېټ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_سېټ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#د_که_پای

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#د_که_پای /* CURL_DISABLE_PROXY */
  إذا(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  وإلا {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  إذا(معلومات->سېټ.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(معلومات, "SSH: failed to enable compression for session");
  }

  إذا(CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS)) {
    صحيح rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    إذا(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = عدم;
      أرجع CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    إذا(rc < 0)
      infof(معلومات, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(معلومات, STRING_SSH_KNOWNHOSTS));
  }

#که_تعریف_شوی CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(معلومات, "SSH: socket %d", (صحيح)sock);
#د_که_پای

  myssh_منزل(معلومات, sshc, SSH_INIT);

  پایله = ssh_multi_statemach(معلومات, بشپړ);

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
  myssh_منزل(معلومات, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  پایله = ssh_multi_statemach(معلومات, dophase_بشپړ);

  *connected = Curl_conn_is_connected(معلومات->conn, FIRSTSOCKET);

  إذا(*dophase_بشپړ) {
    CURL_TRC_SSH(معلومات, "DO phase is complete");
  }

  أرجع پایله;
}

/* called from multi.c while DOing */
ساكن CURLcode scp_doing(بنية Curl_easy *معلومات,
                          bool *dophase_بشپړ)
{
  CURLcode پایله;
  پایله = ssh_multi_statemach(معلومات, dophase_بشپړ);

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
    پایله = ssh_بلاک_statemach(معلومات, sshc, sshp, TRUE);
  }

  إذا(sshc)
    أرجع sshc_cleanup(sshc, معلومات, TRUE);
  أرجع پایله;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ساكن CURLcode ssh_بشپړ(بنية Curl_easy *معلومات, CURLcode حالت)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(معلومات->conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(معلومات, CURL_META_SSH_EASY);
  CURLcode پایله = CURLE_OK;

  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;

  إذا(!حالت)
    /* run the state-machine */
    پایله = ssh_بلاک_statemach(معلومات, sshc, sshp, FALSE);
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

  إذا(sshc && !حالت)
    myssh_منزل(معلومات, sshc, SSH_SCP_بشپړ);

  أرجع ssh_بشپړ(معلومات, حالت);
}

ساكن CURLcode scp_send(بنية Curl_easy *معلومات, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode پایله = CURLE_OK;
  ssize_t nwritten;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  (فراغ)eos;
  *pnwritten = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ثابت حرف *)mem, len);

  ssh_block2waitfor(معلومات, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  إذا(nwritten == LIBSSH2_ERROR_EAGAIN)
    پایله = CURLE_AGAIN;
  وإلا إذا(nwritten < LIBSSH2_ERROR_NONE)
    پایله = libssh2_session_error_منزل_CURLE((صحيح)nwritten);
  وإلا
    *pnwritten = (size_t)nwritten;

  أرجع پایله;
}

ساكن CURLcode scp_recv(بنية Curl_easy *معلومات, int8_t sockindex,
                         حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode پایله = CURLE_OK;
  ssize_t nread;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(معلومات, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  إذا(nread == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nread < LIBSSH2_ERROR_NONE)
    پایله = libssh2_session_error_منزل_CURLE((صحيح)nread);
  وإلا
    *pnread = (size_t)nread;

  أرجع پایله;
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
  پایله = ssh_multi_statemach(معلومات, dophase_بشپړ);

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
  CURLcode پایله = ssh_multi_statemach(معلومات, dophase_بشپړ);

  إذا(*dophase_بشپړ) {
    CURL_TRC_SSH(معلومات, "DO phase is complete");
  }
  أرجع پایله;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode sftp_disconnect(بنية Curl_easy *معلومات,
                                بنية connectdata *conn, bool dead_connection)
{
  CURLcode پایله = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(معلومات, CURL_META_SSH_EASY);
  (فراغ)dead_connection;

  إذا(sshc) {
    إذا(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(معلومات, "DISCONNECT starts now");
      myssh_منزل(معلومات, sshc, SSH_SFTP_SHUTDOWN);
      پایله = ssh_بلاک_statemach(معلومات, sshc, sshp, TRUE);
      CURL_TRC_SSH(معلومات, "DISCONNECT is done -> %d", (صحيح)پایله);
    }
    sshc_cleanup(sshc, معلومات, TRUE);
  }
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
  أرجع ssh_بشپړ(معلومات, حالت);
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

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ثابت حرف *)mem, len);

  ssh_block2waitfor(معلومات, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

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
ساكن CURLcode sftp_recv(بنية Curl_easy *معلومات, int8_t sockindex,
                          حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (فراغ)sockindex;
  *pnread = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(معلومات, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

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
ساكن CURLcode ssh_do(بنية Curl_easy *معلومات, bool *بشپړ)
{
  CURLcode پایله;
  bool connected = FALSE;
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *بشپړ = FALSE; /* default to false */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  معلومات->req.اندازه = -1; /* make sure this is unknown at this point */
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

فراغ Curl_ssh_نسخه(حرف *بفر, size_t buflen)
{
  (فراغ)curl_msnprintf(بفر, buflen, "libssh2/%s", libssh2_نسخه(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ساكن فراغ ssh_attach(بنية Curl_easy *معلومات, بنية connectdata *conn)
{
  DEBUGASSERT(معلومات);
  DEBUGASSERT(conn);
  إذا(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    إذا(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      فراغ **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = معلومات;
    }
  }
}

/*
 * SCP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_scp = {
  ssh_تیاری_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_بشپړ,                             /* done */
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
  sftp_بشپړ,                            /* done */
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

#د_که_پای /* USE_LIBSSH2 */
