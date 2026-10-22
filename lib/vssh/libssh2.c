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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే __VMS
#అన్తర్భావ <in.h>
#అన్తర్భావ <inet.h>
#అయితే_ముగింపు

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "vssh/ssh.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "cfilters.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "parsedate.h" /* for the week day and month names */
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "vssh/vssh.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curlx/base64.h" /* for curlx_base64_encode() */

స్థిర స్థిరరాశి అక్షరం *sftp_libssh2_strerror(గుర్తులేని దీర్ఘ err)
{
  ఎంపిక(err) {
  స్థితి LIBSSH2_FX_NO_SUCH_FILE:
    తిరిగి "No such file or directory";

  స్థితి LIBSSH2_FX_PERMISSION_DENIED:
    తిరిగి "Permission denied";

  స్థితి LIBSSH2_FX_FAILURE:
    తిరిగి "Operation failed";

  స్థితి LIBSSH2_FX_BAD_సందేశం:
    తిరిగి "Bad message from SFTP server";

  స్థితి LIBSSH2_FX_NO_CONNECTION:
    తిరిగి "Not connected to SFTP server";

  స్థితి LIBSSH2_FX_CONNECTION_కోల్పోయిన:
    తిరిగి "Connection to SFTP server lost";

  స్థితి LIBSSH2_FX_OP_UNSUPPORTED:
    తిరిగి "Operation not supported by SFTP server";

  స్థితి LIBSSH2_FX_INVALID_HANDLE:
    తిరిగి "Invalid handle";

  స్థితి LIBSSH2_FX_NO_SUCH_మార్గం:
    తిరిగి "No such file or directory";

  స్థితి LIBSSH2_FX_FILE_ALREADY_EXISTS:
    తిరిగి "File already exists";

  స్థితి LIBSSH2_FX_WRITE_PROTECT:
    తిరిగి "File is write protected";

  స్థితి LIBSSH2_FX_NO_MEDIA:
    తిరిగి "No media";

  స్థితి LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    తిరిగి "Disk full";

  స్థితి LIBSSH2_FX_QUOTA_EXCEEDED:
    తిరిగి "User quota exceeded";

  స్థితి LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    తిరిగి "Unknown principal";

  స్థితి LIBSSH2_FX_తాళం_CONFLICT:
    తిరిగి "File lock conflict";

  స్థితి LIBSSH2_FX_DIR_NOT_ఖాళీలు:
    తిరిగి "Directory not empty";

  స్థితి LIBSSH2_FX_NOT_A_DIRECTORY:
    తిరిగి "Not a directory";

  స్థితి LIBSSH2_FX_INVALID_FILENAME:
    తిరిగి "Invalid filename";

  స్థితి LIBSSH2_FX_LINK_LOOP:
    తిరిగి "Link points to itself";
  }
  తిరిగి "Unknown error in libssh2";
}

స్థిర శూన్య kbd_callback(స్థిరరాశి అక్షరం *name, పూర్ణాంకం name_len,
                         స్థిరరాశి అక్షరం *instruction, పూర్ణాంకం instruction_len,
                         పూర్ణాంకం అంకె_prompts,
                         స్థిరరాశి LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         శూన్య **abstract)
{
  నిర్మాణం Curl_easy *దత్తాంశం = (నిర్మాణం Curl_easy *)*abstract;

#నిర్వచితమైతే CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", అంకె_prompts);
#లేకపోతే
  (శూన్య)name;
  (శూన్య)name_len;
  (శూన్య)instruction;
  (శూన్య)instruction_len;
#అయితే_ముగింపు /* CURL_LIBSSH2_DEBUG */
  ఒకవేళ(అంకె_prompts == 1) {
    నిర్మాణం connectdata *conn = దత్తాంశం->conn;
    స్థిరరాశి అక్షరం *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].పాఠ్యం_3 = Curl_cstrdup(passwd);
    responses[0].నిడివి = responses[0].పాఠ్యం_3 ? curlx_uztoui(అక్షరశ్రేణి_పొడవు(passwd)) : 0;
  }
  (శూన్య)prompts;
} /* kbd_callback */

స్థిర CURLcode sftp_libssh2_error_గమ్యం_CURLE(గుర్తులేని దీర్ఘ err)
{
  ఎంపిక(err) {
  స్థితి LIBSSH2_FX_OK:
    తిరిగి CURLE_OK;

  స్థితి LIBSSH2_FX_NO_SUCH_FILE:
  స్థితి LIBSSH2_FX_NO_SUCH_మార్గం:
    తిరిగి CURLE_REMOTE_FILE_NOT_దొరికింది;

  స్థితి LIBSSH2_FX_PERMISSION_DENIED:
  స్థితి LIBSSH2_FX_WRITE_PROTECT:
  స్థితి LIBSSH2_FX_తాళం_CONFLICT:
    తిరిగి CURLE_REMOTE_ACCESS_DENIED;

  స్థితి LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  స్థితి LIBSSH2_FX_QUOTA_EXCEEDED:
    తిరిగి CURLE_REMOTE_DISK_నిండిన;

  స్థితి LIBSSH2_FX_FILE_ALREADY_EXISTS:
    తిరిగి CURLE_REMOTE_FILE_EXISTS;

  స్థితి LIBSSH2_FX_DIR_NOT_ఖాళీలు:
    తిరిగి CURLE_QUOTE_ERROR;

  అప్రమేయ:
    ఆపు;
  }

  తిరిగి CURLE_SSH;
}

స్థిర CURLcode libssh2_session_error_గమ్యం_CURLE(పూర్ణాంకం err)
{
  ఎంపిక(err) {
  /* Ordered by order of appearance in libssh2.h */
  స్థితి LIBSSH2_ERROR_NONE:
    తిరిగి CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  స్థితి LIBSSH2_ERROR_SCP_PROTOCOL:
    తిరిగి CURLE_REMOTE_FILE_NOT_దొరికింది;

  స్థితి LIBSSH2_ERROR_SOCKET_NONE:
    తిరిగి CURLE_COULDNT_CONNECT;

  స్థితి LIBSSH2_ERROR_ALLOC:
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  స్థితి LIBSSH2_ERROR_SOCKET_SEND:
    తిరిగి CURLE_SEND_ERROR;

  స్థితి LIBSSH2_ERROR_HOSTKEY_INIT:
  స్థితి LIBSSH2_ERROR_HOSTKEY_SIGN:
  స్థితి LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  స్థితి LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    తిరిగి CURLE_PEER_FAILED_VERIFICATION;

  స్థితి LIBSSH2_ERROR_PASSWORD_EXPIRED:
    తిరిగి CURLE_LOGIN_DENIED;

  స్థితి LIBSSH2_ERROR_SOCKET_TIMEOUT:
  స్థితి LIBSSH2_ERROR_TIMEOUT:
    తిరిగి CURLE_OPERATION_TIMEDOUT;

  స్థితి LIBSSH2_ERROR_EAGAIN:
    తిరిగి CURLE_AGAIN;
  }

  తిరిగి CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

స్థిర LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (శూన్య)abstract;
  తిరిగి Curl_cmalloc(లెక్క);
}

స్థిర LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (శూన్య)abstract;
  తిరిగి Curl_crealloc(ptr, లెక్క);
}

