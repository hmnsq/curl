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
#समावेश "curl_setup.h"

#अगर_घोषित USE_LIBSSH2

/* #define CURL_LIBSSH2_DEBUG */

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित __VMS
#समावेश <in.h>
#समावेश <inet.h>
#पूर्ण_अगर

#समावेश "urldata.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "transfer.h"
#समावेश "vssh/ssh.h"
#समावेश "url.h"
#समावेश "cfilters.h"
#समावेश "connect.h"
#समावेश "parsedate.h" /* for the week day and month names */
#समावेश "multiif.h"
#समावेश "select.h"
#समावेश "curlx/win32-fopen.h"
#समावेश "vssh/vssh.h"
#समावेश "curlx/strparse.h"
#समावेश "curlx/base64.h" /* for curlx_base64_encode() */

अटल स्थिर अक्षर *sftp_libssh2_strerror(अचिन्हित दीर्घ err)
{
  चयन(err) {
  हाल LIBSSH2_FX_NO_SUCH_FILE:
    वापस "No such file or directory";

  हाल LIBSSH2_FX_PERMISSION_DENIED:
    वापस "Permission denied";

  हाल LIBSSH2_FX_FAILURE:
    वापस "Operation failed";

  हाल LIBSSH2_FX_BAD_MESSAGE:
    वापस "Bad message from SFTP server";

  हाल LIBSSH2_FX_NO_CONNECTION:
    वापस "Not connected to SFTP server";

  हाल LIBSSH2_FX_CONNECTION_LOST:
    वापस "Connection to SFTP server lost";

  हाल LIBSSH2_FX_OP_UNSUPPORTED:
    वापस "Operation not supported by SFTP server";

  हाल LIBSSH2_FX_INVALID_HANDLE:
    वापस "Invalid handle";

  हाल LIBSSH2_FX_NO_SUCH_PATH:
    वापस "No such file or directory";

  हाल LIBSSH2_FX_FILE_ALREADY_EXISTS:
    वापस "File already exists";

  हाल LIBSSH2_FX_WRITE_PROTECT:
    वापस "File is write protected";

  हाल LIBSSH2_FX_NO_MEDIA:
    वापस "No media";

  हाल LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
    वापस "Disk full";

  हाल LIBSSH2_FX_QUOTA_EXCEEDED:
    वापस "User quota exceeded";

  हाल LIBSSH2_FX_UNKNOWN_PRINCIPAL:
    वापस "Unknown principal";

  हाल LIBSSH2_FX_LOCK_CONFLICT:
    वापस "File lock conflict";

  हाल LIBSSH2_FX_DIR_NOT_EMPTY:
    वापस "Directory not empty";

  हाल LIBSSH2_FX_NOT_A_DIRECTORY:
    वापस "Not a directory";

  हाल LIBSSH2_FX_INVALID_FILENAME:
    वापस "Invalid filename";

  हाल LIBSSH2_FX_LINK_LOOP:
    वापस "Link points to itself";
  }
  वापस "Unknown error in libssh2";
}

अटल व्योम kbd_callback(स्थिर अक्षर *name, पूर्णांक name_len,
                         स्थिर अक्षर *instruction, पूर्णांक instruction_len,
                         पूर्णांक अंक_prompts,
                         स्थिर LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         व्योम **abstract)
{
  काष्ठा Curl_easy *तथ्य = (काष्ठा Curl_easy *)*abstract;

#अगर_घोषित CURL_LIBSSH2_DEBUG
  curl_mfprintf(मानक_त्रुटि, "name=%s\n", name);
  curl_mfprintf(मानक_त्रुटि, "name_len=%d\n", name_len);
  curl_mfprintf(मानक_त्रुटि, "instruction=%s\n", instruction);
  curl_mfprintf(मानक_त्रुटि, "instruction_len=%d\n", instruction_len);
  curl_mfprintf(मानक_त्रुटि, "num_prompts=%d\n", अंक_prompts);
#अन्यथा
  (व्योम)name;
  (व्योम)name_len;
  (व्योम)instruction;
  (व्योम)instruction_len;
#पूर्ण_अगर /* CURL_LIBSSH2_DEBUG */
  अगर(अंक_prompts == 1) {
    काष्ठा connectdata *conn = तथ्य->conn;
    स्थिर अक्षर *passwd = Curl_creds_passwd(conn->creds);
    /* this function must allocate memory that can be freed by libssh2, which
       uses the LIBSSH2_FREE_FUNC callback */
    responses[0].text = Curl_cstrdup(passwd);
    responses[0].लंबाई = responses[0].text ? curlx_uztoui(म_माप(passwd)) : 0;
  }
  (व्योम)prompts;
} /* kbd_callback */

अटल CURLcode sftp_libssh2_error_to_CURLE(अचिन्हित दीर्घ err)
{
  चयन(err) {
  हाल LIBSSH2_FX_OK:
    वापस CURLE_OK;

  हाल LIBSSH2_FX_NO_SUCH_FILE:
  हाल LIBSSH2_FX_NO_SUCH_PATH:
    वापस CURLE_REMOTE_FILE_NOT_FOUND;

  हाल LIBSSH2_FX_PERMISSION_DENIED:
  हाल LIBSSH2_FX_WRITE_PROTECT:
  हाल LIBSSH2_FX_LOCK_CONFLICT:
    वापस CURLE_REMOTE_ACCESS_DENIED;

  हाल LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
  हाल LIBSSH2_FX_QUOTA_EXCEEDED:
    वापस CURLE_REMOTE_DISK_FULL;

  हाल LIBSSH2_FX_FILE_ALREADY_EXISTS:
    वापस CURLE_REMOTE_FILE_EXISTS;

  हाल LIBSSH2_FX_DIR_NOT_EMPTY:
    वापस CURLE_QUOTE_ERROR;

  शेष:
    अवरोध;
  }

  वापस CURLE_SSH;
}

अटल CURLcode libssh2_session_error_to_CURLE(पूर्णांक err)
{
  चयन(err) {
  /* Ordered by order of appearance in libssh2.h */
  हाल LIBSSH2_ERROR_NONE:
    वापस CURLE_OK;

  /* This is the error returned by libssh2_scp_recv2
   * on unknown file */
  हाल LIBSSH2_ERROR_SCP_PROTOCOL:
    वापस CURLE_REMOTE_FILE_NOT_FOUND;

  हाल LIBSSH2_ERROR_SOCKET_NONE:
    वापस CURLE_COULDNT_CONNECT;

  हाल LIBSSH2_ERROR_ALLOC:
    वापस CURLE_OUT_OF_MEMORY;

  हाल LIBSSH2_ERROR_SOCKET_SEND:
    वापस CURLE_SEND_ERROR;

  हाल LIBSSH2_ERROR_HOSTKEY_INIT:
  हाल LIBSSH2_ERROR_HOSTKEY_SIGN:
  हाल LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED:
  हाल LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED:
    वापस CURLE_PEER_FAILED_VERIFICATION;

  हाल LIBSSH2_ERROR_PASSWORD_EXPIRED:
    वापस CURLE_LOGIN_DENIED;

  हाल LIBSSH2_ERROR_SOCKET_TIMEOUT:
  हाल LIBSSH2_ERROR_TIMEOUT:
    वापस CURLE_OPERATION_TIMEDOUT;

  हाल LIBSSH2_ERROR_EAGAIN:
    वापस CURLE_AGAIN;
  }

  वापस CURLE_SSH;
}

/* These functions are made to use the libcurl memory functions - NOT the
   debugmem functions, as that leads us to trigger on libssh2 memory leaks
   that are not ours to care for */

अटल LIBSSH2_ALLOC_FUNC(my_libssh2_malloc)
{
  (व्योम)abstract;
  वापस Curl_cmalloc(गिनती);
}

अटल LIBSSH2_REALLOC_FUNC(my_libssh2_realloc)
{
  (व्योम)abstract;
  वापस Curl_crealloc(ptr, गिनती);
}

अटल LIBSSH2_FREE_FUNC(my_libssh2_free)
{
  (व्योम)abstract;
  अगर(ptr) /* ssh2 agent sometimes call free with null ptr */
    Curl_cfree(ptr);
}

अटल पूर्णांक sshkeycallback(CURL *easy,
                          स्थिर काष्ठा curl_khkey *knownkey, /* known */
                          स्थिर काष्ठा curl_khkey *foundkey, /* found */
                          क्रमागत curl_khmatch match,
                          व्योम *clientp)
{
  (व्योम)easy;
  (व्योम)knownkey;
  (व्योम)foundkey;
  (व्योम)clientp;

  /* we only allow perfect matches, and we reject everything else */
  वापस (match != CURLKHMATCH_OK) ? CURLKHSTAT_REJECT : CURLKHSTAT_FINE;
}

अटल क्रमागत curl_khtype convert_ssh2_keytype(पूर्णांक sshkeytype)
{
  क्रमागत curl_khtype keytype = CURLKHTYPE_UNKNOWN;
  चयन(sshkeytype) {
  हाल LIBSSH2_HOSTKEY_TYPE_RSA:
    keytype = CURLKHTYPE_RSA;
    अवरोध;
#अगर_घोषित LIBSSH2_HOSTKEY_TYPE_DSS
  हाल LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
    keytype = CURLKHTYPE_DSS;
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित LIBSSH2_HOSTKEY_TYPE_ECDSA_256
  हाल LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
    keytype = CURLKHTYPE_ECDSA;
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित LIBSSH2_HOSTKEY_TYPE_ECDSA_384
  हाल LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
    keytype = CURLKHTYPE_ECDSA;
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित LIBSSH2_HOSTKEY_TYPE_ECDSA_521
  हाल LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
    keytype = CURLKHTYPE_ECDSA;
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित LIBSSH2_HOSTKEY_TYPE_ED25519
  हाल LIBSSH2_HOSTKEY_TYPE_ED25519:
    keytype = CURLKHTYPE_ED25519;
    अवरोध;
#पूर्ण_अगर
  }
  वापस keytype;
}

