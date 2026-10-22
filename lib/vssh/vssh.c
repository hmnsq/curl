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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "vssh/ssh.h"

#ifdef USE_SSH

#അന്തര്ഭാവ "vssh/vssh.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "select.h"  /* for Curl_pollset_change() */
#അന്തര്ഭാവ "url.h"  /* for Curl_conn_meta_get() */
#അന്തര്ഭാവ "curlx/win32-fopen.h"

#ifdef CURLVERBOSE
സ്ഥിരം അക്ഷരം *Curl_ssh_statename(sshstate state)
{
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം names[] = {
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
  തിരികെ ((size_t)state < CURL_ARRAYSIZE(names)) ? names[state] : "";
}
#endif /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
ശൂന്യം Curl_ssh_set_state(ഘടന Curl_easy *data,
                        ഘടന ssh_conn *sshc,
                        sshstate nowstate)
{
#ifdef CURLVERBOSE
  എങ്കിൽ(sshc->state != nowstate) {
    CURL_TRC_SSH(data, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
#endif
  sshc->state = nowstate;
}

#define MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(ഘടന Curl_easy *data,
                             സ്ഥിരം അക്ഷരം *homedir, /* when SFTP is used */
                             അക്ഷരം **path) /* returns the  allocated
                                             real path to work with */
{
  അക്ഷരം *working_path;
  size_t working_path_len;
  ഘടന dynbuf npath;
  CURLcode result =
    Curl_urldecode(data->state.up.path, 0, &working_path,
                   &working_path_len, REJECT_ZERO);
  എങ്കിൽ(result)
    തിരികെ result;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  എങ്കിൽ((data->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_path_len > 3) && (!memcmp(working_path, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    എങ്കിൽ(curlx_dyn_addn(&npath, &working_path[3], working_path_len - 3)) {
      curlx_free(working_path);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ((data->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_path) ||
           ((working_path_len > 2) && !memcmp(working_path, "/~/", 3)))) {
    എങ്കിൽ(curlx_dyn_add(&npath, homedir)) {
      curlx_free(working_path);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
    എങ്കിൽ(working_path_len > 2) {
      size_t len;
      സ്ഥിരം അക്ഷരം *p;
      പൂർണ്ണസംഖ്യ copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      എങ്കിൽ(len && (p[len - 1] != '/'))
        copyfrom = 2;

      എങ്കിൽ(curlx_dyn_addn(&npath, &working_path[copyfrom],
                        working_path_len - copyfrom)) {
        curlx_free(working_path);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(curlx_dyn_add(&npath, "/")) {
        curlx_free(working_path);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }
    }
  }

  എങ്കിൽ(curlx_dyn_len(&npath)) {
    curlx_free(working_path);

    /* store the pointer for the caller to receive */
    *path = curlx_dyn_ptr(&npath);
  }
  അല്ലെങ്കിൽ
    *path = working_path;
  DEBUGASSERT(*path && (*path)[0]);

  തിരികെ CURLE_OK;
}

#define MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(സ്ഥിരം അക്ഷരം **cpp, അക്ഷരം **path, സ്ഥിരം അക്ഷരം *homedir)
{
  സ്ഥിരം അക്ഷരം *cp = *cpp;
  ഘടന dynbuf out;
  CURLcode result;

  DEBUGASSERT(homedir);
  *path = ശൂന്യ;
  *cpp = ശൂന്യ;
  എങ്കിൽ(!*cp || !homedir)
    തിരികെ CURLE_QUOTE_ERROR;

  curlx_dyn_init(&out, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  എങ്കിൽ(*cp == '\"' || *cp == '\'') {
    അക്ഷരം quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    ആകുമ്പോൾ(*cp != quot) {
      എങ്കിൽ(!*cp) /* End of string */
        പോകുക fail;

      എങ്കിൽ(*cp == '\\') { /* Escaped characters */
        cp++;
        എങ്കിൽ(*cp != '\'' && *cp != '\"' && *cp != '\\')
          പോകുക fail;
      }
      result = curlx_dyn_addn(&out, cp, 1);
      എങ്കിൽ(result)
        തിരികെ result;
      cp++;
    }
    cp++; /* pass the end quote */

    എങ്കിൽ(!curlx_dyn_len(&out))
      പോകുക fail;
  }
  അല്ലെങ്കിൽ {
    ഘടന Curl_str word;
    bool content = FALSE;
    പൂർണ്ണസംഖ്യ rc;
    /* Handling for relative path - prepend home directory */
    എങ്കിൽ(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      result = curlx_dyn_add(&out, homedir);
      എങ്കിൽ(!result)
        result = curlx_dyn_addn(&out, "/", 1);
      എങ്കിൽ(result)
        തിരികെ result;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    എങ്കിൽ(rc) {
      എങ്കിൽ(rc == STRE_BIG) {
        curlx_dyn_free(&out);
        തിരികെ CURLE_TOO_LARGE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(!content)
        /* no path, no word, this is incorrect */
        പോകുക fail;
    }
    അല്ലെങ്കിൽ {
      /* append the word */
      result = curlx_dyn_addn(&out, curlx_str(&word), curlx_strlen(&word));
      എങ്കിൽ(result)
        തിരികെ result;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *path = curlx_dyn_ptr(&out);
  തിരികെ CURLE_OK;

fail:
  curlx_dyn_free(&out);
  തിരികെ CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(ഘടന Curl_easy *data,
                        സ്ഥിരം അക്ഷരം *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, to;
  പൂർണ്ണസംഖ്യ to_t;
  പൂർണ്ണസംഖ്യ from_t = curlx_str_സംഖ്യ(&range, &from, CURL_OFF_T_MAX);
  എങ്കിൽ(from_t == STRE_OVERFLOW)
    തിരികെ CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (ശൂന്യം)curlx_str_single(&range, '-');

  to_t = curlx_str_numblanks(&range, &to);
  എങ്കിൽ((to_t == STRE_OVERFLOW) || (to_t && from_t) || *range)
    തിരികെ CURLE_RANGE_ERROR;

  എങ്കിൽ(from_t) {
    /* no start point given, set from relative to end of file */
    എങ്കിൽ(!to)
      /* "-0" is not a fine range */
      തിരികെ CURLE_RANGE_ERROR;
    അല്ലെങ്കിൽ എങ്കിൽ(to > filesize)
      to = filesize;
    from = filesize - to;
    to = filesize - 1;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(from > filesize) {
    failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    തിരികെ CURLE_RANGE_ERROR;
  }
  അല്ലെങ്കിൽ എങ്കിൽ((to_t == STRE_NO_അക്കം) || (to >= filesize))
    to = filesize - 1;

  എങ്കിൽ(from > to) {
    failf(data, "Bad range: start offset larger than end offset");
    തിരികെ CURLE_RANGE_ERROR;
  }
  എങ്കിൽ((to - from) == CURL_OFF_T_MAX)
    തിരികെ CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = to - from + 1;
  തിരികെ CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(ഘടന Curl_easy *data, ഘടന easy_pollset *ps)
{
  ഘടന connectdata *conn = data->conn;
  ഘടന ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  പൂർണ്ണസംഖ്യ waitfor;

  എങ്കിൽ(!sshc || (sock == CURL_SOCKET_BAD))
    തിരികെ CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : data->req.io_flags;
  എങ്കിൽ(waitfor) {
    പൂർണ്ണസംഖ്യ flags = 0;
    എങ്കിൽ(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_IN;
    എങ്കിൽ(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_OUT;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(data, "pollset, flags=%x", (unsigned പൂർണ്ണസംഖ്യ)flags);
    തിരികെ Curl_pollset_change(data, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  എങ്കിൽ(sshc->ssh_session)
    തിരികെ Curl_pollset_change(data, ps, sock, CURL_POLL_IN, 0);
  തിരികെ CURLE_OK;
}

CURLcode Curl_ssh_setup_pkey(ഘടന Curl_easy *data, ഘടന ssh_conn *sshc)
{
  അക്ഷരം *home = ശൂന്യ;
  എങ്കിൽ(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    സ്ഥിരം അക്ഷരം *str;

    sshc->pub_താക്കോൽ = sshc->priv_താക്കോൽ = ശൂന്യ;

    എങ്കിൽ(CURL_EASY_STR(data, STRING_SSH_PRIVATE_താക്കോൽ)) {
      sshc->priv_താക്കോൽ = curlx_strdup(
        CURL_EASY_STR(data, STRING_SSH_PRIVATE_താക്കോൽ));
      എങ്കിൽ(!sshc->priv_താക്കോൽ)
        പോകുക fail;
    }
    അല്ലെങ്കിൽ {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      എങ്കിൽ(home) {
        /* Try ~/.ssh first. */
        sshc->priv_താക്കോൽ = curl_maprintf("%s/.ssh/id_rsa", home);
        എങ്കിൽ(!sshc->priv_താക്കോൽ)
          പോകുക fail;
        അല്ലെങ്കിൽ എങ്കിൽ(curlx_stat(sshc->priv_താക്കോൽ, &sbuf)) {
          curlx_free(sshc->priv_താക്കോൽ);
          sshc->priv_താക്കോൽ = curl_maprintf("%s/.ssh/id_dsa", home);
          എങ്കിൽ(!sshc->priv_താക്കോൽ)
            പോകുക fail;
          അല്ലെങ്കിൽ എങ്കിൽ(curlx_stat(sshc->priv_താക്കോൽ, &sbuf)) {
            curlx_safefree(sshc->priv_താക്കോൽ);
          }
        }
        curlx_safefree(home);
      }
      എങ്കിൽ(!sshc->priv_താക്കോൽ) {
        /* Nothing found; try the current dir. */
        sshc->priv_താക്കോൽ = curlx_strdup("id_rsa");
        എങ്കിൽ(sshc->priv_താക്കോൽ && curlx_stat(sshc->priv_താക്കോൽ, &sbuf)) {
          curlx_free(sshc->priv_താക്കോൽ);
          sshc->priv_താക്കോൽ = curlx_strdup("id_dsa");
          എങ്കിൽ(sshc->priv_താക്കോൽ && curlx_stat(sshc->priv_താക്കോൽ, &sbuf)) {
            curlx_free(sshc->priv_താക്കോൽ);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_താക്കോൽ = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(data, STRING_SSH_PUBLIC_താക്കോൽ);
    എങ്കിൽ(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_താക്കോൽ = curlx_strdup(str);
      എങ്കിൽ(!sshc->pub_താക്കോൽ)
        പോകുക fail;
    }

    {
      സ്ഥിരം അക്ഷരം *keypasswd = CURL_EASY_STR(data, STRING_താക്കോൽ_PASSWD);
      എങ്കിൽ(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        എങ്കിൽ(!sshc->passphrase)
          പോകുക fail;
      }
    }

    എങ്കിൽ(sshc->pub_താക്കോൽ)
      infof(data, "SSH: public key file '%s'", sshc->pub_താക്കോൽ);
    എങ്കിൽ(sshc->priv_താക്കോൽ)
      infof(data, "SSH: private key file '%s'", sshc->priv_താക്കോൽ);
    അല്ലെങ്കിൽ
      infof(data, "SSH: public key auth without private key set!");
  }
  തിരികെ CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_താക്കോൽ);
  curlx_safefree(sshc->pub_താക്കോൽ);
  തിരികെ CURLE_OUT_OF_MEMORY;
}

#endif /* USE_SSH */
