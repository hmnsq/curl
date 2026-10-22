#ifndef HEADER_CURL_VSSH_SSH_H
#define HEADER_CURL_VSSH_SSH_H
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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"

বাহ্য ধ্রুব সংরচনা Curl_protocol Curl_protocol_sftp;
বাহ্য ধ্রুব সংরচনা Curl_protocol Curl_protocol_scp;

#ifdef USE_SSH

#ifdef USE_LIBSSH2
#অন্তর্ভাব <libssh2.h>
#অন্তর্ভাব <libssh2_sftp.h>
#নোচেদ্যদি defined(USE_LIBSSH)
/* in 0.10.0 or later, ignore deprecated warnings */
#define SSH_SUPPRESS_DEPRECATED
#অন্তর্ভাব <libssh/libssh.h>
#অন্তর্ভাব <libssh/sftp.h>
#endif

/* meta key for storing protocol meta at easy handle */
#define CURL_META_SSH_EASY   "meta:proto:ssh:easy"
/* meta key for storing protocol meta at connection */
#define CURL_META_SSH_CONN   "meta:proto:ssh:conn"

/****************************************************************************
 * SSH unique setup
 ***************************************************************************/
প্রকারনির্দেশ গণনা {
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
  SSH_AUTH_চাবি_INIT,
  SSH_AUTH_চাবি,
  SSH_AUTH_GSSAPI,
  SSH_AUTH_DONE,
  SSH_SFTP_INIT,
  SSH_SFTP_REALPATH,   /* Last state in SSH-CONNECT */

  SSH_SFTP_QUOTE_INIT, /* First state in SFTP-DO */
  SSH_SFTP_POSTQUOTE_INIT, /* (Possibly) First state in SFTP-DONE */
  SSH_SFTP_QUOTE,
  SSH_SFTP_NEXT_QUOTE,
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
  SSH_SFTP_READDIR_DONE,
  SSH_SFTP_DOWNLOAD_INIT,
  SSH_SFTP_DOWNLOAD_STAT, /* Last state in SFTP-DO */
  SSH_SFTP_CLOSE,    /* Last state in SFTP-DONE */
  SSH_SFTP_SHUTDOWN, /* First state in SFTP-DISCONNECT */
  SSH_SCP_TRANS_INIT, /* First state in SCP-DO */
  SSH_SCP_UPLOAD_INIT,
  SSH_SCP_DOWNLOAD_INIT,
  SSH_SCP_DOWNLOAD,
  SSH_SCP_DONE,
  SSH_SCP_SEND_EOF,
  SSH_SCP_WAIT_EOF,
  SSH_SCP_WAIT_CLOSE,
  SSH_SCP_CHANNEL_FREE,   /* Last state in SCP-DONE */
  SSH_SESSION_DISCONNECT, /* First state in SCP-DISCONNECT */
  SSH_SESSION_FREE,       /* Last state in SCP/SFTP-DISCONNECT */
  SSH_QUIT,
  SSH_LAST  /* never used */
} sshstate;

#define CURL_PATH_MAX 1024

/* this struct is used in the HandleData struct which is part of the
   Curl_easy, which means this is used on a per-easy handle basis.
   Everything that is strictly related to a connection is banned from this
   struct. */
সংরচনা SSHPROTO {
  বর্ণ *path;        /* the path we operate on, at least one byte long */
#ifdef USE_LIBSSH2
  সংরচনা dynbuf readdir_link;
  সংরচনা dynbuf readdir;
  বর্ণ readdir_filename[CURL_PATH_MAX + 1];
  বর্ণ readdir_longentry[CURL_PATH_MAX + 1];

  LIBSSH2_SFTP_ATTRIBUTES quote_attrs; /* used by the SFTP_QUOTE state */

  /* Here's a set of struct members used by the SFTP_READDIR state */
  LIBSSH2_SFTP_ATTRIBUTES readdir_attrs;
#endif
};

/* ssh_conn is used for struct connection-oriented data in the connectdata
   struct */
