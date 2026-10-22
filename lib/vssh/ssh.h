#که_نه_تعریف_شوی سرلیک_CURL_VSSH_SSH_H
#تعریف_کړه سرلیک_CURL_VSSH_SSH_H
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
#ضمن "curl_setup.h"
#ضمن "urldata.h"

خارجي ثابت بنية Curl_protocol Curl_protocol_sftp;
خارجي ثابت بنية Curl_protocol Curl_protocol_scp;

#که_تعریف_شوی USE_SSH

#که_تعریف_شوی USE_LIBSSH2
#ضمن <libssh2.h>
#ضمن <libssh2_sftp.h>
#وإلاإذا defined(USE_LIBSSH)
/* in 0.10.0 or later, ignore deprecated warnings */
#تعریف_کړه SSH_SUPPRESS_DEPRECATED
#ضمن <libssh/libssh.h>
#ضمن <libssh/sftp.h>
#د_که_پای

/* meta key for storing protocol meta at easy handle */
#تعریف_کړه CURL_META_SSH_EASY   "meta:proto:ssh:easy"
/* meta key for storing protocol meta at connection */
#تعریف_کړه CURL_META_SSH_CONN   "meta:proto:ssh:conn"

/****************************************************************************
 * SSH unique setup
 ***************************************************************************/
تعريفنوع تعداد {
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
  SSH_AUTH_کلی_INIT,
  SSH_AUTH_کلی,
  SSH_AUTH_GSSAPI,
  SSH_AUTH_بشپړ,
  SSH_SFTP_INIT,
  SSH_SFTP_REALPATH,   /* Last state in SSH-CONNECT */

  SSH_SFTP_QUOTE_INIT, /* First state in SFTP-DO */
  SSH_SFTP_POSTQUOTE_INIT, /* (Possibly) First state in SFTP-DONE */
  SSH_SFTP_QUOTE,
  SSH_SFTP_بل_QUOTE,
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
  SSH_SFTP_READDIR_بشپړ,
  SSH_SFTP_DOWNLOAD_INIT,
  SSH_SFTP_DOWNLOAD_STAT, /* Last state in SFTP-DO */
  SSH_SFTP_CLOSE,    /* Last state in SFTP-DONE */
  SSH_SFTP_SHUTDOWN, /* First state in SFTP-DISCONNECT */
  SSH_SCP_TRANS_INIT, /* First state in SCP-DO */
  SSH_SCP_UPLOAD_INIT,
  SSH_SCP_DOWNLOAD_INIT,
  SSH_SCP_DOWNLOAD,
  SSH_SCP_بشپړ,
  SSH_SCP_SEND_EOF,
  SSH_SCP_WAIT_EOF,
  SSH_SCP_WAIT_CLOSE,
  SSH_SCP_CHANNEL_FREE,   /* Last state in SCP-DONE */
  SSH_SESSION_DISCONNECT, /* First state in SCP-DISCONNECT */
  SSH_SESSION_FREE,       /* Last state in SCP/SFTP-DISCONNECT */
  SSH_QUIT,
  SSH_وروستی  /* never used */
} sshstate;

#تعریف_کړه CURL_لار_MAX 1024

/* this struct is used in the HandleData struct which is part of the
   Curl_easy, which means this is used on a per-easy handle basis.
   Everything that is strictly related to a connection is banned from this
   struct. */
بنية SSHPROTO {
  حرف *لار;        /* the path we operate on, at least one byte long */
#که_تعریف_شوی USE_LIBSSH2
  بنية dynbuf readdir_link;
  بنية dynbuf readdir;
  حرف readdir_filename[CURL_لار_MAX + 1];
  حرف readdir_longentry[CURL_لار_MAX + 1];

  LIBSSH2_SFTP_ځانګړتیاوې quote_attrs; /* used by the SFTP_QUOTE state */

  /* Here's a set of struct members used by the SFTP_READDIR state */
  LIBSSH2_SFTP_ځانګړتیاوې readdir_attrs;
#د_که_پای
};

/* ssh_conn is used for struct connection-oriented data in the connectdata
   struct */
