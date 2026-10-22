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

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef __VMS
#ضمن <in.h>
#ضمن <inet.h>
#endif

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

ساكن ثابت حرف *sftp_libssh2_strerror(unsigned long err)
{
  بدل(err) {
  حالة LIBSSH2_FX_NO_SUCH_FILE:
    أرجع "No such file or directory";

  حالة LIBSSH2_FX_PERMISSION_DENIED:
    أرجع "Permission denied";

  حالة LIBSSH2_FX_FAILURE:
    أرجع "Operation failed";

  حالة LIBSSH2_FX_BAD_MESSAGE:
    أرجع "Bad message from SFTP server";

  حالة LIBSSH2_FX_NO_CONNECTION:
    أرجع "Not connected to SFTP server";

  حالة LIBSSH2_FX_CONNECTION_LOST:
    أرجع "Connection to SFTP server lost";

  حالة LIBSSH2_FX_OP_UNSUPPORTED:
    أرجع "Operation not supported by SFTP server";

  حالة LIBSSH2_FX_INVALID_HANDLE:
    أرجع "Invalid handle";

  حالة LIBSSH2_FX_NO_SUCH_PATH:
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

  حالة LIBSSH2_FX_LOCK_CONFLICT:
    أرجع "File lock conflict";

  حالة LIBSSH2_FX_DIR_NOT_EMPTY:
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
                         صحيح اڠک_prompts,
                         ثابت LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         فراغ **abstract)
{
  بنية Curl_easy *data = (بنية Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", اڠک_prompts);
#وإلا
  (فراغ)name;
  (فراغ)name_len;
  (فراغ)instruction;
  (فراغ)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  إذا(اڠک_prompts == 1) {
    بنية connectdata *conn = data->conn;
    ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (فراغ)prompts;
} /* kbd_callback */

ساكن CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  بدل(err) {
  حالة LIBSSH2_FX_OK:
    أرجع CURLE_OK;

  حالة LIBSSH2_FX_NO_SUCH_FILE:
  حالة LIBSSH2_FX_NO_SUCH_PATH:
    أرجع CURLE_REMOTE_FILE_NOT_FOUND;

  حالة LIBSSH2_FX_PERMISSION_DENIED:
  حالة LIBSSH2_FX_WRITE_PROTECT:
  حالة LIBSSH2_FX_LOCK_CONFLICT:
    أرجع CURLE_REMOTE_ACCESS_DENIED;

  حالة LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  حالة LIBSSH2_FX_QUOTA_EXCEEDED:
    أرجع CURLE_REMOTE_DISK_FULL;

  حالة LIBSSH2_FX_FILE_ALREADY_EXISTS:
    أرجع CURLE_REMOTE_FILE_EXISTS;

  حالة LIBSSH2_FX_DIR_NOT_EMPTY:
    أرجع CURLE_QUOTE_ERROR;

  افتراضي:
    اكسر;
  }

  أرجع CURLE_SSH;
}

ساكن CURLcode libssh2_session_error_to_CURLE(صحيح err)
{
  بدل(err) {
  /* Ordered by order of appearance in libssh2.h */
  حالة LIBSSH2_ERROR_NONE:
    أرجع CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  حالة LIBSSH2_ERROR_SCP_PROTOCOL:
    أرجع CURLE_REMOTE_FILE_NOT_FOUND;

  حالة LIBSSH2_ERROR_SOCKET_NONE:
    أرجع CURLE_COULDNT_CONNECT;

  حالة LIBSSH2_ERROR_ALLOC:
    أرجع CURLE_OUT_OF_MEMORY;

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
  أرجع Curl_cmalloc(کيراءن);
}

ساكن LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (فراغ)abstract;
  أرجع Curl_crealloc(ptr, کيراءن);
}

ساكن LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (فراغ)abstract;
  إذا(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

ساكن صحيح sshkeycallback(CURL *easy,
                          ثابت بنية curl_khkey *knownkey, /* known */
                          ثابت بنية curl_khkey *foundkey, /* found */
                          تعداد curl_khmatch match,
                          فراغ *clientp)
{
  (فراغ)easy;
  (فراغ)knownkey;
  (فراغ)foundkey;
  (فراغ)clientp;

  /* we only allow perfect matches, and we reject everything else */
  أرجع (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ساكن تعداد curl_khtype convert_ssh2_keytype(صحيح sshkeytype)
{
  تعداد curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  بدل(sshkeytype) {
  حالة LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    اكسر;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  حالة LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    اكسر;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    اكسر;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  حالة LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    اكسر;
#endif
  }
  أرجع keytype;
}

ساكن CURLcode ssh_knownhost(بنية Curl_easy *data,
                              بنية ssh_conn *sshc)
{
  بنية connectdata *conn = data->conn;
  بنية libssh2_knownhost *host = عدم;
  ثابت حرف *remotekey = عدم;
  صحيح keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  صحيح keybit = 0;
  صحيح sshkeytype = 0;
  size_t keylen = 0;
  صحيح rc = 0;
  CURLcode result = CURLE_OK;

  إذا(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
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
      data->set.ssh_keyfunc ? data->set.ssh_keyfunc : sshkeycallback;
    بنية curl_khkey knownkey;
    بنية curl_khkey *knownkeyp = عدم;
    بنية curl_khkey foundkey;

    بدل(sshkeytype) {
    حالة LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_کونچي_SSHRSA;
      اكسر;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    حالة LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_کونچي_SSHDSS;
      اكسر;
#endif
    حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_کونچي_ECDSA_256;
      اكسر;
    حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_کونچي_ECDSA_384;
      اكسر;
    حالة LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_کونچي_ECDSA_521;
      اكسر;
    حالة LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_کونچي_ED25519;
      اكسر;
    افتراضي:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      اكسر;
    }
    إذا(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    وإلا {
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
            host->کونچي : "<none>");

      /* setup 'knownkey' */
      إذا(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.کونچي = host->کونچي;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.کونچي = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (تعداد curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  وإلا {
    /* no remotekey means failure! */
    infof(data, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  بدل(rc) {
  افتراضي: /* unknown return codes is the same as reject */
  حالة CURLKHSTAT_REJECT:
    infof(data, "SSH: knownhost check failed");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  حالة CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    result = CURLE_PEER_FAILED_VERIFICATION;
    اكسر;
  حالة CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    إذا(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  حالة CURLKHSTAT_FINE:
  حالة CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    إذا(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      صحيح addrc;
      ثابت حرف *hostbuf;
      حرف *hostport = عدم;
      إذا(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        إذا(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
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
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        وإلا إذا(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          صحيح wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          إذا(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    وإلا
      infof(data, "SSH: knownhost entry matches host key");
    اكسر;
  }
  أرجع result;
}

ساكن CURLcode ssh_check_fingerprint(بنية Curl_easy *data,
                                      بنية ssh_conn *sshc)
{
  ثابت حرف *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_کونچي_MD5);
  ثابت حرف *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_کونچي_SHA256);

  إذا(pubkey_sha256) {
    ثابت حرف *fingerprint = عدم;
    حرف *fingerprint_b64 = عدم;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    إذا(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    إذا(curlx_base64_encode((ثابت uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    إذا(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

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
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  إذا(pubkey_md5) {
    حرف md5buffer[33];
    ثابت حرف *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    إذا(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      صحيح i;
      لكل(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned حرف)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    إذا(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      إذا(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      وإلا {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  إذا(!pubkey_md5 && !pubkey_sha256) {
    إذا(data->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      صحيح sshkeytype = 0;
      صحيح rc = 0;
      /* we handle the process to the callback */
      ثابت حرف *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      إذا(remotekey) {
        بنية Curl_mapi_guard guard;
        تعداد curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (صحيح)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        إذا(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          أرجع CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      وإلا {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        أرجع CURLE_PEER_FAILED_VERIFICATION;
      }
      أرجع CURLE_OK;
    }
    وإلا {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      أرجع ssh_knownhost(data, sshc);
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
ساكن CURLcode ssh_force_knownhost_کونچي_type(بنية Curl_easy *data,
                                             بنية ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  ساكن ثابت حرف hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  ساكن ثابت حرف hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ساكن ثابت حرف hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ساكن ثابت حرف hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ساكن ثابت حرف hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_کونچي_SSHDSS
  ساكن ثابت حرف hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  إذا(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_کونچي_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_کونچي_SHA256)) {
    بنية libssh2_knownhost *store = عدم;
    بنية connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_کونچي_MASK >>
                      LIBSSH2_KNOWNHOST_کونچي_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    طالما(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      إذا(store) {
        إذا(store->name) {
          إذا(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_size = 0;
            ثابت حرف *p;
            ثابت حرف *kh_name_end = strstr(store->name, "]:");
            إذا(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              استمر;
            }
            p = kh_name_end + 2; /* start of port number */
            إذا(!curlx_str_نومبور(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = strlen(store->name) - 1 - strlen(kh_name_end);
              إذا((kh_name_size == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                اكسر;
              }
            }
          }
          وإلا إذا(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            اكسر;
          }
        }
        وإلا إذا(store->کونچي) {
          unsigned صحيح type = (store->typemask & LIBSSH2_KNOWNHOST_کونچي_MASK)
                              >> LIBSSH2_KNOWNHOST_کونچي_SHIFT;
          إذا(!absent_type[type]) {
            صحيح keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->کونچي, strlen(store->کونچي),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_کونچي_MASK),
              عدم);
            إذا(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              اكسر;
            }
            وإلا إذا(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    إذا(found) {
      صحيح rc;
      ثابت حرف *hostkey_method = عدم;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      بدل(store->typemask & LIBSSH2_KNOWNHOST_کونچي_MASK) {
      حالة LIBSSH2_KNOWNHOST_کونچي_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کونچي_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کونچي_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کونچي_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        اكسر;
      حالة LIBSSH2_KNOWNHOST_کونچي_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        اكسر;
#ifdef LIBSSH2_KNOWNHOST_کونچي_SSHDSS
      حالة LIBSSH2_KNOWNHOST_کونچي_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        اكسر;
#endif
      حالة LIBSSH2_KNOWNHOST_کونچي_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        أرجع CURLE_SSH;
      افتراضي:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_کونچي_MASK));
        أرجع CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      إذا(rc) {
        حرف *err_ڤسنن = عدم;
        صحيح errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_ڤسنن);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    وإلا {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  أرجع result;
}

ساكن CURLcode quote_error(بنية Curl_easy *data,
                            بنية ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_quote(بنية Curl_easy *data,
                           بنية ssh_conn *sshc,
                           بنية SSHPROTO *sshp)
{
  ثابت حرف *cp;
  CURLcode result = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ثابت حرف *cmd = sshc->quote_item->data;
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
                              sshp->path);
    إذا(!tmp)
      أرجع CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    إذا(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    أرجع result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  إذا(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    أرجع CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  إذا(result) {
    إذا(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    أرجع result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  إذا(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(result) {
      إذا(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      أرجع result;
    }
    إذا(*cp)
      أرجع quote_error(data, sshc);

    memset(&sshp->quote_attrs, 0, حجم(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    أرجع result;
  }
  إذا(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(result) {
      إذا(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      أرجع result;
    }
    إذا(*cp)
      أرجع quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    أرجع result;
  }
  وإلا إذا(!strncmp(cmd, "mkdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    أرجع result;
  }
  وإلا إذا(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    إذا(result) {
      إذا(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      أرجع result;
    }
    إذا(*cp)
      أرجع quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    أرجع result;
  }
  وإلا إذا(!strncmp(cmd, "rmdir ", 6)) {
    إذا(*cp)
      أرجع quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    أرجع result;
  }
  وإلا إذا(!strncmp(cmd, "rm ", 3)) {
    إذا(*cp)
      أرجع quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    أرجع result;
  }
  وإلا إذا(!strncmp(cmd, "statvfs ", 8)) {
    إذا(*cp)
      أرجع quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    أرجع result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_upload_init(بنية Curl_easy *data,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  إذا(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    إذا(data->state.resume_from < 0) {
      صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      إذا(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        أرجع CURLE_OK;
      }
      إذا(rc) {
        data->state.resume_from = 0;
      }
      وإلا {
        curl_off_t size = attrs.filesize;
        إذا(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          أرجع CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  إذا(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  وإلا إذا(data->state.resume_from > 0) {
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
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  إذا(!sshc->sftp_handle) {
    CURLcode result;
    unsigned long sftperr;
    صحيح rc = libssh2_session_last_errno(sshc->ssh_session);

    إذا(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      أرجع CURLE_OK;
    }

    إذا(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    وإلا
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    إذا(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      أرجع sftp_libssh2_error_to_CURLE(sftperr);
    }
    إذا((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      أرجع CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    إذا(!result) {
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
    أرجع result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  إذا(data->state.resume_from > 0 && !data->set.remote_append) {
    صحيح seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    إذا(data->set.seek_func) {
      بنية Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    إذا(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      إذا(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        أرجع CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      افعل {
        بنية Curl_mapi_guard guard;
        حرف scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)حجم(scratch)) ?
          حجم(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        إذا((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          أرجع CURLE_FTP_COULDNT_USE_REST;
        }
      } طالما(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    إذا(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  إذا(data->state.infilesize > 0) {
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
  أرجع CURLE_OK;
}

ساكن فراغ ssh_state_pkey_init(بنية Curl_easy *data, بنية ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  إذا((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    إذا(sshc->pub_کونچي)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_کونچي);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_کونچي);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  وإلا
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
}

ساكن CURLcode sftp_quote_stat(بنية Curl_easy *data,
                                بنية ssh_conn *sshc,
                                بنية SSHPROTO *sshp,
                                bool *blockp)
{
  ثابت حرف *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  إذا(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  إذا(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    إذا(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      أرجع CURLE_OK;
    }
    إذا(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      اذهب fail;
    }
  }

  /* Now set the new attributes... */
  إذا(!strncmp(cmd, "chgrp", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t gid;
    إذا(!curlx_str_نومبور(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وإلا إذا(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      اذهب fail;
    }
  }
  وإلا إذا(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ثابت حرف *p = sshc->quote_path1;
    /* permissions are octal */
    إذا(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      اذهب fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  وإلا إذا(!strncmp(cmd, "chown", 5)) {
    ثابت حرف *p = sshc->quote_path1;
    curl_off_t uid;
    إذا(!curlx_str_نومبور(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    وإلا إذا(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      اذهب fail;
    }
  }
  وإلا إذا(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    إذا(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#إذا SIZEOF_TIME_T > SIZEOF_LONG
    إذا(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    إذا(fail)
      اذهب fail;
    إذا(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    وإلا /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  أرجع CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  أرجع CURLE_QUOTE_ERROR;
}

ساكن CURLcode sftp_download_stat(بنية Curl_easy *data,
                                   بنية ssh_conn *sshc,
                                   بنية SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  صحيح rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    أرجع CURLE_OK;
  }
  إذا(rc ||
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
  وإلا {
    curl_off_t size = attrs.filesize;

    إذا(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      أرجع CURLE_BAD_DOWNLOAD_RESUME;
    }
    إذا(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      إذا(result)
        أرجع result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  إذا(data->state.resume_from) {
    إذا(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      إذا((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    وإلا {
      إذا((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        أرجع CURLE_BAD_DOWNLOAD_RESUME;
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
  إذا(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    أرجع CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode sftp_readdir(بنية Curl_easy *data,
                             بنية ssh_conn *sshc,
                             بنية SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode result = CURLE_OK;
  صحيح rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    أرجع result;
  }
  إذا(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    إذا(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      إذا(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      إذا(result)
        أرجع result;
    }
    وإلا {
      result = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      إذا(!result) {
        إذا((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        وإلا {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      أرجع result;
    }
  }
  وإلا إذا(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  وإلا {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  أرجع result;
}

ساكن CURLcode ssh_state_init(بنية Curl_easy *data,
                               بنية ssh_conn *sshc)
{
  CURLcode result;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  result = ssh_force_knownhost_کونچي_type(data, sshc);
  إذا(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  وإلا
    myssh_to(data, sshc, SSH_S_STARTUP);
  أرجع result;
}

ساكن CURLcode ssh_state_startup(بنية Curl_easy *data,
                                  بنية ssh_conn *sshc)
{
  بنية connectdata *conn = data->conn;
  صحيح rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc) {
    حرف *err_ڤسنن = عدم;
    (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_ڤسنن);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    أرجع CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_hostkey(بنية Curl_easy *data,
                                  بنية ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode result = ssh_check_fingerprint(data, sshc);
  إذا(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  أرجع result;
}

ساكن CURLcode ssh_state_authlist(بنية Curl_easy *data,
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
  بنية connectdata *conn = data->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(strlen(user)));

  إذا(!sshc->authlist) {
    صحيح rc;
    إذا(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      أرجع CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    أرجع libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pkey(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  بنية connectdata *conn = data->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحيح rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_کونچي, sshc->priv_کونچي, sshc->passphrase ? sshc->passphrase : "");
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  وإلا {
    حرف *err_ڤسنن = عدم;
    حرف unknown[] = "Reason unknown (-1)";
    إذا(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_ڤسنن = unknown;
    }
    وإلا {
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_ڤسنن);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pass_init(بنية Curl_easy *data,
                                         بنية ssh_conn *sshc)
{
  إذا((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  وإلا {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_pass(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc)
{
  بنية connectdata *conn = data->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  ثابت حرف *passwd = Curl_creds_passwd(conn->creds);
  صحيح rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 عدم);
  إذا(rc == LIBSSH2_ERROR_EAGAIN) {
    أرجع CURLE_AGAIN;
  }
  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  وإلا {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_host_init(بنية Curl_easy *data,
                                         بنية ssh_conn *sshc)
{
  إذا((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  وإلا {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent_init(بنية Curl_easy *data,
                                          بنية ssh_conn *sshc)
{
  صحيح rc = 0;
  إذا((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    إذا(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      إذا(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_کونچي_INIT);
        أرجع CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_کونچي_INIT);
    }
    وإلا {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  وإلا
    myssh_to(data, sshc, SSH_AUTH_کونچي_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent_list(بنية Curl_easy *data,
                                          بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_agent_list_identities(sshc->ssh_agent);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  إذا(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_کونچي_INIT);
  }
  وإلا {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = عدم;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_agent(بنية Curl_easy *data,
                                     بنية ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  صحيح rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_prev_identity);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    إذا(rc < 0) {
      إذا(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        أرجع CURLE_OK;
      }
      أرجع CURLE_AGAIN;
    }
  }

  إذا(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  وإلا إذا(rc == 1)
    infof(data, "SSH: no agent identity would match");

  إذا(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  وإلا {
    myssh_to(data, sshc, SSH_AUTH_کونچي_INIT);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_کونچي_init(بنية Curl_easy *data,
                                        بنية ssh_conn *sshc)
{
  إذا((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_کونچي);
  }
  وإلا {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_auth_کونچي(بنية Curl_easy *data,
                                   بنية ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  بنية connectdata *conn = data->conn;
  ثابت حرف *user = Curl_creds_user(conn->creds);
  صحيح rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    أرجع CURLE_OK;
  }
  أرجع CURLE_LOGIN_DENIED;
}

ساكن CURLcode ssh_state_auth_done(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc)
{
  بنية connectdata *conn = data->conn;
  إذا(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    أرجع CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  إذا(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    أرجع CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_init(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  إذا(!sshc->sftp_session) {
    حرف *err_ڤسنن = عدم;
    إذا(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
    failf(data, "Failure initializing sftp session: %s", err_ڤسنن);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    أرجع CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_realpath(بنية Curl_easy *data,
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
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  إذا(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    إذا(!sshc->homedir)
      أرجع CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    إذا(!data->state.most_recent_ftp_entrypath)
      أرجع CURLE_OUT_OF_MEMORY;
  }
  وإلا {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    إذا(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    وإلا
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (صحيح)result);
    أرجع result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_init(بنية Curl_easy *data,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  CURLcode result;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  إذا(result) {
    myssh_to(data, sshc, SSH_STOP);
    أرجع result;
  }

  إذا(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_postquote_init(بنية Curl_easy *data,
                                              بنية ssh_conn *sshc)
{
  إذا(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    myssh_to(data, sshc, SSH_STOP);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote(بنية Curl_easy *data,
                                     بنية ssh_conn *sshc,
                                     بنية SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  إذا(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع result;
}

ساكن CURLcode ssh_state_sftp_next_quote(بنية Curl_easy *data,
                                          بنية ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  إذا(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  وإلا {
    إذا(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    وإلا {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_stat(بنية Curl_easy *data,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  إذا(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع result;
}

ساكن CURLcode ssh_state_sftp_quote_setstat(بنية Curl_easy *data,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp)
{
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_symlink(بنية Curl_easy *data,
                                             بنية ssh_conn *sshc)
{
  صحيح rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_mkdir(بنية Curl_easy *data,
                                           بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)data->set.new_directory_perms);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_rename(بنية Curl_easy *data,
                                            بنية ssh_conn *sshc)
{
  صحيح rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_rmdir(بنية Curl_easy *data,
                                           بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_unlink(بنية Curl_easy *data,
                                            بنية ssh_conn *sshc)
{
  صحيح rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_quote_statvfs(بنية Curl_easy *data,
                                             بنية ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  صحيح rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    أرجع CURLE_QUOTE_ERROR;
  }
  وإلا إذا(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#وإلا
#define CURL_LIBSSH2_VFS_SIZE_MASK "llu"
#endif
    CURLcode result;
    حرف *tmp = curl_maprintf("statvfs:\n"
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
    إذا(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    إذا(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      أرجع result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_create_dirs_mkdir(بنية Curl_easy *data,
                                                 بنية ssh_conn *sshc,
                                                 بنية SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)data->set.new_directory_perms);
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
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    إذا((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      أرجع sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir_init(بنية Curl_easy *data,
                                            بنية ssh_conn *sshc,
                                            بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  إذا(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    أرجع CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  إذا(!sshc->sftp_handle) {
    unsigned long sftperr;
    إذا(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    أرجع sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir_link(بنية Curl_easy *data,
                                            بنية ssh_conn *sshc,
                                            بنية SSHPROTO *sshp)
{
  CURLcode result;
  صحيح rc;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned صحيح)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  إذا(rc < 0)
    أرجع CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  إذا(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  وإلا
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  أرجع result;
}

ساكن CURLcode ssh_state_scp_download_init(بنية Curl_easy *data,
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
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  إذا(!sshc->ssh_channel) {
    صحيح ssh_err;
    حرف *err_ڤسنن = عدم;

    إذا(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
    failf(data, "%s", err_ڤسنن);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    أرجع libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_close(بنية Curl_easy *data,
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
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_ڤسنن);
    }
    sshc->sftp_handle = عدم;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  إذا(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  وإلا
    myssh_to(data, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_shutdown(بنية Curl_easy *data,
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
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_ڤسنن);
    }
    sshc->sftp_handle = عدم;
  }
  إذا(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = عدم;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_download_init(بنية Curl_easy *data,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  إذا(!sshc->sftp_handle) {
    unsigned long sftperr;
    إذا(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      أرجع CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    أرجع sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_upload_init(بنية Curl_easy *data,
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
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (صحيح)data->set.new_file_perms,
                       (libssh2_int64_t)data->state.infilesize, 0, 0);
  إذا(!sshc->ssh_channel) {
    صحيح ssh_err;
    حرف *err_ڤسنن = عدم;
    CURLcode result;
    إذا(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
    failf(data, "%s", err_ڤسنن);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    إذا(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    أرجع result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_session_disconnect(بنية Curl_easy *data,
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
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ڤسنن);
    }
    sshc->ssh_channel = عدم;
  }

  إذا(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;

    إذا(rc < 0) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_ڤسنن);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode sshc_cleanup(بنية ssh_conn *sshc, بنية Curl_easy *data,
                             bool block)
{
  صحيح rc;

  إذا(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = عدم;
  }

  إذا(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    إذا((rc < 0) && data) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_ڤسنن);
    }
    إذا(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = عدم;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = عدم;
    sshc->sshagent_prev_identity = عدم;
  }

  إذا(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    إذا((rc < 0) && data) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_ڤسنن);
    }
    إذا(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->sftp_handle = عدم;
  }

  إذا(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    إذا((rc < 0) && data) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ڤسنن);
    }
    إذا(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->ssh_channel = عدم;
  }

  إذا(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    إذا((rc < 0) && data) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_ڤسنن);
    }
    إذا(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      أرجع CURLE_AGAIN;

    sshc->sftp_session = عدم;
  }

  إذا(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    إذا((rc < 0) && data) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session, &err_ڤسنن, عدم, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_ڤسنن);
    }
    إذا(!block && (rc == LIBSSH2_ERROR_EAGAIN))
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
  curlx_safefree(sshc->pub_کونچي);
  curlx_safefree(sshc->priv_کونچي);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_getinfo(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc)
{
  إذا(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  وإلا
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_filetime(بنية Curl_easy *data,
                                        بنية ssh_conn *sshc,
                                        بنية SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  صحيح rc;

  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  إذا(rc == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  إذا(rc == 0)
    data->info.filetime = (time_t)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_trans_init(بنية Curl_easy *data,
                                          بنية ssh_conn *sshc,
                                          بنية SSHPROTO *sshp)
{
  إذا(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  وإلا إذا(sshp) {
    size_t plen = strlen(sshp->path);
    إذا(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    وإلا
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  وإلا
    أرجع CURLE_FAILED_INIT;
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_upload_init(بنية Curl_easy *data,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  إذا(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع result;
}

ساكن CURLcode ssh_state_sftp_create_dirs_init(بنية Curl_easy *data,
                                                بنية ssh_conn *sshc,
                                                بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  إذا(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  وإلا
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_create_dirs(بنية Curl_easy *data,
                                           بنية ssh_conn *sshc,
                                           بنية SSHPROTO *sshp)
{
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  إذا(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    أرجع CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_readdir(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc,
                                       بنية SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  إذا(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  أرجع result;
}

ساكن CURLcode ssh_state_sftp_readdir_bottom(بنية Curl_easy *data,
                                              بنية ssh_conn *sshc,
                                              بنية SSHPROTO *sshp)
{
  CURLcode result;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  إذا(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  إذا(result) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(data, sshc, SSH_STOP);
  }
  وإلا {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  أرجع result;
}

ساكن CURLcode ssh_state_sftp_readdir_done(بنية Curl_easy *data,
                                            بنية ssh_conn *sshc)
{
  إذا(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;

  sshc->sftp_handle = عدم;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_sftp_download_stat(بنية Curl_easy *data,
                                             بنية ssh_conn *sshc,
                                             بنية SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  إذا(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  أرجع result;
}

ساكن CURLcode ssh_state_scp_trans_init(بنية Curl_easy *data,
                                         بنية ssh_conn *sshc,
                                         بنية SSHPROTO *sshp)
{
  CURLcode result;
  إذا(!sshp)
    أرجع CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  إذا(result) {
    myssh_to(data, sshc, SSH_STOP);
    أرجع result;
  }

  إذا(data->state.upload) {
    إذا(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      أرجع CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  وإلا
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_done(بنية Curl_easy *data,
                                   بنية ssh_conn *sshc)
{
  إذا(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  وإلا
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_send_eof(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_send_eof(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ڤسنن, عدم, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_ڤسنن);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_wait_eof(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ڤسنن, عدم, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_ڤسنن);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_wait_close(بنية Curl_easy *data,
                                         بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ڤسنن, عدم, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_ڤسنن);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_scp_channel_free(
  بنية Curl_easy *data,
  بنية ssh_conn *sshc)
{
  إذا(sshc->ssh_channel) {
    صحيح rc = libssh2_channel_free(sshc->ssh_channel);
    إذا(rc == LIBSSH2_ERROR_EAGAIN)
      أرجع CURLE_AGAIN;
    إذا(rc < 0) {
      حرف *err_ڤسنن = عدم;
      (فراغ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ڤسنن, عدم, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ڤسنن);
    }
    sshc->ssh_channel = عدم;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

ساكن CURLcode ssh_state_session_free(بنية Curl_easy *data,
                                       بنية ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  بنية connectdata *conn = data->conn;
  إذا(result)
    أرجع result;
  memset(sshc, 0, حجم(بنية ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  أرجع CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ساكن CURLcode ssh_statemachine(بنية Curl_easy *data,
                                 بنية ssh_conn *sshc,
                                 بنية SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode result = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  افعل {
    بدل(sshc->state) {
    حالة SSH_INIT:
      result = ssh_state_init(data, sshc);
      إذا(result)
        اكسر;
      FALLTHROUGH();

    حالة SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      إذا(result)
        اكسر;
      FALLTHROUGH();

    حالة SSH_HOSTKEY:
      result = ssh_state_hostkey(data, sshc);
      اكسر;

    حالة SSH_AUTHLIST:
      result = ssh_state_authlist(data, sshc);
      اكسر;

    حالة SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(data, sshc);
      اكسر;

    حالة SSH_AUTH_PKEY:
      result = ssh_state_auth_pkey(data, sshc);
      اكسر;

    حالة SSH_AUTH_PASS_INIT:
      result = ssh_state_auth_pass_init(data, sshc);
      اكسر;

    حالة SSH_AUTH_PASS:
      result = ssh_state_auth_pass(data, sshc);
      اكسر;

    حالة SSH_AUTH_HOST_INIT:
      result = ssh_state_auth_host_init(data, sshc);
      اكسر;

    حالة SSH_AUTH_HOST:
      myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
      اكسر;

    حالة SSH_AUTH_AGENT_INIT:
      result = ssh_state_auth_agent_init(data, sshc);
      اكسر;

    حالة SSH_AUTH_AGENT_LIST:
      result = ssh_state_auth_agent_list(data, sshc);
      اكسر;

    حالة SSH_AUTH_AGENT:
      result = ssh_state_auth_agent(data, sshc);
      اكسر;

    حالة SSH_AUTH_کونچي_INIT:
      result = ssh_state_auth_کونچي_init(data, sshc);
      اكسر;

    حالة SSH_AUTH_کونچي:
      result = ssh_state_auth_کونچي(data, sshc);
      اكسر;

    حالة SSH_AUTH_DONE:
      result = ssh_state_auth_done(data, sshc);
      اكسر;

    حالة SSH_SFTP_INIT:
      result = ssh_state_sftp_init(data, sshc);
      اكسر;

    حالة SSH_SFTP_REALPATH:
      result = ssh_state_sftp_realpath(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_QUOTE_INIT:
      result = ssh_state_sftp_quote_init(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_POSTQUOTE_INIT:
      result = ssh_state_sftp_postquote_init(data, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE:
      result = ssh_state_sftp_quote(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_NEXT_QUOTE:
      result = ssh_state_sftp_next_quote(data, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_STAT:
      result = ssh_state_sftp_quote_stat(data, sshc, sshp, block);
      اكسر;

    حالة SSH_SFTP_QUOTE_SETSTAT:
      result = ssh_state_sftp_quote_setstat(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_QUOTE_SYMLINK:
      result = ssh_state_sftp_quote_symlink(data, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_MKDIR:
      result = ssh_state_sftp_quote_mkdir(data, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_RENAME:
      result = ssh_state_sftp_quote_rename(data, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_RMDIR:
      result = ssh_state_sftp_quote_rmdir(data, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_UNLINK:
      result = ssh_state_sftp_quote_unlink(data, sshc);
      اكسر;

    حالة SSH_SFTP_QUOTE_STATVFS:
      result = ssh_state_sftp_quote_statvfs(data, sshc);
      اكسر;

    حالة SSH_SFTP_GETINFO:
      result = ssh_state_sftp_getinfo(data, sshc);
      اكسر;

    حالة SSH_SFTP_FILETIME:
      result = ssh_state_sftp_filetime(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_TRANS_INIT:
      result = ssh_state_sftp_trans_init(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_UPLOAD_INIT:
      result = ssh_state_sftp_upload_init(data, sshc, sshp, block);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS_INIT:
      result = ssh_state_sftp_create_dirs_init(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS:
      result = ssh_state_sftp_create_dirs(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_CREATE_DIRS_MKDIR:
      result = ssh_state_sftp_create_dirs_mkdir(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_INIT:
      result = ssh_state_sftp_readdir_init(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR:
      result = ssh_state_sftp_readdir(data, sshc, sshp, block);
      اكسر;

    حالة SSH_SFTP_READDIR_LINK:
      result = ssh_state_sftp_readdir_link(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_BOTTOM:
      result = ssh_state_sftp_readdir_bottom(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_READDIR_DONE:
      result = ssh_state_sftp_readdir_done(data, sshc);
      اكسر;

    حالة SSH_SFTP_DOWNLOAD_INIT:
      result = ssh_state_sftp_download_init(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_DOWNLOAD_STAT:
      result = ssh_state_sftp_download_stat(data, sshc, sshp, block);
      اكسر;

    حالة SSH_SFTP_CLOSE:
      result = ssh_state_sftp_close(data, sshc, sshp);
      اكسر;

    حالة SSH_SFTP_SHUTDOWN:
      result = ssh_state_sftp_shutdown(data, sshc);
      اكسر;

    حالة SSH_SCP_TRANS_INIT:
      result = ssh_state_scp_trans_init(data, sshc, sshp);
      اكسر;

    حالة SSH_SCP_UPLOAD_INIT:
      result = ssh_state_scp_upload_init(data, sshc, sshp);
      اكسر;

    حالة SSH_SCP_DOWNLOAD_INIT:
      result = ssh_state_scp_download_init(data, sshc, sshp);
      اكسر;

    حالة SSH_SCP_DONE:
      result = ssh_state_scp_done(data, sshc);
      اكسر;

    حالة SSH_SCP_SEND_EOF:
      result = ssh_state_scp_send_eof(data, sshc);
      اكسر;

    حالة SSH_SCP_WAIT_EOF:
      result = ssh_state_scp_wait_eof(data, sshc);
      اكسر;

    حالة SSH_SCP_WAIT_CLOSE:
      result = ssh_state_scp_wait_close(data, sshc);
      اكسر;

    حالة SSH_SCP_CHANNEL_FREE:
      result = ssh_state_scp_channel_free(data, sshc);
      اكسر;

    حالة SSH_SESSION_DISCONNECT:
      result = ssh_state_session_disconnect(data, sshc);
      اكسر;

    حالة SSH_SESSION_FREE:
      result = ssh_state_session_free(data, sshc);
      اكسر;

    حالة SSH_QUIT:
    افتراضي:
      /* internal error */
      myssh_to(data, sshc, SSH_STOP);
      اكسر;
    }

  } طالما(!result && (sshc->state != SSH_STOP) && !*block);

  إذا(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (صحيح)result, *block);

  أرجع result;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ساكن فراغ ssh_block2waitfor(بنية Curl_easy *data,
                              بنية ssh_conn *sshc,
                              bool block)
{
  صحيح dir = 0;
  (فراغ)data;
  إذا(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    إذا(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  إذا(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
ساكن CURLcode ssh_multi_statemach(بنية Curl_easy *data, bool *done)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;

  افعل {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } طالما(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  أرجع result;
}

ساكن CURLcode ssh_block_statemach(بنية Curl_easy *data,
                                    بنية ssh_conn *sshc,
                                    بنية SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  بنية curltime start = *Curl_pgrs_now(data);

  طالما((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t کيري_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    إذا(result)
      اكسر;

    إذا(!disconnect) {
      result = Curl_pgrsCheck(data);
      إذا(result)
        اكسر;

      کيري_ms = Curl_timeleft_ms(data);
      إذا(کيري_ms < 0) {
        failf(data, "Operation timed out");
        أرجع CURLE_OPERATION_TIMEDOUT;
      }
    }
    وإلا إذا(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      اكسر;
    }

    إذا(block) {
      صحيح dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      إذا(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      إذا(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (فراغ)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!کيري_ms || کيري_ms > 1000) ? 1000 : کيري_ms);
    }
  }

  أرجع result;
}

ساكن فراغ myssh_easy_dtor(ثابت فراغ *کونچي, size_t klen, فراغ *entry)
{
  بنية SSHPROTO *sshp = entry;
  (فراغ)کونچي;
  (فراغ)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ساكن فراغ myssh_conn_dtor(ثابت فراغ *کونچي, size_t klen, فراغ *entry)
{
  بنية ssh_conn *sshc = entry;
  (فراغ)کونچي;
  (فراغ)klen;
  sshc_cleanup(sshc, عدم, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ساكن CURLcode ssh_setup_connection(بنية Curl_easy *data,
                                     بنية connectdata *conn)
{
  بنية ssh_conn *sshc;
  بنية SSHPROTO *sshp;
  (فراغ)conn;

  sshc = curlx_calloc(1, حجم(*sshc));
  إذا(!sshc)
    أرجع CURLE_OUT_OF_MEMORY;

  إذا(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    أرجع CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, حجم(*sshp));
  إذا(!sshp)
    أرجع CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  إذا(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    أرجع CURLE_OUT_OF_MEMORY;

  أرجع Curl_ssh_setup_pkey(data, sshc);
}

ساكن Curl_recv scp_recv, sftp_recv;
ساكن Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
ساكن ssize_t ssh_tls_recv(libssh2_socket_t sock, فراغ *buffer,
                            size_t length, صحيح flags, فراغ **abstract)
{
  بنية Curl_easy *data = (بنية Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nread;
  CURLcode result;
  بنية connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (فراغ)flags;

  إذا(!sshc)
    أرجع -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  إذا(result == CURLE_AGAIN)
    أرجع -EAGAIN; /* magic return code for libssh2 */
  وإلا إذا(result)
    أرجع -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (ثابت حرف *)buffer, nread);
  أرجع (ssize_t)nread;
}

ساكن ssize_t ssh_tls_send(libssh2_socket_t sock, ثابت فراغ *buffer,
                            size_t length, صحيح flags, فراغ **abstract)
{
  بنية Curl_easy *data = (بنية Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nwrite;
  CURLcode result;
  بنية connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (فراغ)flags;

  إذا(!sshc)
    أرجع -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  إذا(result == CURLE_AGAIN)
    أرجع -EAGAIN; /* magic return code for libssh2 */
  وإلا إذا(result)
    أرجع -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (ثابت حرف *)buffer, nwrite);
  أرجع (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ساكن CURLcode ssh_connect(بنية Curl_easy *data, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result;

#إذا LIBSSH2_VERSION_اڠک >= 0x010b00
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
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

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

  إذا(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    أرجع CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#إذا LIBSSH2_VERSION_اڠک >= 0x010B00
  إذا(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  إذا(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#إذا LIBSSH2_VERSION_اڠک >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#إذا defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic تولق
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#إذا defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic کلوار
#endif
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
  إذا(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  وإلا {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  إذا(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  إذا(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    صحيح rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    إذا(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = عدم;
      أرجع CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    إذا(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (صحيح)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  أرجع result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ساكن CURLcode scp_perform(بنية Curl_easy *data,
                            bool *connected,
                            bool *dophase_done)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  إذا(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  أرجع result;
}

/* called from multi.c while DOing */
ساكن CURLcode scp_doing(بنية Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  إذا(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  أرجع result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode scp_disconnect(بنية Curl_easy *data,
                               بنية connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (فراغ)dead_connection;

  إذا(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  إذا(sshc)
    أرجع sshc_cleanup(sshc, data, TRUE);
  أرجع result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ساكن CURLcode ssh_done(بنية Curl_easy *data, CURLcode status)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  إذا(!sshc || !sshp)
    أرجع CURLE_FAILED_INIT;

  إذا(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  وإلا
    result = status;

  إذا(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    أرجع CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  أرجع result;
}

ساكن CURLcode scp_done(بنية Curl_easy *data, CURLcode status,
                         bool premature)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (فراغ)premature;

  إذا(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  أرجع ssh_done(data, status);
}

ساكن CURLcode scp_send(بنية Curl_easy *data, int8_t sockindex,
                         ثابت uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  (فراغ)eos;
  *pnwritten = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ثابت حرف *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  إذا(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  وإلا إذا(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((صحيح)nwritten);
  وإلا
    *pnwritten = (size_t)nwritten;

  أرجع result;
}

ساكن CURLcode scp_recv(بنية Curl_easy *data, int8_t sockindex,
                         حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (فراغ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  إذا(nread == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((صحيح)nread);
  وإلا
    *pnread = (size_t)nread;

  أرجع result;
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

ساكن CURLcode sftp_perform(بنية Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  بنية ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  إذا(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  أرجع result;
}

/* called from multi.c while DOing */
ساكن CURLcode sftp_doing(بنية Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  إذا(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  أرجع result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ساكن CURLcode sftp_disconnect(بنية Curl_easy *data,
                                بنية connectdata *conn, bool dead_connection)
{
  CURLcode result = CURLE_OK;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  بنية SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (فراغ)dead_connection;

  إذا(sshc) {
    إذا(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (صحيح)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  أرجع result;
}

ساكن CURLcode sftp_done(بنية Curl_easy *data, CURLcode status,
                          bool premature)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  إذا(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    إذا(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  أرجع ssh_done(data, status);
}

/* return number of sent bytes */
ساكن CURLcode sftp_send(بنية Curl_easy *data, int8_t sockindex,
                          ثابت uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (فراغ)sockindex;
  (فراغ)eos;
  *pnwritten = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ثابت حرف *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  إذا(nwrite == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nwrite < LIBSSH2_ERROR_NONE)
    أرجع libssh2_session_error_to_CURLE((صحيح)nwrite);
  *pnwritten = (size_t)nwrite;
  أرجع CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ساكن CURLcode sftp_recv(بنية Curl_easy *data, int8_t sockindex,
                          حرف *mem, size_t len, size_t *pnread)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (فراغ)sockindex;
  *pnread = 0;

  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  إذا(nread == LIBSSH2_ERROR_EAGAIN)
    أرجع CURLE_AGAIN;
  وإلا إذا(nread < 0)
    أرجع libssh2_session_error_to_CURLE((صحيح)nread);

  *pnread = (size_t)nread;
  أرجع CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
ساكن CURLcode ssh_do(بنية Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  إذا(!sshc)
    أرجع CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  إذا(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  وإلا
    result = sftp_perform(data, &connected, done);

  أرجع result;
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

فراغ Curl_ssh_version(حرف *buffer, size_t buflen)
{
  (فراغ)curl_msnprintf(buffer, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ساكن فراغ ssh_attach(بنية Curl_easy *data, بنية connectdata *conn)
{
  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  إذا(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    إذا(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      فراغ **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = data;
    }
  }
}

/*
 * SCP protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_scp = {
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
ثابت بنية Curl_protocol Curl_protocol_sftp = {
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
