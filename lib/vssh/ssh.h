#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VSSH_SSH_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VSSH_SSH_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_sftp;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_scp;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSH

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH2
#ಅನ್ತರ್ಭಾವ <libssh2.h>
#ಅನ್ತರ್ಭಾವ <libssh2_sftp.h>
#ಅಥವಾಆದರೆ defined(USE_LIBSSH)
/* in 0.10.0 or later, ignore deprecated warnings */
#ವ್ಯಾಖ್ಯಾನಿಸು SSH_SUPPRESS_DEPRECATED
#ಅನ್ತರ್ಭಾವ <libssh/libssh.h>
#ಅನ್ತರ್ಭಾವ <libssh/sftp.h>
#ಆದರೆ_ಅಂತ್ಯ

/* meta key for storing protocol meta at easy handle */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_SSH_EASY   "meta:proto:ssh:easy"
/* meta key for storing protocol meta at connection */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_SSH_CONN   "meta:proto:ssh:conn"

/****************************************************************************
 * SSH unique setup
 ***************************************************************************/
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
  SSH_AUTH_ಕೀಲಿ_INIT,
  SSH_AUTH_ಕೀಲಿ,
  SSH_AUTH_GSSAPI,
  SSH_AUTH_ಮುಗಿದ,
  SSH_SFTP_INIT,
  SSH_SFTP_REALPATH,   /* Last state in SSH-CONNECT */

  SSH_SFTP_QUOTE_INIT, /* First state in SFTP-DO */
  SSH_SFTP_POSTQUOTE_INIT, /* (Possibly) First state in SFTP-DONE */
  SSH_SFTP_QUOTE,
  SSH_SFTP_ಮುಂದಿನ_QUOTE,
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
  SSH_SFTP_READDIR_ಮುಗಿದ,
  SSH_SFTP_DOWNLOAD_INIT,
  SSH_SFTP_DOWNLOAD_STAT, /* Last state in SFTP-DO */
  SSH_SFTP_CLOSE,    /* Last state in SFTP-DONE */
  SSH_SFTP_SHUTDOWN, /* First state in SFTP-DISCONNECT */
  SSH_SCP_TRANS_INIT, /* First state in SCP-DO */
  SSH_SCP_UPLOAD_INIT,
  SSH_SCP_DOWNLOAD_INIT,
  SSH_SCP_DOWNLOAD,
  SSH_SCP_ಮುಗಿದ,
  SSH_SCP_SEND_EOF,
  SSH_SCP_WAIT_EOF,
  SSH_SCP_WAIT_CLOSE,
  SSH_SCP_CHANNEL_FREE,   /* Last state in SCP-DONE */
  SSH_SESSION_DISCONNECT, /* First state in SCP-DISCONNECT */
  SSH_SESSION_FREE,       /* Last state in SCP/SFTP-DISCONNECT */
  SSH_QUIT,
  SSH_ಕೊನೆಯ  /* never used */
} sshstate;

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಮಾರ್ಗ_MAX 1024

/* this struct is used in the HandleData struct which is part of the
   Curl_easy, which means this is used on a per-easy handle basis.
   Everything that is strictly related to a connection is banned from this
   struct. */
ರಚನೆ SSHPROTO {
  ಅಕ್ಷರ *ಮಾರ್ಗ;        /* the path we operate on, at least one byte long */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH2
  ರಚನೆ dynbuf readdir_link;
  ರಚನೆ dynbuf readdir;
  ಅಕ್ಷರ readdir_filename[CURL_ಮಾರ್ಗ_MAX + 1];
  ಅಕ್ಷರ readdir_longentry[CURL_ಮಾರ್ಗ_MAX + 1];

  LIBSSH2_SFTP_ಗುಣಲಕ್ಷಣಗಳು quote_attrs; /* used by the SFTP_QUOTE state */

  /* Here's a set of struct members used by the SFTP_READDIR state */
  LIBSSH2_SFTP_ಗುಣಲಕ್ಷಣಗಳು readdir_attrs;
#ಆದರೆ_ಅಂತ್ಯ
};

/* ssh_conn is used for struct connection-oriented data in the connectdata
   struct */
