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
#ضمن "curl_setup.h"
#ضمن "vssh/ssh.h"

#جے_تعریف_شدہ USE_SSH

#ضمن "vssh/vssh.h"
#ضمن "curlx/strparse.h"
#ضمن "curl_trc.h"
#ضمن "escape.h"
#ضمن "select.h"  /* for Curl_pollset_change() */
#ضمن "url.h"  /* for Curl_conn_meta_get() */
#ضمن "curlx/win32-fopen.h"

#جے_تعریف_شدہ CURLVERBOSE
ثابت حرف *Curl_ssh_statename(sshstate state)
{
  ساكن ثابت حرف * ثابت ناں[] = {
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
  DEBUGASSERT(CURL_ARRAYSIZE(ناں) == SSH_آخری);
  أرجع ((size_t)state < CURL_ARRAYSIZE(ناں)) ? ناں[state] : "";
}
#جے_انت /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
فراغ Curl_ssh_سیٹ_state(بنية Curl_easy *انکڑے,
                        بنية ssh_conn *sshc,
                        sshstate nowstate)
{
#جے_تعریف_شدہ CURLVERBOSE
  إذا(sshc->state != nowstate) {
    CURL_TRC_SSH(انکڑے, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#وإلا
  (فراغ)انکڑے;
#جے_انت
  sshc->state = nowstate;
}

#تعریف_کرو MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(بنية Curl_easy *انکڑے,
                             ثابت حرف *homedir, /* when SFTP is used */
                             حرف **رستہ) /* returns the  allocated
                                             real path to work with */
{
  حرف *working_رستہ;
  size_t working_رستہ_len;
  بنية dynbuf npath;
  CURLcode نتیجہ =
    Curl_urldecode(انکڑے->state.اتے.رستہ, 0, &working_رستہ,
                   &working_رستہ_len, REJECT_ZERO);
  إذا(نتیجہ)
    أرجع نتیجہ;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  إذا((انکڑے->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_رستہ_len > 3) && (!memcmp(working_رستہ, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    إذا(curlx_dyn_addn(&npath, &working_رستہ[3], working_رستہ_len - 3)) {
      curlx_free(working_رستہ);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }
  وإلا إذا((انکڑے->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_رستہ) ||
           ((working_رستہ_len > 2) && !memcmp(working_رستہ, "/~/", 3)))) {
    إذا(curlx_dyn_جوڑو(&npath, homedir)) {
      curlx_free(working_رستہ);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
    إذا(working_رستہ_len > 2) {
      size_t len;
      ثابت حرف *p;
      صحيح copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      إذا(len && (p[len - 1] != '/'))
        copyfrom = 2;

      إذا(curlx_dyn_addn(&npath, &working_رستہ[copyfrom],
                        working_رستہ_len - copyfrom)) {
        curlx_free(working_رستہ);
        أرجع CURLE_باہرلا_دا_حافظہ;
      }
    }
    وإلا {
      إذا(curlx_dyn_جوڑو(&npath, "/")) {
        curlx_free(working_رستہ);
        أرجع CURLE_باہرلا_دا_حافظہ;
      }
    }
  }

  إذا(curlx_dyn_len(&npath)) {
    curlx_free(working_رستہ);

    /* store the pointer for the caller to receive */
    *رستہ = curlx_dyn_ptr(&npath);
  }
  وإلا
    *رستہ = working_رستہ;
  DEBUGASSERT(*رستہ && (*رستہ)[0]);

  أرجع CURLE_OK;
}

#تعریف_کرو MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ثابت حرف **cpp, حرف **رستہ, ثابت حرف *homedir)
{
  ثابت حرف *cp = *cpp;
  بنية dynbuf باہرلا;
  CURLcode نتیجہ;

  DEBUGASSERT(homedir);
  *رستہ = عدم;
  *cpp = عدم;
  إذا(!*cp || !homedir)
    أرجع CURLE_QUOTE_ERROR;

  curlx_dyn_init(&باہرلا, MAX_PATHLENGTH);

  /* Ignore leading whitespace */
  curlx_str_passblanks(&cp);

  /* Check for quoted filenames */
  إذا(*cp == '\"' || *cp == '\'') {
    حرف quot = *cp++;

    /* Search for terminating quote, unescape some chars */
    طالما(*cp != quot) {
      إذا(!*cp) /* End of string */
        اذهب fail;

      إذا(*cp == '\\') { /* Escaped characters */
        cp++;
        إذا(*cp != '\'' && *cp != '\"' && *cp != '\\')
          اذهب fail;
      }
      نتیجہ = curlx_dyn_addn(&باہرلا, cp, 1);
      إذا(نتیجہ)
        أرجع نتیجہ;
      cp++;
    }
    cp++; /* pass the end quote */

    إذا(!curlx_dyn_len(&باہرلا))
      اذهب fail;
  }
  وإلا {
    بنية Curl_str لفظ;
    bool content = FALSE;
    صحيح rc;
    /* Handling for relative path - prepend home directory */
    إذا(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      نتیجہ = curlx_dyn_جوڑو(&باہرلا, homedir);
      إذا(!نتیجہ)
        نتیجہ = curlx_dyn_addn(&باہرلا, "/", 1);
      إذا(نتیجہ)
        أرجع نتیجہ;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_لفظ(&cp, &لفظ, MAX_PATHLENGTH);
    إذا(rc) {
      إذا(rc == STRE_BIG) {
        curlx_dyn_free(&باہرلا);
        أرجع CURLE_TOO_LARGE;
      }
      وإلا إذا(!content)
        /* no path, no word, this is incorrect */
        اذهب fail;
    }
    وإلا {
      /* append the word */
      نتیجہ = curlx_dyn_addn(&باہرلا, curlx_str(&لفظ), curlx_strlen(&لفظ));
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *رستہ = curlx_dyn_ptr(&باہرلا);
  أرجع CURLE_OK;

fail:
  curlx_dyn_free(&باہرلا);
  أرجع CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(بنية Curl_easy *انکڑے,
                        ثابت حرف *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, منزل;
  صحيح منزل_t;
  صحيح from_t = curlx_str_نمبر(&range, &from, CURL_OFF_T_MAX);
  إذا(from_t == STRE_حد_توں_ودھ)
    أرجع CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (فراغ)curlx_str_single(&range, '-');

  منزل_t = curlx_str_numblanks(&range, &منزل);
  إذا((منزل_t == STRE_حد_توں_ودھ) || (منزل_t && from_t) || *range)
    أرجع CURLE_RANGE_ERROR;

  إذا(from_t) {
    /* no start point given, set from relative to end of file */
    إذا(!منزل)
      /* "-0" is not a fine range */
      أرجع CURLE_RANGE_ERROR;
    وإلا إذا(منزل > filesize)
      منزل = filesize;
    from = filesize - منزل;
    منزل = filesize - 1;
  }
  وإلا إذا(from > filesize) {
    failf(انکڑے, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    أرجع CURLE_RANGE_ERROR;
  }
  وإلا إذا((منزل_t == STRE_NO_عدد) || (منزل >= filesize))
    منزل = filesize - 1;

  إذا(from > منزل) {
    failf(انکڑے, "Bad range: start offset larger than end offset");
    أرجع CURLE_RANGE_ERROR;
  }
  إذا((منزل - from) == CURL_OFF_T_MAX)
    أرجع CURLE_RANGE_ERROR;

  *startp = from;
  *sizep = منزل - from + 1;
  أرجع CURLE_OK;
}

/* called by the multi interface to figure out what socket(s) to wait for and
   for what actions in the DO_DONE, PERFORM and WAITPERFORM states */
CURLcode Curl_ssh_pollset(بنية Curl_easy *انکڑے, بنية easy_pollset *ps)
{
  بنية connectdata *conn = انکڑے->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح waitfor;

  إذا(!sshc || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : انکڑے->req.io_flags;
  إذا(waitfor) {
    صحيح flags = 0;
    إذا(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_وچ;
    إذا(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_باہرلا;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(انکڑے, "pollset, flags=%x", (بنانشانی صحيح)flags);
    أرجع Curl_pollset_change(انکڑے, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  إذا(sshc->ssh_session)
    أرجع Curl_pollset_change(انکڑے, ps, sock, CURL_POLL_وچ, 0);
  أرجع CURLE_OK;
}

CURLcode Curl_ssh_تیاری_pkey(بنية Curl_easy *انکڑے, بنية ssh_conn *sshc)
{
  حرف *home = عدم;
  إذا(انکڑے->سیٹ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ثابت حرف *str;

    sshc->pub_کلید = sshc->priv_کلید = عدم;

    إذا(CURL_EASY_STR(انکڑے, STRING_SSH_نجی_کلید)) {
      sshc->priv_کلید = curlx_strdup(
        CURL_EASY_STR(انکڑے, STRING_SSH_نجی_کلید));
      إذا(!sshc->priv_کلید)
        اذهب fail;
    }
    وإلا {
      /* To ponder about: should really the lib be messing about with the HOME
         environment variable etc? */
      curlx_struct_stat sbuf;
      home = curl_getenv("HOME");

      /* If no private key file is specified, try some common paths. */
      إذا(home) {
        /* Try ~/.ssh first. */
        sshc->priv_کلید = curl_maprintf("%s/.ssh/id_rsa", home);
        إذا(!sshc->priv_کلید)
          اذهب fail;
        وإلا إذا(curlx_stat(sshc->priv_کلید, &sbuf)) {
          curlx_free(sshc->priv_کلید);
          sshc->priv_کلید = curl_maprintf("%s/.ssh/id_dsa", home);
          إذا(!sshc->priv_کلید)
            اذهب fail;
          وإلا إذا(curlx_stat(sshc->priv_کلید, &sbuf)) {
            curlx_safefree(sshc->priv_کلید);
          }
        }
        curlx_safefree(home);
      }
      إذا(!sshc->priv_کلید) {
        /* Nothing found; try the current dir. */
        sshc->priv_کلید = curlx_strdup("id_rsa");
        إذا(sshc->priv_کلید && curlx_stat(sshc->priv_کلید, &sbuf)) {
          curlx_free(sshc->priv_کلید);
          sshc->priv_کلید = curlx_strdup("id_dsa");
          إذا(sshc->priv_کلید && curlx_stat(sshc->priv_کلید, &sbuf)) {
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
    str = CURL_EASY_STR(انکڑے, STRING_SSH_عوامی_کلید);
    إذا(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_کلید = curlx_strdup(str);
      إذا(!sshc->pub_کلید)
        اذهب fail;
    }

    {
      ثابت حرف *keypasswd = CURL_EASY_STR(انکڑے, STRING_کلید_PASSWD);
      إذا(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        إذا(!sshc->passphrase)
          اذهب fail;
      }
    }

    إذا(sshc->pub_کلید)
      infof(انکڑے, "SSH: public key file '%s'", sshc->pub_کلید);
    إذا(sshc->priv_کلید)
      infof(انکڑے, "SSH: private key file '%s'", sshc->priv_کلید);
    وإلا
      infof(انکڑے, "SSH: public key auth without private key set!");
  }
  أرجع CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_کلید);
  curlx_safefree(sshc->pub_کلید);
  أرجع CURLE_باہرلا_دا_حافظہ;
}

#جے_انت /* USE_SSH */
