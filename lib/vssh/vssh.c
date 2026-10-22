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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "vssh/ssh.h"

#ifdef USE_SSH

#ಅನ್ತರ್ಭಾವ "vssh/vssh.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "select.h"  /* for Curl_pollset_change() */
#ಅನ್ತರ್ಭಾವ "url.h"  /* for Curl_conn_meta_get() */
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"

#ifdef CURLVERBOSE
ಧ್ರುವ ವರ್ಣ *Curl_ssh_statename(sshstate state)
{
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ names[] = {
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
  ಪ್ರತ್ಯಾಗಮ ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
ಶೂನ್ಯ Curl_ssh_set_state(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  ಯದಿ(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ವರ್ಣ *homedir, /* when SFTP is used */
                             ವರ್ಣ **path) /* returns the  allocated
                                             real path to work with */
{
  ವರ್ಣ *working_path;
  size_t working_path_len;
  ಸಂರಚನಾ dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  ಯದಿ((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    ಯದಿ(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    ಯದಿ(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    ಯದಿ(working_path_len > 2) {
      size_t len;
      ಧ್ರುವ ವರ್ಣ *p;
      ಪೂರ್ಣಾಙ್ಕ copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      ಯದಿ(len && (p[len - 1] != '/'))
        copyfrom = 2;

      ಯದಿ(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
    }
  }

  ಯದಿ(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  ಅನ್ಯಥಾ
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ಧ್ರುವ ವರ್ಣ **cpp, ವರ್ಣ **path, ಧ್ರುವ ವರ್ಣ *homedir)
{
  ಧ್ರುವ ವರ್ಣ *cp = *cpp;
  ಸಂರಚನಾ dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = ರಿಕ್ತ;
  *cpp = ರಿಕ್ತ;
  ಯದಿ(!*cp || !homedir)
    ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  ಯದಿ(*cp == '\"' || *cp == '\'') {
    ವರ್ಣ quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    ಯಾವತ್(*cp != quot) {
      ಯದಿ(!*cp) /* End of string */
        ಗಚ್ಛ fail;

      ಯದಿ(*cp == '\\') { /* Escaped characters */
        cp++;
        ಯದಿ(*cp != '\'' && *cp != '\"' && *cp != '\\')
          ಗಚ್ಛ fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      cp++;
    }
    cp++; /* pass the end quote */

    ಯದಿ(!curlx_dyn_len(&out))
      ಗಚ್ಛ fail;
  }
  ಅನ್ಯಥಾ {
    ಸಂರಚನಾ Curl_str word;
    bool content = FALSE;
    ಪೂರ್ಣಾಙ್ಕ rc;
    /* Handling for relative path - prepend home directory */
    ಯದಿ(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      ಯದಿ(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    ಯದಿ(rc) {
      ಯದಿ(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
      }
      ಅನ್ಯಥಾ ಯದಿ(!content)
        /* no path, no word, this is incorrect */
        ಗಚ್ಛ fail;
    }
    ಅನ್ಯಥಾ {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;

fail:
  curlx_dyn_free(&out);
  ಪ್ರತ್ಯಾಗಮ CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(ಸಂರಚನಾ Curl_easy *data,
                        ಧ್ರುವ ವರ್ಣ *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  ಪೂರ್ಣಾಙ್ಕ to_t;
  ಪೂರ್ಣಾಙ್ಕ from_t = curlx_str_ಸಂಖ್ಯೆ(&range, &from, CURL_OFF_T_MAX);
  ಯದಿ(from_t == STRE_OVERFLOW)
    ಪ್ರತ್ಯಾಗಮ CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (ಶೂನ್ಯ)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  ಯದಿ((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    ಪ್ರತ್ಯಾಗಮ CURLE_RANGE_ERROR;

  ಯದಿ(from_t) {
    /* no start point given, set from relative to end of file */
    ಯದಿ(!to)
      /* "-0" is not a fine range */
      ಪ್ರತ್ಯಾಗಮ CURLE_RANGE_ERROR;
    ಅನ್ಯಥಾ ಯದಿ(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  ಅನ್ಯಥಾ ಯದಿ(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    ಪ್ರತ್ಯಾಗಮ CURLE_RANGE_ERROR;
  }
  ಅನ್ಯಥಾ ಯದಿ((to_t == STRE_NO_ಅಂಕೆ) || (to >= filesize))
    to = filesize - 1;

  ಯದಿ(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    ಪ್ರತ್ಯಾಗಮ CURLE_RANGE_ERROR;
  }
  ಯದಿ((to - from) == CURL_OFF_T_MAX)
    ಪ್ರತ್ಯಾಗಮ CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ಪೂರ್ಣಾಙ್ಕ waitfor;

  ಯದಿ(!sshc || (sock == CURL_SOCKET_BAD))
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  ಯದಿ(waitfor) {
    ಪೂರ್ಣಾಙ್ಕ flags = 0;
    ಯದಿ(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    ಯದಿ(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned ಪೂರ್ಣಾಙ್ಕ)flags);
    ಪ್ರತ್ಯಾಗಮ Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  ಯದಿ(sshc->ssh_session)
    ಪ್ರತ್ಯಾಗಮ Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ssh_conn *sshc)
{
  ವರ್ಣ *home = ರಿಕ್ತ;
  ಯದಿ(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ಧ್ರುವ ವರ್ಣ *str;

    sshc->pub_ಕೀಲಿ = sshc->priv_ಕೀಲಿ = ರಿಕ್ತ;

    ಯದಿ(CURL_EASY_STR(data, STRING_SSH_PRIVATE_ಕೀಲಿ)) {
      sshc->priv_ಕೀಲಿ = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_ಕೀಲಿ));
      ಯದಿ(!sshc->priv_ಕೀಲಿ)
        ಗಚ್ಛ fail;
    }
    ಅನ್ಯಥಾ {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      ಯದಿ(home) {
        /* Try ~/.ssh first. */
        sshc->priv_ಕೀಲಿ = curl_maprintf("%s/.ssh/id_rsa", home);
        ಯದಿ(!sshc->priv_ಕೀಲಿ)
          ಗಚ್ಛ fail;
        ಅನ್ಯಥಾ ಯದಿ(curlx_stat(sshc->priv_ಕೀಲಿ, &sbuf)) {
          curlx_free(sshc->priv_ಕೀಲಿ);
          sshc->priv_ಕೀಲಿ = curl_maprintf("%s/.ssh/id_dsa", home);
          ಯದಿ(!sshc->priv_ಕೀಲಿ)
            ಗಚ್ಛ fail;
          ಅನ್ಯಥಾ ಯದಿ(curlx_stat(sshc->priv_ಕೀಲಿ, &sbuf)) {
            curlx_safefree(sshc->priv_ಕೀಲಿ);
          }
        }
        curlx_safefree(home);
      }
      ಯದಿ(!sshc->priv_ಕೀಲಿ) {
        /* Nothing found; try the current dir. */
        sshc->priv_ಕೀಲಿ = curlx_strdup("id_rsa");
        ಯದಿ(sshc->priv_ಕೀಲಿ && curlx_stat(sshc->priv_ಕೀಲಿ, &sbuf)) {
          curlx_free(sshc->priv_ಕೀಲಿ);
          sshc->priv_ಕೀಲಿ = curlx_strdup("id_dsa");
          ಯದಿ(sshc->priv_ಕೀಲಿ && curlx_stat(sshc->priv_ಕೀಲಿ, &sbuf)) {
            curlx_free(sshc->priv_ಕೀಲಿ);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_ಕೀಲಿ = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_ಕೀಲಿ);
    ಯದಿ(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_ಕೀಲಿ = curlx_strdup(str);
      ಯದಿ(!sshc->pub_ಕೀಲಿ)
        ಗಚ್ಛ fail;
    }

    {
      ಧ್ರುವ ವರ್ಣ *keypasswd = CURL_EASY_STR(data, STRING_ಕೀಲಿ_PASSWD);
      ಯದಿ(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        ಯದಿ(!sshc->passphrase)
          ಗಚ್ಛ fail;
      }
    }

    ಯದಿ(sshc->pub_ಕೀಲಿ)
      infof(data, "SSH: public key file '%s'", sshc->pub_ಕೀಲಿ);
    ಯದಿ(sshc->priv_ಕೀಲಿ)
      infof(data, "SSH: private key file '%s'", sshc->priv_ಕೀಲಿ);
    ಅನ್ಯಥಾ
      infof(data, "SSH: public key auth without private key set!");
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_ಕೀಲಿ);
  curlx_safefree(sshc->pub_ಕೀಲಿ);
  ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