స్థిర LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (శూన్య)abstract;
  ఒకవేళ(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

స్థిర పూర్ణాంకం sshkeycallback(CURL *easy,
                          స్థిరరాశి నిర్మాణం curl_khkey *knownkey, /*తెలిసిన*/
                          స్థిరరాశి నిర్మాణం curl_khkey *foundkey, /* found */
                          గణన curl_khmatch సరిపోలిక,
                          శూన్య *clientp)
{
  (శూన్య)easy;
  (శూన్య)knownkey;
  (శూన్య)foundkey;
  (శూన్య)clientp;

  /* we only allow perfect matches, and we reject everything else */
  తిరిగి (సరిపోలిక != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

స్థిర గణన curl_khtype convert_ssh2_keytype(పూర్ణాంకం sshkeytype)
{
  గణన curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  ఎంపిక(sshkeytype) {
  స్థితి LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    ఆపు;
#నిర్వచితమైతే LIBSSH2_HOSTKEY_TYPE_DSS
  స్థితి LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  స్థితి LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  స్థితి LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  స్థితి LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే LIBSSH2_HOSTKEY_TYPE_ED25519
  స్థితి LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    ఆపు;
#అయితే_ముగింపు
  }
  తిరిగి keytype;
}

స్థిర CURLcode ssh_knownhost(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ssh_conn *sshc)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం libssh2_knownhost *host = ఖాళీ;
  స్థిరరాశి అక్షరం *remotekey = ఖాళీ;
  పూర్ణాంకం keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  పూర్ణాంకం keybit = 0;
  పూర్ణాంకం sshkeytype = 0;
  size_t keylen = 0;
  పూర్ణాంకం rc = 0;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS)) {
    infof(దత్తాంశం, "SSH: no knownhosts file configured");
    తిరిగి CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  ఒకవేళ(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    నిర్మాణం Curl_mapi_guard guard;
    గణన curl_khmatch keymatch;
    curl_sshkeycallback func =
      దత్తాంశం->సమితి.ssh_keyfunc ? దత్తాంశం->సమితి.ssh_keyfunc : sshkeycallback;
    నిర్మాణం curl_khkey knownkey;
    నిర్మాణం curl_khkey *knownkeyp = ఖాళీ;
    నిర్మాణం curl_khkey foundkey;

    ఎంపిక(sshkeytype) {
    స్థితి LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_కీలకం_SSHRSA;
      ఆపు;
#నిర్వచితమైతే LIBSSH2_HOSTKEY_TYPE_DSS
    స్థితి LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_కీలకం_SSHDSS;
      ఆపు;
#అయితే_ముగింపు
    స్థితి LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_కీలకం_ECDSA_256;
      ఆపు;
    స్థితి LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_కీలకం_ECDSA_384;
      ఆపు;
    స్థితి LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_కీలకం_ECDSA_521;
      ఆపు;
    స్థితి LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_కీలకం_ED25519;
      ఆపు;
    అప్రమేయ:
      infof(దత్తాంశం, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      ఆపు;
    }
    ఒకవేళ(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    లేకపోతే {
      keycheck = libssh2_knownhost_checkp(sshc->kh,
                                          conn->origin->hostname,
                                          (conn->origin->పోర్టు != పోర్టు_SSH) ?
                                          conn->origin->పోర్టు : -1,
                                          remotekey, keylen,
                                          LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                          LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                          keybit,
                                          &host);

      infof(దత్తాంశం, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->కీలకం : "<none>");

      /* setup 'knownkey' */
      ఒకవేళ(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.కీలకం = host->కీలకం;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.కీలకం = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (గణన curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_ssh_keyfunc);
      rc = func(దత్తాంశం, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, దత్తాంశం->సమితి.ssh_keyfunc_userp);
      CURL_CBAPI_చివర(&guard);
    }
  }
  లేకపోతే {
    /* no remotekey means failure! */
    infof(దత్తాంశం, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  ఎంపిక(rc) {
  అప్రమేయ: /* unknown return codes is the same as reject */
  స్థితి CURLKHSTAT_REJECT:
    infof(దత్తాంశం, "SSH: knownhost check failed");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  స్థితి CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    ఆపు;
  స్థితి CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    ఒకవేళ(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  స్థితి CURLKHSTAT_FINE:
  స్థితి CURLKHSTAT_FINE_కూడు_గమ్యం_FILE:
    /* proceed */
    ఒకవేళ(keycheck != LIBSSH2_KNOWNHOST_CHECK_సరిపోలిక) {
      పూర్ణాంకం addrc;
      స్థిరరాశి అక్షరం *hostbuf;
      అక్షరం *hostport = ఖాళీ;
      ఒకవేళ(conn->origin->పోర్టు != పోర్టు_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->పోర్టు);
        ఒకవేళ(!hostbuf)
          infof(దత్తాంశం, "WARNING: failed allocating buffer for [host]:port");
      }
      లేకపోతే
        hostbuf = conn->origin->hostname;
      ఒకవేళ(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, ఖాళీ,
                                       remotekey, keylen, ఖాళీ, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, ఖాళీ);
        ఒకవేళ(addrc)
          infof(దత్తాంశం, "WARNING: adding the known host %s failed", hostbuf);
        లేకపోతే ఒకవేళ(rc == CURLKHSTAT_FINE_కూడు_గమ్యం_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          పూర్ణాంకం wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          ఒకవేళ(wrc) {
            infof(దత్తాంశం, "WARNING: writing %s failed",
                  CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    లేకపోతే
      infof(దత్తాంశం, "SSH: knownhost entry matches host key");
    ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_check_fingerprint(నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం ssh_conn *sshc)
{
  స్థిరరాశి అక్షరం *pubkey_md5 =
    CURL_EASY_STR(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_MD5);
  స్థిరరాశి అక్షరం *pubkey_sha256 =
    CURL_EASY_STR(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_SHA256);

  ఒకవేళ(pubkey_sha256) {
    స్థిరరాశి అక్షరం *fingerprint = ఖాళీ;
    అక్షరం *fingerprint_b64 = ఖాళీ;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(దత్తాంశం, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    ఒకవేళ(!fingerprint) {
      failf(దత్తాంశం,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
      తిరిగి CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    ఒకవేళ(curlx_base64_encode((స్థిరరాశి uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
      తిరిగి CURLE_PEER_FAILED_VERIFICATION;
    }

    ఒకవేళ(!fingerprint_b64) {
      failf(దత్తాంశం, "SHA256 fingerprint could not be encoded");
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
      తిరిగి CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(దత్తాంశం, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    అయితే((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    అయితే((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    ఒకవేళ((pub_pos != b64_pos) ||
       అక్షరశ్రేణి_పోలిక(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(దత్తాంశం,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
      తిరిగి CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(దత్తాంశం, "SSH: SHA256 checksum match");
  }

  ఒకవేళ(pubkey_md5) {
    అక్షరం md5buffer[33];
    స్థిరరాశి అక్షరం *fingerprint;

    infof(దత్తాంశం, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    ఒకవేళ(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      పూర్ణాంకం i;
      కొరకు(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (గుర్తులేని అక్షరం)fingerprint[i]);
      }

      infof(దత్తాంశం, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    ఒకవేళ(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      ఒకవేళ(fingerprint) {
        failf(దత్తాంశం,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      లేకపోతే {
        failf(దత్తాంశం,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
      తిరిగి CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(దత్తాంశం, "SSH: MD5 checksum match");
  }

  ఒకవేళ(!pubkey_md5 && !pubkey_sha256) {
    ఒకవేళ(దత్తాంశం->సమితి.ssh_hostkeyfunc) {
      size_t keylen = 0;
      పూర్ణాంకం sshkeytype = 0;
      పూర్ణాంకం rc = 0;
      /* we handle the process to the callback */
      స్థిరరాశి అక్షరం *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      ఒకవేళ(remotekey) {
        నిర్మాణం Curl_mapi_guard guard;
        గణన curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_ssh_hostkeyfunc);
        rc = దత్తాంశం->సమితి.ssh_hostkeyfunc(దత్తాంశం->సమితి.ssh_hostkeyfunc_userp,
                                       (పూర్ణాంకం)keytype, remotekey, keylen);
        CURL_CBAPI_చివర(&guard);
        ఒకవేళ(rc != CURLKHMATCH_OK) {
          myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
          failf(దత్తాంశం, "SSH: callback failed host public key verification");
          తిరిగి CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(దత్తాంశం, "SSH: verified public key via callback");
      }
      లేకపోతే {
        myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
        తిరిగి CURLE_PEER_FAILED_VERIFICATION;
      }
      తిరిగి CURLE_OK;
    }
    లేకపోతే {
      CURL_TRC_SSH(దత్తాంశం, "no host key checksum given, checking knownhosts");
      తిరిగి ssh_knownhost(దత్తాంశం, sshc);
    }
  }
  లేకపోతే {
    /* as we already matched, we skip the check for known hosts */
    తిరిగి CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
స్థిర CURLcode ssh_force_knownhost_కీలకం_type(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం ssh_conn *sshc)
{
  CURLcode ఫలితం = CURLE_OK;

  స్థిర స్థిరరాశి అక్షరం hostkey_పద్ధతి_ssh_ed25519[] = "ssh-ed25519";
  స్థిర స్థిరరాశి అక్షరం hostkey_పద్ధతి_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  స్థిర స్థిరరాశి అక్షరం hostkey_పద్ధతి_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  స్థిర స్థిరరాశి అక్షరం hostkey_పద్ధతి_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  స్థిర స్థిరరాశి అక్షరం hostkey_పద్ధతి_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#నిర్వచితమైతే LIBSSH2_KNOWNHOST_కీలకం_SSHDSS
  స్థిర స్థిరరాశి అక్షరం hostkey_పద్ధతి_ssh_dss[] = "ssh-dss";
#అయితే_ముగింపు
  bool దొరికింది = FALSE;

  ఒకవేళ(sshc->kh &&
     !CURL_EASY_STR(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_MD5) &&
     !CURL_EASY_STR(దత్తాంశం, STRING_SSH_HOST_ప్రజా_కీలకం_SHA256)) {
    నిర్మాణం libssh2_knownhost *store = ఖాళీ;
    నిర్మాణం connectdata *conn = దత్తాంశం->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_కీలకం_ముసుగు >>
                      LIBSSH2_KNOWNHOST_కీలకం_జరుపు) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    అయితే(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      ఒకవేళ(store) {
        ఒకవేళ(store->name) {
          ఒకవేళ(store->name[0] == '[') {
            curl_off_t పోర్టు;
            size_t kh_name_కొలత = 0;
            స్థిరరాశి అక్షరం *p;
            స్థిరరాశి అక్షరం *kh_name_చివర = అక్షరశ్రేణి_శోధన(store->name, "]:");
            ఒకవేళ(!kh_name_చివర) {
              infof(దత్తాంశం, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS));
              కొనసాగు;
            }
            p = kh_name_చివర + 2; /* start of port number */
            ఒకవేళ(!curlx_str_సంఖ్య(&p, &పోర్టు, 0xffff) &&
               (kh_name_చివర && (పోర్టు == conn->origin->పోర్టు))) {
              kh_name_కొలత = అక్షరశ్రేణి_పొడవు(store->name) - 1 - అక్షరశ్రేణి_పొడవు(kh_name_చివర);
              ఒకవేళ((kh_name_కొలత == అక్షరశ్రేణి_పొడవు(conn->origin->hostname)) &&
                 !అక్షరశ్రేణి_పోలిక(store->name + 1, conn->origin->hostname,
                          kh_name_కొలత)) {
                దొరికింది = TRUE;
                ఆపు;
              }
            }
          }
          లేకపోతే ఒకవేళ(!strcmp(store->name, conn->origin->hostname)) {
            దొరికింది = TRUE;
            ఆపు;
          }
        }
        లేకపోతే ఒకవేళ(store->కీలకం) {
          గుర్తులేని పూర్ణాంకం type = (store->typemask & LIBSSH2_KNOWNHOST_కీలకం_ముసుగు)
                              >> LIBSSH2_KNOWNHOST_కీలకం_జరుపు;
          ఒకవేళ(!absent_type[type]) {
            పూర్ణాంకం keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->పోర్టు != పోర్టు_SSH) ? conn->origin->పోర్టు : -1,
              store->కీలకం, అక్షరశ్రేణి_పొడవు(store->కీలకం),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_కీలకం_ముసుగు),
              ఖాళీ);
            ఒకవేళ(keycheck == LIBSSH2_KNOWNHOST_CHECK_సరిపోలిక) {
              దొరికింది = TRUE;
              ఆపు;
            }
            లేకపోతే ఒకవేళ(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    ఒకవేళ(దొరికింది) {
      పూర్ణాంకం rc;
      స్థిరరాశి అక్షరం *hostkey_పద్ధతి = ఖాళీ;
      infof(దత్తాంశం, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS));

      ఎంపిక(store->typemask & LIBSSH2_KNOWNHOST_కీలకం_ముసుగు) {
      స్థితి LIBSSH2_KNOWNHOST_కీలకం_ED25519:
        hostkey_పద్ధతి = hostkey_పద్ధతి_ssh_ed25519;
        ఆపు;
      స్థితి LIBSSH2_KNOWNHOST_కీలకం_ECDSA_521:
        hostkey_పద్ధతి = hostkey_పద్ధతి_ssh_ecdsa_521;
        ఆపు;
      స్థితి LIBSSH2_KNOWNHOST_కీలకం_ECDSA_384:
        hostkey_పద్ధతి = hostkey_పద్ధతి_ssh_ecdsa_384;
        ఆపు;
      స్థితి LIBSSH2_KNOWNHOST_కీలకం_ECDSA_256:
        hostkey_పద్ధతి = hostkey_పద్ధతి_ssh_ecdsa_256;
        ఆపు;
      స్థితి LIBSSH2_KNOWNHOST_కీలకం_SSHRSA:
        hostkey_పద్ధతి = hostkey_పద్ధతి_ssh_rsa_all;
        ఆపు;
#నిర్వచితమైతే LIBSSH2_KNOWNHOST_కీలకం_SSHDSS
      స్థితి LIBSSH2_KNOWNHOST_కీలకం_SSHDSS:  /* deprecated upstream */
        hostkey_పద్ధతి = hostkey_పద్ధతి_ssh_dss;
        ఆపు;
#అయితే_ముగింపు
      స్థితి LIBSSH2_KNOWNHOST_కీలకం_RSA1:
        failf(దత్తాంశం, "Found host key type RSA1 which is not supported");
        తిరిగి CURLE_SSH;
      అప్రమేయ:
        failf(దత్తాంశం, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_కీలకం_ముసుగు));
        తిరిగి CURLE_SSH;
      }

      infof(దత్తాంశం, "SSH: set '%s' as hostkey type", hostkey_పద్ధతి);
      rc = libssh2_session_పద్ధతి_pref(sshc->ssh_session,
                                       LIBSSH2_పద్ధతి_HOSTKEY, hostkey_పద్ధతి);
      ఒకవేళ(rc) {
        అక్షరం *err_సందేశం = ఖాళీ;
        పూర్ణాంకం errlen;
        libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, &errlen, 0);
        failf(దత్తాంశం, "libssh2 method '%s' failed: %s", hostkey_పద్ధతి, err_సందేశం);
        ఫలితం = libssh2_session_error_గమ్యం_CURLE(rc);
      }
    }
    లేకపోతే {
      infof(దత్తాంశం, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS));
    }
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode quote_error(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ssh_conn *sshc)
{
  failf(దత్తాంశం, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  తిరిగి CURLE_QUOTE_ERROR;
}

స్థిర CURLcode sftp_quote(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం ssh_conn *sshc,
                           నిర్మాణం SSHPROTO *sshp)
{
  స్థిరరాశి అక్షరం *cp;
  CURLcode ఫలితం = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  స్థిరరాశి అక్షరం *cmd = sshc->quote_అంశం->దత్తాంశం;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ఒకవేళ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ఒకవేళ(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    అక్షరం *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->మార్గం);
    ఒకవేళ(!tmp)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_బహిర్గతం, "PWD\n", 4);
    Curl_debug(దత్తాంశం, CURLINFO_శీర్షిక_లోపల, tmp, అక్షరశ్రేణి_పొడవు(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, tmp, అక్షరశ్రేణి_పొడవు(tmp));
    curlx_free(tmp);
    ఒకవేళ(!ఫలితం)
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
    తిరిగి ఫలితం;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ఒకవేళ(!cp) {
    failf(దత్తాంశం, "Syntax error command '%s', missing parameter", cmd);
    తిరిగి CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  ఫలితం = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం != CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
      failf(దత్తాంశం, "Syntax error: Bad first parameter to '%s'", cmd);
    తిరిగి ఫలితం;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "chgrp ", 6) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "chmod ", 6) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "chown ", 6) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "atime ", 6) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    ఫలితం = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం != CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
        failf(దత్తాంశం, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      తిరిగి ఫలితం;
    }
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);

    memset(&sshp->quote_attrs, 0, పరిమాణం(LIBSSH2_SFTP_లక్షణాలు));
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_STAT);
    తిరిగి ఫలితం;
  }
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "ln ", 3) ||
     !అక్షరశ్రేణి_పోలిక(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    ఫలితం = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం != CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
        failf(దత్తాంశం, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      తిరిగి ఫలితం;
    }
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_SYMLINK);
    తిరిగి ఫలితం;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "mkdir ", 6)) {
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    /* create directory */
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_MKDIR);
    తిరిగి ఫలితం;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    ఫలితం = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ఒకవేళ(ఫలితం) {
      ఒకవేళ(ఫలితం != CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం)
        failf(దత్తాంశం, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      తిరిగి ఫలితం;
    }
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_RENAME);
    తిరిగి ఫలితం;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "rmdir ", 6)) {
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    /* delete directory */
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_RMDIR);
    తిరిగి ఫలితం;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "rm ", 3)) {
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_UNLINK);
    తిరిగి ఫలితం;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "statvfs ", 8)) {
    ఒకవేళ(*cp)
      తిరిగి quote_error(దత్తాంశం, sshc);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_STATVFS);
    తిరిగి ఫలితం;
  }

  failf(దత్తాంశం, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  తిరిగి CURLE_QUOTE_ERROR;
}

స్థిర CURLcode sftp_upload_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ssh_conn *sshc,
                                 నిర్మాణం SSHPROTO *sshp,
                                 bool *blockp)
{
  గుర్తులేని దీర్ఘ flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  ఒకవేళ(దత్తాంశం->state.resume_from) {
    LIBSSH2_SFTP_లక్షణాలు attrs;
    ఒకవేళ(దత్తాంశం->state.resume_from < 0) {
      పూర్ణాంకం rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->మార్గం,
                                    curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshp->మార్గం)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        తిరిగి CURLE_OK;
      }
      ఒకవేళ(rc) {
        దత్తాంశం->state.resume_from = 0;
      }
      లేకపోతే {
        curl_off_t కొలత = attrs.filesize;
        ఒకవేళ(కొలత < 0) {
          failf(దత్తాంశం, "Bad file size (%" FMT_OFF_T ")", కొలత);
          తిరిగి CURLE_BAD_DOWNLOAD_RESUME;
        }
        దత్తాంశం->state.resume_from = attrs.filesize;
      }
    }
  }

  ఒకవేళ(దత్తాంశం->సమితి.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  లేకపోతే ఒకవేళ(దత్తాంశం->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  లేకపోతే {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->మార్గం,
                         curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshp->మార్గం)),
                         flags, (దీర్ఘ)దత్తాంశం->సమితి.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  ఒకవేళ(!sshc->sftp_handle) {
    CURLcode ఫలితం;
    గుర్తులేని దీర్ఘ sftperr;
    పూర్ణాంకం rc = libssh2_session_చివరి_errno(sshc->ssh_session);

    ఒకవేళ(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      తిరిగి CURLE_OK;
    }

    ఒకవేళ(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    లేకపోతే
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    ఒకవేళ(sshc->రెండవ_Create_Dirs) {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      failf(దత్తాంశం, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      తిరిగి sftp_libssh2_error_గమ్యం_CURLE(sftperr);
    }
    ఒకవేళ((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_మార్గం) &&
       దత్తాంశం->సమితి.ftp_create_missing_dirs &&
       (అక్షరశ్రేణి_పొడవు(sshp->మార్గం) > 1)) {
      /* try to create the path remotely */
      sshc->రెండవ_Create_Dirs = 1;
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      తిరిగి CURLE_OK;
    }
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    ఫలితం = sftp_libssh2_error_గమ్యం_CURLE(sftperr);
    ఒకవేళ(!ఫలితం) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      ఫలితం = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(దత్తాంశం, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    తిరిగి ఫలితం;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  ఒకవేళ(దత్తాంశం->state.resume_from > 0 && !దత్తాంశం->సమితి.remote_append) {
    పూర్ణాంకం seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    ఒకవేళ(దత్తాంశం->సమితి.seek_func) {
      నిర్మాణం Curl_mapi_guard guard;
      CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_seek_func);
      seekerr = దత్తాంశం->సమితి.seek_func(దత్తాంశం->సమితి.seek_client,
                                    దత్తాంశం->state.resume_from, SEEK_SET);
      CURL_CBAPI_చివర(&guard);
    }

    ఒకవేళ(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      ఒకవేళ(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(దత్తాంశం, "Could not seek stream");
        తిరిగి CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      చేయి {
        నిర్మాణం Curl_mapi_guard guard;
        అక్షరం scratch[4 * 1024];
        size_t readthisamountnow =
          (దత్తాంశం->state.resume_from - passed >
           (curl_off_t)పరిమాణం(scratch)) ?
          పరిమాణం(scratch) : curlx_sotouz(దత్తాంశం->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fread_func);
        actuallyread = దత్తాంశం->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              దత్తాంశం->state.లోపల);
        CURL_CBAPI_చివర(&guard);

        passed += actuallyread;
        ఒకవేళ((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(దత్తాంశం, "Failed to read data");
          తిరిగి CURLE_FTP_COULDNT_USE_REST;
        }
      } అయితే(passed < దత్తాంశం->state.resume_from);
    }

    /* now, decrease the size of the read */
    ఒకవేళ(దత్తాంశం->state.infilesize > 0) {
      దత్తాంశం->state.infilesize -= దత్తాంశం->state.resume_from;
      దత్తాంశం->req.కొలత = దత్తాంశం->state.infilesize;
      Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)దత్తాంశం->state.resume_from);
  }
  ఒకవేళ(దత్తాంశం->state.infilesize > 0) {
    దత్తాంశం->req.కొలత = దత్తాంశం->state.infilesize;
    Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);
  }
  /* upload data */
  Curl_xfer_అమరిక_send(దత్తాంశం, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  దత్తాంశం->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(దత్తాంశం);

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య ssh_state_pkey_init(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  ఒకవేళ((దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     అక్షరశ్రేణి_శోధన(sshc->authlist, "publickey")) {
    ఒకవేళ(sshc->pub_కీలకం)
      infof(దత్తాంశం, "SSH: trying public key file '%s'", sshc->pub_కీలకం);
    infof(దత్తాంశం, "SSH: trying private key file '%s'", sshc->priv_కీలకం);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PKEY);
  }
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PASS_INIT);
}

స్థిర CURLcode sftp_quote_stat(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ssh_conn *sshc,
                                నిర్మాణం SSHPROTO *sshp,
                                bool *blockp)
{
  స్థిరరాశి అక్షరం *cmd = sshc->quote_అంశం->దత్తాంశం;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  ఒకవేళ(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ఒకవేళ(అక్షరశ్రేణి_పోలిక(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    పూర్ణాంకం rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      తిరిగి CURLE_OK;
    }
    ఒకవేళ(rc && !sshc->acceptfail) { /* get those attributes */
      గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
      failf(దత్తాంశం, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      వెళ్ళు fail;
    }
  }

  /* Now set the new attributes... */
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "chgrp", 5)) {
    స్థిరరాశి అక్షరం *p = sshc->quote_path1;
    curl_off_t gid;
    ఒకవేళ(!curlx_str_సంఖ్య(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (గుర్తులేని దీర్ఘ)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    లేకపోతే ఒకవేళ(!sshc->acceptfail) {
      failf(దత్తాంశం, "Syntax error: chgrp gid not a number");
      వెళ్ళు fail;
    }
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "chmod", 5)) {
    curl_off_t perms;
    స్థిరరాశి అక్షరం *p = sshc->quote_path1;
    /* permissions are octal */
    ఒకవేళ(curlx_str_octal(&p, &perms, 07777)) {
      failf(దత్తాంశం, "Syntax error: chmod permissions not a number");
      వెళ్ళు fail;
    }

    sshp->quote_attrs.permissions = (గుర్తులేని దీర్ఘ)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "chown", 5)) {
    స్థిరరాశి అక్షరం *p = sshc->quote_path1;
    curl_off_t uid;
    ఒకవేళ(!curlx_str_సంఖ్య(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (గుర్తులేని దీర్ఘ)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    లేకపోతే ఒకవేళ(!sshc->acceptfail) {
      failf(దత్తాంశం, "Syntax error: chown uid not a number");
      వెళ్ళు fail;
    }
  }
  లేకపోతే ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "atime", 5) ||
          !అక్షరశ్రేణి_పోలిక(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    ఒకవేళ(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(దత్తాంశం, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ఒకవేళ SIZEOF_సమయం_T > SIZEOF_LONG
    ఒకవేళ(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(దత్తాంశం, "date overflow");
      fail = TRUE;
    }
#అయితే_ముగింపు
    ఒకవేళ(fail)
      వెళ్ళు fail;
    ఒకవేళ(!అక్షరశ్రేణి_పోలిక(cmd, "atime", 5))
      sshp->quote_attrs.atime = (గుర్తులేని దీర్ఘ)date;
    లేకపోతే /* mtime */
      sshp->quote_attrs.mtime = (గుర్తులేని దీర్ఘ)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_SETSTAT);
  తిరిగి CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  తిరిగి CURLE_QUOTE_ERROR;
}

స్థిర CURLcode sftp_download_stat(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ssh_conn *sshc,
                                   నిర్మాణం SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_లక్షణాలు attrs;
  పూర్ణాంకం rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->మార్గం,
                                curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshp->మార్గం)),
                                LIBSSH2_SFTP_STAT, &attrs);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    తిరిగి CURLE_OK;
  }
  ఒకవేళ(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_కొలత) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    దత్తాంశం->req.కొలత = -1;
    దత్తాంశం->req.maxdownload = -1;
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  లేకపోతే {
    curl_off_t కొలత = attrs.filesize;

    ఒకవేళ(కొలత < 0) {
      failf(దత్తాంశం, "Bad file size (%" FMT_OFF_T ")", కొలత);
      తిరిగి CURLE_BAD_DOWNLOAD_RESUME;
    }
    ఒకవేళ(దత్తాంశం->state.use_range) {
      curl_off_t from;
      CURLcode ఫలితం = Curl_ssh_range(దత్తాంశం, దత్తాంశం->state.range, కొలత,
                                       &from, &కొలత);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    దత్తాంశం->req.కొలత = కొలత;
    దత్తాంశం->req.maxdownload = కొలత;
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, కొలత);
  }

  /* We can resume if we can seek to the resume position */
  ఒకవేళ(దత్తాంశం->state.resume_from) {
    ఒకవేళ(దత్తాంశం->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ఒకవేళ((curl_off_t)attrs.filesize < -దత్తాంశం->state.resume_from) {
        failf(దత్తాంశం, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              దత్తాంశం->state.resume_from, (curl_off_t)attrs.filesize);
        తిరిగి CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      దత్తాంశం->state.resume_from += attrs.filesize;
    }
    లేకపోతే {
      ఒకవేళ((curl_off_t)attrs.filesize < దత్తాంశం->state.resume_from) {
        failf(దత్తాంశం, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              దత్తాంశం->state.resume_from, (curl_off_t)attrs.filesize);
        తిరిగి CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    దత్తాంశం->req.కొలత = attrs.filesize - దత్తాంశం->state.resume_from;
    దత్తాంశం->req.maxdownload = attrs.filesize - దత్తాంశం->state.resume_from;
    Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, attrs.filesize - దత్తాంశం->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)దత్తాంశం->state.resume_from);
  }

  /* Setup the actual download */
  ఒకవేళ(దత్తాంశం->req.కొలత == 0) {
    /* no data to transfer */
    Curl_xfer_అమరిక_nop(దత్తాంశం);
    infof(దత్తాంశం, "SSH: file already completely downloaded");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
    తిరిగి CURLE_OK;
  }
  Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, దత్తాంశం->req.కొలత);

  /* not set by Curl_xfer_setup to preserve io_flags */
  దత్తాంశం->conn->send_idx = 0;

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);

  తిరిగి CURLE_OK;
}

స్థిర CURLcode sftp_readdir(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ssh_conn *sshc,
                             నిర్మాణం SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_మార్గం_MAX,
                                   sshp->readdir_longentry, CURL_మార్గం_MAX,
                                   &sshp->readdir_attrs);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    తిరిగి ఫలితం;
  }
  ఒకవేళ(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    ఒకవేళ(దత్తాంశం->సమితి.list_only) {
      ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      ఒకవేళ(!ఫలితం)
        ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY, "\n", 1);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    లేకపోతే {
      ఫలితం = curlx_dyn_కూడు(&sshp->readdir, sshp->readdir_longentry);

      ఒకవేళ(!ఫలితం) {
        ఒకవేళ((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          ఫలితం = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->మార్గం,
                                  sshp->readdir_filename);
          myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_LINK);
        }
        లేకపోతే {
          myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      తిరిగి ఫలితం;
    }
  }
  లేకపోతే ఒకవేళ(!rc) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_పూర్తి);
  }
  లేకపోతే {
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    ఫలితం = sftperr ? sftp_libssh2_error_గమ్యం_CURLE(sftperr) : CURLE_SSH;
    failf(దత్తాంశం, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_చివరి_errno(sshc->ssh_session));
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_init(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ssh_conn *sshc)
{
  CURLcode ఫలితం;
  sshc->రెండవ_Create_Dirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_సమితి_blocking(sshc->ssh_session, 0);

  ఫలితం = ssh_force_knownhost_కీలకం_type(దత్తాంశం, sshc);
  ఒకవేళ(ఫలితం)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_S_STARTUP);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_startup(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ssh_conn *sshc)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  పూర్ణాంకం rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc) {
    అక్షరం *err_సందేశం = ఖాళీ;
    (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
    failf(దత్తాంశం, "Failure establishing ssh session: %d, %s", rc, err_సందేశం);

    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
    తిరిగి CURLE_FAILED_INIT;
  }

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_HOSTKEY);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_hostkey(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode ఫలితం = ssh_check_fingerprint(దత్తాంశం, sshc);
  ఒకవేళ(!ఫలితం)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTHLIST);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_authlist(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ssh_conn *sshc)
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
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి అక్షరం *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(అక్షరశ్రేణి_పొడవు(user)));

  ఒకవేళ(!sshc->authlist) {
    పూర్ణాంకం rc;
    ఒకవేళ(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(దత్తాంశం, "SSH: user accepted with no authentication");
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
      తిరిగి CURLE_OK;
    }
    rc = libssh2_session_చివరి_errno(sshc->ssh_session);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
    తిరిగి libssh2_session_error_గమ్యం_CURLE(rc);
  }
  infof(దత్తాంశం, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PKEY_INIT);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_auth_pkey(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి అక్షరం *user = Curl_creds_user(conn->creds);
  పూర్ణాంకం rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(అక్షరశ్రేణి_పొడవు(user)),
    sshc->pub_కీలకం, sshc->priv_కీలకం, sshc->passphrase ? sshc->passphrase : "");
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc == 0) {
    sshc->authed = TRUE;
    infof(దత్తాంశం, "SSH: authenticated via publickey");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
  }
  లేకపోతే {
    అక్షరం *err_సందేశం = ఖాళీ;
    అక్షరం unknown[] = "Reason unknown (-1)";
    ఒకవేళ(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_సందేశం = unknown;
    }
    లేకపోతే {
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
    }
    infof(దత్తాంశం, "SSH: publickey authentication denied: %s", err_సందేశం);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PASS_INIT);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_auth_pass_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ((దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     అక్షరశ్రేణి_శోధన(sshc->authlist, "password")) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_PASS);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_HOST_INIT);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_auth_pass(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి అక్షరం *user = Curl_creds_user(conn->creds);
  స్థిరరాశి అక్షరం *passwd = Curl_creds_passwd(conn->creds);
  పూర్ణాంకం rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(అక్షరశ్రేణి_పొడవు(user)),
                                 passwd,
                                 curlx_uztoui(అక్షరశ్రేణి_పొడవు(passwd)),
                                 ఖాళీ);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN) {
    తిరిగి CURLE_AGAIN;
  }
  ఒకవేళ(rc == 0) {
    sshc->authed = TRUE;
    infof(దత్తాంశం, "SSH: initialized password authentication");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_HOST_INIT);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_auth_host_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ((దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     అక్షరశ్రేణి_శోధన(sshc->authlist, "hostbased")) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_HOST);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_AGENT_INIT);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_auth_agent_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = 0;
  ఒకవేళ((దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     అక్షరశ్రేణి_శోధన(sshc->authlist, "publickey")) {

    infof(దత్తాంశం, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    ఒకవేళ(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      ఒకవేళ(!sshc->ssh_agent) {
        infof(దత్తాంశం, "SSH: could not create agent object");

        myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_కీలకం_INIT);
        తిరిగి CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;
    ఒకవేళ(rc < 0) {
      infof(దత్తాంశం, "SSH: failure connecting to agent");
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_కీలకం_INIT);
    }
    లేకపోతే {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_కీలకం_INIT);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_auth_agent_list(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = libssh2_agent_list_identities(sshc->ssh_agent);

  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;
  ఒకవేళ(rc < 0) {
    infof(దత్తాంశం, "SSH: failure requesting identities to agent");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_కీలకం_INIT);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_తటస్థం = ఖాళీ;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_auth_agent(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  పూర్ణాంకం rc = libssh2_agent_get_తటస్థం(sshc->ssh_agent,
                                      &sshc->sshagent_తటస్థం,
                                      sshc->sshagent_prev_తటస్థం);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc == 0) {
    CURL_TRC_SSH(దత్తాంశం, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(దత్తాంశం->conn->creds),
                 sshc->sshagent_తటస్థం->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(దత్తాంశం->conn->creds),
                                sshc->sshagent_తటస్థం);

    ఒకవేళ(rc < 0) {
      ఒకవేళ(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_తటస్థం = sshc->sshagent_తటస్థం;
        తిరిగి CURLE_OK;
      }
      తిరిగి CURLE_AGAIN;
    }
  }

  ఒకవేళ(rc < 0)
    infof(దత్తాంశం, "SSH: failure requesting identities to agent");
  లేకపోతే ఒకవేళ(rc == 1)
    infof(దత్తాంశం, "SSH: no agent identity would match");

  ఒకవేళ(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(దత్తాంశం, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(దత్తాంశం->conn->creds),
          sshc->sshagent_తటస్థం->comment);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_కీలకం_INIT);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_auth_కీలకం_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ((దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     అక్షరశ్రేణి_శోధన(sshc->authlist, "keyboard-interactive")) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_కీలకం);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_auth_కీలకం(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  స్థిరరాశి అక్షరం *user = Curl_creds_user(conn->creds);
  పూర్ణాంకం rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(అక్షరశ్రేణి_పొడవు(user)),
                                             &kbd_callback);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc == 0) {
    sshc->authed = TRUE;
    infof(దత్తాంశం, "SSH: initialized keyboard interactive authentication");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_పూర్తి);
    తిరిగి CURLE_OK;
  }
  తిరిగి CURLE_LOGIN_DENIED;
}

స్థిర CURLcode ssh_state_auth_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  ఒకవేళ(!sshc->authed) {
    failf(దత్తాంశం, "Authentication failure");
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
    తిరిగి CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(దత్తాంశం, "SSH: authentication complete");

  Curl_pgrs_సమయం(దత్తాంశం, TIMER_APPCONNECT); /* SSH is connected */

  దత్తాంశం->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ఒకవేళ(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_INIT);
    తిరిగి CURLE_OK;
  }
  infof(దత్తాంశం, "SSH: connection established");
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  ఒకవేళ(!sshc->sftp_session) {
    అక్షరం *err_సందేశం = ఖాళీ;
    ఒకవేళ(libssh2_session_చివరి_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
    failf(దత్తాంశం, "Failure initializing sftp session: %s", err_సందేశం);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
    తిరిగి CURLE_FAILED_INIT;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_REALPATH);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_realpath(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ssh_conn *sshc,
                                        నిర్మాణం SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  పూర్ణాంకం rc;

  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_మార్గం_MAX,
                               LIBSSH2_SFTP_REALPATH);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  ఒకవేళ(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    ఒకవేళ(!sshc->homedir)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    curlx_free(దత్తాంశం->state.most_recent_ftp_entrypath);
    దత్తాంశం->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    ఒకవేళ(!దత్తాంశం->state.most_recent_ftp_entrypath)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }
  లేకపోతే {
    /* Return the error type */
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    CURLcode ఫలితం;
    ఒకవేళ(sftperr)
      ఫలితం = sftp_libssh2_error_గమ్యం_CURLE(sftperr);
    లేకపోతే
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      ఫలితం = CURLE_SSH;
    CURL_TRC_SSH(దత్తాంశం, "error = %lu makes libcurl = %d", sftperr, (పూర్ణాంకం)ఫలితం);
    తిరిగి ఫలితం;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(దత్తాంశం, "CONNECT phase done");
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_quote_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం ssh_conn *sshc,
                                          నిర్మాణం SSHPROTO *sshp)
{
  CURLcode ఫలితం;

  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = Curl_getworkingpath(దత్తాంశం, sshc->homedir, &sshp->మార్గం);
  ఒకవేళ(ఫలితం) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
    తిరిగి ఫలితం;
  }

  ఒకవేళ(దత్తాంశం->సమితి.quote) {
    infof(దత్తాంశం, "SSH: sending quote commands");
    sshc->quote_అంశం = దత్తాంశం->సమితి.quote;
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_GETINFO);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_postquote_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                              నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(దత్తాంశం->సమితి.postquote) {
    infof(దత్తాంశం, "SSH: sending quote commands");
    sshc->quote_అంశం = దత్తాంశం->సమితి.postquote;
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE);
  }
  లేకపోతే {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_quote(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ssh_conn *sshc,
                                     నిర్మాణం SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode ఫలితం;

  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = sftp_quote(దత్తాంశం, sshc, sshp);
  ఒకవేళ(ఫలితం) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_sftp_తదుపరి_quote(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_అంశం = sshc->quote_అంశం->తదుపరి;

  ఒకవేళ(sshc->quote_అంశం) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE);
  }
  లేకపోతే {
    ఒకవేళ(sshc->nextstate != SSH_NO_STATE) {
      myssh_గమ్యం(దత్తాంశం, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    లేకపోతే {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_GETINFO);
    }
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_quote_stat(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం ssh_conn *sshc,
                                          నిర్మాణం SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode ఫలితం;

  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = sftp_quote_stat(దత్తాంశం, sshc, sshp, blockp);
  ఒకవేళ(ఫలితం) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_sftp_quote_setstat(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం ssh_conn *sshc,
                                             నిర్మాణం SSHPROTO *sshp)
{
  పూర్ణాంకం rc;

  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc && !sshc->acceptfail) {
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    failf(దత్తాంశం, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    తిరిగి CURLE_QUOTE_ERROR;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_quote_symlink(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc && !sshc->acceptfail) {
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    failf(దత్తాంశం, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    తిరిగి CURLE_QUOTE_ERROR;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_quote_mkdir(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path1)),
                                 (దీర్ఘ)దత్తాంశం->సమితి.new_directory_perms);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc && !sshc->acceptfail) {
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    failf(దత్తాంశం, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    తిరిగి CURLE_QUOTE_ERROR;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_quote_rename(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc && !sshc->acceptfail) {
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    failf(దత్తాంశం, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    తిరిగి CURLE_QUOTE_ERROR;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_quote_rmdir(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path1)));
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc && !sshc->acceptfail) {
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    failf(దత్తాంశం, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    తిరిగి CURLE_QUOTE_ERROR;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_quote_unlink(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం ssh_conn *sshc)
{
  పూర్ణాంకం rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path1)));
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc && !sshc->acceptfail) {
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    failf(దత్తాంశం, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    తిరిగి CURLE_QUOTE_ERROR;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_quote_statvfs(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  పూర్ణాంకం rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshc->quote_path1)),
                                &statvfs);

  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc && !sshc->acceptfail) {
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    failf(దత్తాంశం, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    తిరిగి CURLE_QUOTE_ERROR;
  }
  లేకపోతే ఒకవేళ(rc == 0) {
#నిర్వచితమైతే _MSC_VER
#నిర్వచించు CURL_LIBSSH2_VFS_కొలత_ముసుగు "I64u"
#లేకపోతే
#నిర్వచించు CURL_LIBSSH2_VFS_కొలత_ముసుగు "llu"
#అయితే_ముగింపు
    CURLcode ఫలితం;
    అక్షరం *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_కొలత_ముసుగు "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_ఖండాలు, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    ఒకవేళ(!tmp) {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_శీర్షిక, tmp, అక్షరశ్రేణి_పొడవు(tmp));
    curlx_free(tmp);
    ఒకవేళ(ఫలితం) {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      తిరిగి ఫలితం;
    }
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_తదుపరి_QUOTE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_create_dirs_mkdir(నిర్మాణం Curl_easy *దత్తాంశం,
                                                 నిర్మాణం ssh_conn *sshc,
                                                 నిర్మాణం SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  పూర్ణాంకం rc;

  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->మార్గం,
                             curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshp->మార్గం)),
                             (దీర్ఘ)దత్తాంశం->సమితి.new_directory_perms);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ఒకవేళ(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    గుర్తులేని దీర్ఘ sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    ఒకవేళ((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
      తిరిగి sftp_libssh2_error_గమ్యం_CURLE(sftperr);
    }
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CREATE_DIRS);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_readdir_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం ssh_conn *sshc,
                                            నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  Curl_pgrs_సమితి_Download_కొలత(దత్తాంశం, -1);
  ఒకవేళ(దత్తాంశం->req.no_body) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
    తిరిగి CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->మార్గం,
                         curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshp->మార్గం)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  ఒకవేళ(!sshc->sftp_handle) {
    గుర్తులేని దీర్ఘ sftperr;
    ఒకవేళ(libssh2_session_చివరి_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    failf(దత్తాంశం, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    తిరిగి sftp_libssh2_error_గమ్యం_CURLE(sftperr);
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_readdir_link(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం ssh_conn *sshc,
                                            నిర్మాణం SSHPROTO *sshp)
{
  CURLcode ఫలితం;
  పూర్ణాంకం rc;
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (గుర్తులేని పూర్ణాంకం)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_మార్గం_MAX, LIBSSH2_SFTP_READLINK);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  ఒకవేళ(rc < 0)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* append filename and extra output */
  ఫలితం = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  ఒకవేళ(ఫలితం)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_BOTTOM);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_scp_download_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం ssh_conn *sshc,
                                            నిర్మాణం SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, పరిమాణం(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->మార్గం, &sb);
  ఒకవేళ(!sshc->ssh_channel) {
    పూర్ణాంకం ssh_err;
    అక్షరం *err_సందేశం = ఖాళీ;

    ఒకవేళ(libssh2_session_చివరి_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    ssh_err = libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
    failf(దత్తాంశం, "%s", err_సందేశం);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_CHANNEL_FREE);
    తిరిగి libssh2_session_error_గమ్యం_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_కొలత;
  దత్తాంశం->req.maxdownload = (curl_off_t)sb.st_కొలత;
  Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  దత్తాంశం->conn->send_idx = 0;

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_close(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం ssh_conn *sshc,
                                     నిర్మాణం SSHPROTO *sshp)
{
  పూర్ణాంకం rc = 0;
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    ఒకవేళ(rc < 0) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to close libssh2 file: %d %s", rc, err_సందేశం);
    }
    sshc->sftp_handle = ఖాళీ;
  }

  curlx_safefree(sshp->మార్గం);

  CURL_TRC_SSH(దత్తాంశం, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  ఒకవేళ(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_గమ్యం(దత్తాంశం, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);

  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_shutdown(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  పూర్ణాంకం rc = 0;
  ఒకవేళ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    ఒకవేళ(rc < 0) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to close libssh2 file: %d %s", rc, err_సందేశం);
    }
    sshc->sftp_handle = ఖాళీ;
  }
  ఒకవేళ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    ఒకవేళ(rc < 0) {
      infof(దత్తాంశం, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = ఖాళీ;
  }

  curlx_safefree(sshc->homedir);

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_DISCONNECT);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_download_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం ssh_conn *sshc,
                                             నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->మార్గం,
                         curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshp->మార్గం)),
                         LIBSSH2_FXF_READ, (దీర్ఘ)దత్తాంశం->సమితి.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  ఒకవేళ(!sshc->sftp_handle) {
    గుర్తులేని దీర్ఘ sftperr;
    ఒకవేళ(libssh2_session_చివరి_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      తిరిగి CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_చివరి_error(sshc->sftp_session);
    failf(దత్తాంశం, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    తిరిగి sftp_libssh2_error_గమ్యం_CURLE(sftperr);
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_DOWNLOAD_STAT);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_scp_upload_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం ssh_conn *sshc,
                                          నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->మార్గం,
                       (పూర్ణాంకం)దత్తాంశం->సమితి.new_file_perms,
                       (libssh2_int64_t)దత్తాంశం->state.infilesize, 0, 0);
  ఒకవేళ(!sshc->ssh_channel) {
    పూర్ణాంకం ssh_err;
    అక్షరం *err_సందేశం = ఖాళీ;
    CURLcode ఫలితం;
    ఒకవేళ(libssh2_session_చివరి_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    ssh_err = libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
    failf(దత్తాంశం, "%s", err_సందేశం);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_CHANNEL_FREE);
    ఫలితం = libssh2_session_error_గమ్యం_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    ఒకవేళ(ఫలితం == CURLE_SSH ||
       ఫలితం == CURLE_REMOTE_FILE_NOT_దొరికింది)
      ఫలితం = CURLE_UPLOAD_FAILED;
    తిరిగి ఫలితం;
  }

  /* upload data */
  దత్తాంశం->req.కొలత = దత్తాంశం->state.infilesize;
  Curl_pgrs_సమితి_Upload_కొలత(దత్తాంశం, దత్తాంశం->state.infilesize);
  Curl_xfer_అమరిక_send(దత్తాంశం, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  దత్తాంశం->conn->recv_idx = FIRSTSOCKET;

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);

  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_session_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  పూర్ణాంకం rc = 0;
  ఒకవేళ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    ఒకవేళ(rc < 0) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to free libssh2 scp subsystem: %d %s", rc, err_సందేశం);
    }
    sshc->ssh_channel = ఖాళీ;
  }

  ఒకవేళ(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;

    ఒకవేళ(rc < 0) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to disconnect libssh2 session: %d %s", rc, err_సందేశం);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_FREE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode sshc_cleanup(నిర్మాణం ssh_conn *sshc, నిర్మాణం Curl_easy *దత్తాంశం,
                             bool ఖండం)
{
  పూర్ణాంకం rc;

  ఒకవేళ(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = ఖాళీ;
  }

  ఒకవేళ(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    ఒకవేళ((rc < 0) && దత్తాంశం) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_సందేశం);
    }
    ఒకవేళ(!ఖండం && (rc == LIBSSH2_ERROR_EAGAIN))
      తిరిగి CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = ఖాళీ;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_తటస్థం = ఖాళీ;
    sshc->sshagent_prev_తటస్థం = ఖాళీ;
  }

  ఒకవేళ(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ఒకవేళ((rc < 0) && దత్తాంశం) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to close libssh2 file: %d %s", rc, err_సందేశం);
    }
    ఒకవేళ(!ఖండం && (rc == LIBSSH2_ERROR_EAGAIN))
      తిరిగి CURLE_AGAIN;

    sshc->sftp_handle = ఖాళీ;
  }

  ఒకవేళ(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ఒకవేళ((rc < 0) && దత్తాంశం) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to free libssh2 scp subsystem: %d %s", rc, err_సందేశం);
    }
    ఒకవేళ(!ఖండం && (rc == LIBSSH2_ERROR_EAGAIN))
      తిరిగి CURLE_AGAIN;

    sshc->ssh_channel = ఖాళీ;
  }

  ఒకవేళ(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ఒకవేళ((rc < 0) && దత్తాంశం) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_సందేశం);
    }
    ఒకవేళ(!ఖండం && (rc == LIBSSH2_ERROR_EAGAIN))
      తిరిగి CURLE_AGAIN;

    sshc->sftp_session = ఖాళీ;
  }

  ఒకవేళ(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    ఒకవేళ((rc < 0) && దత్తాంశం) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session, &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to free libssh2 session: %d %s", rc, err_సందేశం);
    }
    ఒకవేళ(!ఖండం && (rc == LIBSSH2_ERROR_EAGAIN))
      తిరిగి CURLE_AGAIN;

    sshc->ssh_session = ఖాళీ;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_కీలకం);
  curlx_safefree(sshc->priv_కీలకం);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_getinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(దత్తాంశం->సమితి.get_filetime)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_FILETIME);
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_TRANS_INIT);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_filetime(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ssh_conn *sshc,
                                        నిర్మాణం SSHPROTO *sshp)
{
  LIBSSH2_SFTP_లక్షణాలు attrs;
  పూర్ణాంకం rc;

  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->మార్గం,
                            curlx_uztoui(అక్షరశ్రేణి_పొడవు(sshp->మార్గం)),
                            LIBSSH2_SFTP_STAT, &attrs);
  ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  ఒకవేళ(rc == 0)
    దత్తాంశం->info.filetime = (time_t)attrs.mtime;

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_TRANS_INIT);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_trans_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                          నిర్మాణం ssh_conn *sshc,
                                          నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(దత్తాంశం->state.upload)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_UPLOAD_INIT);
  లేకపోతే ఒకవేళ(sshp) {
    size_t plen = అక్షరశ్రేణి_పొడవు(sshp->మార్గం);
    ఒకవేళ(plen && (sshp->మార్గం[plen - 1] == '/'))
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR_INIT);
    లేకపోతే
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  లేకపోతే
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_upload_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం ssh_conn *sshc,
                                           నిర్మాణం SSHPROTO *sshp,
                                           bool *ఖండం)
{
  CURLcode ఫలితం;
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = sftp_upload_init(దత్తాంశం, sshc, sshp, ఖండం);
  ఒకవేళ(ఫలితం) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_sftp_create_dirs_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                                నిర్మాణం ssh_conn *sshc,
                                                నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  ఒకవేళ(అక్షరశ్రేణి_పొడవు(sshp->మార్గం) > 1) {
    sshc->slash_pos = sshp->మార్గం + 1; /* ignore the leading '/' */
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CREATE_DIRS);
  }
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_UPLOAD_INIT);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_create_dirs(నిర్మాణం Curl_easy *దత్తాంశం,
                                           నిర్మాణం ssh_conn *sshc,
                                           నిర్మాణం SSHPROTO *sshp)
{
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ఒకవేళ(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(దత్తాంశం, "SFTP: creating directory '%s'", sshp->మార్గం);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    తిరిగి CURLE_OK;
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_UPLOAD_INIT);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_readdir(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc,
                                       నిర్మాణం SSHPROTO *sshp,
                                       bool *ఖండం)
{
  CURLcode ఫలితం;
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = sftp_readdir(దత్తాంశం, sshc, sshp, ఖండం);
  ఒకవేళ(ఫలితం)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_sftp_readdir_bottom(నిర్మాణం Curl_easy *దత్తాంశం,
                                              నిర్మాణం ssh_conn *sshc,
                                              నిర్మాణం SSHPROTO *sshp)
{
  CURLcode ఫలితం;
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_client_write(దత్తాంశం, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  ఒకవేళ(ఫలితం) {
    curlx_dyn_free(&sshp->readdir);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  }
  లేకపోతే {
    curlx_dyn_reset(&sshp->readdir);
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_READDIR);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_sftp_readdir_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                                            నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;

  sshc->sftp_handle = ఖాళీ;

  /* no data to transfer */
  Curl_xfer_అమరిక_nop(దత్తాంశం);
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_sftp_download_stat(నిర్మాణం Curl_easy *దత్తాంశం,
                                             నిర్మాణం ssh_conn *sshc,
                                             నిర్మాణం SSHPROTO *sshp,
                                             bool *ఖండం)
{
  CURLcode ఫలితం;
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = sftp_download_stat(దత్తాంశం, sshc, sshp, ఖండం);
  ఒకవేళ(ఫలితం) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_state_scp_trans_init(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం ssh_conn *sshc,
                                         నిర్మాణం SSHPROTO *sshp)
{
  CURLcode ఫలితం;
  ఒకవేళ(!sshp)
    తిరిగి CURLE_FAILED_INIT;
  ఫలితం = Curl_getworkingpath(దత్తాంశం, sshc->homedir,
                               &sshp->మార్గం);
  ఒకవేళ(ఫలితం) {
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
    తిరిగి ఫలితం;
  }

  ఒకవేళ(దత్తాంశం->state.upload) {
    ఒకవేళ(దత్తాంశం->state.infilesize < 0) {
      failf(దత్తాంశం, "SCP requires a known file size for upload");
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_CHANNEL_FREE);
      తిరిగి CURLE_UPLOAD_FAILED;
    }
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_UPLOAD_INIT);
  }
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_DOWNLOAD_INIT);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_scp_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(దత్తాంశం->state.upload)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_SEND_EOF);
  లేకపోతే
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_CHANNEL_FREE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_scp_send_eof(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->ssh_channel) {
    పూర్ణాంకం rc = libssh2_channel_send_eof(sshc->ssh_channel);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;
    ఒకవేళ(rc) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session,
                                       &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_సందేశం);
    }
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_WAIT_EOF);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_scp_wait_eof(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->ssh_channel) {
    పూర్ణాంకం rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;
    ఒకవేళ(rc) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session,
                                       &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to get channel EOF: %d %s", rc, err_సందేశం);
    }
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_WAIT_CLOSE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_scp_wait_close(నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->ssh_channel) {
    పూర్ణాంకం rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;
    ఒకవేళ(rc) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session,
                                       &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Channel failed to close: %d %s", rc, err_సందేశం);
    }
  }
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_CHANNEL_FREE);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_scp_channel_free(
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం ssh_conn *sshc)
{
  ఒకవేళ(sshc->ssh_channel) {
    పూర్ణాంకం rc = libssh2_channel_free(sshc->ssh_channel);
    ఒకవేళ(rc == LIBSSH2_ERROR_EAGAIN)
      తిరిగి CURLE_AGAIN;
    ఒకవేళ(rc < 0) {
      అక్షరం *err_సందేశం = ఖాళీ;
      (శూన్య)libssh2_session_చివరి_error(sshc->ssh_session,
                                       &err_సందేశం, ఖాళీ, 0);
      infof(దత్తాంశం, "Failed to free libssh2 scp subsystem: %d %s", rc, err_సందేశం);
    }
    sshc->ssh_channel = ఖాళీ;
  }
  CURL_TRC_SSH(దత్తాంశం, "SCP DONE phase complete");
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssh_state_session_free(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం ssh_conn *sshc)
{
  CURLcode ఫలితం = sshc_cleanup(sshc, దత్తాంశం, FALSE);
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  memset(sshc, 0, పరిమాణం(నిర్మాణం ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
  తిరిగి CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
స్థిర CURLcode ssh_statemachine(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ssh_conn *sshc,
                                 నిర్మాణం SSHPROTO *sshp,
                                 bool *ఖండం)
{
  CURLcode ఫలితం = CURLE_OK;
  *ఖండం = 0; /* we are not blocking by default */

  చేయి {
    ఎంపిక(sshc->state) {
    స్థితి SSH_INIT:
      ఫలితం = ssh_state_init(దత్తాంశం, sshc);
      ఒకవేళ(ఫలితం)
        ఆపు;
      FALLTHROUGH();

    స్థితి SSH_S_STARTUP:
      ఫలితం = ssh_state_startup(దత్తాంశం, sshc);
      ఒకవేళ(ఫలితం)
        ఆపు;
      FALLTHROUGH();

    స్థితి SSH_HOSTKEY:
      ఫలితం = ssh_state_hostkey(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTHLIST:
      ఫలితం = ssh_state_authlist(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_PKEY:
      ఫలితం = ssh_state_auth_pkey(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_PASS_INIT:
      ఫలితం = ssh_state_auth_pass_init(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_PASS:
      ఫలితం = ssh_state_auth_pass(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_HOST_INIT:
      ఫలితం = ssh_state_auth_host_init(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_HOST:
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_AUTH_AGENT_INIT);
      ఆపు;

    స్థితి SSH_AUTH_AGENT_INIT:
      ఫలితం = ssh_state_auth_agent_init(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_AGENT_LIST:
      ఫలితం = ssh_state_auth_agent_list(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_AGENT:
      ఫలితం = ssh_state_auth_agent(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_కీలకం_INIT:
      ఫలితం = ssh_state_auth_కీలకం_init(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_కీలకం:
      ఫలితం = ssh_state_auth_కీలకం(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_AUTH_పూర్తి:
      ఫలితం = ssh_state_auth_పూర్తి(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_INIT:
      ఫలితం = ssh_state_sftp_init(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_REALPATH:
      ఫలితం = ssh_state_sftp_realpath(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE_INIT:
      ఫలితం = ssh_state_sftp_quote_init(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_POSTQUOTE_INIT:
      ఫలితం = ssh_state_sftp_postquote_init(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE:
      ఫలితం = ssh_state_sftp_quote(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_తదుపరి_QUOTE:
      ఫలితం = ssh_state_sftp_తదుపరి_quote(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE_STAT:
      ఫలితం = ssh_state_sftp_quote_stat(దత్తాంశం, sshc, sshp, ఖండం);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE_SETSTAT:
      ఫలితం = ssh_state_sftp_quote_setstat(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE_SYMLINK:
      ఫలితం = ssh_state_sftp_quote_symlink(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE_MKDIR:
      ఫలితం = ssh_state_sftp_quote_mkdir(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE_RENAME:
      ఫలితం = ssh_state_sftp_quote_rename(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE_RMDIR:
      ఫలితం = ssh_state_sftp_quote_rmdir(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE_UNLINK:
      ఫలితం = ssh_state_sftp_quote_unlink(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_QUOTE_STATVFS:
      ఫలితం = ssh_state_sftp_quote_statvfs(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_GETINFO:
      ఫలితం = ssh_state_sftp_getinfo(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_FILETIME:
      ఫలితం = ssh_state_sftp_filetime(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_TRANS_INIT:
      ఫలితం = ssh_state_sftp_trans_init(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_UPLOAD_INIT:
      ఫలితం = ssh_state_sftp_upload_init(దత్తాంశం, sshc, sshp, ఖండం);
      ఆపు;

    స్థితి SSH_SFTP_CREATE_DIRS_INIT:
      ఫలితం = ssh_state_sftp_create_dirs_init(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_CREATE_DIRS:
      ఫలితం = ssh_state_sftp_create_dirs(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_CREATE_DIRS_MKDIR:
      ఫలితం = ssh_state_sftp_create_dirs_mkdir(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_READDIR_INIT:
      ఫలితం = ssh_state_sftp_readdir_init(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_READDIR:
      ఫలితం = ssh_state_sftp_readdir(దత్తాంశం, sshc, sshp, ఖండం);
      ఆపు;

    స్థితి SSH_SFTP_READDIR_LINK:
      ఫలితం = ssh_state_sftp_readdir_link(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_READDIR_BOTTOM:
      ఫలితం = ssh_state_sftp_readdir_bottom(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_READDIR_పూర్తి:
      ఫలితం = ssh_state_sftp_readdir_పూర్తి(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SFTP_DOWNLOAD_INIT:
      ఫలితం = ssh_state_sftp_download_init(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_DOWNLOAD_STAT:
      ఫలితం = ssh_state_sftp_download_stat(దత్తాంశం, sshc, sshp, ఖండం);
      ఆపు;

    స్థితి SSH_SFTP_CLOSE:
      ఫలితం = ssh_state_sftp_close(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SFTP_SHUTDOWN:
      ఫలితం = ssh_state_sftp_shutdown(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SCP_TRANS_INIT:
      ఫలితం = ssh_state_scp_trans_init(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SCP_UPLOAD_INIT:
      ఫలితం = ssh_state_scp_upload_init(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SCP_DOWNLOAD_INIT:
      ఫలితం = ssh_state_scp_download_init(దత్తాంశం, sshc, sshp);
      ఆపు;

    స్థితి SSH_SCP_పూర్తి:
      ఫలితం = ssh_state_scp_పూర్తి(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SCP_SEND_EOF:
      ఫలితం = ssh_state_scp_send_eof(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SCP_WAIT_EOF:
      ఫలితం = ssh_state_scp_wait_eof(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SCP_WAIT_CLOSE:
      ఫలితం = ssh_state_scp_wait_close(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SCP_CHANNEL_FREE:
      ఫలితం = ssh_state_scp_channel_free(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SESSION_DISCONNECT:
      ఫలితం = ssh_state_session_disconnect(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_SESSION_FREE:
      ఫలితం = ssh_state_session_free(దత్తాంశం, sshc);
      ఆపు;

    స్థితి SSH_QUIT:
    అప్రమేయ:
      /* internal error */
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_STOP);
      ఆపు;
    }

  } అయితే(!ఫలితం && (sshc->state != SSH_STOP) && !*ఖండం);

  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *ఖండం = TRUE;
    ఫలితం = CURLE_OK;
  }
  CURL_TRC_SSH(దత్తాంశం, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (పూర్ణాంకం)ఫలితం, *ఖండం);

  తిరిగి ఫలితం;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
స్థిర శూన్య ssh_block2waitfor(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ssh_conn *sshc,
                              bool ఖండం)
{
  పూర్ణాంకం dir = 0;
  (శూన్య)దత్తాంశం;
  ఒకవేళ(ఖండం) {
    dir = libssh2_session_ఖండం_directions(sshc->ssh_session);
    ఒకవేళ(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_ఖండం_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_ఖండం_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  ఒకవేళ(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
స్థిర CURLcode ssh_multi_statemach(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  నిర్మాణం SSHPROTO *sshp = Curl_meta_get(దత్తాంశం, CURL_META_SSH_EASY);
  CURLcode ఫలితం = CURLE_OK;
  bool ఖండం; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  ఒకవేళ(!sshc || !sshp)
    తిరిగి CURLE_FAILED_INIT;

  చేయి {
    ఫలితం = ssh_statemachine(దత్తాంశం, sshc, sshp, &ఖండం);
    *పూర్తి = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } అయితే(!ఫలితం && !*పూర్తి && !ఖండం);
  ssh_block2waitfor(దత్తాంశం, sshc, ఖండం);

  తిరిగి ఫలితం;
}

స్థిర CURLcode ssh_ఖండం_statemach(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ssh_conn *sshc,
                                    నిర్మాణం SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం curltime ప్రారంభం = *Curl_pgrs_now(దత్తాంశం);

  అయితే((sshc->state != SSH_STOP) && !ఫలితం) {
    bool ఖండం;
    timediff_t ఎడమ_ms = 1000;

    ఫలితం = ssh_statemachine(దత్తాంశం, sshc, sshp, &ఖండం);
    ఒకవేళ(ఫలితం)
      ఆపు;

    ఒకవేళ(!disconnect) {
      ఫలితం = Curl_pgrsCheck(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        ఆపు;

      ఎడమ_ms = Curl_timeleft_ms(దత్తాంశం);
      ఒకవేళ(ఎడమ_ms < 0) {
        failf(దత్తాంశం, "Operation timed out");
        తిరిగి CURLE_OPERATION_TIMEDOUT;
      }
    }
    లేకపోతే ఒకవేళ(curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &ప్రారంభం) > 1000) {
      /* disconnect timeout */
      failf(దత్తాంశం, "Disconnect timed out");
      ఫలితం = CURLE_OK;
      ఆపు;
    }

    ఒకవేళ(ఖండం) {
      పూర్ణాంకం dir = libssh2_session_ఖండం_directions(sshc->ssh_session);
      curl_socket_t sock = దత్తాంశం->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      ఒకవేళ(LIBSSH2_SESSION_ఖండం_INBOUND & dir)
        fd_read = sock;
      ఒకవేళ(LIBSSH2_SESSION_ఖండం_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (శూన్య)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!ఎడమ_ms || ఎడమ_ms > 1000) ? 1000 : ఎడమ_ms);
    }
  }

  తిరిగి ఫలితం;
}

స్థిర శూన్య myssh_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం SSHPROTO *sshp = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curlx_safefree(sshp->మార్గం);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

స్థిర శూన్య myssh_conn_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం ssh_conn *sshc = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  sshc_cleanup(sshc, ఖాళీ, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
స్థిర CURLcode ssh_అమరిక_connection(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం connectdata *conn)
{
  నిర్మాణం ssh_conn *sshc;
  నిర్మాణం SSHPROTO *sshp;
  (శూన్య)conn;

  sshc = curlx_calloc(1, పరిమాణం(*sshc));
  ఒకవేళ(!sshc)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(Curl_conn_meta_సమితి(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  sshp = curlx_calloc(1, పరిమాణం(*sshp));
  ఒకవేళ(!sshp)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  curlx_dyn_init(&sshp->readdir, CURL_మార్గం_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_మార్గం_MAX);
  ఒకవేళ(Curl_meta_సమితి(దత్తాంశం, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  తిరిగి Curl_ssh_అమరిక_pkey(దత్తాంశం, sshc);
}

స్థిర Curl_recv scp_recv, sftp_recv;
స్థిర Curl_send scp_send, sftp_send;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
స్థిర ssize_t ssh_tls_recv(libssh2_socket_t sock, శూన్య *బఫరు,
                            size_t నిడివి, పూర్ణాంకం flags, శూన్య **abstract)
{
  నిర్మాణం Curl_easy *దత్తాంశం = (నిర్మాణం Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(దత్తాంశం, sock);
  size_t nread;
  CURLcode ఫలితం;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  Curl_recv *backup = conn->recv[sockindex];
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (శూన్య)flags;

  ఒకవేళ(!sshc)
    తిరిగి -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  ఫలితం = Curl_conn_recv(దత్తాంశం, sockindex, బఫరు, నిడివి, &nread);
  conn->recv[sockindex] = backup;
  ఒకవేళ(ఫలితం == CURLE_AGAIN)
    తిరిగి -EAGAIN; /* magic return code for libssh2 */
  లేకపోతే ఒకవేళ(ఫలితం)
    తిరిగి -1; /* generic error */
  Curl_debug(దత్తాంశం, CURLINFO_దత్తాంశం_లోపల, (స్థిరరాశి అక్షరం *)బఫరు, nread);
  తిరిగి (ssize_t)nread;
}

స్థిర ssize_t ssh_tls_send(libssh2_socket_t sock, స్థిరరాశి శూన్య *బఫరు,
                            size_t నిడివి, పూర్ణాంకం flags, శూన్య **abstract)
{
  నిర్మాణం Curl_easy *దత్తాంశం = (నిర్మాణం Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(దత్తాంశం, sock);
  size_t nwrite;
  CURLcode ఫలితం;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  Curl_send *backup = conn->send[sockindex];
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (శూన్య)flags;

  ఒకవేళ(!sshc)
    తిరిగి -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  ఫలితం = Curl_conn_send(దత్తాంశం, sockindex, బఫరు, నిడివి, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  ఒకవేళ(ఫలితం == CURLE_AGAIN)
    తిరిగి -EAGAIN; /* magic return code for libssh2 */
  లేకపోతే ఒకవేళ(ఫలితం)
    తిరిగి -1; /* error */
  Curl_debug(దత్తాంశం, CURLINFO_దత్తాంశం_బహిర్గతం, (స్థిరరాశి అక్షరం *)బఫరు, nwrite);
  తిరిగి (ssize_t)nwrite;
}
#అయితే_ముగింపు

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
స్థిర CURLcode ssh_connect(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
#నిర్వచితమైతే CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#అయితే_ముగింపు
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ఫలితం;

#ఒకవేళ LIBSSH2_సంస్కరణ_అంకె >= 0x010b00
  {
    స్థిరరాశి అక్షరం *crypto_str;
    ఎంపిక(libssh2_crypto_engine()) {
    స్థితి libssh2_gcrypt:
      crypto_str = "libgcrypt";
      ఆపు;
    స్థితి libssh2_mbedtls:
      crypto_str = "mbedTLS";
      ఆపు;
    స్థితి libssh2_openssl:
      crypto_str = "openssl compatible";
      ఆపు;
    స్థితి libssh2_os400qc3:
      crypto_str = "OS400QC3";
      ఆపు;
    స్థితి libssh2_wincng:
      crypto_str = "WinCNG";
      ఆపు;
    అప్రమేయ:
      crypto_str = ఖాళీ;
      ఆపు;
    }
    ఒకవేళ(crypto_str)
      infof(దత్తాంశం, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#అయితే_ముగింపు

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  infof(దత్తాంశం, "SSH: user '%s'", Curl_creds_user(conn->creds));
#నిర్వచితమైతే CURL_LIBSSH2_DEBUG
  infof(దత్తాంశం, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#అయితే_ముగింపు /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, దత్తాంశం);

  ఒకవేళ(!sshc->ssh_session) {
    failf(దత్తాంశం, "Failure initializing ssh session");
    తిరిగి CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#ఒకవేళ LIBSSH2_సంస్కరణ_అంకె >= 0x010B00
  ఒకవేళ(దత్తాంశం->సమితి.server_response_timeout > 0) {
    libssh2_session_సమితి_read_timeout(sshc->ssh_session,
                             (దీర్ఘ)(దత్తాంశం->సమితి.server_response_timeout / 1000));
  }
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  ఒకవేళ(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#ఒకవేళ LIBSSH2_సంస్కరణ_అంకె >= 0x010b01
    infof(దత్తాంశం, "SSH: using HTTPS proxy");
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
#లేకపోతే
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    సంఘం receive {
      శూన్య *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, శూన్య *, size_t, పూర్ణాంకం, శూన్య **);
    };
    సంఘం transfer {
      శూన్య *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, స్థిరరాశి శూన్య *, size_t, పూర్ణాంకం, శూన్య **);
    };
    సంఘం receive sshrecv;
    సంఘం transfer sshsend;

    sshrecv.recvptr = ssh_tls_recv;
    sshsend.sendptr = ssh_tls_send;

    infof(దత్తాంశం, "SSH: using HTTPS proxy");
    libssh2_session_callback_సమితి(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_సమితి(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#అయితే_ముగింపు

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#అయితే_ముగింపు /* CURL_DISABLE_PROXY */
  ఒకవేళ(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  లేకపోతే {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  ఒకవేళ(దత్తాంశం->సమితి.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(దత్తాంశం, "SSH: failed to enable compression for session");
  }

  ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS)) {
    పూర్ణాంకం rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    ఒకవేళ(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = ఖాళీ;
      తిరిగి CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    ఒకవేళ(rc < 0)
      infof(దత్తాంశం, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(దత్తాంశం, STRING_SSH_KNOWNHOSTS));
  }

#నిర్వచితమైతే CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(దత్తాంశం, "SSH: socket %d", (పూర్ణాంకం)sock);
#అయితే_ముగింపు

  myssh_గమ్యం(దత్తాంశం, sshc, SSH_INIT);

  ఫలితం = ssh_multi_statemach(దత్తాంశం, పూర్తి);

  తిరిగి ఫలితం;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

స్థిర CURLcode scp_perform(నిర్మాణం Curl_easy *దత్తాంశం,
                            bool *connected,
                            bool *dophase_పూర్తి)
{
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SSH_CONN);
  CURLcode ఫలితం = CURLE_OK;

  CURL_TRC_SSH(దత్తాంశం, "DO phase starts");

  *dophase_పూర్తి = FALSE; /* not done yet */
  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  ఫలితం = ssh_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  *connected = Curl_conn_is_connected(దత్తాంశం->conn, FIRSTSOCKET);

  ఒకవేళ(*dophase_పూర్తి) {
    CURL_TRC_SSH(దత్తాంశం, "DO phase is complete");
  }

  తిరిగి ఫలితం;
}

/* called from multi.c while DOing */
స్థిర CURLcode scp_doing(నిర్మాణం Curl_easy *దత్తాంశం,
                          bool *dophase_పూర్తి)
{
  CURLcode ఫలితం;
  ఫలితం = ssh_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  ఒకవేళ(*dophase_పూర్తి) {
    CURL_TRC_SSH(దత్తాంశం, "DO phase is complete");
  }
  తిరిగి ఫలితం;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
స్థిర CURLcode scp_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం connectdata *conn,
                               bool dead_connection)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  నిర్మాణం SSHPROTO *sshp = Curl_meta_get(దత్తాంశం, CURL_META_SSH_EASY);
  (శూన్య)dead_connection;

  ఒకవేళ(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SESSION_DISCONNECT);
    ఫలితం = ssh_ఖండం_statemach(దత్తాంశం, sshc, sshp, TRUE);
  }

  ఒకవేళ(sshc)
    తిరిగి sshc_cleanup(sshc, దత్తాంశం, TRUE);
  తిరిగి ఫలితం;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
స్థిర CURLcode ssh_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2)
{
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SSH_CONN);
  నిర్మాణం SSHPROTO *sshp = Curl_meta_get(దత్తాంశం, CURL_META_SSH_EASY);
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!sshc || !sshp)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(!స్థితి_2)
    /* run the state-machine */
    ఫలితం = ssh_ఖండం_statemach(దత్తాంశం, sshc, sshp, FALSE);
  లేకపోతే
    ఫలితం = స్థితి_2;

  ఒకవేళ(Curl_pgrs_పూర్తి(దత్తాంశం, Curl_pgrs_now(దత్తాంశం)))
    తిరిగి CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(దత్తాంశం);
  తిరిగి ఫలితం;
}

స్థిర CURLcode scp_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2,
                         bool premature)
{
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SSH_CONN);
  (శూన్య)premature;

  ఒకవేళ(sshc && !స్థితి_2)
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SCP_పూర్తి);

  తిరిగి ssh_పూర్తి(దత్తాంశం, స్థితి_2);
}

స్థిర CURLcode scp_send(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                         స్థిరరాశి uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ఫలితం = CURLE_OK;
  ssize_t nwritten;

  (శూన్య)sockindex; /* we only support SCP on the fixed known primary socket */
  (శూన్య)eos;
  *pnwritten = 0;

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (స్థిరరాశి అక్షరం *)mem, len);

  ssh_block2waitfor(దత్తాంశం, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  ఒకవేళ(nwritten == LIBSSH2_ERROR_EAGAIN)
    ఫలితం = CURLE_AGAIN;
  లేకపోతే ఒకవేళ(nwritten < LIBSSH2_ERROR_NONE)
    ఫలితం = libssh2_session_error_గమ్యం_CURLE((పూర్ణాంకం)nwritten);
  లేకపోతే
    *pnwritten = (size_t)nwritten;

  తిరిగి ఫలితం;
}

స్థిర CURLcode scp_recv(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                         అక్షరం *mem, size_t len, size_t *pnread)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode ఫలితం = CURLE_OK;
  ssize_t nread;

  (శూన్య)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(దత్తాంశం, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  ఒకవేళ(nread == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;
  లేకపోతే ఒకవేళ(nread < LIBSSH2_ERROR_NONE)
    ఫలితం = libssh2_session_error_గమ్యం_CURLE((పూర్ణాంకం)nread);
  లేకపోతే
    *pnread = (size_t)nread;

  తిరిగి ఫలితం;
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

స్థిర CURLcode sftp_perform(నిర్మాణం Curl_easy *దత్తాంశం,
                             bool *connected,
                             bool *dophase_పూర్తి)
{
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(దత్తాంశం->conn, CURL_META_SSH_CONN);
  CURLcode ఫలితం = CURLE_OK;

  CURL_TRC_SSH(దత్తాంశం, "DO phase starts");

  *dophase_పూర్తి = FALSE; /* not done yet */
  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  ఫలితం = ssh_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  *connected = Curl_conn_is_connected(దత్తాంశం->conn, FIRSTSOCKET);

  ఒకవేళ(*dophase_పూర్తి) {
    CURL_TRC_SSH(దత్తాంశం, "DO phase is complete");
  }

  తిరిగి ఫలితం;
}

/* called from multi.c while DOing */
స్థిర CURLcode sftp_doing(నిర్మాణం Curl_easy *దత్తాంశం,
                           bool *dophase_పూర్తి)
{
  CURLcode ఫలితం = ssh_multi_statemach(దత్తాంశం, dophase_పూర్తి);

  ఒకవేళ(*dophase_పూర్తి) {
    CURL_TRC_SSH(దత్తాంశం, "DO phase is complete");
  }
  తిరిగి ఫలితం;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
స్థిర CURLcode sftp_disconnect(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn, bool dead_connection)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  నిర్మాణం SSHPROTO *sshp = Curl_meta_get(దత్తాంశం, CURL_META_SSH_EASY);
  (శూన్య)dead_connection;

  ఒకవేళ(sshc) {
    ఒకవేళ(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(దత్తాంశం, "DISCONNECT starts now");
      myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_SHUTDOWN);
      ఫలితం = ssh_ఖండం_statemach(దత్తాంశం, sshc, sshp, TRUE);
      CURL_TRC_SSH(దత్తాంశం, "DISCONNECT is done -> %d", (పూర్ణాంకం)ఫలితం);
    }
    sshc_cleanup(sshc, దత్తాంశం, TRUE);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode sftp_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం, CURLcode స్థితి_2,
                          bool premature)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  ఒకవేళ(!స్థితి_2) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    ఒకవేళ(!premature && దత్తాంశం->సమితి.postquote && !conn->బిట్లు.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_గమ్యం(దత్తాంశం, sshc, SSH_SFTP_CLOSE);
  }
  తిరిగి ssh_పూర్తి(దత్తాంశం, స్థితి_2);
}

/* return number of sent bytes */
స్థిర CURLcode sftp_send(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                          స్థిరరాశి uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (శూన్య)sockindex;
  (శూన్య)eos;
  *pnwritten = 0;

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (స్థిరరాశి అక్షరం *)mem, len);

  ssh_block2waitfor(దత్తాంశం, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  ఒకవేళ(nwrite == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;
  లేకపోతే ఒకవేళ(nwrite < LIBSSH2_ERROR_NONE)
    తిరిగి libssh2_session_error_గమ్యం_CURLE((పూర్ణాంకం)nwrite);
  *pnwritten = (size_t)nwrite;
  తిరిగి CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
స్థిర CURLcode sftp_recv(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                          అక్షరం *mem, size_t len, size_t *pnread)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (శూన్య)sockindex;
  *pnread = 0;

  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(దత్తాంశం, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  ఒకవేళ(nread == LIBSSH2_ERROR_EAGAIN)
    తిరిగి CURLE_AGAIN;
  లేకపోతే ఒకవేళ(nread < 0)
    తిరిగి libssh2_session_error_గమ్యం_CURLE((పూర్ణాంకం)nread);

  *pnread = (size_t)nread;
  తిరిగి CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
స్థిర CURLcode ssh_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం;
  bool connected = FALSE;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *పూర్తి = FALSE; /* default to false */
  ఒకవేళ(!sshc)
    తిరిగి CURLE_FAILED_INIT;

  దత్తాంశం->req.కొలత = -1; /* make sure this is unknown at this point */
  sshc->రెండవ_Create_Dirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(దత్తాంశం);

  ఒకవేళ(conn->scheme->protocol & CURLPROTO_SCP)
    ఫలితం = scp_perform(దత్తాంశం, &connected, పూర్తి);
  లేకపోతే
    ఫలితం = sftp_perform(దత్తాంశం, &connected, పూర్తి);

  తిరిగి ఫలితం;
}

CURLcode Curl_ssh_init(శూన్య)
{
  ఒకవేళ(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    తిరిగి CURLE_FAILED_INIT;
  }
  తిరిగి CURLE_OK;
}

శూన్య Curl_ssh_cleanup(శూన్య)
{
  libssh2_exit();
}

శూన్య Curl_ssh_సంస్కరణ(అక్షరం *బఫరు, size_t buflen)
{
  (శూన్య)curl_msnprintf(బఫరు, buflen, "libssh2/%s", libssh2_సంస్కరణ(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
స్థిర శూన్య ssh_attach(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం connectdata *conn)
{
  DEBUGASSERT(దత్తాంశం);
  DEBUGASSERT(conn);
  ఒకవేళ(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    ఒకవేళ(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      శూన్య **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = దత్తాంశం;
    }
  }
}

/*
 * SCP protocol handler.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_scp = {
  ssh_అమరిక_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  scp_పూర్తి,                             /* done */
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
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_sftp = {
  ssh_అమరిక_connection,                 /* setup_connection */
  ssh_do,                               /* do_it */
  sftp_పూర్తి,                            /* done */
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

#అయితే_ముగింపు /* USE_LIBSSH2 */
