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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "vssh/ssh.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH

#ಅನ್ತರ್ಭಾವ "vssh/vssh.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "select.h"  /* for Curl_pollset_change() */
#ಅನ್ತರ್ಭಾವ "url.h"  /* for Curl_conn_meta_get() */
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_ssh_statename(sshstate state)
{
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಹೆಸರುಗಳು[] = {
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
  DEBUGASSERT(CURL_ARRAYSIZE(ಹೆಸರುಗಳು) == SSH_ಕೊನೆಯ);
  ಹಿಂತಿರುಗಿ ((size_t)state < CURL_ARRAYSIZE(ಹೆಸರುಗಳು)) ? ಹೆಸರುಗಳು[state] : "";
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
ಶೂನ್ಯ Curl_ssh_ಗಣ_state(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ ssh_conn *sshc,
                        sshstate nowstate)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(sshc->state != nowstate) {
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
  sshc->state = nowstate;
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *homedir, /* when SFTP is used */
                             ಅಕ್ಷರ **ಮಾರ್ಗ) /* returns the  allocated
                                             real path to work with */
{
  ಅಕ್ಷರ *working_ಮಾರ್ಗ;
  size_t working_ಮಾರ್ಗ_len;
  ರಚನೆ dynbuf npath;
  CURLcode ಫಲಿತಾಂಶ =
    Curl_urldecode(ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ, 0, &working_ಮಾರ್ಗ,
                   &working_ಮಾರ್ಗ_len, REJECT_ZERO);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  ಆದರೆ((ದತ್ತಾಂಶ->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_ಮಾರ್ಗ_len > 3) && (!memcmp(working_ಮಾರ್ಗ, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    ಆದರೆ(curlx_dyn_addn(&npath, &working_ಮಾರ್ಗ[3], working_ಮಾರ್ಗ_len - 3)) {
      curlx_free(working_ಮಾರ್ಗ);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ದತ್ತಾಂಶ->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_ಮಾರ್ಗ) ||
           ((working_ಮಾರ್ಗ_len > 2) && !memcmp(working_ಮಾರ್ಗ, "/~/", 3)))) {
    ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(&npath, homedir)) {
      curlx_free(working_ಮಾರ್ಗ);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಆದರೆ(working_ಮಾರ್ಗ_len > 2) {
      size_t len;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
      ಪೂರ್ಣಾಂಕ copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      ಆದರೆ(len && (p[len - 1] != '/'))
        copyfrom = 2;

      ಆದರೆ(curlx_dyn_addn(&npath, &working_ಮಾರ್ಗ[copyfrom],
                        working_ಮಾರ್ಗ_len - copyfrom)) {
        curlx_free(working_ಮಾರ್ಗ);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(&npath, "/")) {
        curlx_free(working_ಮಾರ್ಗ);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
  }

  ಆದರೆ(curlx_dyn_len(&npath)) {
    curlx_free(working_ಮಾರ್ಗ);

    /* store the pointer for the caller to receive */
    *ಮಾರ್ಗ = curlx_dyn_ptr(&npath);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    *ಮಾರ್ಗ = working_ಮಾರ್ಗ;
  DEBUGASSERT(*ಮಾರ್ಗ && (*ಮಾರ್ಗ)[0]);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **cpp, ಅಕ್ಷರ **ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *homedir)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cp = *cpp;
  ರಚನೆ dynbuf ಹೊರಹೋಗು;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(homedir);
  *ಮಾರ್ಗ = ಖಾಲಿ;
  *cpp = ಖಾಲಿ;
  ಆದರೆ(!*cp || !homedir)
    ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;

  curlx_dyn_init(&ಹೊರಹೋಗು, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  ಆದರೆ(*cp == '\"' || *cp == '\'') {
    ಅಕ್ಷರ quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    ಆಗ(*cp != quot) {
      ಆದರೆ(!*cp) /* End of string */
        ಹೋಗು fail;

      ಆದರೆ(*cp == '\\') { /* Escaped characters */
        cp++;
        ಆದರೆ(*cp != '\'' && *cp != '\"' && *cp != '\\')
          ಹೋಗು fail;
      }
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, cp, 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      cp++;
    }
    cp++; /* pass the end quote */

    ಆದರೆ(!curlx_dyn_len(&ಹೊರಹೋಗು))
      ಹೋಗು fail;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ರಚನೆ Curl_str ಶಬ್ದ;
    bool content = FALSE;
    ಪೂರ್ಣಾಂಕ rc;
    /* Handling for relative path - prepend home directory */
    ಆದರೆ(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&ಹೊರಹೋಗು, homedir);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, "/", 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_ಶಬ್ದ(&cp, &ಶಬ್ದ, MAX_PATHLENGTH);
    ಆದರೆ(rc) {
      ಆದರೆ(rc == STRE_BIG) {
        curlx_dyn_free(&ಹೊರಹೋಗು);
        ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!content)
        /* no path, no word, this is incorrect */
        ಹೋಗು fail;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* append the word */
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, curlx_str(&ಶಬ್ದ), curlx_strlen(&ಶಬ್ದ));
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *ಮಾರ್ಗ = curlx_dyn_ptr(&ಹೊರಹೋಗು);
  ಹಿಂತಿರುಗಿ CURLE_OK;

fail:
  curlx_dyn_free(&ಹೊರಹೋಗು);
  ಹಿಂತಿರುಗಿ CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, ಗಮ್ಯ;
  ಪೂರ್ಣಾಂಕ ಗಮ್ಯ_t;
  ಪೂರ್ಣಾಂಕ from_t = curlx_str_ಸಂಖ್ಯೆ(&range, &from, CURL_OFF_T_MAX);
  ಆದರೆ(from_t == STRE_ಅತಿಪ್ರವಾಹ)
    ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (ಶೂನ್ಯ)curlx_str_single(&range, '-');

  ಗಮ್ಯ_t = curlx_str_numblanks(&range, &ಗಮ್ಯ);
  ಆದರೆ((ಗಮ್ಯ_t == STRE_ಅತಿಪ್ರವಾಹ) || (ಗಮ್ಯ_t && from_t) || *range)
    ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;

  ಆದರೆ(from_t) {
    /* no start point given, set from relative to end of file */
    ಆದರೆ(!ಗಮ್ಯ)
      /* "-0" is not a fine range */
      ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಮ್ಯ > filesize)
      ಗಮ್ಯ = filesize;
    from = filesize - ಗಮ್ಯ;
    ಗಮ್ಯ = filesize - 1;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(from > filesize) {
    failf(ದತ್ತಾಂಶ, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಗಮ್ಯ_t == STRE_NO_ಅಂಕೆ) || (ಗಮ್ಯ >= filesize))
    ಗಮ್ಯ = filesize - 1;

  ಆದರೆ(from > ಗಮ್ಯ) {
    failf(ದತ್ತಾಂಶ, "Bad range: start offset larger than end offset");
    ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;
  }
  ಆದರೆ((ಗಮ್ಯ - from) == CURL_OFF_T_MAX)
    ಹಿಂತಿರುಗಿ CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = ಗಮ್ಯ - from + 1;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  ಪೂರ್ಣಾಂಕ waitfor;

  ಆದರೆ(!sshc || (sock == CURL_SOCKET_BAD))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : ದತ್ತಾಂಶ->req.io_flags;
  ಆದರೆ(waitfor) {
    ಪೂರ್ಣಾಂಕ flags = 0;
    ಆದರೆ(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_ಒಳ;
    ಆದರೆ(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_ಹೊರಹೋಗು;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(ದತ್ತಾಂಶ, "pollset, flags=%x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)flags);
    ಹಿಂತಿರುಗಿ Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  ಆದರೆ(sshc->ssh_session)
    ಹಿಂತಿರುಗಿ Curl_pollset_change(ದತ್ತಾಂಶ, ps, sock, CURL_POLL_ಒಳ, 0);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_ssh_ಸಿದ್ಧತೆ_pkey(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ssh_conn *sshc)
{
  ಅಕ್ಷರ *home = ಖಾಲಿ;
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;

    sshc->pub_ಕೀಲಿ = sshc->priv_ಕೀಲಿ = ಖಾಲಿ;

    ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_ಖಾಸಗಿ_ಕೀಲಿ)) {
      sshc->priv_ಕೀಲಿ = curlx_strdup(
        CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_ಖಾಸಗಿ_ಕೀಲಿ));
      ಆದರೆ(!sshc->priv_ಕೀಲಿ)
        ಹೋಗು fail;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      ಆದರೆ(home) {
        /* Try ~/.ssh first. */
        sshc->priv_ಕೀಲಿ = curl_maprintf("%s/.ssh/id_rsa", home);
        ಆದರೆ(!sshc->priv_ಕೀಲಿ)
          ಹೋಗು fail;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_stat(sshc->priv_ಕೀಲಿ, &sbuf)) {
          curlx_free(sshc->priv_ಕೀಲಿ);
          sshc->priv_ಕೀಲಿ = curl_maprintf("%s/.ssh/id_dsa", home);
          ಆದರೆ(!sshc->priv_ಕೀಲಿ)
            ಹೋಗು fail;
          ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_stat(sshc->priv_ಕೀಲಿ, &sbuf)) {
            curlx_safefree(sshc->priv_ಕೀಲಿ);
          }
        }
        curlx_safefree(home);
      }
      ಆದರೆ(!sshc->priv_ಕೀಲಿ) {
        /* Nothing found; try the current dir. */
        sshc->priv_ಕೀಲಿ = curlx_strdup("id_rsa");
        ಆದರೆ(sshc->priv_ಕೀಲಿ && curlx_stat(sshc->priv_ಕೀಲಿ, &sbuf)) {
          curlx_free(sshc->priv_ಕೀಲಿ);
          sshc->priv_ಕೀಲಿ = curlx_strdup("id_dsa");
          ಆದರೆ(sshc->priv_ಕೀಲಿ && curlx_stat(sshc->priv_ಕೀಲಿ, &sbuf)) {
            curlx_free(sshc->priv_ಕೀಲಿ);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_ಕೀಲಿ = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSH_ಬಹಿರಂಗ_ಕೀಲಿ);
    ಆದರೆ(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_ಕೀಲಿ = curlx_strdup(str);
      ಆದರೆ(!sshc->pub_ಕೀಲಿ)
        ಹೋಗು fail;
    }

    {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *keypasswd = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_ಕೀಲಿ_PASSWD);
      ಆದರೆ(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        ಆದರೆ(!sshc->passphrase)
          ಹೋಗು fail;
      }
    }

    ಆದರೆ(sshc->pub_ಕೀಲಿ)
      infof(ದತ್ತಾಂಶ, "SSH: public key file '%s'", sshc->pub_ಕೀಲಿ);
    ಆದರೆ(sshc->priv_ಕೀಲಿ)
      infof(ದತ್ತಾಂಶ, "SSH: private key file '%s'", sshc->priv_ಕೀಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "SSH: public key auth without private key set!");
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_ಕೀಲಿ);
  curlx_safefree(sshc->pub_ಕೀಲಿ);
  ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_SSH */
