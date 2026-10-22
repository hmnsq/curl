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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef __VMS
#ଅନ୍ତର୍ଭାଵ <in.h>
#ଅନ୍ତର୍ଭାଵ <inet.h>
#endif

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "vssh/ssh.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "parsedate.h" /* for the week day and month names */
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"
#ଅନ୍ତର୍ଭାଵ "vssh/vssh.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h" /* for curlx_base64_encode() */

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *sftp_libssh2_strerror(unsigned long err)
{
  ବାଛ(err) {
  ସ୍ଥିତି LIBSSH2_FX_NO_SUCH_FILE:
    ଫେରନ୍ତୁ "No such file or directory";

  ସ୍ଥିତି LIBSSH2_FX_PERMISSION_DENIED:
    ଫେରନ୍ତୁ "Permission denied";

  ସ୍ଥିତି LIBSSH2_FX_FAILURE:
    ଫେରନ୍ତୁ "Operation failed";

  ସ୍ଥିତି LIBSSH2_FX_BAD_MESSAGE:
    ଫେରନ୍ତୁ "Bad message from SFTP server";

  ସ୍ଥିତି LIBSSH2_FX_NO_CONNECTION:
    ଫେରନ୍ତୁ "Not connected to SFTP server";

  ସ୍ଥିତି LIBSSH2_FX_CONNECTION_LOST:
    ଫେରନ୍ତୁ "Connection to SFTP server lost";

  ସ୍ଥିତି LIBSSH2_FX_OP_UNSUPPORTED:
    ଫେରନ୍ତୁ "Operation not supported by SFTP server";

  ସ୍ଥିତି LIBSSH2_FX_INVALID_HANDLE:
    ଫେରନ୍ତୁ "Invalid handle";

  ସ୍ଥିତି LIBSSH2_FX_NO_SUCH_PATH:
    ଫେରନ୍ତୁ "No such file or directory";

  ସ୍ଥିତି LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ଫେରନ୍ତୁ "File already exists";

  ସ୍ଥିତି LIBSSH2_FX_WRITE_PROTECT:
    ଫେରନ୍ତୁ "File is write protected";

  ସ୍ଥିତି LIBSSH2_FX_NO_MEDIA:
    ଫେରନ୍ତୁ "No media";

  ସ୍ଥିତି LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    ଫେରନ୍ତୁ "Disk full";

  ସ୍ଥିତି LIBSSH2_FX_QUOTA_EXCEEDED:
    ଫେରନ୍ତୁ "User quota exceeded";

  ସ୍ଥିତି LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    ଫେରନ୍ତୁ "Unknown principal";

  ସ୍ଥିତି LIBSSH2_FX_LOCK_CONFLICT:
    ଫେରନ୍ତୁ "File lock conflict";

  ସ୍ଥିତି LIBSSH2_FX_DIR_NOT_EMPTY:
    ଫେରନ୍ତୁ "Directory not empty";

  ସ୍ଥିତି LIBSSH2_FX_NOT_A_DIRECTORY:
    ଫେରନ୍ତୁ "Not a directory";

  ସ୍ଥିତି LIBSSH2_FX_INVALID_FILENAME:
    ଫେରନ୍ତୁ "Invalid filename";

  ସ୍ଥିତି LIBSSH2_FX_LINK_LOOP:
    ଫେରନ୍ତୁ "Link points to itself";
  }
  ଫେରନ୍ତୁ "Unknown error in libssh2";
}

ସ୍ଥିର ଶୂନ୍ୟ kbd_callback(ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା name_len,
                         ଅଚଳ ଅକ୍ଷର *instruction, ପୂର୍ଣ୍ଣସଂଖ୍ୟା instruction_len,
                         ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_prompts,
                         ଅଚଳ LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         ଶୂନ୍ୟ **abstract)
{
  ସଂରଚନା Curl_easy *data = (ସଂରଚନା Curl_easy *)*abstract;

#ifdef CURL_LIBSSH2_DEBUG
  curl_mfprintf(stderr, "name=%s\n", name);
  curl_mfprintf(stderr, "name_len=%d\n", name_len);
  curl_mfprintf(stderr, "instruction=%s\n", instruction);
  curl_mfprintf(stderr, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(stderr, "num_prompts=%d\n", ଅଙ୍କ_prompts);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)name;
  (ଶୂନ୍ୟ)name_len;
  (ଶୂନ୍ୟ)instruction;
  (ଶୂନ୍ୟ)instruction_len;
#endif /* CURL_LIBSSH2_DEBUG */
  ଯଦି(ଅଙ୍କ_prompts == 1) {
    ସଂରଚନା connectdata *conn = data->conn;
    ଅଚଳ ଅକ୍ଷର *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].length = responses[0].text ? curlx_uztoui(strlen(passwd)) : 0;
  }
  (ଶୂନ୍ୟ)prompts;
} /* kbd_callback */

ସ୍ଥିର CURLcode sftp_libssh2_error_to_CURLE(unsigned long err)
{
  ବାଛ(err) {
  ସ୍ଥିତି LIBSSH2_FX_OK:
    ଫେରନ୍ତୁ CURLE_OK;

  ସ୍ଥିତି LIBSSH2_FX_NO_SUCH_FILE:
  ସ୍ଥିତି LIBSSH2_FX_NO_SUCH_PATH:
    ଫେରନ୍ତୁ CURLE_REMOTE_FILE_NOT_FOUND;

  ସ୍ଥିତି LIBSSH2_FX_PERMISSION_DENIED:
  ସ୍ଥିତି LIBSSH2_FX_WRITE_PROTECT:
  ସ୍ଥିତି LIBSSH2_FX_LOCK_CONFLICT:
    ଫେରନ୍ତୁ CURLE_REMOTE_ACCESS_DENIED;

  ସ୍ଥିତି LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  ସ୍ଥିତି LIBSSH2_FX_QUOTA_EXCEEDED:
    ଫେରନ୍ତୁ CURLE_REMOTE_DISK_FULL;

  ସ୍ଥିତି LIBSSH2_FX_FILE_ALREADY_EXISTS:
    ଫେରନ୍ତୁ CURLE_REMOTE_FILE_EXISTS;

  ସ୍ଥିତି LIBSSH2_FX_DIR_NOT_EMPTY:
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ CURLE_SSH;
}

ସ୍ଥିର CURLcode libssh2_session_error_to_CURLE(ପୂର୍ଣ୍ଣସଂଖ୍ୟା err)
{
  ବାଛ(err) {
  /* Ordered by order of appearance in libssh2.h */
  ସ୍ଥିତି LIBSSH2_ERROR_NONE:
    ଫେରନ୍ତୁ CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  ସ୍ଥିତି LIBSSH2_ERROR_SCP_PROTOCOL:
    ଫେରନ୍ତୁ CURLE_REMOTE_FILE_NOT_FOUND;

  ସ୍ଥିତି LIBSSH2_ERROR_SOCKET_NONE:
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;

  ସ୍ଥିତି LIBSSH2_ERROR_ALLOC:
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ସ୍ଥିତି LIBSSH2_ERROR_SOCKET_SEND:
    ଫେରନ୍ତୁ CURLE_SEND_ERROR;

  ସ୍ଥିତି LIBSSH2_ERROR_HOSTKEY_INIT:
  ସ୍ଥିତି LIBSSH2_ERROR_HOSTKEY_SIGN:
  ସ୍ଥିତି LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  ସ୍ଥିତି LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;

  ସ୍ଥିତି LIBSSH2_ERROR_PASSWORD_EXPIRED:
    ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;

  ସ୍ଥିତି LIBSSH2_ERROR_SOCKET_TIMEOUT:
  ସ୍ଥିତି LIBSSH2_ERROR_TIMEOUT:
    ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;

  ସ୍ଥିତି LIBSSH2_ERROR_EAGAIN:
    ଫେରନ୍ତୁ CURLE_AGAIN;
  }

  ଫେରନ୍ତୁ CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

ସ୍ଥିର LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (ଶୂନ୍ୟ)abstract;
  ଫେରନ୍ତୁ Curl_cmalloc(ଗଣନା_2);
}

ସ୍ଥିର LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (ଶୂନ୍ୟ)abstract;
  ଫେରନ୍ତୁ Curl_crealloc(ptr, ଗଣନା_2);
}

