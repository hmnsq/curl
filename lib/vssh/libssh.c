/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Red Hat, Inc.
 *
 * Authors: Nikos Mavrogiannopoulos, Tomas Mraz, Stanislav Zidek,
 *          Robert Kolcun, Andreas Schneider
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
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#अन्तर्भाव "curl_setup.h"

#ifdef USE_LIBSSH

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef __VMS
#अन्तर्भाव <in.h>
#अन्तर्भाव <inet.h>
#endif

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "vssh/ssh.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "parsedate.h"          /* for the week day and month names */
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "vssh/vssh.h"
#अन्तर्भाव "curlx/base64.h" /* for curlx_base64_encode() */

#ifdef HAVE_UNISTD_H
#अन्तर्भाव <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#अन्तर्भाव <fcntl.h>
#endif

/* A recent macro provided by libssh. Or make our own. */
#ifndef SSH_STRING_FREE_CHAR
#define SSH_STRING_FREE_CHAR(x) \
  करा {                          \
    जर(x) {                     \
      ssh_string_free_char(x);  \
      x = रिक्त;                 \
    }                           \
  } पर्यंत(0)
#endif

/* These stat values may not be the same as the user's S_IFMT / S_IFLNK */
#ifndef SSH_S_IFMT
#define SSH_S_IFMT   00170000
#endif
#ifndef SSH_S_IFLNK
#define SSH_S_IFLNK  0120000
#endif

स्थिर CURLcode sftp_error_to_CURLE(पूर्णांक err)
{
  निवड(err) {
  स्थिती SSH_FX_OK:
    परत CURLE_OK;

  स्थिती SSH_FX_NO_SUCH_FILE:
  स्थिती SSH_FX_NO_SUCH_PATH:
    परत CURLE_REMOTE_FILE_NOT_FOUND;

  स्थिती SSH_FX_PERMISSION_DENIED:
  स्थिती SSH_FX_WRITE_PROTECT:
    परत CURLE_REMOTE_ACCESS_DENIED;

  स्थिती SSH_FX_FILE_ALREADY_EXISTS:
    परत CURLE_REMOTE_FILE_EXISTS;

  पूर्वनिर्धारित:
    थांबा;
  }

  परत CURLE_SSH;
}

/* Multiple options:
 * 1. CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_SHA256) is set
 *    with a SHA256 hash.
 * 2. CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_KEY_MD5) is set
 *    with an MD5 hash (90s style auth, not sure we should have it here)
 * 3. data->set.ssh_keyfunc callback is set. Then we do trust on first
 *    use. We even save on knownhosts if CURLKHSTAT_FINE_ADD_TO_FILE
 *    is returned by it.
 * 4. none of the above. We only accept if it is present on known hosts.
 *
 * Returns SSH_OK or SSH_ERROR.
 */
स्थिर पूर्णांक myssh_is_known(संरचना Curl_easy *data, संरचना ssh_conn *sshc)
{
  पूर्णांक rc;
  ssh_किल्ली pubkey;
  अचिन्हित अक्षर *hash_sha256 = रिक्त;
  माप_प्रकार hlen_sha256;
  अचिन्हित अक्षर *hash_md5 = रिक्त;
  माप_प्रकार hlen_md5;
  अक्षर *found_base64 = रिक्त;
  अक्षर *known_base64 = रिक्त;
  पूर्णांक vstate;
  गणना curl_khmatch keymatch;
  संरचना curl_khkey foundkey;
  संरचना curl_khkey *knownkeyp = रिक्त;
  curl_sshkeycallback func = data->set.ssh_keyfunc;
  संरचना ssh_knownhosts_entry *knownhostsentry = रिक्त;
  संरचना curl_khkey knownkey;

  rc = ssh_get_server_publickey(sshc->ssh_session, &pubkey);

  जर(rc != SSH_OK)
    परत rc;

  जर(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_किल्ली_SHA256)) {
    स्थिरांक अक्षर *pubkey_sha256 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_किल्ली_SHA256);
    अक्षर *fingerprint_b64 = रिक्त;
    माप_प्रकार fingerprint_b64_len;
    माप_प्रकार pub_pos = 0;
    माप_प्रकार b64_pos = 0;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA256,
                                &hash_sha256, &hlen_sha256);
    जर(rc != SSH_OK || hlen_sha256 != 32) {
      failf(data, "Denied establishing ssh session: "
            "SHA256 fingerprint not available");
      जा cleanup;
    }

    जर(curlx_base64_encode((स्थिरांक uint8_t *)hash_sha256, 32, &fingerprint_b64,
                           &fingerprint_b64_len) != CURLE_OK) {
      rc = SSH_ERROR;
      जा cleanup;
    }

    infof(data, "SSH SHA256 fingerprint: %s", fingerprint_b64);

    /* Find the position of any = padding characters in the public key */
    पर्यंत((pubkey_sha256[pub_pos] != '=') && pubkey_sha256[pub_pos]) {
      pub_pos++;
    }

    /* Find the position of any = padding characters in the base64 coded
     * hostkey fingerprint */
    पर्यंत((fingerprint_b64[b64_pos] != '=') && fingerprint_b64[b64_pos]) {
      b64_pos++;
    }

    /* Before we authenticate we check the hostkey's SHA256 fingerprint
     * against a known fingerprint, if available.
     */
    जर((pub_pos != b64_pos) ||
       म_भेदन(fingerprint_b64, pubkey_sha256, pub_pos)) {
      failf(data,
            "Denied establishing ssh session: mismatch SHA256 fingerprint. "
            "Remote %s is not equal to %s", fingerprint_b64, pubkey_sha256);
      curlx_free(fingerprint_b64);
      rc = SSH_ERROR;
      जा cleanup;
    }

    curlx_free(fingerprint_b64);

    rc = SSH_OK;
    जा cleanup;
  }

  जर(CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_किल्ली_MD5)) {
    स्थिरांक अक्षर *pubkey_md5 =
      CURL_EASY_STR(data, STRING_SSH_HOST_PUBLIC_किल्ली_MD5);
    अक्षर md5buffer[33];
    पूर्णांक i;

    rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_MD5,
                                &hash_md5, &hlen_md5);
    जर(rc != SSH_OK || hlen_md5 != 16) {
      failf(data,
            "Denied establishing ssh session: MD5 fingerprint not available");
      जा cleanup;
    }

    साठी(i = 0; i < 16; i++)
      curl_msnprintf(&md5buffer[i * 2], 3, "%02x", hash_md5[i]);

    infof(data, "SSH MD5 fingerprint: %s", md5buffer);

    जर(!curl_strequal(md5buffer, pubkey_md5)) {
      failf(data,
            "Denied establishing ssh session: mismatch MD5 fingerprint. "
            "Remote %s is not equal to %s", md5buffer, pubkey_md5);
      rc = SSH_ERROR;
      जा cleanup;
    }

    rc = SSH_OK;
    जा cleanup;
  }

  जर(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {

    /* Get the known_key from the known hosts file */
    vstate = ssh_session_get_known_hosts_entry(sshc->ssh_session,
                                               &knownhostsentry);

    /* Case an entry was found in a known hosts file */
    जर(knownhostsentry) {
      जर(knownhostsentry->publickey) {
        rc = ssh_pki_export_pubkey_base64(knownhostsentry->publickey,
                                          &known_base64);
        जर(rc != SSH_OK) {
          जा cleanup;
        }
        knownkey.किल्ली = known_base64;
        knownkey.len = म_माप(known_base64);

        निवड(ssh_किल्ली_type(knownhostsentry->publickey)) {
        स्थिती SSH_KEYTYPE_RSA:
          knownkey.keytype = CURLKHTYPE_RSA;
          थांबा;
        स्थिती SSH_KEYTYPE_RSA1:
          knownkey.keytype = CURLKHTYPE_RSA1;
          थांबा;
        स्थिती SSH_KEYTYPE_ECDSA:
        स्थिती SSH_KEYTYPE_ECDSA_P256:
        स्थिती SSH_KEYTYPE_ECDSA_P384:
        स्थिती SSH_KEYTYPE_ECDSA_P521:
          knownkey.keytype = CURLKHTYPE_ECDSA;
          थांबा;
        स्थिती SSH_KEYTYPE_ED25519:
          knownkey.keytype = CURLKHTYPE_ED25519;
          थांबा;
        स्थिती SSH_KEYTYPE_DSS:
          knownkey.keytype = CURLKHTYPE_DSS;
          थांबा;
        पूर्वनिर्धारित:
          rc = SSH_ERROR;
          जा cleanup;
        }
        knownkeyp = &knownkey;
      }
    }

    निवड(vstate) {
    स्थिती SSH_KNOWN_HOSTS_OK:
      keymatch = CURLKHMATCH_OK;
      थांबा;
    स्थिती SSH_KNOWN_HOSTS_OTHER:
      keymatch = CURLKHMATCH_MISMATCH;
      थांबा;
    स्थिती SSH_KNOWN_HOSTS_NOT_FOUND:
    स्थिती SSH_KNOWN_HOSTS_UNKNOWN:
    स्थिती SSH_KNOWN_HOSTS_ERROR:
      keymatch = CURLKHMATCH_MISSING;
      थांबा;
    पूर्वनिर्धारित:
      keymatch = CURLKHMATCH_MISMATCH;
      थांबा;
    }

    जर(func) { /* use callback to determine action */
      संरचना Curl_mapi_guard guard;

      rc = ssh_pki_export_pubkey_base64(pubkey, &found_base64);
      जर(rc != SSH_OK)
        जा cleanup;

      foundkey.किल्ली = found_base64;
      foundkey.len = म_माप(found_base64);

      निवड(ssh_किल्ली_type(pubkey)) {
      स्थिती SSH_KEYTYPE_RSA:
        foundkey.keytype = CURLKHTYPE_RSA;
        थांबा;
      स्थिती SSH_KEYTYPE_RSA1:
        foundkey.keytype = CURLKHTYPE_RSA1;
        थांबा;
      स्थिती SSH_KEYTYPE_ECDSA:
      स्थिती SSH_KEYTYPE_ECDSA_P256:
      स्थिती SSH_KEYTYPE_ECDSA_P384:
      स्थिती SSH_KEYTYPE_ECDSA_P521:
        foundkey.keytype = CURLKHTYPE_ECDSA;
        थांबा;
      स्थिती SSH_KEYTYPE_ED25519:
        foundkey.keytype = CURLKHTYPE_ED25519;
        थांबा;
      स्थिती SSH_KEYTYPE_DSS:
        foundkey.keytype = CURLKHTYPE_DSS;
        थांबा;
      पूर्वनिर्धारित:
        rc = SSH_ERROR;
        जा cleanup;
      }

      CURL_CBAPI_START(&guard, data, easy_ssh_keyfunc);
      rc = func(data, knownkeyp, /* from the knownhosts file */
                &foundkey,       /* from the remote host */
                keymatch, data->set.ssh_keyfunc_userp);
      CURL_CBAPI_END(&guard);

      निवड(rc) {
      स्थिती CURLKHSTAT_FINE_ADD_TO_FILE:
        rc = ssh_session_update_known_hosts(sshc->ssh_session);
        जर(rc != SSH_OK) {
          जा cleanup;
        }
        थांबा;
      स्थिती CURLKHSTAT_FINE:
        थांबा;
      पूर्वनिर्धारित: /* REJECT/DEFER */
        rc = SSH_ERROR;
        जा cleanup;
      }
    }
    अन्यथा {
      जर(keymatch != CURLKHMATCH_OK) {
        rc = SSH_ERROR;
        जा cleanup;
      }
    }
  }
  rc = SSH_OK;

cleanup:
  जर(found_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    मुक्त(found_base64); /* allocated by libssh, deallocate with system free */
  }
  जर(known_base64) {
    /* !checksrc! disable BANNEDFUNC 1 */
    मुक्त(known_base64); /* allocated by libssh, deallocate with system free */
  }
  जर(hash_sha256)
    ssh_clean_pubkey_hash(&hash_sha256);
  जर(hash_md5)
    ssh_clean_pubkey_hash(&hash_md5);
  ssh_किल्ली_free(pubkey);
  जर(knownhostsentry) {
    ssh_knownhosts_entry_free(knownhostsentry);
  }
  परत rc;
}