अटल CURLcode ssh_knownhost(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ssh_conn *sshc)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा libssh2_knownhost *host = शून्य;
  स्थिर अक्षर *remotekey = शून्य;
  पूर्णांक keycheck = LIBSSH2_KNOWNHOST_CHECK_FAILURE;
  पूर्णांक keybit = 0;
  पूर्णांक sshkeytype = 0;
  माप_प्रकार keylen = 0;
  पूर्णांक rc = 0;
  CURLcode परिणाम = CURLE_OK;

  अगर(!CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS)) {
    infof(तथ्य, "SSH: no knownhosts file configured");
    वापस CURLE_OK;
  }

  remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                      &keylen, &sshkeytype);
  अगर(remotekey) {
    /*
     * A subject to figure out is what hostname we need to pass in here.
     * What hostname does OpenSSH store in its file if an IDN name is
     * used?
     */
    काष्ठा Curl_mapi_guard guard;
    क्रमागत curl_khmatch keymatch;
    curl_sshkeycallback func =
      तथ्य->set.ssh_keyfunc ? तथ्य->set.ssh_keyfunc : sshkeycallback;
    काष्ठा curl_khkey knownkey;
    काष्ठा curl_khkey *knownkeyp = शून्य;
    काष्ठा curl_khkey foundkey;

    चयन(sshkeytype) {
    हाल LIBSSH2_HOSTKEY_TYPE_RSA:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_SSHRSA;
      अवरोध;
#अगर_घोषित LIBSSH2_HOSTKEY_TYPE_DSS
    हाल LIBSSH2_HOSTKEY_TYPE_DSS:  /* deprecated upstream */
      keybit = LIBSSH2_KNOWNHOST_कुंजी_SSHDSS;
      अवरोध;
#पूर्ण_अगर
    हाल LIBSSH2_HOSTKEY_TYPE_ECDSA_256:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_ECDSA_256;
      अवरोध;
    हाल LIBSSH2_HOSTKEY_TYPE_ECDSA_384:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_ECDSA_384;
      अवरोध;
    हाल LIBSSH2_HOSTKEY_TYPE_ECDSA_521:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_ECDSA_521;
      अवरोध;
    हाल LIBSSH2_HOSTKEY_TYPE_ED25519:
      keybit = LIBSSH2_KNOWNHOST_कुंजी_ED25519;
      अवरोध;
    शेष:
      infof(तथ्य, "SSH: unsupported host key type for knownhosts check");
      keybit = 0;
      अवरोध;
    }
    अगर(!keybit)
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

      infof(तथ्य, "SSH: host check %d, key: %s", keycheck,
            (keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) ?
            host->कुंजी : "<none>");

      /* setup 'knownkey' */
      अगर(keycheck <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH) {
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
      keymatch = (क्रमागत curl_khmatch)keycheck;

      /* Ask the callback how to behave */
      CURL_CBAPI_START(&guard, तथ्य, easy_ssh_keyfunc);
      rc = func(तथ्य, knownkeyp, /* from the knownhosts file */
                &foundkey, /* from the remote host */
                keymatch, तथ्य->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);
    }
  }
  अन्यथा {
    /* no remotekey means failure! */
    infof(तथ्य, "SSH: host offers no public key");
    rc = CURLKHSTAT_REJECT;
  }

  चयन(rc) {
  शेष: /* unknown return codes is the same as reject */
  हाल CURLKHSTAT_REJECT:
    infof(तथ्य, "SSH: knownhost check failed");
    myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
    FALLTHROUGH();
  हाल CURLKHSTAT_DEFER:
    /* DEFER means bail out but keep the SSH_HOSTKEY state */
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    अवरोध;
  हाल CURLKHSTAT_FINE_REPLACE:
    /* remove old host+key that does not match */
    अगर(host)
      libssh2_knownhost_del(sshc->kh, host);
    FALLTHROUGH();
  हाल CURLKHSTAT_FINE:
  हाल CURLKHSTAT_FINE_ADD_TO_FILE:
    /* proceed */
    अगर(keycheck != LIBSSH2_KNOWNHOST_CHECK_MATCH) {
      पूर्णांक addrc;
      स्थिर अक्षर *hostbuf;
      अक्षर *hostport = शून्य;
      अगर(conn->origin->port != PORT_SSH) {
        hostbuf = hostport = curl_maprintf("[%s]:%u", conn->origin->hostname,
                                           conn->origin->port);
        अगर(!hostbuf)
          infof(तथ्य, "WARNING: failed allocating buffer for [host]:port");
      }
      अन्यथा
        hostbuf = conn->origin->hostname;
      अगर(hostbuf) {
        /* the found host+key did not match but has been told to be fine
           anyway so we add it in memory */
        addrc = libssh2_knownhost_addc(sshc->kh, hostbuf, शून्य,
                                       remotekey, keylen, शून्य, 0,
                                       LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                                       LIBSSH2_KNOWNHOST_KEYENC_RAW |
                                       keybit, शून्य);
        अगर(addrc)
          infof(तथ्य, "WARNING: adding the known host %s failed", hostbuf);
        अन्यथा अगर(rc == CURLKHSTAT_FINE_ADD_TO_FILE ||
                rc == CURLKHSTAT_FINE_REPLACE) {
          /* now we write the entire in-memory list of known hosts to the
             known_hosts file */
          पूर्णांक wrc =
            libssh2_knownhost_writefile(
              sshc->kh, CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS),
              LIBSSH2_KNOWNHOST_FILE_OPENSSH);
          अगर(wrc) {
            infof(तथ्य, "WARNING: writing %s failed",
                  CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS));
          }
        }
      }
      curlx_free(hostport);
    }
    अन्यथा
      infof(तथ्य, "SSH: knownhost entry matches host key");
    अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode ssh_check_fingerprint(काष्ठा Curl_easy *तथ्य,
                                      काष्ठा ssh_conn *sshc)
{
  स्थिर अक्षर *pubkey_md5 =
    CURL_EASY_STR(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_MD5);
  स्थिर अक्षर *pubkey_sha256 =
    CURL_EASY_STR(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_SHA256);

  अगर(pubkey_sha256) {
    स्थिर अक्षर *fingerprint = शून्य;
    अक्षर *fingerprint_b64 = शून्य;
    माप_प्रकार fingerprint_b64_len;
    माप_प्रकार pub_pos = 0;
    माप_प्रकार b64_pos = 0;

    infof(तथ्य, "SSH: SHA256 public key '%s'", pubkey_sha256);
    /* The fingerprint points to static storage (!), do not free() it. */
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_SHA256);
    अगर(!fingerprint) {
      failf(तथ्य,
            "Denied establishing ssh session: SHA256 fingerprint "
            "not available");
      myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
      वापस CURLE_PEER_FAILED_VERIFICATION;
    }

    /* The length of fingerprint is 32 bytes for SHA256.
     * See libssh2_hostkey_hash documentation. */
    अगर(curlx_base64_encode((स्थिर uint8_t *)fingerprint, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
      वापस CURLE_PEER_FAILED_VERIFICATION;
    }

    अगर(!fingerprint_b64) {
      failf(तथ्य, "SHA256 fingerprint could not be encoded");
      myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
      वापस CURLE_PEER_FAILED_VERIFICATION;
    }

    infof(तथ्य, "SSH: SHA256 fingerprint '%s'", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    जबतक((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    जबतक((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    अगर((pub_pos != b64_pos) ||
       म_भेदन(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(तथ्य,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
      वापस CURLE_PEER_FAILED_VERIFICATION;
    }

    curlx_free(fingerprint_b64);

    infof(तथ्य, "SSH: SHA256 checksum match");
  }

  अगर(pubkey_md5) {
    अक्षर md5buffer[33];
    स्थिर अक्षर *fingerprint;

    infof(तथ्य, "SSH: MD5 public key '%s'", pubkey_md5);
    fingerprint = libssh2_hostkey_hash(sshc->ssh_session,
                                       LIBSSH2_HOSTKEY_HASH_MD5);

    अगर(fingerprint) {
      /* The fingerprint points to static storage (!), do not free() it. */
      पूर्णांक i;
      क्रम(i = 0; i < 16; i++) {
        curl_msnprintf(&md5buffer[i * 2], 3, "%02x",
                       (अचिन्हित अक्षर)fingerprint[i]);
      }

      infof(तथ्य, "SSH: MD5 fingerprint '%s'", md5buffer);
    }

    /* This does NOT verify the length of 'pubkey_md5' separately, which
       makes the comparison below fail unless it is exactly 32 characters */
    अगर(!fingerprint || !curl_strequal(md5buffer, pubkey_md5)) {
      अगर(fingerprint) {
        failf(तथ्य,
              "Denied establishing ssh session: mismatch MD5 fingerprint. "
              "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      }
      अन्यथा {
        failf(तथ्य,
              "Denied establishing ssh session: MD5 fingerprint "
              "not available");
      }
      myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
      वापस CURLE_PEER_FAILED_VERIFICATION;
    }
    infof(तथ्य, "SSH: MD5 checksum match");
  }

  अगर(!pubkey_md5 && !pubkey_sha256) {
    अगर(तथ्य->set.ssh_hostkeyfunc) {
      माप_प्रकार keylen = 0;
      पूर्णांक sshkeytype = 0;
      पूर्णांक rc = 0;
      /* we handle the process to the callback */
      स्थिर अक्षर *remotekey = libssh2_session_hostkey(sshc->ssh_session,
                                                      &keylen, &sshkeytype);
      अगर(remotekey) {
        काष्ठा Curl_mapi_guard guard;
        क्रमागत curl_khtype keytype = convert_ssh2_keytype(sshkeytype);
        CURL_CBAPI_START(&guard, तथ्य, easy_ssh_hostkeyfunc);
        rc = तथ्य->set.ssh_hostkeyfunc(तथ्य->set.ssh_hostkeyfunc_userp,
                                       (पूर्णांक)keytype, remotekey, keylen);
        CURL_CBAPI_END(&guard);
        अगर(rc != CURLKHMATCH_OK) {
          myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
          failf(तथ्य, "SSH: callback failed host public key verification");
          वापस CURLE_PEER_FAILED_VERIFICATION;
        }
        infof(तथ्य, "SSH: verified public key via callback");
      }
      अन्यथा {
        myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
        वापस CURLE_PEER_FAILED_VERIFICATION;
      }
      वापस CURLE_OK;
    }
    अन्यथा {
      CURL_TRC_SSH(तथ्य, "no host key checksum given, checking knownhosts");
      वापस ssh_knownhost(तथ्य, sshc);
    }
  }
  अन्यथा {
    /* as we already matched, we skip the check for known hosts */
    वापस CURLE_OK;
  }
}

/*
 * ssh_force_knownhost_key_type() checks the known hosts file and try to
 * force a specific public key type from the server if an entry is found.
 */
अटल CURLcode ssh_force_knownhost_कुंजी_type(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा ssh_conn *sshc)
{
  CURLcode परिणाम = CURLE_OK;

  अटल स्थिर अक्षर hostkey_method_ssh_ed25519[] = "ssh-ed25519";
  अटल स्थिर अक्षर hostkey_method_ssh_ecdsa_521[] = "ecdsa-sha2-nistp521";
  अटल स्थिर अक्षर hostkey_method_ssh_ecdsa_384[] = "ecdsa-sha2-nistp384";
  अटल स्थिर अक्षर hostkey_method_ssh_ecdsa_256[] = "ecdsa-sha2-nistp256";
  अटल स्थिर अक्षर hostkey_method_ssh_rsa_all[] =
    "rsa-sha2-256,rsa-sha2-512,ssh-rsa";
#अगर_घोषित LIBSSH2_KNOWNHOST_कुंजी_SSHDSS
  अटल स्थिर अक्षर hostkey_method_ssh_dss[] = "ssh-dss";
#पूर्ण_अगर
  bool found = FALSE;

  अगर(sshc->kh &&
     !CURL_EASY_STR(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_MD5) &&
     !CURL_EASY_STR(तथ्य, STRING_SSH_HOST_PUBLIC_कुंजी_SHA256)) {
    काष्ठा libssh2_knownhost *store = शून्य;
    काष्ठा connectdata *conn = तथ्य->conn;
    /* key types already confirmed absent for this host, so repeat hashed
       entries of the same type do not each force a fresh checkp() scan
       of the whole known_hosts set. one slot per possible key type (indexed
       by the type shifted down) so no entry ordering can overflow it */
    bool absent_type[(LIBSSH2_KNOWNHOST_कुंजी_MASK >>
                      LIBSSH2_KNOWNHOST_कुंजी_SHIFT) + 1] = { FALSE };
    /* lets try to find our host in the known hosts file */
    जबतक(!libssh2_knownhost_get(sshc->kh, &store, store)) {
      /* For non-standard ports, the name is enclosed in */
      /* square brackets, followed by a colon and the port */
      अगर(store) {
        अगर(store->name) {
          अगर(store->name[0] == '[') {
            curl_off_t port;
            माप_प्रकार kh_name_आकार = 0;
            स्थिर अक्षर *p;
            स्थिर अक्षर *kh_name_end = म_माला(store->name, "]:");
            अगर(!kh_name_end) {
              infof(तथ्य, "SSH: invalid host pattern %s in %s",
                    store->name,
                    CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS));
              जारी;
            }
            p = kh_name_end + 2; /* start of port number */
            अगर(!curlx_str_संख्या(&p, &port, 0xffff) &&
               (kh_name_end && (port == conn->origin->port))) {
              kh_name_आकार = म_माप(store->name) - 1 - म_माप(kh_name_end);
              अगर((kh_name_आकार == म_माप(conn->origin->hostname)) &&
                 !म_भेदन(store->name + 1, conn->origin->hostname,
                          kh_name_आकार)) {
                found = TRUE;
                अवरोध;
              }
            }
          }
          अन्यथा अगर(!म_भेद(store->name, conn->origin->hostname)) {
            found = TRUE;
            अवरोध;
          }
        }
        अन्यथा अगर(store->कुंजी) {
          अचिन्हित पूर्णांक type = (store->typemask & LIBSSH2_KNOWNHOST_कुंजी_MASK)
                              >> LIBSSH2_KNOWNHOST_कुंजी_SHIFT;
          अगर(!absent_type[type]) {
            पूर्णांक keycheck = libssh2_knownhost_checkp(
              sshc->kh, conn->origin->hostname,
              (conn->origin->port != PORT_SSH) ? conn->origin->port : -1,
              store->कुंजी, म_माप(store->कुंजी),
              LIBSSH2_KNOWNHOST_TYPE_PLAIN |
                LIBSSH2_KNOWNHOST_KEYENC_BASE64 |
                (store->typemask & LIBSSH2_KNOWNHOST_कुंजी_MASK),
              शून्य);
            अगर(keycheck == LIBSSH2_KNOWNHOST_CHECK_MATCH) {
              found = TRUE;
              अवरोध;
            }
            अन्यथा अगर(keycheck == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND)
              absent_type[type] = TRUE;
          }
        }
      }
    }

    अगर(found) {
      पूर्णांक rc;
      स्थिर अक्षर *hostkey_method = शून्य;
      infof(तथ्य, "SSH: found host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS));

      चयन(store->typemask & LIBSSH2_KNOWNHOST_कुंजी_MASK) {
      हाल LIBSSH2_KNOWNHOST_कुंजी_ED25519:
        hostkey_method = hostkey_method_ssh_ed25519;
        अवरोध;
      हाल LIBSSH2_KNOWNHOST_कुंजी_ECDSA_521:
        hostkey_method = hostkey_method_ssh_ecdsa_521;
        अवरोध;
      हाल LIBSSH2_KNOWNHOST_कुंजी_ECDSA_384:
        hostkey_method = hostkey_method_ssh_ecdsa_384;
        अवरोध;
      हाल LIBSSH2_KNOWNHOST_कुंजी_ECDSA_256:
        hostkey_method = hostkey_method_ssh_ecdsa_256;
        अवरोध;
      हाल LIBSSH2_KNOWNHOST_कुंजी_SSHRSA:
        hostkey_method = hostkey_method_ssh_rsa_all;
        अवरोध;
#अगर_घोषित LIBSSH2_KNOWNHOST_कुंजी_SSHDSS
      हाल LIBSSH2_KNOWNHOST_कुंजी_SSHDSS:  /* deprecated upstream */
        hostkey_method = hostkey_method_ssh_dss;
        अवरोध;
#पूर्ण_अगर
      हाल LIBSSH2_KNOWNHOST_कुंजी_RSA1:
        failf(तथ्य, "Found host key type RSA1 which is not supported");
        वापस CURLE_SSH;
      शेष:
        failf(तथ्य, "Unknown host key type: %d",
              (store->typemask & LIBSSH2_KNOWNHOST_कुंजी_MASK));
        वापस CURLE_SSH;
      }

      infof(तथ्य, "SSH: set '%s' as hostkey type", hostkey_method);
      rc = libssh2_session_method_pref(sshc->ssh_session,
                                       LIBSSH2_METHOD_HOSTKEY, hostkey_method);
      अगर(rc) {
        अक्षर *err_संदेश = शून्य;
        पूर्णांक errlen;
        libssh2_session_last_error(sshc->ssh_session, &err_संदेश, &errlen, 0);
        failf(तथ्य, "libssh2 method '%s' failed: %s", hostkey_method, err_संदेश);
        परिणाम = libssh2_session_error_to_CURLE(rc);
      }
    }
    अन्यथा {
      infof(तथ्य, "SSH: did not find host '%s' in '%s'",
            conn->origin->hostname,
            CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS));
    }
  }

  वापस परिणाम;
}

अटल CURLcode quote_error(काष्ठा Curl_easy *तथ्य,
                            काष्ठा ssh_conn *sshc)
{
  failf(तथ्य, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  वापस CURLE_QUOTE_ERROR;
}

अटल CURLcode sftp_quote(काष्ठा Curl_easy *तथ्य,
                           काष्ठा ssh_conn *sshc,
                           काष्ठा SSHPROTO *sshp)
{
  स्थिर अक्षर *cp;
  CURLcode परिणाम = CURLE_OK;

  /*
   * Support some of the "FTP" commands
   *
   * 'sshc->quote_item' is already verified to be non-NULL before it
   * switched to this state.
   */
  स्थिर अक्षर *cmd = sshc->quote_item->तथ्य;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  अगर(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  अगर(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    अक्षर *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    अगर(!tmp)
      वापस CURLE_OUT_OF_MEMORY;
    Curl_debug(तथ्य, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(तथ्य, CURLINFO_HEADER_IN, tmp, म_माप(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, tmp, म_माप(tmp));
    curlx_free(tmp);
    अगर(!परिणाम)
      myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
    वापस परिणाम;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = म_अक्षर(cmd, ' ');
  अगर(!cp) {
    failf(तथ्य, "Syntax error command '%s', missing parameter", cmd);
    वापस CURLE_QUOTE_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  परिणाम = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  अगर(परिणाम) {
    अगर(परिणाम != CURLE_OUT_OF_MEMORY)
      failf(तथ्य, "Syntax error: Bad first parameter to '%s'", cmd);
    वापस परिणाम;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands to the server.
   * Instead, we scan for commands used by OpenSSH's sftp program and call the
   * appropriate libssh2 functions.
   */
  अगर(!म_भेदन(cmd, "chgrp ", 6) ||
     !म_भेदन(cmd, "chmod ", 6) ||
     !म_भेदन(cmd, "chown ", 6) ||
     !म_भेदन(cmd, "atime ", 6) ||
     !म_भेदन(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    अगर(परिणाम) {
      अगर(परिणाम != CURLE_OUT_OF_MEMORY)
        failf(तथ्य, "Syntax error in %s: Bad second parameter", cmd);
      curlx_safefree(sshc->quote_path1);
      वापस परिणाम;
    }
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);

    स_रखो(&sshp->quote_attrs, 0, माप(LIBSSH2_SFTP_ATTRIBUTES));
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_STAT);
    वापस परिणाम;
  }
  अगर(!म_भेदन(cmd, "ln ", 3) ||
     !म_भेदन(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    अगर(परिणाम) {
      अगर(परिणाम != CURLE_OUT_OF_MEMORY)
        failf(तथ्य, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      वापस परिणाम;
    }
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_SYMLINK);
    वापस परिणाम;
  }
  अन्यथा अगर(!म_भेदन(cmd, "mkdir ", 6)) {
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    /* create directory */
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_MKDIR);
    वापस परिणाम;
  }
  अन्यथा अगर(!म_भेदन(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    परिणाम = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    अगर(परिणाम) {
      अगर(परिणाम != CURLE_OUT_OF_MEMORY)
        failf(तथ्य, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      वापस परिणाम;
    }
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_RENAME);
    वापस परिणाम;
  }
  अन्यथा अगर(!म_भेदन(cmd, "rmdir ", 6)) {
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    /* delete directory */
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_RMDIR);
    वापस परिणाम;
  }
  अन्यथा अगर(!म_भेदन(cmd, "rm ", 3)) {
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_UNLINK);
    वापस परिणाम;
  }
  अन्यथा अगर(!म_भेदन(cmd, "statvfs ", 8)) {
    अगर(*cp)
      वापस quote_error(तथ्य, sshc);
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_STATVFS);
    वापस परिणाम;
  }

  failf(तथ्य, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  वापस CURLE_QUOTE_ERROR;
}

अटल CURLcode sftp_upload_init(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ssh_conn *sshc,
                                 काष्ठा SSHPROTO *sshp,
                                 bool *blockp)
{
  अचिन्हित दीर्घ flags;

  /*
   * NOTE!!!  libssh2 requires that the destination path is a full path
   *          that includes the destination file and name OR ends in a "/"
   *          If this is not done the destination file is named the
   *          same name as the last directory in the path.
   */

  अगर(तथ्य->state.resume_from) {
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    अगर(तथ्य->state.resume_from < 0) {
      पूर्णांक rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                    curlx_uztoui(म_माप(sshp->path)),
                                    LIBSSH2_SFTP_STAT, &attrs);
      अगर(rc == LIBSSH2_ERROR_EAGAIN) {
        *blockp = TRUE;
        वापस CURLE_OK;
      }
      अगर(rc) {
        तथ्य->state.resume_from = 0;
      }
      अन्यथा {
        curl_off_t आकार = attrs.filesize;
        अगर(आकार < 0) {
          failf(तथ्य, "Bad file size (%" FMT_OFF_T ")", आकार);
          वापस CURLE_BAD_DOWNLOAD_RESUME;
        }
        तथ्य->state.resume_from = attrs.filesize;
      }
    }
  }

  अगर(तथ्य->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_APPEND;
  }
  अन्यथा अगर(तथ्य->state.resume_from > 0) {
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
                         flags, (दीर्घ)तथ्य->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);

  अगर(!sshc->sftp_handle) {
    CURLcode परिणाम;
    अचिन्हित दीर्घ sftperr;
    पूर्णांक rc = libssh2_session_last_errno(sshc->ssh_session);

    अगर(LIBSSH2_ERROR_EAGAIN == rc) {
      *blockp = TRUE;
      वापस CURLE_OK;
    }

    अगर(LIBSSH2_ERROR_SFTP_PROTOCOL == rc)
      /* only when there was an SFTP protocol error can we extract
         the sftp error! */
      sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    अन्यथा
      sftperr = LIBSSH2_FX_OK; /* not an sftp error at all */

    अगर(sshc->secondCreateDirs) {
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      failf(तथ्य, "Creating the dir/file failed: %s",
            sftp_libssh2_strerror(sftperr));
      वापस sftp_libssh2_error_to_CURLE(sftperr);
    }
    अगर((sftperr == LIBSSH2_FX_NO_SUCH_FILE ||
        sftperr == LIBSSH2_FX_FAILURE ||
        sftperr == LIBSSH2_FX_NO_SUCH_PATH) &&
       तथ्य->set.ftp_create_missing_dirs &&
       (म_माप(sshp->path) > 1)) {
      /* try to create the path remotely */
      sshc->secondCreateDirs = 1;
      myssh_to(तथ्य, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      वापस CURLE_OK;
    }
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    परिणाम = sftp_libssh2_error_to_CURLE(sftperr);
    अगर(!परिणाम) {
      /* Sometimes, for some reason libssh2_sftp_last_error() returns zero
         even though libssh2_sftp_open() failed previously! We need to
         work around that! */
      परिणाम = CURLE_SSH;
      sftperr = LIBSSH2_FX_OK;
    }
    failf(तथ्य, "Upload failed: %s (%lu/%d)",
          sftperr != LIBSSH2_FX_OK ?
          sftp_libssh2_strerror(sftperr) : "ssh error",
          sftperr, rc);
    वापस परिणाम;
  }

  /* If we have a restart point then we need to seek to the correct
     Skip if in explicit remote append mode. */
  अगर(तथ्य->state.resume_from > 0 && !तथ्य->set.remote_append) {
    पूर्णांक seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    अगर(तथ्य->set.seek_func) {
      काष्ठा Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, तथ्य, easy_seek_func);
      seekerr = तथ्य->set.seek_func(तथ्य->set.seek_client,
                                    तथ्य->state.resume_from, शुरू_से);
      CURL_CBAPI_END(&guard);
    }

    अगर(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      अगर(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(तथ्य, "Could not seek stream");
        वापस CURLE_FTP_COULDNT_USE_REST;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      करो {
        काष्ठा Curl_mapi_guard guard;
        अक्षर scratch[4 * 1024];
        माप_प्रकार readthisamountnow =
          (तथ्य->state.resume_from - passed >
           (curl_off_t)माप(scratch)) ?
          माप(scratch) : curlx_sotouz(तथ्य->state.resume_from - passed);

        माप_प्रकार actuallyread;
        CURL_CBAPI_START(&guard, तथ्य, easy_fread_func);
        actuallyread = तथ्य->state.fread_func(scratch, 1,
                                              readthisamountnow,
                                              तथ्य->state.in);
        CURL_CBAPI_END(&guard);

        passed += actuallyread;
        अगर((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(तथ्य, "Failed to read data");
          वापस CURLE_FTP_COULDNT_USE_REST;
        }
      } जबतक(passed < तथ्य->state.resume_from);
    }

    /* now, decrease the size of the read */
    अगर(तथ्य->state.infilesize > 0) {
      तथ्य->state.infilesize -= तथ्य->state.resume_from;
      तथ्य->req.आकार = तथ्य->state.infilesize;
      Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);
    }

    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)तथ्य->state.resume_from);
  }
  अगर(तथ्य->state.infilesize > 0) {
    तथ्य->req.आकार = तथ्य->state.infilesize;
    Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);
  }
  /* upload data */
  Curl_xfer_setup_send(तथ्य, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  तथ्य->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so mark this as dirty */
  Curl_multi_mark_dirty(तथ्य);

  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस CURLE_OK;
}

अटल व्योम ssh_state_pkey_init(काष्ठा Curl_easy *तथ्य, काष्ठा ssh_conn *sshc)
{
  /*
   * Check the supported auth types in the order I feel is most secure
   * with the requested type of authentication
   */
  sshc->authed = FALSE;

  अगर((तथ्य->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) &&
     म_माला(sshc->authlist, "publickey")) {
    अगर(sshc->pub_कुंजी)
      infof(तथ्य, "SSH: trying public key file '%s'", sshc->pub_कुंजी);
    infof(तथ्य, "SSH: trying private key file '%s'", sshc->priv_कुंजी);
    myssh_to(तथ्य, sshc, SSH_AUTH_PKEY);
  }
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_AUTH_PASS_INIT);
}

अटल CURLcode sftp_quote_stat(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ssh_conn *sshc,
                                काष्ठा SSHPROTO *sshp,
                                bool *blockp)
{
  स्थिर अक्षर *cmd = sshc->quote_item->तथ्य;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any aborts or
     cancels etc. It causes libcurl to act as if the command is
     successful, whatever the server responds. */

  अगर(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  अगर(म_भेदन(cmd, "chmod", 5)) {
    /* Since chown and chgrp only set owner OR group but libssh2 wants to set
     * them both at once, we need to obtain the current ownership first. This
     * takes an extra protocol round trip.
     */
    पूर्णांक rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                                  curlx_uztoui(म_माप(sshc->quote_path2)),
                                  LIBSSH2_SFTP_STAT,
                                  &sshp->quote_attrs);
    अगर(rc == LIBSSH2_ERROR_EAGAIN) {
      *blockp = TRUE;
      वापस CURLE_OK;
    }
    अगर(rc && !sshc->acceptfail) { /* get those attributes */
      अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
      failf(तथ्य, "Attempt to get SFTP stats failed: %s",
            sftp_libssh2_strerror(sftperr));
      जाओ fail;
    }
  }

  /* Now set the new attributes... */
  अगर(!म_भेदन(cmd, "chgrp", 5)) {
    स्थिर अक्षर *p = sshc->quote_path1;
    curl_off_t gid;
    अगर(!curlx_str_संख्या(&p, &gid, अच_दीर्घ_उच्च)) {
      sshp->quote_attrs.gid = (अचिन्हित दीर्घ)gid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    अन्यथा अगर(!sshc->acceptfail) {
      failf(तथ्य, "Syntax error: chgrp gid not a number");
      जाओ fail;
    }
  }
  अन्यथा अगर(!म_भेदन(cmd, "chmod", 5)) {
    curl_off_t perms;
    स्थिर अक्षर *p = sshc->quote_path1;
    /* permissions are octal */
    अगर(curlx_str_octal(&p, &perms, 07777)) {
      failf(तथ्य, "Syntax error: chmod permissions not a number");
      जाओ fail;
    }

    sshp->quote_attrs.permissions = (अचिन्हित दीर्घ)perms;
    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
  }
  अन्यथा अगर(!म_भेदन(cmd, "chown", 5)) {
    स्थिर अक्षर *p = sshc->quote_path1;
    curl_off_t uid;
    अगर(!curlx_str_संख्या(&p, &uid, अच_दीर्घ_उच्च)) {
      sshp->quote_attrs.uid = (अचिन्हित दीर्घ)uid;
      sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    }
    अन्यथा अगर(!sshc->acceptfail) {
      failf(तथ्य, "Syntax error: chown uid not a number");
      जाओ fail;
    }
  }
  अन्यथा अगर(!म_भेदन(cmd, "atime", 5) ||
          !म_भेदन(cmd, "mtime", 5)) {
    समय_प्रकार date;
    bool fail = FALSE;

    अगर(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(तथ्य, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#अगर SIZEOF_TIME_T > SIZEOF_LONG
    अगर(date > 0xffffffff) {
      /* if 'long' cannot hold >32-bit, this date cannot be sent */
      failf(तथ्य, "date overflow");
      fail = TRUE;
    }
#पूर्ण_अगर
    अगर(fail)
      जाओ fail;
    अगर(!म_भेदन(cmd, "atime", 5))
      sshp->quote_attrs.atime = (अचिन्हित दीर्घ)date;
    अन्यथा /* mtime */
      sshp->quote_attrs.mtime = (अचिन्हित दीर्घ)date;

    sshp->quote_attrs.flags = LIBSSH2_SFTP_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_SETSTAT);
  वापस CURLE_OK;
fail:
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  वापस CURLE_QUOTE_ERROR;
}

अटल CURLcode sftp_download_stat(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ssh_conn *sshc,
                                   काष्ठा SSHPROTO *sshp,
                                   bool *blockp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  पूर्णांक rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                                curlx_uztoui(म_माप(sshp->path)),
                                LIBSSH2_SFTP_STAT, &attrs);
  अगर(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    वापस CURLE_OK;
  }
  अगर(rc ||
     !(attrs.flags & LIBSSH2_SFTP_ATTR_आकार) ||
     (attrs.filesize == 0)) {
    /*
     * libssh2_sftp_open() did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    तथ्य->req.आकार = -1;
    तथ्य->req.maxdownload = -1;
    Curl_pgrs_Set_Download_आकार(तथ्य, -1);
    attrs.filesize = 0; /* might be uninitialized but is read below */
  }
  अन्यथा {
    curl_off_t आकार = attrs.filesize;

    अगर(आकार < 0) {
      failf(तथ्य, "Bad file size (%" FMT_OFF_T ")", आकार);
      वापस CURLE_BAD_DOWNLOAD_RESUME;
    }
    अगर(तथ्य->state.use_range) {
      curl_off_t from;
      CURLcode परिणाम = Curl_ssh_range(तथ्य, तथ्य->state.range, आकार,
                                       &from, &आकार);
      अगर(परिणाम)
        वापस परिणाम;

      libssh2_sftp_seek64(sshc->sftp_handle, (libssh2_uint64_t)from);
    }
    तथ्य->req.आकार = आकार;
    तथ्य->req.maxdownload = आकार;
    Curl_pgrs_Set_Download_आकार(तथ्य, आकार);
  }

  /* We can resume if we can seek to the resume position */
  अगर(तथ्य->state.resume_from) {
    अगर(तथ्य->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      अगर((curl_off_t)attrs.filesize < -तथ्य->state.resume_from) {
        failf(तथ्य, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")",
              तथ्य->state.resume_from, (curl_off_t)attrs.filesize);
        वापस CURLE_BAD_DOWNLOAD_RESUME;
      }
      /* download from where? */
      तथ्य->state.resume_from += attrs.filesize;
    }
    अन्यथा {
      अगर((curl_off_t)attrs.filesize < तथ्य->state.resume_from) {
        failf(तथ्य, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              तथ्य->state.resume_from, (curl_off_t)attrs.filesize);
        वापस CURLE_BAD_DOWNLOAD_RESUME;
      }
    }
    /* Now store the number of bytes we are expected to download */
    तथ्य->req.आकार = attrs.filesize - तथ्य->state.resume_from;
    तथ्य->req.maxdownload = attrs.filesize - तथ्य->state.resume_from;
    Curl_pgrs_Set_Download_आकार(तथ्य, attrs.filesize - तथ्य->state.resume_from);
    libssh2_sftp_seek64(sshc->sftp_handle,
                        (libssh2_uint64_t)तथ्य->state.resume_from);
  }

  /* Setup the actual download */
  अगर(तथ्य->req.आकार == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(तथ्य);
    infof(तथ्य, "SSH: file already completely downloaded");
    myssh_to(तथ्य, sshc, SSH_STOP);
    वापस CURLE_OK;
  }
  Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, तथ्य->req.आकार);

  /* not set by Curl_xfer_setup to preserve io_flags */
  तथ्य->conn->send_idx = 0;

  myssh_to(तथ्य, sshc, SSH_STOP);

  वापस CURLE_OK;
}

अटल CURLcode sftp_readdir(काष्ठा Curl_easy *तथ्य,
                             काष्ठा ssh_conn *sshc,
                             काष्ठा SSHPROTO *sshp,
                             bool *blockp)
{
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक rc = libssh2_sftp_readdir_ex(sshc->sftp_handle,
                                   sshp->readdir_filename, CURL_PATH_MAX,
                                   sshp->readdir_longentry, CURL_PATH_MAX,
                                   &sshp->readdir_attrs);
  अगर(rc == LIBSSH2_ERROR_EAGAIN) {
    *blockp = TRUE;
    वापस परिणाम;
  }
  अगर(rc > 0) {
    माप_प्रकार readdir_len = (माप_प्रकार)rc;
    sshp->readdir_filename[readdir_len] = '\0';

    अगर(तथ्य->set.list_only) {
      परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                                 sshp->readdir_filename,
                                 readdir_len);
      अगर(!परिणाम)
        परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY, "\n", 1);
      अगर(परिणाम)
        वापस परिणाम;
    }
    अन्यथा {
      परिणाम = curlx_dyn_add(&sshp->सूची_पढ़ो, sshp->readdir_longentry);

      अगर(!परिणाम) {
        अगर((sshp->readdir_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
           ((sshp->readdir_attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
            LIBSSH2_SFTP_S_IFLNK)) {
          परिणाम = curlx_dyn_addf(&sshp->readdir_link, "%s%s", sshp->path,
                                  sshp->readdir_filename);
          myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_LINK);
        }
        अन्यथा {
          myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_BOTTOM);
        }
      }
      वापस परिणाम;
    }
  }
  अन्यथा अगर(!rc) {
    myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_DONE);
  }
  अन्यथा {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    परिणाम = sftperr ? sftp_libssh2_error_to_CURLE(sftperr) : CURLE_SSH;
    failf(तथ्य, "Could not open remote file for reading: %s :: %d",
          sftp_libssh2_strerror(sftperr),
          libssh2_session_last_errno(sshc->ssh_session));
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
  }
  वापस परिणाम;
}

अटल CURLcode ssh_state_init(काष्ठा Curl_easy *तथ्य,
                               काष्ठा ssh_conn *sshc)
{
  CURLcode परिणाम;
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;

  /* Set libssh2 to non-blocking, since everything internally is
     non-blocking */
  libssh2_session_set_blocking(sshc->ssh_session, 0);

  परिणाम = ssh_force_knownhost_कुंजी_type(तथ्य, sshc);
  अगर(परिणाम)
    myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_S_STARTUP);
  वापस परिणाम;
}

अटल CURLcode ssh_state_startup(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ssh_conn *sshc)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  पूर्णांक rc = libssh2_session_handshake(sshc->ssh_session,
                                     conn->sock[FIRSTSOCKET]);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc) {
    अक्षर *err_संदेश = शून्य;
    (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
    failf(तथ्य, "Failure establishing ssh session: %d, %s", rc, err_संदेश);

    myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
    वापस CURLE_FAILED_INIT;
  }

  myssh_to(तथ्य, sshc, SSH_HOSTKEY);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_hostkey(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ssh_conn *sshc)
{
  /*
   * Before we authenticate we should check the hostkey's fingerprint
   * against our known hosts. How that is handled (reading from file,
   * whatever) is up to us.
   */
  CURLcode परिणाम = ssh_check_fingerprint(तथ्य, sshc);
  अगर(!परिणाम)
    myssh_to(तथ्य, sshc, SSH_AUTHLIST);
  वापस परिणाम;
}

अटल CURLcode ssh_state_authlist(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ssh_conn *sshc)
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
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर अक्षर *user = Curl_creds_user(conn->creds);
  sshc->authlist = libssh2_userauth_list(sshc->ssh_session,
                                         user, curlx_uztoui(म_माप(user)));

  अगर(!sshc->authlist) {
    पूर्णांक rc;
    अगर(libssh2_userauth_authenticated(sshc->ssh_session)) {
      sshc->authed = TRUE;
      infof(तथ्य, "SSH: user accepted with no authentication");
      myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
      वापस CURLE_OK;
    }
    rc = libssh2_session_last_errno(sshc->ssh_session);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
    वापस libssh2_session_error_to_CURLE(rc);
  }
  infof(तथ्य, "SSH: host offers authentication via: %s", sshc->authlist);

  myssh_to(तथ्य, sshc, SSH_AUTH_PKEY_INIT);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_auth_pkey(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc)
{
  /* The function below checks if the files exists, no need to stat() here.
   */
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर अक्षर *user = Curl_creds_user(conn->creds);
  पूर्णांक rc = libssh2_userauth_publickey_fromfile_ex(
    sshc->ssh_session, user, curlx_uztoui(म_माप(user)),
    sshc->pub_कुंजी, sshc->priv_कुंजी, sshc->passphrase ? sshc->passphrase : "");
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc == 0) {
    sshc->authed = TRUE;
    infof(तथ्य, "SSH: authenticated via publickey");
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
  }
  अन्यथा {
    अक्षर *err_संदेश = शून्य;
    अक्षर unknown[] = "Reason unknown (-1)";
    अगर(rc == -1) {
      /* No error message has been set and the last set error message, if
         any, is from a previous error so ignore it. #11837 */
      err_संदेश = unknown;
    }
    अन्यथा {
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
    }
    infof(तथ्य, "SSH: publickey authentication denied: %s", err_संदेश);
    myssh_to(तथ्य, sshc, SSH_AUTH_PASS_INIT);
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_auth_pass_init(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा ssh_conn *sshc)
{
  अगर((तथ्य->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD) &&
     म_माला(sshc->authlist, "password")) {
    myssh_to(तथ्य, sshc, SSH_AUTH_PASS);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_AUTH_HOST_INIT);
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_auth_pass(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर अक्षर *user = Curl_creds_user(conn->creds);
  स्थिर अक्षर *passwd = Curl_creds_passwd(conn->creds);
  पूर्णांक rc =
    libssh2_userauth_password_ex(sshc->ssh_session, user,
                                 curlx_uztoui(म_माप(user)),
                                 passwd,
                                 curlx_uztoui(म_माप(passwd)),
                                 शून्य);
  अगर(rc == LIBSSH2_ERROR_EAGAIN) {
    वापस CURLE_AGAIN;
  }
  अगर(rc == 0) {
    sshc->authed = TRUE;
    infof(तथ्य, "SSH: initialized password authentication");
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_AUTH_HOST_INIT);
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_auth_host_init(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा ssh_conn *sshc)
{
  अगर((तथ्य->set.ssh_auth_types & CURLSSH_AUTH_HOST) &&
     म_माला(sshc->authlist, "hostbased")) {
    myssh_to(तथ्य, sshc, SSH_AUTH_HOST);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_AUTH_AGENT_INIT);
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_auth_agent_init(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = 0;
  अगर((तथ्य->set.ssh_auth_types & CURLSSH_AUTH_AGENT) &&
     म_माला(sshc->authlist, "publickey")) {

    infof(तथ्य, "SSH: trying publickey authentication via agent");
    /* Connect to the ssh-agent */
    /* The agent could be shared by a curl thread i believe
       but nothing obvious as keys can be added/removed at any time */
    अगर(!sshc->ssh_agent) {
      sshc->ssh_agent = libssh2_agent_init(sshc->ssh_session);
      अगर(!sshc->ssh_agent) {
        infof(तथ्य, "SSH: could not create agent object");

        myssh_to(तथ्य, sshc, SSH_AUTH_कुंजी_INIT);
        वापस CURLE_OK;
      }
    }

    rc = libssh2_agent_connect(sshc->ssh_agent);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;
    अगर(rc < 0) {
      infof(तथ्य, "SSH: failure connecting to agent");
      myssh_to(तथ्य, sshc, SSH_AUTH_कुंजी_INIT);
    }
    अन्यथा {
      myssh_to(तथ्य, sshc, SSH_AUTH_AGENT_LIST);
    }
  }
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_AUTH_कुंजी_INIT);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_auth_agent_list(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = libssh2_agent_list_identities(sshc->ssh_agent);

  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;
  अगर(rc < 0) {
    infof(तथ्य, "SSH: failure requesting identities to agent");
    myssh_to(तथ्य, sshc, SSH_AUTH_कुंजी_INIT);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_AUTH_AGENT);
    sshc->sshagent_पूर्व_identity = शून्य;
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_auth_agent(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ssh_conn *sshc)
{
  /* as prev_identity evolves only after an identity user auth finished we
     can safely request it again as long as EAGAIN is returned here or by
     libssh2_agent_userauth */
  पूर्णांक rc = libssh2_agent_get_identity(sshc->ssh_agent,
                                      &sshc->sshagent_identity,
                                      sshc->sshagent_पूर्व_identity);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc == 0) {
    CURL_TRC_SSH(तथ्य, "[SSH_AUTH_AGENT_LIST] auth user '%s' for key '%s'",
                 Curl_creds_user(तथ्य->conn->creds),
                 sshc->sshagent_identity->comment);
    rc = libssh2_agent_userauth(sshc->ssh_agent,
                                Curl_creds_user(तथ्य->conn->creds),
                                sshc->sshagent_identity);

    अगर(rc < 0) {
      अगर(rc != LIBSSH2_ERROR_EAGAIN) {
        /* tried and failed? go to next identity */
        sshc->sshagent_पूर्व_identity = sshc->sshagent_identity;
        वापस CURLE_OK;
      }
      वापस CURLE_AGAIN;
    }
  }

  अगर(rc < 0)
    infof(तथ्य, "SSH: failure requesting identities to agent");
  अन्यथा अगर(rc == 1)
    infof(तथ्य, "SSH: no agent identity would match");

  अगर(rc == LIBSSH2_ERROR_NONE) {
    sshc->authed = TRUE;
    infof(तथ्य, "SSH: agent authenticated user '%s' with key '%s'",
          Curl_creds_user(तथ्य->conn->creds),
          sshc->sshagent_identity->comment);
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_AUTH_कुंजी_INIT);
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_auth_कुंजी_init(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ssh_conn *sshc)
{
  अगर((तथ्य->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) &&
     म_माला(sshc->authlist, "keyboard-interactive")) {
    myssh_to(तथ्य, sshc, SSH_AUTH_कुंजी);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_auth_कुंजी(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ssh_conn *sshc)
{
  /* Authentication failed. Continue with keyboard-interactive now. */
  काष्ठा connectdata *conn = तथ्य->conn;
  स्थिर अक्षर *user = Curl_creds_user(conn->creds);
  पूर्णांक rc =
    libssh2_userauth_keyboard_interactive_ex(sshc->ssh_session,
                                             user, curlx_uztoui(म_माप(user)),
                                             &kbd_callback);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc == 0) {
    sshc->authed = TRUE;
    infof(तथ्य, "SSH: initialized keyboard interactive authentication");
    myssh_to(तथ्य, sshc, SSH_AUTH_DONE);
    वापस CURLE_OK;
  }
  वापस CURLE_LOGIN_DENIED;
}

अटल CURLcode ssh_state_auth_done(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  अगर(!sshc->authed) {
    failf(तथ्य, "Authentication failure");
    myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
    वापस CURLE_LOGIN_DENIED;
  }

  /*
   * At this point we have an authenticated ssh session.
   */
  infof(तथ्य, "SSH: authentication complete");

  Curl_pgrsTime(तथ्य, TIMER_APPCONNECT); /* SSH is connected */

  तथ्य->conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  अगर(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(तथ्य, sshc, SSH_SFTP_INIT);
    वापस CURLE_OK;
  }
  infof(तथ्य, "SSH: connection established");
  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_init(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc)
{
  /*
   * Start the libssh2 sftp session
   */
  sshc->sftp_session = libssh2_sftp_init(sshc->ssh_session);
  अगर(!sshc->sftp_session) {
    अक्षर *err_संदेश = शून्य;
    अगर(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
    failf(तथ्य, "Failure initializing sftp session: %s", err_संदेश);
    myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
    वापस CURLE_FAILED_INIT;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_REALPATH);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_realpath(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ssh_conn *sshc,
                                        काष्ठा SSHPROTO *sshp)
{
  /*
   * Get the "home" directory
   */
  पूर्णांक rc;

  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session, ".",
                               curlx_uztoui(CURL_CSTRLEN(".")),
                               sshp->readdir_filename, CURL_PATH_MAX,
                               LIBSSH2_SFTP_REALPATH);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  myssh_to(तथ्य, sshc, SSH_STOP);
  अगर(rc > 0) {
    curlx_free(sshc->homedir);
    sshc->homedir = curlx_strdup(sshp->readdir_filename);
    अगर(!sshc->homedir)
      वापस CURLE_OUT_OF_MEMORY;
    curlx_free(तथ्य->state.most_recent_ftp_entrypath);
    तथ्य->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
    अगर(!तथ्य->state.most_recent_ftp_entrypath)
      वापस CURLE_OUT_OF_MEMORY;
  }
  अन्यथा {
    /* Return the error type */
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    CURLcode परिणाम;
    अगर(sftperr)
      परिणाम = sftp_libssh2_error_to_CURLE(sftperr);
    अन्यथा
      /* in this case, the error was not in the SFTP level but for example a
         time-out or similar */
      परिणाम = CURLE_SSH;
    CURL_TRC_SSH(तथ्य, "error = %lu makes libcurl = %d", sftperr, (पूर्णांक)परिणाम);
    वापस परिणाम;
  }

  /* This is the last step in the SFTP connect phase. Do note that while we
     get the homedir here, we get the "workingpath" in the DO action since the
     homedir remains the same between request but the working path does not. */
  CURL_TRC_SSH(तथ्य, "CONNECT phase done");
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_quote_init(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा ssh_conn *sshc,
                                          काष्ठा SSHPROTO *sshp)
{
  CURLcode परिणाम;

  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  परिणाम = Curl_getworkingpath(तथ्य, sshc->homedir, &sshp->path);
  अगर(परिणाम) {
    myssh_to(तथ्य, sshc, SSH_STOP);
    वापस परिणाम;
  }

  अगर(तथ्य->set.quote) {
    infof(तथ्य, "SSH: sending quote commands");
    sshc->quote_item = तथ्य->set.quote;
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_SFTP_GETINFO);
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_postquote_init(काष्ठा Curl_easy *तथ्य,
                                              काष्ठा ssh_conn *sshc)
{
  अगर(तथ्य->set.postquote) {
    infof(तथ्य, "SSH: sending quote commands");
    sshc->quote_item = तथ्य->set.postquote;
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    myssh_to(तथ्य, sshc, SSH_STOP);
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_quote(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ssh_conn *sshc,
                                     काष्ठा SSHPROTO *sshp)
{
  /* Send quote commands */
  CURLcode परिणाम;

  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  परिणाम = sftp_quote(तथ्य, sshc, sshp);
  अगर(परिणाम) {
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  वापस परिणाम;
}

अटल CURLcode ssh_state_sftp_अग्र_quote(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->अग्र;

  अगर(sshc->quote_item) {
    myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    अगर(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(तथ्य, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    अन्यथा {
      myssh_to(तथ्य, sshc, SSH_SFTP_GETINFO);
    }
  }
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_quote_stat(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा ssh_conn *sshc,
                                          काष्ठा SSHPROTO *sshp,
                                          bool *blockp)
{
  CURLcode परिणाम;

  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  परिणाम = sftp_quote_stat(तथ्य, sshc, sshp, blockp);
  अगर(परिणाम) {
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  वापस परिणाम;
}

अटल CURLcode ssh_state_sftp_quote_setstat(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा ssh_conn *sshc,
                                             काष्ठा SSHPROTO *sshp)
{
  पूर्णांक rc;

  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshc->quote_path2,
                            curlx_uztoui(म_माप(sshc->quote_path2)),
                            LIBSSH2_SFTP_SETSTAT,
                            &sshp->quote_attrs);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(तथ्य, "Attempt to set SFTP stats for \"%s\" failed: %s",
          sshc->quote_path2, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    वापस CURLE_QUOTE_ERROR;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_quote_symlink(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc =
    libssh2_sftp_symlink_ex(sshc->sftp_session, sshc->quote_path1,
                            curlx_uztoui(म_माप(sshc->quote_path1)),
                            sshc->quote_path2,
                            curlx_uztoui(म_माप(sshc->quote_path2)),
                            LIBSSH2_SFTP_SYMLINK);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(तथ्य, "symlink \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    वापस CURLE_QUOTE_ERROR;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_quote_mkdir(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(म_माप(sshc->quote_path1)),
                                 (दीर्घ)तथ्य->set.new_directory_perms);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(तथ्य, "mkdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    वापस CURLE_QUOTE_ERROR;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_quote_rename(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc =
    libssh2_sftp_rename_ex(sshc->sftp_session, sshc->quote_path1,
                           curlx_uztoui(म_माप(sshc->quote_path1)),
                           sshc->quote_path2,
                           curlx_uztoui(म_माप(sshc->quote_path2)),
                           LIBSSH2_SFTP_RENAME_OVERWRITE |
                           LIBSSH2_SFTP_RENAME_ATOMIC |
                           LIBSSH2_SFTP_RENAME_NATIVE);

  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(तथ्य, "rename \"%s\" to \"%s\" failed: %s",
          sshc->quote_path1, sshc->quote_path2,
          sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    वापस CURLE_QUOTE_ERROR;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_quote_rmdir(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = libssh2_sftp_rmdir_ex(sshc->sftp_session, sshc->quote_path1,
                                 curlx_uztoui(म_माप(sshc->quote_path1)));
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(तथ्य, "rmdir \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    वापस CURLE_QUOTE_ERROR;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_quote_unlink(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा ssh_conn *sshc)
{
  पूर्णांक rc = libssh2_sftp_unlink_ex(sshc->sftp_session, sshc->quote_path1,
                                  curlx_uztoui(म_माप(sshc->quote_path1)));
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(तथ्य, "rm \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    वापस CURLE_QUOTE_ERROR;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_quote_statvfs(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा ssh_conn *sshc)
{
  LIBSSH2_SFTP_STATVFS statvfs;
  पूर्णांक rc = libssh2_sftp_statvfs(sshc->sftp_session, sshc->quote_path1,
                                curlx_uztoui(म_माप(sshc->quote_path1)),
                                &statvfs);

  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc && !sshc->acceptfail) {
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(तथ्य, "statvfs \"%s\" failed: %s",
          sshc->quote_path1, sftp_libssh2_strerror(sftperr));
    curlx_safefree(sshc->quote_path1);
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    वापस CURLE_QUOTE_ERROR;
  }
  अन्यथा अगर(rc == 0) {
#अगर_घोषित _MSC_VER
#घोषणा CURL_LIBSSH2_VFS_आकार_MASK "I64u"
#अन्यथा
#घोषणा CURL_LIBSSH2_VFS_आकार_MASK "llu"
#पूर्ण_अगर
    CURLcode परिणाम;
    अक्षर *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_frsize: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_blocks: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_bfree: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_bavail: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_files: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_ffree: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_favail: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_fsid: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_flag: %" CURL_LIBSSH2_VFS_आकार_MASK "\n"
                              "f_namemax: %" CURL_LIBSSH2_VFS_आकार_MASK "\n",
                              statvfs.f_bsize, statvfs.f_frsize,
                              statvfs.f_blocks, statvfs.f_bfree,
                              statvfs.f_bavail, statvfs.f_files,
                              statvfs.f_ffree, statvfs.f_favail,
                              statvfs.f_fsid, statvfs.f_flag,
                              statvfs.f_namemax);
    अगर(!tmp) {
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      वापस CURLE_OUT_OF_MEMORY;
    }

    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_HEADER, tmp, म_माप(tmp));
    curlx_free(tmp);
    अगर(परिणाम) {
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      वापस परिणाम;
    }
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_अग्र_QUOTE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_create_dirs_mkdir(काष्ठा Curl_easy *तथ्य,
                                                 काष्ठा ssh_conn *sshc,
                                                 काष्ठा SSHPROTO *sshp)
{
  /* 'mode' - parameter is preliminary - default to 0644 */
  पूर्णांक rc;

  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  rc = libssh2_sftp_mkdir_ex(sshc->sftp_session, sshp->path,
                             curlx_uztoui(म_माप(sshp->path)),
                             (दीर्घ)तथ्य->set.new_directory_perms);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  अगर(rc < 0) {
    /*
     * Abort if failure was not that the directory already exists or
     * the permission was denied (creation might succeed further down
     * the path) - retry on unspecific FAILURE also
     */
    अचिन्हित दीर्घ sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    अगर((sftperr != LIBSSH2_FX_FILE_ALREADY_EXISTS) &&
       (sftperr != LIBSSH2_FX_FAILURE) &&
       (sftperr != LIBSSH2_FX_PERMISSION_DENIED)) {
      myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
      वापस sftp_libssh2_error_to_CURLE(sftperr);
    }
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_CREATE_DIRS);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_readdir_init(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा ssh_conn *sshc,
                                            काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  Curl_pgrs_Set_Download_आकार(तथ्य, -1);
  अगर(तथ्य->req.no_body) {
    myssh_to(तथ्य, sshc, SSH_STOP);
    वापस CURLE_OK;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(म_माप(sshp->path)),
                         0, 0, LIBSSH2_SFTP_OPENDIR);
  अगर(!sshc->sftp_handle) {
    अचिन्हित दीर्घ sftperr;
    अगर(libssh2_session_last_errno(sshc->ssh_session) == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(तथ्य, "Could not open directory for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    वापस sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_READDIR);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_readdir_link(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा ssh_conn *sshc,
                                            काष्ठा SSHPROTO *sshp)
{
  CURLcode परिणाम;
  पूर्णांक rc;
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  rc = libssh2_sftp_symlink_ex(sshc->sftp_session,
                               curlx_dyn_ptr(&sshp->readdir_link),
                               (अचिन्हित पूर्णांक)
                               curlx_dyn_len(&sshp->readdir_link),
                               sshp->readdir_filename,
                               CURL_PATH_MAX, LIBSSH2_SFTP_READLINK);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  curlx_dyn_free(&sshp->readdir_link);

  अगर(rc < 0)
    वापस CURLE_OUT_OF_MEMORY;

  /* append filename and extra output */
  परिणाम = curlx_dyn_addf(&sshp->सूची_पढ़ो, " -> %s", sshp->readdir_filename);
  अगर(परिणाम)
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_BOTTOM);
  वापस परिणाम;
}

अटल CURLcode ssh_state_scp_download_init(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा ssh_conn *sshc,
                                            काष्ठा SSHPROTO *sshp)
{
  curl_off_t bytecount;
  libssh2_struct_stat sb;

  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  /*
   * We must check the remote file; if it is a directory no values are
   * be set in sb
   */

  /*
   * If support for >2GB files exists, use it.
   */

  /* get a fresh new channel from the ssh layer */
  स_रखो(&sb, 0, माप(libssh2_struct_stat));
  sshc->ssh_channel = libssh2_scp_recv2(sshc->ssh_session, sshp->path, &sb);
  अगर(!sshc->ssh_channel) {
    पूर्णांक ssh_err;
    अक्षर *err_संदेश = शून्य;

    अगर(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
    failf(तथ्य, "%s", err_संदेश);
    myssh_to(तथ्य, sshc, SSH_SCP_CHANNEL_FREE);
    वापस libssh2_session_error_to_CURLE(ssh_err);
  }

  /* download data */
  bytecount = (curl_off_t)sb.st_आकार;
  तथ्य->req.maxdownload = (curl_off_t)sb.st_आकार;
  Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, bytecount);

  /* not set by Curl_xfer_setup to preserve io_flags */
  तथ्य->conn->send_idx = 0;

  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_close(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा ssh_conn *sshc,
                                     काष्ठा SSHPROTO *sshp)
{
  पूर्णांक rc = 0;
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  अगर(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    अगर(rc < 0) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to close libssh2 file: %d %s", rc, err_संदेश);
    }
    sshc->sftp_handle = शून्य;
  }

  curlx_safefree(sshp->path);

  CURL_TRC_SSH(तथ्य, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  अगर(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(तथ्य, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_STOP);

  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_shutdown(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  पूर्णांक rc = 0;
  अगर(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    अगर(rc < 0) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to close libssh2 file: %d %s", rc, err_संदेश);
    }
    sshc->sftp_handle = शून्य;
  }
  अगर(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    अगर(rc < 0) {
      infof(तथ्य, "Failed to stop libssh2 sftp subsystem");
    }
    sshc->sftp_session = शून्य;
  }

  curlx_safefree(sshc->homedir);

  myssh_to(तथ्य, sshc, SSH_SESSION_DISCONNECT);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_download_init(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा ssh_conn *sshc,
                                             काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  /*
   * Work on getting the specified file
   */
  sshc->sftp_handle =
    libssh2_sftp_open_ex(sshc->sftp_session, sshp->path,
                         curlx_uztoui(म_माप(sshp->path)),
                         LIBSSH2_FXF_READ, (दीर्घ)तथ्य->set.new_file_perms,
                         LIBSSH2_SFTP_OPENFILE);
  अगर(!sshc->sftp_handle) {
    अचिन्हित दीर्घ sftperr;
    अगर(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN) {
      वापस CURLE_AGAIN;
    }
    sftperr = libssh2_sftp_last_error(sshc->sftp_session);
    failf(तथ्य, "Could not open remote file for reading: %s",
          sftp_libssh2_strerror(sftperr));
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    वापस sftp_libssh2_error_to_CURLE(sftperr);
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_DOWNLOAD_STAT);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_scp_upload_init(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा ssh_conn *sshc,
                                          काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  /*
   * libssh2 requires that the destination path is a full path that
   * includes the destination file and name OR ends in a "/" . If this is
   * not done the destination file is named the same name as the last
   * directory in the path.
   */
  sshc->ssh_channel =
    libssh2_scp_send64(sshc->ssh_session, sshp->path,
                       (पूर्णांक)तथ्य->set.new_file_perms,
                       (libssh2_int64_t)तथ्य->state.infilesize, 0, 0);
  अगर(!sshc->ssh_channel) {
    पूर्णांक ssh_err;
    अक्षर *err_संदेश = शून्य;
    CURLcode परिणाम;
    अगर(libssh2_session_last_errno(sshc->ssh_session) ==
       LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    ssh_err = libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
    failf(तथ्य, "%s", err_संदेश);
    myssh_to(तथ्य, sshc, SSH_SCP_CHANNEL_FREE);
    परिणाम = libssh2_session_error_to_CURLE(ssh_err);

    /* Map generic errors to upload failed */
    अगर(परिणाम == CURLE_SSH ||
       परिणाम == CURLE_REMOTE_FILE_NOT_FOUND)
      परिणाम = CURLE_UPLOAD_FAILED;
    वापस परिणाम;
  }

  /* upload data */
  तथ्य->req.आकार = तथ्य->state.infilesize;
  Curl_pgrs_Set_Upload_आकार(तथ्य, तथ्य->state.infilesize);
  Curl_xfer_setup_send(तथ्य, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  तथ्य->conn->recv_idx = FIRSTSOCKET;

  myssh_to(तथ्य, sshc, SSH_STOP);

  वापस CURLE_OK;
}

अटल CURLcode ssh_state_session_disconnect(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  पूर्णांक rc = 0;
  अगर(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    अगर(rc < 0) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to free libssh2 scp subsystem: %d %s", rc, err_संदेश);
    }
    sshc->ssh_channel = शून्य;
  }

  अगर(sshc->ssh_session) {
    rc = libssh2_session_disconnect(sshc->ssh_session, "Shutdown");
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;

    अगर(rc < 0) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to disconnect libssh2 session: %d %s", rc, err_संदेश);
    }
  }

  curlx_safefree(sshc->homedir);

  myssh_to(तथ्य, sshc, SSH_SESSION_FREE);
  वापस CURLE_OK;
}

अटल CURLcode sshc_cleanup(काष्ठा ssh_conn *sshc, काष्ठा Curl_easy *तथ्य,
                             bool block)
{
  पूर्णांक rc;

  अगर(sshc->kh) {
    libssh2_knownhost_free(sshc->kh);
    sshc->kh = शून्य;
  }

  अगर(sshc->ssh_agent) {
    rc = libssh2_agent_disconnect(sshc->ssh_agent);
    अगर((rc < 0) && तथ्य) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to disconnect from libssh2 agent: %d %s",
            rc, err_संदेश);
    }
    अगर(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      वापस CURLE_AGAIN;

    libssh2_agent_free(sshc->ssh_agent);
    sshc->ssh_agent = शून्य;

    /* NB: there is no need to free identities, they are part of internal
       agent stuff */
    sshc->sshagent_identity = शून्य;
    sshc->sshagent_पूर्व_identity = शून्य;
  }

  अगर(sshc->sftp_handle) {
    rc = libssh2_sftp_close(sshc->sftp_handle);
    अगर((rc < 0) && तथ्य) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to close libssh2 file: %d %s", rc, err_संदेश);
    }
    अगर(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      वापस CURLE_AGAIN;

    sshc->sftp_handle = शून्य;
  }

  अगर(sshc->ssh_channel) {
    rc = libssh2_channel_free(sshc->ssh_channel);
    अगर((rc < 0) && तथ्य) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to free libssh2 scp subsystem: %d %s", rc, err_संदेश);
    }
    अगर(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      वापस CURLE_AGAIN;

    sshc->ssh_channel = शून्य;
  }

  अगर(sshc->sftp_session) {
    rc = libssh2_sftp_shutdown(sshc->sftp_session);
    अगर((rc < 0) && तथ्य) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to stop libssh2 sftp subsystem: %d %s", rc, err_संदेश);
    }
    अगर(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      वापस CURLE_AGAIN;

    sshc->sftp_session = शून्य;
  }

  अगर(sshc->ssh_session) {
    rc = libssh2_session_free(sshc->ssh_session);
    अगर((rc < 0) && तथ्य) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session, &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to free libssh2 session: %d %s", rc, err_संदेश);
    }
    अगर(!block && (rc == LIBSSH2_ERROR_EAGAIN))
      वापस CURLE_AGAIN;

    sshc->ssh_session = शून्य;
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

  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_getinfo(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc)
{
  अगर(तथ्य->set.get_filetime)
    myssh_to(तथ्य, sshc, SSH_SFTP_FILETIME);
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_SFTP_TRANS_INIT);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_filetime(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ssh_conn *sshc,
                                        काष्ठा SSHPROTO *sshp)
{
  LIBSSH2_SFTP_ATTRIBUTES attrs;
  पूर्णांक rc;

  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  rc = libssh2_sftp_stat_ex(sshc->sftp_session, sshp->path,
                            curlx_uztoui(म_माप(sshp->path)),
                            LIBSSH2_SFTP_STAT, &attrs);
  अगर(rc == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  अगर(rc == 0)
    तथ्य->info.filetime = (समय_प्रकार)attrs.mtime;

  myssh_to(तथ्य, sshc, SSH_SFTP_TRANS_INIT);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_trans_init(काष्ठा Curl_easy *तथ्य,
                                          काष्ठा ssh_conn *sshc,
                                          काष्ठा SSHPROTO *sshp)
{
  अगर(तथ्य->state.upload)
    myssh_to(तथ्य, sshc, SSH_SFTP_UPLOAD_INIT);
  अन्यथा अगर(sshp) {
    माप_प्रकार plen = म_माप(sshp->path);
    अगर(plen && (sshp->path[plen - 1] == '/'))
      myssh_to(तथ्य, sshc, SSH_SFTP_READDIR_INIT);
    अन्यथा
      myssh_to(तथ्य, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }
  अन्यथा
    वापस CURLE_FAILED_INIT;
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_upload_init(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा ssh_conn *sshc,
                                           काष्ठा SSHPROTO *sshp,
                                           bool *block)
{
  CURLcode परिणाम;
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  परिणाम = sftp_upload_init(तथ्य, sshc, sshp, block);
  अगर(परिणाम) {
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  वापस परिणाम;
}

अटल CURLcode ssh_state_sftp_create_dirs_init(काष्ठा Curl_easy *तथ्य,
                                                काष्ठा ssh_conn *sshc,
                                                काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  अगर(म_माप(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(तथ्य, sshc, SSH_SFTP_CREATE_DIRS);
  }
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_SFTP_UPLOAD_INIT);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_create_dirs(काष्ठा Curl_easy *तथ्य,
                                           काष्ठा ssh_conn *sshc,
                                           काष्ठा SSHPROTO *sshp)
{
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  sshc->slash_pos = म_अक्षर(sshc->slash_pos, '/');
  अगर(sshc->slash_pos) {
    *sshc->slash_pos = 0;
    infof(तथ्य, "SFTP: creating directory '%s'", sshp->path);
    myssh_to(तथ्य, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    वापस CURLE_OK;
  }
  myssh_to(तथ्य, sshc, SSH_SFTP_UPLOAD_INIT);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_readdir(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc,
                                       काष्ठा SSHPROTO *sshp,
                                       bool *block)
{
  CURLcode परिणाम;
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  परिणाम = sftp_readdir(तथ्य, sshc, sshp, block);
  अगर(परिणाम)
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
  वापस परिणाम;
}

अटल CURLcode ssh_state_sftp_readdir_bottom(काष्ठा Curl_easy *तथ्य,
                                              काष्ठा ssh_conn *sshc,
                                              काष्ठा SSHPROTO *sshp)
{
  CURLcode परिणाम;
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;

  परिणाम = curlx_dyn_addn(&sshp->सूची_पढ़ो, "\n", 1);
  अगर(!परिणाम)
    परिणाम = Curl_client_write(तथ्य, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshp->सूची_पढ़ो),
                               curlx_dyn_len(&sshp->सूची_पढ़ो));
  अगर(परिणाम) {
    curlx_dyn_free(&sshp->सूची_पढ़ो);
    myssh_to(तथ्य, sshc, SSH_STOP);
  }
  अन्यथा {
    curlx_dyn_reset(&sshp->सूची_पढ़ो);
    myssh_to(तथ्य, sshc, SSH_SFTP_READDIR);
  }
  वापस परिणाम;
}

अटल CURLcode ssh_state_sftp_readdir_done(काष्ठा Curl_easy *तथ्य,
                                            काष्ठा ssh_conn *sshc)
{
  अगर(libssh2_sftp_closedir(sshc->sftp_handle) ==
     LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;

  sshc->sftp_handle = शून्य;

  /* no data to transfer */
  Curl_xfer_setup_nop(तथ्य);
  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_sftp_download_stat(काष्ठा Curl_easy *तथ्य,
                                             काष्ठा ssh_conn *sshc,
                                             काष्ठा SSHPROTO *sshp,
                                             bool *block)
{
  CURLcode परिणाम;
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  परिणाम = sftp_download_stat(तथ्य, sshc, sshp, block);
  अगर(परिणाम) {
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
  }
  वापस परिणाम;
}

अटल CURLcode ssh_state_scp_trans_init(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा ssh_conn *sshc,
                                         काष्ठा SSHPROTO *sshp)
{
  CURLcode परिणाम;
  अगर(!sshp)
    वापस CURLE_FAILED_INIT;
  परिणाम = Curl_getworkingpath(तथ्य, sshc->homedir,
                               &sshp->path);
  अगर(परिणाम) {
    myssh_to(तथ्य, sshc, SSH_STOP);
    वापस परिणाम;
  }

  अगर(तथ्य->state.upload) {
    अगर(तथ्य->state.infilesize < 0) {
      failf(तथ्य, "SCP requires a known file size for upload");
      myssh_to(तथ्य, sshc, SSH_SCP_CHANNEL_FREE);
      वापस CURLE_UPLOAD_FAILED;
    }
    myssh_to(तथ्य, sshc, SSH_SCP_UPLOAD_INIT);
  }
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_SCP_DOWNLOAD_INIT);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_scp_done(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ssh_conn *sshc)
{
  अगर(तथ्य->state.upload)
    myssh_to(तथ्य, sshc, SSH_SCP_SEND_EOF);
  अन्यथा
    myssh_to(तथ्य, sshc, SSH_SCP_CHANNEL_FREE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_scp_send_eof(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc)
{
  अगर(sshc->ssh_channel) {
    पूर्णांक rc = libssh2_channel_send_eof(sshc->ssh_channel);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;
    अगर(rc) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session,
                                       &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to send libssh2 channel EOF: %d %s",
            rc, err_संदेश);
    }
  }
  myssh_to(तथ्य, sshc, SSH_SCP_WAIT_EOF);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_scp_wait_eof(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc)
{
  अगर(sshc->ssh_channel) {
    पूर्णांक rc = libssh2_channel_wait_eof(sshc->ssh_channel);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;
    अगर(rc) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session,
                                       &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to get channel EOF: %d %s", rc, err_संदेश);
    }
  }
  myssh_to(तथ्य, sshc, SSH_SCP_WAIT_CLOSE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_scp_wait_close(काष्ठा Curl_easy *तथ्य,
                                         काष्ठा ssh_conn *sshc)
{
  अगर(sshc->ssh_channel) {
    पूर्णांक rc =
      libssh2_channel_wait_closed(sshc->ssh_channel);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;
    अगर(rc) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session,
                                       &err_संदेश, शून्य, 0);
      infof(तथ्य, "Channel failed to close: %d %s", rc, err_संदेश);
    }
  }
  myssh_to(तथ्य, sshc, SSH_SCP_CHANNEL_FREE);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_scp_channel_free(
  काष्ठा Curl_easy *तथ्य,
  काष्ठा ssh_conn *sshc)
{
  अगर(sshc->ssh_channel) {
    पूर्णांक rc = libssh2_channel_free(sshc->ssh_channel);
    अगर(rc == LIBSSH2_ERROR_EAGAIN)
      वापस CURLE_AGAIN;
    अगर(rc < 0) {
      अक्षर *err_संदेश = शून्य;
      (व्योम)libssh2_session_last_error(sshc->ssh_session,
                                       &err_संदेश, शून्य, 0);
      infof(तथ्य, "Failed to free libssh2 scp subsystem: %d %s", rc, err_संदेश);
    }
    sshc->ssh_channel = शून्य;
  }
  CURL_TRC_SSH(तथ्य, "SCP DONE phase complete");
  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस CURLE_OK;
}

अटल CURLcode ssh_state_session_free(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा ssh_conn *sshc)
{
  CURLcode परिणाम = sshc_cleanup(sshc, तथ्य, FALSE);
  काष्ठा connectdata *conn = तथ्य->conn;
  अगर(परिणाम)
    वापस परिणाम;
  स_रखो(sshc, 0, माप(काष्ठा ssh_conn));
  connclose(conn);
  sshc->state = SSH_SESSION_FREE; /* current */
  myssh_to(तथ्य, sshc, SSH_STOP);
  वापस CURLE_OK;
}

/*
 * ssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh2 function returns LIBSSH2_ERROR_EAGAIN
 * meaning it wants to be called again when the socket is ready
 */
अटल CURLcode ssh_statemachine(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ssh_conn *sshc,
                                 काष्ठा SSHPROTO *sshp,
                                 bool *block)
{
  CURLcode परिणाम = CURLE_OK;
  *block = 0; /* we are not blocking by default */

  करो {
    चयन(sshc->state) {
    हाल SSH_INIT:
      परिणाम = ssh_state_init(तथ्य, sshc);
      अगर(परिणाम)
        अवरोध;
      FALLTHROUGH();

    हाल SSH_S_STARTUP:
      परिणाम = ssh_state_startup(तथ्य, sshc);
      अगर(परिणाम)
        अवरोध;
      FALLTHROUGH();

    हाल SSH_HOSTKEY:
      परिणाम = ssh_state_hostkey(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTHLIST:
      परिणाम = ssh_state_authlist(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_PKEY_INIT:
      ssh_state_pkey_init(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_PKEY:
      परिणाम = ssh_state_auth_pkey(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_PASS_INIT:
      परिणाम = ssh_state_auth_pass_init(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_PASS:
      परिणाम = ssh_state_auth_pass(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_HOST_INIT:
      परिणाम = ssh_state_auth_host_init(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_HOST:
      myssh_to(तथ्य, sshc, SSH_AUTH_AGENT_INIT);
      अवरोध;

    हाल SSH_AUTH_AGENT_INIT:
      परिणाम = ssh_state_auth_agent_init(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_AGENT_LIST:
      परिणाम = ssh_state_auth_agent_list(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_AGENT:
      परिणाम = ssh_state_auth_agent(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_कुंजी_INIT:
      परिणाम = ssh_state_auth_कुंजी_init(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_कुंजी:
      परिणाम = ssh_state_auth_कुंजी(तथ्य, sshc);
      अवरोध;

    हाल SSH_AUTH_DONE:
      परिणाम = ssh_state_auth_done(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_INIT:
      परिणाम = ssh_state_sftp_init(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_REALPATH:
      परिणाम = ssh_state_sftp_realpath(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_QUOTE_INIT:
      परिणाम = ssh_state_sftp_quote_init(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_POSTQUOTE_INIT:
      परिणाम = ssh_state_sftp_postquote_init(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_QUOTE:
      परिणाम = ssh_state_sftp_quote(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_अग्र_QUOTE:
      परिणाम = ssh_state_sftp_अग्र_quote(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_QUOTE_STAT:
      परिणाम = ssh_state_sftp_quote_stat(तथ्य, sshc, sshp, block);
      अवरोध;

    हाल SSH_SFTP_QUOTE_SETSTAT:
      परिणाम = ssh_state_sftp_quote_setstat(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_QUOTE_SYMLINK:
      परिणाम = ssh_state_sftp_quote_symlink(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_QUOTE_MKDIR:
      परिणाम = ssh_state_sftp_quote_mkdir(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_QUOTE_RENAME:
      परिणाम = ssh_state_sftp_quote_rename(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_QUOTE_RMDIR:
      परिणाम = ssh_state_sftp_quote_rmdir(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_QUOTE_UNLINK:
      परिणाम = ssh_state_sftp_quote_unlink(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_QUOTE_STATVFS:
      परिणाम = ssh_state_sftp_quote_statvfs(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_GETINFO:
      परिणाम = ssh_state_sftp_getinfo(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_FILETIME:
      परिणाम = ssh_state_sftp_filetime(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_TRANS_INIT:
      परिणाम = ssh_state_sftp_trans_init(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_UPLOAD_INIT:
      परिणाम = ssh_state_sftp_upload_init(तथ्य, sshc, sshp, block);
      अवरोध;

    हाल SSH_SFTP_CREATE_DIRS_INIT:
      परिणाम = ssh_state_sftp_create_dirs_init(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_CREATE_DIRS:
      परिणाम = ssh_state_sftp_create_dirs(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_CREATE_DIRS_MKDIR:
      परिणाम = ssh_state_sftp_create_dirs_mkdir(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_READDIR_INIT:
      परिणाम = ssh_state_sftp_readdir_init(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_READDIR:
      परिणाम = ssh_state_sftp_readdir(तथ्य, sshc, sshp, block);
      अवरोध;

    हाल SSH_SFTP_READDIR_LINK:
      परिणाम = ssh_state_sftp_readdir_link(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_READDIR_BOTTOM:
      परिणाम = ssh_state_sftp_readdir_bottom(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_READDIR_DONE:
      परिणाम = ssh_state_sftp_readdir_done(तथ्य, sshc);
      अवरोध;

    हाल SSH_SFTP_DOWNLOAD_INIT:
      परिणाम = ssh_state_sftp_download_init(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_DOWNLOAD_STAT:
      परिणाम = ssh_state_sftp_download_stat(तथ्य, sshc, sshp, block);
      अवरोध;

    हाल SSH_SFTP_CLOSE:
      परिणाम = ssh_state_sftp_close(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SFTP_SHUTDOWN:
      परिणाम = ssh_state_sftp_shutdown(तथ्य, sshc);
      अवरोध;

    हाल SSH_SCP_TRANS_INIT:
      परिणाम = ssh_state_scp_trans_init(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SCP_UPLOAD_INIT:
      परिणाम = ssh_state_scp_upload_init(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SCP_DOWNLOAD_INIT:
      परिणाम = ssh_state_scp_download_init(तथ्य, sshc, sshp);
      अवरोध;

    हाल SSH_SCP_DONE:
      परिणाम = ssh_state_scp_done(तथ्य, sshc);
      अवरोध;

    हाल SSH_SCP_SEND_EOF:
      परिणाम = ssh_state_scp_send_eof(तथ्य, sshc);
      अवरोध;

    हाल SSH_SCP_WAIT_EOF:
      परिणाम = ssh_state_scp_wait_eof(तथ्य, sshc);
      अवरोध;

    हाल SSH_SCP_WAIT_CLOSE:
      परिणाम = ssh_state_scp_wait_close(तथ्य, sshc);
      अवरोध;

    हाल SSH_SCP_CHANNEL_FREE:
      परिणाम = ssh_state_scp_channel_free(तथ्य, sshc);
      अवरोध;

    हाल SSH_SESSION_DISCONNECT:
      परिणाम = ssh_state_session_disconnect(तथ्य, sshc);
      अवरोध;

    हाल SSH_SESSION_FREE:
      परिणाम = ssh_state_session_free(तथ्य, sshc);
      अवरोध;

    हाल SSH_QUIT:
    शेष:
      /* internal error */
      myssh_to(तथ्य, sshc, SSH_STOP);
      अवरोध;
    }

  } जबतक(!परिणाम && (sshc->state != SSH_STOP) && !*block);

  अगर(परिणाम == CURLE_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
    परिणाम = CURLE_OK;
  }
  CURL_TRC_SSH(तथ्य, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (पूर्णांक)परिणाम, *block);

  वापस परिणाम;
}

/*
 * When one of the libssh2 functions has returned LIBSSH2_ERROR_EAGAIN this
 * function is used to figure out in what direction and stores this info so
 * that the multi interface can take advantage of it. Make sure to call this
 * function in all cases so that when it _does not_ return EAGAIN we can
 * restore the default wait bits.
 */
अटल व्योम ssh_block2waitfor(काष्ठा Curl_easy *तथ्य,
                              काष्ठा ssh_conn *sshc,
                              bool block)
{
  पूर्णांक dir = 0;
  (व्योम)तथ्य;
  अगर(block) {
    dir = libssh2_session_block_directions(sshc->ssh_session);
    अगर(dir) {
      /* translate the libssh2 define bits into our own bit defines */
      sshc->waitfor =
        ((dir & LIBSSH2_SESSION_BLOCK_INBOUND) ? REQ_IO_RECV : 0) |
        ((dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) ? REQ_IO_SEND : 0);
    }
  }
  अगर(!dir)
    sshc->waitfor = 0;
}

/* called repeatedly until done from multi.c */
अटल CURLcode ssh_multi_statemach(काष्ठा Curl_easy *तथ्य, bool *done)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  काष्ठा SSHPROTO *sshp = Curl_meta_get(तथ्य, CURL_META_SSH_EASY);
  CURLcode परिणाम = CURLE_OK;
  bool block; /* we store the status and use that to provide
                 a Curl_ssh_pollset() implementation */
  अगर(!sshc || !sshp)
    वापस CURLE_FAILED_INIT;

  करो {
    परिणाम = ssh_statemachine(तथ्य, sshc, sshp, &block);
    *done = (sshc->state == SSH_STOP);
    /* if there is no error, it is not done and it did not EWOULDBLOCK, then
       try again */
  } जबतक(!परिणाम && !*done && !block);
  ssh_block2waitfor(तथ्य, sshc, block);

  वापस परिणाम;
}

अटल CURLcode ssh_block_statemach(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ssh_conn *sshc,
                                    काष्ठा SSHPROTO *sshp,
                                    bool disconnect)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा curltime start = *Curl_pgrs_now(तथ्य);

  जबतक((sshc->state != SSH_STOP) && !परिणाम) {
    bool block;
    timediff_t बायाँ_ms = 1000;

    परिणाम = ssh_statemachine(तथ्य, sshc, sshp, &block);
    अगर(परिणाम)
      अवरोध;

    अगर(!disconnect) {
      परिणाम = Curl_pgrsCheck(तथ्य);
      अगर(परिणाम)
        अवरोध;

      बायाँ_ms = Curl_timeleft_ms(तथ्य);
      अगर(बायाँ_ms < 0) {
        failf(तथ्य, "Operation timed out");
        वापस CURLE_OPERATION_TIMEDOUT;
      }
    }
    अन्यथा अगर(curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &start) > 1000) {
      /* disconnect timeout */
      failf(तथ्य, "Disconnect timed out");
      परिणाम = CURLE_OK;
      अवरोध;
    }

    अगर(block) {
      पूर्णांक dir = libssh2_session_block_directions(sshc->ssh_session);
      curl_socket_t sock = तथ्य->conn->sock[FIRSTSOCKET];
      curl_socket_t fd_read = CURL_SOCKET_BAD;
      curl_socket_t fd_write = CURL_SOCKET_BAD;
      अगर(LIBSSH2_SESSION_BLOCK_INBOUND & dir)
        fd_read = sock;
      अगर(LIBSSH2_SESSION_BLOCK_OUTBOUND & dir)
        fd_write = sock;
      /* wait for the socket to become ready */
      (व्योम)Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write,
                              (!बायाँ_ms || बायाँ_ms > 1000) ? 1000 : बायाँ_ms);
    }
  }

  वापस परिणाम;
}

अटल व्योम myssh_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा SSHPROTO *sshp = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curlx_safefree(sshp->path);
  curlx_dyn_free(&sshp->सूची_पढ़ो);
  curlx_dyn_free(&sshp->readdir_link);
  curlx_free(sshp);
}

अटल व्योम myssh_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा ssh_conn *sshc = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  sshc_cleanup(sshc, शून्य, TRUE);
  curlx_free(sshc);
}

/*
 * SSH setup and connection
 */
अटल CURLcode ssh_setup_connection(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा connectdata *conn)
{
  काष्ठा ssh_conn *sshc;
  काष्ठा SSHPROTO *sshp;
  (व्योम)conn;

  sshc = curlx_calloc(1, माप(*sshc));
  अगर(!sshc)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, माप(*sshp));
  अगर(!sshp)
    वापस CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshp->सूची_पढ़ो, CURL_PATH_MAX * 2);
  curlx_dyn_init(&sshp->readdir_link, CURL_PATH_MAX);
  अगर(Curl_meta_set(तथ्य, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    वापस CURLE_OUT_OF_MEMORY;

  वापस Curl_ssh_setup_pkey(तथ्य, sshc);
}

अटल Curl_recv scp_recv, sftp_recv;
अटल Curl_send scp_send, sftp_send;

#अगर_अघोषित CURL_DISABLE_PROXY
अटल ssize_t ssh_tls_recv(libssh2_socket_t sock, व्योम *बफर,
                            माप_प्रकार लंबाई, पूर्णांक flags, व्योम **abstract)
{
  काष्ठा Curl_easy *तथ्य = (काष्ठा Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(तथ्य, sock);
  माप_प्रकार nread;
  CURLcode परिणाम;
  काष्ठा connectdata *conn = तथ्य->conn;
  Curl_recv *backup = conn->recv[sockindex];
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (व्योम)flags;

  अगर(!sshc)
    वापस -1;

  /* swap in the TLS reader function for this call only, and then swap back
     the SSH one again */
  conn->recv[sockindex] = sshc->tls_recv;
  परिणाम = Curl_conn_recv(तथ्य, sockindex, बफर, लंबाई, &nread);
  conn->recv[sockindex] = backup;
  अगर(परिणाम == CURLE_AGAIN)
    वापस -EAGAIN; /* magic return code for libssh2 */
  अन्यथा अगर(परिणाम)
    वापस -1; /* generic error */
  Curl_debug(तथ्य, CURLINFO_तथ्य_IN, (स्थिर अक्षर *)बफर, nread);
  वापस (ssize_t)nread;
}

अटल ssize_t ssh_tls_send(libssh2_socket_t sock, स्थिर व्योम *बफर,
                            माप_प्रकार लंबाई, पूर्णांक flags, व्योम **abstract)
{
  काष्ठा Curl_easy *तथ्य = (काष्ठा Curl_easy *)*abstract;
  int8_t sockindex = Curl_conn_sockindex(तथ्य, sock);
  माप_प्रकार nwrite;
  CURLcode परिणाम;
  काष्ठा connectdata *conn = तथ्य->conn;
  Curl_send *backup = conn->send[sockindex];
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  (व्योम)flags;

  अगर(!sshc)
    वापस -1;

  /* swap in the TLS writer function for this call only, and then swap back
     the SSH one again */
  conn->send[sockindex] = sshc->tls_send;
  परिणाम = Curl_conn_send(तथ्य, sockindex, बफर, लंबाई, FALSE, &nwrite);
  conn->send[sockindex] = backup;
  अगर(परिणाम == CURLE_AGAIN)
    वापस -EAGAIN; /* magic return code for libssh2 */
  अन्यथा अगर(परिणाम)
    वापस -1; /* error */
  Curl_debug(तथ्य, CURLINFO_तथ्य_OUT, (स्थिर अक्षर *)बफर, nwrite);
  वापस (ssize_t)nwrite;
}
#पूर्ण_अगर

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
अटल CURLcode ssh_connect(काष्ठा Curl_easy *तथ्य, bool *done)
{
#अगर_घोषित CURL_LIBSSH2_DEBUG
  curl_socket_t sock;
#पूर्ण_अगर
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode परिणाम;

#अगर LIBSSH2_VERSION_अंक >= 0x010b00
  {
    स्थिर अक्षर *crypto_str;
    चयन(libssh2_crypto_engine()) {
    हाल libssh2_gcrypt:
      crypto_str = "libgcrypt";
      अवरोध;
    हाल libssh2_mbedtls:
      crypto_str = "mbedTLS";
      अवरोध;
    हाल libssh2_openssl:
      crypto_str = "openssl compatible";
      अवरोध;
    हाल libssh2_os400qc3:
      crypto_str = "OS400QC3";
      अवरोध;
    हाल libssh2_wincng:
      crypto_str = "WinCNG";
      अवरोध;
    शेष:
      crypto_str = शून्य;
      अवरोध;
    }
    अगर(crypto_str)
      infof(तथ्य, "SSH: libssh2 cryptography backend: %s", crypto_str);
  }
#पूर्ण_अगर

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  infof(तथ्य, "SSH: user '%s'", Curl_creds_user(conn->creds));
#अगर_घोषित CURL_LIBSSH2_DEBUG
  infof(तथ्य, "SSH: password %s", Curl_creds_passwd(conn->creds));
  sock = conn->sock[FIRSTSOCKET];
#पूर्ण_अगर /* CURL_LIBSSH2_DEBUG */

  /* libcurl MUST set custom memory functions so that the kbd_callback
     function's memory allocations can be properly freed */
  sshc->ssh_session = libssh2_session_init_ex(my_libssh2_malloc,
                                              my_libssh2_free,
                                              my_libssh2_realloc, तथ्य);

  अगर(!sshc->ssh_session) {
    failf(तथ्य, "Failure initializing ssh session");
    वापस CURLE_FAILED_INIT;
  }

  /* Set the packet read timeout if the libssh2 version supports it */
#अगर LIBSSH2_VERSION_अंक >= 0x010B00
  अगर(तथ्य->set.server_response_timeout > 0) {
    libssh2_session_set_read_timeout(sshc->ssh_session,
                             (दीर्घ)(तथ्य->set.server_response_timeout / 1000));
  }
#पूर्ण_अगर

#अगर_अघोषित CURL_DISABLE_PROXY
  अगर(conn->http_proxy.proxytype == CURLPROXY_HTTPS) {
    /* Setup libssh2 callbacks to make it read/write TLS from the socket.

       ssize_t
       recvcb(libssh2_socket_t sock, void *buffer, size_t length,
       int flags, void **abstract);

       ssize_t
       sendcb(libssh2_socket_t sock, const void *buffer, size_t length,
       int flags, void **abstract);
     */
#अगर LIBSSH2_VERSION_अंक >= 0x010b01
    infof(तथ्य, "SSH: using HTTPS proxy");
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wcast-function-type-strict"
#पूर्ण_अगर
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_RECV,
                                  (libssh2_cb_generic *)ssh_tls_recv);
    libssh2_session_callback_set2(sshc->ssh_session,
                                  LIBSSH2_CALLBACK_SEND,
                                  (libssh2_cb_generic *)ssh_tls_send);
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic निकालो
#पूर्ण_अगर
#अन्यथा
    /*
     * This crazy union dance is here to avoid assigning a void pointer a
     * function pointer as it is invalid C. The problem is of course that
     * libssh2 has such an API...
     */
    जोड़ receive {
      व्योम *recvp;
      ssize_t (*recvptr)(libssh2_socket_t, व्योम *, माप_प्रकार, पूर्णांक, व्योम **);
    };
    जोड़ transfer {
      व्योम *sendp;
      ssize_t (*sendptr)(libssh2_socket_t, स्थिर व्योम *, माप_प्रकार, पूर्णांक, व्योम **);
    };
    जोड़ receive sshrecv;
    जोड़ transfer sshsend;

    sshrecv.recvptr = ssh_tls_recv;
    sshsend.sendptr = ssh_tls_send;

    infof(तथ्य, "SSH: using HTTPS proxy");
    libssh2_session_callback_set(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_RECV, sshrecv.recvp);
    libssh2_session_callback_set(sshc->ssh_session,
                                 LIBSSH2_CALLBACK_SEND, sshsend.sendp);
#पूर्ण_अगर

    /* Store the underlying TLS recv/send function pointers to be used when
       reading from the proxy */
    sshc->tls_recv = conn->recv[FIRSTSOCKET];
    sshc->tls_send = conn->send[FIRSTSOCKET];
  }

#पूर्ण_अगर /* CURL_DISABLE_PROXY */
  अगर(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  अन्यथा {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  अगर(तथ्य->set.ssh_compression &&
     libssh2_session_flag(sshc->ssh_session, LIBSSH2_FLAG_COMPRESS, 1) < 0) {
    infof(तथ्य, "SSH: failed to enable compression for session");
  }

  अगर(CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS)) {
    पूर्णांक rc;
    sshc->kh = libssh2_knownhost_init(sshc->ssh_session);
    अगर(!sshc->kh) {
      libssh2_session_free(sshc->ssh_session);
      sshc->ssh_session = शून्य;
      वापस CURLE_FAILED_INIT;
    }

    /* read all known hosts from there */
    rc = libssh2_knownhost_readfile(
      sshc->kh, CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS),
      LIBSSH2_KNOWNHOST_FILE_OPENSSH);
    अगर(rc < 0)
      infof(तथ्य, "SSH: failed to read known hosts from %s",
            CURL_EASY_STR(तथ्य, STRING_SSH_KNOWNHOSTS));
  }

#अगर_घोषित CURL_LIBSSH2_DEBUG
  libssh2_trace(sshc->ssh_session, ~0);
  infof(तथ्य, "SSH: socket %d", (पूर्णांक)sock);
#पूर्ण_अगर

  myssh_to(तथ्य, sshc, SSH_INIT);

  परिणाम = ssh_multi_statemach(तथ्य, done);

  वापस परिणाम;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

अटल CURLcode scp_perform(काष्ठा Curl_easy *तथ्य,
                            bool *connected,
                            bool *dophase_done)
{
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(तथ्य->conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_SSH(तथ्य, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(तथ्य, sshc, SSH_SCP_TRANS_INIT);

  /* run the state-machine */
  परिणाम = ssh_multi_statemach(तथ्य, dophase_done);

  *connected = Curl_conn_is_connected(तथ्य->conn, FIRSTSOCKET);

  अगर(*dophase_done) {
    CURL_TRC_SSH(तथ्य, "DO phase is complete");
  }

  वापस परिणाम;
}

/* called from multi.c while DOing */
अटल CURLcode scp_doing(काष्ठा Curl_easy *तथ्य,
                          bool *dophase_done)
{
  CURLcode परिणाम;
  परिणाम = ssh_multi_statemach(तथ्य, dophase_done);

  अगर(*dophase_done) {
    CURL_TRC_SSH(तथ्य, "DO phase is complete");
  }
  वापस परिणाम;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
अटल CURLcode scp_disconnect(काष्ठा Curl_easy *तथ्य,
                               काष्ठा connectdata *conn,
                               bool dead_connection)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  काष्ठा SSHPROTO *sshp = Curl_meta_get(तथ्य, CURL_META_SSH_EASY);
  (व्योम)dead_connection;

  अगर(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(तथ्य, sshc, SSH_SESSION_DISCONNECT);
    परिणाम = ssh_block_statemach(तथ्य, sshc, sshp, TRUE);
  }

  अगर(sshc)
    वापस sshc_cleanup(sshc, तथ्य, TRUE);
  वापस परिणाम;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
अटल CURLcode ssh_done(काष्ठा Curl_easy *तथ्य, CURLcode status)
{
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(तथ्य->conn, CURL_META_SSH_CONN);
  काष्ठा SSHPROTO *sshp = Curl_meta_get(तथ्य, CURL_META_SSH_EASY);
  CURLcode परिणाम = CURLE_OK;

  अगर(!sshc || !sshp)
    वापस CURLE_FAILED_INIT;

  अगर(!status)
    /* run the state-machine */
    परिणाम = ssh_block_statemach(तथ्य, sshc, sshp, FALSE);
  अन्यथा
    परिणाम = status;

  अगर(Curl_pgrsDone(तथ्य, Curl_pgrs_now(तथ्य)))
    वापस CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(तथ्य);
  वापस परिणाम;
}

अटल CURLcode scp_done(काष्ठा Curl_easy *तथ्य, CURLcode status,
                         bool premature)
{
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(तथ्य->conn, CURL_META_SSH_CONN);
  (व्योम)premature;

  अगर(sshc && !status)
    myssh_to(तथ्य, sshc, SSH_SCP_DONE);

  वापस ssh_done(तथ्य, status);
}

अटल CURLcode scp_send(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                         स्थिर uint8_t *mem, माप_प्रकार len, bool eos,
                         माप_प्रकार *pnwritten)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;
  ssize_t nwritten;

  (व्योम)sockindex; /* we only support SCP on the fixed known primary socket */
  (व्योम)eos;
  *pnwritten = 0;

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  /* libssh2_channel_write() returns int! */
  nwritten = (ssize_t)libssh2_channel_write(sshc->ssh_channel,
                                            (स्थिर अक्षर *)mem, len);

  ssh_block2waitfor(तथ्य, sshc, (nwritten == LIBSSH2_ERROR_EAGAIN));

  अगर(nwritten == LIBSSH2_ERROR_EAGAIN)
    परिणाम = CURLE_AGAIN;
  अन्यथा अगर(nwritten < LIBSSH2_ERROR_NONE)
    परिणाम = libssh2_session_error_to_CURLE((पूर्णांक)nwritten);
  अन्यथा
    *pnwritten = (माप_प्रकार)nwritten;

  वापस परिणाम;
}

अटल CURLcode scp_recv(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                         अक्षर *mem, माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;
  ssize_t nread;

  (व्योम)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  /* libssh2_channel_read() returns int */
  nread = (ssize_t)libssh2_channel_read(sshc->ssh_channel, mem, len);

  ssh_block2waitfor(तथ्य, sshc, (nread == LIBSSH2_ERROR_EAGAIN));
  अगर(nread == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;
  अन्यथा अगर(nread < LIBSSH2_ERROR_NONE)
    परिणाम = libssh2_session_error_to_CURLE((पूर्णांक)nread);
  अन्यथा
    *pnread = (माप_प्रकार)nread;

  वापस परिणाम;
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

अटल CURLcode sftp_perform(काष्ठा Curl_easy *तथ्य,
                             bool *connected,
                             bool *dophase_done)
{
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(तथ्य->conn, CURL_META_SSH_CONN);
  CURLcode परिणाम = CURLE_OK;

  CURL_TRC_SSH(तथ्य, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(तथ्य, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  परिणाम = ssh_multi_statemach(तथ्य, dophase_done);

  *connected = Curl_conn_is_connected(तथ्य->conn, FIRSTSOCKET);

  अगर(*dophase_done) {
    CURL_TRC_SSH(तथ्य, "DO phase is complete");
  }

  वापस परिणाम;
}

/* called from multi.c while DOing */
अटल CURLcode sftp_doing(काष्ठा Curl_easy *तथ्य,
                           bool *dophase_done)
{
  CURLcode परिणाम = ssh_multi_statemach(तथ्य, dophase_done);

  अगर(*dophase_done) {
    CURL_TRC_SSH(तथ्य, "DO phase is complete");
  }
  वापस परिणाम;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
अटल CURLcode sftp_disconnect(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn, bool dead_connection)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  काष्ठा SSHPROTO *sshp = Curl_meta_get(तथ्य, CURL_META_SSH_EASY);
  (व्योम)dead_connection;

  अगर(sshc) {
    अगर(sshc->ssh_session) {
      /* only if there is a session still around to use! */
      CURL_TRC_SSH(तथ्य, "DISCONNECT starts now");
      myssh_to(तथ्य, sshc, SSH_SFTP_SHUTDOWN);
      परिणाम = ssh_block_statemach(तथ्य, sshc, sshp, TRUE);
      CURL_TRC_SSH(तथ्य, "DISCONNECT is done -> %d", (पूर्णांक)परिणाम);
    }
    sshc_cleanup(sshc, तथ्य, TRUE);
  }
  वापस परिणाम;
}

अटल CURLcode sftp_done(काष्ठा Curl_easy *तथ्य, CURLcode status,
                          bool premature)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  अगर(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    अगर(!premature && तथ्य->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(तथ्य, sshc, SSH_SFTP_CLOSE);
  }
  वापस ssh_done(तथ्य, status);
}

/* return number of sent bytes */
अटल CURLcode sftp_send(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                          स्थिर uint8_t *mem, माप_प्रकार len, bool eos,
                          माप_प्रकार *pnwritten)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (व्योम)sockindex;
  (व्योम)eos;
  *pnwritten = 0;

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  nwrite = libssh2_sftp_write(sshc->sftp_handle, (स्थिर अक्षर *)mem, len);

  ssh_block2waitfor(तथ्य, sshc, (nwrite == LIBSSH2_ERROR_EAGAIN));

  अगर(nwrite == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;
  अन्यथा अगर(nwrite < LIBSSH2_ERROR_NONE)
    वापस libssh2_session_error_to_CURLE((पूर्णांक)nwrite);
  *pnwritten = (माप_प्रकार)nwrite;
  वापस CURLE_OK;
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
अटल CURLcode sftp_recv(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                          अक्षर *mem, माप_प्रकार len, माप_प्रकार *pnread)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (व्योम)sockindex;
  *pnread = 0;

  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  nread = libssh2_sftp_read(sshc->sftp_handle, mem, len);

  ssh_block2waitfor(तथ्य, sshc, (nread == LIBSSH2_ERROR_EAGAIN));

  अगर(nread == LIBSSH2_ERROR_EAGAIN)
    वापस CURLE_AGAIN;
  अन्यथा अगर(nread < 0)
    वापस libssh2_session_error_to_CURLE((पूर्णांक)nread);

  *pnread = (माप_प्रकार)nread;
  वापस CURLE_OK;
}

/*
 * The DO function is generic for both protocols. There was previously two
 * separate ones but this way means less duplicated code.
 */
अटल CURLcode ssh_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम;
  bool connected = FALSE;
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE; /* default to false */
  अगर(!sshc)
    वापस CURLE_FAILED_INIT;

  तथ्य->req.आकार = -1; /* make sure this is unknown at this point */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(तथ्य);

  अगर(conn->scheme->protocol & CURLPROTO_SCP)
    परिणाम = scp_perform(तथ्य, &connected, done);
  अन्यथा
    परिणाम = sftp_perform(तथ्य, &connected, done);

  वापस परिणाम;
}

CURLcode Curl_ssh_init(व्योम)
{
  अगर(libssh2_init(0)) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: libssh2_init failed\n"));
    वापस CURLE_FAILED_INIT;
  }
  वापस CURLE_OK;
}

व्योम Curl_ssh_cleanup(व्योम)
{
  libssh2_exit();
}

व्योम Curl_ssh_version(अक्षर *बफर, माप_प्रकार buflen)
{
  (व्योम)curl_msnprintf(बफर, buflen, "libssh2/%s", libssh2_version(0));
}

/* The SSH session is associated with the *CONNECTION* but the callback user
 * pointer is an easy handle pointer. This function allows us to reassign the
 * user pointer to the *CURRENT* (new) easy handle.
 */
अटल व्योम ssh_attach(काष्ठा Curl_easy *तथ्य, काष्ठा connectdata *conn)
{
  DEBUGASSERT(तथ्य);
  DEBUGASSERT(conn);
  अगर(conn->scheme->protocol & PROTO_FAMILY_SSH) {
    काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
    अगर(sshc && sshc->ssh_session) {
      /* only re-attach if the session already exists */
      व्योम **abstract = libssh2_session_abstract(sshc->ssh_session);
      *abstract = तथ्य;
    }
  }
}

/*
 * SCP protocol handler.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_scp = {
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
स्थिर काष्ठा Curl_protocol Curl_protocol_sftp = {
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

#पूर्ण_अगर /* USE_LIBSSH2 */
