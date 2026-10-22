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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef __VMS
#অন্তর্ভাব <in.h>
#অন্তর্ভাব <inet.h>
#endif

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "vssh/ssh.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "parsedate.h" /* for the week day and month names */
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "vssh/vssh.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/base64.h" /* for curlx_base64_encode() */

স্থির ধ্রুবক অক্ষর *sftp_libssh2_strerror(unsigned long err)
{
  নির্বাচন(err) {
  ক্ষেত্র LIBSSH2_FX_NO_SUCH_FILE:
    ফেরত "No such file or directory";

  ক্ষেত্র LIBSSH2_FX_PERMISSION_DENIED:
    ফেরত "Permission denied";

  ক্ষেত্র LIBSSH2_FX_FAILURE:
    ফেরত "Operation failed";

  ক্ষেত্র LIBSSH2_FX_BAD_MESSAGE:
    ফেরত "Bad message from SFTP server";

  ক্ষেত্র LIBSSH2_FX_NO_CONNECTION:
    ফেরত "Not connected to SFTP server";

  ক্ষেত্র LIBSSH2_FX_CONNECTION_LOST:
    ফেরত "Connection to SFTP server lost";

  ক্ষেত্র LIBSSH2_FX_OP_UNSUPPORTED:
    ফেরত "Operation not supported by SFTP server";

  ক্ষেত্র LIBSSH2_FX_INVALID_HANDLE:
    ফেরত "Invalid handle";

  ক্ষেত্র LIBSSH2_FX_NO_SUCH_PATH:
    ফেরত "No such file or directory";

  ক্ষেত্র LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ফেরত "File already exists";

  ক্ষেত্র LIBSSH2_FX_WRITE_PROTECT:
    ফেরত "File is write protected";

  ক্ষেত্র LIBSSH2_FX_NO_MEDIA:
    ফেরত "No media";

  ক্ষেত্র LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    ফেরত "Disk full";

  ক্ষেত্র LIBSSH2_FX_QUOTA_EXCEEDED:
    ফেরত "User quota exceeded";

  ক্ষেত্র LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    ফেরত "Unknown principal";

  ক্ষেত্র LIBSSH2_FX_LOCK_CONFLICT:
    ফেরত "File lock conflict";

  ক্ষেত্র LIBSSH2_FX_DIR_NOT_EMPTY:
    ফেরত "Directory not empty";

  ক্ষেত্র LIBSSH2_FX_NOT_A_DIRECTORY:
    ফেরত "Not a directory";

  ক্ষেত্র LIBSSH2_FX_INVALID_FILENAME:
    ফেরত "Invalid filename";

  ক্ষেত্র LIBSSH2_FX_LINK_LOOP:
    ফেরত "Link points to itself";
  }
  ফেরত "Unknown error in libssh2";
}

স্থির শূন্য kbd_callback(ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা name_len,
                         ধ্রুবক অক্ষর *instruction, পূর্ণসংখ্যা instruction_len,
                         পূর্ণসংখ্যা অঙ্ক_prompts,
                         ধ্রুবক LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         শূন্য **abstract)
{
  কাঠামো Curl_easy *তথ্য = (কাঠামো Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", অঙ্ক_prompts);
#অন্যথা
  (শূন্য)name;
  (শূন্য)name_len;
  (শূন্য)instruction;
  (শূন্য)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  যদি(অঙ্ক_prompts == 1) {
    কাঠামো connectdata *conn = তথ্য->conn;
    ধ্রুবক অক্ষর *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].দৈর্ঘ্য = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (শূন্য)prompts;
} /* kbd_callback */

স্থির CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  নির্বাচন(err) {
  ক্ষেত্র LIBSSH2_FX_OK:
    ফেরত CURLE_OK;

  ক্ষেত্র LIBSSH2_FX_NO_SUCH_FILE:
  ক্ষেত্র LIBSSH2_FX_NO_SUCH_PATH:
    ফেরত CURLE_REMOTE_FILE_NOT_FOUND;

  ক্ষেত্র LIBSSH2_FX_PERMISSION_DENIED:
  ক্ষেত্র LIBSSH2_FX_WRITE_PROTECT:
  ক্ষেত্র LIBSSH2_FX_LOCK_CONFLICT:
    ফেরত CURLE_REMOTE_ACCESS_DENIED;

  ক্ষেত্র LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  ক্ষেত্র LIBSSH2_FX_QUOTA_EXCEEDED:
    ফেরত CURLE_REMOTE_DISK_FULL;

  ক্ষেত্র LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ফেরত CURLE_REMOTE_FILE_EXISTS;

  ক্ষেত্র LIBSSH2_FX_DIR_NOT_EMPTY:
    ফেরত CURLE_QUOTE_ERROR;

  পূর্বনির্ধারিত:
    বিরতি;
  }

  ফেরত CURLE_SSH;
}

স্থির CURLcode libssh2_session_error_to_CURLE(পূর্ণসংখ্যা err)
{
  নির্বাচন(err) {
  /* Ordered by order of appearance in libssh2.h */
  ক্ষেত্র LIBSSH2_ERROR_NONE:
    ফেরত CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  ক্ষেত্র LIBSSH2_ERROR_SCP_PROTOCOL:
    ফেরত CURLE_REMOTE_FILE_NOT_FOUND;

  ক্ষেত্র LIBSSH2_ERROR_SOCKET_NONE:
    ফেরত CURLE_COULDNT_CONNECT;

  ক্ষেত্র LIBSSH2_ERROR_ALLOC:
    ফেরত CURLE_OUT_OF_MEMORY;

  ক্ষেত্র LIBSSH2_ERROR_SOCKET_SEND:
    ফেরত CURLE_SEND_ERROR;

  ক্ষেত্র LIBSSH2_ERROR_HOSTKEY_INIT:
  ক্ষেত্র LIBSSH2_ERROR_HOSTKEY_SIGN:
  ক্ষেত্র LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  ক্ষেত্র LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    ফেরত CURLE_PEER_FAILED_VERIFICATION;

  ক্ষেত্র LIBSSH2_ERROR_PASSWORD_EXPIRED:
    ফেরত CURLE_LOGIN_DENIED;

  ক্ষেত্র LIBSSH2_ERROR_SOCKET_TIMEOUT:
  ক্ষেত্র LIBSSH2_ERROR_TIMEOUT:
    ফেরত CURLE_OPERATION_TIMEDOUT;

  ক্ষেত্র LIBSSH2_ERROR_EAGAIN:
    ফেরত CURLE_AGAIN;
  }

  ফেরত CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

স্থির LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (শূন্য)abstract;
  ফেরত Curl_cmalloc(গুনতি);
}

স্থির LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (শূন্য)abstract;
  ফেরত Curl_crealloc(ptr, গুনতি);
}

