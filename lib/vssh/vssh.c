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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "vssh/ssh.h"

#ifdef USE_SSH

#અન્તર્ભાવ "vssh/vssh.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "select.h"  /* for Curl_pollset_change() */
#અન્તર્ભાવ "url.h"  /* for Curl_conn_meta_get() */
#અન્તર્ભાવ "curlx/win32-fopen.h"

#ifdef CURLVERBOSE
અચલ અક્ષર *Curl_ssh_statename(sshstate state)
{
  સ્થિર અચલ અક્ષર * અચલ names[] = {
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
  પાછા ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
શૂન્ય Curl_ssh_set_state(સંરચના Curl_easy *data,
                        સંરચના ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  જો(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#નહીંતર
  (શૂન્ય)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(સંરચના Curl_easy *data,
                             અચલ અક્ષર *homedir, /* when SFTP is used */
                             અક્ષર **path) /* returns the  allocated
                                             real path to work with */
{
  અક્ષર *working_path;
  size_t working_path_len;
  સંરચના dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  જો(result)
    પાછા result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  જો((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    જો(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      પાછા CURLE_OUT_OF_MEMORY;
    }
  }
  નહીંતર જો((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    જો(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      પાછા CURLE_OUT_OF_MEMORY;
    }
    જો(working_path_len > 2) {
      size_t len;
      અચલ અક્ષર *p;
      પૂર્ણાંક copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      જો(len && (p[len - 1] != '/'))
        copyfrom = 2;

      જો(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        પાછા CURLE_OUT_OF_MEMORY;
      }
    }
    નહીંતર {
      જો(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        પાછા CURLE_OUT_OF_MEMORY;
      }
    }
  }

  જો(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  નહીંતર
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  પાછા CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(અચલ અક્ષર **cpp, અક્ષર **path, અચલ અક્ષર *homedir)
{
  અચલ અક્ષર *cp = *cpp;
  સંરચના dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = ખાલી;
  *cpp = ખાલી;
  જો(!*cp || !homedir)
    પાછા CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  જો(*cp == '\"' || *cp == '\'') {
    અક્ષર quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    જ્યારે(*cp != quot) {
      જો(!*cp) /* End of string */
        જાઓ fail;

      જો(*cp == '\\') { /* Escaped characters */
        cp++;
        જો(*cp != '\'' && *cp != '\"' && *cp != '\\')
          જાઓ fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      જો(result)
        પાછા result;
      cp++;
    }
    cp++; /* pass the end quote */

    જો(!curlx_dyn_len(&out))
      જાઓ fail;
  }
  નહીંતર {
    સંરચના Curl_str word;
    bool content = FALSE;
    પૂર્ણાંક rc;
    /* Handling for relative path - prepend home directory */
    જો(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      જો(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      જો(result)
        પાછા result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    જો(rc) {
      જો(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        પાછા CURLE_TOO_LARGE;
      }
      નહીંતર જો(!content)
        /* no path, no word, this is incorrect */
        જાઓ fail;
    }
    નહીંતર {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      જો(result)
        પાછા result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  પાછા CURLE_OK;

fail:
  curlx_dyn_free(&out);
  પાછા CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(સંરચના Curl_easy *data,
                        અચલ અક્ષર *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  પૂર્ણાંક to_t;
  પૂર્ણાંક from_t = curlx_str_સંખ્યા(&range, &from, CURL_OFF_T_MAX);
  જો(from_t == STRE_OVERFLOW)
    પાછા CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (શૂન્ય)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  જો((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    પાછા CURLE_RANGE_ERROR;

  જો(from_t) {
    /* no start point given, set from relative to end of file */
    જો(!to)
      /* "-0" is not a fine range */
      પાછા CURLE_RANGE_ERROR;
    નહીંતર જો(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  નહીંતર જો(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    પાછા CURLE_RANGE_ERROR;
  }
  નહીંતર જો((to_t == STRE_NO_અંક) || (to >= filesize))
    to = filesize - 1;

  જો(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    પાછા CURLE_RANGE_ERROR;
  }
  જો((to - from) == CURL_OFF_T_MAX)
    પાછા CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  પાછા CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(સંરચના Curl_easy *data, સંરચના easy_pollset *ps)
{
  સંરચના connectdata *conn = data->conn;
  સંરચના ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  પૂર્ણાંક waitfor;

  જો(!sshc || (sock == CURL_SOCKET_BAD))
    પાછા CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  જો(waitfor) {
    પૂર્ણાંક flags = 0;
    જો(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    જો(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned પૂર્ણાંક)flags);
    પાછા Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  જો(sshc->ssh_session)
    પાછા Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  પાછા CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(સંરચના Curl_easy *data, સંરચના ssh_conn *sshc)
{
  અક્ષર *home = ખાલી;
  જો(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    અચલ અક્ષર *str;

    sshc->pub_ચાવી = sshc->priv_ચાવી = ખાલી;

    જો(CURL_EASY_STR(data, STRING_SSH_PRIVATE_ચાવી)) {
      sshc->priv_ચાવી = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_ચાવી));
      જો(!sshc->priv_ચાવી)
        જાઓ fail;
    }
    નહીંતર {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      જો(home) {
        /* Try ~/.ssh first. */
        sshc->priv_ચાવી = curl_maprintf("%s/.ssh/id_rsa", home);
        જો(!sshc->priv_ચાવી)
          જાઓ fail;
        નહીંતર જો(curlx_stat(sshc->priv_ચાવી, &sbuf)) {
          curlx_free(sshc->priv_ચાવી);
          sshc->priv_ચાવી = curl_maprintf("%s/.ssh/id_dsa", home);
          જો(!sshc->priv_ચાવી)
            જાઓ fail;
          નહીંતર જો(curlx_stat(sshc->priv_ચાવી, &sbuf)) {
            curlx_safefree(sshc->priv_ચાવી);
          }
        }
        curlx_safefree(home);
      }
      જો(!sshc->priv_ચાવી) {
        /* Nothing found; try the current dir. */
        sshc->priv_ચાવી = curlx_strdup("id_rsa");
        જો(sshc->priv_ચાવી && curlx_stat(sshc->priv_ચાવી, &sbuf)) {
          curlx_free(sshc->priv_ચાવી);
          sshc->priv_ચાવી = curlx_strdup("id_dsa");
          જો(sshc->priv_ચાવી && curlx_stat(sshc->priv_ચાવી, &sbuf)) {
            curlx_free(sshc->priv_ચાવી);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_ચાવી = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_ચાવી);
    જો(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_ચાવી = curlx_strdup(str);
      જો(!sshc->pub_ચાવી)
        જાઓ fail;
    }

    {
      અચલ અક્ષર *keypasswd = CURL_EASY_STR(data, STRING_ચાવી_PASSWD);
      જો(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        જો(!sshc->passphrase)
          જાઓ fail;
      }
    }

    જો(sshc->pub_ચાવી)
      infof(data, "SSH: public key file '%s'", sshc->pub_ચાવી);
    જો(sshc->priv_ચાવી)
      infof(data, "SSH: private key file '%s'", sshc->priv_ચાવી);
    નહીંતર
      infof(data, "SSH: public key auth without private key set!");
  }
  પાછા CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_ચાવી);
  curlx_safefree(sshc->pub_ચાવી);
  પાછા CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
