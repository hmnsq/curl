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
#شامل "curl_setup.h"
#شامل "vssh/ssh.h"

#ifdef USE_SSH

#شامل "vssh/vssh.h"
#شامل "curlx/strparse.h"
#شامل "curl_trc.h"
#شامل "escape.h"
#شامل "select.h"  /* for Curl_pollset_change() */
#شامل "url.h"  /* for Curl_conn_meta_get() */
#شامل "curlx/win32-fopen.h"

#ifdef CURLVERBOSE
ثابت نویسه *Curl_ssh_statename(sshstate state)
{
  ایستا ثابت نویسه * ثابت names[] = {
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
  بازگشت ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
تهی Curl_ssh_set_state(ساختار Curl_easy *data,
                        ساختار ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  اگر(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#وگرنه
  (تهی)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(ساختار Curl_easy *data,
                             ثابت نویسه *homedir, /* when SFTP is used */
                             نویسه **path) /* returns the  allocated
                                             real path to work with */
{
  نویسه *working_path;
  size_t working_path_len;
  ساختار dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  اگر(result)
    بازگشت result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  اگر((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    اگر(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      بازگشت CURLE_OUT_OF_MEMORY;
    }
  }
  وگرنه اگر((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    اگر(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      بازگشت CURLE_OUT_OF_MEMORY;
    }
    اگر(working_path_len > 2) {
      size_t len;
      ثابت نویسه *p;
      صحیح copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      اگر(len && (p[len - 1] != '/'))
        copyfrom = 2;

      اگر(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        بازگشت CURLE_OUT_OF_MEMORY;
      }
    }
    وگرنه {
      اگر(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        بازگشت CURLE_OUT_OF_MEMORY;
      }
    }
  }

  اگر(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  وگرنه
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  بازگشت CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ثابت نویسه **cpp, نویسه **path, ثابت نویسه *homedir)
{
  ثابت نویسه *cp = *cpp;
  ساختار dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = پوچ;
  *cpp = پوچ;
  اگر(!*cp || !homedir)
    بازگشت CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  اگر(*cp == '\"' || *cp == '\'') {
    نویسه quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    وقتی(*cp != quot) {
      اگر(!*cp) /* End of string */
        برو fail;

      اگر(*cp == '\\') { /* Escaped characters */
        cp++;
        اگر(*cp != '\'' && *cp != '\"' && *cp != '\\')
          برو fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      اگر(result)
        بازگشت result;
      cp++;
    }
    cp++; /* pass the end quote */

    اگر(!curlx_dyn_len(&out))
      برو fail;
  }
  وگرنه {
    ساختار Curl_str word;
    bool content = FALSE;
    صحیح rc;
    /* Handling for relative path - prepend home directory */
    اگر(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      اگر(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      اگر(result)
        بازگشت result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    اگر(rc) {
      اگر(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        بازگشت CURLE_TOO_LARGE;
      }
      وگرنه اگر(!content)
        /* no path, no word, this is incorrect */
        برو fail;
    }
    وگرنه {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      اگر(result)
        بازگشت result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  بازگشت CURLE_OK;

fail:
  curlx_dyn_free(&out);
  بازگشت CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(ساختار Curl_easy *data,
                        ثابت نویسه *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  صحیح to_t;
  صحیح from_t = curlx_str_عدد(&range, &from, CURL_OFF_T_MAX);
  اگر(from_t == STRE_OVERFLOW)
    بازگشت CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (تهی)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  اگر((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    بازگشت CURLE_RANGE_ERROR;

  اگر(from_t) {
    /* no start point given, set from relative to end of file */
    اگر(!to)
      /* "-0" is not a fine range */
      بازگشت CURLE_RANGE_ERROR;
    وگرنه اگر(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  وگرنه اگر(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    بازگشت CURLE_RANGE_ERROR;
  }
  وگرنه اگر((to_t == STRE_NO_رقم) || (to >= filesize))
    to = filesize - 1;

  اگر(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    بازگشت CURLE_RANGE_ERROR;
  }
  اگر((to - from) == CURL_OFF_T_MAX)
    بازگشت CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  بازگشت CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(ساختار Curl_easy *data, ساختار easy_pollset *ps)
{
  ساختار connectdata *conn = data->conn;
  ساختار ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحیح waitfor;

  اگر(!sshc || (sock == CURL_SOCKET_BAD))
    بازگشت CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  اگر(waitfor) {
    صحیح flags = 0;
    اگر(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    اگر(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned صحیح)flags);
    بازگشت Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  اگر(sshc->ssh_session)
    بازگشت Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  بازگشت CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(ساختار Curl_easy *data, ساختار ssh_conn *sshc)
{
  نویسه *home = پوچ;
  اگر(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ثابت نویسه *str;

    sshc->pub_کلید = sshc->priv_کلید = پوچ;

    اگر(CURL_EASY_STR(data, STRING_SSH_PRIVATE_کلید)) {
      sshc->priv_کلید = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_کلید));
      اگر(!sshc->priv_کلید)
        برو fail;
    }
    وگرنه {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      اگر(home) {
        /* Try ~/.ssh first. */
        sshc->priv_کلید = curl_maprintf("%s/.ssh/id_rsa", home);
        اگر(!sshc->priv_کلید)
          برو fail;
        وگرنه اگر(curlx_stat(sshc->priv_کلید, &sbuf)) {
          curlx_free(sshc->priv_کلید);
          sshc->priv_کلید = curl_maprintf("%s/.ssh/id_dsa", home);
          اگر(!sshc->priv_کلید)
            برو fail;
          وگرنه اگر(curlx_stat(sshc->priv_کلید, &sbuf)) {
            curlx_safefree(sshc->priv_کلید);
          }
        }
        curlx_safefree(home);
      }
      اگر(!sshc->priv_کلید) {
        /* Nothing found; try the current dir. */
        sshc->priv_کلید = curlx_strdup("id_rsa");
        اگر(sshc->priv_کلید && curlx_stat(sshc->priv_کلید, &sbuf)) {
          curlx_free(sshc->priv_کلید);
          sshc->priv_کلید = curlx_strdup("id_dsa");
          اگر(sshc->priv_کلید && curlx_stat(sshc->priv_کلید, &sbuf)) {
            curlx_free(sshc->priv_کلید);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_کلید = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_کلید);
    اگر(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_کلید = curlx_strdup(str);
      اگر(!sshc->pub_کلید)
        برو fail;
    }

    {
      ثابت نویسه *keypasswd = CURL_EASY_STR(data, STRING_کلید_PASSWD);
      اگر(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        اگر(!sshc->passphrase)
          برو fail;
      }
    }

    اگر(sshc->pub_کلید)
      infof(data, "SSH: public key file '%s'", sshc->pub_کلید);
    اگر(sshc->priv_کلید)
      infof(data, "SSH: private key file '%s'", sshc->priv_کلید);
    وگرنه
      infof(data, "SSH: public key auth without private key set!");
  }
  بازگشت CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_کلید);
  curlx_safefree(sshc->pub_کلید);
  بازگشت CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
