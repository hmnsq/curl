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
 * SPDX-License-Identifier: curl AND ISC
 *
 ***************************************************************************/
#include "curl_setup.h"
#include "vssh/ssh.h"

#ifdef USE_SSH

#include "vssh/vssh.h"
#include "curlx/strparse.h"
#include "curl_trc.h"
#include "escape.h"
#include "select.h"  /* for Curl_pollset_change() */
#include "url.h"  /* for Curl_conn_meta_get() */
#include "curlx/win32-fopen.h"

#ifdef CURLVERBOSE
const char *Curl_ssh_statename(sshstate state)
{
  static const char * const names[] = {
    "SSH_STOP",
    "SSH_INIT",
    "SSH_S_STARTUP",
    "SSH_HOSTKEY",
    "SSH_AUTHLIST",
    "SSH_AUTH_PKEY_INIT",
    "SSH_AUTH_PKEY",
    "SSH_AUTH_PASS_INIT",
    "SSH_AUTH_PASS",
    "SSH_AUTH_AGENT_INIT",
    "SSH_AUTH_AGENT_LIST",
    "SSH_AUTH_AGENT",
    "SSH_AUTH_HOST_INIT",
    "SSH_AUTH_HOST",
    "SSH_AUTH_KEY_INIT",
    "SSH_AUTH_KEY",
    "SSH_AUTH_GSSAPI",
    "SSH_AUTH_DONE",
    "SSH_SFTP_INIT",
    "SSH_SFTP_REALPATH",
    "SSH_SFTP_QUOTE_INIT",
    "SSH_SFTP_POSTQUOTE_INIT",
    "SSH_SFTP_QUOTE",
    "SSH_SFTP_NEXT_QUOTE",
    "SSH_SFTP_QUOTE_STAT",
    "SSH_SFTP_QUOTE_SETSTAT",
    "SSH_SFTP_QUOTE_SYMLINK",
    "SSH_SFTP_QUOTE_MKDIR",
    "SSH_SFTP_QUOTE_RENAME",
    "SSH_SFTP_QUOTE_RMDIR",
    "SSH_SFTP_QUOTE_UNLINK",
    "SSH_SFTP_QUOTE_STATVFS",
    "SSH_SFTP_GETINFO",
    "SSH_SFTP_FILETIME",
    "SSH_SFTP_TRANS_INIT",
    "SSH_SFTP_UPLOAD_INIT",
    "SSH_SFTP_CREATE_DIRS_INIT",
    "SSH_SFTP_CREATE_DIRS",
    "SSH_SFTP_CREATE_DIRS_MKDIR",
    "SSH_SFTP_READDIR_INIT",
    "SSH_SFTP_READDIR",
    "SSH_SFTP_READDIR_LINK",
    "SSH_SFTP_READDIR_BOTTOM",
    "SSH_SFTP_READDIR_DONE",
    "SSH_SFTP_DOWNLOAD_INIT",
    "SSH_SFTP_DOWNLOAD_STAT",
    "SSH_SFTP_CLOSE",
    "SSH_SFTP_SHUTDOWN",
    "SSH_SCP_TRANS_INIT",
    "SSH_SCP_UPLOAD_INIT",
    "SSH_SCP_DOWNLOAD_INIT",
    "SSH_SCP_DOWNLOAD",
    "SSH_SCP_DONE",
    "SSH_SCP_SEND_EOF",
    "SSH_SCP_WAIT_EOF",
    "SSH_SCP_WAIT_CLOSE",
    "SSH_SCP_CHANNEL_FREE",
    "SSH_SESSION_DISCONNECT",
    "SSH_SESSION_FREE",
    "QUIT"
  };
  /* a precaution to make sure the lists are in sync */
  DEBUGASSERT(CURL_ARRAYSIZE(names) == SSH_LAST);
  반환 ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
void Curl_ssh_set_state(struct Curl_easy *data,
                        struct ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  만약(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#아니면
  (void)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(struct Curl_easy *data,
                             const char *homedir, /* when SFTP is used */
                             char **path) /* returns the  allocated
                                             real path to work with */
{
  char *working_path;
  size_t working_path_len;
  struct dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  만약(result)
    반환 result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  만약((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    만약(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      반환 CURLE_OUT_OF_MEMORY;
    }
  }
  아니면 만약((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    만약(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      반환 CURLE_OUT_OF_MEMORY;
    }
    만약(working_path_len > 2) {
      size_t len;
      const char *p;
      정수 copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      만약(len && (p[len - 1] != '/'))
        copyfrom = 2;

      만약(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        반환 CURLE_OUT_OF_MEMORY;
      }
    }
    아니면 {
      만약(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        반환 CURLE_OUT_OF_MEMORY;
      }
    }
  }

  만약(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  아니면
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  반환 CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(const char **cpp, char **path, const char *homedir)
{
  const char *cp = *cpp;
  struct dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = NULL;
  *cpp = NULL;
  만약(!*cp || !homedir)
    반환 CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  만약(*cp == '\"' || *cp == '\'') {
    char quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    동안(*cp != quot) {
      만약(!*cp) /* End of string */
        goto fail;

      만약(*cp == '\\') { /* Escaped characters */
        cp++;
        만약(*cp != '\'' && *cp != '\"' && *cp != '\\')
          goto fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      만약(result)
        반환 result;
      cp++;
    }
    cp++; /* pass the end quote */

    만약(!curlx_dyn_len(&out))
      goto fail;
  }
  아니면 {
    struct Curl_str word;
    bool content = FALSE;
    정수 rc;
    /* Handling for relative path - prepend home directory */
    만약(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      만약(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      만약(result)
        반환 result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    만약(rc) {
      만약(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        반환 CURLE_TOO_LARGE;
      }
      아니면 만약(!content)
        /* no path, no word, this is incorrect */
        goto fail;
    }
    아니면 {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      만약(result)
        반환 result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  반환 CURLE_OK;

fail:
  curlx_dyn_free(&out);
  반환 CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(struct Curl_easy *data,
                        const char *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  정수 to_t;
  정수 from_t = curlx_str_number(&range, &from, CURL_OFF_T_MAX);
  만약(from_t == STRE_OVERFLOW)
    반환 CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (void)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  만약((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    반환 CURLE_RANGE_ERROR;

  만약(from_t) {
    /* no start point given, set from relative to end of file */
    만약(!to)
      /* "-0" is not a fine range */
      반환 CURLE_RANGE_ERROR;
    아니면 만약(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  아니면 만약(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    반환 CURLE_RANGE_ERROR;
  }
  아니면 만약((to_t == STRE_NO_NUM) || (to >= filesize))
    to = filesize - 1;

  만약(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    반환 CURLE_RANGE_ERROR;
  }
  만약((to - from) == CURL_OFF_T_MAX)
    반환 CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  반환 CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(struct Curl_easy *data, struct easy_pollset *ps)
{
  struct connectdata *conn = data->conn;
  struct ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  정수 waitfor;

  만약(!sshc || (sock == CURL_SOCKET_BAD))
    반환 CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  만약(waitfor) {
    정수 flags = 0;
    만약(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    만약(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned 정수)flags);
    반환 Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  만약(sshc->ssh_session)
    반환 Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  반환 CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(struct Curl_easy *data, struct ssh_conn *sshc)
{
  char *home = NULL;
  만약(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    const char *str;

    sshc->pub_key = sshc->priv_key = NULL;

    만약(CURL_EASY_STR(data, STRING_SSH_PRIVATE_KEY)) {
      sshc->priv_key = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_KEY));
      만약(!sshc->priv_key)
        goto fail;
    }
    아니면 {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      만약(home) {
        /* Try ~/.ssh first. */
        sshc->priv_key = curl_maprintf("%s/.ssh/id_rsa", home);
        만약(!sshc->priv_key)
          goto fail;
        아니면 만약(curlx_stat(sshc->priv_key, &sbuf)) {
          curlx_free(sshc->priv_key);
          sshc->priv_key = curl_maprintf("%s/.ssh/id_dsa", home);
          만약(!sshc->priv_key)
            goto fail;
          아니면 만약(curlx_stat(sshc->priv_key, &sbuf)) {
            curlx_safefree(sshc->priv_key);
          }
        }
        curlx_safefree(home);
      }
      만약(!sshc->priv_key) {
        /* Nothing found; try the current dir. */
        sshc->priv_key = curlx_strdup("id_rsa");
        만약(sshc->priv_key && curlx_stat(sshc->priv_key, &sbuf)) {
          curlx_free(sshc->priv_key);
          sshc->priv_key = curlx_strdup("id_dsa");
          만약(sshc->priv_key && curlx_stat(sshc->priv_key, &sbuf)) {
            curlx_free(sshc->priv_key);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_key = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_KEY);
    만약(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_key = curlx_strdup(str);
      만약(!sshc->pub_key)
        goto fail;
    }

    {
      const char *keypasswd = CURL_EASY_STR(data, STRING_KEY_PASSWD);
      만약(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        만약(!sshc->passphrase)
          goto fail;
      }
    }

    만약(sshc->pub_key)
      infof(data, "SSH: public key file '%s'", sshc->pub_key);
    만약(sshc->priv_key)
      infof(data, "SSH: private key file '%s'", sshc->priv_key);
    아니면
      infof(data, "SSH: public key auth without private key set!");
  }
  반환 CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_key);
  curlx_safefree(sshc->pub_key);
  반환 CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
