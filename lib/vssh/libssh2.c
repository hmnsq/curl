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

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef __VMS
#شامل <in.h>
#شامل <inet.h>
#endif

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

ایستا ثابت نویسه *sftp_libssh2_strerror(unsigned long err)
{
  انتخاب(err) {
  حالت LIBSSH2_FX_NO_SUCH_FILE:
    بازگشت "No such file or directory";

  حالت LIBSSH2_FX_PERMISSION_DENIED:
    بازگشت "Permission denied";

  حالت LIBSSH2_FX_FAILURE:
    بازگشت "Operation failed";

  حالت LIBSSH2_FX_BAD_MESSAGE:
    بازگشت "Bad message from SFTP server";

  حالت LIBSSH2_FX_NO_CONNECTION:
    بازگشت "Not connected to SFTP server";

  حالت LIBSSH2_FX_CONNECTION_LOST:
    بازگشت "Connection to SFTP server lost";

  حالت LIBSSH2_FX_OP_UNSUPPORTED:
    بازگشت "Operation not supported by SFTP server";

  حالت LIBSSH2_FX_INVALID_HANDLE:
    بازگشت "Invalid handle";

  حالت LIBSSH2_FX_NO_SUCH_PATH:
    بازگشت "No such file or directory";

  حالت LIBSSH2_FX_FILE_ALREADY_EXISTS:
    بازگشت "File already exists";

  حالت LIBSSH2_FX_WRITE_PROTECT:
    بازگشت "File is write protected";

  حالت LIBSSH2_FX_NO_MEDIA:
    بازگشت "No media";

  حالت LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    بازگشت "Disk full";

  حالت LIBSSH2_FX_QUOTA_EXCEEDED:
    بازگشت "User quota exceeded";

  حالت LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    بازگشت "Unknown principal";

  حالت LIBSSH2_FX_LOCK_CONFLICT:
    بازگشت "File lock conflict";

  حالت LIBSSH2_FX_DIR_NOT_EMPTY:
    بازگشت "Directory not empty";

  حالت LIBSSH2_FX_NOT_A_DIRECTORY:
    بازگشت "Not a directory";

  حالت LIBSSH2_FX_INVALID_FILENAME:
    بازگشت "Invalid filename";

  حالت LIBSSH2_FX_LINK_LOOP:
    بازگشت "Link points to itself";
  }
  بازگشت "Unknown error in libssh2";
}

ایستا تهی kbd_callback(ثابت نویسه *name, صحیح name_len,
                         ثابت نویسه *instruction, صحیح instruction_len,
                         صحیح رقم_prompts,
                         ثابت LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         تهی **abstract)
{
  ساختار Curl_easy *data = (ساختار Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", رقم_prompts);
#وگرنه
  (تهی)name;
  (تهی)name_len;
  (تهی)instruction;
  (تهی)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  اگر(رقم_prompts == 1) {
    ساختار connectdata *conn = data->conn;
    ثابت نویسه *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (تهی)prompts;
} /* kbd_callback */

ایستا CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  انتخاب(err) {
  حالت LIBSSH2_FX_OK:
    بازگشت CURLE_OK;

  حالت LIBSSH2_FX_NO_SUCH_FILE:
  حالت LIBSSH2_FX_NO_SUCH_PATH:
    بازگشت CURLE_REMOTE_FILE_NOT_FOUND;

  حالت LIBSSH2_FX_PERMISSION_DENIED:
  حالت LIBSSH2_FX_WRITE_PROTECT:
  حالت LIBSSH2_FX_LOCK_CONFLICT:
    بازگشت CURLE_REMOTE_ACCESS_DENIED;

  حالت LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  حالت LIBSSH2_FX_QUOTA_EXCEEDED:
    بازگشت CURLE_REMOTE_DISK_FULL;

  حالت LIBSSH2_FX_FILE_ALREADY_EXISTS:
    بازگشت CURLE_REMOTE_FILE_EXISTS;

  حالت LIBSSH2_FX_DIR_NOT_EMPTY:
    بازگشت CURLE_QUOTE_ERROR;

  پیشفرض:
    بشکن;
  }

  بازگشت CURLE_SSH;
}

ایستا CURLcode libssh2_session_error_to_CURLE(صحیح err)
{
  انتخاب(err) {
  /* Ordered by order of appearance in libssh2.h */
  حالت LIBSSH2_ERROR_NONE:
    بازگشت CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  حالت LIBSSH2_ERROR_SCP_PROTOCOL:
    بازگشت CURLE_REMOTE_FILE_NOT_FOUND;

  حالت LIBSSH2_ERROR_SOCKET_NONE:
    بازگشت CURLE_COULDNT_CONNECT;

  حالت LIBSSH2_ERROR_ALLOC:
    بازگشت CURLE_OUT_OF_MEMORY;

  حالت LIBSSH2_ERROR_SOCKET_SEND:
    بازگشت CURLE_SEND_ERROR;

  حالت LIBSSH2_ERROR_HOSTKEY_INIT:
  حالت LIBSSH2_ERROR_HOSTKEY_SIGN:
  حالت LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  حالت LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    بازگشت CURLE_PEER_FAILED_VERIFICATION;

  حالت LIBSSH2_ERROR_PASSWORD_EXPIRED:
    بازگشت CURLE_LOGIN_DENIED;

  حالت LIBSSH2_ERROR_SOCKET_TIMEOUT:
  حالت LIBSSH2_ERROR_TIMEOUT:
    بازگشت CURLE_OPERATION_TIMEDOUT;

  حالت LIBSSH2_ERROR_EAGAIN:
    بازگشت CURLE_AGAIN;
  }

  بازگشت CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

ایستا LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (تهی)abstract;
  بازگشت Curl_cmalloc(شمار);
}

ایستا LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (تهی)abstract;
  بازگشت Curl_crealloc(ptr, شمار);
}

