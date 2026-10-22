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

#यदि_परिभाषितम् USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#यदि_अन्तः

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

स्थिर ध्रुव वर्ण *sftp_libssh2_strerror(अचिह्नित दीर्घ err)
{
  विकल्प(err) {
  पक्ष LIBSSH2_FX_NO_SUCH_FILE:
    प्रत्यागम "No such file or directory";

  पक्ष LIBSSH2_FX_PERMISSION_DENIED:
    प्रत्यागम "Permission denied";

  पक्ष LIBSSH2_FX_FAILURE:
    प्रत्यागम "Operation failed";

  पक्ष LIBSSH2_FX_BAD_सन्देशः:
    प्रत्यागम "Bad message from SFTP server";

  पक्ष LIBSSH2_FX_NO_CONNECTION:
    प्रत्यागम "Not connected to SFTP server";

  पक्ष LIBSSH2_FX_CONNECTION_नष्टम्:
    प्रत्यागम "Connection to SFTP server lost";

  पक्ष LIBSSH2_FX_OP_UNSUPPORTED:
    प्रत्यागम "Operation not supported by SFTP server";

  पक्ष LIBSSH2_FX_INVALID_HANDLE:
    प्रत्यागम "Invalid handle";

  पक्ष LIBSSH2_FX_NO_SUCH_मार्गः:
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

  पक्ष LIBSSH2_FX_तालकम्_CONFLICT:
    प्रत्यागम "File lock conflict";

  पक्ष LIBSSH2_FX_DIR_NOT_रिक्तानि:
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
                         पूर्णाङ्क अङ्कः_prompts,
                         ध्रुव LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         शून्य **abstract)
{
  संरचना Curl_easy *दत्त = (संरचना Curl_easy *)*abstract;

#यदि_परिभाषितम् CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", अङ्कः_prompts);
#अन्यथा
  (शून्य)name;
  (शून्य)name_len;
  (शून्य)instruction;
  (शून्य)instruction_len;
#यदि_अन्तः /* CURL_LIBSSH2_DEBUG */
  यदि(अङ्कः_prompts == 1) {
    संरचना connectdata *conn = दत्त->conn;
    ध्रुव वर्ण *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].दीर्घता = responses[0].text ? curlx_uztoui(वाक्यदैर्घ्यम्(passwd)) : 0;
  }
  (शून्य)prompts;
} /* kbd_callback */

स्थिर CURLcode sftp_libssh2_error_गन्तव्यम्_CURLE(अचिह्नित दीर्घ err)
{
  विकल्प(err) {
  पक्ष LIBSSH2_FX_OK:
    प्रत्यागम CURLE_OK;

  पक्ष LIBSSH2_FX_NO_SUCH_FILE:
  पक्ष LIBSSH2_FX_NO_SUCH_मार्गः:
    प्रत्यागम CURLE_REMOTE_FILE_NOT_प्राप्तम्;

  पक्ष LIBSSH2_FX_PERMISSION_DENIED:
  पक्ष LIBSSH2_FX_WRITE_PROTECT:
  पक्ष LIBSSH2_FX_तालकम्_CONFLICT:
    प्रत्यागम CURLE_REMOTE_ACCESS_DENIED;

  पक्ष LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  पक्ष LIBSSH2_FX_QUOTA_EXCEEDED:
    प्रत्यागम CURLE_REMOTE_DISK_पूर्णम्;

  पक्ष LIBSSH2_FX_FILE_ALREADY_EXISTS:
    प्रत्यागम CURLE_REMOTE_FILE_EXISTS;

  पक्ष LIBSSH2_FX_DIR_NOT_रिक्तानि:
    प्रत्यागम CURLE_QUOTE_ERROR;

  स्वभाव:
    भङ्ग;
  }

  प्रत्यागम CURLE_SSH;
}

स्थिर CURLcode libssh2_session_error_गन्तव्यम्_CURLE(पूर्णाङ्क err)
{
  विकल्प(err) {
  /* Ordered by order of appearance in libssh2.h */
  पक्ष LIBSSH2_ERROR_NONE:
    प्रत्यागम CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  पक्ष LIBSSH2_ERROR_SCP_PROTOCOL:
    प्रत्यागम CURLE_REMOTE_FILE_NOT_प्राप्तम्;

  पक्ष LIBSSH2_ERROR_SOCKET_NONE:
    प्रत्यागम CURLE_COULDNT_CONNECT;

  पक्ष LIBSSH2_ERROR_ALLOC:
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

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
  प्रत्यागम Curl_cmalloc(गणनम्);
}

स्थिर LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (शून्य)abstract;
  प्रत्यागम Curl_crealloc(ptr, गणनम्);
}

