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
ध्रुव वर्ण *Curl_ssh_statename(sshstate state)
{
  स्थिर ध्रुव वर्ण * ध्रुव names[] = {
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
  प्रत्यागम ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
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
  जइ(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#अण्णहा
  (शून्य)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(संरचना Curl_easy *data,
                             ध्रुव वर्ण *homedir, /* when SFTP is used */
                             वर्ण **path) /* returns the  allocated
                                             real path to work with */
{
  वर्ण *working_path;
  size_t working_path_len;
  संरचना dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  जइ(result)
    प्रत्यागम result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  जइ((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    जइ(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
  }
  अण्णहा जइ((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    जइ(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
    जइ(working_path_len > 2) {
      size_t len;
      ध्रुव वर्ण *p;
      पूर्णाङ्क copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      जइ(len && (p[len - 1] != '/'))
        copyfrom = 2;

      जइ(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
    }
    अण्णहा {
      जइ(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      }
    }
  }

  जइ(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  अण्णहा
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  प्रत्यागम CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ध्रुव वर्ण **cpp, वर्ण **path, ध्रुव वर्ण *homedir)
{
  ध्रुव वर्ण *cp = *cpp;
  संरचना dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = रिक्त;
  *cpp = रिक्त;
  जइ(!*cp || !homedir)
    प्रत्यागम CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  जइ(*cp == '\"' || *cp == '\'') {
    वर्ण quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    जाव(*cp != quot) {
      जइ(!*cp) /* End of string */
        गच्छ fail;

      जइ(*cp == '\\') { /* Escaped characters */
        cp++;
        जइ(*cp != '\'' && *cp != '\"' && *cp != '\\')
          गच्छ fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      जइ(result)
        प्रत्यागम result;
      cp++;
    }
    cp++; /* pass the end quote */

    जइ(!curlx_dyn_len(&out))
      गच्छ fail;
  }
  अण्णहा {
    संरचना Curl_str word;
    bool content = FALSE;
    पूर्णाङ्क rc;
    /* Handling for relative path - prepend home directory */
    जइ(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      जइ(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      जइ(result)
        प्रत्यागम result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    जइ(rc) {
      जइ(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        प्रत्यागम CURLE_TOO_LARGE;
      }
      अण्णहा जइ(!content)
        /* no path, no word, this is incorrect */
        गच्छ fail;
    }
    अण्णहा {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      जइ(result)
        प्रत्यागम result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  प्रत्यागम CURLE_OK;

fail:
  curlx_dyn_free(&out);
  प्रत्यागम CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(संरचना Curl_easy *data,
                        ध्रुव वर्ण *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  पूर्णाङ्क to_t;
  पूर्णाङ्क from_t = curlx_str_सङ्ख्या(&range, &from, CURL_OFF_T_MAX);
  जइ(from_t == STRE_OVERFLOW)
    प्रत्यागम CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (शून्य)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  जइ((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    प्रत्यागम CURLE_RANGE_ERROR;

  जइ(from_t) {
    /* no start point given, set from relative to end of file */
    जइ(!to)
      /* "-0" is not a fine range */
      प्रत्यागम CURLE_RANGE_ERROR;
    अण्णहा जइ(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  अण्णहा जइ(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    प्रत्यागम CURLE_RANGE_ERROR;
  }
  अण्णहा जइ((to_t == STRE_NO_अङ्कः) || (to >= filesize))
    to = filesize - 1;

  जइ(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    प्रत्यागम CURLE_RANGE_ERROR;
  }
  जइ((to - from) == CURL_OFF_T_MAX)
    प्रत्यागम CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  प्रत्यागम CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(संरचना Curl_easy *data, संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णाङ्क waitfor;

  जइ(!sshc || (sock == CURL_SOCKET_BAD))
    प्रत्यागम CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  जइ(waitfor) {
    पूर्णाङ्क flags = 0;
    जइ(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    जइ(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned पूर्णाङ्क)flags);
    प्रत्यागम Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  जइ(sshc->ssh_session)
    प्रत्यागम Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(संरचना Curl_easy *data, संरचना ssh_conn *sshc)
{
  वर्ण *home = रिक्त;
  जइ(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ध्रुव वर्ण *str;

    sshc->pub_कुञ्चिका = sshc->priv_कुञ्चिका = रिक्त;

    जइ(CURL_EASY_STR(data, STRING_SSH_PRIVATE_कुञ्चिका)) {
      sshc->priv_कुञ्चिका = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_कुञ्चिका));
      जइ(!sshc->priv_कुञ्चिका)
        गच्छ fail;
    }
    अण्णहा {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      जइ(home) {
        /* Try ~/.ssh first. */
        sshc->priv_कुञ्चिका = curl_maprintf("%s/.ssh/id_rsa", home);
        जइ(!sshc->priv_कुञ्चिका)
          गच्छ fail;
        अण्णहा जइ(curlx_stat(sshc->priv_कुञ्चिका, &sbuf)) {
          curlx_free(sshc->priv_कुञ्चिका);
          sshc->priv_कुञ्चिका = curl_maprintf("%s/.ssh/id_dsa", home);
          जइ(!sshc->priv_कुञ्चिका)
            गच्छ fail;
          अण्णहा जइ(curlx_stat(sshc->priv_कुञ्चिका, &sbuf)) {
            curlx_safefree(sshc->priv_कुञ्चिका);
          }
        }
        curlx_safefree(home);
      }
      जइ(!sshc->priv_कुञ्चिका) {
        /* Nothing found; try the current dir. */
        sshc->priv_कुञ्चिका = curlx_strdup("id_rsa");
        जइ(sshc->priv_कुञ्चिका && curlx_stat(sshc->priv_कुञ्चिका, &sbuf)) {
          curlx_free(sshc->priv_कुञ्चिका);
          sshc->priv_कुञ्चिका = curlx_strdup("id_dsa");
          जइ(sshc->priv_कुञ्चिका && curlx_stat(sshc->priv_कुञ्चिका, &sbuf)) {
            curlx_free(sshc->priv_कुञ्चिका);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_कुञ्चिका = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_कुञ्चिका);
    जइ(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_कुञ्चिका = curlx_strdup(str);
      जइ(!sshc->pub_कुञ्चिका)
        गच्छ fail;
    }

    {
      ध्रुव वर्ण *keypasswd = CURL_EASY_STR(data, STRING_कुञ्चिका_PASSWD);
      जइ(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        जइ(!sshc->passphrase)
          गच्छ fail;
      }
    }

    जइ(sshc->pub_कुञ्चिका)
      infof(data, "SSH: public key file '%s'", sshc->pub_कुञ्चिका);
    जइ(sshc->priv_कुञ्चिका)
      infof(data, "SSH: private key file '%s'", sshc->priv_कुञ्चिका);
    अण्णहा
      infof(data, "SSH: public key auth without private key set!");
  }
  प्रत्यागम CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_कुञ्चिका);
  curlx_safefree(sshc->pub_कुञ्चिका);
  प्रत्यागम CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