ایستا LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (تهی)abstract;
  اگر(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

ایستا صحیح sshkeycallback(CURL *easy,
                          ثابت ساختار curl_khkey *knownkey, /* known */
                          ثابت ساختار curl_khkey *foundkey, /* found */
                          شمارش curl_khmatch match,
                          تهی *clientp)
{
  (تهی)easy;
  (تهی)knownkey;
  (تهی)foundkey;
  (تهی)clientp;

  /* we only allow perfect matches, and we reject everything else */
  بازگشت (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ایستا شمارش curl_khtype convert_ssh2_keytype(صحیح sshkeytype)
{
  شمارش curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  انتخاب(sshkeytype) {
  حالت LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    بشکن;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  حالت LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    بشکن;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    بشکن;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    بشکن;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    بشکن;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  حالت LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    بشکن;
#endif
  }
  بازگشت keytype;
}

ایستا CURLcode ssh_knownhost(ساختار Curl_easy *data,
                              ساختار ssh_conn *sshc)
{
  ساختار connectdata *conn = data->conn;
  ساختار libssh2_knownhost *host = پوچ;
  ثابت نویسه *remotekey = پوچ;
  صحیح keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  صحیح keybit = 0;
  صحیح sshkeytype = 0;
  size_t keylen = 0;
  صحیح rc = 0;
  CURLcode result = CURLE_OK;

  اگر(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
    بازگشت CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  اگر(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    ساختار Curl_mapi_guard guard;
    شمارش curl_khmatch keymatch;
    curl_sshkeycallback func =
      data->set.ssh_keyfunc ? data->set.ssh_keyfunc : sshkeycallback;
    ساختار curl_khkey knownkey;
    ساختار curl_khkey *knownkeyp = پوچ;
    ساختار curl_khkey foundkey;

    انتخاب(sshkeytype) {
    حالت LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_کلید_SSHRSA;
      بشکن;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    حالت LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_کلید_SSHDSS;
      بشکن;
#endif
    حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_کلید_ECDSA_256;
      بشکن;
    حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_کلید_ECDSA_384;
      بشکن;
    حالت LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_کلید_ECDSA_521;
      بشکن;
    حالت LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_کلید_ED25519;
      بشکن;
    پیشفرض:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      بشکن;
    }
    اگر(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    وگرنه {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->port != PORT_SSH) ?
                                          conn->origin->port : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(data, "SSH: host check %d, key: %s", keycheck,
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
      keymatch = (شمارش curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  وگرنه {
    /* no remotekey means failure! */
    infof(data, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  انتخاب(rc) {
  پیشفرض: /* unknown return codes is the same as reject */
  حالت CURLKHSTAT_REJECT:
    infof(data, "SSH: knownhost check failed");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  حالت CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    result = CURLE_PEER_FAILED_VERIFICATION;
    بشکن;
  حالت CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    اگر(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  حالت CURLKHSTAT_FINE:
  حالت CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    اگر(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      صحیح addrc;
      ثابت نویسه *hostbuf;
      نویسه *hostport = پوچ;
      اگر(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        اگر(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
      }
      وگرنه
        hostbuf = conn->origin->hostname;
      اگر(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, پوچ,
                                       remotekey, keylen, پوچ, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, پوچ);
        اگر(addrc)
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        وگرنه اگر(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          صحیح wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          اگر(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    وگرنه
      infof(data, "SSH: knownhost entry matches host key");
    بشکن;
  }
  بازگشت result;
}

ایستا CURLcode ssh_check_fingerprint(ساختار Curl_easy *data,
                                      ساختار ssh_conn *sshc)
{
  ثابت نویسه *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_کلید_MD5);
  ثابت نویسه *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_کلید_SHA256);

  اگر(pubkey_sha256) {
    ثابت نویسه *fingerprint = پوچ;
    نویسه *fingerprint_b64 = پوچ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    اگر(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      بازگشت CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    اگر(curlx_base64_encode((ثابت uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      بازگشت CURLE_PEER_FAILED_VERIFICATION;
    }

    اگر(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      بازگشت CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    وقتی((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    وقتی((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    اگر((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      بازگشت CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  اگر(pubkey_md5) {
    نویسه md5buffer[33];
    ثابت نویسه *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    اگر(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      صحیح i;
      برای(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned نویسه)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    اگر(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      اگر(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      وگرنه {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      بازگشت CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  اگر(!pubkey_md5 && !pubkey_sha256) {
    اگر(data->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      صحیح sshkeytype = 0;
      صحیح rc = 0;
      /* we handle the process to the callback */
      ثابت نویسه *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      اگر(remotekey) {
        ساختار Curl_mapi_guard guard;
        شمارش curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (صحیح)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        اگر(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          بازگشت CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      وگرنه {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        بازگشت CURLE_PEER_FAILED_VERIFICATION;
      }
      بازگشت CURLE_OK;
    }
    وگرنه {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      بازگشت ssh_knownhost(data, sshc);
    }
  }
  وگرنه {
    /* as we already matched, we skip the check for known hosts */
    بازگشت CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
ایستا CURLcode ssh_force_knownhost_کلید_type(ساختار Curl_easy *data,
                                             ساختار ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  ایستا ثابت نویسه hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  ایستا ثابت نویسه hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ایستا ثابت نویسه hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ایستا ثابت نویسه hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ایستا ثابت نویسه hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_کلید_SSHDSS
  ایستا ثابت نویسه hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  اگر(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_کلید_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_کلید_SHA256)) {
    ساختار libssh2_knownhost *store = پوچ;
    ساختار connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_کلید_MASK >>
                      LIBSSH2_KNOWNHOST_کلید_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    وقتی(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      اگر(store) {
        اگر(store->name) {
          اگر(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_size = 0;
            ثابت نویسه *p;
            ثابت نویسه *kh_name_end = strstr(store->name, "]:");
            اگر(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              ادامه;
            }
            p = kh_name_end + 2; /* start of port number */
            اگر(!curlx_str_عدد(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = strlen(store->name) - 1 - strlen(kh_name_end);
              اگر((kh_name_size == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                بشکن;
              }
            }
          }
          وگرنه اگر(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            بشکن;
          }
        }
        وگرنه اگر(store->کلید) {
          unsigned صحیح type = (store->typemask & LIBSSH2_KNOWNHOST_کلید_MASK)
                              >> LIBSSH2_KNOWNHOST_کلید_SHIFT;
          اگر(!absent_type[type]) {
            صحیح keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->کلید, strlen(store->کلید),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_کلید_MASK),
              پوچ);
            اگر(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              بشکن;
            }
            وگرنه اگر(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    اگر(found) {
      صحیح rc;
      ثابت نویسه *hostkey_method = پوچ;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      انتخاب(store->typemask & LIBSSH2_KNOWNHOST_کلید_MASK) {
      حالت LIBSSH2_KNOWNHOST_کلید_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        بشکن;
      حالت LIBSSH2_KNOWNHOST_کلید_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        بشکن;
      حالت LIBSSH2_KNOWNHOST_کلید_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        بشکن;
      حالت LIBSSH2_KNOWNHOST_کلید_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        بشکن;
      حالت LIBSSH2_KNOWNHOST_کلید_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        بشکن;
#ifdef LIBSSH2_KNOWNHOST_کلید_SSHDSS
      حالت LIBSSH2_KNOWNHOST_کلید_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        بشکن;
#endif
      حالت LIBSSH2_KNOWNHOST_کلید_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        بازگشت CURLE_SSH;
      پیشفرض:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_کلید_MASK));
        بازگشت CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      اگر(rc) {
        نویسه *err_پیام = پوچ;
        صحیح errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_پیام, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_پیام);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    وگرنه {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  بازگشت result;
}

ایستا CURLcode quote_error(ساختار Curl_easy *data,
                            ساختار ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  بازگشت CURLE_QUOTE_ERROR;
}

ایستا CURLcode sftp_quote(ساختار Curl_easy *data,
                           ساختار ssh_conn *sshc,
                           ساختار SSHPROTO *sshp)
{
  ثابت نویسه *cp;
  CURLcode result = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ثابت نویسه *cmd = sshc->quote_item->data;
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
    نویسه *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    اگر(!tmp)
      بازگشت CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    اگر(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    بازگشت result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  اگر(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    بازگشت CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  اگر(result) {
    اگر(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    بازگشت result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  اگر(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    اگر(result) {
      اگر(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      بازگشت result;
    }
    اگر(*cp)
      بازگشت quote_error(data, sshc);

    memset(&sshp->quote_attrs, 0, اندازه(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    بازگشت result;
  }
  اگر(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    اگر(result) {
      اگر(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      بازگشت result;
    }
    اگر(*cp)
      بازگشت quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    بازگشت result;
  }
  وگرنه اگر(!strncmp(cmd, "mkdir ", 6)) {
    اگر(*cp)
      بازگشت quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    بازگشت result;
  }
  وگرنه اگر(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    اگر(result) {
      اگر(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      بازگشت result;
    }
    اگر(*cp)
      بازگشت quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    بازگشت result;
  }
  وگرنه اگر(!strncmp(cmd, "rmdir ", 6)) {
    اگر(*cp)
      بازگشت quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    بازگشت result;
  }
  وگرنه اگر(!strncmp(cmd, "rm ", 3)) {
    اگر(*cp)
      بازگشت quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    بازگشت result;
  }
  وگرنه اگر(!strncmp(cmd, "statvfs ", 8)) {
    اگر(*cp)
      بازگشت quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    بازگشت result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  بازگشت CURLE_QUOTE_ERROR;
}

ایستا CURLcode sftp_upload_init(ساختار Curl_easy *data,
                                 ساختار ssh_conn *sshc,
                                 ساختار SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  اگر(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    اگر(data->state.resume_from < 0) {
      صحیح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      اگر(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        بازگشت CURLE_OK;
      }
      اگر(rc) {
        data->state.resume_from = 0;
      }
      وگرنه {
        curl_off_t size = attrs.filesize;
        اگر(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          بازگشت CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  اگر(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  وگرنه اگر(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  وگرنه {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  اگر(!sshc->sftp_handle) {
    CURLcode result;
    unsigned long sftperr;
    صحیح rc = libssh2_session_last_errno(sshc->ssh_session);

    اگر(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      بازگشت CURLE_OK;
    }

    اگر(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    وگرنه
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    اگر(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      بازگشت sftp_libssh2_error_to_CURLE(sftperr);
    }
    اگر((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      بازگشت CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    اگر(!result) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      result = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(data, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    بازگشت result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  اگر(data->state.resume_from > 0 && !data->set.remote_append) {
    صحیح seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    اگر(data->set.seek_func) {
      ساختار Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    اگر(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      اگر(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        بازگشت CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      انجامبده {
        ساختار Curl_mapi_guard guard;
        نویسه scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)اندازه(scratch)) ?
          اندازه(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        اگر((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          بازگشت CURLE_FTP_COULDNT_USE_REST;
        }
      } وقتی(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    اگر(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  اگر(data->state.infilesize > 0) {
    data->req.size = data->state.infilesize;
    Curl_pgrsSetUploadSize(data, data->state.infilesize);
  }
  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(data);

  myssh_to(data, sshc, SSH_STOP);
  بازگشت CURLE_OK;
}

ایستا تهی ssh_state_pkey_init(ساختار Curl_easy *data, ساختار ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  اگر((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    اگر(sshc->pub_کلید)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_کلید);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_کلید);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  وگرنه
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
}

ایستا CURLcode sftp_quote_stat(ساختار Curl_easy *data,
                                ساختار ssh_conn *sshc,
                                ساختار SSHPROTO *sshp,
                                bool *blockp)
{
  ثابت نویسه *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  اگر(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  اگر(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    صحیح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    اگر(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      بازگشت CURLE_OK;
    }
    اگر(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      برو fail;
    }
  }

  /* Now set the new attributes... */
  اگر(!strncmp(cmd, "chgrp", 5)) {
    ثابت نویسه *p = sshc->quote_path1;
    curl_off_t gid;
    اگر(!curlx_str_عدد(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وگرنه اگر(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      برو fail;
    }
  }
  وگرنه اگر(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت نویسه *p = sshc->quote_path1;
    /* permissions are octal */
    اگر(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      برو fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  وگرنه اگر(!strncmp(cmd, "chown", 5)) {
    ثابت نویسه *p = sshc->quote_path1;
    curl_off_t uid;
    اگر(!curlx_str_عدد(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وگرنه اگر(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      برو fail;
    }
  }
  وگرنه اگر(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    اگر(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#اگر SIZEOF_TIME_T > SIZEOF_LONG
    اگر(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    اگر(fail)
      برو fail;
    اگر(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    وگرنه /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  بازگشت CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  بازگشت CURLE_QUOTE_ERROR;
}

ایستا CURLcode sftp_download_stat(ساختار Curl_easy *data,
                                   ساختار ssh_conn *sshc,
                                   ساختار SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  صحیح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  اگر(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    بازگشت CURLE_OK;
  }
  اگر(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_SIZE) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    data->req.size = -1;
    data->req.maxdownload = -1;
    Curl_pgrsSetDownloadSize(data, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  وگرنه {
    curl_off_t size = attrs.filesize;

    اگر(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      بازگشت CURLE_BAD_DOWNLOAD_RESUME;
    }
    اگر(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      اگر(result)
        بازگشت result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  اگر(data->state.resume_from) {
    اگر(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      اگر((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        بازگشت CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    وگرنه {
      اگر((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        بازگشت CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = attrs.filesize - data->state.resume_from;
    data->req.maxdownload = attrs.filesize - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, attrs.filesize - data->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }

  /* Setup the actual download */
  اگر(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    بازگشت CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  بازگشت CURLE_OK;
}

ایستا CURLcode sftp_readdir(ساختار Curl_easy *data,
                             ساختار ssh_conn *sshc,
                             ساختار SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode result = CURLE_OK;
  صحیح rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  اگر(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    بازگشت result;
  }
  اگر(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    اگر(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      اگر(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      اگر(result)
        بازگشت result;
    }
    وگرنه {
      result = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      اگر(!result) {
        اگر((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        وگرنه {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      بازگشت result;
    }
  }
  وگرنه اگر(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  وگرنه {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  بازگشت result;
}

ایستا CURLcode ssh_state_init(ساختار Curl_easy *data,
                               ساختار ssh_conn *sshc)
{
  CURLcode result;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  result = ssh_force_knownhost_کلید_type(data, sshc);
  اگر(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  وگرنه
    myssh_to(data, sshc, SSH_S_STARTUP);
  بازگشت result;
}

ایستا CURLcode ssh_state_startup(ساختار Curl_easy *data,
                                  ساختار ssh_conn *sshc)
{
  ساختار connectdata *conn = data->conn;
  صحیح rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc) {
    نویسه *err_پیام = پوچ;
    (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_پیام);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    بازگشت CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_hostkey(ساختار Curl_easy *data,
                                  ساختار ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode result = ssh_check_fingerprint(data, sshc);
  اگر(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  بازگشت result;
}

ایستا CURLcode ssh_state_authlist(ساختار Curl_easy *data,
                                   ساختار ssh_conn *sshc)
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
  ساختار connectdata *conn = data->conn;
  ثابت نویسه *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(strlen(user)));

  اگر(!sshc->authlist) {
    صحیح rc;
    اگر(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      بازگشت CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    بازگشت libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_auth_pkey(ساختار Curl_easy *data,
                                    ساختار ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  ساختار connectdata *conn = data->conn;
  ثابت نویسه *user = Curl_creds_user(conn->creds);
  صحیح rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_کلید, sshc->priv_کلید, sshc->passphrase ? sshc->passphrase : "");
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  وگرنه {
    نویسه *err_پیام = پوچ;
    نویسه unknown[] = "Reason unknown (-1)";
    اگر(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_پیام = unknown;
    }
    وگرنه {
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_پیام);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_auth_pass_init(ساختار Curl_easy *data,
                                         ساختار ssh_conn *sshc)
{
  اگر((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  وگرنه {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_auth_pass(ساختار Curl_easy *data,
                                    ساختار ssh_conn *sshc)
{
  ساختار connectdata *conn = data->conn;
  ثابت نویسه *user = Curl_creds_user(conn->creds);
  ثابت نویسه *passwd = Curl_creds_passwd(conn->creds);
  صحیح rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 پوچ);
  اگر(rc == LIBSSH2_ERROR_EAGAIN) {
    بازگشت CURLE_AGAIN;
  }
  اگر(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  وگرنه {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_auth_host_init(ساختار Curl_easy *data,
                                         ساختار ssh_conn *sshc)
{
  اگر((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  وگرنه {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_auth_agent_init(ساختار Curl_easy *data,
                                          ساختار ssh_conn *sshc)
{
  صحیح rc = 0;
  اگر((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    اگر(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      اگر(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_کلید_INIT);
        بازگشت CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;
    اگر(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_کلید_INIT);
    }
    وگرنه {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  وگرنه
    myssh_to(data, sshc, SSH_AUTH_کلید_INIT);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_auth_agent_list(ساختار Curl_easy *data,
                                          ساختار ssh_conn *sshc)
{
  صحیح rc = libssh2_agent_list_identities(sshc->ssh_agent);

  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;
  اگر(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_کلید_INIT);
  }
  وگرنه {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = پوچ;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_auth_agent(ساختار Curl_easy *data,
                                     ساختار ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  صحیح rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_prev_identity);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    اگر(rc < 0) {
      اگر(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        بازگشت CURLE_OK;
      }
      بازگشت CURLE_AGAIN;
    }
  }

  اگر(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  وگرنه اگر(rc == 1)
    infof(data, "SSH: no agent identity would match");

  اگر(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  وگرنه {
    myssh_to(data, sshc, SSH_AUTH_کلید_INIT);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_auth_کلید_init(ساختار Curl_easy *data,
                                        ساختار ssh_conn *sshc)
{
  اگر((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_کلید);
  }
  وگرنه {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_auth_کلید(ساختار Curl_easy *data,
                                   ساختار ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  ساختار connectdata *conn = data->conn;
  ثابت نویسه *user = Curl_creds_user(conn->creds);
  صحیح rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    بازگشت CURLE_OK;
  }
  بازگشت CURLE_LOGIN_DENIED;
}

ایستا CURLcode ssh_state_auth_done(ساختار Curl_easy *data,
                                    ساختار ssh_conn *sshc)
{
  ساختار connectdata *conn = data->conn;
  اگر(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    بازگشت CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  اگر(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    بازگشت CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_init(ساختار Curl_easy *data,
                                    ساختار ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  اگر(!sshc->sftp_session) {
    نویسه *err_پیام = پوچ;
    اگر(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
    failf(data, "Failure initializing sftp session: %s", err_پیام);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    بازگشت CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_realpath(ساختار Curl_easy *data,
                                        ساختار ssh_conn *sshc,
                                        ساختار SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  صحیح rc;

  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  اگر(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    اگر(!sshc->homedir)
      بازگشت CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    اگر(!data->state.most_recent_ftp_entrypath)
      بازگشت CURLE_OUT_OF_MEMORY;
  }
  وگرنه {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    اگر(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    وگرنه
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (صحیح)result);
    بازگشت result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_quote_init(ساختار Curl_easy *data,
                                          ساختار ssh_conn *sshc,
                                          ساختار SSHPROTO *sshp)
{
  CURLcode result;

  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  اگر(result) {
    myssh_to(data, sshc, SSH_STOP);
    بازگشت result;
  }

  اگر(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  وگرنه {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_postquote_init(ساختار Curl_easy *data,
                                              ساختار ssh_conn *sshc)
{
  اگر(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  وگرنه {
    myssh_to(data, sshc, SSH_STOP);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_quote(ساختار Curl_easy *data,
                                     ساختار ssh_conn *sshc,
                                     ساختار SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  اگر(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  بازگشت result;
}

ایستا CURLcode ssh_state_sftp_next_quote(ساختار Curl_easy *data,
                                          ساختار ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  اگر(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  وگرنه {
    اگر(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    وگرنه {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_quote_stat(ساختار Curl_easy *data,
                                          ساختار ssh_conn *sshc,
                                          ساختار SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  اگر(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  بازگشت result;
}

ایستا CURLcode ssh_state_sftp_quote_setstat(ساختار Curl_easy *data,
                                             ساختار ssh_conn *sshc,
                                             ساختار SSHPROTO *sshp)
{
  صحیح rc;

  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    بازگشت CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_quote_symlink(ساختار Curl_easy *data,
                                             ساختار ssh_conn *sshc)
{
  صحیح rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    بازگشت CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_quote_mkdir(ساختار Curl_easy *data,
                                           ساختار ssh_conn *sshc)
{
  صحیح rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)data->set.new_directory_perms);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    بازگشت CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_quote_rename(ساختار Curl_easy *data,
                                            ساختار ssh_conn *sshc)
{
  صحیح rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    بازگشت CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_quote_rmdir(ساختار Curl_easy *data,
                                           ساختار ssh_conn *sshc)
{
  صحیح rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    بازگشت CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_quote_unlink(ساختار Curl_easy *data,
                                            ساختار ssh_conn *sshc)
{
  صحیح rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    بازگشت CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_quote_statvfs(ساختار Curl_easy *data,
                                             ساختار ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  صحیح rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    بازگشت CURLE_QUOTE_ERROR;
  }
  وگرنه اگر(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#وگرنه
#define CURL_LIBSSH2_VFS_SIZE_MASK "llu"
#endif
    CURLcode result;
    نویسه *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_SIZE_MASK "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_blocks, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    اگر(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      بازگشت CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    اگر(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      بازگشت result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_create_dirs_mkdir(ساختار Curl_easy *data,
                                                 ساختار ssh_conn *sshc,
                                                 ساختار SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  صحیح rc;

  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)data->set.new_directory_perms);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  اگر(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    اگر((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      بازگشت sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_readdir_init(ساختار Curl_easy *data,
                                            ساختار ssh_conn *sshc,
                                            ساختار SSHPROTO *sshp)
{
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  اگر(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    بازگشت CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  اگر(!sshc->sftp_handle) {
    unsigned long sftperr;
    اگر(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    بازگشت sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_readdir_link(ساختار Curl_easy *data,
                                            ساختار ssh_conn *sshc,
                                            ساختار SSHPROTO *sshp)
{
  CURLcode result;
  صحیح rc;
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned صحیح)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  اگر(rc < 0)
    بازگشت CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  اگر(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  وگرنه
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  بازگشت result;
}

ایستا CURLcode ssh_state_scp_download_init(ساختار Curl_easy *data,
                                            ساختار ssh_conn *sshc,
                                            ساختار SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, اندازه(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  اگر(!sshc->ssh_channel) {
    صحیح ssh_err;
    نویسه *err_پیام = پوچ;

    اگر(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
    failf(data, "%s", err_پیام);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    بازگشت libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_close(ساختار Curl_easy *data,
                                     ساختار ssh_conn *sshc,
                                     ساختار SSHPROTO *sshp)
{
  صحیح rc = 0;
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  اگر(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    اگر(rc < 0) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_پیام);
    }
    sshc->sftp_handle = پوچ;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  اگر(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  وگرنه
    myssh_to(data, sshc, SSH_STOP);

  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_shutdown(ساختار Curl_easy *data,
                                        ساختار ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  صحیح rc = 0;
  اگر(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    اگر(rc < 0) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_پیام);
    }
    sshc->sftp_handle = پوچ;
  }
  اگر(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    اگر(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = پوچ;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_download_init(ساختار Curl_easy *data,
                                             ساختار ssh_conn *sshc,
                                             ساختار SSHPROTO *sshp)
{
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  اگر(!sshc->sftp_handle) {
    unsigned long sftperr;
    اگر(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      بازگشت CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    بازگشت sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_scp_upload_init(ساختار Curl_easy *data,
                                          ساختار ssh_conn *sshc,
                                          ساختار SSHPROTO *sshp)
{
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (صحیح)data->set.new_file_perms,
                       (libssh2_int64_t)data->state.infilesize, 0, 0);
  اگر(!sshc->ssh_channel) {
    صحیح ssh_err;
    نویسه *err_پیام = پوچ;
    CURLcode result;
    اگر(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
    failf(data, "%s", err_پیام);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    اگر(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    بازگشت result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_session_disconnect(ساختار Curl_easy *data,
                                             ساختار ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  صحیح rc = 0;
  اگر(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    اگر(rc < 0) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیام);
    }
    sshc->ssh_channel = پوچ;
  }

  اگر(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;

    اگر(rc < 0) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_پیام);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  بازگشت CURLE_OK;
}

ایستا CURLcode sshc_cleanup(ساختار ssh_conn *sshc, ساختار Curl_easy *data,
                             bool block)
{
  صحیح rc;

  اگر(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = پوچ;
  }

  اگر(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    اگر((rc < 0) && data) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_پیام);
    }
    اگر(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      بازگشت CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = پوچ;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = پوچ;
    sshc->sshagent_prev_identity = پوچ;
  }

  اگر(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    اگر((rc < 0) && data) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_پیام);
    }
    اگر(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      بازگشت CURLE_AGAIN;

    sshc->sftp_handle = پوچ;
  }

  اگر(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    اگر((rc < 0) && data) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیام);
    }
    اگر(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      بازگشت CURLE_AGAIN;

    sshc->ssh_channel = پوچ;
  }

  اگر(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    اگر((rc < 0) && data) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_پیام);
    }
    اگر(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      بازگشت CURLE_AGAIN;

    sshc->sftp_session = پوچ;
  }

  اگر(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    اگر((rc < 0) && data) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session, &err_پیام, پوچ, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_پیام);
    }
    اگر(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      بازگشت CURLE_AGAIN;

    sshc->ssh_session = پوچ;
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

  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_getinfo(ساختار Curl_easy *data,
                                       ساختار ssh_conn *sshc)
{
  اگر(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  وگرنه
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_filetime(ساختار Curl_easy *data,
                                        ساختار ssh_conn *sshc,
                                        ساختار SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  صحیح rc;

  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  اگر(rc == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  اگر(rc == 0)
    data->info.filetime = (time_t)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_trans_init(ساختار Curl_easy *data,
                                          ساختار ssh_conn *sshc,
                                          ساختار SSHPROTO *sshp)
{
  اگر(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  وگرنه اگر(sshp) {
    size_t plen = strlen(sshp->path);
    اگر(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    وگرنه
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  وگرنه
    بازگشت CURLE_FAILED_INIT;
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_upload_init(ساختار Curl_easy *data,
                                           ساختار ssh_conn *sshc,
                                           ساختار SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  اگر(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  بازگشت result;
}

ایستا CURLcode ssh_state_sftp_create_dirs_init(ساختار Curl_easy *data,
                                                ساختار ssh_conn *sshc,
                                                ساختار SSHPROTO *sshp)
{
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  اگر(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  وگرنه
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_create_dirs(ساختار Curl_easy *data,
                                           ساختار ssh_conn *sshc,
                                           ساختار SSHPROTO *sshp)
{
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  اگر(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    بازگشت CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_readdir(ساختار Curl_easy *data,
                                       ساختار ssh_conn *sshc,
                                       ساختار SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  اگر(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  بازگشت result;
}

ایستا CURLcode ssh_state_sftp_readdir_bottom(ساختار Curl_easy *data,
                                              ساختار ssh_conn *sshc,
                                              ساختار SSHPROTO *sshp)
{
  CURLcode result;
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  اگر(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  اگر(result) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(data, sshc, SSH_STOP);
  }
  وگرنه {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  بازگشت result;
}

ایستا CURLcode ssh_state_sftp_readdir_done(ساختار Curl_easy *data,
                                            ساختار ssh_conn *sshc)
{
  اگر(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;

  sshc->sftp_handle = پوچ;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_sftp_download_stat(ساختار Curl_easy *data,
                                             ساختار ssh_conn *sshc,
                                             ساختار SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  اگر(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  بازگشت result;
}

ایستا CURLcode ssh_state_scp_trans_init(ساختار Curl_easy *data,
                                         ساختار ssh_conn *sshc,
                                         ساختار SSHPROTO *sshp)
{
  CURLcode result;
  اگر(!sshp)
    بازگشت CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  اگر(result) {
    myssh_to(data, sshc, SSH_STOP);
    بازگشت result;
  }

  اگر(data->state.upload) {
    اگر(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      بازگشت CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  وگرنه
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_scp_done(ساختار Curl_easy *data,
                                   ساختار ssh_conn *sshc)
{
  اگر(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  وگرنه
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_scp_send_eof(ساختار Curl_easy *data,
                                       ساختار ssh_conn *sshc)
{
  اگر(sshc->ssh_channel) {
    صحیح rc = libssh2_channel_send_eof(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;
    اگر(rc) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session,
                                       &err_پیام, پوچ, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_پیام);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_scp_wait_eof(ساختار Curl_easy *data,
                                       ساختار ssh_conn *sshc)
{
  اگر(sshc->ssh_channel) {
    صحیح rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;
    اگر(rc) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session,
                                       &err_پیام, پوچ, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_پیام);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_scp_wait_close(ساختار Curl_easy *data,
                                         ساختار ssh_conn *sshc)
{
  اگر(sshc->ssh_channel) {
    صحیح rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;
    اگر(rc) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session,
                                       &err_پیام, پوچ, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_پیام);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_scp_channel_free(
  ساختار Curl_easy *data,
  ساختار ssh_conn *sshc)
{
  اگر(sshc->ssh_channel) {
    صحیح rc = libssh2_channel_free(sshc->ssh_channel);
    اگر(rc == LIBSSH2_ERROR_EAGAIN)
      بازگشت CURLE_AGAIN;
    اگر(rc < 0) {
      نویسه *err_پیام = پوچ;
      (تهی)libssh2_session_last_error(sshc->ssh_session,
                                       &err_پیام, پوچ, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_پیام);
    }
    sshc->ssh_channel = پوچ;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  بازگشت CURLE_OK;
}

ایستا CURLcode ssh_state_session_free(ساختار Curl_easy *data,
                                       ساختار ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  ساختار connectdata *conn = data->conn;
  اگر(result)
    بازگشت result;
  memset(sshc, 0, اندازه(ساختار ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  بازگشت CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ایستا CURLcode ssh_statemachine(ساختار Curl_easy *data,
                                 ساختار ssh_conn *sshc,
                                 ساختار SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode result = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  انجامبده {
    انتخاب(sshc->state) {
    حالت SSH_INIT:
      result = ssh_state_init(data, sshc);
      اگر(result)
        بشکن;
      FALLTHROUGH();

    حالت SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      اگر(result)
        بشکن;
      FALLTHROUGH();

    حالت SSH_HOSTKEY:
      result = ssh_state_hostkey(data, sshc);
      بشکن;

    حالت SSH_AUTHLIST:
      result = ssh_state_authlist(data, sshc);
      بشکن;

    حالت SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(data, sshc);
      بشکن;

    حالت SSH_AUTH_PKEY:
      result = ssh_state_auth_pkey(data, sshc);
      بشکن;

    حالت SSH_AUTH_PASS_INIT:
      result = ssh_state_auth_pass_init(data, sshc);
      بشکن;

    حالت SSH_AUTH_PASS:
      result = ssh_state_auth_pass(data, sshc);
      بشکن;

    حالت SSH_AUTH_HOST_INIT:
      result = ssh_state_auth_host_init(data, sshc);
      بشکن;

    حالت SSH_AUTH_HOST:
      myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
      بشکن;

    حالت SSH_AUTH_AGENT_INIT:
      result = ssh_state_auth_agent_init(data, sshc);
      بشکن;

    حالت SSH_AUTH_AGENT_LIST:
      result = ssh_state_auth_agent_list(data, sshc);
      بشکن;

    حالت SSH_AUTH_AGENT:
      result = ssh_state_auth_agent(data, sshc);
      بشکن;

    حالت SSH_AUTH_کلید_INIT:
      result = ssh_state_auth_کلید_init(data, sshc);
      بشکن;

    حالت SSH_AUTH_کلید:
      result = ssh_state_auth_کلید(data, sshc);
      بشکن;

    حالت SSH_AUTH_DONE:
      result = ssh_state_auth_done(data, sshc);
      بشکن;

    حالت SSH_SFTP_INIT:
      result = ssh_state_sftp_init(data, sshc);
      بشکن;

    حالت SSH_SFTP_REALPATH:
      result = ssh_state_sftp_realpath(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_QUOTE_INIT:
      result = ssh_state_sftp_quote_init(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_POSTQUOTE_INIT:
      result = ssh_state_sftp_postquote_init(data, sshc);
      بشکن;

    حالت SSH_SFTP_QUOTE:
      result = ssh_state_sftp_quote(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_NEXT_QUOTE:
      result = ssh_state_sftp_next_quote(data, sshc);
      بشکن;

    حالت SSH_SFTP_QUOTE_STAT:
      result = ssh_state_sftp_quote_stat(data, sshc, sshp, block);
      بشکن;

    حالت SSH_SFTP_QUOTE_SETSTAT:
      result = ssh_state_sftp_quote_setstat(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_QUOTE_SYMLINK:
      result = ssh_state_sftp_quote_symlink(data, sshc);
      بشکن;

    حالت SSH_SFTP_QUOTE_MKDIR:
      result = ssh_state_sftp_quote_mkdir(data, sshc);
      بشکن;

    حالت SSH_SFTP_QUOTE_RENAME:
      result = ssh_state_sftp_quote_rename(data, sshc);
      بشکن;

    حالت SSH_SFTP_QUOTE_RMDIR:
      result = ssh_state_sftp_quote_rmdir(data, sshc);
      بشکن;

    حالت SSH_SFTP_QUOTE_UNLINK:
      result = ssh_state_sftp_quote_unlink(data, sshc);
      بشکن;

    حالت SSH_SFTP_QUOTE_STATVFS:
      result = ssh_state_sftp_quote_statvfs(data, sshc);
      بشکن;

    حالت SSH_SFTP_GETINFO:
      result = ssh_state_sftp_getinfo(data, sshc);
      بشکن;

    حالت SSH_SFTP_FILETIME:
      result = ssh_state_sftp_filetime(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_TRANS_INIT:
      result = ssh_state_sftp_trans_init(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_UPLOAD_INIT:
      result = ssh_state_sftp_upload_init(data, sshc, sshp, block);
      بشکن;

    حالت SSH_SFTP_CREATE_DIRS_INIT:
      result = ssh_state_sftp_create_dirs_init(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_CREATE_DIRS:
      result = ssh_state_sftp_create_dirs(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_CREATE_DIRS_MKDIR:
      result = ssh_state_sftp_create_dirs_mkdir(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_READDIR_INIT:
      result = ssh_state_sftp_readdir_init(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_READDIR:
      result = ssh_state_sftp_readdir(data, sshc, sshp, block);
      بشکن;

    حالت SSH_SFTP_READDIR_LINK:
      result = ssh_state_sftp_readdir_link(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_READDIR_BOTTOM:
      result = ssh_state_sftp_readdir_bottom(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_READDIR_DONE:
      result = ssh_state_sftp_readdir_done(data, sshc);
      بشکن;

    حالت SSH_SFTP_DOWNLOAD_INIT:
      result = ssh_state_sftp_download_init(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_DOWNLOAD_STAT:
      result = ssh_state_sftp_download_stat(data, sshc, sshp, block);
      بشکن;

    حالت SSH_SFTP_CLOSE:
      result = ssh_state_sftp_close(data, sshc, sshp);
      بشکن;

    حالت SSH_SFTP_SHUTDOWN:
      result = ssh_state_sftp_shutdown(data, sshc);
      بشکن;

    حالت SSH_SCP_TRANS_INIT:
      result = ssh_state_scp_trans_init(data, sshc, sshp);
      بشکن;

    حالت SSH_SCP_UPLOAD_INIT:
      result = ssh_state_scp_upload_init(data, sshc, sshp);
      بشکن;

    حالت SSH_SCP_DOWNLOAD_INIT:
      result = ssh_state_scp_download_init(data, sshc, sshp);
      بشکن;

    حالت SSH_SCP_DONE:
      result = ssh_state_scp_done(data, sshc);
      بشکن;

    حالت SSH_SCP_SEND_EOF:
      result = ssh_state_scp_send_eof(data, sshc);
      بشکن;

    حالت SSH_SCP_WAIT_EOF:
      result = ssh_state_scp_wait_eof(data, sshc);
      بشکن;

    حالت SSH_SCP_WAIT_CLOSE:
      result = ssh_state_scp_wait_close(data, sshc);
      بشکن;

    حالت SSH_SCP_CHANNEL_FREE:
      result = ssh_state_scp_channel_free(data, sshc);
      بشکن;

    حالت SSH_SESSION_DISCONNECT:
      result = ssh_state_session_disconnect(data, sshc);
      بشکن;

    حالت SSH_SESSION_FREE:
      result = ssh_state_session_free(data, sshc);
      بشکن;

    حالت SSH_QUIT:
    پیشفرض:
      /* internal error */
      myssh_to(data, sshc, SSH_STOP);
      بشکن;
    }

  } وقتی(!result && (sshc->state != SSH_STOP) && !*block);

  اگر(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحیح)result, *block);

  بازگشت result;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ایستا تهی ssh_block2waitfor(ساختار Curl_easy *data,
                              ساختار ssh_conn *sshc,
                              bool block)
{
  صحیح dir = 0;
  (تهی)data;
  اگر(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    اگر(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  اگر(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
ایستا CURLcode ssh_multi_statemach(ساختار Curl_easy *data, bool *done)
{
  ساختار connectdata *conn = data->conn;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ساختار SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  اگر(!sshc || !sshp)
    بازگشت CURLE_FAILED_INIT;

  انجامبده {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } وقتی(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  بازگشت result;
}

ایستا CURLcode ssh_block_statemach(ساختار Curl_easy *data,
                                    ساختار ssh_conn *sshc,
                                    ساختار SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  ساختار curltime start = *Curl_pgrs_now(data);

  وقتی((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t چپ_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    اگر(result)
      بشکن;

    اگر(!disconnect) {
      result = Curl_pgrsCheck(data);
      اگر(result)
        بشکن;

      چپ_ms = Curl_timeleft_ms(data);
      اگر(چپ_ms < 0) {
        failf(data, "Operation timed out");
        بازگشت CURLE_OPERATION_TIMEDOUT;
      }
    }
    وگرنه اگر(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      بشکن;
    }

    اگر(block) {
      صحیح dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      اگر(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      اگر(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (تهی)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!چپ_ms || چپ_ms > 1000) ? 1000 : چپ_ms);
    }
  }

  بازگشت result;
}

ایستا تهی myssh_easy_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار SSHPROTO *sshp = entry;
  (تهی)کلید;
  (تهی)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ایستا تهی myssh_conn_dtor(ثابت تهی *کلید, size_t klen, تهی *entry)
{
  ساختار ssh_conn *sshc = entry;
  (تهی)کلید;
  (تهی)klen;
  sshc_cleanup(sshc, پوچ, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ایستا CURLcode ssh_setup_connection(ساختار Curl_easy *data,
                                     ساختار connectdata *conn)
{
  ساختار ssh_conn *sshc;
  ساختار SSHPROTO *sshp;
  (تهی)conn;

  sshc = curlx_calloc(1, اندازه(*sshc));
  اگر(!sshc)
    بازگشت CURLE_OUT_OF_MEMORY;

  اگر(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, اندازه(*sshp));
  اگر(!sshp)
    بازگشت CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  اگر(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    بازگشت CURLE_OUT_OF_MEMORY;

  بازگشت Curl_ssh_setup_pkey(data, sshc);
}

ایستا Curl_recv scp_recv, sftp_recv;
ایستا Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
ایستا ssize_t ssh_tls_recv(libssh2_socket_t sock, تهی *buffer,
                            size_t length, صحیح flags, تهی **abstract)
{
  ساختار Curl_easy *data = (ساختار Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nread;
  CURLcode result;
  ساختار connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (تهی)flags;

  اگر(!sshc)
    بازگشت -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  اگر(result == CURLE_AGAIN)
    بازگشت -EAGAIN; /* magic return code for libssh2 */
  وگرنه اگر(result)
    بازگشت -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (ثابت نویسه *)buffer, nread);
  بازگشت (ssize_t)nread;
}

ایستا ssize_t ssh_tls_send(libssh2_socket_t sock, ثابت تهی *buffer,
                            size_t length, صحیح flags, تهی **abstract)
{
  ساختار Curl_easy *data = (ساختار Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nwrite;
  CURLcode result;
  ساختار connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (تهی)flags;

  اگر(!sshc)
    بازگشت -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  اگر(result == CURLE_AGAIN)
    بازگشت -EAGAIN; /* magic return code for libssh2 */
  وگرنه اگر(result)
    بازگشت -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (ثابت نویسه *)buffer, nwrite);
  بازگشت (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ایستا CURLcode ssh_connect(ساختار Curl_easy *data, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  ساختار connectdata *conn = data->conn;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result;

#اگر LIBSSH2_VERSION_رقم >= 0x010b00
  {
    ثابت نویسه *crypto_str;
    انتخاب(libssh2_crypto_engine()) {
    حالت libssh2_gcrypt:
      crypto_str = "libgcrypt";
      بشکن;
    حالت libssh2_mbedtls:
      crypto_str = "mbedTLS";
      بشکن;
    حالت libssh2_openssl:
      crypto_str = "openssl compatible";
      بشکن;
    حالت libssh2_os400qc3:
      crypto_str = "OS400QC3";
      بشکن;
    حالت libssh2_wincng:
      crypto_str = "WinCNG";
      بشکن;
    پیشفرض:
      crypto_str = پوچ;
      بشکن;
    }
    اگر(crypto_str)
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  اگر(!sshc)
    بازگشت CURLE_FAILED_INIT;

  infof(data, "SSH: user '%s'", Curl_creds_user(conn->creds));
#ifdef CURL_LIBSSH2_DEBUG
  infof(data, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#endif /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, data);

  اگر(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    بازگشت CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#اگر LIBSSH2_VERSION_رقم >= 0x010B00
  اگر(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  اگر(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#اگر LIBSSH2_VERSION_رقم >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic برداشت
#endif
#وگرنه
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    اجتماع receive {
      تهی *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, تهی *, size_t, صحیح, تهی **);
    };
    اجتماع transfer {
      تهی *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ثابت تهی *, size_t, صحیح, تهی **);
    };
    اجتماع receive sshrecv;
    اجتماع transfer sshsend;

    sshrecv.recvptr = ssh_tls_recv;
    sshsend.sendptr = ssh_tls_send;

    infof(data, "SSH: using HTTPS proxy");
    libssh2_session_callback_set(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_set(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#endif

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#endif /* CURL_DISABLE_PROXY */
  اگر(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  وگرنه {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  اگر(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  اگر(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    صحیح rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    اگر(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = پوچ;
      بازگشت CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    اگر(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (صحیح)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  بازگشت result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ایستا CURLcode scp_perform(ساختار Curl_easy *data,
                            bool *connected,
                            bool *dophase_done)
{
  ساختار ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  اگر(!sshc)
    بازگشت CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  اگر(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  بازگشت result;
}

/* called from multi.c while DOing */
ایستا CURLcode scp_doing(ساختار Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  اگر(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  بازگشت result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ایستا CURLcode scp_disconnect(ساختار Curl_easy *data,
                               ساختار connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ساختار SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (تهی)dead_connection;

  اگر(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  اگر(sshc)
    بازگشت sshc_cleanup(sshc, data, TRUE);
  بازگشت result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ایستا CURLcode ssh_done(ساختار Curl_easy *data, CURLcode status)
{
  ساختار ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  ساختار SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  اگر(!sshc || !sshp)
    بازگشت CURLE_FAILED_INIT;

  اگر(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  وگرنه
    result = status;

  اگر(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    بازگشت CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  بازگشت result;
}

ایستا CURLcode scp_done(ساختار Curl_easy *data, CURLcode status,
                         bool premature)
{
  ساختار ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (تهی)premature;

  اگر(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  بازگشت ssh_done(data, status);
}

ایستا CURLcode scp_send(ساختار Curl_easy *data, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ساختار connectdata *conn = data->conn;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (تهی)sockindex; /* we only support SCP on the fixed known primary socket */
  (تهی)eos;
  *pnwritten = 0;

  اگر(!sshc)
    بازگشت CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ثابت نویسه *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  اگر(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  وگرنه اگر(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((صحیح)nwritten);
  وگرنه
    *pnwritten = (size_t)nwritten;

  بازگشت result;
}

ایستا CURLcode scp_recv(ساختار Curl_easy *data, int8_t sockindex,
                         نویسه *mem, size_t len, size_t *pnread)
{
  ساختار connectdata *conn = data->conn;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (تهی)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  اگر(!sshc)
    بازگشت CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  اگر(nread == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;
  وگرنه اگر(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((صحیح)nread);
  وگرنه
    *pnread = (size_t)nread;

  بازگشت result;
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

ایستا CURLcode sftp_perform(ساختار Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  ساختار ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  اگر(!sshc)
    بازگشت CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  اگر(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  بازگشت result;
}

/* called from multi.c while DOing */
ایستا CURLcode sftp_doing(ساختار Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  اگر(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  بازگشت result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ایستا CURLcode sftp_disconnect(ساختار Curl_easy *data,
                                ساختار connectdata *conn, bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ساختار SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (تهی)dead_connection;

  اگر(sshc) {
    اگر(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (صحیح)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  بازگشت result;
}

ایستا CURLcode sftp_done(ساختار Curl_easy *data, CURLcode status,
                          bool premature)
{
  ساختار connectdata *conn = data->conn;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  اگر(!sshc)
    بازگشت CURLE_FAILED_INIT;

  اگر(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    اگر(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  بازگشت ssh_done(data, status);
}

/* return number of sent bytes */
ایستا CURLcode sftp_send(ساختار Curl_easy *data, int8_t sockindex,
                          ثابت uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  ساختار connectdata *conn = data->conn;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (تهی)sockindex;
  (تهی)eos;
  *pnwritten = 0;

  اگر(!sshc)
    بازگشت CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ثابت نویسه *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  اگر(nwrite == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;
  وگرنه اگر(nwrite < LIBSSH2_ERROR_NONE)
    بازگشت libssh2_session_error_to_CURLE((صحیح)nwrite);
  *pnwritten = (size_t)nwrite;
  بازگشت CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ایستا CURLcode sftp_recv(ساختار Curl_easy *data, int8_t sockindex,
                          نویسه *mem, size_t len, size_t *pnread)
{
  ساختار connectdata *conn = data->conn;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (تهی)sockindex;
  *pnread = 0;

  اگر(!sshc)
    بازگشت CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  اگر(nread == LIBSSH2_ERROR_EAGAIN)
    بازگشت CURLE_AGAIN;
  وگرنه اگر(nread < 0)
    بازگشت libssh2_session_error_to_CURLE((صحیح)nread);

  *pnread = (size_t)nread;
  بازگشت CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
ایستا CURLcode ssh_do(ساختار Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  ساختار connectdata *conn = data->conn;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  اگر(!sshc)
    بازگشت CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  اگر(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  وگرنه
    result = sftp_perform(data, &connected, done);

  بازگشت result;
}

CURLcode Curl_ssh_init(تهی)
{
  اگر(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    بازگشت CURLE_FAILED_INIT;
  }
  بازگشت CURLE_OK;
}

تهی Curl_ssh_cleanup(تهی)
{
  libssh2_exit();
}

تهی Curl_ssh_version(نویسه *buffer, size_t buflen)
{
  (تهی)curl_msnprintf(buffer, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ایستا تهی ssh_attach(ساختار Curl_easy *data, ساختار connectdata *conn)
{
  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  اگر(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    اگر(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      تهی **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = data;
    }
  }
}

/*
 * SCP protocol handler.
 */
ثابت ساختار Curl_protocol Curl_protocol_scp = {
  ssh_setup_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_done,                             /* done */
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
ثابت ساختار Curl_protocol Curl_protocol_sftp = {
  ssh_setup_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  sftp_done,                            /* done */
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

#endif /* USE_LIBSSH2 */