ସ୍ଥିର LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (ଶୂନ୍ୟ)abstract;
  ଯଦି(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sshkeycallback(CURL *easy,
                          ଅଚଳ ସଂରଚନା curl_khkey *knownkey, /* known */
                          ଅଚଳ ସଂରଚନା curl_khkey *foundkey, /* found */
                          ଗଣନା curl_khmatch match,
                          ଶୂନ୍ୟ *clientp)
{
  (ଶୂନ୍ୟ)easy;
  (ଶୂନ୍ୟ)knownkey;
  (ଶୂନ୍ୟ)foundkey;
  (ଶୂନ୍ୟ)clientp;

  /* we only allow perfect matches, and we reject everything else */
  ଫେରନ୍ତୁ (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

ସ୍ଥିର ଗଣନା curl_khtype convert_ssh2_keytype(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sshkeytype)
{
  ଗଣନା curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  ବାଛ(sshkeytype) {
  ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    ଭାଙ୍ଗ;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
  ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    ଭାଙ୍ଗ;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    ଭାଙ୍ଗ;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    ଭାଙ୍ଗ;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    ଭାଙ୍ଗ;
#endif
#ifdef LIBSSH2_HOSTKEY_TYPE_ED25519
  ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    ଭାଙ୍ଗ;
#endif
  }
  ଫେରନ୍ତୁ keytype;
}

ସ୍ଥିର CURLcode ssh_knownhost(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ssh_conn *sshc)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା libssh2_knownhost *host = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *remotekey = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା keybit = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା sshkeytype = 0;
  size_t keylen = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  CURLcode result = CURLE_OK;

  ଯଦି(!CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "SSH: no knownhosts file configured");
    ଫେରନ୍ତୁ CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  ଯଦି(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    ସଂରଚନା Curl_mapi_guard guard;
    ଗଣନା curl_khmatch keymatch;
    curl_sshkeycallback func =
      data->set.ssh_keyfunc ? data->set.ssh_keyfunc : sshkeycallback;
    ସଂରଚନା curl_khkey knownkey;
    ସଂରଚନା curl_khkey *knownkeyp = ଖାଲି;
    ସଂରଚନା curl_khkey foundkey;

    ବାଛ(sshkeytype) {
    ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_ଚାବି_SSHRSA;
      ଭାଙ୍ଗ;
#ifdef LIBSSH2_HOSTKEY_TYPE_DSS
    ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_ଚାବି_SSHDSS;
      ଭାଙ୍ଗ;
#endif
    ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_ଚାବି_ECDSA_256;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_ଚାବି_ECDSA_384;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_ଚାବି_ECDSA_521;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_ଚାବି_ED25519;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      infof(data, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      ଭାଙ୍ଗ;
    }
    ଯଦି(!keybit)
      /* no check means failure! */
      rc = CURLKHSTAT_REJECT;
    ଅନ୍ୟଥା {
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
            host->ଚାବି : "<none>");

      /* setup 'knownkey' */
      ଯଦି(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
        knownkey.ଚାବି = host->ଚାବି;
        knownkey.len = 0;
        knownkey.keytype = convert_ssh2_keytype(sshkeytype);
        knownkeyp = &knownkey;
      }

      /* setup 'foundkey' */
      foundkey.ଚାବି = remotekey;
      foundkey.len = keylen;
      foundkey.keytype = convert_ssh2_keytype(sshkeytype);

      /*
       * if any of the LIBSSH2_KNOWNHOST_CHECK_* defines and the
       * curl_khmatch enum are ever modified, we need to introduce a
       * translation table here!
       */
      keymatch = (ଗଣନା curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  ଅନ୍ୟଥା {
    /* no remotekey means failure! */
    infof(data, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  ବାଛ(rc) {
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* unknown return codes is the same as reject */
  ସ୍ଥିତି CURLKHSTAT_REJECT:
    infof(data, "SSH: knownhost check failed");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  ସ୍ଥିତି CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    result = CURLE_PEER_FAILED_VERIFICATION;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    ଯଦି(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  ସ୍ଥିତି CURLKHSTAT_FINE:
  ସ୍ଥିତି CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    ଯଦି(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା addrc;
      ଅଚଳ ଅକ୍ଷର *hostbuf;
      ଅକ୍ଷର *hostport = ଖାଲି;
      ଯଦି(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        ଯଦି(!hostbuf)
          infof(data, "WARNING: failed allocating buffer for [host]:port");
      }
      ଅନ୍ୟଥା
        hostbuf = conn->origin->hostname;
      ଯଦି(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, ଖାଲି,
                                       remotekey, keylen, ଖାଲି, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, ଖାଲି);
        ଯଦି(addrc)
          infof(data, "WARNING: adding the known host %s failed", hostbuf);
        ଅନ୍ୟଥା ଯଦି(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          ପୂର୍ଣ୍ଣସଂଖ୍ୟା wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          ଯଦି(wrc) {
            infof(data, "WARNING: writing %s failed",
                  CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    ଅନ୍ୟଥା
      infof(data, "SSH: knownhost entry matches host key");
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_check_fingerprint(ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା ssh_conn *sshc)
{
  ଅଚଳ ଅକ୍ଷର *pubkey_md5 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ଚାବି_MD5);
  ଅଚଳ ଅକ୍ଷର *pubkey_sha256 =
    CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ଚାବି_SHA256);

  ଯଦି(pubkey_sha256) {
    ଅଚଳ ଅକ୍ଷର *fingerprint = ଖାଲି;
    ଅକ୍ଷର *fingerprint_b64 = ଖାଲି;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    infof(data, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    ଯଦି(!fingerprint) {
      failf(data,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    ଯଦି(curlx_base64_encode((ଅଚଳ uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
    }

    ଯଦି(!fingerprint_b64) {
      failf(data, "SHA256 fingerprint could not be encoded");
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(data, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    ଯେତେବେଳେ((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    ଯେତେବେଳେ((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    ଯଦି((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(data, "SSH: SHA256 checksum match");
  }

  ଯଦି(pubkey_md5) {
    ଅକ୍ଷର md5buffer[33];
    ଅଚଳ ଅକ୍ଷର *fingerprint;

    infof(data, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    ଯଦି(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
      ପାଇଁ(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (unsigned ଅକ୍ଷର)fingerprint[i]);
      }

      infof(data, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    ଯଦି(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      ଯଦି(fingerprint) {
        failf(data,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      ଅନ୍ୟଥା {
        failf(data,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(data, sshc, SSH_SESSION_FREE);
      ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(data, "SSH: MD5 checksum match");
  }

  ଯଦି(!pubkey_md5 && !pubkey_sha256) {
    ଯଦି(data->set.ssh_hostkeyfunc) {
      size_t keylen = 0;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sshkeytype = 0;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
      /* we handle the process to the callback */
      ଅଚଳ ଅକ୍ଷର *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      ଯଦି(remotekey) {
        ସଂରଚନା Curl_mapi_guard guard;
        ଗଣନା curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, data, easy_ssh_hostkeyfunc);
        rc = data->set.ssh_hostkeyfunc(data->set.ssh_hostkeyfunc_userp,
                                       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        ଯଦି(rc != CURLKHMATCH_OK) {
          myssh_to(data, sshc, SSH_SESSION_FREE);
          failf(data, "SSH: callback failed host public key verification");
          ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(data, "SSH: verified public key via callback");
      }
      ଅନ୍ୟଥା {
        myssh_to(data, sshc, SSH_SESSION_FREE);
        ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
      }
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଅନ୍ୟଥା {
      CURL_TRC_SSH(data, "no host key checksum given, checking knownhosts");
      ଫେରନ୍ତୁ ssh_knownhost(data, sshc);
    }
  }
  ଅନ୍ୟଥା {
    /* as we already matched, we skip the check for known hosts */
    ଫେରନ୍ତୁ CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
ସ୍ଥିର CURLcode ssh_force_knownhost_ଚାବି_type(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା ssh_conn *sshc)
{
  CURLcode result = CURLE_OK;

  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#ifdef LIBSSH2_KNOWNHOST_ଚାବି_SSHDSS
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର hostkey_method_ssh_dss[] = "ssh-dss";
#endif
  bool found = FALSE;

  ଯଦି(sshc->kh &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ଚାବି_MD5) &&
     !CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_ଚାବି_SHA256)) {
    ସଂରଚନା libssh2_knownhost *store = ଖାଲି;
    ସଂରଚନା connectdata *conn = data->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_ଚାବି_MASK >>
                      LIBSSH2_KNOWNHOST_ଚାବି_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    ଯେତେବେଳେ(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      ଯଦି(store) {
        ଯଦି(store->name) {
          ଯଦି(store->name[0] == '[') {
            curl_off_t port;
            size_t kh_name_size = 0;
            ଅଚଳ ଅକ୍ଷର *p;
            ଅଚଳ ଅକ୍ଷର *kh_name_end = strstr(store->name, "]:");
            ଯଦି(!kh_name_end) {
              infof(data, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
              ଜାରି;
            }
            p = kh_name_end + 2; /* start of port number */
            ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_size = strlen(store->name) - 1 - strlen(kh_name_end);
              ଯଦି((kh_name_size == strlen(conn->origin->hostname)) &&
                 !strncmp(store->name + 1, conn->origin->hostname,
                          kh_name_size)) {
                found = TRUE;
                ଭାଙ୍ଗ;
              }
            }
          }
          ଅନ୍ୟଥା ଯଦି(!strcmp(store->name, conn->origin->hostname)) {
            found = TRUE;
            ଭାଙ୍ଗ;
          }
        }
        ଅନ୍ୟଥା ଯଦି(store->ଚାବି) {
          unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = (store->typemask & LIBSSH2_KNOWNHOST_ଚାବି_MASK)
                              >> LIBSSH2_KNOWNHOST_ଚାବି_SHIFT;
          ଯଦି(!absent_type[type]) {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->ଚାବି, strlen(store->ଚାବି),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_ଚାବି_MASK),
              ଖାଲି);
            ଯଦି(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              ଭାଙ୍ଗ;
            }
            ଅନ୍ୟଥା ଯଦି(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    ଯଦି(found) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
      ଅଚଳ ଅକ୍ଷର *hostkey_method = ଖାଲି;
      infof(data, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));

      ବାଛ(store->typemask & LIBSSH2_KNOWNHOST_ଚାବି_MASK) {
      ସ୍ଥିତି LIBSSH2_KNOWNHOST_ଚାବି_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି LIBSSH2_KNOWNHOST_ଚାବି_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି LIBSSH2_KNOWNHOST_ଚାବି_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି LIBSSH2_KNOWNHOST_ଚାବି_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି LIBSSH2_KNOWNHOST_ଚାବି_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        ଭାଙ୍ଗ;
#ifdef LIBSSH2_KNOWNHOST_ଚାବି_SSHDSS
      ସ୍ଥିତି LIBSSH2_KNOWNHOST_ଚାବି_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        ଭାଙ୍ଗ;
#endif
      ସ୍ଥିତି LIBSSH2_KNOWNHOST_ଚାବି_RSA1:
        failf(data, "Found host key type RSA1 which is not supported");
        ଫେରନ୍ତୁ CURLE_SSH;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        failf(data, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_ଚାବି_MASK));
        ଫେରନ୍ତୁ CURLE_SSH;
      }

      infof(data, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      ଯଦି(rc) {
        ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, &errlen, 0);
        failf(data, "libssh2 method '%s' failed: %s", hostkey_method, err_ସନ୍ଦେଶ);
        result = libssh2_session_error_to_CURLE(rc);
      }
    }
    ଅନ୍ୟଥା {
      infof(data, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    }
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode quote_error(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
}

ସ୍ଥିର CURLcode sftp_quote(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା ssh_conn *sshc,
                           ସଂରଚନା SSHPROTO *sshp)
{
  ଅଚଳ ଅକ୍ଷର *cp;
  CURLcode result = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  ଅଚଳ ଅକ୍ଷର *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  ଯଦି(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ଯଦି(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    ଅକ୍ଷର *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    ଯଦି(!tmp)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    ଯଦି(!result)
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    ଫେରନ୍ତୁ result;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  ଯଦି(!cp) {
    failf(data, "Syntax error command '%s', missing parameter", cmd);
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  ଯଦି(result) {
    ଯଦି(result != CURLE_OUT_OF_MEMORY)
      failf(data, "Syntax error: Bad first parameter to '%s'", cmd);
    ଫେରନ୍ତୁ result;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  ଯଦି(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ଯଦି(result) {
      ଯଦି(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      ଫେରନ୍ତୁ result;
    }
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);

    memset(&sshp->quote_attrs, 0, ଆକାର(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    ଫେରନ୍ତୁ result;
  }
  ଯଦି(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ଯଦି(result) {
      ଯଦି(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ଫେରନ୍ତୁ result;
    }
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "mkdir ", 6)) {
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    ଯଦି(result) {
      ଯଦି(result != CURLE_OUT_OF_MEMORY)
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      ଫେରନ୍ତୁ result;
    }
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "rmdir ", 6)) {
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    /* delete directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "rm ", 3)) {
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    ଫେରନ୍ତୁ result;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "statvfs ", 8)) {
    ଯଦି(*cp)
      ଫେରନ୍ତୁ quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    ଫେରନ୍ତୁ result;
  }

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
}

ସ୍ଥିର CURLcode sftp_upload_init(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ssh_conn *sshc,
                                 ସଂରଚନା SSHPROTO *sshp,
                                 bool *blockp)
{
  unsigned long flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  ଯଦି(data->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    ଯଦି(data->state.resume_from < 0) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(strlen(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      ଯଦି(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଯଦି(rc) {
        data->state.resume_from = 0;
      }
      ଅନ୍ୟଥା {
        curl_off_t size = attrs.filesize;
        ଯଦି(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          ଫେରନ୍ତୁ CURLE_BAD_DOWNLOAD_RESUME;
        }
        data->state.resume_from = attrs.filesize;
      }
    }
  }

  ଯଦି(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  ଅନ୍ୟଥା ଯଦି(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use APPEND; some servers force writes to EOF when
     * APPEND is set, ignoring a prior seek().
     */
    flags = LIBSSH2_FXF_WRITE;
  }
  ଅନ୍ୟଥା {
    /* Clear file before writing (normal behavior) */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC;
  }

  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         flags, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  ଯଦି(!sshc->sftp_handle) {
    CURLcode result;
    unsigned long sftperr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_session_last_errno(sshc->ssh_session);

    ଯଦି(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }

    ଯଦି(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    ଅନ୍ୟଥା
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    ଯଦି(sshc->secondCreateDirs) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      failf(data, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      ଫେରନ୍ତୁ sftp_libssh2_error_to_CURLE(sftperr);
    }
    ଯଦି((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    result = sftp_libssh2_error_to_CURLE(sftperr);
    ଯଦି(!result) {
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
    ଫେରନ୍ତୁ result;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  ଯଦି(data->state.resume_from > 0 && !data->set.remote_append) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    ଯଦି(data->set.seek_func) {
      ସଂରଚନା Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    ଯଦି(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      ଯଦି(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        ଫେରନ୍ତୁ CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      କର {
        ସଂରଚନା Curl_mapi_guard guard;
        ଅକ୍ଷର scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)ଆକାର(scratch)) ?
          ଆକାର(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread;
        CURL_CBAPI_START(&guard, data, easy_fread_func);
        actuallyread = data->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              data->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        ଯଦି((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          ଫେରନ୍ତୁ CURLE_FTP_COULDNT_USE_REST;
        }
      } ଯେତେବେଳେ(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    ଯଦି(data->state.infilesize > 0) {
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)data->state.resume_from);
  }
  ଯଦି(data->state.infilesize > 0) {
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
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ ssh_state_pkey_init(ସଂରଚନା Curl_easy *data, ସଂରଚନା ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  ଯଦି((data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     strstr(sshc->authlist, "publickey")) {
    ଯଦି(sshc->pub_ଚାବି)
      infof(data, "SSH: trying public key file '%s'", sshc->pub_ଚାବି);
    infof(data, "SSH: trying private key file '%s'", sshc->priv_ଚାବି);
    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
}

ସ୍ଥିର CURLcode sftp_quote_stat(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା ssh_conn *sshc,
                                ସଂରଚନା SSHPROTO *sshp,
                                bool *blockp)
{
  ଅଚଳ ଅକ୍ଷର *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  ଯଦି(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  ଯଦି(strncmp(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(strlen(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଯଦି(rc && !sshc->acceptfail) { /* get those attributes */
      unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(data, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      ଯାଅ fail;
    }
  }

  /* Now set the new attributes... */
  ଯଦି(!strncmp(cmd, "chgrp", 5)) {
    ଅଚଳ ଅକ୍ଷର *p = sshc->quote_path1;
    curl_off_t gid;
    ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &gid, ULONG_MAX)) {
      sshp->quote_attrs.gid = (unsigned long)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ଅନ୍ୟଥା ଯଦି(!sshc->acceptfail) {
      failf(data, "Syntax error: chgrp gid not a number");
      ଯାଅ fail;
    }
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    ଅଚଳ ଅକ୍ଷର *p = sshc->quote_path1;
    /* permissions are octal */
    ଯଦି(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      ଯାଅ fail;
    }

    sshp->quote_attrs.permissions = (unsigned long)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "chown", 5)) {
    ଅଚଳ ଅକ୍ଷର *p = sshc->quote_path1;
    curl_off_t uid;
    ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &uid, ULONG_MAX)) {
      sshp->quote_attrs.uid = (unsigned long)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    ଅନ୍ୟଥା ଯଦି(!sshc->acceptfail) {
      failf(data, "Syntax error: chown uid not a number");
      ଯାଅ fail;
    }
  }
  ଅନ୍ୟଥା ଯଦି(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;

    ଯଦି(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#ଯଦି SIZEOF_TIME_T > SIZEOF_LONG
    ଯଦି(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(data, "date overflow");
      fail = TRUE;
    }
#endif
    ଯଦି(fail)
      ଯାଅ fail;
    ଯଦି(!strncmp(cmd, "atime", 5))
      sshp->quote_attrs.atime = (unsigned long)date;
    ଅନ୍ୟଥା /* mtime */
      sshp->quote_attrs.mtime = (unsigned long)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  ଫେରନ୍ତୁ CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
}

ସ୍ଥିର CURLcode sftp_download_stat(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ssh_conn *sshc,
                                   ସଂରଚନା SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(strlen(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଯଦି(rc ||
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
  ଅନ୍ୟଥା {
    curl_off_t size = attrs.filesize;

    ଯଦି(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      ଫେରନ୍ତୁ CURLE_BAD_DOWNLOAD_RESUME;
    }
    ଯଦି(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  ଯଦି(data->state.resume_from) {
    ଯଦି(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      ଯଦି((curl_off_t)attrs.filesize < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        ଫେରନ୍ତୁ CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      data->state.resume_from += attrs.filesize;
    }
    ଅନ୍ୟଥା {
      ଯଦି((curl_off_t)attrs.filesize < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, (curl_off_t)attrs.filesize);
        ଫେରନ୍ତୁ CURLE_BAD_DOWNLOAD_RESUME;
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
  ଯଦି(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "SSH: file already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode sftp_readdir(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ssh_conn *sshc,
                             ସଂରଚନା SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    ଫେରନ୍ତୁ result;
  }
  ଯଦି(rc > 0) {
    size_t readdir_len = (size_t)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    ଯଦି(data->set.list_only) {
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      ଯଦି(!result)
        result = Curl_client_write(data, CLIENTWRITE_BODY, "\n", 1);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଅନ୍ୟଥା {
      result = curlx_dyn_add(&sshp->readdir, sshp->readdir_longentry);

      ଯଦି(!result) {
        ଯଦି((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          result = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        }
        ଅନ୍ୟଥା {
          myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      ଫେରନ୍ତୁ result;
    }
  }
  ଅନ୍ୟଥା ଯଦି(!rc) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  ଅନ୍ୟଥା {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    result = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(data, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_init(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ssh_conn *sshc)
{
  CURLcode result;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  result = ssh_force_knownhost_ଚାବି_type(data, sshc);
  ଯଦି(result)
    myssh_to(data, sshc, SSH_SESSION_FREE);
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_S_STARTUP);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_startup(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ssh_conn *sshc)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc) {
    ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
    (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
    failf(data, "Failure establishing ssh session: %d, %s", rc, err_ସନ୍ଦେଶ);

    myssh_to(data, sshc, SSH_SESSION_FREE);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  myssh_to(data, sshc, SSH_HOSTKEY);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_hostkey(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode result = ssh_check_fingerprint(data, sshc);
  ଯଦି(!result)
    myssh_to(data, sshc, SSH_AUTHLIST);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_authlist(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ssh_conn *sshc)
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
  ସଂରଚନା connectdata *conn = data->conn;
  ଅଚଳ ଅକ୍ଷର *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(strlen(user)));

  ଯଦି(!sshc->authlist) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
    ଯଦି(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(data, "SSH: user accepted with no authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    myssh_to(data, sshc, SSH_SESSION_FREE);
    ଫେରନ୍ତୁ libssh2_session_error_to_CURLE(rc);
  }
  infof(data, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_auth_pkey(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  ସଂରଚନା connectdata *conn = data->conn;
  ଅଚଳ ଅକ୍ଷର *user = Curl_creds_user(conn->creds);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(strlen(user)),
    sshc->pub_ଚାବି, sshc->priv_ଚାବି, sshc->passphrase ? sshc->passphrase : "");
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: authenticated via publickey");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ଅନ୍ୟଥା {
    ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
    ଅକ୍ଷର unknown[] = "Reason unknown (-1)";
    ଯଦି(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_ସନ୍ଦେଶ = unknown;
    }
    ଅନ୍ୟଥା {
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
    }
    infof(data, "SSH: publickey authentication denied: %s", err_ସନ୍ଦେଶ);
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_auth_pass_init(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି((data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     strstr(sshc->authlist, "password")) {
    myssh_to(data, sshc, SSH_AUTH_PASS);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_auth_pass(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ଅଚଳ ଅକ୍ଷର *user = Curl_creds_user(conn->creds);
  ଅଚଳ ଅକ୍ଷର *passwd = Curl_creds_passwd(conn->creds);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(strlen(user)),
                                 passwd,
                                 curlx_uztoui(strlen(passwd)),
                                 ଖାଲି);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN) {
    ଫେରନ୍ତୁ CURLE_AGAIN;
  }
  ଯଦି(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_AUTH_HOST_INIT);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_auth_host_init(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି((data->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     strstr(sshc->authlist, "hostbased")) {
    myssh_to(data, sshc, SSH_AUTH_HOST);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_auth_agent_init(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  ଯଦି((data->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     strstr(sshc->authlist, "publickey")) {

    infof(data, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    ଯଦି(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      ଯଦି(!sshc->ssh_agent) {
        infof(data, "SSH: could not create agent object");

        myssh_to(data, sshc, SSH_AUTH_ଚାବି_INIT);
        ଫେରନ୍ତୁ CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;
    ଯଦି(rc < 0) {
      infof(data, "SSH: failure connecting to agent");
      myssh_to(data, sshc, SSH_AUTH_ଚାବି_INIT);
    }
    ଅନ୍ୟଥା {
      myssh_to(data, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_AUTH_ଚାବି_INIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_auth_agent_list(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_agent_list_identities(sshc->ssh_agent);

  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;
  ଯଦି(rc < 0) {
    infof(data, "SSH: failure requesting identities to agent");
    myssh_to(data, sshc, SSH_AUTH_ଚାବି_INIT);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_prev_identity = ଖାଲି;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_auth_agent(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_prev_identity);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc == 0) {
    CURL_TRC_SSH(data, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(data->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(data->conn->creds),
                                sshc->sshagent_identity);

    ଯଦି(rc < 0) {
      ଯଦି(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_prev_identity = sshc->sshagent_identity;
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
  }

  ଯଦି(rc < 0)
    infof(data, "SSH: failure requesting identities to agent");
  ଅନ୍ୟଥା ଯଦି(rc == 1)
    infof(data, "SSH: no agent identity would match");

  ଯଦି(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(data, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(data->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_AUTH_ଚାବି_INIT);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_auth_ଚାବି_init(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି((data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     strstr(sshc->authlist, "keyboard-interactive")) {
    myssh_to(data, sshc, SSH_AUTH_ଚାବି);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_AUTH_DONE);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_auth_ଚାବି(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  ସଂରଚନା connectdata *conn = data->conn;
  ଅଚଳ ଅକ୍ଷର *user = Curl_creds_user(conn->creds);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(strlen(user)),
                                             &kbd_callback);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc == 0) {
    sshc->authed = TRUE;
    infof(data, "SSH: initialized keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;
}

ସ୍ଥିର CURLcode ssh_state_auth_done(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ଯଦି(!sshc->authed) {
    failf(data, "Authentication failure");
    myssh_to(data, sshc, SSH_SESSION_FREE);
    ଫେରନ୍ତୁ CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(data, "SSH: authentication complete");

  Curl_pgrsTime(data, TIMER_APPCONNECT); /* SSH is connected */

  data->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  ଯଦି(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  infof(data, "SSH: connection established");
  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_init(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  ଯଦି(!sshc->sftp_session) {
    ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
    ଯଦି(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
    failf(data, "Failure initializing sftp session: %s", err_ସନ୍ଦେଶ);
    myssh_to(data, sshc, SSH_SESSION_FREE);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_realpath(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ssh_conn *sshc,
                                        ସଂରଚନା SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  myssh_to(data, sshc, SSH_STOP);
  ଯଦି(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    ଯଦି(!sshc->homedir)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    curlx_free(data->state.most_recent_ftp_entrypath);
    data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    ଯଦି(!data->state.most_recent_ftp_entrypath)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  ଅନ୍ୟଥା {
    /* Return the error type */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode result;
    ଯଦି(sftperr)
      result = sftp_libssh2_error_to_CURLE(sftperr);
    ଅନ୍ୟଥା
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      result = CURLE_SSH;
    CURL_TRC_SSH(data, "error = %lu makes libcurl = %d", sftperr, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଫେରନ୍ତୁ result;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote_init(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା ssh_conn *sshc,
                                          ସଂରଚନା SSHPROTO *sshp)
{
  CURLcode result;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  ଯଦି(result) {
    myssh_to(data, sshc, SSH_STOP);
    ଫେରନ୍ତୁ result;
  }

  ଯଦି(data->set.quote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_postquote_init(ସଂରଚନା Curl_easy *data,
                                              ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(data->set.postquote) {
    infof(data, "SSH: sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ଅନ୍ୟଥା {
    myssh_to(data, sshc, SSH_STOP);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ssh_conn *sshc,
                                     ସଂରଚନା SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode result;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = sftp_quote(data, sshc, sshp);
  ଯଦି(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_sftp_next_quote(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  ଯଦି(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  ଅନ୍ୟଥା {
    ଯଦି(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    ଅନ୍ୟଥା {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote_stat(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା ssh_conn *sshc,
                                          ସଂରଚନା SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode result;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = sftp_quote_stat(data, sshc, sshp, blockp);
  ଯଦି(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote_setstat(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା ssh_conn *sshc,
                                             ସଂରଚନା SSHPROTO *sshp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote_symlink(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(strlen(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(strlen(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote_mkdir(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)),
                                 (long)data->set.new_directory_perms);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote_rename(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(strlen(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(strlen(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote_rmdir(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(strlen(sshc->quote_path1)));
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote_unlink(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା ssh_conn *sshc)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(strlen(sshc->quote_path1)));
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_quote_statvfs(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(strlen(sshc->quote_path1)),
                                &statvfs);

  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc && !sshc->acceptfail) {
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
  }
  ଅନ୍ୟଥା ଯଦି(rc == 0) {
#ifdef _MSC_VER
#define CURL_LIBSSH2_VFS_SIZE_MASK "I64u"
#ଅନ୍ୟଥା
#define CURL_LIBSSH2_VFS_SIZE_MASK "llu"
#endif
    CURLcode result;
    ଅକ୍ଷର *tmp = curl_maprintf("statvfs:\n"
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
    ଯଦି(!tmp) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }

    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    ଯଦି(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      ଫେରନ୍ତୁ result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_create_dirs_mkdir(ସଂରଚନା Curl_easy *data,
                                                 ସଂରଚନା ssh_conn *sshc,
                                                 ସଂରଚନା SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(strlen(sshp->path)),
                             (long)data->set.new_directory_perms);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  ଯଦି(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    unsigned long sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    ଯଦି((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      ଫେରନ୍ତୁ sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_readdir_init(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା ssh_conn *sshc,
                                            ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  Curl_pgrsSetDownloadSize(data, -1);
  ଯଦି(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    ଫେରନ୍ତୁ CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  ଯଦି(!sshc->sftp_handle) {
    unsigned long sftperr;
    ଯଦି(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    ଫେରନ୍ତୁ sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_readdir_link(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା ssh_conn *sshc,
                                            ସଂରଚନା SSHPROTO *sshp)
{
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  ଯଦି(rc < 0)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  result = curlx_dyn_addf(&sshp->readdir, " -> %s", sshp->readdir_filename);
  ଯଦି(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_scp_download_init(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା ssh_conn *sshc,
                                            ସଂରଚନା SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  memset(&sb, 0, ଆକାର(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  ଯଦି(!sshc->ssh_channel) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssh_err;
    ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;

    ଯଦି(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
    failf(data, "%s", err_ସନ୍ଦେଶ);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    ଫେରନ୍ତୁ libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_size;
  data->req.maxdownload = (curl_off_t)sb.st_size;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_close(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା ssh_conn *sshc,
                                     ସଂରଚନା SSHPROTO *sshp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    ଯଦି(rc < 0) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_ସନ୍ଦେଶ);
    }
    sshc->sftp_handle = ଖାଲି;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  ଯଦି(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_STOP);

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_shutdown(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  ଯଦି(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    ଯଦି(rc < 0) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_ସନ୍ଦେଶ);
    }
    sshc->sftp_handle = ଖାଲି;
  }
  ଯଦି(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    ଯଦି(rc < 0) {
      infof(data, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = ଖାଲି;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_download_init(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା ssh_conn *sshc,
                                             ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(strlen(sshp->path)),
                         LIBSSH2_FXF_READ, (long)data->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  ଯଦି(!sshc->sftp_handle) {
    unsigned long sftperr;
    ଯଦି(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(data, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    ଫେରନ୍ତୁ sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_scp_upload_init(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା ssh_conn *sshc,
                                          ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)data->set.new_file_perms,
                       (libssh2_int64_t)data->state.infilesize, 0, 0);
  ଯଦି(!sshc->ssh_channel) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssh_err;
    ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
    CURLcode result;
    ଯଦି(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
    failf(data, "%s", err_ସନ୍ଦେଶ);
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
    result = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    ଯଦି(result == CURLE_SSH ||
       result == CURLE_REMOTE_FILE_NOT_FOUND)
      result = CURLE_UPLOAD_FAILED;
    ଫେରନ୍ତୁ result;
  }

  /* upload data */
  data->req.size = data->state.infilesize;
  Curl_pgrsSetUploadSize(data, data->state.infilesize);
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_session_disconnect(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  ଯଦି(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    ଯଦି(rc < 0) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ସନ୍ଦେଶ);
    }
    sshc->ssh_channel = ଖାଲି;
  }

  ଯଦି(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;

    ଯଦି(rc < 0) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to disconnect libssh2 session: %d %s", rc, err_ସନ୍ଦେଶ);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode sshc_cleanup(ସଂରଚନା ssh_conn *sshc, ସଂରଚନା Curl_easy *data,
                             bool block)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = ଖାଲି;
  }

  ଯଦି(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    ଯଦି((rc < 0) && data) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_ସନ୍ଦେଶ);
    }
    ଯଦି(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ଫେରନ୍ତୁ CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = ଖାଲି;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = ଖାଲି;
    sshc->sshagent_prev_identity = ଖାଲି;
  }

  ଯଦି(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    ଯଦି((rc < 0) && data) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to close libssh2 file: %d %s", rc, err_ସନ୍ଦେଶ);
    }
    ଯଦି(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ଫେରନ୍ତୁ CURLE_AGAIN;

    sshc->sftp_handle = ଖାଲି;
  }

  ଯଦି(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    ଯଦି((rc < 0) && data) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ସନ୍ଦେଶ);
    }
    ଯଦି(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ଫେରନ୍ତୁ CURLE_AGAIN;

    sshc->ssh_channel = ଖାଲି;
  }

  ଯଦି(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    ଯଦି((rc < 0) && data) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_ସନ୍ଦେଶ);
    }
    ଯଦି(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ଫେରନ୍ତୁ CURLE_AGAIN;

    sshc->sftp_session = ଖାଲି;
  }

  ଯଦି(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    ଯଦି((rc < 0) && data) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session, &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to free libssh2 session: %d %s", rc, err_ସନ୍ଦେଶ);
    }
    ଯଦି(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      ଫେରନ୍ତୁ CURLE_AGAIN;

    sshc->ssh_session = ଖାଲି;
  }

  /* worst-case scenario cleanup */
  DEBUGASSERT(!sshc->ssh_session);
  DEBUGASSERT(!sshc->ssh_channel);
  DEBUGASSERT(!sshc->sftp_session);
  DEBUGASSERT(!sshc->sftp_handle);
  DEBUGASSERT(!sshc->kh);
  DEBUGASSERT(!sshc->ssh_agent);

  curlx_safefree(sshc->passphrase);
  curlx_safefree(sshc->pub_ଚାବି);
  curlx_safefree(sshc->priv_ଚାବି);
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  curlx_safefree(sshc->homedir);

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_getinfo(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_filetime(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ssh_conn *sshc,
                                        ସଂରଚନା SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(strlen(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  ଯଦି(rc == 0)
    data->info.filetime = (time_t)attrs.mtime;

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_trans_init(ସଂରଚନା Curl_easy *data,
                                          ସଂରଚନା ssh_conn *sshc,
                                          ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ଅନ୍ୟଥା ଯଦି(sshp) {
    size_t plen = strlen(sshp->path);
    ଯଦି(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    ଅନ୍ୟଥା
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_upload_init(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା ssh_conn *sshc,
                                           ସଂରଚନା SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode result;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = sftp_upload_init(data, sshc, sshp, block);
  ଯଦି(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_sftp_create_dirs_init(ସଂରଚନା Curl_easy *data,
                                                ସଂରଚନା ssh_conn *sshc,
                                                ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଯଦି(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_create_dirs(ସଂରଚନା Curl_easy *data,
                                           ସଂରଚନା ssh_conn *sshc,
                                           ସଂରଚନା SSHPROTO *sshp)
{
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  ଯଦି(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(data, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_readdir(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc,
                                       ସଂରଚନା SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode result;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = sftp_readdir(data, sshc, sshp, block);
  ଯଦି(result)
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_sftp_readdir_bottom(ସଂରଚନା Curl_easy *data,
                                              ସଂରଚନା ssh_conn *sshc,
                                              ସଂରଚନା SSHPROTO *sshp)
{
  CURLcode result;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = curlx_dyn_addn(&sshp->readdir, "\n", 1);
  ଯଦି(!result)
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->readdir),
                               curlx_dyn_len(&sshp->readdir));
  ଯଦି(result) {
    curlx_dyn_free(&sshp->readdir);
    myssh_to(data, sshc, SSH_STOP);
  }
  ଅନ୍ୟଥା {
    curlx_dyn_reset(&sshp->readdir);
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_sftp_readdir_done(ସଂରଚନା Curl_easy *data,
                                            ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;

  sshc->sftp_handle = ଖାଲି;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_sftp_download_stat(ସଂରଚନା Curl_easy *data,
                                             ସଂରଚନା ssh_conn *sshc,
                                             ସଂରଚନା SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode result;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = sftp_download_stat(data, sshc, sshp, block);
  ଯଦି(result) {
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_state_scp_trans_init(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା ssh_conn *sshc,
                                         ସଂରଚନା SSHPROTO *sshp)
{
  CURLcode result;
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  result = Curl_getworkingpath(data, sshc->homedir,
                               &sshp->path);
  ଯଦି(result) {
    myssh_to(data, sshc, SSH_STOP);
    ଫେରନ୍ତୁ result;
  }

  ଯଦି(data->state.upload) {
    ଯଦି(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
      ଫେରନ୍ତୁ CURLE_UPLOAD_FAILED;
    }
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_scp_done(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  ଅନ୍ୟଥା
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_scp_send_eof(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->ssh_channel) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_channel_send_eof(sshc->ssh_channel);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;
    ଯଦି(rc) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_ସନ୍ଦେଶ);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_EOF);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_scp_wait_eof(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->ssh_channel) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;
    ଯଦି(rc) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to get channel EOF: %d %s", rc, err_ସନ୍ଦେଶ);
    }
  }
  myssh_to(data, sshc, SSH_SCP_WAIT_CLOSE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_scp_wait_close(ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->ssh_channel) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;
    ଯଦି(rc) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Channel failed to close: %d %s", rc, err_ସନ୍ଦେଶ);
    }
  }
  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_scp_channel_free(
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା ssh_conn *sshc)
{
  ଯଦି(sshc->ssh_channel) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = libssh2_channel_free(sshc->ssh_channel);
    ଯଦି(rc == LIBSSH2_ERROR_EAGAIN)
      ଫେରନ୍ତୁ CURLE_AGAIN;
    ଯଦି(rc < 0) {
      ଅକ୍ଷର *err_ସନ୍ଦେଶ = ଖାଲି;
      (ଶୂନ୍ୟ)libssh2_session_last_error(sshc->ssh_session,
                                       &err_ସନ୍ଦେଶ, ଖାଲି, 0);
      infof(data, "Failed to free libssh2 scp subsystem: %d %s", rc, err_ସନ୍ଦେଶ);
    }
    sshc->ssh_channel = ଖାଲି;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");
  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssh_state_session_free(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା ssh_conn *sshc)
{
  CURLcode result = sshc_cleanup(sshc, data, FALSE);
  ସଂରଚନା connectdata *conn = data->conn;
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  memset(sshc, 0, ଆକାର(ସଂରଚନା ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(data, sshc, SSH_STOP);
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
ସ୍ଥିର CURLcode ssh_statemachine(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ssh_conn *sshc,
                                 ସଂରଚନା SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode result = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  କର {
    ବାଛ(sshc->state) {
    ସ୍ଥିତି SSH_INIT:
      result = ssh_state_init(data, sshc);
      ଯଦି(result)
        ଭାଙ୍ଗ;
      FALLTHROUGH();

    ସ୍ଥିତି SSH_S_STARTUP:
      result = ssh_state_startup(data, sshc);
      ଯଦି(result)
        ଭାଙ୍ଗ;
      FALLTHROUGH();

    ସ୍ଥିତି SSH_HOSTKEY:
      result = ssh_state_hostkey(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTHLIST:
      result = ssh_state_authlist(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_PKEY:
      result = ssh_state_auth_pkey(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_PASS_INIT:
      result = ssh_state_auth_pass_init(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_PASS:
      result = ssh_state_auth_pass(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_HOST_INIT:
      result = ssh_state_auth_host_init(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_HOST:
      myssh_to(data, sshc, SSH_AUTH_AGENT_INIT);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_AGENT_INIT:
      result = ssh_state_auth_agent_init(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_AGENT_LIST:
      result = ssh_state_auth_agent_list(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_AGENT:
      result = ssh_state_auth_agent(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_ଚାବି_INIT:
      result = ssh_state_auth_ଚାବି_init(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_ଚାବି:
      result = ssh_state_auth_ଚାବି(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_AUTH_DONE:
      result = ssh_state_auth_done(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_INIT:
      result = ssh_state_sftp_init(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_REALPATH:
      result = ssh_state_sftp_realpath(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE_INIT:
      result = ssh_state_sftp_quote_init(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_POSTQUOTE_INIT:
      result = ssh_state_sftp_postquote_init(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE:
      result = ssh_state_sftp_quote(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_NEXT_QUOTE:
      result = ssh_state_sftp_next_quote(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE_STAT:
      result = ssh_state_sftp_quote_stat(data, sshc, sshp, block);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE_SETSTAT:
      result = ssh_state_sftp_quote_setstat(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE_SYMLINK:
      result = ssh_state_sftp_quote_symlink(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE_MKDIR:
      result = ssh_state_sftp_quote_mkdir(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE_RENAME:
      result = ssh_state_sftp_quote_rename(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE_RMDIR:
      result = ssh_state_sftp_quote_rmdir(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE_UNLINK:
      result = ssh_state_sftp_quote_unlink(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_QUOTE_STATVFS:
      result = ssh_state_sftp_quote_statvfs(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_GETINFO:
      result = ssh_state_sftp_getinfo(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_FILETIME:
      result = ssh_state_sftp_filetime(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_TRANS_INIT:
      result = ssh_state_sftp_trans_init(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_UPLOAD_INIT:
      result = ssh_state_sftp_upload_init(data, sshc, sshp, block);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_CREATE_DIRS_INIT:
      result = ssh_state_sftp_create_dirs_init(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_CREATE_DIRS:
      result = ssh_state_sftp_create_dirs(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_CREATE_DIRS_MKDIR:
      result = ssh_state_sftp_create_dirs_mkdir(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_READDIR_INIT:
      result = ssh_state_sftp_readdir_init(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_READDIR:
      result = ssh_state_sftp_readdir(data, sshc, sshp, block);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_READDIR_LINK:
      result = ssh_state_sftp_readdir_link(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_READDIR_BOTTOM:
      result = ssh_state_sftp_readdir_bottom(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_READDIR_DONE:
      result = ssh_state_sftp_readdir_done(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_DOWNLOAD_INIT:
      result = ssh_state_sftp_download_init(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_DOWNLOAD_STAT:
      result = ssh_state_sftp_download_stat(data, sshc, sshp, block);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_CLOSE:
      result = ssh_state_sftp_close(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SFTP_SHUTDOWN:
      result = ssh_state_sftp_shutdown(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SCP_TRANS_INIT:
      result = ssh_state_scp_trans_init(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SCP_UPLOAD_INIT:
      result = ssh_state_scp_upload_init(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SCP_DOWNLOAD_INIT:
      result = ssh_state_scp_download_init(data, sshc, sshp);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SCP_DONE:
      result = ssh_state_scp_done(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SCP_SEND_EOF:
      result = ssh_state_scp_send_eof(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SCP_WAIT_EOF:
      result = ssh_state_scp_wait_eof(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SCP_WAIT_CLOSE:
      result = ssh_state_scp_wait_close(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SCP_CHANNEL_FREE:
      result = ssh_state_scp_channel_free(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SESSION_DISCONNECT:
      result = ssh_state_session_disconnect(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_SESSION_FREE:
      result = ssh_state_session_free(data, sshc);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSH_QUIT:
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* internal error */
      myssh_to(data, sshc, SSH_STOP);
      ଭାଙ୍ଗ;
    }

  } ଯେତେବେଳେ(!result && (sshc->state != SSH_STOP) && !*block);

  ଯଦି(result == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    result = CURLE_OK;
  }
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *block);

  ଫେରନ୍ତୁ result;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
ସ୍ଥିର ଶୂନ୍ୟ ssh_block2waitfor(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ssh_conn *sshc,
                              bool block)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir = 0;
  (ଶୂନ୍ୟ)data;
  ଯଦି(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    ଯଦି(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  ଯଦି(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
ସ୍ଥିର CURLcode ssh_multi_statemach(ସଂରଚନା Curl_easy *data, bool *done)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ସଂରଚନା SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  ଯଦି(!sshc || !sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  କର {
    result = ssh_statemachine(data, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } ଯେତେବେଳେ(!result && !*done && !block);
  ssh_block2waitfor(data, sshc, block);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssh_block_statemach(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ssh_conn *sshc,
                                    ସଂରଚନା SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା curltime start = *Curl_pgrs_now(data);

  ଯେତେବେଳେ((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t ବାମ_ms = 1000;

    result = ssh_statemachine(data, sshc, sshp, &block);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    ଯଦି(!disconnect) {
      result = Curl_pgrsCheck(data);
      ଯଦି(result)
        ଭାଙ୍ଗ;

      ବାମ_ms = Curl_timeleft_ms(data);
      ଯଦି(ବାମ_ms < 0) {
        failf(data, "Operation timed out");
        ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
      }
    }
    ଅନ୍ୟଥା ଯଦି(curlx_ptimediff_ms(Curl_pgrs_now(data), &start) > 1000) {
      /* disconnect timeout */
      failf(data, "Disconnect timed out");
      result = CURLE_OK;
      ଭାଙ୍ଗ;
    }

    ଯଦି(block) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = data->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      ଯଦି(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      ଯଦି(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (ଶୂନ୍ୟ)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!ବାମ_ms || ବାମ_ms > 1000) ? 1000 : ବାମ_ms);
    }
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ myssh_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା SSHPROTO *sshp = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->readdir);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

ସ୍ଥିର ଶୂନ୍ୟ myssh_conn_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା ssh_conn *sshc = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  sshc_cleanup(sshc, ଖାଲି, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
ସ୍ଥିର CURLcode ssh_setup_connection(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା ssh_conn *sshc;
  ସଂରଚନା SSHPROTO *sshp;
  (ଶୂନ୍ୟ)conn;

  sshc = curlx_calloc(1, ଆକାର(*sshc));
  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଯଦି(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, ଆକାର(*sshp));
  ଯଦି(!sshp)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->readdir, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  ଯଦି(Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଫେରନ୍ତୁ Curl_ssh_setup_pkey(data, sshc);
}

ସ୍ଥିର Curl_recv scp_recv, sftp_recv;
ସ୍ଥିର Curl_send scp_send, sftp_send;

#ifndef CURL_DISABLE_PROXY
ସ୍ଥିର ssize_t ssh_tls_recv(libssh2_socket_t sock, ଶୂନ୍ୟ *buffer,
                            size_t length, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଶୂନ୍ୟ **abstract)
{
  ସଂରଚନା Curl_easy *data = (ସଂରଚନା Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nread;
  CURLcode result;
  ସଂରଚନା connectdata *conn = data->conn;
  Curl_recv *backup = conn->recv[sockindex];
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ଶୂନ୍ୟ)flags;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  result = Curl_conn_recv(data, sockindex, buffer, length, &nread);
  conn->recv[sockindex] = backup;
  ଯଦି(result == CURLE_AGAIN)
    ଫେରନ୍ତୁ -EAGAIN; /* magic return code for libssh2 */
  ଅନ୍ୟଥା ଯଦି(result)
    ଫେରନ୍ତୁ -1; /* generic error */
  Curl_debug(data, CURLINFO_DATA_IN, (ଅଚଳ ଅକ୍ଷର *)buffer, nread);
  ଫେରନ୍ତୁ (ssize_t)nread;
}

ସ୍ଥିର ssize_t ssh_tls_send(libssh2_socket_t sock, ଅଚଳ ଶୂନ୍ୟ *buffer,
                            size_t length, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଶୂନ୍ୟ **abstract)
{
  ସଂରଚନା Curl_easy *data = (ସଂରଚନା Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(data, sock);
  size_t nwrite;
  CURLcode result;
  ସଂରଚନା connectdata *conn = data->conn;
  Curl_send *backup = conn->send[sockindex];
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (ଶୂନ୍ୟ)flags;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  result = Curl_conn_send(data, sockindex, buffer, length, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  ଯଦି(result == CURLE_AGAIN)
    ଫେରନ୍ତୁ -EAGAIN; /* magic return code for libssh2 */
  ଅନ୍ୟଥା ଯଦି(result)
    ଫେରନ୍ତୁ -1; /* error */
  Curl_debug(data, CURLINFO_DATA_OUT, (ଅଚଳ ଅକ୍ଷର *)buffer, nwrite);
  ଫେରନ୍ତୁ (ssize_t)nwrite;
}
#endif

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
ସ୍ଥିର CURLcode ssh_connect(ସଂରଚନା Curl_easy *data, bool *done)
{
#ifdef CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#endif
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result;

#ଯଦି LIBSSH2_VERSION_ଅଙ୍କ >= 0x010b00
  {
    ଅଚଳ ଅକ୍ଷର *crypto_str;
    ବାଛ(libssh2_crypto_engine()) {
    ସ୍ଥିତି libssh2_gcrypt:
      crypto_str = "libgcrypt";
      ଭାଙ୍ଗ;
    ସ୍ଥିତି libssh2_mbedtls:
      crypto_str = "mbedTLS";
      ଭାଙ୍ଗ;
    ସ୍ଥିତି libssh2_openssl:
      crypto_str = "openssl compatible";
      ଭାଙ୍ଗ;
    ସ୍ଥିତି libssh2_os400qc3:
      crypto_str = "OS400QC3";
      ଭାଙ୍ଗ;
    ସ୍ଥିତି libssh2_wincng:
      crypto_str = "WinCNG";
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      crypto_str = ଖାଲି;
      ଭାଙ୍ଗ;
    }
    ଯଦି(crypto_str)
      infof(data, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#endif

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

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

  ଯଦି(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#ଯଦି LIBSSH2_VERSION_ଅଙ୍କ >= 0x010B00
  ଯଦି(data->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (long)(data->set.server_response_timeout / 1000));
  }
#endif

#ifndef CURL_DISABLE_PROXY
  ଯଦି(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#ଯଦି LIBSSH2_VERSION_ଅଙ୍କ >= 0x010b01
    infof(data, "SSH: using HTTPS proxy");
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic କାଢ଼
#endif
#ଅନ୍ୟଥା
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    ସଂଘ receive {
      ଶୂନ୍ୟ *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, ଶୂନ୍ୟ *, size_t, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ **);
    };
    ସଂଘ transfer {
      ଶୂନ୍ୟ *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, ଅଚଳ ଶୂନ୍ୟ *, size_t, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ **);
    };
    ସଂଘ receive sshrecv;
    ସଂଘ transfer sshsend;

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
  ଯଦି(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  ଅନ୍ୟଥା {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  ଯଦି(data->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(data, "SSH: failed to enable compression for session");
  }

  ଯଦି(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    ଯଦି(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = ଖାଲି;
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    ଯଦି(rc < 0)
      infof(data, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
  }

#ifdef CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(data, "SSH: socket %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sock);
#endif

  myssh_to(data, sshc, SSH_INIT);

  result = ssh_multi_statemach(data, done);

  ଫେରନ୍ତୁ result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

ସ୍ଥିର CURLcode scp_perform(ସଂରଚନା Curl_easy *data,
                            bool *connected,
                            bool *dophase_done)
{
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ଯଦି(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ଫେରନ୍ତୁ result;
}

/* called from multi.c while DOing */
ସ୍ଥିର CURLcode scp_doing(ସଂରଚନା Curl_easy *data,
                          bool *dophase_done)
{
  CURLcode result;
  result = ssh_multi_statemach(data, dophase_done);

  ଯଦି(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ଫେରନ୍ତୁ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ସ୍ଥିର CURLcode scp_disconnect(ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ସଂରଚନା SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (ଶୂନ୍ୟ)dead_connection;

  ଯଦି(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = ssh_block_statemach(data, sshc, sshp, TRUE);
  }

  ଯଦି(sshc)
    ଫେରନ୍ତୁ sshc_cleanup(sshc, data, TRUE);
  ଫେରନ୍ତୁ result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
ସ୍ଥିର CURLcode ssh_done(ସଂରଚନା Curl_easy *data, CURLcode status)
{
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  ସଂରଚନା SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;

  ଯଦି(!sshc || !sshp)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(!status)
    /* run the state-machine */
    result = ssh_block_statemach(data, sshc, sshp, FALSE);
  ଅନ୍ୟଥା
    result = status;

  ଯଦି(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    ଫେରନ୍ତୁ CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode scp_done(ସଂରଚନା Curl_easy *data, CURLcode status,
                         bool premature)
{
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (ଶୂନ୍ୟ)premature;

  ଯଦି(sshc && !status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  ଫେରନ୍ତୁ ssh_done(data, status);
}

ସ୍ଥିର CURLcode scp_send(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                         ଅଚଳ uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nwritten;

  (ଶୂନ୍ୟ)sockindex; /* we only support SCP on the fixed known primary socket */
  (ଶୂନ୍ୟ)eos;
  *pnwritten = 0;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (ଅଚଳ ଅକ୍ଷର *)mem, len);

  ssh_block2waitfor(data, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  ଯଦି(nwritten == LIBSSH2_ERROR_EAGAIN)
    result = CURLE_AGAIN;
  ଅନ୍ୟଥା ଯଦି(nwritten < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nwritten);
  ଅନ୍ୟଥା
    *pnwritten = (size_t)nwritten;

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode scp_recv(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                         ଅକ୍ଷର *mem, size_t len, size_t *pnread)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;
  ssize_t nread;

  (ଶୂନ୍ୟ)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  ଯଦି(nread == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;
  ଅନ୍ୟଥା ଯଦି(nread < LIBSSH2_ERROR_NONE)
    result = libssh2_session_error_to_CURLE((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nread);
  ଅନ୍ୟଥା
    *pnread = (size_t)nread;

  ଫେରନ୍ତୁ result;
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

ସ୍ଥିର CURLcode sftp_perform(ସଂରଚନା Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = ssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  ଯଦି(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  ଫେରନ୍ତୁ result;
}

/* called from multi.c while DOing */
ସ୍ଥିର CURLcode sftp_doing(ସଂରଚନା Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = ssh_multi_statemach(data, dophase_done);

  ଯଦି(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  ଫେରନ୍ତୁ result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
ସ୍ଥିର CURLcode sftp_disconnect(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn, bool dead_connection)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ସଂରଚନା SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (ଶୂନ୍ୟ)dead_connection;

  ଯଦି(sshc) {
    ଯଦି(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(data, "DISCONNECT starts now");
      myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
      result = ssh_block_statemach(data, sshc, sshp, TRUE);
      CURL_TRC_SSH(data, "DISCONNECT is done -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    }
    sshc_cleanup(sshc, data, TRUE);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode sftp_done(ସଂରଚନା Curl_easy *data, CURLcode status,
                          bool premature)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  ଯଦି(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    ଯଦି(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  ଫେରନ୍ତୁ ssh_done(data, status);
}

/* return number of sent bytes */
ସ୍ଥିର CURLcode sftp_send(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                          ଅଚଳ uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (ଶୂନ୍ୟ)sockindex;
  (ଶୂନ୍ୟ)eos;
  *pnwritten = 0;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (ଅଚଳ ଅକ୍ଷର *)mem, len);

  ssh_block2waitfor(data, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  ଯଦି(nwrite == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;
  ଅନ୍ୟଥା ଯଦି(nwrite < LIBSSH2_ERROR_NONE)
    ଫେରନ୍ତୁ libssh2_session_error_to_CURLE((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nwrite);
  *pnwritten = (size_t)nwrite;
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
ସ୍ଥିର CURLcode sftp_recv(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                          ଅକ୍ଷର *mem, size_t len, size_t *pnread)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (ଶୂନ୍ୟ)sockindex;
  *pnread = 0;

  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(data, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  ଯଦି(nread == LIBSSH2_ERROR_EAGAIN)
    ଫେରନ୍ତୁ CURLE_AGAIN;
  ଅନ୍ୟଥା ଯଦି(nread < 0)
    ଫେରନ୍ତୁ libssh2_session_error_to_CURLE((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nread);

  *pnread = (size_t)nread;
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
ସ୍ଥିର CURLcode ssh_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  ଯଦି(!sshc)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  data->req.size = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  ଯଦି(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  ଅନ୍ୟଥା
    result = sftp_perform(data, &connected, done);

  ଫେରନ୍ତୁ result;
}

CURLcode Curl_ssh_init(ଶୂନ୍ୟ)
{
  ଯଦି(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh2_init failed\n"));
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_ssh_cleanup(ଶୂନ୍ୟ)
{
  libssh2_exit();
}

ଶୂନ୍ୟ Curl_ssh_version(ଅକ୍ଷର *buffer, size_t buflen)
{
  (ଶୂନ୍ୟ)curl_msnprintf(buffer, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
ସ୍ଥିର ଶୂନ୍ୟ ssh_attach(ସଂରଚନା Curl_easy *data, ସଂରଚନା connectdata *conn)
{
  DEBUGASSERT(data);
  DEBUGASSERT(conn);
  ଯଦି(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    ଯଦି(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      ଶୂନ୍ୟ **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = data;
    }
  }
}

/*
 * SCP protocol handler.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_scp = {
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
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_sftp = {
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
