#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VSSH_SSH_H
#నిర్వచించు శీర్షిక_CURL_VSSH_SSH_H
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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"

బాహ్య స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_sftp;
బాహ్య స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_scp;

#నిర్వచితమైతే USE_SSH

#నిర్వచితమైతే USE_LIBSSH2
#అన్తర్భావ <libssh2.h>
#అన్తర్భావ <libssh2_sftp.h>
#లేదావేళ defined(USE_LIBSSH)
/* in 0.10.0 or later, ignore deprecated warnings */
#నిర్వచించు SSH_SUPPRESS_DEPRECATED
#అన్తర్భావ <libssh/libssh.h>
#అన్తర్భావ <libssh/sftp.h>
#అయితే_ముగింపు

/* meta key for storing protocol meta at easy handle */
#నిర్వచించు CURL_META_SSH_EASY   "meta:proto:ssh:easy"
/* meta key for storing protocol meta at connection */
#నిర్వచించు CURL_META_SSH_CONN   "meta:proto:ssh:conn"

/****************************************************************************
 * SSH unique setup
 ***************************************************************************/
రకనిర్వచనం గణన {
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
  SSH_AUTH_కీలకం_INIT,
  SSH_AUTH_కీలకం,
  SSH_AUTH_GSSAPI,
  SSH_AUTH_పూర్తి,
  SSH_SFTP_INIT,
  SSH_SFTP_REALPATH,   /* Last state in SSH-CONNECT */

  SSH_SFTP_QUOTE_INIT, /* First state in SFTP-DO */
  SSH_SFTP_POSTQUOTE_INIT, /* (Possibly) First state in SFTP-DONE */
  SSH_SFTP_QUOTE,
  SSH_SFTP_తదుపరి_QUOTE,
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
  SSH_SFTP_READDIR_పూర్తి,
  SSH_SFTP_DOWNLOAD_INIT,
  SSH_SFTP_DOWNLOAD_STAT, /* Last state in SFTP-DO */
  SSH_SFTP_CLOSE,    /* Last state in SFTP-DONE */
  SSH_SFTP_SHUTDOWN, /* First state in SFTP-DISCONNECT */
  SSH_SCP_TRANS_INIT, /* First state in SCP-DO */
  SSH_SCP_UPLOAD_INIT,
  SSH_SCP_DOWNLOAD_INIT,
  SSH_SCP_DOWNLOAD,
  SSH_SCP_పూర్తి,
  SSH_SCP_SEND_EOF,
  SSH_SCP_WAIT_EOF,
  SSH_SCP_WAIT_CLOSE,
  SSH_SCP_CHANNEL_FREE,   /* Last state in SCP-DONE */
  SSH_SESSION_DISCONNECT, /* First state in SCP-DISCONNECT */
  SSH_SESSION_FREE,       /* Last state in SCP/SFTP-DISCONNECT */
  SSH_QUIT,
  SSH_చివరి  /* never used */
} sshstate;

#నిర్వచించు CURL_మార్గం_MAX 1024

/* this struct is used in the HandleData struct which is part of the
   Curl_easy, which means this is used on a per-easy handle basis.
   Everything that is strictly related to a connection is banned from this
   struct. */
నిర్మాణం SSHPROTO {
  అక్షరం *మార్గం;        /* the path we operate on, at least one byte long */
#నిర్వచితమైతే USE_LIBSSH2
  నిర్మాణం dynbuf readdir_link;
  నిర్మాణం dynbuf readdir;
  అక్షరం readdir_filename[CURL_మార్గం_MAX + 1];
  అక్షరం readdir_longentry[CURL_మార్గం_MAX + 1];

  LIBSSH2_SFTP_లక్షణాలు quote_attrs; /* used by the SFTP_QUOTE state */

  /* Here's a set of struct members used by the SFTP_READDIR state */
  LIBSSH2_SFTP_లక్షణాలు readdir_attrs;
#అయితే_ముగింపు
};

/* ssh_conn is used for struct connection-oriented data in the connectdata
   struct */