بنية ssh_conn {
  ثابت حرف *authlist;       /* List of auth. methods, managed by libssh2 */

  /* common */
  حرف *passphrase;           /* strdup'ed pass-phrase to use or NULL */
  حرف *pub_کلی;              /* strdup'ed public key file */
  حرف *priv_کلی;             /* strdup'ed private key file */
  sshstate state;             /* always use ssh.c:state() to change state! */
  sshstate nextstate;         /* the state to goto after stopping */
  بنية curl_slist *quote_توکی; /* for the quote option */
  حرف *quote_path1;          /* two generic pointers for the QUOTE stuff */
  حرف *quote_path2;

  حرف *homedir;              /* when doing SFTP we figure out home directory
                                 in the connect phase */
  /* end of READDIR stuff */

  صحيح دوهم_Create_Dirs;         /* counter use by the code to see if the
                                   second attempt has been made to change
                                   to/create a directory */
  صحيح waitfor;                  /* REQ_IO_RECV/REQ_IO_SEND bits overriding
                                   pollset given flags */
  حرف *slash_pos;              /* used by the SFTP_CREATE_DIRS state */

#که_تعریف_شوی USE_LIBSSH
  CURLcode actualcode;        /* the actual error code */
  حرف *readdir_link_لار;
  size_t readdir_len;
  بنية dynbuf readdir_buf;
/* our variables */
  بینښه kbd_state; /* 0 or 1 */
  ssh_کلی privkey;
  ssh_کلی pubkey;
  بینښه صحيح auth_methods;
  ssh_session ssh_session;
  ssh_scp scp_session;
  sftp_session sftp_session;
  sftp_file sftp_file;
  sftp_dir sftp_dir;

  بینښه sftp_recv_state; /* 0 or 1 */
#إذا LIBSSH_نسخه_INT > SSH_نسخه_INT(0, 11, 0)
  sftp_aio sftp_recv_aio;

  sftp_aio sftp_send_aio;
  بینښه sftp_send_state; /* 0 or 1 */
#وإلا
  صحيح sftp_file_شاخص; /* for async read */
#د_که_پای
  sftp_ځانګړتیاوې readdir_attrs; /* used by the SFTP readdir actions */
  sftp_ځانګړتیاوې readdir_link_attrs; /* used by the SFTP readdir actions */
  sftp_ځانګړتیاوې quote_attrs; /* used by the SFTP_QUOTE state */

  ثابت حرف *readdir_filename; /* points within readdir_attrs */
  ثابت حرف *readdir_longentry;
  حرف *readdir_tmp;
  بټ(initialized);
#وإلاإذا defined(USE_LIBSSH2)
  LIBSSH2_SESSION *ssh_session; /* Secure Shell session */
  LIBSSH2_CHANNEL *ssh_channel; /* Secure Shell channel handle */
  LIBSSH2_SFTP *sftp_session;   /* SFTP handle */
  LIBSSH2_SFTP_HANDLE *sftp_handle;

#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  /* for HTTPS proxy storage */
  Curl_recv *tls_recv;
  Curl_send *tls_send;
#د_که_پای

  LIBSSH2_AGENT *ssh_agent;     /* proxy to ssh-agent/pageant */
  بنية libssh2_agent_publickey *sshagent_بې_اغېزه_عنصر;
  بنية libssh2_agent_publickey *sshagent_prev_بې_اغېزه_عنصر;
  LIBSSH2_KNOWNHOSTS *kh;
#د_که_پای /* USE_LIBSSH */
  بټ(authed);                /* the connection has been authenticated fine */
  بټ(acceptfail);            /* used by the SFTP_QUOTE (continue if
                                 quote command fails) */
};

#که_تعریف_شوی USE_LIBSSH
#إذا LIBSSH_نسخه_INT < SSH_نسخه_INT(0, 9, 0)
#تېروتنه "SCP/SFTP protocols require libssh 0.9.0 or greater"
#د_که_پای
#د_که_پای

#که_تعریف_شوی USE_LIBSSH2

/* Feature detection based on version numbers to better work with
   non-configure platforms */

#إذا !defined(LIBSSH2_نسخه_عدد) || (LIBSSH2_نسخه_عدد < 0x010900)
#تېروتنه "SCP/SFTP protocols require libssh2 1.9.0 or greater"
/* 1.9.0 was released on June 20 2019 */
#د_که_پای

#د_که_پای /* USE_LIBSSH2 */

#که_تعریف_شوی CURLVERBOSE
ثابت حرف *Curl_ssh_statename(sshstate state);
#وإلا
#تعریف_کړه Curl_ssh_statename(x) ""
#د_که_پای
فراغ Curl_ssh_سېټ_state(بنية Curl_easy *معلومات,
                        بنية ssh_conn *sshc,
                        sshstate nowstate);

#تعریف_کړه myssh_منزل(x, y, z) Curl_ssh_سېټ_state(x, y, z)

/* generic SSH backend functions */
CURLcode Curl_ssh_init(فراغ);
فراغ Curl_ssh_cleanup(فراغ);
فراغ Curl_ssh_نسخه(حرف *بفر, size_t buflen);
فراغ Curl_ssh_attach(بنية Curl_easy *معلومات,
                     بنية connectdata *conn);
#وإلا /* !USE_SSH */
#تعریف_کړه Curl_ssh_cleanup()
#تعریف_کړه Curl_ssh_attach(x, y)
#تعریف_کړه Curl_ssh_init() 0
#د_که_پای /* USE_SSH */

#د_که_پای /* HEADER_CURL_SSH_H */
