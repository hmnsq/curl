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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "vssh/ssh.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "parsedate.h" /* for the week day and month names */
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "vssh/vssh.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/base64.h" /* for curlx_base64_encode() */

स्थिर ध्रुव वर्ण *sftp_libssh2_strerror(अचिन्हित दीर्घ err)
{
  विकल्प(err) {
  पक्ष LIBSSH2_FX_NO_SUCH_FILE:
    प्रत्यागम "No such file or directory";

  पक्ष LIBSSH2_FX_PERMISSION_DENIED:
    प्रत्यागम "Permission denied";

  पक्ष LIBSSH2_FX_FAILURE:
    प्रत्यागम "Operation failed";

  पक्ष LIBSSH2_FX_BAD_MESSAGE:
    प्रत्यागम "Bad message from SFTP server";

  पक्ष LIBSSH2_FX_NO_CONNECTION:
    प्रत्यागम "Not connected to SFTP server";

  पक्ष LIBSSH2_FX_CONNECTION_LOST:
    प्रत्यागम "Connection to SFTP server lost";

  पक्ष LIBSSH2_FX_OP_UNSUPPORTED:
    प्रत्यागम "Operation not supported by SFTP server";

  पक्ष LIBSSH2_FX_INVALID_HANDLE:
    प्रत्यागम "Invalid handle";

  पक्ष LIBSSH2_FX_NO_SUCH_PATH:
    प्रत्यागम "No such file or directory";

  पक्ष LIBSSH2_FX_FILE_ALREADY_EXISTS:
    प्रत्यागम "File already exists";

  पक्ष LIBSSH2_FX_WRITE_PROTECT:
    प्रत्यागम "File is write protected";

  पक्ष LIBSSH2_FX_NO_MEDIA:
    प्रत्यागम "No media";

  पक्ष LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    प्रत्यागम "Disk full";

  पक्ष LIBSSH2_FX_QUOTA_EXCEEDED:
    प्रत्यागम "User quota exceeded";

  पक्ष LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    प्रत्यागम "Unknown principal";

  पक्ष LIBSSH2_FX_LOCK_CONFLICT:
    प्रत्यागम "File lock conflict";

  पक्ष LIBSSH2_FX_DIR_NOT_EMPTY:
    प्रत्यागम "Directory not empty";

  पक्ष LIBSSH2_FX_NOT_A_DIRECTORY:
    प्रत्यागम "Not a directory";

  पक्ष LIBSSH2_FX_INVALID_FILENAME:
    प्रत्यागम "Invalid filename";

  पक्ष LIBSSH2_FX_LINK_LOOP:
    प्रत्यागम "Link points to itself";
  }
  प्रत्यागम "Unknown error in libssh2";
}

