#اگر_غیر_تعریف_شدہ سرخی_CURL_FTP_INT_H
#تعریف_کرو سرخی_CURL_FTP_INT_H
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
#شامل "curl_setup.h"

#شامل "pingpong.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP

/****************************************************************************
 * FTP unique setup
 ***************************************************************************/
شمار {
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
  FTP_سائز, /* get the remote file's size for head-like request */
  FTP_RETR_سائز, /* get the remote file's size for RETR */
  FTP_STOR_سائز, /* get the size for STOR */
  FTP_REST, /* when used to check if the server supports it in head-like */
  FTP_RETR_REST, /* when asking for "resume" in for RETR */
  FTP_پورٹ, /* generic state for PORT, LPRT and EPRT, check count1 */
  FTP_PRET, /* generic state for PRET RETR, PRET STOR and PRET LIST/NLST */
  FTP_PASV, /* generic state for PASV and EPSV, check count1 */
  FTP_LIST, /* generic state for LIST, NLST or a custom list command */
  FTP_RETR,
  FTP_STOR, /* generic state for STOR and APPE */
  FTP_QUIT,
  FTP_آخری  /* never used */
};
قسمتعریف غیرعلامتی حرف ftpstate; /* use the enum values */

ساخت ftp_parselist_ڈیٹا; /* defined later in ftplistparser.c */

ساخت ftp_wc {
  ساخت ftp_parselist_ڈیٹا *parser;
};

/* This FTP struct is used in the Curl_easy. All FTP data that is
   connection-oriented must be in FTP_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
ساخت FTP {
  حرف *راستہ;    /* points to the urlpieces struct field */
  حرف *pathalloc; /* if non-NULL a pointer to an allocated path */

  /* transfer a file/body or not, done as a typedefed enum to make debuggers
     display the full symbol and not the numerical value */
  curl_pp_transfer transfer;
  curl_off_t downloadsize;
};

/* one struct entry for each path component (of 'rawpath') */
ساخت pathcomp {
  صحیح آغاز; /* start column */
  صحیح len;   /* length in bytes */
};

/* ftp_conn is used for struct connection-oriented data in the connectdata
   struct */
ساخت ftp_conn {
  ساخت pingpong pp;
  حرف *account;
  حرف *alternative_مقصد_user;
  حرف *entrypath; /* the PWD reply when we logged on */
  ثابت حرف *file; /* URL-decoded filename (or path), points into rawpath */
  حرف *rawpath; /* URL decoded, allocated, version of the path */
  ساخت pathcomp *dirs; /* allocated array for path components */
  حرف *prevpath;   /* URL-decoded conn->path from the previous transfer */
  حرف transfertype; /* set by ftp_transfertype for use by Curl_client_write()a
                        and others (A/I or zero) */
  حرف *server_os;     /* The target server operating system. */
  curl_off_t معلوم_filesize; /* file size is different from -1, if wildcard
                                LIST parsing was done and wc_statemach set
                                it */
  صحیح count1; /* general purpose counter for the state machine */
  صحیح count2; /* general purpose counter for the state machine */
  صحیح count3; /* general purpose counter for the state machine */
  غیرعلامتی مختصر dirdepth;  /* number of entries used in the 'dirs' array,
                               < FTP_MAX_DIR_DEPTH */
  غیرعلامتی مختصر cwdcount;  /* number of CWD commands issued,
                               < FTP_MAX_DIR_DEPTH */
  غیرعلامتی حرف state; /* (ftpstate enum) always use ftp.c:state() to change
                          state! */
  غیرعلامتی حرف use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or
                              IMAP or POP3 or others! (type: curl_usessl)*/
  غیرعلامتی حرف ccc;       /* ccc level for this connection */
  ثنائی_ہندسہ(ftp_trying_alternative);
  ثنائی_ہندسہ(dont_check);  /* Set to TRUE to prevent the final (post-transfer)
                       file size and 226/250 status check. It should still
                       read the line, ignore the result. */
  ثنائی_ہندسہ(ctl_valid);   /* Tells Curl_ftp_quit() whether or not to do anything. If
                       the connection has timed out or been closed, this
                       should be FALSE when it gets to Curl_ftp_quit() */
  ثنائی_ہندسہ(cwddone);     /* if it has been determined that the proper CWD combo
                       already has been done */
  ثنائی_ہندسہ(cwdfail);     /* set TRUE if a CWD command fails, as then we must prevent
                       caching the current directory */
  ثنائی_ہندسہ(wait_ڈیٹا_conn); /* this is set TRUE if data connection is waited */
  ثنائی_ہندسہ(shutdown);    /* connection is being shutdown, e.g. QUIT */
};

/* meta key for storing `struct FTP` as easy meta data */
#تعریف_کرو CURL_META_FTP_EASY   "meta:proto:ftp:easy"
/* meta key for storing `struct ftp_conn` as connection meta data */
#تعریف_کرو CURL_META_FTP_CONN   "meta:proto:ftp:conn"

#اگر_ختم /* CURL_DISABLE_FTP */

#اگر_ختم /* HEADER_CURL_FTP_INT_H */
