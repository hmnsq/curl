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

#جيڪڏهن_وصف_ٿيل USE_SSH

#ضمن "vssh/vssh.h"
#ضمن "curlx/strparse.h"
#ضمن "curl_trc.h"
#ضمن "escape.h"
#ضمن "select.h"  /* for Curl_pollset_change() */
#ضمن "url.h"  /* for Curl_conn_meta_get() */
#ضمن "curlx/win32-fopen.h"

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ثابت حرف *Curl_ssh_statename(sshstate state)
{
  ساكن ثابت حرف * ثابت نالا[] = {
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
  DEBUGASSERT(CURL_ARRAYSIZE(نالا) == SSH_آخري);
  أرجع ((size_t)state < CURL_ARRAYSIZE(نالا)) ? نالا[state] : "";
}
#جيڪڏهن_پورو /* CURLVERBOSE */

/*
 * SSH State machine related code
 */
/* This is the ONLY way to change SSH state! */
فراغ Curl_ssh_سيٽ_state(بنية Curl_easy *معلومات,
                        بنية ssh_conn *sshc,
                        sshstate nowstate)
{
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  إذا(sshc->state != nowstate) {
    CURL_TRC_SSH(معلومات, "[%s] -> [%s]",
                 Curl_ssh_statename(sshc->state),
                 Curl_ssh_statename(nowstate));
  }
#وإلا
  (فراغ)معلومات;
#جيڪڏهن_پورو
  sshc->state = nowstate;
}

#وصف_ڪريو MAX_SSHPATH_LEN 100000 /* arbitrary */

/* figure out the path to work with in this particular request */
CURLcode Curl_getworkingpath(بنية Curl_easy *معلومات,
                             ثابت حرف *homedir, /* when SFTP is used */
                             حرف **رستو) /* returns the  allocated
                                             real path to work with */
{
  حرف *working_رستو;
  size_t working_رستو_len;
  بنية dynbuf npath;
  CURLcode نتيجو =
    Curl_urldecode(معلومات->state.مٿي.رستو, 0, &working_رستو,
                   &working_رستو_len, REJECT_ZERO);
  إذا(نتيجو)
    أرجع نتيجو;

  /* new path to switch to in case we need to */
  curlx_dyn_init(&npath, MAX_SSHPATH_LEN);

  /* Check for /~/, indicating relative to the user's home directory */
  إذا((معلومات->conn->scheme->protocol & CURLPROTO_SCP) &&
     (working_رستو_len > 3) && (!memcmp(working_رستو, "/~/", 3))) {
    /* It is referenced to the home directory, so strip the leading '/~/' */
    إذا(curlx_dyn_addn(&npath, &working_رستو[3], working_رستو_len - 3)) {
      curlx_free(working_رستو);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }
  وإلا إذا((معلومات->conn->scheme->protocol & CURLPROTO_SFTP) &&
          (!strcmp("/~", working_رستو) ||
           ((working_رستو_len > 2) && !memcmp(working_رستو, "/~/", 3)))) {
    إذا(curlx_dyn_جوڙيو(&npath, homedir)) {
      curlx_free(working_رستو);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    إذا(working_رستو_len > 2) {
      size_t len;
      ثابت حرف *p;
      صحيح copyfrom = 3;
      /* Copy a separating '/' if homedir does not end with one */
      len = curlx_dyn_len(&npath);
      p = curlx_dyn_ptr(&npath);
      إذا(len && (p[len - 1] != '/'))
        copyfrom = 2;

      إذا(curlx_dyn_addn(&npath, &working_رستو[copyfrom],
                        working_رستو_len - copyfrom)) {
        curlx_free(working_رستو);
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      }
    }
    وإلا {
      إذا(curlx_dyn_جوڙيو(&npath, "/")) {
        curlx_free(working_رستو);
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      }
    }
  }

  إذا(curlx_dyn_len(&npath)) {
    curlx_free(working_رستو);

    /* store the pointer for the caller to receive */
    *رستو = curlx_dyn_ptr(&npath);
  }
  وإلا
    *رستو = working_رستو;
  DEBUGASSERT(*رستو && (*رستو)[0]);

  أرجع CURLE_OK;
}

#وصف_ڪريو MAX_PATHLENGTH 65535 /* arbitrary long */

CURLcode Curl_get_pathname(ثابت حرف **cpp, حرف **رستو, ثابت حرف *homedir)
{
  ثابت حرف *cp = *cpp;
  بنية dynbuf ٻاهريون;
  CURLcode نتيجو;

  DEBUGASSERT(homedir);
  *رستو = عدم;
  *cpp = عدم;
  إذا(!*cp || !homedir)
    أرجع CURLE_QUOTE_ERROR;

  curlx_dyn_init(&ٻاهريون, MAX_PATHLENGTH);

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
      نتيجو = curlx_dyn_addn(&ٻاهريون, cp, 1);
      إذا(نتيجو)
        أرجع نتيجو;
      cp++;
    }
    cp++; /* pass the end quote */

    إذا(!curlx_dyn_len(&ٻاهريون))
      اذهب fail;
  }
  وإلا {
    بنية Curl_str لفظ;
    bool content = FALSE;
    صحيح rc;
    /* Handling for relative path - prepend home directory */
    إذا(cp[0] == '/' && cp[1] == '~' && cp[2] == '/') {
      نتيجو = curlx_dyn_جوڙيو(&ٻاهريون, homedir);
      إذا(!نتيجو)
        نتيجو = curlx_dyn_addn(&ٻاهريون, "/", 1);
      إذا(نتيجو)
        أرجع نتيجو;
      cp += 3;
      content = TRUE;
    }
    /* Read to end of filename - either to whitespace or terminator */
    rc = curlx_str_لفظ(&cp, &لفظ, MAX_PATHLENGTH);
    إذا(rc) {
      إذا(rc == STRE_BIG) {
        curlx_dyn_free(&ٻاهريون);
        أرجع CURLE_TOO_LARGE;
      }
      وإلا إذا(!content)
        /* no path, no word, this is incorrect */
        اذهب fail;
    }
    وإلا {
      /* append the word */
      نتيجو = curlx_dyn_addn(&ٻاهريون, curlx_str(&لفظ), curlx_strlen(&لفظ));
      إذا(نتيجو)
        أرجع نتيجو;
    }
  }
  /* skip whitespace */
  curlx_str_passblanks(&cp);

  /* return pointer to second parameter if it exists */
  *cpp = cp;

  *رستو = curlx_dyn_ptr(&ٻاهريون);
  أرجع CURLE_OK;

fail:
  curlx_dyn_free(&ٻاهريون);
  أرجع CURLE_QUOTE_ERROR;
}

CURLcode Curl_ssh_range(بنية Curl_easy *معلومات,
                        ثابت حرف *range, curl_off_t filesize,
                        curl_off_t *startp, curl_off_t *sizep)
{
  curl_off_t from, منزل;
  صحيح منزل_t;
  صحيح from_t = curlx_str_عدد(&range, &from, CURL_OFF_T_MAX);
  إذا(from_t == STRE_حد_کان_مٿي)
    أرجع CURLE_RANGE_ERROR;
  curlx_str_passblanks(&range);
  (فراغ)curlx_str_single(&range, '-');

  منزل_t = curlx_str_numblanks(&range, &منزل);
  إذا((منزل_t == STRE_حد_کان_مٿي) || (منزل_t && from_t) || *range)
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
    failf(معلومات, "Offset (%" FMT_OFF_T ") was beyond file size (%"
          FMT_OFF_T ")", from, filesize);
    أرجع CURLE_RANGE_ERROR;
  }
  وإلا إذا((منزل_t == STRE_NO_انگ) || (منزل >= filesize))
    منزل = filesize - 1;

  إذا(from > منزل) {
    failf(معلومات, "Bad range: start offset larger than end offset");
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
CURLcode Curl_ssh_pollset(بنية Curl_easy *معلومات, بنية easy_pollset *ps)
{
  بنية connectdata *conn = معلومات->conn;
  بنية ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  صحيح waitfor;

  إذا(!sshc || (sock == CURL_SOCKET_BAD))
    أرجع CURLE_FAILED_INIT;

  waitfor = sshc->waitfor ? sshc->waitfor : معلومات->req.io_flags;
  إذا(waitfor) {
    صحيح flags = 0;
    إذا(waitfor & REQ_IO_RECV)
      flags |= CURL_POLL_اندريون;
    إذا(waitfor & REQ_IO_SEND)
      flags |= CURL_POLL_ٻاهريون;
    DEBUGASSERT(flags);
    CURL_TRC_SSH(معلومات, "pollset, flags=%x", (بنانشان صحيح)flags);
    أرجع Curl_pollset_change(معلومات, ps, sock, flags, 0);
  }
  /* While we still have a session, we listen incoming data. */
  إذا(sshc->ssh_session)
    أرجع Curl_pollset_change(معلومات, ps, sock, CURL_POLL_اندريون, 0);
  أرجع CURLE_OK;
}

CURLcode Curl_ssh_تياري_pkey(بنية Curl_easy *معلومات, بنية ssh_conn *sshc)
{
  حرف *home = عدم;
  إذا(معلومات->سيٽ.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY) {
    ثابت حرف *str;

    sshc->pub_ڪنجي = sshc->priv_ڪنجي = عدم;

    إذا(CURL_EASY_STR(معلومات, STRING_SSH_ذاتي_ڪنجي)) {
      sshc->priv_ڪنجي = curlx_strdup(
        CURL_EASY_STR(معلومات, STRING_SSH_ذاتي_ڪنجي));
      إذا(!sshc->priv_ڪنجي)
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
        sshc->priv_ڪنجي = curl_maprintf("%s/.ssh/id_rsa", home);
        إذا(!sshc->priv_ڪنجي)
          اذهب fail;
        وإلا إذا(curlx_stat(sshc->priv_ڪنجي, &sbuf)) {
          curlx_free(sshc->priv_ڪنجي);
          sshc->priv_ڪنجي = curl_maprintf("%s/.ssh/id_dsa", home);
          إذا(!sshc->priv_ڪنجي)
            اذهب fail;
          وإلا إذا(curlx_stat(sshc->priv_ڪنجي, &sbuf)) {
            curlx_safefree(sshc->priv_ڪنجي);
          }
        }
        curlx_safefree(home);
      }
      إذا(!sshc->priv_ڪنجي) {
        /* Nothing found; try the current dir. */
        sshc->priv_ڪنجي = curlx_strdup("id_rsa");
        إذا(sshc->priv_ڪنجي && curlx_stat(sshc->priv_ڪنجي, &sbuf)) {
          curlx_free(sshc->priv_ڪنجي);
          sshc->priv_ڪنجي = curlx_strdup("id_dsa");
          إذا(sshc->priv_ڪنجي && curlx_stat(sshc->priv_ڪنجي, &sbuf)) {
            curlx_free(sshc->priv_ڪنجي);
            /* Out of guesses. Set to the empty string to avoid
             * surprising info messages. */
            sshc->priv_ڪنجي = curlx_strdup("");
          }
        }
      }
    }

    /*
     * Unless the user explicitly specifies a public key file, let the SSH
     * library extract the public key from the private key file. This is done
     * by passing sshc->pub_key = NULL.
     */
    str = CURL_EASY_STR(معلومات, STRING_SSH_عوامي_ڪنجي);
    إذا(str && *str) {  /* treat empty string the same way as NULL */
      sshc->pub_ڪنجي = curlx_strdup(str);
      إذا(!sshc->pub_ڪنجي)
        اذهب fail;
    }

    {
      ثابت حرف *keypasswd = CURL_EASY_STR(معلومات, STRING_ڪنجي_PASSWD);
      إذا(keypasswd) {
        sshc->passphrase = curlx_strdup(keypasswd);
        إذا(!sshc->passphrase)
          اذهب fail;
      }
    }

    إذا(sshc->pub_ڪنجي)
      infof(معلومات, "SSH: public key file '%s'", sshc->pub_ڪنجي);
    إذا(sshc->priv_ڪنجي)
      infof(معلومات, "SSH: private key file '%s'", sshc->priv_ڪنجي);
    وإلا
      infof(معلومات, "SSH: public key auth without private key set!");
  }
  أرجع CURLE_OK;

fail:
  curlx_safefree(home);
  curlx_safefree(sshc->priv_ڪنجي);
  curlx_safefree(sshc->pub_ڪنجي);
  أرجع CURLE_ٻاهريون_جو_يادگيري;
}

#جيڪڏهن_پورو /* USE_SSH */
