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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "vssh/ssh.h"

#ifdef USE_SSH

#अन्तर्भाव "vssh/vssh.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "select.h"  /* for Curl_pollset_change() */
#अन्तर्भाव "url.h"  /* for Curl_conn_meta_get() */
#अन्तर्भाव "curlx/win32-fopen.h"

#ifdef CURLVERBOSE
स्थिरांक अक्षर *Curl_ssh_statename(sshstate state)
{
  स्थिर स्थिरांक अक्षर * स्थिरांक names[] = {
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
  परत ((माप_प्रकार)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
शून्य Curl_ssh_set_state(संरचना Curl_easy *data,
                        संरचना ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  जर(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#अन्यथा
  (शून्य)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(संरचना Curl_easy *data,
                             स्थिरांक अक्षर *homedir, /* when SFTP is used */
                             अक्षर **path) /* returns the  allocated
                                             real path to work with */
{
  अक्षर *working_path;
  माप_प्रकार working_path_len;
  संरचना dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  जर(result)
    परत result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  जर((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!स_भेद(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    जर(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      परत CURLE_OUT_OF_MEMORY;
    }
  }
  अन्यथा जर((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!म_भेद("/~", working_path) ||
           ((working_path_len > 2) && !स_भेद(working_path, "/~/", 3)))) {
    जर(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      परत CURLE_OUT_OF_MEMORY;
    }
    जर(working_path_len > 2) {
      माप_प्रकार len;
      स्थिरांक अक्षर *p;
      पूर्णांक copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      जर(len && (p[len - 1] != '/'))
        copyfrom = 2;

      जर(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        परत CURLE_OUT_OF_MEMORY;
      }
    }
    अन्यथा {
      जर(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        परत CURLE_OUT_OF_MEMORY;
      }
    }
  }

  जर(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  अन्यथा
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  परत CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(स्थिरांक अक्षर **cpp, अक्षर **path, स्थिरांक अक्षर *homedir)
{
  स्थिरांक अक्षर *cp = *cpp;
  संरचना dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = रिक्त;
  *cpp = रिक्त;
  जर(!*cp || !homedir)
    परत CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  जर(*cp == '\"' || *cp == '\'') {
    अक्षर quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    पर्यंत(*cp != quot) {
      जर(!*cp) /* End of string */
        जा fail;

      जर(*cp == '\\') { /* Escaped characters */
        cp++;
        जर(*cp != '\'' && *cp != '\"' && *cp != '\\')
          जा fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      जर(result)
        परत result;
      cp++;
    }
    cp++; /* pass the end quote */

    जर(!curlx_dyn_len(&out))
      जा fail;
  }
  अन्यथा {
    संरचना Curl_str word;
    bool content = FALSE;
    पूर्णांक rc;
    /* Handling for relative path - prepend home directory */
    जर(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      जर(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      जर(result)
        परत result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    जर(rc) {
      जर(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        परत CURLE_TOO_LARGE;
      }
      अन्यथा जर(!content)
        /* no path, no word, this is incorrect */
        जा fail;
    }
    अन्यथा {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      जर(result)
        परत result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  परत CURLE_OK;

fail:
  curlx_dyn_free(&out);
  परत CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(संरचना Curl_easy *data,
                        स्थिरांक अक्षर *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  पूर्णांक to_t;
  पूर्णांक from_t = curlx_str_संख्या(&range, &from, CURL_OFF_T_MAX);
  जर(from_t == STRE_OVERFLOW)
    परत CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (शून्य)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  जर((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    परत CURLE_RANGE_ERROR;

  जर(from_t) {
    /* no start point given, set from relative to end of file */
    जर(!to)
      /* "-0" is not a fine range */
      परत CURLE_RANGE_ERROR;
    अन्यथा जर(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  अन्यथा जर(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    परत CURLE_RANGE_ERROR;
  }
  अन्यथा जर((to_t == STRE_NO_अंक) || (to >= filesize))
    to = filesize - 1;

  जर(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    परत CURLE_RANGE_ERROR;
  }
  जर((to - from) == CURL_OFF_T_MAX)
    परत CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  परत CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(संरचना Curl_easy *data, संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णांक waitfor;

  जर(!sshc || (sock == CURL_SOCKET_BAD))
    परत CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  जर(waitfor) {
    पूर्णांक flags = 0;
    जर(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    जर(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (अचिन्हित पूर्णांक)flags);
    परत Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  जर(sshc->ssh_session)
    परत Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  परत CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(संरचना Curl_easy *data, संरचना ssh_conn *sshc)
{
  अक्षर *home = रिक्त;
  जर(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    स्थिरांक अक्षर *str;

    sshc->pub_किल्ली = sshc->priv_किल्ली = रिक्त;

    जर(CURL_EASY_STR(data, STRING_SSH_PRIVATE_किल्ली)) {
      sshc->priv_किल्ली = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_किल्ली));
      जर(!sshc->priv_किल्ली)
        जा fail;
    }
    अन्यथा {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      जर(home) {
        /* Try ~/.ssh first. */
        sshc->priv_किल्ली = curl_maprintf("%s/.ssh/id_rsa", home);
        जर(!sshc->priv_किल्ली)
          जा fail;
        अन्यथा जर(curlx_stat(sshc->priv_किल्ली, &sbuf)) {
          curlx_free(sshc->priv_किल्ली);
          sshc->priv_किल्ली = curl_maprintf("%s/.ssh/id_dsa", home);
          जर(!sshc->priv_किल्ली)
            जा fail;
          अन्यथा जर(curlx_stat(sshc->priv_किल्ली, &sbuf)) {
            curlx_safefree(sshc->priv_किल्ली);
          }
        }
        curlx_safefree(home);
      }
      जर(!sshc->priv_किल्ली) {
        /* Nothing found; try the current dir. */
        sshc->priv_किल्ली = curlx_strdup("id_rsa");
        जर(sshc->priv_किल्ली && curlx_stat(sshc->priv_किल्ली, &sbuf)) {
          curlx_free(sshc->priv_किल्ली);
          sshc->priv_किल्ली = curlx_strdup("id_dsa");
          जर(sshc->priv_किल्ली && curlx_stat(sshc->priv_किल्ली, &sbuf)) {
            curlx_free(sshc->priv_किल्ली);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_किल्ली = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_किल्ली);
    जर(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_किल्ली = curlx_strdup(str);
      जर(!sshc->pub_किल्ली)
        जा fail;
    }

    {
      स्थिरांक अक्षर *keypasswd = CURL_EASY_STR(data, STRING_किल्ली_PASSWD);
      जर(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        जर(!sshc->passphrase)
          जा fail;
      }
    }

    जर(sshc->pub_किल्ली)
      infof(data, "SSH: public key file '%s'", sshc->pub_किल्ली);
    जर(sshc->priv_किल्ली)
      infof(data, "SSH: private key file '%s'", sshc->priv_किल्ली);
    अन्यथा
      infof(data, "SSH: public key auth without private key set!");
  }
  परत CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_किल्ली);
  curlx_safefree(sshc->pub_किल्ली);
  परत CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
