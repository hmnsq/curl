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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "vssh/ssh.h"

#నిర్వచితమైతే USE_SSH

#అన్తర్భావ "vssh/vssh.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "select.h"  /* for Curl_pollset_change() */
#అన్తర్భావ "url.h"  /* for Curl_conn_meta_get() */
#అన్తర్భావ "curlx/win32-fopen.h"

#నిర్వచితమైతే CURLVERBOSE
స్థిరరాశి అక్షరం *Curl_ssh_statename(sshstate state)
{
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి పేర్లు[] = {
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
  DEBUGASSERT(CURL_ARRAYSIZE(పేర్లు) == SSH_చివరి);
  తిరిగి ((size_t)state < CURL_ARRAYSIZE(పేర్లు)) ? పేర్లు[state] : "";
}
#అయితే_ముగింపు /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
శూన్య Curl_ssh_సమితి_state(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం ssh_conn *sshc,
                        sshstate nowstate)
{
#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(sshc->state != nowstate) {
    CURL_TRC_SSH(దత్తాంశం, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
#అయితే_ముగింపు
  sshc->state = nowstate;
}

#నిర్వచించు MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి అక్షరం *homedir, /* when SFTP is used */
                             అక్షరం **మార్గం) /* returns the  allocated
                                             real path to work with */
{
  అక్షరం *working_మార్గం;
  size_t working_మార్గం_len;
  నిర్మాణం dynbuf npath;
  CURLcode ఫలితం =
    Curl_urldecode(దత్తాంశం->state.పైకి.మార్గం, 0, &working_మార్గం,
                   &working_మార్గం_len, REJECT_ZERO);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  ఒకవేళ((దత్తాంశం->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_మార్గం_len > 3) && (!memcmp(working_మార్గం, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    ఒకవేళ(curlx_dyn_addn(&npath, &working_మార్గం[3], working_మార్గం_len - 3)) {
      curlx_free(working_మార్గం);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  లేకపోతే ఒకవేళ((దత్తాంశం->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_మార్గం) ||
           ((working_మార్గం_len > 2) && !memcmp(working_మార్గం, "/~/", 3)))) {
    ఒకవేళ(curlx_dyn_కూడు(&npath, homedir)) {
      curlx_free(working_మార్గం);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    ఒకవేళ(working_మార్గం_len > 2) {
      size_t len;
      స్థిరరాశి అక్షరం *p;
      పూర్ణాంకం copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      ఒకవేళ(len && (p[len - 1] != '/'))
        copyfrom = 2;

      ఒకవేళ(curlx_dyn_addn(&npath, &working_మార్గం[copyfrom],
                        working_మార్గం_len - copyfrom)) {
        curlx_free(working_మార్గం);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
    లేకపోతే {
      ఒకవేళ(curlx_dyn_కూడు(&npath, "/")) {
        curlx_free(working_మార్గం);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
  }

  ఒకవేళ(curlx_dyn_len(&npath)) {
    curlx_free(working_మార్గం);

    /* store the pointer for the caller to receive */
    *మార్గం = curlx_dyn_ptr(&npath);
  }
  లేకపోతే
    *మార్గం = working_మార్గం;
  DEBUGASSERT(*మార్గం && (*మార్గం)[0]);

  తిరిగి CURLE_OK;
}

#నిర్వచించు MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(స్థిరరాశి అక్షరం **cpp, అక్షరం **మార్గం, స్థిరరాశి అక్షరం *homedir)
{
  స్థిరరాశి అక్షరం *cp = *cpp;
  నిర్మాణం dynbuf బహిర్గతం;
  CURLcode ఫలితం;

  DEBUGASSERT(homedir);
  *మార్గం = ఖాళీ;
  *cpp = ఖాళీ;
  ఒకవేళ(!*cp || !homedir)
    తిరిగి CURLE_QUOTE_ERROR;

  curlx_dyn_init(&బహిర్గతం, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  ఒకవేళ(*cp == '\"' || *cp == '\'') {
    అక్షరం quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    అయితే(*cp != quot) {
      ఒకవేళ(!*cp) /* End of string */
        వెళ్ళు fail;

      ఒకవేళ(*cp == '\\') { /* Escaped characters */
        cp++;
        ఒకవేళ(*cp != '\'' && *cp != '\"' && *cp != '\\')
          వెళ్ళు fail;
      }
      ఫలితం = curlx_dyn_addn(&బహిర్గతం, cp, 1);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      cp++;
    }
    cp++; /* pass the end quote */

    ఒకవేళ(!curlx_dyn_len(&బహిర్గతం))
      వెళ్ళు fail;
  }
  లేకపోతే {
    నిర్మాణం Curl_str మాట;
    bool content = FALSE;
    పూర్ణాంకం rc;
    /* Handling for relative path - prepend home directory */
    ఒకవేళ(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      ఫలితం = curlx_dyn_కూడు(&బహిర్గతం, homedir);
      ఒకవేళ(!ఫలితం)
        ఫలితం = curlx_dyn_addn(&బహిర్గతం, "/", 1);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_మాట(&cp, &మాట, MAX_PATHLENGTH);
    ఒకవేళ(rc) {
      ఒకవేళ(rc == STRE_BIG) {
        curlx_dyn_free(&బహిర్గతం);
        తిరిగి CURLE_TOO_LARGE;
      }
      లేకపోతే ఒకవేళ(!content)
        /* no path, no word, this is incorrect */
        వెళ్ళు fail;
    }
    లేకపోతే {
      /* append the word */
      ఫలితం = curlx_dyn_addn(&బహిర్గతం, curlx_str(&మాట), curlx_strlen(&మాట));
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *మార్గం = curlx_dyn_ptr(&బహిర్గతం);
  తిరిగి CURLE_OK;

fail:
  curlx_dyn_free(&బహిర్గతం);
  తిరిగి CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, గమ్యం;
  పూర్ణాంకం గమ్యం_t;
  పూర్ణాంకం from_t = curlx_str_సంఖ్య(&range, &from, CURL_OFF_T_MAX);
  ఒకవేళ(from_t == STRE_అధికప్రవాహం)
    తిరిగి CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (శూన్య)curlx_str_single(&range, '-');

  గమ్యం_t = curlx_str_numblanks(&range, &గమ్యం);
  ఒకవేళ((గమ్యం_t == STRE_అధికప్రవాహం) || (గమ్యం_t && from_t) || *range)
    తిరిగి CURLE_RANGE_ERROR;

  ఒకవేళ(from_t) {
    /* no start point given, set from relative to end of file */
    ఒకవేళ(!గమ్యం)
      /* "-0" is not a fine range */
      తిరిగి CURLE_RANGE_ERROR;
    లేకపోతే ఒకవేళ(గమ్యం > filesize)
      గమ్యం = filesize;
    from = filesize - గమ్యం;
    గమ్యం = filesize - 1;
  }
  లేకపోతే ఒకవేళ(from > filesize) {
    failf(దత్తాంశం, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    తిరిగి CURLE_RANGE_ERROR;
  }
  లేకపోతే ఒకవేళ((గమ్యం_t == STRE_NO_అంకె) || (గమ్యం >= filesize))
    గమ్యం = filesize - 1;

  ఒకవేళ(from > గమ్యం) {
    failf(దత్తాంశం, "Bad range: start offset larger than end offset");
    తిరిగి CURLE_RANGE_ERROR;
  }
  ఒకవేళ((గమ్యం - from) == CURL_OFF_T_MAX)
    తిరిగి CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = గమ్యం - from + 1;
  తిరిగి CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  పూర్ణాంకం waitfor;

  ఒకవేళ(!sshc || (sock == CURL_SOCKET_BAD))
    తిరిగి CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : దత్తాంశం->req.io_flags;
  ఒకవేళ(waitfor) {
    పూర్ణాంకం flags = 0;
    ఒకవేళ(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_లోపల;
    ఒకవేళ(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_బహిర్గతం;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(దత్తాంశం, "pollset, flags=%x", (గుర్తులేని పూర్ణాంకం)flags);
    తిరిగి Curl_pollset_change(దత్తాంశం, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  ఒకవేళ(sshc->ssh_session)
    తిరిగి Curl_pollset_change(దత్తాంశం, ps, sock, CURL_POLL_లోపల, 0);
  తిరిగి CURLE_OK;
}

CURLcode Curl_ssh_అమరిక_pkey(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ssh_conn *sshc)
{
  అక్షరం *home = ఖాళీ;
  ఒకవేళ(దత్తాంశం->సమితి.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    స్థిరరాశి అక్షరం *str;

    sshc->pub_కీలకం = sshc->priv_కీలకం = ఖాళీ;

    ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSH_వ్యక్తిగత_కీలకం)) {
      sshc->priv_కీలకం = curlx_strdup(
        CURL_EASY_STR(దత్తాంశం, STRING_SSH_వ్యక్తిగత_కీలకం));
      ఒకవేళ(!sshc->priv_కీలకం)
        వెళ్ళు fail;
    }
    లేకపోతే {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      ఒకవేళ(home) {
        /* Try ~/.ssh first. */
        sshc->priv_కీలకం = curl_maprintf("%s/.ssh/id_rsa", home);
        ఒకవేళ(!sshc->priv_కీలకం)
          వెళ్ళు fail;
        లేకపోతే ఒకవేళ(curlx_stat(sshc->priv_కీలకం, &sbuf)) {
          curlx_free(sshc->priv_కీలకం);
          sshc->priv_కీలకం = curl_maprintf("%s/.ssh/id_dsa", home);
          ఒకవేళ(!sshc->priv_కీలకం)
            వెళ్ళు fail;
          లేకపోతే ఒకవేళ(curlx_stat(sshc->priv_కీలకం, &sbuf)) {
            curlx_safefree(sshc->priv_కీలకం);
          }
        }
        curlx_safefree(home);
      }
      ఒకవేళ(!sshc->priv_కీలకం) {
        /* Nothing found; try the current dir. */
        sshc->priv_కీలకం = curlx_strdup("id_rsa");
        ఒకవేళ(sshc->priv_కీలకం && curlx_stat(sshc->priv_కీలకం, &sbuf)) {
          curlx_free(sshc->priv_కీలకం);
          sshc->priv_కీలకం = curlx_strdup("id_dsa");
          ఒకవేళ(sshc->priv_కీలకం && curlx_stat(sshc->priv_కీలకం, &sbuf)) {
            curlx_free(sshc->priv_కీలకం);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_కీలకం = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(దత్తాంశం, STRING_SSH_ప్రజా_కీలకం);
    ఒకవేళ(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_కీలకం = curlx_strdup(str);
      ఒకవేళ(!sshc->pub_కీలకం)
        వెళ్ళు fail;
    }

    {
      స్థిరరాశి అక్షరం *keypasswd = CURL_EASY_STR(దత్తాంశం, STRING_కీలకం_PASSWD);
      ఒకవేళ(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        ఒకవేళ(!sshc->passphrase)
          వెళ్ళు fail;
      }
    }

    ఒకవేళ(sshc->pub_కీలకం)
      infof(దత్తాంశం, "SSH: public key file '%s'", sshc->pub_కీలకం);
    ఒకవేళ(sshc->priv_కీలకం)
      infof(దత్తాంశం, "SSH: private key file '%s'", sshc->priv_కీలకం);
    లేకపోతే
      infof(దత్తాంశం, "SSH: public key auth without private key set!");
  }
  తిరిగి CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_కీలకం);
  curlx_safefree(sshc->pub_కీలకం);
  తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
}

#అయితే_ముగింపు /* USE_SSH */