स्थिर LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (शून्य)abstract;
  यदि(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

स्थिर पूर्णाङ्क sshkeycallback(CURL *easy,
                          ध्रुव संरचना curl_khkey *knownkey, /*ज्ञातम्*/
                          ध्रुव संरचना curl_khkey *foundkey, /* found */
                          गणना curl_khmatch मेलनम्,
                          शून्य *clientp)
{
  (शून्य)easy;
  (शून्य)knownkey;
  (शून्य)foundkey;
  (शून्य)clientp;

  /* we only allow perfect matches, and we reject everything else */
  प्रत्यागम (मेलनम् != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

स्थिर गणना curl_khtype convert_ssh2_keytype(पूर्णाङ्क sshkeytype)
{
  गणना curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  विकल्प(sshkeytype) {
  पक्ष LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    भङ्ग;
#यदि_परिभाषितम् LIBSSH2_HOSTKEY_TYPE_DSS
  पक्ष LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  पक्ष LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् LIBSSH2_HOSTKEY_TYPE_ED25519
  पक्ष LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    भङ्ग;
#यदि_अन्तः
  }
  प्रत्यागम keytype;
}

स्थिर CURLcode ssh_knownhost(संरचना Curl_easy *दत्त,
                              संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना libssh2_knownhost *host = रिक्त;
  ध्रुव वर्ण *remotekey = रिक्त;
  पूर्णाङ्क keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  पूर्णाङ्क keybit = 0;
  पूर्णाङ्क sshkeytype = 0;
  size_t keylen = 0;
  पूर्णाङ्क rc = 0;
  CURLcode परिणाम = CURLE_OK;

  यदि(!CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS)) {
    infof(दत्त, "SSH: no knownhosts file configured");
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
      दत्त->समुच्चयः.ssh_keyfunc ? दत्त->समुच्चयः.ssh_keyfunc : sshkeycallback;
    संरचना curl_khkey knownkey;
    संरचना curl_khkey *knownkeyp = रिक्त;
    संरचना curl_khkey foundkey;

    विकल्प(sshkeytype) {
    पक्ष LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_कुञ्चिका_SSHRSA;
      भङ्ग;
#यदि_परिभाषितम् LIBSSH2_HOSTKEY_TYPE_DSS
    पक्ष LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_कुञ्चिका_SSHDSS;
      भङ्ग;
#यदि_अन्तः
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
      infof(दत्त, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      भङ्ग;
    }
    यदि(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    अन्यथा {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->पत्तनम् != पत्तनम्_SSH) ?
                                          conn->origin->पत्तनम् : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(दत्त, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->कुञ्चिका : "<none>");

      /* setup 'knownkey' */
      यदि(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
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
      CURL_CBAPI_START(&guard, दत्त, easy_ssh_keyfunc);
      rc = func(दत्त, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, दत्त->समुच्चयः.ssh_keyfunc_userp);
      CURL_CBAPI_अन्तः(&guard);
    }
  }
  अन्यथा {
    /* no remotekey means failure! */
    infof(दत्त, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  विकल्प(rc) {
  स्वभाव: /* unknown return codes is the same as reject */
  पक्ष CURLKHSTAT_REJECT:
    infof(दत्त, "SSH: knownhost check failed");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  पक्ष CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    भङ्ग;
  पक्ष CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    यदि(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  पक्ष CURLKHSTAT_FINE:
  पक्ष CURLKHSTAT_FINE_सङ्कलय_गन्तव्यम्_FILE:
    /* proceed */
    यदि(keycheck != LIBSSH2_KNOWNHOST_CHECK_मेलनम्) {
      पूर्णाङ्क addrc;
      ध्रुव वर्ण *hostbuf;
      वर्ण *hostport = रिक्त;
      यदि(conn->origin->पत्तनम् != पत्तनम्_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->पत्तनम्);
        यदि(!hostbuf)
          infof(दत्त, "WARNING: failed allocating buffer for [host]:port");
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
          infof(दत्त, "WARNING: adding the known host %s failed", hostbuf);
        अन्यथा यदि(rc == CURLKHSTAT_FINE_सङ्कलय_गन्तव्यम्_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          पूर्णाङ्क wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          यदि(wrc) {
            infof(दत्त, "WARNING: writing %s failed",
                  CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    अन्यथा
      infof(दत्त, "SSH: knownhost entry matches host key");
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_check_fingerprint(संरचना Curl_easy *दत्त,
                                      संरचना ssh_conn *sshc)
{
  ध्रुव वर्ण *pubkey_md5 =
    CURL_EASY_STR(दत्त, STRING_SSH_HOST_लौकिकम्_कुञ्चिका_MD5);
  ध्रुव वर्ण *pubkey_sha256 =
    CURL_EASY_STR(दत्त, STRING_SSH_HOST_लौकिकम्_कुञ्चिका_SHA256);

  यदि(pubkey_sha256) {
    ध्रुव वर्ण *fingerprint = रिक्त;
    वर्ण *fingerprint_b64 = रिक्त;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(दत्त, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    यदि(!fingerprint) {
      failf(दत्त,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    यदि(curlx_base64_encode((ध्रुव uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }

    यदि(!fingerprint_b64) {
      failf(दत्त, "SHA256 fingerprint could not be encoded");
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(दत्त, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

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
       वाक्ये_तुलय(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(दत्त,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(दत्त, "SSH: SHA256 checksum match");
  }

  यदि(pubkey_md5) {
    वर्ण md5buffer[33];
    ध्रुव वर्ण *fingerprint;

    infof(दत्त, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    यदि(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      पूर्णाङ्क i;
      प्रति(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (अचिह्नित वर्ण)fingerprint[i]);
      }

      infof(दत्त, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    यदि(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      यदि(fingerprint) {
        failf(दत्त,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      अन्यथा {
        failf(दत्त,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(दत्त, "SSH: MD5 checksum match");
  }

  यदि(!pubkey_md5 && !pubkey_sha256) {
    यदि(दत्त->समुच्चयः.ssh_hostkeyfunc) {
      size_t keylen = 0;
      पूर्णाङ्क sshkeytype = 0;
      पूर्णाङ्क rc = 0;
      /* we handle the process to the callback */
      ध्रुव वर्ण *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      यदि(remotekey) {
        संरचना Curl_mapi_guard guard;
        गणना curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, दत्त, easy_ssh_hostkeyfunc);
        rc = दत्त->समुच्चयः.ssh_hostkeyfunc(दत्त->समुच्चयः.ssh_hostkeyfunc_userp,
                                       (पूर्णाङ्क)keytype, remotekey, keylen);
        CURL_CBAPI_अन्तः(&guard);
        यदि(rc != CURLKHMATCH_OK) {
          myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
          failf(दत्त, "SSH: callback failed host public key verification");
          प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(दत्त, "SSH: verified public key via callback");
      }
      अन्यथा {
        myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
        प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
      }
      प्रत्यागम CURLE_OK;
    }
    अन्यथा {
      CURL_TRC_SSH(दत्त, "no host key checksum given, checking knownhosts");
      प्रत्यागम ssh_knownhost(दत्त, sshc);
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
स्थिर CURLcode ssh_force_knownhost_कुञ्चिका_type(संरचना Curl_easy *दत्त,
                                             संरचना ssh_conn *sshc)
{
  CURLcode परिणाम = CURLE_OK;

  स्थिर ध्रुव वर्ण hostkey_विधानम्_ssh_ed25519[] = "ssh-ed25519";
  स्थिर ध्रुव वर्ण hostkey_विधानम्_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  स्थिर ध्रुव वर्ण hostkey_विधानम्_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  स्थिर ध्रुव वर्ण hostkey_विधानम्_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  स्थिर ध्रुव वर्ण hostkey_विधानम्_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#यदि_परिभाषितम् LIBSSH2_KNOWNHOST_कुञ्चिका_SSHDSS
  स्थिर ध्रुव वर्ण hostkey_विधानम्_ssh_dss[] = "ssh-dss";
#यदि_अन्तः
  bool प्राप्तम् = FALSE;

  यदि(sshc->kh &&
     !CURL_EASY_STR(दत्त, STRING_SSH_HOST_लौकिकम्_कुञ्चिका_MD5) &&
     !CURL_EASY_STR(दत्त, STRING_SSH_HOST_लौकिकम्_कुञ्चिका_SHA256)) {
    संरचना libssh2_knownhost *store = रिक्त;
    संरचना connectdata *conn = दत्त->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_कुञ्चिका_आवरणम् >>
                      LIBSSH2_KNOWNHOST_कुञ्चिका_सरणम्) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    यावत्(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      यदि(store) {
        यदि(store->name) {
          यदि(store->name[0] == '[') {
            curl_off_t पत्तनम्;
            size_t kh_name_आकार = 0;
            ध्रुव वर्ण *p;
            ध्रुव वर्ण *kh_name_अन्तः = वाक्ये_अन्विष(store->name, "]:");
            यदि(!kh_name_अन्तः) {
              infof(दत्त, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS));
              अनुवर्त;
            }
            p = kh_name_अन्तः + 2; /* start of port number */
            यदि(!curlx_str_सङ्ख्या(&p, &पत्तनम्, 0xffff) &&
               (kh_name_अन्तः && (पत्तनम् == conn->origin->पत्तनम्))) {
              kh_name_आकार = वाक्यदैर्घ्यम्(store->name) - 1 - वाक्यदैर्घ्यम्(kh_name_अन्तः);
              यदि((kh_name_आकार == वाक्यदैर्घ्यम्(conn->origin->hostname)) &&
                 !वाक्ये_तुलय(store->name + 1, conn->origin->hostname,
                          kh_name_आकार)) {
                प्राप्तम् = TRUE;
                भङ्ग;
              }
            }
          }
          अन्यथा यदि(!वाक्यं_तुलय(store->name, conn->origin->hostname)) {
            प्राप्तम् = TRUE;
            भङ्ग;
          }
        }
        अन्यथा यदि(store->कुञ्चिका) {
          अचिह्नित पूर्णाङ्क type = (store->typemask & LIBSSH2_KNOWNHOST_कुञ्चिका_आवरणम्)
                              >> LIBSSH2_KNOWNHOST_कुञ्चिका_सरणम्;
          यदि(!absent_type[type]) {
            पूर्णाङ्क keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->पत्तनम् != पत्तनम्_SSH) ? conn->origin->पत्तनम् : -1,
              store->कुञ्चिका, वाक्यदैर्घ्यम्(store->कुञ्चिका),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_कुञ्चिका_आवरणम्),
              रिक्त);
            यदि(keycheck == LIBSSH2_KNOWNHOST_CHECK_मेलनम्) {
              प्राप्तम् = TRUE;
              भङ्ग;
            }
            अन्यथा यदि(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    यदि(प्राप्तम्) {
      पूर्णाङ्क rc;
      ध्रुव वर्ण *hostkey_विधानम् = रिक्त;
      infof(दत्त, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS));

      विकल्प(store->typemask & LIBSSH2_KNOWNHOST_कुञ्चिका_आवरणम्) {
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_ED25519:
        hostkey_विधानम् = hostkey_विधानम्_ssh_ed25519;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_ECDSA_521:
        hostkey_विधानम् = hostkey_विधानम्_ssh_ecdsa_521;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_ECDSA_384:
        hostkey_विधानम् = hostkey_विधानम्_ssh_ecdsa_384;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_ECDSA_256:
        hostkey_विधानम् = hostkey_विधानम्_ssh_ecdsa_256;
        भङ्ग;
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_SSHRSA:
        hostkey_विधानम् = hostkey_विधानम्_ssh_rsa_all;
        भङ्ग;
#यदि_परिभाषितम् LIBSSH2_KNOWNHOST_कुञ्चिका_SSHDSS
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_SSHDSS:  /* deprecated upstream */
        hostkey_विधानम् = hostkey_विधानम्_ssh_dss;
        भङ्ग;
#यदि_अन्तः
      पक्ष LIBSSH2_KNOWNHOST_कुञ्चिका_RSA1:
        failf(दत्त, "Found host key type RSA1 which is not supported");
        प्रत्यागम CURLE_SSH;
      स्वभाव:
        failf(दत्त, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_कुञ्चिका_आवरणम्));
        प्रत्यागम CURLE_SSH;
      }

      infof(दत्त, "SSH: set '%s' as hostkey type", hostkey_विधानम्);
      rc = libssh2_session_विधानम्_pref(sshc->ssh_session,
                                       LIBSSH2_विधानम्_HOSTKEY, hostkey_विधानम्);
      यदि(rc) {
        वर्ण *err_सन्देशः = रिक्त;
        पूर्णाङ्क errlen;
        libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, &errlen, 0);
        failf(दत्त, "libssh2 method '%s' failed: %s", hostkey_विधानम्, err_सन्देशः);
        परिणाम = libssh2_session_error_गन्तव्यम्_CURLE(rc);
      }
    }
    अन्यथा {
      infof(दत्त, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS));
    }
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode quote_error(संरचना Curl_easy *दत्त,
                            संरचना ssh_conn *sshc)
{
  failf(दत्त, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  प्रत्यागम CURLE_QUOTE_ERROR;
}

स्थिर CURLcode sftp_quote(संरचना Curl_easy *दत्त,
                           संरचना ssh_conn *sshc,
                           संरचना SSHPROTO *sshp)
{
  ध्रुव वर्ण *cp;
  CURLcode परिणाम = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ध्रुव वर्ण *cmd = sshc->quote_item->दत्त;
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
                              sshp->मार्गः);
    यदि(!tmp)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    Curl_debug(दत्त, CURLINFO_शीर्षकम्_बहिर्गमनम्, "PWD\n", 4);
    Curl_debug(दत्त, CURLINFO_शीर्षकम्_अन्तर्, tmp, वाक्यदैर्घ्यम्(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, tmp, वाक्यदैर्घ्यम्(tmp));
    curlx_free(tmp);
    यदि(!परिणाम)
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
    प्रत्यागम परिणाम;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  यदि(!cp) {
    failf(दत्त, "Syntax error command '%s', missing parameter", cmd);
    प्रत्यागम CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  परिणाम = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  यदि(परिणाम) {
    यदि(परिणाम != CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
      failf(दत्त, "Syntax error: Bad first parameter to '%s'", cmd);
    प्रत्यागम परिणाम;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  यदि(!वाक्ये_तुलय(cmd, "chgrp ", 6) ||
     !वाक्ये_तुलय(cmd, "chmod ", 6) ||
     !वाक्ये_तुलय(cmd, "chown ", 6) ||
     !वाक्ये_तुलय(cmd, "atime ", 6) ||
     !वाक्ये_तुलय(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    यदि(परिणाम) {
      यदि(परिणाम != CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
        failf(दत्त, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      प्रत्यागम परिणाम;
    }
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);

    स्मृतिं_पूरय(&sshp->quote_attrs, 0, परिमाण(LIBSSH2_SFTP_गुणाः));
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_STAT);
    प्रत्यागम परिणाम;
  }
  यदि(!वाक्ये_तुलय(cmd, "ln ", 3) ||
     !वाक्ये_तुलय(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    यदि(परिणाम) {
      यदि(परिणाम != CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
        failf(दत्त, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      प्रत्यागम परिणाम;
    }
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_SYMLINK);
    प्रत्यागम परिणाम;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "mkdir ", 6)) {
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    /* create directory */
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_MKDIR);
    प्रत्यागम परिणाम;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    यदि(परिणाम) {
      यदि(परिणाम != CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः)
        failf(दत्त, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      प्रत्यागम परिणाम;
    }
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_RENAME);
    प्रत्यागम परिणाम;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "rmdir ", 6)) {
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    /* delete directory */
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_RMDIR);
    प्रत्यागम परिणाम;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "rm ", 3)) {
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_UNLINK);
    प्रत्यागम परिणाम;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "statvfs ", 8)) {
    यदि(*cp)
      प्रत्यागम quote_error(दत्त, sshc);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_STATVFS);
    प्रत्यागम परिणाम;
  }

  failf(दत्त, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  प्रत्यागम CURLE_QUOTE_ERROR;
}

स्थिर CURLcode sftp_upload_init(संरचना Curl_easy *दत्त,
                                 संरचना ssh_conn *sshc,
                                 संरचना SSHPROTO *sshp,
                                 bool *blockp)
{
  अचिह्नित दीर्घ flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  यदि(दत्त->state.resume_from) {
    LIBSSH2_SFTP_गुणाः attrs;
    यदि(दत्त->state.resume_from < 0) {
      पूर्णाङ्क rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->मार्गः,
                                    curlx_uztoui(वाक्यदैर्घ्यम्(sshp->मार्गः)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      यदि(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        प्रत्यागम CURLE_OK;
      }
      यदि(rc) {
        दत्त->state.resume_from = 0;
      }
      अन्यथा {
        curl_off_t आकार = attrs.filesize;
        यदि(आकार < 0) {
          failf(दत्त, "Bad file size (%" FMT_OFF_T ")", आकार);
          प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
        }
        दत्त->state.resume_from = attrs.filesize;
      }
    }
  }

  यदि(दत्त->समुच्चयः.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  अन्यथा यदि(दत्त->state.resume_from > 0) {
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
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->मार्गः,
                         curlx_uztoui(वाक्यदैर्घ्यम्(sshp->मार्गः)),
                         flags, (दीर्घ)दत्त->समुच्चयः.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  यदि(!sshc->sftp_handle) {
    CURLcode परिणाम;
    अचिह्नित दीर्घ sftperr;
    पूर्णाङ्क rc = libssh2_session_चरमः_errno(sshc->ssh_session);

    यदि(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      प्रत्यागम CURLE_OK;
    }

    यदि(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    अन्यथा
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    यदि(sshc->secondCreateDirs) {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      failf(दत्त, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      प्रत्यागम sftp_libssh2_error_गन्तव्यम्_CURLE(sftperr);
    }
    यदि((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_मार्गः) &&
       दत्त->समुच्चयः.ftp_create_missing_dirs &&
       (वाक्यदैर्घ्यम्(sshp->मार्गः) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      प्रत्यागम CURLE_OK;
    }
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    परिणाम = sftp_libssh2_error_गन्तव्यम्_CURLE(sftperr);
    यदि(!परिणाम) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      परिणाम = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(दत्त, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    प्रत्यागम परिणाम;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  यदि(दत्त->state.resume_from > 0 && !दत्त->समुच्चयः.remote_append) {
    पूर्णाङ्क seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    यदि(दत्त->समुच्चयः.seek_func) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, दत्त, easy_seek_func);
      seekerr = दत्त->समुच्चयः.seek_func(दत्त->समुच्चयः.seek_client,
                                    दत्त->state.resume_from, SEEK_SET);
      CURL_CBAPI_अन्तः(&guard);
    }

    यदि(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      यदि(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(दत्त, "Could not seek stream");
        प्रत्यागम CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      कुरु {
        संरचना Curl_mapi_guard guard;
        वर्ण scratch[4 * 1024];
        size_t readthisamountnow =
          (दत्त->state.resume_from - passed >
           (curl_off_t)परिमाण(scratch)) ?
          परिमाण(scratch) : curlx_sotouz(दत्त->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, दत्त, easy_fread_func);
        actuallyread = दत्त->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              दत्त->state.अन्तर्);
        CURL_CBAPI_अन्तः(&guard);

        passed += actuallyread;
        यदि((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(दत्त, "Failed to read data");
          प्रत्यागम CURLE_FTP_COULDNT_USE_REST;
        }
      } यावत्(passed < दत्त->state.resume_from);
    }

    /* now, decrease the size of the read */
    यदि(दत्त->state.infilesize > 0) {
      दत्त->state.infilesize -= दत्त->state.resume_from;
      दत्त->req.आकार = दत्त->state.infilesize;
      Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)दत्त->state.resume_from);
  }
  यदि(दत्त->state.infilesize > 0) {
    दत्त->req.आकार = दत्त->state.infilesize;
    Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);
  }
  /* upload data */
  Curl_xfer_सज्जता_send(दत्त, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  दत्त->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(दत्त);

  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य ssh_state_pkey_init(संरचना Curl_easy *दत्त, संरचना ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  यदि((दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     वाक्ये_अन्विष(sshc->authlist, "publickey")) {
    यदि(sshc->pub_कुञ्चिका)
      infof(दत्त, "SSH: trying public key file '%s'", sshc->pub_कुञ्चिका);
    infof(दत्त, "SSH: trying private key file '%s'", sshc->priv_कुञ्चिका);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PKEY);
  }
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PASS_INIT);
}

स्थिर CURLcode sftp_quote_stat(संरचना Curl_easy *दत्त,
                                संरचना ssh_conn *sshc,
                                संरचना SSHPROTO *sshp,
                                bool *blockp)
{
  ध्रुव वर्ण *cmd = sshc->quote_item->दत्त;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  यदि(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  यदि(वाक्ये_तुलय(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    पूर्णाङ्क rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    यदि(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      प्रत्यागम CURLE_OK;
    }
    यदि(rc && !sshc->acceptfail) { /* get those attributes */
      अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
      failf(दत्त, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      गच्छ fail;
    }
  }

  /* Now set the new attributes... */
  यदि(!वाक्ये_तुलय(cmd, "chgrp", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t gid;
    यदि(!curlx_str_सङ्ख्या(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (अचिह्नित दीर्घ)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    अन्यथा यदि(!sshc->acceptfail) {
      failf(दत्त, "Syntax error: chgrp gid not a number");
      गच्छ fail;
    }
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "chmod", 5)) {
    curl_off_t perms;
    ध्रुव वर्ण *p = sshc->quote_path1;
    /* permissions are octal */
    यदि(curlx_str_octal(&p, &perms, 07777)) {
      failf(दत्त, "Syntax error: chmod permissions not a number");
      गच्छ fail;
    }

    sshp->quote_attrs.permissions = (अचिह्नित दीर्घ)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "chown", 5)) {
    ध्रुव वर्ण *p = sshc->quote_path1;
    curl_off_t uid;
    यदि(!curlx_str_सङ्ख्या(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (अचिह्नित दीर्घ)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    अन्यथा यदि(!sshc->acceptfail) {
      failf(दत्त, "Syntax error: chown uid not a number");
      गच्छ fail;
    }
  }
  अन्यथा यदि(!वाक्ये_तुलय(cmd, "atime", 5) ||
          !वाक्ये_तुलय(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    यदि(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(दत्त, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#यदि SIZEOF_TIME_T > SIZEOF_LONG
    यदि(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(दत्त, "date overflow");
      fail = TRUE;
    }
#यदि_अन्तः
    यदि(fail)
      गच्छ fail;
    यदि(!वाक्ये_तुलय(cmd, "atime", 5))
      sshp->quote_attrs.atime = (अचिह्नित दीर्घ)date;
    अन्यथा /* mtime */
      sshp->quote_attrs.mtime = (अचिह्नित दीर्घ)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_SETSTAT);
  प्रत्यागम CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  प्रत्यागम CURLE_QUOTE_ERROR;
}

स्थिर CURLcode sftp_download_stat(संरचना Curl_easy *दत्त,
                                   संरचना ssh_conn *sshc,
                                   संरचना SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_गुणाः attrs;
  पूर्णाङ्क rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->मार्गः,
                                curlx_uztoui(वाक्यदैर्घ्यम्(sshp->मार्गः)),
                                LIBSSH2_SFTP_STAT, &attrs);
  यदि(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    प्रत्यागम CURLE_OK;
  }
  यदि(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_आकार) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    दत्त->req.आकार = -1;
    दत्त->req.maxdownload = -1;
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  अन्यथा {
    curl_off_t आकार = attrs.filesize;

    यदि(आकार < 0) {
      failf(दत्त, "Bad file size (%" FMT_OFF_T ")", आकार);
      प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
    }
    यदि(दत्त->state.use_range) {
      curl_off_t from;
      CURLcode परिणाम = Curl_ssh_range(दत्त, दत्त->state.range, आकार,
                                       &from, &आकार);
      यदि(परिणाम)
        प्रत्यागम परिणाम;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    दत्त->req.आकार = आकार;
    दत्त->req.maxdownload = आकार;
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, आकार);
  }

  /* We can resume if we can seek to the resume position */
  यदि(दत्त->state.resume_from) {
    यदि(दत्त->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      यदि((curl_off_t)attrs.filesize < -दत्त->state.resume_from) {
        failf(दत्त, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              दत्त->state.resume_from, (curl_off_t)attrs.filesize);
        प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      दत्त->state.resume_from += attrs.filesize;
    }
    अन्यथा {
      यदि((curl_off_t)attrs.filesize < दत्त->state.resume_from) {
        failf(दत्त, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              दत्त->state.resume_from, (curl_off_t)attrs.filesize);
        प्रत्यागम CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    दत्त->req.आकार = attrs.filesize - दत्त->state.resume_from;
    दत्त->req.maxdownload = attrs.filesize - दत्त->state.resume_from;
    Curl_pgrs_समुच्चयः_Download_आकार(दत्त, attrs.filesize - दत्त->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)दत्त->state.resume_from);
  }

  /* Setup the actual download */
  यदि(दत्त->req.आकार == 0) {
    /* no data to transfer */
    Curl_xfer_सज्जता_nop(दत्त);
    infof(दत्त, "SSH: file already completely downloaded");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
    प्रत्यागम CURLE_OK;
  }
  Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, दत्त->req.आकार);

  /* not set by Curl_xfer_setup to preserve io_flags */
  दत्त->conn->send_idx = 0;

  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode sftp_readdir(संरचना Curl_easy *दत्त,
                             संरचना ssh_conn *sshc,
                             संरचना SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_मार्गः_MAX,
                                   sshp->readdir_longentry, CURL_मार्गः_MAX,
                                   &sshp->readdir_attrs);
  यदि(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    प्रत्यागम परिणाम;
  }
  यदि(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    यदि(दत्त->समुच्चयः.list_only) {
      परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      यदि(!परिणाम)
        परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY, "\n", 1);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    अन्यथा {
      परिणाम = curlx_dyn_सङ्कलय(&sshp->readdir, sshp->readdir_longentry);

      यदि(!परिणाम) {
        यदि((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          परिणाम = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->मार्गः,
                                  sshp->readdir_filename);
          myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_LINK);
        }
        अन्यथा {
          myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      प्रत्यागम परिणाम;
    }
  }
  अन्यथा यदि(!rc) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_समाप्तम्);
  }
  अन्यथा {
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    परिणाम = sftperr ? sftp_libssh2_error_गन्तव्यम्_CURLE(sftperr) : CURLE_SSH;
    failf(दत्त, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_चरमः_errno(sshc->ssh_session));
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_init(संरचना Curl_easy *दत्त,
                               संरचना ssh_conn *sshc)
{
  CURLcode परिणाम;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_समुच्चयः_blocking(sshc->ssh_session, 0);

  परिणाम = ssh_force_knownhost_कुञ्चिका_type(दत्त, sshc);
  यदि(परिणाम)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_S_STARTUP);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_startup(संरचना Curl_easy *दत्त,
                                  संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = दत्त->conn;
  पूर्णाङ्क rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc) {
    वर्ण *err_सन्देशः = रिक्त;
    (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    failf(दत्त, "Failure establishing ssh session: %d, %s", rc, err_सन्देशः);

    myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  myssh_गन्तव्यम्(दत्त, sshc, SSH_HOSTKEY);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_hostkey(संरचना Curl_easy *दत्त,
                                  संरचना ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode परिणाम = ssh_check_fingerprint(दत्त, sshc);
  यदि(!परिणाम)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTHLIST);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_authlist(संरचना Curl_easy *दत्त,
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
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(वाक्यदैर्घ्यम्(user)));

  यदि(!sshc->authlist) {
    पूर्णाङ्क rc;
    यदि(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(दत्त, "SSH: user accepted with no authentication");
      myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
      प्रत्यागम CURLE_OK;
    }
    rc = libssh2_session_चरमः_errno(sshc->ssh_session);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
    प्रत्यागम libssh2_session_error_गन्तव्यम्_CURLE(rc);
  }
  infof(दत्त, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PKEY_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_pkey(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  पूर्णाङ्क rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(वाक्यदैर्घ्यम्(user)),
    sshc->pub_कुञ्चिका, sshc->priv_कुञ्चिका, sshc->passphrase ? sshc->passphrase : "");
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc == 0) {
    sshc->authed = TRUE;
    infof(दत्त, "SSH: authenticated via publickey");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
  }
  अन्यथा {
    वर्ण *err_सन्देशः = रिक्त;
    वर्ण unknown[] = "Reason unknown (-1)";
    यदि(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_सन्देशः = unknown;
    }
    अन्यथा {
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    }
    infof(दत्त, "SSH: publickey authentication denied: %s", err_सन्देशः);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PASS_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_pass_init(संरचना Curl_easy *दत्त,
                                         संरचना ssh_conn *sshc)
{
  यदि((दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     वाक्ये_अन्विष(sshc->authlist, "password")) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_PASS);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_HOST_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_pass(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  ध्रुव वर्ण *passwd = Curl_creds_passwd(conn->creds);
  पूर्णाङ्क rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(वाक्यदैर्घ्यम्(user)),
                                 passwd,
                                 curlx_uztoui(वाक्यदैर्घ्यम्(passwd)),
                                 रिक्त);
  यदि(rc == LIBSSH2_ERROR_EAGAIN) {
    प्रत्यागम CURLE_AGAIN;
  }
  यदि(rc == 0) {
    sshc->authed = TRUE;
    infof(दत्त, "SSH: initialized password authentication");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_HOST_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_host_init(संरचना Curl_easy *दत्त,
                                         संरचना ssh_conn *sshc)
{
  यदि((दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     वाक्ये_अन्विष(sshc->authlist, "hostbased")) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_HOST);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_AGENT_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_agent_init(संरचना Curl_easy *दत्त,
                                          संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = 0;
  यदि((दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     वाक्ये_अन्विष(sshc->authlist, "publickey")) {

    infof(दत्त, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    यदि(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      यदि(!sshc->ssh_agent) {
        infof(दत्त, "SSH: could not create agent object");

        myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_कुञ्चिका_INIT);
        प्रत्यागम CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc < 0) {
      infof(दत्त, "SSH: failure connecting to agent");
      myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_कुञ्चिका_INIT);
    }
    अन्यथा {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_कुञ्चिका_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_agent_list(संरचना Curl_easy *दत्त,
                                          संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_agent_list_identities(sshc->ssh_agent);

  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;
  यदि(rc < 0) {
    infof(दत्त, "SSH: failure requesting identities to agent");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_कुञ्चिका_INIT);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_पूर्व_तटस्थः = रिक्त;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_agent(संरचना Curl_easy *दत्त,
                                     संरचना ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  पूर्णाङ्क rc = libssh2_agent_get_तटस्थः(sshc->ssh_agent,
                                      &sshc->sshagent_तटस्थः,
                                      sshc->sshagent_पूर्व_तटस्थः);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc == 0) {
    CURL_TRC_SSH(दत्त, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(दत्त->conn->creds),
                 sshc->sshagent_तटस्थः->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(दत्त->conn->creds),
                                sshc->sshagent_तटस्थः);

    यदि(rc < 0) {
      यदि(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_पूर्व_तटस्थः = sshc->sshagent_तटस्थः;
        प्रत्यागम CURLE_OK;
      }
      प्रत्यागम CURLE_AGAIN;
    }
  }

  यदि(rc < 0)
    infof(दत्त, "SSH: failure requesting identities to agent");
  अन्यथा यदि(rc == 1)
    infof(दत्त, "SSH: no agent identity would match");

  यदि(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(दत्त, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(दत्त->conn->creds),
          sshc->sshagent_तटस्थः->comment);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_कुञ्चिका_INIT);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_कुञ्चिका_init(संरचना Curl_easy *दत्त,
                                        संरचना ssh_conn *sshc)
{
  यदि((दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     वाक्ये_अन्विष(sshc->authlist, "keyboard-interactive")) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_कुञ्चिका);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_auth_कुञ्चिका(संरचना Curl_easy *दत्त,
                                   संरचना ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  संरचना connectdata *conn = दत्त->conn;
  ध्रुव वर्ण *user = Curl_creds_user(conn->creds);
  पूर्णाङ्क rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(वाक्यदैर्घ्यम्(user)),
                                             &kbd_callback);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc == 0) {
    sshc->authed = TRUE;
    infof(दत्त, "SSH: initialized keyboard interactive authentication");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_समाप्तम्);
    प्रत्यागम CURLE_OK;
  }
  प्रत्यागम CURLE_LOGIN_DENIED;
}

स्थिर CURLcode ssh_state_auth_समाप्तम्(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = दत्त->conn;
  यदि(!sshc->authed) {
    failf(दत्त, "Authentication failure");
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
    प्रत्यागम CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(दत्त, "SSH: authentication complete");

  Curl_pgrsTime(दत्त, TIMER_APPCONNECT); /* SSH is connected */

  दत्त->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  यदि(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_INIT);
    प्रत्यागम CURLE_OK;
  }
  infof(दत्त, "SSH: connection established");
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_init(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  यदि(!sshc->sftp_session) {
    वर्ण *err_सन्देशः = रिक्त;
    यदि(libssh2_session_चरमः_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    failf(दत्त, "Failure initializing sftp session: %s", err_सन्देशः);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
    प्रत्यागम CURLE_FAILED_INIT;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_REALPATH);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_realpath(संरचना Curl_easy *दत्त,
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
                               sshp->readdir_filename, CURL_मार्गः_MAX,
                               LIBSSH2_SFTP_REALPATH);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  यदि(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    यदि(!sshc->homedir)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    curlx_free(दत्त->state.most_recent_ftp_entrypath);
    दत्त->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    यदि(!दत्त->state.most_recent_ftp_entrypath)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }
  अन्यथा {
    /* Return the error type */
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    CURLcode परिणाम;
    यदि(sftperr)
      परिणाम = sftp_libssh2_error_गन्तव्यम्_CURLE(sftperr);
    अन्यथा
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      परिणाम = CURLE_SSH;
    CURL_TRC_SSH(दत्त, "error = %lu makes libcurl = %d", sftperr, (पूर्णाङ्क)परिणाम);
    प्रत्यागम परिणाम;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(दत्त, "CONNECT phase done");
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_init(संरचना Curl_easy *दत्त,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  CURLcode परिणाम;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = Curl_getworkingpath(दत्त, sshc->homedir, &sshp->मार्गः);
  यदि(परिणाम) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
    प्रत्यागम परिणाम;
  }

  यदि(दत्त->समुच्चयः.quote) {
    infof(दत्त, "SSH: sending quote commands");
    sshc->quote_item = दत्त->समुच्चयः.quote;
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_GETINFO);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_postquote_init(संरचना Curl_easy *दत्त,
                                              संरचना ssh_conn *sshc)
{
  यदि(दत्त->समुच्चयः.postquote) {
    infof(दत्त, "SSH: sending quote commands");
    sshc->quote_item = दत्त->समुच्चयः.postquote;
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote(संरचना Curl_easy *दत्त,
                                     संरचना ssh_conn *sshc,
                                     संरचना SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode परिणाम;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = sftp_quote(दत्त, sshc, sshp);
  यदि(परिणाम) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_sftp_अग्र_quote(संरचना Curl_easy *दत्त,
                                          संरचना ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->अग्र;

  यदि(sshc->quote_item) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    यदि(sshc->nextstate != SSH_NO_STATE) {
      myssh_गन्तव्यम्(दत्त, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    अन्यथा {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_GETINFO);
    }
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_stat(संरचना Curl_easy *दत्त,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode परिणाम;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = sftp_quote_stat(दत्त, sshc, sshp, blockp);
  यदि(परिणाम) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_sftp_quote_setstat(संरचना Curl_easy *दत्त,
                                             संरचना ssh_conn *sshc,
                                             संरचना SSHPROTO *sshp)
{
  पूर्णाङ्क rc;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    failf(दत्त, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_symlink(संरचना Curl_easy *दत्त,
                                             संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    failf(दत्त, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_mkdir(संरचना Curl_easy *दत्त,
                                           संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path1)),
                                 (दीर्घ)दत्त->समुच्चयः.new_directory_perms);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    failf(दत्त, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_rename(संरचना Curl_easy *दत्त,
                                            संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    failf(दत्त, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_rmdir(संरचना Curl_easy *दत्त,
                                           संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path1)));
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    failf(दत्त, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_unlink(संरचना Curl_easy *दत्त,
                                            संरचना ssh_conn *sshc)
{
  पूर्णाङ्क rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path1)));
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    failf(दत्त, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_quote_statvfs(संरचना Curl_easy *दत्त,
                                             संरचना ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  पूर्णाङ्क rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(वाक्यदैर्घ्यम्(sshc->quote_path1)),
                                &statvfs);

  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc && !sshc->acceptfail) {
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    failf(दत्त, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    प्रत्यागम CURLE_QUOTE_ERROR;
  }
  अन्यथा यदि(rc == 0) {
#यदि_परिभाषितम् _MSC_VER
#परिभाषय CURL_LIBSSH2_VFS_आकार_आवरणम् "I64u"
#अन्यथा
#परिभाषय CURL_LIBSSH2_VFS_आकार_आवरणम् "llu"
#यदि_अन्तः
    CURLcode परिणाम;
    वर्ण *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_आकार_आवरणम् "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_खण्डानि, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    यदि(!tmp) {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_शीर्षकम्, tmp, वाक्यदैर्घ्यम्(tmp));
    curlx_free(tmp);
    यदि(परिणाम) {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      प्रत्यागम परिणाम;
    }
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_अग्र_QUOTE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_create_dirs_mkdir(संरचना Curl_easy *दत्त,
                                                 संरचना ssh_conn *sshc,
                                                 संरचना SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  पूर्णाङ्क rc;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->मार्गः,
                             curlx_uztoui(वाक्यदैर्घ्यम्(sshp->मार्गः)),
                             (दीर्घ)दत्त->समुच्चयः.new_directory_perms);
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
    अचिह्नित दीर्घ sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    यदि((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
      प्रत्यागम sftp_libssh2_error_गन्तव्यम्_CURLE(sftperr);
    }
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CREATE_DIRS);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_readdir_init(संरचना Curl_easy *दत्त,
                                            संरचना ssh_conn *sshc,
                                            संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  Curl_pgrs_समुच्चयः_Download_आकार(दत्त, -1);
  यदि(दत्त->req.no_body) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
    प्रत्यागम CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->मार्गः,
                         curlx_uztoui(वाक्यदैर्घ्यम्(sshp->मार्गः)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  यदि(!sshc->sftp_handle) {
    अचिह्नित दीर्घ sftperr;
    यदि(libssh2_session_चरमः_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    failf(दत्त, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    प्रत्यागम sftp_libssh2_error_गन्तव्यम्_CURLE(sftperr);
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_readdir_link(संरचना Curl_easy *दत्त,
                                            संरचना ssh_conn *sshc,
                                            संरचना SSHPROTO *sshp)
{
  CURLcode परिणाम;
  पूर्णाङ्क rc;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (अचिह्नित पूर्णाङ्क)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_मार्गः_MAX, LIBSSH2_SFTP_READLINK);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  यदि(rc < 0)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* append filename and extra output */
  परिणाम = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  यदि(परिणाम)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_BOTTOM);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_scp_download_init(संरचना Curl_easy *दत्त,
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
  स्मृतिं_पूरय(&sb, 0, परिमाण(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->मार्गः, &sb);
  यदि(!sshc->ssh_channel) {
    पूर्णाङ्क ssh_err;
    वर्ण *err_सन्देशः = रिक्त;

    यदि(libssh2_session_चरमः_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    ssh_err = libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    failf(दत्त, "%s", err_सन्देशः);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_CHANNEL_FREE);
    प्रत्यागम libssh2_session_error_गन्तव्यम्_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_आकार;
  दत्त->req.maxdownload = (curl_off_t)sb.st_आकार;
  Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  दत्त->conn->send_idx = 0;

  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_close(संरचना Curl_easy *दत्त,
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
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to close libssh2 file: %d %s", rc, err_सन्देशः);
    }
    sshc->sftp_handle = रिक्त;
  }

  curlx_safefree(sshp->मार्गः);

  CURL_TRC_SSH(दत्त, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  यदि(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_गन्तव्यम्(दत्त, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_shutdown(संरचना Curl_easy *दत्त,
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
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to close libssh2 file: %d %s", rc, err_सन्देशः);
    }
    sshc->sftp_handle = रिक्त;
  }
  यदि(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    यदि(rc < 0) {
      infof(दत्त, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = रिक्त;
  }

  curlx_safefree(sshc->homedir);

  myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_DISCONNECT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_download_init(संरचना Curl_easy *दत्त,
                                             संरचना ssh_conn *sshc,
                                             संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->मार्गः,
                         curlx_uztoui(वाक्यदैर्घ्यम्(sshp->मार्गः)),
                         LIBSSH2_FXF_READ, (दीर्घ)दत्त->समुच्चयः.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  यदि(!sshc->sftp_handle) {
    अचिह्नित दीर्घ sftperr;
    यदि(libssh2_session_चरमः_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      प्रत्यागम CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_चरमः_error(sshc->sftp_session);
    failf(दत्त, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    प्रत्यागम sftp_libssh2_error_गन्तव्यम्_CURLE(sftperr);
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_DOWNLOAD_STAT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_upload_init(संरचना Curl_easy *दत्त,
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
    libssh2_scp_send64(sshc->ssh_session, sshp->मार्गः,
                       (पूर्णाङ्क)दत्त->समुच्चयः.new_file_perms,
                       (libssh2_int64_t)दत्त->state.infilesize, 0, 0);
  यदि(!sshc->ssh_channel) {
    पूर्णाङ्क ssh_err;
    वर्ण *err_सन्देशः = रिक्त;
    CURLcode परिणाम;
    यदि(libssh2_session_चरमः_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    ssh_err = libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
    failf(दत्त, "%s", err_सन्देशः);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_CHANNEL_FREE);
    परिणाम = libssh2_session_error_गन्तव्यम्_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    यदि(परिणाम == CURLE_SSH ||
       परिणाम == CURLE_REMOTE_FILE_NOT_प्राप्तम्)
      परिणाम = CURLE_UPLOAD_FAILED;
    प्रत्यागम परिणाम;
  }

  /* upload data */
  दत्त->req.आकार = दत्त->state.infilesize;
  Curl_pgrs_समुच्चयः_Upload_आकार(दत्त, दत्त->state.infilesize);
  Curl_xfer_सज्जता_send(दत्त, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  दत्त->conn->recv_idx = FIRSTSOCKET;

  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_session_disconnect(संरचना Curl_easy *दत्त,
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
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to free libssh2 scp subsystem: %d %s", rc, err_सन्देशः);
    }
    sshc->ssh_channel = रिक्त;
  }

  यदि(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;

    यदि(rc < 0) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to disconnect libssh2 session: %d %s", rc, err_सन्देशः);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_FREE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode sshc_cleanup(संरचना ssh_conn *sshc, संरचना Curl_easy *दत्त,
                             bool खण्डम्)
{
  पूर्णाङ्क rc;

  यदि(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = रिक्त;
  }

  यदि(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    यदि((rc < 0) && दत्त) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_सन्देशः);
    }
    यदि(!खण्डम् && (rc == LIBSSH2_ERROR_EAGAIN))
      प्रत्यागम CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = रिक्त;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_तटस्थः = रिक्त;
    sshc->sshagent_पूर्व_तटस्थः = रिक्त;
  }

  यदि(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    यदि((rc < 0) && दत्त) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to close libssh2 file: %d %s", rc, err_सन्देशः);
    }
    यदि(!खण्डम् && (rc == LIBSSH2_ERROR_EAGAIN))
      प्रत्यागम CURLE_AGAIN;

    sshc->sftp_handle = रिक्त;
  }

  यदि(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    यदि((rc < 0) && दत्त) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to free libssh2 scp subsystem: %d %s", rc, err_सन्देशः);
    }
    यदि(!खण्डम् && (rc == LIBSSH2_ERROR_EAGAIN))
      प्रत्यागम CURLE_AGAIN;

    sshc->ssh_channel = रिक्त;
  }

  यदि(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    यदि((rc < 0) && दत्त) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_सन्देशः);
    }
    यदि(!खण्डम् && (rc == LIBSSH2_ERROR_EAGAIN))
      प्रत्यागम CURLE_AGAIN;

    sshc->sftp_session = रिक्त;
  }

  यदि(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    यदि((rc < 0) && दत्त) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session, &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to free libssh2 session: %d %s", rc, err_सन्देशः);
    }
    यदि(!खण्डम् && (rc == LIBSSH2_ERROR_EAGAIN))
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
  curlx_safefree(sshc->pub_कुञ्चिका);
  curlx_safefree(sshc->priv_कुञ्चिका);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_getinfo(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc)
{
  यदि(दत्त->समुच्चयः.get_filetime)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_FILETIME);
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_TRANS_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_filetime(संरचना Curl_easy *दत्त,
                                        संरचना ssh_conn *sshc,
                                        संरचना SSHPROTO *sshp)
{
  LIBSSH2_SFTP_गुणाः attrs;
  पूर्णाङ्क rc;

  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->मार्गः,
                            curlx_uztoui(वाक्यदैर्घ्यम्(sshp->मार्गः)),
                            LIBSSH2_SFTP_STAT, &attrs);
  यदि(rc == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  यदि(rc == 0)
    दत्त->info.filetime = (time_t)attrs.mtime;

  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_TRANS_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_trans_init(संरचना Curl_easy *दत्त,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  यदि(दत्त->state.upload)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_UPLOAD_INIT);
  अन्यथा यदि(sshp) {
    size_t plen = वाक्यदैर्घ्यम्(sshp->मार्गः);
    यदि(plen && (sshp->मार्गः[plen - 1] == '/'))
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR_INIT);
    अन्यथा
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  अन्यथा
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_upload_init(संरचना Curl_easy *दत्त,
                                           संरचना ssh_conn *sshc,
                                           संरचना SSHPROTO *sshp,
                                           bool *खण्डम्)
{
  CURLcode परिणाम;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = sftp_upload_init(दत्त, sshc, sshp, खण्डम्);
  यदि(परिणाम) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_sftp_create_dirs_init(संरचना Curl_easy *दत्त,
                                                संरचना ssh_conn *sshc,
                                                संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  यदि(वाक्यदैर्घ्यम्(sshp->मार्गः) > 1) {
    sshc->slash_pos = sshp->मार्गः + 1; /* ignore the leading '/' */
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CREATE_DIRS);
  }
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_UPLOAD_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_create_dirs(संरचना Curl_easy *दत्त,
                                           संरचना ssh_conn *sshc,
                                           संरचना SSHPROTO *sshp)
{
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  यदि(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(दत्त, "SFTP: creating directory '%s'", sshp->मार्गः);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    प्रत्यागम CURLE_OK;
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_UPLOAD_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_readdir(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc,
                                       संरचना SSHPROTO *sshp,
                                       bool *खण्डम्)
{
  CURLcode परिणाम;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = sftp_readdir(दत्त, sshc, sshp, खण्डम्);
  यदि(परिणाम)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_sftp_readdir_bottom(संरचना Curl_easy *दत्त,
                                              संरचना ssh_conn *sshc,
                                              संरचना SSHPROTO *sshp)
{
  CURLcode परिणाम;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  यदि(!परिणाम)
    परिणाम = Curl_client_write(दत्त, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  यदि(परिणाम) {
    curlx_dyn_free(&sshp->readdir);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  }
  अन्यथा {
    curlx_dyn_reset(&sshp->readdir);
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_READDIR);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_sftp_readdir_समाप्तम्(संरचना Curl_easy *दत्त,
                                            संरचना ssh_conn *sshc)
{
  यदि(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;

  sshc->sftp_handle = रिक्त;

  /* no data to transfer */
  Curl_xfer_सज्जता_nop(दत्त);
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_sftp_download_stat(संरचना Curl_easy *दत्त,
                                             संरचना ssh_conn *sshc,
                                             संरचना SSHPROTO *sshp,
                                             bool *खण्डम्)
{
  CURLcode परिणाम;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = sftp_download_stat(दत्त, sshc, sshp, खण्डम्);
  यदि(परिणाम) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_state_scp_trans_init(संरचना Curl_easy *दत्त,
                                         संरचना ssh_conn *sshc,
                                         संरचना SSHPROTO *sshp)
{
  CURLcode परिणाम;
  यदि(!sshp)
    प्रत्यागम CURLE_FAILED_INIT;
  परिणाम = Curl_getworkingpath(दत्त, sshc->homedir,
                               &sshp->मार्गः);
  यदि(परिणाम) {
    myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
    प्रत्यागम परिणाम;
  }

  यदि(दत्त->state.upload) {
    यदि(दत्त->state.infilesize < 0) {
      failf(दत्त, "SCP requires a known file size for upload");
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_CHANNEL_FREE);
      प्रत्यागम CURLE_UPLOAD_FAILED;
    }
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_UPLOAD_INIT);
  }
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_DOWNLOAD_INIT);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_समाप्तम्(संरचना Curl_easy *दत्त,
                                   संरचना ssh_conn *sshc)
{
  यदि(दत्त->state.upload)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_SEND_EOF);
  अन्यथा
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_CHANNEL_FREE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_send_eof(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc)
{
  यदि(sshc->ssh_channel) {
    पूर्णाङ्क rc = libssh2_channel_send_eof(sshc->ssh_channel);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session,
                                       &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_सन्देशः);
    }
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_WAIT_EOF);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_wait_eof(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc)
{
  यदि(sshc->ssh_channel) {
    पूर्णाङ्क rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session,
                                       &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to get channel EOF: %d %s", rc, err_सन्देशः);
    }
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_WAIT_CLOSE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_wait_close(संरचना Curl_easy *दत्त,
                                         संरचना ssh_conn *sshc)
{
  यदि(sshc->ssh_channel) {
    पूर्णाङ्क rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session,
                                       &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Channel failed to close: %d %s", rc, err_सन्देशः);
    }
  }
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_CHANNEL_FREE);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_scp_channel_free(
  संरचना Curl_easy *दत्त,
  संरचना ssh_conn *sshc)
{
  यदि(sshc->ssh_channel) {
    पूर्णाङ्क rc = libssh2_channel_free(sshc->ssh_channel);
    यदि(rc == LIBSSH2_ERROR_EAGAIN)
      प्रत्यागम CURLE_AGAIN;
    यदि(rc < 0) {
      वर्ण *err_सन्देशः = रिक्त;
      (शून्य)libssh2_session_चरमः_error(sshc->ssh_session,
                                       &err_सन्देशः, रिक्त, 0);
      infof(दत्त, "Failed to free libssh2 scp subsystem: %d %s", rc, err_सन्देशः);
    }
    sshc->ssh_channel = रिक्त;
  }
  CURL_TRC_SSH(दत्त, "SCP DONE phase complete");
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssh_state_session_free(संरचना Curl_easy *दत्त,
                                       संरचना ssh_conn *sshc)
{
  CURLcode परिणाम = sshc_cleanup(sshc, दत्त, FALSE);
  संरचना connectdata *conn = दत्त->conn;
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  स्मृतिं_पूरय(sshc, 0, परिमाण(संरचना ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
  प्रत्यागम CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
स्थिर CURLcode ssh_statemachine(संरचना Curl_easy *दत्त,
                                 संरचना ssh_conn *sshc,
                                 संरचना SSHPROTO *sshp,
                                 bool *खण्डम्)
{
  CURLcode परिणाम = CURLE_OK;
  *खण्डम् = 0; /* we are not blocking by default */

  कुरु {
    विकल्प(sshc->state) {
    पक्ष SSH_INIT:
      परिणाम = ssh_state_init(दत्त, sshc);
      यदि(परिणाम)
        भङ्ग;
      FALLTHROUGH();

    पक्ष SSH_S_STARTUP:
      परिणाम = ssh_state_startup(दत्त, sshc);
      यदि(परिणाम)
        भङ्ग;
      FALLTHROUGH();

    पक्ष SSH_HOSTKEY:
      परिणाम = ssh_state_hostkey(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTHLIST:
      परिणाम = ssh_state_authlist(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_PKEY:
      परिणाम = ssh_state_auth_pkey(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_PASS_INIT:
      परिणाम = ssh_state_auth_pass_init(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_PASS:
      परिणाम = ssh_state_auth_pass(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_HOST_INIT:
      परिणाम = ssh_state_auth_host_init(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_HOST:
      myssh_गन्तव्यम्(दत्त, sshc, SSH_AUTH_AGENT_INIT);
      भङ्ग;

    पक्ष SSH_AUTH_AGENT_INIT:
      परिणाम = ssh_state_auth_agent_init(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_AGENT_LIST:
      परिणाम = ssh_state_auth_agent_list(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_AGENT:
      परिणाम = ssh_state_auth_agent(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_कुञ्चिका_INIT:
      परिणाम = ssh_state_auth_कुञ्चिका_init(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_कुञ्चिका:
      परिणाम = ssh_state_auth_कुञ्चिका(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_AUTH_समाप्तम्:
      परिणाम = ssh_state_auth_समाप्तम्(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_INIT:
      परिणाम = ssh_state_sftp_init(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_REALPATH:
      परिणाम = ssh_state_sftp_realpath(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_INIT:
      परिणाम = ssh_state_sftp_quote_init(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_POSTQUOTE_INIT:
      परिणाम = ssh_state_sftp_postquote_init(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE:
      परिणाम = ssh_state_sftp_quote(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_अग्र_QUOTE:
      परिणाम = ssh_state_sftp_अग्र_quote(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_STAT:
      परिणाम = ssh_state_sftp_quote_stat(दत्त, sshc, sshp, खण्डम्);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_SETSTAT:
      परिणाम = ssh_state_sftp_quote_setstat(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_SYMLINK:
      परिणाम = ssh_state_sftp_quote_symlink(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_MKDIR:
      परिणाम = ssh_state_sftp_quote_mkdir(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_RENAME:
      परिणाम = ssh_state_sftp_quote_rename(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_RMDIR:
      परिणाम = ssh_state_sftp_quote_rmdir(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_UNLINK:
      परिणाम = ssh_state_sftp_quote_unlink(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_QUOTE_STATVFS:
      परिणाम = ssh_state_sftp_quote_statvfs(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_GETINFO:
      परिणाम = ssh_state_sftp_getinfo(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_FILETIME:
      परिणाम = ssh_state_sftp_filetime(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_TRANS_INIT:
      परिणाम = ssh_state_sftp_trans_init(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_UPLOAD_INIT:
      परिणाम = ssh_state_sftp_upload_init(दत्त, sshc, sshp, खण्डम्);
      भङ्ग;

    पक्ष SSH_SFTP_CREATE_DIRS_INIT:
      परिणाम = ssh_state_sftp_create_dirs_init(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_CREATE_DIRS:
      परिणाम = ssh_state_sftp_create_dirs(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_CREATE_DIRS_MKDIR:
      परिणाम = ssh_state_sftp_create_dirs_mkdir(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR_INIT:
      परिणाम = ssh_state_sftp_readdir_init(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR:
      परिणाम = ssh_state_sftp_readdir(दत्त, sshc, sshp, खण्डम्);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR_LINK:
      परिणाम = ssh_state_sftp_readdir_link(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR_BOTTOM:
      परिणाम = ssh_state_sftp_readdir_bottom(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_READDIR_समाप्तम्:
      परिणाम = ssh_state_sftp_readdir_समाप्तम्(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SFTP_DOWNLOAD_INIT:
      परिणाम = ssh_state_sftp_download_init(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_DOWNLOAD_STAT:
      परिणाम = ssh_state_sftp_download_stat(दत्त, sshc, sshp, खण्डम्);
      भङ्ग;

    पक्ष SSH_SFTP_CLOSE:
      परिणाम = ssh_state_sftp_close(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SFTP_SHUTDOWN:
      परिणाम = ssh_state_sftp_shutdown(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SCP_TRANS_INIT:
      परिणाम = ssh_state_scp_trans_init(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SCP_UPLOAD_INIT:
      परिणाम = ssh_state_scp_upload_init(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SCP_DOWNLOAD_INIT:
      परिणाम = ssh_state_scp_download_init(दत्त, sshc, sshp);
      भङ्ग;

    पक्ष SSH_SCP_समाप्तम्:
      परिणाम = ssh_state_scp_समाप्तम्(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SCP_SEND_EOF:
      परिणाम = ssh_state_scp_send_eof(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SCP_WAIT_EOF:
      परिणाम = ssh_state_scp_wait_eof(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SCP_WAIT_CLOSE:
      परिणाम = ssh_state_scp_wait_close(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SCP_CHANNEL_FREE:
      परिणाम = ssh_state_scp_channel_free(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SESSION_DISCONNECT:
      परिणाम = ssh_state_session_disconnect(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_SESSION_FREE:
      परिणाम = ssh_state_session_free(दत्त, sshc);
      भङ्ग;

    पक्ष SSH_QUIT:
    स्वभाव:
      /* internal error */
      myssh_गन्तव्यम्(दत्त, sshc, SSH_STOP);
      भङ्ग;
    }

  } यावत्(!परिणाम && (sshc->state != SSH_STOP) && !*खण्डम्);

  यदि(परिणाम == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *खण्डम् = TRUE;
    परिणाम = CURLE_OK;
  }
  CURL_TRC_SSH(दत्त, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (पूर्णाङ्क)परिणाम, *खण्डम्);

  प्रत्यागम परिणाम;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
स्थिर शून्य ssh_block2waitfor(संरचना Curl_easy *दत्त,
                              संरचना ssh_conn *sshc,
                              bool खण्डम्)
{
  पूर्णाङ्क dir = 0;
  (शून्य)दत्त;
  यदि(खण्डम्) {
    dir = libssh2_session_खण्डम्_directions(sshc->ssh_session);
    यदि(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_खण्डम्_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_खण्डम्_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  यदि(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode ssh_multi_statemach(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(दत्त, CURL_META_SSH_EASY);
  CURLcode परिणाम = CURLE_OK;
  bool खण्डम्; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  यदि(!sshc || !sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  कुरु {
    परिणाम = ssh_statemachine(दत्त, sshc, sshp, &खण्डम्);
    *समाप्तम् = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } यावत्(!परिणाम && !*समाप्तम् && !खण्डम्);
  ssh_block2waitfor(दत्त, sshc, खण्डम्);

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssh_खण्डम्_statemach(संरचना Curl_easy *दत्त,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना curltime start = *Curl_pgrs_now(दत्त);

  यावत्((sshc->state != SSH_STOP) && !परिणाम) {
    bool खण्डम्;
    timediff_t वामः_ms = 1000;

    परिणाम = ssh_statemachine(दत्त, sshc, sshp, &खण्डम्);
    यदि(परिणाम)
      भङ्ग;

    यदि(!disconnect) {
      परिणाम = Curl_pgrsCheck(दत्त);
      यदि(परिणाम)
        भङ्ग;

      वामः_ms = Curl_timeleft_ms(दत्त);
      यदि(वामः_ms < 0) {
        failf(दत्त, "Operation timed out");
        प्रत्यागम CURLE_OPERATION_TIMEDOUT;
      }
    }
    अन्यथा यदि(curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &start) > 1000) {
      /* disconnect timeout */
      failf(दत्त, "Disconnect timed out");
      परिणाम = CURLE_OK;
      भङ्ग;
    }

    यदि(खण्डम्) {
      पूर्णाङ्क dir = libssh2_session_खण्डम्_directions(sshc->ssh_session);
      curl_socket_t sock = दत्त->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      यदि(LIBSSH2_SESSION_खण्डम्_INBOUND & dir)
        fd_read = sock;
      यदि(LIBSSH2_SESSION_खण्डम्_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (शून्य)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!वामः_ms || वामः_ms > 1000) ? 1000 : वामः_ms);
    }
  }

  प्रत्यागम परिणाम;
}

स्थिर शून्य myssh_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना SSHPROTO *sshp = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curlx_safefree(sshp->मार्गः);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

स्थिर शून्य myssh_conn_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना ssh_conn *sshc = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  sshc_cleanup(sshc, रिक्त, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
स्थिर CURLcode ssh_सज्जता_connection(संरचना Curl_easy *दत्त,
                                     संरचना connectdata *conn)
{
  संरचना ssh_conn *sshc;
  संरचना SSHPROTO *sshp;
  (शून्य)conn;

  sshc = curlx_calloc(1, परिमाण(*sshc));
  यदि(!sshc)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(Curl_conn_meta_समुच्चयः(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  sshp = curlx_calloc(1, परिमाण(*sshp));
  यदि(!sshp)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  curlx_dyn_init(&sshp->readdir, CURL_मार्गः_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_मार्गः_MAX);
  यदि(Curl_meta_समुच्चयः(दत्त, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रत्यागम Curl_ssh_सज्जता_pkey(दत्त, sshc);
}

स्थिर Curl_recv scp_recv, sftp_recv;
स्थिर Curl_send scp_send, sftp_send;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
स्थिर ssize_t ssh_tls_recv(libssh2_socket_t sock, शून्य *बफर,
                            size_t दीर्घता, पूर्णाङ्क flags, शून्य **abstract)
{
  संरचना Curl_easy *दत्त = (संरचना Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(दत्त, sock);
  size_t nread;
  CURLcode परिणाम;
  संरचना connectdata *conn = दत्त->conn;
  Curl_recv *backup = conn->recv[sockindex];
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (शून्य)flags;

  यदि(!sshc)
    प्रत्यागम -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  परिणाम = Curl_conn_recv(दत्त, sockindex, बफर, दीर्घता, &nread);
  conn->recv[sockindex] = backup;
  यदि(परिणाम == CURLE_AGAIN)
    प्रत्यागम -EAGAIN; /* magic return code for libssh2 */
  अन्यथा यदि(परिणाम)
    प्रत्यागम -1; /* generic error */
  Curl_debug(दत्त, CURLINFO_दत्त_अन्तर्, (ध्रुव वर्ण *)बफर, nread);
  प्रत्यागम (ssize_t)nread;
}

स्थिर ssize_t ssh_tls_send(libssh2_socket_t sock, ध्रुव शून्य *बफर,
                            size_t दीर्घता, पूर्णाङ्क flags, शून्य **abstract)
{
  संरचना Curl_easy *दत्त = (संरचना Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(दत्त, sock);
  size_t nwrite;
  CURLcode परिणाम;
  संरचना connectdata *conn = दत्त->conn;
  Curl_send *backup = conn->send[sockindex];
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (शून्य)flags;

  यदि(!sshc)
    प्रत्यागम -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  परिणाम = Curl_conn_send(दत्त, sockindex, बफर, दीर्घता, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  यदि(परिणाम == CURLE_AGAIN)
    प्रत्यागम -EAGAIN; /* magic return code for libssh2 */
  अन्यथा यदि(परिणाम)
    प्रत्यागम -1; /* error */
  Curl_debug(दत्त, CURLINFO_दत्त_बहिर्गमनम्, (ध्रुव वर्ण *)बफर, nwrite);
  प्रत्यागम (ssize_t)nwrite;
}
#यदि_अन्तः

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
स्थिर CURLcode ssh_connect(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
#यदि_परिभाषितम् CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#यदि_अन्तः
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode परिणाम;

#यदि LIBSSH2_संस्करणम्_अङ्कः >= 0x010b00
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
      infof(दत्त, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#यदि_अन्तः

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  infof(दत्त, "SSH: user '%s'", Curl_creds_user(conn->creds));
#यदि_परिभाषितम् CURL_LIBSSH2_DEBUG
  infof(दत्त, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#यदि_अन्तः /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, दत्त);

  यदि(!sshc->ssh_session) {
    failf(दत्त, "Failure initializing ssh session");
    प्रत्यागम CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#यदि LIBSSH2_संस्करणम्_अङ्कः >= 0x010B00
  यदि(दत्त->समुच्चयः.server_response_timeout > 0) {
    libssh2_session_समुच्चयः_read_timeout(sshc->ssh_session,
                             (दीर्घ)(दत्त->समुच्चयः.server_response_timeout / 1000));
  }
#यदि_अन्तः

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  यदि(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#यदि LIBSSH2_संस्करणम्_अङ्कः >= 0x010b01
    infof(दत्त, "SSH: using HTTPS proxy");
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wcast-function-type-strict"
#यदि_अन्तः
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
#अन्यथा
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

    infof(दत्त, "SSH: using HTTPS proxy");
    libssh2_session_callback_समुच्चयः(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_समुच्चयः(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#यदि_अन्तः

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#यदि_अन्तः /* CURL_DISABLE_PROXY */
  यदि(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  अन्यथा {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  यदि(दत्त->समुच्चयः.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(दत्त, "SSH: failed to enable compression for session");
  }

  यदि(CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS)) {
    पूर्णाङ्क rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    यदि(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = रिक्त;
      प्रत्यागम CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    यदि(rc < 0)
      infof(दत्त, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(दत्त, STRING_SSH_KNOWNHOSTS));
  }

#यदि_परिभाषितम् CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(दत्त, "SSH: socket %d", (पूर्णाङ्क)sock);
#यदि_अन्तः

  myssh_गन्तव्यम्(दत्त, sshc, SSH_INIT);

  परिणाम = ssh_multi_statemach(दत्त, समाप्तम्);

  प्रत्यागम परिणाम;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

स्थिर CURLcode scp_perform(संरचना Curl_easy *दत्त,
                            bool *connected,
                            bool *dophase_समाप्तम्)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(दत्त->conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_SSH(दत्त, "DO phase starts");

  *dophase_समाप्तम् = FALSE; /* not done yet */
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  परिणाम = ssh_multi_statemach(दत्त, dophase_समाप्तम्);

  *connected = Curl_conn_is_connected(दत्त->conn, FIRSTSOCKET);

  यदि(*dophase_समाप्तम्) {
    CURL_TRC_SSH(दत्त, "DO phase is complete");
  }

  प्रत्यागम परिणाम;
}

/* called from multi.c while DOing */
स्थिर CURLcode scp_doing(संरचना Curl_easy *दत्त,
                          bool *dophase_समाप्तम्)
{
  CURLcode परिणाम;
  परिणाम = ssh_multi_statemach(दत्त, dophase_समाप्तम्);

  यदि(*dophase_समाप्तम्) {
    CURL_TRC_SSH(दत्त, "DO phase is complete");
  }
  प्रत्यागम परिणाम;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode scp_disconnect(संरचना Curl_easy *दत्त,
                               संरचना connectdata *conn,
                               bool dead_connection)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(दत्त, CURL_META_SSH_EASY);
  (शून्य)dead_connection;

  यदि(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SESSION_DISCONNECT);
    परिणाम = ssh_खण्डम्_statemach(दत्त, sshc, sshp, TRUE);
  }

  यदि(sshc)
    प्रत्यागम sshc_cleanup(sshc, दत्त, TRUE);
  प्रत्यागम परिणाम;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
स्थिर CURLcode ssh_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(दत्त->conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(दत्त, CURL_META_SSH_EASY);
  CURLcode परिणाम = CURLE_OK;

  यदि(!sshc || !sshp)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!स्थितिः)
    /* run the state-machine */
    परिणाम = ssh_खण्डम्_statemach(दत्त, sshc, sshp, FALSE);
  अन्यथा
    परिणाम = स्थितिः;

  यदि(Curl_pgrs_समाप्तम्(दत्त, Curl_pgrs_now(दत्त)))
    प्रत्यागम CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(दत्त);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode scp_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः,
                         bool premature)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(दत्त->conn, CURL_META_SSH_CONN);
  (शून्य)premature;

  यदि(sshc && !स्थितिः)
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SCP_समाप्तम्);

  प्रत्यागम ssh_समाप्तम्(दत्त, स्थितिः);
}

स्थिर CURLcode scp_send(संरचना Curl_easy *दत्त, int8_t sockindex,
                         ध्रुव uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;
  ssize_t nwritten;

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  (शून्य)eos;
  *pnwritten = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ध्रुव वर्ण *)mem, len);

  ssh_block2waitfor(दत्त, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  यदि(nwritten == LIBSSH2_ERROR_EAGAIN)
    परिणाम = CURLE_AGAIN;
  अन्यथा यदि(nwritten < LIBSSH2_ERROR_NONE)
    परिणाम = libssh2_session_error_गन्तव्यम्_CURLE((पूर्णाङ्क)nwritten);
  अन्यथा
    *pnwritten = (size_t)nwritten;

  प्रत्यागम परिणाम;
}

स्थिर CURLcode scp_recv(संरचना Curl_easy *दत्त, int8_t sockindex,
                         वर्ण *mem, size_t len, size_t *pnread)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;
  ssize_t nread;

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(दत्त, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  यदि(nread == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;
  अन्यथा यदि(nread < LIBSSH2_ERROR_NONE)
    परिणाम = libssh2_session_error_गन्तव्यम्_CURLE((पूर्णाङ्क)nread);
  अन्यथा
    *pnread = (size_t)nread;

  प्रत्यागम परिणाम;
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

स्थिर CURLcode sftp_perform(संरचना Curl_easy *दत्त,
                             bool *connected,
                             bool *dophase_समाप्तम्)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(दत्त->conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_SSH(दत्त, "DO phase starts");

  *dophase_समाप्तम् = FALSE; /* not done yet */
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  परिणाम = ssh_multi_statemach(दत्त, dophase_समाप्तम्);

  *connected = Curl_conn_is_connected(दत्त->conn, FIRSTSOCKET);

  यदि(*dophase_समाप्तम्) {
    CURL_TRC_SSH(दत्त, "DO phase is complete");
  }

  प्रत्यागम परिणाम;
}

/* called from multi.c while DOing */
स्थिर CURLcode sftp_doing(संरचना Curl_easy *दत्त,
                           bool *dophase_समाप्तम्)
{
  CURLcode परिणाम = ssh_multi_statemach(दत्त, dophase_समाप्तम्);

  यदि(*dophase_समाप्तम्) {
    CURL_TRC_SSH(दत्त, "DO phase is complete");
  }
  प्रत्यागम परिणाम;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode sftp_disconnect(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn, bool dead_connection)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(दत्त, CURL_META_SSH_EASY);
  (शून्य)dead_connection;

  यदि(sshc) {
    यदि(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(दत्त, "DISCONNECT starts now");
      myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_SHUTDOWN);
      परिणाम = ssh_खण्डम्_statemach(दत्त, sshc, sshp, TRUE);
      CURL_TRC_SSH(दत्त, "DISCONNECT is done -> %d", (पूर्णाङ्क)परिणाम);
    }
    sshc_cleanup(sshc, दत्त, TRUE);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode sftp_समाप्तम्(संरचना Curl_easy *दत्त, CURLcode स्थितिः,
                          bool premature)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  यदि(!स्थितिः) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    यदि(!premature && दत्त->समुच्चयः.postquote && !conn->द्वयङ्काः.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_गन्तव्यम्(दत्त, sshc, SSH_SFTP_CLOSE);
  }
  प्रत्यागम ssh_समाप्तम्(दत्त, स्थितिः);
}

/* return number of sent bytes */
स्थिर CURLcode sftp_send(संरचना Curl_easy *दत्त, int8_t sockindex,
                          ध्रुव uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (शून्य)sockindex;
  (शून्य)eos;
  *pnwritten = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ध्रुव वर्ण *)mem, len);

  ssh_block2waitfor(दत्त, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  यदि(nwrite == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;
  अन्यथा यदि(nwrite < LIBSSH2_ERROR_NONE)
    प्रत्यागम libssh2_session_error_गन्तव्यम्_CURLE((पूर्णाङ्क)nwrite);
  *pnwritten = (size_t)nwrite;
  प्रत्यागम CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
स्थिर CURLcode sftp_recv(संरचना Curl_easy *दत्त, int8_t sockindex,
                          वर्ण *mem, size_t len, size_t *pnread)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (शून्य)sockindex;
  *pnread = 0;

  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(दत्त, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  यदि(nread == LIBSSH2_ERROR_EAGAIN)
    प्रत्यागम CURLE_AGAIN;
  अन्यथा यदि(nread < 0)
    प्रत्यागम libssh2_session_error_गन्तव्यम्_CURLE((पूर्णाङ्क)nread);

  *pnread = (size_t)nread;
  प्रत्यागम CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
स्थिर CURLcode ssh_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम;
  bool connected = FALSE;
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *समाप्तम् = FALSE; /* default to false */
  यदि(!sshc)
    प्रत्यागम CURLE_FAILED_INIT;

  दत्त->req.आकार = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(दत्त);

  यदि(conn->scheme->protocol & CURLPROTO_SCP)
    परिणाम = scp_perform(दत्त, &connected, समाप्तम्);
  अन्यथा
    परिणाम = sftp_perform(दत्त, &connected, समाप्तम्);

  प्रत्यागम परिणाम;
}

CURLcode Curl_ssh_init(शून्य)
{
  यदि(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    प्रत्यागम CURLE_FAILED_INIT;
  }
  प्रत्यागम CURLE_OK;
}

शून्य Curl_ssh_cleanup(शून्य)
{
  libssh2_exit();
}

शून्य Curl_ssh_संस्करणम्(वर्ण *बफर, size_t buflen)
{
  (शून्य)curl_msnprintf(बफर, buflen, "libssh2/%s", libssh2_संस्करणम्(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
स्थिर शून्य ssh_attach(संरचना Curl_easy *दत्त, संरचना connectdata *conn)
{
  DEBUGASSERT(दत्त);
  DEBUGASSERT(conn);
  यदि(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    यदि(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      शून्य **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = दत्त;
    }
  }
}

/*
 * SCP protocol handler.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_scp = {
  ssh_सज्जता_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_समाप्तम्,                             /* done */
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
  ssh_सज्जता_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  sftp_समाप्तम्,                            /* done */
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

#यदि_अन्तः /* USE_LIBSSH2 */
