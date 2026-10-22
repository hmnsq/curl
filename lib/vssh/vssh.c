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

#اگر_تعریف_شدہ USE_SSH

#شامل "vssh/vssh.h"
#شامل "curlx/strparse.h"
#شامل "curl_trc.h"
#شامل "escape.h"
#شامل "select.h"  /* for Curl_pollset_change() */
#شامل "url.h"  /* for Curl_conn_meta_get() */
#شامل "curlx/win32-fopen.h"

#اگر_تعریف_شدہ CURLVERBOSE
ثابت حرف *Curl_ssh_statename(sshstate state)
{
  ساکن ثابت حرف * ثابت نام[] = {
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
  DEBUGASSERT(CURL_ARRAYSIZE(نام) == SSH_آخری);
  واپس ((size_t)state < CURL_ARRAYSIZE(نام)) ? نام[state] : "";
}
#اگر_ختم /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
خالی Curl_ssh_سیٹ_state(ساخت Curl_easy *ڈیٹا,
                        ساخت ssh_conn *sshc,
                        sshstate nowstate)
{
#اگر_تعریف_شدہ CURLVERBOSE
  اگر(sshc->state != nowstate) {
    CURL_TRC_SSH(ڈیٹا, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#ورنہ
  (خالی)ڈیٹا;
#اگر_ختم
  sshc->state = nowstate;
}

#تعریف_کرو MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(ساخت Curl_easy *ڈیٹا,
                             ثابت حرف *homedir, /* when SFTP is used */
                             حرف **راستہ) /* returns the  allocated
                                             real path to work with */
{
  حرف *working_راستہ;
  size_t working_راستہ_len;
  ساخت dynbuf npath;
  CURLcode نتیجہ =
    Curl_urldecode(ڈیٹا->state.اوپر.راستہ, 0, &working_راستہ,
                   &working_راستہ_len, REJECT_ZERO);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  اگر((ڈیٹا->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_راستہ_len > 3) && (!memcmp(working_راستہ, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    اگر(curlx_dyn_addn(&npath, &working_راستہ[3], working_راستہ_len - 3)) {
      curlx_free(working_راستہ);
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }
  ورنہ اگر((ڈیٹا->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_راستہ) ||
           ((working_راستہ_len > 2) && !memcmp(working_راستہ, "/~/", 3)))) {
    اگر(curlx_dyn_جوڑو(&npath, homedir)) {
      curlx_free(working_راستہ);
      واپس CURLE_برآمد_کا_حافظہ;
    }
    اگر(working_راستہ_len > 2) {
      size_t len;
      ثابت حرف *p;
      صحیح copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      اگر(len && (p[len - 1] != '/'))
        copyfrom = 2;

      اگر(curlx_dyn_addn(&npath, &working_راستہ[copyfrom],
                        working_راستہ_len - copyfrom)) {
        curlx_free(working_راستہ);
        واپس CURLE_برآمد_کا_حافظہ;
      }
    }
    ورنہ {
      اگر(curlx_dyn_جوڑو(&npath, "/")) {
        curlx_free(working_راستہ);
        واپس CURLE_برآمد_کا_حافظہ;
      }
    }
  }

  اگر(curlx_dyn_len(&npath)) {
    curlx_free(working_راستہ);

    /* store the pointer for the caller to receive */
    *راستہ = curlx_dyn_ptr(&npath);
  }
  ورنہ
    *راستہ = working_راستہ;
  DEBUGASSERT(*راستہ && (*راستہ)[0]);

  واپس CURLE_OK;
}

#تعریف_کرو MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ثابت حرف **cpp, حرف **راستہ, ثابت حرف *homedir)
{
  ثابت حرف *cp = *cpp;
  ساخت dynbuf برآمد;
  CURLcode نتیجہ;

  DEBUGASSERT(homedir);
  *راستہ = تہی;
  *cpp = تہی;
  اگر(!*cp || !homedir)
    واپس CURLE_QUOTE_ERROR;

  curlx_dyn_init(&برآمد, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  اگر(*cp == '\"' || *cp == '\'') {
    حرف quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    جب(*cp != quot) {
      اگر(!*cp) /* End of string */
        جاؤ fail;

      اگر(*cp == '\\') { /* Escaped characters */
        cp++;
        اگر(*cp != '\'' && *cp != '\"' && *cp != '\\')
          جاؤ fail;
      }
      نتیجہ = curlx_dyn_addn(&برآمد, cp, 1);
      اگر(نتیجہ)
        واپس نتیجہ;
      cp++;
    }
    cp++; /* pass the end quote */

    اگر(!curlx_dyn_len(&برآمد))
      جاؤ fail;
  }
  ورنہ {
    ساخت Curl_str لفظ;
    bool content = FALSE;
    صحیح rc;
    /* Handling for relative path - prepend home directory */
    اگر(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      نتیجہ = curlx_dyn_جوڑو(&برآمد, homedir);
      اگر(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&برآمد, "/", 1);
      اگر(نتیجہ)
        واپس نتیجہ;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_لفظ(&cp, &لفظ, MAX_PATHLENGTH);
    اگر(rc) {
      اگر(rc == STRE_BIG) {
        curlx_dyn_free(&برآمد);
        واپس CURLE_TOO_LARGE;
      }
      ورنہ اگر(!content)
        /* no path, no word, this is incorrect */
        جاؤ fail;
    }
    ورنہ {
      /* append the word */
      نتیجہ = curlx_dyn_addn(&برآمد, curlx_str(&لفظ), curlx_strlen(&لفظ));
      اگر(نتیجہ)
        واپس نتیجہ;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *راستہ = curlx_dyn_ptr(&برآمد);
  واپس CURLE_OK;

fail:
  curlx_dyn_free(&برآمد);
  واپس CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(ساخت Curl_easy *ڈیٹا,
                        ثابت حرف *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, مقصد;
  صحیح مقصد_t;
  صحیح from_t = curlx_str_نمبر(&range, &from, CURL_OFF_T_MAX);
  اگر(from_t == STRE_حد_سے_تجاوز)
    واپس CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (خالی)curlx_str_single(&range, '-');

  مقصد_t = curlx_str_numblanks(&range, &مقصد);
  اگر((مقصد_t == STRE_حد_سے_تجاوز) || (مقصد_t && from_t) || *range)
    واپس CURLE_RANGE_ERROR;

  اگر(from_t) {
    /* no start point given, set from relative to end of file */
    اگر(!مقصد)
      /* "-0" is not a fine range */
      واپس CURLE_RANGE_ERROR;
    ورنہ اگر(مقصد > filesize)
      مقصد = filesize;
    from = filesize - مقصد;
    مقصد = filesize - 1;
  }
  ورنہ اگر(from > filesize) {
    failf(ڈیٹا, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    واپس CURLE_RANGE_ERROR;
  }
  ورنہ اگر((مقصد_t == STRE_NO_عدد) || (مقصد >= filesize))
    مقصد = filesize - 1;

  اگر(from > مقصد) {
    failf(ڈیٹا, "Bad range: start offset larger than end offset");
    واپس CURLE_RANGE_ERROR;
  }
  اگر((مقصد - from) == CURL_OFF_T_MAX)
    واپس CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = مقصد - from + 1;
  واپس CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(ساخت Curl_easy *ڈیٹا, ساخت easy_pollset *ps)
{
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحیح waitfor;

  اگر(!sshc || (sock == CURL_SOCKET_BAD))
    واپس CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : ڈیٹا->req.io_flags;
  اگر(waitfor) {
    صحیح flags = 0;
    اگر(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_اندر;
    اگر(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_برآمد;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(ڈیٹا, "pollset, flags=%x", (غیرعلامتی صحیح)flags);
    واپس Curl_pollset_change(ڈیٹا, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  اگر(sshc->ssh_session)
    واپس Curl_pollset_change(ڈیٹا, ps, sock, CURL_POLL_اندر, 0);
  واپس CURLE_OK;
}

CURLcode Curl_ssh_تیاری_pkey(ساخت Curl_easy *ڈیٹا, ساخت ssh_conn *sshc)
{
  حرف *home = تہی;
  اگر(ڈیٹا->سیٹ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ثابت حرف *str;

    sshc->pub_کلید = sshc->priv_کلید = تہی;

    اگر(CURL_EASY_STR(ڈیٹا, STRING_SSH_نجی_کلید)) {
      sshc->priv_کلید = curlx_strdup(
        CURL_EASY_STR(ڈیٹا, STRING_SSH_نجی_کلید));
      اگر(!sshc->priv_کلید)
        جاؤ fail;
    }
    ورنہ {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      اگر(home) {
        /* Try ~/.ssh first. */
        sshc->priv_کلید = curl_maprintf("%s/.ssh/id_rsa", home);
        اگر(!sshc->priv_کلید)
          جاؤ fail;
        ورنہ اگر(curlx_stat(sshc->priv_کلید, &sbuf)) {
          curlx_free(sshc->priv_کلید);
          sshc->priv_کلید = curl_maprintf("%s/.ssh/id_dsa", home);
          اگر(!sshc->priv_کلید)
            جاؤ fail;
          ورنہ اگر(curlx_stat(sshc->priv_کلید, &sbuf)) {
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
    str = CURL_EASY_STR(ڈیٹا, STRING_SSH_عوامی_کلید);
    اگر(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_کلید = curlx_strdup(str);
      اگر(!sshc->pub_کلید)
        جاؤ fail;
    }

    {
      ثابت حرف *keypasswd = CURL_EASY_STR(ڈیٹا, STRING_کلید_PASSWD);
      اگر(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        اگر(!sshc->passphrase)
          جاؤ fail;
      }
    }

    اگر(sshc->pub_کلید)
      infof(ڈیٹا, "SSH: public key file '%s'", sshc->pub_کلید);
    اگر(sshc->priv_کلید)
      infof(ڈیٹا, "SSH: private key file '%s'", sshc->priv_کلید);
    ورنہ
      infof(ڈیٹا, "SSH: public key auth without private key set!");
  }
  واپس CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_کلید);
  curlx_safefree(sshc->pub_کلید);
  واپس CURLE_برآمد_کا_حافظہ;
}

#اگر_ختم /* USE_SSH */
