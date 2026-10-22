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
#समावेश "curl_setup.h"
#समावेश "vssh/ssh.h"

#अगर_घोषित USE_SSH

#समावेश "vssh/vssh.h"
#समावेश "curlx/strparse.h"
#समावेश "curl_trc.h"
#समावेश "escape.h"
#समावेश "select.h"  /* for Curl_pollset_change() */
#समावेश "url.h"  /* for Curl_conn_meta_get() */
#समावेश "curlx/win32-fopen.h"

#अगर_घोषित CURLVERBOSE
स्थिर अक्षर *Curl_ssh_statename(sshstate state)
{
  अटल स्थिर अक्षर * स्थिर names[] = {
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
  वापस ((माप_प्रकार)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#पूर्ण_अगर /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
व्योम Curl_ssh_set_state(काष्ठा Curl_easy *तथ्य,
                        काष्ठा ssh_conn *sshc,
                        sshstate nowstate)
{
#अगर_घोषित CURLVERBOSE
  अगर(sshc->state != nowstate) {
    CURL_TRC_SSH(तथ्य, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#अन्यथा
  (व्योम)तथ्य;
#पूर्ण_अगर
  sshc->state = nowstate;
}

#घोषणा MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(काष्ठा Curl_easy *तथ्य,
                             स्थिर अक्षर *homedir, /* when SFTP is used */
                             अक्षर **path) /* returns the  allocated
                                             real path to work with */
{
  अक्षर *working_path;
  माप_प्रकार working_path_len;
  काष्ठा dynbuf npath;
  CURLcode परिणाम =
    Curl_urldecode(तथ्य->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  अगर(परिणाम)
    वापस परिणाम;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  अगर((तथ्य->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!स_भेद(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    अगर(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      वापस CURLE_OUT_OF_MEMORY;
    }
  }
  अन्यथा अगर((तथ्य->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!म_भेद("/~", working_path) ||
           ((working_path_len > 2) && !स_भेद(working_path, "/~/", 3)))) {
    अगर(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      वापस CURLE_OUT_OF_MEMORY;
    }
    अगर(working_path_len > 2) {
      माप_प्रकार len;
      स्थिर अक्षर *p;
      पूर्णांक copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      अगर(len && (p[len - 1] != '/'))
        copyfrom = 2;

      अगर(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        वापस CURLE_OUT_OF_MEMORY;
      }
    }
    अन्यथा {
      अगर(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        वापस CURLE_OUT_OF_MEMORY;
      }
    }
  }

  अगर(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  अन्यथा
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  वापस CURLE_OK;
}

#घोषणा MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(स्थिर अक्षर **cpp, अक्षर **path, स्थिर अक्षर *homedir)
{
  स्थिर अक्षर *cp = *cpp;
  काष्ठा dynbuf out;
  CURLcode परिणाम;

  DEBUGASSERT(homedir);
  *path = शून्य;
  *cpp = शून्य;
  अगर(!*cp || !homedir)
    वापस CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  अगर(*cp == '\"' || *cp == '\'') {
    अक्षर quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    जबतक(*cp != quot) {
      अगर(!*cp) /* End of string */
        जाओ fail;

      अगर(*cp == '\\') { /* Escaped characters */
        cp++;
        अगर(*cp != '\'' && *cp != '\"' && *cp != '\\')
          जाओ fail;
      }
      परिणाम = curlx_dyn_addn(&out, cp, 1);
      अगर(परिणाम)
        वापस परिणाम;
      cp++;
    }
    cp++; /* pass the end quote */

    अगर(!curlx_dyn_len(&out))
      जाओ fail;
  }
  अन्यथा {
    काष्ठा Curl_str word;
    bool content = FALSE;
    पूर्णांक rc;
    /* Handling for relative path - prepend home directory */
    अगर(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      परिणाम = curlx_dyn_add(&out, homedir);
      अगर(!परिणाम)
        परिणाम = curlx_dyn_addn(&out, "/", 1);
      अगर(परिणाम)
        वापस परिणाम;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    अगर(rc) {
      अगर(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        वापस CURLE_TOO_LARGE;
      }
      अन्यथा अगर(!content)
        /* no path, no word, this is incorrect */
        जाओ fail;
    }
    अन्यथा {
      /* append the word */
      परिणाम = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      अगर(परिणाम)
        वापस परिणाम;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  वापस CURLE_OK;

fail:
  curlx_dyn_free(&out);
  वापस CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(काष्ठा Curl_easy *तथ्य,
                        स्थिर अक्षर *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  पूर्णांक to_t;
  पूर्णांक from_t = curlx_str_संख्या(&range, &from, CURL_OFF_T_MAX);
  अगर(from_t == STRE_OVERFLOW)
    वापस CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (व्योम)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  अगर((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    वापस CURLE_RANGE_ERROR;

  अगर(from_t) {
    /* no start point given, set from relative to end of file */
    अगर(!to)
      /* "-0" is not a fine range */
      वापस CURLE_RANGE_ERROR;
    अन्यथा अगर(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  अन्यथा अगर(from > filesize) {
    failf(तथ्य, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    वापस CURLE_RANGE_ERROR;
  }
  अन्यथा अगर((to_t == STRE_NO_अंक) || (to >= filesize))
    to = filesize - 1;

  अगर(from > to) {
    failf(तथ्य, "Bad range: start offset larger than end offset");
    वापस CURLE_RANGE_ERROR;
  }
  अगर((to - from) == CURL_OFF_T_MAX)
    वापस CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  वापस CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(काष्ठा Curl_easy *तथ्य, काष्ठा easy_pollset *ps)
{
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णांक waitfor;

  अगर(!sshc || (sock == CURL_SOCKET_BAD))
    वापस CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : तथ्य->req.io_flags;
  अगर(waitfor) {
    पूर्णांक flags = 0;
    अगर(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    अगर(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(तथ्य, "pollset, flags=%x", (अचिन्हित पूर्णांक)flags);
    वापस Curl_pollset_change(तथ्य, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  अगर(sshc->ssh_session)
    वापस Curl_pollset_change(तथ्य, ps, sock, CURL_POLL_IN, 0);
  वापस CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(काष्ठा Curl_easy *तथ्य, काष्ठा ssh_conn *sshc)
{
  अक्षर *home = शून्य;
  अगर(तथ्य->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    स्थिर अक्षर *str;

    sshc->pub_कुंजी = sshc->priv_कुंजी = शून्य;

    अगर(CURL_EASY_STR(तथ्य, STRING_SSH_PRIVATE_कुंजी)) {
      sshc->priv_कुंजी = curlx_strdup(
        CURL_EASY_STR(तथ्य, STRING_SSH_PRIVATE_कुंजी));
      अगर(!sshc->priv_कुंजी)
        जाओ fail;
    }
    अन्यथा {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      अगर(home) {
        /* Try ~/.ssh first. */
        sshc->priv_कुंजी = curl_maprintf("%s/.ssh/id_rsa", home);
        अगर(!sshc->priv_कुंजी)
          जाओ fail;
        अन्यथा अगर(curlx_stat(sshc->priv_कुंजी, &sbuf)) {
          curlx_free(sshc->priv_कुंजी);
          sshc->priv_कुंजी = curl_maprintf("%s/.ssh/id_dsa", home);
          अगर(!sshc->priv_कुंजी)
            जाओ fail;
          अन्यथा अगर(curlx_stat(sshc->priv_कुंजी, &sbuf)) {
            curlx_safefree(sshc->priv_कुंजी);
          }
        }
        curlx_safefree(home);
      }
      अगर(!sshc->priv_कुंजी) {
        /* Nothing found; try the current dir. */
        sshc->priv_कुंजी = curlx_strdup("id_rsa");
        अगर(sshc->priv_कुंजी && curlx_stat(sshc->priv_कुंजी, &sbuf)) {
          curlx_free(sshc->priv_कुंजी);
          sshc->priv_कुंजी = curlx_strdup("id_dsa");
          अगर(sshc->priv_कुंजी && curlx_stat(sshc->priv_कुंजी, &sbuf)) {
            curlx_free(sshc->priv_कुंजी);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_कुंजी = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(तथ्य, STRING_SSH_PUBLIC_कुंजी);
    अगर(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_कुंजी = curlx_strdup(str);
      अगर(!sshc->pub_कुंजी)
        जाओ fail;
    }

    {
      स्थिर अक्षर *keypasswd = CURL_EASY_STR(तथ्य, STRING_कुंजी_PASSWD);
      अगर(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        अगर(!sshc->passphrase)
          जाओ fail;
      }
    }

    अगर(sshc->pub_कुंजी)
      infof(तथ्य, "SSH: public key file '%s'", sshc->pub_कुंजी);
    अगर(sshc->priv_कुंजी)
      infof(तथ्य, "SSH: private key file '%s'", sshc->priv_कुंजी);
    अन्यथा
      infof(तथ्य, "SSH: public key auth without private key set!");
  }
  वापस CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_कुंजी);
  curlx_safefree(sshc->pub_कुंजी);
  वापस CURLE_OUT_OF_MEMORY;
}

#पूर्ण_अगर /* USE_SSH */
