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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/ssh.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH

#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/vssh.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"  /* for Curl_pollset_change() */
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"  /* for Curl_conn_meta_get() */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਅਚਲ ਅੱਖਰ *Curl_ssh_statename(sshstate state)
{
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ਨਾਂ[] = {
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
  DEBUGASSERT(CURL_ARRAYSIZE(ਨਾਂ) == SSH_ਆਖਰੀ);
  ਵਾਪਸ ((size_t)state < CURL_ARRAYSIZE(ਨਾਂ)) ? ਨਾਂ[state] : "";
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
ਖਾਲੀ Curl_ssh_ਸੈੱਟ_state(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ ssh_conn *sshc,
                        sshstate nowstate)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(sshc->state != nowstate) {
    CURL_TRC_SSH(ਅੰਕੜੇ, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
  sshc->state = nowstate;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਅੱਖਰ *homedir, /* when SFTP is used */
                             ਅੱਖਰ **ਰਸਤਾ) /* returns the  allocated
                                             real path to work with */
{
  ਅੱਖਰ *working_ਰਸਤਾ;
  size_t working_ਰਸਤਾ_len;
  ਬਣਤਰ dynbuf npath;
  CURLcode ਨਤੀਜਾ =
    Curl_urldecode(ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ, 0, &working_ਰਸਤਾ,
                   &working_ਰਸਤਾ_len, REJECT_ZERO);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  ਜੇ((ਅੰਕੜੇ->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_ਰਸਤਾ_len > 3) && (!memcmp(working_ਰਸਤਾ, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    ਜੇ(curlx_dyn_addn(&npath, &working_ਰਸਤਾ[3], working_ਰਸਤਾ_len - 3)) {
      curlx_free(working_ਰਸਤਾ);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ((ਅੰਕੜੇ->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_ਰਸਤਾ) ||
           ((working_ਰਸਤਾ_len > 2) && !memcmp(working_ਰਸਤਾ, "/~/", 3)))) {
    ਜੇ(curlx_dyn_ਜੋੜੋ(&npath, homedir)) {
      curlx_free(working_ਰਸਤਾ);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਜੇ(working_ਰਸਤਾ_len > 2) {
      size_t len;
      ਅਚਲ ਅੱਖਰ *p;
      ਪੂਰਨ copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      ਜੇ(len && (p[len - 1] != '/'))
        copyfrom = 2;

      ਜੇ(curlx_dyn_addn(&npath, &working_ਰਸਤਾ[copyfrom],
                        working_ਰਸਤਾ_len - copyfrom)) {
        curlx_free(working_ਰਸਤਾ);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(curlx_dyn_ਜੋੜੋ(&npath, "/")) {
        curlx_free(working_ਰਸਤਾ);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
  }

  ਜੇ(curlx_dyn_len(&npath)) {
    curlx_free(working_ਰਸਤਾ);

    /* store the pointer for the caller to receive */
    *ਰਸਤਾ = curlx_dyn_ptr(&npath);
  }
  ਨਹੀਂਤਾਂ
    *ਰਸਤਾ = working_ਰਸਤਾ;
  DEBUGASSERT(*ਰਸਤਾ && (*ਰਸਤਾ)[0]);

  ਵਾਪਸ CURLE_OK;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ਅਚਲ ਅੱਖਰ **cpp, ਅੱਖਰ **ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *homedir)
{
  ਅਚਲ ਅੱਖਰ *cp = *cpp;
  ਬਣਤਰ dynbuf ਬਾਹਰਲਾ;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(homedir);
  *ਰਸਤਾ = ਨਿਰਾ;
  *cpp = ਨਿਰਾ;
  ਜੇ(!*cp || !homedir)
    ਵਾਪਸ CURLE_QUOTE_ERROR;

  curlx_dyn_init(&ਬਾਹਰਲਾ, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  ਜੇ(*cp == '\"' || *cp == '\'') {
    ਅੱਖਰ quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    ਜਦੋਂ(*cp != quot) {
      ਜੇ(!*cp) /* End of string */
        ਜਾਓ fail;

      ਜੇ(*cp == '\\') { /* Escaped characters */
        cp++;
        ਜੇ(*cp != '\'' && *cp != '\"' && *cp != '\\')
          ਜਾਓ fail;
      }
      ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, cp, 1);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      cp++;
    }
    cp++; /* pass the end quote */

    ਜੇ(!curlx_dyn_len(&ਬਾਹਰਲਾ))
      ਜਾਓ fail;
  }
  ਨਹੀਂਤਾਂ {
    ਬਣਤਰ Curl_str ਸ਼ਬਦ;
    bool content = FALSE;
    ਪੂਰਨ rc;
    /* Handling for relative path - prepend home directory */
    ਜੇ(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&ਬਾਹਰਲਾ, homedir);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, "/", 1);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_ਸ਼ਬਦ(&cp, &ਸ਼ਬਦ, MAX_PATHLENGTH);
    ਜੇ(rc) {
      ਜੇ(rc == STRE_BIG) {
        curlx_dyn_free(&ਬਾਹਰਲਾ);
        ਵਾਪਸ CURLE_TOO_LARGE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!content)
        /* no path, no word, this is incorrect */
        ਜਾਓ fail;
    }
    ਨਹੀਂਤਾਂ {
      /* append the word */
      ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, curlx_str(&ਸ਼ਬਦ), curlx_strlen(&ਸ਼ਬਦ));
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *ਰਸਤਾ = curlx_dyn_ptr(&ਬਾਹਰਲਾ);
  ਵਾਪਸ CURLE_OK;

fail:
  curlx_dyn_free(&ਬਾਹਰਲਾ);
  ਵਾਪਸ CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, ਮੰਜ਼ਿਲ;
  ਪੂਰਨ ਮੰਜ਼ਿਲ_t;
  ਪੂਰਨ from_t = curlx_str_ਸੰਖਿਆ(&range, &from, CURL_OFF_T_MAX);
  ਜੇ(from_t == STRE_ਉਛਾਲ)
    ਵਾਪਸ CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (ਖਾਲੀ)curlx_str_single(&range, '-');

  ਮੰਜ਼ਿਲ_t = curlx_str_numblanks(&range, &ਮੰਜ਼ਿਲ);
  ਜੇ((ਮੰਜ਼ਿਲ_t == STRE_ਉਛਾਲ) || (ਮੰਜ਼ਿਲ_t && from_t) || *range)
    ਵਾਪਸ CURLE_RANGE_ERROR;

  ਜੇ(from_t) {
    /* no start point given, set from relative to end of file */
    ਜੇ(!ਮੰਜ਼ਿਲ)
      /* "-0" is not a fine range */
      ਵਾਪਸ CURLE_RANGE_ERROR;
    ਨਹੀਂਤਾਂ ਜੇ(ਮੰਜ਼ਿਲ > filesize)
      ਮੰਜ਼ਿਲ = filesize;
    from = filesize - ਮੰਜ਼ਿਲ;
    ਮੰਜ਼ਿਲ = filesize - 1;
  }
  ਨਹੀਂਤਾਂ ਜੇ(from > filesize) {
    failf(ਅੰਕੜੇ, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    ਵਾਪਸ CURLE_RANGE_ERROR;
  }
  ਨਹੀਂਤਾਂ ਜੇ((ਮੰਜ਼ਿਲ_t == STRE_NO_ਅੰਕ) || (ਮੰਜ਼ਿਲ >= filesize))
    ਮੰਜ਼ਿਲ = filesize - 1;

  ਜੇ(from > ਮੰਜ਼ਿਲ) {
    failf(ਅੰਕੜੇ, "Bad range: start offset larger than end offset");
    ਵਾਪਸ CURLE_RANGE_ERROR;
  }
  ਜੇ((ਮੰਜ਼ਿਲ - from) == CURL_OFF_T_MAX)
    ਵਾਪਸ CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = ਮੰਜ਼ਿਲ - from + 1;
  ਵਾਪਸ CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ਪੂਰਨ waitfor;

  ਜੇ(!sshc || (sock == CURL_SOCKET_BAD))
    ਵਾਪਸ CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : ਅੰਕੜੇ->req.io_flags;
  ਜੇ(waitfor) {
    ਪੂਰਨ flags = 0;
    ਜੇ(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_ਵਿੱਚ;
    ਜੇ(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_ਬਾਹਰਲਾ;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(ਅੰਕੜੇ, "pollset, flags=%x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)flags);
    ਵਾਪਸ Curl_pollset_change(ਅੰਕੜੇ, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  ਜੇ(sshc->ssh_session)
    ਵਾਪਸ Curl_pollset_change(ਅੰਕੜੇ, ps, sock, CURL_POLL_ਵਿੱਚ, 0);
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_ssh_ਤਿਆਰੀ_pkey(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ssh_conn *sshc)
{
  ਅੱਖਰ *home = ਨਿਰਾ;
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ਅਚਲ ਅੱਖਰ *str;

    sshc->pub_ਕੁੰਜੀ = sshc->priv_ਕੁੰਜੀ = ਨਿਰਾ;

    ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_ਨਿੱਜੀ_ਕੁੰਜੀ)) {
      sshc->priv_ਕੁੰਜੀ = curlx_strdup(
        CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_ਨਿੱਜੀ_ਕੁੰਜੀ));
      ਜੇ(!sshc->priv_ਕੁੰਜੀ)
        ਜਾਓ fail;
    }
    ਨਹੀਂਤਾਂ {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      ਜੇ(home) {
        /* Try ~/.ssh first. */
        sshc->priv_ਕੁੰਜੀ = curl_maprintf("%s/.ssh/id_rsa", home);
        ਜੇ(!sshc->priv_ਕੁੰਜੀ)
          ਜਾਓ fail;
        ਨਹੀਂਤਾਂ ਜੇ(curlx_stat(sshc->priv_ਕੁੰਜੀ, &sbuf)) {
          curlx_free(sshc->priv_ਕੁੰਜੀ);
          sshc->priv_ਕੁੰਜੀ = curl_maprintf("%s/.ssh/id_dsa", home);
          ਜੇ(!sshc->priv_ਕੁੰਜੀ)
            ਜਾਓ fail;
          ਨਹੀਂਤਾਂ ਜੇ(curlx_stat(sshc->priv_ਕੁੰਜੀ, &sbuf)) {
            curlx_safefree(sshc->priv_ਕੁੰਜੀ);
          }
        }
        curlx_safefree(home);
      }
      ਜੇ(!sshc->priv_ਕੁੰਜੀ) {
        /* Nothing found; try the current dir. */
        sshc->priv_ਕੁੰਜੀ = curlx_strdup("id_rsa");
        ਜੇ(sshc->priv_ਕੁੰਜੀ && curlx_stat(sshc->priv_ਕੁੰਜੀ, &sbuf)) {
          curlx_free(sshc->priv_ਕੁੰਜੀ);
          sshc->priv_ਕੁੰਜੀ = curlx_strdup("id_dsa");
          ਜੇ(sshc->priv_ਕੁੰਜੀ && curlx_stat(sshc->priv_ਕੁੰਜੀ, &sbuf)) {
            curlx_free(sshc->priv_ਕੁੰਜੀ);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_ਕੁੰਜੀ = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSH_ਜਨਤਕ_ਕੁੰਜੀ);
    ਜੇ(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_ਕੁੰਜੀ = curlx_strdup(str);
      ਜੇ(!sshc->pub_ਕੁੰਜੀ)
        ਜਾਓ fail;
    }

    {
      ਅਚਲ ਅੱਖਰ *keypasswd = CURL_EASY_STR(ਅੰਕੜੇ, STRING_ਕੁੰਜੀ_PASSWD);
      ਜੇ(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        ਜੇ(!sshc->passphrase)
          ਜਾਓ fail;
      }
    }

    ਜੇ(sshc->pub_ਕੁੰਜੀ)
      infof(ਅੰਕੜੇ, "SSH: public key file '%s'", sshc->pub_ਕੁੰਜੀ);
    ਜੇ(sshc->priv_ਕੁੰਜੀ)
      infof(ਅੰਕੜੇ, "SSH: private key file '%s'", sshc->priv_ਕੁੰਜੀ);
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "SSH: public key auth without private key set!");
  }
  ਵਾਪਸ CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_ਕੁੰਜੀ);
  curlx_safefree(sshc->pub_ਕੁੰਜੀ);
  ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
}

#ਜੇ_ਅੰਤ /* USE_SSH */
