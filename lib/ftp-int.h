#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_FTP_INT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_FTP_INT_H
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

#ਅਨ੍ਤਰ੍ਭਾਵ "pingpong.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP

/****************************************************************************
 * FTP unique setup
 ***************************************************************************/
ਗਿਣਤੀ {
  FTP_STOP,    /* do nothing state, stops the state machine */
  FTP_WAIT220, /* waiting for the initial 220 response immediately after
                  a connect */
  FTP_AUTH,
  FTP_USER,
  FTP_PASS,
  FTP_ACCT,
  FTP_PBSZ,
  FTP_PROT,
  FTP_CCC,
  FTP_PWD,
  FTP_SYST,
  FTP_NAMEFMT,
  FTP_QUOTE, /* waiting for a response to a command sent in a quote list */
  FTP_RETR_PREQUOTE,
  FTP_STOR_PREQUOTE,
  FTP_LIST_PREQUOTE,
  FTP_POSTQUOTE,
  FTP_CWD,  /* change directory */
  FTP_MKD,  /* if the directory did not exist */
  FTP_MDTM, /* to figure out the datestamp */
  FTP_TYPE, /* to set type when doing a head-like request */
  FTP_LIST_TYPE, /* set type when about to do a directory list */
  FTP_RETR_LIST_TYPE,
  FTP_RETR_TYPE, /* set type when about to RETR a file */
  FTP_STOR_TYPE, /* set type when about to STOR a file */
  FTP_ਮਾਪ, /* get the remote file's size for head-like request */
  FTP_RETR_ਮਾਪ, /* get the remote file's size for RETR */
  FTP_STOR_ਮਾਪ, /* get the size for STOR */
  FTP_REST, /* when used to check if the server supports it in head-like */
  FTP_RETR_REST, /* when asking for "resume" in for RETR */
  FTP_ਪੋਰਟ, /* generic state for PORT, LPRT and EPRT, check count1 */
  FTP_PRET, /* generic state for PRET RETR, PRET STOR and PRET LIST/NLST */
  FTP_PASV, /* generic state for PASV and EPSV, check count1 */
  FTP_LIST, /* generic state for LIST, NLST or a custom list command */
  FTP_RETR,
  FTP_STOR, /* generic state for STOR and APPE */
  FTP_QUIT,
  FTP_ਆਖਰੀ  /* never used */
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ftpstate; /* use the enum values */

ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ; /* defined later in ftplistparser.c */

ਬਣਤਰ ftp_wc {
  ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser;
};

/* This FTP struct is used in the Curl_easy. All FTP data that is
   connection-oriented must be in FTP_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
ਬਣਤਰ FTP {
  ਅੱਖਰ *ਰਸਤਾ;    /* points to the urlpieces struct field */
  ਅੱਖਰ *pathalloc; /* if non-NULL a pointer to an allocated path */

  /* transfer a file/body or not, done as a typedefed enum to make debuggers
     display the full symbol and not the numerical value */
  curl_pp_transfer transfer;
  curl_off_t downloadsize;
};

/* one struct entry for each path component (of 'rawpath') */
ਬਣਤਰ pathcomp {
  ਪੂਰਨ ਸ਼ੁਰੂ; /* start column */
  ਪੂਰਨ len;   /* length in bytes */
};

/* ftp_conn is used for struct connection-oriented data in the connectdata
   struct */
ਬਣਤਰ ftp_conn {
  ਬਣਤਰ pingpong pp;
  ਅੱਖਰ *account;
  ਅੱਖਰ *alternative_ਮੰਜ਼ਿਲ_user;
  ਅੱਖਰ *entrypath; /* the PWD reply when we logged on */
  ਅਚਲ ਅੱਖਰ *file; /* URL-decoded filename (or path), points into rawpath */
  ਅੱਖਰ *rawpath; /* URL decoded, allocated, version of the path */
  ਬਣਤਰ pathcomp *dirs; /* allocated array for path components */
  ਅੱਖਰ *prevpath;   /* URL-decoded conn->path from the previous transfer */
  ਅੱਖਰ transfertype; /* set by ftp_transfertype for use by Curl_client_write()a
                        and others (A/I or zero) */
  ਅੱਖਰ *server_os;     /* The target server operating system. */
  curl_off_t ਗਿਆਤ_filesize; /* file size is different from -1, if wildcard
                                LIST parsing was done and wc_statemach set
                                it */
  ਪੂਰਨ count1; /* general purpose counter for the state machine */
  ਪੂਰਨ count2; /* general purpose counter for the state machine */
  ਪੂਰਨ count3; /* general purpose counter for the state machine */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ dirdepth;  /* number of entries used in the 'dirs' array,
                               < FTP_MAX_DIR_DEPTH */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ cwdcount;  /* number of CWD commands issued,
                               < FTP_MAX_DIR_DEPTH */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ state; /* (ftpstate enum) always use ftp.c:state() to change
                          state! */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or
                              IMAP or POP3 or others! (type: curl_usessl)*/
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ccc;       /* ccc level for this connection */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ftp_trying_alternative);
  ਦੋਆਧਾਰੀ_ਅੰਕ(dont_check);  /* Set to TRUE to prevent the final (post-transfer)
                       file size and 226/250 status check. It should still
                       read the line, ignore the result. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ctl_valid);   /* Tells Curl_ftp_quit() whether or not to do anything. If
                       the connection has timed out or been closed, this
                       should be FALSE when it gets to Curl_ftp_quit() */
  ਦੋਆਧਾਰੀ_ਅੰਕ(cwddone);     /* if it has been determined that the proper CWD combo
                       already has been done */
  ਦੋਆਧਾਰੀ_ਅੰਕ(cwdfail);     /* set TRUE if a CWD command fails, as then we must prevent
                       caching the current directory */
  ਦੋਆਧਾਰੀ_ਅੰਕ(wait_ਅੰਕੜੇ_conn); /* this is set TRUE if data connection is waited */
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown);    /* connection is being shutdown, e.g. QUIT */
};

/* meta key for storing `struct FTP` as easy meta data */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_FTP_EASY   "meta:proto:ftp:easy"
/* meta key for storing `struct ftp_conn` as connection meta data */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_FTP_CONN   "meta:proto:ftp:conn"

#ਜੇ_ਅੰਤ /* CURL_DISABLE_FTP */

#ਜੇ_ਅੰਤ /* HEADER_CURL_FTP_INT_H */
