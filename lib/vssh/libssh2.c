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

स्थिर ध्रुव वर्ण *sftp_libssh2_strerror(unsigned long err)
{
  विकल्प(err) {
  पक्ष LIBSSH2_FX_NO_SUCH_FILE:
    पटिनिवत्त "No such file or directory";

  पक्ष LIBSSH2_FX_PERMISSION_DENIED:
    पटिनिवत्त "Permission denied";

  पक्ष LIBSSH2_FX_FAILURE:
    पटिनिवत्त "Operation failed";

  पक्ष LIBSSH2_FX_BAD_MESSAGE:
    पटिनिवत्त "Bad message from SFTP server";

  पक्ष LIBSSH2_FX_NO_CONNECTION:
    पटिनिवत्त "Not connected to SFTP server";

  पक्ष LIBSSH2_FX_CONNECTION_LOST:
    पटिनिवत्त "Connection to SFTP server lost";

  पक्ष LIBSSH2_FX_OP_UNSUPPORTED:
    पटिनिवत्त "Operation not supported by SFTP server";

  पक्ष LIBSSH2_FX_INVALID_HANDLE:
    पटिनिवत्त "Invalid handle";

  पक्ष LIBSSH2_FX_NO_SUCH_PATH:
    पटिनिवत्त "No such file or directory";

  पक्ष LIBSSH2_FX_FILE_ALREADY_EXISTS:
    पटिनिवत्त "File already exists";

  पक्ष LIBSSH2_FX_WRITE_PROTECT:
    पटिनिवत्त "File is write protected";

  पक्ष LIBSSH2_FX_NO_MEDIA:
    पटिनिवत्त "No media";

  पक्ष LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    पटिनिवत्त "Disk full";

  पक्ष LIBSSH2_FX_QUOTA_EXCEEDED:
    पटिनिवत्त "User quota exceeded";

  पक्ष LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    पटिनिवत्त "Unknown principal";

  पक्ष LIBSSH2_FX_LOCK_CONFLICT:
    पटिनिवत्त "File lock conflict";

  पक्ष LIBSSH2_FX_DIR_NOT_EMPTY:
    पटिनिवत्त "Directory not empty";

  पक्ष LIBSSH2_FX_NOT_A_DIRECTORY:
    पटिनिवत्त "Not a directory";

  पक्ष LIBSSH2_FX_INVALID_FILENAME:
    पटिनिवत्त "Invalid filename";

  पक्ष LIBSSH2_FX_LINK_LOOP:
    पटिनिवत्त "Link points to itself";
  }
  पटिनिवत्त "Unknown error in libssh2";
}

स्थिर शून्य kbd_callback(ध्रुव वर्ण *name, पूर्णाङ्क name_len,
                         ध्रुव वर्ण *instruction, पूर्णाङ्क instruction_len,
                         पूर्णाङ्क अङ्कः_prompts,
                         ध्रुव LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         शून्य **abstract)
{
  संरचना Curl_easy *data = (संरचना Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", अङ्कः_prompts);
#अञ्ञथा
  (शून्य)name;
  (शून्य)name_len;
  (शून्य)instruction;
  (शून्य)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  सचे(अङ्कः_prompts == 1) {
    संरचना connectdata *conn = data->conn;
    ध्रुव वर्ण *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (शून्य)prompts;
} /* kbd_callback */

स्थिर CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  विकल्प(err) {
  पक्ष LIBSSH2_FX_OK:
    पटिनिवत्त CURLE_OK;

  पक्ष LIBSSH2_FX_NO_SUCH_FILE:
  पक्ष LIBSSH2_FX_NO_SUCH_PATH:
    पटिनिवत्त CURLE_REMOTE_FILE_NOT_FOUND;

  पक्ष LIBSSH2_FX_PERMISSION_DENIED:
  पक्ष LIBSSH2_FX_WRITE_PROTECT:
  पक्ष LIBSSH2_FX_LOCK_CONFLICT:
    पटिनिवत्त CURLE_REMOTE_ACCESS_DENIED;

  पक्ष LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  पक्ष LIBSSH2_FX_QUOTA_EXCEEDED:
    पटिनिवत्त CURLE_REMOTE_DISK_FULL;

  पक्ष LIBSSH2_FX_FILE_ALREADY_EXISTS:
    पटिनिवत्त CURLE_REMOTE_FILE_EXISTS;

  पक्ष LIBSSH2_FX_DIR_NOT_EMPTY:
    पटिनिवत्त CURLE_QUOTE_ERROR;

  स्वभाव:
    भङ्ग;
  }

  पटिनिवत्त CURLE_SSH;
}

स्थिर CURLcode libssh2_session_error_to_CURLE(पूर्णाङ्क err)
{
  विकल्प(err) {
  /* Ordered by order of appearance in libssh2.h */
  पक्ष LIBSSH2_ERROR_NONE:
    पटिनिवत्त CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  पक्ष LIBSSH2_ERROR_SCP_PROTOCOL:
    पटिनिवत्त CURLE_REMOTE_FILE_NOT_FOUND;

  पक्ष LIBSSH2_ERROR_SOCKET_NONE:
    पटिनिवत्त CURLE_COULDNT_CONNECT;

  पक्ष LIBSSH2_ERROR_ALLOC:
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  पक्ष LIBSSH2_ERROR_SOCKET_SEND:
    पटिनिवत्त CURLE_SEND_ERROR;

  पक्ष LIBSSH2_ERROR_HOSTKEY_INIT:
  पक्ष LIBSSH2_ERROR_HOSTKEY_SIGN:
  पक्ष LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  पक्ष LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    पटिनिवत्त CURLE_PEER_FAILED_VERIFICATION;

  पक्ष LIBSSH2_ERROR_PASSWORD_EXPIRED:
    पटिनिवत्त CURLE_LOGIN_DENIED;

  पक्ष LIBSSH2_ERROR_SOCKET_TIMEOUT:
  पक्ष LIBSSH2_ERROR_TIMEOUT:
    पटिनिवत्त CURLE_OPERATION_TIMEDOUT;

  पक्ष LIBSSH2_ERROR_EAGAIN:
    पटिनिवत्त CURLE_AGAIN;
  }

  पटिनिवत्त CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

स्थिर LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (शून्य)abstract;
  पटिनिवत्त Curl_cmalloc(गणनम्);
}

स्थिर LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (शून्य)abstract;
  पटिनिवत्त Curl_crealloc(ptr, गणनम्);
}

