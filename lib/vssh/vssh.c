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

#यदि_परिभाषितम् USE_SSH

#अन्तर्भाव "vssh/vssh.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "select.h"  /* for Curl_pollset_change() */
#अन्तर्भाव "url.h"  /* for Curl_conn_meta_get() */
#अन्तर्भाव "curlx/win32-fopen.h"

#यदि_परिभाषितम् CURLVERBOSE
ध्रुव वर्ण *Curl_ssh_statename(sshstate state)
{
  स्थिर ध्रुव वर्ण * ध्रुव नामानि[] = {
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
  DEBUGASSERT(CURL_ARRAYSIZE(नामानि) == SSH_चरमः);
  प्रत्यागम ((size_t)state < CURL_ARRAYSIZE(नामानि)) ? नामानि[state] : "";
}
#यदि_अन्तः /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
शून्य Curl_ssh_समुच्चयः_state(संरचना Curl_easy *दत्त,
                        संरचना ssh_conn *sshc,
                        sshstate nowstate)
{
#यदि_परिभाषितम् CURLVERBOSE
  यदि(sshc->state != nowstate) {
    CURL_TRC_SSH(दत्त, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#अन्यथा
  (शून्य)दत्त;
#यदि_अन्तः
  sshc->state = nowstate;
}

#परिभाषय MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(संरचना Curl_easy *दत्त,
                             ध्रुव वर्ण *homedir, /* when SFTP is used */
                             वर्ण **मार्गः) /* returns the  allocated
                                             real path to work with */
{
  वर्ण *working_मार्गः;
  size_t working_मार्गः_len;
  संरचना dynbuf npath;
  CURLcode परिणाम =
    Curl_urldecode(दत्त->state.ऊर्ध्वम्.मार्गः, 0, &working_मार्गः,
                   &working_मार्गः_len, REJECT_ZERO);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  यदि((दत्त->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_मार्गः_len > 3) && (!memcmp(working_मार्गः, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    यदि(curlx_dyn_addn(&npath, &working_मार्गः[3], working_मार्गः_len - 3)) {
      curlx_free(working_मार्गः);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }
  अन्यथा यदि((दत्त->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!वाक्यं_तुलय("/~", working_मार्गः) ||
           ((working_मार्गः_len > 2) && !memcmp(working_मार्गः, "/~/", 3)))) {
    यदि(curlx_dyn_सङ्कलय(&npath, homedir)) {
      curlx_free(working_मार्गः);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    यदि(working_मार्गः_len > 2) {
      size_t len;
      ध्रुव वर्ण *p;
      पूर्णाङ्क copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      यदि(len && (p[len - 1] != '/'))
        copyfrom = 2;

      यदि(curlx_dyn_addn(&npath, &working_मार्गः[copyfrom],
                        working_मार्गः_len - copyfrom)) {
        curlx_free(working_मार्गः);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
    अन्यथा {
      यदि(curlx_dyn_सङ्कलय(&npath, "/")) {
        curlx_free(working_मार्गः);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
  }

  यदि(curlx_dyn_len(&npath)) {
    curlx_free(working_मार्गः);

    /* store the pointer for the caller to receive */
    *मार्गः = curlx_dyn_ptr(&npath);
  }
  अन्यथा
    *मार्गः = working_मार्गः;
  DEBUGASSERT(*मार्गः && (*मार्गः)[0]);

  प्रत्यागम CURLE_OK;
}

#परिभाषय MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ध्रुव वर्ण **cpp, वर्ण **मार्गः, ध्रुव वर्ण *homedir)
{
  ध्रुव वर्ण *cp = *cpp;
  संरचना dynbuf बहिर्गमनम्;
  CURLcode परिणाम;

  DEBUGASSERT(homedir);
  *मार्गः = रिक्त;
  *cpp = रिक्त;
  यदि(!*cp || !homedir)
    प्रत्यागम CURLE_QUOTE_ERROR;

  curlx_dyn_init(&बहिर्गमनम्, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  यदि(*cp == '\"' || *cp == '\'') {
    वर्ण quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    यावत्(*cp != quot) {
      यदि(!*cp) /* End of string */
        गच्छ fail;

      यदि(*cp == '\\') { /* Escaped characters */
        cp++;
        यदि(*cp != '\'' && *cp != '\"' && *cp != '\\')
          गच्छ fail;
      }
      परिणाम = curlx_dyn_addn(&बहिर्गमनम्, cp, 1);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      cp++;
    }
    cp++; /* pass the end quote */

    यदि(!curlx_dyn_len(&बहिर्गमनम्))
      गच्छ fail;
  }
  अन्यथा {
    संरचना Curl_str word;
    bool content = FALSE;
    पूर्णाङ्क rc;
    /* Handling for relative path - prepend home directory */
    यदि(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      परिणाम = curlx_dyn_सङ्कलय(&बहिर्गमनम्, homedir);
      यदि(!परिणाम)
        परिणाम = curlx_dyn_addn(&बहिर्गमनम्, "/", 1);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_word(&cp, &word, MAX_PATHLENGTH);
    यदि(rc) {
      यदि(rc == STRE_BIG) {
        curlx_dyn_free(&बहिर्गमनम्);
        प्रत्यागम CURLE_TOO_LARGE;
      }
      अन्यथा यदि(!content)
        /* no path, no word, this is incorrect */
        गच्छ fail;
    }
    अन्यथा {
      /* append the word */
      परिणाम = curlx_dyn_addn(&बहिर्गमनम्, curlx_str(&word), curlx_strlen(&word));
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *मार्गः = curlx_dyn_ptr(&बहिर्गमनम्);
  प्रत्यागम CURLE_OK;

fail:
  curlx_dyn_free(&बहिर्गमनम्);
  प्रत्यागम CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(संरचना Curl_easy *दत्त,
                        ध्रुव वर्ण *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, गन्तव्यम्;
  पूर्णाङ्क गन्तव्यम्_t;
  पूर्णाङ्क from_t = curlx_str_सङ्ख्या(&range, &from, CURL_OFF_T_MAX);
  यदि(from_t == STRE_अतिप्रवाहः)
    प्रत्यागम CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (शून्य)curlx_str_single(&range, '-');

  गन्तव्यम्_t = curlx_str_numblanks(&range, &गन्तव्यम्);
  यदि((गन्तव्यम्_t == STRE_अतिप्रवाहः) || (गन्तव्यम्_t && from_t) || *range)
    प्रत्यागम CURLE_RANGE_ERROR;

  यदि(from_t) {
    /* no start point given, set from relative to end of file */
    यदि(!गन्तव्यम्)
      /* "-0" is not a fine range */
      प्रत्यागम CURLE_RANGE_ERROR;
    अन्यथा यदि(गन्तव्यम् > filesize)
      गन्तव्यम् = filesize;
    from = filesize - गन्तव्यम्;
    गन्तव्यम् = filesize - 1;
  }
  अन्यथा यदि(from > filesize) {
    failf(दत्त, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    प्रत्यागम CURLE_RANGE_ERROR;
  }
  अन्यथा यदि((गन्तव्यम्_t == STRE_NO_अङ्कः) || (गन्तव्यम् >= filesize))
    गन्तव्यम् = filesize - 1;

  यदि(from > गन्तव्यम्) {
    failf(दत्त, "Bad range: start offset larger than end offset");
    प्रत्यागम CURLE_RANGE_ERROR;
  }
  यदि((गन्तव्यम् - from) == CURL_OFF_T_MAX)
    प्रत्यागम CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = गन्तव्यम् - from + 1;
  प्रत्यागम CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(संरचना Curl_easy *दत्त, संरचना easy_pollset *ps)
{
  संरचना connectdata *conn = दत्त->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णाङ्क waitfor;

  यदि(!sshc || (sock == CURL_SOCKET_BAD))
    प्रत्यागम CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : दत्त->req.io_flags;
  यदि(waitfor) {
    पूर्णाङ्क flags = 0;
    यदि(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_अन्तर्;
    यदि(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_बहिर्गमनम्;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(दत्त, "pollset, flags=%x", (अचिह्नित पूर्णाङ्क)flags);
    प्रत्यागम Curl_pollset_change(दत्त, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  यदि(sshc->ssh_session)
    प्रत्यागम Curl_pollset_change(दत्त, ps, sock, CURL_POLL_अन्तर्, 0);
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_ssh_सज्जता_pkey(संरचना Curl_easy *दत्त, संरचना ssh_conn *sshc)
{
  वर्ण *home = रिक्त;
  यदि(दत्त->समुच्चयः.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ध्रुव वर्ण *str;

    sshc->pub_कुञ्चिका = sshc->priv_कुञ्चिका = रिक्त;

    यदि(CURL_EASY_STR(दत्त, STRING_SSH_वैयक्तिकम्_कुञ्चिका)) {
      sshc->priv_कुञ्चिका = curlx_strdup(
        CURL_EASY_STR(दत्त, STRING_SSH_वैयक्तिकम्_कुञ्चिका));
      यदि(!sshc->priv_कुञ्चिका)
        गच्छ fail;
    }
    अन्यथा {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      यदि(home) {
        /* Try ~/.ssh first. */
        sshc->priv_कुञ्चिका = curl_maprintf("%s/.ssh/id_rsa", home);
        यदि(!sshc->priv_कुञ्चिका)
          गच्छ fail;
        अन्यथा यदि(curlx_stat(sshc->priv_कुञ्चिका, &sbuf)) {
          curlx_free(sshc->priv_कुञ्चिका);
          sshc->priv_कुञ्चिका = curl_maprintf("%s/.ssh/id_dsa", home);
          यदि(!sshc->priv_कुञ्चिका)
            गच्छ fail;
          अन्यथा यदि(curlx_stat(sshc->priv_कुञ्चिका, &sbuf)) {
            curlx_safefree(sshc->priv_कुञ्चिका);
          }
        }
        curlx_safefree(home);
      }
      यदि(!sshc->priv_कुञ्चिका) {
        /* Nothing found; try the current dir. */
        sshc->priv_कुञ्चिका = curlx_strdup("id_rsa");
        यदि(sshc->priv_कुञ्चिका && curlx_stat(sshc->priv_कुञ्चिका, &sbuf)) {
          curlx_free(sshc->priv_कुञ्चिका);
          sshc->priv_कुञ्चिका = curlx_strdup("id_dsa");
          यदि(sshc->priv_कुञ्चिका && curlx_stat(sshc->priv_कुञ्चिका, &sbuf)) {
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
    str = CURL_EASY_STR(दत्त, STRING_SSH_लौकिकम्_कुञ्चिका);
    यदि(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_कुञ्चिका = curlx_strdup(str);
      यदि(!sshc->pub_कुञ्चिका)
        गच्छ fail;
    }

    {
      ध्रुव वर्ण *keypasswd = CURL_EASY_STR(दत्त, STRING_कुञ्चिका_PASSWD);
      यदि(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        यदि(!sshc->passphrase)
          गच्छ fail;
      }
    }

    यदि(sshc->pub_कुञ्चिका)
      infof(दत्त, "SSH: public key file '%s'", sshc->pub_कुञ्चिका);
    यदि(sshc->priv_कुञ्चिका)
      infof(दत्त, "SSH: private key file '%s'", sshc->priv_कुञ्चिका);
    अन्यथा
      infof(दत्त, "SSH: public key auth without private key set!");
  }
  प्रत्यागम CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_कुञ्चिका);
  curlx_safefree(sshc->pub_कुञ्चिका);
  प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
}

#यदि_अन्तः /* USE_SSH */
