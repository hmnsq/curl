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
#ضمن "curl_setup.h"
#ضمن "vssh/ssh.h"

#ifdef USE_SSH

#ضمن "vssh/vssh.h"
#ضمن "curlx/strparse.h"
#ضمن "curl_trc.h"
#ضمن "escape.h"
#ضمن "select.h"  /* for Curl_pollset_change() */
#ضمن "url.h"  /* for Curl_conn_meta_get() */
#ضمن "curlx/win32-fopen.h"

#ifdef CURLVERBOSE
ثابت حرف *Curl_ssh_statename(sshstate state)
{
  ساكن ثابت حرف * ثابت names[] = {
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
  أرجع ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
فراغ Curl_ssh_set_state(بنية Curl_easy *data,
                        بنية ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  إذا(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#وإلا
  (فراغ)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(بنية Curl_easy *data,
                             ثابت حرف *homedir, /* when SFTP is used */
                             حرف **path) /* returns the  allocated
                                             real path to work with */
{
  حرف *working_path;
  size_t working_path_len;
  بنية dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  إذا(result)
    أرجع result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  إذا((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    إذا(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      أرجع CURLE_OUT_OF_MEMORY;
    }
  }
  وإلا إذا((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    إذا(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      أرجع CURLE_OUT_OF_MEMORY;
    }
    إذا(working_path_len > 2) {
      size_t len;
      ثابت حرف *p;
      صحيح copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      إذا(len && (p[len - 1] != '/'))
        copyfrom = 2;

      إذا(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        أرجع CURLE_OUT_OF_MEMORY;
      }
    }
    وإلا {
      إذا(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        أرجع CURLE_OUT_OF_MEMORY;
      }
    }
  }

  إذا(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  وإلا
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  أرجع CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ثابت حرف **cpp, حرف **path, ثابت حرف *homedir)
{
  ثابت حرف *cp = *cpp;
  بنية dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = عدم;
  *cpp = عدم;
  إذا(!*cp || !homedir)
    أرجع CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  إذا(*cp == '\"' || *cp == '\'') {
    حرف quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    طالما(*cp != quot) {
      إذا(!*cp) /* End of string */
        اذهب fail;

      إذا(*cp == '\\') { /* Escaped characters */
        cp++;
        إذا(*cp != '\'' && *cp != '\"' && *cp != '\\')
          اذهب fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      إذا(result)
        أرجع result;
      cp++;
    }
    cp++; /* pass the end quote */

    إذا(!curlx_dyn_len(&out))
      اذهب fail;
  }
  وإلا {
    بنية Curl_str word;
    bool content = FALSE;
    صحيح rc;
    /* Handling for relative path - prepend home directory */
    إذا(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      إذا(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      إذا(result)
        أرجع result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    إذا(rc) {
      إذا(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        أرجع CURLE_TOO_LARGE;
      }
      وإلا إذا(!content)
        /* no path, no word, this is incorrect */
        اذهب fail;
    }
    وإلا {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      إذا(result)
        أرجع result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  أرجع CURLE_OK;

fail:
  curlx_dyn_free(&out);
  أرجع CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(بنية Curl_easy *data,
                        ثابت حرف *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  صحيح to_t;
  صحيح from_t = curlx_str_نومبور(&range, &from, CURL_OFF_T_MAX);
  إذا(from_t == STRE_OVERFLOW)
    أرجع CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (فراغ)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  إذا((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    أرجع CURLE_RANGE_ERROR;

  إذا(from_t) {
    /* no start point given, set from relative to end of file */
    إذا(!to)
      /* "-0" is not a fine range */
      أرجع CURLE_RANGE_ERROR;
    وإلا إذا(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  وإلا إذا(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    أرجع CURLE_RANGE_ERROR;
  }
  وإلا إذا((to_t == STRE_NO_اڠک) || (to >= filesize))
    to = filesize - 1;

  إذا(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    أرجع CURLE_RANGE_ERROR;
  }
  إذا((to - from) == CURL_OFF_T_MAX)
    أرجع CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  أرجع CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(بنية Curl_easy *data, بنية easy_pollset *ps)
{
  بنية connectdata *conn = data->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح waitfor;

  إذا(!sshc || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  إذا(waitfor) {
    صحيح flags = 0;
    إذا(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    إذا(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned صحيح)flags);
    أرجع Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  إذا(sshc->ssh_session)
    أرجع Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  أرجع CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(بنية Curl_easy *data, بنية ssh_conn *sshc)
{
  حرف *home = عدم;
  إذا(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ثابت حرف *str;

    sshc->pub_کونچي = sshc->priv_کونچي = عدم;

    إذا(CURL_EASY_STR(data, STRING_SSH_PRIVATE_کونچي)) {
      sshc->priv_کونچي = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_کونچي));
      إذا(!sshc->priv_کونچي)
        اذهب fail;
    }
    وإلا {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      إذا(home) {
        /* Try ~/.ssh first. */
        sshc->priv_کونچي = curl_maprintf("%s/.ssh/id_rsa", home);
        إذا(!sshc->priv_کونچي)
          اذهب fail;
        وإلا إذا(curlx_stat(sshc->priv_کونچي, &sbuf)) {
          curlx_free(sshc->priv_کونچي);
          sshc->priv_کونچي = curl_maprintf("%s/.ssh/id_dsa", home);
          إذا(!sshc->priv_کونچي)
            اذهب fail;
          وإلا إذا(curlx_stat(sshc->priv_کونچي, &sbuf)) {
            curlx_safefree(sshc->priv_کونچي);
          }
        }
        curlx_safefree(home);
      }
      إذا(!sshc->priv_کونچي) {
        /* Nothing found; try the current dir. */
        sshc->priv_کونچي = curlx_strdup("id_rsa");
        إذا(sshc->priv_کونچي && curlx_stat(sshc->priv_کونچي, &sbuf)) {
          curlx_free(sshc->priv_کونچي);
          sshc->priv_کونچي = curlx_strdup("id_dsa");
          إذا(sshc->priv_کونچي && curlx_stat(sshc->priv_کونچي, &sbuf)) {
            curlx_free(sshc->priv_کونچي);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_کونچي = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_کونچي);
    إذا(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_کونچي = curlx_strdup(str);
      إذا(!sshc->pub_کونچي)
        اذهب fail;
    }

    {
      ثابت حرف *keypasswd = CURL_EASY_STR(data, STRING_کونچي_PASSWD);
      إذا(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        إذا(!sshc->passphrase)
          اذهب fail;
      }
    }

    إذا(sshc->pub_کونچي)
      infof(data, "SSH: public key file '%s'", sshc->pub_کونچي);
    إذا(sshc->priv_کونچي)
      infof(data, "SSH: private key file '%s'", sshc->priv_کونچي);
    وإلا
      infof(data, "SSH: public key auth without private key set!");
  }
  أرجع CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_کونچي);
  curlx_safefree(sshc->pub_کونچي);
  أرجع CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