स्थिर LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (शून्य)abstract;
  सचे(ptr) /* ssh2 agent sometimes call free with null ptr */
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
  पटिनिवत्त (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
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
  पटिनिवत्त keytype;
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
  size_t keylen = 0;
  पूर्णाङ्क rc = 0;
  CURLcode result = CURLE_OK;

  सचे(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
    पटिनिवत्त CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  सचे(remotekey) {
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
      keybit = LIBSSH2_KNOWNHOST_कुञ्चिका_SSHRSA;
      भङ्ग;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    पक्ष LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_कुञ्चिका_SSHDSS;
      भङ्ग;
#endif
    पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_कुञ्चिका_ECDSA_256;
      भङ्ग;
    पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_कुञ्चिका_ECDSA_384;
      भङ्ग;
    पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_कुञ्चिका_ECDSA_521;
      भङ्ग;
    पक्ष LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_कुञ्चिका_ED25519;
      भङ्ग;
    स्वभाव:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      भङ्ग;
    }
    सचे(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    अञ्ञथा {
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
            host->कुञ्चिका : "<none>");

      /* setup 'knownkey' */
      सचे(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.कुञ्चिका = host->कुञ्चिका;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.कुञ्चिका = remotekey;
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
  अञ्ञथा {
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
    सचे(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  पक्ष CURLKHSTAT_FINE:
  पक्ष CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    सचे(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      पूर्णाङ्क addrc;
      ध्रुव वर्ण *hostbuf;
      वर्ण *hostport = रिक्त;
      सचे(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        सचे(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
      }
      अञ्ञथा
        hostbuf = conn->origin->hostname;
      सचे(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, रिक्त,
                                       remotekey, keylen, रिक्त, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, रिक्त);
        सचे(addrc)
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        अञ्ञथा सचे(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          पूर्णाङ्क wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          सचे(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    अञ्ञथा
      infof(data, "SSH: knownhost entry matches host key");
    भङ्ग;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode ssh_check_fingerprint(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  ध्रुव वर्ण *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुञ्चिका_MD5);
  ध्रुव वर्ण *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुञ्चिका_SHA256);

  सचे(pubkey_sha256) {
    ध्रुव वर्ण *fingerprint = रिक्त;
    वर्ण *fingerprint_b64 = रिक्त;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    सचे(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      पटिनिवत्त CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    सचे(curlx_base64_encode((ध्रुव uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      पटिनिवत्त CURLE_PEER_FAILED_VERIFICATION;
    }

    सचे(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      पटिनिवत्त CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    याव((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    याव((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    सचे((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      पटिनिवत्त CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  सचे(pubkey_md5) {
    वर्ण md5buffer[33];
    ध्रुव वर्ण *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    सचे(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      पूर्णाङ्क i;
      अत्थाय(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned वर्ण)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    सचे(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      सचे(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      अञ्ञथा {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      पटिनिवत्त CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  सचे(!pubkey_md5 && !pubkey_sha256) {
    सचे(data->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      पूर्णाङ्क sshkeytype = 0;
      पूर्णाङ्क rc = 0;
      /* we handle the process to the callback */
      ध्रुव वर्ण *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      सचे(remotekey) {
        संरचना Curl_mapi_guard guard;
        गणना curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (पूर्णाङ्क)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        सचे(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          पटिनिवत्त CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      अञ्ञथा {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        पटिनिवत्त CURLE_PEER_FAILED_VERIFICATION;
      }
      पटिनिवत्त CURLE_OK;
    }
    अञ्ञथा {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      पटिनिवत्त ssh_knownhost(data, sshc);
    }
  }
  अञ्ञथा {
    /* as we already matched, we skip the check for known hosts */
    पटिनिवत्त CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
स्थिर CURLcode ssh_force_knownhost_कुञ्चिका_type(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  स्थिर ध्रुव वर्ण hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  स्थिर ध्रुव वर्ण hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  स्थिर ध्रुव वर्ण hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  स्थिर ध्रुव वर्ण hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  स्थिर ध्रुव वर्ण hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_कुञ्चिका_SSHDSS
  स्थिर ध्रुव वर्ण hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  सचे(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुञ्चिका_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_कुञ्चिका_SHA256)) {
    संरचना libssh2_knownhost *store = रिक्त;
    संरचना connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_कुञ्चिका_MASK >>
                      LIBSSH2_KNOWNHOST_कुञ्चिका_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    याव(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      सचे(store) {
        सचे(store->name) {
          सचे(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_size = 0;
            ध्रुव वर्ण *p;
            ध्रुव वर्ण *kh_name_end = strstr(store->name, "]:");
            सचे(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              अनुवर्त;
            }
            p = kh_name_end + 2; /* start of port number */
            सचे(!curlx_str_सङ्ख्या(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = strlen(store->name) - 1 - strlen(kh_name_end);
              सचे((kh_name_size == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                भङ्ग;
              }
            }
          }
          अञ्ञथा सचे(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            भङ्ग;
          }
        }
        अञ्ञथा सचे(store->कुञ्चिका) {
          unsigned पूर्णाङ्क type = (store->typemask & LIBSSH2_KNOWNHOST_कुञ्चिका_MASK)
                              >> LIBSSH2_KNOWNHOST_कुञ्चिका_SHIFT;
          सचे(!absent_type[type]) {
            पूर्णाङ्क keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->कुञ्चिका, strlen(store->कुञ्चिका),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_कुञ्चिका_MASK),
              रिक्त);
            सचे(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              भङ्ग;
            }
            अञ्ञथा सचे(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    सचे(found) {
      पूर्णाङ्क rc;
      ध्रुव वर्ण *hostkey_method = रिक्त;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      विकल्प(store->typemask & LIBSSH2_KNOWNHOST_कुञ्चिका_MASK) {
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        भङ्ग;
#ifdef LIBSSH2_KNOWNHOST_कुञ्चिका_SSHDSS
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        भङ्ग;
#endif
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        पटिनिवत्त CURLE_SSH;
      स्वभाव:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_कुञ्चिका_MASK));
        पटिनिवत्त CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      सचे(rc) {
        वर्ण *err_सन्देशः = रिक्त;
        पूर्णाङ्क errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_सन्देशः);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    अञ्ञथा {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  पटिनिवत्त result;
}

स्थिर CURLcode quote_error(संरचना Curl_easy *data,
                            संरचना ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  पटिनिवत्त CURLE_QUOTE_ERROR;
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

  सचे(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  सचे(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    वर्ण *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    सचे(!tmp)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    सचे(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    पटिनिवत्त result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  सचे(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    पटिनिवत्त CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  सचे(result) {
    सचे(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    पटिनिवत्त result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  सचे(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    सचे(result) {
      सचे(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      पटिनिवत्त result;
    }
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);

    memset(&sshp->quote_attrs, 0, परिमाण(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    पटिनिवत्त result;
  }
  सचे(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    सचे(result) {
      सचे(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      पटिनिवत्त result;
    }
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    पटिनिवत्त result;
  }
  अञ्ञथा सचे(!strncmp(cmd, "mkdir ", 6)) {
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    पटिनिवत्त result;
  }
  अञ्ञथा सचे(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    सचे(result) {
      सचे(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      पटिनिवत्त result;
    }
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    पटिनिवत्त result;
  }
  अञ्ञथा सचे(!strncmp(cmd, "rmdir ", 6)) {
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    पटिनिवत्त result;
  }
  अञ्ञथा सचे(!strncmp(cmd, "rm ", 3)) {
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    पटिनिवत्त result;
  }
  अञ्ञथा सचे(!strncmp(cmd, "statvfs ", 8)) {
    सचे(*cp)
      पटिनिवत्त quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    पटिनिवत्त result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  पटिनिवत्त CURLE_QUOTE_ERROR;
}

स्थिर CURLcode sftp_upload_init(संरचना Curl_easy *data,
                                 संरचना ssh_conn *sshc,
                                 संरचना SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  सचे(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    सचे(data->state.resume_from < 0) {
      पूर्णाङ्क rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      सचे(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        पटिनिवत्त CURLE_OK;
      }
      सचे(rc) {
        data->state.resume_from = 0;
      }
      अञ्ञथा {
        curl_off_t size = attrs.filesize;
        सचे(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          पटिनिवत्त CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  सचे(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  अञ्ञथा सचे(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  अञ्ञथा {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  सचे(!sshc->sftp_handle) {
    CURLcode result;
    unsigned long sftperr;
    पूर्णाङ्क rc = libssh2_session_last_errno(sshc->ssh_session);

    सचे(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      पटिनिवत्त CURLE_OK;
    }

    सचे(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    अञ्ञथा
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    सचे(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      पटिनिवत्त sftp_libssh2_error_to_CURLE(sftperr);
    }
    सचे((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      पटिनिवत्त CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    सचे(!result) {
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
    पटिनिवत्त result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  सचे(data->state.resume_from > 0 && !data->set.remote_append) {
    पूर्णाङ्क seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    सचे(data->set.seek_func) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    सचे(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      सचे(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        पटिनिवत्त CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      करोहि {
        संरचना Curl_mapi_guard guard;
        वर्ण scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)परिमाण(scratch)) ?
          परिमाण(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        सचे((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          पटिनिवत्त CURLE_FTP_COULDNT_USE_REST;
        }
      } याव(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    सचे(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  सचे(data->state.infilesize > 0) {
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
  पटिनिवत्त CURLE_OK;
}

स्थिर शून्य ssh_state_pkey_init(संरचना Curl_easy *data, संरचना ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  सचे((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    सचे(sshc->pub_कुञ्चिका)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_कुञ्चिका);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_कुञ्चिका);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  अञ्ञथा
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

  सचे(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  सचे(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    पूर्णाङ्क rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    सचे(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      पटिनिवत्त CURLE_OK;
    }
    सचे(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      गच्छ fail;
    }
  }

  /* Now set the new attributes... */
  सचे(!strncmp(cmd, "chgrp", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t gid;
    सचे(!curlx_str_सङ्ख्या(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    अञ्ञथा सचे(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      गच्छ fail;
    }
  }
  अञ्ञथा सचे(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ध्रुव वर्ण *p = sshc->quote_path1;
    /* permissions are octal */
    सचे(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      गच्छ fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  अञ्ञथा सचे(!strncmp(cmd, "chown", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t uid;
    सचे(!curlx_str_सङ्ख्या(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    अञ्ञथा सचे(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      गच्छ fail;
    }
  }
  अञ्ञथा सचे(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    सचे(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#सचे SIZEOF_TIME_T > SIZEOF_LONG
    सचे(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    सचे(fail)
      गच्छ fail;
    सचे(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    अञ्ञथा /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  पटिनिवत्त CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  पटिनिवत्त CURLE_QUOTE_ERROR;
}

स्थिर CURLcode sftp_download_stat(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc,
                                   संरचना SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  पूर्णाङ्क rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  सचे(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    पटिनिवत्त CURLE_OK;
  }
  सचे(rc ||
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
  अञ्ञथा {
    curl_off_t size = attrs.filesize;

    सचे(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      पटिनिवत्त CURLE_BAD_DOWNLOAD_RESUME;
    }
    सचे(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      सचे(result)
        पटिनिवत्त result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  सचे(data->state.resume_from) {
    सचे(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      सचे((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        पटिनिवत्त CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    अञ्ञथा {
      सचे((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        पटिनिवत्त CURLE_BAD_DOWNLOAD_RESUME;
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
  सचे(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    पटिनिवत्त CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  पटिनिवत्त CURLE_OK;
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
  सचे(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    पटिनिवत्त result;
  }
  सचे(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    सचे(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      सचे(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      सचे(result)
        पटिनिवत्त result;
    }
    अञ्ञथा {
      result = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      सचे(!result) {
        सचे((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        अञ्ञथा {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      पटिनिवत्त result;
    }
  }
  अञ्ञथा सचे(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  अञ्ञथा {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  पटिनिवत्त result;
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

  result = ssh_force_knownhost_कुञ्चिका_type(data, sshc);
  सचे(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  अञ्ञथा
    myssh_to(data, sshc, SSH_S_STARTUP);
  पटिनिवत्त result;
}

स्थिर CURLcode ssh_state_startup(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  पूर्णाङ्क rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc) {
    वर्ण *err_सन्देशः = रिक्त;
    (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_सन्देशः);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  पटिनिवत्त CURLE_OK;
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
  सचे(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  पटिनिवत्त result;
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
                                         user, curlx_uztoui(strlen(user)));

  सचे(!sshc->authlist) {
    पूर्णाङ्क rc;
    सचे(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      पटिनिवत्त CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    पटिनिवत्त libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_pkey(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  पूर्णाङ्क rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_कुञ्चिका, sshc->priv_कुञ्चिका, sshc->passphrase ? sshc->passphrase : "");
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  अञ्ञथा {
    वर्ण *err_सन्देशः = रिक्त;
    वर्ण unknown[] = "Reason unknown (-1)";
    सचे(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_सन्देशः = unknown;
    }
    अञ्ञथा {
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_सन्देशः);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_pass_init(संरचना Curl_easy *data,
                                         संरचना ssh_conn *sshc)
{
  सचे((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_pass(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  ध्रुव वर्ण *passwd = Curl_creds_passwd(conn->creds);
  पूर्णाङ्क rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 रिक्त);
  सचे(rc == LIBSSH2_ERROR_EAGAIN) {
    पटिनिवत्त CURLE_AGAIN;
  }
  सचे(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_host_init(संरचना Curl_easy *data,
                                         संरचना ssh_conn *sshc)
{
  सचे((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_agent_init(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = 0;
  सचे((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    सचे(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      सचे(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_कुञ्चिका_INIT);
        पटिनिवत्त CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;
    सचे(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_कुञ्चिका_INIT);
    }
    अञ्ञथा {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  अञ्ञथा
    myssh_to(data, sshc, SSH_AUTH_कुञ्चिका_INIT);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_agent_list(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_agent_list_identities(sshc->ssh_agent);

  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;
  सचे(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_कुञ्चिका_INIT);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = रिक्त;
  }
  पटिनिवत्त CURLE_OK;
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
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    सचे(rc < 0) {
      सचे(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        पटिनिवत्त CURLE_OK;
      }
      पटिनिवत्त CURLE_AGAIN;
    }
  }

  सचे(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  अञ्ञथा सचे(rc == 1)
    infof(data, "SSH: no agent identity would match");

  सचे(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_AUTH_कुञ्चिका_INIT);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_कुञ्चिका_init(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc)
{
  सचे((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_कुञ्चिका);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_कुञ्चिका(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  संरचना connectdata *conn = data->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  पूर्णाङ्क rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    पटिनिवत्त CURLE_OK;
  }
  पटिनिवत्त CURLE_LOGIN_DENIED;
}

स्थिर CURLcode ssh_state_auth_done(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  सचे(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    पटिनिवत्त CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  सचे(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    पटिनिवत्त CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_init(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  सचे(!sshc->sftp_session) {
    वर्ण *err_सन्देशः = रिक्त;
    सचे(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    failf(data, "Failure initializing sftp session: %s", err_सन्देशः);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    पटिनिवत्त CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_realpath(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc,
                                        संरचना SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  पूर्णाङ्क rc;

  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  सचे(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    सचे(!sshc->homedir)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    सचे(!data->state.most_recent_ftp_entrypath)
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }
  अञ्ञथा {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    सचे(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    अञ्ञथा
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (पूर्णाङ्क)result);
    पटिनिवत्त result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_init(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  CURLcode result;

  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  सचे(result) {
    myssh_to(data, sshc, SSH_STOP);
    पटिनिवत्त result;
  }

  सचे(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_postquote_init(संरचना Curl_easy *data,
                                              संरचना ssh_conn *sshc)
{
  सचे(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अञ्ञथा {
    myssh_to(data, sshc, SSH_STOP);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc,
                                     संरचना SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  सचे(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode ssh_state_sftp_next_quote(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  सचे(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अञ्ञथा {
    सचे(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    अञ्ञथा {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_stat(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  सचे(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode ssh_state_sftp_quote_setstat(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc,
                                             संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क rc;

  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    पटिनिवत्त CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_symlink(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    पटिनिवत्त CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_mkdir(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)data->set.new_directory_perms);
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    पटिनिवत्त CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_rename(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    पटिनिवत्त CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_rmdir(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    पटिनिवत्त CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_unlink(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    पटिनिवत्त CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_statvfs(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  पूर्णाङ्क rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    पटिनिवत्त CURLE_QUOTE_ERROR;
  }
  अञ्ञथा सचे(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#अञ्ञथा
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
    सचे(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    सचे(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      पटिनिवत्त result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_create_dirs_mkdir(संरचना Curl_easy *data,
                                                 संरचना ssh_conn *sshc,
                                                 संरचना SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  पूर्णाङ्क rc;

  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)data->set.new_directory_perms);
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  सचे(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    सचे((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      पटिनिवत्त sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_readdir_init(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc,
                                            संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  सचे(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    पटिनिवत्त CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  सचे(!sshc->sftp_handle) {
    unsigned long sftperr;
    सचे(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    पटिनिवत्त sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_readdir_link(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc,
                                            संरचना SSHPROTO *sshp)
{
  CURLcode result;
  पूर्णाङ्क rc;
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned पूर्णाङ्क)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  सचे(rc < 0)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  सचे(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  अञ्ञथा
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  पटिनिवत्त result;
}

स्थिर CURLcode ssh_state_scp_download_init(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc,
                                            संरचना SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, परिमाण(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  सचे(!sshc->ssh_channel) {
    पूर्णाङ्क ssh_err;
    वर्ण *err_सन्देशः = रिक्त;

    सचे(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    failf(data, "%s", err_सन्देशः);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    पटिनिवत्त libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_close(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc,
                                     संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क rc = 0;
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  सचे(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    सचे(rc < 0) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_सन्देशः);
    }
    sshc->sftp_handle = रिक्त;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  सचे(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  अञ्ञथा
    myssh_to(data, sshc, SSH_STOP);

  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_shutdown(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  पूर्णाङ्क rc = 0;
  सचे(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    सचे(rc < 0) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_सन्देशः);
    }
    sshc->sftp_handle = रिक्त;
  }
  सचे(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    सचे(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = रिक्त;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_download_init(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc,
                                             संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  सचे(!sshc->sftp_handle) {
    unsigned long sftperr;
    सचे(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      पटिनिवत्त CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    पटिनिवत्त sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_upload_init(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
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
  सचे(!sshc->ssh_channel) {
    पूर्णाङ्क ssh_err;
    वर्ण *err_सन्देशः = रिक्त;
    CURLcode result;
    सचे(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    failf(data, "%s", err_सन्देशः);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    सचे(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    पटिनिवत्त result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_session_disconnect(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  पूर्णाङ्क rc = 0;
  सचे(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    सचे(rc < 0) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_सन्देशः);
    }
    sshc->ssh_channel = रिक्त;
  }

  सचे(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;

    सचे(rc < 0) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_सन्देशः);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode sshc_cleanup(संरचना ssh_conn *sshc, संरचना Curl_easy *data,
                             bool block)
{
  पूर्णाङ्क rc;

  सचे(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = रिक्त;
  }

  सचे(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    सचे((rc < 0) && data) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_सन्देशः);
    }
    सचे(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      पटिनिवत्त CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = रिक्त;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = रिक्त;
    sshc->sshagent_prev_identity = रिक्त;
  }

  सचे(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    सचे((rc < 0) && data) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_सन्देशः);
    }
    सचे(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      पटिनिवत्त CURLE_AGAIN;

    sshc->sftp_handle = रिक्त;
  }

  सचे(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    सचे((rc < 0) && data) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_सन्देशः);
    }
    सचे(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      पटिनिवत्त CURLE_AGAIN;

    sshc->ssh_channel = रिक्त;
  }

  सचे(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    सचे((rc < 0) && data) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_सन्देशः);
    }
    सचे(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      पटिनिवत्त CURLE_AGAIN;

    sshc->sftp_session = रिक्त;
  }

  सचे(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    सचे((rc < 0) && data) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_सन्देशः);
    }
    सचे(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      पटिनिवत्त CURLE_AGAIN;

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
  curlx_safefree(sshc->pub_कुञ्चिका);
  curlx_safefree(sshc->priv_कुञ्चिका);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_getinfo(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  सचे(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  अञ्ञथा
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_filetime(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc,
                                        संरचना SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  पूर्णाङ्क rc;

  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  सचे(rc == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  सचे(rc == 0)
    data->info.filetime = (time_t)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_trans_init(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  सचे(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  अञ्ञथा सचे(sshp) {
    size_t plen = strlen(sshp->path);
    सचे(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    अञ्ञथा
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  अञ्ञथा
    पटिनिवत्त CURLE_FAILED_INIT;
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_upload_init(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc,
                                           संरचना SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  सचे(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode ssh_state_sftp_create_dirs_init(संरचना Curl_easy *data,
                                                संरचना ssh_conn *sshc,
                                                संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  सचे(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  अञ्ञथा
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_create_dirs(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc,
                                           संरचना SSHPROTO *sshp)
{
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  सचे(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    पटिनिवत्त CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_readdir(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc,
                                       संरचना SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  सचे(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  पटिनिवत्त result;
}

स्थिर CURLcode ssh_state_sftp_readdir_bottom(संरचना Curl_easy *data,
                                              संरचना ssh_conn *sshc,
                                              संरचना SSHPROTO *sshp)
{
  CURLcode result;
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  सचे(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  सचे(result) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(data, sshc, SSH_STOP);
  }
  अञ्ञथा {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  पटिनिवत्त result;
}

स्थिर CURLcode ssh_state_sftp_readdir_done(संरचना Curl_easy *data,
                                            संरचना ssh_conn *sshc)
{
  सचे(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;

  sshc->sftp_handle = रिक्त;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_download_stat(संरचना Curl_easy *data,
                                             संरचना ssh_conn *sshc,
                                             संरचना SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  सचे(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  पटिनिवत्त result;
}

स्थिर CURLcode ssh_state_scp_trans_init(संरचना Curl_easy *data,
                                         संरचना ssh_conn *sshc,
                                         संरचना SSHPROTO *sshp)
{
  CURLcode result;
  सचे(!sshp)
    पटिनिवत्त CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  सचे(result) {
    myssh_to(data, sshc, SSH_STOP);
    पटिनिवत्त result;
  }

  सचे(data->state.upload) {
    सचे(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      पटिनिवत्त CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  अञ्ञथा
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_done(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc)
{
  सचे(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  अञ्ञथा
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_send_eof(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  सचे(sshc->ssh_channel) {
    पूर्णाङ्क rc = libssh2_channel_send_eof(sshc->ssh_channel);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;
    सचे(rc) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session,
                                       &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_सन्देशः);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_wait_eof(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  सचे(sshc->ssh_channel) {
    पूर्णाङ्क rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;
    सचे(rc) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session,
                                       &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_सन्देशः);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_wait_close(संरचना Curl_easy *data,
                                         संरचना ssh_conn *sshc)
{
  सचे(sshc->ssh_channel) {
    पूर्णाङ्क rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;
    सचे(rc) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session,
                                       &err_सन्देशः, रिक्त, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_सन्देशः);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_channel_free(
  संरचना Curl_easy *data,
  संरचना ssh_conn *sshc)
{
  सचे(sshc->ssh_channel) {
    पूर्णाङ्क rc = libssh2_channel_free(sshc->ssh_channel);
    सचे(rc == LIBSSH2_ERROR_EAGAIN)
      पटिनिवत्त CURLE_AGAIN;
    सचे(rc < 0) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_last_error(sshc->ssh_session,
                                       &err_सन्देशः, रिक्त, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_सन्देशः);
    }
    sshc->ssh_channel = रिक्त;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssh_state_session_free(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  संरचना connectdata *conn = data->conn;
  सचे(result)
    पटिनिवत्त result;
  memset(sshc, 0, परिमाण(संरचना ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  पटिनिवत्त CURLE_OK;
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

  करोहि {
    विकल्प(sshc->state) {
    पक्ष SSH_INIT:
      result = ssh_state_init(data, sshc);
      सचे(result)
        भङ्ग;
      FALLTHROUGH();

    पक्ष SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      सचे(result)
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

    पक्ष SSH_AUTH_कुञ्चिका_INIT:
      result = ssh_state_auth_कुञ्चिका_init(data, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_कुञ्चिका:
      result = ssh_state_auth_कुञ्चिका(data, sshc);
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

  } याव(!result && (sshc->state != SSH_STOP) && !*block);

  सचे(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (पूर्णाङ्क)result, *block);

  पटिनिवत्त result;
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
  सचे(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    सचे(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  सचे(!dir)
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
  सचे(!sshc || !sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  करोहि {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } याव(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  पटिनिवत्त result;
}

स्थिर CURLcode ssh_block_statemach(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  संरचना curltime start = *Curl_pgrs_now(data);

  याव((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t वामः_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    सचे(result)
      भङ्ग;

    सचे(!disconnect) {
      result = Curl_pgrsCheck(data);
      सचे(result)
        भङ्ग;

      वामः_ms = Curl_timeleft_ms(data);
      सचे(वामः_ms < 0) {
        failf(data, "Operation timed out");
        पटिनिवत्त CURLE_OPERATION_TIMEDOUT;
      }
    }
    अञ्ञथा सचे(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      भङ्ग;
    }

    सचे(block) {
      पूर्णाङ्क dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      सचे(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      सचे(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (शून्य)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!वामः_ms || वामः_ms > 1000) ? 1000 : वामः_ms);
    }
  }

  पटिनिवत्त result;
}

स्थिर शून्य myssh_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना SSHPROTO *sshp = entry;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

स्थिर शून्य myssh_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *entry)
{
  संरचना ssh_conn *sshc = entry;
  (शून्य)कुञ्चिका;
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
  सचे(!sshc)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  सचे(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, परिमाण(*sshp));
  सचे(!sshp)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  सचे(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  पटिनिवत्त Curl_ssh_setup_pkey(data, sshc);
}

स्थिर Curl_recv scp_recv, sftp_recv;
स्थिर Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
स्थिर ssize_t ssh_tls_recv(libssh2_socket_t sock, शून्य *buffer,
                            size_t length, पूर्णाङ्क flags, शून्य **abstract)
{
  संरचना Curl_easy *data = (संरचना Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nread;
  CURLcode result;
  संरचना connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (शून्य)flags;

  सचे(!sshc)
    पटिनिवत्त -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  सचे(result == CURLE_AGAIN)
    पटिनिवत्त -EAGAIN; /* magic return code for libssh2 */
  अञ्ञथा सचे(result)
    पटिनिवत्त -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (ध्रुव वर्ण *)buffer, nread);
  पटिनिवत्त (ssize_t)nread;
}

स्थिर ssize_t ssh_tls_send(libssh2_socket_t sock, ध्रुव शून्य *buffer,
                            size_t length, पूर्णाङ्क flags, शून्य **abstract)
{
  संरचना Curl_easy *data = (संरचना Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nwrite;
  CURLcode result;
  संरचना connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (शून्य)flags;

  सचे(!sshc)
    पटिनिवत्त -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  सचे(result == CURLE_AGAIN)
    पटिनिवत्त -EAGAIN; /* magic return code for libssh2 */
  अञ्ञथा सचे(result)
    पटिनिवत्त -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (ध्रुव वर्ण *)buffer, nwrite);
  पटिनिवत्त (ssize_t)nwrite;
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

#सचे LIBSSH2_VERSION_अङ्कः >= 0x010b00
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
    सचे(crypto_str)
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

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

  सचे(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    पटिनिवत्त CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#सचे LIBSSH2_VERSION_अङ्कः >= 0x010B00
  सचे(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  सचे(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#सचे LIBSSH2_VERSION_अङ्कः >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#सचे defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic प्रक्षिप
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#सचे defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic निष्कासय
#endif
#अञ्ञथा
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    सङ्घ receive {
      शून्य *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, शून्य *, size_t, पूर्णाङ्क, शून्य **);
    };
    सङ्घ transfer {
      शून्य *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ध्रुव शून्य *, size_t, पूर्णाङ्क, शून्य **);
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
  सचे(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  अञ्ञथा {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  सचे(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  सचे(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    पूर्णाङ्क rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    सचे(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = रिक्त;
      पटिनिवत्त CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    सचे(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (पूर्णाङ्क)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  पटिनिवत्त result;
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
  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  सचे(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  पटिनिवत्त result;
}

/* called from multi.c while DOing */
स्थिर CURLcode scp_doing(संरचना Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  सचे(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  पटिनिवत्त result;
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

  सचे(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  सचे(sshc)
    पटिनिवत्त sshc_cleanup(sshc, data, TRUE);
  पटिनिवत्त result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
स्थिर CURLcode ssh_done(संरचना Curl_easy *data, CURLcode status)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  सचे(!sshc || !sshp)
    पटिनिवत्त CURLE_FAILED_INIT;

  सचे(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  अञ्ञथा
    result = status;

  सचे(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    पटिनिवत्त CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  पटिनिवत्त result;
}

स्थिर CURLcode scp_done(संरचना Curl_easy *data, CURLcode status,
                         bool premature)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (शून्य)premature;

  सचे(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  पटिनिवत्त ssh_done(data, status);
}

स्थिर CURLcode scp_send(संरचना Curl_easy *data, int8_t sockindex,
                         ध्रुव uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  (शून्य)eos;
  *pnwritten = 0;

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ध्रुव वर्ण *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  सचे(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  अञ्ञथा सचे(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((पूर्णाङ्क)nwritten);
  अञ्ञथा
    *pnwritten = (size_t)nwritten;

  पटिनिवत्त result;
}

स्थिर CURLcode scp_recv(संरचना Curl_easy *data, int8_t sockindex,
                         वर्ण *mem, size_t len, size_t *pnread)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  सचे(nread == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;
  अञ्ञथा सचे(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((पूर्णाङ्क)nread);
  अञ्ञथा
    *pnread = (size_t)nread;

  पटिनिवत्त result;
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
  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  सचे(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  पटिनिवत्त result;
}

/* called from multi.c while DOing */
स्थिर CURLcode sftp_doing(संरचना Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  सचे(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  पटिनिवत्त result;
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

  सचे(sshc) {
    सचे(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (पूर्णाङ्क)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  पटिनिवत्त result;
}

स्थिर CURLcode sftp_done(संरचना Curl_easy *data, CURLcode status,
                          bool premature)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  सचे(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    सचे(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  पटिनिवत्त ssh_done(data, status);
}

/* return number of sent bytes */
स्थिर CURLcode sftp_send(संरचना Curl_easy *data, int8_t sockindex,
                          ध्रुव uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (शून्य)sockindex;
  (शून्य)eos;
  *pnwritten = 0;

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ध्रुव वर्ण *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  सचे(nwrite == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;
  अञ्ञथा सचे(nwrite < LIBSSH2_ERROR_NONE)
    पटिनिवत्त libssh2_session_error_to_CURLE((पूर्णाङ्क)nwrite);
  *pnwritten = (size_t)nwrite;
  पटिनिवत्त CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
स्थिर CURLcode sftp_recv(संरचना Curl_easy *data, int8_t sockindex,
                          वर्ण *mem, size_t len, size_t *pnread)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (शून्य)sockindex;
  *pnread = 0;

  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  सचे(nread == LIBSSH2_ERROR_EAGAIN)
    पटिनिवत्त CURLE_AGAIN;
  अञ्ञथा सचे(nread < 0)
    पटिनिवत्त libssh2_session_error_to_CURLE((पूर्णाङ्क)nread);

  *pnread = (size_t)nread;
  पटिनिवत्त CURLE_OK;
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
  सचे(!sshc)
    पटिनिवत्त CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  सचे(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  अञ्ञथा
    result = sftp_perform(data, &connected, done);

  पटिनिवत्त result;
}

CURLcode Curl_ssh_init(शून्य)
{
  सचे(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    पटिनिवत्त CURLE_FAILED_INIT;
  }
  पटिनिवत्त CURLE_OK;
}

शून्य Curl_ssh_cleanup(शून्य)
{
  libssh2_exit();
}

शून्य Curl_ssh_version(वर्ण *buffer, size_t buflen)
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
  सचे(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    सचे(sshc && sshc->ssh_session) {
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
