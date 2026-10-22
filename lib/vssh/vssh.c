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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "vssh/ssh.h"

#ifdef USE_SSH

#ଅନ୍ତର୍ଭାଵ "vssh/vssh.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "select.h"  /* for Curl_pollset_change() */
#ଅନ୍ତର୍ଭାଵ "url.h"  /* for Curl_conn_meta_get() */
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"

#ifdef CURLVERBOSE
ଅଚଳ ଅକ୍ଷର *Curl_ssh_statename(sshstate state)
{
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ names[] = {
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
  ଫେରନ୍ତୁ ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
ଶୂନ୍ୟ Curl_ssh_set_state(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  ଯଦି(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ଅକ୍ଷର *homedir, /* when SFTP is used */
                             ଅକ୍ଷର **path) /* returns the  allocated
                                             real path to work with */
{
  ଅକ୍ଷର *working_path;
  size_t working_path_len;
  ସଂରଚନା dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  ଯଦି((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    ଯଦି(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
  }
  ଅନ୍ୟଥା ଯଦି((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    ଯଦି(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    ଯଦି(working_path_len > 2) {
      size_t len;
      ଅଚଳ ଅକ୍ଷର *p;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      ଯଦି(len && (p[len - 1] != '/'))
        copyfrom = 2;

      ଯଦି(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
    }
    ଅନ୍ୟଥା {
      ଯଦି(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
    }
  }

  ଯଦି(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  ଅନ୍ୟଥା
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  ଫେରନ୍ତୁ CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ଅଚଳ ଅକ୍ଷର **cpp, ଅକ୍ଷର **path, ଅଚଳ ଅକ୍ଷର *homedir)
{
  ଅଚଳ ଅକ୍ଷର *cp = *cpp;
  ସଂରଚନା dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = ଖାଲି;
  *cpp = ଖାଲି;
  ଯଦି(!*cp || !homedir)
    ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  ଯଦି(*cp == '\"' || *cp == '\'') {
    ଅକ୍ଷର quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    ଯେତେବେଳେ(*cp != quot) {
      ଯଦି(!*cp) /* End of string */
        ଯାଅ fail;

      ଯଦି(*cp == '\\') { /* Escaped characters */
        cp++;
        ଯଦି(*cp != '\'' && *cp != '\"' && *cp != '\\')
          ଯାଅ fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      cp++;
    }
    cp++; /* pass the end quote */

    ଯଦି(!curlx_dyn_len(&out))
      ଯାଅ fail;
  }
  ଅନ୍ୟଥା {
    ସଂରଚନା Curl_str word;
    bool content = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
    /* Handling for relative path - prepend home directory */
    ଯଦି(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      ଯଦି(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    ଯଦି(rc) {
      ଯଦି(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        ଫେରନ୍ତୁ CURLE_TOO_LARGE;
      }
      ଅନ୍ୟଥା ଯଦି(!content)
        /* no path, no word, this is incorrect */
        ଯାଅ fail;
    }
    ଅନ୍ୟଥା {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  ଫେରନ୍ତୁ CURLE_OK;

fail:
  curlx_dyn_free(&out);
  ଫେରନ୍ତୁ CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଅକ୍ଷର *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା to_t;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା from_t = curlx_str_ସଂଖ୍ୟା(&range, &from, CURL_OFF_T_MAX);
  ଯଦି(from_t == STRE_OVERFLOW)
    ଫେରନ୍ତୁ CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (ଶୂନ୍ୟ)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  ଯଦି((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    ଫେରନ୍ତୁ CURLE_RANGE_ERROR;

  ଯଦି(from_t) {
    /* no start point given, set from relative to end of file */
    ଯଦି(!to)
      /* "-0" is not a fine range */
      ଫେରନ୍ତୁ CURLE_RANGE_ERROR;
    ଅନ୍ୟଥା ଯଦି(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  ଅନ୍ୟଥା ଯଦି(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    ଫେରନ୍ତୁ CURLE_RANGE_ERROR;
  }
  ଅନ୍ୟଥା ଯଦି((to_t == STRE_NO_ଅଙ୍କ) || (to >= filesize))
    to = filesize - 1;

  ଯଦି(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    ଫେରନ୍ତୁ CURLE_RANGE_ERROR;
  }
  ଯଦି((to - from) == CURL_OFF_T_MAX)
    ଫେରନ୍ତୁ CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  ଫେରନ୍ତୁ CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(ସଂରଚନା Curl_easy *data, ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା waitfor;

  ଯଦି(!sshc || (sock == CURL_SOCKET_BAD))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  ଯଦି(waitfor) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = 0;
    ଯଦି(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    ଯଦି(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)flags);
    ଫେରନ୍ତୁ Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  ଯଦି(sshc->ssh_session)
    ଫେରନ୍ତୁ Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(ସଂରଚନା Curl_easy *data, ସଂରଚନା ssh_conn *sshc)
{
  ଅକ୍ଷର *home = ଖାଲି;
  ଯଦି(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ଅଚଳ ଅକ୍ଷର *str;

    sshc->pub_ଚାବି = sshc->priv_ଚାବି = ଖାଲି;

    ଯଦି(CURL_EASY_STR(data, STRING_SSH_PRIVATE_ଚାବି)) {
      sshc->priv_ଚାବି = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_ଚାବି));
      ଯଦି(!sshc->priv_ଚାବି)
        ଯାଅ fail;
    }
    ଅନ୍ୟଥା {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      ଯଦି(home) {
        /* Try ~/.ssh first. */
        sshc->priv_ଚାବି = curl_maprintf("%s/.ssh/id_rsa", home);
        ଯଦି(!sshc->priv_ଚାବି)
          ଯାଅ fail;
        ଅନ୍ୟଥା ଯଦି(curlx_stat(sshc->priv_ଚାବି, &sbuf)) {
          curlx_free(sshc->priv_ଚାବି);
          sshc->priv_ଚାବି = curl_maprintf("%s/.ssh/id_dsa", home);
          ଯଦି(!sshc->priv_ଚାବି)
            ଯାଅ fail;
          ଅନ୍ୟଥା ଯଦି(curlx_stat(sshc->priv_ଚାବି, &sbuf)) {
            curlx_safefree(sshc->priv_ଚାବି);
          }
        }
        curlx_safefree(home);
      }
      ଯଦି(!sshc->priv_ଚାବି) {
        /* Nothing found; try the current dir. */
        sshc->priv_ଚାବି = curlx_strdup("id_rsa");
        ଯଦି(sshc->priv_ଚାବି && curlx_stat(sshc->priv_ଚାବି, &sbuf)) {
          curlx_free(sshc->priv_ଚାବି);
          sshc->priv_ଚାବି = curlx_strdup("id_dsa");
          ଯଦି(sshc->priv_ଚାବି && curlx_stat(sshc->priv_ଚାବି, &sbuf)) {
            curlx_free(sshc->priv_ଚାବି);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_ଚାବି = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_ଚାବି);
    ଯଦି(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_ଚାବି = curlx_strdup(str);
      ଯଦି(!sshc->pub_ଚାବି)
        ଯାଅ fail;
    }

    {
      ଅଚଳ ଅକ୍ଷର *keypasswd = CURL_EASY_STR(data, STRING_ଚାବି_PASSWD);
      ଯଦି(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        ଯଦି(!sshc->passphrase)
          ଯାଅ fail;
      }
    }

    ଯଦି(sshc->pub_ଚାବି)
      infof(data, "SSH: public key file '%s'", sshc->pub_ଚାବି);
    ଯଦି(sshc->priv_ଚାବି)
      infof(data, "SSH: private key file '%s'", sshc->priv_ଚାବି);
    ଅନ୍ୟଥା
      infof(data, "SSH: public key auth without private key set!");
  }
  ଫେରନ୍ତୁ CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_ଚାବି);
  curlx_safefree(sshc->pub_ଚାବି);
  ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
