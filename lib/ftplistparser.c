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

#ifndef CURL_DISABLE_FTP

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

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "fileinfo.h"
#অন্তর্ভাব "llist.h"
#অন্তর্ভাব "ftp.h"
#অন্তর্ভাব "ftp-int.h"
#অন্তর্ভাব "ftplistparser.h"
#অন্তর্ভাব "curl_fnmatch.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "curlx/strparse.h"

প্রকারলেখ গণনা {
  PL_UNIX_TOTALSIZE = 0,
  PL_UNIX_FILETYPE,
  PL_UNIX_PERMISSION,
  PL_UNIX_HLINKS,
  PL_UNIX_USER,
  PL_UNIX_GROUP,
  PL_UNIX_আকার,
  PL_UNIX_TIME,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

প্রকারলেখ সংঘ {
  গণনা {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } যোগ_dirsize;

  গণনা {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_সংখ্যা
  } hlinks;

  গণনা {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  গণনা {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  গণনা {
    PL_UNIX_আকার_PRESPACE = 0,
    PL_UNIX_আকার_সংখ্যা
  } আকার_2;

  গণনা {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  গণনা {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  গণনা {
    PL_UNIX_SYMLINK_PRESPACE = 0,
    PL_UNIX_SYMLINK_NAME,
    PL_UNIX_SYMLINK_PRETARGET1,
    PL_UNIX_SYMLINK_PRETARGET2,
    PL_UNIX_SYMLINK_PRETARGET3,
    PL_UNIX_SYMLINK_PRETARGET4,
    PL_UNIX_SYMLINK_TARGET,
    PL_UNIX_SYMLINK_WINDOWSEOL
  } symlink;
} pl_unix_substate;

প্রকারলেখ গণনা {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

প্রকারলেখ সংঘ {
  গণনা {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  গণনা {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  গণনা {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
কাঠামো ftp_parselist_তথ্য {
  গণনা {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  সংঘ {
    কাঠামো {
      pl_unix_mainstate প্রধান;
      pl_unix_substate sub;
    } UNIX;

    কাঠামো {
      pl_winNT_mainstate প্রধান;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  কাঠামো fileinfo *file_তথ্য;
  unsigned পূর্ণসংখ্যা item_দৈর্ঘ্য;
  size_t item_offset;
  কাঠামো {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

স্থির শূন্য fileinfo_dtor(শূন্য *user, শূন্য *element)
{
  (শূন্য)user;
  Curl_fileinfo_cleanup(element);
}

শূন্য Curl_wildcard_init(কাঠামো Wildcard_তথ্য *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

শূন্য Curl_wildcard_dtor(কাঠামো Wildcard_তথ্য **wcp)
{
  কাঠামো Wildcard_তথ্য *wc = *wcp;
  যদি(!wc)
    ফেরত;

  যদি(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = ফাঁকা;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, ফাঁকা);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = ফাঁকা;
}

কাঠামো ftp_parselist_তথ্য *Curl_ftp_parselist_তথ্য_alloc(শূন্য)
{
  ফেরত curlx_calloc(1, আকার(কাঠামো ftp_parselist_তথ্য));
}

শূন্য Curl_ftp_parselist_তথ্য_free(কাঠামো ftp_parselist_তথ্য **parserp)
{
  কাঠামো ftp_parselist_তথ্য *parser = *parserp;
  যদি(parser)
    Curl_fileinfo_cleanup(parser->file_তথ্য);
  curlx_free(parser);
  *parserp = ফাঁকা;
}

CURLcode Curl_ftp_parselist_geterror(কাঠামো ftp_parselist_তথ্য *pl_তথ্য)
{
  ফেরত pl_তথ্য->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

স্থির unsigned পূর্ণসংখ্যা ftp_pl_get_permission(ধ্রুবক অক্ষর *str)
{
  unsigned পূর্ণসংখ্যা permissions = 0;
  /* USER */
  যদি(str[0] == 'r')
    permissions |= 1 << 8;
  অন্যথা যদি(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[1] == 'w')
    permissions |= 1 << 7;
  অন্যথা যদি(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  যদি(str[2] == 'x')
    permissions |= 1 << 6;
  অন্যথা যদি(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  অন্যথা যদি(str[2] == 'S')
    permissions |= 1 << 11;
  অন্যথা যদি(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  যদি(str[3] == 'r')
    permissions |= 1 << 5;
  অন্যথা যদি(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[4] == 'w')
    permissions |= 1 << 4;
  অন্যথা যদি(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[5] == 'x')
    permissions |= 1 << 3;
  অন্যথা যদি(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  অন্যথা যদি(str[5] == 'S')
    permissions |= 1 << 10;
  অন্যথা যদি(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  যদি(str[6] == 'r')
    permissions |= 1 << 2;
  অন্যথা যদি(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[7] == 'w')
    permissions |= 1 << 1;
  অন্যথা যদি(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[8] == 'x')
    permissions |= 1;
  অন্যথা যদি(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  অন্যথা যদি(str[8] == 'T')
    permissions |= 1 << 9;
  অন্যথা যদি(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ফেরত permissions;
}

স্থির CURLcode ftp_pl_ঢোকাও_finfo(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো fileinfo *infop)
{
  curl_fnmatch_callback compare;
  কাঠামো Wildcard_তথ্য *wc = তথ্য->wildcard;
  কাঠামো ftp_wc *ftpwc = wc->ftpwc;
  কাঠামো Curl_llist *llist = &wc->filelist;
  কাঠামো ftp_parselist_তথ্য *parser = ftpwc->parser;
  bool add = TRUE;
  কাঠামো curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  অক্ষর *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : ফাঁকা;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : ফাঁকা;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : ফাঁকা;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : ফাঁকা;

  /* get correct fnmatch callback */
  compare = তথ্য->set.fnmatch;
  যদি(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  যদি(strchr(finfo->filename, '/'))
    add = FALSE;
  অন্যথা {
    /* a real file name cannot contain a slash */
    কাঠামো Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, তথ্য, easy_fnmatch_তথ্য);
    যদি(compare(তথ্য->set.fnmatch_তথ্য, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      যদি((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    অন্যথা {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  যদি(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  অন্যথা {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_তথ্য = ফাঁকা;
  ফেরত CURLE_OK;
}

#define MAX_FTPLIST_বাফার 10000 /* arbitrarily set */

স্থির CURLcode unix_filetype(ধ্রুবক অক্ষর c, curlfiletype *t)
{
  নির্বাচন(c) {
  ক্ষেত্র '-':
    *t = CURLFILETYPE_FILE;
    বিরতি;
  ক্ষেত্র 'd':
    *t = CURLFILETYPE_DIRECTORY;
    বিরতি;
  ক্ষেত্র 'l':
    *t = CURLFILETYPE_SYMLINK;
    বিরতি;
  ক্ষেত্র 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    বিরতি;
  ক্ষেত্র 's':
    *t = CURLFILETYPE_SOCKET;
    বিরতি;
  ক্ষেত্র 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    বিরতি;
  ক্ষেত্র 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    বিরতি;
  ক্ষেত্র 'D':
    *t = CURLFILETYPE_DOOR;
    বিরতি;
  পূর্বনির্ধারিত:
    ফেরত CURLE_FTP_BAD_FILE_LIST;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode parse_unix_totalsize(কাঠামো ftp_parselist_তথ্য *parser,
                                     কাঠামো fileinfo *infop,
                                     ধ্রুবক অক্ষর c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);
  নির্বাচন(parser->state.UNIX.sub.যোগ_dirsize) {
  ক্ষেত্র PL_UNIX_TOTALSIZE_INIT:
    যদি(c == 't') {
      parser->state.UNIX.sub.যোগ_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_দৈর্ঘ্য++;
    }
    অন্যথা {
      parser->state.UNIX.প্রধান = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_TOTALSIZE_READING:
    parser->item_দৈর্ঘ্য++;
    যদি(c == '\r') {
      parser->item_দৈর্ঘ্য--;
      যদি(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    অন্যথা যদি(c == '\n') {
      mem[parser->item_দৈর্ঘ্য - 1] = 0;
      যদি(!strncmp("total ", mem, 6)) {
        ধ্রুবক অক্ষর *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        যখন(ISDIGIT(*endptr))
          endptr++;
        যদি(*endptr) {
          ফেরত CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.প্রধান = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      অন্যথা
        ফেরত CURLE_FTP_BAD_FILE_LIST;
    }
    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode parse_unix_permission(কাঠামো ftp_parselist_তথ্য *parser,
                                      কাঠামো fileinfo *infop,
                                      ধ্রুবক অক্ষর c)
{
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_দৈর্ঘ্য++;
  যদি((parser->item_দৈর্ঘ্য <= 9) && !strchr("rwx-tTsS", c))
    ফেরত CURLE_FTP_BAD_FILE_LIST;

  অন্যথা যদি(parser->item_দৈর্ঘ্য == 10) {
    unsigned পূর্ণসংখ্যা perm;
    যদি(c != ' ')
      ফেরত CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    যদি(perm & FTP_LP_MALFORMATED_PERM)
      ফেরত CURLE_FTP_BAD_FILE_LIST;

    parser->file_তথ্য->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_তথ্য->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_দৈর্ঘ্য = 0;
    parser->state.UNIX.প্রধান = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode parse_unix_hlinks(কাঠামো ftp_parselist_তথ্য *parser,
                                  কাঠামো fileinfo *infop,
                                  ধ্রুবক অক্ষর c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);

  নির্বাচন(parser->state.UNIX.sub.hlinks) {
  ক্ষেত্র PL_UNIX_HLINKS_PRESPACE:
    যদি(c != ' ') {
      যদি(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_দৈর্ঘ্য = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_সংখ্যা;
      }
      অন্যথা
        ফেরত CURLE_FTP_BAD_FILE_LIST;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_HLINKS_সংখ্যা:
    parser->item_দৈর্ঘ্য++;
    যদি(c == ' ') {
      ধ্রুবক অক্ষর *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;

      যদি(!curlx_str_সংখ্যা(&p, &hlinks, LONG_MAX)) {
        parser->file_তথ্য->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_তথ্য->info.hardlinks = (long)hlinks;
      }
      parser->item_দৈর্ঘ্য = 0;
      parser->item_offset = 0;
      parser->state.UNIX.প্রধান = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    অন্যথা যদি(!ISDIGIT(c))
      ফেরত CURLE_FTP_BAD_FILE_LIST;

    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode parse_unix_user(কাঠামো ftp_parselist_তথ্য *parser,
                                কাঠামো fileinfo *infop,
                                ধ্রুবক অক্ষর c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);
  নির্বাচন(parser->state.UNIX.sub.user) {
  ক্ষেত্র PL_UNIX_USER_PRESPACE:
    যদি(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_দৈর্ঘ্য = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_USER_PARSING:
    parser->item_দৈর্ঘ্য++;
    যদি(c == ' ') {
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.প্রধান = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_দৈর্ঘ্য = 0;
    }
    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode parse_unix_group(কাঠামো ftp_parselist_তথ্য *parser,
                                 কাঠামো fileinfo *infop,
                                 ধ্রুবক অক্ষর c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);
  নির্বাচন(parser->state.UNIX.sub.group) {
  ক্ষেত্র PL_UNIX_GROUP_PRESPACE:
    যদি(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_দৈর্ঘ্য = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_GROUP_NAME:
    parser->item_দৈর্ঘ্য++;
    যদি(c == ' ') {
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.প্রধান = PL_UNIX_আকার;
      parser->state.UNIX.sub.আকার_2 = PL_UNIX_আকার_PRESPACE;
      parser->item_offset = 0;
      parser->item_দৈর্ঘ্য = 0;
    }
    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode parse_unix_আকার(কাঠামো ftp_parselist_তথ্য *parser,
                                কাঠামো fileinfo *infop,
                                ধ্রুবক অক্ষর c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);
  নির্বাচন(parser->state.UNIX.sub.আকার_2) {
  ক্ষেত্র PL_UNIX_আকার_PRESPACE:
    যদি(c != ' ') {
      যদি(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_দৈর্ঘ্য = 1;
        parser->state.UNIX.sub.আকার_2 = PL_UNIX_আকার_সংখ্যা;
      }
      অন্যথা
        ফেরত CURLE_FTP_BAD_FILE_LIST;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_আকার_সংখ্যা:
    parser->item_দৈর্ঘ্য++;
    যদি(c == ' ') {
      ধ্রুবক অক্ষর *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
      যদি(!curlx_str_numblanks(&p, &fsize)) {
        যদি(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_তথ্য->info.flags |= CURLFINFOFLAG_KNOWN_আকার;
          parser->file_তথ্য->info.আকার_2 = fsize;
        }
        parser->item_দৈর্ঘ্য = 0;
        parser->item_offset = 0;
        parser->state.UNIX.প্রধান = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    অন্যথা যদি(!ISDIGIT(c))
      ফেরত CURLE_FTP_BAD_FILE_LIST;

    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode parse_unix_time(কাঠামো ftp_parselist_তথ্য *parser,
                                কাঠামো fileinfo *infop,
                                ধ্রুবক অক্ষর c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);
  কাঠামো curl_fileinfo *finfo = &infop->info;

  নির্বাচন(parser->state.UNIX.sub.time) {
  ক্ষেত্র PL_UNIX_TIME_PREPART1:
    যদি(c != ' ') {
      যদি(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_দৈর্ঘ্য = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      অন্যথা
        ফেরত CURLE_FTP_BAD_FILE_LIST;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_TIME_PART1:
    parser->item_দৈর্ঘ্য++;
    যদি(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    অন্যথা যদি(!ISALNUM(c) && c != '.')
      ফেরত CURLE_FTP_BAD_FILE_LIST;

    বিরতি;
  ক্ষেত্র PL_UNIX_TIME_PREPART2:
    parser->item_দৈর্ঘ্য++;
    যদি(c != ' ') {
      যদি(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      অন্যথা
        ফেরত CURLE_FTP_BAD_FILE_LIST;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_TIME_PART2:
    parser->item_দৈর্ঘ্য++;
    যদি(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    অন্যথা যদি(!ISALNUM(c) && c != '.')
      ফেরত CURLE_FTP_BAD_FILE_LIST;
    বিরতি;
  ক্ষেত্র PL_UNIX_TIME_PREPART3:
    parser->item_দৈর্ঘ্য++;
    যদি(c != ' ') {
      যদি(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      অন্যথা
        ফেরত CURLE_FTP_BAD_FILE_LIST;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_TIME_PART3:
    parser->item_দৈর্ঘ্য++;
    যদি(c == ' ') {
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
      parser->offsets.time = parser->item_offset;
      যদি(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.প্রধান = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      অন্যথা {
        parser->state.UNIX.প্রধান = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    অন্যথা যদি(!ISALNUM(c) && c != '.' && c != ':')
      ফেরত CURLE_FTP_BAD_FILE_LIST;
    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode parse_unix_filename(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো ftp_parselist_তথ্য *parser,
                                    কাঠামো fileinfo *infop,
                                    ধ্রুবক অক্ষর c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ফলাফল = CURLE_OK;

  নির্বাচন(parser->state.UNIX.sub.filename) {
  ক্ষেত্র PL_UNIX_FILENAME_PRESPACE:
    যদি(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_দৈর্ঘ্য = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_FILENAME_NAME:
    parser->item_দৈর্ঘ্য++;
    যদি(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    অন্যথা যদি(c == '\n') {
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.প্রধান = PL_UNIX_FILETYPE;
      ফলাফল = ftp_pl_ঢোকাও_finfo(তথ্য, infop);
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_FILENAME_WINDOWSEOL:
    যদি(c == '\n') {
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.প্রধান = PL_UNIX_FILETYPE;
      ফলাফল = ftp_pl_ঢোকাও_finfo(তথ্য, infop);
    }
    অন্যথা
      ফলাফল = CURLE_FTP_BAD_FILE_LIST;
    বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode parse_unix_symlink(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ftp_parselist_তথ্য *parser,
                                   কাঠামো fileinfo *infop,
                                   ধ্রুবক অক্ষর c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ফলাফল = CURLE_OK;

  নির্বাচন(parser->state.UNIX.sub.symlink) {
  ক্ষেত্র PL_UNIX_SYMLINK_PRESPACE:
    যদি(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_দৈর্ঘ্য = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_SYMLINK_NAME:
    parser->item_দৈর্ঘ্য++;
    যদি(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    অন্যথা যদি(c == '\r' || c == '\n')
      ফেরত CURLE_FTP_BAD_FILE_LIST;

    বিরতি;
  ক্ষেত্র PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_দৈর্ঘ্য++;
    যদি(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    অন্যথা যদি(c == '\r' || c == '\n')
      ফেরত CURLE_FTP_BAD_FILE_LIST;
    অন্যথা
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    বিরতি;
  ক্ষেত্র PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_দৈর্ঘ্য++;
    যদি(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    অন্যথা যদি(c == '\r' || c == '\n')
      ফেরত CURLE_FTP_BAD_FILE_LIST;
    অন্যথা
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    বিরতি;
  ক্ষেত্র PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_দৈর্ঘ্য++;
    যদি(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_দৈর্ঘ্য = 0;
      parser->item_offset = 0;
    }
    অন্যথা যদি(c == '\r' || c == '\n')
      ফেরত CURLE_FTP_BAD_FILE_LIST;
    অন্যথা
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    বিরতি;
  ক্ষেত্র PL_UNIX_SYMLINK_PRETARGET4:
    যদি(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_দৈর্ঘ্য = 1;
    }
    অন্যথা
      ফেরত CURLE_FTP_BAD_FILE_LIST;

    বিরতি;
  ক্ষেত্র PL_UNIX_SYMLINK_TARGET:
    parser->item_দৈর্ঘ্য++;
    যদি(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    অন্যথা যদি(c == '\n') {
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      ফলাফল = ftp_pl_ঢোকাও_finfo(তথ্য, infop);
      যদি(ফলাফল)
        বিরতি;

      parser->state.UNIX.প্রধান = PL_UNIX_FILETYPE;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_SYMLINK_WINDOWSEOL:
    যদি(c == '\n') {
      mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      ফলাফল = ftp_pl_ঢোকাও_finfo(তথ্য, infop);
      যদি(ফলাফল)
        বিরতি;

      parser->state.UNIX.প্রধান = PL_UNIX_FILETYPE;
    }
    অন্যথা
      ফলাফল = CURLE_FTP_BAD_FILE_LIST;

    বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode parse_unix(কাঠামো Curl_easy *তথ্য,
                           কাঠামো ftp_parselist_তথ্য *parser,
                           কাঠামো fileinfo *infop,
                           ধ্রুবক অক্ষর c)
{
  কাঠামো curl_fileinfo *finfo = &infop->info;
  CURLcode ফলাফল = CURLE_OK;

  নির্বাচন(parser->state.UNIX.প্রধান) {
  ক্ষেত্র PL_UNIX_TOTALSIZE:
    ফলাফল = parse_unix_totalsize(parser, infop, c);
    যদি(ফলাফল)
      বিরতি;
    যদি(parser->state.UNIX.প্রধান != PL_UNIX_FILETYPE)
      বিরতি;
    FALLTHROUGH();
  ক্ষেত্র PL_UNIX_FILETYPE:
    ফলাফল = unix_filetype(c, &finfo->filetype);
    যদি(!ফলাফল) {
      parser->state.UNIX.প্রধান = PL_UNIX_PERMISSION;
      parser->item_দৈর্ঘ্য = 0;
      parser->item_offset = 1;
    }
    বিরতি;
  ক্ষেত্র PL_UNIX_PERMISSION:
    ফলাফল = parse_unix_permission(parser, infop, c);
    বিরতি;
  ক্ষেত্র PL_UNIX_HLINKS:
    ফলাফল = parse_unix_hlinks(parser, infop, c);
    বিরতি;
  ক্ষেত্র PL_UNIX_USER:
    ফলাফল = parse_unix_user(parser, infop, c);
    বিরতি;
  ক্ষেত্র PL_UNIX_GROUP:
    ফলাফল = parse_unix_group(parser, infop, c);
    বিরতি;
  ক্ষেত্র PL_UNIX_আকার:
    ফলাফল = parse_unix_আকার(parser, infop, c);
    বিরতি;
  ক্ষেত্র PL_UNIX_TIME:
    ফলাফল = parse_unix_time(parser, infop, c);
    বিরতি;
  ক্ষেত্র PL_UNIX_FILENAME:
    ফলাফল = parse_unix_filename(তথ্য, parser, infop, c);
    বিরতি;
  ক্ষেত্র PL_UNIX_SYMLINK:
    ফলাফল = parse_unix_symlink(তথ্য, parser, infop, c);
    বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode parse_winnt(কাঠামো Curl_easy *তথ্য,
                            কাঠামো ftp_parselist_তথ্য *parser,
                            কাঠামো fileinfo *infop,
                            ধ্রুবক অক্ষর c)
{
  কাঠামো curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  অক্ষর *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ফলাফল = CURLE_OK;

  নির্বাচন(parser->state.NT.প্রধান) {
  ক্ষেত্র PL_WINNT_DATE:
    parser->item_দৈর্ঘ্য++;
    যদি(parser->item_দৈর্ঘ্য < 9) {
      যদি(!strchr("0123456789-", c)) { /* only simple control */
        ফেরত CURLE_FTP_BAD_FILE_LIST;
      }
    }
    অন্যথা যদি(parser->item_দৈর্ঘ্য == 9) {
      যদি(c == ' ') {
        parser->state.NT.প্রধান = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      অন্যথা
        ফেরত CURLE_FTP_BAD_FILE_LIST;
    }
    অন্যথা
      ফেরত CURLE_FTP_BAD_FILE_LIST;
    বিরতি;
  ক্ষেত্র PL_WINNT_TIME:
    parser->item_দৈর্ঘ্য++;
    নির্বাচন(parser->state.NT.sub.time) {
    ক্ষেত্র PL_WINNT_TIME_PRESPACE:
      যদি(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      বিরতি;
    ক্ষেত্র PL_WINNT_TIME_TIME:
      যদি(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
        parser->state.NT.প্রধান = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_দৈর্ঘ্য = 0;
      }
      অন্যথা যদি(!strchr("APM0123456789:", c))
        ফেরত CURLE_FTP_BAD_FILE_LIST;
      বিরতি;
    }
    বিরতি;
  ক্ষেত্র PL_WINNT_DIRORSIZE:
    নির্বাচন(parser->state.NT.sub.dirorsize) {
    ক্ষেত্র PL_WINNT_DIRORSIZE_PRESPACE:
      যদি(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_দৈর্ঘ্য = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      বিরতি;
    ক্ষেত্র PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_দৈর্ঘ্য++;
      যদি(c == ' ') {
        mem[parser->item_offset + parser->item_দৈর্ঘ্য - 1] = 0;
        যদি(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->আকার_2 = 0;
        }
        অন্যথা {
          ধ্রুবক অক্ষর *p = mem + parser->item_offset;
          যদি(curlx_str_numblanks(&p, &finfo->আকার_2)) {
            ফেরত CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_তথ্য->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_তথ্য->info.flags |= CURLFINFOFLAG_KNOWN_আকার;
        parser->item_দৈর্ঘ্য = 0;
        parser->state.NT.প্রধান = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      বিরতি;
    }
    বিরতি;
  ক্ষেত্র PL_WINNT_FILENAME:
    নির্বাচন(parser->state.NT.sub.filename) {
    ক্ষেত্র PL_WINNT_FILENAME_PRESPACE:
      যদি(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_দৈর্ঘ্য = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      বিরতি;
    ক্ষেত্র PL_WINNT_FILENAME_CONTENT:
      parser->item_দৈর্ঘ্য++;
      যদি(!len)
        ফেরত CURLE_FTP_BAD_FILE_LIST;
      যদি(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      অন্যথা যদি(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        ফলাফল = ftp_pl_ঢোকাও_finfo(তথ্য, infop);
        যদি(ফলাফল)
          ফেরত ফলাফল;

        parser->state.NT.প্রধান = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      বিরতি;
    ক্ষেত্র PL_WINNT_FILENAME_WINEOL:
      যদি(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        ফলাফল = ftp_pl_ঢোকাও_finfo(তথ্য, infop);
        যদি(ফলাফল)
          ফেরত ফলাফল;

        parser->state.NT.প্রধান = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      অন্যথা
        ফেরত CURLE_FTP_BAD_FILE_LIST;

      বিরতি;
    }
    বিরতি;
  }

  ফেরত CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(কাঠামো Curl_easy *তথ্য)
{
  যদি(তথ্য->state.wildcardmatch) {
    কাঠামো Wildcard_তথ্য * ধ্রুবক wildcard = তথ্য->wildcard;
    ফেরত wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  ফেরত FALSE;
}

size_t Curl_ftp_parselist(অক্ষর *বাফার, size_t আকার_2, size_t nmemb,
                          শূন্য *connptr)
{
  size_t bufflen = আকার_2 * nmemb;
  কাঠামো Curl_easy *তথ্য = (কাঠামো Curl_easy *)connptr;
  কাঠামো ftp_wc *ftpwc = তথ্য->wildcard->ftpwc;
  কাঠামো ftp_parselist_তথ্য *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode ফলাফল;
  size_t retsize = bufflen;

  যদি(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    যাও fail;
  }

  যদি(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(বাফার[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  যখন(i < bufflen) { /* FSM */
    অক্ষর c = বাফার[i];
    কাঠামো fileinfo *infop;
    যদি(!parser->file_তথ্য) { /* tmp file data is not allocated yet */
      parser->file_তথ্য = Curl_fileinfo_alloc();
      যদি(!parser->file_তথ্য) {
        parser->error = CURLE_OUT_OF_MEMORY;
        যাও fail;
      }
      parser->item_offset = 0;
      parser->item_দৈর্ঘ্য = 0;
      curlx_dyn_init(&parser->file_তথ্য->buf, MAX_FTPLIST_বাফার);
    }

    infop = parser->file_তথ্য;

    যদি(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      যাও fail;
    }

    নির্বাচন(parser->os_type) {
    ক্ষেত্র OS_TYPE_UNIX:
      ফলাফল = parse_unix(তথ্য, parser, infop, c);
      বিরতি;
    ক্ষেত্র OS_TYPE_WIN_NT:
      ফলাফল = parse_winnt(তথ্য, parser, infop, c);
      বিরতি;
    পূর্বনির্ধারিত:
      retsize = bufflen + 1;
      যাও fail;
    }
    যদি(ফলাফল) {
      parser->error = ফলাফল;
      যাও fail;
    }

    i++;
  }
  ফেরত retsize;

fail:

  /* Clean up any allocated memory. */
  যদি(parser->file_তথ্য) {
    Curl_fileinfo_cleanup(parser->file_তথ্য);
    parser->file_তথ্য = ফাঁকা;
  }

  ফেরত retsize;
}

#endif /* !CURL_DISABLE_FTP */
