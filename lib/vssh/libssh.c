/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Red Hat, Inc.
 *
 * Authors: Nikos Mavrogiannopoulos, Tomas Mraz, Stanislav Zidek,
 *          Robert Kolcun, Andreas Schneider
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
#include "curl_setup.h"

#ifdef USE_LIBSSH

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef __VMS
#include <in.h>
#include <inet.h>
#endif

#include "urldata.h"
#include "sendf.h"
#include "curl_trc.h"
#include "progress.h"
#include "transfer.h"
#include "vssh/ssh.h"
#include "url.h"
#include "cfilters.h"
#include "connect.h"
#include "parsedate.h"          /* for the week day and month names */
#include "curlx/strparse.h"
#include "multiif.h"
#include "select.h"
#include "vssh/vssh.h"
#include "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  do {                          \
    если(x) {                     \
      ssh_string_free_char(x);  \
      x = NULL;                 \
    }                           \
  } пока(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

static CURLcode sftp_error_to_CURLE(целое err)
{
  switch(err) {
  case SSH_FX_OK:
    вернуть CURLE_OK;

  case SSH_FX_NO_SUCH_FILE:
  case SSH_FX_NO_SUCH_PATH:
    вернуть CURLE_REMOTE_FILE_NOT_FOUND;

  case SSH_FX_PERMISSION_DENIED:
  case SSH_FX_WRITE_PROTECT:
    вернуть CURLE_REMOTE_ACCESS_DENIED;

  case SSH_FX_FILE_ALREADY_EXISTS:
    вернуть CURLE_REMOTE_FILE_EXISTS;

  default:
    break;
  }

  вернуть CURLE_SSH;
}

/* Multiple options:
 * 1. CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256) is set
 *    with a SHA256 hash.
 * 2. CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_MD5) is set
 *    with an MD5 hash (90s style auth, not sure we should have it here)
 * 3. data->set.ssh_keyfunc callback is set. Then we do trust on first
 *    use. We even save on knownhosts if CURLKHSTAT_FINE_ADD_TO_FILE
 *    is returned by it.
 * 4. none of the above. We only accept if it is present on known hosts.
 *
 * Returns SSH_OK or SSH_ERROR.
 */
static целое myssh_is_known(struct Curl_easy *data, struct ssh_conn *sshc)
{
  целое rc;
  ssh_key pubkey;
  unsigned char *hash_sha256 = NULL;
  size_t hlen_sha256;
  unsigned char *hash_md5 = NULL;
  size_t hlen_md5;
  char *found_base64 = NULL;
  char *known_base64 = NULL;
  целое vstate;
  enum curl_khmatch keymatch;
  struct curl_khkey foundkey;
  struct curl_khkey *knownkeyp = NULL;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  struct ssh_knownhosts_entry *knownhostsentry = NULL;
  struct curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  если(rc != SSH_OK)
    вернуть rc;

  если(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256)) {
    const char *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256);
    char *fingerprint_b64 = NULL;
    size_t fingerprint_b64_len;
    size_t pub_pos = 0;
    size_t b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    если(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      goto cleanup;
    }

    если(curlx_base64_encode((const uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      goto cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    пока((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    пока((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    если((pub_pos != b64_pos) ||
       strncmp(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      goto cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    goto cleanup;
  }

  если(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_MD5)) {
    const char *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_MD5);
    char md5buffer[33];
    целое i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    если(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      goto cleanup;
    }

    для(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    если(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      goto cleanup;
    }

    rc = SSH_OK;
    goto cleanup;
  }

  если(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    если(knownhostsentry) {
      если(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        если(rc != SSH_OK) {
          goto cleanup;
        }
        knownkey.key = known_base64;
        knownkey.len = strlen(known_base64);

        switch(ssh_key_type(knownhostsentry->publickey)) {
        case SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          break;
        case SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          break;
        case SSH_KEYTYPE_ECDSA:
        case SSH_KEYTYPE_ECDSA_P256:
        case SSH_KEYTYPE_ECDSA_P384:
        case SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          break;
        case SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          break;
        case SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          break;
        default:
          rc = SSH_ERROR;
          goto cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    switch(vstate) {
    case SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      break;
    case SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      break;
    case SSH_KNOWN_HOSTS_NOT_FOUND:
    case SSH_KNOWN_HOSTS_UNKNOWN:
    case SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      break;
    default:
      keymatch = CURLKHMATCH_MISMATCH;
      break;
    }

    если(func) { /* use callback to determine action */
      struct Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      если(rc != SSH_OK)
        goto cleanup;

      foundkey.key = found_base64;
      foundkey.len = strlen(found_base64);

      switch(ssh_key_type(pubkey)) {
      case SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        break;
      case SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        break;
      case SSH_KEYTYPE_ECDSA:
      case SSH_KEYTYPE_ECDSA_P256:
      case SSH_KEYTYPE_ECDSA_P384:
      case SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        break;
      case SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        break;
      case SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        break;
      default:
        rc = SSH_ERROR;
        goto cleanup;
      }

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      switch(rc) {
      case CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        если(rc != SSH_OK) {
          goto cleanup;
        }
        break;
      case CURLKHSTAT_FINE:
        break;
      default: /* REJECT/DEFER */
        rc = SSH_ERROR;
        goto cleanup;
      }
    }
    иначе {
      если(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        goto cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  если(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    free(found_base64); /* allocated by libssh, deallocate with system free */
  }
  если(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    free(known_base64); /* allocated by libssh, deallocate with system free */
  }
  если(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  если(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_key_free(pubkey);
  если(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  вернуть rc;
}

static целое myssh_to_ERROR(struct Curl_easy *data,
                          struct ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  вернуть SSH_ERROR;
}

static целое myssh_to_SFTP_CLOSE(struct Curl_easy *data,
                               struct ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  вернуть SSH_ERROR;
}

static целое myssh_to_PASSWD_AUTH(struct Curl_easy *data,
                                struct ssh_conn *sshc)
{
  если(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    вернуть SSH_OK;
  }
  вернуть myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

static целое myssh_to_KEY_AUTH(struct Curl_easy *data,
                             struct ssh_conn *sshc)
{
  если(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_KEY_INIT);
    вернуть SSH_OK;
  }
  вернуть myssh_to_PASSWD_AUTH(data, sshc);
}

static целое myssh_to_GSSAPI_AUTH(struct Curl_easy *data,
                                struct ssh_conn *sshc)
{
  если(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    вернуть SSH_OK;
  }
  вернуть myssh_to_KEY_AUTH(data, sshc);
}

static целое myssh_in_SFTP_READDIR_INIT(struct Curl_easy *data,
                                      struct ssh_conn *sshc,
                                      struct SSHPROTO *sshp)
{
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  если(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    вернуть SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  если(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    вернуть myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_READDIR(struct Curl_easy *data,
                                 struct ssh_conn *sshc,
                                 struct SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  если(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  если(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = strlen(sshc->readdir_filename);

    если(data->set.list_only) {
      char *tmpLine;

      tmpLine = curl_maprintf("%s\n", sshc->readdir_filename);
      если(!tmpLine) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        вернуть SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmpLine, sshc->readdir_len + 1);
      curlx_free(tmpLine);

      если(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        вернуть SSH_NO_ERROR;
      }
    }
    иначе {
      если(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        вернуть SSH_ERROR;
      }

      если((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        если(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          вернуть SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        вернуть SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      вернуть SSH_NO_ERROR;
    }
  }
  иначе если(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  иначе {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    вернуть myssh_to_SFTP_CLOSE(data, sshc);
  }
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_READDIR_LINK(struct Curl_easy *data,
                                      struct ssh_conn *sshc)
{
  если(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  если(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    вернуть myssh_to_SFTP_CLOSE(data, sshc);
  }

  если(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    если(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    иначе
      sshc->readdir_len = strlen(sshc->readdir_tmp);
    sshc->readdir_longentry = NULL;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  иначе {
    sshc->readdir_len = strlen(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  если(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    вернуть SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = NULL;
  sshc->readdir_filename = NULL;
  sshc->readdir_longentry = NULL;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_READDIR_BOTTOM(struct Curl_easy *data,
                                        struct ssh_conn *sshc)
{
  CURLcode result;

  если(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  иначе
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = NULL;

  если(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  иначе
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_READDIR_DONE(struct Curl_easy *data,
                                      struct ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = NULL;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  вернуть SSH_NO_ERROR;
}

static void myssh_quote_error(struct Curl_easy *data, struct ssh_conn *sshc,
                              const char *cmd)
{
  если(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

static целое myssh_in_SFTP_QUOTE_STATVFS(struct Curl_easy *data,
                                       struct ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  если(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    вернуть SSH_OK;
  }
  иначе если(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#elif defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#иначе
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    char *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_files: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_SIZE_MASK "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_blocks, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    если(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    если(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
      curlx_free(tmp);
    }
    если(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  вернуть SSH_OK;
}

static целое myssh_auth_interactive(struct connectdata *conn,
                                  struct ssh_conn *sshc)
{
  целое rc;
  целое nprompts;

restart:
  switch(sshc->kbd_state) {
  case 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, NULL, NULL);
    если(rc == SSH_AUTH_AGAIN)
      вернуть SSH_AGAIN;

    если(rc != SSH_AUTH_INFO)
      вернуть SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    если(nprompts != 1)
      вернуть SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    если(rc < 0)
      вернуть SSH_ERROR;

    FALLTHROUGH();
  case 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, NULL, NULL);
    если(rc == SSH_AUTH_AGAIN)
      вернуть SSH_AGAIN;
    иначе если(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    иначе если(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      если(nprompts)
        вернуть SSH_ERROR;

      sshc->kbd_state = 2;
      goto restart;
    }
    иначе
      rc = SSH_ERROR;
    break;
  case 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, NULL, NULL);
    если(rc == SSH_AUTH_AGAIN)
      вернуть SSH_AGAIN;
    иначе если(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    иначе
      rc = SSH_ERROR;

    break;
  default:
    вернуть SSH_ERROR;
  }

  sshc->kbd_state = 0;
  вернуть rc;
}

static void myssh_state_init(struct Curl_easy *data,
                             struct ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#если 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

static void myssh_block2waitfor(struct connectdata *conn,
                                struct ssh_conn *sshc,
                                bool block)
{
  (void)conn;
  если(block) {
    целое dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  иначе
    sshc->waitfor = 0;
}

static целое myssh_in_S_STARTUP(struct Curl_easy *data,
                              struct ssh_conn *sshc)
{
  struct connectdata *conn = data->conn;
  целое rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  если(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  иначе если(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  иначе
    myssh_to(data, sshc, SSH_HOSTKEY);

  вернуть rc;
}

static целое myssh_in_AUTHLIST(struct Curl_easy *data,
                             struct ssh_conn *sshc)
{
  целое rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, NULL);
  если(rc == SSH_AUTH_AGAIN)
    вернуть SSH_AGAIN;

  если(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    вернуть rc;
  }
  иначе если(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    вернуть rc;
  }

  sshc->auth_methods =
    (unsigned целое)ssh_userauth_list(sshc->ssh_session, NULL);
  если(sshc->auth_methods)
    infof(data, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  если((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_key && sshc->priv_key[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  иначе если(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  иначе если(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_KEY_INIT);
  }
  иначе если(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  иначе {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  вернуть rc;
}

static целое myssh_in_AUTH_PKEY_INIT(struct Curl_easy *data,
                                   struct ssh_conn *sshc)
{
  целое rc;
  если(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    вернуть rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  если(sshc->priv_key && sshc->priv_key[0]) {
    если(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, NULL, sshc->pubkey);
      если(rc == SSH_AUTH_AGAIN)
        вернуть SSH_AGAIN;

      если(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        вернуть rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_key,
                                     sshc->passphrase, NULL,
                                     NULL, &sshc->privkey);
    если(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_key);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      вернуть rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  иначе {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, NULL,
                                     sshc->passphrase);
    если(rc == SSH_AUTH_AGAIN)
      вернуть SSH_AGAIN;

    если(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      вернуть rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  вернуть rc;
}

static целое myssh_in_AUTH_PKEY(struct Curl_easy *data,
                              struct ssh_conn *sshc)
{
  целое rc = ssh_userauth_publickey(sshc->ssh_session, NULL, sshc->privkey);
  если(rc == SSH_AUTH_AGAIN)
    вернуть SSH_AGAIN;
  иначе если(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    вернуть SSH_OK;
  }
  иначе {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    вернуть myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

static целое myssh_in_AUTH_GSSAPI(struct Curl_easy *data,
                                struct ssh_conn *sshc)
{
  целое rc;
  если(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    вернуть myssh_to_KEY_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  если(rc == SSH_AUTH_AGAIN)
    вернуть SSH_AGAIN;

  если(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    вернуть SSH_OK;
  }

  вернуть myssh_to_KEY_AUTH(data, sshc);
}

static целое myssh_in_AUTH_KEY_INIT(struct Curl_easy *data,
                                  struct ssh_conn *sshc)
{
  если(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_KEY);
    вернуть SSH_NO_ERROR;
  }
  вернуть myssh_to_PASSWD_AUTH(data, sshc);
}

static целое myssh_in_AUTH_KEY(struct Curl_easy *data,
                             struct ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  целое rc = myssh_auth_interactive(data->conn, sshc);
  если(rc == SSH_AGAIN)
    вернуть rc;
  иначе если(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    вернуть SSH_NO_ERROR;
  }
  иначе
    вернуть myssh_to_PASSWD_AUTH(data, sshc);
}

static целое myssh_in_AUTH_PASS_INIT(struct Curl_easy *data,
                                   struct ssh_conn *sshc)
{
  если(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    вернуть myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_AUTH_PASS(struct Curl_easy *data,
                              struct ssh_conn *sshc)
{
  целое rc = ssh_userauth_password(sshc->ssh_session, NULL,
                                 Curl_creds_passwd(data->conn->creds));
  если(rc == SSH_AUTH_AGAIN)
    вернуть SSH_AGAIN;
  иначе если(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    вернуть SSH_NO_ERROR;
  }
  вернуть myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

static целое myssh_in_AUTH_DONE(struct Curl_easy *data,
                              struct ssh_conn *sshc)
{
  struct connectdata *conn = data->conn;
  если(!sshc->authed) {
    failf(data, "Authentication failure");
    вернуть myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  если(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    вернуть SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_UPLOAD_INIT(struct Curl_easy *data,
                                struct ssh_conn *sshc,
                                struct SSHPROTO *sshp)
{
  целое flags;
  целое rc = 0;

  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  если(data->state.resume_from) {
    sftp_attributes attrs;

    если(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      если(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        если(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          вернуть rc;
        }
        data->state.resume_from = size;
      }
      иначе {
        data->state.resume_from = 0;
      }
    }
  }

  если(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  иначе если(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  иначе
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  если(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  если(!sshc->sftp_file) {
    целое err = sftp_get_error(sshc->sftp_session);

    если((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (strlen(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      вернуть rc;
    }
    иначе {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      вернуть rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  если(data->state.resume_from > 0 && !data->set.remote_append) {
    целое seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    если(data->set.seek_func) {
      struct Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, SEEK_SET);
      CURL_CBAPI_END(&guard);
    }

    если(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      если(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        вернуть rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      do {
        char scratch[4 * 1024];
        size_t readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)sizeof(scratch)) ?
          sizeof(scratch) : curlx_sotouz(data->state.resume_from - passed);

        size_t actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        если((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          вернуть rc;
        }
      } пока(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    если(data->state.infilesize > 0) {
      если(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        вернуть myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    если(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      вернуть rc;
    }
  }
  если(data->state.infilesize > 0) {
    data->req.size = data->state.infilesize;
    Curl_pgrsSetUploadSize(data, data->state.infilesize);
  }
  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(data);
#если LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  вернуть rc;
}

static целое myssh_in_SFTP_DOWNLOAD_INIT(struct Curl_easy *data,
                                       struct ssh_conn *sshc,
                                       struct SSHPROTO *sshp)
{
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  если(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  если(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    вернуть myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_DOWNLOAD_STAT(struct Curl_easy *data,
                                       struct ssh_conn *sshc)
{
  curl_off_t size;
  целое rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  если(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_SIZE) ||
     (attrs->size == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    data->req.size = -1;
    data->req.maxdownload = -1;
    Curl_pgrsSetDownloadSize(data, -1);
    size = 0;
    если(attrs)
      sftp_attributes_free(attrs);
  }
  иначе {
    size = attrs->size;

    sftp_attributes_free(attrs);

    если(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      вернуть myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    если(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      если(result)
        вернуть myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      если(rc)
        вернуть myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  если(data->state.resume_from) {
    если(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      если(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        вернуть myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    иначе {
      если(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        вернуть myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    если(rc)
      вернуть myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  если(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    вернуть rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  вернуть rc;
}

static целое myssh_in_SFTP_CLOSE(struct Curl_easy *data,
                               struct ssh_conn *sshc,
                               struct SSHPROTO *sshp)
{
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  если(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = NULL;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  если(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  иначе {
    myssh_to(data, sshc, SSH_STOP);
  }
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_SHUTDOWN(struct Curl_easy *data,
                                  struct ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#если LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  если(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = NULL;
  }

  если(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = NULL;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_INIT(struct Curl_easy *data,
                              struct ssh_conn *sshc)
{
  целое rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  если(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    вернуть myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  если(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    вернуть myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_REALPATH(struct Curl_easy *data,
                                  struct ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  если(!sshc->homedir)
    вернуть myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  если(!data->state.most_recent_ftp_entrypath)
    вернуть myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_QUOTE_INIT(struct Curl_easy *data,
                                    struct ssh_conn *sshc,
                                    struct SSHPROTO *sshp)
{
  CURLcode result;

  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  если(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  иначе если(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  иначе
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_POSTQUOTE_INIT(struct Curl_easy *data,
                                        struct ssh_conn *sshc)
{
  если(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  иначе {
    myssh_to(data, sshc, SSH_STOP);
  }
  вернуть SSH_NO_ERROR;
}

static целое quote_error(struct Curl_easy *data,
                       struct ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_QUOTE(struct Curl_easy *data,
                               struct ssh_conn *sshc,
                               struct SSHPROTO *sshp)
{
  const char *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  const char *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  если(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  если(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    char *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    если(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      вернуть SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, strlen(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, strlen(tmp));
    curlx_free(tmp);
    если(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    иначе
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    вернуть SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = strchr(cmd, ' ');
  если(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    вернуть SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  если(result) {
    если(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    иначе
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    вернуть SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  если(!strncmp(cmd, "chgrp ", 6) ||
     !strncmp(cmd, "chmod ", 6) ||
     !strncmp(cmd, "chown ", 6) ||
     !strncmp(cmd, "atime ", 6) ||
     !strncmp(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    если(result) {
      если(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      иначе
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      вернуть SSH_NO_ERROR;
    }
    если(*cp)
      вернуть quote_error(data, sshc);
    sshc->quote_attrs = NULL;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    вернуть SSH_NO_ERROR;
  }
  если(!strncmp(cmd, "ln ", 3) ||
     !strncmp(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    если(result) {
      если(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      иначе
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      вернуть SSH_NO_ERROR;
    }
    если(*cp)
      вернуть quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    вернуть SSH_NO_ERROR;
  }
  иначе если(!strncmp(cmd, "mkdir ", 6)) {
    если(*cp)
      вернуть quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    вернуть SSH_NO_ERROR;
  }
  иначе если(!strncmp(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    если(result) {
      если(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      иначе
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      вернуть SSH_NO_ERROR;
    }
    если(*cp)
      вернуть quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    вернуть SSH_NO_ERROR;
  }
  иначе если(!strncmp(cmd, "rmdir ", 6)) {
    /* delete directory */
    если(*cp)
      вернуть quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    вернуть SSH_NO_ERROR;
  }
  иначе если(!strncmp(cmd, "rm ", 3)) {
    если(*cp)
      вернуть quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    вернуть SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  иначе если(!strncmp(cmd, "statvfs ", 8)) {
    если(*cp)
      вернуть quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    вернуть SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_NEXT_QUOTE(struct Curl_easy *data,
                                    struct ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  если(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  иначе {
    если(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    иначе {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_QUOTE_STAT(struct Curl_easy *data,
                                    struct ssh_conn *sshc)
{
  const char *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  если(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  если(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  если(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, NULL);
    вернуть SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  если(!strncmp(cmd, "chgrp", 5)) {
    const char *p = sshc->quote_path1;
    curl_off_t gid;
    если(curlx_str_number(&p, &gid, UINT_MAX)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, NULL);
      вернуть SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  иначе если(!strncmp(cmd, "chmod", 5)) {
    curl_off_t perms;
    const char *p = sshc->quote_path1;
    если(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, NULL);
      вернуть SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  иначе если(!strncmp(cmd, "chown", 5)) {
    const char *p = sshc->quote_path1;
    curl_off_t uid;
    если(curlx_str_number(&p, &uid, UINT_MAX)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, NULL);
      вернуть SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  иначе если(!strncmp(cmd, "atime", 5) ||
          !strncmp(cmd, "mtime", 5)) {
    time_t date;
    bool fail = FALSE;
    если(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#если SIZEOF_TIME_T > 4
    иначе если(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    если(fail) {
      myssh_quote_error(data, sshc, NULL);
      вернуть SSH_NO_ERROR;
    }
    если(!strncmp(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    иначе /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  вернуть SSH_NO_ERROR;
}

static void conn_forget_socket(struct Curl_easy *data, int8_t sockindex)
{
  struct connectdata *conn = data->conn;
  если(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    struct Curl_cfilter *cf = conn->cfilter[sockindex];
    если(cf)
      (void)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, NULL);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

static void myssh_SESSION_DISCONNECT(struct Curl_easy *data,
                                     struct ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  если(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = NULL;
  }

  если(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = NULL;
  }
  если(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = NULL;
  }

  ssh_disconnect(sshc->ssh_session);
  если(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

static целое myssh_SSH_SCP_DOWNLOAD(struct Curl_easy *data,
                                  struct ssh_conn *sshc)
{
  uint64_t bytecount;
  целое rc = ssh_scp_pull_request(sshc->scp_session);
  если(rc != SSH_SCP_REQUEST_NEWFILE) {
    const char *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    вернуть myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  если(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    вернуть myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  вернуть 0;
}

static целое myssh_in_TRANS_INIT(struct Curl_easy *data, struct ssh_conn *sshc,
                               struct SSHPROTO *sshp)
{
  CURLcode result;
  целое rc = 0;
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  если(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    вернуть 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  если(data->state.upload) {
    если(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      вернуть myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  иначе {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  если(!sshc->scp_session) {
    const char *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  вернуть rc;
}

static void sshc_cleanup(struct ssh_conn *sshc)
{
  если(sshc->initialized) {
    если(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = NULL;
    }
    если(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = NULL;
    }
    если(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = NULL;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    если(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = NULL;
    }
    если(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = NULL;
    }
    если(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = NULL;
    }
    если(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = NULL;
    }
    если(sshc->privkey) {
      ssh_key_free(sshc->privkey);
      sshc->privkey = NULL;
    }
    если(sshc->pubkey) {
      ssh_key_free(sshc->pubkey);
      sshc->pubkey = NULL;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_key);
    curlx_safefree(sshc->priv_key);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

static целое myssh_in_SFTP_QUOTE_SETSTAT(struct Curl_easy *data,
                                       struct ssh_conn *sshc)
{
  целое rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  если(rc == SSH_AGAIN)
    вернуть rc;
  если(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    вернуть rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_QUOTE_SYMLINK(struct Curl_easy *data,
                                       struct ssh_conn *sshc)
{
  целое rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  если(rc == SSH_AGAIN)
    вернуть rc;
  если(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    вернуть rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_QUOTE_MKDIR(struct Curl_easy *data,
                                     struct ssh_conn *sshc)
{
  целое rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  если(rc == SSH_AGAIN)
    вернуть rc;
  если(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    вернуть rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_QUOTE_RENAME(struct Curl_easy *data,
                                      struct ssh_conn *sshc)
{
  целое rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  если(rc == SSH_AGAIN)
    вернуть rc;
  если(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    вернуть rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_QUOTE_RMDIR(struct Curl_easy *data,
                                     struct ssh_conn *sshc)
{
  целое rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  если(rc == SSH_AGAIN)
    вернуть rc;
  если(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    вернуть rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_QUOTE_UNLINK(struct Curl_easy *data,
                                      struct ssh_conn *sshc)
{
  целое rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  если(rc == SSH_AGAIN)
    вернуть rc;
  если(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    вернуть rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_GETINFO(struct Curl_easy *data,
                                 struct ssh_conn *sshc)
{
  если(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  иначе
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_FILETIME(struct Curl_easy *data,
                                  struct ssh_conn *sshc,
                                  struct SSHPROTO *sshp)
{
  sftp_attributes attrs;
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  если(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_TRANS_INIT(struct Curl_easy *data,
                                    struct ssh_conn *sshc,
                                    struct SSHPROTO *sshp)
{
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  если(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  иначе {
    size_t path_len = strlen(sshp->path);

    если(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    иначе
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_CREATE_DIRS_INIT(struct Curl_easy *data,
                                          struct ssh_conn *sshc,
                                          struct SSHPROTO *sshp)
{
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  если(strlen(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  иначе {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_CREATE_DIRS(struct Curl_easy *data,
                                     struct ssh_conn *sshc,
                                     struct SSHPROTO *sshp)
{
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = strchr(sshc->slash_pos, '/');
  если(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    вернуть SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SFTP_CREATE_DIRS_MKDIR(struct Curl_easy *data,
                                           struct ssh_conn *sshc,
                                           struct SSHPROTO *sshp)
{
  целое rc;
  целое err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  если(rc == SSH_AGAIN)
    вернуть rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  если(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    если((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      вернуть myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  вернуть rc;
}

static целое myssh_in_SCP_UPLOAD_INIT(struct Curl_easy *data,
                                    struct ssh_conn *sshc,
                                    struct SSHPROTO *sshp)
{
  целое rc;
  если(!sshp)
    вернуть myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  если(rc != SSH_OK) {
    const char *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    вернуть myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_push_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (целое)data->set.new_file_perms);

  если(rc != SSH_OK) {
    const char *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    вернуть myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SCP_DOWNLOAD_INIT(struct Curl_easy *data,
                                      struct ssh_conn *sshc)
{
  целое rc = ssh_scp_init(sshc->scp_session);
  если(rc != SSH_OK) {
    const char *err_msg = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_msg);
    вернуть myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SCP_DONE(struct Curl_easy *data,
                             struct ssh_conn *sshc)
{
  если(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  иначе
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SCP_SEND_EOF(struct Curl_easy *data,
                                 struct ssh_conn *sshc)
{
  если(sshc->scp_session) {
    целое rc = ssh_scp_close(sshc->scp_session);
    если(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      вернуть SSH_AGAIN;
    }
    если(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  вернуть SSH_NO_ERROR;
}

static целое myssh_in_SCP_CHANNEL_FREE(struct Curl_easy *data,
                                     struct ssh_conn *sshc)
{
  если(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = NULL;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  вернуть SSH_NO_ERROR;
}

static CURLcode myssh_in_SESSION_FREE(struct Curl_easy *data,
                                      struct ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  memset(sshc, 0, sizeof(struct ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  вернуть result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
static CURLcode myssh_statemachine(struct Curl_easy *data,
                                   struct ssh_conn *sshc,
                                   struct SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  целое rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  do {

    switch(sshc->state) {
    case SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    case SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      если(rc)
        break;
      FALLTHROUGH();
    case SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      если(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        break;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    case SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      break;
    case SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      break;
    case SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      break;
    case SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      break;
    case SSH_AUTH_KEY_INIT:
      rc = myssh_in_AUTH_KEY_INIT(data, sshc);
      break;
    case SSH_AUTH_KEY:
      rc = myssh_in_AUTH_KEY(data, sshc);
      break;
    case SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      break;
    case SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      break;
    case SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      break;
    case SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      break;
    case SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      break;
    case SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      break;
    case SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      break;
    case SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      break;
    case SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      break;
    case SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      break;
    case SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      break;
    case SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      break;
    case SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      break;
    case SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      break;
    case SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      break;
    case SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      break;
    case SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      break;
    case SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      break;
    case SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      break;
    case SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      break;
    case SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      break;
    case SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      break;
    case SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      break;
    case SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      break;
    case SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      break;
    case SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      break;
    case SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      break;
    case SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      break;
    case SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      break;
    case SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      break;
    case SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      break;
    case SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      break;
    case SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      break;
    case SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      break;
    case SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      break;
    case SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      если(rc)
        break;
      FALLTHROUGH();
    case SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      break;
    case SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      break;
    case SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      break;
    case SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    case SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    case SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      break;
    case SSH_QUIT:
    default:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      break;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } пока((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  если(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  если(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (целое)result, *block);
  вернуть result;
}

/* called repeatedly until done from multi.c */
static CURLcode myssh_multi_statemach(struct Curl_easy *data,
                                      bool *done)
{
  struct connectdata *conn = data->conn;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  struct SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  если(!sshc || !sshp)
    вернуть CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  вернуть result;
}

static CURLcode myssh_block_statemach(struct Curl_easy *data,
                                      struct ssh_conn *sshc,
                                      struct SSHPROTO *sshp,
                                      bool disconnect)
{
  struct connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  пока((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t left_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
    если(result)
      break;

    если(!disconnect) {
      result = Curl_pgrsCheck(data);
      если(result)
        break;

      left_ms = Curl_timeleft_ms(data);
      если(left_ms < 0) {
        failf(data, "Operation timed out");
        вернуть CURLE_OPERATION_TIMEDOUT;
      }
    }

    если(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (void)SOCKET_READABLE(fd_read,
                            (!left_ms || left_ms > 1000) ? 1000 : left_ms);
    }
  }

  вернуть result;
}

static void myssh_easy_dtor(const void *key, size_t klen, void *entry)
{
  struct SSHPROTO *sshp = entry;
  (void)key;
  (void)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

static void myssh_conn_dtor(const void *key, size_t klen, void *entry)
{
  struct ssh_conn *sshc = entry;
  (void)key;
  (void)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
static CURLcode myssh_setup_connection(struct Curl_easy *data,
                                       struct connectdata *conn)
{
  struct SSHPROTO *sshp;
  struct ssh_conn *sshc;

  sshc = curlx_calloc(1, sizeof(*sshc));
  если(!sshc)
    вернуть CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  если(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    вернуть CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, sizeof(*sshp));
  если(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    вернуть CURLE_OUT_OF_MEMORY;

  вернуть Curl_ssh_setup_pkey(data, sshc);
}

static Curl_recv scp_recv, sftp_recv;
static Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
static CURLcode myssh_connect(struct Curl_easy *data, bool *done)
{
  CURLcode result;
  struct connectdata *conn = data->conn;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  struct SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  целое rc;

  если(!sshc || !ssh)
    вернуть CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  если(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  иначе {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  если(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    вернуть CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  если(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    вернуть CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, NULL);
  если(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  если(rc != SSH_OK) {
    failf(data, "Could not set socket");
    вернуть CURLE_FAILED_INIT;
  }

  если(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    если(rc != SSH_OK) {
      failf(data, "Could not set user");
      вернуть CURLE_FAILED_INIT;
    }
  }

  если(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    если(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    если(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      вернуть CURLE_FAILED_INIT;
    }
  }

  если(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    если(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      вернуть CURLE_FAILED_INIT;
    }
  }

  если(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    если(rc != SSH_OK) {
      failf(data, "Could not set compression");
      вернуть CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = NULL;
  sshc->pubkey = NULL;

  если(sshc->pub_key) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_key, &sshc->pubkey);
    если(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      вернуть CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  вернуть result;
}

/* called from multi.c while DOing */
static CURLcode scp_doing(struct Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  если(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  вернуть result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

static CURLcode scp_perform(struct Curl_easy *data,
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  struct ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  если(!sshc)
    вернуть CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  если(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  вернуть result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
static CURLcode scp_disconnect(struct Curl_easy *data,
                               struct connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  struct SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (void)dead_connection;

  если(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  вернуть result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
static CURLcode myssh_done(struct Curl_easy *data,
                           struct ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  struct SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  если(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
  иначе
    result = status;

  если(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    вернуть CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  вернуть result;
}

static CURLcode scp_done(struct Curl_easy *data, CURLcode status,
                         bool premature)
{
  struct ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (void)premature;

  если(!sshc)
    вернуть CURLE_FAILED_INIT;
  если(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  вернуть myssh_done(data, sshc, status);
}

static CURLcode scp_send(struct Curl_easy *data, int8_t sockindex,
                         const uint8_t *mem, size_t len, bool eos,
                         size_t *pnwritten)
{
  целое rc;
  struct connectdata *conn = data->conn;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (void)sockindex; /* we only support SCP on the fixed known primary socket */
  (void)eos;
  *pnwritten = 0;

  если(!sshc)
    вернуть CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#если 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  если(rc == SSH_AGAIN)
    вернуть CURLE_AGAIN;
  иначе
#endif
  если(rc != SSH_OK)
    вернуть CURLE_SSH;

  *pnwritten = len;
  вернуть CURLE_OK;
}

static CURLcode scp_recv(struct Curl_easy *data, int8_t sockindex,
                         char *mem, size_t len, size_t *pnread)
{
  struct connectdata *conn = data->conn;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  целое nread;

  (void)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  если(!sshc)
    вернуть CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  если(nread == SSH_ERROR)
    вернуть CURLE_SSH;
#если 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  если(nread == SSH_AGAIN)
    вернуть CURLE_AGAIN;
#endif
  *pnread = (size_t)nread;
  вернуть CURLE_OK;
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

static CURLcode sftp_perform(struct Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  struct ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  если(!sshc)
    вернуть CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  если(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  вернуть result;
}

/* called from multi.c while DOing */
static CURLcode sftp_doing(struct Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  если(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  вернуть result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
static CURLcode sftp_disconnect(struct Curl_easy *data,
                                struct connectdata *conn,
                                bool dead_connection)
{
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  struct SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (void)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  если(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
  вернуть result;
}

static CURLcode sftp_done(struct Curl_easy *data, CURLcode status,
                          bool premature)
{
  struct connectdata *conn = data->conn;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  если(!sshc)
    вернуть CURLE_FAILED_INIT;
  если(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    если(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  вернуть myssh_done(data, sshc, status);
}

/* return number of sent bytes */
static CURLcode sftp_send(struct Curl_easy *data, int8_t sockindex,
                          const uint8_t *mem, size_t len, bool eos,
                          size_t *pnwritten)
{
  struct connectdata *conn = data->conn;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (void)sockindex;
  (void)eos;
  *pnwritten = 0;

  если(!sshc)
    вернуть CURLE_FAILED_INIT;

#если LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  switch(sshc->sftp_send_state) {
  case 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    если(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      вернуть CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  case 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    если(nwrite == SSH_AGAIN)
      вернуть CURLE_AGAIN;
    иначе если(nwrite < 0)
      вернуть CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (size_t)nwrite;
    вернуть CURLE_OK;
  default:
    /* we never reach here */
    вернуть CURLE_SEND_ERROR;
  }
#иначе
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  если(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#если 0 /* not returned by libssh on write */
  если(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  иначе
#endif
  если(nwrite < 0)
    вернуть CURLE_SSH;

  *pnwritten = (size_t)nwrite;
  вернуть CURLE_OK;
#endif
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
static CURLcode sftp_recv(struct Curl_easy *data, int8_t sockindex,
                          char *mem, size_t len, size_t *pnread)
{
  struct connectdata *conn = data->conn;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (void)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  если(!sshc)
    вернуть CURLE_FAILED_INIT;

  switch(sshc->sftp_recv_state) {
  case 0:
#если LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    если(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      вернуть CURLE_RECV_ERROR;
    }
#иначе
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    если(sshc->sftp_file_index < 0)
      вернуть CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  case 1:
    sshc->sftp_recv_state = 1;

#если LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#иначе
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    если(nread == SSH_AGAIN)
      вернуть CURLE_AGAIN;
    иначе если(nread < 0)
      вернуть CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (size_t)nread;
    вернуть CURLE_OK;

  default:
    /* we never reach here */
    вернуть CURLE_RECV_ERROR;
  }
}

static CURLcode myssh_do_it(struct Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  struct connectdata *conn = data->conn;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  если(!sshc)
    вернуть CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  если(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  иначе
    result = sftp_perform(data, &connected, done);

  вернуть result;
}

CURLcode Curl_ssh_init(void)
{
  если(ssh_init()) {
    DEBUGF(curl_mfprintf(stderr, "Error: libssh_init failed\n"));
    вернуть CURLE_FAILED_INIT;
  }
  вернуть CURLE_OK;
}

void Curl_ssh_cleanup(void)
{
  (void)ssh_finalize();
}

void Curl_ssh_version(char *buffer, size_t buflen)
{
  (void)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
const struct Curl_protocol Curl_protocol_scp = {
  myssh_setup_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_done,                             /* done */
  ZERO_NULL,                            /* do_more */
  myssh_connect,                        /* connect_it */
  myssh_multi_statemach,                /* connecting */
  scp_doing,                            /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  scp_disconnect,                       /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

/*
 * SFTP.
 */
const struct Curl_protocol Curl_protocol_sftp = {
  myssh_setup_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_done,                            /* done */
  ZERO_NULL,                            /* do_more */
  myssh_connect,                        /* connect_it */
  myssh_multi_statemach,                /* connecting */
  sftp_doing,                           /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  sftp_disconnect,                      /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#endif /* USE_LIBSSH */