সংরচনা ssh_conn {
  ধ্রুব বর্ণ *authlist;       /* List of auth. methods, managed by libssh2 */

  /* common */
  বর্ণ *passphrase;           /* strdup'ed pass-phrase to use or NULL */
  বর্ণ *pub_চাবি;              /* strdup'ed public key file */
  বর্ণ *priv_চাবি;             /* strdup'ed private key file */
  sshstate state;             /* always use ssh.c:state() to change state! */
  sshstate nextstate;         /* the state to goto after stopping */
  সংরচনা curl_slist *quote_item; /* for the quote option */
  বর্ণ *quote_path1;          /* two generic pointers for the QUOTE stuff */
  বর্ণ *quote_path2;

  বর্ণ *homedir;              /* when doing SFTP we figure out home directory
                                 in the connect phase */
  /* end of READDIR stuff */

  পূর্ণাঙ্ক secondCreateDirs;         /* counter use by the code to see if the
                                   second attempt has been made to change
                                   to/create a directory */
  পূর্ণাঙ্ক waitfor;                  /* REQ_IO_RECV/REQ_IO_SEND bits overriding
                                   pollset given flags */
  বর্ণ *slash_pos;              /* used by the SFTP_CREATE_DIRS state */

#ifdef USE_LIBSSH
  CURLcode actualcode;        /* the actual error code */
  বর্ণ *readdir_linkPath;
  size_t readdir_len;
  সংরচনা dynbuf readdir_buf;
/* our variables */
  unsigned kbd_state; /* 0 or 1 */
  ssh_চাবি privkey;
  ssh_চাবি pubkey;
  unsigned পূর্ণাঙ্ক auth_methods;
  ssh_session ssh_session;
  ssh_scp scp_session;
  sftp_session sftp_session;
  sftp_file sftp_file;
  sftp_dir sftp_dir;

  unsigned sftp_recv_state; /* 0 or 1 */
#যদি LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sftp_aio sftp_recv_aio;

  sftp_aio sftp_send_aio;
  unsigned sftp_send_state; /* 0 or 1 */
#অন্যথা
  পূর্ণাঙ্ক sftp_file_index; /* for async read */
#endif
  sftp_attributes readdir_attrs; /* used by the SFTP readdir actions */
  sftp_attributes readdir_link_attrs; /* used by the SFTP readdir actions */
  sftp_attributes quote_attrs; /* used by the SFTP_QUOTE state */

  ধ্রুব বর্ণ *readdir_filename; /* points within readdir_attrs */
  ধ্রুব বর্ণ *readdir_longentry;
  বর্ণ *readdir_tmp;
  BIT(initialized);
#নোচেদ্যদি defined(USE_LIBSSH2)
  LIBSSH2_SESSION *ssh_session; /* Secure Shell session */
  LIBSSH2_CHANNEL *ssh_channel; /* Secure Shell channel handle */
  LIBSSH2_SFTP *sftp_session;   /* SFTP handle */
  LIBSSH2_SFTP_HANDLE *sftp_handle;

#ifndef CURL_DISABLE_PROXY
  /* for HTTPS proxy storage */
  Curl_recv *tls_recv;
  Curl_send *tls_send;
#endif

  LIBSSH2_AGENT *ssh_agent;     /* proxy to ssh-agent/pageant */
  সংরচনা libssh2_agent_publickey *sshagent_identity;
  সংরচনা libssh2_agent_publickey *sshagent_prev_identity;
  LIBSSH2_KNOWNHOSTS *kh;
#endif /* USE_LIBSSH */
  BIT(authed);                /* the connection has been authenticated fine */
  BIT(acceptfail);            /* used by the SFTP_QUOTE (continue if
                                 quote command fails) */
};

#ifdef USE_LIBSSH
#যদি LIBSSH_VERSION_INT < SSH_VERSION_INT(0, 9, 0)
#error "SCP/SFTP protocols require libssh 0.9.0 or greater"
#endif
#endif

#ifdef USE_LIBSSH2

/* Feature detection based on version numbers to better work with
   non-configure platforms */

#যদি !defined(LIBSSH2_VERSION_অঙ্ক) || (LIBSSH2_VERSION_অঙ্ক < 0x010900)
#error "SCP/SFTP protocols require libssh2 1.9.0 or greater"
/* 1.9.0 was released on June 20 2019 */
#endif

#endif /* USE_LIBSSH2 */

#ifdef CURLVERBOSE
ধ্রুব বর্ণ *Curl_ssh_statename(sshstate state);
#অন্যথা
#define Curl_ssh_statename(x) ""
#endif
শূন্য Curl_ssh_set_state(সংরচনা Curl_easy *data,
                        সংরচনা ssh_conn *sshc,
                        sshstate nowstate);

#define myssh_to(x, y, z) Curl_ssh_set_state(x, y, z)

/* generic SSH backend functions */
CURLcode Curl_ssh_init(শূন্য);
শূন্য Curl_ssh_cleanup(শূন্য);
শূন্য Curl_ssh_version(বর্ণ *buffer, size_t buflen);
শূন্য Curl_ssh_attach(সংরচনা Curl_easy *data,
                     সংরচনা connectdata *conn);
#অন্যথা /* !USE_SSH */
#define Curl_ssh_cleanup()
#define Curl_ssh_attach(x, y)
#define Curl_ssh_init() 0
#endif /* USE_SSH */

#endif /* HEADER_CURL_SSH_H */
