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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

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
#شامل "parsedate.h" /* for the week day and month names */
#شامل "multiif.h"
#شامل "select.h"
#شامل "curlx/win32-fopen.h"
#شامل "vssh/vssh.h"
#شامل "curlx/strparse.h"
#شامل "curlx/base64.h" /* for curlx_base64_encode() */

ساکن ثابت حرف *sftp_libssh2_strerror(غیرعلامتی طویل err)
{
  انتخاب(err) {
  حالت LIBSSH2_FX_NO_SUCH_FILE:
    واپس "No such file or directory";

  حالت LIBSSH2_FX_PERMISSION_DENIED:
    واپس "Permission denied";

  حالت LIBSSH2_FX_FAILURE:
    واپس "Operation failed";

  حالت LIBSSH2_FX_BAD_پیغام:
    واپس "Bad message from SFTP server";

  حالت LIBSSH2_FX_NO_CONNECTION:
    واپس "Not connected to SFTP server";

  حالت LIBSSH2_FX_CONNECTION_گمشدہ:
    واپس "Connection to SFTP server lost";

  حالت LIBSSH2_FX_OP_UNSUPPORTED:
    واپس "Operation not supported by SFTP server";

  حالت LIBSSH2_FX_INVALID_HANDLE:
    واپس "Invalid handle";

  حالت LIBSSH2_FX_NO_SUCH_راستہ:
    واپس "No such file or directory";

  حالت LIBSSH2_FX_FILE_ALREADY_EXISTS:
    واپس "File already exists";

  حالت LIBSSH2_FX_WRITE_PROTECT:
    واپس "File is write protected";

  حالت LIBSSH2_FX_NO_MEDIA:
    واپس "No media";

  حالت LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    واپس "Disk full";

  حالت LIBSSH2_FX_QUOTA_EXCEEDED:
    واپس "User quota exceeded";

  حالت LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    واپس "Unknown principal";

  حالت LIBSSH2_FX_تالا_CONFLICT:
    واپس "File lock conflict";

  حالت LIBSSH2_FX_DIR_NOT_خالی_خانے:
    واپس "Directory not empty";

  حالت LIBSSH2_FX_NOT_A_DIRECTORY:
    واپس "Not a directory";

  حالت LIBSSH2_FX_INVALID_FILENAME:
    واپس "Invalid filename";

  حالت LIBSSH2_FX_LINK_LOOP:
    واپس "Link points to itself";
  }
  واپس "Unknown error in libssh2";
}

ساکن خالی kbd_callback(ثابت حرف *name, صحیح name_len,
                         ثابت حرف *instruction, صحیح instruction_len,
                         صحیح عدد_prompts,
                         ثابت LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         خالی **abstract)
{
  ساخت Curl_easy *ڈیٹا = (ساخت Curl_easy *)*abstract;

#اگر_تعریف_شدہ CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", عدد_prompts);
#ورنہ
  (خالی)name;
  (خالی)name_len;
  (خالی)instruction;
  (خالی)instruction_len;
#اگر_ختم /* CURL_LIBSSH2_DEBUG */
  اگر(عدد_prompts == 1) {
    ساخت connectdata *conn = ڈیٹا->conn;
    ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].متن_3 = Curl_cstrdup(passwd);
    responses[0].لمبائی = responses[0].متن_3 ? curlx_uztoui(لڑی_لمبائی(passwd)) : 0;
  }
  (خالی)prompts;
} /* kbd_callback */

ساکن CURLcode sftp_libssh2_error_مقصد_CURLE(غیرعلامتی طویل err)
{
  انتخاب(err) {
  حالت LIBSSH2_FX_OK:
    واپس CURLE_OK;

  حالت LIBSSH2_FX_NO_SUCH_FILE:
  حالت LIBSSH2_FX_NO_SUCH_راستہ:
    واپس CURLE_REMOTE_FILE_NOT_ملا;

  حالت LIBSSH2_FX_PERMISSION_DENIED:
  حالت LIBSSH2_FX_WRITE_PROTECT:
  حالت LIBSSH2_FX_تالا_CONFLICT:
    واپس CURLE_REMOTE_ACCESS_DENIED;

  حالت LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  حالت LIBSSH2_FX_QUOTA_EXCEEDED:
    واپس CURLE_REMOTE_DISK_بھرا;

  حالت LIBSSH2_FX_FILE_ALREADY_EXISTS:
    واپس CURLE_REMOTE_FILE_EXISTS;

  حالت LIBSSH2_FX_DIR_NOT_خالی_خانے:
    واپس CURLE_QUOTE_ERROR;

  طےشدہ:
    توڑو;
  }

  واپس CURLE_SSH;
}

ساکن CURLcode libssh2_session_error_مقصد_CURLE(صحیح err)
{
  انتخاب(err) {
  /* Ordered by order of appearance in libssh2.h */
  حالت LIBSSH2_ERROR_NONE:
    واپس CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  حالت LIBSSH2_ERROR_SCP_PROTOCOL:
    واپس CURLE_REMOTE_FILE_NOT_ملا;

  حالت LIBSSH2_ERROR_SOCKET_NONE:
    واپس CURLE_COULDNT_CONNECT;

  حالت LIBSSH2_ERROR_ALLOC:
    واپس CURLE_برآمد_کا_حافظہ;

  حالت LIBSSH2_ERROR_SOCKET_SEND:
    واپس CURLE_SEND_ERROR;

  حالت LIBSSH2_ERROR_HOSTKEY_INIT:
  حالت LIBSSH2_ERROR_HOSTKEY_SIGN:
  حالت LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  حالت LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    واپس CURLE_PEER_FAILED_VERIFICATION;

  حالت LIBSSH2_ERROR_PASSWORD_EXPIRED:
    واپس CURLE_LOGIN_DENIED;

  حالت LIBSSH2_ERROR_SOCKET_TIMEOUT:
  حالت LIBSSH2_ERROR_TIMEOUT:
    واپس CURLE_OPERATION_TIMEDOUT;

  حالت LIBSSH2_ERROR_EAGAIN:
    واپس CURLE_AGAIN;
  }

  واپس CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

ساکن LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (خالی)abstract;
  واپس Curl_cmalloc(گنتی);
}

ساکن LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (خالی)abstract;
  واپس Curl_crealloc(ptr, گنتی);
}