স্থির LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (শূন্য)abstract;
  যদি(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

স্থির পূর্ণসংখ্যা sshkeycallback(CURL *easy,
                          ধ্রুবক কাঠামো curl_khkey *knownkey, /* known */
                          ধ্রুবক কাঠামো curl_khkey *foundkey, /* found */
                          গণনা curl_khmatch match,
                          শূন্য *clientp)
{
  (শূন্য)easy;
  (শূন্য)knownkey;
  (শূন্য)foundkey;
  (শূন্য)clientp;

  /* we only allow perfect matches, and we reject everything else */
  ফেরত (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

স্থির গণনা curl_khtype convert_ssh2_keytype(পূর্ণসংখ্যা sshkeytype)
{
  গণনা curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  নির্বাচন(sshkeytype) {
  ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    বিরতি;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    বিরতি;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    বিরতি;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    বিরতি;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    বিরতি;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    বিরতি;
#endif
  }
  ফেরত keytype;
}

স্থির CURLcode ssh_knownhost(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ssh_conn *sshc)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো libssh2_knownhost *host = ফাঁকা;
  ধ্রুবক অক্ষর *remotekey = ফাঁকা;
  পূর্ণসংখ্যা keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  পূর্ণসংখ্যা keybit = 0;
  পূর্ণসংখ্যা sshkeytype = 0;
  size_t keylen = 0;
  পূর্ণসংখ্যা rc = 0;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS)) {
    infof(তথ্য, "SSH: no knownhosts file configured");
    ফেরত CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  যদি(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    কাঠামো Curl_mapi_guard guard;
    গণনা curl_khmatch keymatch;
    curl_sshkeycallback func =
      তথ্য->set.ssh_keyfunc ? তথ্য->set.ssh_keyfunc : sshkeycallback;
    কাঠামো curl_khkey knownkey;
    কাঠামো curl_khkey *knownkeyp = ফাঁকা;
    কাঠামো curl_khkey foundkey;

    নির্বাচন(sshkeytype) {
    ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_চাবি_SSHRSA;
      বিরতি;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_চাবি_SSHDSS;
      বিরতি;
#endif
    ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_চাবি_ECDSA_256;
      বিরতি;
    ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_চাবি_ECDSA_384;
      বিরতি;
    ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_চাবি_ECDSA_521;
      বিরতি;
    ক্ষেত্র LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_চাবি_ED25519;
      বিরতি;
    পূর্বনির্ধারিত:
      infof(তথ্য, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      বিরতি;
    }
    যদি(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    অন্যথা {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->port != PORT_SSH) ?
                                          conn->origin->port : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(তথ্য, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->চাবি : "<none>");

      /* setup 'knownkey' */
      যদি(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.চাবি = host->চাবি;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.চাবি = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (গণনা curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, তথ্য, easy_ssh_keyfunc);
      rc = func(তথ্য, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, তথ্য->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  অন্যথা {
    /* no remotekey means failure! */
    infof(তথ্য, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  নির্বাচন(rc) {
  পূর্বনির্ধারিত: /* unknown return codes is the same as reject */
  ক্ষেত্র CURLKHSTAT_REJECT:
    infof(তথ্য, "SSH: knownhost check failed");
    myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  ক্ষেত্র CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    বিরতি;
  ক্ষেত্র CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    যদি(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  ক্ষেত্র CURLKHSTAT_FINE:
  ক্ষেত্র CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    যদি(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      পূর্ণসংখ্যা addrc;
      ধ্রুবক অক্ষর *hostbuf;
      অক্ষর *hostport = ফাঁকা;
      যদি(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        যদি(!hostbuf)
          infof(তথ্য, "WARNING: failed allocating buffer for [host]:port");
      }
      অন্যথা
        hostbuf = conn->origin->hostname;
      যদি(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, ফাঁকা,
                                       remotekey, keylen, ফাঁকা, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, ফাঁকা);
        যদি(addrc)
          infof(তথ্য, "WARNING: adding the known host %s failed", hostbuf);
        অন্যথা যদি(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          পূর্ণসংখ্যা wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          যদি(wrc) {
            infof(তথ্য, "WARNING: writing %s failed",
                  CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    অন্যথা
      infof(তথ্য, "SSH: knownhost entry matches host key");
    বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_check_fingerprint(কাঠামো Curl_easy *তথ্য,
                                      কাঠামো ssh_conn *sshc)
{
  ধ্রুবক অক্ষর *pubkey_md5 =
    CURL_EASY_STR(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_MD5);
  ধ্রুবক অক্ষর *pubkey_sha256 =
    CURL_EASY_STR(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_SHA256);

  যদি(pubkey_sha256) {
    ধ্রুবক অক্ষর *fingerprint = ফাঁকা;
    অক্ষর *fingerprint_b64 = ফাঁকা;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(তথ্য, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    যদি(!fingerprint) {
      failf(তথ্য,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
      ফেরত CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    যদি(curlx_base64_encode((ধ্রুবক uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
      ফেরত CURLE_PEER_FAILED_VERIFICATION;
    }

    যদি(!fingerprint_b64) {
      failf(তথ্য, "SHA256 fingerprint could not be encoded");
      myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
      ফেরত CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(তথ্য, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    যখন((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    যখন((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    যদি((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(তথ্য,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
      ফেরত CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(তথ্য, "SSH: SHA256 checksum match");
  }

  যদি(pubkey_md5) {
    অক্ষর md5buffer[33];
    ধ্রুবক অক্ষর *fingerprint;

    infof(তথ্য, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    যদি(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      পূর্ণসংখ্যা i;
      জন্য(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned অক্ষর)fingerprint[i]);
      }

      infof(তথ্য, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    যদি(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      যদি(fingerprint) {
        failf(তথ্য,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      অন্যথা {
        failf(তথ্য,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
      ফেরত CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(তথ্য, "SSH: MD5 checksum match");
  }

  যদি(!pubkey_md5 && !pubkey_sha256) {
    যদি(তথ্য->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      পূর্ণসংখ্যা sshkeytype = 0;
      পূর্ণসংখ্যা rc = 0;
      /* we handle the process to the callback */
      ধ্রুবক অক্ষর *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      যদি(remotekey) {
        কাঠামো Curl_mapi_guard guard;
        গণনা curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, তথ্য, easy_ssh_hostkeyfunc);
        rc = তথ্য->set.ssh_hostkeyfunc(তথ্য->set.ssh_hostkeyfunc_userp,
                                       (পূর্ণসংখ্যা)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        যদি(rc != CURLKHMATCH_OK) {
          myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
          failf(তথ্য, "SSH: callback failed host public key verification");
          ফেরত CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(তথ্য, "SSH: verified public key via callback");
      }
      অন্যথা {
        myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
        ফেরত CURLE_PEER_FAILED_VERIFICATION;
      }
      ফেরত CURLE_OK;
    }
    অন্যথা {
      CURL_TRC_SSH(তথ্য, "no host key checksum given, checking knownhosts");
      ফেরত ssh_knownhost(তথ্য, sshc);
    }
  }
  অন্যথা {
    /* as we already matched, we skip the check for known hosts */
    ফেরত CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
স্থির CURLcode ssh_force_knownhost_চাবি_type(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো ssh_conn *sshc)
{
  CURLcode ফলাফল = CURLE_OK;

  স্থির ধ্রুবক অক্ষর hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  স্থির ধ্রুবক অক্ষর hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  স্থির ধ্রুবক অক্ষর hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  স্থির ধ্রুবক অক্ষর hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  স্থির ধ্রুবক অক্ষর hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_চাবি_SSHDSS
  স্থির ধ্রুবক অক্ষর hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  যদি(sshc->kh &&
     !CURL_EASY_STR(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_MD5) &&
     !CURL_EASY_STR(তথ্য, STRING_SSH_HOST_PUBLIC_চাবি_SHA256)) {
    কাঠামো libssh2_knownhost *store = ফাঁকা;
    কাঠামো connectdata *conn = তথ্য->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_চাবি_MASK >>
                      LIBSSH2_KNOWNHOST_চাবি_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    যখন(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      যদি(store) {
        যদি(store->name) {
          যদি(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_আকার = 0;
            ধ্রুবক অক্ষর *p;
            ধ্রুবক অক্ষর *kh_name_end = strstr(store->name, "]:");
            যদি(!kh_name_end) {
              infof(তথ্য, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS));
              চালু;
            }
            p = kh_name_end + 2; /* start of port number */
            যদি(!curlx_str_সংখ্যা(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_আকার = strlen(store->name) - 1 - strlen(kh_name_end);
              যদি((kh_name_আকার == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_আকার)) {
                found = TRUE;
                বিরতি;
              }
            }
          }
          অন্যথা যদি(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            বিরতি;
          }
        }
        অন্যথা যদি(store->চাবি) {
          unsigned পূর্ণসংখ্যা type = (store->typemask & LIBSSH2_KNOWNHOST_চাবি_MASK)
                              >> LIBSSH2_KNOWNHOST_চাবি_SHIFT;
          যদি(!absent_type[type]) {
            পূর্ণসংখ্যা keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->চাবি, strlen(store->চাবি),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_চাবি_MASK),
              ফাঁকা);
            যদি(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              বিরতি;
            }
            অন্যথা যদি(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    যদি(found) {
      পূর্ণসংখ্যা rc;
      ধ্রুবক অক্ষর *hostkey_method = ফাঁকা;
      infof(তথ্য, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS));

      নির্বাচন(store->typemask & LIBSSH2_KNOWNHOST_চাবি_MASK) {
      ক্ষেত্র LIBSSH2_KNOWNHOST_চাবি_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        বিরতি;
      ক্ষেত্র LIBSSH2_KNOWNHOST_চাবি_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        বিরতি;
      ক্ষেত্র LIBSSH2_KNOWNHOST_চাবি_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        বিরতি;
      ক্ষেত্র LIBSSH2_KNOWNHOST_চাবি_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        বিরতি;
      ক্ষেত্র LIBSSH2_KNOWNHOST_চাবি_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        বিরতি;
#ifdef LIBSSH2_KNOWNHOST_চাবি_SSHDSS
      ক্ষেত্র LIBSSH2_KNOWNHOST_চাবি_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        বিরতি;
#endif
      ক্ষেত্র LIBSSH2_KNOWNHOST_চাবি_RSA1:
        failf(তথ্য, "Found host key type RSA1 which is not supported");
        ফেরত CURLE_SSH;
      পূর্বনির্ধারিত:
        failf(তথ্য, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_চাবি_MASK));
        ফেরত CURLE_SSH;
      }

      infof(তথ্য, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      যদি(rc) {
        অক্ষর *err_বার্তা = ফাঁকা;
        পূর্ণসংখ্যা errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, &errlen, 0);
        failf(তথ্য, "libssh2 method '%s' failed: %s", hostkey_method, err_বার্তা);
        ফলাফল = libssh2_session_error_to_CURLE(rc);
      }
    }
    অন্যথা {
      infof(তথ্য, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS));
    }
  }

  ফেরত ফলাফল;
}

স্থির CURLcode quote_error(কাঠামো Curl_easy *তথ্য,
                            কাঠামো ssh_conn *sshc)
{
  failf(তথ্য, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ফেরত CURLE_QUOTE_ERROR;
}

স্থির CURLcode sftp_quote(কাঠামো Curl_easy *তথ্য,
                           কাঠামো ssh_conn *sshc,
                           কাঠামো SSHPROTO *sshp)
{
  ধ্রুবক অক্ষর *cp;
  CURLcode ফলাফল = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ধ্রুবক অক্ষর *cmd = sshc->quote_item->তথ্য;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  যদি(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  যদি(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    অক্ষর *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    যদি(!tmp)
      ফেরত CURLE_OUT_OF_MEMORY;
    Curl_debug(তথ্য, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(তথ্য, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    যদি(!ফলাফল)
      myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
    ফেরত ফলাফল;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  যদি(!cp) {
    failf(তথ্য, "Syntax error command '%s', missing parameter", cmd);
    ফেরত CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  ফলাফল = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  যদি(ফলাফল) {
    যদি(ফলাফল != CURLE_OUT_OF_MEMORY)
      failf(তথ্য, "Syntax error: Bad first parameter to '%s'", cmd);
    ফেরত ফলাফল;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  যদি(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    ফলাফল = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    যদি(ফলাফল) {
      যদি(ফলাফল != CURLE_OUT_OF_MEMORY)
        failf(তথ্য, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      ফেরত ফলাফল;
    }
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);

    memset(&sshp->quote_attrs, 0, আকার(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_STAT);
    ফেরত ফলাফল;
  }
  যদি(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    ফলাফল = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    যদি(ফলাফল) {
      যদি(ফলাফল != CURLE_OUT_OF_MEMORY)
        failf(তথ্য, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ফেরত ফলাফল;
    }
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ফেরত ফলাফল;
  }
  অন্যথা যদি(!strncmp(cmd, "mkdir ", 6)) {
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    /* create directory */
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_MKDIR);
    ফেরত ফলাফল;
  }
  অন্যথা যদি(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    ফলাফল = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    যদি(ফলাফল) {
      যদি(ফলাফল != CURLE_OUT_OF_MEMORY)
        failf(তথ্য, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ফেরত ফলাফল;
    }
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_RENAME);
    ফেরত ফলাফল;
  }
  অন্যথা যদি(!strncmp(cmd, "rmdir ", 6)) {
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    /* delete directory */
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_RMDIR);
    ফেরত ফলাফল;
  }
  অন্যথা যদি(!strncmp(cmd, "rm ", 3)) {
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_UNLINK);
    ফেরত ফলাফল;
  }
  অন্যথা যদি(!strncmp(cmd, "statvfs ", 8)) {
    যদি(*cp)
      ফেরত quote_error(তথ্য, sshc);
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_STATVFS);
    ফেরত ফলাফল;
  }

  failf(তথ্য, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ফেরত CURLE_QUOTE_ERROR;
}

স্থির CURLcode sftp_upload_init(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ssh_conn *sshc,
                                 কাঠামো SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  যদি(তথ্য->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    যদি(তথ্য->state.resume_from < 0) {
      পূর্ণসংখ্যা rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      যদি(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        ফেরত CURLE_OK;
      }
      যদি(rc) {
        তথ্য->state.resume_from = 0;
      }
      অন্যথা {
        curl_off_t আকার_2 = attrs.filesize;
        যদি(আকার_2 < 0) {
          failf(তথ্য, "Bad file size (%" FMT_OFF_T ")", আকার_2);
          ফেরত CURLE_BAD_DOWNLOAD_RESUME;
        }
        তথ্য->state.resume_from = attrs.filesize;
      }
    }
  }

  যদি(তথ্য->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  অন্যথা যদি(তথ্য->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  অন্যথা {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)তথ্য->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  যদি(!sshc->sftp_handle) {
    CURLcode ফলাফল;
    unsigned long sftperr;
    পূর্ণসংখ্যা rc = libssh2_session_last_errno(sshc->ssh_session);

    যদি(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      ফেরত CURLE_OK;
    }

    যদি(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    অন্যথা
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    যদি(sshc->secondCreateDirs) {
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      failf(তথ্য, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      ফেরত sftp_libssh2_error_to_CURLE(sftperr);
    }
    যদি((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       তথ্য->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(তথ্য, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ফেরত CURLE_OK;
    }
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    ফলাফল = sftp_libssh2_error_to_CURLE(sftperr);
    যদি(!ফলাফল) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      ফলাফল = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(তথ্য, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    ফেরত ফলাফল;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  যদি(তথ্য->state.resume_from > 0 && !তথ্য->set.remote_append) {
    পূর্ণসংখ্যা seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    যদি(তথ্য->set.seek_func) {
      কাঠামো Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, তথ্য, easy_seek_func);
      seekerr = তথ্য->set.seek_func(তথ্য->set.seek_client,
                                    তথ্য->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    যদি(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      যদি(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(তথ্য, "Could not seek stream");
        ফেরত CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      করো {
        কাঠামো Curl_mapi_guard guard;
        অক্ষর scratch[4 * 1024];
        size_t readthisamountnow =
          (তথ্য->state.resume_from - passed >
           (curl_off_t)আকার(scratch)) ?
          আকার(scratch) : curlx_sotouz(তথ্য->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, তথ্য, easy_fread_func);
        actuallyread = তথ্য->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              তথ্য->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        যদি((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(তথ্য, "Failed to read data");
          ফেরত CURLE_FTP_COULDNT_USE_REST;
        }
      } যখন(passed < তথ্য->state.resume_from);
    }

    /* now, decrease the size of the read */
    যদি(তথ্য->state.infilesize > 0) {
      তথ্য->state.infilesize -= তথ্য->state.resume_from;
      তথ্য->req.আকার_2 = তথ্য->state.infilesize;
      Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)তথ্য->state.resume_from);
  }
  যদি(তথ্য->state.infilesize > 0) {
    তথ্য->req.আকার_2 = তথ্য->state.infilesize;
    Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);
  }
  /* upload data */
  Curl_xfer_setup_send(তথ্য, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  তথ্য->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(তথ্য);

  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত CURLE_OK;
}

স্থির শূন্য ssh_state_pkey_init(কাঠামো Curl_easy *তথ্য, কাঠামো ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  যদি((তথ্য->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    যদি(sshc->pub_চাবি)
      infof(তথ্য, "SSH: trying public key file '%s'", sshc->pub_চাবি);
    infof(তথ্য, "SSH: trying private key file '%s'", sshc->priv_চাবি);
    myssh_to(তথ্য, sshc, SSH_AUTH_PKEY);
  }
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_AUTH_PASS_INIT);
}

স্থির CURLcode sftp_quote_stat(কাঠামো Curl_easy *তথ্য,
                                কাঠামো ssh_conn *sshc,
                                কাঠামো SSHPROTO *sshp,
                                bool *blockp)
{
  ধ্রুবক অক্ষর *cmd = sshc->quote_item->তথ্য;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  যদি(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  যদি(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    পূর্ণসংখ্যা rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    যদি(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      ফেরত CURLE_OK;
    }
    যদি(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(তথ্য, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      যাও fail;
    }
  }

  /* Now set the new attributes... */
  যদি(!strncmp(cmd, "chgrp", 5)) {
    ধ্রুবক অক্ষর *p = sshc->quote_path1;
    curl_off_t gid;
    যদি(!curlx_str_সংখ্যা(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    অন্যথা যদি(!sshc->acceptfail) {
      failf(তথ্য, "Syntax error: chgrp gid not a number");
      যাও fail;
    }
  }
  অন্যথা যদি(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ধ্রুবক অক্ষর *p = sshc->quote_path1;
    /* permissions are octal */
    যদি(curlx_str_octal(&p, &perms, 07777)) {
      failf(তথ্য, "Syntax error: chmod permissions not a number");
      যাও fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  অন্যথা যদি(!strncmp(cmd, "chown", 5)) {
    ধ্রুবক অক্ষর *p = sshc->quote_path1;
    curl_off_t uid;
    যদি(!curlx_str_সংখ্যা(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    অন্যথা যদি(!sshc->acceptfail) {
      failf(তথ্য, "Syntax error: chown uid not a number");
      যাও fail;
    }
  }
  অন্যথা যদি(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    যদি(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(তথ্য, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#যদি SIZEOF_TIME_T > SIZEOF_LONG
    যদি(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(তথ্য, "date overflow");
      fail = TRUE;
    }
#endif
    যদি(fail)
      যাও fail;
    যদি(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    অন্যথা /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ফেরত CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ফেরত CURLE_QUOTE_ERROR;
}

স্থির CURLcode sftp_download_stat(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ssh_conn *sshc,
                                   কাঠামো SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  পূর্ণসংখ্যা rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  যদি(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ফেরত CURLE_OK;
  }
  যদি(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_আকার) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    তথ্য->req.আকার_2 = -1;
    তথ্য->req.maxdownload = -1;
    Curl_pgrs_Set_Download_আকার(তথ্য, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  অন্যথা {
    curl_off_t আকার_2 = attrs.filesize;

    যদি(আকার_2 < 0) {
      failf(তথ্য, "Bad file size (%" FMT_OFF_T ")", আকার_2);
      ফেরত CURLE_BAD_DOWNLOAD_RESUME;
    }
    যদি(তথ্য->state.use_range) {
      curl_off_t from;
      CURLcode ফলাফল = Curl_ssh_range(তথ্য, তথ্য->state.range, আকার_2,
                                       &from, &আকার_2);
      যদি(ফলাফল)
        ফেরত ফলাফল;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    তথ্য->req.আকার_2 = আকার_2;
    তথ্য->req.maxdownload = আকার_2;
    Curl_pgrs_Set_Download_আকার(তথ্য, আকার_2);
  }

  /* We can resume if we can seek to the resume position */
  যদি(তথ্য->state.resume_from) {
    যদি(তথ্য->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      যদি((curl_off_t)attrs.filesize < -তথ্য->state.resume_from) {
        failf(তথ্য, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              তথ্য->state.resume_from, (curl_off_t)attrs.filesize);
        ফেরত CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      তথ্য->state.resume_from += attrs.filesize;
    }
    অন্যথা {
      যদি((curl_off_t)attrs.filesize < তথ্য->state.resume_from) {
        failf(তথ্য, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              তথ্য->state.resume_from, (curl_off_t)attrs.filesize);
        ফেরত CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    তথ্য->req.আকার_2 = attrs.filesize - তথ্য->state.resume_from;
    তথ্য->req.maxdownload = attrs.filesize - তথ্য->state.resume_from;
    Curl_pgrs_Set_Download_আকার(তথ্য, attrs.filesize - তথ্য->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)তথ্য->state.resume_from);
  }

  /* Setup the actual download */
  যদি(তথ্য->req.আকার_2 == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(তথ্য);
    infof(তথ্য, "SSH: file already completely downloaded");
    myssh_to(তথ্য, sshc, SSH_STOP);
    ফেরত CURLE_OK;
  }
  Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, তথ্য->req.আকার_2);

  /* not set by Curl_xfer_setup to preserve io_flags */
  তথ্য->conn->send_idx = 0;

  myssh_to(তথ্য, sshc, SSH_STOP);

  ফেরত CURLE_OK;
}

স্থির CURLcode sftp_readdir(কাঠামো Curl_easy *তথ্য,
                             কাঠামো ssh_conn *sshc,
                             কাঠামো SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  যদি(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ফেরত ফলাফল;
  }
  যদি(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    যদি(তথ্য->set.list_only) {
      ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      যদি(!ফলাফল)
        ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY, "\n", 1);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    অন্যথা {
      ফলাফল = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      যদি(!ফলাফল) {
        যদি((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          ফলাফল = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_LINK);
        }
        অন্যথা {
          myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      ফেরত ফলাফল;
    }
  }
  অন্যথা যদি(!rc) {
    myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_DONE);
  }
  অন্যথা {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    ফলাফল = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(তথ্য, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_init(কাঠামো Curl_easy *তথ্য,
                               কাঠামো ssh_conn *sshc)
{
  CURLcode ফলাফল;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  ফলাফল = ssh_force_knownhost_চাবি_type(তথ্য, sshc);
  যদি(ফলাফল)
    myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_S_STARTUP);
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_startup(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ssh_conn *sshc)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  পূর্ণসংখ্যা rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc) {
    অক্ষর *err_বার্তা = ফাঁকা;
    (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
    failf(তথ্য, "Failure establishing ssh session: %d, %s", rc, err_বার্তা);

    myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
    ফেরত CURLE_FAILED_INIT;
  }

  myssh_to(তথ্য, sshc, SSH_HOSTKEY);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_hostkey(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode ফলাফল = ssh_check_fingerprint(তথ্য, sshc);
  যদি(!ফলাফল)
    myssh_to(তথ্য, sshc, SSH_AUTHLIST);
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_authlist(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ssh_conn *sshc)
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
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক অক্ষর *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(strlen(user)));

  যদি(!sshc->authlist) {
    পূর্ণসংখ্যা rc;
    যদি(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(তথ্য, "SSH: user accepted with no authentication");
      myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
      ফেরত CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
    ফেরত libssh2_session_error_to_CURLE(rc);
  }
  infof(তথ্য, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(তথ্য, sshc, SSH_AUTH_PKEY_INIT);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_auth_pkey(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক অক্ষর *user = Curl_creds_user(conn->creds);
  পূর্ণসংখ্যা rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_চাবি, sshc->priv_চাবি, sshc->passphrase ? sshc->passphrase : "");
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc == 0) {
    sshc->authed = TRUE;
    infof(তথ্য, "SSH: authenticated via publickey");
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
  }
  অন্যথা {
    অক্ষর *err_বার্তা = ফাঁকা;
    অক্ষর unknown[] = "Reason unknown (-1)";
    যদি(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_বার্তা = unknown;
    }
    অন্যথা {
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
    }
    infof(তথ্য, "SSH: publickey authentication denied: %s", err_বার্তা);
    myssh_to(তথ্য, sshc, SSH_AUTH_PASS_INIT);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_auth_pass_init(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো ssh_conn *sshc)
{
  যদি((তথ্য->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(তথ্য, sshc, SSH_AUTH_PASS);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_AUTH_HOST_INIT);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_auth_pass(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক অক্ষর *user = Curl_creds_user(conn->creds);
  ধ্রুবক অক্ষর *passwd = Curl_creds_passwd(conn->creds);
  পূর্ণসংখ্যা rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 ফাঁকা);
  যদি(rc == LIBSSH2_ERROR_EAGAIN) {
    ফেরত CURLE_AGAIN;
  }
  যদি(rc == 0) {
    sshc->authed = TRUE;
    infof(তথ্য, "SSH: initialized password authentication");
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_AUTH_HOST_INIT);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_auth_host_init(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো ssh_conn *sshc)
{
  যদি((তথ্য->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(তথ্য, sshc, SSH_AUTH_HOST);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_AUTH_AGENT_INIT);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_auth_agent_init(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = 0;
  যদি((তথ্য->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(তথ্য, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    যদি(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      যদি(!sshc->ssh_agent) {
        infof(তথ্য, "SSH: could not create agent object");

        myssh_to(তথ্য, sshc, SSH_AUTH_চাবি_INIT);
        ফেরত CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;
    যদি(rc < 0) {
      infof(তথ্য, "SSH: failure connecting to agent");
      myssh_to(তথ্য, sshc, SSH_AUTH_চাবি_INIT);
    }
    অন্যথা {
      myssh_to(তথ্য, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_AUTH_চাবি_INIT);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_auth_agent_list(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = libssh2_agent_list_identities(sshc->ssh_agent);

  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;
  যদি(rc < 0) {
    infof(তথ্য, "SSH: failure requesting identities to agent");
    myssh_to(তথ্য, sshc, SSH_AUTH_চাবি_INIT);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_পূর্ব_identity = ফাঁকা;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_auth_agent(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  পূর্ণসংখ্যা rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_পূর্ব_identity);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc == 0) {
    CURL_TRC_SSH(তথ্য, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(তথ্য->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(তথ্য->conn->creds),
                                sshc->sshagent_identity);

    যদি(rc < 0) {
      যদি(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_পূর্ব_identity = sshc->sshagent_identity;
        ফেরত CURLE_OK;
      }
      ফেরত CURLE_AGAIN;
    }
  }

  যদি(rc < 0)
    infof(তথ্য, "SSH: failure requesting identities to agent");
  অন্যথা যদি(rc == 1)
    infof(তথ্য, "SSH: no agent identity would match");

  যদি(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(তথ্য, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(তথ্য->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_AUTH_চাবি_INIT);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_auth_চাবি_init(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ssh_conn *sshc)
{
  যদি((তথ্য->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(তথ্য, sshc, SSH_AUTH_চাবি);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_auth_চাবি(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  কাঠামো connectdata *conn = তথ্য->conn;
  ধ্রুবক অক্ষর *user = Curl_creds_user(conn->creds);
  পূর্ণসংখ্যা rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc == 0) {
    sshc->authed = TRUE;
    infof(তথ্য, "SSH: initialized keyboard interactive authentication");
    myssh_to(তথ্য, sshc, SSH_AUTH_DONE);
    ফেরত CURLE_OK;
  }
  ফেরত CURLE_LOGIN_DENIED;
}

স্থির CURLcode ssh_state_auth_done(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  যদি(!sshc->authed) {
    failf(তথ্য, "Authentication failure");
    myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
    ফেরত CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(তথ্য, "SSH: authentication complete");

  Curl_pgrsTime(তথ্য, TIMER_APPCONNECT); /* SSH is connected */

  তথ্য->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  যদি(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(তথ্য, sshc, SSH_SFTP_INIT);
    ফেরত CURLE_OK;
  }
  infof(তথ্য, "SSH: connection established");
  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_init(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  যদি(!sshc->sftp_session) {
    অক্ষর *err_বার্তা = ফাঁকা;
    যদি(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
    failf(তথ্য, "Failure initializing sftp session: %s", err_বার্তা);
    myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
    ফেরত CURLE_FAILED_INIT;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_REALPATH);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_realpath(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ssh_conn *sshc,
                                        কাঠামো SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  পূর্ণসংখ্যা rc;

  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  myssh_to(তথ্য, sshc, SSH_STOP);
  যদি(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    যদি(!sshc->homedir)
      ফেরত CURLE_OUT_OF_MEMORY;
    curlx_free(তথ্য->state.most_recent_ftp_entrypath);
    তথ্য->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    যদি(!তথ্য->state.most_recent_ftp_entrypath)
      ফেরত CURLE_OUT_OF_MEMORY;
  }
  অন্যথা {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode ফলাফল;
    যদি(sftperr)
      ফলাফল = sftp_libssh2_error_to_CURLE(sftperr);
    অন্যথা
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      ফলাফল = CURLE_SSH;
    CURL_TRC_SSH(তথ্য, "error = %lu makes libcurl = %d", sftperr, (পূর্ণসংখ্যা)ফলাফল);
    ফেরত ফলাফল;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(তথ্য, "CONNECT phase done");
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_quote_init(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো ssh_conn *sshc,
                                          কাঠামো SSHPROTO *sshp)
{
  CURLcode ফলাফল;

  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = Curl_getworkingpath(তথ্য, sshc->homedir, &sshp->path);
  যদি(ফলাফল) {
    myssh_to(তথ্য, sshc, SSH_STOP);
    ফেরত ফলাফল;
  }

  যদি(তথ্য->set.quote) {
    infof(তথ্য, "SSH: sending quote commands");
    sshc->quote_item = তথ্য->set.quote;
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_SFTP_GETINFO);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_postquote_init(কাঠামো Curl_easy *তথ্য,
                                              কাঠামো ssh_conn *sshc)
{
  যদি(তথ্য->set.postquote) {
    infof(তথ্য, "SSH: sending quote commands");
    sshc->quote_item = তথ্য->set.postquote;
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE);
  }
  অন্যথা {
    myssh_to(তথ্য, sshc, SSH_STOP);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_quote(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো ssh_conn *sshc,
                                     কাঠামো SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode ফলাফল;

  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = sftp_quote(তথ্য, sshc, sshp);
  যদি(ফলাফল) {
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_sftp_পরবর্তী_quote(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->পরবর্তী;

  যদি(sshc->quote_item) {
    myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE);
  }
  অন্যথা {
    যদি(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(তথ্য, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    অন্যথা {
      myssh_to(তথ্য, sshc, SSH_SFTP_GETINFO);
    }
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_quote_stat(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো ssh_conn *sshc,
                                          কাঠামো SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode ফলাফল;

  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = sftp_quote_stat(তথ্য, sshc, sshp, blockp);
  যদি(ফলাফল) {
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_sftp_quote_setstat(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো ssh_conn *sshc,
                                             কাঠামো SSHPROTO *sshp)
{
  পূর্ণসংখ্যা rc;

  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(তথ্য, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ফেরত CURLE_QUOTE_ERROR;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_quote_symlink(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(তথ্য, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ফেরত CURLE_QUOTE_ERROR;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_quote_mkdir(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)তথ্য->set.new_directory_perms);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(তথ্য, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ফেরত CURLE_QUOTE_ERROR;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_quote_rename(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(তথ্য, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ফেরত CURLE_QUOTE_ERROR;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_quote_rmdir(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(তথ্য, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ফেরত CURLE_QUOTE_ERROR;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_quote_unlink(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো ssh_conn *sshc)
{
  পূর্ণসংখ্যা rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(তথ্য, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ফেরত CURLE_QUOTE_ERROR;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_quote_statvfs(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  পূর্ণসংখ্যা rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(তথ্য, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ফেরত CURLE_QUOTE_ERROR;
  }
  অন্যথা যদি(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_আকার_MASK "I64u"
#অন্যথা
#define CURL_LIBSSH2_VFS_আকার_MASK "llu"
#endif
    CURLcode ফলাফল;
    অক্ষর *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_আকার_MASK "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_আকার_MASK "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_blocks, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    যদি(!tmp) {
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ফেরত CURLE_OUT_OF_MEMORY;
    }

    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    যদি(ফলাফল) {
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ফেরত ফলাফল;
    }
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_পরবর্তী_QUOTE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_create_dirs_mkdir(কাঠামো Curl_easy *তথ্য,
                                                 কাঠামো ssh_conn *sshc,
                                                 কাঠামো SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  পূর্ণসংখ্যা rc;

  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)তথ্য->set.new_directory_perms);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  যদি(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    যদি((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
      ফেরত sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_CREATE_DIRS);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_readdir_init(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো ssh_conn *sshc,
                                            কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  Curl_pgrs_Set_Download_আকার(তথ্য, -1);
  যদি(তথ্য->req.no_body) {
    myssh_to(তথ্য, sshc, SSH_STOP);
    ফেরত CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  যদি(!sshc->sftp_handle) {
    unsigned long sftperr;
    যদি(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(তথ্য, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    ফেরত sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_READDIR);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_readdir_link(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো ssh_conn *sshc,
                                            কাঠামো SSHPROTO *sshp)
{
  CURLcode ফলাফল;
  পূর্ণসংখ্যা rc;
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned পূর্ণসংখ্যা)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  যদি(rc < 0)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  ফলাফল = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  যদি(ফলাফল)
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_BOTTOM);
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_scp_download_init(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো ssh_conn *sshc,
                                            কাঠামো SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, আকার(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  যদি(!sshc->ssh_channel) {
    পূর্ণসংখ্যা ssh_err;
    অক্ষর *err_বার্তা = ফাঁকা;

    যদি(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
    failf(তথ্য, "%s", err_বার্তা);
    myssh_to(তথ্য, sshc, SSH_SCP_CHANNEL_FREE);
    ফেরত libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_আকার;
  তথ্য->req.maxdownload = (curl_off_t)sb.st_আকার;
  Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  তথ্য->conn->send_idx = 0;

  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_close(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো ssh_conn *sshc,
                                     কাঠামো SSHPROTO *sshp)
{
  পূর্ণসংখ্যা rc = 0;
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  যদি(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    যদি(rc < 0) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to close libssh2 file: %d %s", rc, err_বার্তা);
    }
    sshc->sftp_handle = ফাঁকা;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(তথ্য, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  যদি(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(তথ্য, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_STOP);

  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_shutdown(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  পূর্ণসংখ্যা rc = 0;
  যদি(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    যদি(rc < 0) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to close libssh2 file: %d %s", rc, err_বার্তা);
    }
    sshc->sftp_handle = ফাঁকা;
  }
  যদি(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    যদি(rc < 0) {
      infof(তথ্য, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = ফাঁকা;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(তথ্য, sshc, SSH_SESSION_DISCONNECT);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_download_init(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো ssh_conn *sshc,
                                             কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)তথ্য->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  যদি(!sshc->sftp_handle) {
    unsigned long sftperr;
    যদি(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      ফেরত CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(তথ্য, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    ফেরত sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_scp_upload_init(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো ssh_conn *sshc,
                                          কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (পূর্ণসংখ্যা)তথ্য->set.new_file_perms,
                       (libssh2_int64_t)তথ্য->state.infilesize, 0, 0);
  যদি(!sshc->ssh_channel) {
    পূর্ণসংখ্যা ssh_err;
    অক্ষর *err_বার্তা = ফাঁকা;
    CURLcode ফলাফল;
    যদি(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
    failf(তথ্য, "%s", err_বার্তা);
    myssh_to(তথ্য, sshc, SSH_SCP_CHANNEL_FREE);
    ফলাফল = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    যদি(ফলাফল == CURLE_SSH ||
       ফলাফল == CURLE_REMOTE_FILE_NOT_FOUND)
      ফলাফল = CURLE_UPLOAD_FAILED;
    ফেরত ফলাফল;
  }

  /* upload data */
  তথ্য->req.আকার_2 = তথ্য->state.infilesize;
  Curl_pgrs_Set_Upload_আকার(তথ্য, তথ্য->state.infilesize);
  Curl_xfer_setup_send(তথ্য, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  তথ্য->conn->recv_idx = FIRSTSOCKET;

  myssh_to(তথ্য, sshc, SSH_STOP);

  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_session_disconnect(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  পূর্ণসংখ্যা rc = 0;
  যদি(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    যদি(rc < 0) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to free libssh2 scp subsystem: %d %s", rc, err_বার্তা);
    }
    sshc->ssh_channel = ফাঁকা;
  }

  যদি(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;

    যদি(rc < 0) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to disconnect libssh2 session: %d %s", rc, err_বার্তা);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(তথ্য, sshc, SSH_SESSION_FREE);
  ফেরত CURLE_OK;
}

স্থির CURLcode sshc_cleanup(কাঠামো ssh_conn *sshc, কাঠামো Curl_easy *তথ্য,
                             bool block)
{
  পূর্ণসংখ্যা rc;

  যদি(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = ফাঁকা;
  }

  যদি(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    যদি((rc < 0) && তথ্য) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_বার্তা);
    }
    যদি(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ফেরত CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = ফাঁকা;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = ফাঁকা;
    sshc->sshagent_পূর্ব_identity = ফাঁকা;
  }

  যদি(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    যদি((rc < 0) && তথ্য) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to close libssh2 file: %d %s", rc, err_বার্তা);
    }
    যদি(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ফেরত CURLE_AGAIN;

    sshc->sftp_handle = ফাঁকা;
  }

  যদি(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    যদি((rc < 0) && তথ্য) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to free libssh2 scp subsystem: %d %s", rc, err_বার্তা);
    }
    যদি(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ফেরত CURLE_AGAIN;

    sshc->ssh_channel = ফাঁকা;
  }

  যদি(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    যদি((rc < 0) && তথ্য) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_বার্তা);
    }
    যদি(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ফেরত CURLE_AGAIN;

    sshc->sftp_session = ফাঁকা;
  }

  যদি(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    যদি((rc < 0) && তথ্য) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session, &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to free libssh2 session: %d %s", rc, err_বার্তা);
    }
    যদি(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ফেরত CURLE_AGAIN;

    sshc->ssh_session = ফাঁকা;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_চাবি);
  curlx_safefree(sshc->priv_চাবি);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_getinfo(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc)
{
  যদি(তথ্য->set.get_filetime)
    myssh_to(তথ্য, sshc, SSH_SFTP_FILETIME);
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_SFTP_TRANS_INIT);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_filetime(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ssh_conn *sshc,
                                        কাঠামো SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  পূর্ণসংখ্যা rc;

  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  যদি(rc == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  যদি(rc == 0)
    তথ্য->info.filetime = (time_t)attrs.mtime;

  myssh_to(তথ্য, sshc, SSH_SFTP_TRANS_INIT);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_trans_init(কাঠামো Curl_easy *তথ্য,
                                          কাঠামো ssh_conn *sshc,
                                          কাঠামো SSHPROTO *sshp)
{
  যদি(তথ্য->state.upload)
    myssh_to(তথ্য, sshc, SSH_SFTP_UPLOAD_INIT);
  অন্যথা যদি(sshp) {
    size_t plen = strlen(sshp->path);
    যদি(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(তথ্য, sshc, SSH_SFTP_READDIR_INIT);
    অন্যথা
      myssh_to(তথ্য, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  অন্যথা
    ফেরত CURLE_FAILED_INIT;
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_upload_init(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো ssh_conn *sshc,
                                           কাঠামো SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode ফলাফল;
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = sftp_upload_init(তথ্য, sshc, sshp, block);
  যদি(ফলাফল) {
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_sftp_create_dirs_init(কাঠামো Curl_easy *তথ্য,
                                                কাঠামো ssh_conn *sshc,
                                                কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  যদি(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(তথ্য, sshc, SSH_SFTP_CREATE_DIRS);
  }
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_SFTP_UPLOAD_INIT);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_create_dirs(কাঠামো Curl_easy *তথ্য,
                                           কাঠামো ssh_conn *sshc,
                                           কাঠামো SSHPROTO *sshp)
{
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  যদি(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(তথ্য, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(তথ্য, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ফেরত CURLE_OK;
  }
  myssh_to(তথ্য, sshc, SSH_SFTP_UPLOAD_INIT);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_readdir(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc,
                                       কাঠামো SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode ফলাফল;
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = sftp_readdir(তথ্য, sshc, sshp, block);
  যদি(ফলাফল)
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_sftp_readdir_bottom(কাঠামো Curl_easy *তথ্য,
                                              কাঠামো ssh_conn *sshc,
                                              কাঠামো SSHPROTO *sshp)
{
  CURLcode ফলাফল;
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  যদি(!ফলাফল)
    ফলাফল = Curl_client_write(তথ্য, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  যদি(ফলাফল) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(তথ্য, sshc, SSH_STOP);
  }
  অন্যথা {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(তথ্য, sshc, SSH_SFTP_READDIR);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_sftp_readdir_done(কাঠামো Curl_easy *তথ্য,
                                            কাঠামো ssh_conn *sshc)
{
  যদি(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;

  sshc->sftp_handle = ফাঁকা;

  /* no data to transfer */
  Curl_xfer_setup_nop(তথ্য);
  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_sftp_download_stat(কাঠামো Curl_easy *তথ্য,
                                             কাঠামো ssh_conn *sshc,
                                             কাঠামো SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode ফলাফল;
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = sftp_download_stat(তথ্য, sshc, sshp, block);
  যদি(ফলাফল) {
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ssh_state_scp_trans_init(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো ssh_conn *sshc,
                                         কাঠামো SSHPROTO *sshp)
{
  CURLcode ফলাফল;
  যদি(!sshp)
    ফেরত CURLE_FAILED_INIT;
  ফলাফল = Curl_getworkingpath(তথ্য, sshc->homedir,
                               &sshp->path);
  যদি(ফলাফল) {
    myssh_to(তথ্য, sshc, SSH_STOP);
    ফেরত ফলাফল;
  }

  যদি(তথ্য->state.upload) {
    যদি(তথ্য->state.infilesize < 0) {
      failf(তথ্য, "SCP requires a known file size for upload");
      myssh_to(তথ্য, sshc, SSH_SCP_CHANNEL_FREE);
      ফেরত CURLE_UPLOAD_FAILED;
    }
    myssh_to(তথ্য, sshc, SSH_SCP_UPLOAD_INIT);
  }
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_SCP_DOWNLOAD_INIT);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_scp_done(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ssh_conn *sshc)
{
  যদি(তথ্য->state.upload)
    myssh_to(তথ্য, sshc, SSH_SCP_SEND_EOF);
  অন্যথা
    myssh_to(তথ্য, sshc, SSH_SCP_CHANNEL_FREE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_scp_send_eof(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc)
{
  যদি(sshc->ssh_channel) {
    পূর্ণসংখ্যা rc = libssh2_channel_send_eof(sshc->ssh_channel);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;
    যদি(rc) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session,
                                       &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_বার্তা);
    }
  }
  myssh_to(তথ্য, sshc, SSH_SCP_WAIT_EOF);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_scp_wait_eof(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc)
{
  যদি(sshc->ssh_channel) {
    পূর্ণসংখ্যা rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;
    যদি(rc) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session,
                                       &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to get channel EOF: %d %s", rc, err_বার্তা);
    }
  }
  myssh_to(তথ্য, sshc, SSH_SCP_WAIT_CLOSE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_scp_wait_close(কাঠামো Curl_easy *তথ্য,
                                         কাঠামো ssh_conn *sshc)
{
  যদি(sshc->ssh_channel) {
    পূর্ণসংখ্যা rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;
    যদি(rc) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session,
                                       &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Channel failed to close: %d %s", rc, err_বার্তা);
    }
  }
  myssh_to(তথ্য, sshc, SSH_SCP_CHANNEL_FREE);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_scp_channel_free(
  কাঠামো Curl_easy *তথ্য,
  কাঠামো ssh_conn *sshc)
{
  যদি(sshc->ssh_channel) {
    পূর্ণসংখ্যা rc = libssh2_channel_free(sshc->ssh_channel);
    যদি(rc == LIBSSH2_ERROR_EAGAIN)
      ফেরত CURLE_AGAIN;
    যদি(rc < 0) {
      অক্ষর *err_বার্তা = ফাঁকা;
      (শূন্য)libssh2_session_last_error(sshc->ssh_session,
                                       &err_বার্তা, ফাঁকা, 0);
      infof(তথ্য, "Failed to free libssh2 scp subsystem: %d %s", rc, err_বার্তা);
    }
    sshc->ssh_channel = ফাঁকা;
  }
  CURL_TRC_SSH(তথ্য, "SCP DONE phase complete");
  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত CURLE_OK;
}

স্থির CURLcode ssh_state_session_free(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো ssh_conn *sshc)
{
  CURLcode ফলাফল = sshc_cleanup(sshc, তথ্য, FALSE);
  কাঠামো connectdata *conn = তথ্য->conn;
  যদি(ফলাফল)
    ফেরত ফলাফল;
  memset(sshc, 0, আকার(কাঠামো ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(তথ্য, sshc, SSH_STOP);
  ফেরত CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
স্থির CURLcode ssh_statemachine(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ssh_conn *sshc,
                                 কাঠামো SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode ফলাফল = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  করো {
    নির্বাচন(sshc->state) {
    ক্ষেত্র SSH_INIT:
      ফলাফল = ssh_state_init(তথ্য, sshc);
      যদি(ফলাফল)
        বিরতি;
      FALLTHROUGH();

    ক্ষেত্র SSH_S_STARTUP:
      ফলাফল = ssh_state_startup(তথ্য, sshc);
      যদি(ফলাফল)
        বিরতি;
      FALLTHROUGH();

    ক্ষেত্র SSH_HOSTKEY:
      ফলাফল = ssh_state_hostkey(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTHLIST:
      ফলাফল = ssh_state_authlist(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_PKEY:
      ফলাফল = ssh_state_auth_pkey(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_PASS_INIT:
      ফলাফল = ssh_state_auth_pass_init(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_PASS:
      ফলাফল = ssh_state_auth_pass(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_HOST_INIT:
      ফলাফল = ssh_state_auth_host_init(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_HOST:
      myssh_to(তথ্য, sshc, SSH_AUTH_AGENT_INIT);
      বিরতি;

    ক্ষেত্র SSH_AUTH_AGENT_INIT:
      ফলাফল = ssh_state_auth_agent_init(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_AGENT_LIST:
      ফলাফল = ssh_state_auth_agent_list(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_AGENT:
      ফলাফল = ssh_state_auth_agent(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_চাবি_INIT:
      ফলাফল = ssh_state_auth_চাবি_init(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_চাবি:
      ফলাফল = ssh_state_auth_চাবি(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_AUTH_DONE:
      ফলাফল = ssh_state_auth_done(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_INIT:
      ফলাফল = ssh_state_sftp_init(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_REALPATH:
      ফলাফল = ssh_state_sftp_realpath(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE_INIT:
      ফলাফল = ssh_state_sftp_quote_init(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_POSTQUOTE_INIT:
      ফলাফল = ssh_state_sftp_postquote_init(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE:
      ফলাফল = ssh_state_sftp_quote(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_পরবর্তী_QUOTE:
      ফলাফল = ssh_state_sftp_পরবর্তী_quote(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE_STAT:
      ফলাফল = ssh_state_sftp_quote_stat(তথ্য, sshc, sshp, block);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE_SETSTAT:
      ফলাফল = ssh_state_sftp_quote_setstat(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE_SYMLINK:
      ফলাফল = ssh_state_sftp_quote_symlink(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE_MKDIR:
      ফলাফল = ssh_state_sftp_quote_mkdir(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE_RENAME:
      ফলাফল = ssh_state_sftp_quote_rename(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE_RMDIR:
      ফলাফল = ssh_state_sftp_quote_rmdir(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE_UNLINK:
      ফলাফল = ssh_state_sftp_quote_unlink(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_QUOTE_STATVFS:
      ফলাফল = ssh_state_sftp_quote_statvfs(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_GETINFO:
      ফলাফল = ssh_state_sftp_getinfo(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_FILETIME:
      ফলাফল = ssh_state_sftp_filetime(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_TRANS_INIT:
      ফলাফল = ssh_state_sftp_trans_init(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_UPLOAD_INIT:
      ফলাফল = ssh_state_sftp_upload_init(তথ্য, sshc, sshp, block);
      বিরতি;

    ক্ষেত্র SSH_SFTP_CREATE_DIRS_INIT:
      ফলাফল = ssh_state_sftp_create_dirs_init(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_CREATE_DIRS:
      ফলাফল = ssh_state_sftp_create_dirs(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_CREATE_DIRS_MKDIR:
      ফলাফল = ssh_state_sftp_create_dirs_mkdir(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_READDIR_INIT:
      ফলাফল = ssh_state_sftp_readdir_init(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_READDIR:
      ফলাফল = ssh_state_sftp_readdir(তথ্য, sshc, sshp, block);
      বিরতি;

    ক্ষেত্র SSH_SFTP_READDIR_LINK:
      ফলাফল = ssh_state_sftp_readdir_link(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_READDIR_BOTTOM:
      ফলাফল = ssh_state_sftp_readdir_bottom(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_READDIR_DONE:
      ফলাফল = ssh_state_sftp_readdir_done(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SFTP_DOWNLOAD_INIT:
      ফলাফল = ssh_state_sftp_download_init(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_DOWNLOAD_STAT:
      ফলাফল = ssh_state_sftp_download_stat(তথ্য, sshc, sshp, block);
      বিরতি;

    ক্ষেত্র SSH_SFTP_CLOSE:
      ফলাফল = ssh_state_sftp_close(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SFTP_SHUTDOWN:
      ফলাফল = ssh_state_sftp_shutdown(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SCP_TRANS_INIT:
      ফলাফল = ssh_state_scp_trans_init(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SCP_UPLOAD_INIT:
      ফলাফল = ssh_state_scp_upload_init(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SCP_DOWNLOAD_INIT:
      ফলাফল = ssh_state_scp_download_init(তথ্য, sshc, sshp);
      বিরতি;

    ক্ষেত্র SSH_SCP_DONE:
      ফলাফল = ssh_state_scp_done(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SCP_SEND_EOF:
      ফলাফল = ssh_state_scp_send_eof(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SCP_WAIT_EOF:
      ফলাফল = ssh_state_scp_wait_eof(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SCP_WAIT_CLOSE:
      ফলাফল = ssh_state_scp_wait_close(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SCP_CHANNEL_FREE:
      ফলাফল = ssh_state_scp_channel_free(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SESSION_DISCONNECT:
      ফলাফল = ssh_state_session_disconnect(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_SESSION_FREE:
      ফলাফল = ssh_state_session_free(তথ্য, sshc);
      বিরতি;

    ক্ষেত্র SSH_QUIT:
    পূর্বনির্ধারিত:
      /* internal error */
      myssh_to(তথ্য, sshc, SSH_STOP);
      বিরতি;
    }

  } যখন(!ফলাফল && (sshc->state != SSH_STOP) && !*block);

  যদি(ফলাফল == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    ফলাফল = CURLE_OK;
  }
  CURL_TRC_SSH(তথ্য, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (পূর্ণসংখ্যা)ফলাফল, *block);

  ফেরত ফলাফল;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
স্থির শূন্য ssh_block2waitfor(কাঠামো Curl_easy *তথ্য,
                              কাঠামো ssh_conn *sshc,
                              bool block)
{
  পূর্ণসংখ্যা dir = 0;
  (শূন্য)তথ্য;
  যদি(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    যদি(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  যদি(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
স্থির CURLcode ssh_multi_statemach(কাঠামো Curl_easy *তথ্য, bool *done)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  কাঠামো SSHPROTO *sshp = Curl_meta_get(তথ্য, CURL_META_SSH_EASY);
  CURLcode ফলাফল = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  যদি(!sshc || !sshp)
    ফেরত CURLE_FAILED_INIT;

  করো {
    ফলাফল = ssh_statemachine(তথ্য, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } যখন(!ফলাফল && !*done && !block);
  ssh_block2waitfor(তথ্য, sshc, block);

  ফেরত ফলাফল;
}

স্থির CURLcode ssh_block_statemach(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ssh_conn *sshc,
                                    কাঠামো SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো curltime start = *Curl_pgrs_now(তথ্য);

  যখন((sshc->state != SSH_STOP) && !ফলাফল) {
    bool block;
    timediff_t বাম_ms = 1000;

    ফলাফল = ssh_statemachine(তথ্য, sshc, sshp, &block);
    যদি(ফলাফল)
      বিরতি;

    যদি(!disconnect) {
      ফলাফল = Curl_pgrsCheck(তথ্য);
      যদি(ফলাফল)
        বিরতি;

      বাম_ms = Curl_timeleft_ms(তথ্য);
      যদি(বাম_ms < 0) {
        failf(তথ্য, "Operation timed out");
        ফেরত CURLE_OPERATION_TIMEDOUT;
      }
    }
    অন্যথা যদি(curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &start) > 1000) {
      /* disconnect timeout */
      failf(তথ্য, "Disconnect timed out");
      ফলাফল = CURLE_OK;
      বিরতি;
    }

    যদি(block) {
      পূর্ণসংখ্যা dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = তথ্য->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      যদি(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      যদি(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (শূন্য)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!বাম_ms || বাম_ms > 1000) ? 1000 : বাম_ms);
    }
  }

  ফেরত ফলাফল;
}

স্থির শূন্য myssh_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো SSHPROTO *sshp = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

স্থির শূন্য myssh_conn_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো ssh_conn *sshc = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  sshc_cleanup(sshc, ফাঁকা, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
স্থির CURLcode ssh_setup_connection(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো connectdata *conn)
{
  কাঠামো ssh_conn *sshc;
  কাঠামো SSHPROTO *sshp;
  (শূন্য)conn;

  sshc = curlx_calloc(1, আকার(*sshc));
  যদি(!sshc)
    ফেরত CURLE_OUT_OF_MEMORY;

  যদি(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, আকার(*sshp));
  যদি(!sshp)
    ফেরত CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  যদি(Curl_meta_set(তথ্য, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ফেরত CURLE_OUT_OF_MEMORY;

  ফেরত Curl_ssh_setup_pkey(তথ্য, sshc);
}

স্থির Curl_recv scp_recv, sftp_recv;
স্থির Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
স্থির ssize_t ssh_tls_recv(libssh2_socket_t sock, শূন্য *বাফার,
                            size_t দৈর্ঘ্য, পূর্ণসংখ্যা flags, শূন্য **abstract)
{
  কাঠামো Curl_easy *তথ্য = (কাঠামো Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(তথ্য, sock);
  size_t nread;
  CURLcode ফলাফল;
  কাঠামো connectdata *conn = তথ্য->conn;
  Curl_recv *backup = conn->recv[sockindex];
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (শূন্য)flags;

  যদি(!sshc)
    ফেরত -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  ফলাফল = Curl_conn_recv(তথ্য, sockindex, বাফার, দৈর্ঘ্য, &nread);
  conn->recv[sockindex] = backup;
  যদি(ফলাফল == CURLE_AGAIN)
    ফেরত -EAGAIN; /* magic return code for libssh2 */
  অন্যথা যদি(ফলাফল)
    ফেরত -1; /* generic error */
  Curl_debug(তথ্য, CURLINFO_তথ্য_IN, (ধ্রুবক অক্ষর *)বাফার, nread);
  ফেরত (ssize_t)nread;
}

স্থির ssize_t ssh_tls_send(libssh2_socket_t sock, ধ্রুবক শূন্য *বাফার,
                            size_t দৈর্ঘ্য, পূর্ণসংখ্যা flags, শূন্য **abstract)
{
  কাঠামো Curl_easy *তথ্য = (কাঠামো Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(তথ্য, sock);
  size_t nwrite;
  CURLcode ফলাফল;
  কাঠামো connectdata *conn = তথ্য->conn;
  Curl_send *backup = conn->send[sockindex];
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (শূন্য)flags;

  যদি(!sshc)
    ফেরত -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  ফলাফল = Curl_conn_send(তথ্য, sockindex, বাফার, দৈর্ঘ্য, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  যদি(ফলাফল == CURLE_AGAIN)
    ফেরত -EAGAIN; /* magic return code for libssh2 */
  অন্যথা যদি(ফলাফল)
    ফেরত -1; /* error */
  Curl_debug(তথ্য, CURLINFO_তথ্য_OUT, (ধ্রুবক অক্ষর *)বাফার, nwrite);
  ফেরত (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
স্থির CURLcode ssh_connect(কাঠামো Curl_easy *তথ্য, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ফলাফল;

#যদি LIBSSH2_VERSION_অঙ্ক >= 0x010b00
  {
    ধ্রুবক অক্ষর *crypto_str;
    নির্বাচন(libssh2_crypto_engine()) {
    ক্ষেত্র libssh2_gcrypt:
      crypto_str = "libgcrypt";
      বিরতি;
    ক্ষেত্র libssh2_mbedtls:
      crypto_str = "mbedTLS";
      বিরতি;
    ক্ষেত্র libssh2_openssl:
      crypto_str = "openssl compatible";
      বিরতি;
    ক্ষেত্র libssh2_os400qc3:
      crypto_str = "OS400QC3";
      বিরতি;
    ক্ষেত্র libssh2_wincng:
      crypto_str = "WinCNG";
      বিরতি;
    পূর্বনির্ধারিত:
      crypto_str = ফাঁকা;
      বিরতি;
    }
    যদি(crypto_str)
      infof(তথ্য, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  infof(তথ্য, "SSH: user '%s'", Curl_creds_user(conn->creds));
#ifdef CURL_LIBSSH2_DEBUG
  infof(তথ্য, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#endif /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, তথ্য);

  যদি(!sshc->ssh_session) {
    failf(তথ্য, "Failure initializing ssh session");
    ফেরত CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#যদি LIBSSH2_VERSION_অঙ্ক >= 0x010B00
  যদি(তথ্য->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(তথ্য->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  যদি(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#যদি LIBSSH2_VERSION_অঙ্ক >= 0x010b01
    infof(তথ্য, "SSH: using HTTPS proxy");
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
#endif
#অন্যথা
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    সংঘ receive {
      শূন্য *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, শূন্য *, size_t, পূর্ণসংখ্যা, শূন্য **);
    };
    সংঘ transfer {
      শূন্য *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ধ্রুবক শূন্য *, size_t, পূর্ণসংখ্যা, শূন্য **);
    };
    সংঘ receive sshrecv;
    সংঘ transfer sshsend;

    sshrecv.recvptr = ssh_tls_recv;
    sshsend.sendptr = ssh_tls_send;

    infof(তথ্য, "SSH: using HTTPS proxy");
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
  যদি(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  অন্যথা {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  যদি(তথ্য->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(তথ্য, "SSH: failed to enable compression for session");
  }

  যদি(CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS)) {
    পূর্ণসংখ্যা rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    যদি(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = ফাঁকা;
      ফেরত CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    যদি(rc < 0)
      infof(তথ্য, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(তথ্য, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(তথ্য, "SSH: socket %d", (পূর্ণসংখ্যা)sock);
#endif

  myssh_to(তথ্য, sshc, SSH_INIT);

  ফলাফল = ssh_multi_statemach(তথ্য, done);

  ফেরত ফলাফল;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

স্থির CURLcode scp_perform(কাঠামো Curl_easy *তথ্য,
                            bool *connected,
                            bool *dophase_done)
{
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(তথ্য->conn, CURL_META_SSH_CONN);
  CURLcode ফলাফল = CURLE_OK;

  CURL_TRC_SSH(তথ্য, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(তথ্য, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  ফলাফল = ssh_multi_statemach(তথ্য, dophase_done);

  *connected = Curl_conn_is_connected(তথ্য->conn, FIRSTSOCKET);

  যদি(*dophase_done) {
    CURL_TRC_SSH(তথ্য, "DO phase is complete");
  }

  ফেরত ফলাফল;
}

/* called from multi.c while DOing */
স্থির CURLcode scp_doing(কাঠামো Curl_easy *তথ্য,
                          bool *dophase_done)
{
  CURLcode ফলাফল;
  ফলাফল = ssh_multi_statemach(তথ্য, dophase_done);

  যদি(*dophase_done) {
    CURL_TRC_SSH(তথ্য, "DO phase is complete");
  }
  ফেরত ফলাফল;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
স্থির CURLcode scp_disconnect(কাঠামো Curl_easy *তথ্য,
                               কাঠামো connectdata *conn,
                               bool dead_connection)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  কাঠামো SSHPROTO *sshp = Curl_meta_get(তথ্য, CURL_META_SSH_EASY);
  (শূন্য)dead_connection;

  যদি(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(তথ্য, sshc, SSH_SESSION_DISCONNECT);
    ফলাফল = ssh_block_statemach(তথ্য, sshc, sshp, TRUE);
  }

  যদি(sshc)
    ফেরত sshc_cleanup(sshc, তথ্য, TRUE);
  ফেরত ফলাফল;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
স্থির CURLcode ssh_done(কাঠামো Curl_easy *তথ্য, CURLcode status)
{
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(তথ্য->conn, CURL_META_SSH_CONN);
  কাঠামো SSHPROTO *sshp = Curl_meta_get(তথ্য, CURL_META_SSH_EASY);
  CURLcode ফলাফল = CURLE_OK;

  যদি(!sshc || !sshp)
    ফেরত CURLE_FAILED_INIT;

  যদি(!status)
    /* run the state-machine */
    ফলাফল = ssh_block_statemach(তথ্য, sshc, sshp, FALSE);
  অন্যথা
    ফলাফল = status;

  যদি(Curl_pgrsDone(তথ্য, Curl_pgrs_now(তথ্য)))
    ফেরত CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(তথ্য);
  ফেরত ফলাফল;
}

স্থির CURLcode scp_done(কাঠামো Curl_easy *তথ্য, CURLcode status,
                         bool premature)
{
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(তথ্য->conn, CURL_META_SSH_CONN);
  (শূন্য)premature;

  যদি(sshc && !status)
    myssh_to(তথ্য, sshc, SSH_SCP_DONE);

  ফেরত ssh_done(তথ্য, status);
}

স্থির CURLcode scp_send(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                         ধ্রুবক uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ফলাফল = CURLE_OK;
  ssize_t nwritten;

  (শূন্য)sockindex; /* we only support SCP on the fixed known primary socket */
  (শূন্য)eos;
  *pnwritten = 0;

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ধ্রুবক অক্ষর *)mem, len);

  ssh_block2waitfor(তথ্য, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  যদি(nwritten == LIBSSH2_ERROR_EAGAIN)
    ফলাফল = CURLE_AGAIN;
  অন্যথা যদি(nwritten < LIBSSH2_ERROR_NONE)
    ফলাফল = libssh2_session_error_to_CURLE((পূর্ণসংখ্যা)nwritten);
  অন্যথা
    *pnwritten = (size_t)nwritten;

  ফেরত ফলাফল;
}

স্থির CURLcode scp_recv(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                         অক্ষর *mem, size_t len, size_t *pnread)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ফলাফল = CURLE_OK;
  ssize_t nread;

  (শূন্য)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(তথ্য, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  যদি(nread == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;
  অন্যথা যদি(nread < LIBSSH2_ERROR_NONE)
    ফলাফল = libssh2_session_error_to_CURLE((পূর্ণসংখ্যা)nread);
  অন্যথা
    *pnread = (size_t)nread;

  ফেরত ফলাফল;
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

স্থির CURLcode sftp_perform(কাঠামো Curl_easy *তথ্য,
                             bool *connected,
                             bool *dophase_done)
{
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(তথ্য->conn, CURL_META_SSH_CONN);
  CURLcode ফলাফল = CURLE_OK;

  CURL_TRC_SSH(তথ্য, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(তথ্য, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  ফলাফল = ssh_multi_statemach(তথ্য, dophase_done);

  *connected = Curl_conn_is_connected(তথ্য->conn, FIRSTSOCKET);

  যদি(*dophase_done) {
    CURL_TRC_SSH(তথ্য, "DO phase is complete");
  }

  ফেরত ফলাফল;
}

/* called from multi.c while DOing */
স্থির CURLcode sftp_doing(কাঠামো Curl_easy *তথ্য,
                           bool *dophase_done)
{
  CURLcode ফলাফল = ssh_multi_statemach(তথ্য, dophase_done);

  যদি(*dophase_done) {
    CURL_TRC_SSH(তথ্য, "DO phase is complete");
  }
  ফেরত ফলাফল;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
স্থির CURLcode sftp_disconnect(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn, bool dead_connection)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  কাঠামো SSHPROTO *sshp = Curl_meta_get(তথ্য, CURL_META_SSH_EASY);
  (শূন্য)dead_connection;

  যদি(sshc) {
    যদি(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(তথ্য, "DISCONNECT starts now");
      myssh_to(তথ্য, sshc, SSH_SFTP_SHUTDOWN);
      ফলাফল = ssh_block_statemach(তথ্য, sshc, sshp, TRUE);
      CURL_TRC_SSH(তথ্য, "DISCONNECT is done -> %d", (পূর্ণসংখ্যা)ফলাফল);
    }
    sshc_cleanup(sshc, তথ্য, TRUE);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode sftp_done(কাঠামো Curl_easy *তথ্য, CURLcode status,
                          bool premature)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  যদি(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    যদি(!premature && তথ্য->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(তথ্য, sshc, SSH_SFTP_CLOSE);
  }
  ফেরত ssh_done(তথ্য, status);
}

/* return number of sent bytes */
স্থির CURLcode sftp_send(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                          ধ্রুবক uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (শূন্য)sockindex;
  (শূন্য)eos;
  *pnwritten = 0;

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ধ্রুবক অক্ষর *)mem, len);

  ssh_block2waitfor(তথ্য, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  যদি(nwrite == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;
  অন্যথা যদি(nwrite < LIBSSH2_ERROR_NONE)
    ফেরত libssh2_session_error_to_CURLE((পূর্ণসংখ্যা)nwrite);
  *pnwritten = (size_t)nwrite;
  ফেরত CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
স্থির CURLcode sftp_recv(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                          অক্ষর *mem, size_t len, size_t *pnread)
{
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (শূন্য)sockindex;
  *pnread = 0;

  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(তথ্য, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  যদি(nread == LIBSSH2_ERROR_EAGAIN)
    ফেরত CURLE_AGAIN;
  অন্যথা যদি(nread < 0)
    ফেরত libssh2_session_error_to_CURLE((পূর্ণসংখ্যা)nread);

  *pnread = (size_t)nread;
  ফেরত CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
স্থির CURLcode ssh_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল;
  bool connected = FALSE;
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  যদি(!sshc)
    ফেরত CURLE_FAILED_INIT;

  তথ্য->req.আকার_2 = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(তথ্য);

  যদি(conn->scheme->protocol & CURLPROTO_SCP)
    ফলাফল = scp_perform(তথ্য, &connected, done);
  অন্যথা
    ফলাফল = sftp_perform(তথ্য, &connected, done);

  ফেরত ফলাফল;
}

CURLcode Curl_ssh_init(শূন্য)
{
  যদি(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    ফেরত CURLE_FAILED_INIT;
  }
  ফেরত CURLE_OK;
}

শূন্য Curl_ssh_cleanup(শূন্য)
{
  libssh2_exit();
}

শূন্য Curl_ssh_version(অক্ষর *বাফার, size_t buflen)
{
  (শূন্য)curl_msnprintf(বাফার, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
স্থির শূন্য ssh_attach(কাঠামো Curl_easy *তথ্য, কাঠামো connectdata *conn)
{
  DEBUGASSERT(তথ্য);
  DEBUGASSERT(conn);
  যদি(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    কাঠামো ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    যদি(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      শূন্য **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = তথ্য;
    }
  }
}

/*
 * SCP protocol handler.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_scp = {
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
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_sftp = {
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
