#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VSSH_SSH_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VSSH_SSH_H
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
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_sftp;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_scp;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSH

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH2
#ਅਨ੍ਤਰ੍ਭਾਵ <libssh2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libssh2_sftp.h>
#ਜਾਂਜੇ defined(USE_LIBSSH)
/* in 0.10.0 or later, ignore deprecated warnings */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSH_SUPPRESS_DEPRECATED
#ਅਨ੍ਤਰ੍ਭਾਵ <libssh/libssh.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libssh/sftp.h>
#ਜੇ_ਅੰਤ

/* meta key for storing protocol meta at easy handle */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_SSH_EASY   "meta:proto:ssh:easy"
/* meta key for storing protocol meta at connection */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_SSH_CONN   "meta:proto:ssh:conn"

/****************************************************************************
 * SSH unique setup
 ***************************************************************************/
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  SSH_NO_STATE = -1,  /* Used for "nextState" so say there is none */
  SSH_STOP = 0,       /* do nothing state, stops the state machine */

  SSH_INIT,           /* First state in SSH-CONNECT */
  SSH_S_STARTUP,      /* Session startup */
  SSH_HOSTKEY,        /* verify hostkey */
  SSH_AUTHLIST,
  SSH_AUTH_PKEY_INIT,
  SSH_AUTH_PKEY,
  SSH_AUTH_PASS_INIT,
  SSH_AUTH_PASS,
  SSH_AUTH_AGENT_INIT, /* initialize then wait for connection to agent */
  SSH_AUTH_AGENT_LIST, /* ask for list then wait for entire list to come */
  SSH_AUTH_AGENT,      /* attempt one key at a time */
  SSH_AUTH_HOST_INIT,
  SSH_AUTH_HOST,
  SSH_AUTH_ਕੁੰਜੀ_INIT,
  SSH_AUTH_ਕੁੰਜੀ,
  SSH_AUTH_GSSAPI,
  SSH_AUTH_ਪੂਰਾ,
  SSH_SFTP_INIT,
  SSH_SFTP_REALPATH,   /* Last state in SSH-CONNECT */

  SSH_SFTP_QUOTE_INIT, /* First state in SFTP-DO */
  SSH_SFTP_POSTQUOTE_INIT, /* (Possibly) First state in SFTP-DONE */
  SSH_SFTP_QUOTE,
  SSH_SFTP_ਅਗਲਾ_QUOTE,
  SSH_SFTP_QUOTE_STAT,
  SSH_SFTP_QUOTE_SETSTAT,
  SSH_SFTP_QUOTE_SYMLINK,
  SSH_SFTP_QUOTE_MKDIR,
  SSH_SFTP_QUOTE_RENAME,
  SSH_SFTP_QUOTE_RMDIR,
  SSH_SFTP_QUOTE_UNLINK,
  SSH_SFTP_QUOTE_STATVFS,
  SSH_SFTP_GETINFO,
  SSH_SFTP_FILETIME,
  SSH_SFTP_TRANS_INIT,
  SSH_SFTP_UPLOAD_INIT,
  SSH_SFTP_CREATE_DIRS_INIT,
  SSH_SFTP_CREATE_DIRS,
  SSH_SFTP_CREATE_DIRS_MKDIR,
  SSH_SFTP_READDIR_INIT,
  SSH_SFTP_READDIR,
  SSH_SFTP_READDIR_LINK,
  SSH_SFTP_READDIR_BOTTOM,
  SSH_SFTP_READDIR_ਪੂਰਾ,
  SSH_SFTP_DOWNLOAD_INIT,
  SSH_SFTP_DOWNLOAD_STAT, /* Last state in SFTP-DO */
  SSH_SFTP_CLOSE,    /* Last state in SFTP-DONE */
  SSH_SFTP_SHUTDOWN, /* First state in SFTP-DISCONNECT */
  SSH_SCP_TRANS_INIT, /* First state in SCP-DO */
  SSH_SCP_UPLOAD_INIT,
  SSH_SCP_DOWNLOAD_INIT,
  SSH_SCP_DOWNLOAD,
  SSH_SCP_ਪੂਰਾ,
  SSH_SCP_SEND_EOF,
  SSH_SCP_WAIT_EOF,
  SSH_SCP_WAIT_CLOSE,
  SSH_SCP_CHANNEL_FREE,   /* Last state in SCP-DONE */
  SSH_SESSION_DISCONNECT, /* First state in SCP-DISCONNECT */
  SSH_SESSION_FREE,       /* Last state in SCP/SFTP-DISCONNECT */
  SSH_QUIT,
  SSH_ਆਖਰੀ  /* never used */
} sshstate;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਰਸਤਾ_MAX 1024

