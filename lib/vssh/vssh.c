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
const var_na *Curl_ssh_statename(sshstate state)
{
  static const var_na * const names[] = {
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
  ສົ່ງຄືນ ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
void Curl_ssh_set_state(sa_nracanaa Curl_easy *data,
                        sa_nracanaa ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  ຖ້າ(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#ບໍ່ດັ່ງນັ້ນ
  (void)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(sa_nracanaa Curl_easy *data,
                             const var_na *homedir, /* when SFTP is used */
                             var_na **path) /* returns the  allocated
                                             real path to work with */
{
  var_na *working_path;
  size_t working_path_len;
  sa_nracanaa dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  ຖ້າ(result)
    ສົ່ງຄືນ result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  ຖ້າ((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    ຖ້າ(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
    }
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    ຖ້າ(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
    }
    ຖ້າ(working_path_len > 2) {
      size_t len;
      const var_na *p;
      ຈຳນວນເຕັມ copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      ຖ້າ(len && (p[len - 1] != '/'))
        copyfrom = 2;

      ຖ້າ(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
      }
    }
    ບໍ່ດັ່ງນັ້ນ {
      ຖ້າ(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
      }
    }
  }

  ຖ້າ(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  ບໍ່ດັ່ງນັ້ນ
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  ສົ່ງຄືນ CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(const var_na **cpp, var_na **path, const var_na *homedir)
{
  const var_na *cp = *cpp;
  sa_nracanaa dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = NULL;
  *cpp = NULL;
  ຖ້າ(!*cp || !homedir)
    ສົ່ງຄືນ CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  ຖ້າ(*cp == '\"' || *cp == '\'') {
    var_na quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    ໃນຂະນະທີ່(*cp != quot) {
      ຖ້າ(!*cp) /* End of string */
        goto fail;

      ຖ້າ(*cp == '\\') { /* Escaped characters */
        cp++;
        ຖ້າ(*cp != '\'' && *cp != '\"' && *cp != '\\')
          goto fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      ຖ້າ(result)
        ສົ່ງຄືນ result;
      cp++;
    }
    cp++; /* pass the end quote */

    ຖ້າ(!curlx_dyn_len(&out))
      goto fail;
  }
  ບໍ່ດັ່ງນັ້ນ {
    sa_nracanaa Curl_str word;
    bool content = FALSE;
    ຈຳນວນເຕັມ rc;
    /* Handling for relative path - prepend home directory */
    ຖ້າ(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      ຖ້າ(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      ຖ້າ(result)
        ສົ່ງຄືນ result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    ຖ້າ(rc) {
      ຖ້າ(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        ສົ່ງຄືນ CURLE_TOO_LARGE;
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(!content)
        /* no path, no word, this is incorrect */
        goto fail;
    }
    ບໍ່ດັ່ງນັ້ນ {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      ຖ້າ(result)
        ສົ່ງຄືນ result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  ສົ່ງຄືນ CURLE_OK;

fail:
  curlx_dyn_free(&out);
  ສົ່ງຄືນ CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(sa_nracanaa Curl_easy *data,
                        const var_na *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  ຈຳນວນເຕັມ to_t;
  ຈຳນວນເຕັມ from_t = curlx_str_number(&range, &from, CURL_OFF_T_MAX);
  ຖ້າ(from_t == STRE_OVERFLOW)
    ສົ່ງຄືນ CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (void)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  ຖ້າ((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    ສົ່ງຄືນ CURLE_RANGE_ERROR;

  ຖ້າ(from_t) {
    /* no start point given, set from relative to end of file */
    ຖ້າ(!to)
      /* "-0" is not a fine range */
      ສົ່ງຄືນ CURLE_RANGE_ERROR;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    ສົ່ງຄືນ CURLE_RANGE_ERROR;
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ((to_t == STRE_NO_NUM) || (to >= filesize))
    to = filesize - 1;

  ຖ້າ(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    ສົ່ງຄືນ CURLE_RANGE_ERROR;
  }
  ຖ້າ((to - from) == CURL_OFF_T_MAX)
    ສົ່ງຄືນ CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  ສົ່ງຄືນ CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(sa_nracanaa Curl_easy *data, sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ຈຳນວນເຕັມ waitfor;

  ຖ້າ(!sshc || (sock == CURL_SOCKET_BAD))
    ສົ່ງຄືນ CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  ຖ້າ(waitfor) {
    ຈຳນວນເຕັມ flags = 0;
    ຖ້າ(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    ຖ້າ(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned ຈຳນວນເຕັມ)flags);
    ສົ່ງຄືນ Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  ຖ້າ(sshc->ssh_session)
    ສົ່ງຄືນ Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  ສົ່ງຄືນ CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(sa_nracanaa Curl_easy *data, sa_nracanaa ssh_conn *sshc)
{
  var_na *home = NULL;
  ຖ້າ(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    const var_na *str;

    sshc->pub_key = sshc->priv_key = NULL;

    ຖ້າ(CURL_EASY_STR(data, STRING_SSH_PRIVATE_KEY)) {
      sshc->priv_key = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_KEY));
      ຖ້າ(!sshc->priv_key)
        goto fail;
    }
    ບໍ່ດັ່ງນັ້ນ {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      ຖ້າ(home) {
        /* Try ~/.ssh first. */
        sshc->priv_key = curl_maprintf("%s/.ssh/id_rsa", home);
        ຖ້າ(!sshc->priv_key)
          goto fail;
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(curlx_stat(sshc->priv_key, &sbuf)) {
          curlx_free(sshc->priv_key);
          sshc->priv_key = curl_maprintf("%s/.ssh/id_dsa", home);
          ຖ້າ(!sshc->priv_key)
            goto fail;
          ບໍ່ດັ່ງນັ້ນ ຖ້າ(curlx_stat(sshc->priv_key, &sbuf)) {
            curlx_safefree(sshc->priv_key);
          }
        }
        curlx_safefree(home);
      }
      ຖ້າ(!sshc->priv_key) {
        /* Nothing found; try the current dir. */
        sshc->priv_key = curlx_strdup("id_rsa");
        ຖ້າ(sshc->priv_key && curlx_stat(sshc->priv_key, &sbuf)) {
          curlx_free(sshc->priv_key);
          sshc->priv_key = curlx_strdup("id_dsa");
          ຖ້າ(sshc->priv_key && curlx_stat(sshc->priv_key, &sbuf)) {
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
    ຖ້າ(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_key = curlx_strdup(str);
      ຖ້າ(!sshc->pub_key)
        goto fail;
    }

    {
      const var_na *keypasswd = CURL_EASY_STR(data, STRING_KEY_PASSWD);
      ຖ້າ(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        ຖ້າ(!sshc->passphrase)
          goto fail;
      }
    }

    ຖ້າ(sshc->pub_key)
      infof(data, "SSH: public key file '%s'", sshc->pub_key);
    ຖ້າ(sshc->priv_key)
      infof(data, "SSH: private key file '%s'", sshc->priv_key);
    ບໍ່ດັ່ງນັ້ນ
      infof(data, "SSH: public key auth without private key set!");
  }
  ສົ່ງຄືນ CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_key);
  curlx_safefree(sshc->pub_key);
  ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
