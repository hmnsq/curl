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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "vssh/ssh.h"

#ifdef USE_SSH

#অন্তর্ভাব "vssh/vssh.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "select.h"  /* for Curl_pollset_change() */
#অন্তর্ভাব "url.h"  /* for Curl_conn_meta_get() */
#অন্তর্ভাব "curlx/win32-fopen.h"

#ifdef CURLVERBOSE
ধ্রুব বর্ণ *Curl_ssh_statename(sshstate state)
{
  স্থির ধ্রুব বর্ণ * ধ্রুব names[] = {
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
  প্রত্যাগম ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
শূন্য Curl_ssh_set_state(সংরচনা Curl_easy *data,
                        সংরচনা ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  যদি(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#অন্যথা
  (শূন্য)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(সংরচনা Curl_easy *data,
                             ধ্রুব বর্ণ *homedir, /* when SFTP is used */
                             বর্ণ **path) /* returns the  allocated
                                             real path to work with */
{
  বর্ণ *working_path;
  size_t working_path_len;
  সংরচনা dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  যদি(result)
    প্রত্যাগম result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  যদি((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    যদি(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
  }
  অন্যথা যদি((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    যদি(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
    যদি(working_path_len > 2) {
      size_t len;
      ধ্রুব বর্ণ *p;
      পূর্ণাঙ্ক copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      যদি(len && (p[len - 1] != '/'))
        copyfrom = 2;

      যদি(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }
    }
    অন্যথা {
      যদি(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      }
    }
  }

  যদি(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  অন্যথা
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  প্রত্যাগম CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ধ্রুব বর্ণ **cpp, বর্ণ **path, ধ্রুব বর্ণ *homedir)
{
  ধ্রুব বর্ণ *cp = *cpp;
  সংরচনা dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = রিক্ত;
  *cpp = রিক্ত;
  যদি(!*cp || !homedir)
    প্রত্যাগম CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  যদি(*cp == '\"' || *cp == '\'') {
    বর্ণ quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    যাবত্(*cp != quot) {
      যদি(!*cp) /* End of string */
        গচ্ছ fail;

      যদি(*cp == '\\') { /* Escaped characters */
        cp++;
        যদি(*cp != '\'' && *cp != '\"' && *cp != '\\')
          গচ্ছ fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      যদি(result)
        প্রত্যাগম result;
      cp++;
    }
    cp++; /* pass the end quote */

    যদি(!curlx_dyn_len(&out))
      গচ্ছ fail;
  }
  অন্যথা {
    সংরচনা Curl_str word;
    bool content = FALSE;
    পূর্ণাঙ্ক rc;
    /* Handling for relative path - prepend home directory */
    যদি(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      যদি(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      যদি(result)
        প্রত্যাগম result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    যদি(rc) {
      যদি(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        প্রত্যাগম CURLE_TOO_LARGE;
      }
      অন্যথা যদি(!content)
        /* no path, no word, this is incorrect */
        গচ্ছ fail;
    }
    অন্যথা {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      যদি(result)
        প্রত্যাগম result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  প্রত্যাগম CURLE_OK;

fail:
  curlx_dyn_free(&out);
  প্রত্যাগম CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(সংরচনা Curl_easy *data,
                        ধ্রুব বর্ণ *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  পূর্ণাঙ্ক to_t;
  পূর্ণাঙ্ক from_t = curlx_str_সংখ্যা(&range, &from, CURL_OFF_T_MAX);
  যদি(from_t == STRE_OVERFLOW)
    প্রত্যাগম CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (শূন্য)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  যদি((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    প্রত্যাগম CURLE_RANGE_ERROR;

  যদি(from_t) {
    /* no start point given, set from relative to end of file */
    যদি(!to)
      /* "-0" is not a fine range */
      প্রত্যাগম CURLE_RANGE_ERROR;
    অন্যথা যদি(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  অন্যথা যদি(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    প্রত্যাগম CURLE_RANGE_ERROR;
  }
  অন্যথা যদি((to_t == STRE_NO_অঙ্ক) || (to >= filesize))
    to = filesize - 1;

  যদি(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    প্রত্যাগম CURLE_RANGE_ERROR;
  }
  যদি((to - from) == CURL_OFF_T_MAX)
    প্রত্যাগম CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  প্রত্যাগম CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(সংরচনা Curl_easy *data, সংরচনা easy_pollset *ps)
{
  সংরচনা connectdata *conn = data->conn;
  সংরচনা ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  পূর্ণাঙ্ক waitfor;

  যদি(!sshc || (sock == CURL_SOCKET_BAD))
    প্রত্যাগম CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  যদি(waitfor) {
    পূর্ণাঙ্ক flags = 0;
    যদি(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    যদি(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned পূর্ণাঙ্ক)flags);
    প্রত্যাগম Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  যদি(sshc->ssh_session)
    প্রত্যাগম Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(সংরচনা Curl_easy *data, সংরচনা ssh_conn *sshc)
{
  বর্ণ *home = রিক্ত;
  যদি(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ধ্রুব বর্ণ *str;

    sshc->pub_চাবি = sshc->priv_চাবি = রিক্ত;

    যদি(CURL_EASY_STR(data, STRING_SSH_PRIVATE_চাবি)) {
      sshc->priv_চাবি = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_চাবি));
      যদি(!sshc->priv_চাবি)
        গচ্ছ fail;
    }
    অন্যথা {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      যদি(home) {
        /* Try ~/.ssh first. */
        sshc->priv_চাবি = curl_maprintf("%s/.ssh/id_rsa", home);
        যদি(!sshc->priv_চাবি)
          গচ্ছ fail;
        অন্যথা যদি(curlx_stat(sshc->priv_চাবি, &sbuf)) {
          curlx_free(sshc->priv_চাবি);
          sshc->priv_চাবি = curl_maprintf("%s/.ssh/id_dsa", home);
          যদি(!sshc->priv_চাবি)
            গচ্ছ fail;
          অন্যথা যদি(curlx_stat(sshc->priv_চাবি, &sbuf)) {
            curlx_safefree(sshc->priv_চাবি);
          }
        }
        curlx_safefree(home);
      }
      যদি(!sshc->priv_চাবি) {
        /* Nothing found; try the current dir. */
        sshc->priv_চাবি = curlx_strdup("id_rsa");
        যদি(sshc->priv_চাবি && curlx_stat(sshc->priv_চাবি, &sbuf)) {
          curlx_free(sshc->priv_চাবি);
          sshc->priv_চাবি = curlx_strdup("id_dsa");
          যদি(sshc->priv_চাবি && curlx_stat(sshc->priv_চাবি, &sbuf)) {
            curlx_free(sshc->priv_চাবি);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_চাবি = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_চাবি);
    যদি(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_চাবি = curlx_strdup(str);
      যদি(!sshc->pub_চাবি)
        গচ্ছ fail;
    }

    {
      ধ্রুব বর্ণ *keypasswd = CURL_EASY_STR(data, STRING_চাবি_PASSWD);
      যদি(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        যদি(!sshc->passphrase)
          গচ্ছ fail;
      }
    }

    যদি(sshc->pub_চাবি)
      infof(data, "SSH: public key file '%s'", sshc->pub_চাবি);
    যদি(sshc->priv_চাবি)
      infof(data, "SSH: private key file '%s'", sshc->priv_চাবি);
    অন্যথা
      infof(data, "SSH: public key auth without private key set!");
  }
  প্রত্যাগম CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_চাবি);
  curlx_safefree(sshc->pub_চাবি);
  প্রত্যাগম CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
