#నిర్వచితం_కాకపోతే శీర్షిక_CURL_FTP_INT_H
#నిర్వచించు శీర్షిక_CURL_FTP_INT_H
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

#అన్తర్భావ "pingpong.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP

/****************************************************************************
 * FTP unique setup
 ***************************************************************************/
గణన {
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
  FTP_కొలత, /* get the remote file's size for head-like request */
  FTP_RETR_కొలత, /* get the remote file's size for RETR */
  FTP_STOR_కొలత, /* get the size for STOR */
  FTP_REST, /* when used to check if the server supports it in head-like */
  FTP_RETR_REST, /* when asking for "resume" in for RETR */
  FTP_పోర్టు, /* generic state for PORT, LPRT and EPRT, check count1 */
  FTP_PRET, /* generic state for PRET RETR, PRET STOR and PRET LIST/NLST */
  FTP_PASV, /* generic state for PASV and EPSV, check count1 */
  FTP_LIST, /* generic state for LIST, NLST or a custom list command */
  FTP_RETR,
  FTP_STOR, /* generic state for STOR and APPE */
  FTP_QUIT,
  FTP_చివరి  /* never used */
};
రకనిర్వచనం గుర్తులేని అక్షరం ftpstate; /* use the enum values */

నిర్మాణం ftp_parselist_దత్తాంశం; /* defined later in ftplistparser.c */

నిర్మాణం ftp_wc {
  నిర్మాణం ftp_parselist_దత్తాంశం *parser;
};

/* This FTP struct is used in the Curl_easy. All FTP data that is
   connection-oriented must be in FTP_conn to properly deal with the fact that
   perhaps the Curl_easy is changed between the times the connection is
   used. */
నిర్మాణం FTP {
  అక్షరం *మార్గం;    /* points to the urlpieces struct field */
  అక్షరం *pathalloc; /* if non-NULL a pointer to an allocated path */

  /* transfer a file/body or not, done as a typedefed enum to make debuggers
     display the full symbol and not the numerical value */
  curl_pp_transfer transfer;
  curl_off_t downloadsize;
};

/* one struct entry for each path component (of 'rawpath') */
నిర్మాణం pathcomp {
  పూర్ణాంకం ప్రారంభం; /* start column */
  పూర్ణాంకం len;   /* length in bytes */
};

/* ftp_conn is used for struct connection-oriented data in the connectdata
   struct */
నిర్మాణం ftp_conn {
  నిర్మాణం pingpong pp;
  అక్షరం *account;
  అక్షరం *alternative_గమ్యం_user;
  అక్షరం *entrypath; /* the PWD reply when we logged on */
  స్థిరరాశి అక్షరం *file; /* URL-decoded filename (or path), points into rawpath */
  అక్షరం *rawpath; /* URL decoded, allocated, version of the path */
  నిర్మాణం pathcomp *dirs; /* allocated array for path components */
  అక్షరం *prevpath;   /* URL-decoded conn->path from the previous transfer */
  అక్షరం transfertype; /* set by ftp_transfertype for use by Curl_client_write()a
                        and others (A/I or zero) */
  అక్షరం *server_os;     /* The target server operating system. */
  curl_off_t తెలిసిన_filesize; /* file size is different from -1, if wildcard
                                LIST parsing was done and wc_statemach set
                                it */
  పూర్ణాంకం count1; /* general purpose counter for the state machine */
  పూర్ణాంకం count2; /* general purpose counter for the state machine */
  పూర్ణాంకం count3; /* general purpose counter for the state machine */
  గుర్తులేని పొట్టి dirdepth;  /* number of entries used in the 'dirs' array,
                               < FTP_MAX_DIR_DEPTH */
  గుర్తులేని పొట్టి cwdcount;  /* number of CWD commands issued,
                               < FTP_MAX_DIR_DEPTH */
  గుర్తులేని అక్షరం state; /* (ftpstate enum) always use ftp.c:state() to change
                          state! */
  గుర్తులేని అక్షరం use_ssl;   /* if AUTH TLS is to be attempted etc, for FTP or
                              IMAP or POP3 or others! (type: curl_usessl)*/
  గుర్తులేని అక్షరం ccc;       /* ccc level for this connection */
  బిట్టు(ftp_trying_alternative);
  బిట్టు(dont_check);  /* Set to TRUE to prevent the final (post-transfer)
                       file size and 226/250 status check. It should still
                       read the line, ignore the result. */
  బిట్టు(ctl_valid);   /* Tells Curl_ftp_quit() whether or not to do anything. If
                       the connection has timed out or been closed, this
                       should be FALSE when it gets to Curl_ftp_quit() */
  బిట్టు(cwddone);     /* if it has been determined that the proper CWD combo
                       already has been done */
  బిట్టు(cwdfail);     /* set TRUE if a CWD command fails, as then we must prevent
                       caching the current directory */
  బిట్టు(wait_దత్తాంశం_conn); /* this is set TRUE if data connection is waited */
  బిట్టు(shutdown);    /* connection is being shutdown, e.g. QUIT */
};

/* meta key for storing `struct FTP` as easy meta data */
#నిర్వచించు CURL_META_FTP_EASY   "meta:proto:ftp:easy"
/* meta key for storing `struct ftp_conn` as connection meta data */
#నిర్వచించు CURL_META_FTP_CONN   "meta:proto:ftp:conn"

#అయితే_ముగింపు /* CURL_DISABLE_FTP */

#అయితే_ముగింపు /* HEADER_CURL_FTP_INT_H */