स्थिर पूर्णांक myssh_to_ERROR(संरचना Curl_easy *data,
                          संरचना ssh_conn *sshc,
                          CURLcode result)
{
  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  sshc->actualcode = result;
  परत SSH_ERROR;
}

स्थिर पूर्णांक myssh_to_SFTP_CLOSE(संरचना Curl_easy *data,
                               संरचना ssh_conn *sshc)
{
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->actualcode = sftp_error_to_CURLE(sftp_get_error(sshc->sftp_session));
  परत SSH_ERROR;
}

स्थिर पूर्णांक myssh_to_PASSWD_AUTH(संरचना Curl_easy *data,
                                संरचना ssh_conn *sshc)
{
  जर(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
    परत SSH_OK;
  }
  परत myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

स्थिर पूर्णांक myssh_to_किल्ली_AUTH(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  जर(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_किल्ली_INIT);
    परत SSH_OK;
  }
  परत myssh_to_PASSWD_AUTH(data, sshc);
}

स्थिर पूर्णांक myssh_to_GSSAPI_AUTH(संरचना Curl_easy *data,
                                संरचना ssh_conn *sshc)
{
  जर(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
    परत SSH_OK;
  }
  परत myssh_to_किल्ली_AUTH(data, sshc);
}

स्थिर पूर्णांक myssh_in_SFTP_READDIR_INIT(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc,
                                      संरचना SSHPROTO *sshp)
{
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  Curl_pgrsSetDownloadSize(data, -1);
  जर(data->req.no_body) {
    myssh_to(data, sshc, SSH_STOP);
    परत SSH_NO_ERROR;
  }

  /*
   * This is a directory that we are trying to get, so produce a directory
   * listing
   */
  sshc->sftp_dir = sftp_opendir(sshc->sftp_session,
                                sshp->path);
  जर(!sshc->sftp_dir) {
    failf(data, "Could not open directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    परत myssh_to_SFTP_CLOSE(data, sshc);
  }
  myssh_to(data, sshc, SSH_SFTP_READDIR);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_READDIR(संरचना Curl_easy *data,
                                 संरचना ssh_conn *sshc,
                                 संरचना SSHPROTO *sshp)
{
  CURLcode result = CURLE_OK;

  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  curlx_dyn_reset(&sshc->readdir_buf);
  जर(sshc->readdir_attrs)
    sftp_attributes_free(sshc->readdir_attrs);

  sshc->readdir_attrs = sftp_readdir(sshc->sftp_session, sshc->sftp_dir);
  जर(sshc->readdir_attrs) {
    sshc->readdir_filename = sshc->readdir_attrs->name;
    sshc->readdir_longentry = sshc->readdir_attrs->longname;
    sshc->readdir_len = म_माप(sshc->readdir_filename);

    जर(data->set.list_only) {
      अक्षर *tmp_ओळ;

      tmp_ओळ = curl_maprintf("%s\n", sshc->readdir_filename);
      जर(!tmp_ओळ) {
        myssh_to(data, sshc, SSH_SFTP_CLOSE);
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        परत SSH_ERROR;
      }
      result = Curl_client_write(data, CLIENTWRITE_BODY,
                                 tmp_ओळ, sshc->readdir_len + 1);
      curlx_free(tmp_ओळ);

      जर(result) {
        myssh_to(data, sshc, SSH_STOP);
        sshc->actualcode = result;
        परत SSH_NO_ERROR;
      }
    }
    अन्यथा {
      जर(curlx_dyn_add(&sshc->readdir_buf, sshc->readdir_longentry)) {
        sshc->actualcode = CURLE_OUT_OF_MEMORY;
        myssh_to(data, sshc, SSH_STOP);
        परत SSH_ERROR;
      }

      जर((sshc->readdir_attrs->flags & SSH_FILEXFER_ATTR_PERMISSIONS) &&
         ((sshc->readdir_attrs->permissions & SSH_S_IFMT) ==
          SSH_S_IFLNK)) {
        sshc->readdir_linkPath = curl_maprintf("%s%s", sshp->path,
                                               sshc->readdir_filename);

        जर(!sshc->readdir_linkPath) {
          myssh_to(data, sshc, SSH_SFTP_CLOSE);
          sshc->actualcode = CURLE_OUT_OF_MEMORY;
          परत SSH_ERROR;
        }

        myssh_to(data, sshc, SSH_SFTP_READDIR_LINK);
        परत SSH_NO_ERROR;
      }
      myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
      परत SSH_NO_ERROR;
    }
  }
  अन्यथा जर(sftp_dir_eof(sshc->sftp_dir)) {
    myssh_to(data, sshc, SSH_SFTP_READDIR_DONE);
  }
  अन्यथा {
    failf(data, "Could not open remote directory for reading: %s",
          ssh_get_error(sshc->ssh_session));
    परत myssh_to_SFTP_CLOSE(data, sshc);
  }
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_READDIR_LINK(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  जर(sshc->readdir_link_attrs)
    sftp_attributes_free(sshc->readdir_link_attrs);

  sshc->readdir_link_attrs = sftp_lstat(sshc->sftp_session,
                                        sshc->readdir_linkPath);
  जर(!sshc->readdir_link_attrs) {
    failf(data, "Could not read symlink for reading: %s",
          ssh_get_error(sshc->ssh_session));
    परत myssh_to_SFTP_CLOSE(data, sshc);
  }

  जर(!sshc->readdir_link_attrs->name) {
    sshc->readdir_tmp = sftp_readlink(sshc->sftp_session,
                                      sshc->readdir_linkPath);
    जर(!sshc->readdir_tmp)
      sshc->readdir_len = 0;
    अन्यथा
      sshc->readdir_len = म_माप(sshc->readdir_tmp);
    sshc->readdir_longentry = रिक्त;
    sshc->readdir_filename = sshc->readdir_tmp;
  }
  अन्यथा {
    sshc->readdir_len = म_माप(sshc->readdir_link_attrs->name);
    sshc->readdir_filename = sshc->readdir_link_attrs->name;
    sshc->readdir_longentry = sshc->readdir_link_attrs->longname;
  }

  curlx_safefree(sshc->readdir_linkPath);

  जर(curlx_dyn_addf(&sshc->readdir_buf, " -> %s", sshc->readdir_filename)) {
    /* Not using:
     * return myssh_to_SFTP_CLOSE(data, sshc);
     *
     * as that assumes an sftp related error while
     * assigning sshc->actualcode whereas the current
     * error is curlx_dyn_addf() related.
     */
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->actualcode = CURLE_OUT_OF_MEMORY;
    परत SSH_ERROR;
  }

  sftp_attributes_free(sshc->readdir_link_attrs);
  sshc->readdir_link_attrs = रिक्त;
  sshc->readdir_filename = रिक्त;
  sshc->readdir_longentry = रिक्त;

  myssh_to(data, sshc, SSH_SFTP_READDIR_BOTTOM);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_READDIR_BOTTOM(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc)
{
  CURLcode result;

  जर(curlx_dyn_addn(&sshc->readdir_buf, "\n", 1))
    result = CURLE_OUT_OF_MEMORY;
  अन्यथा
    result = Curl_client_write(data, CLIENTWRITE_BODY,
                               curlx_dyn_ptr(&sshc->readdir_buf),
                               curlx_dyn_len(&sshc->readdir_buf));

  ssh_string_free_char(sshc->readdir_tmp);
  sshc->readdir_tmp = रिक्त;

  जर(result) {
    myssh_to(data, sshc, SSH_STOP);
    sshc->actualcode = result;
  }
  अन्यथा
    myssh_to(data, sshc, SSH_SFTP_READDIR);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_READDIR_DONE(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  sftp_closedir(sshc->sftp_dir);
  sshc->sftp_dir = रिक्त;

  /* no data to transfer */
  Curl_xfer_setup_nop(data);
  myssh_to(data, sshc, SSH_STOP);
  परत SSH_NO_ERROR;
}

स्थिर शून्य myssh_quote_error(संरचना Curl_easy *data, संरचना ssh_conn *sshc,
                              स्थिरांक अक्षर *cmd)
{
  जर(cmd)
    failf(data, "%s command failed: %s", cmd,
          ssh_get_error(sshc->ssh_session));
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE_STATVFS(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  sftp_statvfs_t statvfs;

  statvfs = sftp_statvfs(sshc->sftp_session, sshc->quote_path1);
  जर(!statvfs && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "statvfs");
    परत SSH_OK;
  }
  अन्यथा जर(statvfs) {
#ifdef _MSC_VER
#define CURL_LIBSSH_VFS_SIZE_MASK "I64u"
#किंवाजर defined(__MINGW32__) && (__MINGW64_VERSION_MAJOR <= 6)
#define CURL_LIBSSH_VFS_SIZE_MASK "llu"
#अन्यथा
#define CURL_LIBSSH_VFS_SIZE_MASK PRIu64
#endif
    CURLcode result = CURLE_OK;
    अक्षर *tmp = curl_maprintf("statvfs:\n"
                              "f_bsize: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_frsize: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_blocks: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_bfree: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_bavail: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_files: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_ffree: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_favail: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_fsid: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_flag: %" CURL_LIBSSH_VFS_SIZE_MASK "\n"
                              "f_namemax: %" CURL_LIBSSH_VFS_SIZE_MASK "\n",
                              statvfs->f_bsize, statvfs->f_frsize,
                              statvfs->f_blocks, statvfs->f_bfree,
                              statvfs->f_bavail, statvfs->f_files,
                              statvfs->f_ffree, statvfs->f_favail,
                              statvfs->f_fsid, statvfs->f_flag,
                              statvfs->f_namemax);
    sftp_statvfs_free(statvfs);

    जर(!tmp)
      result = CURLE_OUT_OF_MEMORY;

    जर(!result) {
      result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, म_माप(tmp));
      curlx_free(tmp);
    }
    जर(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  परत SSH_OK;
}

स्थिर पूर्णांक myssh_auth_interactive(संरचना connectdata *conn,
                                  संरचना ssh_conn *sshc)
{
  पूर्णांक rc;
  पूर्णांक nprompts;

restart:
  निवड(sshc->kbd_state) {
  स्थिती 0:
    rc = ssh_userauth_kbdint(sshc->ssh_session, रिक्त, रिक्त);
    जर(rc == SSH_AUTH_AGAIN)
      परत SSH_AGAIN;

    जर(rc != SSH_AUTH_INFO)
      परत SSH_ERROR;

    nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
    जर(nprompts != 1)
      परत SSH_ERROR;

    rc = ssh_userauth_kbdint_setanswer(sshc->ssh_session, 0,
                                       Curl_creds_passwd(conn->creds));
    जर(rc < 0)
      परत SSH_ERROR;

    FALLTHROUGH();
  स्थिती 1:
    sshc->kbd_state = 1;

    rc = ssh_userauth_kbdint(sshc->ssh_session, रिक्त, रिक्त);
    जर(rc == SSH_AUTH_AGAIN)
      परत SSH_AGAIN;
    अन्यथा जर(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    अन्यथा जर(rc == SSH_AUTH_INFO) {
      nprompts = ssh_userauth_kbdint_getnprompts(sshc->ssh_session);
      जर(nprompts)
        परत SSH_ERROR;

      sshc->kbd_state = 2;
      जा restart;
    }
    अन्यथा
      rc = SSH_ERROR;
    थांबा;
  स्थिती 2:
    sshc->kbd_state = 2;

    rc = ssh_userauth_kbdint(sshc->ssh_session, रिक्त, रिक्त);
    जर(rc == SSH_AUTH_AGAIN)
      परत SSH_AGAIN;
    अन्यथा जर(rc == SSH_AUTH_SUCCESS)
      rc = SSH_OK;
    अन्यथा
      rc = SSH_ERROR;

    थांबा;
  पूर्वनिर्धारित:
    परत SSH_ERROR;
  }

  sshc->kbd_state = 0;
  परत rc;
}

स्थिर शून्य myssh_state_init(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  sshc->secondCreateDirs = 0;
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_OK;

#जर 0
  ssh_set_log_level(SSH_LOG_PROTOCOL);
#endif

  /* Set libssh to non-blocking, since everything internally is
     non-blocking */
  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_S_STARTUP);
}

स्थिर शून्य myssh_block2waitfor(संरचना connectdata *conn,
                                संरचना ssh_conn *sshc,
                                bool block)
{
  (शून्य)conn;
  जर(block) {
    पूर्णांक dir = ssh_get_poll_flags(sshc->ssh_session);
    /* translate the libssh define bits into our own bit defines */
    sshc->waitfor =
      ((dir & SSH_READ_PENDING) ? REQ_IO_RECV : 0) |
      ((dir & SSH_WRITE_PENDING) ? REQ_IO_SEND : 0);
  }
  अन्यथा
    sshc->waitfor = 0;
}

स्थिर पूर्णांक myssh_in_S_STARTUP(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  पूर्णांक rc = ssh_connect(sshc->ssh_session);

  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));
  जर(rc == SSH_AGAIN) {
    CURL_TRC_SSH(data, "connect -> EAGAIN");
  }
  अन्यथा जर(rc != SSH_OK) {
    failf(data, "Failure establishing ssh session");
    rc = myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  }
  अन्यथा
    myssh_to(data, sshc, SSH_HOSTKEY);

  परत rc;
}

स्थिर पूर्णांक myssh_in_AUTHLIST(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  पूर्णांक rc;
  sshc->authed = FALSE;

  rc = ssh_userauth_none(sshc->ssh_session, रिक्त);
  जर(rc == SSH_AUTH_AGAIN)
    परत SSH_AGAIN;

  जर(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Authenticated with none");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    परत rc;
  }
  अन्यथा जर(rc == SSH_AUTH_ERROR) {
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
    परत rc;
  }

  sshc->auth_methods =
    (अचिन्हित पूर्णांक)ssh_userauth_list(sshc->ssh_session, रिक्त);
  जर(sshc->auth_methods)
    infof(data, "SSH authentication methods available: %s%s%s%s",
          sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY ?
          "public key, " : "",
          sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC ?
          "GSSAPI, " : "",
          sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE ?
          "keyboard-interactive, " : "",
          sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD ?
          "password" : "");
  /* For public key auth we need either the private key or
     CURLSSH_AUTH_AGENT. */
  जर((sshc->auth_methods & SSH_AUTH_METHOD_PUBLICKEY) &&
     ((sshc->priv_किल्ली && sshc->priv_किल्ली[0]) ||
      (data->set.ssh_auth_types & CURLSSH_AUTH_AGENT))) {
    myssh_to(data, sshc, SSH_AUTH_PKEY_INIT);
    infof(data, "Authentication using SSH public key file");
  }
  अन्यथा जर(sshc->auth_methods & SSH_AUTH_METHOD_GSSAPI_MIC) {
    myssh_to(data, sshc, SSH_AUTH_GSSAPI);
  }
  अन्यथा जर(sshc->auth_methods & SSH_AUTH_METHOD_INTERACTIVE) {
    myssh_to(data, sshc, SSH_AUTH_किल्ली_INIT);
  }
  अन्यथा जर(sshc->auth_methods & SSH_AUTH_METHOD_PASSWORD) {
    myssh_to(data, sshc, SSH_AUTH_PASS_INIT);
  }
  अन्यथा {                  /* unsupported authentication method */
    rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }
  परत rc;
}

स्थिर पूर्णांक myssh_in_AUTH_PKEY_INIT(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc)
{
  पूर्णांक rc;
  जर(!(data->set.ssh_auth_types & CURLSSH_AUTH_PUBLICKEY)) {
    rc = myssh_to_GSSAPI_AUTH(data, sshc);
    परत rc;
  }

  /* Two choices, (1) private key was given on CMD,
   * (2) use the "default" keys. */
  जर(sshc->priv_किल्ली && sshc->priv_किल्ली[0]) {
    जर(sshc->pubkey && !sshc->passphrase) {
      rc = ssh_userauth_try_publickey(sshc->ssh_session, रिक्त, sshc->pubkey);
      जर(rc == SSH_AUTH_AGAIN)
        परत SSH_AGAIN;

      जर(rc != SSH_OK) {
        rc = myssh_to_GSSAPI_AUTH(data, sshc);
        परत rc;
      }
    }

    rc = ssh_pki_import_privkey_file(sshc->priv_किल्ली,
                                     sshc->passphrase, रिक्त,
                                     रिक्त, &sshc->privkey);
    जर(rc != SSH_OK) {
      failf(data, "Could not load private key file %s", sshc->priv_किल्ली);
      rc = myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
      परत rc;
    }

    myssh_to(data, sshc, SSH_AUTH_PKEY);
  }
  अन्यथा {
    rc = ssh_userauth_publickey_auto(sshc->ssh_session, रिक्त,
                                     sshc->passphrase);
    जर(rc == SSH_AUTH_AGAIN)
      परत SSH_AGAIN;

    जर(rc == SSH_AUTH_SUCCESS) {
      rc = SSH_OK;
      sshc->authed = TRUE;
      infof(data, "Completed public key authentication");
      myssh_to(data, sshc, SSH_AUTH_DONE);
      परत rc;
    }

    rc = myssh_to_GSSAPI_AUTH(data, sshc);
  }
  परत rc;
}

स्थिर पूर्णांक myssh_in_AUTH_PKEY(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  पूर्णांक rc = ssh_userauth_publickey(sshc->ssh_session, रिक्त, sshc->privkey);
  जर(rc == SSH_AUTH_AGAIN)
    परत SSH_AGAIN;
  अन्यथा जर(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed public key authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    परत SSH_OK;
  }
  अन्यथा {
    infof(data, "Failed public key authentication (rc: %d)", rc);
    परत myssh_to_GSSAPI_AUTH(data, sshc);
  }
}

स्थिर पूर्णांक myssh_in_AUTH_GSSAPI(संरचना Curl_easy *data,
                                संरचना ssh_conn *sshc)
{
  पूर्णांक rc;
  जर(!(data->set.ssh_auth_types & CURLSSH_AUTH_GSSAPI))
    परत myssh_to_किल्ली_AUTH(data, sshc);

  rc = ssh_userauth_gssapi(sshc->ssh_session);
  जर(rc == SSH_AUTH_AGAIN)
    परत SSH_AGAIN;

  जर(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed gssapi authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    परत SSH_OK;
  }

  परत myssh_to_किल्ली_AUTH(data, sshc);
}

स्थिर पूर्णांक myssh_in_AUTH_किल्ली_INIT(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  जर(data->set.ssh_auth_types & CURLSSH_AUTH_KEYBOARD) {
    myssh_to(data, sshc, SSH_AUTH_किल्ली);
    परत SSH_NO_ERROR;
  }
  परत myssh_to_PASSWD_AUTH(data, sshc);
}

स्थिर पूर्णांक myssh_in_AUTH_किल्ली(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  /* keyboard-interactive authentication */
  पूर्णांक rc = myssh_auth_interactive(data->conn, sshc);
  जर(rc == SSH_AGAIN)
    परत rc;
  अन्यथा जर(rc == SSH_OK) {
    sshc->authed = TRUE;
    infof(data, "completed keyboard interactive authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    परत SSH_NO_ERROR;
  }
  अन्यथा
    परत myssh_to_PASSWD_AUTH(data, sshc);
}

स्थिर पूर्णांक myssh_in_AUTH_PASS_INIT(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc)
{
  जर(!(data->set.ssh_auth_types & CURLSSH_AUTH_PASSWORD))
    परत myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  myssh_to(data, sshc, SSH_AUTH_PASS);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_AUTH_PASS(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  पूर्णांक rc = ssh_userauth_password(sshc->ssh_session, रिक्त,
                                 Curl_creds_passwd(data->conn->creds));
  जर(rc == SSH_AUTH_AGAIN)
    परत SSH_AGAIN;
  अन्यथा जर(rc == SSH_AUTH_SUCCESS) {
    sshc->authed = TRUE;
    infof(data, "Completed password authentication");
    myssh_to(data, sshc, SSH_AUTH_DONE);
    परत SSH_NO_ERROR;
  }
  परत myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
}

स्थिर पूर्णांक myssh_in_AUTH_DONE(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  संरचना connectdata *conn = data->conn;
  जर(!sshc->authed) {
    failf(data, "Authentication failure");
    परत myssh_to_ERROR(data, sshc, CURLE_LOGIN_DENIED);
  }

  /* At this point we have an authenticated ssh session. */
  infof(data, "Authentication complete");
  Curl_pgrsTime(data, TIMER_APPCONNECT);      /* SSH is connected */
  conn->recv_idx = FIRSTSOCKET;
  conn->send_idx = -1;

  जर(conn->scheme->protocol == CURLPROTO_SFTP) {
    myssh_to(data, sshc, SSH_SFTP_INIT);
    परत SSH_NO_ERROR;
  }
  infof(data, "SSH CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_UPLOAD_INIT(संरचना Curl_easy *data,
                                संरचना ssh_conn *sshc,
                                संरचना SSHPROTO *sshp)
{
  पूर्णांक flags;
  पूर्णांक rc = 0;

  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  जर(data->state.resume_from) {
    sftp_attributes attrs;

    जर(data->state.resume_from < 0) {
      attrs = sftp_stat(sshc->sftp_session, sshp->path);
      जर(attrs) {
        curl_off_t size = attrs->size;
        sftp_attributes_free(attrs);
        जर(size < 0) {
          failf(data, "Bad file size (%" FMT_OFF_T ")", size);
          rc = myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
          परत rc;
        }
        data->state.resume_from = size;
      }
      अन्यथा {
        data->state.resume_from = 0;
      }
    }
  }

  जर(data->set.remote_append) {
    /* True append mode: create if nonexisting */
    flags = O_WRONLY | O_CREAT | O_APPEND;
  }
  अन्यथा जर(data->state.resume_from > 0) {
    /*
     * Resume MUST NOT use O_APPEND. Many SFTP servers/impls force all
     * writes to EOF when O_APPEND is set, ignoring a prior seek().
     * Open write-only and seek to the resume offset instead.
     */
    flags = O_WRONLY;
  }
  अन्यथा
    /* Clear file before writing (normal behavior) */
    flags = O_WRONLY | O_CREAT | O_TRUNC;

  जर(sshc->sftp_file)
    sftp_close(sshc->sftp_file);
  sshc->sftp_file =
    sftp_open(sshc->sftp_session, sshp->path,
              flags, (mode_t)data->set.new_file_perms);
  जर(!sshc->sftp_file) {
    पूर्णांक err = sftp_get_error(sshc->sftp_session);

    जर((err == SSH_FX_NO_SUCH_FILE ||
        err == SSH_FX_FAILURE ||
        err == SSH_FX_NO_SUCH_PATH) &&
       data->set.ftp_create_missing_dirs &&
       (म_माप(sshp->path) > 1)) {
      /* try to create the path remotely */
      rc = 0;
      sshc->secondCreateDirs = 1;
      myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_INIT);
      परत rc;
    }
    अन्यथा {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      परत rc;
    }
  }

  /* If we have a restart point then we need to seek to the correct
     position. Skip if in explicit remote append mode. */
  जर(data->state.resume_from > 0 && !data->set.remote_append) {
    पूर्णांक seekerr = CURL_SEEKFUNC_OK;
    /* Let's read off the proper amount of bytes from the input. */
    जर(data->set.seek_func) {
      संरचना Curl_mapi_guard guard;
      CURL_CBAPI_START(&guard, data, easy_seek_func);
      seekerr = data->set.seek_func(data->set.seek_client,
                                    data->state.resume_from, शुरू_से);
      CURL_CBAPI_END(&guard);
    }

    जर(seekerr != CURL_SEEKFUNC_OK) {
      curl_off_t passed = 0;

      जर(seekerr != CURL_SEEKFUNC_CANTSEEK) {
        failf(data, "Could not seek stream");
        rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
        परत rc;
      }
      /* seekerr == CURL_SEEKFUNC_CANTSEEK (cannot seek to offset) */
      करा {
        अक्षर scratch[4 * 1024];
        माप_प्रकार readthisamountnow =
          (data->state.resume_from - passed >
           (curl_off_t)आकार(scratch)) ?
          आकार(scratch) : curlx_sotouz(data->state.resume_from - passed);

        माप_प्रकार actuallyread =
          data->state.fread_func(scratch, 1,
                                 readthisamountnow, data->state.in);

        passed += actuallyread;
        जर((actuallyread == 0) || (actuallyread > readthisamountnow)) {
          /* this checks for greater-than only to make sure that the
             CURL_READFUNC_ABORT return code still aborts */
          failf(data, "Failed to read data");
          rc = myssh_to_ERROR(data, sshc, CURLE_FTP_COULDNT_USE_REST);
          परत rc;
        }
      } पर्यंत(passed < data->state.resume_from);
    }

    /* now, decrease the size of the read */
    जर(data->state.infilesize > 0) {
      जर(data->state.resume_from > data->state.infilesize) {
        failf(data, "Resume point beyond size");
        परत myssh_to_ERROR(data, sshc, CURLE_BAD_FUNCTION_ARGUMENT);
      }
      data->state.infilesize -= data->state.resume_from;
      data->req.size = data->state.infilesize;
      Curl_pgrsSetUploadSize(data, data->state.infilesize);
    }

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    जर(rc) {
      rc = myssh_to_SFTP_CLOSE(data, sshc);
      परत rc;
    }
  }
  जर(data->state.infilesize > 0) {
    data->req.size = data->state.infilesize;
    Curl_pgrsSetUploadSize(data, data->state.infilesize);
  }
  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  /* since we do not really wait for anything at this point, we want the
     state machine to move on as soon as possible so we mark this as dirty */
  Curl_multi_mark_dirty(data);
#जर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sshc->sftp_send_state = 0;
#endif
  myssh_to(data, sshc, SSH_STOP);
  परत rc;
}

स्थिर पूर्णांक myssh_in_SFTP_DOWNLOAD_INIT(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc,
                                       संरचना SSHPROTO *sshp)
{
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  /* Work on getting the specified file */
  जर(sshc->sftp_file)
    sftp_close(sshc->sftp_file);

  sshc->sftp_file = sftp_open(sshc->sftp_session, sshp->path,
                              O_RDONLY, (mode_t)data->set.new_file_perms);
  जर(!sshc->sftp_file) {
    failf(data, "Could not open remote file for reading: %s",
          ssh_get_error(sshc->ssh_session));

    परत myssh_to_SFTP_CLOSE(data, sshc);
  }
  sftp_file_set_nonblocking(sshc->sftp_file);
  myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_STAT);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_DOWNLOAD_STAT(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  curl_off_t size;
  पूर्णांक rc = 0;
  sftp_attributes attrs = sftp_fstat(sshc->sftp_file);
  जर(!attrs ||
     !(attrs->flags & SSH_FILEXFER_ATTR_SIZE) ||
     (attrs->size == 0)) {
    /*
     * sftp_fstat did not return an error, so maybe the server
     * does not support stat()
     * OR the server does not return a file size with a stat()
     * OR file size is 0
     */
    data->req.size = -1;
    data->req.maxdownload = -1;
    Curl_pgrsSetDownloadSize(data, -1);
    size = 0;
    जर(attrs)
      sftp_attributes_free(attrs);
  }
  अन्यथा {
    size = attrs->size;

    sftp_attributes_free(attrs);

    जर(size < 0) {
      failf(data, "Bad file size (%" FMT_OFF_T ")", size);
      परत myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
    }
    जर(data->state.use_range) {
      curl_off_t from;
      CURLcode result = Curl_ssh_range(data, data->state.range, size,
                                       &from, &size);
      जर(result)
        परत myssh_to_ERROR(data, sshc, result);

      rc = sftp_seek64(sshc->sftp_file, from);
      जर(rc)
        परत myssh_to_SFTP_CLOSE(data, sshc);
    }
    data->req.size = size;
    data->req.maxdownload = size;
    Curl_pgrsSetDownloadSize(data, size);
  }

  /* We can resume if we can seek to the resume position */
  जर(data->state.resume_from) {
    जर(data->state.resume_from < 0) {
      /* We are supposed to download the last abs(from) bytes */
      जर(size < -data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T ") was beyond file size (%"
              FMT_OFF_T ")", data->state.resume_from, size);
        परत myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
      /* download from where? */
      data->state.resume_from += size;
    }
    अन्यथा {
      जर(size < data->state.resume_from) {
        failf(data, "Offset (%" FMT_OFF_T
              ") was beyond file size (%" FMT_OFF_T ")",
              data->state.resume_from, size);
        परत myssh_to_ERROR(data, sshc, CURLE_BAD_DOWNLOAD_RESUME);
      }
    }
    /* Now store the number of bytes we are expected to download */
    data->req.size = size - data->state.resume_from;
    data->req.maxdownload = size - data->state.resume_from;
    Curl_pgrsSetDownloadSize(data, size - data->state.resume_from);

    rc = sftp_seek64(sshc->sftp_file, data->state.resume_from);
    जर(rc)
      परत myssh_to_SFTP_CLOSE(data, sshc);
  }

  /* Setup the actual download */
  जर(data->req.size == 0) {
    /* no data to transfer */
    Curl_xfer_setup_nop(data);
    infof(data, "File already completely downloaded");
    myssh_to(data, sshc, SSH_STOP);
    परत rc;
  }
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.size);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  sshc->sftp_recv_state = 0;
  myssh_to(data, sshc, SSH_STOP);

  परत rc;
}

स्थिर पूर्णांक myssh_in_SFTP_CLOSE(संरचना Curl_easy *data,
                               संरचना ssh_conn *sshc,
                               संरचना SSHPROTO *sshp)
{
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  जर(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = रिक्त;
  }
  curlx_safefree(sshp->path);

  CURL_TRC_SSH(data, "SFTP DONE done");

  /* Check if nextstate is set and move .nextstate could be POSTQUOTE_INIT
     After nextstate is executed, the control should come back to
     SSH_SFTP_CLOSE to pass the correct result back */
  जर(sshc->nextstate != SSH_NO_STATE &&
     sshc->nextstate != SSH_SFTP_CLOSE) {
    myssh_to(data, sshc, sshc->nextstate);
    sshc->nextstate = SSH_SFTP_CLOSE;
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_STOP);
  }
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_SHUTDOWN(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  /* during times we get here due to a broken transfer and then the
     sftp_handle might not have been taken down so make sure that is done
     before we proceed */
  ssh_set_blocking(sshc->ssh_session, 0);
#जर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  SFTP_AIO_FREE(sshc->sftp_send_aio);
  SFTP_AIO_FREE(sshc->sftp_recv_aio);
#endif

  जर(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = रिक्त;
  }

  जर(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = रिक्त;
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_INIT(संरचना Curl_easy *data,
                              संरचना ssh_conn *sshc)
{
  पूर्णांक rc;
  ssh_set_blocking(sshc->ssh_session, 1);

  sshc->sftp_session = sftp_new(sshc->ssh_session);
  जर(!sshc->sftp_session) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    परत myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);
  }

  rc = sftp_init(sshc->sftp_session);
  जर(rc != SSH_OK) {
    failf(data, "Failure initializing sftp session: %s",
          ssh_get_error(sshc->ssh_session));
    परत myssh_to_ERROR(data, sshc, sftp_error_to_CURLE(SSH_FX_FAILURE));
  }
  myssh_to(data, sshc, SSH_SFTP_REALPATH);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_REALPATH(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  /* Get the "home" directory */
  sshc->homedir = sftp_canonicalize_path(sshc->sftp_session, ".");
  जर(!sshc->homedir)
    परत myssh_to_ERROR(data, sshc, CURLE_COULDNT_CONNECT);

  curlx_free(data->state.most_recent_ftp_entrypath);
  data->state.most_recent_ftp_entrypath = curlx_strdup(sshc->homedir);
  जर(!data->state.most_recent_ftp_entrypath)
    परत myssh_to_ERROR(data, sshc, CURLE_OUT_OF_MEMORY);

  /* This is the last step in the SFTP connect phase. Do note that while
     we get the homedir here, we get the "workingpath" in the DO action
     since the homedir remains the same between request but the
     working path does not. */
  CURL_TRC_SSH(data, "CONNECT phase done");
  myssh_to(data, sshc, SSH_STOP);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE_INIT(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp)
{
  CURLcode result;

  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  जर(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
  }
  अन्यथा जर(data->set.quote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.quote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा
    myssh_to(data, sshc, SSH_SFTP_GETINFO);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_POSTQUOTE_INIT(संरचना Curl_easy *data,
                                        संरचना ssh_conn *sshc)
{
  जर(data->set.postquote) {
    infof(data, "Sending quote commands");
    sshc->quote_item = data->set.postquote;
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_STOP);
  }
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक quote_error(संरचना Curl_easy *data,
                       संरचना ssh_conn *sshc)
{
  failf(data, "Suspicious data after the command line");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE(संरचना Curl_easy *data,
                               संरचना ssh_conn *sshc,
                               संरचना SSHPROTO *sshp)
{
  स्थिरांक अक्षर *cp;
  CURLcode result;

  /*
   * Support some of the "FTP" commands
   */
  स्थिरांक अक्षर *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  जर(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  जर(curl_strequal("pwd", cmd)) {
    /* output debug output if that is requested */
    अक्षर *tmp = curl_maprintf("257 \"%s\" is current directory.\n",
                              sshp->path);
    जर(!tmp) {
      sshc->actualcode = CURLE_OUT_OF_MEMORY;
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      परत SSH_NO_ERROR;
    }
    Curl_debug(data, CURLINFO_HEADER_OUT, "PWD\n", 4);
    Curl_debug(data, CURLINFO_HEADER_IN, tmp, म_माप(tmp));

    /* this sends an FTP-like "header" to the header callback so that
       the current directory can be read similar to how it is read when
       using ordinary FTP. */
    result = Curl_client_write(data, CLIENTWRITE_HEADER, tmp, म_माप(tmp));
    curlx_free(tmp);
    जर(result) {
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
    }
    अन्यथा
      myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
    परत SSH_NO_ERROR;
  }

  /*
   * the arguments following the command must be separated from the
   * command with a space so we can check for it unconditionally
   */
  cp = म_अक्षर(cmd, ' ');
  जर(!cp) {
    failf(data, "Syntax error in SFTP command. Supply parameter(s)");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = CURLE_QUOTE_ERROR;
    परत SSH_NO_ERROR;
  }

  /*
   * also, every command takes at least one argument so we get that
   * first argument right now
   */
  result = Curl_get_pathname(&cp, &sshc->quote_path1, sshc->homedir);
  जर(result) {
    जर(result == CURLE_OUT_OF_MEMORY)
      failf(data, "Out of memory");
    अन्यथा
      failf(data, "Syntax error: Bad first parameter");
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
    sshc->nextstate = SSH_NO_STATE;
    sshc->actualcode = result;
    परत SSH_NO_ERROR;
  }

  /*
   * SFTP is a binary protocol, so we do not send text commands
   * to the server. Instead, we scan for commands used by
   * OpenSSH's sftp program and call the appropriate libssh
   * functions.
   */
  जर(!म_भेदन(cmd, "chgrp ", 6) ||
     !म_भेदन(cmd, "chmod ", 6) ||
     !म_भेदन(cmd, "chown ", 6) ||
     !म_भेदन(cmd, "atime ", 6) ||
     !म_भेदन(cmd, "mtime ", 6)) {
    /* attribute change */

    /* sshc->quote_path1 contains the mode to set */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    जर(result) {
      जर(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      अन्यथा
        failf(data, "Syntax error in chgrp/chmod/chown/atime/mtime: "
              "Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      परत SSH_NO_ERROR;
    }
    जर(*cp)
      परत quote_error(data, sshc);
    sshc->quote_attrs = रिक्त;
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STAT);
    परत SSH_NO_ERROR;
  }
  जर(!म_भेदन(cmd, "ln ", 3) ||
     !म_भेदन(cmd, "symlink ", 8)) {
    /* symbolic linking */
    /* sshc->quote_path1 is the source */
    /* get the destination */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    जर(result) {
      जर(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      अन्यथा
        failf(data, "Syntax error in ln/symlink: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      परत SSH_NO_ERROR;
    }
    जर(*cp)
      परत quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_SYMLINK);
    परत SSH_NO_ERROR;
  }
  अन्यथा जर(!म_भेदन(cmd, "mkdir ", 6)) {
    जर(*cp)
      परत quote_error(data, sshc);
    /* create directory */
    myssh_to(data, sshc, SSH_SFTP_QUOTE_MKDIR);
    परत SSH_NO_ERROR;
  }
  अन्यथा जर(!म_भेदन(cmd, "rename ", 7)) {
    /* rename file */
    /* first param is the source path */
    /* second param is the dest. path */
    result = Curl_get_pathname(&cp, &sshc->quote_path2, sshc->homedir);
    जर(result) {
      जर(result == CURLE_OUT_OF_MEMORY)
        failf(data, "Out of memory");
      अन्यथा
        failf(data, "Syntax error in rename: Bad second parameter");
      curlx_safefree(sshc->quote_path1);
      myssh_to(data, sshc, SSH_SFTP_CLOSE);
      sshc->nextstate = SSH_NO_STATE;
      sshc->actualcode = result;
      परत SSH_NO_ERROR;
    }
    जर(*cp)
      परत quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RENAME);
    परत SSH_NO_ERROR;
  }
  अन्यथा जर(!म_भेदन(cmd, "rmdir ", 6)) {
    /* delete directory */
    जर(*cp)
      परत quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_RMDIR);
    परत SSH_NO_ERROR;
  }
  अन्यथा जर(!म_भेदन(cmd, "rm ", 3)) {
    जर(*cp)
      परत quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_UNLINK);
    परत SSH_NO_ERROR;
  }
#ifdef HAS_STATVFS_SUPPORT
  अन्यथा जर(!म_भेदन(cmd, "statvfs ", 8)) {
    जर(*cp)
      परत quote_error(data, sshc);
    myssh_to(data, sshc, SSH_SFTP_QUOTE_STATVFS);
    परत SSH_NO_ERROR;
  }
#endif

  failf(data, "Unknown SFTP command");
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);
  myssh_to(data, sshc, SSH_SFTP_CLOSE);
  sshc->nextstate = SSH_NO_STATE;
  sshc->actualcode = CURLE_QUOTE_ERROR;
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_NEXT_QUOTE(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  curlx_safefree(sshc->quote_path1);
  curlx_safefree(sshc->quote_path2);

  sshc->quote_item = sshc->quote_item->next;

  जर(sshc->quote_item) {
    myssh_to(data, sshc, SSH_SFTP_QUOTE);
  }
  अन्यथा {
    जर(sshc->nextstate != SSH_NO_STATE) {
      myssh_to(data, sshc, sshc->nextstate);
      sshc->nextstate = SSH_NO_STATE;
    }
    अन्यथा {
      myssh_to(data, sshc, SSH_SFTP_GETINFO);
    }
  }
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE_STAT(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc)
{
  स्थिरांक अक्षर *cmd = sshc->quote_item->data;
  sshc->acceptfail = FALSE;

  /* if a command starts with an asterisk, which a legal SFTP command never
     can, the command is allowed to fail without it causing any
     aborts or cancels etc. It causes libcurl to act as if the command
     is successful, whatever the server responds. */

  जर(cmd[0] == '*') {
    cmd++;
    sshc->acceptfail = TRUE;
  }

  /* We read the file attributes, store them in sshc->quote_attrs
   * and modify them accordingly to command. Then we switch to
   * QUOTE_SETSTAT state to write new ones.
   */

  जर(sshc->quote_attrs)
    sftp_attributes_free(sshc->quote_attrs);
  sshc->quote_attrs = sftp_stat(sshc->sftp_session, sshc->quote_path2);
  जर(!sshc->quote_attrs) {
    failf(data, "Attempt to get SFTP stats failed: %d",
          sftp_get_error(sshc->sftp_session));
    myssh_quote_error(data, sshc, रिक्त);
    परत SSH_NO_ERROR;
  }

  /* Now set the new attributes... */
  जर(!म_भेदन(cmd, "chgrp", 5)) {
    स्थिरांक अक्षर *p = sshc->quote_path1;
    curl_off_t gid;
    जर(curlx_str_संख्या(&p, &gid, अच_पूर्णांक_उच्च)) {
      failf(data, "Syntax error: chgrp gid not a number");
      myssh_quote_error(data, sshc, रिक्त);
      परत SSH_NO_ERROR;
    }
    sshc->quote_attrs->gid = (uint32_t)gid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  अन्यथा जर(!म_भेदन(cmd, "chmod", 5)) {
    curl_off_t perms;
    स्थिरांक अक्षर *p = sshc->quote_path1;
    जर(curlx_str_octal(&p, &perms, 07777)) {
      failf(data, "Syntax error: chmod permissions not a number");
      myssh_quote_error(data, sshc, रिक्त);
      परत SSH_NO_ERROR;
    }
    sshc->quote_attrs->permissions = (mode_t)perms;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_PERMISSIONS;
  }
  अन्यथा जर(!म_भेदन(cmd, "chown", 5)) {
    स्थिरांक अक्षर *p = sshc->quote_path1;
    curl_off_t uid;
    जर(curlx_str_संख्या(&p, &uid, अच_पूर्णांक_उच्च)) {
      failf(data, "Syntax error: chown uid not a number");
      myssh_quote_error(data, sshc, रिक्त);
      परत SSH_NO_ERROR;
    }
    sshc->quote_attrs->uid = (uint32_t)uid;
    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_UIDGID;
  }
  अन्यथा जर(!म_भेदन(cmd, "atime", 5) ||
          !म_भेदन(cmd, "mtime", 5)) {
    समय_प्रकार date;
    bool fail = FALSE;
    जर(Curl_getdate_capped(sshc->quote_path1, &date)) {
      failf(data, "incorrect date format for %.*s", 5, cmd);
      fail = TRUE;
    }
#जर SIZEOF_TIME_T > 4
    अन्यथा जर(date > 0xffffffff) {
      failf(data, "date overflow");
      fail = TRUE; /* avoid setting a capped time */
    }
#endif
    जर(fail) {
      myssh_quote_error(data, sshc, रिक्त);
      परत SSH_NO_ERROR;
    }
    जर(!म_भेदन(cmd, "atime", 5))
      sshc->quote_attrs->atime = (uint32_t)date;
    अन्यथा /* mtime */
      sshc->quote_attrs->mtime = (uint32_t)date;

    sshc->quote_attrs->flags |= SSH_FILEXFER_ATTR_ACMODTIME;
  }

  /* Now send the completed structure... */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_SETSTAT);
  परत SSH_NO_ERROR;
}

स्थिर शून्य conn_forget_socket(संरचना Curl_easy *data, int8_t sockindex)
{
  संरचना connectdata *conn = data->conn;
  जर(conn && CONN_SOCK_IDX_VALID(sockindex)) {
    संरचना Curl_cfilter *cf = conn->cfilter[sockindex];
    जर(cf)
      (शून्य)Curl_conn_cf_cntrl(cf, data, TRUE, CF_CTRL_FORGET_SOCKET, 0, रिक्त);
    fake_sclose(conn->sock[sockindex]);
    conn->sock[sockindex] = CURL_SOCKET_BAD;
  }
}

स्थिर शून्य myssh_SESSION_DISCONNECT(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc)
{
  /* during weird times when we have been prematurely aborted, the channel
     is still alive when we reach this state and we MUST kill the channel
     properly first */
  जर(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = रिक्त;
  }

  जर(sshc->sftp_file) {
    sftp_close(sshc->sftp_file);
    sshc->sftp_file = रिक्त;
  }
  जर(sshc->sftp_session) {
    sftp_free(sshc->sftp_session);
    sshc->sftp_session = रिक्त;
  }

  ssh_disconnect(sshc->ssh_session);
  जर(!ssh_version(SSH_VERSION_INT(0, 10, 0))) {
    /* conn->sock[FIRSTSOCKET] is closed by ssh_disconnect behind our back,
       tell the connection to forget about it. This libssh
       bug is fixed in 0.10.0. */
    conn_forget_socket(data, FIRSTSOCKET);
  }

  SSH_STRING_FREE_CHAR(sshc->homedir);

  myssh_to(data, sshc, SSH_SESSION_FREE);
}

स्थिर पूर्णांक myssh_SSH_SCP_DOWNLOAD(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc)
{
  uint64_t bytecount;
  पूर्णांक rc = ssh_scp_pull_request(sshc->scp_session);
  जर(rc != SSH_SCP_REQUEST_NEWFILE) {
    स्थिरांक अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_संदेश);
    परत myssh_to_ERROR(data, sshc, CURLE_REMOTE_FILE_NOT_FOUND);
  }

  /* download data */
  bytecount = ssh_scp_request_get_size64(sshc->scp_session);
  जर(bytecount > CURL_OFF_T_MAX)
    /* unlikely to be a real file */
    परत myssh_to_ERROR(data, sshc, CURLE_TOO_LARGE);
  data->req.maxdownload = (curl_off_t)bytecount;
  Curl_xfer_setup_recv(data, FIRSTSOCKET, data->req.maxdownload);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->send_idx = 0;

  myssh_to(data, sshc, SSH_STOP);
  परत 0;
}

स्थिर पूर्णांक myssh_in_TRANS_INIT(संरचना Curl_easy *data, संरचना ssh_conn *sshc,
                               संरचना SSHPROTO *sshp)
{
  CURLcode result;
  पूर्णांक rc = 0;
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  result = Curl_getworkingpath(data, sshc->homedir, &sshp->path);
  जर(result) {
    sshc->actualcode = result;
    myssh_to(data, sshc, SSH_STOP);
    परत 0;
  }

  /* Functions from the SCP subsystem cannot handle/return SSH_AGAIN */
  ssh_set_blocking(sshc->ssh_session, 1);

  जर(data->state.upload) {
    जर(data->state.infilesize < 0) {
      failf(data, "SCP requires a known file size for upload");
      परत myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
    }

    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_WRITE, sshp->path);
    myssh_to(data, sshc, SSH_SCP_UPLOAD_INIT);
  }
  अन्यथा {
    sshc->scp_session =
      ssh_scp_new(sshc->ssh_session, SSH_SCP_READ, sshp->path);
    myssh_to(data, sshc, SSH_SCP_DOWNLOAD_INIT);
  }

  जर(!sshc->scp_session) {
    स्थिरांक अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_संदेश);
    rc = myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }
  परत rc;
}

स्थिर शून्य sshc_cleanup(संरचना ssh_conn *sshc)
{
  जर(sshc->initialized) {
    जर(sshc->sftp_file) {
      sftp_close(sshc->sftp_file);
      sshc->sftp_file = रिक्त;
    }
    जर(sshc->sftp_session) {
      sftp_free(sshc->sftp_session);
      sshc->sftp_session = रिक्त;
    }
    जर(sshc->ssh_session) {
      ssh_free(sshc->ssh_session);
      sshc->ssh_session = रिक्त;
    }

    /* worst-case scenario cleanup */
    DEBUGASSERT(!sshc->ssh_session);
    DEBUGASSERT(!sshc->scp_session);

    जर(sshc->readdir_tmp) {
      ssh_string_free_char(sshc->readdir_tmp);
      sshc->readdir_tmp = रिक्त;
    }
    जर(sshc->quote_attrs) {
      sftp_attributes_free(sshc->quote_attrs);
      sshc->quote_attrs = रिक्त;
    }
    जर(sshc->readdir_attrs) {
      sftp_attributes_free(sshc->readdir_attrs);
      sshc->readdir_attrs = रिक्त;
    }
    जर(sshc->readdir_link_attrs) {
      sftp_attributes_free(sshc->readdir_link_attrs);
      sshc->readdir_link_attrs = रिक्त;
    }
    जर(sshc->privkey) {
      ssh_किल्ली_free(sshc->privkey);
      sshc->privkey = रिक्त;
    }
    जर(sshc->pubkey) {
      ssh_किल्ली_free(sshc->pubkey);
      sshc->pubkey = रिक्त;
    }

    curlx_safefree(sshc->passphrase);
    curlx_safefree(sshc->pub_किल्ली);
    curlx_safefree(sshc->priv_किल्ली);
    curlx_safefree(sshc->quote_path1);
    curlx_safefree(sshc->quote_path2);
    curlx_dyn_free(&sshc->readdir_buf);
    curlx_safefree(sshc->readdir_linkPath);
    SSH_STRING_FREE_CHAR(sshc->homedir);
    sshc->initialized = FALSE;
  }
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE_SETSTAT(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  पूर्णांक rc = sftp_setstat(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_attrs);
  जर(rc == SSH_AGAIN)
    परत rc;
  जर(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "setstat");
    परत rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE_SYMLINK(संरचना Curl_easy *data,
                                       संरचना ssh_conn *sshc)
{
  पूर्णांक rc = sftp_symlink(sshc->sftp_session, sshc->quote_path2,
                        sshc->quote_path1);
  जर(rc == SSH_AGAIN)
    परत rc;
  जर(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "symlink");
    परत rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE_MKDIR(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc)
{
  पूर्णांक rc = sftp_mkdir(sshc->sftp_session, sshc->quote_path1,
                      (mode_t)data->set.new_directory_perms);
  जर(rc == SSH_AGAIN)
    परत rc;
  जर(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "mkdir");
    परत rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE_RENAME(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  पूर्णांक rc = sftp_rename(sshc->sftp_session, sshc->quote_path1,
                       sshc->quote_path2);
  जर(rc == SSH_AGAIN)
    परत rc;
  जर(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rename");
    परत rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE_RMDIR(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc)
{
  पूर्णांक rc = sftp_rmdir(sshc->sftp_session, sshc->quote_path1);
  जर(rc == SSH_AGAIN)
    परत rc;
  जर(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rmdir");
    परत rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_QUOTE_UNLINK(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  पूर्णांक rc = sftp_unlink(sshc->sftp_session, sshc->quote_path1);
  जर(rc == SSH_AGAIN)
    परत rc;
  जर(rc && !sshc->acceptfail) {
    myssh_quote_error(data, sshc, "rm");
    परत rc;
  }
  myssh_to(data, sshc, SSH_SFTP_NEXT_QUOTE);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_GETINFO(संरचना Curl_easy *data,
                                 संरचना ssh_conn *sshc)
{
  जर(data->set.get_filetime)
    myssh_to(data, sshc, SSH_SFTP_FILETIME);
  अन्यथा
    myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_FILETIME(संरचना Curl_easy *data,
                                  संरचना ssh_conn *sshc,
                                  संरचना SSHPROTO *sshp)
{
  sftp_attributes attrs;
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  attrs = sftp_stat(sshc->sftp_session, sshp->path);
  जर(attrs) {
    data->info.filetime = attrs->mtime;
    sftp_attributes_free(attrs);
  }

  myssh_to(data, sshc, SSH_SFTP_TRANS_INIT);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_TRANS_INIT(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp)
{
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);
  जर(data->state.upload)
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  अन्यथा {
    माप_प्रकार path_len = म_माप(sshp->path);

    जर(path_len && sshp->path[path_len - 1] == '/')
      myssh_to(data, sshc, SSH_SFTP_READDIR_INIT);
    अन्यथा
      myssh_to(data, sshc, SSH_SFTP_DOWNLOAD_INIT);
  }

  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_CREATE_DIRS_INIT(संरचना Curl_easy *data,
                                          संरचना ssh_conn *sshc,
                                          संरचना SSHPROTO *sshp)
{
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  जर(म_माप(sshp->path) > 1) {
    sshc->slash_pos = sshp->path + 1; /* ignore the leading '/' */
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  }
  अन्यथा {
    myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  }
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_CREATE_DIRS(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc,
                                     संरचना SSHPROTO *sshp)
{
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  sshc->slash_pos = म_अक्षर(sshc->slash_pos, '/');
  जर(sshc->slash_pos) {
    *sshc->slash_pos = 0;

    infof(data, "Creating directory '%s'", sshp->path);
    myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS_MKDIR);
    परत SSH_NO_ERROR;
  }
  myssh_to(data, sshc, SSH_SFTP_UPLOAD_INIT);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SFTP_CREATE_DIRS_MKDIR(संरचना Curl_easy *data,
                                           संरचना ssh_conn *sshc,
                                           संरचना SSHPROTO *sshp)
{
  पूर्णांक rc;
  पूर्णांक err;
  /* 'mode' - parameter is preliminary - default to 0644 */
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = sftp_mkdir(sshc->sftp_session, sshp->path,
                  (mode_t)data->set.new_directory_perms);
  जर(rc == SSH_AGAIN)
    परत rc;
  *sshc->slash_pos = '/';
  ++sshc->slash_pos;
  जर(rc < 0) {
    /*
     * Abort if failure was not that the directory already
     * exists or the permission was denied (creation might
     * succeed further down the path) - retry on unspecific
     * FAILURE also
     */
    err = sftp_get_error(sshc->sftp_session);
    जर((err != SSH_FX_FILE_ALREADY_EXISTS) &&
       (err != SSH_FX_FAILURE) &&
       (err != SSH_FX_PERMISSION_DENIED)) {
      परत myssh_to_SFTP_CLOSE(data, sshc);
    }
    rc = 0; /* clear rc and continue */
  }
  myssh_to(data, sshc, SSH_SFTP_CREATE_DIRS);
  परत rc;
}

स्थिर पूर्णांक myssh_in_SCP_UPLOAD_INIT(संरचना Curl_easy *data,
                                    संरचना ssh_conn *sshc,
                                    संरचना SSHPROTO *sshp)
{
  पूर्णांक rc;
  जर(!sshp)
    परत myssh_to_ERROR(data, sshc, CURLE_FAILED_INIT);

  rc = ssh_scp_init(sshc->scp_session);
  जर(rc != SSH_OK) {
    स्थिरांक अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_संदेश);
    परत myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  rc = ssh_scp_ढकला_file64(sshc->scp_session, sshp->path,
                           (uint64_t)data->state.infilesize,
                           (पूर्णांक)data->set.new_file_perms);

  जर(rc != SSH_OK) {
    स्थिरांक अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_संदेश);
    परत myssh_to_ERROR(data, sshc, CURLE_UPLOAD_FAILED);
  }

  /* upload data */
  Curl_xfer_setup_send(data, FIRSTSOCKET);

  /* not set by Curl_xfer_setup to preserve io_flags */
  data->conn->recv_idx = FIRSTSOCKET;

  myssh_to(data, sshc, SSH_STOP);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SCP_DOWNLOAD_INIT(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  पूर्णांक rc = ssh_scp_init(sshc->scp_session);
  जर(rc != SSH_OK) {
    स्थिरांक अक्षर *err_संदेश = ssh_get_error(sshc->ssh_session);
    failf(data, "%s", err_संदेश);
    परत myssh_to_ERROR(data, sshc,
                          CURLE_COULDNT_CONNECT);
  }
  myssh_to(data, sshc, SSH_SCP_DOWNLOAD);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SCP_DONE(संरचना Curl_easy *data,
                             संरचना ssh_conn *sshc)
{
  जर(data->state.upload)
    myssh_to(data, sshc, SSH_SCP_SEND_EOF);
  अन्यथा
    myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SCP_SEND_EOF(संरचना Curl_easy *data,
                                 संरचना ssh_conn *sshc)
{
  जर(sshc->scp_session) {
    पूर्णांक rc = ssh_scp_close(sshc->scp_session);
    जर(rc == SSH_AGAIN) {
      /* Currently the ssh_scp_close handles waiting for
       * EOF in blocking way.
       */
      परत SSH_AGAIN;
    }
    जर(rc != SSH_OK) {
      infof(data,
            "Failed to close libssh scp channel: %s",
            ssh_get_error(sshc->ssh_session));
    }
  }

  myssh_to(data, sshc, SSH_SCP_CHANNEL_FREE);
  परत SSH_NO_ERROR;
}

स्थिर पूर्णांक myssh_in_SCP_CHANNEL_FREE(संरचना Curl_easy *data,
                                     संरचना ssh_conn *sshc)
{
  जर(sshc->scp_session) {
    ssh_scp_free(sshc->scp_session);
    sshc->scp_session = रिक्त;
  }
  CURL_TRC_SSH(data, "SCP DONE phase complete");

  ssh_set_blocking(sshc->ssh_session, 0);

  myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
  परत SSH_NO_ERROR;
}

स्थिर CURLcode myssh_in_SESSION_FREE(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc)
{
  CURLcode result;
  sshc_cleanup(sshc);
  /* the code we are about to return */
  result = sshc->actualcode;
  स_रखो(sshc, 0, आकार(संरचना ssh_conn));
  connclose(data->conn);
  sshc->state = SSH_SESSION_FREE;   /* current */
  sshc->nextstate = SSH_NO_STATE;
  myssh_to(data, sshc, SSH_STOP);
  परत result;
}

/*
 * myssh_statemachine() runs the SSH state machine as far as it can without
 * blocking and without reaching the end. The data the pointer 'block' points
 * to is set to TRUE if the libssh function returns SSH_AGAIN
 * meaning it wants to be called again when the socket is ready
 */
स्थिर CURLcode myssh_statemachine(संरचना Curl_easy *data,
                                   संरचना ssh_conn *sshc,
                                   संरचना SSHPROTO *sshp,
                                   bool *block)
{
  CURLcode result = CURLE_OK;
  पूर्णांक rc = SSH_NO_ERROR;

  *block = FALSE;                   /* we are not blocking by default */
  करा {

    निवड(sshc->state) {
    स्थिती SSH_INIT:
      myssh_state_init(data, sshc);
      FALLTHROUGH();
    स्थिती SSH_S_STARTUP:
      rc = myssh_in_S_STARTUP(data, sshc);
      जर(rc)
        थांबा;
      FALLTHROUGH();
    स्थिती SSH_HOSTKEY:
      rc = myssh_is_known(data, sshc);
      जर(rc != SSH_OK) {
        rc = myssh_to_ERROR(data, sshc, CURLE_PEER_FAILED_VERIFICATION);
        थांबा;
      }
      myssh_to(data, sshc, SSH_AUTHLIST);
      FALLTHROUGH();
    स्थिती SSH_AUTHLIST:
      rc = myssh_in_AUTHLIST(data, sshc);
      थांबा;
    स्थिती SSH_AUTH_PKEY_INIT:
      rc = myssh_in_AUTH_PKEY_INIT(data, sshc);
      थांबा;
    स्थिती SSH_AUTH_PKEY:
      rc = myssh_in_AUTH_PKEY(data, sshc);
      थांबा;
    स्थिती SSH_AUTH_GSSAPI:
      rc = myssh_in_AUTH_GSSAPI(data, sshc);
      थांबा;
    स्थिती SSH_AUTH_किल्ली_INIT:
      rc = myssh_in_AUTH_किल्ली_INIT(data, sshc);
      थांबा;
    स्थिती SSH_AUTH_किल्ली:
      rc = myssh_in_AUTH_किल्ली(data, sshc);
      थांबा;
    स्थिती SSH_AUTH_PASS_INIT:
      rc = myssh_in_AUTH_PASS_INIT(data, sshc);
      थांबा;
    स्थिती SSH_AUTH_PASS:
      rc = myssh_in_AUTH_PASS(data, sshc);
      थांबा;
    स्थिती SSH_AUTH_DONE:
      rc = myssh_in_AUTH_DONE(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_INIT:
      rc = myssh_in_SFTP_INIT(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_REALPATH:
      rc = myssh_in_SFTP_REALPATH(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE_INIT:
      rc = myssh_in_SFTP_QUOTE_INIT(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_POSTQUOTE_INIT:
      rc = myssh_in_SFTP_POSTQUOTE_INIT(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE:
      rc = myssh_in_SFTP_QUOTE(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_NEXT_QUOTE:
      rc = myssh_in_SFTP_NEXT_QUOTE(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE_STAT:
      rc = myssh_in_SFTP_QUOTE_STAT(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE_SETSTAT:
      rc = myssh_in_SFTP_QUOTE_SETSTAT(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE_SYMLINK:
      rc = myssh_in_SFTP_QUOTE_SYMLINK(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE_MKDIR:
      rc = myssh_in_SFTP_QUOTE_MKDIR(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE_RENAME:
      rc = myssh_in_SFTP_QUOTE_RENAME(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE_RMDIR:
      rc = myssh_in_SFTP_QUOTE_RMDIR(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE_UNLINK:
      rc = myssh_in_SFTP_QUOTE_UNLINK(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_QUOTE_STATVFS:
      rc = myssh_in_SFTP_QUOTE_STATVFS(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_GETINFO:
      rc = myssh_in_SFTP_GETINFO(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_FILETIME:
      rc = myssh_in_SFTP_FILETIME(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_TRANS_INIT:
      rc = myssh_in_SFTP_TRANS_INIT(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_UPLOAD_INIT:
      rc = myssh_in_UPLOAD_INIT(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_CREATE_DIRS_INIT:
      rc = myssh_in_SFTP_CREATE_DIRS_INIT(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_CREATE_DIRS:
      rc = myssh_in_SFTP_CREATE_DIRS(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_CREATE_DIRS_MKDIR:
      rc = myssh_in_SFTP_CREATE_DIRS_MKDIR(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_READDIR_INIT:
      rc = myssh_in_SFTP_READDIR_INIT(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_READDIR:
      rc = myssh_in_SFTP_READDIR(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_READDIR_LINK:
      rc = myssh_in_SFTP_READDIR_LINK(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_READDIR_BOTTOM:
      rc = myssh_in_SFTP_READDIR_BOTTOM(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_READDIR_DONE:
      rc = myssh_in_SFTP_READDIR_DONE(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_DOWNLOAD_INIT:
      rc = myssh_in_SFTP_DOWNLOAD_INIT(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_DOWNLOAD_STAT:
      rc = myssh_in_SFTP_DOWNLOAD_STAT(data, sshc);
      थांबा;
    स्थिती SSH_SFTP_CLOSE:
      rc = myssh_in_SFTP_CLOSE(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SFTP_SHUTDOWN:
      rc = myssh_in_SFTP_SHUTDOWN(data, sshc);
      थांबा;
    स्थिती SSH_SCP_TRANS_INIT:
      rc = myssh_in_TRANS_INIT(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SCP_UPLOAD_INIT:
      rc = myssh_in_SCP_UPLOAD_INIT(data, sshc, sshp);
      थांबा;
    स्थिती SSH_SCP_DOWNLOAD_INIT:
      rc = myssh_in_SCP_DOWNLOAD_INIT(data, sshc);
      जर(rc)
        थांबा;
      FALLTHROUGH();
    स्थिती SSH_SCP_DOWNLOAD:
      rc = myssh_SSH_SCP_DOWNLOAD(data, sshc);
      थांबा;
    स्थिती SSH_SCP_DONE:
      rc = myssh_in_SCP_DONE(data, sshc);
      थांबा;
    स्थिती SSH_SCP_SEND_EOF:
      rc = myssh_in_SCP_SEND_EOF(data, sshc);
      थांबा;
    स्थिती SSH_SCP_CHANNEL_FREE:
      myssh_in_SCP_CHANNEL_FREE(data, sshc);
      FALLTHROUGH();
    स्थिती SSH_SESSION_DISCONNECT:
      myssh_SESSION_DISCONNECT(data, sshc);
      FALLTHROUGH();
    स्थिती SSH_SESSION_FREE:
      result = myssh_in_SESSION_FREE(data, sshc);
      थांबा;
    स्थिती SSH_QUIT:
    पूर्वनिर्धारित:
      /* internal error */
      sshc->nextstate = SSH_NO_STATE;
      myssh_to(data, sshc, SSH_STOP);
      थांबा;
    }
    /* break the loop only on STOP or SSH_AGAIN. If `rc` is some
     * other error code, we will have progressed the state accordingly. */
  } पर्यंत((rc != SSH_AGAIN) && (sshc->state != SSH_STOP));

  जर(rc == SSH_AGAIN) {
    /* we would block, we need to wait for the socket to be ready (in the
       right direction too)! */
    *block = TRUE;
  }
  जर(!result && (sshc->state == SSH_STOP))
    result = sshc->actualcode;
  CURL_TRC_SSH(data, "[%s] statemachine() -> %d, block=%d",
               Curl_ssh_statename(sshc->state), (पूर्णांक)result, *block);
  परत result;
}

/* called repeatedly until done from multi.c */
स्थिर CURLcode myssh_multi_statemach(संरचना Curl_easy *data,
                                      bool *done)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  bool block;    /* we store the status and use that to provide
                    a Curl_ssh_pollset() implementation */
  CURLcode result;

  जर(!sshc || !sshp)
    परत CURLE_FAILED_INIT;
  result = myssh_statemachine(data, sshc, sshp, &block);
  *done = (sshc->state == SSH_STOP);
  myssh_block2waitfor(conn, sshc, block);

  परत result;
}

स्थिर CURLcode myssh_block_statemach(संरचना Curl_easy *data,
                                      संरचना ssh_conn *sshc,
                                      संरचना SSHPROTO *sshp,
                                      bool disconnect)
{
  संरचना connectdata *conn = data->conn;
  CURLcode result = CURLE_OK;

  पर्यंत((sshc->state != SSH_STOP) && !result) {
    bool block;
    timediff_t डावा_ms = 1000;

    result = myssh_statemachine(data, sshc, sshp, &block);
    जर(result)
      थांबा;

    जर(!disconnect) {
      result = Curl_pgrsCheck(data);
      जर(result)
        थांबा;

      डावा_ms = Curl_timeleft_ms(data);
      जर(डावा_ms < 0) {
        failf(data, "Operation timed out");
        परत CURLE_OPERATION_TIMEDOUT;
      }
    }

    जर(block) {
      curl_socket_t fd_read = conn->sock[FIRSTSOCKET];
      /* wait for the socket to become ready */
      (शून्य)SOCKET_READABLE(fd_read,
                            (!डावा_ms || डावा_ms > 1000) ? 1000 : डावा_ms);
    }
  }

  परत result;
}

स्थिर शून्य myssh_easy_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना SSHPROTO *sshp = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  curlx_safefree(sshp->path);
  curlx_free(sshp);
}

स्थिर शून्य myssh_conn_dtor(स्थिरांक शून्य *किल्ली, माप_प्रकार klen, शून्य *entry)
{
  संरचना ssh_conn *sshc = entry;
  (शून्य)किल्ली;
  (शून्य)klen;
  sshc_cleanup(sshc);
  curlx_free(sshc);
}

/*
 * SSH setup connection
 */
स्थिर CURLcode myssh_setup_connection(संरचना Curl_easy *data,
                                       संरचना connectdata *conn)
{
  संरचना SSHPROTO *sshp;
  संरचना ssh_conn *sshc;

  sshc = curlx_calloc(1, आकार(*sshc));
  जर(!sshc)
    परत CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&sshc->readdir_buf, CURL_PATH_MAX * 2);
  sshc->initialized = TRUE;
  जर(Curl_conn_meta_set(conn, CURL_META_SSH_CONN, sshc, myssh_conn_dtor))
    परत CURLE_OUT_OF_MEMORY;

  sshp = curlx_calloc(1, आकार(*sshp));
  जर(!sshp ||
     Curl_meta_set(data, CURL_META_SSH_EASY, sshp, myssh_easy_dtor))
    परत CURLE_OUT_OF_MEMORY;

  परत Curl_ssh_setup_pkey(data, sshc);
}

स्थिर Curl_recv scp_recv, sftp_recv;
स्थिर Curl_send scp_send, sftp_send;

/*
 * Curl_ssh_connect() gets called from Curl_protocol_connect() to allow us to
 * do protocol-specific actions at connect-time.
 */
स्थिर CURLcode myssh_connect(संरचना Curl_easy *data, bool *done)
{
  CURLcode result;
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *ssh = Curl_meta_get(data, CURL_META_SSH_EASY);
  curl_socket_t sock = conn->sock[FIRSTSOCKET];
  पूर्णांक rc;

  जर(!sshc || !ssh)
    परत CURLE_FAILED_INIT;

  CURL_TRC_SSH(data, "myssh_connect");
  जर(conn->scheme->protocol & CURLPROTO_SCP) {
    conn->recv[FIRSTSOCKET] = scp_recv;
    conn->send[FIRSTSOCKET] = scp_send;
  }
  अन्यथा {
    conn->recv[FIRSTSOCKET] = sftp_recv;
    conn->send[FIRSTSOCKET] = sftp_send;
  }

  sshc->ssh_session = ssh_new();
  जर(!sshc->ssh_session) {
    failf(data, "Failure initializing ssh session");
    परत CURLE_FAILED_INIT;
  }

  /* For IPv6 origins, use the `user_hostname` that has the "[]" enclosure.
   * Otherwise, use `hostname` that is IDN converted. */
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_HOST,
                       conn->origin->ipv6 ?
                       conn->origin->user_hostname : conn->origin->hostname);

  जर(rc != SSH_OK) {
    failf(data, "Could not set remote host");
    परत CURLE_FAILED_INIT;
  }

  rc = ssh_options_parse_config(sshc->ssh_session, रिक्त);
  जर(rc != SSH_OK) {
    infof(data, "Could not parse SSH configuration files");
    /* ignore */
  }

  CURL_TRC_SSH(data, "myssh_connect, set socket=%" FMT_SOCKET_T, sock);
  rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_FD, &sock);
  जर(rc != SSH_OK) {
    failf(data, "Could not set socket");
    परत CURLE_FAILED_INIT;
  }

  जर(Curl_creds_has_user(conn->creds)) {
    infof(data, "User: %s", conn->creds->user);
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_USER,
                         conn->creds->user);
    जर(rc != SSH_OK) {
      failf(data, "Could not set user");
      परत CURLE_FAILED_INIT;
    }
  }

  जर(CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS)) {
    infof(data, "Known hosts: %s",
          CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_KNOWNHOSTS,
                         CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    जर(rc == SSH_OK)
      /* libssh has two separate options for this. Set both to the same file
         to avoid surprises */
      rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_GLOBAL_KNOWNHOSTS,
                           CURL_EASY_STR(data, STRING_SSH_KNOWNHOSTS));
    जर(rc != SSH_OK) {
      failf(data, "Could not set known hosts file path");
      परत CURLE_FAILED_INIT;
    }
  }

  जर(conn->origin->port) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_PORT,
                         &conn->origin->port);
    जर(rc != SSH_OK) {
      failf(data, "Could not set remote port");
      परत CURLE_FAILED_INIT;
    }
  }

  जर(data->set.ssh_compression) {
    rc = ssh_options_set(sshc->ssh_session, SSH_OPTIONS_COMPRESSION,
                         "zlib,zlib@openssh.com,none");
    जर(rc != SSH_OK) {
      failf(data, "Could not set compression");
      परत CURLE_FAILED_INIT;
    }
  }

  sshc->privkey = रिक्त;
  sshc->pubkey = रिक्त;

  जर(sshc->pub_किल्ली) {
    rc = ssh_pki_import_pubkey_file(sshc->pub_किल्ली, &sshc->pubkey);
    जर(rc != SSH_OK) {
      failf(data, "Could not load public key file");
      परत CURLE_FAILED_INIT;
    }
  }

  /* we do not verify here, we do it at the state machine,
   * after connection */

  myssh_to(data, sshc, SSH_INIT);

  result = myssh_multi_statemach(data, done);

  परत result;
}

/* called from multi.c while DOing */
स्थिर CURLcode scp_doing(संरचना Curl_easy *data, bool *dophase_done)
{
  CURLcode result;

  result = myssh_multi_statemach(data, dophase_done);

  जर(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  परत result;
}

/*
 ***********************************************************************
 *
 * scp_perform()
 *
 * This is the actual DO function for SCP. Get a file according to
 * the options previously setup.
 */

स्थिर CURLcode scp_perform(संरचना Curl_easy *data,
                            bool *connected, bool *dophase_done)
{
  CURLcode result = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE;        /* not done yet */
  जर(!sshc)
    परत CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SCP_TRANS_INIT);

  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  जर(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  परत result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode scp_disconnect(संरचना Curl_easy *data,
                               संरचना connectdata *conn,
                               bool dead_connection)
{
  CURLcode result = CURLE_OK;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  (शून्य)dead_connection;

  जर(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SESSION_DISCONNECT);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  परत result;
}

/* generic done function for both SCP and SFTP called from their specific
   done functions */
स्थिर CURLcode myssh_done(संरचना Curl_easy *data,
                           संरचना ssh_conn *sshc,
                           CURLcode status)
{
  CURLcode result = CURLE_OK;
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);

  जर(!status && sshp) {
    /* run the state-machine */
    result = myssh_block_statemach(data, sshc, sshp, FALSE);
  }
  अन्यथा
    result = status;

  जर(Curl_pgrsDone(data, Curl_pgrs_now(data)))
    परत CURLE_ABORTED_BY_CALLBACK;

  CURL_REQ_CLEAR_IO(data);
  परत result;
}

स्थिर CURLcode scp_done(संरचना Curl_easy *data, CURLcode status,
                         bool premature)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  (शून्य)premature;

  जर(!sshc)
    परत CURLE_FAILED_INIT;
  जर(!status)
    myssh_to(data, sshc, SSH_SCP_DONE);

  परत myssh_done(data, sshc, status);
}

स्थिर CURLcode scp_send(संरचना Curl_easy *data, int8_t sockindex,
                         स्थिरांक uint8_t *mem, माप_प्रकार len, bool eos,
                         माप_प्रकार *pnwritten)
{
  पूर्णांक rc;
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  (शून्य)eos;
  *pnwritten = 0;

  जर(!sshc)
    परत CURLE_FAILED_INIT;

  rc = ssh_scp_write(sshc->scp_session, mem, len);

#जर 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be number of bytes read or SSH_ERROR. */
  myssh_block2waitfor(conn, sshc, (rc == SSH_AGAIN));

  जर(rc == SSH_AGAIN)
    परत CURLE_AGAIN;
  अन्यथा
#endif
  जर(rc != SSH_OK)
    परत CURLE_SSH;

  *pnwritten = len;
  परत CURLE_OK;
}

स्थिर CURLcode scp_recv(संरचना Curl_easy *data, int8_t sockindex,
                         अक्षर *mem, माप_प्रकार len, माप_प्रकार *pnread)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  पूर्णांक nread;

  (शून्य)sockindex; /* we only support SCP on the fixed known primary socket */
  *pnread = 0;

  जर(!sshc)
    परत CURLE_FAILED_INIT;

  /* libssh returns int */
  nread = ssh_scp_read(sshc->scp_session, mem, len);
  जर(nread == SSH_ERROR)
    परत CURLE_SSH;
#जर 0
  /* The following code is misleading, mostly added as wishful thinking
   * that libssh at some point would implement non-blocking ssh_scp_write/read.
   * Currently rc can only be SSH_OK or SSH_ERROR. */

  myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));
  जर(nread == SSH_AGAIN)
    परत CURLE_AGAIN;
#endif
  *pnread = (माप_प्रकार)nread;
  परत CURLE_OK;
}

/*
 * =============== SFTP ===============
 */

/*
 ***********************************************************************
 *
 * sftp_perform()
 *
 * This is the actual DO function for SFTP. Get a file/directory according to
 * the options previously setup.
 */

स्थिर CURLcode sftp_perform(संरचना Curl_easy *data,
                             bool *connected,
                             bool *dophase_done)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(data->conn, CURL_META_SSH_CONN);
  CURLcode result = CURLE_OK;

  CURL_TRC_SSH(data, "DO phase starts");

  *dophase_done = FALSE; /* not done yet */
  जर(!sshc)
    परत CURLE_FAILED_INIT;

  /* start the first command in the DO phase */
  myssh_to(data, sshc, SSH_SFTP_QUOTE_INIT);

  /* run the state-machine */
  result = myssh_multi_statemach(data, dophase_done);

  *connected = Curl_conn_is_connected(data->conn, FIRSTSOCKET);

  जर(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }

  परत result;
}

/* called from multi.c while DOing */
स्थिर CURLcode sftp_doing(संरचना Curl_easy *data,
                           bool *dophase_done)
{
  CURLcode result = myssh_multi_statemach(data, dophase_done);
  जर(*dophase_done) {
    CURL_TRC_SSH(data, "DO phase is complete");
  }
  परत result;
}

/* BLOCKING, but the function is using the state machine so the only reason
   this is still blocking is that the multi interface code has no support for
   disconnecting operations that takes a while */
स्थिर CURLcode sftp_disconnect(संरचना Curl_easy *data,
                                संरचना connectdata *conn,
                                bool dead_connection)
{
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  संरचना SSHPROTO *sshp = Curl_meta_get(data, CURL_META_SSH_EASY);
  CURLcode result = CURLE_OK;
  (शून्य)dead_connection;

  CURL_TRC_SSH(data, "DISCONNECT starts now");

  जर(sshc && sshc->ssh_session) {
    /* only if there is a session still around to use! */
    myssh_to(data, sshc, SSH_SFTP_SHUTDOWN);
    result = myssh_block_statemach(data, sshc, sshp, TRUE);
  }

  CURL_TRC_SSH(data, "DISCONNECT is done");
  परत result;
}

स्थिर CURLcode sftp_done(संरचना Curl_easy *data, CURLcode status,
                          bool premature)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  जर(!sshc)
    परत CURLE_FAILED_INIT;
  जर(!status) {
    /* Post quote commands are executed after the SFTP_CLOSE state to avoid
       errors that could happen due to open file handles during POSTQUOTE
       operation */
    जर(!premature && data->set.postquote && !conn->bits.retry)
      sshc->nextstate = SSH_SFTP_POSTQUOTE_INIT;
    myssh_to(data, sshc, SSH_SFTP_CLOSE);
  }
  परत myssh_done(data, sshc, status);
}

/* return number of sent bytes */
स्थिर CURLcode sftp_send(संरचना Curl_easy *data, int8_t sockindex,
                          स्थिरांक uint8_t *mem, माप_प्रकार len, bool eos,
                          माप_प्रकार *pnwritten)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nwrite;

  (शून्य)sockindex;
  (शून्य)eos;
  *pnwritten = 0;

  जर(!sshc)
    परत CURLE_FAILED_INIT;

#जर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  निवड(sshc->sftp_send_state) {
  स्थिती 0:
    sftp_file_set_nonblocking(sshc->sftp_file);
    जर(sftp_aio_begin_write(sshc->sftp_file, mem, len,
                            &sshc->sftp_send_aio) == SSH_ERROR) {
      परत CURLE_SEND_ERROR;
    }
    sshc->sftp_send_state = 1;
    FALLTHROUGH();
  स्थिती 1:
    nwrite = sftp_aio_wait_write(&sshc->sftp_send_aio);
    myssh_block2waitfor(conn, sshc, (nwrite == SSH_AGAIN) ? TRUE : FALSE);
    जर(nwrite == SSH_AGAIN)
      परत CURLE_AGAIN;
    अन्यथा जर(nwrite < 0)
      परत CURLE_SEND_ERROR;

    /*
     * sftp_aio_wait_write() would free sftp_send_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_send_state = 0;
    *pnwritten = (माप_प्रकार)nwrite;
    परत CURLE_OK;
  पूर्वनिर्धारित:
    /* we never reach here */
    परत CURLE_SEND_ERROR;
  }
#अन्यथा
  /*
   * limit the writes to the maximum specified in Section 3 of
   * https://datatracker.ietf.org/doc/html/draft-ietf-secsh-filexfer-02
   *
   * libssh started applying appropriate read/write length limits
   * internally since version 0.11.0, hence such an operation is
   * not needed for versions after (and including) 0.11.0.
   */
  जर(len > 32768)
    len = 32768;

  nwrite = sftp_write(sshc->sftp_file, mem, len);

  myssh_block2waitfor(conn, sshc, FALSE);

#जर 0 /* not returned by libssh on write */
  जर(nwrite == SSH_AGAIN) {
    *err = CURLE_AGAIN;
    nwrite = 0;
  }
  अन्यथा
#endif
  जर(nwrite < 0)
    परत CURLE_SSH;

  *pnwritten = (माप_प्रकार)nwrite;
  परत CURLE_OK;
#endif
}

/*
 * Return number of received (decrypted) bytes
 * or <0 on error
 */
स्थिर CURLcode sftp_recv(संरचना Curl_easy *data, int8_t sockindex,
                          अक्षर *mem, माप_प्रकार len, माप_प्रकार *pnread)
{
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);
  ssize_t nread;

  (शून्य)sockindex;
  *pnread = 0;

  DEBUGASSERT(len < CURL_MAX_READ_SIZE);
  जर(!sshc)
    परत CURLE_FAILED_INIT;

  निवड(sshc->sftp_recv_state) {
  स्थिती 0:
#जर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    जर(sftp_aio_begin_read(sshc->sftp_file, len,
                           &sshc->sftp_recv_aio) == SSH_ERROR) {
      परत CURLE_RECV_ERROR;
    }
#अन्यथा
    sshc->sftp_file_index =
      sftp_async_read_begin(sshc->sftp_file, (uint32_t)len);
    जर(sshc->sftp_file_index < 0)
      परत CURLE_RECV_ERROR;
#endif

    FALLTHROUGH();
  स्थिती 1:
    sshc->sftp_recv_state = 1;

#जर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
    nread = sftp_aio_wait_read(&sshc->sftp_recv_aio, mem, len);
#अन्यथा
    nread = sftp_async_read(sshc->sftp_file, mem, (uint32_t)len,
                            (uint32_t)sshc->sftp_file_index);
#endif

    myssh_block2waitfor(conn, sshc, (nread == SSH_AGAIN));

    जर(nread == SSH_AGAIN)
      परत CURLE_AGAIN;
    अन्यथा जर(nread < 0)
      परत CURLE_RECV_ERROR;

    /*
     * sftp_aio_wait_read() would free sftp_recv_aio and
     * assign it NULL in all cases except when it returns
     * SSH_AGAIN.
     */

    sshc->sftp_recv_state = 0;
    *pnread = (माप_प्रकार)nread;
    परत CURLE_OK;

  पूर्वनिर्धारित:
    /* we never reach here */
    परत CURLE_RECV_ERROR;
  }
}

स्थिर CURLcode myssh_do_it(संरचना Curl_easy *data, bool *done)
{
  CURLcode result;
  bool connected = FALSE;
  संरचना connectdata *conn = data->conn;
  संरचना ssh_conn *sshc = Curl_conn_meta_get(conn, CURL_META_SSH_CONN);

  *done = FALSE;                /* default to false */
  जर(!sshc)
    परत CURLE_FAILED_INIT;

  data->req.size = -1;          /* make sure this is unknown at this point */

  sshc->actualcode = CURLE_OK;  /* reset error code */
  sshc->secondCreateDirs = 0;   /* reset the create directory attempt state
                                   variable */

  Curl_pgrsReset(data);

  जर(conn->scheme->protocol & CURLPROTO_SCP)
    result = scp_perform(data, &connected, done);
  अन्यथा
    result = sftp_perform(data, &connected, done);

  परत result;
}

CURLcode Curl_ssh_init(शून्य)
{
  जर(ssh_init()) {
    DEBUGF(curl_mfprintf(मानक_त्रुटि, "Error: libssh_init failed\n"));
    परत CURLE_FAILED_INIT;
  }
  परत CURLE_OK;
}

शून्य Curl_ssh_cleanup(शून्य)
{
  (शून्य)ssh_finalize();
}

शून्य Curl_ssh_version(अक्षर *buffer, माप_प्रकार buflen)
{
  (शून्य)curl_msnprintf(buffer, buflen, "libssh/%s", ssh_version(0));
}

/*
 * SCP.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_scp = {
  myssh_setup_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  scp_done,                             /* done */
  ZERO_NULL,                            /* do_more */
  myssh_connect,                        /* connect_it */
  myssh_multi_statemach,                /* connecting */
  scp_doing,                            /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  scp_disconnect,                       /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

/*
 * SFTP.
 */
स्थिरांक संरचना Curl_protocol Curl_protocol_sftp = {
  myssh_setup_connection,               /* setup_connection */
  myssh_do_it,                          /* do_it */
  sftp_done,                            /* done */
  ZERO_NULL,                            /* do_more */
  myssh_connect,                        /* connect_it */
  myssh_multi_statemach,                /* connecting */
  sftp_doing,                           /* doing */
  Curl_ssh_pollset,                     /* proto_pollset */
  Curl_ssh_pollset,                     /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  Curl_ssh_pollset,                     /* perform_pollset */
  sftp_disconnect,                      /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#endif /* USE_LIBSSH */