ರಚನೆ ssh_conn {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authlist;       /* List of auth. methods, managed by libssh2 */

  /* common */
  ಅಕ್ಷರ *passphrase;           /* strdup'ed pass-phrase to use or NULL */
  ಅಕ್ಷರ *pub_ಕೀಲಿ;              /* strdup'ed public key file */
  ಅಕ್ಷರ *priv_ಕೀಲಿ;             /* strdup'ed private key file */
  sshstate state;             /* always use ssh.c:state() to change state! */
  sshstate nextstate;         /* the state to goto after stopping */
  ರಚನೆ curl_slist *quote_ಅಂಶ; /* for the quote option */
  ಅಕ್ಷರ *quote_path1;          /* two generic pointers for the QUOTE stuff */
  ಅಕ್ಷರ *quote_path2;

  ಅಕ್ಷರ *homedir;              /* when doing SFTP we figure out home directory
                                 in the connect phase */
  /* end of READDIR stuff */

  ಪೂರ್ಣಾಂಕ ಎರಡನೇ_Create_Dirs;         /* counter use by the code to see if the
                                   second attempt has been made to change
                                   to/create a directory */
  ಪೂರ್ಣಾಂಕ waitfor;                  /* REQ_IO_RECV/REQ_IO_SEND bits overriding
                                   pollset given flags */
  ಅಕ್ಷರ *slash_pos;              /* used by the SFTP_CREATE_DIRS state */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH
  CURLcode actualcode;        /* the actual error code */
  ಅಕ್ಷರ *readdir_link_ಮಾರ್ಗ;
  size_t readdir_len;
  ರಚನೆ dynbuf readdir_buf;
/* our variables */
  ಚಿಹ್ನೆರಹಿತ kbd_state; /* 0 or 1 */
  ssh_ಕೀಲಿ privkey;
  ssh_ಕೀಲಿ pubkey;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ auth_methods;
  ssh_session ssh_session;
  ssh_scp scp_session;
  sftp_session sftp_session;
  sftp_file sftp_file;
  sftp_dir sftp_dir;

  ಚಿಹ್ನೆರಹಿತ sftp_recv_state; /* 0 or 1 */
#ಆದರೆ LIBSSH_ಆವೃತ್ತಿ_INT > SSH_ಆವೃತ್ತಿ_INT(0, 11, 0)
  sftp_aio sftp_recv_aio;

  sftp_aio sftp_send_aio;
  ಚಿಹ್ನೆರಹಿತ sftp_send_state; /* 0 or 1 */
#ಇಲ್ಲದಿದ್ದರೆ
  ಪೂರ್ಣಾಂಕ sftp_file_ಸೂಚ್ಯಂಕ; /* for async read */
#ಆದರೆ_ಅಂತ್ಯ
  sftp_ಗುಣಲಕ್ಷಣಗಳು readdir_attrs; /* used by the SFTP readdir actions */
  sftp_ಗುಣಲಕ್ಷಣಗಳು readdir_link_attrs; /* used by the SFTP readdir actions */
  sftp_ಗುಣಲಕ್ಷಣಗಳು quote_attrs; /* used by the SFTP_QUOTE state */

  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *readdir_filename; /* points within readdir_attrs */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *readdir_longentry;
  ಅಕ್ಷರ *readdir_tmp;
  ಬಿಟ್(initialized);
#ಅಥವಾಆದರೆ defined(USE_LIBSSH2)
  LIBSSH2_SESSION *ssh_session; /* Secure Shell session */
  LIBSSH2_CHANNEL *ssh_channel; /* Secure Shell channel handle */
  LIBSSH2_SFTP *sftp_session;   /* SFTP handle */
  LIBSSH2_SFTP_HANDLE *sftp_handle;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  /* for HTTPS proxy storage */
  Curl_recv *tls_recv;
  Curl_send *tls_send;
#ಆದರೆ_ಅಂತ್ಯ

  LIBSSH2_AGENT *ssh_agent;     /* proxy to ssh-agent/pageant */
  ರಚನೆ libssh2_agent_publickey *sshagent_ತಟಸ್ಥ;
  ರಚನೆ libssh2_agent_publickey *sshagent_prev_ತಟಸ್ಥ;
  LIBSSH2_KNOWNHOSTS *kh;
#ಆದರೆ_ಅಂತ್ಯ /* USE_LIBSSH */
  ಬಿಟ್(authed);                /* the connection has been authenticated fine */
  ಬಿಟ್(acceptfail);            /* used by the SFTP_QUOTE (continue if
                                 quote command fails) */
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH
#ಆದರೆ LIBSSH_ಆವೃತ್ತಿ_INT < SSH_ಆವೃತ್ತಿ_INT(0, 9, 0)
#ದೋಷ "SCP/SFTP protocols require libssh 0.9.0 or greater"
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_LIBSSH2

/* Feature detection based on version numbers to better work with
   non-configure platforms */

#ಆದರೆ !defined(LIBSSH2_ಆವೃತ್ತಿ_ಅಂಕೆ) || (LIBSSH2_ಆವೃತ್ತಿ_ಅಂಕೆ < 0x010900)
#ದೋಷ "SCP/SFTP protocols require libssh2 1.9.0 or greater"
/* 1.9.0 was released on June 20 2019 */
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* USE_LIBSSH2 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_ssh_statename(sshstate state);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssh_statename(x) ""
#ಆದರೆ_ಅಂತ್ಯ
ಶೂನ್ಯ Curl_ssh_ಗಣ_state(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ ssh_conn *sshc,
                        sshstate nowstate);

#ವ್ಯಾಖ್ಯಾನಿಸು myssh_ಗಮ್ಯ(x, y, z) Curl_ssh_ಗಣ_state(x, y, z)

/* generic SSH backend functions */
CURLcode Curl_ssh_init(ಶೂನ್ಯ);
ಶೂನ್ಯ Curl_ssh_cleanup(ಶೂನ್ಯ);
ಶೂನ್ಯ Curl_ssh_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t buflen);
ಶೂನ್ಯ Curl_ssh_attach(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     ರಚನೆ connectdata *conn);
#ಇಲ್ಲದಿದ್ದರೆ /* !USE_SSH */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssh_cleanup()
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssh_attach(x, y)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssh_init() 0
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSH */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SSH_H */
