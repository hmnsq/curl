#अगर_अघोषित HEADER_CURL_VSSH_SSH_H
#घोषणा HEADER_CURL_VSSH_SSH_H
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
#समावेश "curl_setup.h"
#समावेश "urldata.h"

बाह्य स्थिर काष्ठा Curl_protocol Curl_protocol_sftp;
बाह्य स्थिर काष्ठा Curl_protocol Curl_protocol_scp;

#अगर_घोषित USE_SSH

#अगर_घोषित USE_LIBSSH2
#समावेश <libssh2.h>
#समावेश <libssh2_sftp.h>
#या_अगर defined(USE_LIBSSH)
/* in 0.10.0 or later, ignore deprecated warnings */
#घोषणा SSH_SUPPRESS_DEPRECATED
#समावेश <libssh/libssh.h>
#समावेश <libssh/sftp.h>
#पूर्ण_अगर

/* meta key for storing protocol meta at easy handle */
#घोषणा CURL_META_SSH_EASY   "meta:proto:ssh:easy"
/* meta key for storing protocol meta at connection */
#घोषणा CURL_META_SSH_CONN   "meta:proto:ssh:conn"

/****************************************************************************
 * SSH unique setup
 ***************************************************************************/
प्रकार क्रमागत {
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
  SSH_AUTH_कुंजी_INIT,
  SSH_AUTH_कुंजी,
  SSH_AUTH_GSSAPI,
  SSH_AUTH_DONE,
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

#घोषणा CURL_PATH_MAX 1024

/* this struct is used in the HandleData struct which is part of the
   Curl_easy, which means this is used on a per-easy handle basis.
   Everything that is strictly related to a connection is banned from this
   struct. */
काष्ठा SSHPROTO {
  अक्षर *path;        /* the path we operate on, at least one byte long */
#अगर_घोषित USE_LIBSSH2
  काष्ठा dynbuf readdir_link;
  काष्ठा dynbuf सूची_पढ़ो;
  अक्षर readdir_filename[CURL_PATH_MAX + 1];
  अक्षर readdir_longentry[CURL_PATH_MAX + 1];

  LIBSSH2_SFTP_ATTRIBUTES quote_attrs; /* used by the SFTP_QUOTE state */

  /* Here's a set of struct members used by the SFTP_READDIR state */
  LIBSSH2_SFTP_ATTRIBUTES readdir_attrs;
#पूर्ण_अगर
};

/* ssh_conn is used for struct connection-oriented data in the connectdata
   struct */
काष्ठा ssh_conn {
  स्थिर अक्षर *authlist;       /* List of auth. methods, managed by libssh2 */

  /* common */
  अक्षर *passphrase;           /* strdup'ed pass-phrase to use or NULL */
  अक्षर *pub_कुंजी;              /* strdup'ed public key file */
  अक्षर *priv_कुंजी;             /* strdup'ed private key file */
  sshstate state;             /* always use ssh.c:state() to change state! */
  sshstate nextstate;         /* the state to goto after stopping */
  काष्ठा curl_slist *quote_item; /* for the quote option */
  अक्षर *quote_path1;          /* two generic pointers for the QUOTE stuff */
  अक्षर *quote_path2;

  अक्षर *homedir;              /* when doing SFTP we figure out home directory
                                 in the connect phase */
  /* end of READDIR stuff */

  पूर्णांक secondCreateDirs;         /* counter use by the code to see if the
                                   second attempt has been made to change
                                   to/create a directory */
  पूर्णांक waitfor;                  /* REQ_IO_RECV/REQ_IO_SEND bits overriding
                                   pollset given flags */
  अक्षर *slash_pos;              /* used by the SFTP_CREATE_DIRS state */

#अगर_घोषित USE_LIBSSH
  CURLcode actualcode;        /* the actual error code */
  अक्षर *readdir_linkPath;
  माप_प्रकार readdir_len;
  काष्ठा dynbuf readdir_buf;
/* our variables */
  अचिन्हित kbd_state; /* 0 or 1 */
  ssh_कुंजी privkey;
  ssh_कुंजी pubkey;
  अचिन्हित पूर्णांक auth_methods;
  ssh_session ssh_session;
  ssh_scp scp_session;
  sftp_session sftp_session;
  sftp_file sftp_file;
  sftp_dir sftp_dir;

  अचिन्हित sftp_recv_state; /* 0 or 1 */
#अगर LIBSSH_VERSION_INT > SSH_VERSION_INT(0, 11, 0)
  sftp_aio sftp_recv_aio;

  sftp_aio sftp_send_aio;
  अचिन्हित sftp_send_state; /* 0 or 1 */
#अन्यथा
  पूर्णांक sftp_file_सूचकांक; /* for async read */
#पूर्ण_अगर
  sftp_attributes readdir_attrs; /* used by the SFTP readdir actions */
  sftp_attributes readdir_link_attrs; /* used by the SFTP readdir actions */
  sftp_attributes quote_attrs; /* used by the SFTP_QUOTE state */

  स्थिर अक्षर *readdir_filename; /* points within readdir_attrs */
  स्थिर अक्षर *readdir_longentry;
  अक्षर *readdir_tmp;
  BIT(initialized);
#या_अगर defined(USE_LIBSSH2)
  LIBSSH2_SESSION *ssh_session; /* Secure Shell session */
  LIBSSH2_CHANNEL *ssh_channel; /* Secure Shell channel handle */
  LIBSSH2_SFTP *sftp_session;   /* SFTP handle */
  LIBSSH2_SFTP_HANDLE *sftp_handle;

#अगर_अघोषित CURL_DISABLE_PROXY
  /* for HTTPS proxy storage */
  Curl_recv *tls_recv;
  Curl_send *tls_send;
#पूर्ण_अगर

  LIBSSH2_AGENT *ssh_agent;     /* proxy to ssh-agent/pageant */
  काष्ठा libssh2_agent_publickey *sshagent_identity;
  काष्ठा libssh2_agent_publickey *sshagent_पूर्व_identity;
  LIBSSH2_KNOWNHOSTS *kh;
#पूर्ण_अगर /* USE_LIBSSH */
  BIT(authed);                /* the connection has been authenticated fine */
  BIT(acceptfail);            /* used by the SFTP_QUOTE (continue if
                                 quote command fails) */
};

#अगर_घोषित USE_LIBSSH
#अगर LIBSSH_VERSION_INT < SSH_VERSION_INT(0, 9, 0)
#त्रुटि "SCP/SFTP protocols require libssh 0.9.0 or greater"
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_घोषित USE_LIBSSH2

/* Feature detection based on version numbers to better work with
   non-configure platforms */

#अगर !defined(LIBSSH2_VERSION_अंक) || (LIBSSH2_VERSION_अंक < 0x010900)
#त्रुटि "SCP/SFTP protocols require libssh2 1.9.0 or greater"
/* 1.9.0 was released on June 20 2019 */
#पूर्ण_अगर

#पूर्ण_अगर /* USE_LIBSSH2 */

#अगर_घोषित CURLVERBOSE
स्थिर अक्षर *Curl_ssh_statename(sshstate state);
#अन्यथा
#घोषणा Curl_ssh_statename(x) ""
#पूर्ण_अगर
व्योम Curl_ssh_set_state(काष्ठा Curl_easy *तथ्य,
                        काष्ठा ssh_conn *sshc,
                        sshstate nowstate);

#घोषणा myssh_to(x, y, z) Curl_ssh_set_state(x, y, z)

/* generic SSH backend functions */
CURLcode Curl_ssh_init(व्योम);
व्योम Curl_ssh_cleanup(व्योम);
व्योम Curl_ssh_version(अक्षर *बफर, माप_प्रकार buflen);
व्योम Curl_ssh_attach(काष्ठा Curl_easy *तथ्य,
                     काष्ठा connectdata *conn);
#अन्यथा /* !USE_SSH */
#घोषणा Curl_ssh_cleanup()
#घोषणा Curl_ssh_attach(x, y)
#घोषणा Curl_ssh_init() 0
#पूर्ण_अगर /* USE_SSH */

#पूर्ण_अगर /* HEADER_CURL_SSH_H */
