#यदि_अपरिभाषितम् शीर्षकम्_CURL_VSSH_SSH_H
#परिभाषय शीर्षकम्_CURL_VSSH_SSH_H
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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"

बाह्य ध्रुव संरचना Curl_protocol Curl_protocol_sftp;
बाह्य ध्रुव संरचना Curl_protocol Curl_protocol_scp;

#यदि_परिभाषितम् USE_SSH

#यदि_परिभाषितम् USE_LIBSSH2
#अन्तर्भाव <libssh2.h>
#अन्तर्भाव <libssh2_sftp.h>
#नोचेद्यदि defined(USE_LIBSSH)
/* in 0.10.0 or later, ignore deprecated warnings */
#परिभाषय SSH_SUPPRESS_DEPRECATED
#अन्तर्भाव <libssh/libssh.h>
#अन्तर्भाव <libssh/sftp.h>
#यदि_अन्तः

/* meta key for storing protocol meta at easy handle */
#परिभाषय CURL_META_SSH_EASY   "meta:proto:ssh:easy"
/* meta key for storing protocol meta at connection */
#परिभाषय CURL_META_SSH_CONN   "meta:proto:ssh:conn"

/****************************************************************************
 * SSH unique setup
 ***************************************************************************/
प्रकारनिर्देश गणना {
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
  SSH_AUTH_कुञ्चिका_INIT,
  SSH_AUTH_कुञ्चिका,
  SSH_AUTH_GSSAPI,
  SSH_AUTH_समाप्तम्,
  SSH_SFTP_INIT,
  SSH_SFTP_REALPATH,   /* Last state in SSH-CONNECT */

  SSH_SFTP_QUOTE_INIT, /* First state in SFTP-DO */
  SSH_SFTP_POSTQUOTE_INIT, /* (Possibly) First state in SFTP-DONE */
  SSH_SFTP_QUOTE,
  SSH_SFTP_अग्र_QUOTE,
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
  SSH_SFTP_READDIR_समाप्तम्,
  SSH_SFTP_DOWNLOAD_INIT,
  SSH_SFTP_DOWNLOAD_STAT, /* Last state in SFTP-DO */
  SSH_SFTP_CLOSE,    /* Last state in SFTP-DONE */
  SSH_SFTP_SHUTDOWN, /* First state in SFTP-DISCONNECT */
  SSH_SCP_TRANS_INIT, /* First state in SCP-DO */
  SSH_SCP_UPLOAD_INIT,
  SSH_SCP_DOWNLOAD_INIT,
  SSH_SCP_DOWNLOAD,
  SSH_SCP_समाप्तम्,
  SSH_SCP_SEND_EOF,
  SSH_SCP_WAIT_EOF,
  SSH_SCP_WAIT_CLOSE,
  SSH_SCP_CHANNEL_FREE,   /* Last state in SCP-DONE */
  SSH_SESSION_DISCONNECT, /* First state in SCP-DISCONNECT */
  SSH_SESSION_FREE,       /* Last state in SCP/SFTP-DISCONNECT */
  SSH_QUIT,
  SSH_चरमः  /* never used */
} sshstate;

#परिभाषय CURL_मार्गः_MAX 1024

/* this struct is used in the HandleData struct which is part of the
   Curl_easy, which means this is used on a per-easy handle basis.
   Everything that is strictly related to a connection is banned from this
   struct. */
संरचना SSHPROTO {
  वर्ण *मार्गः;        /* the path we operate on, at least one byte long */
#यदि_परिभाषितम् USE_LIBSSH2
  संरचना dynbuf readdir_link;
  संरचना dynbuf readdir;
  वर्ण readdir_filename[CURL_मार्गः_MAX + 1];
  वर्ण readdir_longentry[CURL_मार्गः_MAX + 1];

  LIBSSH2_SFTP_गुणाः quote_attrs; /* used by the SFTP_QUOTE state */

  /* Here's a set of struct members used by the SFTP_READDIR state */
  LIBSSH2_SFTP_गुणाः readdir_attrs;
#यदि_अन्तः
};

/* ssh_conn is used for struct connection-oriented data in the connectdata
   struct */