ساکن LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (خالی)abstract;
  اگر(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

ساکن صحیح sshkeycallback(CURL *easy,
                          ثابت ساخت curl_khkey *knownkey, /*معلوم*/
                          ثابت ساخت curl_khkey *foundkey, /* found */
                          شمار curl_khmatch مطابقت,
                          خالی *clientp)
{
  (خالی)easy;
  (خالی)knownkey;
  (خالی)foundkey;
  (خالی)clientp;

  /* we only allow perfect matches, and we reject everything else */
  واپس (مطابقت != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ساکن شمار curl_khtype convert_ssh2_keytype(صحیح sshkeytype)
{
  شمار curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  انتخاب(sshkeytype) {
  حالت LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    توڑو;
#اگر_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_DSS
  حالت LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_ED25519
  حالت LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    توڑو;
#اگر_ختم
  }
  واپس keytype;
}

ساکن CURLcode ssh_knownhost(ساخت Curl_easy *ڈیٹا,
                              ساخت ssh_conn *sshc)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت libssh2_knownhost *host = تہی;
  ثابت حرف *remotekey = تہی;
  صحیح keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  صحیح keybit = 0;
  صحیح sshkeytype = 0;
  size_t keylen = 0;
  صحیح rc = 0;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS)) {
    infof(ڈیٹا, "SSH: no knownhosts file configured");
    واپس CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  اگر(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    ساخت Curl_mapi_guard guard;
    شمار curl_khmatch keymatch;
    curl_sshkeycallback func =
      ڈیٹا->سیٹ.ssh_keyfunc ? ڈیٹا->سیٹ.ssh_keyfunc : sshkeycallback;
    ساخت curl_khkey knownkey;
    ساخت curl_khkey *knownkeyp = تہی;
    ساخت curl_khkey foundkey;

    انتخاب(sshkeytype) {
    حالت LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_کلید_SSHRSA;
      توڑو;
#اگر_تعریف_شدہ LIBSSH2_HOSTKEY_TYPE_DSS
    حالت LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_کلید_SSHDSS;
      توڑو;
#اگر_ختم
    حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_کلید_ECDSA_256;
      توڑو;
    حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_کلید_ECDSA_384;
      توڑو;
    حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_کلید_ECDSA_521;
      توڑو;
    حالت LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_کلید_ED25519;
      توڑو;
    طےشدہ:
      infof(ڈیٹا, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      توڑو;
    }
    اگر(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    ورنہ {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->پورٹ != پورٹ_SSH) ?
                                          conn->origin->پورٹ : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(ڈیٹا, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->کلید : "<none>");

      /* setup 'knownkey' */
      اگر(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
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
      keymatch = (شمار curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_ssh_keyfunc);
      rc = func(ڈیٹا, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, ڈیٹا->سیٹ.ssh_keyfunc_userp);
      CURL_CBAPI_انتہا(&guard);
    }
  }
  ورنہ {
    /* no remotekey means failure! */
    infof(ڈیٹا, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  انتخاب(rc) {
  طےشدہ: /* unknown return codes is the same as reject */
  حالت CURLKHSTAT_REJECT:
    infof(ڈیٹا, "SSH: knownhost check failed");
    myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  حالت CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    توڑو;
  حالت CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    اگر(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  حالت CURLKHSTAT_FINE:
  حالت CURLKHSTAT_FINE_جوڑو_مقصد_FILE:
    /* proceed */
    اگر(keycheck != LIBSSH2_KNOWNHOST_CHECK_مطابقت) {
      صحیح addrc;
      ثابت حرف *hostbuf;
      حرف *hostport = تہی;
      اگر(conn->origin->پورٹ != پورٹ_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->پورٹ);
        اگر(!hostbuf)
          infof(ڈیٹا, "WARNING: failed allocating buffer for [host]:port");
      }
      ورنہ
        hostbuf = conn->origin->hostname;
      اگر(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, تہی,
                                       remotekey, keylen, تہی, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, تہی);
        اگر(addrc)
          infof(ڈیٹا, "WARNING: adding the known host %s failed", hostbuf);
        ورنہ اگر(rc == CURLKHSTAT_FINE_جوڑو_مقصد_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          صحیح wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          اگر(wrc) {
            infof(ڈیٹا, "WARNING: writing %s failed",
                  CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    ورنہ
      infof(ڈیٹا, "SSH: knownhost entry matches host key");
    توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode ssh_check_fingerprint(ساخت Curl_easy *ڈیٹا,
                                      ساخت ssh_conn *sshc)
{
  ثابت حرف *pubkey_md5 =
    CURL_EASY_STR(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_MD5);
  ثابت حرف *pubkey_sha256 =
    CURL_EASY_STR(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_SHA256);

  اگر(pubkey_sha256) {
    ثابت حرف *fingerprint = تہی;
    حرف *fingerprint_b64 = تہی;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(ڈیٹا, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    اگر(!fingerprint) {
      failf(ڈیٹا,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
      واپس CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    اگر(curlx_base64_encode((ثابت uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
      واپس CURLE_PEER_FAILED_VERIFICATION;
    }

    اگر(!fingerprint_b64) {
      failf(ڈیٹا, "SHA256 fingerprint could not be encoded");
      myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
      واپس CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(ڈیٹا, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

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
      myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
      واپس CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(ڈیٹا, "SSH: SHA256 checksum match");
  }

  اگر(pubkey_md5) {
    حرف md5buffer[33];
    ثابت حرف *fingerprint;

    infof(ڈیٹا, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    اگر(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      صحیح i;
      برائے(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (غیرعلامتی حرف)fingerprint[i]);
      }

      infof(ڈیٹا, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    اگر(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      اگر(fingerprint) {
        failf(ڈیٹا,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      ورنہ {
        failf(ڈیٹا,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
      واپس CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(ڈیٹا, "SSH: MD5 checksum match");
  }

  اگر(!pubkey_md5 && !pubkey_sha256) {
    اگر(ڈیٹا->سیٹ.ssh_hostkeyfunc) {
      size_t keylen = 0;
      صحیح sshkeytype = 0;
      صحیح rc = 0;
      /* we handle the process to the callback */
      ثابت حرف *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      اگر(remotekey) {
        ساخت Curl_mapi_guard guard;
        شمار curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_ssh_hostkeyfunc);
        rc = ڈیٹا->سیٹ.ssh_hostkeyfunc(ڈیٹا->سیٹ.ssh_hostkeyfunc_userp,
                                       (صحیح)keytype, remotekey, keylen);
        CURL_CBAPI_انتہا(&guard);
        اگر(rc != CURLKHMATCH_OK) {
          myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
          failf(ڈیٹا, "SSH: callback failed host public key verification");
          واپس CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(ڈیٹا, "SSH: verified public key via callback");
      }
      ورنہ {
        myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
        واپس CURLE_PEER_FAILED_VERIFICATION;
      }
      واپس CURLE_OK;
    }
    ورنہ {
      CURL_TRC_SSH(ڈیٹا, "no host key checksum given, checking knownhosts");
      واپس ssh_knownhost(ڈیٹا, sshc);
    }
  }
  ورنہ {
    /* as we already matched, we skip the check for known hosts */
    واپس CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
ساکن CURLcode ssh_force_knownhost_کلید_type(ساخت Curl_easy *ڈیٹا,
                                             ساخت ssh_conn *sshc)
{
  CURLcode نتیجہ = CURLE_OK;

  ساکن ثابت حرف hostkey_طریقہ_ssh_ed25519[] = "ssh-ed25519";
  ساکن ثابت حرف hostkey_طریقہ_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ساکن ثابت حرف hostkey_طریقہ_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ساکن ثابت حرف hostkey_طریقہ_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ساکن ثابت حرف hostkey_طریقہ_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#اگر_تعریف_شدہ LIBSSH2_KNOWNHOST_کلید_SSHDSS
  ساکن ثابت حرف hostkey_طریقہ_ssh_dss[] = "ssh-dss";
#اگر_ختم
  bool ملا = FALSE;

  اگر(sshc->kh &&
     !CURL_EASY_STR(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_MD5) &&
     !CURL_EASY_STR(ڈیٹا, STRING_SSH_HOST_عوامی_کلید_SHA256)) {
    ساخت libssh2_knownhost *store = تہی;
    ساخت connectdata *conn = ڈیٹا->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_کلید_نقاب >>
                      LIBSSH2_KNOWNHOST_کلید_سرکاؤ) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    جب(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      اگر(store) {
        اگر(store->name) {
          اگر(store->name[0] == '[') {
            curl_off_t پورٹ;
            size_t kh_name_سائز = 0;
            ثابت حرف *p;
            ثابت حرف *kh_name_انتہا = لڑی_تلاش(store->name, "]:");
            اگر(!kh_name_انتہا) {
              infof(ڈیٹا, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS));
              جاری;
            }
            p = kh_name_انتہا + 2; /* start of port number */
            اگر(!curlx_str_نمبر(&p, &پورٹ, 0xffff) &&
               (kh_name_انتہا && (پورٹ == conn->origin->پورٹ))) {
              kh_name_سائز = لڑی_لمبائی(store->name) - 1 - لڑی_لمبائی(kh_name_انتہا);
              اگر((kh_name_سائز == لڑی_لمبائی(conn->origin->hostname)) &&
                 !لڑی_موازنہ(store->name + 1, conn->origin->hostname,
                          kh_name_سائز)) {
                ملا = TRUE;
                توڑو;
              }
            }
          }
          ورنہ اگر(!strcmp(store->name, conn->origin->hostname)) {
            ملا = TRUE;
            توڑو;
          }
        }
        ورنہ اگر(store->کلید) {
          غیرعلامتی صحیح type = (store->typemask & LIBSSH2_KNOWNHOST_کلید_نقاب)
                              >> LIBSSH2_KNOWNHOST_کلید_سرکاؤ;
          اگر(!absent_type[type]) {
            صحیح keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->پورٹ != پورٹ_SSH) ? conn->origin->پورٹ : -1,
              store->کلید, لڑی_لمبائی(store->کلید),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_کلید_نقاب),
              تہی);
            اگر(keycheck == LIBSSH2_KNOWNHOST_CHECK_مطابقت) {
              ملا = TRUE;
              توڑو;
            }
            ورنہ اگر(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    اگر(ملا) {
      صحیح rc;
      ثابت حرف *hostkey_طریقہ = تہی;
      infof(ڈیٹا, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS));

      انتخاب(store->typemask & LIBSSH2_KNOWNHOST_کلید_نقاب) {
      حالت LIBSSH2_KNOWNHOST_کلید_ED25519:
        hostkey_طریقہ = hostkey_طریقہ_ssh_ed25519;
        توڑو;
      حالت LIBSSH2_KNOWNHOST_کلید_ECDSA_521:
        hostkey_طریقہ = hostkey_طریقہ_ssh_ecdsa_521;
        توڑو;
      حالت LIBSSH2_KNOWNHOST_کلید_ECDSA_384:
        hostkey_طریقہ = hostkey_طریقہ_ssh_ecdsa_384;
        توڑو;
      حالت LIBSSH2_KNOWNHOST_کلید_ECDSA_256:
        hostkey_طریقہ = hostkey_طریقہ_ssh_ecdsa_256;
        توڑو;
      حالت LIBSSH2_KNOWNHOST_کلید_SSHRSA:
        hostkey_طریقہ = hostkey_طریقہ_ssh_rsa_all;
        توڑو;
#اگر_تعریف_شدہ LIBSSH2_KNOWNHOST_کلید_SSHDSS
      حالت LIBSSH2_KNOWNHOST_کلید_SSHDSS:  /* deprecated upstream */
        hostkey_طریقہ = hostkey_طریقہ_ssh_dss;
        توڑو;
#اگر_ختم
      حالت LIBSSH2_KNOWNHOST_کلید_RSA1:
        failf(ڈیٹا, "Found host key type RSA1 which is not supported");
        واپس CURLE_SSH;
      طےشدہ:
        failf(ڈیٹا, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_کلید_نقاب));
        واپس CURLE_SSH;
      }

      infof(ڈیٹا, "SSH: set '%s' as hostkey type", hostkey_طریقہ);
      rc = libssh2_session_طریقہ_pref(sshc->ssh_session,
                                       LIBSSH2_طریقہ_HOSTKEY, hostkey_طریقہ);
      اگر(rc) {
        حرف *err_پیغام = تہی;
        صحیح errlen;
        libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, &errlen, 0);
        failf(ڈیٹا, "libssh2 method '%s' failed: %s", hostkey_طریقہ, err_پیغام);
        نتیجہ = libssh2_session_error_مقصد_CURLE(rc);
      }
    }
    ورنہ {
      infof(ڈیٹا, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS));
    }
  }

  واپس نتیجہ;
}

ساکن CURLcode quote_error(ساخت Curl_easy *ڈیٹا,
                            ساخت ssh_conn *sshc)
{
  failf(ڈیٹا, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  واپس CURLE_QUOTE_ERROR;
}

ساکن CURLcode sftp_quote(ساخت Curl_easy *ڈیٹا,
                           ساخت ssh_conn *sshc,
                           ساخت SSHPROTO *sshp)
{
  ثابت حرف *cp;
  CURLcode نتیجہ = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
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

  اگر(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    حرف *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->راستہ);
    اگر(!tmp)
      واپس CURLE_برآمد_کا_حافظہ;
    Curl_debug(ڈیٹا, CURLINFO_سرخی_برآمد, "PWD\n", 4);
    Curl_debug(ڈیٹا, CURLINFO_سرخی_اندر, tmp, لڑی_لمبائی(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, tmp, لڑی_لمبائی(tmp));
    curlx_free(tmp);
    اگر(!نتیجہ)
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
    واپس نتیجہ;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  اگر(!cp) {
    failf(ڈیٹا, "Syntax error command '%s', missing parameter", cmd);
    واپس CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  اگر(نتیجہ) {
    اگر(نتیجہ != CURLE_برآمد_کا_حافظہ)
      failf(ڈیٹا, "Syntax error: Bad first parameter to '%s'", cmd);
    واپس نتیجہ;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
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
      اگر(نتیجہ != CURLE_برآمد_کا_حافظہ)
        failf(ڈیٹا, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      واپس نتیجہ;
    }
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);

    memset(&sshp->quote_attrs, 0, حجم(LIBSSH2_SFTP_اوصاف));
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_STAT);
    واپس نتیجہ;
  }
  اگر(!لڑی_موازنہ(cmd, "ln ", 3) ||
     !لڑی_موازنہ(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    اگر(نتیجہ) {
      اگر(نتیجہ != CURLE_برآمد_کا_حافظہ)
        failf(ڈیٹا, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      واپس نتیجہ;
    }
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_SYMLINK);
    واپس نتیجہ;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "mkdir ", 6)) {
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    /* create directory */
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_MKDIR);
    واپس نتیجہ;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    نتیجہ = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    اگر(نتیجہ) {
      اگر(نتیجہ != CURLE_برآمد_کا_حافظہ)
        failf(ڈیٹا, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      واپس نتیجہ;
    }
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_RENAME);
    واپس نتیجہ;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "rmdir ", 6)) {
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    /* delete directory */
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_RMDIR);
    واپس نتیجہ;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "rm ", 3)) {
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_UNLINK);
    واپس نتیجہ;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "statvfs ", 8)) {
    اگر(*cp)
      واپس quote_error(ڈیٹا, sshc);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_STATVFS);
    واپس نتیجہ;
  }

  failf(ڈیٹا, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  واپس CURLE_QUOTE_ERROR;
}

ساکن CURLcode sftp_upload_init(ساخت Curl_easy *ڈیٹا,
                                 ساخت ssh_conn *sshc,
                                 ساخت SSHPROTO *sshp,
                                 bool *blockp)
{
  غیرعلامتی طویل flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  اگر(ڈیٹا->state.resume_from) {
    LIBSSH2_SFTP_اوصاف attrs;
    اگر(ڈیٹا->state.resume_from < 0) {
      صحیح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->راستہ,
                                    curlx_uztoui(لڑی_لمبائی(sshp->راستہ)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      اگر(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        واپس CURLE_OK;
      }
      اگر(rc) {
        ڈیٹا->state.resume_from = 0;
      }
      ورنہ {
        curl_off_t سائز = attrs.filesize;
        اگر(سائز < 0) {
          failf(ڈیٹا, "Bad file size (%" FMT_OFF_T ")", سائز);
          واپس CURLE_BAD_DOWNLOAD_RESUME;
        }
        ڈیٹا->state.resume_from = attrs.filesize;
      }
    }
  }

  اگر(ڈیٹا->سیٹ.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  ورنہ اگر(ڈیٹا->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  ورنہ {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->راستہ,
                         curlx_uztoui(لڑی_لمبائی(sshp->راستہ)),
                         flags, (طویل)ڈیٹا->سیٹ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  اگر(!sshc->sftp_handle) {
    CURLcode نتیجہ;
    غیرعلامتی طویل sftperr;
    صحیح rc = libssh2_session_آخری_errno(sshc->ssh_session);

    اگر(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      واپس CURLE_OK;
    }

    اگر(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    ورنہ
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    اگر(sshc->دوسرا_Create_Dirs) {
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      failf(ڈیٹا, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      واپس sftp_libssh2_error_مقصد_CURLE(sftperr);
    }
    اگر((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_راستہ) &&
       ڈیٹا->سیٹ.ftp_create_missing_dirs &&
       (لڑی_لمبائی(sshp->راستہ) > 1)) {
      /* try to create the path remotely */
      sshc->دوسرا_Create_Dirs = 1;
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      واپس CURLE_OK;
    }
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    نتیجہ = sftp_libssh2_error_مقصد_CURLE(sftperr);
    اگر(!نتیجہ) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      نتیجہ = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(ڈیٹا, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    واپس نتیجہ;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
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
        واپس CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      کرو {
        ساخت Curl_mapi_guard guard;
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (ڈیٹا->state.resume_from - passed >
           (curl_off_t)حجم(scratch)) ?
          حجم(scratch) : curlx_sotouz(ڈیٹا->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fread_func);
        actuallyread = ڈیٹا->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              ڈیٹا->state.اندر);
        CURL_CBAPI_انتہا(&guard);

        passed += actuallyread;
        اگر((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(ڈیٹا, "Failed to read data");
          واپس CURLE_FTP_COULDNT_USE_REST;
        }
      } جب(passed < ڈیٹا->state.resume_from);
    }

    /* now, decrease the size of the read */
    اگر(ڈیٹا->state.infilesize > 0) {
      ڈیٹا->state.infilesize -= ڈیٹا->state.resume_from;
      ڈیٹا->req.سائز = ڈیٹا->state.infilesize;
      Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, ڈیٹا->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)ڈیٹا->state.resume_from);
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
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(ڈیٹا);

  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس CURLE_OK;
}

ساکن خالی ssh_state_pkey_init(ساخت Curl_easy *ڈیٹا, ساخت ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  اگر((ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     لڑی_تلاش(sshc->authlist, "publickey")) {
    اگر(sshc->pub_کلید)
      infof(ڈیٹا, "SSH: trying public key file '%s'", sshc->pub_کلید);
    infof(ڈیٹا, "SSH: trying private key file '%s'", sshc->priv_کلید);
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PKEY);
  }
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PASS_INIT);
}

ساکن CURLcode sftp_quote_stat(ساخت Curl_easy *ڈیٹا,
                                ساخت ssh_conn *sshc,
                                ساخت SSHPROTO *sshp,
                                bool *blockp)
{
  ثابت حرف *cmd = sshc->quote_شے->ڈیٹا;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  اگر(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  اگر(لڑی_موازنہ(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    صحیح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(لڑی_لمبائی(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    اگر(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      واپس CURLE_OK;
    }
    اگر(rc && !sshc->acceptfail) { /* get those attributes */
      غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
      failf(ڈیٹا, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      جاؤ fail;
    }
  }

  /* Now set the new attributes... */
  اگر(!لڑی_موازنہ(cmd, "chgrp", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t gid;
    اگر(!curlx_str_نمبر(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (غیرعلامتی طویل)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ورنہ اگر(!sshc->acceptfail) {
      failf(ڈیٹا, "Syntax error: chgrp gid not a number");
      جاؤ fail;
    }
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت حرف *p = sshc->quote_path1;
    /* permissions are octal */
    اگر(curlx_str_octal(&p, &perms, 07777)) {
      failf(ڈیٹا, "Syntax error: chmod permissions not a number");
      جاؤ fail;
    }

    sshp->quote_attrs.permissions = (غیرعلامتی طویل)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "chown", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t uid;
    اگر(!curlx_str_نمبر(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (غیرعلامتی طویل)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ورنہ اگر(!sshc->acceptfail) {
      failf(ڈیٹا, "Syntax error: chown uid not a number");
      جاؤ fail;
    }
  }
  ورنہ اگر(!لڑی_موازنہ(cmd, "atime", 5) ||
          !لڑی_موازنہ(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    اگر(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(ڈیٹا, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#اگر SIZEOF_وقت_T > SIZEOF_LONG
    اگر(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(ڈیٹا, "date overflow");
      fail = TRUE;
    }
#اگر_ختم
    اگر(fail)
      جاؤ fail;
    اگر(!لڑی_موازنہ(cmd, "atime", 5))
      sshp->quote_attrs.atime = (غیرعلامتی طویل)date;
    ورنہ /* mtime */
      sshp->quote_attrs.mtime = (غیرعلامتی طویل)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE_SETSTAT);
  واپس CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  واپس CURLE_QUOTE_ERROR;
}

ساکن CURLcode sftp_download_stat(ساخت Curl_easy *ڈیٹا,
                                   ساخت ssh_conn *sshc,
                                   ساخت SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_اوصاف attrs;
  صحیح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->راستہ,
                                curlx_uztoui(لڑی_لمبائی(sshp->راستہ)),
                                LIBSSH2_SFTP_STAT, &attrs);
  اگر(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    واپس CURLE_OK;
  }
  اگر(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_سائز) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    ڈیٹا->req.سائز = -1;
    ڈیٹا->req.maxdownload = -1;
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  ورنہ {
    curl_off_t سائز = attrs.filesize;

    اگر(سائز < 0) {
      failf(ڈیٹا, "Bad file size (%" FMT_OFF_T ")", سائز);
      واپس CURLE_BAD_DOWNLOAD_RESUME;
    }
    اگر(ڈیٹا->state.use_range) {
      curl_off_t from;
      CURLcode نتیجہ = Curl_ssh_range(ڈیٹا, ڈیٹا->state.range, سائز,
                                       &from, &سائز);
      اگر(نتیجہ)
        واپس نتیجہ;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    ڈیٹا->req.سائز = سائز;
    ڈیٹا->req.maxdownload = سائز;
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, سائز);
  }

  /* We can resume if we can seek to the resume position */
  اگر(ڈیٹا->state.resume_from) {
    اگر(ڈیٹا->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      اگر((curl_off_t)attrs.filesize < -ڈیٹا->state.resume_from) {
        failf(ڈیٹا, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              ڈیٹا->state.resume_from, (curl_off_t)attrs.filesize);
        واپس CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      ڈیٹا->state.resume_from += attrs.filesize;
    }
    ورنہ {
      اگر((curl_off_t)attrs.filesize < ڈیٹا->state.resume_from) {
        failf(ڈیٹا, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              ڈیٹا->state.resume_from, (curl_off_t)attrs.filesize);
        واپس CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    ڈیٹا->req.سائز = attrs.filesize - ڈیٹا->state.resume_from;
    ڈیٹا->req.maxdownload = attrs.filesize - ڈیٹا->state.resume_from;
    Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, attrs.filesize - ڈیٹا->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)ڈیٹا->state.resume_from);
  }

  /* Setup the actual download */
  اگر(ڈیٹا->req.سائز == 0) {
    /* no data to transfer */
    Curl_xfer_تیاری_nop(ڈیٹا);
    infof(ڈیٹا, "SSH: file already completely downloaded");
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
    واپس CURLE_OK;
  }
  Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, ڈیٹا->req.سائز);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ڈیٹا->conn->send_idx = 0;

  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);

  واپس CURLE_OK;
}

ساکن CURLcode sftp_readdir(ساخت Curl_easy *ڈیٹا,
                             ساخت ssh_conn *sshc,
                             ساخت SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode نتیجہ = CURLE_OK;
  صحیح rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_راستہ_MAX,
                                   sshp->readdir_longentry, CURL_راستہ_MAX,
                                   &sshp->readdir_attrs);
  اگر(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    واپس نتیجہ;
  }
  اگر(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    اگر(ڈیٹا->سیٹ.list_only) {
      نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      اگر(!نتیجہ)
        نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY, "\n", 1);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ورنہ {
      نتیجہ = curlx_dyn_جوڑو(&sshp->readdir, sshp->readdir_longentry);

      اگر(!نتیجہ) {
        اگر((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          نتیجہ = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->راستہ,
                                  sshp->readdir_filename);
          myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_LINK);
        }
        ورنہ {
          myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      واپس نتیجہ;
    }
  }
  ورنہ اگر(!rc) {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_مکمل);
  }
  ورنہ {
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    نتیجہ = sftperr ? sftp_libssh2_error_مقصد_CURLE(sftperr) : CURLE_SSH;
    failf(ڈیٹا, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_آخری_errno(sshc->ssh_session));
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
  }
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_init(ساخت Curl_easy *ڈیٹا,
                               ساخت ssh_conn *sshc)
{
  CURLcode نتیجہ;
  sshc->دوسرا_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_سیٹ_blocking(sshc->ssh_session, 0);

  نتیجہ = ssh_force_knownhost_کلید_type(ڈیٹا, sshc);
  اگر(نتیجہ)
    myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_S_STARTUP);
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_startup(ساخت Curl_easy *ڈیٹا,
                                  ساخت ssh_conn *sshc)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  صحیح rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc) {
    حرف *err_پیغام = تہی;
    (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
    failf(ڈیٹا, "Failure establishing ssh session: %d, %s", rc, err_پیغام);

    myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
    واپس CURLE_FAILED_INIT;
  }

  myssh_مقصد(ڈیٹا, sshc, SSH_HOSTKEY);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_hostkey(ساخت Curl_easy *ڈیٹا,
                                  ساخت ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode نتیجہ = ssh_check_fingerprint(ڈیٹا, sshc);
  اگر(!نتیجہ)
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTHLIST);
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_authlist(ساخت Curl_easy *ڈیٹا,
                                   ساخت ssh_conn *sshc)
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
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(لڑی_لمبائی(user)));

  اگر(!sshc->authlist) {
    صحیح rc;
    اگر(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(ڈیٹا, "SSH: user accepted with no authentication");
      myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
      واپس CURLE_OK;
    }
    rc = libssh2_session_آخری_errno(sshc->ssh_session);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
    واپس libssh2_session_error_مقصد_CURLE(rc);
  }
  infof(ڈیٹا, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PKEY_INIT);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_auth_pkey(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحیح rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(لڑی_لمبائی(user)),
    sshc->pub_کلید, sshc->priv_کلید, sshc->passphrase ? sshc->passphrase : "");
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc == 0) {
    sshc->authed = TRUE;
    infof(ڈیٹا, "SSH: authenticated via publickey");
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
  }
  ورنہ {
    حرف *err_پیغام = تہی;
    حرف unknown[] = "Reason unknown (-1)";
    اگر(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_پیغام = unknown;
    }
    ورنہ {
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
    }
    infof(ڈیٹا, "SSH: publickey authentication denied: %s", err_پیغام);
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PASS_INIT);
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_auth_pass_init(ساخت Curl_easy *ڈیٹا,
                                         ساخت ssh_conn *sshc)
{
  اگر((ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     لڑی_تلاش(sshc->authlist, "password")) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_PASS);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_HOST_INIT);
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_auth_pass(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
  صحیح rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(لڑی_لمبائی(user)),
                                 passwd,
                                 curlx_uztoui(لڑی_لمبائی(passwd)),
                                 تہی);
  اگر(rc == LIBSSH2_ERROR_EAGAIN) {
    واپس CURLE_AGAIN;
  }
  اگر(rc == 0) {
    sshc->authed = TRUE;
    infof(ڈیٹا, "SSH: initialized password authentication");
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_HOST_INIT);
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_auth_host_init(ساخت Curl_easy *ڈیٹا,
                                         ساخت ssh_conn *sshc)
{
  اگر((ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     لڑی_تلاش(sshc->authlist, "hostbased")) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_HOST);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_AGENT_INIT);
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_auth_agent_init(ساخت Curl_easy *ڈیٹا,
                                          ساخت ssh_conn *sshc)
{
  صحیح rc = 0;
  اگر((ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     لڑی_تلاش(sshc->authlist, "publickey")) {

    infof(ڈیٹا, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    اگر(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      اگر(!sshc->ssh_agent) {
        infof(ڈیٹا, "SSH: could not create agent object");

        myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_کلید_INIT);
        واپس CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;
    اگر(rc < 0) {
      infof(ڈیٹا, "SSH: failure connecting to agent");
      myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_کلید_INIT);
    }
    ورنہ {
      myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_کلید_INIT);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_auth_agent_list(ساخت Curl_easy *ڈیٹا,
                                          ساخت ssh_conn *sshc)
{
  صحیح rc = libssh2_agent_list_identities(sshc->ssh_agent);

  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;
  اگر(rc < 0) {
    infof(ڈیٹا, "SSH: failure requesting identities to agent");
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_کلید_INIT);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_پچھلا_شناختی_عنصر = تہی;
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_auth_agent(ساخت Curl_easy *ڈیٹا,
                                     ساخت ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  صحیح rc = libssh2_agent_get_شناختی_عنصر(sshc->ssh_agent,
                                      &sshc->sshagent_شناختی_عنصر,
                                      sshc->sshagent_پچھلا_شناختی_عنصر);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc == 0) {
    CURL_TRC_SSH(ڈیٹا, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(ڈیٹا->conn->creds),
                 sshc->sshagent_شناختی_عنصر->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(ڈیٹا->conn->creds),
                                sshc->sshagent_شناختی_عنصر);

    اگر(rc < 0) {
      اگر(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_پچھلا_شناختی_عنصر = sshc->sshagent_شناختی_عنصر;
        واپس CURLE_OK;
      }
      واپس CURLE_AGAIN;
    }
  }

  اگر(rc < 0)
    infof(ڈیٹا, "SSH: failure requesting identities to agent");
  ورنہ اگر(rc == 1)
    infof(ڈیٹا, "SSH: no agent identity would match");

  اگر(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(ڈیٹا, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(ڈیٹا->conn->creds),
          sshc->sshagent_شناختی_عنصر->comment);
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_کلید_INIT);
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_auth_کلید_init(ساخت Curl_easy *ڈیٹا,
                                        ساخت ssh_conn *sshc)
{
  اگر((ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     لڑی_تلاش(sshc->authlist, "keyboard-interactive")) {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_کلید);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_auth_کلید(ساخت Curl_easy *ڈیٹا,
                                   ساخت ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  ساخت connectdata *conn = ڈیٹا->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحیح rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(لڑی_لمبائی(user)),
                                             &kbd_callback);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc == 0) {
    sshc->authed = TRUE;
    infof(ڈیٹا, "SSH: initialized keyboard interactive authentication");
    myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_مکمل);
    واپس CURLE_OK;
  }
  واپس CURLE_LOGIN_DENIED;
}

ساکن CURLcode ssh_state_auth_مکمل(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  اگر(!sshc->authed) {
    failf(ڈیٹا, "Authentication failure");
    myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
    واپس CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(ڈیٹا, "SSH: authentication complete");

  Curl_pgrs_وقت(ڈیٹا, TIMER_APPCONNECT); /* SSH is connected */

  ڈیٹا->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  اگر(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_INIT);
    واپس CURLE_OK;
  }
  infof(ڈیٹا, "SSH: connection established");
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_init(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  اگر(!sshc->sftp_session) {
    حرف *err_پیغام = تہی;
    اگر(libssh2_session_آخری_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
    failf(ڈیٹا, "Failure initializing sftp session: %s", err_پیغام);
    myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
    واپس CURLE_FAILED_INIT;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_REALPATH);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_realpath(ساخت Curl_easy *ڈیٹا,
                                        ساخت ssh_conn *sshc,
                                        ساخت SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  صحیح rc;

  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_راستہ_MAX,
                               LIBSSH2_SFTP_REALPATH);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  اگر(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    اگر(!sshc->homedir)
      واپس CURLE_برآمد_کا_حافظہ;
    curlx_free(ڈیٹا->state.most_recent_ftp_entrypath);
    ڈیٹا->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    اگر(!ڈیٹا->state.most_recent_ftp_entrypath)
      واپس CURLE_برآمد_کا_حافظہ;
  }
  ورنہ {
    /* Return the error type */
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    CURLcode نتیجہ;
    اگر(sftperr)
      نتیجہ = sftp_libssh2_error_مقصد_CURLE(sftperr);
    ورنہ
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      نتیجہ = CURLE_SSH;
    CURL_TRC_SSH(ڈیٹا, "error = %lu makes libcurl = %d", sftperr, (صحیح)نتیجہ);
    واپس نتیجہ;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(ڈیٹا, "CONNECT phase done");
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_quote_init(ساخت Curl_easy *ڈیٹا,
                                          ساخت ssh_conn *sshc,
                                          ساخت SSHPROTO *sshp)
{
  CURLcode نتیجہ;

  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  نتیجہ = Curl_getworkingpath(ڈیٹا, sshc->homedir, &sshp->راستہ);
  اگر(نتیجہ) {
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
    واپس نتیجہ;
  }

  اگر(ڈیٹا->سیٹ.quote) {
    infof(ڈیٹا, "SSH: sending quote commands");
    sshc->quote_شے = ڈیٹا->سیٹ.quote;
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_GETINFO);
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_postquote_init(ساخت Curl_easy *ڈیٹا,
                                              ساخت ssh_conn *sshc)
{
  اگر(ڈیٹا->سیٹ.postquote) {
    infof(ڈیٹا, "SSH: sending quote commands");
    sshc->quote_شے = ڈیٹا->سیٹ.postquote;
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_QUOTE);
  }
  ورنہ {
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_quote(ساخت Curl_easy *ڈیٹا,
                                     ساخت ssh_conn *sshc,
                                     ساخت SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode نتیجہ;

  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  نتیجہ = sftp_quote(ڈیٹا, sshc, sshp);
  اگر(نتیجہ) {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_sftp_اگلا_quote(ساخت Curl_easy *ڈیٹا,
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
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_quote_stat(ساخت Curl_easy *ڈیٹا,
                                          ساخت ssh_conn *sshc,
                                          ساخت SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode نتیجہ;

  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  نتیجہ = sftp_quote_stat(ڈیٹا, sshc, sshp, blockp);
  اگر(نتیجہ) {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_sftp_quote_setstat(ساخت Curl_easy *ڈیٹا,
                                             ساخت ssh_conn *sshc,
                                             ساخت SSHPROTO *sshp)
{
  صحیح rc;

  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(لڑی_لمبائی(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(ڈیٹا, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    واپس CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_quote_symlink(ساخت Curl_easy *ڈیٹا,
                                             ساخت ssh_conn *sshc)
{
  صحیح rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(لڑی_لمبائی(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(ڈیٹا, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    واپس CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_quote_mkdir(ساخت Curl_easy *ڈیٹا,
                                           ساخت ssh_conn *sshc)
{
  صحیح rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)),
                                 (طویل)ڈیٹا->سیٹ.new_directory_perms);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(ڈیٹا, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    واپس CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_quote_rename(ساخت Curl_easy *ڈیٹا,
                                            ساخت ssh_conn *sshc)
{
  صحیح rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(لڑی_لمبائی(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(ڈیٹا, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    واپس CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_quote_rmdir(ساخت Curl_easy *ڈیٹا,
                                           ساخت ssh_conn *sshc)
{
  صحیح rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)));
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(ڈیٹا, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    واپس CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_quote_unlink(ساخت Curl_easy *ڈیٹا,
                                            ساخت ssh_conn *sshc)
{
  صحیح rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)));
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(ڈیٹا, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    واپس CURLE_QUOTE_ERROR;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_quote_statvfs(ساخت Curl_easy *ڈیٹا,
                                             ساخت ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  صحیح rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(لڑی_لمبائی(sshc->quote_path1)),
                                &statvfs);

  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(ڈیٹا, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    واپس CURLE_QUOTE_ERROR;
  }
  ورنہ اگر(rc == 0) {
#اگر_تعریف_شدہ _MSC_VER
#تعریف_کرو CURL_LIBSSH2_VFS_سائز_نقاب "I64u"
#ورنہ
#تعریف_کرو CURL_LIBSSH2_VFS_سائز_نقاب "llu"
#اگر_ختم
    CURLcode نتیجہ;
    حرف *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_سائز_نقاب "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_بلاکیں, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    اگر(!tmp) {
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      واپس CURLE_برآمد_کا_حافظہ;
    }

    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_سرخی, tmp, لڑی_لمبائی(tmp));
    curlx_free(tmp);
    اگر(نتیجہ) {
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      واپس نتیجہ;
    }
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_اگلا_QUOTE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_create_dirs_mkdir(ساخت Curl_easy *ڈیٹا,
                                                 ساخت ssh_conn *sshc,
                                                 ساخت SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  صحیح rc;

  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->راستہ,
                             curlx_uztoui(لڑی_لمبائی(sshp->راستہ)),
                             (طویل)ڈیٹا->سیٹ.new_directory_perms);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  اگر(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    غیرعلامتی طویل sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    اگر((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
      واپس sftp_libssh2_error_مقصد_CURLE(sftperr);
    }
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CREATE_DIRS);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_readdir_init(ساخت Curl_easy *ڈیٹا,
                                            ساخت ssh_conn *sshc,
                                            ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  Curl_pgrs_سیٹ_Download_سائز(ڈیٹا, -1);
  اگر(ڈیٹا->req.no_body) {
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
    واپس CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->راستہ,
                         curlx_uztoui(لڑی_لمبائی(sshp->راستہ)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  اگر(!sshc->sftp_handle) {
    غیرعلامتی طویل sftperr;
    اگر(libssh2_session_آخری_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(ڈیٹا, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    واپس sftp_libssh2_error_مقصد_CURLE(sftperr);
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_readdir_link(ساخت Curl_easy *ڈیٹا,
                                            ساخت ssh_conn *sshc,
                                            ساخت SSHPROTO *sshp)
{
  CURLcode نتیجہ;
  صحیح rc;
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (غیرعلامتی صحیح)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_راستہ_MAX, LIBSSH2_SFTP_READLINK);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  اگر(rc < 0)
    واپس CURLE_برآمد_کا_حافظہ;

  /* append filename and extra output */
  نتیجہ = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  اگر(نتیجہ)
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_BOTTOM);
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_scp_download_init(ساخت Curl_easy *ڈیٹا,
                                            ساخت ssh_conn *sshc,
                                            ساخت SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, حجم(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->راستہ, &sb);
  اگر(!sshc->ssh_channel) {
    صحیح ssh_err;
    حرف *err_پیغام = تہی;

    اگر(libssh2_session_آخری_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    ssh_err = libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
    failf(ڈیٹا, "%s", err_پیغام);
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_CHANNEL_FREE);
    واپس libssh2_session_error_مقصد_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_سائز;
  ڈیٹا->req.maxdownload = (curl_off_t)sb.st_سائز;
  Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ڈیٹا->conn->send_idx = 0;

  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_close(ساخت Curl_easy *ڈیٹا,
                                     ساخت ssh_conn *sshc,
                                     ساخت SSHPROTO *sshp)
{
  صحیح rc = 0;
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  اگر(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    اگر(rc < 0) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to close libssh2 file: %d %s", rc, err_پیغام);
    }
    sshc->sftp_handle = تہی;
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
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);

  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_shutdown(ساخت Curl_easy *ڈیٹا,
                                        ساخت ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  صحیح rc = 0;
  اگر(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    اگر(rc < 0) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to close libssh2 file: %d %s", rc, err_پیغام);
    }
    sshc->sftp_handle = تہی;
  }
  اگر(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    اگر(rc < 0) {
      infof(ڈیٹا, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = تہی;
  }

  curlx_safefree(sshc->homedir);

  myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_DISCONNECT);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_download_init(ساخت Curl_easy *ڈیٹا,
                                             ساخت ssh_conn *sshc,
                                             ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->راستہ,
                         curlx_uztoui(لڑی_لمبائی(sshp->راستہ)),
                         LIBSSH2_FXF_READ, (طویل)ڈیٹا->سیٹ.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  اگر(!sshc->sftp_handle) {
    غیرعلامتی طویل sftperr;
    اگر(libssh2_session_آخری_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      واپس CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_آخری_error(sshc->sftp_session);
    failf(ڈیٹا, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    واپس sftp_libssh2_error_مقصد_CURLE(sftperr);
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_DOWNLOAD_STAT);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_scp_upload_init(ساخت Curl_easy *ڈیٹا,
                                          ساخت ssh_conn *sshc,
                                          ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->راستہ,
                       (صحیح)ڈیٹا->سیٹ.new_file_perms,
                       (libssh2_int64_t)ڈیٹا->state.infilesize, 0, 0);
  اگر(!sshc->ssh_channel) {
    صحیح ssh_err;
    حرف *err_پیغام = تہی;
    CURLcode نتیجہ;
    اگر(libssh2_session_آخری_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    ssh_err = libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
    failf(ڈیٹا, "%s", err_پیغام);
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_CHANNEL_FREE);
    نتیجہ = libssh2_session_error_مقصد_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    اگر(نتیجہ == CURLE_SSH ||
       نتیجہ == CURLE_REMOTE_FILE_NOT_ملا)
      نتیجہ = CURLE_UPLOAD_FAILED;
    واپس نتیجہ;
  }

  /* upload data */
  ڈیٹا->req.سائز = ڈیٹا->state.infilesize;
  Curl_pgrs_سیٹ_Upload_سائز(ڈیٹا, ڈیٹا->state.infilesize);
  Curl_xfer_تیاری_send(ڈیٹا, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  ڈیٹا->conn->recv_idx = FIRSTSOCKET;

  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);

  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_session_disconnect(ساخت Curl_easy *ڈیٹا,
                                             ساخت ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  صحیح rc = 0;
  اگر(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    اگر(rc < 0) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیغام);
    }
    sshc->ssh_channel = تہی;
  }

  اگر(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;

    اگر(rc < 0) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to disconnect libssh2 session: %d %s", rc, err_پیغام);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_مقصد(ڈیٹا, sshc, SSH_SESSION_FREE);
  واپس CURLE_OK;
}

ساکن CURLcode sshc_cleanup(ساخت ssh_conn *sshc, ساخت Curl_easy *ڈیٹا,
                             bool بلاک)
{
  صحیح rc;

  اگر(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = تہی;
  }

  اگر(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    اگر((rc < 0) && ڈیٹا) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_پیغام);
    }
    اگر(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      واپس CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = تہی;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_شناختی_عنصر = تہی;
    sshc->sshagent_پچھلا_شناختی_عنصر = تہی;
  }

  اگر(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    اگر((rc < 0) && ڈیٹا) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to close libssh2 file: %d %s", rc, err_پیغام);
    }
    اگر(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      واپس CURLE_AGAIN;

    sshc->sftp_handle = تہی;
  }

  اگر(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    اگر((rc < 0) && ڈیٹا) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیغام);
    }
    اگر(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      واپس CURLE_AGAIN;

    sshc->ssh_channel = تہی;
  }

  اگر(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    اگر((rc < 0) && ڈیٹا) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_پیغام);
    }
    اگر(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      واپس CURLE_AGAIN;

    sshc->sftp_session = تہی;
  }

  اگر(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    اگر((rc < 0) && ڈیٹا) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session, &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to free libssh2 session: %d %s", rc, err_پیغام);
    }
    اگر(!بلاک && (rc == LIBSSH2_ERROR_EAGAIN))
      واپس CURLE_AGAIN;

    sshc->ssh_session = تہی;
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

  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_getinfo(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc)
{
  اگر(ڈیٹا->سیٹ.get_filetime)
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_FILETIME);
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_TRANS_INIT);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_filetime(ساخت Curl_easy *ڈیٹا,
                                        ساخت ssh_conn *sshc,
                                        ساخت SSHPROTO *sshp)
{
  LIBSSH2_SFTP_اوصاف attrs;
  صحیح rc;

  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->راستہ,
                            curlx_uztoui(لڑی_لمبائی(sshp->راستہ)),
                            LIBSSH2_SFTP_STAT, &attrs);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  اگر(rc == 0)
    ڈیٹا->info.filetime = (time_t)attrs.mtime;

  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_TRANS_INIT);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_trans_init(ساخت Curl_easy *ڈیٹا,
                                          ساخت ssh_conn *sshc,
                                          ساخت SSHPROTO *sshp)
{
  اگر(ڈیٹا->state.upload)
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_UPLOAD_INIT);
  ورنہ اگر(sshp) {
    size_t plen = لڑی_لمبائی(sshp->راستہ);
    اگر(plen && (sshp->راستہ[plen - 1] == '/'))
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR_INIT);
    ورنہ
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  ورنہ
    واپس CURLE_FAILED_INIT;
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_upload_init(ساخت Curl_easy *ڈیٹا,
                                           ساخت ssh_conn *sshc,
                                           ساخت SSHPROTO *sshp,
                                           bool *بلاک)
{
  CURLcode نتیجہ;
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  نتیجہ = sftp_upload_init(ڈیٹا, sshc, sshp, بلاک);
  اگر(نتیجہ) {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_sftp_create_dirs_init(ساخت Curl_easy *ڈیٹا,
                                                ساخت ssh_conn *sshc,
                                                ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  اگر(لڑی_لمبائی(sshp->راستہ) > 1) {
    sshc->slash_pos = sshp->راستہ + 1; /* ignore the leading '/' */
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_UPLOAD_INIT);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_create_dirs(ساخت Curl_easy *ڈیٹا,
                                           ساخت ssh_conn *sshc,
                                           ساخت SSHPROTO *sshp)
{
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  اگر(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(ڈیٹا, "SFTP: creating directory '%s'", sshp->راستہ);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    واپس CURLE_OK;
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_UPLOAD_INIT);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_readdir(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc,
                                       ساخت SSHPROTO *sshp,
                                       bool *بلاک)
{
  CURLcode نتیجہ;
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  نتیجہ = sftp_readdir(ڈیٹا, sshc, sshp, بلاک);
  اگر(نتیجہ)
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_sftp_readdir_bottom(ساخت Curl_easy *ڈیٹا,
                                              ساخت ssh_conn *sshc,
                                              ساخت SSHPROTO *sshp)
{
  CURLcode نتیجہ;
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;

  نتیجہ = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  اگر(!نتیجہ)
    نتیجہ = Curl_client_write(ڈیٹا, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  اگر(نتیجہ) {
    curlx_dyn_free(&sshp->readdir);
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  }
  ورنہ {
    curlx_dyn_reset(&sshp->readdir);
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_READDIR);
  }
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_sftp_readdir_مکمل(ساخت Curl_easy *ڈیٹا,
                                            ساخت ssh_conn *sshc)
{
  اگر(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;

  sshc->sftp_handle = تہی;

  /* no data to transfer */
  Curl_xfer_تیاری_nop(ڈیٹا);
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_sftp_download_stat(ساخت Curl_easy *ڈیٹا,
                                             ساخت ssh_conn *sshc,
                                             ساخت SSHPROTO *sshp,
                                             bool *بلاک)
{
  CURLcode نتیجہ;
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  نتیجہ = sftp_download_stat(ڈیٹا, sshc, sshp, بلاک);
  اگر(نتیجہ) {
    myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  واپس نتیجہ;
}

ساکن CURLcode ssh_state_scp_trans_init(ساخت Curl_easy *ڈیٹا,
                                         ساخت ssh_conn *sshc,
                                         ساخت SSHPROTO *sshp)
{
  CURLcode نتیجہ;
  اگر(!sshp)
    واپس CURLE_FAILED_INIT;
  نتیجہ = Curl_getworkingpath(ڈیٹا, sshc->homedir,
                               &sshp->راستہ);
  اگر(نتیجہ) {
    myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
    واپس نتیجہ;
  }

  اگر(ڈیٹا->state.upload) {
    اگر(ڈیٹا->state.infilesize < 0) {
      failf(ڈیٹا, "SCP requires a known file size for upload");
      myssh_مقصد(ڈیٹا, sshc, SSH_SCP_CHANNEL_FREE);
      واپس CURLE_UPLOAD_FAILED;
    }
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_DOWNLOAD_INIT);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_scp_مکمل(ساخت Curl_easy *ڈیٹا,
                                   ساخت ssh_conn *sshc)
{
  اگر(ڈیٹا->state.upload)
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_SEND_EOF);
  ورنہ
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_CHANNEL_FREE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_scp_send_eof(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc)
{
  اگر(sshc->ssh_channel) {
    صحیح rc = libssh2_channel_send_eof(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;
    اگر(rc) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session,
                                       &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_پیغام);
    }
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SCP_WAIT_EOF);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_scp_wait_eof(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc)
{
  اگر(sshc->ssh_channel) {
    صحیح rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;
    اگر(rc) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session,
                                       &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to get channel EOF: %d %s", rc, err_پیغام);
    }
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SCP_WAIT_CLOSE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_scp_wait_close(ساخت Curl_easy *ڈیٹا,
                                         ساخت ssh_conn *sshc)
{
  اگر(sshc->ssh_channel) {
    صحیح rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;
    اگر(rc) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session,
                                       &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Channel failed to close: %d %s", rc, err_پیغام);
    }
  }
  myssh_مقصد(ڈیٹا, sshc, SSH_SCP_CHANNEL_FREE);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_scp_channel_free(
  ساخت Curl_easy *ڈیٹا,
  ساخت ssh_conn *sshc)
{
  اگر(sshc->ssh_channel) {
    صحیح rc = libssh2_channel_free(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      واپس CURLE_AGAIN;
    اگر(rc < 0) {
      حرف *err_پیغام = تہی;
      (خالی)libssh2_session_آخری_error(sshc->ssh_session,
                                       &err_پیغام, تہی, 0);
      infof(ڈیٹا, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیغام);
    }
    sshc->ssh_channel = تہی;
  }
  CURL_TRC_SSH(ڈیٹا, "SCP DONE phase complete");
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس CURLE_OK;
}

ساکن CURLcode ssh_state_session_free(ساخت Curl_easy *ڈیٹا,
                                       ساخت ssh_conn *sshc)
{
  CURLcode نتیجہ = sshc_cleanup(sshc, ڈیٹا, FALSE);
  ساخت connectdata *conn = ڈیٹا->conn;
  اگر(نتیجہ)
    واپس نتیجہ;
  memset(sshc, 0, حجم(ساخت ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
  واپس CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ساکن CURLcode ssh_statemachine(ساخت Curl_easy *ڈیٹا,
                                 ساخت ssh_conn *sshc,
                                 ساخت SSHPROTO *sshp,
                                 bool *بلاک)
{
  CURLcode نتیجہ = CURLE_OK;
  *بلاک = 0; /* we are not blocking by default */

  کرو {
    انتخاب(sshc->state) {
    حالت SSH_INIT:
      نتیجہ = ssh_state_init(ڈیٹا, sshc);
      اگر(نتیجہ)
        توڑو;
      FALLTHROUGH();

    حالت SSH_S_STARTUP:
      نتیجہ = ssh_state_startup(ڈیٹا, sshc);
      اگر(نتیجہ)
        توڑو;
      FALLTHROUGH();

    حالت SSH_HOSTKEY:
      نتیجہ = ssh_state_hostkey(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTHLIST:
      نتیجہ = ssh_state_authlist(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_PKEY:
      نتیجہ = ssh_state_auth_pkey(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_PASS_INIT:
      نتیجہ = ssh_state_auth_pass_init(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_PASS:
      نتیجہ = ssh_state_auth_pass(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_HOST_INIT:
      نتیجہ = ssh_state_auth_host_init(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_HOST:
      myssh_مقصد(ڈیٹا, sshc, SSH_AUTH_AGENT_INIT);
      توڑو;

    حالت SSH_AUTH_AGENT_INIT:
      نتیجہ = ssh_state_auth_agent_init(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_AGENT_LIST:
      نتیجہ = ssh_state_auth_agent_list(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_AGENT:
      نتیجہ = ssh_state_auth_agent(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_کلید_INIT:
      نتیجہ = ssh_state_auth_کلید_init(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_کلید:
      نتیجہ = ssh_state_auth_کلید(ڈیٹا, sshc);
      توڑو;

    حالت SSH_AUTH_مکمل:
      نتیجہ = ssh_state_auth_مکمل(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_INIT:
      نتیجہ = ssh_state_sftp_init(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_REALPATH:
      نتیجہ = ssh_state_sftp_realpath(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_QUOTE_INIT:
      نتیجہ = ssh_state_sftp_quote_init(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_POSTQUOTE_INIT:
      نتیجہ = ssh_state_sftp_postquote_init(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_QUOTE:
      نتیجہ = ssh_state_sftp_quote(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_اگلا_QUOTE:
      نتیجہ = ssh_state_sftp_اگلا_quote(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_QUOTE_STAT:
      نتیجہ = ssh_state_sftp_quote_stat(ڈیٹا, sshc, sshp, بلاک);
      توڑو;

    حالت SSH_SFTP_QUOTE_SETSTAT:
      نتیجہ = ssh_state_sftp_quote_setstat(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_QUOTE_SYMLINK:
      نتیجہ = ssh_state_sftp_quote_symlink(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_QUOTE_MKDIR:
      نتیجہ = ssh_state_sftp_quote_mkdir(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_QUOTE_RENAME:
      نتیجہ = ssh_state_sftp_quote_rename(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_QUOTE_RMDIR:
      نتیجہ = ssh_state_sftp_quote_rmdir(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_QUOTE_UNLINK:
      نتیجہ = ssh_state_sftp_quote_unlink(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_QUOTE_STATVFS:
      نتیجہ = ssh_state_sftp_quote_statvfs(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_GETINFO:
      نتیجہ = ssh_state_sftp_getinfo(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_FILETIME:
      نتیجہ = ssh_state_sftp_filetime(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_TRANS_INIT:
      نتیجہ = ssh_state_sftp_trans_init(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_UPLOAD_INIT:
      نتیجہ = ssh_state_sftp_upload_init(ڈیٹا, sshc, sshp, بلاک);
      توڑو;

    حالت SSH_SFTP_CREATE_DIRS_INIT:
      نتیجہ = ssh_state_sftp_create_dirs_init(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_CREATE_DIRS:
      نتیجہ = ssh_state_sftp_create_dirs(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_CREATE_DIRS_MKDIR:
      نتیجہ = ssh_state_sftp_create_dirs_mkdir(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_READDIR_INIT:
      نتیجہ = ssh_state_sftp_readdir_init(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_READDIR:
      نتیجہ = ssh_state_sftp_readdir(ڈیٹا, sshc, sshp, بلاک);
      توڑو;

    حالت SSH_SFTP_READDIR_LINK:
      نتیجہ = ssh_state_sftp_readdir_link(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_READDIR_BOTTOM:
      نتیجہ = ssh_state_sftp_readdir_bottom(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_READDIR_مکمل:
      نتیجہ = ssh_state_sftp_readdir_مکمل(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SFTP_DOWNLOAD_INIT:
      نتیجہ = ssh_state_sftp_download_init(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_DOWNLOAD_STAT:
      نتیجہ = ssh_state_sftp_download_stat(ڈیٹا, sshc, sshp, بلاک);
      توڑو;

    حالت SSH_SFTP_CLOSE:
      نتیجہ = ssh_state_sftp_close(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SFTP_SHUTDOWN:
      نتیجہ = ssh_state_sftp_shutdown(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SCP_TRANS_INIT:
      نتیجہ = ssh_state_scp_trans_init(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SCP_UPLOAD_INIT:
      نتیجہ = ssh_state_scp_upload_init(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SCP_DOWNLOAD_INIT:
      نتیجہ = ssh_state_scp_download_init(ڈیٹا, sshc, sshp);
      توڑو;

    حالت SSH_SCP_مکمل:
      نتیجہ = ssh_state_scp_مکمل(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SCP_SEND_EOF:
      نتیجہ = ssh_state_scp_send_eof(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SCP_WAIT_EOF:
      نتیجہ = ssh_state_scp_wait_eof(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SCP_WAIT_CLOSE:
      نتیجہ = ssh_state_scp_wait_close(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SCP_CHANNEL_FREE:
      نتیجہ = ssh_state_scp_channel_free(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SESSION_DISCONNECT:
      نتیجہ = ssh_state_session_disconnect(ڈیٹا, sshc);
      توڑو;

    حالت SSH_SESSION_FREE:
      نتیجہ = ssh_state_session_free(ڈیٹا, sshc);
      توڑو;

    حالت SSH_QUIT:
    طےشدہ:
      /* internal error */
      myssh_مقصد(ڈیٹا, sshc, SSH_STOP);
      توڑو;
    }

  } جب(!نتیجہ && (sshc->state != SSH_STOP) && !*بلاک);

  اگر(نتیجہ == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *بلاک = TRUE;
    نتیجہ = CURLE_OK;
  }
  CURL_TRC_SSH(ڈیٹا, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحیح)نتیجہ, *بلاک);

  واپس نتیجہ;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ساکن خالی ssh_block2waitfor(ساخت Curl_easy *ڈیٹا,
                              ساخت ssh_conn *sshc,
                              bool بلاک)
{
  صحیح dir = 0;
  (خالی)ڈیٹا;
  اگر(بلاک) {
    dir = libssh2_session_بلاک_directions(sshc->ssh_session);
    اگر(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_بلاک_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_بلاک_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  اگر(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
ساکن CURLcode ssh_multi_statemach(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ساخت SSHPROTO *sshp = Curl_meta_get(ڈیٹا, CURL_META_SSH_EASY);
  CURLcode نتیجہ = CURLE_OK;
  bool بلاک; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  اگر(!sshc || !sshp)
    واپس CURLE_FAILED_INIT;

  کرو {
    نتیجہ = ssh_statemachine(ڈیٹا, sshc, sshp, &بلاک);
    *مکمل = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } جب(!نتیجہ && !*مکمل && !بلاک);
  ssh_block2waitfor(ڈیٹا, sshc, بلاک);

  واپس نتیجہ;
}

ساکن CURLcode ssh_بلاک_statemach(ساخت Curl_easy *ڈیٹا,
                                    ساخت ssh_conn *sshc,
                                    ساخت SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت curltime آغاز = *Curl_pgrs_now(ڈیٹا);

  جب((sshc->state != SSH_STOP) && !نتیجہ) {
    bool بلاک;
    timediff_t بایاں_ms = 1000;

    نتیجہ = ssh_statemachine(ڈیٹا, sshc, sshp, &بلاک);
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
    ورنہ اگر(curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &آغاز) > 1000) {
      /* disconnect timeout */
      failf(ڈیٹا, "Disconnect timed out");
      نتیجہ = CURLE_OK;
      توڑو;
    }

    اگر(بلاک) {
      صحیح dir = libssh2_session_بلاک_directions(sshc->ssh_session);
      curl_socket_t sock = ڈیٹا->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      اگر(LIBSSH2_SESSION_بلاک_INBOUND & dir)
        fd_read = sock;
      اگر(LIBSSH2_SESSION_بلاک_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (خالی)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
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
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ساکن خالی myssh_conn_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت ssh_conn *sshc = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  sshc_cleanup(sshc, تہی, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ساکن CURLcode ssh_تیاری_connection(ساخت Curl_easy *ڈیٹا,
                                     ساخت connectdata *conn)
{
  ساخت ssh_conn *sshc;
  ساخت SSHPROTO *sshp;
  (خالی)conn;

  sshc = curlx_calloc(1, حجم(*sshc));
  اگر(!sshc)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(Curl_conn_meta_سیٹ(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  sshp = curlx_calloc(1, حجم(*sshp));
  اگر(!sshp)
    واپس CURLE_برآمد_کا_حافظہ;

  curlx_dyn_init(&sshp->readdir, CURL_راستہ_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_راستہ_MAX);
  اگر(Curl_meta_سیٹ(ڈیٹا, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    واپس CURLE_برآمد_کا_حافظہ;

  واپس Curl_ssh_تیاری_pkey(ڈیٹا, sshc);
}

ساکن Curl_recv scp_recv, sftp_recv;
ساکن Curl_send scp_send, sftp_send;

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
ساکن ssize_t ssh_tls_recv(libssh2_socket_t sock, خالی *بفر,
                            size_t لمبائی, صحیح flags, خالی **abstract)
{
  ساخت Curl_easy *ڈیٹا = (ساخت Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(ڈیٹا, sock);
  size_t nread;
  CURLcode نتیجہ;
  ساخت connectdata *conn = ڈیٹا->conn;
  Curl_recv *backup = conn->recv[sockindex];
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (خالی)flags;

  اگر(!sshc)
    واپس -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  نتیجہ = Curl_conn_recv(ڈیٹا, sockindex, بفر, لمبائی, &nread);
  conn->recv[sockindex] = backup;
  اگر(نتیجہ == CURLE_AGAIN)
    واپس -EAGAIN; /* magic return code for libssh2 */
  ورنہ اگر(نتیجہ)
    واپس -1; /* generic error */
  Curl_debug(ڈیٹا, CURLINFO_ڈیٹا_اندر, (ثابت حرف *)بفر, nread);
  واپس (ssize_t)nread;
}

ساکن ssize_t ssh_tls_send(libssh2_socket_t sock, ثابت خالی *بفر,
                            size_t لمبائی, صحیح flags, خالی **abstract)
{
  ساخت Curl_easy *ڈیٹا = (ساخت Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(ڈیٹا, sock);
  size_t nwrite;
  CURLcode نتیجہ;
  ساخت connectdata *conn = ڈیٹا->conn;
  Curl_send *backup = conn->send[sockindex];
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (خالی)flags;

  اگر(!sshc)
    واپس -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  نتیجہ = Curl_conn_send(ڈیٹا, sockindex, بفر, لمبائی, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  اگر(نتیجہ == CURLE_AGAIN)
    واپس -EAGAIN; /* magic return code for libssh2 */
  ورنہ اگر(نتیجہ)
    واپس -1; /* error */
  Curl_debug(ڈیٹا, CURLINFO_ڈیٹا_برآمد, (ثابت حرف *)بفر, nwrite);
  واپس (ssize_t)nwrite;
}
#اگر_ختم

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ساکن CURLcode ssh_connect(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
#اگر_تعریف_شدہ CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#اگر_ختم
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode نتیجہ;

#اگر LIBSSH2_نسخہ_عدد >= 0x010b00
  {
    ثابت حرف *crypto_str;
    انتخاب(libssh2_crypto_engine()) {
    حالت libssh2_gcrypt:
      crypto_str = "libgcrypt";
      توڑو;
    حالت libssh2_mbedtls:
      crypto_str = "mbedTLS";
      توڑو;
    حالت libssh2_openssl:
      crypto_str = "openssl compatible";
      توڑو;
    حالت libssh2_os400qc3:
      crypto_str = "OS400QC3";
      توڑو;
    حالت libssh2_wincng:
      crypto_str = "WinCNG";
      توڑو;
    طےشدہ:
      crypto_str = تہی;
      توڑو;
    }
    اگر(crypto_str)
      infof(ڈیٹا, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#اگر_ختم

  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  infof(ڈیٹا, "SSH: user '%s'", Curl_creds_user(conn->creds));
#اگر_تعریف_شدہ CURL_LIBSSH2_DEBUG
  infof(ڈیٹا, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#اگر_ختم /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, ڈیٹا);

  اگر(!sshc->ssh_session) {
    failf(ڈیٹا, "Failure initializing ssh session");
    واپس CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#اگر LIBSSH2_نسخہ_عدد >= 0x010B00
  اگر(ڈیٹا->سیٹ.server_response_timeout > 0) {
    libssh2_session_سیٹ_read_timeout(sshc->ssh_session,
                             (طویل)(ڈیٹا->سیٹ.server_response_timeout / 1000));
  }
#اگر_ختم

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  اگر(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#اگر LIBSSH2_نسخہ_عدد >= 0x010b01
    infof(ڈیٹا, "SSH: using HTTPS proxy");
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
#ورنہ
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    اتحاد receive {
      خالی *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, خالی *, size_t, صحیح, خالی **);
    };
    اتحاد transfer {
      خالی *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ثابت خالی *, size_t, صحیح, خالی **);
    };
    اتحاد receive sshrecv;
    اتحاد transfer sshsend;

    sshrecv.recvptr = ssh_tls_recv;
    sshsend.sendptr = ssh_tls_send;

    infof(ڈیٹا, "SSH: using HTTPS proxy");
    libssh2_session_callback_سیٹ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_سیٹ(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#اگر_ختم

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#اگر_ختم /* CURL_DISABLE_PROXY */
  اگر(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ورنہ {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  اگر(ڈیٹا->سیٹ.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(ڈیٹا, "SSH: failed to enable compression for session");
  }

  اگر(CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS)) {
    صحیح rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    اگر(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = تہی;
      واپس CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    اگر(rc < 0)
      infof(ڈیٹا, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(ڈیٹا, STRING_SSH_KNOWNHOSTS));
  }

#اگر_تعریف_شدہ CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(ڈیٹا, "SSH: socket %d", (صحیح)sock);
#اگر_ختم

  myssh_مقصد(ڈیٹا, sshc, SSH_INIT);

  نتیجہ = ssh_multi_statemach(ڈیٹا, مکمل);

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
  myssh_مقصد(ڈیٹا, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  نتیجہ = ssh_multi_statemach(ڈیٹا, dophase_مکمل);

  *connected = Curl_conn_is_connected(ڈیٹا->conn, FIRSTSOCKET);

  اگر(*dophase_مکمل) {
    CURL_TRC_SSH(ڈیٹا, "DO phase is complete");
  }

  واپس نتیجہ;
}

/* called from multi.c while DOing */
ساکن CURLcode scp_doing(ساخت Curl_easy *ڈیٹا,
                          bool *dophase_مکمل)
{
  CURLcode نتیجہ;
  نتیجہ = ssh_multi_statemach(ڈیٹا, dophase_مکمل);

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
    نتیجہ = ssh_بلاک_statemach(ڈیٹا, sshc, sshp, TRUE);
  }

  اگر(sshc)
    واپس sshc_cleanup(sshc, ڈیٹا, TRUE);
  واپس نتیجہ;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ساکن CURLcode ssh_مکمل(ساخت Curl_easy *ڈیٹا, CURLcode حالت_2)
{
  ساخت ssh_conn *sshc = Curl_conn_meta_get(ڈیٹا->conn, CURL_META_SSH_CONN);
  ساخت SSHPROTO *sshp = Curl_meta_get(ڈیٹا, CURL_META_SSH_EASY);
  CURLcode نتیجہ = CURLE_OK;

  اگر(!sshc || !sshp)
    واپس CURLE_FAILED_INIT;

  اگر(!حالت_2)
    /* run the state-machine */
    نتیجہ = ssh_بلاک_statemach(ڈیٹا, sshc, sshp, FALSE);
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

  اگر(sshc && !حالت_2)
    myssh_مقصد(ڈیٹا, sshc, SSH_SCP_مکمل);

  واپس ssh_مکمل(ڈیٹا, حالت_2);
}

ساکن CURLcode scp_send(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nwritten;

  (خالی)sockindex; /* we only support SCP on the fixed known primary socket */
  (خالی)eos;
  *pnwritten = 0;

  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ثابت حرف *)mem, len);

  ssh_block2waitfor(ڈیٹا, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  اگر(nwritten == LIBSSH2_ERROR_EAGAIN)
    نتیجہ = CURLE_AGAIN;
  ورنہ اگر(nwritten < LIBSSH2_ERROR_NONE)
    نتیجہ = libssh2_session_error_مقصد_CURLE((صحیح)nwritten);
  ورنہ
    *pnwritten = (size_t)nwritten;

  واپس نتیجہ;
}

ساکن CURLcode scp_recv(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                         حرف *mem, size_t len, size_t *pnread)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nread;

  (خالی)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(ڈیٹا, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  اگر(nread == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;
  ورنہ اگر(nread < LIBSSH2_ERROR_NONE)
    نتیجہ = libssh2_session_error_مقصد_CURLE((صحیح)nread);
  ورنہ
    *pnread = (size_t)nread;

  واپس نتیجہ;
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
  نتیجہ = ssh_multi_statemach(ڈیٹا, dophase_مکمل);

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
  CURLcode نتیجہ = ssh_multi_statemach(ڈیٹا, dophase_مکمل);

  اگر(*dophase_مکمل) {
    CURL_TRC_SSH(ڈیٹا, "DO phase is complete");
  }
  واپس نتیجہ;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساکن CURLcode sftp_disconnect(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn, bool dead_connection)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ساخت SSHPROTO *sshp = Curl_meta_get(ڈیٹا, CURL_META_SSH_EASY);
  (خالی)dead_connection;

  اگر(sshc) {
    اگر(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(ڈیٹا, "DISCONNECT starts now");
      myssh_مقصد(ڈیٹا, sshc, SSH_SFTP_SHUTDOWN);
      نتیجہ = ssh_بلاک_statemach(ڈیٹا, sshc, sshp, TRUE);
      CURL_TRC_SSH(ڈیٹا, "DISCONNECT is done -> %d", (صحیح)نتیجہ);
    }
    sshc_cleanup(sshc, ڈیٹا, TRUE);
  }
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
  واپس ssh_مکمل(ڈیٹا, حالت_2);
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

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ثابت حرف *)mem, len);

  ssh_block2waitfor(ڈیٹا, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  اگر(nwrite == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;
  ورنہ اگر(nwrite < LIBSSH2_ERROR_NONE)
    واپس libssh2_session_error_مقصد_CURLE((صحیح)nwrite);
  *pnwritten = (size_t)nwrite;
  واپس CURLE_OK;
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

  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(ڈیٹا, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  اگر(nread == LIBSSH2_ERROR_EAGAIN)
    واپس CURLE_AGAIN;
  ورنہ اگر(nread < 0)
    واپس libssh2_session_error_مقصد_CURLE((صحیح)nread);

  *pnread = (size_t)nread;
  واپس CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
ساکن CURLcode ssh_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ;
  bool connected = FALSE;
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *مکمل = FALSE; /* default to false */
  اگر(!sshc)
    واپس CURLE_FAILED_INIT;

  ڈیٹا->req.سائز = -1; /* make sure this is unknown at this point */
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
  اگر(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    واپس CURLE_FAILED_INIT;
  }
  واپس CURLE_OK;
}

خالی Curl_ssh_cleanup(خالی)
{
  libssh2_exit();
}

خالی Curl_ssh_نسخہ(حرف *بفر, size_t buflen)
{
  (خالی)curl_msnprintf(بفر, buflen, "libssh2/%s", libssh2_نسخہ(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ساکن خالی ssh_attach(ساخت Curl_easy *ڈیٹا, ساخت connectdata *conn)
{
  DEBUGASSERT(ڈیٹا);
  DEBUGASSERT(conn);
  اگر(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    اگر(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      خالی **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = ڈیٹا;
    }
  }
}

/*
 * SCP protocol handler.
 */
ثابت ساخت Curl_protocol Curl_protocol_scp = {
  ssh_تیاری_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_مکمل,                             /* done */
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
ثابت ساخت Curl_protocol Curl_protocol_sftp = {
  ssh_تیاری_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  sftp_مکمل,                            /* done */
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

#اگر_ختم /* USE_LIBSSH2 */