स्थिर शून्य kbd_callback(ध्रुव वर्ण *name, पूर्णाङ्क name_len,
                         ध्रुव वर्ण *instruction, पूर्णाङ्क instruction_len,
                         पूर्णाङ्क अंक_prompts,
                         ध्रुव LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         शून्य **abstract)
{
  संरचना Curl_easy *data = (संरचना Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(मानक_त्रुटि, "name=%s\n", name);
  curl_mfprintf(मानक_त्रुटि, "name_len=%d\n", name_len);
  curl_mfprintf(मानक_त्रुटि, "instruction=%s\n", instruction);
  curl_mfprintf(मानक_त्रुटि, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(मानक_त्रुटि, "num_prompts=%d\n", अंक_prompts);
#अन्यथा
  (शून्य)name;
  (शून्य)name_len;
  (शून्य)instruction;
  (शून्य)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  यदि(अंक_prompts == 1) {
    संरचना connectdata *conn = data->conn;
    ध्रुव वर्ण *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(म_माप(passwd)) : 0;
  }
  (शून्य)prompts;
} /* kbd_callback */

स्थिर CURLcode sftp_libssh2_error_to_CURLE(अचिन्हित दीर्घ err)
{
  विकल्प(err) {
  पक्ष LIBSSH2_FX_OK:
    प्रत्यागम CURLE_OK;

  पक्ष LIBSSH2_FX_NO_SUCH_FILE:
  पक्ष LIBSSH2_FX_NO_SUCH_PATH:
    प्रत्यागम CURLE_REMOTE_FILE_NOT_FOUND;

  पक्ष LIBSSH2_FX_PERMISSION_DENIED:
  पक्ष LIBSSH2_FX_WRITE_PROTECT:
  पक्ष LIBSSH2_FX_LOCK_CONFLICT:
    प्रत्यागम CURLE_REMOTE_ACCESS_DENIED;

  पक्ष LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  पक्ष LIBSSH2_FX_QUOTA_EXCEEDED:
    प्रत्यागम CURLE_REMOTE_DISK_FULL;

  पक्ष LIBSSH2_FX_FILE_ALREADY_EXISTS:
    प्रत्यागम CURLE_REMOTE_FILE_EXISTS;

  पक्ष LIBSSH2_FX_DIR_NOT_EMPTY:
    प्रत्यागम CURLE_QUOTE_ERROR;

  स्वभाव:
    भङ्ग;
  }

  प्रत्यागम CURLE_SSH;
}

स्थिर CURLcode libssh2_session_error_to_CURLE(पूर्णाङ्क err)
{
  विकल्प(err) {
  /* Ordered by order of appearance in libssh2.h */
  पक्ष LIBSSH2_ERROR_NONE:
    प्रत्यागम CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  पक्ष LIBSSH2_ERROR_SCP_PROTOCOL:
    प्रत्यागम CURLE_REMOTE_FILE_NOT_FOUND;

  पक्ष LIBSSH2_ERROR_SOCKET_NONE:
    प्रत्यागम CURLE_COULDNT_CONNECT;

  पक्ष LIBSSH2_ERROR_ALLOC:
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  पक्ष LIBSSH2_ERROR_SOCKET_SEND:
    प्रत्यागम CURLE_SEND_ERROR;

  पक्ष LIBSSH2_ERROR_HOSTKEY_INIT:
  पक्ष LIBSSH2_ERROR_HOSTKEY_SIGN:
  पक्ष LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  पक्ष LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;

  पक्ष LIBSSH2_ERROR_PASSWORD_EXPIRED:
    प्रत्यागम CURLE_LOGIN_DENIED;

  पक्ष LIBSSH2_ERROR_SOCKET_TIMEOUT:
  पक्ष LIBSSH2_ERROR_TIMEOUT:
    प्रत्यागम CURLE_OPERATION_TIMEDOUT;

  पक्ष LIBSSH2_ERROR_EAGAIN:
    प्रत्यागम CURLE_AGAIN;
  }

  प्रत्यागम CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

स्थिर LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (शून्य)abstract;
  प्रत्यागम Curl_cmalloc(गिनती);
}

स्थिर LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (शून्य)abstract;
  प्रत्यागम Curl_crealloc(ptr, गिनती);
}

स्थिर LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (शून्य)abstract;
  यदि(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

स्थिर पूर्णाङ्क sshkeycallback(CURL *easy,
                          ध्रुव संरचना curl_khkey *knownkey, /* known */
                          ध्रुव संरचना curl_khkey *foundkey, /* found */
                          गणना curl_khmatch match,
                          शून्य *clientp)
{
  (शून्य)easy;
  (शून्य)knownkey;
  (शून्य)foundkey;
  (शून्य)clientp;

  /* we only allow perfect matches, and we reject everything else */
  प्रत्यागम (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

स्थिर गणना curl_khtype convert_ssh2_keytype(पूर्णाङ्क sshkeytype)
{
  गणना curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  विकल्प(sshkeytype) {
  पक्ष LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    भङ्ग;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  पक्ष LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    भङ्ग;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    भङ्ग;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    भङ्ग;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    भङ्ग;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  पक्ष LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    भङ्ग;
#endif
  }
  प्रत्यागम keytype;
}

स्थिर CURLcode ssh_knownhost(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  संरचना libssh2_knownhost *host = रिक्त;
  ध्रुव वर्ण *remotekey = रिक्त;
  पूर्णाङ्क keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  पूर्णाङ्क keybit = 0;
  पूर्णाङ्क sshkeytype = 0;
  माप_प्रकार keylen = 0;
  पूर्णाङ्क rc = 0;
  CURLcode result = CURLE_OK;

  यदि(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
    प्रत्यागम CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  यदि(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    संरचना Curl_mapi_guard guard;
    गणना curl_khmatch keymatch;
    curl_sshkeycallback func =
      data->set.ssh_keyfunc ? data->set.ssh_keyfunc : sshkeycallback;
    संरचना curl_khkey knownkey;
    संरचना curl_khkey *knownkeyp = रिक्त;
    संरचना curl_khkey foundkey;

    विकल्प(sshkeytype) {
    पक्ष LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_SSHRSA;
      भङ्ग;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    पक्ष LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_कुंजी_SSHDSS;
      भङ्ग;
#endif
    पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_ECDSA_256;
      भङ्ग;
    पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_ECDSA_384;
      भङ्ग;
    पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_ECDSA_521;
      भङ्ग;
    पक्ष LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_ED25519;
      भङ्ग;
    स्वभाव:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      भङ्ग;
    }
    यदि(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    अन्यथा {
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
            host->कुंजी : "<none>");

      /* setup 'knownkey' */
      यदि(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.कुंजी = host->कुंजी;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.कुंजी = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (गणना curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  अन्यथा {
    /* no remotekey means failure! */
    infof(data, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  विकल्प(rc) {
  स्वभाव: /* unknown return codes is the same as reject */
  पक्ष CURLKHSTAT_REJECT:
    infof(data, "SSH: knownhost check failed");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  पक्ष CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    result = CURLE_PEER_FAILED_VERIFICATION;
    भङ्ग;
  पक्ष CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    यदि(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  पक्ष CURLKHSTAT_FINE:
  पक्ष CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    यदि(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      पूर्णाङ्क addrc;
      ध्रुव वर्ण *hostbuf;
      वर्ण *hostport = रिक्त;
      यदि(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        यदि(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
      }
      अन्यथा
        hostbuf = conn->origin->hostname;
      यदि(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, रिक्त,
                                       remotekey, keylen, रिक्त, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, रिक्त);
        यदि(addrc)
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        अन्यथा यदि(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          पूर्णाङ्क wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          यदि(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    अन्यथा
      infof(data, "SSH: knownhost entry matches host key");
    भङ्ग;
  }
  प्रत्यागम result;
}

स्थिर CURLcode ssh_check_fingerprint(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  ध्रुव वर्ण *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुंजी_MD5);
  ध्रुव वर्ण *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुंजी_SHA256);

  यदि(pubkey_sha256) {
    ध्रुव वर्ण *fingerprint = रिक्त;
    वर्ण *fingerprint_b64 = रिक्त;
    माप_प्रकार fingerprint_b64_len;
    माप_प्रकार pub_pos = 0;
    माप_प्रकार b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    यदि(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    यदि(curlx_base64_encode((ध्रुव uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }

    यदि(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    यावत्((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    यावत्((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    यदि((pub_pos != b64_pos) ||
       म_भेदन(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  यदि(pubkey_md5) {
    वर्ण md5buffer[33];
    ध्रुव वर्ण *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    यदि(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      पूर्णाङ्क i;
      प्रति(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (अचिन्हित वर्ण)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    यदि(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      यदि(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      अन्यथा {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  यदि(!pubkey_md5 && !pubkey_sha256) {
    यदि(data->set.ssh_hostkeyfunc) {
      माप_प्रकार keylen = 0;
      पूर्णाङ्क sshkeytype = 0;
      पूर्णाङ्क rc = 0;
      /* we handle the process to the callback */
      ध्रुव वर्ण *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      यदि(remotekey) {
        संरचना Curl_mapi_guard guard;
        गणना curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (पूर्णाङ्क)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        यदि(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      अन्यथा {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
      }
      प्रत्यागम CURLE_OK;
    }
    अन्यथा {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      प्रत्यागम ssh_knownhost(data, sshc);
    }
  }
  अन्यथा {
    /* as we already matched, we skip the check for known hosts */
    प्रत्यागम CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
स्थिर CURLcode ssh_force_knownhost_कुंजी_type(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  स्थिर ध्रुव वर्ण hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  स्थिर ध्रुव वर्ण hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  स्थिर ध्रुव वर्ण hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  स्थिर ध्रुव वर्ण hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  स्थिर ध्रुव वर्ण hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_कुंजी_SSHDSS
  स्थिर ध्रुव वर्ण hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  यदि(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुंजी_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुंजी_SHA256)) {
    संरचना libssh2_knownhost *store = रिक्त;
    संरचना connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_कुंजी_MASK >>
                      LIBSSH2_KNOWNHOST_कुंजी_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    यावत्(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      यदि(store) {
        यदि(store->name) {
          यदि(store->name[0] == '[') {
            curl_off_t port;
            माप_प्रकार kh_name_size = 0;
            ध्रुव वर्ण *p;
            ध्रुव वर्ण *kh_name_end = म_माला(store->name, "]:");
            यदि(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              अनुवर्त;
            }
            p = kh_name_end + 2; /* start of port number */
            यदि(!curlx_str_संख्या(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = म_माप(store->name) - 1 - म_माप(kh_name_end);
              यदि((kh_name_size == म_माप(conn->origin->hostname)) &&
                 !म_भेदन(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                भङ्ग;
              }
            }
          }
          अन्यथा यदि(!म_भेद(store->name, conn->origin->hostname)) {
            found = TRUE;
            भङ्ग;
          }
        }
        अन्यथा यदि(store->कुंजी) {
          अचिन्हित पूर्णाङ्क type = (store->typemask & LIBSSH2_KNOWNHOST_कुंजी_MASK)
                              >> LIBSSH2_KNOWNHOST_कुंजी_SHIFT;
          यदि(!absent_type[type]) {
            पूर्णाङ्क keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->कुंजी, म_माप(store->कुंजी),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_कुंजी_MASK),
              रिक्त);
            यदि(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              भङ्ग;
            }
            अन्यथा यदि(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    यदि(found) {
      पूर्णाङ्क rc;
      ध्रुव वर्ण *hostkey_method = रिक्त;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      विकल्प(store->typemask & LIBSSH2_KNOWNHOST_कुंजी_MASK) {
      पक्ष LIBSSH2_KNOWNHOST_कुंजी_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुंजी_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुंजी_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुंजी_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुंजी_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        भङ्ग;
#ifdef LIBSSH2_KNOWNHOST_कुंजी_SSHDSS
      पक्ष LIBSSH2_KNOWNHOST_कुंजी_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        भङ्ग;
#endif
      पक्ष LIBSSH2_KNOWNHOST_कुंजी_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        प्रत्यागम CURLE_SSH;
      स्वभाव:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_कुंजी_MASK));
        प्रत्यागम CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      यदि(rc) {
        वर्ण *err_संदेश = रिक्त;
        पूर्णाङ्क errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_संदेश, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_संदेश);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    अन्यथा {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  प्रत्यागम result;
}

स्थिर CURLcode quote_error(संरचना Curl_easy *data,
                            संरचना ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  प्रत्यागम CURLE_QUOTE_ERROR;
}

स्थिर CURLcode sftp_quote(संरचना Curl_easy *data,
                           संरचना ssh_conn *sshc,
                           संरचना SSHPROTO *sshp)
{
  ध्रुव वर्ण *cp;
  CURLcode result = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ध्रुव वर्ण *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  यदि(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  यदि(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    वर्ण *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    यदि(!tmp)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, म_माप(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, म_माप(tmp));
    curlx_free(tmp);
    यदि(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    प्रत्यागम result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = म_अक्षर(cmd, ' ');
  यदि(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    प्रत्यागम CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  यदि(result) {
    यदि(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    प्रत्यागम result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  यदि(!म_भेदन(cmd, "chgrp ", 6) ||
     !म_भेदन(cmd, "chmod ", 6) ||
     !म_भेदन(cmd, "chown ", 6) ||
     !म_भेदन(cmd, "atime ", 6) ||
     !म_भेदन(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    यदि(result) {
      यदि(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      प्रत्यागम result;
    }
    यदि(*cp)
      प्रत्यागम quote_error(data, sshc);

    स_रखो(&sshp->quote_attrs, 0, परिमाण(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    प्रत्यागम result;
  }
  यदि(!म_भेदन(cmd, "ln ", 3) ||
     !म_भेदन(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    यदि(result) {
      यदि(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      प्रत्यागम result;
    }
    यदि(*cp)
      प्रत्यागम quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    प्रत्यागम result;
  }
  अन्यथा यदि(!म_भेदन(cmd, "mkdir ", 6)) {
    यदि(*cp)
      प्रत्यागम quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    प्रत्यागम result;
  }
  अन्यथा यदि(!म_भेदन(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    यदि(result) {
      यदि(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      प्रत्यागम result;
    }
    यदि(*cp)
      प्रत्यागम quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    प्रत्यागम result;
  }
  अन्यथा यदि(!म_भेदन(cmd, "rmdir ", 6)) {
    यदि(*cp)
      प्रत्यागम quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    प्रत्यागम result;
  }
  अन्यथा यदि(!म_भेदन(cmd, "rm ", 3)) {
    यदि(*cp)
      प्रत्यागम quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    प्रत्यागम result;
  }
  अन्यथा यदि(!म_भेदन(cmd, "statvfs ", 8)) {
    यदि(*cp)
      प्रत्यागम quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    प्रत्यागम result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  प्रत्यागम CURLE_QUOTE_ERROR;
}

स्थिर CURLcode sftp_upload_init(संरचना Curl_easy *data,
                                 संरचना ssh_conn *sshc,
                                 संरचना SSHPROTO *sshp,
                                 bool *blockp)
{
  अचिन्हित दीर्घ flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  यदि(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    यदि(data->state.resume_from < 0) {
      पूर्णाङ्क rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(म_माप(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      यदि(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        प्रत्यागम CURLE_OK;
      }
      यदि(rc) {
        data->state.resume_from = 0;
      }
      अन्यथा {
        curl_off_t size = attrs.filesize;
        यदि(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  यदि(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  अन्यथा यदि(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  अन्यथा {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(म_माप(sshp->path)),
                         flags, (दीर्घ)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  यदि(!sshc->sftp_handle) {
    CURLcode result;
    अचिन्हित दीर्घ sftperr;
    पूर्णाङ्क rc = libssh2_session_last_errno(sshc->ssh_session);

    यदि(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      प्रत्यागम CURLE_OK;
    }

    यदि(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    अन्यथा
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    यदि(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      प्रत्यागम sftp_libssh2_error_to_CURLE(sftperr);
    }
    यदि((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (म_माप(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      प्रत्यागम CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    यदि(!result) {
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
    प्रत्यागम result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  यदि(data->state.resume_from > 0 && !data->set.remote_append) {
    पूर्णाङ्क seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    यदि(data->set.seek_func) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, शुरू_से);
      CURL_CBAPI_END(&guard);
    }

    यदि(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      यदि(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        प्रत्यागम CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      कुरु {
        संरचना Curl_mapi_guard guard;
        वर्ण scratch[4 * 1024];
        माप_प्रकार readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)परिमाण(scratch)) ?
          परिमाण(scratch) : curlx_sotouz(data->state.resume_from - passed);

        माप_प्रकार actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        यदि((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          प्रत्यागम CURLE_FTP_COULDNT_USE_REST;
        }
      } यावत्(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    यदि(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  यदि(data->state.infilesize > 0) {
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
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य ssh_state_pkey_init(संरचना Curl_easy *data, संरचना ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  यदि((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     म_माला(sshc->authlist, "publickey")) {
    यदि(sshc->pub_कुंजी)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_कुंजी);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_कुंजी);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  अन्यथा
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
}

स्थिर CURLcode sftp_quote_stat(संरचना Curl_easy *data,
                                संरचना ssh_conn *sshc,
                                संरचना SSHPROTO *sshp,
                                bool *blockp)
{
  ध्रुव वर्ण *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  यदि(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  यदि(म_भेदन(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    पूर्णाङ्क rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(म_माप(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    यदि(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      प्रत्यागम CURLE_OK;
    }
    यदि(rc && !sshc->acceptfail) { /* get those attributes */
      अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      गच्छ fail;
    }
  }

  /* Now set the new attributes... */
  यदि(!म_भेदन(cmd, "chgrp", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t gid;
    यदि(!curlx_str_संख्या(&p, &gid, अच_दीर्घ_उच्च)) {
      sshp->quote_attrs.gid = (अचिन्हित दीर्घ)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    अन्यथा यदि(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      गच्छ fail;
    }
  }
  अन्यथा यदि(!म_भेदन(cmd, "chmod", 5)) {
    curl_off_t perms;
    ध्रुव वर्ण *p = sshc->quote_path1;
    /* permissions are octal */
    यदि(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      गच्छ fail;
    }

    sshp->quote_attrs.permissions = (अचिन्हित दीर्घ)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  अन्यथा यदि(!म_भेदन(cmd, "chown", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t uid;
    यदि(!curlx_str_संख्या(&p, &uid, अच_दीर्घ_उच्च)) {
      sshp->quote_attrs.uid = (अचिन्हित दीर्घ)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    अन्यथा यदि(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      गच्छ fail;
    }
  }
  अन्यथा यदि(!म_भेदन(cmd, "atime", 5) ||
          !म_भेदन(cmd, "mtime", 5)) {
    समय_प्रकार date;
    bool fail = FALSE;

    यदि(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#यदि SIZEOF_TIME_T > SIZEOF_LONG
    यदि(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    यदि(fail)
      गच्छ fail;
    यदि(!म_भेदन(cmd, "atime", 5))
      sshp->quote_attrs.atime = (अचिन्हित दीर्घ)date;
    अन्यथा /* mtime */
      sshp->quote_attrs.mtime = (अचिन्हित दीर्घ)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  प्रत्यागम CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  प्रत्यागम CURLE_QUOTE_ERROR;
}

स्थिर CURLcode sftp_download_stat(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc,
                                   संरचना SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  पूर्णाङ्क rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(म_माप(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  यदि(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    प्रत्यागम CURLE_OK;
  }
  यदि(rc ||
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
  अन्यथा {
    curl_off_t size = attrs.filesize;

    यदि(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
    }
    यदि(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      यदि(result)
        प्रत्यागम result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  यदि(data->state.resume_from) {
    यदि(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      यदि((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    अन्यथा {
      यदि((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
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
  यदि(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    प्रत्यागम CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode sftp_readdir(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc,
                             संरचना SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode result = CURLE_OK;
  पूर्णाङ्क rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  यदि(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    प्रत्यागम result;
  }
  यदि(rc > 0) {
    माप_प्रकार readdir_len = (माप_प्रकार)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    यदि(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      यदि(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      यदि(result)
        प्रत्यागम result;
    }
    अन्यथा {
      result = curlx_dyn_add(&sshp->सूची_पढ़ो, sshp->readdir_longentry);

      यदि(!result) {
        यदि((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        अन्यथा {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      प्रत्यागम result;
    }
  }
  अन्यथा यदि(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  अन्यथा {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_init(संरचना Curl_easy *data,
                               संरचना ssh_conn *sshc)
{
  CURLcode result;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  result = ssh_force_knownhost_कुंजी_type(data, sshc);
  यदि(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  अन्यथा
    myssh_to(data, sshc, SSH_S_STARTUP);
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_startup(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  पूर्णाङ्क rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc) {
    वर्ण *err_संदेश = रिक्त;
    (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_संदेश);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_hostkey(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode result = ssh_check_fingerprint(data, sshc);
  यदि(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_authlist(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc)
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
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(म_माप(user)));

  यदि(!sshc->authlist) {
    पूर्णाङ्क rc;
    यदि(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      प्रत्यागम CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    प्रत्यागम libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_pkey(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  पूर्णाङ्क rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(म_माप(user)),
    sshc->pub_कुंजी, sshc->priv_कुंजी, sshc->passphrase ? sshc->passphrase : "");
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  अन्यथा {
    वर्ण *err_संदेश = रिक्त;
    वर्ण unknown[] = "Reason unknown (-1)";
    यदि(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_संदेश = unknown;
    }
    अन्यथा {
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_संदेश);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_pass_init(संरचना Curl_easy *data,
                                         संरचना ssh_conn *sshc)
{
  यदि((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     म_माला(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_pass(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  ध्रुव वर्ण *passwd = Curl_creds_passwd(conn->creds);
  पूर्णाङ्क rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(म_माप(user)),
                                 passwd,
                                 curlx_uztoui(म_माप(passwd)),
                                 रिक्त);
  यदि(rc == LIBSSH2_ERROR_EAGAIN) {
    प्रत्यागम CURLE_AGAIN;
  }
  यदि(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_host_init(संरचना Curl_easy *data,
                                         संरचना ssh_conn *sshc)
{
  यदि((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     म_माला(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_agent_init(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = 0;
  यदि((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     म_माला(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    यदि(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      यदि(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_कुंजी_INIT);
        प्रत्यागम CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_कुंजी_INIT);
    }
    अन्यथा {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  अन्यथा
    myssh_to(data, sshc, SSH_AUTH_कुंजी_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_agent_list(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_agent_list_identities(sshc->ssh_agent);

  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;
  यदि(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_कुंजी_INIT);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = रिक्त;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_agent(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  पूर्णाङ्क rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_prev_identity);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    यदि(rc < 0) {
      यदि(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        प्रत्यागम CURLE_OK;
      }
      प्रत्यागम CURLE_AGAIN;
    }
  }

  यदि(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  अन्यथा यदि(rc == 1)
    infof(data, "SSH: no agent identity would match");

  यदि(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_AUTH_कुंजी_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_कुंजी_init(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc)
{
  यदि((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     म_माला(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_कुंजी);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_कुंजी(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  पूर्णाङ्क rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(म_माप(user)),
                                             &kbd_callback);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    प्रत्यागम CURLE_OK;
  }
  प्रत्यागम CURLE_LOGIN_DENIED;
}

स्थिर CURLcode ssh_state_auth_done(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  यदि(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    प्रत्यागम CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  यदि(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    प्रत्यागम CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_init(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  यदि(!sshc->sftp_session) {
    वर्ण *err_संदेश = रिक्त;
    यदि(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
    failf(data, "Failure initializing sftp session: %s", err_संदेश);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    प्रत्यागम CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_realpath(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc,
                                        संरचना SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  पूर्णाङ्क rc;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  यदि(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    यदि(!sshc->homedir)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    यदि(!data->state.most_recent_ftp_entrypath)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
  }
  अन्यथा {
    /* Return the error type */
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    यदि(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    अन्यथा
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (पूर्णाङ्क)result);
    प्रत्यागम result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_init(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  CURLcode result;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  यदि(result) {
    myssh_to(data, sshc, SSH_STOP);
    प्रत्यागम result;
  }

  यदि(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_postquote_init(संरचना Curl_easy *data,
                                              संरचना ssh_conn *sshc)
{
  यदि(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_STOP);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc,
                                     संरचना SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  यदि(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_sftp_next_quote(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  यदि(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    यदि(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    अन्यथा {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_stat(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  यदि(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_sftp_quote_setstat(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc,
                                             संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क rc;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(म_माप(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_symlink(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(म_माप(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(म_माप(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_mkdir(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(म_माप(sshc->quote_path1)),
                                 (दीर्घ)data->set.new_directory_perms);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_rename(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(म_माप(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(म_माप(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_rmdir(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(म_माप(sshc->quote_path1)));
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_unlink(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(म_माप(sshc->quote_path1)));
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_statvfs(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  पूर्णाङ्क rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(म_माप(sshc->quote_path1)),
                                &statvfs);

  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  अन्यथा यदि(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#अन्यथा
#define CURL_LIBSSH2_VFS_SIZE_MASK "llu"
#endif
    CURLcode result;
    वर्ण *tmp = curl_maprintf("statvfs:\n"
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
    यदि(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, म_माप(tmp));
    curlx_free(tmp);
    यदि(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      प्रत्यागम result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_create_dirs_mkdir(संरचना Curl_easy *data,
                                                 संरचना ssh_conn *sshc,
                                                 संरचना SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  पूर्णाङ्क rc;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(म_माप(sshp->path)),
                             (दीर्घ)data->set.new_directory_perms);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  यदि(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    यदि((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      प्रत्यागम sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_readdir_init(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc,
                                            संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  यदि(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    प्रत्यागम CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(म_माप(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  यदि(!sshc->sftp_handle) {
    अचिन्हित दीर्घ sftperr;
    यदि(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    प्रत्यागम sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_readdir_link(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc,
                                            संरचना SSHPROTO *sshp)
{
  CURLcode result;
  पूर्णाङ्क rc;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (अचिन्हित पूर्णाङ्क)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  यदि(rc < 0)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->सूची_पढ़ो, " -> %s", sshp->readdir_filename);
  यदि(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  अन्यथा
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_scp_download_init(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc,
                                            संरचना SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  स_रखो(&sb, 0, परिमाण(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  यदि(!sshc->ssh_channel) {
    पूर्णाङ्क ssh_err;
    वर्ण *err_संदेश = रिक्त;

    यदि(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
    failf(data, "%s", err_संदेश);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    प्रत्यागम libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_close(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc,
                                     संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क rc = 0;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    यदि(rc < 0) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_संदेश);
    }
    sshc->sftp_handle = रिक्त;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  यदि(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  अन्यथा
    myssh_to(data, sshc, SSH_STOP);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_shutdown(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  पूर्णाङ्क rc = 0;
  यदि(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    यदि(rc < 0) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_संदेश);
    }
    sshc->sftp_handle = रिक्त;
  }
  यदि(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    यदि(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = रिक्त;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_download_init(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc,
                                             संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(म_माप(sshp->path)),
                         LIBSSH2_FXF_READ, (दीर्घ)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  यदि(!sshc->sftp_handle) {
    अचिन्हित दीर्घ sftperr;
    यदि(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      प्रत्यागम CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    प्रत्यागम sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_upload_init(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (पूर्णाङ्क)data->set.new_file_perms,
                       (libssh2_int64_t)data->state.infilesize, 0, 0);
  यदि(!sshc->ssh_channel) {
    पूर्णाङ्क ssh_err;
    वर्ण *err_संदेश = रिक्त;
    CURLcode result;
    यदि(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
    failf(data, "%s", err_संदेश);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    यदि(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    प्रत्यागम result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_session_disconnect(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  पूर्णाङ्क rc = 0;
  यदि(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    यदि(rc < 0) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_संदेश);
    }
    sshc->ssh_channel = रिक्त;
  }

  यदि(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    यदि(rc < 0) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_संदेश);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode sshc_cleanup(संरचना ssh_conn *sshc, संरचना Curl_easy *data,
                             bool block)
{
  पूर्णाङ्क rc;

  यदि(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = रिक्त;
  }

  यदि(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    यदि((rc < 0) && data) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_संदेश);
    }
    यदि(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      प्रत्यागम CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = रिक्त;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = रिक्त;
    sshc->sshagent_prev_identity = रिक्त;
  }

  यदि(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    यदि((rc < 0) && data) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_संदेश);
    }
    यदि(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      प्रत्यागम CURLE_AGAIN;

    sshc->sftp_handle = रिक्त;
  }

  यदि(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    यदि((rc < 0) && data) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_संदेश);
    }
    यदि(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      प्रत्यागम CURLE_AGAIN;

    sshc->ssh_channel = रिक्त;
  }

  यदि(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    यदि((rc < 0) && data) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_संदेश);
    }
    यदि(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      प्रत्यागम CURLE_AGAIN;

    sshc->sftp_session = रिक्त;
  }

  यदि(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    यदि((rc < 0) && data) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, रिक्त, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_संदेश);
    }
    यदि(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      प्रत्यागम CURLE_AGAIN;

    sshc->ssh_session = रिक्त;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_कुंजी);
  curlx_safefree(sshc->priv_कुंजी);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_getinfo(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  यदि(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  अन्यथा
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_filetime(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc,
                                        संरचना SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  पूर्णाङ्क rc;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(म_माप(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc == 0)
    data->info.filetime = (समय_प्रकार)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_trans_init(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  यदि(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  अन्यथा यदि(sshp) {
    माप_प्रकार plen = म_माप(sshp->path);
    यदि(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    अन्यथा
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  अन्यथा
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_upload_init(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc,
                                           संरचना SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  यदि(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_sftp_create_dirs_init(संरचना Curl_easy *data,
                                                संरचना ssh_conn *sshc,
                                                संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(म_माप(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  अन्यथा
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_create_dirs(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc,
                                           संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  sshc->slash_pos = म_अक्षर(sshc->slash_pos, '/');
  यदि(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    प्रत्यागम CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_readdir(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc,
                                       संरचना SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  यदि(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_sftp_readdir_bottom(संरचना Curl_easy *data,
                                              संरचना ssh_conn *sshc,
                                              संरचना SSHPROTO *sshp)
{
  CURLcode result;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->सूची_पढ़ो, "\n", 1);
  यदि(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->सूची_पढ़ो),
                               curlx_dyn_len(&sshp->सूची_पढ़ो));
  यदि(result) {
    curlx_dyn_free(&sshp->सूची_पढ़ो);
    myssh_to(data, sshc, SSH_STOP);
  }
  अन्यथा {
    curlx_dyn_reset(&sshp->सूची_पढ़ो);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_sftp_readdir_done(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc)
{
  यदि(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  sshc->sftp_handle = रिक्त;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_download_stat(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc,
                                             संरचना SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  यदि(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  प्रत्यागम result;
}

स्थिर CURLcode ssh_state_scp_trans_init(संरचना Curl_easy *data,
                                         संरचना ssh_conn *sshc,
                                         संरचना SSHPROTO *sshp)
{
  CURLcode result;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  यदि(result) {
    myssh_to(data, sshc, SSH_STOP);
    प्रत्यागम result;
  }

  यदि(data->state.upload) {
    यदि(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      प्रत्यागम CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  अन्यथा
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_done(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc)
{
  यदि(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  अन्यथा
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_send_eof(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  यदि(sshc->ssh_channel) {
    पूर्णाङ्क rc = libssh2_channel_send_eof(sshc->ssh_channel);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session,
                                       &err_संदेश, रिक्त, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_संदेश);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_wait_eof(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  यदि(sshc->ssh_channel) {
    पूर्णाङ्क rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session,
                                       &err_संदेश, रिक्त, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_संदेश);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_wait_close(संरचना Curl_easy *data,
                                         संरचना ssh_conn *sshc)
{
  यदि(sshc->ssh_channel) {
    पूर्णाङ्क rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session,
                                       &err_संदेश, रिक्त, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_संदेश);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_channel_free(
  संरचना Curl_easy *data,
  संरचना ssh_conn *sshc)
{
  यदि(sshc->ssh_channel) {
    पूर्णाङ्क rc = libssh2_channel_free(sshc->ssh_channel);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc < 0) {
      वर्ण *err_संदेश = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session,
                                       &err_संदेश, रिक्त, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_संदेश);
    }
    sshc->ssh_channel = रिक्त;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_session_free(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  संरचना connectdata *conn = data->conn;
  यदि(result)
    प्रत्यागम result;
  स_रखो(sshc, 0, परिमाण(संरचना ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
स्थिर CURLcode ssh_statemachine(संरचना Curl_easy *data,
                                 संरचना ssh_conn *sshc,
                                 संरचना SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode result = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  कुरु {
    विकल्प(sshc->state) {
    पक्ष SSH_INIT:
      result = ssh_state_init(data, sshc);
      यदि(result)
        भङ्ग;
      FALLTHROUGH();

    पक्ष SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      यदि(result)
        भङ्ग;
      FALLTHROUGH();

    पक्ष SSH_HOSTKEY:
      result = ssh_state_hostkey(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTHLIST:
      result = ssh_state_authlist(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_PKEY:
      result = ssh_state_auth_pkey(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_PASS_INIT:
      result = ssh_state_auth_pass_init(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_PASS:
      result = ssh_state_auth_pass(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_HOST_INIT:
      result = ssh_state_auth_host_init(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_HOST:
      myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
      भङ्ग;

    पक्ष SSH_AUTH_AGENT_INIT:
      result = ssh_state_auth_agent_init(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_AGENT_LIST:
      result = ssh_state_auth_agent_list(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_AGENT:
      result = ssh_state_auth_agent(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_कुंजी_INIT:
      result = ssh_state_auth_कुंजी_init(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_कुंजी:
      result = ssh_state_auth_कुंजी(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_DONE:
      result = ssh_state_auth_done(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_INIT:
      result = ssh_state_sftp_init(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_REALPATH:
      result = ssh_state_sftp_realpath(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_INIT:
      result = ssh_state_sftp_quote_init(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_POSTQUOTE_INIT:
      result = ssh_state_sftp_postquote_init(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE:
      result = ssh_state_sftp_quote(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_NEXT_QUOTE:
      result = ssh_state_sftp_next_quote(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_STAT:
      result = ssh_state_sftp_quote_stat(data, sshc, sshp, block);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_SETSTAT:
      result = ssh_state_sftp_quote_setstat(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_SYMLINK:
      result = ssh_state_sftp_quote_symlink(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_MKDIR:
      result = ssh_state_sftp_quote_mkdir(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_RENAME:
      result = ssh_state_sftp_quote_rename(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_RMDIR:
      result = ssh_state_sftp_quote_rmdir(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_UNLINK:
      result = ssh_state_sftp_quote_unlink(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_STATVFS:
      result = ssh_state_sftp_quote_statvfs(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_GETINFO:
      result = ssh_state_sftp_getinfo(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_FILETIME:
      result = ssh_state_sftp_filetime(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_TRANS_INIT:
      result = ssh_state_sftp_trans_init(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_UPLOAD_INIT:
      result = ssh_state_sftp_upload_init(data, sshc, sshp, block);
      भङ्ग;

    पक्ष SSH_SFTP_CREATE_DIRS_INIT:
      result = ssh_state_sftp_create_dirs_init(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_CREATE_DIRS:
      result = ssh_state_sftp_create_dirs(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_CREATE_DIRS_MKDIR:
      result = ssh_state_sftp_create_dirs_mkdir(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR_INIT:
      result = ssh_state_sftp_readdir_init(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR:
      result = ssh_state_sftp_readdir(data, sshc, sshp, block);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR_LINK:
      result = ssh_state_sftp_readdir_link(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR_BOTTOM:
      result = ssh_state_sftp_readdir_bottom(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR_DONE:
      result = ssh_state_sftp_readdir_done(data, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_DOWNLOAD_INIT:
      result = ssh_state_sftp_download_init(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_DOWNLOAD_STAT:
      result = ssh_state_sftp_download_stat(data, sshc, sshp, block);
      भङ्ग;

    पक्ष SSH_SFTP_CLOSE:
      result = ssh_state_sftp_close(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_SHUTDOWN:
      result = ssh_state_sftp_shutdown(data, sshc);
      भङ्ग;

    पक्ष SSH_SCP_TRANS_INIT:
      result = ssh_state_scp_trans_init(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SCP_UPLOAD_INIT:
      result = ssh_state_scp_upload_init(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SCP_DOWNLOAD_INIT:
      result = ssh_state_scp_download_init(data, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SCP_DONE:
      result = ssh_state_scp_done(data, sshc);
      भङ्ग;

    पक्ष SSH_SCP_SEND_EOF:
      result = ssh_state_scp_send_eof(data, sshc);
      भङ्ग;

    पक्ष SSH_SCP_WAIT_EOF:
      result = ssh_state_scp_wait_eof(data, sshc);
      भङ्ग;

    पक्ष SSH_SCP_WAIT_CLOSE:
      result = ssh_state_scp_wait_close(data, sshc);
      भङ्ग;

    पक्ष SSH_SCP_CHANNEL_FREE:
      result = ssh_state_scp_channel_free(data, sshc);
      भङ्ग;

    पक्ष SSH_SESSION_DISCONNECT:
      result = ssh_state_session_disconnect(data, sshc);
      भङ्ग;

    पक्ष SSH_SESSION_FREE:
      result = ssh_state_session_free(data, sshc);
      भङ्ग;

    पक्ष SSH_QUIT:
    स्वभाव:
      /* internal error */
      myssh_to(data, sshc, SSH_STOP);
      भङ्ग;
    }

  } यावत्(!result && (sshc->state != SSH_STOP) && !*block);

  यदि(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (पूर्णाङ्क)result, *block);

  प्रत्यागम result;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
स्थिर शून्य ssh_block2waitfor(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc,
                              bool block)
{
  पूर्णाङ्क dir = 0;
  (शून्य)data;
  यदि(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    यदि(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  यदि(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode ssh_multi_statemach(संरचना Curl_easy *data, bool *done)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  यदि(!sshc || !sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  कुरु {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } यावत्(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  प्रत्यागम result;
}

स्थिर CURLcode ssh_block_statemach(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  संरचना curltime start = *Curl_pgrs_now(data);

  यावत्((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t बायाँ_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    यदि(result)
      भङ्ग;

    यदि(!disconnect) {
      result = Curl_pgrsCheck(data);
      यदि(result)
        भङ्ग;

      बायाँ_ms = Curl_timeleft_ms(data);
      यदि(बायाँ_ms < 0) {
        failf(data, "Operation timed out");
        प्रत्यागम CURLE_OPERATION_TIMEDOUT;
      }
    }
    अन्यथा यदि(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      भङ्ग;
    }

    यदि(block) {
      पूर्णाङ्क dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      यदि(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      यदि(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (शून्य)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!बायाँ_ms || बायाँ_ms > 1000) ? 1000 : बायाँ_ms);
    }
  }

  प्रत्यागम result;
}

स्थिर शून्य myssh_easy_dtor(ध्रुव शून्य *कुंजी, माप_प्रकार klen, शून्य *entry)
{
  संरचना SSHPROTO *sshp = entry;
  (शून्य)कुंजी;
  (शून्य)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->सूची_पढ़ो);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

स्थिर शून्य myssh_conn_dtor(ध्रुव शून्य *कुंजी, माप_प्रकार klen, शून्य *entry)
{
  संरचना ssh_conn *sshc = entry;
  (शून्य)कुंजी;
  (शून्य)klen;
  sshc_cleanup(sshc, रिक्त, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
स्थिर CURLcode ssh_setup_connection(संरचना Curl_easy *data,
                                     संरचना connectdata *conn)
{
  संरचना ssh_conn *sshc;
  संरचना SSHPROTO *sshp;
  (शून्य)conn;

  sshc = curlx_calloc(1, परिमाण(*sshc));
  यदि(!sshc)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  यदि(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, परिमाण(*sshp));
  यदि(!sshp)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->सूची_पढ़ो, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  यदि(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  प्रत्यागम Curl_ssh_setup_pkey(data, sshc);
}

स्थिर Curl_recv scp_recv, sftp_recv;
स्थिर Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
स्थिर ssize_t ssh_tls_recv(libssh2_socket_t sock, शून्य *buffer,
                            माप_प्रकार length, पूर्णाङ्क flags, शून्य **abstract)
{
  संरचना Curl_easy *data = (संरचना Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  माप_प्रकार nread;
  CURLcode result;
  संरचना connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (शून्य)flags;

  यदि(!sshc)
    प्रत्यागम -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  यदि(result == CURLE_AGAIN)
    प्रत्यागम -EAGAIN; /* magic return code for libssh2 */
  अन्यथा यदि(result)
    प्रत्यागम -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (ध्रुव वर्ण *)buffer, nread);
  प्रत्यागम (ssize_t)nread;
}

स्थिर ssize_t ssh_tls_send(libssh2_socket_t sock, ध्रुव शून्य *buffer,
                            माप_प्रकार length, पूर्णाङ्क flags, शून्य **abstract)
{
  संरचना Curl_easy *data = (संरचना Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  माप_प्रकार nwrite;
  CURLcode result;
  संरचना connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (शून्य)flags;

  यदि(!sshc)
    प्रत्यागम -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  यदि(result == CURLE_AGAIN)
    प्रत्यागम -EAGAIN; /* magic return code for libssh2 */
  अन्यथा यदि(result)
    प्रत्यागम -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (ध्रुव वर्ण *)buffer, nwrite);
  प्रत्यागम (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
स्थिर CURLcode ssh_connect(संरचना Curl_easy *data, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result;

#यदि LIBSSH2_VERSION_अंक >= 0x010b00
  {
    ध्रुव वर्ण *crypto_str;
    विकल्प(libssh2_crypto_engine()) {
    पक्ष libssh2_gcrypt:
      crypto_str = "libgcrypt";
      भङ्ग;
    पक्ष libssh2_mbedtls:
      crypto_str = "mbedTLS";
      भङ्ग;
    पक्ष libssh2_openssl:
      crypto_str = "openssl compatible";
      भङ्ग;
    पक्ष libssh2_os400qc3:
      crypto_str = "OS400QC3";
      भङ्ग;
    पक्ष libssh2_wincng:
      crypto_str = "WinCNG";
      भङ्ग;
    स्वभाव:
      crypto_str = रिक्त;
      भङ्ग;
    }
    यदि(crypto_str)
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

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

  यदि(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#यदि LIBSSH2_VERSION_अंक >= 0x010B00
  यदि(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (दीर्घ)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  यदि(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#यदि LIBSSH2_VERSION_अंक >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#यदि defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic धकेलो
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#यदि defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic निकालो
#endif
#अन्यथा
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    सङ्घ receive {
      शून्य *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, शून्य *, माप_प्रकार, पूर्णाङ्क, शून्य **);
    };
    सङ्घ transfer {
      शून्य *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ध्रुव शून्य *, माप_प्रकार, पूर्णाङ्क, शून्य **);
    };
    सङ्घ receive sshrecv;
    सङ्घ transfer sshsend;

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
  यदि(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  अन्यथा {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  यदि(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  यदि(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    पूर्णाङ्क rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    यदि(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = रिक्त;
      प्रत्यागम CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    यदि(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (पूर्णाङ्क)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  प्रत्यागम result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

स्थिर CURLcode scp_perform(संरचना Curl_easy *data,
                            bool *connected,
                            bool *dophase_done)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  यदि(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  प्रत्यागम result;
}

/* called from multi.c while DOing */
स्थिर CURLcode scp_doing(संरचना Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  यदि(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  प्रत्यागम result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode scp_disconnect(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (शून्य)dead_connection;

  यदि(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  यदि(sshc)
    प्रत्यागम sshc_cleanup(sshc, data, TRUE);
  प्रत्यागम result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
स्थिर CURLcode ssh_done(संरचना Curl_easy *data, CURLcode status)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  यदि(!sshc || !sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  अन्यथा
    result = status;

  यदि(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    प्रत्यागम CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  प्रत्यागम result;
}

स्थिर CURLcode scp_done(संरचना Curl_easy *data, CURLcode status,
                         bool premature)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (शून्य)premature;

  यदि(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  प्रत्यागम ssh_done(data, status);
}

स्थिर CURLcode scp_send(संरचना Curl_easy *data, int8_t sockindex,
                         ध्रुव uint8_t *mem, माप_प्रकार len, bool eos,
                         माप_प्रकार *pnwritten)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  (शून्य)eos;
  *pnwritten = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ध्रुव वर्ण *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  यदि(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  अन्यथा यदि(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((पूर्णाङ्क)nwritten);
  अन्यथा
    *pnwritten = (माप_प्रकार)nwritten;

  प्रत्यागम result;
}

स्थिर CURLcode scp_recv(संरचना Curl_easy *data, int8_t sockindex,
                         वर्ण *mem, माप_प्रकार len, माप_प्रकार *pnread)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  यदि(nread == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;
  अन्यथा यदि(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((पूर्णाङ्क)nread);
  अन्यथा
    *pnread = (माप_प्रकार)nread;

  प्रत्यागम result;
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

स्थिर CURLcode sftp_perform(संरचना Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  यदि(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  प्रत्यागम result;
}

/* called from multi.c while DOing */
स्थिर CURLcode sftp_doing(संरचना Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  यदि(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  प्रत्यागम result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode sftp_disconnect(संरचना Curl_easy *data,
                                संरचना connectdata *conn, bool dead_connection)
{
  CURLcode result = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (शून्य)dead_connection;

  यदि(sshc) {
    यदि(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (पूर्णाङ्क)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  प्रत्यागम result;
}

स्थिर CURLcode sftp_done(संरचना Curl_easy *data, CURLcode status,
                          bool premature)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    यदि(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  प्रत्यागम ssh_done(data, status);
}

/* return number of sent bytes */
स्थिर CURLcode sftp_send(संरचना Curl_easy *data, int8_t sockindex,
                          ध्रुव uint8_t *mem, माप_प्रकार len, bool eos,
                          माप_प्रकार *pnwritten)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (शून्य)sockindex;
  (शून्य)eos;
  *pnwritten = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ध्रुव वर्ण *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  यदि(nwrite == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;
  अन्यथा यदि(nwrite < LIBSSH2_ERROR_NONE)
    प्रत्यागम libssh2_session_error_to_CURLE((पूर्णाङ्क)nwrite);
  *pnwritten = (माप_प्रकार)nwrite;
  प्रत्यागम CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
स्थिर CURLcode sftp_recv(संरचना Curl_easy *data, int8_t sockindex,
                          वर्ण *mem, माप_प्रकार len, माप_प्रकार *pnread)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (शून्य)sockindex;
  *pnread = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  यदि(nread == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;
  अन्यथा यदि(nread < 0)
    प्रत्यागम libssh2_session_error_to_CURLE((पूर्णाङ्क)nread);

  *pnread = (माप_प्रकार)nread;
  प्रत्यागम CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
स्थिर CURLcode ssh_do(संरचना Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  यदि(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  अन्यथा
    result = sftp_perform(data, &connected, done);

  प्रत्यागम result;
}

CURLcode Curl_ssh_init(शून्य)
{
  यदि(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: libssh2_init failed\n"));
    प्रत्यागम CURLE_FAILED_INIT;
  }
  प्रत्यागम CURLE_OK;
}

शून्य Curl_ssh_cleanup(शून्य)
{
  libssh2_exit();
}

शून्य Curl_ssh_version(वर्ण *buffer, माप_प्रकार buflen)
{
  (शून्य)curl_msnprintf(buffer, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
स्थिर शून्य ssh_attach(संरचना Curl_easy *data, संरचना connectdata *conn)
{
  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  यदि(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    यदि(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      शून्य **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = data;
    }
  }
}

/*
 * SCP protocol handler.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_scp = {
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
ध्रुव संरचना Curl_protocol Curl_protocol_sftp = {
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