/* this struct is used in the HandleData struct which is part of the
   Curl_easy, which means this is used on a per-easy handle basis.
   Everything that is strictly related to a connection is banned from this
   struct. */
ਬਣਤਰ SSHPROTO {
  ਅੱਖਰ *ਰਸਤਾ;        /* the path we operate on, at least one byte long */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH2
  ਬਣਤਰ dynbuf readdir_link;
  ਬਣਤਰ dynbuf readdir;
  ਅੱਖਰ readdir_filename[CURL_ਰਸਤਾ_MAX + 1];
  ਅੱਖਰ readdir_longentry[CURL_ਰਸਤਾ_MAX + 1];

  LIBSSH2_SFTP_ਗੁਣ quote_attrs; /* used by the SFTP_QUOTE state */

  /* Here's a set of struct members used by the SFTP_READDIR state */
  LIBSSH2_SFTP_ਗੁਣ readdir_attrs;
#ਜੇ_ਅੰਤ
};

/* ssh_conn is used for struct connection-oriented data in the connectdata
   struct */
ਬਣਤਰ ssh_conn {
  ਅਚਲ ਅੱਖਰ *authlist;       /* List of auth. methods, managed by libssh2 */

  /* common */
  ਅੱਖਰ *passphrase;           /* strdup'ed pass-phrase to use or NULL */
  ਅੱਖਰ *pub_ਕੁੰਜੀ;              /* strdup'ed public key file */
  ਅੱਖਰ *priv_ਕੁੰਜੀ;             /* strdup'ed private key file */
  sshstate state;             /* always use ssh.c:state() to change state! */
  sshstate nextstate;         /* the state to goto after stopping */
  ਬਣਤਰ curl_slist *quote_ਵਸਤੂ; /* for the quote option */
  ਅੱਖਰ *quote_path1;          /* two generic pointers for the QUOTE stuff */
  ਅੱਖਰ *quote_path2;

  ਅੱਖਰ *homedir;              /* when doing SFTP we figure out home directory
                                 in the connect phase */
  /* end of READDIR stuff */

  ਪੂਰਨ ਦੂਜਾ_Create_Dirs;         /* counter use by the code to see if the
                                   second attempt has been made to change
                                   to/create a directory */
  ਪੂਰਨ waitfor;                  /* REQ_IO_RECV/REQ_IO_SEND bits overriding
                                   pollset given flags */
  ਅੱਖਰ *slash_pos;              /* used by the SFTP_CREATE_DIRS state */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH
  CURLcode actualcode;        /* the actual error code */
  ਅੱਖਰ *readdir_link_ਰਸਤਾ;
  size_t readdir_len;
  ਬਣਤਰ dynbuf readdir_buf;
/* our variables */
  ਬਿਨਾਨਿਸ਼ਾਨੀ kbd_state; /* 0 or 1 */
  ssh_ਕੁੰਜੀ privkey;
  ssh_ਕੁੰਜੀ pubkey;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ auth_methods;
  ssh_session ssh_session;
  ssh_scp scp_session;
  sftp_session sftp_session;
  sftp_file sftp_file;
  sftp_dir sftp_dir;

  ਬਿਨਾਨਿਸ਼ਾਨੀ sftp_recv_state; /* 0 or 1 */
#ਜੇ LIBSSH_ਸੰਸਕਰਣ_INT > SSH_ਸੰਸਕਰਣ_INT(0, 11, 0)
  sftp_aio sftp_recv_aio;

  sftp_aio sftp_send_aio;
  ਬਿਨਾਨਿਸ਼ਾਨੀ sftp_send_state; /* 0 or 1 */
#ਨਹੀਂਤਾਂ
  ਪੂਰਨ sftp_file_ਸੂਚਕਾਂਕ; /* for async read */
#ਜੇ_ਅੰਤ
  sftp_ਗੁਣ readdir_attrs; /* used by the SFTP readdir actions */
  sftp_ਗੁਣ readdir_link_attrs; /* used by the SFTP readdir actions */
  sftp_ਗੁਣ quote_attrs; /* used by the SFTP_QUOTE state */

  ਅਚਲ ਅੱਖਰ *readdir_filename; /* points within readdir_attrs */
  ਅਚਲ ਅੱਖਰ *readdir_longentry;
  ਅੱਖਰ *readdir_tmp;
  ਦੋਆਧਾਰੀ_ਅੰਕ(initialized);
#ਜਾਂਜੇ defined(USE_LIBSSH2)
  LIBSSH2_SESSION *ssh_session; /* Secure Shell session */
  LIBSSH2_CHANNEL *ssh_channel; /* Secure Shell channel handle */
  LIBSSH2_SFTP *sftp_session;   /* SFTP handle */
  LIBSSH2_SFTP_HANDLE *sftp_handle;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  /* for HTTPS proxy storage */
  Curl_recv *tls_recv;
  Curl_send *tls_send;
#ਜੇ_ਅੰਤ

  LIBSSH2_AGENT *ssh_agent;     /* proxy to ssh-agent/pageant */
  ਬਣਤਰ libssh2_agent_publickey *sshagent_ਪਛਾਣ_ਤੱਤ;
  ਬਣਤਰ libssh2_agent_publickey *sshagent_prev_ਪਛਾਣ_ਤੱਤ;
  LIBSSH2_KNOWNHOSTS *kh;
#ਜੇ_ਅੰਤ /* USE_LIBSSH */
  ਦੋਆਧਾਰੀ_ਅੰਕ(authed);                /* the connection has been authenticated fine */
  ਦੋਆਧਾਰੀ_ਅੰਕ(acceptfail);            /* used by the SFTP_QUOTE (continue if
                                 quote command fails) */
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH
#ਜੇ LIBSSH_ਸੰਸਕਰਣ_INT < SSH_ਸੰਸਕਰਣ_INT(0, 9, 0)
#ਗਲਤੀ "SCP/SFTP protocols require libssh 0.9.0 or greater"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_LIBSSH2

/* Feature detection based on version numbers to better work with
   non-configure platforms */

#ਜੇ !defined(LIBSSH2_ਸੰਸਕਰਣ_ਅੰਕ) || (LIBSSH2_ਸੰਸਕਰਣ_ਅੰਕ < 0x010900)
#ਗਲਤੀ "SCP/SFTP protocols require libssh2 1.9.0 or greater"
/* 1.9.0 was released on June 20 2019 */
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* USE_LIBSSH2 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਅਚਲ ਅੱਖਰ *Curl_ssh_statename(sshstate state);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssh_statename(x) ""
#ਜੇ_ਅੰਤ
ਖਾਲੀ Curl_ssh_ਸੈੱਟ_state(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ ssh_conn *sshc,
                        sshstate nowstate);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ myssh_ਮੰਜ਼ਿਲ(x, y, z) Curl_ssh_ਸੈੱਟ_state(x, y, z)

/* generic SSH backend functions */
CURLcode Curl_ssh_init(ਖਾਲੀ);
ਖਾਲੀ Curl_ssh_cleanup(ਖਾਲੀ);
ਖਾਲੀ Curl_ssh_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t buflen);
ਖਾਲੀ Curl_ssh_attach(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                     ਬਣਤਰ connectdata *conn);
#ਨਹੀਂਤਾਂ /* !USE_SSH */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssh_cleanup()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssh_attach(x, y)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssh_init() 0
#ਜੇ_ਅੰਤ /* USE_SSH */

#ਜੇ_ਅੰਤ /* HEADER_CURL_SSH_H */