నిర్మాణం ssh_conn {
  స్థిరరాశి అక్షరం *authlist;       /* List of auth. methods, managed by libssh2 */

  /* common */
  అక్షరం *passphrase;           /* strdup'ed pass-phrase to use or NULL */
  అక్షరం *pub_కీలకం;              /* strdup'ed public key file */
  అక్షరం *priv_కీలకం;             /* strdup'ed private key file */
  sshstate state;             /* always use ssh.c:state() to change state! */
  sshstate nextstate;         /* the state to goto after stopping */
  నిర్మాణం curl_slist *quote_అంశం; /* for the quote option */
  అక్షరం *quote_path1;          /* two generic pointers for the QUOTE stuff */
  అక్షరం *quote_path2;

  అక్షరం *homedir;              /* when doing SFTP we figure out home directory
                                 in the connect phase */
  /* end of READDIR stuff */

  పూర్ణాంకం రెండవ_Create_Dirs;         /* counter use by the code to see if the
                                   second attempt has been made to change
                                   to/create a directory */
  పూర్ణాంకం waitfor;                  /* REQ_IO_RECV/REQ_IO_SEND bits overriding
                                   pollset given flags */
  అక్షరం *slash_pos;              /* used by the SFTP_CREATE_DIRS state */

#నిర్వచితమైతే USE_LIBSSH
  CURLcode actualcode;        /* the actual error code */
  అక్షరం *readdir_link_మార్గం;
  size_t readdir_len;
  నిర్మాణం dynbuf readdir_buf;
/* our variables */
  గుర్తులేని kbd_state; /* 0 or 1 */
  ssh_కీలకం privkey;
  ssh_కీలకం pubkey;
  గుర్తులేని పూర్ణాంకం auth_methods;
  ssh_session ssh_session;
  ssh_scp scp_session;
  sftp_session sftp_session;
  sftp_file sftp_file;
  sftp_dir sftp_dir;

  గుర్తులేని sftp_recv_state; /* 0 or 1 */
#ఒకవేళ LIBSSH_సంస్కరణ_INT > SSH_సంస్కరణ_INT(0, 11, 0)
  sftp_aio sftp_recv_aio;

  sftp_aio sftp_send_aio;
  గుర్తులేని sftp_send_state; /* 0 or 1 */
#లేకపోతే
  పూర్ణాంకం sftp_file_సూచీ; /* for async read */
#అయితే_ముగింపు
  sftp_లక్షణాలు readdir_attrs; /* used by the SFTP readdir actions */
  sftp_లక్షణాలు readdir_link_attrs; /* used by the SFTP readdir actions */
  sftp_లక్షణాలు quote_attrs; /* used by the SFTP_QUOTE state */

  స్థిరరాశి అక్షరం *readdir_filename; /* points within readdir_attrs */
  స్థిరరాశి అక్షరం *readdir_longentry;
  అక్షరం *readdir_tmp;
  బిట్టు(initialized);
#లేదావేళ defined(USE_LIBSSH2)
  LIBSSH2_SESSION *ssh_session; /* Secure Shell session */
  LIBSSH2_CHANNEL *ssh_channel; /* Secure Shell channel handle */
  LIBSSH2_SFTP *sftp_session;   /* SFTP handle */
  LIBSSH2_SFTP_HANDLE *sftp_handle;

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  /* for HTTPS proxy storage */
  Curl_recv *tls_recv;
  Curl_send *tls_send;
#అయితే_ముగింపు

  LIBSSH2_AGENT *ssh_agent;     /* proxy to ssh-agent/pageant */
  నిర్మాణం libssh2_agent_publickey *sshagent_తటస్థం;
  నిర్మాణం libssh2_agent_publickey *sshagent_prev_తటస్థం;
  LIBSSH2_KNOWNHOSTS *kh;
#అయితే_ముగింపు /* USE_LIBSSH */
  బిట్టు(authed);                /* the connection has been authenticated fine */
  బిట్టు(acceptfail);            /* used by the SFTP_QUOTE (continue if
                                 quote command fails) */
};

#నిర్వచితమైతే USE_LIBSSH
#ఒకవేళ LIBSSH_సంస్కరణ_INT < SSH_సంస్కరణ_INT(0, 9, 0)
#దోషం "SCP/SFTP protocols require libssh 0.9.0 or greater"
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితమైతే USE_LIBSSH2

/* Feature detection based on version numbers to better work with
   non-configure platforms */

#ఒకవేళ !defined(LIBSSH2_సంస్కరణ_అంకె) || (LIBSSH2_సంస్కరణ_అంకె < 0x010900)
#దోషం "SCP/SFTP protocols require libssh2 1.9.0 or greater"
/* 1.9.0 was released on June 20 2019 */
#అయితే_ముగింపు

#అయితే_ముగింపు /* USE_LIBSSH2 */

#నిర్వచితమైతే CURLVERBOSE
స్థిరరాశి అక్షరం *Curl_ssh_statename(sshstate state);
#లేకపోతే
#నిర్వచించు Curl_ssh_statename(x) ""
#అయితే_ముగింపు
శూన్య Curl_ssh_సమితి_state(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం ssh_conn *sshc,
                        sshstate nowstate);

#నిర్వచించు myssh_గమ్యం(x, y, z) Curl_ssh_సమితి_state(x, y, z)

/* generic SSH backend functions */
CURLcode Curl_ssh_init(శూన్య);
శూన్య Curl_ssh_cleanup(శూన్య);
శూన్య Curl_ssh_సంస్కరణ(అక్షరం *బఫరు, size_t buflen);
శూన్య Curl_ssh_attach(నిర్మాణం Curl_easy *దత్తాంశం,
                     నిర్మాణం connectdata *conn);
#లేకపోతే /* !USE_SSH */
#నిర్వచించు Curl_ssh_cleanup()
#నిర్వచించు Curl_ssh_attach(x, y)
#నిర్వచించు Curl_ssh_init() 0
#అయితే_ముగింపు /* USE_SSH */

#అయితే_ముగింపు /* HEADER_CURL_SSH_H */