संरचना ssh_conn {
  ध्रुव वर्ण *authlist;       /* List of auth. methods, managed by libssh2 */

  /* common */
  वर्ण *passphrase;           /* strdup'ed pass-phrase to use or NULL */
  वर्ण *pub_कुञ्चिका;              /* strdup'ed public key file */
  वर्ण *priv_कुञ्चिका;             /* strdup'ed private key file */
  sshstate state;             /* always use ssh.c:state() to change state! */
  sshstate nextstate;         /* the state to goto after stopping */
  संरचना curl_slist *quote_item; /* for the quote option */
  वर्ण *quote_path1;          /* two generic pointers for the QUOTE stuff */
  वर्ण *quote_path2;

  वर्ण *homedir;              /* when doing SFTP we figure out home directory
                                 in the connect phase */
  /* end of READDIR stuff */

  पूर्णाङ्क secondCreateDirs;         /* counter use by the code to see if the
                                   second attempt has been made to change
                                   to/create a directory */
  पूर्णाङ्क waitfor;                  /* REQ_IO_RECV/REQ_IO_SEND bits overriding
                                   pollset given flags */
  वर्ण *slash_pos;              /* used by the SFTP_CREATE_DIRS state */

#यदि_परिभाषितम् USE_LIBSSH
  CURLcode actualcode;        /* the actual error code */
  वर्ण *readdir_link_मार्गः;
  size_t readdir_len;
  संरचना dynbuf readdir_buf;
/* our variables */
  अचिह्नित kbd_state; /* 0 or 1 */
  ssh_कुञ्चिका privkey;
  ssh_कुञ्चिका pubkey;
  अचिह्नित पूर्णाङ्क auth_methods;
  ssh_session ssh_session;
  ssh_scp scp_session;
  sftp_session sftp_session;
  sftp_file sftp_file;
  sftp_dir sftp_dir;

  अचिह्नित sftp_recv_state; /* 0 or 1 */
#यदि LIBSSH_संस्करणम्_INT > SSH_संस्करणम्_INT(0, 11, 0)
  sftp_aio sftp_recv_aio;

  sftp_aio sftp_send_aio;
  अचिह्नित sftp_send_state; /* 0 or 1 */
#अन्यथा
  पूर्णाङ्क sftp_file_सूचकांक; /* for async read */
#यदि_अन्तः
  sftp_गुणाः readdir_attrs; /* used by the SFTP readdir actions */
  sftp_गुणाः readdir_link_attrs; /* used by the SFTP readdir actions */
  sftp_गुणाः quote_attrs; /* used by the SFTP_QUOTE state */

  ध्रुव वर्ण *readdir_filename; /* points within readdir_attrs */
  ध्रुव वर्ण *readdir_longentry;
  वर्ण *readdir_tmp;
  BIT(initialized);
#नोचेद्यदि defined(USE_LIBSSH2)
  LIBSSH2_SESSION *ssh_session; /* Secure Shell session */
  LIBSSH2_CHANNEL *ssh_channel; /* Secure Shell channel handle */
  LIBSSH2_SFTP *sftp_session;   /* SFTP handle */
  LIBSSH2_SFTP_HANDLE *sftp_handle;

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  /* for HTTPS proxy storage */
  Curl_recv *tls_recv;
  Curl_send *tls_send;
#यदि_अन्तः

  LIBSSH2_AGENT *ssh_agent;     /* proxy to ssh-agent/pageant */
  संरचना libssh2_agent_publickey *sshagent_तटस्थः;
  संरचना libssh2_agent_publickey *sshagent_पूर्व_तटस्थः;
  LIBSSH2_KNOWNHOSTS *kh;
#यदि_अन्तः /* USE_LIBSSH */
  BIT(authed);                /* the connection has been authenticated fine */
  BIT(acceptfail);            /* used by the SFTP_QUOTE (continue if
                                 quote command fails) */
};

#यदि_परिभाषितम् USE_LIBSSH
#यदि LIBSSH_संस्करणम्_INT < SSH_संस्करणम्_INT(0, 9, 0)
#दोषः "SCP/SFTP protocols require libssh 0.9.0 or greater"
#यदि_अन्तः
#यदि_अन्तः

#यदि_परिभाषितम् USE_LIBSSH2

/* Feature detection based on version numbers to better work with
   non-configure platforms */

#यदि !defined(LIBSSH2_संस्करणम्_अङ्कः) || (LIBSSH2_संस्करणम्_अङ्कः < 0x010900)
#दोषः "SCP/SFTP protocols require libssh2 1.9.0 or greater"
/* 1.9.0 was released on June 20 2019 */
#यदि_अन्तः

#यदि_अन्तः /* USE_LIBSSH2 */

#यदि_परिभाषितम् CURLVERBOSE
ध्रुव वर्ण *Curl_ssh_statename(sshstate state);
#अन्यथा
#परिभाषय Curl_ssh_statename(x) ""
#यदि_अन्तः
शून्य Curl_ssh_समुच्चयः_state(संरचना Curl_easy *दत्त,
                        संरचना ssh_conn *sshc,
                        sshstate nowstate);

#परिभाषय myssh_गन्तव्यम्(x, y, z) Curl_ssh_समुच्चयः_state(x, y, z)

/* generic SSH backend functions */
CURLcode Curl_ssh_init(शून्य);
शून्य Curl_ssh_cleanup(शून्य);
शून्य Curl_ssh_संस्करणम्(वर्ण *बफर, size_t buflen);
शून्य Curl_ssh_attach(संरचना Curl_easy *दत्त,
                     संरचना connectdata *conn);
#अन्यथा /* !USE_SSH */
#परिभाषय Curl_ssh_cleanup()
#परिभाषय Curl_ssh_attach(x, y)
#परिभाषय Curl_ssh_init() 0
#यदि_अन्तः /* USE_SSH */

#यदि_अन्तः /* HEADER_CURL_SSH_H */
