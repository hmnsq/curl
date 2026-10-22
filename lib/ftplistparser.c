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

#నిర్వచితం_కాకపోతే CURL_DISABLE_FTP

/**
 * Now implemented:
 *
 * 1) Unix version 1
 * drwxr-xr-x 1 user01 ftp  512 Jan 29 23:32 prog
 * 2) Unix version 2
 * drwxr-xr-x 1 user01 ftp  512 Jan 29 1997  prog
 * 3) Unix version 3
 * drwxr-xr-x 1      1   1  512 Jan 29 23:32 prog
 * 4) Unix symlink
 * lrwxr-xr-x 1 user01 ftp  512 Jan 29 23:32 prog -> prog2000
 * 5) DOS style
 * 01-29-97 11:32PM <DIR> prog
 */

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "fileinfo.h"
#అన్తర్భావ "llist.h"
#అన్తర్భావ "ftp.h"
#అన్తర్భావ "ftp-int.h"
#అన్తర్భావ "ftplistparser.h"
#అన్తర్భావ "curl_fnmatch.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "curlx/strparse.h"

రకనిర్వచనం గణన {
  PL_UNIX_TOTALSIZE = 0,
  PL_UNIX_FILETYPE,
  PL_UNIX_PERMISSION,
  PL_UNIX_HLINKS,
  PL_UNIX_USER,
  PL_UNIX_సమూహం,
  PL_UNIX_కొలత,
  PL_UNIX_సమయం,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

రకనిర్వచనం సంఘం {
  గణన {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } మొత్తం_dirsize;

  గణన {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_సంఖ్య
  } hlinks;

  గణన {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  గణన {
    PL_UNIX_సమూహం_PRESPACE = 0,
    PL_UNIX_సమూహం_NAME
  } సమూహం;

  గణన {
    PL_UNIX_కొలత_PRESPACE = 0,
    PL_UNIX_కొలత_సంఖ్య
  } కొలత;

  గణన {
    PL_UNIX_సమయం_PREPART1 = 0,
    PL_UNIX_సమయం_PART1,
    PL_UNIX_సమయం_PREPART2,
    PL_UNIX_సమయం_PART2,
    PL_UNIX_సమయం_PREPART3,
    PL_UNIX_సమయం_PART3
  } సమయం;

  గణన {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  గణన {
    PL_UNIX_SYMLINK_PRESPACE = 0,
    PL_UNIX_SYMLINK_NAME,
    PL_UNIX_SYMLINK_PRETARGET1,
    PL_UNIX_SYMLINK_PRETARGET2,
    PL_UNIX_SYMLINK_PRETARGET3,
    PL_UNIX_SYMLINK_PRETARGET4,
    PL_UNIX_SYMLINK_లక్ష్యం,
    PL_UNIX_SYMLINK_WINDOWSEOL
  } symlink;
} pl_unix_substate;

రకనిర్వచనం గణన {
  PL_WINNT_DATE = 0,
  PL_WINNT_సమయం,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

రకనిర్వచనం సంఘం {
  గణన {
    PL_WINNT_సమయం_PRESPACE = 0,
    PL_WINNT_సమయం_సమయం
  } సమయం;
  గణన {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  గణన {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
నిర్మాణం ftp_parselist_దత్తాంశం {
  గణన {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  సంఘం {
    నిర్మాణం {
      pl_unix_mainstate ప్రధాన;
      pl_unix_substate sub;
    } UNIX;

    నిర్మాణం {
      pl_winNT_mainstate ప్రధాన;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  నిర్మాణం fileinfo *file_దత్తాంశం;
  గుర్తులేని పూర్ణాంకం అంశం_నిడివి;
  size_t అంశం_స్థానభ్రంశం;
  నిర్మాణం {
    size_t filename;
    size_t user;
    size_t సమూహం;
    size_t సమయం;
    size_t perm;
    size_t symlink_లక్ష్యం;
  } offsets;
};

స్థిర శూన్య fileinfo_dtor(శూన్య *user, శూన్య *element)
{
  (శూన్య)user;
  Curl_fileinfo_cleanup(element);
}

శూన్య Curl_wildcard_init(నిర్మాణం Wildcard_దత్తాంశం *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

శూన్య Curl_wildcard_dtor(నిర్మాణం Wildcard_దత్తాంశం **wcp)
{
  నిర్మాణం Wildcard_దత్తాంశం *wc = *wcp;
  ఒకవేళ(!wc)
    తిరిగి;

  ఒకవేళ(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = ఖాళీ;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, ఖాళీ);
  curlx_safefree(wc->మార్గం);
  curlx_safefree(wc->నమూనా);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = ఖాళీ;
}

నిర్మాణం ftp_parselist_దత్తాంశం *Curl_ftp_parselist_దత్తాంశం_alloc(శూన్య)
{
  తిరిగి curlx_calloc(1, పరిమాణం(నిర్మాణం ftp_parselist_దత్తాంశం));
}

శూన్య Curl_ftp_parselist_దత్తాంశం_free(నిర్మాణం ftp_parselist_దత్తాంశం **parserp)
{
  నిర్మాణం ftp_parselist_దత్తాంశం *parser = *parserp;
  ఒకవేళ(parser)
    Curl_fileinfo_cleanup(parser->file_దత్తాంశం);
  curlx_free(parser);
  *parserp = ఖాళీ;
}

CURLcode Curl_ftp_parselist_geterror(నిర్మాణం ftp_parselist_దత్తాంశం *pl_దత్తాంశం)
{
  తిరిగి pl_దత్తాంశం->error;
}

#నిర్వచించు FTP_LP_MALFORMATED_PERM 0x01000000

స్థిర గుర్తులేని పూర్ణాంకం ftp_pl_get_permission(స్థిరరాశి అక్షరం *str)
{
  గుర్తులేని పూర్ణాంకం permissions = 0;
  /* USER */
  ఒకవేళ(str[0] == 'r')
    permissions |= 1 << 8;
  లేకపోతే ఒకవేళ(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ఒకవేళ(str[1] == 'w')
    permissions |= 1 << 7;
  లేకపోతే ఒకవేళ(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ఒకవేళ(str[2] == 'x')
    permissions |= 1 << 6;
  లేకపోతే ఒకవేళ(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  లేకపోతే ఒకవేళ(str[2] == 'S')
    permissions |= 1 << 11;
  లేకపోతే ఒకవేళ(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /*సమూహం*/
  ఒకవేళ(str[3] == 'r')
    permissions |= 1 << 5;
  లేకపోతే ఒకవేళ(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ఒకవేళ(str[4] == 'w')
    permissions |= 1 << 4;
  లేకపోతే ఒకవేళ(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ఒకవేళ(str[5] == 'x')
    permissions |= 1 << 3;
  లేకపోతే ఒకవేళ(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  లేకపోతే ఒకవేళ(str[5] == 'S')
    permissions |= 1 << 10;
  లేకపోతే ఒకవేళ(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  ఒకవేళ(str[6] == 'r')
    permissions |= 1 << 2;
  లేకపోతే ఒకవేళ(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ఒకవేళ(str[7] == 'w')
    permissions |= 1 << 1;
  లేకపోతే ఒకవేళ(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ఒకవేళ(str[8] == 'x')
    permissions |= 1;
  లేకపోతే ఒకవేళ(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  లేకపోతే ఒకవేళ(str[8] == 'T')
    permissions |= 1 << 9;
  లేకపోతే ఒకవేళ(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  తిరిగి permissions;
}

స్థిర CURLcode ftp_pl_చొప్పించు_finfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం fileinfo *infop)
{
  curl_fnmatch_callback compare;
  నిర్మాణం Wildcard_దత్తాంశం *wc = దత్తాంశం->wildcard;
  నిర్మాణం ftp_wc *ftpwc = wc->ftpwc;
  నిర్మాణం Curl_llist *llist = &wc->filelist;
  నిర్మాణం ftp_parselist_దత్తాంశం *parser = ftpwc->parser;
  bool కూడు = TRUE;
  నిర్మాణం curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  అక్షరం *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->అక్షరశ్రేణులు.సమూహం  = parser->offsets.సమూహం ?
                          str + parser->offsets.సమూహం : ఖాళీ;
  finfo->అక్షరశ్రేణులు.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : ఖాళీ;
  finfo->అక్షరశ్రేణులు.లక్ష్యం = parser->offsets.symlink_లక్ష్యం &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_లక్ష్యం : ఖాళీ;
  finfo->అక్షరశ్రేణులు.సమయం   = str + parser->offsets.సమయం;
  finfo->అక్షరశ్రేణులు.user   = parser->offsets.user ?
                          str + parser->offsets.user : ఖాళీ;

  /* get correct fnmatch callback */
  compare = దత్తాంశం->సమితి.fnmatch;
  ఒకవేళ(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  ఒకవేళ(strchr(finfo->filename, '/'))
    కూడు = FALSE;
  లేకపోతే {
    /* a real file name cannot contain a slash */
    నిర్మాణం Curl_mapi_guard guard;
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fnmatch_దత్తాంశం);
    ఒకవేళ(compare(దత్తాంశం->సమితి.fnmatch_దత్తాంశం, wc->నమూనా, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      ఒకవేళ((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->అక్షరశ్రేణులు.లక్ష్యం &&
         అక్షరశ్రేణి_శోధన(finfo->అక్షరశ్రేణులు.లక్ష్యం, " -> ")) {
        కూడు = FALSE;
      }
    }
    లేకపోతే {
      కూడు = FALSE;
    }
    CURL_CBAPI_చివర(&guard);
  }

  ఒకవేళ(కూడు) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  లేకపోతే {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_దత్తాంశం = ఖాళీ;
  తిరిగి CURLE_OK;
}

#నిర్వచించు MAX_FTPLIST_బఫరు 10000 /* arbitrarily set */

స్థిర CURLcode unix_filetype(స్థిరరాశి అక్షరం c, curlfiletype *t)
{
  ఎంపిక(c) {
  స్థితి '-':
    *t = CURLFILETYPE_FILE;
    ఆపు;
  స్థితి 'd':
    *t = CURLFILETYPE_DIRECTORY;
    ఆపు;
  స్థితి 'l':
    *t = CURLFILETYPE_SYMLINK;
    ఆపు;
  స్థితి 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    ఆపు;
  స్థితి 's':
    *t = CURLFILETYPE_SOCKET;
    ఆపు;
  స్థితి 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    ఆపు;
  స్థితి 'b':
    *t = CURLFILETYPE_DEVICE_ఖండం;
    ఆపు;
  స్థితి 'D':
    *t = CURLFILETYPE_DOOR;
    ఆపు;
  అప్రమేయ:
    తిరిగి CURLE_FTP_BAD_FILE_LIST;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode parse_unix_totalsize(నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                                     నిర్మాణం fileinfo *infop,
                                     స్థిరరాశి అక్షరం c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);
  ఎంపిక(parser->state.UNIX.sub.మొత్తం_dirsize) {
  స్థితి PL_UNIX_TOTALSIZE_INIT:
    ఒకవేళ(c == 't') {
      parser->state.UNIX.sub.మొత్తం_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->అంశం_నిడివి++;
    }
    లేకపోతే {
      parser->state.UNIX.ప్రధాన = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    ఆపు;
  స్థితి PL_UNIX_TOTALSIZE_READING:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == '\r') {
      parser->అంశం_నిడివి--;
      ఒకవేళ(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    లేకపోతే ఒకవేళ(c == '\n') {
      mem[parser->అంశం_నిడివి - 1] = 0;
      ఒకవేళ(!అక్షరశ్రేణి_పోలిక("total ", mem, 6)) {
        స్థిరరాశి అక్షరం *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        అయితే(ISDIGIT(*endptr))
          endptr++;
        ఒకవేళ(*endptr) {
          తిరిగి CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.ప్రధాన = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      లేకపోతే
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
    }
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode parse_unix_permission(నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                                      నిర్మాణం fileinfo *infop,
                                      స్థిరరాశి అక్షరం c)
{
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);
  parser->అంశం_నిడివి++;
  ఒకవేళ((parser->అంశం_నిడివి <= 9) && !strchr("rwx-tTsS", c))
    తిరిగి CURLE_FTP_BAD_FILE_LIST;

  లేకపోతే ఒకవేళ(parser->అంశం_నిడివి == 10) {
    గుర్తులేని పూర్ణాంకం perm;
    ఒకవేళ(c != ' ')
      తిరిగి CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->అంశం_స్థానభ్రంశం);
    ఒకవేళ(perm & FTP_LP_MALFORMATED_PERM)
      తిరిగి CURLE_FTP_BAD_FILE_LIST;

    parser->file_దత్తాంశం->info.flags |= CURLFINFOFLAG_తెలిసిన_PERM;
    parser->file_దత్తాంశం->info.perm = perm;
    parser->offsets.perm = parser->అంశం_స్థానభ్రంశం;

    parser->అంశం_నిడివి = 0;
    parser->state.UNIX.ప్రధాన = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode parse_unix_hlinks(నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                                  నిర్మాణం fileinfo *infop,
                                  స్థిరరాశి అక్షరం c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);

  ఎంపిక(parser->state.UNIX.sub.hlinks) {
  స్థితి PL_UNIX_HLINKS_PRESPACE:
    ఒకవేళ(c != ' ') {
      ఒకవేళ(ISDIGIT(c) && len) {
        parser->అంశం_స్థానభ్రంశం = len - 1;
        parser->అంశం_నిడివి = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_సంఖ్య;
      }
      లేకపోతే
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
    }
    ఆపు;
  స్థితి PL_UNIX_HLINKS_సంఖ్య:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == ' ') {
      స్థిరరాశి అక్షరం *p = &mem[parser->అంశం_స్థానభ్రంశం];
      curl_off_t hlinks;
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;

      ఒకవేళ(!curlx_str_సంఖ్య(&p, &hlinks, LONG_MAX)) {
        parser->file_దత్తాంశం->info.flags |= CURLFINFOFLAG_తెలిసిన_HLINKCOUNT;
        parser->file_దత్తాంశం->info.hardlinks = (దీర్ఘ)hlinks;
      }
      parser->అంశం_నిడివి = 0;
      parser->అంశం_స్థానభ్రంశం = 0;
      parser->state.UNIX.ప్రధాన = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    లేకపోతే ఒకవేళ(!ISDIGIT(c))
      తిరిగి CURLE_FTP_BAD_FILE_LIST;

    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode parse_unix_user(నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                                నిర్మాణం fileinfo *infop,
                                స్థిరరాశి అక్షరం c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);
  ఎంపిక(parser->state.UNIX.sub.user) {
  స్థితి PL_UNIX_USER_PRESPACE:
    ఒకవేళ(c != ' ' && len) {
      parser->అంశం_స్థానభ్రంశం = len - 1;
      parser->అంశం_నిడివి = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    ఆపు;
  స్థితి PL_UNIX_USER_PARSING:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == ' ') {
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
      parser->offsets.user = parser->అంశం_స్థానభ్రంశం;
      parser->state.UNIX.ప్రధాన = PL_UNIX_సమూహం;
      parser->state.UNIX.sub.సమూహం = PL_UNIX_సమూహం_PRESPACE;
      parser->అంశం_స్థానభ్రంశం = 0;
      parser->అంశం_నిడివి = 0;
    }
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode parse_unix_సమూహం(నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                                 నిర్మాణం fileinfo *infop,
                                 స్థిరరాశి అక్షరం c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);
  ఎంపిక(parser->state.UNIX.sub.సమూహం) {
  స్థితి PL_UNIX_సమూహం_PRESPACE:
    ఒకవేళ(c != ' ' && len) {
      parser->అంశం_స్థానభ్రంశం = len - 1;
      parser->అంశం_నిడివి = 1;
      parser->state.UNIX.sub.సమూహం = PL_UNIX_సమూహం_NAME;
    }
    ఆపు;
  స్థితి PL_UNIX_సమూహం_NAME:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == ' ') {
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
      parser->offsets.సమూహం = parser->అంశం_స్థానభ్రంశం;
      parser->state.UNIX.ప్రధాన = PL_UNIX_కొలత;
      parser->state.UNIX.sub.కొలత = PL_UNIX_కొలత_PRESPACE;
      parser->అంశం_స్థానభ్రంశం = 0;
      parser->అంశం_నిడివి = 0;
    }
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode parse_unix_కొలత(నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                                నిర్మాణం fileinfo *infop,
                                స్థిరరాశి అక్షరం c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);
  ఎంపిక(parser->state.UNIX.sub.కొలత) {
  స్థితి PL_UNIX_కొలత_PRESPACE:
    ఒకవేళ(c != ' ') {
      ఒకవేళ(ISDIGIT(c) && len) {
        parser->అంశం_స్థానభ్రంశం = len - 1;
        parser->అంశం_నిడివి = 1;
        parser->state.UNIX.sub.కొలత = PL_UNIX_కొలత_సంఖ్య;
      }
      లేకపోతే
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
    }
    ఆపు;
  స్థితి PL_UNIX_కొలత_సంఖ్య:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == ' ') {
      స్థిరరాశి అక్షరం *p = mem + parser->అంశం_స్థానభ్రంశం;
      curl_off_t fsize;
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
      ఒకవేళ(!curlx_str_numblanks(&p, &fsize)) {
        ఒకవేళ(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_దత్తాంశం->info.flags |= CURLFINFOFLAG_తెలిసిన_కొలత;
          parser->file_దత్తాంశం->info.కొలత = fsize;
        }
        parser->అంశం_నిడివి = 0;
        parser->అంశం_స్థానభ్రంశం = 0;
        parser->state.UNIX.ప్రధాన = PL_UNIX_సమయం;
        parser->state.UNIX.sub.సమయం = PL_UNIX_సమయం_PREPART1;
      }
    }
    లేకపోతే ఒకవేళ(!ISDIGIT(c))
      తిరిగి CURLE_FTP_BAD_FILE_LIST;

    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode parse_unix_సమయం(నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                                నిర్మాణం fileinfo *infop,
                                స్థిరరాశి అక్షరం c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);
  నిర్మాణం curl_fileinfo *finfo = &infop->info;

  ఎంపిక(parser->state.UNIX.sub.సమయం) {
  స్థితి PL_UNIX_సమయం_PREPART1:
    ఒకవేళ(c != ' ') {
      ఒకవేళ(ISALNUM(c) && len) {
        parser->అంశం_స్థానభ్రంశం = len - 1;
        parser->అంశం_నిడివి = 1;
        parser->state.UNIX.sub.సమయం = PL_UNIX_సమయం_PART1;
      }
      లేకపోతే
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
    }
    ఆపు;
  స్థితి PL_UNIX_సమయం_PART1:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == ' ')
      parser->state.UNIX.sub.సమయం = PL_UNIX_సమయం_PREPART2;

    లేకపోతే ఒకవేళ(!ISALNUM(c) && c != '.')
      తిరిగి CURLE_FTP_BAD_FILE_LIST;

    ఆపు;
  స్థితి PL_UNIX_సమయం_PREPART2:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c != ' ') {
      ఒకవేళ(ISALNUM(c))
        parser->state.UNIX.sub.సమయం = PL_UNIX_సమయం_PART2;
      లేకపోతే
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
    }
    ఆపు;
  స్థితి PL_UNIX_సమయం_PART2:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == ' ')
      parser->state.UNIX.sub.సమయం = PL_UNIX_సమయం_PREPART3;
    లేకపోతే ఒకవేళ(!ISALNUM(c) && c != '.')
      తిరిగి CURLE_FTP_BAD_FILE_LIST;
    ఆపు;
  స్థితి PL_UNIX_సమయం_PREPART3:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c != ' ') {
      ఒకవేళ(ISALNUM(c))
        parser->state.UNIX.sub.సమయం = PL_UNIX_సమయం_PART3;
      లేకపోతే
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
    }
    ఆపు;
  స్థితి PL_UNIX_సమయం_PART3:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == ' ') {
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
      parser->offsets.సమయం = parser->అంశం_స్థానభ్రంశం;
      ఒకవేళ(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.ప్రధాన = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      లేకపోతే {
        parser->state.UNIX.ప్రధాన = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    లేకపోతే ఒకవేళ(!ISALNUM(c) && c != '.' && c != ':')
      తిరిగి CURLE_FTP_BAD_FILE_LIST;
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode parse_unix_filename(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                                    నిర్మాణం fileinfo *infop,
                                    స్థిరరాశి అక్షరం c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ఫలితం = CURLE_OK;

  ఎంపిక(parser->state.UNIX.sub.filename) {
  స్థితి PL_UNIX_FILENAME_PRESPACE:
    ఒకవేళ(c != ' ' && len) {
      parser->అంశం_స్థానభ్రంశం = len - 1;
      parser->అంశం_నిడివి = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    ఆపు;
  స్థితి PL_UNIX_FILENAME_NAME:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    లేకపోతే ఒకవేళ(c == '\n') {
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
      parser->offsets.filename = parser->అంశం_స్థానభ్రంశం;
      parser->state.UNIX.ప్రధాన = PL_UNIX_FILETYPE;
      ఫలితం = ftp_pl_చొప్పించు_finfo(దత్తాంశం, infop);
    }
    ఆపు;
  స్థితి PL_UNIX_FILENAME_WINDOWSEOL:
    ఒకవేళ(c == '\n') {
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
      parser->offsets.filename = parser->అంశం_స్థానభ్రంశం;
      parser->state.UNIX.ప్రధాన = PL_UNIX_FILETYPE;
      ఫలితం = ftp_pl_చొప్పించు_finfo(దత్తాంశం, infop);
    }
    లేకపోతే
      ఫలితం = CURLE_FTP_BAD_FILE_LIST;
    ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode parse_unix_symlink(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                                   నిర్మాణం fileinfo *infop,
                                   స్థిరరాశి అక్షరం c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ఫలితం = CURLE_OK;

  ఎంపిక(parser->state.UNIX.sub.symlink) {
  స్థితి PL_UNIX_SYMLINK_PRESPACE:
    ఒకవేళ(c != ' ' && len) {
      parser->అంశం_స్థానభ్రంశం = len - 1;
      parser->అంశం_నిడివి = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    ఆపు;
  స్థితి PL_UNIX_SYMLINK_NAME:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    లేకపోతే ఒకవేళ(c == '\r' || c == '\n')
      తిరిగి CURLE_FTP_BAD_FILE_LIST;

    ఆపు;
  స్థితి PL_UNIX_SYMLINK_PRETARGET1:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    లేకపోతే ఒకవేళ(c == '\r' || c == '\n')
      తిరిగి CURLE_FTP_BAD_FILE_LIST;
    లేకపోతే
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ఆపు;
  స్థితి PL_UNIX_SYMLINK_PRETARGET2:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    లేకపోతే ఒకవేళ(c == '\r' || c == '\n')
      తిరిగి CURLE_FTP_BAD_FILE_LIST;
    లేకపోతే
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    ఆపు;
  స్థితి PL_UNIX_SYMLINK_PRETARGET3:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 4] = 0;
      parser->offsets.filename = parser->అంశం_స్థానభ్రంశం;
      parser->అంశం_నిడివి = 0;
      parser->అంశం_స్థానభ్రంశం = 0;
    }
    లేకపోతే ఒకవేళ(c == '\r' || c == '\n')
      తిరిగి CURLE_FTP_BAD_FILE_LIST;
    లేకపోతే
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ఆపు;
  స్థితి PL_UNIX_SYMLINK_PRETARGET4:
    ఒకవేళ(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_లక్ష్యం;
      parser->అంశం_స్థానభ్రంశం = len - 1;
      parser->అంశం_నిడివి = 1;
    }
    లేకపోతే
      తిరిగి CURLE_FTP_BAD_FILE_LIST;

    ఆపు;
  స్థితి PL_UNIX_SYMLINK_లక్ష్యం:
    parser->అంశం_నిడివి++;
    ఒకవేళ(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    లేకపోతే ఒకవేళ(c == '\n') {
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
      parser->offsets.symlink_లక్ష్యం = parser->అంశం_స్థానభ్రంశం;
      ఫలితం = ftp_pl_చొప్పించు_finfo(దత్తాంశం, infop);
      ఒకవేళ(ఫలితం)
        ఆపు;

      parser->state.UNIX.ప్రధాన = PL_UNIX_FILETYPE;
    }
    ఆపు;
  స్థితి PL_UNIX_SYMLINK_WINDOWSEOL:
    ఒకవేళ(c == '\n') {
      mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
      parser->offsets.symlink_లక్ష్యం = parser->అంశం_స్థానభ్రంశం;
      ఫలితం = ftp_pl_చొప్పించు_finfo(దత్తాంశం, infop);
      ఒకవేళ(ఫలితం)
        ఆపు;

      parser->state.UNIX.ప్రధాన = PL_UNIX_FILETYPE;
    }
    లేకపోతే
      ఫలితం = CURLE_FTP_BAD_FILE_LIST;

    ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode parse_unix(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                           నిర్మాణం fileinfo *infop,
                           స్థిరరాశి అక్షరం c)
{
  నిర్మాణం curl_fileinfo *finfo = &infop->info;
  CURLcode ఫలితం = CURLE_OK;

  ఎంపిక(parser->state.UNIX.ప్రధాన) {
  స్థితి PL_UNIX_TOTALSIZE:
    ఫలితం = parse_unix_totalsize(parser, infop, c);
    ఒకవేళ(ఫలితం)
      ఆపు;
    ఒకవేళ(parser->state.UNIX.ప్రధాన != PL_UNIX_FILETYPE)
      ఆపు;
    FALLTHROUGH();
  స్థితి PL_UNIX_FILETYPE:
    ఫలితం = unix_filetype(c, &finfo->filetype);
    ఒకవేళ(!ఫలితం) {
      parser->state.UNIX.ప్రధాన = PL_UNIX_PERMISSION;
      parser->అంశం_నిడివి = 0;
      parser->అంశం_స్థానభ్రంశం = 1;
    }
    ఆపు;
  స్థితి PL_UNIX_PERMISSION:
    ఫలితం = parse_unix_permission(parser, infop, c);
    ఆపు;
  స్థితి PL_UNIX_HLINKS:
    ఫలితం = parse_unix_hlinks(parser, infop, c);
    ఆపు;
  స్థితి PL_UNIX_USER:
    ఫలితం = parse_unix_user(parser, infop, c);
    ఆపు;
  స్థితి PL_UNIX_సమూహం:
    ఫలితం = parse_unix_సమూహం(parser, infop, c);
    ఆపు;
  స్థితి PL_UNIX_కొలత:
    ఫలితం = parse_unix_కొలత(parser, infop, c);
    ఆపు;
  స్థితి PL_UNIX_సమయం:
    ఫలితం = parse_unix_సమయం(parser, infop, c);
    ఆపు;
  స్థితి PL_UNIX_FILENAME:
    ఫలితం = parse_unix_filename(దత్తాంశం, parser, infop, c);
    ఆపు;
  స్థితి PL_UNIX_SYMLINK:
    ఫలితం = parse_unix_symlink(దత్తాంశం, parser, infop, c);
    ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode parse_winnt(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ftp_parselist_దత్తాంశం *parser,
                            నిర్మాణం fileinfo *infop,
                            స్థిరరాశి అక్షరం c)
{
  నిర్మాణం curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  అక్షరం *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ఫలితం = CURLE_OK;

  ఎంపిక(parser->state.NT.ప్రధాన) {
  స్థితి PL_WINNT_DATE:
    parser->అంశం_నిడివి++;
    ఒకవేళ(parser->అంశం_నిడివి < 9) {
      ఒకవేళ(!strchr("0123456789-", c)) { /* only simple control */
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
      }
    }
    లేకపోతే ఒకవేళ(parser->అంశం_నిడివి == 9) {
      ఒకవేళ(c == ' ') {
        parser->state.NT.ప్రధాన = PL_WINNT_సమయం;
        parser->state.NT.sub.సమయం = PL_WINNT_సమయం_PRESPACE;
      }
      లేకపోతే
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
    }
    లేకపోతే
      తిరిగి CURLE_FTP_BAD_FILE_LIST;
    ఆపు;
  స్థితి PL_WINNT_సమయం:
    parser->అంశం_నిడివి++;
    ఎంపిక(parser->state.NT.sub.సమయం) {
    స్థితి PL_WINNT_సమయం_PRESPACE:
      ఒకవేళ(!ISBLANK(c))
        parser->state.NT.sub.సమయం = PL_WINNT_సమయం_సమయం;
      ఆపు;
    స్థితి PL_WINNT_సమయం_సమయం:
      ఒకవేళ(c == ' ') {
        parser->offsets.సమయం = parser->అంశం_స్థానభ్రంశం;
        mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
        parser->state.NT.ప్రధాన = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->అంశం_నిడివి = 0;
      }
      లేకపోతే ఒకవేళ(!strchr("APM0123456789:", c))
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
      ఆపు;
    }
    ఆపు;
  స్థితి PL_WINNT_DIRORSIZE:
    ఎంపిక(parser->state.NT.sub.dirorsize) {
    స్థితి PL_WINNT_DIRORSIZE_PRESPACE:
      ఒకవేళ(c != ' ' && len) {
        parser->అంశం_స్థానభ్రంశం = len - 1;
        parser->అంశం_నిడివి = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      ఆపు;
    స్థితి PL_WINNT_DIRORSIZE_CONTENT:
      parser->అంశం_నిడివి++;
      ఒకవేళ(c == ' ') {
        mem[parser->అంశం_స్థానభ్రంశం + parser->అంశం_నిడివి - 1] = 0;
        ఒకవేళ(!strcmp("<DIR>", mem + parser->అంశం_స్థానభ్రంశం)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->కొలత = 0;
        }
        లేకపోతే {
          స్థిరరాశి అక్షరం *p = mem + parser->అంశం_స్థానభ్రంశం;
          ఒకవేళ(curlx_str_numblanks(&p, &finfo->కొలత)) {
            తిరిగి CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_దత్తాంశం->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_దత్తాంశం->info.flags |= CURLFINFOFLAG_తెలిసిన_కొలత;
        parser->అంశం_నిడివి = 0;
        parser->state.NT.ప్రధాన = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ఆపు;
    }
    ఆపు;
  స్థితి PL_WINNT_FILENAME:
    ఎంపిక(parser->state.NT.sub.filename) {
    స్థితి PL_WINNT_FILENAME_PRESPACE:
      ఒకవేళ(c != ' ' && len) {
        parser->అంశం_స్థానభ్రంశం = len - 1;
        parser->అంశం_నిడివి = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      ఆపు;
    స్థితి PL_WINNT_FILENAME_CONTENT:
      parser->అంశం_నిడివి++;
      ఒకవేళ(!len)
        తిరిగి CURLE_FTP_BAD_FILE_LIST;
      ఒకవేళ(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      లేకపోతే ఒకవేళ(c == '\n') {
        parser->offsets.filename = parser->అంశం_స్థానభ్రంశం;
        mem[len - 1] = 0;
        ఫలితం = ftp_pl_చొప్పించు_finfo(దత్తాంశం, infop);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;

        parser->state.NT.ప్రధాన = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ఆపు;
    స్థితి PL_WINNT_FILENAME_WINEOL:
      ఒకవేళ(c == '\n') {
        parser->offsets.filename = parser->అంశం_స్థానభ్రంశం;
        ఫలితం = ftp_pl_చొప్పించు_finfo(దత్తాంశం, infop);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;

        parser->state.NT.ప్రధాన = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      లేకపోతే
        తిరిగి CURLE_FTP_BAD_FILE_LIST;

      ఆపు;
    }
    ఆపు;
  }

  తిరిగి CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(దత్తాంశం->state.wildcardmatch) {
    నిర్మాణం Wildcard_దత్తాంశం * స్థిరరాశి wildcard = దత్తాంశం->wildcard;
    తిరిగి wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  తిరిగి FALSE;
}

size_t Curl_ftp_parselist(అక్షరం *బఫరు, size_t కొలత, size_t nmemb,
                          శూన్య *connptr)
{
  size_t bufflen = కొలత * nmemb;
  నిర్మాణం Curl_easy *దత్తాంశం = (నిర్మాణం Curl_easy *)connptr;
  నిర్మాణం ftp_wc *ftpwc = దత్తాంశం->wildcard->ftpwc;
  నిర్మాణం ftp_parselist_దత్తాంశం *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode ఫలితం;
  size_t retsize = bufflen;

  ఒకవేళ(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    వెళ్ళు fail;
  }

  ఒకవేళ(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(బఫరు[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  అయితే(i < bufflen) { /* FSM */
    అక్షరం c = బఫరు[i];
    నిర్మాణం fileinfo *infop;
    ఒకవేళ(!parser->file_దత్తాంశం) { /* tmp file data is not allocated yet */
      parser->file_దత్తాంశం = Curl_fileinfo_alloc();
      ఒకవేళ(!parser->file_దత్తాంశం) {
        parser->error = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు fail;
      }
      parser->అంశం_స్థానభ్రంశం = 0;
      parser->అంశం_నిడివి = 0;
      curlx_dyn_init(&parser->file_దత్తాంశం->buf, MAX_FTPLIST_బఫరు);
    }

    infop = parser->file_దత్తాంశం;

    ఒకవేళ(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు fail;
    }

    ఎంపిక(parser->os_type) {
    స్థితి OS_TYPE_UNIX:
      ఫలితం = parse_unix(దత్తాంశం, parser, infop, c);
      ఆపు;
    స్థితి OS_TYPE_WIN_NT:
      ఫలితం = parse_winnt(దత్తాంశం, parser, infop, c);
      ఆపు;
    అప్రమేయ:
      retsize = bufflen + 1;
      వెళ్ళు fail;
    }
    ఒకవేళ(ఫలితం) {
      parser->error = ఫలితం;
      వెళ్ళు fail;
    }

    i++;
  }
  తిరిగి retsize;

fail:

  /* Clean up any allocated memory. */
  ఒకవేళ(parser->file_దత్తాంశం) {
    Curl_fileinfo_cleanup(parser->file_దత్తాంశం);
    parser->file_దత్తాంశం = ఖాళీ;
  }

  తిరిగి retsize;
}

#అయితే_ముగింపు /* !CURL_DISABLE_FTP */
